#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
# Copyright (C) 2019-2026 Michał Bąbik
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

AC_PREREQ([2.71])
AC_INIT([wallchanger], [1.6.20], [michal.babik@protonmail.com], [wallchanger],
        [https://www.nongnu.org/wallchanger/])
AC_CONFIG_SRCDIR([src/setting.c])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIRS([m4])
AC_CONFIG_HEADERS([config.h])
AC_CANONICAL_HOST
AM_INIT_AUTOMAKE([1.16 subdir-objects foreign -Wall])
AM_SILENT_RULES([yes])

# Checks for programs.
# AC_USE_SYSTEM_EXTENSIONS must come before AC_PROG_CC: src/iminfo.c calls
# strndup(), which a strict -std=c23 hides without _GNU_SOURCE / _POSIX_C_SOURCE.
AC_USE_SYSTEM_EXTENSIONS
AC_PROG_CC
# The sources use C23 (nullptr, [[maybe_unused]], constexpr); Autoconf's own
# AC_PROG_CC only goes up to C11, so ask for C23 explicitly (m4/wc_prog_cc_c23.m4).
WC_PROG_CC_C23

# Check procstat for FreeBSD; only src/procfn.c uses it, and only there.
# AC_SEARCH_LIBS appends to the global LIBS on success, which would link
# -lprocstat into every target, so save and restore LIBS around it and pass
# the result to the two programs through PROCSTAT_LIBS instead.
PROCSTAT_LIBS=
AS_CASE([$host_os],
  [freebsd*],
  [wc_save_LIBS=$LIBS
   AC_SEARCH_LIBS([procstat_getprocs], [procstat],
                  [AS_IF([test "x$ac_cv_search_procstat_getprocs" != "xnone required"],
                         [PROCSTAT_LIBS=$ac_cv_search_procstat_getprocs])],
                  [AC_MSG_ERROR([libprocstat is required on FreeBSD])])
   LIBS=$wc_save_LIBS])
AC_SUBST([PROCSTAT_LIBS])

# Checks for libraries.
PKG_CHECK_MODULES([GTK],  [gtk+-3.0 >= 3.22.0])
PKG_CHECK_MODULES([JSON], [json-c >= 0.12.1])
PKG_CHECK_MODULES([CURL], [libcurl >= 7.68.0])
PKG_CHECK_MODULES([FLICKCURL], [flickcurl >= 1.24],
                  [AC_DEFINE([HAVE_FLICKCURL], [1], [Use flickcurl])],
                  [AC_MSG_WARN([flickcurl not present])])
AM_CONDITIONAL([FLICKCURL], [test -n "$FLICKCURL_LIBS"])
AS_IF([test -n "$FLICKCURL_LIBS"],
      [PKG_CHECK_MODULES([LIBXML], [libxml-2.0 >= 2.9.4],
                         [AC_DEFINE([HAVE_LIBXML], [1], [Use libxml])],
                         [AC_MSG_WARN([libxml not present])])])
AM_CONDITIONAL([LIBXML], [test -n "$LIBXML_LIBS"])
# Check for Check unit testing framework (optional, used by tests/).
PKG_CHECK_MODULES([CHECK], [check >= 0.15.0], [have_check=yes],
                  [have_check=no
                   AC_MSG_WARN([check not present, unit tests will be skipped])])
AM_CONDITIONAL([HAVE_CHECK], [test "x$have_check" = "xyes"])
# Check for bash completion and set its directory.
PKG_CHECK_VAR([bashcompdir], [bash-completion], [completionsdir], ,
  [bashcompdir="${sysconfdir}/bash_completion.d"])
AC_SUBST([bashcompdir])

# Checks for header files.
# Only the results actually consulted by src/ are checked; HAVE_JSON_C_JSON_H
# gates the json-c include, HAVE_CURL_CURL_H documents the libcurl dependency.
AC_CHECK_HEADERS([json-c/json.h curl/curl.h])

AC_CONFIG_FILES([Makefile src/Makefile other/Makefile tests/Makefile])
AC_OUTPUT
