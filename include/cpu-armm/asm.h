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

///
/// \file   asm.h
/// \brief  assembly macro supports arm mode in-line assembly
///
#ifndef ASM150815
#define ASM150815

#include "../cfg.h"

#define asm_beg(_tmp)	""

#define asm_end(_tmp)	""

#define asm_mb()	asm volatile ("dsb\n isb\n" ::: "memory")

#ifdef __ASSEMBLY__
	.macro	.asm_syn
		.syntax unified
		.arch armv7-m
		.fpu vfp
		.thumb
	.endm

	.macro  .asm_fun name
		.global \name
		.thumb
		.thumb_func
		.type \name, %function
	.endm
#endif

#endif
