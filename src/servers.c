/* Copyright (C) 2008 Patrick Galbraith, Brian Aker

  See COPYING file found with distribution for license.

*/

#include <mysql.h>
#include <string.h>
#include <pthread.h>

#include <stdio.h>
#include <assert.h>

#include "common.h"
#define BRIAN_LIKES_LOOP 1;

pthread_mutex_t memc_servers_mutex= PTHREAD_MUTEX_INITIALIZER;

/*
  select memc_servers_set("hostname:23,hostname2:44");
*/

static memcached_st *master_memc= NULL;


my_bool memc_servers_set_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
long long memc_servers_set(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error);
/*void memc_servers_set_deinit(UDF_INIT *initid);*/

my_bool memc_server_count_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
long long memc_server_count(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error);

my_bool memc_servers_behavior_set_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
long long memc_servers_behavior_set(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error);
/*void memc_servers_behavior_set_deinit(UDF_INIT *initid);*/
my_bool memc_behavior_set_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
long long memc_behavior_set(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error);

my_bool memc_servers_behavior_get_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
char *memc_servers_behavior_get(UDF_INIT *initid, UDF_ARGS *args, char *result,
                                    unsigned long *length, char *is_null, char *error);

my_bool memc_behavior_get_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
char *memc_behavior_get(UDF_INIT *initid, UDF_ARGS *args, char *result,
                                    unsigned long *length, char *is_null, char *error);

my_bool memc_list_behaviors_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
char *memc_list_behaviors(UDF_INIT *initid, UDF_ARGS *args, char *result,
                          unsigned long *length, char *is_null, char *error);
void memc_list_behaviors_deinit(UDF_INIT *initid);

my_bool memc_list_hash_types_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
char *memc_list_hash_types(UDF_INIT *initid, UDF_ARGS *args, char *result,
                          unsigned long *length, char *is_null, char *error);

my_bool memc_list_distribution_types_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
char *memc_list_distribution_types(UDF_INIT *initid, UDF_ARGS *args, char *result,
                          unsigned long *length, char *is_null, char *error);

my_bool memc_servers_set_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
  /* this is how to fail */
  if (args->arg_count != 1)
  {
    strncpy(message, "String of memcached servers.", MYSQL_ERRMSG_SIZE);
    return 1;
  }

  args->arg_type[0]= STRING_RESULT;

  /* initid->ptr keeps state for between memc_get_init and memc_get_deinit */
  initid->ptr= NULL;

  return 0;
}

/*
  memc_servers_set
  get cached object, takes hash-key arg
*/
long long memc_servers_set(__attribute__ ((unused)) UDF_INIT *initid,
                       UDF_ARGS *args,
                       __attribute__ ((unused)) char *result,
                       __attribute__ ((unused)) unsigned long *length,
                       char *is_null,
                       __attribute__ ((unused)) char *error)
{
  uint64_t set= 1;
  memcached_return rc= 0;
  memcached_server_st *servers;


  pthread_mutex_lock(&memc_servers_mutex);

  if (master_memc == NULL)
    master_memc= memcached_create(NULL);
  if (master_memc == NULL)
  {
    *is_null= 1;
    return 0;
  }

  servers= memcached_servers_parse(args->args[0]);
  if (servers == NULL)
  {
    *is_null= 1;
    return 0;
  }
  rc= memcached_server_push(master_memc, servers);
  if (rc != MEMCACHED_SUCCESS)
  {
    *is_null= 1;
    return 0;
  }
  /*
    enable cas by default. This can be undonw with
    memc_behavior_set
  */
  memcached_version(master_memc);

  /*
   * don't enable CAS anyway, this breaks MemcacheQ
   *
   * If server pool is heterogenous for some reason,
   * (memcached was added as first server and CAS
   * was enabled, then MemcacheQ added).
   */

  /*if (master_memc->hosts[0].major_version >= 1 &&
      master_memc->hosts[0].minor_version >= 2 &&
      master_memc->hosts[0].micro_version >= 4)
    memcached_behavior_set(master_memc, MEMCACHED_BEHAVIOR_SUPPORT_CAS, set);
    fprintf(stderr, "version %d.%d.%d\n", master_memc->hosts[0].major_version,
            master_memc->hosts[0].minor_version,
            master_memc->hosts[0].micro_version);*/

  memcached_server_list_free(servers);

  pthread_mutex_unlock(&memc_servers_mutex);

  fprintf(stderr, "rc %d\n", rc);
  return ((long long) rc == MEMCACHED_SUCCESS ? 0 : rc);
}

my_bool memc_server_count_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
  /* this is how to fail */
  if (args->arg_count > 0)
  {
    strncpy(message, "This UDF takes no arguments: memc_server_count()", MYSQL_ERRMSG_SIZE);
    return 1;
  }

  /* initid->ptr keeps state for between memc_server_count_init and memc_server_count_deinit*/
  initid->ptr= NULL;

  return 0;
}

/*
  memc_server_count
  get cached object, takes hash-key arg
*/
long long memc_server_count(__attribute__ ((unused)) UDF_INIT *initid,
                       __attribute__ ((unused)) UDF_ARGS *args,
                       __attribute__ ((unused)) char *result,
                       __attribute__ ((unused)) unsigned long *length,
                       __attribute__ ((unused)) char *is_null,
                       __attribute__ ((unused)) char *error)
{

  unsigned int count;
  pthread_mutex_lock(&memc_servers_mutex);

  if (master_memc == NULL)
    master_memc= memcached_create(NULL);

  count= memcached_server_count(master_memc);

  pthread_mutex_unlock(&memc_servers_mutex);

  return ((long long) count);
}
my_bool memc_server_version_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
  memcached_return rc;
  memc_function_st *container;

  /* this is how to fail */
  if (args->arg_count != 1)
  {
    strncpy(message,
            "1 argument required: servers, comma-separated: memc_server_version(<servers>)",
            MYSQL_ERRMSG_SIZE);
    return 1;
  }

  args->arg_type[0]= STRING_RESULT;

  initid->max_length= MEMC_UDF_MAX_SIZE;
  container= calloc(1, sizeof(memc_function_st));

  /* Init the memcached_st we will use for this pass */
  rc= memc_get_servers(&container->memc);

  /* Now setup the string */
  container->stats_string= memcached_string_create(&container->memc, NULL, 1024);

  initid->ptr= (char *)container;

  return 0;
}

/*
  memc_server_count
  get cached object, takes hash-key arg
*/
char *memc_server_version(UDF_INIT *initid,
                       UDF_ARGS *args,
                       __attribute__ ((unused)) char *result,
                       unsigned long *length,
                       __attribute__ ((unused)) char *is_null,
                       __attribute__ ((unused)) char *error)
{

  unsigned int count= 0;
  unsigned int i= 0;
  memcached_return rc= MEMCACHED_SUCCESS;
  char buf[100];
  memcached_server_st *servers;
  memcached_server_st *server_list;

  memc_function_st *container= (memc_function_st *)initid->ptr;
  memcached_string_reset(container->stats_string);

  servers= memcached_servers_parse(args->args[0]);
  memcached_server_push(&container->memc, servers);
  memcached_server_list_free(servers);

  if (rc != MEMCACHED_SUCCESS && rc != MEMCACHED_SOME_ERRORS)
  {
    sprintf(error, "Failure to communicate with servers (%s)\n",
	   memcached_strerror(&container->memc, rc));
    *length= strlen(error);
    return(error);
  }

  server_list= memcached_server_list(&container->memc);

  for (i= 0; i < memcached_server_count(&container->memc); i++)
  {
    sprintf(buf, "Server: %s (%u) ",
            memcached_server_name(&container->memc, server_list[i]),
            memcached_server_port(&container->memc, server_list[i]));

    memcached_string_append(container->stats_string, buf, strlen(buf));

    rc= memcached_version(&container->memc);

    fprintf(stderr, "rc %d error: %s\n",
            rc, memcached_strerror(&container->memc, rc));

    fprintf(stderr, "version %d.%d.%d\n",
            container->memc.hosts[i].major_version,
            container->memc.hosts[i].minor_version,
            container->memc.hosts[i].micro_version);

    sprintf(buf, "version %d.%d.%d\n",
            container->memc.hosts[i].major_version,
            container->memc.hosts[i].minor_version,
            container->memc.hosts[i].micro_version);

    memcached_string_append(container->stats_string, buf, strlen(buf));
  }
  *length= container->stats_string->end - container->stats_string->string;
  return container->stats_string->string;
}

void memc_server_version_deinit(UDF_INIT *initid)
{
  /* if we allocated initid->ptr, free it here */
  memc_function_st *container= (memc_function_st *)initid->ptr;

  memcached_string_free(container->stats_string);
  memcached_free(&container->memc);
  free(container);

  return;
}




/* memc_servers_behavior_set_init */
my_bool memc_servers_behavior_set_init(__attribute__ ((unused)) UDF_INIT *initid,
                                       UDF_ARGS *args,
                                       char *message)
{
  uint64_t setting;
  unsigned int count;

  /* Fail if not two args */
  if (args->arg_count != 2)
  {
    strncpy(message,
            "2 arguments must be supplied: memc_servers_behavior_set('<behavior type>', '<value>')",
            MYSQL_ERRMSG_SIZE);
    return 1;
  }
  pthread_mutex_lock(&memc_servers_mutex);
  if (master_memc == NULL)
    master_memc= memcached_create(NULL);

  count= memcached_server_count(master_memc);

  pthread_mutex_unlock(&memc_servers_mutex);

  if (!count) {
    strncpy(message,
            "Servers not set! memc_servers_set('server:port,...')",
            MYSQL_ERRMSG_SIZE);
    return 1;
  }

  args->arg_type[0]= STRING_RESULT;

  if (args->arg_type[1] == INT_RESULT)
    setting= *(uint64_t*) args->args[1];
  else if (args->arg_type[1] == STRING_RESULT)
    setting= (uint64_t)atoi(args->args[1]);

  /*
    Do some checking of supplied behavior and setting

    If a 1|0 value, check. Print error if not. We need some sort of 
    check for the other non 1|0 behaviors
  */
  if (! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_SUPPORT_CAS") ||
      ! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_NO_BLOCK") ||
      ! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_BUFFER_REQUESTS") ||
      ! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_USER_DATA") ||
      ! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_SORT_HOSTS") ||
      ! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_VERIFY_KEY") ||
      ! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_TCP_NODELAY") ||
      ! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_KETAMA") ||
      ! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_CACHE_LOOKUPS") ||
      ! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_BUFFER_REQUESTS"))
  {
      if (setting != 1 && setting != 0) {
        strncpy(message, "INALID VALUE FOR BEHAVIOR - MUST BE 1 OR 0 !\n", MYSQL_ERRMSG_SIZE);
        return 1;
      }
  }
  else if ( ! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_DISTRIBUTION"))
  {
    if (strcasecmp(args->args[1], "MEMCACHED_DISTRIBUTION_MODULA") &&
        strcasecmp(args->args[1], "MEMCACHED_DISTRIBUTION_CONSISTENT") &&
        strcasecmp(args->args[1], "MEMCACHED_DISTRIBUTION_CONSISTENT_KETAMA") )
    {
        strncpy(message, "INALID VALUE FOR DISTRIBUTION! see: memc_list_distribution_types();", MYSQL_ERRMSG_SIZE);
        return 1;
    }
    /* args->arg_type[1]= STRING_RESULT; */
  }
  else if (! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_HASH"))
  {
    if (strcasecmp(args->args[1], "MEMCACHED_HASH_DEFAULT") &&
        strcasecmp(args->args[1], "MEMCACHED_HASH_MD5") &&
        strcasecmp(args->args[1], "MEMCACHED_HASH_CRC") &&
        strcasecmp(args->args[1], "MEMCACHED_HASH_FNV1_64") &&
        strcasecmp(args->args[1], "MEMCACHED_HASH_FNV1A_64") &&
        strcasecmp(args->args[1], "MEMCACHED_HASH_FNV1_32") &&
        strcasecmp(args->args[1], "MEMCACHED_HASH_FNV1A_32") &&
        strcasecmp(args->args[1], "MEMCACHED_HASH_JENKINS") &&
        strcasecmp(args->args[1], "MEMCACHED_HASH_HSIEH") &&
        strcasecmp(args->args[1], "MEMCACHED_HASH_MURMUR"))
    {
        strncpy(message, "INVALID VALUE FOR MEMCACHED HASH ALGORITHM! see: memc_list_hash_types();", 71);
        return 1;
    }
    /*args->arg_type[1]= STRING_RESULT;*/
  }
  else if ( ! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_KETAMA_HASH"))
  {
    if (strcasecmp(args->args[1], "MEMCACHED_HASH_DEFAULT") &&
        strcasecmp(args->args[1], "MEMCACHED_HASH_MD5") &&
        strcasecmp(args->args[1], "MEMCACHED_HASH_CRC") &&
        strcasecmp(args->args[1], "MEMCACHED_HASH_FNV1_64") &&
        strcasecmp(args->args[1], "MEMCACHED_HASH_FNV1A_64") &&
        strcasecmp(args->args[1], "MEMCACHED_HASH_FNV1_32") &&
        strcasecmp(args->args[1], "MEMCACHED_HASH_FNV1A_32"))
    {
        strncpy(message, "INVALID VALUE FOR MEMCACHED HASH ALGORITHM! see: memc_list_hash_types();", 71);
        return 1;
    }
  }
  else if ( ! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_SOCKET_SEND_SIZE") ||
      ! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_SOCKET_RECV_SIZE") ||
      ! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_POLL_TIMEOUT") ||
      ! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_CONNECT_TIMEOUT") ||
      ! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_RETRY_TIMEOUT") ||
      ! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_IO_MSG_WATERMARK") ||
      ! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_IO_BYTES_WATERMARK"))
  {
    /*
      What type of check the values passed to these behaviors?
      Range?
    */
  }
  else
  {
    strncpy(message, "UNKNOWN BEHAVIOR TYPE! try memc_list_behaviors() for a list of behaviors.", MYSQL_ERRMSG_SIZE);
    return 1;
  }



  return 0;
}

/* memc_servers_behavior_set */
long long memc_servers_behavior_set(__attribute__ ((unused)) UDF_INIT *initid,
                                    UDF_ARGS *args,
                                    __attribute__ ((unused)) char *is_null,
                                    __attribute__ ((unused)) char *error)
{
  memcached_return rc;
  memcached_behavior behavior;
  uint64_t setting;

  if (args->arg_type[1] == INT_RESULT)
    setting= *(uint64_t*) args->args[1];
  else if (args->arg_type[1] == STRING_RESULT)
    setting= (uint64_t)atoi(args->args[1]);

  if (! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_SUPPORT_CAS"))
  {
    behavior= MEMCACHED_BEHAVIOR_SUPPORT_CAS;
  }
  else if (! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_NO_BLOCK"))
  {
    behavior= MEMCACHED_BEHAVIOR_NO_BLOCK;
  }
  else if (! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_TCP_NODELAY"))
  {
    behavior= MEMCACHED_BEHAVIOR_TCP_NODELAY;
  }
  else if (! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_HASH"))
  {
    if (! strcasecmp(args->args[1], "MEMCACHED_HASH_DEFAULT"))
    {
      setting= MEMCACHED_HASH_DEFAULT;
    }
    else if (! strcasecmp(args->args[1], "MEMCACHED_HASH_MD5"))
    {
      setting= MEMCACHED_HASH_MD5;
    }
    else if (! strcasecmp(args->args[1], "MEMCACHED_HASH_CRC"))
    {
      setting= MEMCACHED_HASH_CRC;
    }
    else if (! strcasecmp(args->args[1],"MEMCACHED_HASH_FNV1_64"))
    {
      setting= MEMCACHED_HASH_FNV1_64;
    }
    else if (! strcasecmp(args->args[1], "MEMCACHED_HASH_FNV1A_64"))
    {
      setting= MEMCACHED_HASH_FNV1A_64;
    }
    else if (! strcasecmp(args->args[1],"MEMCACHED_HASH_FNV1_32"))
    {
      setting= MEMCACHED_HASH_FNV1_32;
    }
    else if (! strcasecmp(args->args[1], "MEMCACHED_HASH_FNV1A_32"))
    {
      setting= MEMCACHED_HASH_FNV1A_32;
    }
    else if (! strcasecmp(args->args[1], "MEMCACHED_HASH_JENKINS"))
    {
      setting= MEMCACHED_HASH_JENKINS;
    }
    else if (! strcasecmp(args->args[1], "MEMCACHED_HASH_HSIEH"))
    {
      setting= MEMCACHED_HASH_HSIEH;
    }
    else if (! strcasecmp(args->args[1], "MEMCACHED_HASH_MURMUR"))
    {
      setting= MEMCACHED_HASH_MURMUR;
    }
    else {
      setting= MEMCACHED_HASH_DEFAULT;
    }
    behavior= MEMCACHED_BEHAVIOR_HASH;
  }
  else if (! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_CACHE_LOOKUPS"))
  {
    behavior= MEMCACHED_BEHAVIOR_CACHE_LOOKUPS;
  }
  else if (! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_SOCKET_SEND_SIZE"))
  {
    behavior= MEMCACHED_BEHAVIOR_SOCKET_SEND_SIZE;
  }
  else if (! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_SOCKET_RECV_SIZE"))
  {
    behavior= MEMCACHED_BEHAVIOR_SOCKET_RECV_SIZE;
  }
  else if (! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_BUFFER_REQUESTS"))
  {
    behavior= MEMCACHED_BEHAVIOR_BUFFER_REQUESTS;
  }
  else if (! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_BUFFER_REQUESTS"))
  {
    behavior= MEMCACHED_BEHAVIOR_BUFFER_REQUESTS;
  }
  else if (! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_KETAMA"))
  {
    behavior= MEMCACHED_BEHAVIOR_KETAMA;
  }
  else if (! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_POLL_TIMEOUT"))
  {
    behavior= MEMCACHED_BEHAVIOR_POLL_TIMEOUT;
  }
  else if (! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_RETRY_TIMEOUT"))
  {
    behavior= MEMCACHED_BEHAVIOR_RETRY_TIMEOUT;
  }
  else if (! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_DISTRIBUTION"))
  {
    if (! strcasecmp(args->args[1], "MEMCACHED_DISTRIBUTION_MODULA")) 
      setting= MEMCACHED_DISTRIBUTION_MODULA;
    else if (! strcasecmp(args->args[1], "MEMCACHED_DISTRIBUTION_CONSISTENT")) 
      setting= MEMCACHED_DISTRIBUTION_CONSISTENT;
    else if (! strcasecmp(args->args[1], "MEMCACHED_DISTRIBUTION_CONSISTENT_KETAMA")) 
      setting= MEMCACHED_DISTRIBUTION_CONSISTENT_KETAMA;
    else
      setting= MEMCACHED_DISTRIBUTION_MODULA;

    behavior= MEMCACHED_BEHAVIOR_DISTRIBUTION;
  }
  else if (! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_USER_DATA"))
  {
    behavior= MEMCACHED_BEHAVIOR_USER_DATA;
  }
  else if (! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_SORT_HOSTS"))
  {
    behavior= MEMCACHED_BEHAVIOR_SORT_HOSTS;
  }
  else if (! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_VERIFY_KEY"))
  {
    behavior= MEMCACHED_BEHAVIOR_VERIFY_KEY;
  }
  else if (! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_CONNECT_TIMEOUT"))
  {
    behavior= MEMCACHED_BEHAVIOR_CONNECT_TIMEOUT;
  }
  else if (! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_KETAMA_WEIGHTED"))
  {
    behavior= MEMCACHED_BEHAVIOR_KETAMA_WEIGHTED;
  }
  else if (! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_KETAMA_HASH"))
  {
    if (! strcasecmp(args->args[1], "MEMCACHED_HASH_DEFAULT"))
    {
      setting= MEMCACHED_HASH_DEFAULT;
    }
    else if (! strcasecmp(args->args[1], "MEMCACHED_HASH_MD5"))
    {
      setting= MEMCACHED_HASH_MD5;
    }
    else if (! strcasecmp(args->args[1], "MEMCACHED_HASH_CRC"))
    {
      setting= MEMCACHED_HASH_CRC;
    }
    else if (! strcasecmp(args->args[1],"MEMCACHED_HASH_FNV1_64"))
    {
      setting= MEMCACHED_HASH_FNV1_64;
    }
    else if (! strcasecmp(args->args[1], "MEMCACHED_HASH_FNV1A_64"))
    {
      setting= MEMCACHED_HASH_FNV1A_64;
    }
    else if (! strcasecmp(args->args[1],"MEMCACHED_HASH_FNV1_32"))
    {
      setting= MEMCACHED_HASH_FNV1_32;
    }
    else if (! strcasecmp(args->args[1], "MEMCACHED_HASH_FNV1A_32"))
    {
      setting= MEMCACHED_HASH_FNV1A_32;
    }
    else {
      setting= MEMCACHED_HASH_DEFAULT;
    }
    behavior= MEMCACHED_BEHAVIOR_KETAMA_HASH;
  }
  else if (! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_BINARY_PROTOCOL"))
  {
    behavior= MEMCACHED_BEHAVIOR_BINARY_PROTOCOL;
  }
  else if (! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_SND_TIMEOUT"))
  {
    behavior= MEMCACHED_BEHAVIOR_SND_TIMEOUT;
  }
  else if (! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_RCV_TIMEOUT"))
  {
    behavior= MEMCACHED_BEHAVIOR_RCV_TIMEOUT;
  }
  else if (! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_SERVER_FAILURE_LIMIT"))
  {
    behavior= MEMCACHED_BEHAVIOR_SERVER_FAILURE_LIMIT;

  }
  else if (! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_IO_MSG_WATERMARK"))
  {
    behavior= MEMCACHED_BEHAVIOR_IO_MSG_WATERMARK;

  }
  else if (! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_IO_BYTES_WATERMARK"))
  {
    behavior= MEMCACHED_BEHAVIOR_IO_BYTES_WATERMARK;

  }
  else
  {
    sprintf(error, "ERROR: UNKNOWN BEHAVIOR TYPE!");
    return 1;
  }


  /*
    Get a lock and modify the behavior of the master_memc
    that changes memcached_st behavior.
  */
  pthread_mutex_lock(&memc_servers_mutex);
  rc= memcached_behavior_set(master_memc, behavior, setting);
  pthread_mutex_unlock(&memc_servers_mutex);

  return 0;
}

/* memc_servers_behavior_set_init */
my_bool memc_servers_behavior_get_init(__attribute__ ((unused)) UDF_INIT *initid,
                                       UDF_ARGS *args,
                                       char *message)
{
  unsigned int count;
  /* Fail if not two args */
  if (args->arg_count != 1)
  {
    strncpy(message,
            "one argument must be supplied: memc_servers_behavior_get('<behavior type>')",
            MYSQL_ERRMSG_SIZE);
    return 1;
  }
  pthread_mutex_lock(&memc_servers_mutex);
  if (master_memc == NULL)
    master_memc= memcached_create(NULL);
  count= memcached_server_count(master_memc);

  pthread_mutex_unlock(&memc_servers_mutex);

  if (!count) {
    strncpy(message,
            "Servers not set! memc_servers_set('server:port,...')",
            MYSQL_ERRMSG_SIZE);
    return 1;
  }
  args->arg_type[0]= STRING_RESULT;

  /* make sure valid behavior */
  if (! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_SUPPORT_CAS") ||
      ! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_NO_BLOCK") ||
      ! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_POLL_TIMEOUT") ||
      ! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_CONNECT_TIMEOUT") ||
      ! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_RETRY_TIMEOUT") ||
      ! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_BUFFER_REQUESTS") ||
      ! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_USER_DATA") ||
      ! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_SORT_HOSTS") ||
      ! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_VERIFY_KEY") ||
      ! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_TCP_NODELAY") ||
      ! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_KETAMA") ||
      ! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_CACHE_LOOKUPS") ||
      ! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_BUFFER_REQUESTS") ||
      ! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_SOCKET_SEND_SIZE") ||
      ! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_SOCKET_RECV_SIZE") ||
      ! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_DISTRIBUTION") ||
      ! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_KETAMA_HASH") ||
      ! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_HASH") ||
      ! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_SOCKET_SEND_SIZE") ||
      ! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_POLL_TIMEOUT") ||
      ! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_CONNECT_TIMEOUT") ||
      ! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_RETRY_TIMEOUT") ||
      ! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_SOCKET_RECV_SIZE") ||
      ! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_IO_MSG_WATERMARK") ||
      ! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_IO_BYTES_WATERMARK"))
  {
        return 0;
  }
  else
  {
    strncpy(message, "UNKNOWN BEHAVIOR TYPE! try memc_list_behaviors() for a list of behaviors.", MYSQL_ERRMSG_SIZE);
    return 1;
  }
}

/* memc_servers_behavior_get */
char *memc_servers_behavior_get(__attribute__ ((unused)) UDF_INIT *initid,
                                    UDF_ARGS *args,
                                    __attribute__ ((unused)) char *result,
                                    unsigned long *length,
                                    __attribute__ ((unused)) char *is_null,
                                    __attribute__ ((unused)) char *error)
{
  memcached_return rc;
  memcached_behavior behavior;
  uint64_t ivalue;
  static char svalue[40];

  if (! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_SUPPORT_CAS"))
    behavior= MEMCACHED_BEHAVIOR_SUPPORT_CAS;
  else if (! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_NO_BLOCK"))
    behavior= MEMCACHED_BEHAVIOR_NO_BLOCK;
  else if (! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_TCP_NODELAY"))
    behavior= MEMCACHED_BEHAVIOR_TCP_NODELAY;
  else if (! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_HASH"))
    behavior= MEMCACHED_BEHAVIOR_HASH;
  else if (! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_CACHE_LOOKUPS"))
    behavior= MEMCACHED_BEHAVIOR_CACHE_LOOKUPS;
  else if (! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_SOCKET_SEND_SIZE"))
    behavior= MEMCACHED_BEHAVIOR_SOCKET_SEND_SIZE;
  else if (! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_SOCKET_RECV_SIZE"))
    behavior= MEMCACHED_BEHAVIOR_SOCKET_RECV_SIZE;
  else if (! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_BUFFER_REQUESTS"))
    behavior= MEMCACHED_BEHAVIOR_BUFFER_REQUESTS;
  else if (! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_BUFFER_REQUESTS"))
    behavior= MEMCACHED_BEHAVIOR_BUFFER_REQUESTS;
  else if (! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_KETAMA"))
    behavior= MEMCACHED_BEHAVIOR_KETAMA;
  else if (! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_POLL_TIMEOUT"))
    behavior= MEMCACHED_BEHAVIOR_POLL_TIMEOUT;
  else if (! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_RETRY_TIMEOUT"))
    behavior= MEMCACHED_BEHAVIOR_RETRY_TIMEOUT;
  else if (! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_DISTRIBUTION"))
    behavior= MEMCACHED_BEHAVIOR_DISTRIBUTION;
  else if (! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_USER_DATA"))
    behavior= MEMCACHED_BEHAVIOR_USER_DATA;
  else if (! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_SORT_HOSTS"))
    behavior= MEMCACHED_BEHAVIOR_SORT_HOSTS;
  else if (! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_VERIFY_KEY"))
    behavior= MEMCACHED_BEHAVIOR_VERIFY_KEY;
  else if (! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_CONNECT_TIMEOUT"))
    behavior= MEMCACHED_BEHAVIOR_CONNECT_TIMEOUT;
  else if (! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_KETAMA_WEIGHTED"))
    behavior= MEMCACHED_BEHAVIOR_KETAMA_WEIGHTED;
  else if (! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_KETAMA_HASH"))
    behavior= MEMCACHED_BEHAVIOR_KETAMA_HASH;
  else if (! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_BINARY_PROTOCOL"))
    behavior= MEMCACHED_BEHAVIOR_BINARY_PROTOCOL;
  else if (! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_SND_TIMEOUT"))
    behavior= MEMCACHED_BEHAVIOR_SND_TIMEOUT;
  else if (! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_RCV_TIMEOUT"))
    behavior= MEMCACHED_BEHAVIOR_RCV_TIMEOUT;
  else if (! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_SERVER_FAILURE_LIMIT"))
    behavior= MEMCACHED_BEHAVIOR_SERVER_FAILURE_LIMIT;
  else if (! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_IO_MSG_WATERMARK"))
    behavior= MEMCACHED_BEHAVIOR_IO_MSG_WATERMARK;
  else if (! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_IO_BYTES_WATERMARK"))
    behavior= MEMCACHED_BEHAVIOR_IO_BYTES_WATERMARK;
  else
  {
    sprintf(error, "ERROR: UNKNOWN BEHAVIOR TYPE!");
    return (char *)NULL;
  }

  /*
    Get a lock and modify the behavior of the master_memc
    that changes memcached_st behavior.
  */
  pthread_mutex_lock(&memc_servers_mutex);
  ivalue= memcached_behavior_get(master_memc, behavior);
  pthread_mutex_unlock(&memc_servers_mutex);

  if ( ! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_DISTRIBUTION")) {
    if (ivalue == MEMCACHED_DISTRIBUTION_MODULA) {
        sprintf(svalue, "%s", "MEMCACHED_DISTRIBUTION_MODULA");
    }
    else if (ivalue == MEMCACHED_DISTRIBUTION_CONSISTENT) {
        sprintf(svalue, "%s", "MEMCACHED_DISTRIBUTION_CONSISTENT");
    }
    else if (ivalue == MEMCACHED_DISTRIBUTION_CONSISTENT_KETAMA) {
        sprintf(svalue, "%s", "MEMCACHED_DISTRIBUTION_CONSISTENT_KETAMA");
    }
    else
    {
        sprintf(svalue, "%s", "UNKOWN DISTRIBUTION");
    }
  }
  else if (! strcasecmp(args->args[0], "MEMCACHED_BEHAVIOR_HASH")) {
    if (ivalue == MEMCACHED_HASH_DEFAULT) {
        sprintf(svalue, "%s", "MEMCACHED_HASH_DEFAULT");
    }
    else if (ivalue == MEMCACHED_HASH_MD5) {
        sprintf(svalue, "%s", "MEMCACHED_HASH_MD5");
    }
    else if (ivalue == MEMCACHED_HASH_CRC) {
        sprintf(svalue, "%s", "MEMCACHED_HASH_CRC");
    }
    else if (ivalue == MEMCACHED_HASH_FNV1_64) {
        sprintf(svalue, "%s", "MEMCACHED_HASH_FNV1_64");
    }
    else if (ivalue == MEMCACHED_HASH_FNV1A_64) {
        sprintf(svalue, "%s", "MEMCACHED_HASH_FNV1A_64");
    }
    else if (ivalue == MEMCACHED_HASH_FNV1_32) {
        sprintf(svalue, "%s", "MEMCACHED_HASH_FNV1_32");
    }
    else if (ivalue == MEMCACHED_HASH_FNV1A_32) {
        sprintf(svalue, "%s", "MEMCACHED_HASH_FNV1A_32");
    }
    else if (ivalue == MEMCACHED_HASH_JENKINS) {
        sprintf(svalue, "%s", "MEMCACHED_HASH_JENKINS");
    }
    else if (ivalue == MEMCACHED_HASH_HSIEH) {
        sprintf(svalue, "%s", "MEMCACHED_HASH_HSIEH");
    }
    else if (ivalue == MEMCACHED_HASH_MURMUR) {
        sprintf(svalue, "%s", "MEMCACHED_HASH_MURMUR");
    }
    else {
        sprintf(svalue, "%s", "UNKNOWN HASH TYPE");
    }
  }
  else
  {
    sprintf(svalue, "%d", ivalue);
  }
  *length= strlen(svalue);
  return (char *)svalue;
}

my_bool memc_behavior_get_init(__attribute__ ((unused)) UDF_INIT *initid,
                                       UDF_ARGS *args,
                                       char *message)
{
  return memc_servers_behavior_get_init(initid, args, message);
}

char *memc_behavior_get(__attribute__ ((unused)) UDF_INIT *initid,
                                    UDF_ARGS *args,
                                    __attribute__ ((unused)) char *result,
                                    unsigned long *length,
                                    __attribute__ ((unused)) char *is_null,
                                    __attribute__ ((unused)) char *error)
{

  return memc_servers_behavior_get(initid, args, result, length, is_null, error);
}

my_bool memc_behavior_set_init(__attribute__ ((unused)) UDF_INIT *initid,
                                       UDF_ARGS *args,
                                       char *message)
{
  return memc_servers_behavior_set_init(initid, args, message);

}

long long memc_behavior_set(__attribute__ ((unused)) UDF_INIT *initid,
                                    UDF_ARGS *args,
                                    __attribute__ ((unused)) char *is_null,
                                    __attribute__ ((unused)) char *error)
{
  return memc_servers_behavior_set(initid, args, is_null, error);
}

#ifdef NOT_DONE
/*
  @@brief
  memc_servers_reset is a UDF that resets all of the behaviors and servers
  for memcached.
*/
char *memc_servers_reset(UDF_INIT *initid, UDF_ARGS *args, char *result,
                                unsigned long *length, char *is_null, char *error)
{
  /* Dump all of host names - what/why is this? */
  /* host_reset(&new_host_list[count-1], NULL, 0, MEMCACHED_CONNECTION_UNKNOWN); */
}
#endif

my_bool memc_list_behaviors_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
  /* this is how to fail */
  if (args->arg_count > 0)
  {
    strncpy(message, "Usage: memc_list_behaviors();", MYSQL_ERRMSG_SIZE);
    return 1;
  }

  return 0;
}

/*
  memc_list_behaviors
  this is informational so the user doesn't have to know the values by heart
*/
char *memc_list_behaviors(UDF_INIT *initid,
                          __attribute__ ((unused)) UDF_ARGS *args,
                          __attribute__ ((unused)) char *result,
                          unsigned long *length,
                          __attribute__ ((unused)) char *is_null,
                          __attribute__ ((unused)) char *error)
{
  *length= BEHAVIORS_STRING_LENGTH + 1;
  return (BEHAVIORS_STRING);
}


my_bool memc_list_hash_types_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
  /* this is how to fail */
  if (args->arg_count > 0)
  {
    strncpy(message, "Usage: memc_list_hash_types().", MYSQL_ERRMSG_SIZE);
    return 1;
  }

  return 0;
}

/*
  memc_list_hash_types
  this is informational so the user doesn't have to know the values by heart
*/
char *memc_list_hash_types(UDF_INIT *initid,
                          __attribute__ ((unused)) UDF_ARGS *args,
                          __attribute__ ((unused)) char *result,
                          unsigned long *length,
                          __attribute__ ((unused)) char *is_null,
                          __attribute__ ((unused)) char *error)
{
  *length= HASH_TYPES_STRING_LENGTH + 1;
  return (HASH_TYPES_STRING);
}

my_bool memc_list_distribution_types_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
  /* this is how to fail */
  if (args->arg_count > 0)
  {
    strncpy(message, "Usage: memc_list_distribution_types().", MYSQL_ERRMSG_SIZE);
    return 1;
  }

  return 0;
}



/*
  memc_list_distribution_types
  this is informational so the user doesn't have to know the values by heart
*/
char *memc_list_distribution_types(UDF_INIT *initid,
                          __attribute__ ((unused)) UDF_ARGS *args,
                          __attribute__ ((unused)) char *result,
                          unsigned long *length,
                          __attribute__ ((unused)) char *is_null,
                          __attribute__ ((unused)) char *error)
{
  *length= DISTRIBUTION_TYPES_STRING_LENGTH + 1;
  return (DISTRIBUTION_TYPES_STRING);
}


my_bool memc_libmemcached_version_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
  /* this is how to fail */
  if (args->arg_count > 0)
  {
    strncpy(message, "Usage: memc_libmemcached_version().", MYSQL_ERRMSG_SIZE);
    return 1;
  }

  return 0;
}


char *memc_libmemcached_version(UDF_INIT *initid,
                          __attribute__ ((unused)) UDF_ARGS *args,
                          __attribute__ ((unused)) char *result,
                          unsigned long *length,
                          __attribute__ ((unused)) char *is_null,
                          __attribute__ ((unused)) char *error)
{
  static char lib_version[10];
  strcpy(lib_version, memcached_lib_version());
  *length= strlen(lib_version) + 1;
  return lib_version;
}
/*
  @@brief
  memc_get_servers() take a memc structure and clones the master structures into it.
  Each UDF calls this to gain access to the information it needs to speak
  with memcached.
*/
int memc_get_servers(memcached_st *clone)
{
  int retval;
  memcached_st *test;

  pthread_mutex_lock(&memc_servers_mutex);
  test= memcached_clone(clone, master_memc);
  pthread_mutex_unlock(&memc_servers_mutex);
  retval= test ? 1 : 0;

  return retval ;
}


