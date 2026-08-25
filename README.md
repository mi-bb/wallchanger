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
  - [Basic Installation](#basic-installation)
  - [Compilers and Options](#compilers-and-options)
  - [Building with CMake](#building-with-cmake)
- [Contact and help](#contact-and-help)

## About

Wall Changer is an automatic wallpaper changer. User can select images
that will be used as wallpapers, select command to set wallpaper,
wallpaper change time interval.

Program works on GNU/Linux, FreeBSD.

### Supported wallpaper search pages:

 - Pexels
 - Pixbay
 - wallhaven.cc
 - Wallpaper Abyss
 - Flickr

## Requirements

- GNU/Linux, FreeBSD
- GCC / Clang
- GTK+ 3 (>= 3.22)
- json-c (>= 0.12.1)
- libcurl (>= 7.68.0)

## Installation and running

### Basic Installation

You must have autotools installed for the `autogen.sh` script to generate
the `configure` and `make` scripts. Otherwise these scripts are included
in the standard program package.

Commands to configure, build and install the program:

```
./autogen.sh
./configure
make
make install
```

It's suggested to configure and compile with the more detailed options
described below.

### Compilers and Options

For a normal daily use of this program a good option should be:

```
./configure CFLAGS="-march=native -O2 -pipe" --prefix=/usr
```

or more specific:

```
./configure CC="gcc" CFLAGS="-march=native -O2 -pipe -std=gnu23" \
--prefix=/usr
```

with Clang:

```
./configure CC="clang" CFLAGS="-march=native -O2 -pipe -std=gnu23" \
--prefix=/usr
```

This disables the standard `-g` option, which produces debugging
information needed for gdb and enlarges the output file:

- `CC="gcc"` — sets the C compiler to GCC
- `CC="clang"` — sets the C compiler to Clang
- `-march=native` — enables all instruction subsets supported by the local machine
- `-O2` — sets the code optimization to level 2
- `-pipe` — use pipes rather than temporary files for communication between the various stages of compilation
- `-std=gnu23` — sets the C standard to C23 with GNU extensions
- `--prefix=/usr` — where the app should be installed

Executing:

```
./configure --help
```

will print a detailed description of available initial values for
configuration parameters.

### Building with CMake

As an alternative to the Autotools flow above, the program can be built
with CMake (>= 3.13). Both build systems can coexist in the same source
tree; use an out-of-source build directory (e.g. `./build`) so CMake does
not clobber the Autotools-generated files.

Commands to configure, build and install the program:

```
cmake -S . -B build
cmake --build build
sudo cmake --install build
```

The build type defaults to `RelWithDebInfo`. To pick a different one, or
to set a custom install prefix:

```
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr
```

Unit tests (equivalent to `make check` with Autotools) are built
automatically when the `check` library is found, and can be run with:

```
ctest --test-dir build
```

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
