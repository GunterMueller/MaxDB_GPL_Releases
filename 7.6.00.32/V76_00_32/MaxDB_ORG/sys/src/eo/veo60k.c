/* @lastChanged: "1998-04-22  10:23"
 * @filenameveo60kveo60kveo60kveo60kveo60kveo60kveo60kveo60k.c
 * @purpose:     "vmessage / eventing"
 * @release:     7.1.1.1
 * @see:         ""
 *
 * @Copyright (c) 1998-2005 SAP AG"


\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 1998-2005 SAP AG

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

#include <stdarg.h>
#include "geo00_2.h"
#include "geo002.h"
#include "vsp0031.h"
#include "geo00_0.h"
#include "geo007_1.h"
#include "geo50_0.h"
#include "heo46.h"
#include "heo60.h"
#include "heo60_2.h"
#include "hsp77.h"
#include "geo60.h"
#ifdef KERNEL_LZU
#  include "RunTime/System/RTESys_Time.h" /* nocheck */
#  include "heo52.h" /* nocheck */
#  include "geo67.h" /* nocheck */
#endif /* KERNEL_LZU */

#ifdef NEW_DIAG_FILES
#include "Messages/Msg_OutputKnlDiag.h" /* nocheck */
#include "Messages/Msg_OutputUtilProt.h" /* nocheck */
#endif

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

#if !defined(_WIN32) && !defined(OS2)
global  int                             e60_dbgdbglvl ;
#define IS_DEBUGLEVEL_SET (e60_dbgdbglvl == 1)
#else
#define IS_DEBUGLEVEL_SET TRUE
#endif

#ifdef KERNEL_LZU
#  if !defined(_WIN32)
#  define CTASK_STATE(_ukt)     (_ukt)->curr_task->state
#  else
#  define CTASK_STATE(_ukt)     (_ukt)->pCTask->TaskState
#  endif
#  define USE_EVENT_FILE (XPARAM(ulEventSize) > 0 && XPARAM(szEventFile)[0] != '\0')
#endif /* KERNEL_LZU */

/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/



/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/



/*===========================================================================*
 *  GLOBAL VARIABLES                                                         *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL VARIABLES                                                          *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL FUNCTIONS (PROTOTYPES)                                             *
 *===========================================================================*/


/*===========================================================================*
 *  GLOBAL FUNCTIONS (CODE)                                                  *
 *===========================================================================*/

#ifdef KERNEL_LZU

void	vtime_in_seconds  ( tsp00_Int4          *time_in_sec )
{
#undef  MF__
#define MF__ MOD__"vtime_in_seconds"

  time_t  TimeInSecSince1970 ;

  DBGPAS;

  time( &TimeInSecSince1970 );

  *time_in_sec = (tsp00_Int4 )TimeInSecSince1970 ;

  DBG3 (( MF__, "seconds since 1970: %ld", *time_in_sec )) ;

  return ;
}

/*
 * ==================================================================
 */

void	vmessage     ( tsp3_priority_Param  msg_prio ,
                       tsp3_msg_type_Param  msg_type ,
                       tsp00_Int4             msg_no ,
                       tsp00_C8               msg_label ,
                       tsp00_C40              msg_txt )
{
#undef  MF__
#define MF__ MOD__"vmessage"

  char                            msg_txtc[ sizeof ( tsp00_C40 ) + 1 ] ;
  char                            msg_labelc[ sizeof ( tsp00_C8 ) + 1 ] ;
  UKT_CTRL                        this_ukt  = THIS_UKT_CTRL;
  UINT1                           curr_state;

  DBGPAS;

  if  ( this_ukt )
  {
    curr_state            = CTASK_STATE(this_ukt);
    CTASK_STATE(this_ukt) = TSK_VOPMSG_EO00;
  }

  eo46PtoC ( msg_txtc   , msg_txt   , sizeof(tsp00_C40) );
  eo46PtoC ( msg_labelc , msg_label , sizeof(tsp00_C8) );

#ifdef NEW_DIAG_FILES
  Msg_OutputToKnlDiag(msg_no,msg_type,msg_labelc,msg_txtc);
#endif
#ifndef NO_OLD_DIAG_FILES
  eo60k_Message ( msg_prio , (tsp3_msg_type)msg_type, msg_no, msg_labelc, msg_txtc );
#endif

  if  ( this_ukt )
    CTASK_STATE(this_ukt) = curr_state;

  return ;
}
/*
 * ==================================================================
 */

void	vutilprot    ( char             *msg_txt,
                       tsp00_Int4          msg_len)
{
#undef  MF__
#define MF__ MOD__"vutilprot"
  UKT_CTRL                        this_ukt  = THIS_UKT_CTRL;
  UINT1                           curr_state;

  DBGPAS;

  if  ( this_ukt )
  {
    curr_state            = CTASK_STATE(this_ukt);
    CTASK_STATE(this_ukt) = TSK_VOPMSG_EO00;
  }

#ifdef NEW_UTIL_PROT
  Msg_OutputToUtilProt( msg_txt, msg_len );
#endif
  sql60_write_to_util_diag ( msg_txt, msg_len );

  if  ( this_ukt )
    CTASK_STATE(this_ukt) = curr_state;

  return ;
}


/*
 * ==================================================================
 */

void 	eo60InsertEvent    ( tsp31_event_description const *Event )
{
#undef  MF__
#define MF__ MOD__"eo60InsertEvent"

  DBGPAS;

#if defined(_WIN32)
  if ( !kgs.XParam )
#else
  if ( !kgs || !kgs->XParam ) 
#endif
  {
      /* Shared memory or parameter structure not yet attached... */
      return;
  }

  if ( USE_EVENT_FILE )
    {
      /* add some extra space for ascii conversion */
	  char EventTxt [ sizeof(tsp31_event_description) + 50] ;
	  char EventTxtFormat [ 50 ] ;
	  int EventTxtLen, FieldValueLen ;

      EventTxtLen = sp77sprintf( EventTxt, sizeof(EventTxt), "%8.8s %8.8s %10u %2u %1u ",
                                       Event->sp31ed_date,
									   Event->sp31ed_time,
                                       Event->sp31ed_eventcnt,
									   Event->sp31ed_ident,
									   Event->sp31ed_priority);

      if ( Event->sp31ed_value_1 != MAX_INT4_SP00 )
      {
		    FieldValueLen = sp77sprintf( EventTxt + EventTxtLen, sizeof(EventTxt) - EventTxtLen, "%10u ", Event->sp31ed_value_1);
      }
      else
      {
		    FieldValueLen = sp77sprintf( EventTxt + EventTxtLen, sizeof(EventTxt) - EventTxtLen, "%10.10s ", "");
      }
	  EventTxtLen += FieldValueLen ;                       			

      if ( Event->sp31ed_value_2 != MAX_INT4_SP00 )
      {
		    FieldValueLen = sp77sprintf( EventTxt + EventTxtLen, sizeof(EventTxt) - EventTxtLen, "%10u ", Event->sp31ed_value_2);
      }
      else
      {
		    FieldValueLen = sp77sprintf( EventTxt + EventTxtLen, sizeof(EventTxt) - EventTxtLen, "%10.10s ", "");
      }
	  EventTxtLen += FieldValueLen ;                       			

      /* restrict length of string output (not ZERO terminated pascal string was input... */
      sp77sprintf( EventTxtFormat, sizeof(EventTxtFormat), "%%%d.%ds", Event->sp31ed_text_len, Event->sp31ed_text_len);
      EventTxtLen += sp77sprintf( EventTxt + EventTxtLen, sizeof(EventTxt) - EventTxtLen, EventTxtFormat, Event->sp31ed_text_value );

      sql60_write_to_event_file ( EventTxt, EventTxtLen);

    }

}
#endif /* KERNEL_LZU */

/*
 * ==================================================================
 */

void sql60c_varg_message   ( tsp3_priority        msg_prio ,
                             tsp3_msg_type        msg_type ,
                             tsp00_Int4             msg_no ,
                             const char          *msg_label ,
                             const char          *msg_format ,
                             ...                           )
{
#undef  MF__
#define MF__ MOD__"sql60c_varg_message"

  char                            msg_txt [ 256 ];
  va_list                         args ;

  DBGPAS;

  va_start ( args, msg_format ) ;
  sp77vsprintf ( msg_txt, sizeof(msg_txt), msg_format, args ) ;
  va_end ( args ) ;

#ifdef NEW_DIAG_FILES
  Msg_OutputToKnlDiag(msg_no,msg_type,msg_label,msg_txt);
#endif
#ifndef NO_OLD_DIAG_FILES
  eo60k_Message( msg_prio , msg_type, msg_no, msg_label, msg_txt );
#endif

  return ;
}


/*
 * ==================================================================
 */

void	sql60c_msg_6    ( tsp00_Int4            msg_no ,
                          tsp00_Int4            msg_type ,
                          const char         *msg_label ,
                          const char         *msg_format ,
                          ...                           )
{
#undef  MF__
#define MF__ MOD__"sql60c_msg_6"

  char                            msg_txt [ 256 ];
  va_list                         args ;

  va_start ( args, msg_format ) ;
  sp77vsprintf ( msg_txt, sizeof(msg_txt), msg_format, args ) ;
  va_end ( args ) ;

#ifdef NEW_DIAG_FILES
  Msg_OutputToKnlDiag(msg_no,msg_type,msg_label,msg_txt);
#endif
#ifndef NO_OLD_DIAG_FILES
  eo60k_Message( 6 , msg_type, msg_no, msg_label, msg_txt );
#endif

}

/*
 * ==================================================================
 */

void	sql60c_msg_7    ( tsp00_Int4            msg_no ,
                          tsp00_Int4            msg_type ,
                          const char         *msg_label ,
                          const char         *msg_format ,
                          ...                           )
{
#undef  MF__
#define MF__ MOD__"sql60c_msg_7"

  char                            msg_txt [ 256 ];
  va_list                         args ;

  va_start ( args, msg_format ) ;
  sp77vsprintf ( msg_txt, sizeof(msg_txt), msg_format, args ) ;
  va_end ( args ) ;

#ifdef NEW_DIAG_FILES
  Msg_OutputToKnlDiag(msg_no,msg_type,msg_label,msg_txt);
#endif
#ifndef NO_OLD_DIAG_FILES
  eo60k_Message( 7 , msg_type, msg_no, msg_label, msg_txt );
#endif
}

/*
 * ==================================================================
 */
void	sql60c_msg_8    ( tsp00_Int4            msg_no ,
                          tsp00_Int4            msg_type ,
                          const char         *msg_label ,
                          const char         *msg_format ,
                          ...                           )
{
#undef  MF__
#define MF__ MOD__"sql60c_msg_8"

  char                            msg_txt [ 256 ];
  va_list                         args ;

  va_start ( args, msg_format ) ;
  sp77vsprintf ( msg_txt, sizeof(msg_txt), msg_format, args ) ;
  va_end ( args ) ;

#ifdef NEW_DIAG_FILES
  Msg_OutputToKnlDiag(msg_no,msg_type,msg_label,msg_txt);
#endif
#ifndef NO_OLD_DIAG_FILES
  eo60k_Message(  8, msg_type, msg_no, msg_label, msg_txt );
#endif
}
/*
 * ==================================================================
 */

void	sql60c_msg_9    ( tsp00_Int4            msg_no ,
                          tsp00_Int4            msg_type ,
                          const char         *msg_label ,
                          const char         *msg_format ,
                          ...                           )
{
#undef  MF__
#define MF__ MOD__"sql60c_msg_9"

  char                            msg_txt [ 256 ];
  va_list                         args ;

  if ( IS_DEBUGLEVEL_SET )
    { va_start ( args, msg_format ) ;
      sp77vsprintf ( msg_txt, sizeof(msg_txt), msg_format, args ) ;
      va_end ( args ) ;
#ifdef NEW_DIAG_FILES
      Msg_OutputToKnlDiag(msg_no,msg_type, msg_label,msg_txt);
#endif
#ifndef NO_OLD_DIAG_FILES
      eo60k_Message( 9 , msg_type, msg_no, msg_label, msg_txt );
#endif
    }
}

/*===========================================================================*
 *  LOCAL FUNCTIONS (CODE)                                                   *
 *===========================================================================*/


/*
 * ==================================================================
 */
#ifndef NO_OLD_DIAG_FILES
void eo60k_Message      ( tsp3_priority_Param msg_prio ,
                          tsp00_Int4            msg_type ,
                          tsp00_Int4            msg_no ,
                          const char         *msg_label ,
                          const char         *msg_txt )
{
#undef  MF__
#define MF__ MOD__"eo60k_Message"

#if !defined(_WIN32)
    /* all internal errors (index 0 -> IER) are errors now (ERR)... */
    static char * msg_type_txt[] = { "ERR", "ERR", "WNG", "   " } ;
    char          msg_line [ 256 ];
#endif

    DBGPAS;

    DBG3 (( MF__, "prio:%d, type:%d, no:%d, label:%.8s, txt:%s\n",
                  msg_prio, msg_type, msg_no, msg_label, msg_txt )) ;
#if !defined(_WIN32)
        /* PTS 1110451 */

    if ( msg_type <= ( sizeof ( msg_type_txt ) / sizeof ( char *) ) )
    {
#ifdef KERNEL_LZU
        if ( !RTESys_IsInCrashSignalHandler()  /* PTS 1115271 */
         && ( msg_type == 0 || msg_type == 1 ) )
        {
            eo67InsertMessageEvent( msg_no, msg_txt);
        }
#endif /* KERNEL_LZU */
        /* PTS 1110451 */
        sp77sprintf( msg_line, sizeof(msg_line), "%3s %5d %-8.8s %s",
              msg_type_txt [ msg_type ], msg_no, msg_label, msg_txt) ;

        DBG3 (( MF__, "msg_line:%s\n", msg_line )) ;

        sql60kc_msg ( msg_prio, msg_line , msg_type);
    }
#else
    /* this includes now eo67InsertMessageEvent() call */
    sql60_msg_prio_x (msg_prio, msg_no, msg_type, msg_label, msg_txt );
#endif
}
#endif
/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
