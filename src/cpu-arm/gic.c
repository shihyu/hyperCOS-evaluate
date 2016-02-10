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

#include "cfg.h"

#include "gic-cfg.h"
#include "cpu-arm/gic.h"

enum {
	CPU_CTRL = 0x00,
	CPU_PRIMASK = 0x04,
	CPU_BINPOINT = 0x08,
	CPU_INTACK = 0x0c,
	CPU_EOI = 0x10,
	CPU_RUNNINGPRI = 0x14,
	CPU_HIGHPRI = 0x18,
};

enum {
	DIST_CTRL = 0x000,
	DIST_CTR = 0x004,
	DIST_PENDING_SET = 0x200,
	DIST_PENDING_CLEAR = 0x280,
	DIST_ACTIVE_BIT = 0x300,
	DIST_PRI = 0x400,
	DIST_TARGET = 0x800,
	DIST_CONFIG = 0xc00,
	DIST_SOFTINT = 0xf00,
};

unsigned gic_dist_base, gic_cpu_base;

int gic_set_type(unsigned irq, int is_level)
{
	unsigned gicirq = irq;
	unsigned enablemask = 1 << (gicirq & 0x1f);
	unsigned enableoff = (gicirq / 32) << 2;
	unsigned confmask = 0x2 << ((gicirq & 0xf) << 1);
	unsigned confoff = (gicirq / 16) << 2;
	int enabled = 0;
	unsigned val;

	val = readl(gic_dist_base + DIST_CONFIG + confoff);
	if (is_level)
		val &= ~confmask;
	else
		val |= confmask;
	if (readl(gic_dist_base + DIST_ENABLE_SET + enableoff) & enablemask) {
		writel(enablemask,
		       (void *)gic_dist_base + DIST_ENABLE_CLEAR + enableoff);
		enabled = 1;
	}
	writel(val, (void *)(gic_dist_base + DIST_CONFIG + confoff));
	if (enabled)
		writel(enablemask,
		       (void *)(gic_dist_base + DIST_ENABLE_SET + enableoff));
	return 0;
}

void gic_init(unsigned dist_base, unsigned cpu_base)
{
	unsigned gic_irqs, i, msk;

	gic_dist_base = dist_base;
	gic_cpu_base = cpu_base;

	writel(0, (void *)gic_dist_base + DIST_CTRL);

	//      get number of interrupts
	gic_irqs = readl(gic_dist_base + DIST_CTR) & 0x1f;
	gic_irqs = (gic_irqs + 1) << 5;
	if (gic_irqs > 1020)
		gic_irqs = 1020;

	//      level triggered, active low.
	for (i = 32; i < gic_irqs; i += 16)
		writel(0,
		       (void *)gic_dist_base + DIST_CONFIG + ((i << 2) >> 4));

	msk = 1;
	msk |= msk << 8;
	msk |= msk << 16;

	// Set all global interrupts to this CPU
	for (i = 32; i < gic_irqs; i += 4)
		writel(msk,
		       (void *)gic_dist_base + DIST_TARGET + ((i << 2) >> 2));

	//      priority for global interrupts.
	for (i = 32; i < gic_irqs; i += 4)
		writel(0xa0a0a0a0,
		       (void *)gic_dist_base + DIST_PRI + ((i << 2) >> 2));

	//      Disable all.  Leave the PPI and SGIs alone
	//      as these enables are banked registers.
	for (i = 32; i < gic_irqs; i += 32)
		writel(0xffffffff,
		       (void *)gic_dist_base + DIST_ENABLE_CLEAR +
		       ((i << 2) >> 5));

	writel(1, (void *)(gic_dist_base + DIST_CTRL));

	//      Deal with the banked PPI and SGI interrupts - disable all
	//      PPI interrupts, ensure all SGI interrupts are enabled.
	writel(0xffff0000, (void *)gic_dist_base + DIST_ENABLE_CLEAR);
	writel(0x0000ffff, (void *)gic_dist_base + DIST_ENABLE_SET);

	//      Set priority on PPI and SGI interrupts
	for (i = 0; i < 32; i += 4)
		writel(0x0f0f0f0f,
		       (void *)gic_dist_base + DIST_PRI + ((i << 2) >> 2));
	//      Set priority on external interrupts
	for (i = 32; i < 96; i += 4)
		writel(0x0f0f0f0f,
		       (void *)gic_dist_base + DIST_PRI + ((i << 2) >> 2));
	writel(GICP_INIT, (void *)gic_cpu_base + CPU_PRIMASK);
	// enable FIQ for security irq
	writel(0xb, (void *)gic_cpu_base + CPU_CTRL);

	writel(0, (void *)gic_dist_base + DIST_CTRL + 0x80);
	writel(0, (void *)gic_dist_base + DIST_CTRL + 0x80);
	writel(0, (void *)gic_dist_base + DIST_CTRL + 0x80);
	writel(0, (void *)gic_dist_base + DIST_CTRL + 0x80);
}

void gic_set_ns(int irq, int ns)
{
	unsigned o, v, n;
	o = gic_dist_base + DIST_CTRL + 0x80 + ((irq >> 5) << 2);
	v = readl(o);
	if (ns)
		v |= (1 << (irq & 0x1f));
	else
		v &= ~(1 << (irq & 0x1f));
	writel(v, (void *)o);

	o = gic_dist_base + DIST_PRI + ((irq >> 2) & ~0x3);
	v = readl(o);
	n = ((irq & 0x3) << 3);
	v &= ~(0xff << n);
	if (ns)
		v |= 0x80 << n;
	else
		v |= 0x0f << n;
	writel(v, (void *)o);
}
