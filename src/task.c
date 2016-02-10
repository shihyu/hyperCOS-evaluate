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

#include "task.h"
#include "irq.h"
#include "wait.h"
#include "sch.h"
#include "dbg.h"
#include "cpu/_cpu.h"
#include <string.h>

reg_t *_cpu_task_init(struct task *t, void *priv, void *dest);

task_t *_task_cur, *_task_pending;

void (*task_gc) (task_t * t);

static int task_wait_timeout(void *p)
{
	task_t *t = (task_t *) p;
	unsigned iflag = irq_lock();
	t->tm_out |= 1;
	if (t->status != TASK_READY) {
		lle_del(&t->ll);
		sch_wake(t);
	}
	irq_restore(iflag);
	return 0;
}

static void task_dest()
{
	unsigned iflag;
	task_t *tc = _task_cur;
	tc->status = TASK_DEST;
	iflag = irq_lock();
	ll_addt(&core_gc_task, &tc->ll);
	irq_restore(iflag);
	task_yield();
}

task_t *task_init(task_t * t,
		  const char *name,
		  task_ent e,
		  int pri, unsigned *stack, int stack_sz, int slice, void *priv)
{
	unsigned iflag;
	reg_t *r;
	memset(t, 0, sizeof(*t));
	t->name = name;
	t->ent = e;
	t->pri = pri;
	t->stack = stack;
	t->stack_sz = stack_sz;
	t->slice = slice;
	t->slice_cur = slice;
	tmr_init(&t->to, t, task_wait_timeout);

	_assert(pri != 0);
	_assert(pri < CFG_TPRI_NUM);

	r = _cpu_task_init(t, priv, task_dest);
	t->context = r;
	lle_init(&t->ll);
	iflag = irq_lock();
	t->status = TASK_READY;
	sch_add(t);
#ifdef CFG_STACK_CHK
	*stack = 0xABBA;
#endif
	irq_restore(iflag);

	if (_task_cur && _task_cur->pri > t->pri)
		task_yield();
	return t;
}

/// requirecritical section protection
int task_suspend(ll_t * wq, wait_t w)
{
	int f;
	lle_t *p;
	task_t *tc = _task_cur;

	irq_dep_chk(irq_depth == 0);
	if (w == WAIT_NO)
		return -1;

	ll_for_each(wq, p) {
		task_t *t = lle_get(p, task_t, ll);
		if (t->pri > tc->pri)
			break;
	}
	lle_add_before(p, &tc->ll);

	tc->status = TASK_WAIT;
	if (w != WAIT) {
		tmr_on(&tc->to, (unsigned)w);
	}
	tc->tm_out &= ~1;
	task_yield();
	_tmr_of(&tc->to);
	f = tc->tm_out & 1;
	return f;
}

void task_sleep(wait_t w)
{
	task_t *tc = _task_cur;
	unsigned iflag;
	iflag = irq_lock();
	irq_dep_chk(irq_depth == 0);
	if (w) {
		tc->status = TASK_WAIT;
		tmr_on(&tc->to, (unsigned)w);
	}
	task_yield();
	irq_restore(iflag);
}

reg_t **_task_switch_status(task_t * _tnext)
{
	task_t *_task_ori = _task_cur;
	if (_task_cur->status == TASK_READY)
		sch_add(_task_cur);
	sch_del(_tnext);
	_task_cur = _tnext;
	_task_cur->slice_cur = _task_cur->slice;
	return &_task_ori->context;
}
