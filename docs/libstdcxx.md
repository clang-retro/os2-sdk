# libstdc++ (GCC's C++ library)

The 32-bit OS/2 sysroot (`i386-os2`) can have two C++ standard libraries:

* libc++ (LLVM). This is the default. `build-libcxx.sh` builds it.
* libstdc++ (GCC 15.2.0). `build-libstdcxx.sh` builds it. Add
  `-stdlib=libstdc++` to select it.

Use libstdc++ when a program or a library needs GCC's C++ library. For
example, use it for code that uses GCC extensions (`<ext/...>`,
`__gnu_cxx`).

```sh
clang++ --target=i386-pc-os2 -stdlib=libstdc++ -O2 hello.cpp -o hello.exe
```

16-bit OS/2 does not have libstdc++. It does not have libc++ either.

## Where it is

The files are in the sysroot as a GCC installation:

```
include/c++/15.2.0/              the headers
include/c++/15.2.0/i386-os2/     the headers for the target
lib/gcc/i386-os2/15.2.0/         libstdc++.a, libsupc++.a, libstdc++exp.a
share/licenses/libstdc++/        COPYING3, COPYING.RUNTIME
```

With `-stdlib=libstdc++`, clang uses the newest version in
`lib/gcc/i386-os2`. It adds the include directories of that version, and
it links `libstdc++.a` and `libunwind.a` (the SjLj unwinder of
`build-libcxx.sh`).

A clang that does not have the `-stdlib=libstdc++` support for OS/2 can
use the library with these options:

```sh
S=/path/to/clang-retro/i386-os2
clang++ --target=i386-pc-os2 -O2 -nostdinc++ -nostdlib++ \
  -isystem $S/include/c++/15.2.0 -isystem $S/include/c++/15.2.0/i386-os2 \
  hello.cpp $S/lib/gcc/i386-os2/15.2.0/libstdc++.a $S/lib/libunwind.a \
  -o hello.exe
```

## How it is built

First build the sysroot and libc++ (`build-sysroot.sh 32` and
`build-libcxx.sh`). libstdc++ uses the C library and `libunwind.a` of the
sysroot. Then build libstdc++:

```sh
BIN=/path/to/clang-retro/bin GCC_SRC=~/gcc-15.2.0 \
  sh build-libstdcxx.sh /path/to/clang-retro/i386-os2
```

`GCC_SRC` is an unpacked GCC 15.2.0 release. The script does not change
it. The script does these steps:

1. It copies the parts of GCC that it uses to its build directory.
2. It applies `libstdcxx/gcc-15.2.0.patch` to the copy. The patch has
   small changes for clang. The top of the patch file gives each change.
3. It adds the thread model `c11` (`libstdcxx/gthr-c11.h`).
4. It runs the `configure` script of `libstdc++-v3` with clang as the
   compiler. GCC does not compile anything. Thus the library has the same
   ABI as the programs that clang compiles.
5. It installs the headers and the libraries in the sysroot.

GCC does not have an OS/2 target. The script gives `configure` the host
`i386-pc-elf` with `--with-newlib`. These are the settings for a C
library without an operating system layer. picolibc comes from newlib,
and the objects of the OS/2 target are ELF objects.

`configure` does not find `clock_gettime` and `nanosleep`, because
picolibc does not define `_POSIX_TIMERS` for OS/2. The C library has these
functions. Thus the script sets `_GLIBCXX_USE_CLOCK_MONOTONIC`,
`_GLIBCXX_USE_CLOCK_REALTIME` and `_GLIBCXX_USE_NANOSLEEP`.

## Exceptions

Exceptions use the SjLj unwinder (`runtime/unwind-sjlj.c`), as with
libc++. libstdc++ and libc++ use the same unwinder interface.

## Threads

`libstdcxx/gthr-c11.h` puts GCC's thread interface (`gthr.h`) on the C11
threads of the C library (`<threads.h>`). The C library runs them on the
threads and semaphores of OS/2. With it, these work:

* `std::thread`, `std::jthread`, `std::this_thread`
* `std::mutex`, `std::recursive_mutex`, `std::timed_mutex`,
  `std::call_once`
* `std::condition_variable`, `std::future`, `std::async`

Mutexes and condition variables are made when the object is constructed
and freed when it is destroyed. Thus each one has an OS/2 semaphore only
while it exists.

Programs use emulated TLS (`-femulated-tls`). libstdc++ itself does not
use `thread_local` (`--disable-tls`). It keeps its exception data with the
C library's thread-specific storage.

## Limits

* The library is static. There is no libstdc++ DLL.
* A program can use libc++ or libstdc++, not the two together.
* `std::random_device` uses the `rdseed` or `rdrand` instruction if the
  processor has one. If not, it uses `std::mt19937`.
* The library was compiled and linked into the test program
  (`tests/cxx.cpp`). It was not run on OS/2 yet.

## Licence

libstdc++ is under the GPL 3 with the GCC Runtime Library Exception
(`share/licenses/libstdc++`). A program that clang compiles and links with
libstdc++ can have any licence: clang is not a work based on GCC, so the
compilation is an "Eligible Compilation Process". If you pass on the
sysroot, also make available the source of the library: the GCC 15.2.0
release and `libstdcxx/` of this repository. `libstdcxx/gthr-c11.h` is
under the same licence as libstdc++.
