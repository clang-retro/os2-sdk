#!/bin/sh
# Build libc++, libc++abi and the unwinder for 32-bit OS/2 into an i386-os2
# sysroot that build-sysroot.sh 32 made:
#
#   OUT/include/c++/v1     headers
#   OUT/lib/libc++.a       C++ library, with threads (<thread>, <mutex>
#                          and the rest, on the C library's <threads.h>)
#   OUT/lib/libc++abi.a    C++ ABI support, with exceptions and RTTI
#   OUT/lib/libunwind.a    the SjLj unwinder (runtime/unwind-sjlj.c)
#
#   build-libcxx.sh OUT
#
#   BIN       directory with clang, llvm-ar and the other LLVM tools
#   LLVM_SRC  clang-retro/llvm checkout
#   WORK      build directory (default: ./build-libcxx)
#
# Programs built with -fno-exceptions or -fno-rtti link against the same
# libraries.
set -e
out=${1:?usage: build-libcxx.sh OUT}
: "${BIN:?set BIN}" "${LLVM_SRC:?set LLVM_SRC}"
here=$(cd "$(dirname "$0")" && pwd)
WORK=${WORK:-$PWD/build-libcxx}
mkdir -p "$WORK"
out=$(cd "$out" && pwd) WORK=$(cd "$WORK" && pwd)
[ -f "$out/lib/libc.a" ] || { echo "no C library in $out: run build-sysroot.sh 32 first" >&2; exit 1; }

flags="-march=i386 --sysroot=$out"
rm -rf "$WORK/libcxx" "$WORK/install"
cmake -G Ninja -S "$LLVM_SRC/runtimes" -B "$WORK/libcxx" \
  -DLLVM_ENABLE_RUNTIMES="libcxx;libcxxabi" \
  -DCMAKE_SYSTEM_NAME=Generic -DCMAKE_BUILD_TYPE=MinSizeRel \
  -DCMAKE_C_COMPILER="$BIN/clang" -DCMAKE_CXX_COMPILER="$BIN/clang++" \
  -DCMAKE_ASM_COMPILER="$BIN/clang" \
  -DCMAKE_C_COMPILER_TARGET=i386-pc-os2 \
  -DCMAKE_CXX_COMPILER_TARGET=i386-pc-os2 \
  -DCMAKE_ASM_COMPILER_TARGET=i386-pc-os2 \
  -DCMAKE_C_FLAGS="$flags" \
  -DCMAKE_CXX_FLAGS="$flags -D_GNU_SOURCE" \
  -DCMAKE_AR="$BIN/llvm-ar" -DCMAKE_RANLIB="$BIN/llvm-ranlib" \
  -DCMAKE_NM="$BIN/llvm-nm" \
  -DCMAKE_TRY_COMPILE_TARGET_TYPE=STATIC_LIBRARY \
  -DCMAKE_INSTALL_PREFIX="$WORK/install" \
  -DLIBCXX_ENABLE_SHARED=OFF -DLIBCXX_ENABLE_EXCEPTIONS=ON \
  -DLIBCXX_ENABLE_RTTI=ON -DLIBCXX_ENABLE_THREADS=ON \
  -DLIBCXX_HAS_C11_THREAD_API=ON \
  -DLIBCXX_ENABLE_MONOTONIC_CLOCK=ON -DLIBCXX_ENABLE_LOCALIZATION=ON \
  -DLIBCXX_ENABLE_WIDE_CHARACTERS=ON -DLIBCXX_ENABLE_UNICODE=ON \
  -DLIBCXX_ENABLE_FILESYSTEM=ON -DLIBCXX_ENABLE_RANDOM_DEVICE=OFF \
  -DLIBCXX_ENABLE_TIME_ZONE_DATABASE=OFF -DLIBCXX_INCLUDE_BENCHMARKS=OFF \
  -DLIBCXX_INCLUDE_TESTS=OFF -DLIBCXX_USE_COMPILER_RT=ON \
  -DLIBCXX_CXX_ABI=libcxxabi -DRUNTIMES_USE_LIBC=picolibc \
  -DLIBCXXABI_ENABLE_SHARED=OFF -DLIBCXXABI_ENABLE_EXCEPTIONS=ON \
  -DLIBCXXABI_ENABLE_THREADS=ON \
  -DLIBCXXABI_BAREMETAL=ON -DLIBCXXABI_HAS_CXA_THREAD_ATEXIT_IMPL=ON \
  -DLIBCXXABI_ENABLE_DEMANGLER=OFF -DLIBCXXABI_NON_DEMANGLING_TERMINATE=ON \
  -DLIBCXXABI_USE_LLVM_UNWINDER=OFF -DLIBCXXABI_USE_COMPILER_RT=ON \
  -DLIBCXXABI_INCLUDE_TESTS=OFF >"$WORK/libcxx.log" 2>&1 ||
  { tail -20 "$WORK/libcxx.log"; exit 1; }
ninja -C "$WORK/libcxx" install-cxx install-cxxabi >>"$WORK/libcxx.log" 2>&1 ||
  { grep -E 'error|FAILED' "$WORK/libcxx.log" | head -20; exit 1; }
cp "$WORK/install/lib/libc++.a" "$WORK/install/lib/libc++abi.a" "$out/lib/"
mkdir -p "$out/include/c++"
rm -rf "$out/include/c++/v1"
cp -R "$WORK/install/include/c++/v1" "$out/include/c++/v1"
# The unwinder: SjLj function contexts, one list per thread.
"$BIN/clang" --target=i386-pc-os2 $flags -fexceptions -Os -ffunction-sections \
  -c "$here/runtime/unwind-sjlj.c" -o "$WORK/unwind-sjlj.o"
rm -f "$out/lib/libunwind.a"
"$BIN/llvm-ar" rcs "$out/lib/libunwind.a" "$WORK/unwind-sjlj.o"
echo "libc++ installed in $out"
