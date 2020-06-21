/*!
  @file           ven60.c
  @author         JoergM
  @brief          Kernel RunTime: Message Output
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

#define MOD__ "ven60.c:"

#include        <ctype.h>
#include        <stdarg.h>

#include "gen00.h"
#include "heo00.h"
#include "geo50_0.h"
#include "geo00_2.h"
#include "RunTime/RTE_Types.h"
#include "hsp77.h"
#include "heo60_2.h"

#include "SAPDBCommon/SAPDB_Names.h"

#define MSG_TO_OPMSG1     1
#define MSG_TO_OPMSG2     2
#define MSG_TO_OPMSG3     3
#define MSG_TO_ERR        4
#define MSG_TO_UTIL_PROT  5
#define MSG_TO_EVENT_FILE 6
#define INIT_DIAG        -2

#define MESSAGE_LINE_MXEN60 512

/* exported functions */
#include "gen60.h"
#include "gen60_1.h"
#include "hen60.h"
#include "hen40.h"

/* imported functions */

#include "gen41.h"
#include "heo46.h"
#include "gen48.h"
#include "geo007_1.h"
#include "geo007_2.h"
#include "RunTime/Diagnose/RTEDiag_ServerCrashHist.h"
#include "RunTime/System/RTESys_Time.h"
#include "hsp100.h"
#include "RunTime/RTE_Speed.h"
#include "RunTime/RTE_DiagPipe.h"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageOutput.h"
#include "Messages/Msg_OutputKnlDiag.h"

extern struct tm *en52Localtime_r(const time_t *clock,
                                  struct tm *res);


/* local functions */

static  void    en60_Message ( int     prio,
                          int     msgno,
                          const char   *msgtyp,
                          const char   *ModuleInfo,
                          const char   *text,
                          va_list args );
static  int                             en60_OpenOp1Msg ( RTE_XPARAM_REC *XparamParameters );
static  int                             en60_OpenOp2Msg ( RTE_XPARAM_REC *XparamParameters );
static  int                             en60_OpenOp3Msg ( RTE_XPARAM_REC *XparamParameters );
#define TRY_TO_WRITE_STARTING_MESSAGE 1
#define DO_NOT_TRY_TO_WRITE_STARTING_MESSAGE 0
static  int                             en60_OpenErrmsg ( int tryToWriteStartingMessage );
static  int                             en60_OpenUtilProt ( RTE_XPARAM_REC *XparamParameters );
static  void                            en60_Opmsg ( int prio , 
                                                     int msgno , 
                                                     const char *text , ... );
static  void                            en60_opNOut( char *msg,
                                                     int length,
                                                     int outputNumber );
static  void                            en60_Write ( int * fd,
                                                     const char * msg,
                                                    int length );
static  void                            en60_InitDiag (const char *msg,
                                                      int length);
static  void                            en60_SetFilePos ( int *fd, 
                                                           long *min_pos, 
                                                           long *act_pos,
                                                           long f_size,
                                                           int length);
static	void                            en60_FillFile ( int *fd,
                                                        long f_size,
                                                        char *f_name);
static  void                            en60_FindUtilBeginPos( char *f_name,
                                                               long *act_pos );
static  void                            en60_CheckForUtilProtHeader(char *f_name );
static  void                            en60_Endmark (int fd,
                                                     long  *minpos,
                                                     long  *act_pos );
static  void    err ( char *text, ... );
static  int                             en60_ReopenDiag ( RTE_XPARAM_REC *XparamParameters);

/* exported data */

extern int                             e60_dbgdbglvl ;

/* imported data */

extern  tsp00_DbNamec                   en81_dbname; 

/* local data */

static  tsp00_Int4                      e60_diag_size ;
static  tsp00_Int4                      e60_util_size ;
static  tsp00_Int4                      en60_EventFileSize = 0;
static  long                            e60_filesize = 0;
static  int                             e60_operrfd = 0;
static  int                             e60_opmsg1fd = 0;
static  int                             e60_opmsg2fd = 0;
static  int                             e60_opmsg3fd = 1 ;
static  teo07_Mutex                     en60_DiagMutex = NULL;
static  int                             e60_op_ut_fd = -1;
static  int                             en60_EventFileFd = 0;
static	int								e60_op1_to_op3 = 0;
static	int								e60_op2_to_op3 = 0;
static	int								e60_kern_rubbish = 0;
static  int                             e60_op1_istty = 0;
static  int                             e60_op2_istty = 0;
static  int                             e60_op3_istty = 0;
static  int                             e60_err_istty = 0;
static  int                             e60_op_ut_istty = 0;
static  int                             en60_EventFileIsTty = 0;
static  long                            e60_op3_pos = 0;
static  long                            e60_op3_minpos = INIT_DIAG ;
static  long                            e60_ut_pos = 0;
static  long                            e60_ut_minpos  = 0 ;
static  long                            en60_EventFilePos = 0;
static  long                            en60_EventFileMinPos  = 0 ;
static  char                            e60_FILLINE [] =
"                                                                                \n";
/* NO end of line wanted for e60_ENDMARK!!!! Line will look ugly, but parsing a line */
/* otherwise not possible, since beginning of line would be random... */
static  char                            e60_ENDMARK [] = /* CURR_DIAG_WRITE_POS */ 
"--------------------------------------- current write position -----------------";
static  char                            e60_ENDSTARTUP [] = /* PTS 1110178 */
"======================================= end of startup part ====================\n";
static  char                            e60_EOFMARK [] =    /* PTS 1110178 */
"============================= end of file, look for current write positon ======\n";
static  char                            en60_EXITMARK[] =
"+++++++++++++++++++++++++++++++++++++++ Kernel Exit ++++++++++++++++++++++++++++\n";
static  char                            en60_KNLDIAG_HEADER[] =
"--------------------------------------------------------------------------------\n"
"Date       Time       TID Typ MsgID Label    Message-Text\n"
"--------------------------------------------------------------------------------\n";
static  char                            en60_UTILPROT_HEADER[] = /* PTS 1112542 */
"--------------------------------------------------------------------------------\n"
"Date       Time     Message-Text\n"
"--------------------------------------------------------------------------------\n";
#ifdef  DEBUG_RTE
#define LOCALDBG(x)        {err x;}
#else
#define LOCALDBG(x)        {}
#endif

/*
 * ===========================================================================
 */

externC  void    vopmsg ( 
tsp00_Int2                        prio ,
tsp00_Int2                        msgno ,
tsp00_Int4                        npar1 ,
tsp00_Int4                        npar2 ,
tsp00_C64                         spar ,
tsp00_C64                         msgline )
{
	ten50_UKT_Control *this_ukt = THIS_UKT_CTRL;
    char                            line [ MESSAGE_LINE_MXEN60 ];

    this_ukt->curr_task->state = TSK_VOPMSG_EO00 ;

    /*
     *  First line of output is the formatted message line.
     */
    eo46PtoC ( line , msgline , sizeof(tsp00_C64) );
#ifdef NEW_DIAG_FILES
    Msg_OutputToKnlDiag(msgno,0,"KERNEL",line);
#endif
#ifndef NO_OLD_DIAG_FILES
    en60_Opmsg ( (int) prio , (int) msgno , "%s\n" , line );
#endif

    /*
     *  Second line consists of two numbers and a string.
     *  Modify the priority to distinguish this from the first line.
     *
     *  eo46PtoC ( line , spar , sizeof(tsp00_C64) );
     *  en60_Opmsg ( (int) prio | 0x80 , (int) msgno , "%4ld %4ld %s\n" ,
	 *				    npar1 , npar2 , line );
     */

    this_ukt->curr_task->state = TSK_RUNNING_EO00 ;
}

/*
 * ===========================================================================
 */

#ifndef NO_OLD_DIAG_FILES
externC  void    sql60kc_msg ( 
tsp00_Uint1                          prio ,
const char                         * text ,
int                                  msg_type )
{
    en60_Opmsg ( (int) prio , (int) 0 , "%s\n" , text );
}
#endif

#define UTILPROT_HEADER_LEN (  1 + (sizeof(RTESys_Timestamp)-1) )
                         /* PRIO + DATE + TIME */

/*
 * ===========================================================================
 */

externC void    sql60_write_to_util_diag ( const char *text ,
                                          int          len )

{
    char                            * MsgTxt; /* PTS 1108955 */
    RTESys_Timestamp             timeBuffer;

    /* PTS 1108952 */
    MsgTxt = alloca ( UTILPROT_HEADER_LEN + len + 2 ) ;  /* PTS 1108955 */
	if ( MsgTxt == NULL ) 
	{
      MSGD (( ERR_MALLOC_FAILED_FOR, "sql60_write_to_util_diag"));
	  return ;
    }
    /*  Create Message Header */
    (void) sp77sprintf ( MsgTxt , UTILPROT_HEADER_LEN + len + 2, "%c%s ", '4', RTESys_FillProtocolTimeStamp(timeBuffer) );
    SAPDB_memcpy ( MsgTxt+UTILPROT_HEADER_LEN+1, text, len ) ;
    len += UTILPROT_HEADER_LEN+1 ;
    MsgTxt [ len ] = '\n' ;
    len++ ;
    e60_deliver_msg (MsgTxt, len);
}

/*------------------------------*/

externC void sql60_write_to_event_file ( const char *text ,
                                        int          len )
{
    char MsgTxt[MESSAGE_LINE_MXEN60] ;

    MsgTxt[0] = '5';
    SAPDB_memcpy ( MsgTxt+1, text, len ) ;
    len++ ;
    MsgTxt [ len ] = '\n' ;
    len++ ;
    e60_deliver_msg (MsgTxt, len);
}

/*
 * ===========================================================================
 */

/**VARARGS3*/
/*PRINTFLIKE3*/
#ifndef NO_OLD_DIAG_FILES
static  void    en60_Opmsg ( int prio , 
                            int msgno , 
                            const char *text , ... )
{
    va_list                         args ;

    va_start ( args, text );
    en60_Message ( prio , msgno , "   " , NULL, text , args );
    va_end ( args );
}
#endif
/*
 * ===========================================================================
 */

static  void    en60_Message ( 
int                             prio ,
int                             msgno ,
const char                      *msgtyp ,
const char                      *ModuleInfo ,
const char                      *text ,
va_list                         args )
{
    RTESys_Timestamp              timeBuffer;
    char                          opmsg [ MESSAGE_LINE_MXEN60 ];
    int                           oplgt ;
    long                          tid;

  /* PTS 1105263 */
    tid = ( en60_DiagMutex && RTESys_IsInCrashSignalHandler() == 0 ? sqlgetthreadid() : 0 );
    oplgt = 0 ;

    /*
     *  Create Message Header
     */
    if ( ModuleInfo )
        sp77sprintf ( opmsg , sizeof(opmsg),
  	        "%c %-8.8s %s %5ld %3.3s %s ",
	        /*  2      11     31   37    41  offset */
	        (char)(prio + '0') , 
            en81_dbname ,
            RTESys_FillProtocolTimeStamp(timeBuffer),
	        tid,
            msgtyp, 
            ModuleInfo );
    else if ( msgno ) 
        sp77sprintf ( opmsg , sizeof(opmsg),
  	        "%c %-8.8s %s %5ld %3.3s %5d ",
	        /*  2      11 31   37    42  offset */
	        (char)(prio + '0') , 
            en81_dbname ,
            RTESys_FillProtocolTimeStamp(timeBuffer),
	        tid ,
            msgtyp ,
            msgno );
    else
        sp77sprintf ( opmsg , sizeof(opmsg),
  	        "%c %-8.8s %s %5ld ",
	        /*  2      11     26     offset */
	        (char)(prio + '0') , 
            en81_dbname ,
            RTESys_FillProtocolTimeStamp(timeBuffer),
		    tid );

#define POS_PRIO                0
#define POS_DBNAME              2
#define POS_DATE                11
#define POS_TIME                22
#define POS_MSG_TYPE            37
#define POS_LABEL               41
#define POS_MSGTXT              50

    oplgt = strlen ( opmsg );

  /*
   *  Append Message Text
   */
  (void) sp77vsprintf ( &opmsg[oplgt] , sizeof(opmsg)-oplgt, text , args );
  if ( ModuleInfo )
    strcat(opmsg, "\n") ; 
  oplgt = strlen ( opmsg );
  e60_deliver_msg (opmsg, oplgt);

}

/*
 * ===========================================================================
 */


#undef MF__
#define MF__ MOD__"e60_deliver_msg"
externC void e60_deliver_msg (char *msg, int lgt )
{
    register int                    is_par_msg ;
    register int                    prio ;
    register int                    rc ;

    if ( RTESys_IsInCrashSignalHandler() == 0 )
    {
        /* 
           If we are crashing, mutex could already be blocked.
           We simply ignore it, since we cannot 'reset' it...
         */
        if ( en60_DiagMutex ) sqlbeginmutex(&en60_DiagMutex);
    }

    /*
     *  Messages read from the diag fifo by the console writer
     *  need not be formatted by en60_Message!
     */
    if ( ( lgt > POS_MSGTXT &&
	       isdigit(msg [ POS_PRIO ] & 0x7f) &&
	       msg [ POS_PRIO + 1 ] == ' '
         )
       ||  (msg [ POS_PRIO ] == '4')
       ||  (msg [ POS_PRIO ] == '5') )
    {
		/*
		 *  The eights bit of the first byte (wich is the message priority)
		 *  distinguishes a text message from a parameter message.
		 */
		is_par_msg = msg [ POS_PRIO ] & 0x80 ;
		prio = msg [ POS_PRIO ] - '0' ;

		if ( is_par_msg != 0 )
		{
    		msg [ POS_PRIO ] &= 0x7f ; /* strip the eights data bit */
			if ( (prio >= 1) && (prio <= 3) ) 
            {
                en60_opNOut( msg, lgt, MSG_TO_OPMSG3 );
            }

            if ( RTESys_IsInCrashSignalHandler() == 0 )
            {
                /* 
                   If we are crashing, mutex could already be blocked.
                   We simply ignore it, since we cannot 'reset' it...
                 */
            	if ( en60_DiagMutex ) sqlendmutex(&en60_DiagMutex);
            }

			return ;
		}
    }
    else
    {
		/* messages NOT generated by en60_Message go into the diag file only */
		is_par_msg = FALSE ;
		prio = 8 ;
    }

    if (prio != 4 &&  
        msg[POS_MSG_TYPE + 2] == 'R' ) /* IER || ERR */
	{
		en60_opNOut( msg, lgt, MSG_TO_ERR );
	}

    switch ( prio )
    {

    case 1 :
    case 6 :
				en60_opNOut( msg, lgt, MSG_TO_OPMSG3 );
				if ( ! e60_op2_to_op3 ) en60_opNOut( msg, lgt, MSG_TO_OPMSG2 );
				if ( ! e60_op1_to_op3 ) en60_opNOut( msg, lgt, MSG_TO_OPMSG1 );
        		break ;
    case 2 :
    case 7 :
				en60_opNOut( msg, lgt, MSG_TO_OPMSG3 );
				if ( ! e60_op2_to_op3 ) en60_opNOut( msg, lgt, MSG_TO_OPMSG2 );
                break ;
    case 3 :
				if ( e60_kern_rubbish ) en60_opNOut( msg, lgt, MSG_TO_OPMSG3 );
                break ;

    case 4 :
				en60_opNOut( msg, lgt, MSG_TO_UTIL_PROT );
                break ;

    case 5 :
				en60_opNOut( msg, lgt, MSG_TO_EVENT_FILE );
                break ;

    case 8 :
    case 9 :
	default:
				en60_opNOut( msg, lgt, MSG_TO_OPMSG3 );
                RTE_WriteToDiagPipe(msg,lgt);
                break ;

    }

    if ( RTESys_IsInCrashSignalHandler() == 0 )
    {
        /* 
           If we are crashing, mutex could already be blocked.
           We simply ignore it, since we cannot 'reset' it...
         */
        if ( en60_DiagMutex ) sqlendmutex(&en60_DiagMutex);
    }
}

/*
 * ===========================================================================
 */

/**VARARGS1*/
/*PRINTFLIKE1*/
externC  void    sqldbgwrite ( const char *text , ... )

{
va_list                         args ;

	va_start ( args, text );
	en60_Message ( 8 , 0 , NULL , NULL, text , args );
	va_end ( args );
}

/*
 * ===========================================================================
 */

/**VARARGS1*/
/*PRINTFLIKE1*/
externC  void    sqlerrwrite ( const char *text , ... )

{
va_list                         args ;

	va_start ( args, text );
	en60_Message ( 8 , 11999 , "ERR" , NULL, text , args );
	va_end ( args );
}

/*
 * ===========================================================================
 */

/**VARARGS1*/
/*PRINTFLIKE1*/
externC  void    sqlwngwrite ( const char *text , ... )

{
va_list                         args ;

	va_start ( args, text );
	en60_Message ( 8 , 11999 , "WNG" , NULL, text , args );
	va_end ( args );
}

/*
 * ===========================================================================
 */

/**VARARGS2*/
/*PRINTFLIKE2*/
externC  void    sqldbg_modinfo_write ( const char *ModInfo, 
                                       const char *text , ... )

{
va_list                         args ;

	va_start ( args, text );
	en60_Message ( 8 , 11999 , "   " , ModInfo, text , args );
	va_end ( args );
}

externC void 
vksprintf ( tsp00_Addr pszMsg, const tsp00_Addr pszFormatStr, ...)
{
#undef  MF__
#define MF__ MOD__"vksprintf"
  va_list     args ;

  DBGIN;

  va_start ( args, pszFormatStr);

  /* Only used in vbd520.cpp */
  sp77vsprintf ( pszMsg, sizeof(tsp00_C40), pszFormatStr , args );

  va_end ( args );

  DBGOUT;

  return;
}

/*
 * Replaces implementations of e60_op[12]_out()
 */
static  void    en60_opNOut ( char *msg, int lgt, int opN )
{
    int off;
	int *pOpMsgFd;
	int op_istty;

	switch( opN )
	{
	case MSG_TO_OPMSG1:
		pOpMsgFd = &e60_opmsg1fd;
		op_istty = e60_op1_istty;
		break;

	case MSG_TO_OPMSG2:
		pOpMsgFd = &e60_opmsg2fd;
		op_istty = e60_op2_istty;
		break;

	case MSG_TO_OPMSG3:
		op_istty = e60_op3_istty;
		break;

	case MSG_TO_ERR:
		pOpMsgFd = &e60_operrfd;
		op_istty = e60_err_istty;
		break;

	case MSG_TO_UTIL_PROT:
		pOpMsgFd = &e60_op_ut_fd;
		op_istty = e60_op_ut_istty;
		break;

    case MSG_TO_EVENT_FILE:
		pOpMsgFd = &en60_EventFileFd;
		op_istty = en60_EventFileIsTty;
		break;

	default:
		return;
	}
		
	/* messages which are not generated by en60_Message must not be cut */
	if ( opN != MSG_TO_OPMSG3 )
	{
		if ( *pOpMsgFd <= 0 ) return ;
    }

	switch( opN )
	{
	case MSG_TO_OPMSG1:
	case MSG_TO_OPMSG2:
		off = ( (lgt > POS_MSGTXT) &&
				isdigit(msg [ POS_PRIO ]) &&
				(msg [ POS_PRIO + 1 ] == ' ') ) ? POS_DBNAME : 0 ;
		break;
	case MSG_TO_OPMSG3:
		off = ( (lgt > POS_LABEL) &&
		 	    isdigit(msg [ POS_PRIO ]) &&
			    (msg [ POS_PRIO + 1 ] == ' ') ) ? POS_DATE : 0 ;
		break;
	case MSG_TO_ERR:
		off = ( (lgt > POS_LABEL) &&
				isdigit(msg [ POS_PRIO ]) &&
				(msg [ POS_PRIO + 1 ] == ' ') ) ? POS_DATE : 0 ;
		break;
	case MSG_TO_UTIL_PROT:
		off = isdigit(msg [ POS_PRIO ]) ||
			  msg [ POS_PRIO ] == '4' ? 1 : 0 ;
		break;
	case MSG_TO_EVENT_FILE:
		off = isdigit(msg [ POS_PRIO ]) ||
			  msg [ POS_PRIO ] == '5' ? 1 : 0 ;
		break;
	default:
		return;
	}

	if ( opN == MSG_TO_OPMSG3 )
	{
        /* always reopen knldiag */
		if ( e60_opmsg3fd <= 0 )
		{
    		if ( KGS && KGS->XParam != NULL )
            {
                en60_OpenOp3Msg ( KGS->XParam );
            }
        }

		if ( e60_opmsg3fd <= 0 )
        {
			if ( ! KGS || KGS->state == SERVER_STARTING )
			{
				int     fd = 1 ;
				en60_Write ( & fd , msg + off , lgt - off );
			}
		}
	}

    if ( op_istty )
    {
		msg [ lgt ++ ] = '\r' ;
		msg [ lgt ] = '\0' ;
    }

	switch ( opN )
	{
	case MSG_TO_OPMSG1:
	case MSG_TO_OPMSG2:
	case MSG_TO_ERR:
    	en60_Write ( pOpMsgFd, msg + off , lgt - off );
		break;
	case MSG_TO_OPMSG3:
    	en60_InitDiag ( msg, lgt - 11 );
        en60_SetFilePos( &e60_opmsg3fd, &e60_op3_minpos, &e60_op3_pos, 
                        e60_filesize, lgt );
    	en60_Write ( &e60_opmsg3fd, msg + off, lgt - off );
		en60_Endmark (e60_opmsg3fd, &e60_op3_minpos, &e60_op3_pos  );
		break;
	case MSG_TO_UTIL_PROT:
		en60_SetFilePos( &e60_op_ut_fd, &e60_ut_minpos, &e60_ut_pos, 
					  e60_util_size, lgt );
		en60_Write ( &e60_op_ut_fd, msg + off, lgt - off );
		en60_Endmark (e60_op_ut_fd, &e60_ut_minpos, &e60_ut_pos  );
		break;
	case MSG_TO_EVENT_FILE:
		en60_SetFilePos( pOpMsgFd, &en60_EventFileMinPos, &en60_EventFilePos, 
					  en60_EventFileSize, lgt );
		en60_Write ( pOpMsgFd, msg + off, lgt - off );
		en60_Endmark (*pOpMsgFd, &en60_EventFileMinPos, &en60_EventFilePos  );
		break;
	default:
		return;
	}

    if ( op_istty )
    {
		msg [ -- lgt ] = '\0' ;
    }
}

/*
 * ===========================================================================
 */
static void en60_SetFilePos( int      *fd, 
                              long     *min_pos, 
                              long     *act_pos, 
                              long     f_size,
                              int      length)
{
    long                                pos ;
    int                                 lgt ;
    int                                 off ;

    if ( *min_pos >= 0 )
    {
        pos = lseek ( *fd , NO_OFFSET, SEEK_CUR );
        if ( (pos + length + sizeof(e60_ENDMARK)-1) > f_size )
        {
            lgt = f_size - pos ;
            if ( lgt < 0 ) 
                lgt = 0 ;
            if ( lgt > sizeof(e60_EOFMARK)-1 )
                lgt = sizeof(e60_EOFMARK)-1 ;
            off = sizeof(e60_EOFMARK)-1 - lgt ;
            en60_Write ( fd , e60_EOFMARK + off , lgt );
            *act_pos = *min_pos ;
            pos = lseek ( *fd , *act_pos , SEEK_SET );
            if ( pos != *act_pos )
            {
                err ( "set_file_pos: seek error, %s\n", sqlerrs() );
                /* stop seeking */
                *min_pos = -1 ;
            }
        }
    }
}

/*
 * ===========================================================================
 */
static  void    en60_Write ( 
int                             * fdp ,
const char                            * text ,
int                             length )
{
    int                             rc ;

    rc = write ( * fdp , text , (unsigned) length );
    if ( rc != length )
    {
		err ( "en60_Write: write returned %d, %s, fd %d, length %u text %s\n",
					    rc , sqlerrs() , * fdp , length, text );
		if ( rc == 0 || ( (rc == -1) && (errno == EBADF) ) )
		{
			/* terminal not online */
			DBG1 (( MF__,"en60_Write: closing fd %d, setting to -1 \n", * fdp ));
			(void) close ( * fdp );
			/* negative file descriptor requests reopen */
			* fdp = -1 ;
			return ;
		}
    }
}

/*
 * ===========================================================================
 */
void sql60_strt_msg_wrt_cycle(void)
{
    long                                pos ;

#ifdef NEW_DIAG_FILES  
    Msg_StartupPhaseCompleted();
#endif
    en60_Write ( & e60_opmsg3fd , e60_ENDSTARTUP , sizeof(e60_ENDSTARTUP)-1 );  /* PTS 1110178 */

    pos = lseek ( e60_opmsg3fd , NO_OFFSET , SEEK_CUR );
    if ( pos > 0 )
    {
        e60_op3_pos = pos ;
        e60_op3_minpos = pos ;
        e60_filesize = e60_diag_size ;
    }
    else
    {
        err ( "en60_InitDiag: seek error, %s\n", sqlerrs() );
        /* don't seek */
        e60_op3_minpos = -1 ;
    }
}

/*
 * ===========================================================================
 */
static  void    en60_InitDiag ( const char *msg, int length )
{

    if ( e60_op3_minpos == INIT_DIAG )
    {
        /* search for 'startup complete' */
		if ( sql48_strloc ( (char *)msg, "startup  complete" ) )
		{
            sql60_strt_msg_wrt_cycle();
		}
    }

}

/*
 * ===========================================================================
 */
static  void    en60_Endmark (int   fd, 
                             long  *minpos,
                             long  *act_pos )
{
    long pos ;

    if ( *minpos >= 0  )
    {
        pos = lseek ( fd , NO_OFFSET , SEEK_CUR );
        if ( pos < 0 )
        {
            err ( "en60_Endmark: seek error %s\n", sqlerrs() );
            /* stop seeking */
            *minpos = -1 ;
        }
        *act_pos = pos ;
        en60_Write ( & fd , e60_ENDMARK , sizeof(e60_ENDMARK)-1 );
        lseek ( fd , pos, SEEK_SET ); 
        /* do the seek immediatly behind the write... */
    }
}

/*
 * ===========================================================================
 */
externC	int e60_open_opmsgs ( RTE_XPARAM_REC *XparamParameters )
{
    int				rc ;
    char			*ptr ;

    LOCALDBG (( "e60_open_opmsgs: called \n" ))
    ptr = getenv ( "DBKERNRUBBISH" );
    e60_kern_rubbish = (! ptr || (*ptr != '0'));

    rc  = en60_OpenOp1Msg ( XparamParameters );
    rc |= en60_OpenOp2Msg ( XparamParameters );
    rc |= en60_OpenOp3Msg ( XparamParameters );

    /* --- Make backup of diagnostic files if the database last time was shutting */
    /* --- down abnormally */
    RTEDiag_MakeCrashHist (XparamParameters, en81_dbname);  

    /* --- Posting the flag for an abnormal database shutdown */
    RTEDiag_SetCrashSem (CRASH_SEM_POST, en81_dbname);

    rc |= en60_OpenUtilProt ( XparamParameters );
    LOCALDBG (( "e60_open_opmsgs: returning %d \n", rc ))
    return ( rc );
}

/*
 * ===========================================================================
 */

static	int     en60_OpenOp1Msg ( RTE_XPARAM_REC *XparamParameters )
{
    LOCALDBG (( "en60_OpenOp1Msg: called \n" ))

    /*
     *  Usually, opmsg1 is used for the system console
     */
    if ( XparamParameters->szOpMsg1 [ 0 ] != '\0' )
    {
		if ( ! strcmp ( XparamParameters->szOpMsg1 , SAPDB_DIAG_FILE_NAME) )
		{
			e60_op1_to_op3 = TRUE ;
		}
		else
		{
			LOCALDBG (( "en60_OpenOp1Msg: Open opmsg1 '%s' \n", 
						XparamParameters->szOpMsg1 ))
			e60_opmsg1fd = open ( XparamParameters->szOpMsg1 , 
									  O_WRONLY|O_NDELAY , 0664 );
			if ( e60_opmsg1fd < 0 )
			{
				e60_opmsg1fd = 0 ;
                MSGD (( 11489, WRN_TYPE, "PARAM   ", "Open system console '%s' reports (%d) %s", 
                                        XparamParameters->szOpMsg1, errno, sqlerrs() ));
                if ( EPERM == errno || EACCES == errno )
                {
                    MSGD (( 11489, WRN_TYPE, "PARAM   ", "Modify parameter OPMSG1 or enable write access to prevent this message" ));
                }
                else
                {
                    MSGD (( 11489, WRN_TYPE, "PARAM   ", "Modify parameter OPMSG1 to prevent this message" ));
                }
				LOCALDBG (( MF__": returning 0x200\n" ))
				return ( 0x200 );
			}
			LOCALDBG (( "en60_OpenOp1Msg: opmsg1fd %d \n", e60_opmsg1fd ))
			e60_op1_istty = isatty ( e60_opmsg1fd );
			LOCALDBG (( "en60_OpenOp1Msg: isatty   %d \n", e60_op1_istty ))
		}
    }

    LOCALDBG (( MF__": returning 0\n" ))
    return ( 0 );
}

/*
 * ===========================================================================
 */

static	int     en60_OpenOp2Msg ( RTE_XPARAM_REC *XparamParameters )
{
    LOCALDBG (( "en60_OpenOp2Msg: called \n" ))

    /*
     *  Usually, opmsg2 is used for the database console
     */
    if ( XparamParameters->szOpMsg2 [ 0 ] != '\0' )
	{
		if ( ! strcmp ( XparamParameters->szOpMsg2 , SAPDB_DIAG_FILE_NAME ) )
		{
			e60_op2_to_op3 = TRUE ;
		}
		else
		{
			LOCALDBG (( "en60_OpenOp2Msg: Open opmsg2 '%s'\n", 
						XparamParameters->szOpMsg2 ))
			e60_opmsg2fd = open ( XparamParameters->szOpMsg2 , 
									  O_WRONLY|O_NDELAY , 0664 );
			if ( e60_opmsg2fd < 0 )
			{
				e60_opmsg2fd = 0 ;
                MSGD (( 11489, WRN_TYPE, "PARAM   ", "Open database console '%s' reports (%d) %s", 
                                        XparamParameters->szOpMsg2, errno, sqlerrs() ));
                if ( EPERM == errno || EACCES == errno )
                {
                    MSGD (( 11489, WRN_TYPE, "PARAM   ", "Modify parameter OPMSG2 or enable write access to prevent this message" ));
                }
                else
                {
                    MSGD (( 11489, WRN_TYPE, "PARAM   ", "Modify parameter OPMSG2 to prevent this message" ));
                }
				return ( 0x20 );
			}
			LOCALDBG (( "en60_OpenOp2Msg: opmsg2fd %d \n", e60_opmsg2fd ))
			e60_op2_istty = isatty ( e60_opmsg2fd );
			LOCALDBG (( "en60_OpenOp2Msg: isatty   %d \n", e60_op2_istty ))
		}
	}

    LOCALDBG (( "en60_OpenOp2Msg: returning \n" ))
    return ( 0 );
}

/* =========================================================================== */

static	int     en60_OpenOp3Msg ( RTE_XPARAM_REC *XParam )

{
    off_t                           pos;

    LOCALDBG (( "en60_OpenOp3Msg: called \n" ))

    /* knldiag is already open, so only the size information needs an update and the missing formatting must be done */
	e60_diag_size = XParam->ulDiagSize * 1024; /* size now in KB */

    pos = lseek ( e60_opmsg3fd, NO_OFFSET, SEEK_CUR );

    en60_FillFile ( &e60_opmsg3fd, e60_diag_size, SAPDB_DIAG_FILE_NAME ) ;
    if ( e60_opmsg3fd < 0 ) /* Formatting failed... */
    {
        e60_opmsg3fd = open(SAPDB_DIAG_FILE_NAME, O_WRONLY|O_NDELAY, 0664);
        if ( e60_opmsg3fd < 0 )
        {
            return (2); /* very fatal... cannot reopen knldiag */
        }
        /* seek to position before formatting started */
        lseek(e60_opmsg3fd, pos, SEEK_SET);
        MSGD (( ERR_WRITE_FILE, SAPDB_DIAG_FILE_NAME, errno ));

        /* Redirect standard output and standard error to opmsg3 */
	    if ( 1 != e60_opmsg3fd )
        {
            if ( 1 != dup2( e60_opmsg3fd, 1) )
            {
		        MSGD (( ERR_REDIRECT_FAILED ,"stdout", errno ));
	        }
        }

        if ( 2 != e60_opmsg3fd )
        {
            if ( 2 != dup2( e60_opmsg3fd, 2) )
	        {
		        MSGD (( ERR_REDIRECT_FAILED ,"stderr", errno ));
	        }
        }

	    /* Save the resources of a file descriptor */
	    (void) close ( e60_opmsg3fd );
	    e60_opmsg3fd = 1 ;
        return ( 2 );
    }

    (void)lseek(e60_opmsg3fd, pos, SEEK_SET);

    LOCALDBG (( "en60_OpenOp3Msg: returning \n" ));
    return ( 0 );
}
/*
 * ===========================================================================
  Function: en60_CheckForUtilProtHeader
  Description: Add the standard header to utility protocol file if needed

  ---------------------------------------------------------------------------------------------------------------
  gedankenexperiment... complete but much too complicated 'migration' of previous format:
  ---------------------------------------------------------------------------------------------------------------

	  A second file is opened and its content is copied as follows

	  original file
	  |entry n+1
	  |...
	  |last entry
	  |----ENDMARK
	  |oldest entry
	  |....
	  |oldest + x entry
	  |....
	  |entry n
	  |----EOFMARK

	  or

	  |oldest entry
	  |....
	  |oldest + x entry
	  |...
	  |last entry
	  |----ENDMARK


	  new file
	  |----HEADER
	  |oldest + x entry
	  |....
	  |last entry
	  |----ENDMARK

	  Why oldest + x entry? The header needs some space so we have to discard oldest
	  entry up to oldest + x - 1 entry....

	  The problem is to find oldest + x entry.

	  Fileoffsets in original file first variant
	  entry n + 1         is 0
	  ----ENDMARK         is cur_pos (found with string comparing ENDMARK)
	  ----EOFMARK         is end_pos (found with string comparing EOFMARK) 
	  oldest entry        is old_pos (is 1 + position of first '\n' behind cur_pos )
	  space used by entries = (cur_pos - 0) + (end_pos - old_pos)


	  Fileoffsets in original file second variant
	  oldest entry        is old_pos (0)
	  ----ENDMARK         is cur_pos (found with string comparing ENDMARK or last '\n' in file)
	  space used by entries = (cur_pos - 0)


	  new_old_pos: position of oldest entry + x is found by searching from old_pos ('\n' separates entries) 
	  until the following equation is valid

	  headersize + endmarksize + space used by entries - (new_old_pos - old_pos) <= filesize

	  After the position is found, the contents are copied into a help file which replaces the original afterwards

  ---------------------------------------------------------------------------------------------------------------
  end of complete but much too complicated 'migration' of previous format
  ---------------------------------------------------------------------------------------------------------------

  Instead of this if header is not detected, filename is renamed into 'filename.old'...
  This leads to an not existing file, which is handled correctly...

  Arguments: filename [in] protocol filename
 */

static void en60_CheckForUtilProtHeader( char *filename )
{
  int fd = open ( filename, O_RDONLY );
  long headerLength = strlen(en60_UTILPROT_HEADER);

  if ( fd >= 0 ) 
  {
	  char * line = alloca ( headerLength );

	  if ( read ( fd, line, headerLength ) == headerLength
		&& memcmp( line, en60_UTILPROT_HEADER, headerLength ) == 0 )
	  {
	      close ( fd );
		  return;
	  }

      close ( fd ) ;

	  {
		  char * oldFilename = alloca ( strlen(filename) + strlen(".old") + 1 );
		  strcpy(oldFilename, filename);
		  strcat(oldFilename, ".old");
		  rename(filename, oldFilename);
	  }
  }
}

/*
 * ===========================================================================
 */
static int getNextFileLine(int fd, off_t *pLinePos, char *bufferStart, long bufferSize, long *pBytesCached, long *lengthFound)
{
    int  iBytes;
    long bytesIn;

    if ( 0 == *lengthFound )
    {
        *pLinePos = 0;
    }
    else
    {
        *pLinePos += *lengthFound;
    }

    if ( *pBytesCached > 0 )
    {
        SAPDB_memmove( bufferStart
                     , bufferStart + *lengthFound
                     , *pBytesCached);
        memset(bufferStart + *pBytesCached
             , 0
             , bufferSize - *pBytesCached);
        for ( iBytes = 0
            ; iBytes < *pBytesCached
            ; ++iBytes )
        {
            if ( bufferStart[iBytes] == '\n' )
            {
                bufferStart[iBytes] = 0;
                *lengthFound = (iBytes+1);
                *pBytesCached -= *lengthFound;
                return 1;
            }
        }
    }

    do {
        bytesIn = read( fd
                      , bufferStart + *pBytesCached
                      , bufferSize - *pBytesCached );

        if ( bytesIn == 0 ) /* end of file condition */
        {
            bufferStart[*pBytesCached ] = 0;
            *lengthFound = *pBytesCached;
            *pBytesCached = 0;
            return (*lengthFound > 0 ? 1 : 0); /* check for end of file... */
        }

        if ( bytesIn > 0 )
        {
            *pBytesCached += bytesIn;
            for ( iBytes = *pBytesCached - bytesIn; iBytes < *pBytesCached; ++iBytes )
            {
                if ( bufferStart[iBytes] == '\n' )
                {
                    bufferStart[iBytes] = 0;
                    *lengthFound = (iBytes+1);
                    *pBytesCached -= *lengthFound;
                    return 1;
                }
            }

            if ( bufferSize == *pBytesCached ) /* slowly skip until end of line... */
            {
                char searchEndOfLine;

                bufferStart[bufferSize] = 0;
                *lengthFound = bufferSize;
                do {
                    switch( read( fd
                          , &searchEndOfLine
                          , 1 ) )
                    {
                        case 0:
                            *pBytesCached = 0;
                            break;
                        case 1:
                            ++*lengthFound;
                            if ( '\n' == searchEndOfLine )
                            {
                                *pBytesCached = 0;
                            }
                            break;
                        default:
                            if ( errno != EINTR )
                            {
                                return 0;
                            }
                            break;
                    }
                } while ( *pBytesCached > 0 );

                return 1;
            }
        }
    } while ( bytesIn > 0 || EINTR == errno );  /* force ignore of interrupted read call... */

    return 0;
}

/*
 * ===========================================================================
 */
static void en60_FindUtilBeginPos( char *filename, 
                                   long *act_pos  ) 
{
  int fd = open ( filename, O_RDONLY );
  long headerLength = strlen(en60_UTILPROT_HEADER) ;

  *act_pos = headerLength ;

  if ( fd >= 0 ) 
  {
      off_t cur_pos = 0;
	  int   reachedEndOfFile = 0 ;
      long   bytesCached = 0;
      long   lengthFound = 0;
      char line[MESSAGE_LINE_MXEN60] ;
      SAPDB_memset(line, 0, sizeof(line));

	  do
      {
          if ( getNextFileLine(fd, &cur_pos, &line[0], sizeof ( line ) - 1, &bytesCached, &lengthFound) )
          {
              if ( memcmp ( line, e60_ENDMARK , sizeof(e60_ENDMARK)-1 ) == 0 )
              { 
                  LOCALDBG (( "en60_FindUtilBeginPos: end found at pos %ld\n", cur_pos ));
				  *act_pos = (long)cur_pos;
                  reachedEndOfFile = 1 ;
              }
          }
          else
		  {
              LOCALDBG (( "en60_FindUtilBeginPos: no ENDMARK. Begin after header\n" ));
              reachedEndOfFile = 1 ;
		  }
      } while ( !reachedEndOfFile ) ;
 
      close ( fd ) ;
  }
}

/*
 * ===========================================================================
 */

static	void    en60_FillFile ( int  *fd,
                                long  f_size ,
                                char *f_name)

{
    int    idx ;
    size_t FillLength;

    FillLength = strlen(e60_FILLINE);
    idx = (f_size + FillLength - 1) / FillLength;

    for ( ; idx && *fd != -1 ; idx -- )
    {
    	if ( write ( *fd , e60_FILLINE , FillLength) != FillLength )
	    {
	        MSGD (( ERR_WRITE_FILE, f_name, errno ));
	        (void) close ( *fd );
    	    *fd = -1 ;
    	}
    }

    if ( *fd > 0 )
    {
        if ( (long) lseek ( *fd , NO_OFFSET , SEEK_SET ) == -1L )
        {
	        MSGD (( ERR_SEEK_FILE, f_name, errno ));
	        (void) close ( *fd );
        	*fd = -1 ;
        }
	}
}

/*
 * ===========================================================================
 */

#undef MF__
#define MF__ MOD__"en60_OpenErrmsg"
static	int     en60_OpenErrmsg ( int tryToWriteStartingMessage )

{
    const char *e79_errmsg = SAPDB_ERROR_FILE_NAME;
	SAPDB_Char	speed[RTE_MAXSPEEDLEN + 1];	

    LOCALDBG (( MF__": called \n" ))
   
	e60_operrfd = open ( e79_errmsg, O_WRONLY | O_APPEND | O_CREAT | O_NDELAY, 0664);
	if ( e60_operrfd < 0 )
	{
	  e60_operrfd = e60_opmsg3fd;
	  MSGD (( ERR_OPEN, e79_errmsg, errno ));
	  return ( 0x20 );
	}
	LOCALDBG (( MF__": operrfd %d \n", e60_operrfd ))
	e60_err_istty = false;

     /* PTS 1108255 */
    if ( tryToWriteStartingMessage )
    {
        char MsgTxt[MESSAGE_LINE_MXEN60] ;
        RTESys_Timestamp         timeBuffer;
        RTESys_Timestamp         timeBufferGMT;
        /* PTS 1109227 */
        struct stat              stat_buf;
        tsp00_Versionc           currentVersion;

        if (  fstat( e60_operrfd, &stat_buf ) != 0
           || stat_buf.st_size == 0 )
        {
            write( e60_operrfd, en60_KNLDIAG_HEADER, strlen(en60_KNLDIAG_HEADER) );
        }
		
		/* PTS 1110621 */
		SAPDB_memset (speed, 0, RTE_MAXSPEEDLEN + 1);
		strcpy (speed, RTE_GetSpeed ());
		if (!strcmp (speed, RTE_FAST) )
		{
			strcpy (speed, " ");
		}
		else
		{
			strcat (speed, " ");
		}

		sp100_GetVersionString( speed, 
	    						s100buildnumber,
		    					&currentVersion[0]);

        /*  Create Starting Message */
        (void) sp77sprintf ( MsgTxt , sizeof(MsgTxt), "%s %5.5s %3.3s %5.5s %8.8s --- Starting GMT %s %s \n",
                                    RTESys_FillProtocolTimeStamp(timeBuffer), "", "", "", "", 
                                    RTESys_FillProtocolGMTTimeStamp(timeBufferGMT), &currentVersion[0] );

        write(e60_operrfd, MsgTxt, strlen(MsgTxt));
	}

    LOCALDBG (( "en60_OpenErrmsg: returning \n" ))
    return ( 0 );
}

/*
 * ===========================================================================
 */
void RTE_WriteDatabaseStopTime()
{
    char MsgTxt[MESSAGE_LINE_MXEN60] ;
    RTESys_Timestamp         timeBuffer;
    RTESys_Timestamp         timeBufferGMT;
    tsp00_Versionc           currentVersion;
	SAPDB_Char	speed[RTE_MAXSPEEDLEN + 1];	

	SAPDB_memset (speed, 0, RTE_MAXSPEEDLEN + 1);
	strcpy (speed, RTE_GetSpeed ());
	if (!strcmp (speed, RTE_FAST) )
	{
		strcpy (speed, " ");
	}
	else
	{
		strcat (speed, " ");
	}

	sp100_GetVersionString( speed, 
	    					s100buildnumber,
		    				&currentVersion[0]);

    /*  Create Starting Message */
    (void) sp77sprintf ( MsgTxt , sizeof(MsgTxt), "%s %5.5s %3.3s %5.5s %8.8s ___ Stopping GMT %s %s \n",
                                RTESys_FillProtocolTimeStamp(timeBuffer), "", "", "", "", 
                                RTESys_FillProtocolGMTTimeStamp(timeBufferGMT), &currentVersion[0] );

    write(e60_operrfd, MsgTxt, strlen(MsgTxt));
}

/*
 * ===========================================================================
 */
#undef MF__
#define MF__ MOD__"en60_OpenUtilProt"
static	int     en60_OpenUtilProt ( RTE_XPARAM_REC *XParam) 

{
    struct stat                     statbuf ;
    long                            curr_filesize ;
    size_t                          fillLength;
    long pos ;
    LOCALDBG (( MF__": called \n" ));
   
    /*
     *  Utility protfile is normally named 'dbm.utl'
     */
    if ( XParam->szUtilityProtFile [ 0 ] != '\0' )
    {
		LOCALDBG (( MF__": Open op_ut_prt '%s'\n", XParam->szUtilityProtFile ));

        fillLength = strlen(e60_FILLINE);

        /*--- round up filesize to number of lines (PTS 1110178) */
        e60_util_size = ((XParam->ulUtilityProtSize * XParam->ulDBPageSize) + fillLength - 1) / fillLength;
        /*--- calculate the amount of bytes for these lines */
        e60_util_size *= fillLength;

		/* if header is not found in file, file is moved into dbm.utl.old... PTS 1112465 */
		en60_CheckForUtilProtHeader( XParam->szUtilityProtFile );

		/* If size was modified remove old protocol file */
        if ( stat (XParam->szUtilityProtFile, & statbuf ) == 0
		  && statbuf.st_size != e60_util_size )
		{
		    unlink ( XParam->szUtilityProtFile ) ;
		}

		/* read file to find 'current write position' */
		en60_FindUtilBeginPos( XParam->szUtilityProtFile, &e60_ut_pos  );

		e60_op_ut_fd = open ( XParam->szUtilityProtFile , 
                      O_WRONLY | O_CREAT | O_NDELAY , 0664);
		if ( e60_op_ut_fd < 0 )
		{
			e60_op_ut_fd = 0 ;
			MSGD (( ERR_OPEN, XParam->szUtilityProtFile, errno ));
			return ( 0x2 );
		}

        if ( fstat (e60_op_ut_fd, & statbuf ) < 0 )
		{
			MSGD (( ERR_OPEN, XParam->szUtilityProtFile, errno ));
			close(e60_op_ut_fd);
			return ( 0x2 );
		}

		if ( (statbuf.st_mode & S_IFMT) == S_IFREG )
		{   
            e60_ut_minpos = strlen(en60_UTILPROT_HEADER);

		    if ( statbuf.st_size == 0 
		      || statbuf.st_size != e60_util_size )
			{
				LOCALDBG (( MF__": e60_op_ut_fd %d \n", e60_op_ut_fd ));

			    en60_FillFile ( &e60_op_ut_fd, 
				                e60_util_size, 
							    XParam->szUtilityProtFile ) ;

                if ( e60_op_ut_fd < 0 ) 
				{
					MSGD (( ERR_OPEN, XParam->szUtilityProtFile, errno ));
    	  	 	    LOCALDBG (( MF__": fill file failed -> returning 2\n" ));
		            return ( 0x2 );
				}

                write(e60_op_ut_fd, en60_UTILPROT_HEADER, strlen(en60_UTILPROT_HEADER));
			}
		    else
			{
                /* PTS 1110178 */
                pos = lseek (e60_op_ut_fd, e60_ut_pos , SEEK_SET);
                if (pos != e60_ut_pos)
				{
                    err ("set_file_pos: seek error, %s\n", sqlerrs());
                    /* stop seeking */
                    e60_ut_minpos = -1 ;
				}
			}
		
		    e60_op_ut_istty = FALSE ;
		}
		else
		{
			e60_op_ut_istty = isatty ( e60_op_ut_fd );
		}

		LOCALDBG (( MF__": op_ut_fd %d \n", e60_op_ut_fd ));
		LOCALDBG (( MF__": isatty   %d \n", e60_op_ut_istty ));
    }

    LOCALDBG (( MF__": returning 0\n" ));
    return ( 0 );
}

/*--------------*/

#undef MF__
#define MF__ MOD__"en60InitializeMessageOutput"
int en60InitializeMessageOutput()
{
    /* PTS 1117575 Open knldiag before parameter file is known */
    int         fd;
    tsp00_Pathc fname;
    struct stat statbuf;
    const char *lastMsg;
    int lastErrno = 0;

    sqlcreatemutex(&en60_DiagMutex);

    RTE_ConnectToDiagPipe(en81_dbname);
#ifdef NO_OLD_DIAG_FILES
    return 0;
#else
#define INITIAL_KNLDIAG_SIZE_EN60 (256*1024)
    /* initial size overridden if Xparameter are known */
	e60_diag_size = INITIAL_KNLDIAG_SIZE_EN60;

	/*
	 *  Save old diag file
	 */
	if ( stat ( SAPDB_DIAG_FILE_NAME, & statbuf ) >= 0 )
	{
        if ( statbuf.st_mode & S_IFREG )
		{
			if ( unlink  ( SAPDB_DIAG_FILE_NAME KNLDIAG_EXT_OLD ) < 0 )
            {
                lastErrno = errno;
                if ( lastErrno == ENOENT )
                {
                    lastErrno = 0; /* if no knldiag.old found, ignore it */
                }
                else
                {
                    lastMsg = "Unlink of " SAPDB_DIAG_FILE_NAME KNLDIAG_EXT_OLD " failed";
                }
            }

            if ( 0 == lastErrno )
            {
                if ( link ( SAPDB_DIAG_FILE_NAME, SAPDB_DIAG_FILE_NAME KNLDIAG_EXT_OLD ) < 0 )
			    {
                    lastMsg = "Link of " SAPDB_DIAG_FILE_NAME " to " SAPDB_DIAG_FILE_NAME KNLDIAG_EXT_OLD " failed";
                    lastErrno = errno;
                } else if ( unlink ( SAPDB_DIAG_FILE_NAME ) < 0 )
                {
                    lastMsg = "Unlink of " SAPDB_DIAG_FILE_NAME " failed";
                    lastErrno = errno;
                }
            }
		}
		else
		{
            lastMsg = SAPDB_DIAG_FILE_NAME " is no regular file";
            lastErrno = ENOENT;
        }
    }

	e60_opmsg3fd = open ( SAPDB_DIAG_FILE_NAME, O_WRONLY|O_CREAT|O_NDELAY , 0664 );
	if ( e60_opmsg3fd < 0 )
	{
		e60_opmsg3fd = 1 ;
        printf("Failed to create %s:%s\n", SAPDB_DIAG_FILE_NAME,sqlerrs());
        if ( lastErrno != 0 )
        {
            errno = lastErrno;
            printf("Previous problem: %s : %s\n", lastMsg, sqlerrs() );
            return ( 10 );
        }
	}
	if ( fstat ( e60_opmsg3fd , & statbuf ) == -1 )
	{
        printf("Failed to stat %s:%s\n", SAPDB_DIAG_FILE_NAME, sqlerrs());
        e60_opmsg3fd = 1 ;
		close ( e60_opmsg3fd );
		return ( 11 );
	}

    write(e60_opmsg3fd, en60_KNLDIAG_HEADER, strlen(en60_KNLDIAG_HEADER));

	e60_op3_istty = FALSE ;

    /* Redirect standard output and standard error to opmsg3 */
#ifndef NEW_DIAG_FILES
	if ( 1 != dup2 ( e60_opmsg3fd, 1 ) )
    {
		MSGD (( ERR_REDIRECT_FAILED ,"stdout", errno ));
		return ( 12 );
	}

	if ( 2 != dup2 ( e60_opmsg3fd, 2 ) )
    {
		MSGD (( ERR_REDIRECT_FAILED ,"stderr", errno ));
		return ( 13 );
	}

    /*
	 *  Save the resources of a file descriptor
	 */
	(void) close ( e60_opmsg3fd );
	e60_opmsg3fd = 1 ;
#endif
	/*
	 *  Try to change the owner of the diag file
	 */
	(void) chown ( SAPDB_DIAG_FILE_NAME, getuid() , getgid() );

    /*
     *  Finally open the knldiag.err file too
     */
    return en60_OpenErrmsg ( TRY_TO_WRITE_STARTING_MESSAGE );
#endif
}

/*--------------*/

static	void    en60_UpdateDiagPosition( char *filename, long *min_pos, long *act_pos )
{
  long last_pos;
  char line[MESSAGE_LINE_MXEN60] ;
  int fd;

  last_pos = 0;
  *min_pos = 0;
  *act_pos = 0;

  fd = open ( filename, O_RDONLY );
  if ( fd >= 0 ) 
  {
      off_t  cur_pos = 0;
	  int    reachedEndOfFile = 0 ;
      long   bytesCached = 0;
      long   lengthFound = 0;

      do
      {
          if ( getNextFileLine( fd, &cur_pos, &line[0], sizeof ( line ) -1, &bytesCached, &lengthFound) )
          {
              /* found "--- current write position --- " */
              if ( memcmp( line, e60_ENDMARK, strlen(e60_ENDMARK)-1 ) == 0 )
              {
                  last_pos = (long)(cur_pos);
                  break;
              }

              if ( memcmp( line, e60_FILLINE, strlen(e60_FILLINE)-1 ) == 0 )
              {
                  if ( last_pos == 0 )
                  {
                      /* Prepare for the case, that no "--- current write position --- " was ever written... */
                      last_pos = (long)cur_pos;
                  }
              }
              else if ( memcmp ( line, e60_ENDSTARTUP , strlen(e60_ENDSTARTUP)-1 ) == 0 )
              {
                  /* Save position of end of startup part, to prevent cyclic rewrite... */
				  *min_pos = (long)(cur_pos + lengthFound);
                  LOCALDBG (( "en60_UpdateDiagPosition: min found at pos %ld\n", *min_pos));
              }
          }
          else
		  {
              LOCALDBG (( "en60_UpdateDiagPosition: no ENDMARK. Begin at %ld\n", last_pos ));
              reachedEndOfFile = 1 ;
              if ( last_pos == 0 )
              {
                  last_pos = (long)(cur_pos + lengthFound);
              }
		  }
      } while ( !reachedEndOfFile ) ;
 
      if ( *min_pos == 0 )
      {
          *min_pos = last_pos;
      }
      *act_pos = last_pos;

      close ( fd ) ;
  }
}

static	int     en60_ReopenOpMsg ( RTE_XPARAM_REC *XparamParameters, int which )
{
    char *szOpMsg;
    int  *opMsgFd;
    int  *opIsTty;

    LOCALDBG (( "en60_ReopenOpmsg: called \n" ))

    switch ( which )
    {
    case 1:
		szOpMsg = XparamParameters->szOpMsg1; 
        opMsgFd = &e60_opmsg1fd;
        opIsTty = &e60_op1_istty;
        break;
    case 2:
		szOpMsg = XparamParameters->szOpMsg2; 
        opMsgFd = &e60_opmsg2fd;
        opIsTty = &e60_op2_istty;
        break;
    default:
        return 0;
    }

    if ( szOpMsg [ 0 ] != '\0' )
	{
		if ( ! strcmp ( szOpMsg , SAPDB_DIAG_FILE_NAME ) )
    	{
            if ( which == 1 )
    	    	e60_op1_to_op3 = TRUE ;
            else if ( which == 2 )
    			e60_op2_to_op3 = TRUE ;
    	}
	    else
		{
			LOCALDBG (( "en60_ReopenOpmsg: Open opmsg%d '%s'\n", which, szOpMsg ))
    		*opMsgFd = open ( szOpMsg , O_WRONLY|O_APPEND|O_NDELAY , 0664 );
    		if ( *opMsgFd < 0 )
	    	{
    			*opMsgFd = 0 ;
				MSGD (( ERR_OPEN, szOpMsg, errno ));
    			return ( 0x20 );
	    	}
    		LOCALDBG (( "en60_ReopenOpmsg: opmsg%dfd %d \n", which, *opMsgFd ))
    		*opIsTty = isatty ( *opMsgFd );
    		LOCALDBG (( "en60_ReopenOpmsg: isatty%d   %d \n", which, *opIsTty ))
        }
	}

    LOCALDBG (( "en60_ReopenOpmsg: returning \n" ))
    return ( 0 );
}

static int en60_ReopenDiag ( RTE_XPARAM_REC *XparamParameters)
{
    char *szOpMsg;
    int  *opMsgFd;
    int  *opIsTty;
    int   fd;

    LOCALDBG (( "en60_ReopenDiag: called \n" ))

    szOpMsg = SAPDB_DIAG_FILE_NAME;
    opMsgFd = &e60_opmsg3fd;
    opIsTty = &e60_op3_istty;

    if ( szOpMsg [ 0 ] != '\0' )
	{
        /*
         * Since System messages can be inserted at any time, it is NOT 
         * a good idea to find the last position... Instead appending
         * at end of file is the only reasonable solution.
         */
    	*opMsgFd = open ( szOpMsg , O_WRONLY|O_NDELAY , 0664 );
    	if ( *opMsgFd < 0 )
	    {
    		*opMsgFd = 1 ;
			MSGD (( ERR_OPEN, szOpMsg, errno ));
    		return ( 0x2 );
	    }
    	*opIsTty = isatty ( *opMsgFd );
    	LOCALDBG (( "en60_ReopenDiag: isatty3   %d \n", *opIsTty ))

        /*
    	 * Redirect standard output and standard error to opmsg3
  	     */
        (void) close ( 1 );
	    fd = dup ( *opMsgFd );
		if ( fd != 1 )
    	{
	    	MSGD (( ERR_REDIRECT_FAILED ,"stdout", errno ));
		    return ( 0x2 );
    	}

    	(void) close ( 2 );
	    fd = dup ( *opMsgFd );
		if ( fd != 2 )
    	{
	    	MSGD (( ERR_REDIRECT_FAILED ,"stderr", errno ));
		    return ( 0x2 );
    	}

        en60_UpdateDiagPosition( szOpMsg, &e60_op3_minpos, &e60_op3_pos);

        (void)lseek ( *opMsgFd ,e60_op3_pos, SEEK_SET );
 
        e60_filesize = e60_diag_size ;

	    en60_opNOut( en60_EXITMARK, strlen(en60_EXITMARK), MSG_TO_OPMSG3 );
	}

    LOCALDBG (( "en60_ReopenOpmsg: returning \n" ))
    return ( 0 );
}

#undef MF__
#define MF__ MOD__"en60_ReopenKnldiagFile"
externC void en60_ReopenKnldiagFile(RTE_XPARAM_REC *XparamParameters)
{
    int				rc ;

    LOCALDBG (( "en60_ReopenKnldiagFile: called \n" ))

    en60_DiagMutex = 0; /* Reopened knldiag output in watchdog thread is no longer serialized... */

    rc  = en60_ReopenDiag  ( XparamParameters);
    rc |= en60_OpenErrmsg ( DO_NOT_TRY_TO_WRITE_STARTING_MESSAGE );

    LOCALDBG (( "en60_ReopenKnldiagFile: returning %d \n", rc ))
    return;
}

/*
 * ===========================================================================
 */

/* cannot use DBG1 facility in this module ! */
/*ARGSUSED*/
/*VARARGS1*/
/*PRINTFLIKE1*/
static  void    err ( char *txt , ... )
{
    int                         rc ;
    int                         lgt ;
    va_list                     args ;
    char                        line [ 256 ];

    va_start ( args, txt );
    (void) sp77vsprintf ( line , sizeof(line), txt , args );
    va_end ( args );
    lgt = strlen ( line );
    rc = write ( 1 , line , (unsigned) lgt );
    if ( rc != lgt ) (void)write ( 2 , line , (unsigned) lgt );
}

/*--------------------------------------------------------------*/

int en60OpenEventFile ( tsp00_Pathc VAR_ARRAY_REF Filename, tsp00_Int4 MaxSize )
{
    if ( Filename[ 0 ] != '\0' )
	{
        en60_EventFileSize = MaxSize * XPARAM(ulDBPageSize);
        en60_EventFileFd = open ( Filename, 
							 O_WRONLY | O_CREAT | O_NDELAY , 0664);
		if ( en60_EventFileFd < 0 )
		{
		  MSGD (( ERR_OPEN, Filename, errno ));
		  return ( 1 );
		}
		en60_EventFileIsTty = isatty ( en60_EventFileFd );
	}

    return ( 0 );
}

/*--------------------------------------------------------------*/

void en60MessageOutputCall( SAPDB_Int4         msgOutputMask,
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
            msgPrio = 6; /* Message to SYSCON, DBCON + DIAGFILE */
            break;

        case SAPDB_MSG_TO_DIAGFILE:
            msgPrio = 8; /* Message to DIAGFILE */
            break;

        default:
            msgPrio = 7; /* Message to DBCOM + DIAGFILE */
            break;
    }

#ifdef NEW_DIAG_FILES
    Msg_OutputToKnlDiag(msgNumber,msgType,msgLabel,msgText);
#endif
#ifndef NO_OLD_DIAG_FILES
    eo60k_Message( msgPrio, msgType, msgNumber, msgLabel,msgText );
#endif
}
