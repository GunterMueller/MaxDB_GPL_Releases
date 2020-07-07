/*!
 * \file    OMS_KeyRangeIter.hpp
 * \author  MarkusS
 * \brief   type safe interface of a key range iterator
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


#ifndef __OMS_KEYRANGEITER_HPP
#define __OMS_KEYRANGEITER_HPP

#include "Oms/OMS_ObjByKeyIterBase.hpp"

/*---------------------------------------------------------------------------*/
/// Type safe key iterator
/*! Class defines a type safe interface of a key range iterator, i.e. an     
**  iterator that returns keyed objects with keys >= a start key and         
**  <= a stop key.                                                           
**  Using the operator++ the iterators yields a sequence of keyed objects,   
**  that is sorted in ascending order by the key using memcmp for comparism. 
**  Analogous using the operator-- the objects are returned in descending    
**  order.                                                                   
**  It is also possible to mix these operators.  
**  An instance of this class should be created using the methods 
**  OmsKeyedObject<Obj,Key>::omsFirstKey or OmsKeyedObject<Obj,Key>::omsLastKey. 
**
** \attention
** While iterating over objects, it is not allowed to create new objects belonging 
** to the same class. Furthermore it is not allowed to key an iterator beyond 
** transaction end.
** 
** \see \ref oms_OmsKeyRangeIterAPI
** \see \ref example_omsKeyedObject
*/
template< class T >
class OmsKeyRangeIter : public OmsObjByKeyIterBase
{
public:
  /*-------------------------------------------------------------------------*/
  /// operator bool                                                          
  /*! Method returns true, if the iterator points to an keyed object;        
  **  otherwise false.                                                       
  */
  inline operator bool () const;

  /*-------------------------------------------------------------------------*/
  /// Dereferences the object the iterator is currently pointing to.         
  /*! Method returns a pointer to the keyed object the iterator is pointing  
  **  to. If operator bool returns false, then result is undefined.          
  **  \return Pointer to the current keyed object.                           
  */  
  inline const T* omsDeRef () const;

  /*-------------------------------------------------------------------------*/
  /// Dereferences for update the object the iterator is pointing to.        
  /*! Method returns a pointer to the keyed object the iterator is pointing  
  **  to. If operator bool returns false, then result is undefined.          
  **  \param doLock If true, the current keyed object is locked.             
  **  \param shared Aquire shared or exclusive lock?
  **  \return Pointer to the current keyed object.                           
  */  
  inline T* omsDeRefForUpd (bool doLock) const;

  /*-------------------------------------------------------------------------*/
  /// Dereferences for update the object the iterator is pointing to.        
  /*! Method returns a pointer to the keyed object the iterator is pointing  
  **  to. If operator bool returns false, then result is undefined.          
  **  \param doLock If true, the current keyed object is locked.             
  **  \param shared Aquire shared or exclusive lock?
  **  \return Pointer to the current keyed object.                           
  */  
  inline const T* omsDeRefLockShared () const;

  /*-------------------------------------------------------------------------*/
  /// Increment                                                              
  /*! Method steps to the next largest keyed object considering the ordering  
  **  defined by the method memcmp.                                                  
  */
  inline void operator++ ();

  /*-------------------------------------------------------------------------*/
  /// Decrement                                                              
  /*! Method steps to the next smallest keyed object considering the ordering  
  **  defined by the method memcmp.                                                  
  **  \since PTS 1119480                                                     
  */
  inline void operator-- ();  

private:
  OmsKeyRangeIter() {}
  void operator= (const OmsKeyRangeIter<T>&); // no copy
};

/*============================================================================*/

template< class T >
inline
OmsKeyRangeIter<T>::operator bool () const
{
  return OmsObjByKeyIterBase::operator bool ();
}

/*============================================================================*/

template< class T >
inline
const T* OmsKeyRangeIter<T>::omsDeRef () const
{
  return REINTERPRET_CAST(T*, OmsObjByKeyIterBase::omsDeRef(false, false));
}

/*============================================================================*/

template< class T >
inline
T* OmsKeyRangeIter<T>::omsDeRefForUpd (bool doLock) const
{
  return REINTERPRET_CAST(T*, OmsObjByKeyIterBase::omsDeRef(true, doLock, /*shared=*/false));
}

/*============================================================================*/

template< class T >
inline
const T* OmsKeyRangeIter<T>::omsDeRefLockShared () const
{
  return REINTERPRET_CAST(T*, OmsObjByKeyIterBase::omsDeRef(true, /*doLock=*/true, /*shared=*/true));
}

/*============================================================================*/

template< class T >
inline
void OmsKeyRangeIter<T>::operator++ ()
{
  OmsObjByKeyIterBase::operator++();
}

/*============================================================================*/

template< class T >   
inline
void OmsKeyRangeIter<T>::operator-- ()
{
  OmsObjByKeyIterBase::operator--();
}

/*============================================================================*/

#endif // __OMS_KEYRANGEITER_HPP