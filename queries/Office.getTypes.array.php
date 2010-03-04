<?php
//drush script thisfile.php > output.php
$query = _votesmart_webform_api('Office.getTypes');
$types = $query['type'];
$o = '$VOTESMART_WEBFORM_OFFICE_TYPES = array(';
foreach($types as $t )
  $o .= '\''.$t->officeTypeId.'\' => array(\'name\'=> \''.$t->name.'\', \'level\' => \''.$t->officeLevelId.'\',\'branch\' => \''.$t->officeBranchId.'\'),'."\n";
$o .= ');';
echo $o;
