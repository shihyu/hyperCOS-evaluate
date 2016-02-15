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

enum {
	ICSR = 0xE000ED04,
	VTOR = 0xE000ED08,
	STICK = 0xE000E010,
	CCR = 0xE000ED14,
	SHPR = 0xE000ED18,
	CFSR = 0xE000ED28,
	CPACR = 0xE000ED88,
	FPCCR = 0xE000EF34,
	FPCAR = 0xE000EF38,
};

typedef struct reg_irq {
	unsigned r0, r1, r2, r3, r12;
	unsigned lr;
	unsigned pc, cpsr;
} reg_irq_t;

typedef struct {
	unsigned fpscr[2];
	unsigned s[16];
} reg_fhard_t;

typedef struct {
	unsigned sh[16];
	reg_fhard_t h;
} reg_f_t;

typedef struct reg {
	unsigned eret;
	unsigned r4, r5, r6, r7, r8, r9, r10, r11;
	unsigned r0, r1, r2, r3, r12;
	unsigned lr;
	unsigned pc, cpsr;
} reg_t;

static inline void reg_skip(reg_t * context, int off)
{
	//FIXME
	//context->pc += 2;
}

#endif
