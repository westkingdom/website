<?php

/**
 * Implements hook_variable_info().
 */
function multiple_email_variable_info($options) {
  $variables['multiple_email_confirmation_[mail_part]'] = array(
    'title' => t('Confirmation e-mail', array(), $options),
    'type' => 'mail_text',
    'description' => t("Confirmation e-mail subject & body.", array(), $options),
    'group' => 'multiple_email',
  );
  $variables['multiple_email_expire_[mail_part]'] = array(
    'title' => t('Expiration e-mail', array(), $options),
    'type' => 'mail_text',
    'description' => t("Expiration e-mail subject & body.", array(), $options),
    'group' => 'multiple_email',
  );

  return $variables;
}

/**
 * Implements hook_variable_group_info().
 */
function multiple_email_variable_group_info() {
  $groups['multiple_email'] = array(
    'title' => t('Multiple E-mails'),
    'description' => t('Multiple E-Mails settings'),
    'access' => 'administer users',
    'path' => array('admin/config/people/multiple-email'),
  );
  return $groups;
}
