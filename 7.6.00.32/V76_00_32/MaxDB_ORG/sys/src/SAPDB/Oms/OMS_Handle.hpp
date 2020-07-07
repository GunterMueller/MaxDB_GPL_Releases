/*!
 * \file    OMS_Handle.hpp
 * \author  MarkusSi, PeterG, Roterring
 * \brief   OMS handle
 */
/*

    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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

#ifndef __OMS_HANDLE_HPP
#define __OMS_HANDLE_HPP

#include "Oms/OMS_DbpBase.hpp"
#include "Oms/OMS_ObjByKeyIterBase.hpp"
#include "Oms/OMS_ObjByClsIterBase.hpp"
//#include "Oms/OMS_VarObjByClsIter.hpp"
#include "Oms/OMS_StackTraceCallbackInterface.hpp"
#include "Oms/OMS_MassDerefIterators.hpp"    // PTS 1120478

class OmsCallbackInterface;
class OmsIOidReadIterator;
class OmsIOidAppender;
class OmsAllocatorInterface;
struct OmsMonitorData;
class OMS_Session;
class OMS_Context;
class OMS_ContainerEntry;
class OMS_COMMonitor;
class OmsVarObjByClsIter;
class OMS_KeyDesc;
class OMS_RWLock;

#ifdef OMS_IMPLEMENTATION
// OMS implementation-specific:
struct tgg01_COMMonitorTransferBuf;
#endif


/*--------------------------------------------------------------------------*/
/// Interface functions for OMS library
/*!
** The OmsHandle class defines the basic functions of the OMS. These include transaction 
** control functions, functions for reading and changing instances of persistent classes, as 
** well as memory management and diagnostic functions. 
** OmsHandle also contains a mechanism for converting OIDs to addresses. This 
** mechanism is implemented as a hash list, the size of which is optimized automatically to 
** minimize mapping outlay. 
** An application component class is derived, among others, from OmsHandle. In this way, 
** OmsHandle can be accessed via the 'this' pointer in its scope, due to the implicit casting 
**property of C++. 
**
** \see \ref oms_OmsHandle
** \see \ref omsref
**
** \ingroup oms
*/
class AFX_EXT_CLASS OmsHandle : public DbpBase
{
  friend class OmsAbstractObject;
  friend class OmsLockObject;
  friend class OmsLockScope;
  friend class OmsReleasableScope;
  friend class OmsMultiScope;
  /*--- OLD INTERFACES ---*/
  friend class OmsRWLock;
  friend class OmsSharedScope;
  friend class OmsExclusiveScope;
  /*----------------------*/
  friend class OMS_InternalLockScope;
  friend class OMS_InternalMultiScope;
  friend class OmsGlobalAnchorScopeBase;
  friend class OmsSequence;
  friend class OmsCriticalSection;
  friend class OmsObjByClsIterBase;
  friend class OmsObjByKeyIterBase;
  friend class OmsStream;
  friend class OMS_SessionLockObjects;
  friend class OMS_Session;
  friend class OMS_StreamBody;
  friend class OMS_COMMonitor;
  friend class OmsArrayDeref;  // PTS 1120478
  friend class OmsArrayObjectIteratorBase;
  friend class OmsArrayDataIteratorBase;
  friend class OmsDataBuffer; 
  friend class OmsSessionObject; 
  friend class OMS_GlobalUserAllocatorDirectory;
  friend class OMS_GlobalAnchorDirectory;

public:
  OmsHandle  (OmsTypeInt4 sz = 1007);
  ~OmsHandle ();

  /*! \name Transaction Control
  ** \see \ref oms_TransactionAPI
  ** \see \ref example_subtransactions
  */
  //@{
  /*--------------------------------------------------------------------------*/
  /// Commit subtransaction.
  /*!
  ** Successfully closes a subtransaction. The subtransaction is identified by requiredLevel 
  ** resulting from omsStartSubtrans. All open subtransactions with an index greater than or 
  ** equal to requiredLevel are closed successfully. 
  ** If –1 is specified, the subtransaction last opened is successfully closed. 
  ** OmsCommitSubtrans does not influence locks granted within the subtransaction. These 
  ** locks are released at the end of the transaction.
  */
  void        omsCommitSubtrans(int requiredLevel = -1);

  /*--------------------------------------------------------------------------*/
  /// Provides the current nesting depth of subtransactions.
  int         omsCurrentSubtransLevel() const;

  /*--------------------------------------------------------------------------*/
  /// Get current timeout.
  /*!
  ** Provides the current timeout value; in other words, the maximum wait time in seconds 
  ** before a lock is released.
  */
  int         omsGetTimeOut() const;

  /*--------------------------------------------------------------------------*/
  /// Get current callback interface.
  /*!
  ** Provides a pointer to the callback interface identified by guid (if this is active for the 
  ** invoker). In all other cases, NULL is provided.
  */
  OmsCallbackInterface* omsGetTransactionInterface (ClassID) const;

  /*--------------------------------------------------------------------------*/
  /// Create new consistent view.
  void        omsNewConsistentView();

  /*--------------------------------------------------------------------------*/
  /// Create new consistent view, locking specified objects.
  void        omsNewConsistentView (OmsObjectId* pOids, int cntOids, short timeout);

  /*--------------------------------------------------------------------------*/
  /// Create new consistent view, locking specified objects.
  /*!
  ** The instances identified by pOids and cntOids are locked. The maximum wait time 
  ** before the locks are granted is specified in the timeout parameter. If the operation fails 
  ** because the wait time has elapsed, an error of class OmsLockTimeout is thrown. 
  ** If, when a lock is requested, it is determined that the related object does not exist in the 
  ** current consistent view, the corresponding OID is entered under pErrOids. When the 
  ** method is entered, the cntErrOids parameter specifies the maximum number of entries 
  ** that can be stored under pErrOids. If this number is exceeded, an error event of the class 
  ** OmsOutOfDate is thrown. When the omsNewConsistentView call returns, cntErrOids 
  ** specifies the number of OIDs stored at pErrOids. 
  ** Once the omsNewConsistentView method has been completed successfully, a new 
  ** consistent view is defined. 
  */ 
  void        omsNewConsistentView (OmsObjectId* pOids, int cntOids, short timeout, OmsObjectId*, int&);

  /*--------------------------------------------------------------------------*/
  /// Create new consistent view, locking specified objects.
  void        omsNewConsistentView (OmsIOidReadIterator &exclOids, 
                                    short                timeout, 
                                    OmsIOidAppender     &errOids);   // PTS 1115027
  
  /*--------------------------------------------------------------------------*/
  /// Create new consistent view, locking specified objects.
  void        omsNewConsistentView (OmsIOidReadIterator &exclOids, 
                                    OmsIOidReadIterator &sharedOids, 
                                    short                timeout, 
                                    OmsIOidAppender     &exclErrOids,
                                    OmsIOidAppender     &sharedErrOids);   
  
  /*--------------------------------------------------------------------------*/
  /// Set/unset read-only operation.
  /*!
  ** With readOnly = true, persistent objects can no longer be locked within the affected 
  ** OMS session and, as a result, no more changes can be carried out. This applies until 
  ** omsReadOnly is executed with readOnly = false.
  */
  void        omsReadOnly (bool = true);

  /*--------------------------------------------------------------------------*/
  /// Return read-only operation status.
  /*!
  ** \return current read-only status
  ** \see omsReadOnly()
  */
  bool        omsGetReadOnly();

  /*--------------------------------------------------------------------------*/
  /// Release current consistent view.
  /*!
  ** Terminates the current consistent view and generates a new context.
  */
  void        omsReleaseConsistentView();
  
  /*--------------------------------------------------------------------------*/
  /// Register callback interface.
  /*!
  ** Registers and activates the CallbackInterface p identified by guid in the current OMS 
  ** session. The pointer p refers to an instance of a class that is derived from 
  ** OmsCallbackInterface. Only one CallbackInterface can be active at any time; in other 
  ** words, any CallbackInterface that is already active when the call is carried out is 
  ** implicitly deactivated.
  */
  void        omsRegTransactionInterface (ClassID, OmsCallbackInterface*);

  /*--------------------------------------------------------------------------*/
  /// Rollback subtransaction.
  /*!
  ** Undoes the changes of a subtransaction and terminates it. The subtransaction is identified 
  ** by the requiredLevel received as the result of omsStartSubtrans. All changes of all open 
  ** subtransactions whose index is greater than or equal to requiredLevel are undone. 
  ** When –1 is specified, a rollback of the last open subtransaction is carried out. 
  ** OmsRollbackSubtrans has no influence on locks granted within the subtransaction. These 
  ** locks are released at the end of the transaction.
  */
  void        omsRollbackSubtrans( int requiredLevel = - 1);

  /*--------------------------------------------------------------------------*/
  /// Set timeout.
  /*!
  ** Sets the timeout value; in other words, the maximum wait time (in seconds) before the 
  ** release of an exclusive lock requested for a persistent object.
  */
  void        omsSetTimeOut( short );

  /*--------------------------------------------------------------------------*/
  /// Start a new subtransaction.
  /*!
  ** Opens a subtransaction. This means that the point within the current transaction is 
  ** recorded by the liveCache. This can be followed by any sequence of OMS operations. All 
  ** the changes up to this point can then be undone by invoking omsRollbackSubtrans.  
  ** The sequence of OMS operations can contain further calls of omsStartSubtrans, which 
  ** results in a stack of open subtransactions being built up. The index in this stack is 
  ** returned as the result of the method. To close a specific subtransaction with omsCommit 
  ** or omsRollback, this index can be specified by means of the application even if the 
  ** subtransactions that have been opened within the subtransaction are not known.
  */
  int         omsStartSubtrans();

  /// Starts the mode, in which local changes on objects are not seen.
  /*! 
  **  Starts the mode, in which local changes on objects are not seen, this means, that
  **  the objects are dereferenced in the state, they were at the beginning of their
  **  current consistent-view.
  **  During this mode is active, it is not possible to apply any changes on the objects.
  **  This mode can be stopped using a call of omsReadUnchangedObjectsEnd. 
  **  If the method is called with a versionId, then no other session can open this version
  **  and no other session can call this method with the same versionId concurrently.
  ** \since PTS 1137538
  ** 
  ** \parameter versionId If a versionId is specified, then the objects are dereferenced
  **                      in the consistent view of this version (if it exists);
  **                      if no parameter is specified, then the consistent view of the
  **                      current transaction is used.
  **
  ** \attention It is not allowed, that a version is open, or a subtransaction is active 
  **   when these methods are called, and it is also not allowed to open a version or a
  **   subtransaction while being in this mode. 
  */
  void omsReadUnchangedObjectsStart(const OmsVersionId& versionId);
  void omsReadUnchangedObjectsStart();

  /// Stops the mode, in which local changes on objects are not seen.
  /*!
  **  Stops the mode which has been started by calling omsReadUnchangedObjectsStart.
  **  The consistent view, which was valid when starting this mode is active again.
  **  As the read-only-flag (m_read_only) might have been changed by omsReadUnchangedObjectsStart
  **  the variable is now reset to their former value again. 
  */
  void omsReadUnchangedObjectsEnd();
  //@}


  /*! \name Operations on Persistent Objects
  ** \see \ref oms_PersistentObjectsAPI
  */
  //@{
  /*--------------------------------------------------------------------------*/
  /// Create an iterator iterating over all items in the container.
  OmsObjByClsIterBase omsAllOids( 
    ClassID guid, 
    OmsSchemaHandle  Schema,   
    OmsContainerNo   ContainerNo,
    int              maxBufferSize = 20);  // PTS 1120478  

  /*--------------------------------------------------------------------------*/
  /// Cast OID type.
  OmsObjectId omsCastOid(ClassID guid, const OmsObjectId& oid);

  /*--------------------------------------------------------------------------*/
  /// Check OID for validity.
  /*!
  ** Provides true if oid identifies an instance of the persistent class identified by guid in the 
  ** current context; otherwise false.
  */
  bool        omsCheckOid    (const OmsObjectId&, ClassID guid);

  /*--------------------------------------------------------------------------*/
  /// Create key range iterator.
  OmsObjByKeyIterBase omsCreateKeyRangeIter(
    ClassID guid,
    OmsSchemaHandle  sh, 
    OmsContainerNo   ContainerNo,
    const void*      pStartKey,                  
    const void*      pLowerKey,          // PTS 1119480
    const void*      pUpperKey,
    OmsIterDirection direction,          // PTS 1119480
    int              maxBufferSize = 20  // PTS 1119480
    );

  /*--------------------------------------------------------------------------*/
  /// Delete an object.
  void        omsDelete ( OmsAbstractObject* );

  /*--------------------------------------------------------------------------*/
  /// Delete an object.
  /*!
  ** Deletes the instance identified by Oid in the liveCache. A lock for the instance must have 
  ** already been granted to the current transaction. 
  ** The instance is removed both from the liveCache, and from the current context. The 
  ** virtual omsCleanUp method is first invoked. 
  ** The instance is first technically flagged as deleted only in the current context. Normally, 
  ** however, the instance is not actually deleted from the liveCache until the associated 
  ** transaction performs a 'commit'. The liveCache does not, therefore, determine errors, 
  ** such as a missing lock on the instance, until the 'commit' is executed. In this case, the 
  ** actions in the transaction are rolled back implicitly.
  */
  void 		    omsDelete ( const OmsObjectId& );

  /*--------------------------------------------------------------------------*/
  /// Delete all objects in a container.
  void		    omsDeleteAll(ClassID guid, OmsSchemaHandle, OmsContainerNo);

  /*--------------------------------------------------------------------------*/
  /// Get object lock status.
  /*!
  ** Provides true if the invoking transaction has a lock on the instance identified by oid; 
  ** otherwise, false.
  */
  bool        omsIsLocked ( const OmsObjectId& , bool shared=false);

  /*--------------------------------------------------------------------------*/
  /// Lock an object for update or delete.
  /*!
  ** Requests an exclusive lock for the instance identified by oid. 
  ** If the instance is locked by another transaction when the request is made in the 
  ** liveCache, the system waits for the release for the time set in the timeout value.  
  ** If the lock cannot be granted because the maximum wait time has elapsed, an error event 
  ** of the type OmsLockTimeout occurs. 
  ** If, when the lock is requested, the liveCache determines that the current consistent view 
  ** no longer corresponds to the current state, an error event of the type OmsOutOfDate 
  ** occurs.
  **
  ** \param oid           [in] oid of the object which should be locked
  ** \param withoutDeref  [in] if the parameter equals true, then the object is not dereferenced
  **                           when it does not exist in the liboms (only a header frame is allocated)
  */
  void        omsLock(const OmsObjectId &oid, bool withoutDeref=false);

  /*--------------------------------------------------------------------------*/
  /// Auire a shared lock of an object.
  /*!
  ** Requests a shared lock for the instance identified by oid. 
  ** If, when the lock is requested, the liveCache determines that the current consistent view 
  ** no longer corresponds to the current state, an error event of the type OmsOutOfDate 
  ** occurs.
  ** It is not waited for a release of the lock if an exclusive lock is currently hold by another
  ** transaction.
  **
  ** \param oid           [in] oid of the object which should be locked
  ** \param withoutDeref  [in] if the parameter equals true, then the object is not dereferenced
  **                           when it does not exist in the liboms (only a header frame is allocated)
  */
  void        omsLockShared(const OmsObjectId &oid, bool withoutDeref=false);

  /*--------------------------------------------------------------------------*/
  /// Try to lock an object for update or delete.
  /*!
  ** Requests an exclusive lock for the instance identified by oid. 
  ** If the instance is locked by another transaction when the request is made in the 
  ** liveCache, the system waits for the release for the time set in the timeout value.  
  ** If the lock cannot be granted because the maximum wait time has elapsed, \c false
  ** is returned.
  ** If, when the lock is requested, the liveCache determines that the current consistent view 
  ** no longer corresponds to the current state, \c false is returned.
  **
  ** \param oid           [in] oid of the object which should be locked
  ** \param withoutDeref  [in] if the parameter equals true, then the object is not dereferenced
  **                           when it does not exist in the liboms (only a header frame is allocated)
  */
  bool        omsTryLock(const OmsObjectId &oid, bool withoutDeref=false);

  /*--------------------------------------------------------------------------*/
  /// Try to lock an object in shared mode.
  /*!
  ** Requests an shared lock for the instance identified by oid. 
  ** If the instance is locked by another transaction when the request is made in the 
  ** liveCache, the system waits for the release for the time set in the timeout value.  
  ** If the lock cannot be granted because the maximum wait time has elapsed, \c false
  ** is returned.
  ** If, when the lock is requested, the liveCache determines that the current consistent view 
  ** no longer corresponds to the current state, \c false is returned.
  **
  ** \param oid           [in] oid of the object which should be locked
  ** \param withoutDeref  [in] if the parameter equals true, then the object is not dereferenced
  **                           when it does not exist in the liboms (only a header frame is allocated)
  */
  bool        omsTryLockShared(const OmsObjectId &oid, bool withoutDeref=false);

  /*--------------------------------------------------------------------------*/
  /// Lock an array of objects for update or delete.
  /*! 
  ** Requests an exclusive lock for each instance identified by an oid in the array.
  ** (Underneath the method OMS_Handle::omsLock is called for each array-entry. So
  ** refer to this method for a more detailed description.)
  **
  ** \param pOidArray     [in] array with oid for which a lock should be aquired
  ** \param noOfEntries   [in] size of entries in the array
  ** \param withoutDeref  [in] if the parameter equals true, then the objects are not dereferenced
  **                           when they do not exist in the liboms (only header frames is allocated)
  */
  void        omsLocks(const OmsObjectId *pOidArray, int noOfEntries, bool withoutDeref=false);

  /*--------------------------------------------------------------------------*/
  /// Aquire shared locks of an array of objects.
  /*! 
  ** Requests a shared lock for each instance identified by an oid in the array.
  ** (Underneath the method OMS_Handle::omsLockShared is called for each array-entry. So
  ** refer to this method for a more detailed description.)
  **
  ** \param pOidArray     [in] array with oid for which a lock should be aquired
  ** \param noOfEntries   [in] size of entries in the array
  ** \param withoutDeref  [in] if the parameter equals true, then the objects are not dereferenced
  **                           when they do not exist in the liboms (only header frames is allocated)
  */
  void        omsLocksShared(const OmsObjectId *pOidArray, int noOfEntries, bool withoutDeref=false);

  /*--------------------------------------------------------------------------*/
  /// Locks all objects for update or delete which are enumerated by pOids.
  /*!
  ** This method tries to lock all objects whose oid is enumerated by the iterator pOids. 
  ** If some of these objects are out-of-date, then the corresponding oids are appended
  ** to the iterator pErrOids. If more objects are out-of-date than the iterator can contain,
  ** then an exception OmsOutOfDate is thrown and the already aquired locks of all objects, 
  ** which are not stored in the local oms-cache, are freed.
  ** The timeout specified, is used for the complete routine. If a timeout occurs, then the
  ** exception e_OMS_lock_collision is thrown.
  **
  ** \param oids          [in] Iterator which enumerates all objects for which a lock shoud be aquired.
  ** \param timeout       [in] the maximal execution time in seconds for getting all the locks.
  ** \param errOids       [out] Iterator which enumerates all objects for which no lock could be
  **                            aquired because of an out-of-date.
  ** \param withoutDeref  [in]  if the parameter equals true, then the objects are not dereferenced
  **                            when they do not exist in the liboms (only header frames is allocated)
  **
  ** \since 7.4.4 (PTS 1121449)
  */
  void        omsLockMass(OmsIOidReadIterator& oids, short timeout, OmsIOidAppender& errOids, bool withoutDeref=false);   

  /*--------------------------------------------------------------------------*/
  /// Aquire shared locks on all objects which are enumerated by pOids.
  /*!
  ** This method tries to get a shared lock on all objects whose oid is enumerated by the iterator pOids. 
  ** If some of these objects are out-of-date, then the corresponding oids are appended
  ** to the iterator pErrOids. If more objects are out-of-date than the iterator can contain,
  ** then an exception OmsOutOfDate is thrown and the already aquired locks of all objects, 
  ** which are not stored in the local oms-cache, are freed.
  **
  ** \param oids          [in]  Iterator which enumerates all objects for which a lock shoud be aquired.
  ** \param timeout       [in] the maximal execution time in seconds for getting all the locks.
  ** \param errOids       [out] Iterator which enumerates all objects for which no lock could be
  **                            aquired because of an out-of-date.
  ** \param withoutDeref  [in]  if the parameter equals true, then the objects are not dereferenced
  **                            when they do not exist in the liboms (only header frames is allocated)
  **
  ** \since 7.6 
  */
  void        omsLockSharedMass(OmsIOidReadIterator& oids, short timeout, OmsIOidAppender& errOids, bool withoutDeref=false);   

  /*--------------------------------------------------------------------------*/
  /// Dereferences several objects in a single call.
  /*!
  ** Dereferences several objects in a single call.
  **
  ** \param derefIter Interface to get the oid's which should be dereferenced and after
  **        the call the iterator contains the dereferenced pointers and an error code 
  **        if an error has occured
  ** \return number of errors that has occured during dereferencing
  **
  ** \see \ref omsMassDeref_selfDefinedIterator
  **
  ** \since 7.4.4 PTS 1120717
  */
  int         omsDerefMass(const OmsIDerefIter &derefIter);

  /*--------------------------------------------------------------------------*/
  /// Dereferences several objects for update in a single call.
  /*!
  ** Dereferences several objects for update in a single call.
  **
  ** \param derefIter Interface to get the oid's which should be dereferenced and after
  **        the call the iterator contains the dereferenced pointers and an error code 
  **        if an error has occured
  ** \param doLock Should the objects be locked?
  ** \return number of errors that has occured during dereferencing
  **
  ** \see \ref omsMassDeref_selfDefinedIterator
  **
  ** \since 7.4.4 PTS 1120717
  */
  int         omsDerefForUpdMass(const OmsIDerefIterForUpd &derefIter, bool doLock);
  
  /*--------------------------------------------------------------------------*/
  /// Wrapper to dereference several objects in a single call.
  /*!
  ** Wrapper to dereference several objects in a single call.
  **
  ** \param derefIter As input parameter this interface is used to enumerate
  **        the oids to be dereferenced, and as output parameter the objects together
  **        with the pointer to the dereferenced objects.
  ** \return number of errors that has occured during dereferencing
  **
  ** \see \ref omsMassDeref_setIterator
  **
  ** \since 7.4.4 PTS 1120717
  */
  int         omsDerefMass(const OmsDeref &derefIter);

  /*--------------------------------------------------------------------------*/
  /// Wrapper to dereference several objects for update in a single call.
  /*!
  ** Wrapper to dereference several objects in a single call for update.
  **
  ** \param derefIter As input parameter this interface is used to enumerate
  **        the oids to be dereferenced, and as output parameter the objects together
  **        with the pointer to the dereferenced objects.
  ** \param doLock Should the objects be locked?
  ** \return number of errors that has occured during dereferencing
  **
  ** \see \ref omsMassDeref_setIterator
  **
  ** \since 7.4.4 PTS 1120717
  */
  int         omsDerefForUpdMass(const OmsDerefForUpd &derefIter, bool doLock);

  /*--------------------------------------------------------------------------*/
  /// Wrapper to dereference an array of objects in a single call.
  /*!
  ** Wrapper to dereference an array of objects in a single call.
  **
  ** \param derefIter As input parameter this interface is used to enumerate
  **        the oids to be dereferenced, and as output parameter the objects together
  **        with the pointer to the dereferenced objects.
  ** \return number of errors that has occured during dereferencing
  **
  ** \see \ref omsMassDeref_arrayIterator
  **
  ** \since 7.4.4 PTS 1120717
  */
  int         omsDerefMass(const OmsDerefArray &derefIter);

  /*--------------------------------------------------------------------------*/
  /// Wrapper to dereference an array of objects for update in a single call.
  /*!
  ** Wrapper to dereference an array of objects for update in a single call.
  **
  ** \param derefIter As input parameter this interface is used to enumerate
  **        the oids to be dereferenced, and as output parameter the objects together
  **        with the pointer to the dereferenced objects.
  ** \param doLock Should the objects be locked?
  ** \return number of errors that has occured during dereferencing
  **
  ** \see \ref omsMassDeref_arrayIterator
  **
  ** \since 7.4.4 PTS 1120717
  */
  int         omsDerefForUpdMass(const OmsDerefArrayForUpd &derefIter, bool doLock);


  /*--------------------------------------------------------------------------*/
  /// Dereferences several objects via key in a single call.
  /*!
  ** Dereferences several objects via key in a single call.
  **
  ** \param derefIter Interface to get the keys's which should be dereferenced and after
  **        the call the iterator contains the dereferenced pointers and an error code 
  **        if an error has occured
  ** \return number of errors that has occured during dereferencing
  **
  ** \see \ref omsMassDerefKey_selfDefinedIterator
  **
  ** \since 7.4.4 PTS 1122194
  */
  int         omsDerefKeyMass(const OmsIDerefKeyIter &derefIter);

  /*--------------------------------------------------------------------------*/
  /// Wrapper to dereference an array of objects via key in a single call.
  /*!
  ** Wrapper to dereference an array of objects via key in a single call.
  **
  ** \param derefIter This interface is used to enumerate the keys (together with 
  **        the class, schema and container information) to be dereferenced.
  **        The result of the dereference-operation is also written into this
  **        interface.
  ** \return The number of errors, that occured during the operation
  **
  ** \see \ref omsMassDerefKey_arrayIterator
  **
  ** \since 7.4.4 PTS 1122194
  */
  int         omsDerefKeyMass(const OmsDerefKeyArray &derefIter);

  /*--------------------------------------------------------------------------*/
  /// Dereferences several objects via key for update in a single call.
  /*!
  ** Dereferences several objects via key for update in a single call.
  **
  ** \param derefIter Interface to get the keys's which should be dereferenced and after
  **        the call the iterator contains the dereferenced pointers and an error code 
  **        if an error has occured
  ** \param doLock Should the objects be locked?
  ** \return number of errors that has occured during dereferencing
  **
  ** \see \ref omsMassDerefKey_selfDefinedIterator
  **
  ** \since 7.4.4 PTS 1122194
  */
  int         omsDerefKeyMassForUpd(const OmsIDerefKeyIterForUpd &derefIter, bool doLock);

  /*--------------------------------------------------------------------------*/
  /// Wrapper to dereference an array of objects via key in a single call.
  /*!
  ** Wrapper to dereference an array of objects via key in a single call.
  **
  ** \param derefIter This interface is used to enumerate the keys (together with 
  **        the class, schema and container information) to be dereferenced.
  **        The result of the dereference-operation is also written into this
  **        interface.
  ** \param doLock Should the objects be locked?
  ** \return The number of errors, that occured during the operation
  **
  ** \see \ref omsMassDerefKey_arrayIterator
  **
  ** \since 7.4.4 PTS 1122194
  */
  int         omsDerefKeyMassForUpd(const OmsDerefKeyArrayForUpd &derefIter, bool doLock);

  /*--------------------------------------------------------------------------*/
  /// Create a new object.
  OmsAbstractObject* omsNewObject ( ClassID, OmsSchemaHandle, OmsContainerNo, size_t );

  /*--------------------------------------------------------------------------*/
  /// Create a new keyed object.
  OmsAbstractObject* omsNewKeyedObject( ClassID guid, const unsigned char* key, OmsSchemaHandle, OmsContainerNo );

  /*--------------------------------------------------------------------------*/
  /// Create a new object for registration.
  OmsAbstractObject* omsNewRegistryObject (size_t sz, ClassID guid);

  /*--------------------------------------------------------------------------*/
  /// Release object from the cache.
  /*!
  ** Notifies the current OMS context that the instance identified by oid can be removed from 
  ** the current context cache. 
  ** It cannot be guaranteed, however, that the instance will actually be removed from the 
  ** context cache. This is not possible, for example, if the instance has already been updated 
  ** in a subtransaction. 
  ** No error event occurs if the instance does not exist in the current context cache.
  */
  void        omsRelease ( const OmsObjectId& );

  /*--------------------------------------------------------------------------*/
  /// Release all objects from the cache.
  /*!
  ** Executes the omsRelease method for every instance in the current context cache.
  */
  void        omsReleaseAll();

  /*--------------------------------------------------------------------------*/
  /// Return the revision number of the object in the current consistent view
  /*!
  ** Return the revision number of the object in the current consistent view. This counter
  ** is updated in the kernel(!) each time an object is changed and this change is committed. 
  ** Inside a consistent view this counter always remain unchanged. 
  **
  ** \attention Because of space limitations, only the last 4 bytes are returned. Therefore the 
  ** monotony can not be guaranteed.
  */
  unsigned int omsGetObjectRevisionNumber(const OmsObjectId &oid);
  //@}


  /*! \name Operations on Persistent Objects of Variable Length
  ** \see \ref oms_VarObjectsAPI
  ** \see \ref example_varobjects
  */
  //@{
  /*--------------------------------------------------------------------------*/
  /// Create an iterator iterating over all items in the container.
  /*!
  ** Create an iterator iterating over all items in the container.
  **
  ** \param Schema        [in] Schema handle of the schema in which the instance is stored.
  ** \param ContainerNo   [in] Number of the container in which the instance is stored.
  ** \param maxBufferSize [in] Number of oids which are fetched in a single call from the kernel. (Default 20)
  **
  ** \since PTS 1126047
  */
  OmsVarObjByClsIter omsAllVarOids( 
    ClassID          guid,
    OmsSchemaHandle  schema,   
    OmsContainerNo   containerNo,
    int              maxBufferSize = 20);      

  /*--------------------------------------------------------------------------*/
  /// Check whether OID belongs to a valid object of variable size.
  /*!
  ** Provides true if oid identifies an instance of an object of variable length in the current 
  ** context; otherwise false.
  */
  bool        omsCheckOid    (const OmsVarOid& );

  /*--------------------------------------------------------------------------*/
  /// Delete an object.
  /*!
  ** Deletes the instance of an object of variable length identified by Oid in the liveCache and 
  ** in the current context cache. A lock for the instance must already have been granted to 
  ** the current transaction. 
  ** The instance is removed from the liveCache. In fact, the instance is only initially flagged 
  ** as deleted; it is not actually deleted in liveCache Basis until the transaction performs a 
  ** 'commit'.
  */
  void        omsDelete ( const OmsVarOid& );

  /*--------------------------------------------------------------------------*/
  /// Dereference a variable-sized object.
  const void* omsDeRef(const OmsVarOid&);

  /*--------------------------------------------------------------------------*/
  /// Dereference a variable-sized object for update.
  /*!
  **  Dereference a variable-sized object for update.
  **  After the object has been dereferenced via this method and has been updated  
  **  the method OmsHandle::omsStore(const OmsVarOid) must be called.
  **
  ** \param oid  [in]  oid of the object which should be dereferenced
  ** \param size [out] size of the dereferenced object
  ** \return     pointer to the variable-sized object
  **
  ** \attention: it is not allowed to change the size of the object.
  */
  void* omsDeRefForUpd_ConstSize(const OmsVarOid &oid, unsigned long &size);

  /*--------------------------------------------------------------------------*/
  /// Read variable-sized object size.
  /*!
  ** Provides the size in bytes of the instance identified by oid of a persistent object of 
  ** variable length. If the instance does not exist in the current consistent view, an error of 
  ** type DbpError is thrown.
  */
  unsigned long  omsGetVarObjectSize (const OmsVarOid& );

  /*--------------------------------------------------------------------------*/
  /// Get variable-sized object size.
  unsigned long omsGetVarObjSize (const OmsVarOid& oid);

  /*--------------------------------------------------------------------------*/
  /// Read variable-sized object.
  /*!
  ** Generates a copy of the instance (identified by oid) of a persistent object of variable 
  ** length at the address specified by p. The memory area of the size size must be allocated 
  ** beforehand by the invoker. The required size can be determined by invoking the 
  ** omsGetVarObjectSize method. 
  ** If no instance identified by oid exists, an error of type DbpError is thrown.
  */
  void        omsLoadVarObject (const OmsVarOid& oid, unsigned long size, void* buf);

  /*--------------------------------------------------------------------------*/
  /// Lock an object for update or delete.
  /*!
  ** Requests an exclusive lock for the instance identified by an Oid of a persistent object of 
  ** variable length. 
  ** If the instance is locked by another transaction when the request is made in the 
  ** liveCache, the system waits for the release for the time set in the timeout value.  
  ** If the lock cannot be granted because the maximum wait time has elapsed, an error event 
  ** of the type OmsLockTimeout occurs. 
  ** If, when the lock is requested, the liveCache determines that the current consistent view 
  ** no longer corresponds to the current object state, an error of type OmsOutOfDate is 
  ** thrown.
  **
  ** \param oid           [in] oid of the object which should be locked
  ** \param withoutDeref  [in] if the parameter equals true, then the object is not dereferenced
  **                           when it does not exist in the liboms (only a header frame is allocated)
  */
  void        omsLock( const OmsVarOid& oid, bool withoutDeref=false);

  /*--------------------------------------------------------------------------*/
  /// Try to lock an object for update or delete.
  /*!
  ** Requests an exclusive lock for the instance identified by an Oid of a persistent object of 
  ** variable length. 
  ** If the instance is locked by another transaction when the request is made in the 
  ** liveCache, the system waits for the release for the time set in the timeout value.  
  ** If the lock cannot be granted because the maximum wait time has elapsed, \c false
  ** is returned.
  ** If, when the lock is requested, the liveCache determines that the current consistent view 
  ** no longer corresponds to the current object state, \c false is returned.
  **
  ** \param oid           [in] oid of the object which should be locked
  ** \param withoutDeref  [in] if the parameter equals true, then the object is not dereferenced
  **                           when it does not exist in the liboms (only a header frame is allocated)
  */
  bool        omsTryLock(const OmsVarOid& oid, bool withoutDeref=false);

  /*--------------------------------------------------------------------------*/
  /// Try to lock an object in shared mode.
  /*!
  ** Requests an shared lock for the instance identified by an Oid of a persistent object of 
  ** variable length. 
  ** If the instance is locked by another transaction when the request is made in the 
  ** liveCache, the system waits for the release for the time set in the timeout value.  
  ** If the lock cannot be granted because the maximum wait time has elapsed, \c false
  ** is returned.
  ** If, when the lock is requested, the liveCache determines that the current consistent view 
  ** no longer corresponds to the current object state, \c false is returned.
  **
  ** \param oid           [in] oid of the object which should be locked
  ** \param withoutDeref  [in] if the parameter equals true, then the object is not dereferenced
  **                           when it does not exist in the liboms (only a header frame is allocated)
  */
  bool        omsTryLockShared(const OmsVarOid &oid, bool withoutDeref=false);

  /*--------------------------------------------------------------------------*/
  /// Lock an object in shared mode.
  /*!
  ** Requests a shared lock for the instance identified by an Oid of a persistent object of 
  ** variable length. 
  ** If, when the lock is requested, the liveCache determines that the current consistent view 
  ** no longer corresponds to the current object state, an error of type OmsOutOfDate is 
  ** thrown.
  ** It is not waited for a release of the lock if an exclusive lock is currently hold by another
  ** transaction.
  **
  ** \param oid           [in] oid of the object which should be locked
  ** \param withoutDeref  [in] if the parameter equals true, then the object is not dereferenced
  **                           when it does not exist in the liboms (only a header frame is allocated)
  **
  ** \since PTS 1130435
  */
  void        omsLockShared(const OmsVarOid& oid, bool withoutDeref=false);

  /*--------------------------------------------------------------------------*/
  /// Create new variable-sized object.
  /*!
  ** Provides the OID for a new instance of a persistent object of variable length. The 
  ** instance can be stored later with this OID in the liveCache using the omsStoreVarObject 
  ** method.
  */
  OmsVarOid omsNewVarObject(OmsSchemaHandle, OmsContainerNo, ClassID guid=OMS_VAR_OBJ_GUID);

  /*--------------------------------------------------------------------------*/
  /// Release variable-sized object from the cache.
  void        omsRelease ( const OmsVarOid& oid );

  /*--------------------------------------------------------------------------*/
  /// Release "session-specific lock" of variable-sized object.
  /*!
  ** By invoking this method, the invoker indicates that it is releasing a copy of an object of 
  ** variable length received from omsLoadVarObject. This ensures that a session has at 
  ** most one copy of an object.
  */
  void        omsReleaseVarObject(const OmsVarOid& oid);

  /*--------------------------------------------------------------------------*/
  /// Stores the updates of an object after this has been dereferenced via omsDeRefForUpd_ConstSize
  /*!
  **  Stores the updates of an object after this has been dereferenced via omsDeRefForUpd_ConstSize.
  **  In this method it is checked that no overwriting has occurred.
  **
  **  \param oid [in] oid of the object which should be stored
  */
  void        omsStore(const OmsVarOid& oid);

  /*--------------------------------------------------------------------------*/
  /// Store variable-sized object data.
  /*!
  ** Stores the instance of the length objLength identified by obj under the oid in the 
  ** liveCache. Before the object is stored for the first time, oid must be requested by 
  ** invoking omsNewVarObject. 
  ** omsStoreVarObject fails if the current transaction has no lock for the oid.
  */
  void        omsStoreVarObject ( const OmsVarOid& oid, const void*, unsigned long objLength);
  //@}


  /*! \name Versions
  ** \see \ref oms_VersionAPI
  ** \see \ref example_version
  */
  //@{
  /*--------------------------------------------------------------------------*/
  /// Close version.
  /*!
  ** If the version identified by versionId is active for the invoker, it is deactivated, and a new 
  ** context is created implicitly. All subsequent operations then refer to the liveCache state 
  ** when omsCloseVersion was executed. 
  ** If the version identified by versionId is not active for the user, the execution of 
  ** omsCloseVersion has no effect.
  */
  void           omsCloseVersion     ( const OmsVersionId& );

  /*--------------------------------------------------------------------------*/
  /// Create a new version.
  /*!
  ** Creates a version and stores it in the form of meta data in the liveCache catalog under the 
  ** name specified by versionId. A version is a named context that represents a logical copy 
  ** of the consistent liveCache state that is valid when it was executed. 
  ** The version is activated implicitly; in other words, until omsCloseVersion is executed, all 
  ** subsequent OMS operations in the OMS session refer to the logical copy identified by 
  ** the version. 
  ** After the version has been closed, it can be reactivated at any time with 
  ** omsOpenVersion. Since the copy of a version only exists logically, the liveCache may no 
  ** longer be capable of providing older versions. In this case, an instance of the class 
  ** DbpError is thrown. 
  ** When the call is carried out, instances must not have been changed in any way in the 
  ** current transaction. 
  ** Versions are temporary objects. This means that when the liveCache is shut down, all 
  ** versions are implicitly deleted.
  */
  void           omsCreateVersion(const OmsVersionId&);                                             // PTS 1117690

  /*--------------------------------------------------------------------------*/
  /// Create a new version.
#if defined(OMS_IMPLEMENTATION) || defined(SAPDB_UNICODE)
  void           omsCreateVersion(const OmsVersionId&, const OmsTypeWyde* pDesc);                    // PTS 1117690
#endif
#if defined(OMS_IMPLEMENTATION) || !defined(SAPDB_UNICODE)
  void           omsCreateVersion(const OmsVersionId&, const char* pDesc);                           // PTS 1117690
#endif

  /*--------------------------------------------------------------------------*/
  /// Return current version ID, if in version.
  /*!
  ** If a version is active for the invoker, a pointer to its name is provided. 
  ** The result is otherwise NULL. The pointer provided is only valid as long 
  ** as the version exists.
  */
  const OmsVersionId* omsCurrentVersionId () const;

  /*--------------------------------------------------------------------------*/
  /// Drop version.
  /*!
  ** Deletes the version identified by versionId. If this version is active for the invoker, it is 
  ** deactivated implicitly with omsCloseVersion. 
  ** If the version is active for a task other than the invoker, omsDropVersion fails. 
  ** Once omsDropVersion has been executed, the version is finally deleted, and cannot be 
  ** restored.
  */
  void           omsDropVersion      ( const OmsVersionId& );

  /*--------------------------------------------------------------------------*/
  /// Check version existence.
  /*!
  ** Provides true if the version identified by versionId exists; otherwise false.
  */
  bool           omsExistsVersion    ( const OmsVersionId& );

  /*--------------------------------------------------------------------------*/
  /// Force version drop.
  /*!
  ** Functions in the same way as omsDropVersion, but does not fail if the version is opened 
  ** by a task other than the invoker when the method is invoked. Instead, the version is 
  ** flagged for deletion, and is not actually deleted until the version is closed.
  */
  void           omsForceDropVersion ( const OmsVersionId& );

  /*--------------------------------------------------------------------------*/
  /// Get version information.
  /*!
  ** Returns information about the version specified by the versionId. 
  ** \param versionId Id of the version the information should be read for
  ** \param versionInfo This structure delivers the following information:<br>
  **   <b>createDate, createTime:</b> Date and time of the creation of the version<br>
  **   <b>openDate, openTime:</b> Date and time of the last open operation (only if
  **       version is active<br>
  **   <b>consistentView:</b> Transaction-Id of the consistent view<br>
  **   <b>isMarked, isOpen, isUnloaded:</b> Flag which indicates whether the version is
  **       marked, active or unloaded from the oms-cache<br>
  **   <b>HeapUsage, hashSize:</b> Size (in bytes) of the allocated heap and number of
  **       hash-buckets in the object cache of this version
  ** \param pDescBuf a pointer to a buffer to read the description of the version. 
  **        The description is stored in this buffer as NULL-terminated string.
  **        If the pointer equals NULL, then nothing no description is returned.
  ** \param bufLen length of the buffer to store the description. If the description
  **        stored in the version is longer than the buffer, then the description
  **        is truncated.
  ** \since PTS 1117690
  */
#if defined(OMS_IMPLEMENTATION) || defined(SAPDB_UNICODE)
  bool           omsGetVersionInfo(const OmsVersionId& versionId, OmsVersionInfo& versionInfo, 
                                   OmsTypeWyde* pDescBuf, int bufLen) const;    
#endif
#if defined(OMS_IMPLEMENTATION) || !defined(SAPDB_UNICODE)
  bool           omsGetVersionInfo(const OmsVersionId& versionId, OmsVersionInfo& versionInfo, 
                                   char* pDescBuf, int bufLen) const;           
#endif

  /*--------------------------------------------------------------------------*/
  /// ???
  void           omsMarkVersion      ( const OmsVersionId& );

  /*--------------------------------------------------------------------------*/
  /// Open existing version.
  /*!
  ** Activates the version identified by versionId for the invoker; in other words, until 
  ** omsCloseVersion is executed, all the subsequent OMS operations within the OMS 
  ** session refer to the context identified by the version. 
  ** When omsOpenVersion is executed, the version must not be active for another liveCache 
  ** task. 
  ** After the version has been closed, it can be reactivated at any time by means of 
  ** omsOpenVersion. Since the copy of a version only exists logically, however, the 
  ** liveCache may no longer be capable of providing older versions. In this case, an instance 
  ** of the class DbpError is thrown. 
  ** When the call is carried out, instances must not have been changed in any way in the 
  ** current transactions. 
  */
  void           omsOpenVersion      ( const OmsVersionId& );

  /*--------------------------------------------------------------------------*/
  /// Rename a version.
  /*!
  ** Changes the name of the version identified by oldId to newId. NewId must not 
  ** correspond to the name of an existing version.
  ** 
  ** \param oldId id of the version which should be renamed
  ** \param newId id after the rename
  **
  ** \exception DbpError e_unknown_version (= -28514): There is no version with the old oid
  ** \exception DbpError e_version_bound_by_trans (= -28532): The version which should be renamed, is bound
  **            by another transaction
  ** \exception DbpError e_OMS_duplicate_name (= -6006) There is already a version existing which has the 
  **            same id as the specified new id.
  */
  void           omsRenameVersion    ( const OmsVersionId& oldId, const OmsVersionId& newId);

  /*--------------------------------------------------------------------------*/
  /// Reset version status.
  void        omsResetVersion(const OmsVersionId& version); /* PTS 1115134 */

  /*--------------------------------------------------------------------------*/
  /// Set version description.
  /*!
  ** Updates resp. deletes (pDesc == NULL) the description of the currently active
  ** version. 
  **
  ** \param pDesc NULL-terminated string with the description resp. NULL
  **
  ** \attention If no version is active, then an exception is thrown.
  */
#if defined(OMS_IMPLEMENTATION) || defined(SAPDB_UNICODE)
  void           omsSetCurrVersionDesc(const OmsTypeWyde* pDesc);                                    // PTS 1117690
#endif
#if defined(OMS_IMPLEMENTATION) || !defined(SAPDB_UNICODE)
  void           omsSetCurrVersionDesc(const char* pDesc);                                           // PTS 1117690
#endif

  /*--------------------------------------------------------------------------*/
  /// Check whether the current version is marked as dropped.
  /*! 
  ** \return \c true if session is currently in a version and this version is marked
  **         as dropped; otherwise \c false
  **
  ** \attention If no version is active, then an exception is thrown.
  */
  bool        omsCurrVersionIsDropped() const; /* PTS 1133968 */
  //@}


  /*! \name Error Handling and Diagnostics
  ** \see \ref oms_ErrorAPI
  */
  //@{
  /*--------------------------------------------------------------------------*/
  /// Set cancel flag.
  void        omsCancel(); // PTS 1113128 

  /*--------------------------------------------------------------------------*/
  /// Get/set cancel flag.
  bool        omsCancel(bool toCancel);     // PTS 1118194

  /*--------------------------------------------------------------------------*/
  /// Check cancel flag and throw exception if set.
  void        omsCheckCancelFlag() const; // PTS 1110812

  /*--------------------------------------------------------------------------*/
  /// Control test kernel generating random bad_alloc exception.
  void        omsControlBadAllocExceptions(int);  // PTS 1112358

  /*--------------------------------------------------------------------------*/
  /// Control test kernel generating cancel request.
  void        omsControlCancelFlag(int);          // PTS 1112358

  /*--------------------------------------------------------------------------*/
  /// Control test kernel generating random DbpError exception.
  void        omsControlDbpException(int);        // PTS 1112358

  /*--------------------------------------------------------------------------*/
  /// Dereferences an untyped oid for update (Only for analysis purpose! Do not use!)
  /*!
  ** This method can be used to dereference untyped oids for update operations. 
  ** \attentaion This is only needed for analysis purposes! Normal dereference-operations 
  ** should always use the type-proof interface.
  **
  ** The oid can either belong to a fix-sized or to a variable sized object. If an object
  ** was found, this object is already locked exclusively and if the oid belongs to a fixed
  ** sized object, then the pointer, which is returned, is already aquired for update.
  ** 
  ** \param page       [in]  Page of the oid which should be dereferenced.
  ** \param offset     [in]  Offset in the page of the oid which should be dereferenced.
  ** \param generation [in]  Generation of the oid which should be dereferenced.
  ** \param pObj       [out] If the oid belongs to a fix sized object, then after the call
  **                         pObj is pointing to the dereferenced object.
  ** \param byteSize   [out] If the oid belongs to a fix sized object, then this variable
  **                         contains the size of the object in bytes. 
  ** \param varOid     [out] If the oid belongs to a variable sized object, then the oid of
  **                         the object is returned (as OmsVarOid).
  **
  ** \return true,  if an object for the specified oid was found,
  **         false, otherwise.
  */
  bool        omsDerefUntypedOid(unsigned int      page,  
                                 unsigned short    offset,  
                                 unsigned short    generation,
                                 OmsAbstractObject *&pObj, 
                                 int               &byteSize,
                                 OmsVarOid         &varOid);

  /*--------------------------------------------------------------------------*/
  /// Dump OMS state.
  /*!
  ** Writes the OIDs for all instances from the current context cache to the liveCache trace 
  ** area. The instance representation in the trace is determined by the implementation of the 
  ** virtual omsDump method of the persistent object. 
  ** The trace area can be evaluated with the xkernprot tool.
  */
  void          omsDump();

  /*--------------------------------------------------------------------------*/
  /// Dump stack trace into knldiag (slow!).
  static void    omsDumpStackTrace();

  /*--------------------------------------------------------------------------*/
  /// Save stack trace into pointer array.
  /*!
   * This method stores the stack trace to the current location in the buffer.
   * Maximum of given levels are stored in the buffer (i.e., the buffer must
   * have space to hold \c levels pointers).
   *
   * \param buffer buffer, where to store frame pointers,
   * \param levels how many levels at most to store.
   *
   * \return count of levels filled in.
   */
  static unsigned int omsSaveStackTrace(void **buffer, unsigned int levels);

  /*--------------------------------------------------------------------------*/
  /// Evaluate saved stack trace.
  /*!
   * Evaluate saved stack trace, by calling a callback for all levels in the
   * trace, innermost first.
   *
   * \param buffer buffer previously filled with omsSaveStackTrace,
   * \param levels how many levels to print out,
   * \param cb callback instance for processing single stack trace levels
   *  (called for each level in turn).
   */
  static void    omsEvalStackTrace(void **buffer, unsigned int levels,
    OmsStackTraceCallbackInterface *cb);

  /*--------------------------------------------------------------------------*/
  /// Exception handler for errors.
  /*!
  ** Defines a standard exception handler for DbpError type error events. Depending on the 
  ** error event, the application is provided with the associated SQL error code. When 
  ** omsExeptionHandler is executed, the current DB procedure is terminated immediately; in 
  ** other words, all the required clean-up tasks must be completed beforehand.
  */
  void           omsExceptionHandler(DbpError&);

  /*--------------------------------------------------------------------------*/
  /// Get object (diagnostic function, do not use directly).
  void        omsGetObj(const OmsObjectId& oid, unsigned char* cnsView, int& ObjLen, unsigned char* obj); 

  /*--------------------------------------------------------------------------*/
  /// Get OID information.
  /*!
  ** Provides information on the instance identified by oid
  **
  ** \param oid object-id of the object to get information for
  ** \param guid The GUID of the class to which the instance belongs.
  ** \param type Type of the corresponding object (normal, keyed, variable sized, array)
  ** \param sh Schema handle of the schema in which the instance is stored.
  ** \param schemaName Name of the schema in which the instance is stored. The buffer 
  **        for the name must be provided by the invoker.
  ** \param schemaNameBufSize Size of the buffer for the schema name in bytes.
  ** \param className Class name of the class to which the instance belongs. The 
  **        buffer for the name must be provided by the invoker.
  ** \param classNameBufSize Size of the buffer for the class name in bytes.
  ** \param cno Class-specific number of the container in which the instance is stored.
  ** 
  ** \return 0 if the oid corresponds to an existing object,
  **        -1 if no object for the given oid was found.
  **
  ** \attention The determination of the schemaName is quite expensive. Therefore if
  ** this information is not needed, the parameter schemaNameBufSize should be set equal
  ** zero. In this case the schema name is not returned; therefore the pointer to the buffer
  ** schemaName can be equal NULL.
  */
#if defined(OMS_IMPLEMENTATION) || !defined(SAPDB_UNICODE)
  int         omsOidInfo (const OmsObjectId& oid, ClassID& guid, OmsObjectType &type,
                          OmsSchemaHandle& sh, char* schemaName, int schemaNameBufsize, 
                          char* className, int classNameBufSize, OmsContainerNo& cno); 
#endif
#if defined(OMS_IMPLEMENTATION) || defined(SAPDB_UNICODE)
  int         omsOidInfo (const OmsObjectId& oid, ClassID& guid, OmsObjectType &type,
                          OmsSchemaHandle& sh, OmsTypeWyde* schemaName, int schemaNameBufSize, 
                          char* className, int classNameBufSize, OmsContainerNo& cno); 
#endif

  /*--------------------------------------------------------------------------*/
  /// Returns the stored error code.
  /*!
  ** Returns the stored error code.
  *
  * \return stored error code  
  *
  * \see OMS_Handle::omsStoreErrorCode
  *
  * \since PTS 1122839
  */
  long            omsGetStoredErrorCode() const;    

  /*--------------------------------------------------------------------------*/
  /// Print OID information into buffer (diagnostic).
  void        omsPrintOid (char* buf, size_t bufSize, const OmsObjectId oid);

  /*--------------------------------------------------------------------------*/
  /// Returns whether a provoked error has been occurred in the current execution of the com-routine
  /*
  ** If a testkernel is running it is possible to ask for exceptions and for bad-allocs
  ** to test the exception handling of the program. 
  ** \see OmsHandle::omsControlBadAllocExceptions, OmsHandle::omsControlCancelFlag
  ** and OmsHandle::omsControlDbpException
  ** This method can be used to ask whether in the current com-routine such a provoked
  ** exception resp. badAlloc has been occurred. With this information it is possible 
  ** to handle such errors in a special manner e.g. do not write them into the knldiag,...
  **
  ** If a normal kernel is running, then \c false is returned for 'exception' and 'badAlloc'.
  **
  ** \param exception [out] \c true if there was a provoked exception; \c false otherwise 
  ** \param badAlloc  [out] \c true if there was a provoked bad-alloc-exception; \c false otherwise 
  */
  void        omsProvokedError(bool &exception, bool &badAlloc);

  /*--------------------------------------------------------------------------*/
  /// Raise error.
  /*!
  ** Terminates the current DB procedure immediately, and returns the error number msgno 
  ** and the error text errmsg to the application. The stack is not "unwound"; in other words, 
  ** all the clean-up tasks must have been completed before this method is executed.
  */
  void		    omsRaiseError (short msgno, const OmsTypeUTF8* errmsg);

  /*--------------------------------------------------------------------------*/
  /// Stores an given error code in the session for later recall.
  /*!
  ** Stores an given error code in the session for later recall.
  *
  * \param errorCode [in] error code to be stored  
  *
  * \see OMS_Handle::omsGetStoredErrorCode
  *
  * \since PTS 1122839
  */
  void            omsStoreErrorCode(long errorCode);    

  /*--------------------------------------------------------------------------*/
  /// Throw cancel exception.
  void omsThrowCancelledError() const; // PTS 1110812

  /*--------------------------------------------------------------------------*/
  /// Print OID into vtrace.
  void           omsTraceOid (const OmsObjectId& oid);

  /*--------------------------------------------------------------------------*/
  /// Get monitor data.
  bool        omsGetMethodCallMonitorInfo(OmsMonitorData& info) const;

  /*--------------------------------------------------------------------------*/
  /// Check whether there are corrupt var-objects and if some are found then remove them.
  void omsRemoveCorruptObjects(bool checkOnly, const ClassID &guid, OmsSchemaHandle schema, OmsContainerNo containerNo);
  //@}


  /*! \name Schema and Container Management
  ** \see \ref oms_SchemaAPI
  */
  //@{

  /*--------------------------------------------------------------------------*/
  /// Create container.
  /*!
  ** Creates a new container.
  **
  ** \param guid           [in] guid of the class for which a container should be created,
  ** \param schema         [in] schema in which the class should be registered,
  ** \param containerNo    [in] container-number in which the class should be registered,
  ** \param useCachedKeys  [in] default value for the cached-key-feature for this container
  ** \param partitionedKey [in] Should several B*-trees be used to store the keys?
  **        If true, several trees are used to store the keys. When inserting a new object
  **        the tree in which the object has to be stored is determined using a hash function. 
  **        This distribution on several trees should prevent bottlenecks caused by concurrent 
  **        operations on the tree. Currently the keys are always distributed among 16 trees. 
  **  \param withoutWait [in] If the container is already existing and locked by another
  **        transaction, then it is not waited for the lock and the method returns imediately
  **        with an error code.      
  **
  **  \return  1  the container has been newly created in the kernel (if it was already existing  
  **           0  an error occurred
  **          -1  if there is already a container existing and this container is locked by another transaction.
  **              As the parameer 'withoutWait' was set to true, it was not waited for the lock, and the
  **              operation of the current transaction was not completed.
  **
  ** \attention
  ** Iterating on objects which are stored with partitioned keys might lead to performance degradation,
  ** as the different trees must be merged during the iteration. Partitioned keys are even worse if
  ** the running direction of the iterator is changed during a single run.
  */
  int        omsCreateContainer(ClassID          guid, 
                                OmsSchemaHandle  schema, 
                                OmsContainerNo   containerNo,
                                bool             useCachedKeys=false,
                                bool             partitionedKeys=false,
                                bool             noWait=false);

  /*--------------------------------------------------------------------------*/
  /// Create schema.
  /*!
  ** Creates a schema and stores it in the form of meta data in the liveCache catalog under the 
  ** name specified by SchemaName. The SchemaName must be unique; in other words, a 
  ** schema with the name specified must not exist when the method is invoked. The creation 
  ** of a schema is not subject to the transaction concept, but is executed in a separate 
  ** liveCache session that is terminated implicitly with a 'commit'. For this reason, it is not 
  ** possible to remove the schema created by resetting the transaction. 
  ** omsCreateSchema provides a 'schema handle' that is used in the OMS to identify the 
  ** schema created. 
  ** The liveCache implicitly provides the OMS schema with the 
  ** OMS_DEFAULT_SCHEMA_HANDLE OmsSchemaHandle . 
  ** You can get information about schemas by selecting the table DOMAIN.SCHEMAS.
  */
#if defined(OMS_IMPLEMENTATION) || !defined(SAPDB_UNICODE)
  OmsSchemaHandle omsCreateSchema(const char* SchemaName);
#endif
#if defined(OMS_IMPLEMENTATION) || defined(SAPDB_UNICODE)
  OmsSchemaHandle omsCreateSchema(const OmsTypeWyde* SchemaName);
#endif

  /*--------------------------------------------------------------------------*/
  /// Drop schema.
  /*!
  ** Deletes the schema identified by handle, along with all of its containers, and the 
  ** persistent objects stored in them. The deletion of a schema is not subject to the 
  ** transaction concept, but is executed in a separate liveCache session that is terminated 
  ** implicitly by means of 'commit'. For this reason, the action cannot be reversed by 
  ** resetting the transaction.
  ** The invoker of omsDropSchema must ensure that there is no context that still requires 
  ** access to the data in the schema container. If a deleted schema is accessed by mistake, the 
  ** consequences cannot be predicted.
  */
  void        omsDropSchema(OmsSchemaHandle);

  /*--------------------------------------------------------------------------*/
  /// Drop container.
  /*!
  ** Deletes the container with the cno number from the schema identified by schema, as well 
  ** as all the persistent objects stored in the container. 
  ** The deletion of a container is not subject to the transaction concept, but is executed in a 
  ** separate liveCache session that is terminated implicitly with a 'commit'. For this reason, 
  ** the action cannot be reversed by resetting the transaction.
  ** The invoker of omsDropContainer must ensure that there is no context that still requires 
  ** access to the data in the deleted container. If the deleted container is accessed by mistake, 
  ** the consequences cannot be predicted.
  */
  void        omsDropContainer(ClassID guid, OmsSchemaHandle sh, OmsContainerNo cn);

  /*--------------------------------------------------------------------------*/
  /// Check container existence.
  /*! Checks whether the container corresponding to the given class-id, schema-handle and
  **  container number exists.
  **
  ** \param guid          [in] Identification of the object class
  ** \param schema        [in] Schema handle of the schema in which the instance is stored.
  ** \param containerNo   [in] Number of the container in which the instance is stored.
  **
  ** \return \c true, if the container exists and \c false otherwise.
  **
  ** \attention If the class is not already registered in the session, then an exception with
  ** the error code -28003 ('unknown-guid') will be thrown
  */
  bool        omsExistsContainer(ClassID guid, OmsSchemaHandle schema, OmsContainerNo containerNo); // PTS 1110334

  /*--------------------------------------------------------------------------*/
  /// Get container number for given OID.
  /*!
  ** Provides the number of the container in which the persistent instance identified by oid is 
  ** stored. If this does not exist in the current consistent view, an error of the type DbpError 
  ** is thrown.
  */
  OmsContainerNo  omsGetContainerNo (const OmsObjectId&);

  /*--------------------------------------------------------------------------*/
  /// Get schema handle for given OID.
  /*!
  ** Provides the schema handle to which the container is assigned in which the persistent 
  ** instance identified by oid is stored. If this does not exist in the in the current consistent 
  ** view, an error of the type DbpError is thrown.
  */
  OmsSchemaHandle omsGetSchemaHandle(const OmsObjectId&);

  /*--------------------------------------------------------------------------*/
  /// Get schema handle for given schema name.
  /*!
  ** Provides the handle of the schema identified by schemaName. If this does not exist, an 
  ** error of the type DbpError is thrown.
  */
#if defined(OMS_IMPLEMENTATION) || !defined(SAPDB_UNICODE)
  OmsSchemaHandle omsGetSchemaHandle(const char* SchemaName) const;
#endif
#if defined(OMS_IMPLEMENTATION) || defined(SAPDB_UNICODE)
  OmsSchemaHandle omsGetSchemaHandle(const OmsTypeWyde* SchemaName) const;
#endif

  /*--------------------------------------------------------------------------*/
  /// Get schema name for given schema handle.
  /*!
  ** Provides the schema name of the schema identified by handle. The name is stored as a 
  ** null-terminated string in the buffer buf provided by the invoker, the size of which is 
  ** specified by means of the buflen parameter. If the buffer is too small for the schema 
  ** name, the error DbpError(-28529) is thrown.
  */
#if defined(OMS_IMPLEMENTATION) || !defined(SAPDB_UNICODE)
  void        omsGetSchemaName(OmsSchemaHandle,char*,int) const;
#endif
#if defined(OMS_IMPLEMENTATION) || defined(SAPDB_UNICODE)
  void        omsGetSchemaName(OmsSchemaHandle,OmsTypeWyde*,int) const;
#endif

  /*--------------------------------------------------------------------------*/
  /// Check whether a class is registered 
  /*!
  ** Checks whether the class with the given id is registered in the current session
  **
  ** \param guid   [in] Identification of the object class
  **
  ** \return \c true, if the specified class is already registered; 
  ** otherwise, \c false.
  */
  bool        omsIsRegistered(ClassID guid) const;

  /*--------------------------------------------------------------------------*/

  void omsRegClass(ClassID            guid,
                   const char        *pClassName,
                   size_t             objectSize,
                   const OMS_KeyDesc &keyDesc,
                   ClassID            baseGuid,
                   OmsAbstractObject *pObj);

  /*--------------------------------------------------------------------------*/

  void omsRegVarClass(ClassID      guid, 
                      const char  *pClassName);

  /*--------------------------------------------------------------------------*/

  void omsRegArrayClass(ClassID            guid, 
                        const char        *pClassName,
                        size_t             objectSize,
                        OmsAbstractObject *pObj);  

  /*--------------------------------------------------------------------------*/
  /// Set cached keys usage for a container.
  /*!
  ** Set cached key feature for the specified class, schema, and container either
  ** on or off. 
  **
  ** Cached keys enable the key access to objects, stored in the local oms-cache,
  ** without determining the oid of the corresponding object first. As the determination
  ** of the oid is done in the kernel, cached keys reduce the communication between
  ** the oms-layer and the kernel and thereby speed up the performance. As the
  ** determination can be skipped only if the object is already stored in the local
  ** cache, the performance gain is only achieved if there are at least two key accesses 
  ** to the same object within one transaction.
  ** Additional to this information cache misses (key accesses with keys for which there
  ** are no objects) are also stored, so that the next key access to such a key can 
  ** be performed without asking the kernel.
  **
  ** As the administration of cached keys cause a small overhead, this feature is switched
  ** off by default and it had to be switched on if explicitely. If it had been switched on,
  ** it remains active until the container is dropped or it is switched off again.
  **
  ** Because of the overhead it is recommended to switch on this feature only if it is known,
  ** that within a transaction objects with the same key are accessed several times.
  **
  ** \param on if true the cached key feature is switched on, otherwise off
  ** \param guid guid of the persistent class
  ** \param sh schema handle
  ** \param cno container number
  ** \return true if the current state has been changed; otherwise false (this might occur
  **         e.g. if the object is not a keyed object, or if the call with the specified
  **         value for 'on' would not change the current state)
  **
  ** \since PTS 1117571
  */
  bool        omsSetCachedKeys(bool on, ClassID guid, OmsSchemaHandle sh, OmsContainerNo cno);  
  //@}


  /*! \name Allocator interface
  ** \see \ref oms_AllocatorAPI
  */
  //@{
  /*--------------------------------------------------------------------------*/
  /// Return reference to the COM routine-specific memory allocator.
  OmsAllocatorInterface &omsGetCOMRoutineAllocator();

  /*--------------------------------------------------------------------------*/
  /// Return reference to the global memory allocator.
  static OmsAllocatorInterface &omsGetGlobalAllocator();

  /*--------------------------------------------------------------------------*/
  /// Return reference to a named global memory allocator.
  static OmsAllocatorInterface &omsGetGlobalAllocator(const char *name);

  /*--------------------------------------------------------------------------*/
  /// Return reference to the session-specific memory allocator.
  OmsAllocatorInterface &omsGetSessionAllocator();

  /*--------------------------------------------------------------------------*/
  /// Return reference to the transaction-specific memory allocator.
  OmsAllocatorInterface &omsGetTransactionAllocator();
  //@}

  /*--------------------------------------------------------------------------*/
  /// Register the application area.
  void omsRegisterArea(OmsTypeInt4 areaId, const char *name);
  //@}

  /*--------------------------------------------------------------------------*/
  /// Perform memory barrier serializing operation.
  void omsMemoryBarrier();
  //@}

  /*--------------------------------------------------------------------------*/
  /// 
#if defined(OMS_IMPLEMENTATION) || !defined(SAPDB_UNICODE)
  void OmsSetTransactionComment(const char *pComment); 
#endif
#if defined(OMS_IMPLEMENTATION) || defined(SAPDB_UNICODE)
  void OmsSetTransactionComment(const OmsTypeWyde *pComment ); 
#endif

  /*--------------------------------------------------------------------------*/
  /// Initialize session (internal function, do not use directly).
  void        omsInitSession (IUnknown** s);

  /*--------------------------------------------------------------------------*/
  /// Return pointer to some OMS handle belonging to the current session.
  static OmsHandle* omsGetOmsHandle();

  /*--------------------------------------------------------------------------*/
  /// Return the task-id of the own session.
  int         omsGetTaskId();     /* PTS 1128641 */

  /*--------------------------------------------------------------------------*/
  /// Call COM routine epilog callback.
  void        omsMethodCallEpilog(void* pSH);     /* PTS 1120794 */

  /*--------------------------------------------------------------------------*/
  /// Force rescheduling for long-running tasks.
  void        omsReschedule() const; /* PTS 1107849 */

  /*----- PTS 1132163, PG ----------------------------------------------------*/
  /// Set process sleeping.
  void        omsSleep(const OmsTypeUInt1) const;
 
  /*--------------------------------------------------------------------------*/
  /// Force session termination.
  /*!
  ** Destroys all component instances of the current liveCache session. 
  */
  void           omsTerminate();

  /*--------------------------------------------------------------------------*/
  /// Force session termination.
#if defined(OMS_IMPLEMENTATION) || !defined(SAPDB_UNICODE)
  void           omsTerminate(const char*); /* PTS 1109693 */
#endif
#if defined(OMS_IMPLEMENTATION) || defined(SAPDB_UNICODE)
  void           omsTerminate(const OmsTypeUTF8*);
#endif

  /*--------------------------------------------------------------------------*/
  /// Return HTML reference for given OID (simulator only).
  const char*    omsSimDumpGetHtmlRef(const OmsObjectId &oid);

  /// Clean up global structures after the initialization of a livecache in a all-in-one system.
  void        omsCleanUpAfterInitialization();

  /// Return name of currently running routine.
  const OmsTypeAscii7 * omsGetCurrentMethod() const; /* PTS 1135058, PG */


#if defined(SAPDB_LVCSIM_EXTRAS) || defined(OMS_IMPLEMENTATION)
  // liveCache Simulator extra functionality:

  /// Call dump of simulator state.
  void		      omsSimDump();
  /// Call dump of simulator state.
  void		      omsSimDump(bool version);
  /// Call simulator transaction commit.
  void          omsCommit();
  /// Call simulator transaction rollback.
  void          omsRollback();
#endif

#ifdef OMS_IMPLEMENTATION
protected :
  /// Dereference an object.
  OmsAbstractObject* omsDeRef    (const OmsObjectId& oid, ClassID guid, bool forUpd, bool doLock, bool shared=false);
  /// Dereference a keyed object.
  OmsAbstractObject* omsDeRefKey (unsigned char* key, ClassID guid, bool forUpd, bool doLock, OmsSchemaHandle, OmsContainerNo, bool shared=false);

public:
  // OMS implementation-specific functions:
  /// Register container info in session in container register method.
  OMS_ContainerEntry*  omsAddContainerInfo (OMS_ContainerEntry* ContainerInfo);
  /// Attach default context to this session.
  HRESULT     omsAttachDefaultContext();
  /// Release default context.
  HRESULT     omsReleaseDefaultContext();
  /// Clean up the session after crash.
  void        omsCleanUpAfterCrash();
  /// Throw exception if OID empty.
  inline void omsIfNilOidThrowError(OmsHandle* h, const OmsObjectId& oid, const char* method,
                                    const char* pFile, unsigned int line);
  /// Get monitor data.
  void        omsGetMonitorInfo (tgg01_COMMonitorTransferBuf* lpMonInfo);
  /// Release object from cache unconditionally.
  void        omsReleaseUnconditional( const OmsObjectId& ); // not exported in livecache.h
  /// Commit.
  short       omsCommitHandling();
  /// Monitor lock.
  void        omsMonitorLock();   // PTS 1112275
  /// Monitor unlock.
  void        omsMonitorUnlock(); // PTS 1112275
  /// Rollback.
  void        omsRollbackHandling (bool release, bool ignoreErrors);
  /// Validate callback.
  void        omsValidate();
  /// Register monitor.
  void        omsRegMonitor     (void* pMon); 
  /// Cleaning-up after the trans-end (commit/rollback) in the kernel.
  void        omsTransEndEpilog(bool isCommit, bool release, int &phase); 
  /// Trace message.
  void        omsTrace ( const char* tracemsg, int TraceMsgLen);
  /// Trace message.
  void        omsTracef( const char*fmt, ... );
  /// Unlock an object.
  void        omsUnlock( const OmsObjectId& ); 

  bool CheckSession();
  void SetSessionCorrupt();

  /// Session end handling.
  void SessionEnd();
  /// Convert identifier to wyde.
  void IdentifierToWyde(const char* pSource, OmsTypeWyde* pDest) const; 
  /// Convert wyde to normal chars.
  void WydeToChar(const OmsTypeWyde* pSource, char* pDest, int destSize, const char* errorMsg) const;
  /// Print trace info about commit phase.
  void TraceCommitPhase(int);

  /// Set name of currently running routine.
  void omsSetCurrentMethod(unsigned char * name); /* PTS 1135058, PG */
  /// Re-set name of currently running routine.
  void omsResetCurrentMethod();                   /* PTS 1135058, PG */
#endif

private:
  /// Copy not allowed.
  OmsHandle (const OmsHandle&);             // no copy
  /// Assignment not allowed.
  OmsHandle& operator= (const OmsHandle&r); // no assignment

  void CopyVersionInfo (OMS_Context *pContext, OmsVersionInfo &info, 
                        OmsTypeWyde *pDescBuf, int             bufLen) const;

  /// Underlying session sink.
  IliveCacheSink**      m_session; 
  /// Underlying monitor.
  OMS_COMMonitor*       m_monitor;
  /// Underlying session object.
  OMS_Session*          m_pSession;
};

#endif // __OMS_HANDLE_HPP
