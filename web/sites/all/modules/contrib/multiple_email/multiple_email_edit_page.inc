<?php

/**
 * @file
 * Functions for the page to edit an existing e-mail address in the registry.
 */

/**
 * Renders the page for adding a new e-mail address.
 *
 * @param object $account
 *   Loaded user account object.
 * @param object $email
 *   Loaded e-mail object.
 *
 * @return string
 */
function multiple_email_edit_page($account, $email) {
  menu_set_active_item('user/' . $account->uid . '/edit/email-addresses');
  return drupal_get_form('multiple_email_edit_form', $account, $email);
}

/**
 * Builds form for editing an e-mail address
 *
 * @ingroup forms
 *
 * @param object $account User object
 */
function multiple_email_edit_form($form, &$form_state, $account, $email) {
  $form['email'] = array(
    '#type' => 'textfield',
    '#title' => t('E-mail Address'),
    '#required' => TRUE,
    '#default_value' => $email->email,
  );

  $form['eid'] = array(
    '#type' => 'value',
    '#value' => $email->eid,
  );

  $form['account'] = array(
    '#type' => 'value',
    '#value' => $account,
  );

  if (user_access('administer multiple emails')) {
    $form['confirm'] = array(
      '#type' => 'checkbox',
      '#title' => t('Activate this email.'),
      '#default_value' => $email->confirmed,
    );
  }
  else {
    $form['confirm'] = array(
      '#type' => 'value',
      '#value' => $email->confirmed,
    );
  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}

/**
 * Validates multiple_email_edit_form
 *
 * @param string $form_id
 * @param array $form_values
 */
function multiple_email_edit_form_validate($form, &$form_state) {
  if (!valid_email_address($form_state['values']['email'])) {
    form_set_error('email', t('You must enter a valid e-mail address!'));
  }
  elseif ($form_state['values']['email'] != $form['email']['#default_value'] && multiple_email_find_address($form_state['values']['email'])) {
    form_set_error('email', t('Entered address is already registered on this site.'));
  }
}

/**
 * Processes form submission of multiple_email_edit_form
 *
 * @param string $form_id
 * @param array $form_values
 */
function multiple_email_edit_form_submit($form, &$form_state) {
  $account = $form_state['values']['account'];
  $form_state['redirect'] = check_url('user/' . $account->uid . '/edit/email-addresses');

  if ($form['email']['#default_value'] != $form_state['values']['email']) {
    // E-mail address has been changed. Register E-mail.
    multiple_email_register_email($account->uid, $form_state['values']['email'], FALSE, $form_state['values']['eid']);
    $confirm = user_access('administer multiple emails') ? $form_state['values']['confirm'] : FALSE;
    drupal_set_message(t('The e-mail address has been changed to %email.', array('%email' => $form_state['values']['email'])));
  }
  else {
    $confirm = $form_state['values']['confirm'];
    drupal_set_message(t('No change was made to the e-mail address.'));
  }

  $email = multiple_email_load($form_state['values']['eid']);
  multiple_email_confirm_email($email, $confirm);

  switch ($confirm) {
    case TRUE:
      drupal_set_message(t('The e-mail address %email is confirmed.', array('%email' => $form_state['values']['email'])));
      break;

    case FALSE:
      drupal_set_message(t('The e-mail address %email is awaiting confirmation.', array('%email' => $form_state['values']['email'])));
      multiple_email_send_confirmation($account, multiple_email_load($form_state['values']['eid']));
      break;
  }
}
