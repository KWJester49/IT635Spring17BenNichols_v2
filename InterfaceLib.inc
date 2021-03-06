<?php

class InventoryAccess
{
	
	private $db;
	//private $mdb;
	private $permission;
	const GUEST=0;//Low level access of reports, cannot change anything in database
	const USER=3;//General user of the database and supply storeroom. Above plus can receive items, submit order files, and use items.
	const ADMIN=5;//Can do all of the above plus can manually change data (data stewarding) and create other users.

	public function __construct()
	{
		$this->db = new mysqli("localhost","root","12345","MainORDB");
		if ($this->db->connect_error)
		{
			echo "error connecting to databse: ".$this->db->connect_error.PHP_EOL;
			exit();
		}
		//$mdb = new MongoDB\Driver\Manager("mongodb://webuser:psychotrope13@ds123311.mlab.com:23311/it635");	
		//$command = new MongoDB\Driver\Command(['ping' => 1]);
		//$mdb->executeCommand('db', $command);
	}

	public function __destruct()
	{
		if (isset($this->db))
		{
			$this->db->close();
		}
	}

	/*
	Checks the provided username and password against the database, logging them into the DB.
	Sets their permission level associated with their logon.
	*/
	public function databaseLogin($username,$password)
	{
		$db_username = $this->db->real_escape_string($username);
		$db_password = md5($this->db->real_escape_string($password));

		$query = "select * from users where user_name = '$db_username';";
		$result= $this->db->query($query);
		while ($row = $result->fetch_assoc())
		{
			if ($row["password"] == $db_password)
			{
				$this->permission = $row["permissions"];
				return true;
			}
		}
		return false;
	}

	/*
	This function will be called by all the modifying 
	*/
	private function permissions($req_permission, $task_error_msg)
	{
		if($this->permission >= $req_permission)
		{
			return true;
		}
		else
		{
			echo "<h3>Do not have authorization to $task_error_msg.</h3>";
			return false;
		}
	}

	/*
	This function will take the results from a mongo query and turn them from std class objects to arrays, including sub arrays.
	*/
	function mongoTypeMap($responseValue)
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
				$value= mongoTypeMap($value);
			}
		}

		return $returnValue;
	}

		/*
	Takes .csv file output from ordering system and uploads into the database.
	Will insert into the orders and order_line_items tables (and into the inventory table if it does not exist there).
	Will mark items as on order in the inventory table.
	Order of .csv file is order_id, vendor_name, date_ordered, item_id, and quantity
	*/
	public function uploadOrder($filename)
	{
		//Make sure user has permission to upload an order
		if(!$permission=$this->permissions(self::USER, "upload an order"))
			return $permission;
		echo "Can execute".PHP_EOL;

		/*
		Should really take the outputs and put them into a temporary file, then load that, so you can see what the output is before loading or in case of problems.
		*/
		if($file_input=fopen($filename, "r"))
		{
			$comp_order_id= NULL;
			$vendor_id= NULL;
			while($data= fgetcsv($file_input))
			{
				$order_id= $data[0];

				//if its a new order_id, insert data into orders table
				if($comp_order_id != $order_id)
				{
					$comp_order_id= $order_id;
					$vendor_name= $data[1];
					$query= "SELECT vendor_id FROM vendors WHERE vendor_name = '$vendor_name';";
					$result= $this->db->query($query);
					$row= $result->fetch_assoc();
					$vendor_id= $row["vendor_id"];
					$date_ordered= $data[2];

					$sql= "INSERT INTO orders (order_id, vendor_id, date_ordered) VALUES ('$order_id', $vendor_id, '$date_ordered');";
					$this->db->query($sql);
				}
				
				$item_id= $data[3];
				$quantity= $data[4];
				
				$query= "SELECT price FROM vendor_prices WHERE vendor_id=$vendor_id AND item_id=$item_id;";
				$result= $this->db->query($query);
				$row= $result->fetch_assoc();
				$unit_price= $row["price"];

				$sql= "INSERT INTO order_line_items (order_id, item_id, quantity, unit_price) VALUES ('$order_id', $item_id, $quantity, $unit_price);";
				$this->db->query($sql);


				$sql= "UPDATE inventory SET on_order= 1 WHERE item_id=$item_id;";
				$this->db->query($sql);
			}

			fclose($file_input);
		}

	}

	/*
	error handling function for receiveOrder()
	*/
	private function receiveOrderError($order_id, $item_id)
	{
		echo "Could not receive item $item_id for order $order_id.".PHP_EOL;
		echo "Possible problems:\n1. Bad item number\n2. Bad order number\n3. Already been received".PHP_EOL;
		return false;
	}

	/*
	When personnel receive an order that had been placed.  
	For an order_id and item_id it will mark the current date as receiving that item in the order_line_item table,
	it will then increase the quantity for that item_id in the inventory table,
	then it will mark it as no longer on order in the inventory table.
	*/
	public function receiveOrder($order_id, $item_id)
	{
		//Make sure user has the permissions to receive an order.
		if(!$permission=$this->permissions(self::USER, "receive an order"))
			return $permission;
		
		//Mark the material as received with the current date in the order_line_items table
		$date= date("Y-m-d"); //current date
		$sql= "UPDATE order_line_items SET date_received='$date' WHERE order_id='$order_id' AND item_id=$item_id AND date_received IS NULL;";

		//Problem running query, or if after running no rows are affected (didn't update any rows),
		//meaning already received, or problem with $order_id, $item_id, or the combo of the two.
		if(!$this->db->query($sql) || $this->db->affected_rows==0)
		{
			return $this->receiveOrderError($order_id, $item_id);
		}
		//Doesn't update any rows, meaning already received, or problem with $order_id, $item_id, or the combo of the two.
				
		//Get the quantity ordered and quantity on hand in the inventory, then update the inventory with their sum
		$query= "SELECT order_line_items.quantity, inventory.quantity FROM order_line_items, inventory WHERE order_line_items.order_id='$order_id' AND order_line_items.item_id=$item_id AND order_line_items.item_id=inventory.item_id;";
		$result= $this->db->query($query);
		$row= $result->fetch_row();
		$new_inventory= $row[0]+$row[1];
		$sql= "UPDATE inventory SET quantity=$new_inventory, on_order=0 WHERE item_id=$item_id;";

		//Problem running query, or after running, that no rows were updated
		if(!$this->db->query($sql) || $this->db->affected_rows==0)
		{
			return $this->receiveOrderError($order_id, $item_id);
		}
		return true;
	}

	/*
	Gets a list of receivable order (order_id & item_id combinations)
	*/
	public function getReceivableOrders()
	{
		//Make sure user has the permissions to receive an order.
		if(!$permission=$this->permissions(self::USER, "receive an order"))
			return $permission;

		$query= "SELECT order_id, item_id FROM order_line_items	WHERE date_received IS NULL;";
		$result= $this->db->query($query);

		$html= "<h5>&ensp; Order Number &ensp;&nbsp; NSN</h5>";
		while($row=$result->fetch_assoc())
		{
			$order_id= $row['order_id'];
			$item_id= $row['item_id'];
			$combinedValue= $order_id.$item_id;
			$html .= "<div class='checkbox'><label><input type='checkbox' name='receivableOrder[]' value=$combinedValue>$order_id &emsp; $item_id</label></div>";
		}
		echo $html;
	}


	/*
	This function is used to mark that an item has been used.  
	It will reduce the quantity in the inventory by $quantity.
	It will then insert a new row into historical usage for that item and quantity at the time it is used (using the computer's clock).
	*/
	public function useItem($item_id, $quantity)
	{
		//Make sure user has the permissions to use an item.
		if(!$permission=$this->permissions(self::USER, "use an item"))
			return $permission;
		
		//Check that inventory.quantity is >= $quantity
		$query= "SELECT quantity FROM inventory WHERE item_id=$item_id;";
		$result= $this->db->query($query);
		$row= $result->fetch_assoc();
		$inventory_quantity=$row["quantity"];
		if($quantity>$inventory_quantity)
		{
			echo "$quantity is bigger than the quantity on hand: $inventory_quantity.".PHP_EOL;
			return false;
		}
		$inventory_quantity-=$quantity;
		$sql= "UPDATE inventory SET quantity=$inventory_quantity WHERE item_id=$item_id;";

		//Problem updating inventory's quantity
		if(!$this->db->query($sql) || $this->db->affected_rows==0)
		{
			echo "problem with updating inventory's quantity".PHP_EOL;
			return false;
		}

		//Record the number of the item used in the historical usage table
		$datetime= date("Y-m-d H:i:s"); //current date
		$sql= "INSERT INTO historical_usage (date_used, item_id, quantity) VALUES ('$datetime', $item_id, $quantity);";
		//Problem updating inventory's quantity
		if(!$this->db->query($sql) || $this->db->affected_rows==0)
		{
			echo "problem updating historical usage".PHP_EOL;
			return false;
		}
		return true;
	}

	/*
	Displays items and quantities so they can be selected to be used.
	*/
	public function getUsableItems()
	{
		//Make sure user has the permissions to use an item.
		if(!$permission=$this->permissions(self::USER, "use an item"))
			return $permission;

		$query= "SELECT item_id FROM inventory;";
		$result= $this->db->query($query);

		$html= "<div class='form-group'>
					<h5>NSN</h5>
					<select class='form-control' name='usedItem'>
						<option disabled selected value> Select an NSN </option>";
		while($row=$result->fetch_assoc())
		{
			$item_id= $row['item_id'];
			$html .= "<option value=$item_id>$item_id</option>";
		}
		$html .= "</select><h5>Quantity</h5><input type='number' name='quantity'></div>";
		echo $html;
	}

	/*
	This function is used to update the par level (low water mark) of an item in the inventory.
	*/
	public function updateParLevel($item_id, $par_level)
	{
		//Make sure user has the permissions to update the par level.
		if(!$permission=$this->permissions(self::USER, "update the par level"))
			return $permission;

		//Checks the par level you want to change vs the current par level, if it is more than a 50% change then you need admin rights
		$query= "SELECT par_level from inventory WHERE item_id=$item_id;";
		$result= $this->db->query($query);
		$row= $result->fetch_assoc();
		$current_par_level= $row["par_level"];
		if((($par_level >= ($current_par_level*1.5)) ||  ($par_level <= ($current_par_level*0.5))) && 
			(!$permission=$this->permissions(self::ADMIN, "update the par level by more than 50% of its current value")))
		{
			return $permission;
		}

		$sql= "UPDATE inventory SET par_level=$par_level WHERE item_id=$item_id;";

		//Problem updating inventory's par_level
		if(!$this->db->query($sql) || $this->db->affected_rows==0)
		{
			echo "problem with updating inventory's par_level".PHP_EOL;
			return false;
		}
		return true;
	}

	/*
	Get items from inventory table to be used to update par levels.
	*/
	public function getItemsParUpdate()
	{
		//Make sure user has the permissions to update the par level.
		if(!$permission=$this->permissions(self::USER, "update the par level"))
			return $permission;

		$query= "SELECT item_id FROM inventory;";
		$result= $this->db->query($query);

		$html= "<div class='form-group'>
					<h5>NSN</h5>
					<select class='form-control' name='usedItem'>
						<option disabled selected value> Select an NSN </option>";
		while($row=$result->fetch_assoc())
		{
			$item_id= $row['item_id'];
			$html .= "<option value=$item_id>$item_id</option>";
		}
		$html .= "</select><h5>Quantity</h5><input type='number' name='par_level'></div>";
		echo $html;
	}

	/*
	This function is used to update the par level (low water mark) of an item in the inventory.
	*/
	public function updateItemExtraInfo($item_id, $purpose, $usage_info, $warning_info)
	{
		//Make sure user has the permissions to update the par level.
		if(!$permission=$this->permissions(self::USER, "update extra info"))
			return $permission;

		$mdb = new MongoDB\Driver\Manager("mongodb://webuser:psychotrope13@ds123311.mlab.com:23311/it635");	
		$command = new MongoDB\Driver\Command(['ping' => 1]);
		$mdb->executeCommand('db', $command);

		$writer= new MongoDB\Driver\BulkWrite;
		$writer->delete(array("item_id"=>$item_id));
		$mdb->executeBulkWrite("it635.item_purposes", $writer);
		$writer->insert(array("item_id"=>$item_id,"purpose"=>$purpose, "usage_info"=>$usage_info, "warning_info"=>$warning_info));
		$mdb->executeBulkWrite("it635.item_purposes", $writer);
		
	}
	

	/*
	Get items from inventory table to add a purpose to.
	*/
	public function getItemsAddExtraInfo()
	{
		//Make sure user has the permissions to update the par level.
		if(!$permission=$this->permissions(self::USER, "update extra info"))
			return $permission;

		$query= "SELECT item_id FROM inventory;";
		$result= $this->db->query($query);

		$html= "<div class='form-group'>
					<h5>NSN</h5>
					<select class='form-control' name='purposeItem'>
						<option disabled selected value> Select an NSN </option>";
		while($row=$result->fetch_assoc())
		{
			$item_id= $row['item_id'];
			$html .= "<option value=$item_id>$item_id</option>";
		}
		$html .= "</select>
			<h5>Purpose</h5><input type='text' name='purpose'>
			<h5>Usage Info</h5><input type='text' name='usage_info'>
			<h5>Warning Info</h5><input type='text' name='warning_info'></div>";
		echo $html;
	}

	/*
	This function is used display inventory matching the specified purpose.
	*/
	public function displayItemsByPurpose($purpose)
	{
		//Make sure user has the permissions to update the par level.
		if(!$permission=$this->permissions(self::GUEST, "search for item by purpose"))
			return $permission;

		$mdb = new MongoDB\Driver\Manager("mongodb://webuser:psychotrope13@ds123311.mlab.com:23311/it635");	
		$command = new MongoDB\Driver\Command(['ping' => 1]);
		$mdb->executeCommand('db', $command);

		$filter=  array('purpose'=>"$purpose");
		$mongoQuery = new MongoDB\Driver\Query($filter);
		$mongoResults = $mdb->executeQuery("it635.item_purposes",$mongoQuery);
		$mongoResults->setTypeMap(['root'=>'array', 'document' =>'array', 'arra'=>'array']);
		$mongoData=$mongoResults->toArray();
		$item_ids=array();
		for($i=0;$i<sizeof($mongoData);$i++)
		{
			$item_ids[]=intval($mongoData[$i]['item_id']);
		}
		
		//get MySQL inventory data
		$query= "SELECT inventory.item_id, items.item_name, items.item_desc, inventory.quantity, inventory.location, inventory.par_level, inventory.on_order
			FROM inventory, items WHERE inventory.item_id=items.item_id;";
		$result = $this->db->query($query);
		$html= "<!DOCTYPE html>
<html>
<head>
	<title>Inventory Page</title>
	<meta charset='utf-8'>
	<meta name='viewport' content='width=device-width, initial-scale=1'>
	<link rel='stylesheet' href='css/bootstrap.min.css'>
	<link rel='stylesheet' href='css/databaseCSS.css'>
	<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.1.1/jquery.min.js'></script>
	<script src='js/bootstrap.min.js'></script>
</head>
<body>
<div class='text-center'><h3>Search Results for: $purpose</h3></div>
<div class='row'>
	<div class='col-md-12'>

				<div class='container'><div class='row'><div class='col-md'12'>
				<table class='table table-bordered table-striped table-condensed table-responsive'>
					<thead>
						<tr>
							<th>National Stock Number</th>
							<th>Item Name</th>
							<th>Item Description</th>
							<th>Quantity</th>
							<th>Location</th>
							<th>Par Level</th>
							<th>On Order</th>
							<th>Purpose</th>
							<th>Usage Info</th>
							<th>Warning Info</th>
						</tr>
					</thead>
					<tbody>";
		while ($row = $result->fetch_row())
		{
			$item_id= $row[0];
			if(in_array($item_id, $item_ids))
			{
				$html .= "<tr>";
				
				for($i=0; $i<sizeof($row); $i++)
				{
					if($i==6)
					{
						if($row[$i]==1)
						{
							$row[$i]="Yes";
						}
						else
						{
							$row[$i]="No";
						}
					}
					$html .= "<td> $row[$i] </td>";
				}

				//search through the results of mongo search for their NSN, output the purpose
				$purpose= "";
				$usage_info="";
				$warning_info="";
				for($i=0; $i<sizeof($mongoData); $i++)
				{
					if($item_id==$mongoData[$i]['item_id'])
					{
						$purpose=$mongoData[$i]['purpose'];
						$usage_info=$mongoData[$i]['usage_info'];;
						$warning_info=$mongoData[$i]['warning_info'];;
					}
				}
				$html .= "<td> $purpose</td>";
				$html .= "<td> $usage_info</td>";
				$html .= "<td> $warning_info</td>";
				$html .= "</tr>";
			}
		}
		$html .= "	</tbody>
				</table>
				</div></div></div></div></div></body></html>";
		echo $html;
		
	}
	

	/*
	Get items from inventory table to add a purpose to.
	*/
	public function getItemsByPurpose()
	{
		//Make sure user has the permissions to update the par level.
		if(!$permission=$this->permissions(self::GUEST, "search for item by purpose"))
			return $permission;

		$mdb = new MongoDB\Driver\Manager("mongodb://webuser:psychotrope13@ds123311.mlab.com:23311/it635");	
		$command = new MongoDB\Driver\Command(['ping' => 1]);
		$mdb->executeCommand('db', $command);
		$filter=  array();
		$query = new MongoDB\Driver\Query($filter);
		$results = $mdb->executeQuery("it635.item_purposes",$query);
		$results->setTypeMap(['root'=>'array', 'document' =>'array', 'arra'=>'array']);
		$row=$results->toArray();
		$purposes=array();
		for($i=0;$i< sizeof($row);$i++)
		{
			$purpose=$row[$i]['purpose'];
			if(!in_array($purpose, $purposes))
			{
				$purposes[]=$purpose;
			}
			
		}

		
		$html= "<div class='form-group'>
					<h5>Purposes</h5>
					<select class='form-control' name='purposeItem'>
						<option disabled selected value> Select a purpose </option>";
		for($i=0;$i< sizeof($purposes);$i++)
		{
			$purpose= $purposes[$i];
			$html .= "<option value='$purpose'>$purpose</option>";
		}
		$html .= "</select></div>";
		echo $html;
	}	

	/*
	This function will remove an item from inventory if it is no longer carried
	*/
	public function deleteItemFromInventory($item_id)
	{
		//Make sure user has the permissions to remove an item from inventory.
		if(!$permission=$this->permissions(self::USER, "remove item from inventory"))
			return $permission;

		$query= "SELECT quantity FROM inventory WHERE item_id=$item_id;";
		$result= $this->db->query($query);
		$row=$result->fetch_assoc();
		if($row["quantity"]>0)
		{
			echo "Can't remove $item_id from inventory because it is still in stock.".PHP_EOL;
			return false;
		}

		$sql= "DELETE FROM inventory WHERE item_id=$item_id;";
		$this->db->query($sql);

	}


	/*
	Runs a report of items on inventory
	*/
	public function reportInventory()
	{
		//Make sure user has permission to run inventory report. Everyone as of now.
		if(!$permission=$this->permissions(self::GUEST, "run an Inventory Report"))
			return $permission;
		
		//Get mongo data about inventory
		$mdb = new MongoDB\Driver\Manager("mongodb://webuser:psychotrope13@ds123311.mlab.com:23311/it635");	
		$command = new MongoDB\Driver\Command(['ping' => 1]);
		$mdb->executeCommand('db', $command);
		$filter = array();
		$mongoQuery = new MongoDB\Driver\Query($filter);
		$mongoResults = $mdb->executeQuery("it635.item_purposes",$mongoQuery);
		$mongoResults->setTypeMap(['root'=>'array', 'document' =>'array', 'array'=>'array']);
		$mongoData=$mongoResults->toArray();
		
		//get MySQL inventory data
		$query= "SELECT inventory.item_id, items.item_name, items.item_desc, inventory.quantity, inventory.location, inventory.par_level, inventory.on_order
			FROM inventory, items WHERE inventory.item_id=items.item_id;";
		$result = $this->db->query($query);
		$html= "<div class='container'><div class='row'><div class='col-md'12'>
				<table class='table table-bordered table-striped table-condensed table-responsive'>
					<thead>
						<tr>
							<th>National Stock Number</th>
							<th>Item Name</th>
							<th>Item Description</th>
							<th>Quantity</th>
							<th>Location</th>
							<th>Par Level</th>
							<th>On Order</th>
							<th>Purpose</th>
							<th>Usage Info</th>
							<th>Warning Info</th>
						</tr>
					</thead>
					<tbody>";
		while ($row = $result->fetch_row())
		{
			$html .= "<tr>";
			$item_id= intval($row[0]);
			for($i=0; $i<sizeof($row); $i++)
			{
				if($i==6)
				{
					if($row[$i]==1)
					{
						$row[$i]="Yes";
					}
					else
					{
						$row[$i]="No";
					}
				}
				$html .= "<td> $row[$i] </td>";
			}

			//search through the results of mongo search for their NSN, output the purpose
			$purpose= "";
			$usage_info="";
			$warning_info="";
			for($i=0; $i<sizeof($mongoData); $i++)
			{
				if($item_id==$mongoData[$i]['item_id'])
				{
					$purpose=$mongoData[$i]['purpose'];
					$usage_info=$mongoData[$i]['usage_info'];;
					$warning_info=$mongoData[$i]['warning_info'];;
				}
			}
			$html .= "<td> $purpose</td>";
			$html .= "<td> $usage_info</td>";
			$html .= "<td> $warning_info</td>";
			$html .= "</tr>";
		}
		$html .= "	</tbody>
				</table>
				</div></div></div>";
		echo $html;
	}

	/*
	Runs a report of items on order
	*/
	public function reportItemsOrdered()
	{
		//Reports of items on order available to everyone as of now
		if(!$permission=$this->permissions(self::GUEST, "run an On Order Report"))
			return $permission;
		
		
		$query="SELECT orders.order_id, order_line_items.item_id, items.item_name, order_line_items.quantity, order_line_items.unit_price, vendors.vendor_name, orders.date_ordered, order_line_items.date_received
		FROM orders, order_line_items, items, vendors
		WHERE orders.order_id=order_line_items.order_id 
		AND order_line_items.item_id=items.item_id 
		AND orders.vendor_id=vendors.vendor_id";
		$result = $this->db->query($query);
		$html= "<div class='container'><div class='row'><div class='col-md-12'>
				<table class='table table-bordered table-striped table-condensed table-responsive'>
					<thead>
						<tr>
							<th>Order Number</th>
							<th>NSN</th>
							<th>Item Name</th>
							<th>Quantity</th>
							<th>Unit Price</th>
							<th>Vendor Name</th>
							<th>Date Ordered</th>
							<th>Date Received</th>
						</tr>
					</thead>
					<tbody>";
		while ($row = $result->fetch_row())
		{
			$html .= "<tr>";
			for($i=0; $i<sizeof($row); $i++)
			{
				$html .= "<td> $row[$i] </td>";
			}
			$html .= "</tr>";
		}
		$html .= "	</tbody>
				</table>
				</div></div></div>";
		echo $html;
	}

	/*
	Runs a report of items at or below their par level (low water mark)
	*/
	public function reportItemsBelowParLevel()
	{
		//Reports of items at or below par level available to everyone as of now
		if(!$permission=$this->permissions(self::GUEST, "run a Par Level Report"))
			return $permission;
		
		
		$query="SELECT inventory.item_id, items.item_name, inventory.quantity, inventory.par_level FROM inventory, items WHERE inventory.item_id=items.item_id AND inventory.quantity<=inventory.par_level;";
		$result = $this->db->query($query);
		$html= "<div class='container'><div class='row'><div class='col-md-12'>
				<table class='table table-bordered table-striped table-condensed table-responsive' style='margin: auto; width:50%;'>
					<thead>
						<tr>
							<th>NSN</th>
							<th>Item Name</th>
							<th>Quantity</th>
							<th>Par Level</th>
						</tr>
					</thead>
					<tbody>";
		while ($row = $result->fetch_row())
		{
			
			$html .= "<tr>";
			for($i=0; $i<sizeof($row); $i++)
			{
				$html .= "<td> $row[$i] </td>";
			}
			$html .= "</tr>";
		}
		$html .= "	</tbody>
				</table>
				</div></div></div>";
		echo $html;
	}

	/*
	Gets the database's recommended par levels.  
	One of the problems with the storeroom is excessive stock.  
	With the quick and guaranteed delivery time of most items (overnight for over 80% of items), 
	the plan is to only keep on hand double the par level.
	The par level should be enough that when supplies are reordered, they will be close to running out (but not actually run out, so buffer is added in).
	With this system, most supplies will be ordered multiple times per week, but in low quantities so the storeroom has what it needs and only that.
	Par level should be calculated as average time that it takes for an order to arrive for that line item, multiplied by the burn rate of that item.
	Buffer should be double the average delivery time (should be very short).
	*/
	public function reportRecommendedParLevels()
	{
		//Reports of recommended par levels for items.   
		if(!$permission=$this->permissions(self::GUEST, "run a Recommended Par Level Report"))
			return $permission;

		$query="SELECT historical_usage.item_id, items.item_name, MIN(historical_usage.date_used), MAX(historical_usage.date_used), SUM(historical_usage.quantity)
			FROM historical_usage, items
			WHERE items.item_id=historical_usage.item_id 
			Group By historical_usage.item_id;";
		$result= $this->db->query($query);
		$html= "<div class='container'><div class='row'><div class='col-md-12'>
				<table class='table table-bordered table-striped table-condensed table-responsive' style='margin: auto; width:50%;'>
					<thead>
						<tr>
							<th>NSN</th>
							<th>Item Name</th>
							<th>Recommended Par Level</th>
						</tr>
					</thead>
					<tbody>";
		while($row= $result->fetch_row())
		{
			$item_id=$row[0];
			$item_name=$row[1];

			$old_date= new DateTime($row[2]);
			$new_date= new DateTime($row[3]);
			$diff= $old_date->diff($new_date);
			$ave_use=$row[4]/($diff->days); //average number of item used per day (BURN RATE)
			
			//Get the average time it takes for an order to be delivered for this item
			$query2= "SELECT AVG(datediff(order_line_items.date_received, orders.date_ordered)) 
				FROM orders, order_line_items 
				WHERE order_line_items.order_id=orders.order_id
				AND order_line_items.item_id=$item_id;";
			$result2= $this->db->query($query2);
			$row2=$result2->fetch_row();

			$par_level=2*$ave_use*$row2[0];//double the order time * burn rate
			$html .= "<tr>
						<td> $item_id </td>
						<td> $item_name </td>
						<td> $par_level </td>
					</tr>";
		}
		$html .= "	</tbody>
				</table>
				</div></div></div>";
		echo $html;
	}

	/*
	Get the historical usage of items in the database.
	*/
	public function reportHistoricalUsage()
	{
		//Reports of historical usage of items.   
		if(!$permission=$this->permissions(self::GUEST, "run a Historical Usage of Items Report"))
			return $permission;

		$query="SELECT historical_usage.date_used, historical_usage.item_id, items.item_name, historical_usage.quantity
			FROM historical_usage, items
			WHERE items.item_id=historical_usage.item_id 
			ORDER BY historical_usage.date_used DESC;";
		$result= $this->db->query($query);
		$html= "<div class='container'><div class='row'><div class='col-md-12'>
				<table class='table table-bordered table-striped table-condensed table-responsive' style='margin: auto; width:50%;'>
					<thead>
						<tr>
							<th>Date Used</th>
							<th>NSN</th>
							<th>Item Name</th>
							<th>Quantity Used</th>
						</tr>
					</thead>
					<tbody>";
		while ($row = $result->fetch_row())
		{
			
			$html .= "<tr>";
			for($i=0; $i<sizeof($row); $i++)
			{
				$html .= "<td> $row[$i] </td>";
			}
			$html .= "</tr>";
		}
		$html .= "	</tbody>
				</table>
				</div></div></div>";
		echo $html;
	}



	/*
	Add an item to the database.
	Supply technicians (USER) should be able to add items smartly (be trained),
	rather than only being an ADMIN role.
	*/
	public function ADMIN_addItem($item_id,$item_name,$item_desc)
	{
		//Check if user has permission to add an item
		if(!$permission=$this->permissions(self::ADMIN, "add an item"))
			return $permission;
		
		$query="INSERT INTO items (item_id, item_name, item_desc) VALUES ('$item_id', '$item_name', '$item_desc');";
		if(!$this->db->query($query))
		{
			echo "failed to add $item_id".PHP_EOL;
		}

		$query="INSERT INTO inventory (item_id, quantity) VALUES ('$item_id', 0);";
		if(!$this->db->query($query))
		{
			echo "failed to add $item_id to inventory".PHP_EOL;
		}
	}

	/*
	Delete an item from the items table (only available to admin)
	*/
	public function ADMIN_deleteItem($item_id)
	{
		//Check if admin rights to delete an item from items table
		if(!$permission=$this->permissions(self::ADMIN, "delete an Item"))
			return $permission;

		$sql= "DELETE FROM inventory WHERE item_id=$item_id;";
		if(!$this->db->query($sql))
		{
			echo "Error deleting $item_id from the DB.".PHP_EOL;
			return false;
		}

		$sql= "DELETE FROM items WHERE item_id=$item_id;";
		if(!$this->db->query($sql))
		{
			echo "Error deleting $item_id from the DB.".PHP_EOL;
			return false;
		}
		return true;
	}

	/*
	Update the inventory quantity.  
	Used for when a mistake was made in using or receiving an item, or for gains/loss by physical inventory.
	*/
	public function ADMIN_updateInventoryQuantity($item_id, $quantity)
	{
		//Check if admin rights to update the quantity in the inventory
		if(!$permission=$this->permissions(self::ADMIN, "update inventory quantity"))
			return $permission;

		$sql= "UPDATE inventory SET quantity=$quantity WHERE item_id=$item_id;";

		//Problem updating inventory's quantity
		if(!$this->db->query($sql) || $this->db->affected_rows==0)
		{
			echo "problem with updating inventory's quantity".PHP_EOL;
			return false;
		}
	}

	/*
	Get the item id for items in inventory, to be used with ADMIN update inventory quantity and update inventory information
	*/
	public function ADMIN_getUpdatableInventoryQuantity()
	{
		//Check if admin rights to update information in the inventory
		if(!$permission=$this->permissions(self::ADMIN, "update inventory information"))
			return $permission;

		$query= "SELECT item_id, quantity FROM inventory;";
		$result= $this->db->query($query);

		$html= "<div class='form-group'><h5>NSN &emsp;&emsp;&emsp;&emsp; Current Quantity</h5>
			<select class='form-control' name= 'item_id'>
				<option disabled selected value> Select an NSN </option>";
		while($row=$result->fetch_assoc())
		{
			$item_id= $row['item_id'];
			$quantity= $row['quantity'];
			$html .= "<option value= '$item_id'>$item_id &emsp; $quantity</option>";
		}
		$html .= "</select><h5>New Quantity</h5><input type='number' name='quantity'></div>";
		echo $html;
	}

	/*
	Update the inventory quantity.  
	Used for when a mistake was made in using or receiving an item, or for gains/loss by physical inventory.
	*/
	public function ADMIN_updateInventoryInfo($item_id, $quantity, $location, $par_level, $on_order)
	{
		//Check if admin rights to update the quantity in the inventory
		if(!$permission=$this->permissions(self::ADMIN, "update inventory information"))
			return $permission;

		$sql= "UPDATE inventory SET quantity=$quantity, location='$location', par_level=$par_level, on_order=$on_order WHERE item_id=$item_id;";

		//Problem updating inventory's quantity
		if(!$this->db->query($sql) || $this->db->affected_rows==0)
		{
			echo "problem with updating inventory's info".PHP_EOL;
			return false;
		}
	}

	/*
	Get the item id for items in inventory, to be used with ADMIN update inventory quantity and update inventory information
	*/
	public function ADMIN_getUpdatableInventoryInfo()
	{
		//Check if admin rights to update information in the inventory
		if(!$permission=$this->permissions(self::ADMIN, "update inventory information"))
			return $permission;

		$query= "SELECT item_id FROM inventory;";
		$result= $this->db->query($query);

		$html= "<div class='form-group'><h5>NSN</h5>
			<select class='form-control' name= 'item_id'>
				<option disabled selected value> Select an NSN </option>";
		while($row=$result->fetch_assoc())
		{
			$item_id= $row['item_id'];
			$html .= "<option value= '$item_id'>$item_id</option>";
		}
		$html .= "</select></div>";
		echo $html;
	}

	/*

	*/
	public function ADMIN_getUpdatableInventoryInfoForItemID($item_id)
	{
		//Check if admin rights to update information in the inventory
		if(!$permission=$this->permissions(self::ADMIN, "update inventory information"))
			return $permission;
		echo "<h1>$item_id</h5>";
		$query= "SELECT * FROM inventory WHERE item_id=$item_id;";
		$result= $this->db->query($query);
		return $result->fetch_assoc();
	}

	/*
	Delete an order from the orders database (only available to admin)
	*/
	public function ADMIN_deleteOrder($order_id)
	{
		//Check if admin rights to delete an order
		if(!$permission=$this->permissions(self::ADMIN, "delete an Order"))
			return $permission;

		//delete from order_line_items first, because its a child table of orders
		$sql= "DELETE FROM order_line_items WHERE order_id='$order_id';";
		if(!$this->db->query($sql))
		{
			echo "Error deleting $order_id from the order_line_items table.".PHP_EOL;
			return false;
		}

		//delete from orders table
		$sql= "DELETE FROM orders WHERE order_id='$order_id';";
		if(!$this->db->query($sql))
		{
			echo "Error deleting $order_id from the orders table.".PHP_EOL;
			return false;
		}	
		return true;
	}

	/*
	Get the order ids for orders not received yet, able to be deleted
	*/
	public function ADMIN_getDeletableOrders()
	{
		//Check if admin rights to update information in the inventory
		if(!$permission=$this->permissions(self::ADMIN, "delete an Order"))
			return $permission;

		$query= "SELECT order_id FROM order_line_items WHERE date_received IS NULL GROUP BY order_id;";
		$result= $this->db->query($query);

		$html= "<div class='form-group'><h5>Order ID</h5>
			<select class='form-control' name= 'order_id'>
				<option disabled selected value> Select an Order ID </option>";
		while($row=$result->fetch_assoc())
		{
			$order_id= $row['order_id'];
			$html .= "<option value= '$order_id'>$order_id</option>";
		}
		$html .= "</select></div>";
		echo $html;
	}

	/*
	Delete a vendor from the vendors table (and vendor_prices table)
	*/
	public function ADMIN_deleteVendor($vendor_id)
	{
		//Check if admin rights to delete a vendor
		if(!$permission=$this->permissions(self::ADMIN, "delete a Vendor"))
			return $permission;

		//orders table references vendors table, don't delete information if something references it
		$query="SELECT * FROM orders WHERE vendor_id=$vendor_id;";
		$this->db->query($query);
		if($this->db->affected_rows>0)
		{
			echo "Cannot delete vendor $vendor_id while there are orders for them.".PHP_EOL;
			return false;
		}


		//delete from vendor_prices first, because its a child table of vendors
		$sql= "DELETE FROM vendor_prices WHERE vendor_id=$vendor_id;";
		if(!$this->db->query($sql))
		{
			echo "Error deleting $vendor_id from the vendor_prices table.".PHP_EOL;
			return false;
		}

		//delete from vendors table
		$sql= "DELETE FROM vendors WHERE vendor_id=$vendor_id;";
		if(!$this->db->query($sql))
		{
			echo "Error deleting $vendor_id from the vendors table.".PHP_EOL;
			return false;
		}	
		return true;
	}

		/*
	Displays a report of the users and their md5 passwords
	*/
	public function ADMIN_reportUsers()
	{
		//Check if user has permission to see this report
		if(!$permission=$this->permissions(self::ADMIN, "run a user report"))
			return $permission;

		$query="SELECT user_name, password FROM users;";
		$result=$this->db->query($query);

		$html= "<div class='container'><div class='row'><div class='col-md-12'>
				<table class='table table-bordered table-striped table-condensed table-responsive' style='margin: auto;'>
					<thead>
						<tr>
							<th>User Name</th>
							<th>MD5 Password</th>
						</tr>
					</thead>
					<tbody>";
		while ($row = $result->fetch_row())
		{
			
			$html .= "<tr>";
			for($i=0; $i<sizeof($row); $i++)
			{
				$html .= "<td> $row[$i] </td>";
			}
			$html .= "</tr>";
		}
		$html .= "	</tbody>
				</table>
				</div></div></div>";
		echo $html;

	}



}

?>