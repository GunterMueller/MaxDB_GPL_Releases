/*****************************************************************************/
/*!
  @file         SQLMan_InternalTrans.cpp
  @author       MartinKi
  @ingroup      SQLManager

  @brief        Implementation file for class SQLMan_InternalTrans.

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

#include "SQLManager/SQLMan_InternalTrans.hpp"

#include "SQLManager/SQLMan_Context.hpp"
#include "SQLManager/ErrorHandling/SQLManErr_Interface.hpp"

#include "Transaction/Trans_Context.hpp"

#include "SAPDB/Messages/Msg_List.hpp"
#include "SAPDBCommon/Messages/SDBMsg_SQLMan.h"

#include "hak51.h" // a51_internal_trans

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/


/**************************************************************************/

SQLMan_InternalTransInfo::~SQLMan_InternalTransInfo()
{
    if ( m_transSet ) {
        SAPDBERR_ASSERT_STATE( ! m_transSet );

        // emergency code only; it is considered a programming error
        // not to have properly closed the transaction before
        // destruction of the object

        Msg_List dummyMsg;
        this->Close( doRollback, dummyMsg );
    }
}

/**************************************************************************/

SAPDB_Bool
SQLMan_InternalTransInfo::NewTrans(Msg_List& errMsg)
{
    SAPDBERR_ASSERT_STATE( m_transSet == false );

    if ( m_transSet ) {
        return false;
    }

    SAPDBERR_ASSERT_STATE( m_parentContext.IsOk() );

    a51_internal_trans( m_parentContext, m_oldTrans );

    if ( m_parentContext.IsOk()
         && ( m_parentContext.GetTransContext().OldTrans().trError_gg00 == e_ok) ) {

        m_internalTransContext = m_parentContext.TransContext();
        m_transSet = true;
        m_internalTransSet = true;

    }

    if ( m_parentContext.GetTransContext().OldTrans().trError_gg00 != e_ok ) {

        Msg_List& savedMsg    = m_parentContext.GetErrorList();
        tgg00_BasisError bErr =
            m_parentContext.GetTransContext().OldTrans().trError_gg00;

        m_parentContext.GetTransContext().OldTrans().trError_gg00 = e_ok;
        m_parentContext.ResetReturnCode();

        errMsg = savedMsg;
        errMsg.Overrule(
            Msg_List(
                Msg_List::Error,
                SDBMSG_SQLMAN_INTERNALTRANS_CREATE_FAILED,
                Msg_Arg(SDBMSGTAG_SQLMAN_INTERNALTRANS_CREATE_FAILED__ERRORTEXT,
                        SQLManErr_Interface::GetInstance()
                        .GetBaseErrorText( bErr )),
                Msg_Arg(SDBMSGTAG_SQLMAN_INTERNALTRANS_CREATE_FAILED__RETURNCODE,
                        SAPDB_ToString( bErr ))
                ) );

        return false;

    }

    return true;
}

/**************************************************************************/

SAPDB_Bool
SQLMan_InternalTransInfo::Close(
    const CloseType closeType,
    Msg_List&       errMsg)
{
    if ( ! m_transSet ) {
        return true;
    }

    this->SwitchToInternal();

    tgg00_MessType mType;
    if ( closeType == doCommit )
    {
        mType.becomes( m_commit );
    }
    else
    {
        mType.becomes( m_rollback );
    }
    m_parentContext.TransContext().trError_gg00 = e_ok;

    a51close_internal_trans( m_parentContext, m_oldTrans, mType );

    m_internalTransSet = false;
    m_transSet         = false;

    if ( m_parentContext.GetTransContext().OldTrans().trError_gg00 != e_ok ) {

        Msg_List& savedMsg = m_parentContext.GetErrorList();
        tgg00_BasisError bErr =
            m_parentContext.GetTransContext().OldTrans().trError_gg00;

        m_parentContext.GetTransContext().OldTrans().trError_gg00 = e_ok;
        m_parentContext.ResetReturnCode();

        errMsg = savedMsg;
        errMsg.Overrule (
            Msg_List(
                Msg_List::Error,
                SDBMSG_SQLMAN_INTERNALTRANS_CLOSE_FAILED,
                Msg_Arg(SDBMSGTAG_SQLMAN_INTERNALTRANS_CLOSE_FAILED__ERRORTEXT,
                        SQLManErr_Interface::GetInstance()
                        .GetBaseErrorText( bErr )),
                Msg_Arg(SDBMSGTAG_SQLMAN_INTERNALTRANS_CLOSE_FAILED__COMMAND_NAME,
                        closeType == doCommit ? "COMMIT" : "ROLLBACK"),
                Msg_Arg(SDBMSGTAG_SQLMAN_INTERNALTRANS_CLOSE_FAILED__RETURNCODE,
                        SAPDB_ToString( bErr ))
                ) );

        return false;
    }

    return true;
}

/**************************************************************************/

SAPDB_Bool
SQLMan_InternalTransInfo::IsInternalTrans() const
{
    return m_transSet && m_internalTransSet;
}

/**************************************************************************/

void
SQLMan_InternalTransInfo::SwitchToInternal()
{
    if ( m_transSet && ! m_internalTransSet ) {
        m_parentContext.TransContext() = m_internalTransContext;
        m_internalTransSet             = true;
    }
}

/**************************************************************************/

void
SQLMan_InternalTransInfo::SwitchToParent()
{
    if ( m_transSet && m_internalTransSet ) {
        m_parentContext.TransContext() = m_oldTrans.utrTrans();
        m_internalTransSet             = false;
    }
}

/**************************************************************************/

SAPDB_Bool
SQLMan_InternalTrans::CommitRollbackInternal(
    const CloseType closeType,
    Msg_List&       errMsg)
{
    SAPDBERR_ASSERT_STATE( m_transSet );

    if ( ! m_transSet ) {
        return false;
    }

    const SAPDB_Bool internalSetOnEntry = this->IsInternalTrans();

    if ( (! this->Close( closeType, errMsg ))
         || (! this->NewTrans( errMsg )) ) {
        return false;
    }

    if ( ! internalSetOnEntry ) {

        this->SwitchToParent();

    }

    return true;
}

/**************************************************************************/
