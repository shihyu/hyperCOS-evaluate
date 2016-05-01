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

#include "asm-off.h"
#include "cpu/asm.h"
#include "cpu/_irq.h"

	.asm_syn
	.text
	// @param r0  next_task->context
	// @param r1  &(cur_task->context)
	// @param r2   reg_irq_t* cur_irq
	.asm_fun _task_switch_irq
_task_switch_irq:
	// pop the irq stack
	add     sp, sp, #reg_irq_sz
	// get cur task sp
	ldr     r12, [r2, #reg_irq_cpsr]
	mrs     r3, cpsr
#if CFG_ASM_BFI
	bfi     r3, r12, #0, #5
#else
	bic     r3, r3, #0x1f
	and     r12,r12, #0x1f
	orr     r3, r3, r12
#endif
	orr     r3, r3, #0x80
	msr     cpsr, r3

	add     sp, sp, #-reg_sz
	str     sp, [r1]

	ldr     r3 , [r2, #reg_irq_pc]
	ldr     r12, [r2, #reg_irq_cpsr]

	str 	 lr, [sp, #reg_lr]
	str 	r3 , [sp, #reg_pc]
	str 	r12, [sp, #reg_cpsr]

	mov     lr, r0
	ldmia   r2, {r0-r3,r12}
	stmia   sp, {r0-r12}
	mov     r0, lr

	b       _task_load

	// @param r0  next_task->context
	// @param r1  &(cur_task->context)
	.asm_fun _task_switch
_task_switch:
	add	sp, sp, #-reg_sz   // alloc reg_t
	str     sp, [r1]
	add     r2, sp, # reg_r4   // skip  r0 ~ r3
	stmia   r2, {r4-r11}

	mrs     r2, cpsr_all
	and     r3, lr, #1
	orr	r2, r2, r3, lsl #5
	str     lr, [sp,# reg_pc]  // pc
	str     r2, [sp,# reg_cpsr]// cpsr
_stack_check:
#if CFG_STACK_CHK
	ldr     r2, [r1, #(task_stack- task_context)]
	ldr     r3, [r2]
	ldr     r4, =0xABBA
	cmp     r3, r4
	beq     1f
	mov     r5, r0
	add     r0, r1, #-task_context
	ldr     r6, task_ov
	blx     r6
	mov     r0, r5
1:
#endif
	// @param r0   next_task->context
	.asm_fun _task_load
_task_load:
	mov     sp, r0
#if CFG_ASM_RFE
	ldmia   sp!, {r0-r12, lr}
	rfeia   sp!
#else
	ldr     r1, [sp, #reg_cpsr]
	msr     spsr, r1
	ldmia   sp!, {r0-r12, lr}
	add     sp, sp, #4
	ldmia   sp!, {pc}^
#endif

	.global task_ov
task_ov:
	.word   _stackov
