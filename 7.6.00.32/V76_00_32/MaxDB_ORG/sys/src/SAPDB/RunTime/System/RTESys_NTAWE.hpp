/*!---------------------------------------------------------------------
  @file           RTESys_NTAWE.hpp
  @author         RaymondR
  @brief          Address Windowing Extensions (AWE)
  @see            


                AWE provides 32-bit applications to address more than 32-bit regions
                of physical memory.
                AWE is supported on all platforms supported by Windows 2000, including
                the 64-bit version of Windows 2000. 
                Windows 2000 (x86) provides applications with a flat 32-bit virtual 
                address space, which describes 4 GB of virtual memory. This address space 
                is usually splitted into two parts. The first part (2GB of address space) 
                is directly accessible to the application, the other 2GB memory is reserved 
                for the operating system.
                On Windows 2000 Advanced Server x86 systems an 'boot.ini' option (/3GB) 
                is provided, so that the operating system reserves only 1 GB of the address 
                space, and the application can access up to 3 GB.
                AWE solves the problem accessing more than 3GB of memory by allowing 
                applications to directly address huge amounts of physical memory while 
                continuing to use 32-bit pointers (where sufficient physical memory is 
                present). 

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
---------------------------------------------------------------------*/



#ifndef RTESYS_NTAWE_HPP
#define RTESYS_NTAWE_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#if !defined (_WIN32_WINNT)
# define _WIN32_WINNT  0x0500
#else
# if _WIN32_WINNT < 0x0500
#  error "_WIN32_WINNT must be at least 0x0500"
# endif
#endif

#include <windows.h>
#include "SAPDBCommon/SAPDB_Types.h"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp"
#include "RunTime/System/RTESys_NTLoadDLLFunc.h"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IAllocatorInfo.hpp"
#include "RunTime/MemoryManagement/RTEMem_AllocatorRegister.hpp"
#include "RunTime/Synchronisation/RTESync_Spinlock.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

class RTESys_NTAWEPhysMem;
class RTESys_NTAWEMapAreaHashList;

/*!--------------------------------------------------------------------
  @class          RTESys_NTAWE
   @description    Address Windowing Extensions


                This class allows the application to use up to 64 GB of
                physical non-paged memory in a 32-bit virtual address space on 32-bit 
                platforms, with windowed views to this physical memory from within 
                the application’s virtual address space.

 --------------------------------------------------------------------*/


class RTESys_NTAWE : public SAPDBMem_IAllocatorInfo 
{
public:
         RTESys_NTAWE();
virtual ~RTESys_NTAWE();

public:

  /*!------------------------------------------------------------------
     @description    Initialize the RTESys_NTAWE instance. 
                This method always returns a 'MessageList'. Its filled with an success 
                or an error message.
     @param          DBPageSize [in] - Database page size in bytes
     @param          NumberOfMapAreaPages [in] - Number of mapping pages to reserve, which
                                                 are used to map physical pages into them.
     @param          NumberOfPhysPages [in/out] - Number of physical pages to allocate.
     @param          ReservedPhysMemory [in] - The size of the reserved physical memory, 
                                                which is used for applications virtual 
                                                address space. Zero means, the size of
                                                the complete virtual address space 
                                                ( excluding the mapping area) is reserved for
                                                for application space. If the /3GB option is
                                                set the application reserves up to 3 GB of 
                                                physical memory which is accessed via the
                                                virtual address space

 Return value:  none

   ------------------------------------------------------------------*/

  SAPDB_UInt4 Initialize            ( SAPDB_UInt4 const     DBPageSize,
                                      SAPDB_UInt4 const     NumberOfMapAreaPages,
                                      SAPDB_UInt4           &NumberOfPhysPages,
                                      SAPDBErr_MessageList  &MessageList,
                                      SAPDB_UInt8           ReservedPhysMemory = 0);  // Use this parameter for tests on small systems only


  /*!------------------------------------------------------------------
     @description    Function returns true if AWE memory is available
     @return value   True if AWE memory is available otherwise false.

   ------------------------------------------------------------------*/

  bool        AWEAvailable          () const  { return m_Initalized; };

  /*!------------------------------------------------------------------
     @description    AllocatMappingPage one mapping page and mappes a physical page into it.
                 To map a new physical page into the mapping page call 'Map'.


                   The memory is suitably aligned for all types.
                   The memory is not initialised.  
     @param          ppMappingPageAddr [out] - Position where mapping page is allocated.
     @param          pPhysPageNo [out] - A reserved physical page number;
                                           If NULL no physical page is mapped.
     @param          MessageList [out] - Error and/or Info messages.
 Return value:   System return code

   ------------------------------------------------------------------*/

  SAPDB_UInt4 AllocatMappingPage    ( void ** const          ppMappingPageAddr,
                                      SAPDB_UInt4* const     pPhysPageNo,
                                      SAPDBErr_MessageList  &MessageList );

  /*!------------------------------------------------------------------
     @description    Frees the memory.


                  The pointer must have been obtained by a previous call to
                  'AllocatMappingPage'.
     @param          pMappingPageAddr [in] - Pointer to memory to deallocate
     @param          MessageList [out] - Error and/or Info messages.
     @return value   System return code

   ------------------------------------------------------------------*/

  SAPDB_UInt4 FreeMappingPage       ( void ** const          ppMappingPageAddr,
                                      SAPDBErr_MessageList  &MessageList );


  /*!------------------------------------------------------------------
     @description    Map a physical page into a specified mapping page.


                  The pointer must have been obtained by a previous call to
                  'allocate'. The physical page number must have been 
                  obtained by a call to 'allocate' or 'ReservePhysPage'
     @param          pMappingPageAddr [in] - Pointer to mapping page.
     @param          PhysPageNo [in] - Physical page number.
     @param          MessageList [out] - Error and/or Info messages.
     @return value   System return code

   ------------------------------------------------------------------*/

  SAPDB_UInt4 Map                   ( void * const          pMappingPageAddr,
                                      SAPDB_UInt4 const     PhysPageNo,
                                      SAPDBErr_MessageList  &MessageList );

  /*!------------------------------------------------------------------
     @description    UnMap a physical page from a specified mapping page.


                  The pointer must have been obtained by a previous call to
                  'allocate' and physical page has to be mapped.
     @param          pMappingPageAddr [in] - Pointer to mapping page.
     @param          MessageList [out] - Error and/or Info messages.
     @return value   System return code

   ------------------------------------------------------------------*/

  SAPDB_UInt4 UnMap                 ( void * const           pMappingPageAddr,
                                      SAPDBErr_MessageList   &MessageList);

  /*!------------------------------------------------------------------
     @description    Reserve a physical page.
     @param          PhysPageNo [out] - Reserved physical page. The page can be accessed
                                        by mapping it into a mapping page.
     @param          MessageList [out] - Error and/or Info messages.
     @return value   System return code

   ------------------------------------------------------------------*/

  SAPDB_UInt4 ReservePhysPage       ( SAPDB_UInt4           &PhysPageNo,
                                      SAPDBErr_MessageList  &MessageList );
  /*!------------------------------------------------------------------
     @description    Release a physical page.
     @param          PhysPageNo [in] - Physical page to release.
     @param          MessageList [out] - Error and/or Info messages.
     @return value   System return code

   ------------------------------------------------------------------*/

  SAPDB_UInt4 ReleasePhysPage       ( SAPDB_UInt4 const     PhysPageNo,
                                      SAPDBErr_MessageList  &MessageList );

  /*!------------------------------------------------------------------
     @description    Maximum number of physical pages.
     @return value   Maximum number of physical pages

   ------------------------------------------------------------------*/

  SAPDB_UInt4 NumOfPhysicalPages    ()  const;

  /*!------------------------------------------------------------------
     @description    Available number of physical pages.
     @return value   Available number of physical pages

   ------------------------------------------------------------------*/

  SAPDB_UInt4 NumOfFreePhysicalPages()  const;

    /*!----------------------------------------------------------------
       @description    Maximum number of mapping pages.
       @return value   Maximum number of mapping pages

     ----------------------------------------------------------------*/

  SAPDB_UInt4 NumOfMapAreaPages     ()  const { return m_NumberOfMapAreaPages; };


  /*!------------------------------------------------------------------
     @description    Available number of mapping pages.
     @return value   Available number of mapping pages

   ------------------------------------------------------------------*/

  SAPDB_UInt4 NumOfFreeMapAreaPages ()  const { RTESync_LockedScope LockedScope( (RTESync_Spinlock&)m_Lock );
                                                return m_NumberOfFreeMapAreaPages; };

  /*!------------------------------------------------------------------
     @description    Size of a database page.
     @return value   Size of a database page.

   ------------------------------------------------------------------*/

  SAPDB_UInt4 DBPageSize            ()  const { return m_DBPageSize; };

  /*!------------------------------------------------------------------
     @description    Number of Map calls.
     @return value   Number of Map calls.

   ------------------------------------------------------------------*/

  SAPDB_UInt4 NumOfMaps             ()  const;

  /*!------------------------------------------------------------------
     @description    Number of UnMap calls.
     @return value   Number of UnMap calls.

   ------------------------------------------------------------------*/

  SAPDB_UInt4 NumOfUnMaps           ()  const;

  /*!------------------------------------------------------------------
     @description    Get info of a physical page.
     @param          Handle [out] - (GetFirstMapPageInfo)use for iteration
                       [in/out]       - (GetNextMapPageInfo) use for iteration
                pMappingPageAddr[out] - Address of the map page
                PhysPageNo[out]       - physical page assign to the map page
                Reserved[out]         - If page has been reserved
                MapCount[out]         - Number of map in calls
     @return value   false - if no info available or wrong page number
                true  - info available

   ------------------------------------------------------------------*/

  bool        GetFirstMapPageInfo   ( SAPDB_UInt4            &Idx,
                                      void *                 &pMappingPageAddr,
                                      SAPDB_UInt4            &PhysPageNo,
                                      bool                   &Reserved,
                                      SAPDB_UInt8            &MapCount )  const;

  bool        GetNextMapPageInfo   ( SAPDB_UInt4            &Idx,
                                     void *                 &pMappingPageAddr,
                                     SAPDB_UInt4            &PhysPageNo,
                                     bool                   &Reserved,
                                     SAPDB_UInt8            &MapCount )  const;


  /*!------------------------------------------------------------------
     @description    Get info of a physical page.
     @param          PhysPageNo [in] - Physical page to release.
                Reserved[out]        - If page has been reserved
                MapCount[out]        - Number of map in calls
     @return value   false - if no info available or wrong page number
                true  - info available

   ------------------------------------------------------------------*/

  bool        GetPhysPageInfo       ( SAPDB_UInt4  const     PhysPageNo,
                                      bool                   &Reserved,
                                      SAPDB_UInt8            &MapCount )  const;

private:
 /*-----------------------------------------------------------------------
  Function:     GetCallStatistics
  Description:  Outputs statistics about the number of Allocate and Deallocate calls.
  -----------------------------------------------------------------------*/
  virtual void  GetCallStatistics          ( SAPDB_ULong            &CountAlloc,
                                             SAPDB_ULong            &CountDealloc ) const;
 /*-----------------------------------------------------------------------
  Function:     GetBaseAllocatorCallStatistics
  Description:  Outputs statistics about the number of Allocate and Deallocate calls of
                the allocator to the underlying base allocator.
  -----------------------------------------------------------------------*/
  virtual void  GetBaseAllocatorCallStatistics( SAPDB_ULong         &CountAlloc,
                                                SAPDB_ULong         &CountDealloc ) const
                                              { CountAlloc   = 0; CountDealloc = 0; };

 /*-----------------------------------------------------------------------
  Function:     CalcStatistics
  Description:  Outputs statistics about the memory usage of the allocator.
  -----------------------------------------------------------------------*/
  virtual void  CalcStatistics                ( SAPDB_ULong         &BytesUsed,
                                                SAPDB_ULong         &BytesControlled );

 /*-----------------------------------------------------------------------
  Function:     GetIdentifier
  Description:  Class identifier.
  -----------------------------------------------------------------------*/
  virtual const SAPDB_UTF8 * GetIdentifier    () const  
                               { return (const SAPDB_UTF8 *)"RTESys_NTAWE"; }

private:
  bool                          m_Initalized;

  RTESync_Spinlock              m_Lock;                      // used for locked scopes

  RTEMem_AllocatorInfo          m_AllocatorInfo;

  SAPDB_UInt4                   m_NumberOfFreeMapAreaPages;  // DB page size
  SAPDB_UInt4                   m_NumberOfMapAreaPages;      // DB page size
  SAPDB_UInt4                   m_DBPageSize;

  SAPDB_ULong                   m_CountAlloc;
  SAPDB_ULong                   m_CountDealloc;

  RTESys_NTAWEPhysMem*          m_pAWEPhysMem;
  RTESys_NTAWEMapAreaHashList*  m_pMapAreaHashList;
};



#endif  /* RTESYS_NTAWE_HPP */
