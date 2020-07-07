/*!
  @file           RTESync_SpinlockPool.hpp
  @author         RobinW
  @ingroup        Runtime
  @brief          Spinlock pool

  A pool of spinlocks for striping purposes 

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
#ifndef RTESYNC_SPINLOCKPOOL_HPP
#define RTESYNC_SPINLOCKPOOL_HPP

#include "SAPDB/SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDB/RunTime/RTE_Types.hpp"
#include "SAPDB/RunTime/RTE_ItemRegister.hpp"
#include "SAPDB/RunTime/MemoryManagement/RTEMem_RteAllocator.hpp"

#include "RunTime/Synchronisation/RTESync_RWRegionRegister.hpp"

class RTESync_IRWRegion;

class SAPDBMem_IRawAllocator;
class RTESync_NamedSpinlock;

/*!
   @class RTESync_SpinlockPool
   @brief Spinlock pool used to reduce collisions

   This special spinlock pool constructor uses the specified allocator only and is therefore 
   sharable across process borders, if the allocator returns memory which is sharable.

   The spinlock pooling allows to lock a large amount of objects without the need to have an 
   spinlock for each object.
 */
class RTESync_SpinlockPool
{
public:
    /*!
      @brief Basic spinlock pool constructor

      Allocate memory using given allocator. The RTEMem_RteAllocator::Instance()
      is a good choice for kernel runtime, but a GlobalSharedMemory based allocator
      will allow to use a spinlock pool even in separate processes! To do that, the
      pool itself should have been allocated using that allocator, so that the pool
      reference is also sharable between processes.

      @param poolIdentifier [in] spinlock pool identifier
      @param numberOfElements [in] fixed number of elements in pool
      @param allocator [in] allocator to use
      @return none
     */
    RTESync_SpinlockPool(const SAPDB_UTF8 *poolIdentifier,
                         SAPDB_Int4 numberOfElements,
                         SAPDBMem_IRawAllocator &allocator = RTEMem_RteAllocator::Instance());

    /*!
      @brief destruct spinlock pool

      Deallocates the pool memory. Be carefull to do that, because you are not protected
      against any dangling references to spinlocks in the pool...
     */
    ~RTESync_SpinlockPool();

    /*!
      @brief retrieve internal lock index from given index
      The returned lock index allows a faster access to the spinlock in the lock pool

      @param id [in] any index value
      @return [SAPDB_Int4] the internal lock index
     */
    SAPDB_Int4 GetLockIndex(SAPDB_Int8 id)
    {
        return (SAPDB_Int4)((SAPDB_UInt8)id % m_numberOfElements);
    }


    /*!
      @brief retrieve internal lock index from given pointer
      The returned lock index allows a faster access to the spinlock in the lock pool

      @param ptr [in] any pointer value
      @return [SAPDB_Int4] the internal lock index
     */
    SAPDB_Int4 GetLockIndex(void *memoryAddress)
    {
        // shifting is due to most likely aligned integer or pointer references used...
        SAPDB_UInt8 ui8 = (SAPDB_Byte *)memoryAddress - (SAPDB_Byte *) 0;
        return (SAPDB_Int4)((ui8 >> 3) % m_numberOfElements);
    }


    /*!
      @brief lock via internal index

      @param lockIndex [in] internal lock pool index
      @return [SAPDB_Bool] true if lock failed, false if locked
     */
    void Lock(SAPDB_Int4 poolIndex)
    {
        SAPDBERR_ASSERT_STATE( poolIndex >= 0 && poolIndex < m_numberOfElements );
        m_pool[poolIndex].Lock();
    }


    /*!
      @brief tries to lock via internal index

      @param lockIndex [in] internal lock pool index
      @return [SAPDB_Bool] true if lock failed, false if locked
     */
    SAPDB_Bool TryLock(SAPDB_Int4 poolIndex)
    {
        SAPDBERR_ASSERT_STATE( poolIndex >= 0 && poolIndex < m_numberOfElements );
        return m_pool[poolIndex].TryLock();
    }


    /*!
      @brief unlock via internal index

      @param lockIndex [in] internal lock pool index 
      @return none
     */
    void Unlock(SAPDB_Int4 poolIndex)
    {
        SAPDBERR_ASSERT_STATE( poolIndex >= 0 && poolIndex < m_numberOfElements );
        m_pool[poolIndex].Unlock();
    }


    /*!
      @brief get access to pool name

      @return spinlock pool name
     */
    SAPDB_UTF8 *Name(void)
    {
        return m_Name;
    };

    /*!
      @brief register RWRegion
      @param itemInfo [in] item info to register
      @return true if success, false if failed
     */
    SAPDB_Bool Register(RTE_ItemRegister<RTESync_IRWRegion>::Info &itemInfo);

    /*!
      @brief deregister RWRegion
      @param itemInfo [in] item info to deregister
      @return true if success, false if failed
     */
    SAPDB_Bool Deregister(RTE_ItemRegister<RTESync_IRWRegion>::Info &itemInfo);

    /*!
      @brief reset singleton pointer to 0
      Used by watchdog process to allow pool usage without overwriting
      runtime memory dump content
     */
    void Reset(void);


    /*!
      @brief return the RWRegionRegister of the spinlock pool
	*/
	RTESync_RWRegionRegister *GetRWRegionRegister(void)
	{
		return &m_RWRegionRegister;
	}

private:
    SAPDB_Int4              m_numberOfElements; ///< fixed number of elements in pool
    RTESync_NamedSpinlock * m_pool;             ///< pool spinlock array
    SAPDBMem_IRawAllocator &m_allocator;        ///< allocator used to allocate pool
    RTESync_RWRegionRegister m_RWRegionRegister;
protected:
    SAPDB_UTF8              m_Name[40+1];       ///< The copied spinlock name
friend class RTESync_SpinlockPoolRegister;
protected:
    RTE_ItemRegister<RTESync_SpinlockPool>::Info    m_info;
};

#endif
