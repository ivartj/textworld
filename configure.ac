AC_INIT([textworld], [0.01], [it.jarlsby@stud.uis.no])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

AC_PROG_CC
AM_PROG_CC_C_O
m4_ifdef([AM_PROG_AR], [AM_PROG_AR])
AC_PROG_RANLIB

AC_CHECK_HEADER([windows.h], [
	AC_DEFINE([WIN32], [1], [We are compiling for Windows.])
	AC_CHECK_LIB([ws2_32], [_head_libws2_32_a])
	windows=[yes]
])
AM_CONDITIONAL([WIN32], [test x$windows = xyes])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
	Makefile
	src/Makefile
	net/Makefile
	])
AC_OUTPUT
