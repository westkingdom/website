<?php

/**
 * @file
 * Bulk export of default_corresponding_node_references_presets objects generated by Bulk export module.
 */

/**
 * Implements hook_default_corresponding_node_references().
 */
function wk_config_default_corresponding_node_references() {
  $cnr_objs = array();

  $cnr_obj = new stdClass();
  $cnr_obj->disabled = FALSE; /* Edit this to true to make a default cnr_obj disabled initially */
  $cnr_obj->api_version = 1;
  $cnr_obj->node_types_content_fields = 'cal_event*field_event_site*location*field_site_events';
  $cnr_obj->enabled = 1;
  $cnr_objs['cal_event*field_event_site*location*field_site_events'] = $cnr_obj;

  return $cnr_objs;
}
