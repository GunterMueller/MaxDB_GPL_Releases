/*!
 * \file    OMS_ArrayObject.hpp
 * \author  MarkusSi
 * \brief   Array object.
 */

/*

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



*/

#ifndef __OMS_ARRAYOBJECT_HPP
#define __OMS_ARRAYOBJECT_HPP

#include "Oms/OMS_Object.hpp"
#include "Oms/OMS_Handle.hpp"
#include "Oms/OMS_ArrayObjectUtils.hpp"

static const size_t OMS_ARROBJ_OPTSIZECNT = 31;

// Forward declarations
class OmsAbstractObject;
class OMS_ContainerDirectory;
template <class T, int ClsGuid>
class OmsArrayObject;


//==========================================================================

/**
 * \brief Base for array objects.
 *
 * This class is the base for all array object classes.
 *
 * \see OmsArrayObject for more information.
 */
class AFX_EXT_CLASS OmsArrayObjectBase {
public:
  /*!
   * \return usable size (= size, which can be used to stored array-elements)
   * for given class category (1..31)
   */
  static size_t GetMaxByteSize(size_t category);

  /*!
   * \return class category (1..31) for given usable size
   */
  static short GetCategory(size_t byteSize);

  /*!
   * \return usable size (= size, which can be used to stored array-elements)
   * for category which corresponds to given abstract object
   */
  static size_t GetMaxByteSize(OmsHandle &h, OmsAbstractObject *pObj);

  static void ThrowInvalidParam(const char* pFile, unsigned int line);
  static void ThrowObjectLinkCorruption(const char* pFile, unsigned int line);
};


//==========================================================================

/**
 * \brief Base for array object iterator.
 *
 * Each array object size iterator is derived from this class.
 * This class provides common methods for all iterators and
 * base (untyped) methods.
 *
 * \see OmsArrayObjectIterator for more information.
 */
class AFX_EXT_CLASS OmsArrayObjectIteratorBase {
  friend class OMS_ContainerDirectory;

protected:
  OmsArrayObjectIteratorBase(OmsHandle &_h, int _guid, OmsSchemaHandle _sh, OmsContainerNo _cno)
    : m_h(_h), m_guid(_guid), m_sh(_sh), m_cno(_cno), m_category(0)
  {
    omsNext();
  }

public:
  /// Return true, iff there are some more registered sizes.
  inline operator bool() const
  {
    return m_category != 0;
  }

  /// Return true, iff there are no more registered sizes.
  inline bool operator!() const
  {
    return m_category == 0;
  }

  inline short omsGetCategory() const
  {
    return m_category;
  }

  /// Move to the next size class.
  void omsNext();

  /// Move to the next size class.
  inline void operator++()
  {
    omsNext();
  }

protected:
  /// Get iterator over current size class.
  OmsObjByClsIterBase omsAllOids(int maxBufferSize = 20);

  OmsHandle       &m_h;          ///< OMS handle
  int              m_guid;       ///< Object GUID
  OmsSchemaHandle  m_sh;         ///< Schema
  OmsContainerNo   m_cno;        ///< container number.
  short            m_category;   ///< Current category
};


//==========================================================================

/*!
 * \brief OmsArrayObject size iterator.
 *
 * This class enables iterating over all sizes in an array object container.
 * It is not possible to iterate over all objects in OmsArrayObject container
 * directly, since the objects are spread over more physical containers.
 * The same holds for some other functions, like omsDropContainer or
 * omsDeleteAll.
 *
 * To iterate over all objects in an OmsArrayObject container, use the
 * following construct:
 * \code
 *  OmsArrayObjectIterator<MyElement, MyGuid> iter =
 *    OmsArrayObject<MyElement, MyGuid>::omsGetSizesIterator(h, schema, cno);
 *  while (iter) {
 *    OmsObjByClsIter<OmsArrayObject<MyElement, MyGuid> > i = iter.omsAllOids();
 *    while (i) {
 *      // process the object
 *      ++i;
 *    }
 *    ++iter;
 *  }
 * \endcode
 *
 * \see OmsArrayObject
 * \since 7.4.3.30, 7.5.0.2
 */
template<class T, int ClsGuid, class ObjType = OmsArrayObject<T, ClsGuid> >
class OmsArrayObjectIterator : public OmsArrayObjectIteratorBase {
public:
  /// Object type.
  typedef ObjType  ObjectType;

  /// Construct new iterator over sizes.
  OmsArrayObjectIterator(OmsHandle &h, OmsSchemaHandle sh, OmsContainerNo cn)
    : OmsArrayObjectIteratorBase(h, ClsGuid, sh, cn)
  {
  }

  /// Get iterator over current size class.
  inline OmsObjByClsIter<ObjectType> omsAllOids(int maxBufferSize = 20)
  {
    OmsObjByClsIterBase iter = OmsArrayObjectIteratorBase::omsAllOids(maxBufferSize);
    return *REINTERPRET_CAST(OmsObjByClsIter<ObjectType>*, &iter);
  }
};


//==========================================================================

/*!
 * \brief Array object.
 *
 * This object type allows storing \c capa elements of equal type T, where
 * \c n can be specified at container registration time.
 *
 * \param T array element type (must provide default constructor and may
 *  not have a vtable, i.e., no virtual functions are allowed),
 * \param ClsGuid GUID of the class.
 *
 * Example:
 * \code
 * const int ClsId = 4711;
 *
 * // Register class (typedef is used, as DEFINE might have problems with comma in template class)
 * typedef OmsArrayObject<int, ClsId> OmsArrayObjectType;
 * OMS_REG_ARRAYCLASS(OmsArrayObjectType, h);
 *
 * // Create logical container (no size is specified!).  This call is only needed if logical 
 * //container does not exist already. If container exists, then the following statement does
 * //not make anything.
 * h.omsCreateContainer(OmsArrayObjectType::omsGetGuid(), 1, 1);
 *
 * // After the creation of a container a commit should be executed to release a possible lock.
 * //commit   
 *
 * // Creation of persistent objects (if there is no sub-container for this size category, then 
 * // this sub-container is created implicitly.)
 * const int arraySize1 = 100;
 * OmsArrayObjectType *p1 = new(h, 1, 1, arraySize1) OmsArrayObjectType(h, arraySize1);
 * OmsArrayObjectOid oid1 = p1->omsGetOid();
 *
 * const int arraySize2 = 200;
 * OmsArrayObjectType *p2 = new(h, 1, 1, arraySize2) OmsArrayObjectType(h, arraySize2);
 * OmsArrayObjectOid oid2 = p2->omsGetOid();
 *
 * // Read objects
 * oid1.omsDeRef(h);
 * oid2.omsDeRef(h);
 * \endcode
 *
 * Optimal sizes for objects (including vtable pointer and size_t for element count) are
 * 16, 24, 32, 40, 56, 72, 88, 104, 120, 144, 168, 192, 216, 248, 280, 328, 376, 424, 480,
 * 552, 648, 784, 872, 984, 1128, 1320, 1592, 2000, 2672, 4024 and 8072 bytes. It is not
 * possible to create an array object >8072 bytes.
 *
 * \note In order not to include conflicting header <new>, new instances of T are simply
 * zero-initialized.
 *
 * \see OmsArrayObjectIterator
 * \since 7.4.3.30, 7.5.0.2
 */
template <class T, int ClsGuid>
class OmsArrayObject : public OmsObject< OmsArrayObject<T, ClsGuid>, ClsGuid >
{
public:
  /// Object type.
  typedef OmsArrayObject<T, ClsGuid>  ObjectType;

  /// Object size class iterator type.
  typedef OmsArrayObjectIterator<T, ClsGuid>  Iterator;

  /*!
   * \brief Constructor
   *
   * Initialize all array elements using default constructor of T.
   *
   * \note In order not to include conflicting header <new>, new instances of T are simply
   * zero-initialized.
   */
  OmsArrayObject( OmsHandle& h, size_t n, bool init = true )
  {
    if (n > omsGetCapacity(h))
      OmsArrayObjectBase::ThrowInvalidParam((const char*)__FILE__, __LINE__);
    m_size = n;
    if( init )
    {
      // zero initialize
      omsConstructT(m_data, m_size);
    }
  }

  OmsArrayObject()
  {}

  /*! 
   * new operator for object creation. 
   * 
   * \param sz size for allocation
   * \param handle OmsHandle for internal lc access
   * \param schema Schema of new object
   * \param containerNo ContainerNo of new object
   * \param arraySize number of elements in the new array
   */
  void* operator new (size_t          sz, 
                      OmsHandle&      handle, 
                      OmsSchemaHandle schema, 
                      OmsContainerNo  containerNo,
                      size_t          arraySize)
  {
    // calculate number of bytes for this array
    size_t arrayByteSize = arraySize * sizeof( T ) + omsGetAdminDataSize();
    return (void*) OmsAbstractObject::omsNewObject(handle, ClsGuid, schema, containerNo, arrayByteSize); 
  }

  /*!
   * Placement delete
   */
  void operator delete (void           *p, 
                        OmsHandle      &handle, 
                        OmsSchemaHandle schema, 
                        OmsContainerNo  containerNo, 
                        size_t          arraySize) 
  {}

  /*!
   * New operator for for class registration
   */
  void* operator new (size_t                         sz, 
                      OmsHandle                     &handle, 
                      OmsAbstractObject::OmsRegEnum  e)
  { 
    // adjust object size in order to be compatible with old version
    // not really needed, but is necessary to be compatible with old sizes
    sz += sizeof( size_t );  
    return (void*) OmsAbstractObject::omsNewRegistryObject( sz , handle, ClsGuid); 
  }

  /*!
   * Placement delete
   */
  void operator delete (void                          *p, 
                        OmsHandle                     &handle, 
                        OmsAbstractObject::OmsRegEnum  e) 
  {}

  /*!
   * \return reference to i-th array element.
   */
  T& operator[](int idx)
  {
    if (size_t(idx) >= m_size)
      OmsArrayObjectBase::ThrowInvalidParam((const char*)__FILE__, __LINE__);
    return m_data[idx];
  }

  /*!
   * \return const reference to i-th array element
   */
  const T& operator[](int idx) const
  {
    if (size_t(idx) >= m_size)
      OmsArrayObjectBase::ThrowInvalidParam((const char*)__FILE__, __LINE__);
    return m_data[idx];
  }

  static void omsRegClass( OmsHandle& h
                         , const char* pClassName
                         , OmsArrayObject<T, ClsGuid> *pObj
                         )
  { 
    // plain object size is sufficient
    // old implementation internally adds 2*sizeof( size_t ) to size of base class
    // this is not necessary and was removed
    // it is added here, so that the total sizes in the old array implementation
    // remain the same
    size_t objectsize = sizeof( T ) + sizeof( size_t );  
    OmsAbstractObject::omsRegArrayClass( h, ClsGuid, pClassName, objectsize, pObj ); 
  }

  /*!
   * \brief Fill the array.
   *
   * Fill the first \c n array elements reading from \c p. If the array
   * is bigger than \c n, the remaining elements are untouched.
   *
   * \param p pointer to array with elements to fill in,
   * \param n count of elements to fill in.
   */
  void omsFill(const T* p, size_t n) 
  {
    if (n > m_size)
      OmsArrayObjectBase::ThrowInvalidParam((const char*)__FILE__, __LINE__);
    T* p_ = m_data;
    for (int i = 0; i < int(n); ++i)
    { *p_ = *p; ++p_; ++p; }
  }

  /*!
   * \brief Fill the array with memcpy.
   *
   * Fill the first \c n array elements reading from \c p. If the array
   * is bigger than \c n, the remaining elements are untouched.
   *
   * This version uses memcpy() instead of assignment operator.
   *
   * \param p pointer to array with elements to fill in,
   * \param n count of elements to fill in.
   */
  void omsFillMemcpy(const T* p, size_t n) 
  {
    if (n > m_size)
      OmsArrayObjectBase::ThrowInvalidParam((const char*)__FILE__, __LINE__);
    if (n > 0)
      OmsArrayUtils::tmemcpy( m_data, p, OmsTypeUInt4( sizeof(T) * n ) );
  }

  /*!
   * \brief Get pointer to the first element
   *
   * \return const pointer to the first array element.
   */
  const T* omsBegin() const
  { 
    return m_data; 
  }

  /*!
   * \brief Get array size
   *
   * \return the size of the array (number of possible entries).
   */
  size_t omsGetSize() const
  { 
    return m_size;
  }

  /*!
   * \return Return the maximal number of objects, which can be stored in the
   * same category as the actual object is belonging to.
   */
  size_t omsGetCapacity(OmsHandle &h)
  {
    size_t size = OmsArrayObjectBase::GetMaxByteSize(h, this) - omsGetAdminDataSize(); 
    return size / sizeof(T);
  }

  /*!
   * \return The maximal number of array elements which can be stored in the category 
   * which is the smallest category to hold the given number of elements (capacity).
   */
  static size_t omsGetCapacity(size_t capacity)
  {
    size_t size_needed = capacity * sizeof(T) + omsGetAdminDataSize();
    size_t size_max = OmsArrayObjectBase::GetMaxByteSize( OmsArrayObjectBase::GetCategory( size_needed ) ) - omsGetAdminDataSize();
    return size_max / sizeof(T);
  }
 
  /*!
   * \brief Return maximal possible capacity for this array object type.
   */
  static size_t omsGetMaxCapacity()
  {
    return ( OmsArrayObjectBase::GetMaxByteSize(OMS_ARROBJ_OPTSIZECNT) - omsGetAdminDataSize() ) / sizeof(T);
  }

  /*!
   * \brief Resize the object array.
   *
   * This method allows resizing existing array in object in the range 0 to
   * omsGetCapacity(). Newly added objects are default initialized.
   *
   * \param h OMS handle,
   * \param newSize new size of the array.
   *
   * \note In order not to include conflicting header <new>, new instances of T are simply
   * zero-initialized.
   */
  void omsResize(OmsHandle &h, size_t newSize)
  {
    if (newSize > omsGetCapacity(h))
      OmsArrayObjectBase::ThrowInvalidParam((const char*)__FILE__, __LINE__);
    if (m_size < newSize) {
      omsConstructT(m_data + m_size, newSize - m_size);
    }
    m_size = newSize;
  }

  /*!
   * \brief Get iterator over all sizes in this container.
   *
   * \param h OMS handle,
   * \param sh schema handle,
   * \param cn container number (base).
   * \return iterator over all size classes registered in this container.
   */
  static Iterator omsGetSizesIterator(OmsHandle &h, OmsSchemaHandle sh, OmsContainerNo cn)
  {
    return Iterator(h, sh, cn);
  }

private:
  /// Disallow default constructor.
  //OmsArrayObject() {}

  static size_t omsGetAdminDataSize() 
  {
    return sizeof( size_t ); 
  }

  /// Default constructor for objects - zero initialize.
  void omsConstructT(T *obj, size_t count)
  {
    OmsArrayUtils::tmemset( obj, 0, OmsTypeUInt4( sizeof(T) * count ) );
  }

protected:
  size_t    m_size;    ///< Size of the array (count of elements).
  T         m_data[1]; ///< First array element (placeholder for the whole array).
};

/*----------------------------------------------------------------------*/

#endif  // __OMS_ARRAYOBJECT_HPP

