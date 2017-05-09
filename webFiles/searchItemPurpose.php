<?php
require_once('../InterfaceLib.inc');

$db= new InventoryAccess();
$db->databaseLogin($_COOKIE['username'], $_COOKIE['password']);

if(!empty($_POST['purposeItem']))
{
	
	$db->displayItemsByPurpose($_POST['purposeItem']);
}
//header('Location: inventory.html');

?>