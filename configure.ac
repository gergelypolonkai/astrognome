AC_INIT([gradix], [0.1], [gergely@polonkai.eu])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_ARG_ENABLE(debug, [AC_HELP_STRING([--enable-debug], [compile with debugging support. Be warned that debugging support can eat a large amount of CPU when many clients are connected. Also, debug logs can become very large!])], , enable_debug=no)

if test "x$enable_debug" = "xyes" ; then
	AC_DEFINE([DEBUG], [1], [Define if debugging is enabled.])
fi

AC_PROG_CC
AM_PROG_AR
LT_INIT
AC_PROG_LIBTOOL
PKG_PROG_PKG_CONFIG
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
	Makefile
	swe/src/Makefile
	src/Makefile
])
AC_OUTPUT
