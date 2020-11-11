<?php

namespace Westkingdom\HierarchicalGroupEmail\Internal;

use Westkingdom\HierarchicalGroupEmail;

class Journal {
  protected $ctrl;
  protected $operationQueues;
  protected $existingState = array();
  protected $log;

  const SETUP_QUEUE = 'setup';
  const CREATION_QUEUE = 'create';
  const DEFAULT_QUEUE = 'default';
  const TEARDOWN_QUEUE = 'last';

  protected $queues = array(Journal::SETUP_QUEUE, Journal::CREATION_QUEUE, Journal::DEFAULT_QUEUE, Journal::TEARDOWN_QUEUE);

  function __construct($ctrl, $existingState) {
    $this->ctrl = $ctrl;
    $this->existingState = $existingState;
    $this->operationQueues = array();
    // TODO: recreate operationQueues from #queues, as appropriate
    // Need to decide on a strategy; try-and-abandon?
    if (isset($this->existingState['#queues'])) {
      $queues = $this->existingState['#queues'];
      unset($this->existingState['#queues']);
      $this->importOperationQueues($queues);
    }
    $this->log = function($line) { print($line); };
  }

  function log($line) {
    $log = $this->log;
    // $log($line);
  }

  function getExistingState() {
    $result = $this->existingState;
    $result = $this->alphebetizeLists($result);
    return $result;
  }

  function export() {
    $result = $this->getExistingState();
    $queues = $this->exportOperationQueues();
    if (!empty($queues)) {
      $result['#queues'] = $queues;
    }
    return $result;
  }

  protected function alphebetizeLists($lists) {
    $result = $lists;
    // Alphabetize all of our lists by key
    foreach ($result as $key => $data) {
      if ($key[0] != '#') {
        if (array_key_exists('lists', $data)) {
          ksort($result[$key]['lists']);
          foreach ($result[$key]['lists'] as $officename => $officedata) {
            if (array_key_exists('members', $officedata)) {
              sort($result[$key]['lists'][$officename]['members']);
            }
          }
        }
      }
    }
    return $result;
  }

  function exportOperationQueues() {
    $result = array();
    foreach ($this->queues as $queueName) {
      if (array_key_exists($queueName, $this->operationQueues)) {
        foreach ($this->operationQueues[$queueName] as $op) {
          $result[$queueName][] = $op->export();
        }
      }
    }
    return $result;
  }

  function importOperationQueues($queues) {
    foreach ($queues as $queueName => $operations) {
      foreach ($operations as $opData) {
        $op = Operation::import($this->ctrl, $opData);
        $this->queue($op, $queueName);
      }
    }
    $this->verify();
  }

  function hasOperation($other, $queueName = Journal::DEFAULT_QUEUE) {
    if (array_key_exists($queueName, $this->operationQueues)) {
      foreach ($this->operationQueues[$queueName] as $op) {
        if ($op->equals($other)) {
          return TRUE;
        }
      }
    }
    return FALSE;
  }

  function queue(Operation $op, $queueName = Journal::DEFAULT_QUEUE) {
    // TODO: validate that $queueName exists in QUEUES
    // Don't allow the same operations to pile up; we'll keep the
    // older one.
    if (!$this->hasOperation($op, $queueName)) {
      $this->operationQueues[$queueName][] = $op;
    }
  }

  // TODO: Operations should have a 'ready' test, and we should also
  // check the return value of 'run'.  We should return a list of all
  // operations that we ran without error.
  function executeQueue($queueName) {
    if (array_key_exists($queueName, $this->operationQueues)) {
      foreach ($this->operationQueues[$queueName] as $op) {
        // TODO: record the time we attempted to run $op
        $op->run();
      }
    }
  }

  // TODO: This should take the array of operations that was returned
  // from 'executeQueue', and only try to verify those.  Remove only
  // those items from the queue that verified.  The problem with this
  // idea is that we run in batch mode; we'd have to reassociate the
  // failure from the batch execute with the operation it belonged to.
  function verifyQueue($queueName) {
    $unfinished = array();
    if (array_key_exists($queueName, $this->operationQueues)) {
      foreach ($this->operationQueues[$queueName] as $op) {
        $verified = FALSE;
        if ($op->needsVerification()) {
          $verified = $op->verify();
        }
        // If the operation is verified, then run the 'verified'
        // method that matches the name of the run function for this
        // operation (with 'Verified' appended).
        if ($verified) {
          $verifiedMethodName = $op->getRunFunctionName() . 'Verified';
          if (method_exists($this, $verifiedMethodName)) {
            call_user_func_array(array($this, $verifiedMethodName), $op->getRunFunctionParametersForCall());
          }
        }
        else {
          $unfinished[] = $op;
        }
      }
    }
    // Remove finished operations from the queue
    $this->operationQueues[$queueName] = $unfinished;
  }

  function findOpById($key) {
    foreach ($this->queues as $queueName) {
      $op = $this->findOpByIdInQueue($key, $queueName);
      if ($op) {
        return $op;
      }
    }
    return FALSE;
  }

  function findOpByIdInQueue($key, $queueName) {
    if (array_key_exists($queueName, $this->operationQueues)) {
      foreach ($this->operationQueues[$queueName] as $op) {
        if ($op->compareId($key)) {
          return $op;
        }
      }
    }
    return FALSE;
  }

  function execute() {
    $executionResults = array();
    // Run all of the operations in all of the queues
    foreach ($this->queues as $queueName) {
      $this->ctrl->begin();
      $this->executeQueue($queueName);
      // TODO: can we associate any failures in $batchResult with the op that it belongs to?
      $batchResult = $this->ctrl->complete(TRUE);
      if (!empty($batchResult)) {
        foreach ($batchResult as $batchKey => $batchInfo) {
          $op = $this->findOpByIdInQueue($batchKey, $queueName);
          if ($op) {
            $op->recordExecutionResult($batchInfo);
          }
        }
      }
      $executionResults[$queueName] = $batchResult;
    }
    $this->verify();
    return $executionResults;
  }

  function verify() {
    // Verify each operation
    foreach ($this->queues as $queueName) {
      $this->verifyQueue($queueName);
    }
  }

  function begin() {
  }

  function insertBranch($branch) {
    $this->log("insert $branch\n");
    $op = new Operation(
      array($this->ctrl, "insertBranch"),
      array($branch),
      array($this->ctrl, "verifyBranch")
    );
    $this->queue($op, Journal::SETUP_QUEUE);
  }

  function insertBranchVerified($op, $branch) {
    $this->log("verified $branch");
    if (!array_key_exists($branch, $this->existingState)) {
      $this->existingState[$branch] = array();
    }
  }

  function deleteBranch($branch) {
    $this->log("delete $branch");
    $op = new Operation(
      array($this->ctrl, "deleteBranch"),
      array($branch)
    );
    $this->queue($op, Journal::TEARDOWN_QUEUE);
  }

  function deleteBranchVerified($op, $branch) {
    $this->log("verified delete $branch");
    unset($this->existingState[$branch]);
  }

  function insertMember($branch, $officename, $group_id, $memberEmailAddress) {
    $this->log("insert into $branch $officename ($group_id) $memberEmailAddress\n");
    $op = new Operation(
      array($this->ctrl, "insertMember"),
      array($branch, $officename, $group_id, $memberEmailAddress),
      array($this->ctrl, "verifyMember")
    );
    $this->queue($op);
  }

  function insertMemberVerified($op, $branch, $officename, $group_id, $memberEmailAddress) {
    $this->log("verified insert into $branch $officename ($group_id) $memberEmailAddress\n");
    // TODO: unique only. Should we sort as well?
    $this->existingState[$branch]['lists'][$officename]['members'][] = $memberEmailAddress;
  }

  function removeMember($branch, $officename, $group_id, $memberEmailAddress) {
    $this->log("remove from $branch $officename ($group_id) $memberEmailAddress\n");
    $op = new Operation(
      array($this->ctrl, "removeMember"),
      array($branch, $officename, $group_id, $memberEmailAddress)
    );
    $this->queue($op);
  }

  function removeMemberVerified($op, $branch, $officename, $group_id, $memberEmailAddress) {
    $this->log("verified remove from $branch $officename ($group_id) $memberEmailAddress\n");
    $this->existingState[$branch]['lists'][$officename]['members'] = array_unique(array_diff(
      $this->existingState[$branch]['lists'][$officename]['members'],
      array($memberEmailAddress)));
  }

  function insertGroupAlternateAddress($branch, $officename, $group_id, $alternateAddress) {
    $this->log("alternate address for $branch $officename ($group_id): $alternateAddress\n");
    $op = new Operation(
      array($this->ctrl, "insertGroupAlternateAddress"),
      array($branch, $officename, $group_id, $alternateAddress),
      array($this->ctrl, "verifyGroupAlternateAddress")
    );
    $this->queue($op);
  }

  function insertGroupAlternateAddressVerified($op, $branch, $officename, $group_id, $alternateAddress) {
    $this->log("verified alternate address for $branch $officename ($group_id): $alternateAddress\n");
    // TODO: unique only. Should we sort as well?
    $this->existingState[$branch]['lists'][$officename]['properties']['alternate-addresses'][] = $alternateAddress;
  }

  function removeGroupAlternateAddress($branch, $officename, $group_id, $alternateAddress) {
    $this->log("remove alternate address for $branch $officename ($group_id): $alternateAddress\n");
    $op = new Operation(
      array($this->ctrl, "removeGroupAlternateAddress"),
      array($branch, $officename, $group_id, $alternateAddress)
    );
    $this->queue($op);
  }

  function removeGroupAlternateAddressVerified($op, $branch, $officename, $group_id, $alternateAddress) {
    $this->log("verified remove alternate address for $branch $officename ($group_id): $alternateAddress\n");
    $this->existingState[$branch]['lists'][$officename]['properties']['alternate-addresses'] = array_unique(array_diff(
      $this->existingState[$branch]['lists'][$officename]['properties']['alternate-addresses'],
      array($alternateAddress)));
  }

  function insertOffice($branch, $officename, $properties) {
    $this->log("insert $branch $officename\n");
    $op = new Operation(
      array($this->ctrl, "insertOffice"),
      array($branch, $officename, $properties),
      array($this->ctrl, "verifyOffice")
    );
    $this->queue($op, Journal::CREATION_QUEUE);
    $op = new Operation(
      array($this->ctrl, "configureOffice"),
      array($branch, $officename, $properties),
      array($this->ctrl, "verifyOfficeConfiguration")
    );
    $this->queue($op);
  }

  function insertOfficeVerified($op, $branch, $officename, $properties) {
    $this->log("verified insert $branch $officename\n");
    foreach (array('group-name', 'group-email', 'group-id') as $key) {
      if (array_key_exists($key, $properties)) {
        $this->existingState[$branch]['lists'][$officename]['properties'][$key] = $properties[$key];
      }
    }
  }

  function configureOfficeVerified($op, $branch, $officename, $properties) {
    // TODO: At the moment, all configuration is hardcoded.  When that changes,
    // we will need to update our state here as well.
  }

  function deleteOffice($branch, $officename, $properties) {
    $this->log("delete $branch $officename\n");
    $op = new Operation(
      array($this->ctrl, "deleteOffice"),
      array($branch, $officename, $properties)
    );
    $this->queue($op);
  }

  function deleteOfficeVerified($op, $branch, $officename, $properties) {
    $this->log("verified delete $branch $officename\n");
    unset($this->existingState[$branch]['lists'][$officename]);
  }

  function complete() {
  }
}

