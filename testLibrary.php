#!/usr/bin/php
<?php

require_once('InterfaceLib.inc');

$testDB = new InventoryAccess();
$testDB->databaseLogin("BNICHOLS","12345");
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
$testDB->useItem(6515000002271,7);
$testDB->deleteItemFromInventory(6515000002271);
$testDB->uploadOrder("csvFiles/20170312Order.csv");

?>
