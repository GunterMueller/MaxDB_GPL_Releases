/*!
  @file           RTETst_Parser.cpp
  @author         JoergM
  @ingroup        Runtime
  @brief          Runtime Test Parser implementation

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

*/

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "RunTime/Test/RTETst_Parser.hpp"
#include "RunTime/MemoryManagement/RTEMem_RteAllocator.hpp"
#include "RunTime/System/RTESys_AtomicOperation.hpp"
#include "RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "SAPDBCommon/SAPDB_string.h"
#include "SAPDBCommon/SAPDB_ToString.hpp"
#include "RunTime/RTE_Messages.hpp"
#include "RunTime/RTE_Test_Messages.hpp"
#include "ggg00.h"
#include "SAPDBCommon/SAPDB_MemCopyMoveNoCheck.hpp"
/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/
RTETst_Parser * RTETst_Parser::m_Instance = 0; ///< the test parser implementation

    /*!
      @brief Test parser singleton access
     */
RTETst_Parser & RTETst_Parser::Instance()
{
    SAPDBMEM_STATIC_RAW_ALLOCATE(Space, sizeof(RTETst_Parser));

    if ( !m_Instance )
    {
        m_Instance = new (Space) RTETst_Parser();
    }

    return *m_Instance;
}

    /*!
      @brief execute a test for given task

      If no argument string is given, the default test function is called.
      Otherwise the test command is isolated and the corresponding entry in the
      test list is searched and executed.

      @param pSelf [ in ] the task interface
      @param argumentString [ in ] the test argument string including test command
      @param argumentLength [ in ] the test argument length
      @param errList [out] error list filled if false is returned
      @return true if test succeeded, false if test failed
     */
SAPDB_Bool RTETst_IParser::Execute( RTETask_ITask *pSelf
                                   , SAPDB_UTF8 const *argumentString
                                   , SAPDB_Int4 argumentLength
                                   , SAPDB_Int4 &errCode
                                   , SAPDBErr_MessageList &errList )
{
    RTETst_Parser &parser = RTETst_Parser::Instance();
    // find command string length (in non zero terminated argument string...)
    SAPDB_Int4 iCommandLength = argumentLength;

    // default command without argument string
    if ( 0 == argumentLength ) 
    {
        RTETst_CallEntry *pEntry = parser.m_DefaultTestEntry;
        if ( pEntry )
        {
            return ((pEntry->Executor)( pSelf
                                      , (SAPDB_UTF8 const *)""
                                      , 0
                                      , pEntry->pContext
                                      , errCode
                                      , errList));
        }
    }
    else
    {
        for ( iCommandLength = 0; iCommandLength < argumentLength; ++iCommandLength )
        {
            if ( argumentString[iCommandLength] == ' '
              || argumentString[iCommandLength] == '\t' )
            {
                break;
            }
        }

        // find exact match
        RTETst_CallEntry *pEntry = parser.m_FirstTestEntry;

        while ( pEntry )
        {
            if ( 0 == SAPDB_strncasecmp((SAPDB_Char const *)pEntry->testCommand, (SAPDB_Char const *)argumentString, iCommandLength) )
            {
                return ((pEntry->Executor)( pSelf
                                          , argumentString
                                          , argumentLength
                                          , pEntry->pContext
                                          , errCode
                                          , errList));
            }
            pEntry = pEntry->pNext;
        }
    }

    if ( !argumentString )
    {
        errList = SAPDBErr_MessageList("RTETEST"
                                    , __CONTEXT__
                                    , RTEERR_TEST_UNKNOWN_SUBCOMMAND
                                    , SAPDB_ToString("<default>") );
    }
    else
    {
        errList = SAPDBErr_MessageList("RTETEST"
                                    , __CONTEXT__
                                    , RTEERR_TEST_UNKNOWN_SUBCOMMAND
                                    , SAPDB_ToString((SAPDB_Char const *)argumentString
                                    , (SAPDB_UInt2)iCommandLength
                                    , (SAPDB_UInt2)iCommandLength) );
    }
    errCode = e_unknown_functionname;
    return false;
}

    /*!
      @brief register a test

      A new element is allocated and added either as default test or

      @param testCommand [in] unique test command
      @param pTestContext [in] test context pointer passed to each such test call
      @param pExecutor [in] the test command to execute
      @param errList [out] error list filled if false is returned
      @return true if registration passed, false if failed(i.e. not unique...)
     */
SAPDB_Bool RTETst_IParser::Register( SAPDB_UTF8 const *testCommand
                                   , RTETst_IContext *pTestContext
                                   , RTETst_ExecutorCall pExecutor
                                   , SAPDBErr_MessageList &errList )
{
    RTETst_Parser &parser = RTETst_Parser::Instance();

    RTEConf_Parameter::String callEnabled;
    SAPDBErr_MessageList tmpErrList;

    if ( RTEConf_ParameterAccess::Instance()->GetString( (RTEConf_Parameter::Name)"ENABLE_CHECK_INSTANCE",
                                                         callEnabled,
                                                         tmpErrList) 
      && callEnabled[0] == 'N' )
    {
        errList = SAPDBErr_MessageList("RTETEST"
                                    , __CONTEXT__
                                    , RTEWARN_TESTS_DISABLED );
        return false;
    }

    RTETst_CallEntry *newEntry = (RTETst_CallEntry *)
        RTEMem_RteAllocator::Instance().Allocate(sizeof(RTETst_CallEntry));

    if ( !newEntry )
    {
        errList = SAPDBErr_MessageList("RTETEST"
                                    , __CONTEXT__
                                    , RTEERR_ALLOCATION_FAILED
                                    , "RTETst_CallEntry" );
        return false;
    }
    newEntry->pContext = pTestContext;
    newEntry->Executor = pExecutor;

    SAPDB_UInt4 testCommandLength = (testCommand ? SAPDB_strlen((SAPDB_Char const *)testCommand) : 0);

    if ( testCommandLength == 0 )
    {
        newEntry->testCommand = 0;
        newEntry->pNext = 0;
        parser.m_DefaultTestEntry = newEntry;
        return true; // last default entry registered wins
    }

    // we got a non default testCommand...
    newEntry->testCommand = (SAPDB_UTF8 *)RTEMem_RteAllocator::Instance().Allocate(testCommandLength+1);
    if ( !newEntry->testCommand )
    {
        RTEMem_RteAllocator::Instance().Deallocate(newEntry);
        errList = SAPDBErr_MessageList("RTETEST"
                                    , __CONTEXT__
                                    , RTEERR_ALLOCATION_FAILED
                                    , (SAPDB_Char const *)testCommand );
        return false;
    }

    SAPDB_MemCopyNoCheck(newEntry->testCommand, testCommand, testCommandLength+1);

    RTETst_CallEntry *pList = parser.m_FirstTestEntry;

    do {
        newEntry->pNext = pList;
        while ( pList )
        {
            SAPDB_UInt4 checkLength = (SAPDB_UInt4)strlen((SAPDB_Char const *)pList->testCommand);
            if ( checkLength > testCommandLength )
            {
                checkLength = testCommandLength;
            }

            if ( 0 == SAPDB_strncasecmp((SAPDB_Char const *)pList->testCommand, (SAPDB_Char const *)testCommand, checkLength) )
            {
                errList = SAPDBErr_MessageList("RTETEST"
                                            , __CONTEXT__
                                    		, RTEERR_TEST_NON_UNIQUE_COMMAND
                                            , (SAPDB_Char const *)testCommand
                                            , (SAPDB_Char const *)pList->testCommand );
                RTEMem_RteAllocator::Instance().Deallocate(newEntry->testCommand);
                RTEMem_RteAllocator::Instance().Deallocate(newEntry);
                return false;
            }
            pList = pList->pNext;
        }
        // the atomic cmpxchg protects chaecks for collision with other registration...
    } while( !RTESys_CompareAndExchange( parser.m_FirstTestEntry
                                       , newEntry->pNext
                                       , newEntry
                                       , pList ) );
    return true; 
}
