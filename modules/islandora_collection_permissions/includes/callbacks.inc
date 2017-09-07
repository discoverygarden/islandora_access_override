<?php

/**
 * @file
 * Callback and friends.
 */

/**
 * Implements callback_islandora_access_override_object_handler().
 */
function islandora_collection_permissions_test_object($op, AbstractObject $object, $user) {
  $handler = IslandoraCollectionPermissionsHandler::build($op, $object, $user);
  return $handler === NULL ?
    NULL :
    $handler->handle();
}
