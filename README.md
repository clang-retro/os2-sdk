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
- `include/os2v2/`: the 32-bit API (`os2.h`): the Dos calls, the error codes
  and a part of the Presentation Manager (windows, messages, painting,
  message boxes). `APIENTRY` is the C convention, which is what OS/2's
  `_System` is.
- `lib/os2-16.lnk`, `lib/os2-32.lnk`: `import` directives for every entry
  point of the OS/2 system DLLs (DOSCALLS, VIOCALLS, KBDCALLS, MOUCALLS,
  MSG, NLS, PMWIN, PMGPI and the rest). The driver gives them to the
  linker, which keeps only the imports a program uses. They were made by
  `tools/gen-imports.py` from the DLL export lists in Open Watcom's
  `bld/os2api/lib286` and `lib386` (the names and ordinals only).
- `build-sysroot.sh`: builds picolibc with its OS/2 layer, the compiler-rt
  builtins, and installs the headers and import lists:
  `build-sysroot.sh 32 i386-os2` makes the 32-bit sysroot, and
  `build-sysroot.sh 16 i8086-msdos` adds the 16-bit libraries to the DOS
  sysroot, in `lib/os2/<model>`.
- `examples/`: `hello.c` and `sysinfo.c` (both 16 and 32-bit) and
  `pmhello.c` (a PM window, 32-bit).

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

The C library is picolibc with an OS/2 layer (`libos/os2`): a file
descriptor is the OS/2 file handle, and `\n` is written as `\r\n` to the
console and other devices. Files are binary.

## Testing

`tests/` boots OS/2 Warp 3 from its two boot diskettes in QEMU and runs
programs from a diskette in drive B: (see `tests/README.md`). The 16-bit
examples pass in all four memory models, and the 32-bit ones pass too.

## Not yet

- 16-bit DLLs, and 16-bit Presentation Manager headers.
- From 32-bit code, the 16-bit Vio, Kbd and Mou calls (they need thunks).
- C++ (libc++), threads in the C library.
