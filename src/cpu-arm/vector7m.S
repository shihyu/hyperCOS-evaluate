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

#include "cfg.h"
#include "asm-off.h"
#include "cpu/asm.h"
	.asm_syn

#if CFG_FIX_VECT
	.align 7
	.section ".vectors","ax"
	.global __isr_vector
__isr_vector:
	.space ((16+CFG_FIX_VECT)*4)
#endif
	.bss
	.align 4
	.global _stack_end
_stack_end:
	.space CFG_STACK_IRQ
	.global _stack
_stack:

	.text
	.asm_fun _svc
_svc:
	cmp	r3, 0x1
	beq	__task_switch
	cmp	r3, 0x2
	beq	__task_load
	// fall through to __abt

	.asm_fun __abt
__abt:
	tst 	lr, #4
	ite	eq
	mrseq	r0, MSP
	mrsne	r0, PSP
	push	{lr}
	bl	cpu_tf
	pop	{lr}
	cmp	r0, #0
	it	ne
	// return iff it's a lazy float trap
	bxne	lr
	bl	_abt
1:
	b	1b
	bx	lr
