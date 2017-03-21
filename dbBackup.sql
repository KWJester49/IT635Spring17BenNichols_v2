-- MySQL dump 10.13  Distrib 5.7.17, for Linux (x86_64)
--
-- Host: localhost    Database: MainORDB
-- ------------------------------------------------------
-- Server version	5.7.17-0ubuntu0.16.04.1

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Current Database: `MainORDB`
--

CREATE DATABASE /*!32312 IF NOT EXISTS*/ `MainORDB` /*!40100 DEFAULT CHARACTER SET latin1 */;

USE `MainORDB`;

--
-- Table structure for table `historical_usage`
--

DROP TABLE IF EXISTS `historical_usage`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `historical_usage` (
  `date_used` datetime NOT NULL,
  `item_id` bigint(13) NOT NULL,
  `quantity` int(4) NOT NULL,
  PRIMARY KEY (`date_used`,`item_id`),
  KEY `item_id` (`item_id`),
  CONSTRAINT `historical_usage_ibfk_1` FOREIGN KEY (`item_id`) REFERENCES `items` (`item_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `historical_usage`
--

LOCK TABLES `historical_usage` WRITE;
/*!40000 ALTER TABLE `historical_usage` DISABLE KEYS */;
INSERT INTO `historical_usage` VALUES ('2017-02-01 00:00:01',6515000002157,9),('2017-02-01 00:00:01',6515000002264,1),('2017-02-01 00:00:01',6515000006430,1),('2017-02-02 00:00:01',6515000002157,10),('2017-02-02 00:00:01',6515000002264,1),('2017-02-02 00:00:01',6515000006430,1),('2017-02-03 00:00:01',6515000002157,11),('2017-02-03 00:00:01',6515000006430,1),('2017-02-04 00:00:01',6515000002157,10),('2017-02-04 00:00:01',6515000002264,1),('2017-02-04 00:00:01',6515000006430,1),('2017-02-05 00:00:01',6515000002157,10),('2017-02-05 00:00:01',6515000002264,1),('2017-02-05 00:00:01',6515000006430,1),('2017-02-06 00:00:01',6515000002157,8),('2017-02-06 00:00:01',6515000006430,1),('2017-02-07 00:00:01',6515000002157,10),('2017-02-07 00:00:01',6515000002264,1),('2017-02-07 00:00:01',6515000006430,1),('2017-02-08 00:00:01',6515000002157,10),('2017-02-08 00:00:01',6515000002264,1),('2017-02-08 00:00:01',6515000006430,1),('2017-02-09 00:00:01',6515000002157,12),('2017-02-09 00:00:01',6515000006430,1),('2017-02-10 00:00:01',6515000002157,10),('2017-02-10 00:00:01',6515000002264,1),('2017-02-10 00:00:01',6515000006430,1),('2017-02-11 00:00:01',6515000002157,10),('2017-02-11 00:00:01',6515000002264,1),('2017-02-11 00:00:01',6515000006430,1),('2017-02-12 00:00:01',6515000002157,10),('2017-02-12 00:00:01',6515000006430,1),('2017-02-13 00:00:01',6515000002157,10),('2017-02-13 00:00:01',6515000002264,1),('2017-02-13 00:00:01',6515000006430,1),('2017-02-14 00:00:01',6515000002157,7),('2017-02-14 00:00:01',6515000002264,1),('2017-02-14 00:00:01',6515000006430,1),('2017-02-15 00:00:01',6515000002157,10),('2017-02-15 00:00:01',6515000006430,1),('2017-02-16 00:00:01',6515000002157,10),('2017-02-16 00:00:01',6515000002264,1),('2017-02-16 00:00:01',6515000006430,1),('2017-02-17 00:00:01',6515000002157,13),('2017-02-17 00:00:01',6515000002264,1),('2017-02-17 00:00:01',6515000006430,1),('2017-02-18 00:00:01',6515000002157,10),('2017-02-18 00:00:01',6515000006430,1),('2017-02-19 00:00:01',6515000002157,10),('2017-02-19 00:00:01',6515000002264,1),('2017-02-19 00:00:01',6515000006430,1),('2017-02-20 00:00:01',6515000002157,10),('2017-02-20 00:00:01',6515000002264,1),('2017-02-20 00:00:01',6515000006430,1),('2017-02-21 00:00:01',6515000002157,11),('2017-02-21 00:00:01',6515000006430,1),('2017-02-22 00:00:01',6515000002157,10),('2017-02-22 00:00:01',6515000002264,1),('2017-02-22 00:00:01',6515000006430,1),('2017-02-23 00:00:01',6515000002157,10),('2017-02-23 00:00:01',6515000002264,1),('2017-02-23 00:00:01',6515000006430,1),('2017-02-24 00:00:01',6515000002157,9),('2017-02-24 00:00:01',6515000006430,1),('2017-02-25 00:00:01',6515000002157,10),('2017-02-25 00:00:01',6515000002264,1),('2017-02-25 00:00:01',6515000006430,1),('2017-02-26 00:00:01',6515000002157,10),('2017-02-26 00:00:01',6515000002264,1),('2017-02-26 00:00:01',6515000006430,1),('2017-02-27 00:00:01',6515000002157,14),('2017-02-27 00:00:01',6515000006430,1),('2017-02-28 00:00:01',6515000002157,6),('2017-02-28 00:00:01',6515000002264,1),('2017-02-28 00:00:01',6515000006430,1),('2017-03-01 00:00:01',6515000002157,10);
/*!40000 ALTER TABLE `historical_usage` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `inventory`
--

DROP TABLE IF EXISTS `inventory`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `inventory` (
  `item_id` bigint(13) NOT NULL,
  `quantity` int(4) NOT NULL,
  `location` varchar(255) DEFAULT NULL,
  `par_level` int(4) DEFAULT NULL,
  `on_order` tinyint(1) DEFAULT '0',
  UNIQUE KEY `item_id` (`item_id`),
  CONSTRAINT `inventory_ibfk_1` FOREIGN KEY (`item_id`) REFERENCES `items` (`item_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `inventory`
--

LOCK TABLES `inventory` WRITE;
/*!40000 ALTER TABLE `inventory` DISABLE KEYS */;
INSERT INTO `inventory` VALUES (6515000002157,72,'Shelf 7 Row 3',60,1),(6515000002263,8,'Shelf 2 Row 1',5,0),(6515000002264,15,'Shelf 2 Row 2',20,1),(6515000002265,11,'Shelf 2 Row 2',7,0),(6515000002271,7,'Shelf 2 Row 1',5,0),(6515000006430,20,'Shelf 7 Row 2',30,0);
/*!40000 ALTER TABLE `inventory` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `items`
--

DROP TABLE IF EXISTS `items`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `items` (
  `item_id` bigint(13) NOT NULL,
  `item_name` varchar(255) NOT NULL,
  `item_desc` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`item_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `items`
--

LOCK TABLES `items` WRITE;
/*!40000 ALTER TABLE `items` DISABLE KEYS */;
INSERT INTO `items` VALUES (6515000002157,'ADMINISTRATION SET FOR IV FLUIDS','A universal administration set for IV fluids or album in 67 inches (170 cm) with 15 micron filter, roller clamp, with disc filter, male luer slip and vein needle (19 g x 1-Â½ inch in blister pack) approximate drip rate 20 drops/ml'),(6515000002263,'AEROCHAMBER','Pediatric with mask, small'),(6515000002264,'AEROCHAMBER','Adult with mask'),(6515000002265,'AEROCHAMBER','Adult without mask'),(6515000002271,'AEROCHAMBER','Pediatric without mask, small'),(6515000006430,'ADMINISTRATION SET FOR IV FLUIDS','Nonvented IV set with 10, 15, or 60 drops per cc:selectable drop chambers. Sterile and non pyrogenic if package integrity is maintained.');
/*!40000 ALTER TABLE `items` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `order_line_items`
--

DROP TABLE IF EXISTS `order_line_items`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `order_line_items` (
  `order_id` varchar(14) NOT NULL,
  `item_id` bigint(13) NOT NULL,
  `quantity` int(4) NOT NULL,
  `unit_price` decimal(11,4) DEFAULT NULL,
  `date_received` date DEFAULT NULL,
  PRIMARY KEY (`order_id`,`item_id`),
  KEY `item_id` (`item_id`),
  CONSTRAINT `order_line_items_ibfk_1` FOREIGN KEY (`order_id`) REFERENCES `orders` (`order_id`),
  CONSTRAINT `order_line_items_ibfk_2` FOREIGN KEY (`item_id`) REFERENCES `items` (`item_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `order_line_items`
--

LOCK TABLES `order_line_items` WRITE;
/*!40000 ALTER TABLE `order_line_items` DISABLE KEYS */;
INSERT INTO `order_line_items` VALUES ('N0018370321456',6515000002157,100,1.3200,'2017-02-03'),('N0018370321456',6515000002263,5,45.2100,'2017-02-04'),('N0018370321456',6515000002271,5,69.9700,'2017-02-04'),('N0018370321457',6515000002264,10,38.9200,'2017-02-07'),('N0018370321457',6515000002265,10,30.0000,'2017-02-07'),('N0018370411458',6515000002157,80,1.3200,'2017-02-12'),('N0018370411458',6515000006430,50,4.1400,'2017-02-13'),('N0018370521459',6515000002157,90,1.3200,'2017-02-22'),('N0018370521459',6515000002264,12,43.2400,'2017-02-27'),('N0018370521459',6515000002265,8,33.3000,'2017-02-24'),('N0018370560460',6515000002157,50,1.3200,NULL),('N0018370560460',6515000002264,8,43.2400,NULL);
/*!40000 ALTER TABLE `order_line_items` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `orders`
--

DROP TABLE IF EXISTS `orders`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `orders` (
  `order_id` varchar(14) NOT NULL,
  `vendor_id` int(4) NOT NULL,
  `date_ordered` date NOT NULL,
  PRIMARY KEY (`order_id`),
  KEY `vendor_id` (`vendor_id`),
  CONSTRAINT `orders_ibfk_1` FOREIGN KEY (`vendor_id`) REFERENCES `vendors` (`vendor_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `orders`
--

LOCK TABLES `orders` WRITE;
/*!40000 ALTER TABLE `orders` DISABLE KEYS */;
INSERT INTO `orders` VALUES ('N0018370321456',1,'2017-02-01'),('N0018370321457',2,'2017-02-01'),('N0018370411458',1,'2017-02-10'),('N0018370521459',1,'2017-02-21'),('N0018370560460',1,'2017-03-01');
/*!40000 ALTER TABLE `orders` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `users`
--

DROP TABLE IF EXISTS `users`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `users` (
  `user_id` int(4) NOT NULL AUTO_INCREMENT,
  `user_name` varchar(10) NOT NULL,
  `password` varchar(20) DEFAULT NULL,
  `permissions` int(1) DEFAULT '0',
  PRIMARY KEY (`user_id`),
  UNIQUE KEY `user_name` (`user_name`)
) ENGINE=InnoDB AUTO_INCREMENT=8 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `users`
--

LOCK TABLES `users` WRITE;
/*!40000 ALTER TABLE `users` DISABLE KEYS */;
INSERT INTO `users` VALUES (1,'ADMIN','12345',5),(2,'BNICHOLS','12345',3),(3,'JJOHNSON','12345',3),(4,'Guest','',0);
/*!40000 ALTER TABLE `users` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `vendor_prices`
--

DROP TABLE IF EXISTS `vendor_prices`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `vendor_prices` (
  `item_id` bigint(13) NOT NULL,
  `vendor_id` int(4) NOT NULL,
  `price` decimal(11,4) DEFAULT NULL,
  PRIMARY KEY (`item_id`,`vendor_id`),
  KEY `vendor_id` (`vendor_id`),
  CONSTRAINT `vendor_prices_ibfk_1` FOREIGN KEY (`item_id`) REFERENCES `items` (`item_id`),
  CONSTRAINT `vendor_prices_ibfk_2` FOREIGN KEY (`vendor_id`) REFERENCES `vendors` (`vendor_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `vendor_prices`
--

LOCK TABLES `vendor_prices` WRITE;
/*!40000 ALTER TABLE `vendor_prices` DISABLE KEYS */;
INSERT INTO `vendor_prices` VALUES (6515000002157,1,1.3200),(6515000002157,2,1.4500),(6515000002263,1,45.2100),(6515000002264,1,43.2400),(6515000002264,2,38.9200),(6515000002265,1,33.3000),(6515000002265,2,30.0000),(6515000002271,1,69.9700),(6515000006430,1,4.1400),(6515000006430,2,4.5500);
/*!40000 ALTER TABLE `vendor_prices` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `vendors`
--

DROP TABLE IF EXISTS `vendors`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `vendors` (
  `vendor_id` int(4) NOT NULL AUTO_INCREMENT,
  `vendor_name` varchar(255) NOT NULL,
  `vendor_address` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`vendor_id`)
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `vendors`
--

LOCK TABLES `vendors` WRITE;
/*!40000 ALTER TABLE `vendors` DISABLE KEYS */;
INSERT INTO `vendors` VALUES (1,'Stryker','4100 East Milhelm Ave., Kalamazoo, MI 49001'),(2,'Cardinal Health','7000 Cardinal Place, Dublin, OH 43017');
/*!40000 ALTER TABLE `vendors` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2017-03-21 13:52:12
