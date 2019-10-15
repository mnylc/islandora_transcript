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

  foreach ($object as $ds => $value) {
    if ((strpos($ds,'TRANSCRIPT') === 0) && islandora_datastream_access(ISLANDORA_VIEW_OBJECTS, $object[$ds])) {
     $transcript_ds[$ds] = $object[$ds]->label;
    }
  }
  $keys =  array_keys($transcript_ds);
  $first_key = current($keys);
  $current_ds = isset($form_state['values']['transcript_select'])? $form_state['values']['transcript_select']:$first_key;
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

  $form['transcript'] = array(
    '#type' => 'markup',
    '#prefix' => '<div id="transcript-container">',
    '#suffix' => '</div>',
    '#markup' => '<pre>' . wordwrap($transcript) . '</pre>',
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),

  );
  $form['#tree'] = TRUE;
  $form['#attached']['library'][] = 'core/drupal.ajax';
  return $form;
}

function islandora_transcript_view_form_submit(array $form, array &$form_state, AbstractObject $object) {
  $form_state['rebuild'] = TRUE;
  return $form;
}

function ajax_transcript_select_callback(array $form, array &$form_state) {
  return $form['transcript'];
}


/**
 * Updates this object's TRANSCRIPT datastream.
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
  $pid = $object->id;
  $path = drupal_get_path('module', 'islandora_transcript');
  $form_state['object'] = $object;
  $form = array();
  $label = $object->label;
  $form['transcript'] = array(
    '#title' => t('Current Transcript'),
    '#type' => 'textarea',
    '#default_value' => $object['TRANSCRIPT']->content,
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
  return $form;
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
  $object = islandora_object_load($form_state['values']['pid']);
  $success = TRUE;
  try {
    $object["TRANSCRIPT"]->content = $form_state['values']['transcript'];
  }
  catch (Exception $e) {
    $success = FALSE;
    drupal_set_message(t("TRANSCRIPT update failed."));
  }
  if ($success) {
    drupal_set_message(t("TRANSCRIPT successfully updated."));
  }
}
