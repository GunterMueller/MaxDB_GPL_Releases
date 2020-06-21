
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
!#/ @(#)sql__ucmp.c		2.0.5  1996-09-03  SQL DBS

	.seg	"text"			! [internal]
	.proc	2
	.global _sql__ucmp
!#/============================================================================/
!#/
!#/       int  sql__ucmp ( src1 , src2 , lgt )
!#/       char * src1 ;
!#/       char * src2 ;
!#/       int  lgt ;
!#/
!#/       SUN SPARC version
!#/
_sql__ucmp:
		cmp     %o0, %o1
		be      L_u_mem_0xc4
		cmp     %o2, 0x7
		ble,a   L_u_mem_0x9c
		sub     %o1, %o0, %o1
		andcc   %o0, 0x3, %o3
		be,a    L_u_mem_0x12c
		andcc   %o1, 0x3, %o4
		cmp     %o3, 0x2
		be      L_u_mem_0x54
		cmp     %o3, 0x3
		ldsb    [%o0], %o4
		add     %o0, 0x1, %o0
		ldsb    [%o1], %o5
		add     %o1, 0x1, %o1
		sub     %o2, 0x1, %o2
		be      L_u_mem_0x90
		cmp     %o4, %o5
		be      L_u_mem_0x54
		nop
		ba,a    L_u_mem_0x124
L_u_mem_0x54:   ldsh    [%o0], %o4
		add     %o0, 0x2, %o0
		ldsb    [%o1], %o5
		add     %o1, 0x1, %o1
		sra     %o4, 0x8, %o3
		cmp     %o3, %o5
		be,a    L_u_mem_0x7c
		ldsb    [%o1], %o5
L_u_mem_0x74:   ba      L_u_mem_0x124
		mov     %o3, %o4
L_u_mem_0x7c:   add     %o1, 0x1, %o1
		sub     %o2, 0x2, %o2
		sll     %o4, 0x18, %o4
		sra     %o4, 0x18, %o4
		cmp     %o4, %o5
L_u_mem_0x90:   be,a    L_u_mem_0x12c
		andcc   %o1, 0x3, %o4
		ba,a    L_u_mem_0x124
L_u_mem_0x9c:   ba      L_u_mem_0xbc
		subcc   %o2, 0x1, %o2
L_u_mem_0xa4:   ldsb    [%o0 + %o1], %o5
		add     %o0, 0x1, %o0
		cmp     %o4, %o5
		be,a    L_u_mem_0xbc
		subcc   %o2, 0x1, %o2
		ba,a    L_u_mem_0x124
L_u_mem_0xbc:   bge,a   L_u_mem_0xa4
		ldsb    [%o0], %o4
L_u_mem_0xc4:   jmp     %o7 + 0x8
		clr     %o0
L_u_mem_0xcc:   sra     %o4, 0x18, %o1
		sra     %o5, 0x18, %o2
		cmp     %o1, %o2
		bne     L_u_mem_0x11c
		sll     %o4, 0x8, %o4
		sll     %o5, 0x8, %o5
		sra     %o4, 0x18, %o1
		sra     %o5, 0x18, %o2
		cmp     %o1, %o2
		bne     L_u_mem_0x11c
		sll     %o4, 0x8, %o4
		sll     %o5, 0x8, %o5
		sra     %o4, 0x18, %o1
		sra     %o5, 0x18, %o2
		cmp     %o1, %o2
		bne     L_u_mem_0x11c
		sll     %o4, 0x8, %o4
		sll     %o5, 0x8, %o5
		sra     %o4, 0x18, %o1
		sra     %o5, 0x18, %o2
L_u_mem_0x11c:  and     %o1,255,%o1
		and     %o2,255,%o2
		jmp     %o7 + 0x8
		sub     %o1, %o2, %o0
L_u_mem_0x124:  and     %o4,255,%o4
		and     %o5,255,%o5
		jmp     %o7 + 0x8
		sub     %o4, %o5, %o0
L_u_mem_0x12c:  andn    %o2, 0x3, %o3
		and     %o2, 0x3, %o2
		be      L_u_mem_0x218
		cmp     %o4, 0x2
		be      L_u_mem_0x1d4
		cmp     %o4, 0x1
		ldub    [%o1], %g1
		add     %o1, 0x1, %o1
		be      L_u_mem_0x18c
		sll     %g1, 0x18, %o5
		sub     %o1, %o0, %o1
L_u_mem_0x158:  ld      [%o0 + %o1], %g1
		ld      [%o0], %o4
		add     %o0, 0x4, %o0
		srl     %g1, 0x8, %g2
		or      %g2, %o5, %o5
		cmp     %o4, %o5
		bne     L_u_mem_0xcc
		subcc   %o3, 0x4, %o3
		bne     L_u_mem_0x158
		sll     %g1, 0x18, %o5
		sub     %o1, 0x1, %o1
		ba      L_u_mem_0xbc
		subcc   %o2, 0x1, %o2
L_u_mem_0x18c:  lduh    [%o1], %g1
		add     %o1, 0x2, %o1
		sll     %g1, 0x8, %g2
		or      %o5, %g2, %o5
		sub     %o1, %o0, %o1
L_u_mem_0x1a0:  ld      [%o0 + %o1], %g1
		ld      [%o0], %o4
		add     %o0, 0x4, %o0
		srl     %g1, 0x18, %g2
		or      %g2, %o5, %o5
		cmp     %o4, %o5
		bne     L_u_mem_0xcc
		subcc   %o3, 0x4, %o3
		bne     L_u_mem_0x1a0
		sll     %g1, 0x8, %o5
		sub     %o1, 0x3, %o1
		ba      L_u_mem_0xbc
		subcc   %o2, 0x1, %o2
L_u_mem_0x1d4:  lduh    [%o1], %g1
		add     %o1, 0x2, %o1
		sll     %g1, 0x10, %o5
		sub     %o1, %o0, %o1
L_u_mem_0x1e4:  ld      [%o0 + %o1], %g1
		ld      [%o0], %o4
		add     %o0, 0x4, %o0
		srl     %g1, 0x10, %g2
		or      %g2, %o5, %o5
		cmp     %o4, %o5
		bne     L_u_mem_0xcc
		subcc   %o3, 0x4, %o3
		bne     L_u_mem_0x1e4
		sll     %g1, 0x10, %o5
		sub     %o1, 0x2, %o1
		ba      L_u_mem_0xbc
		subcc   %o2, 0x1, %o2
L_u_mem_0x218:  sub     %o1, %o0, %o1
		ld      [%o0 + %o1], %o5
L_u_mem_0x220:  ld      [%o0], %o4
		add     %o0, 0x4, %o0
		cmp     %o4, %o5
		bne     L_u_mem_0xcc
		subcc   %o3, 0x4, %o3
		bne,a   L_u_mem_0x220
		ld      [%o0 + %o1], %o5
		ba      L_u_mem_0xbc
		subcc   %o2, 0x1, %o2
		nop
