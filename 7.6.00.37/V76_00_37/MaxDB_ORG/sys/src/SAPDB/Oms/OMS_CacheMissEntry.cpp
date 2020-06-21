/*!
 * \file    OMS_CacheMissEntry.cpp
 * \author  MarkusSi, Roterring
 * \brief   Entry which is used to remember cache misses (cached keys)
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

#include "Oms/OMS_CacheMissEntry.hpp"
#include "Oms/OMS_Context.hpp"
#include "Oms/OMS_Session.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"

/*----------------------------------------------------------------------*/
/* Implementation of class OMS_CacheMissEntry inline methods          */
/*----------------------------------------------------------------------*/

void OMS_CacheMissEntry::setKey (const unsigned char *key, size_t keyLen)
{
  if (key == NULL || keyLen <= 0){
    OMS_Globals::Throw(e_OMS_invalid_parameter, "OMS_CacheMissEntry::setKey", __MY_FILE__, __LINE__);
  }

  // Copy key
  SAPDB_MemCopyNoCheck(&m_key, key, keyLen);
}

//----------------------------------------------------------------------/

void* OMS_CacheMissEntry::operator new (size_t      sz, 
                                        size_t      keyLen, 
                                        OMS_Context *pContext)
{
  return pContext->allocate(sz + keyLen - 1);
}

//----------------------------------------------------------------------/

void OMS_CacheMissEntry::deleteSelf(OMS_Context* pContext)
{
   pContext->deallocate(this);
}

//----------------------------------------------------------------------/

#if defined(OMS_PLACEMENT_DELETE)
void OMS_CacheMissEntry::operator delete (void        *p,
                                          size_t       keyLen,
                                          OMS_Context *pContext)
{
  pContext->deallocate(p);
}
#endif

//----------------------------------------------------------------------/

