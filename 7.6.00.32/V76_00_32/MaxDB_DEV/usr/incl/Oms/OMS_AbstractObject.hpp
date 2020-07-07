/*!
 * \file    OMS_AbstractObject.hpp
 * \author  MarkusSi, PeterG, Roterring
 * \brief   Base class for all persistent objects
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

#ifndef __OMS_ABSTRACTOBJECT_HPP
#define __OMS_ABSTRACTOBJECT_HPP

#include "Oms/OMS_Defines.h"
#include "Oms/OMS_Oid.hpp"
#include "Oms/OMS_ObjByClsIterBase.hpp"

class OmsSimDumpCollector;
class OmsHandle;

/*!
 * \brief Base class for all persistent objects.
 *
 * This class is a base class for template class OmsObject and (indirect)
 * OmsKeyedObject.
 *
 * \see \ref oms_AbstractObjectAPI
 */
class AFX_EXT_CLASS OmsAbstractObject {

  friend class OmsArrayObjectBase;
  friend class OmsArrayObjectHandleBase;
  friend class OmsArrayDataIteratorBase;
  friend class OmsObjectContainer;
  friend class OmsHandle;
  friend class OMS_OidHash;
  friend class OmsObjByClsIterBase;
  friend class OMS_Context;
  friend class OMS_Session;

public:
  enum OmsRegEnum { USE_IN_REGISTRY_ONLY };

  /// Callback called just before delete.
  virtual void       omsCleanUp (OmsHandle&);

  /*----------------------------------------------------------------------------------*/
  /// Dump OID and flags into trace.
  /*!
  ** Defines how the instance is represented when the method OmsHandle::omsDump is 
  ** invoked. The "flags" variable contains OMS administration information on the instance 
  ** and should always be output. 
  **
  ** \param h current handle
  ** \param flags
  **
  ** \see \ref example_omsDump
  */
  virtual void       omsDump    (OmsHandle& h, const char* flags) const;

  /*----------------------------------------------------------------------------------*/
  /// Delete the object.
  /*!
  ** Deletes the instance. A lock for the instance must already have been granted to the 
  ** current transaction. The instance is removed from the liveCache as well as the current 
  ** OMS context cache.
  **
  ** \attention
  ** After omsDelete has been executed, the pointer to the instance of the object is 
  ** irrelevant. If the pointer is used to invoke other methods of the instance, 
  ** unforeseeable errors can occur in the OMS.
  **
  ** \param h current handle
  */
  void               omsDelete          (OmsHandle& h);

  /*----------------------------------------------------------------------------------*/
  /// Flush the object (write to LC base)
  void               omsFlush           (OmsHandle& h);

  /*----------------------------------------------------------------------------------*/ 
  /// Get schema the object belong to.
  OmsSchemaHandle    omsGetSchemaHandle (OmsHandle&) const;

  /*----------------------------------------------------------------------------------*/
  /// Get locked status.
  /*!
  ** Checks whether transaction holds a lock on the instance.
  **
  ** \param h current handle
  ** \param shared, check for exclusive or for shared lock?
  ** \return true, if open transaction has a lock for the instance; otherwise, false.
  */
  bool               omsIsLocked        (OmsHandle& h, bool shared=false) const;
  
  /*----------------------------------------------------------------------------------*/
  /// Lock the object for update or delete.
  /*! Requests an exclusive lock for the instance. The lock can only be granted if the 
  ** transaction-consistent view corresponds to the current "committed" state of the instance 
  ** in the liveCache. If this is not the case, an error of type OmsOutOfDate is thrown. 
  ** If the instance is locked by another transaction when the request is made in the 
  ** liveCache, the system waits for the release for the time set in the timeout value. 
  ** If the lock cannot be granted in this time, an error event of the type OmsLockTimeout 
  **occurs.  
  **
  ** \param h current handle
  */
  void               omsLock            (OmsHandle& h);

  /*----------------------------------------------------------------------------------*/
  /// Try to lock the object for update or delete.
  /*! Requests an exclusive lock for the instance. The lock can only be granted if the 
  ** transaction-consistent view corresponds to the current "committed" state of the instance 
  ** in the liveCache. If this is not the case, \c false is returned. 
  ** If the instance is locked by another transaction when the request is made in the 
  ** liveCache, the system waits for the release for the time set in the timeout value. 
  ** If the lock cannot be granted in this time, \c false is returned.
  **
  ** \param h current handle
  ** \return \c true if successfully locked, \c false otherwise.
  */
  bool               omsTryLock            (OmsHandle& h);

  /*----------------------------------------------------------------------------------*/
  /// Try to lock the object in shared mode.
  /*! Requests an shared lock for the instance. The lock can only be granted if the 
  ** transaction-consistent view corresponds to the current "committed" state of the instance 
  ** in the liveCache. If this is not the case, \c false is returned. 
  ** If the instance is locked by another transaction when the request is made in the 
  ** liveCache, the system waits for the release for the time set in the timeout value. 
  ** If the lock cannot be granted in this time, \c false is returned.
  **
  ** \param h current handle
  ** \return \c true if successfully locked, \c false otherwise.
  */
  bool               omsTryLockShared      (OmsHandle& h);

  /*----------------------------------------------------------------------------------*/
  /// Lock the object in shared mode.
  /*! Requests a shared lock for the instance. The lock can only be granted if the 
  ** transaction-consistent view corresponds to the current "committed" state of the instance 
  ** in the liveCache. If this is not the case, an error of type OmsOutOfDate is thrown. 
  ** If the instance is locked by another transaction in exclusive mode when the request is made 
  ** in the liveCache, the system waits for the release for the time set in the timeout value. 
  ** If the lock cannot be granted in this time, an error event of the type OmsLockTimeout 
  ** occurs.  
  **
  ** \param h current handle
  **
  ** \ since PTS 1130435
  */
  void               omsLockShared            (OmsHandle& h) const;

  /*----------------------------------------------------------------------------------*/
  /// Release the object from the cache.
  /*!
  ** Notifies the OMS that the instance can be removed from the current context cache. It 
  ** cannot be guaranteed, however, that the OMS will actually remove the instance 
  ** immediately, since changes to the object in a subtransaction, for example, must remain in 
  ** the OMS context cache longer.
  **  
  ** \attention
  ** After omsRelease has been executed, the pointer to the instance of the OmsObject is 
  ** irrelevant. If the pointer is used to call other methods of the instance, unforeseeable 
  ** errors can occur in the OMS.  
  **
  ** \param h current handle
  */
  void               omsRelease         (OmsHandle& h);

  /*----------------------------------------------------------------------------------*/
  /// Store changes in the object.
  /*! 
  ** Notifies the OMS that the instance is to be copied from the current context cache to the 
  ** liveCache. A lock for the instance must be granted to the current transaction beforehand. 
  ** The liveCache decides automatically when the instance is actually copied to the 
  ** liveCache. This will generally be at the end of the transaction.
  **
  ** \param h current handle
  **/
  void               omsStore           (OmsHandle& h);

  /*----------------------------------------------------------------------------------*/
  /// Callback to verify object consistency at store time.
  virtual void       omsVerify  (OmsHandle& h) const;

  /*----------------------------------------------------------------------------------*/
  /// Get object ID.
  const OmsObjectId omsGetOid() const;

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
  unsigned int omsGetObjectRevisionNumber() const;


protected:
  friend class OmsArrayObjectIteratorBase;

  /// Convert key for keyed object to platform-independent format.
  virtual void       omsKeyToBinary(const void* pKey, void* pDest) const;
  /// Convert key for keyed object to platform-dependent format.
  virtual void       omsBinaryToKey(void* pSource, void* pKey) const;

  OmsAbstractObject ();
  /// Get object ID.
  const OMS_ObjectId8& omsOid() const;
  /// Dereference the object for update.
  OmsAbstractObject* omsForUpdPtr(OmsHandle& h, bool doLock) const;
  /// Get history usage status.
  bool  omsHistoryInUse(OmsHandle&) const;
  /// Get history usage status.
  bool  omsHistoryInUse(OmsHandle&, bool ignoreVersions, OmsTypeUInt8& OldestTransNo) const;
  /// Return keyed object key.
  void omsKey(OmsHandle& Handle, unsigned char* key) const;
  /// Delete all objects in container.
  static void omsDeleteAll(OmsHandle& h, ClassID guid, OmsSchemaHandle, OmsContainerNo);
  /// Create new object.
  static OmsAbstractObject* omsNewObject (OmsHandle& Handle, ClassID guid, OmsSchemaHandle, OmsContainerNo, size_t arrayByteSize=0 );
  /// Create new object for container registration.
  static OmsAbstractObject* omsNewRegistryObject(size_t sz, OmsHandle& Handle, ClassID guid);
  /// Create new object with key.
  static OmsAbstractObject* omsNewKeyedObject (OmsHandle& Handle, ClassID guid, const unsigned char* key, OmsSchemaHandle, OmsContainerNo ); 
  /// Create iterator running over all objects in container.
  static OmsObjByClsIterBase omsAllOids( OmsHandle& h, ClassID guid, OmsSchemaHandle, OmsContainerNo, int maxBufferSize = 20);
  /*! Register a new container.  
  **  \attention old style. Should not be used any more
  */
  static void omsRegClass (
    OmsHandle& Handle, 
    ClassID Guid, 
    const char* ClassName, 
    size_t PersistentSize, 
    size_t ObjectSize, 
    ClassID baseClass, 
    OmsAbstractObject* Obj, 
    OmsSchemaHandle, 
    OmsContainerNo,
    size_t = 0);
  /*! Register a new container with key.
  **  \attention old style. Should not be used any more
  */
  static void omsRegClassAndKey (
    OmsHandle&, 
    ClassID, 
    const char* ClassName, 
    size_t PersistentSize, 
    size_t KeySize, 
    size_t ObjectSize, 
    ClassID baseClass, 
    OmsAbstractObject* Obj, 
    OmsSchemaHandle, 
    OmsContainerNo,
    bool partitionedKey = false); /* PTS 1109717 */

  /// Register a new class (either with (keysize>0) or without (keysize=0) key).
  static void omsRegClass(
    OmsHandle         &handle,
    ClassID            guid,
    const char        *pClassName,
    size_t             objectSize,
    size_t             keySize,
    ClassID            baseGuid,
    OmsAbstractObject *pObj);

  /// Register a new class for array objects.
  static void omsRegArrayClass(
    OmsHandle         &handle,
    ClassID            guid,
    const char        *pClassName,
    size_t             objectSize,
    OmsAbstractObject *pObj);

  /// Dereference an object.
  static const OmsAbstractObject* omsDeRef (const OmsObjectId& oid, OmsHandle& Handle, ClassID guid);
  /// Dereference an object for update.
  static OmsAbstractObject* omsDeRefForUpd (const OmsObjectId& oid, OmsHandle& Handle, ClassID guid, bool doLock, bool shared=false);
  /// Dereference a keyed object via key.
  static const OmsAbstractObject* omsDeRefKey (unsigned char* key, OmsHandle& Handle, ClassID guid, OmsSchemaHandle, OmsContainerNo);
  /// Dereference a keyed object via key for update.
  static OmsAbstractObject* omsDeRefKeyForUpd (unsigned char* key, OmsHandle& Handle, ClassID guid, bool doLock, OmsSchemaHandle, OmsContainerNo, bool shared=false);
private:
  /// Disallow copy.
  OmsAbstractObject(const OmsAbstractObject&);
  /// Disallow assignment.
  OmsAbstractObject& operator= (const OmsAbstractObject&r);

  // extras for liveCache simulator
public:
  /// Dump object state to simulator dump collector.
  virtual void omsSimDumpToHtmlShort(OmsHandle&, OmsSimDumpCollector&) const;
  /// Dump object state to simulator dump collector.
  virtual void omsSimDumpToHtmlLong(OmsHandle&, OmsSimDumpCollector&) const;
  /// Set object name for simulator dump.
  void omsSimSetObjectName(OmsHandle &h, const char *name);
  /// Get object name for simulator dump.
  const char* omsSimGetObjectName(OmsHandle &h) const;
  /// Make OID HTML reference for simulator dump.
  static const char *omsSimDumpMakeOidRef(OmsHandle &h, const OmsObjectId &oid, int flags = 3);
  /// Make OID HTML reference for simulator dump.
  static const char *omsSimDumpMakeOidRef(const OmsObjectId &oid, int flags = 3);

#ifdef	SAPDB_LVCSIM_EXTRAS
#ifdef	SAPDB_LVCSIM_DEPRECATED
public:
  /// Register a container.
  static void omsRegClass(OmsHandle &h, ClassID guid, size_t psoSize, size_t objSize, 
    const char*cls_name, ClassID base_class, const OmsAbstractObject*pobj, 
    OmsSchemaHandle sh, OmsContainerNo cno);
  /// Register a container for keyed objects.
  static void omsRegClassAndKey(OmsHandle &h, ClassID guid, size_t psoSize, 
    size_t objSize, size_t keySize, const char*cls_name, ClassID base_class, 
    const OmsAbstractObject*pobj, OmsSchemaHandle sh, OmsContainerNo cno);
#endif
#endif
};

#endif // __OMS_ABSTRACTOBJECT_HPP
