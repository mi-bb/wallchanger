# AGENTS.md

Guidance for coding agents working in this repository. General usage: `README.md`.

## About

Wall Changer is an automatic wallpaper changer for GNU/Linux and FreeBSD, written in C with GTK+3. It builds two executables from one `src/` tree:

- `wchangerd` — background daemon that switches the wallpaper on a configured interval (`--start`, `--stop`, `--restart`, `--once`, `--config FILE`). Links only json-c — shared helpers must not pull in GTK or libcurl.
- `wchangercfg` — GTK+3 configuration GUI (links GTK/json-c/libcurl) for managing the wallpaper list, change interval, per-window-manager wallpaper-set command, and online wallpaper search/download.

Config is read from `~/.config/wchanger/config.json` (falling back to `~/.config/wchanger.json` / `~/.config/wchanger/wchanger.json`), or an explicit path via `--config`.

## Build

CMake (>= 3.21, out-of-source only):

```sh
cmake --preset default && cmake --build --preset default
sudo cmake --install build
ctest --test-dir build
cmake -S . -B build -DCMAKE_C_COMPILER=gcc -DCMAKE_C_FLAGS="-march=native -O2 -pipe" -DCMAKE_INSTALL_PREFIX=/usr   # dev builds
```

Presets: `default` = RelWithDebInfo in `./build`; `release` = optimized with the prefix set to `/usr`; `debug` = unoptimized with `-Wall -Wextra`.

- Never pass `-std=` by hand. Sources are C23; the probe in `CMakeLists.txt` finds the dialect flag (`WC_C23_FLAG`) and it is applied with `target_compile_options` in `src/CMakeLists.txt` and `tests/CMakeLists.txt`. The probe errors out with a readable message if the compiler cannot provide C23 (GCC >= 13 or Clang >= 16 required).
- Sources include `config.h` unqualified and get the build directory on the include path (`target_include_directories` in `src/CMakeLists.txt`).
- `cmake/config.h.in` is hand-maintained; every new define that `src/` reads needs a matching `#cmakedefine` there, and only the results `src/` reads should be listed.
- Release tarballs: `cpack --config build/CPackSourceConfig.cmake` (CPack source config in root `CMakeLists.txt`). Uninstall: `cmake --build build --target uninstall` (script template in `cmake/cmake_uninstall.cmake.in`).

Two constraints that are easy to break:

- `_GNU_SOURCE` — `src/image_info.c` calls `strndup()`, which a strict C23 mode hides. It is defined via `target_compile_definitions(… _GNU_SOURCE)` in `src/CMakeLists.txt` rather than in `config.h`, so include ordering cannot break it.
- `-lprocstat` is FreeBSD-only and is found into `PROCSTAT_LIBRARY` in the root `CMakeLists.txt`, linked only into the two executables rather than globally, so it does not get linked into targets that do not need it.

### C23 dialect flag

The CMake probe tries the dialect options in order (`-std=gnu23`, `-std=c23`, `-std=gnu2x`, `-std=c2x`), GNU dialects first because a strict `-std=c23` hides `strndup()`, and passes the winner explicitly on every compile line rather than letting the compiler's default stand in.

Keep it that way. On a compiler that already defaults to C23 (GCC >= 15) an implicit standard leaves `compile_commands.json` with no `-std=` flag, and every libclang-based tool that reads it — clangd, clang-tidy, IWYU — falls back to `gnu17` and marks each `nullptr`/`constexpr`/`bool` in the tree as an error. Passing it always also keeps the build reproducible across compilers whose defaults differ. Do not switch to `target_compile_features(... c_std_23)`: it emits a flag only when the compiler's default is older than C23, and spells it `-std=gnu2x` when it does.

`compile_commands.json` at the root is a symlink into `build/`; `.clangd` pins `-std=gnu23` only as a fallback for files absent from the compile database.

### Required dependencies (checked in the root `CMakeLists.txt`)

- GTK+ 3 (>= 3.22.0), json-c (>= 0.12.1), libcurl (>= 7.68.0)
- Optional: flickcurl (>= 1.24) for Flickr search — enables `HAVE_FLICKCURL` and, if present, libxml2 (>= 2.9.4) for `HAVE_LIBXML`. Both are conditionally compiled in (`pkg_check_modules(FLICKCURL …)` / `if(FLICKCURL_FOUND …)` in the root and `src/CMakeLists.txt`).
- Optional: `check` (>= 0.15.0) to build/run the unit tests.
- Optional: `bash-completion` — only to locate `completionsdir`; falls back to `$(sysconfdir)/bash_completion.d`.

## Generated files — do not hand-edit

`build/` (including the `config.h` generated into it) is CMake output. `src/cmdline.c`/`.h` are checked-in GNU Gengetopt 2.23 output (generated from `wchangerd.ggo`, which is *not* in the repo) — do not hand-edit casually; keep changes consistent with gengetopt output, and mirror new CLI flags in `other/wchangerd` and `other/wchangercfg` (bash completions).

## Tests

Single `check`-based binary exercising `setting.c` + `hash_djb2.c`: `build/tests/test_setting` (builds only when libcheck >= 0.15 is present, else silently skipped). Runs one suite/case via check's env vars:

```sh
CK_RUN_SUITE="Setting" ./build/tests/test_setting
CK_RUN_CASE="Removal" ./build/tests/test_setting
CK_FORK=no gdb ./build/tests/test_setting              # debuggable
CK_VERBOSITY=verbose ./build/tests/test_setting        # list every test
```

TCase names are declared in `setting_suite()` at the bottom of `tests/test_setting.c` (`Creation`, `Accessors`, `StringMutation`, `ListOps`, `ChildOps`, `Removal`, `Replace`, `Misc`) — add tests to the matching case rather than growing a new one per test. New test binaries go in `tests/CMakeLists.txt` with `add_test()`, gated on `BUILD_TESTING AND CHECK_FOUND` in the root file; list the `src/*.c` files under test directly in `_SOURCES` (no link against built binaries).

## Architecture

### Two binaries sharing one `src/` tree

`src/CMakeLists.txt` lists `WCHANGERD_SOURCES` and `WCHANGERCFG_SOURCES` separately, but both pull from the same pool of `.c`/`.h` files in `src/`. Core logic (settings, JSON I/O, wallpaper-set commands, process/daemon helpers, string/hash/random utilities) is shared; GTK dialog and web-search code (`dlgs*.c`, `treev.c`, `preview.c`, `web*.c`, `thumbs.c`, `icons.c`) is exclusive to `wchangercfg`.

### Config/settings pipeline

- `cfg_file.c/.h` — locates and validates the config file (search order above).
- `json_file.c` / `json_functions.c` — JSON object read/write helpers built on json-c.
- `setting.c/.h` — the in-memory `Setting` data model (linked list of key/value settings, with parent/child nesting and a name hash from `hash_djb2.c`) that both binaries read/write.
- `setts.c/.h` — higher-level settings load/apply logic used by both the daemon loop and the GUI.

`wchangerd` reloads settings from disk on every wallpaper-change cycle (see `chk_setts_ch_wall` in `wcngdmn.c`), so config changes made via `wchangercfg` take effect on the daemon's next interval without a restart — except the interval/time-align state itself, which is why `wchangerd --restart` is recommended after changing those.

### Daemon lifecycle (`wcngdmn.c`, `daemon.c/.h`, `procfn.c/.h`, `proc_list.c/.h`, `proc_item.c/.h`)

`wcngdmn.c` is the `wchangerd` entry point: parses CLI args (`cmdfn.c/.h`, `cmdline.c/.h`), checks for/kills an already-running daemon via process-list scanning (`procfn.c` — includes a FreeBSD `procstat` path guarded by the FreeBSD-only `find_library(PROCSTAT_LIBRARY)` check in the root `CMakeLists.txt`), daemonizes (`dmfn_daemonize`), then loops: sleep for the configured interval (optionally time-aligned to the hour, see `check_time_align_val`), reload settings, change wallpaper.

### Wallpaper setting (`wallpaper_set.c/.h`)

Central point where a wallpaper is actually applied: builds the configured shell command (with `[F]` substituted for the file path, e.g. default `feh --bg-fill "[F]"` from `DEFAULT_BG_CMD` in `defs.h`) and runs it. Called both from the daemon loop and from `wchangercfg` (startup preview / "test" button / on save).

### Window-manager command presets (`wmsfn.c/.h`, `other/wms.json`)

`wms.json` ships a list of window managers and their default wallpaper-set commands (Xfce needs a non-standard one). `wchangercfg`'s "Select" dialog (`dlgcmd.c/.h`) lets the user pick/override the detected WM's command; `wmsfn.c` handles matching the running WM to a preset.

### Online wallpaper search (`web*.c/.h`, `webwidget*.c/.h`)

Each provider (`webflickr`, `webpixabay`, `webwallhaven`, `webabyss`) implements the same pattern against a shared `WebWidget` GTK widget (`webwidget.c/.h`, `webwidget_common.c` for the client-side glue, `webwidget_struct.h` for the shared struct). All use libcurl for HTTP and json-c to parse API responses; `webflickr.c` additionally needs flickcurl/libxml2 and is compiled in only when those are detected. `cache_query.c/.h` and `urldata.c/.h` build the search queries/URLs; downloaded images are cached under `PTH_WALLP_DIR`, thumbnails under `PTH_THUMB_DIR`, and per-image metadata under `PTH_IINFO_DIR`/`PTH_QUERY_DIR` (see path defines in `defs.h`).

### GUI structure (`wchanger.c`, `dialog_data.c/.h`, `dlgs*.c/.h`, `treev.c/.h`)

`wchanger.c` is the `wchangercfg` entry point and wires together the main settings window. `dialog_data.c/.h` (`DialogData`) bundles the window's widgets and current settings state, passed around to the get/set-from-widgets helper functions in each dialog source file. `treev.c/.h` manages the GtkTreeView showing the selected wallpaper list; `preview.c/.h` and `thumbs.c/.h` handle image preview/thumbnail generation.

### Shared low-level utilities

`string_functions.c`, `nstrings.c`, `hash_djb2.c`, `randomm.c`, `file_io.c` (read/write data/time), `file_dir_functions.c` (file/dir helpers), `dir_content.c`, `errors.c` (error reporting) — generic helpers used throughout both binaries.

### Installed data (`other/`)

`other/` is not build output — it holds installed data files, each wired up in `other/CMakeLists.txt`: the bash-completion scripts `wchangerd` and `wchangercfg` (installed to `bashcompdir`), `wchangercfg.desktop`, `wchangercfg.png`, and the app data `wchangerd.autostart` + `wms.json`.

## Style and workflow

- C23 house style: `constexpr` over numeric `#define`, `nullptr`, `bool` predicates, `enum … : int`, `[[nodiscard]]` on allocating constructors and pure query helpers.
- Doxygen `/** @file/@brief/@param/@return */` blocks on files and public functions, with `[in]`/`[out]`/`[in,out]` on `@param`; Hungarian-ish prefixes (`s_`, `i_`/`ui_`, `st_`, `dd_`); `/*----*/` rules between sections.
- GPLv3 header on every source file, and a copyright comment header on build files too (every `CMakeLists.txt`, `cmake/cmake_uninstall.cmake.in`).
- `CHANGELOG.md` follows [Keep a Changelog](https://keepachangelog.com/en/1.1.0/): user-visible changes go under `## [Unreleased]` in an `### Added`/`### Changed`/`### Fixed` section as part of the change.
- Work happens on `release/vX.Y.Z` branches off `master`.
