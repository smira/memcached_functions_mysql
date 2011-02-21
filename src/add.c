/* Copyright (C) 2008 Patrick Galbraith, Brian Aker

  See COPYING file found with distribution for license.

*/

#include <mysql.h>
#include <string.h>

#include <stdio.h>

#include "common.h"

my_bool memc_add_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
long long memc_add(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error);
void memc_add_deinit(UDF_INIT *initid);
my_bool memc_add_by_key_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
long long memc_add_by_key(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error);
void memc_add_by_key_deinit(UDF_INIT *initid);

my_bool memc_add_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
  unsigned int x;
  memcached_return rc;
  memc_function_st *container;

  container= prepare_args(args, message, MEMC_ADD, 2, 3);
  if (container == NULL)
    return 1;

  /* Init the memcached_st we will use for this pass */
  rc= memc_get_servers(&container->memc);

  initid->ptr= (char *)container;

  return 0;
}

long long memc_add(UDF_INIT *initid, UDF_ARGS *args,
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
  if (args->args[1] == NULL)
    args->lengths[1]= 0;

  rc= memcached_add(&container->memc,
                    args->args[0], (size_t)args->lengths[0],
                    args->args[1], (size_t)args->lengths[1],
                    container->expiration, (uint16_t)0);

  return (rc != MEMCACHED_SUCCESS) ? (long long) 0 : (long long) 1;

}

void memc_add_deinit(UDF_INIT *initid)
{
  /* if we allocated initid->ptr, free it here */
  memc_function_st *container= (memc_function_st *)initid->ptr;

  memcached_free(&container->memc);
  free(container);
}
my_bool memc_add_by_key_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
  unsigned int x;
  memcached_return rc;
  memc_function_st *container;

  container= prepare_args(args, message, MEMC_ADD_BY_KEY, 3, 4);
  if (container == NULL)
    return 1;

  /* Init the memcached_st we will use for this pass */
  rc= memc_get_servers(&container->memc);

  initid->ptr= (char *)container;

  return 0;
}

long long memc_add_by_key(UDF_INIT *initid, UDF_ARGS *args,
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

  rc= memcached_add_by_key(&container->memc,
                           args->args[0], (size_t)args->lengths[0],
                           args->args[1], (size_t)args->lengths[1],
                           args->args[2], (size_t)args->lengths[2],
                           container->expiration, (uint16_t)0);

  return (rc != MEMCACHED_SUCCESS) ? (long long) 0 : (long long) 1;
}

void memc_add_by_key_deinit(UDF_INIT *initid)
{
  /* if we allocated initid->ptr, free it here */
  memc_function_st *container= (memc_function_st *)initid->ptr;

  memcached_free(&container->memc);
  free(container);
}
