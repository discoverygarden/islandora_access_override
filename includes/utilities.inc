<?php

/**
 * @file
 * Miscellaneous utility functions.
 */

/**
 * Helper; get the list of handlers for the given hook (and permission).
 *
 * @param string $hook
 *   The hook to invoke. Expected to be one of:
 *   - islandora_access_override_object_handlers; or,
 *   - islandora_access_override_datastream_handlers.
 * @param string|null $op
 *   An optional permission to reduce the scope of the return.
 *
 * @return array
 *   The array of handlers for the given hook. If $op is non-null, then only
 *   the hook results for the given permission will be returned.
 */
function _islandora_access_override_get_handlers($hook, $op = NULL) {
  $results =& drupal_static(__FUNCTION__, array());

  if (!isset($results[$hook])) {
    $results[$hook] = module_invoke_all($hook);
    drupal_alter($hook, $results[$hook]);
  }

  if (isset($op)) {
    return isset($results[$hook][$op]) ?
      $results[$hook][$op] :
      NULL;
  }

  return $results;
}

/**
 * Helper; route to the appropriate hook based on arguments.
 *
 * @param string $op
 *   The permission being tested.
 * @param AbstractObject|AbstractDatastream $in
 *   The object/datastream for which permissions are being tested.
 * @param object $user
 *   The user for whom permissions are being tested.
 *
 * @return bool|null|array
 *   Similar to islandora_object_access() and friends, TRUE to allow, FALSE to
 *   deny or NULL to make no assertion. An array will be passed up to be
 *   evaluated accordingly (allow if the array contains a TRUE and no FALSE).
 */
function _islandora_access_override_handle($op, $in, $user) {
  $hook_map = array(
    'AbstractObject' => 'islandora_access_override_object_handlers',
    'AbstractDatastream' => 'islandora_access_override_datastream_handlers',
  );

  $handlers = FALSE;

  foreach ($hook_map as $type => $hook) {
    if ($in instanceof $type) {
      $handlers = _islandora_access_override_get_handlers($hook, $op);
      break;
    }
  }

  if ($handlers === FALSE) {
    throw new UnexpectedValueException(t('$in of unexpected type.'));
  }
  elseif ($handlers === NULL) {
    // Don't have any handlers.
    return NULL;
  }

  $handle_handler = function ($handler) use ($op, $in, $user) {
    if (isset($handler['file'])) {
      require_once $handler['file'];
    }

    return call_user_func($handler['callable'], $op, $in, $user);
  };

  $result = array_map($handle_handler, $handlers);

  // Allowing or denying, pass the result through... if neither, let's return
  // NULL directly.
  return (in_array(TRUE, $result, TRUE) || in_array(FALSE, $result, TRUE)) ?
    $result :
    NULL;
}
