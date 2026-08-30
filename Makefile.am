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

ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src other tests

# Ship the CMake build definition in the dist tarball, so a release can be
# built either way (see README.md, "Building with CMake").
EXTRA_DIST = CMakeLists.txt \
             CMakePresets.json \
             cmake/config.h.in \
             src/CMakeLists.txt \
             other/CMakeLists.txt \
             tests/CMakeLists.txt \
             autogen.sh \
             CHANGELOG.md \
             README.md

