/*
 * call16.c - calls from 32-bit code into the 16-bit OS/2 API, for the
 * inline functions of bsesub.h. Part of libos2.a.
 *
 * Copyright (C) clang-retro contributors.
 * SPDX-License-Identifier: AGPL-3.0-only
 */

/*
 * Below 512 MiB every 64 KiB of a process's memory also has a 16-bit
 * selector of its own in the LDT, the "tile": linear address L is
 * ((L >> 16) << 3 | 7):(L & 0xffff). So 32-bit code can run 16-bit code
 * (a stub in a 16-bit segment of the program), on a 16-bit stack (a tile
 * of a block of its own), and pass 16:16 pointers to its data.
 *
 * Each thread gets a 16 KiB block the first time it calls: the lower half
 * holds copies of buffers that would cross a 64 KiB boundary, the upper
 * half is the 16-bit stack. DosAllocMem returns 64 KiB aligned blocks, so
 * the block is in one tile.
 */

#include <os2.h>
#include <string.h>

#define AREA_SIZE   0x4000
#define BOUNCE_SIZE 0x2000
#define SAVED_SIZE  28 /* what the stub returns through */
#define MAXBUFS     4

/* The thunk code: switches to the 16-bit stack at FRAME (the arguments,
   then SAVED_SIZE bytes that this fills in: where to return to, and the
   32-bit stack and segment registers) and jumps to STUB, 16-bit code that
   calls the function and comes back: "call far fn; o32 retf". Returns
   AX.

   The stubs (tools/gen-bsesub.py --stubs) are in a 16-bit code segment
   of the program, and the loader fixes up their far calls as it does
   16-bit programs'. They have to be direct calls: many 16-bit entry
   points are call gates, which ignore the offset of a far pointer, and the
   system finds the function from the offset in the instruction that
   called it. */
unsigned short __os2_call16_raw(const void *stub, void *frame, unsigned long nargs);

__asm__(".text\n"
        ".globl __os2_call16_raw\n"
        "__os2_call16_raw:\n"
        "  pushl %ebp\n"
        "  pushl %ebx\n"
        "  pushl %esi\n"
        "  pushl %edi\n"
        "  subl  $8, %esp\n"         /* the far pointer to the stub */
        "  movl  28(%esp), %ebx\n"   /* stub */
        "  movl  32(%esp), %edi\n"   /* frame */
        "  movl  36(%esp), %esi\n"
        "  addl  %edi, %esi\n"       /* the saved state, after the args */
        "  movl  $1f, (%esi)\n"      /* where the stub returns: 1: */
        "  xorl  %eax, %eax\n"
        "  movw  %cs, %ax\n"
        "  movl  %eax, 4(%esi)\n"
        "  movl  %esp, 8(%esi)\n"
        "  movw  %ss, %ax\n"
        "  movl  %eax, 12(%esi)\n"
        "  movw  %ds, %ax\n"
        "  movl  %eax, 16(%esi)\n"
        "  movw  %es, %ax\n"
        "  movl  %eax, 20(%esi)\n"
        "  movw  %fs, %ax\n"
        "  movl  %eax, 24(%esi)\n"
        "  movl  %ebx, %ecx\n"       /* the tile of the stub */
        "  andl  $0xffff, %ebx\n"
        "  movl  %ebx, (%esp)\n"
        "  shrl  $16, %ecx\n"
        "  leal  7(,%ecx,8), %ecx\n"
        "  movl  %ecx, 4(%esp)\n"
        "  movl  %esp, %ebp\n"
        "  movl  %edi, %eax\n"       /* the 16-bit stack */
        "  shrl  $16, %eax\n"
        "  leal  7(,%eax,8), %eax\n"
        "  andl  $0xffff, %edi\n"
        "  movw  %ax, %ss\n"
        "  movl  %edi, %esp\n"
        "  ljmp  *%ds:(%ebp)\n"     /* ebp would mean the new ss */
        "1:\n"                      /* back in 32-bit code, on the 16-bit stack */
        "  popl  %edx\n"
        "  popl  %ecx\n"
        "  popl  %ds\n"
        "  popl  %es\n"
        "  popl  %fs\n"
        "  movw  %cx, %ss\n"
        "  movl  %edx, %esp\n"
        "  addl  $8, %esp\n"
        "  movzwl %ax, %eax\n"
        "  popl  %edi\n"
        "  popl  %esi\n"
        "  popl  %ebx\n"
        "  popl  %ebp\n"
        "  ret\n");

static PULONG area_slot;

static char *
thread_area(void)
{
    if (area_slot == NULL) {
        DosEnterCritSec();
        if (area_slot == NULL && DosAllocThreadLocalMemory(1, &area_slot) != 0)
            area_slot = NULL;
        DosExitCritSec();
        if (area_slot == NULL)
            return NULL;
    }
    if (*area_slot == 0) {
        PVOID p;
        if (DosAllocMem(&p, AREA_SIZE, PAG_COMMIT | PAG_READ | PAG_WRITE) != 0)
            return NULL;
        *area_slot = (unsigned long)p;
    }
    return (char *)*area_slot;
}

unsigned long
__os2_ptr16(const void *p)
{
    unsigned long l = (unsigned long)p;
    if (p == NULL)
        return 0;
    return ((l >> 16) << 19) | 0x70000 | (l & 0xffff);
}

unsigned
__os2_call16(const void *stub, void *args, unsigned nargs, const struct __os2_buf16 *bufs,
             unsigned nbufs)
{
    char         *area = thread_area();
    char         *bounce[MAXBUFS];
    void         *orig[MAXBUFS];
    unsigned      used = 0, i;
    if (area == NULL || nbufs > MAXBUFS ||
        (nargs & 1) || nargs > 0x400)
        return 0xffff;
    /* Turn the flat pointers into 16:16 ones; a buffer that crosses a
       64 KiB boundary is copied into the area. */
    for (i = 0; i < nbufs; i++) {
        unsigned long *slot = (unsigned long *)((char *)args + bufs[i].argoff);
        unsigned long  l = *slot, n = bufs[i].size;
        bounce[i] = NULL;
        orig[i] = (void *)l;
        if (l != 0 && n > 1 && (l >> 16) != ((l + n - 1) >> 16)) {
            if (n > BOUNCE_SIZE - used)
                return 0xffff;
            bounce[i] = area + used;
            memcpy(bounce[i], (void *)l, n);
            used += (n + 3) & ~3u;
            l = (unsigned long)bounce[i];
        }
        *slot = __os2_ptr16((void *)l);
    }
    char *frame = area + AREA_SIZE - SAVED_SIZE - nargs;
    memcpy(frame, args, nargs);
    unsigned r = __os2_call16_raw(stub, frame, nargs);
    for (i = 0; i < nbufs; i++)
        if (bounce[i] && bufs[i].out)
            memcpy(orig[i], bounce[i], bufs[i].size);
    return r;
}
