<?php

/**
 * @file
 * Functions for creating and managing forms submissions in the forms module.
 */

/*
 * Menu callback.
 *
 * @return
 *   build array.
 */
function forms_submission_page_view($forms, $forms_submission, $mode = 'default', $lang = NULL) {

  $forms_submission->build = array(
    '#view_mode' => $mode,
  );
  field_attach_prepare_view('forms_submission', array($forms_submission->fssid => $forms_submission), $mode, $lang);
  entity_prepare_view('forms_submission', array($forms_submission->fssid => $forms_submission), $mode, $lang);

  $forms_submission->build += field_attach_view('forms_submission', $forms_submission, $mode, $lang);

  module_invoke_all('forms_submission_view', $forms_submission, $mode, $lang);
  module_invoke_all('entity_view', $forms_submission, 'forms_submission', $mode, $lang);

  return $forms_submission->build;
}


/*
 * Menu callback.
 */
function forms_submission_edit($forms, $forms_submission = NULL) {
  global $user;

  $defaults = forms_set_defaults();
  $forms->settings += $defaults->settings;

  if (!$forms->page) {
    drupal_set_message(t('This form is not published on a page. You can publish the form on the !link page.', array('!link' => l(t('edit'), 'form/' . str_replace('_', '-', $forms->form) . '/edit/form'))),
      'warning'
    );
  }
  if (!$forms->submissions) {
    drupal_set_message(t('This form does not accept new submissions. You can change the form settings on the !link page.', array('!link' => l(t('edit'), 'form/' . str_replace('_', '-', $forms->form) . '/edit/form'))));
  }
  // Set the default for a new form submission.
  if (!$forms_submission) {
    $forms_submission = (object) array(
      'uid' => $user->uid,
      'form' => $forms->form,
      'submit_label' => $forms->settings['submit_label'],
    );
  }

  return drupal_get_form($forms->form . '_forms_submission_form', $forms, $forms_submission);
}


/**
 * Implements hook_form().
 */
function forms_submission_form($form, &$form_state, $forms, $forms_submission) {
  global $user;

  if (!empty($form_state['forms_submission_saved'])) {

    // Do the housekeeping ourselves.
    // We are finished, but also did a rebuild, so cache is still here.
    // Bye Bye good old cache.
    cache_clear_all('form_' . $form_state['values']['form_build_id'], 'cache_form');
    cache_clear_all('form_state_' . $form_state['values']['form_build_id'], 'cache_form');

    // Display forms_submission (with or without message).
    if ($forms->settings['after_submit'] == 'display') {
      $form = array(); // build array.
      if ($forms->settings['display_message'] == 'display') {
        $form['message'] = array(
          '#type' => 'markup',
          '#markup' => '<div class="forms-message">' . t($forms->settings['confirmation_message']) . '</div>',
        );
      }
      $form += forms_submission_page_view($forms, $form_state['forms_submission'], 'form_submission');
      // return rendered, otherwise pre_render is not called.
      return array(
        'submission' => array(
          '#markup' => drupal_render($form),
        ),
      );
    }

  }

  $form_state['forms'] = $forms;
  $form_state['forms_submission'] = $forms_submission;

  $form['#attributes']['class'][] = 'forms-submission';
  $form['#attributes']['class'][] = 'forms-' . str_replace('_', '-', $forms->form) . '-form';

  field_attach_form('forms_submission', $forms_submission , $form, $form_state);

  if (!empty($forms->settings['submit_label'])) {
    $form['actions'] = array('#type' => 'actions');
    $form['actions']['submit'] = array(
      '#type' => 'submit',
      '#value' => t($forms->settings['submit_label']),
    );
  }

  return $form;
}

/*
 * Submit callback.
 */
function forms_submission_form_submit($form, &$form_state) {
  $forms = $form_state['forms'];
  $forms_submission = $form_state['forms_submission'];
  if (!$forms->submissions && empty($forms_submission->fssid)) {
    drupal_set_message(
      t('This form does not accept new submissions'),
      'error'
    );
    $form_state['rebuild'] = TRUE;
  }
  else {
    entity_form_submit_build_entity('forms_submission', $forms_submission, $form, $form_state);
    forms_submission_save($forms_submission);
    $form_state['forms_submission'] = $forms_submission;
    $form_state['forms_submission_saved'] = TRUE;
    if ($forms->settings['after_submit'] == 'display') {
      $form_state['rebuild'] = TRUE;
    }
    if ($forms->settings['display_message'] == 'system') {
      drupal_set_message(t($forms->settings['confirmation_message']));
    }
    if ($forms->settings['after_submit'] == 'redirect') {
      $form_state['redirect'] = $forms->settings['redirect_url'];
    }
  }
}

/**
 * Menu callback for a delete confirm.
 */
function forms_submission_delete_confirm($form, &$form_state, $forms, $forms_submission) {
  $form['#forms_submission'] = $forms_submission;
  $form['fssid'] = array('#type' => 'value', '#value' => $forms_submission->fssid);
  $form = confirm_form($form,
    t('Are you sure you want this submission?'),
    'form/' . str_replace('_', '-', $forms->form) . '/submission/' . $forms_submission->fssid,
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
  $form['build'] = forms_submission_page_view($forms, $forms_submission);
  return $form;
}

/**
 * Execute after a confirmed delete.
 */
function forms_submission_delete_confirm_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    $forms_submission = forms_submission_load($form_state['values']['fssid']);
    forms_submission_delete($form_state['values']['fssid']);
    watchdog('forms', '@form: deleted submission %fssid.', array('@form' => $forms_submission->form, '%fssid' => $forms_submission->fssid));
    drupal_set_message(t('Submission %fssid has been deleted.', array('%fssid' => $forms_submission->fssid)));
  }

  $form_state['redirect'] = 'form/' . str_replace('_', '-', $forms_submission->form) . '/submissions';
}
