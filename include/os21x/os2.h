/*
 * os2.h - the 16-bit OS/2 1.x API, for clang-retro (i8086-pc-os2).
 *
 * Everything is declared whatever INCL_ macros are defined: the Dos calls
 * (bsedos.h), video, keyboard and mouse (bsesub.h) and the error codes
 * (bseerr.h).
 */
#ifndef __OS2_H
#define __OS2_H

#if !defined(__OS2__) || defined(__32BIT__)
#error This os2.h is for 16-bit OS/2 1.x (--target=i8086-pc-os2)
#endif

#include <os2def.h>
#include <bsedos.h>
#include <bsesub.h>
#include <bseerr.h>

#endif /* __OS2_H */
