<?php

use Westkingdom\HierarchicalGroupEmail\LegacyGroups;
use Westkingdom\HierarchicalGroupEmail\StandardGroupPolicy;

use Symfony\Component\Yaml\Yaml;
use Symfony\Component\Yaml\Dumper;

class LegacyGroupsTestCase extends PHPUnit_Framework_TestCase {

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

  public function testLegacyGroups() {
    $testData = "
west:
  lists:
    webminister:
      members:
        - minister@sca.org
        - deputy@sca.org
      properties:
        group-name: 'West Kingdom Web Minister'
        group-email: west-webminister@westkingdom.org
        alternate-addresses:
          - webminister@westkingdom.org";
    $testMemberships = Yaml::parse(trim($testData));

    // We are going to add two members to an existing group,
    // and create a new legacy group with two members.
    $testLegacy ="
west-webminister@westkingdom.org   anotherwebguy@hitec.com
webminister@westkingdom.org   deputywebdude@boilerstrap.com
webminister@mists.westkingdom.org   mistywebdude@fog.com
other@mists.westkingdom.org   othermistydude@fog.com
some-old-group@westkingdom.org   person1@somewhere.org,person2@somewhereelse.org";

    $properties = array(
      'top-level-group' => 'west',
      'subdomains' => 'mists'
    );
    $policy = new StandardGroupPolicy('westkingdom.org', $properties);
    $testMemberships = LegacyGroups::applyLegacyGroups($testMemberships, LegacyGroups::parseLegacyDreamHostGroups($testLegacy), $policy);

    $expected = "
west:
  lists:
    webminister:
      members:
        - minister@sca.org
        - deputy@sca.org
        - anotherwebguy@hitec.com
        - deputywebdude@boilerstrap.com
      properties:
        group-name: 'West Kingdom Web Minister'
        group-email: west-webminister@westkingdom.org
        alternate-addresses:
          - webminister@westkingdom.org
mists:
  lists:
    webminister:
      members:
        - mistywebdude@fog.com
      properties:
        group-name: Webminister
        alternate-addresses:
          - webminister@mists.westkingdom.org
_legacy:
  lists:
    mists:
      members:
        - mistywebdude@fog.com
        - othermistydude@fog.com
      properties:
        group-name: 'Mists Legacy Group Members'
        alternate-addresses:
          - legacy@mists.westkingdom.org
    mists-other:
      members:
        - othermistydude@fog.com
      properties:
        group-name: 'Mists other'
        alternate-addresses:
          - other@mists.westkingdom.org
    some-old-group:
      members:
        - person1@somewhere.org
        - person2@somewhereelse.org
      properties:
        group-name: 'Some old group'
        alternate-addresses:
          - some-old-group@westkingdom.org";

    $this->assertEquals(trim($expected), $this->arrayToYaml($testMemberships));

  }
}
