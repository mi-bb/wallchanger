# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## About

Wall Changer is an automatic wallpaper changer for GNU/Linux and FreeBSD, written in C with GTK+3. It builds two executables:

- `wchangerd` — background daemon that switches the wallpaper on a configured interval (`--start`, `--stop`, `--restart`, `--once`, `--config FILE`).
- `wchangercfg` — GTK+3 configuration GUI for managing the wallpaper list, change interval, per-window-manager wallpaper-set command, and online wallpaper search/download.

Config is read from `~/.config/wchanger/config.json` (falling back to `~/.config/wchanger.json` / `~/.config/wchanger/wchanger.json`), or an explicit path via `--config`.

## Build

Standard GNU Autotools flow:

```sh
./autogen.sh
./configure
make
make install
```

Recommended configure invocation for daily development builds:

```sh
./configure CC="gcc" CFLAGS="-march=native -O2 -pipe -std=gnu17" --prefix=/usr
```

Build artifacts (`Makefile`, `configure`, `config.h`, `autom4te.cache/`, etc.) are generated and gitignored — do not hand-edit them; edit `configure.ac` / `Makefile.am` instead and rerun `autogen.sh`.

CMake (>= 3.13) is also supported as an alternative to Autotools (`CMakeLists.txt` / `src/CMakeLists.txt` / `tests/CMakeLists.txt`, documented in `INSTALL`); keep both build definitions in sync when adding sources or dependencies.

### Tests

`tests/` has a `check`-based unit test suite (`tests/test_setting.c`, exercising `setting.c` + `hashfun.c`). It only builds if the `check` library (>= 0.15.0) is found by `configure` (`HAVE_CHECK` conditional in `configure.ac`/`tests/Makefile.am`); otherwise it's silently skipped. Run it with:

```sh
make check
```

New test binaries go in `tests/Makefile.am` under the `if HAVE_CHECK` block, following the `test_setting` pattern (list the `src/*.c`/`.h` files under test directly in `_SOURCES` since `tests/` doesn't link against the built binaries).

### Required dependencies (checked in `configure.ac`)

- GTK+ 3 (>= 3.22.0), json-c (>= 0.12.1), libcurl (>= 7.68.0)
- Optional: flickcurl (>= 1.24) for Flickr search — enables `HAVE_FLICKCURL` and, if present, libxml2 (>= 2.9.4) for `HAVE_LIBXML`. Both are conditionally compiled in (`AM_CONDITIONAL(FLICKCURL...)` / `AM_CONDITIONAL(LIBXML...)` in `configure.ac`, `if FLICKCURL` / `if LIBXML` blocks in `src/Makefile.am`).
- Optional: `check` (>= 0.15.0) to build/run the unit tests above.

## Architecture

### Two binaries sharing one `src/` tree

`src/Makefile.am` lists `wchangerd_SOURCES` and `wchangercfg_SOURCES` separately, but both pull from the same pool of `.c`/`.h` files in `src/`. Core logic (settings, JSON I/O, wallpaper-set commands, process/daemon helpers, string/hash/random utilities) is shared; GTK dialog and web-search code (`dlgs*.c`, `treev.c`, `preview.c`, `web*.c`, `thumbs.c`, `imgs.c`) is exclusive to `wchangercfg`. When adding a shared helper, add its object to both `_SOURCES` lists in `src/Makefile.am`.

### Config/settings pipeline

- `cfgfile.c/.h` — locates and validates the config file (search order above).
- `jsof.c` / `jsfun.c` — JSON object read/write helpers built on json-c.
- `setting.c/.h` — the in-memory `Setting` data model (linked list of key/value settings) that both binaries read/write.
- `setts.c/.h` — higher-level settings load/apply logic used by both the daemon loop and the GUI.

`wchangerd` reloads settings from disk on every wallpaper-change cycle (see `chk_setts_ch_wall` in `wcngdmn.c`), so config changes made via `wchangercfg` take effect on the daemon's next interval without a restart — except the interval/time-align state itself, which is why `wchangerd --restart` is recommended after changing those.

### Daemon lifecycle (`wcngdmn.c`, `dmfn.c/.h`, `procfn.c/.h`, `proclist.c/.h`, `procitem.c/.h`)

`wcngdmn.c` is the `wchangerd` entry point: parses CLI args (`cmdfn.c/.h`, `cmdline.c/.h`), checks for/kills an already-running daemon via process-list scanning (`procfn.c` — includes a FreeBSD `procstat` path guarded by the `AC_CHECK_LIB([procstat], ...)` check in `configure.ac`), daemonizes (`dmfn_daemonize`), then loops: sleep for the configured interval (optionally time-aligned to the hour, see `check_time_align_val`), reload settings, change wallpaper.

### Wallpaper setting (`wpset.c/.h`)

Central point where a wallpaper is actually applied: builds the configured shell command (with `[F]` substituted for the file path, e.g. default `feh --bg-fill "[F]"`) and runs it. Called both from the daemon loop and from `wchangercfg` (startup preview / "test" button / on save).

### Window-manager command presets (`wmsfn.c/.h`, `other/wms.json`)

`wms.json` ships a list of window managers and their default wallpaper-set commands (Xfce needs a non-standard one). `wchangercfg`'s "Select" dialog (`dlgcmd.c/.h`) lets the user pick/override the detected WM's command; `wmsfn.c` handles matching the running WM to a preset.

### Online wallpaper search (`web*.c/.h`, `webwidget*.c/.h`)

Each provider (`webflickr`, `webpexels`, `webpixbay`, `webwallhaven`, `webabyss`) implements the same pattern against a shared `WebWidget` GTK widget (`webwidget.c/.h`, `webwidget_c.c` for the client-side glue, `webwidget_s.h` for the shared struct). All use libcurl for HTTP and json-c to parse API responses; `webflickr.c` additionally needs flickcurl/libxml2 and is compiled in only when those are detected. `chquery.c/.h` and `urldata.c/.h` build the search queries/URLs; downloaded images are cached under `PTH_WALLP_DIR`, thumbnails under `PTH_THUMB_DIR`, and per-image metadata under `PTH_IINFO_DIR`/`PTH_QUERY_DIR` (see path defines in `defs.h`).

### GUI structure (`wchanger.c`, `dialogdata.c/.h`, `dlgs*.c/.h`, `treev.c/.h`)

`wchanger.c` is the `wchangercfg` entry point and wires together the main settings window. `dialogdata.c/.h` (`DialogData`) bundles the window's widgets and current settings state, passed around to the get/set-from-widgets helper functions in each dialog source file. `treev.c/.h` manages the GtkTreeView showing the selected wallpaper list; `preview.c/.h` and `thumbs.c/.h` handle image preview/thumbnail generation.

### Shared low-level utilities

`strfun.c`, `nstrings.c`, `hashfun.c`, `randomm.c`, `rwdt.c` (read/write data/time), `fdfn.c` (file/dir helpers), `dirlist.c`, `errs.c` (error reporting) — generic helpers used throughout both binaries.

### Naming/style conventions to follow

- Doxygen-style `/** @file / @brief / @param / @return */` comment blocks on every file and public function declaration in headers, plus a `/*----...----*/` rule between sections.
- Hungarian-ish prefixes on variables/params encode type and direction, e.g. `s_` (string), `i_`/`ui_` (int/uint), `st_` (Setting*), `dd_` (DialogData*), `[in]`/`[out]`/`[in,out]` in doxygen `@param` tags.
- GPLv3 license header at the top of every source/header file.
