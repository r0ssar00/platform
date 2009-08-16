#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT(platform, 0.5, r0ssar00@gmail.com)
AC_CONFIG_SRCDIR([src/database.cpp])

AC_ARG_ENABLE(doxygen-docs,
        AS_HELP_STRING([--enable-doxygen-docs],
                [build DOXYGEN documentation (requires Doxygen)]),
        [enable_doxygen_docs=$enableval],
        [enable_doxygen_docs=no]
)

AM_INIT_AUTOMAKE(platform, 0.5)
# Checks for programs.
AC_PROG_AWK
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_RANLIB
AC_PROG_LIBTOOL
AC_CONFIG_MACRO_DIR([m4])
AC_PROG_MKDIR_P
# DX_INIT_DOXYGEN(platform, doc/Doxyfile, doc)
# Checks for libraries.
AC_LANG_CPLUSPLUS
AC_PROG_CXX
AC_SUBST(CXXFLAGS)
AC_SUBST(LDFLAGS)

# Checks for header files.
AC_HEADER_STDC

# Checks for header files.
# AC_CHECK_HEADER([sqlite3.h],[],[AC_ERROR([SQlite3 engine not installed.  Install SQlite3.])])
# AC_CHECK_HEADER([cc++/common.h],[],[AC_ERROR([GNU Telephony project commoncpp not installed.  Install commoncpp.])])

# PKG_PROG_PKG_CONFIG([MIN-VERSION])
PKG_PROG_PKG_CONFIG
PKG_CHECK_MODULES([commoncpp], [libccgnu2 libccext2])
PKG_CHECK_MODULES([sqlite], [sqlite3])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_TYPE_INT64_T
AC_TYPE_SIZE_T

# Checks for doxygen

AC_PATH_PROG(DOXYGEN, doxygen, no)

AC_MSG_CHECKING([whether to build Doxygen documentation])
if test "$DOXYGEN" = "no" ; then
        have_doxygen=no
else
        have_doxygen=yes
fi

if test "$enable_doxygen_docs" = "auto" ; then
        enable_doxygen_docs=no

        AC_MSG_RESULT(no)
fi

if test "$enable_doxygen_docs" = "yes" ; then
        if test "$have_doxygen" = "no"; then
                AC_MSG_ERROR([Building Doxygen docs explicitly required, but Doxygen not found])
        fi

        AC_MSG_RESULT(yes)
fi

AM_CONDITIONAL(DOXYGEN_DOCS_ENABLED, test "$enable_doxygen_docs" = "yes")

# Checks for library functions.

AC_CONFIG_FILES([Makefile
                 osx/Makefile
                 sqlite3x/Makefile
                 src/Makefile
		 tests/Makefile
		 demos/Makefile
		 doc/Doxyfile
		 doc/Makefile])
AC_OUTPUT
