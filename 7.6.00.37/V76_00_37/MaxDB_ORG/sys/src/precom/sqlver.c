/* @lastChanged: "1999-10-21  10:49"
 * @filename:    sqlver.c
 * @purpose:     "sqlver shows versions of precompiled programs and runtime"
 * @release:     7.2.0
 * @see:         ""
 *
 * @Copyright (c) 1999-2005 SAP AG"



    ========== licence begin  GPL
    Copyright (c) 1999-2005 SAP AG

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




 */


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include <stdio.h>
#if defined (SOLARIS)
#include <stdlib.h>
#endif
#include "precom/what.h"
#include "gsp100.h"
/*===========================================================================*
 *  CODE                                                                     *
 *===========================================================================*/

int main( int argc , char **argv )
{
  static char *szProgName = "sqlver";
  static char *szSearchArg = COMP_NAME_C_PREC_SP100;
  static char *arg[3];
  arg[0] = szProgName;
  if ( argc != 2 ) { 
    fprintf( stderr , "Usage: %s filename\n" , szProgName );
    exit( 1 );
  } 
  else 
    arg[1] = argv[1];
  arg[2] = szSearchArg;
  PrecomWhat(szProgName, 3, arg);
  return 0;
}
