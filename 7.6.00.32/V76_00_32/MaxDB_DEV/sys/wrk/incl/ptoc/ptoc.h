/*!
\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
    ========== licence end



\endif
*/

#include "ptoc_base.h" /* no check */

/* _ptoc_long is not defined in very old ptoc header files */
#ifndef _ptoc_long
#if defined (_M_IA64) || defined (_M_AMD64)
# define _ptoc_long  __int64
#else
# define _ptoc_long  long
#endif
#endif

#if !defined(_WIN32)
#define vbegexcl (*vbegexcl_func_ptr)
#endif

#define s35add_bufaddr_ptocm(addr,pos) \
    ((tsp00_BufAddr) ((unsigned _ptoc_long) ( ((char *) addr) + (pos) ) ))

#define s35add_moveobj_ptr_ptocm(addr,pos) \
    ((tsp00_MoveObjPtr) ((unsigned _ptoc_long) ( ((char *) addr) + (pos) ) ))

#define s35add_addr1_ptocm(addr,pos) \
    ((tsp00_Addr) ((unsigned _ptoc_long) ( ((char *) addr) + (pos-1) ) ))

#define s35inc_st_ptocm(addr,pos) \
    ((tgg00_StEntryAddr) ((unsigned _ptoc_long) ( ((tgg00_StackEntry *) addr) + (pos) ) ))

#define s35lt_bufaddr_ptocm(addr1,addr2) \
    ((char *) addr1 < (char *) addr2)

#define s35le_bufaddr_ptocm(addr1,addr2) \
    ((char *) addr1 <= (char *) addr2)

#define s35gt_bufaddr_ptocm(addr1,addr2) \
    ((char *) addr1 > (char *) addr2)

#define s35ge_bufaddr_ptocm(addr1,addr2) \
    ((char *) addr1 >= (char *) addr2)

#define s35op_case_ptocm(A,B,C,D) \
   (*A) (B,C,D)
