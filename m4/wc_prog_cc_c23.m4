# wc_prog_cc_c23.m4 - Find the compiler option that enables C23.  -*- Autoconf -*-
#
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

# WC_PROG_CC_C23
# --------------
# Autoconf 2.72's AC_PROG_CC only knows how to request C11, but the sources
# use C23 features (nullptr, [[maybe_unused]], constexpr).  Probe for the
# option that enables C23 and append it to $CC, the same way AC_PROG_CC
# appends its own standard option.  Abort with a readable message when the
# compiler cannot provide C23 at all.
#
# The GNU dialects are tried before the strict ones: the sources call
# strndup(), and a strict -std=c23 hides it unless the right feature test
# macro is defined (see AC_USE_SYSTEM_EXTENSIONS in configure.ac).
AC_DEFUN([WC_PROG_CC_C23],
[AC_REQUIRE([AC_PROG_CC])dnl
AC_CACHE_CHECK([for $CC option to enable C23 features],
               [wc_cv_prog_cc_c23],
[wc_cv_prog_cc_c23=no
wc_save_CC=$CC
for wc_arg in '' -std=gnu23 -std=c23 -std=gnu2x -std=c2x
do
  CC="$wc_save_CC $wc_arg"
  AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
/* C23: nullptr constant, standard attributes, constexpr, enum with a
   fixed underlying type - all used by src/.  */
constexpr int wc_answer = 42;
enum wc_enum : int { WC_ENUM_VALUE = 1 };
[[maybe_unused]] static int wc_unused (void *p) { return p == nullptr; }
]], [[
  static_assert (wc_answer == 42, "constexpr");
  return WC_ENUM_VALUE - 1;
]])],
    [wc_cv_prog_cc_c23=${wc_arg:-none needed}])
  CC=$wc_save_CC
  test "x$wc_cv_prog_cc_c23" != xno && break
done
])
AS_CASE([$wc_cv_prog_cc_c23],
  [no], [AC_MSG_ERROR([$CC does not support C23.

Wall Changer requires a C23-capable compiler (GCC >= 13 or Clang >= 16).
Set CC to a newer compiler, e.g. ./configure CC=gcc-13])],
  ["none needed"], [],
  [CC="$CC $wc_cv_prog_cc_c23"])
])# WC_PROG_CC_C23
