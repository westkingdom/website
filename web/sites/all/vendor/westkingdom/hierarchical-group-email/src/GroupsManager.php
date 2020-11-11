<?php

namespace Westkingdom\HierarchicalGroupEmail;

use Westkingdom\HierarchicalGroupEmail\Internal\Journal;

class GroupsManager {
  protected $ctrl;
  protected $policy;
  protected $journal;

  /**
   * @param $ctrl control object that does actual actions
   * @param $policy policy object that decides how groups are named, etc.
   * @param $state initial state
   * @param $journal internal object, provided during testing only.
   */
  function __construct(GroupsController $ctrl, $policy, $state, $journal = NULL) {
    $this->ctrl = $ctrl;
    $this->policy = $policy;
    if (!isset($state['_aggregated']['lists'])) {
      $state['_aggregated']['lists'] = array();
    }
    if ($journal == NULL) {
      $this->journal = new Journal($ctrl, $state);
    }
    else {
      $this->journal = $journal;
    }
  }

  static function createForDomain($applicationName, $domain, $state) {
    $authenticator = ServiceAccountAuthenticator($applicationName);
    $client = $authenticator->authenticate();
    $policy = new StandardGroupPolicy($domain);
    $controller = new GoogleAppsGroupsController($client);
    $groupManager = new GroupsManager($controller, $policy, $state);
    return $groupManager;
  }

  function normalize($memberships) {
    return $this->policy->normalize($memberships);
  }

  /**
   * Update our group memberships
   *
   *
   * @param $memberships nested associative array
   *    BRANCHES contain LISTS and ALIASES.
   *
   *      branchname => array(lists => ..., aliases=> ...)
   *
   *      LISTS contain groups.
   *
   *        lists => array(group1 => ..., group2 => ...)
   *
   *          Groups can be one of three formats:
   *
   *            string - a group with one member; data is email address
   *
   *            simple array of strings - email addresses of members
   *
   *            associative array - element 'members' contains email addresses of members
   *
   *      ALIASES are structured just like groups.
   *
   *    The difference between a LIST and an ALIAS is that a list is
   *    expected to keep an archive of all email that is sent to it,
   *    and an alias just passes the email through.
   */
  function update($memberships) {
    $existingState = $this->journal->getExistingState();
    if (isset($memberships['_aggregated'])) {
      unset($memberships['_aggregated']);
    }
    if (isset($existingState['_aggregated'])) {
      unset($existingState['_aggregated']);
    }
    $memberships = $this->policy->normalize($memberships);
    $this->journal->begin();

    foreach ($memberships as $branch => $officesLists) {
      if ($branch[0] != '#') {
        // print ">>> Update branch $branch\n";
        $offices = $officesLists['lists'];
        // Next, update or insert, depending on whether this branch is new.
        if (array_key_exists($branch, $existingState)) {
          $this->updateBranch($branch, $offices);
        }
        else {
          $this->insertBranch($branch, $offices);
        }
      }
    }
    // Finally, delete any branch that is no longer with us.
    foreach ($existingState as $branch => $offices) {
      if ($branch[0] != '#') {
        if (!array_key_exists($branch, $memberships)) {
          // print "<<< Delete branch $branch\n";
          $this->deleteBranch($branch, $offices);
        }
      }
    }
    $this->journal->complete();
  }

  protected function updateAggregated() {
    $existingState = $this->journal->getExistingState();
    unset($existingState['_aggregated']);
    $existingState = $this->policy->generateParentage($existingState);
    $aggregated = $this->policy->generateAggregatedGroups($existingState);
    $masterDirectory = $this->policy->generateMasterDirectory($existingState);
    $this->policy->removeDuplicateAlternates($aggregated, $masterDirectory);
    $this->updateBranch('_aggregated', $aggregated);
  }

  function execute() {
    $result = $this->journal->execute();
    $this->updateAggregated();
    $secondaryResult = $this->journal->execute();
    return array_merge($result, $secondaryResult);
  }

  function getExistingState() {
    return $this->journal->getExistingState();
  }

  function export() {
    return $this->journal->export();
  }

  function updateBranch($branch, $updateOffices) {
    $existingState = $this->journal->getExistingState();
    $existingOffices = isset($existingState[$branch]['lists']) ? $existingState[$branch]['lists'] : array();
    return $this->updateAlteredBranch($branch, $updateOffices, $existingOffices);
  }

  function updateAlteredBranch($branch, $updateOffices, $existingOffices) {
    foreach ($updateOffices as $officename => $officeData) {
      if (array_key_exists($officename, $existingOffices)) {
        $this->updateOffice($branch, $officename, $officeData);
      }
      else {
        $this->insertOffice($branch, $officename, $officeData);
      }
    }
    foreach ($existingOffices as $officename => $officeData) {
      if (!array_key_exists($officename, $updateOffices)) {
        $this->deleteOffice($branch, $officename, $officeData);
      }
    }
  }

  function insertBranch($branch, $newOffices) {
    $this->journal->insertBranch($branch);
    $this->updateAlteredBranch($branch, $newOffices, array());
  }

  function deleteBranch($branch, $removingOffices) {
    $this->journal->deleteBranch($branch);
  }

  function updateOffice($branch, $officename, $officeData) {
    $existingState = $this->journal->getExistingState();
    $existingOffices = $existingState[$branch]['lists'];
    if (!isset($existingOffices[$officename])) {
      $existingOffices[$officename] = array();
    }
    $existingOffices[$officename] += array('members' => array());
    $this->updateOfficeMembers($branch, $officename, $officeData['properties']['group-id'], $officeData['members'], $existingOffices[$officename]['members']);
    $newAlternateAddresses = GroupsManager::getAlternateAddresses($branch, $officename, $officeData);
    $existingAlternateAddresses = GroupsManager::getAlternateAddresses($branch, $officename, $existingOffices[$officename]);
    $this->updateOfficeAlternateAddresses($branch, $officename, $officeData['properties']['group-id'], $newAlternateAddresses, $existingAlternateAddresses);
  }

  function updateOfficeMembers($branch, $officename, $groupId, $updateMembers, $existingMembers) {
    foreach ($updateMembers as $emailAddress) {
      if (!in_array($emailAddress, $existingMembers)) {
        // print "    +++ $officename: $emailAddress\n";
        $this->journal->insertMember($branch, $officename, $groupId, $emailAddress);
      }
    }
    foreach ($existingMembers as $emailAddress) {
      if (!in_array($emailAddress, $updateMembers)) {
        // print "    --- $officename: $emailAddress [DELETE]\n";
        $this->journal->removeMember($branch, $officename, $groupId, $emailAddress);
      }
    }
  }

  function updateOfficeAlternateAddresses($branch, $officename, $groupId, $newAlternateAddresses, $existingAlternateAddresses) {
    foreach ($newAlternateAddresses as $emailAddress) {
      if (!in_array($emailAddress, $existingAlternateAddresses)) {
        $this->journal->insertGroupAlternateAddress($branch, $officename, $groupId, $emailAddress);
      }
    }
    foreach ($existingAlternateAddresses as $emailAddress) {
      if (!in_array($emailAddress, $newAlternateAddresses)) {
        // print "    ::: remove group alternate address: $officename: $emailAddress\n";
        $this->journal->removeGroupAlternateAddress($branch, $officename, $groupId, $emailAddress);
      }
    }
  }

  function insertOffice($branch, $officename, $officeData) {
    $this->journal->insertOffice($branch, $officename, $officeData['properties']);
    $this->updateOfficeMembers($branch, $officename, $officeData['properties']['group-id'], $officeData['members'], array());
    $newAlternateAddresses = GroupsManager::getAlternateAddresses($branch, $officename, $officeData['properties']['group-id'], $officeData);
    $this->updateOfficeAlternateAddresses($branch, $officename, $officeData['properties']['group-id'], $newAlternateAddresses, array());
  }

  function deleteOffice($branch, $officename, $officeData) {
    $this->journal->deleteOffice($branch, $officename, isset($officeData['properties']) ? $officeData['properties'] : array());
  }

  static function getAlternateAddresses($branch, $officename, $officeData) {
    $result = array();
    if (isset($officeData['properties']['alternate-addresses'])) {
      $result = (array)$officeData['properties']['alternate-addresses'];
    }
    return $result;
  }
}
