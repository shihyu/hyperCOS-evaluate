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
#include "cpu-arm/asm.h"

	.align 5
	.section ".vectors","ax"
	ldr	pc,(_vects + 0)
	ldr	pc,(_vects + 4)
	ldr	pc,(_vects + 8)
	ldr	pc,(_vects +12)
	ldr	pc,(_vects +16)
	ldr	pc,(_vects +20)
	ldr	pc,(_vects +24)
	ldr	pc,(_vects +28)
	.global _vects
_vects:
	_reset: .word	_start
	_undef: .word	__abt
	_swi:   .word	__abt
	_pabort:.word	__pabt
	_dabort:.word	__abt
	_reserv:.word	__abt
	_irq:   .word	__irq
	_fiq:   .word	__fiq

	.text
	.global __fiq
__fiq:
	sub     lr, lr, #4
//	srsdb   sp!, #0x11
	stmdb	sp!,{lr}
	mrs     lr, spsr
	stmdb	sp!,{lr}
	stmdb	sp!, {r0-r3}

	bl      irq_ack
	mov     r1, sp

	ldr     r1, =fiq_cnt
	ldr     r0, [r1]
	add     r0, r0, #1
	str     r0, [r1]

	bl      irq_dispatch

//	ldmia   sp!, {r0-r3}
//	rfeia   sp!
	ldr     r1, [sp, #reg_fiq_cpsr]
	msr     spsr, r1
	ldmia   sp!, {r0-r3}
	add     sp, sp, #4
	ldmia   sp!, {pc}^
//-----------------------
	.global __irq
__irq:
	sub     lr, lr, #4
//	srsdb   sp!, #0x12
	stmdb   sp!, {lr}
	mrs     lr, spsr
	stmdb   sp!, {lr}
//-----------------------
	stmdb	sp!, {r0-r3,r12}

	bl      irq_ack
#if CFG_DBM
	stmdb   sp!, {r0}
	bl      dbm_irq
	mov     r1, r0
	ldmia   sp!, {r0}
	cmp     r1, r0
#endif
	mov     r1, sp

	ldr     r3, =irq_cnt
	ldr     r2, [r3]
	add     r2, r2, #1
	str     r2, [r3]

	ldr     r3, =irq_depth
	ldr     r2, [r3]
	add     r2, r2, #1
	str     r2, [r3]

#if CFG_INT_NESTED
	mrs     r3, cpsr
	cmp     r2, #CFG_INT_NESTED_MAX
	bicle   r3, r3, #0x80
	orr     r3, r3, #(0x12^0x13)// 0x12 => 0x13
	msr     cpsr, r3
	stmdb   sp!, {lr}
#endif

#if CFG_DBM
	bne     8f
	mrs     r3, cpsr
	orr     r3, r3, #(0x12^0x13)// 0x12 => 0x13
	msr     cpsr, r3
	add     sp, sp, #-8
	stmdb   sp!, {lr}
	stmdb   sp!, {r4-r12}
	add     sp, sp, #-4*4
	ldr     r3, =dbm_ctx
	str     sp, [r3]
	ldr     sp, =_dbm_sp
8:
#endif
	bl      irq_dispatch
#if CFG_DBM
	cmp     r0, #0
	beq     8f
	ldr     r3, =dbm_ctx
	ldr     r3, [r3]
	add     sp, r3, #reg_sz
	add     r3, #16
	ldmia   r3!, {r4-r12}
	ldmia   r3!, {lr}
	mrs     r3, cpsr
	bic     r3, r3, #(0x12^0x13)// 0x13 => 0x12
	msr     cpsr, r3
8:
#endif
#if CFG_INT_NESTED
	ldmia   sp!, {lr}
	mrs     r3, cpsr
	orr     r3, r3, #0x80
	bic     r3, r3, #(0x12^0x13)// 0x13 => 0x12
	msr     cpsr, r3
#endif

	ldr     r3, =irq_depth
	ldr     r2, [r3]
	add     r2, r2, #-1
	cmp     r2, #0
	str     r2, [r3]
	bne     1f

	ldr     r3, = _task_pending
	ldr     r0, [r3]
	cmp     r0, #0
	beq     1f
	mov     r2, #0
	str     r2, [r3]

	stmdb   sp!,{r0}
	bl      _task_switch_status
	mov     r1, r0
	ldmia   sp!,{r0}
	add     r0, r0, #task_context
	ldr     r0, [r0]
	mov     r2, sp
	b       _task_switch_irq
1:
//	ldmia   sp!, {r0-r3,r12}
//	rfeia   sp!
	ldr     r1, [sp, #reg_irq_cpsr]
	msr     spsr, r1
	ldmia   sp!, {r0-r3,r12}
	add     sp, sp, #4
	ldmia   sp!, {pc}^
//-------------------------------
	.global __abt
__abt:
	stmdb   sp!,{lr}
	mrs     lr, spsr
	stmdb   sp!,{r0, lr}
	mov     r0, sp
	add     sp, sp, #0x4*3

	mrs     lr, cpsr
	bic     lr, lr, #(0x17^0x13)  // 0x17 => 0x13
	msr     cpsr, lr
	add     sp, sp, #-8
	stmdb	sp!,{lr}
	stmdb	sp!,{r1-r12}
	ldmia   r0, {r0, r1, r2}
	stmdb   sp!,{r0}
	str     r1, [sp, #reg_cpsr]
	str     r2, [sp, #reg_pc]
	mov     r0, sp
	ldr     lr, core_abt
	blx     lr
	mov     r0, sp
	b       _task_load

	.global core_abt
core_abt:
	.word   _abt

	.global __pabt
__pabt:
#if CFG_DBM
	add     lr, lr, #-4
	stmdb   sp!,{lr}
	mrs     lr, spsr
	stmdb   sp!,{r0, lr}
	mov     r0, sp
	add     sp, sp, #4*3

	mrs     lr, cpsr
	orr     lr, lr, #0x80
	bic     lr, lr, #(0x17^0x13)  // 0x17 => 0x13
	msr     cpsr, lr

	add     sp, sp, #-8
	stmdb   sp!, {lr}
	stmdb   sp!, {r1-r12}
	add     sp, sp, #-4
	ldmia   r0, {r0-r2}
	str     r0, [sp, #0]
	str     r1, [sp, #reg_cpsr]
	str     r2, [sp, #reg_pc]

	ldr     r0, =dbm_ctx
	str     sp, [r0]
	ldr     sp, =_dbm_sp

	ldr     lr, =dbm_exc        ///< enter dbm
	blx     lr

	ldr     r0, =dbm_ctx
	ldr     r0, [r0]
	b       _task_load
#else
	b       __abt
#endif

#if CFG_DBM
	.bss
	.align 2
	.space CFG_DBM_STACK
	.global _dbm_sp
_dbm_sp:
#endif
