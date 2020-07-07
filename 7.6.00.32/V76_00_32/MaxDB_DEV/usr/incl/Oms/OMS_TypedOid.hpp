/*!
 * \file    OMS_TypedOid.hpp
 * \author  IvanS, MarkusS, PeterG
 * \brief   Typed OID of the persistent objects
 */

/*

    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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

#ifndef __OMS_TYPEDOID_HPP
#define __OMS_TYPEDOID_HPP

#include "Oms/OMS_Oid.hpp"
#include "Oms/OMS_Handle.hpp"
#include "Oms/OMS_SimDumpCollector.hpp"
#include "Oms/OMS_AbstractObject.hpp"

/*-----------------------------------------------------------------------------------*/
/// type safe Oid
/*!
** \see \ref example_OmsOid
*/
template<class T>
class OmsOid : public OmsObjectId
{
public:
  typedef bool (T::* OmsQualFunc) ();

  /*-----------------------------------------------------------------------------------*/
  /// Constructors
  inline OmsOid();
  inline OmsOid( OmsHandle&h, const OmsExternalOid&r );
  inline OmsOid( OmsHandle&h, const OmsObjectId&from ); 

  /*-----------------------------------------------------------------------------------*/
  /// Constructor which considers inheritance hierarchy
  template<class T2>
  inline OmsOid(OmsOid<T2> oid) {
    // Check whether type is compatible
    T* typeCheck = reinterpret_cast<T2*>(NULL);

    // Assignment
    OmsObjectId::operator = (oid);
  }

  inline static bool omsCheckOid ( OmsHandle&, const OmsExternalOid& r );
  inline bool omsCheckOid ( OmsHandle& ) const;

  /*-----------------------------------------------------------------------------------*/
  /// Dereferences an object
  /*! \see OmsOid::omsDeRef
  */
  inline const T* operator()(OmsHandle&) const;

  /*-----------------------------------------------------------------------------------*/
  /// Dereferences an object for update for update
  /*! \see OmsOid::omsDeRefForUpd
  */
  inline T* operator()(OmsHandle&, bool doLock) const;
  
  /*-----------------------------------------------------------------------------------*/
  /// Dereferences an object
  /*!
  ** Copies the instance of type T identified by oid from the liveCache to the current context 
  ** cache. The instance provided corresponds to the "committed" state that existed at the 
  ** beginning of the requesting transaction; in other words, the instance can differ from the 
  ** state that was valid when the request was carried out. 
  ** The instance cannot be changed. 
  ** If the instance identified by the oid does not exist or is incompatible with type T, an error 
  ** of type DbpError is thrown. 
  ** This does not apply to the NULL OID, however; the NULL pointer is returned instead.
  */
  inline const T* omsDeRef(OmsHandle&) const;

  /*-----------------------------------------------------------------------------------*/
  /// Dereferences an object for update
  /*!
  ** Copies the instance of type T identified by OID from the liveCache to the current context 
  ** cache. The doLock parameter can specify whether the instance is locked before it is 
  ** copied. If this is specified, the omsLock function is executed beforehand; in other words, 
  ** the relevant rules apply. The instance provided corresponds to the "committed" state that 
  ** existed at the start of the requesting transaction; in other words, the instance can differ 
  ** from the state that was valid when the request was carried out. 
  ** After this operation has been carried out, the instance can be changed. 
  ** If the instance identified by the oid does not exist or is incompatible with type T, this 
  ** results in an error event of the type DbpError. 
  ** An error event does not occur, however, when the NULL OID is dereferenced. In this 
  ** case, the NULL pointer is returned.
  */
  inline T* omsDeRefForUpd(OmsHandle&, bool doLock=true, bool shared=false) const;

  /*-----------------------------------------------------------------------------------*/
  /// Dereferences an object and concurrently aquire a shared lock
  /*!
  ** Copies the instance of type T identified by OID from the liveCache to the current context 
  ** cache. The instance is locked in shared mode before it is copied. 
  */
  inline const T* omsDeRefLockShared(OmsHandle&) const;


  /*-----------------------------------------------------------------------------------*/
  // the following is compatibility support for old simulator
  inline void omsSimDumpToHtmlShort(OmsHandle&, OmsSimDumpCollector&, const OmsObjectId&) const;
  inline void omsSimDumpToHtmlLong(OmsHandle&, OmsSimDumpCollector&, const OmsObjectId&) const;

  /*-----------------------------------------------------------------------------------*/
  /// Assignment operator which considers inheritance hierarchy
  template<class T2>  
  inline OmsOid<T>& operator=(const OmsOid<T2>& oid)   {
    // Check whether type is compatible
    T* typeCheck = reinterpret_cast<T2*>(NULL);

    // Assignment
    OmsObjectId::operator = (oid);
    return *this;
  }
};


/*-----------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------*/


template<class T>
inline OmsOid<T>::OmsOid() : OmsObjectId() 
{}

template<class T>
inline OmsOid<T>::OmsOid( OmsHandle&h, const OmsExternalOid&r )
{
  OmsObjectId::operator=( r );
  T::omsDeRef( *this, h ); // PTS 1109694 
}

template<class T>
inline OmsOid<T>::OmsOid( OmsHandle&h, const OmsObjectId&from )
{
  OmsObjectId::operator=(T::omsCastOid(h, from));
}

template<class T>
inline bool OmsOid<T>::omsCheckOid( OmsHandle&h, const OmsExternalOid& r )
{
  return T::omsCheckOid(h, reinterpret_cast<const OmsObjectId&>(r));
}

template<class T>
inline bool OmsOid<T>::omsCheckOid( OmsHandle&h ) const
{
  return T::omsCheckOid(h, reinterpret_cast<const OmsObjectId&>(*this));
}

template<class T> 
inline  const T* OmsOid<T>::operator() ( OmsHandle& ah ) const 
{
  return T::omsDeRef( *this, ah ); 
}

template<class T> 
inline T* OmsOid<T>::operator() ( OmsHandle& ah, bool doLock) const 
{
  return T::omsDeRefForUpd( *this, ah, doLock ); 
}

template<class T> 
inline const T* OmsOid<T>::omsDeRef ( OmsHandle& ah ) const 
{
  return T::omsDeRef( *this, ah ); 
}

template<class T> 
inline T* OmsOid<T>::omsDeRefForUpd ( OmsHandle& ah, bool doLock, bool shared) const 
{
  return T::omsDeRefForUpd( *this, ah, doLock, shared ); 
}


template<class T> 
inline 
void OmsOid<T>::omsSimDumpToHtmlShort(OmsHandle &h, OmsSimDumpCollector &s, const OmsObjectId &ref) const
{
  if (!*this) {
    s.printf("NULL OID");
  } else if (this->omsCheckOid(h)) {
    const T *p = this->omsDeRef(h);
    p->omsSimDumpToHtmlShort(h, s);
  } else {
    s.printf("<B><FONT COLOR=#FF0000> Invalid OID %s </FONT></B>", 
      OmsAbstractObject::omsSimDumpMakeOidRef(*this));
    s.printf("<B><FONT COLOR=#FF0000>Ref OID %s </FONT></B>", 
      OmsAbstractObject::omsSimDumpMakeOidRef(ref));
  }
}

template<class T> 
inline 
void OmsOid<T>::omsSimDumpToHtmlLong(OmsHandle &h, OmsSimDumpCollector &s, const OmsObjectId &ref) const
{
  if (!*this) {
    s.printf("NULL OID");
  } else if (this->omsCheckOid(h)) {
    const T *p = this->omsDeRef(h);
    p->omsSimDumpToHtmlLong(h, s);
  } else {
    s.printf("<B><FONT COLOR=#FF0000> Invalid OID %s </FONT></B>", 
      OmsAbstractObject::omsSimDumpMakeOidRef(*this));
    s.printf("<B><FONT COLOR=#FF0000>Ref OID %s </FONT></B>", 
      OmsAbstractObject::omsSimDumpMakeOidRef(ref));
  }
}

#endif // __OMS_TYPEDOID_HPP
