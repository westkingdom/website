<?php

use Symfony\Component\Yaml\Yaml;

/**
 * @file
 * Contains Behat Drush command file, for use by the Behat Drush Extension.
 */

/**
 * Implements hook_drush_command().
 */
function behat_drush_command() {
  $items = array();

  $items['behat'] = array(
    'description' => "Drush command behat.",
    // Describe the arguments for this command.  Delete
    // this seciton if command takes no arguments.
    'arguments' => array(
      'operation' => 'Behat operation, e.g. create-node.',
      'data' => 'Operation data in json format.',
    ),
    // Command options.
    'options' => array(
      'file' => 'Path to file to load data from.',
    ),

    // Give one or more example commandline usages for this command.
    'examples' => array(
      "drush behat create-node '{\"title\":\"Example page\",\"type\":\"page\"}'" => 'Create a page with the title "Example page".',
    ),
    // Default output is json, --pipe produces yml.
    'outputformat' => array(
      'default' => 'json',
      'pipe-format' => 'yaml',
    ),
  );

  return $items;
}

/**
 * Implements hook_drush_help().
 */
function behat_drush_help($section) {
  switch ($section) {
    case 'drush:behat':
      return dt("The Drush Behat command file must be installed in the target Drupal site in order to use more advanced features such as creating nodes and taxonomy terms.");
  }
}

/**
 * Determines whether or not the provided data is
 * in json format.
 */
function _drush_behat_is_json($provided_data) {
  $provided_data = trim($provided_data);
  switch($provided_data[0]) {
    case "'":
    case '"':
    case '[':
    case '{':
      return TRUE;
    default:
      return FALSE;
  }
}

/**
 * Implements drush_hook_COMMAND().
 */
function drush_behat($operation, $provided_data = "") {
  // Data may be provided either via --file, or via the second parameter.
  $file = drush_get_option('file');
  if ($file) {
    $provided_data = file_get_contents($file);
  }
  // Provided data may be either json or yaml.
  if (_drush_behat_is_json($provided_data)) {
    $decoded_data = json_decode($provided_data);
  }
  else {
    $decoded_data = Yaml::parse($provided_data);
  }

  return _drush_behat_process($operation, $decoded_data);
}

/**
 * Process an operation.
 */
function _drush_behat_process($operation, $decoded_data) {
  // Dispatch if the operation exists.
  $fn = 'drush_behat_op_' . strtr($operation, '-', '_');
  drush_print("process function is $fn");
  if (function_exists($fn)) {
    return $fn($decoded_data);
  }
  else {
    return drush_set_error('DRUSH_BEHAT_NO_OPERATION', dt("Operation '!op' unknown", array('!op' => $operation)));
  }
}

/**
 * Import multiple items of different kinds.
 */
function drush_behat_op_import($import_data) {
  var_export($import_data);
  foreach ((array) $import_data as $operation => $data_list) {
    print("\n\n$operation\n");
    $data_list = _drush_behat_expand_data($operation, $data_list);
    var_export($data_list);
    foreach ($data_list as $data) {
      _drush_behat_process($operation, $data);
    }
  }
}

/**
 * By default, the $data_list is just a list of items
 * to create.  For some item types, though, the data
 * might be in a different format.
 * @see drush_behat_expand_data_create_term()
 */
function _drush_behat_expand_data($operation, $data_list) {
  $fn = 'drush_behat_expand_data_' . strtr($operation, '-', '_');
  if (function_exists($fn)) {
    return $fn($data_list);
  }
  else {
    return $data_list;
  }
}

/**
 * Expand data for taxonomy term imports.  Each list
 * of terms should appear underneath its respective
 * vocabulary name.  This parent key is copied into
 * the 'vocabulary_machine_name' for each term.
 */
function drush_behat_expand_data_create_term($data_list) {
  $result = array();
  foreach ($data_list as $vocabulary_name => $terms) {
    foreach ($terms as $term) {
      $term['vocabulary_machine_name'] = $vocabulary_name;
      $result[] = $term;
    }
  }
  return $result;
}

/**
 * Create a node.
 */
function drush_behat_op_create_node($node) {
  $node = (object) $node;
  // Set original if not set.
  if (!isset($node->original)) {
    $node->original = clone $node;
  }

  // Assign authorship if none exists and `author` is passed.
  if (!isset($node->uid) && !empty($node->author) && ($user = user_load_by_name($node->author))) {
    $node->uid = $user->uid;
  }

  // Convert properties to expected structure.
  _drush_behat_expand_entity_properties($node);

  // Attempt to decipher any fields that may be specified.
  _drush_behat_expand_entity_fields('node', $node);

  // Set defaults that haven't already been set.
  $defaults = clone $node;
  node_object_prepare($defaults);
  $node = (object) array_merge((array) $defaults, (array) $node);

  node_save($node);
  return (array) $node;
}

/**
 * Delete a node.
 */
function drush_behat_op_delete_node($node) {
  $node = (object) $node;
  node_delete($node->nid);
}

/**
 * Create a taxonomy term.
 */
function drush_behat_op_create_term($term) {
  $term = (object) $term;
  // Map vocabulary names to vid, these take precedence over machine names.
  if (!isset($term->vid)) {
    $vocabularies = \taxonomy_get_vocabularies();
    foreach ($vocabularies as $vid => $vocabulary) {
      if ($vocabulary->name == $term->vocabulary_machine_name) {
        $term->vid = $vocabulary->vid;
      }
    }
  }

  if (!isset($term->vid)) {
    // Try to load vocabulary by machine name.
    $vocabularies = \taxonomy_vocabulary_load_multiple(FALSE, array(
      'machine_name' => $term->vocabulary_machine_name,
    ));
    if (!empty($vocabularies)) {
      $vids = array_keys($vocabularies);
      $term->vid = reset($vids);
    }
  }

  // If `parent` is set, look up a term in this vocab with that name.
  if (isset($term->parent)) {
    $parent = \taxonomy_get_term_by_name($term->parent, $term->vocabulary_machine_name);
    if (!empty($parent)) {
      $parent = reset($parent);
      $term->parent = $parent->tid;
    }
  }

  if (empty($term->vid)) {
    throw new \Exception(sprintf('No "%s" vocabulary found.'));
  }

  // Attempt to decipher any fields that may be specified.
  _drush_behat_expand_entity_fields('taxonomy_term', $term);

  \taxonomy_term_save($term);
  var_export($term);

  return $term;
}

/**
 * Delete a taxonomy term.
 */
function drush_behat_op_delete_term(\stdClass $term) {
  $term = (object) $term;
  var_export($term);
  $status = 0;
  if (isset($term->tid)) {
    $status = \taxonomy_term_delete($term->tid);
  }
  // Will be SAVED_DELETED (3) on success.
  return $status;
}

/**
 * Expands properties on the given entity object to the expected structure.
 *
 * @param \stdClass $entity
 *   The entity object.
 *
 * @see Drupal\Driver\Cores\Drupal7\expandEntityProperties
 */
function _drush_behat_expand_entity_properties(\stdClass $entity) {
  // The created field may come in as a readable date, rather than a
  // timestamp.
  if (isset($entity->created) && !is_numeric($entity->created)) {
    $entity->created = strtotime($entity->created);
  }

  // Map human-readable node types to machine node types.
  $types = \node_type_get_types();
  foreach ($types as $type) {
    if ($entity->type == $type->name) {
      $entity->type = $type->type;
      continue;
    }
  }
}

/**
 * Expands properties on the given entity object to the expected structure.
 *
 * @param \stdClass $entity
 *   Entity object.
 *
 * @see Drupal\Driver\Cores\AbstractCore\expandEntityFields
 */
function _drush_behat_expand_entity_fields($entity_type, \stdClass $entity) {
  $field_types = _drush_behat_get_entity_field_types($entity_type);
  foreach ($field_types as $field_name => $type) {
    if (isset($entity->$field_name)) {
      $entity->$field_name = _drush_behat_get_field_handler($entity, $entity_type, $field_name)
        ->expand($entity->$field_name);
    }
  }
}

/**
 * Converts a seried of words into camel case.
 *
 * @see Symfony\Component\DependencyInjection\Container\camelize
 */
function _drush_behat_camelize($id) {
  return strtr(ucwords(strtr($id, array('_' => ' ', '.' => '_ ', '\\' => '_ '))), array(' ' => ''));
}

/**
 * Get the field handler for the specified field of the specified entity.
 *
 * Note that this function instantiates a field handler class that is
 * provided by the Behat Drush Driver.  In order for this to work, an
 * appropriate autoload.inc file must be included.  This will be done
 * automatically if the Drupal site is managed by Composer, and requires
 * the Behat Drush Driver in its composer.json file.
 *
 * @see Drupal\Driver\Cores\AbstractCore\getFieldHandler
 */
function _drush_behat_get_field_handler($entity, $entity_type, $field_name) {
  $core_namespace = "Drupal7";
  $field_types = _drush_behat_get_entity_field_types($entity_type);
  $camelized_type = _drush_behat_camelize($field_types[$field_name]);
  $default_class = sprintf('\Drupal\Driver\Fields\%s\DefaultHandler', $core_namespace);
  $class_name = sprintf('\Drupal\Driver\Fields\%s\%sHandler', $core_namespace, $camelized_type);
  if (class_exists($class_name)) {
    return new $class_name($entity, $entity_type, $field_name);
  }
  return new $default_class($entity, $entity_type, $field_name);
}

/**
 * Get all of the field attached to the specified entity type.
 *
 * @see Drupal\Driver\Cores\Drupal7\getEntityFieldTypes in Behat
 */
function _drush_behat_get_entity_field_types($entity_type) {
  $return = array();
  $fields = field_info_field_map();
  foreach ($fields as $field_name => $field) {
    if (array_key_exists($entity_type, $field['bundles'])) {
      $return[$field_name] = $field['type'];
    }
  }
  return $return;
}
