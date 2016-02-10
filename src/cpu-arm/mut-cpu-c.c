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

#include "mut.h"
#include "task.h"
#include "irq.h"
#include <string.h>

int mut_lock_wait(mut_t * u, wait_t w);

/// \return 0 on success
int mut_lock(mut_t * u, wait_t w)
{
	unsigned iflag = irq_lock();
	if (!u->val) {
		u->val++;
		u->own = _task_cur;
		u->own_pri = _task_cur->pri;
		irq_restore(iflag);
		return 0;
	}
	irq_restore(iflag);
	return mut_lock_wait(u, w);
}

void mut_unlock_wake(mut_t * u);

void mut_unlock(mut_t * u)
{
	unsigned iflag = irq_lock();
	if (u->val == 1) {
		irq_restore(iflag);
		return mut_unlock_wake(u);
	}
	u->val--;
	irq_restore(iflag);
}
