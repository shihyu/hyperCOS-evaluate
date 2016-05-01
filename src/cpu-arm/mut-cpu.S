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
	.asm_fun mut_lock
mut_lock:
	ldrex  r2, [r0,#mut_val]
	cmp    r2, #0
	bne    mut_lock_wait
	add    r2, r2, #1
	strex  r3, r2, [r0]
	cmp    r3, #0
	bne    mut_lock
	ldr    r2, = _task_cur
	ldr    r2, [r2]
	str    r2, [r0, #mut_own]
	ldrh   r3, [r2, #task_pri]
	strh   r3, [r0, #mut_own_pri]
	mov    r0, #0
1:
	bx     lr

	.asm_fun mut_unlock
mut_unlock:
	ldrex  r2, [r0, #mut_val]
	cmp    r2, #1
	ble    mut_unlock_wake
	add    r2, r2, #-1
	strex  r3, r2, [r0]
	cmp    r3, #0
	bne    mut_unlock
	bx     lr


