/*!
  @file           ven15.c
  @author         JoergM
  @brief          Client RunTime: Miscellaneous Functions
  @see            

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

#define MOD__ "ven15.c:"

void                            sqlgeteuid ();
void                            sqlfileexists ();

#include "gen00.h"
#include "heo46.h"

/*
 * ===================================================================
 */
#undef MF__
#define MF__ MOD__"sqlgeteuid"
void                           sqlgeteuid ( 
tsp00_Int4                       *euid )
{
DBGIN;
    *euid = (tsp00_Int4) geteuid ();
DBGOUT;
} 

/*
 * ===================================================================
 */
#undef MF__
#define MF__ MOD__"sqlfileexists"
void                           sqlfileexists ( 
tsp00_VFilename                  filename ,
tsp00_Bool                        * exists )
{

tsp00_VFilenamec               tempn ;
tsp00_Int4                       fildes ;

DBGIN;

fildes = sizeof ( tsp00_VFilename );
eo46PtoC ( tempn , filename , fildes );
fildes = open ( (char *)tempn , O_RDONLY , 0 );
if ( fildes >= 0 )
    {
    * exists  = TRUE ;
    close ( (int) fildes );
    }
else
    {
    * exists  = FALSE ;
    }

DBGOUT;
}
