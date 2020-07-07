/*!
  @file           SysView_PseudoLongHandler.cpp
  @author         ThomasA
  @ingroup        SystemViews
  @brief          support for columns of datatype long in system views

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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
*/

#include "SystemViews/SysView_PseudoLongHandler.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "hak101.h"
#include "hgg04.h"
#include "hbd02.h"

int SysView_PseudoLongHandler::m_keyLength     = sizeof(SAPDB_Int4) + sizeof(unsigned char);
int SysView_PseudoLongHandler::m_infoOffset    = cgg_rec_key_offset + SysView_PseudoLongHandler::m_keyLength;
int SysView_PseudoLongHandler::m_maxInfoLength = MAX_RECLEN_GG00 - SysView_PseudoLongHandler::m_infoOffset;

//--------------------------------------------------------------------------------------------------------------

SysView_PseudoLongHandler::SysView_PseudoLongHandler(SQLMan_Context& context)
: m_fileIdGenerator(0)
{
    context.a_resultCurrLongFileId = -1;
}

//--------------------------------------------------------------------------------------------------------------

void SysView_PseudoLongHandler::CreateFileId(SQLMan_Context& context, SAPDB_Int4 fileId, tgg00_FileId& file)
{
    g04build_temp_tree_id (file, context.TransContext());
    file.fileTfnTemp_gg00().becomes(ttfnColumn_egg00);
    a101_SetTempFileIndex (context, file, fileId);
}

//--------------------------------------------------------------------------------------------------------------

void SysView_PseudoLongHandler::DestroyPseudoLongContainer(SQLMan_Context& context, SAPDB_Int4& fileId)
{
    tgg00_FileId file;
    this->CreateFileId (context, fileId, file); 
    a101_DestroyGroupedTempFile (context.TransContext(), file);
    if (e_ok != context.TransContext().trError_gg00)
    {
        context.ThrowError (context.TransContext().trError_gg00, 1);
    }
}

//--------------------------------------------------------------------------------------------------------------

void SysView_PseudoLongHandler::InsertLong(SQLMan_Context& context, const void* pValue, int size, SysView_PseudoLongHandler::LongIdentificator& key)
{
    tgg00_BasisError e = e_ok;
    if (-1 == context.a_resultCurrLongFileId )
    {
        // file for current statement does not exist, create one
        ++m_fileIdGenerator;
        context.a_resultCurrLongFileId  = m_fileIdGenerator; 
        context.a_resultCurrLongFileKey = 0;
        this->CreateFileId (context, m_fileIdGenerator, context.a_resultCurrLongFile);
        a101_CreateGroupedTempFile (
            context.TransContext(),
            context.a_resultCurrLongFile,
            ttfnColumn_egg00);
        e = context.TransContext().trError_gg00;
    }
    union {
        void*     m_align;
        tgg00_Rec m_rec;
    } rec;
    SAPDB_Int4*    pKey1    = REINTERPRET_CAST( SAPDB_Int4*, &rec.m_rec.recBody_gg00());
    unsigned char* pKey2    = REINTERPRET_CAST( unsigned char*, &rec.m_rec.recBody_gg00()[sizeof(*pKey1)]);
    rec.m_rec.recKeyLen_gg00()    = m_keyLength;
    rec.m_rec.recVarcolCnt_gg00() = 0;
    ++context.a_resultCurrLongFileKey;
    unsigned char  key2 = 0;
    const unsigned char* p    = REINTERPRET_CAST(const unsigned char*, pValue);
    // store the value as a sequence of rows
    while ((e_ok == e) && (size > 0))
    {
        *pKey1 = context.a_resultCurrLongFileKey;
        *pKey2 = ++key2;
        int moveLen = (size > m_maxInfoLength) ? m_maxInfoLength : size;
        SAPDB_MemCopyNoCheck (pKey2 + 1, p, moveLen);
        size -= moveLen;
        p    += moveLen;
        rec.m_rec.recLen_gg00()          = m_infoOffset + moveLen;
        rec.m_rec.recVarcolOffset_gg00() = rec.m_rec.recLen_gg00() - m_infoOffset;
        b02add_record (context.TransContext(), context.a_resultCurrLongFile, rec.m_rec);
        e = context.TransContext().trError_gg00;
    }
    if (e_ok != e)
    {
        context.ThrowError (e, 1);
    }
    else
    {
        key.m_fileId   = a101_GetTempFileIndex (context, context.a_resultCurrLongFile);
        key.m_rowId    = context.a_resultCurrLongFileKey;
        key.m_rowCount = key2;
    }
}

//--------------------------------------------------------------------------------------------------------------

int SysView_PseudoLongHandler::ReadLong (SQLMan_Context& context, 
                                         SysView_PseudoLongHandler::LongIdentificator& longId, 
                                         int  startPos, 
                                         int  length, 
                                         void* buf, 
                                         int   bufSize)
{
    union {
        void*      m_align;
        tgg00_Lkey m_key; 
    } key;

    SAPDB_Int4*    pKey1    = REINTERPRET_CAST( SAPDB_Int4*, &key.m_key.k());
    unsigned char* pKey2    = REINTERPRET_CAST( unsigned char*, &key.m_key.k()[sizeof(*pKey1)]);
    *pKey1          = longId.m_rowId;
    *pKey2          = (startPos / m_maxInfoLength) + 1;
    key.m_key.len() = m_keyLength; 
    int pos = startPos % m_maxInfoLength;
    tgg00_Rec    rec;
    tgg00_FileId file;
    this->CreateFileId (context, longId.m_fileId, file);
    a101_GetTempFileInstance (context.TransContext(), file);
    tgg00_BasisError e  = context.TransContext().trError_gg00;
    int restLength      = length;
    if (e_ok == e)
    {
        unsigned char* pBuf = REINTERPRET_CAST(unsigned char*, buf);
        do {
            b02get_record (context.TransContext(), file, key.m_key, rec);
            e = context.TransContext().trError_gg00;
            if (e_ok == e)
            {
                int moveLength = rec.recLen_gg00() - m_infoOffset;
                if (moveLength > restLength)
                {
                    moveLength = restLength;
                }
                SAPDB_MemCopyNoCheck (pBuf, &rec.recBuf_gg00()[m_infoOffset], moveLength);
                restLength -= moveLength;
                pBuf       += moveLength;
                ++(*pKey2);
                if (*pKey2 > longId.m_rowCount)
                {
                    break;
                }
            }
            else
            {
                break;
            }
        }
        while (restLength > 0);
    }
    if (e_ok != e)
    {
        context.ThrowError (e);
    }
    return length - restLength;
}
