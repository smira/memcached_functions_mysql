/* Copyright (C) 2008 Patrick Galbraith, Brian Aker

  See COPYING file found with distribution for license.

*/

#include <mysql.h>
#include <string.h>

#include <stdio.h>

#include <libmemcached/memcached.h>
#include "common.h"

my_bool memc_stats_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
char *memc_stats(UDF_INIT *initid, UDF_ARGS *args,
                 char *result,
                 unsigned long *length,
                 char *is_null,
                 char *error);
void memc_stats_deinit(UDF_INIT *initid);
my_bool memc_stat_get_keys_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
char *memc_stat_get_keys(UDF_INIT *initid, UDF_ARGS *args,
                         char *result,
                         unsigned long *length,
                         char *is_null, char *error);
void memc_stat_get_keys_deinit(UDF_INIT *initid);
my_bool memc_stat_get_value_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
char *memc_stat_get_value(UDF_INIT *initid, UDF_ARGS *args,
                          __attribute__ ((unused)) char *result,
                          unsigned long *length,
                          __attribute__ ((unused)) char *is_null,
                          char *error);
void memc_stat_get_value_deinit(UDF_INIT *initid);

my_bool memc_stats_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
  unsigned int x;
  memcached_return rc;
  memc_function_st *container;

  /* this is how to fail */
  if (args->arg_count != 1)
  {
    strncpy(message,
            "1 argument required: servers, comma-separated: memc_stats(<servers>)",
            MYSQL_ERRMSG_SIZE);
    return 1;
  }

  args->arg_type[0]= STRING_RESULT;

  initid->max_length= MEMC_UDF_MAX_SIZE;
  container= calloc(1, sizeof(memc_function_st));

  /* Init the memcached_st we will use for this pass */
  rc= memc_get_servers(&container->memc);

  /* Now setup the string */
  container->stats_string= string_create(1024);

  initid->ptr= (char *)container;

  return 0;
}

char *memc_stats(UDF_INIT *initid, UDF_ARGS *args,
                __attribute__ ((unused)) char *result,
               unsigned long *length,
                __attribute__ ((unused)) char *is_null,
                __attribute__ ((unused)) char *error)
{
  /* how do I utilise this? Print out in case of error? */
  /* We'll just hard-code now? */
  unsigned int x;
  memcached_return rc;
  char buf[100];

  memcached_stat_st *stat;
  memcached_server_st *servers;
  /*memcached_server_st *server_list;*/

  memc_function_st *container= (memc_function_st *)initid->ptr;
  string_reset(container->stats_string);

  servers= memcached_servers_parse(args->args[0]);
  memcached_server_push(&container->memc, servers);
  memcached_server_list_free(servers);

  stat= memcached_stat(&container->memc, NULL, &rc);

  if (rc != MEMCACHED_SUCCESS && rc != MEMCACHED_SOME_ERRORS)
  {
    sprintf(error, "Failure to communicate with servers (%s)\n",
	   memcached_strerror(&container->memc, rc));
    *length= strlen(error);
    return(error);
  }

  /*server_list= memcached_server_list(&container->memc);*/

  sprintf(buf, "Listing %u Server\n\n", memcached_server_count(&container->memc));
  string_append(container->stats_string, buf);
  for (x= 0; x < memcached_server_count(&container->memc); x++)
  {
    char **list;
    char **ptr;
    memcached_server_instance_st instance=
        memcached_server_instance_by_position(&container->memc, x);

    list= memcached_stat_get_keys(&container->memc, &stat[x], &rc);

    sprintf(buf, "Server: %s (%u)\n",
            memcached_server_name(instance),
            memcached_server_port(instance));


    string_append(container->stats_string, buf);

    for (ptr= list; *ptr; ptr++)
    {
      memcached_return rc;
      char *value= memcached_stat_get_value(&container->memc, &stat[x], *ptr, &rc);

      sprintf(buf, "\t %s: %s\n", *ptr, value);
      free(value);
      string_append(container->stats_string, buf);
    }

    free(list);
    string_append(container->stats_string,"\n");
  }
  *length= container->stats_string->length;
  free(stat);
  return container->stats_string->string;
}

void memc_stats_deinit(UDF_INIT *initid)
{
  /* if we allocated initid->ptr, free it here */
  memc_function_st *container= (memc_function_st *)initid->ptr;

  free_string(container->stats_string);
  memcached_free(&container->memc);
  free(container);

  return;
}


my_bool memc_stat_get_value_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
  char **list;
  char **ptr;
  memcached_return rc;
  int exists= 0;
  unsigned int x;
  memc_function_st *container;
  memcached_stat_st *stat;
  memcached_server_st *servers;


  /* this is how to fail */
  if (args->arg_count != 2)
  {
    strncpy(message,
            "two arguments must be supplied: memc_stat_get_value('<server>', '<stat name>')",
            MYSQL_ERRMSG_SIZE);
    return 1;
  }

  initid->max_length= MEMC_UDF_MAX_SIZE;
  container= calloc(1, sizeof(memc_function_st));

  /* Init the memcached_st we will use for this pass */
  rc= memc_get_servers(&container->memc);
  servers= memcached_servers_parse(args->args[0]);
  memcached_server_push(&container->memc, servers);
  memcached_server_list_free(servers);

  stat= memcached_stat(&container->memc, NULL, &rc);

  list= memcached_stat_get_keys(&container->memc, &stat[0], &rc);
  for (ptr= list; *ptr; ptr++)
  {
    if (!strcmp(args->args[1], *ptr))
    {
      exists++;
    }
  }
  if (!exists)
  {
    char err_buf[50];
    sprintf(err_buf, "ERROR: the stat key %s is not a valid stat!\n", args->args[1]);
    strncpy(message, err_buf, MYSQL_ERRMSG_SIZE);
    memcached_free(&container->memc);
    free(container);
    return 1;
  }

  initid->ptr= (char *)container;

  return 0;
}
/*
  memc_get
  get cached object, takes hash-key arg
*/
char *memc_stat_get_value(UDF_INIT *initid, UDF_ARGS *args,
                __attribute__ ((unused)) char *result,
               unsigned long *length,
                __attribute__ ((unused)) char *is_null,
                char *error)
{
  /* how do I utilise this? Print out in case of error? */
  memcached_return rc;
  char *value;
  char **list;
  char **ptr;
  char buf[100];
  int exists= 0;

  memcached_stat_st *stat;
  memcached_server_st *servers;

  memc_function_st *container= (memc_function_st *)initid->ptr;

  servers= memcached_servers_parse(args->args[0]);
  memcached_server_push(&container->memc, servers);
  memcached_server_list_free(servers);

  stat= memcached_stat(&container->memc, NULL, &rc);

  list= memcached_stat_get_keys(&container->memc, &stat[0], &rc);
  for (ptr= list; *ptr; ptr++)
  {
    if (!strcmp(args->args[1], *ptr))
    {
      exists++;
    }
  }
  if (exists)
  {
    value= memcached_stat_get_value(&container->memc, &stat[0], args->args[1], &rc);
    *length= strlen(value);
  }
  else
  {
    sprintf(error, "ERROR: the stat key %s is not a valid stat!\n", args->args[1]);
    *length=0;
    value= NULL;
  }

  return value;
}

/* de-init UDF */
void memc_stat_get_value_deinit(UDF_INIT *initid)
{
  /* if we allocated initid->ptr, free it here */
  memc_function_st *container= (memc_function_st *)initid->ptr;

  memcached_free(&container->memc);
  free(container);

  return;
}

my_bool memc_stat_get_keys_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
  unsigned int x;
  memcached_return rc;
  memc_function_st *container;


  /* this is how to fail */
  if (args->arg_count > 1)
  {
    strncpy(message, "This function takes no arguments: memc_stat_get_keys()", MYSQL_ERRMSG_SIZE);
    return 1;
  }

  initid->max_length= MEMC_UDF_MAX_SIZE;
  container= calloc(1, sizeof(memc_function_st));

  /* Init the memcached_st we will use for this pass */
  rc= memc_get_servers(&container->memc);

  /* Now setup the string */
  container->stats_string= string_create(1024);

  initid->ptr= (char *)container;

  return 0;
}

char *memc_stat_get_keys(UDF_INIT *initid, UDF_ARGS *args,
                __attribute__ ((unused)) char *result,
               unsigned long *length,
                __attribute__ ((unused)) char *is_null,
                __attribute__ ((unused)) char *error)
{
/*
  memc_stat
  get cached object, takes hash-key arg
*/
  char **list;
  char **ptr;
  char buf[100];
  memcached_stat_st stat;
  memcached_return rc;
  memc_function_st *container= (memc_function_st *)initid->ptr;

  list= memcached_stat_get_keys(&container->memc, &stat, &rc);
  for (ptr= list; *ptr; ptr++)
  {
    string_append(container->stats_string, *ptr);
    string_append(container->stats_string, "\n");
  }
  free(list);

  *length= container->stats_string->length;
  return container->stats_string->string;
}
/* de-init UDF */
void memc_stat_get_keys_deinit(UDF_INIT *initid)
{
  /* if we allocated initid->ptr, free it here */
  memc_function_st *container= (memc_function_st *)initid->ptr;

  free_string(container->stats_string);
  memcached_free(&container->memc);
  free(container);

  return;
}

