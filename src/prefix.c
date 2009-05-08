/* Copyright (C) 2008 Patrick Galbraith, Brian Aker

  See COPYING file found with distribution for license.

*/

#include <mysql.h>
#include <string.h>
#include <pthread.h>

#include <stdio.h>
#include <assert.h>

#include "common.h"


my_bool memc_prefix_set_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
long long memc_prefix_set(UDF_INIT *initid,
                          UDF_ARGS *args,
                          char *result,
                          unsigned long *length,
                          char *is_null,
                          char *error);

my_bool memc_prefix_get_init(UDF_INIT *initid, UDF_ARGS *args, char *message);

char *memc_prefix_get(__attribute__ ((unused)) UDF_INIT *initid,
                          UDF_ARGS *args,
                          __attribute__ ((unused)) char *is_null,
                          char *error);

my_bool memc_prefix_set_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
  memcached_return rc;
  memc_function_st *container;

  container= prepare_args(args, message, MEMC_PREFIX_SET, 1, 1);
  if (container == NULL)
    return 1;

  /* Init the memcached_st we will use for this pass */
  rc= memc_get_servers(&container->memc);


  initid->ptr= (char *)container;

  return 0;

}

/*
  memc_prefix_set
  sets key prefix
*/
long long memc_prefix_set(__attribute__ ((unused)) UDF_INIT *initid,
                       __attribute__ ((unused)) UDF_ARGS *args,
                       __attribute__ ((unused)) char *result,
                       __attribute__ ((unused)) unsigned long *length,
                       __attribute__ ((unused)) char *is_null,
                       char *error)
{

  memcached_return rc;
  memc_function_st *container= (memc_function_st *)initid->ptr;

  rc= memcached_callback_set(&container->memc,
                             MEMCACHED_CALLBACK_PREFIX_KEY,
                             (void *) args->args[0]);

  if (rc != MEMCACHED_SUCCESS)
  {
    sprintf(error,"Failure setting prefix\n");
    return ((long long) rc);
  }
  return ((long long) 0);
}

my_bool memc_prefix_get_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
  memcached_return rc;
  memc_function_st *container;

  container= prepare_args(args, message, MEMC_PREFIX_GET, 0, 0);
  if (container == NULL)
    return 1;

  /* Init the memcached_st we will use for this pass */
  rc= memc_get_servers(&container->memc);

  initid->ptr= (char *)container;

  return 0;
}
/*
  memc_prefix_get - obtaincurrent prefix
*/
char *memc_prefix_get(__attribute__ ((unused)) UDF_INIT *initid,
                          UDF_ARGS *args,
                          __attribute__ ((unused)) char *is_null,
                          char *error)
{
  memcached_return rc;
  char *prefix;
  memc_function_st *container= (memc_function_st *)initid->ptr;

  prefix= memcached_callback_get(&container->memc,
                             MEMCACHED_CALLBACK_PREFIX_KEY,
                             &rc);

  if (rc != MEMCACHED_SUCCESS)
  {
    sprintf(error,"Failure getting prefix\n");
    return ((char *)NULL);
  }

  return (prefix);

}
