/****************************************************************************/
/*!

  @file        SysView_LogVolumes.cpp

  -------------------------------------------------------------------------

  @author      ElkeZ

  @ingroup     SystemViews
  @brief       This module implements the "LogVolumes" view class.

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

#include "FrameControl/FrameCtrl_ICommon.hpp"
#include "IOManager/IOMan_ILogVolumeInfo.hpp"
#include "IOManager/IOMan_ILogInfo.hpp"
#include "SystemViews/SysView_Defines.hpp"
#include "SystemViews/SysView_ITableObj.hpp"
#include "SystemViews/SysView_LogVolumes.hpp"
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

void SysView_LogVolumes::Create(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_LogVolumes::Create", SysView_Trace, 5);

    m_Table->AppendCol (ITOCT_FIXED, SV_ID, 5 );
    m_Table->AppendCol (ITOCT_FIXED, SV_CONFIGUREDSIZE, 20);
    m_Table->AppendCol (ITOCT_FIXED, SV_USABLESIZE, 20);
    m_Table->AppendCol (ITOCT_CHARUNICODE, SV_PATH, 256);
    m_Table->AppendCol (ITOCT_CHARUNICODE, SV_MIRROREDPATH, 256);

}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_LogVolumes::GetColCount(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_LogVolumes::GetColCount", SysView_Trace, 5);

    return SV_CC_LOGVOLUMES;
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_LogVolumes::EstimateRows(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_LogVolumes::EstimateRows", SysView_Trace, 5);

    return SV_ER_LOGVOLUMES;
}

/*---------------------------------------------------------------------------*/

void SysView_LogVolumes::Execute(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_LogVolumes::Execute", SysView_Trace, 5);

    const SAPDB_UInt4 pageSize   = (SAPDB_UInt4)(FrameCtrl_ICommon::GetInstance().GetPageSize() / 1024);
    const bool        bIsMirrord = IOMan_ILogInfo::GetInstance().IsLogMirrored();

    IOMan_LogVolumeInfoIterator      iter;
    SAPDB_UInt4                      devNo;
    tsp00_VFilename                  devName;
    tsp00_VFilename                  mirrDevName;
    SAPDB_UInt4                      devSizeConfigured;
    SAPDB_UInt4                      devSizeUsable;


    m_Table->GetCatalogTable();

    iter.Begin();

    while ((!iter.End()) && m_Context.IsOk())
    {
        if((*iter).IsVolumeOnline())
        {
            bool bFound = (*iter).GetVolumeName( devName );

            if( bFound && bIsMirrord ){
                bFound = (*iter).GetMirrVolumeName( mirrDevName );
            }

            if( ! bFound )
            {
                m_Context.ThrowError( e_devname_invalid, 1 );
                return;
            }

            devNo             = (SAPDB_UInt4)((*iter).GetVolumeNo());
            devSizeConfigured = (SAPDB_UInt4)((*iter).GetVolumeSize() * pageSize );
            devSizeUsable     = (SAPDB_UInt4)((*iter).GetUsableVolumeSize() * pageSize );

            m_Table->MoveToCol( ITOVT_UINT4,    &devNo, 0 );
            m_Table->MoveToCol( ITOVT_UINT4,    &devSizeConfigured, 0 );
            m_Table->MoveToCol( ITOVT_UINT4,    &devSizeUsable, 0 );
            m_Table->MoveToCol (ITOVT_UTF8PTR,  devName.asCharp(), devName.length());

            if( bIsMirrord )
                m_Table->MoveToCol (ITOVT_UTF8PTR,  mirrDevName.asCharp(), mirrDevName.length());
            else
                m_Table->MoveToCol (ITOVT_NULL,  NULL, 0);

        }
        ++iter;
    }
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
