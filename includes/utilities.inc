<?php

/**
 * @file
 * Miscellaneous utility functions.
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
