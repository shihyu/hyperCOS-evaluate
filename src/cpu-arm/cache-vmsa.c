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

#include "io.h"
#include "core.h"
#include "cpu-arm/cache-vmsa.h"
#include "cpu-arm/asm.h"
#include <string.h>

unsigned *cache_ttb;

enum {
	MMU_L1_FAULT_ID = 0x0,
	MMU_L1_PAGE_ID = 0x1,
	MMU_L1_SEC_ID = 0x2,
};

struct MMU_L1_FAULT {
	int id:2;
	int sbz:30;
};

struct MMU_L1_PAGE {
	int id:2;
	int imp:2;
	int dom:4;
	int sbz:1;
	int base:23;
};

struct MMU_L1_SEC {
	int id:2;
	int b:1;
	int c:1;
	int imp:1;
	int dom:4;
	int sbz0:1;
	int ap:2;
	int sbz1:7;
	int ns:1;
	int base:12;
};

struct MMU_L1_RES {
	int id:2;
	int sbz:30;
};

union MMU_L1_DES {
	unsigned long word;
	struct MMU_L1_FAULT fault;
	struct MMU_L1_PAGE page;
	struct MMU_L1_SEC sec;
	struct MMU_L1_RES res;
};

void cache_init(void)
{
	unsigned t;

#define NO_ACCESS(dom_num)	(0x0 << (dom_num)*2)
#define CLIENT(dom_num)	(0x1 << (dom_num)*2)
#define MANAGER(dom_num)	(0x3 << (dom_num)*2)

	// Set the Domain Access Control Register
	t = MANAGER(0) |
	    NO_ACCESS(1) |
	    NO_ACCESS(2) |
	    NO_ACCESS(3) |
	    NO_ACCESS(4) |
	    NO_ACCESS(5) |
	    NO_ACCESS(6) |
	    NO_ACCESS(7) |
	    NO_ACCESS(8) |
	    NO_ACCESS(9) |
	    NO_ACCESS(10) |
	    NO_ACCESS(11) |
	    NO_ACCESS(12) | NO_ACCESS(13) | NO_ACCESS(14) | NO_ACCESS(15);
	asm volatile (""
		      asm_beg(r2) "mcr	p15,0,%0,c3,c0,0" "\n\t" asm_end(r2)
		      ::"r"(t)
		      :"r2");
	cache_ttb = core_alloc(0x4000, 14);
	memset((void *)cache_ttb, 0, 0x4000);
}

static void mmu_section(unsigned ppn, unsigned vpn, unsigned c, unsigned b,
			unsigned perm)
{
	register union MMU_L1_DES desc;
	desc.word = MMU_L1_SEC_ID & 0x3;
	desc.sec.imp = 1;
	desc.sec.dom = 0;
	desc.sec.c = (c);
	desc.sec.b = (b);
	desc.sec.ap = (perm);
	desc.sec.ns = 0;
	desc.sec.base = (ppn);
	writel(desc.word, (void *)(cache_ttb) + ((vpn) << 2));
}

void cache_mmu_section(unsigned pa, unsigned va,
		       unsigned c, unsigned b, cache_perm_t perm, unsigned sz)
{
	unsigned ppn = pa >> 20;
	unsigned vpn = va >> 20;
	unsigned np = (((sz) >> 20) + ((sz) & 0xfffff ? 1 : 0));
	int i;

	for (i = np; i > 0; i--) {
		mmu_section(ppn, vpn, c, b, perm);
		ppn++;
		vpn++;
	}
}

unsigned cache_mmu_on(void)
{
	register unsigned ov, tmp;

	// load ttb
	asm volatile ("" asm_beg(r2) "mcr  p15,0,%0,c2,c0,0 \n" asm_end(r2)
		      ::"r"(cache_ttb)
		      :"r2");

	// enable mmu
	asm volatile (""
		      asm_beg(r2)
		      "mrc  p15,0,%0,c1,c0,0 \n"
		      "orr  %1,%0,#1         \n"
		      "mcr  p15,0,%1,c1,c0,0 \n" asm_end(r2)
		      :"=r"(ov), "=r"(tmp)
		      ::"r2");
	return ov;
}

void cache_i_inv(unsigned sta, unsigned sz)
{
	asm volatile (""
		      asm_beg(r2)
		      "mov    r1,#0\n"
		      "mcr    p15,0,r1,c7,c5,0\n"
		      "mcr    p15,0,r1,c8,c5,0\n"
		      "nop\n"
		      "nop\n" "nop\n" "nop\n" "nop\n" "nop\n" asm_end(r2)
		      :::"r1", "r2");
}

void cache_d_flu(unsigned sta, unsigned sz)
{
/*
    asm volatile (""
	asm_beg(r2)
        "1:\n"
	"mrc	p15, 0, r15, c7, c14, 3\n"
	"bne    1b\n"
        "mcr    p15,0,r0,c7,c10,4\n"
	asm_end(r2)
	:
	:
	: "r0", "r2");
*/

}
