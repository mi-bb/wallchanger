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

cmake_minimum_required(VERSION 3.13)

project(wallchanger
    VERSION 1.6.18
    DESCRIPTION "Wall Changer is an automatic wallpaper changer."
    HOMEPAGE_URL "https://www.nongnu.org/wallchanger/"
    LANGUAGES C)

set(PACKAGE_BUGREPORT "michal.babik@pm.me")

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Build type" FORCE)
endif()

include(GNUInstallDirs)
include(CheckIncludeFile)
include(CheckSymbolExists)
include(CheckLibraryExists)
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

# FreeBSD procstat support, used by src/procfn.c.
check_library_exists(procstat procstat_getprocs "" HAVE_LIBPROCSTAT)
if(HAVE_LIBPROCSTAT)
    find_library(PROCSTAT_LIBRARY procstat)
endif()

# Checks for header files.
check_include_file(inttypes.h  HAVE_INTTYPES_H)
check_include_file(stddef.h    HAVE_STDDEF_H)
check_include_file(stdint.h    HAVE_STDINT_H)
check_include_file(stdio.h     HAVE_STDIO_H)
check_include_file(stdlib.h    HAVE_STDLIB_H)
check_include_file(string.h    HAVE_STRING_H)
check_include_file(strings.h   HAVE_STRINGS_H)
check_include_file(unistd.h    HAVE_UNISTD_H)
check_include_file(sys/param.h HAVE_SYS_PARAM_H)
check_include_file(sys/stat.h  HAVE_SYS_STAT_H)
check_include_file(sys/time.h  HAVE_SYS_TIME_H)
check_include_file(sys/types.h HAVE_SYS_TYPES_H)

set(CMAKE_REQUIRED_INCLUDES ${JSONC_INCLUDE_DIRS})
check_include_file(json-c/json.h HAVE_JSON_C_JSON_H)
unset(CMAKE_REQUIRED_INCLUDES)

set(CMAKE_REQUIRED_INCLUDES ${CURL_INCLUDE_DIRS})
check_include_file(curl/curl.h HAVE_CURL_CURL_H)
unset(CMAKE_REQUIRED_INCLUDES)

# Checks for library functions.
check_symbol_exists(fork    unistd.h HAVE_FORK)
check_symbol_exists(alarm   unistd.h HAVE_ALARM)
check_symbol_exists(memset  string.h HAVE_MEMSET)
check_symbol_exists(mkdir   sys/stat.h HAVE_MKDIR)
check_symbol_exists(strchr  string.h HAVE_STRCHR)
check_symbol_exists(strdup  string.h HAVE_STRDUP)
check_symbol_exists(strndup string.h HAVE_STRNDUP)
check_symbol_exists(strrchr string.h HAVE_STRRCHR)
check_symbol_exists(strstr  string.h HAVE_STRSTR)
if(HAVE_FORK)
    set(HAVE_WORKING_FORK 1)
endif()
set(HAVE_MALLOC 1)
set(HAVE_REALLOC 1)

# src/*.c include config.h as "../config.h" (a plain, unconditional quoted
# include, not gated on HAVE_CONFIG_H), which the compiler resolves relative
# to each source file's own directory in src/ — i.e. always against the top
# of the source tree, regardless of where the build directory is. Generate
# it there, exactly where Autotools puts it; config.h is already gitignored.
configure_file(${CMAKE_SOURCE_DIR}/cmake/config.h.in
               ${CMAKE_SOURCE_DIR}/config.h @ONLY)

# Automake always adds this to DEFS once AC_CONFIG_HEADERS is in play;
# src/cmdline.c/.h (gengetopt output) guards its config.h include on it.
add_compile_definitions(HAVE_CONFIG_H)

add_subdirectory(src)
add_subdirectory(other)

if(BUILD_TESTING AND CHECK_FOUND)
    add_subdirectory(tests)
endif()
