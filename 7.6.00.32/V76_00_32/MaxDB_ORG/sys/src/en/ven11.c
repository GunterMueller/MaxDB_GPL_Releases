/*!
  @file           ven11.c
  @author         JoergM
  @brief          Client RunTime: param_functions / XParam Parameter File Functions
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

#define MOD__ "ven11.c:"

#include "gen00.h"
#include "heo01.h"
#include "heo11.h"
#include "heo46.h"
#include "geo007_1.h"
#include "geo001.h"
#include "geo007_2.h"
#include "geo57.h"
#include "hen40.h"
#include "RunTime/System/RTESys_Time.h"

/* ***** externC vars ****************************************************** */

#define         ACT                              0
#define         NEXT                             1

#define         XP_PAGE_SIZE ( sizeof( XP_KEY_TYPE ) + sizeof( XP_VALUE_REC ) ) 
#define         NO_BUFFERING   0
#define         FULL_BUFFERING 1
#define         NUM_START_PARAMS 200
#define         NUM_NEW_PARAMS    50

/*! opening a parameter fail may file when another process is currently accessing the file */
/*! If this case is detected, just sleep for a given interval and try again */
#define NUMBER_OF_ATTEMPTS_ON_OPEN_ 600
/*! the sleep interval between open attempts in 100 milliseconds  */
#define WAIT_INTERVAL_ON_OPEN_      5

/* ***** macro definitions ************************************************ */

#define SEEK_BEG_OF_FILE(_fd, _ret, _errtext) \
          if ( xp_data [ _fd ] . params ) xp_data [ _fd ] . act_param = 0 ; \
          else \
           if ( lseek ( xp_data [ _fd ] . xp_stream , 0 , SEEK_SET ) < 0 ) \
             { *_ret = XP_NOT_OK ; \
		ERRTEXT ( _errtext , CANNOT_FIND_START_POINT ) ; \
		return ; \
	     }

#define ERRTEXT(v,t)    { eo46CtoP(v,t,(tsp00_Int4)MAXERRORTEXT); }
#define ONLY_BLANKS( string ) ( strlen ( string ) == strspn ( string," " ) )

/* ***** error text ******************************************************* */

static char CANNOT_OPEN_XPARAM_FILE [] = "cannot open xparam file" ;
static char NO_XPARAM_FILE [] =          "cannot find xparam file" ;

static char BLANK_XP_KEY []            = "blank keys not allowed" ;
static char KEY_NOT_FOUND []           = "key not found" ;
static char CANNOT_WRITE_XP_KEY []     = "cannot write key" ;
static char CANNOT_WRITE_XP_VALUE []   = "cannot write value" ;
static char WRITING_ERROR []           = "some writing error" ;
static char CANNOT_READ_XP_KEY []      = "cannot read key" ;
static char CANNOT_READ_XP_VALUE []    = "cannot read value" ;
static char READING_ERROR []           = "some reading error" ;
static char CANNOT_FIND_START_POINT [] = "cannot find start point" ;
static char CANNOT_FIND_PREV_ENTRY []  = "cannot find previous entry" ;
static char CANNOT_FIND_ACT_ENTRY []   = "cannot find actual entry" ;
static char EOF_REACHED []             = "end of file reached" ;
static char NO_MORE_MEMORY[]           = "not enough memory" ;
static char PARAMETER_ALREADY_IN_USE[] = "Parameter already in use" ;

#define MAX_OPEN_PARAMS  3
#define CACHE_PER_STREAM 20000

typedef struct  xp_data_struc
  { 
    tsp00_C132 xp_file ;
    int   xp_stream ;
    char *params ;
    ULONG ulOpenMode ; /* PTS 1104765 */
    LONG act_param ;
    LONG last_param ;
    LONG max_params ;
    char *RefXparams ;
    ULONG ulRefSize ;
    tsp00_Bool file_is_new ; 
  } XP_DATA_STRUC ;

static XP_DATA_STRUC  xp_data [ MAX_OPEN_PARAMS ] = { 0 }; 
/* ***** externC functions ************************************************* */

externC	void			sql11c_get_xparam_buffer ();
externC  void            sql11c_SeqBinReadXParam ();
externC  void            sql11c_SeqBinWriteXParam ();

externC	int     sql11c_close_xparam_file ( tsp00_Int4 xp_fd , 
											tsp00_Bool flush_buffer );
externC	void 	        sql11c_FsizeXparam();

/* ***** external procedures/functions ************************************ */

/* ***** forward declarations ********************************************* */

externC int			    sql11c_open_xparam_file ( tsp00_Pathc xp_file, 
                                                  tsp00_Int4    OpenMode );
/* ************************************************************************ */

/*==========================================================================*/

void    sql11c_OpenXParam   ( tsp00_Pathc  xp_file,
                              tsp00_Int4    *lXParamFd,
                              tsp00_ErrText  acErrorText,
                              tsp00_Int4     OpenMode,
                              tsp00_Bool   *ok   )
{

	* lXParamFd = sql11c_open_xparam_file ( xp_file, OpenMode ) ;

	* ok = * lXParamFd >= 0 ;

	if ( ! *ok )
	{
		/* ***** error ***** */
		if ( errno == ENOENT )
		{
			ERRTEXT ( acErrorText , NO_XPARAM_FILE )
		}
		else
		{
			ERRTEXT ( acErrorText , CANNOT_OPEN_XPARAM_FILE )
			MSGD (( ERR_CANNOT_OPEN_XPARAM_FILE, xp_file ));
		}
	}
}

/*==========================================================================*/
void    sql11c_CloseXParam  (   tsp00_Int4    lXParamFd,
                                tsp00_Bool     fFlush,
                                tsp00_ErrText acErrorText,
                                tsp00_Bool     *ok        )
{
    SAPDB_memset (( char* ) acErrorText, ' ', sizeof ( tsp00_ErrText ));

	*ok = sql11c_close_xparam_file ( lXParamFd , fFlush ) == 0 ;
}

/*==========================================================================*/

externC void    sql11c_get_xparam_buffer ( tsp00_Int4   xp_fd , 
                                          char       **xp_buf, 
                                          tsp00_Int4   *xp_bufsiz)
{
  if ( xp_fd >= 0 && xp_fd < MAX_OPEN_PARAMS )
    { *xp_buf    = xp_data [ xp_fd ] . params ;
      *xp_bufsiz = xp_data [ xp_fd ] . last_param ;
    }
  else
    { *xp_buf    = NULL ;
      *xp_bufsiz = 0 ;
    }
  return ;
}

/*==========================================================================*/

#undef MF__
#define MF__ MOD__"sql11c_GetFreeXparamSlot"
static int     sql11c_GetFreeXparamSlot ()
{
  int xp_fd ;
  for ( xp_fd = 0; xp_fd < MAX_OPEN_PARAMS &&
                   xp_data[xp_fd].xp_stream > 0; xp_fd++ ) ; 

 if ( xp_data [ xp_fd ]. xp_stream > 0 ) 
   { MSGD (( -11113, ERR_TYPE, "XPARAM  ", "too many open paramfiles" ));
     xp_fd = -1 ;
   }
  else
    DBG1 (( MF__,"free slot: %d\n", xp_fd ))

  return ( xp_fd ) ;
}

/*==========================================================================*/
#undef MF__
#define MF__ MOD__"sql11c_open_xparam_file"
externC int     sql11c_open_xparam_file ( 
tsp00_Pathc     xp_file,
tsp00_Int4        OpenMode )
{
	tsp00_Bool         file_exists ;
	tsp00_Int4        xp_fd ;
        struct stat     stat_buf ;

    SAPDB_UInt attempts;
    SAPDB_Bool dummyFlag = false;

	DBGIN;

	/* ***** look for xparam file ***** */

	if ( stat ( xp_file , &stat_buf ) < 0 )
	{
		if ( errno != 2 )
		       MSGD (( -11700 , ERR_TYPE, "XPARAM  ", "stat error, %s\n",
								 sqlerrs() ))
		DBG1 (( MF__,"no xparam file \n" ))

		file_exists = FALSE ;
	}
	else
	{
		DBG1 (( MF__,"xparam file found \n" ))

                if ( (OpenMode & XP_OPEN_CREATE_SP11) && (OpenMode & XP_OPEN_BIN_SP11) )
                  { 
                    /* sqlextract_xparamfile should          */
                    /* overwrite / truncate an existing file */
                    unlink ( xp_file ) ;
                    file_exists = FALSE ;
                  }
                else
                  file_exists = TRUE ;
	}


	/* ***** open xparam file ***** */

          if ( (xp_fd = sql11c_GetFreeXparamSlot() ) < 0 )
    	    return ( - 1 ) ;

          xp_data[xp_fd].ulOpenMode = OpenMode ;
          xp_data[xp_fd].act_param  = 0 ;
          xp_data[xp_fd].last_param  = 0 ;
          xp_data[xp_fd].file_is_new  = !file_exists ;
          strcpy ( (char *)xp_data[xp_fd].xp_file, xp_file ) ;

	  if ( !file_exists )
            if ( OpenMode & XP_OPEN_CREATE_SP11 )
              { int fd = open ( xp_file, O_RDWR | O_CREAT, 0660 ) ;
                if ( fd != -1 ) 
                  { close ( fd ) ;
                    DBG1 (( MF__,"sql11c_open_xp_f: dummyfile %s created\n", xp_file))
                  }
              }
            else
              { MSGD (( ERR_NO_XPARAM_FILE )) ;
    	        return ( - 1 ) ;
              }

      for(attempts=0;attempts < NUMBER_OF_ATTEMPTS_ON_OPEN_;attempts++)   
      {
        xp_data[xp_fd].xp_stream = open ( xp_file , O_RDWR ) ;
        /* If (and only if!) the file is open by another process, wait a little while and try again. */
        if( EACCES == errno )
        {
            RTESys_Timeout(&dummyFlag, WAIT_INTERVAL_ON_OPEN_+1);
        }
        else
        {
            break;
        }
      }
	  if ( xp_data[xp_fd].xp_stream < 0 )
	    {
	      MSGD (( -11113, ERR_TYPE, "XPARAM  ", "open error (creat) %s\n", sqlerrs() ))
	      return ( - 1 ) ;
	    }

      xp_data[xp_fd].max_params = 0 ;
      xp_data[xp_fd].params = ( char *) NULL ;

      return ( xp_fd ) ;
}

/*==========================================================================*/
#define RESET_PARAM_CHANGED_FLAG 1

/*------------------------------*/

#undef MF__
#define MF__ MOD__"sql11c_close_xparam_file"
externC	int     sql11c_close_xparam_file ( 
tsp00_Int4                xp_fd ,
tsp00_Bool                 flush_buffer )
{
	DBGIN;

	/* ***** valid xuser file descriptor ? ***** */

	if ( xp_fd > -1 && xp_fd < MAX_OPEN_PARAMS )
	{
      DBG1 (( MF__,"flush_buffer is %s \n", flush_buffer ? "TRUE" : "FALSE")) ;
      /* the code that was executed if flush_buffer=true operated on the contents of the xparam file. */
      /* Because these have changed and the function is currently never called with flush_buffer=true, this case is forbidden */
      if(flush_buffer)  
        MSGD (( -11114, ERR_TYPE, "XPARAM  ", "flush_buffer = true no longer supported for sql11c_close_xparam_file()")) ;
		/* ***** close XPARAN file ***** */
                
  	  (void) close ( xp_data [ xp_fd ]. xp_stream ) ;
      if ( xp_data [ xp_fd ] . last_param <= 1 &&
           xp_data [ xp_fd ] . file_is_new ) 
        (void) unlink ( (char *)xp_data [ xp_fd ] . xp_file ) ;

      if ( xp_data [ xp_fd ] . params != ( char *) NULL ) 
        FREE_MEM_EO57 ( xp_data [ xp_fd ] . params ) ; 
      if ( xp_data [ xp_fd ] . RefXparams != ( char *) NULL ) 
        FREE_MEM_EO57 ( xp_data [ xp_fd ] . RefXparams ) ; 

      SAPDB_memset ( & xp_data [ xp_fd ], 0, sizeof ( XP_DATA_STRUC ) ) ;
	  return ( 0 ) ;
	}
	else
	{
      /* ***** no valid xparam file descriptor ***** */
       DBG1 (( MF__,"invalid fd: %d\n", xp_fd ))
	   return ( - 1 ) ;
	}
}

/*==========================================================================*/

externC void    sql11c_SeqBinReadXParam ( 
tsp00_Int4        xparam_fd ,
char *          buf ,
tsp00_Int4        bytes_to_read ,
tsp00_Int4        *bytes_read ,
tsp00_ErrText       errtext ,
tsp00_Uint1        * xp_ret )

{
  *bytes_read = read ( xp_data[ xparam_fd ]. xp_stream
                     , buf
                     , bytes_to_read ) ;

  if ( *bytes_read < 0 )
  {   * xp_ret = XP_NOT_OK ;
      ERRTEXT ( errtext , READING_ERROR )
      MSGD (( ERR_READ_ERROR )) ;
  }
  else if ( *bytes_read < bytes_to_read )
  {
      * xp_ret = XP_EOF ;
	  ERRTEXT ( errtext , EOF_REACHED )
  }
  else
    * xp_ret = XP_OK ;

  return  ;
}


/*==========================================================================*/

externC void    sql11c_SeqBinWriteXParam ( 
tsp00_Int4        xparam_fd ,
char            *buf ,
tsp00_Int4        bytes_to_write ,
tsp00_Int4        *bytes_written ,
tsp00_ErrText     errtext ,
tsp00_Uint1        * xp_ret )
{
  *bytes_written = write ( xp_data[ xparam_fd ]. xp_stream , buf, bytes_to_write );

  if ( *bytes_written != bytes_to_write )
  {
      * xp_ret = XP_NOT_OK ;
      ERRTEXT ( errtext , WRITING_ERROR )
      MSGD (( ERR_WRITE_ERROR )) ;
  }
  else
  {
      xp_data[xparam_fd].act_param = bytes_to_write / XP_PAGE_SIZE ;
      xp_data[xparam_fd].last_param += xp_data[xparam_fd].act_param;
      * xp_ret = XP_OK ;
  }

  return  ;
}
