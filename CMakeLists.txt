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
#
# CMake build description, provided as an alternative to the Autotools
# flow (autogen.sh/configure/make). Both build systems can coexist; run
# an out-of-source CMake build (e.g. in ./build) to avoid clobbering
# Autotools-generated files.

# 3.21 is the real floor: it is the version CMakePresets.json's "version": 3
# requires, and the first release that knows the c_std_23 compile feature.
cmake_minimum_required(VERSION 3.21)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(wallchanger
    VERSION 1.6.20
    DESCRIPTION "Wall Changer is an automatic wallpaper changer."
    HOMEPAGE_URL "https://www.nongnu.org/wallchanger/"
    LANGUAGES C)

set(PACKAGE_BUGREPORT "michal.babik@protonmail.com")

# The sources use C23 with GNU extensions.  The standard is passed explicitly
# rather than through target_compile_features(... c_std_23), because that only
# emits a -std= flag when the compiler's own default is *older* than C23: on a
# compiler that already defaults to it (GCC >= 15) the compile line carries no
# standard at all, and every libclang-based tool reading compile_commands.json
# - clangd, clang-tidy, include-what-you-use - falls back to gnu17 and reports
# each nullptr/constexpr/bool in the tree as an error.  Passing it always also
# keeps the build reproducible across compilers with different defaults, and
# pins the modern spelling (CMake's own feature table still asks GCC for
# -std=gnu2x).
#
# The probe mirrors m4/wc_prog_cc_c23.m4: same dialect list, same test
# program, and the GNU dialects first because src/iminfo.c calls strndup(),
# which a strict -std=c23 hides.  It doubles as the capability check the
# compile feature used to provide.
include(CheckCSourceCompiles)

set(WC_C23_TEST_SOURCE [=[
/* C23: nullptr constant, standard attributes, constexpr, enum with a
   fixed underlying type - all used by src/.  */
constexpr int wc_answer = 42;
enum wc_enum : int { WC_ENUM_VALUE = 1 };
[[maybe_unused]] static int wc_unused (void *p) { return p == nullptr; }
int main (void)
{
    static_assert (wc_answer == 42, "constexpr");
    return WC_ENUM_VALUE - 1;
}
]=])

set(WC_C23_FLAG "WC_C23_FLAG-NOTFOUND")
foreach(wc_flag IN ITEMS -std=gnu23 -std=c23 -std=gnu2x -std=c2x)
    string(MAKE_C_IDENTIFIER "WC_HAVE_C23${wc_flag}" wc_result_var)
    set(CMAKE_REQUIRED_FLAGS "${wc_flag}")
    check_c_source_compiles("${WC_C23_TEST_SOURCE}" ${wc_result_var})
    unset(CMAKE_REQUIRED_FLAGS)
    if(${wc_result_var})
        set(WC_C23_FLAG "${wc_flag}")
        break()
    endif()
endforeach()

if(NOT WC_C23_FLAG)
    message(FATAL_ERROR
        "${CMAKE_C_COMPILER} does not support C23.\n"
        "Wall Changer requires a C23-capable compiler (GCC >= 13 or "
        "Clang >= 16).\n"
        "Set the compiler with -DCMAKE_C_COMPILER=gcc-13 or similar.")
endif()
message(STATUS "C23 option: ${WC_C23_FLAG}")

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Build type" FORCE)
endif()

include(GNUInstallDirs)
include(CheckIncludeFile)
include(CTest)

find_package(PkgConfig REQUIRED)

# Checks for libraries (mirrors the PKG_CHECK_MODULES calls in configure.ac).
pkg_check_modules(GTK   REQUIRED IMPORTED_TARGET gtk+-3.0>=3.22.0)
pkg_check_modules(JSONC REQUIRED IMPORTED_TARGET json-c>=0.12.1)
pkg_check_modules(CURL  REQUIRED IMPORTED_TARGET libcurl>=7.68.0)

pkg_check_modules(FLICKCURL IMPORTED_TARGET flickcurl>=1.24)
if(FLICKCURL_FOUND)
    set(HAVE_FLICKCURL 1)
    pkg_check_modules(LIBXML IMPORTED_TARGET libxml-2.0>=2.9.4)
    if(LIBXML_FOUND)
        set(HAVE_LIBXML 1)
    else()
        message(WARNING "libxml-2.0 not present")
    endif()
else()
    message(WARNING "flickcurl not present, Flickr search support disabled")
endif()

# Optional 'check' unit testing framework used by tests/.
pkg_check_modules(CHECK IMPORTED_TARGET check>=0.15.0)
if(NOT CHECK_FOUND)
    message(WARNING "check not present, unit tests will be skipped")
endif()

# Bash completion install directory.
pkg_check_modules(BASH_COMPLETION QUIET bash-completion)
if(BASH_COMPLETION_FOUND)
    pkg_get_variable(BASHCOMPDIR bash-completion completionsdir)
endif()
if(NOT BASHCOMPDIR)
    set(BASHCOMPDIR "${CMAKE_INSTALL_FULL_SYSCONFDIR}/bash_completion.d")
endif()

# FreeBSD procstat support; only src/procfn.c uses it, and only there.
set(PROCSTAT_LIBRARY "")
if(CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
    find_library(PROCSTAT_LIBRARY procstat REQUIRED)
endif()

# Checks for header files. Only the results src/ actually consults are
# probed (mirrors the AC_CHECK_HEADERS list in configure.ac).
set(CMAKE_REQUIRED_INCLUDES ${JSONC_INCLUDE_DIRS})
check_include_file(json-c/json.h HAVE_JSON_C_JSON_H)
unset(CMAKE_REQUIRED_INCLUDES)

set(CMAKE_REQUIRED_INCLUDES ${CURL_INCLUDE_DIRS})
check_include_file(curl/curl.h HAVE_CURL_CURL_H)
unset(CMAKE_REQUIRED_INCLUDES)

# config.h goes to the top of the build tree, exactly where Autotools puts
# it; the targets get that directory on their include path so the sources'
# `#include "config.h"` resolves there and the source tree stays clean.
configure_file(${PROJECT_SOURCE_DIR}/cmake/config.h.in
               ${PROJECT_BINARY_DIR}/config.h @ONLY)

add_subdirectory(src)
add_subdirectory(other)

if(BUILD_TESTING AND CHECK_FOUND)
    add_subdirectory(tests)
endif()
