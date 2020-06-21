/*!
  @file           FBM_DataVolumeArray.hpp
  @author         AlexanderK
  @author         TorstenS
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

#ifndef FBM_DATAVOLUMEARRAY_H
#define FBM_DATAVOLUMEARRAY_H

/*!
  @class    FBM_DataVolumeArray
  @brief    Array of all attached data volumes

 */

#include "FreeBlockManagement/FBM_IVolume.hpp"
#include "Container/Container_Vector.hpp"

class FBM_DataVolumeArray
{
public:

    /*!
       @brief   Default constructor
       @param   allocator [in] Allocator for memory requests
       @return  none

       - create an empty array of zero length
     */
    FBM_DataVolumeArray(SAPDBMem_IRawAllocator &allocator)
    :m_DataVolumes  (allocator)
    {};

    /*!
       @brief   Creates an array of a given length
       @param   maxNumVolumes    [in] length of the array
       @return  none
     */
    void Initialize ( const IOMan_VolumeNo  maxNumVolumes )
    {
        m_MinVolumeIndex   = 1;
        m_MaxVolumeIndex   = m_MinVolumeIndex + maxNumVolumes - 1;
        m_FirstVolumeIndex = m_MaxVolumeIndex + 1;
        m_LastVolumeIndex  = 0;
        m_DataVolumes.Resize( maxNumVolumes + m_MinVolumeIndex );  // waste one pointer, index:0 is not used
    }

    /*!
       @brief   All memory is released an the member variables are set to null and zero
       @return  none
     */
    void Shutdown()
    {
        m_MinVolumeIndex.Invalidate();
        m_MaxVolumeIndex.Invalidate();
        m_DataVolumes.Delete();
    }

    /*!
       @brief    Returns a reference to a particular DataDevice from the array
       @param    volumeIndex [in] index of the device to return
       @return  (FBM_IVolume) reference to the wanted device
    */
    FBM_IVolume & operator[](const IOMan_VolumeNo volumeIndex)
    {
        SAPDBERR_ASSERT_ARGUMENT( IsVolumeNoValid( volumeIndex ));

        return *m_DataVolumes[ volumeIndex ];
    }

    /*!
       @brief    Returns a reference to a particular DataDevice from the array
       @param    volumeIndex [in] index of the device to return
       @return  (FBM_IVolume) reference to the wanted device
    */
    const FBM_IVolume & operator[](const IOMan_VolumeNo volumeIndex) const
    {
        SAPDBERR_ASSERT_ARGUMENT( IsVolumeNoValid( volumeIndex ));

        return *m_DataVolumes[ volumeIndex ];
    }

    /*!
       @brief   register a new data volume to the array. 
       @param   volumeIndex  [in] index of the volume to register
       @param   pDataDevice  [in] data volume
       @return  none
    */
    void Register(
        const IOMan_VolumeNo    volumeIndex, 
        FBM_IVolume*            pDataDevice)
    {
        SAPDBERR_ASSERT_STATE( NULL != pDataDevice );

        m_DataVolumes[ volumeIndex ] = pDataDevice;

        m_FirstVolumeIndex = SAPDB_MIN(m_FirstVolumeIndex, volumeIndex);
        m_LastVolumeIndex = SAPDB_MAX(m_LastVolumeIndex, volumeIndex);
    }

    /*!
        @brief  unregister a given data volume
        @param  volumeIndex [in] index of the volume to be unregistered
        @return none
    */
    void UnRegister( const IOMan_VolumeNo    volumeIndex )
    {
        m_DataVolumes[ volumeIndex ] = NULL;
        
        if (volumeIndex == m_FirstVolumeIndex) // was first volume ?
        {                                      // search new first volume
            do {
                m_FirstVolumeIndex++;
            } while ((m_LastVolumeIndex != m_FirstVolumeIndex) &&
                   (NULL == m_DataVolumes[ m_FirstVolumeIndex ]));
        }
  
        if (volumeIndex == m_LastVolumeIndex) // was last volume ?
        {                                     // search new last volume
            do {
                m_LastVolumeIndex--;
            } while ((m_LastVolumeIndex != m_FirstVolumeIndex) &&
                   (NULL == m_DataVolumes[ m_LastVolumeIndex ]));
        }
    }

    /*!
        @brief  Check if volume exist. Note that it is assumed that the given
                volumeIndex is valid. The validity of the volumeIndex could be 
                checked with IsVolumeNoValid(). See also method IsAvailable()
        @param  volumeIndex [in] index of the volume to be checked
        @return (bool) true if volume is assigned
    */
    bool IsRegistered( const IOMan_VolumeNo  volumeIndex ) const
    {
        return( NULL != m_DataVolumes[ volumeIndex ] );
    }
 
    /*!
        @brief  Check if the given volumeNo is within the lower and upper bound
                of the volume list. Note that if the volumeIndex is valid it is
                not clear that the corresponding volume is registered. This
                could be checked with IsRegistered(). See also method IsAvailable().
        @param  volumeIndex [in] index of the volume to be checked
        @return (bool) true if volume is assigned
    */
    bool IsVolumeNoValid( const IOMan_VolumeNo  volumeIndex ) const
    {
        return( volumeIndex >= m_MinVolumeIndex && volumeIndex <= m_MaxVolumeIndex );
    }

    /*!
        @brief  Check if the given volume index fulfils lower and upper bound
                and a volume is registered under this index. T
        @param  volumeIndex  [in] index of the volume to be checked
        @return (bool) true if volume index is valid and volume is registered
    */
    
    bool IsAvailable( const IOMan_VolumeNo  volumeIndex ) const
    {
        return( IsVolumeNoValid( volumeIndex ) && IsRegistered( volumeIndex ));
    }

    /*! 
        @brief Return volume index /number of the minimal configurable volume.
               It is not said if a volume is registered with this index
        @return (IOMan_VolumeNo) volume index
    */

    IOMan_VolumeNo GetMinVolumeNo() const
    {
        return( m_MinVolumeIndex );
    }
    
    /*! 
        @brief Return volume index /number of the first configured volume.
        @return (IOMan_VolumeNo) volume index
    */
    
    IOMan_VolumeNo GetFirstVolumeNo() const
    {
        return( m_FirstVolumeIndex  ); 
    }
   
    /*! 
        @brief Return volume index /number of the last configured volume.
        @return (IOMan_VolumeNo) volume index
    */
    
    IOMan_VolumeNo GetLastVolumeNo() const
    {
        return( m_LastVolumeIndex  ); 
    }
   
     /*! 
        @brief Return volume index /number of the maximal configurable volume.
               It is not said if a volume is registered with this index
        @return (IOMan_VolumeNo) volume index
    */
    IOMan_VolumeNo GetMaxVolumeNo() const
    {
        return( m_MaxVolumeIndex );
    }

private:

    Container_Vector<FBM_IVolume *>   m_DataVolumes;
    IOMan_VolumeNo                    m_MinVolumeIndex;
    IOMan_VolumeNo                    m_MaxVolumeIndex;
    IOMan_VolumeNo                    m_FirstVolumeIndex;
    IOMan_VolumeNo                    m_LastVolumeIndex;
};

#endif //FBM_DATAVOLUMEARRAY_H
