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

#include "mq.h"
#include "task.h"
#include "dbg.h"
#include "irq.h"
#include <string.h>

mq_t *mq_init(mq_t * m, int no_words, unsigned *b, int bsz)
{
	memset(m, 0, sizeof(*m));
	m->b = (void *)b;
	m->h = (void *)b;
	m->t = (void *)b;
	m->bsz = bsz;
	m->isz = sizeof(unsigned) * no_words;
	m->sz = bsz / m->isz;
	m->n = 0;
	ll_init(&m->waitp);
	ll_init(&m->waitg);
	_assert(m->isz * m->sz == bsz);
	return m;
}

int mq_put(mq_t * m, unsigned *msg, wait_t w)
{
	unsigned *d;
	unsigned iflag;

	if (m->n >= m->sz && !w)
		return -1;
	iflag = irq_lock();
	if (m->n >= m->sz) {
		if (!task_suspend(&m->waitp, w)) {
			irq_restore(iflag);
			return -1;
		}
	}

	_assert(m->n < m->sz);

	d = (void *)m->t;
	switch (m->isz >> 2) {
	case 4:
		*d++ = *msg++;
	case 3:
		*d++ = *msg++;
	case 2:
		*d++ = *msg++;
	case 1:
		*d++ = *msg++;
		break;
	default:
		memcpy(d, msg, m->isz);
	}

	m->t += m->isz;
	if (m->t >= m->b + m->bsz)
		m->t = m->b;
	m->n++;
	_task_wakeq(&m->waitg, iflag);
	return 0;
}

static int mq_fetch(mq_t * m, unsigned *msg, wait_t w)
{
	unsigned *s;
	if (!m->n && !w)
		return -1;
	if (!m->n) {
		if (task_suspend(&m->waitg, w)) {
			return -1;
		}
	}
	_assert(m->n > 0);
	s = (void *)m->h;
	switch (m->isz >> 2) {
	case 4:
		*msg++ = *s++;
	case 3:
		*msg++ = *s++;
	case 2:
		*msg++ = *s++;
	case 1:
		*msg++ = *s++;
		break;
	default:
		memcpy(msg, m->h, m->isz);
	}
	return 0;
}

int mq_peek(mq_t * m, unsigned *msg, wait_t w)
{
	int r;
	unsigned iflag;
	iflag = irq_lock();
	r = mq_fetch(m, msg, w);
	irq_restore(iflag);
	return r;
}

int mq_get(mq_t * m, unsigned *msg, wait_t w)
{
	unsigned iflag;
	iflag = irq_lock();
	if (mq_fetch(m, msg, w)) {
		irq_restore(iflag);
		return -1;
	}
	m->h += m->isz;
	if (m->h >= m->b + m->bsz)
		m->h = m->b;
	m->n--;
	_task_wakeq(&m->waitp, iflag);
	return 0;
}
