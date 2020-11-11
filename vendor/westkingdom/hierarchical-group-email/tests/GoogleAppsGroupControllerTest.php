<?php

use Westkingdom\HierarchicalGroupEmail\GoogleAppsGroupsController;
use Westkingdom\HierarchicalGroupEmail\GroupPolicy;
use Westkingdom\HierarchicalGroupEmail\BatchWrapper;
use Westkingdom\HierarchicalGroupEmail\Internal\Operation;

use Symfony\Component\Yaml\Dumper;

class GoogleAppsGroupsControllerTestCase extends PHPUnit_Framework_TestCase {

  protected $client;

  public function setUp() {
    parent::setup();

    // Create a new client, set it to use batch mode, but do not authenticate
    $this->client = new Google_Client();
    $this->client->setApplicationName("Google Apps Groups Controller Test");
    $this->client->setUseBatch(true);
  }

  function testGroupsController() {
    // Use a batch standin, which acts like a Google_Http_Batch, but
    // merely accumulates the requests added to it, and returns them
    // to us when requested.
    $batch = new BatchWrapper();

    // Create a new Google Apps group controller, and add some users
    // and groups to it.
    $controller = new GoogleAppsGroupsController($this->client, $batch);

    $presidentProperties = array(
      'group-email' => 'north-president@testdomain.org',
      'group-id' => 'north-president@testdomain.org',
      'group-name' => 'North President',
    );
    $vicePresidentProperties = array(
      'group-email' => 'north-vicepresident@testdomain.org',
      'group-id' => 'north-vicepresident@testdomain.org',
      'group-name' => 'North Vice-President',
    );

    $op = new Operation('placeholderOperation', array());

    $controller->insertOffice($op, 'north', 'president', $presidentProperties);
    $controller->configureOffice($op, 'north', 'president', $presidentProperties);
    $controller->insertMember($op, 'north', 'president', $presidentProperties['group-id'], 'franklin@testdomain.org');
    $controller->insertGroupAlternateAddress($op, 'north', 'president', $presidentProperties['group-id'], 'elpresidente@testdomain.org');
    $controller->removeMember($op, 'north', 'president', $presidentProperties['group-id'], 'franklin@testdomain.org');
    $controller->removeGroupAlternateAddress($op, 'north', 'president', $presidentProperties['group-id'], 'elpresidente@testdomain.org');
    $controller->insertMember($op, 'north', 'vice-president', $vicePresidentProperties['group-id'], 'garner@testdomain.org');
    $controller->removeMember($op, 'north', 'vice-president', $vicePresidentProperties['group-id'], 'garner@testdomain.org');
    $controller->deleteOffice($op, 'north', 'president', $presidentProperties);

    // The expected list of requests corresponding to the calls above:
    $expected = <<< EOT
-
  requestMethod: POST
  url: /admin/directory/v1/groups
  email: north-president@testdomain.org
  name: 'North President'
-
  requestMethod: PATCH
  url: /groups/v1/groups/north-president%40testdomain.org
  isArchived: true
  whoCanJoin: INVITED_CAN_JOIN
  whoCanPostMessage: ANYONE_CAN_POST
-
  requestMethod: POST
  url: /admin/directory/v1/groups/north-president%40testdomain.org/members
  email: franklin@testdomain.org
  role: MEMBER
  type: USER
-
  requestMethod: POST
  url: /admin/directory/v1/groups/north-president%40testdomain.org/aliases
  alias: elpresidente@testdomain.org
-
  requestMethod: DELETE
  url: /admin/directory/v1/groups/north-president%40testdomain.org/members/franklin%40testdomain.org
-
  requestMethod: DELETE
  url: /admin/directory/v1/groups/north-president%40testdomain.org/aliases/elpresidente%40testdomain.org
-
  requestMethod: POST
  url: /admin/directory/v1/groups/north-vicepresident%40testdomain.org/members
  email: garner@testdomain.org
  role: MEMBER
  type: USER
-
  requestMethod: DELETE
  url: /admin/directory/v1/groups/north-vicepresident%40testdomain.org/members/garner%40testdomain.org
-
  requestMethod: DELETE
  url: /admin/directory/v1/groups/north-president%40testdomain.org
EOT;

    $requests = $batch->getSimplifiedRequests();
    $this->assertEquals(trim($expected), $this->arrayToYaml($requests));
  }

  public function arrayToYaml($data) {
    // Convert data to YAML
    $dumper = new Dumper();
    $dumper->setIndentation(2);
    return trim($dumper->dump($data, PHP_INT_MAX));
  }
}
