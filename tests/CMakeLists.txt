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
# Only added by the top-level CMakeLists.txt when 'check' was found.

add_executable(test_setting
    test_setting_item.c
    ${PROJECT_SOURCE_DIR}/src/settings/setting_item.c ${PROJECT_SOURCE_DIR}/src/settings/setting_item.h
    ${PROJECT_SOURCE_DIR}/src/util/hash_djb2.c ${PROJECT_SOURCE_DIR}/src/util/hash_djb2.h)
target_include_directories(test_setting
    PRIVATE ${PROJECT_BINARY_DIR} ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(test_setting PRIVATE PkgConfig::CHECK)

add_test(NAME test_setting COMMAND test_setting)

add_executable(test_process
    test_process.c
    ${PROJECT_SOURCE_DIR}/src/process/process.c   ${PROJECT_SOURCE_DIR}/src/process/process.h
    ${PROJECT_SOURCE_DIR}/src/process/proc_item.c ${PROJECT_SOURCE_DIR}/src/process/proc_item.h
    ${PROJECT_SOURCE_DIR}/src/process/proc_list.c ${PROJECT_SOURCE_DIR}/src/process/proc_list.h)
target_include_directories(test_process
    PRIVATE ${PROJECT_BINARY_DIR} ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(test_process PRIVATE PkgConfig::CHECK)
if(PROCSTAT_LIBRARY)
    target_link_libraries(test_process PRIVATE ${PROCSTAT_LIBRARY})
endif()

add_test(NAME test_process COMMAND test_process)

# The image search services: request urls and response parsing. The service
# files have no widget code, so GTK is needed for its GLib only.
add_executable(test_web_parse
    test_web_parse.c
    ${PROJECT_SOURCE_DIR}/src/util/cres.c             ${PROJECT_SOURCE_DIR}/src/util/cres.h
    ${PROJECT_SOURCE_DIR}/src/util/string_functions.c ${PROJECT_SOURCE_DIR}/src/util/string_functions.h
    ${PROJECT_SOURCE_DIR}/src/web/search_item.c       ${PROJECT_SOURCE_DIR}/src/web/search_item.h
    ${PROJECT_SOURCE_DIR}/src/web/url_data.c          ${PROJECT_SOURCE_DIR}/src/web/url_data.h
    ${PROJECT_SOURCE_DIR}/src/web/web_artic.c         ${PROJECT_SOURCE_DIR}/src/web/web_artic.h
    ${PROJECT_SOURCE_DIR}/src/web/web_commons.c       ${PROJECT_SOURCE_DIR}/src/web/web_commons.h
    ${PROJECT_SOURCE_DIR}/src/web/web_met.c           ${PROJECT_SOURCE_DIR}/src/web/web_met.h
    ${PROJECT_SOURCE_DIR}/src/web/web_nasa.c          ${PROJECT_SOURCE_DIR}/src/web/web_nasa.h
    ${PROJECT_SOURCE_DIR}/src/web/web_openverse.c     ${PROJECT_SOURCE_DIR}/src/web/web_openverse.h
    ${PROJECT_SOURCE_DIR}/src/web/web_smithsonian.c   ${PROJECT_SOURCE_DIR}/src/web/web_smithsonian.h
    ${PROJECT_SOURCE_DIR}/src/web/web_util.c          ${PROJECT_SOURCE_DIR}/src/web/web_util.h
    ${PROJECT_SOURCE_DIR}/src/web/web_provider.h)
target_include_directories(test_web_parse
    PRIVATE ${PROJECT_BINARY_DIR} ${PROJECT_SOURCE_DIR}/src)
target_compile_definitions(test_web_parse
    PRIVATE _GNU_SOURCE TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data")
target_link_libraries(test_web_parse
    PRIVATE PkgConfig::CHECK PkgConfig::GTK PkgConfig::JSONC PkgConfig::CURL)

add_test(NAME test_web_parse COMMAND test_web_parse)

add_executable(test_live
    test_live.c
    ${PROJECT_SOURCE_DIR}/src/live/live_bing.c        ${PROJECT_SOURCE_DIR}/src/live/live_bing.h
    ${PROJECT_SOURCE_DIR}/src/live/live_commons.c     ${PROJECT_SOURCE_DIR}/src/live/live_commons.h
    ${PROJECT_SOURCE_DIR}/src/live/live_source.c      ${PROJECT_SOURCE_DIR}/src/live/live_source.h
    ${PROJECT_SOURCE_DIR}/src/live/live_wallhaven.c   ${PROJECT_SOURCE_DIR}/src/live/live_wallhaven.h
    ${PROJECT_SOURCE_DIR}/src/util/cres.c             ${PROJECT_SOURCE_DIR}/src/util/cres.h
    ${PROJECT_SOURCE_DIR}/src/util/randomm.c          ${PROJECT_SOURCE_DIR}/src/util/randomm.h
    ${PROJECT_SOURCE_DIR}/src/util/string_functions.c ${PROJECT_SOURCE_DIR}/src/util/string_functions.h
    ${PROJECT_SOURCE_DIR}/src/web/url_data.c          ${PROJECT_SOURCE_DIR}/src/web/url_data.h
    ${PROJECT_SOURCE_DIR}/src/web/web_util.c          ${PROJECT_SOURCE_DIR}/src/web/web_util.h)
target_include_directories(test_live
    PRIVATE ${PROJECT_BINARY_DIR} ${PROJECT_SOURCE_DIR}/src)
target_compile_definitions(test_live
    PRIVATE _GNU_SOURCE TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data")
target_link_libraries(test_live
    PRIVATE PkgConfig::CHECK PkgConfig::JSONC PkgConfig::CURL)

add_test(NAME test_live COMMAND test_live)
