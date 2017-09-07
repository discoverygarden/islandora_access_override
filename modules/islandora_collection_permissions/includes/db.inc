<?php

function _islandora_collection_permissions_has_permissions_defined($pid) {
  return (bool) _islandora_collection_permissions_get_icppid_if_exists($pid);
}

function _islandora_collection_permissions_get_icppid_if_exists($pid) {
  return db_select('islandora_collection_permissions_pids', 'p')
    ->fields('p', array('icppid'))
    ->condition('pid', $pid)
    ->execute()
    ->fetchField();
}

function _islandora_collection_permissions_has_permission($op, $pid, $user) {
  $query = db_select('islandora_collection_permissions_pids', 'p')
    ->fields('p', array('icppid'))
    ->condition('p.pid', $pid);

  $alias = $query->join('islandora_collection_permissions_role_perms', 'rp', 'rp.pid = p.icppid');
  $query->condition("$alias.rid", array_keys($user->roles));

  $map = _islandora_collection_permissions_map();
  $query->condition("$alias.{$map[$op]['column']}", 1);

  return (bool) $query->countQuery()->execute()->fetchField();
}

/**
 * Adapted from user_role_permissions().
 *
 * @see user_role_permissions()
 */
function _islandora_collection_permissions_role_permissions($roles, AbstractObject $object) {
  $top_cache = &drupal_static(__FUNCTION__, array());
  $top_cache[$object->id] = array();
  $cache =& $top_cache[$object->id];

  $role_permissions = $fetch = array();

  if ($roles) {
    foreach ($roles as $rid => $name) {
      if (isset($cache[$rid])) {
        $role_permissions[$rid] = $cache[$rid];
      }
      else {
        // Add this rid to the list of those needing to be fetched.
        $fetch[] = $rid;
        // Prepare in case no permissions are returned.
        $cache[$rid] = array();
      }
    }

    if ($fetch) {
      $map = _islandora_collection_permissions_map();
      $map_to_columns = function ($perm) {
        return $perm['column'];
      };
      $perm_to_column = array_map($map_to_columns, $map);

      // Get from the database permissions that were not in the static variable.
      // Only role IDs with at least one permission assigned will return rows.
      $query = db_select('islandora_collection_permissions_pids', 'p')
        ->condition('p.pid', $object->id);

      $alias = $query->join('islandora_collection_permissions_role_perms', 'rp', 'rp.pid = p.icppid');
      $query->condition("$alias.rid", $fetch);
      $query->fields($alias, array_merge(array('rid'), array_values($perm_to_column)));
      $result = $query->execute();


      foreach ($result as $row) {
        foreach ($perm_to_column as $perm => $column) {
          $cache[$row->rid][$perm] = (bool) $row->{$column};
        }
      }
      foreach ($fetch as $rid) {
        // For every rid, we know we at least assigned an empty array.
        $role_permissions[$rid] = $cache[$rid];
      }
    }
  }

  return $role_permissions;
}

function _islandora_collection_permissions_delete_for_pid($pid) {
  $transaction = db_transaction();
  $icppid = _islandora_collection_permissions_get_icppid_if_exists($pid);

  if ($icppid !== NULL) {
    db_delete('islandora_collection_permissions_role_perms')
      ->condition('pid', $icppid)
      ->execute();
    db_delete('islandora_collection_permissions_pids')
      ->condition('pid', $pid)
      ->execute();
  }
}

function _islandora_collection_permissions_get_icppid_for_pid($pid) {
  $transaction = db_transaction();

  $result = _islandora_collection_permissions_get_icppid_if_exists($pid);

  if ($result) {
    return $result;
  }

  return db_insert('islandora_collection_permissions_pids')
    ->fields(array(
      'pid' => $pid,
    ))
    ->execute();
}
