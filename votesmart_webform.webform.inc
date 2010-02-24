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
    'pid' => 0,
    'weight' => 0,
    'zip' => NULL,
    'extra' => array(
      'width' => '',
      'attributes' => array(),
    )
  );
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
    '#element_validate' => array('_webform_edit_votesmart_default_zip_validate'),
  );

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
 * Define the component to structure to hook_form_builder_types().
 */
function _webform_form_builder_types_votesmart() {
  $fields = array();

  $fields['votesmart_webform_zip'] = array(
    'title' => t('Zip'),
    'properties' => array(
      'title',
      'description',
      'required',
      'size',
      'key'
    ),
    'default' => array(
      '#title' => t('Zip'),
      '#type' => 'textfield',
      '#size' => 9,
      '#required' => true
    ),
  );

  $fields['votesmart_webform_candidates'] = array(
    'title' => t('Candidates'),
    'properties' => array(
      'title',
      'description',
      'required',
      'size',
      'key'
    ),
    'default' => array(
      '#title' => t('Candidates'),
      '#type' => 'textfield',
    ),
  );


  return $fields;
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
  kpr($component);
  kpr($value);
  $form = array(
    '#prefix' => '<div id="webform-component-'.$component['form_key'].'" class="webform-component-votesmart">',
    'votesmart' => array(),
    '#suffix' => '</div><!--/.webform-component-votesmart-->'
  );
  $form['votesmart']['zip'] = array(
    '#type' => 'textfield',
    '#title' => t('Zip'),
    '#required' => TRUE,
    '#weight' => $component['weight'],
    '#description'   => _webform_filter_descriptions($component['extra']['description']),
  );

  $form['votesmart']['candidates'] = array(
    '#title' => t('Candidates'),
    '#type' => 'select',
    '#multiple' => true,
    '#weight' => $component['weight']+1
  );

  if( !isset($form['votesmart']['zip']['#default_value'])
      && !empty($component['extra']['zip'])
  )
  {
    $form['votesmart']['zip']['#default_value'] = $component['extra']['zip'];
  }

  if( votesmart_webform_is_zip($form['votesmart']['zip']['#default_value']) )
  {

    $form['votesmart']['candidates']['#options'] = votesmart_webform_get_candidate_options($form['votesmart']['zip']['#default_value']);

  }

  return $form;
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
  return array(
    '#title' => $component['name'],
    '#weight' => $component['weight'],
    '#theme' => 'webform_display_textfield',
    '#theme_wrappers' => $format == 'html' ? array('webform_element') : array('webform_element_text'),
    '#post_render' => array('webform_element_wrapper'),
    '#component' => $component,
    '#format' => $format,
    '#value' => isset($value[0]) ? $value[0] : '',
  );
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

