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

#include "Oms/OMS_VarObjInfo.hpp"
#include "Oms/OMS_Context.hpp"
#include "Oms/OMS_Session.hpp"


/*-------------------------------------------------------------*/

void* OMS_VarObjInfoNode::operator new(size_t size, OMS_Context& c) 
{
  if (c.IsVersion()) {
    return c.allocate(sizeof(OMS_VarObjInfoNode));
  }
  else {
    return c.StackHeapMalloc(sizeof(OMS_VarObjInfoNode));
  }
}

/*-------------------------------------------------------------*/

#if defined(OMS_PLACEMENT_DELETE)
void OMS_VarObjInfoNode::operator delete(void* p, OMS_Context& c) 
{
#ifndef USE_SYSTEM_ALLOC_CO13
  if (c.IsVersion()) {
#endif
    c.deallocate(p);
#ifndef USE_SYSTEM_ALLOC_CO13
  }
#endif
}
#endif

/*-------------------------------------------------------------*/

void OMS_VarObjInfoNode::deleteSelf (OMS_Context& c)
{
#ifndef USE_SYSTEM_ALLOC_CO13
  if (c.IsVersion())
  {
#endif
    c.deallocate(this);
#ifndef USE_SYSTEM_ALLOC_CO13
  }
#endif
}

/*=============================================================*/


void OMS_VarObjInfo::freeVarObj (OMS_Context* context) 
{
  context->FreeVarObj(m_pvobj, m_vobjSize);
#ifndef USE_SYSTEM_ALLOC_CO13
  if (context->IsVersion())
  {
#endif
    OMS_VarObjInfoNode** prev = &m_vobjInfo.m_vobjNext;
    OMS_VarObjInfoNode* next  = m_vobjInfo.m_vobjNext;
    while (NULL != next)
    {
      OMS_VarObjInfoNode* toFree = next;
      --toFree->m_refCnt;   
      next = next->m_vobjNext;
      if (0 == toFree->m_refCnt)
      {
        *prev = next;
        context->deallocate(toFree);
      }
      else
      {
        prev = &toFree->m_vobjNext;
      }
    }
#ifndef USE_SYSTEM_ALLOC_CO13
  }
#endif
  m_pvobj = NULL;
}

/*-------------------------------------------------------------*/

void OMS_VarObjInfo::freeVarObjData(OMS_Context* context) 
{
  context->FreeVarObj(m_pvobj, m_vobjSize);
  m_pvobj = NULL;
}
  
/*-------------------------------------------------------------*/

