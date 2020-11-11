<?php

use Westkingdom\HierarchicalGroupEmail\GroupsManager;
use Westkingdom\HierarchicalGroupEmail\StandardGroupPolicy;
use Westkingdom\HierarchicalGroupEmail\Internal\Journal;

use Prophecy\PhpUnit\ProphecyTestCase;
use Prophecy\Argument\Token\AnyValueToken;
use Symfony\Component\Yaml\Yaml;
use Symfony\Component\Yaml\Dumper;

class GroupsManagerTestCase extends ProphecyTestCase {

  protected $initialState = array();
  protected $policy;

  public function setUp() {
    parent::setup();

    $groupData = "
west:
  lists:
    webminister:
      members:
        - minister@sca.org
        - deputy@sca.org
      properties:
        group-name: West Kingdom Web Minister";
    $this->initialState = Yaml::parse(trim($groupData));

    $properties = array(
      'top-level-group' => 'north',
      'subdomains' => 'fogs,geese,wolves,lightwoods',
    );
    $this->policy = new StandardGroupPolicy('testdomain.org', $properties);
  }

  public function assertYamlEquals($expected, $data) {
    $this->assertEquals($this->arrayToYaml($expected), $this->arrayToYaml($data));
  }

  public function arrayToYaml($data) {
    if (is_string($data)) {
      return trim($data);
    }
    else {
      // Convert data to YAML
      $dumper = new Dumper();
      $dumper->setIndentation(2);
      return trim($dumper->dump($data, PHP_INT_MAX));
    }
  }

  public function testLoadingOfTestData() {
    // Do a nominal test to check to see that our test data loaded
    $this->assertEquals('west', implode(',', array_keys($this->initialState)));
  }

  public function testImportOperations() {
    // Test importing of queues.  The two identical items should
    // be noticed, and the second discarded.
    $initial = Yaml::parse(trim("
'#queues':
  create:
    -
      run-function: insertOffice
      run-params:
        - _aggregated
        - all-rapiermarshals
        -
          group-id: all-rapiermarshals@westkingdom.org
          group-name: 'All Rapier-marshals'
          group-email: all-rapiermarshals@westkingdom.org
      verify-function: verifyOffice
    -
      run-function: insertOffice
      run-params:
        - _aggregated
        - all-rapiermarshals
        -
          group-id: all-rapiermarshals@westkingdom.org
          group-name: 'All Rapier-marshals'
          group-email: all-rapiermarshals@westkingdom.org
      verify-function: verifyOffice"));

    $testController = $this->prophesize('Westkingdom\HierarchicalGroupEmail\GroupsController');
    $groupManager = new GroupsManager($testController->reveal(), $this->policy, $initial);

    $exported = $groupManager->export();

    // Note: we fail verification here because we verify all ops on import,
    // and we have mocked the group controller, but not its verify function,
    // so it cannot verify.
    $expected = "
create:
  -
    run-function: insertOffice
    run-params:
      - _aggregated
      - all-rapiermarshals
      -
        group-id: all-rapiermarshals@westkingdom.org
        group-name: 'All Rapier-marshals'
        group-email: all-rapiermarshals@westkingdom.org
    verify-function: verifyOffice
    state:
      failedVerification: true";

    $this->assertEquals(trim($expected), $this->arrayToYaml($exported['#queues']));
  }

  public function testInsertMember() {
    // Create a new state object by copying our existing state and adding
    // a member to the "west-webminister" group.
    $newState = $this->initialState;
    $newState['west']['lists']['webminister']['members'][] = 'new.admin@somewhere.com';
    $newState['west']['lists']['webminister']['properties']['alternate-addresses'] = 'webminister@westkingdom.org';

    // Create a new test controller prophecy, and reveal it to the
    // Groups object we are going to test.
    $testController = $this->prophesize('Westkingdom\HierarchicalGroupEmail\GroupsController');
    $groupManager = new GroupsManager($testController->reveal(), $this->policy, $this->initialState);

    // Prophesize that the new user will be added to the west webministers group.
    $testController->begin()->shouldBeCalled();
    $testController->insertMember()->shouldBeCalled()->withArguments(array(new AnyValueToken(), "west", "webminister", "west-webminister@testdomain.org", "new.admin@somewhere.com"));
    $testController->insertGroupAlternateAddress()->shouldBeCalled()->withArguments(array(new AnyValueToken(), "west", "webminister", "west-webminister@testdomain.org", "webminister@westkingdom.org"));
    $testController->verifyMember()->shouldBeCalled()->withArguments(array(new AnyValueToken(), "west", "webminister", "west-webminister@testdomain.org", "new.admin@somewhere.com"));
    $testController->verifyGroupAlternateAddress()->shouldBeCalled()->withArguments(array(new AnyValueToken(), "west", "webminister", "west-webminister@testdomain.org", "webminister@westkingdom.org"));
    $testController->complete()->shouldBeCalled()->withArguments(array(TRUE));

    // Update the group.  The prophecies are checked against actual
    // behavior during teardown.
    $groupManager->update($newState);
    $groupManager->execute();

    // Note: we fail verification because we have mocked the group
    // controller, but not its verify function, so it cannot verify.
    $expectedFinalState = "
west:
  lists:
    webminister:
      members:
        - deputy@sca.org
        - minister@sca.org
      properties:
        group-name: 'West Kingdom Web Minister'
_aggregated:
  lists: {  }
'#queues':
  default:
    -
      run-function: insertMember
      run-params:
        - west
        - webminister
        - west-webminister@testdomain.org
        - new.admin@somewhere.com
      verify-function: verifyMember
      state:
        failedVerification: true
    -
      run-function: insertGroupAlternateAddress
      run-params:
        - west
        - webminister
        - west-webminister@testdomain.org
        - webminister@westkingdom.org
      verify-function: verifyGroupAlternateAddress
      state:
        failedVerification: true";

    $state = $groupManager->export();
    $this->assertEquals(trim($expectedFinalState), $this->arrayToYaml($state));
  }

  public function testRemoveMember() {
    // Create a new state object by copying our existing state and adding
    // a member to the "west-webminister" group.
    $newState = $this->initialState;
    $removed = array_pop($newState['west']['lists']['webminister']['members']);

    // Create a new test controller prophecy, and reveal it to the
    // Groups object we are going to test.
    $testController = $this->prophesize('Westkingdom\HierarchicalGroupEmail\GroupsController');
    $revealedController = $testController->reveal();
    $journal = new Journal($revealedController, $this->initialState);
    $groupManager = new GroupsManager($revealedController, $this->policy, $this->initialState, $journal);

    // Prophesize that a user will be removed from the west webministers group,
    // and then removed again
    $testController->begin()->shouldBeCalled();
    $testController->removeMember()->shouldBeCalled()->withArguments(array(new AnyValueToken(), "west", "webminister", "west-webminister@testdomain.org", $removed));
    $testController->complete()->shouldBeCalled()->withArguments(array(TRUE));

    // Update the group.  The prophecies are checked against actual
    // behavior during teardown.
    $groupManager->update($newState);
    $groupManager->execute();

    // Again, we have mocked the group controller, so verification is not done.
    // If the controller did verify, then it would call the following function
    $journal->removeMemberVerified(NULL, "west", "webminister", "west-webminister@testdomain.org", $removed);

    $expectedFinalState = "
west:
  lists:
    webminister:
      members:
        - minister@sca.org
      properties:
        group-name: 'West Kingdom Web Minister'";

    $state = $groupManager->export();
    unset($state['#queues']);
    $this->assertEquals(trim($expectedFinalState), $this->arrayToYaml($state));

  }

  public function testInsertOffice() {
    // Create a new state object by copying our existing state and adding
    // a member to the "west-webminister" group.
    $newState = $this->initialState;
    $newState['west']['lists']['seneschal']['members'][] = 'anne@kingdom.org';
    $newState['west']['lists']['seneschal']['properties']['group-name'] = 'West Seneschal';

    // Create a new test controller prophecy, and reveal it to the
    // Groups object we are going to test.
    $testController = $this->prophesize('Westkingdom\HierarchicalGroupEmail\GroupsController');
    $revealedController = $testController->reveal();
    $journal = new Journal($revealedController, $this->initialState);
    $groupManager = new GroupsManager($revealedController, $this->policy, $this->initialState, $journal);

    // Prophesize that the new user will be added to the west webministers group.
    $testController->begin()->shouldBeCalled();

    $testController->insertOffice()->shouldBeCalled()->withArguments(array(new AnyValueToken(), "west", "seneschal", array("group-name" => "West Seneschal", "group-email" => "west-seneschal@testdomain.org", "group-id" => "west-seneschal@testdomain.org")));
    $testController->configureOffice()->shouldBeCalled()->withArguments(array(new AnyValueToken(), "west", "seneschal", array("group-name" => "West Seneschal", "group-email" => "west-seneschal@testdomain.org", "group-id" => "west-seneschal@testdomain.org")));
    $testController->verifyOffice()->shouldBeCalled()->withArguments(array(new AnyValueToken(), "west", "seneschal", array("group-name" => "West Seneschal", "group-email" => "west-seneschal@testdomain.org", "group-id" => "west-seneschal@testdomain.org")));
    $testController->verifyOfficeConfiguration()->shouldBeCalled()->withArguments(array(new AnyValueToken(), "west", "seneschal", array("group-name" => "West Seneschal", "group-email" => "west-seneschal@testdomain.org", "group-id" => "west-seneschal@testdomain.org")));
    $testController->insertMember()->shouldBeCalled()->withArguments(array(new AnyValueToken(), "west", "seneschal", "west-seneschal@testdomain.org", "anne@kingdom.org"));
    $testController->verifyMember()->shouldBeCalled()->withArguments(array(new AnyValueToken(), "west", "seneschal", "west-seneschal@testdomain.org", "anne@kingdom.org"));
    $testController->insertOffice()->shouldBeCalled()->withArguments(array(new AnyValueToken(), "_aggregated", "west-officers", array("group-name" => "West Officers", "group-email" => "west-officers@testdomain.org", "group-id" => "west-officers@testdomain.org", 'alternate-addresses' => array('officers@west.testdomain.org'))));
    $testController->configureOffice()->shouldBeCalled()->withArguments(array(new AnyValueToken(), "_aggregated", "west-officers", array("group-name" => "West Officers", "group-email" => "west-officers@testdomain.org", "group-id" => "west-officers@testdomain.org", 'alternate-addresses' => array('officers@west.testdomain.org'))));
    $testController->verifyOfficeConfiguration()->shouldBeCalled()->withArguments(array(new AnyValueToken(), "_aggregated", "west-officers", array("group-name" => "West Officers", "group-email" => "west-officers@testdomain.org", "group-id" => "west-officers@testdomain.org", 'alternate-addresses' => array('officers@west.testdomain.org'))));
    $testController->verifyOffice()->shouldBeCalled()->withArguments(array(new AnyValueToken(), "_aggregated", "west-officers", array("group-name" => "West Officers", "group-email" => "west-officers@testdomain.org", "group-id" => "west-officers@testdomain.org", 'alternate-addresses' => array('officers@west.testdomain.org'))));
    $testController->insertMember()->shouldBeCalled()->withArguments(array(new AnyValueToken(), "_aggregated", "west-officers", "west-officers@testdomain.org", "west-webminister@testdomain.org"));
    $testController->insertMember()->shouldBeCalled()->withArguments(array(new AnyValueToken(), "_aggregated", "west-officers", "west-officers@testdomain.org", "west-seneschal@testdomain.org"));
    $testController->verifyMember()->shouldBeCalled()->withArguments(array(new AnyValueToken(), "_aggregated", "west-officers", "west-officers@testdomain.org", "west-webminister@testdomain.org"));
    $testController->verifyMember()->shouldBeCalled()->withArguments(array(new AnyValueToken(), "_aggregated", "west-officers", "west-officers@testdomain.org", "west-seneschal@testdomain.org"));

//    $testController->insertOffice()->shouldBeCalled()->withArguments(array(new AnyValueToken(), "_aggregated", "all-seneschals", array("group-id" => "all-seneschals@testdomain.org", "group-name" => "All Seneschals", "group-email" => "all-seneschals@testdomain.org")));
//    $testController->configureOffice()->shouldBeCalled()->withArguments(array(new AnyValueToken(), "_aggregated", "all-seneschals", array("group-id" => "all-seneschals@testdomain.org", "group-name" => "All Seneschals", "group-email" => "all-seneschals@testdomain.org")));
//    $testController->verifyOffice()->shouldBeCalled()->withArguments(array(new AnyValueToken(), "_aggregated", "all-seneschals", array("group-id" => "all-seneschals@testdomain.org", "group-name" => "All Seneschals", "group-email" => "all-seneschals@testdomain.org")));
//    $testController->verifyOfficeConfiguration()->shouldBeCalled()->withArguments(array(new AnyValueToken(), "_aggregated", "all-seneschals", array("group-id" => "all-seneschals@testdomain.org", "group-name" => "All Seneschals", "group-email" => "all-seneschals@testdomain.org")));
//    $testController->insertMember()->shouldBeCalled()->withArguments(array(new AnyValueToken(), "_aggregated", "all-seneschals", "all-seneschals@testdomain.org", "west-seneschal@testdomain.org"));
//    $testController->verifyMember()->shouldBeCalled()->withArguments(array(new AnyValueToken(), "_aggregated", "all-seneschals", "all-seneschals@testdomain.org", "west-seneschal@testdomain.org"));

//    $testController->insertMember()->shouldBeCalled()->withArguments(array(new AnyValueToken(), "_aggregated", "west-officers", "west-officers@testdomain.org", "west-seneschal@testdomain.org"));
//    $testController->verifyMember()->shouldBeCalled()->withArguments(array(new AnyValueToken(), "_aggregated", "west-officers", "west-officers@testdomain.org", "west-seneschal@testdomain.org"));

    $testController->complete()->shouldBeCalled()->withArguments(array(TRUE));

    // Update the group.  The prophecies are checked against actual
    // behavior during teardown.
    $groupManager->update($newState);
    $groupManager->execute();

    // Force the call to insertOfficeVerified, so we can test the generation
    // of aggregated groups.
    $journal->insertOfficeVerified(array(), 'west', 'seneschal', $newState['west']['lists']['seneschal']['properties']);

    // Call 'execute' again, to insure that updateAggregated() is called.
    $groupManager->execute();

    // We don't see the aggregated group here, because the verify functions
    // are never called (due to the mocked controller), so the verified()
    // functions are never called, and these are what update the state.
    $expectedFinalState = "
west:
  lists:
    seneschal:
      properties:
        group-name: 'West Seneschal'
    webminister:
      members:
        - deputy@sca.org
        - minister@sca.org
      properties:
        group-name: 'West Kingdom Web Minister'";

    $state = $groupManager->export();
    unset($state['#queues']);
    $this->assertEquals(trim($expectedFinalState), $this->arrayToYaml($state));

  }
}
