/*!**************************************************************************

  module      : RTEMem_AWEAllocator.cpp

  -------------------------------------------------------------------------

  responsible : RaymondR

  special area: Address Windowing Extensions (AWE)
  description : AWE provides 32-bit applications to address more than 32-bit regions
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


  last changed: 2001-11-28  12:27
  see also    : 

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2001-2005 SAP AG

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


*****************************************************************************/



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_Exceptions.hpp"
#include "RunTime/MemoryManagement/RTEMem_AWEAllocator.hpp"
#include "RunTime/RTE_Crash.hpp"
#include "RunTime/RTE_MessageList.hpp"
#include "RunTime/RTE_Message.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"

extern SAPDBTrace_Topic Memory_Trace;

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define SAPDB_COMPILE_WITHOUT_EXCEPTION_HANDLING

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTION PROTOTYPES                                        *
 *===========================================================================*/



/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/


RTEMem_AWEAllocator & RTEMem_AWEAllocator::Instance( )
{
  SAPDBTRACE_ROUTINE_DEBUG("RTEMem_AWEAllocator::Instance", Memory_Trace, 5);

  SAPDBMEM_STATIC_RAW_ALLOCATE(Space, sizeof(RTEMem_AWEAllocator));

  if ( m_Instance == NULL )
    m_Instance = new (Space) RTEMem_AWEAllocator();

  SAPDBERR_ASSERT_STATE(m_Instance != NULL);

  return *m_Instance;
}

RTEMem_AWEAllocator * RTEMem_AWEAllocator::m_Instance = NULL;

/*---------------------------------------------------------------------------*/
#if defined(_WIN32) && !defined(_WIN64)
/*---------------------------------------------------------------------------*/

void RTEMem_AWEAllocator::Initialize( SAPDB_UInt4 const DBPageSize,
                                      SAPDB_UInt4 const NumberOfMapAreaPages,
                                      SAPDB_UInt4       &NumberOfPhysPages,
                                      SAPDB_UInt8       ReservedMemory )

{
    SAPDBTRACE_ROUTINE_DEBUG("RTEMem_AWEAllocator::Initialize", Memory_Trace, 1);

    SAPDBErr_MessageList MessageList;

    SAPDB_UInt4   rc = Instance().AWE.Initialize( DBPageSize, NumberOfMapAreaPages,
                                                  NumberOfPhysPages, MessageList,
                                                  ReservedMemory );

    // 'AWE.Initialize' always returns a 'MessageList'. Its filled with an success 
    //  or an error message!
    RTE_Message ( MessageList );
}
/*---------------------------------------------------------------------------*/

bool RTEMem_AWEAllocator::AWEAvailable() const
{ 
  SAPDBTRACE_ROUTINE_DEBUG("RTEMem_AWEAllocator::AWEAvailable", Memory_Trace, 5);

  return AWE.AWEAvailable();
};

/*---------------------------------------------------------------------------*/

SAPDB_UInt4 RTEMem_AWEAllocator::NumOfPhysicalPages() const 
{ 
  SAPDBTRACE_ROUTINE_DEBUG("RTEMem_AWEAllocator::NumOfPhysicalPages", Memory_Trace, 1);
  return AWE.NumOfPhysicalPages(); 
};

/*---------------------------------------------------------------------------*/

SAPDB_UInt4 RTEMem_AWEAllocator::NumOfFreePhysicalPages() const 
{ 
  SAPDBTRACE_ROUTINE_DEBUG("RTEMem_AWEAllocator::NumOfFreePhysicalPages", Memory_Trace, 1);
  return AWE.NumOfFreePhysicalPages(); 
}

/*---------------------------------------------------------------------------*/

SAPDB_UInt4 RTEMem_AWEAllocator::NumOfMapAreaPages() const 
{ 
  SAPDBTRACE_ROUTINE_DEBUG("RTEMem_AWEAllocator::NumOfMapAreaPages", Memory_Trace, 1);
  return AWE.NumOfMapAreaPages(); 
}

/*---------------------------------------------------------------------------*/

SAPDB_UInt4 RTEMem_AWEAllocator::NumOfFreeMapAreaPages() const 
{ 
  SAPDBTRACE_ROUTINE_DEBUG("RTEMem_AWEAllocator::NumOfFreeMapAreaPages", Memory_Trace, 1);
  return AWE.NumOfFreeMapAreaPages(); 
}

/*---------------------------------------------------------------------------*/

SAPDB_UInt4 RTEMem_AWEAllocator::DBPageSize() const 
{ 
  SAPDBTRACE_ROUTINE_DEBUG("RTEMem_AWEAllocator::DBPageSize", Memory_Trace, 1);
  return AWE.DBPageSize(); 
}

/*---------------------------------------------------------------------------*/

SAPDB_UInt4 RTEMem_AWEAllocator::NumOfMaps() const 
{ 
  SAPDBTRACE_ROUTINE_DEBUG("RTEMem_AWEAllocator::NumOfMaps", Memory_Trace, 1);
  return AWE.NumOfMaps(); 
}

/*---------------------------------------------------------------------------*/

SAPDB_UInt4 RTEMem_AWEAllocator::NumOfUnMaps() const 
{ 
  SAPDBTRACE_ROUTINE_DEBUG("RTEMem_AWEAllocator::NumOfUnMaps", Memory_Trace, 1);
  return AWE.NumOfUnMaps(); 
};

/*---------------------------------------------------------------------------*/

bool RTEMem_AWEAllocator::GetFirstMapPageInfo( SAPDB_UInt4            &Idx,
                                               void *                 &pMappingPageAddr,
                                               SAPDB_UInt4            &PhysPageNo,
                                               bool                   &Reserved,
                                               SAPDB_UInt8            &MapCount )const
{ 
  SAPDBTRACE_ROUTINE_DEBUG("RTEMem_AWEAllocator::GetFirstMapPageInfo", Memory_Trace, 1);
  return AWE.GetFirstMapPageInfo( Idx,  pMappingPageAddr, PhysPageNo, Reserved, MapCount ); 
}

/*---------------------------------------------------------------------------*/

bool RTEMem_AWEAllocator::GetNextMapPageInfo( SAPDB_UInt4             &Idx,
                                               void *                 &pMappingPageAddr,
                                               SAPDB_UInt4            &PhysPageNo,
                                               bool                   &Reserved,
                                               SAPDB_UInt8            &MapCount )const
{ 
  SAPDBTRACE_ROUTINE_DEBUG("RTEMem_AWEAllocator::GetNextMapPageInfo", Memory_Trace, 1);
  return AWE.GetNextMapPageInfo( Idx,  pMappingPageAddr, PhysPageNo, Reserved, MapCount ); 
}

/*---------------------------------------------------------------------------*/

bool RTEMem_AWEAllocator::GetPhysPageInfo( SAPDB_UInt4  const     PhysPageNo,
                                           bool                   &Reserved,
                                           SAPDB_UInt8            &MapCount )const
{ 
  SAPDBTRACE_ROUTINE_DEBUG("RTEMem_AWEAllocator::GetPhysPageInfo", Memory_Trace, 1);
  return AWE.GetPhysPageInfo( PhysPageNo, Reserved, MapCount ); 
}

/*---------------------------------------------------------------------------*/

void* RTEMem_AWEAllocator::Allocate ( SAPDB_UInt4*           pPhysPageNo )
{
  SAPDBTRACE_ROUTINE_DEBUG("RTEMem_AWEAllocator::Allocate", Memory_Trace, 1);

  SAPDBErr_MessageList  MessageList;
  void*                 pMappingPageAddr;
  SAPDB_UInt4           rc;

  rc = AWE.AllocatMappingPage ( &pMappingPageAddr, pPhysPageNo, MessageList );

  if ( rc != NO_ERROR )
  {
    if ( rc != ERROR_NOT_ENOUGH_MEMORY )
      RTE_Message( MessageList );

    pMappingPageAddr = NULL;

#   ifndef SAPDB_COMPILE_WITHOUT_EXCEPTION_HANDLING
     throw SAPDBMem_BadAllocException(__CONTEXT__, AWE.DBPageSize() );
#   endif
  }
  return pMappingPageAddr;
}

/*---------------------------------------------------------------------------*/

void RTEMem_AWEAllocator::Deallocate ( void * pMappingPageAddr  )
{
  SAPDBTRACE_ROUTINE_DEBUG("RTEMem_AWEAllocator::Deallocate", Memory_Trace, 1);

  SAPDBErr_MessageList  MessageList;

  if ( AWE.FreeMappingPage ( &pMappingPageAddr, MessageList ) != NO_ERROR )
  {
    RTE_Message ( MessageList );
#   ifndef SAPDB_COMPILE_WITHOUT_EXCEPTION_HANDLING
     throw SAPDBMem_BadAllocException(__CONTEXT__, AWE.DBPageSize() );
#   endif
  }
  return;
};

/*---------------------------------------------------------------------------*/

void RTEMem_AWEAllocator::Map ( void * const          pMappingPageAddr,
                                SAPDB_UInt4 const     PhysPageNo )
{
  SAPDBTRACE_ROUTINE_DEBUG("RTEMem_AWEAllocator::Map", Memory_Trace, 1);
  SAPDBErr_MessageList  MessageList;
  bool                  Ok;

  Ok = AWE.Map ( pMappingPageAddr, PhysPageNo, MessageList ) == NO_ERROR;

  if ( !Ok )  
    RTE_Crash ( MessageList );

  return;
}

/*---------------------------------------------------------------------------*/

void RTEMem_AWEAllocator::UnMap ( void * const          pMappingPageAddr )
{
  SAPDBTRACE_ROUTINE_DEBUG("RTEMem_AWEAllocator::UnMap", Memory_Trace, 1);
  SAPDBErr_MessageList  MessageList;
  bool                  Ok;

  Ok = AWE.UnMap ( pMappingPageAddr, MessageList ) == NO_ERROR;

  if ( !Ok )  
    RTE_Crash ( MessageList );

  return;
}

/*---------------------------------------------------------------------------*/

void RTEMem_AWEAllocator::ReservePhysPage ( SAPDB_UInt4           &PhysPageNo )
{
  SAPDBTRACE_ROUTINE_DEBUG("RTEMem_AWEAllocator::ReservePhysPage", Memory_Trace, 1);
  SAPDBErr_MessageList  MessageList;
  bool                  Ok;

  Ok = AWE.ReservePhysPage ( PhysPageNo, MessageList )  == NO_ERROR;

  if ( !Ok )
    RTE_Crash ( MessageList );

  return;
}

/*---------------------------------------------------------------------------*/

 void RTEMem_AWEAllocator::ReleasePhysPage( SAPDB_UInt4 const     PhysPageNo )
 {
  SAPDBTRACE_ROUTINE_DEBUG("RTEMem_AWEAllocator::ReleasePhysPage", Memory_Trace, 1);
  SAPDBErr_MessageList  MessageList;
  bool                  Ok;

  Ok = AWE.ReleasePhysPage( PhysPageNo, MessageList )  == NO_ERROR;

  if ( !Ok )  
    RTE_Crash ( MessageList );

  return;
 }


/*---------------------------------------------------------------------------*/
#else /* UNIX and _WIN64 */
/*---------------------------------------------------------------------------*/

void RTEMem_AWEAllocator::Initialize( SAPDB_UInt4 const DBPageSize,
                                      SAPDB_UInt4 const  NumberOfMapAreaPages,
                                      SAPDB_UInt4        &NumberOfPhysPages,
                                      SAPDB_UInt8        ReservedMemory )

{
    SAPDBTRACE_ROUTINE_DEBUG("RTEMem_AWEAllocator::Initialize", Memory_Trace, 1);
}

/*---------------------------------------------------------------------------*/

SAPDB_UInt4 RTEMem_AWEAllocator::NumOfPhysicalPages       () const { return 0; };
SAPDB_UInt4 RTEMem_AWEAllocator::NumOfFreePhysicalPages   () const { return 0; };
SAPDB_UInt4 RTEMem_AWEAllocator::NumOfMapAreaPages        () const { return 0; };
SAPDB_UInt4 RTEMem_AWEAllocator::NumOfFreeMapAreaPages    () const { return 0; };
void        RTEMem_AWEAllocator::Map                      ( void * const          pMappingPageAddr,
                                                            SAPDB_UInt4 const     PhysPageNo )      { };
void        RTEMem_AWEAllocator::UnMap                    ( void * const          pMappingPageAddr ){ };
void        RTEMem_AWEAllocator::ReservePhysPage          ( SAPDB_UInt4           &PhysPageNo )     { };
void        RTEMem_AWEAllocator::ReleasePhysPage          ( SAPDB_UInt4 const     PhysPageNo  )     { };
void        RTEMem_AWEAllocator::Deallocate               ( void*                 pMappingPageAddr) { return; };
bool        RTEMem_AWEAllocator::AWEAvailable             () const                                  { return false; };
SAPDB_UInt4 RTEMem_AWEAllocator::DBPageSize               () const                                  { return 0; };
SAPDB_UInt4 RTEMem_AWEAllocator::NumOfMaps                () const                                  { return 0; };
SAPDB_UInt4 RTEMem_AWEAllocator::NumOfUnMaps              () const                                  { return 0; };
bool        RTEMem_AWEAllocator::GetFirstMapPageInfo      ( SAPDB_UInt4            &Idx,
                                                            void *                 &pMappingPageAddr,
                                                            SAPDB_UInt4            &PhysPageNo,
                                                            bool                   &Reserved,
                                                            SAPDB_UInt8            &MapCount )const { return false; };
bool        RTEMem_AWEAllocator::GetNextMapPageInfo       ( SAPDB_UInt4            &Idx,
                                                            void *                 &pMappingPageAddr,
                                                            SAPDB_UInt4            &PhysPageNo,
                                                            bool                   &Reserved,
                                                            SAPDB_UInt8            &MapCount )const { return false; };
bool        RTEMem_AWEAllocator::GetPhysPageInfo          ( SAPDB_UInt4  const     PhysPageNo,
                                                            bool                   &Reserved,
                                                            SAPDB_UInt8            &MapCount )const { return false; };



void*      RTEMem_AWEAllocator::Allocate                  (SAPDB_UInt4*           PhysPageNo)
                                                          { 
#                                                         ifndef SAPDB_COMPILE_WITHOUT_EXCEPTION_HANDLING
                                                           throw SAPDBMem_BadAllocException(__CONTEXT__, 0); 
#                                                         endif
                                                          return (void *)0;
                                                          };

#endif


/*===========================================================================*
 *  extern "C" call stubs                                                    *
 *===========================================================================*/

externC void  RTEMem_AWEInitialize ( SAPDB_UInt4 const     DBPageSize,
                                     SAPDB_UInt4 const     NumberOfMapAreaPages,
                                     SAPDB_UInt4*          pNumberOfPhysPages,
                                     SAPDB_UInt8           ReservedPhysMemory )
{
  RTEMem_AWEAllocator::Initialize( DBPageSize, NumberOfMapAreaPages, 
                                   *pNumberOfPhysPages, ReservedPhysMemory );
}

/*---------------------------------------------------------------------------*/

externC bool RTEMem_AWEAvailable ()
{
  return RTEMem_AWEAllocator::Instance().AWEAvailable();
}

/*---------------------------------------------------------------------------*/

externC void          RTEMem_AWEReleaseInstance        ()
{
  RTEMem_AWEAllocator::Instance().~RTEMem_AWEAllocator();
}

/*---------------------------------------------------------------------------*/

externC void* RTEMem_AWEAllocate ( SAPDB_UInt4*          pPhysPageNo )
{
  void* p;

  try
  {
    p = RTEMem_AWEAllocator::Instance().Allocate( pPhysPageNo );
  }
  catch ( SAPDBMem_BadAllocException ) {  p = NULL; }

  return p;
}

/*---------------------------------------------------------------------------*/

externC void RTEMem_AWEDeallocate ( void *  pMappingPageAddr )
{
  RTEMem_AWEAllocator::Instance().Deallocate( pMappingPageAddr );
}

/*---------------------------------------------------------------------------*/

externC void RTEMem_AWEMap ( void * const      pMappingPageAddr,
                             SAPDB_UInt4 const PhysPageNo )
{
  RTEMem_AWEAllocator::Instance().Map ( pMappingPageAddr, PhysPageNo );
}

/*---------------------------------------------------------------------------*/

externC void RTEMem_AWEUnMap ( void * const      pMappingPageAddr )
{
  RTEMem_AWEAllocator::Instance().UnMap ( pMappingPageAddr );
}

/*---------------------------------------------------------------------------*/

externC void RTEMem_AWEReservePhysPage ( SAPDB_UInt4* pPhysPageNo )
{
  RTEMem_AWEAllocator::Instance().ReservePhysPage( *pPhysPageNo );
}

/*---------------------------------------------------------------------------*/

externC void RTEMem_AWEReleasePhysPage ( SAPDB_UInt4 const PhysPageNo )
{
  RTEMem_AWEAllocator::Instance().ReleasePhysPage( PhysPageNo );
}

/*---------------------------------------------------------------------------*/

externC SAPDB_UInt4 RTEMem_AWENumOfPhysicalPages ()
{
  return RTEMem_AWEAllocator::Instance().NumOfPhysicalPages();
}

/*---------------------------------------------------------------------------*/

externC SAPDB_UInt4 RTEMem_AWENumOfFreePhysicalPages ()
{
  return RTEMem_AWEAllocator::Instance().NumOfFreePhysicalPages();
}

/*---------------------------------------------------------------------------*/

externC SAPDB_UInt4 RTEMem_AWENumOfMapAreaPages ()
{
  return RTEMem_AWEAllocator::Instance().NumOfMapAreaPages();
}

/*---------------------------------------------------------------------------*/

externC SAPDB_UInt4 RTEMem_AWENumOfFreeMapAreaPages ()
{
  return RTEMem_AWEAllocator::Instance().NumOfFreeMapAreaPages();
}

/*---------------------------------------------------------------------------*/

externC SAPDB_UInt4 RTEMem_AWENumOfMaps ()
{
  return RTEMem_AWEAllocator::Instance().NumOfMaps();
}

/*---------------------------------------------------------------------------*/

externC SAPDB_UInt4 RTEMem_AWENumOfUnMaps ()
{
  return RTEMem_AWEAllocator::Instance().NumOfUnMaps();
}

/*---------------------------------------------------------------------------*/

externC bool RTEMem_AWEGetFirstMapPageInfo ( SAPDB_UInt4            *pIdx,
                                             void * * const         ppMappingPageAddr,
                                             SAPDB_UInt4            *PhysPageNo,
                                             bool                   *pReserved,
                                             SAPDB_UInt8            *pMapCount )
{
  return RTEMem_AWEAllocator::Instance().GetFirstMapPageInfo(*pIdx, *ppMappingPageAddr, *PhysPageNo, *pReserved, *pMapCount );
}

/*---------------------------------------------------------------------------*/

externC bool RTEMem_AWEGetNextMapPageInfo ( SAPDB_UInt4            *pIdx,
                                            void * * const          ppMappingPageAddr,
                                            SAPDB_UInt4            *PhysPageNo,
                                            bool                   *pReserved,
                                            SAPDB_UInt8            *pMapCount )
{
  return RTEMem_AWEAllocator::Instance().GetNextMapPageInfo(*pIdx, *ppMappingPageAddr, *PhysPageNo, *pReserved, *pMapCount );
}

/*---------------------------------------------------------------------------*/

externC bool RTEMem_AWEGetPhysPageInfo ( SAPDB_UInt4  const     PhysPageNo,
                                         bool                   *pReserved,
                                         SAPDB_UInt8            *pMapCount )
{
  return RTEMem_AWEAllocator::Instance().GetPhysPageInfo(PhysPageNo, *pReserved, *pMapCount );
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/