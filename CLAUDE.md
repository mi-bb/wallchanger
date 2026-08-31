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
./configure CC="gcc" CFLAGS="-march=native -O2 -pipe" --prefix=/usr
```

Do not pass `-std=` by hand. The sources are C23 (`nullptr`, `constexpr`, `bool`, `[[maybe_unused]]`, enums with a fixed underlying type); `WC_PROG_CC_C23` in `m4/wc_prog_cc_c23.m4` probes for the option that enables it and appends it to `CC`, erroring out if the compiler cannot provide C23. Requires Autoconf >= 2.71 / Automake >= 1.16.

Both build systems build out-of-source; sources include `config.h` unqualified and get the build directory on the include path (`AM_CPPFLAGS` in `src/Makefile.am`, `target_include_directories` in `src/CMakeLists.txt`). Nothing is generated into the checkout by the CMake build.

Generated build artifacts (`Makefile`, `configure`, `config.h`, `autom4te.cache/`, `build-aux/`, and everything in `m4/` except `wc_prog_cc_c23.m4`) are gitignored — do not hand-edit them; edit `configure.ac` / `Makefile.am` instead and rerun `autogen.sh`.

CMake (>= 3.21, the first version with the `c_std_23` compile feature) is also supported as an alternative to Autotools (`CMakeLists.txt` / `src/CMakeLists.txt` / `other/CMakeLists.txt` / `tests/CMakeLists.txt` / `CMakePresets.json`, documented in `README.md`); keep both build definitions in sync when adding sources or dependencies. `cmake --preset default` is the quickest way in (`default` = RelWithDebInfo in `./build`, `release` = optimized with `--prefix=/usr`, `debug` = unoptimized with `-Wall -Wextra`). Before releasing a build-system change, check it with `make distcheck`, which exercises the tarball, a VPATH build, `make check` and a staged install/uninstall in one go.

### Keeping the two build systems in sync

Four things have to be changed in lockstep, and nothing checks them automatically:

- **Sources** — `src/Makefile.am` (`wchangerd_SOURCES` / `wchangercfg_SOURCES`) and `src/CMakeLists.txt` (`WCHANGERD_SOURCES` / `WCHANGERCFG_SOURCES`). A shared helper goes into *both* lists in *both* files.
- **`config.h` contents** — Autotools generates `config.h.in` from `configure.ac` via `autoheader`, but `cmake/config.h.in` is hand-maintained. Any new `AC_DEFINE`/`AC_CHECK_HEADERS` result that `src/` actually consults needs a matching `#cmakedefine` there. Both files deliberately carry only the results `src/` reads.
- **Dependency checks** — `PKG_CHECK_MODULES` in `configure.ac` ↔ `pkg_check_modules` in `CMakeLists.txt`.
- **Version** — `AC_INIT` in `configure.ac` and `project(VERSION …)` in `CMakeLists.txt`. Sources read it as `PACKAGE_VERSION` (re-exported as `APP_VER` in `defs.h`), never as a literal.

Two constraints that are easy to break:

- `AC_USE_SYSTEM_EXTENSIONS` must stay *before* `AC_PROG_CC` — `src/iminfo.c` calls `strndup()`, which a strict C23 mode hides without `_GNU_SOURCE`. The CMake build gets the same thing from `target_compile_definitions(… _GNU_SOURCE)`.
- `-lprocstat` is FreeBSD-only and is threaded through `PROCSTAT_LIBS` rather than the global `LIBS`, so it does not get linked into every target on hosts that merely happen to have the library.
- The C23 dialect probe is duplicated in `m4/wc_prog_cc_c23.m4` and `CMakeLists.txt` — same dialect list, same test program, same fallback order. Change one, change the other (see "C23 dialect flag" below).

### C23 dialect flag

Both build systems probe for the C23 dialect option and pass it explicitly on every compile line, rather than letting the compiler's default stand in — `WC_PROG_CC_C23` (`m4/wc_prog_cc_c23.m4`) appends it to `$CC`, and the matching `foreach` probe in `CMakeLists.txt` sets `WC_C23_FLAG`, applied with `target_compile_options` in `src/CMakeLists.txt` and `tests/CMakeLists.txt`. Both try the same list, GNU dialects first (`-std=gnu23`, `-std=c23`, `-std=gnu2x`, `-std=c2x`), because a strict `-std=c23` hides `strndup()`; both accept "no option needed" only as a last resort, and abort with a readable message when the compiler cannot do C23 at all.

Keep it that way. On a compiler that already defaults to C23 (GCC >= 15) an implicit standard leaves `compile_commands.json` with no `-std=` flag, and every libclang-based tool that reads it — clangd, clang-tidy, IWYU — falls back to `gnu17` and marks each `nullptr`/`constexpr`/`bool` in the tree as an error. Passing it always also keeps the build reproducible across compilers whose defaults differ. This is specifically why the CMake side uses `target_compile_options` and *not* `target_compile_features(... c_std_23)`: the compile feature emits a flag only when the compiler's default is older than C23, and spells it `-std=gnu2x` when it does.

`compile_commands.json` in the root is a symlink into `build/`. `.clangd` still pins `-std=gnu23`, now only as a fallback for files outside the compile database (a fresh clone that has not been configured, or a newly added source file).

### Tests

`tests/` has a `check`-based unit test suite (`tests/test_setting.c`, exercising `setting.c` + `hashfun.c`). It only builds if the `check` library (>= 0.15.0) is found by `configure` (`HAVE_CHECK` conditional in `configure.ac`/`tests/Makefile.am`); otherwise it's silently skipped. Under CMake it is gated on `BUILD_TESTING AND CHECK_FOUND`.

```sh
make check                 # Autotools; failures land in tests/test-suite.log
ctest --test-dir build     # CMake equivalent
```

The suite is a single binary, so running one test means using `check`'s own environment variables against it directly (`tests/test_setting` for an Autotools build, `build/tests/test_setting` for a CMake one):

```sh
CK_RUN_CASE="Removal" ./tests/test_setting     # one TCase
CK_RUN_SUITE="Setting" ./tests/test_setting    # one Suite
CK_FORK=no gdb ./tests/test_setting            # debuggable: no per-test fork
CK_VERBOSITY=verbose ./tests/test_setting      # list every test
```

TCase names are declared in `setting_suite()` at the bottom of `tests/test_setting.c` (`Creation`, `Accessors`, `StringMutation`, `ListOps`, `ChildOps`, `Removal`, `Replace`, `Misc`) — add new tests to the matching case rather than growing a new one per test.

New test binaries go in `tests/Makefile.am` under the `if HAVE_CHECK` block *and* in `tests/CMakeLists.txt` with an `add_test()`, following the `test_setting` pattern (list the `src/*.c`/`.h` files under test directly in `_SOURCES` since `tests/` doesn't link against the built binaries).

### Required dependencies (checked in `configure.ac`)

- GTK+ 3 (>= 3.22.0), json-c (>= 0.12.1), libcurl (>= 7.68.0)
- Optional: flickcurl (>= 1.24) for Flickr search — enables `HAVE_FLICKCURL` and, if present, libxml2 (>= 2.9.4) for `HAVE_LIBXML`. Both are conditionally compiled in (`AM_CONDITIONAL(FLICKCURL...)` / `AM_CONDITIONAL(LIBXML...)` in `configure.ac`, `if FLICKCURL` / `if LIBXML` blocks in `src/Makefile.am`).
- Optional: `check` (>= 0.15.0) to build/run the unit tests above.
- Optional: `bash-completion` — only to locate `completionsdir`; falls back to `$(sysconfdir)/bash_completion.d`.

## Architecture

### Two binaries sharing one `src/` tree

`src/Makefile.am` lists `wchangerd_SOURCES` and `wchangercfg_SOURCES` separately, but both pull from the same pool of `.c`/`.h` files in `src/`. Core logic (settings, JSON I/O, wallpaper-set commands, process/daemon helpers, string/hash/random utilities) is shared; GTK dialog and web-search code (`dlgs*.c`, `treev.c`, `preview.c`, `web*.c`, `thumbs.c`, `imgs.c`) is exclusive to `wchangercfg`. Note the asymmetry in link flags: `wchangerd` links only json-c, so a shared helper must not drag in GTK or libcurl.

### Config/settings pipeline

- `cfgfile.c/.h` — locates and validates the config file (search order above).
- `jsof.c` / `jsfun.c` — JSON object read/write helpers built on json-c.
- `setting.c/.h` — the in-memory `Setting` data model (linked list of key/value settings, with parent/child nesting and a name hash from `hashfun.c`) that both binaries read/write.
- `setts.c/.h` — higher-level settings load/apply logic used by both the daemon loop and the GUI.

`wchangerd` reloads settings from disk on every wallpaper-change cycle (see `chk_setts_ch_wall` in `wcngdmn.c`), so config changes made via `wchangercfg` take effect on the daemon's next interval without a restart — except the interval/time-align state itself, which is why `wchangerd --restart` is recommended after changing those.

### Daemon lifecycle (`wcngdmn.c`, `dmfn.c/.h`, `procfn.c/.h`, `proclist.c/.h`, `procitem.c/.h`)

`wcngdmn.c` is the `wchangerd` entry point: parses CLI args (`cmdfn.c/.h`, `cmdline.c/.h`), checks for/kills an already-running daemon via process-list scanning (`procfn.c` — includes a FreeBSD `procstat` path guarded by the FreeBSD-only `AC_SEARCH_LIBS` check in `configure.ac`), daemonizes (`dmfn_daemonize`), then loops: sleep for the configured interval (optionally time-aligned to the hour, see `check_time_align_val`), reload settings, change wallpaper.

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

`other/` is not build output — it holds installed data files, each wired up in `other/Makefile.am` and `other/CMakeLists.txt`: the bash-completion scripts `wchangerd` and `wchangercfg` (installed to `bashcompdir`; **update these whenever a CLI option is added or renamed**), `wchangercfg.desktop`, `wchangercfg.png`, and the app data `wchangerd.autostart` + `wms.json`.

## Conventions

### Naming/style

- Doxygen-style `/** @file / @brief / @param / @return */` comment blocks on every file and public function declaration in headers, plus a `/*----...----*/` rule between sections.
- Hungarian-ish prefixes on variables/params encode type and direction, e.g. `s_` (string), `i_`/`ui_` (int/uint), `st_` (Setting*), `dd_` (DialogData*), `[in]`/`[out]`/`[in,out]` in doxygen `@param` tags.
- GPLv3 license header at the top of every source/header file, and a copyright comment header on build files too (`configure.ac`, every `Makefile.am`/`CMakeLists.txt`, `autogen.sh`).
- C23 idiom is the house style: `constexpr` over numeric `#define`, `nullptr` over `NULL`, `bool` returns for predicates, enums with a fixed underlying type (`enum … : int`), `[[nodiscard]]` on allocating constructors and pure query helpers.

### Changelog and releases

`CHANGELOG.md` follows [Keep a Changelog](https://keepachangelog.com/en/1.1.0/) — user-visible changes go under `## [Unreleased]` in an `### Added`/`### Changed`/`### Fixed` section as part of the change itself. Work happens on `release/vX.Y.Z` branches off `master`.
