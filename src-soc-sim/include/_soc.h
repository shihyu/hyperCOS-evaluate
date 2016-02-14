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
#ifndef SOC130408
#define SOC130408

#include "uart.h"

enum {
	BASE_DRAM = 0x00000000,
	BASE_MISC = 0xB0100000,
	BASE_UART0 = 0xB0200000,
	BASE_UART1 = 0xB0300000,
	BASE_CPU = 0xB0400000,
	BASE_DIST = 0xB0401000,
	BASE_TIME = 0xB0500000,
	BASE_TIME1 = 0xB0600000,
	BASE_ETH = 0xB0700000,
	BASE_RTC = 0xB0800000,
};

enum {
	IRQ_TIME = 32,
	IRQ_TIME1 = 33,
	IRQ_RTC = 35,
	IRQ_ETH = 37,
	IRQ_UART0 = 39,
	IRQ_UART1 = 40,
};

enum {
	UART_BAUD = 115200,
	UART_CLK = 1000000,
};

static inline unsigned tmr_ticks2ms(unsigned ticks)
{
	return ticks * 10;
}

static inline unsigned tmr_ms2ticks(unsigned ms)
{
	return ms / 10;
}

extern uart_t u0, u1;

#endif
