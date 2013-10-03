<?php

/**
 * @file
 * Domain manager configuration options.
 *
 * Provides admin screens for configuration overrides.
 * @ingroup domain_variable
 */

/**
 * Variables settings form
 */
function domain_variable_admin_variables($domain) {
  $realm_key = _domain_variable_realm_key($domain);
  module_load_include('form.inc', 'variable_realm');
  return drupal_get_form('variable_realm_edit_variables_form', 'domain', $realm_key);
}
