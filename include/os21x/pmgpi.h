/*
 * pmgpi.h - the Graphics Programming Interface of the 16-bit Presentation
 * Manager (PMGPI, OS/2 1.1 and later), for clang-retro (i8086-pc-os2):
 * presentation spaces, colors and mixes, lines, arcs, areas and paths,
 * text and fonts, bitmaps (the OS/2 1.x BITMAPINFOHEADER and BITMAPINFO),
 * and device contexts (DevOpenDC). Not yet: segments, metafiles, regions
 * beyond their handles, transforms and printing.
 *
 * Copyright (C) clang-retro contributors.
 * SPDX-License-Identifier: AGPL-3.0-only
 */
#ifndef __PMGPI_H
#define __PMGPI_H

#include <os2def.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef LONG FIXED, FAR *PFIXED;
#define MAKEFIXED(intpart, fractpart) \
    ((FIXED)(((ULONG)(USHORT)(intpart) << 16) | (USHORT)(fractpart)))
#define FIXEDINT(fx)   ((SHORT)HIUSHORT(fx))
#define FIXEDFRAC(fx)  (LOUSHORT(fx))

typedef struct _SIZEF {
    FIXED cx;
    FIXED cy;
} SIZEF, FAR *PSIZEF;

typedef struct _ARCPARAMS {
    LONG lP;
    LONG lQ;
    LONG lR;
    LONG lS;
} ARCPARAMS, FAR *PARCPARAMS;

typedef struct _GRADIENTL {
    LONG x;
    LONG y;
} GRADIENTL, FAR *PGRADIENTL;

#define GPI_ERROR 0L
#define GPI_OK    1L
#define GPI_ALTERROR (-1L)

/* Colors (GPI) */
#define CLR_FALSE      (-5L)
#define CLR_TRUE       (-4L)
#define CLR_ERROR      (-255L)
#define CLR_DEFAULT    (-3L)
#define CLR_WHITE      (-2L)
#define CLR_BLACK      (-1L)
#define CLR_BACKGROUND 0L
#define CLR_BLUE       1L
#define CLR_RED        2L
#define CLR_PINK       3L
#define CLR_GREEN      4L
#define CLR_CYAN       5L
#define CLR_YELLOW     6L
#define CLR_NEUTRAL    7L
#define CLR_DARKGRAY   8L
#define CLR_DARKBLUE   9L
#define CLR_DARKRED    10L
#define CLR_DARKPINK   11L
#define CLR_DARKGREEN  12L
#define CLR_DARKCYAN   13L
#define CLR_BROWN      14L
#define CLR_PALEGRAY   15L

/* GPI */
BOOL APIENTRY GpiSetColor(HPS hps, LONG lColor);
BOOL APIENTRY GpiSetBackColor(HPS hps, LONG lColor);
BOOL APIENTRY GpiSetBackMix(HPS hps, LONG lMixMode);
BOOL APIENTRY GpiMove(HPS hps, PPOINTL pptlPoint);
LONG APIENTRY GpiLine(HPS hps, PPOINTL pptlEndPoint);
LONG APIENTRY GpiBox(HPS hps, LONG lControl, PPOINTL pptlPoint, LONG lHRound, LONG lVRound);
LONG APIENTRY GpiCharString(HPS hps, LONG lCount, PCH pchString);
LONG APIENTRY GpiCharStringAt(HPS hps, PPOINTL pptlPoint, LONG lCount, PCH pchString);
BOOL APIENTRY GpiErase(HPS hps);
#define DRO_FILL        1L
#define DRO_OUTLINE     2L
#define DRO_OUTLINEFILL 3L
#define BM_LEAVEALONE   5L
#define BM_OVERPAINT    2L

#define RGB_ERROR (-255L)
#define RGB_BLACK 0x00000000L
#define RGB_BLUE  0x000000FFL
#define RGB_GREEN 0x0000FF00L
#define RGB_CYAN  0x0000FFFFL
#define RGB_RED   0x00FF0000L
#define RGB_PINK  0x00FF00FFL
#define RGB_YELLOW 0x00FFFF00L
#define RGB_WHITE 0x00FFFFFFL

/* Presentation spaces */
#define PU_ARBITRARY 0x0004L
#define PU_PELS      0x0008L
#define PU_LOMETRIC  0x000CL
#define PU_HIMETRIC  0x0010L
#define PU_LOENGLISH 0x0014L
#define PU_HIENGLISH 0x0018L
#define PU_TWIPS     0x001CL
#define GPIF_DEFAULT 0L
#define GPIF_SHORT   0x0100L
#define GPIF_LONG    0x0200L
#define GPIT_NORMAL  0L
#define GPIT_MICRO   0x1000L
#define GPIA_NOASSOC 0L
#define GPIA_ASSOC   0x4000L
HPS  APIENTRY GpiCreatePS(HAB hab, HDC hdc, PSIZEL psizlSize, ULONG flOptions);
BOOL APIENTRY GpiDestroyPS(HPS hps);
BOOL APIENTRY GpiAssociate(HPS hps, HDC hdc);
BOOL APIENTRY GpiResetPS(HPS hps, ULONG flOptions);
LONG APIENTRY GpiSavePS(HPS hps);
BOOL APIENTRY GpiRestorePS(HPS hps, LONG lPSid);
HDC  APIENTRY GpiQueryDevice(HPS hps);
#define GRES_ATTRS    0x0001L
#define GRES_SEGMENTS 0x0002L
#define GRES_ALL      0x0004L

/* Device contexts (PMGPI's Dev calls) */
#define OD_QUEUED   2L
#define OD_DIRECT   5L
#define OD_INFO     6L
#define OD_METAFILE 7L
#define OD_MEMORY   8L
#define OD_METAFILE_NOQUERY 9L
typedef PSZ FAR *PDEVOPENDATA;
HDC  APIENTRY DevOpenDC(HAB hab, LONG lType, PCSZ pszToken, LONG lCount,
                        PDEVOPENDATA pdopData, HDC hdcComp);
HMF  APIENTRY DevCloseDC(HDC hdc);
BOOL APIENTRY DevQueryCaps(HDC hdc, LONG lStart, LONG lCount, PLONG alArray);
#define CAPS_FAMILY             0L
#define CAPS_IO_CAPS            1L
#define CAPS_TECHNOLOGY         2L
#define CAPS_DRIVER_VERSION     3L
#define CAPS_HEIGHT             5L
#define CAPS_WIDTH              4L
#define CAPS_HEIGHT_IN_CHARS    7L
#define CAPS_WIDTH_IN_CHARS     6L
#define CAPS_VERTICAL_RESOLUTION   9L
#define CAPS_HORIZONTAL_RESOLUTION 8L
#define CAPS_CHAR_HEIGHT        11L
#define CAPS_CHAR_WIDTH         10L
#define CAPS_SMALL_CHAR_HEIGHT  13L
#define CAPS_SMALL_CHAR_WIDTH   12L
#define CAPS_COLORS             14L
#define CAPS_COLOR_PLANES       15L
#define CAPS_COLOR_BITCOUNT     16L
#define CAPS_COLOR_TABLE_SUPPORT 17L
#define CAPS_MOUSE_BUTTONS      18L

/* Colors and mixes */
#define FM_ERROR       (-1L)
#define FM_DEFAULT     0L
#define FM_OR          1L
#define FM_OVERPAINT   2L
#define FM_XOR         4L
#define FM_LEAVEALONE  5L
#define FM_AND         6L
#define FM_SUBTRACT    7L
#define FM_MASKSRCNOT  8L
#define FM_ZERO        9L
#define FM_NOTMERGESRC 10L
#define FM_NOTXORSRC   11L
#define FM_INVERT      12L
#define FM_MERGESRCNOT 13L
#define FM_NOTCOPYSRC  14L
#define FM_MERGENOTSRC 15L
#define FM_NOTMASKSRC  16L
#define FM_ONE         17L
#define BM_ERROR       (-1L)
#define BM_DEFAULT     0L
#define BM_OR          1L
#define BM_XOR         4L
#define LCOL_RESET        0x0001L
#define LCOL_REALIZABLE   0x0002L
#define LCOL_PURECOLOR    0x0004L
#define LCOLF_DEFAULT     0L
#define LCOLF_INDRGB      1L
#define LCOLF_CONSECRGB   2L
#define LCOLF_RGB         3L
LONG APIENTRY GpiQueryColor(HPS hps);
LONG APIENTRY GpiQueryBackColor(HPS hps);
BOOL APIENTRY GpiSetMix(HPS hps, LONG lMixMode);
LONG APIENTRY GpiQueryMix(HPS hps);
LONG APIENTRY GpiQueryBackMix(HPS hps);
BOOL APIENTRY GpiCreateLogColorTable(HPS hps, ULONG flOptions, LONG lFormat,
                                     LONG lStart, LONG lCount, PLONG alTable);
LONG APIENTRY GpiQueryRGBColor(HPS hps, ULONG flOptions, LONG lColorIndex);
LONG APIENTRY GpiQueryColorIndex(HPS hps, ULONG flOptions, LONG lRgbColor);

/* Lines and positions */
#define LINETYPE_DEFAULT       0L
#define LINETYPE_DOT           1L
#define LINETYPE_SHORTDASH     2L
#define LINETYPE_DASHDOT       3L
#define LINETYPE_DOUBLEDOT     4L
#define LINETYPE_LONGDASH      5L
#define LINETYPE_DASHDOUBLEDOT 6L
#define LINETYPE_SOLID         7L
#define LINETYPE_INVISIBLE     8L
#define LINETYPE_ALTERNATE     9L
#define LINEWIDTH_DEFAULT      0L
#define LINEWIDTH_NORMAL       0x00010000L
BOOL APIENTRY GpiSetLineType(HPS hps, LONG lLineType);
LONG APIENTRY GpiQueryLineType(HPS hps);
BOOL APIENTRY GpiSetLineWidth(HPS hps, FIXED fxLineWidth);
BOOL APIENTRY GpiSetLineWidthGeom(HPS hps, LONG lLineWidth);
BOOL APIENTRY GpiSetCurrentPosition(HPS hps, PPOINTL pptlPoint);
BOOL APIENTRY GpiQueryCurrentPosition(HPS hps, PPOINTL pptlPoint);
LONG APIENTRY GpiPolyLine(HPS hps, LONG lCount, PPOINTL aptlPoints);
LONG APIENTRY GpiPolyLineDisjoint(HPS hps, LONG lCount, PPOINTL aptlPoints);
LONG APIENTRY GpiPolyFillet(HPS hps, LONG lCount, PPOINTL aptlPoints);
LONG APIENTRY GpiPolySpline(HPS hps, LONG lCount, PPOINTL aptlPoints);
LONG APIENTRY GpiSetPel(HPS hps, PPOINTL pptlPoint);
LONG APIENTRY GpiQueryPel(HPS hps, PPOINTL pptlPoint);
LONG APIENTRY GpiMarker(HPS hps, PPOINTL pptlPoint);
LONG APIENTRY GpiPolyMarker(HPS hps, LONG lCount, PPOINTL aptlPoint);
BOOL APIENTRY GpiSetMarker(HPS hps, LONG lSymbol);

/* Arcs */
BOOL APIENTRY GpiSetArcParams(HPS hps, PARCPARAMS parcpArcParams);
BOOL APIENTRY GpiQueryArcParams(HPS hps, PARCPARAMS parcpArcParams);
LONG APIENTRY GpiFullArc(HPS hps, LONG lControl, FIXED fxMultiplier);
LONG APIENTRY GpiPartialArc(HPS hps, PPOINTL pptlCenter, FIXED fxMultiplier,
                            FIXED fxStartAngle, FIXED fxSweepAngle);
LONG APIENTRY GpiPointArc(HPS hps, PPOINTL pptl2);

/* Areas, patterns and paths */
#define PATSYM_ERROR     (-1L)
#define PATSYM_DEFAULT   0L
#define PATSYM_DENSE1    1L
#define PATSYM_DENSE2    2L
#define PATSYM_DENSE3    3L
#define PATSYM_DENSE4    4L
#define PATSYM_DENSE5    5L
#define PATSYM_DENSE6    6L
#define PATSYM_DENSE7    7L
#define PATSYM_DENSE8    8L
#define PATSYM_VERT      9L
#define PATSYM_HORIZ     10L
#define PATSYM_DIAG1     11L
#define PATSYM_DIAG2     12L
#define PATSYM_DIAG3     13L
#define PATSYM_DIAG4     14L
#define PATSYM_NOSHADE   15L
#define PATSYM_SOLID     16L
#define PATSYM_HALFTONE  17L
#define PATSYM_BLANK     64L
#define BA_NOBOUNDARY 0L
#define BA_BOUNDARY   0x0001L
#define BA_ALTERNATE  0L
#define BA_WINDING    0x0002L
#define FPATH_ALTERNATE 0L
#define FPATH_WINDING   0x0002L
BOOL APIENTRY GpiSetPattern(HPS hps, LONG lPatternSymbol);
LONG APIENTRY GpiQueryPattern(HPS hps);
BOOL APIENTRY GpiBeginArea(HPS hps, ULONG flOptions);
LONG APIENTRY GpiEndArea(HPS hps);
BOOL APIENTRY GpiBeginPath(HPS hps, LONG lPath);
BOOL APIENTRY GpiEndPath(HPS hps);
BOOL APIENTRY GpiCloseFigure(HPS hps);
LONG APIENTRY GpiFillPath(HPS hps, LONG lPath, LONG lOptions);
LONG APIENTRY GpiStrokePath(HPS hps, LONG lPath, ULONG flOptions);
LONG APIENTRY GpiOutlinePath(HPS hps, LONG lPath, LONG lOptions);
BOOL APIENTRY GpiModifyPath(HPS hps, LONG lPath, LONG lMode);
#define MPATH_STROKE 6L

/* Text and fonts */
#define FACESIZE 32
#define LCID_ALL     (-1L)
#define LCID_DEFAULT 0L
#define FATTR_SEL_ITALIC      0x0001
#define FATTR_SEL_UNDERSCORE  0x0002
#define FATTR_SEL_OUTLINE     0x0008
#define FATTR_SEL_STRIKEOUT   0x0010
#define FATTR_SEL_BOLD        0x0020
#define FATTR_TYPE_KERNING    0x0004
#define FATTR_TYPE_MBCS       0x0008
#define FATTR_TYPE_DBCS       0x0010
#define FATTR_TYPE_ANTIALIASED 0x0020
#define FATTR_FONTUSE_NOMIX         0x0002
#define FATTR_FONTUSE_OUTLINE       0x0004
#define FATTR_FONTUSE_TRANSFORMABLE 0x0008
#define FONT_DEFAULT 1L
#define FONT_MATCH   2L
#define CHS_OPAQUE   0x0001L
#define CHS_VECTOR   0x0002L
#define CHS_LEAVEPOS 0x0008L
#define CHS_CLIP     0x0010L
#define TXTBOX_TOPLEFT     0L
#define TXTBOX_BOTTOMLEFT  1L
#define TXTBOX_TOPRIGHT    2L
#define TXTBOX_BOTTOMRIGHT 3L
#define TXTBOX_CONCAT      4L
#define TXTBOX_COUNT       5L
#define QF_PUBLIC   0x0001L
#define QF_PRIVATE  0x0002L
typedef struct _FATTRS {
    USHORT usRecordLength;
    USHORT fsSelection;
    LONG   lMatch;
    CHAR   szFacename[FACESIZE];
    USHORT idRegistry;
    USHORT usCodePage;
    LONG   lMaxBaselineExt;
    LONG   lAveCharWidth;
    USHORT fsType;
    USHORT fsFontUse;
} FATTRS, FAR *PFATTRS;
typedef struct _FONTMETRICS {
    CHAR   szFamilyname[FACESIZE];
    CHAR   szFacename[FACESIZE];
    USHORT idRegistry;
    USHORT usCodePage;
    LONG   lEmHeight;
    LONG   lXHeight;
    LONG   lMaxAscender;
    LONG   lMaxDescender;
    LONG   lLowerCaseAscent;
    LONG   lLowerCaseDescent;
    LONG   lInternalLeading;
    LONG   lExternalLeading;
    LONG   lAveCharWidth;
    LONG   lMaxCharInc;
    LONG   lEmInc;
    LONG   lMaxBaselineExt;
    SHORT  sCharSlope;
    SHORT  sInlineDir;
    SHORT  sCharRot;
    USHORT usWeightClass;
    USHORT usWidthClass;
    SHORT  sXDeviceRes;
    SHORT  sYDeviceRes;
    SHORT  sFirstChar;
    SHORT  sLastChar;
    SHORT  sDefaultChar;
    SHORT  sBreakChar;
    SHORT  sNominalPointSize;
    SHORT  sMinimumPointSize;
    SHORT  sMaximumPointSize;
    USHORT fsType;
    USHORT fsDefn;
    USHORT fsSelection;
    USHORT fsCapabilities;
    LONG   lSubscriptXSize;
    LONG   lSubscriptYSize;
    LONG   lSubscriptXOffset;
    LONG   lSubscriptYOffset;
    LONG   lSuperscriptXSize;
    LONG   lSuperscriptYSize;
    LONG   lSuperscriptXOffset;
    LONG   lSuperscriptYOffset;
    LONG   lUnderscoreSize;
    LONG   lUnderscorePosition;
    LONG   lStrikeoutSize;
    LONG   lStrikeoutPosition;
    SHORT  sKerningPairs;
    SHORT  sFamilyClass;
    LONG   lMatch;
} FONTMETRICS, FAR *PFONTMETRICS;
LONG    APIENTRY GpiCreateLogFont(HPS hps, PSTR8 pName, LONG lLcid, PFATTRS pfatAttrs);
BOOL APIENTRY GpiDeleteSetId(HPS hps, LONG lLcid);
BOOL APIENTRY GpiSetCharSet(HPS hps, LONG llcid);
LONG APIENTRY GpiQueryCharSet(HPS hps);
BOOL APIENTRY GpiSetCharBox(HPS hps, PSIZEF psizfxBox);
BOOL APIENTRY GpiQueryCharBox(HPS hps, PSIZEF psizfxSize);
BOOL APIENTRY GpiSetCharAngle(HPS hps, PGRADIENTL pgradlAngle);
BOOL APIENTRY GpiQueryFontMetrics(HPS hps, LONG lMetricsLength, PFONTMETRICS pfmMetrics);
LONG APIENTRY GpiQueryFonts(HPS hps, ULONG flOptions, PCSZ pszFacename,
                            PLONG plReqFonts, LONG lMetricsLength,
                            PFONTMETRICS afmMetrics);
BOOL APIENTRY GpiQueryTextBox(HPS hps, LONG lCount1, PCH pchString, LONG lCount2,
                              PPOINTL aptlPoints);
LONG APIENTRY GpiCharStringPos(HPS hps, PRECTL prclRect, ULONG flOptions, LONG lCount,
                               PCH pchString, PLONG alAdx);
LONG APIENTRY GpiCharStringPosAt(HPS hps, PPOINTL pptlStart, PRECTL prclRect,
                                 ULONG flOptions, LONG lCount, PCH pchString,
                                 PLONG alAdx);

/* Bitmaps */
#define ROP_SRCCOPY     0x00CCL
#define ROP_SRCPAINT    0x00EEL
#define ROP_SRCAND      0x0088L
#define ROP_SRCINVERT   0x0066L
#define ROP_SRCERASE    0x0044L
#define ROP_NOTSRCCOPY  0x0033L
#define ROP_NOTSRCERASE 0x0011L
#define ROP_MERGECOPY   0x00C0L
#define ROP_MERGEPAINT  0x00BBL
#define ROP_PATCOPY     0x00F0L
#define ROP_PATPAINT    0x00FBL
#define ROP_PATINVERT   0x005AL
#define ROP_DSTINVERT   0x0055L
#define ROP_ZERO        0x0000L
#define ROP_ONE         0x00FFL
#define BBO_OR     0L
#define BBO_AND    1L
#define BBO_IGNORE 2L
#define CBM_INIT 0x0004L
typedef struct _BITMAPINFOHEADER {
    ULONG  cbFix;
    USHORT cx;
    USHORT cy;
    USHORT cPlanes;
    USHORT cBitCount;
} BITMAPINFOHEADER, FAR *PBITMAPINFOHEADER;
typedef struct _RGB {
    BYTE bBlue;
    BYTE bGreen;
    BYTE bRed;
} RGB, FAR *PRGB;
typedef struct _BITMAPINFO {
    ULONG  cbFix;
    USHORT cx;
    USHORT cy;
    USHORT cPlanes;
    USHORT cBitCount;
    RGB    argbColor[1];
} BITMAPINFO, FAR *PBITMAPINFO;
HBITMAP APIENTRY GpiLoadBitmap(HPS hps, HMODULE Resource, USHORT idBitmap, LONG lWidth,
                               LONG lHeight);
BOOL    APIENTRY GpiDeleteBitmap(HBITMAP hbm);
HBITMAP APIENTRY GpiSetBitmap(HPS hps, HBITMAP hbm);
HBITMAP APIENTRY GpiCreateBitmap(HPS hps, PBITMAPINFOHEADER pbmpNew, ULONG flOptions,
                                 PBYTE pbInitData, PBITMAPINFO pbmiInfoTable);
LONG    APIENTRY GpiBitBlt(HPS hpsTarget, HPS hpsSource, LONG lCount, PPOINTL aptlPoints,
                           LONG lRop, ULONG flOptions);
LONG    APIENTRY GpiWCBitBlt(HPS hpsTarget, HBITMAP hbmSource, LONG lCount,
                             PPOINTL aptlPoints, LONG lRop, ULONG flOptions);
LONG    APIENTRY GpiQueryBitmapBits(HPS hps, LONG lScanStart, LONG lScans,
                                    PBYTE pbBuffer, PBITMAPINFO pbmiInfoTable);
LONG    APIENTRY GpiSetBitmapBits(HPS hps, LONG lScanStart, LONG lScans,
                                  PBYTE pbBuffer, PBITMAPINFO pbmiInfoTable);

/* Clipping */
LONG APIENTRY GpiIntersectClipRectangle(HPS hps, PRECTL prclRectangle);
LONG APIENTRY GpiExcludeClipRectangle(HPS hps, PRECTL prclRectangle);

#ifdef __cplusplus
}
#endif

#endif /* __PMGPI_H */
