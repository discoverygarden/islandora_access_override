<?php

/**
 * @file
 * Form and associated handlers.
 */

/**
 * Collection permissions management form.
 *
 * Adapted from user_admin_permissions() form.
 *
 * @see user_admin_permissions()
 */
function islandora_collection_permissions_object_form($base_form, $form_state, $object) {
  form_load_include($form_state, 'inc', 'islandora_collection_permissions', 'includes/db');
  form_load_include($form_state, 'inc', 'islandora_collection_permissions', 'includes/form');

  $object_id = $object->id;

  // XXX: Force reload of object, just in case.
  $object = islandora_object_load($object_id);
  $defined = _islandora_collection_permissions_has_permissions_defined($object);

  $base_form['enable'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable collection permission overrides for this collection.'),
    '#default_value' => $defined,
  );
  $base_form['permission_table'] = array(
    '#type' => 'fieldset',
    '#states' => array(
      'visible' => array(
        ':input[name="enable"]' => array(
          'checked' => TRUE,
        ),
      ),
    ),
    'table' => array(
      '#type' => 'markup',
      '#theme' => 'user_admin_permissions',
    ),
  );
  $form =& $base_form['permission_table']['table'];

  // Retrieve role names for columns.
  $role_names = user_roles(TRUE);
  // Fetch permissions for all roles or the one selected role.
  $role_permissions = _islandora_collection_permissions_role_permissions($role_names, $object);

  // Store $role_names for use when saving the data.
  $form['role_names'] = array(
    '#type' => 'value',
    '#value' => $role_names,
  );
  // Render role/permission overview:
  $options = array();

  $permissions = _islandora_collection_permissions_map();
  foreach ($permissions as $perm => $perm_item) {
    // Fill in default values for the permission.
    $perm_item += array(
      'description' => '',
      'restrict access' => FALSE,
      'warning' => !empty($perm_item['restrict access']) ? t('Warning: Give to trusted roles only; this permission has security implications.') : '',
    );
    $options[$perm] = '';
    $form['permission'][$perm] = array(
      '#type' => 'item',
      '#markup' => $perm_item['title'],
      '#description' => filter_xss(theme('user_permission_description', array('permission_item' => $perm_item, 'hide' => TRUE))),
    );
    foreach ($role_names as $rid => $name) {
      // Builds arrays for checked boxes for each role
      if (isset($role_permissions[$rid][$perm]) && $role_permissions[$rid][$perm]) {
        $status[$rid][] = $perm;
      }
    }
  }

  // Have to build checkboxes here after checkbox arrays are built
  foreach ($role_names as $rid => $name) {
    $form['checkboxes'][$rid] = array(
      '#type' => 'checkboxes',
      '#options' => $options,
      '#default_value' => isset($status[$rid]) ? $status[$rid] : array(),
      '#attributes' => array('class' => array('rid-' . $rid)),
    );
    $form['role_names'][$rid] = array('#markup' => check_plain($name), '#tree' => TRUE);
  }

  $base_form['actions'] = array('#type' => 'actions');
  $base_form['actions']['submit'] = array('#type' => 'submit', '#value' => t('Save'));

  return $base_form;
}

function islandora_collection_permissions_object_form_submit($form, $form_state) {
  $transaction = db_transaction();
  $values = $form_state['values'];

  list($object) = $form_state['build_info']['args'];
  $pid = $object->id;

  if (!$values['enable']) {
    // Not enabled; delete any of our entries from the DB.
    _islandora_collection_permissions_delete_for_pid($pid);
    return;
  }
  else {
    $map_to_column = function ($perm) {
      return $perm['column'];
    };
    $permissions = array_map($map_to_column, _islandora_collection_permissions_map());

    $icppid = _islandora_collection_permissions_get_icppid_for_pid($pid);
    db_delete('islandora_collection_permissions_role_perms')
      ->condition('pid', $icppid)
      ->execute();
    $insert = db_insert('islandora_collection_permissions_role_perms')
      ->fields(array_merge(array('pid', 'rid'), array_values($permissions)));
    foreach ($values['role_names'] as $rid => $name) {
      $row = array($icppid, $rid);
      foreach ($permissions as $name => $col) {
        $row[] = $values[$rid][$name] ? 1 : 0;
      }
      $insert->values($row);
    }
    $insert->execute();
  }
}
