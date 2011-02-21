/* Copyright (C) 2009 Patrick Galbraith

  See COPYING file found with distribution for license.

*/

#include <mysql.h>
#include <string.h>

#include <stdio.h>

#include <libmemcached/memcached.h>
#include "common.h"

my_bool memc_get_cas_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
long long memc_get_cas(UDF_INIT *initid, UDF_ARGS *args,
                      __attribute__ ((unused)) char *is_null,
                      __attribute__ ((unused)) char *error);
void memc_get_cas_deinit(UDF_INIT *initid);

my_bool memc_get_cas_by_key_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
long long memc_get_casby_key_(UDF_INIT *initid, UDF_ARGS *args,
                             __attribute__ ((unused)) char *is_null,
                             __attribute__ ((unused)) char *error);

void memc_get_cas_by_key_deinit(UDF_INIT *initid);

my_bool memc_get_cas_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
  memcached_return rc;
  memc_function_st *container;


  /* this is how to fail */
  if (args->arg_count != 1)
  {
    strncpy(message,
            "one argument must be supplied: memc_get_cas('<key>').",
            MYSQL_ERRMSG_SIZE);
    return 1;
  }

  args->arg_type[0]= STRING_RESULT;

  initid->max_length= MEMC_UDF_MAX_SIZE;
  container= calloc(1, sizeof(memc_function_st));

  /* Init the memcached_st we will use for this pass */
  rc= memc_get_servers(&container->memc);

  /* make this available to pass to value function  */
  initid->ptr= (char *)container;

  return 0;
}


/*
  memc_get_cas
  get cached item's cas value, takes hash-key arg
*/
long long memc_get_cas(UDF_INIT *initid, UDF_ARGS *args,
                char *is_null,
                __attribute__ ((unused)) char *error)
{
  memcached_return rc;
  uint64_t cas = 0;
  unsigned long length;
  memcached_result_st *results;

  memc_function_st *container= (memc_function_st *)initid->ptr;

  /* get the item */
  rc= memcached_mget(&container->memc, (const char **) args->args, (size_t *)args->lengths, 1);

  /* create a result */
  results= memcached_result_create(&container->memc, &container->results);

  /* fetch the result */
  memcached_fetch_result(&container->memc, results, &rc);

  /* obtain length so we have something to check */
  length= memcached_result_length(results);
  if (! length)
  {
     /* function to return null if no value */
    *is_null= 1;
    return 0;
  }

  /* we now have the cas value */
  cas = memcached_result_cas(results);

  /* free the result */
  //memcached_result_free(results);

  /* return the cas value */
  return (long long) cas;
}

/* de-init UDF */
void memc_get_cas_deinit(UDF_INIT *initid)
{
  /* if we allocated initid->ptr, free it here */
  memc_function_st *container= (memc_function_st *)initid->ptr;

  memcached_result_free(&container->results);
  free(container);

  return;
}

my_bool memc_get_cas_by_key_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
  memcached_return rc;
  memc_function_st *container;

  /* error if not 2 args */
  if (args->arg_count != 2)
  {
    strncpy(message,
            "2 arguments must be supplied: memc_get_by_key('<master key>', '<key>')",
            MYSQL_ERRMSG_SIZE);
    return 1;
  }

  args->arg_type[0]= args->arg_type[1]= STRING_RESULT;

  initid->max_length= MEMC_UDF_MAX_SIZE;
  container= calloc(1,sizeof(memc_function_st));

  /* Init the memcached_st we will use for this pass */
  rc= memc_get_servers(&container->memc);

  /* make this available to pass to value function  */
  initid->ptr= (char *)container;

  return 0;
}

/*
  memc_get_cas_by_key
  get cas value of cached item, takes hash-key arg
*/
long long memc_get_cas_by_key(UDF_INIT *initid, UDF_ARGS *args,
                      __attribute__ ((unused)) char *is_null,
                      __attribute__ ((unused)) char *error)
{
  memcached_return rc;
  uint64_t cas = 0;
  char **keys;
  size_t *lengths;
  size_t length;
  keys= args->args;
  keys++;
  lengths= (size_t*)args->lengths;
  lengths++;
  memcached_result_st *results;

  memc_function_st *container= (memc_function_st *)initid->ptr;

  /* get the item */
  rc= memcached_mget_by_key(&container->memc,
                            args->args[0],
                            (size_t )args->lengths[0],
                            (const char **) keys,
                            lengths,
                            args->arg_count - 1);
  /* create a result */
  results= memcached_result_create(&container->memc, &container->results);

  /* fetch the result */
  memcached_fetch_result(&container->memc, results, &rc);

  /* obtain length so we have something to check */
  length= memcached_result_length(results);
  if (! length)
  {
     /* function to return null if no value */
    *is_null= 1;
    return 0;
  }

  /* we now have the cas value */
  cas = memcached_result_cas(results);

  /* free the result */
  //memcached_result_free(results);

  /* return the cas value */
  return (long long) cas;
}

/* de-init UDF */
void memc_get_cas_by_key_deinit(UDF_INIT *initid)
{
  /* if we allocated initid->ptr, free it here */
  memc_function_st *container= (memc_function_st *)initid->ptr;

  //memcached_result_free(&container->results);
  free(container);

  return;
}
