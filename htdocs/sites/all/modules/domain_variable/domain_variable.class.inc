<?php
/**
 * @file
 * Variable Realm controller class definition.
 */


/**
 * Realm controller for domain realms.
 */
class DomainVariableRealmController extends VariableRealmDefaultController {
  /**
   * Implementation of VariableRealmControllerInterface::getAvailableVariables().
   */
  public function getAvailableVariables() {
    $multidomain = array();
    foreach (variable_get_info() as $name => $variable) {
      if (!empty($variable['multidomain'])) {
        $multidomain[] = $name;
      }
    }
    return $multidomain;
  }
  /**
   * Implementation of VariableRealmControllerInterface::getDefaultKey().
   */
  public function getDefaultKey() {
    return _domain_variable_realm_key(domain_default());
  }
  /**
   * Implementation of VariableRealmControllerInterface::getRequestKey().
   */
  public function getRequestKey() {
    return _domain_variable_realm_key(domain_get_domain());
  }
  /**
   * Implementation of VariableRealmControllerInterface::getAllKeys().
   */
  public function getAllKeys() {
    $domain_list = array();
    foreach (domain_domains() as $data) {
      $domain_list[$data['machine_name']] = $data['sitename'];
    }
    return $domain_list;
  }
}
