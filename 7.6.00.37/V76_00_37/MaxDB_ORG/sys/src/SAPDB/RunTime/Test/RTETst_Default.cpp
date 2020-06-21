/*!***************************************************************************

  module      : RTETst_Default.cpp

  -------------------------------------------------------------------------

  responsible : JoergM

  special area: RTE KernelMain
  description : Runtime default test code

  last changed: 2000-07-11  13:40
  first created:2000-06-06  14:25

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2000-2005 SAP AG




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




*****************************************************************************/

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "RunTime/Test/RTETst_Default.hpp"
#include "RunTime/Tasking/RTETask_ITask.hpp"
#include "RunTime/Test/RTETst_IParser.hpp"
#include "RunTime/RTE_Message.hpp"
#include "SAPDBCommon/SAPDB_string.h"
#include "RunTime/RTE_Test_Messages.hpp"
#include "ggg00.h"
/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define RTETST_ECHO_COMMAND "echo"
#define RTETST_SLEEP_COMMAND "sleep"
#ifdef WANT_TEST_CRASH
#define RTETST_CRASH_COMMAND "crash"
#endif
/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/

static SAPDB_Bool Test_Default( RTETask_ITask        *pTask
                              , SAPDB_UTF8 const     * /* argumentString */
                              , SAPDB_Int4             /* argumentLength */
                              , RTETst_IContext      * /* pTestContext   */
                              , SAPDB_Int4 &errCode
                              , Msg_List &errList );

static SAPDB_Bool Test_TaskEcho( RTETask_ITask        * pTask
                               , SAPDB_UTF8 const     * argumentString
                               , SAPDB_Int4             /* argumentLength */
                               , RTETst_IContext      * /* pTestContext   */
                               , SAPDB_Int4 &errCode
                               , Msg_List &errList );

static SAPDB_Bool Test_TaskSleep( RTETask_ITask        * pTask
                                , SAPDB_UTF8 const     * argumentString
                                , SAPDB_Int4             /* argumentLength */
                                , RTETst_IContext      * /* pTestContext   */
                                , SAPDB_Int4 &errCode
                                , Msg_List &errList );
#ifdef WANT_TEST_CRASH
extern void RTE_Crash(const Msg_List &msgList);
static SAPDB_Bool Test_TaskCrash( RTETask_ITask        * pTask
                                , SAPDB_UTF8 const     * argumentString
                                , SAPDB_Int4             /* argumentLength */
                                , RTETst_IContext      * /* pTestContext   */
                                , SAPDB_Int4 &errCode
                                , Msg_List &errList );
#endif
/*===========================================================================*
 *  BEGIN OF CODE                                                            *
 *===========================================================================*/

SAPDB_Bool RTETst_RegisterDefault(Msg_List &errList)
{
    SAPDB_Bool ok = true;
    /* Setup default RTE test and initialize RTE_TestParser singleton */
    ok = RTETst_IParser::Register((SAPDB_UTF8 const *)"", 0, Test_Default, errList);
    if ( !ok )
    {  
        return false;
    }

    ok = RTETst_IParser::Register((SAPDB_UTF8 const *)RTETST_ECHO_COMMAND, 0, Test_TaskEcho, errList);
    if ( !ok )
    {  
        return false;
    }

    ok = RTETst_IParser::Register((SAPDB_UTF8 const *)RTETST_SLEEP_COMMAND, 0, Test_TaskSleep, errList);
    if ( !ok )
    {  
        return false;
    }

#ifdef WANT_TEST_CRASH
    ok = RTETst_IParser::Register((SAPDB_UTF8 const *)RTETST_CRASH_COMMAND, 0, Test_TaskCrash, errList);
    if ( !ok )
    {  
        return false;
    }
#endif
    return true;
}

/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/

static SAPDB_Bool Test_Default( RTETask_ITask        *pTask
                                , SAPDB_UTF8 const     * /* argumentString */
                                , SAPDB_Int4             /* argumentLength */
                                , RTETst_IContext      * /* pTestContext   */
                                , SAPDB_Int4 &errCode
                                , Msg_List &errList )
{
    SAPDB_Int4 countDown = 3;

    while ( !pTask->CancelPending() && --countDown > 0 )
	{
#if defined(_WIN32)
        	Sleep(2000);
#else
	        sleep(2);
#endif
    }

#if defined(_WIN32)
    Sleep(2000);
#else
	sleep(2);
#endif

    // error reporting....

    if ( countDown > 0 )
    {
        if ( pTask->CancelPending() )
        {
        	RTE_Message( SAPDBErr_MessageList("RTETEST"
                                             , __CONTEXT__
                                             , RTEINFO_TEST_WAS_CANCELLED
                                             , "SLEEP(3+2)"
                                             , ToStr(pTask->ID()) ));
            return true;
        }

        errList = SAPDBErr_MessageList("RTETEST"
                                     , __CONTEXT__
                                     , RTEERR_TEST_SLEEP_WAS_ABORTED
                                     , ToStr(pTask->ID()) );
        errCode = e_invalid;
    }
    else
    {
        errList = SAPDBErr_MessageList("RTETEST"
                                     , __CONTEXT__
                                     , SAPDBErr_MessageList::Info
                                     , RTEERR_TEST_TIMEOUT
                                     , "SLEEP(3+2)"
                                     , ToStr(pTask->ID()) );
        errCode = e_command_timeout;
    }

    return false;
}

//----------------------------------------

static SAPDB_Bool Test_TaskSleep( RTETask_ITask        * pTask
                              , SAPDB_UTF8 const     * argumentString
                              , SAPDB_Int4             /* argumentLength */
                              , RTETst_IContext      * /* pTestContext   */
                              , SAPDB_Int4 &errCode
                              , Msg_List &errList )
{
    SAPDB_Int4 sleepTime = 0;

    SAPDB_Char const *pTime = (SAPDB_Char const *)(argumentString + SAPDB_strlen(RTETST_SLEEP_COMMAND));

    while ( *pTime == ' ' ) ++pTime;
    while ( *pTime )
    {
        SAPDB_Int4 digit = *pTime - '0';
        if ( digit >= 0 && digit <= 9 )
        {
            sleepTime *= 10; sleepTime += digit;
            ++pTime;
        }
        else
        {
            break;
        }
    }

    RTE_Message( SAPDBErr_MessageList("RTETEST"
                                     , __CONTEXT__
                                     , RTEINFO_TEST
                                     , (SAPDB_Char const *)argumentString
                                     , ToStr(pTask->ID()) ) );

    pTask->Sleep(sleepTime);

    RTE_Message( SAPDBErr_MessageList("RTETEST"
                                     , __CONTEXT__
                                     , RTEINFO_TEST_WOKE_UP
                                     , ToStr(pTask->ID()) ) );
    errCode = e_ok;
    return true;
}

//----------------------------------------

static SAPDB_Bool Test_TaskEcho( RTETask_ITask        *pTask
                               , SAPDB_UTF8 const     *argumentString
                               , SAPDB_Int4             /* argumentLength */
                               , RTETst_IContext      * /* pTestContext   */
                               , SAPDB_Int4 & errCode
                               , Msg_List &errList )
{
    errList = SAPDBErr_MessageList("RTETEST"
                                 , __CONTEXT__
                                 , Msg_List::Info
                                 , 19823
                                 , "RTE_Test Task T%s:%s"
                                 , 2
                                 , ToStr(pTask->ID())
                                 , (SAPDB_Char const *)(argumentString+strlen(RTETST_ECHO_COMMAND)) );
    errCode = e_illegal_escape_sequence;
    return false;
}

//----------------------------------------

#ifdef WANT_TEST_CRASH
static SAPDB_Bool Test_TaskCrash( RTETask_ITask        * pTask
                              , SAPDB_UTF8 const     * argumentString
                              , SAPDB_Int4             /* argumentLength */
                              , RTETst_IContext      * /* pTestContext   */
                              , SAPDB_Int4 &errCode
                              , Msg_List &errList )
{
    SAPDBErr_MessageList visibleMessage = SAPDBErr_MessageList("RTETEST"
                                 , __CONTEXT__
                                 , Msg_List::Info
                                 , 42
                                 , "This message must be visible in registry dump since it was never used for output"
                                 , 0 );
    SAPDBErr_MessageList message = SAPDBErr_MessageList("RTETEST"
                                 , __CONTEXT__
                                 , Msg_List::Info
                                 , 19823
                                 , "RTE_Test Wanted Crash Task T%s:%s"
                                 , 2
                                 , ToStr(pTask->ID())
                                 , (SAPDB_Char const *)(argumentString+strlen(RTETST_CRASH_COMMAND)) );
    RTE_Crash(message);
    return true;
}
#endif
/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
