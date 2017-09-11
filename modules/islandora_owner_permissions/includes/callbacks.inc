<?php

/**
 * @file
 * Callback implementations.
 */

/**
 * Implements callback_islandora_access_override_object_handler().
 *
 * Really, just gather the results of two other calls.
 */
function islandora_ownership_permissions_test_object($op, $object, $user) {
  if ($object instanceof AbstractDatastream) {
    dd(debug_backtrace());
    throw new UnexpectedValueException();
  }
  return array(
    _islandora_ownership_permissions_test_owner($op, $object, $user),
    _islandora_ownership_permissions_test_proxies($op, $object, $user),
  );
}

/**
 * Implements callback_islandora_access_override_object_handler().
 *
 * If the given user appears to be the object's owner, grant them the specified
 * permission (if we map it).
 */
function _islandora_ownership_permissions_test_owner($op, AbstractObject $object, $user) {
  module_load_include('inc', 'islandora_owner_permissions', 'includes/utilities');
  $perm_map = islandora_owner_permissions_permission_map();

  if (!isset($perm_map[$op]) || !isset($user->name) || $user->name != $object->owner || !islandora_object_access($perm_map[$op], $object, $user)) {
    // No mapping for op, no user name, mismatched user, or no access... make no
    // assertion (implicit denial, if nothing else allows).
    return NULL;
  }

  return TRUE;
}

/**
 * Implements callback_islandora_access_override_object_handler().
 *
 * If the given user appears to be one of the object's proxy users, grant them
 * the specified permission (if we map it).
 */
function _islandora_ownership_permissions_test_proxies($op, AbstractObject $object, $user) {
  module_load_include('inc', 'islandora_owner_permissions', 'includes/utilities');
  module_load_include('inc', 'islandora_owner_permissions', 'includes/db');
  $perm_map = islandora_owner_permissions_proxy_permission_map();
  $proxies = islandora_owner_permissions_get_proxies_for_pid($object->id);

  if (!isset($perm_map[$op]) || !isset($user->uid) || !array_key_exists($user->uid, $proxies) || !islandora_object_access($perm_map[$op], $object, $user)) {
    // No mapping for op, no user id, mismatched user, or no access... make no
    // assertion (implicit denial, if nothing else allows).
    return NULL;
  }

  return TRUE;
}
