<?php

/**
 * Implements hook_variable_info().
 */
function wk_variable_info($options) {
  $variable['wk_branch_seneschal'] = array(
    'title' => t('Branch Seneschal'),
    'description' => t('Officer node for the branch group Seneschal. Should be filled in automatically; do not change.'),
    'type' => 'string',
    'token' => TRUE, // Produce tokens automatically
    'format callback' => 'variable_format_text', // Critical if you want to avoid wrapping token value in <pre> tags
    // 'allowed tags' => ... // More control over filter_xss in format text
    'multidomain' => TRUE, // Edit with domain_variable
    'group' => 'site_information', // Group with other site information
  );
  $variable['wk_branch_webminister'] = array(
    'title' => t('Branch Web Minister'),
    'description' => t('Officer node for the branch group Web Minister. Should be filled in automatically; do not change.'),
    'type' => 'string',
    'token' => TRUE, // Produce tokens automatically
    'format callback' => 'variable_format_text', // Critical if you want to avoid wrapping token value in <pre> tags
    // 'allowed tags' => ... // More control over filter_xss in format text
    'multidomain' => TRUE, // Edit with domain_variable
    'group' => 'site_information', // Group with other site information
  );
  $variable['wk_branch_chatalain'] = array(
    'title' => t('Branch Chatalain'),
    'description' => t('Officer node for the branch group Chatalain. Should be filled in automatically; do not change.'),
    'type' => 'string',
    'token' => TRUE, // Produce tokens automatically
    'format callback' => 'variable_format_text', // Critical if you want to avoid wrapping token value in <pre> tags
    // 'allowed tags' => ... // More control over filter_xss in format text
    'multidomain' => TRUE, // Edit with domain_variable
    'group' => 'site_information', // Group with other site information
  );
  // Declare our branch footer variable.
  $variable['wk_branch_footer'] = array(
    'title' => t('Branch footer'),
    'description' => t('Message to display in footer of Branch Group site.'),
    'type' => 'text',
    'token' => TRUE, // Produce tokens automatically
    'format callback' => 'variable_format_text', // Critical if you want to avoid wrapping token value in <pre> tags
    // 'allowed tags' => ... // More control over filter_xss in format text
    'multidomain' => TRUE, // Edit with domain_variable
    'group' => 'site_information', // Group with other site information
  );
  $variable['wk_branch_location'] = array(
    'title' => t('Branch location'),
    'description' => t('Mundane location (city, county, etc.) that describes the area covered by the local branch group.'),
    'type' => 'string',
    'token' => TRUE, // Produce tokens automatically
    'format callback' => 'variable_format_text', // Critical if you want to avoid wrapping token value in <pre> tags
    // 'allowed tags' => ... // More control over filter_xss in format text
    'multidomain' => TRUE, // Edit with domain_variable
    'group' => 'site_information', // Group with other site information
  );

  return $variable;
}
