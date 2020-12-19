#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
# Copyright (C) 2019-2020 Michał Bąbik
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

AC_PREREQ([2.69])
AC_INIT([wallchanger], [1.6.15], [michal.babik@pm.me], [wallchanger],\
        [https://www.nongnu.org/wallchanger/])
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([src/setting.c])
AC_CONFIG_HEADERS([config.h])
AC_GNU_SOURCE

# Checks for programs.
AC_PROG_CC
AC_PATH_XTRA

# Check procstat for FreeBSD.
AC_CHECK_LIB([procstat], [procstat_getprocs], [], [], [])

# Checks for libraries.
PKG_CHECK_MODULES([X11],  [x11 >= [1.6.9]])
PKG_CHECK_MODULES([GTK],  [gtk+-3.0 >= [3.22.0]])
PKG_CHECK_MODULES([JSON], [json-c >= [0.12.1]])
PKG_CHECK_MODULES([CURL], [libcurl >= [7.68.0]])
PKG_CHECK_MODULES([FLICKCURL], [flickcurl >= [1.24]],
                  [AC_DEFINE([HAVE_FLICKCURL], [1], [Use flickcurl])],
                  [AC_MSG_WARN("flickrcurl not present")])
AM_CONDITIONAL(FLICKCURL, test -n "$FLICKCURL_LIBS")
if test -n "$FLICKCURL_LIBS"; then
    PKG_CHECK_MODULES([LIBXML], [libxml-2.0 >= [2.9.4]],
                      [AC_DEFINE([HAVE_LIBXML], [1], [Use libxml])],
                      [AC_MSG_WARN("libxml not present")])
fi
AM_CONDITIONAL(LIBXML, test -n "$LIBXML_LIBS")
# Check for bash completion and set its directory.
PKG_CHECK_VAR(bashcompdir, [bash-completion], [completionsdir], ,
  bashcompdir="${sysconfdir}/bash_completion.d")
AC_SUBST(bashcompdir)

# Checks for header files.
AC_CHECK_HEADERS([inttypes.h stddef.h stdint.h stdlib.h string.h unistd.h\
                  json-c/json.h sys/param.h curl/curl.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_INT8_T
AC_TYPE_SIZE_T
AC_TYPE_UID_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_MKTIME
AC_CHECK_FUNCS([memset mkdir strchr strdup strndup strrchr strstr])

AC_CONFIG_FILES([Makefile src/Makefile other/Makefile])
AC_OUTPUT

