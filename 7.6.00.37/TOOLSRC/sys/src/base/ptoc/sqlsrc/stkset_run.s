
;
;    ========== licence begin LGPL
;    Copyright (C) 2002 SAP AG
;
;    This library is free software; you can redistribute it and/or
;    modify it under the terms of the GNU Lesser General Public
;    License as published by the Free Software Foundation; either
;    version 2.1 of the License, or (at your option) any later version.
;
;    This library is distributed in the hope that it will be useful,
;    but WITHOUT ANY WARRANTY; without even the implied warranty of
;    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
;    Lesser General Public License for more details.
;
;    You should have received a copy of the GNU Lesser General Public
;    License along with this library; if not, write to the Free Software
;    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
;    ========== licence end
;
	.file   "stkset_run.s"

/	'stket_run' is called immediately after build up of the stack frame:
/		- saved %eip
/		- saved %ebp
/		- optionally allocated stack space ( subl $x,%esp )
/		- optionally saved nonvolatile registers (%ebp,%esi,%edi)
/	Thus, the stack contains additionally:
/		- saved %eip (the instruction to use after stkset_run)
/		- saved %edi (explicitly saved by stkset_run, see below)
/
/	The stack space to set up with the fill value is the
/	'optionally allocated stack space'. This is the stack location
/	from the %eip saved by stkset_run, optionally incremented by the
/	space used for the saved novolatile registers, up to the location
/	just below the saved %ebp.
/
/       stkset_run ( register_saved_mask )
/
/               1       %ebx
/               2       %esi
/               4       %edi

	.globl  stkset_run
	.text
stkset_run :
	pushl   %edi                    / save nonvolatile register

	leal    12(%esp), %edi          / start address to %edi
					/ (skip saved %edi +
					/       saved %eip +
					/       mask)
	movl    8(%esp), %edx           / get mask
	movl    %edx, %ecx              / copy mask
	andl    $1, %ecx                / test, if %ebx was saved
	jz      .L01_noebx              / if not, skip
	addl    $4, %edi                / adjust start address
.L01_noebx :
	movl    %edx, %ecx              / copy mask
	andl    $2, %ecx                / test, if %esi was saved
	jz      .L01_noesi              / if not, skip
	addl    $4, %edi                / adjust start address
.L01_noesi :
	movl    %edx, %ecx              / copy mask
	andl    $4, %ecx                / test, if %edi was saved
	jz      .L01_noedi              / if not, skip
	addl    $4, %edi                / adjust start address
.L01_noedi :

	movl    %ebp, %ecx              / %ebp (end address) to %ecx
	subl    %edi, %ecx              / end - start = fill length

	movl    $0xbbbbbbbb, %eax       / get fillchar

	orl     %ecx, %ecx              / test count for zero
	jz      .L01_regs               / if zero, jump

	cld                             / set direction to forward
	rep                             / repeat the following instruction
					/ %ecx times
	sstob                           / store %al to (%edi), update %edi

.L01_regs:
	popl    %edi                    / restore nonvolatile register

	movl    %edx, %ecx              / copy mask
	andl    $1, %ecx                / test, if %ebx was saved
	jz      .L01_Noebx              / if not, skip
	movl    %eax, %ebx              / set saved nonvolatile register
.L01_Noebx :
	movl    %edx, %ecx              / copy mask
	andl    $2, %ecx                / test, if %esi was saved
	jz      .L01_Noesi              / if not, skip
	movl    %eax, %esi              / set saved nonvolatile register
.L01_Noesi :
	movl    %edx, %ecx              / copy mask
	andl    $4, %ecx                / test, if %edi was saved
	jz      .L01_Noedi              / if not, skip
	movl    %eax, %edi              / set saved nonvolatile register
.L01_Noedi :

	movl    %eax, %ecx              / set volatile register
	movl    %eax, %edx              / set volatile register
	ret

