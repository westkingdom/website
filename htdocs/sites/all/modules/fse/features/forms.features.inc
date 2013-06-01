<?php
/**
 * @file
 * Provide feature support for forms.module.
 */

/**
 * Implements hook_features_export_options().
 */
function forms_features_export_options() {
  $options = array();
  foreach (forms_info() as $form_machinename => $forms) {
    $options[$form_machinename] = $forms->name;
  }
  return $options;
}

/**
 * Implements hook_features_export().
 */
function forms_features_export($data, &$export, $module_name = '') {
  $pipe = array();
  $map = features_get_default_map('forms');

  foreach ($data as $forms_machinename) {
    if ($forms = forms_info($forms_machinename)) {
      $export['features']['forms'][$forms_machinename] = $forms_machinename;
      $export['dependencies']['forms'] = 'forms';
      $export['dependencies']['features'] = 'features';
      $fields = field_info_instances('forms_submission', $forms_machinename);
      foreach ($fields as $name => $field) {
        $pipe['field'][] = "forms_submission-{$field['bundle']}-{$field['field_name']}";
      }
    }
  }

  return $pipe;
}

/**
 * Implements hook_features_export_render().
 */
function forms_features_export_render($module, $data, $export = NULL) {
  $elements = array(
    'name' => TRUE,
    'description' => TRUE,
    'module' => FALSE,
    'page' => FALSE,
    'block' => FALSE,
    'submissions' => FALSE,
  );
  $settings_elements = array(
    'submit_label' => TRUE,
    'after_submit' => TRUE,
    'redirect_url' => TRUE,
    'confirmation_message' => TRUE,
    'display_message' => TRUE,
  );
  $settings_elements_translate = array(
    'submit_label' => TRUE,
    'after_submit' => FALSE,
    'redirect_url' => FALSE,
    'confirmation_message' => TRUE,
    'display_message' => FALSE,
  );
  
  $output = array();
  $output[] = '  $items = array(';
  foreach ($data as $forms_machinename) {
    if ($forms = forms_info($forms_machinename)) {
      // Force module name to be 'features' if set to 'forms'. If we leave as
      // 'forms' the content type will be assumed to be database-stored by
      // the forms module.
      $forms->module = ($forms->module === 'forms') ? 'features' : $forms->module;
      $output[] = "    '{$forms_machinename}' => array(";
      //Add elements
      foreach ($elements as $key => $t) {
        if (isset($forms->$key)) {
          if ($t) {
            $text = str_replace("'", "\'", $forms->$key);
            $text = !empty($text) ? "t('{$text}')" : "''";
            $output[] = "      '{$key}' => {$text},";
          }
          else {
            $output[] = "      '{$key}' => {$forms->$key},";
          }
        }
      }
      // Add settings elements
      $output[] = "      'settings' => array(";
      foreach ($settings_elements as $key => $t) {
        if (isset($forms->settings[$key])) {
          if ($t) {
            $text = str_replace("'", "\'", $forms->settings[$key]);
            
            if ($settings_elements_translate[$key]) {
              $text = !empty($text) ? "t('{$text}')" : "''";
            } else {
              $text = !empty($text) ? "'{$text}'" : "''";  
            }
            
            $output[] = "          '{$key}' => {$text},";
          }
          else {
            $output[] = "          '{$key}' => {$forms->settings[$key]},";
          }
        }
      }
      $output[] = "      )";
      
      $output[] = "    ),";
    }
  }
  $output[] = '  );';
  $output[] = '  return $items;';
  $output = implode("\n", $output);
  
  return array('forms_info' => $output);
}

/**
 * Implements hook_features_revert().
 *
 * @param $module
 *   Name of module to revert content for.
 */
function forms_features_revert($module = NULL) {
  if ($default_forms = features_get_default('forms', $module)) {
    foreach ($default_forms as $forms_machinename => $forms) {
      // Delete forms.
      // We don't use forms_type_delete() because we do not actually
      // want to delete the form (and invoke hooks).
      // This can lead to bad consequences like CCK deleting field
      // storage in the DB.
      db_delete('forms')
        ->condition('form', $forms_machinename)
        ->execute();
    }
  }
}
