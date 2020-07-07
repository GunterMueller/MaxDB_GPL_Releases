/*!
  @file           ven542.c
  @author         JoergM
  @brief          Kernel RunTime: Asynchronous Disk I/O
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

#define MOD__ "ven542.c:"

#include "gen00.h"
#include "heo00.h"
#include "hen40.h"
#include "geo50_0.h"
#include "geo54.h"
#include "gen500.h"
#include "gen54.h"
#include "RunTime/Tasking/RTETask_LegacyTaskCtrl.h"

/* exported functions */

int     e54a_install_aio_signal_handler() ;

int     e54_asyn_vblockio         ( struct DOUBLY_LINKED  * request,
                                           tsp00_ErrText             errtext) ;

int     e54_asyn_vasynio          ( struct DOUBLY_LINKED  * request,
                                           tsp00_ErrText             errtext) ;

int     e54_asyn_vasynio_result   ( struct DOUBLY_LINKED  * request,
                                           tsp00_ErrText             errtext) ;


/**************************************************************************/
#    if     SUN_SOL6 || OSF1
/**************************************************************************/

void	e54a_aio_catch_signal ( int sig,
                                siginfo_t *info ,
                                void      *ignore ) 
{
}

/**************************************************************************/
#    endif
/**************************************************************************/

/*
 * ===========================================================================
 */
int  e54a_get_aio_struct_size ()
{
    return ( 0 ) ;
}

/*
 * ===========================================================================
 */

void  e54a_init_sys_aio_struc () 
{
}

/*
 * ===========================================================================
 */
int e54a_install_aio_signal_handler ()
{
    return ( 1 ) ;
}

/*
 * ===========================================================================
 */
int e54_asyn_vblockio  ( struct DOUBLY_LINKED  * request,
                                 tsp00_ErrText             errtext)
{
    return ( 0 );
}
  

/*
 * ===========================================================================
 */
int e54_asyn_vasynio  ( struct DOUBLY_LINKED  * request,
                                tsp00_ErrText             errtext)
{
    return ( 0 );
}
  
/*
 * ===========================================================================
 */
int e54_asyn_vasynio_result  ( struct DOUBLY_LINKED  * request,
                                       tsp00_ErrText             errtext)
{
    return ( 0 );
}
