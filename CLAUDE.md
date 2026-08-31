# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## About

Wall Changer is an automatic wallpaper changer for GNU/Linux and FreeBSD, written in C with GTK+3. It builds two executables:

- `wchangerd` — background daemon that switches the wallpaper on a configured interval (`--start`, `--stop`, `--restart`, `--once`, `--config FILE`).
- `wchangercfg` — GTK+3 configuration GUI for managing the wallpaper list, change interval, per-window-manager wallpaper-set command, and online wallpaper search/download.

Config is read from `~/.config/wchanger/config.json` (falling back to `~/.config/wchanger.json` / `~/.config/wchanger/wchanger.json`), or an explicit path via `--config`.

## Build

CMake (>= 3.21) is the only build system; builds are out-of-source and write nothing into the checkout.

```sh
cmake --preset default
cmake --build --preset default
sudo cmake --install build
```

`default` = RelWithDebInfo in `./build`; `release` = optimized with the prefix set to `/usr`; `debug` = unoptimized with `-Wall -Wextra`. Recommended invocation for daily development builds:

```sh
cmake -S . -B build -DCMAKE_C_COMPILER=gcc \
    -DCMAKE_C_FLAGS="-march=native -O2 -pipe" -DCMAKE_INSTALL_PREFIX=/usr
```

Do not pass `-std=` by hand. The sources are C23 (`nullptr`, `constexpr`, `bool`, `[[maybe_unused]]`, enums with a fixed underlying type); the probe in `CMakeLists.txt` finds the option that enables it, stores it in `WC_C23_FLAG`, and errors out with a readable message if the compiler cannot provide C23 (GCC >= 13 or Clang >= 16 required).

Sources include `config.h` unqualified and get the build directory on the include path (`target_include_directories` in `src/CMakeLists.txt`). Generated build artifacts live under `build/` (gitignored), including the generated `config.h`. `cmake/config.h.in` is hand-maintained — any new define that `src/` actually consults needs a matching `#cmakedefine` there, and only the results `src/` reads should be listed.

Release tarballs are produced with `cpack --config build/CPackSourceConfig.cmake` (TGZ/ZIP; exclusions configured in the root `CMakeLists.txt`). Uninstalling is `cmake --build build --target uninstall` (script template in `cmake/cmake_uninstall.cmake.in`).

Two constraints that are easy to break:

- `_GNU_SOURCE` — `src/iminfo.c` calls `strndup()`, which a strict C23 mode hides. It is defined via `target_compile_definitions(… _GNU_SOURCE)` in `src/CMakeLists.txt` rather than in `config.h`, so include ordering cannot break it.
- `-lprocstat` is FreeBSD-only and is found into `PROCSTAT_LIBRARY` in the root `CMakeLists.txt`, linked only into the two executables rather than globally, so it does not get linked into targets that do not need it.

### C23 dialect flag

The CMake probe tries the dialect options in order (`-std=gnu23`, `-std=c23`, `-std=gnu2x`, `-std=c2x`), GNU dialects first because a strict `-std=c23` hides `strndup()`, and passes the winner explicitly on every compile line via `target_compile_options` in `src/CMakeLists.txt` and `tests/CMakeLists.txt`, rather than letting the compiler's default stand in.

Keep it that way. On a compiler that already defaults to C23 (GCC >= 15) an implicit standard leaves `compile_commands.json` with no `-std=` flag, and every libclang-based tool that reads it — clangd, clang-tidy, IWYU — falls back to `gnu17` and marks each `nullptr`/`constexpr`/`bool` in the tree as an error. Passing it always also keeps the build reproducible across compilers whose defaults differ. This is specifically why the build uses `target_compile_options` and *not* `target_compile_features(... c_std_23)`: the compile feature emits a flag only when the compiler's default is older than C23, and spells it `-std=gnu2x` when it does.

`compile_commands.json` in the root is a symlink into `build/`. `.clangd` still pins `-std=gnu23`, now only as a fallback for files outside the compile database (a fresh clone that has not been configured, or a newly added source file).

### Tests

`tests/` has a `check`-based unit test suite (`tests/test_setting.c`, exercising `setting.c` + `hashfun.c`). It only builds when the `check` library (>= 0.15.0) is found (`BUILD_TESTING AND CHECK_FOUND` in the root `CMakeLists.txt`); otherwise it's silently skipped.

```sh
ctest --test-dir build
```

The suite is a single binary, so running one test means using `check`'s own environment variables against it directly (`build/tests/test_setting`):

```sh
CK_RUN_CASE="Removal" ./build/tests/test_setting     # one TCase
CK_RUN_SUITE="Setting" ./build/tests/test_setting    # one Suite
CK_FORK=no gdb ./build/tests/test_setting            # debuggable: no per-test fork
CK_VERBOSITY=verbose ./build/tests/test_setting      # list every test
```

TCase names are declared in `setting_suite()` at the bottom of `tests/test_setting.c` (`Creation`, `Accessors`, `StringMutation`, `ListOps`, `ChildOps`, `Removal`, `Replace`, `Misc`) — add new tests to the matching case rather than growing a new one per test.

New test binaries go in `tests/CMakeLists.txt` with an `add_test()`, following the `test_setting` pattern (list the `src/*.c`/`.h` files under test directly in `_SOURCES` since `tests/` doesn't link against the built binaries).

### Required dependencies (checked in the root `CMakeLists.txt`)

- GTK+ 3 (>= 3.22.0), json-c (>= 0.12.1), libcurl (>= 7.68.0)
- Optional: flickcurl (>= 1.24) for Flickr search — enables `HAVE_FLICKCURL` and, if present, libxml2 (>= 2.9.4) for `HAVE_LIBXML`. Both are conditionally compiled in (`pkg_check_modules(FLICKCURL …)` / `if(FLICKCURL_FOUND …)` in the root and `src/CMakeLists.txt`).
- Optional: `check` (>= 0.15.0) to build/run the unit tests above.
- Optional: `bash-completion` — only to locate `completionsdir`; falls back to `$(sysconfdir)/bash_completion.d`.

## Architecture

### Two binaries sharing one `src/` tree

`src/CMakeLists.txt` lists `WCHANGERD_SOURCES` and `WCHANGERCFG_SOURCES` separately, but both pull from the same pool of `.c`/`.h` files in `src/`. Core logic (settings, JSON I/O, wallpaper-set commands, process/daemon helpers, string/hash/random utilities) is shared; GTK dialog and web-search code (`dlgs*.c`, `treev.c`, `preview.c`, `web*.c`, `thumbs.c`, `imgs.c`) is exclusive to `wchangercfg`. Note the asymmetry in link flags: `wchangerd` links only json-c, so a shared helper must not drag in GTK or libcurl.

### Config/settings pipeline

- `cfgfile.c/.h` — locates and validates the config file (search order above).
- `jsof.c` / `jsfun.c` — JSON object read/write helpers built on json-c.
- `setting.c/.h` — the in-memory `Setting` data model (linked list of key/value settings, with parent/child nesting and a name hash from `hashfun.c`) that both binaries read/write.
- `setts.c/.h` — higher-level settings load/apply logic used by both the daemon loop and the GUI.

`wchangerd` reloads settings from disk on every wallpaper-change cycle (see `chk_setts_ch_wall` in `wcngdmn.c`), so config changes made via `wchangercfg` take effect on the daemon's next interval without a restart — except the interval/time-align state itself, which is why `wchangerd --restart` is recommended after changing those.

### Daemon lifecycle (`wcngdmn.c`, `dmfn.c/.h`, `procfn.c/.h`, `proclist.c/.h`, `procitem.c/.h`)

`wcngdmn.c` is the `wchangerd` entry point: parses CLI args (`cmdfn.c/.h`, `cmdline.c/.h`), checks for/kills an already-running daemon via process-list scanning (`procfn.c` — includes a FreeBSD `procstat` path guarded by the FreeBSD-only `find_library(PROCSTAT_LIBRARY)` check in the root `CMakeLists.txt`), daemonizes (`dmfn_daemonize`), then loops: sleep for the configured interval (optionally time-aligned to the hour, see `check_time_align_val`), reload settings, change wallpaper.

`src/cmdline.c` / `src/cmdline.h` are GNU Gengetopt 2.23 output and are checked in — do not hand-edit them; regenerate from the gengetopt source if the option set changes, and mirror any new flag in the bash-completion scripts (below).

### Wallpaper setting (`wpset.c/.h`)

Central point where a wallpaper is actually applied: builds the configured shell command (with `[F]` substituted for the file path, e.g. default `feh --bg-fill "[F]"` from `DEFAULT_BG_CMD` in `defs.h`) and runs it. Called both from the daemon loop and from `wchangercfg` (startup preview / "test" button / on save).

### Window-manager command presets (`wmsfn.c/.h`, `other/wms.json`)

`wms.json` ships a list of window managers and their default wallpaper-set commands (Xfce needs a non-standard one). `wchangercfg`'s "Select" dialog (`dlgcmd.c/.h`) lets the user pick/override the detected WM's command; `wmsfn.c` handles matching the running WM to a preset.

### Online wallpaper search (`web*.c/.h`, `webwidget*.c/.h`)

Each provider (`webflickr`, `webpixabay`, `webwallhaven`, `webabyss`) implements the same pattern against a shared `WebWidget` GTK widget (`webwidget.c/.h`, `webwidget_c.c` for the client-side glue, `webwidget_s.h` for the shared struct). All use libcurl for HTTP and json-c to parse API responses; `webflickr.c` additionally needs flickcurl/libxml2 and is compiled in only when those are detected. `chquery.c/.h` and `urldata.c/.h` build the search queries/URLs; downloaded images are cached under `PTH_WALLP_DIR`, thumbnails under `PTH_THUMB_DIR`, and per-image metadata under `PTH_IINFO_DIR`/`PTH_QUERY_DIR` (see path defines in `defs.h`).

### GUI structure (`wchanger.c`, `dialogdata.c/.h`, `dlgs*.c/.h`, `treev.c/.h`)

`wchanger.c` is the `wchangercfg` entry point and wires together the main settings window. `dialogdata.c/.h` (`DialogData`) bundles the window's widgets and current settings state, passed around to the get/set-from-widgets helper functions in each dialog source file. `treev.c/.h` manages the GtkTreeView showing the selected wallpaper list; `preview.c/.h` and `thumbs.c/.h` handle image preview/thumbnail generation.

### Shared low-level utilities

`strfun.c`, `nstrings.c`, `hashfun.c`, `randomm.c`, `rwdt.c` (read/write data/time), `fdfn.c` (file/dir helpers), `dirlist.c`, `errs.c` (error reporting) — generic helpers used throughout both binaries.

### Installed data (`other/`)

`other/` is not build output — it holds installed data files, each wired up in `other/CMakeLists.txt`: the bash-completion scripts `wchangerd` and `wchangercfg` (installed to `bashcompdir`; **update these whenever a CLI option is added or renamed**), `wchangercfg.desktop`, `wchangercfg.png`, and the app data `wchangerd.autostart` + `wms.json`.

## Conventions

### Naming/style

- Doxygen-style `/** @file / @brief / @param / @return */` comment blocks on every file and public function declaration in headers, plus a `/*----...----*/` rule between sections.
- Hungarian-ish prefixes on variables/params encode type and direction, e.g. `s_` (string), `i_`/`ui_` (int/uint), `st_` (Setting*), `dd_` (DialogData*), `[in]`/`[out]`/`[in,out]` in doxygen `@param` tags.
- GPLv3 license header at the top of every source/header file, and a copyright comment header on build files too (every `CMakeLists.txt`, `cmake/cmake_uninstall.cmake.in`).
- C23 idiom is the house style: `constexpr` over numeric `#define`, `nullptr` over `NULL`, `bool` returns for predicates, enums with a fixed underlying type (`enum … : int`), `[[nodiscard]]` on allocating constructors and pure query helpers.

### Changelog and releases

`CHANGELOG.md` follows [Keep a Changelog](https://keepachangelog.com/en/1.1.0/) — user-visible changes go under `## [Unreleased]` in an `### Added`/`### Changed`/`### Fixed` section as part of the change itself. Work happens on `release/vX.Y.Z` branches off `master`.
