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

#ifndef HTOS
#define HTOS

#include "ll.h"
#include "cpu/reg.h"
#include "cpu/_irq.h"

extern ll_t core_gc_task;

/// mandatory initialize before any APIs could be used
void core_init(void);

/// start the OS
/// \note this function does not return
void core_start(void);

/// allocate memory block from the start heap
/// \param sz
/// \param align_bits number of bits the required memory should align to
void *core_alloc(unsigned sz, int align_bits);

#define _alloc(_sz)      core_alloc(_sz, 2)

/// number of samples on busy
extern unsigned core_nbusy;

/// number of samples on idle
extern unsigned core_nidle;

static inline void core_ut_rst()
{
	unsigned flag = irq_lock();
	core_nbusy = core_nidle = 0;
	irq_restore(flag);
}

/// start the utilization sampling
void core_ut_init();

extern void (*core_abt) (void *ctx);

#endif
