/****************************************************************************/
/*!

  @file        SysView_Transactions.cpp

  -------------------------------------------------------------------------

  @author      FerdiF

  @ingroup     SystemViews
  @brief       This module implements the "Transactions" view class.

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
#include "ggg91.h"
#include "ggg92.h"
#include "hgg06.h"
//#include "vkb522.hpp"
#include "SystemViews/SysView_Defines.hpp"
#include "SystemViews/SysView_ITableObj.hpp"
#include "SystemViews/SysView_Transactions.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "Transaction/Trans_Iterator.hpp"
#include "RunTime/Tasking/RTETask_ITask.hpp"
#include "Logging/Log_Types.hpp"
#include "Logging/Log_Transaction.hpp"

#define KERNEL_LZU
#include "RunTime/Tasking/RTETask_ITask.hpp"
#include "RunTime/Tasking/RTETask_Task.hpp"

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

static void
int2string (
    char       * target,
    tsp00_Int4   intval,
    int          intsize)
{
    for (int i = intsize - 1; i >= 0; --i) {
        char digit = (char) (intval % 10);
        target [i] = digit + '0';
        intval /= 10;
    }
}

static bool
ints2timestamp (
    tsp00_Timestamp & datetime,
    tsp00_Int4        date,
    tsp00_Int4        time)
{
    if (( 0 == date ) || (0 == time))
        return false;

    for (int i = 0; i < sizeof (datetime); ++i) {
        datetime [i] = '0';
    }
    int2string (datetime, date, 8);
    int2string (datetime.asCharp () + 8, time, 6);
    return true;
}

/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

void SysView_Transactions::Create(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_Transactions::Create", SysView_Trace, 5);

	m_Table->AppendCol (ITOCT_FIXED, SV_TASKID, 10);
	m_Table->AppendCol (ITOCT_FIXED, SV_TRANSACTIONID, 15);
	m_Table->AppendCol (ITOCT_TIMESTAMP, SV_STARTDATE, 0);
	m_Table->AppendCol (ITOCT_FIXED, SV_WRITETRANSACTIONID, 15);
	m_Table->AppendCol (ITOCT_FIXED, SV_SQLLOCKREQUESTTIMEOUT, 10);
	m_Table->AppendCol (ITOCT_FIXED, SV_OMSLOCKREQUESTTIMEOUT, 10);
	m_Table->AppendCol (ITOCT_FIXED, SV_LASTWRITE, 10);
	m_Table->AppendCol (ITOCT_CHAR, SV_SQLLOCKMODE, 14);
	m_Table->AppendCol (ITOCT_CHAR, SV_OMSLOCKMODE, 14);
	m_Table->AppendCol (ITOCT_CHAR, SV_REQUESTMODE, 14);
	m_Table->AppendCol (ITOCT_FIXED, SV_HISTORYROOT, 10);
	m_Table->AppendCol (ITOCT_FIXED, SV_HISTORYLAST, 10);
	m_Table->AppendCol (ITOCT_FIXED, SV_HISTORYPAGECOUNT, 10);
	m_Table->AppendCol (ITOCT_FIXED, SV_LASTHISTORYSEQUENCE, 10);
	m_Table->AppendCol (ITOCT_FIXED, SV_CONSISTENTVIEWID, 15);
	m_Table->AppendCol (ITOCT_CHAR ,SV_TASKTYPE, 32);
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_Transactions::GetColCount(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_Transactions::GetColCount", SysView_Trace, 5);

	return -1; // not needed anymore   SV_CC_TRANSACTIONS value is wrong
}
/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_Transactions::EstimateRows(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_Transactions::EstimateRows", SysView_Trace, 5);

	return SV_ER_TRANSACTIONS;
}

/*---------------------------------------------------------------------------*/

void SysView_Transactions::Execute(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_Transactions::Execute", SysView_Trace, 5);
 
    tgg91_TransNo               transno;
    SAPDB_Int4                  taskid;
    SAPDB_Int4                  LockTimeout;
    SAPDB_Int4                  TimeLastWrite;
    tkb05_TransLockMode_Param   SqlReqMode;
    tkb05_TransLockMode_Param   OmsReqMode;
    tkb05_TransLockMode_Param   LockMode;

    Data_PageNo                 UndoRoot;
    Data_PageNo                 UndoLast;
    SAPDB_UInt                  UndoPageCount;
    Log_EntrySequence           LastUndoSequence;
    Log_Transaction*            LogTrans;
    RTETask_Task               *pTaskObject;
    RTESys_SQLTimestamp         timeBuffer;

    m_Table->GetCatalogTable();

    Trans_Iterator    iter    (*RTETask_ITask::Self(), Trans_Iterator::All);
    if (m_Context.IsOk())
    {
        while ( iter.Next() )
        {
            taskid = iter.GetTaskId();
            m_Table->MoveToCol (ITOVT_INT4, &taskid, 0);                    /* TASKID */
    
            iter.GetTransNo (transno);
            m_Table->MoveToCol (ITOVT_TRANSNO, &transno, 0);                /* TRANSACTIONID */

            if ( iter.GetTransStartTime(timeBuffer) )
                m_Table->MoveToCol (ITOVT_CHARPTR, &timeBuffer[0], sizeof(timeBuffer)); /* STARTDATE */
            else
                m_Table->MoveToCol (ITOVT_NULL, NULL, 0);                   /* STARTDATE */

            iter.GetWriteTransNo (transno);
            m_Table->MoveToCol (ITOVT_TRANSNO, &transno, 0);                /* WRITETRANSACTIONID */

            LockTimeout = iter.GetSqlLockReqTimeout(SqlReqMode);
            if(tlmFree_ekb05 != SqlReqMode)
                m_Table->MoveToCol(ITOVT_INT4, &LockTimeout, 0);            /* SQLLOCKREQUESTTIMEOUT */
            else
                m_Table->MoveToCol(ITOVT_NULL, NULL, 0);                    /* SQLLOCKREQUESTTIMEOUT */

            LockTimeout = iter.GetOmsLockReqTimeout(OmsReqMode);            
            if(tlmFree_ekb05 != OmsReqMode)
                m_Table->MoveToCol(ITOVT_INT4, &LockTimeout, 0);            /* OMSLOCKREQUESTTIMEOUT */
            else
                m_Table->MoveToCol(ITOVT_NULL, NULL, 0);                    /* OMSLOCKREQUESTTIMEOUT */

            TimeLastWrite = iter.GetTimeLastWrite();
            if (SqlReqMode == tlmEotExcl_ekb05)
                m_Table->MoveToCol(ITOVT_INT4, &TimeLastWrite, 0);          /* LASTWRITE */
            else
                m_Table->MoveToCol(ITOVT_NULL, NULL, 0);                    /* LASTWRITE */
            
            LockMode = iter.GetSqlLockMode();
            m_Table->MoveToCol (ITOVT_TRANSLOCKMODE, &LockMode, 0);         /* SQLLOCKMODE */

            LockMode = iter.GetOmsLockMode();
            m_Table->MoveToCol (ITOVT_TRANSLOCKMODE, &LockMode, 0);         /* OMSLOCKMODE */

            if (tlmFree_ekb05 != OmsReqMode) 
                m_Table->MoveToCol (ITOVT_TRANSLOCKMODE, &OmsReqMode, 0);   /* REQUESTMODE */
            else if (tlmFree_ekb05 != SqlReqMode)
                m_Table->MoveToCol (ITOVT_TRANSLOCKMODE, &SqlReqMode, 0);   /* REQUESTMODE */
            else
                m_Table->MoveToCol (ITOVT_NULL, NULL, 0);                   /* REQUESTMODE */

            LogTrans = iter.GetLogTransPtr();
            if ( LogTrans && 
                LogTrans->GetInfo (UndoRoot, UndoLast, UndoPageCount, LastUndoSequence, m_Context.GetErrorList()) &&
                (UndoRoot != NIL_PAGE_NO_GG00))
            {         
                m_Table->MoveToCol (ITOVT_INT4, &UndoRoot, 0);              /* HISTORYROOT */          
                m_Table->MoveToCol (ITOVT_INT4, &UndoLast, 0);              /* HISTORYLAST */                     
                m_Table->MoveToCol (ITOVT_UINT4, &UndoPageCount, 0);        /* HISTORYPAGECOUNT */      
                m_Table->MoveToCol (ITOVT_UINT4, &LastUndoSequence, 0);     /* LASTHISTORYSEQUENCE */   
            }
            else
            {
                m_Table->MoveToCol (ITOVT_NULL, NULL, 0);                   /* HISTORYROOT */          
                m_Table->MoveToCol (ITOVT_NULL, NULL, 0);                   /* HISTORYLAST */                     
                m_Table->MoveToCol (ITOVT_NULL, NULL, 0);                   /* HISTORYPAGECOUNT */      
                m_Table->MoveToCol (ITOVT_NULL, NULL, 0);                   /* LASTHISTORYSEQUENCE */  
            } 
            iter.GetConsistViewNo (transno);
            m_Table->MoveToCol (ITOVT_TRANSNO, &transno, 0);                /* CONSISTENTVIEWID */

            if ( pTaskObject = RTETask_Task::ByTaskID(iter.GetTaskId()))    /* TASKTYPE */
            {
                char const *str = pTaskObject->TypeAsString();
                SAPDB_Int   len = (SAPDB_Int)(strlen(str)); 
                if (len > 32) len = 32;
                m_Table->MoveToCol (ITOVT_CHARPTR , const_cast<char *>(str), len); 
            }
            else
                m_Table->MoveToCol (ITOVT_NULL, NULL, 0);                   /* TASKTYPE */  
        }
    }
}
/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
