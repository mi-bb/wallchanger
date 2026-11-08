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
