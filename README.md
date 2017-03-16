# IT635Spring17BenNichols_v2

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