/*!
  @file           RTECons_Main.cpp
  @author         StefanP
  @special area   Console (XCons)
  @brief          Main Module
  @see            

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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

#if (defined (LINUX) && !defined(HAS_NPTL) && !defined(HAS_NO_PTHREAD_MUTEX_PROCESS_SCOPE)) || defined (SUN) || defined(SOLARIS)
#define HAS_NO_PTHREAD_MUTEX_PROCESS_SCOPE
#endif
/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include    "heo92.h"
#include    "geo007_1.h"
#include    "hsp100.h"

#include    "RunTime/RTE_CompilerFeatures.h"
#include    "RunTime/RTE_MessageList.hpp"
#include    "RunTime/RTE_Message.hpp"
#include    "RunTime/RTE_XCons_Messages.hpp"
#include    "RunTime/RTE_ConsoleCommon.hpp"
#include    "RunTime/Console/RTECons_StandardSession.hpp"
#include    "RunTime/MemoryManagement/RTEMem_RteAllocator.hpp"
#include    "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include    "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include    "RunTime/RTE_ConsoleTraceTopic.hpp"
#include    "RunTime/RTE_ConsoleStandardOutput.h"

#if defined (HAS_NO_PTHREAD_MUTEX_PROCESS_SCOPE)            /*&if ($OSSPEC = LINUX && $__HAS_NPTL != 1) || $MACH = SUN || $OSSPEC == SOLARIS */
#include    "RunTime/RTE_ConsoleSemaphoreTimeoutList.hpp"   /* nocheck */
#endif                                                      /*&endif*/        


extern SAPDBTrace_Topic Console_Trace; ///< global console trace object

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


    
/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  DECLARATIONS                                                             *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/
  


/*===========================================================================*
 *  STATIC/INLINE FUNCTION PROTOTYPES                                        *
 *===========================================================================*/

static SAPDB_UInt4  GetComParam             (SAPDB_Char *                               pszParam );

static SAPDB_Bool   GetIdxParam             (SAPDB_Char *                               pszParam,
                                             SAPDB_Char                                 cPrefix,
                                             SAPDB_Long *                               plNum);

static SAPDB_Char * StringToUpper           (SAPDB_Char *                               string);

static SAPDB_Bool   GetNumParam             (SAPDB_Char *                               pszParam,
                                             SAPDB_Long *                               plNum);

static SAPDB_Bool   GetProcessorAffinityMask (SAPDB_Char *                              pszParam,
                                              SAPDB_Long *                              ProcessAffinityMask);

static SAPDB_UInt4  GetTimeParam            (SAPDB_Char *                               pszParam);

static SAPDB_UInt4  GetTraceTopic           (SAPDB_Char *                               pszParam);

static SAPDB_UInt4  GetSetClearParam        (SAPDB_Char *                               pszParam);

static SAPDB_UInt4  GetShowResParam         (SAPDB_Char *                               pszParam,
                                             SAPDB_UInt4                                command);

static SAPDB_Bool   GetRequestFromParameter (int                             argc, 
                                             char *                          argv[],
                                             RTE_ConsoleRequestRecord &      request);
/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  USED AS OWN PROCESS                                                      *
 *===========================================================================*/

#if defined (_WIN32)
/*!
   @brief          Main Routine of XCons
   @return         

 */


INT main 
(
    int     argc, 
    char    *argv[]
)
{
   /*===========================================================================*
    *  Locals                                                                   *
    *===========================================================================*/
    SAPDBErr_MessageList            messageList;


   /*===========================================================================*
    *  Instructions                                                             *
    *===========================================================================*/

/*
    RTE_PID                 pid;
    SAPDB_Char              diagXCons[80];
    
    GETPROCESSID (&pid);
    memset (diagXCons, 0, sizeof (diagXCons));
    strcpy (diagXCons, "d:\\SAPDevelop\\7.4\\usr\\wrk\\xcons");
    strcpy (diagXCons, "d:\\SAPDevelop\\Console\\xcons");

    sprintf (diagXCons + strlen (diagXCons),"_%lu", pid);
    strcat (diagXCons, ".txt");
    sql60_open_diag_file (diagXCons, 800, NULL);
*/

    sql60_enable_console ();

    SAPDBTRACE_ROUTINE_DEBUG("Main", Console_Trace, 1);


    if (!RTECons_ProcessStandardRequest (argc, argv))
    {
        printf("communication error\n");
    }
    return 0;
}

#endif

/*===========================================================================*
 *  LOCAL/PRIVATE/PROTECTED FUNCTIONS (CODE)                                 *
 *===========================================================================*/
static  SAPDB_Bool   GetRequestFromParameter  
(
    int                             argc, 
    char *                          argv[],
    RTE_ConsoleRequestRecord &      xcr
)
{
   /*===========================================================================*
    *  Locals                                                                   *
    *===========================================================================*/

    int                     ArgN          = 1;
    SAPDB_Bool              rc = true;
    RTE_SystemRc            sysRc = RTE_SYSTEMRC_NO_ERROR;
    SAPDB_UInt4             lCommand;
    SAPDB_UInt4             lParam;
    SAPDB_Char const *      pszServerDB;

   /*===========================================================================*
    *  Instructions                                                             *
    *===========================================================================*/
   
    SAPDBTRACE_ROUTINE_DEBUG("GetRequestFromParameter", Console_Trace, 1);

    memset ( &xcr, 0, sizeof(xcr) );
    xcr.lInterval     = 0;
    xcr.lRepeatCount  = 1;
    xcr.lCommand      = COM_UNDEF;
    xcr.lParam        = 0;
    xcr.lExtParam     = 0;

    if (ARGS(argc, ArgN))
    {
        if ((( argv [ ArgN ] [ 0 ] == '-' ) || ( argv [ ArgN ] [ 0 ] == '/' )) &&
            (( argv [ ArgN ] [ 1 ] == '?' ) || ( argv [ ArgN ] [ 1 ] == 'h' )) &&
            ( argv [ ArgN ] [ 2 ] == '\0'))
        {
            xcr.lCommand = COM_HELP;
        }
        else
        {
            //
            // --- get command and server database name
            //
            xcr.lCommand = GetComParam (argv[ArgN]);

            if (( xcr.lCommand == COM_UNDEF ) || ( xcr.lCommand != COM_HELP ) || ( argc > 2 ))
            {
                pszServerDB = argv[ArgN++];

                if ( ARGS(argc, ArgN) )
                {
                    xcr.lCommand = GetComParam ( argv[ArgN++] );
                    if ( xcr.lCommand == COM_UNDEF )
                    {
                        rc = false;
                    }
                }
                else
                {
                    rc = false;
                }
            }
            else if ( ARGS(argc, ++ArgN) )
            {
                lCommand = GetComParam ( argv[ArgN] );

                if ( lCommand != COM_UNDEF ) // - is there another command?
                {
                    pszServerDB = argv[ArgN - 1];
                    ArgN++;
                }
            }

            if (rc && ( xcr.lCommand != COM_HELP ))
            {
                switch ( xcr.lCommand )
                {
                case COM_TEST01:
                case COM_TEST02:
                case COM_TEST03:
                    break;

                case COM_TRACE_FLUSH:
                    break;

                case COM_PAGECACHE_RELEASE:
                    break;

                case COM_DIAGNOSE_PAGECACHE:
                    break;

                case COM_TRACELEVEL:
                    if ( ARGS(argc, ArgN) )
                    {
                        lParam = GetTraceTopic ( argv[ArgN++] );

                        if ( lParam != PARA_NONE )
                        {
                            xcr.lParam |= lParam;
                            rc = GetNumParam (argv[ArgN++], &xcr.lValue);
                            if (xcr.lValue < 0 ||  xcr.lValue > 9)
                            {
                                rc = false;
                            }
                        }
                        else
                        {
                            rc = false;
                        }
                    }
                    else
                    {
                        rc = false;
                    }
                    break;

                case COM_UKT_AFFINITY:
                    if ( ARGS(argc, ArgN) )
                    {
                        rc = GetIdxParam ( argv[ArgN++], 'u', &xcr.lParam );
                    }
                    else
                        rc = false;
                    break;

                    // PTS 1107070
                case COM_PROCESSOR_MASK:
                    if ( ARGS(argc, ArgN) )
                    {
                        rc = GetProcessorAffinityMask ( argv[ArgN++], &xcr.lParam );

                        if (rc && ARGS(argc, ArgN))
                        {
                            if (!e92_get_tt_to_show (argv[ArgN], (int *)&xcr.lExtParam))
                            {
                                rc = false;
                            }
                        }
                    }
                    else
                    {
                        rc = false;
                    }

                    break;

                case COM_CANCEL:
                    if ( ARGS(argc, ArgN) )
                    {
                        rc =  GetIdxParam( argv[ArgN++], 't', &xcr.lParam );
                    }
                    else
                    {
                        rc = false;
                    }
                    break;

                case COM_KILL  :
                    if ( ARGS(argc, ArgN) )
                    {
                        rc = GetIdxParam ( argv[ArgN++], 't', &xcr.lParam );
                    }
                    else
                    {
                        rc = false;
                    }
                    break;

                case COM_DROP  :
                    if ( !ARGS(argc, ArgN) ||
                        !e92_strcont( (char *)XCONS_CMD_TABLES, StringToUpper(argv[ArgN++]),
                        XCONS_MIN_CMD_LEN_TABLES ))
                    {
                        rc = false;
                    }
                    break;

                case COM_CRASH_DUMP:
                    if ( !ARGS(argc, ArgN) ||
                        !e92_strcont( (char *)XCONS_CMD_DUMP, StringToUpper(argv[ArgN++]),
                        XCONS_MIN_CMD_LEN_DUMP ))
                    {
                        rc = false;
                    }
                    break;

                case COM_MINI_DUMP:
                    if ( !ARGS(argc, ArgN) ||
                        !e92_strcont( (char *)XCONS_CMD_DUMP, StringToUpper(argv[ArgN++]),
                        XCONS_MIN_CMD_LEN_DUMP ))
                    {
                        rc = false;
                    }
                    break;

                case COM_TIME_MEASURE :
                    if ( ARGS(argc, ArgN) )
                    {
                        lParam = GetTimeParam ( argv[ArgN++] );

                        if ( lParam != PARA_NONE )
                        {
                            xcr.lParam |= lParam;
                        }
                        else
                        {
                            rc = false;
                        }
                    }
                    else
                    {
                        rc = false;
                    }
                    break;

                case COM_RESET :
                    if ( ARGS(argc, ArgN) )
                    {
                        lParam = GetShowResParam( argv[ArgN++], xcr.lCommand );

                        if ( lParam != PARA_NONE )
                        {
                            xcr.lParam |= lParam;
                        }
                        else
                        {
                            rc = false;
                        }
                    }
                    else
                    {
                        rc = false;
                    }
                    break;

                case COM_ERRIOR :
                case COM_ERRIOW :
                    if ( ARGS(argc, ArgN) )
                    {
                        rc = GetNumParam ( argv[ArgN++], &xcr.lParam );
                    }
                    else
                    {
                        rc = false;
                    }
                    break;

                case COM_SET_DEBUGLEVEL :
                    if ( ARGS(argc, ArgN) )
                    {
                        rc = GetNumParam ( argv[ArgN++], &xcr.lParam );
                    }
                    else
                    {
                        rc = false;
                    }
                    break;

                case COM_GETPARAM :

                    if ( ARGS(argc, ArgN) )
                    {
                        xcr.pszParam = argv[ArgN++];
                    }
                    else
                    {
                        *xcr.pszParam = '\0';
                    }

                    break;

                case COM_SHOW_UPDATE :

                    // --- command parameter found?
                    if (( ARGS(argc, ArgN)) && ( !isdigit(argv[ArgN][0]) ))
                    {
                        xcr.lParam = 0;

                        do
                        {
                            lParam = GetShowResParam (argv[ArgN++], xcr.lCommand);

                            if ( lParam != PARA_NONE )
                            {
                                if (( lParam == PARA_TASK_COUNTS ) && (ARGS(argc, ArgN)))
                                {
                                    if (!GetIdxParam(argv[ArgN], 't', &xcr.lExtParam))
                                    { 
                                        if (e92_get_tt_to_show (argv[ArgN], (int *)&xcr.lExtParam))
                                        {
                                            ArgN++;
                                        }
                                    }
                                    else
                                    {
                                        xcr.lExtParam = -xcr.lExtParam ;
                                        ArgN++;
                                    }
                                }
                                else if (( lParam == PARA_RWREGIONS ) && (ARGS(argc, ArgN)))
                                {
                                    if (0 != strcmp("all",argv[ArgN]))
                                    { 
                                        if (e92_get_tt_to_show (argv[ArgN], (int *)&xcr.lExtParam))
                                        {
                                            ArgN++;
                                        }
                                    }
                                    else
                                    {
                                        xcr.lExtParam = 27 ;
                                        ArgN++;
                                    }
                                }
                                else if (lParam == PARA_TASKS || lParam == PARA_ACTIVE || lParam == PARA_RUNNABLE  || lParam == PARA_TASK_MOVE)
                                {
                                    xcr.lExtParam = SHOW_TT_ALL_EO92;
                                    if ( ARGS(argc, ArgN) )
                                    {
                                        if (e92_get_tt_to_show (argv[ArgN], (int *)&xcr.lExtParam))
                                        {
                                            ArgN++;
                                        }
                                    }
                                }

                                xcr.lParam |= lParam;
                            }
                            else
                            {
                                rc = false;
                            }
                        }
                        while (rc && (ARGS(argc, ArgN)) && (!isdigit(argv[ArgN][0])));
                    }
                    else
                    {
                        xcr.lParam = PARA_DEFAULT;
                    }

                    // --- try to get interval and repeat count
                    if (rc && ARGS(argc, ArgN))
                    {
                        xcr.lRepeatCount = -1;

                        // --- try to get interval
                        rc = GetNumParam (argv[ArgN++], &xcr.lInterval);

                        if (rc)
                        {
                            if ( xcr.lInterval < 0 )
                            {
                                rc = false;
                            }
                            else if ( ARGS(argc, ArgN))
                            {
                                // --- try to get repeat count
                                rc = GetNumParam (argv[ArgN++], &xcr.lRepeatCount);

                                if ( xcr.lRepeatCount < 1 )
                                {
                                    rc = false;
                                }
                            }
                        }
                    }
                    break;

                case COM_SET_LB_CHK:
                    if ( ARGS(argc, ArgN) )
                    {
                        rc = GetNumParam (argv[ArgN++], &xcr.lParam);
                    }
                    else
                    {
                        rc = false;
                    }
                    break;

                case COM_DEBUG_TASK:
                    if (ARGS(argc, ArgN))
                    {
                        rc = GetIdxParam ( argv[ArgN++], 't', &xcr.lParam );
                    }
                    else
                    {
                        rc = false;
                    }
                    break;

                case COM_DEBUG_BREAK:
                    if ( ARGS(argc, ArgN) )
                    {
                        xcr.lParam = GetSetClearParam ( argv[ArgN++] );

                        if ( xcr.lParam == PARA_SET )
                        {
                            if ( ARGS(argc, ArgN) )
                            {
                                rc = GetNumParam (argv[ArgN++], &xcr.lValue);
                            }
                            else
                            {
                                rc = false;
                            }
                        }
                        else if ( xcr.lParam != PARA_CLEAR )
                        {
                            rc = false;
                        }
                    }
                    else
                    {
                        rc = false;
                    }

                    break;
                default :
                    rc = false;
                }

                ArgN++;
            }
        }
    }
    else
    {
        rc = false;
    }

    //
    // --- display help info ?
    //
    if (( ARGS( argc, ArgN ) )     ||
        ( !rc  )        ||
        ( xcr.lCommand == COM_HELP   ) ||
        ( xcr.lCommand == COM_UNDEF  ))
    {
        return false;
    }
  

    if (pszServerDB == NULL)
    {
#if defined (_WIN32)
        sysRc = GETENV(SERVERDB_ENV_VAR, &pszServerDB);

        if (sysRc != RTE_SYSTEMRC_NO_ERROR  ||  *pszServerDB == '\0')
        {
            return false;
        }
#else
        return false;
#endif
    }

    strcpy (xcr.serverDB, pszServerDB);
    return true;
}

/*---------------------------------------------------------------------------*/

static SAPDB_Bool GetNumParam  
( 
    SAPDB_Char *    pszParam,
    SAPDB_Long *    plNum  
)
{
    SAPDBTRACE_ROUTINE_DEBUG("GetNumParam", Console_Trace, 1);
    SAPDB_Char *      pszTmp;
  

    // --- is the parameter valid?
    pszTmp = pszParam;
    while ( *pszTmp != '\0')
    {
        if (!isdigit ( *pszTmp ))
        {
            return false;
        }

        pszTmp++;
    }

    *plNum = atol (pszParam);
    return true;
  }

/*---------------------------------------------------------------------------*/

static SAPDB_Bool GetIdxParam  
(
    SAPDB_Char *    pszParam,
    SAPDB_Char      cPrefix,
    SAPDB_Long *    plNum
)
{
    SAPDBTRACE_ROUTINE_DEBUG("GetIdxParam", Console_Trace, 1);

    SAPDB_Char *      pszTmp;
    SAPDB_Char *      pszParamTmp;

    // --- is the parameter valid?
    if ( *pszParam == _toupper(cPrefix) || *pszParam == cPrefix )
    {
        pszTmp      = pszParam + 1;
        pszParamTmp = pszParam + 1;
    }
    else
    {
        pszTmp      = pszParam;
        pszParamTmp = pszParam;
    }

    while ( *pszTmp != '\0')
    {
        if (!isdigit ( *pszTmp ))
        {
            return false;
        }

        pszTmp++;
    }

    *plNum = atol (pszParamTmp);

    return true;
}

/*---------------------------------------------------------------------------*/
static SAPDB_Bool GetProcessorAffinityMask 
(
    SAPDB_Char *    pszParam,
    SAPDB_Long *    ProcessAffinityMask  
)
{
    SAPDBTRACE_ROUTINE_DEBUG("GetProcessorAffinityMask", Console_Trace, 1);

    SAPDB_Bool     rc = true;
    SAPDB_Long     i, ParamLen ;


    for ( i = ParamLen = strlen(pszParam), *ProcessAffinityMask = 0 ; 
          i > 0 && rc; i-- )
    {
	    if  ( pszParam[i-1] == '1' ) 
        {
            *ProcessAffinityMask |= ( 1L << ( ParamLen-i) ) ;
        }
	    else
        {
	        if ( pszParam[i-1] != '0' )
            {
                rc = false;
            }
        }
    }

    return rc;
}

/*---------------------------------------------------------------------------*/

static SAPDB_Char *  StringToUpper  
( 
    SAPDB_Char *    string 
)
{
    SAPDB_Char * tmpString = string;
    while ( *tmpString )
    {
        *tmpString = toupper(*tmpString);
        ++tmpString;
    }

    return string;
}

/*---------------------------------------------------------------------------*/

static SAPDB_UInt4  GetTimeParam  
( 
    SAPDB_Char *    pszParam 
)
{
    SAPDBTRACE_ROUTINE_DEBUG("GetTimeParam", Console_Trace, 1);

    StringToUpper (pszParam);

    if ( e92_strcont((char *)XCONS_CMD_ENABLE, pszParam, XCONS_MIN_CMD_LEN_ENABLE))
    {
        return PARA_ENABLE;
    }

    if ( e92_strcont((char *)XCONS_CMD_DISABLE, pszParam, XCONS_MIN_CMD_LEN_DISABLE ))
    {
        return PARA_DISABLE;
    }

    return PARA_NONE;
}

/*---------------------------------------------------------------------------*/

static SAPDB_UInt4  GetTraceTopic  
( 
    SAPDB_Char *    pszParam 
)
{
    SAPDBTRACE_ROUTINE_DEBUG("GetTraceTopic", Console_Trace, 1);

    StringToUpper (pszParam);

#ifdef SAPDB_SLOW
    if ( e92_strcont((char *)XCONS_CMD_TRACE_XCONS, pszParam, XCONS_MIN_CMD_LEN_TRACE_XCONS))
    {
        return PARA_TRACE_XCONS;
    }
#endif

    if ( e92_strcont((char *)XCONS_CMD_TRACE_CONSOLE, pszParam, XCONS_MIN_CMD_LEN_TRACE_CONSOLE ))
    {
        return PARA_TRACE_CONSOLE;
    }

    return PARA_NONE;
}

/*---------------------------------------------------------------------------*/

static SAPDB_UInt4  GetSetClearParam  
( 
    SAPDB_Char *    pszParam 
)
{
    SAPDBTRACE_ROUTINE_DEBUG("GetSetClearParam", Console_Trace, 1);

    StringToUpper (pszParam);

    if ( e92_strcont((char *)XCONS_CMD_SET, pszParam, XCONS_MIN_CMD_LEN_SET ))
    {
        return PARA_SET;
    }

    if (e92_strcont ((char *)XCONS_CMD_CLEAR, pszParam, XCONS_MIN_CMD_LEN_CLEAR))
    {
        return PARA_CLEAR;
    }

    return PARA_NONE;
}

/*---------------------------------------------------------------------------*/

static SAPDB_UInt4  GetShowResParam  
( 
    SAPDB_Char *    pszParam,
    SAPDB_UInt4     command 
)
{
    SAPDBTRACE_ROUTINE_DEBUG("GetShowResParam", Console_Trace, 1);

    StringToUpper (pszParam);

    if (e92_strcont ((char *)XCONS_CMD_IO, pszParam, XCONS_MIN_CMD_LEN_IO))
    {
        return PARA_IO;
    }

    if (e92_strcont ((char *)XCONS_CMD_AIO, pszParam, XCONS_MIN_CMD_LEN_AIO))
    {
        return PARA_AIO;
    }

    if (e92_strcont ((char *)XCONS_CMD_MOVE_INFO, pszParam, XCONS_MIN_CMD_LEN_MOVE_INFO))
    {
        return PARA_MOVE_INFO;
    }

    if (e92_strcont ((char *)XCONS_CMD_TASK_MOVE, pszParam, XCONS_MIN_CMD_LEN_TASK_MOVE))
    {
        return PARA_TASK_MOVE;
    }

    if (e92_strcont ((char *)XCONS_CMD_AWE, pszParam, XCONS_MIN_CMD_LEN_AWE))
    {
        return PARA_AWE;
    }

    if (e92_strcont ((char *)XCONS_CMD_RWREGIONS, pszParam, XCONS_MIN_CMD_LEN_RWREGIONS))
    {
        return PARA_RWREGIONS;
    }

    if (e92_strcont ((char *)XCONS_CMD_REGIONS, pszParam, XCONS_MIN_CMD_LEN_REGIONS))
    {
        return PARA_REGIONS;
    }

    if (e92_strcont ((char *)XCONS_CMD_UKT_SLEEP_STAT, pszParam, XCONS_MIN_CMD_LEN_UKT_SLEEP_STAT))
    {
        return PARA_UKT_SL_STAT;
    }

    if (e92_strcont ((char *)XCONS_CMD_ALL, pszParam, XCONS_MIN_CMD_LEN_ALL))
    {
        return PARA_ALL;
    }

    if (e92_strcont ((char *)XCONS_CMD_THREAD_TIMES, pszParam, XCONS_MIN_CMD_LEN_THREAD_TIMES))
    {
        return PARA_THREAD_TIMES;
    }

    if (e92_strcont ((char *)XCONS_CMD_UKT_QUEUES, pszParam, XCONS_MIN_CMD_LEN_UKT_QUEUES))
    {
        return PARA_UKT_QUEUES;
    }

    if (e92_strcont ((char *)XCONS_CMD_SUSPENDS, pszParam, XCONS_MIN_CMD_LEN_SUSPENDS))
    {
        return PARA_SUSPENDS;
    }

    if (e92_strcont ((char *)XCONS_CMD_TASK_COUNTS, pszParam, XCONS_MIN_CMD_LEN_TASK_COUNTS))
    {
        return PARA_TASK_COUNTS;
    }

    if (e92_strcont ((char *)XCONS_CMD_QUEUE_STAT, pszParam, XCONS_MIN_CMD_LEN_QUEUE_STAT))
    {
        return PARA_QUEUE_STAT;
    }

    if (command == COM_SHOW_UPDATE)
    {
        if (e92_strcont ((char *)XCONS_CMD_VERSIONS, pszParam, XCONS_MIN_CMD_LEN_VERSIONS))
        {
            return PARA_VERSIONS;
        }

        if (e92_strcont ((char *)XCONS_CMD_STORAGE,  pszParam, XCONS_MIN_CMD_LEN_STORAGE))
        {
            return PARA_STORAGE;
        }

        if (e92_strcont ((char *)XCONS_CMD_RTE, pszParam, XCONS_MIN_CMD_LEN_RTE))
        {
            return PARA_RTE;
        }

        if (e92_strcont ((char *)XCONS_CMD_TASKS, pszParam, XCONS_MIN_CMD_LEN_TASKS))
        {
            return PARA_TASKS;
        }

        if (e92_strcont ((char *)XCONS_CMD_ACTIVE_TASKS, pszParam, XCONS_MIN_CMD_LEN_ACTIVE_TASKS))
        {
            return PARA_ACTIVE;
        }

        if (e92_strcont ((char *)XCONS_CMD_RUNNABLE_TASKS, pszParam, XCONS_MIN_CMD_LEN_RUNNABLE_TASKS))
        {
            return PARA_RUNNABLE;
        }

        if (e92_strcont ((char *)XCONS_CMD_STATE, pszParam, XCONS_MIN_CMD_LEN_STATE))
        {
            return PARA_STATE;
        }
/*
        if (e92_strcont ((char *)XCONS_CMD_TASK_STAT, pszParam, XCONS_MIN_CMD_LEN_TASK_STAT))
        {
            return PARA_TASK_STAT;
        }
*/
        if (e92_strcont ((char *)XCONS_CMD_TOTAL_COUNTS, pszParam, XCONS_MIN_CMD_LEN_TOTAL_COUNTS))
        {
            return PARA_TOTAL_COUNTS;
        }

        if (e92_strcont ((char *)XCONS_CMD_CONNECTIONS, pszParam, XCONS_MIN_CMD_LEN_CONNECTIONS))
        {
            return PARA_CONNECTIONS;
        }
    }

    return ( PARA_NONE );
}

/*---------------------------------------------------------------------------*/

static SAPDB_UInt4  GetComParam  
(
    SAPDB_Char  *pszParam
)
{
    SAPDBTRACE_ROUTINE_DEBUG("GetComParam", Console_Trace, 1);

    StringToUpper (pszParam);
//*************************************************************************
//Test
//*************************************************************************
if ( e92_strcont( (char *)XCONS_CMD_TEST01, pszParam, XCONS_MIN_CMD_LEN_TEST01 ))
        return(COM_TEST01);
if ( e92_strcont( (char *)XCONS_CMD_TEST02, pszParam, XCONS_MIN_CMD_LEN_TEST02 ))
        return(COM_TEST02);
if ( e92_strcont( (char *)XCONS_CMD_TEST03, pszParam, XCONS_MIN_CMD_LEN_TEST03 ))
        return(COM_TEST03);

//*************************************************************************


    if (e92_strcont ((char *)XCONS_CMD_HELP1, pszParam, XCONS_MIN_CMD_LEN_HELP1))
    {
        return COM_HELP;
    }

    if (e92_strcont ((char *)XCONS_CMD_HELP2, pszParam, XCONS_MIN_CMD_LEN_HELP2))
    {
        return COM_HELP;
    }

    if (e92_strcont ((char *)XCONS_CMD_ERRIOR, pszParam, XCONS_MIN_CMD_LEN_ERRIO))
    {
        return COM_ERRIOR;
    }

    if (e92_strcont ((char *)XCONS_CMD_ERRIOW, pszParam, XCONS_MIN_CMD_LEN_ERRIO))
    {
        return COM_ERRIOW;
    }

    if (e92_strcont ((char *)XCONS_CMD_CANCEL, pszParam, XCONS_MIN_CMD_LEN_CANCEL))
    {
        return COM_CANCEL;
    }

    if (e92_strcont ((char *)XCONS_CMD_DEBUG_TASK, pszParam, XCONS_MIN_CMD_LEN_DEBUG_TASK))
    {
        return COM_DEBUG_TASK;
    }

    if ( e92_strcont( (char *)XCONS_CMD_SET_LB_CHK, pszParam, XCONS_MIN_CMD_SET_LB_CHK_TASK ))
    {
        return( COM_SET_LB_CHK );
    }

    if (e92_strcont ((char *)XCONS_CMD_DEBUG_BREAK, pszParam, XCONS_MIN_CMD_LEN_DEBUG_BREAK))
    {
        return COM_DEBUG_BREAK;
    }

    if (e92_strcont ((char *)XCONS_CMD_UKT_AFFINITY, pszParam, XCONS_MIN_CMD_LEN_UKT_AFFINITY))
    {
        return COM_UKT_AFFINITY;
    }

   // PTS 1107070
    if (e92_strcont ((char *)XCONS_CMD_PROCESSOR_MASK, pszParam, XCONS_MIN_CMD_LEN_PROCESSOR_MASK))
    {
        return COM_PROCESSOR_MASK;
    }

    if (e92_strcont ((char *)XCONS_CMD_KILL, pszParam, XCONS_MIN_CMD_LEN_KILL))
    {
        return COM_KILL;
    }

    if (e92_strcont ((char *)XCONS_CMD_SHOW, pszParam, XCONS_MIN_CMD_LEN_SHOW))
    {
        return COM_SHOW_UPDATE;
    }

    if (e92_strcont ((char *)XCONS_CMD_TIME_MEASURE,  pszParam, XCONS_MIN_CMD_LEN_TIME_MEASURE))
    {
        return COM_TIME_MEASURE;
    }

    if (e92_strcont ((char *)XCONS_CMD_RESET_CNT, pszParam, XCONS_MIN_CMD_LEN_RESET_CNT))
    {
        return COM_RESET;
    }

    if (e92_strcont ((char *)XCONS_CMD_GETPARAM, pszParam, XCONS_MIN_CMD_LEN_GETPARAM))
    {
        return COM_GETPARAM;
    }
/*
    if (e92_strcont ((char *)XCONS_CMD_PUTPARAM , pszParam, XCONS_MIN_CMD_LEN_PUTPARAM))
    {
        return COM_PUTPARAM;
    }
*/
    if (e92_strcont ((char *)XCONS_CMD_DEBUGLEVEL , pszParam, XCONS_MIN_CMD_LEN_DEBUGLEVEL))
    {
        return COM_SET_DEBUGLEVEL;
    }

    if ( e92_strcont ((char *)XCONS_CMD_CRASH, pszParam, XCONS_MIN_CMD_LEN_CRASH))
    {
            return COM_CRASH_DUMP;
    }

    if ( e92_strcont ((char *)XCONS_CMD_MINI, pszParam, XCONS_MIN_CMD_LEN_MINI))
    {
            return COM_MINI_DUMP;
    }

    if ( e92_strcont((char *)XCONS_CMD_TRACE_FLUSH, pszParam, XCONS_MIN_CMD_LEN_TRACE_FLUSH ))
    {
        return( COM_TRACE_FLUSH );
    }
  
    if ( e92_strcont((char *)XCONS_CMD_PAGECACHE_RELEASE, pszParam, XCONS_MIN_CMD_LEN_PAGECACHE_RELEASE ))
    {
        return( COM_PAGECACHE_RELEASE );
    }
  
    if ( e92_strcont((char *)XCONS_CMD_DIAGNOSE_PAGECACHE, pszParam, XCONS_MIN_CMD_LEN_DIAGNOSE_PAGECACHE ))
    {
        return( COM_DIAGNOSE_PAGECACHE );
    }

    if ( e92_strcont((char *)XCONS_CMD_TRACELEVEL, pszParam, XCONS_MIN_CMD_LEN_TRACELEVEL ))
    {
        return( COM_TRACELEVEL );
    }

    return COM_UNDEF;
}

/*===========================================================================*
 *  extern "C" call stubs                                                    *
 *===========================================================================*/


externC SAPDB_Bool  RTECons_ProcessStandardRequest  
(
    int     argc, 
    char*   argv[]
)
{
   /*===========================================================================*
    *  Locals                                                                   *
    *===========================================================================*/
    RTE_ConsoleRequestRecord        request;
    SAPDBErr_MessageList            messageList;
    SAPDB_UInt4                     rc = RTE_CONS_NO_ERROR;
    RTECons_Communication::CommRC   rcComm = RTECons_Communication::NoError;
   /*===========================================================================*
    *  Instructions                                                             *
    *===========================================================================*/

#ifdef SAPDB_SLOW
    if (!InitTrace ("xcons.trc", messageList))
    {
		RTE_Message (messageList);
		messageList.ClearMessageList ();
    }
#endif

    if (!GetRequestFromParameter (argc, argv, request))
    {
        printf  (XCONS_USAGE);
        return false;
    }

    RTECons_StandardSession         xConsStandardSession (request);

#if defined (HAS_NO_PTHREAD_MUTEX_PROCESS_SCOPE)
    // Create Semaphore Timeout List
    if (!RTE_ConsoleSemaphoreTimeoutList::CreateInstance(KGS->consoleSemTimeout/1000, 1, messageList))
    {
        RTE_Message (messageList, MSG_DIAG_CONSOLE);
        return false;
    }
#endif
    rcComm = xConsStandardSession.Create (messageList);
    if (RTECons_Communication::NoError == rcComm)
    {
        if (RTE_CONS_NO_ERROR == (rc = xConsStandardSession.Connect (messageList)))
        {
            rc = xConsStandardSession.RequestToStandardOutput (messageList);

            if (RTE_CONS_NO_ERROR != rc)
            {
                if (RTE_CONS_TIMEOUT != rc)
                {
                    RTE_Message (messageList, MSG_DIAG_CONSOLE);
                }
            }

            if (RTE_CONS_NO_ERROR != (rc = xConsStandardSession.Release (messageList)))
            {
                if (RTE_CONS_TIMEOUT != rc)
                {
                    RTE_Message (messageList, MSG_DIAG_CONSOLE);
                }
            }
        }
        else
        {
            if (RTE_CONS_TIMEOUT != rc)
            {
                RTE_Message (messageList, MSG_DIAG_CONSOLE);
            }
        }
    }
    else
    {
        if (RTECons_Communication::Offline == rcComm)
        {
            printf ( XCONS_SERVERDB, request.serverDB);
            printf ( XCONS_SHOW_STATE_INFO, XCONS_SHOW_STATE_OFFLINE );
        }
        else
        {
            RTE_Message (messageList, MSG_DIAG_CONSOLE);
            rc = RTE_CONS_ERROR;
        }
    }
 
    //Timeout Thread on HAS_NO_PTHREAD_MUTEX_PROCESS_SCOPE is not terminated explicitely at the moment. The thread terminates
    //if the hole process terminates. If it becomes necessary to terminate the thread a special element
    //could be enqueued in the timeout list of the timeout thread. But at the moment there is no need for it.

    return (rc == RTE_CONS_NO_ERROR);
}

