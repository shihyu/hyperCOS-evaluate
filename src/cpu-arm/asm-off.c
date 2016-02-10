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

#ifdef ASM_UTIL

#include "task.h"
#include "sem.h"
#include "mut.h"
#include "cpu/reg.h"

#define add(_n, _v) \
	asm volatile("define " #_n " %0" : : "i" (_v))

#define off(_t, _m) \
	(unsigned)(&((_t*)((void*)0))->_m)

int main(void)
{
	add(mut_val, off(mut_t, val));
	add(mut_own, off(mut_t, own));
	add(mut_own_pri, off(mut_t, own_pri));
	add(sem_val, off(sem_t, val));

	add(task_pri, off(task_t, pri));
	add(task_context, off(task_t, context));
	add(task_stack, off(task_t, stack));
	add(task_stack_sz, off(task_t, stack_sz));

	add(reg_irq_sz, sizeof(reg_irq_t));
	add(reg_irq_pc, off(reg_irq_t, pc));
	add(reg_irq_cpsr, off(reg_irq_t, cpsr));
#if CFG_IRQ_VECTS
	add(reg_irq_sz, sizeof(reg_irq_t));
	add(reg_irq_lr, off(reg_irq_t, lr));
	add(reg_sz, sizeof(reg_t));
	add(reg_pc, off(reg_t, pc));
#else
	add(reg_sz, sizeof(reg_t));
	add(reg_r0, off(reg_t, r0));
	add(reg_r1, off(reg_t, r1));
	add(reg_r2, off(reg_t, r2));
	add(reg_r3, off(reg_t, r3));
	add(reg_r4, off(reg_t, r4));
	add(reg_r11, off(reg_t, r11));
	add(reg_r12, off(reg_t, r12));
	add(reg_lr, off(reg_t, lr));
	add(reg_pc, off(reg_t, pc));
	add(reg_cpsr, off(reg_t, cpsr));
#endif
#ifdef REG_FIQ_T_DEF
	add(reg_fiq_pc, off(reg_fiq_t, pc));
	add(reg_fiq_cpsr, off(reg_fiq_t, cpsr));
#endif
	return 0;
}

#endif
