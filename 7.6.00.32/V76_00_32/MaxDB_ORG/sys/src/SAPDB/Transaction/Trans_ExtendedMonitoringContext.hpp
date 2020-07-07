/*!
  @file           Trans_ExtendedMonitoringContext.hpp
  @author         FerdiF
  @ingroup        Transaction
  @since          2004-18-19

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
#ifndef Trans_ExtendedMonitoringContext_HPP
#define Trans_ExtendedMonitoringContext_HPP

//--------------------------------------------------------------------------------------
#include "SAPDBCommon/SAPDB_Types.hpp"
#include "ggg91.h"
#include "RunTime/System/RTESys_Time.h"
//#include "Transaction/Trans_IExtendedMonitoring.hpp"
//--------------------------------------------------------------------------------------

/// class to maintain transaction history information for one transaction
class Trans_ExtendedMonitoringContext 
{
    friend class Trans_ExtendedMonitoring;
    friend class Trans_ExtendedMonitoringIter;

private:
    static const int ExtMon_CommentLength;

    tgg91_TransNo    m_transactionId;
    SAPDB_Int4       m_taskId;
    tgg91_TransNo    m_writeTransactionId;
    SAPDB_UInt8      m_startTimeUTC;
    SAPDB_UInt8      m_endTimeUTC;
    SAPDB_Bool       m_committed;
    SAPDB_Bool       m_inUse;
    SAPDB_UInt2      m_allocatedLen;  
    SAPDB_UInt2      m_CommentLength;  
    SAPDB_Char*      m_Comment;  

public:
    Trans_ExtendedMonitoringContext()
        : m_CommentLength(0)
        , m_Comment(0)
        , m_allocatedLen(0)
        , m_committed(SAPDB_FALSE)
        , m_inUse(SAPDB_FALSE)
        , m_taskId(0)
    {
        m_transactionId.gg90SetZero();
        m_writeTransactionId.gg90SetZero();
    };

    /// accessory method for comment string
    virtual const void*             GetComment() { return this->m_Comment; };

    /// accessory method for comment string length
    virtual  SAPDB_UInt2            GetCommentLength() { return this->m_CommentLength; };

    /// accessory method for transaction end time
    virtual const SAPDB_Char*       GetTransactionEndTime(RTESys_SQLTimestamp& timeBuffer);

    /// accessory method for transaction start time
    virtual const SAPDB_Char*       GetTransactionStartTime(RTESys_SQLTimestamp& timeBuffer);

    /// accessory method for transaction id
    virtual const tgg91_TransNo&    GetTransNo() { return this->m_transactionId; };

    /// accessory method for session id
    virtual const SAPDB_Int4        GetTaskId() { return this->m_taskId; };

    /// accessory method for update transaction id
    virtual const tgg91_TransNo&    GetWriteTransNo() { return this->m_writeTransactionId; };

    /// accessory method for transaction status ( true == active / false == terminated )
    virtual SAPDB_Bool              IsTransactionActive() { return this->m_inUse; };

    /// accessory method for transaction status ( true == committed / false == rollbacked )
    /// may only be called if IsTransactionActive() has delivered false
    virtual SAPDB_Bool              HasTransactionCommitted() { return this->m_committed; };
};
/// 
#endif // Trans_ExtendedMonitoringContext_HPP
