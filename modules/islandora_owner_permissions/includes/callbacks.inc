<?php

/**
 * @file
 * Callback implementations.
 */

/**
 * Implements callback_islandora_access_override_object_handler().
 */
function islandora_ownership_permissions_test_object($op, AbstractObject $object, $user) {
  module_load_include('inc', 'islandora_owner_permissions', 'includes/utilities');
  $perm_map = islandora_owner_permissions_permission_map();

  if (!isset($perm_map[$op]) || !isset($user->name) || $user->name != $object->owner || !user_access($perm_map[$op], $user)) {
    // No mapping for op, no user name, mismatched user, or no access... make no
    // assertion (implicit denial, if nothing else allows).
    return NULL;
  }

  return TRUE;
}
