<?php
// $Id$

/**
 * Specify the default properties of votesmart_webform component
 *
 * @return An array defining the default structure of a component.
 */
function _webform_defaults_votesmart() {
  return array(
    'name' => '',
    'form_key' => NULL,
    'email' => 1,
    'pid' => 0,
    'weight' => 0,
    'type' => 'hidden',
    'extra' => array(
      'email' => 1,
      'allow_candidates_without_email' => 1,
      'hide_no_email_warnings' => 1,
      'attributes' => array(),
      'name_format' => '!last !suffix, !first',
      'zip' => '',
      'hide_zip' => TRUE,
      'state' => '',
      'office_types' => array(),
      'hide_state' => FALSE
    )
  );
}

/**
 * Given candidate and tokenized format string, return candidates name formatted.
 * @param $candidate candidate array object
 * @param $format tokenized tring used to format name
 * @return string containing candidate's name formatted
 */
function _votesmart_webform_format_name($candidate, $format = '!last !suffix, !first') {
  if ( !is_array($candidate) ) return '';

  $name = t($format, array(
    '!title' => $candidate['title'],
    '!first' => $candidate['firstName'],
    '!last' =>  $candidate['lastName'],
    '!suffix' => $candidate['suffix'],
    '!middle' => $candidate['middleName'],
    '!nick' => $candidate['nickName']
  ));
  $name = trim($name);
  $name = preg_replace('@ +@', ' ', $name);
  $name = str_replace(' ,', ',', $name);
  return $name;
}

/**
 * Generate the form for editing votesmart_webform component
 * @param $component
 *   A Webform component array.
 * @return
 *   An array of form items to be displayed on the edit component page
 */
function _webform_edit_votesmart($component) {
  include('votesmart_webform.defines.php');

  $form = array('extra' => array());
  $form['extra']['state'] = array(
    '#type' => 'select',
    '#title' => t('Default State'),
    '#description' => t('Specify Default State, has no effect if zip is specified'),
    '#weight' => -1.2,
    '#default_value' => $component['extra']['state'],
    '#element_validate' => array('_webform_edit_votesmart_default_state_validate'),
    '#options' => array_merge(array('' => ''), $VOTESMART_WEBFORM_US_STATES)
  );

  $form['extra']['hide_state'] = array(
    '#type' => 'checkbox',
    '#title' => t('Hide State on Form, don\'t allow user to change value'),
    '#weight'  => -1.15,
    '#default_value' => $component['extra']['hide_state']
  );

  $form['extra']['office_types'] = array(
    '#type' => 'select',
    '#multiple' => TRUE,
    '#title' => t('Default Office(s)'),
    '#description' => t('Show only officials in these offices. If you all offices leave this blank.'),
    '#weight' => -1.2,
    '#default_value' => $component['extra']['office_types'],
    '#options' => array_merge(array('' => ''), $VOTESMART_WEBFORM_OFFICE_TYPES)
  );

  $form['extra']['hide_state'] = array(
    '#type' => 'checkbox',
    '#title' => t('Hide State on Form, don\'t allow user to change value'),
    '#weight'  => -1.15,
    '#default_value' => $component['extra']['hide_state']
  );


  $form['extra']['zip'] = array(
    '#type' => 'textfield',
    '#title' => t('Default Zip'),
    '#description' => t('Leave blank if you wish to use the State dropdown on form.'),
    '#weight' => -1.1,
    '#size' => '10',
    '#maxlength' => 10,
    '#default_value' => $component['extra']['zip'],
    '#element_validate' => array('_webform_edit_votesmart_default_zip_validate'),
  );

  $form['extra']['hide_zip'] = array(
    '#type' => 'checkbox',
    '#title' => t('Hide Zip on form, don\'t allow user to change value.'),
    '#weight'  => -1.05,
    '#default_value' => $component['extra']['hide_zip']
  );

  $form['extra']['allow_candidates_without_email'] = array(
    '#type' => 'checkbox',
    '#title' => t('Allow Candidates Without Email to be Checked'),
    '#weight'  => 0,
    '#default_value' => $component['extra']['allow_candidates_without_email']
  );

  $form['extra']['hide_no_email_warnings'] = array(
    '#type' => 'checkbox',
    '#title' => t('Hide No Email Warning Descriptions'),
    '#weight'  => 0,
    '#default_value' => $component['extra']['hide_no_email_warnings']
  );


  $form['extra']['name_format'] = array(
    '#type' => 'textfield',
    '#title' => t('Candidate Name Format'),
    '#weight' => 0,
    '#description' => t('valid tokens include: !title !first !middle !last !suffix !nick ... extra spaces are trimmed'),
    '#default_value' => $component['extra']['name_format']
  );

  return $form;
}

/**
 * Validate the Default Zip Code
 * @param $zip_element The zip form element.
 * @param $form_state The full form state for the webform.
 */
function _webform_edit_votesmart_default_zip_validate($element, &$form_state) {
  $zip = $element['#value'];//alias
  if ( empty($zip) ) return;

  if ( !votesmart_webform_is_zip($zip) ) {
    form_error($element, t('!value is not a valid zip code.', array('!value' => $zip)));
  }
}

/**
 * Validate the Zip Code BLANK NOT ALLOWED
 * @param $element The zip form element.
 * @param $form_state The full form state for the webform.
 */
function _webform_edit_votesmart_zip_validate($element, &$form_state) {
  $zip = $element['#value'];//alias
  if ( empty($zip) )
    form_error($element, t('zip can not be blank.', array('!value' => $zip)));

  if ( !votesmart_webform_is_zip($zip) ) {
    form_error($element, t('!value is not a valid zip code.', array('!value' => $zip)));
  }
}

/**
 * Validate the Default State
 * @param $element The state form element.
 * @param $form_state The full form state for the webform.
 */
function _webform_edit_votesmart_default_state_validate($element, &$form_state) {
  $state = $element['#value'];//alias
  if ( empty($state) ) return;

  if ( !votesmart_webform_is_us_state($state) ) {
    form_error($element, t('!value is not a valid state.', array('!value' => $state)));
  }
}

/**
 * Validate the State BLANK NOT ALLOWED
 * @param $element The state form element.
 * @param $form_state The full form state for the webform.
 */
function _webform_edit_votesmart_state_validate($element, &$form_state) {
  $state = $element['#value'];//alias
  if ( empty($state) )
    form_error($element, t('Please select a state.'));

  if ( !votesmart_webform_is_us_state($state) ) {
    form_error($element, t('!value is not a valid state.', array('!value' => $state)));
  }
}



/**
 * Render a Webform component to be part of a form.
 *
 * @param $component
 *   A Webform component array.
 * @param $value
 *   If editing an existing submission or resuming a draft, this will contain
 *   an array of values to be shown instead of the default in the component
 *   configuration. This value will always be an array, keyed numerically for
 *   each value saved in this field.
 */
function _webform_render_votesmart($component, $value = NULL) {
  include('votesmart_webform.defines.php');

  if ( !empty($value) )
    $value = unserialize($value[0]);

  if ( isset($value['votesmart']) ) {
    if ( isset($value['votesmart']['zip']) )
      $component['extra']['zip'] = $value['votesmart']['zip'];
    if ( isset($value['votesmart']['state']) )
      $component['extra']['state'] = $value['votesmart']['state'];
    if ( isset($value['votesmart']['office_types']) )
      $component['extra']['office_types'] = $value['votesmart']['office_types'];

  }
  
  $dom_id = 'webform-component-' . $component['form_key'];
  $form = array(
    '#prefix' => '<div id="' . $dom_id.'" class="webform-component-votesmart">',
    'votesmart' => array(),
    '#suffix' => '</div><!--/#' . $dom_id.' .webform-component-votesmart-->',
    '#weight' => $component['weight']
  );

  //HACK: Need to find a better way
  $votesmart_post = _votesmart_webform_array_key_search($_POST, 'votesmart');
  if ( $votesmart_post ) {
    if ( isset($votesmart_post['votesmart']) ) {
      $votesmart_post = $votesmart_post['votesmart'];
    }
    $component['extra']['zip'] = $votesmart_post['zip'];
    $component['extra']['state'] = $votesmart_post['state'];
    $component['extra']['office_types'] = $votesmart_post['office_types'];
    if ( isset($component['extra']['office_types']) )
      $component['extra']['office_types'] = unserialize($component['extra']['office_types']);
  }

  if ( !$component['extra']['hide_zip'] ) {
    $form['votesmart']['zip'] = array(
      '#type' => 'textfield',
      '#title' => t('Zip'),
      '#required' => TRUE,
      '#size' => 10,
      '#maxlength' => 10,
      '#default_value' => $component['extra']['zip'],
      '#description'   => t('Postal Code in format 12345 or 12345-1234'),
      '#element_validate' => array('_webform_edit_votesmart_zip_validate'),
      '#ahah' => array(
        'event' => 'change',
        'wrapper' => $dom_id,
        'path' => 'votesmart_webform/ahah/get_candidates',
        'progress' => array('type' => 'throbber')
      )
    );
  }

  if ( !$component['extra']['hide_state'] ) {
    $form['votesmart']['state'] = array(
      '#type' => 'select',
      '#title' => t('State'),
      '#options' => array_merge(array('' => ''), $VOTESMART_WEBFORM_US_STATES),
      '#multiple' => FALSE,
      '#required' => TRUE,
      '#default_value' => $component['extra']['state'],
      '#element_validate' => array('_webform_edit_votesmart_state_validate'),
      '#ahah' => array(
        'event' => 'change',
        'wrapper' => $dom_id,
        'path' => 'votesmart_webform/ahah/get_candidates',
        'progress' => array('type' => 'throbber')
      )
    );
  }

  $form['votesmart']['office_types'] = array(
    '#type' => 'hidden',
    '#value' => serialize($component['extra']['office_types'])
  );

  $form['votesmart']['jumper'] = array();//used to jump to various offices

  $zip = $form['votesmart']['zip']['#default_value'];//alias
  $state = $form['votesmart']['state']['#default_value'];//alias

  $office_types = array();
  if ( !empty($component['extra']['office_types']) ) {
    $office_types = $component['extra']['office_types'];
  }

  $candidates = votesmart_webform_candidate_form_elements(array('state' => $state, 'zip' => $zip, 'officeTypes' => $office_types), $component, $form['votesmart']['jumper']);

  if ( !empty($candidates) ) {
    $candidate_count = count($candidates);
    $form['votesmart']['candidates_prefix'] = array(
      '#type' => 'markup',
      '#value' => '<div class="votesmart-webform-candidate-wrapper"><span class="votesmart-webform-count">' . t('Showing %candidate_count Officials', array('%candidate_count' => $candidate_count)).'</span>'
    );

    $form['votesmart']['candidates'] = $candidates;
    $form['votesmart']['candidate_suffix'] = array(
      '#type' => 'markup',
      '#value' => '</div><!--/.votesmart-webform-candidate-wrapper-->'
    );
  }

  if ( is_array($value['votesmart']['candidates']) )
    foreach ($value['votesmart']['candidates'] as $k => $v )
      $form['votesmart']['candidates'][$k]['#default_value'] = $v;

  return $form;
}

/**
 * @param $args array containing conditions the officials must meet to be included
 * @param $component array containing component options
 * @param $jumper_element html element containing list used to jump to various branches in the list
 * @return array of candidate form elements
 */
function votesmart_webform_candidate_form_elements($args, $component, &$jumper_element = NULL) {
  include('votesmart_webform.defines.php');

  $candidates = $candidate_elements = $cand_names = array();

  $candidates = votesmart_webform_get_candidates($args);
  if ( empty($candidates) ) return array();

  $cand_names = array();
  foreach ($candidates as $id => $c) {
    if ( !isset($cand_names[$c['officeTypeId']]) )
      $cand_names[$c['officeTypeId']] = array();
    $cand_names[$c['officeTypeId']][$id] = _votesmart_webform_format_name($c, $component['extra']['name_format']);
  }

  $needs_jumper = count($cand_names) > 1 && count($candidates) > 10;

  if ( $needs_jumper ) {
    $jumper_element = array(
      '#type' => 'markup',
      '#value' => '<ul class="votesmart-webform-jumper">'
    );
  }

  asort($cand_names);
  foreach ($cand_names as $office => $cands ) {
    $anchor_name = 'votesmart-webform-office-type-' . strtolower($office);
    $office_label = t($VOTESMART_WEBFORM_OFFICE_TYPES[$office]);
    if ( $needs_jumper )
      $jumper_element['#value'] .= '<li><a title="jump to ' . $office_label.'" href="#' . $anchor_name . '">' . $office_label . '</a></li>';
    $candidate_elements[] = array(
      '#type' => 'markup',
      '#value' => '<h4><a name="' . $anchor_name.'">' . $office_label.'</a></h4>'
    );
    asort($cands);
    foreach ($cands as $cand_id => $cand_name ) {
      $element = array(
        '#type' => 'checkbox',
        '#title' => $cand_name,
      );

      if ( !$component['extra']['hide_no_email_warnings'] && !isset($candidates[$cand_id]['Email']) ) {     
        if ( !$component['extra']['allow_candidates_without_email'] )
          $element['#disabled'] = TRUE;
        $element['#description'] = t('No email found for !candidate.', array('!candidate' => $cand_name));

        if ( isset($candidates[$cand_id]['Webmail']) || isset($candidate[$cand_id]['Website']) ) {
          if ( isset($candidates[$cand_id]['Webmail']) ) {       
            $website = $candidates[$cand_id]['Webmail'];
          }      
          else
            $website = $candidates[$cand_id]['Website'];

          $element['#description'] = t('No email found for !candidate. Try contacting them at <a target="_blank" rel="nofollow" href="!website">their&nbsp;website</a>.', array(
            '!candidate' => $cand_name,
            '!website' => $website
          ));

        }  
      }

      $candidate_elements[$cand_id] = $element;
    }
  }//foreach 

  if ( $needs_jumper )
    $jumper_element['#value'] .= '</ul>';

 
  return $candidate_elements;
}

/**
 * Display the result of a submission for a component.
 * 
 * The output of this function will be displayed under the "Results" tab then
 * "Submissions". This should output the saved data in some reasonable manner.
 *
 * @param $component
 *   A Webform component array.
 * @param $value
 *   An array of information containing the submission result, directly
 *   correlating to the webform_submitted_data database table schema.
 * @param $format
 *   Either 'html' or 'text' . Defines the format that the content should be
 *   returned as. Make sure that returned content is run through check_plain()
 *   or other filtering functions when returning HTML.
 * @return
 *   A renderable element containing at the very least these properties:
 *    - #title
 *    - #weight
 *    - #component
 *    - #format
 *    - #value
 *   Webform also uses #theme_wrappers to output the end result to the user,
 *   which will properly format the label and content for use within an e-mail
 *   (such as wrapping the text) or as HTML (ensuring consistent output).
 */
function _webform_display_votesmart($component, $value, $format = 'html') {
  if ( !in_array($format, array('html', 'table-cell', 'text')) || empty($value) ) {
    return '';
  }

  $ret = array(
    '#weight' => $component['weight'],
    '#theme' => 'markup',
  );

  if ( $format == 'html' ) {
    $ret['#prefix'] = '<fieldset><legend>' . t($component['name']) . '</legend>';
    $ret['#suffix'] = '</fieldset>';
  }

  $value[0] = unserialize($value[0]);

  $candidates = $value[0]['votesmart']['candidates'];
  if ( !$candidates ) $candidates = array();
  $cand_names = array();

  if ( is_array($candidates) ) {
    foreach ($candidates as $cand_id => $checked ) {
      $c = votesmart_webform_get_candidate($cand_id);
      if ( $checked ) //sanity check{
        $name = _votesmart_webform_format_name($c, $component['extra']['name_format']);
        if ( !empty($c['Email']) ) {
          $e = $c['Email'];
          $name .= $format == 'text' ? ' ' . $e : ' ' . l($e, 'mailto:' . $e);
        }
        $cand_names[] = $name;
      }
    }//foreach
  }//if is_array
  $o = '';


  if ( !empty($value[0]['votesmart']['state']) ) {
    $o = t('State') . ': ' . $value[0]['votesmart']['state'] . "<br />\r\n";
  }

  if ( !empty($value[0]['votesmart']['zip']) ) {
    $o = t('Zip') . ': ' . $value[0]['votesmart']['zip'] . "<br />\r\n";
  }

  unset($value[0]);

  $o .= t('Officials Selected') . ': ';

  if ( $format == 'text' ) {
    $o .= "\r\n" . implode("\r\n", $cand_names) . "\r\n";
    $ret['#value'] = strip_tags($o);
    return array();//hack to keep this out of emails
    return $ret;//must return an array
  }
  else{
    $o .= '<ul><li>' . implode('</li><li>', $cand_names) . '</li></ul>';
  }

  if ( $format == 'table-cell' ) {
    return $o;
  }
 
  $ret['#value'] = $o;

  return $ret;
}

/**
 * Implementation of _webform_submit_component().
 *
 * NOTE: When webform sends out emails it will loop
 * through the returned array, validating the emails;
 * If the email is valid webform will send an email.
 */
function _webform_submit_votesmart($component, &$value) {
  if ( empty($value) || !is_array($value['votesmart']['candidates']) ) return NULL;

  $ret = array(0 => ''); //placeholder for serialized value
  foreach ($value['votesmart']['candidates'] as $cand_id => $checked) {
    if ( !$checked ) {
      unset($value['votesmart']['candidates'][$cand_id]);
    }
    else{
      $candidate = votesmart_webform_get_candidate($cand_id,NULL,TRUE);
      if ( !empty($candidate['Email']) ) {
        $ret[] = $candidate['Email'];
      }
    }
  }
  $ret[0] = serialize($value);
  return $ret;
}

function votesmart_webform_ahah_get_candidates() {
  $form_state = array('storage' =>  NULL, 'submitted' => FALSE);
  $form_build_id = $_POST['form_build_id'];
  $form = form_get_cache($form_build_id, $form_state);
  $args = $form['#parameters'];
  if ( !is_array($args) ) return drupal_json(array('status' => FALSE, 'data' => ''));

  $form_id = array_shift($args);
  $form['#post'] = $form_state['post'] = $_POST;
  $form['#redirect'] = $form['#programmed'] = FALSE;

  drupal_process_form($form_id, $form, $form_state);
  $form = drupal_rebuild_form($form_id, $form_state, $args, $form_build_id);

  $output = theme('status_messages');

  $form_portion = _votesmart_webform_element_children_search($form, 'votesmart') ;
  if ( $form_portion )
    $output .= drupal_render($form_portion);
  
  drupal_json(array('status' => TRUE, 'data' => $output));
}

function votesmart_webform_form_alter(&$form, $form_state, $form_id) {
  if ( preg_match('@webform_(.*)@', $form_id, $matches) ) {
    if ( $votesmart = _votesmart_webform_element_children_search($form, 'votesmart') ) {
      $form['#validate'][] = '_votesmart_webform_validate_candidates';
    }
  }
}

function _votesmart_webform_element_children_search($form, $key) {
  $element_children = element_children($form);
  $found = array_search($key, $element_children);
  if ( $found ) return $form[$key];

  if ( !is_array($element_children) ) return FALSE;

  foreach ($element_children as $e) {
    $result = _votesmart_webform_element_children_search($form[$e], $key);
    if ( $result != FALSE ) return $result;
  }
  return FALSE;
}

function _votesmart_webform_array_key_search($arr, $key) {
  if ( !is_array($arr) ) return FALSE;

  $found = array_key_exists($key, $arr);
  if ( $found ) return $arr[$key];

  if ( !is_array($arr) ) return FALSE;

  foreach ($arr as $e) {
    $result = _votesmart_webform_array_key_search($e, $key);
    if ( $result != FALSE ) return $result;
  }
  return FALSE;
}

function _votesmart_webform_validate_candidates($form, &$form_state, $first_run = TRUE) {
  $votesmart = _votesmart_webform_element_children_search($form, 'votesmart');
  if ( isset($votesmart['votesmart'] ) ) {
    $votesmart = $votesmart['votesmart'];
  }

  if ( ( isset($votesmart['zip']) && form_get_error($votesmart['zip']) )
      || ( isset($votesmart['state']) && form_get_error($votesmart['state']) )
  ) {
    return; // wait until we have a valid zip or state
  }
  
  if ( empty($votesmart['candidates']) ) {
    form_set_error('votesmart', t('Please select one or more officials.'));
  }

  $candidates = element_children($votesmart['candidates']);//alias

  $error = TRUE;
  if ( is_array($candidates) ) {
    foreach ( $candidates as $cand_id ) {
      if ( $votesmart['candidates'][$cand_id]['#value'] == 1 ) {
        $error = FALSE;
        break;
      }
    }//foreach
  }//if is_array
  
  if ( $error )
    form_set_error('votesmart', t('Please select one or more officials.'));
}

/**
 * Implementation of _webform_table_component().
 */
function _webform_table_votesmart($component, $value) {
  return _webform_display_votesmart($component, $value, 'table-cell');
}


/**
 * Implementation of _webform_csv_headers_component().
 */
function _webform_csv_headers_votesmart($component, $export_options) {
  return array(
    0 => array('', '', ''),
    1 => array($component['name'], $component['name'], $component['name']),
    2 => array('State', 'Zip', 'Selected Officials')
  );
}

/**
 * Implementation of _webform_csv_data_component().
 */
function _webform_csv_data_votesmart($component, $export_options, $value) {
  if ( !isset($value[0]) ) return '';

  $v = unserialize($value[0]);
  $v = $v['votesmart'];
  return array(
    $v['state'],
    $v['zip'],
    implode(';', array_keys($v['candidates']))
  );
}

