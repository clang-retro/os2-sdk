/*
 * bsedos.h - the OS/2 1.x Dos API (DOSCALLS), for clang-retro
 * (i8086-pc-os2). Every function is far Pascal and takes far pointers.
 */
#ifndef __BSEDOS_H
#define __BSEDOS_H

#include <os2def.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Processes, threads and programs */
#define EXIT_THREAD  0
#define EXIT_PROCESS 1
VOID   APIENTRY DosExit(USHORT fTerminate, USHORT usExitCode);

#define EXEC_SYNC         0
#define EXEC_ASYNC        1
#define EXEC_ASYNCRESULT  2
#define EXEC_TRACE        3
#define EXEC_BACKGROUND   4
USHORT APIENTRY DosExecPgm(PCHAR pchFailName, SHORT cbFailName, USHORT fExecFlags,
                           PSZ pszArgs, PSZ pszEnv, PRESULTCODES prescResults,
                           PSZ pszPgmName);
#define DCWA_PROCESS     0
#define DCWA_PROCESSTREE 1
#define DCWW_WAIT        0
#define DCWW_NOWAIT      1
USHORT APIENTRY DosCwait(USHORT fScope, USHORT fWait, PRESULTCODES prescResults,
                         PPID ppidProcess, PID pidWaitProcess);
USHORT APIENTRY DosKillProcess(USHORT fScope, PID pidProcess);
typedef VOID (FAR *PFNTHREAD)(VOID);
USHORT APIENTRY DosCreateThread(PFNTHREAD pfnFun, PTID ptidThread, PBYTE pbThrdStack);
USHORT APIENTRY DosEnterCritSec(VOID);
USHORT APIENTRY DosExitCritSec(VOID);
#define EXLST_ADD    1
#define EXLST_REMOVE 2
#define EXLST_EXIT   3
USHORT APIENTRY DosExitList(USHORT fFnCode, PFN pfnFunction);

typedef struct _PIDINFO {
    PID pid;
    TID tid;
    PID pidParent;
} PIDINFO, FAR *PPIDINFO;
USHORT APIENTRY DosGetPID(PPIDINFO ppidInfo);
USHORT APIENTRY DosGetPrty(USHORT usScope, PUSHORT pusPriority, USHORT pid);
USHORT APIENTRY DosSetPrty(USHORT usScope, USHORT fPrtyClass, SHORT sChange, USHORT id);
USHORT APIENTRY DosGetEnv(PSEL pselEnv, PUSHORT pOffsetCmd);
USHORT APIENTRY DosScanEnv(PSZ pszVarName, PSZ FAR *ppszResult);
USHORT APIENTRY DosSearchPath(USHORT fsSearch, PSZ pszPath, PSZ pszFName, PBYTE pBuf,
                              USHORT cbBuf);
USHORT APIENTRY DosGetVersion(PUSHORT pVer);
USHORT APIENTRY DosGetMachineMode(PBYTE pMachMode);
#define MODE_REAL      0
#define MODE_PROTECTED 1

typedef struct _GINFOSEG {
    ULONG  time;
    ULONG  msecs;
    UCHAR  hour, minutes, seconds, hundredths;
    USHORT timezone;
    USHORT cusecTimerInterval;
    UCHAR  day, month;
    USHORT year;
    UCHAR  weekday;
    UCHAR  uchMajorVersion, uchMinorVersion, chRevisionLetter;
    UCHAR  sgCurrent, sgMax, cHugeShift, fProtectModeOnly;
    USHORT pidForeground;
    UCHAR  fDynamicSched, csecMaxWait;
    USHORT cmsecMinSlice, cmsecMaxSlice;
    USHORT bootdrive;
    UCHAR  amecRAS[32];
    UCHAR  csgWindowableVioMax, csgPMMax;
} GINFOSEG, FAR *PGINFOSEG;
typedef struct _LINFOSEG {
    PID    pidCurrent, pidParent;
    USHORT prtyCurrent;
    TID    tidCurrent;
    USHORT sgCurrent;
    UCHAR  rfProcStatus, dummy1;
    BOOL   fForeground;
    UCHAR  typeProcess, dummy2;
    SEL    selEnvironment;
    USHORT offCmdLine;
    USHORT cbDataSegment, cbStack, cbHeap;
    USHORT hmod;
    SEL    selDS;
} LINFOSEG, FAR *PLINFOSEG;
USHORT APIENTRY DosGetInfoSeg(PSEL pselGlobal, PSEL pselLocal);

/* Memory */
#define SEG_NONSHARED     0x0000
#define SEG_GIVEABLE      0x0001
#define SEG_GETTABLE      0x0002
#define SEG_DISCARDABLE   0x0004
USHORT APIENTRY DosAllocSeg(USHORT usSize, PSEL pSel, USHORT fAlloc);
USHORT APIENTRY DosReallocSeg(USHORT usNewSize, SEL sel);
USHORT APIENTRY DosFreeSeg(SEL sel);
USHORT APIENTRY DosAllocHuge(USHORT cSegs, USHORT cbPartialSeg, PSEL psel, USHORT cMaxSegs,
                             USHORT fsAlloc);
USHORT APIENTRY DosGetHugeShift(PUSHORT pusShiftCount);
USHORT APIENTRY DosAllocShrSeg(USHORT cbSeg, PSZ pszSegName, PSEL psel);
USHORT APIENTRY DosGetShrSeg(PSZ pszSegName, PSEL psel);
USHORT APIENTRY DosGiveSeg(SEL sel, PID pid, PSEL pselRecipient);
USHORT APIENTRY DosGetSeg(SEL sel);
USHORT APIENTRY DosLockSeg(SEL sel);
USHORT APIENTRY DosUnlockSeg(SEL sel);
USHORT APIENTRY DosSizeSeg(SEL sel, PULONG pcbSize);
USHORT APIENTRY DosMemAvail(PULONG pcbFree);
USHORT APIENTRY DosCreateCSAlias(SEL selDS, PSEL pselCS);
USHORT APIENTRY DosSubAlloc(SEL sel, PUSHORT pusOffset, USHORT cb);
USHORT APIENTRY DosSubFree(SEL sel, USHORT offBlock, USHORT cb);
USHORT APIENTRY DosSubSet(SEL sel, USHORT fFlags, USHORT cbNew);

/* Modules */
USHORT APIENTRY DosLoadModule(PSZ pszFailName, USHORT cbFileName, PSZ pszModName,
                              PHMODULE phmod);
USHORT APIENTRY DosFreeModule(HMODULE hmod);
USHORT APIENTRY DosGetProcAddr(HMODULE hmod, PSZ pszProcName, PPFN ppfnProcAddr);
USHORT APIENTRY DosGetModHandle(PSZ pszModName, PHMODULE phMod);
USHORT APIENTRY DosGetModName(HMODULE hmod, USHORT cbBuf, PCHAR pchBuf);

/* Resources: os2rc.py binds them, each one a segment of the program. */
#define RT_POINTER      1   /* also icons */
#define RT_BITMAP       2
#define RT_MENU         3
#define RT_DIALOG       4   /* DLGTEMPLATE */
#define RT_STRING       5   /* sixteen strings: id / 16 + 1 */
#define RT_FONTDIR      6
#define RT_FONT         7
#define RT_ACCELTABLE   8
#define RT_RCDATA       9
#define RT_MESSAGE      10
#define RT_DLGINCLUDE   11
#define RT_VKEYTBL      12
#define RT_KEYTBL       13
#define RT_CHARTBL      14
#define RT_DISPLAYINFO  15
#define RT_FKASHORT     16
#define RT_FKALONG      17
#define RT_HELPTABLE    18
#define RT_HELPSUBTABLE 19
/* The resource's selector; DosGetResource2 and DosFreeResource are OS/2 1.2. */
USHORT APIENTRY DosGetResource(HMODULE hmod, USHORT idType, USHORT idName, PSEL psel);
USHORT APIENTRY DosGetResource2(HMODULE hmod, USHORT idType, USHORT idName,
                                PVOID FAR *ppData);
USHORT APIENTRY DosFreeResource(PVOID pData);

/* Semaphores */
#define SEM_INDEFINITE_WAIT (-1L)
#define SEM_IMMEDIATE_RETURN 0L
#define CSEM_PRIVATE 0
#define CSEM_PUBLIC  1
USHORT APIENTRY DosCreateSem(USHORT fExclusive, PHSEM phsem, PSZ pszSemName);
USHORT APIENTRY DosOpenSem(PHSEM phsem, PSZ pszSemName);
USHORT APIENTRY DosCloseSem(HSEM hsem);
USHORT APIENTRY DosSemClear(HSEM hsem);
USHORT APIENTRY DosSemSet(HSEM hsem);
USHORT APIENTRY DosSemWait(HSEM hsem, LONG lTimeOut);
USHORT APIENTRY DosSemSetWait(HSEM hsem, LONG lTimeOut);
USHORT APIENTRY DosSemRequest(HSEM hsem, LONG lTimeOut);

/* Time */
typedef struct _DATETIME {
    UCHAR  hours, minutes, seconds, hundredths;
    UCHAR  day, month;
    USHORT year;
    SHORT  timezone;
    UCHAR  weekday;
} DATETIME, FAR *PDATETIME;
USHORT APIENTRY DosGetDateTime(PDATETIME pdatetime);
USHORT APIENTRY DosSetDateTime(PDATETIME pdatetime);
USHORT APIENTRY DosSleep(ULONG ulTime);
USHORT APIENTRY DosBeep(USHORT usFrequency, USHORT usDuration);

/* Files */
#define FILE_BEGIN   0
#define FILE_CURRENT 1
#define FILE_END     2

#define FILE_NORMAL    0x0000
#define FILE_READONLY  0x0001
#define FILE_HIDDEN    0x0002
#define FILE_SYSTEM    0x0004
#define FILE_DIRECTORY 0x0010
#define FILE_ARCHIVED  0x0020

#define FILE_EXISTED  1
#define FILE_CREATED  2
#define FILE_TRUNCATED 3

#define FILE_OPEN     0x0001
#define FILE_TRUNCATE 0x0002
#define FILE_CREATE   0x0010
#define OPEN_ACTION_FAIL_IF_EXISTS    0x0000
#define OPEN_ACTION_OPEN_IF_EXISTS    0x0001
#define OPEN_ACTION_REPLACE_IF_EXISTS 0x0002
#define OPEN_ACTION_FAIL_IF_NEW       0x0000
#define OPEN_ACTION_CREATE_IF_NEW     0x0010

#define OPEN_ACCESS_READONLY  0x0000
#define OPEN_ACCESS_WRITEONLY 0x0001
#define OPEN_ACCESS_READWRITE 0x0002
#define OPEN_SHARE_DENYREADWRITE 0x0010
#define OPEN_SHARE_DENYWRITE  0x0020
#define OPEN_SHARE_DENYREAD   0x0030
#define OPEN_SHARE_DENYNONE   0x0040
#define OPEN_FLAGS_NOINHERIT  0x0080
#define OPEN_FLAGS_FAIL_ON_ERROR 0x2000
#define OPEN_FLAGS_WRITE_THROUGH 0x4000
#define OPEN_FLAGS_DASD       0x8000

USHORT APIENTRY DosOpen(PSZ pszFname, PHFILE phfOpen, PUSHORT pusAction, ULONG ulFSize,
                        USHORT usAttr, USHORT fsOpenFlags, USHORT fsOpenMode,
                        ULONG ulReserved);
USHORT APIENTRY DosClose(HFILE hf);
USHORT APIENTRY DosRead(HFILE hf, PVOID pBuf, USHORT cbBuf, PUSHORT pcbBytesRead);
USHORT APIENTRY DosWrite(HFILE hf, PVOID pBuf, USHORT cbBuf, PUSHORT pcbBytesWritten);
USHORT APIENTRY DosChgFilePtr(HFILE hf, LONG lOffset, USHORT fsMethod, PULONG pulNewOffset);
USHORT APIENTRY DosNewSize(HFILE hf, ULONG ulNewSize);
USHORT APIENTRY DosBufReset(HFILE hf);
USHORT APIENTRY DosDupHandle(HFILE hfOld, PHFILE phfNew);
USHORT APIENTRY DosSetMaxFH(USHORT usHandles);
USHORT APIENTRY DosQFHandState(HFILE hf, PUSHORT pfsOpenMode);
USHORT APIENTRY DosSetFHandState(HFILE hf, USHORT fsState);
#define HANDTYPE_FILE    0x0000
#define HANDTYPE_DEVICE  0x0001
#define HANDTYPE_PIPE    0x0002
#define HANDTYPE_NETWORK 0x8000
USHORT APIENTRY DosQHandType(HFILE hf, PUSHORT pfsType, PUSHORT pusDevAttr);
USHORT APIENTRY DosMakePipe(PHFILE phfRead, PHFILE phfWrite, USHORT cbPipe);

USHORT APIENTRY DosDelete(PSZ pszFName, ULONG ulReserved);
USHORT APIENTRY DosMove(PSZ pszOldName, PSZ pszNewName, ULONG ulReserved);
USHORT APIENTRY DosMkDir(PSZ pszDirName, ULONG ulReserved);
USHORT APIENTRY DosRmDir(PSZ pszDir, ULONG ulReserved);
USHORT APIENTRY DosChDir(PSZ pszDirPath, ULONG ulReserved);
USHORT APIENTRY DosQCurDir(USHORT usDriveNumber, PBYTE pszPathBuf, PUSHORT pcbPathBuf);
USHORT APIENTRY DosQCurDisk(PUSHORT pusDriveNumber, PULONG pulLogicalDrives);
USHORT APIENTRY DosSelectDisk(USHORT usDriveNumber);
USHORT APIENTRY DosQFileMode(PSZ pszFname, PUSHORT pusAttr, ULONG ulReserved);
USHORT APIENTRY DosSetFileMode(PSZ pszFName, USHORT usAttr, ULONG ulReserved);

typedef struct _FDATE {
    USHORT day : 5;
    USHORT month : 4;
    USHORT year : 7;
} FDATE, FAR *PFDATE;
typedef struct _FTIME {
    USHORT twosecs : 5;
    USHORT minutes : 6;
    USHORT hours : 5;
} FTIME, FAR *PFTIME;
typedef struct _FILESTATUS {
    FDATE  fdateCreation;
    FTIME  ftimeCreation;
    FDATE  fdateLastAccess;
    FTIME  ftimeLastAccess;
    FDATE  fdateLastWrite;
    FTIME  ftimeLastWrite;
    ULONG  cbFile;
    ULONG  cbFileAlloc;
    USHORT attrFile;
} FILESTATUS, FAR *PFILESTATUS;
#define FIL_STANDARD 1
USHORT APIENTRY DosQFileInfo(HFILE hf, USHORT usInfoLevel, PFILESTATUS pfstsInfo,
                             USHORT cbInfoBuf);
USHORT APIENTRY DosSetFileInfo(HFILE hf, USHORT usInfoLevel, PFILESTATUS pfstsInfo,
                               USHORT cbInfoBuf);
USHORT APIENTRY DosQPathInfo(PSZ pszPath, USHORT usInfoLevel, PBYTE pInfoBuf,
                             USHORT cbInfoBuf, ULONG ulReserved);

typedef struct _FILEFINDBUF {
    FDATE  fdateCreation;
    FTIME  ftimeCreation;
    FDATE  fdateLastAccess;
    FTIME  ftimeLastAccess;
    FDATE  fdateLastWrite;
    FTIME  ftimeLastWrite;
    ULONG  cbFile;
    ULONG  cbFileAlloc;
    USHORT attrFile;
    UCHAR  cchName;
    CHAR   achName[13];
} FILEFINDBUF, FAR *PFILEFINDBUF;
#define HDIR_SYSTEM 1
#define HDIR_CREATE 0xFFFF
USHORT APIENTRY DosFindFirst(PSZ pszFSpec, PHDIR phdir, USHORT usAttr, PFILEFINDBUF pffb,
                             USHORT cbBuf, PUSHORT pcSearch, ULONG ulReserved);
USHORT APIENTRY DosFindNext(HDIR hdir, PFILEFINDBUF pffb, USHORT cbBuf, PUSHORT pcSearch);
USHORT APIENTRY DosFindClose(HDIR hdir);

/* Devices and errors */
USHORT APIENTRY DosDevIOCtl(PVOID pData, PVOID pParms, USHORT usFunction,
                            USHORT usCategory, HFILE hDevice);
USHORT APIENTRY DosDevConfig(PVOID pDevInfo, USHORT usItem, USHORT usReserved);
#define HARDERROR_DISABLE 0
#define HARDERROR_ENABLE  1
#define EXCEPTION_DISABLE 0
#define EXCEPTION_ENABLE  2
USHORT APIENTRY DosError(USHORT fEnable);

/* Messages (MSG.DLL) */
USHORT APIENTRY DosPutMessage(HFILE hf, USHORT cbMsg, PCHAR pchMsg);
/* MSG.DLL's DosGetMessage takes, last, the program's table of messages
   bound into it (by MSGBIND). The toolkits' DosGetMessage is a stub that
   passes it; this one passes an empty table, so messages come from the
   message file. */
USHORT APIENTRY DosTrueGetMessage(PCHAR FAR *ppchVTable, USHORT usVCount,
                                  PCHAR pchBuf, USHORT cbBuf, USHORT usMsgNum,
                                  PSZ pszFileName, PUSHORT pcbMsg, PVOID pMsgSeg);
static __inline__ USHORT DosGetMessage(PCHAR FAR *ppchVTable, USHORT usVCount,
                                       PCHAR pchBuf, USHORT cbBuf, USHORT usMsgNum,
                                       PSZ pszFileName, PUSHORT pcbMsg)
{
    static const UCHAR seg[18] = {
        0xFF, 'M', 'K', 'M', 'S', 'G', 'S', 'E', 'G', 0, 1, 0, 0, 0, 0, 0
    };
    return DosTrueGetMessage(ppchVTable, usVCount, pchBuf, cbBuf, usMsgNum,
                             pszFileName, pcbMsg, (PVOID)seg);
}

#ifdef __cplusplus
}
#endif

#endif /* __BSEDOS_H */
