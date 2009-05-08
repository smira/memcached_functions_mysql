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



my_bool memc_udf_version_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
char *memc_udf_version(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error);

my_bool memc_udf_version_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
  char *version_string;
  /* this is how to fail */
  if (args->arg_count > 0)
  {
    strncpy(message, "This function requires no arguments: memc_udf_version()", MYSQL_ERRMSG_SIZE);
    return 1;
  }

  version_string= calloc(1, VERSION_STRING_LENGTH + 1);
  strncpy(version_string, VERSION_STRING, VERSION_STRING_LENGTH);
  initid->ptr= version_string;

  return 0;
}

/*
  memc_servers_set
  get cached object, takes hash-key arg
*/
char *memc_udf_version(__attribute__ ((unused)) UDF_INIT *initid,
                       __attribute__ ((unused)) UDF_ARGS *args,
                       __attribute__ ((unused)) char *result,
                       __attribute__ ((unused)) unsigned long *length,
                       __attribute__ ((unused)) char *is_null,
                       __attribute__ ((unused)) char *error)
{
  char *version_string= initid->ptr;
  *length= VERSION_STRING_LENGTH;
  return (version_string);
}
