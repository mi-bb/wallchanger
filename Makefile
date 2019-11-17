# Copyright (C) 2019 Michał Bąbik
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
CC = gcc
CFLAGS = -Os
LIBS = -ljson-c
PACKAGE = $(shell pkg-config --cflags --libs gtk+-3.0)
LDLIBS = $(shell pkg-config --libs gtk+-3.0)
SRCSG = wchanger.c treev.c dlgs.c iminfo.c wallset.c setts.c jsfun.c \
	   	miscfun.c imgs.c flist.c randomm.c settstr.c
SRCSM = wcngdmn.c wallset.c setts.c jsfun.c miscfun.c flist.c randomm.c \
		settstr.c
OBJSG = $(SRCSG:.c=.o)
OBJSM = $(SRCSM:.c=.o)
WGUI = wchanger-cfg
MAIN = wchangerd

.PHONY: all clean

all: $(WGUI) $(MAIN)

$(WGUI): $(OBJSG)
	$(CC) $(CFLAGS) $(PACKAGE) -o $(WGUI) $(OBJSG) $(LDLIBS) $(LIBS)

$(MAIN): $(OBJSM)
	$(CC) $(CFLAGS) -o $(MAIN) $(OBJSM) $(LIBS)
#$(MAIN): $(OBJSM)
#	$(CC) $(CFLAGS) $(PACKAGE) -o $(MAIN) $(OBJSM) $(LDLIBS) $(LIBS)

.c.o:
	$(CC) $(CFLAGS) $(PACKAGE) -c $<  -o $@

clean:
	$(RM) *.o *~ $(WGUI) $(MAIN)

