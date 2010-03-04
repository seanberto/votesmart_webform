<?php
//drush script thisfile.php > output.php
$query = _votesmart_webform_api('Officials.getStatewide',array('stateId' => 'NA'),'candidate');
print_r($query);
