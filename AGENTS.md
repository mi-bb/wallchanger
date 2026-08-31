# AGENTS.md

Compact guidance for coding agents. Full architecture detail: `CLAUDE.md`. General usage: `README.md`.

## Build

Two executables from one `src/` tree: `wchangerd` (daemon, links only json-c — shared helpers must not pull in GTK or libcurl) and `wchangercfg` (GTK+3 GUI, links GTK/json-c/libcurl).

Autotools:

```sh
./autogen.sh && ./configure && make
./configure CC="gcc" CFLAGS="-march=native -O2 -pipe" --prefix=/usr   # dev builds
make check        # tests; failures in tests/test-suite.log
```

CMake (alternative, out-of-source only):

```sh
cmake --preset default && cmake --build --preset default
ctest --test-dir build
```

- Never pass `-std=` by hand. Sources are C23; the build systems probe the dialect flag themselves (`m4/wc_prog_cc_c23.m4`, matching probe in `CMakeLists.txt`) and append it to `CC`. Keep the two probes in sync — same dialect list, same test program.
- `AC_USE_SYSTEM_EXTENSIONS` must stay before `AC_PROG_CC` in `configure.ac` (strndup needs `_GNU_SOURCE`).
- `cmake/config.h.in` is hand-maintained; every new `AC_DEFINE` that `src/` reads needs a matching `#cmakedefine`.

## Keeping the two build systems in sync (nothing checks this)

When adding sources or dependencies, change in lockstep:

- Sources: `src/Makefile.am` (`wchangerd_SOURCES`/`wchangercfg_SOURCES`) and `src/CMakeLists.txt` (`WCHANGERD_SOURCES`/`WCHANGERCFG_SOURCES`).
- Dependencies: `PKG_CHECK_MODULES` in `configure.ac` ↔ `pkg_check_modules` in `CMakeLists.txt`.
- Version: `AC_INIT` in `configure.ac` ↔ `project(VERSION …)` in `CMakeLists.txt`; sources read it only as `PACKAGE_VERSION`.
- Validate build-system changes with `make distcheck`.

## Generated files — do not hand-edit

`Makefile`, `configure`, `config.h`, `autom4te.cache/`, `build-aux/`, and everything in `m4/` except `wc_prog_cc_c23.m4` are generated. Edit `configure.ac`/`Makefile.am` and rerun `autogen.sh`. `src/cmdline.c`/`.h` are checked-in GNU Gengetopt 2.23 output (generated from `wchangerd.ggo`, which is *not* in the repo) — do not hand-edit casually; keep changes consistent with gengetopt output, and mirror new CLI flags in `other/wchangerd` and `other/wchangercfg` (bash completions).

## Tests

Single `check`-based binary: `tests/test_setting` (builds only when libcheck >= 0.15 is present, else silently skipped). Runs one suite/case via check's env vars:

```sh
CK_RUN_SUITE="Setting" ./tests/test_setting      # build/tests/test_setting under CMake
CK_RUN_CASE="Removal" ./tests/test_setting
CK_FORK=no gdb ./tests/test_setting              # debuggable
```

TCase names are declared in `setting_suite()` at the bottom of `tests/test_setting.c` — add tests to the matching case. New test binaries go in `tests/Makefile.am` under `if HAVE_CHECK` *and* in `tests/CMakeLists.txt` with `add_test()`; list the `src/*.c` files under test directly in `_SOURCES` (no link against built binaries).

## Style and workflow

- C23 house style: `constexpr` over numeric `#define`, `nullptr`, `bool` predicates, `enum … : int`, `[[nodiscard]]` on allocating constructors.
- Doxygen `/** @file/@brief/@param/@return */` blocks on files and public functions; Hungarian-ish prefixes (`s_`, `i_`/`ui_`, `st_`, `dd_`); GPLv3 header on every source file; `/*----*/` rules between sections.
- User-visible changes go under `## [Unreleased]` in `CHANGELOG.md` (Keep a Changelog format) as part of the change.
- `compile_commands.json` at the root is a symlink into `build/`; `.clangd` pins `-std=gnu23` only as a fallback for files absent from the compile database. Do not switch the CMake side to `target_compile_features(... c_std_23)` — it omits the flag when the compiler already defaults to C23, which breaks clangd/clang-tidy.
