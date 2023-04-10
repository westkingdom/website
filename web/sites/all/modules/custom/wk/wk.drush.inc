<?php

use Drush\User\UserList;

/**
 * Implements hook_drush_command().
 */
function wk_drush_command() {
  $items = array();

  $items['wk-export-mailinglists'] = array(
    'description' => "Export mailing list data.",
    //'arguments' => array(
    //  'arg' => 'Description of command argument.',
    //),
    //'options' => array(
    //  'option-name' => array(
    //    'description' => 'Description of option.',
    //    'example-value' => 'Example values for option, if required.',
    //  ),
    //),
    // Give one or more example commandline usages for this command
    'examples' => array(
      'drush wk-export-mailinglists' => 'Export mailing list data.',
    ),
    // Delete the 'outputformat' record if command does not
    // produce any output.
    'outputformat' => array(
      'default' => 'yaml',
      'pipe-format' => 'var_export',
    ),
    'aliases' => array('wkx'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH, // No bootstrap at all. Remove for full bootstrap of Drupal site.
  );

  $items['wk-insert-email'] = array(
    'description' => "Add an email address that has already been created to the specified user.",
    'arguments' => array(
      'user' => 'Username',
      'email' => '@westkingdom.org email address',
    ),
    'examples' => array(
      'drush wk-insert-email username address@westkingdom.org',
    ),
  );

  $items['wk-show-email'] = array(
    'description' => "Show organization email address associated with account",
    'arguments' => array(
      'user' => 'Username',
    ),
    'outputformat' => array(
      'default' => 'yaml',
      'pipe-format' => 'var_export',

      'field-labels' => array(
        'uid' => 'User ID',

        'eid' => 'ID',
        'uid' => 'UID',
        'email' => 'e-mail',
        'time_registered' => 'Registered',
        'confirmed' => 'Confirmed',
        'confirm_code' => 'Code',
        'time_code_generated' => 'Time Code Generated',
        'attempts' => 'Attempts',
        'primary_address' => 'Primary',
      ),
    ),
  );

  return $items;
}

function drush_wk_show_email($username) {
  $user = user_load_by_name($username);
  $result = multiple_email_load_addresses($user->uid);

  // $result[0] is a map of eid => email address.
  // The other items in the array are eid => mail record
  unset($result[0]);
  return $result;
}

function drush_wk_insert_email($username, $org_email) {
  $user = user_load_by_name($username);
  multiple_email_register_email($user->uid, $org_email, TRUE);
}

/**
 * When using the PHP built-in webserver, the wysiwyg editor
 * will hang forever, because it makes a request to the web server,
 * and the built-in webserver can only handle one request at a time.
 *
 * We therefore remove all wysiwyg editors when running with 'drush rs'.
 */
function wk_remove_wysiwyg_profiles() {
  $filter_formats = filter_formats();
  foreach ($filter_formats as $key => $info) {
    wysiwyg_profile_delete($info->format);
  }
  wysiwyg_profile_cache_clear();
}

/**
 * Implements hook_drush_help().
 *
 * @param
 *   A string with the help section (prepend with 'drush:')
 *
 * @return
 *   A string with the help text for your command.
 */
function wk_drush_help($section) {
  switch ($section) {
    case 'drush:wk-export-mailinglists':
      return dt("Export mailing list data in yaml.");
    // The 'title' meta item is used to name a group of
    // commands in `drush help`.  If a title is not defined,
    // the default is "All commands in ___", with the
    // specific name of the commandfile (e.g. wk).
    // Command files with less than four commands will
    // be placed in the "Other commands" section, _unless_
    // they define a title.  It is therefore preferable
    // to not define a title unless the file defines a lot
    // of commands.
    case 'meta:wk:title':
      return dt("West Kingdom commands");
    // The 'summary' meta item is displayed in `drush help --filter`,
    // and is used to give a general idea what the commands in this
    // command file do, and what they have in common.
    case 'meta:wk:summary':
      return dt("Commands specific to the West Kingdom site.");
  }
}

/**
 * Implementation of drush_hook_COMMAND().
 *
 * Main command implementation goes here.
 */
function drush_wk_export_mailinglists() {
  $mailinglists = wk_export_mailinglists();
  // return array("smartlist::subdomain_lists" => $mailinglists);
  return $mailinglists;
}

function wk_drush_sql_sync_sanitize($site) {
  $fields_to_sanitize = array(
    'field_alternate_phone' => '555-1234',
    'field_phone_number' => '555-1212',
    'field_additional_information' => '',
    'field_email_address' => 'user@sca.org',
    'field_location_street_address' => '1234 Main st.',
    'field_membership_number' => '1',
    'field_real_name' => '',
  );

  $site_settings = drush_sitealias_get_record($site);
  $databases = sitealias_get_databases_from_record($site_settings);
  $prefix = $databases['default']['default']['prefix'];

  foreach ($fields_to_sanitize as $field_name => $field_value) {
    $sql = "UPDATE {$prefix}field_data_{$field_name} SET {$field_name}_value = '$field_value' where {$field_name}_value != ''; ";
    $sql .= "UPDATE {$prefix}field_revision_{$field_name} SET {$field_name}_value = '$field_value' where {$field_name}_value != '';";
    drush_sql_register_post_sync_op('sanitize ' . $field_name, dt("Set all data in !field to '!value'", array('!field' => $field_name, '!value' => $field_value)), $sql);
  }
}
