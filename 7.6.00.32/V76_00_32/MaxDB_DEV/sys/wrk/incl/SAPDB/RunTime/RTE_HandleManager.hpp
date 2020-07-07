/*!========================================================================

  @file         RTE_HandleManager.hpp
  @ingroup      Runtime
  @author       RaymondR

  @brief        Handle Manager
  
                In most situations where an application software layer needs 
                to deal with resources like connections, files etc. an opaque
                handle is used to identify a single resource.
                Using the Handle Manager is efficient way to join an opaque 
                handle and the resource specific parameters.

  @since        2003-09-22  15:05
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


#ifndef RTE_HANDLEMANAGER_HPP
#define RTE_HANDLEMANAGER_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include    "SAPDBCommon/SAPDB_Types.hpp"
#include    "SAPDBCommon/SAPDB_ToString.hpp"
#include    "RunTime/Synchronisation/RTESync_Spinlock.hpp"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/
#define	RTE_HANDLEMANAGER_UNDEF_HANDLE	0

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

class RTE_HandleTable; // forward declaration

/*! 
  @class        RTE_HandleManager
  @brief        Access resource specific parameters via an opaque handle.

                In most situations where an application software layer needs 
                to deal with resources like connections, files etc. an opaque
                handle is used to identify a single resource. The handle is a 
                true magic cookie without any meaning to anyone except for the
                resource layer that are allowed on it (such as file manager).
                Inside the resource layer, a handle is the selector of a 
                data buffer or object that contains resource specific parameters.
                To avoid crashes caused by a corrupt handle, a handle should not 
                be a pointer to direct access this data buffer. However, the 
                use of hash lists to locate the resource data buffer of a 
                given handle leads to perfomance penalties. Using handle tables
                might be more efficient.
                Finally, there should not be a hardcoded limit on the number of 
                handles like a fixed handle array.

                The following "Handle Manager" deals with these constrains.

                Handles to the resource specific parameter blocks are not 
                unique; that is, upon destruction of a resource, the 
                corresponding handle may be recycled and might look exactly 
                like the handle to the destroyed resource. Therefore, if a 
                handle has been recycled it is possible to access the new 
                resource by an old handle value. To mitigate the risk of 
                invalid handle accesses, each handle value contains a 
                16 bits range ( plausibility counter) which is incremented 
                each time the handle is allocated. The remaining bits are
                used to select the data, which means 

*/
class RTE_HandleManager
{
public:
    enum HdlRet  { NoError = 0,        // No error.
                   HandleFreed,        // Handle has been freed.
                   LeasePending,       // Handle could not be freed, because the handle 
                                       // data is currently leased.
                   FreePending,        // Free request is pending.
                   AllHandlesInUse,    // No new handle available because of all handles are in use.
                   InvalidHandle,      // Given handle value is invalid or not in use.
                   NoHandleFound,      // Could not find a used handle value.
                   OutOfMemory };      // No more memory to complete operation.

    typedef SAPDB_UInt1 HdlTypeInfo;
    typedef SAPDB_UInt4 Hdl;

public:
    /*!
      @brief    contructor / destructor of handle manager
     */
            RTE_HandleManager();
    virtual ~RTE_HandleManager();

    /*!
      @brief    cleanup handle manager resource

	  Calling this methode will free all internal used resources. The 
	  caller must be aware that no other thread is calling any other
	  methode of this class during 'Finish' is in progress.
     */
	void    Finish();


    /*!
      @brief    Reserve a new handle value and assign a data pointer and 
                a type to it.

      Calling this methode leads to a new handle value which has been bound to a
      caller defined data pointer or object pointer. A special type can be 
      defined to distinguish the different handle types ( e.g. local connection, 
      remote connection, and so on ).

      @param    handle       [out]      - returned handle value used to locate the
                                          associated data pointer.
      @param    pHandleData  [in]       - data pointer.
      @param    handleType   [in]       - optional argument to distinguish different 
                                          handle types.

      @return   NoError, AllHandlesInUse, OutOfMemory
     */
    HdlRet  NewHandle                 ( Hdl                         &handle,
                                        void *  const               pHandleData,
                                        HdlTypeInfo const           handleType = 0 );

    /*!
      @brief    Lease (get) the data pointer of a given handle value to
                to deal with its data.

      'LeaseHandleData' returns the data pointer of a given handle to 
      access this data. An internal reference counter is incremented to signal
      that the data is in use. The 'ReleaseHandleData' methode subtracts this
      reference counter which meens that the data operation has been finished. If 
      'FreeHandle' has been called between this two calls, the 'FreeHandle' methode 
      returns that the free request was not successful because of a pending data
      lease. After 'ReleaseHandleData' methode has finished and no other leases are 
      pending 'ReleaseHandleData' returns that the handle has been freed. In this 
      situation the data buffer must be delete after the 'ReleaseHandleData' 
      operation. It should not be freed after previous free request!

      It is not allowed to lease handle data after 'FreeHandle' has been called
      (The error 'FreePending' will be returned).

      @param    handle       [in]       - handle value used to locate the associated 
                                          data pointer.
      @param    ppHandleData [out]      - pointer to the data.

      @return   NoError, InvalidHandle, FreePending
     */
    HdlRet  LeaseHandleData           ( Hdl const                   handle,
                                        void **  const              ppHandleData );

    /*!
      @brief    Release the data pointer leased via 'LeaseHandleData'.

      The 'ReleaseHandleData' signals that the data operation has been done. 
      If 'FreeHandle' has been called before, 'ReleaseHandleData' might return
      that the handle has been freed ('HandleFreed'). In this situation the data
      buffer or object must be delete. From now the handle value is invalid.

      @param    handle       [in]       - handle value used to locate the associated 
                                          data pointer.

      @return   NoError, InvalidHandle, HandleFreed
     */
    HdlRet  ReleaseHandleData         ( Hdl const                   handle );


    /*!
      @brief    Free the handle.

      Frees a handle value which has been reserved by 'NewHandle'. If 'FreeHandle'
      has been called during the handle data has been leased via 'LeaseHandleData'
      the return value 'LeasePending' will be replied. In this situation the data
      buffer or object must not be delete after calling 'FreeHandle' but it must be
      done after the last 'ReleaseHandleData' operation.

      Refer to 'LeaseHandleData' and 'ReleaseHandleData'.

      @param    handle       [in]       - handle value used to locate the associated 
                                          data pointer.
      @param    ppHandleData [out]      - optional pointer to the data.

      @return   NoError, InvalidHandle, LeasePending
     */
    HdlRet  FreeHandle                ( Hdl const                   handle,
                                        void **  const              ppHandleData = 0 );


    /*!
      @brief    Find first used handle value.

      This methode returns the first used handle value of a specified handle type.

      @param    handle       [out]      - Returns the first used handle value.
                                          It's also be used as input for 
                                          'FindNextHandle'.
      @param    handleType   [in]       - Optional argument to distinguish different 
                                          handle types. Zero (default) means any type.

      @return   'NoError' if handle found else 'NoHandleFound'
     */
    HdlRet  FindFirstHandle           ( Hdl                         &handle,
                                        HdlTypeInfo const           handleType = 0 )
    {
        return FindHandle ( handle, true, handleType );
    }

    /*!
      @brief    Find next used handle value.

      This methode returns the next used handle value of specified handle type.

      @param    handle       [in/out]   - Returns the the next handle following the
                                          given handle value. 
      @param    handleType   [in]       - Optional argument to distinguish different 
                                          handle types. Zero (default) means any type.

      @return   'NoError' if handle found else 'NoHandleFound'
     */
    HdlRet  FindNextHandle            ( Hdl                         &handle,
                                        HdlTypeInfo const           handleType = 0 )
    {
        return FindHandle ( handle, false, handleType );
    }


    /*!
      @brief    Find first used handle value.

      This methode returns the first used handle value of a specified handle type
      where a free request is pending.

      @param    handle       [out]      - Returns the first used handle value.
                                          It's also be used as input for 
                                          'FindNextPendingFree'.
      @param    handleType   [in]       - Optional argument to distinguish different 
                                          handle types. Zero (default) means any type.

      @return   'NoError' if handle found else 'NoHandleFound'
     */
    HdlRet  FindFirstPendingFree      ( Hdl                         &handle,
                                        HdlTypeInfo const           handleType = 0 )
    {
        return FindHandle ( handle, true, handleType, true );
    }

    /*!
      @brief    Find next used handle value.

      This methode returns the next used handle value of specified handle type,
      where a free request is pending.

      @param    handle       [in/out]   - Returns the the next handle following the
                                          given handle value. 
      @param    handleType   [in]       - Optional argument to distinguish different 
                                          handle types. Zero (default) means any type.

      @return   'NoError' if handle found else 'NoHandleFound'
     */
    HdlRet  FindNextPendingFree       ( Hdl                         &handle,
                                        HdlTypeInfo const           handleType = 0 )
    {
        return FindHandle ( handle, false, handleType, true );
    }

    /*!
      @brief    Number of handles identified by the given type.

      This methode counts the number of handles belonging to the 
      specified type. Keep in mind that the calculate return value
      might be volatile if the handle manager is running in a 
      multi-threaded environment.

      @param    handleType   [in]       - Optional argument handle type. 
                                          Zero (default) means any type.
      @return   Number of used handles
     */
    SAPDB_UInt4 CountHandles(HdlTypeInfo const  handleType = 0) const;

private:
    enum NewHdlState { EntryFound = 0, UseNewSlot, NoEntryFound };

    NewHdlState NewHandleInExistingLists( Hdl                   &handle,
                                          SAPDB_UInt1           &ListSearchPos,
                                          void *  const         pHandleData,
                                          HdlTypeInfo const     handleType );

    HdlRet  FindHandle                ( Hdl                         &handle,
                                        bool                        findFirstHandle,
                                        HdlTypeInfo const           handleType = 0,
                                        bool const                  pendingFrees = false );

private: // don't allow a copy the handle manager
	inline   RTE_HandleManager( const RTE_HandleManager &HdlMan ) {;};

private:
    RTESync_Spinlock     m_Lock;                                    // used for locked scopes
    RTE_HandleTable*     m_pHandleTabDir[((unsigned char)-1) + 1];
    SAPDB_UInt1          m_ListSearchPos;
};


inline SAPDB_ToStringClass SAPDB_ToString(RTE_HandleManager::HdlRet     rc)
{
    switch (rc)
    {
    case RTE_HandleManager::NoError: 
        return SAPDB_ToStringClass("NOERROR");
    case RTE_HandleManager::HandleFreed: 
        return SAPDB_ToStringClass("HANDLEFREED");
    case RTE_HandleManager::LeasePending: 
        return SAPDB_ToStringClass("LEASEPENDING");
    case RTE_HandleManager::FreePending: 
        return SAPDB_ToStringClass("FREEPENDING");
    case RTE_HandleManager::AllHandlesInUse: 
        return SAPDB_ToStringClass("ALLHANDLESINUSE");
    case RTE_HandleManager::InvalidHandle: 
        return SAPDB_ToStringClass("INVALIDHANDLE");
    case RTE_HandleManager::NoHandleFound: 
        return SAPDB_ToStringClass("NOHANDLEFOUND");
    case RTE_HandleManager::OutOfMemory: 
        return SAPDB_ToStringClass("OUTOFMEMORY");
    default:                          
        break;
    }
    return SAPDB_ToStringClass((SAPDB_Int4)rc); 
}


#endif  /* RTE_HANDLEMANAGER_HPP */
