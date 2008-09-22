#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT([Kanatest], [0.4.6], [pasp@users.sf.net])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_CONFIG_SRCDIR([src/about.c])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.

AC_MSG_CHECKING(whether GTK+ version >= 2.6)
if pkg-config --exists 'gtk+-2.0 >= 2.6'; then

    gtk_CFLAGS=`pkg-config --cflags gtk+-2.0`
	gtk_LIBS=`pkg-config --libs gtk+-2.0`

    AC_MSG_RESULT(yes)
else
	AC_MSG_RESULT(no)
	AC_MSG_ERROR(GTK+ not found or too old (version < 2.6))
fi


AC_CHECK_PROG([var], [xml2-config], [yes], [no])
if test ! "$var" = "yes"; then
	AC_MSG_ERROR(You do not appear to have xml2-config installed.)
fi

xml_CFLAGS=`xml2-config --cflags`
xml_LIBS=`xml2-config --libs`

AC_CHECK_LIB(xml2,
	xmlNewDoc,
	[],
	[AC_MSG_ERROR(You do not appear to have libxml2 installed.)])

CFLAGS="$BUILD_CFLAGS -Wall $PLATFORM_CFLAGS -D_GNU_SOURCE"
CXXFLAGS="$CFLAGS"
CPPFLAGS="$gtk_CFLAGS $xml_CFLAGS"
LIBS="$gtk_LIBS $xml_LIBS"

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([libintl.h stdlib.h string.h sys/time.h unistd.h locale.h limits.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_HEADER_TIME
AC_STRUCT_TM

# Checks for library functions.
AC_FUNC_STAT
AC_CHECK_FUNCS([gettimeofday strrchr])
AC_CONFIG_FILES([ 
    Makefile
    src/Makefile
    po/Makefile
])
AC_OUTPUT
