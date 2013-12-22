AC_INIT([pktanon], [1.4.0-dev], [www.tm.kit.edu/pktanon])
AM_INIT_AUTOMAKE([-Wall, -Werror])
AC_CONFIG_SRCDIR([src/main.cpp])
AC_PROG_CXX
AC_PROG_INSTALL

AC_CHECK_LIB(xerces-c, main, [], 
 AC_MSG_ERROR([xerces-c library not found]))

AC_CHECK_LIB(pthread, main, [], 
 AC_MSG_ERROR([pthread library not found]))

AC_LANG_SAVE
AC_LANG_CPLUSPLUS
AC_CHECK_HEADER([boost/pool/object_pool.hpp],[], 
 AC_MSG_ERROR([boost library not found]))
AC_LANG_RESTORE

AC_OUTPUT([Makefile])
