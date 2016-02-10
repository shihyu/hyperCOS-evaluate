/*-****************************************************************************/
/*-                                                                           */
/*-            Copyright (c) of hyperCOS.                                     */
/*-                                                                           */
/*-  This software is copyrighted by and is the sole property of its authors. */
/*-  All rights, title, ownership, or other interests in the software remain  */
/*-  the property of the authors. The source code is for FREE short-term      */
/*-  evaluation, educational or non-commercial research only. Any commercial  */
/*-  application may only be used in accordance with the corresponding license*/
/*-  agreement. Any unauthorized use, duplication, transmission, distribution,*/
/*-  or disclosure of this software is expressly forbidden.                   */
/*-                                                                           */
/*-  Knowledge of the source code may NOT be used to develop a similar product*/
/*-                                                                           */
/*-  This Copyright notice may not be removed or modified without prior       */
/*-  written consent of the authors.                                          */
/*-                                                                           */
/*-  The authors reserves the right to modify this software                   */
/*-  without notice.                                                          */
/*-                                                                           */
/*-  To email the authors:                                                    */
/*-                                                                           */
/*-             socware.help@gmail.com                                        */
/*-                                                                           */
/*-****************************************************************************/

#include "core.h"
#include "task.h"
#include "cpu-arm/asm.h"
#include "cpu-arm/_cpu.h"
#include "cpu-arm/reg.h"
#include <string.h>

void *cpu_cur_pc(unsigned line)
{
	return __builtin_return_address(0);
}

void cpu_set_stack(unsigned mode, void *stack)
{
	unsigned cpsr_old, cpsr_new;
	mode &= 0x1f;
	asm volatile (""
		      asm_beg(%0)
		      "mrs %0, CPSR    \n"
		      "bic %1, %0,#0x1f\n"
		      "orr %1, %1,%2   \n"
		      "msr CPSR_cxsf,%1\n"
		      "mov sp, %3      \n" "msr CPSR, %0    \n" asm_end(%0)
		      :"=&r"(cpsr_old), "=&r"(cpsr_new)
		      :"r"(mode), "r"(stack));
}

void cpu_init(void)
{
	unsigned tmp;
	// trapping in arm mode
	asm volatile (""
		      asm_beg(r2)
		      "mrc p15, 0, %0, c1, c0, 0\n"
		      "bic %0, #0x40000000      \n"
		      "mcr p15, 0, %0, c1, c0, 0\n" asm_end(r2)
		      :"=r"(tmp)
		      ::"r2");

	extern unsigned _vects[];
	// load vector base
	asm volatile ("" asm_beg(r2) "mcr p15, 0, %0, c12, c0, 0\n" asm_end(r2)
		      ::"r"(((unsigned)(&_vects)) - 0x20)
		      :"r2");
	cpu_set_stack(_IRQ, _alloc(CFG_STACK_IRQ) + CFG_STACK_IRQ);
	cpu_set_stack(_ABT, _alloc(CFG_STACK_ABT) + CFG_STACK_ABT);
	irqs = _alloc(CFG_IRQ_MAX * sizeof(irq_t));
}

static inline void *stack_top(task_t * t)
{
	return (((char *)t->stack) + t->stack_sz);
}

reg_t *_cpu_task_init(task_t * t, void *priv, void *dest)
{
	reg_t *r = stack_top(t) - sizeof(reg_t);
	memset(r, 0, sizeof(reg_t));
	r->r0 = (unsigned)priv;
	r->lr = (unsigned)dest;
	r->pc = (unsigned)t->ent;
	r->cpsr = 0x13 | ((r->pc & 0x1) << 5);
	return r;
}

char *cpu_reg_names[] = {
	"r0", "r1", "r2", "r3",
	"r4", "r5", "r6", "r7",
	"r8", "r9", "r10", "r11",
	"r12", "lr",
#if CFG_ASM_RFE
	"pc", "cpsr"
#else
	"cpsr", "pc"
#endif
};
