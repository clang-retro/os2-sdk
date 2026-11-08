# OS/2 SDK for clang-retro

C programs for OS/2 with clang-retro's clang, picolibc and JWlink:

| Target | Triple | Output | Runs on |
| --- | --- | --- | --- |
| 16-bit OS/2 1.x | `i8086-pc-os2` | NE program | OS/2 1.x, and 2.x to 4.x and ArcaOS |
| 32-bit OS/2 | `i386-pc-os2` | LX program or DLL | OS/2 2.0 to 4.x, eComStation, ArcaOS |

```sh
clang --target=i386-pc-os2 -Os hello.c -o hello.exe               # 32-bit
clang --target=i386-pc-os2 -mwindows -Os pmhello.c -o pmhello.exe # 32-bit PM
clang --target=i386-pc-os2 -shared -Os mylib.c -o MYLIB.DLL       # 32-bit DLL
clang --target=i8086-pc-os2 -Os hello.c -o hello.exe              # 16-bit, small model
clang --target=i8086-pc-os2 -mcmodel=large -Os app.c -o app.exe   # 16-bit, large model
```

## What's here

- `include/os21x/`: the 16-bit OS/2 1.x API (`os2.h`): types, the Dos
  calls, Vio, Kbd and Mou, and the error codes. Every function is
  `__far __pascal` with far pointers, as in the IBM and Microsoft toolkits.
- `include/os2v2/`: the 32-bit API (`os2.h`): the Dos calls (files and
  extended attributes, large files, memory, threads, semaphores and
  timers, queues, named pipes, sessions, exception handlers, modules and
  resources, national language support, messages, file systems and device
  IOCTLs), the error codes, and the Presentation Manager: windows and
  messages, frames, menus, dialogs, accelerators, the standard controls
  (buttons, entry fields, list boxes, combo boxes, scroll bars, MLEs, spin
  buttons), the clipboard, pointers and cursors (`pmwin.h`); presentation
  spaces, colors, lines, areas, paths, fonts and bitmaps (`pmgpi.h`); the
  window list and profiles (`pmshl.h`); and the file dialog
  (`pmstddlg.h`). `APIENTRY` is the C convention, which is what OS/2's
  `_System` is. The video, keyboard and mouse calls (Vio, Kbd, Mou), which
  are 16-bit on every OS/2, are in `bsesub.h` (see below).
- `lib/call16.c`: the thunk from 32-bit code into the 16-bit API, built
  into `libos2.a` with a 16-bit stub for each call of `bsesub.h`.
- `lib/os2-16.lnk`, `lib/os2-32.lnk`: `import` directives for every entry
  point of the OS/2 system DLLs (DOSCALLS, VIOCALLS, KBDCALLS, MOUCALLS,
  MSG, NLS, PMWIN, PMGPI and the rest). The driver gives them to the
  linker, which keeps only the imports a program uses. They were made by
  `tools/gen-imports.py` from the DLL export lists in Open Watcom's
  `bld/os2api/lib286` and `lib386` (the names and ordinals only).
- `tools/`: `gen-imports.py` (above), `check-headers.py`, which checks that
  every function the headers declare has an import, and `check-syntax.sh`,
  which compiles `tests/headers.c` against both sets of headers and checks
  the sizes of the structures. CI runs both. `os2rc.py` is a resource
  compiler (see below). `gen-bsesub.py` writes `bsesub.h` and the 16-bit
  stubs.
- `build-sysroot.sh`: builds picolibc with its OS/2 layer, the compiler-rt
  builtins, and installs the headers and import lists:
  `build-sysroot.sh 32 i386-os2` makes the 32-bit sysroot, and
  `build-sysroot.sh 16 i8086-msdos` adds the 16-bit libraries to the DOS
  sysroot, in `lib/os2/<model>`.
- `examples/`: `hello.c` and `sysinfo.c` (both 16 and 32-bit), and two
  32-bit PM programs: `pmhello.c` (a window) and `pmnotes.c` (a menu,
  accelerators, dialogs and strings, from `pmnotes.rc`).

## How programs are built

**32-bit.** clang compiles to ELF objects; JWlink links them with
`format os2 lx`. `crt0.o` is first: OS/2 calls `_start` with the
environment and the command line, which become `environ` and `argv`. The
heap is address space reserved with `DosAllocMem` and committed as
`malloc` grows it. Programs are marked to run in a window (`pmcompatible`);
`-mwindows` marks a PM program. A DLL (`-shared`) starts with `dllcrt0.o`,
is initialized for each process that loads it, and has its data per
process.

**16-bit.** clang compiles to OMF objects for a memory model (small,
medium, compact or large; OS/2 has no tiny or huge model) and JWlink links
them with `format os2`. The code is compiled as for 16-bit Windows: the
stack segment may not be DGROUP, so `va_list` and returned structures are
far. The heap is the rest of DGROUP, grown to 64 KiB with `DosReallocSeg`,
in the small and medium models, and a 64 KiB segment from `DosAllocSeg` in
the compact and large ones.

**Vio, Kbd and Mou from 32-bit code.** These calls are 16-bit far
Pascal functions in VIOCALLS, KBDCALLS and MOUCALLS. Each function of
`bsesub.h` is an inline one that lays out the 16-bit arguments and calls
`__os2_call16` (`lib/call16.c`, in `libos2.a`, which the driver links).
That switches to a 16-bit stack of the thread's own and jumps to the
call's stub, 16-bit code in the program that calls the function (`call far
VIO16WRTTTY`, fixed up by the loader as in a 16-bit program) and comes
back. Pointers become 16:16 ones through the tiles of OS/2's memory, and a
buffer that crosses a 64 KiB boundary is copied for the call.

**Resources.** `tools/os2rc.py` compiles a `.rc` file in the OS/2
toolkits' resource language (STRINGTABLE, MENU, ACCELTABLE, DLGTEMPLATE
with DIALOG, FRAME, WINDOW, CONTROL and the predefined controls, RCDATA,
ICON, POINTER, BITMAP, FONT and user types, with `#include` and `#define`
from the headers) and binds the resources into a 32-bit program, as one
more object of the LX file:

```sh
clang --target=i386-pc-os2 -mwindows -Os pmnotes.c -o pmnotes.exe
python3 tools/os2rc.py -I include/os2v2 examples/pmnotes.rc pmnotes.exe
python3 tools/os2rc.py -l pmnotes.exe     # list the resources
```

`-r` compiles to a `.res` file only, which a later run binds. 16-bit (NE)
programs can't have resources bound yet.

The C library is picolibc with an OS/2 layer (`libos/os2`): a file
descriptor is the OS/2 file handle, and `\n` is written as `\r\n` to the
console and other devices. Files are binary. Directories are read with
`opendir` and `readdir` (`DosFindFirst`); `getcwd`, `chdir`, `getpid` and
`getppid` work too. `chdir` sets the current directory of a drive but does
not change the current drive.

## Testing

`tests/` boots OS/2 Warp 3 from its two boot diskettes in QEMU and runs
programs from a diskette in drive B: (see `tests/README.md`). The 16-bit
examples pass in all four memory models, and the 32-bit ones pass too.
`tests/dosapi.c` exercises the 32-bit Dos calls (exception handlers,
semaphores, queues, named pipes, timers, NLS, file systems, messages and
extended attributes), `tests/restest.c` reads back the resources
`os2rc.py` bound into it with `DosGetResource`, `tests/posix.c` tries
the directory calls of the C library, and `tests/vio.c` the Vio, Kbd and
Mou calls from 32-bit code; all pass on Warp 3. Warp 3's boot
diskettes have no Presentation Manager, so PM programs are only built.

## Not yet

- 16-bit DLLs, and 16-bit Presentation Manager headers.
- The rarer Vio, Kbd and Mou calls from 32-bit code: `bsesub.h` has 39
  common ones, and a call is added with a line in `tools/gen-bsesub.py`.
- Resources in 16-bit programs.
- `system`, `spawn` and `exec` in the C library.
- C++ (libc++), threads in the C library.

## License

Copyright (C) clang-retro contributors.

The clang-retro OS/2 SDK is free software under the GNU Affero General Public
License, version 3 only (`AGPL-3.0-only`); the full text is in
[LICENSE](LICENSE). If you distribute a program built with it, or let
people use a modified version over a network, you must offer the
program's complete source under the same license.

Each contributor keeps the copyright in their own work and contributes
it under the same license; see [CONTRIBUTING.md](CONTRIBUTING.md).
