/*-****************************************************************************/
/*-                                                                           */
/*-            Copyright (c) of hyperCOS.                                     */
/*-                                                                           */
/*-  This software is copyrighted by and is the sole property of its authors. */
/*-  All rights, title, ownership, or other interests in the software remain  */
/*-  the property of the authors. The source code is for FREE short-term      */
/*-  evaluation, educational or non-commercial research only. Any commercial  */
/*-  application may only be used in accordance with the corresponding license*/
/*-  agreement. Any unauthorized use, duplication, transmission, distribution,*/
/*-  or disclosure of this software is expressly forbidden.                   */
/*-                                                                           */
/*-  Knowledge of the source code may NOT be used to develop a similar product*/
/*-                                                                           */
/*-  This Copyright notice may not be removed or modified without prior       */
/*-  written consent of the authors.                                          */
/*-                                                                           */
/*-  The authors reserves the right to modify this software                   */
/*-  without notice.                                                          */
/*-                                                                           */
/*-  To email the authors:                                                    */
/*-                                                                           */
/*-             socware.help@gmail.com                                        */
/*-                                                                           */
/*-****************************************************************************/

#ifndef TIMER150423
#define TIMER150423

#include "ll.h"
#include "irq.h"

typedef int (*tmr_do) (void *p);

typedef struct {
	lle_t ll;
	tmr_do f;
	void *p;
	unsigned expire;
} tmr_t;

/// total ticks since boot up
extern unsigned tmr_ticks;

/// timer irq number
extern unsigned tmr_irq;

/// log2(rtcs_freq/tick_freq)
extern unsigned tmr_rtcs2tick;

tmr_t *tmr_init(tmr_t * t, void *p, tmr_do f);

void tmr_on(tmr_t * t, unsigned expire);

static inline void _tmr_of(tmr_t * t)
{
	lle_del(&t->ll);
}

static inline void tmr_of(tmr_t * t)
{
	unsigned iflag = irq_lock();
	_tmr_of(t);
	irq_restore(iflag);
}

irq_handler_decl(_tmr_tickf);

#endif
