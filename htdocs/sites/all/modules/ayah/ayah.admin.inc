<?php

/**
 * @file
 * Functionality and helper functions for ayah administration.
 */


/**
 * Form builder function for the general AYAH configuration.
 */
function ayah_admin_settings() {
  module_load_include('inc', 'ayah');

  // Configuration of which forms to protect, with what challenge.
  $form['ayah_form_protection'] = array(
    '#type' => 'fieldset',
    '#title' => t('Form protection'),
    '#description' => t("Enter the form id(s) you want the AYAH game to appear on.  Some common id's include <em>comment_node_page_form, contact_personal_form, contact_site_form, forum_node_form, user_login, user_login_block, user_pass, user_register_form.</em>"),
  );
  // List known form_ids.
  $form['ayah_form_protection']['ayah_form_id_overview'] = array(
    '#theme' => 'ayah_admin_settings_ayah_listforms',
    '#tree' => TRUE,
  );
  $form['ayah_form_protection']['ayah_form_id_overview']['ayah_ayah_forms'] = array();
  $result = variable_get('ayah_forms', '');

  if (!empty($result)) {
    foreach ($result as $ayah_point => $value) {
      $form['ayah_form_protection']['ayah_form_id_overview']['ayah_ayah_forms'][$ayah_point] = array();
      $form['ayah_form_protection']['ayah_form_id_overview']['ayah_ayah_forms'][$ayah_point]['form_id'] = array(
        '#markup' => $ayah_point,
      );

      // Additional operations.
      $form['ayah_form_protection']['ayah_form_id_overview']['ayah_ayah_forms'][$ayah_point]['operations'] = array(
        '#markup' => implode(", ", array(
          l(t('delete'), "admin/user/ayah/ayah_game/" . $ayah_point . "/delete"),
        )),
      );
    }
  }
  else {
    drupal_set_message(t('No forms are configured.  Please add some in the AYAH configuration.'), 'warning');
  }
  // Form items for new form_id.
  $form['ayah_form_protection']['ayah_form_id_overview']['ayah_new_ayah_form'] = array();
  // Textfield for form_id.
  $form['ayah_form_protection']['ayah_form_id_overview']['ayah_new_ayah_form']['form_id'] = array(
    '#type' => 'textfield',
    '#size' => 16,
  );
  $form['ayah_form_protection']['ayah_form_id_overview']['ayah_new_ayah_form']['new_add_game'] = array(
    '#type' => 'checkbox',
  );
  // Field for the AYAH game on admin pages.
  $form['ayah_form_protection']['ayah_allow_on_admin_pages'] = array(
    '#type' => 'checkbox',
    '#title' => t('Allow AYAH games on administrative pages'),
    '#default_value' => variable_get('ayah_allow_on_admin_pages', FALSE),
    '#description' => t("This option makes it possible to add AYAH games to forms on administrative pages. AYAH games are disabled by default on administrative pages (which shouldn't be accessible to untrusted users normally) to avoid the related overhead. In some situations, e.g. in the case of demo sites, it can be usefull to allow AYAH games on administrative pages."),
  );

  // Field for AYAH form id finder.
  $form['ayah_form_protection']['ayah_form_finder'] = array(
    '#type' => 'checkbox',
    '#title' => t('Display form id of current form'),
    '#default_value' => variable_get('ayah_form_finder', FALSE),
    '#description' => t("This option displays the form id with a status message making it easier to find form ids."),
  );

  // Publisher key.
  $form['ayah_publisher_key'] = array(
    '#type' => 'textfield',
    '#title' => t('Publisher key'),
    '#default_value' => variable_get('ayah_publisher_key', ''),
    '#description' => t('The Publisher Key associated with your domain on the <a href="http://portal.areyouahuman.com/" target="_blank">portal</a>'),
  );

  // Scoring key.
  $form['ayah_scoring_key'] = array(
    '#type' => 'textfield',
    '#title' => t('Scoring key'),
    '#default_value' => variable_get('ayah_scoring_key', ''),
    '#description' => t('The Scoring Key associated with your domain on the <a href="http://portal.areyouahuman.com/" target="_blank">portal</a>'),
  );

  $form = system_settings_form($form);

  $form['#submit'][] = 'ayah_admin_settings_submit';

  return $form;
}

/**
 * Custom theme function for a table of (form_id -> selected) settings.
 */
function theme_ayah_admin_settings_ayah_listforms($variables) {
  $form = $variables['form'];
  $header = array('form_id', t('Operations'));
  $rows = array();
  // Existing ayah forms.
  foreach (element_children($form['ayah_ayah_forms']) as $key) {
    $row = array();
    $row[] = drupal_render($form['ayah_ayah_forms'][$key]['form_id']);
    $row[] = drupal_render($form['ayah_ayah_forms'][$key]['operations']);
    $rows[] = $row;
  }
  // For new ayah form.
  $row = array();
  $row[] = drupal_render($form['ayah_new_ayah_form']['form_id']);
  $row[] = '';
  $rows[] = $row;

  $output = theme('table', array('header' => $header, 'rows' => $rows));
  return $output;
}

/**
 * Validation handler for ayah_admin_settings form.
 */
function ayah_admin_settings_validate($form, $form_state) {
  $form_id = $form_state['values']['ayah_form_id_overview']['ayah_new_ayah_form']['form_id'];
  if (!preg_match('/^[a-z0-9_]*$/', $form_id)) {
    form_set_error('ayah_form_id_overview][ayah_new_ayah_form][form_id', t('Illegal form_id'));
  }
}

/**
 * Submission function for ayah_admin_settings form.
 */
function ayah_admin_settings_submit($form, &$form_state) {
  // Process ayah forms.
  if (isset($form_state['values']['ayah_form_id_overview']['ayah_ayah_forms'])) {
    foreach ($form_state['values']['ayah_form_id_overview']['ayah_ayah_forms'] as $ayah_new_form_id => $data) {
      ayah_set_form_id_setting($ayah_new_form_id, $data['add_game']);
    }
  }
  // Add new ayah form?
  $ayah_new_form_id = $form_state['values']['ayah_form_id_overview']['ayah_new_ayah_form']['form_id'];
  if (!empty($ayah_new_form_id)) {
    $ayah_type = $form_state['values']['ayah_form_id_overview']['ayah_new_ayah_form']['new_add_game'];
    ayah_set_form_id_setting($ayah_new_form_id, $ayah_type);
    drupal_set_message(t('Added ayah form.'), 'status');
  }
}

/**
 * Central handler for AYAH game administration (adding, disabling, deleting).
 */
function ayah_game_admin($ayah_form_id = NULL, $op = NULL) {
  module_load_include('inc', 'ayah');

  // If $ayah_form_id and action $op given: do the action.
  if ($ayah_form_id) {
    switch ($op) {
      case 'delete':
        return drupal_get_form('ayah_game_formdelete_confirm', $ayah_form_id, TRUE);
    }
    // Return edit form for AYAH settings.
    return drupal_get_form('ayah_admin_settings', $ayah_form_id);
  }
  // Return add form for AYAH settings.
  return drupal_get_form('ayah_admin_settings');
}

/**
 * Confirm dialog for deleting a form.
 */
function ayah_game_formdelete_confirm($form, &$form_state, $ayah_form_id, $delete) {
  $form = array();
  $form['ayah_game_form_id'] = array(
    '#type' => 'value',
    '#value' => $ayah_form_id,
  );
  $form['ayah_game_delete'] = array(
    '#type' => 'value',
    '#value' => $delete,
  );

  return confirm_form(
    $form,
    t('Are you sure you want to delete the AYAH game for form_id %form_id?', array('%form_id' => $ayah_form_id)),
    'admin/user/ayah',
    t('This action can not be undone.'),
    t('Delete'),
    t('Cancel')
  );
}

/**
 * Submission handler of AYAH game disabling/deleting confirm_form.
 */
function ayah_game_formdelete_confirm_submit($form, &$form_state) {
  module_load_include('inc', 'ayah');
  $ayah_game_form_id = $form_state['values']['ayah_game_form_id'];
  $delete = $form_state['values']['ayah_game_delete'];
  if ($delete) {
    ayah_set_form_id_setting($ayah_game_form_id, 0, TRUE);
    drupal_set_message(t('Deleted AYAH game for form %form_id.', array('%form_id' => $ayah_game_form_id)));
  }
  $form_state['redirect'] = 'admin/config/people/ayah';
}
