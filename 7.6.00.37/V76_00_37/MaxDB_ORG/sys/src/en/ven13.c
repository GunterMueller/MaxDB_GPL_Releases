/*!
  @file           ven13.c
  @author         JoergM
  @brief          Client RunTime: Sqluser Functions
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


#define MOD__ "ven13.c:"

#include "gen00.h"
#include "gen13.h"
#include "heo46.h"
#include "geo007_1.h"
#include "geo001.h"
#include "geo57.h"
#include "hen40.h"

/* ***** external procedures/functions ************************************ */

        int             sql01c_get_serverdb();
extern  int             sql01c_get_dblang();

extern char *           getenv () ;

/* ***** macro definitions ************************************************ */

#define ERRTEXT(v,t)   { eo46CtoP(v,t,MAXERRORTEXT); }

#define XUSER_62 ".XUSER.62"
#define XUSER_BEF_62 ".XUSER"
#define MAX_PATH_LEN 256
static char CANNOT_READ_XU_ENTRY []     = "cannot read xuser entry" ;
static char CANNOT_OPEN_XUSER_FILE []   = "cannot open xuser file" ;

static teo00_Bool fConvertOldXUser = TRUE ;

static teo00_Bool check_xuser_file_exists ( char *xu_file, 
                                         char *full_xu_path );
/*==========================================================================*/

tsp00_Int4    sql13u_remove_xuser_entries (SAPDB_Char *   accountName, /* not used for UNIX */
                                           tsp00_ErrText errtext )
{
    char            path_name [ MAX_PATH_LEN ] ;

    DBG1 (( MF__,"********** sqlclearuser called ***********************\n" ))

    /* ***** look for ".XUSER" file in $HOME directory ***** */

    fConvertOldXUser = FALSE ;
    if ( !check_xuser_file_exists ( XUSER_62, path_name ) )
    {
	    DBG1 (( MF__,"no .XUSER file to delete       \n" ))
	    return ( TRUE ) ;
    }


    /* ***** delete ".XUSER" file ***** */

    if ( unlink ( path_name ) == - 1 )
    {
        MSGD(( ERR_DELETE_XUSER_FILE__SYSMSG, sqlerrs() ));
	return  ( FALSE ) ;
    }

    return ( TRUE ) ;
}

/*==========================================================================*/

int sql13c_get_serverdb ( char **serverdb )
{
extern	char		*sql01_dbname ;
	char            * env_ptr ;
        int             dbname_found ;

	DBG1 (( MF__,"********** sql13c_get_serverdb called ****************\n" ))

	env_ptr = sql01_dbname ; 
	if ( ( dbname_found = env_ptr != NULL && 
               !sql13c_only_blanks ( env_ptr )) )
	{
          *serverdb = env_ptr ;
	  DBG1 (( MF__,"13c_get_serverdb: serverdb = %s\n", *serverdb ))
	}
        return ( dbname_found ) ;
}

/*==========================================================================*/

int     sql13c_get_dblang ( char **dblang )

{
extern	char	*sql01_dblang ;
        char    *env_dblang = sql01_dblang ;
        int     dblang_found ;
    
	if ( (dblang_found = env_dblang != NULL && 
             !sql13c_only_blanks ( env_dblang )))
	{
          *dblang = env_dblang ;
	   DBG1 (( MF__,"13c_get_dblang: dblang = %s\n", *dblang ))
	}

        return ( dblang_found ) ;
}

/*==========================================================================*/

teo00_Bool   sql13c_only_blanks ( 
char                    * str )
{
	int             str_len ;
	int             initial_len ;

	DBG1 (( MF__,"********** sql13c_only_blanks called *************\n" ))

	str_len = strlen ( str ) ;
	initial_len = strspn ( str , " " ) ;

	DBG1 (( MF__,"only_blanks     : str_len                      = %d\n",
				  str_len ))
	DBG1 (( MF__,"only_blanks     : initial_len                  = %d\n",
				  initial_len ))
	DBG1 (( MF__,"only_blanks     : string                       = %s\n",
				  str ))

	if ( str_len == initial_len )
		return ( TRUE ) ;
	else
		return ( FALSE ) ;
}

/*==========================================================================*/

tsp00_Int4    sql13u_get_all_entries ( SAPDB_Char *             accountName, /* not used for UNIX */
                                       char                     **pcDataBuffer,
                                       tsp00_Int4               *lDataLen,
                                       PREL611_XUSER_PAGE_UNION *pRel611XUserPages, 
                                       tsp00_ErrText              errtext )
{

    char                path_name [ MAX_PATH_LEN ] ;
    char                *Buf ;
    int                 byte_cnt ;
    teo00_Bool          fIsOldXuser = FALSE ;
    teo00_Bool          file_exists, ok = TRUE ;
    int                 xu_fd, real_len = 0 ;
    struct stat         buf ;

    DBG1 (( MF__,"********** sql13c_open_xuser_file called *************\n" ))

    /* ***** initialize vars ***** */
    *lDataLen = 0 ;
    *pcDataBuffer = NULL ;
    *pRel611XUserPages = ( PREL611_XUSER_PAGE_UNION ) NULL ;

    if ( ! (file_exists = check_xuser_file_exists ( XUSER_62, path_name )) ) 
    {
      if ( fConvertOldXUser &&
          (file_exists = check_xuser_file_exists ( XUSER_BEF_62, path_name )) )
        fIsOldXuser = TRUE ;
      else
        { DBG1 (( MF__,"no .XUSER file \n"  )) ;
          return ( ERROR_FILE_NOT_FOUND ) ; 
        }
    }
    /* ***** get size informations about .XUSER file ***** */

    if ( stat ( path_name , & buf ) < 0 )
    {
        ok = FALSE ;
        MSGD (( ERR_CANNOT_STAT_XUSER_FILE__SYSMSG, sqlerrs() ))
    }
    else
    {
        /* ***** .XUSER file must be a regular file ***** */
    
        if ( ( ( buf . st_mode ) & S_IFREG ) == S_IFREG )
        {
          real_len = buf . st_size ;
    	  DBG1 (( MF__,"real_len = %d, fIsOldXuser = %d\n",
    				    real_len, fIsOldXuser )) ;
        }
        else
        {
    	  ok = FALSE ;
          MSGD (( ERR_XUSER_WRONG_FILETYPE, path_name ));
	}
    }

    /* ***** if file exists take over the ***** */
    /* ***** content from .XUSER file     ***** */

    if ( ok )
    {
	if ( ( xu_fd = open ( path_name , O_RDWR , 0 ) ) < 0 )
	{
        MSGD (( ERR_CANNOT_OPEN_XUSER_FILE__SYSMSG, sqlerrs() ));
	    ERRTEXT ( errtext , CANNOT_OPEN_XUSER_FILE )
	    return ( - 1 ) ;
        }
	DBG1 (( MF__,"rd/wr  XUSER file, xu_fd  = %d\n", xu_fd ));
    if ( ALLOC_MEM_EO57( (void **)&Buf, real_len) != NO_ERROR_EO001 )
    {
	    ERRTEXT ( errtext , CANNOT_OPEN_XUSER_FILE )
        return ( - 1 );
    }
	byte_cnt = read ( xu_fd , Buf , real_len ) ;
        if ( fIsOldXuser )
          *pRel611XUserPages = ( PREL611_XUSER_PAGE_UNION ) Buf ;
        else
          *pcDataBuffer = Buf ;
        *lDataLen = (tsp00_Int4)byte_cnt ;
	(void) close ( xu_fd ) ;

	DBG1 (( MF__,"read   XUSER file, byte_cnt  = %d\n",
						     byte_cnt ));
	if ( byte_cnt < 0 )
	{
        MSGD (( ERR_CANNOT_READ_XUSER_FILE__SYSMSG, sqlerrs() ));
	    ERRTEXT ( errtext , CANNOT_READ_XU_ENTRY )
	    return ( - 1 ) ;
	}

	if ( byte_cnt != real_len )
	{
        MSGD (( ERR_WRONG_XUSER_FILE_LENGTH, byte_cnt ));
	    ERRTEXT ( errtext , CANNOT_READ_XU_ENTRY )
	    return ( - 1 ) ;
	}
    }

    return ( 0 ) ;
}

/*==========================================================================*/

tsp00_Int4    sql13u_write_xuser_entries ( SAPDB_Char *              accountName, /* not used for UNIX */
                                           char *                    XuserData,
                                           PXUSER_INFO_PAGE          pXUserInfoPage,
                                           tsp00_ErrText             errtext )
{
	int             cnt ;
	int             xuser_fd ;
        char            path_name [ MAX_PATH_LEN ], *env_ptr ;

	DBG1 (( MF__,"********** sql13c_close_xuser_file called ************\n" ))

	/* ***** valid xuser file descriptor ? ***** */


	if ( ( env_ptr = ( char * ) getenv ("HOME") ) == NULL )
		(void) strcpy ( path_name , XUSER_62 ) ;
	else
                sprintf ( path_name, "%s/%s", env_ptr , XUSER_62 ) ;

    if ( ( xuser_fd = open ( path_name , O_RDWR | O_CREAT | O_TRUNC ,
						 0600 ) ) < 0 )
    {
        MSGD (( ERR_CANNOT_OPEN_XUSER_FILE__SYSMSG, sqlerrs() ));
        return ( - 1 ) ;
    }

    DBG1 (( MF__,"create XUSER file, xuser_fd  = %d\n",
						     xuser_fd ))
	if ( xuser_fd > - 1 )
	{
		/* ***** point to the file's beginning ***** */
                teo00_Longint lDataLen = pXUserInfoPage->ulInfoPageSize;
                
                lDataLen += (pXUserInfoPage->ulPages * pXUserInfoPage->ulPageSize);

		/* ***** put all informations into .XUSER file ***** */

		cnt = write ( xuser_fd , XuserData, lDataLen ) ;

		if ( cnt < 0 )
		{
			MSGD (( ERR_CANNOT_WRITE_XUSER_FILE__SYSMSG, sqlerrs() ));
			(void) close ( xuser_fd ) ;
			return ( - 1 ) ;
		}

		if ( cnt != lDataLen )
		{
			MSGD (( ERR_WRONG_XUSER_FILE_LENGTH, cnt ));
			(void) close ( xuser_fd ) ;
			return ( - 1 ) ;
		}

		/* ***** close XUSER file ***** */

		(void) close ( xuser_fd ) ;
		return ( 0 ) ;
	}
}

/*==========================================================================*/

static teo00_Bool check_xuser_file_exists ( char *xu_file, 
                                            char *full_xu_path ) 
{

  int file_exists = 0;
  char *env_ptr ;

  /* PTS 1108544 use SAPDBINSTKEY if it exists */
  if ( ( env_ptr = ( char * ) getenv ("SAPDBINSTKEY") ) != NULL )
  {
     sprintf ( full_xu_path, "%s/config/XUSER", env_ptr ) ;
     file_exists = access ( full_xu_path, 0 ) == 0 ;
  }

  if ( !file_exists )
  {

    if ( ( env_ptr = ( char * ) getenv ("HOME") ) == NULL )
      (void) strcpy ( full_xu_path, xu_file ) ;
    else
       sprintf ( full_xu_path, "%s/%s", env_ptr , xu_file ) ;

    file_exists = access ( full_xu_path, 0 ) == 0 ;
  }

  return ( file_exists ) ;
}
