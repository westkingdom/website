<?php

namespace Westkingdom\HierarchicalGroupEmail;

/**
 * A class that can be used like a Google_Http_Batch
 * object with a GoogleAppsGroupController object.
 * It can either be used by itelf, or it can wrap a
 * real Google_Http_Batch, provided in the constructor.
 *
 * It accumulates all of the requests added to it, and
 * will later return a simplified list of requests for
 * use in test assertions, logs, confirmation messages,
 * etc.
 */
class BatchWrapper
{
  /** @var array service requests to be executed. */
  private $requests = array();
  /** @var batch object to pass requests to */
  private $batch;

  public function __construct($batch = NULL)
  {
    $this->batch = $batch;
  }

  /**
   * Stash our requests, just like Google_Http_Batch does.
   */
  public function add(\Google_Http_Request $request, $key = false)
  {
    if (false == $key) {
      $key = mt_rand();
    }

    $this->requests[$key] = $request;
    if (isset($this->batch)) {
      $this->batch->add($request, $key);
    }
  }

  public function execute()
  {
    $result = array();

    if (isset($this->batch)) {
      $result = $this->batch->execute();
    }
    else {
      foreach ($requests as $key => $req) {
        // The Google API uses NULL to indicate "OK"; we will use
        // FALSE to indicate "simulated".
        $result[$key] = FALSE;
      }
    }
    $this->requests = array();
    return $result;
  }

  /**
   * Return all of the stashed requests.
   */
  public function getRequests() {
    return $this->requests;
  }

  /**
   * Return only the 'url' and 'postBody' of each request.
   * Everything is merged together for compactness.
   * Useful for logging, confirming, testing, etc.
   */
  public function getSimplifiedRequests($keys = array('requestMethod', 'url', 'queryParams', 'postBody')) {
    $result = array();
    foreach ($this->requests as $request) {
      $request->setBaseComponent('');
      $item = array();
      foreach ($keys as $key) {
        $methodName = 'get' . ucfirst($key);
        $value = $request->{$methodName}();
        if (null != $value) {
          if ($key == 'postBody') {
            $value = (array)json_decode($value);
            $item = array_merge($item, $value);
          }
          else {
            $item[$key] = $value;
          }
        }
      }
      $result[] = $item;
    }
    return $result;
  }
}
