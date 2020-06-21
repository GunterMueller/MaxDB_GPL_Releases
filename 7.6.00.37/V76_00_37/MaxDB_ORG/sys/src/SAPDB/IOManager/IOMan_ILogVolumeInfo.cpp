/*!
  @file     IOMan_ILogVolumeInfo.cpp
  @author   TorstenS
  @ingroup  IOManagement
  @brief    Interface for log volume informations
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

#include "IOManager/IOMan_ILogVolumeInfo.hpp"
#include "IOManager/IOMan_Manager.hpp"

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
IOMan_LogVolumeInfo::IsVolumeOnline() const
{
    return( IOMan_Manager::GetInstance().IsLogVolumeOnline( m_VolumeNo ));
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
IOMan_LogVolumeInfo::GetMirrVolumeName( tsp00_VFilename   &mirrVolumeName ) const
{
    return( IOMan_Manager::GetInstance().GetMirrLogVolumeName( m_VolumeNo, mirrVolumeName ));
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
IOMan_LogVolumeInfo::GetVolumeName( tsp00_VFilename   &volumeName ) const
{
    return( IOMan_Manager::GetInstance().GetLogVolumeName( m_VolumeNo, volumeName ));
}
/*---------------------------------------------------------------------------*/

IOMan_BlockCount
IOMan_LogVolumeInfo::GetVolumeSize() const
{
    return( IOMan_Manager::GetInstance().GetLogVolumeSize( m_VolumeNo ));
}

/*---------------------------------------------------------------------------*/

IOMan_BlockCount
IOMan_LogVolumeInfo::GetUsableVolumeSize() const
{
    return( IOMan_Manager::GetInstance().GetLogVolumeUsableSize( m_VolumeNo ));
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

IOMan_LogVolumeInfoIterator::IOMan_LogVolumeInfoIterator()
:m_VolumeInfo( IOMan_Manager::GetInstance().GetFirstLogVolumeNo() )
{
}

/*---------------------------------------------------------------------------*/

void
IOMan_LogVolumeInfoIterator::Begin()
{
    m_VolumeInfo.SetVolumeNo( IOMan_Manager::GetInstance().GetFirstLogVolumeNo());  
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
IOMan_LogVolumeInfoIterator::End() const
{
    return( ! IOMan_Manager::GetInstance().IsLogVolumeNoValid( m_VolumeInfo.GetVolumeNo() ));
}

/*---------------------------------------------------------------------------*/

void
IOMan_LogVolumeInfoIterator::operator++()
{
    const IOMan_DeviceNo nextDevNo = 
        IOMan_Manager::GetInstance().GetNextLogVolumeNo( m_VolumeInfo.GetVolumeNo() );

    m_VolumeInfo.SetVolumeNo( nextDevNo );
}

/*---------------------------------------------------------------------------*/

const IOMan_LogVolumeInfo&
IOMan_LogVolumeInfoIterator::operator*() const
{
    return( m_VolumeInfo );
}


/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
