# IT635Spring17BenNichols_v2

Final Project


Master-Slave Replication:

Master IP Address: 10.0.2.15
Slave IP Address:

Master: 
1. Make the following changes in /etc/mysql/mysql.conf.d/mysqld.cnf:

1.1. bind-address = 10.0.2.15
1.2. server-id = 1
1.3. log_bin = /var/log/mysql/mysql-bin.log
1.4. binlog_do_db = MainORDB

2. Refresh MySQL

3. Open MySQL and do the following:

3.1 GRANT REPLICATION SLAVE ON *.* TO 'slave_user'@'%' IDENTIFIED BY '12345';
3.2 FLUSH PRIVILEGES;

4. make a backup file of database.

In order to get .csv files to load, I had to do the following:

1. Open Terminal
2. Edit mysqld.cnf via the following command:
sudo subl /etc/mysql/mysql.conf.d/mysqld.cnf
3. Add the below to the bottom of mysqld.cnf:
secure_file_priv=""
4. Save and exit.


In order to get php to run locally, I had to do the following:

1. Install apache2 and set root directory to my project.
2. Edit '/etc/apache2/apache2.conf' to include the following:
	1. Change '<Directory />' option to:
		<Directory />
			Options Indexes FollowSymLinks Includes ExecCGI
			AllowOverride ALL
			Require all granted
		</Directory>
	2. Add the following:
		<FilesMatch \.php$>
			SetHandler application/x-httpd-php
		</FilesMatch>

		<FilesMatch "\.html$">
			ForceType application/x-httpd-php
		</FilesMatch>
3. Restart apache with: sudo service apache2 restart

