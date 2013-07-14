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
AC_CHECK_LIB([m], [sincos])
AC_CHECK_LIB([m], [atan2])
AC_CHECK_LIB([m], [fmod])
AC_CHECK_LIB([m], [acos])
AC_CHECK_LIB([m], [sin])
AC_CHECK_LIB([m], [atan])
AC_CHECK_LIB([m], [asin])
AC_CHECK_LIB([m], [exp])
AC_CHECK_LIB([m], [trunc])
AC_CHECK_LIB([m], [tan])
AC_CHECK_LIB([m], [cos])
AC_CHECK_LIB([m], [log])
AC_CHECK_LIB([m], [pow])
AC_CHECK_LIB([m], [log10])
AC_CHECK_LIB([m], [sqrt])
AC_CHECK_LIB([m], [floor])
PKG_PROG_PKG_CONFIG
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
	Makefile
	swe/src/Makefile
	src/Makefile
])
AC_OUTPUT
