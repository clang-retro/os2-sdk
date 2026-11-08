#!/usr/bin/env python3
"""The 16-bit Vio, Kbd and Mou calls for 32-bit programs.

    python3 tools/gen-bsesub.py > include/os2v2/bsesub.h
    python3 tools/gen-bsesub.py --stubs DIR

The first writes the header: an inline function for each call, which
calls it through __os2_call16 (lib/call16.c) and the call's 16-bit stub.
The second writes the stubs, one OMF object each (DIR/VIO16WRTTTY.obj
...), for libos2.a. A stub is a 16-bit code segment with
"call far VIO16WRTTTY; o32 retf", whose far call the loader fixes up.

Each call is listed as (name, parameters). A parameter is (type, name)
for a 16-bit word, or (type, name, size, out) for a pointer: size is a C
expression for the bytes it points to, and out is true when the call
writes there. The call's import is its name in upper case with 16 after
the first three letters (VioWrtTTY: VIO16WRTTTY), from lib/os2-32.lnk.
"""
import os
import re
import struct
import sys

W = "USHORT"
CALLS = [
    # Video
    ("VioWrtTTY", [("PCCH", "pch", "cb", 0), (W, "cb"), ("HVIO", "hvio")]),
    ("VioGetMode", [("PVIOMODEINFO", "pvioModeInfo", "sizeof(VIOMODEINFO)", 1), ("HVIO", "hvio")]),
    ("VioSetMode", [("PVIOMODEINFO", "pvioModeInfo", "sizeof(VIOMODEINFO)", 0), ("HVIO", "hvio")]),
    ("VioGetCurPos", [("PUSHORT", "pusRow", "2", 1), ("PUSHORT", "pusColumn", "2", 1), ("HVIO", "hvio")]),
    ("VioSetCurPos", [(W, "usRow"), (W, "usColumn"), ("HVIO", "hvio")]),
    ("VioGetCurType", [("PVIOCURSORINFO", "pvioCursorInfo", "sizeof(VIOCURSORINFO)", 1), ("HVIO", "hvio")]),
    ("VioSetCurType", [("PVIOCURSORINFO", "pvioCursorInfo", "sizeof(VIOCURSORINFO)", 0), ("HVIO", "hvio")]),
] + [
    ("VioScroll" + d, [(W, "usTopRow"), (W, "usLeftCol"), (W, "usBotRow"), (W, "usRightCol"),
                                      (W, "cbLines"), ("PBYTE", "pCell", "2", 0), ("HVIO", "hvio")])
    for d in ("Up", "Dn", "Lf", "Rt")
] + [
    ("VioWrtCharStr", [("PCCH", "pchStr", "cbStr", 0), (W, "cbStr"), (W, "usRow"), (W, "usColumn"), ("HVIO", "hvio")]),
    ("VioWrtCharStrAtt", [("PCCH", "pch", "cb", 0), (W, "cb"), (W, "usRow"), (W, "usColumn"),
                                          ("PBYTE", "pAttr", "1", 0), ("HVIO", "hvio")]),
    ("VioWrtCellStr", [("PCCH", "pchCellStr", "cb", 0), (W, "cb"), (W, "usRow"), (W, "usColumn"), ("HVIO", "hvio")]),
    ("VioWrtNCell", [("PBYTE", "pCell", "2", 0), (W, "usTimes"), (W, "usRow"), (W, "usColumn"), ("HVIO", "hvio")]),
    ("VioWrtNAttr", [("PBYTE", "pAttr", "1", 0), (W, "usTimes"), (W, "usRow"), (W, "usColumn"), ("HVIO", "hvio")]),
    ("VioWrtNChar", [("PCCH", "pchChar", "1", 0), (W, "usTimes"), (W, "usRow"), (W, "usColumn"), ("HVIO", "hvio")]),
    ("VioReadCellStr", [("PCH", "pchCellStr", "*pcb", 1), ("PUSHORT", "pcb", "2", 1), (W, "usRow"), (W, "usColumn"), ("HVIO", "hvio")]),
    ("VioReadCharStr", [("PCH", "pch", "*pcb", 1), ("PUSHORT", "pcb", "2", 1), (W, "usRow"), (W, "usColumn"), ("HVIO", "hvio")]),
    ("VioGetCp", [(W, "usReserved"), ("PUSHORT", "pIdCodePage", "2", 1), ("HVIO", "hvio")]),
    ("VioSetCp", [(W, "usReserved"), (W, "idCodePage"), ("HVIO", "hvio")]),
    ("VioGetAnsi", [("PUSHORT", "pfAnsi", "2", 1), ("HVIO", "hvio")]),
    ("VioSetAnsi", [(W, "fAnsi"), ("HVIO", "hvio")]),
    # Keyboard
    ("KbdCharIn", [("PKBDKEYINFO", "pkbci", "sizeof(KBDKEYINFO)", 1), (W, "fWait"), ("HKBD", "hkbd")]),
    ("KbdPeek", [("PKBDKEYINFO", "pkbci", "sizeof(KBDKEYINFO)", 1), ("HKBD", "hkbd")]),
    ("KbdStringIn", [("PCH", "pch", "pchIn->cb", 1), ("PSTRINGINBUF", "pchIn", "sizeof(STRINGINBUF)", 1),
                                    (W, "fsWait"), ("HKBD", "hkbd")]),
    ("KbdFlushBuffer", [("HKBD", "hkbd")]),
    ("KbdGetStatus", [("PKBDINFO", "pkbdinfo", "sizeof(KBDINFO)", 1), ("HKBD", "hkbd")]),
    ("KbdSetStatus", [("PKBDINFO", "pkbdinfo", "sizeof(KBDINFO)", 0), ("HKBD", "hkbd")]),
    # Mouse
    ("MouOpen", [("PCSZ", "pszDvrName", "__os2_strsize(pszDvrName)", 0), ("PHMOU", "phmou", "2", 1)]),
    ("MouClose", [("HMOU", "hmou")]),
    ("MouDrawPtr", [("HMOU", "hmou")]),
    ("MouRemovePtr", [("PNOPTRRECT", "pmourtRect", "sizeof(NOPTRRECT)", 0), ("HMOU", "hmou")]),
    ("MouReadEventQue", [("PMOUEVENTINFO", "pmouevEvent", "sizeof(MOUEVENTINFO)", 1),
                                         ("PUSHORT", "pfWait", "2", 0), ("HMOU", "hmou")]),
    ("MouGetNumQueEl", [("PMOUQUEINFO", "pmouqi", "sizeof(MOUQUEINFO)", 1), ("HMOU", "hmou")]),
    ("MouGetEventMask", [("PUSHORT", "pfsEvents", "2", 1), ("HMOU", "hmou")]),
    ("MouSetEventMask", [("PUSHORT", "pfsEvents", "2", 0), ("HMOU", "hmou")]),
    ("MouGetPtrPos", [("PPTRLOC", "pmouLoc", "sizeof(PTRLOC)", 1), ("HMOU", "hmou")]),
    ("MouSetPtrPos", [("PPTRLOC", "pmouLoc", "sizeof(PTRLOC)", 0), ("HMOU", "hmou")]),
    ("MouGetNumButtons", [("PUSHORT", "pcButtons", "2", 1), ("HMOU", "hmou")]),
    ("MouFlushQue", [("HMOU", "hmou")]),
    ("MouGetDevStatus", [("PUSHORT", "pfsDevStatus", "2", 1), ("HMOU", "hmou")]),
    ("MouSetDevStatus", [("PUSHORT", "pfsDevStatus", "2", 0), ("HMOU", "hmou")]),
]

HEAD = r'''/*
 * bsesub.h - the video (VIOCALLS), keyboard (KBDCALLS) and mouse
 * (MOUCALLS) API for 32-bit programs.
 *
 * Copyright (C) clang-retro contributors.
 * SPDX-License-Identifier: AGPL-3.0-only
 *
 * These are 16-bit calls on every OS/2. Each function here is an inline
 * that calls the 16-bit one through __os2_call16 and a 16-bit stub, both
 * in libos2.a: it runs on a 16-bit stack of the thread's own, and its
 * pointers become 16:16 ones. A buffer that crosses a 64 KiB boundary is
 * copied (8 KiB at most, in all, for one call). The thunk needs the
 * program's memory to be below 512 MiB, which it is unless it asks for
 * high memory (OBJ_ANY). Written by tools/gen-bsesub.py.
 */
#ifndef __BSESUB_H
#define __BSESUB_H

#include <os2def.h>

#ifdef __cplusplus
extern "C" {
#endif

/* The 16-bit structures, as they are in memory. */
#pragma pack(push, 2)

/* Video */
typedef struct _VIOMODEINFO {
    USHORT cb;
    UCHAR  fbType;
    UCHAR  color;
    USHORT col;
    USHORT row;
    USHORT hres;
    USHORT vres;
    UCHAR  fmt_ID;
    UCHAR  attrib;
} VIOMODEINFO, *PVIOMODEINFO;
#define VGMT_OTHER        0x01
#define VGMT_GRAPHICS     0x02
#define VGMT_DISABLEBURST 0x04
typedef struct _VIOCURSORINFO {
    USHORT yStart;
    USHORT cEnd;
    USHORT cx;
    USHORT attr; /* 0xffff hides the cursor */
} VIOCURSORINFO, *PVIOCURSORINFO;

/* Keyboard */
typedef struct _KBDKEYINFO {
    UCHAR  chChar;
    UCHAR  chScan;
    UCHAR  fbStatus;
    UCHAR  bNlsShift;
    USHORT fsState;
    ULONG  time;
} KBDKEYINFO, *PKBDKEYINFO;
typedef struct _STRINGINBUF {
    USHORT cb;
    USHORT cchIn;
} STRINGINBUF, *PSTRINGINBUF;
typedef struct _KBDINFO {
    USHORT cb;
    USHORT fsMask;
    USHORT chTurnAround;
    USHORT fsInterim;
    USHORT fsState;
} KBDINFO, *PKBDINFO;
#define IO_WAIT   0
#define IO_NOWAIT 1
#define KEYBOARD_ECHO_ON     0x0001
#define KEYBOARD_ECHO_OFF    0x0002
#define KEYBOARD_BINARY_MODE 0x0004
#define KEYBOARD_ASCII_MODE  0x0008
#define KBDTRF_FINAL_CHAR_IN 0x40

/* Mouse */
typedef struct _MOUEVENTINFO {
    USHORT fs;
    ULONG  time;
    SHORT  row;
    SHORT  col;
} MOUEVENTINFO, *PMOUEVENTINFO;
typedef struct _MOUQUEINFO {
    USHORT cEvents;
    USHORT cmaxEvents;
} MOUQUEINFO, *PMOUQUEINFO;
typedef struct _PTRLOC {
    USHORT row;
    USHORT col;
} PTRLOC, *PPTRLOC;
typedef struct _NOPTRRECT {
    USHORT row, col, cRow, cCol;
} NOPTRRECT, *PNOPTRRECT;
#define MOU_NOWAIT 0
#define MOU_WAIT   1
#define MOUSE_MOTION              0x0001
#define MOUSE_MOTION_WITH_BN1_DOWN 0x0002
#define MOUSE_BN1_DOWN            0x0004
#define MOUSE_MOTION_WITH_BN2_DOWN 0x0008
#define MOUSE_BN2_DOWN            0x0010
#define MOUSE_MOTION_WITH_BN3_DOWN 0x0020
#define MOUSE_BN3_DOWN            0x0040

#pragma pack(pop)

/* The thunk (lib/call16.c, in libos2.a). */
struct __os2_buf16 {
    unsigned short argoff;
    unsigned short size;
    unsigned char  out;
};
unsigned long __os2_ptr16(const void *p);
unsigned __os2_call16(const void *stub, void *args, unsigned nargs,
                      const struct __os2_buf16 *bufs, unsigned nbufs);

static __inline__ unsigned short __os2_strsize(const char *s)
{
    unsigned short n = 1;
    while (*s++)
        n++;
    return n;
}

'''

TAIL = r'''
#ifdef __cplusplus
}
#endif

#endif /* __BSESUB_H */
'''


def import_name(name):
    return name[:3].upper() + "16" + name[3:].upper()


def gen(name, params):
    proto = ", ".join("%s %s" % (p[0], p[1]) for p in params)
    out = ["static __inline__ USHORT %s(%s)" % (name, proto), "{"]
    stub = "__os2_stub_" + import_name(name)
    out.append("    extern const char %s[];" % stub)
    rev = list(reversed(params))  # a far Pascal call pushes the first one first
    fields, init, bufs = [], [], []
    for p in rev:
        if len(p) == 2:
            fields.append("USHORT %s;" % p[1])
            init.append(p[1])
        else:
            fields.append("ULONG %s;" % p[1])
            init.append("(ULONG)%s" % p[1])
    out.append("    struct { %s } __attribute__((packed)) a = { %s };"
               % (" ".join(fields), ", ".join(init)))
    ptrs = [p for p in rev if len(p) == 4]
    if ptrs:
        for p in ptrs:
            size = p[2]
            if size.startswith("*") or "->" in size or "(" in size and not size.startswith("sizeof"):
                # read through a pointer, which may be NULL
                ptr = size.lstrip("*").split("->")[0].split("(")[-1].rstrip(")")
                size = "(%s ? %s : 0)" % (ptr, size)
            bufs.append("{ __builtin_offsetof(__typeof__(a), %s), %s, %d }" % (p[1], size, p[3]))
        out.append("    const struct __os2_buf16 b[] = { %s };" % ", ".join(bufs))
        out.append("    return (USHORT)__os2_call16(%s, &a, sizeof a, b, %d);" % (stub, len(ptrs)))
    else:
        out.append("    return (USHORT)__os2_call16(%s, &a, sizeof a, 0, 0);" % stub)
    out.append("}")
    return "\n".join(out)


def omf_record(kind, data):
    rec = bytes([kind]) + struct.pack("<H", len(data) + 1) + data
    return rec + bytes([-sum(rec) & 0xFF])


def omf_name(s):
    return bytes([len(s)]) + s.encode()


def omf_stub(imp):
    """An OMF object: segment OS2STUB16 (16-bit, class CODE) with
    __os2_stub_IMP: call far IMP; o32 retf."""
    sym = "__os2_stub_" + imp
    code = b"\x9a\0\0\0\0\x66\xcb"
    out = omf_record(0x80, omf_name(sym))                         # THEADR
    out += omf_record(0x96, omf_name("") + omf_name("OS2STUB16") +
                      omf_name("CODE"))                            # LNAMES
    # SEGDEF: byte aligned, public, 16-bit; names 2 and 3
    out += omf_record(0x98, bytes([0x28]) + struct.pack("<H", len(code)) +
                      bytes([2, 3, 1]))
    out += omf_record(0x8C, omf_name(imp) + b"\0")                # EXTDEF
    out += omf_record(0x90, bytes([0, 1]) + omf_name(sym) +
                      struct.pack("<H", 0) + b"\0")               # PUBDEF
    out += omf_record(0xA0, bytes([1]) + struct.pack("<H", 0) + code)  # LEDATA
    # FIXUPP: a 16:16 pointer at offset 1, to external 1, frame of target
    out += omf_record(0x9C, bytes([0xCC, 0x01, 0x56, 0x01]))
    out += omf_record(0x8A, b"\0")                                # MODEND
    return out


def check_imports():
    here = os.path.dirname(os.path.abspath(__file__))
    with open(os.path.join(here, "..", "lib", "os2-32.lnk")) as f:
        known = set(re.findall(r"^import (\w+) ", f.read(), re.M))
    missing = [import_name(c[0]) for c in CALLS if import_name(c[0]) not in known]
    if missing:
        sys.exit("gen-bsesub.py: not in lib/os2-32.lnk: " + " ".join(missing))


def main():
    check_imports()
    if sys.argv[1:2] == ["--stubs"]:
        outdir = sys.argv[2]
        os.makedirs(outdir, exist_ok=True)
        for c in CALLS:
            imp = import_name(c[0])
            with open(os.path.join(outdir, imp + ".obj"), "wb") as f:
                f.write(omf_stub(imp))
        return
    sys.stdout.write(HEAD)
    group = None
    for c in CALLS:
        g = c[0][:3]
        if g != group:
            sys.stdout.write({"Vio": "/* Video */\n", "Kbd": "\n/* Keyboard */\n",
                              "Mou": "\n/* Mouse */\n"}[g])
            group = g
        sys.stdout.write(gen(*c) + "\n")
    sys.stdout.write(TAIL)


if __name__ == "__main__":
    main()
