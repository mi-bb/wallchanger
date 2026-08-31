# AGENTS.md

Compact guidance for coding agents. Full architecture detail: `CLAUDE.md`. General usage: `README.md`.

## Build

Two executables from one `src/` tree: `wchangerd` (daemon, links only json-c — shared helpers must not pull in GTK or libcurl) and `wchangercfg` (GTK+3 GUI, links GTK/json-c/libcurl).

CMake (>= 3.21, out-of-source only):

```sh
cmake --preset default && cmake --build --preset default
ctest --test-dir build
cmake -S . -B build -DCMAKE_C_COMPILER=gcc -DCMAKE_C_FLAGS="-march=native -O2 -pipe" -DCMAKE_INSTALL_PREFIX=/usr   # dev builds
```

- Never pass `-std=` by hand. Sources are C23; the probe in `CMakeLists.txt` finds the dialect flag (`WC_C23_FLAG`) and it is applied with `target_compile_options` in `src/CMakeLists.txt` and `tests/CMakeLists.txt`.
- `cmake/config.h.in` is hand-maintained; every new define that `src/` reads needs a matching `#cmakedefine` there.
- Release tarballs: `cpack --config build/CPackSourceConfig.cmake` (CPack source config in root `CMakeLists.txt`). Uninstall: `cmake --build build --target uninstall`.

## Generated files — do not hand-edit

`build/` (including the `config.h` generated into it) is CMake output. `src/cmdline.c`/`.h` are checked-in GNU Gengetopt 2.23 output (generated from `wchangerd.ggo`, which is *not* in the repo) — do not hand-edit casually; keep changes consistent with gengetopt output, and mirror new CLI flags in `other/wchangerd` and `other/wchangercfg` (bash completions).

## Tests

Single `check`-based binary: `build/tests/test_setting` (builds only when libcheck >= 0.15 is present, else silently skipped). Runs one suite/case via check's env vars:

```sh
CK_RUN_SUITE="Setting" ./build/tests/test_setting
CK_RUN_CASE="Removal" ./build/tests/test_setting
CK_FORK=no gdb ./build/tests/test_setting              # debuggable
```

TCase names are declared in `setting_suite()` at the bottom of `tests/test_setting.c` — add tests to the matching case. New test binaries go in `tests/CMakeLists.txt` with `add_test()`, gated on `BUILD_TESTING AND CHECK_FOUND` in the root file; list the `src/*.c` files under test directly in `_SOURCES` (no link against built binaries).

## Style and workflow

- C23 house style: `constexpr` over numeric `#define`, `nullptr`, `bool` predicates, `enum … : int`, `[[nodiscard]]` on allocating constructors.
- Doxygen `/** @file/@brief/@param/@return */` blocks on files and public functions; Hungarian-ish prefixes (`s_`, `i_`/`ui_`, `st_`, `dd_`); GPLv3 header on every source file; `/*----*/` rules between sections.
- User-visible changes go under `## [Unreleased]` in `CHANGELOG.md` (Keep a Changelog format) as part of the change.
- `compile_commands.json` at the root is a symlink into `build/`; `.clangd` pins `-std=gnu23` only as a fallback for files absent from the compile database. Do not switch the CMake side to `target_compile_features(... c_std_23)` — it omits the flag when the compiler already defaults to C23, which breaks clangd/clang-tidy.
