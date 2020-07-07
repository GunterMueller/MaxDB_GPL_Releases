/*!
 * \file    OMS_ContainerDirectoryDef.hpp
 * \author  MarkusSi, Roterring
 * \brief   Container Directory.
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
#ifndef __OMS_CONTAINERDIRECTORYDEF_HPP
#define __OMS_CONTAINERDIRECTORYDEF_HPP

#include "Oms/OMS_ContainerHandle.hpp"  
#include "Oms/OMS_IGuidHash.hpp"

//class OMS_DumpInterface;
class OMS_Context;
class OMS_Session;
class OMS_ContainerEntry;
class OMS_FreeListHeader;
class OmsObjectContainer;

/// Local container dictionary
class OMS_ContainerDirectory : public OMS_IGuidHash {
private:
  /*! Current size of the hash array. Starts with the size of HEAD_ENTRIES (= 257) */
  tsp00_Int4              m_headentries;
  /*! number of containers registered in local container directory */
  tsp00_Int4              m_count;
  /*! Hash array with pointers to the container entries which uses the container handle 
  ** to compute the hash value. */
  OMS_ContainerEntry    **m_ppContHandleHead;
  /*! Hash array with pointers to the container entries which uses the GUID, the schema, 
  ** and the container number to compute the hash value. */
  OMS_ContainerEntry    **m_ppContIdHead;
  /*! Backward-pointer to the context, in which the instance was created */
  OMS_Context            *m_pContext;
  /*! Starting point of a list with the headers for free-lists. For each size of object 
  ** container a header is inserted into this list and below of this header all free 
  ** object containers are linked. As the size of the object container is different for 
  ** objects which are created in a version and those which are created in a normal context, 
  ** there might be two headers per class. If several classes have the same size, then they 
  ** use a shared free-list. */
  OMS_FreeListHeader     *m_pFreeListHeader;
  /*! Member is used to prevent the change of the class-entry pointer in certain situations, e.g. it is not
  **  allowed, that this pointer is changed from NULL to another value after the version has been unbound
  ** from a session.
  */
  bool                    m_allowChangeOfClassEntryPtr;

public:
  enum { HEAD_ENTRIES=257 }; 

  class Iter {
    friend class OMS_ContainerDirectory;

  private:
    OMS_ContainerDirectory *m_pContainerDir;
    long                    m_headIndex;
    OMS_ContainerEntry     *m_pCurr;

  public :
    inline Iter(OMS_ContainerDirectory*);
    inline Iter(const Iter&);
    inline void operator++();
    inline operator bool() const;
    inline OMS_ContainerEntry* operator()() const;
  };

  OMS_ContainerDirectory  ();
  ~OMS_ContainerDirectory ();

  void                       Create(OMS_Context*  pContext);
  void                       UnregisterAll();
  
  inline Iter                First ();

  OMS_ContainerEntry*        AutoRegisterArrayContainer(ClassID guid, OmsSchemaHandle Schema, OmsContainerNo ContainerNo, int arrayByteSize);
  void                       Delete (OMS_ContainerEntry *pContainerInfo, bool deleteEntry=true);
  void                       DeleteAllObjects (ClassID guid, OmsSchemaHandle schema,OmsContainerNo cno);
  void                       DropSchema (OmsSchemaHandle Schema); 
  void                       DropContainer (ClassID guid, OmsSchemaHandle schema,OmsContainerNo cno);
  bool                       ExistsContainer (ClassID guid, OmsSchemaHandle schema, OmsContainerNo cno);
  bool                       ExistsSubContainer (ClassID guid, OmsSchemaHandle schema, OmsContainerNo cno, short category);
  /// Creates a container in the kernel (if not already existing) and an entry in the local directory
  int                        CreateContainer (ClassID guid, OmsSchemaHandle schema, OmsContainerNo containerNo,
                                              bool useCachedKeys, bool partitionedKeys, bool noWait);
  inline OMS_ContainerEntry* GetContainerEntry(ClassID guid, OmsSchemaHandle schema, OmsContainerNo containerNo);
  inline OMS_ContainerEntry* GetContainerEntry(ClassID baseGuid, OmsSchemaHandle schema, OmsContainerNo containerNo, int arrayByteSize);  
  inline OMS_ContainerEntry* GetContainerEntry(OMS_ContainerHandle &containerHandle);
  void                       RemoveDroppedContainers(); 
  void                       CleanupAfterTransEnd(bool isCommit);
  
  void                       CommitDropContainer(OMS_ContainerHandle *pContainerHandle);
  void                       CommitCreateContainer(OMS_ContainerHandle *pContainerHandle);
  void                       RollbackDropContainer(OMS_ContainerHandle *pContainerHandle);
  void                       RollbackCreateContainer(OMS_ContainerHandle *pContainerHandle);

  void                       ClearCachedKeys();    
  void                       ClearReusableOids();   
  void                       ClearNewVersionObjects();

  void                       ThrowUnknownContainer(OMS_ContainerEntry *pContainerEntry, const char *pFile, unsigned int line);
  void                       ThrowUnknownContainer(ClassID guid, OmsSchemaHandle schema, OmsContainerNo containerNo, const char *pFile, unsigned int line);
  void                       ThrowUnknownContainer(OMS_ContainerHandle &containerHandle, const char *pFile, unsigned int line); 

  OmsObjectContainer*        GetMemory(size_t size);
  void                       ChainFree(OmsObjectContainer *&pObj, size_t size, int caller);
  OMS_FreeListHeader*        GetFreeList(size_t size);
  void                       DeleteFreeList(int caller);
  void                       ClearFreeList(int caller);

  void                       ResetNewObjectsToFlush();

  void AllowChangeOfClassEntryPtr()          { m_allowChangeOfClassEntryPtr = true;  }
  void ForbitChangeOfClassEntryPtr()         { m_allowChangeOfClassEntryPtr = false; }
  bool IsChangeOfClassEntryPtrAllowed()const { return m_allowChangeOfClassEntryPtr;  }

  /// Checks whether there are containers in the container-directory which belong to not-registered classes in this session
  bool CheckForUnregisteredClasses(OMS_Session *pSession);

private:
  /// Creates a local entry if the container does already exist in the kernel
  OMS_ContainerEntry*        AutoRegisterContainer (ClassID guid, OmsSchemaHandle sh, OmsContainerNo ContainerNo, short &error);
  OMS_ContainerEntry*        AutoRegisterContainer (OMS_ContainerHandle &containerHandle); 
  void                       DropPhysicalContainer (ClassID guid, OmsSchemaHandle schema,OmsContainerNo cno, bool kernelUpdate=true);
  inline OMS_ContainerEntry* Find(OMS_ContainerHandle containerHandle, bool checkDropFlag = true);
  inline OMS_ContainerEntry* Find(ClassID guid, OmsSchemaHandle schema, OmsContainerNo containerNo, bool checkDropFlag = true);
  void                       Insert (OMS_ContainerEntry *pContainerEntry);
  void                       Resize();


  friend class OMS_ContainerDirectory::Iter;
};

#endif // __OMS_CONTAINERDIRECTORYDEF_HPP