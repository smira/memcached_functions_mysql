

DROP FUNCTION IF EXISTS memc_udf_version;
DROP FUNCTION IF EXISTS memc_servers_set;
DROP FUNCTION IF EXISTS memc_add;
DROP FUNCTION IF EXISTS memc_add_by_key;
DROP FUNCTION IF EXISTS memc_cas;
DROP FUNCTION IF EXISTS memc_cas_by_key;
DROP FUNCTION IF EXISTS memc_set;
DROP FUNCTION IF EXISTS memc_set_by_key;
DROP FUNCTION IF EXISTS memc_get;
DROP FUNCTION IF EXISTS memc_get_by_key;
DROP FUNCTION IF EXISTS memc_delete;
DROP FUNCTION IF EXISTS memc_delete_by_key;
DROP FUNCTION IF EXISTS memc_append;
DROP FUNCTION IF EXISTS memc_append_by_key;
DROP FUNCTION IF EXISTS memc_prepend;
DROP FUNCTION IF EXISTS memc_prepend_by_key;
DROP FUNCTION IF EXISTS memc_increment;
DROP FUNCTION IF EXISTS memc_decrement;
DROP FUNCTION IF EXISTS memc_replace;
DROP FUNCTION IF EXISTS memc_replace_by_key;
DROP FUNCTION IF EXISTS memc_behavior_set;
DROP FUNCTION IF EXISTS memc_servers_behavior_set;
DROP FUNCTION IF EXISTS memc_behavior_get;
DROP FUNCTION IF EXISTS memc_servers_behavior_get;
DROP FUNCTION IF EXISTS memc_list_behaviors;
DROP FUNCTION IF EXISTS memc_list_hash_types;
DROP FUNCTION IF EXISTS memc_list_distribution_types;
DROP FUNCTION IF EXISTS memc_add;
DROP FUNCTION IF EXISTS memc_add_by_key;
DROP FUNCTION IF EXISTS memc_stats;
DROP FUNCTION IF EXISTS memc_stat_get_keys;
DROP FUNCTION IF EXISTS memc_stat_get_value;
DROP FUNCTION IF EXISTS memc_server_count;
DROP FUNCTION IF EXISTS memc_libmemcached_version;


CREATE FUNCTION memc_add RETURNS INT SONAME "libmemcached_functions_mysql.so";
CREATE FUNCTION memc_add_by_key RETURNS INT SONAME "libmemcached_functions_mysql.so";
CREATE FUNCTION memc_servers_set RETURNS INT SONAME "libmemcached_functions_mysql.so";
CREATE FUNCTION memc_server_count RETURNS INT SONAME "libmemcached_functions_mysql.so";
CREATE FUNCTION memc_set RETURNS INT SONAME "libmemcached_functions_mysql.so";
CREATE FUNCTION memc_set_by_key RETURNS INT SONAME "libmemcached_functions_mysql.so";
CREATE FUNCTION memc_cas RETURNS INT SONAME "libmemcached_functions_mysql.so";
CREATE FUNCTION memc_cas_by_key RETURNS INT SONAME "libmemcached_functions_mysql.so";
CREATE FUNCTION memc_get RETURNS STRING SONAME "libmemcached_functions_mysql.so";
CREATE FUNCTION memc_get_by_key RETURNS STRING SONAME "libmemcached_functions_mysql.so";
CREATE FUNCTION memc_delete RETURNS INT SONAME "libmemcached_functions_mysql.so";
CREATE FUNCTION memc_delete_by_key RETURNS INT SONAME "libmemcached_functions_mysql.so";
CREATE FUNCTION memc_append RETURNS INT SONAME "libmemcached_functions_mysql.so";
CREATE FUNCTION memc_append_by_key RETURNS INT SONAME "libmemcached_functions_mysql.so";
CREATE FUNCTION memc_prepend RETURNS INT SONAME "libmemcached_functions_mysql.so";
CREATE FUNCTION memc_prepend_by_key RETURNS INT SONAME "libmemcached_functions_mysql.so";
CREATE FUNCTION memc_increment RETURNS INT SONAME "libmemcached_functions_mysql.so";
CREATE FUNCTION memc_decrement RETURNS INT SONAME "libmemcached_functions_mysql.so";
CREATE FUNCTION memc_replace RETURNS INT SONAME "libmemcached_functions_mysql.so";
CREATE FUNCTION memc_replace_by_key RETURNS INT SONAME "libmemcached_functions_mysql.so";

CREATE FUNCTION memc_servers_behavior_set RETURNS INT SONAME "libmemcached_functions_mysql.so";
CREATE FUNCTION memc_servers_behavior_get RETURNS STRING SONAME "libmemcached_functions_mysql.so";
CREATE FUNCTION memc_behavior_set RETURNS INT SONAME "libmemcached_functions_mysql.so";
CREATE FUNCTION memc_behavior_get RETURNS STRING SONAME "libmemcached_functions_mysql.so";
CREATE FUNCTION memc_list_behaviors RETURNS STRING SONAME "libmemcached_functions_mysql.so";
CREATE FUNCTION memc_list_hash_types RETURNS STRING SONAME "libmemcached_functions_mysql.so";
CREATE FUNCTION memc_list_distribution_types RETURNS STRING SONAME "libmemcached_functions_mysql.so";

CREATE FUNCTION memc_udf_version RETURNS STRING SONAME "libmemcached_functions_mysql.so";
CREATE FUNCTION memc_libmemcached_version RETURNS STRING SONAME "libmemcached_functions_mysql.so";
CREATE FUNCTION memc_stats RETURNS STRING SONAME "libmemcached_functions_mysql.so";
CREATE FUNCTION memc_stat_get_keys RETURNS STRING SONAME "libmemcached_functions_mysql.so";
CREATE FUNCTION memc_stat_get_value RETURNS STRING SONAME "libmemcached_functions_mysql.so";


DROP TABLE IF EXISTS `t1`;
select memc_udf_version();
select memc_servers_set('127.0.0.1:11211');
select memc_server_count();

CREATE TABLE `t1` (id int(8) NOT NULL auto_increment primary key, bcol mediumtext);
INSERT INTO `t1` (bcol) values ('This documentation is NOT distributed under a GPL license. Use of this documentation is subject to the following terms: You may create a printed copy of this documentation solely for your own personal use. Conversion to other formats is allowed as long as the actual content is not altered or edited in any way. You shall not publish or distribute this documentation in any form or on any media, except if you distribute the documentation in a manner similar to how MySQL disseminates it (that is, electronically for download on a Web site with the software) or on a CD-ROM or similar medium, provided however that the documentation is disseminated together with the software on the same medium. Any other use, such as any dissemination of printed copies or use of this documentation, in whole or in part, in another publication, requires the prior written consent from an authorized representative of MySQL AB. MySQL AB reserves any and all rights to this documentation not expressly granted above.');
select memc_set('mysql:doc1', bcol) from t1;
select memc_get('mysql:doc1');

select memc_delete('mysql:doc1');
select memc_get('mysql:doc1');

-- test expiration argument 
select memc_set('mysql:doc1', bcol, 10) from t1;
select memc_get('mysql:doc1');
select sleep(5);
select memc_get('mysql:doc1');
select sleep(6);
select memc_get('mysql:doc1');

select memc_set('spot:test', ' Spot ');
select memc_get('spot:test');
select memc_prepend('spot:test', 'See');
select memc_get('spot:test');
select memc_append('spot:test', 'run.');
select memc_get('spot:test');
select memc_delete('spot:test');

select memc_set_by_key('A','mysql:doc1', bcol) from t1;
select memc_get_by_key('A','mysql:doc1');
select memc_delete_by_key('A','mysql:doc1');
select memc_get_by_key('A','mysql:doc1');

select memc_set_by_key('A', 'mysql:doc1', bcol, 10) from t1;
select memc_get_by_key('A', 'mysql:doc1');
select sleep(5);
select memc_get_by_key('A', 'mysql:doc1');
select sleep(6);
select memc_get_by_key('A', 'mysql:doc1');

select memc_set_by_key('A','spot:test', ' Spot ');
select memc_get_by_key('A','spot:test');
select memc_prepend_by_key('A','spot:test', 'See');
select memc_get_by_key('A','spot:test');
select memc_append_by_key('A','spot:test', 'run.');
select memc_get_by_key('A','spot:test');
select memc_delete_by_key('A','spot:test');

select memc_set('increment:test', 1);
select memc_get('increment:test');
select memc_increment('increment:test');
select memc_get('increment:test');
select memc_increment('increment:test', 5);
select memc_get('increment:test');
select memc_increment('increment:test', 20);
select memc_get('increment:test');
select memc_decrement('increment:test', 20);
select memc_get('increment:test');
select memc_decrement('increment:test');
select memc_get('increment:test');
select memc_delete('increment:test');

select memc_set('add:test', 'someval');
select memc_add('add:test', 'someval');
select memc_delete('add:test');
select memc_add('add:test', 'someval');
select memc_get('add:test');
select memc_delete('add:test');

select memc_add('add:test', 'someval', 5);
select memc_get('add:test');
select sleep(2);
select memc_get('add:test');
select sleep(4);
select memc_get('add:test');

select memc_replace('replace:test', 'replaced');
select memc_set('replace:test', 'initial');
select memc_get('replace:test');
select memc_replace('replace:test', 'replaced');
select memc_get('replace:test');
select memc_replace('replace:test', 'replaced again', 5);
select memc_get('replace:test');
select sleep(6);
select memc_get('replace:test');

select memc_replace_by_key('A', 'replace:test', 'replaced');
select memc_set_by_key('A', 'replace:test', 'initial');
select memc_get_by_key('A', 'replace:test');
select memc_replace_by_key('A', 'replace:test', 'replaced');
select memc_get_by_key('A', 'replace:test');
select memc_replace_by_key('A', 'replace:test', 'replaced again', 5);
select memc_get_by_key('A', 'replace:test');
select sleep(6);
select memc_get_by_key('A', 'replace:test');

select memc_add_by_key('A', 'add:test', 'someval', 5);
select memc_get_by_key('A', 'add:test');
select sleep(2);
select memc_get_by_key('A', 'add:test');
select sleep(4);
select memc_get_by_key('A', 'add:test');

select memc_set_by_key('A', 'add:test', 'someval');
select memc_add_by_key('A', 'add:test', 'someval');
select memc_delete_by_key('A', 'add:test');
select memc_add_by_key('A', 'add:test', 'someval');
select memc_get_by_key('A', 'add:test');
select memc_delete_by_key('A', 'add:test');

select memc_set('cas:test', 'casvalue');
select memc_get('cas:test');
select memc_cas('cas:test', 'casvalue changed?', 1);
select memc_get('cas:test');
select memc_cas('cas:test', 'casvalue changed again?', 22);
select memc_get('cas:test');

select memc_set_by_key('A','cas:test', 'casvalue');
select memc_get_by_key('A','cas:test');
select memc_cas_by_key('A','cas:test', 'casvalue', 1);
select memc_get_by_key('A','cas:test');
select memc_cas_by_key('A','cas:test', 'casvalue', 22);
select memc_get_by_key('A','cas:test');


select memc_list_behaviors();
select memc_list_distribution_types();
select memc_list_hash_types();

set @behavior = memc_behavior_get('MEMCACHED_BEHAVIOR_NO_BLOCK');
select @behavior;
select memc_behavior_set('MEMCACHED_BEHAVIOR_NO_BLOCK', '1');
select memc_behavior_get('MEMCACHED_BEHAVIOR_NO_BLOCK');
select memc_behavior_set('MEMCACHED_BEHAVIOR_NO_BLOCK', @behavior);

set @behavior = memc_behavior_get('MEMCACHED_BEHAVIOR_SUPPORT_CAS');
select @behavior;
select memc_behavior_set('MEMCACHED_BEHAVIOR_SUPPORT_CAS', '1');
select memc_behavior_get('MEMCACHED_BEHAVIOR_SUPPORT_CAS');
select memc_behavior_set('MEMCACHED_BEHAVIOR_SUPPORT_CAS', @behavior);

set @behavior = memc_behavior_get('MEMCACHED_BEHAVIOR_TCP_NODELAY');
select memc_behavior_set('MEMCACHED_BEHAVIOR_TCP_NODELAY', '1');
select memc_behavior_get('MEMCACHED_BEHAVIOR_TCP_NODELAY');
select memc_behavior_set('MEMCACHED_BEHAVIOR_TCP_NODELAY', @behavior);

set @behavior = memc_behavior_get('MEMCACHED_BEHAVIOR_BUFFER_REQUESTS');
select memc_behavior_set('MEMCACHED_BEHAVIOR_BUFFER_REQUESTS', '1');
select memc_behavior_get('MEMCACHED_BEHAVIOR_BUFFER_REQUESTS');
select memc_behavior_set('MEMCACHED_BEHAVIOR_BUFFER_REQUESTS', @behavior);

set @behavior = memc_behavior_get('MEMCACHED_BEHAVIOR_USER_DATA');
select memc_behavior_set('MEMCACHED_BEHAVIOR_USER_DATA', '1');
select memc_behavior_get('MEMCACHED_BEHAVIOR_USER_DATA');
select memc_behavior_set('MEMCACHED_BEHAVIOR_USER_DATA', @behavior);

set @behavior = memc_behavior_get('MEMCACHED_BEHAVIOR_SORT_HOSTS');
select memc_behavior_set('MEMCACHED_BEHAVIOR_SORT_HOSTS', '1');
select memc_behavior_get('MEMCACHED_BEHAVIOR_SORT_HOSTS');
select memc_behavior_set('MEMCACHED_BEHAVIOR_SORT_HOSTS', @behavior);

set @behavior = memc_behavior_get('MEMCACHED_BEHAVIOR_VERIFY_KEY');
select memc_behavior_set('MEMCACHED_BEHAVIOR_VERIFY_KEY', '1');
select memc_behavior_get('MEMCACHED_BEHAVIOR_VERIFY_KEY');
select memc_behavior_set('MEMCACHED_BEHAVIOR_VERIFY_KEY', @behavior);

set @behavior = memc_behavior_get('MEMCACHED_BEHAVIOR_KETAMA');
select memc_behavior_set('MEMCACHED_BEHAVIOR_KETAMA', '1');
select memc_behavior_get('MEMCACHED_BEHAVIOR_KETAMA');

set @behavior = memc_behavior_get('MEMCACHED_BEHAVIOR_CACHE_LOOKUPS');
select memc_behavior_set('MEMCACHED_BEHAVIOR_CACHE_LOOKUPS', '1');
select memc_behavior_get('MEMCACHED_BEHAVIOR_CACHE_LOOKUPS');
select memc_behavior_set('MEMCACHED_BEHAVIOR_CACHE_LOOKUPS', @behavior);

set @behavior = memc_behavior_get('MEMCACHED_BEHAVIOR_BUFFER_REQUESTS');
select memc_behavior_set('MEMCACHED_BEHAVIOR_BUFFER_REQUESTS', '1');
select memc_behavior_get('MEMCACHED_BEHAVIOR_BUFFER_REQUESTS');
select memc_behavior_set('MEMCACHED_BEHAVIOR_BUFFER_REQUESTS', @behavior);

set @preserve_behavior = memc_behavior_get('MEMCACHED_BEHAVIOR_HASH');
select memc_behavior_set('MEMCACHED_BEHAVIOR_HASH','MEMCACHED_HASH_DEFAULT' );
select memc_behavior_get('MEMCACHED_BEHAVIOR_HASH');

select memc_behavior_set('MEMCACHED_BEHAVIOR_HASH','MEMCACHED_HASH_MD5' );
select memc_behavior_get('MEMCACHED_BEHAVIOR_HASH');

select memc_behavior_set('MEMCACHED_BEHAVIOR_HASH','MEMCACHED_HASH_CRC' );
select memc_behavior_get('MEMCACHED_BEHAVIOR_HASH');

select memc_behavior_set('MEMCACHED_BEHAVIOR_HASH','MEMCACHED_HASH_FNV1_64' );
select memc_behavior_get('MEMCACHED_BEHAVIOR_HASH');

select memc_behavior_set('MEMCACHED_BEHAVIOR_HASH','MEMCACHED_HASH_FNV1A_64' );
select memc_behavior_get('MEMCACHED_BEHAVIOR_HASH');

select memc_behavior_set('MEMCACHED_BEHAVIOR_HASH','MEMCACHED_HASH_FNV1_32' );
select memc_behavior_get('MEMCACHED_BEHAVIOR_HASH');

select memc_behavior_set('MEMCACHED_BEHAVIOR_HASH','MEMCACHED_HASH_FNV1A_32' );
select memc_behavior_get('MEMCACHED_BEHAVIOR_HASH');

select memc_behavior_set('MEMCACHED_BEHAVIOR_HASH','MEMCACHED_HASH_JENKINS' );
select memc_behavior_get('MEMCACHED_BEHAVIOR_HASH');

select memc_behavior_set('MEMCACHED_BEHAVIOR_HASH','MEMCACHED_HASH_HSIEH' );
select memc_behavior_get('MEMCACHED_BEHAVIOR_HASH');

select memc_behavior_set('MEMCACHED_BEHAVIOR_HASH','MEMCACHED_HASH_MURMUR' );
select memc_behavior_get('MEMCACHED_BEHAVIOR_HASH');

select memc_behavior_set('MEMCACHED_BEHAVIOR_HASH', @preserve_behavior);

set @preserve_dist= memc_behavior_get('MEMCACHED_BEHAVIOR_DISTRIBUTION');
select memc_behavior_set('MEMCACHED_BEHAVIOR_DISTRIBUTION', 'MEMCACHED_DISTRIBUTION_MODULA');
select memc_behavior_get('MEMCACHED_BEHAVIOR_DISTRIBUTION');

select memc_behavior_set('MEMCACHED_BEHAVIOR_DISTRIBUTION', 'MEMCACHED_DISTRIBUTION_CONSISTENT');
select memc_behavior_get('MEMCACHED_BEHAVIOR_DISTRIBUTION');

select memc_behavior_set('MEMCACHED_BEHAVIOR_DISTRIBUTION', 'MEMCACHED_DISTRIBUTION_CONSISTENT_KETAMA');
select memc_behavior_get('MEMCACHED_BEHAVIOR_DISTRIBUTION');
select memc_behavior_set('MEMCACHED_BEHAVIOR_DISTRIBUTION', @preserve_dist);

set @size = memc_behavior_get('MEMCACHED_BEHAVIOR_SOCKET_SEND_SIZE');
select @size;
select memc_behavior_set('MEMCACHED_BEHAVIOR_SOCKET_SEND_SIZE', 60000);
select memc_behavior_get('MEMCACHED_BEHAVIOR_SOCKET_SEND_SIZE');
select memc_behavior_set('MEMCACHED_BEHAVIOR_SOCKET_SEND_SIZE', @size);

set @size = memc_behavior_get('MEMCACHED_BEHAVIOR_SOCKET_RECV_SIZE');
select @size;
select memc_behavior_set('MEMCACHED_BEHAVIOR_SOCKET_RECV_SIZE', 120000);
select memc_behavior_get('MEMCACHED_BEHAVIOR_SOCKET_RECV_SIZE');
select memc_behavior_set('MEMCACHED_BEHAVIOR_SOCKET_RECV_SIZE', @size);

set @size = memc_behavior_get('MEMCACHED_BEHAVIOR_POLL_TIMEOUT');
select @size;
select memc_behavior_set('MEMCACHED_BEHAVIOR_POLL_TIMEOUT', 100);
select memc_behavior_get('MEMCACHED_BEHAVIOR_POLL_TIMEOUT');
select memc_behavior_set('MEMCACHED_BEHAVIOR_POLL_TIMEOUT', @size);


set @size = memc_behavior_get('MEMCACHED_BEHAVIOR_RETRY_TIMEOUT');
select @size;
select memc_behavior_set('MEMCACHED_BEHAVIOR_RETRY_TIMEOUT', 5);
select memc_behavior_get('MEMCACHED_BEHAVIOR_RETRY_TIMEOUT');
select memc_behavior_set('MEMCACHED_BEHAVIOR_RETRY_TIMEOUT', @size);

set @size = memc_behavior_get('MEMCACHED_BEHAVIOR_IO_MSG_WATERMARK');
select @size;
select memc_behavior_set('MEMCACHED_BEHAVIOR_IO_MSG_WATERMARK', 5);
select memc_behavior_get('MEMCACHED_BEHAVIOR_IO_MSG_WATERMARK');
select memc_behavior_set('MEMCACHED_BEHAVIOR_IO_MSG_WATERMARK', @size);

set @size = memc_behavior_get('MEMCACHED_BEHAVIOR_IO_BYTES_WATERMARK');
select @size;
select memc_behavior_set('MEMCACHED_BEHAVIOR_IO_BYTES_WATERMARK', 5);
select memc_behavior_get('MEMCACHED_BEHAVIOR_IO_BYTES_WATERMARK');
select memc_behavior_set('MEMCACHED_BEHAVIOR_IO_BYTES_WATERMARK', @size);

select memc_stat_get_keys();

select if (memc_stats('localhost') IS NOT NULL, 'true', 'false');
