
/*!
 * \file    OMS_ArrayObjectProxy.hpp
 * \author  ReinerSi
 * \brief   Proxy implementation for array objects.
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

#ifndef OMS_ARRAY_OBJECT_PROXY_HPP
#define OMS_ARRAY_OBJECT_PROXY_HPP

#include "Oms/OMS_ArrayObjectPlain.hpp"

class OmsDefaultProxyUserObj
{ };

template< class Base, class UserObject >
class OmsArrayProxyBase : public Base
{
public: 

  const UserObject& getUserObject() const
  {
    return m_user_object; 
  }

  UserObject& getUserObject() 
  {
    return m_user_object; 
  }

  void setUserObject( const UserObject& obj )
  {
    m_user_object = obj; 
  }

private: 
  UserObject m_user_object; 
};

template < class Base >
class OmsArrayProxyBase< Base, OmsDefaultProxyUserObj > : public Base
{
};

/*!
 * This is the proxy object for arrays with invariant oid. The proxy objects stores
 * the oid of the first array data chunk and the number of elements the array contains. 
 * Optionally the proxy object can be used to store an user-defined object, that
 * has the same lifetime as the array object. The type of this user-defined object is
 * passed via template parameter. 
 * 
 */
template < class T, int ClsGuid, class UserObject >
class OmsArrayProxy : public OmsArrayProxyBase< OmsArrayHeadObject< OmsArrayProxy< T, ClsGuid, UserObject >, ClsGuid >, UserObject >
{
public: 
  typedef OmsOid< OmsArrayData< T, ClsGuid > > OmsArrayDataOid; 

  struct OmsArrayInfo {
    
    OmsArrayInfo()
      : m_size( 0 )
    { }

    OmsArrayDataOid m_oid; 
    OmsTypeUInt4 m_size; 
  };

  OmsArrayProxy( OmsTypeUInt4 version = 0 ) 
    : m_version( version )
  {  }

  void setOid( const OmsArrayDataOid& oid )  
  {
    m_info.m_oid = oid; 
  }

  const OmsArrayDataOid& getOid() const
  {
    return m_info.m_oid; 
  }

  void setSize( OmsTypeUInt4 size ) 
  {
    m_info.m_size = size; 
  }

  OmsTypeUInt4 getSize() const
  {
    return m_info.m_size; 
  }

  void setVersion( OmsTypeUInt4 version )
  {
    m_version = version; 
  }

  OmsTypeUInt4 getVersion() const
  {
    return m_version; 
  }

  const OmsArrayInfo& getInfo() const
  {
    return m_info; 
  }

  OmsArrayInfo& getInfo()
  {
    return m_info; 
  }

private: 

  OmsArrayInfo m_info;                    ///< info structure for a single array( oid + size )
  OmsTypeUInt4 m_version;                 ///< array storage version (implementation defined field) 
};

//==========================================================================

template< class T, int ClsGuid, OmsArraySplitPolicyEnum splitPolicy, class UserObject >
class OmsProxyArrayImpl : protected OmsArrayDataImpl< T, ClsGuid, splitPolicy >
{
public: 
  typedef OmsProxyArrayImpl< T, ClsGuid, splitPolicy, UserObject > Self; 
  typedef OmsArrayDataImpl< T, ClsGuid, splitPolicy > ParentType; 

  typedef OmsArrayProxy< T, ClsGuid, UserObject > OmsArrayProxyType; 
  typedef OmsOid< OmsArrayProxyType > OmsArrayProxyOid; 
  typedef typename ParentType::OmsArrayDataOid OmsArrayDataOid; 
  typedef typename ParentType::OmsArrayDataType OmsArrayDataType; 
  typedef typename ParentType::OmsArrayDataIterator OmsProxyArrayIterator; 
  typedef typename ParentType::OmsArrayObjectBuffer OmsArrayObjectBuffer; 
  typedef OmsObjByClsIter< OmsArrayProxyType > OmsArrayProxyOidIterator; 
  typedef OmsArrayImplBase< T, ClsGuid, splitPolicy > OmsArrayImplBaseType; 

  OmsProxyArrayImpl( OmsHandle& h
                   , const OmsArrayProxyOid& oid 
                   ) 
    : ParentType( h, initProxyObject( h, oid ) )
  { }

  OmsProxyArrayImpl( OmsHandle& h
                   , OmsSchemaHandle sh
                   , OmsContainerNo cno
                   , OmsTypeUInt4 n = 0
                   , OmsTypeUInt4 magic = 0
                   )
                   : ParentType( h, sh, cno, n )
  { 
    // create immediately new proxy object for new array
    OmsArrayProxyType* proxy = new( h, sh, cno ) OmsArrayProxyType( magic );
    m_const_proxy = proxy; 
    if( n )
    {
      proxy->setOid( ParentType::omsGetOid() ); 
      proxy->setSize( n ); 
    }
  }

  /*!
   * Returns the user-defined object. 
   */
  const UserObject& getUserObject() const 
  {
    return m_const_proxy->getUserObject(); 
  }

  UserObject& getUserObjectForUpd() 
  {
    OmsArrayProxyType* proxy = m_const_proxy->omsForUpdPtr( this->getOmsHandle() ); 
    proxy->omsStore( this->getOmsHandle() ); 
    return proxy->getUserObject(); 
  }

  void setUserObject( const UserObject& obj )
  {
    OmsArrayProxyType* proxy = m_const_proxy->omsForUpdPtr( this->getOmsHandle() );  
    proxy->setUserObject( obj ); 
    proxy->omsStore( this->getOmsHandle() ); 
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

  /*!
   * Store data in this array object. 
   *
   * \param input_data data to be stored
   * \param n number of elements
   * \param check_data indicates, if compression algorithm has to be checked
   */
  void omsSetData( OmsEnumerator< T >& input_data, OmsTypeUInt4 n, bool check_data ) 
  {
    if( n != m_const_proxy->getSize() )
    {
      // get lock for proxy
      OmsArrayProxyType* proxy = m_const_proxy->omsForUpdPtr( this->getOmsHandle() );
      proxy->omsStore( this->getOmsHandle() ); 

      if( n )
      {
        OmsArrayObjectBuffer& buf( *( this->getBuffer().forUpdate() ) ); 
        this->setDataTryReuseChunks( buf, input_data, n ); 
        proxy->setOid( ParentType::omsGetOid() ); 
        proxy->setSize( n ); 
      } else {
        // size is 0 -> delete existing data
        proxy->setOid( OmsArrayDataOid() ); 
        ParentType::omsDelete(); 
      }
    } else {
      // size has not changed, proxy has not to be updated, because all data fits in existing chunks
      ParentType::omsSetData( input_data, n, check_data ); 
    }
  }

  /*!
   * Returns an iterator, that is used for sequentiell access
   * 
   * \return iterator for data access
   */
  OmsProxyArrayIterator omsGetData() const
  {
    return OmsProxyArrayIterator( this->getOmsHandle(), this->getBuffer() ); 
  }


  const T& omsGetAt( OmsTypeUInt4 idx ) const
  {
    return ParentType::omsGetAt( idx ) ; 
  }

  void omsSetAt( OmsTypeUInt4 idx, const T& val ) 
  {
    ParentType::omsSetAt( idx, val ); 
  }

  void omsDelete() 
  {
    OmsArrayProxyType* proxy = m_const_proxy->omsForUpdPtr( this->getOmsHandle() ); 
    proxy->omsDelete( this->getOmsHandle() ); 
    ParentType::omsDelete(); 
  }

  void omsRelease() const 
  {
    this->getOmsHandle().omsRelease( m_const_proxy->omsGetOid() ); 
    ParentType::omsRelease(); 
  }

  OmsTypeUInt4 omsGetSize() const
  {
    return m_const_proxy->getSize(); 
  }

  /*! 
   * Return the oid of the proxy object
   * 
   * \return oid of the proxy object
   */
  OmsArrayProxyOid omsGetOid() const
  {
    return m_const_proxy->omsGetOid(); 
  }

  /*!
   * Returns const pointer to the first chunk in the chunk chain. 
   */
  const OmsArrayDataType* omsGetChunk() const
  {
    return ParentType::omsGetChunk(); 
  }

  /*!
   * Returns non-const pointer to the first chunk in the chunk chain. 
   */
  OmsArrayDataType* omsGetChunkForUpd() 
  {
    return ParentType::omsGetChunkForUpd(); 
  }

  OmsTypeUInt8 omsGetBruttoBytes() const
  {
    return ( this->getBruttoObjSize( sizeof( OmsArrayProxyType ) ) + ParentType::omsGetBruttoBytes() ); 
  }

  OmsTypeUInt8 omsGetUsedBytes() const
  {
    return ( sizeof( OmsArrayProxyType ) + ParentType::omsGetUsedBytes() ); 
  }

  OmsTypeUInt8 omsGetReservedBytes() const
  {
    return ( this->getBruttoObjSize( sizeof( OmsArrayProxyType ) ) + ParentType::omsGetReservedBytes() ); 
  }

  /*!
   * Returns an iterator, that gives access to all proxy objects in
   * the given container. 
   */
  static OmsArrayProxyOidIterator omsAllOids( OmsHandle& h
                                            , OmsSchemaHandle sh
                                            , OmsContainerNo cno
                                            )
  {
    return OmsArrayProxyType::omsAllOids( h, sh, cno ); 
  }


  static bool omsIsEmpty( OmsHandle& h
                        , OmsSchemaHandle sh
                        , OmsContainerNo cno 
                        ) 
  {
    // no proxy array exists, if no proxy object exists
    // array data without proxy should never happen
    OmsArrayProxyOidIterator iter = omsAllOids( h, sh, cno ); 
    return ( ! iter ); 
  }

  /*! 
   * Analyze operations have no effect on proxy based arrays. 
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


  static void omsRegClass( OmsHandle& h )
  {
    Self::omsRegClass( h, "OmsArrayObjectProxy" );
  }

  static void omsRegClass( OmsHandle& h, const char* base_name )
  {
    char strbuf[ OMS_MAX_ARRAY_CLASS_NAME_LENGTH ]; 
    strbuf[ 0 ] = '\0'; 
    OmsArrayUtils::tstrcat( strbuf, sizeof( strbuf ), base_name ); 
    OmsArrayUtils::tstrcat( strbuf, sizeof( strbuf ), "_PROXY" ); 

    ParentType::omsRegClass( h, base_name ); 
    OmsArrayProxyType::omsRegClass( h
                                  , strbuf
                                  , new( h, OmsAbstractObject::USE_IN_REGISTRY_ONLY ) OmsArrayProxyType() 
                                  );
  }

  static void omsCreateContainer( OmsHandle& h, OmsSchemaHandle sh, OmsContainerNo cno )
  {
    ParentType::omsCreateContainer( h, sh, cno ); 
    h.omsCreateContainer( OmsArrayProxyType::omsGetGuid(), sh, cno ); 
  }

  static bool omsCheckOid( OmsHandle& h, const OmsArrayProxyOid& oid )
  {
    if( h.omsCheckOid( oid, OmsArrayProxyType::omsGetGuid() ) )
    {
      const OmsArrayProxyType* proxy = oid( h ); 
      return ParentType::omsCheckOid( h, proxy->getOid() ); 
    }
    return false; 
  }

  virtual ~OmsProxyArrayImpl() 
  { }

protected: 

  /*!
   * Initializes the local const proxy object with the given oid.
   * The const data chunk is also initialized, if the oid in the proxy object is
   * not nil. 
   * 
   * \param h OmsHandle used for initialization
   * \oid oid of proxy object 
   */
  const OmsArrayDataOid& initProxyObject( OmsHandle& h, const OmsArrayProxyOid& oid ) 
  {
    m_const_proxy = oid( h );
    ClassID guid;
    OmsObjectType type; 
    h.omsOidInfo( oid, guid, type, this->m_sh, 0, 0, 0, 0, this->m_cno ); 
    return m_const_proxy->getOid(); 
  }

  const OmsArrayProxyType* m_const_proxy;  ///< oid of the proxy object
};

#endif // #ifndef OMS_ARRAY_OBJECT_PROXY_HPP