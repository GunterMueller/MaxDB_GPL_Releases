/*!
  @file           RTEMem_AllocatorRegister.hpp
  @author         JoergM
  @ingroup        Runtime
  @brief          Allocator registration 

    The class RTEMem_AllocatorRegister implements a singleton, that allows to register 
    allocators. This allows to collect the memory usage information for all registered
    allocators.

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
#ifndef RTEMEM_ALLOCATORREGISTER_HPP
#define RTEMEM_ALLOCATORREGISTER_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/MemoryManagement/SAPDBMem_IAllocatorInfo.hpp"
#include "RunTime/RTE_ItemRegister.hpp"

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*! 
  @class RTEMem_AllocatorRegister

  @brief The allocator register singleton
  
  The allocator register allows individual allocators to Register()/Deregister()
  themselfs. To be able to register, these allocator must be derived from
  SAPDBMem_IRawAllocator and contain Register() call in their ctor and a Deregister()
  call in their dtor. The Register() call is supplied with an variable of
  type RTEMem_AllocatorInfo which must be member variable of the allocator.
  To be used with RTEMem_AllocatorRegister, the class SAPDBMem_IRawAllocator got 
  an extra GetIdentifier() member function added. The registration takes place 
  automatically during construction, no extra call is needed.

  To obtain the register information a list of all currently registered allocator
  can be retrieved. To do this first the register must be locked via a Lock() call. 
  A loop over all registered allocators is done using the Next() function, which returns
  a pointer to an RTEMem_AllocatorInfo. If no more elements are returned, a NULL pointer
  is returned. An Unlock() call is needed to complete the register access.
  
  The Register() and Deregister() functions use the same internal Spinlock Lock() and 
  Unlock() use on enter and leave. After unlock is called any pointer retunred by a previous
  Next() call is no longer garantuid to point to an existing object!!!!

  Since the Deregister() function is called in every dtor of a class derived from
  RTEMem_IRawAllocator, the allocator instances will never be detructed while 
  registered in the RTEMem_AllocatorRegister.

  Example:

  \#include "Runtime/MemoryManagement/RTEMem_AllocatorRegister.hpp"

  void SaveAllocatorInfo()
  {
      RTEMem_AllocatorRegister & register = RTEMem_AllocatorRegister::Instance() ;

      RTEMem_AllocatorInfo     * info ;

      register.Lock();

      while ( 0 != (info = register.Next()) )
      {
          SAPDB_ULong usedCount;
          SAPDB_ULong allocCount;

          info->GetItem().CalcStatistics(usedCount, allocCount);
          InsertIntoAllocatorSystemTable( info->GetIdentifier(), 
                                          usedCount,
                                          allocCount,
                                          info->GetBaseIdentifier() );
      }

      register.Unlock();
  }

 */
class RTEMem_AllocatorRegister : public RTE_ItemRegister<SAPDBMem_IAllocatorInfo>
{
public:
    static const SAPDB_Int4 m_MaxIdentfierLength; ///< maximum identifier length

    //! the statistic info
    struct StatisticInfo
    {
        SAPDB_ULong          UsedBytes;        ///< current used size
        SAPDB_ULong          MaxUsedBytes;     ///< maximum used size
        SAPDB_ULong          AllocatedBytes;   ///< current allocated size
        SAPDB_ULong          CountAlloc;       ///< number of allocate calls
        SAPDB_ULong          CountDealloc;     ///< number of deallocate calls
        SAPDB_ULong          BaseCountAlloc;   ///< number of base allocator allocate calls
        SAPDB_ULong          BaseCountDealloc; ///< number of base allocator deallocate calls
        SAPDB_ULong          CountFailedAlloc; ///< number of failed allocate calls
        SAPDB_ULong          CountFoundError;  ///< number of fixed internal errors
        SAPDB_UTF8           AllocatorName[40+4]; ///< allocator name
        SAPDB_UTF8           AllocatorBaseName[40+4]; ///< base allocator name
    };

    /*!
        @brief Return reference to single instance of RTEMem_AllocatorRegister

        The InitializeInstance function must have been called before.

        @return Reference to singleton instance
     */
    static RTEMem_AllocatorRegister & Instance( );

    /*!
       @brief Get a snapshot of all statistic informations
       @param  pInfoArray [out] Pointer to array of StatisticInfo
       @param  itemCount [out] Number of items in statistic info array

       @return True if info array fill, false if not not enough memory (itemCount updated)
     */
    SAPDB_Bool GetStatisticInfo(StatisticInfo * &pInfoArray
                               , SAPDB_Int4 &itemCount);

#ifdef AIX
    /*!
       @brief reset the register
       needed for watchdog process after kernel stopped
     */
    static void Reset(void)
    {
        m_Instance = NULL;
    }
#endif
    /*!
       @brief Free the snapshot of all statistic informations collected by GetStatisticInfo

       @param pInfoArray [in] Pointer to array of StatisticInfo
     */
    void FreeStatisticInfo(StatisticInfo *pInfoArray);

private:
    /*! @breif private ctor */
    RTEMem_AllocatorRegister(SAPDB_Bool makeBackupCopy):RTE_ItemRegister<SAPDBMem_IAllocatorInfo>(makeBackupCopy) {}

    /*!
       @brief Fill the statistic informations
       
       Called by GetStatisticInfo, to call at least memory allocation outside of locked registration code

       @param pInfoArray [in] Pointer to array of StatisticInfo
       @param maxCount [in] maximum number of entries to copy
       @param itemCount [out] the actual item count
       @return true if fill succeeded, false if not enough space (inspect itemCount for a second try)

     */
    SAPDB_Bool FillStatisticInfo(StatisticInfo * &pInfoArray, SAPDB_Int4 maxCount, SAPDB_Int4 &itemCount);

    static RTEMem_AllocatorRegister *m_Instance; ///< the singleton instance pointer
};
//! the allocator info type
typedef RTEMem_AllocatorRegister::Info RTEMem_AllocatorInfo;
//! the allocator identifier type
typedef SAPDB_UTF8 RTEMem_AllocatorIdentifier;
/*!
  @endclass RTEMem_AllocatorRegister
 */
#endif  /* RTEMEM_ALLOCATORREGISTER_HPP */
