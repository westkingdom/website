<?php

/**
 * Menu callback for admin configuration settings.
 */
function token_filter_admin_configure() {

  $form['token_filter_allowed_tags'] = array(
    '#type' => 'textfield',
    '#title' => t('Allowed HTML Tags'),
    '#default_value' => variable_get('token_filter_allowed_tags', TOKEN_FILTER_DEFAULT_ALLOWED_TAGS),
    '#maxlength' => 180,
  );

  return system_settings_form($form);
}
