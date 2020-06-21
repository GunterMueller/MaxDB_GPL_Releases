/*!
  @file           ven930.c
  @author         JoergM
  @brief          Client RunTime: xclear - Clear a Kernel
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

#define MOD__ "ven930.c:"

#include "gen00.h"
#include "gen50.h"
#include "geo007_1.h"
#include "heo07.h"
#include "hsp100.h"
#include "hen40.h"

/* imported functions */

extern  key_t                           sql41_get_server_key ();
extern  int                             sql34_map_kgs ();
extern  void                            sql45_killall ();
extern	void				sqlinit ();

/* imported data */

/* local functions */

static  int                             kill_kernel ( char *dbname );

/* exported data */

#ifndef HP9
struct KERNEL_GLOBAL_SECTION          * kgs ;
#endif

/*==========================================================================*/

int c_main ( int argc, char *argv[] )
{
int                             rc ;
char                          * dbname ;

	sqlinit ( "clear_kernel" , (char*)0 );
	DBG1 (( MF__,"CLEAR_KERNEL: called \n" ));
	/*
	 *  File permission mode is always specified absolutely
	 */
        /* PTS 1105151 */
	umask ( 007 );

	if ( argc != 2 )
    {
		(void) fprintf ( stderr, "USAGE: clear dbname \n", 0 );
		return ( 1 );
    }

	dbname = argv [ 1 ] ;

	rc = kill_kernel ( dbname );

	DBG1 (( MF__,"CLEAR_KERNEL: returning %d \n", rc ));
	return ( rc );
}


/*==========================================================================*/


static  int     kill_kernel ( char *dbname )
{
int                             shmid , rc ;
key_t                           key ;


/*
 *  Compute the key for ipc resources from the database name
 */
	key = sql41_get_server_key ( dbname );
	if ( key == (key_t) -1 )
	{
		DBG1 (( -11987, "CLEAR_KERNEL: Database '%s' is not running \n", dbname ))
		return ( 2 );
	}
	if ( key == (key_t) -2 )
	{
		DBG1 (( MF__,"CLEAR_KERNEL: getserverid error, dbname '%s' \n", dbname ))
		return ( 2 );
	}
	DBG1 (( MF__,"CLEAR_KERNEL: key 0x%08x \n", key ))

/*
 *  Get the shmid of the Kernel Shared Memory Segment
 *  and test read/write accessibility
 */
	shmid = shmget ( key , 0 , 0600 );
	if ( shmid < 0 )
    {
		if ( errno == ENOENT )
		{
			DBG1 (( MF__,"Kernel Shared Memory Segment not found \n" ))
			DBG1 (( MF__,"Database not running '%s' \n", dbname ))
			return ( 0 );
		}
		if ( errno == EPERM || errno == EACCES )
		{
			MSGCD (( -11110,ERR_TYPE,"SECURITY","Cannot attach to Kernel Shared Memory Segment: Permission denied\n" ));
			return ( 2 );
		}
		MSGCD (( -11201,ERR_TYPE,"        ","Kernel Shared Memory Segment shmget error, %s\n", sqlerrs() ));
		return ( 2 );
    }
	DBG1 (( MF__,"CLEAR_KERNEL: shmid %d \n", shmid ))

	rc = sql34_map_kgs ( shmid , 0 );
	if ( rc )
    {
		return ( 2 );
    }

    if ( IsEqual_esp100 != sp100_CompareVersionIDs ( &kgs->RTEVersionID ) )
	{
		tsp00_Versionc currentVersion;
		
		sp100_GetVersionString( COMP_NAME_RTE_SP100,
								s100buildnumber,
								(char *)&currentVersion);

		MSGALL (( -11112,ERR_TYPE,"VERSION ","Incompatible version of running kernel and clear!\n" ))
		MSGALL (( -11112,ERR_TYPE,"VERSION ","Running kernel-version is: %s\n", kgs->RTEVersion ))
		MSGALL (( -11112,ERR_TYPE,"VERSION ","Actual   clear-version is: %s\n", currentVersion ))
		return ( 2 );
	}

	DBG1 (( MF__,"CLEAR_KERNEL: kgs 0x%08x \n", kgs ))
	kgs->state = SERVER_KILL ;
	sql45_killall ( kgs );

	return ( 0 );
}
