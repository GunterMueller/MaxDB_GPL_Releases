/*!
  @file     SysView_CommandCacheStatisticsReset.cpp
  @author   DirkT
  @ingroup  SystemViews
  @brief    Systemview COMMANDCACHESTATISTICSRESET for the SharedSQL_CommandCache, reset version
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
#include "SystemViews/SysView_CommandCacheStatisticsReset.hpp"
#include "SQLManager/SharedSQL/SharedSQL_SQLCache.hpp"
#include "SQLManager/SharedSQL/SharedSQL_CommandCacheStatistics.hpp"
//#include "SQLManager/SharedSQL/SharedSQL_Types.hpp"
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

void SysView_CommandCacheStatisticsReset::Create(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_CommandCacheStatisticsReset::Create", SysView_Trace, 5);

	m_Table->AppendCol (ITOCT_TIMESTAMP, SV_RESETDATE, 0);
    m_Table->AppendCol (ITOCT_FIXED, SV_USABLESIZE, 10);
    //m_Table->AppendCol (ITOCT_FIXED, SV_USEDSIZE, 10);
    //m_Table->AppendCol (ITOCT_FIXED, SV_USEDSIZEPERCENTAGE, 3);
    m_Table->AppendCol (ITOCT_FIXED, SV_MAXUSEDSIZE, 10);
    m_Table->AppendCol (ITOCT_FIXED, SV_MAXUSEDSIZEPERCENTAGE, 3);
                
    //m_Table->AppendCol (ITOCT_FIXED, SV_COMMANDCOUNT, 10);
    m_Table->AppendCol (ITOCT_FIXED, SV_MAXCOMMANDCOUNT, 10);
    m_Table->AppendCol (ITOCT_FIXED, SV_INSERTCOMMANDCOUNT, 10);
    m_Table->AppendCol (ITOCT_FIXED, SV_DELETECOMMANDCOUNT, 10);

    // the access data can be accessed through CACHESTATISTICSRESET
    //m_Table->AppendCol (ITOCT_FIXED, SV_ACCESSCOUNT, 10);
    //m_Table->AppendCol (ITOCT_FIXED, SV_SUCCESSFULACCESSCOUNT, 10);
    //m_Table->AppendCol (ITOCT_FIXED, SV_UNSUCCESSFULACCESSCOUNT, 10);
    //m_Table->AppendCol (ITOCT_FIXED, SV_HITRATE, 3);

    m_Table->AppendCol (ITOCT_FIXED, SV_FAILEDALLOCATECOUNT, 10);
    m_Table->AppendCol (ITOCT_FIXED, SV_CLEANUPCOUNT, 10);
    m_Table->AppendCol (ITOCT_FIXED, SV_CLEANUPCOMMANDCOUNT, 10);
    m_Table->AppendCol (ITOCT_FIXED, SV_INVALIDATECOUNT, 10);

    //m_Table->AppendCol (ITOCT_FIXED, SV_CURRENTUNLOADSTATEMENTCOUNT, 10);
    m_Table->AppendCol (ITOCT_FIXED, SV_UNLOADSTATEMENTCOUNT, 10);
    m_Table->AppendCol (ITOCT_FIXED, SV_RELOADSTATEMENTCOUNT, 10);
    m_Table->AppendCol (ITOCT_FIXED, SV_DELETEPLANCOUNT, 10);

    //m_Table->AppendCol (ITOCT_FIXED, SV_HASHTABLESLOTCOUNT, 5);
    //m_Table->AppendCol (ITOCT_FIXED, SV_USEDHASHTABLESLOTCOUNT, 5);
    m_Table->AppendCol (ITOCT_FIXED, SV_MAXUSEDHASHTABLESLOTCOUNT, 5);
    //m_Table->AppendCol (ITOCT_FIXED, SV_AVGHASHTABLECHAINENTRYCOUNT, 5);
    m_Table->AppendCol (ITOCT_FIXED, SV_MAXHASHTABLECHAINENTRYCOUNT, 5);}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_CommandCacheStatisticsReset::GetColCount(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_CommandCacheStatisticsReset::GetColCount", SysView_Trace, 5);

	return SV_CC_COMMANDCACHESTATISTICSRESET;
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_CommandCacheStatisticsReset::EstimateRows(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_CommandCacheStatisticsReset::EstimateRows", SysView_Trace, 5);

	return SV_ER_COMMANDCACHESTATISTICSRESET;
}

/*---------------------------------------------------------------------------*/

void SysView_CommandCacheStatisticsReset::Execute(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_CommandCacheStatisticsReset::Execute", SysView_Trace, 5);

// 15 Cols

    m_Table->GetCatalogTable();
    SharedSQL_SQLCache& SQLCache = m_Context.GetSharedSQL_SQLCache();
    SharedSQL_CommandCacheInfo S;
    SQLCache.GetCommandCacheInfoReset(S);
    SAPDB_Int2 Rate;
    RTE_KGS::Statistic const &instanceStatistic = RTE_KGS::Instance().GetStatistic();
    AddColumn( S.ResetTime, AsTimestamp);
        
    m_Table->MoveToCol (ITOVT_INT4, &S.UsableSize, 0);
    m_Table->MoveToCol (ITOVT_INT4, &S.MaxUsedSize, 0);
    if ( S.UsableSize > 0 )
    {
        Rate  = static_cast<SAPDB_Int2>(( S.MaxUsedSize * 100 ) / S.UsableSize);
        Rate += (( S.MaxUsedSize * 100 ) % S.UsableSize ? 1 : 0);
        m_Table->MoveToCol (ITOVT_INT2, &Rate, 0); // MaxUsedSizePercentage
    }
    else
    {
        m_Table->MoveToCol (ITOVT_NULL, &Rate, 0); // NULL
    }
                
    m_Table->MoveToCol (ITOVT_INT4, &S.MaxCommandCount, 0);
    m_Table->MoveToCol (ITOVT_INT4, &S.InsertCommandCount, 0);
    m_Table->MoveToCol (ITOVT_INT4, &S.DeleteCommandCount, 0);


    m_Table->MoveToCol (ITOVT_INT4, &S.FailedAllocateCount, 0);
    m_Table->MoveToCol (ITOVT_INT4, &S.CleanUpCount, 0);
    m_Table->MoveToCol (ITOVT_INT4, &S.CleanUpCommandCount, 0);
    m_Table->MoveToCol (ITOVT_INT4, &S.InvalidateCount, 0);

    m_Table->MoveToCol (ITOVT_INT4, &S.UnloadStmtCount, 0);
    m_Table->MoveToCol (ITOVT_INT4, &S.ReloadStmtCount, 0);
    m_Table->MoveToCol (ITOVT_INT4, &S.DeletePlanCount, 0);

    m_Table->MoveToCol (ITOVT_INT2, &S.HashTab_MaxUsedSlotCount, 0);
    m_Table->MoveToCol (ITOVT_INT2, &S.HashTab_MaxChainEntryCount, 0);
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
