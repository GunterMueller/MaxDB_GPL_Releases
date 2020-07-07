/****************************************************************************/
/*!

  @file        SysView_CacheStatistics.cpp

  -------------------------------------------------------------------------

  @author      ElkeZ

  @ingroup     SystemViews
  @brief       This module implements the "CacheStatistics" view class.

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
#include "hak10.h"
#include "hak42.h"
#include "SystemViews/SysView_Defines.hpp"
#include "SystemViews/SysView_CacheStatistics.hpp"
#include "SQLManager/SharedSQL/SharedSQL_SQLCache.hpp"
#include "SQLManager/SharedSQL/SharedSQL_CommandCacheStatistics.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "SQLManager/SQLMan_ContextSessionMembers.hpp"
#include "SQLManager/SQLMan_SessionIterator.hpp"
#include "SQLManager/Catalog/Catalog_Cache.hpp"

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

void SysView_CacheStatistics::Create(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_CacheStatistics::Create", SysView_Trace, 5);

    m_Table->AppendCol (ITOCT_FIXED, SV_SESSIONID, 10);
	m_Table->AppendCol (ITOCT_CHAR, SV_NAME, 20);
	m_Table->AppendCol (ITOCT_FIXED, SV_ACCESSCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED, SV_SUCCESSFULACCESSCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED, SV_UNSUCCESSFULACCESSCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED, SV_HITRATE, 3);

}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_CacheStatistics::GetColCount(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_CacheStatistics::GetColCount", SysView_Trace, 5);

	return SV_CC_CACHESTATISTICS;
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_CacheStatistics::EstimateRows(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_CacheStatistics::EstimateRows", SysView_Trace, 5);

	return SV_ER_CACHESTATISTICS;
}

/*---------------------------------------------------------------------------*/

void SysView_CacheStatistics::Execute(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_CacheStatistics::Execute", SysView_Trace, 5);

    // 1. the old system views
	a42caches_statistics ( m_Context, *(m_Table->GetPascalObjPtr()) );

    // 2. new system views
    m_Table->GetCatalogTable();

    // 2.1 SharedSQL_CommandCache
    SharedSQL_SQLCache& SQLCache = m_Context.GetSharedSQL_SQLCache();
    SharedSQL_CommandCacheInfo S;
    SQLCache.GetCommandCacheInfo(S);
    SAPDB_Int2 Rate;
    // SharedSQL - Preparing
    AddColumn(); // null value for column sessionid
    m_Table->MoveToCol (ITOVT_CHARPTR, (SAPDB_Char*)"COMMANDPREPARE", 14);
    m_Table->MoveToCol (ITOVT_INT4, &S.AccessCountPrepare, 0);
    m_Table->MoveToCol (ITOVT_INT4, &S.SuccessfulAccessCountPrepare, 0);
    m_Table->MoveToCol (ITOVT_INT4, &S.UnSuccessfulAccessCountPrepare, 0);
    if ( S.AccessCountPrepare > 0 )
    {
        Rate = static_cast<SAPDB_Int2>(100.0 * double(S.SuccessfulAccessCountPrepare) / double(S.AccessCountPrepare));
        m_Table->MoveToCol (ITOVT_INT2, &Rate, 0); // Hitrate
    }
    else
    {
        m_Table->MoveToCol (ITOVT_NULL, &Rate, 0); // NULL
    }
    // SharedSQL - Executing
    AddColumn(); // null value for column sessionid
    m_Table->MoveToCol (ITOVT_CHARPTR, (SAPDB_Char*)"COMMANDEXECUTE", 14);
    m_Table->MoveToCol (ITOVT_INT4, &S.AccessCountExecute, 0);
    m_Table->MoveToCol (ITOVT_INT4, &S.SuccessfulAccessCountExecute, 0);
    m_Table->MoveToCol (ITOVT_INT4, &S.UnSuccessfulAccessCountExecute, 0);
    if ( S.AccessCountExecute > 0 )
    {
        Rate = static_cast<SAPDB_Int2>(100.0 * double(S.SuccessfulAccessCountExecute) / double(S.AccessCountExecute));
        m_Table->MoveToCol (ITOVT_INT2, &Rate, 0); // Hitrate
    }
    else
    {
        m_Table->MoveToCol (ITOVT_NULL, &Rate, 0); // NULL
    }
    // shared Catalog statistics
    SAPDB_UInt8 hitCount;
    SAPDB_UInt8 missCount;
    const bool getResetStatistics = true;
    Catalog_Cache::GetStatistics(!getResetStatistics, hitCount, missCount);
    SAPDB_UInt8 total = missCount + hitCount;
    AddColumn(); // null value for column sessionid
    AddColumn("CATALOGCACHE");
    AddColumn(total);
    AddColumn(hitCount);
    AddColumn(missCount);
    if (total > 0)
    {
        SAPDB_Int2 rate = static_cast<SAPDB_Int2>(100.0 * double( (double) hitCount / (double) total));
        AddColumn(rate);
    }
    else
    {
        AddColumn();
    }
    // catalog cache statistics
    SQLMan_SessionIterator sessionIterator;
    tgg91_SessionNo session;
    do
    {
       if (sessionIterator.Next())
       {
           sessionIterator.GetSessionId(session);
           SAPDB_Int4 i4 = 256 * (256 * (256 * session[0] + session[1]) + session[2]) + session[3]; 
           AddColumn(i4);
           AddColumn("CATALOG");
           SAPDB_Int4 accessCount;
           SAPDB_Int4 successful;
           int        hitRate;
           const bool doReset = true;
           sessionIterator.GetCatalogCacheStatistics (accessCount, successful, hitRate);
           AddColumn (accessCount);
           AddColumn (successful);
           AddColumn (accessCount - successful);
           if (accessCount > 0)
           {
               AddColumn (hitRate);
           }
           else
           {
               AddColumn();
           }
       }
       else
       {
           break;
       }
    }
    while (m_Context.IsOk());
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool SysView_CacheStatistics::ResetCounter(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_CacheStatistics::ResetCounter", SysView_Trace, 5);
     // catalog cache statistics
    Catalog_Cache::ResetStatistics();
    SQLMan_SessionIterator sessionIterator;
    do
    {
        if (sessionIterator.Next())
        {
            sessionIterator.ResetCatalogCacheStatistics();
        }
        else
        {
            break;
        }
    }
    while (m_Context.IsOk());
    m_Context.ThrowError (e_not_implemented, 2);
    return ( true );
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
