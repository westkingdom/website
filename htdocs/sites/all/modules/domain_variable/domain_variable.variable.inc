<?php
/**
 * @file
 * Variable hooks for domain_variable.
 */


/**
 * Implements hook_variable_info_alter().
 *
 * Only variables which have the "multidomain" property set to TRUE, can be
 * used as domain specific variables. Here we set this property for a set of
 * default system variables.
 *
 * @see _domain_variable_variable_realm_list()
 */
function domain_variable_variable_info_alter(&$variables, $options) {
  $list = array(
    'site_name',
    'site_slogan',
    'site_frontpage',
    'site_mail',
    'anonymous',
    'theme_settings',
    'theme_[theme]_settings',
    'admin_theme',
    'date_default_timezone',
    'cache',
    'cache_lifetime',
    'maintenance_mode',
    'maintenance_mode_message',
    'language_default',
    'menu_main_links_source',
    'menu_secondary_links_source',
    //'menu_options_[node_type]', //??
    //'menu_parent_[node_type]', //??
  );
  foreach ($list as $name) {
    if (isset($variables[$name])) {
      $variables[$name]['multidomain'] = TRUE;
    }
  }
}

/**
 * Implements hook_variable_settings_form_alter().
 */
function domain_variable_variable_settings_form_alter(&$form, &$form_state, $form_id) {
  module_load_include('form.inc', 'variable_realm');
  $controller = variable_realm_controller('domain');
  if ($variables = $controller->getEnabledVariables()) {
    // Set element #prefix to correct subdomain for path-related variables
    if ($url_variables = array_intersect($variables, array('site_frontpage', 'site_403', 'site_404'))) {
      $domain = domain_machine_name_load($controller->getKey());
      _domain_variable_variable_settings_fix_url_labels($form, $url_variables, $domain);
    }
  }
}

/**
 * Replaces current base url in element #field_prefix values with the given
 * subdomain; only applied for given list of variables.
 */
function _domain_variable_variable_settings_fix_url_labels(&$form, $variables, $domain) {
  // Make sure we have at least one variable
  if (empty($variables)) return;

  // Build new prefix
  $fix = &drupal_static(__FUNCTION__, NULL);
  if (empty($fix)) {
    global $base_url;
    $prefix = $base_url . '/';
    $_path = parse_url($prefix);
    $str = $_path['host'];
    $fix = preg_replace("/$str/", $domain['subdomain'], $prefix, 1);
  }

  // Recursively replace #field_prefix on form children
  foreach (element_children($form) as $field) {
    if (count(element_children($form[$field])) && empty($form[$field]['#tree'])) {
      _domain_variable_variable_settings_fix_url_labels($form[$field], $variables, $domain);
    }
    elseif (in_array($field, $variables)) {
      if (!empty($form[$field]['#field_prefix'])) {
        $form[$field]['#field_prefix'] = $fix;
      }
    }
  }
}
