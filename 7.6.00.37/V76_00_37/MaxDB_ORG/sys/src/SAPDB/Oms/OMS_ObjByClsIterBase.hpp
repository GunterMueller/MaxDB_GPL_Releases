/*!
 * \file    OMS_ObjByClsIterBase.hpp
 * \author  MarkusSi, Roterring
 * \brief   implementation of an oid-iterator
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


#ifndef __OMS_OBJBYCLSITERBASE_HPP
#define __OMS_OBJBYCLSITERBASE_HPP

#include "Oms/OMS_AFX.h"
#include "Oms/OMS_Oid.hpp"

class OmsAbstractObject;
class OmsObjectContainer;
class OMS_Session;
class OMS_ContainerEntry;
class OMS_KernelClsIter;
class OMS_VersionClsIter;

/*---------------------------------------------------------------------------*/
/*! Class defines an object iterator, i.e. an iterator that returns all      
**  objects which belongs to the given class.                                
**  Using the operator++ the iterators yields a sequence of the objects.     
**  First the objects which have been created in the kernel are returned,    
**  then those objects which are created in a unloaded version and finally   
**  those objects which are created in a (now) unloaded version.             
*/
/*---------------------------------------------------------------------------*/
class AFX_EXT_CLASS OmsObjByClsIterBase {

private:
  /// Variable is set to true if iterator is empty. 
  bool                 m_end;
  /// If true, then the iterator is reading oids from a version              
  bool                 m_readFromVersion;
  /// Is the iterator running on a container which contains variable sized objects 
  bool                 m_isVarObjContainer;
  /// Pointer to the current session                                         
  OMS_Session*         m_pSession;
  /// Pointer to the class meta data                                         
  OMS_ContainerEntry*  m_pContainerInfo;
  /// Pointer to the kernel iterator                                         
  OMS_KernelClsIter*   m_pKernelIter;
  /// Pointer to the version iterator                                        
  OMS_VersionClsIter*  m_pVersionIter;

public:
  /*-------------------------------------------------------------------------*/
  /// Constructor                                                            
  /*! Method constructs a OmsObjByClsIterBase instance. The iterator delivers
  **  all objects belonging to the specified class and container.            
  **  After the initialization the iterator is pointing to the first object  
  **  \param pSession pointer to the current session                          
  **  \param pContainerInfo pointer to the information about the class and   
  **         the container of the objects which should be read               
  **         the container.                                                  
  **  \param maxBufferSize Number of oids which are bufferd in the oms-layer. 
  **         This parameter is only relevant for the kernel iterator.        
  */
  OmsObjByClsIterBase(OMS_Session *pSession, OMS_ContainerEntry *pContainerInfo, int maxBufferSize);
  
  /*-------------------------------------------------------------------------*/
  /// Copy constructor.                                                      
  /* Method constructs a OmsObjByClsIterBase instance as a copy of another   
  ** OmsObjByClsIterBase instance.                                            
  ** As the result both instances share pointer the same basis-iterators.    
  ** To ensure, that those basis-iterators are only deleted when there are   
  ** no iterators pointing to then, a reference counting is used.            
  ** The copy constructor is used only for returning an iterator from a      
  ** method call.                                                            
  ** \param source The instance to be copied.                                
  */ 
  OmsObjByClsIterBase(const OmsObjByClsIterBase&); /* PTS 1110401 */

  /*-------------------------------------------------------------------------*/
  /// Destructor                                                             
  /*! Method destructs the OmsObjByClsIterBase instance and releases all     
  **  memory used by it, if no other instance is still pointing to the same  
  **  basis iterators.                                                       
  */
  ~OmsObjByClsIterBase();



protected:
  /// yields true, if the iterator has not reached its end
  operator bool () const;
  /// returns the oid of the object the iterator is currently pointing to
  const OMS_ObjectId8& operator() () const;
  /// dereferences the object the iterator is currently pointing to
  OmsAbstractObject* omsDeRef (bool forUpd, bool doLock, bool shared=false) const;
  /// dereferences the variable sized object the iterator is currently pointing to (PTS 1126047)
  const void* omsDeRefVarObj (bool doLock, bool shared=false) const;
  /// Increments the iterator
  void operator++ ();

  friend class OMS_Session;
};

#endif  // __OMS_OBJBYCLSITERBASE_HPP
