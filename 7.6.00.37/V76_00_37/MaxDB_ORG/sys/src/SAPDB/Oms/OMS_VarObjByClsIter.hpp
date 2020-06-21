/*!
 * \file    OMS_VarObjByClsIter.hpp
 * \author  MarkusSi, Roterring
 * \brief   implementation of an oid-iterator for variable sized objects
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


#ifndef __OMS_VAROBJBYCLSITER_HPP
#define __OMS_VAROBJBYCLSITER_HPP

#include "Oms/OMS_ObjByClsIterBase.hpp"
#include "Oms/OMS_TypedOid.hpp"

/*---------------------------------------------------------------------------*/
/// Iterator for variable sized objects 
/*! Class defines a object iterator, i.e. an iterator that returns 
**  all variable sized objects which belongs to the given class and container.                            
**  Using the operator++ the iterators yields a sequence of the objects.     
**  First the objects which have been created in the kernel are returned,    
**  then those objects which are created in a unloaded version and finally   
**  those objects which are created in a (now) unloaded version. 
**  An instance of this class should be created using the method
**  OmsObject::omsAllVarOids. 
**
** \attention
** Instead of iterator.omsDeref, it also is possible to write iterator()(*this, false);
** in other words, iterator() provides an OID, which is then dereferenced. This results, 
** however, in a marked deterioration in performance.
**
** \attention
** While iterating over objects, it is not allowed to create new objects belonging 
** to the same class. Furthermore it is not allowed to keep an iterator beyond 
** transaction end.
** 
** \see \ref oms_OmsObjByClsIterAPI
**
** \since PTS 1126047
*/
class OmsVarObjByClsIter : public OmsObjByClsIterBase
{
public:
  /// Constructor                                                           
  OmsVarObjByClsIter(OMS_Session *pSession, OMS_ContainerEntry *pContainerInfo, int maxBufferSize) 
    : OmsObjByClsIterBase (pSession, pContainerInfo, maxBufferSize) 
  {}
  /// returns true, if the iterator has not reached its end
  inline operator bool () const;
  /// returns the oid of the object the iterator is currently pointing to
  inline const OmsVarOid operator() () const;                
  /// dereferences the object the iterator is currently pointing to
  inline const void* omsDeRef () const;
  /// Increments the iterator
  inline void operator++ ();

private:
  void operator= (const OmsVarObjByClsIter&); // no copy
};


/*=============================================================================*/

inline
OmsVarObjByClsIter::operator bool () const
{ 
  return OmsObjByClsIterBase::operator bool ();
}

/*=============================================================================*/

inline
const void* OmsVarObjByClsIter::omsDeRef () const 
{ 
  return OmsObjByClsIterBase::omsDeRefVarObj(false);
}

/*=============================================================================*/

inline
void OmsVarObjByClsIter::operator++ () 
{
  OmsObjByClsIterBase::operator++();
}

/*=============================================================================*/

inline
const OmsVarOid OmsVarObjByClsIter::operator() () const 
{
  return *REINTERPRET_CAST(const OmsVarOid*, &OmsObjByClsIterBase::operator()());
}

/*=============================================================================*/

#endif // __OMS_VAROBJBYCLSITER_HPP
