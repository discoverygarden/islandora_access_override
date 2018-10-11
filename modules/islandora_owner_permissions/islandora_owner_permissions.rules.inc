<?php

/**
 * @file
 * Rules hook implementations.
 */

/**
 * Implements hook_rules_data_info().
 */
function islandora_owner_permissions_rules_action_info() {
  $actions = array();
  $actions['islandora_owner_permissions_get_proxy_owners'] = array(
    'label' => t('Get Proxy Owners'),
    'group' => t('Islandora Owner Permissions'),
    'parameter' => array(
      'object' => array(
        'type' => 'islandora_object',
        'label' => t('Object'),
        'description' => t('The object to get the proxy owners from.'),
      ),
    ),
    'provides' => array(
      'owners' => array(
        'type' => 'list<user>',
        'label' => t('Proxy Owners'),
      ),
    ),
  );
  return $actions;
}

/**
 * Gets a list of loaded proxy owners for an object.
 *
 * @param AbstractObject $object
 *   The object to get proxy owners for.
 *
 * @return stdClass[]
 *   An array containing any loaded proxy users.
 */
function islandora_owner_permissions_get_proxy_owners(AbstractObject $object) {
  module_load_include('inc', 'islandora_owner_permissions', 'includes/db');
  $mapper = function ($proxy) {
    $user_object = user_load($proxy['uid']);
    if (!$user_object) {
      watchdog('islandora_owner_permissions',
        'Failed to load user @uid; this user will not be available for rules actions using proxy ownership.',
        array('@uid' => $proxy['uid']),
        WATCHDOG_WARNING);
      return FALSE;
    }
    return $user_object;
  };
  return array(
    'owners' => array_filter(array_map($mapper, islandora_owner_permissions_get_proxies_for_pid($object->id))),
  );
}
