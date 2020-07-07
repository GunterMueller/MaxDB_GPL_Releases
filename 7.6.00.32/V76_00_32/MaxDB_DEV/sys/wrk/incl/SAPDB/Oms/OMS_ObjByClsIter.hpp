/*!
 * \file    OMS_ObjByClsIter.hpp
 * \author  MarkusS
 * \brief   type-safe implementation of an oid-iterator
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


#ifndef __OMS_OBJBYCLSITER_HPP
#define __OMS_OBJBYCLSITER_HPP

#include "Oms/OMS_ObjByClsIterBase.hpp"
#include "Oms/OMS_TypedOid.hpp"

/*---------------------------------------------------------------------------*/
/// Type safe object iterator
/*! Class defines a type safe object iterator, i.e. an iterator that returns 
**  all objects which belongs to the given class.                            
**  Using the operator++ the iterators yields a sequence of the objects.     
**  First the objects which have been created in the kernel are returned,    
**  then those objects which are created in a unloaded version and finally   
**  those objects which are created in a (now) unloaded version. 
**  An instance of this class should be created using the method
**  OmsObject::omsAllOids. 
**
** \attention
** Instead of iterator(*this, false), it also is possible to write iterator()(*this, false);
** in other words, iterator() provides an OID, which is then dereferenced. This results, 
** however, in a marked deterioration in performance.
**
** \attention
** While iterating over objects, it is not allowed to create new objects belonging 
** to the same class. Furthermore it is not allowed to key an iterator beyond 
** transaction end.
** 
** \see \ref oms_OmsObjByClsIterAPI
*/
template< class T3 >
class OmsObjByClsIter : public OmsObjByClsIterBase
{
public:
  /// Constructor                                                           
  OmsObjByClsIter( OmsHandle& h ) : OmsObjByClsIterBase (h) {}
  /// returns true, if the iterator has not reached its end
  inline operator bool () const;
  /// returns the oid of the object the iterator is currently pointing to
  inline const OmsOid<T3> operator() () const;                
  /// dereferences the object the iterator is currently pointing to
  inline const T3* omsDeRef () const;
  /// dereferences the object, the iterator is currently pointing to, for update
  inline T3* omsDeRefForUpd (bool doLock) const;
  /// dereferences the object, the iterator is currently pointing to, for update
  inline const T3* omsDeRefLockShared () const;
  /// Increments the iterator
  inline void operator++ ();


  inline const T3* operator() ( OmsHandle&h ) const;         // old style
  inline T3* operator() ( OmsHandle&h, bool doLock ) const;  // old style
  //inline T3* omsDeRefForUpd (OmsHandle&h, bool doLock) const; // old style - undefined

private:
  void operator= (const OmsObjByClsIter<T3>&); // no copy
};


/*=============================================================================*/

template< class T3 >
inline
OmsObjByClsIter<T3>::operator bool () const
{ 
  return OmsObjByClsIterBase::operator bool ();
}

/*=============================================================================*/

template< class T3 >
inline
const T3* OmsObjByClsIter<T3>::operator() ( OmsHandle&h ) const 
{ 
  return REINTERPRET_CAST(T3*, OmsObjByClsIterBase::omsDeRef(false, false));
}

/*=============================================================================*/

template< class T3 >
inline
T3* OmsObjByClsIter<T3>::operator() ( OmsHandle&h, bool doLock ) const 
{ 
  return REINTERPRET_CAST(T3*, OmsObjByClsIterBase::omsDeRef(true, doLock, /*shared=*/false));
}

/*=============================================================================*/

template< class T3 >
inline
const T3* OmsObjByClsIter<T3>::omsDeRef () const 
{ 
  return REINTERPRET_CAST(T3*, OmsObjByClsIterBase::omsDeRef(false,false));
}

/*=============================================================================*/

template< class T3 >
inline
T3* OmsObjByClsIter<T3>::omsDeRefForUpd ( bool doLock ) const 
{ 
  return REINTERPRET_CAST(T3*, OmsObjByClsIterBase::omsDeRef(true, doLock, /*shared=*/false));
}

/*=============================================================================*/

template< class T3 >
inline
const T3* OmsObjByClsIter<T3>::omsDeRefLockShared () const 
{ 
  return REINTERPRET_CAST(T3*, OmsObjByClsIterBase::omsDeRef(true, /*doLock=*/true, /*shared=*/true));
}

/*=============================================================================*/

template< class T3 >
inline
void OmsObjByClsIter<T3>::operator++ () 
{
  OmsObjByClsIterBase::operator++();
}

/*=============================================================================*/

template< class T3 >
inline
const OmsOid<T3> OmsObjByClsIter<T3>::operator() () const 
{
  return *REINTERPRET_CAST(const OmsOid<T3>*, &OmsObjByClsIterBase::operator()());
}

/*=============================================================================*/

#endif // __OMS_OBJBYCLSITER_HPP
