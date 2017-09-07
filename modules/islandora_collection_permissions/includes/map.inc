<?php

/**
 * @file
 * Permission mappings.
 */

/**
 * Define the mapping.
 *
 * @return array
 *   An associative array mapping our pseudo-permissions to an array of real
 *   permissions which they are to grant.
 */
function _islandora_collection_permissions_map() {
  static $perm = NULL;

  if ($perm === NULL) {
    $perm =& drupal_static(__FUNCTION__, array());

    $perm[ISLANDORA_COLLECTION_PERMISSIONS_CREATE] = array(
      'title' => t('Create child collections'),
      'column' => 'create_perm',
      'perms' => array(
        ISLANDORA_INGEST,
        ISLANDORA_BASIC_COLLECTION_CREATE_CHILD_COLLECTION,
      ),
    );
    $perm[ISLANDORA_COLLECTION_PERMISSIONS_UPDATE] = array(
      'title' => t('Modify objects and datastreams'),
      'column' => 'update_perm',
      'perms' => array(
        ISLANDORA_METADATA_EDIT,
        ISLANDORA_ADD_DS,
        ISLANDORA_MANAGE_PROPERTIES,
        ISLANDORA_REVERT_DATASTREAM,
        ISLANDORA_VIEW_DATASTREAM_HISTORY,
        ISLANDORA_REGENERATE_DERIVATIVES,
        ISLANDORA_REPLACE_DATASTREAM_CONTENT,
      ),
    );
    $perm[ISLANDORA_COLLECTION_PERMISSIONS_DELETE] = array(
      'title' => t('Purge objects and datastreams'),
      'column' => 'delete_perm',
      'perms' => array(
        ISLANDORA_PURGE,
      ),
    );

    drupal_alter(__FUNCTION__, $perm);
  }

  return $perm;
}

/**
 * Get the inverse mapping.
 *
 * @return array
 *   An associative array mapping permission to one of our pseudo-permission
 *   strings.
 */
function _islandora_collection_permissions_flipped_map() {
  $perm =& drupal_static(__FUNCTION__);

  if ($perm === NULL) {
    $perm = array();

    foreach (_islandora_collection_permissions_map() as $k => $v) {
      foreach ($v['perms'] as $inner) {
        $perm[$inner] = $k;
      }
    }
  }

  return $perm;
}
