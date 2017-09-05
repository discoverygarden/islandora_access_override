<?php

interface IslandoraAccessOverrideHandlerInterface {
  /**
   * @return bool|null
   *   TRUE to allow, FALSE to deny, or NULL to go with default behaviour.
   */
  public function handle();
}

interface IslandoraAccessOverrideObjectHandlerInterface extends IslandoraAccessOverrideHandlerInterface {
  /**
   * @return IslandoraAccessOverrideHandlerInterface
   *   An instance of this class.
   */
  public function build($op, AbstractObject $object, $user);

}

interface IslandoraAccessOverrideDatastreamHandlerInterface extends IslandoraAccessOverrideHandlerInterface {
  /**
   * @return IslandoraAccessOverrideHandlerInterface
   *   An instance of this class.
   */
  public function build($op, AbstractDatastream $datastream, $user);

}
