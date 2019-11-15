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
   * A lazy populated cache.
   *
   * @var array
   *   An associative array mapping PID to their permissioned ancestor if
   *   applicable, or NULL if they do not have any overrides.
   */
  protected static $cache = array();

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
    if (!array_key_exists($this->object->id, static::$cache)) {
      static::$cache[$this->object->id] = NULL;

      module_load_include('inc', 'islandora_collection_permissions', 'includes/db');
      while ($operand_in = array_shift($this->pidsToExamine)) {
        // XXX: Explictly as a string...
        $operand = "$operand_in";
        if (in_array($operand, $this->examinedPids)) {
          continue;
        }
        else {
          $this->examinedPids[] = $operand;
        }

        if (_islandora_collection_permissions_has_permissions_defined($operand)) {
          static::$cache[$this->object->id] = $operand;
          break;
        }

        $this->pidsToExamine = array_merge($this->pidsToExamine, $this->getRelatedPids($operand_in));
      }
    }

    return static::$cache[$this->object->id];
  }

  /**
   * Helper; find the related PIDs of the given object.
   *
   * Related, meaning "isMemberOfCollection" or "isMemberOf" relationships.
   *
   * @param string $pid
   *   The PID of the object for which to obtain related PIDs.
   *
   * @return string[]|IslandoraCollectionPermissionsSolrString[]
   *   An array ancestor PIDs. If _not_ using the Solr ancestor field, this
   *   should be direct ancestors only (resulting in a breadth-first search).
   *   If _using_ a Solr ancestor field (and the given PID is indexed), this
   *   will be return all of the values (depth-first search).
   */
  protected function getRelatedPids($pid) {
    if (module_exists('islandora_solr') && variable_get('islandora_collection_permissions_use_solr_ancestor_field', FALSE)) {
      if ($pid instanceof IslandoraCollectionPermissionsSolrString) {
        // Avoid hitting something which already has had its ancestors
        // included.
        return array();
      }

      $result = $this->querySolrForAncestors($pid);
      if ($result !== FALSE) {
        return $result;
      }
    }

    $object = islandora_object_load($pid);
    if (is_object($object)) {
      $related = array();

      foreach (array('isMemberOfCollection', 'isMemberOf') as $pred) {
        foreach ($object->relationships->get(FEDORA_RELS_EXT_URI, $pred) as $relation) {
          $test = islandora_object_load($relation['object']['value']);
          if (is_object($test)) {
            $related[] = $relation['object']['value'];
          }
        }
      }

      $related = array_unique($related);
      sort($related);

      return $related;
    }
  }
  /**
   * Helper; hit Solr for ancestors of the given PID.
   *
   * @param string $pid
   *   The PID of the object to process.
   *
   * @return bool|IslandoraCollectionPermissionsSolrString[]
   *   An array of strings wrapped in instances of,
   *   IslandoraCollectionPermissionsSolrString, if there were any results from
   *   Solr; otherwise, boolean FALSE if we couldn't find anything for the
   *   given document.
   */
  protected function querySolrForAncestors($pid) {
    $ancestor_field = variable_get('islandora_collection_permissions_solr_ancestor_field', 'ancestors_ms');

    $qp = new IslandoraSolrQueryProcessor();
    $qp->buildQuery(format_string('PID:"!pid"', array(
      '!pid' => $pid,
    )));
    $qp->solrParams['facet'] = 'false';
    $qp->solrParams['fl'] = "PID,$ancestor_field";
    // Should only be one document for the PID.
    $qp->solrLimit = 1;
    $qp->executeQuery(FALSE, TRUE);

    $to_pid = function ($pid) {
      return new IslandoraCollectionPermissionsSolrString($pid);
    };

    if ($qp->islandoraSolrResult['response']['numFound'] == 0) {
      // Found nothing; fallback to RI lookup.
      return FALSE;
    }

    $ancestor_field_existed = FALSE;
    $ancestor_field_values = drupal_array_get_nested_value($qp->islandoraSolrResult, array(
      'response',
      'objects',
      0,
      'solr_doc',
      $ancestor_field,
    ), $ancestor_field_existed);
    if (!$ancestor_field_existed) {
      // No result entry; fallback.
      return FALSE;
    }
    elseif (!isset($ancestor_field_values)) {
      // XXX: The value of the ancestor field was actually "NULL"; this should
      // never happen.
      return FALSE;
    }

    // Wrap up the PIDs, to know they came from here.
    return array_map($to_pid, $ancestor_field_values);
  }

}

/**
 * Dumb string wrapper, for provenance while iterating.
 */
class IslandoraCollectionPermissionsSolrString {
  protected $string;

  /**
   * Constructor.
   */
  public function __construct($string) {
    $this->string = $string;
  }

  /**
   * Implementation of the magic __toString() method.
   */
  public function __toString() {
    return $this->string;
  }

}
