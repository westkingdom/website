<?php

/**
 * @file
 * Functions for displaying and processing the confirmation page
 */

/**
 * Renders the page to confirm an e-mail address.
 *
 * If code is passed in on URL, then it will populate that value in the
 * text field.
 *
 * @param object $account
 *   Loaded user account object.
 * @param object $email
 *   Loaded e-mail object.
 * @param string $code
 *   Optional confirmation code.
 *
 * @return string
 */
function multiple_email_confirm_page($account, $email, $code = NULL) {
  global $user;
  if (_multiple_email_access('confirm', $account, $email)) {

    menu_set_active_item('user/' . $account->uid . '/edit/email-addresses');
    return drupal_get_form('multiple_email_confirm_form', $account, $email, $code);
  }
  elseif (empty($user->uid)) {
    drupal_set_message(t('You must be signed in to confirm an address.'));
    drupal_goto('user/login', array('query' => drupal_get_destination()));
  }
  else {
    drupal_access_denied();
  }
}

/**
 * Builds e-mail confirmation form
 *
 * @ingroup forms
 *
 * @see multiple_email_confirm_form_submit()
 *
 * @param object $email
 * @param string $confirm_code
 *
 * @return array
 */
function multiple_email_confirm_form($form, &$form_state, $account, $email, $confirm_code) {
  $form = array();

  $form[] = array(
    '#type' => 'markup',
    '#markup' => '<p>' . t('The e-mail address %email is awaiting confirmation. You should have received an e-mail at that address with a confirmation code in it. Enter the code below and click confirm. If you have lost this e-mail or did not recieve a confirmation code you can try <a href="!resend">resending</a> it.', array(
      '%email' => $email->email,
      '!resend' => url('user/' . $account->uid . '/edit/email-addresses/confirm/' . $email->eid . '/resend'),
    )) . '</p>',
  );

  $form['#email'] = $email;
  $form['#account'] = $account;

  $form['code'] = array(
    '#type' => 'textfield',
    '#title' => t('Confirmation Code'),
    '#required' => TRUE,
    '#default_value' => check_plain($confirm_code),
  );

  $form_state['#redirect'] = 'user/' . $account->uid . '/edit/email-addresses';
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Confirm'),
  );

  return $form;
}

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function multiple_email_confirm_form_validate($form, &$form_state) {
  $code = trim($form_state['values']['code']);

  $email = $form['#email'];
  $account = $form['#account'];

  if ($code != '') {
    $attempts = $email->attempts + 1;
    $allowed = (int) variable_get('multiple_email_confirm_attempts', 3);
    db_update('multiple_email')
      ->fields(array(
        'attempts' => $attempts,
      ))
      ->condition('eid', $email->eid)
      ->execute();

    if (!$allowed || $attempts <= $allowed) {
      if ($code != $email->confirm_code) {
        form_set_error('', t('The confirmation code was incorrect'));
      }
    }
    else {
      $email->confirm_code = multiple_email_code();
      db_update('multiple_email')
        ->fields(array(
          'confirm_code' => $email->confirm_code,
          'time_code_generated' => REQUEST_TIME,
          'attempts' => 0,
        ))
        ->condition('eid', $email->eid)
        ->execute();
      multiple_email_send_confirmation($account, $email);

      // Redirect & Message
      drupal_set_message(t('You have exhausted your allowed attempts at confirming this e-mail address. A new confirmation code has been sent.'), 'error');
      drupal_goto('user/' . $account->uid . '/edit/email-addresses');
    }
  }
}

/**
 * Process multiple_email_confirm_form submission.
 *
 * @param string $form_id
 * @param array $form_values
 */
function multiple_email_confirm_form_submit($form, &$form_state) {
  // Confirmation successful!
  $email = $form['#email'];
  multiple_email_confirm_email($email);
  drupal_set_message(t('The address %email has been confirmed!', array('%email' => $email->email)));
}

/**
 * Confirmation form for resending a confirmation code.
 *
 * @param array $form_state
 *   FormAPI state array.
 * @param object $account
 *   Drupal user account object.
 * @param object $email
 *   Multiple E-Mails email object.
 */
function multiple_email_confirm_page_resend($form, &$form_state, $account, $email) {
  $msg = t('Please make sure that %email is correct. If not, please <a href="!delete">delete</a> it and add the correct address.', array(
    '%email' => $email->email,
    '!delete' => url('user/' . $account->uid . '/edit/email-addresses/delete/' . $email->eid),
  ));

  $form['#email'] = $email;
  $form['#account'] = $account;

  return confirm_form($form, t('Resend confirmation to %email.', array('%email' => $email->email)), 'user/' . $account->uid . '/edit/email-addresses/confirm/' . $email->eid, $msg, t('Resend'));
}

/**
 * Callback for resending a confirmation email.
 */
function multiple_email_confirm_page_resend_submit($form, &$form_state) {
  $email = $form['#email'];
  $account = $form['#account'];
  // Send a confirmation e-mail if one has not been sent within the specified
  // days.
  $email->confirm_code = multiple_email_code();
  db_update('multiple_email')
    ->fields(array(
      'confirm_code' => $email->confirm_code,
      'time_code_generated' => REQUEST_TIME,
      'attempts' => 0,
    ))
    ->condition('eid', $email->eid)
    ->execute();
  multiple_email_send_confirmation($account, $email);
  drupal_set_message(t('Check your %email email for your confirmation.', ['%email' => $email->email]));
}
