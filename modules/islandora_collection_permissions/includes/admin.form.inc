<?php

/**
 * @file
 * Admin form.
 */

/**
 * Form building function; build the admin form.
 */
function islandora_collection_permissions_admin_form($form, &$form_state) {
  $ancestor_queryable = module_exists('islandora_solr');
  $form['islandora_collection_permissions_use_solr_ancestor_field'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use Solr ancestor field?'),
    '#description' => t('Instead of the standard traversal up the object tree, attempt to get the ancestors from Solr.'),
    '#disabled' => !$ancestor_queryable,
    '#default_value' => $ancestor_queryable && variable_get('islandora_collection_permissions_use_solr_ancestor_field', FALSE),
  );

  $form['islandora_collection_permissions_solr_ancestor_field'] = array(
    '#type' => 'textfield',
    '#title' => t('Ancestor Solr field'),
    '#description' => t('A multi-valued Solr field containing all ancestor PIDs (similar to that used by Islandora Collection Search), with the closest ancestors first.'),
    '#default_value' => variable_get('islandora_collection_permissions_solr_ancestor_field', 'ancestors_ms'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_collection_permissions_use_solr_ancestor_field"]' => array(
          'checked' => TRUE,
        ),
      ),
    ),
  );

  return system_settings_form($form);
}
