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
# CMake build description. Builds are out-of-source only
# (cmake --preset default configures and builds in ./build).

# 3.22 is the floor: it makes CMP0128 default to NEW, so CMAKE_C_STANDARD
# below always puts a -std= flag on the compile line (see the comment there).
cmake_minimum_required(VERSION 3.22)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(wallchanger
    VERSION 1.7.0
    DESCRIPTION "Wall Changer is an automatic wallpaper changer."
    HOMEPAGE_URL "https://www.nongnu.org/wallchanger/"
    LANGUAGES C)

set(PACKAGE_BUGREPORT "michal.babik@protonmail.com")

# The sources use C23 with GNU extensions (src/image_info.c calls strndup(),
# which a strict -std=c23 hides).  CMake picks the spelling the compiler knows
# (-std=gnu23 or -std=gnu2x).  CMP0128 (NEW since CMake 3.22) makes it emit
# the flag even when the compiler's own default already matches, so libclang-based
# tools reading compile_commands.json - clangd, clang-tidy, include-what-you-
# use - do not fall back to gnu17 on GCC >= 15.  Do not pass -std= by hand.
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

if((CMAKE_C_COMPILER_ID STREQUAL "GNU" AND CMAKE_C_COMPILER_VERSION VERSION_LESS 13) OR
   (CMAKE_C_COMPILER_ID MATCHES "Clang" AND CMAKE_C_COMPILER_VERSION VERSION_LESS 16))
    message(FATAL_ERROR
        "${CMAKE_C_COMPILER} is too old for C23.\n"
        "Wall Changer requires GCC >= 13 or Clang >= 16.\n"
        "Set the compiler with -DCMAKE_C_COMPILER=gcc-13 or similar.")
endif()

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Build type" FORCE)
endif()

include(GNUInstallDirs)
include(CheckIncludeFile)
include(CTest)

find_package(PkgConfig REQUIRED)

# Checks for libraries.
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

# FreeBSD procstat support; only src/process/process.c uses it, and only there.
set(PROCSTAT_LIBRARY "")
if(CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
    find_library(PROCSTAT_LIBRARY procstat REQUIRED)
endif()

# Checks for header files. Only the results src/ actually consults are
# probed.
set(CMAKE_REQUIRED_INCLUDES ${JSONC_INCLUDE_DIRS})
check_include_file(json-c/json.h HAVE_JSON_C_JSON_H)
unset(CMAKE_REQUIRED_INCLUDES)

set(CMAKE_REQUIRED_INCLUDES ${CURL_INCLUDE_DIRS})
check_include_file(curl/curl.h HAVE_CURL_CURL_H)
unset(CMAKE_REQUIRED_INCLUDES)

# config.h goes to the top of the build tree; the targets get that
# directory on their include path so the sources' `#include "config.h"`
# resolves there and the source tree stays clean.
configure_file(${PROJECT_SOURCE_DIR}/cmake/config.h.in
               ${PROJECT_BINARY_DIR}/config.h @ONLY)

add_subdirectory(src)
add_subdirectory(other)

if(BUILD_TESTING AND CHECK_FOUND)
    add_subdirectory(tests)
endif()

# `cmake --build build --target uninstall` undoes `cmake --install`: it
# removes every path recorded in install_manifest.txt and then drops the
# now-empty data directory.
if(NOT TARGET uninstall)
    configure_file(${PROJECT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in
                   ${PROJECT_BINARY_DIR}/cmake_uninstall.cmake IMMEDIATE @ONLY)
    add_custom_target(uninstall
        COMMAND ${CMAKE_COMMAND} -P ${PROJECT_BINARY_DIR}/cmake_uninstall.cmake)
endif()

# Source package for releases. Run
#   cpack --config build/CPackSourceConfig.cmake
# to produce wallchanger-<version>.tar.gz / .zip from the source tree.
set(CPACK_SOURCE_PACKAGE_FILE_NAME ${PROJECT_NAME}-${PROJECT_VERSION})
set(CPACK_SOURCE_GENERATOR "TGZ;ZIP")
set(CPACK_PACKAGE_DIRECTORY ${CMAKE_BINARY_DIR})
set(CPACK_SOURCE_IGNORE_FILES
    "^${CMAKE_BINARY_DIR}/"
    "/\\.git/"
    "\\.gitignore$"
    "/build/"
    "/_CPack_Packages/"
    "/\\.cache/"
    "/\\.vscode/"
    "/AGENTS\\.md$"
    "/CLAUDE\\.md$"
    "compile_commands\\.json$")
include(CPack)
