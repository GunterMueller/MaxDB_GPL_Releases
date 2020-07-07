/*!
  @file     SysView_Snapshots.cpp
  @author   TorstenS
  @ingroup  SystemViews
  @brief    This contains a class object representing the snapshots view.
  @see            
*/
/*

    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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



*/



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/


#include "Converter/Converter_ICommon.hpp"
#include "FrameControl/FrameCtrl_ICommon.hpp"
#include "KernelCommon/Kernel_DateTime.hpp"
#include "RunTime/RTE_Types.h"
#include "SystemViews/SysView_Defines.hpp"
#include "SystemViews/SysView_Snapshots.hpp"


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

void SysView_Snapshots::Create()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_Snapshots::Create", SysView_Trace, 5);

    m_Table->AppendCol (ITOCT_FIXED ,       SV_ID, 10);
    m_Table->AppendCol (ITOCT_TIMESTAMP,    SV_CREATEDATE, 0);
    m_Table->AppendCol (ITOCT_FIXED ,       SV_USEDSIZE, 20);
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_Snapshots::GetColCount()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_Snapshots::GetColCount", SysView_Trace, 5);

    return SV_CC_SNAPSHOTS;
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_Snapshots::EstimateRows()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_Snapshots::EstimateRows", SysView_Trace, 5);

    return SV_ER_SNAPSHOTS;
}

/*---------------------------------------------------------------------------*/

void SysView_Snapshots::Execute()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_Snapshots::Execute", SysView_Trace, 5);

    Converter_ICommon   &iConvCom = Converter_ICommon::Instance();
    const SAPDB_UInt4   pageSize  = FrameCtrl_ICommon::GetInstance().GetPageSize() / 1024;

    m_Table->GetCatalogTable();

    // ID

    Converter_Version snapShotVersion = iConvCom.GetSnapShotVersion();

    if( snapShotVersion.IsInvalid())
        return;

     m_Table->MoveToCol( ITOVT_UINT4, &snapShotVersion, 0 );

    // CREATE_DATE

    RTESys_SQLTimestamp timeStamp;
    Kernel_DateTime     dateTime = iConvCom.GetSnapShotCreationTime();

    dateTime.GetDateTimeAsSQLTimeStamp( timeStamp );
    
    m_Table->MoveToCol (ITOVT_CHARPTR, &timeStamp, sizeof( RTESys_SQLTimestamp ));

    // USED_SIZE

    SAPDB_UInt snapShotSize = iConvCom.GetNumberOfSnapShotPages() * pageSize;

    m_Table->MoveToCol( ITOVT_UINT4, &snapShotSize, 0 );
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
