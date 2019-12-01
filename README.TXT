CONTENTS OF THIS FILE
----------------------

 * About
 * Copyright and license
 * Requirements
 * Installation and running
 * Contact and help


About
------

Wall Changer is an automatic wallpaper changer.
User can select images that will be used as wallpapers,
select command to set wallpaper, wallpaper change time interval.

Program works in GNU/Linux.


Copyright and license
----------------------

Copyright (C) 2019 Michał Bąbik

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.


Requirements
-------------

 * GNU/Linux
 * GCC (my version was 9.2.0)
 * json-c (0.13.1)
 * GTK+ 3 (3.24)
 * GLib (2.60)

Program may work using previous versions, I just wrote on which I was compiling
this program and there were no problems.


Installation and running
------------------------

To compile program go to source code directory and type:

    make

If compilation ends without problems, 2 executable files will be created:

    wchanger-cfg
    wchangerd

wchangerd works in background and changes wallpaper
in specified time intervals.

wchanger-cfg is a configuration window to set wallpaper change time intervals,
images on wallpaper list and a way wallpapers change.

Afer changing settings using wchanger-cfg, wchangerd needs to be restarted.

Application configuration is stored in ~/.config/wchanger.json


Contact and help
-----------------

In case of problems with the program, please contact me:
michalb1981@o2.pl

