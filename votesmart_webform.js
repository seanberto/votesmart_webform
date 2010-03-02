function votesmart_webform_candidate_wrapper( $candidate_wrapper )
{
    if( $candidate_wrapper.length < 1 ) return;
    $selected_candidates = $('<div class="votesmart-webform-selected-candidates" />');
    $selected_candidates_wrapper = $('<div class="form-item">');

    $selected_candidates_wrapper.append('<label>Selected Candidates:</label>');
    $selected_candidates_wrapper.append($selected_candidates);

    $candidate_wrapper.after($selected_candidates_wrapper);
    $('input[type=checkbox]').change(function(){
      var $t = $(this);

      var selected_id = 'selected-'+ $t.attr('name').match(/\d+/)[0];
      var label = $t.parent().text();

      if( $t.filter(':checked').length == 1 )
      {
        $selected_candidate = $('<a href="#" title="uncheck candidate" id="'+ selected_id+'">'+ label +'; </a>');
        var that = this;
        $selected_candidate.click(function(){
          $(that).attr('checked','');
          $(this).remove();
          return false;
        });
        $selected_candidates.children().length == 0
        $selected_candidates.append($selected_candidate);
      }
      else
        $selected_candidates.children('#'+selected_id).trigger('click');
    });


}
jQuery(function(){
  $component = $('.webform-component-votesmart');
  $candidate_wrapper = $component.find('.votesmart-webform-candidate-wrapper');
  window.setTimeout(function(){
    votesmart_webform_candidate_wrapper($candidate_wrapper)
  },500);


  $component.bind('DOMNodeInserted',function(e){
    $new_content = $(e.target);
    $candidate_wrapper = $new_content.find('.votesmart-webform-candidate-wrapper');
    window.setTimeout(function(){
      votesmart_webform_candidate_wrapper($candidate_wrapper)
    },500);

  })
});
