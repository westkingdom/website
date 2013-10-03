<?php

/**
 * @file
 * Domain hooks for Domain Conf.
 *
 * @ingroup domain_variable
 */

/**
 * Implements hook_domain_batch().
 *
 * @todo can we make this work again?
 */
function domain_variable_domain_batch() {
}

/**
 * Implements hook_domain_delete().
 */
function domain_variable_domain_delete($domain, $form_values = array()) {
  $realm_key = _domain_variable_realm_key($domain);
  variable_store_delete_all('domain', $domain['domain_id']);
}
