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

#ifndef GIC150303
#define GIC150303

#include "../io.h"

#define DIST_ENABLE_SET		0x100
#define DIST_ENABLE_CLEAR	0x180

extern unsigned gic_dist_base, gic_cpu_base;

void gic_init(unsigned dist_base, unsigned cpu_base);

int gic_set_type(unsigned irq, int is_level);

/// set an IRQ for none-secure world to use
void gic_set_ns(int irq, int ns);

static inline unsigned gic_irq_mask(unsigned irq)
{
	unsigned en, reg, msk;

	msk = 1 << (irq & 0x1f);
	reg = gic_dist_base + DIST_ENABLE_CLEAR + ((irq >> 5) << 2);
	en = readl(reg) & msk;
	writel(msk, (void *)reg);
	return en ? 1 : 0;
}

static inline void gic_irq_unmask(unsigned irq)
{
	unsigned mask = 1 << (irq & 0x1f);
	writel(mask,
	       (void *)(gic_dist_base + DIST_ENABLE_SET + ((irq >> 5) << 2)));
}

static inline unsigned gic_get_irq()
{
	unsigned b, v;
	b = (gic_cpu_base + 0xc);
	v = readl(b) & 0x1ff;
	return v == 0x1ff ? 0xffffffff : v;
}

static inline void gic_eoi(unsigned irq)
{
	unsigned b = (gic_cpu_base + 0x10);
	writel(irq, (void *)b);
}

static inline void gic_sgi(int id)
{
	unsigned v = id | (0x2 << 24);
	writel(v, (void *)gic_dist_base + 0xF00);
}

#endif
