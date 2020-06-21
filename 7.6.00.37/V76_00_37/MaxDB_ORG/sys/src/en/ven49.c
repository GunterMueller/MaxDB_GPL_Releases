/*!
  @file           ven49.c
  @author         JoergM
  @brief          RunTime: interlock 4fold
  @see            


   The lock algorithm works on an integer variable.
   For both sides, application and kernel, a single byte indicates
   that the respective process has the segment locked; if both bytes
   are zero, the segment is free.
   It is an error if any side finds its byte already set on entry.
   In the kernel case, this indicates illegal reuse of the segment;
   in the application case, it indiates late access by a previous user.

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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

#define MOD__ "ven49.c:"

/* exported functions */

#include "gen49.h"
#include "geo60.h"

/* imported functions */

#ifndef MULTITHREADED
extern	unsigned			sleep ();
#endif /* MULTITHREADED */

/*
 * ===========================================================================
 */
#undef MF__
#define MF__ MOD__"e49_lock_int4_4fold"
externC int e49_lock_int4_4fold (
UINT4 * addr , /* the address of the lock variable */
int     part ) /* one index per contestant */
{
    UINT4                  lockval;
    UINT4                  mask = 0xffffffff;
    volatile teo00_Uint1 * partPointer = ((volatile teo00_Uint1 *)addr) + part;
    volatile teo00_Uint4 * wordPointer = ((volatile teo00_Uint4 *)addr);

    ((teo00_Uint1 *) &mask)[part] = 0x00;

    DBGIN;
    DBG1 (( MF__,"addr 0x%lx part %d lockval 0x%08x\n", addr, part, * wordPointer ));
    DBG1 (( MF__,"mask pattern is 0x%08x\n", mask ));

    do
    {
        lockval = (* wordPointer) & mask; /* check the others only ! */
        if ( lockval )
        {
	    	DBG1 (( MF__,"Returning 1... masked lockval 0x%08x\n", lockval ));
            return ( 1 );
        }

        *partPointer = 0xff; /* try to lock */

        lockval = * wordPointer;
        DBG1 (( MF__,"lockval after first check == 0x%08x\n", lockval ));
        lockval = lockval & mask;
        DBG1 (( MF__,"lockval masked == 0x%08x\n", lockval ));
        if ( lockval ) /* somebody else locked already */
        {
            *partPointer = 0x00; /* reset to allow somebody else in */
#ifdef MULTITHREADED
            sqlyieldthread();
#else
            sleep(0);
#endif /* MULTITHREADED */
        }
    } while( lockval );

    DBGOUT;
    return ( 0 );
}

/*
 *=======================================================================
 */
#undef MF__
#define MF__ MOD__"e49_unlock_int4_4fold"
externC void e49_unlock_int4_4fold ( 
UINT4 * addr , /* the address of the lock variable */
int     part ) /* one index per contestant */
{
    DBGIN;
    DBG1 (( MF__,"addr 0x%lx part %d lockval 0x%08x\n", addr, part, * addr ));
    ((teo00_Uint1 *) addr)[part] = 0x00 ;
    DBGOUT;
}
