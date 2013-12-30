<?php

// Put the logo path into JavaScript for the live preview.
drupal_add_js(array('color' => array('logo' => theme_get_setting('logo', 'bartik'))), 'setting');

$info = array(
  // Available colors and color labels used in theme.
  'fields' => array(
    'top' => t('Header top'),
    'bottom' => t('Header bottom'),
    'bg' => t('Main background'),
    'sidebar' => t('Sidebar background'),
    'sidebarborders' => t('Sidebar borders'),
    'footer' => t('Footer background'),
    'titleslogan' => t('Title and slogan'),
    'text' => t('Text color'),
    'link' => t('Link color'),
    'multiday' => t('Calendar multi-day event'),
    'continuation' => t('Calendar continuation'),
    'singleday' => t('Calendar single-day event'),
    'meetingdemo' => t('Calendar meeting or demo'),
    'calendartext' => t('Calendar text'),
  ),
  // Pre-defined color schemes.
  'schemes' => array(
    'default' => array(
      'title' => t('Azure and Argent (default)'),
      'colors' => array(
        'top' => '#0779bf',
        'bottom' => '#48a9e4',
        'bg' => '#ffffff',
        'sidebar' => '#f6f6f2',
        'sidebarborders' => '#f9f9f9',
        'footer' => '#292929',
        'titleslogan' => '#fffeff',
        'text' => '#3b3b3b',
        'link' => '#0071B3',
        'multiday' => '#699b73',
        'continuation' => '#4d7255',
        'singleday' => '#81bd8c',
        'meetingdemo' => '#caffce',
        'calendartext' => '#020202',
      ),
    ),
    'kingdom' => array(
      'title' => t('Kingdom Or and Vert'),
      'colors' => array(
        'top' => '#ffc43a',
        'bottom' => '#fde1a0',
        'bg' => '#ffffff',
        'sidebar' => '#f6f6f2',
        'sidebarborders' => '#f9f9f9',
        'footer' => '#1f1d1c',
        'titleslogan' => '#fffeff',
        'text' => '#3b3b3b',
        'link' => '#548c36',
        'multiday' => '#ffee99',
        'continuation' => '#f0e090',
        'singleday' => '#ffffaa',
        'meetingdemo' => '#ffffdd',
        'calendartext' => '#020202',
      ),
    ),
    'gray' => array(
      'title' => t('Argent and Gray'),
      'colors' => array(
        'top' => '#798790',
        'bottom' => '#c6d7e2',
        'bg' => '#ffffff',
        'sidebar' => '#ffffff',
        'sidebarborders' => '#cccccc',
        'footer' => '#24272c',
        'titleslogan' => '#fffeff',
        'text' => '#4a4a4a',
        'link' => '#43738f',
        'multiday' => '#699b73',
        'continuation' => '#4d7255',
        'singleday' => '#81bd8c',
        'meetingdemo' => '#caffce',
        'calendartext' => '#020202',
      ),
    ),
  ),

  // CSS files (excluding @import) to rewrite with new color scheme.
  'css' => array(
    'css/colors.css',
  ),

  // Files to copy.
  'copy' => array(
    'logo.png',
  ),

  // Gradient definitions.
  'gradients' => array(
    array(
      // (x, y, width, height).
      'dimension' => array(0, 0, 0, 0),
      // Direction of gradient ('vertical' or 'horizontal').
      'direction' => 'vertical',
      // Keys of colors to use for the gradient.
      'colors' => array('top', 'bottom'),
    ),
  ),

  // Color areas to fill (x, y, width, height).
  'fill' => array(),

  // Coordinates of all the theme slices (x, y, width, height)
  // with their filename as used in the stylesheet.
  'slices' => array(),

  // Reference color used for blending. Matches the base.png's colors.
  'blend_target' => '#ffffff',

  // Preview files.
  'preview_css' => 'color/preview.css',
  'preview_js' => 'color/preview.js',
  'preview_html' => 'color/preview.html',

  // Base file for image generation.
  'base_image' => 'color/base.png',
);
