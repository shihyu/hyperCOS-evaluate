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

#ifndef IRQ_CPU150313
#define IRQ_CPU150313

#include "asm.h"
#include "../cfg.h"

#ifdef __ASSEMBLY__

	.macro  .irq_lock  iflag, tmp
	mrs    \iflag, primask
	cpsid i
	.endm

	.macro  .irq_restore  iflag
	msr primask, \iflag
	.endm
#else

#include "_cpu.h"

#define irq_dep_chk(_exp)

inline static int irq_act()
{
	int ipsr;
	asm volatile("mrs   %0, ipsr":"=r"(ipsr)::);
	return ipsr;
}

inline static void irq_restore(unsigned flag)
{
	asm volatile("msr   primask, %0"::"r"(flag):);
}

/// lock IRQ, keep FIQ
/// \return saved flag
inline static unsigned irq_lock()
{
	unsigned flag;
	asm volatile("mrs   %0, primask":"=r"(flag)::);
	asm volatile("cpsid i":::"memory");
	return flag;
}

inline static unsigned irq_unlock()
{
	unsigned flag;
	asm volatile("mrs   %0, primask":"=r"(flag)::);
	asm volatile("cpsie i" : : : "memory");
	return flag;
}

#define irq_eoi(_n)

typedef enum {
	IRQ_NA = 0x1ff,
	IRQ_DONE = 0x2ff
} irq_sta_t;

/// \return IRQ_DONE if irq_eoi has already been called
typedef irq_sta_t (*irq_t) (void);

#define irq_handler_decl(_name)	\
	irq_sta_t _name (void)

#define irq_handler	irq_handler_decl

#endif	//__ASSEMBLY__
#endif
