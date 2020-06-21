/*!
  @file           RTE_ISystem.hpp
  @author         JoergM
  @ingroup        Runtime
  @brief          System Interface

  Interface class to hide system dependencys from RTE_Global class

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
#ifndef RTE_ISYSTEM_HPP
#define RTE_ISYSTEM_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "RunTime/RTE_Types.hpp"
#include "RunTime/System/RTESys_MemProtect.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

class Msg_List;

/*!
    @class  RTE_ISystem
    @brief  Interface class to access to system dependend information

     This pure virtual class abstracts from system information.
     The implementation must derive from this class.
     Caller must not depend on actual size of RTE_ISystem pointer.
 */
class RTE_ISystem 
{
public:
  /*!---------------------------------------------------------------------------
    Public types used by public member functions.
  */
  typedef struct { SAPDB_Int2   Year;         ///< Year
                   SAPDB_Int2   Month;        ///< Month of year
                   SAPDB_Int2   DayOfWeek;    ///< Day of the week 
                   SAPDB_Int2   Day;          ///< Day of the month
                   SAPDB_Int2   Hour;         ///< Hour of day
                   SAPDB_Int2   Minute;       ///< Minute of the hour
                   SAPDB_Int2   Second;       ///< Second of the minute
                   SAPDB_Int2   Milliseconds; ///< Millisecond of the second
  } DateTime;

  ///! Constant for unlimited memory
  static const SAPDB_ULong       UnlimitedMemory;

  ///! Constant for no error detected
  static const RTE_SystemRc NoError;

public:
  /*!
    @brief  Return the system instance

    Since there is only one system, implementation of this is done in the system
    specific implementation module.
    @return The instance reference
   */
  static RTE_ISystem & Instance();

  /*!
    @brief  Give up the time slice

    In multithreaded environment this allows the scheduler to select the next thread in the
    current process.

    In singlethreaded environment this allows the system to schedule the next process.

    It makes no difference weather this is called within a task context (in term of internal tasking)
    or within a 'normal' context.
   */
  static void   GiveUpTimeSlice();

  /*!
    @brief Return the size of a memory page in bytes
    
    Size of memory pages handled in the operating system depends on architecture.
    The value maybe not fixed, so this call returns the system configuration value.

    @return The system memory page size in bytes
   */
  virtual const SAPDB_ULong GetSystemPageSize() const =0;

  /*!
    @brief Round up to multiple size of a memory page in bytes
    
    @return The size round up to system memory page size in bytes
   */
  virtual const SAPDB_ULong RoundUpToSystemPageSize(SAPDB_ULong numberOfBytes) const = 0;

  /*!
    @brief  Return size of phsical memory in bytes
    @return Physical memory size in bytes
   */
  virtual const SAPDB_ULong GetPhysicalMemorySize() const =0;

  /*!
    @brief Protect the given memory range

    @param pvAdr [in] Start address (must be system memory page aligned...)
    @param ulSize [in] size of memory area to protect (must be multiple of system memory page)
    @param type [in] MEMORYPROTECTION_READWRITE, MEMORYPROTECTION_READONLY, MEMORYPROTECTION_NOACCESS
    @return RTE_ISystem::NoError if call succeeded, else system error code directly returned
   */
  virtual RTE_SystemRc MemProtect(void *pvAdr,SAPDB_ULong ulSize,SAPDB_Int type) =0;

  /*!
    @brief Return the maximum number of CPUs available on the system
    
    This call only returns the maximum number of CPUs available on the machine. If an external tool assign
    specific CPUs to specific processes or assign a group of CPUs to the program, it will not be detected.
    The call is mainly for deciding if Spinlocks should do some hot loops before giving up time slice.

    @return Maximum number of CPUs avaliable in the machine
   */
  virtual const SAPDB_UInt4 GetNumberOfCPU() const =0;

  /*!
    @brief Allocate to given number of bytes as SystemPages
    
    This function allocates the given number of bytes as system pages .
    Size of a system page is returned by GetSystemPageSize().
    The rest of a page is returned to the caller too...
    
    @param SizeInBytes [ in ] Number of consecutive bytes to allocate
    @param DoCommit [ in ] Flag to indicate commit wanted
    @return Start address of system pages or NULL if no more pages
   */
  virtual void * AllocSystemPages(SAPDB_ULong SizeInBytes, bool DoCommit=true) =0;

  /*!
    @brief Allocate to given number of bytes as SystemPages
    
    This function allocates the given number of bytes as system pages .
    Size of a system page is returned by GetSystemPageSize().
    The rest of a page is returned to the caller too...
    
    @param SizeInBytes [ in ] Number of consecutive bytes to allocate
    @param DoCommit [ in ] Flag to indicate commit wanted
    @param messageList [out] message list
    @return Start address of system pages or NULL if no more pages
   */
  virtual void * AllocSystemPages(SAPDB_ULong SizeInBytes, bool DoCommit, Msg_List &messageList) =0;

  /*!
    @brief Allocate to given number of bytes as SystemPages

    This function allocates the given number of bytes as system pages .
    Size of a system page is returned by GetSystemPageSize().
    The rest of a page is returned to the caller too...

    @param FixedAddress [ in ] Address where to allocate the system pages or NULL if any address
    @param SizeInBytes [ in ] Number of consecutive bytes to allocate
    @param DoCommit [ in ] Flag to indicate commit wanted
    @return Start address of system pages or NULL if no more pages
   */
  virtual void * UncachedAllocSystemPagesAtFixedAddress( void *FixedAddress,
                                                         SAPDB_ULong SizeInBytes,
                                                         bool DoCommit=true) =0;

  /*!
    @brief Allocate to given number of bytes as SystemPages

    This function allocates the given number of bytes as system pages .
    Size of a system page is returned by GetSystemPageSize().
    The rest of a page is returned to the caller too...

    @param FixedAddress [ in ] Address where to allocate the system pages or NULL if any address
    @param SizeInBytes [ in ] Number of consecutive bytes to allocate
    @param DoCommit [ in ] Flag to indicate commit wanted
    @param messageList [out] message list
    @return Start address of system pages or NULL if no more pages
   */
  virtual void * UncachedAllocSystemPagesAtFixedAddress( void *FixedAddress,
                                                         SAPDB_ULong SizeInBytes,
                                                         bool DoCommit,
                                                         Msg_List &messageList) =0;

  /*!
    @brief Reserve the virtual address space for the given number of bytes as SystemPages
    
    This function looks up space for the given number of bytes as system pages .
    Size of a system page is returned by GetSystemPageSize().
    The rest of a page is reserved for the caller too, but no physical pages are allocated, not even uncommitted.
    This function works only since the address space on the 64Bit maschines allows huge amount 
    The address can be used later in a call to AllocSystemPagesAtFixedAddress()

    @param SizeInBytes [ in ] Number of consecutive bytes to allocate
    @return Start address of the first reserved system page or NULL if no more pages
   */
  virtual void * ReserveSystemPages(SAPDB_ULong SizeInBytes) =0;

  /*!
    @brief Allocate to given number of bytes as SystemPages at given address
    
    This function allocates the given number of bytes as system pages.
    Size of a system page is returned by GetSystemPageSize().
    The rest of a page is returned to the caller too...
    If the FixedAddress is already occupied, the function fails and returns NULL.
    
    @param FixedAddress [ in ] Address where to allocate the system pages or NULL if any address
    @param SizeInBytes [ in ] Number of consecutive bytes to allocate
    @param DoCommit [ in ] Flag to indicate commit wanted
    @return Fixed address of system pages or NULL if no more pages or address already used
   */
  virtual void * AllocSystemPagesAtFixedAddress(void *FixedAddress, SAPDB_ULong SizeInBytes, bool DoCommit=true) =0;

  /*!
    @brief Allocate to given number of bytes as SystemPages at given address
    
    This function allocates the given number of bytes as system pages.
    Size of a system page is returned by GetSystemPageSize().
    The rest of a page is returned to the caller too...
    If the FixedAddress is already occupied, the function fails and returns NULL.
    
    @param FixedAddress [ in ] Address where to allocate the system pages or NULL if any address
    @param SizeInBytes [ in ] Number of consecutive bytes to allocate
    @param DoCommit [ in ] Flag to indicate commit wanted
    @param messageList [out] message list
    @return Fixed address of system pages or NULL if no more pages or address already used
   */
  virtual void * AllocSystemPagesAtFixedAddress(void *FixedAddress, SAPDB_ULong SizeInBytes, bool DoCommit, Msg_List &messageList) =0;

  /*!
    @brief Commit the given number of bytes as SystemPages
    
    This function commits the given number of bytes as system pages at the given start address.
    The start address is returned by a previous call to AllocSystemPages() with DoCommit
    flag set to false. If call fails NULL is returned.

    @param FirstPageAddress [ in ] Start address of block in pages
    @param NumberOfBytes [ in ] Number of consecutive bytes to commit
    @return FirstPageAddress or NULL if call failed.
   */
  virtual void * CommitSystemPages(void *FirstPageAddress, SAPDB_ULong NumberOfBytes) =0;

  /*!
    @brief Free the given system pages
    
    @param FirstPageAddress [ in ] Start address of block in pages
    @param NumberOfBytes [ in ] Number of consecutive bytes to free
              
    @return true if call succeeded, false else.
   */
  virtual bool   FreeSystemPages(void *FirstPageAddress, SAPDB_ULong NumberOfBytes) =0;

  /*!
    @brief Uncache free the given system pages
    
    @param FirstPageAddress [ in ] Start address of block in pages
    @param NumberOfBytes [ in ] Number of consecutive bytes to free
              
    @return true if call succeeded, false else.
   */
  virtual bool   UncachedFreeSystemPages(void *FirstPageAddress, SAPDB_ULong NumberOfBytes) =0;

  /*!
    @brief Set the total limit of Memory
    
    If memory limit cannot be set during initialization, it is possible to delay setting
    the limit until the really wanted / configured limit is known.

    If memory limit is already exceeded, the current used memory is used as new limit,
    so next allocation would fail!!!!

    @param NewMemoryLimit [in] New memory limit to set
    @param OldMemoryLimit [out] Former memory limit found
  
    @return Now used memory limit (can differ from wanted limit, see above)
   */
  virtual SAPDB_ULong SetMemoryLimitInBytes(SAPDB_ULong NewMemoryLimit, SAPDB_ULong & OldMemoryLimit) =0;

  /*!
    @brief Get the current limit of MemoryUsage
    
    If MemoryLimit cannot be set during initialization, it is possible to delay setting
    the limit until the really wanted / configured limit is known.

    @param MemoryLimitInBytes    [out] actual MemoryLimit (or RTE_ISystem::UnlimitedMemory)
    @param MemoryCommitedInBytes [out] actual amount of memory commited
    @param MemoryUsedInBytes     [out] actual amount of memory used
    @param MemoryMaxUsedInBytes  [out] maximum amount of memory used
    @param MemorySuccessfullAllocs   [out] actual number of successfull allocation requests
    @param MemoryFailedAllocs        [out] actual number of failed allocation requests
    @param MemoryCommits             [out] actual number of commit requests
    @param MemoryFrees               [out] actual number of deallocation requests
   */
  virtual void GetMemoryStatistics( SAPDB_ULong & MemoryLimitInBytes,
                                    SAPDB_ULong & MemoryCommitedInBytes,
                                    SAPDB_ULong & MemoryUsedInBytes,
                                    SAPDB_ULong & MemoryMaxUsedInBytes,
                                    SAPDB_UInt8 & MemorySuccessfullAllocs,
                                    SAPDB_UInt8 & MemoryFailedAllocs,
                                    SAPDB_UInt8 & MemoryCommits,
                                    SAPDB_UInt8 & MemoryFrees) =0;

  /*!
    @brief Get the local defined node name
    
    This name is independend from database configuration file OFFICAL_NODENAME parameter. It is the 
    lokal node name which is returned if this parameter is not found.
    This node name is used in a cluster configuration to contact the node instead of the official 
    node name, if an inter database server call is issued (i.e. from master to standby client)
    The sqlhostname() returns this name too.
    @return pointer to local defined node name
   */
  virtual SAPDB_Char const * GetLocalNodeName() = 0;

  /*!
    @brief Initialization of Atomic lock area

    @param lock [ in ] The lock space
   */
  static void InitLock( RTE_Lock &lock );

  /*!
    @brief Atomic test and lock

    @param lock [ in ] The lock space
    @return true if the lock space now locked by caller, false if not
   */
  static bool TestAndLock( RTE_Lock &lock );

  /*!
    @brief Unlock Atomic lock area

    @param lock [ in ] The lock space
   */
  static void Unlock( RTE_Lock &lock );


  /*!
    @brief GetLocalDateTime returns the local time

    @param LocalDateTime [ out ] - local date and time
   */
  static void GetLocalDateTime( RTE_ISystem::DateTime  &LocalDateTime );

  /*!
    @brief Sleep the specified number of milliseconds
    
    This actually calls the system function, that reschedules the current thread.
    
    @param NumberOfMilliseconds [ in ] - Number of milliseconds to sleep
   */
  static void DoSleep(SAPDB_UInt4 NumberOfMilliseconds);

private:
  /*!
    @brief Initialize the system instance

    Since there is only one system, implementation of this is done in the system
    specific implementation module. Parameters given are the user imposed limits
    on the system. These parameters are known by the RTE_IGlobal

    @param MemoryLimit [in] Limitation to dynamic memory usage
   */
  static void Initialize(SAPDB_ULong MemoryLimitInBytes=UnlimitedMemory);

  friend class RTE_IGlobal;
}; 
/*! EndClass: RTE_ISystem */

#endif  /* RTE_ISYSTEM_HPP */
