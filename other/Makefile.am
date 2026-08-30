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

bashcompdir = @bashcompdir@
dist_bashcomp_DATA = wchangerd wchangercfg

desktopdir = $(datadir)/applications
dist_desktop_DATA = wchangercfg.desktop

pixmapdir=$(datadir)/pixmaps
dist_pixmap_DATA=wchangercfg.png

appdatadir=$(datadir)/wchanger
dist_appdata_DATA=wchangerd.autostart wms.json

# The directory is only removed when it is left empty, and the rmdir has to
# honour DESTDIR (a staged uninstall, as done by `make distcheck`, never
# touches the real prefix). Failure is not an error: the directory may hold
# files from another package, or be gone already.
uninstall-hook:
	-rmdir "$(DESTDIR)$(datadir)/wchanger"

