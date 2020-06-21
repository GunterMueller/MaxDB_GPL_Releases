/*!
  @file           geo60.h
  @author         JoergM
  @special area   Debugging and Message Output + Trace Class
  @brief          Operating system dependend debugging macros
  @see            

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



#ifndef GEO60_H
#define GEO60_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include "geo00.h"
#include "gsp00_0.h"
#include "geo47.h"
#if !defined(_WIN32)
#include "gen60.h" /* nocheck */
#endif

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

#if defined(__cplusplus)
extern "C" {
#endif

#if defined(_WIN32)

#if defined ( DEBUG_RTE )
 #define     DEBUG0
 #define     DEBUG1
 #define     DEBUG2
 #define     DEBUG3
 #define     DEBUG4
 #define     DEBUG5
 #define     DEBUG6
#endif

#if defined (DEBUG0) || defined (DEBUG1) || defined (DEBUG2) ||               \
    defined (DEBUG3) || defined (DEBUG4) || defined (DEBUG5) ||               \
    defined (DEBUG6)
# if defined(_WIN32)
#  include   <debm_api.h>
# else
#  include   <pmmonapi.h>
# endif
#endif

/* --- Function calls */
#ifdef  DEBUG0
 #define     DBGIN                                                            \
               {MON_Named_Msg0 (MF__, "--> called");}
 #define     DBGOUT                                                           \
               {MON_Named_Msg0 (MF__, "<-- returning");}
 #define     DBGPAS                                                           \
               {MON_Named_Msg0 (MF__, "-- passed -->");}
 #define     DBGIN_T(_T)                                                      \
               {MON_Named_Msg0 (MF__, "[T:0x%03u] --> called", _T);}
 #define     DBGOUT_T(_T)                                                     \
               {MON_Named_Msg0 (MF__, "[T:0x%03u] <-- returning", _T);}
 #define     DBGPAS_T(_T)                                                     \
               {MON_Named_Msg0 (MF__, "[T:0x%03u] -- passed -->", _T);}
#else
 #define     DBGIN        {;}
 #define     DBGOUT       {;}
 #define     DBGPAS       {;}
 #define     DBGIN_T(_T)  {;}
 #define     DBGOUT_T(_T) {;}
 #define     DBGPAS_T(_T) {;}
#endif

/* --- Errors */
#ifdef  DEBUG1
 #define     DBG1(_arg) {MON_Named_Msg1 _arg;}
#else
 #define     DBG1(_arg) {;}
#endif

/* --- Buffer contents */
#ifdef  DEBUG2
 #define     DBG2(_arg) {MON_Named_Buf2 _arg;}
#else
 #define     DBG2(_arg) {;}
#endif

/* --- Information */
#ifdef  DEBUG3
 #define     DBG3(_arg) {MON_Named_Msg3 _arg;}
#else
 #define     DBG3(_arg) {;}
#endif

/* --- Dispatching */
#ifdef  DEBUG4
 #define     DBG4(_arg) {MON_Named_Msg4 _arg;}
#else
 #define     DBG4(_arg) {;}
#endif

/* --- Dispatching */
#ifdef  DEBUG5
 #define     DBG5(_arg) {MON_Named_Msg5 _arg;}
#else
 #define     DBG5(_arg) {;}
#endif

/* --- Remote communication */
#ifdef  DEBUG6
 #define     DBG6(_arg) {MON_Named_Msg6 _arg;}
#else
 #define     DBG6(_arg) {;}
#endif

/* --- Error output */
#define _EVTLST_MSG_PARAM(_e)  ((LONG) _e.eo200_EventID(),    \
                                (ULONG)_e.eo200_EventType(),  \
                                (const PSZ) _e.eo200_EventLabel(), \
                                (const PSZ) _e.eo200_EventMsg() )

/* PTS 1120317 */
#define SAVE_LAST_ERROR_     DWORD lastError_ = GetLastError();
#define RESTORE_LAST_ERROR_  SetLastError(lastError_);

/* ALL = All possible error outputs */
#define  MSGALL(_arg)                   {SAVE_LAST_ERROR_; sql60_msg_all _arg;                              RESTORE_LAST_ERROR_; }
#define  MSGALL_EVTLST(_evt)            {SAVE_LAST_ERROR_; sql60_msg_all _EVTLST_MSG_PARAM(_evt);           RESTORE_LAST_ERROR_; }
/* CD  = Console + Diagnose */
#define  MSGCD(_arg)                    {SAVE_LAST_ERROR_; sql60_msg_con_diag _arg;                         RESTORE_LAST_ERROR_; }
#define  MSGCD_EVTLST(_evt)             {SAVE_LAST_ERROR_; sql60_msg_con_diag _EVTLST_MSG_PARAM(_evt);      RESTORE_LAST_ERROR_; }
/* C   = Console only */
#define  MSGC(_arg)                     {SAVE_LAST_ERROR_; sql60_msg_con _arg;                              RESTORE_LAST_ERROR_; }
#define  MSGC_EVTLST(_evt)              {SAVE_LAST_ERROR_; sql60_msg_con _EVTLST_MSG_PARAM(_evt);           RESTORE_LAST_ERROR_; }
/* D   = Diagnose only */
#define  MSGD(_arg)                     {SAVE_LAST_ERROR_; sql60_msg_diag _arg;                             RESTORE_LAST_ERROR_; }
#define  MSGD_EVTLST(_evt)              {SAVE_LAST_ERROR_; sql60_msg_diag _EVTLST_MSG_PARAM(_evt);          RESTORE_LAST_ERROR_; }
/* BOX = Messagebox (NT only) on UNIX -> console */
#define  MSGBOX(_arg)                   {SAVE_LAST_ERROR_; sql60_msg_box _arg;                              RESTORE_LAST_ERROR_; }
#define  MSGBOX_EVTLST(_evt)            {SAVE_LAST_ERROR_; sql60_msg_box _EVTLST_MSG_PARAM(_evt);           RESTORE_LAST_ERROR_; }
/* EL  = Eventlog (NT only) on UNIX -> console + diagnose */
#define  MSGEL(_arg)                    {SAVE_LAST_ERROR_; sql60_msg_event_log _arg;                        RESTORE_LAST_ERROR_; }
#define  MSGEL_EVTLST(_evt)             {SAVE_LAST_ERROR_; sql60_msg_event_log _EVTLST_MSG_PARAM(_evt);     RESTORE_LAST_ERROR_; }
/* CEL = Console + Eventlog (NT only) on UNIX -> console + diagnose */
#define  MSGCEL(_arg)                   {SAVE_LAST_ERROR_; sql60_msg_con_event_log _arg;                    RESTORE_LAST_ERROR_; }
#define  MSGCEL_EVTLST(_evt)            {SAVE_LAST_ERROR_; sql60_msg_con_event_log _EVTLST_MSG_PARAM(_evt); RESTORE_LAST_ERROR_; }

externC void  sql60_msg_all                ( LONG              lMsgID,
                                             ULONG             ulEventType,
                                             const char        *pszLabel,
                                             const char        *pszFormatStr,
                                             ... );
externC void  sql60_msg_con_diag           ( LONG              lMsgID,
                                             ULONG             ulEventType,
                                             const char        *pszLabel,
                                             const char        *pszFormatStr,
                                             ... );
externC void  sql60_msg_diag               ( LONG              lMsgID,
                                             ULONG             ulEventType,
                                             const char        *pszLabel,
                                             const char        *pszFormatStr,
                                             ... );
externC void  sql60_msg_box                ( LONG              lMsgID,
                                             ULONG             ulEventType,
                                             const char        *pszLabel,
                                             const char        *pszFormatStr,
                                             ... );
externC void  sql60_msg_con                ( LONG              lMsgID,
                                             ULONG             ulEventType,
                                             const char        *pszLabel,
                                             const char        *pszFormatStr,
                                             ... );
externC void  sql60_msg_event_log          ( LONG              lMsgID,
                                             ULONG             ulEventType,
                                             const char        *pszLabel,
                                             const char        *pszFormatStr,
                                             ... );
externC void sql60_msg_con_event_log       ( LONG               lMsgID,
                                             ULONG              ulEventType,
                                             const char        *pszLabel,
                                             const char        *pszFormatStr,
                                             ...);

#else /* defined(WIN32) */

#include <errno.h> /* nocheck */

#if defined ( DEBUG_RTE )
 #define     DEBUG0
 #define     DEBUG1
 #define     DEBUG2
 #define     DEBUG3
 #define     DEBUG4
 #define     DEBUG5
 #define     DEBUG6
#endif

/* --- Function calls */
#ifdef  DEBUG0
# define     DBGIN   {sqldbg_modinfo_write(MF__,"--> called");} 
# define     DBGIN_T(_T) {sqldbg_modinfo_write(MF__,"[T:0x%03u] --> called", _T);} 
# define     DBGOUT  {sqldbg_modinfo_write(MF__,"<-- returning");} 
# define     DBGOUT_T(_T) {sqldbg_modinfo_write(MF__,"[T:0x%03u] --> returning", _T);} 
# define     DBGPAS  {sqldbg_modinfo_write(MF__,"-- passed -->");} 
# define     DBGPAS_T(_T) {sqldbg_modinfo_write(MF__,"[T:0x%03u] -- passed -->", _T);} 
#else
# define     DBGIN        {;}
# define     DBGOUT       {;}
# define     DBGPAS       {;}
# define     DBGIN_T(_T)  {;}
# define     DBGOUT_T(_T) {;}
# define     DBGPAS_T(_T) {;}
#endif

/* --- Errors */
#ifdef  DEBUG1
# define     DBG1(x) {sqldbg_modinfo_write x ;}
#else
# define     DBG1(_arg) {;}
#endif

/* --- Buffer contents */
#ifdef  DEBUG2
# define     DBG2(x) {sqldbg_modinfo_write x ;}
#else
# define     DBG2(_arg) {;}
#endif

/* --- Information */
#ifdef  DEBUG3
# define     DBG3(x) {sqldbg_modinfo_write x ;}
#else
# define     DBG3(_arg) {;}
#endif

/* --- Dispatching */
#ifdef  DEBUG4
# define     DBG4(x) {sqldbg_modinfo_write x ;}
#else
# define     DBG4(_arg) {;}
#endif

/* --- Dispatching */
#ifdef  DEBUG5
# define     DBG5(x) {sqldbg_modinfo_write x ;}
#else
# define     DBG5(_arg) {;}
#endif

/* --- Remote communication */
#ifdef  DEBUG6
# define     DBG6(x) {sqldbg_modinfo_write x ;}
#else
# define     DBG6(_arg) {;}
#endif

/*
 --- Error output: Number = PRIO
  < 8 additional output on stderr (for Applications)
*/
#define _EVTLST_MSG_PARAM(_e)  ((tsp00_Int4) _e.eo200_EventID(),    \
                                (tsp00_Int4) _e.eo200_EventType(),  \
                                (const char*)    _e.eo200_EventLabel(), \
                                (const char*)    _e.eo200_EventMsg() )

/* PTS 1120317 */
#define SAVE_ERRNO_     int lastErrno_ = errno;
#define RESTORE_ERRNO_  errno = lastErrno_;

/* ALL = All possible error outputs */
#define  MSGALL(_arg)                   { SAVE_ERRNO_; sql60c_msg_6 _arg;                    RESTORE_ERRNO_;}
#define  MSGALL_EVTLST(_evt)            { SAVE_ERRNO_; sql60c_msg_6 _EVTLST_MSG_PARAM(_evt); RESTORE_ERRNO_;}
/* CD  = Console + Diagnose */
#define  MSGCD(_arg)                    { SAVE_ERRNO_; sql60c_msg_7 _arg;                    RESTORE_ERRNO_;}
#define  MSGCD_EVTLST(_evt)             { SAVE_ERRNO_; sql60c_msg_7 _EVTLST_MSG_PARAM(_evt); RESTORE_ERRNO_;}
/* C   = Console only */
#define  MSGC(_arg)                     { SAVE_ERRNO_; sql60c_msg_7 _arg;                    RESTORE_ERRNO_;}
#define  MSGC_EVTLST(_evt)              { SAVE_ERRNO_; sql60c_msg_7 _EVTLST_MSG_PARAM(_evt); RESTORE_ERRNO_;}
/* D   = Diagnose only */
#define  MSGD(_arg)                     { SAVE_ERRNO_; sql60c_msg_8 _arg;                    RESTORE_ERRNO_;}
#define  MSGD_EVTLST(_evt)              { SAVE_ERRNO_; sql60c_msg_8 _EVTLST_MSG_PARAM(_evt); RESTORE_ERRNO_;}
/* BOX = Messagebox (NT only) on UNIX -> console */
#define  MSGBOX(_arg)                   { SAVE_ERRNO_; sql60c_msg_7 _arg;                    RESTORE_ERRNO_;}
#define  MSGBOX_EVTLST(_evt)            { SAVE_ERRNO_; sql60c_msg_7 _EVTLST_MSG_PARAM(_evt); RESTORE_ERRNO_;}
/* EL  = Eventlog (NT only) on UNIX -> console + diagnose */
#define  MSGEL(_arg)                    { SAVE_ERRNO_; sql60c_msg_7 _arg;                    RESTORE_ERRNO_;}
#define  MSGEL_EVTLST(_evt)             { SAVE_ERRNO_; sql60c_msg_7 _EVTLST_MSG_PARAM(_evt); RESTORE_ERRNO_;}
/* CEL = Console + Eventlog (NT only) on UNIX -> console + diagnose */
#define  MSGCEL(_arg)                   { SAVE_ERRNO_; sql60c_msg_7 _arg;                    RESTORE_ERRNO_;}
#define  MSGCEL_EVTLST(_evt)            { SAVE_ERRNO_; sql60c_msg_7 _EVTLST_MSG_PARAM(_evt); RESTORE_ERRNO_;}

externC void	       sql60c_msg_6    ( int                  msg_no ,
                                         int                  msg_type ,
                                         const char          *msg_label ,
                                         const char          *msg_format ,
                                         ...                           ) ;

externC void	       sql60c_msg_7    ( int                  msg_no ,
                                         int                  msg_type ,
                                         const char          *msg_label ,
                                         const char          *msg_format ,
                                         ...                           ) ;

externC void	       sql60c_msg_8    ( int                  msg_no ,
                                         int                  msg_type ,
                                         const char          *msg_label ,
                                         const char          *msg_format ,
                                         ...                           ) ;

externC void	       sql60c_msg_9    ( int                  msg_no ,
                                         int                  msg_type ,
                                         const char          *msg_label ,
                                         const char          *msg_format ,
                                         ...                           ) ;
externC void sqldbg_modinfo_write ( const char *ModInfo,
                                    const char *text ,
                                    ... );

#endif /* !defined(WIN32) */

#define     DBGMC                       {int Missing_Code;}

#ifdef  DEBUG_RTE
# define STATIC
#else
# define STATIC                          static
#endif  /* DEBUG_RTE */

#if defined(__cplusplus)
}
#endif

#if defined (__cplusplus)

#if (COMPILEMODE_MSP00 >= SLOW_MSP00) || defined(DEBUG_RTE)

#  define ROUTINE_DBG_MEO60(name_eo60) \
 	teo60_Tracer localTrace (name_eo60) 

#  define DBG_MEO60(args) localTrace.trace args

#  define FUNCTION_NAME_MEO60 (_TCHAR *)localTrace.currentFuncName()

# else

/* fast + no debug */

#  define ROUTINE_DBG_MEO60(name_eo60) \
    static const _TCHAR *__currentFuncName = name_eo60 

#  define DBG_MEO60(args)
#  define FUNCTION_NAME_MEO60 (_TCHAR *)__currentFuncName

#endif
# else
/* fast + no debug */
#  define ROUTINE_DBG_MEO60(name_eo60) \
    static const _TCHAR *__currentFuncName = name_eo60
#  define DBG_MEO60(args)
#  define FUNCTION_NAME_MEO60 (char *)__currentFuncName

#endif /* __cplusplus */

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

#if defined (__cplusplus)


 class teo60_Tracer
{
private:
	const _TCHAR *name_eo60;
	int   level_eo60;
public:

    teo60_Tracer (const _TCHAR *nameparm, int levelparm = 1)
	:name_eo60(nameparm),
	level_eo60(levelparm)
	{
      DBG1 (( (char *)name_eo60, "--> enter"));
	}

    ~teo60_Tracer ()
	{
      DBG1 (( (char *)name_eo60, "<-- leave"));
	}

    void trace (int level, const _TCHAR *format, ...)
	{
    }

	/* for DBG macros... */
    const _TCHAR *currentFuncName() const { return name_eo60; }

};
#endif

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/

#endif  /* GEO60_H */

