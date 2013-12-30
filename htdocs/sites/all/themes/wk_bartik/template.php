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
  static $outp = FALSE;
  // At the last possible minute we fix the values in rendered_fields so it
  // contains the correct rendered content for the type of item and item display.
  $item = $vars['item'];

  $branch_output = '';
  $branch_group_terms=FALSE;
  if (isset($item->entity->taxonomy_vocabulary_2[LANGUAGE_NONE])) {
    $branch_group_terms = $item->entity->taxonomy_vocabulary_2[LANGUAGE_NONE];
    foreach ($branch_group_terms as $index => $term_info) {
      if (isset($term_info['tid'])) {
        $term = taxonomy_term_load($term_info['tid']);
        if (isset($term->field_arms[LANGUAGE_NONE])) {
          $img_uri = $term->field_arms[LANGUAGE_NONE][0]['uri'];
          $image_path =  file_create_url($img_uri);
          $image_path =  parse_url($image_path, PHP_URL_PATH);
          $branch_output .= "<img src='$image_path'>";
        }
        /*
        // The "drupal way" would be to replace the code above
        // and use field_view_field instead, as shown below,
        // but we don't want all of the markup this produces,
        // and stripping the markup off the img tags is inelegant.
        $view = field_view_field('taxonomy_term', $term, 'field_arms');
        if (!empty($view)) {
          $branch_output .= render($view);
        }
        */
      }
    }
  }

  if (!empty($item->rendered) && empty($item->is_multi_day)) {
    if (!empty($branch_output)) {
      $item->rendered = "<div class='calendar-event-branch'>$branch_output</div>" . $item->rendered;
    }
    $vars['rendered_fields'] = array($item->rendered);
  }
  if (!empty($item->is_multi_day) && empty($item->continuation) && !empty($vars['rendered_fields'])) {
    array_unshift($vars['rendered_fields'], "<div class='calendar-event-branch'>$branch_output</div>");
    if (isset($vars['rendered_fields']['province'])) {
      $vars['rendered_fields']['province'] = "<br>" . $vars['rendered_fields']['province'];
    }
  }
  if (!empty($vars['rendered_fields'])) {
    $event_type = $item->entity->field_event_type[LANGUAGE_NONE][0]['value'];
    array_unshift($item->rendered_fields, "<div class='calendar-event-type--$event_type'>");
    $item->rendered_fields[] = "</div>";
  }
}
