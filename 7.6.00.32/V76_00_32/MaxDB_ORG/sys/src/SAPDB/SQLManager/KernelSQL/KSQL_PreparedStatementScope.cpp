/*****************************************************************************/
/*!
  @file         KSQL_PreparedStatementScope.cpp
  @author       MartinKi

  @brief        Implementation file for class KSQL_PreparedStatementScope.

  \if EMIT_LICENCE
  ========== licence begin  GPL
  Copyright (C) 2005 SAP AG

  This program is free software; you can redisytribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
  ========== licence end
  \endif
*/
/*****************************************************************************/

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SQLManager/KernelSQL/KSQL_PreparedStatementScope.hpp"
#include "SAPDBCommon/Messages/SDBMsg_KSQL.h"

#include "SQLManager/SQLMan_SQLStatementContext.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "SQLManager/ErrorHandling/SQLManErr_Interface.hpp"
#include "SQLManager/KernelSQL/KSQL_Connection.hpp"
#include "SQLManager/KernelSQL/KSQL_PreparedStatement.hpp"

#include "Messages/Msg_List.hpp"

#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp" // Routine trace macros
#include "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

extern SAPDBTrace_Topic KernelSQL_Trace;   //!< kernel sql trace

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

KSQL_PreparedStatementScope::KSQL_PreparedStatementScope(
    SAPDBMem_IRawAllocator& alloc)
    : m_alloc( alloc ),
      m_pStatementContext( 0 ),
      m_pConnection( 0 ),
      m_pStatement( 0 )
{}

/*************************************************************************/

KSQL_PreparedStatementScope::~KSQL_PreparedStatementScope()
{
    SAPDBTRACE_METHOD_DEBUG (
        "KSQL_PreparedStatementScope::~KSQL_PreparedStatementScopeRuntime",
        KernelSQL_Trace, 1);

    // order is important
    destroy( m_pStatement, m_alloc );
    destroy( m_pConnection, m_alloc );
    destroy( m_pStatementContext, m_alloc );
}

/*************************************************************************/

KSQL_PreparedStatementScope::InitRc
KSQL_PreparedStatementScope::Initialize(
    SQLMan_Context& context,
    char const*     statementText,
    Msg_List&       errMsg)
{
    SAPDBTRACE_METHOD_DEBUG (
        "KSQL_PreparedStatementScope::Initialize", KernelSQL_Trace, 1);

    SAPDBERR_ASSERT_ARGUMENT( context.IsOk() );
    SAPDBERR_ASSERT_STATE( m_pStatementContext == 0 );
    SAPDBERR_ASSERT_STATE( m_pConnection == 0 );
    SAPDBERR_ASSERT_STATE( m_pStatement == 0 );

    m_pStatementContext = new ( m_alloc )
        SQLMan_SQLStatementContext( context );

    if ( ! m_pStatementContext ) {

        errMsg = Msg_List(
            Msg_List::Error,
            SDBMSG_KSQL_NO_MEMORY,
            Msg_Arg( SDBMSGTAG_KSQL_NO_MEMORY__MEMREQUEST,
                     SAPDB_ToString( sizeof( SQLMan_SQLStatementContext ) ) )
            );

        return initRcOutOfMemory;
    }

    if ( ! context.IsOk() ) {

        const SAPDB_Int returnCode = context.ReturnCode();
        context.ResetReturnCode();

        errMsg.Overrule(
            Msg_List(
                Msg_List::Error,
                SDBMSG_KSQL_PREPSTMTSCOPE_NEWCONTEXT_FAILED,
                Msg_Arg(SDBMSGTAG_KSQL_PREPSTMTSCOPE_NEWCONTEXT_FAILED__ERRORTEXT,
                        SQLManErr_Interface::GetInstance().GetErrorText(
                            returnCode ) ),
                Msg_Arg(SDBMSGTAG_KSQL_PREPSTMTSCOPE_NEWCONTEXT_FAILED__RETURNCODE,
                        returnCode )
            ) );

        return initRcSystemError;
    }

    m_pConnection = new ( m_alloc ) KSQL_Connection();

    if ( ! m_pConnection ) {

        errMsg = Msg_List(
            Msg_List::Error,
            SDBMSG_KSQL_NO_MEMORY,
            Msg_Arg( SDBMSGTAG_KSQL_NO_MEMORY__MEMREQUEST,
                     SAPDB_ToString( sizeof( KSQL_Connection ) ) )
            );

        return initRcOutOfMemory;
    }

    if ( ! m_pConnection->isConnected() ) {

        const SAPDB_Int returnCode = context.ReturnCode();
        context.ResetReturnCode();

        errMsg.Overrule(
            Msg_List(
                Msg_List::Error,
                SDBMSG_KSQL_PREPSTMTSCOPE_CREATESTATEMENT_FAILED,
                Msg_Arg(SDBMSGTAG_KSQL_PREPSTMTSCOPE_NEWCONTEXT_FAILED__ERRORTEXT,
                        SQLManErr_Interface::GetInstance().GetErrorText(
                            returnCode ) ),
                Msg_Arg(SDBMSGTAG_KSQL_PREPSTMTSCOPE_NEWCONTEXT_FAILED__RETURNCODE,
                        returnCode )
                ) );

        return initRcSystemError;
    }

    // statement prepare
    m_pStatement = new ( m_alloc )
        KSQL_PreparedStatement( m_pConnection->createPreparedStatement() );

    if ( ! m_pStatement ) {

        errMsg = Msg_List(
            Msg_List::Error,
            SDBMSG_KSQL_NO_MEMORY,
            Msg_Arg( SDBMSGTAG_KSQL_NO_MEMORY__MEMREQUEST,
                     SAPDB_ToString( sizeof( KSQL_PreparedStatement ) ) )
            );

        return initRcOutOfMemory;
    }

    KSQL_Statement::SQLCode sqlCode =
        m_pStatement->prepare( statementText );

    if ( sqlCode != 0 ) {

        const SAPDB_Int returnCode = context.ReturnCode();
        context.ResetReturnCode();

        errMsg.Overrule(
            Msg_List(
                Msg_List::Error,
                SDBMSG_KSQL_PREPSTMTSCOPE_CREATESTATEMENT_FAILED,
                Msg_Arg(SDBMSGTAG_KSQL_PREPSTMTSCOPE_NEWCONTEXT_FAILED__ERRORTEXT,
                        SQLManErr_Interface::GetInstance().GetErrorText(
                            returnCode ) ),
                Msg_Arg(SDBMSGTAG_KSQL_PREPSTMTSCOPE_NEWCONTEXT_FAILED__RETURNCODE,
                        returnCode )
                ) );

        return initRcSystemError;
    }

    SAPDBERR_ASSERT_STATE( context.IsOk() );
    return initRcOk;
}

/*************************************************************************/
