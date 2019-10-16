<?php

/**
 * @file
 * This file contains the functions to render the transcript tab.
 */

/**
 * Renders the TRANSCRIPT tab.
 *
 * @pararm AbstractObject $object
 *   The object the TRANSCRIPT belongs to.
 *
 * @return array
 *   A renderable array representing the tab.
 */
function islandora_transcript_view_form(array $form, array &$form_state, AbstractObject $object) {
  $form = array();
  foreach ($object as $ds => $value) {
    if ((strpos($ds,'TRANSCRIPT') === 0) && islandora_datastream_access(ISLANDORA_VIEW_OBJECTS, $object[$ds])) {
      $label = substr($ds, strlen('TRANSCRIPT'));
      $label = str_replace('_','',$label);
      $display_label = (strlen($label) != 0) ? t("Translated Transcript to @lang",array('@lang' => $label)) : t("Transcript in original language");
      $transcript_ds[$ds] = $display_label;
    }
  }
  $keys =  array_keys($transcript_ds);
  $first_key = current($keys);
  $current_ds = isset($form_state['values']['transcript_select'])? $form_state['values']['transcript_select']: $first_key;
  $transcript = isset($object[$current_ds]) ? $object[$current_ds]->content : 'Sorry, there is no transcript available';
  $form['transcript_select'] = array(
    '#type' => 'select',
    '#title' => 'Available Transcripts',
    '#options' => $transcript_ds,
    '#default_value' => $current_ds,
    '#ajax' => array(
      'event' => 'change',
      'method' => 'replace',
      'callback' => 'ajax_transcript_select_callback',
      'wrapper' => 'transcript-container',
    ),
  );
  // Note. we are forcing here ISO because of the data we are dealing with
  // But the right format should be UTF-8. Sadly people are not encoding
  // their transcripts correctly.
  $form['transcript'] = array(
    '#type' => 'markup',
    '#prefix' => '<div id="transcript-container">',
    '#suffix' => '</div>',
    '#markup' => '<pre>' . htmlentities($transcript, ENT_SUBSTITUTE, 'ISO-8859-1', false) . '</pre>',
  );

  $form['download'] = array(
    '#type' => 'markup',
    '#markup' => theme('islandora_datastream_download_link',
    array('datastream' => $object[$current_ds])),
  );

  $form['#tree'] = TRUE;
  $form['#submit'][] = 'islandora_transcript_view_form_submit';
  return $form;
}

function islandora_transcript_view_form_submit(array $form, array &$form_state) {
  $form_state['rebuild'] = TRUE;
  return $form;
}

function ajax_transcript_select_callback(array $form, array &$form_state) {
  return $form['transcript'];
}

/**
 * Updates this object's TRANSCRIPT datastreams.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 * @param AbstractObject $object
 *   The page object to be updated.
 *
 * @return array
 *   The Drupal form definition.
 */
function islandora_transcript_edit_transcript_form(array $form, array &$form_state, AbstractObject $object) {
  $form = array();
  foreach ($object as $ds => $value) {
    if ((strpos($ds,'TRANSCRIPT') === 0) && islandora_datastream_access(ISLANDORA_METADATA_EDIT, $object[$ds])) {
      $label = substr($ds, strlen('TRANSCRIPT'));
      $label = str_replace('_','',$label);
      $display_label = (strlen($label) != 0) ? t("Translated Transcript to @lang",array('@lang' => $label)) : t("Transcript in original language");
      $transcript_ds[$ds] = $display_label;
    }
  }
  // If we are here it is because there is at least one editable Datastream.
  $keys =  array_keys($transcript_ds);
  $first_key = current($keys);
  $current_ds = isset($form_state['values']['transcript_select'])? $form_state['values']['transcript_select']: $first_key;
  $transcript = isset($object[$current_ds]) ? $object[$current_ds]->content : 'Sorry, there is no transcript available';
  unset($form_state['input']['transcript']);
  $form['transcript_select'] = array(
    '#type' => 'select',
    '#title' => 'Available Transcripts',
    '#options' => $transcript_ds,
    '#default_value' => $current_ds,
    '#ajax' => array(
      'event' => 'change',
      'method' => 'replace',
      'callback' => 'ajax_transcript_edit_callback',
      'wrapper' => 'transcript-container-update',
    ),
  );
  $form['transcript'] = array(
    '#title' => t('Current Transcript'),
    '#type' => 'textarea',
    '#prefix' => '<div id="transcript-container-update">',
    '#suffix' => '</div>',
    '#default_value' => utf8_encode($transcript),
    '#rows' => 40,
    '#cols' => 20,
    '#attributes' => array('class' => array('transcript_window')),
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Update Transcript'),
  );
  $form['pid'] = array(
    '#type' => 'hidden',
    '#value' => $object->id,
  );
  $form['#tree'] = TRUE;
  return $form;
}

function ajax_transcript_edit_callback(array $form, array &$form_state) {
  return $form['transcript'];
}

/**
 * Submit handler for the edit form.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 */
function islandora_transcript_edit_transcript_form_submit(array $form, array &$form_state) {
  module_load_include('inc', 'islandora_transcript', 'includes/utilities');
  $ds = $form_state['values']['transcript_select'];
  $object = islandora_object_load($form_state['values']['pid']);
  $success = TRUE;
  try {
    $object[$ds]->content = utf8_decode($form_state['values']['transcript']);
  }
  catch (Exception $e) {
    $success = FALSE;
    drupal_set_message(t("@ds update failed.", array('@ds' => $ds)));
  }
  if ($success) {
    drupal_set_message(t("@ds successfully updated.", array('@ds' => $ds)));
  }
}
