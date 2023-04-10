<?php

/**
 * @file
 * Functions for making an e-mail address the primary address for a user.
 */

/**
 * Builds form for confirming moving an e-mail to primary status
 *
 * @ingroup forms
 * @see multiple_email_primary_form_validate()
 * @see multiple_email_primary_form_submit()
 *
 * @param object $email
 *   And e-mail object from the database
 *
 * @return array
 */
function multiple_email_primary_form($form, &$form_state, $account, $email) {
  $form['email'] = array(
    '#type' => 'value',
    '#value' => $email,
  );

  $form['account'] = array(
    '#type' => 'value',
    '#value' => $account,
  );

  return confirm_form($form,
    t('Are you sure you wish to make the address %email your primary e-mail address?', array('%email' => $email->email)),
    'user/' . $account->uid . '/edit/email-addresses',
    t('Your primary address cannot be edited or deleted.'),
    t('Make Primary'), t('Cancel'));
}

/**
 * Processes multiple_email_primary_form submission
 *
 * @param string $form_id
 * @param array $form_values
 */
function multiple_email_primary_form_submit($form, &$form_state) {
  $email = $form_state['values']['email'];
  $account = $form_state['values']['account'];

  if ($email->confirmed == 0) {
    // if admin set address as primary, confirm email
    multiple_email_confirm_email($email);
  }

  multiple_email_make_primary($email);
  drupal_set_message(t('%email is now your primary e-mail address.', array('%email' => $email->email)));

  $form_state['redirect'] = 'user/' . $account->uid . '/edit/email-addresses';
}
