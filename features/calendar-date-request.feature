Feature: Calendar Event Request Form
  In order to test the Calendar Date Request workflow
  As a website user
  I need to be able to submit Calendar Date requests

  @api
  Scenario: Regnum form loads
    Given I am on "/calendar-date-request"
    Then I should see "Calendar Date Request"
    And I should see "Sponsoring Branch"
