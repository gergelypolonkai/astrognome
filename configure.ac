AC_INIT([astrognome], [0.1], [gergely@polonkai.eu])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([-Wall foreign])
AM_SILENT_RULES([yes])
AC_ARG_ENABLE(debug, [AC_HELP_STRING([--enable-debug], [compile with debugging support. Be warned that debugging support can eat a large amount of CPU when many clients are connected. Also, debug logs can become very large!])], , enable_debug=no)

if test "x$enable_debug" = "xyes" ; then
	AC_DEFINE([DEBUG], [1], [Define if debugging is enabled.])
fi

AC_PROG_CC
AM_PROG_CC_C_O
AM_PROG_AR
AC_PROG_LIBTOOL
IT_PROG_INTLTOOL([0.35.0])
GETTEXT_PACKAGE=astrognome
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, "$GETTEXT_PACKAGE", [GETTEXT package name])
AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.17])
LT_INIT
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
GTK_DOC_CHECK([1.19], [--flavour no-tmpl])
PKG_PROG_PKG_CONFIG
PKG_CHECK_MODULES([GTK], [gtk+-3.0])
AC_CONFIG_MACRO_DIR([m4])

LIBSWE_LIBS='$(top_builddir)/swe/src/libswe-1.75.la'
AC_SUBST(LIBSWE_LIBS)

AC_CONFIG_FILES([
    Makefile
    swe/src/Makefile
    src/Makefile
    po/Makefile.in
    docs/reference/astrognome/Makefile
])
AC_OUTPUT
