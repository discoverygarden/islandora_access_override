<?php

/**
 * @file
 * Handler class.
 */

/**
 * Handler class; wrap things up.
 */
class IslandoraCollectionPermissionsHandler {
  /**
   * Our module-defined pseudo-permission.
   *
   * @var string
   */
  protected $op;

  /**
   * The object for which we are evaluating the pseudo-permission.
   *
   * @var AbstractObject
   */
  protected $object;

  /**
   * The loaded user object for whom to check permission for the given object.
   *
   * @var object
   */
  protected $user;

  /**
   * PIDs already examined in the ::getActivePermissionedCollection().
   *
   * To avoid infinite looping in the case of looping collection hierarchies.
   *
   * @var string[]
   */
  protected $examinedPids = array();

  /**
   * PID queue to breadth-first search in ::getActivePermissionedCollection().
   *
   * @var string[]
   */
  protected $pidsToExamine = array();

  /**
   * Constructor.
   *
   * @param string $op
   *   Our defined pseudo-permission to evaluate.
   * @param AbstractObject $object
   *   The object for which to evaluate the permission.
   * @param object $user
   *   A loaded user object for which to evaluate the permission.
   */
  protected function __construct($op, AbstractObject $object, $user) {
    $this->op = $op;
    $this->object = $object;
    $this->user = $user;

    $this->pidsToExamine[] = $object->id;
  }

  /**
   * Factory; construct if appropriate.
   *
   * @param string $op
   *   A Drupal permission to evaluate.
   * @param AbstractObject $object
   *   The object for which to evaluate the permission.
   * @param object $user
   *   The user for which to evaluate the permssion.
   *
   * @return IslandoraCollectionPermissionsHandler|null
   *   If we define a mapping for the given permission, an instance of this
   *   class; otherwise, NULL.
   */
  public static function build($op, AbstractObject $object, $user) {
    static $map = NULL;
    if ($map === NULL) {
      module_load_include('inc', 'islandora_collection_permissions', 'includes/map');
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
   * Evaluate the permission.
   *
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

  /**
   * Helper; determine the object defining permission for the current object.
   *
   * @return string|null
   *   The PID of the object defining permission for the current object, if
   *   there is one (may be the current object); otherwise, NULL.
   */
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

  /**
   * Helper; find the related PIDs of the given object.
   *
   * Related, meaning "isMemberOfCollection" or "isMemberOf" relationships.
   *
   * @param string $pid
   *   The PID of the object for which to obtain related PIDs.
   *
   * @return string[]
   *   An array direct ancestor PIDs.
   */
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
