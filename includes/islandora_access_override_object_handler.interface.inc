<?php

class IslandoraAccessOverrideAbstractHandler implements IslandoraAccessOverrideHandlerInterface {
  const HOOK = 'abstract';

  protected static function getHandlers($op = NULL) {
    $stash =& drupal_static(static::HOOK);

    if ($stash === NULL) {
      $stash = module_invoke_all(static::HOOK);
    }

    if ($op !== NULL) {
      return isset($stash[$op]) ?
        $stash[$op] :
        NULL;
    }

    return $stash;
  }

  protected static function getHandler($op, $in, $user) {
    if ($in instanceof AbstractObject) {
      return IslandoraAccessOverrideObjectHandler::build($op, $in, $user);
    }
    elseif ($in instanceof AbstractDatastream) {
      return IslandoraAccessOverrideDatastreamHandler::build($op, $in, $user);
    }
    else {
      throw UnexpectedValueException('$in of unexpected type.');
    }
  }
}

class IslandoraAccessOverrideObjectHandler extends IslandoraAccessOverrideAbstractHandler implements IslandoraAccessOverrideObjerHandlerInterface {
  const HOOK = 'islandora_access_override_object_handlers';

  protected $op;
  protected $object;
  protected $user;

  protected function __construct($op, AbstractObject $object, $user, $handlers) {

  }

  public static function build($op, AbstractObject $object, $user) {
    $handlers = static::getHandlers($op);

    if (!isset($handlers)) {
      return NULL;
    }

    
  }

  public function handle() {
    foreach ($this->getHandlers($this->op) as $handler) {
      $result = 
    }
  }
}
