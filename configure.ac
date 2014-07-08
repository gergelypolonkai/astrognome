AC_INIT([Astrognome], [0.1], [gergely@polonkai.eu], [astrognome])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([-Wall foreign])
AM_SILENT_RULES([yes])
AC_CONFIG_MACRO_DIR([m4])

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
AM_GLIB_GNU_GETTEXT
LT_INIT
YELP_HELP_INIT
GTK_DOC_CHECK([1.19], [--flavour no-tmpl])
PKG_PROG_PKG_CONFIG
GLIB_GSETTINGS
AC_PATH_PROG([GLIB_MKENUMS], [glib-mkenums])
PKG_CHECK_MODULES([GLIB], [glib-2.0 >= 2.38])
PKG_CHECK_MODULES([GOBJECT], [gobject-2.0])
PKG_CHECK_MODULES([GTK], [gtk+-3.0 >= 3.8])
PKG_CHECK_MODULES([LIBXML], [libxml-2.0])
PKG_CHECK_MODULES([LIBXSLT], [libexslt])
PKG_CHECK_MODULES([WEBKIT], [webkitgtk-3.0])
PKG_CHECK_MODULES([SWE_GLIB], [swe-glib >= 2.0.0])

AC_CONFIG_FILES([
    Makefile
    src/Makefile
    help/Makefile
    po/Makefile.in
    data/Makefile
    data/astrognome.desktop.in
    data/geonames/Makefile
    data/icons/Makefile
])
AC_OUTPUT
