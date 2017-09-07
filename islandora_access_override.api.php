<?php

/**
 * @file
 * API definitions.
 */

/**
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
 * @return bool|null
 *   TRUE to allow, FALSE to forbid, NULL to make no assertion.
 */
function callback_islandora_access_override_object_handler($op, AbstractObject $object, $user) {
}

/**
 * @return bool|null
 *   TRUE to allow, FALSE to forbid, NULL to make no assertion.
 */
function callback_islandora_access_override_datastream_handler($op, AbstractDatastream $datastream, $user) {
}
