/*!
  @file           FBM_DataVolumeIterator.hpp
  @author         Henrik
  @ingroup        FBM

\if EMIT_LICENCE
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

\endif
*/

#ifndef FBM_DATAVOLUMEITERATOR_H
#define FBM_DATAVOLUMEITERATOR_H

/*!
  @class    FBM_DataVolumeIterator
  @brief    Iterator over all registered and non sequential data volumes

 */

#include "FreeBlockManagement/FBM_DataVolumeArray.hpp"
#include "FreeBlockManagement/FBM_IDataVolume.hpp"

class FBM_DataVolumeIterator
{
public:
    
    /*!
         \brief constructor of FBM_DataVolumeIterator
         \param dataVolumes [in] array of data volumes
    */
    FBM_DataVolumeIterator(const FBM_DataVolumeArray &dataVolumes):
    m_dataVolumes(dataVolumes),
    m_pos(dataVolumes.GetFirstVolumeNo())
    {
        // search first valid data volume
        while ((m_pos <= m_dataVolumes.GetLastVolumeNo()) &&
               ( !m_dataVolumes.IsRegistered( m_pos ) ||
               m_dataVolumes[m_pos].VolMode() != RTE_VolumeAccessModeNormal))
        {
            m_pos++;
        }
    };

    FBM_IDataVolume * operator*() const
    {
        if (m_pos > m_dataVolumes.GetLastVolumeNo())
        {
            return NULL;
        }
        else
        {
            return (FBM_IDataVolume*)&m_dataVolumes[m_pos];
        }
    }


    FBM_IDataVolume * operator++()
    {
        do {
            m_pos++;
        }
        while ((m_pos <= m_dataVolumes.GetLastVolumeNo()) &&
               ( !m_dataVolumes.IsRegistered( m_pos ) ||
               m_dataVolumes[m_pos].VolMode() != RTE_VolumeAccessModeNormal));

        if (m_pos > m_dataVolumes.GetLastVolumeNo())
        {
            return NULL;
        }
        else
        {
            return (FBM_IDataVolume*)&m_dataVolumes[m_pos];
        }
    }

    IOMan_VolumeNo GetPosition()
    {
        return m_pos;
    }


private:
    IOMan_VolumeNo       m_pos;

    const FBM_DataVolumeArray &m_dataVolumes;
};

#endif
