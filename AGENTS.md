# AGENTS.md

Guidance for coding agents working in this repository. General usage: `README.md`.

## About

Wall Changer is an automatic wallpaper changer for GNU/Linux and FreeBSD, written in C with GTK+3. It builds two executables from one `src/` tree:

- `wchangerd` — background daemon that switches the wallpaper on a configured interval (`--start`, `--stop`, `--restart`, `--once`, `--config FILE`). Links only json-c — shared helpers must not pull in GTK or libcurl.
- `wchangercfg` — GTK+3 configuration GUI (links GTK/json-c/libcurl) for managing the wallpaper list, change interval, per-window-manager wallpaper-set command, and online wallpaper search/download.

Config is read from `~/.config/wchanger/config.json` (falling back to `~/.config/wchanger.json` / `~/.config/wchanger/wchanger.json`), or an explicit path via `--config`.

## Build

CMake (>= 3.22, out-of-source only):

```sh
cmake --preset default && cmake --build --preset default
sudo cmake --install build
ctest --test-dir build
cmake -S . -B build -DCMAKE_C_COMPILER=gcc -DCMAKE_C_FLAGS="-march=native -O2 -pipe" -DCMAKE_INSTALL_PREFIX=/usr   # dev builds
```

Presets (each has matching build and test presets, e.g. `cmake --build --preset debug`, `ctest --preset debug`): `default` = RelWithDebInfo in `./build`; `release` = optimized with the prefix set to `/usr`, in `build/release`; `debug` = unoptimized with `-Wall -Wextra`, in `build/debug`; `asan` = `debug` plus AddressSanitizer/UBSan, in `build/asan` (needs the `libasan`/`libubsan` runtimes). Only `default` uses `./build` itself, which is where the root `compile_commands.json` symlink points.

- Never pass `-std=` by hand. Sources are C23; `CMakeLists.txt` sets `CMAKE_C_STANDARD 23` with `CMAKE_C_EXTENSIONS ON` and CMake emits the flag for every target. A compiler-version check errors out with a readable message if the compiler is too old for C23 (GCC >= 13 or Clang >= 16 required).
- Sources include `config.h` unqualified and get the build directory on the include path (`target_include_directories` in `src/CMakeLists.txt`).
- `cmake/config.h.in` is hand-maintained; every new define that `src/` reads needs a matching `#cmakedefine` there, and only the results `src/` reads should be listed.
- Release tarballs: `cpack --config build/CPackSourceConfig.cmake` (CPack source config in root `CMakeLists.txt`). Uninstall: `cmake --build build --target uninstall` (script template in `cmake/cmake_uninstall.cmake.in`).

Two constraints that are easy to break:

- `_GNU_SOURCE` — `src/gui/image_info.c` calls `strndup()`, which a strict C23 mode hides. It is defined via `target_compile_definitions(… _GNU_SOURCE)` in `src/CMakeLists.txt` rather than in `config.h`, so include ordering cannot break it.
- `-lprocstat` is FreeBSD-only and is found into `PROCSTAT_LIBRARY` in the root `CMakeLists.txt`, linked only into the two executables rather than globally, so it does not get linked into targets that do not need it.

### C23 dialect flag

`CMAKE_C_STANDARD 23` + `CMAKE_C_EXTENSIONS ON` gives `-std=gnu23` (or `-std=gnu2x` on older compilers/CMake), GNU dialect because a strict `-std=c23` hides `strndup()`. Policy `CMP0128` is `NEW` (guaranteed by `cmake_minimum_required(VERSION 3.22)`) so the flag is emitted on every compile line even when the compiler's default already matches.

Keep it that way. On a compiler that already defaults to C23 (GCC >= 15) an implicit standard leaves `compile_commands.json` with no `-std=` flag, and every libclang-based tool that reads it — clangd, clang-tidy, IWYU — falls back to `gnu17` and marks each `nullptr`/`constexpr`/`bool` in the tree as an error. Do not lower the CMake minimum below 3.22, and do not use `target_compile_features(... c_std_23)`.

`compile_commands.json` at the root is a symlink into `build/`; `.clangd` pins `-std=gnu23` only as a fallback for files absent from the compile database.

### Required dependencies (checked in the root `CMakeLists.txt`)

- GTK+ 3 (>= 3.22.0), json-c (>= 0.12.1), libcurl (>= 7.68.0)
- Optional: flickcurl (>= 1.24) for Flickr search — enables `HAVE_FLICKCURL` and, if present, libxml2 (>= 2.9.4) for `HAVE_LIBXML`. Both are conditionally compiled in (`pkg_check_modules(FLICKCURL …)` / `if(FLICKCURL_FOUND …)` in the root and `src/CMakeLists.txt`).
- Optional: `check` (>= 0.15.0) to build/run the unit tests.
- Optional: `bash-completion` — only to locate `completionsdir`; falls back to `$(sysconfdir)/bash_completion.d`.

## Generated files — do not hand-edit

`build/` (including the `config.h` generated into it) is CMake output. `src/daemon/cmdline.c`/`.h` are checked-in GNU Gengetopt 2.23 output (generated from `wchangerd.ggo`, which is *not* in the repo) — do not hand-edit casually; keep changes consistent with gengetopt output, and mirror new CLI flags in `other/wchangerd` and `other/wchangercfg` (bash completions).

## Tests

Two `check`-based binaries (built only when libcheck >= 0.15 is present, else silently skipped):

- `build/tests/test_setting` — `settings/setting_item.c` + `util/hash_djb2.c`.
- `build/tests/test_process` — `src/process/` (name matching, list growth, scanning the live process list, zombie skipping, kill by name). It forks helper children; a child that must die on SIGTERM resets the signal to `SIG_DFL` first, because check's own SIGTERM handler, inherited over `fork()`, kills the whole process group.

Run one suite/case via check's env vars:

```sh
CK_RUN_SUITE="Setting" ./build/tests/test_setting
CK_RUN_CASE="Removal" ./build/tests/test_setting
CK_FORK=no gdb ./build/tests/test_setting              # debuggable
CK_VERBOSITY=verbose ./build/tests/test_setting        # list every test
```

TCase names are declared in `setting_suite()` at the bottom of `tests/test_setting_item.c` (`Creation`, `Accessors`, `StringMutation`, `ListOps`, `ChildOps`, `Removal`, `Replace`, `Misc`) and in `process_suite()` in `tests/test_process.c` (`Matching`, `List`, `Scan`, `Kill`) — add tests to the matching case rather than growing a new one per test. New test binaries go in `tests/CMakeLists.txt` with `add_test()`, gated on `BUILD_TESTING AND CHECK_FOUND` in the root file; list the `src/*.c` files under test directly in `_SOURCES` (no link against built binaries).

## Architecture

### Two binaries sharing one `src/` tree

`src/CMakeLists.txt` lists `WCHANGERD_SOURCES` and `WCHANGERCFG_SOURCES` separately, but both pull from the same pool of `.c`/`.h` files under `src/`, which is split into one directory per subsystem:

| Directory | Contents | Linked into |
|---|---|---|
| `util/` | generic helpers: strings, `NStrings`, `cres`, hash, `RandMem`, error codes, file I/O, file/dir paths | both |
| `settings/` | config file lookup, JSON read/write, the `Setting` model, settings load/update | both |
| `process/` | process-list scanning and kill by name | both |
| `wallpaper/` | applying a wallpaper, window-manager command presets | both |
| `daemon/` | `wchangerd` entry point, daemon start/stop, change loop, CLI parsing | `daemon.c` both, the rest `wchangerd` |
| `web/` | online search: `WebWidget`, providers, HTTP, query cache, thumbnails | `wchangercfg` |
| `gui/` | `wchangercfg` entry point, main window, dialogs, tree view, preview, icons | `wchangercfg` |

`defs.h` (app-wide constants and paths) stays at the top of `src/`.

`util/`, `settings/`, `process/`, `wallpaper/` and `daemon/` end up in `wchangerd`, so they must not include GTK, GLib or libcurl headers. Code that needs GLib (`gui/image_info.c`, `gui/dir_content.c`) lives in `gui/` even when it is not a widget.

Conventions for the directories:

- No nested `CMakeLists.txt`. Every file is listed in `src/CMakeLists.txt` — in one or both source lists, sorted by directory — with its path relative to `src/` (`process/process.c   process/process.h`).
- Only `src/` itself is on the include path (`target_include_directories`), and nothing else is added per directory. So a header is included from another directory with its prefix (`#include "settings/settings.h"`), from a sibling file in the same directory by plain name, and `defs.h`/`config.h` by plain name.
- Doxygen `@file` tags stay bare basenames.
- A new file goes in the directory of the subsystem it serves; a function that only one subsystem uses belongs in that subsystem, as a `static` in its one caller's file where possible.

### Config/settings pipeline (`settings/`)

- `cfg_file.c/.h` — locates and validates the config file (search order above), returns the app data/cache directory paths, creates the cache directories, and manages the autostart desktop file.
- `json_file.c` / `json_functions.c` — JSON object read/write helpers built on json-c.
- `setting_item.c/.h` — the in-memory `Setting` data model (linked list of key/value settings, with parent/child nesting and a name hash from `util/hash_djb2.c`) that both binaries read/write.
- `settings.c/.h` — higher-level settings load/apply logic used by both the daemon loop and the GUI.

`wchangerd` reloads settings from disk on every wallpaper-change cycle (see `chk_setts_ch_wall` in `daemon/wallpaper_change.c`), so config changes made via `wchangercfg` take effect on the daemon's next interval without a restart — except the interval/time-align state itself, which is why `wchangerd --restart` is recommended after changing those.

### Daemon lifecycle (`daemon/`, `process/`)

`daemon/daemon_main.c` is the `wchangerd` entry point: parses CLI args (`daemon/cmd_functions.c/.h`, `daemon/cmdline.c/.h`), checks for/kills an already-running daemon via process-list scanning (`daemon/daemon.c` on top of `process/process.c` — which includes a FreeBSD `procstat` path guarded by the FreeBSD-only `find_library(PROCSTAT_LIBRARY)` check in the root `CMakeLists.txt`), daemonizes (`daemon_daemonize`), then loops in `daemon/wallpaper_change.c`: sleep for the configured interval (optionally time-aligned to the hour, see `check_time_align_val`), reload settings, change wallpaper.

### Wallpaper setting (`wallpaper/wallpaper_set.c/.h`)

Central point where a wallpaper is actually applied: builds the configured shell command (`wallpaper_set_build_command`) (with `[F]` substituted for the file path, e.g. default `feh --bg-fill "[F]"` from `DEFAULT_BG_CMD` in `defs.h`) and runs it. Called both from the daemon loop and from `wchangercfg` (startup preview / "test" button / on save).

### Window-manager command presets (`wallpaper/window_manager_info.c/.h`, `other/wms.json`)

`wms.json` ships a list of window managers and their default wallpaper-set commands (Xfce needs a non-standard one). `wchangercfg`'s "Select" dialog (`gui/wallpaper_set_command_dialog.c/.h`) lets the user pick/override the detected WM's command; `window_manager_info.c` handles matching the running WM to a preset.

### Online wallpaper search (`web/`)

Each provider (`web_flickr.c`, `web_pixabay.c`, `web_wallhaven.c`, `web_abyss.c`) implements the same pattern against a shared `WebWidget` GTK widget (`webwidget.c/.h`, `webwidget_common.c` for the client-side glue, `webwidget_struct.h` for the shared struct). Each provider builds its own search request URL and fetches it with the provider-agnostic `urldata_get_data` (`url_data.c/.h`, libcurl), then parses the response with json-c; `web_flickr.c` instead goes through flickcurl/libxml2 and is compiled in only when those are detected. `search_item.c/.h` holds one search result, `cache_query.c/.h` caches query results, and `thumbnail.c/.h` fetches result thumbnails. Downloaded images are cached under `PTH_WALLP_DIR`, thumbnails under `PTH_THUMB_DIR`, and per-image metadata under `PTH_IINFO_DIR`/`PTH_QUERY_DIR` (see path defines in `defs.h`).

### GUI structure (`gui/`)

`config_main.c` is the `wchangercfg` entry point: `main` and the `GApplication` signal handlers. Its `activate` calls `main_window_create` (`main_window.c/.h`), which builds the main settings window and holds all of its widget builders and event handlers. `dialog_data.c/.h` (`DialogData`) bundles the window's widgets and current settings state, passed around to the get/set-from-widgets helper functions in each dialog source file (`image_dialogs.c`, `message_dialogs.c`, `dialogs_other.c`, `other_settings_dialog.c`, `wallpaper_set_command_dialog.c`). `tree_view.c/.h` manages the GtkTreeView showing the selected wallpaper list, with `image_info.c/.h` as its per-image data; `preview.c/.h` handles the image preview, `dir_content.c/.h` lists, sizes and empties directories, and `icons.c/.h` holds the embedded icons.

### Shared low-level utilities (`util/`)

`string_functions.c`, `nstrings.c`, `cres.c` (create/resize a dynamic array), `hash_djb2.c`, `randomm.c`, `file_io.c` (read/write data/time), `file_dir_functions.c` (file/dir helpers), `errors.c` (error reporting) — generic helpers used throughout both binaries.

### Installed data (`other/`)

`other/` is not build output — it holds installed data files, each wired up in `other/CMakeLists.txt`: the bash-completion scripts `wchangerd` and `wchangercfg` (installed to `bashcompdir`), `wchangercfg.desktop`, `wchangercfg.png`, and the app data `wchangerd.autostart` + `wms.json`.

## Style and workflow

- C23 house style: `constexpr` over numeric `#define`, `nullptr`, `bool` predicates, `enum … : int`, `[[nodiscard]]` on allocating constructors and pure query helpers.
- Doxygen `/** @file/@brief/@param/@return */` blocks on files and public functions, with `[in]`/`[out]`/`[in,out]` on `@param`; `/*----*/` rules between sections.
- Plain, descriptive names for local variables and function parameters (`name`, `settings`, `tree_view`, `path_len`), not Hungarian-style type prefixes (`s_name`, `st_settings`, `gw_tview`, `ui_plen`). Struct fields still carry the old prefixes (`Setting::s_name`, `ProcList::pi_list`); leave them alone unless doing a dedicated rename across every user.
- GPLv3 header on every source file, and a copyright comment header on build files too (every `CMakeLists.txt`, `cmake/cmake_uninstall.cmake.in`).
- `CHANGELOG.md` follows [Keep a Changelog](https://keepachangelog.com/en/1.1.0/): user-visible changes go under `## [Unreleased]` in an `### Added`/`### Changed`/`### Fixed` section as part of the change.
- Work happens on `release/vX.Y.Z` branches off `master`.
