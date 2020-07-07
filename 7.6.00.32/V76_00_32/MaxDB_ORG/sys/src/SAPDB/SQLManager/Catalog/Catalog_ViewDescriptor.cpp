/*!
  @file           Catalog_ViewDescriptor.cpp
  @author         ThomasA
  @ingroup        Catalog
  @brief          yields information about a view

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

#include "SQLManager/Catalog/Catalog_Instance.hpp"
#include "SQLManager/Catalog/Catalog_ViewDescriptor.hpp"
#include "SQLManager/Catalog/Catalog_SessionCache.hpp"
#include "SQLManager/Catalog/Catalog_ViewtextObject.hpp"
#include "SQLManager/SQLMan_Context.hpp"

inline static bool IsWhiteSpaceChar(SAPDB_Byte* pChar, bool IsUnicode)
{
    if (IsUnicode && (0 != *(pChar - 1)))
    {
        return false;
    }
    return ((' ' == *pChar) || (9 == *pChar) || (10 == *pChar) || (13 == *pChar));
}

//-----------------------------------------------------------------------------

Catalog_ViewDescriptor::Catalog_ViewDescriptor(SQLMan_Context& Context, const Catalog_ObjectId& ViewId)
: Catalog_TableDescriptor(Context, ViewId)
{
    if (m_pTable)
    {
        if (!IsView())
        {
            // TODO
            m_pTable = 0;
            Context.ThrowError (e_missing_viewtable); 
        }
    }
}

//-----------------------------------------------------------------------------

Catalog_ViewDescriptor::DateTimeFormat Catalog_ViewDescriptor::GetDateTimeFormat() const
{
    if (TableValid(m_pContext, m_pTable))
    {
        Catalog_SessionCache& sessionCache = *reinterpret_cast<Catalog_SessionCache*>(&m_pContext->GetSessionCatalogCache()); 
        Catalog_ViewtextObject::Catalog_Key key(this->GetTableId());
        Catalog_ViewtextObject* pViewtext = reinterpret_cast<Catalog_ViewtextObject*>(sessionCache.Load(key));
        if (pViewtext)
        {
            return pViewtext->GetDateTimeFormat();
        }
    }
    return dtf_none;
}

//-----------------------------------------------------------------------------

int Catalog_ViewDescriptor::GetDefinitionLength(bool SelectExpressionOnly) const
{
    return this->GetDefinition (SelectExpressionOnly, 0, 0);
}

//-----------------------------------------------------------------------------

Catalog_ViewDescriptor::SQLMode Catalog_ViewDescriptor::GetSqlMode() const
{
    if (TableValid(m_pContext, m_pTable))
    {
        Catalog_SessionCache& sessionCache = *reinterpret_cast<Catalog_SessionCache*>(&m_pContext->GetSessionCatalogCache()); 
        Catalog_ViewtextObject::Catalog_Key key(this->GetTableId());
        Catalog_ViewtextObject* pViewtext = reinterpret_cast<Catalog_ViewtextObject*>(sessionCache.Load(key));
        if (pViewtext)
        {
            return pViewtext->GetSqlMode();
        }
    }
    return sqlm_internal;
}

//-----------------------------------------------------------------------------

bool Catalog_ViewDescriptor::CopyDefinition (bool SelectExpressionOnly, bool RemoveInternal, void* pDestination, SAPDB_UInt DestinationSize) const
{
    int definitionLength = GetDefinition (SelectExpressionOnly, pDestination, DestinationSize);
    if (definitionLength > 0)
    {
        if (RemoveInternal)
        {
            SAPDB_Byte* p = reinterpret_cast<SAPDB_Byte*>(pDestination);
            p += definitionLength - 1;
            bool isUnicode =  Catalog_Instance::Instance().IsUnicodeCatalog();
            int  step      = (isUnicode) ? 2 : 1;
            // skip blanks
            do
            {
                if (!IsWhiteSpaceChar(p, isUnicode)) break;
                p -= step;
            }
            while (true);
            const char* pInternal = "INTERNAL";
            int ix = 7; // index of L of pInternal
            do
            {
                if (*p != pInternal[ix--] || (isUnicode && (*(p-1) != 0))) return true;
                p -= step;
            }
            while (ix >= 0);
            if (IsWhiteSpaceChar (p, isUnicode))
            {
                // substitute internal by blanks
                for (int ix = 0; ix < 8; ++ix)
                {
                    p += step;
                    *p = ' ';
                }
            }
        }
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------

int Catalog_ViewDescriptor::GetDefinition (bool SelectExpressionOnly, void* pDestination, SAPDB_UInt DestinationSize) const
{
    int definitionLength = 0;
    if (TableValid(m_pContext, m_pTable))
    {
        Catalog_SessionCache& sessionCache = *reinterpret_cast<Catalog_SessionCache*>(&m_pContext->GetSessionCatalogCache()); 
        Catalog_ViewtextObject::Catalog_Key key(this->GetTableId());

        int selectOffset   = 0;
        SAPDB_Byte* pDest  = reinterpret_cast<SAPDB_Byte*>(pDestination);
        do
        {
            Catalog_ViewtextObject* pViewtext = reinterpret_cast<Catalog_ViewtextObject*>(sessionCache.Load(key));
            if (pViewtext)
            {
                int length = pViewtext->GetLength();
                if (SelectExpressionOnly)
                {
                    SelectExpressionOnly = 0;
                    selectOffset = pViewtext->GetSelectOffset();
                    if (selectOffset > length)
                    {
                        selectOffset -= length;
                        ++key;
                        break;
                    }
                }
                length           -= selectOffset;
                definitionLength += length;
                if (pDestination) 
                {
                    if (definitionLength > (int) DestinationSize)
                    {
                        m_pContext->ThrowError (e_buffer_limit);
                        return 0;
                    }
                    SAPDB_MemCopyNoCheck (pDest, 
                        pViewtext->GetTextPtr() + selectOffset, length);
                    pDest += length;
                }
                if (! pViewtext->NextExists())
                {
                    break;
                }
                if (selectOffset > 0)
                {
                    selectOffset = 0;
                }
            }
            else
            {
                m_pContext->ThrowError(sessionCache.GetLastError());
                break;
            }
            ++key;
        }
        while (true);
    }
    return definitionLength;
}