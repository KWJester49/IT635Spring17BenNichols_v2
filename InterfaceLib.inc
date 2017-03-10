<?php

class InventoryAccess
{
	
	private $db;
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
		$db_password = $this->db->real_escape_string($password);

		$query = "select * from users where user_name = '$db_username';";
		echo $query.PHP_EOL;
		$result = $this->db->query($query);
		while ($row = $result->fetch_assoc())
		{
			if ($row["password"] == $db_password)
			{
				$this->permission = $row["permissions"];
				echo $this->permission.PHP_EOL;
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
			echo "Do not have authorization to $task_error_msg.".PHP_EOL;
			return false;
		}
	}

	/*
	Add an item to the database.
	Supply technicians (USER) should be able to add items smartly (be trained),
	rather than only being an ADMIN role.
	*/
	public function addItem($item_id,$item_name,$item_desc)
	{
		//Check if user has permission to add an item
		if(!$permission=$this->permissions(self::USER, "add an item"))
			return $permission;
		
		$query="INSERT INTO items (item_id, item_name, item_desc) VALUES ('$item_id', '$item_name', '$item_desc');";
		if(!$this->db->query($query))
		{
			echo "failed to add $item_id".PHP_EOL;
		}
	}

	/*
	Takes .csv file output from ordering system and uploads into the database.
	Will insert into the orders and order_line_items tables (and into the inventory table if it does not exist there).
	Will mark items as on order in the inventory table.
	*/
	public function uploadOrder($filename)
	{
		//Make sure user has permission to upload an order
		if(!$permission=$this->permissions(self::USER, "upload an order"))
			return $permission;
		echo "Can execute".PHP_EOL;

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
	Runs a report of items on inventory
	*/
	public function reportInventory()
	{
		//Make sure user has permission to run inventory report. Everyone as of now.
		if(!$permission=$this->permissions(self::GUEST, "run an Inventory Report"))
			return $permission;
		
		$query= "SELECT inventory.item_id, items.item_name, items.item_desc, inventory.quantity, inventory.location, inventory.par_level, inventory.on_order
			FROM inventory, items WHERE inventory.item_id=items.item_id;";
		$result = $this->db->query($query);
		echo "National Stock Number  |  Item Name  |  Item Description  |  Quantity  |  Location  |  Par Level  |  On Order".PHP_EOL;
		while ($row = $result->fetch_assoc())
		{
			$on_order=$row["on_order"];
			if($on_order==1)
			{
				$on_order="Yes";
			}
			else
			{
				$on_order="No";
			}
			echo $row["item_id"]." | ".$row["item_name"]." | ".$row["item_desc"]." | ".$row["quantity"]." | ".$row["location"]." | ".$row["par_level"]." | ".$on_order.PHP_EOL;
		}
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
		$counter=1;
		while ($row = $result->fetch_assoc())
		{
			echo "There is row $counter".PHP_EOL;
			$counter++;
		}
	}

	/*
	Runs a report of items at or below their par level (low water mark)
	*/
	public function reportItemsBelowParLevel()
	{
		//Reports of items at or below par level available to everyone as of now
		if(!$permission=$this->permissions(self::GUEST, "run a Par Level Report"))
			return $permission;
		
		
		$query="SELECT * FROM inventory, items WHERE inventory.item_id=items.item_id AND inventory.quantity<=inventory.par_level;";
		$result = $this->db->query($query);
		while ($row = $result->fetch_row())
		{
			
			for($i=0; $i<sizeof($row); $i++)
			{
				echo $row[$i].PHP_EOL;
			}
			echo PHP_EOL.PHP_EOL;
		}
	}


}


?>