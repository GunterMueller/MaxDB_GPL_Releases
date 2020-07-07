/*!
  @file     Converter_PageNoIterator.hpp
  @author   Henrik
  @ingroup  Converter
  @brief    iterator over all pages on a given volume / used by drop volume
*/

/*
  ========== licence begin  GPL
  Copyright (c) 2000-2004 SAP AG

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


#ifndef CONVERTER_PAGENOITERATOR_HPP
#define CONVERTER_PAGENOITERATOR_HPP

// subiterator
#include "Converter/Converter_MapSectionIterator.hpp"

#include "RunTime/Synchronisation/RTESync_RWRegion.hpp"
#include "RunTime/MemoryManagement/RTEMem_AllocatorWrapper.hpp"

class Converter_PermMapControl;
class Converter_PermStaticMapControl;

class Converter_PageNoIterator
{

public:


    Converter_PageNoIterator(IOMan_DeviceNo volNo,
                             const RTETask_ITask &task,
                             SAPDBMem_IRawAllocator  &allocator,
                             SAPDB_Bool iterEnd = SAPDB_FALSE);

    ~Converter_PageNoIterator(void)
    {
        if (m_pRwLock)
        {
            RTESync_DestroyRWRegion(m_pRwLock,m_Allocator);
        }
    }


    /*!
      @brief return next page on volume
      @param  pageNo[out] next page number
      @param  recMode[out] recovery mode of page
      @param  task[in] own task
      @return (SAPDB_Bool) page found
    */
    SAPDB_Bool GetNextPageNo(Data_PageNo &pageNo,
                   Data_PageRecoveryMode &recMode,
                   const RTETask_ITask &task );

    /*!
      @brief compare operator
      @param comp[in] compare value
      @return (SAPDB_Bool)
    */
    SAPDB_Bool operator==(const Converter_PageNoIterator &comp) const
    {
        return ((m_MapType == Map_INVALID) && (m_MapType == comp.m_MapType)) || (
            (m_MapType == comp.m_MapType) && (m_IntraMapIter == comp.m_IntraMapIter));
    }

    /*!
      @brief compare operator / not equal
      @param comp[in] compare value
      @return (SAPDB_Bool)
    */
    SAPDB_Bool operator!=(const Converter_PageNoIterator &comp) const
    {
        return ! ((*this)==comp);

    }


    /*!
      @brief assign operator
      @param source[in] source value
    */
    void operator=(Converter_PageNoIterator const &source)
    {
        m_MapType              = source.m_MapType;
        m_VolNo                = source.m_VolNo;
        m_IntraMapIter         = source.m_IntraMapIter;
        m_IntraMapEndIter      = source.m_IntraMapEndIter;
    }


private:


    /// local iterator states
    typedef enum {
       Map_Temp,
       Map_Perm,
       Map_PermStatic,
       Map_INVALID}
    MapType;

    /// subiterator
    Converter_MapSectionIterator   m_IntraMapIter;
    /// end of subiterator
    Converter_MapSectionIterator   m_IntraMapEndIter;

    /// perm map
    Converter_PermMapControl       &m_PermMapControl;
    /// static perm map
    Converter_PermStaticMapControl &m_PermStaticMapControl;

    /// local state
    MapType                        m_MapType;

    /// volume filter for page iterator
    IOMan_DeviceNo                 m_VolNo;

    /// allocator for rwLocks
    RTEMem_AllocatorWrapper        m_Allocator;
    /// spinLockPool for rwLocks
    RTESync_SpinlockPool           m_SpinLockPool;
    /// reader/writer lock
    RTESync_IRWRegion*             m_pRwLock;

    class LockedScope
    {
    public:
        LockedScope( RTESync_IRWRegion& rwRegion ): m_RWRegion( rwRegion )
        { m_RWRegion.enter( true );}

        ~LockedScope(){ m_RWRegion.leave( true );}

    private :
        RTESync_IRWRegion& m_RWRegion;
    };

};

#endif // CONVERTER_PAGENOITERATOR_HPP
