<?php
/**
 * @file
 * Provide filter for forms on behalve of the forms.module.
 */

 /**
  * Extends views_handler_filter_in_operator and provide all forms to views.
  */
class forms_handler_filter_forms extends views_handler_filter_in_operator {

  function get_value_options() {
    if (!isset($this->value_options)) {
      $this->value_title = t('Forms');
      $options = array();
      foreach (forms_info() as $form_name => $forms) {
        $options[$form_name] = t($forms->name);
      }
      asort($options);
      $this->value_options = $options;
    }
  }

}