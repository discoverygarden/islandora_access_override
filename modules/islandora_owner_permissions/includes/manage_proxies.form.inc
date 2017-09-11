<?php

/**
 * @file
 * Menu callback functions.
 */

/**
 * Form for managing proxies.
 */
function islandora_owner_permissions_manage_proxies_form($form, &$form_state, $object) {
  module_load_include('inc', 'islandora_owner_permissions', 'includes/db');

  $form['#tree'] = TRUE;
  $form_state['#storage']['pid'] = $object->id;

  $proxies = islandora_owner_permissions_get_proxies_for_pid($object->id);
  $options = array();
  foreach ($proxies as $proxy) {
    $proxy_user = user_load($proxy['uid']);
    $options[$proxy['uid']] = array(
      'proxies' => $proxy_user->name,
    );
  }

  $form['remove'] = array(
    'proxies' => array(
      '#type' => 'tableselect',
      '#header' => array('proxies' => t('Proxies')),
      '#options' => $options,
      '#empty' => t('There are no owner proxies.'),
    ),
    'proxies_remove' => array(
      '#type' => 'submit',
      '#value' => t('Remove Proxies'),
    ),
  );

  $form['add'] = array(
    'user' => array(
      '#type' => 'textfield',
      '#title' => t('Owner Proxy'),
      '#autocomplete_path' => 'user/autocomplete',
      '#element_validate' => array('islandora_owner_permissions_user_validate'),
    ),
    'user_submit' => array(
      '#type' => 'submit',
      '#value' => t('Add Proxy'),
    ),
  );

  return $form;
}

/**
 * Element validation for user.
 */
function islandora_owner_permissions_user_validate($element, &$form_state) {
  $object = islandora_object_load($form_state['#storage']['pid']);
  $proxy = user_load_by_name($element['#value']);
  $proxies = islandora_owner_permissions_get_proxies_for_pid($object->id);
  if (!in_array('user_submit', $form_state['triggering_element']['#parents'])) {
    return;
  }
  if (!$element['#value']) {
    form_error($element, t('Please enter a user.'));
  }
  elseif (!$proxy) {
    form_error(
      $element,
      t(
        'User <strong>@user</strong> does not exist.',
        array('@user' => $element['#value'])
      )
    );
  }
  elseif ($object->owner == $element['#value']) {
    form_error(
      $element,
      t(
        'User <strong>@user</strong> is already the owner.',
        array('@user' => $element['#value'])
      )
    );
  }
  elseif (array_key_exists($proxy->uid, $proxies)) {
    form_error(
      $element,
      t(
        'User <strong>@user</strong> is already a proxy.',
        array('@user' => $element['#value'])
      )
    );
  }
}

/**
 * Form submit handler for managing proxies.
 */
function islandora_owner_permissions_manage_proxies_form_submit($form, &$form_state) {
  module_load_include('inc', 'islandora_owner_permissions', 'includes/db');

  if (in_array('user_submit', $form_state['triggering_element']['#parents'])) {
    islandora_owner_permissions_add_proxy_to_pid(
      $form_state['#storage']['pid'],
      $form_state['values']['add']['user']
    );
  }
  else {
    foreach ($form_state['values']['remove']['proxies'] as $proxy) {
      islandora_owner_permissions_remove_proxy_from_pid(
        $form_state['#storage']['pid'],
        $proxy
      );
    }
  }
}
