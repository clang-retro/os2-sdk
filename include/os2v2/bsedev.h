/*
 * bsedev.h - DosDevIOCtl categories, functions and parameter structures,
 * for clang-retro (32-bit OS/2, i386-pc-os2).
 *
 * Copyright (C) clang-retro contributors.
 * SPDX-License-Identifier: AGPL-3.0-only
 */
#ifndef __BSEDEV_H
#define __BSEDEV_H

#include <os2def.h>

/* Categories */
#define IOCTL_ASYNC           0x0001
#define IOCTL_SCR_AND_PTRDRAW 0x0003
#define IOCTL_KEYBOARD        0x0004
#define IOCTL_PRINTER         0x0005
#define IOCTL_LIGHTPEN        0x0006
#define IOCTL_POINTINGDEVICE  0x0007
#define IOCTL_DISK            0x0008
#define IOCTL_PHYSICALDISK    0x0009
#define IOCTL_MONITOR         0x000A
#define IOCTL_GENERAL         0x000B
#define IOCTL_POWER           0x000C
#define IOCTL_OEMHLP          0x0080
#define IOCTL_TESTCFG_SYS     0x0080
#define IOCTL_CDROMDISK       0x0080
#define IOCTL_CDROMAUDIO      0x0081

/* IOCTL_ASYNC: serial ports */
#define ASYNC_SETBAUDRATE     0x0041
#define ASYNC_SETLINECTRL     0x0042
#define ASYNC_EXTSETBAUDRATE  0x0043
#define ASYNC_TRANSMITIMM     0x0044
#define ASYNC_SETBREAKOFF     0x0045
#define ASYNC_SETMODEMCTRL    0x0046
#define ASYNC_STOPTRANSMIT    0x0047
#define ASYNC_STARTTRANSMIT   0x0048
#define ASYNC_SETBREAKON      0x004B
#define ASYNC_SETDCBINFO      0x0053
#define ASYNC_SETENHANCEDMODEPARMS 0x0054
#define ASYNC_GETBAUDRATE     0x0061
#define ASYNC_GETLINECTRL     0x0062
#define ASYNC_EXTGETBAUDRATE  0x0063
#define ASYNC_GETCOMMSTATUS   0x0064
#define ASYNC_GETLINESTATUS   0x0065
#define ASYNC_GETMODEMOUTPUT  0x0066
#define ASYNC_GETMODEMINPUT   0x0067
#define ASYNC_GETINQUECOUNT   0x0068
#define ASYNC_GETOUTQUECOUNT  0x0069
#define ASYNC_GETCOMMERROR    0x006D
#define ASYNC_GETCOMMEVENT    0x0072
#define ASYNC_GETDCBINFO      0x0073
#define ASYNC_GETENHANCEDMODEPARMS 0x0074

/* IOCTL_SCR_AND_PTRDRAW */
#define SCR_ALLOCLDT          0x0070
#define SCR_DEALLOCLDT        0x0071
#define PTR_GETPTRDRAWADDRESS 0x0072
#define SCR_ALLOCLDTOFF       0x0075

/* IOCTL_KEYBOARD */
#define KBD_SETTRANSTABLE     0x0050
#define KBD_SETINPUTMODE      0x0051
#define KBD_SETINTERIMFLAG    0x0052
#define KBD_SETSHIFTSTATE     0x0053
#define KBD_SETTYPAMATICRATE  0x0054
#define KBD_SETFGNDSCREENGRP  0x0055
#define KBD_SETSESMGRHOTKEY   0x0056
#define KBD_SETFOCUS          0x0057
#define KBD_SETKCB            0x0058
#define KBD_SETNLS            0x005C
#define KBD_CREATE            0x005D
#define KBD_DESTROY           0x005E
#define KBD_GETINPUTMODE      0x0071
#define KBD_GETINTERIMFLAG    0x0072
#define KBD_GETSHIFTSTATE     0x0073
#define KBD_READCHAR          0x0074
#define KBD_PEEKCHAR          0x0075
#define KBD_GETSESMGRHOTKEY   0x0076
#define KBD_GETKEYBDTYPE      0x0077
#define KBD_GETCODEPAGEID     0x0078
#define KBD_XLATESCAN         0x0079

/* IOCTL_PRINTER */
#define PRT_SETFRAMECTL       0x0042
#define PRT_SETINFINITERETRY  0x0044
#define PRT_INITPRINTER       0x0046
#define PRT_ACTIVATEFONT      0x0048
#define PRT_GETFRAMECTL       0x0062
#define PRT_GETINFINITERETRY  0x0064
#define PRT_GETPRINTERSTATUS  0x0066
#define PRT_QUERYACTIVEFONT   0x0069
#define PRT_VERIFYFONT        0x006A

/* IOCTL_POINTINGDEVICE */
#define MOU_ALLOWPTRDRAW      0x0050
#define MOU_UPDATEDISPLAYMODE 0x0051
#define MOU_SCREENSWITCH      0x0052
#define MOU_SETSCALEFACTORS   0x0053
#define MOU_SETEVENTMASK      0x0054
#define MOU_SETHOTKEYBUTTON   0x0055
#define MOU_SETPTRSHAPE       0x0056
#define MOU_DRAWPTR           0x0057
#define MOU_REMOVEPTR         0x0058
#define MOU_SETPTRPOS         0x0059
#define MOU_SETPROTDRAWADDRESS 0x005A
#define MOU_SETREALDRAWADDRESS 0x005B
#define MOU_SETMOUSTATUS      0x005C
#define MOU_DISPLAYMODECHANGE 0x005D
#define MOU_GETBUTTONCOUNT    0x0060
#define MOU_GETMICKEYCOUNT    0x0061
#define MOU_GETMOUSTATUS      0x0062
#define MOU_READEVENTQUE      0x0063
#define MOU_GETQUESTATUS      0x0064
#define MOU_GETEVENTMASK      0x0065
#define MOU_GETSCALEFACTORS   0x0066
#define MOU_GETPTRPOS         0x0067
#define MOU_GETPTRSHAPE       0x0068
#define MOU_GETHOTKEYBUTTON   0x0069
#define MOU_VER               0x006A

/* IOCTL_DISK: logical drives */
#define DSK_LOCKDRIVE         0x0000
#define DSK_UNLOCKDRIVE       0x0001
#define DSK_REDETERMINEMEDIA  0x0002
#define DSK_SETLOGICALMAP     0x0003
#define DSK_BLOCKREMOVABLE    0x0020
#define DSK_GETLOGICALMAP     0x0021
#define DSK_SETDEVICEPARAMS   0x0043
#define DSK_WRITETRACK        0x0044
#define DSK_FORMATVERIFY      0x0045
#define DSK_GETDEVICEPARAMS   0x0063
#define DSK_READTRACK         0x0064
#define DSK_VERIFYTRACK       0x0065

/* IOCTL_PHYSICALDISK */
#define PDSK_LOCKPHYSDRIVE      0x0000
#define PDSK_UNLOCKPHYSDRIVE    0x0001
#define PDSK_WRITEPHYSTRACK     0x0044
#define PDSK_GETPHYSDEVICEPARAMS 0x0063
#define PDSK_READPHYSTRACK      0x0064
#define PDSK_VERIFYPHYSTRACK    0x0065

/* IOCTL_MONITOR */
#define MON_REGISTERMONITOR   0x0040

/* IOCTL_GENERAL: any device */
#define DEV_FLUSHINPUT        0x0001
#define DEV_FLUSHOUTPUT       0x0002
#define DEV_SYSTEMNOTIFYPDD   0x0041
#define DEV_QUERYMONSUPPORT   0x0060

/* ASYNC_SETDCBINFO and ASYNC_GETDCBINFO */
typedef struct _DCBINFO {
    USHORT usWriteTimeout;
    USHORT usReadTimeout;
    BYTE   fbCtlHndShake;
    BYTE   fbFlowReplace;
    BYTE   fbTimeout;
    BYTE   bErrorReplacementChar;
    BYTE   bBreakReplacementChar;
    BYTE   bXONChar;
    BYTE   bXOFFChar;
} DCBINFO, *PDCBINFO;

#pragma pack(push, 1)
/* ASYNC_SETLINECTRL and ASYNC_GETLINECTRL */
typedef struct _LINECONTROL {
    BYTE bDataBits;
    BYTE bParity;        /* 0 none, 1 odd, 2 even, 3 mark, 4 space */
    BYTE bStopBits;      /* 0 one, 1 one and a half, 2 two */
    BYTE fTransBreak;
} LINECONTROL, *PLINECONTROL;

/* ASYNC_SETMODEMCTRL */
typedef struct _MODEMSTATUS {
    BYTE fbModemOn;
    BYTE fbModemOff;
} MODEMSTATUS, *PMODEMSTATUS;

/* DSK_GETDEVICEPARAMS */
typedef struct _BIOSPARAMETERBLOCK {
    USHORT usBytesPerSector;
    BYTE   bSectorsPerCluster;
    USHORT usReservedSectors;
    BYTE   cFATs;
    USHORT cRootEntries;
    USHORT cSectors;
    BYTE   bMedia;
    USHORT usSectorsPerFAT;
    USHORT usSectorsPerTrack;
    USHORT cHeads;
    ULONG  cHiddenSectors;
    ULONG  cLargeSectors;
    BYTE   abReserved[6];
    USHORT cCylinders;
    BYTE   bDeviceType;
    USHORT fsDeviceAttr;
} BIOSPARAMETERBLOCK, *PBIOSPARAMETERBLOCK;

/* KBD_GETSHIFTSTATE and KBD_SETSHIFTSTATE */
typedef struct _SHIFTSTATE {
    USHORT fsState;
    BYTE   fNLS;
} SHIFTSTATE, *PSHIFTSTATE;
#pragma pack(pop)

#define DEVTYPE_48TPI   0x0000
#define DEVTYPE_96TPI   0x0001
#define DEVTYPE_35      0x0002
#define DEVTYPE_8SD     0x0003
#define DEVTYPE_8DD     0x0004
#define DEVTYPE_FIXED   0x0005
#define DEVTYPE_TAPE    0x0006
#define DEVTYPE_UNKNOWN 0x0007

#endif /* __BSEDEV_H */
