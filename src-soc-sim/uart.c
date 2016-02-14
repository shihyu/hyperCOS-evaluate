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
#include "uart.h"

void uart_init(uart_t * o, unsigned base, unsigned irq)
{
	o->base = base;
	o->irq = irq;
}

void uart_baud(uart_t * o, unsigned clk, unsigned baud)
{
	unsigned div = (clk + 4 * baud) / (8 * baud);
	unsigned base = o->base;
	writeb(0xBF, (void *)UART_LCR(base));
	writeb(0x00, (void *)UART_FCR(base));
	writeb(0x03, (void *)UART_LCR(base));
	writeb(0x83, (void *)UART_LCR(base));
	writeb(div, (void *)UART_DLL(base));
	writeb(0x00, (void *)UART_DLH(base));
	writeb(0x03, (void *)UART_LCR(base));
	writeb(0x07, (void *)UART_FCR(base));	// FIFO
	writeb(readb(UART_IER(base)) | 0x01, (void *)UART_IER(base));
}

void uart_put(uart_t * o, char c)
{
	unsigned base = o->base;
	while (1) {
		unsigned short lsr = readb(UART_LSR(base));
		if (lsr & UART_LSR_THRE) {
			writeb(c, (void *)UART_THR(base));
			break;
		}
	}
}

int uart_get(uart_t * o)
{
	unsigned base = o->base;
	unsigned char lsr;

	lsr = readb(base + 0x14);
	if ((lsr & 0x1) == 0)
		return -1;
	return readb(base + 0x00);
}
