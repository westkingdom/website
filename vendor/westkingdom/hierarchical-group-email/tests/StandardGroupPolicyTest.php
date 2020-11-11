<?php

use Westkingdom\HierarchicalGroupEmail\StandardGroupPolicy;
use Prophecy\PhpUnit\ProphecyTestCase;

class StandardGroupPolicyTestCase extends ProphecyTestCase {

  public function setUp() {
    parent::setup();
  }

  public function testStandardPolicy() {
    $policy = new StandardGroupPolicy('mydomain.org');

    $this->assertTrue(in_array('domain', $policy->availableDefaults()));
    $this->assertEquals('mydomain.org', $policy->getProperty('domain'));
    $this->assertEquals('north-president@mydomain.org', $policy->getGroupId('north', 'president'));
    $this->assertEquals('north-president@mydomain.org', $policy->getGroupEmail('north', 'president'));
    $this->assertEquals('North President', $policy->getGroupName('north', 'president'));
  }
}
