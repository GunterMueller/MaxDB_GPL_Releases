/*!
  @file     SysView_CatalogCacheStatistics.cpp
  @author   ThomasA
  @ingroup  SystemViews
  @brief    Systemview CATALOGCACHESTATISTICS for the shared catalog cache
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

#include "SystemViews/SysView_Defines.hpp"
#include "SystemViews/SysView_ITableObj.hpp"
#include "SystemViews/SysView_CatalogCacheStatistics.hpp"
//#include "SQLManager/SQLMan_ContextSessionMembers.hpp"
#include "SQLManager/SQLMan_Context.hpp"
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

void SysView_CatalogCacheStatistics::Create(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_CatalogCacheStatistics::Create", SysView_Trace, 5);

    m_Table->AppendCol (ITOCT_FIXED, SV_RELEASEMEMORYCOUNT, 20);
    m_Table->AppendCol (ITOCT_FIXED, SV_ENTRYCOUNT,         10);
    m_Table->AppendCol (ITOCT_FIXED, SV_MAXENTRYCOUNT,      10);
    m_Table->AppendCol (ITOCT_FIXED, SV_ENTERLRU,           20);
    m_Table->AppendCol (ITOCT_FIXED, SV_REMOVELRU,          20);
    m_Table->AppendCol (ITOCT_FIXED, SV_INVALIDATECOUNT,    10);
    m_Table->AppendCol (ITOCT_FIXED, SV_LRUENTRYCOUNT,      10);
    m_Table->AppendCol (ITOCT_FIXED, "INVALIDHANDLECOUNT",  10);
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_CatalogCacheStatistics::GetColCount(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_CatalogCacheStatistics::GetColCount", SysView_Trace, 5);
	return 7;
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_CatalogCacheStatistics::EstimateRows(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_CatalogCacheStatistics::EstimateRows", SysView_Trace, 5);
	return 1;
}

/*---------------------------------------------------------------------------*/

void SysView_CatalogCacheStatistics::Execute(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_CatalogCacheStatistics::Execute", SysView_Trace, 5);
    m_Table->GetCatalogTable();
    Catalog_Cache::Cache_Statistics statistics;
    Catalog_Cache::GetAllStatistics (statistics);
    AddColumn(statistics.m_releasedMemoryCount);  
    AddColumn(statistics.m_entryCount);  
    AddColumn(statistics.m_maxEntryCount);
    AddColumn(statistics.m_enterLruCount);
    AddColumn(statistics.m_removeLruCount);
    AddColumn(statistics.m_invalidateCount);
    AddColumn(statistics.m_lruEntryCount);
    AddColumn(statistics.m_invalidHandleCount);
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool SysView_CatalogCacheStatistics::ResetCounter(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_CatalogCacheStatistics::ResetCounter", SysView_Trace, 5);

    m_Context.ThrowError (e_not_implemented, 2);
    return ( true );
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
