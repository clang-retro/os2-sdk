#!/bin/sh
# Add GCC's C++ library (libstdc++ and libsupc++) to the 32-bit OS/2
# sysroot, next to libc++. The library is compiled by clang, not by GCC.
# Programs select it with -stdlib=libstdc++.
#
#   build-libstdcxx.sh OUT
#
#   OUT       i386-os2 sysroot from build-sysroot.sh 32 and build-libcxx.sh
#             (the library uses its C library and its libunwind.a)
#   GCC_SRC   unpacked GCC release (gcc-15.2.0); only libstdc++-v3,
#             libgcc, libiberty, include, config, contrib/relpath.sh and
#             the top-level files are used
#   BIN       directory with clang, clang++, llvm-ar, llvm-ranlib, llvm-nm
#   WORK      build directory (default: ./build-libstdcxx)
#
# The script does not change GCC_SRC. It copies the parts that it uses to
# WORK and changes the copy:
#   * libstdcxx/gcc-VERSION.patch: small changes for clang.
#   * libstdcxx/gthr-c11.h: the thread model "c11", which puts GCC's thread
#     interface on the C library's <threads.h>.
#
# The files are installed as a GCC installation in the sysroot:
#   OUT/include/c++/VERSION/          headers
#   OUT/include/c++/VERSION/i386-os2/ headers for the target
#   OUT/lib/gcc/i386-os2/VERSION/     libstdc++.a, libsupc++.a
#
# docs/libstdcxx.md tells more.
set -e
out=${1:?usage: build-libstdcxx.sh OUT}
: "${GCC_SRC:?set GCC_SRC}" "${BIN:?set BIN}"
here=$(cd "$(dirname "$0")" && pwd)
WORK=${WORK:-$PWD/build-libstdcxx}
mkdir -p "$WORK"
out=$(cd "$out" && pwd) WORK=$(cd "$WORK" && pwd)
GCC_SRC=$(cd "$GCC_SRC" && pwd)
[ -f "$out/lib/libunwind.a" ] ||
  { echo "no libunwind.a in $out: run build-libcxx.sh first" >&2; exit 1; }
VERSION=$(cat "$GCC_SRC/gcc/BASE-VER")
GCCLIB=$out/lib/gcc/i386-os2/$VERSION

# The copy of the GCC sources, with the changes.
src=$WORK/gcc
rm -rf "$src"
mkdir -p "$src/gcc" "$src/contrib"
find "$GCC_SRC" -maxdepth 1 -type f -exec cp {} "$src/" \;
cp -R "$GCC_SRC/libstdc++-v3" "$GCC_SRC/libgcc" "$GCC_SRC/libiberty" \
  "$GCC_SRC/include" "$GCC_SRC/config" "$src/"
cp "$GCC_SRC"/gcc/BASE-VER "$GCC_SRC"/gcc/DATESTAMP "$GCC_SRC"/gcc/DEV-PHASE \
  "$src/gcc/"
cp "$GCC_SRC/contrib/relpath.sh" "$src/contrib/"
patch -d "$src" -p1 -s <"$here/libstdcxx/gcc-$VERSION.patch"
cp "$here/libstdcxx/gthr-c11.h" "$src/libgcc/config/gthr-c11.h"
sed 's|^    posix)	thread_header=gthr-posix.h ;;$|&\
    c11)	thread_header=config/gthr-c11.h ;;|' \
  "$src/libstdc++-v3/configure" >"$WORK/configure.new"
grep -q 'c11)	thread_header=config/gthr-c11.h' "$WORK/configure.new"
mv "$WORK/configure.new" "$src/libstdc++-v3/configure"
chmod +x "$src/libstdc++-v3/configure"

FLAGS="-march=i386"
# libstdc++ asks for the init_priority values that GCC keeps for itself
# (the standard streams are made first), and for GCC's warnings.
WARN="-Wno-error=init-priority-reserved -Wno-init-priority-reserved"
WARN="$WARN -Wno-unknown-warning-option -Wno-unknown-attributes"
WARN="$WARN -Wno-mismatched-tags -Wno-invalid-constexpr"
# The library calls the sized operator delete. _GNU_SOURCE (C++ only; the
# C demangler has its own declarations): picolibc shows clock_gettime and
# nanosleep with it.
WARN="$WARN -fsized-deallocation"

# The compiler commands. libstdc++'s configure reads the thread model from
# "$CXX -v"; the wrapper says c11. clang does not use the sysroot's libc++
# headers or link a C++ library here.
mkdir -p "$WORK/bin"
for tool in cc cxx; do
  case $tool in
  cc) drv=clang extra= ;;
  cxx) drv=clang++ extra="-nostdinc++ -nostdlib++" ;;
  esac
  cat >"$WORK/bin/i386-os2-$tool" <<EOF
#!/bin/sh
if [ "\$*" = -v ]; then
  "$BIN/$drv" --target=i386-pc-os2 --sysroot="$out" -v 2>&1 |
    sed 's/^Thread model: .*/Thread model: c11/' >&2
  exit 0
fi
exec "$BIN/$drv" --target=i386-pc-os2 --sysroot="$out" $extra "\$@"
EOF
  chmod +x "$WORK/bin/i386-os2-$tool"
done

# configure looks for gthr-default.h next to libgcc's gthr.h, where a GCC
# build makes it.
mkdir -p "$WORK/gthr-inc"
cp "$here/libstdcxx/gthr-c11.h" "$WORK/gthr-inc/gthr-default.h"

rm -rf "$WORK/build"
mkdir -p "$WORK/build"
cd "$WORK/build"
# GCC has no OS/2 target. i386-pc-elf with --with-newlib selects the
# settings for a C library without an OS layer that configure knows
# (picolibc comes from newlib). The objects of the OS/2 target are ELF.
"$src/libstdc++-v3/configure" \
  --build="$("$src/config.guess")" --host=i386-pc-elf --with-newlib \
  --prefix=/usr --with-gxx-include-dir=/include/c++/$VERSION \
  --disable-shared --enable-static --disable-multilib --disable-nls \
  --disable-libstdcxx-pch --disable-tls \
  CC="$WORK/bin/i386-os2-cc" CXX="$WORK/bin/i386-os2-cxx" \
  AR="$BIN/llvm-ar" RANLIB="$BIN/llvm-ranlib" NM="$BIN/llvm-nm" \
  CFLAGS="$FLAGS -O2" \
  CXXFLAGS="$FLAGS -O2 -D_GNU_SOURCE $WARN -I$WORK/gthr-inc" \
  >"$WORK/configure.log" 2>&1 || {
  tail -n 40 config.log >&2
  exit 1
}
# configure does not find the clocks and nanosleep: picolibc does not
# define _POSIX_TIMERS for OS/2. The C library has them. It has no usleep,
# and <sys/statvfs.h> but no statvfs (std::filesystem::space).
sed -e 's|^/\* #undef \(_GLIBCXX_USE_CLOCK_MONOTONIC\) \*/$|#define \1 1|' \
  -e 's|^/\* #undef \(_GLIBCXX_USE_CLOCK_REALTIME\) \*/$|#define \1 1|' \
  -e 's|^/\* #undef \(_GLIBCXX_USE_NANOSLEEP\) \*/$|#define \1 1|' \
  -e 's|^#define HAVE_USLEEP 1$|/* #undef HAVE_USLEEP */|' \
  -e 's|^#define HAVE_SYS_STATVFS_H 1$|/* #undef HAVE_SYS_STATVFS_H */|' \
  config.h >config.h.new
[ "$(grep -c '^#define _GLIBCXX_USE_\(CLOCK_MONOTONIC\|CLOCK_REALTIME\|NANOSLEEP\) 1$' config.h.new)" = 3 ]
mv config.h.new config.h
make -j"$(nproc)" >"$WORK/build.log" 2>&1 || {
  grep -n -B2 -A8 'error' "$WORK/build.log" | head -n 80 >&2
  exit 1
}
rm -rf "$WORK/stage"
make install DESTDIR="$WORK/stage" >"$WORK/install.log" 2>&1
mkdir -p "$GCCLIB" "$out/include/c++"
rm -rf "$out/include/c++/$VERSION"
cp -R "$WORK/stage/include/c++/$VERSION" "$out/include/c++/"
mv "$out/include/c++/$VERSION/i386-pc-elf" \
  "$out/include/c++/$VERSION/i386-os2"
cp "$WORK"/stage/usr/lib/*.a "$GCCLIB/"

# The licences: GPL 3 with the GCC Runtime Library Exception.
lic=$out/share/licenses/libstdc++
mkdir -p "$lic"
cp "$GCC_SRC/COPYING3" "$GCC_SRC/COPYING.RUNTIME" "$lic/"

echo "libstdc++ $VERSION installed in $out"
