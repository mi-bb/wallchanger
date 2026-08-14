# Changelog

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

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/).

## [Unreleased]

### Changed

- Switched the C standard from C17 to C23 (`gnu23`) in both the Autotools and CMake build definitions.
- Modernized the code for C23: replaced GNU `__attribute__((unused))`/`__attribute__((noreturn))` with the standard `[[maybe_unused]]`/`[[noreturn]]` attributes where available, converted numeric `#define` constants to typed `constexpr` values, and replaced `NULL` with `nullptr` throughout the codebase.
- `randomm.c` no longer uses the process-global `rand()`/`srand()` generator; each `RandMem` now carries its own xorshift32 state, seeded from the current time, the object's address, and a per-process counter, so separate `RandMem` instances (and any other code in the process calling `rand()`/`srand()`) can no longer interfere with one another.
- Merged `INSTALL.md` into `README.md`; installation instructions now live under the "Installation and running" section instead of a separate file.
- Bumped the project version to 1.6.19 in `configure.ac` and `CMakeLists.txt`.

### Fixed

- Fixed a latent const-correctness bug in the file extension and file path helpers (`get_file_ext`, `imageinfo_new_from_file`) that only surfaced as a warning under C23, where glibc's type-generic `strrchr()` returns a `const char *` for a `const char *` argument.
- Fixed a signed left-shift (`1 << i_pos`) in the random-number bitmask helpers in `randomm.c` that was undefined behavior once the shift reached bit 31; the shifted literal is now unsigned (`1u << i_pos`).

## [1.6.18] - 2026-07-13

### Changed

- Improved libcurl error reporting, using `CURLOPT_ERRORBUFFER` for more accurate error messages.
- Minor code cleanup in the web wallpaper search widgets.

### Fixed

- Fixed a compiler warning for an unchecked `system()` call result in the other settings dialog.
- Fixed default settings being silently lost (and leaked) when checked against an empty settings list, e.g. on first run with no config file yet.
- Fixed a memory leak in the Pexels/Pixabay image URL base name extraction helper.

### Removed

- Dropped the libX11 build dependency. `wchangerd` now checks the `WAYLAND_DISPLAY`/`DISPLAY` environment variables to detect a graphical session instead of opening an X11 connection, so it no longer refuses to start under Wayland-only sessions without XWayland.

## [1.6.17] - 2020-05-03

### Added

- Added buttons to open directories in the other settings dialog.
- Added deletion of a no longer needed empty shared data directory during the uninstallation of the app.

## [1.6.16] - 2020-01-22

### Changed

- Some code cleaning, minor updates.

## [1.6.15] - 2020-12-19

### Changed

- Source code updates.

## [1.6.14] - 2020-12-15

### Fixed

- Corrected the rare occurrence of an infinite loop in `wchangerd`. When looking for process and reading comm file it could not read eof once in a while.

## [1.6.13] - 2020-12-07

### Changed

- Source code updates.

## [1.6.12] - 2020-11-13

### Changed

- Source code updates.

### Fixed

- Corrected page number showing when getting data from cache on wallpaper search page.
- Corrected Flickr search crash.

## [1.6.11] - 2020-11-08

### Added

- Added Wallpaper Abyss to wallpaper search.

### Changed

- Source code updates.

## [1.6.10] - 2020-11-05

### Added

- Added "Other settings" dialog.

### Changed

- Source code updates.

## [1.6.9] - 2020-11-01

### Added

- Added search options for Pexels, Pixbay, Wallhaven.cc

### Changed

- Source code updates.

## [1.6.8] - 2020-10-26

### Added

- Added wallhaven.cc to wallpaper search.

### Changed

- Source code updates.

## [1.6.7] - 2020-10-25

### Added

- Added caching of search queries.

### Changed

- Source code updates.

## [1.6.6] - 2020-10-22

### Fixed

- Corrected `wchangerd` daemon getting stuck in rare occasions when using time alignment.

## [1.6.5] - 2020-10-18

### Added

- Added marking of previously downloaded wallpapers in search results.

### Changed

- Updated random wallpapers select.
- Some icons changed, some compacted.
- Other minor source code updates.

## [1.6.4] - 2020-10-13

### Added

- Added Pixbay to wallpaper search.
- Added thumbnail caching.

### Changed

- Source code updates.

## [1.6.3] - 2020-10-08

### Added

- Added Flickr to wallpaper search.

### Changed

- Source code updates.

## [1.6.2] - 2020-10-02

### Fixed

- Corrected version shown by `wchangerd` daemon.

## [1.6.1] - 2020-09-29

### Fixed

- Corrected API key refresh after setting using config dialog.

## [1.6.0] - 2020-09-27

### Added

- Added option for getting wallpapers from web.
- Added Pexels to wallpaper search.

### Changed

- Source code updates.

## [1.5.5] - 2020-08-26

### Changed

- Updated structure and functions for remembering randomed numbers.

## [1.5.4] - 2020-08-07

### Added

- Added files with default window manager info and autostart entry in application share folder.

### Changed

- Source code updates.

## [1.5.3] - 2020-07-14

### Changed

- Source code updates.

## [1.5.2] - 2020-07-09

### Changed

- Source code updates.

## [1.5.1] - 2020-07-02

### Added

- Added dwm to window manager list.

### Changed

- Source code updates.

## [1.5.0] - 2020-06-22

### Added

- Added `--once` option for `wchangerd` daemon to change wallpaper once and exit.

### Changed

- Wallpaper change command is now set on start, based on currently used window manager.
- Code cleaning, changed internal setting passing, couple files removed.

## [1.4.9] - 2020-06-02

### Added

- Added option to get default wallpaper set command for Xfce.

## [1.4.8] - 2020-05-28

### Added

- Added time align option

## [1.4.7] - 2020-05-24

### Added

- Added possibility to create autostart entry for `wchangerd` daemon.

### Changed

- Updated command select dialog.
- Source code updates.

## [1.4.6] - 2020-05-18

### Changed

- Source code updates.

## [1.4.5] - 2020-05-05

### Added

- Added some window manager commands.

### Changed

- Source code updates.

## [1.4.4] - 2020-05-03

### Added

- Added dialog for detecting and setting wallpaper set command.

### Changed

- Source code updates.

## [1.4.3] - 2020-04-28

### Added

- Added config file info on statusbar.

### Changed

- Source code updates.

## [1.4.2] - 2020-04-17

### Added

- Option to run `wchangerd` daemon with other than standard config file using option `--config`.
- Added script for `wchangercfg` options bash completion.
- Added config dialog icon and desktop entry.

## [1.4.1] - 2020-04-12

### Added

- Added script for `wchangerd` options bash completion.

### Fixed

- Corrected random wallpaper select.

## [1.4.0] - 2020-04-11

### Added

- Added controls for starting, stopping and monitoring of `wchangerd` daemon.
- Command line options for starting, stopping, restarting, getting status of `wchangerd` daemon.

### Changed

- Source code updates.

## [1.3.11] - 2020-03-06

### Changed

- Code improvements.

## [1.3.10] - 2020-02-19

### Removed

- Removed some unneeded code.

## [1.3.9] - 2020-02-14

### Changed

- Source code updates.

### Fixed

- Corrected setting wrong wallpaper set command when it is made with file sign `[F]`.

## [1.3.8] - 2020-02-06

### Changed

- Changed adding files from directory, it works a little faster.
- Some source code updates.

### Fixed

- Corrected reading settings when json file is not a valid json file.

### Removed

- Removed sort button, list sorts itself after clicking title.

## [1.3.7] - 2020-01-15

### Fixed

- Corrected filtering files when adding from directory.

## [1.3.6] - 2020-01-14

### Added

- Added screen description below wallpaper preview.

### Changed

- Small performance improvements.
- Source code updates.

## [1.3.5] - 2019-12-22

### Changed

- Application should work in FreeBSD too.
- Slimmed wallpaper changing program `wchangerd`.
- There is no need to restart `wchangerd` after settings change with `wchangercfg`, Settings are loaded before every wallpaper change.
- Couple improvements for faster loading and saving list of wallpapers.
- Source code updates.

## [1.3.4] - 2019-12-09

### Changed

- Some code updates for better compatibility with Clang.
- Source code updates.

## [1.3.3] - 2019-12-07

### Added

- Added scripts to configure and build app.

### Changed

- Renamed wchanger-cfg to `wchangercfg`.

## [1.3.2] - 2019-12-02

### Changed

- Some code updates.

## [1.3.1] - 2019-11-27

### Added

- Added application information dialog.

### Changed

- Some code updates.

## [1.3.0] - 2019-11-25

### Changed

- Changed setting manage method.
- Some code updates.

## [1.2.3] - 2019-11-19

### Changed

- Small code updates.

## [1.2.2] - 2019-11-16

### Added

- Added removing duplicates from list button.
- Added saving width and height of configuration window.

### Changed

- Some code updates.

## [1.2.1] - 2019-11-14

### Added

- Added ComboBox with minutes and hours in wallpaper change interval option.

### Changed

- Some code updates.

## [1.2.0] - 2019-11-14

### Changed

- Changed random wallpaper select method.
- Changed file list storing.
- Background changing program (`wchangerd`) doesn't use glib now, it is lighter.
- Some other code updates.

## [1.1.1] - 2019-11-08

### Changed

- Some code updates.

## [1.1.0] - 2019-11-07

### Changed

- Changed wallpaper preview.

## [1.0.1] - 2019-11-04

### Changed

- Code updates.

## [1.0] - 2019-11-03

- Initial release.
