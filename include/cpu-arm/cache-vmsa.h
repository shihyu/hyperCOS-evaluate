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

#ifndef VMSA150426
#define VMSA150426

#include "asm.h"

typedef enum {
	CACHE_PERM_RO_RO = 0,
	CACHE_PERM_RW_NONE = 1,
	CACHE_PERM_RW_RO = 2,
	CACHE_PERM_RW_RW = 3,
} cache_perm_t;

void cache_init(void);

/// \param pa   physical address
/// \param va   virtual address
/// \param c    cache-able
/// \param b    buffer-able
/// \param perm permission
/// \param sz   section size
void cache_mmu_section(unsigned pa, unsigned va,
		       unsigned c, unsigned b, cache_perm_t perm, unsigned sz);

unsigned cache_mmu_on(void);

static inline unsigned cache_mmu_off(void)
{
	register unsigned ov, tmp;
	asm volatile (""
		      asm_beg(r2)
		      "mrc  p15,0,%0,c1,c0,0 \n"
		      "bic  %1,%0,#1         \n"
		      "mcr  p15,0,%1,c1,c0,0 \n" asm_end(r2)
		      :"=r"(ov), "=r"(tmp)
		      ::"r2");
	return ov;
}

#endif
