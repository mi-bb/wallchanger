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
# Only added by the top-level CMakeLists.txt when 'check' was found
# (mirrors the HAVE_CHECK Automake conditional).

add_executable(test_setting
    test_setting.c
    ${CMAKE_SOURCE_DIR}/src/setting.c ${CMAKE_SOURCE_DIR}/src/setting.h
    ${CMAKE_SOURCE_DIR}/src/hashfun.c ${CMAKE_SOURCE_DIR}/src/hashfun.h)
target_include_directories(test_setting PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_setting PRIVATE PkgConfig::CHECK)

add_test(NAME test_setting COMMAND test_setting)
