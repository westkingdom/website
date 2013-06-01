<?php
/**
 * @file
 * Provide filter for entity types on behalve of the forms_attach_field.module.
 */

 /**
  * Extends views_handler_filter_in_operator and provide all
  * entity types we can attach to.
  */
class forms_attach_field_handler_filter_entity_types extends views_handler_filter_in_operator {

  function get_value_options() {
    if (!isset($this->value_options)) {
      $this->value_title = t('Entities');
      $options = array();
      foreach (entity_get_info() as $entity_type => $info) {
        if (!empty($info['fieldable']) && $entity_type != 'forms_submission') {
          $options[$entity_type] = ucfirst($info['label']);
        }
      }
      asort($options);
      $this->value_options = $options;
    }
  }

}