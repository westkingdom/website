<?php

/**
 * @file
 * Configuration file for Drupal's multi-site directory aliasing feature.
 *
 * If doing local development, create a folder
 * 'sites/local', and place a settings.php and files
 * folder there, for the local test system used with
 * `drush runserver :8888 /`
 *
 * @see default.settings.php
 * @see conf_path()
 * @see http://drupal.org/documentation/install/multi-site
 */
$sites['8888.localhost'] = 'local';
$sites['8888.127.0.0.1'] = 'local';
