<?php

/**
 * @file
 * API definitions.
 */

/**
 * Define object permission override handlers.
 *
 * @return array
 *   An associative array mapping permission names to an indexed array of
 *   associative arrays, each containing:
 *   - callable: The name of a callable implementing
 *     callback_islandora_access_override_object_handler().
 *   - file: A file to be loaded, if necessary.
 */
function hook_islandora_access_override_object_handlers() {
  $handlers = array();

  $handlers[ISLANDORA_VIEW_OBJECTS] = array(
    array(
      'callable' => 'my_awesome_object_handler',
    ),
  );

  return $handlers;
}

/**
 * Define datastream permission override handlers.
 *
 * @return array
 *   An associative array mapping permission names to an indexed array of
 *   associative arrays, each containing:
 *   - callable: The name of a callable implementing
 *     callback_islandora_access_override_datastream_handler()
 *   - file: A file to be loaded, if necessary.
 */
function hook_islandora_access_override_datastream_handlers() {
  $handlers = array();

  $handlers[ISLANDORA_VIEW_OBJECTS] = array(
    array(
      'callable' => 'my_awesome_datastream_handler',
    ),
  );

  return $handlers;
}

/**
 * The signature required by the object handler callable.
 *
 * @param string $op
 *   The Drupal permission being checked.
 * @param AbstractObject $object
 *   The object for which to check.
 * @param object $user
 *   A loaded user object for which to check the permission.
 *
 * @return bool|null
 *   TRUE to allow, FALSE to forbid, NULL to make no assertion.
 */
function callback_islandora_access_override_object_handler($op, AbstractObject $object, $user) {
  return NULL;
}

/**
 * The signature required by the datastream handler callable.
 *
 * @param string $op
 *   The Drupal permission being checked.
 * @param AbstractDatastream $datastream
 *   The datastream for which to check.
 * @param object $user
 *   A loaded user object for which to check the permission.
 *
 * @return bool|null
 *   TRUE to allow, FALSE to forbid, NULL to make no assertion.
 */
function callback_islandora_access_override_datastream_handler($op, AbstractDatastream $datastream, $user) {
  return NULL;
}
