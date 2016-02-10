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

#ifndef CPU150822
#define CPU150822

#include "cfg.h"

static inline void cpu_idle()
{
#if CFG_ASM_WFI
	asm volatile ("" asm_beg(r0) "wfi\n" asm_end(r0)
		      :::"r0");
#else
	asm volatile ("" asm_beg(r0) "mcr p15, 0, r0, c7, c0, 4\n" asm_end(r0)
		      :::"r0");
#if CFG_TICKLESS
	asm volatile (""
		      asm_beg(r0)
		      "1:\n"
		      "ldr r0,=tmr_rtcs\n"
		      "ldr r0, [r0]\n" "cmp r0, #0\n" "bne 1b\n" asm_end(r0)
		      :::"r0");
#endif
#endif

}

void cpu_init(void);

enum {
	_IRQ = 0x12,
	_SVC = 0x13,
	_ABT = 0x17,
	_MON = 0x16
};

/// \param mode cpu mode
/// \param stack
void cpu_set_stack(unsigned mode, void *stack);

#define cpu_req_switch()

#endif
