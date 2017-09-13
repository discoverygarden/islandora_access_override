<?php

/**
 * @file
 * Database helper functions.
 */

/**
 * Get the proxies for a given PID.
 */
function islandora_owner_permissions_get_proxies_for_pid($pid) {
  $magic = db_transaction();
  $pid_id = islandora_owner_permissions_get_pid_id($pid);
  return db_select('islandora_owner_permissions_object_proxies', 'proxies')
    ->fields('proxies', array('uid'))
    ->condition('object', $pid_id)
    ->execute()
    ->fetchAllAssoc('uid', PDO::FETCH_ASSOC);
}

/**
 * Add a proxy to a given PID.
 */
function islandora_owner_permissions_add_proxy_to_pid($pid, $user_name) {
  $magic = db_transaction();
  $user = user_load_by_name($user_name);
  $pid_id = islandora_owner_permissions_get_pid_id($pid);
  return db_insert('islandora_owner_permissions_object_proxies')
    ->fields(array('object' => $pid_id, 'uid' => $user->uid))
    ->execute();
}

/**
 * Remove a proxy from a given PID.
 */
function islandora_owner_permissions_remove_proxy_from_pid($pid, $uid) {
  $magic = db_transaction();
  $pid_id = islandora_owner_permissions_get_pid_id($pid);
  db_delete('islandora_owner_permissions_object_proxies')
    ->condition('uid', $uid)
    ->condition('object', $pid_id)
    ->execute();
}

/**
 * Remove a user from the db.
 */
function islandora_owner_permissions_remove_user_from_db($uid) {
  db_delete('islandora_owner_permissions_object_proxies')
    ->condition('uid', $uid)
    ->execute();
}

/**
 * Remove an object from the db.
 */
function islandora_owner_permissions_remove_object_from_db($pid) {
  $magic = db_transaction();
  $pid_id = islandora_owner_permissions_get_pid_id($pid);
  db_delete('islandora_owner_permissions_objects')
    ->condition('id', $pid_id)
    ->execute();
  db_delete('islandora_owner_permissions_object_proxies')
    ->condition('object', $pid_id)
    ->execute();
}

/**
 * Gets the database id related to a PID.
 *
 * Sets one if it doesn't exist.
 */
function islandora_owner_permissions_get_pid_id($pid) {
  $magic = db_transaction();
  $id = islandora_owner_permissions_pid_id($pid);
  // If no matches, create one.
  if (!$id) {
    return islandora_owner_permissions_pid_to_db($pid);
  }
  return $id;
}

/**
 * Creates/gets the database id related to a PID.
 *
 * @throws Exception
 *   If the object cannot be loaded.
 */
function islandora_owner_permissions_pid_to_db($pid) {
  $object = islandora_object_load($pid);
  if ($object) {
    return db_insert('islandora_owner_permissions_objects')
      ->fields(array('pid' => $pid))
      ->execute();
  }
  else {
    throw new Exception("Could not load '{$pid}' as Islandora object");
  }
}

/**
 * Gets the database id related to a PID.
 */
function islandora_owner_permissions_pid_id($pid) {
  $result = db_select('islandora_owner_permissions_objects')
    ->fields('islandora_owner_permissions_objects', array('id'))
    ->condition('pid', $pid)
    ->execute();
  $record = $result->fetchAssoc();
  return isset($record['id']) ? $record['id'] : FALSE;
}
