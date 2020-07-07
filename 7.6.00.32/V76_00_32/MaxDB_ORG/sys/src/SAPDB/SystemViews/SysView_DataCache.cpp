/****************************************************************************/
/*!

  @file        SysView_DataCache.cpp

  -------------------------------------------------------------------------

  @author      ElkeZ

  @ingroup     SystemViews
  @brief       This module implements the "DataCache" view class.

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
#include "SystemViews/SysView_Defines.hpp"
#include "SystemViews/SysView_ITableObj.hpp"
#include "SystemViews/SysView_DataCache.hpp"

#include "FrameControl/FrameCtrl_ICommon.hpp"

#include "hbd20_10.h"

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

void SysView_DataCache::Create()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_DataCache::Create", SysView_Trace, 5);

    m_Table->AppendCol (ITOCT_FIXED ,SV_USABLESIZE, 20);
    m_Table->AppendCol (ITOCT_FIXED ,SV_USEDSIZE, 20);
    m_Table->AppendCol (ITOCT_FIXED ,SV_MAXUSEDSIZE, 20);
    m_Table->AppendCol (ITOCT_FIXED ,SV_OMSDATASIZE, 20);
    m_Table->AppendCol (ITOCT_FIXED ,SV_OMSUNLOADEDVERSIONSIZE, 20);
    m_Table->AppendCol (ITOCT_FIXED ,SV_HISTORYDATASIZE, 20);
    m_Table->AppendCol (ITOCT_FIXED ,SV_SQLDATASIZE, 20);
    m_Table->AppendCol (ITOCT_FIXED ,SV_CHANGEDDATASIZE, 20);
    m_Table->AppendCol (ITOCT_FIXED ,SV_PENDINGOPERATIONCOUNT, 10);
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_DataCache::GetColCount()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_DataCache::GetColCount", SysView_Trace, 5);

    return SV_CC_DATACACHE;
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_DataCache::EstimateRows()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_DataCache::EstimateRows", SysView_Trace, 5);

    return SV_ER_DATACACHE;
}

/*---------------------------------------------------------------------------*/

void SysView_DataCache::Execute()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_DataCache::Execute", SysView_Trace, 5);

    const SAPDB_UInt4   pageSize = FrameCtrl_ICommon::GetInstance().GetPageSize() / 1024;

    SAPDB_Int4  usableSize             = 0;
    SAPDB_Int4  usedSize               = 0;
    SAPDB_Int4  maxUsedSize            = 0;
    SAPDB_Int4  omsDataPageCnt         = 0;
    SAPDB_Int4  omsUnloadedVersPageCnt = 0;
    SAPDB_Int4  omsHistoryPageCnt      = 0;
    SAPDB_Int4  sqlDataPageCnt         = 0;
    SAPDB_Int4  changedPageCnt         = 0;
    SAPDB_Int4  pendingSyncSvpCnt      = 0;

    bd20SystemViewDataCache( usableSize, usedSize, maxUsedSize, omsDataPageCnt,
                             omsUnloadedVersPageCnt, omsHistoryPageCnt, 
                             sqlDataPageCnt, changedPageCnt, pendingSyncSvpCnt );

    m_Table->GetCatalogTable();

    // USABLESIZE
    const SAPDB_UInt4 _usableSize = usableSize * pageSize;
    m_Table->MoveToCol( ITOVT_UINT4, & _usableSize, 0 );
    
    // USEDSIZE
    const SAPDB_UInt4 _usedSize = usedSize * pageSize;
    m_Table->MoveToCol( ITOVT_UINT4, & _usedSize, 0 );
    
    // MAXUSEDSIZE
    const SAPDB_UInt4 _maxUsedSize = maxUsedSize * pageSize;
    m_Table->MoveToCol( ITOVT_UINT4, & _maxUsedSize, 0 );
    
    // OMSDATASIZE
    const SAPDB_UInt4 _omsDataPageCnt = omsDataPageCnt * pageSize;
    m_Table->MoveToCol( ITOVT_UINT4, & _omsDataPageCnt, 0 );

    // OMSUNLOADEDVERSIONSIZE
    const SAPDB_UInt4 _omsUnloadedVersPageCnt = omsUnloadedVersPageCnt * pageSize;
    m_Table->MoveToCol( ITOVT_UINT4, &_omsUnloadedVersPageCnt, 0 );
    
    // HISTORYDATASIZE
    const SAPDB_UInt4 _omsHistoryPageCnt = omsHistoryPageCnt * pageSize;
    m_Table->MoveToCol( ITOVT_UINT4, & _omsHistoryPageCnt, 0 );

    // SQLDATASIZE
    const SAPDB_UInt4 _sqlDataPageCnt = sqlDataPageCnt * pageSize;
    m_Table->MoveToCol( ITOVT_UINT4, & _sqlDataPageCnt, 0 );

    // CHANGEDDATASIZE
    const SAPDB_UInt4 _changedPageCnt = changedPageCnt * pageSize;
    m_Table->MoveToCol( ITOVT_UINT4, & _changedPageCnt, 0 );

    // PENDINGOPERATIONCOUNT
    m_Table->MoveToCol( ITOVT_UINT4, & pendingSyncSvpCnt, 0 );
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
