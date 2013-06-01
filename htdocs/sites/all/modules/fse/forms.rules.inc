<?php
/**
 * @file
 * Provide basic rules support for the forms.module.
 */


/**
 * Implements hook_rules_event_info().
 */
function forms_rules_event_info() {
  $items = array(
    'forms_submission_insert' => array(
      'label' => t('After saving a forms submission'),
      'group' => t('Forms submission'),
      'variables' =>  array(
        'forms_submission' => array(
          'type' => 'forms_submission',
          'label' => t('Saved forms submission'),
        ),
      ),
    ),
    'forms_submission_update' => array(
      'label' => t('After updating a existing forms submission'),
      'group' => t('Forms submission'),
      'variables' =>  array(
        'forms_submission' => array(
          'type' => 'forms_submission',
          'label' => t('Updated forms submission'),
        ),
        'forms_submission_unchanged' => array(
          'type' => 'forms_submission',
          'label' => t('Unchanged forms submission'),
          'handler' => 'rules_events_entity_unchanged',
        ),
      ),
    ),
    'forms_submission_presave' => array(
      'label' => t('Forms submission is going to be saved'),
      'group' => t('Forms submission'),
      'variables' =>  array(
        'forms_submission' => array(
          'type' => 'forms_submission',
          'label' => t('Saved forms submission'),
          'skip save' => TRUE,
        ),
        'forms_submission_unchanged' => array(
          'type' => 'forms_submission',
          'label' => t('Unchanged forms submission'),
          'handler' => 'rules_events_entity_unchanged',
        ),
      ),
    ),
    'forms_submission_delete' => array(
      'label' => t('After deleting a forms submission'),
      'group' => t('Forms submission'),
      'variables' =>  array(
        'forms_submission' => array(
          'type' => 'forms_submission',
          'label' => t('Deleted forms submission'),
        ),
      ),
    ),
  );
  return $items;
}

/**
 * Implements hook_rules_data_info().
 *
 * @TODO: Add fields that are not in Field.API.
 */
function forms_rules_data_info() {
  $data = array(
    'forms_submission' => array(
      'label' => t('forms submission'),
      'parent' => 'entity',
      'group' => t('Forms submission'),
    ),
  );
  return $data;
}

/**
 * Implements hook_rules_action_info().
 */
function forms_rules_action_info() {
  $actions = array();
  return $actions;
}

/**
 * Implements hook_rules_condition_info().
 */
function forms_rules_condition_info() {

  $items['forms_submission_in_form'] = array(
    'label' => t('Forms submission in form'),
    'help' => t('Evaluates to TRUE if the given forms submission is to one of the selected forms.'),
    'base' => 'forms_condition_submission_in_form',
    'group' => t('Forms submission'),
    'parameter' => array(
      'forms_submission' => array(
        'type' => 'forms_submission',
        'label' => t('Forms submission')
      ),
      'forms' => array(
        'type' => 'list<text>',
        'label' => t('Forms'),
        'options list' => '_available_forms',
        'description' => t('The form(s) to check for.'),
        'restriction' => 'input',
      )
    ),
  );
  return $items;
}

/**
 * Supplies a array with all forms.
 */
function _available_forms() {
  $info = array();
  foreach (forms_info() as $form_machinename => $forms) {
    $info[$form_machinename] = $forms->name;
  }
  return $info;
}
/**
 * Condition callback; Checks if submission is in $forms.
 */
function forms_condition_submission_in_form($forms_submission, $forms) {
  return in_array($forms_submission->form, $forms);
}



