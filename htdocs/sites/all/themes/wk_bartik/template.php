<?php

function wk_bartik_preprocess_html(&$variables) {
  if (theme_get_setting('page_style', FALSE)) {
    $variables['classes_array'][] = 'page-style';
  }
  else {
    $variables['classes_array'][] = 'borderless-page';
  }
}

function wk_bartik_process_page(&$variables) {
  $variables['title_aux_info'] = '';
  if (isset($variables['node'])) {
    $fn = 'wk_bartik_process_page__' . $variables['node']->type;
    if (function_exists($fn)) {
      $fn($variables);
    }
  }
}

function wk_bartik_process_page__cal_event(&$variables) {
    $variables['title_prefix']['#markup'] = '<b>YES:</b>' . var_export(array_keys($variables), TRUE);
  $node = $variables['node'];
  // Output device arms before the page title
  $output = '';
  foreach ($node->taxonomy_vocabulary_2[LANGUAGE_NONE] as $index => $term) {
    if (isset($term['taxonomy_term'])) {
      $view = field_view_field('taxonomy_term', $term['taxonomy_term'], 'field_arms');
      if (!empty($view)) {
        $output .= render($view);
      }
    }
  }
  if (!empty($output)) {
    $variables['title_prefix']['#markup'] = "
<div class='calendar-event-arms'>
  <div class='left-arms'>$output</div>
  <div class='right-arms'>$output</div>
</div>";
  }
  // Output additional information about the event below the page title
  $cal_event_date = array();
  $cal_event_location_long_summary_markup = '';
  if (isset($node->field_date[LANGUAGE_NONE][0]['rrule'])) {
    // Repeating events:  special checking for "weekly", then make our
    // own formatted string.  Question: could we do this with a date format?
    if (substr($node->field_date[LANGUAGE_NONE][0]['rrule'], 0, 28) == "RRULE:FREQ=WEEKLY;INTERVAL=1") {
      // See http://drupal.org/node/1108164 for other options.
      $date = new DateObject($node->field_date[LANGUAGE_NONE][0]['value'],'UTC',DATE_FORMAT_ISO);
      $cal_event_date = array('#markup' => '<div class="field-name-field-date">' . format_date($date->format('U'),'custom','\E\v\e\r\y l \a\t g:ia') . '</div>');
    }
  }
  else {
    // Non-repeating dates:  just use the default view mode, and print the
    // date with the format defined for 'field_date'.
    $cal_event_date = field_view_field('node', $node, 'field_date', 'default');
  }
  $variables['cal_event_date'] = $cal_event_date;
  $variables['cal_event_branch'] = field_view_field('node', $node, 'taxonomy_vocabulary_2', 'default'); // $node->taxonomy_vocabulary_2;
  if (isset($node->field_event_site[LANGUAGE_NONE][0]['node']) && isset($node->field_event_site[LANGUAGE_NONE][0]['node']->field_location[LANGUAGE_NONE][0])) {
    $city = $node->field_event_site[LANGUAGE_NONE][0]['node']->field_location[LANGUAGE_NONE][0]['city'];
    $province = $node->field_event_site[LANGUAGE_NONE][0]['node']->field_location[LANGUAGE_NONE][0]['province'];
    $province_name = $node->field_event_site[LANGUAGE_NONE][0]['node']->field_location[LANGUAGE_NONE][0]['province_name'];
    if (!empty($city) && !empty($province)) {
      $variables['cal_event_location_summary'] = '(' . $city . ', ' . $province . ')';
      $variables['cal_event_location_long_summary'] =  $city . ', ' . $province_name;
      $cal_event_location_long_summary_markup = '<div class="cal-event-location-long-summary">' . $variables['cal_event_location_long_summary'] . '</div>';
    }
  }
  $variables['title_aux_info']['#markup'] = '
  <div class="event-info">
    ' . render($cal_event_date) . $cal_event_location_long_summary_markup . render($variables['cal_event_branch']) . '
  </div>
';

}

/**
 * Override or insert variables into the node template.
 */
function wk_bartik_preprocess_node(&$variables) {
  $fn = 'wk_bartik_preprocess_node__' . $variables['type'];
  if (function_exists($fn)) {
    $fn($variables);
  }
}

function wk_bartik_preprocess_node__cal_event(&$variables) {
  $node = $variables['node'];
}

