/*
 * os2.h - the 32-bit OS/2 API (2.0 and later), for clang-retro
 * (i386-pc-os2).
 *
 * Everything here is declared whatever INCL_ macros are defined: the Dos
 * calls (bsedos.h, with exceptions in bsexcpt.h), the error codes
 * (bseerr.h), Vio, Kbd and Mou (bsesub.h) and, unless INCL_NOPMAPI is defined, the Presentation
 * Manager: windows, menus, dialogs and controls (pmwin.h), graphics
 * (pmgpi.h), the switch list and profiles (pmshl.h) and the file dialog
 * (pmstddlg.h). The DosDevIOCtl constants are in bsedev.h, which os2.h
 * does not include.
 *
 * The video, keyboard and mouse calls (Vio, Kbd, Mou), which are 16-bit
 * on every OS/2, are in bsesub.h, as inline functions that call them
 * through the C library's thunk.
 */
#ifndef __OS2_H
#define __OS2_H

#if !defined(__OS2__) || !defined(__32BIT__)
#error This os2.h is for 32-bit OS/2 (--target=i386-pc-os2)
#endif

#include <os2def.h>
#include <bsedos.h>
#include <bseerr.h>
#include <bsesub.h>
#ifndef INCL_NOPMAPI
#include <pmwin.h>
#include <pmgpi.h>
#include <pmshl.h>
#include <pmstddlg.h>
#endif

#endif /* __OS2_H */
