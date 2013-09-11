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
AC_PATH_PROGS(PERL, [perl5 perl])
AC_PATH_PROGS(WGET, [wget])
AC_PATH_PROGS(CURL, [curl])
AC_PATH_PROGS(UNZIP, [unzip])
have_geonames_perl_modules=no
AX_PROG_PERL_MODULES([XML::Writer IO::File], [have_geonames_perl_modules=yes], AC_MSG_WARN([XML::Writer and IO::File perl modules are required if you want to regenerate geodata.xml!]))
AC_SUBST([have_geonames_perl_modules])
IT_PROG_INTLTOOL([0.35.0])
GETTEXT_PACKAGE=astrognome
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, "$GETTEXT_PACKAGE", [GETTEXT package name])
AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.17])
LT_INIT
YELP_HELP_INIT
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
AC_PATH_PROG([GLIB_MKENUMS], [glib-mkenums])
PKG_CHECK_MODULES([GLIB], [glib-2.0])
PKG_CHECK_MODULES([GOBJECT], [gobject-2.0])
PKG_CHECK_MODULES([GTK], [gtk+-3.0])
PKG_CHECK_MODULES([SWE_GLIB], [swe-glib])
AC_CONFIG_MACRO_DIR([m4])

LIBGD_INIT([
    header-bar
    static
])

AC_CONFIG_FILES([
    Makefile
    libgd/Makefile
    src/Makefile
    help/Makefile
    po/Makefile.in
    data/geonames/Makefile
    data/icons/Makefile
    docs/reference/astrognome/Makefile
])
AC_OUTPUT
