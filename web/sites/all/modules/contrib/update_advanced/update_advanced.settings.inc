<?php

/**
 * @file
 * Code only required on the settings tab of the update status page.
 */

/**
 * Alters the update_settings form to add per-project settings.
 */
function _update_advanced_alter_settings(&$form, $form_state) {
  $available = update_get_available(TRUE);
  if (!$available) {
    return;
  }

  $form['#theme'] = 'update_advanced_settings';
  $form['#attached']['css'][] = drupal_get_path('module', 'update_advanced') . '/update_advanced.css';

  $values = variable_get('update_advanced_project_settings', array());
  $form['update_advanced_project_settings'] = array('#tree' => TRUE);

  module_load_include('inc', 'update', 'update.compare');
  $data = update_calculate_project_data($available);
  $form['data'] = array('#type' => 'value', '#value' => $data);
  $form['available'] = array('#type' => 'value', '#value' => $available);
  // We need to call our own submit callback first, not the one from
  // update_settings_form, so that we can unset 'data' and 'available'
  // before they are saved into the {variables} table.
  array_unshift($form['#submit'], 'update_advanced_settings_submit');

  $form['update_advanced_project_settings_help'] = array(
    '#markup' => t('These settings allow you to control if a certain project, or even a specific release of that project, should be ignored by the available updates report. For each project, you can select if it should always warn you about a newer release, never warn you (ignore the project completely), or ignore a specific available release you do not want to upgrade to. You can also specify a note explaining why you are ignoring a specific project or version, and that will be displayed on the available updates report.'),
  );

  foreach ($data as $key => $project) {
    if (isset($available[$key])) {
      if (!isset($values[$key])) {
        $values[$key] = array(
          'check' => 'always',
          'notes' => '',
        );
      }

      $options = array();
      $options['always'] = t('Always');
      if (isset($project['recommended'])) {
        $options[$project['recommended']] = t('Ignore @version', array('@version' => $project['recommended']));
      }
      $options['never'] = t('Never');

      $form['update_advanced_project_settings'][$key]['check'] = array(
        '#type' => 'select',
        '#options' => $options,
        '#default_value' => $values[$key]['check'],
      );
      $form['update_advanced_project_settings'][$key]['notes'] = array(
        '#type' => 'textfield',
        '#size' => 50,
        '#default_value' => $values[$key]['notes'],
      );
    }
  }
}

/**
 * Theme function for the update status settings tab.
 *
 * Formats the table of per-project settings, and renders the rest of the
 * form elements into their proper places for maximum clarity.
 */
function theme_update_advanced_settings($variables) {
  $form = $variables['form'];

  $output = '';
  // Render the core update settings form.
  $output .= drupal_render($form['update_check_frequency']);
  $output .= drupal_render($form['update_check_disabled']);
  $output .= drupal_render($form['update_notify_emails']);
  $output .= drupal_render($form['update_notification_threshold']);

  $header = array(
    array(
      'data' => t('Project'),
      'class' => array('update-advanced-project'),
    ),
    array(
      'data' => t('Warn if out of date'),
      'class' => 'update-advanced-status',
    ),
    array(
      'data' => t('Notes'),
      'class' => array('update-advanced-notes'),
    ),
  );

  $data = $form['data']['#value'];
  $available = $form['available']['#value'];

  $rows = array();
  foreach ($data as $key => $project) {
    if (isset($available[$key])) {
      $row = array();
      $row[] = array(
        'class' => array('update-project'),
        'data' => check_plain(isset($project['title']) ? $project['title'] : $project['info']['name']),
      );
      $row[] = array(
        'class' => array('update-status'),
        'data' => drupal_render($form['update_advanced_project_settings'][$key]['check']),
      );
      $row[] = array(
        'class' => array('update-notes'),
        'data' => drupal_render($form['update_advanced_project_settings'][$key]['notes']),
      );
      if (!isset($rows[$project['project_type']])) {
        $rows[$project['project_type']] = array();
      }
      $row_key = drupal_strtolower(isset($project['title']) ? $project['title'] : $project['info']['name']);
      $rows[$project['project_type']][$row_key] = $row;
    }
  }

  $split_rows = array();
  $project_types = array(
    'core' => t('Drupal core'),
    'module' => t('Modules'),
    'theme' => t('Themes'),
    'module-disabled' => t('Disabled modules'),
    'theme-disabled' => t('Disabled themes'),
  );
  foreach ($project_types as $type_name => $type_label) {
    if (!empty($rows[$type_name])) {
      $split_rows[] = array(
        'class' => array('update-advanced-settings-label'),
        'data' => array(
          array(
            'class' => array('update-advanced-settings-label'),
            'data' => $type_label,
            'colspan' => 3,
          ),
        ),
      );
      ksort($rows[$type_name]);
      $split_rows = array_merge($split_rows, $rows[$type_name]);
    }
  }

  $output .= theme('table', array('header' => $header, 'rows' => $split_rows, 'attributes' => array('class' => array('update-advanced-settings'))));
  $output .= '<div class="description">' ."\n";
  $output .= drupal_render($form['update_advanced_project_settings_help']);
  $output .= "</div>\n";
  $output .= drupal_render($form['actions']);
  $output .= drupal_render_children($form);
  return $output;
}

/**
 * Submit handler for the update status settings tab.
 *
 * Ensures that the temporary form data required for the theme function is not
 * actually saved into the {variables} table, then invokes the true submit
 * handler for the settings form to save or reset all the values.
 */
function update_advanced_settings_submit($form, &$form_state) {
  unset($form_state['values']['data']);
  unset($form_state['values']['available']);
}
