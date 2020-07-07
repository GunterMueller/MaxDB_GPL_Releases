/****************************************************************************/
/*!

  @file        SysView_DataStatisticsReset.cpp

  -------------------------------------------------------------------------

  @author      ElkeZ

  @ingroup     SystemViews
  @brief       This module implements the "DataStatisticsReset" view class.

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
#include "SystemViews/SysView_DataStatisticsReset.hpp"

#include "Converter/Converter_ICommon.hpp"
#include "FrameControl/FrameCtrl_ICommon.hpp"
#include "IOManager/IOMan_IDataInfo.hpp"


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

void SysView_DataStatisticsReset::Create()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_DataStatisticsReset::Create", SysView_Trace, 5);

	m_Table->AppendCol (ITOCT_TIMESTAMP, SV_RESETDATE, 0);
    m_Table->AppendCol (ITOCT_FIXED ,   SV_USABLESIZE, 20);
    m_Table->AppendCol (ITOCT_FIXED ,   SV_MAXUSEDSIZE, 20);
    m_Table->AppendCol (ITOCT_FIXED ,   SV_MAXUSEDPERMANENTSIZE, 20);
    m_Table->AppendCol (ITOCT_FIXED ,   SV_MAXUSEDTEMPORARYSIZE, 20);
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_DataStatisticsReset::GetColCount()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_DataStatisticsReset::GetColCount", SysView_Trace, 5);

    return SV_CC_DATASTATISTICSRESET;
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_DataStatisticsReset::EstimateRows()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_DataStatisticsReset::EstimateRows", SysView_Trace, 5);

    return SV_ER_DATASTATISTICSRESET;
}

/*---------------------------------------------------------------------------*/

void SysView_DataStatisticsReset::Execute()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_DataStatisticsReset::Execute", SysView_Trace, 5);


    Converter_ICommon   &iConvCom = Converter_ICommon::Instance();
    const SAPDB_UInt4   pageSize  = FrameCtrl_ICommon::GetInstance().GetPageSize() / 1024;

    m_Table->GetCatalogTable();

    // RESETDATE

    const SAPDB_UInt8 resetTime = iConvCom.GetSystemViewResetTimeStamp();
    AddColumn( resetTime, AsTimestamp);
        
    // USABLE_SIZE

    SAPDB_UInt usableSize = IOMan_IDataInfo::GetInstance().TotalUsableDataPages() * pageSize;
    
    m_Table->MoveToCol( ITOVT_UINT4, &usableSize, 0 );

    // MAX_USED_SIZE

    SAPDB_UInt permPeak     = iConvCom.GetPeakForFrozenPermPages() * pageSize;
    SAPDB_UInt tempPeak     = iConvCom.GetPeakForFrozenTempPages() * pageSize;
    SAPDB_UInt maxPagesUsed = SAPDB_MAX( permPeak, tempPeak );

    m_Table->MoveToCol( ITOVT_UINT4, &maxPagesUsed, 0 );

    // MAX_USED_PERMANENT_SIZE

    m_Table->MoveToCol( ITOVT_UINT4, &permPeak, 0 );

    // MAX_USED_TEMPORARY_SIZE

    m_Table->MoveToCol( ITOVT_UINT4, &tempPeak, 0 );
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
