/****************************************************************************/
/*!

  @file        SysView_TransactionHistory.cpp

  -------------------------------------------------------------------------

  @author      FerdiF

  @ingroup     SystemViews
  @brief       This module implements the "Transactionhistory" view class.

  @see        

*/
/*-------------------------------------------------------------------------

  copyright:    Copyright (c) 2002-2005 SAP AG


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




*****************************************************************************/



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "ggg00.h"
#include "hak07.h"
#include "hgg01.h"
#include "hgg01_3.h"
#include "ggg91.h"
#include "ggg92.h"
#include "hgg06.h"
#include "SystemViews/SysView_Defines.hpp"
#include "SystemViews/SysView_ITableObj.hpp"
#include "SystemViews/SysView_TransactionHistory.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "Transaction/Trans_Iterator.hpp"
#include "RunTime/Tasking/RTETask_ITask.hpp"
#include "Transaction/Trans_ExtendedMonitoringIter.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS  (PROTOTYPES)                                     *
 *===========================================================================*/



/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

/// fills an empty table object with the column definitions.
void SysView_TransactionHistory::Create(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_TransactionHistory::Create", SysView_Trace, 5);

	m_Table->AppendCol (ITOCT_FIXED, SV_TRANSACTIONID, 15);
	m_Table->AppendCol (ITOCT_FIXED, SV_TASKID, 10);
	m_Table->AppendCol (ITOCT_FIXED, SV_WRITETRANSACTIONID, 15);
  	m_Table->AppendCol (ITOCT_TIMESTAMP, SV_STARTDATE, 0);
  	m_Table->AppendCol (ITOCT_TIMESTAMP, SV_ENDDATE, 0);
  	m_Table->AppendCol (ITOCT_CHAR, SV_STATE, 10);
    
    if (g01unicode)
        m_Table->AppendCol (ITOCT_CHARUNICODE, SV_COMMENT, 2048);
    else
        m_Table->AppendCol (ITOCT_CHAR, SV_COMMENT, 2048);
}

/*---------------------------------------------------------------------------*/
/// returns the number of columns in this table.
SAPDB_Int SysView_TransactionHistory::GetColCount(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_TransactionHistory::GetColCount", SysView_Trace, 5);

	return SV_CC_TRANSACTIONHISTORY;
}

/*---------------------------------------------------------------------------*/
/// returns the estimated number of rows for this table.
SAPDB_Int SysView_TransactionHistory::EstimateRows(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_TransactionHistory::EstimateRows", SysView_Trace, 5);

	return SV_ER_TRANSACTIONHISTORY;
}

/*---------------------------------------------------------------------------*/
/// fills this table with information
void SysView_TransactionHistory::Execute(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_TransactionHistory::Execute", SysView_Trace, 5);

    SAPDB_Int4        taskid;
    tgg91_TransNo     writetransno;

    m_Table->GetCatalogTable();

    Trans_ExtendedMonitoringIter   iter2(m_Context.GetTaskId());

    if (m_Context.IsOk())
    {
        while ( iter2.Next() )
        {
            Trans_ExtendedMonitoringContext* pctx = iter2.GetMonitoringContext();

            m_Table->MoveToCol (ITOVT_TRANSNO, &pctx->GetTransNo(), 0);

            taskid = pctx->GetTaskId();
            m_Table->MoveToCol (ITOVT_INT4, &taskid, 0);

            m_Table->MoveToCol (ITOVT_TRANSNO, &pctx->GetWriteTransNo(), 0);

            RTESys_SQLTimestamp timeBuffer;
            pctx->GetTransactionStartTime(timeBuffer);
            m_Table->MoveToCol (ITOVT_CHARPTR, &timeBuffer[0], sizeof(timeBuffer));

            pctx->GetTransactionEndTime(timeBuffer);
            m_Table->MoveToCol (ITOVT_CHARPTR, &timeBuffer[0], sizeof(timeBuffer));

            if ( pctx->HasTransactionCommitted() )
                m_Table->MoveToCol (ITOVT_CHARPTR,  const_cast<char *>("COMMITTED"), (SAPDB_Int)strlen("COMMITTED"));   
            else
                m_Table->MoveToCol (ITOVT_CHARPTR,  const_cast<char *>("ROLLBACKED"), (SAPDB_Int)strlen("ROLLBACKED"));   

            const void* comment    = pctx->GetComment();
            SAPDB_UInt2 commentlen = pctx->GetCommentLength();
            if (g01unicode)
                m_Table->MoveToCol (ITOVT_UNICODEPTR, comment, commentlen);
            else
                m_Table->MoveToCol (ITOVT_CHARPTR, comment, commentlen);

        }
    }
    Trans_Iterator    iter    (*RTETask_ITask::Self(), Trans_Iterator::HasMonitoringInfo);
    if (m_Context.IsOk())
    {
        while ( iter.Next() )
        {
            Trans_ExtendedMonitoringContext* pctx = iter.GetMonitoringContext();
            if ( pctx->IsTransactionActive())
            {
                m_Table->MoveToCol (ITOVT_TRANSNO, &pctx->GetTransNo(), 0);

                taskid = pctx->GetTaskId();
                m_Table->MoveToCol (ITOVT_INT4, &taskid, 0);

                iter.GetWriteTransNo(writetransno);
                m_Table->MoveToCol (ITOVT_TRANSNO, &writetransno, 0);

                RTESys_SQLTimestamp timeBuffer;
                pctx->GetTransactionStartTime(timeBuffer);
                m_Table->MoveToCol (ITOVT_CHARPTR, &timeBuffer[0], sizeof(timeBuffer));

                pctx->GetTransactionEndTime(timeBuffer);
                m_Table->MoveToCol (ITOVT_CHARPTR, &timeBuffer[0], sizeof(timeBuffer));

                m_Table->MoveToCol (ITOVT_CHARPTR,  const_cast<char *>("ACTIVE"), (SAPDB_Int)strlen("ACTIVE"));   

                const void* comment    = pctx->GetComment();
                SAPDB_UInt2 commentlen = pctx->GetCommentLength();

                if (g01unicode)
                    m_Table->MoveToCol (ITOVT_UNICODEPTR, comment, commentlen);
                else
                    m_Table->MoveToCol (ITOVT_CHARPTR, comment, commentlen);
            }
        }
    }
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
