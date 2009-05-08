/* Copyright (C) 2008 Patrick Galbraith, Brian Aker

  See COPYING file found with distribution for license.

*/

#include <mysql.h>
#include <string.h>

#include <stdio.h>

#include "common.h"

my_bool memc_increment_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
long long memc_increment(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error);
void memc_increment_deinit(UDF_INIT *initid);

my_bool memc_increment_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
  unsigned int x;
  memcached_return rc;
  memc_function_st *container;

  container= prepare_args(args, message, MEMC_INCREMENT, 1, 2);
  if (container == NULL)
    return 1;

  /* Init the memcached_st we will use for this pass */
  rc= memc_get_servers(&container->memc);

  initid->ptr= (char *)container;

  return 0;
}

long long memc_increment(UDF_INIT *initid, UDF_ARGS *args,
                          __attribute__ ((unused)) char *is_null,
                           __attribute__ ((unused)) char *error)
{
  memcached_return rc;
  uint64_t value;
  memc_function_st *container= (memc_function_st *)initid->ptr;
  container->offset= args->arg_count == 2 ? (unsigned int)atoi(args->args[1]) : 1;

  rc= memcached_increment(&container->memc,
                    args->args[0], (size_t)args->lengths[0],
                    container->offset,
                    &value);

  return ((long long)value);
}

void memc_increment_deinit(UDF_INIT *initid)
{
  /* if we allocated initid->ptr, free it here */
  memc_function_st *container= (memc_function_st *)initid->ptr;

  memcached_free(&container->memc);
  free(container);
}
