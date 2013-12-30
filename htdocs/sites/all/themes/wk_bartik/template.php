<?php

function wk_bartik_preprocess_html(&$variables) {
  if (theme_get_setting('page_style') != 'borderless') {
    $variables['classes_array'][] = 'page-with-background';
  }
  else {
    $variables['classes_array'][] = 'borderless-page';
  }
}

/**
 * Implementation of hook_preprocess_calendar_item().
 */
function wk_bartik_preprocess_calendar_item(&$vars) {
  // At the last possible minute we fix the values in rendered_fields so it
  // contains the correct rendered content for the type of item and item display.
  $item = $vars['item'];
  $event_type = $item->entity->field_event_type[LANGUAGE_NONE][0]['value'];
  $item->rendered = "<div class='calendar-event-type--$event_type'>" . $item->rendered . "</div>";

  if (!empty($item->rendered) && empty($item->is_multi_day)) {
    $vars['rendered_fields'] = array($item->rendered);
  }
}
