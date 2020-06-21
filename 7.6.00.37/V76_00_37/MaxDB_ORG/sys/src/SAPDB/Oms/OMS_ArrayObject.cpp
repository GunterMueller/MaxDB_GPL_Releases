/*!
 * \file    OMS_ArrayObject.cpp
 * \author  MarkusSi, Roterring
 * \brief   Array object.
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

#include "Oms/OMS_ArrayObject.hpp"
#include "Oms/OMS_Globals.hpp"
#include "Oms/OMS_ClassEntry.hpp"
#include "Oms/OMS_ContainerEntry.hpp"
#include "Oms/OMS_GUID.hpp"
#include "Oms/OMS_Context.hpp"
#include "Oms/OMS_Session.hpp"
#include "hsp77.h"
#include "ggg01.h"



/*=======================================================================*/

/// Optimal array object sizes.
static const size_t OMS_ARROBJ_OPTSIZES[] = {
      16,   24,   32,   40,   56,   72,   88, 104,
     120,  144,  168,  192,  216,  248,  280, 328, 
     376,  424,  480,  552,  648,  784,  872, 984, 
    1128, 1320, 1592, 2000, 2672, 4024, 8072 };  
    // changed 8076 to 8072, because maximal size of var-objects is 8072 and as
    // array-objects are dereferenced like derived objects and as for derived
    // objects the same method is used as for var-objects, the size was reduced.

/*----------------------------------------------------------------------*/

size_t OmsArrayObjectBase::GetMaxByteSize(size_t category)
{
  if( category < 1 || category > OMS_ARROBJ_OPTSIZECNT ) 
  {
    ThrowInvalidParam((const char*)__FILE__, __LINE__);
  }
  return OMS_ARROBJ_OPTSIZES[ category-1 ]; 
}

/*----------------------------------------------------------------------*/

short OmsArrayObjectBase::GetCategory(size_t byteSize)
{
  if( byteSize < 1 || byteSize > OMS_ARROBJ_OPTSIZES[OMS_ARROBJ_OPTSIZECNT-1] ) 
  {
    ThrowInvalidParam((const char*)__FILE__, __LINE__);
  }
  const size_t *p = OMS_ARROBJ_OPTSIZES;
  // linear search performs for small number of entries (32)
  // better than binary...
  while (*p < byteSize) ++p;
  return p + 1 - OMS_ARROBJ_OPTSIZES;
}

/*----------------------------------------------------------------------*/

void OmsArrayObjectBase::ThrowInvalidParam(const char* pFile, unsigned int line)
{
  OMS_Globals::Throw(e_illegal_object_pos, "Invalid array object index or size", pFile, line);
}

void OmsArrayObjectBase::ThrowObjectLinkCorruption(const char* pFile, unsigned int line)
{
  OMS_Globals::Throw(e_internal_obj_link_corruption, "Internal array link structure corrupt", pFile, line);
}

/*----------------------------------------------------------------------*/

size_t OmsArrayObjectBase::GetMaxByteSize(OmsHandle &h, OmsAbstractObject *pObj)
{
  OmsObjectContainer *p = OmsObjectContainer::containerPtr(pObj);
  ClassID baseGuid;
  short   category;
  omsSplitArrayGuid(p->GetContainerInfoNoCheck()->GetGuid(), baseGuid, category);
  
  return OmsArrayObjectBase::GetMaxByteSize(category);
}


/*=======================================================================*/


void OmsArrayObjectIteratorBase::omsNext()
{
  ++m_category;

  while( m_category <= OMS_ARROBJ_OPTSIZECNT+1 ) 
  {
    // check if sub-container for current category exists
    if (m_h.m_pSession->m_context->GetContainerDir().ExistsSubContainer(m_guid, m_sh, m_cno, m_category)){
      break;
    }
    ++m_category;
  }

  if (m_category >= OMS_ARROBJ_OPTSIZECNT+1) {
    // does not exist
    m_category = 0;
    return;
  }
}

/*----------------------------------------------------------------------*/

OmsObjByClsIterBase OmsArrayObjectIteratorBase::omsAllOids(int maxBufferSize)
{ 
  ClassID arrayGuid = omsMakeArrayGuid(m_guid, m_category);
  return OmsAbstractObject::omsAllOids(m_h, arrayGuid, m_sh, m_cno, maxBufferSize);
}

/*======================================================================*/


