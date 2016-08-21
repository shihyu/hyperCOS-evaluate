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
#include <string.h>
#include <stdio.h>

#if ARM_HYPER
#include <hcos/cpu/hyper.h>
unsigned ns_ints[] = {
	0xffffffff
};
#endif

#if _EXE_

static void hellof(void *priv)
{
	unsigned ts = (unsigned)priv;
	while (1) {
		printf("%s %d\n", _task_cur->name, tmr_ticks);
		task_sleep(ts);
	}
}

int main(void)
{
	core_init();
	task_new("hello-1", hellof, 56, 1024, -1, (void *)(tmr_hz / 2));
	task_new("hello-2", hellof, 10, 1024, -1, (void *)(tmr_hz));
	core_start();
	return 0;
}
#endif
