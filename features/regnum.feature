Feature: Regnum
  In order to know that configuration import worked
  As a website user
  I need to be able to visit the Regnum form

  Scenario: Regnum form loads
    Given I am on "/regnum"
    Then I should see "Regnum Change"

  Scenario: Submit Regnum with some required fields missing
    Given I am on "/regnum"
    Given I enter "john.doe@domain.com" for "Personal email address"
    Given I press "Submit Form"
    Then I should see "Office field is required."
