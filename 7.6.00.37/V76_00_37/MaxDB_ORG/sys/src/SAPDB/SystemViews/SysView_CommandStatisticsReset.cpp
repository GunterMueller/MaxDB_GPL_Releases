/*!
  @file     SysView_CommandStatisticsReset.cpp
  @author   DirkT
  @ingroup  SystemViews
  @brief    Systemview COMMANDSTATISTICSRESET for SharedSQL_Commands, reset version
  @see            

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



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "ggg00.h"
#include "hgg01_3.h"
#include "SystemViews/SysView_Defines.hpp"
#include "SystemViews/SysView_ITableObj.hpp"
#include "SystemViews/SysView_CommandStatisticsReset.hpp"
#include "SQLManager/SharedSQL/SharedSQL_SQLCache.hpp"
#include "SQLManager/SharedSQL/SharedSQL_Command.hpp"
#include "SQLManager/SharedSQL/SharedSQL_Types.hpp"
#include "SQLManager/SQLMan_ContextSessionMembers.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "RunTime/RTE_KGS.hpp"

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

void SysView_CommandStatisticsReset::Create(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_CommandsReset::Create", SysView_Trace, 5);

	m_Table->AppendCol (ITOCT_TIMESTAMP, SV_RESETDATE, 0);
    //
	m_Table->AppendCol (ITOCT_CHARBYTE, SV_COMMANDID, 8);		
    //
    m_Table->AppendCol (ITOCT_FIXED,    SV_PREPARECOUNT, 10);
    m_Table->AppendCol (ITOCT_FIXED,    SV_EXECUTECOUNT, 10);
    //
    m_Table->AppendCol (ITOCT_FIXED,    SV_EXECUTETIME, 20);
    m_Table->AppendCol (ITOCT_FIXED,    SV_AVGEXECUTETIME, 20);	
    m_Table->AppendCol (ITOCT_FIXED,    SV_MINEXECUTETIME, 20);	
    m_Table->AppendCol (ITOCT_FIXED,    SV_MAXEXECUTETIME, 20);	
    //
    m_Table->AppendCol (ITOCT_FIXED,    SV_READROWCOUNT, 20);	
    m_Table->AppendCol (ITOCT_FIXED,    SV_QUALIFIEDROWCOUNT, 20);
    m_Table->AppendCol (ITOCT_FIXED,    SV_VIRTUALREADCOUNT, 20);
    m_Table->AppendCol (ITOCT_FIXED,    SV_PHYSICALREADCOUNT, 20);
    m_Table->AppendCol (ITOCT_FIXED,    SV_FETCHROWCOUNT, 20);
    m_Table->AppendCol (ITOCT_FIXED,    SV_SUSPENDCOUNT, 20);
    m_Table->AppendCol (ITOCT_FIXED,    SV_WAITCOUNT, 20);
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_CommandStatisticsReset::GetColCount(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_CommandsReset::GetColCount", SysView_Trace, 5);

	return SV_CC_COMMANDSTATISTICSRESET;
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_CommandStatisticsReset::EstimateRows(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_CommandsReset::EstimateRows", SysView_Trace, 5);

	return SV_ER_COMMANDSTATISTICSRESET;
}

/*---------------------------------------------------------------------------*/

void SysView_CommandStatisticsReset::Execute(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_CommandsReset::Execute", SysView_Trace, 5);

// 14 Cols

    m_Table->GetCatalogTable();
    SharedSQL_SQLCache& SQLCache = m_Context.GetSharedSQL_SQLCache();
    RTE_KGS::Statistic const &instanceStatistic = RTE_KGS::Instance().GetStatistic();

    SharedSQL_CommandCacheIterator Iter = SQLCache.GetCommandCacheIterator();
    while (Iter.IsValid())
    {
        SharedSQL_CommandInfo S;
        Iter->GetCommandInfoReset(S);

        SAPDB_Int8 AVGTime = 0;
        SAPDB_Int8 mInt8 = 0;
        SAPDB_Int4 mInt4 = 0;

        AddColumn( S.mResetTime, AsTimestamp);
        
        m_Table->MoveToCol (ITOVT_INT8,  &S.mCommandID, 8);	
        //
        mInt4      = S.mStatistics.GetTotalPrepareCount();              m_Table->MoveToCol (ITOVT_INT4, &mInt4, 0);
        SAPDB_Int8 mExecCount = S.mStatistics.GetTotalExecuteCount();   m_Table->MoveToCol (ITOVT_INT4, &mExecCount, 0);
        //
        SAPDB_Int8 mExecTime  = S.mStatistics.GetExecuteTime();         m_Table->MoveToCol (ITOVT_INT8, &mExecTime, 0);
        if ( mExecCount > 0 )
        {
            AVGTime = (mExecTime / mExecCount);
            m_Table->MoveToCol (ITOVT_UINT8, &AVGTime, 0); 
        }
        else
        {
            m_Table->MoveToCol (ITOVT_NULL, &AVGTime, 0); // NULL
        }
        //
        mInt8 = S.mStatistics.GetMinExecuteTime();      m_Table->MoveToCol (ITOVT_INT8, &mInt8, 0);	
        mInt8 = S.mStatistics.GetMaxExecuteTime();      m_Table->MoveToCol (ITOVT_INT8, &mInt8, 0);	
        mInt8 = S.mStatistics.GetReadRowCount();        m_Table->MoveToCol (ITOVT_INT8, &mInt8, 0);	
        mInt8 = S.mStatistics.GetQualifiedRowCount();   m_Table->MoveToCol (ITOVT_INT8, &mInt8, 0);	
        mInt8 = S.mStatistics.GetVirtualReadCount();    m_Table->MoveToCol (ITOVT_INT8, &mInt8, 0);	
        mInt8 = S.mStatistics.GetPhysicalReadCount();   m_Table->MoveToCol (ITOVT_INT8, &mInt8, 0);	
        mInt8 = S.mStatistics.GetFetchRowCount();       m_Table->MoveToCol (ITOVT_INT8, &mInt8, 0);	
        mInt8 = S.mStatistics.GetSuspendCount();        m_Table->MoveToCol (ITOVT_INT8, &mInt8, 0);	
        mInt8 = S.mStatistics.GetWaitCount();           m_Table->MoveToCol (ITOVT_INT8, &mInt8, 0);	

        ++Iter;
    }
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
