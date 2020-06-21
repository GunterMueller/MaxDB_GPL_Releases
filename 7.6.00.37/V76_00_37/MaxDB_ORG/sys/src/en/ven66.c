/*!
  @file           ven66.c
  @author         JoergM
  @brief          Kernel RunTime: Virtual File for Kernel
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

#define MOD__ "ven66.c:"
#include "gen00.h"
#include "heo00.h"
#include "gen500.h"
#include "gen66.h"
#include "heo46.h"
#include "hen40.h"

#define BLOCKSIZE PAGE_MXSP00

#ifdef DEBUG_RTE
#define            ERRTEXT(v,t)  { DBG1(((char*)t,(const char *)v))\
				 eo46CtoP(v,(char*)t,(tsp00_Int4)MAXERRORTEXT);}
#else   
#define            ERRTEXT(v,t)  { eo46CtoP(v,(char*)t,(tsp00_Int4)MAXERRORTEXT);}
#endif  

static char OPEN_ERRTEXT []  = "Open ERROR";
static char WRITE_ERRTEXT [] = "Writing ERROR";
static char CLOSE_ERRTEXT [] = "Close ERROR ";

/* exported functions */

/* imported functions */

/* local functions */

/* exported data */

/* imported data */

/* local data */

extern SAPDB_Bool RTE_CSetDatabasePathOwnership( SAPDB_Char const *volume );

/*==========================================================================*/

void	sql66kc_open ( char			  *filename ,
                       tsp00_Bool     no_magic_record,
                       tsp00_Int4		  *hostfileno ,
                       tsp00_VfReturn  *error ,
                       tsp00_ErrText    errtext )
{
  #undef  MF__
  #define MF__ MOD__"sql66kc_open"

    *hostfileno  = open ( filename , O_WRONLY | O_CREAT | O_TRUNC , 0660 );

    DBG3 ((MF__, "hostfileno = %d " , *hostfileno ));
    if ( *hostfileno < 0 )
    {
		*error = VF_NOTOK ;
		MSGD (( ERR_VFOPEN_ERR, filename , sqlerrs() ))
        ERRTEXT (errtext, OPEN_ERRTEXT );
		return ;
    }

    RTE_CSetDatabasePathOwnership(filename);

    /*
     *  Must not use 0 (zero).
     */
    if ( *hostfileno == 0 )
    {
		*hostfileno = dup ( 0 );
		if ( *hostfileno < 0 )
		{
			*error = VF_NOTOK ;
			MSGD (( ERR_VFOPEN_DUP_ERR, sqlerrs() ))
			(void) close ( 0 );
				ERRTEXT (errtext, OPEN_ERRTEXT );
			return ;
		}
		(void) close ( 0 );
    }

    if ( e66_lock_file ( *hostfileno ) < 0 )
    {
		*error = VF_NOTOK ;
		(void) close ( *hostfileno );
        ERRTEXT (errtext, OPEN_ERRTEXT );
        return ;
    }

    *error = VF_OK ;

    if ( !no_magic_record )
    {
        DBG3 ((MF__, "hostfileno = %d " , *hostfileno ));
        if ( e66_write_magic_record ( *hostfileno ) < 0 )
        {
	  	    *error = VF_NOTOK ;
		    (void) close ( *hostfileno );
            ERRTEXT (errtext, OPEN_ERRTEXT );
        }
    }

    DBG3 ((MF__, "hostfileno = %d " , *hostfileno ));
    return ;
}

/*==========================================================================*/

void sql66kc_append_open ( 
char			*filename ,
tsp00_Int4		*hostfileno ,
tsp00_VfReturn		*error ,
tsp00_ErrText             errtext )
{
    *hostfileno  = open ( filename , O_WRONLY | O_CREAT | O_APPEND , 0660 );
    if ( *hostfileno < 0 )
    {
		*error = VF_NOTOK ;
		MSGD (( ERR_VFOPEN_ERR, filename , sqlerrs() ))
        ERRTEXT (errtext, OPEN_ERRTEXT );
        return ;
    }

    /*
     *  Must not use 0 (zero).
     */
    if ( *hostfileno == 0 )
    {
		*hostfileno = dup ( 0 );
		if ( *hostfileno < 0 )
		{
			*error = VF_NOTOK ;
			MSGD (( ERR_VFOPEN_DUP_ERR, sqlerrs() ))
			(void) close ( 0 );
			ERRTEXT (errtext, OPEN_ERRTEXT );
			return ;
		}
		(void) close ( 0 );
    }

    /* do not lock the file anymore to allow access by dbmsrv http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1137869 */
    *error = VF_OK ;

    return ;
}

/*==========================================================================*/

/*ARGSUSED*/	
void sql66kc_close ( 
tsp00_Int4		hostfileno ,
tsp00_VfReturn		*error ,
tsp00_ErrText             errtext )
{
    if ( close ( hostfileno ) < 0 )
      *error = VF_NOTOK ;
    else
      *error = VF_OK ;

    return ;
}

/*==========================================================================*/


void sql66kc_write ( 
tsp00_Int4		hostfileno ,
char			*block ,
tsp00_Int4		block_len ,
tsp00_VfReturn		*error ,
tsp00_ErrText             errtext )
{
    int				rc ;

    rc = write ( hostfileno , block ,  (long) block_len );
    if ( rc < 0 )
    {
		MSGD (( ERR_VFWRITE_ERR, sqlerrs() ))
        ERRTEXT (errtext, WRITE_ERRTEXT );
        *error = VF_NOTOK ;
		return ;
    }

    if ( rc != block_len )
    {
		MSGD (( ERR_VFWRITE_EOF, rc ))
        ERRTEXT (errtext, WRITE_ERRTEXT );
        *error = VF_NOTOK ;
    }
    else
        *error = VF_OK ;

    return ;
}

/*==========================================================================*/

int e66_lock_file ( 
int			fd )
{
    int				rc ;
    struct flock		lockf ;


    lockf.l_type   = F_WRLCK ;
    lockf.l_whence = 0 ;
    lockf.l_start  = 0 ;
    lockf.l_len    = 0 ;
    lockf.l_pid    = -1 ;

    rc = fcntl ( fd , F_SETLK , &lockf );
    if ( rc < 0 )
    {
		MSGD (( ERR_VFOPEN_LCK_ERR, sqlerrs() )) ;
		return ( -1 );
    }

    return ( 0 );
}

/*==========================================================================*/

int e66_write_magic_record ( 
int			fd )
{
    int                         rc ;
    char                        buf [ BLOCKSIZE ];


    SAPDB_memset ( buf , 0 , BLOCKSIZE );
    buf[2] = (char) (BLOCKSIZE >> 8);
    buf[3] = (char) (BLOCKSIZE);

    rc = write ( fd , buf ,  (long) BLOCKSIZE );
    if ( rc < 0 )
    {
        MSGD (( ERR_VFOPEN_PAGE_0_WRITE, sqlerrs() ))
        return ( -1 );
    }
    if ( rc != BLOCKSIZE )
    {
        MSGD (( ERR_VFOPEN_PAGE_0_SIZE, rc ))
        return ( -1 );
    }

    return ( 0 );
}

/*==========================================================================*/
#ifdef	INGO_TEST
#define	pri	(void) printf
struct TASK_TYPE this_task , *curr_task = &this_task ;

int main ( )
{
    tsp00_VFilename		hostfile ;
    tsp00_Int4			hostfileno ;
    tsp00_VfReturn		error ;
    tsp00_ErrText			errtext ;
    tsp00_Buf			block ;
    tsp00_Bool			ok ;

    vfinit ( &ok );
    pri ( "INGO: vfinit ok: %d \n", (int) ok );

    eo46CtoP( hostfile , "INGOOUT" , sizeof(tsp00_VFilename) );
    vfopen ( hostfile , &hostfileno , &error , errtext );
    pri ( "INGO: vfopen rc: %d \n", (int) error );
    pri ( "INGO: hostfilno: %d \n", (int) hostfileno );
    pri ( "INGO: errtext: %.40s \n", errtext );

    SAPDB_memset ( block , 'A' , sizeof(tsp00_Buf) );
    vfwrite ( hostfileno , block , &error , errtext );
    pri ( "INGO: vfwrite rc: %d \n", (int) error );
    pri ( "INGO: errtext: %.40s \n", errtext );

    vfclose ( hostfileno , &error , errtext );
    pri ( "INGO: vfclose rc: %d \n", (int) error );
    pri ( "INGO: errtext: %.40s \n", errtext );

    return ( 0 );
}
#endif	/*INGO_TEST*/
