Feature: Regnum
  In order to know that configuration import worked
  As an officer in the organization
  I need to be able to visit the Regnum form

  Scenario: Regnum form loads
    Given I am on "/regnum"
    Then I should see "Regnum Change"
