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
#ifndef UART0721
#define UART0721

#include <hcos/io.h>

#define UART_THR(_b)           ((_b)+0x0)
#define UART_DLL(_b)           ((_b)+0x0)
#define UART_DLH(_b)           ((_b)+0x4)
#define UART_IER(_b)           ((_b)+0x4)
#define UART_FCR(_b)           ((_b)+0x8)
#define UART_LCR(_b)           ((_b)+0xc)
#define UART_LSR(_b)           ((_b)+0x14)

#define UART_LSR_THRE     	    0x0020

typedef struct {
	unsigned base, irq;
} uart_t;

void uart_init(uart_t * o, unsigned base, unsigned irq);

void uart_baud(uart_t * o, unsigned clk, unsigned baud);

void uart_put(uart_t * o, char c);

int uart_get(uart_t * o);

static inline int uart_w(uart_t * o, const char *buf, int n)
{
	while (n--) {
		char c = *buf++;
		uart_put(o, c);
	}
	return 0;
}

#endif
