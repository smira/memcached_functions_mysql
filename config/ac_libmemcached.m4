dnl ---------------------------------------------------------------------------
dnl Macro: LIBMEMCACHED_CONFIG_TEST
dnl ---------------------------------------------------------------------------
AC_DEFUN([LIBMEMCACHED_CONFIG_TEST], [
  LIBS="-lmemcached"
  AC_ARG_WITH(libmemcached,
  [  --with-libmemcached     Specify libmemcached location],
  [
    if test -n "$withval"
    then
      CFLAGS="-I${withval}/include $CFLAGS"
      LDFLAGS="-L${withval}/lib $LDFLAGS"

      if test "`uname -s`" = "SunOS"
      then
        LDFLAGS="$LDFLAGS -Wl,-R${withval}/lib"
      else
        LDFLAGS="$LDFLAGS -Wl,--rpath -Wl,${withval}/lib"
      fi
    fi
  ])

  AC_MSG_CHECKING([for libmemcached >= 0.17])
  AC_RUN_IFELSE([
    AC_LANG_PROGRAM([
      #include <string.h>
      #include <stdlib.h>
      #include "libmemcached/memcached.h"
    ], [
      char *dot = strrchr(LIBMEMCACHED_VERSION_STRING, '.');
      if ((atoi(LIBMEMCACHED_VERSION_STRING)) > 0 ||
          (dot != NULL && atoi(dot + 1) > 16)) {
        return 0;
      }

      return 1;
    ])
  ],, AC_MSG_ERROR([libmemcached not found]))
  AC_MSG_RESULT(yes)
])
dnl ---------------------------------------------------------------------------
dnl Macro: LIBMEMCACHED_CONFIG_TEST
dnl ---------------------------------------------------------------------------
