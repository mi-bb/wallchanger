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

if HAVE_CHECK

check_PROGRAMS = test_setting
TESTS = $(check_PROGRAMS)

test_setting_SOURCES = test_setting.c\
                       $(top_srcdir)/src/setting.c $(top_srcdir)/src/setting.h\
                       $(top_srcdir)/src/hashfun.c $(top_srcdir)/src/hashfun.h
test_setting_CPPFLAGS = -I$(top_srcdir)/src $(CHECK_CFLAGS)
test_setting_LDADD = $(CHECK_LIBS)

endif
