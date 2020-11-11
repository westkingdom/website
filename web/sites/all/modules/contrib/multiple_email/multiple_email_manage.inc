<?php

/**
 * @file
 * Callbacks for user management of users e-mail addresses.
 */

/**
 * FormAPI callback for managing an accounts e-mail addresses.
 *
 * @param unknown_type $form_state
 * @param unknown_type $account
 */
function multiple_email_manage($form, &$form_state, $account) {
  $form = array();

  $addresses = multiple_email_load_addresses($account->uid);
  unset($addresses[0]);
  $form['#account'] = $account;

  $form['addresses']['#tree'] = TRUE;
  foreach ($addresses as $email) {
    $form['addresses'][$email->eid] = array(
      '#email' => $email,
      'email' => array(
        '#type' => 'item',
        '#markup' => $email->email,
      ),
    );
    if (user_access('administer multiple emails')) {
      $form['addresses'][$email->eid]['confirm'] = array(
        '#type' => 'checkbox',
        '#title' => t('Confirm this e-mail (<a href="!resend">resend</a>)',
          array('!resend' => url('user/' . $account->uid . '/edit/email-addresses/confirm/' . $email->eid . '/resend', array('query' => drupal_get_destination())))),
        '#default_value' => $email->confirmed,
      );
      if ($email->primary_address) {
        // The primary address can not be un-confirmed.
        $form['addresses'][$email->eid]['confirm']['#disabled'] = TRUE;
        $form['addresses'][$email->eid]['confirm']['#default_value'] = TRUE;
      }
    }
  }

  $form['new'] = array(
    'email' => array(
      '#prefix' => '<div class="add-new-placeholder">' . t('Add new e-mail') . '</div>',
      '#type' => 'textfield',
      '#size' => 30,
    ),
    'confirm' => array(
      '#prefix' => '<div class="add-new-placeholder"></div>',
      '#type' => 'checkbox',
      '#title' => t('Confirm this e-mail'),
      '#access' => user_access('administer multiple emails'),
    ),
  );

  $form['save'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}

/**
 * Handle validation of multiple_email_manage_form.
 */
function multiple_email_manage_validate($form, &$form_state) {
  if ($form_state['values']['email']) {
    if (!valid_email_address($form_state['values']['email'])) {
      form_set_error('email', t('You must enter a valid e-mail address!'));
    }
    elseif (multiple_email_find_address($form_state['values']['email'])) {
      form_set_error('email', t('Entered address is already registered on this site.'));
    }
  }
}

/**
 * Handle submission of multiple_email_manage_form.
 */
function multiple_email_manage_submit($form, &$form_state) {
  $account = $form['#account'];

  if (!empty($form_state['values']['addresses'])) {
    foreach ($form_state['values']['addresses'] as $eid => $info) {
      $email = $form['addresses'][$eid]['#email'];
      if ($email->confirmed != $info['confirm']) {
        multiple_email_confirm_email($email, $info['confirm']);
        if ($info['confirm']) {
          drupal_set_message(t('The address %email has been confirmed!', array('%email' => $email->email)));
        }
        else {
          drupal_set_message(t('The address %email is no longer confirmed.', array('%email' => $email->email)));
        }
      }
    }
  }

  // Add a new e-mail address.
  if ($form_state['values']['email']) {
    $eid = multiple_email_register_email($account->uid, $form_state['values']['email']);

    if ($eid) {
      if (user_access('administer multiple emails') && $form_state['values']['confirm'] == TRUE) {
        $email = multiple_email_load($eid);
        multiple_email_confirm_email($email);
        drupal_set_message(t('The address %email has been confirmed!', array('%email' => $email->email)));
      }
      else {
        multiple_email_send_confirmation($account, multiple_email_load($eid));
        drupal_set_message(t('The e-mail address %email has been added to your account. Check your e-mail in order to confirm this new address.',
          array('%email' => $form_state['values']['email'])));
      }
    }
    else {
      form_set_error('', t('Error attempting to register %email', array('%email' => $form_state['values']['email'])));
    }
  }
}

/**
 * Theme multiple_email_manage form.
 *
 * @param array $form
 */
function theme_multiple_email_manage($variables) {
  $form = $variables['form'];

  $rows = array();
  $account = $form['#account'];

  foreach (element_children($form['addresses']) as $eid) {
    $address = $form['addresses'][$eid]['#email'];
    $row = array();
    $row[] = drupal_render($form['addresses'][$eid]['email']);
    if (isset($form['addresses'][$eid]['confirm'])) {
      $row[] = drupal_render($form['addresses'][$eid]['confirm']);
    }
    else {
      $row[] = $address->confirmed == 1 ? t('Yes') :
                l(t('Click to confirm'), 'user/' . $account->uid . '/edit/email-addresses/confirm/' . $eid);
    }

    $ops = array();

    // See if they can edit this e-mail address.
    if (user_access('administer multiple emails') || (variable_get('multiple_email_edit_emails', 0) && !$address->primary_address)) {
      $ops[] = l(t('Edit'), 'user/' . $account->uid . '/edit/email-addresses/edit/' . $address->eid);
    }

    if ($address->primary_address == 0) {
      // Only non-primary addresses can be deleted and set as primary
      $ops[] = l(t('Delete'), 'user/' . $account->uid . '/edit/email-addresses/delete/' . $address->eid);
      if (_multiple_email_access('primary', $account, $address)) {
        $ops[] = l(t('Set as primary'), 'user/' . $account->uid . '/edit/email-addresses/primary/' . $address->eid);
      }
    }
    else {
      // Let the user know that this is the primary address.
      $ops[] = t('Primary address');
    }

    $row[] = implode(' | ', $ops);
    $rows[] = $row;
  }

  if (empty($rows)) {
    $rows = array(array(
      'class' => array('error'),
      'data' => array(
        array(
          'colspan' => 3,
          'data' => t('Unable to find any e-mail addresses.'),
        ),
      ),
    ));
  }

  $rows[] = array(
    drupal_render($form['new']['email']),
    drupal_render($form['new']['confirm']),
    '',
  );

  $headers = array(t('E-mail'), t('Confirmed'), t('Operations'));
  $output = theme('table', array('header' => $headers, 'rows' => $rows));

  $output .= drupal_render_children($form);

  return $output;
}
