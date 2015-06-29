<?php

function wk_bartik_form_system_theme_settings_alter(&$form, $form_state) {
  $form['wk_bartik'] = array(
    '#type' => 'fieldset',
    '#title' => t('Page border style'),
    '#description' => t('Choose a style for the page borders.'),
    '#attributes' => array('class' => array('bartik-settings')),
  );
  $form['wk_bartik']['page_style'] = array(
    '#type'          => 'radios',
    '#title'         => t('Border'),
    '#default_value' => theme_get_setting('page_style') ? theme_get_setting('page_style') : 'page',
    '#options' => array(
      'page' => t('Page - regions centered on a page over a background pattern.'),
      'borderless' => t('Borderless - region backgrounds stretch to edge of window.'),
    ),
  );
}
