<?php
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
    'zip' => NULL,
    'type' => 'hidden',
    'extra' => array(
      'email' => 1,
      'allow_candidates_without_email' => 1,
      'hide_no_email_warnings' => 1,
      'width' => '',
      'attributes' => array(),
      'name_format' => '!last, !first',
      'hide_zip' => FALSE
    )
  );
}

function _votesmart_webform_format_name($candidate,$format = '!title !last, !first')
{
  return t($format,array(
    '!title' => $candidate['title'],
    '!first' => $candidate['firstName'],
    '!last' =>  $candidate['lastName'],
    '!middle' => $candidate['middleName'],
    '!nick' => $candidate['nickName']
  ));
}

/**
 * Generate the form for editing votesmart_webform component
 * @param $component
 *   A Webform component array.
 * @return
 *   An array of form items to be displayed on the edit component page
 */
function _webform_edit_votesmart($component) {
  $form = array('extra' => array());

  $form['extra']['zip'] = array(
    '#type' => 'textfield',
    '#title' => t('Specify Default Zip'),
    '#weight' => 0,
    '#default_value' => $component['extra']['zip'],
    '#element_validate' => array('_webform_edit_votesmart_default_zip_validate'),
  );

  $form['extra']['hide_zip'] = array(
    '#type' => 'checkbox',
    '#title' => t('Hide Zip'),
    '#weight'  => 0,
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
    '#description' => t('valid tokens include: !title !first !middle !last !nick'),
    '#default_value' => $component['extra']['name_format'] ? $component['extra']['name_format'] : '!title !last, !first');

  $form['extra']['width'] = array(
    '#type' => 'textfield',
    '#title' => t('Width'),
    '#default_value' => $component['extra']['width'],
    '#description' => t('Width of the textfield.') .' '. t('Leaving blank will use the default size.'),
    '#size' => 5,
    '#maxlength' => 10,
  );

  return $form;
}

/**
 * Validate the Default Zip Code
 * @param $zip_element The zip form element.
 * @param $form_state The full form state for the webform.
 */
function _webform_edit_votesmart_default_zip_validate($zip_element, &$form_state)
{
  $zip = $zip_element['#value'];//alias
  if( empty($zip) ) return;

  if( !votesmart_webform_is_zip($zip) )
  {
    form_error($zip_element,t('!value is not a valid zip code.',array('!value'=>$zip)));
  }
}

/**
 * Validate the Zip Code BLANK NOT ALLOWED
 * @param $zip_element The zip form element.
 * @param $form_state The full form state for the webform.
 */
function _webform_edit_votesmart_zip_validate($zip_element, &$form_state)
{
  $zip = $zip_element['#value'];//alias
  if( empty($zip) )
    form_error($zip_element,t('zip can not be blank.',array('!value'=>$zip)));

  if( !votesmart_webform_is_zip($zip) )
  {
    form_error($zip_element,t('!value is not a valid zip code.',array('!value'=>$zip)));
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

  drupal_add_css(drupal_get_path('module','votesmart_webform').'/votesmart_webform.css','module','all',false);

  if ($value[0]) {
    $zip_value = $value[0];
  }
  else {
    $zip_value = _webform_filter_values($component['zip']);
  }

  $dom_id = 'webform-component-'.$component['form_key'];
  $form = array(
    '#prefix' => '<div id="'.$dom_id.'" class="webform-component-votesmart">',
    'votesmart' => array(),
    '#suffix' => '</div><!--/#'.$dom_id.' .webform-component-votesmart-->',
    '#weight' => $component['weight']
  );

  //HACK: Need to find a better way
  $votesmart_post = _votesmart_webform_array_key_search($_POST,'votesmart');
  if( $votesmart_post )
    $component['zip'] = $votesmart_post['zip'];

  $form['votesmart']['zip'] = array(
    '#type' => $component['extra']['hide_zip'] ? 'hidden' : 'textfield',
    '#title' => t('Zip'),
    '#required' => TRUE,
    '#size' => 10,
    '#default_value' => $component['zip'],
    '#description'   => t('Postal Code in format 12345 or 12345-1234'),
    '#element_validate' => array('_webform_edit_votesmart_zip_validate'),
    '#ahah' => array(
      'event' => 'change',
      'wrapper' => $dom_id,
      'path' => 'votesmart_webform/ahah/zip_changed',
      'progress'=>array('type'=> 'throbber')

    )
  );


  if( !isset($form['votesmart']['zip']['#default_value'])
      && !empty($component['extra']['zip'])
  )
  {
    $form['votesmart']['zip']['#default_value'] = $component['extra']['zip'];
  }


  $zip = $form['votesmart']['zip']['#default_value'];//alias

  $candidates = votesmart_webform_candidate_form_elements($zip,$component);

  if( !empty($candidates) )
  {
    $form['votesmart']['candidates_prefix'] = array(
      '#type' => 'markup',
      '#value' => '<div class="votesmart-webform-candidate-wrapper">'
    );
    $form['votesmart']['candidates'] = $candidates;
    $form['votesmart']['candidate_suffix'] = array(
      '#type' => 'markup',
      '#value' => '</div><!--/.votesmart-webform-candidate-wrapper-->'
    );
  }

  return $form;
}

function votesmart_webform_candidate_form_elements($zip,$component)
{
  if( !votesmart_webform_is_zip($zip) ) return array();

  $candidates = votesmart_webform_get_candidates_by_zip($zip);
  $candidate_elements = array();
  
  $cand_names = array();
  foreach($candidates as $id => $c )
    $cand_names[$id] = _votesmart_webform_format_name($c,$component['extra']['name_format']);

  asort($cand_names);

  foreach($cand_names as $cand_id => $cand_name )
  {
    $element = array(
      '#type' => 'checkbox',
      '#title' => $cand_name,
    );

    if( !$component['extra']['hide_no_email_warnings'] && !isset($candidates[$cand_id]['Email']) )
    {
      if( !$component['extra']['allow_candidates_without_email'] )
        $element['#disabled'] = true;
      $element['#description'] = t('No email found for !candidate.',array('!candidate'=>$cand_name));
      if( isset($candidates[$cand_id]['Webmail']) || isset($candidate[$cand_id]['Website']) )
      {
        if( isset($candidates[$cand_id]['Webmail']) )
        {
          $website = $candidates[$cand_id]['Webmail'];
        }
        else
          $website = $candidates[$cand_id]['Website'];

        $element['#description'] = t('No email found for !candidate. Try contacting them at <a target="_blank" rel="nofollow" href="!website">their&nbsp;website</a>.',array(
          '!candidate'=>$cand_name,
          '!website' => $website
        ));

      }
    }

    $candidate_elements[$cand_id] = $element;
  }
  
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
 *   Either 'html' or 'text'. Defines the format that the content should be
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
  if( empty($value) ) return array();
  $ret = array(
    '#weight' => $component['weight'],
    '#theme' => 'markup',
    '#theme_wrappers' => $format == 'html' ? array('webform_element') : array('webform_element_text'),
    '#post_render' => array('webform_element_wrapper'),
    '#component' => $component,
    '#format' => $format,
  );

  $value = unserialize($value[0]);

  $candidates = $value['votesmart']['candidates'];
  if( !$candidates ) $candidates = array();
  $cand_names = array();

  foreach($candidates as $cand_id => $checked )
  {
    $c = votesmart_webform_get_candidate($cand_id);
    if( $checked ) //sanity check
      $cand_names[] = _votesmart_webform_format_name($c,$component['extra']['name_format']).' '.l($c['Email'],'mailto:'.$c['Email']);
  }
  $o = t('Zip').': '.$value['votesmart']['zip'].'<br />';
  $o .= t('Candidates Selected').': <ul><li>'.implode('</li><li>',$cand_names).'</li></ul>';

  $ret['#value'] = $o;
  
  return $ret;
}

/**
 * Implementation of _webform_submit_component().
 */
function _webform_submit_votesmart($component, &$value)
{
  if( empty($value) || !isset($value['votesmart']['candidates']) ) return null;
  foreach($value['votesmart']['candidates'] as $cand_id => $checked)
  {
    if( !$checked )
      unset($value['votesmart']['candidates'][$cand_id]);
  }

  $value = serialize($value);
  return $value;
}


/**
 * Module specific instance of hook_help().
 *
 * This allows each Webform component to add information into hook_help().
 */
function _webform_help_votesmart($section) {
  switch ($section) {
    case 'admin/settings/webform#votesmart_webform_description':
      return t('Allows for representative lookup by zip code.');
  }
}

function votesmart_webform_ahah_zip_changed()
{
  $form_state = array('storage' =>  NULL, 'submitted' => FALSE);
  $form_build_id = $_POST['form_build_id'];
  $form = form_get_cache($form_build_id, $form_state);
  $args = $form['#parameters'];
  $form_id = array_shift($args);
  $form['#post'] = $form_state['post'] = $_POST;
  $form['#redirect'] = $form['#programmed'] = FALSE;

  drupal_process_form($form_id, $form, $form_state);
  $form = drupal_rebuild_form($form_id, $form_state, $args, $form_build_id);

  $output = theme('status_messages');

  $form_portion = _votesmart_webform_element_children_search($form,'votesmart') ;
  if( $form_portion )
    $output .= drupal_render($form_portion);
  
  drupal_json(array('status' => TRUE, 'data' => $output));
}



function votesmart_webform_form_alter(&$form, $form_state, $form_id)
{
  if( preg_match('@webform_(.*)@',$form_id,$matches) )
  {
    if( $votesmart = _votesmart_webform_element_children_search($form,'votesmart') )
    {
      $form['#validate'][] = '_votesmart_webform_validate_candidates';
    }
  }
}

function _votesmart_webform_element_children_search($form,$key)
{
  $element_children = element_children($form);
  $found = array_search($key,$element_children);
  if( $found ) return $form[$key];

  foreach($element_children as $e)
  {
    $result = _votesmart_webform_element_children_search($form[$e],$key);
    if( $result != false ) return $result;
  }
  return false;
}

function _votesmart_webform_array_key_search($arr,$key)
{
  if( !is_array($arr) ) return false;

  $found = array_key_exists($key,$arr);
  if( $found ) return $arr[$key];

  foreach($arr as $e)
  {
    $result = _votesmart_webform_array_key_search($e,$key);
    if( $result != false ) return $result;
  }
  return false;
}




function _votesmart_webform_validate_candidates($form, &$form_state, $first_run = TRUE)
{
  $votesmart = _votesmart_webform_element_children_search($form,'votesmart');
  if( form_get_error($votesmart['zip']) )
    return; // wait until we have a valid zip

  $candidates = element_children($votesmart['candidates']);//alias

  $error = true;

  foreach( $candidates as $cand_id )
  {
    if( $votesmart['candidates'][$cand_id]['#value'] == 1 )
    {
      $error = false;
      break;
    }
  }
  
  if( $error )
    form_set_error('votesmart',t('Please select a candidate'));
}
