Hi!

This is the memcached mysql UDFs (user defined functions) to work with libmemcached, Brian Aker's super duper
new fast client library for memcached.


Prerequisites

* MySQL 5.0 and greater
* Latest memcached (svn co http://code.sixapart.com/svn/memcached/trunk)
* libmemcached (hg clone http://hg.tangent.org/libmemcached/)
* latest autoconf tools

To build:
./configure --with-mysql=/usr/local/mysql/bin/mysql_config --libdir=/usr/local/mysql/lib/mysql/
make
make install


To then load the functions that you need:

Please keep in mind that for your UDF to be loaded, it must be in the
library path for your server (and yes, we should fix this). On Linux you can
set this by exporting the correct path in LD_LIBRARY_PATH for your mysql
server.

You can install the functions as listed below by cutting and pasting,or 
optionally there now are two ways to install these easily:

* sql/install_functions.sql - this loads the functions with simple CREATE FUNCTION
commands as shown below

* utils/install.pl - This is a perl script that queries your mysql.func table to see what 
functions are already installed and installs them if not yet installed. It will ask you 
each time for every function not installed, if you want to install, or you can run it with 
-s/--silent to have it not prompt you. It runs as the 'root' database user (required).

memc_servers_add()

CREATE FUNCTION memc_servers_add RETURNS INT SONAME "libmemcached_functions_mysql.so";

memc_servers_add_by_key()

CREATE FUNCTION memc_servers_add_by_key RETURNS INT SONAME "libmemcached_functions_mysql.so";

memc_servers_set()

CREATE FUNCTION memc_servers_set RETURNS INT SONAME "libmemcached_functions_mysql.so";

memc_set()

CREATE FUNCTION memc_set RETURNS INT SONAME "libmemcached_functions_mysql.so";

memc_set_by_key()

CREATE FUNCTION memc_set_by_key RETURNS INT SONAME "libmemcached_functions_mysql.so";

memc_cas()

CREATE FUNCTION memc_cas RETURNS INT SONAME "libmemcached_functions_mysql.so";

memc_cas_by_key()

CREATE FUNCTION memc_cas_by_key RETURNS INT SONAME "libmemcached_functions_mysql.so";

memc_get()

CREATE FUNCTION memc_get RETURNS STRING SONAME "libmemcached_functions_mysql.so";

memc_get_by_key()

CREATE FUNCTION memc_get RETURNS STRING SONAME "libmemcached_functions_mysql.so";

memc_delete()

CREATE FUNCTION memc_delete RETURNS INT SONAME "libmemcached_functions_mysql.so";

memc_delete_by_key()

CREATE FUNCTION memc_delete_by_key RETURNS INT SONAME "libmemcached_functions_mysql.so";

memc_append()

CREATE FUNCTION memc_append RETURNS INT SONAME "libmemcached_functions_mysql.so";

memc_append_by_key()

CREATE FUNCTION memc_append_by_key RETURNS INT SONAME "libmemcached_functions_mysql.so";

memc_prepend()

CREATE FUNCTION memc_prepend RETURNS INT SONAME "libmemcached_functions_mysql.so";

memc_prepend_by_key()

CREATE FUNCTION memc_prepend_by_key RETURNS INT SONAME "libmemcached_functions_mysql.so";

memc_increment()

CREATE FUNCTION memc_increment RETURNS INT SONAME "libmemcached_functions_mysql.so";

memc_decrement()

CREATE FUNCTION memc_decrement RETURNS INT SONAME "libmemcached_functions_mysql.so";

memc_replace()

CREATE FUNCTION memc_replace RETURNS INT SONAME "libmemcached_functions_mysql.so";

memc_replace_by_key()

CREATE FUNCTION memc_replace_by_key RETURNS INT SONAME "libmemcached_functions_mysql.so";

memc_servers_behavior_set()

CREATE FUNCTION memc_servers_behavior_set RETURNS INT SONAME "libmemcached_functions_mysql.so";

memc_list_behaviors()

CREATE FUNCTION memc_list_behaviors RETURNS STRING SONAME "libmemcached_functions_mysql.so";

memc_stats()

CREATE FUNCTION memc_stats RETURNS STRING SONAME "libmemcached_functions_mysql.so";

memc_stats_get_keys()

CREATE FUNCTION memc_stats_get_keys RETURNS STRING SONAME "libmemcached_functions_mysql.so";

memc_stats_get_value()

CREATE FUNCTION memc_stats_get_value RETURNS STRING SONAME "libmemcached_functions_mysql.so";


Usage of functions

memc_servers_set()

I want to set the server I'm using to localhost:

select memc_servers_set('127.0.0.1');

I have several servers, mem1.grazr.com, mem2.grazr.com:

select memc_servers_set('mem1.grazr.com,mem2.grazr.com');


memc_set()

mysql> select memc_set('abc', 'cool new memcached udf in mysql');
+----------------------------------------------------+
| memc_set('abc', 'cool new memcached udf in mysql') |
+----------------------------------------------------+
|                                                  0 | 
+----------------------------------------------------+
1 row in set (0.00 sec)


memc_get()

mysql> select memc_get('abc');
+---------------------------------+
| memc_get('abc')                 |
+---------------------------------+
| cool new memcached udf in mysql | 
+---------------------------------+
1 row in set (0.00 sec)


memc_delete()

mysql> select memc_delete('abc');
+--------------------+
| memc_delete('abc') |
+--------------------+
|                  0 | 
+--------------------+
1 row in set (0.00 sec)

mysql> select memc_get('abc');
+-----------------+
| memc_get('abc') |
+-----------------+
| NULL            | 
+-----------------+
1 row in set (0.00 sec)


memc_append() and memc_prepend()

mysql> select memc_set('abc', ' Spot ');
+---------------------------+
| memc_set('abc', ' Spot ') |
+---------------------------+
|                         0 | 
+---------------------------+
1 row in set (0.00 sec)

mysql> select memc_append('abc', ' run.');
+-----------------------------+
| memc_append('abc', ' run.') |
+-----------------------------+
|                           0 | 
+-----------------------------+
1 row in set (0.00 sec)

mysql> select memc_get('abc');    
+-----------------+
| memc_get('abc') |
+-----------------+
|  Spot  run.     | 
+-----------------+
1 row in set (0.00 sec)

mysql> select memc_prepend('abc', 'See');
+----------------------------+
| memc_prepend('abc', 'See') |
+----------------------------+
|                          0 | 
+----------------------------+
1 row in set (0.00 sec)

mysql> select memc_get('abc');
+-----------------+
| memc_get('abc') |
+-----------------+
| See Spot  run.  | 
+-----------------+
1 row in set (0.00 sec)


memc_increment() and memc_decrement()

mysql> select memc_set('sequence:1', 1);
+---------------------------+
| memc_set('sequence:1', 1) |
+---------------------------+
|                         0 | 
+---------------------------+
1 row in set (0.00 sec)

mysql> select memc_get('sequence:1');
+------------------------+
| memc_get('sequence:1') |
+------------------------+
| 1                      | 
+------------------------+
1 row in set (0.00 sec)

mysql> select memc_increment('sequence:1', 5);
+---------------------------------+
| memc_increment('sequence:1', 5) |
+---------------------------------+
|                               6 | 
+---------------------------------+
1 row in set (0.00 sec)

mysql> select memc_increment('sequence:1', 10);
]+----------------------------------+
| memc_increment('sequence:1', 10) |
+----------------------------------+
|                               16 | 
+----------------------------------+
1 row in set (0.01 sec)

mysql> select memc_increment('sequence:1');
+------------------------------+
| memc_increment('sequence:1') |
+------------------------------+
|                           17 | 
+------------------------------+
1 row in set (0.00 sec)

mysql> select memc_decrement('sequence:1');
+------------------------------+
| memc_decrement('sequence:1') |
+------------------------------+
|                           16 | 
+------------------------------+
1 row in set (0.00 sec)

mysql> select memc_decrement('sequence:1', 15);
+----------------------------------+
| memc_decrement('sequence:1', 15) |
+----------------------------------+
|                                1 | 
+----------------------------------+
1 row in set (0.00 sec)


memc_replace()

mysql> select memc_replace('sequence:1', 'String value');
+--------------------------------------------+
| memc_replace('sequence:1', 'String value') |
+--------------------------------------------+
|                                          0 | 
+--------------------------------------------+
1 row in set (0.00 sec)

mysql> select memc_get('sequence:1');
+------------------------+
| memc_get('sequence:1') |
+------------------------+
| String value           | 
+------------------------+
1 row in set (0.01 sec)

memc_list_behaviors()

Use this to see what behaviors you have to chose from


memc_servers_behavior_set()

You use this to set the type of behavior your client has to the memcached server

mysql> select memc_list_behaviors()\G
*************************** 1. row ***************************
memc_list_behaviors(): 
+-------------------------------------+
| MEMCACHED SERVER BEHAVIORS          |
+-------------------------------------+
| MEMCACHED_BEHAVIOR_SUPPORT_CAS      |
| MEMCACHED_BEHAVIOR_NO_BLOCK         |
| MEMCACHED_BEHAVIOR_TCP_NODELAY      |
| MEMCACHED_BEHAVIOR_HASH             |
| MEMCACHED_BEHAVIOR_CACHE_LOOKUPS    |
| MEMCACHED_BEHAVIOR_SOCKET_SEND_SIZE |
| MEMCACHED_BEHAVIOR_SOCKET_RECV_SIZE |
+-------------------------------------+


mysql> select memc_servers_behavior_set('MEMCACHED_BEHAVIOR_TCP_NODELAY','1');
+-----------------------------------------------------------------+
| memc_servers_behavior_set('MEMCACHED_BEHAVIOR_TCP_NODELAY','1') |
+-----------------------------------------------------------------+
|                                                               0 | 
+-----------------------------------------------------------------+
1 row in set (0.01 sec)

Have fun!

Cheers,
  Brian, 
    Seattle, WA 
  Patrick, 
    Sharon, NH (Live Free or Die!).
