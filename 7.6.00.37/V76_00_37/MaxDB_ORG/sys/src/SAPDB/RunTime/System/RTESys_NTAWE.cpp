/*!---------------------------------------------------------------------
  @file           RTESys_NTAWE.cpp
  @author         RaymondR
  @brief          Windows 2000 AWE support
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
---------------------------------------------------------------------*/




/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include "RunTime/RTE_Types.h"
#include "RunTime/MemoryManagement/RTEMem_Allocator.hpp" 
#include "RunTime/RTE_Crash.hpp"
#include "RunTime/RTE_MessageList.hpp"
#include "RunTime/RTE_Message.hpp"
#include "RunTime/System/RTESys_NTLoadDLLFunc.h"
#include "RunTime/System/RTESys_NTAWE.hpp"
#include "RunTime/RTE_AWE_Messages.hpp"
#include "RunTime/RTE_MessageList.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"


extern SAPDBTrace_Topic Memory_Trace; ///< global memory trace object

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define _1MB                       (1024 * 1024)
#define NO_MORE_ENTRIES            (SAPDB_UInt4)-1
#define NO_PHYS_PAGE_MAPPED        (SAPDB_UInt4)-1
#define HASH_LIST_LEN_PER_SLOT     8

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

#define OFFSET_OFF(_strct,_elem)   ((size_t)&(((_strct *)0)->_elem))


/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

/*-----------------------------------------------------------------------
  Class:          RTESys_NTAWEPhysMem
  Description:    Handle the AWE physical pages but not the map 
                  area pages
 -----------------------------------------------------------------------*/
class RTESys_NTAWEPhysMem : public SAPDBMem_IAllocatorInfo 
{
public:
  RTESys_NTAWEPhysMem();
  ~RTESys_NTAWEPhysMem();

 /*-----------------------------------------------------------------------
   Function:       Initialize
   Description:    Initialize the RTESys_NTAWEPhysMem instance
  -----------------------------------------------------------------------*/
  SAPDB_UInt4 Initialize            ( SAPDB_UInt4 const      DBPageSize,
                                      SAPDB_UInt4 const      NumberOfMapAreaPages,
                                      SAPDB_UInt4            &NumberOfPhysPages,
                                      SAPDBErr_MessageList   &MessageList,
                                      SAPDB_UInt8 const      ReservedPhysMemory = 0 ); // Use this parameter for tests on small systems only

 /*-----------------------------------------------------------------------
  Function:       ReservePhysPage / ReleasePhysPage
  Description:    Reserve/Release a physical page (DBPageSize).
                  A reserved physical page can be accessed by mapping 
                  it into a mapping page ( refer to 'map' ). 
  -----------------------------------------------------------------------*/
  inline SAPDB_UInt4 ReservePhysPage   ( SAPDB_UInt4            &PhysPageNo,
                                         SAPDBErr_MessageList   &MessageList );
  inline SAPDB_UInt4 ReleasePhysPage   ( SAPDB_UInt4 const      PhysPageNo,
                                         SAPDBErr_MessageList   &MessageList );

/*-----------------------------------------------------------------------
  Function:       Map / UnMap
  Description:    Map/UnMap a physical page into/from a specified mapping page.
  -----------------------------------------------------------------------*/
  inline SAPDB_UInt4 Map            ( void * const           pMappingPageAddr,
                                      SAPDB_UInt4  const     PhysPageNo,
                                      SAPDBErr_MessageList   &MessageList );

  inline SAPDB_UInt4 UnMap          ( void * const           pMappingPageAddr,
                                      SAPDBErr_MessageList   &MessageList);

 /*-----------------------------------------------------------------------
  Function:       NumOfPhysicalPages
  Description:    Maximum number of physical pages (DBPageSize).
  -----------------------------------------------------------------------*/
  SAPDB_UInt4 NumOfPhysicalPages       ()  const { return m_AWEPhysPages / m_SystemPagesPerDBPage; };

 /*-----------------------------------------------------------------------
  Function:       NumOfFreePhysicalPages
  Description:    Available number of physical pages (DBPageSize).
  -----------------------------------------------------------------------*/
  SAPDB_UInt4 NumOfFreePhysicalPages   ()  const { RTESync_LockedScope LockedScope( (RTESync_Spinlock&)m_Lock );
                                                   return m_FreeAWEPhysPages / m_SystemPagesPerDBPage; };

 /*-----------------------------------------------------------------------
  Function:       NumOfMaps
  Description:    Number of Map calls.
  -----------------------------------------------------------------------*/
  SAPDB_UInt4 NumOfMaps                ()  const { RTESync_LockedScope LockedScope( (RTESync_Spinlock&)m_Lock );
                                                   return m_CountMap; };

 /*-----------------------------------------------------------------------
  Function:       NumOfUnMaps
  Description:    Number of UnMap calls.
  -----------------------------------------------------------------------*/
  SAPDB_UInt4 NumOfUnMaps              ()  const { RTESync_LockedScope LockedScope( (RTESync_Spinlock&)m_Lock );
                                                   return m_CountUnMap; };

 /*-----------------------------------------------------------------------
  Function:       GetPhysPageInfo
  Description:    Get info of a physical page.
  -----------------------------------------------------------------------*/
  bool        GetPhysPageInfo          ( SAPDB_UInt4  const     PhysPageNo,
                                         bool                   &Reserved,
                                         SAPDB_UInt8            &MapCount )  const;


 /*-----------------------------------------------------------------------
  function:     PhysPageIsReserved
  Description:  True if the physical page has been reserved
  -----------------------------------------------------------------------*/
  inline bool   PhysPageIsReserved         ( SAPDB_UInt4 const      PhysPageNo );

private:
 /*-----------------------------------------------------------------------
  function:     LoggedSetLockPagesPrivilege
  Description:  Set privelege to allocate physical AWE memory
                phyical page infos
  -----------------------------------------------------------------------*/
  SAPDB_UInt4   LoggedSetLockPagesPrivilege( bool                   SetPrivilege,
                                             SAPDBErr_MessageList   &MessageList );
 /*-----------------------------------------------------------------------
  Function:     AllocPageFrameAndInfoArea / FreePageFrameAndInfoArea
  Description:  Allocate /Deallocate  memory for page frame numbers and 
                phyical page infos
  -----------------------------------------------------------------------*/
  SAPDB_UInt4   AllocPageFrameAndInfoArea  ( DWORD       const      NumberOfAWEPhysPages,
                                             SAPDBErr_MessageList   &MessageList );
  SAPDB_UInt4   FreePageFrameAndInfoArea   ();

 /*-----------------------------------------------------------------------
  Function:     AllocPhysicalPages / FreePhysicalPages
  Description:  Allocate / Release AWE physical pages from / to system.
  -----------------------------------------------------------------------*/
  SAPDB_UInt4   AllocPhysicalPages         ( DWORD                  &NumberOfAWEPhysPages,
                                             SAPDBErr_MessageList   &MessageList);
  SAPDB_UInt4   FreePhysicalPages          ( DWORD                  NumberOfAWEPhysPages = 0 );

 /*-----------------------------------------------------------------------
  Function:     GetCallStatistics
  Description:  Outputs statistics about the number of reserve and release 
                physical page calls.
  -----------------------------------------------------------------------*/
  virtual void  GetCallStatistics          ( SAPDB_ULong            &CountReservePhysPage,
                                             SAPDB_ULong            &CountReleasePhysPage ) const;
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
                               { return (const SAPDB_UTF8 *)"RTESys_NTAWEPhysMem"; }

private:
  bool                   m_Initalized;

  RTESync_Spinlock       m_Lock;                  // used for locked scopes

  RTEMem_AllocatorInfo   m_AllocatorInfo;

  SAPDB_UInt4            m_SystemPagesPerDBPage;
  SAPDB_UInt4            m_SystemPageSize;

  SAPDB_UInt4            m_AWEPhysPages;          // System page size
  SAPDB_UInt4            m_FreeAWEPhysPages;      // System page size 

  PULONG_PTR             m_pPageFrameNumbers;     // Pointer to page frame number area

  SAPDB_ULong            m_CountReservePhysPage;  
  SAPDB_ULong            m_CountReleasePhysPage;

  SAPDB_ULong            m_CountMap;
  SAPDB_ULong            m_CountUnMap;

  struct PhysPageInfo 
  {
    SAPDB_UInt8            Reserved:1,            // If true, page has been reserved
                           MapCount;
    SAPDB_UInt4            NextPage;              // Next page info if page info is in free list
  };

  PhysPageInfo*          m_pPPInfos;              // Pointer to all physical page infos
  SAPDB_UInt4            m_FirstFreePPInfo;       // First unused physical page info
};

/*-----------------------------------------------------------------------
  Class:          RTESys_NTAWEMapAreaHashList
  Description:    Store he AWE map area page informations in a hash list.
                  The hash list is used to get the page information via
                  map page address.
 -----------------------------------------------------------------------*/

 class RTESys_NTAWEMapAreaHashList
 {
 public:
   RTESys_NTAWEMapAreaHashList() : m_NumOfMapAreaPageInfos(0),
                                   m_NumOfHashSlots(0),
                                   m_pListEntries(0),
                                   m_FirstFreeListEntries(0),
                                   m_FirstUnassignedListEntries(0),
                                   m_ppSlots(0),
                                   m_Lock(0)  {;}
                                 
   ~RTESys_NTAWEMapAreaHashList();

   typedef void * HashListHdl;

 public:
 /*-----------------------------------------------------------------------
   Function:       Initialize
   Description:    Initialize the RTESys_NTAWEMapAreaHashList instance
  -----------------------------------------------------------------------*/
   SAPDB_UInt4              Initialize            ( SAPDB_UInt4            NumOfMapAreaPageInfos,
                                                    SAPDB_UInt4            NumOfHashSlots,
                                                    SAPDBErr_MessageList   &MessageList );

 /*-----------------------------------------------------------------------
   Function:       InitialAssignAddress
   Description:    Assign a map page address to a free hash list information
                   entriy and insert it into a slot list.
  -----------------------------------------------------------------------*/
   void                     InitialAssignAddress   ( void * const          pMapAreaPageAddr );
 /*-----------------------------------------------------------------------
   Function:       Reserve / Release
   Description:    Reserve a unused / Release a map page and its hash list
                   information  entry. 
  -----------------------------------------------------------------------*/
   HashListHdl              Reserve                ();
   void                     Release                ( HashListHdl           Handle );
 /*-----------------------------------------------------------------------
   Function:       GetHandleOfAddr
   Description:    Get a hash list information entry via map page address
                   information  entry. 
  -----------------------------------------------------------------------*/
   inline HashListHdl       GetHandleOfAddr        ( void const * const    pMapAreaPageAddr );

 /*-----------------------------------------------------------------------
   Function:       IsReserved, GetAddress and GetPhysPageNo
   Description:    Get informations of a map page.
  -----------------------------------------------------------------------*/
   bool                     PhysPageMapped         ( HashListHdl           Handle );
   bool                     IsReserved             ( HashListHdl           Handle );
   inline void*             GetAddress             ( HashListHdl           Handle );
   inline SAPDB_UInt4       GetPhysPageNo          ( HashListHdl           Handle );
   inline SAPDB_UInt8       GetMapCount            ( HashListHdl           Handle );

 /*-----------------------------------------------------------------------
   Function:       AssignPhysPageNo / IncrementMapCount
   Description:    Assign / alter external informations.
  -----------------------------------------------------------------------*/
   inline void              ResetPhysPageNo        ( HashListHdl           Handle );
   inline void              AssignPhysPageNo       ( HashListHdl           Handle,
                                                     SAPDB_UInt4           PhysPageNo );
   inline void              IncrementMapCount      ( HashListHdl           Handle );

 /*-----------------------------------------------------------------------
   Function:       GetFirstAssignedPageAddr / GetNextAssignedPageAddr
   Description:    Used to step throw the map page informations. 
  -----------------------------------------------------------------------*/
   inline void*             GetFirstAssignedPageAddr( SAPDB_UInt4          &Idx )
                                                     { Idx = 0;  return GetNextAssignedPageAddr ( Idx ); }
   inline void*             GetNextAssignedPageAddr ( SAPDB_UInt4          &Idx );

 private:
   struct MapAreaPageInfo
   {
     void *                 pMapAreaPageAddr;
     SAPDB_UInt4            PhysPageNo;

     SAPDB_UInt8            Reserved:1,             // If true, page has been allocated
                            MapCount;    
   };

  struct ListEntry;  // we need access to 'MapAreaPageInfo'
  friend struct RTESys_NTAWEMapAreaHashList::ListEntry;

  struct ListEntry
  {
    MapAreaPageInfo        PageInfo;

    SAPDB_UInt4            NextEntry;
    ListEntry *            pNextEntryInHashList;

#   if defined ( LIST_ENRY_WITH_SLOT_INFO )
     SAPDB_UInt4            SlotNo;
#   endif
  };

 private:
   SAPDB_UInt4 GetSlotNo( const SAPDB_UInt4  HashValue ) const  { return HashValue % m_NumOfHashSlots; }


 private: 
   RTESync_Spinlock   m_Lock;                  // used for locked scopes

   SAPDB_UInt4        m_NumOfMapAreaPageInfos;
   SAPDB_UInt4        m_NumOfHashSlots;

   ListEntry*         m_pListEntries;
   SAPDB_UInt4        m_FirstFreeListEntries;        // First not reserved list entry
   SAPDB_UInt4        m_FirstUnassignedListEntries;  // First list entry with unassigned 
                                                     // MapAreaAddress

   ListEntry**        m_ppSlots;
 };


/*===========================================================================*
 *  STATIC/INLINE FUNCTION PROTOTYPES                                        *
 *===========================================================================*/



/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

RTESys_NTAWE::RTESys_NTAWE() : m_NumberOfFreeMapAreaPages(0),
                               m_NumberOfMapAreaPages(0),
                               m_DBPageSize(0),
                               m_Initalized(false),
                               m_pAWEPhysMem(0),
                               m_pMapAreaHashList(0),
                               m_Lock(0),
                               m_AllocatorInfo((const SAPDB_UTF8 *)"RTESys_NTAWE")
{
  SAPDBTRACE_ROUTINE_DEBUG("RTESys_NTAWE::RTESys_NTAWE", Memory_Trace, 5);
}

/*------------------------------*/

RTESys_NTAWE::~RTESys_NTAWE()
{
  SAPDBTRACE_ROUTINE_DEBUG("RTESys_NTAWEPhysMem::~RTESys_NTAWEPhysMem", Memory_Trace, 5);

  if ( m_Initalized == true )
    RTEMem_AllocatorRegister::Instance().Deregister(m_AllocatorInfo);

  m_Initalized = false;

  if ( m_pMapAreaHashList )
  {
    SAPDB_UInt4   Idx;
    void *        pMapAreaPageAddr = m_pMapAreaHashList->GetFirstAssignedPageAddr(Idx);

    // release all mapping area pages
    for ( SAPDB_UInt4 MapAreaPage = 0; 
          MapAreaPage < m_NumberOfMapAreaPages && pMapAreaPageAddr != NULL; 
          MapAreaPage++ )
    {
      VirtualFree( pMapAreaPageAddr, 0, MEM_RELEASE );
      pMapAreaPageAddr = m_pMapAreaHashList->GetNextAssignedPageAddr(Idx);
    }
  }

  if ( m_pAWEPhysMem )
  {
      m_pAWEPhysMem->~RTESys_NTAWEPhysMem();
      m_pAWEPhysMem = NULL;
  }

  if ( m_pMapAreaHashList )
  {
      m_pMapAreaHashList->~RTESys_NTAWEMapAreaHashList();
      m_pMapAreaHashList = NULL;
  }
}

/*------------------------------*/

SAPDB_UInt4  RTESys_NTAWE::Initialize ( SAPDB_UInt4 const     DBPageSize,
                                        SAPDB_UInt4 const     NumberOfMapAreaPages,
                                        SAPDB_UInt4           &NumberOfPhysPages,
                                        SAPDBErr_MessageList  &MessageList,
                                        SAPDB_UInt8 const     ReservedPhysMemory )
{
  SAPDBTRACE_ROUTINE_DEBUG("RTESys_NTAWE::Initialize", Memory_Trace, 1);

  SAPDBMEM_STATIC_RAW_ALLOCATE(RTESys_NTAWEMapAreaHashListSpace, sizeof(RTESys_NTAWEMapAreaHashList) );
  SAPDBMEM_STATIC_RAW_ALLOCATE(RTESys_NTAWEPhysMemSpace, sizeof(RTESys_NTAWEPhysMem) );

  SAPDB_UInt4 rc = NO_ERROR;
  
  // already initialized?
  if ( m_Initalized )
  {
    rc          = ERROR_ALREADY_INITIALIZED;
    MessageList = SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_SYS_AWE_ALREADY_INIT, SAPDB_ToString(rc) );
  }
  else
  {
    m_NumberOfMapAreaPages     = NumberOfMapAreaPages;
    m_NumberOfFreeMapAreaPages = NumberOfMapAreaPages;
    m_DBPageSize               = DBPageSize;

    m_pMapAreaHashList = new(RTESys_NTAWEMapAreaHashListSpace) RTESys_NTAWEMapAreaHashList;

    if ( m_pMapAreaHashList )
       m_pAWEPhysMem = new(RTESys_NTAWEPhysMemSpace) RTESys_NTAWEPhysMem;

    if (( m_pAWEPhysMem ) && ( m_pMapAreaHashList ))
    {
      rc = m_pAWEPhysMem->Initialize ( DBPageSize, NumberOfMapAreaPages, NumberOfPhysPages, 
                                       MessageList, ReservedPhysMemory );
      if ( rc == NO_ERROR )
      {
        rc = m_pMapAreaHashList->Initialize ( NumberOfMapAreaPages, 
                                              (NumberOfMapAreaPages / HASH_LIST_LEN_PER_SLOT) + 1,
                                              MessageList );
        if ( rc == NO_ERROR )
        {
          void *       pMappingPageAddr;
          SAPDB_UInt4  AllocatedMapAreaPages;

          for ( SAPDB_UInt4 MapAreaPage = 0; MapAreaPage < NumberOfMapAreaPages; MapAreaPage += AllocatedMapAreaPages )
          {
            AllocatedMapAreaPages = NumberOfMapAreaPages - MapAreaPage;
            do
            {
              // allocate mapping area page chunk
              pMappingPageAddr = VirtualAlloc( NULL, m_DBPageSize * AllocatedMapAreaPages, MEM_RESERVE | MEM_PHYSICAL, PAGE_READWRITE );

              if ( pMappingPageAddr == NULL && GetLastError() == ERROR_NOT_ENOUGH_MEMORY )
                AllocatedMapAreaPages--;
            } 
            while (( AllocatedMapAreaPages != 0 )    &&
                   ( pMappingPageAddr      == NULL ) && 
                   ( GetLastError()        == ERROR_NOT_ENOUGH_MEMORY ));

            if ( pMappingPageAddr ) 
            { 
              for ( SAPDB_UInt4 MapAreaPage = 0; MapAreaPage < AllocatedMapAreaPages; MapAreaPage++ )
              { // Assign this to a mapping area page info to an hash list info entry.
                m_pMapAreaHashList->InitialAssignAddress( (char*)pMappingPageAddr + (m_DBPageSize * MapAreaPage) );
              }
            }
            else
            {
              rc = GetLastError();
              if ( rc = ERROR_NOT_ENOUGH_MEMORY )
                MessageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_SYS_AWE_MAP_AREA_OUT_OF_MEM );
              else
                MessageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_SYS_AWE_ALLOC_MAP_AREA, SAPDB_ToString(rc) );
              break;
            }
          }
        }
      }

      if ( rc != NO_ERROR )
      {
        m_pAWEPhysMem->~RTESys_NTAWEPhysMem();
        m_pMapAreaHashList->~RTESys_NTAWEMapAreaHashList();
        m_pAWEPhysMem      = NULL;
        m_pMapAreaHashList = NULL;
      }
    }
    else
    {
      rc          = ERROR_NOT_ENOUGH_MEMORY;
      MessageList = SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_SYS_AWE_OUT_OF_HEAP_MEM, SAPDB_ToString(rc) );
    }

    if ( rc == NO_ERROR )
    {
      m_AllocatorInfo = RTEMem_AllocatorInfo( this->GetIdentifier(), this, (const SAPDB_UTF8 *)"" );
      RTEMem_AllocatorRegister::Instance().Register(m_AllocatorInfo);

      MessageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEINFO_SYS_AWE_AVAIL_INFO, 
                                          SAPDB_ToString(((SAPDB_UInt8)NumberOfPhysPages * (SAPDB_UInt8)DBPageSize) / _1MB ),
                                          SAPDB_ToString((NumberOfMapAreaPages * DBPageSize)  / _1MB ) );
      m_Initalized = true;
    }
  }

  return rc;
}

/*------------------------------*/

SAPDB_UInt4 RTESys_NTAWE::Map ( void * const           pMappingPageAddr,
                                SAPDB_UInt4  const     PhysPageNo,
                                SAPDBErr_MessageList   &MessageList )
{ 
  SAPDBTRACE_ROUTINE_DEBUG("RTESys_NTAWE::Map", Memory_Trace, 5);
  SAPDB_UInt4 rc;

  RTESys_NTAWEMapAreaHashList::HashListHdl   HashListHandle;

  if ( !m_Initalized )
  {
    MessageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_SYS_AWE_NOT_INIT, 
                                        SAPDB_ToString(ERROR_INVALID_FUNCTION) );
    return ERROR_INVALID_FUNCTION;
  }

  SAPDBERR_ASSERT_STATE(pMappingPageAddr != NULL);

  rc = m_pAWEPhysMem->Map ( pMappingPageAddr, PhysPageNo, MessageList );

  if ( rc == NO_ERROR )
  {
    HashListHandle = m_pMapAreaHashList->GetHandleOfAddr( pMappingPageAddr );

    if ( HashListHandle != NULL )
    {
      m_pMapAreaHashList->AssignPhysPageNo( HashListHandle, PhysPageNo );
      m_pMapAreaHashList->IncrementMapCount( HashListHandle );
    }
    else
    {
      rc = ERROR_INVALID_PARAMETER;
      MessageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_SYS_AWE_MAP_PHYS_PAGE, SAPDB_ToString(PhysPageNo), SAPDB_ToString(rc) );
      return rc;
    }
  }
  else if ( rc == ERROR_INVALID_PARAMETER )
  {
    SAPDB_UInt4                                Idx;
    void *                                     pTmpAddr;
    RTESys_NTAWEMapAreaHashList::HashListHdl   TmpHdl;

    // now we are checking if this page is already mapped!
    //
    pTmpAddr = m_pMapAreaHashList->GetFirstAssignedPageAddr(Idx);

    for ( SAPDB_UInt4 MapAreaPage = 0; MapAreaPage < m_NumberOfMapAreaPages && pTmpAddr; MapAreaPage++ )
    {
     TmpHdl = m_pMapAreaHashList->GetHandleOfAddr(pTmpAddr);

     if (( m_pMapAreaHashList->IsReserved(TmpHdl)    == true ) &&
         ( m_pMapAreaHashList->GetPhysPageNo(TmpHdl) == PhysPageNo ))
     {
       MessageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_SYS_AWE_PHYS_PAGE_ALR_MAPPED, 
                                           SAPDB_ToString(PhysPageNo), SAPDB_ToString(pTmpAddr), 
                                           SAPDB_ToString(pMappingPageAddr) );
       break;
     }

      pTmpAddr = m_pMapAreaHashList->GetNextAssignedPageAddr(Idx);
    }
  }
  return rc;
}

/*------------------------------*/

SAPDB_UInt4 RTESys_NTAWE::UnMap ( void * const           pMappingPageAddr,
                                  SAPDBErr_MessageList   &MessageList)
{ 
  SAPDBTRACE_ROUTINE_DEBUG("RTESys_NTAWE::UnMap", Memory_Trace, 5);
  SAPDB_UInt4 rc;

  RTESys_NTAWEMapAreaHashList::HashListHdl   HashListHandle;

  if ( !m_Initalized )
  {
    MessageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_SYS_AWE_NOT_INIT, 
                                        SAPDB_ToString(ERROR_INVALID_FUNCTION) );
    return ERROR_INVALID_FUNCTION;
  }

  SAPDBERR_ASSERT_STATE(pMappingPageAddr != NULL);

  rc = m_pAWEPhysMem->UnMap ( pMappingPageAddr, MessageList );

  if ( rc == NO_ERROR )
  {
    HashListHandle = m_pMapAreaHashList->GetHandleOfAddr( pMappingPageAddr );

    if ( HashListHandle != NULL )
      m_pMapAreaHashList->ResetPhysPageNo( HashListHandle );
    else
    {
      SAPDB_UInt4 rc = ERROR_INVALID_PARAMETER;
      MessageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_SYS_AWE_UNMAP_PHYS_PAGE, SAPDB_ToString(rc) );
      return rc;
    }
  }

  return rc;
}


/*------------------------------*/

SAPDB_UInt4 RTESys_NTAWE::ReleasePhysPage( SAPDB_UInt4 const      PhysPageNo,
                                           SAPDBErr_MessageList   &MessageList )
{ 
  SAPDBTRACE_ROUTINE_DEBUG("RTESys_NTAWE::ReleasePhysPage", Memory_Trace, 5);

  if ( !m_Initalized )
  {
    MessageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_SYS_AWE_NOT_INIT, 
                                        SAPDB_ToString(ERROR_INVALID_FUNCTION) );
    return ERROR_INVALID_FUNCTION;
  }

  return m_pAWEPhysMem->ReleasePhysPage( PhysPageNo, MessageList );
}

/*------------------------------*/

SAPDB_UInt4 RTESys_NTAWE::ReservePhysPage( SAPDB_UInt4            &PhysPageNo,
                                           SAPDBErr_MessageList   &MessageList )
{ 
  SAPDBTRACE_ROUTINE_DEBUG("RTESys_NTAWE::ReservePhysPage", Memory_Trace, 5);

  if ( !m_Initalized )
  {
    MessageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_SYS_AWE_NOT_INIT, 
                                        SAPDB_ToString(ERROR_INVALID_FUNCTION) );
    return ERROR_INVALID_FUNCTION;
  }

  return m_pAWEPhysMem->ReservePhysPage( PhysPageNo, MessageList );
}

/*------------------------------*/

SAPDB_UInt4 RTESys_NTAWE::AllocatMappingPage ( void ** const          ppMappingPageAddr,
                                               SAPDB_UInt4* const     pPhysPageNo,
                                               SAPDBErr_MessageList   &MessageList )
{
  SAPDBTRACE_ROUTINE_DEBUG("RTESys_NTAWE::AllocatMappingPage", Memory_Trace, 1);

  SAPDB_UInt4                                rc = NO_ERROR;
  RTESys_NTAWEMapAreaHashList::HashListHdl   HashListHandle;

  if ( !m_Initalized )
  {
    MessageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_SYS_AWE_NOT_INIT, 
                                        SAPDB_ToString(ERROR_INVALID_FUNCTION) );
    return ERROR_INVALID_FUNCTION;
  }

  if ( m_NumberOfFreeMapAreaPages == 0 )
  {
    MessageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_SYS_AWE_OUT_OF_RES_MAP_AREA, 
                                        SAPDB_ToString(m_NumberOfMapAreaPages) );
    return ERROR_NOT_ENOUGH_MEMORY;
  }

  // Locked scope
  { RTESync_LockedScope LockedScope( m_Lock );

    m_NumberOfFreeMapAreaPages--;
    m_CountAlloc++;
  } 

  // reserve a mapping area page and get its page info from hash list
  HashListHandle     = m_pMapAreaHashList->Reserve();
  *ppMappingPageAddr = m_pMapAreaHashList->GetAddress(HashListHandle);

  if ( pPhysPageNo != NULL )
  {
    rc = m_pAWEPhysMem->ReservePhysPage( *pPhysPageNo, MessageList );

    if ( rc == NO_ERROR )
    {
      rc = m_pAWEPhysMem->Map( *ppMappingPageAddr, *pPhysPageNo, MessageList );

      if ( rc == NO_ERROR )
      {
        m_pMapAreaHashList->AssignPhysPageNo( HashListHandle, *pPhysPageNo );
        m_pMapAreaHashList->IncrementMapCount( HashListHandle );
      }
      else
        m_pAWEPhysMem->ReleasePhysPage( *pPhysPageNo, MessageList );
    }

    if ( rc != NO_ERROR )
      m_pMapAreaHashList->Release( HashListHandle ); // release mapping area page info
  }

  return rc;
}

/*------------------------------*/

SAPDB_UInt4 RTESys_NTAWE::FreeMappingPage ( void ** const          ppMappingPageAddr,
                                            SAPDBErr_MessageList   &MessageList )
{
  SAPDBTRACE_ROUTINE_DEBUG("RTESys_NTAWE::FreeMappingPage", Memory_Trace, 1);

  SAPDB_UInt4                                 rc = NO_ERROR;
  RTESys_NTAWEMapAreaHashList::HashListHdl    HashListHandle;

  if ( !m_Initalized )
  {
    MessageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_SYS_AWE_NOT_INIT, 
                                        SAPDB_ToString(ERROR_INVALID_FUNCTION) );
    return ERROR_INVALID_FUNCTION;
  }

  SAPDBERR_ASSERT_STATE(*ppMappingPageAddr != NULL);

  HashListHandle = m_pMapAreaHashList->GetHandleOfAddr( *ppMappingPageAddr );

  if ( HashListHandle )
  {
	if ( m_pMapAreaHashList->PhysPageMapped(HashListHandle))
	{
		SAPDB_UInt4 PhysPageNo = m_pMapAreaHashList->GetPhysPageNo(HashListHandle);

		SAPDBERR_ASSERT_STATE(m_pAWEPhysMem->PhysPageIsReserved( PhysPageNo ));

		// The physical page is reserved. Unmap and release physical page
		rc = m_pAWEPhysMem->UnMap(  m_pMapAreaHashList->GetAddress(HashListHandle), MessageList );

		if ( rc == NO_ERROR )
			rc =  m_pAWEPhysMem->ReleasePhysPage( PhysPageNo, MessageList );
	}

    if ( rc == NO_ERROR )
    {
      // Locked scope
      { RTESync_LockedScope LockedScope( m_Lock );

      m_NumberOfFreeMapAreaPages++;
      m_CountDealloc++;
      } 
     *ppMappingPageAddr = NULL;

      m_pMapAreaHashList->Release( HashListHandle ); // release mapping area page info
    }
  }
  else
  {
    rc = ERROR_INVALID_PARAMETER;
    MessageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_SYS_AWE_FREE_MAP_AREA, SAPDB_ToString(rc) );
  }

  return rc;
}

/*------------------------------*/

SAPDB_UInt4 RTESys_NTAWE::NumOfFreePhysicalPages()  const 
{
  SAPDBTRACE_ROUTINE_DEBUG("RTESys_NTAWE::NumOfFreePhysicalPages", Memory_Trace, 1);

  if ( !m_Initalized )
   return 0;

  return m_pAWEPhysMem->NumOfFreePhysicalPages();
}

/*------------------------------*/

SAPDB_UInt4 RTESys_NTAWE::NumOfPhysicalPages()  const
{
  SAPDBTRACE_ROUTINE_DEBUG("RTESys_NTAWE::NumOfPhysicalPages", Memory_Trace, 1);

  if ( !m_Initalized )
   return 0;

  return m_pAWEPhysMem->NumOfPhysicalPages();
}

/*------------------------------*/

void RTESys_NTAWE::GetCallStatistics ( SAPDB_ULong &CountAlloc,
                                       SAPDB_ULong &CountDealloc ) const
{ RTESync_LockedScope LockedScope( (RTESync_Spinlock&)m_Lock );

  CountAlloc   = m_CountAlloc;
  CountDealloc = m_CountDealloc;
}

/*------------------------------*/

void RTESys_NTAWE::CalcStatistics( SAPDB_ULong &BytesUsed,
                                   SAPDB_ULong &BytesControlled )
{ RTESync_LockedScope LockedScope( m_Lock );

  BytesUsed       = (m_NumberOfMapAreaPages - m_NumberOfFreeMapAreaPages) * m_DBPageSize;
  BytesControlled = BytesUsed;
}

/*------------------------------*/

SAPDB_UInt4 RTESys_NTAWE::NumOfMaps() const
{
 return m_Initalized ?  m_pAWEPhysMem->NumOfMaps() : 0;
}

/*------------------------------*/

SAPDB_UInt4 RTESys_NTAWE::NumOfUnMaps() const
{
 return m_Initalized ?  m_pAWEPhysMem->NumOfUnMaps() : 0;
}

/*------------------------------*/

bool RTESys_NTAWE::GetPhysPageInfo( SAPDB_UInt4  const     PhysPageNo,
                                    bool                   &Reserved,
                                    SAPDB_UInt8            &MapCount )  const
{
 return m_Initalized ?  m_pAWEPhysMem->GetPhysPageInfo(PhysPageNo,Reserved,MapCount) : false;
}

/*------------------------------*/

bool RTESys_NTAWE::GetFirstMapPageInfo ( SAPDB_UInt4            &Idx,
                                         void *                 &pMappingPageAddr,
                                         SAPDB_UInt4            &PhysPageNo,
                                         bool                   &Reserved,
                                         SAPDB_UInt8            &MapCount )  const
{
  Idx = 0;
  return GetNextMapPageInfo ( Idx, pMappingPageAddr, PhysPageNo, Reserved, MapCount );
}

/*------------------------------*/

bool RTESys_NTAWE::GetNextMapPageInfo ( SAPDB_UInt4             &Idx,
                                        void *                  &pMappingPageAddr,
                                        SAPDB_UInt4             &PhysPageNo,
                                        bool                    &Reserved,
                                        SAPDB_UInt8             &MapCount )  const
{
  void *        pMapAreaPageAddr;
  bool          Ok = false;

  if ( m_Initalized == false )
    return Ok;

  if ( Idx )
    pMapAreaPageAddr = m_pMapAreaHashList->GetNextAssignedPageAddr( Idx );
  else
    pMapAreaPageAddr = m_pMapAreaHashList->GetFirstAssignedPageAddr( Idx );

  if ( pMapAreaPageAddr )
  {
    RTESys_NTAWEMapAreaHashList::HashListHdl Handle; 
    
    Handle = m_pMapAreaHashList->GetHandleOfAddr( pMapAreaPageAddr );

    pMappingPageAddr = pMapAreaPageAddr;
    Reserved         = m_pMapAreaHashList->IsReserved(Handle);
    PhysPageNo       = m_pMapAreaHashList->GetPhysPageNo(Handle);
    MapCount         = m_pMapAreaHashList->GetMapCount(Handle);
    Ok = true;
  }
  return Ok;
}

/*===========================================================================*
 *  RTESys_NTAWEPhysMem                                                      *
 *===========================================================================*/

RTESys_NTAWEPhysMem::RTESys_NTAWEPhysMem() : m_Initalized(false),
                                             m_FreeAWEPhysPages(0),
                                             m_pPPInfos(NULL),
                                             m_FirstFreePPInfo(0),
                                             m_pPageFrameNumbers(NULL),
                                             m_AWEPhysPages(0),
                                             m_SystemPagesPerDBPage(0),
                                             m_CountReservePhysPage(0),
                                             m_CountReleasePhysPage(0),
                                             m_SystemPageSize(0),
                                             m_CountMap(0),
                                             m_CountUnMap(0),
                                             m_Lock(0),
                                             m_AllocatorInfo((const SAPDB_UTF8 *)"RTESys_NTAWEPhysMem")
{
  SAPDBTRACE_ROUTINE_DEBUG("RTESys_NTAWEPhysMem::RTESys_NTAWEPhysMem", Memory_Trace, 5);

  m_AllocatorInfo = RTEMem_AllocatorInfo( this->GetIdentifier(), this, (const SAPDB_UTF8 *)"" );
  RTEMem_AllocatorRegister::Instance().Register(m_AllocatorInfo);
}

/*------------------------------*/

RTESys_NTAWEPhysMem::~RTESys_NTAWEPhysMem()
{
  SAPDBTRACE_ROUTINE_DEBUG("RTESys_NTAWEPhysMem::~RTESys_NTAWEPhysMem", Memory_Trace, 5);

  RTEMem_AllocatorRegister::Instance().Deregister(m_AllocatorInfo);

  if ( m_Initalized )
  {
    FreePhysicalPages();
    FreePageFrameAndInfoArea();
  }
}

/*------------------------------*/

SAPDB_UInt4 RTESys_NTAWEPhysMem::Initialize ( SAPDB_UInt4  const     DBPageSize,
                                              SAPDB_UInt4  const     NumberOfMapAreaPages,
                                              SAPDB_UInt4            &NumberOfPhysPages,
                                              SAPDBErr_MessageList   &MessageList,
                                              SAPDB_UInt8            ReservedPhysMemory )
{
  SAPDBTRACE_ROUTINE_DEBUG("RTESys_NTAWEPhysMem::Initialize", Memory_Trace, 1);
  SYSTEM_INFO           sSysInfo;                // useful system information
  RTE_NTMEMORYSTATUSEX  MemoryStatus;
  SAPDB_UInt4           rc              = NO_ERROR;
  DWORD                 NumberOfAWEPhysPages;
  SAPDB_UInt8           TotalUsedPhysicalMem;
  SAPDB_UInt8           UsedProcessMem;
  SAPDB_UInt8           BySysAndOtherAppsUsedMem;
  SAPDB_UInt8           AvailPhysMem;
  SAPDB_UInt8           MapAreaSize;

  if ( m_Initalized )
  {
    MessageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_SYS_AWE_ALREADY_INIT, 
                                        SAPDB_ToString(ERROR_ALREADY_INITIALIZED) );
    return ERROR_ALREADY_INITIALIZED;
  }

  // get system page size
  GetSystemInfo(&sSysInfo);
  m_SystemPageSize       = sSysInfo.dwPageSize;
  m_SystemPagesPerDBPage = DBPageSize / m_SystemPageSize;

  // get memory status
  MemoryStatus.dwLength = sizeof (MemoryStatus);
  RTESys_NTGlobalMemoryStatusEx(&MemoryStatus);

  // calculate the memory used by the system and other applications
  //
  TotalUsedPhysicalMem     = MemoryStatus.ullTotalPhys - MemoryStatus.ullAvailPhys;
  UsedProcessMem           = MemoryStatus.ullTotalVirtual - MemoryStatus.ullAvailVirtual; 
  BySysAndOtherAppsUsedMem = TotalUsedPhysicalMem - UsedProcessMem - MemoryStatus.ullAvailExtendedVirtual;

  // calculate the map area size
  //
  MapAreaSize = (SAPDB_UInt8)NumberOfMapAreaPages * DBPageSize;

  // If we are assuming that the /3GB boot.ini option was set, the system 
  // has about 768MB for paged pool, non paged pool.... (4096MB - 256MB - 3072MB = 768MB)
  // and processes needs the whole virtual memory excluding map area (data + converter cache)
  // for its own purposes. 
  // ReservedPhysMemory is the size of the reserved physical memory, which is used in 
  // applications virtual address space. Zero means, the size of the complete virtual 
  // address space ( excluding the mapping area) is reserved for for application space.
  // If the /3GB option was set the application reserves up to 3 GB of physical memory 
  // which is accessed via virtual address space.
  if ( MemoryStatus.ullTotalPhys > (4096I64 * _1MB))
    BySysAndOtherAppsUsedMem = max( BySysAndOtherAppsUsedMem, (768 * _1MB));

  if (( ReservedPhysMemory == 0 ) && ( MemoryStatus.ullTotalVirtual > MapAreaSize ))
    ReservedPhysMemory  = MemoryStatus.ullTotalVirtual - MapAreaSize; 
  else if ( ReservedPhysMemory > MapAreaSize )
    ReservedPhysMemory -= MapAreaSize;  // ReservedPhysMemory defined by caller (should be use for tests only)
  else
  {
    rc          = ERROR_NOT_ENOUGH_MEMORY;
    MessageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEINFO_SYS_AWE_WRONG_MAP_AREA_SIZE, 
                                        SAPDB_ToString(ReservedPhysMemory ? ReservedPhysMemory / _1MB : 
                                                       MemoryStatus.ullTotalVirtual / _1MB), 
                                        SAPDB_ToString(MapAreaSize / _1MB) );
    return rc;
  }

  if ( MemoryStatus.ullTotalPhys < ReservedPhysMemory + BySysAndOtherAppsUsedMem )
  {
    rc          = ERROR_NOT_ENOUGH_MEMORY;
    MessageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEINFO_SYS_AWE_OUT_OF_PHYS_MEM, 
                                        SAPDB_ToString(MemoryStatus.ullTotalPhys / _1MB), 
                                        SAPDB_ToString(BySysAndOtherAppsUsedMem /_1MB), 
                                        SAPDB_ToString((MemoryStatus.ullTotalPhys - BySysAndOtherAppsUsedMem) / _1MB ) );
    return rc;
  }

  // caclulate the unused physical memory 
  AvailPhysMem = MemoryStatus.ullTotalPhys - ReservedPhysMemory - BySysAndOtherAppsUsedMem;

  if ( NumberOfPhysPages == 0 )
    NumberOfAWEPhysPages = (SAPDB_UInt4)(AvailPhysMem / m_SystemPageSize);
  else
  {
    // we are using the smallest value physical memory available or number 
    // physical pages requested
    NumberOfAWEPhysPages = min ( (SAPDB_UInt4)(AvailPhysMem / m_SystemPageSize ),
                                 NumberOfPhysPages * m_SystemPagesPerDBPage );
  }

  // value must be a multiple of 'm_SystemPagesPerDBPage'
  NumberOfAWEPhysPages = (NumberOfAWEPhysPages / m_SystemPagesPerDBPage) * m_SystemPagesPerDBPage;

  if ( NumberOfAWEPhysPages < (MapAreaSize / m_SystemPageSize) )
  {
    rc          = ERROR_NOT_ENOUGH_MEMORY;
    MessageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEINFO_SYS_AWE_MAP_AREA_TO_BIG ); 
    return rc;
  }

  rc = LoggedSetLockPagesPrivilege( true , MessageList );

  if ( rc == NO_ERROR )
    rc = AllocPageFrameAndInfoArea( NumberOfAWEPhysPages, MessageList );

  if ( rc == NO_ERROR )
  {  
    rc = AllocPhysicalPages( NumberOfAWEPhysPages, MessageList );

    if ( rc == NO_ERROR )
    {
      // Now we know the correct phyical memory size, do a size check again!
      if ( NumberOfAWEPhysPages < (MapAreaSize / m_SystemPageSize) )
      {
        rc          = ERROR_NOT_ENOUGH_MEMORY;
        MessageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEINFO_SYS_AWE_MAP_AREA_TO_BIG ); 

        FreePhysicalPages( NumberOfAWEPhysPages );
        FreePageFrameAndInfoArea();
        NumberOfPhysPages = 0;
      }
      else
      {
        NumberOfPhysPages   = NumberOfAWEPhysPages / m_SystemPagesPerDBPage;
        m_FreeAWEPhysPages  = NumberOfAWEPhysPages;
        m_AWEPhysPages      = NumberOfAWEPhysPages;
        m_Initalized = true;
      }
    }
    else
    {
      FreePageFrameAndInfoArea();
      NumberOfPhysPages = 0;
    }
  }
  return rc;
}

/*------------------------------*/

SAPDB_UInt4 RTESys_NTAWEPhysMem::AllocPageFrameAndInfoArea( DWORD const            NumberOfAWEPhysPages,
                                                            SAPDBErr_MessageList   &MessageList )
{
  SAPDBTRACE_ROUTINE_DEBUG("RTESys_NTAWEPhysMem::AllocPageFrameAndInfoArea", Memory_Trace, 5);

  SAPDB_UInt4  rc             = NO_ERROR;
  SAPDB_UInt4  NumOfPhysPages = NumberOfAWEPhysPages / m_SystemPagesPerDBPage; // DB page size

  // --- allocate space for the physical page frame numbers
  //
  m_pPPInfos = (PhysPageInfo*)VirtualAlloc(  NULL, sizeof(PhysPageInfo) * NumOfPhysPages,
                                             MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE  );
  if ( m_pPPInfos == NULL )
    rc = GetLastError();
  else
  {
    m_pPageFrameNumbers = (PULONG_PTR)VirtualAlloc(  NULL, sizeof(PULONG_PTR) * NumberOfAWEPhysPages,
                                                     MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE  );

    if ( m_pPageFrameNumbers == NULL )
    {
      VirtualFree( m_pPPInfos, 0, MEM_RELEASE );

      m_pPPInfos = NULL;
      rc         = GetLastError();
    }
    else
    {
      memset ( m_pPPInfos, 0, sizeof(PhysPageInfo) * NumOfPhysPages );

      m_FirstFreePPInfo = 0;

      // build up free list
      for ( SAPDB_UInt4  Page = 0 ;  Page < ( NumOfPhysPages - 1 );  Page++ )
        m_pPPInfos[Page].NextPage = Page + 1;

      m_pPPInfos[NumOfPhysPages - 1].NextPage = NO_MORE_ENTRIES;
    }
  }

  if ( rc != NO_ERROR )
    MessageList = SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_SYS_AWE_ALLOC_FRAME_INFO, SAPDB_ToString(rc) );

  return rc;
}

/*------------------------------*/

SAPDB_UInt4 RTESys_NTAWEPhysMem::FreePageFrameAndInfoArea()
{
  SAPDBTRACE_ROUTINE_DEBUG("RTESys_NTAWEPhysMem::FreePageFrameAndInfoArea", Memory_Trace, 5);

  SAPDB_UInt4  rc = NO_ERROR;

  if (!VirtualFree( m_pPageFrameNumbers, 0, MEM_RELEASE ))
  {
    rc = GetLastError();
  }

  if (!VirtualFree( m_pPPInfos, 0, MEM_RELEASE ))
  {
    if ( rc == NO_ERROR )
      rc = GetLastError();
  }
  
  m_pPPInfos           = NULL;
  m_pPageFrameNumbers  = NULL;

  return rc;
}

/*------------------------------*/

SAPDB_UInt4 RTESys_NTAWEPhysMem::AllocPhysicalPages( DWORD                  &NumberOfAWEPhysPages,
                                                     SAPDBErr_MessageList   &MessageList )
{
  SAPDBTRACE_ROUTINE_DEBUG("RTESys_NTAWEPhysMem::AllocPhysicalPages", Memory_Trace, 1);

  SAPDB_UInt4  rc = NO_ERROR;

  if ( !RTESys_NTAllocateUserPhysicalPages( GetCurrentProcess(), &NumberOfAWEPhysPages, 
                                            m_pPageFrameNumbers) )
  {
    rc          = GetLastError();
    MessageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_SYS_AWE_ALLOC_PHYS_PAGES, 
                                        SAPDB_ToString(NumberOfAWEPhysPages), SAPDB_ToString(rc) );
  } 
  else
  {
    // value must be a multiple of 'm_SystemPagesPerDBPage'
    NumberOfAWEPhysPages = (NumberOfAWEPhysPages / m_SystemPagesPerDBPage) * m_SystemPagesPerDBPage;
  }

  return rc;
}

/*------------------------------*/

SAPDB_UInt4 RTESys_NTAWEPhysMem::FreePhysicalPages( DWORD                  NumberOfAWEPhysPages )
{
  SAPDBTRACE_ROUTINE_DEBUG("RTESys_NTAWEPhysMem::FreePhysicalPages", Memory_Trace, 1);

  SAPDB_UInt4  rc                    = NO_ERROR;
  DWORD        AllocatedAWEPhysPages;

  AllocatedAWEPhysPages = NumberOfAWEPhysPages == 0 ? m_AWEPhysPages : NumberOfAWEPhysPages;

  if ( !RTESys_NTFreeUserPhysicalPages( GetCurrentProcess(), &AllocatedAWEPhysPages, 
                                        m_pPageFrameNumbers ) )
  {
    rc = GetLastError();
  }
  m_AWEPhysPages = 0;

  return rc;
}

/*------------------------------*/

inline SAPDB_UInt4  RTESys_NTAWEPhysMem::ReservePhysPage ( SAPDB_UInt4            &PhysPageNo,
                                                           SAPDBErr_MessageList   &MessageList )
{
  SAPDBTRACE_ROUTINE_DEBUG("RTESys_NTAWEPhysMem::ReservePhysPage", Memory_Trace, 5);

  SAPDB_UInt4  rc            = NO_ERROR;
  bool         FreePageFound = false;

  if ( !m_Initalized )
      return ERROR_INVALID_FUNCTION;

  // Locked scope
  { RTESync_LockedScope LockedScope( m_Lock );

    if (( m_FreeAWEPhysPages ) && ( NO_MORE_ENTRIES != m_FirstFreePPInfo ))
    {
      SAPDBERR_ASSERT_STATE(m_pPPInfos[m_FirstFreePPInfo].Reserved == 0);

      m_CountReservePhysPage++;
      m_FreeAWEPhysPages          -= m_SystemPagesPerDBPage;

      PhysPageNo                      = m_FirstFreePPInfo;
      m_FirstFreePPInfo               = m_pPPInfos[PhysPageNo].NextPage;
      m_pPPInfos[PhysPageNo].Reserved = 1;
      m_pPPInfos[PhysPageNo].NextPage = NO_MORE_ENTRIES;
      FreePageFound                   = true;
    }
  } // End of locked scope

  if ( FreePageFound == false )
  {
    rc          = ERROR_NOT_ENOUGH_MEMORY;
    MessageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_SYS_AWE_ALL_PHYS_RESERVED,  
                                        SAPDB_ToString(m_AWEPhysPages  / m_SystemPagesPerDBPage ) );
  }

  return rc;
}

/*------------------------------*/

inline SAPDB_UInt4  RTESys_NTAWEPhysMem::ReleasePhysPage ( SAPDB_UInt4 const      PhysPageNo,
                                                           SAPDBErr_MessageList   &MessageList )
{
  SAPDBTRACE_ROUTINE_DEBUG("RTESys_NTAWEPhysMem::ReleasePhysPage", Memory_Trace, 5);

  SAPDB_UInt4  rc = NO_ERROR;

  if ( !m_Initalized )
  {
    MessageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_SYS_AWE_NOT_INIT, 
                                        SAPDB_ToString(ERROR_INVALID_FUNCTION) );
    return ERROR_INVALID_FUNCTION;
  }

  SAPDBERR_ASSERT_STATE(PhysPageNo < (m_AWEPhysPages / m_SystemPagesPerDBPage));
  SAPDBERR_ASSERT_STATE(m_pPPInfos[PhysPageNo].Reserved == 1);

  // Locked scope
  { RTESync_LockedScope LockedScope( m_Lock );

    m_CountReleasePhysPage++;
    m_FreeAWEPhysPages += m_SystemPagesPerDBPage;

    // add page to free list
    m_pPPInfos[PhysPageNo].Reserved = 0;
    m_pPPInfos[PhysPageNo].NextPage = m_FirstFreePPInfo;
    m_FirstFreePPInfo               = PhysPageNo;
  } 

  return rc;
}

/*------------------------------*/

inline bool  RTESys_NTAWEPhysMem::PhysPageIsReserved ( SAPDB_UInt4 const      PhysPageNo )
{
  SAPDBTRACE_ROUTINE_DEBUG("RTESys_NTAWEPhysMem::PhysPageIsReserved", Memory_Trace, 5);

  if ( !m_Initalized )
    return false;

  SAPDBERR_ASSERT_STATE(PhysPageNo < (m_AWEPhysPages / m_SystemPagesPerDBPage));
  return m_pPPInfos[PhysPageNo].Reserved;
}

/*------------------------------*/

inline SAPDB_UInt4 RTESys_NTAWEPhysMem::Map ( void * const           pMappingPageAddr,
                                              SAPDB_UInt4 const      PhysPageNo,
                                              SAPDBErr_MessageList   &MessageList )
{ 
  SAPDBTRACE_ROUTINE_DEBUG("RTESys_NTAWEPhysMem::Map", Memory_Trace, 5);

  SAPDBERR_ASSERT_STATE(PhysPageNo < (m_AWEPhysPages / m_SystemPagesPerDBPage));
  SAPDBERR_ASSERT_STATE(pMappingPageAddr != NULL);

  if ( !m_Initalized )
  {
    MessageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_SYS_AWE_NOT_INIT, 
                                        SAPDB_ToString(ERROR_INVALID_FUNCTION) );
    return ERROR_INVALID_FUNCTION;
  }

  PULONG_PTR  pPageFrameNumbers = &m_pPageFrameNumbers[PhysPageNo * m_SystemPagesPerDBPage];

  SAPDBERR_ASSERT_STATE(m_pPPInfos[PhysPageNo].Reserved == 1);

  // Locked scope
  { RTESync_LockedScope LockedScope( m_Lock );

    m_pPPInfos[PhysPageNo].MapCount++;
    m_CountMap++;
  }

  SAPDB_UInt4  rc = RTESys_NTMapUserPhysicalPages( pMappingPageAddr, m_SystemPagesPerDBPage, pPageFrameNumbers ) ? NO_ERROR : GetLastError();

  if ( rc != NO_ERROR )
    MessageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_SYS_AWE_MAP_PHYS_PAGE, SAPDB_ToString(PhysPageNo), SAPDB_ToString(rc) );

  return rc;
}

/*------------------------------*/

inline SAPDB_UInt4 RTESys_NTAWEPhysMem::UnMap ( void * const           pMappingPageAddr,
                                                SAPDBErr_MessageList   &MessageList )
{
  SAPDBTRACE_ROUTINE_DEBUG("RTESys_NTAWEPhysMem::UnMap", Memory_Trace, 5);

  if ( !m_Initalized )
  {
    MessageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_SYS_AWE_NOT_INIT, 
                                        SAPDB_ToString(ERROR_INVALID_FUNCTION) );
    return ERROR_INVALID_FUNCTION;
  }

  // Locked scope
  { RTESync_LockedScope LockedScope( m_Lock );

    m_CountUnMap++;
  }

  SAPDBERR_ASSERT_STATE(pMappingPageAddr != NULL);

  SAPDB_UInt4  rc = RTESys_NTMapUserPhysicalPages( pMappingPageAddr, m_SystemPagesPerDBPage, NULL ) ? NO_ERROR : GetLastError();

  if ( rc != NO_ERROR )
    MessageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_SYS_AWE_UNMAP_PHYS_PAGE, SAPDB_ToString(rc) );

  return rc;
}

/*------------------------------*/

SAPDB_UInt4 RTESys_NTAWEPhysMem::LoggedSetLockPagesPrivilege( bool                  SetPrivilege,
                                                              SAPDBErr_MessageList  &MessageList )
{
  SAPDBTRACE_ROUTINE_DEBUG("RTESys_NTAWEPhysMem::LoggedSetLockPagesPrivilege", Memory_Trace, 1);

  SAPDB_UInt4       rc = NO_ERROR;
  TOKEN_PRIVILEGES  tp;
  LUID              luid;
  HANDLE            hTokenUsed;
  
  // if no token specified open process token
  if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hTokenUsed))
  {
    rc = GetLastError();
    MessageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_SYS_AWE_OPT, SAPDB_ToString(rc) );
    return rc;
  }
  
  if (!LookupPrivilegeValue(NULL, SE_LOCK_MEMORY_NAME, &luid ))
  {
    rc = GetLastError();
    MessageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_SYS_AWE_LPV, SE_LOCK_MEMORY_NAME, SAPDB_ToString(rc) );
    CloseHandle(hTokenUsed);
    return rc;
  }
  
  tp.PrivilegeCount           = 1;
  tp.Privileges[0].Luid       = luid;

  if ( SetPrivilege )
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
  else
    tp.Privileges[0].Attributes = 0;
  
  if ((!AdjustTokenPrivileges(hTokenUsed, FALSE, &tp, NULL, NULL, NULL )) ||
      ( GetLastError() != ERROR_SUCCESS ))
  {
    rc = GetLastError();

    if ( rc == ERROR_NOT_ALL_ASSIGNED )
      MessageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_SYS_AWE_MISS_USER_RIGHT, "Lock pages in memory" );
    else
      MessageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_SYS_AWE_ATP, SE_LOCK_MEMORY_NAME, SAPDB_ToString(rc) );
  }

  CloseHandle(hTokenUsed);
  
  return rc;
}

/*------------------------------*/

bool RTESys_NTAWEPhysMem::GetPhysPageInfo ( SAPDB_UInt4  const     PhysPageNo,
                                            bool                   &Reserved,
                                            SAPDB_UInt8            &MapCount )  const
{
  if (PhysPageNo < (m_AWEPhysPages / m_SystemPagesPerDBPage))
  {
    RTESync_LockedScope LockedScope( (RTESync_Spinlock&)m_Lock );
    Reserved  = m_pPPInfos[PhysPageNo].Reserved;
    MapCount  = m_pPPInfos[PhysPageNo].MapCount;
    return true;
  }

return false;
}


/*------------------------------*/

void RTESys_NTAWEPhysMem::GetCallStatistics ( SAPDB_ULong &CountReservePhysPage,
                                              SAPDB_ULong &CountReleasePhysPage ) const
{
  RTESync_LockedScope LockedScope( (RTESync_Spinlock&)m_Lock );

  CountReservePhysPage = m_CountReservePhysPage;
  CountReleasePhysPage = m_CountReleasePhysPage;
}

/*------------------------------*/

void RTESys_NTAWEPhysMem::CalcStatistics( SAPDB_ULong &BytesUsed,
                                          SAPDB_ULong &BytesControlled )
{ 
  RTESync_LockedScope LockedScope( (RTESync_Spinlock&)m_Lock );

  BytesUsed       = (m_AWEPhysPages - m_FreeAWEPhysPages) * m_SystemPageSize;
  BytesControlled = BytesUsed;
}

/*===========================================================================*
 *  RTESys_NTAWEMapAreaHashList                                              *
 *===========================================================================*/

SAPDB_UInt4  RTESys_NTAWEMapAreaHashList::Initialize( SAPDB_UInt4            NumOfMapAreaPageInfos,
                                                      SAPDB_UInt4            NumOfHashSlots,
                                                      SAPDBErr_MessageList   &MessageList )
{
  SAPDBTRACE_ROUTINE_DEBUG("RTESys_NTAWEPhysMem::Initialize", Memory_Trace, 5);

  SAPDB_UInt4  rc             = NO_ERROR;

  // --- allocate space for the hash list slots
  //
  m_ppSlots = (ListEntry**)VirtualAlloc(  NULL, sizeof(ListEntry*) * NumOfHashSlots,
                                          MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE  );

  if ( m_ppSlots == NULL )
  {
    rc = GetLastError();
    MessageList = SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_SYS_AWE_ALLOC_MAP_AREA_HASH, SAPDB_ToString(rc) );
  }
  else
  {
    // --- allocate space for the ListEnties
    //
    m_pListEntries = (ListEntry*)VirtualAlloc(  NULL, sizeof(ListEntry) * NumOfMapAreaPageInfos,
                                                MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE  );

    if ( m_pListEntries == NULL )
    {
      VirtualFree( m_ppSlots, 0, MEM_RELEASE );

      m_pListEntries = NULL;
      rc             = GetLastError();
      MessageList = SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_SYS_AWE_ALLOC_MAP_AREA_INFO, SAPDB_ToString(rc) );
    }
    else
    {
      memset ( m_pListEntries, 0, sizeof(ListEntry) * NumOfMapAreaPageInfos );

      m_FirstFreeListEntries       = 0;
      m_FirstUnassignedListEntries = 0;

      // build up free list
      for ( SAPDB_UInt4  Entry = 0 ;  Entry < ( NumOfMapAreaPageInfos - 1 );  Entry++ )
        m_pListEntries[Entry].NextEntry = Entry + 1;

      m_pListEntries[NumOfMapAreaPageInfos - 1].NextEntry = (SAPDB_UInt4)-1;

      m_NumOfMapAreaPageInfos = NumOfMapAreaPageInfos;
      m_NumOfHashSlots        = NumOfHashSlots;
    }
  }

  return rc;
}

/*------------------------------*/

void RTESys_NTAWEMapAreaHashList::InitialAssignAddress( void * const           pMapAreaPageAddr )
{
  SAPDBTRACE_ROUTINE_DEBUG("RTESys_NTAWEPhysMem::InitialAssignAddress", Memory_Trace, 5);

  SAPDB_UInt4   UnassignedEntry = NO_MORE_ENTRIES;
  ListEntry**   ppSlotEntry;
  SAPDB_UInt4   SlotNo = GetSlotNo( (SAPDB_UInt4) pMapAreaPageAddr );

  SAPDBERR_ASSERT_STATE(SlotNo < m_NumOfHashSlots );

  ppSlotEntry = &m_ppSlots[SlotNo];

  { RTESync_LockedScope LockedScope( m_Lock ); // Locked scope

   // get a unassigned entry
    if ( m_FirstUnassignedListEntries != NO_MORE_ENTRIES )
    {
      UnassignedEntry                                           = m_FirstUnassignedListEntries;
      m_FirstUnassignedListEntries                              = m_pListEntries[UnassignedEntry].NextEntry;
      m_pListEntries[UnassignedEntry].PageInfo.pMapAreaPageAddr = pMapAreaPageAddr; 

      // Insert entry into a hash list slot
      if ( *ppSlotEntry != NULL )
        m_pListEntries[UnassignedEntry].pNextEntryInHashList = *ppSlotEntry;
      else
        m_pListEntries[UnassignedEntry].pNextEntryInHashList = NULL;

      *ppSlotEntry = &m_pListEntries[UnassignedEntry];

#     if defined ( LIST_ENRY_WITH_SLOT_INFO )
        m_pListEntries[UnassignedEntry].SlotNo = SlotNo;
#     endif
    }
  }

  if ( UnassignedEntry == NO_MORE_ENTRIES )
    RTE_Crash ( SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_SYS_AWE_NO_MORE_HL_ITEMS ));
}

/*------------------------------*/

RTESys_NTAWEMapAreaHashList::HashListHdl RTESys_NTAWEMapAreaHashList::Reserve()
{
  SAPDBTRACE_ROUTINE_DEBUG("RTESys_NTAWEPhysMem::Reserve", Memory_Trace, 5);

  SAPDB_UInt4   Entry = NO_MORE_ENTRIES;

  // get free entry
  { RTESync_LockedScope LockedScope( m_Lock ); // Locked scope

    if ( m_FirstFreeListEntries != NO_MORE_ENTRIES )
    {
      SAPDBERR_ASSERT_STATE(m_pListEntries[m_FirstFreeListEntries].PageInfo.Reserved == 0);

      Entry                           = m_FirstFreeListEntries;
      m_FirstFreeListEntries          = m_pListEntries[Entry].NextEntry;
      m_pListEntries[Entry].NextEntry = NO_MORE_ENTRIES;
    }
  }

  if ( Entry != NO_MORE_ENTRIES )
  {
    m_pListEntries[Entry].PageInfo.Reserved   = 1;
    m_pListEntries[Entry].PageInfo.MapCount   = 0;
    m_pListEntries[Entry].PageInfo.PhysPageNo = NO_PHYS_PAGE_MAPPED;
  }
  else
    RTE_Crash ( SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_SYS_AWE_NO_MORE_HL_ITEMS ));

  return &m_pListEntries[Entry].PageInfo;
}

/*------------------------------*/

void RTESys_NTAWEMapAreaHashList::Release( RTESys_NTAWEMapAreaHashList::HashListHdl Handle )
{
  SAPDBTRACE_ROUTINE_DEBUG("RTESys_NTAWEPhysMem::Release", Memory_Trace, 5);
  SAPDB_UInt4        Entry;
  MapAreaPageInfo*   pMapAreaPageInfo = (MapAreaPageInfo*)Handle;

  Entry = ((char*)pMapAreaPageInfo - OFFSET_OFF( ListEntry, PageInfo) - (char*)m_pListEntries ) / sizeof(ListEntry);

  SAPDBERR_ASSERT_STATE(m_pListEntries[Entry].PageInfo.Reserved == 1);

  m_pListEntries[Entry].PageInfo.Reserved   = 0;
  m_pListEntries[Entry].PageInfo.MapCount   = 0;
  m_pListEntries[Entry].PageInfo.PhysPageNo = NO_PHYS_PAGE_MAPPED;

  // add entry to free list
  { RTESync_LockedScope LockedScope( m_Lock ); // Locked scope
    
    m_pListEntries[Entry].NextEntry = m_FirstFreeListEntries;
    m_FirstFreeListEntries          = Entry;
  } 
}

/*------------------------------*/

inline void * RTESys_NTAWEMapAreaHashList::GetNextAssignedPageAddr( SAPDB_UInt4  &Idx )
{ 
  SAPDBTRACE_ROUTINE_DEBUG("RTESys_NTAWEPhysMem::GetNextAssignedPageAddr", Memory_Trace, 5);

  if ((Idx < m_NumOfMapAreaPageInfos) && 
       m_pListEntries[Idx].PageInfo.pMapAreaPageAddr != NULL )
  {
    return m_pListEntries[Idx++].PageInfo.pMapAreaPageAddr;
  }

  return NULL;
}

/*------------------------------*/

inline bool RTESys_NTAWEMapAreaHashList::PhysPageMapped( HashListHdl           Handle )
{ 
  return ((MapAreaPageInfo*)Handle)->PhysPageNo != NO_PHYS_PAGE_MAPPED;
}

/*------------------------------*/

inline void RTESys_NTAWEMapAreaHashList::ResetPhysPageNo( HashListHdl           Handle )
{ 
  ((MapAreaPageInfo*)Handle)->PhysPageNo = NO_PHYS_PAGE_MAPPED;
}

/*------------------------------*/

inline void RTESys_NTAWEMapAreaHashList::AssignPhysPageNo( HashListHdl           Handle,
                                                           SAPDB_UInt4           PhysPageNo )
{ 
  ((MapAreaPageInfo*)Handle)->PhysPageNo = PhysPageNo;
}

/*------------------------------*/

inline SAPDB_UInt4 RTESys_NTAWEMapAreaHashList::GetPhysPageNo( HashListHdl  Handle )
{ 
  return ((MapAreaPageInfo*)Handle)->PhysPageNo;
}

/*------------------------------*/

inline void RTESys_NTAWEMapAreaHashList::IncrementMapCount( HashListHdl  Handle )
{ 
  ((MapAreaPageInfo*)Handle)->MapCount++;
}

/*------------------------------*/

inline SAPDB_UInt8 RTESys_NTAWEMapAreaHashList::GetMapCount( HashListHdl   Handle )
{ 
  return ((MapAreaPageInfo*)Handle)->MapCount;
}

/*------------------------------*/

inline void* RTESys_NTAWEMapAreaHashList::GetAddress( HashListHdl   Handle )
{ 
  return ((MapAreaPageInfo*)Handle)->pMapAreaPageAddr;
}

/*------------------------------*/

inline bool RTESys_NTAWEMapAreaHashList::IsReserved( HashListHdl   Handle )
{ 
  return ((MapAreaPageInfo*)Handle)->Reserved;
}

/*------------------------------*/

inline RTESys_NTAWEMapAreaHashList::HashListHdl
                        RTESys_NTAWEMapAreaHashList::GetHandleOfAddr( void const * const  pMapAreaPageAddr )
{
  SAPDBTRACE_ROUTINE_DEBUG("RTESys_NTAWEPhysMem::GetHandleOfAddr", Memory_Trace, 5);

  ListEntry*        pSlotEntry;
  MapAreaPageInfo*  pInfo  = NULL;
  SAPDB_UInt4       SlotNo = GetSlotNo( (SAPDB_UInt4) pMapAreaPageAddr );

  SAPDBERR_ASSERT_STATE(SlotNo < m_NumOfHashSlots );

  pSlotEntry = m_ppSlots[SlotNo];

  while ( pSlotEntry )
  {
    if ( pSlotEntry->PageInfo.pMapAreaPageAddr == pMapAreaPageAddr )
    {
      pInfo = &pSlotEntry->PageInfo;
      break;
    }
    pSlotEntry = pSlotEntry->pNextEntryInHashList;
  }

  SAPDBERR_ASSERT_STATE(pInfo != NULL );

  return pInfo;
}

/*------------------------------*/

RTESys_NTAWEMapAreaHashList::~RTESys_NTAWEMapAreaHashList()
{
  SAPDBTRACE_ROUTINE_DEBUG("RTESys_NTAWEPhysMem::~RTESys_NTAWEMapAreaHashList", Memory_Trace, 5);

  if ( m_pListEntries == NULL )
    VirtualFree( m_pListEntries, 0, MEM_RELEASE );

  if ( m_ppSlots == NULL )
    VirtualFree( m_ppSlots, 0, MEM_RELEASE );

  m_pListEntries   = 0;
  m_ppSlots        = 0;
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/