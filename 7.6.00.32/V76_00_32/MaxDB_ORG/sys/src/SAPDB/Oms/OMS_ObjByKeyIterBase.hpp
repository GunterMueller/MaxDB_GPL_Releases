/*!
 * \file    OMS_ObjByKeyIterBase.hpp
 * \author  MarkusSi, Roterring
 * \brief   implementation of key range iterator
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


#ifndef __OMS_OBJBYKEYITERBASE_HPP
#define __OMS_OBJBYKEYITERBASE_HPP

#include "Oms/OMS_AFX.h"
#include "Oms/OMS_GUID.hpp"
#include "Oms/OMS_IterDirection.hpp"
#include "livecachetypes.h" // nocheck

class  OmsAbstractObject;
class  OmsHandle;
class  OMS_KernelVersionMergeKeyIter;

/*---------------------------------------------------------------------------*/
/*! Class defines a key range iterator, i.e. an iterator that returns        
**  keyed objects with keys >= a start key and <= a stop key.                
**  Using the operator++ the iterators yields a sequence of keyed objects,   
**  that is sorted in ascending order by the key using memcmp for comparism. 
**  Analogous using the operator-- the objects are returned in descending    
**  order.                                                                   
**  It is also possible to mix these operators.                              
*/
class AFX_EXT_CLASS OmsObjByKeyIterBase 
{
  friend class OmsHandle;

public:
  /*-------------------------------------------------------------------------*/
  /// Copy constructor.                                                      
  /* Method constructs a OmsObjByKeyIterBase instance as a copy of another   
  ** OmsObjByKeyIterBase instance.                                            
  ** As the result both instances share the same m_pBody.                    
  ** The copy constructor is used only for returning an iterator from a      
  ** method call.                                                            
  ** \param source The instance to be copied.                                
  */ 
  OmsObjByKeyIterBase(const OmsObjByKeyIterBase& source);

  /*-------------------------------------------------------------------------*/
  /// Constructor                                                            
  /*! Method constructs a OmsObjByKeyIterBase instance. The iterator delivers
  **  objects which lie in between the interval which is represented by the  
  **  lower and the upper key. After the initialization the iterator         
  **  is pointing to the object which corresponds to the lower key.          
  **  \param handle the OmsHandle instance of the caller.                    
  **  \param guid the guid of the persistent keyed object, this is a part of  
  **         the container identification of the container to be traversed.  
  **  \param Schema The schema identification of the schema containing       
  **         the container.                                                  
  **  \param ContainerNo The container number of the container to be         
  **         traversed.                                                      
  **  \param pLowerKey A pointer to the lower bound key of the range. This   
  **         key is used as start key, too.                                  
  **  \param pUpperKey A pointer to the upper bound key of the range.        
  **  \param maxBufferSize Number of oids which are bufferd in the oms-layer. 
  **         This parameter is only relevant for the kernel iterator.        
  */
  OmsObjByKeyIterBase
    (
    OmsHandle&        handle, 
    ClassID           guid, 
    OmsSchemaHandle   Schema, 
    OmsContainerNo    ContainerNo,
    const void*       pLowerKey,
    const void*       pUpperKey,
    int               maxBufferSize
    );

  /*-------------------------------------------------------------------------*/
  /// Constructor                                                            
  /*! Method constructs a OmsObjByKeyIterBase instance. The iterator delivers
  **  objects which lie in between the interval which is represented by the  
  **  lower and the upper key. After the initialization the iterator         
  **  is pointing to the object which corresponds to the lower key.          
  **  \param handle the OmsHandle instance of the caller.                    
  **  \param guid the guid of the persistent keyed object, this is a part of  
  **         the container identification of the container to be traversed.  
  **  \param Schema The schema identification of the schema containing       
  **         the container.                                                  
  **  \param ContainerNo The container number of the container to be         
  **         traversed.                                                      
  **  \param pStartKey A pointer to the start key.                            
  **  \param pLowerKey A pointer to the lower bound key of the range.        
  **  \param pUpperKey A pointer to the upper bound key of the range.        
  **  \param direction If there is no object with a key equal to the start   
  **         key, then this parameter determines whether the iterator points 
  **         to the next largest (OMS_ASCENDING) or the next smallest        
  **         (OMS_DESCENDING) object.                                        
  **  \param maxBufferSize Number of oids which are bufferd in the oms-layer. 
  **         This parameter is only relevant for the kernel iterator.        
  **  \since PTS 1119480                                                     
  */
  OmsObjByKeyIterBase
    (
    OmsHandle&        handle, 
    ClassID           guid, 
    OmsSchemaHandle   Schema, 
    OmsContainerNo    ContainerNo,
    const void*       pStartKey,
    const void*       pLowerKey,
    const void*       pUpperKey,
    OmsIterDirection  direction,
    int               maxBufferSize
    );

  /*-------------------------------------------------------------------------*/
  /// Destructor                                                             
  /*! Method destructs the OmsObjByKeyIterBase instance and releases all     
  **  memory used by it.                                                     
  */
  ~OmsObjByKeyIterBase();

protected:
  OmsObjByKeyIterBase() {}  // required to compile properly with GCC

  /*-------------------------------------------------------------------------*/
  /// operator bool                                                          
  /*! Method yields true, if the iterator points to an keyed object;         
  **  otherwise false.                                                       
  */
  operator bool () const;

  /*-------------------------------------------------------------------------*/
  /// Dereferences the object the iterator is currently pointing to.         
  /*! Method returns a pointer to the keyed object the iterator is pointing  
  **  to. If operator bool returns false, then result is undefined.          
  **  \param forUpd If true, the returned keyed object may be updated by the 
  **         caller.                                                         
  **  \param doLock If true, the current keyed object is locked. 
  **  \param shared Aquire shared or exclusive lock?
  **  \return Pointer to the current keyed object.                           
  */
  OmsAbstractObject* omsDeRef(bool forUpd, bool doLock, bool shared=false) const;

  /*-------------------------------------------------------------------------*/
  /// Increment                                                              
  /*! Method steps to the next keyed object considering the ordering defined 
  **  by the method memcmp.                                                  
  */
  void operator++ ();

  /*-------------------------------------------------------------------------*/
  /// Decrement                                                              
  /*! Method steps to the previous keyed object considering the ordering     
  **  defined by the method memcmp.                                          
  **  \since PTS 1119480                                                     
  */
  void operator-- ();   


private:
  OMS_KernelVersionMergeKeyIter* m_pIterator;
};

#endif  // __OMS_OBJBYKEYITERBASE_HPP