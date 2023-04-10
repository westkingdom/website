<?php

namespace Westkingdom\HierarchicalGroupEmail;

use Westkingdom\HierarchicalGroupEmail\Internal\Operation;

/**
 * Use this groups controller with Westkingdom\HierarchicalGroupEmail\Groups
 * to update groups and group memberships in Google Apps.
 *
 * Batch mode is always used.  You may provide your own batch object,
 * in which case you should call $client->setUseBatch(true) and
 * $batch->execute() yourself.  If you do not provide a batch object,
 * then one will be created in the constructor for you, and its
 * execute() method will be called at the end of the update.
 */
class GoogleAppsGroupsController implements GroupsController {
  protected $client;
  protected $batch;
  protected $directoryService;
  protected $groupSettingsService;
  protected $autoExecute = FALSE;

  /**
   * @param $client Google Apps API client object
   * @param $batch Google Apps batch object. Optional; one will be created
   * if none provided.
   */
  function __construct($client, $batch = NULL) {
    $this->client = $client;
    $this->batch = $batch;
    if (!isset($batch)) {
      $this->batch = new \Google_Http_Batch($client);
      $this->autoExecute = TRUE;
    }
    $this->directoryService = new \Google_Service_Directory($client);
    $this->groupSettingsService = new \Google_Service_Groupssettings($client);
  }

  /**
   * Fetch and export memberships using the Google API.
   */
  function fetch($domain) {
    $result = array();

    // Fetch all the groups in this domain
    $opt = array('domain' => $domain);
    $data = $this->directoryService->groups->listGroups($opt);

    $groups = $data->getGroups();

    while (!empty($groups) ) {
      print "Fetched " . count($groups) . " groups this iteration.\n";

      // Iterate over the groups, and fill in info about each.
      foreach ($groups as $groupInfo) {
        $email = $groupInfo->getEmail();
        $emailParts = explode('@', $email);
        $addressParts = explode('-', $emailParts[0]);
        $office = array_pop($addressParts);
        $branch = implode('-', $addressParts);
        $members = array();
        print ">>> Fetch info about group $email\n";
        $membersData = $this->directoryService->members->listMembers($email);
        $membersList = $membersData->getMembers();
        foreach ($membersList as $memberInfo) {
          $members[] = strtolower($memberInfo->getEmail());
        }
        print "    " . implode(',', $members) . "\n";
        $properties['group-name'] = $groupInfo->getName();
        $properties['group-id'] = $groupInfo->getId();
        $results[$branch]['lists'][$office] = array(
          'members' => $members,
          'properties' => $properties,
        );
        sleep(1);
      }

      $nextPageToken = $data->getNextPageToken();
      print "=============================================================\n";
      print "Next page token is: $nextPageToken\n";
      print "=============================================================\n";
      if (!empty($nextPageToken)) {
        $opt['pageToken'] = $nextPageToken;
        $data = $this->directoryService->groups->listGroups($opt);
        $groups = $data->getGroups();
      }
      else {
        $groups = [];
      }
    }

    return $results;
  }

  function insertBranch(Operation $op, $branch) {
    // no-op; we create groups for offices in a branch, but presently
    // we have no Google object that we create for branches.
  }

  function deleteBranch(Operation $op, $branch) {
    // no-op; @see insertBranch.
  }

  function verifyBranch(Operation $op, $branch) {
    // no-op; @see insertBranch.
    return TRUE;
  }

  function insertMember(Operation $op, $branch, $officename, $group_id, $memberEmailAddress) {
    $member = new \Google_Service_Directory_Member(array(
                            'email' => $memberEmailAddress,
                            'role'  => 'MEMBER',
                            'type'  => 'USER'));

    $req = $this->directoryService->members->insert($group_id, $member);
    $this->batch->add($req, $op->nextSequenceNumber());
  }

  function removeMember(Operation $op, $branch, $officename, $group_id, $memberEmailAddress) {
    $req = $this->directoryService->members->delete($group_id, $memberEmailAddress);
    $this->batch->add($req, $op->nextSequenceNumber());
  }

  // TODO: it is inefficient to verify group memberships one user at a time.
  function verifyMember(Operation $op, $branch, $officename, $group_id, $memberEmailAddress) {
    // If we got a "duplicate" error, then we'll count this as verified
    // without doing an extra call.
    // TODO:  Should we return FALSE without checking when errorReason() is some other value?
    if ($op->errorReason() == "duplicate") {
      return TRUE;
    }
    try {
      $data = $this->directoryService->members->listMembers($group_id);
    }
    catch (\Exception $e) {
      return FALSE;
    }
    // TODO: check $memberEmailAddress against returned data
    return TRUE;
  }

  function insertOffice(Operation $op, $branch, $officename, $properties) {
    if (!array_key_exists('group-email', $properties)) {
      print("\nIn insertOffice: missing 'group-email' for $branch $officename\n");
      var_export($properties);
      print("\n");
    }
    else {
      $group_email = $properties['group-email'];
      $group_name = $properties['group-name'];

      $newgroup = new \Google_Service_Directory_Group(array(
        'email' => "$group_email",
        'name' => "$group_name",
      ));

      $req = $this->directoryService->groups->insert($newgroup);
      $this->batch->add($req, $op->nextSequenceNumber());
    }
  }

  function configureOffice(Operation $op, $branch, $officename, $properties) {
    if (!array_key_exists('group-id', $properties)) {
      print("\nIn configureOffice: missing 'group-id' for $branch $officename\n");
      var_export($properties);
      print("\n");
    }
    else {
      $groupId = $properties['group-id'];
      $settingData = new \Google_Service_Groupssettings_Groups();

      // TODO: pull settings from properties

      // INVITED_CAN_JOIN or CAN_REQUEST_TO_JOIN, etc.
      $settingData->setWhoCanJoin("INVITED_CAN_JOIN");
      // ALL_MANAGERS_CAN_POST, ALL_IN_DOMAIN_CAN_POST,
      // ANYONE_CAN_POST, etc.
      $settingData->setWhoCanPostMessage("ANYONE_CAN_POST");

      // By default, we will archive emails in all groups except for the
      // aggregated groups, because it is assumed that these forward to
      // groups that are archived.
      $defaultIsArchived = ($branch != '_aggregated');
      $settingData->setIsArchived(array_key_exists('archived', $properties) ? $properties['archived'] : $defaultIsArchived);

      $req = $this->groupSettingsService->groups->patch($groupId, $settingData);
      $this->batch->add($req, $op->nextSequenceNumber());

      if (isset($properties['alternate-addresses'])) {
        foreach ($properties['alternate-addresses'] as $alternate_address) {
          $newalias = new \Google_Service_Directory_Alias(array(
            'alias' => $alternate_address,
          ));
          $req = $this->directoryService->groups_aliases->insert($groupId, $newalias);
          $this->batch->add($req, $op->nextSequenceNumber());
        }
      }
    }
  }

  function deleteOffice(Operation $op, $branch, $officename, $properties) {
    $groupId = $properties['group-id'];
    $req = $this->directoryService->groups->delete($groupId);
    $this->batch->add($req, $op->nextSequenceNumber());
  }

  function verifyOffice(Operation $op, $branch, $officename, $properties) {
    if ($op->errorReason() == "duplicate") {
      return TRUE;
    }
    $groupId = $properties['group-id'];
    try {
      $data = $this->directoryService->groups->get($groupId);
      // TODO: maybe return FALSE if $properties has a group-email or group-name that is different (changing office info)?
      // No, we probably want to use a different operation if we are updating rather than creating the office.
      $newProperties['group-email'] = $data['email'];
      $newProperties['group-id'] = $data['id'];
      $newProperties['group-name'] = $data['name'];
      // Fill in the group id etc. provided to us.
      $parameters = array($branch, $officename, $newProperties);
      $op->setRunFunctionParameters($parameters);
    }
    catch (\Exception $e) {
      return FALSE;
    }
    return TRUE;
  }

  function verifyOfficeConfiguration(Operation $op, $branch, $officename, $properties) {
    $groupId = $properties['group-id'];
    $opt_params = array(
      'alt' => "json"
    );
    try {
      $settingData = $this->groupSettingsService->groups->get($groupId, $opt_params);
    }
    catch (\Exception $e) {
      return FALSE;
    }

    if (($settingData->getWhoCanJoin() != "INVITED_CAN_JOIN") ||
        ($settingData->getWhoCanPostMessage() != "ANYONE_CAN_POST")) {
      return FALSE;
    }

    return TRUE;
  }

  function insertGroupAlternateAddress(Operation $op, $branch, $officename, $group_id, $alternateAddress) {
    $newAlternateAddress = new \Google_Service_Directory_Alias(array(
      'alias' => $alternateAddress,
      ));
    $req = $this->directoryService->groups_aliases->insert($group_id, $newAlternateAddress);
    $this->batch->add($req, $op->nextSequenceNumber());
  }

  function removeGroupAlternateAddress(Operation $op, $branch, $officename, $group_id, $alternateAddress) {
    // n.b. inserting an alias also adds a non-editable alias, but deleting
    // an alias does not delete its non-editable counterpart.
    $req = $this->directoryService->groups_aliases->delete($group_id, $alternateAddress);
    $this->batch->add($req, $op->nextSequenceNumber());
  }

  // TODO: it is inefficient to verify alternate addresses one at a time.
  function verifyGroupAlternateAddress(Operation $op, $branch, $officename, $group_id, $alternateAddress) {
    if ($op->errorReason() == "duplicate") {
      return TRUE;
    }
    try {
      $aliasData = $this->directoryService->groups_aliases->listGroupsAliases($group_id);
    }
    catch (\Exception $e) {
      return FALSE;
    }
    // TODO: check $aliasData against $alternateAddress
    return TRUE;
  }

  function begin() {
    $this->client->setUseBatch(TRUE);
  }

  function complete($execute = NULL) {
    $result = array();
    $this->client->setUseBatch(FALSE);
    if (!isset($execute)) {
      $execute = $this->autoExecute;
    }
    if ($execute) {
      $result = $this->execute();
      if (method_exists($result, "getErrors")) {
        $result = $result->getErrors();
      }
    }
    return $result;
  }

  function execute() {
    return $this->batch->execute();
  }
}
