/*!
  @file           RTETst_RWRegionCode.cpp
  @author         RobinW
  @ingroup        Runtime
  @brief          Runtime RWRegion access test code

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

#include "RunTime/Tasking/RTETask_ITask.hpp"
#include "RunTime/Test/RTETst_IParser.hpp"
#include "SAPDBCommon/SAPDB_string.h"
#include "RunTime/MemoryManagement/RTEMem_RteAllocator.hpp"
#include "RunTime/RTE_Test_Messages.hpp"
#include "RunTime/Synchronisation/RTESync_RWRegion.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*! The common region test command introducer */
#define RTETST_RWREGION_COMMAND "RWRegion"



/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/

/*!
  @brief This test enters/leaves the test RWRegion

  DIAGNOSE CHECK INSTANCE 4 'RWRegion ...'

  the following commands are available:

  RWRegion EnterExclusive (may be abbrevated to EnterE)
  RWRegion EnterNonExclusive (may be abbrevated to EnterN)
  RWRegion SwitchToNonExclusive (may be abbrevated to Switch)
  RWRegion LeaveExclusive (may be abbrevated to LeaveE)
  RWRegion LeaveNonExclusive (may be abbrevated to LeaveN)


  @param pTask [in] the task itself
  @param argumentString [in] the given argument to be parsed
  @param argumentLength [in] the length of given argument string in bytes
  @param errList [out] error list filled in case of problems
  @return true if test passed, false if not
 */
static SAPDB_Bool Test_RWRegionCode( RTETask_ITask        *pTask
                                   , SAPDB_UTF8 const     * argumentString
                                   , SAPDB_Int4             argumentLength
                                   , RTETst_IContext      * pTestContext
                                   , SAPDB_Int4           & errCode
                                   , SAPDBErr_MessageList & errList );

/*===========================================================================*
 *  BEGIN OF CODE                                                            *
 *===========================================================================*/
/*!
  @class RTETst_RWRegionTestContext
  @brief The RWRegion test context passed to all RWRegion test callers
 */
class RTETst_RWRegionTestContext : public RTETst_IContext
{
private:
    RTESync_SpinlockPool m_pool;
public:
    /*!
      @brief test context constructor
     */
    RTETst_RWRegionTestContext()
        :m_pRWRegion(RTESync_CreateRWRegion (1, m_pool, RTEMem_RteAllocator::Instance()))
        ,m_pool((SAPDB_UTF8 *)"RWRegionTest",1,RTEMem_RteAllocator::Instance())
    {
    }

    RTESync_IRWRegion *m_pRWRegion;
};
/*! @endclass RTETst_RWRegionTestContext */

//----------------------------------------------------------------------

/*!
  @brief register the region test code
  @param errList [out] filled in case of registration problem
  @return true if registration done, false if failed
 */
SAPDB_Bool RTETst_RegisterRWRegionTest(SAPDBErr_MessageList &errList)
{
    RTETst_RWRegionTestContext *pContext = new ( RTEMem_RteAllocator::Instance() ) RTETst_RWRegionTestContext();

    SAPDB_Bool ok = true;
    /* Setup default RTE test and initialize RTE_TestParser singleton */
    ok = RTETst_IParser::Register((SAPDB_UTF8 const *)RTETST_RWREGION_COMMAND, pContext, Test_RWRegionCode, errList);
    if ( !ok )
    {
        destroy( pContext, RTEMem_RteAllocator::Instance() );
        return false;
    }

    return true;
}

/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/

//----------------------------------------

static SAPDB_Bool Test_RWRegionCodeEnterNonExclusive( RTETask_ITask     * pTask
                                        , SAPDB_UTF8 const     * argumentString
                                        , RTETst_RWRegionTestContext      * pTestContext
                                        , SAPDB_Int4           &errCode
                                        , SAPDBErr_MessageList &errList );

//----------------------------------------

static SAPDB_Bool Test_RWRegionCodeSwitchToNonExclusive( RTETask_ITask     * pTask
                                        , SAPDB_UTF8 const     * argumentString
                                        , RTETst_RWRegionTestContext      * pTestContext
                                        , SAPDB_Int4           &errCode
                                        , SAPDBErr_MessageList &errList );

//----------------------------------------

static SAPDB_Bool Test_RWRegionCodeEnterExclusive( RTETask_ITask     * pTask
                                        , SAPDB_UTF8 const     * argumentString
                                        , RTETst_RWRegionTestContext      * pTestContext
                                        , SAPDB_Int4           &errCode
                                        , SAPDBErr_MessageList &errList );

//----------------------------------------

static SAPDB_Bool Test_RWRegionCodeLeaveExclusive( RTETask_ITask     * pTask
                                        , SAPDB_UTF8 const     * argumentString
                                        , RTETst_RWRegionTestContext      * pTestContext
                                        , SAPDB_Int4           &errCode
                                        , SAPDBErr_MessageList &errList );

//----------------------------------------

static SAPDB_Bool Test_RWRegionCodeLeaveNonExclusive( RTETask_ITask     * pTask
                                        , SAPDB_UTF8 const     * argumentString
                                        , RTETst_RWRegionTestContext      * pTestContext
                                        , SAPDB_Int4           &errCode
                                        , SAPDBErr_MessageList &errList );
//----------------------------------------

/*!
  The table of subcommands
 */
static struct subcommands_ {
    SAPDB_Int4 keyCode; ///< The subcommand index
    SAPDB_Char const *keyString; ///< the subcommand string
    /// The subcommand code
    SAPDB_Bool (*keyCall)(RTETask_ITask *, SAPDB_UTF8 const *, RTETst_RWRegionTestContext *, SAPDB_Int4 &, SAPDBErr_MessageList &);
} keyTable[] =
{
    { 0, "EnterE", Test_RWRegionCodeEnterExclusive },
    { 1, "EnterN", Test_RWRegionCodeEnterNonExclusive },
    { 2, "Switch", Test_RWRegionCodeSwitchToNonExclusive },
    { 3, "LeaveE", Test_RWRegionCodeLeaveExclusive },
    { 4, "LeaveN", Test_RWRegionCodeLeaveNonExclusive },
    { 5, "EnterExclusive", Test_RWRegionCodeEnterExclusive },
    { 6, "EnterNonExclusive", Test_RWRegionCodeEnterNonExclusive },
    { 7, "SwitchToNonExclusive", Test_RWRegionCodeSwitchToNonExclusive },
    { 8, "LeaveExclusive", Test_RWRegionCodeLeaveExclusive },
    { 9, "LeaveNonExclusive", Test_RWRegionCodeLeaveNonExclusive },
    { 10, 0, 0 }
};

//----------------------------------------

/*!
  @brief parse a single positive but signed 4 byte number number
  @param pArg [inout] current position in argument string
  @return -1 in case of problem, number else
 */
static SAPDB_Int4 parseNumber(SAPDB_UTF8 const *&pArg )
{
    SAPDB_Char const *pCount = (SAPDB_Char const *)pArg;

    SAPDB_Int4 count = -1;

    while ( *pCount )
    {
        SAPDB_Int4 digit = *pCount - '0';
        if ( digit >= 0 && digit <= 9 )
        {
            if ( count == -1 )
            {
                count = digit;
            }
            else
            {
                count *= 10; count += digit;
            }
            ++pCount;
        }
        else
        {
            break;
        }
    }

    while ( *pCount == ' ' )
        ++pCount;
    pArg = (SAPDB_UTF8 const *)pCount;

    return count;
}

//----------------------------------------

/*!
  @brief parse a subcommand string using well known table
  single positive but signed 4 byte number number
  @param argumentString [in] current position in argument string
  @param pArg [out] position behind the subcommand
  @return index into keytable.
 */
static SAPDB_Int4 parseKeyword(SAPDB_UTF8 const *argumentString, SAPDB_UTF8 const * &pArg)
{
    SAPDB_Int4 keyword;

     pArg = argumentString + SAPDB_strlen(RTETST_RWREGION_COMMAND);

    while ( *pArg == ' ' )
        ++pArg;

    for ( keyword = 0; keyTable[keyword].keyString; ++keyword )
    {
        SAPDB_Int4 keyLength = SAPDB_strlen(keyTable[keyword].keyString);
        if ( !SAPDB_strncasecmp((SAPDB_Char const *)pArg, keyTable[keyword].keyString, keyLength) )
        {
            pArg += keyLength;
            while ( *pArg == ' ' )
                ++pArg;
            break;
        }
    }

    return keyTable[keyword].keyCode;
}

//----------------------------------------

/*
  @brief common region test code entry
  Parses subcommand and call function entry from keytable
  @param pTask [in] task calling
  @param argumentString [in] the complete argument string
  @param argumentLength [in] the complete argument string length in bytes
  @param pTestContext [in] the shared test context
  @param errCode [out] error code reported as system error
  @param errList [out] error list filled in case of problems
  @return true if call succeeded, false if call failed
 */
static SAPDB_Bool Test_RWRegionCode( RTETask_ITask     * pTask
                                , SAPDB_UTF8 const     * argumentString
                                , SAPDB_Int4             argumentLength
                                , RTETst_IContext      * pTestContext
                                , SAPDB_Int4           &errCode
                                , SAPDBErr_MessageList &errList )
{
    SAPDB_UTF8 const *pArg = argumentString;
    SAPDB_Int4 keyword = parseKeyword(argumentString, pArg);

    if ( 0 == keyTable[keyword].keyString )
    {
        errList = SAPDBErr_MessageList("RTETEST"
                                      , __CONTEXT__
                                      , RTEERR_TEST_UNKNOWN_SUBCOMMAND
                                      , (SAPDB_Char const *)argumentString );
        errCode = e_unknown_functionname;
        return false;
    }

    return keyTable[keyword].keyCall(pTask, pArg, (RTETst_RWRegionTestContext *)pTestContext, errCode, errList);
}

//----------------------------------------

static SAPDB_Bool Test_RWRegionCodeEnterExclusive( RTETask_ITask     * pTask
                                        , SAPDB_UTF8 const     * argumentString
                                        , RTETst_RWRegionTestContext      * pTestContext
                                        , SAPDB_Int4           &errCode
                                        , SAPDBErr_MessageList &errList )
{
    pTestContext->m_pRWRegion->enter(true);

    return true;
}

//----------------------------------------

static SAPDB_Bool Test_RWRegionCodeSwitchToNonExclusive( RTETask_ITask     * pTask
                                        , SAPDB_UTF8 const     * argumentString
                                        , RTETst_RWRegionTestContext      * pTestContext
                                        , SAPDB_Int4           &errCode
                                        , SAPDBErr_MessageList &errList )
{
/*
    if(!pRWRegion)
        pRWRegion = RTESync_CreateRWRegion (1, pool, RTEMem_RteAllocator::Instance());
*/
    pTestContext->m_pRWRegion->SwitchToNonExclusiveAccess();

    return true;
}

//----------------------------------------

static SAPDB_Bool Test_RWRegionCodeEnterNonExclusive( RTETask_ITask     * pTask
                                        , SAPDB_UTF8 const     * argumentString
                                        , RTETst_RWRegionTestContext      * pTestContext
                                        , SAPDB_Int4           &errCode
                                        , SAPDBErr_MessageList &errList )
{
/*
    if(!pRWRegion)
        pRWRegion = RTESync_CreateRWRegion (1, pool, RTEMem_RteAllocator::Instance());
*/
    pTestContext->m_pRWRegion->enter(false);

    return true;
}

//----------------------------------------

static SAPDB_Bool Test_RWRegionCodeLeaveNonExclusive( RTETask_ITask     * pTask
                                        , SAPDB_UTF8 const     * argumentString
                                        , RTETst_RWRegionTestContext      * pTestContext
                                        , SAPDB_Int4           &errCode
                                        , SAPDBErr_MessageList &errList )
{
/*
    if(!pRWRegion)
        pRWRegion = RTESync_CreateRWRegion (1, pool, RTEMem_RteAllocator::Instance());
*/
    pTestContext->m_pRWRegion->leave(false);

    return true;
}

//----------------------------------------

static SAPDB_Bool Test_RWRegionCodeLeaveExclusive( RTETask_ITask     * pTask
                                        , SAPDB_UTF8 const     * argumentString
                                        , RTETst_RWRegionTestContext      * pTestContext
                                        , SAPDB_Int4           &errCode
                                        , SAPDBErr_MessageList &errList )
{
/*
    if(!pRWRegion)
        pRWRegion = RTESync_CreateRWRegion (1, pool, RTEMem_RteAllocator::Instance());
*/
    pTestContext->m_pRWRegion->leave(true);

    return true;
}

//----------------------------------------

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
