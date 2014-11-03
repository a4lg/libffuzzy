AC_PREREQ([2.65])
AC_INIT([libffuzzy], [2.1.4], [li@livegrid.org])
AC_CONFIG_SRCDIR([ffuzzy_compare.c])
AC_CONFIG_AUX_DIR([ext])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([ffuzzy_config.h])
AM_INIT_AUTOMAKE([foreign dist-xz])

AC_ARG_ENABLE([debug],AS_HELP_STRING([--enable-debug],[enable debugging code]),,[enable_debug=no])
if test "x$enable_debug" = xno
then
AC_DEFINE([NDEBUG],[1],[Disable assertion code])
fi

AC_PROG_CC_C99
LT_INIT

AC_OUTPUT([Makefile])
