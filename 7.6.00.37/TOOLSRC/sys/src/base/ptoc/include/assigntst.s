
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
	.file	"assigntst.c"
	.version	"02.01"
	.data
	.align	4
	.globl	b
b:
	.long	7
	.long	0x34333231
	.long	0x00373635
	.long	55
	.comm	d,236
	.comm	f,512
	.text
	.align	4
	.def	main;	.val	main;	.scl	2;	.type	044;	.endef
	.globl	main
main:
	pushl	%ebp
	movl	%esp,%ebp
	subl	$764,%esp
	pushl	%edi
	pushl	%esi
	movl	$b,%eax
	movl	(%eax),%ecx
	movl	%ecx,-16(%ebp)
	movl	4(%eax),%ecx
	movl	%ecx,-12(%ebp)
	movl	8(%eax),%ecx
	movl	%ecx,-8(%ebp)
	movl	12(%eax),%ecx
	movl	%ecx,-4(%ebp)
	movl	$d,%esi
	leal	-252(%ebp),%edi
	movl	$59,%ecx
	rep	
	smovl	
	movl	$f+7,%eax
	movl	$d,%esi
	movl	%eax,%edi
	movl	$59,%ecx
	rep	
	smovl	
	leal	-743(%ebp),%esi
	movl	%eax,%edi
	movl	$6,%ecx
	rep	
	smovl	
	movw	1(%esi),%cx
	movw	%cx,1(%edi)
	movb	0(%esi),%cl
	movb	%cl,0(%edi)
	popl	%esi
	popl	%edi
	leave	
	ret	
	.align	4
	.def	main;	.val	.;	.scl	-1;	.endef
	.data
	.text
