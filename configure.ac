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
AC_INIT([wallchanger], [1.4.8], [michalb1981@o2.pl], [wallchanger],\
        [https://www.nongnu.org/wallchanger/])
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([src/settlist.c])
AC_CONFIG_HEADERS([config.h])
AC_GNU_SOURCE

# Checks for programs.
AC_PROG_CC
AC_PATH_XTRA

# Checks for procstat for FreeBSD.
AC_CHECK_LIB([procstat], [procstat_getprocs], [], [], [])

# Checks for libraries.
PKG_CHECK_MODULES([GTK],  [gtk+-3.0 >= [3.22.0]])
PKG_CHECK_MODULES([JSON], [json-c >= [0.12.1]])

# Check for bash completion and set its directory.
PKG_CHECK_VAR(bashcompdir, [bash-completion], [completionsdir], ,
  bashcompdir="${sysconfdir}/bash_completion.d")
AC_SUBST(bashcompdir)

# Checks for header files.
AC_CHECK_HEADERS([inttypes.h stddef.h stdint.h stdlib.h string.h unistd.h\
                  json-c/json.h sys/param.h])

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

