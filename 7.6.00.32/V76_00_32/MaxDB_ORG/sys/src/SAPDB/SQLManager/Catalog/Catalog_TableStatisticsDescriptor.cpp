/*!***************************************************************************

  module      : Catalog_TableStatisticsDescriptor.cpp

  -------------------------------------------------------------------------

  responsible : ThomasA

  special area: Catalog Manager
  description : Catalog Table Statistics Descriptor

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


#include "SQLManager/Catalog/Catalog_TableStatisticsDescriptor.hpp"
#include "SQLManager/Catalog/Catalog_TableStatisticsObject.hpp"
#include "SQLManager/Catalog/Catalog_TableObject.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "hak10.h"
#include "hak28.h"

//-----------------------------------------------------------------------------

Catalog_TableStatisticsDescriptor::Catalog_TableStatisticsDescriptor ()
: m_pContext(0)
, m_tableId ()
, m_pStatistics(0)
{
}

//-----------------------------------------------------------------------------

Catalog_TableStatisticsDescriptor::Catalog_TableStatisticsDescriptor (SQLMan_Context& Context, const Catalog_TableObject* pTable, bool LockExclusive)
: m_pContext(&Context)
, m_tableId (pTable->GetObjectId())
, m_pStatistics(0)
{
    tak_sysbufferaddress p;
    tgg00_BasisError     e;
    a28lock_stat_catalog_rec (Context, reinterpret_cast<tak_sysbufferaddress>(const_cast<Catalog_TableObject*>(pTable)), LockExclusive, p, e);
    if (e_ok == e)
    {
        if (a10GetRefCount(p) > 0)
        {
            Context.ThrowError (e_invalid);
        }
        else
        {
            a10Lock (*m_pContext, p);
            m_pStatistics = reinterpret_cast<Catalog_TableStatisticsObject*>(p);
        }
    }
    else
    {
        Context.ThrowError (e);
    }
}

//-----------------------------------------------------------------------------

Catalog_TableStatisticsDescriptor::Catalog_TableStatisticsDescriptor (const Catalog_TableStatisticsDescriptor& Source)
: m_pContext(Source.m_pContext)
, m_tableId (Source.m_tableId)
, m_pStatistics(Source.m_pStatistics)
{
    if (Source.m_pStatistics)
    {
        a10Lock (*m_pContext, reinterpret_cast<tak_sysbufferaddress>(m_pStatistics));
    }
}

//-----------------------------------------------------------------------------

Catalog_TableStatisticsDescriptor::~Catalog_TableStatisticsDescriptor()
{
    if (m_pStatistics)
    {
        a10Unlock      (*m_pContext, reinterpret_cast<tak_sysbufferaddress>(m_pStatistics));
        a10rel_sysinfo (reinterpret_cast<tak_sysbufferaddress>(m_pStatistics));
    }
}

//-----------------------------------------------------------------------------

Catalog_TableStatisticsDescriptor::operator bool() const
{
    return (0 != m_pStatistics);
}

//-----------------------------------------------------------------------------

void Catalog_TableStatisticsDescriptor::operator= (const Catalog_TableStatisticsDescriptor& Source)
{
    new((void*) this) Catalog_TableStatisticsDescriptor (Source);
}

//-----------------------------------------------------------------------------

bool Catalog_TableStatisticsDescriptor::ExistsColumnStatistics (int ColNo) const
{
    if (m_pStatistics->sti_col_cnt > 0)
    {
        return m_pStatistics->sti_index.includes(ColNo);
    }
    return false;
}

//-----------------------------------------------------------------------------

bool Catalog_TableStatisticsDescriptor::SetColumnStatistics (int ColNo, SAPDB_Int4 DistinctValues)
{
    if (m_pContext->IsOk() && m_pStatistics)
    {
        Catalog_TableStatisticsObject::Catalog_Key key(m_tableId);
        tak_sysbufferaddress p = reinterpret_cast<tak_sysbufferaddress>(m_pStatistics);
        a10Unlock (*m_pContext, p);
        a28assign_col_info (*m_pContext,  key, p, ColNo, DistinctValues, cak_is_undefined, cak_is_undefined);
        if (m_pContext->IsOk())
        {
            a10Lock (*m_pContext, p);
            m_pStatistics = reinterpret_cast<Catalog_TableStatisticsObject*>(p);
            return true;
        }
    }
    return false;
}

//-----------------------------------------------------------------------------

void Catalog_TableStatisticsDescriptor::SetRowCount (SAPDB_Int4 RowCount)
{
    if (m_pStatistics)
    {
        m_pStatistics->SetRowCount (RowCount);
    }
}

//-----------------------------------------------------------------------------

void Catalog_TableStatisticsDescriptor::SetPageCount (SAPDB_Int4 PageCount)
{
    if (m_pStatistics)
    {
        m_pStatistics->SetPageCount (PageCount);
    }
}

//-----------------------------------------------------------------------------

bool Catalog_TableStatisticsDescriptor::Store() const
{
    if (m_pStatistics && m_pContext->IsOk())
    {
        tgg00_BasisError e;
        m_pStatistics->SetTimestamp();
        tak_sysbufferaddress p = reinterpret_cast<tak_sysbufferaddress>(m_pStatistics);
        a10repl_sysinfo( *m_pContext, p, e );
        if (e_ok != e)
        {
            m_pContext->ThrowError (e);
        }
        return (e_ok == e);
    }
    return false;
}
