<?php

function wk_bartik_preprocess_html(&$variables) {
  if (theme_get_setting('page_style') != 'borderless') {
    $variables['classes_array'][] = 'page-with-background';
  }
  else {
    $variables['classes_array'][] = 'borderless-page';
  }
}

