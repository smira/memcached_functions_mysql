AC_INIT(src/get.c)
AC_CONFIG_AUX_DIR(config)
AM_CONFIG_HEADER(src/libmemcached_config.h)

AM_INIT_AUTOMAKE("memcached_functions_mysql", 1.1 )

AC_PROG_CC
AC_PROG_LIBTOOL
LIBTOOL="$LIBTOOL --preserve-dup-deps"
AC_SUBST(LIBTOOL)dnl

sinclude(config/ac_mysql.m4)
MYSQL_CONFIG_TEST

AC_SUBST(MYSQL_CONFIG)
AC_SUBST(MYSQL_INC)
AC_SUBST(MYSQL_LIB)

sinclude(config/ac_libmemcached.m4)
LIBMEMCACHED_CONFIG_TEST

AC_SUBST(DEPS_CFLAGS)
AC_SUBST(DEPS_LIBS)

AC_C_CONST
AC_TYPE_SIZE_T
AC_CHECK_HEADERS(limits.h syslimits.h)
AC_OUTPUT(Makefile src/Makefile tests/Makefile docs/Makefile)
