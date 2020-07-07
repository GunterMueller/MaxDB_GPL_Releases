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
#include "Oms/OMS_UnloadableVersionList.hpp"
#include "Oms/OMS_Session.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"

OMS_UnloadableVersionList::OMS_UnloadableVersionList()
  : m_first(NULL)
{
}

/*---------------------------------------------------------------------------*/

void OMS_UnloadableVersionList::InsertVersion(OMS_Context* pVersion)
{
  OMS_Context* curr = m_first;
  while (curr)
  {
    if (curr == pVersion)
    {
      char versionId[sizeof(OmsVersionId)+1];
      SAPDB_MemCopyNoCheck(&versionId[0], &pVersion->GetVersionId()[0], sizeof(OmsVersionId));
      versionId[sizeof(OmsVersionId)] = 0;

      DbpBase opMsg(OMS_Globals::GetCurrentLcSink());
      opMsg.dbpOpError("OMS_UnloadableVersionList::InsertVersion: ignore duplicate insert of version %s", &versionId[0]);
      return;
    }
    curr = curr->GetNextUnloaded();
  }
  pVersion->SetNextUnloaded(m_first);
  m_first = pVersion;
}

/*---------------------------------------------------------------------------*/

OMS_Context* OMS_UnloadableVersionList::RemoveVersion(OMS_Context* pVersion)
{
  OMS_Context* prev = NULL;
  OMS_Context* curr = m_first;
  while (NULL != curr)
  {
    if ((curr == pVersion) ||
      ((NULL == pVersion) && (NULL == curr->GetNextUnloaded()))) // remove last
    {
      if (NULL == prev)
      {
        m_first = curr->GetNextUnloaded();
      }
      else
      {
        prev->SetNextUnloaded(curr->GetNextUnloaded());
      }
      return curr;
    }
    else
    {
      prev = curr;
      curr = curr->GetNextUnloaded();
    }
  }
  return NULL;
}

/*---------------------------------------------------------------------------*/
 
bool OMS_UnloadableVersionList::ExistsVersion(OMS_Context* pVersion)
{
  OMS_Context* curr = m_first;
  while (curr){
    if (curr == pVersion){
      return true;
    }
    curr = curr->GetNextUnloaded();
  }
  return false;
}

/*---------------------------------------------------------------------------*/
