&if  $ASCOMMENT
;==========================================================================
;
; @file         RTESys_MemoryBarrier.s
; @ingroup      
; @author       RaymondR/JoergM
;
; @brief        Memory Ordering.
;
; The term memory ordering refers to the order in which the processor issues reads (loads) and
; writes (stores) through the system bus to system memory. The different CPU architectures
; supports several memory ordering models depending on the implementation of the architecture.
; For example, the Intel 386 processor enforces program ordering (generally referred to 
; as strong ordering), where reads and writes are issued on the system bus in the order 
; they occur in the instruction stream under all circumstances. To optimize instruction 
; execution, the newer CPU architectures supporting performance enhancing operations such 
; as allowing reads to pass buffered writes. The goal of any of these order variations is
; to increase instruction execution speeds, while maintaining memory coherency, even in 
; multiple-processor systems.
;
; The RTESys_MemoryBarrier, RTESys_WriteMemoryBarrier, and RTESys_ReadMemoryBarrier 
; functions provide a performance-efficient way of insuring load and store memory 
; ordering between routines that produce weakly-ordered results and routines that 
; consume that data.
;
; @since        2003-08-27  16:39
; @sa           
;
; ==========================================================================
;
; \if EMIT_LICENCE
;
;    ========== licence begin  GPL
;    Copyright (c) 2003-2005 SAP AG
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

; \endif
;============================================================================*/
&endif

&if  $OS = WIN32
&if $MACH == AMD64

; ****************************************************************************
; *                                                                          *
; *  A M D 6 4   ( W I N 3 2 )                                               *
; *                                                                          *
; ****************************************************************************
;

PUBLIC  RTESys_WriteMemoryBarrier
_TEXT SEGMENT
RTESys_WriteMemoryBarrier   PROC
	sfence
	ret 0
RTESys_WriteMemoryBarrier   ENDP
_TEXT ENDS

; -----------------------------------------------------------------

PUBLIC  RTESys_ReadMemoryBarrier
_TEXT SEGMENT
RTESys_ReadMemoryBarrier   PROC
	lfence
	ret 0
RTESys_ReadMemoryBarrier   ENDP
_TEXT ENDS

; -----------------------------------------------------------------

PUBLIC  RTESys_MemoryBarrier
_TEXT SEGMENT
RTESys_MemoryBarrier   PROC
	mfence
	ret 0
RTESys_MemoryBarrier   ENDP
_TEXT ENDS
END

&elif $MACH == I386 && defined BIT64
// ****************************************************************************
// *                                                                          *
// *  I N T E L   ( W I N 6 4 )                                               *
// *                                                                          *
// ****************************************************************************
//
//  Note that the Itanium supports only one memory fence (mf) instruction which
//  serializes all store and load operations. Hence, the the three functions does the same.
//
//


 .file    "RTESys_MemoryBarrier.a"
 .section    .text
 .type   RTESys_MemoryBarrier#   ,@function 
 .global RTESys_MemoryBarrier#
 .proc   RTESys_MemoryBarrier#
 .align 32
RTESys_MemoryBarrier: 
 {   .mfb         // Template selection directive (Explicit Bundling)
                  //   A set of templates define the combinations of functional units that can be 
                  //   invoked by a executing a single instruction bundle.
                  //     M - is a memory function
                  //     I - is an integer function
                  //     F - is a floating point function
                  //     B - is a branch function
                  //     L - is a function involving a long immediate
                  //     "s" indicates a stop.
                  
	mf                                      // Memory Fence Instruction
	nop.f	 0
	br.ret.sptk.few b0;;				    // return to caller
 }
 .endp   RTESys_MemoryBarrier#


// END

&elif $MACH == I386 && not defined BIT64
; ****************************************************************************
; *                                                                          *
; *  I N T E L   ( W I N 3 2 )                                               *
; *                                                                          *
; ****************************************************************************
;

    TITLE   RTESys_MemoryBarrier.a
    .386P
    .model FLAT
    
    CPU_ID MACRO
     db 0fh       ; Hardcoded CPUID instruction
     db 0a2h
    ENDM

    S_FENCE MACRO
     db 0fh       ; Hardcoded MFENCE instruction
     db 0aeh
     db 0f8h
    ENDM
    
    M_FENCE MACRO
     db 0fh       ; Hardcoded MFENCE instruction
     db 0aeh
     db 0f0h
    ENDM
    
    L_FENCE MACRO
     db 0fh       ; Hardcoded MFENCE instruction
     db 0aeh
     db 0E8h
    ENDM
    
    .data
    public _CPUType
    public _VendorID
    public _CPUSignature
    public _FeaturesEDX

    _CPUType db 0
    _CPUSignature dd 0
    _VendorID db "------------"
    _FeaturesEDX dd 0
   

PUBLIC _GetCPUType
  _TEXT SEGMENT
    _GetCPUType PROC
     ; Execute CPUID instruction to determine vendor, family,
     ; model, stepping and features. For the purpose of this
     ; code, only the initial set of CPUID information is saved.
     push ebx                               ; save registers
     push esi
     push edi
     mov eax, 0                             ; set up for CPUID instruction
     CPU_ID                                 ; get and save vendor ID
     mov dword ptr _VendorID, ebx
     mov dword ptr _VendorID[+4], edx
     mov dword ptr _VendorID[+8], ecx
     cmp eax, 1                             ; make sure 1 is valid input for CPUID
     jl end_cpuid_type                      ; if not, jump to end
     mov eax, 1
     CPU_ID                                 ; get family/model/stepping/features
     mov _CPUSignature, eax
     mov _FeaturesEDX, edx
     shr eax, 8                             ; isolate family
     and eax, 0fh
     mov _CPUType, al                       ; set _CPUType with family
    end_cpuid_type:
     pop edi ; restore registers
     pop esi
     pop ebx
     ret
    _GetCPUType ENDP
  _TEXT ENDS

; -----------------------------------------------------------------

PUBLIC  _RTESys_WriteMemoryBarrier
  _TEXT SEGMENT
    _RTESys_WriteMemoryBarrier   PROC NEAR
    
      test byte ptr _CPUType,255     ; Test _CPUType with family
      jne dont_get_cpu_type          ; GetCPUType already called?
      call _GetCPUType
     dont_get_cpu_type:
      
      mov edx, _FeaturesEDX
      bt edx, 25t                   ; Feature Flags Bit 25 is SSE support
      jc sse_or_sse2_supported      ; jump if SSE is supported

sse_or_sse2_not_supported:
      lock add dword ptr[esp],0
      ret
      
sse_or_sse2_supported:
      S_FENCE
      ret
    _RTESys_WriteMemoryBarrier   ENDP
  _TEXT ENDS


; -----------------------------------------------------------------

PUBLIC  _RTESys_ReadMemoryBarrier
  _TEXT SEGMENT
    _RTESys_ReadMemoryBarrier   PROC NEAR
    
      test byte ptr _CPUType,255     ; Test _CPUType with family
      jne dont_get_cpu_type          ; GetCPUType already called?
      call _GetCPUType
dont_get_cpu_type:
      
      mov edx, _FeaturesEDX
    
      bt edx, 26t                   ; Feature Flags Bit 26 is SSE2 support
      jc sse2_supported             ; jump if SSE2 is not supported      
      
sse2_not_supported:
      lock add dword ptr[esp],0
      ret
      
sse2_supported:
      L_FENCE
      ret
    _RTESys_ReadMemoryBarrier   ENDP
  _TEXT ENDS

; -----------------------------------------------------------------

PUBLIC  _RTESys_MemoryBarrier
  _TEXT SEGMENT
    _RTESys_MemoryBarrier   PROC NEAR
    
      test byte ptr _CPUType,255     ; Test _CPUType with family
      jne dont_get_cpu_type          ; GetCPUType already called?
      call _GetCPUType
dont_get_cpu_type:
      
      mov edx, _FeaturesEDX
    
      bt edx, 26t                   ; Feature Flags Bit 26 is SSE2 support
      jc sse2_supported             ; jump if SSE2 is not supported      
      
sse2_not_supported:
      lock add dword ptr[esp],0
      ret
      
sse2_supported:
      M_FENCE
      ret
    _RTESys_MemoryBarrier   ENDP
  _TEXT ENDS
END

&endif
&endif

&if  $ASCOMMENT
;-------------------------------------------------------------------------
&endif

&if $OSSPEC in [ SOLARIS, SVR4 ]
&  if $MACH in [ SPARC, SUN ]
        .seg    "text"                  ! [internal]
        .proc   4
        .global RTESys_MemoryBarrier
        .global RTESys_ReadMemoryBarrier
        .global RTESys_WriteMemoryBarrier

&    if $BIT64 = 1

!==========================================================================
! Sun Sparc V9 (64Bit) memory barrier implementation
! found in <sys/atomic.h>: Spitfire and Blackbirds have a problem with membars
! in the delay slot (SF_ERRATA_51). For savety's sake, we assume that the
! whole world needs the workaround. 
!==========================================================================
RTESys_MemoryBarrier:
        membar  #LoadLoad | #LoadStore | #StoreStore
        retl
        nop

RTESys_ReadMemoryBarrier:
        membar  #LoadLoad | #LoadStore
        retl
        nop

RTESys_WriteMemoryBarrier:
        membar  #StoreStore
        retl
        nop

&    else

!==========================================================================
! Sun Sparc V8 (32Bit) memory barrier dummy implementation
!==========================================================================
RTESys_MemoryBarrier:
RTESys_ReadMemoryBarrier:
RTESys_WriteMemoryBarrier:
        retl
		nop

&    endif

&  elif $MACH = X86_64 
      .file   "RTESys_MemoryBarrier.s"
      .text

      .globl RTESys_MemoryBarrier
      .align 4
      .globl RTESys_ReadMemoryBarrier
      .align 4
      .globl RTESys_WriteMemoryBarrier
      .align 4

RTESys_MemoryBarrier:
      sfence
      ret

RTESys_ReadMemoryBarrier:
      lfence
      ret

RTESys_WriteMemoryBarrier:
      mfence
      ret

&  elif $MACH = I386
 	.file	"RTESys_MemoryBarrier.s"
	.text
 
	.globl RTESys_WriteMemoryBarrier
	.align  4

RTESys_WriteMemoryBarrier:
	lock
	addl	$0,(%esp)
	ret
	.globl  RTESys_ReadMemoryBarrier
	.align 4

RTESys_ReadMemoryBarrier:
	lock
	addl	$0,(%esp)
	ret
	.globl RTESys_MemoryBarrier
	.align 4

RTESys_MemoryBarrier:
	lock
	addl    $0,(%esp)
	ret
&  endif
&endif

&if  $ASCOMMENT
;-------------------------------------------------------------------------
&endif

&if  $MACH = ALPHA and $OSSPEC = OSF1

/*==========================================================================
   DEC Alpha only single type of memory barrier (mb command)
  ==========================================================================*/
	.file	2 "RTESys_MemoryBarrier.s"
	.text
	.align	4
	.globl  RTESys_MemoryBarrier
	.globl  RTESys_ReadMemoryBarrier
	.globl  RTESys_WriteMemoryBarrier
    .ent    RTESys_MemoryBarrier
RTESys_MemoryBarrier:
RTESys_ReadMemoryBarrier:
RTESys_WriteMemoryBarrier:
    .frame  $sp, 0, $26, 0
    .prologue       0
    mb
    ret     ($26)
    .end    RTESys_MemoryBarrier

&endif

&if  $ASCOMMENT
;-------------------------------------------------------------------------
&endif

&if  $MACH = I386 and $OSSPEC = LINUX

#==========================================================================
# LINUX Intel coding could be derived from Windows code above...
#==========================================================================
	.file	"RTESys_MemoryBarrier.s"
    .text

    .globl  RTESys_WriteMemoryBarrier
    .align  4
RTESys_WriteMemoryBarrier:
    testb   $0xff,RTESys_i386CPUType   # get RTESys_i386CPUType with family
    jne     _dont_get_cpu_type_wmb  # GetCPUType already called?
    call    _GetCPUType
_dont_get_cpu_type_wmb:

    mov     RTESys_i386FeaturesEDX,%edx
    bt      $25,%edx                # Feature Flags Bit 25 is SSE support
    jc      _sse_or_sse2_supported  # jump if SSE is supported

    lock
    addl    $0,(%esp)
    ret

_sse_or_sse2_supported:
    sfence
    ret

    .globl  RTESys_ReadMemoryBarrier
    .align  4
RTESys_ReadMemoryBarrier:
    testb   $0xff,RTESys_i386CPUType    # get RTESys_i386CPUType with family
    jne     _dont_get_cpu_type_rmb  # GetCPUType already called?
    call    _GetCPUType
_dont_get_cpu_type_rmb:

    mov     RTESys_i386FeaturesEDX,%edx
    bt      $26,%edx                # Feature Flags Bit 26 is SSE2 support
    jc      _sse2_supported         # jump if SSE2 is supported

    lock
    addl    $0,(%esp)
    ret

_sse2_supported:
    lfence
    ret

    .globl  RTESys_MemoryBarrier
    .align  4
RTESys_MemoryBarrier:
    testb   $0xff,RTESys_i386CPUType    # get RTESys_i386CPUType with family
    jne     _dont_get_cpu_type_mb   # GetCPUType already called?
    call    _GetCPUType
_dont_get_cpu_type_mb:

    mov     RTESys_i386FeaturesEDX,%edx
    bt      $26,%edx                # Feature Flags Bit 26 is SSE2 support
    jc      _sse2_supported2        # jump if SSE2 is supported

    lock
    addl    $0,(%esp)
    ret

_sse2_supported2:
    mfence
    ret

&endif

&if  $ASCOMMENT
;-------------------------------------------------------------------------
&endif

&if  $MACH = IA64 and $OSSPEC = LINUX
#--------------------------------------------------------------------------------------------------------------
#   changed to using inline assembler in RTESys_MemoryBarrier.h ... 
#--------------------------------------------------------------------------------------------------------------
&if  $ASCOMMENT
#==========================================================================
# LINUX Intel coding derived from Windows code above...
#==========================================================================
	.file	"RTESys_MemoryBarrier.s"
	.text

	.globl  RTESys_MemoryBarrier
	.align  4
RTESys_MemoryBarrier:
    mf
	nop.f	 0
	br.ret.sptk.few b0;;
&endif
&endif

&if  $ASCOMMENT
;-------------------------------------------------------------------------
&endif

&if $MACH = PA11

;==========================================================================
; HP PA-RISC does not need to know about memory barrier
;==========================================================================
	.LEVEL 1.1
    .SPACE $TEXT$
    .SUBSPA $CODE$,QUAD=0,ALIGN=4,ACCESS=44,CODE_ONLY
RTESys_MemoryBarrier
    .PROC
    .CALLINFO 
    .ENTRY
    BVE     (%r2)
    .EXIT	
    NOP
    .PROCEND

    .SPACE  $TEXT$
    .SUBSPA $CODE$
    .EXPORT RTESys_MemoryBarrier,ENTRY
    .END

&endif

&if  $ASCOMMENT
;-------------------------------------------------------------------------
&endif

&if $MACH = PA20W

;==========================================================================
; HP PA-RISC does not need to know about memory barrier
;==========================================================================
    .LEVEL 2.0W
    .SPACE $TEXT$,SORT=8
    .SUBSPA $CODE$,QUAD=0,ALIGN=8,ACCESS=0x2c,CODE_ONLY,SORT=24
RTESys_MemoryBarrier
    .PROC
    .CALLINFO 
    .ENTRY
    BVE     (%r2)
    .EXIT	
    NOP
    .PROCEND

    .SPACE  $TEXT$
    .SUBSPA $CODE$
    .EXPORT RTESys_MemoryBarrier,ENTRY
    .END

&endif

&if  $ASCOMMENT
;-------------------------------------------------------------------------
&endif

&if  $MACH = HP_IA64

&if  $ASCOMMENT
//==========================================================================
// HP on IA64 uses <ia64/sys/inline.h> _Asm_mf() in "RTESys_MemoryBarrier.h"
//==========================================================================
&endif

&endif

&if  $ASCOMMENT
;-------------------------------------------------------------------------
&endif

&if  $MACH = _IBMR2

#--------------------------------------------------------------------------------------------------------------
#   changed to using pragmas in RTESys_MemoryBarrier.h ... but with Visual Age 6.0 this code may be reactivated
#--------------------------------------------------------------------------------------------------------------
&if  $ASCOMMENT
        .set CR0_LT,0;
		.set BO_ALWAYS,20;
#-------------------------------------------------------------------------
# text section
        .file	"RTESys_AsmSpinlock.s"
		.globl		.RTESys_WriteMemoryBarrier[pr]
		.csect		.RTESys_WriteMemoryBarrier[pr]
.RTESys_WriteMemoryBarrier:
		lwsync
		bcr     BO_ALWAYS,CR0_LT

		.long   0x00000000
        .byte   0x00                    # VERSION=0
        .byte   0x00                    # LANG=TB_CPLUSPLUS
        .byte   0x20                    # IS_GL=0,IS_EPROL=0,HAS_TBOFF=1
                                        # INT_PROC=0,HAS_CTL=0,TOCLESS=0
                                        # FP_PRESENT=0,LOG_ABORT=0
        .byte   0x40                    # INT_HNDL=0,NAME_PRESENT=1
                                        # USES_ALLOCA=0,CL_DIS_INV=WALK_ONCOND
                                        # SAVES_CR=0,SAVES_LR=0
        .byte   0x00                    # STORES_BC=0,FPR_SAVED=0
        .byte   0x00                    # GPR_SAVED=0
        .byte   0x00                    # FIXEDPARMS=0
        .byte   0x01                    # FLOATPARMS=0,PARMSONSTK=1
        .long   0x00000004              # TB_OFFSET
        .short  25                      # NAME_LEN
        .byte   "RTESys_WriteMemoryBarrier"
        .byte   0                       # padding
        .byte   0                       # padding
#-------------------------------------------------------------------------

		.globl		.RTESys_ReadMemoryBarrier[pr]
		.csect		.RTESys_ReadMemoryBarrier[pr]
.RTESys_ReadMemoryBarrier:
		isync
		bcr     BO_ALWAYS,CR0_LT

		.long   0x00000000
        .byte   0x00                    # VERSION=0
        .byte   0x00                    # LANG=TB_CPLUSPLUS
        .byte   0x20                    # IS_GL=0,IS_EPROL=0,HAS_TBOFF=1
                                        # INT_PROC=0,HAS_CTL=0,TOCLESS=0
                                        # FP_PRESENT=0,LOG_ABORT=0
        .byte   0x40                    # INT_HNDL=0,NAME_PRESENT=1
                                        # USES_ALLOCA=0,CL_DIS_INV=WALK_ONCOND
                                        # SAVES_CR=0,SAVES_LR=0
        .byte   0x00                    # STORES_BC=0,FPR_SAVED=0
        .byte   0x00                    # GPR_SAVED=0
        .byte   0x00                    # FIXEDPARMS=0
        .byte   0x01                    # FLOATPARMS=0,PARMSONSTK=1
        .long   0x00000004              # TB_OFFSET
        .short  24                      # NAME_LEN
        .byte   "RTESys_ReadMemoryBarrier"
        .byte   0                       # padding
        .byte   0                       # padding
#-------------------------------------------------------------------------

		.globl		.RTESys_MemoryBarrier[pr]
		.csect		.RTESys_MemoryBarrier[pr]
.RTESys_MemoryBarrier:
		sync
		bcr     BO_ALWAYS,CR0_LT

		.long   0x00000000
        .byte   0x00                    # VERSION=0
        .byte   0x00                    # LANG=TB_CPLUSPLUS
        .byte   0x20                    # IS_GL=0,IS_EPROL=0,HAS_TBOFF=1
                                        # INT_PROC=0,HAS_CTL=0,TOCLESS=0
                                        # FP_PRESENT=0,LOG_ABORT=0
        .byte   0x40                    # INT_HNDL=0,NAME_PRESENT=1
                                        # USES_ALLOCA=0,CL_DIS_INV=WALK_ONCOND
                                        # SAVES_CR=0,SAVES_LR=0
        .byte   0x00                    # STORES_BC=0,FPR_SAVED=0
        .byte   0x00                    # GPR_SAVED=0
        .byte   0x00                    # FIXEDPARMS=0
        .byte   0x01                    # FLOATPARMS=0,PARMSONSTK=1
        .long   0x00000004              # TB_OFFSET
        .short  20                      # NAME_LEN
        .byte   "RTESys_MemoryBarrier"
        .byte   0                       # padding
        .byte   0                       # padding
#-------------------------------------------------------------------------
&endif
&endif

