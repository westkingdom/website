<?php
/**
 * Easy way of making sure a view is versioned.
 * added 6-3-2014
 * Implementation of hook_views_default_views().
 */
function regnum_views_default_views() {
  $view = new view();
  $view->name = 'regnum_listings';
  $view->description = '';
  $view->tag = 'default';
  $view->base_table = 'users';
  $view->human_name = 'Regnum Listing';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'Regnum List - All (Grouped)';
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'role';
  $handler->display->display_options['access']['role'] = array(
    4 => '4',
  );
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'some';
  $handler->display->display_options['pager']['options']['items_per_page'] = '50';
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['default_row_class'] = FALSE;
  $handler->display->display_options['style_options']['row_class_special'] = FALSE;
  $handler->display->display_options['style_options']['columns'] = array(
    'name' => 'name',
    'mail' => 'mail',
    'field_type_of_deputy' => 'field_type_of_deputy',
  );
  $handler->display->display_options['style_options']['default'] = 'field_type_of_deputy';
  $handler->display->display_options['style_options']['info'] = array(
    'name' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'mail' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'field_type_of_deputy' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
  );
  /* Relationship: OG membership: OG membership from User */
  $handler->display->display_options['relationships']['og_membership_rel']['id'] = 'og_membership_rel';
  $handler->display->display_options['relationships']['og_membership_rel']['table'] = 'users';
  $handler->display->display_options['relationships']['og_membership_rel']['field'] = 'og_membership_rel';
  /* Field: User: Name */
  $handler->display->display_options['fields']['name']['id'] = 'name';
  $handler->display->display_options['fields']['name']['table'] = 'users';
  $handler->display->display_options['fields']['name']['field'] = 'name';
  $handler->display->display_options['fields']['name']['label'] = '';
  $handler->display->display_options['fields']['name']['alter']['word_boundary'] = FALSE;
  $handler->display->display_options['fields']['name']['alter']['ellipsis'] = FALSE;
  /* Field: User: E-mail */
  $handler->display->display_options['fields']['mail']['id'] = 'mail';
  $handler->display->display_options['fields']['mail']['table'] = 'users';
  $handler->display->display_options['fields']['mail']['field'] = 'mail';
  $handler->display->display_options['fields']['mail']['element_default_classes'] = FALSE;
  /* Field: Field: Type of Deputy */
  $handler->display->display_options['fields']['field_type_of_deputy']['id'] = 'field_type_of_deputy';
  $handler->display->display_options['fields']['field_type_of_deputy']['table'] = 'field_data_field_type_of_deputy';
  $handler->display->display_options['fields']['field_type_of_deputy']['field'] = 'field_type_of_deputy';
  $handler->display->display_options['fields']['field_type_of_deputy']['relationship'] = 'og_membership_rel';
  $handler->display->display_options['fields']['field_type_of_deputy']['element_default_classes'] = FALSE;
  $handler->display->display_options['fields']['field_type_of_deputy']['type'] = 'text_plain';
  /* Filter criterion: User: Active */
  $handler->display->display_options['filters']['status']['id'] = 'status';
  $handler->display->display_options['filters']['status']['table'] = 'users';
  $handler->display->display_options['filters']['status']['field'] = 'status';
  $handler->display->display_options['filters']['status']['value'] = '1';
  $handler->display->display_options['filters']['status']['group'] = 1;
  $handler->display->display_options['filters']['status']['expose']['operator'] = FALSE;
  /* Filter criterion: Field: Type of Deputy (field_type_of_deputy) */
  $handler->display->display_options['filters']['field_type_of_deputy_value']['id'] = 'field_type_of_deputy_value';
  $handler->display->display_options['filters']['field_type_of_deputy_value']['table'] = 'field_data_field_type_of_deputy';
  $handler->display->display_options['filters']['field_type_of_deputy_value']['field'] = 'field_type_of_deputy_value';
  $handler->display->display_options['filters']['field_type_of_deputy_value']['relationship'] = 'og_membership_rel';
  $handler->display->display_options['filters']['field_type_of_deputy_value']['operator'] = 'not empty';

  /* Display: Page */
  $handler = $view->new_display('page', 'Page', 'page_1');
  $handler->display->display_options['defaults']['title'] = FALSE;
  $handler->display->display_options['title'] = 'Regnum List - Seneschals';
  $handler->display->display_options['defaults']['filter_groups'] = FALSE;
  $handler->display->display_options['defaults']['filters'] = FALSE;
  /* Filter criterion: User: Active */
  $handler->display->display_options['filters']['status']['id'] = 'status';
  $handler->display->display_options['filters']['status']['table'] = 'users';
  $handler->display->display_options['filters']['status']['field'] = 'status';
  $handler->display->display_options['filters']['status']['value'] = '1';
  $handler->display->display_options['filters']['status']['group'] = 1;
  $handler->display->display_options['filters']['status']['expose']['operator'] = FALSE;
  /* Filter criterion: Field: Type of Deputy (field_type_of_deputy) */
  $handler->display->display_options['filters']['field_type_of_deputy_value']['id'] = 'field_type_of_deputy_value';
  $handler->display->display_options['filters']['field_type_of_deputy_value']['table'] = 'field_data_field_type_of_deputy';
  $handler->display->display_options['filters']['field_type_of_deputy_value']['field'] = 'field_type_of_deputy_value';
  $handler->display->display_options['filters']['field_type_of_deputy_value']['relationship'] = 'og_membership_rel';
  $handler->display->display_options['filters']['field_type_of_deputy_value']['operator'] = 'contains';
  $handler->display->display_options['filters']['field_type_of_deputy_value']['value'] = 'Seneschal';
  $handler->display->display_options['path'] = 'regnum/listing-seneschal';

  /* Display: Page 2 */
  $handler = $view->new_display('page', 'Page 2', 'page_2');
  $handler->display->display_options['path'] = 'regnum/listing-all';

  $views[$view->name] = $view;
  return $views;
}