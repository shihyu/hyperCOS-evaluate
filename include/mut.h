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

#ifndef MUT150429
#define MUT150429

#include "wait.h"
#include "ll.h"
#include "task.h"

typedef struct {
	int val;		///< 0:unlock, 1+:lock
	task_t *own;		///< owner thread
	ll_t wait;		///< wait queue
	short own_pri;		///< owner thread original priority
	short dummy;
} mut_t;

mut_t *mut_init(mut_t * u);

/// \return 0 on success
int mut_lock(mut_t * u, wait_t w);

/// \note **NOT** support irq context
void mut_unlock(mut_t * u);

#endif
