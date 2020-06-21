/*!***************************************************************************

  module      : Catalog_ColumnDescriptor.cpp

  -------------------------------------------------------------------------

  responsible : ThomasA

  special area: Catalog Manager
  description : descriptor for a column of a base or view table

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


#include "SQLManager/Catalog/Catalog_ColumnDescriptor.hpp"
#include "SQLManager/Catalog/Catalog_Instance.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"

#include "hgg04.h"
#include "vak001.h"

void Catalog_ColumnDescriptor::GetColumnName(SQLMan_Identifier& ColumnName) const
{

    int colNameLength = m_pColumnInfo->ccolumnn_len();
    SAPDB_MemCopyNoCheck( &ColumnName[0], &m_pColumnInfo->ccolumnn()[0], colNameLength );
    if (colNameLength < sizeof(ColumnName))
    {
        SAPDB_MemCopyNoCheck( &ColumnName[colNameLength], &Catalog_Instance::Instance().BlankIdentifier(),
            sizeof(ColumnName) - colNameLength);
    }
}

//-----------------------------------------------------------------------------

 SQLMan_Identifier Catalog_ColumnDescriptor::GetColumnName () const
 {
    SQLMan_Identifier columnName;
    this->GetColumnName(columnName);
    return columnName;
 }

//-----------------------------------------------------------------------------

int Catalog_ColumnDescriptor::GetDataLength() const
{
    return m_pColumnInfo->cdatalen();
}

//-----------------------------------------------------------------------------

int Catalog_ColumnDescriptor::GetIOLength() const
{
    return m_pColumnInfo->cinoutlen();
}

//-----------------------------------------------------------------------------

const SQLMan_StackEntry& Catalog_ColumnDescriptor::GetStackCode() const
{
    return m_pColumnInfo->ccolstack();
}

//-----------------------------------------------------------------------------

bool Catalog_ColumnDescriptor::IsColumnnameEqual (const SQLMan_Identifier& CompareName) const
{
    if (0 == memcmp (&m_pColumnInfo->ccolumnn()[0], &CompareName[0], m_pColumnInfo->ccolumnn_len()))
    {
        return (0 == memcmp (&CompareName[m_pColumnInfo->ccolumnn_len()], 
            &Catalog_Instance::Instance().BlankIdentifier(), 
            sizeof(Catalog_Instance::Instance().BlankIdentifier()) - m_pColumnInfo->ccolumnn_len()));
    }
    return false;
}

//-----------------------------------------------------------------------------

bool Catalog_ColumnDescriptor::IsNullable() const
{
    return (m_pColumnInfo->ccolpropset().includes(ctopt) && !m_pColumnInfo->ccolpropset().includes(ctkey));
}

bool Catalog_ColumnDescriptor::IsKeyColumn() const
{
    return (m_pColumnInfo->ccolpropset().includes(ctkey));
}

bool Catalog_ColumnDescriptor::IsIndexed() const
{
    return (m_pColumnInfo->ccolpropset().includes(ctmulti));
}

bool Catalog_ColumnDescriptor::IsDropped() const
{
    return (m_pColumnInfo->ccolpropset().includes(ctdropped));
}

bool Catalog_ColumnDescriptor::IsVisible() const
{
    return (!m_pColumnInfo->ccolpropset().includes(ctinvisible));
}

bool Catalog_ColumnDescriptor::IsSysKey() const
{
    SQLMan_Identifier SysKeyName(cak_n_tablekey, /*ASCIISource*/true);   // "SYSKEY            "
    return IsColumnnameEqual(SysKeyName);
}

bool Catalog_ColumnDescriptor::IsLongDataType() const
{
	return ( m_pColumnInfo->cdatatyp() == dstra		||
             m_pColumnInfo->cdatatyp() == dstre		||
             m_pColumnInfo->cdatatyp() == dstrb		||
             m_pColumnInfo->cdatatyp() == dstrdb	||
             m_pColumnInfo->cdatatyp() == dstruni	||
             m_pColumnInfo->cdatatyp() == dlonga	||
             m_pColumnInfo->cdatatyp() == dlonge	||
             m_pColumnInfo->cdatatyp() == dlongb	||
             m_pColumnInfo->cdatatyp() == dlongdb	||
             m_pColumnInfo->cdatatyp() == dlonguni	);
}

SAPDB_Int2 Catalog_ColumnDescriptor::ExtColNo() const
{
    return (m_pColumnInfo->cextcolno());
}

SAPDB_Int2 Catalog_ColumnDescriptor::RecColNo() const
{
    return (m_pColumnInfo->creccolno());
}

//-----------------------------------------------------------------------------

void Catalog_ColumnDescriptor::LocateColumnValue (const tgg00_Rec& Row, const SAPDB_Byte* &pValue, int& ValueLength) const
{
    tgg00_VarColPosList varColPos;
    int                 pos;
    varColPos.vpl_last = -1;
    g04locate_col (m_pColumnInfo->ccolstack(), &Row, varColPos, pos, ValueLength);
    pValue = &Row.recBuf_gg00()[pos-1];
}
