Feature: Calendar Navigation
  In order to ensure that Calendar Navigation works
  As a website user
  I need to be able to go to the correct month when I use the nav buttons

  Scenario: Next button works for even months
    Given I am on "/calendar/sca/2019-12"
    And I follow "Next"
    Then I should see "January 2020"

  Scenario: Next button works for odd months
    Given I am on "/calendar/sca/2020-01"
    And I follow "Next"
    Then I should see "February 2020"
