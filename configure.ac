AC_PREREQ([2.68])
AC_INIT([testgear-plugins], [0.2], [], [testgear-plugins], [http://github.com/testgear/testgear-plugins])
AM_INIT_AUTOMAKE([1.11 foreign dist-xz no-dist-gzip -Wall -Werror])
AM_SILENT_RULES([yes])
AC_PROG_CC
AM_PROG_AR

AC_DISABLE_STATIC
AC_ENABLE_SHARED

LT_PREREQ([2.2])
LT_INIT

AC_CONFIG_MACRO_DIR([m4])
AC_LANG([C])
AC_PROG_INSTALL
AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([src/Makefile])
AC_CONFIG_FILES([src/dummy/Makefile])
AC_CONFIG_FILES([src/shell/Makefile])
AC_OUTPUT
