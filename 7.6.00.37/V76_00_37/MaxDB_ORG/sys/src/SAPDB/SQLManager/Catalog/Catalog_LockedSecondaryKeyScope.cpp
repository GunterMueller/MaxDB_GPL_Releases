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

#include "SQLManager/Catalog/Catalog_LockedSecondaryKeyScope.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "SQLManager/ErrorHandling/SQLManErr_Instance.hpp"
#include "SAPDBCommon/SAPDB_RangeCode.hpp"
#include "hkb53.h"

//-----------------------------------------------------------------------------

Catalog_LockedSecondaryKeyScope::Catalog_LockedSecondaryKeyScope(SQLMan_Context& Context)
: m_context(Context)
{
     m_key.keyLen_gg00() = 0;
}

//-----------------------------------------------------------------------------

Catalog_LockedSecondaryKeyScope::~Catalog_LockedSecondaryKeyScope()
{
    if (m_key.keyLen_gg00() > 0)
    {
          k53key_unlock (m_context.TransContext(), lckRowShare_egg00, m_indexId, m_key);
    }
}

//-----------------------------------------------------------------------------

int Catalog_LockedSecondaryKeyScope::Lock(const Catalog_ObjectId& IndexId, const SAPDB_Byte* pKey, int KeyLength, SAPDB_Byte DefinedByte)
{
    tgg00_BasisError& e = m_context.TransContext().trError_gg00; 
    
    e                      = e_ok;
    m_indexId              = IndexId;
    m_key.keyVal_gg00()[0] = DefinedByte;
    SAPDB_RangeMove (__FILE__, 1, KeyLength, sizeof (m_key.keyVal_gg00()), pKey, 1,
                &m_key.keyVal_gg00(), 2, KeyLength, e);
    if (e_ok == e)
    {
        m_key.keyLen_gg00() = 1 + KeyLength;
        tgg00_LockReqState reqState;
        tgg00_LockReqMode  grantedMode;
        bool noWait;
        bool collisionTest;
        if (show_command == m_context.a_command_kind)
        {
            noWait        = true;
            collisionTest = true;
        }
        else
        {
            noWait        = false;
            collisionTest = false;
        }
        reqState.clear();
        k53lock (m_context.TransContext(), *const_cast<Catalog_ObjectId*>(&IndexId), 
            m_key, lckRowShare_egg00, reqState, noWait, collisionTest, grantedMode);
        if (e_wait_for_lock_release == e)
        {
            k53wait (m_context.TransContext(), m_lock, mm_nil);
        }
        if (show_command == m_context.a_command_kind)
        {
            // collision test, no unlock
            m_key.keyLen_gg00() = 0;
        }
    }
    if (e_ok != e)
    {
        return SQLManErr_Interface::GetInstance().GetReturnCode(e, m_context.a_sqlmode);
    }
    return 0;
}

