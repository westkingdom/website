<?php

use Behat\Behat\Context\Context;
use Behat\Behat\Context\SnippetAcceptingContext;
use Behat\Gherkin\Node\PyStringNode;
use Behat\Gherkin\Node\TableNode;

use Drupal\DrupalExtension\Context\RawDrupalContext;

/**
 * Define application features from the specific context.
 */
class FeatureContext extends RawDrupalContext implements Context, SnippetAcceptingContext {
  /**
   * Initializes context.
   * Every scenario gets its own context object.
   *
   * @param array $parameters
   *   Context parameters (set them in behat.yml)
   */
  public function __construct(array $parameters = []) {
    // Initialize your context here
  }

//
// Place your definition and hook methods here:
//
//  /**
//   * @Given I have done something with :stuff
//   */
//  public function iHaveDoneSomethingWith($stuff) {
//    doSomethingWith($stuff);
//  }
//

  /**
   * @Given that the widget for the :field_name field of the :bundle :entity_type is changed to :widget_type_machine_name
   *
   * Behat is not very good at filling in form fields that are taxonomy
   * term trees, or checkboxes.  The reason for this is that Behat is
   * overly energetic about regularly deleting and recreating all of the
   * test content, sometimes even in the middle of a scenario.  The result
   * of this is that the term ids all change between the time the checkbox
   * is "clicked", and the time the form is submitted, leading to a validation
   * error.  With this step definition, we can change taxonomy checkbox
   * fields into autocomplete forms, which Behat fills in with a String value
   * rather than a term id -- which works consistently.
   *
   * Example:
   *     "Given that the widget for the offices field of the regnum entityform is changed to taxonomy_autocomplete"
   *     "And the cache has been cleared"
   *
   * http://dropbucket.org/node/1265
   */
  function change_form_widget($entity_type, $bundle, $field_name, $widget_type_machine_name) {
    // Retrieve the stored instance settings to merge with the incoming values.
    $instance = field_read_instance($entity_type, $field_name, $bundle);
    // Set the right module information.
    $widget_type = field_info_widget_types($widget_type_machine_name);
    $widget_module = $widget_type['module'];

    $instance['widget']['type'] = $widget_type_machine_name;
    $instance['widget']['module'] = $widget_module;

    // Update field instance
    field_update_instance($instance);
  }


}
