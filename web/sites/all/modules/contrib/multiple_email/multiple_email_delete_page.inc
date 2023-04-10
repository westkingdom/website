<?php

/**
 * @file
 * Contains functions the interface side of deleting an address
 */

/**
 * Form displayed to confirm deletion of an e-mail address from the registry
 *
 * @ingroup forms
 * @see multiple_email_delete_form_validate()
 * @see multiple_email_delete_form_submit()
 *
 * @param object $email
 *   E-mail object from the database
 *
 * @return string
 */
function multiple_email_delete_form($form, &$form_state, $account, $email) {
  $form['email'] = array(
    '#type' => 'value',
    '#value' => $email,
  );

  $form['account'] = array(
    '#type' => 'value',
    '#value' => $account,
  );

  return confirm_form($form,
    t('Are you sure you wish to delete the address %email from your user account?', array('%email' => $email->email)),
    'user/' . $account->uid . '/edit/email-addresses',
    NULL,
    t('Delete'), t('Cancel'));
}

/**
 * Processes mulitple_email_delete_form_submit
 *
 * This is where users-input triggers deletion!
 *
 * @param string $form_id
 * @param array $form_values
 */
function multiple_email_delete_form_submit($form, &$form_state) {
  $email = $form_state['values']['email'];
  $account = $form_state['values']['account'];

  multiple_email_delete_email($email->eid);
  drupal_set_message(t('The e-mail address %email has been removed from your account.', array('%email' => $email->email)));

  $form_state['redirect'] = 'user/' . $account->uid . '/edit/email-addresses';
}
