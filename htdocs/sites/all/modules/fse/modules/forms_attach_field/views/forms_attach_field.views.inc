<?php
/**
 * @file
 * Provide views data and handlers for forms_attach_field.module.
 */

/**
 * Implements hook_views_data_alter().
 */
function forms_attach_field_views_data_alter(&$data) {
  // ----------------------------------------------------------------
  // Forms table.


  // Entity type.
  $data['forms_submission']['entity_type'] = array(
    'title' => t('Forms Attach Field: Entity type'), // The item it appears as on the UI.
    'help' => t('The entity type this submission is made against, when the form was attached to an entity with forms_attach_field.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'title' => t('Forms Attach Field: Entity type'),
      'handler' => 'forms_attach_field_handler_filter_entity_types',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  $data['forms_submission']['entity_id'] = array(
    'title' => t('Forms Attach Field: Entity id'),
    'help' => t('The entity id this submission is made against, when the form was attached to an entity with forms_attach_field.'),
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
  $data['forms_submission']['field_name'] = array(
    'title' => t('Forms Attach Field: Field name'),
    'help' => t('The field name this submission is made against, when the form was attached to an entity with forms_attach_field.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'title' => t('Forms Attach Field: Field name'),
      'handler' => 'forms_attach_field_handler_filter_field_names',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  $data['forms_submission']['field_delta'] = array(
    'title' => t('Forms Attach Field: Field delta'),
    'help' => t('The delta if the field this submission is made against, when the form was attached to an entity with forms_attach_field.'),
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

  /**
   * Relation
   */
  $usages = forms_attach_field_usage_forms_info();
  foreach ($usages as $usage) {
    $info = entity_get_info($usage['entity_type']);

    $data[$info['base table']]['forms_submissions'] = array(
      'group' => t('Forms Submissions'),
      'title' => t('Forms Submissions against @entity', array('@entity' => ucfirst($info['label']))),
      'help' => t('Forms Submissions associated with the @entity entity.', array('@entity' => ucfirst($info['label']))),
      'relationship' => array(
        'entity' => $usage['entity_type'],
        'handler' => 'views_handler_relationship',
        'label' => t('Forms Submission'),
        'base' => 'forms_submission',
        'base field' => 'entity_id',
        'relationship field' => $info['entity keys']['id'],
        'extra' => array(array('field' => 'entity_type', 'value'=>$usage['entity_type'], 'operator' => '=')),
      ),
    );



  }
}
