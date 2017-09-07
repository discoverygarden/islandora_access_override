<?php

/**
 * @file
 * Callback and friends.
 */

function islandora_collection_permissions_test_object($op, AbstractObject $object, $user) {
  $handler = IslandoraCollectionPermissionsHandler::build($op, $object, $user);
  return $handler === NULL ?
    NULL :
    $handler->handle();
}

class IslandoraCollectionPermissionsHandler {
  protected $op;
  protected $object;
  protected $user;

  protected $examinedPids = array();
  protected $pidsToExamine = array();

  protected function __construct($op, AbstractObject $object, $user) {
    $this->op = $op;
    $this->object = $object;
    $this->user = $user;

    $this->pidsToExamine[] = $object->id;
  }

  public static function build($op, AbstractObject $object, $user) {
    static $map = NULL;
    if ($map === NULL) {
      $map = _islandora_collection_permissions_flipped_map();
    }

    if (!isset($map[$op])) {
      // Isn't handled; nothing to build.
      return NULL;
    }

    return new static(
      $map[$op],
      $object,
      $user
    );
  }

  /**
   * @return bool|null
   *   TRUE to allow, FALSE to deny or NULL to make no assertion.
   */
  public function handle() {
    module_load_include('inc', 'islandora_collection_permissions', 'includes/db');
    $active = $this->getActivePermissionedCollection();

    if ($active === NULL) {
      // Exhausted all options, make no assertion.
      return NULL;
    }

    $result = _islandora_collection_permissions_has_permission($this->op, $active, $this->user);
    // Avoid making any assertion when not explicitly allowing.
    return $result ?
      $result :
      NULL;
  }

  public function getActivePermissionedCollection() {
    module_load_include('inc', 'islandora_collection_permissions', 'includes/db');
    while ($operand = array_shift($this->pidsToExamine)) {
      if (in_array($operand, $this->examinedPids)) {
        continue;
      }

      if (_islandora_collection_permissions_has_permissions_defined($operand)) {
        return $operand;
      }

      $this->pidsToExamine = array_merge($this->pidsToExamine, $this->getRelatedPids($operand));
    }
  }

  protected function getRelatedPids($pid) {
    $object = islandora_object_load($pid);
    $related = array();

    foreach (array('isMemberOfCollection', 'isMemberOf') as $pred) {
      foreach ($object->relationships->get(FEDORA_RELS_EXT_URI, $pred) as $relation) {
        $related[] = $relation['object']['value'];
      }
    }

    $related = array_unique($related);
    sort($related);
    return $related;
  }
}
