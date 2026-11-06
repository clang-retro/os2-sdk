/*
 * bsedos.h - the 32-bit OS/2 Dos API (DOSCALLS), for clang-retro
 * (i386-pc-os2).
 */
#ifndef __BSEDOS_H
#define __BSEDOS_H

#include <os2def.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Processes and threads */
#define EXIT_THREAD  0
#define EXIT_PROCESS 1
VOID   APIENTRY DosExit(ULONG action, ULONG result);

#define EXEC_SYNC         0
#define EXEC_ASYNC        1
#define EXEC_ASYNCRESULT  2
#define EXEC_TRACE        3
#define EXEC_BACKGROUND   4
APIRET APIENTRY DosExecPgm(PCHAR pObjname, LONG cbObjname, ULONG execFlag, PCSZ pArg,
                           PCSZ pEnv, PRESULTCODES pRes, PCSZ pName);
#define DCWA_PROCESS     0
#define DCWA_PROCESSTREE 1
#define DCWW_WAIT        0
#define DCWW_NOWAIT      1
APIRET APIENTRY DosWaitChild(ULONG action, ULONG option, PRESULTCODES pres, PPID ppid,
                             PID pid);
APIRET APIENTRY DosKillProcess(ULONG action, PID pid);

typedef VOID APIENTRY FNTHREAD(ULONG);
typedef FNTHREAD *PFNTHREAD;
#define CREATE_READY     0
#define CREATE_SUSPENDED 1
#define STACK_SPARSE     0
#define STACK_COMMITTED  2
APIRET APIENTRY DosCreateThread(PTID ptid, PFNTHREAD pfn, ULONG param, ULONG flag,
                                ULONG cbStack);
#define DCWW_WAIT   0
APIRET APIENTRY DosWaitThread(PTID ptid, ULONG option);
APIRET APIENTRY DosKillThread(TID tid);
APIRET APIENTRY DosSuspendThread(TID tid);
APIRET APIENTRY DosResumeThread(TID tid);
APIRET APIENTRY DosEnterCritSec(VOID);
APIRET APIENTRY DosExitCritSec(VOID);
#define EXLST_ADD    1
#define EXLST_REMOVE 2
#define EXLST_EXIT   3
APIRET APIENTRY DosExitList(ULONG ordercode, PFN pfn);
APIRET APIENTRY DosSetPriority(ULONG scope, ULONG ulClass, LONG delta, ULONG PorTid);

typedef struct _TIB2 {
    ULONG  tib2_ultid;
    ULONG  tib2_ulpri;
    ULONG  tib2_version;
    USHORT tib2_usMCCount;
    USHORT tib2_fMCForceFlag;
} TIB2, *PTIB2;
typedef struct _TIB {
    PVOID  tib_pexchain;
    PVOID  tib_pstack;
    PVOID  tib_pstacklimit;
    PTIB2  tib_ptib2;
    ULONG  tib_version;
    ULONG  tib_ordinal;
} TIB, *PTIB;
typedef struct _PIB {
    ULONG pib_ulpid;
    ULONG pib_ulppid;
    ULONG pib_hmte;
    PCHAR pib_pchcmd;
    PCHAR pib_pchenv;
    ULONG pib_flstatus;
    ULONG pib_ultype;
} PIB, *PPIB;
APIRET APIENTRY DosGetInfoBlocks(PTIB *pptib, PPIB *pppib);

APIRET APIENTRY DosScanEnv(PCSZ pszName, PCSZ *ppszValue);
APIRET APIENTRY DosSearchPath(ULONG flag, PCSZ pszPathOrName, PCSZ pszFilename, PBYTE pBuf,
                              ULONG cbBuf);

#define QSV_MAX_PATH_LENGTH 1
#define QSV_MAX_TEXT_SESSIONS 2
#define QSV_MAX_PM_SESSIONS 3
#define QSV_MAX_VDM_SESSIONS 4
#define QSV_BOOT_DRIVE 5
#define QSV_DYN_PRI_VARIATION 6
#define QSV_MAX_WAIT 7
#define QSV_MIN_SLICE 8
#define QSV_MAX_SLICE 9
#define QSV_PAGE_SIZE 10
#define QSV_VERSION_MAJOR 11
#define QSV_VERSION_MINOR 12
#define QSV_VERSION_REVISION 13
#define QSV_MS_COUNT 14
#define QSV_TIME_LOW 15
#define QSV_TIME_HIGH 16
#define QSV_TOTPHYSMEM 17
#define QSV_TOTRESMEM 18
#define QSV_TOTAVAILMEM 19
#define QSV_MAXPRMEM 20
#define QSV_MAXSHMEM 21
#define QSV_TIMER_INTERVAL 22
#define QSV_MAX_COMP_LENGTH 23
#define QSV_FOREGROUND_FS_SESSION 24
#define QSV_FOREGROUND_PROCESS 25
#define QSV_MAX QSV_FOREGROUND_PROCESS
APIRET APIENTRY DosQuerySysInfo(ULONG iStart, ULONG iLast, PVOID pBuf, ULONG cbBuf);

/* Memory */
#define PAG_READ      0x0001
#define PAG_WRITE     0x0002
#define PAG_EXECUTE   0x0004
#define PAG_GUARD     0x0008
#define PAG_COMMIT    0x0010
#define PAG_DECOMMIT  0x0020
#define OBJ_TILE      0x0040
#define OBJ_PROTECTED 0x0080
#define OBJ_GETTABLE  0x0100
#define OBJ_GIVEABLE  0x0200
#define PAG_DEFAULT   0x0400
#define PAG_SHARED    0x2000
#define PAG_FREE      0x4000
#define PAG_BASE      0x00010000
#define fPERM         (PAG_EXECUTE | PAG_READ | PAG_WRITE)
#define fSHARE        (OBJ_GETTABLE | OBJ_GIVEABLE)
APIRET APIENTRY DosAllocMem(PPVOID ppb, ULONG cb, ULONG flag);
APIRET APIENTRY DosFreeMem(PVOID pb);
APIRET APIENTRY DosSetMem(PVOID pb, ULONG cb, ULONG flag);
APIRET APIENTRY DosQueryMem(PVOID pb, PULONG pcb, PULONG pFlag);
APIRET APIENTRY DosAllocSharedMem(PPVOID ppb, PCSZ pszName, ULONG cb, ULONG flag);
APIRET APIENTRY DosGetNamedSharedMem(PPVOID ppb, PCSZ pszName, ULONG flag);
APIRET APIENTRY DosGetSharedMem(PVOID pb, ULONG flag);
APIRET APIENTRY DosGiveSharedMem(PVOID pb, PID pid, ULONG flag);
#define DOSSUB_INIT       0x01
#define DOSSUB_GROW       0x02
#define DOSSUB_SPARSE_OBJ 0x04
#define DOSSUB_SERIALIZE  0x08
APIRET APIENTRY DosSubSetMem(PVOID pbBase, ULONG flag, ULONG cb);
APIRET APIENTRY DosSubAllocMem(PVOID pbBase, PPVOID ppb, ULONG cb);
APIRET APIENTRY DosSubFreeMem(PVOID pbBase, PVOID pb, ULONG cb);
APIRET APIENTRY DosSubUnsetMem(PVOID pbBase);

/* Modules */
APIRET APIENTRY DosLoadModule(PSZ pszName, ULONG cbName, PCSZ pszModname, PHMODULE phmod);
APIRET APIENTRY DosFreeModule(HMODULE hmod);
APIRET APIENTRY DosQueryProcAddr(HMODULE hmod, ULONG ordinal, PCSZ pszName, PFN *ppfn);
APIRET APIENTRY DosQueryModuleHandle(PCSZ pszModname, PHMODULE phmod);
APIRET APIENTRY DosQueryModuleName(HMODULE hmod, ULONG cbName, PCHAR pch);

/* Semaphores */
#define SEM_INDEFINITE_WAIT  ((ULONG)-1)
#define SEM_IMMEDIATE_RETURN 0
#define DC_SEM_SHARED        0x01
APIRET APIENTRY DosCreateEventSem(PCSZ pszName, PHEV phev, ULONG flAttr, BOOL32 fState);
APIRET APIENTRY DosOpenEventSem(PCSZ pszName, PHEV phev);
APIRET APIENTRY DosCloseEventSem(HEV hev);
APIRET APIENTRY DosResetEventSem(HEV hev, PULONG pulPostCt);
APIRET APIENTRY DosPostEventSem(HEV hev);
APIRET APIENTRY DosWaitEventSem(HEV hev, ULONG ulTimeout);
APIRET APIENTRY DosQueryEventSem(HEV hev, PULONG pulPostCt);
APIRET APIENTRY DosCreateMutexSem(PCSZ pszName, PHMTX phmtx, ULONG flAttr, BOOL32 fState);
APIRET APIENTRY DosOpenMutexSem(PCSZ pszName, PHMTX phmtx);
APIRET APIENTRY DosCloseMutexSem(HMTX hmtx);
APIRET APIENTRY DosRequestMutexSem(HMTX hmtx, ULONG ulTimeout);
APIRET APIENTRY DosReleaseMutexSem(HMTX hmtx);

/* Time */
typedef struct _DATETIME {
    UCHAR  hours, minutes, seconds, hundredths;
    UCHAR  day, month;
    USHORT year;
    SHORT  timezone;
    UCHAR  weekday;
} DATETIME, *PDATETIME;
APIRET APIENTRY DosGetDateTime(PDATETIME pdt);
APIRET APIENTRY DosSetDateTime(PDATETIME pdt);
APIRET APIENTRY DosSleep(ULONG msec);
APIRET APIENTRY DosBeep(ULONG freq, ULONG dur);

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

#define FILE_EXISTED   1
#define FILE_CREATED   2
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

typedef struct _EAOP2 *PEAOP2;
APIRET APIENTRY DosOpen(PCSZ pszFileName, PHFILE pHf, PULONG pulAction, ULONG cbFile,
                        ULONG ulAttribute, ULONG fsOpenFlags, ULONG fsOpenMode,
                        PEAOP2 peaop2);
APIRET APIENTRY DosClose(HFILE hFile);
APIRET APIENTRY DosRead(HFILE hFile, PVOID pBuffer, ULONG cbRead, PULONG pcbActual);
APIRET APIENTRY DosWrite(HFILE hFile, const VOID *pBuffer, ULONG cbWrite, PULONG pcbActual);
APIRET APIENTRY DosSetFilePtr(HFILE hFile, LONG ib, ULONG method, PULONG ibActual);
APIRET APIENTRY DosSetFileSize(HFILE hFile, ULONG cbSize);
APIRET APIENTRY DosResetBuffer(HFILE hFile);
APIRET APIENTRY DosDupHandle(HFILE hFile, PHFILE pHfile);
APIRET APIENTRY DosSetMaxFH(ULONG cFH);
APIRET APIENTRY DosQueryFHState(HFILE hFile, PULONG pMode);
APIRET APIENTRY DosSetFHState(HFILE hFile, ULONG mode);
#define HANDTYPE_FILE    0x0000
#define HANDTYPE_DEVICE  0x0001
#define HANDTYPE_PIPE    0x0002
#define HANDTYPE_NETWORK 0x8000
APIRET APIENTRY DosQueryHType(HFILE hFile, PULONG pType, PULONG pAttr);
APIRET APIENTRY DosCreatePipe(PHFILE phfRead, PHFILE phfWrite, ULONG cb);

APIRET APIENTRY DosDelete(PCSZ pszFile);
APIRET APIENTRY DosMove(PCSZ pszOld, PCSZ pszNew);
#define DCPY_EXISTING 0x0001
#define DCPY_APPEND   0x0002
APIRET APIENTRY DosCopy(PCSZ pszOld, PCSZ pszNew, ULONG option);
APIRET APIENTRY DosCreateDir(PCSZ pszDirName, PEAOP2 peaop2);
APIRET APIENTRY DosDeleteDir(PCSZ pszDir);
APIRET APIENTRY DosSetCurrentDir(PCSZ pszDir);
APIRET APIENTRY DosQueryCurrentDir(ULONG disknum, PBYTE pBuf, PULONG pcbBuf);
APIRET APIENTRY DosQueryCurrentDisk(PULONG pdisknum, PULONG plogical);
APIRET APIENTRY DosSetDefaultDisk(ULONG disknum);

typedef struct _FDATE {
    USHORT day : 5;
    USHORT month : 4;
    USHORT year : 7;
} FDATE, *PFDATE;
typedef struct _FTIME {
    USHORT twosecs : 5;
    USHORT minutes : 6;
    USHORT hours : 5;
} FTIME, *PFTIME;
#pragma pack(push, 1)
typedef struct _FILESTATUS3 {
    FDATE fdateCreation;
    FTIME ftimeCreation;
    FDATE fdateLastAccess;
    FTIME ftimeLastAccess;
    FDATE fdateLastWrite;
    FTIME ftimeLastWrite;
    ULONG cbFile;
    ULONG cbFileAlloc;
    ULONG attrFile;
} FILESTATUS3, *PFILESTATUS3;
typedef struct _FILEFINDBUF3 {
    ULONG oNextEntryOffset;
    FDATE fdateCreation;
    FTIME ftimeCreation;
    FDATE fdateLastAccess;
    FTIME ftimeLastAccess;
    FDATE fdateLastWrite;
    FTIME ftimeLastWrite;
    ULONG cbFile;
    ULONG cbFileAlloc;
    ULONG attrFile;
    UCHAR cchName;
    CHAR  achName[256];
} FILEFINDBUF3, *PFILEFINDBUF3;
#pragma pack(pop)
#define FIL_STANDARD     1
#define FIL_QUERYEASIZE  2
#define FIL_QUERYFULLNAME 5
APIRET APIENTRY DosQueryFileInfo(HFILE hf, ULONG ulInfoLevel, PVOID pInfo, ULONG cbInfoBuf);
APIRET APIENTRY DosSetFileInfo(HFILE hf, ULONG ulInfoLevel, PVOID pInfo, ULONG cbInfoBuf);
APIRET APIENTRY DosQueryPathInfo(PCSZ pszPathName, ULONG ulInfoLevel, PVOID pInfo,
                                 ULONG cbInfoBuf);
APIRET APIENTRY DosSetPathInfo(PCSZ pszPathName, ULONG ulInfoLevel, PVOID pInfo,
                               ULONG cbInfoBuf, ULONG flOptions);

#define HDIR_SYSTEM 1
#define HDIR_CREATE ((HDIR)-1)
APIRET APIENTRY DosFindFirst(PCSZ pszFileSpec, PHDIR phdir, ULONG flAttribute,
                             PVOID pfindbuf, ULONG cbBuf, PULONG pcFileNames,
                             ULONG ulInfoLevel);
APIRET APIENTRY DosFindNext(HDIR hDir, PVOID pfindbuf, ULONG cbfindbuf, PULONG pcFilenames);
APIRET APIENTRY DosFindClose(HDIR hDir);

/* Devices and errors */
APIRET APIENTRY DosDevIOCtl(HFILE hDevice, ULONG category, ULONG function, PVOID pParams,
                            ULONG cbParmLenMax, PULONG pcbParmLen, PVOID pData,
                            ULONG cbDataLenMax, PULONG pcbDataLen);
APIRET APIENTRY DosDevConfig(PVOID pdevinfo, ULONG item);
#define FERR_DISABLEHARDERR   0x0000
#define FERR_ENABLEHARDERR    0x0001
#define FERR_ENABLEEXCEPTION  0x0000
#define FERR_DISABLEEXCEPTION 0x0002
APIRET APIENTRY DosError(ULONG error);

/* Messages (MSG.DLL) */
APIRET APIENTRY DosPutMessage(HFILE hfile, ULONG cbMsg, PCHAR pBuf);
APIRET APIENTRY DosGetMessage(PCHAR *pTable, ULONG cTable, PCHAR pBuf, ULONG cbBuf,
                              ULONG msgnumber, PCSZ pszFile, PULONG pcbMsg);

#ifdef __cplusplus
}
#endif

#endif /* __BSEDOS_H */
