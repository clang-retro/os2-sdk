#!/bin/sh
# check-syntax.sh [CLANG]: compile tests/headers.c against the 32-bit and
# the 16-bit headers with any clang (a host clang will do: the OS/2 macros
# and the 16-bit keywords are defined here), with warnings as errors.
#
# Copyright (C) clang-retro contributors.
# SPDX-License-Identifier: AGPL-3.0-only
set -e
here=$(cd "$(dirname "$0")/.." && pwd)
cc=${1:-clang}
flags="-fsyntax-only -ffreestanding -std=c11 -Wall -Wextra -Werror"
$cc -target i386-unknown-none -D__OS2__ -D__32BIT__ $flags \
  -I"$here/include/os2v2" "$here/tests/headers.c"
$cc -target i386-unknown-none -D__OS2__ -D__far= -D__near= -D__pascal= \
  -D__huge= $flags -I"$here/include/os21x" "$here/tests/headers.c"
echo "headers compile"
