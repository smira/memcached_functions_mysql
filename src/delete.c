/* Copyright (C) 2008 Patrick Galbraith, Brian Aker

  See COPYING file found with distribution for license.

*/


#include <mysql.h>
#include <string.h>

#include <stdio.h>

#include "common.h"

my_bool memc_delete_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
long long memc_delete(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error);
void memc_delete_deinit(UDF_INIT *initid);
my_bool memc_delete_by_key_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
long long memc_delete_by_key(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error);
void memc_delete_by_key_deinit(UDF_INIT *initid);

my_bool memc_delete_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
  memcached_return rc;
  memc_function_st *container;

  container= prepare_args(args, message, MEMC_DELETE, 1, 2);
  if (container == NULL)
    return 1;

  /* Init the memcached_st we will use for this pass */
  rc= memc_get_servers(&container->memc);

  initid->ptr= (char *)container;

  return 0;
}

long long memc_delete(UDF_INIT *initid,
                      UDF_ARGS *args,
                      __attribute__ ((unused)) char *is_null,
                      char *error)
{
  memcached_return rc;
  memc_function_st *container= (memc_function_st *)initid->ptr;

  rc= memcached_delete(&container->memc,
                       args->args[0], (size_t)args->lengths[0],
                       container->expiration);

  return (rc != MEMCACHED_SUCCESS) ? (long long) 0 : (long long) 1;
}

void memc_delete_deinit(UDF_INIT *initid)
{
  /* if we allocated initid->ptr, free it here */
  memc_function_st *container= (memc_function_st *)initid->ptr;

  memcached_free(&container->memc);
  free(container);
}

my_bool memc_delete_by_key_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
  unsigned int x;
  memcached_return rc;
  memc_function_st *container;

  container= prepare_args(args, message, MEMC_DELETE_BY_KEY, 2, 3);
  if (container == NULL)
    return 1;

  /* Init the memcached_st we will use for this pass */
  rc= memc_get_servers(&container->memc);

  initid->ptr= (char *)container;

  return 0;
}

long long memc_delete_by_key(UDF_INIT *initid,
                      UDF_ARGS *args,
                      __attribute__ ((unused)) char *is_null,
                       char *error)
{
  memcached_return rc;
  memc_function_st *container= (memc_function_st *)initid->ptr;

  rc= memcached_delete_by_key(&container->memc,
                       args->args[0], (size_t)args->lengths[0],
                       args->args[1], (size_t)args->lengths[1],
                       container->expiration);

  return (rc != MEMCACHED_SUCCESS) ? (long long) 0 : (long long) 1;
}

void memc_delete_by_key_deinit(UDF_INIT *initid)
{
  /* if we allocated initid->ptr, free it here */
  memc_function_st *container= (memc_function_st *)initid->ptr;

  memcached_free(&container->memc);
  free(container);
}
