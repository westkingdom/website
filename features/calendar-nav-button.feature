Feature: Calendar Navigation
  In order to ensure that Calendar Navigation works
  As a website user
  I need to be able to go to the correct month when I use the nav buttons

  Scenario: Next button works for even months
    Given I am on "/calendar/sca/2016-12"
    And I follow "Next"
    Then I should see "January 2017"

  Scenario: Next button works for odd months
    Given I am on "/calendar/sca/2017-01"
    And I follow "Next"
    Then I should see "February 2017"
