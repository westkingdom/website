<?php

namespace Westkingdom\HierarchicalGroupEmail;

use Symfony\Component\Yaml\Yaml;

/**
 * In order to use the Google API, it is necessary to create
 * a client object and authenticate it, either using the
 * user's credentials (by redirecting to a Google login
 * page, and capturing the token returned when Google redirects
 * back to us), or use a service account.
 *
 * When using a service account, it is undesirable to hard-code
 * the secrets needed to log in.  Instead, we want to be able
 * to load them from files or a database.
 *
 * This class supports authenticating service accounts using
 * information stored in .yml files.
 */
class ServiceAccountAuthenticator {
  /** @var list of directories to search for service account information */
  protected $searchpath;
  /** @var name of application, to provide to Google_Client */
  protected $applicationName;

  /** list of elements in the service account info that are paths to files */
  protected $serviceAccountFileElements = array(
    'key-file'
  );

  /**
   * Create a new Service Account Authenticator, specifying
   * the search path to use.  The default search path is:
   *
   *    - $HOME/.google-api
   *    - /etc/google-api.
   */
  function __construct($applicationName, $searchpath = array()) {
    $this->applicationName = $applicationName;
    $this->searchpath = $searchpath;

    if (empty($searchpath)) {
      $this->searchpath[] = '.google-api';
      $this->searchpath[] = '/etc/google-api';
    }
  }

  /**
   * Load a specific service account file, and authenticate with it.
   *
   * @param $serviceAccount the name of the service account credentials file to load
   * @param $scopes the Google Apps scopes needed by this application
   * @param $serviceToken any token that may have been previously generated
   *
   * @returns Google_Client $client
   */
  function authenticate($serviceAccount = 'service-account.yaml', $scopes = array(), $serviceToken = NULL) {
    // Create a new google client.  We need this for all API access.
    $client = new \Google_Client();
    $client->setApplicationName($this->applicationName);

    // Look up our service account, if we can find it.
    $service_account_info = array();
    $home = $_SERVER['HOME'];
    foreach ($this->searchpath as $searchdir) {
      if ($searchdir[0] != '/') {
        $searchdir = "$home/$searchdir";
      }
      if (substr($searchdir, -1) != "/") {
        $searchdir .= '/';
      }
      $service_account_path = $searchdir . $serviceAccount;
      if (file_exists($service_account_path)) {
        $service_account_info = Yaml::parse($service_account_path);
        // Iterate over all of the items in the service account info
        // that are filepaths.  Convert any that are relative paths
        // into full paths, relative to the service account info file.
        foreach ($this->serviceAccountFileElements as $filepath_item) {
          if (isset($service_account_info[$filepath_item]) && $service_account_info[$filepath_item][0] != '/') {
            $orig = $service_account_info[$filepath_item];
            $service_account_info[$filepath_item] = dirname($service_account_path) . '/' . basename($service_account_info[$filepath_item]);
          }
        }
      }
    }

    // If we have an API key, that will give us a certain amount of access.
    if (isset($service_account_info['api-key'])) {
      $client->setDeveloperKey($service_account_info['api-key']);
    }

    // If we have a service account, that will give us even more access.
    // TODO: report if the service account info has some of these keys,
    // but not all?
    if ($this->hasKeys($service_account_info, array('client-id', 'email-address', 'key-file', 'delegate-user-email'))) {
      $client_id = $service_account_info['client-id'];
      $service_account_name = $service_account_info['email-address'];
      $key_file_location = $service_account_info['key-file'];
      $key_file_password = $service_account_info['key-file-password'];

      if (isset($service_account_info['scopes'])) {
        // If the service account info lists the scopes that it provides, and
        // the program provides a list of scopes that it requires, then
        // we want to insure that all of the requried scopes are provided.
        if (!empty($scopes)) {
          // TODO: report and fail if $requiredScopesNotProvided is not empty.
          // We probably don't need to calculate or report this, as the
          // Google_Auth_AssertionCredentials constructor will probably do
          // that for us.
          $requiredScopesNotProvided = array_diff($scopes, $service_account_info['scopes']);
        }
        else {
          $scopes = $service_account_info['scopes'];
        }
      }

      if (!isset($serviceToken) && isset($_SESSION['service_token'])) {
        $serviceToken = $_SESSION['service_token'];
      }
      if (isset($serviceToken)) {
        $client->setAccessToken($serviceToken);
      }
      $key = FALSE;
      if (file_exists($key_file_location)) {
        $key = file_get_contents($key_file_location);
      }

      $cred = new \Google_Auth_AssertionCredentials(
        $service_account_name,
        $scopes,
        $key,
        $key_file_password
      );

      //
      // Very important step:  the service account must also declare the
      // identity (via email address) of a user with admin priviledges that
      // it would like to masquerade as.  If we do not do this, then we
      // get no priviledges.
      //
      // See:  http://stackoverflow.com/questions/22772725/trouble-making-authenticated-calls-to-google-api-via-oauth
      //
      $cred->sub = $service_account_info['delegate-user-email'];
      $client->setAssertionCredentials($cred);
      if ($client->getAuth()->isAccessTokenExpired()) {
        $client->getAuth()->refreshTokenWithAssertion($cred);
      }
      $_SESSION['service_token'] = $client->getAccessToken();
    }
    return $client;
  }

  /**
   * Confirm that the provided array contains all of the
   * keys specified in the key list.
   */
  protected function hasKeys($info, $keyList) {
    foreach ($keyList as $key) {
      if (!array_key_exists($key, $info)) {
        return FALSE;
      }
    }
    return TRUE;
  }
}
