<?php
//drush script thisfile.php > output.php
$query = _votesmart_webform_api('Officials.getStatewide',array('stateId' => 'AZ'),'candidate');
print_r($query);
