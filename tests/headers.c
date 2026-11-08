/*
 * headers.c - compile every OS/2 header, and check the sizes of
 * structures the system fills in. Built by tools/check-syntax.sh with any
 * clang, and by the OS/2 targets themselves.
 *
 * Copyright (C) clang-retro contributors.
 * SPDX-License-Identifier: AGPL-3.0-only
 */
#include <os2.h>

#define SIZE(t, n) _Static_assert(sizeof(t) == (n), "sizeof(" #t ")")
#ifdef __32BIT__
#include <bsedev.h>

SIZE(CONTEXTRECORD, 176);
SIZE(EXCEPTIONREPORTRECORD, 36);
SIZE(EXCEPTIONREGISTRATIONRECORD, 8);
SIZE(STARTDATA, 60);
SIZE(FILESTATUS3, 24);
SIZE(FILESTATUS4, 28);
SIZE(FILEFINDBUF3, 288);
SIZE(FILEFINDBUF4, 292);
SIZE(FILESTATUS3L, 32);
SIZE(FSALLOCATE, 20);
SIZE(COUNTRYINFO, 44);
SIZE(BIOSPARAMETERBLOCK, 36);
SIZE(DCBINFO, 12);
SIZE(QMSG, 32);
SIZE(SWP, 36);
SIZE(MENUITEM, 16);
SIZE(ACCEL, 6);
SIZE(DLGTITEM, 30);
SIZE(FRAMECDATA, 10);
SIZE(FATTRS, 56);
SIZE(FONTMETRICS, 228); /* with the Warp fields: atoms and PANOSE */
SIZE(BITMAPINFOHEADER2, 64);
SIZE(SWCNTRL, 96);
SIZE(VIOMODEINFO, 14); /* the 16-bit structures of bsesub.h */
SIZE(KBDKEYINFO, 10);
SIZE(MOUEVENTINFO, 10);
#elif defined(_M_I86)
/* The 16-bit Presentation Manager (only with clang-retro's i8086 target:
 * check-syntax.sh's i386 stand-in aligns LONG to 4 bytes). */
SIZE(QMSG, 26);
SIZE(SWP, 18);
SIZE(CLASSINFO, 10);
SIZE(CREATESTRUCT, 42);
SIZE(MENUITEM, 16);
SIZE(ACCEL, 6);
SIZE(DLGTITEM, 30);
SIZE(FRAMECDATA, 10);
SIZE(FATTRS, 56);
SIZE(FONTMETRICS, 208);
SIZE(BITMAPINFOHEADER, 12);
SIZE(SWCNTRL, 80);
#endif

int main(void) { return 0; }
