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
#include <hcos/core.h>
#include <hcos/task.h>
#include <hcos/soc.h>
#include <hcos/mod.h>
#include <hcos/dbm.h>
#include <string.h>
#include <stdio.h>
#include "_soc.h"

void count(void *priv)
{
	while (1) {
		asm volatile (".global _count\n"
			      "_count:\n"
			      "ldr r0, =0\n"
			      "add r0, #1\n"
			      "add r0, #1\n"
			      "ldr r1, =0\n"
			      "mov r1, #1\n" "mov r1, #1\n":::"r0", "r1");
	}
}

void gc(task_t * t)
{
}

int main(void)
{
	core_init();
	core_ut_init();
	dbm_init(IRQ_UART1);
	tmr_enable(0);
	task_new("count", count, 9, 1024, -1, 0);
	core_start();
	return 0;
}
