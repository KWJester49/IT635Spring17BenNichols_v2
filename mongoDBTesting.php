
<?php

function respectTypeMap($responseValue)
{

	$returnValue=$responseValue;
	if($responseValue instanceof \stdClass)
	{
		$returnValue= (array)$responseValue;
	}

	if(is_array($returnValue))
	{
		foreach ($returnValue as &$value) 
		{
			$value= respectTypeMap($value);
		}
	}

	return $returnValue;
}

	try
{
	mongodb://<dbuser>:<dbpassword>@ds123311.mlab.com:23311/it635
	$mdb = new MongoDB\Driver\Manager("mongodb://webuser:psychotrope13@ds123311.mlab.com:23311/it635");	
	$command = new MongoDB\Driver\Command(['ping' => 1]);
	$mdb->executeCommand('db', $command);
	$servers = $mdb->getServers();
	print_r($servers);
	$item_id=6515000002157;
	//$filter = array('item_id'=>$item_id);
	$filter=  array();
	//echo "$filter[item_id]";
	$query = new MongoDB\Driver\Query($filter);
	/*$results = $mdb->executeQuery("it635.item_purposes",$query);
	$row=$results->toArray();
	
	$row= respectTypeMap($row);

	for($i=0;$i<sizeof($row);$i++)
	{
		$NSN=$row[$i]['item_id'];
		$purpose=$row[$i]['purpose'];
		echo "NSN: $NSN".PHP_EOL;
		echo "Purpose: $purpose".PHP_EOL;
	}*/

	$cursor= $mdb->executeQuery("it635.item_purposes",$query);
	$cursor->setTypeMap(['root'=>'array', 'document' =>'array', 'arra'=>'array']);
	$row=$cursor->toArray();
	for($i=0;$i<sizeof($row);$i++)
	{
		$NSN=$row[$i]['item_id'];
		$purpose=$row[$i]['purpose'];
		echo "NSN: $NSN".PHP_EOL;
		echo "Purpose: $purpose".PHP_EOL;
	}
	$writer= new MongoDB\Driver\BulkWrite;
	$writer->insert(array("item_id"=>6515000002157,"purpose"=>"tool to set up IV"));
	//$mdb->executeBulkWrite("it635.item_purposes", $writer);

	
}
catch(exception $e)
{
	print_r($e);
}
?>