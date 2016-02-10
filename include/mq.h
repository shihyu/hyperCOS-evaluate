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

#ifndef MQ150313
#define MQ150313

#include "wait.h"
#include "ll.h"

typedef struct {
	char *b, *h, *t;
	short bsz;		///< size of the whole buffer
	short isz;		///< bytes in a msg
	short sz;		///< max msgs
	short n;		///< current enqueued msgs
	ll_t waitp;		///< wait queue for put
	ll_t waitg;		///< wait queue for get
} mq_t;

/// \param no_words number of words within a single message
mq_t *mq_init(mq_t * m, int no_words, unsigned *buf, int buf_sz);

/// \return 0 on success
int mq_get(mq_t * m, unsigned *msg, wait_t w);

/// \note support irq context
/// \return 0 on success
int mq_put(mq_t * m, unsigned *msg, wait_t w);

#endif
