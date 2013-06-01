<?php
/**
 * @file
 * Functions for creating and managing forms in the forms module.
 */

/**
 * Page callback for the forms overview admin section.
 *
 * Overview page of all forms type definitions.
 */
function forms_overview() {

  $field_ui = module_exists('field_ui');

  if (!$field_ui) {
    drupal_set_message(t('The Field UI module is currently disabled. If you would like to be able to manage form fields, you will need to enable it on the !module_page.', array('!module_page' => l(t('modules page'), 'admin/modules/list', array('fragment' => 'edit-modules-core-field-enable')))),'warning');
  }

  $header = array(t('Name'), t('View'), t('Submissions'), array('data' => t('Operations'), 'colspan' => $field_ui ? '4' : '2'));
  $rows = array();
  $result = db_query("SELECT * FROM {forms}");

  while ($forms = $result->fetchObject()) {

    $forms_url_str = str_replace('_', '-', $forms->form);
    $row = array();

    $row = array(theme('forms_admin_overview', array('forms' => $forms)));

    // Set the view column.
    $row[] = array('data' => l(t('view'), 'form/' . $forms_url_str));

    // Set the submissions column.
    $row[] = array('data' => l(t('submissions'), 'form/' . $forms_url_str . '/submissions'));

    if ($field_ui) {
      // Manage fields.
      $row[] = array('data' => l(t('manage form fields'), 'form/' . $forms_url_str . '/edit'));

      // Display fields.
      $row[] = array('data' => l(t('manage submission display'), 'form/' . $forms_url_str . '/edit/display'));
    }

    // Set the edit column.
    $row[] = array('data' => l(t('settings'), 'form/' . $forms_url_str . '/edit/form'));

    // Set the delete column.
    $row[] = array('data' => l(t('delete'), 'form/' . $forms_url_str . '/delete'));

    $rows[] = $row;
  }

  $build['forms_table'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
    '#empty' => t('No forms available. !link.', array('!link' => l(t('Add a form'), 'admin/structure/forms/add'))),
  );

  return $build;
}

/**
 * Returns HTML for a forms description for the admin overview page.
 */
function theme_forms_admin_overview($variables) {
  $forms = $variables['forms'];

  $output = check_plain($forms->name);
  $output .= ' <small>' . t('(Machine name: @type)', array('@type' => $forms->form)) . '</small>';
  if (!empty($forms->description)) {
    $output .= '<div class="description">' . filter_xss_admin($forms->description) . '</div>';
  }
  return $output;
}

/**
 * Pagecallback for table with all submissions.
 */
function forms_submission_list_view($forms, $filters = array(), $view_mode = 'submissions_overview') {
  $build = array();

  $num_on_page = 50;
  $view_modes = field_view_mode_settings('forms_submission', $forms->form);
  $rows = array();
  $header = array();

  if (empty($view_modes[$view_mode]['custom_settings'])) {
    // Not custom, we render a default table.
    $header = array(
      array('data' => t('Submitted'), 'field' => 'forms_submission.created'),
      array('data' => t('Changed'), 'field' => 'forms_submission.changed', 'sort' => 'desc'),
      array('data' => t('User'), 'field' => 'user_name'),
      array('data' => t('IP Address'), 'field' => 'remote_addr'),
      array('data' => t('Operations'), 'colspan' => 3),
    );

    $query = db_select('forms_submission', 'forms_submission');
    $query->join('forms_submission_revision', 'forms_submission_revision', 'forms_submission.fsvid = forms_submission_revision.fsvid');
    $query->join('users', 'u', 'forms_submission.uid = u.uid');
    $query->fields('forms_submission');
    $query->addField('forms_submission_revision', 'remote_addr', 'remote_addr');
    $query->addField('u', 'name', 'user_name');
    $query->condition('forms_submission.form', $forms->form, '=');

    $query = $query->extend('PagerDefault')->limit($num_on_page);
    $query = $query->extend('TableSort')->orderByHeader($header);
    $query->addTag('forms_submission_list');
    $query->addTag('forms_submission_list_default');
    $query->addMetaData('forms', $forms);
    // We do not filter here.
    // Use hook_query_alter().
    $query->addMetaData('filters', $filters);


    $result = $query->execute();

    while ($forms_submission = $result->fetchObject()) {
      $row = array();
      $subm_uri = forms_submission_uri($forms_submission);
      $row[] = format_date($forms_submission->created, 'short');
      $row[] = format_date($forms_submission->changed, 'short');
      $row[] = l($forms_submission->user_name, 'users/' . $forms_submission->uid);
      $row[] = $forms_submission->remote_addr;
      $row[] = l(t('view'), $subm_uri['path']);
      $row[] = l(t('edit'), $subm_uri['path'] . '/edit');
      $row[] = l(t('delete'), $subm_uri['path'] . '/delete');
      $rows[] = $row;
    }
  } else {
    // Build the fields array().
    $fields = array();
    // Find fields.
    $instances = field_info_instances('forms_submission', $forms->form);
    foreach ($instances as $name => $instance) {
      $display = $instance['display'][$view_mode];
      if ($display['type'] == 'hidden') {
        continue;
      }
      $field_info = field_info_field($instance['field_name']);
      $sort_column = 'value';
      if (!isset($field_info['columns'][$sort_column])) {
        // TODO: investigate this more.
        // How can we find the column to sort on?
        // Maybe we must add this as option in the display ui?
        // For now, we take 'value' or the first valid column.
        $sort_column = FALSE;
        $columns = array_keys($field_info['columns']);
        if (count($columns)) {
          $sort_column = $columns[0];
        }
      }
      $fields[] = array(
        'label' => t($instance['label']),
        'display' => $display,
        'field' => $instance['field_name'],
        'type' => 'field',
        'weight' => $display['weight'],
        'specifier' => array(
          'field' => $instance['field_name'],
          'column' => $sort_column,
        ),
        'sortable' => ($sort_column ? TRUE : FALSE),
      );
    }
    // Find extra fields.
    $extra_fields = field_info_extra_fields('forms_submission', $forms->form, 'display');
    foreach ($extra_fields as $name => $extra_field) {
      $display = $extra_field['display'][$view_mode];
      if (!$display['visible']) {
        continue;
      }
      $fields[] = array(
        'label' => t($extra_field['label']),
        'display' => $display,
        'field' => $name,
        'type' => 'property',
        'weight' => $display['weight'],
        'specifier' => (isset($extra_field['entity_property']) && !empty($extra_field['entity_property']) ? $extra_field['entity_property'] : ''),
        'sortable' => (isset($extra_field['entity_property']) && !empty($extra_field['entity_property']) ? TRUE : FALSE),
      );
    }
    usort($fields, 'drupal_sort_weight');

    // Create header.
    foreach($fields as $field) {
      $head = array(
        'data' => $field['label'],
      );
      if ($field['sortable']) {
        $head += array(
          'field' => $field['field'],
          'type' => $field['type'],
          'specifier' => $field['specifier'],
        );
      }
      $header[] = $head;
    }
    // Add operations column.
    $header[] = array('data' => t('Operations'), 'colspan' => 3);

    // Now, let us query for the rows.
    $query = new EntityFieldQuery();
    $query->entityCondition('entity_type', 'forms_submission')
          // use property condition so that the forms_submission always joins.
          ->propertyCondition('form', $forms->form)
          ->pager($num_on_page)
          ->tableSort($header)
          ->addTag('forms_submission_list')
          ->addTag('forms_submission_list_overridden')
          ->addMetaData('forms', $forms)
          // We do not filter here.
          // Use hook_query_alter().
          ->addMetaData('filters', $filters);

    $result = $query->execute();
    $forms_submissions = array();
    if (isset($result['forms_submission']) && count($result['forms_submission'])) {
      $forms_submissions = forms_submission_load_multiple(array_keys($result['forms_submission']));
    }

    foreach ($forms_submissions as $fssid => $forms_submission) {
      //Attach fields and psuedo fields.
      $forms_submission->build = field_attach_view('forms_submission', $forms_submission, $view_mode);
      module_invoke_all('forms_submission_view', $forms_submission, $view_mode, NULL);

      $row = array();
      $subm_uri = forms_submission_uri($forms_submission);
      foreach ($fields as $field) {
        // Hide title.
        $forms_submission->build[$field['field']]['#label_display'] = 'hidden';
        $row[] = render($forms_submission->build[$field['field']]);
      }
      // Add the operations.
      $row[] = l(t('view'), $subm_uri['path']);
      $row[] = l(t('edit'), $subm_uri['path'] . '/edit');
      $row[] = l(t('delete'), $subm_uri['path'] . '/delete');
      $rows[] = $row;
    }
  }

  $build['submissions_table'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
    '#empty' => t('No submissions available.'),
  );
  $build['pager'] = array('#markup' => theme('pager'));
  return $build;
}

/**
 * Add/edit forms form definition.
 */
function forms_form($form, &$form_state, $forms = NULL) {

  $defaults = forms_set_defaults();
  if (!isset($forms->form)) {
    $forms  = $defaults;
  }
  else {
    // Add default settings to avoid notice errors.
    $forms->settings += $defaults->settings;
  }

  $form['name'] = array(
    '#title' => t('Name'),
    '#type' => 'textfield',
    '#default_value' => $forms->name,
    '#description' => t('The human-readable name of this form. It is recommended that this name begin with a capital letter and contain only letters, numbers, and spaces. This name must be unique.'),
    '#required' => TRUE,
    '#size' => 30,
  );

  $form['form'] = array(
    '#type' => 'machine_name',
    '#default_value' => $forms->form,
    '#maxlength' => 32,
    '#machine_name' => array(
      'exists' => 'forms_load',
    ),
    '#description' => t('A unique machine-readable name for this content type. It must only contain lowercase letters, numbers, and underscores. This name will be used for constructing the URL of the forms, in which underscores will be converted into hyphens.'),
  );
  $form['old_form'] = array(
    '#type' => 'value',
    '#value' => $forms->form,
  );
  $form['orig_form'] = array(
    '#type' => 'value',
    '#value' => isset($forms->orig_form) ? $forms->orig_form : '',
  );

  $form['description'] = array(
    '#title' => t('Description'),
    '#type' => 'textarea',
    '#default_value' => $forms->description,
    '#description' => t('Describe this form. This description is only used on administration pages.'),
  );

  $form['status'] = array(
    '#type' => 'fieldset',
    '#title' => t('Status settings'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['status']['page'] = array(
    '#type' => 'checkbox',
    '#title' => t('Available as page'),
    '#default_value' => $forms->page,
    '#description' => t('This form is published on its own page (and visible to non-administrators).'),
  );
  $form['status']['block'] = array(
    '#type' => 'checkbox',
    '#title' => t('Available as block'),
    '#default_value' => $forms->block,
    '#description' => t('This form is available as block.'),
  );
  $form['status']['submissions'] = array(
    '#type' => 'checkbox',
    '#title' => t('New submissions'),
    '#default_value' => $forms->submissions,
    '#description' => t('This form accepts new submissions.'),
  );

  $form['submit_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Submit settings'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['submit_settings']['after_submit'] = array(
    '#type' => 'radios',
    '#title' => t('After submit'),
    '#default_value' => $forms->settings['after_submit'],
    '#options' => array(
      'form' => t('Display empty form; ready for a new submission.'),
      'display' => t('Show the <em>forms submission</em> display. (Edit display on <em>Manage display</em> page.)'),
      'redirect' => t('Send user to redirect url.'),
    ),
  );
  $form['submit_settings']['redirect_url'] = array(
    '#title' => t('Redirect url'),
    '#type' => 'textfield',
    '#default_value' => $forms->settings['redirect_url'],
    '#description' => t('The redirect url a user is send to, after a submission.'),
    '#size' => 60,
    '#states' => array(
      'visible' => array(
        ':input[name="after_submit"]' => array('value' => 'redirect'),
      ),
    ),
    '#required' => TRUE,
  );
  $form['submit_settings']['display_message'] = array(
    '#type' => 'radios',
    '#title' => t('Display confirmation message'),
    '#default_value' => $forms->settings['display_message'],
    '#options' => array(
      '0' => t('Don\'t display confirmation message.'),
      'system' => t('Display confirmation message as system message.'),
      'display' => t('Add confirmation message to <em>forms submission</em> display.'),
    ),
  );
  $form['submit_settings']['confirmation_message'] = array(
    '#title' => t('Confirmation message'),
    '#type' => 'textarea',
    '#default_value' => $forms->settings['confirmation_message'],
    '#description' => t('The translateable confirmation message that is displayed after a submission.'),
    '#rows' => 2,
    '#states' => array(
      'invisible' => array(
        ':input[name="display_message"]' => array('value' => '0'),
      ),
    ),
    '#required' => TRUE,
  );

  $form['display'] = array(
    '#type' => 'fieldset',
    '#title' => t('Display settings'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['display']['submit_label'] = array(
    '#title' => t('Submit label'),
    '#type' => 'textfield',
    '#default_value' => $forms->settings['submit_label'],
    '#maxlength' => 32,
    '#description' => t('The translateable label on the submit button.'),
    '#required' => TRUE,
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save form'),
    '#weight' => 40,
  );
  if (isset($forms->form) && !empty($forms->form)) {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete form'),
      '#weight' => 45,
    );
  }
  return $form;
}

/**
 * Validate callback.
 */
function forms_form_validate($form, &$form_state) {
  $values = &$form_state['values'];

  if (in_array(trim($values['form']), array('0', 'theme'))) {
    form_set_error('form', t("Invalid machine-readable name. Enter a name other than %invalid.", array('%invalid' => trim($values['form']))));
  }
}

/**
 * Submit callback.
 */
function forms_form_submit($form, &$form_state) {
  $values = &$form_state['values'];

  $forms = forms_set_defaults();
  $forms->form = trim($values['form']);
  $forms->orig_form = trim($values['orig_form']);
  $forms->old_form = isset($values['old_form']) ? $values['old_form'] : $forms->form;
  $forms->name = trim($values['name']);
  $forms->description = $values['description'];
  $forms->page = $values['page'];
  $forms->block = $values['block'];
  $forms->submissions = $values['submissions'];
  $forms->settings = array(
    'submit_label' => $values['submit_label'],
    'after_submit' => $values['after_submit'],
    'redirect_url' => $values['redirect_url'],
    'confirmation_message' => $values['confirmation_message'],
    'display_message' => $values['display_message'],
  );

  if ($values['op'] == t('Delete form')) {
    $form_state['redirect'] = 'form/' . str_replace('_', '-', $forms->old_form) . '/delete';
    return;
  }

  switch (forms_save($forms)) {
    case SAVED_UPDATED:
      drupal_set_message(t('The form %name has been updated.', array('%name' => $forms->name)));
      break;

    case SAVED_NEW:
      drupal_set_message(t('The form %name has been added.', array('%name' => $forms->name)));
      watchdog('forms', 'Added form %name.', array('%name' => $forms->name), WATCHDOG_NOTICE, l(t('view'), 'admin/structure/forms/manage/' . str_replace('_', '-', $forms->form)));
      break;
  }

  $form_state['redirect'] = 'form/' . str_replace('_', '-', $forms->form) . '/edit';
  return;
}

/**
 * Confirmation form before deleting a forms type definition.
 */
function forms_delete_confirm($form, &$form_state, $forms) {
  $form['form'] = array('#type' => 'value', '#value' => $forms->form);
  $form['name'] = array('#type' => 'value', '#value' => $forms->name);

  $message = t('Are you sure you want to delete the form %form_name?', array('%form_name' => $forms->name));
  $caption = '';

  $num_submissions = db_query("SELECT COUNT(fssid) FROM {forms_submission} WHERE form = :form", array(':form' => $forms->form))->fetchField();
  if ($num_submissions) {
    $caption .= '<p>' . format_plural($num_submissions, '%form_name has 1 submission. If you remove this form, you will not be able to edit the %form_name submission and it may not display correctly.', '%form_name has @count submissions. If you remove %form_name, you will not be able to edit the %form_name submissions and it may not display correctly.', array('%form_name' => $forms->name)) . '</p>';
  }

  $caption .= '<p>' . t('This action cannot be undone.') . '</p>';
  $form = confirm_form($form, $message, 'form/' . str_replace('_', '-', $forms->form) , $caption, t('Delete'));
  if ($num_submissions) {
    $form['submissions'] = array(
      '#type' => 'checkbox',
      '#title' => t('Delete submissions'),
      '#default_value' => 0,
      '#description' => t('Delete the submissions also.'),
    );
  }
  return $form;
}

/**
 * Submit callback.
 */
function forms_delete_confirm_submit($form, &$form_state) {
  $values = &$form_state['values'];

  if (isset($values['submissions']) && $values['submissions']) {
    // Work in batch, this can take along time.
    $batch = array(
      'title' => t('Deleting submissions for form %form_name', array('%form_name' => $values['name'])),
      'operations' => array(
        array('forms_batch_delete_all_submissions', array($values['form'])),
      ),
      'finished' => 'forms_batch_delete_all_submissions_finished',
      'file' => drupal_get_path('module', 'forms') . '/forms.admin.inc',
    );
    batch_set($batch);
  }
  else {
    $t_args = array('%name' => $values['name']);
    drupal_set_message(t('The form %name has been deleted.', $t_args));
    watchdog('forms', 'Deleted form %name.', $t_args, WATCHDOG_NOTICE);
    forms_delete($values['form']);
  }


  $form_state['redirect'] = 'admin/structure/forms';
  return;
}

/**
 * Batch callback.
 *
 * Deletes submissions form a from in a batch.
 */
function forms_batch_delete_all_submissions($form, &$context) {
  if (empty($context['sandbox'])) {
    $context['sandbox']['progress'] = 0;
    $context['sandbox']['current_submission'] = 0;
    $context['sandbox']['max'] = db_query('SELECT COUNT(DISTINCT fssid) FROM {forms_submission} WHERE form = :form', array(':form' => $form))->fetchField();
    $context['results'][] = $form; // first result is the form!
  }
  $limit = 10;
  $result = db_select('forms_submission')
    ->fields('forms_submission', array('fssid'))
    ->condition('fssid', $context['sandbox']['current_submission'], '>')
    ->condition('form', $form, '=')
    ->orderBy('fssid')
    ->range(0, $limit)
    ->execute();
  foreach ($result as $row) {
    $forms_submission = forms_submission_load($row->fssid, TRUE);
    $context['results'][] = $forms_submission->fssid;
    $context['sandbox']['progress']++;
    $context['sandbox']['current_submission'] = $forms_submission->fssid;
    $context['message'] = t('Deleting submission: @fssid.', array('@fssid' => $forms_submission->fssid));
    forms_submission_delete($forms_submission->fssid);
  }
  if ($context['sandbox']['progress'] != $context['sandbox']['max']) {
    $context['finished'] = $context['sandbox']['progress'] / $context['sandbox']['max'];
  }
}

/**
 * Batch finished callback.
 *
 * Runs once after a batch delete of submissions.
 */
function forms_batch_delete_all_submissions_finished($success, $results, $operations) {
  if ($success) {
    $form = array_shift($results);
    $forms = forms_info($form);
    $message = format_plural(count($results), 'one submission', '@count submissions');
    $t_args = array('%name' => $forms->name, '@submissions' => $message);
    drupal_set_message(t('The form %name and @submissions has been deleted.', $t_args));
    watchdog('forms', 'Deleted form %name and @submissions.', $t_args, WATCHDOG_NOTICE);
    forms_delete($forms->form);
  }
  else {
    drupal_set_message(t('Finished with an error.'), 'error');
  }
}