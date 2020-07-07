/*!
 * \file    OMS_Object.hpp
 * \author  MarkusSi, PeterG, Roterring
 * \brief   Base classes for persistent objects
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


#ifndef __OMS_OBJECT_HPP
#define __OMS_OBJECT_HPP

#include "Oms/OMS_Handle.hpp"
#include "Oms/OMS_TypedOid.hpp"
#include "Oms/OMS_AbstractObject.hpp"
#include "Oms/OMS_ObjByClsIter.hpp"
#include "Oms/OMS_KeyRangeIter.hpp"
#include "Oms/OMS_DbpError.hpp"


#define LIVECACHE_OBJECT_TMPLPAR(T, Guid)	<T, Guid>
#define LIVECACHE_KEYEDOBJECT_TMPLPAR(T, key, Guid)	<T, key, Guid>
#define LIVECACHE_INHOBJECT_TMPLPAR(T, Base, Guid)	<T, Base, Guid>
#define LIVECACHE_INHKEYEDOBJECT_TMPLPAR(T, Base, key, Guid)	<T, Base, key, Guid>

#ifdef WIN32
// disable warning
#pragma warning(disable: 4624)
#endif

///////////////////////////////////////////////////////////////////////////////////////
////////                             OmsObjectBase                             ////////
///////////////////////////////////////////////////////////////////////////////////////

/*------------------------------------------------------------------------------------*/
/// Base template for persistent objects
/*!
** The OmsObjectBase<T> template defines the methods of a persistent C++ class T.
** Own Objects should not be derived directly from this class but from the templates
** OmsObject (for base classes) or from OmsInheritedObject (for inherited classes)
*/
template<class T2, class Base, ClassID Guid>
class OmsObjectBase : public Base
{
  friend class OmsHandle;
  friend class OmsOid<T2>;
public:
  typedef void* KEY; 

  /// New operator
  inline void* operator new ( size_t sz, OmsHandle&ah, OmsSchemaHandle, OmsContainerNo);
  inline void* operator new ( size_t sz, OmsHandle& ah, OmsAbstractObject::OmsRegEnum e);
  inline void* operator new ( size_t sz, OmsHandle& ah, OmsAbstractObject::OmsRegEnum e, size_t rsz);

  /// Delete operator
  void operator delete ( void* p, OmsHandle&ah, OmsSchemaHandle, OmsContainerNo) {}
  void operator delete ( void* p, OmsHandle& ah, OmsAbstractObject::OmsRegEnum e) {}
  void operator delete ( void* p, OmsHandle& ah, OmsAbstractObject::OmsRegEnum e, size_t rsz) {}

  /*------------------------------------------------------------------------------------*/
  /// Get object ID.
  /*!
  ** Provides the OID of the instance.
  */
  inline const OmsOid<T2> omsGetOid() const;

  /*------------------------------------------------------------------------------------*/
  /// Dereference the object for update
  /*!
  ** Notifies the OMS that changes to the instance are intended. As a result, the invoker 
  ** receives a pointer, by means of which the instance can be manipulated.  
  ** The Oms requires this note to store information in case the current subtransaction is 
  ** reset. 
  **
  ** \param h current handle
  ** \param doLock If true, then a lock is also requested for the instance.
  **
  ** \see \ref example_omsForUpdPtr 
  */
  inline T2* omsForUpdPtr(OmsHandle& h, bool doLock=true) const; 

  ///
  inline bool  omsHistoryInUse(OmsHandle&) const;

	///
  inline bool  omsHistoryInUse(OmsHandle&, bool ignoreVersions, OmsTypeUInt8& OldestTransNo) const;

  /*--------------------------------------------------------------------------------------*/
  /// Deletes all the instances from a class container.
  /*! 
  ** Deletes all the instances of the persistent class in the specified container and schema. 
  ** The method carries out this task considerably faster than a comparable iterator with 
  ** multiple omsDelete calls. Unlike with an iterator of this type, however, the virtual 
  ** omsCleanUp method is not executed here. 
  ** The method fails if a lock within the consistent view cannot be obtained. The application 
  ** must ensure, therefore, that exclusive access to the container is provided. An 
  ** OmsLockObject, for example, can be used for this purpose.
  **
  ** \param h handle,
  ** \param sh schema of the objects which should be deleted,
  ** \param ContainerNo  container number of the objects which should be deleted.
  **
  ** \see \ref example_omsDeleteAll
  */
  static void  omsDeleteAll ( OmsHandle &h, OmsSchemaHandle sh, OmsContainerNo ContainerNo )
  { OmsAbstractObject::omsDeleteAll( h, Guid, sh, ContainerNo ); }

  /*--------------------------------------------------------------------------------------*/
  ///  Provides an iterator for a sequential scan across all the instances of the class. 
  /*! 
  ** Provides an iterator, which yields all instances of T stored in the containers 
  ** identified by schema and containerNo.
  */
  static OmsObjByClsIter<T2> omsAllOids( OmsHandle&h, OmsSchemaHandle sh, OmsContainerNo ContainerNo, int maxBufferSize = 20)
  { 
    OmsObjByClsIterBase iter = OmsAbstractObject::omsAllOids( h, Guid, sh, ContainerNo, maxBufferSize );
    return *REINTERPRET_CAST (OmsObjByClsIter<T2>*, &iter);
  }

  /*--------------------------------------------------------------------------------------*/
  /// Checks whether object, specified by oid, belongs to an instance of type OmsObject<T>
  /*!
  ** Provides true if an instance of OmsObject<T> is identified by oid; otherwise, false.
  **
  ** \param h current handle
  ** \param oid oid to be checked
  ** \return true if object, specified by oid, belongs to current instance of OmsObject<T>
  */
  static bool omsOidIsInstanceOf(OmsHandle &h, const OmsExternalOid &oid) 
  { return T2::omsCheckOid(h, reinterpret_cast<const OmsObjectId&>(oid)); }

  /*--------------------------------------------------------------------------------------*/  
  /// .
  static bool omsCheckOid(OmsHandle &h, const OmsObjectId &id)
  { return h.omsCheckOid(id, Guid); }

  /*--------------------------------------------------------------------------------------*/
  /// Returns the guid of the corresponding class.
  static ClassID omsGetGuid()
  { return Guid; }

  /*--------------------------------------------------------------------------------------*/
  /// .
  static OmsObjectId omsCastOid(OmsHandle &h, const OmsObjectId &from)
  { return h.omsCastOid(Guid, from); }

protected:
  OmsObjectBase() {}
  template <class P1>
  OmsObjectBase(P1 p1) : Base(p1) {}
  template <class P1, class P2>
  OmsObjectBase(P1 p1, P2 p2) : Base(p1, p2) {}
  template <class P1, class P2, class P3>
  OmsObjectBase(P1 p1, P2 p2, P3 p3) : Base(p1, p2, p3) {}
  template <class P1, class P2, class P3, class P4>
  OmsObjectBase(P1 p1, P2 p2, P3 p3, P4 p4) : Base(p1, p2, p3, p4) {}

#if defined(WIN32) && !defined(BIT64)
#pragma warning(disable: 4624)  // for .Net
private:
#endif
  ~OmsObjectBase() {} // derived class must not be destructed

private:
  static const T2* omsDeRef( const OmsOid<T2>&aoid, OmsHandle&ah )   
  { 
    return (T2*)OmsAbstractObject::omsDeRef( aoid, ah, Guid );
  }
  static T2* omsDeRefForUpd( const OmsOid<T2>&aoid, OmsHandle&ah, bool doLock, bool shared=false )
  { 
    return (T2*)OmsAbstractObject::omsDeRefForUpd( aoid, ah, Guid, doLock, shared );
  }
  OmsObjectBase( const OmsObjectBase<T2, Base, Guid>& );   // no copy
  void operator= ( const OmsObjectBase<T2, Base, Guid>& ); // no assignment
};



///////////////////////////////////////////////////////////////////////////////////////
////////                               OmsObject                               ////////
///////////////////////////////////////////////////////////////////////////////////////

/*------------------------------------------------------------------------------------*/
/// Template for persistent base objects
/*!
** Any class becomes persistent when it derives itself from the OmsObject<T>
** template.
*/
template<class T2, ClassID Guid>
class OmsObject : public OmsObjectBase<T2, OmsAbstractObject, Guid>
{
public:
  /*--------------------------------------------------------------------------------------*/
  /// Registers the class in the OMS.
  /*!
  ** Before a container is accessed for the first time, it must be registered. Registration 
  ** defines the schema to which a container belongs and the size of the instances stored in it. 
  ** The method takes the properties of the class to which the instances belong from an 
  ** instance of the class, which is generated by means of a special new operator of the class. 
  ** To use this new operator, each persistent class requires a default constructor.  
  ** During registration, the liveCache checks whether the container already exists. If this is 
  ** not the case, it is generated implicitly. 
  ** More than one container can be created for the instances of a class. This is expedient if 
  ** the instances can be grouped according to different criteria, and these criteria are used to 
  ** execute scan operations. omsRegClass must then be invoked with the corresponding 
  ** container number for each container of the class. 
  ** Each time the liveCache is started, a container must be registered before it is accessed for 
  ** the first time. Multiple registrations of a container are not critical. 
  ** The macro OMS_REG_CLASS facilitates registering the class.
  **
  ** \param h current handle,
  ** \param className name of the class to be registered,
  ** \param size size of the object to be registered,
  ** \param pObj pointer to an instance of the class to be registered,
  ** \param sh schema in which the class should be registered,
  ** \param containerNo  number of the container in which the class should be registered.
  ** 
  ** \see \ref example_omsRegClass
  */
  static void omsRegClass( 
    OmsHandle      &h, 
    const char     *className, 
    size_t          size, 
    OmsObjectBase<T2,OmsAbstractObject,Guid>* pObj, 
    OmsSchemaHandle sh, 
    OmsContainerNo  containerNo)
  { 
    OmsAbstractObject::omsRegClass( h, Guid, className, size, sizeof(T2), NULL, pObj, sh, containerNo ); 
  }

  static void omsRegClass( 
    OmsHandle      &handle, 
    const char     *pClassName, 
    OmsObjectBase<T2,OmsAbstractObject,Guid>* pObj,
    ClassID         baseGuid = 0)   // only for old style inheritence
  { 
    OmsAbstractObject::omsRegClass( handle, Guid, pClassName, sizeof(T2), /*keySize=*/0,
                       baseGuid, pObj); 
  }


  /*--------------------------------------------------------------------------------------*/
  /// Registers a derived class in the OMS. (Only for old-style registration of pseudo inheritance)
  /*!
  ** Formerly real derivations in the C++ sense could not be used when persistent classes are defined. 
  ** Each persistent class must have exactly one virtual method table. This means that 
  ** multiple inheritance is excluded. 
  ** To allow the advantages of inheritance to be leveraged despite this, the OMS offers a 
  ** method for simulating inheritance with the help of templates and registration. This 
  ** variant of omsRegClass is used to inform the OMS that the class to be registered must be 
  ** understood as a derivation of the class identified by BaseClassGuid. 
  ** 
  ** The macro OMS_REG_CLASS_INH facilitates this type of registration.
  **
  ** \param h current handle,
  ** \param className name of the class to be registered,
  ** \param size size of the object to be registered,
  ** \param baseClass a reference to the class which should be used as base,
  ** \param pObj pointer to an instance of the class to be registered,
  ** \param sh schema in which the class should be registered,
  ** \param containerNo  number of the container in which the class should be registered.
  **
  ** \attention As real inheritance is now possible, this method should not be used any longer.
  ** See \ref omsInheritance for an example of how to implement inheritance.
  */
  static void omsRegClass( 
    OmsHandle       &h, 
    const char      *className, 
    size_t           size, 
    ClassID baseClass,
    OmsObjectBase<T2,OmsAbstractObject,Guid>* pObj, 
    OmsSchemaHandle  sh, 
    OmsContainerNo   containerNo)
  { 
    OmsAbstractObject::omsRegClass( h, Guid, className, size, sizeof(T2), baseClass, pObj, sh, containerNo ); 
  }


  #ifdef	SAPDB_LVCSIM_EXTRAS
/// /name Extras for liveCache simulator
//@{
#ifdef	SAPDB_LVCSIM_DEPRECATED
  /// Registers the class in the OMS.
  /*! 
  ** This method is implemented only because of backwards compatibility of the simulator. 
  ** A description of the parameters can be found OmsObject::omsRegClass 
  */ 
  static inline void omsRegClass(OmsHandle&h, size_t sz, const char*cls_name, const OmsAbstractObject*pobj, OmsSchemaHandle sh, OmsContainerNo cno )
  { OmsAbstractObject::omsRegClass(h, Guid, sz, sizeof(T2), cls_name, 0, pobj, sh, cno ); }
  
  /// Registers a derived class in the OMS. (Only for old-style registration of pseudo inheritance)
  /*! 
  ** As real inheritance is now supported, this method should be used no longer.
  ** This method is implemented only because of backwards compatibility of the simulator. 
  ** Please see \ref omsInheritance for an example of how to implement inheritance.
  */
  static inline void omsRegClass(OmsHandle&h, size_t sz, const char*cls_name, ClassID base_class, const OmsAbstractObject*pobj, OmsSchemaHandle sh, OmsContainerNo cno )
  { OmsAbstractObject::omsRegClass(h, Guid, sz, sizeof(T2), cls_name, base_class, pobj, sh, cno ); }
#endif
//@}
#endif
};


template< class T2, ClassID Guid >
class OmsArrayHeadObject : public OmsObjectBase<T2, OmsAbstractObject, ARRAY_HEAD_GUID( Guid ) >
{
public:

  /*--------------------------------------------------------------------------------------*/
  /// Registers the class in the OMS.
  /*!
  ** Before a container is accessed for the first time, it must be registered. Registration 
  ** defines the schema to which a container belongs and the size of the instances stored in it. 
  ** The method takes the properties of the class to which the instances belong from an 
  ** instance of the class, which is generated by means of a special new operator of the class. 
  ** To use this new operator, each persistent class requires a default constructor.  
  ** During registration, the liveCache checks whether the container already exists. If this is 
  ** not the case, it is generated implicitly. 
  ** More than one container can be created for the instances of a class. This is expedient if 
  ** the instances can be grouped according to different criteria, and these criteria are used to 
  ** execute scan operations. omsRegClass must then be invoked with the corresponding 
  ** container number for each container of the class. 
  ** Each time the liveCache is started, a container must be registered before it is accessed for 
  ** the first time. Multiple registrations of a container are not critical. 
  ** The macro OMS_REG_CLASS facilitates registering the class.
  **
  ** \param h current handle,
  ** \param className name of the class to be registered,
  ** \param size size of the object to be registered,
  ** \param pObj pointer to an instance of the class to be registered,
  ** \param sh schema in which the class should be registered,
  ** \param containerNo  number of the container in which the class should be registered.
  ** 
  ** \see \ref example_omsRegClass
  */
  static void omsRegClass( 
    OmsHandle      &h, 
    const char     *className, 
    size_t          size, 
    OmsObjectBase<T2,OmsAbstractObject, ARRAY_HEAD_GUID( Guid )>* pObj, 
    OmsSchemaHandle sh, 
    OmsContainerNo  containerNo)
  { 
    OmsAbstractObject::omsRegClass( h,  ARRAY_HEAD_GUID( Guid ), className, size, sizeof(T2), NULL, pObj, sh, containerNo ); 
  }

  /*--------------------------------------------------------------------------------------*/
  /// Registers a derived class in the OMS. (Only for old-style registration of pseudo inheritance)
  /*!
  ** Formerly real derivations in the C++ sense could not be used when persistent classes are defined. 
  ** Each persistent class must have exactly one virtual method table. This means that 
  ** multiple inheritance is excluded. 
  ** To allow the advantages of inheritance to be leveraged despite this, the OMS offers a 
  ** method for simulating inheritance with the help of templates and registration. This 
  ** variant of omsRegClass is used to inform the OMS that the class to be registered must be 
  ** understood as a derivation of the class identified by BaseClassGuid. 
  ** 
  ** The macro OMS_REG_CLASS_INH facilitates this type of registration.
  **
  ** \param h current handle,
  ** \param className name of the class to be registered,
  ** \param size size of the object to be registered,
  ** \param baseClass a reference to the class which should be used as base,
  ** \param pObj pointer to an instance of the class to be registered,
  ** \param sh schema in which the class should be registered,
  ** \param containerNo  number of the container in which the class should be registered.
  **
  ** \attention As real inheritance is now possible, this method should not be used any longer.
  ** See \ref omsInheritance for an example of how to implement inheritance.
  */
  static void omsRegClass( 
    OmsHandle       &h, 
    const char      *className, 
    size_t           size, 
    ClassID baseClass,
    OmsObjectBase<T2,OmsAbstractObject, ARRAY_HEAD_GUID( Guid )>* pObj, 
    OmsSchemaHandle  sh, 
    OmsContainerNo   containerNo)
  { 
    OmsAbstractObject::omsRegClass( h,  ARRAY_HEAD_GUID( Guid ), className, size, sizeof(T2), baseClass, pObj, sh, containerNo ); 
  }

  static void omsRegClass( 
    OmsHandle      &handle, 
    const char     *pClassName, 
    OmsObjectBase<T2, OmsAbstractObject, ARRAY_HEAD_GUID( Guid )>* pObj,
    ClassID         baseGuid = 0)   // only for old style inheritence
  { 
    OmsAbstractObject::omsRegClass( handle,  ARRAY_HEAD_GUID( Guid ), pClassName, sizeof(T2), /*keySize=*/0,
      baseGuid, pObj); 
  }

#ifdef	SAPDB_LVCSIM_EXTRAS
  /// /name Extras for liveCache simulator
  //@{
#ifdef	SAPDB_LVCSIM_DEPRECATED
  /// Registers the class in the OMS.
  /*! 
  ** This method is implemented only because of backwards compatibility of the simulator. 
  ** A description of the parameters can be found OmsObject::omsRegClass 
  */ 
  static inline void omsRegClass(OmsHandle&h, size_t sz, const char*cls_name, const OmsAbstractObject*pobj, OmsSchemaHandle sh, OmsContainerNo cno )
  { 
    OmsAbstractObject::omsRegClass(h,  ARRAY_HEAD_GUID( Guid ), sz, sizeof(T2), cls_name, 0, pobj, sh, cno ); 
  }

  /// Registers a derived class in the OMS. (Only for old-style registration of pseudo inheritance)
  /*! 
  ** As real inheritance is now supported, this method should be used no longer.
  ** This method is implemented only because of backwards compatibility of the simulator. 
  ** Please see \ref omsInheritance for an example of how to implement inheritance.
  */
  static inline void omsRegClass(OmsHandle&h, size_t sz, const char*cls_name, ClassID base_class, const OmsAbstractObject*pobj, OmsSchemaHandle sh, OmsContainerNo cno )
  { 
    OmsAbstractObject::omsRegClass(h,  ARRAY_HEAD_GUID( Guid ), sz, sizeof(T2), cls_name, base_class, pobj, sh, cno ); 
  }
#endif
  //@}
#endif
};


///////////////////////////////////////////////////////////////////////////////////////
////////                          OmsInheritedObject                           ////////
///////////////////////////////////////////////////////////////////////////////////////

/*------------------------------------------------------------------------------------*/
/// Template for persistent inherited objects
/*!
** Any class becomes persistent when it derives itself from the OmsInheritedObject
** template. This template class can be used to create an object which is derived from 
** another persistent object.
** \attention Only single inheritance is allowed and the constructur of the base class
**    must not have more than 4 parameters.
**
** \ref omsInheritance
**
** \since 7.6 PTS 1124070
*/
template<class T2, class Base, ClassID Guid>
class OmsInheritedObject : public OmsObjectBase<T2, Base, Guid>
{
public:
  typedef typename Base::KEY BASEKEY; 

  OmsInheritedObject() {}
  template <class P1>
  OmsInheritedObject(P1 p1) : OmsObjectBase<T2,Base,Guid>(p1) {}
  template <class P1, class P2>
  OmsInheritedObject(P1 p1, P2 p2) : OmsObjectBase<T2,Base,Guid>(p1, p2) {}
  template <class P1, class P2, class P3>
  OmsInheritedObject(P1 p1, P2 p2, P3 p3) : OmsObjectBase<T2,Base,Guid>(p1, p2, p3) {}
  template <class P1, class P2, class P3, class P4>
  OmsInheritedObject(P1 p1, P2 p2, P3 p3, P4 p4) : OmsObjectBase<T2,Base,Guid>(p1, p2, p3, p4) {}

  /*--------------------------------------------------------------------------------------*/
  /// Registers a derived class in the OMS.
  /*!
  ** Registers a derived class in the OMS. The base class is specified as template parameter.
  **
  ** Before a container is accessed for the first time, it must be registered. Registration 
  ** defines the schema to which a container belongs and the size of the instances stored in it. 
  ** The method takes the properties of the class to which the instances belong from an 
  ** instance of the class, which is generated by means of a special new operator of the class. 
  ** To use this new operator, each persistent class requires a default constructor.  
  ** During registration, the liveCache checks whether the container already exists. If this is 
  ** not the case, it is generated implicitly. 
  ** More than one container can be created for the instances of a class. This is expedient if 
  ** the instances can be grouped according to different criteria, and these criteria are used to 
  ** execute scan operations. omsRegClass must then be invoked with the corresponding 
  ** container number for each container of the class. 
  ** Each time the liveCache is started, a container must be registered before it is accessed for 
  ** the first time. Multiple registrations of a container are not critical. 
  ** The macro OMS_REG_CLASS facilitates registering the class.
  **
  ** \param h current handle,
  ** \param className name of the class to be registered,
  ** \param size size of the object to be registered,
  ** \param pObj pointer to an instance of the class to be registered,
  ** \param sh schema in which the class should be registered,
  ** \param containerNo  number of the container in which the class should be registered.
  ** 
  ** \see \ref example_omsRegClass
  */
  static void omsRegClass( 
      OmsHandle      &h, 
      const char     *className, 
      size_t          size, 
      OmsObjectBase<T2,Base,Guid> *pObj, 
      OmsSchemaHandle sh, 
      OmsContainerNo  containerNo)
  { 
    OmsAbstractObject::omsRegClass( h, Guid, className, size, sizeof(T2), Base::omsGetGuid(), pObj, sh, containerNo ); 
  }

  static void omsRegClass( 
    OmsHandle      &handle, 
    const char     *pClassName, 
    OmsObjectBase<T2,Base,Guid> *pObj)
  { 
    OmsAbstractObject::omsRegClass( handle, Guid, pClassName, sizeof(T2), /*keySize=*/0, 
                       Base::omsGetGuid(), pObj); 
  }


#ifdef	SAPDB_LVCSIM_EXTRAS
/// /name Extras for liveCache simulator
//@{
#ifdef	SAPDB_LVCSIM_DEPRECATED
  /// Registers a derived class in the OMS. 
  /*!
  ** This method is implemented only because of backwards compatibility of the simulator. 
  ** Please see \ref OmsInheritedObject::omsRegClass for a description of the parameters.
  */
  static inline void omsRegClass(OmsHandle&h, size_t sz, const char*cls_name, const OmsAbstractObject*pobj, OmsSchemaHandle sh, OmsContainerNo cno )
  { OmsAbstractObject::omsRegClass(h, Guid, sz, sizeof(T2), cls_name, Base::omsGetGuid(), pobj, sh, cno ); }
#endif
//@}
#endif

private:
  /// Prevent calling omsGetKey on a non-keyed object 
  /*!
  ** This method is only defined to overwrite possible methods in parent classes.
  ** it is defined as private because it is not allowed to call this method on
  ** a non-keyed object. Even if a pointer to a keyed object is assigned to a
  ** non-keyed object and this method would be called using this pointer it is
  ** not allowed.
  */
  void omsGetKey(OmsHandle &h, BASEKEY &key) const   
  {
    throw DbpError(-28537 /*e_no_keyed_object*/, "OmsInheritedObject::omsGetKey");
  }  

  /// Prevent the virtual call of getKey on a non-keyed object 
  /*!
  ** This method is only defined to overwrite possible virtual calls. This is needed
  ** if a non-keyed object is derived from a keyed object and a pointer of the 
  ** keyed class is pointing to the derived object. If now omsGetKey is called on 
  ** the pointer, the virtual call would be directed to the non-keyed object and
  ** as there is no key an error is thrown.
  */
  virtual void getKey(OmsHandle &h, BASEKEY &key) const     
  { 
    throw DbpError(-28537 /*e_no_keyed_object*/, "OmsInheritedObject::getKey");
  }
};



///////////////////////////////////////////////////////////////////////////////////////
////////                          OmsKeyedObjectBase                           ////////
///////////////////////////////////////////////////////////////////////////////////////


/*------------------------------------------------------------------------------------*/
/// Base template for persistent objects with key
/*!
**  The OmsKeyedObjectBase template defines the methods of a persistent class 
**  \e Obj with a key of the type \e Key. It is derived from OmsObjectBase and, thus, inherits 
**  the basic methods of this object. 
**
** A key must be assigned to each instance of OmsKeyedObjectBase in the new operator. The 
** key value must have the key attribute; in other words, when the new operator is executed 
** within a container, a second instance of the class with the same key value must not exist.
**
** The class Key must implement the methods omsKeyToBinary and omsBinaryToKey. The objects of
** type OmsKeyedObjectBase are organized as a B*-tree in the liveCache, where the keys are sorted
** using the memcmp semantics. If this sorting is not desired, the method omsKeyToBinary allows
** a user-defined transformation of the key before it is inserted into the B*-tree.  
** Accordingly the method omsBinaryToKey allows the transformation back from the internal
** representation of the key into the external format.
**
** \see \ref oms_OmsKeyedObjectAPI
** \see \ref example_omsBinaryToKey
** \see \ref example_omsKeyedObject
*/
template<class Obj, class Base, class Key, ClassID Guid>
class OmsKeyedObjectBase : public OmsObjectBase<Obj,Base,Guid>
{
public:
  typedef Key KEY; 

  /*------------------------------------------------------------------------------------*/
  /// Returns the key of the object
  /*!
  ** Provides the key of an instance of a persistent class derived from OmsKeyedObject.
  ** Because of a possible inheritance hierarchy the method calls the virtual method
  ** getKey.
  **
  ** \param h   [in] handle
  ** \param key [out] contains after the call the key of the object
  */
  void omsGetKey(OmsHandle &h, KEY &key) const
  { 
    getKey(h,key); 
  }

  /*------------------------------------------------------------------------------------*/
  /// Operator new
  /*!
  ** Defines the new operator for persistent classes with a key. The persistent object is 
  ** created in the container identified by schema and containerNo. The key is assigned when 
  ** the object is created and cannot be changed subsequently. The key must be unique in the 
  ** container; in other words, there must not be two instances with the same key at any time. 
  ** This key constraint is guaranteed by the liveCache. 
  ** The instance can be accessed by means of its OID as well as by the key. 
  */
  void* operator new ( size_t sz, OmsHandle&ah, const Key&akey ,OmsSchemaHandle sh, OmsContainerNo ContainerNo)
  { return (void*) OmsAbstractObject::omsNewKeyedObject( ah, Guid, (unsigned char*) &akey, sh, ContainerNo ); } 
  void* operator new ( size_t sz, OmsHandle& ah, OmsAbstractObject::OmsRegEnum e)
  { return (void*) OmsAbstractObject::omsNewRegistryObject( sz + sizeof(Key), ah, Guid); }
  
  /// Operator delete
  void operator delete (void* p, OmsHandle&ah, const Key&akey ,OmsSchemaHandle sh, OmsContainerNo ContainerNo) {}
  void operator delete (void* p, OmsHandle& ah, OmsAbstractObject::OmsRegEnum e) {}

#ifdef	SAPDB_LVCSIM_EXTRAS
  // extras for liveCache simulator
#ifdef	SAPDB_LVCSIM_DEPRECATED
  static inline void  omsRegClass(OmsHandle &ah, size_t sz, const char*cls_name, OmsAbstractObject*pobj, OmsSchemaHandle sh, OmsContainerNo cno)
  { OmsAbstractObject::omsRegClassAndKey(ah, Guid, sz, sizeof(Obj), sizeof(Key), cls_name, 0, pobj, sh, cno); }
  static inline void  omsRegClass(OmsHandle &ah, const char*cls_name, size_t sz, ClassID base_class, OmsAbstractObject*pobj, OmsSchemaHandle sh, OmsContainerNo cno)
  { OmsAbstractObject::omsRegClassAndKey(ah, Guid, sz, sizeof(Obj), sizeof(Key), cls_name, base_class, pobj, sh, cno); }
#endif
#endif

  /*------------------------------------------------------------------------------------*/
  /// Dereferences the instance with the specified key.
  /*!
  ** Copies an instance of a class derived from OmsKeyedObject (identified by the key) from 
  ** the container identified by schema and cno to the current context cache, and provides a 
  ** pointer back to this. If there is no instance identified by key in the consistent view in the 
  ** container, the NULL pointer is provided. The pointer provides only read access to the 
  ** instance; changes cannot be carried out. To allow changes to be carried out, the pointer 
  ** must be converted to an update pointer by invoking omsForUpdPtr.
  */
  static const Obj* omsKeyAccess( OmsHandle&h, const Key&key, OmsSchemaHandle shdl, OmsContainerNo cno )  
  { return (Obj*)OmsAbstractObject::omsDeRefKey( (unsigned char*)(&key), h, Guid, shdl, cno ); }
  
  /*------------------------------------------------------------------------------------*/
  /// Dereferences the instance with the specified key for update.
  /*!
  ** Copies an instance of a class derived from OmsKeyedObject (identified by the key) from 
  ** the container identified by schema and cno to the current context cache, and provides a 
  ** pointer back to this. If there is no instance identified by key in the consistent view in the 
  ** container, the NULL pointer is provided. The pointer can be used to apply changes.
  */
  static Obj* omsKeyAccessForUpd( OmsHandle&h, const Key&key, bool doLock, OmsSchemaHandle sh, OmsContainerNo cno)  
  { return (Obj*)OmsAbstractObject::omsDeRefKeyForUpd((unsigned char*)(&key), h, Guid, doLock, sh, cno, /*shared=*/false ); }
  
  /*------------------------------------------------------------------------------------*/
  /// Dereferences the instance with the specified key and aquire a shared lock.
  /*!
  ** Copies an instance of a class derived from OmsKeyedObject (identified by the key) from 
  ** the container identified by schema and cno to the current context cache, and provides a 
  ** pointer back to this. If there is no instance identified by key in the consistent view in the 
  ** container, the NULL pointer is provided. 
  */
  static Obj* omsKeyAccessLockShared( OmsHandle&h, const Key&key, OmsSchemaHandle sh, OmsContainerNo cno)  
  { return (Obj*)OmsAbstractObject::omsDeRefKeyForUpd((unsigned char*)(&key), h, Guid, /*doLock=*/true, sh, cno, /*shared=*/true ); }

  /*------------------------------------------------------------------------------------*/
  /// Creates a key-iterator and positions the iterator onto the smallest element 
  /*!
  ** Creates a keyiterator, which returns objects lying in the interval [startKey, stopKey]
  ** and which belongs to the given schema handle and container number.
  ** After the creation the iterator is positioned onto the smallest object within the
  ** interval.
  ** The boundaries of the interval are inclusive. 
  **
  ** \param h handle
  ** \param sh schema handle 
  ** \param ContainerNo container number
  ** \param lowerKey lower boundary of the interval
  ** \param upperKey upper boundary of the interval
  ** \param maxBufferSize Size of the local buffer which is used in the oms-layer to buffer
  **        oids. This buffer reduced the communication overhead between the oms-layer and
  **        the kernel and therefore it should be increased/decreased dependend on the cost
  **        of the communication between these two layers. The buffer is only used for objects 
  **        created in the kernel; so it is not relevant for objects created in a version. 
  */
  static OmsKeyRangeIter<Obj> omsFirstKey(
    OmsHandle&      h, 
    OmsSchemaHandle sh, 
    OmsContainerNo  ContainerNo,
    const Key&      lowerKey,
    const Key&      upperKey,
    int             maxBufferSize = 20)
  { 
    // Ensure alignment PTS 1124028
    //unsigned char binStartKey[sizeof(Key)];
    //unsigned char binStopKey [sizeof(Key)];
    long binStartKeyAlign[sizeof(Key)/sizeof(long) + 1];
    long binStopKeyAlign [sizeof(Key)/sizeof(long) + 1];
    unsigned char* binStartKey = reinterpret_cast<unsigned char*>(&binStartKeyAlign[0]);
    unsigned char* binStopKey  = reinterpret_cast<unsigned char*>(&binStopKeyAlign[0]);

    lowerKey.omsKeyToBinary(&binStartKey[0]);
    upperKey.omsKeyToBinary(&binStopKey[0]);
    OmsObjByKeyIterBase iter = h.omsCreateKeyRangeIter( Guid, sh, ContainerNo, 
       &binStartKey[0], &binStartKey[0], &binStopKey[0], OMS_ASCENDING, maxBufferSize);
    return *REINTERPRET_CAST (OmsKeyRangeIter<Obj>*, &iter);
  }

  /*------------------------------------------------------------------------------------*/
  /// Creates a key-iterator and positions the iterator onto the largest element 
  /*!
  ** Creates a keyiterator, which returns objects lying in the interval [startKey, stopKey]
  ** and which belongs to the given schema handle and container number.
  ** After the creation the iterator is positioned onto the smallest object within the
  ** interval.
  ** The boundaries of the interval are inclusive. 
  **
  ** \param h handle
  ** \param sh schema handle 
  ** \param ContainerNo container number
  ** \param lowerKey lower boundary of the interval
  ** \param upperKey upper boundary of the interval
  ** \param maxBufferSize Size of the local buffer which is used in the oms-layer to buffer
  **        oids. This buffer reduced the communication overhead between the oms-layer and
  **        the kernel and therefore it should be increased/decreased dependend on the cost
  **        of the communication between these two layers. The buffer is only used for objects 
  **        created in the kernel; so it is not relevant for objects created in a version. 
  **
  ** \since PTS 1119480
  */
  static OmsKeyRangeIter<Obj> omsLastKey( 
    OmsHandle&      h, 
    OmsSchemaHandle sh, 
    OmsContainerNo  ContainerNo,
    const Key&      lowerKey,
    const Key&      upperKey,
    int             maxBufferSize = 20)
  { 
    // Ensure alignment PTS 1124028
    //unsigned char binStartKey[sizeof(Key)];
    //unsigned char binStopKey [sizeof(Key)];
    long binStartKeyAlign[sizeof(Key)/sizeof(long) + 1];
    long binStopKeyAlign [sizeof(Key)/sizeof(long) + 1];
    unsigned char* binStartKey = reinterpret_cast<unsigned char*>(&binStartKeyAlign[0]);
    unsigned char* binStopKey  = reinterpret_cast<unsigned char*>(&binStopKeyAlign[0]);

    lowerKey.omsKeyToBinary(&binStartKey[0]);
    upperKey.omsKeyToBinary(&binStopKey[0]);
    OmsObjByKeyIterBase iter = h.omsCreateKeyRangeIter( Guid, sh, ContainerNo,
        &binStopKey[0], &binStartKey[0], &binStopKey[0], OMS_DESCENDING, maxBufferSize);
    return *REINTERPRET_CAST (OmsKeyRangeIter<Obj>*, &iter);
  }

protected:
  virtual void  omsKeyToBinary(const void* pKey, void* pDest) const 
  {
    REINTERPRET_CAST(const Key*, pKey)->omsKeyToBinary(pDest);
  }
  virtual void omsBinaryToKey(void* pSource, void* pKey) const 
  {
    REINTERPRET_CAST(Key*, pKey)->omsBinaryToKey(pSource);
  }

private:
  /*------------------------------------------------------------------------------------*/
  /// Virtual method which returns the key of the object
  /*!
  ** Provides the key of an instance of a persistent class.
  **
  ** \param h   [in] handle
  ** \param key [out] contains after the call the key of the object
  */
  virtual void getKey(OmsHandle &h, KEY &key) const
  {
    OmsAbstractObject::omsKey(h, reinterpret_cast<unsigned char*>(&key));
  }
};



///////////////////////////////////////////////////////////////////////////////////////
////////                            OmsKeyedObject                             ////////
///////////////////////////////////////////////////////////////////////////////////////


/*------------------------------------------------------------------------------------*/
/// Template for persistent keyed base objects
/*!
** Any class becomes persistent when it derives itself from the OmsKeyedObject
** template. 
*/
template<class Obj, class Key, ClassID Guid>
class OmsKeyedObject : public OmsKeyedObjectBase<Obj, OmsAbstractObject, Key, Guid>
{
public:
  /*------------------------------------------------------------------------------------*/
  /// Registers a class
  /*!
  ** Registers a container of a class derived by OmsKeyedObject. To do so, it uses the 
  ** omsRegClass method defined in the OmsHandle. The macro OMS_REG_CLASS facilitates 
  ** registration. 
  ** After the liveCache has been started, each container must be registered at least once by 
  ** means of OMS_REG_CLASS before an instance of the container is accessed for the first 
  ** time. Multiple calls of OMS_REG_CLASS are permissible.
  ** 
  ** \param ah   current handle,
  ** \param className name of the class to be registered,
  ** \param sz   size of the object to be registered,
  ** \param pobj pointer to an instance of the class to be registered,
  ** \param sh   schema in which the class should be registered,
  ** \param cno  number of the container in which the class should be registered.
  ** \param partitionedKey Should several B*-trees be used to store the keys?
  **        If true, several trees are used to store the keys. When inserting a new object
  **        the tree in which the object has to be stored is determined using a hash function. 
  **        This distribution on several trees should prevent bottlenecks caused by concurrent 
  **        operations on the tree. Currently the keys are always distributed among 16 trees. 
  **
  ** \attention
  ** Iterating on objects which are stored with partitioned keys might lead to performance degradation,
  ** as the different trees must be merged during the iteration. Partitioned keys are even worse if
  ** the running direction of the iterator is changed during a single run.
  */
  static void  omsRegClass(
    OmsHandle      &ah, 
    const char     *className, 
    size_t          sz, 
    OmsObjectBase<Obj,OmsAbstractObject,Guid>* pobj, 
    OmsSchemaHandle sh, 
    OmsContainerNo  cno,
    bool            partitionedKey = false)
  { 
    OmsAbstractObject::omsRegClassAndKey( ah, Guid, 
      className, sz, sizeof(Key), sizeof(Obj), NULL, pobj, sh, cno, partitionedKey ); 
  }

  static void omsRegClass( 
    OmsHandle      &handle, 
    const char     *pClassName, 
    OmsObjectBase<Obj,OmsAbstractObject,Guid> *pObj)
  { 
    OmsAbstractObject::omsRegClass( handle, Guid, pClassName, sizeof(Obj), sizeof(Key),
                       /*baseGuid=*/0, pObj); 
  }
};




///////////////////////////////////////////////////////////////////////////////////////
////////                       OmsInheritedKeyedObjectBase                     ////////
///////////////////////////////////////////////////////////////////////////////////////

/// Template class for persistent keyed inherited objects
/*!
** Template class for persistent keyed inherited objects. This class is only needed
** to allow check of correct key-access.
*/
template<class Obj, class Base, class Key, ClassID Guid>
class OmsInheritedKeyedObjectBase : public OmsKeyedObjectBase<Obj, Base, Key, Guid>
{
public:
  typedef typename Base::KEY BASEKEY;

  OmsInheritedKeyedObjectBase() {}
  template <class P1>
  OmsInheritedKeyedObjectBase(P1 p1) : OmsObjectBase<Obj,Base,Guid>(p1) {}
  template <class P1, class P2>
  OmsInheritedKeyedObjectBase(P1 p1, P2 p2) : OmsObjectBase<Obj,Base,Guid>(p1, p2) {}
  template <class P1, class P2, class P3>
  OmsInheritedKeyedObjectBase(P1 p1, P2 p2, P3 p3) : OmsObjectBase<Obj,Base,Guid>(p1, p2, p3) {}
  template <class P1, class P2, class P3, class P4>
  OmsInheritedKeyedObjectBase(P1 p1, P2 p2, P3 p3, P4 p4) : OmsObjectBase<Obj,Base,Guid>(p1, p2, p3, p4) {}

private:
  /// Prevent calling the virtual getKey method on the parent key.
  /*!
  ** Overwrites the virtual getKey method for the key of the base-class. This is needed
  ** in case, that a key class is derived from another key class. If in this case a pointer
  ** of the parent class is pointing to an object of the child class, it is not allowed
  ** to call the getKey on this pointer, as the key-definitions may differ.
  */
  virtual void getKey (OmsHandle &h, BASEKEY &key) const 
  {
    throw DbpError(-28538 /*e_wrong_key*/, "OmsInheritedKeyedObjectBase::getKey");

  }
}; 


///////////////////////////////////////////////////////////////////////////////////////
////////                       OmsInheritedKeyedObject                         ////////
///////////////////////////////////////////////////////////////////////////////////////

/*------------------------------------------------------------------------------------*/
/// Template for persistent keyed inherited objects
/*!
** Any keyed class becomes persistent when it derives itself from the OmsInheritedKeyedObject
** template. This template class can be used to create an object which is derived from 
** another persistent object.
** \attention Only single inheritance is allowed and the constructur or the base class
**    must not have more than 4 parameters.
**
** \ref omsInheritance
**
** \since 7.6 PTS 1124070
*/
template<class Obj, class Base, class Key, ClassID Guid>
class OmsInheritedKeyedObject : public OmsInheritedKeyedObjectBase<Obj, Base, Key, Guid>
{
public:
  typedef Key KEY;

  OmsInheritedKeyedObject() {}
  template <class P1>
  OmsInheritedKeyedObject(P1 p1) : OmsInheritedKeyedObjectBase<Obj,Base,Key,Guid>(p1) {}
  template <class P1, class P2>
  OmsInheritedKeyedObject(P1 p1, P2 p2) : OmsInheritedKeyedObjectBase<Obj,Base,Key,Guid>(p1, p2) {}
  template <class P1, class P2, class P3>
  OmsInheritedKeyedObject(P1 p1, P2 p2, P3 p3) : OmsInheritedKeyedObjectBase<Obj,Base,Key,Guid>(p1, p2, p3) {}
  template <class P1, class P2, class P3, class P4>
  OmsInheritedKeyedObject(P1 p1, P2 p2, P3 p3, P4 p4) : OmsInheritedKeyedObjectBase<Obj,Base,Key,Guid>(p1, p2, p3, p4) {}
  
  /*------------------------------------------------------------------------------------*/
  /// Registers an inherited class with key
  /*!
  ** Registers a container of a class with key which is derived from another persistent
  ** class (either with or without key).  The macro OMS_REG_CLASS facilitates
  ** the registration. 
  ** After the liveCache has been started, each container must be registered at least once by 
  ** means of OMS_REG_CLASS before an instance of the container is accessed for the first 
  ** time. Multiple calls of OMS_REG_CLASS are allowed.
  ** 
  ** \param ah   current handle,
  ** \param className name of the class to be registered,
  ** \param sz   size of the object to be registered,
  ** \param pobj pointer to an instance of the class to be registered,
  ** \param sh   schema in which the class should be registered,
  ** \param cno  number of the container in which the class should be registered.
  ** \param partitionedKey Should several B*-trees be used to store the keys?
  **        If true, several trees are used to store the keys. When inserting a new object
  **        the tree in which the object has to be stored is determined using a hash function. 
  **        This distribution on several trees should prevent bottlenecks caused by concurrent 
  **        operations on the tree. Currently the keys are always distributed among 16 trees. 
  **
  ** \attention
  ** Iterating on objects which are stored with partitioned keys might lead to performance degradation,
  ** as the different trees must be merged during the iteration. Partitioned keys are even worse if
  ** the running direction of the iterator is changed during a single run.
  */
  static void  omsRegClass(
    OmsHandle      &ah, 
    const char     *className, 
    size_t          sz, 
    OmsObjectBase<Obj,Base,Guid>* pobj, 
    OmsSchemaHandle sh, 
    OmsContainerNo  cno,
    bool            partitionedKey = false)
  { 
    OmsAbstractObject::omsRegClassAndKey( ah, Guid, 
      className, sz, sizeof(Key), sizeof(Obj), Base::omsGetGuid(), pobj, sh, cno, partitionedKey ); 
  }

  static void omsRegClass( 
    OmsHandle      &handle, 
    const char     *pClassName, 
    OmsObjectBase<Obj,Base,Guid> *pObj)
  { 
    OmsAbstractObject::omsRegClass( handle, Guid, pClassName, sizeof(Obj), sizeof(Key),
                       Base::omsGetGuid(), pObj); 
  }

  /*------------------------------------------------------------------------------------*/
  /// Returns the key of the object
  /*!
  ** Provides the key of an instance of a persistent keyed class.
  ** Because of a possible inheritance hierarchy the method calls the virtual method
  ** getKey.
  **
  ** \param h   [in] handle
  ** \param key [out] contains after the call the key of the object
  */
  void omsGetKey (OmsHandle &h, KEY &key) const
  { 
    getKey(h,key); 
  } 

private:
  /*------------------------------------------------------------------------------------*/
  /// Virtual method to read the key
  /*!
  ** Provides the key of an instance of a persistent keyed class.
  **
  ** \param h   [in] handle
  ** \param key [out] contains after the call the key of the object
  */
  virtual void getKey(OmsHandle &h, KEY &key) const 
  { 
    OmsAbstractObject::omsKey(h, reinterpret_cast<unsigned char*>(&key)); 
  }

friend class OmsKeyedObject<Obj, Key, Guid>;                    
friend class OmsInheritedKeyedObjectBase<Obj, Base, Key, Guid>;  
};






/*========================= Implementation of OmsObjectBase =========================*/

template<class T2, class Base, ClassID Guid>
inline 
const OmsOid<T2> OmsObjectBase<T2,Base,Guid>::omsGetOid() const 
{ 
  return reinterpret_cast<const OmsOid<T2>&>(Base::omsOid());
}

template<class T2, class Base, ClassID Guid>
inline
bool OmsObjectBase<T2,Base,Guid>::omsHistoryInUse(OmsHandle& h) const
{
  return OmsAbstractObject::omsHistoryInUse(h);
}

// *** new ***  PTS 1130354 FF
template<class T2, class Base, ClassID Guid>
inline
bool OmsObjectBase<T2,Base,Guid>::omsHistoryInUse(OmsHandle& h, bool ignoreVersions, OmsTypeUInt8& OldestTransNo) const
{
  return OmsAbstractObject::omsHistoryInUse(h, ignoreVersions, OldestTransNo);
}

template<class T2, class Base, ClassID Guid>
inline 
T2* OmsObjectBase<T2,Base,Guid>::omsForUpdPtr(OmsHandle& h, bool doLock) const
{
  return (T2*) OmsAbstractObject::omsForUpdPtr(h, doLock); 
}

template<class T2, class Base, ClassID Guid>
inline 
void* OmsObjectBase<T2, Base, Guid>::operator new ( size_t sz, OmsHandle&ah, OmsSchemaHandle sh, OmsContainerNo cno )
{
  return (void*) OmsAbstractObject::omsNewObject( ah, Guid, sh, cno );
}

template<class T2, class Base, ClassID Guid>
inline 
void* OmsObjectBase<T2,Base,Guid>::operator new ( size_t sz, OmsHandle& ah, OmsAbstractObject::OmsRegEnum e ) 
{
  return (void*) OmsAbstractObject::omsNewRegistryObject( sz, ah, Guid ); 
}

template<class T2, class Base, ClassID Guid>
inline 
void* OmsObjectBase<T2,Base,Guid>::operator new ( size_t sz, OmsHandle& ah, OmsAbstractObject::OmsRegEnum e, size_t rsz ) 
{
  return (void*) OmsAbstractObject::omsNewRegistryObject( rsz, ah, Guid ); 
}

#endif  // __OMS_OBJECT_HPP
