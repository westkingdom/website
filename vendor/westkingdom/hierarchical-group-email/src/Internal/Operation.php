<?php

namespace Westkingdom\HierarchicalGroupEmail\Internal;

class Operation {
  protected $runFunction;
  protected $runParameters;
  protected $verifyFunction;
  protected $verifyParameters;
  protected $operationId;
  protected $operationSequence;
  protected $executed;
  protected $failedVerification;
  protected $batchResult;

  function __construct($runFn, $runParams, $verifyFn = NULL, $verifyParams = NULL) {
    $this->runFunction = $runFn;
    $this->runParameters = $runParams;
    $this->verifyFunction = $verifyFn;
    $this->verifyParameters = $verifyParams;

    $this->operationId = mt_rand();
    $this->operationSequence = 0;
    $this->executed = FALSE;
    $this->failedVerification = FALSE;
    $this->batchResult = FALSE;
  }

  static function import($ctrl, $data) {
    $runFn = array($ctrl, $data['run-function']);
    $runParams = $data['run-params'];
    $verifyFn = NULL;
    $verifyParams = NULL;
    if (array_key_exists('verify-function', $data)) {
      $verifyFn = array($ctrl, $data['verify-function']);
      if (array_key_exists('verify-params', $data)) {
        $verifyParams['verify-params'] = $data['verify-params'];
      }
    }
    $op = new Operation($runFn, $runParams, $verifyFn, $verifyParams);
    $op->setImportData($data);
    return $op;
  }

  protected function setImportData($data) {
    if (array_key_exists('state', $data)) {
      $state = $data['state'];
      unset($state['runFunction']);
      unset($state['verifyFunction']);
      foreach ($state as $key => $value) {
        $this->$key = $value;
      }
    }
    // TODO: import any batch error data that was exported.
  }

  function export() {
    $result = array();

    $result['run-function'] = $this->getRunFunctionName();
    $result['run-params'] = $this->runParameters;
    if ($this->verifyFunction) {
      $result['verify-function'] = $this->verifyFunction[1];
      if ($this->verifyParameters) {
        $result['verify-params'] = $this->verifyParameters;
      }
    }
    foreach (array('executed', 'failedVerification') as $key) {
      if (!empty($this->$key)) {
        $result['state'][$key] = $this->$key;
      }
    }
    // TODO: prune any irrelevant parts of the batch errors
    if (is_array($this->batchResult) && !empty($this->batchResult)) {
      $result['error'] = $this->batchResult;
    }
    // TODO: if batchResult is TRUE, then this op was executed
    // at least once without an error.  Maybe we should export this
    // state as well?
    return $result;
  }

  // Return the reason this call failed, or FALSE if it did not fail.
  function errorReason() {
    if (is_array($this->batchResult) && array_key_exists('reason', $this->batchResult)) {
      $this->batchResult['reason'];
    }
    return FALSE;
  }

  function equals(Operation $other) {
    // Must have the same run function to be the same
    if ($this->getRunFunctionName() != $other->getRunFunctionName()) {
      return FALSE;
    }
    // Compare the parameters too
    $thisParameters = $this->getRunFunctionParameters();
    $otherParameters = $other->getRunFunctionParameters();
    foreach ($thisParameters as $param) {
      $otherParam = array_shift($otherParameters);
      if (is_array($param)) {
        if ((!is_array($otherParam)) || (count(array_intersect_assoc($param, $otherParam)) != count($param))) {
          return FALSE;
        }
      }
      elseif ($param != $otherParam) {
        return FALSE;
      }
    }
    return TRUE;
  }

  /**
   * Return a seqence number consisting of the operation id
   * for this operation followed by ":" and a sequence number.
   * Example: 414530998:1
   */
  function nextSequenceNumber() {
    $this->operationSequence++;
    return $this->operationId . ":" . $this->operationSequence;
  }

  /**
   * Check to see if a batch response id matches this operation.
   * Batch response ids are made by appending the operation sequence
   * number to "response-", so we strip off everything up to the
   * first "-", and everything after the ":", and see if the remainder
   * matches our operation id.
   */
  function compareId($checkId) {
    $checkId = preg_replace('/.*-|:.*/','', $checkId);
    return $checkId == $this->operationId;
  }

  function recordExecutionResult($batchResult) {
    // The Google API returns a 'null' record when everything is okay
    if (empty($batchResult)) {
      $this->executed = TRUE;
      $this->batchResult = TRUE;
      // If simulated, don't run the verify function -- just claim the operation worked.
      if ($batchResult === FALSE) {
        $this->verifyFunction = NULL;
      }
    }
    else {
      $this->batchResult = $batchResult;
    }
  }

  function needsVerification() {
    // We'd better verify all the time until we understand the different states an op can be in better.
    return TRUE; // $this->executed;
  }

  function getRunFunction() {
    return $runFunction;
  }

  function getRunFunctionName() {
    if (is_string($this->runFunction)) {
      return $this->runFunction;
    }
    else {
      return $this->runFunction[1];
    }
  }

  function getRunFunctionParameters() {
    return $this->runParameters;
  }

  function getRunFunctionParametersForCall() {
    $result = $this->runParameters;
    array_unshift($result, $this);
    return $result;
  }

  function setRunFunctionParameters($parameters) {
    $this->runParameters = $parameters;
  }

  function getVerifyFunctionParameters() {
    return $this->verifyParameters ?: $this->runParameters;
  }

  function getVerifyFunctionParametersForCall() {
    $result = $this->getVerifyFunctionParameters();
    array_unshift($result, $this);
    return $result;
  }

  /**
   * Do the operation
   */
  function run() {
    return call_user_func_array($this->runFunction, $this->getRunFunctionParametersForCall());
  }

  /**
   * Check to see if the operation succeeded
   *
   * @return TRUE if done, any other value if it needs to be retried later.
   */
  function verify() {
    if (!$this->verifyFunction) {
      return TRUE;
    }
    $result = call_user_func_array($this->verifyFunction, $this->getVerifyFunctionParametersForCall());
    if (!$result) {
      $this->failedVerification = TRUE;
    }
    return $result;
  }
}
