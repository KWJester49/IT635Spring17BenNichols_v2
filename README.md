# IT635Spring17BenNichols_v2

In order to get .csv files to load, I had to do the following:

1. Open Terminal
2. Edit mysqld.cnf via the following command:
sudo subl /etc/mysql/mysql.conf.d/mysqld.cnf
3. Add the below to the bottom of mysqld.cnf:
secure_file_priv=""
4. Save and exit.
