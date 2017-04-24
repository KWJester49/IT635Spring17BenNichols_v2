#!/usr/bin/php
<?php

require_once('InterfaceLib.inc');

$testDB = new InventoryAccess();
$testDB->databaseLogin("ADMIN","12345");
$testDB->ADMIN_reportUsers();

?>
