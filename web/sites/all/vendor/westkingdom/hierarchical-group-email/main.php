<?php

//
// This is sample code.  Read it and modify to suit; it is not useful
// to run as-is, at the moment.
//

include dirname(__FILE__) . "/vendor/autoload.php";

use Symfony\Component\Yaml\Yaml;
use Symfony\Component\Yaml\Dumper;
use GoogleAPIExtensions\Groups;
use Westkingdom\HierarchicalGroupEmail\ServiceAccountAuthenticator;
use Westkingdom\HierarchicalGroupEmail\StandardGroupPolicy;
use Westkingdom\HierarchicalGroupEmail\GoogleAppsGroupsController;
use Westkingdom\HierarchicalGroupEmail\BatchWrapper;


$scopes = array(
    // Books is only for testing.  The rest I think we actually need.
    Google_Service_Books::BOOKS,

    Google_Service_Groupssettings::APPS_GROUPS_SETTINGS,

    Google_Service_Directory::ADMIN_DIRECTORY_GROUP,
    Google_Service_Directory::ADMIN_DIRECTORY_GROUP_READONLY,

    Google_Service_Directory::ADMIN_DIRECTORY_GROUP_MEMBER,
    Google_Service_Directory::ADMIN_DIRECTORY_GROUP_MEMBER_READONLY,

    Google_Service_Directory::ADMIN_DIRECTORY_NOTIFICATIONS,

    Google_Service_Directory::ADMIN_DIRECTORY_ORGUNIT,
    Google_Service_Directory::ADMIN_DIRECTORY_ORGUNIT_READONLY,

    Google_Service_Directory::ADMIN_DIRECTORY_USER,
    Google_Service_Directory::ADMIN_DIRECTORY_USER_READONLY,

    Google_Service_Directory::ADMIN_DIRECTORY_USER_ALIAS,
    Google_Service_Directory::ADMIN_DIRECTORY_USER_ALIAS_READONLY,

    Google_Service_Directory::ADMIN_DIRECTORY_USER_SECURITY,

    Google_Service_Directory::ADMIN_DIRECTORY_USERSCHEMA,
    Google_Service_Directory::ADMIN_DIRECTORY_USERSCHEMA_READONLY,

    Google_Service_Calendar::CALENDAR,
    Google_Service_Calendar::CALENDAR_READONLY,
  );



// Load our YAML data
$groupData = file_get_contents(dirname(__FILE__) . "/server.westkingdom.org.yaml");
$newState = Yaml::parse($groupData);

print "about to authenticate\n";

$authenticator = new ServiceAccountAuthenticator("Google Group API Test");
$client = $authenticator->authenticate('service-account.yaml', $scopes);

if (empty($client->getAccessToken())) {
  print "Could not authenticate.\n";
  exit(1);
}

print "authenticated\n";

/*
// Get info about a user

$service = new Google_Service_Directory($client);
$data = $service->users->get("gregor.eilburg@westkingdom.org");
var_export($data);
print("\n");
exit(0);
*/


$policy = new StandardGroupPolicy('westkingdom.org');
$batch = new \Google_Http_Batch($client);
$batchWrapper = new BatchWrapper($batch);
$controller = new GoogleAppsGroupsController($client, $batchWrapper);

if (!file_exists('currentstate.westkingdom.org.yaml')) {
  print "about to fetch\n";

  $existing = $controller->fetch('westkingdom.org');
  $dumper = new Dumper();
  $dumper->setIndentation(2);
  $existingAsYaml = trim($dumper->dump($existing, PHP_INT_MAX));

  file_put_contents('currentstate.westkingdom.org.yaml', $existingAsYaml);

  exit(0);
}

$groupData = file_get_contents(dirname(__FILE__) . "/currentstate.westkingdom.org.yaml");
$currentState = Yaml::parse($groupData);

$groupManager = new Westkingdom\HierarchicalGroupEmail\GroupsManager($controller, $policy, $currentState);
$groupManager->update($newState);

$dumper = new Dumper();
$dumper->setIndentation(2);

/*
$pendingOperations = $batchWrapper->getSimplifiedRequests();
$pendingAsYaml = trim($dumper->dump($pendingOperations, PHP_INT_MAX));
print($pendingAsYaml);
*/

//exit(0);

$groupManager->execute();

$updatedState = $groupManager->export();
$updatedStateAsYaml = trim($dumper->dump($updatedState, PHP_INT_MAX));
print($updatedStateAsYaml);
file_put_contents('currentstate.westkingdom.org.yaml', $updatedStateAsYaml);

exit(0);



// Actually do the update
$batchWrapper->execute();


exit(0);

// Test the Google Books API


print("Get books service.\n");

$service = new Google_Service_Books($client);

print("Call books service:\n");

$optParams = array('filter' => 'free-ebooks');
$results = $service->volumes->listVolumes('Henry David Thoreau', $optParams);
echo "### Results Of Call:\n";
foreach ($results as $item) {
  echo $item['volumeInfo']['title'], "\n";
}

print("Get directory service.\n");

$service = new Google_Service_Directory($client);

print("Call directory service:\n");

// Get info about a group
$data = $service->groups->get($group_email);
var_export($data);
print("\n");

// Add a user
$user = new Google_Service_Directory_User(array(
    'name' => array(
      'familyName' => 'Firstname',
      'givenName' => 'Lastname',
    ),
    'primaryEmail' => "first.last@$domain",
    'password' => sha1('secretsecret'),
    'hashfunction' => 'SHA-1',
  ));

$service->users->insert($user);


// Get info about a user

$data = $service->users->get("first.last@$domain");
var_export($data);
print("\n");


/*
  public $addresses;
  public $agreedToTerms;
  public $aliases;
  public $changePasswordAtNextLogin;
  public $creationTime;
  public $customSchemas;
  public $customerId;
  public $deletionTime;
  public $emails;
  public $etag;
  public $externalIds;
  public $hashFunction;
  public $id;
  public $ims;
  public $includeInGlobalAddressList;
  public $ipWhitelisted;
  public $isAdmin;
  public $isDelegatedAdmin;
  public $isMailboxSetup;
  public $kind;
  public $lastLoginTime;
  protected $nameType = 'Google_Service_Directory_UserName';
  protected $nameDataType = '';
  public $nonEditableAliases;
  public $orgUnitPath;
  public $organizations;
  public $password;
  public $phones;
  public $primaryEmail;
  public $relations;
  public $suspended;
  public $suspensionReason;
  public $thumbnailPhotoUrl;
*/




// Get members of a group
$data = $service->members->listMembers($group_email);
var_export($data);
print("\n");


// List all the groups
$opt = array('domain' => "$domain");
$data = $service->groups->listGroups($opt);
var_export($data);
print("\n");



$service = new Google_Service_Groupssettings($client);

$settingData = new Google_Service_Groupssettings_Groups();

// Some API calls require that we request that the returned data be
// sent as JSON.  The PHP API for the Google Apps API only works with
// JSON, but some calls default to returning XML.
$opt_params = array(
  'alt' => "json"
);
$data = $service->groups->get($group_email, $opt_params);

var_export($data);
print("\n");

// INVITED_CAN_JOIN or CAN_REQUEST_TO_JOIN, etc.
$settingData->setWhoCanJoin("CAN_REQUEST_TO_JOIN");
// ALL_MANAGERS_CAN_POST, ALL_IN_DOMAIN_CAN_POST, works
// ANYONE_CAN_POST returns 'permission denied'.
$settingData->setWhoCanPostMessage("ANYONE_CAN_POST");

$data = $service->groups->patch($group_email, $settingData);

