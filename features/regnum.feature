Feature: Regnum
  In order to test the Regnum workflow
  As a website user
  I need to be able to submit and approve Regnum change requests

  # See the 'install-configuration' script for static taxonomy terms
  # used by these tests.

  # When we set these via a Behat "Background", we would employ
  # a workaround in tests that needed to submit a taxonomy tree
  # checkboxes field:

  # Given that the widget for the "taxonomy_vocabulary_2" field
  #     of the "regnum_change" "entityform" is changed to
  #     "taxonomy_autocomplete"
  # And that the widget for the "field_office" field of the
  #     "regnum_change" "entityform" is changed to
  #     "taxonomy_autocomplete"
  # And the cache has been cleared
  # And I enter "Principality of Contrivance" for "For Branch"
  # And I enter "Chatalaine" for "Office"

  # Now, though, we can use the more straightforward directive:
  # And I check the box "Chatalaine"

  @api
  Scenario: Regnum form loads
    Given I am on "/regnum/fill-in"
    Then I should see "Regnum Change"
    And I should see "Seneschal"

  @api
  Scenario: Regnum form loads for a logged-in user
    Given I am logged in as a user with the "administrator" role
    And I am on "/regnum/fill-in"
    Then I should see "Regnum Change"

  @api
  Scenario: Submit Regnum with some required fields missing
    Given I am on "/regnum/fill-in"
    And I enter "john.doe@domain.com" for "Personal email address"
    And I press "Submit Form"
    Then I should see "Office field is required."

  @api
  Scenario: Submit Regnum with some required fields missing
    Given I am on "/regnum/fill-in"
    And I enter "john.doe@domain.com" for "Personal email address"
    And I check the box "Principality of Contrivance"
    And I check the box "Chatalaine"
    And I enter "Joesephous the Imaginative" for "Society Reference Name"
    And I enter "Joe Bloggs" for "Legal Name"
    And I press "Submit Form"
    Then I should see "Regnum Change Notification Submitted"
