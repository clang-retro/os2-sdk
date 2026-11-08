#!/usr/bin/env python3
"""os2rc.py - compile OS/2 resources and bind them into a program.

    os2rc.py [-I DIR]... [-D NAME[=VALUE]]... APP.RC APP.EXE
    os2rc.py [-16] [-I DIR]... -r APP.RC APP.RES (compile only)
    os2rc.py APP.RES APP.EXE                    (bind compiled resources)
    os2rc.py -l APP.EXE                         (list a program's resources)

APP.RC is written in the resource language of the OS/2 toolkits:

    #include <os2.h>
    #include "app.h"
    ICON     ID_APP "app.ico"
    POINTER  ID_HAND "hand.ptr"
    BITMAP   ID_LOGO "logo.bmp"
    MENU ID_APP
    BEGIN
        SUBMENU "~File", IDM_FILE
        BEGIN
            MENUITEM "~Open...\\tCtrl+O", IDM_OPEN
            MENUITEM SEPARATOR
            MENUITEM "E~xit", IDM_EXIT, MIS_TEXT, MIA_DISABLED
        END
    END
    ACCELTABLE ID_APP
    BEGIN
        "o", IDM_OPEN, CONTROL
        VK_F3, IDM_EXIT, VIRTUALKEY
    END
    STRINGTABLE
    BEGIN
        IDS_HELLO, "Hello"
    END
    DLGTEMPLATE IDD_ABOUT
    BEGIN
        DIALOG "About", IDD_ABOUT, 10, 10, 160, 80, WS_VISIBLE,
               FCF_TITLEBAR | FCF_SYSMENU | FCF_DLGBORDER
        BEGIN
            CTEXT "Hello", -1, 10, 40, 140, 10
            ENTRYFIELD "", IDC_NAME, 12, 26, 136, 8, ES_MARGIN
            DEFPUSHBUTTON "OK", DID_OK, 55, 6, 50, 14
        END
    END
    RCDATA 1
    BEGIN
        1, 2, "text", 3L
    END

Header files contribute only their #define lines, as with the IBM and
Microsoft resource compilers. The controls are PUSHBUTTON, DEFPUSHBUTTON,
CHECKBOX, AUTOCHECKBOX, RADIOBUTTON, AUTORADIOBUTTON, LTEXT, CTEXT,
RTEXT, GROUPBOX, ENTRYFIELD, LISTBOX, COMBOBOX, MLE, SPINBUTTON, ICON,
CONTAINER, NOTEBOOK, SLIDER, VALUESET and CONTROL "text", id, x, y, cx,
cy, class, style. A DIALOG, WINDOW or FRAME may have PRESPARAMS and
CTLDATA.

Binding adds the resources to a 32-bit (LX) program as one more object
(readable, shared, discardable), sorted by type and id as OS/2 requires,
the way the toolkits' RC does. A 16-bit (NE) program gets one segment
per resource. For a 16-bit program the headers are those of
include/os21x, without __32BIT__ (-16 says so when only compiling). A .RES file here is this tool's own: "OS2RES1\\0"
then, for each resource, type, id (16 bits each), size (32 bits) and the
data.

Copyright (C) clang-retro contributors.
SPDX-License-Identifier: AGPL-3.0-only
"""
import os
import re
import struct
import sys

RT = {"POINTER": 1, "ICON": 1, "BITMAP": 2, "MENU": 3, "DLGTEMPLATE": 4,
      "STRING": 5, "FONTDIR": 6, "FONT": 7, "ACCELTABLE": 8, "RCDATA": 9,
      "MESSAGE": 10, "DLGINCLUDE": 11, "HELPTABLE": 18, "HELPSUBTABLE": 19}

WS_VISIBLE, WS_GROUP, WS_TABSTOP = 0x80000000, 0x00010000, 0x00020000
WS_CLIPSIBLINGS, WS_SAVEBITS = 0x10000000, 0x04000000
FS_DLGBORDER, FCF_SIZEBORDER = 0x00000080, 0x00000008
DT_LEFT, DT_CENTER, DT_RIGHT = 0x0000, 0x0100, 0x0200

# Control statements: window class (WC_*) and default style.
CONTROLS = {
    "PUSHBUTTON":      (3, 0x0000 | WS_VISIBLE | WS_TABSTOP),
    "DEFPUSHBUTTON":   (3, 0x0400 | WS_VISIBLE | WS_TABSTOP),
    "CHECKBOX":        (3, 0x0001 | WS_VISIBLE | WS_TABSTOP),
    "AUTOCHECKBOX":    (3, 0x0002 | WS_VISIBLE | WS_TABSTOP),
    "RADIOBUTTON":     (3, 0x0003 | WS_VISIBLE),
    "AUTORADIOBUTTON": (3, 0x0004 | WS_VISIBLE),
    "LTEXT":           (5, 0x0001 | DT_LEFT | WS_VISIBLE | WS_GROUP),
    "CTEXT":           (5, 0x0001 | DT_CENTER | WS_VISIBLE | WS_GROUP),
    "RTEXT":           (5, 0x0001 | DT_RIGHT | WS_VISIBLE | WS_GROUP),
    "GROUPBOX":        (5, 0x0002 | WS_VISIBLE | WS_GROUP),
    "ICON":            (5, 0x0003 | WS_VISIBLE),
    "ENTRYFIELD":      (6, 0x0004 | WS_VISIBLE | WS_TABSTOP),
    "EDITTEXT":        (6, 0x0004 | WS_VISIBLE | WS_TABSTOP),
    "LISTBOX":         (7, WS_VISIBLE | WS_TABSTOP),
    "COMBOBOX":        (2, 0x0001 | WS_VISIBLE | WS_TABSTOP),
    "MLE":             (10, 0x0002 | WS_VISIBLE | WS_TABSTOP),
    "SPINBUTTON":      (0x20, WS_VISIBLE | WS_TABSTOP),
    "CONTAINER":       (0x25, WS_VISIBLE | WS_TABSTOP),
    "SLIDER":          (0x26, WS_VISIBLE | WS_TABSTOP),
    "VALUESET":        (0x27, WS_VISIBLE | WS_TABSTOP),
    "NOTEBOOK":        (0x28, WS_VISIBLE | WS_TABSTOP),
}
NO_TEXT = {"LISTBOX", "SPINBUTTON", "CONTAINER", "SLIDER", "VALUESET",
           "NOTEBOOK"}

ACCEL_FLAGS = {"CHAR": 0x0001, "VIRTUALKEY": 0x0002, "SCANCODE": 0x0004,
               "SHIFT": 0x0008, "CONTROL": 0x0010, "ALT": 0x0020,
               "LONEKEY": 0x0040, "SYSCOMMAND": 0x0100, "HELP": 0x0200}
LOAD_OPTS = {"PRELOAD", "LOADONCALL", "FIXED", "MOVEABLE", "DISCARDABLE",
             "PURE", "IMPURE", "SEGALIGN"}

MIS_TEXT, MIS_SEPARATOR, MIS_SUBMENU = 0x0001, 0x0004, 0x0010


class RcError(Exception):
    pass


# --- Preprocessor -----------------------------------------------------------

class Preprocessor:
    """#include, #define, #undef and conditionals. Lines of included .h
    files other than directives are dropped."""

    def __init__(self, incdirs, defines, bits16=False):
        self.incdirs = incdirs
        self.macros = {"RC_INVOKED": "1", "__OS2__": "1"}
        if not bits16:
            self.macros["__32BIT__"] = "1"
        self.macros.update(defines)
        self.funcs = set()
        self.out = []

    def find(self, name, here, angle):
        dirs = ([] if angle else [here]) + self.incdirs
        for d in dirs:
            p = os.path.join(d, name)
            if os.path.isfile(p):
                return p
        raise RcError("cannot find include file %s" % name)

    def eval_if(self, expr, where):
        expr = re.sub(r"\bdefined\s*\(\s*(\w+)\s*\)|\bdefined\s+(\w+)",
                      lambda m: "1" if (m.group(1) or m.group(2)) in self.macros
                      else "0", expr)
        expr = self.expand(expr)
        expr = re.sub(r"\b[A-Za-z_]\w*\b", "0", expr)
        try:
            return bool(c_eval(expr))
        except Exception:
            raise RcError("%s: cannot evaluate #if %s" % (where, expr))

    def expand(self, text, depth=0):
        if depth > 32:
            return text

        def rep(m):
            w = m.group(0)
            if w in self.macros:
                return self.expand(self.macros[w], depth + 1)
            return w
        return re.sub(r'"(?:[^"\\]|\\.)*"|\b[A-Za-z_]\w*\b',
                      lambda m: m.group(0) if m.group(0).startswith('"')
                      else rep(m), text)

    def run(self, path, keep=True):
        here = os.path.dirname(os.path.abspath(path))
        with open(path, encoding="latin-1") as f:
            text = f.read()
        text = re.sub(r"\\\n", "", text)
        text = strip_comments(text)
        stack = []   # (taking, taken_any)
        for n, line in enumerate(text.split("\n"), 1):
            where = "%s:%d" % (path, n)
            m = re.match(r"\s*#\s*(\w+)\s*(.*)$", line)
            taking = all(t for t, _ in stack)
            if m:
                d, rest = m.group(1), m.group(2).strip()
                if d in ("ifdef", "ifndef"):
                    v = (rest.split()[0] in self.macros) if rest else False
                    v = v if d == "ifdef" else not v
                    stack.append((v and taking, v))
                elif d == "if":
                    v = taking and self.eval_if(rest, where)
                    stack.append((v, v))
                elif d == "elif":
                    t, any_ = stack.pop()
                    outer = all(t for t, _ in stack)
                    v = (not any_) and outer and self.eval_if(rest, where)
                    stack.append((v, any_ or v))
                elif d == "else":
                    t, any_ = stack.pop()
                    outer = all(t for t, _ in stack)
                    stack.append((outer and not any_, True))
                elif d == "endif":
                    stack.pop()
                elif not taking:
                    pass
                elif d == "include":
                    mm = re.match(r'[<"]([^>"]+)[>"]', rest)
                    if not mm:
                        raise RcError("%s: bad #include" % where)
                    p = self.find(mm.group(1), here, rest.startswith("<"))
                    self.run(p, keep=not p.lower().endswith(".h"))
                elif d == "define":
                    mm = re.match(r"(\w+)(\([^)]*\))?\s*(.*)$", rest)
                    if mm.group(2):
                        self.funcs.add(mm.group(1))
                    else:
                        self.macros[mm.group(1)] = mm.group(3).strip()
                elif d == "undef":
                    self.macros.pop(rest.split()[0], None)
                elif d in ("pragma", "error", "line"):
                    if d == "error":
                        raise RcError("%s: #error %s" % (where, rest))
                continue
            if taking and keep:
                self.out.append((where, self.expand(line)))


def strip_comments(text):
    out, i, n = [], 0, len(text)
    while i < n:
        c = text[i]
        if c == '"':
            j = i + 1
            while j < n and text[j] != '"' and text[j] != "\n":
                j += 2 if text[j] == "\\" else 1
            out.append(text[i:j + 1])
            i = j + 1
        elif text.startswith("/*", i):
            j = text.find("*/", i + 2)
            j = n if j < 0 else j + 2
            out.append("\n" * text.count("\n", i, j) or " ")
            i = j
        elif text.startswith("//", i):
            j = text.find("\n", i)
            i = n if j < 0 else j
        else:
            out.append(c)
            i += 1
    return "".join(out)


CAST_TYPES = {"PSZ", "SZ", "PCSZ", "ULONG", "LONG", "USHORT", "SHORT", "BYTE",
              "UCHAR", "HWND", "MPARAM", "BOOL", "PVOID", "HMODULE",
              "unsigned", "long", "short", "int", "char"}


def c_eval(expr):
    """Evaluate a C integer constant expression."""
    e = re.sub(r"\(\s*(?:P?SZ|PCSZ|ULONG|LONG|USHORT|SHORT|BYTE|UCHAR|"
               r"HWND|MPARAM|BOOL|PVOID|HMODULE|unsigned(?: long| short| int)?|"
               r"long|short|int|char\s*\*)\s*\)", " ", expr)
    e = re.sub(r"\b(0[xX][0-9a-fA-F]+|\d+)[uUlL]+\b", r"\1", e)
    e = re.sub(r"\b0(\d+)\b", lambda m: str(int(m.group(1), 8)), e)
    e = e.replace("&&", " and ").replace("||", " or ")
    e = re.sub(r"!(?!=)", " not ", e)
    if re.search(r"[^\w\s()+\-*/%|&^~<>=!x,.]", e):
        raise ValueError(expr)
    e = e.replace("/", "//")
    return int(eval(e, {"__builtins__": {}}, {}))


OPERATORS = {"|", "&", "+", "-", "*", "/", "%", "~", "^", "!", "(", "<<", ">>",
             "<", ">", "<=", ">=", "==", "!=", "&&", "||", "?", ":"}


# --- Tokens -----------------------------------------------------------------

TOKEN = re.compile(r'\s*(?:(?P<str>"(?:[^"\\]|\\.|"")*")|'
                   r"(?P<chr>'(?:[^'\\]|\\.)')|"
                   r"(?P<word>[A-Za-z_][\w.]*)|"
                   r"(?P<num>0[xX][0-9a-fA-F]+[uUlL]*|\d+[uUlL]*)|"
                   r"(?P<op><<|>>|[-+*/%|&^~(),!]))")


class Lexer:
    def __init__(self, lines):
        self.toks = []
        for where, line in lines:
            pos = 0
            while pos < len(line):
                m = TOKEN.match(line, pos)
                if not m:
                    if line[pos:].strip() == "":
                        break
                    raise RcError("%s: cannot read %r" % (where, line[pos:]))
                pos = m.end()
                kind = m.lastgroup
                self.toks.append((kind, m.group(kind), where))
        self.i = 0

    def peek(self, k=0):
        if self.i + k < len(self.toks):
            return self.toks[self.i + k]
        return (None, None, "end of file")

    def next(self):
        t = self.peek()
        self.i += 1
        return t

    def where(self):
        return self.peek()[2]

    def accept(self, value):
        if self.peek()[1] is not None and str(self.peek()[1]).upper() == value \
                and self.peek()[0] in ("word", "op"):
            self.i += 1
            return True
        return False

    def expect(self, value):
        if not self.accept(value):
            raise RcError("%s: expected %s, found %s"
                          % (self.where(), value, self.peek()[1]))

    def at_end_of_list(self):
        k, v, _ = self.peek()
        return k is None or (k == "word" and v.upper() in ("END", "BEGIN"))


def unquote(s):
    body = s[1:-1].replace('""', '"')
    out, i = [], 0
    esc = {"n": "\n", "t": "\t", "r": "\r", "a": "\a", "\\": "\\", '"': '"',
           "0": "\0"}
    while i < len(body):
        c = body[i]
        if c == "\\" and i + 1 < len(body):
            d = body[i + 1]
            if d in "xX":
                m = re.match(r"[0-9a-fA-F]{1,2}", body[i + 2:])
                out.append(chr(int(m.group(0), 16)))
                i += 2 + len(m.group(0))
                continue
            if d.isdigit():
                m = re.match(r"[0-7]{1,3}", body[i + 1:])
                out.append(chr(int(m.group(0), 8)))
                i += 1 + len(m.group(0))
                continue
            out.append(esc.get(d, d))
            i += 2
            continue
        out.append(c)
        i += 1
    return "".join(out).encode("latin-1")


# --- Parser -----------------------------------------------------------------

class Compiler:
    def __init__(self, lex, basedir, incdirs):
        self.lex = lex
        self.basedir = basedir
        self.incdirs = incdirs
        self.res = {}       # (type, id) -> bytes
        self.strings = {}   # id -> bytes
        self.codepage = 850

    # Expressions: | & + - * / ~ NOT, numbers, characters, parentheses.
    def expr(self, allow_not=False):
        lex = self.lex
        parts = []
        depth = 0
        notmask = 0
        while True:
            k, v, where = lex.peek()
            if k is None:
                break
            if k == "word" and v.upper() == "NOT" and allow_not:
                lex.next()
                notmask |= self.term_value()
                continue
            if k == "op" and v == ",":
                if depth == 0:
                    break
            if k == "op" and v == ")":
                if depth == 0:
                    break
                depth -= 1
            if k == "op" and v == "(":
                depth += 1
            # A string, or an operand right after an operand, starts the
            # next field ("RESOURCE 300 1 ...").
            if k == "str" or (k in ("word", "num", "chr") and depth == 0
                              and parts and parts[-1] not in OPERATORS):
                break
            if k == "word" and v.upper() in ("BEGIN", "END") and depth == 0:
                break
            lex.next()
            if k == "chr":
                parts.append(str(ord(unquote('"' + v[1:-1] + '"').decode("latin-1"))))
            elif k == "word" and v in CAST_TYPES and parts and parts[-1] == "(":
                parts.append(v)  # a cast, which c_eval() drops
            elif k == "word":
                raise RcError("%s: unknown name %s" % (where, v))
            else:
                parts.append(v)
        if not parts and not notmask:
            raise RcError("%s: expected a number" % lex.where())
        value = c_eval(" ".join(parts)) if parts else 0
        if allow_not:
            return value, notmask
        return value

    def term_value(self):
        k, v, where = self.lex.next()
        if k == "op" and v == "(":
            val = self.expr()
            self.lex.expect(")")
            return val
        if k == "num":
            return c_eval(v)
        raise RcError("%s: expected a style after NOT" % where)

    def comma(self):
        return self.lex.accept(",")

    def num(self):
        return self.expr() & 0xFFFFFFFF

    def string(self):
        k, v, where = self.lex.next()
        if k != "str":
            raise RcError("%s: expected a string, found %s" % (where, v))
        s = unquote(v)
        while self.lex.peek()[0] == "str":
            s += unquote(self.lex.next()[1])
        return s

    def file_name(self):
        k, v, where = self.lex.next()
        name = unquote(v).decode("latin-1") if k == "str" else v
        for d in [self.basedir] + self.incdirs:
            p = os.path.join(d, name)
            if os.path.isfile(p):
                return p
        raise RcError("%s: cannot find %s" % (where, name))

    def skip_load_options(self):
        while self.lex.peek()[0] == "word" and \
                self.lex.peek()[1].upper() in LOAD_OPTS:
            self.lex.next()

    def add(self, rtype, rid, data):
        key = (rtype, rid & 0xFFFF)
        if key in self.res:
            raise RcError("%s: resource %d of type %d is defined twice"
                          % (self.lex.where(), rid, rtype))
        self.res[key] = bytes(data)

    def compile(self):
        lex = self.lex
        while lex.peek()[0] is not None:
            k, v, where = lex.next()
            word = v.upper() if k == "word" else None
            if word == "CODEPAGE":
                self.codepage = self.num()
            elif word == "STRINGTABLE":
                self.skip_load_options()
                self.stringtable()
            elif word in ("MESSAGETABLE",):
                self.skip_load_options()
                self.stringtable()
            else:
                lex.i -= 1
                self.resource()
        self.flush_strings()
        return self.res

    def resource(self):
        lex = self.lex
        k, v, where = lex.peek()
        if k == "word" and v.upper() in ("ICON", "POINTER", "BITMAP", "FONT",
                                         "MENU", "DLGTEMPLATE", "ACCELTABLE",
                                         "RCDATA", "HELPTABLE",
                                         "HELPSUBTABLE", "DLGINCLUDE"):
            lex.next()
            kind = v.upper()
            rid = self.num()
            self.skip_load_options()
            if kind in ("ICON", "POINTER", "BITMAP", "FONT"):
                with open(self.file_name(), "rb") as f:
                    self.add(RT[kind], rid, f.read())
            elif kind == "DLGINCLUDE":
                self.add(RT[kind], rid, self.file_name().encode() + b"\0")
            elif kind == "MENU":
                self.add(RT[kind], rid, self.menu())
            elif kind == "DLGTEMPLATE":
                self.add(RT[kind], rid, self.dlgtemplate())
            elif kind == "ACCELTABLE":
                self.add(RT[kind], rid, self.acceltable())
            else:
                self.add(RT[kind], rid, self.rcdata())
            return
        # RESOURCE type id file, or a user type: TYPE id file / BEGIN data END
        if k == "word" and v.upper() == "RESOURCE":
            lex.next()
        rtype = self.num()
        self.comma()
        rid = self.num()
        self.skip_load_options()
        if lex.peek()[1] and str(lex.peek()[1]).upper() == "BEGIN":
            self.add(rtype, rid, self.rcdata())
        else:
            with open(self.file_name(), "rb") as f:
                self.add(rtype, rid, f.read())

    def block(self):
        self.lex.expect("BEGIN")

    def end(self):
        return self.lex.accept("END")

    # STRINGTABLE: bundles of 16 strings.
    def stringtable(self):
        self.block()
        while not self.end():
            sid = self.num()
            self.comma()
            s = self.string()
            if sid in self.strings:
                raise RcError("%s: string %d is defined twice"
                              % (self.lex.where(), sid))
            self.strings[sid] = s

    def flush_strings(self):
        bundles = {}
        for sid, s in self.strings.items():
            bundles.setdefault(sid >> 4, {})[sid & 15] = s
        for b, strs in bundles.items():
            out = bytearray(struct.pack("<H", self.codepage))
            for i in range(16):
                if i in strs:
                    s = strs[i][:254]
                    out += bytes([len(s) + 1]) + s + b"\0"
                else:
                    out += b"\x01\x00"
            self.add(RT["STRING"], b + 1, out)

    # MENU: items of style, attribute and command, text, and submenus.
    def menu(self):
        self.block()
        return self.menu_items()

    def menu_items(self):
        items = []
        while not self.end():
            k, v, where = self.lex.next()
            word = v.upper()
            if word not in ("MENUITEM", "SUBMENU"):
                raise RcError("%s: expected MENUITEM or SUBMENU, found %s"
                              % (where, v))
            if word == "MENUITEM" and self.lex.accept("SEPARATOR"):
                items.append((MIS_SEPARATOR, 0, 0xFFFF, None, None))
                continue
            text = self.string()
            self.comma()
            cmd = self.num() & 0xFFFF
            style, attr = MIS_TEXT, 0
            if self.comma():
                style, notmask = self.expr(allow_not=True)
                style = (style | (MIS_TEXT if not notmask & MIS_TEXT else 0)) \
                    & ~notmask
                if self.comma():
                    attr = self.num()
            sub = None
            if word == "SUBMENU":
                style |= MIS_SUBMENU
                self.block()
                sub = self.menu_items()
            items.append((style & 0xFFFF, attr & 0xFFFF, cmd, text, sub))
        body = bytearray()
        for style, attr, cmd, text, sub in items:
            body += struct.pack("<HHH", style, attr, cmd)
            if not style & MIS_SEPARATOR and text is not None:
                body += text + b"\0"
            if sub is not None:
                body += sub
        return struct.pack("<LHHH", 10 + len(body), self.codepage, 4,
                           len(items)) + body

    # ACCELTABLE
    def acceltable(self):
        self.block()
        entries = []
        while not self.end():
            k, v, where = self.lex.peek()
            if k == "str":
                key = unquote(self.lex.next()[1])
                if key.startswith(b"^") and len(key) == 2:
                    keyval, flags = key[1], ACCEL_FLAGS["CONTROL"]
                else:
                    keyval, flags = key[0], ACCEL_FLAGS["CHAR"]
            else:
                keyval, flags = self.num(), 0
            self.comma()
            cmd = self.num()
            while self.comma():
                w = self.lex.next()[1].upper()
                if w not in ACCEL_FLAGS:
                    raise RcError("%s: unknown accelerator option %s" % (where, w))
                flags |= ACCEL_FLAGS[w]
            if not flags & (0x0001 | 0x0002):
                flags |= ACCEL_FLAGS["CHAR"]
            entries.append(struct.pack("<HHH", flags, keyval & 0xFFFF,
                                       cmd & 0xFFFF))
        return struct.pack("<HH", len(entries), self.codepage) + b"".join(entries)

    # RCDATA: numbers (16-bit, or 32-bit with L) and strings
    def rcdata(self):
        self.block()
        return self.data_list()

    def data_list(self):
        out = bytearray()
        while not self.end():
            k, v, where = self.lex.peek()
            if k == "str":
                out += self.string()
            else:
                long_ = k == "num" and v[-1:] in "lL"
                n = self.num()
                out += struct.pack("<L" if long_ else "<H",
                                   n & (0xFFFFFFFF if long_ else 0xFFFF))
            self.comma()
        return bytes(out)

    # DLGTEMPLATE: a tree of windows, written depth first.
    def dlgtemplate(self):
        self.skip_load_options()
        self.block()
        windows = []
        while not self.end():
            windows.append(self.window())
        return build_dlgtemplate(windows, self.codepage)

    def position(self):
        """x, y, cx, cy"""
        pos = [self.expr()]
        for _ in range(3):
            self.comma()
            pos.append(self.expr())
        return pos

    def window(self):
        k, v, where = self.lex.next()
        word = v.upper() if v else ""
        w = {"presparams": [], "ctldata": None, "children": []}
        if word in ("DIALOG", "FRAME", "WINDOW"):
            w["text"] = self.string()
            self.comma()
            w["id"] = self.num()
            self.comma()
            w["pos"] = self.position()
            if word == "WINDOW":
                self.comma()
                w["cls"] = self.window_class()
            style = WS_VISIBLE if word in ("FRAME", "WINDOW") else \
                (WS_CLIPSIBLINGS | WS_SAVEBITS | FS_DLGBORDER)
            fcf = 0
            if self.comma():
                if self.lex.peek()[1] != ",":  # the style may be left empty
                    val, notmask = self.expr(allow_not=True)
                    style = (style | val) & ~notmask
                    if word == "WINDOW":
                        style = val & ~notmask
                if self.comma():
                    fcf = self.num()
            if word != "WINDOW":
                w["cls"] = 1
                if fcf & FCF_SIZEBORDER:
                    style &= ~FS_DLGBORDER
                w["ctldata"] = struct.pack("<L", fcf)
            w["style"] = style & 0xFFFFFFFF
        elif word == "CONTROL":
            w["text"] = self.text_or_id()
            self.comma()
            w["id"] = self.num()
            self.comma()
            w["pos"] = self.position()
            self.comma()
            w["cls"] = self.window_class()
            w["style"] = WS_VISIBLE
            if self.comma():
                val, notmask = self.expr(allow_not=True)
                w["style"] = (val & ~notmask) & 0xFFFFFFFF
        elif word in CONTROLS:
            cls, defstyle = CONTROLS[word]
            if word in NO_TEXT:
                w["text"] = None
            else:
                w["text"] = self.text_or_id()
                self.comma()
            w["id"] = self.num()
            self.comma()
            w["pos"] = self.position()
            w["cls"] = cls
            style = defstyle
            if self.comma():
                val, notmask = self.expr(allow_not=True)
                style = (style | val) & ~notmask
            w["style"] = style & 0xFFFFFFFF
        else:
            raise RcError("%s: unknown dialog statement %s" % (where, v))
        # PRESPARAMS, CTLDATA and children
        while True:
            if self.lex.accept("PRESPARAMS"):
                pid = self.num()
                vals = bytearray()
                while self.comma():
                    if self.lex.peek()[0] == "str":
                        vals += self.string() + b"\0"
                    else:
                        vals += struct.pack("<L", self.num())
                w["presparams"].append((pid, bytes(vals)))
            elif self.lex.accept("CTLDATA"):
                data = bytearray(w["ctldata"] or b"")
                first = True
                while first or self.comma():
                    first = False
                    if self.lex.peek()[0] == "str":
                        data += self.string() + b"\0"
                    else:
                        data += struct.pack("<H", self.num() & 0xFFFF)
                w["ctldata"] = bytes(data)
            else:
                break
        if self.lex.peek()[1] and str(self.lex.peek()[1]).upper() == "BEGIN":
            self.lex.next()
            while not self.end():
                w["children"].append(self.window())
        return w

    def text_or_id(self):
        if self.lex.peek()[0] == "str":
            return self.string()
        # An icon's text is "#id"
        n = self.num()
        return ("#%d" % n).encode()

    def window_class(self):
        if self.lex.peek()[0] == "str":
            return self.string()
        n = self.num()
        return n & 0xFFFF if (n >> 16) in (0, 0xFFFF) else n


def build_dlgtemplate(windows, codepage):
    flat = []

    def walk(ws):
        for w in ws:
            flat.append(w)
            walk(w["children"])
    walk(windows)
    head = 14
    items = bytearray(30 * len(flat))
    data = bytearray()
    base = head + len(items)

    def count(w):
        return len(w["children"])

    for i, w in enumerate(flat):
        off = base + len(data)
        if isinstance(w["cls"], bytes):
            cch_class, off_class = len(w["cls"]), off
            data += w["cls"] + b"\0"
        else:
            cch_class, off_class = 0, w["cls"]
        text = w["text"] if w["text"] is not None else b""
        off_text = base + len(data)
        data += text + b"\0"
        off_ctl = 0xFFFF
        if w["ctldata"] is not None:
            off_ctl = base + len(data)
            data += w["ctldata"]
        off_pp = 0xFFFF
        if w["presparams"]:
            off_pp = base + len(data)
            body = bytearray()
            for pid, vals in w["presparams"]:
                body += struct.pack("<LL", pid, len(vals)) + vals
            data += struct.pack("<L", len(body)) + body
        x, y, cx, cy = (p & 0xFFFF for p in w["pos"])
        struct.pack_into("<HHHHHHLHHHHHHH", items, 30 * i, 0, count(w),
                         cch_class, off_class & 0xFFFF, len(text), off_text,
                         w["style"], x, y, cx, cy, w["id"] & 0xFFFF, off_pp,
                         off_ctl)
    total = head + len(items) + len(data)
    if total > 0xFFFF:
        raise RcError("dialog template is larger than 64 KiB")
    return struct.pack("<HHHHHHH", total, 0, codepage, head, 1, 0xFFFF, 0) + \
        bytes(items) + bytes(data)


# --- .RES files -------------------------------------------------------------

RES_MAGIC = b"OS2RES1\0"


def write_res(path, res):
    with open(path, "wb") as f:
        f.write(RES_MAGIC)
        for (t, i), data in sorted(res.items()):
            f.write(struct.pack("<HHL", t, i, len(data)) + data)


def read_res(path):
    with open(path, "rb") as f:
        blob = f.read()
    if not blob.startswith(RES_MAGIC):
        raise RcError("%s is not a resource file made by os2rc.py" % path)
    res, pos = {}, len(RES_MAGIC)
    while pos < len(blob):
        t, i, n = struct.unpack_from("<HHL", blob, pos)
        res[(t, i)] = blob[pos + 8:pos + 8 + n]
        pos += 8 + n
    return res


# --- Binding into an LX program ----------------------------------------------

OBJ_READ, OBJ_RSRC, OBJ_DISCARD, OBJ_SHARED, OBJ_BIG = 1, 8, 0x10, 0x20, 0x2000
PAGE = 4096


def u32(b, o):
    return struct.unpack_from("<L", b, o)[0]


def lx_header(exe):
    if exe[:2] == b"MZ":
        lx = u32(exe, 0x3C)
    else:
        lx = 0
    if exe[lx:lx + 2] != b"LX":
        raise RcError("not an LX program (a 32-bit OS/2 .EXE or .DLL)")
    return lx


def list_resources(exe):
    lx = lx_header(exe)
    off, cnt = u32(exe, lx + 0x50), u32(exe, lx + 0x54)
    names = {v: k for k, v in RT.items() if k != "ICON"}
    for n in range(cnt):
        t, i, size, obj, ofs = struct.unpack_from("<HHLHL", exe, lx + off + 14 * n)
        print("%-12s %5d  %6d bytes  object %d +%d"
              % (names.get(t, str(t)), i, size, obj, ofs))


def bind(exe, res):
    """Return the program with RES as its resources (replacing any)."""
    lx = lx_header(exe)
    h = bytearray(exe[lx:lx + 0xAC])
    g = lambda o: u32(h, o)
    if g(0x54):
        raise RcError("the program already has resources")
    shift = g(0x2C)
    nobj, npages = g(0x44), g(0x14)
    objtab, pagetab, rsrctab = g(0x40), g(0x48), g(0x50)
    resname, fpt = g(0x58), g(0x68)
    if not rsrctab:  # no resource table yet: it goes before the names
        rsrctab = resname
    loader_end = objtab + g(0x38)
    fixup_end = fpt + g(0x30)
    datapages = g(0x80)
    # The tables keep their order: object table, page table, resource
    # table, resident names ... (end of loader section), fixup page table,
    # fixup records and imports (end of fixup section).
    if not (objtab < pagetab <= rsrctab <= resname <= loader_end <= fpt):
        raise RcError("unexpected layout of the LX header")
    # Iterated pages are fine when they share the data pages' base.
    if g(0x4C) not in (0, g(0x80)) or g(0x7C) or g(0x60):
        raise RcError("separate iterated pages, page checksums or module "
                      "directives are not supported")
    old_objs = exe[lx + objtab:lx + objtab + 24 * nobj]
    old_pages = exe[lx + pagetab:lx + pagetab + 8 * npages]
    rest_loader = exe[lx + resname:lx + loader_end]
    old_fpt = exe[lx + fpt:lx + fpt + 4 * (npages + 1)]
    rest_fixup = exe[lx + fpt + 4 * (npages + 1):lx + fixup_end]

    # The resource object: every resource, 4-byte aligned, in id order.
    blob = bytearray()
    table = bytearray()
    for (t, i), data in sorted(res.items()):
        table += struct.pack("<HHLHL", t, i, len(data), nobj + 1, len(blob))
        blob += data + b"\0" * (-len(data) % 4)
    nres_pages = max(1, (len(blob) + PAGE - 1) // PAGE)
    objects = old_objs + struct.pack(
        "<LLLLLL", len(blob), 0, OBJ_READ | OBJ_RSRC | OBJ_DISCARD |
        OBJ_SHARED | OBJ_BIG, npages + 1, nres_pages, 0)

    # Where the old pages end, relative to the data pages.
    old_end = 0
    for n in range(npages):
        po, size, fl = struct.unpack_from("<LHH", old_pages, 8 * n)
        if fl in (0, 1, 5):
            old_end = max(old_end, (po << shift) + size)
    align = 1 << shift
    res_start = (old_end + align - 1) & ~(align - 1)
    pages = bytearray(old_pages)
    for n in range(nres_pages):
        size = min(PAGE, len(blob) - n * PAGE)
        pages += struct.pack("<LHH", (res_start + n * PAGE) >> shift, size, 0)

    last = u32(old_fpt, 4 * npages)
    fpt_new = old_fpt + struct.pack("<L", last) * nres_pages

    # New loader and fixup sections.
    new_objtab = objtab
    new_pagetab = new_objtab + len(objects)
    new_rsrctab = new_pagetab + len(pages)
    new_resname = new_rsrctab + len(table)
    d_loader = new_resname - resname
    new_loader_end = loader_end + d_loader
    gap = fpt - loader_end
    new_fpt = new_loader_end + gap
    d_fixup = (new_fpt + len(fpt_new)) - (fpt + len(old_fpt))
    for o in (0x58, 0x5C):  # resident names, entry table
        if g(o):
            struct.pack_into("<L", h, o, g(o) + d_loader)
    for o in (0x6C, 0x70, 0x78):  # fixup records, import modules and procs
        if g(o):
            struct.pack_into("<L", h, o, g(o) + d_fixup)
    struct.pack_into("<L", h, 0x68, new_fpt)
    struct.pack_into("<L", h, 0x48, new_pagetab)
    struct.pack_into("<L", h, 0x50, new_rsrctab)
    struct.pack_into("<L", h, 0x54, len(res))
    struct.pack_into("<L", h, 0x44, nobj + 1)
    struct.pack_into("<L", h, 0x14, npages + nres_pages)
    struct.pack_into("<L", h, 0x38, new_loader_end - new_objtab)
    struct.pack_into("<L", h, 0x30, g(0x30) + 4 * nres_pages)

    head = bytearray(exe[:lx]) + h + bytearray(exe[lx + 0xAC:lx + objtab])
    head += objects + pages + table + rest_loader
    head += exe[lx + loader_end:lx + fpt]
    head += fpt_new + rest_fixup
    between = exe[lx + fixup_end:datapages]
    head += between
    new_datapages = (len(head) + align - 1) & ~(align - 1) \
        if datapages % align == 0 else len(head)
    head += b"\0" * (new_datapages - len(head))
    body = bytearray(exe[datapages:datapages + old_end])
    body += b"\0" * (res_start - len(body))
    body += blob
    body += b"\0" * (-len(body) % 4)
    out = head + body
    tail_start = datapages + old_end
    lxh = len(exe[:lx])
    struct.pack_into("<L", out, lxh + 0x80, new_datapages)
    if g(0x4C):
        struct.pack_into("<L", out, lxh + 0x4C, new_datapages)
    moved = len(out) - tail_start
    for o in (0x88, 0x98):  # non-resident names, debug information
        v = u32(h, o)
        if v:
            if v < tail_start:
                raise RcError("non-resident names before the data pages are "
                              "not supported")
            struct.pack_into("<L", out, lxh + o, v + moved)
    out += exe[tail_start:]
    return bytes(out)


# --- Binding into an NE program ----------------------------------------------
#
# A 16-bit (OS/2 1.x) program keeps each resource in a segment of its own,
# the last ones of the segment table, and its resource table lists their
# types and ids, one pair of 16-bit words per segment.

NSDATA, NSMOVE, NSPURE, NSRELOC, NSDISCARD = 1, 0x10, 0x20, 0x100, 0x1000
NSREADONLY, NSDPL3 = 0x80, 0xC00
NE_RES_FLAGS = NSDATA | NSMOVE | NSPURE | NSREADONLY | NSDPL3 | NSDISCARD


def ne_header(exe):
    if exe[:2] != b"MZ":
        return None
    ne = u32(exe, 0x3C)
    return ne if exe[ne:ne + 2] == b"NE" else None


def u16(b, o):
    return struct.unpack_from("<H", b, o)[0]


def list_ne_resources(exe, ne):
    g = lambda o: u16(exe, ne + o)
    names = {v: k for k, v in RT.items() if k != "ICON"}
    cseg, cres, shift = g(0x1C), g(0x34), g(0x32)
    for n in range(cres):
        t, i = struct.unpack_from("<HH", exe, ne + g(0x24) + 4 * n)
        seg = cseg - cres + n
        size = u16(exe, ne + g(0x22) + 8 * seg + 2) or 0x10000
        print("%-12s %5d  %6d bytes  segment %d"
              % (names.get(t, str(t)), i, size, seg + 1))


def bind_ne(exe, ne, res):
    """Return the 16-bit program with RES as its resources."""
    h = bytearray(exe[ne:ne + 0x40])
    g = lambda o: u16(h, o)
    if g(0x34):
        raise RcError("the program already has resources")
    cseg, shift = g(0x1C), g(0x32)
    segtab, rsrctab = g(0x22), g(0x24)
    nrestab, cbnres = u32(h, 0x2C), g(0x20)
    if rsrctab != segtab + 8 * cseg:
        raise RcError("unexpected layout of the NE header")
    tables_end = ne + max(g(0x04) + g(0x06), g(0x2A))
    if cbnres:
        tables_end = max(tables_end, nrestab + cbnres)
    segs = [list(struct.unpack_from("<HHHH", exe, ne + segtab + 8 * n))
            for n in range(cseg)]
    # Each segment's bytes in the file, and its relocations after them.
    chunks = []
    data_end = tables_end
    for sector, size, flags, _ in segs:
        if not sector:
            chunks.append(b"")
            continue
        start = sector << shift
        n = size or 0x10000
        if flags & NSRELOC:
            n += 2 + 8 * u16(exe, start + n)
        chunks.append(exe[start:start + n])
        data_end = max(data_end, start + n)
    if any(sector and sector << shift < tables_end for sector, *_ in segs):
        raise RcError("unexpected layout of the NE program")
    trailer = exe[data_end:]

    items = sorted(res.items())
    for (t, i), data in items:
        if len(data) > 0x10000:
            raise RcError("resource %d of type %d is over 64 KB" % (i, t))
    grow = 12 * len(items)
    head_len = tables_end + grow
    total = head_len + sum(len(c) for c in chunks) + \
        sum(len(d) for _, d in items) + 512 * (cseg + len(items))
    while total >> shift >= 0x10000:
        shift += 1
    align = 1 << shift

    out = bytearray(exe[:ne + segtab + 8 * cseg])
    out[ne:ne + 0x40] = h
    newsegs = bytearray()
    body = bytearray()
    pos = (head_len + align - 1) & ~(align - 1)

    def place(data):
        nonlocal pos
        at = pos
        body.extend(b"\0" * (at - head_len - len(body)))
        body.extend(data)
        pos = (at + len(data) + align - 1) & ~(align - 1)
        return at >> shift

    for (sector, size, flags, minalloc), chunk in zip(segs, chunks):
        sector = place(chunk) if chunk else 0
        newsegs += struct.pack("<HHHH", sector, size, flags, minalloc)
    table = bytearray()
    for (t, i), data in items:
        size = len(data) & 0xFFFF
        newsegs += struct.pack("<HHHH", place(data), size, NE_RES_FLAGS, size)
        table += struct.pack("<HH", t, i)
    out[ne + segtab:] = newsegs
    out += table + exe[ne + rsrctab:tables_end]
    assert len(out) == head_len
    out += body
    out += b"\0" * (-len(out) % align) + trailer

    put = lambda o, v: struct.pack_into("<H", out, ne + o, v)
    put(0x1C, cseg + len(items))
    put(0x34, len(items))
    put(0x32, shift)
    put(0x24, rsrctab + 8 * len(items))
    for o in (0x04, 0x26, 0x28, 0x2A):  # entry, names, modules, imports
        put(o, g(o) + grow)
    if cbnres:
        struct.pack_into("<L", out, ne + 0x2C, nrestab + grow)
    struct.pack_into("<L", out, ne + 0x38, 0)  # no fast-load area
    return bytes(out)


def main(argv):
    incdirs, defines, args = [], {}, []
    compile_only = listing = bits16 = False
    it = iter(argv)
    for a in it:
        if a == "-I":
            incdirs.append(next(it))
        elif a.startswith("-I"):
            incdirs.append(a[2:])
        elif a == "-D" or a.startswith("-D"):
            d = next(it) if a == "-D" else a[2:]
            k, _, v = d.partition("=")
            defines[k] = v or "1"
        elif a == "-r":
            compile_only = True
        elif a == "-l":
            listing = True
        elif a == "-16":
            bits16 = True
        elif a in ("-h", "--help"):
            print(__doc__)
            return 0
        else:
            args.append(a)
    exe = ne = None
    if len(args) == 2 and not compile_only and not listing:
        with open(args[1], "rb") as f:
            exe = f.read()
        ne = ne_header(exe)
        bits16 = bits16 or ne is not None
    here = os.path.dirname(os.path.abspath(__file__))
    for d in (os.path.join(here, "..", "include",
                           "os21x" if bits16 else "os2v2"),
              os.path.join(here, "..", "include")):
        if os.path.isdir(d):
            incdirs.append(d)
    if listing:
        with open(args[0], "rb") as f:
            exe = f.read()
        ne = ne_header(exe)
        if ne is None:
            list_resources(exe)
        else:
            list_ne_resources(exe, ne)
        return 0
    if len(args) != 2:
        print(__doc__.split("\n\n")[1], file=sys.stderr)
        return 2
    src, dst = args
    if src.lower().endswith(".res"):
        res = read_res(src)
    else:
        pp = Preprocessor(incdirs, defines, bits16)
        pp.run(src)
        res = Compiler(Lexer(pp.out), os.path.dirname(os.path.abspath(src)),
                       incdirs).compile()
    if compile_only:
        write_res(dst, res)
        return 0
    out = bind(exe, res) if ne is None else bind_ne(exe, ne, res)
    with open(dst, "wb") as f:
        f.write(out)
    return 0


if __name__ == "__main__":
    try:
        sys.exit(main(sys.argv[1:]))
    except RcError as e:
        print("os2rc: %s" % e, file=sys.stderr)
        sys.exit(1)
