<?php

/**
 * Form builder; Configure regnum settings for this site.
 *
 * @ingroup forms
 * @see system_settings_form()
 */
function regnum_admin_settings() {
  // TODO: configuration for _regnum_configured_entityforms() (maybe a different form?)
  /*
  $form['regnum_configured_forms'] = array(
    '#type' => 'fieldset',
    '#title' => t('Regnum Forms'),
  );
  */

  $form['email_title'] = array(
    '#type' => 'item',
    '#title' => t('E-mails'),
  );
  $form['email'] = array(
    '#type' => 'vertical_tabs',
  );
  // These email tokens are shared for all settings, so just define
  // the list once to help ensure they stay in sync.
  // TODO: Define these
  $email_token_help = t('Available variables are: [site:name], [site:url], ...');

  //
  // Regnum form submitted: confirm with submitter
  //
  $form['email_regnum_submission_confirmation'] = array(
    '#type' => 'fieldset',
    '#title' => t('Confirmation of submission'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#description' => t('Edit the notification e-mail messages sent to approvers when a Regnum form is submitted.') . ' ' . $email_token_help,
    '#group' => 'email',
  );
  $form['email_regnum_submission_confirmation']['regnum_mail_regnum_submission_confirmation_subject'] = array(
    '#type' => 'textfield',
    '#title' => t('Subject'),
    '#default_value' => _regnum_mail_text('regnum_submission_confirmation_subject', NULL, array(), FALSE),
    '#maxlength' => 180,
  );
  $form['email_regnum_submission_confirmation']['regnum_mail_regnum_submission_confirmation_body'] = array(
    '#type' => 'textarea',
    '#title' => t('Body'),
    '#default_value' => _regnum_mail_text('regnum_submission_confirmation_body', NULL, array(), FALSE),
    '#rows' => 15,
  );

  //
  // Regnum form submitted: notify approvers
  //
  $form['email_regnum_form_submitted'] = array(
    '#type' => 'fieldset',
    '#title' => t('Notification to approvers'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#description' => t('Edit the notification e-mail messages sent to approvers when a Regnum form is submitted.') . ' ' . $email_token_help,
    '#group' => 'email',
  );

  $form['email_regnum_form_submitted']['regnum_mail_regnum_form_submitted_subject'] = array(
    '#type' => 'textfield',
    '#title' => t('Subject'),
    '#default_value' => _regnum_mail_text('regnum_form_submitted_subject', NULL, array(), FALSE),
    '#maxlength' => 180,
  );
  $form['email_regnum_form_submitted']['regnum_mail_regnum_form_submitted_body'] = array(
    '#type' => 'textarea',
    '#title' => t('Body'),
    '#default_value' => _regnum_mail_text('regnum_form_submitted_body', NULL, array(), FALSE),
    '#rows' => 15,
  );

  //
  // Regnum submission approved
  //
  $form['email_regnum_submission_approved'] = array(
    '#type' => 'fieldset',
    '#title' => t('Notification of approval'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#description' => t('Edit the notification e-mail messages sent to approvers when a Regnum form is approved.') . ' ' . $email_token_help,
    '#group' => 'email',
  );
  $form['email_regnum_submission_approved']['regnum_mail_regnum_submission_approved_subject'] = array(
    '#type' => 'textfield',
    '#title' => t('Subject'),
    '#default_value' => _regnum_mail_text('regnum_submission_approved_subject', NULL, array(), FALSE),
    '#maxlength' => 180,
  );
  $form['email_regnum_submission_approved']['regnum_mail_regnum_submission_approved_body'] = array(
    '#type' => 'textarea',
    '#title' => t('Body'),
    '#default_value' => _regnum_mail_text('regnum_submission_approved_body', NULL, array(), FALSE),
    '#rows' => 15,
  );

  //
  // Regnum submission approved later
  //
  $form['email_regnum_submission_approved_later'] = array(
    '#type' => 'fieldset',
    '#title' => t('Notification of deferred approval'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#description' => t('Edit the notification e-mail messages sent to approvers when a Regnum form is approved, to be activated later (on the effective date).') . ' ' . $email_token_help,
    '#group' => 'email',
  );
  $form['email_regnum_submission_approved_later']['regnum_mail_regnum_submission_approved_later_subject'] = array(
    '#type' => 'textfield',
    '#title' => t('Subject'),
    '#default_value' => _regnum_mail_text('regnum_submission_approved_later_subject', NULL, array(), FALSE),
    '#maxlength' => 180,
  );
  $form['email_regnum_submission_approved_later']['regnum_mail_regnum_submission_approved_later_body'] = array(
    '#type' => 'textarea',
    '#title' => t('Body'),
    '#default_value' => _regnum_mail_text('regnum_submission_approved_later_body', NULL, array(), FALSE),
    '#rows' => 15,
  );

  //
  // Regnum submission denied
  //
  $form['email_regnum_submission_denied'] = array(
    '#type' => 'fieldset',
    '#title' => t('Notification of denial'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#description' => t('Edit the notification e-mail messages sent to approvers when a Regnum form is denied.') . ' ' . $email_token_help,
    '#group' => 'email',
  );
  $form['email_regnum_submission_denied']['regnum_mail_regnum_submission_denied_subject'] = array(
    '#type' => 'textfield',
    '#title' => t('Subject'),
    '#default_value' => _regnum_mail_text('regnum_submission_denied_subject', NULL, array(), FALSE),
    '#maxlength' => 180,
  );
  $form['email_regnum_submission_denied']['regnum_mail_regnum_submission_denied_body'] = array(
    '#type' => 'textarea',
    '#title' => t('Body'),
    '#default_value' => _regnum_mail_text('regnum_submission_denied_body', NULL, array(), FALSE),
    '#rows' => 15,
  );

  return system_settings_form($form);
}

