<?php

/**
 * @file
 * API definitions.
 */

/**
 * @return array
 *   An associative array mapping permission names to an indexed array of
 *   associative arrays, each containing:
 *   - class: The name of a class implementing
 *     IslandoraAccessOverrideObjectHandlerInterface.
 */
function hook_islandora_access_override_object_handlers() {
  $handlers = array();

  $handlers[ISLANDORA_VIEW_OBJECTS] = array(
    array(
      'class' => 'MyAwesomeObjectHandler',
    ),
  );

  return $handlers;
}

/**
 * @return array
 *   An associative array mapping permission names to an indexed array of
 *   associative arrays, each containing:
 *   - class: The name of a class implementing
 *     IslandoraAccessOverrideDatastreamHandlerInterface.
 */
function hook_islandora_access_override_datastream_handlers() {
  $handlers = array();

  $handlers[ISLANDORA_VIEW_OBJECTS] = array(
    array(
      'class' => 'MyAwesomeDatastreamHandler',
    ),
  );

  return $handlers;
}
