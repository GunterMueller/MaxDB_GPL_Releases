/*!========================================================================

  @file         RTE_HandleManager.cpp
  @ingroup      Runtime
  @author       RaymondR

  @brief        Handle Manager
  
                In most situations where an application software layer needs 
                to deal with resources like connections, files etc. an opaque
                handle is used to identify a single resource.
                Using the Handle Manager is efficient way to join a opaque handle
                and the resource specific parameters.

  @since        2003-10-06  11:16
  @sa           

  ==========================================================================

  \if EMIT_LICENCE


    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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
 ============================================================================*/



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include    "SAPDBCommon/SAPDB_Types.hpp"
#include    "RunTime/MemoryManagement/RTEMem_RteAllocator.hpp"
#include    "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include    "RunTime/Synchronisation/RTESync_Spinlock.hpp"
#include    "RunTime/RTE_HandleManager.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

/*!
  @brief        In most situations where an application software layer needs 
                to deal with resources like connections, files etc. an opaque
                handle is used to identify a single resource. The handle is a 
                true magic cookie without any meaning to anyone except for the
                resource layer that are allowed on it (such as file manager).
                Inside the resource layer, a handle is the selector of a data
                buffer or object that contains resource specific parameters. To 
                avoid crashes caused by a corrupt handle, a handle should not 
                be a pointer to direct access this data buffer. However, the 
                use of hash lists to locate the resource data buffer of a 
                given handle leads to perfomance penalties. Using handle tables
                might be more efficient.
                Finally, there should not be a hardcoded limit on the number of 
                handles like a fixed handle array.

                The following "Handle Manager" implementation deals with these 
                constrains.

                Handles to the resource specific parameter blocks are not 
                unique; that is, upon destruction of a resource, the 
                corresponding handle may be recycled and might look exactly 
                like the handle to the destroyed resource. Therefore, if a 
                handle has been recycled it is possible to access the new 
                resource by an old handle value. To mitigate the risk of 
                invalid handle accesses, each handle value contains a range
                of bits ( plausibility counter) which is incremented each time
                the handle is allocated.
    
                The complete structure of a single handle value looks 
                like this:

                    |32              16|15     8|7      0|
                    +------------------+--------+--------+
                    |    Handle        |HdlTDir |HdlTab  |
                    |       PlausCnt   |   Index|   Index|
                    +------------------+--------+--------+
                                            |       |
                                            |       |
                        +-------------------+       |
                        |                           |
                        |  Handle Table Directory   |
                        |     RTE_HandleManager     |
                        | +-----------------------+ |
                        | | Hdl Tab Dir Entry 1   | |        Handle Table
                        | +-----------------------+ |      RTE_HandleTable 
                        +>| Hdl Tab Dir Entry 2   |--->+---------------------+
                          +-----------------------+ |  | Hdl Table Entry 1   |
                          :                       : |  | RTE_HandleTableEntry|
                          +-----------------------+ |  +---------------------+
                          | Hdl Tab Dir Entry 256 | |  :                     :
                          +-----------------------+ |  +---------------------+
                                                    +->| Hdl Table Entry 255 |->resource 
                                                       +---------------------+  data block
                                                       | Hdl Table Entry 256 |
                                                       +---------------------+
      
*/


/*! 
  @class        RTE_HandleTableEntry
  @brief        'RTE_HandleTableEntry' is the class holding the control information of a 
                single handle value.
  
                It contains information about handle usage, handle lock status, type of 
                handle and pending handle free requests.

                The major information is the pointer to the data buffer containing
                resource specific parameters of the surrounding software using the 
                handle manger.

                The first action should be, marking the handle as used. This is been 
                done by calling the methode 'SetStateUsed'. 'SetStateUsed' updates 
                the plausibility counter which is part of the handle value. It's 
                used to check if a given handle value is valid.

                The methode 'SetStateFree' try to set the handle info free. But, if the 
                reference count is not equal to zero which means the handle data is in
                use, the state free pending will be set and 'SetStateFree' returns 
                false. The  free state will be set if the reference count is set to
                zero ( Refer to 'ReleaseRef'). If 'SetStateFree' returns true, the free
                state has been set and the handle information block can be reused 

                'AddRef' is used to signal that the handle data pointer is currently in
                access. It updates a counter representing how often the handle data is 
                in access. 'ReleaseRef' signals that the data pointer access has been 
                finished. If the 'SetStateFree' methode has been called during the
                handle data is in access, 'ReleaseRef' sets the handle state to
                free and 'ReleaseRef' returns  true. 

                To build up a linked free of 'RTE_HandleTableEntry' elements, the methods 
                'SetNext' can be used. To retrieve the next pointer use 'next'.

                !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                ATTENTION: Keep in mind that the most methods insides this class
                           are used in a locked scope of 'RTE_HandleTable'
                !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                */ 

class  RTE_HandleTableEntry
{
public:
    /*
    !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    ATTENTION: Keep in mind that the most methods insides this class
               are used in a locked scope of 'RTE_HandleTable'
    !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    */
    RTE_HandleTableEntry() : m_PlausibilityCnt(1),
                             m_RefCnt(0),
                             m_HandleType(0),
                             m_pHandleData(0),
                             m_HandleState(Free),
                             m_Next(0) {;}

    /* The first action should be, marking the handle as used. This is been done 
       by calling the methode 'SetStateUsed'. 'SetStateUsed' updates the  plausibility
       counter which is part of the handle value. It's used to check if a given 
       handle value is valid. 
       The methode 'SetStateFree' try to set the handle info free. But, if the 
       reference count is not equal to zero which means the handle data is in use,
       the state free pending will be set and 'SetStateFree' returns false. The 
       free state will be set if the reference count is set to zero ( Refer to
       'ReleaseRef'). If 'SetStateFree' returns true the free state has been set
       and the handle information block can be reused 
       */

    void SetStateUsed()                   
        { 
            SAPDBERR_ASSERT_STATE( HandleStateValid() );
            m_HandleState = Used;
        }


    bool SetStateFree()                   
        { 
            SAPDBERR_ASSERT_STATE( HandleStateValid() );

            if ( m_RefCnt )
                m_HandleState = FreePending;
            else
            {
                MarkEntryAsFree();
                return true;
            }
            return false; 
        }

    bool IsUsed() const                   
        { 
            SAPDBERR_ASSERT_STATE( HandleStateValid() );
            return m_HandleState == Used;
        }

    bool FreeIsPending() const                   
        { 
            SAPDBERR_ASSERT_STATE( HandleStateValid() );
            return m_HandleState == FreePending;
        }

    /* 'AddRef' is used to signal that the handle data pointer is currently in access.
       It updates a counter representing how often the handle data is in access.
       'ReleaseRef' signals that the data pointer access has been finished. If the
       'SetStateFree' methode has been called during the handle data is in access,
       'ReleaseRef'  sets the handle state to free and 'ReleaseRef' returns 
       true. */
    SAPDB_UInt4  AddRef()             
       {
           return ++m_RefCnt; 
       }

    bool ReleaseRef()         
        { 
            SAPDBERR_ASSERT_STATE( HandleStateValid() );
            SAPDBERR_ASSERT_STATE( 0 != m_RefCnt); // oops...

            if (( 0 == --m_RefCnt ) && 
                ( FreePending == m_HandleState ))
            {
                MarkEntryAsFree();
                return true;
            }

            return false; 
        }

    /* Referenced returns true if handle info is referenced */
    bool Referenced()
        { 
            SAPDBERR_ASSERT_STATE( HandleStateValid() );
            return 0 != m_RefCnt;
        }

public:
    /*
    !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    ATTENTION: Keep in mind that the most methods insides this class
               are used in a locked scope of 'RTE_HandleTable'
    !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    */
    /* Set or get a pointer to the data buffer that contains resource 
       specific parameters of the surrounding software using the handle manger. */
    void SetHandleData( void *                               pHandleData, 
                        RTE_HandleManager::HdlTypeInfo const handleType )

       { 
           SetHandleData(pHandleData);
           SetHandleType(handleType);
       }

    void SetHandleData( void * const pHandleData )                    
        { 
            m_pHandleData = pHandleData; 
        }

    void* const GetHandleData() const 
        { 
            SAPDBERR_ASSERT_STATE( HandleStateValid() );
            return m_pHandleData;
        }


    /* Set or get a specific handle type, describing the handle kind ( e.g. communication, file ) */ 
    void SetHandleType( RTE_HandleManager::HdlTypeInfo const handleType ) 
        { 
            m_HandleType = handleType; 
        }

    RTE_HandleManager::HdlTypeInfo const  GetHandleType() const
        { 
            SAPDBERR_ASSERT_STATE( HandleStateValid() );
            return m_HandleType;
        }

    /* To build up a linked free or used list of 'RTE_HandleTableEntry' 
       elements, the methods 'SetNext' can be used. To retrieve
       the next pointer use 'next'.
    */ 
    RTE_HandleTableEntry*   Next() const
        { 
            SAPDBERR_ASSERT_STATE( HandleStateValid() );
            return m_Next; 
        }

    void  SetNext( RTE_HandleTableEntry* pNext ) 
        { 
            SAPDBERR_ASSERT_STATE( HandleStateValid() );
            m_Next = pNext; 
        }

    /* Returns the plausibility value */
    SAPDB_UInt2           PlausiblityCnt() const           { return  m_PlausibilityCnt; }

private:
    /*
    !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    ATTENTION: Keep in mind that the most methods insides this class
               are used in a locked scope of 'RTE_HandleTable'
    !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    */
    typedef enum SAPDB_UInt1 { Free = 0, Used = 0x1, FreePending = 0x2 } HdlState;

    // Used by ASSERTION-Checks
    bool HandleStateValid() const
        {
            return m_HandleState <= FreePending;
        }

    void MarkEntryAsFree()
        {
			if ( m_PlausibilityCnt != (SAPDB_UInt2)-1 )
				++m_PlausibilityCnt;
			else
				m_PlausibilityCnt = 1;

            m_pHandleData = 0;
            m_HandleType  = 0;
            m_HandleState = Free;
        }

private:
    HdlState                        m_HandleState;     // state like connected, free pending ...
    RTE_HandleManager::HdlTypeInfo  m_HandleType;      // type of handle like file, connection etc.
    SAPDB_UInt2                     m_PlausibilityCnt; // Used to check if a given handle value is plausible.
    SAPDB_UInt4                     m_RefCnt;          // number of references
    void*                           m_pHandleData;     // pointer to resource parameter block
    RTE_HandleTableEntry*           m_Next;            // used by free list chain
};




/*----------------------------- RTE_HandleTable ------------------------------*/

SAPDB_UInt2 const RTE_HandleTableEntries = ((unsigned char)-1) + 1;

/*! 
  @class        RTE_HandleTable
  @brief        'RTE_HandleTable' is the container for currently 256 handle entries
                 each representing a single handle value. 

                The corresponding list entry of a given handle is found by 
                cutting of the last byte of the handle value and use them as
                index value.

                  Example:  Handle:0x7F224566 -> TableIndex:0x66

  */
class RTE_HandleTable
  {
  public:
       /*! Union of opaque access and interior handle type */
       typedef union { SAPDB_UInt4                             Opaque;
           /*! Interior handle type */
                      struct { SAPDB_UInt1 TableIndex;
                               SAPDB_UInt1 DirIndex;
                               SAPDB_UInt2 PlausibilityCnt; } Interior;
                             } IntHdl;
  public:
      /*! 
        @brief ctor 
       */
      RTE_HandleTable();

      inline RTE_HandleManager::HdlRet
                   NewHandle         ( RTE_HandleTable::IntHdl              &interiorHandle,
                                       void *  const                        pHandleData,
                                       RTE_HandleManager::HdlTypeInfo const handleType );

      inline RTE_HandleManager::HdlRet
                   LeaseHandleData   ( RTE_HandleTable::IntHdl const        interiorHandle,
                                       void **  const                       ppHandleData );

      inline RTE_HandleManager::HdlRet
                   ReleaseHandleData ( RTE_HandleTable::IntHdl const        interiorHandle );

      inline RTE_HandleManager::HdlRet      
                   FindHandle        ( RTE_HandleTable::IntHdl              &interiorHandle,
                                       bool                                 findFirstHandle,
                                       RTE_HandleManager::HdlTypeInfo const handleType,
                                       bool const                           pendingFrees );

      inline RTE_HandleManager::HdlRet
                   FreeHandle        ( RTE_HandleTable::IntHdl const        interiorHandle,
                                       void **  const                       ppHandleData );

      SAPDB_UInt4  CountHandles      ( RTE_HandleManager::HdlTypeInfo const handleType = 0 ) const;


       bool        EntriesFree       () const { return NULL != m_pFirstInFreeList; }

  private:
      void                   AddHandleEntryToFreeList   ( RTE_HandleTableEntry*  pHandleEntry );
      RTE_HandleTableEntry*  GetHandleEntryFromFreeList ();

  private:
      RTESync_Spinlock           m_Lock;                                      // used for locked scopes
      RTE_HandleTableEntry       m_HandleListEntries[RTE_HandleTableEntries]; 

      RTE_HandleTableEntry*      m_pFirstInFreeList;                          // Pointer to the first handle info object which is unused.
      RTE_HandleTableEntry*      m_pLastInFreeList;                           // Pointer to the last handle info object which is unused.
  };



/*===========================================================================*
 *  STATIC/INLINE FUNCTION PROTOTYPES                                        *
 *===========================================================================*/



/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/


/*----------------------------- RTE_HandleTable ------------------------------*/

RTE_HandleTable::RTE_HandleTable() 
                : m_Lock(0),
                  m_pFirstInFreeList(0),
                  m_pLastInFreeList(0)
{
    // create free-list chain
    for ( SAPDB_UInt4 i = 0; i < RTE_HandleTableEntries; i++ )
        AddHandleEntryToFreeList( &m_HandleListEntries[static_cast<SAPDB_UInt1>(i)]);
}

/*---------------------------------------------------------------------------*/

void RTE_HandleTable::AddHandleEntryToFreeList( RTE_HandleTableEntry*  pHandleEntry )
{
    if ( NULL == m_pFirstInFreeList )
    {
        m_pFirstInFreeList = pHandleEntry;
    }
    else
    {   // add handle entry to the already filled free list
        m_pLastInFreeList->SetNext(pHandleEntry);
    }
    m_pLastInFreeList = pHandleEntry;
    pHandleEntry->SetNext(NULL);
}

/*---------------------------------------------------------------------------*/

RTE_HandleTableEntry* RTE_HandleTable::GetHandleEntryFromFreeList()
{
    RTE_HandleTableEntry*     pHandleEntry = NULL;

    if ( NULL != m_pFirstInFreeList )
    {   // LOCK SCOPE; all 'RTE_HandleTableEntry' methods are 
        //             designed to be used in a locked scope!!
        RTESync_LockedScope Lock( m_Lock );

        if ( NULL != m_pFirstInFreeList )
        {
            pHandleEntry       = m_pFirstInFreeList;
            m_pFirstInFreeList = pHandleEntry->Next();
            pHandleEntry->SetNext(0);

            if ( NULL == m_pFirstInFreeList )
                m_pLastInFreeList = NULL;
        }
    }
    return pHandleEntry;
}

/*---------------------------------------------------------------------------*/

inline RTE_HandleManager::HdlRet
    RTE_HandleTable::NewHandle( RTE_HandleTable::IntHdl              &interiorHandle,
                                void *  const                        pHandleData,
                                RTE_HandleManager::HdlTypeInfo const handleType )
{
    RTE_HandleManager::HdlRet     retVal;
    RTE_HandleTableEntry*         pHandleEntry = GetHandleEntryFromFreeList();

    if ( NULL != pHandleEntry )
    {
        pHandleEntry->SetStateUsed();
        pHandleEntry->SetHandleData( pHandleData, handleType );
        interiorHandle.Interior.PlausibilityCnt  = pHandleEntry->PlausiblityCnt();
        interiorHandle.Interior.TableIndex       = static_cast<SAPDB_UInt1>((pHandleEntry - m_HandleListEntries));

        retVal = RTE_HandleManager::NoError;
    }
    else
    {
        retVal = RTE_HandleManager::AllHandlesInUse;
    }

    return retVal;
}

/*---------------------------------------------------------------------------*/

inline RTE_HandleManager::HdlRet
    RTE_HandleTable::FreeHandle( RTE_HandleTable::IntHdl const  interiorHandle,
                                  void **  const                ppHandleData )
{
    RTE_HandleManager::HdlRet  retVal = RTE_HandleManager::NoError;

    RTE_HandleTableEntry*  pHandleEntry = &m_HandleListEntries[interiorHandle.Interior.TableIndex];

    {   // LOCK SCOPE; all 'RTE_HandleTableEntry' methods are 
        //             designed to be used in a locked scope!!
        RTESync_LockedScope Lock( m_Lock ); 

        // is the handle value plausible?
        if (( pHandleEntry->PlausiblityCnt() == interiorHandle.Interior.PlausibilityCnt ) &&
              pHandleEntry->IsUsed() )
        {
            if (ppHandleData) *ppHandleData = pHandleEntry->GetHandleData();

            if ( false == pHandleEntry->SetStateFree() ) 
            {   // only true if we have released the handle!
                retVal = RTE_HandleManager::LeasePending;
            }
        }
        else
            retVal = RTE_HandleManager::InvalidHandle;

        if ( RTE_HandleManager::NoError == retVal )
            AddHandleEntryToFreeList( pHandleEntry );
    }


    return retVal;
}

/*---------------------------------------------------------------------------*/

inline RTE_HandleManager::HdlRet
    RTE_HandleTable::LeaseHandleData( RTE_HandleTable::IntHdl const  interiorHandle,
                                      void **  const                 ppHandleData )
 {
    RTE_HandleManager::HdlRet  retVal;
    RTE_HandleTableEntry*      pHandleEntry = &m_HandleListEntries[interiorHandle.Interior.TableIndex];

    // LOCK SCOPE; all 'RTE_HandleTableEntry' methods are 
    //             designed to be used in a locked scope!!
    RTESync_LockedScope Lock( m_Lock ); 

    // is handle value plausible and still used?
    if (( pHandleEntry->PlausiblityCnt() == interiorHandle.Interior.PlausibilityCnt ) &&
          pHandleEntry->IsUsed() )
    {
        pHandleEntry->AddRef();
        *ppHandleData = pHandleEntry->GetHandleData();
        retVal        = RTE_HandleManager::NoError;
    }
    else
    {
        *ppHandleData = 0;
        retVal        = pHandleEntry->Referenced() ? RTE_HandleManager::FreePending :
                                                     RTE_HandleManager::InvalidHandle;
    }

    return retVal;
}

/*---------------------------------------------------------------------------*/

inline RTE_HandleManager::HdlRet
    RTE_HandleTable::ReleaseHandleData( RTE_HandleTable::IntHdl const  interiorHandle )
{
    RTE_HandleManager::HdlRet  retVal = RTE_HandleManager::NoError;


    RTE_HandleTableEntry*  pHandleEntry = &m_HandleListEntries[interiorHandle.Interior.TableIndex];

    // LOCK SCOPE; all 'RTE_HandleTableEntry' methods are 
    //             designed to be used in a locked scope!!
    RTESync_LockedScope Lock( m_Lock ); 

    // is handle value plausible
    if (( pHandleEntry->PlausiblityCnt() == interiorHandle.Interior.PlausibilityCnt ) &&
        ( pHandleEntry->Referenced() ))
    {   // ReleaseRef returns true if the handle has been freed 
        // during data access and we are the last releasing the handle
        // data.
        if ( pHandleEntry->ReleaseRef() )
            retVal = RTE_HandleManager::HandleFreed;

    }
    else
        retVal = RTE_HandleManager::InvalidHandle;

    if ( RTE_HandleManager::HandleFreed == retVal )
        AddHandleEntryToFreeList( pHandleEntry );

    return retVal;
}

/*---------------------------------------------------------------------------*/

SAPDB_UInt4  
    RTE_HandleTable::CountHandles ( RTE_HandleManager::HdlTypeInfo const handleType ) const
{
    SAPDB_UInt4  count = 0;
    SAPDB_UInt2  idx   = RTE_HandleTableEntries;

    // We do this without a locked scope because any counted value might be
    // volatile. So a locked scope leads to the same rough value.
    do
    {
        RTE_HandleTableEntry const * pHandleEntry = &m_HandleListEntries[static_cast<SAPDB_UInt1>(idx-1)];

        if (( pHandleEntry->IsUsed() ) &&
            (( handleType == 0 ) || ( handleType == pHandleEntry->GetHandleType() )))
        {
            count++;
        }
    }
    while ( --idx  );

    return count;
}

/*---------------------------------------------------------------------------*/

RTE_HandleManager::HdlRet 
    RTE_HandleTable::FindHandle( RTE_HandleTable::IntHdl               &interiorHandle,
                                 bool                                  findFirstHandle,
                                 RTE_HandleManager::HdlTypeInfo const  handleType,
                                 bool const                            pendingFrees )
{
    RTE_HandleManager::HdlRet  retVal = RTE_HandleManager::NoHandleFound;
    SAPDB_UInt2                idx    = (findFirstHandle) ? 0 : interiorHandle.Interior.TableIndex + 1;

    while ( RTE_HandleTableEntries > idx  )
    {
        RTE_HandleTableEntry*  pHandleEntry = &m_HandleListEntries[static_cast<SAPDB_UInt1>(idx)];

        // do a 'dirty' check first !
        if (( pendingFrees ? pHandleEntry->FreeIsPending(): pHandleEntry->IsUsed()) && 
            (( handleType == 0 ) || ( handleType == pHandleEntry->GetHandleType() )))
        {   // LOCK SCOPE; all 'RTE_HandleTableEntry' methods are 
            //             designed to be used in a locked scope!!
            RTESync_LockedScope Lock( m_Lock ); 

            if (( pendingFrees ? pHandleEntry->FreeIsPending(): pHandleEntry->IsUsed()) && 
                (( handleType == 0 ) || ( handleType == pHandleEntry->GetHandleType() )))
            {   // we have found a handle
                interiorHandle.Interior.PlausibilityCnt = pHandleEntry->PlausiblityCnt();
                retVal                                  = RTE_HandleManager::NoError;
                interiorHandle.Interior.TableIndex      = (SAPDB_UInt1)idx;
                break;
            }
        }
        ++idx;
    }

    return retVal;
}

/*---------------------------- RTE_HandleManager ----------------------------*/

SAPDB_UInt2 const RTE_HandleTableDirEntries = ((unsigned char)-1) + 1;

RTE_HandleManager::RTE_HandleManager() 
                  : m_Lock(0),
                    m_ListSearchPos(0)
{
    SAPDBERR_ASSERT_STATE(RTE_HandleTableDirEntries == ( sizeof(m_pHandleTabDir) / sizeof(m_pHandleTabDir[0])));

    for ( SAPDB_UInt2 i = 0; i < RTE_HandleTableDirEntries; i++ )
        m_pHandleTabDir[static_cast<SAPDB_UInt1>(i)] = 0;
}

/*---------------------------------------------------------------------------*/

RTE_HandleManager::~RTE_HandleManager()
{
    Finish();
}

/*---------------------------------------------------------------------------*/

void RTE_HandleManager::Finish()
{
    for ( SAPDB_UInt2 i = 0; i < RTE_HandleTableDirEntries; i++ )
    {
        RTE_HandleTable* pHdlTab = m_pHandleTabDir[static_cast<SAPDB_UInt1>(i)];

        if ( 0 != pHdlTab )
        {
            { RTESync_LockedScope Lock( m_Lock ); 
              m_pHandleTabDir[static_cast<SAPDB_UInt1>(i)] = 0; }

            destroy ( pHdlTab, RTEMem_RteAllocator::Instance());
        }
    }
}

/*---------------------------------------------------------------------------*/

RTE_HandleManager::NewHdlState 
     RTE_HandleManager::NewHandleInExistingLists( Hdl                   &handle,
                                                  SAPDB_UInt1           &ListSearchPos,
                                                  void *  const         pHandleData,
                                                  HdlTypeInfo const     handleType )
{
    NewHdlState             NewHdlStateVal = NoEntryFound;
    RTE_HandleTable::IntHdl interiorHandle;

    interiorHandle.Opaque = handle;

    for ( SAPDB_UInt2 i = 0; i < RTE_HandleTableDirEntries; i++ )
    {
        RTE_HandleTable* pHandleTab = m_pHandleTabDir[ListSearchPos];

        if ( 0 == pHandleTab )
        {
            NewHdlStateVal = UseNewSlot; // There is at least on free slot, 
                                         // mitigate return value from 
                                         // 'NoEntryFound' to 'UseNewSlot'
        }
        else if ( pHandleTab->EntriesFree())
        {
            if (  pHandleTab->NewHandle( interiorHandle,pHandleData, handleType ) == NoError )
            {
                interiorHandle.Interior.DirIndex = ListSearchPos;
                NewHdlStateVal                   = EntryFound;
                break;
            }
        }
        // calculate new seach position
        ++ListSearchPos;
    }

    handle = interiorHandle.Opaque;

    return NewHdlStateVal;
}

/*---------------------------------------------------------------------------*/

SAPDB_UInt4
    RTE_HandleManager::CountHandles( HdlTypeInfo const handleType ) const
{
    SAPDB_UInt4 usedHandles = 0;

    for ( SAPDB_UInt2 i = 0; i < RTE_HandleTableDirEntries; i++ )
    {
        RTE_HandleTable* pHandleTab = m_pHandleTabDir[static_cast<SAPDB_UInt1>(i)];

        if ( 0 != pHandleTab )
            usedHandles += pHandleTab->CountHandles(handleType);
    }

    return usedHandles;
}

/*---------------------------------------------------------------------------*/

RTE_HandleManager::HdlRet 
    RTE_HandleManager::NewHandle( Hdl                                  &handle,
                                  void *  const                        pHandleData,
                                  RTE_HandleManager::HdlTypeInfo const handleType )
{
    RTE_HandleTable*  pNewHandleList = 0;
    HdlRet            retVal         = NoError;
    NewHdlState       NewHdlStateVal;
    SAPDB_UInt1       ListSearchPos = m_ListSearchPos; // we don't lock this, It's a single byte

    do 
    {
        // Try to create handle in existing handle lists
        NewHdlStateVal = NewHandleInExistingLists( handle, ListSearchPos, pHandleData, handleType );

        if ( UseNewSlot == NewHdlStateVal )
        {   // insert a new handle list object
            if ( 0 == pNewHandleList )
            {
                pNewHandleList = new (RTEMem_RteAllocator::Instance()) RTE_HandleTable;

                if ( 0 == pNewHandleList )
                {
                    retVal = OutOfMemory;
                    break;
                }
            }

            for ( SAPDB_UInt2 i = 0; i < RTE_HandleTableDirEntries; i++ )
            {
                // do a 'dirty' check first!
                if ( 0 == m_pHandleTabDir[ListSearchPos])
                {
                    RTESync_LockedScope Lock( m_Lock ); 

                    if ( 0 == m_pHandleTabDir[ListSearchPos]) 
                    {   // new list inserted
                        m_pHandleTabDir[ListSearchPos] = pNewHandleList;
                        pNewHandleList                 = 0;
                        break;
                    }
                    else
                        break; // Oops, anybody else has inserted a hande list object.
                               // Break out of loop and try to use it.
                }
                // calculate new seach position
                ++ListSearchPos;
            }
        }
    }
    while (( EntryFound   != NewHdlStateVal ) && 
           ( NoEntryFound != NewHdlStateVal ));

    if ( 0 != pNewHandleList )
        destroy ( pNewHandleList, RTEMem_RteAllocator::Instance());

    if ( NoEntryFound == NewHdlStateVal )
        retVal = AllHandlesInUse;

                
    if ( m_ListSearchPos < ListSearchPos ) // do a 'dirty' check first!
    {   
        RTESync_LockedScope Lock( m_Lock ); 

        if ( m_ListSearchPos < ListSearchPos )
            m_ListSearchPos = ListSearchPos;
    }


    return retVal;
}

/*---------------------------------------------------------------------------*/

RTE_HandleManager::HdlRet 
    RTE_HandleManager::LeaseHandleData( Hdl const         handle,
                                        void **  const    ppHandleData )
{
    RTE_HandleTable::IntHdl    interiorHandle;
    RTE_HandleManager::HdlRet  retVal;

    interiorHandle.Opaque = handle;

    RTE_HandleTable* pHandleTab = m_pHandleTabDir[interiorHandle.Interior.DirIndex];

    if ( 0 != pHandleTab )
        retVal = pHandleTab->LeaseHandleData( interiorHandle, ppHandleData );
    else
        retVal = InvalidHandle;

    return retVal;
}

/*---------------------------------------------------------------------------*/

RTE_HandleManager::HdlRet 
    RTE_HandleManager::ReleaseHandleData( Hdl const  handle )
{
    RTE_HandleTable::IntHdl    interiorHandle;
    RTE_HandleManager::HdlRet  retVal;

    interiorHandle.Opaque = handle;

    RTE_HandleTable* pHandleTab = m_pHandleTabDir[interiorHandle.Interior.DirIndex];

    if ( 0 != pHandleTab )
        retVal = pHandleTab->ReleaseHandleData( interiorHandle );
    else
        retVal = InvalidHandle;

    return retVal;
}

/*---------------------------------------------------------------------------*/

RTE_HandleManager::HdlRet 
    RTE_HandleManager::FindHandle( Hdl               &handle,
                                   bool              findFirstHandle,
                                   HdlTypeInfo const handleType,
                                   bool const        pendingFrees )
{
    RTE_HandleTable::IntHdl    interiorHandle;
    RTE_HandleManager::HdlRet  retVal = RTE_HandleManager::NoHandleFound;

    if ( findFirstHandle )
        interiorHandle.Opaque    = 0;
    else
        interiorHandle.Opaque    = handle;

    do
    {
        RTE_HandleTable* pHandleTab = m_pHandleTabDir[interiorHandle.Interior.DirIndex];

        if ( 0 != pHandleTab )
            retVal = pHandleTab->FindHandle( interiorHandle, findFirstHandle, handleType, pendingFrees );

        findFirstHandle = true;
    }
    while (( RTE_HandleManager::NoHandleFound == retVal ) && 
           ( 0 != ++interiorHandle.Interior.DirIndex ));

    if ( RTE_HandleManager::NoError == retVal )
      handle = interiorHandle.Opaque;

    return retVal;
}

/*---------------------------------------------------------------------------*/

RTE_HandleManager::HdlRet
    RTE_HandleManager::FreeHandle( Hdl const                   handle,
                                   void **  const              ppHandleData )
{
    RTE_HandleTable::IntHdl    interiorHandle;
    RTE_HandleManager::HdlRet  retVal;

    interiorHandle.Opaque = handle;

    RTE_HandleTable* pHandleTab = m_pHandleTabDir[interiorHandle.Interior.DirIndex];

    if ( 0 != pHandleTab )
        retVal = pHandleTab->FreeHandle( interiorHandle, ppHandleData );
    else
        retVal = InvalidHandle;

    return retVal;
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/