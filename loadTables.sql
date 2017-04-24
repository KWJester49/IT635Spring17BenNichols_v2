/*
Loads data into each of the tables of the database.
Starts with items, vendors, and users because they have primary keys and do not reference other tables.
orders is loaded next, because it has a primary key, then order_line_items is done after that.
	As I am starting right now, for convenience, I am having them load as separate files, but in actual
	implementation, when passed a .csv file from the ordering system, it would separate the information
	and load it into the two separate tables.
Following this, loads the tables in an arbitrary order.
*/
LOAD DATA LOCAL INFILE 'csvFiles/items.csv'
INTO TABLE items 
	FIELDS TERMINATED BY ','
		OPTIONALLY ENCLOSED BY '"'
	LINES TERMINATED BY '\n'
(item_id, item_name, item_desc);

LOAD DATA LOCAL INFILE 'csvFiles/vendors.csv'
INTO TABLE vendors 
	FIELDS TERMINATED BY ','
		OPTIONALLY ENCLOSED BY '"'
	LINES TERMINATED BY '\n'
(vendor_name, vendor_address);

LOAD DATA LOCAL INFILE 'csvFiles/vendor_prices.csv'
INTO TABLE vendor_prices 
	FIELDS TERMINATED BY ','
		OPTIONALLY ENCLOSED BY '"'
	LINES TERMINATED BY '\n'
(item_id, vendor_id, price);

LOAD DATA LOCAL INFILE 'csvFiles/users.csv'
INTO TABLE users 
	FIELDS TERMINATED BY ','
		OPTIONALLY ENCLOSED BY '"'
	LINES TERMINATED BY '\n'
(user_name, password, permissions);

UPDATE users
SET password= MD5(password);


LOAD DATA LOCAL INFILE 'csvFiles/orders.csv'
INTO TABLE orders 
	FIELDS TERMINATED BY ','
		OPTIONALLY ENCLOSED BY '"'
	LINES TERMINATED BY '\n'
(order_id, vendor_id, date_ordered);

LOAD DATA LOCAL INFILE 'csvFiles/order_line_items.csv'
INTO TABLE order_line_items 
	FIELDS TERMINATED BY ','
		OPTIONALLY ENCLOSED BY '"'
	LINES TERMINATED BY '\n'
(order_id, item_id, quantity, unit_price, date_received);

LOAD DATA LOCAL INFILE 'csvFiles/inventory.csv'
INTO TABLE inventory 
	FIELDS TERMINATED BY ','
		OPTIONALLY ENCLOSED BY '"'
	LINES TERMINATED BY '\n'
(item_id, quantity, location, par_level, on_order);

LOAD DATA LOCAL INFILE 'csvFiles/historical_usage.csv'
INTO TABLE historical_usage 
	FIELDS TERMINATED BY ','
		OPTIONALLY ENCLOSED BY '"'
	LINES TERMINATED BY '\n'
(date_used, item_id, quantity);