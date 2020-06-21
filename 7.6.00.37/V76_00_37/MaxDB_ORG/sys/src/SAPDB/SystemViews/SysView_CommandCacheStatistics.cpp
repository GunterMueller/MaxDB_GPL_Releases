/*!
  @file     SysView_CommandCacheStatistics.cpp
  @author   DirkT
  @ingroup  SystemViews
  @brief    Systemview COMMANDCACHESTATISTICS for the SharedSQL_CommandCache
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
#include "SystemViews/SysView_CommandCacheStatistics.hpp"
#include "SQLManager/SharedSQL/SharedSQL_SQLCache.hpp"
#include "SQLManager/SharedSQL/SharedSQL_CommandCacheStatistics.hpp"
//#include "SQLManager/SharedSQL/SharedSQL_Types.hpp"
#include "SQLManager/SQLMan_ContextSessionMembers.hpp"
#include "SQLManager/SQLMan_Context.hpp"

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

void SysView_CommandCacheStatistics::Create(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_CommandCacheStatistics::Create", SysView_Trace, 5);

    m_Table->AppendCol (ITOCT_FIXED, SV_USABLESIZE, 10);
    m_Table->AppendCol (ITOCT_FIXED, SV_USEDSIZE, 10);
    m_Table->AppendCol (ITOCT_FIXED, SV_USEDSIZEPERCENTAGE, 3);
    m_Table->AppendCol (ITOCT_FIXED, SV_MAXUSEDSIZE, 10);
    m_Table->AppendCol (ITOCT_FIXED, SV_MAXUSEDSIZEPERCENTAGE, 3);
                
    m_Table->AppendCol (ITOCT_FIXED, SV_COMMANDCOUNT, 10);
    m_Table->AppendCol (ITOCT_FIXED, SV_MAXCOMMANDCOUNT, 10);
    m_Table->AppendCol (ITOCT_FIXED, SV_INSERTCOMMANDCOUNT, 10);
    m_Table->AppendCol (ITOCT_FIXED, SV_DELETECOMMANDCOUNT, 10);

    // the access data can be accessed through CACHESTATISTICSRESET
    //m_Table->AppendCol (ITOCT_FIXED, SV_ACCESSCOUNT, 11, 10);
    //m_Table->AppendCol (ITOCT_FIXED, SV_SUCCESSFULACCESSCOUNT, 21, 10);
    //m_Table->AppendCol (ITOCT_FIXED, SV_UNSUCCESSFULACCESSCOUNT, 23, 10);
    //m_Table->AppendCol (ITOCT_FIXED, SV_HITRATE, 7, 3);

    m_Table->AppendCol (ITOCT_FIXED, SV_FAILEDALLOCATECOUNT, 10);
    m_Table->AppendCol (ITOCT_FIXED, SV_CLEANUPCOUNT, 10);
    m_Table->AppendCol (ITOCT_FIXED, SV_CLEANUPCOMMANDCOUNT, 10);
    m_Table->AppendCol (ITOCT_FIXED, SV_INVALIDATECOUNT, 10);

    m_Table->AppendCol (ITOCT_FIXED, SV_CURRENTUNLOADSTATEMENTCOUNT, 10);
    m_Table->AppendCol (ITOCT_FIXED, SV_UNLOADSTATEMENTCOUNT, 10);
    m_Table->AppendCol (ITOCT_FIXED, SV_RELOADSTATEMENTCOUNT, 10);
    m_Table->AppendCol (ITOCT_FIXED, SV_DELETEPLANCOUNT, 10);

    m_Table->AppendCol (ITOCT_FIXED, SV_HASHTABLESLOTCOUNT, 5);
    m_Table->AppendCol (ITOCT_FIXED, SV_USEDHASHTABLESLOTCOUNT, 5);
    m_Table->AppendCol (ITOCT_FIXED, SV_MAXUSEDHASHTABLESLOTCOUNT, 5);
    m_Table->AppendCol (ITOCT_FIXED, SV_AVGHASHTABLECHAINENTRYCOUNT, 5);
    m_Table->AppendCol (ITOCT_FIXED, SV_MAXHASHTABLECHAINENTRYCOUNT, 5);
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_CommandCacheStatistics::GetColCount(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_CommandCacheStatistics::GetColCount", SysView_Trace, 5);

	return SV_CC_COMMANDCACHESTATISTICS;
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_CommandCacheStatistics::EstimateRows(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_CommandCacheStatistics::EstimateRows", SysView_Trace, 5);

	return SV_ER_COMMANDCACHESTATISTICS;
}

/*---------------------------------------------------------------------------*/

void SysView_CommandCacheStatistics::Execute(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_CommandCacheStatistics::Execute", SysView_Trace, 5);

// 22 Cols

    m_Table->GetCatalogTable();
    SharedSQL_SQLCache& SQLCache = m_Context.GetSharedSQL_SQLCache();
    SharedSQL_CommandCacheInfo S; 
    SQLCache.GetCommandCacheInfo(S);
    SAPDB_Int2 Rate;

    m_Table->MoveToCol (ITOVT_INT4, &S.UsableSize, 0);
    m_Table->MoveToCol (ITOVT_INT4, &S.UsedSize, 0);
    if ( S.UsableSize > 0 )
    {
        Rate  = static_cast<SAPDB_Int2>(( S.UsedSize * 100 ) / S.UsableSize);
        Rate += (( S.UsedSize * 100 ) % S.UsableSize ? 1 : 0);
        m_Table->MoveToCol (ITOVT_INT2, &Rate, 0); // UsedSizePercentage
    }
    else
    {
        m_Table->MoveToCol (ITOVT_NULL, &Rate, 0); // NULL
    }
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

    m_Table->MoveToCol (ITOVT_INT4, &S.CommandCount, 0);
    m_Table->MoveToCol (ITOVT_INT4, &S.MaxCommandCount, 0);
    m_Table->MoveToCol (ITOVT_INT4, &S.InsertCommandCount, 0);
    m_Table->MoveToCol (ITOVT_INT4, &S.DeleteCommandCount, 0);

    //m_Table->MoveToCol (ITOVT_INT4, &S.AccessCount, 0);
    //m_Table->MoveToCol (ITOVT_INT4, &S.SuccessfulAccessCount, 0);
    //m_Table->MoveToCol (ITOVT_INT4, &S.UnSuccessfulAccessCount, 0);
    //if ( S.AccessCount > 0 )
    //{
    //    Rate = static_cast<SAPDB_Int2>(( S.SuccessfulAccessCount * 100 ) / S.AccessCount);
    //    m_Table->MoveToCol (ITOVT_INT2, &Rate, 0); // Hitrate
    //}
    //else
    //{
    //    m_Table->MoveToCol (ITOVT_NULL, &Rate, 0); // NULL
    //}

    m_Table->MoveToCol (ITOVT_INT4, &S.FailedAllocateCount, 0);
    m_Table->MoveToCol (ITOVT_INT4, &S.CleanUpCount, 0);
    m_Table->MoveToCol (ITOVT_INT4, &S.CleanUpCommandCount, 0);
    m_Table->MoveToCol (ITOVT_INT4, &S.InvalidateCount, 0);

    m_Table->MoveToCol (ITOVT_INT4, &S.CurrentUnloadStmtCount, 0);
    m_Table->MoveToCol (ITOVT_INT4, &S.UnloadStmtCount, 0);
    m_Table->MoveToCol (ITOVT_INT4, &S.ReloadStmtCount, 0);
    m_Table->MoveToCol (ITOVT_INT4, &S.DeletePlanCount, 0);

    m_Table->MoveToCol (ITOVT_INT2, &S.HashTab_SlotCount, 0);
    m_Table->MoveToCol (ITOVT_INT2, &S.HashTab_UsedSlotCount, 0);
    m_Table->MoveToCol (ITOVT_INT2, &S.HashTab_MaxUsedSlotCount, 0);
    if ( S.HashTab_UsedSlotCount > 0 )
    {
        Rate  = S.CommandCount / S.HashTab_UsedSlotCount;
        Rate += (S.CommandCount % S.HashTab_UsedSlotCount ? 1 : 0);
        m_Table->MoveToCol (ITOVT_INT2, &Rate, 0);
    }
    else
    {
        m_Table->MoveToCol (ITOVT_NULL, &Rate, 0); // NULL
    }
    m_Table->MoveToCol (ITOVT_INT2, &S.HashTab_MaxChainEntryCount, 0);
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool SysView_CommandCacheStatistics::ResetCounter(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_CommandCacheStatistics::ResetCounter", SysView_Trace, 5);

	SharedSQL_SQLCache& SQLCache = m_Context.GetSharedSQL_SQLCache();
	SQLCache.ResetCommandCacheInfo();

    return ( true );
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
