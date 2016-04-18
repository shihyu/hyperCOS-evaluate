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
#ifndef NVIC1229
#define NVIC1229

#include "../io.h"

enum {
	ICTR = 0xE000E004,
	ISER = 0xE000E100,
	ICER = 0xE000E180,
	ICPR = 0XE000E280,
	IPR = 0xE000E400,
	STIR = 0xE000EF00,
};

/// \return number of irqs
static inline unsigned nvic_irqn()
{
	return (readl(ICTR) + 1) << 5;
}

static inline unsigned nvic_irq_mask(unsigned irq)
{
	reg(ICER + (irq >> 5) * 4) = (1 << (irq & 0x1f));
	return reg(ISER + (irq >> 5) * 4) & (1 << (irq & 0x1f));
}

static inline void nvic_irq_unmask(unsigned irq)
{
	reg(ISER + (irq >> 5) * 4) = (1 << (irq & 0x1f));
}

static inline void nvic_set_pri(unsigned irq, unsigned pri)
{
	void *b = (void *)(IPR + (irq));
	writeb((pri << cpu_pbits) & 0xff, b);
}

static inline void nvic_sgi(int id)
{
	reg(STIR) = id;
}

#endif
