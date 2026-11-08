/*
 * dosapi.c - exercise the 32-bit Dos API on a running OS/2: exceptions,
 * semaphores and muxwait lists, queues, named pipes, threads, timers,
 * national language support, file systems, extended attributes and
 * messages. Each check prints "ok" or "FAIL" and the program returns the
 * number of failures.
 *
 * Copyright (C) clang-retro contributors.
 * SPDX-License-Identifier: AGPL-3.0-only
 */
#include <os2.h>
#include <setjmp.h>
#include <stdio.h>
#include <string.h>

static int failures;

static void check(const char *what, int ok, unsigned long detail)
{
    printf("%-28s %s (%lu)\n", what, ok ? "ok" : "FAIL", detail);
    if (!ok)
        failures++;
}

/* Exceptions: a handler that recovers from an access violation with
   longjmp, and one that continues after a raised exception. */
typedef struct {
    EXCEPTIONREGISTRATIONRECORD reg;
    jmp_buf env;
    ULONG seen;
} RECOVER;

static ULONG APIENTRY recover(PEXCEPTIONREPORTRECORD rep,
                              PEXCEPTIONREGISTRATIONRECORD reg,
                              PCONTEXTRECORD ctx, PVOID unused)
{
    RECOVER *r = (RECOVER *)reg;
    (void)ctx;
    (void)unused;
    if (rep->fHandlerFlags & (EH_UNWINDING | EH_EXIT_UNWIND))
        return XCPT_CONTINUE_SEARCH;
    r->seen = rep->ExceptionNum;
    if (rep->ExceptionNum == XCPT_ACCESS_VIOLATION)
        longjmp(r->env, 1);
    if (rep->ExceptionNum == 0x20000001)   /* ours, continuable */
        return XCPT_CONTINUE_EXECUTION;
    return XCPT_CONTINUE_SEARCH;
}

static void exceptions(void)
{
    RECOVER r;
    volatile int *bad = (volatile int *)16;
    EXCEPTIONREPORTRECORD rep;

    memset(&r, 0, sizeof r);
    r.reg.ExceptionHandler = recover;
    check("DosSetExceptionHandler", DosSetExceptionHandler(&r.reg) == NO_ERROR, 0);
    if (setjmp(r.env) == 0) {
        *bad = 1;
        check("access violation caught", 0, 0);
    } else {
        check("access violation caught", r.seen == XCPT_ACCESS_VIOLATION, r.seen);
    }
    memset(&rep, 0, sizeof rep);
    rep.ExceptionNum = 0x20000001;
    r.seen = 0;
    DosRaiseException(&rep);
    check("DosRaiseException", r.seen == 0x20000001, r.seen);
    check("DosUnsetExceptionHandler", DosUnsetExceptionHandler(&r.reg) == NO_ERROR, 0);
}

static HEV posted;

static VOID APIENTRY poster(ULONG arg)
{
    DosSleep(50);
    DosPostEventSem((HEV)arg);
}

static void semaphores(void)
{
    HEV a, b;
    HMUX mux;
    SEMRECORD recs[2];
    ULONG user = 0, n = 0, count = 99;
    HMTX mtx;
    PID pid;
    TID tid, thread;
    APIRET rc;

    DosCreateEventSem(NULL, &a, 0, FALSE);
    DosCreateEventSem(NULL, &b, 0, FALSE);
    recs[0].hsemCur = (HSEM)a;
    recs[0].ulUser = 1;
    recs[1].hsemCur = (HSEM)b;
    recs[1].ulUser = 2;
    rc = DosCreateMuxWaitSem(NULL, &mux, 2, recs, DCMW_WAIT_ANY);
    check("DosCreateMuxWaitSem", rc == NO_ERROR, rc);
    rc = DosCreateThread(&thread, poster, (ULONG)b, CREATE_READY | STACK_SPARSE, 16384);
    check("DosCreateThread", rc == NO_ERROR, rc);
    rc = DosWaitMuxWaitSem(mux, 5000, &user);
    check("DosWaitMuxWaitSem", rc == NO_ERROR && user == 2, user);
    DosWaitThread(&thread, DCWW_WAIT);
    n = 2;
    rc = DosQueryMuxWaitSem(mux, &n, recs, &user);
    check("DosQueryMuxWaitSem", rc == NO_ERROR && n == 2, n);
    DosCloseMuxWaitSem(mux);
    DosCloseEventSem(a);
    DosCloseEventSem(b);

    DosCreateMutexSem(NULL, &mtx, 0, TRUE);
    rc = DosQueryMutexSem(mtx, &pid, &tid, &count);
    check("DosQueryMutexSem", rc == NO_ERROR && count == 1, count);
    DosReleaseMutexSem(mtx);
    DosCloseMutexSem(mtx);
    (void)posted;
}

static void queues(void)
{
    HQUEUE q;
    REQUESTDATA req;
    ULONG cb, n = 0;
    PVOID data;
    BYTE prio;
    static char first[] = "first", second[] = "second";
    APIRET rc;

    rc = DosCreateQueue(&q, QUE_FIFO | QUE_NOCONVERT_ADDRESS, "\\QUEUES\\CLANGRT");
    check("DosCreateQueue", rc == NO_ERROR, rc);
    if (rc)
        return;
    DosWriteQueue(q, 7, sizeof first, first, 0);
    DosWriteQueue(q, 8, sizeof second, second, 0);
    DosQueryQueue(q, &n);
    check("DosQueryQueue", n == 2, n);
    rc = DosReadQueue(q, &req, &cb, &data, 0, DCWW_WAIT, &prio, 0);
    check("DosReadQueue", rc == NO_ERROR && req.ulData == 7 &&
          strcmp((char *)data, "first") == 0, req.ulData);
    DosPurgeQueue(q);
    DosQueryQueue(q, &n);
    check("DosPurgeQueue", n == 0, n);
    DosCloseQueue(q);
}

#define PIPE "\\PIPE\\CLANGRT"

static VOID APIENTRY pipe_server(ULONG arg)
{
    HPIPE p = (HPIPE)arg;
    char buf[32];
    ULONG got = 0, put;

    if (DosConnectNPipe(p) == NO_ERROR &&
        DosRead(p, buf, sizeof buf, &got) == NO_ERROR) {
        buf[0] = 'P';  /* "ping" becomes "Ping" */
        DosWrite(p, buf, got, &put);
    }
    DosResetBuffer(p);
}

static void pipes(void)
{
    HPIPE p;
    TID thread;
    char reply[32] = "";
    ULONG got = 0;
    APIRET rc;

    rc = DosCreateNPipe(PIPE, &p, NP_ACCESS_DUPLEX,
                        NP_WAIT | NP_TYPE_MESSAGE | NP_READMODE_MESSAGE | 1,
                        256, 256, 0);
    check("DosCreateNPipe", rc == NO_ERROR, rc);
    if (rc)
        return;
    DosCreateThread(&thread, pipe_server, (ULONG)p, CREATE_READY, 16384);
    DosSleep(50);
    rc = DosCallNPipe(PIPE, "ping", 4, reply, sizeof reply - 1, &got, 5000);
    reply[got < sizeof reply ? got : sizeof reply - 1] = 0;
    check("DosCallNPipe", rc == NO_ERROR && strcmp(reply, "Ping") == 0, rc);
    DosWaitThread(&thread, DCWW_WAIT);
    DosDisConnectNPipe(p);
    DosClose(p);
}

static void misc(void)
{
    ULONG freq = 0, cps[4], ncp = 0, cb = 0, flags = 0, *tls = 0;
    QWORD t1, t2;
    COUNTRYCODE cc = { 0, 0 };
    COUNTRYINFO ci;
    FSALLOCATE fsa;
    BYTE fsq[256];
    char msg[256];
    APIRET rc;
    PPIB pib;
    PTIB tib;

    rc = DosTmrQueryFreq(&freq);
    DosTmrQueryTime(&t1);
    DosSleep(32);
    DosTmrQueryTime(&t2);
    check("DosTmrQueryTime", rc == NO_ERROR && freq != 0 &&
          (t2.ulHi > t1.ulHi || t2.ulLo > t1.ulLo), freq);

    rc = DosQueryCp(sizeof cps, cps, &ncp);
    check("DosQueryCp", rc == NO_ERROR && ncp >= sizeof(ULONG), cps[0]);
    rc = DosQueryCtryInfo(sizeof ci, &cc, &ci, &cb);
    check("DosQueryCtryInfo", rc == NO_ERROR && ci.country != 0, ci.country);

    rc = DosQueryFSInfo(0, FSIL_ALLOC, &fsa, sizeof fsa);
    check("DosQueryFSInfo", rc == NO_ERROR && fsa.cbSector == 512, fsa.cbSector);
    cb = sizeof fsq;
    rc = DosQueryFSAttach("B:", 0, FSAIL_QUERYNAME, (PFSQBUFFER2)fsq, &cb);
    check("DosQueryFSAttach", rc == NO_ERROR &&
          ((PFSQBUFFER2)fsq)->iType == FSAT_LOCALDRV, rc);

    DosGetInfoBlocks(&tib, &pib);
    DosQueryModuleName(pib->pib_hmte, sizeof msg, msg);
    rc = DosQueryAppType(msg, &flags);
    check("DosQueryAppType", rc == NO_ERROR &&
          (flags & FAPPTYP_32BIT), flags);

    rc = DosAllocThreadLocalMemory(1, &tls);
    if (rc == NO_ERROR) {
        *tls = 42;
        DosFreeThreadLocalMemory(tls);
    }
    check("DosAllocThreadLocalMemory", rc == NO_ERROR, rc);

    /* XDF.MSG is on the first Warp 3 boot diskette; runtests.sh is given
       it to copy to B:. Its first message, XDF3502, is
       "Press any key to continue". */
    cb = 0;
    rc = DosGetMessage(NULL, 0, msg, sizeof msg - 1, 3502, "B:\\XDF.MSG", &cb);
    msg[cb < sizeof msg ? cb : sizeof msg - 1] = 0;
    printf("  rc %lu, %lu bytes: %.60s\n", rc, cb, msg);
    check("DosGetMessage", rc == NO_ERROR && cb > 0 && memcmp(msg, "Press any key", 13) == 0, cb);
}

static void eas(void)
{
    static const char name[] = "CLANG.RETRO";
    static const char value[] = "hello";
    char buf[512];
    PFEA2LIST list = (PFEA2LIST)buf;
    PFEA2 fea = list->list;
    EAOP2 op;
    FILESTATUS4 fs4;
    HFILE hf;
    ULONG action, count = 1;
    APIRET rc;

    rc = DosOpen("B:\\EATEST.TMP", &hf, &action, 0, FILE_NORMAL,
                 OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_REPLACE_IF_EXISTS,
                 OPEN_ACCESS_READWRITE | OPEN_SHARE_DENYNONE, NULL);
    if (rc != NO_ERROR) {
        check("EAs: DosOpen", 0, rc);
        return;
    }
    DosClose(hf);
    memset(buf, 0, sizeof buf);
    fea->oNextEntryOffset = 0;
    fea->fEA = 0;
    fea->cbName = sizeof name - 1;
    fea->cbValue = sizeof value - 1;
    memcpy(fea->szName, name, sizeof name);
    memcpy(fea->szName + sizeof name, value, sizeof value - 1);
    list->cbList = sizeof(ULONG) + 8 + sizeof name + sizeof value - 1;
    op.fpGEA2List = NULL;
    op.fpFEA2List = list;
    op.oError = 0;
    rc = DosSetPathInfo("B:\\EATEST.TMP", FIL_QUERYEASIZE, &op, sizeof op, 0);
    check("DosSetPathInfo (EA)", rc == NO_ERROR, rc);
    rc = DosQueryPathInfo("B:\\EATEST.TMP", FIL_QUERYEASIZE, &fs4, sizeof fs4);
    check("FIL_QUERYEASIZE", rc == NO_ERROR && fs4.cbList > 4, fs4.cbList);
    rc = DosEnumAttribute(ENUMEA_REFTYPE_PATH, "B:\\EATEST.TMP", 1, buf,
                          sizeof buf, &count, ENUMEA_LEVEL_NO_VALUE);
    check("DosEnumAttribute", rc == NO_ERROR && count == 1 &&
          strcmp(((PDENA2)buf)->szName, name) == 0, count);
    DosDelete("B:\\EATEST.TMP");
}

int main(void)
{
    exceptions();
    semaphores();
    queues();
    pipes();
    misc();
    eas();
    printf("%d failed\n", failures);
    return failures;
}
