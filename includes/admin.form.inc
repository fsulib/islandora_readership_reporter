<?php

/**
 * @file
 * Administration page callbacks for the Islandora Readership Reporter module.
 */

/**
 * Islandora Readership Reporter administration and configuration form.
 *
 * @ingroup forms
 *
 * @return array
 *   The Drupal form definition.
 */
function islandora_readership_reporter_admin_form() {
  $form = array();

  $form['islandora_readership_reporter_debug_mode_email'] = array(
    '#type' => 'textfield',
    '#title' => t('Debug email'),
    '#description' => t('Address where readership reports are sent in debug mode.'),
    '#default_value' => variable_get('islandora_readership_reporter_debug_mode_email', 'user@example.org'),
  );

  $form['islandora_readership_reporter_debug_mode'] = array(
    '#type' => 'checkbox',
    '#title' => t('Debug mode'),
    '#description' => t('Send readership reports to the debug email address instead.'),
    '#default_value' => variable_get('islandora_readership_reporter_debug_mode', TRUE),
  );

  $form['islandora_readership_reporter_email_primary_color'] = array(
    '#type' => 'textfield',
    '#title' => t('Primary color code'),
    '#description' => t('Primary color code to be used in readership report emails, in "#XXXXXX" format.'),
    '#default_value' => variable_get('islandora_readership_reporter_email_primary_color', '#5E0117'),
  );

  $form['islandora_readership_reporter_email_secondary_color'] = array(
    '#type' => 'textfield',
    '#title' => t('Secondary color code'),
    '#description' => t('Secondary color code to be used in readership report emails, in "#XXXXXX" format.'),
    '#default_value' => variable_get('islandora_readership_reporter_email_secondary_color', '#CEC194'),
  );

  $form['islandora_readership_reporter_email_banner_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Banner image URL'),
    '#description' => t('URL to an image to be used as email banner (leave blank to choose no banner)'),
    '#default_value' => variable_get('islandora_readership_reporter_email_banner_url', ''),
  );

  $form['islandora_readership_reporter_email_footer'] = array(
    '#type' => 'textarea',
    '#title' => t('Email footer'),
    '#description' => t('HTML code for email footer to come after report.'),
    '#default_value' => variable_get('islandora_readership_reporter_email_footer', ''),
  );

  return system_settings_form($form);
}
