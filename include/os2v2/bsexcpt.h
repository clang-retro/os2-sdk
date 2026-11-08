/*
 * bsexcpt.h - exceptions and signals of 32-bit OS/2 (2.0 and later), for
 * clang-retro (i386-pc-os2): the exception numbers, the report,
 * registration and context records, and what a handler returns. The
 * calls (DosSetExceptionHandler and the rest) are in bsedos.h.
 *
 * Copyright (C) clang-retro contributors.
 * SPDX-License-Identifier: AGPL-3.0-only
 *
 * A handler is
 *
 *   ULONG APIENTRY handler(PEXCEPTIONREPORTRECORD, PEXCEPTIONREGISTRATIONRECORD,
 *                          PCONTEXTRECORD, PVOID);
 *
 * and its EXCEPTIONREGISTRATIONRECORD must be on the stack of the thread
 * that registers it, and be unset before that function returns.
 */
#ifndef __BSEXCPT_H
#define __BSEXCPT_H

#include <os2def.h>

#ifdef __cplusplus
extern "C" {
#endif

/* What a handler returns */
#define XCPT_CONTINUE_SEARCH    0x00000000
#define XCPT_CONTINUE_EXECUTION 0xFFFFFFFF
#define XCPT_CONTINUE_STOP      0x00716668

/* fHandlerFlags */
#define EH_NONCONTINUABLE 0x01
#define EH_UNWINDING      0x02
#define EH_EXIT_UNWIND    0x04
#define EH_STACK_INVALID  0x08
#define EH_NESTED_CALL    0x10

/* Exception numbers. Bits 31-30 give the severity. */
#define XCPT_FATAL_EXCEPTION    0xC0000000
#define XCPT_SEVERITY_CODE      0xC0000000
#define XCPT_CUSTOMER_CODE      0x20000000
#define XCPT_FACILITY_CODE      0x1FFF0000
#define XCPT_EXCEPTION_CODE     0x0000FFFF

#define XCPT_GUARD_PAGE_VIOLATION     0x80000001
#define XCPT_UNABLE_TO_GROW_STACK     0x80010001
#define XCPT_DATATYPE_MISALIGNMENT    0xC000009E
#define XCPT_BREAKPOINT               0xC000009F
#define XCPT_SINGLE_STEP              0xC00000A0
#define XCPT_ACCESS_VIOLATION         0xC0000005
#define XCPT_ILLEGAL_INSTRUCTION      0xC000001C
#define XCPT_FLOAT_DENORMAL_OPERAND   0xC0000094
#define XCPT_FLOAT_DIVIDE_BY_ZERO     0xC0000095
#define XCPT_FLOAT_INEXACT_RESULT     0xC0000096
#define XCPT_FLOAT_INVALID_OPERATION  0xC0000097
#define XCPT_FLOAT_OVERFLOW           0xC0000098
#define XCPT_FLOAT_STACK_CHECK        0xC0000099
#define XCPT_FLOAT_UNDERFLOW          0xC000009A
#define XCPT_INTEGER_DIVIDE_BY_ZERO   0xC000009B
#define XCPT_INTEGER_OVERFLOW         0xC000009C
#define XCPT_PRIVILEGED_INSTRUCTION   0xC000009D
#define XCPT_IN_PAGE_ERROR            0xC0000006
#define XCPT_PROCESS_TERMINATE        0xC0010001
#define XCPT_ASYNC_PROCESS_TERMINATE  0xC0010002
#define XCPT_NONCONTINUABLE_EXCEPTION 0xC0000024
#define XCPT_INVALID_DISPOSITION      0xC0000025
#define XCPT_INVALID_LOCK_SEQUENCE    0xC000001D
#define XCPT_ARRAY_BOUNDS_EXCEEDED    0xC0000093
#define XCPT_B1NPX_ERRATA_02          0xC0010004
#define XCPT_UNWIND                   0xC0000026
#define XCPT_BAD_STACK                0xC0000027
#define XCPT_INVALID_UNWIND_TARGET    0xC0000028
#define XCPT_SIGNAL                   0xC0010003

/* ExceptionInfo[0] of an access violation */
#define XCPT_UNKNOWN_ACCESS 0x00000000
#define XCPT_READ_ACCESS    0x00000001
#define XCPT_WRITE_ACCESS   0x00000002
#define XCPT_EXECUTE_ACCESS 0x00000004
#define XCPT_SPACE_ACCESS   0x00000008
#define XCPT_LIMIT_ACCESS   0x00000010
#define XCPT_DATA_UNKNOWN   0xFFFFFFFF

/* ExceptionInfo[0] of XCPT_SIGNAL */
#define XCPT_SIGNAL_INTR     1
#define XCPT_SIGNAL_KILLPROC 3
#define XCPT_SIGNAL_BREAK    4

/* ContextFlags */
#define CONTEXT_CONTROL        0x00000001
#define CONTEXT_INTEGER        0x00000002
#define CONTEXT_SEGMENTS       0x00000004
#define CONTEXT_FLOATING_POINT 0x00000008
#define CONTEXT_FULL           0x0000000F

#pragma pack(push, 1)
typedef struct _FPREG {
    ULONG  losig;
    ULONG  hisig;
    USHORT signexp;
} FPREG, *PFPREG;
#pragma pack(pop)

typedef struct _CONTEXTRECORD {
    ULONG ContextFlags;
    ULONG ctx_env[7];        /* the FSAVE image: environment, */
    FPREG ctx_stack[8];      /* and the eight registers */
    ULONG ctx_SegGs;
    ULONG ctx_SegFs;
    ULONG ctx_SegEs;
    ULONG ctx_SegDs;
    ULONG ctx_RegEdi;
    ULONG ctx_RegEsi;
    ULONG ctx_RegEax;
    ULONG ctx_RegEbx;
    ULONG ctx_RegEcx;
    ULONG ctx_RegEdx;
    ULONG ctx_RegEbp;
    ULONG ctx_RegEip;
    ULONG ctx_SegCs;
    ULONG ctx_EFlags;
    ULONG ctx_RegEsp;
    ULONG ctx_SegSs;
} CONTEXTRECORD, *PCONTEXTRECORD;

#define EXCEPTION_MAXIMUM_PARAMETERS 4

typedef struct _EXCEPTIONREPORTRECORD {
    ULONG  ExceptionNum;
    ULONG  fHandlerFlags;
    struct _EXCEPTIONREPORTRECORD *NestedExceptionReportRecord;
    PVOID  ExceptionAddress;
    ULONG  cParameters;
    ULONG  ExceptionInfo[EXCEPTION_MAXIMUM_PARAMETERS];
} EXCEPTIONREPORTRECORD, *PEXCEPTIONREPORTRECORD;

struct _EXCEPTIONREGISTRATIONRECORD;
typedef ULONG APIENTRY _ERR(PEXCEPTIONREPORTRECORD,
                            struct _EXCEPTIONREGISTRATIONRECORD *,
                            PCONTEXTRECORD, PVOID);
typedef _ERR *PFNERR;

typedef struct _EXCEPTIONREGISTRATIONRECORD {
    struct _EXCEPTIONREGISTRATIONRECORD *prev_structure;
    _ERR *ExceptionHandler;
} EXCEPTIONREGISTRATIONRECORD, *PEXCEPTIONREGISTRATIONRECORD;

/* The end of a thread's chain of handlers */
#define END_OF_CHAIN ((PEXCEPTIONREGISTRATIONRECORD)-1)

#ifdef __cplusplus
}
#endif

#endif /* __BSEXCPT_H */
