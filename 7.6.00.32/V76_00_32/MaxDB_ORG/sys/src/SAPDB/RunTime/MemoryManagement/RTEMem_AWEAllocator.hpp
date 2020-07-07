/*!
  @file           RTEMem_AWEAllocator.hpp
  @author         RaymondR
  @ingroup        RunTime
  @brief          Address Windowing Extensions (AWE)

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
*/
#ifndef RTEMEM_AWEALLOCATOR_HPP
#define RTEMEM_AWEALLOCATOR_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

# if defined (_WIN32) && !defined(_WIN64)
#  include "RunTime/System/RTESys_NTAWE.hpp"  /* nocheck */
# endif
# include "SAPDBCommon/SAPDB_Singleton.hpp"
# include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!
  @class RTEMem_AWEAllocator
  @brief Address Windowing Extensions

                This class allows the application to handle up to 64 GB of
                physical non-paged memory in a 32-bit virtual address space on 32-bit 
                platforms, with windowed views to this physical memory from within 
                the application’s virtual address space.
*/
class RTEMem_AWEAllocator
{
public:
   /*!
     @brief Initialize the RTEMem_AWEAllocator instance

     @param DBPageSize               [in] - Database page size in bytes
     @param  NumberOfMapAreaPages     [in] - Number of mapping pages to reserve, which
                                                    are used to map physical pages into them.
     @param  NumberOfPhysPages    [in/out] - Number of physical pages to allocate.
     @param  ReservedPhysMemory       [in] - The size of the reserved physical memory, 
                                                    which is used for applications virtual 
                                                    address space. Zero means, the size of
                                                    the complete virtual address space 
                                                    ( excluding the mapping area) is reserved for
                                                    for application space. If the /3GB option is
                                                    set the application reserves up to 3 GB of 
                                                    physical memory which is accessed via the
                                                    virtual address space

     @return none
   */
   static void Initialize( SAPDB_UInt4 const  DBPageSize,
                           SAPDB_UInt4 const  NumberOfMapAreaPages,
                           SAPDB_UInt4        &NumberOfPhysPages,
                           SAPDB_UInt8        ReservedPhysMemory = 0 );  // Use this parameter for tests on small systems only

  /*!
    @brief Return reference to single instance of RTEMem_AWEAllocator

                    The Initialize() function must have been called before.
                    This is necessary since the database page size is only 
                    known by the caller of the Initialize() function.
                    If an error occurs a 'SAPDBErr_MessageList' is written to to
                    diagnostic file.

    @return Reference to RTEMem_AWEAllocator instance
   */
   static RTEMem_AWEAllocator & Instance( );

   /*!
    @brief  Function returns true if AWE memory is available

    @return True if AWE memory is available otherwise false.
   */
   bool      AWEAvailable() const;

   /*! 
    @brief  Allocates one mapping page and mappes a physical page into it.

    To map a new physical page into the mapping page call 'Map'.

    The memory is suitably aligned for all types.

    The memory is not initialised.  

    If compiled with -DSAPDB_COMPILE_WITHOUT_EXCEPTION_HANDLING a (void *)0 
    is returned on failure.

    If compiled without this switch an SAPDBMem_BadAllocException exception
    is thrown on failure.

    If an error occurs a 'SAPDBErr_MessageList' is written to to
    diagnostic file.

    @param pPhysPageNo [out] - A reserved physical page number;
                                        If NULL no physical page is mapped.
    @return Position where mapping page is allocated
   */
   void* Allocate( SAPDB_UInt4*           pPhysPageNo = NULL );

  /*!
    @brief  Frees the memory. 

    The pointer must have been obtained by a previous call to
    'allocate'.

    If an error occurs a 'SAPDBErr_MessageList' is written to to
    diagnostic file.

    @param pMappingPageAddr [in] - Pointer to memory to deallocate
    @return none
  */
  void Deallocate( void * pMappingPageAddr );

  /*!
    @brief  Map a physical page into a specified mapping page.
                   
                    The pointer must have been obtained by a previous call to
                    'allocate'. The physical page number must have been 
                    obtained by a call to 'allocate' or 'ReservePhysPage'

                    If an error occurs a 'SAPDBErr_MessageList' is written to to
                    diagnostic file and the database system stops.

    @param pMappingPageAddr [in] - Pointer to mapping page.
    @param PhysPageNo [in]       - Physical page number.
    @return none
  */
  void        Map ( void * const          pMappingPageAddr,
                    SAPDB_UInt4 const     PhysPageNo );

  /*!
    @brief UnMap a physical page from a specified mapping page.
                   
    The pointer must have been obtained by a previous call to
    'allocate' and physical page has to be mapped.

    If an error occurs a 'SAPDBErr_MessageList' is written to to
    diagnostic file and the database system stops.

    @param pMappingPageAddr [in] - Pointer to mapping page.
    @return none
  */
  void        UnMap( void * const           pMappingPageAddr );

  /*!
    @brief Reserve a physical page.
                   
    @param PhysPageNo [out]      - Reserved physical page. The page can be accessed
                                            by mapping it into a mapping page. 

                    If an error occurs a 'SAPDBErr_MessageList' is written to to
                    diagnostic file and the database system stops.
    @return none
  */
  void        ReservePhysPage ( SAPDB_UInt4           &PhysPageNo );

  /*!
    @brief Release a physical page.
                   
                    If an error occurs a 'SAPDBErr_MessageList' is written to to
                    diagnostic file and the database system stops.

    @param PhysPageNo [in] - Physical page to release.
    @return none
  */
  void        ReleasePhysPage ( SAPDB_UInt4 const     PhysPageNo );

  /*!
    @brief Maximum number of physical pages.

    @return Maximum number of physical pages
  */
  SAPDB_UInt4 NumOfPhysicalPages () const;

  /*!
    @brief Available number of physical pages.

    @return Available number of physical pages
  */
  SAPDB_UInt4 NumOfFreePhysicalPages () const;

  /*!
    @brief Maximum number of mapping pages.

    @return Maximum number of mapping pages
  */
  SAPDB_UInt4 NumOfMapAreaPages () const;

  /*!
    @brief Available number of mapping pages.

    @return Available number of mapping pages
  */
  SAPDB_UInt4 NumOfFreeMapAreaPages () const;

  /*!
    @brief Size of a database page.

    @return Size of a database page.
  */
  SAPDB_UInt4 DBPageSize            ()  const;

  /*!
    @brief Number of Map calls.

    @return Number of Map calls.
  */
  SAPDB_UInt4 NumOfMaps             ()  const;

  /*!
    @brief Number of UnMap calls.

    @return Number of UnMap calls.
  */
  SAPDB_UInt4 NumOfUnMaps           ()  const;

  /*!
    @brief Get first info of a physical page.

    @param Handle [out]          - use for iteration
    @param pMappingPageAddr[out] - Address of the map page
    @param PhysPageNo[out]       - physical page assign to the map page
    @param Reserved[out]         - If page has been reserved
    @param MapCount[out]         - Number of map in calls
    @return false - if no info available or wrong page number
                    true  - info available
  */
  bool        GetFirstMapPageInfo  ( SAPDB_UInt4            &Idx,
                                     void *                 &pMappingPageAddr,
                                     SAPDB_UInt4            &PhysPageNo,
                                     bool                   &Reserved,
                                     SAPDB_UInt8            &MapCount )  const;

  /*!
    @brief Get next info of a physical page.

    @param Handle [inout]       -  use for iteration
    @param pMappingPageAddr[out] - Address of the map page
    @param PhysPageNo[out]       - physical page assign to the map page
    @param Reserved[out]         - If page has been reserved
    @param MapCount[out]         - Number of map in calls
    @return false - if no info available or wrong page number
                    true  - info available
  */
  bool        GetNextMapPageInfo   ( SAPDB_UInt4            &Idx,
                                     void *                 &pMappingPageAddr,
                                     SAPDB_UInt4            &PhysPageNo,
                                     bool                   &Reserved,
                                     SAPDB_UInt8            &MapCount )  const;

  /*!
    @brief Get info of a physical page.

    @param PhysPageNo [in]      - Physical page to release.
    @param Reserved[out]        - If page has been reserved
    @param MapCount[out]        - Number of map in calls
    @return false - if no info available or wrong page number
                    true  - info available
  */
  bool        GetPhysPageInfo       ( SAPDB_UInt4  const     PhysPageNo,
                                      bool                   &Reserved,
                                      SAPDB_UInt8            &MapCount )  const; 

  /*!
    @brief dtor
   */
  ~RTEMem_AWEAllocator () { m_Instance = NULL; }
private:

    SAPDB_Singleton m_ThisIsASingleton; // prevents copy and assignment

    static RTEMem_AWEAllocator *m_Instance;
  // Using a static reference would mean, that initialization function is called before main
  // This we have to prevent for the same reason we prevent static class instances
  // -> dynamic linked RunTime would not get initialized correctly on some OS...

#if defined (_WIN32) && !defined(_WIN64)
    RTESys_NTAWE       AWE;
#endif
};

/* EndClass: RTEMem_AWEAllocator */

/*===========================================================================*
 *  EXTERN "C" PROTOTYPES                                                    *
 *===========================================================================*/

#if !defined(externC)
# define externC extern "C"
#endif  

/*
  C functions used to access AWE functionality in pascal or c modules. 

  For function descritpions refer to class RTEMem_AWEAllocator.

*/
   /*! 
    @brief  Initialize the RTEMem_AWEAllocator instance

    @see RTEMem_AWEAllocator::Initialize
   */
externC void          RTEMem_AWEInitialize             ( SAPDB_UInt4 const     DBPageSize,
                                                         SAPDB_UInt4 const     NumberOfMapAreaPages,
                                                         SAPDB_UInt4*          pNumberOfPhysPages,
                                                         SAPDB_UInt8           ReservedPhysMemory );  
   /*! 
    @brief  Allocates one mapping page and mappes a physical page into it.

    @see RTEMem_AWEAllocator::AWEAvailable
   */
externC bool          RTEMem_AWEAvailable              ();
   /*! 
    @brief  Allocates one mapping page and mappes a physical page into it.

    @see RTEMem_AWEAllocator::Allocate
   */
externC void*         RTEMem_AWEAllocate               ( SAPDB_UInt4*          pPhysPageNo );
  /*!
    @brief  Frees the memory

    @see RTEMem_AWEAllocator::Deallocate
  */
externC void          RTEMem_AWEDeallocate             ( void *                pMappingPageAddr );
  /*!
    @brief  Map a physical page into a specified mapping page.

    @see RTEMem_AWEAllocator::Map
  */
externC void          RTEMem_AWEMap                    ( void * const          pMappingPageAddr,
                                                         SAPDB_UInt4 const     PhysPageNo );
  /*!
    @brief UnMap a physical page from a specified mapping page.

    @see RTEMem_AWEAllocator::UnMap
  */
externC void          RTEMem_AWEUnMap                  ( void * const          pMappingPageAddr );
  /*!
    @brief Reserve a physical page.

    @see RTEMem_AWEAllocator::ReservePhysPage
  */
externC void          RTEMem_AWEReservePhysPage        ( SAPDB_UInt4*          pPhysPageNo );
  /*!
    @brief Release a physical page.

    @see RTEMem_AWEAllocator::ReleasePhysPage
  */
externC void          RTEMem_AWEReleasePhysPage        ( SAPDB_UInt4 const     PhysPageNo );
  /*!
    @brief Maximum number of physical pages.

    @return Maximum number of physical pages
  */
externC SAPDB_UInt4   RTEMem_AWENumOfPhysicalPages     ();

  /*!
    @brief Available number of physical pages.

    @return Available number of physical pages
  */
externC SAPDB_UInt4   RTEMem_AWENumOfFreePhysicalPages ();

  /*!
    @brief Maximum number of mapping pages.

    @return Maximum number of mapping pages
  */
externC SAPDB_UInt4   RTEMem_AWENumOfMapAreaPages      ();

  /*!
    @brief Available number of mapping pages.

    @return Available number of mapping pages
  */
externC SAPDB_UInt4   RTEMem_AWENumOfFreeMapAreaPages  ();

  /*!
    @brief Number of Map calls.

    @return Number of Map calls.
  */
externC SAPDB_UInt4   RTEMem_AWENumOfMaps              ();

  /*!
    @brief Number of UnMap calls.

    @return Number of UnMap calls.
  */
externC SAPDB_UInt4   RTEMem_AWENumOfUnMaps            ();

  /*!
    @brief Get first info of a physical page.

    @param Handle [out]          - use for iteration
    @param pMappingPageAddr[out] - Address of the map page
    @param PhysPageNo[out]       - physical page assign to the map page
    @param Reserved[out]         - If page has been reserved
    @param MapCount[out]         - Number of map in calls
    @return false - if no info available or wrong page number
                    true  - info available
  */
externC bool          RTEMem_AWEGetFirstMapPageInfo    ( SAPDB_UInt4            *pIdx,
                                                         void * * const          ppMappingPageAddr,
                                                         SAPDB_UInt4            *PhysPageNo,
                                                         bool                   *pReserved,
                                                         SAPDB_UInt8            *pMapCount );
  /*!
    @brief Get next info of a physical page.

    @param Handle [inout]       -  use for iteration
    @param pMappingPageAddr[out] - Address of the map page
    @param PhysPageNo[out]       - physical page assign to the map page
    @param Reserved[out]         - If page has been reserved
    @param MapCount[out]         - Number of map in calls
    @return false - if no info available or wrong page number
                    true  - info available
  */
externC bool          RTEMem_AWEGetNextMapPageInfo     ( SAPDB_UInt4            *pIdx,
                                                         void * * const          ppMappingPageAddr,
                                                         SAPDB_UInt4            *PhysPageNo,
                                                         bool                   *pReserved,
                                                         SAPDB_UInt8            *pMapCount );
  /*!
    @brief Get info of a physical page.

    @param PhysPageNo [in]      - Physical page to release.
    @param Reserved[out]        - If page has been reserved
    @param MapCount[out]        - Number of map in calls
    @return false - if no info available or wrong page number
                    true  - info available
  */
externC bool          RTEMem_AWEGetPhysPageInfo        ( SAPDB_UInt4  const     PhysPageNo,
                                                         bool                   *pReserved,
                                                         SAPDB_UInt8            *pMapCount );
/*!
   @brief Release instance
   @return none
 */
externC void          RTEMem_AWEReleaseInstance        ();

#endif  /* RTEMEM_AWEALLOCATOR_HPP */
