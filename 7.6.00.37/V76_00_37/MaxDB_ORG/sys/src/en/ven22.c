/*!
  @file           ven22.c
  @author         JoergM
  @brief          Client RunTime: Diagwrite for Applications
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

#define MOD__ "ven22.c:"

#include "gen00.h"
#include "vsp003c.h"
#include "hen40.h"
#include "gsp01.h"
#include "heo01.h"
#include "hsp77.h"
#include "heo06.h"
#include	<stdarg.h>
#include <time.h>
#include "RunTime/System/RTESys_Time.h"
#include "gen22.h"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageOutput.h"

/* exported functions */


/* imported functions */


/* local functions */

static  void    sql22_msg  ( int         prio ,
                             int         msgno ,
                             const char *msgtyp ,
                             const char *msg_label ,
                             const char *text ,
                             va_list     args );

static void en22_writeToDiagFile(char *diagline, int lgt);

void	eo60k_Message  ( tsp3_priority        msg_prio ,
                             tsp00_Int4             msg_type ,
                             tsp00_Int4             msg_no ,
                             const char          *msg_label ,
                             const char          *msg_txt );
/* exported data */

int             sqldbglvl = 0;
int             sql22_timestamp = TRUE ;
int				sql22_keep_open = 0;
char           *sql22_file = 0;
int				sql22_fd = -1 ;

int                             e60_dbgdbglvl = 0;
#define IS_DEBUGLEVEL_SET (e60_dbgdbglvl == 1)

/* imported data */

extern	char				*sql01_username ;

/* local data */

static  tsp00_Pathc     sql22_local_file = { 0 };

/*
 * ==================================================================
 */

#undef MF__
#define MF__ MOD__"sqlrtemsg6"
/**VARARGS2*/
/*PRINTFLIKE2*/
void sqlrtemsg6 ( int  msgno , 
                  const char *text , 
                  ... )
{
va_list                         args ;
	
	va_start ( args , text );
	sql22_msg ( 6 , msgno , "   " , "", text , args );
	va_end ( args );
}

/*
 * ===========================================================================
 */

#undef MF__
#define MF__ MOD__"sqlrtemsg7"
/**VARARGS2*/
/*PRINTFLIKE2*/
void    sqlrtemsg7 ( int  msgno , 
                     const char *text , 
                     ... )
{
va_list                         args ;	

	va_start ( args , text );
	sql22_msg ( 7 , msgno , "   " , "", text , args );
	va_end ( args );
}

/*
 * ===========================================================================
 */

#undef MF__
#define MF__ MOD__"sqlrtemsg8"
/**VARARGS2*/
/*PRINTFLIKE2*/
void    sqlrtemsg8 ( int  msgno , 
                     const char *text , 
                     ... )
{
va_list                         args ;
	
	va_start ( args , text );
	sql22_msg ( 8 , msgno , "   " , "", text , args );
	va_end ( args );
}

/*
 * ===========================================================================
 */
#undef MF__
#define MF__ MOD__"sqlrtemsg_8"
void    sqlrtemsg_8 ( int  msgno , 
                      const char *msg_type,
                      const char *msg_label,
                      const char *text , 
                      ... )
{
va_list                         args ;
	
	va_start ( args , text );
	sql22_msg ( 8 , msgno , msg_type, msg_label, text , args );
	va_end ( args );
}

/*
 * ===========================================================================
 */

#undef MF__
#define MF__ MOD__"sqlrtemsg9"
/**VARARGS2*/
/*PRINTFLIKE2*/
void    sqlrtemsg9 ( int  msgno , 
                     const char *text , 
                     ... )
{
va_list                         args ;
	
	if ( sqldbglvl < 1 ) return ;

	va_start ( args , text );
	sql22_msg ( 9 , msgno , "   " , "", text , args );
	va_end ( args );
}


/*
 * ===========================================================================
 */
/* PTS 1106789 */
static void sql22_SetApplDiagFileName ()
{
    if ( ! sql22_file )
    {
		int rc; 
		sql22_file = getenv ( "DBAPPLDIAG" );
		if ( ! sql22_file ) sql22_file = getenv ( "SQLADIAG" );

		if ( !sql22_file && sql01_username )
		{
		  tsp01_RteError  RteError ;
		  if ( sqlGetIndependentWrkPath ( sql22_local_file, TERM_WITH_DELIMITER_EO01, &RteError ) )
		  {
			 sql22_file = sql22_local_file;
			 strcat( sql22_local_file, sql01_username ) ;

			 if ( !sqlcreate_dirc ( sql22_file, &RteError ) )
			 {
				 int fd;
				 /* there is no chance to be sure we have any output channel.... */
				 fd = open("/dev/tty", O_RDONLY);
				 if ( fd > 0 )
				 {
					 char  diagline [ 1024 ];
					(void) sp77sprintf ( diagline , sizeof(diagline),
					"sql22_msg: '%s': %s\r\n",    RteError.RteErrText, RteError.OsErrText );
					(void) write ( fd , diagline , (unsigned) strlen(diagline) );
					close(fd);
				 }
				sql22_file = 0 ;
			 }
	  		 else
			 {
				(void) strcat ( sql22_file , "/appldiag" );
			 }
		  }
		}
    }
}

/*
 * ===========================================================================
 */
#undef MF__
#define MF__ MOD__"sql22_msg"
static  void    sql22_msg ( int         prio ,
                            int         msgno ,
                            const char *msgtyp ,
                            const char *msg_label ,
                            const char *text ,
                            va_list     args )

{
    int  errfd , lgt , lgtpref , rc ;
    char diagline [ 1024 ];

    sql22_SetApplDiagFileName();

    if ( sql22_timestamp && sql22_file )
    {
        RTESys_Timestamp timeBuffer;
  /* PTS 1109704 */
    	(void) sp77sprintf ( diagline, sizeof(diagline),
			    "%s %5d %3.3s %-6d %-8.8s",
                 RTESys_FillProtocolTimeStamp(timeBuffer),
			    (int)getpid() ,
                 msgtyp,
                 msgno,
                 msg_label );
    	lgtpref = strlen ( diagline );
    }
    else
    {
		lgtpref = 0 ;
    }

#ifdef USE_SQLDBGLBL
    if ( sqldbglbl )
    {
		(void) sp77sprintf ( diagline + lgtpref , sizeof(diagline)-lgtpref, "%s: " , sqldbglbl );
		lgt = strlen ( diagline );
    }
    else
#endif
    {
		lgt = lgtpref ;
    }

    /*
     *  Append text
     */
    (void) sp77vsprintf ( diagline + lgt , sizeof(diagline) - lgt, text , args );
    lgt = strlen ( diagline );
    if ( diagline [lgt-1] != '\n' )
      {
        diagline[lgt++] = '\n' ;
        diagline[lgt]   = '\0' ;
      }

	/*
     *  Write to diag file
     *  If diag file cannot be opened for writing, write to stderr
     */
	en22_writeToDiagFile(diagline, lgt);
}
/*
 * ===========================================================================
 */

#undef MF__
#define MF__ MOD__"sql22_n_msg"
static  void    sql22_n_msg ( 
const char        * ModuleInfo ,
const char        * text ,
va_list				args )
{
    int  errfd , lgt , lgtpref , rc ;
    char diagline [ 1024 ];

    sql22_SetApplDiagFileName();

    if ( sql22_timestamp && sql22_file )
    {
        RTESys_Timestamp timeBuffer;
      /* PTS 1109704 */
    	(void) sp77sprintf ( diagline , sizeof(diagline),
			    "%s %5d %-8.8s",
                 RTESys_FillProtocolTimeStamp(timeBuffer),
  			   (int) getpid() , 
               ModuleInfo );
    	lgtpref = strlen ( diagline );
    }
    else
    {
		lgtpref = 0 ;
    }

    /*
     *  Append text
     */
    (void) sp77vsprintf ( diagline + lgtpref , sizeof(diagline) - lgtpref, text , args );
    if ( ModuleInfo )
      strcat ( diagline , "\n" ) ;
    lgt = strlen ( diagline );

    /*
     *  Write to diag file
     *  If diag file cannot be opened for writing, write to stderr
     */
	en22_writeToDiagFile(diagline, lgt);
}

/*
 * ===========================================================================
 */

/**VARARGS1*/
/*PRINTFLIKE1*/
#undef MF__
#define MF__ MOD__"sqldbgwrite"
void    sqldbgwrite ( const char *text , ... )
{
va_list                         args ;

	va_start ( args , text );
	sql22_msg ( 8 , 11999 , "   " , "", text , args );
	va_end ( args );
}

/*
 * ===========================================================================
 */

/**VARARGS1*/
/*PRINTFLIKE1*/
#undef MF__
#define MF__ MOD__"sqlerrwrite"
void    sqlerrwrite ( const char *text , ... )
{
va_list                         args ;

	va_start ( args , text );
	sql22_msg ( 8 , 11999 , "ERR" , "", text , args );
	va_end ( args );
}

/*
 * ===========================================================================
 */

/**VARARGS1*/
/*PRINTFLIKE1*/
#undef MF__
#define MF__ MOD__"sqlwngwrite"
void    sqlwngwrite ( const char *text , ... )
{
va_list                         args ;

	va_start ( args , text );
	sql22_msg ( 8 , 11999 , "WNG" , "", text , args );
	va_end ( args );
}

/*
 * ===========================================================================
 */

#undef MF__
#define MF__ MOD__"sqldbg_modinfo_write"
void    sqldbg_modinfo_write ( const char *ModuleInfo, const char *text , ... )
{
va_list                         args ;

	va_start ( args , text );
	sql22_n_msg ( ModuleInfo , text , args );
	va_end ( args );
}

/*
 * ==================================================================
 */

#undef MF__
#define MF__ MOD__"sqloutwrite"
void	sqloutwrite     ( tsp00_Int4             msg_no ,
                          tsp3_msg_type        msg_type ,
                          const char          *msg_label ,
                          const char          *msg_format ,
                          ...                           )
{
  char                            msg_txt [ 1024 ];
  va_list                         args ;
  static char	*msg_type_txt[] = { "INTERNAL ERROR", "ERROR", "WARNING", "INFO" };

  if ( msg_type <= ( sizeof ( msg_type_txt ) / sizeof ( char *) ) )
  {
      printf ( "%s ", msg_type_txt[msg_type] );
  }
  printf ( "%d: ", msg_no );

  va_start ( args, msg_format ) ;
  vprintf ( msg_format, args ) ;
  va_end ( args ) ;

  printf ( "\n" );
  fflush ( stdout );
}

/*
 * ===========================================================================
 */

void	sql60c_msg_6    ( int                 msg_no ,
                          int                 msg_type ,
                          const char         *msg_label ,
                          const char         *msg_format ,
                          ...                           )
{
#undef  MF__
#define MF__ MOD__"sql60c_msg_6"

  char                            msg_txt [ 1024 ];
  va_list                         args ;

  va_start ( args, msg_format ) ;
  sp77vsprintf ( msg_txt, sizeof(msg_txt), msg_format, args ) ;
  va_end ( args ) ;

  eo60k_Message( 6 , msg_type, msg_no, msg_label, msg_txt);
}

/*
 * ==================================================================
 */

void	sql60c_msg_7    ( int                 msg_no ,
                          int                 msg_type ,
                          const char         *msg_label ,
                          const char         *msg_format ,
                          ...                           )
{
#undef  MF__
#define MF__ MOD__"sql60c_msg_7"

  char                            msg_txt [ 1024 ];
  va_list                         args ;

  va_start ( args, msg_format ) ;
  sp77vsprintf ( msg_txt, sizeof(msg_txt), msg_format, args ) ;
  va_end ( args ) ;

  eo60k_Message( 7 , msg_type, msg_no, msg_label, msg_txt);
}

/*
 * ==================================================================
 */

void	sql60c_msg_8    ( int                 msg_no ,
                          int                 msg_type ,
                          const char         *msg_label ,
                          const char         *msg_format ,
                          ...                           )
{
#undef  MF__
#define MF__ MOD__"sql60c_msg_8"

  char                            msg_txt [ 1024 ];
  va_list                         args ;

  va_start ( args, msg_format ) ;
  sp77vsprintf ( msg_txt, sizeof(msg_txt), msg_format, args ) ;
  va_end ( args ) ;

  eo60k_Message(  8, msg_type, msg_no, msg_label, msg_txt);
}

/*
 * ==================================================================
 */

void	sql60c_msg_9    ( int                 msg_no ,
                          int                 msg_type ,
                          const char         *msg_label ,
                          const char         *msg_format ,
                          ...                           )
{
#undef  MF__
#define MF__ MOD__"sql60c_msg_9"

  char                            msg_txt [ 1024 ];
  va_list                         args ;

  if ( IS_DEBUGLEVEL_SET )
    { va_start ( args, msg_format ) ;
      sp77vsprintf ( msg_txt, sizeof(msg_txt), msg_format, args ) ;
      va_end ( args ) ;
      eo60k_Message( 9 , msg_type, msg_no, msg_label, msg_txt);
    }
}

/*
 * ===========================================================================
 */

#undef MF__
#define MF__ MOD__"sql60kc_msg"
void    sql60kc_msg ( 
SAPDB_Byte                              prio ,
const char                            * text ,
SAPDB_Int                               msg_type )
{
    int                             errfd , lgt , lgtpref , rc ;
    char                            diagline [ 1024 ];

    sql22_SetApplDiagFileName();
    if ( sql22_timestamp && sql22_file )
    {
        RTESys_Timestamp timeBuffer;
      /* PTS 1109704 */
    	(void) sp77sprintf ( diagline , sizeof(diagline),
			            "%s %5d ",
                         RTESys_FillProtocolTimeStamp(timeBuffer),
  	                     (int) getpid() );
    	lgtpref = strlen ( diagline );
    }
    else
    {
    	lgtpref = 0 ;
    }

    /*
     *  Append text
     */
    (void) sp77sprintf ( diagline + lgtpref, sizeof(diagline) - lgtpref,
			            "%s\n", text );
    lgt = strlen ( diagline );

    /*
     *  Write to diag file
     *  If diag file cannot be opened for writing, write to stderr
     */
	en22_writeToDiagFile(diagline, lgt);
}

/*
 * ==================================================================
 */
#undef  MF__
#define MF__ MOD__"eo60k_Message"

void	eo60k_Message   ( tsp3_priority       msg_prio ,
                             tsp00_Int4             msg_type ,
                             tsp00_Int4             msg_no ,
                             const char          *msg_label ,
                             const char          *msg_txt )
{
    static char * msg_type_txt[] = { "ERR", "ERR", "WNG", "   " } ;
    char          msg_line [ 1024 ];

    if ( msg_type <= ( sizeof ( msg_type_txt ) / sizeof ( char *) ) )
    {
      sp77sprintf (msg_line, sizeof(msg_line), "%3.3s %-6d %-8.8s %s",
              msg_type_txt [ msg_type ], msg_no, msg_label, msg_txt) ;

      DBG3 (( MF__, "msg_line:%s\n", msg_line )) ;

      sql60kc_msg ( msg_prio, msg_line , msg_type);
    }

    return ;
}

/*===========================================================================*/
/*
  Function: en22_writeToDiagFile
  Description: Doing the actual writing for all message outputs
  Arguments: diagline [in] line to be written
             lgt [in] length in bytes
 */
static void en22_writeToDiagFile(char *diagline, int lgt)
{
    if ( sql22_fd < 0 )
    {
		if ( sql22_file )
		{
			sql22_fd = open ( sql22_file , O_WRONLY | O_APPEND | O_CREAT , 0666 );
		}
		else
		{
			sql22_fd = open ( "/dev/tty", O_WRONLY );
		}
    }

	if ( sql22_fd >= 0 )
	{
		(void)write ( sql22_fd , diagline , (unsigned) lgt );
	    if ( ! sql22_keep_open )
	    {
			(void) close ( sql22_fd );
			sql22_fd = -1 ;
		}
	}
}

/*
  Function: en22MessageOutputCall
  Description: Doing the actual writing for all message outputs
 */
void en22MessageOutputCall( SAPDB_Int4         msgOutputMask,
                            SAPDB_Int4         msgNumber, 
                            SAPDB_Int4         msgType, 
                            SAPDB_Char const * msgLabel, 
                            SAPDB_Char const * msgFormat,
                            va_list            msgArguments )
{
    SAPDB_Char msgText[1024];
    SAPDB_Int4 msgPrio = 8;

    sp77vsprintf ( msgText, sizeof(msgText), msgFormat, msgArguments ) ;

    /* Individual selection of output not supported yet !!!! See vos60.c for better implementation... */
    switch ( msgOutputMask )
    {
        case SAPDB_MSG_TO_ALL:
            msgPrio = 6;
            break;

        case SAPDB_MSG_TO_DIAGFILE:
            msgPrio = 8;
            break;

        default:
            msgPrio = 7;
            break;
    }

    eo60k_Message( msgPrio, msgType, msgNumber, msgLabel, msgText);
}

/*
  @brief select stdout instead of appldiag for output of diagnostic messages
 */
void en22SelectChannelForMessageOutput(SAPDB_Int channel, SAPDB_Bool withTimstamp)
{
    sql22_timestamp = withTimstamp;
    sql22_keep_open = true;
    sql22_file = (SAPDB_Char *)"";
    sql22_fd   = channel;
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
