/*!
 * \file    OMS_BeforeImageList.hpp
 * \author  MarkusSi, Roterring
 * \brief   Handling of BeforeImages.
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
#ifndef __OMS_BEFOREIMAGELIST_HPP
#define __OMS_BEFOREIMAGELIST_HPP

#define OMS_BEFOREIMAGE_MAX_SUBTRANS_LEVEL 32

class OMS_DumpInterface;
class OMS_Context;
class OMS_Session;
class OMS_ContainerEntry;
class OmsObjectContainer;
class OMS_ContainerHandle;

class OMS_BeforeImageList {
public :
  OMS_BeforeImageList ();
  ~OMS_BeforeImageList();
  void Dump(OMS_DumpInterface& dumpObj) const;
  void freeBeforeImages     (int subtransLvl);
  void init                 (OMS_Session* c);
  void insertBeforeImage    (OmsObjectContainer*, int subtransLvl);
  void insertNewBeforeImage (OmsObjectContainer*, OMS_ContainerEntry* clsinfo, int subtransLvl);
  void insertDropContainerBeforeImage   (const OMS_ContainerHandle *pContainerHandle, OMS_ContainerEntry *pContainerEntry, int subtransLvl) ;
  void insertCreateContainerBeforeImage (const OMS_ContainerHandle *pContainerHandle, OMS_ContainerEntry *pContainerEntry, int subtransLvl) ;
  void removeContext        (OMS_Context* pContext);
  void subtransRollback     (int subtransLvl);
  void subtransCommit       (int subtransLvl);

  /// Check if all updated objects have been stored (in simulator)
  void checkStores          ();

private :
  OmsObjectContainer* m_beforeImages[OMS_BEFOREIMAGE_MAX_SUBTRANS_LEVEL]; 
  OMS_Session*        m_session;
};

#endif // __OMS_BEFOREIMAGELIST_HPP
