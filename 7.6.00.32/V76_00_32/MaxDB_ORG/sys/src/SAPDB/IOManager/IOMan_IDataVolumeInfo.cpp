/*!
  @file     IOMan_IDataVolumeInfo.cpp
  @author   TorstenS
  @ingroup  IOManagement
  @brief    Interface for data volume informations
  @see            

\if EMIT_LICENCE

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



\endif
*/




/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "IOManager/IOMan_IDataVolumeInfo.hpp"
#include "IOManager/IOMan_Manager.hpp"
#include "FreeBlockManagement/FBM_IManager.hpp"

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
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/



/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

SAPDB_Bool 
IOMan_DataVolumeInfo::IsVolumeOnline() const
{
    return( IOMan_Manager::GetInstance().IsDataVolumeOnline( m_VolumeNo ));
}

SAPDB_Bool 
IOMan_DataVolumeInfo::IsVolumeToDrop() const
{
    return( IOMan_Manager::GetInstance().IsDataVolumeToDrop( m_VolumeNo ));
}


/*---------------------------------------------------------------------------*/

SAPDB_Bool
IOMan_DataVolumeInfo::GetVolumeName( tsp00_VFilename   &volumeName ) const
{
    return( IOMan_Manager::GetInstance().GetDataVolumeName( m_VolumeNo, volumeName ));
}

/*---------------------------------------------------------------------------*/

IOMan_BlockCount
IOMan_DataVolumeInfo::GetVolumeSize() const
{
    return( IOMan_Manager::GetInstance().GetDataVolumeSize( m_VolumeNo ));
}

/*---------------------------------------------------------------------------*/

IOMan_BlockCount
IOMan_DataVolumeInfo::GetUsableVolumeSize() const
{
    return( IOMan_Manager::GetInstance().GetDataVolumeUsableSize( m_VolumeNo ));
}

/*---------------------------------------------------------------------------*/

IOMan_BlockCount 
IOMan_DataVolumeInfo::GetUsedVolumeSize() const
{
    if( IsVolumeOnline() )
        return( FBM_IManager::Instance().NumBlocksUsed( m_VolumeNo ) - 1); // don't incl. IOMan_Infopage in used size for statistics
    else
        return( 0 );
}

/*---------------------------------------------------------------------------*/

IOMan_BlockCount
IOMan_DataVolumeInfo::GetTotalClusterAreaSize() const
{
	if( IsVolumeOnline() )
		return( FBM_IManager::Instance().GetTotalClusterAreaSize( m_VolumeNo ));
	else
		return( 0 );
}

/*---------------------------------------------------------------------------*/

IOMan_BlockCount
IOMan_DataVolumeInfo::GetReservedClusterAreaSize() const
{
	if( IsVolumeOnline() )
		return( FBM_IManager::Instance().GetReservedClusterAreaSize( m_VolumeNo ));
	else
		return( 0 );
}

/*---------------------------------------------------------------------------*/

IOMan_BlockCount
IOMan_DataVolumeInfo::GetUsedClusterAreaSize() const
{
	if( IsVolumeOnline() )
		return( FBM_IManager::Instance().GetUsedClusterAreaSize( m_VolumeNo ));
	else
		return( 0 );
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

IOMan_DataVolumeInfoIterator::IOMan_DataVolumeInfoIterator()
:m_VolumeInfo( IOMan_Manager::GetInstance().GetFirstDataVolumeNo() )
{
}

/*---------------------------------------------------------------------------*/

void
IOMan_DataVolumeInfoIterator::Begin()
{
    m_VolumeInfo.SetVolumeNo( IOMan_Manager::GetInstance().GetFirstDataVolumeNo());  
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
IOMan_DataVolumeInfoIterator::End() const
{
    return( ! IOMan_Manager::GetInstance().IsDataVolumeNoValid( m_VolumeInfo.GetVolumeNo() ));
}

/*---------------------------------------------------------------------------*/

void
IOMan_DataVolumeInfoIterator::operator++()
{
    const IOMan_DeviceNo nextDevNo = 
        IOMan_Manager::GetInstance().GetNextDataVolumeNo( m_VolumeInfo.GetVolumeNo()  );

    m_VolumeInfo.SetVolumeNo( nextDevNo );
}

/*---------------------------------------------------------------------------*/

const IOMan_DataVolumeInfo&
IOMan_DataVolumeInfoIterator::operator*() const
{
    return( m_VolumeInfo );
}


/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
