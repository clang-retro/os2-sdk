#!/bin/sh
# Build the OS/2 libraries:
#
#   build-sysroot.sh 32 OUT    the i386-os2 sysroot (32-bit OS/2 2.x and
#                              later): include/ (picolibc and the OS/2 API),
#                              lib/crt0.o, libc.a, libm.a,
#                              libclang_rt.builtins.a, libos2.a and os2.lnk
#   build-sysroot.sh 16 SYSROOT
#                              the 16-bit OS/2 1.x libraries, into an
#                              i8086-msdos sysroot that dos-sdk built:
#                              include/os2.h ..., lib/os2/os2.lnk and
#                              lib/os2/<model>/{crt0.obj,libc.a,
#                              libclang_rt.builtins.a}
#
#   BIN           directory with clang, llvm-ar and the other LLVM tools
#   LLVM_SRC      clang-retro/llvm checkout (compiler-rt)
#   PICOLIBC_SRC  clang-retro/picolibc checkout
#   DOS_SDK       clang-retro/dos-sdk checkout (16-bit: its builtins script;
#                 default: ../dos-sdk next to this checkout)
#   OS2_MODELS    16-bit memory models (default: small medium compact large;
#                 OS/2 has no tiny or huge model)
#   WORK          build directory (default: ./build-os2-BITS)
set -e
bits=${1:?usage: build-sysroot.sh 16|32 OUT} out=${2:?usage: build-sysroot.sh 16|32 OUT}
: "${BIN:?set BIN}" "${LLVM_SRC:?set LLVM_SRC}" "${PICOLIBC_SRC:?set PICOLIBC_SRC}"
here=$(cd "$(dirname "$0")" && pwd)
WORK=${WORK:-$PWD/build-os2-$bits}
mkdir -p "$out/include" "$out/lib" "$WORK"
out=$(cd "$out" && pwd) WORK=$(cd "$WORK" && pwd)
PATH=$BIN:$PATH
export PATH
# The cross files run the linker as MSDOS_LD (JWlink next to clang).
MSDOS_LD=${MSDOS_LD:-$BIN/jwlink}
export MSDOS_LD

picolibc() { # cross-file build-dir install-dir
  rm -rf "$2" "$3"
  meson setup "$2" "$PICOLIBC_SRC" --cross-file "$1" \
    -Dos-os2=true -Dmultilib=false -Dpicocrt=false -Dsemihost=false \
    -Dtests=false -Dposix-console=true -Dsingle-thread=true \
    -Dthread-local-storage=false -Dspecsdir=none \
    --prefix="$3" >"$2.log" 2>&1 || { tail -20 "$2.log"; exit 1; }
  ninja -C "$2" install >>"$2.log" 2>&1 ||
    { grep -E 'error|FAILED' "$2.log" | head -20; exit 1; }
}

case $bits in
32)
  picolibc "$PICOLIBC_SRC/scripts/cross-clang-i386-os2.txt" \
    "$WORK/picolibc" "$WORK/picolibc-install"
  cp -R "$WORK/picolibc-install/include/." "$out/include/"
  for f in crt0.o dllcrt0.o libc.a libm.a; do
    cp "$WORK/picolibc-install/lib/$f" "$out/lib/"
  done
  # compiler-rt builtins: 64-bit division and the rest, for the 386.
  cmake -G Ninja -S "$LLVM_SRC/compiler-rt/lib/builtins" -B "$WORK/builtins" \
    -DCMAKE_SYSTEM_NAME=Generic -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_C_COMPILER="$BIN/clang" -DCMAKE_ASM_COMPILER="$BIN/clang" \
    -DCMAKE_CXX_COMPILER="$BIN/clang++" \
    -DCMAKE_C_COMPILER_TARGET=i386-pc-os2 \
    -DCMAKE_CXX_COMPILER_TARGET=i386-pc-os2 \
    -DCMAKE_ASM_COMPILER_TARGET=i386-pc-os2 \
    -DCMAKE_C_FLAGS="-march=i386" -DCMAKE_ASM_FLAGS="-march=i386" \
    -DCMAKE_AR="$BIN/llvm-ar" -DCMAKE_RANLIB="$BIN/llvm-ranlib" \
    -DCMAKE_NM="$BIN/llvm-nm" \
    -DCMAKE_TRY_COMPILE_TARGET_TYPE=STATIC_LIBRARY \
    -DCOMPILER_RT_DEFAULT_TARGET_ONLY=ON -DCOMPILER_RT_BAREMETAL_BUILD=ON \
    -DCOMPILER_RT_OS_DIR=os2 -DLLVM_CMAKE_DIR="${LLVM_CMAKE_DIR:-$BIN/../lib/cmake/llvm}" \
    >"$WORK/builtins.log" 2>&1 || { tail -20 "$WORK/builtins.log"; exit 1; }
  ninja -C "$WORK/builtins" >>"$WORK/builtins.log" 2>&1 ||
    { grep -E 'error|FAILED' "$WORK/builtins.log" | head -20; exit 1; }
  cp "$WORK/builtins/lib/os2/libclang_rt.builtins-i386.a" \
     "$out/lib/libclang_rt.builtins.a"
  cp "$here"/include/os2v2/*.h "$out/include/"
  cp "$here/lib/os2-32.lnk" "$out/lib/os2.lnk"
  # libos2.a: the thunk into the 16-bit Vio, Kbd and Mou calls (bsesub.h),
  # and a 16-bit stub, an OMF object, for each of them.
  rm -rf "$WORK/libos2" "$out/lib/libos2.a"
  mkdir -p "$WORK/libos2"
  "$BIN/clang" --target=i386-pc-os2 -march=i386 -Os -Wall -Werror \
    --sysroot="$out" -c "$here/lib/call16.c" -o "$WORK/libos2/call16.o"
  python3 "$here/tools/gen-bsesub.py" --stubs "$WORK/libos2/stubs"
  "$BIN/llvm-ar" rcs "$out/lib/libos2.a" "$WORK/libos2/call16.o" \
    "$WORK/libos2/stubs/"*.obj
  ;;
16)
  DOS_SDK=${DOS_SDK:-$here/../dos-sdk}
  [ -f "$DOS_SDK/scripts/build-builtins.sh" ] ||
    { echo "set DOS_SDK to a clang-retro/dos-sdk checkout" >&2; exit 1; }
  models=${OS2_MODELS:-small medium compact large}
  mkdir -p "$out/lib/os2"
  for m in $models; do
    picolibc "$PICOLIBC_SRC/scripts/cross-clang-i8086-os2-$m.txt" \
      "$WORK/picolibc-$m" "$WORK/picolibc-install-$m"
    mkdir -p "$out/lib/os2/$m"
    cp "$WORK/picolibc-install-$m/lib/crt0.obj" \
       "$WORK/picolibc-install-$m/lib/libc.a" "$out/lib/os2/$m/"
  done
  OS=os2 MODELS=$models SYSROOT=$out WORK=$WORK sh "$DOS_SDK/scripts/build-builtins.sh"
  cp "$here"/include/os21x/*.h "$out/include/"
  cp "$here/lib/os2-16.lnk" "$out/lib/os2/os2.lnk"
  ;;
*)
  echo "usage: build-sysroot.sh 16|32 OUT" >&2; exit 1 ;;
esac
mkdir -p "$out/share/licenses/os2-sdk"
cp "$here/LICENSE" "$out/share/licenses/os2-sdk/LICENSE"
echo "OS/2 $bits-bit libraries installed in $out"
