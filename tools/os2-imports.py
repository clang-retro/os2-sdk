#!/usr/bin/env python3
"""Write linker import directives for the exports of an OS/2 DLL.

    os2-imports.py MYDLL.DLL > mydll.lnk
    clang --target=i8086-pc-os2 app.c -Wl,@mydll.lnk

Reads the resident and nonresident name tables of a 16-bit (NE) or 32-bit
(LX) DLL and prints one JWlink directive per exported name, by ordinal:

    import DLLCOUNT MYDLL.1

Copyright (C) clang-retro contributors.
SPDX-License-Identifier: AGPL-3.0-only
"""
import struct
import sys


def names(data, off, end=None):
    """The (name, ordinal) entries of a name table at off. The first entry
    is the module name (resident) or description (nonresident)."""
    out = []
    while off < len(data) and (end is None or off < end):
        n = data[off]
        if n == 0:
            break
        name = data[off + 1:off + 1 + n].decode("latin-1")
        (ordinal,) = struct.unpack_from("<H", data, off + 1 + n)
        out.append((name, ordinal))
        off += 3 + n
    return out


def main():
    if len(sys.argv) != 2:
        sys.exit("usage: os2-imports.py MYDLL.DLL > mydll.lnk")
    data = open(sys.argv[1], "rb").read()
    (hdr,) = struct.unpack_from("<I", data, 0x3C) if data[:2] == b"MZ" else (0,)
    sig = data[hdr:hdr + 2]
    if sig == b"NE":
        (restab,) = struct.unpack_from("<H", data, hdr + 0x26)
        (nrestab,) = struct.unpack_from("<I", data, hdr + 0x2C)
        (nrsize,) = struct.unpack_from("<H", data, hdr + 0x20)
        resident = names(data, hdr + restab)
    elif sig == b"LX":
        (restab,) = struct.unpack_from("<I", data, hdr + 0x58)
        (nrestab, nrsize) = struct.unpack_from("<II", data, hdr + 0x88)
        resident = names(data, hdr + restab)
    else:
        sys.exit(f"{sys.argv[1]}: not an NE or LX file")
    nonresident = names(data, nrestab, nrestab + nrsize) if nrestab else []
    if not resident:
        sys.exit(f"{sys.argv[1]}: no module name")
    module = resident[0][0]
    for name, ordinal in resident[1:] + nonresident[1:]:
        print(f"import {name} {module}.{ordinal}")


if __name__ == "__main__":
    main()
