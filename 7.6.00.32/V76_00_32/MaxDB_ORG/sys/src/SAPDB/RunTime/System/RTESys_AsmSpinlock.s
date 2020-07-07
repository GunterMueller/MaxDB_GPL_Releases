&if COMMENT = 1
;****************************************************************************
;
; module      : RTE_AsmSpinlock.s
;
; -------------------------------------------------------------------------
;
; responsible : JoergM
;
; special area: RTE System Interface
; description : Spinlock coding for various platforms
;
; -------------------------------------------------------------------------
;
; copyright:    Copyright (c) 2000-2005 SAP AG
;
;*****************************************************************************/
;
;**************************************************************************
;
;************************************************************************ 
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
;------------------------------------------------------------------------------
; Implementation of RTESys_AsmTestAndLock
;
; This routine atomically tests the spinlock at 'lockAddr' and returns
; 'true' if the spinlock is already locked.
; Otherwise it is set into locked state and 'false' is returned.
;
; @param lockAddr [in] pointer to the spinlock address
; 
; SAPDB_Bool RTESys_AsmTestAndLock(RTE_Lock *lockAddr)
;
; @return [SAPDB_Bool] true if spinlock is already locked, false else
;
; This routine does not block! See RTESys_Lock() for a blocking variant!
;
;------------------------------------------------------------------------------
; Implementation of RTESys_AsmUnlock
;
; This routine atomically unlocks the spinlock at 'lockAddr' and
; forces all memory store operation to complete (reach global visibility)
; before the spinlock state is reset to unlocked.
;
; @param lockAddr [in] pointer to the spinlock address
;
; SAPDB_Bool RTESys_AsmTestAndLock(RTE_Lock *lockAddr)
;
; @return [SAPDB_Bool] always 0
;
; This routine does not block!
;
&endif

&if $OSSPEC = LINUX AND $MACH = I386
        .file  "RTESys_AsmSpinlock.s"
        .data
	    .globl  RTESys_i386CPUType
		.globl  RTESys_i386VendorID
		.globl  RTESys_i386CPUSignature
	    .globl  RTESys_i386FeaturesEDX
	    .align  4
RTESys_i386VendorID:     .byte '-','-','-','-','-','-','-','-','-','-','-','-',0
RTESys_i386CPUType:      .byte 0
		.align  4
RTESys_i386CPUSignature: .int 0
RTESys_i386FeaturesEDX:  .int 0

	    .text
		.globl  _GetCPUType
	    .align  4
_GetCPUType:
# Execute CPUID instruction to determine vendor, family,
# model, stepping and features. For the purpose of this
# code, only the initial set of CPUID information is saved.

	     push   %ebx    ################ save registers

		 mov    $0,%eax         # set up for CPUID instruction
	     cpuid                  # get and save vendor ID
		 mov    %ebx,RTESys_i386VendorID
	     mov    %edx,RTESys_i386VendorID+4
		 mov    %ecx,RTESys_i386VendorID+8
	     cmpl   $1,%eax         # make sure 1 is valid input for CPUID
		 jl     _end_cpuid_type # if not, jump to end

	     mov    $1,%eax
		 cpuid                  # get family/model/stepping/features
		 mov    %eax,RTESys_i386CPUSignature
		 mov    %edx,RTESys_i386FeaturesEDX
		 shrl   $8,%eax         # isolate family
		 and    $0x0f,%eax
		 mov    %al,RTESys_i386CPUType # set RTESys_CPUType with family

_end_cpuid_type:

	     pop    %ebx  ################### restore registers
		 ret

        .globl  RTESys_AsmTestAndLock
        .align  4
		.type   RTESys_AsmTestAndLock,@function
RTESys_AsmTestAndLock:
        movl    4(%esp),%edx            # get (* addr)
        movl    (%edx), %eax            # get the current lock value
        movl    $1, %ecx                # set the intended lock value

_lock_again:
        lock
        cmpxchg %ecx,(%edx)
		jne     _lock_again

		ret

#-------------------------------------------
#
# Old implementation not using memory barrier
# 
#        .globl  RTESys_AsmUnlock
#        .align  4
#		.type   RTESys_AsmUnlock,@function
#RTESys_AsmUnlock:
#        movl    4(%esp),%edx            # get (* addr)
#        movl    (%edx), %eax            # get the current lock value
#        xorl    %ecx,%ecx               # set the intended unlock value
#
#_unlock_again:
#        lock
#        cmpxchg %ecx,(%edx)
#		jne     _unlock_again			# loop until no collision detected
#
#		ret
#-------------------------------------------
#
# Alternative implementation using memory barrier
# 
        .globl  RTESys_AsmUnlock
        .align  4
		.type   RTESys_AsmUnlock,@function
RTESys_AsmUnlock:
        testb   $0xff,RTESys_i386CPUType   # get RTESys_i386CPUType with family
        jne     _got_cpu_type_unlock # GetCPUType already called?
        call    _GetCPUType
_got_cpu_type_unlock:

        mov     RTESys_i386FeaturesEDX,%edx
        bt      $25,%edx                # Feature Flags Bit 25 is SSE support
        jc      _memory_barrier_supported_unlock  # jump if SSE is supported
#
# Cannot use memory barrier, fallback to lock cmpxchg
#
        movl    4(%esp),%edx            # get (* addr)
        movl    (%edx), %eax            # get the current lock value
        xorl    %ecx,%ecx               # set the intended unlock value
_unlock_again:
        lock
        cmpxchg %ecx,(%edx)
		jne     _unlock_again			# loop until no collision detected
		ret
#
# Using write memory barrier
#
_memory_barrier_supported_unlock:
        movl    4(%esp),%edx            # get (* addr)
		sfence							# write memory barrier
        movl    $0, (%edx)              # set the intended lock value
        ret
&endif

&if $OSSPEC = LINUX AND $MACH = X86_64
        .file  "RTESys_AsmSpinlock.s"
        .text
        .globl  RTESys_AsmTestAndLock
        .align  16
		.type   RTESys_AsmTestAndLock,@function
RTESys_AsmTestAndLock:
         mov    $1, %rax                # set the intended lock value
         lock                           # lock the bus for other processors
                                        # during the following instruction
         xchg   %eax , (%rdi)           # exchange %eax and (* addr)
                                        # %eax contains the previous (* addr)
         ret

        .globl  RTESys_AsmUnlock
        .align  16
		.type   RTESys_AsmUnlock,@function
RTESys_AsmUnlock:
         mov    $0, %rax                # reset lock
         sfence							# write memory barrier
         mov    %eax, (%rdi)            # at (* addr)
         ret
&endif

&if $OSSPEC = LINUX AND $MACH = IA64
        .file  "RTESys_AsmSpinlock.s"
        .text
        .align 16
        .global RTESys_AsmTestAndLock#
        .proc RTESys_AsmTestAndLock#
RTESys_AsmTestAndLock:
        .prologue 2, 2
        .vframe r2
        mov r2 = r12
        .body
        ;;
        st8 [r2] = r32
        ;;
        mov ar.ccv = 0 ;;
        ;;
        ld8 r16 = [r2]
        ld8 r15 = [r2]
        addl r14 = 1, r0
        ;;
        cmpxchg4.acq r14=[r15],r14,ar.ccv
        adds r15 = 8, r2
        ;;
        st4 [r15] = r14
        adds r14 = 8, r2
        ;;
        ld4 r14 = [r14]
        ;;
        cmp4.ne p6, p7 = 0, r14
        ;;
        (p6) addl r14 = 1, r0
        (p7) mov r14 = r0
        ;;
        mov r8 = r14
        .restore sp
        mov r12 = r2
        br.ret.sptk.many b0
        ;;
        .endp RTESys_AsmTestAndLock#

        .align 16
        .global RTESys_AsmUnlock#
        .proc RTESys_AsmUnlock#
RTESys_AsmUnlock:
        .prologue 2, 2
        .vframe r2
        mov r2 = r12
        .body
        ;;
        st8 [r2] = r32
        ;;
        mov ar.ccv = 1 ;;
        ;;
        ld8 r16 = [r2]
        ld8 r15 = [r2]
        mov r14 = r0
        ;;
        cmpxchg4.rel r14=[r15],r14,ar.ccv
        adds r15 = 8, r2
        ;;
        st4 [r15] = r14
        .restore sp
        mov r12 = r2
        br.ret.sptk.many b0
        ;;
        .endp RTESys_AsmUnlock#
&endif

&if $MACH = PA20W
    .LEVEL 2.0W
    .SPACE $TEXT$,SORT=8
    .SUBSPA $CODE$,QUAD=0,ALIGN=8,ACCESS=0x2c,CODE_ONLY,SORT=24
RTESys_AsmTestAndLock
; RTESys_AsmTestAndLock(addr)
;*****************************************************
; RTESys_AsmTestAndLock is true if we succeeded in acquiering the latch
    .PROC
    .CALLINFO
    .ENTRY
     stbys,e 0,(arg0)        ; scrub cache; important for performance
     ldcws (arg0), ret0      ; load and clear the spinlock word
     nop                     ; 3 No-Op instructions; needed for older
     nop                     ; HP-PA chips
     nop
     bve (rp)
	 nop
    .EXIT
    .PROCEND

    .SPACE  $TEXT$
    .SUBSPA $CODE$
    .EXPORT RTESys_AsmTestAndLock,ENTRY
    .END
&endif

&if $MACH in [ HP9, PA11 ]
    .code
;**************************************************************************
; Test and set routines implemented with the LDCWS instruction
;
; The LDCWS instruction is very similar to a traditional test+set instruction
; with the following exceptions:
;   o Zero means set and One means clear.
;   o Word must be 16 byte aligned
;
; Acheiving 16 byte alignment is awkward in C, so these routines have
; been designed to take a larger unaligned structure and round up to the
; first 16 byte aligned word of the structure.
;
; A reasonable C declaration for this structure could be:
;     typedef  struct { int words[4]; }  latch_t.
;
; On future multiprocessor  machines,  normal load and store instructions could
; be reordered arbitrarily by the hardware.  The stws,ma 0() and ldws,ma 0()
; instructions will force a synchronization of reordered loads and stores.
;************************************************************************

; RTESys_AsmTestAndLock(addr)
;*****************************************************
; RTESys_AsmTestAndLock is true if we succeeded in acquiering the latch
;****************************************************
    .proc
    .callinfo
    .export RTESys_AsmTestAndLock
RTESys_AsmTestAndLock
     stbys,e 0,(arg0)        ; scrub cache; important for performance
     ldcws (arg0), ret0      ; load and clear the spinlock word
     nop                     ; 3 No-Op instructions; needed for older
     nop                     ; HP-PA chips
     nop
     bve (rp)
	 nop
    .procend
&endif

&if $OSSPEC in [ SOLARIS, SVR4 ]
&  if $MACH in [ SPARC, SUN ] 
!-----------------------------------------------------------------------------
!       Sparc: RTE_Lock is 4 byte long
!-----------------------------------------------------------------------------
&    if $BIT64 = 1
        .seg    "text"                  ! [internal]
        .proc   4
        .global RTESys_AsmTestAndLock
RTESys_AsmTestAndLock:
        mov     0, %o1                  ! expected value
        mov     1, %o2                  ! new value
        cas     [%o0], %o1, %o2         ! compare and exchange
        cmp     %o2, %o1
        bne,a   LockAlreadySet
        nop
        membar  #StoreLoad | #StoreStore ! membar_enter
LockAlreadySet:
        retl
        mov     %o2, %o0

        .global RTESys_AsmUnlock
RTESys_AsmUnlock:
        membar  #LoadStore | #StoreStore ! membar_exit
        mov     0, %o1
        retl
        st      %o1, [%o0]

        .global RTESys_AtomicCmpxchgWithMembarInt4
RTESys_AtomicCmpxchgWithMembarInt4:
        cas     [%o0], %o1, %o2         ! compare and exchange
        st      %o2, [%o3]
        clruw   %o2
        clruw   %o1
        mov     0, %o0
        cmp     %o2, %o1
        retl
        move    %icc, 1, %o0

        .global RTESys_AtomicCmpxchgWithMembarInt8
RTESys_AtomicCmpxchgWithMembarInt8:
        casx    [%o0], %o1, %o2         ! compare and exchange
        stx     %o2, [%o3]
        mov     0, %o0
        cmp     %o2, %o1
        retl
        move    %icc, 1, %o0
&    else
!-----------------------------------------------------------------------------
!       Sparc V8 (32Bit) does not know membar opcode...
!-----------------------------------------------------------------------------
        .seg    "text"                  ! [internal]
        .proc   4
        .global RTESys_AsmTestAndLock
RTESys_AsmTestAndLock:
        mov     1,%o1
        swap    [%o0],%o1
        tst     %o1
        bne,a   LockAlreadySet
        mov     1,%o0
        mov     0,%o0
LockAlreadySet:
        retl
        nop                             ! [internal]

        .global RTESys_AsmUnlock
RTESys_AsmUnlock:
        mov     0,%o1
        swap    [%o0],%o1
        mov     0,%o0
        retl
        nop
&    endif
	
&  elif $MACH = X86_64
        .file  "RTESys_AsmSpinlock.s"
        .text
        .globl  RTESys_AsmTestAndLock
        .align  16
        .type   RTESys_AsmTestAndLock,@function
RTESys_AsmTestAndLock:
         movq   $1, %rax                // set the intended lock value
         lock                           // lock the bus for other processors
                                        // during the following instruction
         xchg   %eax , (%rdi)           // exchange %eax and (* addr)
                                        // %eax contains the previous (* addr)
         ret
        .globl  RTESys_AsmUnlock
        .align  16
        .type   RTESys_AsmUnlock,@function
RTESys_AsmUnlock:
         movq   $0, %rax                // reset lock
         sfence                         // write memory barrier
         mov    %eax, (%rdi)            // at (* addr)
         ret
        .globl RTESys_AtomicCmpxchgWithMembarInt4
        .align  16
        .type   RTESys_AtomicCmpxchgWithMembarInt4, @function
RTESys_AtomicCmpxchgWithMembarInt4:
        pushq   %rbp
        movq    %rsp, %rbp
        movq    %rdi, -8(%rbp)
        movl    %esi, -12(%rbp)
        movl    %edx, -16(%rbp)
        movq    %rcx, -24(%rbp)
        movq    -24(%rbp), %rsi
        movq    -8(%rbp), %rcx
        movl    -16(%rbp), %edx
        movl -12(%rbp), %eax
        lock
        cmpxchgl %edx, (%rcx)
        movl %eax, (%rsi)
        movq    -24(%rbp), %rdx
        movl    -12(%rbp), %eax
        cmpl    (%rdx), %eax
        sete    %al
        movzbl  %al, %eax
        leave
        ret
        .globl RTESys_AtomicCmpxchgWithMembarInt8
        .align  16
        .type   RTESys_AtomicCmpxchgWithMembarInt8, @function
RTESys_AtomicCmpxchgWithMembarInt8:
        pushq   %rbp
        movq    %rsp, %rbp
        pushq   %rbx
        movq    %rdi, -16(%rbp)
        movq    %rsi, -24(%rbp)
        movq    %rdx, -32(%rbp)
        movq    %rcx, -40(%rbp)
        movq    -32(%rbp), %rax
        sarq    $32, %rax
        movl    %eax, -44(%rbp)
        movq    -24(%rbp), %rax
        sarq    $32, %rax
        movl    %eax, -48(%rbp)
        movq    -16(%rbp), %rsi
        push %ebx
        movl -24(%rbp), %eax
        movl -48(%rbp), %edx
        movl -32(%rbp), %ebx
        movl -44(%rbp), %ecx
        lock
        cmpxchg8b (%rsi)
        movl %edx, -48(%rbp)
        movl %eax, -44(%rbp)
        pop %ebx
        movq    -40(%rbp), %rcx
        mov     -44(%rbp), %edx
        mov     -48(%rbp), %eax
        salq    $32, %rax
        orq     %rdx, %rax
        movq    %rax, (%rcx)
        movq    -40(%rbp), %rdx
        movq    -24(%rbp), %rax
        cmpq    (%rdx), %rax
        sete    %al
        movzbl  %al, %eax
        popq    %rbx
        leave
        ret
	
&  elif $MACH = I386
         .file  "RTESys_AsmSpinlock.s"
	.align 4
	.text
	.globl RTESys_AsmTestAndLock
	.type  RTESys_AsmTestAndLock,@function
RTESys_AsmTestAndLock:
	movl  4(%esp),%edx
	movl  (%edx), %eax 
	movl  $1, %ecx

_lock_again:
	lock
	cmpxchg %ecx,(%edx)
	jne	_lock_again
	ret

	.globl	RTESys_AsmUnlock
	.align	4
	.type	RTESys_AsmUnlock,@function
RTESys_AsmUnlock:
	movl	4(%esp),%edx
	movl	(%edx), %eax
	xorl	%ecx,%ecx
_unlock_again:
	lock
	cmpxchg %ecx,(%edx)
	jne    _unlock_again

	ret
	# Solaris x86 32-bit missing /* JnZ ... */
&  endif
&endif

&if  $MACH = ALPHA
/*--------------------------------------------------
  Alpha uses memory barrier for lock and unlock
 ---------------------------------------------------*/
        .file   2 "RTESys_AsmSpinlock.s"
        .text
        .align  4
        .globl  RTESys_AsmTestAndLock
        .ent    RTESys_AsmTestAndLock
RTESys_AsmTestAndLock:
        ldgp    $gp, 0($27)
        .frame  $sp, 0, $26, 0
        .prologue       1
        ldl_l   $1, 0($16)      /* get *addr */
        blbs    $1, LockAlreadySet
        or      $1, 1, $2       /* 0 becomes 1 */
        stl_c   $2, 0($16)      /* try to write to *addr */
        beq     $2, LockAlreadySet
        mb                      /* memory barrier #1: critical section begins */
        bis     $31, $31, $0    /* set: return 0 */
        ret     $31, ($26), 1   /* return to caller */
LockAlreadySet:
        ldil    $0, 1           /* not set: return 1 */
        ret     $31, ($26), 1   /* return to caller */
        .end    RTESys_AsmTestAndLock

        .globl  RTESys_AsmUnlock
        .ent    RTESys_AsmUnlock 2
RTESys_AsmUnlock:
        ldgp    $gp, 0($27)
        .frame  $sp, 0, $26, 0
        .prologue       1
        mb                      /* memory barrier #2: critical section ends */
        stl     $31, 0($16)
        bis     $31, $31, $0
        ret     $31, ($26), 1   /* return 0 */
        .end    RTESys_AsmUnlock
&endif
