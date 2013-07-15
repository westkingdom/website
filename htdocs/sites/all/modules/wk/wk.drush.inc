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
      'branch' => 'The branch group for the office.',
      'office' => 'The office to assign the user to.',
      'title' => 'The custom title for the office.'
    ),
  );
  return $items;
}

function drush_wk_assign_officer($name) {
  $user_info = array(
    'name' => $name,
    'mail' => drush_get_option('mail', ''),
    'realname' => drush_get_option('realname', ''),
    'phone' => drush_get_option('phone', ''),
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
