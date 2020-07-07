/*!***************************************************************************

  module      : RTE_Test
  responsible : JoergM
  special area: RunTime
  created     : 2000-11-22
  last changed: 2000-11-22 12:00
  Copyright (c) 2000-2005 SAP AG
  description : entrypoint for component tests



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

#include "RunTime/RTE_Test.hpp"
#include "RunTime/Test/RTETst_IParser.hpp"
#include "RunTime/RTE_Message.hpp"
#include "SAPDBCommon/SAPDB_ToString.hpp"
#if !defined(_WIN32)
#include <unistd.h>
#endif
#include "RunTime/Tasking/RTETask_ITask.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "RunTime/RTE_Test_Messages.hpp"
#include "RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/
/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/
/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/
/*===========================================================================*
 *  LOCAL FUNCTIONS                                                          *
 *===========================================================================*/
/*===========================================================================*
 *  EXTERNAL FUNCTIONS                                                       *
 *===========================================================================*/

/*!
  @brief test if parameter ENABLE_CHECK_INSTANCE is not set or set to YES
  @return true if parameter not found or set to YES, false if tests are disabled
 */
static SAPDB_Bool IsCheckInstanceCallEnabled()
{
    RTEConf_Parameter::String callEnabled;
    SAPDBErr_MessageList errList;

    return ( !RTEConf_ParameterAccess::Instance()->GetString( (RTEConf_Parameter::Name)"ENABLE_CHECK_INSTANCE",
                                                         callEnabled,
                                                         errList) 
                                                         || callEnabled[0] == 'Y' );
}

/*!
  @brief DIAGNOSE CHECK INSTANCE 4 main entry point

  The RTE_Test parses the input string, select the test for it and calls the test that registered
  for the first keyword (which is unique for all tests)

  Result of test is returned in Trans.tr_error_gg00.

  Additional messages are using SqlMan_Context::GetContext()->GetErrorList() as return.

  @param Trans [ inout ] the transaction context
  @param utf8StringArgument [ in ] the additional arguement string
  @param utf8StringArgumentByteLength [ in ] length of string in bytes (not characters!)
  @return none
 */
externCpp void RTE_Test ( tgg00_TransContext& Trans
                        , SAPDB_UTF8 const *utf8StringArgument
                        , SAPDB_Int4 utf8StringArgumentByteLength)
{
    ROUTINE_DBG_MSP00 ("RTE_Test");
    RTETask_ITask *pSelf = RTETask_ITask::Self();

    SQLMan_Context *sqlContext = SQLMan_Context::GetContext();
    if ( !sqlContext )
    {
    	RTE_Message( SAPDBErr_MessageList( "RTETEST"
                                         , __CONTEXT__
                                         , RTEERR_TEST_OUTSIDE_SESSION
                                         ));
        Trans.trError_gg00 = e_unknown_session;
        return;
    }

    if ( !IsCheckInstanceCallEnabled() )
    {
    	sqlContext->GetErrorList() = SAPDBErr_MessageList( "RTETEST"
                                                         , __CONTEXT__
                                                         , RTEWARN_TESTS_DISABLED
                                                         );
        Trans.trError_gg00 = e_invalid_command;
        return;
    }

    if ( !pSelf )
    {
    	sqlContext->GetErrorList() = SAPDBErr_MessageList( "RTETEST"
                                                         , __CONTEXT__
                                                         , RTEERR_TEST_OUTSIDE_TASKING 
                                                         );
        Trans.trError_gg00 = e_user_not_connected;
        return;
    }

    if ( !utf8StringArgument )
    {
        utf8StringArgumentByteLength = 0;
    }

    SAPDB_Int4 errCode;
    SAPDBErr_MessageList errList;
    if ( !RTETst_IParser::Execute( pSelf
                                  , utf8StringArgument
                                  , utf8StringArgumentByteLength
                                  , errCode
                                  , errList) )
    {
        sqlContext->GetErrorList() = errList;
        Trans.trError_gg00 = errCode;
    }
}

