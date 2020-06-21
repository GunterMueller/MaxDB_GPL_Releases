/*!*********************************************************************

    @file         Catalog_FieldSequenceMarshaler.cpp

    @author       ThomasA

    @brief        stores and restores FieldSequences into and from the catalog

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2004-2005 SAP AG

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

\endif
***********************************************************************/

#include "SQLManager/Catalog/Catalog_FieldSequenceMarshaler.hpp"
#include "SQLManager/Catalog/Catalog_SessionCache.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"

Catalog_FieldSequenceMarshaler::Catalog_FieldSequenceMarshaler(Catalog_SessionCache        & SessionCache, 
                                                               Catalog_Object::Catalog_Key & Key,
                                                               int                           MaxStoredKey)
: m_kind(kind_undef)
, m_sessionCache(SessionCache)
, m_maxKey(MaxStoredKey)
, m_written(0)
, m_key(Key)
{
    new ((void*) &m_record) Catalog_Object::Catalog_Key(Key);
    m_record.len() = cgg_rec_key_offset + Key.Length();
}

//-----------------------------------------------------------------------------

Catalog_FieldSequenceMarshaler::~Catalog_FieldSequenceMarshaler()
{
    this->Flush();
}

//-----------------------------------------------------------------------------

bool Catalog_FieldSequenceMarshaler::AddIntoCatalog ()
{
    int currKey = m_record.recBuf_gg00()[cgg_rec_key_offset + m_record.recKeyLen_gg00() - 1];
    bool ok = m_sessionCache.AddIntoCatalog (m_record, currKey > m_maxKey);
    m_record.recLen_gg00() = cgg_rec_key_offset + m_record.recKeyLen_gg00();
    if (ok)
    {
        ++m_written;
    }
    return ok;
}

//-----------------------------------------------------------------------------

void Catalog_FieldSequenceMarshaler::Flush()
{
    if ((kind_writer == m_kind) && (m_record.recLen_gg00() > cgg_rec_key_offset + m_record.recKeyLen_gg00()))
    {
        this->AddIntoCatalog ();
    }
}

//-----------------------------------------------------------------------------

int Catalog_FieldSequenceMarshaler::GetReadRows() const
{
    return m_maxKey;
}

//-----------------------------------------------------------------------------

int Catalog_FieldSequenceMarshaler::GetWrittenRows() const
{
    return m_written;
}

//-----------------------------------------------------------------------------

bool Catalog_FieldSequenceMarshaler::ReadBytes (void* pDestination, int RequiredBytes, Msg_List& MsgList)
{
    if (kind_undef == m_kind)
    {
        // very first call
        if (!m_sessionCache.ReadFromCatalog (m_record))
        {
            return false;
        }
        ++m_maxKey;
        m_offset = cgg_rec_key_offset + m_record.recKeyLen_gg00();
        m_kind   = kind_reader;
    }
    SAPDB_Byte* pCurr = reinterpret_cast<SAPDB_Byte*>(pDestination);
    while (true)
    {
        int moveLen = (RequiredBytes > (m_record.recLen_gg00() - m_offset)) ? m_record.recLen_gg00() - m_offset : RequiredBytes;
        if (moveLen > 0)
        {
            SAPDB_MemCopyNoCheck (pCurr, &m_record.recBuf_gg00()[m_offset], moveLen);
            pCurr         += moveLen;
            RequiredBytes -= moveLen;
            m_offset      += moveLen;
        }
        if (0 == RequiredBytes)
        {
            return true;
        }
        m_offset = cgg_rec_key_offset + m_record.recKeyLen_gg00();
        ++m_record.recBuf_gg00()[m_offset - 1];
        if (!m_sessionCache.ReadFromCatalog (m_record))
        {
            return false;
        }
        ++m_maxKey;
    }
    return false;
}

//-----------------------------------------------------------------------------

bool Catalog_FieldSequenceMarshaler::WriteBytes (const void* pSource, int ByteCount, Msg_List& MsgList)
{
    m_kind = kind_writer;
    const SAPDB_Byte* pCurr = reinterpret_cast<const SAPDB_Byte*>(pSource);
    while (ByteCount > 0)
    {
        int moveLen = (ByteCount > MAX_RECLEN_GG00 - m_record.len()) ? MAX_RECLEN_GG00 - m_record.len() : ByteCount; 
        SAPDB_MemCopyNoCheck (&m_record.recBuf_gg00()[m_record.len()], pCurr, moveLen);
        m_record.len() += moveLen;
        pCurr          += moveLen;
        ByteCount      -= moveLen;
        if (MAX_RECLEN_GG00 == m_record.len())
        {
            if (!this->AddIntoCatalog ())
            {
                return false;
            }
            ++m_record.recBuf_gg00()[m_record.recLen_gg00()-1];
        }
    }
    return true;
}

//-----------------------------------------------------------------------------