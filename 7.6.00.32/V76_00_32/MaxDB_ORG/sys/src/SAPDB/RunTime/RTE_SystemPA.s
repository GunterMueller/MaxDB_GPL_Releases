;****************************************************************************
;
; module      : RTE_SystemPA.s
;
; -------------------------------------------------------------------------
;
; responsible : Joerg Mensing - D027923
;
; special area: RTE System Interface
; description : Needed for Precision architecture specialties.
;               aCC (C++ compiler on HPUX) does not support inline assembler
;
; first created:2000-05-25  19:15
; last changed: 2000-05-25  19:46
;
; -------------------------------------------------------------------------
;
; copyright:    Copyright (c) 2000-2005 SAP AG
;
;*****************************************************************************/
;
;**************************************************************************
; Test and set routines implemented with the LDCWS instruction
;
; The LDCWS instruction is very similar to a traditional test+set instruction
; with the following exceptions:
;   o Zero means set and One means clear.
;   o Word must be 16 byte aligned
;
; On future multiprocessor  machines,  normal load and store instructions could
; be reordered arbitrarily by the hardware.  The stws,ma 0() and ldws,ma 0()
; instructions will force a synchronization of reordered loads and stores.
;************************************************************************ 
;
; RTE_PATestAndSet(addr)
;*****************************************************
; addr must be 16Byte aligned!!!!
; RTE_PATestAndSet returns true if lock was already set
;****************************************************
;
;
;    ========== licence begin  GPL
;    Copyright (c) 2000-2005 SAP AG
;
;    This program is free software; you can redistribute it and/or
;    modify it under the terms of the GNU General Public License
;    as published by the Free Software Foundation; either version 2
;    of the License, or (at your option) any later version.
;
;    This program is distributed in the hope that it will be useful,
;    but WITHOUT ANY WARRANTY; without even the implied warranty of
;    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;    GNU General Public License for more details.
;
;    You should have received a copy of the GNU General Public License
;    along with this program; if not, write to the Free Software
;    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
;    ========== licence end

;

&if $MACH = PA20W
;-------------------------------------------------------------------------------------
; Implementation of own context setup interface
;
;   struct RTE_OwnCoroutineContext
;   {
;       void *sp ;          Stack pointer
;       void (*addr)() ;	Return pointer
;   };
;
;   void RTE_OwnMakeContext(struct RTE_OwnCoroutineContext *pContext)
;
;   pContext must point to an already filled context
;     pContext->sp   set to first byte of stack array,
;     pContext->addr point to initial function
;
    .LEVEL 2.0W
    .SPACE $TEXT$,SORT=8
    .SUBSPA $CODE$,QUAD=0,ALIGN=8,ACCESS=0x2c,CODE_ONLY,SORT=24
;
RTE_OwnMakeContext
$PIC$0
	.PROC
	.CALLINFO CALLER,FRAME=320,SAVE_RP
    .ENTRY
; store frame marker, setup new sp
    STD     %r2,-16(%r30)   ;save rp
    STD,MA  %r3,400(%r30)   ;move sp to new frame
    STD     %r27,-384(%r30) ;save gp in stack frame

;fill in func address of task local variable area
    LDD     8(%r26),%r1     ;r1 = arg0[1]
    LDD     16(%r1),%r3     ;r3 = addr((void(*)())arg0[1])
    STD     %r3,-368(%r30)  ;reg_set.rp = addr((void(*)())arg0[1])

;fill in sp of task in local variable area, leave space for frame marker 
    LDD     0(%r26),%r1     ;r1 = arg0[0]
	ADDI	16, %r1, %r1	;r1 = arg0[0] + sizeof (frame_marker)
    STD     %r1,-360(%r30)  ;reg_set.sp = arg0[0] + sizeof (frame_marker)

;copy frame marker to task stack
	STD		%r30, -8(%r1)  ; save real sp in task stacks frame marker
	STD		%r2, -16(%r1)  ; save real rp in task stacks frame marker

;write back new task context
	STD		%r1, 0(%r26)	; task stacks sp
    STD     %r3, 8(%r26)  	; task stacks rp 

;copy addr + sp to task stack
	ADDI	400, %r1, %r1
	LDD		-368(%r30), %r3 ;rp
	STD		%r3, -368(%r1)
	LDD		-360(%r30), %r3 ;sp
	STD		%r3, -360(%r1)
    LDD		-384(%r30), %r3	;gp
    STD		%r3,-384(%r1) 

;return
	LDD		-384(%r30),%r27	;restore gp
    LDD		-416(%r30),%r2  ;restore rp
    BVE		(%r2)         	;return
    .EXIT
    LDD,MB	-400(%r30),%r3  ;restore old sp
	.PROCEND

;-------------------------------------------------------------------------------------
; Implementation of own context switch interface
;
;   struct RTE_OwnCoroutineContext
;   {
;       void *sp ;          Stack pointer
;       void (*addr)() ;	Return pointer
;   };
;
;   void RTE_OwnSwapContext( struct RTE_OwnCoroutineContext *pCaller,
;                            struct RTE_OwnCoroutineContext *pNew )
; 
; This call replaces swapcontext() but does not check for stack overflow
;
	.SPACE	$TEXT$
	.SUBSPA	$CODE$,QUAD=0,ALIGN=8,ACCESS=0x2c,CODE_ONLY,SORT=24
RTE_OwnSwapContext
$PIC$2
	.PROC
	.CALLINFO NO_CALLS,FRAME=320,SAVE_RP
    .ENTRY
	STD		%r2,8(%r26) 	;arg0[1] = rp
	STD		%r30,0(%r26)	;arg0[0] = sp
	ADDI	400,%r30,%r30
	STD		%r3, -352(%r30)
	STD		%r4, -344(%r30)
	STD		%r5, -336(%r30)
	STD		%r6, -328(%r30)
	STD		%r7, -320(%r30)
	STD		%r8, -312(%r30)
	STD		%r9, -304(%r30)
	STD		%r10, -296(%r30)
	STD		%r11, -288(%r30)
	STD		%r12, -280(%r30)
	STD		%r13, -272(%r30)
	STD		%r14, -264(%r30)
	STD		%r15, -256(%r30)
	STD		%r16, -248(%r30)
	STD		%r17, -240(%r30)
	STD		%r18, -232(%r30)
	FSTD	%fr12, -224(%r30)
	FSTD	%fr13, -216(%r30)
	FSTD	%fr14, -208(%r30)
	FSTD	%fr15, -200(%r30)
	FSTD	%fr16, -192(%r30)
	FSTD	%fr17, -184(%r30)
	FSTD	%fr18, -176(%r30)
	FSTD	%fr19, -168(%r30)
	FSTD	%fr20, -160(%r30)
	FSTD	%fr21, -152(%r30)
	
	LDD		0(%r25),	%r30 ; sp = arg1[0]
	ADDI	400,%r30,	%r30
	LDD		-352(%r30), %r3
	LDD		-344(%r30), %r4
	LDD		-336(%r30), %r5
	LDD		-328(%r30), %r6
	LDD		-320(%r30), %r7
	LDD		-312(%r30), %r8
	LDD		-304(%r30), %r9
	LDD		-296(%r30), %r10
	LDD		-288(%r30), %r11
	LDD		-280(%r30), %r12
	LDD		-272(%r30), %r13
	LDD		-264(%r30), %r14
	LDD		-256(%r30), %r15
	LDD		-248(%r30), %r16
	LDD		-240(%r30), %r17
	LDD		-232(%r30), %r18
	FLDD	-224(%r30), %fr12
	FLDD	-216(%r30), %fr13
	FLDD	-208(%r30), %fr14
	FLDD	-200(%r30), %fr15
	FLDD	-192(%r30), %fr16
	FLDD	-184(%r30), %fr17
	FLDD	-176(%r30), %fr18
	FLDD	-168(%r30), %fr19
	FLDD	-160(%r30), %fr20
	FLDD	-152(%r30), %fr21
	LDD		8(%r25),	%r2	;rp = arg1[1]
    BVE		(%r2)         	;return
    .EXIT
	ADDI	-400,%r30, %r30
	.PROCEND	;

    .SPACE  $TEXT$
    .SUBSPA $CODE$
	.EXPORT	RTE_OwnMakeContext,ENTRY,PRIV_LEV=3,NO_RELOCATION,LONG_RETURN
	.EXPORT	RTE_OwnSwapContext,ENTRY,PRIV_LEV=3,NO_RELOCATION,LONG_RETURN
    .END
&endif
