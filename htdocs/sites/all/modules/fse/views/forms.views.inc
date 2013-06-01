<?php
/**
 * @file
 * Provide views data and handlers for forms.module.
 */

/**
 * @defgroup views_user_module user.module handlers.
 *
 * @{
 */

/**
 * Implements hook_views_data().
 *
 * TODO: Revision table support
 * TODO: links to crud.
 */
function forms_views_data() {
  // ----------------------------------------------------------------
  // Forms table.

  // Define the base group of this table. Fields that don't
  // have a group defined will go into this field by default.
  $data['forms_submission']['table']['group']  = t('Forms submission');
  $data['forms_submission_revision']['table']['group']  = t('Forms submission revision');
  $data['forms']['table']['group']  = t('Forms');

  // Advertise this table as a possible base table.
  $data['forms_submission']['table']['base'] = array(
    'field' => 'fssid',
    'title' => t('Forms submission'),
    'help' => t('Forms submission on your site.'),
  );

  // For other base tables, explain how we join.
  $data['forms_submission']['table']['join'] = array(
    'forms_submission_revision' => array(
      'handler' => 'views_join', // This is actually optional.
      'left_table' => 'forms_submission_revision', // Because this is a direct link it could be left out.
      'left_field' => 'fsvid',
      'field' => 'fsvid',
    ),
    'forms' => array(
      'handler' => 'views_join', // This is actually optional.
      'left_table' => 'forms', // Because this is a direct link it could be left out.
      'left_field' => 'form',
      'field' => 'form',
    ),
  );

  $data['forms_submission']['fssid'] = array(
    'title' => t('Fssid'),
    'help' => t('The forms submission ID.'), // The help that appears on the UI.
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_integer',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  // Forms.
  $data['forms_submission']['fsvid'] = array(
    'title' => t('Form submission revision'), // The item it appears as on the UI.
    'help' => t('The current revision of the form.'),
    'relationship' => array(
      'base' => 'forms_submission_revision',
      'base field' => 'fsvid',
      'handler' => 'views_handler_relationship',
      'label' => t('Form submission revision'),
    ),
  );

  // Form.
  $data['forms_submission']['form'] = array(
    'title' => t('Form'), // The item it appears as on the UI.
    'help' => t('The from this submmision is made in.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'title' => t('Form'),
      'handler' => 'forms_handler_filter_forms',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'relationship' => array(
      'base' => 'forms',
      'base field' => 'form',
      'handler' => 'views_handler_relationship',
      'label' => t('Form'),
    ),
  );

  // Uid field.
  $data['forms_submission']['uid'] = array(
    'title' => t('User'),
    'help' => t('Relate the submission to the user who created it.'),
    'relationship' => array(
      'handler' => 'views_handler_relationship',
      'base' => 'users',
      'field' => 'uid',
      'label' => t('user'),
    ),
  );

  $data['forms_submission']['created_fulldate'] = array(
    'title' => t('Created date'),
    'help' => t('In the form of CCYYMMDD.'),
    'argument' => array(
      'field' => 'created',
      'handler' => 'views_handler_argument_node_created_fulldate',
    ),
  );

  $data['forms_submission']['created_year_month'] = array(
    'title' => t('Created year + month'),
    'help' => t('In the form of YYYYMM.'),
    'argument' => array(
      'field' => 'created',
      'handler' => 'views_handler_argument_node_created_year_month',
    ),
  );

  $data['forms_submission']['created_year'] = array(
    'title' => t('Created year'),
    'help' => t('In the form of YYYY.'),
    'argument' => array(
      'field' => 'created',
      'handler' => 'views_handler_argument_node_created_year',
    ),
  );

  $data['forms_submission']['created_month'] = array(
    'title' => t('Created month'),
    'help' => t('In the form of MM (01 - 12).'),
    'argument' => array(
      'field' => 'created',
      'handler' => 'views_handler_argument_node_created_month',
    ),
  );

  $data['forms_submission']['created_day'] = array(
    'title' => t('Created day'),
    'help' => t('In the form of DD (01 - 31).'),
    'argument' => array(
      'field' => 'created',
      'handler' => 'views_handler_argument_node_created_day',
    ),
  );

  $data['forms_submission']['created_week'] = array(
    'title' => t('Created week'),
    'help' => t('In the form of WW (01 - 53).'),
    'argument' => array(
      'field' => 'created',
      'handler' => 'views_handler_argument_node_created_week',
    ),
  );

  $data['forms_submission']['changed_fulldate'] = array(
    'title' => t('Updated date'),
    'help' => t('In the form of CCYYMMDD.'),
    'argument' => array(
      'field' => 'changed',
      'handler' => 'views_handler_argument_node_created_fulldate',
    ),
  );

  $data['forms_submission']['changed_year_month'] = array(
    'title' => t('Updated year + month'),
    'help' => t('In the form of YYYYMM.'),
    'argument' => array(
      'field' => 'changed',
      'handler' => 'views_handler_argument_node_created_year_month',
    ),
  );

  $data['forms_submission']['changed_year'] = array(
    'title' => t('Updated year'),
    'help' => t('In the form of YYYY.'),
    'argument' => array(
      'field' => 'changed',
      'handler' => 'views_handler_argument_node_created_year',
    ),
  );

  $data['forms_submission']['changed_month'] = array(
    'title' => t('Updated month'),
    'help' => t('In the form of MM (01 - 12).'),
    'argument' => array(
      'field' => 'changed',
      'handler' => 'views_handler_argument_node_created_month',
    ),
  );

  $data['forms_submission']['changed_day'] = array(
    'title' => t('Updated day'),
    'help' => t('In the form of DD (01 - 31).'),
    'argument' => array(
      'field' => 'changed',
      'handler' => 'views_handler_argument_node_created_day',
    ),
  );

  $data['forms_submission']['changed_week'] = array(
    'title' => t('Updated week'),
    'help' => t('In the form of WW (01 - 53).'),
    'argument' => array(
      'field' => 'changed',
      'handler' => 'views_handler_argument_node_created_week',
    ),
  );

  // Created field.
  $data['forms_submission']['created'] = array(
    'title' => t('Submission date'), // The item it appears as on the UI.
    'help' => t('The date the submission was submitted.'), // The help that appears on the UI.
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

  // Changed field.
  $data['forms_submission']['changed'] = array(
    'title' => t('Updated date'), // The item it appears as on the UI.
    'help' => t('The date the forms was last updated.'), // The help that appears on the UI.
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

  $data['forms_submission_revision']['remote_addr'] = array(
    'title' => t('Remote address'),
    'help' => t('The remote IP address of the user that submitted this submission.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'title' => t('Remote address'),
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  return $data;
}
