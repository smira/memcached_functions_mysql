/* Copyright (C) 2008 Patrick Galbraith, Brian Aker

  See COPYING file found with distribution for license.

*/

#include <mysql.h>
#include <string.h>

#include <stdio.h>

#include "common.h"

my_bool memc_set_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
long long memc_set(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error);
void memc_set_deinit(UDF_INIT *initid);
my_bool memc_set_by_key_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
long long memc_set_by_key(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error);
void memc_set_by_key_deinit(UDF_INIT *initid);
my_bool memc_cas_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
long long memc_cas(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error);
void memc_cas_deinit(UDF_INIT *initid);
my_bool memc_cas_by_key_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
long long memc_cas_by_key(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error);
void memc_cas_by_key_deinit(UDF_INIT *initid);

my_bool memc_set_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
  memcached_return rc;
  unsigned int count;
  memc_function_st *container;

  container= prepare_args(args, message, MEMC_SET, 2, 3);
  if (container == NULL)
    return 1;

  /* Init the memcached_st we will use for this pass */
  rc= memc_get_servers(&container->memc);
  count= memcached_server_count(&container->memc);

  initid->ptr= (char *)container;

  return 0;
}

long long memc_set(UDF_INIT *initid, UDF_ARGS *args,
                   char *is_null,
                   char *error)
{
  memcached_return rc;
  *is_null= false;

  memc_function_st *container= (memc_function_st *)initid->ptr;
  /*
    This seems like a bug I'm trying to work around, but without
    this, setting a NULL using a user-defined variable causes segfault

    set @foo=concat('a',':', NULL);

    mysql> select memc_set('a',@foo);
    ERROR 2013 (HY000): Lost connection to MySQL server during query

  */
  if (args->args[1] == NULL)
    args->lengths[1]= 0;

  rc= memcached_set(&container->memc,
                    args->args[0], (size_t)args->lengths[0],
                    args->args[1], (size_t)args->lengths[1],
                    container->expiration, (uint16_t)0);

  return (rc != MEMCACHED_SUCCESS) ? (long long) 0 : (long long) 1;
}

void memc_set_deinit(UDF_INIT *initid)
{
  /* if we allocated initid->ptr, free it here */
  memc_function_st *container= (memc_function_st *)initid->ptr;

  memcached_free(&container->memc);
  free(container);
}
my_bool memc_set_by_key_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
  memcached_return rc;
  memc_function_st *container;

  container= prepare_args(args, message, MEMC_SET_BY_KEY, 3, 4);
  if (container == NULL)
    return 1;

  /* Init the memcached_st we will use for this pass */
  rc= memc_get_servers(&container->memc);

  initid->ptr= (char *)container;
  fprintf(stderr, "1: container->expiration %d", container->expiration);

  return 0;
}

long long memc_set_by_key(UDF_INIT *initid, UDF_ARGS *args,
                   __attribute__ ((unused)) char *is_null,
                   char *error)
{
  memcached_return rc;
  memc_function_st *container= (memc_function_st *)initid->ptr;

  /*
    This seems like a bug I'm trying to work around, but without
    this, setting a NULL using a user-defined variable causes segfault

    set @foo=concat('a',':', NULL);

    mysql> select memc_set('a',@foo);
    ERROR 2013 (HY000): Lost connection to MySQL server during query

  */
  if (args->args[2] == NULL)
    args->lengths[2]= 0;

  rc= memcached_set_by_key(&container->memc,
                           args->args[0], (size_t)args->lengths[0],
                           args->args[1], (size_t)args->lengths[1],
                           args->args[2], (size_t)args->lengths[2],
                           container->expiration, (uint16_t)0);

  return (rc != MEMCACHED_SUCCESS) ? (long long) 0 : (long long) 1;

}

void memc_set_by_key_deinit(UDF_INIT *initid)
{
  /* if we allocated initid->ptr, free it here */
  memc_function_st *container= (memc_function_st *)initid->ptr;

  memcached_free(&container->memc);
  free(container);
}

my_bool memc_cas_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
  memcached_return rc;
  memc_function_st *container;

  container= prepare_args(args, message, MEMC_CAS, 3, 4);
  if (container == NULL)
    return 1;

  /* Init the memcached_st we will use for this pass */
  rc= memc_get_servers(&container->memc);

  initid->ptr= (char *)container;

  return 0;
}

long long memc_cas(UDF_INIT *initid, UDF_ARGS *args,
                   __attribute__ ((unused)) char *is_null,
                   char *error)
{
  memcached_return rc;
  memc_function_st *container= (memc_function_st *)initid->ptr;

  if (args->args[1] == NULL)
    args->lengths[1]= 0;

  rc= memcached_cas(&container->memc,
                    args->args[0], (size_t)args->lengths[0],
                    args->args[1], (size_t)args->lengths[1],
                    container->expiration, (uint16_t)0,
                    (uint64_t) strtol(args->args[2], (char **)NULL, 10));


  return (rc != MEMCACHED_SUCCESS) ? (long long) 0 : (long long) 1;
}

void memc_cas_deinit(UDF_INIT *initid)
{
  /* if we allocated initid->ptr, free it here */
  memc_function_st *container= (memc_function_st *)initid->ptr;

  memcached_free(&container->memc);
  free(container);
}
my_bool memc_cas_by_key_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
  memcached_return rc;
  memc_function_st *container;

  container= prepare_args(args, message, MEMC_CAS_BY_KEY, 4, 5);
  if (container == NULL)
    return 1;

  /* Init the memcached_st we will use for this pass */
  rc= memc_get_servers(&container->memc);

  initid->ptr= (char *)container;

  return 0;
}

long long memc_cas_by_key(UDF_INIT *initid, UDF_ARGS *args,
                   __attribute__ ((unused)) char *is_null,
                   char *error)
{
  memcached_return rc;
  memc_function_st *container= (memc_function_st *)initid->ptr;

  if (args->args[2] == NULL)
    args->lengths[2]= 0;

  rc= memcached_cas_by_key(&container->memc,
                    args->args[0], (size_t)args->lengths[0],
                    args->args[1], (size_t)args->lengths[1],
                    args->args[2], (size_t)args->lengths[2],
                    container->expiration, (uint16_t)0,
                    (uint64_t) strtol(args->args[3], (char **)NULL, 10));

  return (rc != MEMCACHED_SUCCESS) ? (long long) 0 : (long long) 1;
}

void memc_cas_by_key_deinit(UDF_INIT *initid)
{
  /* if we allocated initid->ptr, free it here */
  memc_function_st *container= (memc_function_st *)initid->ptr;

  memcached_free(&container->memc);
  free(container);
}
