<?php
require_once('../InterfaceLib.inc');

$db= new InventoryAccess();
$db->databaseLogin($_COOKIE['username'], $_COOKIE['password']);

if(!empty($_POST['purposeItem']))
{
	
	$db->updateItemExtraInfo($_POST['purposeItem'], $_POST['purpose'], $_POST['usage_info'], $_POST['warning_info']);
}
header('Location: inventory.html');

?>