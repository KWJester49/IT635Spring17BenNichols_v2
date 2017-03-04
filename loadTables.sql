LOAD DATA LOCAL INFILE 'csvFiles/items.csv'
INTO TABLE items 
	FIELDS TERMINATED BY ','
		OPTIONALLY ENCLOSED BY '"'
	LINES TERMINATED BY '\n'
(item_id, item_name, item_desc);
