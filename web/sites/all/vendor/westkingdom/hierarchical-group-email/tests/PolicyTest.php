<?php

use Westkingdom\HierarchicalGroupEmail\StandardGroupPolicy;

use Prophecy\PhpUnit\ProphecyTestCase;
use Prophecy\Argument\Token\AnyValueToken;
use Symfony\Component\Yaml\Yaml;
use Symfony\Component\Yaml\Dumper;

class PolicyTestCase extends ProphecyTestCase {

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
      'primary-office' => 'president',
      'subdomains' => 'fogs,geese,wolves,lightwoods',
      'aggregated-groups' => "
lesseroffices:
  members:
    - vice-president
    - secretary
    - gophers
  properties:
    group-name: '\${branch} Lesser Officers'",
    );
    $this->policy = new StandardGroupPolicy('testdomain.org', $properties);
  }

  public function testNormalize() {
    $data = Yaml::parse("
north:
  lists:
    president:
      - bill@testdomain.org
    vice-president:
      members:
        - walter@testdomain.org
      properties:
        alternate-addresses:
          - vice@testdomain.org
          - vicepresidents
    secretary:
      - george@testdomain.org
fogs:
  lists:
    president:
      - frank@testdomain.org");

    $normalized = $this->policy->normalize($data);

    $expected = "
north:
  lists:
    president:
      members:
        - bill@testdomain.org
      properties:
        group-email: north-president@testdomain.org
        group-id: north-president@testdomain.org
        group-name: 'North President'
        alternate-addresses:
          - president@testdomain.org
    vice-president:
      members:
        - walter@testdomain.org
      properties:
        alternate-addresses:
          - vice@testdomain.org
          - vicepresident@testdomain.org
          - vicepresidents@testdomain.org
        group-email: north-vicepresident@testdomain.org
        group-id: north-vicepresident@testdomain.org
        group-name: 'North Vice-president'
    secretary:
      members:
        - george@testdomain.org
      properties:
        group-email: north-secretary@testdomain.org
        group-id: north-secretary@testdomain.org
        group-name: 'North Secretary'
        alternate-addresses:
          - secretary@testdomain.org
fogs:
  lists:
    president:
      members:
        - frank@testdomain.org
      properties:
        group-email: fogs-president@testdomain.org
        group-id: fogs-president@testdomain.org
        group-name: 'Fogs President'
        alternate-addresses:
          - president@fogs.testdomain.org";

    $this->assertYamlEquals(trim($expected), $normalized);
  }


  public function testParentage() {
    $data = Yaml::parse("
north:
  subgroups:
    - fogs
    - geese
    - wolves
fogs:
  subgroups:
    - lightwoods
geese:
  subgroups:
    - gustyplains
wolves:
  subgroups:
    - coldholm
lightwoods:
  subgroups:
    - seamountain
gustyplains:
  subgroups: {  }
coldholm:
  subgroups: {  }
seamountain:
  subgroups: {  }");

    $result = $this->policy->generateParentage($data);

    $expected = "
north:
  subgroups:
    - fogs
    - geese
    - wolves
fogs:
  subgroups:
    - lightwoods
geese:
  subgroups:
    - gustyplains
wolves:
  subgroups:
    - coldholm
lightwoods:
  subgroups:
    - seamountain
  parentage:
    - fogs
gustyplains:
  subgroups: {  }
  parentage:
    - geese
coldholm:
  subgroups: {  }
  parentage:
    - wolves
seamountain:
  subgroups: {  }
  parentage:
    - lightwoods
    - fogs";

    $this->assertYamlEquals(trim($expected), $result);
  }


  public function testAggregatedSubgroups() {
    $this->assertTrue($this->policy->isSubdomain('fogs'));
    $this->assertTrue($this->policy->isSubdomain('wolves'));

    $this->assertYamlEquals("- president@fogs.testdomain.org", $this->policy->getGroupDefaultAlternateAddresses('fogs', 'president'));

    $data = Yaml::parse("
north:
  lists:
    president:
      members:
        - bill@testdomain.org
  subgroups:
    - fogs
    - geese
    - wolves
fogs:
  lists:
    president:
      members:
        - ron@testdomain.org
  subgroups:
    - lightwoods
geese:
  lists:
    president:
      members:
        - george@testdomain.org
  subgroups:
    - gustyplains
wolves:
  lists:
    president:
      members:
        - frank@testdomain.org
  subgroups:
    - coldholm
lightwoods:
  lists:
    president:
      members:
        - alex@testdomain.org
  subgroups:
    - seamountain
gustyplains:
  lists:
    president:
      members:
        - tom@testdomain.org
  subgroups: {  }
coldholm:
  lists:
    president:
      members:
        - richard@testdomain.org
  subgroups: {  }
seamountain:
  lists:
    president:
      members:
        - gerald@testdomain.org
  subgroups: {  }");

    $data = $this->policy->normalize($data);

    // Strip out 'members' and 'subgroups', as we are only
    // testing the properties here.
    $reducedData = array();
    foreach ($data as $branch => $branchinfo) {
      foreach ($branchinfo['lists'] as $office => $officeinfo) {
        $reducedData[$branch]['lists'][$office]['properties'] = $officeinfo['properties'];
      }
    }

    $expected = "
north:
  lists:
    president:
      properties:
        group-email: north-president@testdomain.org
        group-id: north-president@testdomain.org
        group-name: 'North President'
        alternate-addresses:
          - president@testdomain.org
fogs:
  lists:
    president:
      properties:
        group-email: fogs-president@testdomain.org
        group-id: fogs-president@testdomain.org
        group-name: 'Fogs President'
        alternate-addresses:
          - president@fogs.testdomain.org
geese:
  lists:
    president:
      properties:
        group-email: geese-president@testdomain.org
        group-id: geese-president@testdomain.org
        group-name: 'Geese President'
        alternate-addresses:
          - president@geese.testdomain.org
wolves:
  lists:
    president:
      properties:
        group-email: wolves-president@testdomain.org
        group-id: wolves-president@testdomain.org
        group-name: 'Wolves President'
        alternate-addresses:
          - president@wolves.testdomain.org
lightwoods:
  lists:
    president:
      properties:
        group-email: lightwoods-president@testdomain.org
        group-id: lightwoods-president@testdomain.org
        group-name: 'Lightwoods President'
        alternate-addresses:
          - president@lightwoods.testdomain.org
          - lightwoods@fogs.testdomain.org
gustyplains:
  lists:
    president:
      properties:
        group-email: gustyplains-president@testdomain.org
        group-id: gustyplains-president@testdomain.org
        group-name: 'Gustyplains President'
        alternate-addresses:
          - gustyplains@geese.testdomain.org
coldholm:
  lists:
    president:
      properties:
        group-email: coldholm-president@testdomain.org
        group-id: coldholm-president@testdomain.org
        group-name: 'Coldholm President'
        alternate-addresses:
          - coldholm@wolves.testdomain.org
seamountain:
  lists:
    president:
      properties:
        group-email: seamountain-president@testdomain.org
        group-id: seamountain-president@testdomain.org
        group-name: 'Seamountain President'
        alternate-addresses:
          - seamountain@lightwoods.testdomain.org";
    $this->assertYamlEquals(trim($expected), $reducedData);

    $data = $this->policy->generateParentage($data);
    $result = $this->policy->generateAggregatedGroups($data);

    $expected = "
all-presidents:
  properties:
    group-id: all-presidents@testdomain.org
    group-name: 'All Presidents'
    group-email: all-presidents@testdomain.org
    alternate-addresses:
      - presidents@testdomain.org
  members:
    - north-president@testdomain.org
    - fogs-president@testdomain.org
    - geese-president@testdomain.org
    - wolves-president@testdomain.org
    - lightwoods-president@testdomain.org
    - gustyplains-president@testdomain.org
    - coldholm-president@testdomain.org
    - seamountain-president@testdomain.org
fogs-all-presidents:
  properties:
    group-id: fogs-all-presidents@testdomain.org
    group-name: 'All Fogs Presidents'
    group-email: fogs-all-presidents@testdomain.org
    alternate-addresses:
      - all-presidents@fogs.testdomain.org
      - presidents@fogs.testdomain.org
  members:
    - fogs-president@testdomain.org
    - lightwoods-president@testdomain.org
    - seamountain-president@testdomain.org
geese-all-presidents:
  properties:
    group-id: geese-all-presidents@testdomain.org
    group-name: 'All Geese Presidents'
    group-email: geese-all-presidents@testdomain.org
    alternate-addresses:
      - all-presidents@geese.testdomain.org
      - presidents@geese.testdomain.org
  members:
    - geese-president@testdomain.org
    - gustyplains-president@testdomain.org
wolves-all-presidents:
  properties:
    group-id: wolves-all-presidents@testdomain.org
    group-name: 'All Wolves Presidents'
    group-email: wolves-all-presidents@testdomain.org
    alternate-addresses:
      - all-presidents@wolves.testdomain.org
      - presidents@wolves.testdomain.org
  members:
    - wolves-president@testdomain.org
    - coldholm-president@testdomain.org
lightwoods-all-presidents:
  properties:
    group-id: lightwoods-all-presidents@testdomain.org
    group-name: 'All Lightwoods Presidents'
    group-email: lightwoods-all-presidents@testdomain.org
    alternate-addresses:
      - all-presidents@lightwoods.testdomain.org
      - presidents@lightwoods.testdomain.org
  members:
    - lightwoods-president@testdomain.org
    - seamountain-president@testdomain.org";

    $this->assertYamlEquals(trim($expected), $result);
  }


  public function testCustomAggregatedGroups() {
    $data = Yaml::parse("
north:
  lists:
    president:
      - bill@testdomain.org
    vice-president:
      members:
        - walter@testdomain.org
      properties:
        alternate-addresses:
          - vice@testdomain.org
          - vicepresidents
    secretary:
      - george@testdomain.org
    gophers:
      - intern1@testdomain.org
      - intern2@testdomain.org
fogs:
  lists:
    president:
      - frank@testdomain.org
    vicepresident:
      - richard@testdomain.org
    gophers:
      - intern3@testdomain.org
      - intern4@testdomain.org");

    $normalized = $this->policy->normalize($data);

    $result = $this->policy->generateAggregatedGroups($normalized);

    $expected = "
all-presidents:
  properties:
    group-id: all-presidents@testdomain.org
    group-name: 'All Presidents'
    group-email: all-presidents@testdomain.org
    alternate-addresses:
      - presidents@testdomain.org
  members:
    - north-president@testdomain.org
    - fogs-president@testdomain.org
north-officers:
  properties:
    group-id: north-officers@testdomain.org
    group-name: 'North Officers'
    group-email: north-officers@testdomain.org
    alternate-addresses:
      - officers@testdomain.org
  members:
    - north-president@testdomain.org
    - north-vicepresident@testdomain.org
    - north-secretary@testdomain.org
    - north-gophers@testdomain.org
all-vicepresidents:
  properties:
    group-id: all-vicepresidents@testdomain.org
    group-name: 'All Vice-presidents'
    group-email: all-vicepresidents@testdomain.org
    alternate-addresses:
      - vicepresidents@testdomain.org
  members:
    - north-vicepresident@testdomain.org
    - fogs-vicepresident@testdomain.org
all-gophers:
  properties:
    group-id: all-gophers@testdomain.org
    group-name: 'All Gophers'
    group-email: all-gophers@testdomain.org
    alternate-addresses:
      - gophers@testdomain.org
  members:
    - north-gophers@testdomain.org
    - fogs-gophers@testdomain.org
fogs-officers:
  properties:
    group-id: fogs-officers@testdomain.org
    group-name: 'Fogs Officers'
    group-email: fogs-officers@testdomain.org
    alternate-addresses:
      - officers@fogs.testdomain.org
  members:
    - fogs-president@testdomain.org
    - fogs-vicepresident@testdomain.org
    - fogs-gophers@testdomain.org
north-lesseroffices:
  properties:
    group-email: north-lesseroffices@testdomain.org
    group-id: north-lesseroffices@testdomain.org
    group-name: 'North Lesser Officers'
    alternate-addresses:
      - lesseroffices@testdomain.org
  members:
    - north-vicepresident@testdomain.org
    - north-secretary@testdomain.org
    - north-gophers@testdomain.org
fogs-lesseroffices:
  properties:
    group-email: fogs-lesseroffices@testdomain.org
    group-id: fogs-lesseroffices@testdomain.org
    group-name: 'Fogs Lesser Officers'
    alternate-addresses:
      - lesseroffices@fogs.testdomain.org
  members:
    - fogs-gophers@testdomain.org";

    $this->assertYamlEquals(trim($expected), $result);

    $masterDirectory = $this->policy->generateMasterDirectory($normalized);

    // $this->assertEquals("", var_export($masterDirectory, TRUE));
    $this->policy->removeDuplicateAlternates($result, $masterDirectory);

    $expectedAfterRemoval = "
all-presidents:
  properties:
    group-id: all-presidents@testdomain.org
    group-name: 'All Presidents'
    group-email: all-presidents@testdomain.org
    alternate-addresses:
      - presidents@testdomain.org
  members:
    - north-president@testdomain.org
    - fogs-president@testdomain.org
north-officers:
  properties:
    group-id: north-officers@testdomain.org
    group-name: 'North Officers'
    group-email: north-officers@testdomain.org
    alternate-addresses:
      - officers@testdomain.org
  members:
    - north-president@testdomain.org
    - north-vicepresident@testdomain.org
    - north-secretary@testdomain.org
    - north-gophers@testdomain.org
all-vicepresidents:
  properties:
    group-id: all-vicepresidents@testdomain.org
    group-name: 'All Vice-presidents'
    group-email: all-vicepresidents@testdomain.org
  members:
    - north-vicepresident@testdomain.org
    - fogs-vicepresident@testdomain.org
all-gophers:
  properties:
    group-id: all-gophers@testdomain.org
    group-name: 'All Gophers'
    group-email: all-gophers@testdomain.org
  members:
    - north-gophers@testdomain.org
    - fogs-gophers@testdomain.org
fogs-officers:
  properties:
    group-id: fogs-officers@testdomain.org
    group-name: 'Fogs Officers'
    group-email: fogs-officers@testdomain.org
    alternate-addresses:
      - officers@fogs.testdomain.org
  members:
    - fogs-president@testdomain.org
    - fogs-vicepresident@testdomain.org
    - fogs-gophers@testdomain.org
north-lesseroffices:
  properties:
    group-email: north-lesseroffices@testdomain.org
    group-id: north-lesseroffices@testdomain.org
    group-name: 'North Lesser Officers'
    alternate-addresses:
      - lesseroffices@testdomain.org
  members:
    - north-vicepresident@testdomain.org
    - north-secretary@testdomain.org
    - north-gophers@testdomain.org
fogs-lesseroffices:
  properties:
    group-email: fogs-lesseroffices@testdomain.org
    group-id: fogs-lesseroffices@testdomain.org
    group-name: 'Fogs Lesser Officers'
    alternate-addresses:
      - lesseroffices@fogs.testdomain.org
  members:
    - fogs-gophers@testdomain.org";

    $this->assertYamlEquals(trim($expectedAfterRemoval), $result);

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

}
