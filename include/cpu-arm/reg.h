/*-****************************************************************************/
/*-                                                                           */
/*-            Copyright (c) of hyperCOS.                                     */
/*-                                                                           */
/*-  This software is copyrighted by and is the sole property of socware.net. */
/*-  All rights, title, ownership, or other interests in the software remain  */
/*-  the property of socware.net. The source code is FREE for short-term      */
/*-  evaluation, educational or non-commercial research only. Any commercial  */
/*-  application may only be used in accordance with the corresponding license*/
/*-  agreement. Any unauthorized use, duplication, transmission, distribution,*/
/*-  or disclosure of this software is expressly forbidden.                   */
/*-                                                                           */
/*-  Knowledge of the source code may NOT be used to develop a similar product*/
/*-                                                                           */
/*-  This Copyright notice may not be removed or modified without prior       */
/*-  written consent of socware.net.                                          */
/*-                                                                           */
/*-  socware.net reserves the right to modify this software                   */
/*-  without notice.                                                          */
/*-                                                                           */
/*-  To contact socware.net:                                                  */
/*-                                                                           */
/*-             socware.help@gmail.com                                        */
/*-                                                                           */
/*-****************************************************************************/

#ifndef REG150313
#define REG150313

#include "cfg.h"

typedef struct reg {
	unsigned r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12;
	unsigned lr;
#if CFG_ASM_RFE
	unsigned pc, cpsr;
#else
	unsigned cpsr, pc;
#endif
} reg_t;

typedef struct {
	unsigned r0, r1, r2, r3, r12;
#if CFG_ASM_RFE
	unsigned pc, cpsr;
#else
	unsigned cpsr, pc;
#endif
} reg_irq_t;

typedef struct {
	unsigned r0, r1, r2, r3;
#if CFG_ASM_RFE
	unsigned pc, cpsr;
#else
	unsigned cpsr, pc;
#endif
} reg_fiq_t;

#define REG_FIQ_T_DEF

/// skip one instruction in context
static inline void reg_skip(reg_t * context, int off)
{
	context->pc += off;
	context->pc += 4;
	context->pc -= 2 * ((context->cpsr >> 5) & 0x1);
}

#endif
