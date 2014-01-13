<?php

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
