/*
Creates the items table.
This will hold information about the items that the main operating room can have or has had in its inventory.
*/
DROP TABLE IF EXISTS 'items';

CREATE TABLE 'items'(
	'item_id' int(13) NOT NULL, 
	/* 
	The primary key for the items table.
	Identification number of the item, unique for each item regardless of manufacturer.  
	Navy uses a 13 digit number called an NSN (National Stock Number), so I reused that here. 
	NSN is not determined locally, but assigned to items by NATO.
	Items will have the same NSN regardless of who manufactured it. 
	I would have called it NSN, but want uniformity across my database.  
	I can always have it display as NSN through the user interface. 
	*/
	'item_name' varchar(255) NOT NULL, --Name of the item
	'item_desc' varchar(255) DEFAULT NULL, --Description of the item.
	PRIMARY KEY ('item_id')
);

/*
Creates inventory table.  
Connects to 'items' table using 'item_id' as foreign key.
Used to track what is currently on stock in the store room inventory.
*/
DROP TABLE IF EXISTS 'inventory';

CREATE TABLE 'inventory' (
	'item_id' int(13) NOT NULL UNIQUE, 
	/* 
	Foreign key from 'items' table.  
	Multiples of the same item just increases the quantity, so it it unique.
	*/
	'quantity' int(4) NOT NULL, --Quantity on hand of the item.
	'location' varchar(255) DEFAULT NULL, --Where the item is located in the store room.
	'par_level' int(4) DEFAULT NULL, --The reorder point for the item.  When quantity is below this, reorder.
	'on_order' bit DEFAULT NULL, --If the item is on order or not, equivalent of boolean.
	FOREIGN KEY ('item_id') REFERENCES items('item_id')
);

/*
Creates the historical_usage table.
Stores when all items stored in the 'inventory' table are used.
*/
DROP TABLE IF EXISTS 'historical_usage';

CREATE TABLE 'historical_usage' (
	'datetime_used' datetime NOT NULL, --When an item from inventory is used.
	'item_id' int(13) NOT NULL, --The item from the inventory used.
	'quantity' int(4) NOT NULL, --The number of the item used.
	PRIMARY KEY ('date', 'item_id'), --Composite key of the item used and when it was used.
	FOREIGN KEY ('item_id') REFERENCES items('item_id')
);

/*
Creates the vendors table.
The different companies from which the medical supplies in the storeroom are purchased.
*/
DROP TABLE IF EXISTS 'vendors';

CREATE TABLE 'vendors' (
	'vendor_id' int(4) NOT NULL AUTO_INCREMENT, 
	/*
	Primary key for the vendors table.
	Internal identification number for vendor, just auto increment for each new vendor.
	Probably not possible to even break 100 vendors with the restrictions on government suppliers, but for safety can go up to 9999.
	*/
	'vendor_name' varchar(255) NOT NULL, --Name of the vendor.
	'vendor_address' varchar(255) DEFAULT NULL, --Address of where the vendor is located.
	PRIMARY KEY ('vendor_id')
);

/*
Creates the vendor_prices tables.
Containts pricing information for items sold by the vendor.
Connects to the 'inventory' table using the 'item_id' as a foreign key.
Connects to the 'vendors' table using the 'vendor_id' as a foreign key.
*/
DROP TABLE IF EXISTS 'vendor_prices';

CREATE TABLE 'vendor_prices' (
	'item_id' int(13) NOT NULL, 
	/*
	Foreign key from the items table.
	The item sold by the vendor.
	*/
	'vendor_id' int(4) NOT NULL, 
	/*
	Foreign key from the vendors table.
	The vendor selling the item.
	*/
	'price'  decimal(11,4) DEFAULT NULL, 
	/*
	The price of the item sold by the vendor. 
	I debated using the money datatype, which I think would have been fine.
	The only concern was the mathematical operations (multiplication/division) introducing errors.
	I do not believe would occur with this project as I saw it happening when people were doing money*money 
	or money/money, which doesn't make sense why you would even perform those operations in the first place.
	The smallmoney datatype might actually be too small for some of the medical supplies purchased (although probably not for a single item).
	Just to be safe and in part for space savings I went with decimal and made it slightly larger than smallmoney (can go up to $9,999,999.999).
	*/
	PRIMARY KEY ('item_id', 'vendor_id'), 
	/*
	Create a composite key of the item_id and vendor_id, making their combination unique.
	If I were actually implementing this database I would probably have to add some other fields or another table and keep a pricing history.
	*/
	FOREIGN KEY ('item_id') REFERENCES items('item_id'),
	FOREIGN KEY ('vendor_id') REFERENCES vendors('vendor_id')
);

/*
Creates the users table.
Right now only to be used for the personnel that can reorder supplies.
Used with 'orders' table to track who placed an order.
*/
DROP TABLE IF EXISTS 'users';

CREATE TABLE 'users' (
	'user_id' int(4) NOT NULL AUTO_INCREMENT, 
	/*
	Primary key of the users table.
	Internal identification number for user, just auto increment for each new user.
	*/
	'user_name' varchar(10) NOT NULL UNIQUE,
	/*
	The user's username, which I am requiring to be unique in the database.
	The format will be the first initial followed by up to the first 9 letters of the last name, all caps.
	Example: My user_name would be BNICHOLS.
	*/
	PRIMARY KEY ('user_id')
);

/*
Creates the orders table.
Connects to the 'vendors' table using 'vendor_id' as a foreign key.
Connects to the 'users' table using 'user_id' as a foreign key.
Keeps track of all the orders that have been placed, whether they have been received or not.
*/
DROP TABLE IF EXISTS 'orders';

CREATE TABLE 'orders' (
	'order_id' int(13) NOT NULL AUTO_INCREMENT,
	/*
	Primary key of the orders table.
	Internal identification number for order, just auto increment for each new order.
	*/
	'vendor_id' int(4) NOT NULL,
	/*
	Foreign key from the vendors table.
	Who the order is being purchased from.
	*/
	'user_id' int(4) DEFAULT NULL,
	/*
	Foreign key from the users table.
	Who placed the order.
	*/
	'date_ordered' date NOT NULL,
	/*
	The date that the order is placed on.
	*/
	PRIMARY KEY ('order_id'),
	FOREIGN KEY ('vendor_id') REFERENCES vendors('vendor_id'),
	FOREIGN KEY ('user_id') REFERENCES users('user_id')
);

/*
Creates order_line_items table.
Connects to the 'orders' table using 'order_id' as a foreign key.
Connects to the 'items' table using 'item_id' as a foreign key.
Stores information about each item that is in an order.
Table is necessary because it is quite common to order multiple things at once from the same vendor.
A line item is uniquely defined based off of the order_id and item_id.
*/
DROP TABLE IF EXISTS 'order_line_items';

CREATE TABLE 'order_line_items' (
	'order_id' int(13) NOT NULL,
	/*
	Foreign key from the orders table.
	The overall order of which this line item belongs to.
	*/
	'item_id' int(13) NOT NULL, 
	/*
	Foreign key from the items table.
	The item being ordered on this line item.
	*/
	'quantity' int(4) NOT NULL, --Quantity being ordered.
	'unit_price' decimal(11,4) DEFAULT NULL, 
	/*
	The price of the item when it was ordered.
	Not as good as keeping track of the vendors prices, but provides some historical price information.
	*/
	'date_received' date DEFAULT NULL,
	/*
	The date when the item is received.
	This implementation is assuming that specific line items of an order may not be delivered at the same time, 
	but that all of a single line item WILL be delivered at the same time.
	In real life this might not actually occur, partial delivery of a single line item may occur, but for the purposes of this project and simplicity I opted for this.
	*/
	PRIMARY KEY ('order_id', 'item_id'),
	/*
	Creates a composite key of order_id and item_id to uniquely identify each line item of an order.
	*/
	FOREIGN KEY ('order_id') REFERENCES orders('order_id'),
	FOREIGN KEY ('item_id') REFERENCES items('item_id'),
);