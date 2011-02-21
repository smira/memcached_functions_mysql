#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

typedef struct {
    char *end;
    char *string;
    size_t current_size;
    size_t length;
    /*struct {
        bool is_allocated;
        bool is_initialized;
    } options;
    */
    int is_initialized;
    int is_allocated;
} my_string_st;

my_string_st *string_create(size_t string_size);
int string_append(my_string_st *str_st, char *from_string);
void free_string (my_string_st *str_st);
void string_reset(my_string_st *str_st);
