#!/usr/bin/env python3
"""Check that every function the OS/2 headers declare has an import.

    check-headers.py

Each APIENTRY (and EXPENTRY) function of include/os2v2 must be imported
by lib/os2-32.lnk, and each one of include/os21x by lib/os2-16.lnk (whose
names are upper case, as the __pascal convention makes them). A function
without an import would compile and then fail to link.

Copyright (C) clang-retro contributors.
SPDX-License-Identifier: AGPL-3.0-only
"""
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)

# A function declaration: "TYPE APIENTRY Name(", possibly with the return
# type on the line before. Typedefs of function types are skipped.
DECL = re.compile(r"^(?!\s*typedef)[^#/*\n]*\b(?:APIENTRY|EXPENTRY)\s+(\w+)\s*\(",
                  re.M)


def imports(lnk):
    names = set()
    with open(os.path.join(ROOT, "lib", lnk)) as f:
        for line in f:
            m = re.match(r"import\s+(\S+)\s+\S+", line)
            if m:
                names.add(m.group(1))
    return names


def declared(incdir):
    found = {}
    d = os.path.join(ROOT, "include", incdir)
    for name in sorted(os.listdir(d)):
        if not name.endswith(".h"):
            continue
        with open(os.path.join(d, name)) as f:
            text = f.read()
        for m in DECL.finditer(text):
            found.setdefault(m.group(1), name)
    return found


def main():
    bad = 0
    for incdir, lnk, upper in (("os2v2", "os2-32.lnk", False),
                               ("os21x", "os2-16.lnk", True)):
        have = imports(lnk)
        decls = declared(incdir)
        missing = [(fn, hdr) for fn, hdr in sorted(decls.items())
                   if (fn.upper() if upper else fn) not in have]
        for fn, hdr in missing:
            print("%s/%s: %s has no import in lib/%s" % (incdir, hdr, fn, lnk))
        print("%s: %d functions, %d without an import"
              % (incdir, len(decls), len(missing)))
        bad += len(missing)
    sys.exit(1 if bad else 0)

if __name__ == "__main__":
    main()
