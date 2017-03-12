#!/usr/bin/php
<?php

require_once('InterfaceLib.inc');

$testDB = new InventoryAccess();
$testDB->databaseLogin("BNICHOLS","12345");
$testDB->uploadOrder("filename");
$testDB->addItem(6515000002266, "AIRLIFE ASTHMA CHECK", "Peak flow meter with asthma management zone system");
$testDB->reportInventory();
$testDB->reportItemsOrdered();
$testDB->receiveOrder("N0018370560460",6515000002157);
$testDB->useItem(6515000002157,12);
$testDB->updateParLevel(6515000002157, 40);
$testDB->updateParLevel(6515000002157, 80);
$testDB->reportItemsBelowParLevel();
$testDB->reportRecommendedParLevels();
$testDB->databaseLogin("ADMIN","12345");
$testDB->ADMIN_deleteItem(6515000002266);
$testDB->ADMIN_deleteOrder("N0018370560460");
$testDB->ADMIN_deleteVendor(1);
$testDB->useItem(6515000002271,7);
$testDB->deleteItemFromInventory(6515000002271);

$test=2;
echo $test.PHP_EOL;
if($test!=($test=5))
	echo "operation works".PHP_EOL;
echo $test.PHP_EOL;
?>
