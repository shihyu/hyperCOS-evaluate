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

mut_t *mut_init(mut_t * u)
{
	memset(u, 0, sizeof(*u));
	ll_init(&u->wait);
	return u;
}

int mut_lock_wait(mut_t * u, wait_t w)
{
	unsigned iflag;

	task_t *tc = _task_cur;
	task_t *own = u->own;

	if (u->own == tc) {
		u->val++;
		return 0;
	}

	iflag = irq_lock();
	if (!u->val)
		goto done;

	if (own->pri > tc->pri) {
		own->pri = tc->pri;
		if (own->status == TASK_READY) {
			lle_del(&own->ll);
			sch_wake(own);
		}
	}

	if (task_suspend(&u->wait, w)) {
		irq_restore(iflag);
		return -1;
	} else {
done:
		_assert(u->val == 0);
		u->val++;
		u->own = tc;
		u->own_pri = tc->pri;
		irq_restore(iflag);
		return 0;
	}
}

void mut_unlock_wake(mut_t * u)
{
	task_t *tc = _task_cur;
	unsigned iflag;
	int own_pri;

	_assert(u->own == tc);
	own_pri = tc->pri;

	iflag = irq_lock();
	tc->pri = u->own_pri;
	u->val--;
	if (_task_wakeq(&u->wait, iflag) != 0 && own_pri != tc->pri)
		sch_schedule(own_pri);
}
