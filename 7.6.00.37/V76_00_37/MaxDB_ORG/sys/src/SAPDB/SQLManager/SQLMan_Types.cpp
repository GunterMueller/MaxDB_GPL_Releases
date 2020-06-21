/*!***************************************************************************

  module      : SQLMan_Types.cpp

  -------------------------------------------------------------------------

  responsible : ThomasA

  special area: SQLManager
  description : Types used by the sql manager

  last changed: 2000-12-06  19:04
  see also    : example.html ...
  first created:2000-03-09  18:21

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2000-2005 SAP AG




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




*****************************************************************************/


//-----------------------------------------------------------------------------

#include "SQLManager/SQLMan_Types.hpp"
#include "SQLManager/Catalog/Catalog_Instance.hpp"
#include "hak01.h" // a01_il_b_identifier
#include "hak061.h" // a061identifier_len
#include "hsp83.h"

SQLMan_Identifier::SQLMan_Identifier()
{
    this->SetBlank();
}

SQLMan_Identifier::SQLMan_Identifier(bool initialize)
{
    if (initialize)
        this->SetBlank();
}

//-----------------------------------------------------------------------------

SQLMan_Identifier::SQLMan_Identifier(const char* Source, bool AsciiSource)
{
	SAPDBERR_ASSERT_ARGUMENT( AsciiSource ); // if it's a '\0' terminated char-string it must be ascii !

    if (Catalog_Instance::Instance().IsUnicodeCatalog() && AsciiSource)
    {
		int ix = 0;
		while (*Source && (ix < sizeof(*this)))
        {
            (*this)[ix++] = 0;
            (*this)[ix++] = *Source;
            Source++;
        }
        while (ix < sizeof(*this))
        {
            (*this)[ix++] = 0;
            (*this)[ix++] = ' ';
        }
    }
    else
    {
		int len = (int) strlen(Source);
        if (len > sizeof(*this))
        {
            len = sizeof(*this);
        }
        SAPDB_MemCopyNoCheck (this, Source, len);
        SAPDB_MemFillNoCheck(&(*this)[len], ' ', sizeof(*this) - len);
    }
}

//-----------------------------------------------------------------------------

SQLMan_Identifier::SQLMan_Identifier(const void* Source, const SAPDB_UInt2 Length, bool AsciiSource)
{
    const char* S = (const char*) Source;
    int len = Length;
    if (Catalog_Instance::Instance().IsUnicodeCatalog() && AsciiSource)
    {
		int ix = 0;
		while (len-- && (ix < sizeof(*this)))
        {
            (*this)[ix++] = 0;
            (*this)[ix++] = *S;
            S++;
        }
        while (ix < sizeof(*this))
        {
            (*this)[ix++] = 0;
            (*this)[ix++] = ' ';
        }
    }
    else
    {
        if (len > sizeof(*this))
        {
            len = sizeof(*this);
        }
        SAPDB_MemCopyNoCheck (this, S, len);
        SAPDB_MemFillNoCheck(&(*this)[len], ' ', sizeof(*this) - len);
    }
}

//-----------------------------------------------------------------------------

void SQLMan_Identifier::SetBlank()
{
    if (Catalog_Instance::Instance().IsUnicodeCatalog())
    {
        for (int ix = 0; ix < sizeof(*this); ++ix)
        {
            (*this)[ix++] = 0;
            (*this)[ix  ] = ' ';
        }
    }
    else
    {
        SAPDB_MemFillNoCheck(this, ' ', sizeof(*this));
    }
}

//-----------------------------------------------------------------------------

SAPDB_Bool SQLMan_Identifier::IsBlank() const
{
    return memcmp (this, a01_il_b_identifier, sizeof(*this)) == 0;
}

//-----------------------------------------------------------------------------

SAPDB_UInt SQLMan_Identifier::GetLength() const
{
    return a061identifier_len (*this);
}

//-----------------------------------------------------------------------------

void SQLMan_Identifier::ToUtf8(SAPDB_UTF8* buffer, SAPDB_UInt bufferSize) const
{
    if (Catalog_Instance::Instance().IsUnicodeCatalog())
    {
        const tsp81_UCS2Char* srcPos;
        tsp00_Byte*  dstPos;
        sp83UTF8ConvertFromUCS2(
            reinterpret_cast<const tsp81_UCS2Char*>(&(*this)[0]),
            reinterpret_cast<const tsp81_UCS2Char*>(&(*this)[sizeof(*this) - 1]),
            &srcPos,
            0,
            reinterpret_cast<tsp00_Byte*>(buffer),
            reinterpret_cast<tsp00_Byte*>(buffer + bufferSize),
            &dstPos);
        *dstPos = '\0';
    }
    else
    {
        const size_t copyLen =
            sizeof( *this ) < bufferSize ? sizeof( *this ) : bufferSize - 1;

        SAPDB_MemCopyNoCheck( buffer, &(*this)[0], copyLen );
        buffer[ copyLen ] = '\0';
    }
}

//-----------------------------------------------------------------------------
