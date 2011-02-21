EXTRA_DIST = common.h
INCLUDES = -I$(top_builddir)/include $(MYSQL_INC)

lib_LTLIBRARIES = libmemcached_functions_mysql.la
libmemcached_functions_mysql_la_SOURCES = util.c add.c get.c set.c prepend.c replace.c increment.c decrement.c append.c delete.c servers.c version.c stats.c get_cas.c
libmemcached_functions_mysql_la_LDFLAGS = -module
# had to add this back
libmemcached_functions_mysql_la_LIBADD = $(DEPS_LIBS)

