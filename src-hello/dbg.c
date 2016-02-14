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
#include <hcos/dbg.h>
#include <hcos/cpu.h>
#include <hcos/task.h>
#include <hcos/cpu/reg.h>
#include <stdio.h>

void _exit(int status)
{
	unsigned *p = (unsigned *)0xB0100004;
	*p = status;
	while (1) ;
}

void _fail(void *addr, const char *f, unsigned line)
{
	printf("_fail 0x%08x %s:%d\n", (unsigned)addr, f, line);
	_exit(-1);
}

void _abt(void *_ctx)
{
	reg_t *ctx = (reg_t *) _ctx;
	int i, n = sizeof(reg_t) / sizeof(unsigned);
	unsigned *p = (unsigned *)ctx;
	for (i = 0; i < n; i++)
		printf("%6s: 0x%08x\n", cpu_reg_names[i], *p++);
	// only skip the abort instruction
	reg_skip(ctx, -8);
}

void _stackov(task_t * t)
{
	printf("stack ov\n");
}
