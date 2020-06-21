/*!
 * \file    OMS_ClassEntry.hpp
 * \author  MarkusSi, Roterring
 * \brief   Single entry of OMS Class Dictionary.
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

#ifndef __OMS_CLASSENTRY_HPP
#define __OMS_CLASSENTRY_HPP

#include "Oms/OMS_Defines.h"
#include "Oms/OMS_KeyDesc.hpp"
#include "Oms/OMS_ObjectContainerDef.hpp"

class OMS_AbstractObject;
class OMS_Session;
class OmsObjectContainer;
class OMS_FreeListHeader;


/// Entry (containing class properties) of the class directory.  
class OMS_ClassEntry {
public:
  OMS_ClassEntry(
    OMS_Session            *pSession,
    const char             *pClassName, 
    ClassID                 guid,
    ClassID                 baseClassGuid,
    const OMS_KeyDesc      &keyDesc,
    size_t                  objectSize, 
    void                   *pVTblPtr,
    bool                    isVarObject,
    bool                    isArrayObject);

  void           DeleteSelf();
  void* operator new(size_t sz, OMS_Session *pSession);
#if defined(OMS_PLACEMENT_DELETE)
  void  operator delete (void* p, OMS_Session *pSession);  
#endif

  ClassID                 GetBaseGuid()       const { return m_pBaseClass ? m_pBaseClass->GetGuid() : 0; }
  const char*             GetClassName()      const { return REINTERPRET_CAST(const char*, &m_className[0]); }
  ClassID                 GetGuid()           const { return m_guid; }
  const OMS_KeyDesc&      GetKeyDesc()        const { return m_keyDesc; }       // PTS 1122540
  inline unsigned char*   GetKeyPtr(OmsObjectContainer* p) const;
  size_t                  GetObjectSize()     const { return m_objectSize; }
  size_t                  GetPersistentSize() const { return m_persistentSize; }
  size_t                  GetTransientSize()  const { return m_transientSize; }
  void*                   GetVTblPtr()        const { return m_pVTblPtr; }
  bool                    IsArrayObject()     const { return m_isArrayObject; }
  bool                    IsBaseClass()       const { return m_isBaseClass; }
  bool                    IsDerivedClass()    const { return m_pBaseClass != NULL; }
  inline bool             IsDerivedClassOf(ClassID guid) const; 
  bool                    IsKeyedObject()     const { return m_keyDesc.IsKeyed(); }
  bool                    IsVarObject()       const { return m_isVarObject; }

  void                    SetBaseClass()        { m_isBaseClass = true; }

  OmsObjectContainer*     GetMemory();
  void                    ChainFree(OmsObjectContainer *&pObj, int caller);

  OMS_FreeListHeader*     GetFreeList()                      { return m_pFreeList; }
  void                    SetFreeList(OMS_FreeListHeader* p) { m_pFreeList = p;    }
  void                    ResetFreeList()                    { m_pFreeList = NULL; }

  unsigned int            GetVersion()                       { return m_version;   }

  /*! Pointer to link together entries which belong to the same hash value 
  ** (see OMS_ClassDirectory) 
  */
  OMS_ClassEntry*       m_pHashNext;

private :
  /*! Version of this entry. This version is used to check whether a container-entry is 
  **  still pointing to the right class-entry.
  */
  unsigned int          m_version;
  OMS_Session          *m_pSession;
  ///  class Guid 
  ClassID               m_guid;
  ///  Name of the class 
  tsp00_C64             m_className;
  /// Description of the key, if one exist. 
  OMS_KeyDesc           m_keyDesc;      // PTS 1122540
  /// Size of the object itself 
  size_t                m_objectSize;
  /// Transient size:  object-size + key-length + header of frame 
  size_t                m_transientSize;
  /// Persistent size: object-size + key-length - sizeof(vtbl-pointer) 
  size_t                m_persistentSize;
  /*! Pointer to the virtual table (addresses of virtual functions). The objects are stored 
  ** in the kernel without the virtual table (as this is dependent on the current memory 
  ** allocation) and so if an object is copied into the local context, this vtable-pointer 
  ** is added to the object container. This means, that all objects belonging to the same
  ** class share the same vtable. */
  void*                 m_pVTblPtr;
  /// If class is derived, then this member points to the base class entry 
  OMS_ClassEntry       *m_pBaseClass;
  /// Pointer to the context-specific freelist of object frames
  OMS_FreeListHeader   *m_pFreeList;
  /*! Class is a base class? */
  bool                  m_isBaseClass;
  /*! Object with variable length? */
  bool                  m_isVarObject;
  /*! Object is used to store arrays? */
  bool                  m_isArrayObject;

  /*! Variable is incremented each time an instance in created and this counter is used
  **  to determine the value of the member m_instance.
  */
  static unsigned int   m_versionCounter;
};


/*==========================================================================*/

inline bool OMS_ClassEntry::IsDerivedClassOf(ClassID guid) const 
{
  const OMS_ClassEntry* pBaseClass = m_pBaseClass; 
  while (NULL != pBaseClass){
    if (pBaseClass->GetGuid() == guid){
      return true;
    }
    else {
      pBaseClass = pBaseClass->m_pBaseClass;
    }
  }
  return false;
}

/*--------------------------------------------------------------------------*/

inline unsigned char* OMS_ClassEntry::GetKeyPtr(OmsObjectContainer* p) const 
{
  return (((unsigned char*) &p->m_pobj) 
         + sizeof (void*)             /* vtblptr */ 
         + GetKeyDesc().GetPos()      /* key */ 
         - 1);
}

/*----------------------------------------------------------------------*/

#endif  // __OMS_CLASSINFO_HPP