&if  $ASCOMMENT
/*!***************************************************************************

  module      : RTE_IA64GetBsp.s

  -------------------------------------------------------------------------

  responsible : Robin

  special area: RTE
  description : Get the Backing Store out of the depths of the IA64 processor...

  As this pointer is stored in a special register, the implementation is
  found in an assembly language module

  last changed: 2004-03-01  17:32
  see also    : 

  -------------------------------------------------------------------------

  copyright:    Copyright by SAP AG, 2004

;    ========== licence begin  GPL
;    Copyright (c) 2004-2005 SAP AG
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



*****************************************************************************/
/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/
&endif
// This code is for Windows, Linux and HP-UX
    .file	"RTE_IA64GetBsp.c"
&if $OSSPEC = LINUX
    .pred.safe_across_calls p1-p5,p16-p63
&endif
&if $OSSPEC = HPUX
    .psr msb
&endif
    .psr abi64
    .text
    .type   RTE_IA64GetBsp#   ,@function 
    .global RTE_IA64GetBsp#
    .proc   RTE_IA64GetBsp#
&if  $OS = WIN32
    .align 32
&else 
    .align 16
&endif	
RTE_IA64GetBsp: 
    mov r8 = ar17			// the backing store pointer is stored in register ar17, 
	                        // the return value of a function is expected in register r8
    br.ret.sptk.many b0;;	// return to caller
    .endp   RTE_IA64GetBsp#
