
/*!
 * \file    OMS_ArrayObjectPlain.hpp
 * \author  ReinerSi
 * \brief   Plain implementation for array objects.
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

#ifndef OMS_ARRAY_OBJECT_PLAIN_HPP
#define OMS_ARRAY_OBJECT_PLAIN_HPP

#include "Oms/OMS_Object.hpp"
#include "Oms/OMS_Handle.hpp"
#include "Oms/OMS_ArrayObject.hpp"
#include "Oms/OMS_ArrayObjectUtils.hpp"
#include "Oms/OMS_AllocatorInterface.hpp"

//==========================================================================

#define OMS_PERSISTENT_PAGE_SIZE 8100
#define OMS_PERSISTENT_PAGE_HEADER_TRAILER_SIZE 92
#define OMS_PERSISTENT_OBJECT_HEADER_SIZE 24

#define OMS_ARRAY_OBJECT_BUFFER_SIZE 5 
#define OMS_ARRAY_SPLIT_TOLERANCE ( OMS_PERSISTENT_OBJECT_HEADER_SIZE + 10 ) 

#define OMS_ARRAY_SPLIT_CHUNK_SIZES_COUNT 3

#define OMS_MAX_ARRAY_CLASS_NAME_LENGTH 256

extern const AFX_EXT_CLASS OmsTypeUInt4 OMS_ARRAY_SPLIT_CHUNK_SIZES[]; 

//==========================================================================

enum OmsArraySplitPolicyEnum {
  OmsArraySplitNone = 1,
  OmsArraySplitFull = 2
}; 

template < OmsArraySplitPolicyEnum splitPolicy >
struct OmsArraySplitPolicy
{ };

template <>
struct OmsArraySplitPolicy< OmsArraySplitNone >
{ 
  static OmsTypeUInt4 getSplitSize() { return 1; }
};

template <>
struct OmsArraySplitPolicy< OmsArraySplitFull >
{ 
  static OmsTypeUInt4 getSplitSize() { return OMS_ARRAY_SPLIT_CHUNK_SIZES_COUNT; }
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
template < typename T, int ClsGuid >
class OmsArrayData : public OmsObject< OmsArrayData< T, ClsGuid >, ClsGuid >
{
public:
  /// Object type.
  typedef OmsArrayData< T, ClsGuid > Self;
  typedef OmsOid< Self > SelfOid; 
  /// Object size class iterator type.
  typedef OmsArrayObjectIterator< T, ClsGuid, Self > Iterator;

  /*!
   * \brief Constructor
   *
   * Initialize all array elements using default constructor of T.
   *
   * \note In order not to include conflicting header <new>, new instances of T are simply
   * zero-initialized.
   */
  OmsArrayData( OmsHandle& h, OmsTypeUInt4 n, bool headChunk = false )
    : m_size( n )
  {
    if ( n > omsGetCapacity(h) )
      OmsArrayObjectBase::ThrowInvalidParam( (const char*)__FILE__, __LINE__ );

    setHeadChunk( headChunk ); 
    omsConstructT( m_data, n );
  }

  OmsArrayData()
  { }

  void* operator new ( size_t          sz
                     , OmsHandle      &handle 
                     , OmsSchemaHandle schema
                     , OmsContainerNo  containerNo
                     , size_t          arraySize
                     )
  {
    // convert array element count into no of bytes needed to store persistent array data 
    size_t no_bytes = omsGetAdminDataSize() + arraySize * sizeof(T); 
    return (void*) OmsAbstractObject::omsNewObject( handle, ClsGuid, schema, containerNo, no_bytes ); 
  }

  void operator delete ( void* p
                       , OmsHandle& handle
                       , OmsSchemaHandle schema
                       , OmsContainerNo containerNo
                       , size_t arraySize
                       ) 
  { }

  void* operator new ( size_t sz
                     , OmsHandle& handle
                     , OmsAbstractObject::OmsRegEnum  e
                     )
  { 
    sz = OmsArrayObjectBase::GetMaxByteSize(OmsArrayObjectBase::GetCategory( omsGetAdminDataSize() )) + sizeof(void*);
    return (void*) OmsAbstractObject::omsNewRegistryObject( sz , handle, ClsGuid ); 
  }

  void operator delete ( void* p 
                       , OmsHandle& handle
                       , OmsAbstractObject::OmsRegEnum e
                       ) 
  { }

  /*!
   * \return reference to i-th array element.
   */
  T& operator[]( size_t idx )
  {
    if ( idx >= omsGetSize() )
      OmsArrayObjectBase::ThrowInvalidParam( (const char*)__FILE__, __LINE__ );
    return m_data[idx];
  }


  /*!
   * \return const reference to i-th array element
   */
  const T& operator[]( size_t idx ) const
  {
    if ( size_t(idx) >= omsGetSize() )
      OmsArrayObjectBase::ThrowInvalidParam( (const char*)__FILE__, __LINE__ );
    return m_data[idx];
  }

  static void omsRegClass( OmsHandle& handle
                         , const char* pClassName
                         , OmsArrayData< T, ClsGuid >* pObj
                         )
  { 
    OmsAbstractObject::omsRegArrayClass( handle, ClsGuid, pClassName, omsGetAdminDataSize(), pObj ); 
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
  void omsFill( const T* p, OmsTypeUInt4 n ) 
  {
    if( n > omsGetSize() )
      OmsArrayObjectBase::ThrowInvalidParam((const char*)__FILE__, __LINE__);
    T* p_ = m_data;
    for( int i = 0; i < int(n); ++i )
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
  void omsFillMemcpy( const T* p, OmsTypeUInt4 n ) 
  {
    if ( n > omsGetSize() )
      OmsArrayObjectBase::ThrowInvalidParam((const char*)__FILE__, __LINE__);

    if ( n > 0 )
      OmsArrayUtils::tmemcpy( m_data, p, sizeof(T) * n );
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

  T* omsBegin() 
  {
    return m_data; 
  }

  /*!
   * \brief Get array size
   *
   * \return the size of the array (number of possible entries).
   */
  OmsTypeUInt4 omsGetSize() const
  { 
    // size is coded in first 24 bits
    return ( m_size & 0xffffff );
  }

  bool isHeadChunk() const
  {
    // object is head chunk, if bit 25 is set
    return ( m_size & ( 1 << 24 ) ) != 0; 
  }

  void setHeadChunk( bool headChunk )
  {
    if( headChunk )
    {
      m_size = m_size | ( 1 << 24 ); 
    } else {
      m_size = m_size & ~( 1 << 24 ); 
    }
  }

  /*!
   * Returns the next chunk in the chunk chain for const objects. 
   * If this is the last chunk, the returned value is 0.
   * 
   * \return next chunk
   */
  const Self* getNextChunk( OmsHandle& h ) const
  {
    if( m_next.isTrue() )
    {
      if( h.omsCheckOid( m_next, ClsGuid ) ) {
        return ( m_next )( h ); 
      } else {
        OmsArrayObjectBase::ThrowObjectLinkCorruption( __FILE__, __LINE__ ); 
      }
    }
    return 0; 
  }

  /*!
   * Returns the next chunk in the chunk chain for non-const objects. 
   * If this is the last chunk, the returned value is 0.
   * 
   * \return next chunk
   */
  Self* getNextChunk( OmsHandle& h )
  {
    if( m_next.isTrue() )
    {
      if( h.omsCheckOid( m_next, ClsGuid ) ) {
        return ( m_next )( h, true ); 
      } else {
        OmsArrayObjectBase::ThrowObjectLinkCorruption( __FILE__, __LINE__ ); 
      }
    }
    return 0; 
  }

  /*!
   * Returns the oid of the next chunk
   * 
   * \return oid of next chunk
   */
  const SelfOid& getNextChunkOid() const
  {
    return m_next; 
  }

  /*!
   * Sets the next chunk in the chunk chain. 
   * If this is the last chunk in the chain, the next chunk should be set to 0.
   * 
   * \param next_chunk next chunk in chain
   */
  void setNextChunk( Self* next_chunk )
  {
    if( next_chunk )
    {
      m_next = next_chunk->omsGetOid(); 
    } else {
      // set oid to next chunk to nil oid
      m_next = OmsOid< Self >(); 
    }
  }

  size_t getSizeInBytes( OmsHandle& h ) const
  {
    return OmsArrayObjectBase::GetMaxByteSize( h, const_cast<Self*>(this) ); 
  }

  /*!
   * \return Return the maximal number of objects, which can be stored in the
   * same category as the actual object is belonging to.
   */
  size_t omsGetCapacity( OmsHandle& h ) const
  {
    // calculate bytes used for array data: 
    // bytes in category - admin data
    size_t bytes = OmsArrayObjectBase::GetMaxByteSize( h, const_cast<Self*>(this) ) - omsGetAdminDataSize(); 
    return ( bytes / sizeof(T) );
  }


  /*!
   * \return The maximal number of array elements which can be stored in the category 
   * which is the smallest category to hold the given number of elements (capacity).
   */
  static OmsTypeUInt4 omsGetCapacity( OmsTypeUInt4 capacity ) 
  {
    // calculate size needed for array with "capacity" elements
    OmsTypeUInt4 size_needed = capacity * sizeof( T ) + omsGetAdminDataSize(); 
    // get byte count in category
    OmsTypeUInt4 size_max = OmsArrayObjectBase::GetMaxByteSize( OmsArrayObjectBase::GetCategory( size_needed ) ); 
    return ( size_max - omsGetAdminDataSize() ) / sizeof( T );
  }

  /*!
   * \brief Return maximal possible capacity for this array object type.
   */
  static OmsTypeUInt4 omsGetMaxCapacity()
  {
    return OmsArrayObjectBase::GetMaxByteSize( OMS_ARROBJ_OPTSIZECNT ) / sizeof( T );
  }

  /*!
   * Returns the size of the administrative data, that
   * is has to be stored in addition to the data elements. 
   */
  static OmsTypeUInt4 omsGetAdminDataSize() 
  {
    // size of object - vtableptr - size of( m_data )
    return sizeof( Self ) - sizeof( void* ); 
  }

  /*!
   * \brief Get iterator over all sizes in this container.
   *
   * \param h OMS handle,
   * \param sh schema handle,
   * \param cn container number (base).
   * 
   * \return iterator over all size classes registered in this container.
   */
  static Iterator omsGetSizesIterator( OmsHandle& h
                                     , OmsSchemaHandle sh
                                     , OmsContainerNo cn
                                     )
  {
    return Iterator( h, sh, cn );
  }

private:
  /// Disallow default constructor.
  //OmsArrayObject() {}

  /// Default constructor for objects - zero initialize.
  void omsConstructT( T* obj, OmsTypeUInt4 count )
  {
    OmsArrayUtils::tmemset( obj, 0, sizeof( T ) * count );
  }

protected:
  OmsTypeUInt4  m_size;     ///< Size of the array (count of elements).
  SelfOid       m_next;     ///< Oid pointing to the next chunk in chain
  T             m_data[1];  ///< First array element (placeholder for the whole array).
};

//==========================================================================

/*
template < typename T, int ClsGuid > 
class OmsArrayHeadData : public OmsArrayHeadObject< OmsArrayData< T, ClsGuid >, ClsGuid >
{
public:

}
*/
//==========================================================================

template< class T, int ClsGuid, class OmsArrayObjectBuffer >
class OmsArrayDataIter
{
  typedef OmsArrayData< T, ClsGuid > OmsArrayDataType; 

public: 

  OmsArrayDataIter( OmsHandle& h, const OmsArrayObjectBuffer& buf ) 
    : m_h( h )
    , m_buffer( buf )
    , m_chunk( 0 )
    , m_data_ptr( 0 )
    , m_pos( 0 )
  { 
    buf.first(); 
    m_chunk = buf.curr(); 
    m_data_ptr = m_chunk->omsBegin(); 
  }

  const T& getValue() 
  {
    return *m_data_ptr; 
  }

  void operator++()
  {
    ++m_pos; 
    if( m_pos >= m_chunk->omsGetSize() )
    {
      m_chunk = m_buffer.next(); 
      m_data_ptr = m_chunk ? m_chunk->omsBegin() : 0; 
      m_pos = 0; 
    } else {
      ++m_data_ptr;
    }
  }

  operator bool() const
  {
    return ( m_data_ptr != 0 );
  }

private: 

  OmsHandle& m_h;                        ///< handle to livecache
  const OmsArrayObjectBuffer& m_buffer;  ///< buffer for chunk chain management
  const OmsArrayDataType* m_chunk;       ///< current data chunk
  const T* m_data_ptr;                   ///< current data buffer
  OmsTypeUInt4 m_pos;                    ///< nr of elem the iterator points to
};

//==========================================================================

template< class T, int ClsGuid >
class OmsArrayDataOidIter 
{
public: 
  typedef OmsArrayData< T, ClsGuid > OmsArrayDataType; 
  typedef OmsOid< OmsArrayDataType > OmsArrayOid; 

  OmsArrayDataOidIter( OmsHandle& h
                     , OmsSchemaHandle sh
                     , OmsContainerNo cno
                     ) 
                     : m_baseIter( OmsArrayDataType::omsAllOids( h, sh, cno ) )
  { }

  const OmsArrayDataType* omsDeRef() const
  {
    return m_baseIter.omsDeRef(); 
  }

  void operator++() 
  {
    while( m_baseIter().isTrue() )
    {
      ++m_baseIter; 
      const OmsArrayDataType* chunk = m_baseIter.omsDeRef(); 
      if( chunk->isHeadChunk() )
      {
        // return only, if object is head chunk
        return; 
      }
    }
  }

  const OmsArrayOid operator() () const
  {
    return m_baseIter(); 
  }

  operator bool() 
  {
    return m_baseIter.operator bool(); 
  }

private: 

  OmsObjByClsIter< OmsArrayDataType > m_baseIter;   ///< iterator used for iterating over all array elements
};

//==========================================================================

class AFX_EXT_CLASS OmsSessionObject 
{
protected: 
  OmsSessionObject( OmsHandle& h )
    : m_h( h )
    , m_alloc( h.omsGetSessionAllocator() )
  { }

  OmsAllocatorInterface& getAllocator() { return m_alloc; }

  OmsHandle& m_h; 
  OmsAllocatorInterface& m_alloc; 
};

//==========================================================================

/*! 
 * Buffer object used for managing chunk chains
 */
template < class T, int BUF_SIZE >
class OmsObjectBuffer : public OmsSessionObject
{
public: 
  typedef OmsObjectBuffer< T, BUF_SIZE > Self; 

  friend class OmsObjectBufferContainer; 

  class OmsObjectBufferContainer
  {
  public: 

    OmsObjectBufferContainer() 
      : m_next( 0 )
      , m_prev( 0 )
      , m_count( 0 )
    { }

    bool append( const T* obj ) 
    {
      if( m_count < BUF_SIZE )
      {
        m_data[ m_count ] = const_cast< T* >( obj ); 
        ++m_count; 
        return true; 
      }
      return false; 
    }

    void move( Self& tgt, OmsHandle& h, OmsTypeInt4 newCount = 0 ) 
    {
      for( OmsTypeInt4 i = newCount; i < m_count; ++i )
      {
        tgt.append( m_data[ i ] ); 
      }
      m_count = newCount; 
    }

    void drop( OmsHandle& h, OmsTypeInt4 newCount = 0 ) 
    {
      for( OmsTypeInt4 i = newCount; i < m_count; ++i )
      {
        m_data[ i ]->omsDelete( h ); 
      }
      m_count = newCount; 
    }

    void release( OmsHandle& h, OmsTypeInt4 aboveIndex = 0 )
    {
      for( OmsTypeInt4 i = aboveIndex; i < m_count; ++i )
      {
        h.omsRelease( m_data[ i ]->omsGetOid() ); 
      }
      m_count = aboveIndex; 
    }

    OmsTypeUInt4 getSize() 
    {
      return m_count; 
    }

    T* get( OmsTypeInt4 index )
    {
      if( index < m_count )
      {
        return m_data[ index ]; 
      }
      return 0; 
    }

    T* m_data[ BUF_SIZE ]; 
    OmsTypeInt4 m_count; 
    OmsObjectBufferContainer* m_next; 
    OmsObjectBufferContainer* m_prev; 
  };

  OmsObjectBuffer( OmsHandle& h ) 
    : OmsSessionObject( h )
    , m_first( createContainer() )
    , m_last( m_first )
    , m_current( m_first )
    , m_index( -1 )
    , m_size( 0 )
    , m_locked( false )
  { }

  OmsObjectBuffer( OmsHandle& h, const T* first ) 
    : OmsSessionObject( h )
    , m_first( createContainer() )
    , m_last( m_first )
    , m_current( m_first )
    , m_index( -1 )
    , m_size( 0 )
    , m_locked( false )
  { 
    setData( first ); 
  }

  void setData( const T* first ) 
  {
    const T* obj( first ); 
    while( obj )
    {
      append( const_cast< T* >( obj ) ); 
      obj = obj->getNextChunk( m_h ); 
    }  
  }

  void lock() const
  {
    if( ! m_locked )
    {
      first(); 
      const T* firstChunk( curr() ); 
      if( firstChunk )
      {
        T* chunk = firstChunk->omsForUpdPtr( m_h ); 
        while( chunk ) 
        {
          chunk = chunk->getNextChunk( m_h ); 
        }
      }
      m_locked = true; 
    }
  }

  bool isLocked() const
  {
    return m_locked; 
  }

  bool append( T* newObj ) 
  {
    if( newObj )
    { 
      T* curObj = curr(); 
      if( curObj )
      {
        curObj->setNextChunk( newObj ); 
      }
      ++m_size; 
      if( ! m_last->append( newObj ) )
      {
        enlarge(); 
        m_index = 0; 
        return ( m_last->append( newObj ) ); 
      } else {
        ++m_index; 
      }
      return true; 
    }
    return false; 
  }

  Self* forUpdate() const
  {
    lock(); 
    return const_cast< Self* >( this ); 
  }

  void beforeFirst()
  {
    m_current = m_first; 
    m_index = -1; 
  }

  void first() 
  {
    m_current = m_first; 
    m_index = ( m_size ? 0 : -1 ); 
  }

  void last()
  {
    m_current = m_last; 
    m_index = m_current->m_count - 1; 
  }

  T* next() 
  {
    ++m_index; 
    if( m_index == m_current->getSize() )
    {
      if( ! m_current->m_next )
      {
        --m_index; 
        return 0; 
      }
      m_index = 0; 
      m_current = m_current->m_next; 
    }
    T* res( m_current->get( m_index ) ); 
    return res; 
  }

  T* prev() 
  {
    --m_index; 
    if( m_index < 0 ) 
    {
      if( m_current == m_first )
      {
        ++m_index; 
        return 0; 
      }
      m_index = BUF_SIZE - 1; 
      m_current = m_current->m_prev; 
    }
    return m_current->get( m_index );  
  }

  T* curr() 
  {
    return m_current->get( m_index ); 
  }

  T* operator() () 
  {
    return curr(); 
  }

  void beforeFirst() const
  {
    Self* non_const_ptr = const_cast< Self* >( this ); 
    non_const_ptr->beforeFirst(); 
  }

  void first() const
  {
    Self* non_const_ptr = const_cast< Self* >( this ); 
    non_const_ptr->first(); 
  }

  void last() const
  {
    Self* non_const_ptr = const_cast< Self* >( this ); 
    non_const_ptr->last(); 
  }

  const T* next() const
  {
    Self* non_const_ptr = const_cast< Self* >( this ); 
    return non_const_ptr->next(); 
  }

  const T* prev() const
  {
    Self* non_const_ptr = const_cast< Self* >( this ); 
    return non_const_ptr->prev(); 
  }

  const T* curr() const
  {
    Self* non_const_ptr = const_cast< Self* >( this ); 
    return non_const_ptr->curr(); 
  }

  const T* operator() () const
  {
    return curr(); 
  }

  /*! 
   * Clears the complete buffer, deleting all containers and removing all entries. 
   */
  void clear() const
  {
    m_first->release( this->m_h ); 
    OmsObjectBufferContainer* cur = m_current->m_next; 
    while( cur )
    {
      cur->release( this->m_h ); 
      OmsObjectBufferContainer* tmp = cur; 
      cur = cur->m_next; 
      const_cast< Self* >( this )->freeContainer( tmp ); 
    }

    m_size = 0; 
    m_index = -1; 
    m_last = m_first; 
    m_current = m_first; 
    m_locked = false; 
  }

  /*!
   * Drop all chunks in this buffer
   */
  void drop() 
  {
    first(); 
    dropTail(); 
  }

  void moveTail( Self& tgt ) 
  {
    moveTail( tgt, m_current, m_index ); 
    afterTailCut(); 
  }

  /*!
   * Deletes all containers starting from the current position (including) to 
   * the end of the buffer. 
   */
  void dropTail() 
  {
    dropTail( m_current, m_index ); 
    afterTailCut(); 
  }

  /*!
   * Return number of chunks in chain
   */
  OmsTypeUInt4 getChunkCount() const 
  {
    return m_size; 
  }

  OmsTypeUInt4 getElemCount() const
  {
    OmsTypeUInt4 count( 0 ); 
    first(); 
    const T* chunk( curr() );
    while( chunk )
    {
      count += chunk->omsGetSize(); 
      chunk = next(); 
    }
    return count; 
  }

  virtual ~OmsObjectBuffer() 
  { 
    try {
      clear(); 
      freeContainer( m_first ); 
    } catch( DbpError& err )
    {   
      this->m_h.dbpOpError( "DbpError caught in OmsObjectBuffer::~OmsObjectBuffer() code: %d, text: ", err.m_errorNo, err.m_errorText ); 
      throw err; 
    }
  }

private: 

  void afterTailCut() 
  {
    if( m_index == 0 )
    {
      // all elements in current container dropped
      if( m_current != m_first )
      {
        m_current = m_current->m_prev; 
        freeContainer( m_current ); 
      } 
    }
    m_last = m_current; 
    m_current->m_next = 0; 
    m_size = calculateSize(); 
    m_index = m_current->getSize() - 1; 
    T* obj = curr(); 
    if( obj )
    {
      obj->setNextChunk( 0 ); 
    }
  }

  OmsTypeInt4 calculateSize() 
  {
    OmsTypeUInt4 count( 0 ); 
    OmsObjectBufferContainer* cur = m_first; 
    while( cur )
    {
      count += cur->getSize(); 
      cur = cur->m_next; 
    }
    return count; 
  }

  void dropTail( OmsObjectBufferContainer* cont, OmsTypeInt4 lastIndex ) 
  {
    cont->drop( this->m_h, lastIndex ); 
    OmsObjectBufferContainer* cur = cont->m_next; 
    while( cur )
    {
      cur->drop( this->m_h ); 
      OmsObjectBufferContainer* tmp = cur; 
      cur = cur->m_next; 
      freeContainer( tmp ); 
    }
  }

  void moveTail( Self& tgt, OmsObjectBufferContainer* cont, OmsTypeInt4 lastIndex ) 
  {
    cont->move( tgt, this->m_h, lastIndex ); 
    OmsObjectBufferContainer* cur = cont->m_next; 
    while( cur )
    {
      cur->move( tgt, this->m_h ); 
      OmsObjectBufferContainer* tmp = cur; 
      cur = cur->m_next; 
      freeContainer( tmp ); 
    }
  }

  void enlarge() 
  {
    OmsObjectBufferContainer* tmp = createContainer(); 
    // TODO+++ bad alloc?
    if( tmp )
    {
      m_last->m_next = tmp; 
      tmp->m_prev = m_last; 
      m_last = tmp; 
    }
  }

  OmsObjectBufferContainer* createContainer() 
  {
    return new( this->getAllocator() ) OmsObjectBufferContainer(); 
  }

  void freeContainer( OmsObjectBufferContainer* ptr )
  {
    this->getAllocator().omsDeallocate( ptr ); 
  }

  mutable OmsObjectBufferContainer* m_first; 
  mutable OmsObjectBufferContainer* m_last; 
  mutable OmsObjectBufferContainer* m_current; 
  mutable OmsTypeInt4 m_index; 
  mutable OmsTypeInt4 m_size; 
  mutable bool m_locked; 
};

//==========================================================================

template< class T, int ClsGuid, int BUF_SIZE >
class OmsObjectBufferEnumerator : public OmsEnumerator< T >
{
public: 
  typedef OmsArrayData< T, ClsGuid > OmsArrayDataType; 
  typedef OmsObjectBuffer< OmsArrayDataType, BUF_SIZE > ObjectBufferType; 

  OmsObjectBufferEnumerator( const ObjectBufferType& buffer, OmsTypeUInt4 offset = 0 )
    : m_buffer( buffer )
    , m_start( offset )
    , m_current( 0 )
    , m_pos( 0 )
  { 
    reset(); 
  }

  const T& operator() () 
  {
    return *m_cur_elem; 
  }

  void operator++() 
  {
    ++m_cur_elem; 
    if( m_cur_elem >= m_end_elem )
    {
      m_current = m_buffer.next(); 
      if( m_current )
      {
        m_cur_elem = m_current->omsBegin(); 
        m_end_elem = m_current->omsBegin() + m_current->omsGetSize(); 
      }
    }
  }

  operator bool() 
  {
    return ( m_current && m_pos < m_current->omsGetSize() ); 
  }

  void reset() 
  {
    m_current = m_buffer.first(); 
    if( m_current )
    {
      m_pos = m_start; 
      while( m_pos > m_current->omsGetSize() )
      {
        m_pos -= m_current->omsGetSize(); 
        m_current = m_buffer.next(); 
      }
      m_cur_elem = m_current->omsBegin() + m_pos; 
      m_end_elem = m_current->omsBegin() + m_current->omsGetSize(); 
    }
  }

private: 

  const ObjectBufferType& m_buffer; 
  const OmsTypeUInt4 m_start; 
  const OmsArrayDataType* m_current; 
  const T* m_cur_elem; 
  const T* m_end_elem; 
  OmsTypeUInt4 m_pos; 
};

//==========================================================================

class AFX_EXT_CLASS OmsArrayTrace
{
protected: 

  bool isTracing(); 
};

//==========================================================================

class OmsArrayImplGeneric : public OmsArrayTrace
{
public: 

  /*
  in der Vergangenheit haben wir mit Oswald in Bezug auf die anfallenden persistenten
  Kosten eines Objekts der Grösse X folgende Bruttobedarfsrechnung gemacht:

  Nettobedarf: X Bytes
  Persistenter Kopf: 24 Bytes
  Pageeintrag: Y = X + 24 Bytes
  Bruttobedarf: Y + AveragePageWaste(Y)

  wobei AveragePageWaste(Y) = ( ( 8100 MOD Y ) + 92 )/ (8100 DIV Y) der pro Objekt anfallende Verschnitt
  zwischen Bruttobedarf und Page-Gesamtgrösse ist.

  Nur unter Einbeziehung dieses Verschnitts kriegt man eine akkurate Schätzung der persistenten Kosten
  hin. Ich wollte dich bitten, diese Art von Bruttorechnung noch in irgendeiner Methode zur Verfügung zu stellen.
  (getBruttoBytes() ???), da wir aussen an das tatsächliche Layout der notwendigen Subframes ja nicht
  mehr rankommen und uns somit der externe Verschnitt verloren geht.
  */

  static OmsTypeUInt4 getBruttoObjSize( OmsTypeUInt4 plain_obj_size ) 
  {
    OmsTypeUInt4 size = getPersistentSize( plain_obj_size ); 
    size += getAveragePageWaste( size ); 
    return size; 
  }

  static OmsTypeUInt4 getAveragePageWaste( OmsTypeUInt4 page_obj_size )
  {
    OmsTypeUInt4 page_waste = ( OMS_PERSISTENT_PAGE_SIZE % page_obj_size ) + OMS_PERSISTENT_PAGE_HEADER_TRAILER_SIZE; 
    OmsTypeUInt4 obj_page = OMS_PERSISTENT_PAGE_SIZE / page_obj_size; 
    return page_waste / obj_page; 
  }

  static OmsTypeUInt4 getPersistentSize( OmsTypeUInt4 plain_obj_size )
  {
    // vtable ist not stored persistent, but the header
    return ( plain_obj_size - sizeof( void* ) + OMS_PERSISTENT_OBJECT_HEADER_SIZE ); 
  }

  /*!
   * Returns the next best split size for data chunks
   * 
   * \param elem_count number of elements, that have to be placed in chunks 
   *
   * \return best elem count for splitting chunks
   */ 
  static OmsTypeUInt4 getBestChunkSize( const OmsTypeUInt4 elem_count
                                      , const OmsTypeUInt4 admDataSize
                                      , const OmsTypeUInt4 typeSize
                                      , const OmsTypeUInt4 splitSize
                                      ) 
  {
    OmsTypeUInt4 byte_count = elem_count * typeSize; 
    // check, if data fits in one of the selected chunk sizes
    {
      for( OmsTypeUInt4 i=0; i < splitSize; ++i )
      {
        OmsTypeUInt4 split_size_byte = OMS_ARRAY_SPLIT_CHUNK_SIZES[ i ];
        OmsTypeUInt4 s = split_size_byte - admDataSize;
        if( byte_count >= s ) {
          return ( s / typeSize );
        } 
      }
    }
    return elem_count; 
  }

};



//==========================================================================

template< class T, int ClsGuid, OmsArraySplitPolicyEnum splitPolicy = OmsArraySplitFull >
class OmsArrayImplBase : public OmsArrayImplGeneric
{
protected: 
  typedef OmsArrayData< T, ClsGuid > OmsArrayDataType; 
  typedef OmsOid< OmsArrayData< T, ClsGuid > > OmsArrayDataOid; 
  typedef OmsObjectBuffer< OmsArrayDataType, OMS_ARRAY_OBJECT_BUFFER_SIZE > OmsArrayObjectBuffer; 
  typedef OmsObjectBufferEnumerator< OmsArrayDataType, ClsGuid, OMS_ARRAY_OBJECT_BUFFER_SIZE > OmsObjectBufferEnum; 

  OmsArrayImplBase( OmsHandle& h, OmsSchemaHandle sh, OmsContainerNo cno )
    : m_h( h )
    , m_sh( sh )
    , m_cno( cno )
  { }
 

  /*!
   * Store new data in underlying data chunks. This method tries to reuse as many data chunks
   * as possible and creates new chunks, if necessary. Chunks, that are available, but
   * do not have the best fit size are deleted. 
   * 
   * \param input_data new array data
   * \param number of elements in enumerator
   */
  void setDataTryReuseChunks( OmsArrayObjectBuffer& buf, OmsEnumerator< T >& input_data, OmsTypeUInt4 n )
  {
    // some chunks already exist, try to reuse if possible
    OmsTypeUInt4 remainder = n; 
    OmsTypeUInt4 used_count = 0; 
    // first try to reuse as much chunks as possible and delete all other old chunks
    buf.beforeFirst(); 
    fillChunksMaximumReuse( buf, input_data, remainder, used_count ); 
    remainder -= used_count; 

    // then create new chunks for all remaining array elements
    fillChunksCreateRemainder( buf, input_data, remainder ); 
    buf.first();
    setFirstChunk( buf() ); 
  }

  /*!
   * Create new data chunks for the given input stream and appends the chunks to the given data buffer
   * 
   * \param input_data input data 
   * \param n number of elements in input stream 
   */
  void fillChunksCreateRemainder( OmsArrayObjectBuffer& buf, OmsEnumerator< T >& input_data, OmsTypeUInt4 n )
  {
    // chunks are not created -> first store operation
    OmsTypeUInt4 size_needed( n ); 
    while( size_needed )
    {
      OmsArrayDataType* new_chunk = this->getNewChunk( size_needed );         
      size_needed -= new_chunk->omsGetSize(); 
      for( OmsTypeUInt4 i=0; i < new_chunk->omsGetSize(); ++i )
      {
        (*new_chunk)[i] = input_data(); 
        ++input_data; 
      }
      buf.append( new_chunk ); 
    }
  }


  void fillChunksMaximumReuse( OmsArrayObjectBuffer& buf
                             , OmsEnumerator< T >& input_data
                             , const OmsTypeUInt4 n
                             , OmsTypeUInt4& used_count 
                             )
  {
    used_count = 0; 
    OmsTypeUInt4 remainder = n; 
    OmsTypeUInt4 best_chunk_size = getBestChunkSize( remainder
                                                   , OmsArrayDataType::omsGetAdminDataSize()
                                                   , sizeof( T )
                                                   , OmsArraySplitPolicy< splitPolicy >::getSplitSize()
                                                   ); 
    bool full = true; 
    OmsArrayDataType* chunk( buf.next() ); 

    while( chunk && chunk->omsGetSize() == best_chunk_size )
    {
      full = fillDataChunk( input_data, *chunk, best_chunk_size ); 
      chunk->omsStore( this->getOmsHandle() ); 
      chunk = buf.next(); 
      remainder -= best_chunk_size; 
      best_chunk_size = getBestChunkSize( remainder
                                        , OmsArrayDataType::omsGetAdminDataSize()
                                        , sizeof( T )
                                        , OmsArraySplitPolicy< splitPolicy >::getSplitSize()
                                        ); 
    }
    if( buf.next() )
    {
      buf.dropTail(); 
    }
    used_count = n - remainder; 
  }

  /*!
   * Copy the given element count from enumerator to data chunk
   * 
   * \param input_data elements to copy
   * \param chunk target chunk
   * \param n number of elements to copy
   * 
   * \return true, if the given chunk is completely filled after the operation
   */
  bool fillDataChunk( OmsEnumerator< T >& input_data
                     , OmsArrayDataType& chunk
                     , const OmsTypeUInt4 n 
                     )
  {
    OmsTypeUInt4 i=0; 
    for( ; input_data && i < n; ++input_data )
    {
      chunk[ i ] = input_data(); 
      ++i;
    }
    // chunk is completely filled, if i == n after loop
    return ( i == n ); 
  }


  /*!
   * Store new data in underlying data chunks. This method reuses all data chunks
   * that belong already to this array object and assumes, that the data 
   * fits in the existing chunks. 
   * 
   * \param input_data new array data
   * \param number of elements in input data
   * \param offset where to start
   */
  OmsTypeUInt4 setDataReuseChunks( OmsArrayObjectBuffer& buf, OmsEnumerator< T >& input_data, const OmsTypeUInt4 offset )
  {
    // chunks already created -> no size change allowed
    OmsArrayDataType* chunk( buf() ); 
    chunk->omsStore( m_h ); 
    OmsTypeUInt4 count = 0; 
    OmsTypeUInt4 to_skip( offset ); 
    while( chunk && offset > chunk->omsGetSize() )
    {
      to_skip -= chunk->omsGetSize(); 
    }
    if( ! chunk )
    {
      return 0; 
    }

    const T* elem_start( chunk->omsBegin() + to_skip ); 
    const T* elem_end( chunk->omsBegin() + chunk->omsGetSize() ); 
    const T* elem_cur( elem_start ); 
    OmsTypeUInt4 elem_written( 0 ); 
    for( ; input_data; ++input_data )
    {
      // check, if chunk is already full
      if( elem_cur >= elem_end )
      {
        chunk = buf.next(); 
        if( ! chunk )
        {
          return ( elem_written + OmsTypeUInt4( elem_cur - elem_start ) ); 
        }
        elem_written += OmsTypeUInt4( elem_cur - elem_start ); 
        elem_start = chunk->omsBegin(); 
        elem_end = chunk->omsBegin() + chunk->omsGetSize(); 
        elem_cur = elem_start; 
      }
      *const_cast< T* >( elem_cur ) = input_data(); 
      ++elem_cur; 
    }
    return ( elem_written + OmsTypeUInt4( elem_cur - elem_start ) ); 
  }

  /*!
   * Store new data in underlying data chunks. This method stores all data
   * on newly created data chunks and assumes, that no data chunks belong to the object. 
   * 
   * \param input_data new array data
   * \param n number of data elements
   */
  void setDataCreateChunks( OmsArrayObjectBuffer& buf
                          , OmsEnumerator< T >& input_data
                          , OmsTypeUInt4 n 
                          )
  {
    // chunks are not created -> first store operation
    OmsTypeUInt4 size_needed( n ); 
    while( size_needed )
    {
      OmsArrayDataType* new_chunk = this->getNewChunk( size_needed );         
      size_needed -= new_chunk->omsGetSize(); 
      for( OmsTypeUInt4 i=0; i < new_chunk->omsGetSize(); ++i )
      {
        (*new_chunk)[i] = input_data(); 
        ++input_data; 
      }
      buf.append( new_chunk ); 
    }
    buf.first(); 
    // set local data pointers
    setFirstChunk( buf() ); 
  }


  void setFirstChunk( OmsArrayDataType* first_chunk ) 
  {
    if( first_chunk )
    {
      first_chunk->setHeadChunk( true ); 
    }
  }

  /*! 
  * Returns a new data chunk, that is able to store the best number of elements
  * in order to minimize split size. 
  * 
  * \param number of array elements needed
  * 
  * \return new data chunk. 
  */
  OmsArrayDataType* getNewChunk( const OmsTypeUInt4 elem_count ) 
  {
    // calculate best chunk size
    OmsTypeUInt4 cur_elem_count = getBestChunkSize( elem_count
                                                  , OmsArrayDataType::omsGetAdminDataSize()
                                                  , sizeof( T )
                                                  , OmsArraySplitPolicy< splitPolicy >::getSplitSize() 
                                                  ); 
    if( this->isTracing() )
    {
      this->getOmsHandle().dbpTrace( "OmsArrayDataImpl::getNewChunk: needed_count: %d; current_count: %d ", elem_count, cur_elem_count ); 
    }
    // create chunk with new chunk count
    return getChunkWithExactElemCount( cur_elem_count );
  }

  /*! 
   * Returns a new chunk containing storage size for the given number of elements. 
   * 
   * \param elem_count number of elements the new chunk should store
   * 
   * \return new data chunk
   */
  OmsArrayDataType* getChunkWithExactElemCount( const OmsTypeUInt4 elem_count )
  {
    return new( m_h, m_sh, m_cno, elem_count ) OmsArrayDataType( m_h, elem_count ); 
  }

  OmsHandle& getOmsHandle() const
  {
    return m_h; 
  }

  OmsArrayDataType* getTryReuseChunk( OmsArrayObjectBuffer& buf, OmsTypeUInt4 size )
  {
    bool reused; 
    return getTryReuseChunk( buf, size, reused, 0 ); 
  }


  /*! 
  * Returns a data chunk, that has a maximum capacity of the given size
  * If possible, the next chunk in the reuse buffer is used, if not
  * a new chunk is created. 
  * 
  * \param buf buffer containing data chunks
  * \param size preferred size, that is needed
  * \param reused indicates, if the chunk has been reused
  * \param drop_buf buffer for collecting dropped objects
  * 
  * \return next chunk
  */
  OmsArrayDataType* getTryReuseChunk( OmsArrayObjectBuffer& buf, OmsTypeUInt4 size, bool& reused, OmsArrayObjectBuffer* drop_buf = 0 )
  {
    OmsArrayDataType* chunk( buf.next() ); 
    if( chunk )
    {
      OmsTypeUInt4 best_chunk_size = this->getBestChunkSize( size 
        , OmsArrayDataType::omsGetAdminDataSize()
        , sizeof( T )
        , OmsArraySplitPolicy< splitPolicy >::getSplitSize()
        ); 
      if( best_chunk_size == chunk->omsGetSize() )
      {
        if( this->isTracing() )
        {
          this->getOmsHandle().dbpTrace( "OmsArrayImplBase::getTryReuseChunk(): chunk re-used; size: %d", best_chunk_size ); 
        }
        // chunk fits -> reuse chunk and store
        chunk->omsStore( this->getOmsHandle() ); 
        // return reused chunk
        return chunk; 
      } else {
        if( this->isTracing() )
        {
          this->getOmsHandle().dbpTrace( "OmsArrayImplBase::getTryReuseChunk(): no fit -> drop remaining chunks" ); 
        }
        // chunk does not fit 
        if( drop_buf )
        {
          // move last chunks to drop buffer
          buf.moveTail( *drop_buf ); 
        } else {
          // or delete them
          buf.dropTail(); 
        }
      }
    }
    if( this->isTracing() )
    {
      this->getOmsHandle().dbpTrace( "OmsArrayImplBase::getTryReuseChunk(): create new chunk; size: %d", size ); 
    }
    chunk = this->getNewChunk( size ); 
    buf.append( chunk ); 
    // return new chunk
    return chunk; 
  }

  OmsHandle& m_h; 
  OmsSchemaHandle m_sh; 
  OmsContainerNo m_cno; 
};

//==========================================================================

template< class T, int ClsGuid, OmsArraySplitPolicyEnum splitPolicy >
class OmsArrayDataImpl : public OmsArrayImplBase< T, ClsGuid, splitPolicy >
{
public: 
  typedef OmsArrayDataImpl< T, ClsGuid, splitPolicy > Self; 
  typedef OmsArrayImplBase< T, ClsGuid, splitPolicy > ParentType; 
  typedef typename ParentType::OmsArrayDataType OmsArrayDataType; 
  typedef typename ParentType::OmsArrayDataOid OmsArrayDataOid; 
  typedef typename ParentType::OmsArrayObjectBuffer OmsArrayObjectBuffer; 
  typedef typename ParentType::OmsObjectBufferEnum OmsObjectBufferEnum; 

  typedef OmsArrayDataIter< T, ClsGuid, OmsArrayObjectBuffer > OmsArrayDataIterator; 
  typedef OmsArrayDataOidIter< T, ClsGuid > OmsArrayDataOidIterator; 

  OmsArrayDataImpl( OmsHandle& h
                  , OmsSchemaHandle sh
                  , OmsContainerNo cno
                  , OmsTypeUInt4 n
                  )
                  : OmsArrayImplBase< T, ClsGuid, splitPolicy >( h, sh, cno )
                  , m_buffer( h )
  { 
    if( this->isTracing() )
    {
      h.dbpTrace( "OmsArrayDataImpl::OmsArrayDataImpl:  schema: %d, container: %d, size: %d", sh, cno, n ); 
    }

    if( n > 0 )
    {
      // create data chunks for all elements
      OmsArrayObjectBuffer& buf( *m_buffer.forUpdate() );
      createAllChunks( buf, n ); 
    }
  }

  OmsArrayDataImpl( OmsHandle& h
                  , OmsArrayDataOid oid
                  )
                  : OmsArrayImplBase< T, ClsGuid, splitPolicy >( h, 1, 1 )
                  , m_buffer( h, oid( h ) )
  {
    ClassID guid;
    OmsObjectType type; 
    h.omsOidInfo( oid, guid, type, this->m_sh, 0, 0, 0, 0, this->m_cno ); 
  }

  /*! 
   * Returns the oid of this array. 
   * 
   * \return oid of this array
   */
  OmsArrayDataOid omsGetDataOid() const
  {
    m_buffer.first(); 
    const OmsArrayDataType* obj( m_buffer() ); 
    if( obj )
    {
      return obj->omsGetOid(); 
    }
    return OmsArrayDataOid(); 
  }

  /*!
   * Returns an iterator, that is used for sequentiell access
   * 
   * \return iterator for data access
   */
  OmsArrayDataIterator omsGetData() const
  {
    return OmsArrayDataIterator( this->getOmsHandle(), m_buffer ); 
  }

  /*!
   * Sets the data for this array object by passing an array. 
   * If the array object is not new, all existing data is overwritten and the 
   * new size of the object will be set to the size of the new data passed. 
   * 
   * \param input_data enumerator with array data
   * \param n number of elements to be read from the array (new size of array)
   * \param check_data indicates, if internal validation checks should be performed (e.g. for compression)
   */
  void omsSetData( const T* data, OmsTypeUInt4 n, bool check_data = false ) 
  {
    OmsArrayEnumerator< T > input_data( data, n ); 
    omsSetData( input_data, n, check_data ); 
  }

  void omsSetData( OmsEnumerator< T >& input_data, OmsTypeUInt4 n, bool check_data )
  {
    omsSetData( input_data, n, 0, check_data ); 
  }

  /*!
   * Store data in this array object. 
   *
   * \param input_data data to be stored
   * \param n number of elements
   * \param check_data indicates, if compression algorithm has to be checked
   * \param offset
   */
  void omsSetData( OmsEnumerator< T >& input_data, OmsTypeUInt4 n, OmsTypeUInt4 offset, bool check_data )
  {
    // check, if data elements already exist
    OmsArrayObjectBuffer& buf( *m_buffer.forUpdate() ); 
    if( buf.getChunkCount() ) 
    {
      // change of element number is not allowed for plain array objects!
      if( ( n + offset ) > omsGetSize() )
        OmsArrayObjectBase::ThrowInvalidParam( (const char*)__FILE__, __LINE__ );

      buf.first(); 
      setDataReuseChunks( buf, input_data, offset ); 
    } else {
      setDataCreateChunks( buf, input_data, n ); 
    }
  }

  /*!
   * Register array data class
   */
  static void omsRegClass( OmsHandle& h )
  {
    Self::omsRegClass( h, "OmsArrayObjectPlain" ); 
  }

  static void omsRegClass( OmsHandle& h, const char* base_name )
  {
    char strbuf[ OMS_MAX_ARRAY_CLASS_NAME_LENGTH ]; 
    strbuf[ 0 ] = '\0'; 
    OmsArrayUtils::tstrcat( strbuf, sizeof( strbuf ), base_name ); 
    OmsArrayUtils::tstrcat( strbuf, sizeof( strbuf ), "_DATA" ); 
    OmsArrayDataType::omsRegClass( h
      , strbuf
      , new( h, OmsAbstractObject::USE_IN_REGISTRY_ONLY ) OmsArrayDataType() 
      ); 
  }

  /*!
   * Create container for array data. 
   */
  static void omsCreateContainer( OmsHandle& h, OmsSchemaHandle sh, OmsContainerNo cno )
  {
    h.omsCreateContainer( ClsGuid, sh, cno ); 
  }


  /*!
   * Check all oids in the given array object. 
   * Indicates, if a deref for the given oid will succeed. 
   * 
   * \param h OmsHandle 
   * \param oid OID of the array object
   * 
   * \return true, if all oids are ok, false otherwise
   */
  static bool omsCheckOid( OmsHandle& h, const OmsArrayDataOid& oid )
  {
    if( h.omsCheckOid( oid, OmsArrayDataType::omsGetGuid() ) )
    {
      const OmsArrayDataType* cur( oid( h ) ); 
      while( cur->getNextChunkOid().isTrue() )
      {
        if( h.omsCheckOid( cur->getNextChunkOid(), ClsGuid ) ) 
        {
          cur = cur->getNextChunk( h ); 
        } else {
          OmsArrayObjectBase::ThrowObjectLinkCorruption( __FILE__, __LINE__ ); 
        }
      }
      return true; 
    }
    return false; 
  }


  /*!
   * Deletes all data chunks for this array
   */
  void omsDelete() 
  {
    omsDeleteData(); 
  }

  void omsErase() 
  {
  }

  /*!
   * Releases all data chunks for this array
   */
  void omsRelease() const
  {
    m_buffer.clear(); 
  }

  /*! 
   * Deletes all data chunks, that belong to this array. 
   */
  void omsDeleteData()
  {
    OmsArrayObjectBuffer& buf( *m_buffer.forUpdate() ); 
    buf.drop(); 
  }

  /*!
   * Returns the number of elements in the array
   */
  OmsTypeUInt4 omsGetSize() const
  {
    return m_buffer.getElemCount(); 
  }

  OmsTypeUInt8 omsGetUsedBytes() const
  {
    return ( omsGetSize() * sizeof( T ) ); 
  }

  OmsTypeUInt8 omsGetReservedBytes() const
  {
    OmsTypeUInt8 size( 0 ); 
    m_buffer.first();
    const OmsArrayDataType* chunk( m_buffer() ); 
    while( chunk )
    {
      size += chunk->omsGetCapacity( this->getOmsHandle() ) * sizeof( T ); 
      chunk = chunk->getNextChunk( this->getOmsHandle() ); 
    }
    return size; 
  }

  OmsTypeUInt8 omsGetBruttoBytes() const
  {
    OmsTypeUInt8 total( 0 ); 
    m_buffer.first(); 
    const OmsArrayDataType* chunk( m_buffer() ); 
    while( chunk )
    {
      total += this->getBruttoObjSize( OmsTypeUInt4( chunk->getSizeInBytes( this->getOmsHandle() ) ) ); 
      chunk = chunk->getNextChunk( this->getOmsHandle() ); 
    }
    return total; 
  }

  /*!
   * Random access write operator
   * Sets the i-th element to a new value
   * 
   * \param idx index of value
   * \param val new value
   */
  void omsSetAt( OmsTypeUInt4 idx, const T& val )
  {
    // search chunk, that contains index ..
    OmsTypeUInt4 local_idx( idx ); 
    OmsArrayObjectBuffer& buf( *m_buffer.forUpdate() ); 
    buf.first(); 
    OmsArrayDataType* chunk( buf() ); 
    while( chunk )
    {
      if( local_idx < chunk->omsGetSize() )
      {
        // get update pointer and store chunk
        OmsArrayDataType* upd_chunk = chunk->omsForUpdPtr( this->getOmsHandle() ); 
        upd_chunk->omsStore( this->getOmsHandle() ); 
        // set value and return
        (*upd_chunk)[ local_idx ] = val; 
        return; 
      }
      local_idx -= chunk->omsGetSize(); 
      chunk = buf.next(); 
    }
    // index not found
    OmsArrayObjectBase::ThrowInvalidParam((const char*)__FILE__, __LINE__);
  }

  /*!
   * Random access read operator
   * 
   * \param idx index of element to be retrieved
   * \return const reference to i-th array element
   */
  const T& omsGetAt( OmsTypeUInt4 idx ) const
  {
    // search chunk, that contains index ..
    m_buffer.first(); 
    const OmsArrayDataType* chunk( m_buffer() ); 
    OmsTypeUInt4 local_idx( idx );
    while( chunk ) 
    {
      if( local_idx < chunk->omsGetSize() )
      {
        // return value
        return ( *chunk )[ local_idx ]; 
      }
      local_idx -= chunk->omsGetSize(); 
      chunk = chunk->getNextChunk( this->getOmsHandle() ); 
    }
    // index not found
    OmsArrayObjectBase::ThrowInvalidParam((const char*)__FILE__, __LINE__);
    // should never be reached
    return *((T*)0);
  }

  /*!
   * Returns const pointer to the first chunk in the chunk chain. 
   */
  const OmsArrayDataType* omsGetChunk() const
  {
    this->m_buffer.first(); 
    return this->m_buffer(); 
  }

  /*!
   * Returns non-const pointer to the first chunk in the chunk chain. 
   */
  OmsArrayDataType* omsGetChunkForUpd() 
  {
    OmsArrayObjectBuffer& buf( *( this->m_buffer.forUpdate() ) ); 
    buf.first(); 
    return buf(); 
  }

  /*! 
   * Return the oid of this array object
   * This is the oid of the first data chunk. 
   * 
   * \param oid of this array object
   */
  OmsArrayDataOid omsGetOid() const
  {
    m_buffer.first(); 
    const OmsArrayDataType* firstChunk( m_buffer() ); 
    if( firstChunk )
    {
      return firstChunk->omsGetOid(); 
    } else {
      return OmsArrayDataOid(); 
    }
  }

  static OmsArrayDataOidIterator omsAllOids( OmsHandle& h
                                           , OmsSchemaHandle sh
                                           , OmsContainerNo cno
                                           )
  {
    return OmsArrayDataOidIterator( h, sh, cno ); 
  }



  /*!
   * Indicates, if at least one ArrayData-object exists in any category.
   * 
   * \return true, if no data-object is found in any array-container for the given parameters
   */
  static bool omsIsEmpty( OmsHandle& h 
                        , OmsSchemaHandle sh 
                        , OmsContainerNo cno 
                        )
  {
    typename OmsArrayDataType::Iterator iter = OmsArrayDataType::omsGetSizesIterator( h, sh, cno ); 
    bool found = false; 
    while( iter )
    {
      OmsObjByClsIter< OmsArrayDataType > data_iter = iter.omsAllOids(); 
      if( data_iter )
      {
        return false; 
      }
      ++iter; 
    }

    return true; 
  }

  /*!
   * Statistic operations not implemented for plain array objects
   */
  static void omsClearStatistics( OmsHandle& h )
  { }

  static void omsAnalyzeData( OmsHandle& h
                            , OmsSchemaHandle sh
                            , OmsContainerNo cno
                            ) 
  { }

  template< class tout >
  static void omsAnalyzeData( OmsHandle& h, OmsSchemaHandle sh, OmsContainerNo cno, tout& out )
  { }


  /*! 
   * Initializes the whole array with the given value. 
   * 
   * \param value value used for initialization
   */
  void initialize( T& value ) 
  {
    OmsArrayObjectBuffer& buf( *m_buffer.forUpdate() ); 
    buf.first(); 
    OmsArrayDataType* chunk( buf() ); 
    while( chunk )
    {
      T* elem = chunk->omsBegin(); 
      for( OmsTypeUInt4 i=0; i < chunk->omsGetSize(); ++i )
      {
        *elem = value; 
      }
      chunk->omsStore( this->getOmsHandle() ); 
      chunk = chunk->getNextChunk( this->getOmsHandle() ); 
    }
  }

  const OmsArrayObjectBuffer& getBuffer() const
  {
    return m_buffer; 
  }

  /*!
   * Appends the given data elements to the current array, by creating new chunks. 
   * Chunks, that have been splitted before are not changed. 
   * 
   * \param input_data new input data
   * \param n number of elements to be appended
   */
  void omsAppend( OmsEnumerator< T >& input_data, OmsTypeUInt4 n )
  {
    OmsArrayObjectBuffer& buf( *m_buffer.forUpdate() ); 
    //TODO merge chunks, that are below a size limit
    this->fillChunksCreateRemainder( buf, input_data, n ); 
  }


  /*! 
   * Resize array to a new size
   * 
   * \param new_size new size of array
   */
  void omsResize( OmsTypeUInt4 new_size )
  {
    OmsArrayObjectBuffer& buf( *( this->getBuffer().forUpdate() ) ); 
    if( new_size )
    {
      OmsArrayDataOid old_oid( this->omsGetOid() ); 
      OmsArrayObjectBuffer drop_buffer( this->getOmsHandle() ); 
      buf.first(); 

      OmsTypeUInt4 pending_size( new_size ); 
      // create all chunks
      OmsTypeUInt4 elem_reused( 0 ); 
      // create buffers 
      while( pending_size )
      {
        bool reused; 
        OmsArrayDataType* chunk = getTryReuseChunk( this->getOmsHandle(), buf, reused, &drop_buffer ); 
        pending_size -= chunk->omsGetSize(); 
        if( reused )
        {
          elem_reused += chunk->omsGetSize(); 
        }
      }
      // copy elements, that are still in the array, but due to new chunk sizes in drop buffer
      // to newly created chunks
      OmsObjectBufferEnum drop_enum( drop_buffer ); 
      OmsTypeUInt4 elem_drop( drop_enum.getElemCount() ); 
      drop_enum.first(); 
      this->setDataReuseChunks( buf, drop_enum, elem_drop, elem_reused ); 

      // really drop old chunks
      drop_buffer.drop(); 
    } else {
      // drop all chunks, if new size is 0
      OmsArrayObjectBuffer& buf( *( this->getBuffer().forUpdate() ) ); 
      buf.drop(); 
    }
  }

protected: 

  /*! 
   * Deletes all chunks in chain starting from the given chunk
   * 
   * \param chunk start-chunk of chunk-chain to delete
   */
  void deleteChunkChain( OmsArrayDataType* chunk )
  {
    // only delete, if chunk really exists
    while( chunk )
    {
      // delete chunk and get next
      chunk->omsDelete( this->getOmsHandle() ); 
      chunk = chunk->getNextChunk( this->getOmsHandle() ); 
    }
  }

  /*!
   * Create new chunks for the given number of chunks and return 
   * the first chunk in the chunk chain. 
   * 
   * \param n the number of bytes the chunk chain should be able to store
   * 
   * \return first chunk in chain
   */
  void createAllChunks( OmsArrayObjectBuffer& buf, OmsTypeUInt4 n )
  {
    if( this->isTracing() )
    {
      this->getOmsHandle().dbpTrace( "OmsArrayDataImpl::createAllChunks(): size: %d ", n ); 
    }
    // chunks are not created -> first store operation
    OmsTypeUInt4 size_needed = n; 
    while( size_needed )
    {
      OmsArrayDataType* new_chunk = this->getNewChunk( size_needed ); 
      buf.append( new_chunk ); 
      size_needed -= new_chunk->omsGetSize(); 
    }
    // set local data pointers
    buf.first(); 
    setFirstChunk( buf() ); 
  }

  const OmsArrayObjectBuffer m_buffer; 
}; 

#endif // OMS_ARRAY_OBJECT_PLAIN_HPP
