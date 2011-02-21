/* Copyright (C) 2008 Patrick Galbraith, Brian Aker

  See COPYING file found with distribution for license.

*/

#include <mysql.h>
#include <string.h>

#include <stdio.h>

#include "common.h"

char* memc_error_msg(memcached_udf_function func);

memc_function_st *prepare_args(UDF_ARGS *args,
                               char *message, int func,
                               uint min_args, uint max_args);

char* memc_error_msg(memcached_udf_function func)
{
  switch (func) {
  case MEMC_GET:
    return "memc_get('<key>').";
  case MEMC_GET_BY_KEY:
    return "memc_get_by_key('<master key>', '<key>')";
  case MEMC_SET:
    return "memc_set(<key>, <pair>, <optional: expiration>).";
  case MEMC_SET_BY_KEY:
    return "memc_set_by_key(<master key>, <key>, <value>, <optional: expiration>).";
  case MEMC_ADD:
    return "memc_add(<key>, <pair>, <optional: expiration>).";
  case MEMC_ADD_BY_KEY:
    return "memc_add_by_key(<master key>, <key>, <value>, <optional: expiration>).";
  case MEMC_REPLACE:
    return "memc_replace('<key>', '<pair>', <optional: expiration>).";
  case MEMC_REPLACE_BY_KEY:
    return "memc_replace_by_key('<master key>', '<key>', '<pair>', <optional: expiration>).";
  case MEMC_DELETE:
    return "memc_delete('<key>', <optional: expiration>).";
  case MEMC_DELETE_BY_KEY:
    return "memc_delete_by_key('<master key>', '<key>', <optional: expiration>).";
  case MEMC_APPEND:
    return "memcached_append('<key>', '<value>', <optional: expiration>).";
  case MEMC_APPEND_BY_KEY:
    return "memcached_append_by_key('<master key>', '<key>', '<value>', <optional: expiration>).";
  case MEMC_CAS:
    return "memc_cas('<key>', '<value>', '<cas>', <optional: expiration>).";
  case MEMC_CAS_BY_KEY:
    return "memc_cas_by_key('<master key>', '<key>', '<value>', '<cas>', <optional: expiration>)";
  case MEMC_PREPEND:
    return "memc_prepend('<key>', '<value>', <optional: expiration>).";

  case MEMC_PREPEND_BY_KEY:
    return "memcached_prepend_by_key('<master_key>', '<key>', '<value>', <optional:expiration>)";
  case MEMC_STATS:
    return "memc_stats(<servers>)";
  case MEMC_STATS_GET_KEYS:
    return "memc_stat_get_keys()";
  case MEMC_STATS_GET_VALUE:
    return "memc_stat_get_value('<stat name>', '<server>')";
  case MEMC_SERVERS_SET:
    return "memc_servers_set('<server list>')";
  case MEMC_SERVERS_COUNT:
    return "memc_servers_count('<server list>')";
  case MEMC_SERVERS_BEHAVIOR_SET:
    return "memc_servers_behavior_set('<behavior type>', '<value>')";
  case MEMC_LIST_BEHAVIORS:
    return "memc_list_behaviors()";
  case MEMC_DECREMENT:
    return "memc_decrement('<key>', '<offset>')";
  case MEMC_INCREMENT:
    return "memc_increment('<key>', <optional: offset>)";
  case MEMC_PREFIX_SET:
    return "memc_prefix_set('<prefix>')";
  case MEMC_PREFIX_GET:
    return "memc_prefix_get()";
  }
}


memc_function_st *prepare_args(UDF_ARGS *args,
                               char *message, int func,
                               uint min_args, uint max_args)
{
  int iter;
  memc_function_st *container;

  if (args->arg_count < min_args || args->arg_count > max_args)
  {
    char msg_buf[200];
    sprintf(msg_buf, "Usage: %s",
            memc_error_msg(func), min_args);
    fprintf(stderr, "Usage: %s",
            memc_error_msg(func), min_args);

    strncpy(message, msg_buf, MYSQL_ERRMSG_SIZE);
    return (memc_function_st*)NULL;
  }
  container= calloc(1, sizeof(memc_function_st));

  fprintf(stderr, "min args %d max args %d", min_args, max_args);
  /* no need to do anything else */
  if (min_args == 0 && max_args == 0)
    return container;

  /*
    functions like append, prepend, increment, decrement don't
    have expiration arg
  */


  for (iter= 0; iter < min_args; iter++)
    args->arg_type[iter]= STRING_RESULT;

  switch(func) {
	case MEMC_SET:
	case MEMC_SET_BY_KEY:
	case MEMC_ADD:
	case MEMC_ADD_BY_KEY:
	case MEMC_REPLACE:
	case MEMC_REPLACE_BY_KEY:
	case MEMC_APPEND:
	case MEMC_APPEND_BY_KEY:
	case MEMC_PREPEND:
	case MEMC_PREPEND_BY_KEY:
	case MEMC_DELETE:
	case MEMC_DELETE_BY_KEY:
    {
	  if (args->arg_count == max_args)
	  {
		  if (args->arg_type[max_args - 1] == STRING_RESULT)
		  {
			  container->expiration= (time_t)atoi(args->args[max_args -1]);
		  }
		  else if (args->arg_type[max_args - 1] == INT_RESULT)
		  {
			  container->expiration= *((time_t*)args->args[max_args -1]);
		  }
		  else
		  {
			  container->expiration= (time_t)0;
		  }

	  }
	  else
	  {
		  container->expiration= (time_t) 0;
	  }
	  fprintf(stderr, "expiration %d\n", (int) container->expiration);
	}
	break;

	case MEMC_INCREMENT:
	case MEMC_DECREMENT:
  {
    if (args->arg_count == max_args)
      args->arg_type[max_args - 1] = STRING_RESULT;
  }
  break;

	default:
		break;
  }

  fprintf(stderr, "prepare_args finished\n");
  return container;
}

my_string_st *string_create(size_t string_size)
{
    my_string_st *str_st = malloc(sizeof(my_string_st));
    str_st->string = NULL;
    str_st->length= 0;
    str_st->string = malloc(string_size);
    str_st->current_size = string_size;
    str_st->is_allocated = 1;
    str_st->end = str_st->string;
    return str_st;
}

int string_append(my_string_st *str_st, char *from_string)
{
        size_t str_len= strlen(from_string);
        size_t tmp_len= str_st->end - str_st->string + str_len;
        char *tmp_str= NULL;

        if (tmp_len > str_st->current_size)
        {
            tmp_str= realloc((void *)str_st->string, tmp_len);

            if (tmp_str == NULL)
            {
                perror("error allocating string");
                exit(1);
            }
            str_st->string= tmp_str;
            /*
              still have not appended, so end is still start + current.
              Realloc could have changed address
            */
            str_st->end= str_st->string + str_st->length;
            /* though, it is needed to be known the new size */
            str_st->current_size= tmp_len;

        }
        memcpy(str_st->end, from_string, str_len);
        str_st->end += str_len;
        str_st->length += str_len;
        /*printf("string size: %d start: %x end: %x content: %s\n",
          str_st->current_size, str_st->string, str_st->end, str_st->string);*/
}

void free_string (my_string_st *str_st)
{
    free(str_st->string);
    free(str_st);
}

void string_reset(my_string_st *str_st)
{
    bzero(str_st->string, str_st->current_size);
    str_st->length= 0;
    str_st->end = str_st->string;
}




