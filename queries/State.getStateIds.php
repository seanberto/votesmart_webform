<?php
//drush script thisfile.php > output.php
$query = _votesmart_webform_api('State.getStateIDs');
print_r($query);
