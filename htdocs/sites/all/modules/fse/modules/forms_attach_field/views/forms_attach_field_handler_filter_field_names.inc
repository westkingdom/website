<?php
/**
 * @file
 * Provide filter for entity types on behalve of the forms_attach_field.module.
 */

 /**
  * Extends views_handler_filter_in_operator and provide all
  * fields of our type.
  */
class forms_attach_field_handler_filter_field_names extends views_handler_filter_in_operator {

  function get_value_options() {
    if (!isset($this->value_options)) {
      $this->value_title = t('Fields');
      $options = array();
      $field_types = field_info_field_types();

      foreach (field_info_instances() as $existing_entity_type => $bundles) {
        foreach ($bundles as $existing_bundle => $instances) {
          // No need to look in the current bundle.
          if ($existing_entity_type != 'forms_submission') {
            foreach ($instances as $instance) {
              $field = field_info_field($instance['field_name']);

              if (empty($field['locked'])
                && empty($field_types[$field['type']]['no_ui'])
                && $field['type'] == 'forms_attach_field'
                ) {
                $text = t('@label (@field)', array(
                  '@label' => t($instance['label']), '@field' => $instance['field_name'],
                ));
                $options[$instance['field_name']] = (drupal_strlen($text) > 80 ? truncate_utf8($text, 77) . '...' : $text);
              }

            }
          }
        }
      }
      asort($options);
      $this->value_options = $options;
    }
  }

}