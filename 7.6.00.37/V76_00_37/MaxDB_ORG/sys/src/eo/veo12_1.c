/*!
  @file           veo12_1.c
  @author         JoergM
  @special area   GetDefaultDBName ( using sqlarg3 )
  @brief          description ...
  @see            example.html ...

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


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "vsp004.h"
#include "gsp00.h"
#include "geo00_0.h"
#include "heo46.h"
#include "heo12_1.h"
#include "heo102.h"

/*===========================================================================*
 *  GLOBAL FUNCTIONS (CODE)                                                  *
 *===========================================================================*/

int GetDefaultDBName ( tsp00_DbNamec  DefaultDBName, 
					   tsp00_ErrTextc ErrTextc )
{

  #undef  MF__
  #define MF__ MOD__"GetDefaultDBName"

  struct tsp4_xuser_record        UserParams ;
  tsp00_Pw                        password ;
  struct tsp4_args_options        options ;
  tsp4_xuserset                   xusertype ;
  tsp00_ErrText                   ErrorText ;
  tsp00_Bool                      Ok ;
 
  options.opt_component = sp4co_sql_pc_runtime ;
  sqlarg3 ( &UserParams , password , &options , xusertype , ErrorText , &Ok );

  if ( Ok )
    eo46PtoC( DefaultDBName, UserParams.xu_serverdb, sizeof (tsp00_DbName) );
  else
    eo46PtoC( ErrTextc, ErrorText, sizeof (tsp00_ErrText) );
  
  return ( Ok ) ;
}

