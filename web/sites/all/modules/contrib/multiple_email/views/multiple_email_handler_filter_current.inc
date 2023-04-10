<?php

/**
 * @file
 * filter heandler for matching e-mail addresses to cck e-mail field.
 */

/**
 * Filter handler for the current user's e-mail addresses to match a field.
 */
class multiple_email_handler_filter_current extends views_handler_filter_boolean_operator {

  function construct() {
    parent::construct();
    $this->value_value = t('Matches one of the multiple e-mail addresses of the logged in user');
  }

  function query() {
    $this->ensure_my_table();
    $where = "$this->table_alias.$this->real_field ";
    // get the current user's addresses
    global $user;
    $emails = multiple_email_load_addresses($user->uid);
    $addresses = array();
    foreach ($emails as $email) {
      if (is_object($email) && $email->confirm_code) {
        $addresses[] = $email->email;
      }
    }
    if (empty($this->value)) {
      $where .= 'NOT ';
    }
    $where .= "IN ('" . implode("','", $addresses) . "') ";
    $this->query->add_where($this->options['group'], $where);
  }

}
