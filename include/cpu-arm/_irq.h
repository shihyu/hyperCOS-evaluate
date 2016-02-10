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

#ifdef __ASSEMBLY__

	.macro  .irq_lock  iflag, tmp
	mrs    \iflag, cpsr
	orr    \tmp, \iflag, #0x80
	msr    cpsr, \tmp
	.endm

	.macro  .irq_restore  iflag
	msr    cpsr, \iflag
	.endm
#else
#include "../dbg.h"

#define irq_dep_chk(_exp)	_assert(_exp)

#define irq_act()		irq_depth

inline static void irq_restore(unsigned flag)
{
	asm volatile(""
		asm_beg(r2)
		"msr     cpsr, %0 \n"
		asm_end(r2)
		:
		:"r"(flag)
		:"r2", "memory");
}

#define _cpsr(_op, _msk)	\
{				\
	unsigned ori, tmp;	\
	asm volatile (""	\
		asm_beg(r2)	\
		"mrs     %0, cpsr\n"		\
		#_op "   %1, %0,"#_msk"\n"	\
		"msr     cpsr, %1\n"		\
		asm_end(r2)			\
		:"=r"(ori),"=r"(tmp)		\
		:				\
		:"memory","r2");		\
	return ori;\
}

/// lock IRQ, keep FIQ
/// \return saved flag
inline static unsigned irq_lock()		_cpsr(orr, #0x80)

inline static unsigned irq_unlock()		_cpsr(bic, #0x80)

/// lock IRQ & FIQ
/// \return saved flag
inline static unsigned irq_lock_all()	_cpsr(orr, #0xc0)

inline static unsigned irq_unlock_all()	_cpsr(bic, #0xc0)

typedef enum {
	IRQ_NA = 0x1ff,
	IRQ_DONE = 0x2ff
} irq_sta_t;

/// \return IRQ_DONE if irq_eoi has already been called
typedef irq_sta_t (*irq_t) (unsigned irq, void *ctx, int depth);

#define irq_handler(_name)	\
	irq_sta_t _name (unsigned irq, void *reg_irq, int depth)

#define irq_handler_decl(_name)	\
	irq_handler(_name)

#endif
#endif

