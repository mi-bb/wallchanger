# Wall Changer

Copyright (C) 2019-2026 Michał Bąbik

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.

## Contents

- [About](#about)
- [Requirements](#requirements)
- [Installation and running](#installation-and-running)
- [Contact and help](#contact-and-help)

## About

Wall Changer is an automatic wallpaper changer. User can select images
that will be used as wallpapers, select command to set wallpaper,
wallpaper change time interval.

Program works on GNU/Linux, FreeBSD.

## Requirements

- GNU/Linux, FreeBSD
- GCC / Clang
- GTK+ 3 (>= 3.22)
- json-c (>= 0.12.1)
- libcurl (>= 7.68.0)

## Installation and running

For installation instructions see the [INSTALL.md](INSTALL.md) file.

If compilation ends without problems, 2 executable files will be created:

```
wchangerd
wchangercfg
```

`wchangerd` works in background and changes wallpaper in specified time
intervals.

`wchangercfg` is a configuration window to set wallpaper change time
intervals, images on wallpaper list, the way wallpapers change, etc.

Working with the `wchangerd` daemon:

```
wchangerd --start      Starts the wchangerd daemon
wchangerd --stop       Stops the wchangerd daemon
wchangerd --restart    Restarts the wchangerd daemon

wchangerd --once       Change wallpaper once and exit

wchangerd --config     Loads configuration from given file
```

The wallpaper change command is selected based on the currently used
window manager. Application has default wallpaper change commands for
several window managers. Default commands can be changed through
`wchangercfg`, using the Select button. It opens a wallpaper command set
dialog with a list of window managers and commands. After checking and
setting commands, especially for Xfce, which has an unusual wallpaper set
command, you can use this app with different window managers without
changing settings.

After changing settings using `wchangercfg`, the `wchangerd` daemon will
load them before the next wallpaper change. If you want to load settings
before that time, you need to restart the `wchangerd` daemon using the
`wchangerd --restart` command.

Default places for configuration file (the application will look for it
in order like on the list below):

```
~/.config/wchanger/config.json
~/.config/wchanger.json
~/.config/wchanger/wchanger.json
```

To use other than standard config path, use the `--config` option:

```
wchangerd --config [FILENAME]
wchangercfg --config [FILENAME]
```

## Contact and help

In case of problems with the program, please contact me:
michal.babik@pm.me
