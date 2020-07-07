/*!
  @file     SysView_DataVolumes.cpp
  @author   TorstenS
  @ingroup  SystemViews
  @brief    This contains a class object representing the DATAVOLUMES view.
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

#include "gsp00.h"
#include "ggg00.h"

#include "FrameControl/FrameCtrl_ICommon.hpp"
#include "FreeBlockManagement/FBM_IManager.hpp"
#include "IOManager/IOMan_IDataVolumeInfo.hpp"
#include "SystemViews/SysView_Defines.hpp"
#include "SystemViews/SysView_ITableObj.hpp"
#include "SystemViews/SysView_DataVolumes.hpp"
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

void SysView_DataVolumes::Create(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_DataVolumes::Create", SysView_Trace, 5);

    m_Table->AppendCol (ITOCT_FIXED,        SV_ID, 5);
    m_Table->AppendCol (ITOCT_CHAR,         SV_MODE, 10);
    m_Table->AppendCol (ITOCT_FIXED,        SV_CONFIGUREDSIZE, 20);
    m_Table->AppendCol (ITOCT_FIXED,        SV_USABLESIZE, 20);
    m_Table->AppendCol (ITOCT_FIXED,        SV_USEDSIZE, 20);
    m_Table->AppendCol (ITOCT_FIXED,        SV_USEDSIZEPERCENTAGE, 3);
	m_Table->AppendCol (ITOCT_CHAR,         SV_DROPVOLUME, 3);
	m_Table->AppendCol (ITOCT_FIXED,        SV_TOTALCLUSTERAREASIZE, 20);
	m_Table->AppendCol (ITOCT_FIXED,        SV_RESERVEDCLUSTERAREASIZE, 20);
	m_Table->AppendCol (ITOCT_FIXED,        SV_USEDCLUSTERAREASIZE, 20);
	m_Table->AppendCol (ITOCT_CHARUNICODE,  SV_PATH, 256);
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_DataVolumes::GetColCount(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_DataVolumes::GetColCount", SysView_Trace, 5);

    return SV_CC_DATAVOLUMES;
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_DataVolumes::EstimateRows(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_DataVolumes::EstimateRows", SysView_Trace, 5);

    return SV_ER_DATAVOLUMES;
}

/*---------------------------------------------------------------------------*/

void SysView_DataVolumes::Execute(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_DataVolumes::Execute", SysView_Trace, 5);

    const SAPDB_UInt4 pageSize = (SAPDB_UInt4)(FrameCtrl_ICommon::GetInstance().GetPageSize() / 1024);

    IOMan_DataVolumeInfoIterator     iter;
    SAPDB_UInt4                      devNo;
    tsp00_VFilename                  devName;
    SAPDB_UInt4                      devSizeConfigured;
    SAPDB_UInt4                      devSizeUsable;
    SAPDB_UInt4                      devSizeUsed;
	SAPDB_UInt4                      devTotalClusterArea;
	SAPDB_UInt4                      devReservedClusterArea;
	SAPDB_UInt4                      devUsedClusterArea;
	SAPDB_UInt4                      devUsedPercent;
    SAPDB_Bool                       devDrop;

    m_Table->GetCatalogTable();

    iter.Begin();

    while ((!iter.End()) && m_Context.IsOk())
    {
        if ((*iter).IsVolumeOnline())
        {
            if (! (*iter).GetVolumeName( devName ))
            {
                m_Context.ThrowError (e_devname_invalid, 1);
                break;
            }
            devNo             = (SAPDB_UInt4)((*iter).GetVolumeNo());
            devSizeConfigured = (SAPDB_UInt4)((*iter).GetVolumeSize() * pageSize );
            devSizeUsable     = (SAPDB_UInt4)((*iter).GetUsableVolumeSize() * pageSize );
            devSizeUsed		  = (SAPDB_UInt4)((*iter).GetUsedVolumeSize() * pageSize );
			devTotalClusterArea    = (SAPDB_UInt4)((*iter).GetTotalClusterAreaSize() * pageSize );
			devUsedClusterArea     = (SAPDB_UInt4)((*iter).GetUsedClusterAreaSize() * pageSize );
			devReservedClusterArea = (SAPDB_UInt4)((*iter).GetReservedClusterAreaSize() * pageSize );
            devDrop           = (*iter).IsVolumeToDrop();


            if (devSizeUsable > 0)
            {
                devUsedPercent = (SAPDB_UInt4)(((devSizeUsed * 100) / devSizeUsable));
            }
            else
            {
                devUsedPercent = 0;
            }

            m_Table->MoveToCol( ITOVT_UINT4, &devNo, 0 );

            switch( FBM_IManager::Instance().GetVolMode( devNo ))
            {
            case RTE_VolumeAccessModeNormal:
                m_Table->MoveToCol (ITOVT_CHARPTR, (SAPDB_Char *)"NORMAL", 6);
                break;
            case RTE_VolumeAccessModeSequential:
                m_Table->MoveToCol (ITOVT_CHARPTR, (SAPDB_Char *)"SEQUENTIAL", 10);
                break;
            default:
                m_Table->MoveToCol (ITOVT_CHARPTR, (SAPDB_Char *)"UNKNOWN", 7);
            }

            m_Table->MoveToCol( ITOVT_UINT4, &devSizeConfigured, 0 );
            m_Table->MoveToCol( ITOVT_UINT4, &devSizeUsable, 0 );
            m_Table->MoveToCol( ITOVT_UINT4, &devSizeUsed, 0 );
            m_Table->MoveToCol( ITOVT_UINT4, &devUsedPercent, 0 );
            m_Table->MoveToCol( ITOVT_YESNO, &devDrop, 0);
			m_Table->MoveToCol( ITOVT_UINT4, &devTotalClusterArea, 0 );
			m_Table->MoveToCol( ITOVT_UINT4, &devReservedClusterArea, 0 );
			m_Table->MoveToCol( ITOVT_UINT4, &devUsedClusterArea, 0 );
			m_Table->MoveToCol (ITOVT_UTF8PTR, devName.asCharp(), devName.length());
        }
        ++iter;
    }
}

/*===========================================================================*
*  END OF CODE                                                              *
*===========================================================================*/
