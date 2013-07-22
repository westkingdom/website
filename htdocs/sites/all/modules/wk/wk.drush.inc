<?php

function wk_drush_command() {
  $items['assign-officer'] = array(
    'description' => 'Assign the specified user to an office.',
    'arguments' => array(
      'name' => 'The name of the user to assign'
    ),
    'examples' => array(
      'drush assign-officer "Gregor Eilburg" --branch="West Kingdom" --office="Web Minister"' =>
        'Assign Gregor the office of West Kingdom Web Minister',
    ),
    'options' => array(
      'mail' => 'The email address for the new account.',
      'realname' => 'The real name for the new account.',
      'phone' => 'The phone number for the new account.',
      'alternate-phone' => 'Alternate phone number (e.g. mobile) for the new account.',
      'membership-id' => "User's membership ID.",
      'branch' => 'The branch group for the office.',
      'office' => 'The office to assign the user to.',
      'title' => 'The custom title for the office.  Optional.',
    ),
  );
  $items['approve-officer'] = array(
    'description' => 'Approve a Regnum form submitted for a particular office.',
    'arguments' => array(
      'filter' => 'The name of the user, office or branch group to approve.'
    ),
    'examples' => array(
      'drush approve-officer "Gregor Eilburg"' =>
        'Approve an active submission made by Gregor Eilburg.',
    ),
    'options' => array(
    ),
  );
  return $items;
}

function drush_wk_approve_officer($filter = '') {
  $field_map = _wk_regnum_change_field_map();
  $submission = drush_wk_select_submission('regnum_change', $filter, $field_map, "name (MKA legal) title", dt("Select a submission to view:"));
  if (!$submission) {
    return FALSE;
  }
  $user_info = _wk_entity_to_data_array($submission, $field_map);
  $user_info = _wk_fix_up_submission_data($user_info);
  drush_print_format($user_info, 'key-value');
  $confirm = drush_confirm(dt("Approve Regnum change assigning !name to the office !title?", _wk_map_to_dt_data($user_info)));
  if (!$confirm) {
    return drush_user_abort();
  }

  return wk_approve_officer($submission);
}

function drush_wk_find_value_in_array($needle, $haystack) {
  $needle = strtolower($needle);
  foreach ($haystack as $key => $value) {
    if (strpos(strtolower($value), $needle) !== FALSE) {
      return TRUE;
    }
  }
  return FALSE;
}

function drush_wk_assign_officer($name) {
  $user_info = array(
    'name' => $name,
    'mail' => drush_get_option('mail', ''),
    'realname' => drush_get_option('realname', ''),
    'phone' => drush_get_option('phone', ''),
    'alternate-phone' => drush_get_option('alternate-phone', ''),
    'membership-id' => drush_get_option('membership-id', ''),
  );
  $branch_group = drush_get_option('branch', 'Kingdom of the West');
  $branch_group_id = NULL;
  $branch_group_terms = array_keys(taxonomy_get_term_by_name($branch_group, 'vocabulary_2'));
  if (!empty($branch_group_terms)) {
    $branch_group_id = $branch_group_terms[0];
  }
  else {
    $branch_group_id = _drush_wk_select_term('vocabulary_2', 'Select a branch group', $branch_group);\
    var_export($branch_group_id);
    if ($branch_group_id === FALSE) {
      return FALSE;
    }
  }
  if (empty($branch_group_id)) {
    return drush_set_error('WK_BRANCH_NOT_FOUND', dt("Could not find the branch group called !branch.", array('!branch' => $branch_group)));
  }
  $office = drush_get_option('office', 'Web Minister');
  $office_id = NULL;
  $office_terms = array_keys(taxonomy_get_term_by_name($office, 'offices'));
  if (!empty($office_terms)) {
    $office_id = $office_terms[0];
  }
  else {
    $office_id = _drush_wk_select_term('offices', 'Select an office', $office);
    if ($office_id === FALSE) {
      return FALSE;
    }
  }
  if (empty($office_id)) {
    return drush_set_error('WK_OFFICE_NOT_FOUND', dt("Could not find the office !office.", array('!office' => $office)));
  }
  wk_assign_officer($user_info, $branch_group_id, $office_id);
}

function _drush_wk_select_term($vocabulary, $prompt, $filter = NULL) {
  $vocabularies = taxonomy_vocabulary_get_names();
  if (isset($vocabularies[$vocabulary])) {
    $vid = $vocabularies[$vocabulary]->vid;
  }
  $taxonomy_tree = taxonomy_get_tree($vid);
  //var_export($taxonomy_tree);
  $terms = array();
  foreach ($taxonomy_tree as $info) {
    if (empty($filter) || (stristr($info->name, $filter))) {
      $terms[$info->tid] = $info->name;
    }
  }
  //var_export($terms);
  if (count($terms) == 1) {
    $tids = array_keys($terms);
    return $tids[0];
  }
  if (!empty($terms)) {
    $choice = drush_choice($terms, $prompt);
    if (empty($choice)) {
      return drush_user_abort();
    }
    else {
      return $choice;
    }
  }
  return FALSE;
}

function drush_wk_select_submission($type, $filter = '', $field_map, $template, $prompt) {
  $submissions = array();
  $choices = array();
  $toc = entityform_get_submissions($type);
  foreach ($toc as $id => $info) {
    $submission = new EntityDrupalWrapper('entityform', $id);
    if ($submission) {
      $data = _wk_entity_to_data_array($submission, $field_map);
      $data = _wk_fix_up_submission_data($data);
      if (empty($filter) || drush_wk_find_value_in_array($filter, $data)) {
        $submissions[$id] = $submission;
        $choices[$id] = str_replace(array_keys($data), array_values($data), $template);
      }
    }
  }
  if (empty($choices)) {
    return drush_set_error('DRUSH_NO_RESULTS', dt("No matching submissions found."));
  }
  if (count($choices) == 1) {
    $choice = reset(array_keys($choices));
  }
  else {
    $choice = drush_choice($choices, $prompt);
  }

  return $submissions[$choice];
}
