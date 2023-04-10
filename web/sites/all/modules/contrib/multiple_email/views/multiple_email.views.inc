<?php

/**
 * @file
 * Contains Views integration for the multiple_email module.
 */

/**
 * Implements hook_views_handlers().
 */
function multiple_email_views_handlers() {
  $handlers = array();
  $handlers['info'] = array('path' => drupal_get_path('module', 'multiple_email') . '/views');
  $handlers['handlers'] = array();
  if (module_exists('email')) {
    $handlers['handlers']['multiple_email_handler_filter_current'] = array(
      'parent' => 'views_handler_filter_boolean_operator',
    );
  }

  return $handlers;
}

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function multiple_email_views_data_alter(&$views_data) {
  foreach ($views_data as $table_key => $table) {
    foreach ($table as $key => $data) {
      if ($key != 'table' && $key != 'delta' && isset($data['field']['content_field_name'])) {
        if ($field = content_fields($data['field']['content_field_name'])) {
          if ($field['type'] == 'email') {
            $views_data[$table_key][$data['field']['content_field_name'] . '_match'] = array(
              'title' => $data['title short'] . ' (' . $field['field_name'] . ') - ' . t('match'),
              'help' => t("Match current user's multiple e-mail addresses to CCK E-mail field."),
              'group' => t('Content'),
              'filter' => $data['filter'],
            );
            $views_data[$table_key][$field['field_name'] . '_match']['filter']['handler'] = 'multiple_email_handler_filter_current';
          }
        }
      }
    }
  }
}

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function multiple_email_views_data() {
  $data = array();

  // Basic table information.
  $data['multiple_email']['table']['group'] = t('Multiple E-mails');

  // Join to 'user' as a base table.
  $data['multiple_email']['table']['join'] = array(
    'user' => array(
      'left_field' => 'uid',
      'field' => 'uid',
    ),
    'node' => array(
      'left_field' => 'uid',
      'field' => 'uid',
    ),
  );

  $data['multiple_email']['table']['base'] = array(
    'field' => 'eid',
    'title' => t('Multiple E-mail Addresses'),
    'help' => t('List each e-mail address as a separate row.'),
    'weight' => 10,
  );

  // Eid field.
  $data['multiple_email']['eid'] = array(
    'title' => t('eid'),
    'help' => t('The email ID.'),
    // Information for displaying the nid.
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),

    // Information for accepting a nid as a filter.
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),

    // Information for sorting on a nid.
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  // Uid field.
  $data['multiple_email']['uid'] = array(
    'title' => t('Owner uid'),
    'help' => t('The user owning the e-mail address. If you need more fields than the uid add "content: owner relationship."'),
    'relationship' => array(
      'title' => t('Owner'),
      'help' => t('Relate address to the user who owns it.'),
      'handler' => 'views_handler_relationship',
      'base' => 'users',
      'field' => 'uid',
      'label' => t('owner'),
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_user_name',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
    'field' => array(
      'handler' => 'views_handler_field_user',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  // E-mail address field.
  $data['multiple_email']['email'] = array(
    'title' => t('Address'),
    'help' => t('The multiple e-mail address that the user may have.'),
    'field' => array(
      'handler' => 'views_handler_field_user_mail',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
      'allow empty' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  if (module_exists('email')) {
    // Match E-mail addresses to CCK e-mail field.
    $data['multiple_email']['email_match'] = array(
      'title' => t('Match E-mail Addresses'),
      'help' => t("Match current user's multiple e-mail addresses to CCK E-mail field."),
      'real_field' => 'email',
      'filter' => array(
        'handler' => 'multiple_email_views_handler_filter_email_current',
        'allow empty' => TRUE,
      ),
    );
  }

  // Time registered field.
  $data['multiple_email']['time_registered'] = array(
    'title' => t('Registration date'),
    'help' => t('The date the e-mail address was registered.'),
    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort_date',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_date',
    ),
  );

  // Confirmed status field.
  $data['multiple_email']['confirmed'] = array(
    'title' => t('Confirmed'),
    'help' => t('Whether or not the address has been confirmed.'),
    'field' => array(
      'handler' => 'views_handler_field_boolean',
      'click sortable' => TRUE,
      'output formats' => array(
        'confirmed-unconfirmed' => array(t('Confirmed'), t('Not confirmed')),
      ),
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_boolean_operator',
      'label' => t('Confirmed'),
      'type' => 'yes-no',
      // Use `status = 1` instead of `status <> 0` in `WHERE` statement.
      'use equal' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  return $data;
}
