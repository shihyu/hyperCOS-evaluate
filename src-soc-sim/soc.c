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
#if ! CFG_IRQ_VECTS
#include <hcos/irq.h>
#include <hcos/soc.h>
#include <hcos/io.h>
#include <hcos/core.h>
#include <hcos/cpu/cache.h>
#include <hcos/cfg.h>
#include <hcos/cpu/hyper.h>
#include <hcos/cpu/gic.h>
#include <hcos/cpu/_cpu.h>
#include <hcos/dbm.h>
#include "uart.h"
#include "_soc.h"

uart_t u0, u1;

static int tmr_off;

void soc_idle(int next_expire)
{
	cpu_idle();
}

void soc_init(void)
{
#if CFG_CACHE_VMSA
	cache_mmu_section(BASE_DRAM,
			  BASE_DRAM, 0, 0, CACHE_PERM_RW_RW, 0x10000000);
	cache_mmu_section(BASE_MISC,
			  BASE_MISC, 0, 0, CACHE_PERM_RW_RW, 0x800000);

	cache_mmu_on();
#endif
	gic_init(BASE_DIST, BASE_CPU);
//      hyper_init(ns_ints);
	uart_init(&u0, BASE_UART0, -1);
	uart_baud(&u0, UART_CLK, UART_BAUD);

	uart_init(&u1, BASE_UART1, -1);
	uart_baud(&u1, UART_CLK, UART_BAUD);
	irq_cfg(IRQ_UART0, 1);
	irq_cfg(IRQ_UART1, 1);
}

void irq_cfg(unsigned irq, int is_level)
{
	gic_set_type(irq, is_level);
}

unsigned irq_mask(unsigned irq)
{
	return gic_irq_mask(irq);
}

void irq_unmask(unsigned irq)
{
	gic_irq_unmask(irq);
}

irq_sta_t irq_ack(void)
{
	unsigned v = gic_get_irq();
	v &= 0x1ff;
	return v == 0x1ff ? IRQ_NA : v;
}

void irq_eoi(unsigned irq)
{
	gic_eoi(irq);
}

void irq_sgi(unsigned irq)
{
	gic_sgi(irq);
}

void tmr_enable(int on)
{
	writel(on ? 0x3 : 0, (void *)BASE_TIME + 0x8);
	writel(on ? 0x3 : 0, (void *)BASE_TIME1 + 0x8);
	tmr_off = !on;
}

int tmr_init_soc(unsigned *rtcs2tick)
{
	irq_cfg(IRQ_RTC, 0);
	irq_cfg(IRQ_TIME, 0);
	irq_unmask(IRQ_RTC);
	writel(0x1, (void *)BASE_TIME + 0x0);
	writel(0x2, (void *)BASE_TIME + 0x4);
	writel(0x3, (void *)BASE_TIME + 0x8);
	*rtcs2tick = 2;
	return IRQ_TIME;
}

unsigned soc_rtcs()
{
	return readl(BASE_RTC + 0x4);
}

void tmr_tickless_soc(unsigned next_expire)
{
	if (!tmr_off)
		writel(next_expire << 2, (void *)BASE_RTC + 0x0);
}

unsigned core_ut_init_soc()
{
	writel(0x1, (void *)BASE_TIME1 + 0x0);
	writel(0x0, (void *)BASE_TIME1 + 0x4);
	writel(0x3, (void *)BASE_TIME1 + 0x8);
	return IRQ_TIME1;
}

int _dbm_get()
{
	return uart_get(&u1);
}

void dbm_put(char ch)
{
	uart_put(&u1, ch);
}

unsigned dbm_irq(void)
{
	return IRQ_UART1;
}

void udelay(int us)
{
	volatile int i;
	int n = us * 120;
	for (i = 0; i < n; i++) ;
}
#endif
