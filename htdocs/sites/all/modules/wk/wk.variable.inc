<?php

/**
 * Implements hook_variable_info().
 */
function wk_variable_info($options) {
  // Declare our branch footer variable.
  $variable['wk_branch_footer'] = array(
    'title' => t('Branch footer'),
    'description' => t('Message to display in footer of Branch Group site.'),
    'token' => TRUE, // Produce tokens automatically
  );

  return $variable;
}
