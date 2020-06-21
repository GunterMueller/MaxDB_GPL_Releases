/*!
 * \file    OMS_ClassDirectory.hpp
 * \author  MarkusSi, Roterring
 * \brief   OMS Class Dictionary.
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
#ifndef __OMS_CLASSDIRECTORY_HPP
#define __OMS_CLASSDIRECTORY_HPP

#include "Oms/OMS_IGuidHash.hpp"
#include "Oms/OMS_ClassEntry.hpp"
#include "Oms/OMS_KeyDesc.hpp"
#include "Oms/OMS_Globals.hpp"
#include "Oms/OMS_DbpError.hpp"
#include "hsp77.h"

class OMS_Session;
class OMS_FreeListHeader;
class OMS_ClassEntry;


class OMS_ClassDirectory : public OMS_IGuidHash {
public:
  enum {HEAD_ENTRIES=51};

  class Iter {
    friend class OMS_ClassDirectory;

  private:
    OMS_ClassDirectory *m_pClassDir;
    long                m_headIndex;
    OMS_ClassEntry     *m_pCurr;

  public :
    inline Iter(OMS_ClassDirectory*);
    inline Iter(const Iter&);
    inline void operator++();
    inline operator bool() const;
    inline OMS_ClassEntry* operator()() const;
  };

  OMS_ClassDirectory();
  ~OMS_ClassDirectory();

  void                   Create(OMS_Session* pSession);
  OMS_ClassEntry*        RegisterClass (
                            const char             *pClassName, 
                            ClassID                 guid,
                            ClassID                 baseClassGuid,
                            const OMS_KeyDesc      &keyDesc,
                            size_t                  objectSize, 
                            void                   *pVTblPtr,
                            bool                    isVarObject,
                            bool                    isArrayObject);
  inline OMS_ClassEntry* Find(ClassID guid);
  inline OMS_ClassEntry* GetClassEntry(ClassID guid); 

  bool                   IsBaseClassOf(ClassID, ClassID);
  bool                   IsDerivedClassOf(ClassID , ClassID);

  void                   CleanupAfterContextChange();

  OMS_ClassDirectory::Iter  First(); 

private :
  OMS_ClassEntry*  AutoRegisterSubClass(ClassID guid);

  OMS_Session    *m_pSession;
  OMS_ClassEntry *m_head[HEAD_ENTRIES];

  void  Insert(OMS_ClassEntry *pClassEntry);

friend class OMS_ClassDirectory::Iter;
};



/*----------------------------------------------------------------------*/
 
inline OMS_ClassEntry* OMS_ClassDirectory::Find(ClassID guid) 
{
  int slot = HashValue(guid, HEAD_ENTRIES);
  OMS_ClassEntry *p = m_head[slot];
  while (NULL != p) {
    if (p->GetGuid() == guid) {
      return p;
    }
    p = p->m_pHashNext;
  }
  return NULL;
}

/*----------------------------------------------------------------------*/
 
inline OMS_ClassEntry* OMS_ClassDirectory::GetClassEntry(ClassID guid) 
{
  OMS_ClassEntry *pClassInfo = Find(guid);
  if (pClassInfo == NULL){
    if (!omsIsArrayGuid(guid)){
      char buf[256];
      sp77sprintf(buf, sizeof(buf), "GetClassEntry: Class (GUID=%d) not registered in current session", guid);
      OMS_Globals::Throw(e_unknown_guid, buf, __MY_FILE__, __LINE__); 
    }
    else {
      // Check whether basis class exists and if so register sub-class
      pClassInfo = AutoRegisterSubClass(guid);
    }
  }
  
  return pClassInfo;
}

/*----------------------------------------------------------------------*/

#endif // __OMS_CLASSDIRECTORY_HPP
