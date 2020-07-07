/*!***************************************************************************

  module      : Catalog_IndexIterator.cpp

  -------------------------------------------------------------------------

  responsible : ThomasA

  special area: Catalog Manager
  description : Iterator over all indexes of a base table

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

#include "SQLManager/SQLMan_Context.hpp"
#include "SQLManager/Catalog/Catalog_IndexIterator.hpp"
#include "SQLManager/Catalog/Catalog_TableDescriptor.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "SAPDB/SAPDBCommon/Messages/SDBMsg_SQLMan.h"
#include "hak10.h"
#include "hak24.h"

class Catalog_IndexScanContext : public tak_index_scan_record
{
public :
    Catalog_IndexScanContext(SQLMan_Context& Context)
        : m_context(Context)
        , m_buf(0)
    {
        isr_buf = 0;
    }
    ~Catalog_IndexScanContext()
    {
        if (m_buf)
        {
            a10Unlock (m_context, m_buf);
        }
    }
    void IncrementRefCount()
    {
        if (isr_buf)
        {
            m_buf = isr_buf;
            a10Lock (m_context, isr_buf);
        }
    }
    void operator=(const Catalog_IndexScanContext& Source)
    {
         isr_buf   = Source.isr_buf;
         isr_index = Source.isr_index;
         isr_sysk  = Source.isr_sysk;
    }
private :
    SQLMan_Context&      m_context;
    tak_sysbufferaddress m_buf;
};

//-----------------------------------------------------------------------------

Catalog_IndexIterator::Catalog_IndexIterator (SQLMan_Context& Context, const Catalog_TableDescriptor& TableDescriptor, bool WithKey)
: m_pContext(&Context)
, m_iteratorContext(0)
, m_pTableDescriptor(0)
, m_currIsKey(false)
{
    SAPDBMem_IRawAllocator& allocator = Context.GetAllocator();
    m_pTableDescriptor = new (allocator) Catalog_TableDescriptor(TableDescriptor);
    if (0 == m_pTableDescriptor)
    {
        Context.SetNoMoreMemoryError (Msg_List(Msg_List::Error, 
            SDBMSG_SQLMAN_NO_MEMORY, Msg_OptArg("MEMREQUEST",sizeof(Catalog_TableDescriptor))));
        return;
    }

    if (TableDescriptor.AnyIndexExists())
    {
        m_iteratorContext = new (allocator) Catalog_IndexScanContext(Context);
        if (m_iteratorContext)
        {
            a24init_index_scan (Context, TableDescriptor.GetTableId(), *m_iteratorContext);
            m_iteratorContext->IncrementRefCount();
            this->operator++();
            m_currIsKey = WithKey;
        }
        else
        {
            destroy (m_pTableDescriptor, allocator); 
            Context.SetNoMoreMemoryError (Msg_List(Msg_List::Error, 
                SDBMSG_SQLMAN_NO_MEMORY, Msg_OptArg("MEMREQUEST",sizeof(Catalog_IndexScanContext))));
        }
    }
    else
    {
        m_currIsKey = WithKey;
    }
}

//-----------------------------------------------------------------------------

Catalog_IndexIterator::Catalog_IndexIterator (const Catalog_IndexIterator& Source)
: m_pContext(Source.m_pContext)
, m_iteratorContext(0)
, m_pTableDescriptor(0)
, m_currIsKey(Source.m_currIsKey)
{
    SAPDBMem_IRawAllocator& allocator = m_pContext->GetAllocator();
    if (Source.m_pTableDescriptor)
    {
        m_pTableDescriptor = new (allocator) Catalog_TableDescriptor(*Source.m_pTableDescriptor);
        if (!m_pTableDescriptor)
        {
            m_pContext->SetNoMoreMemoryError (Msg_List(Msg_List::Error, 
                SDBMSG_SQLMAN_NO_MEMORY, Msg_OptArg("MEMREQUEST",sizeof(Catalog_TableDescriptor))));
            return;
        }
    }
    if (Source.m_iteratorContext)
    {
        m_iteratorContext = new (allocator) Catalog_IndexScanContext(*m_pContext);
        if (m_iteratorContext)
        {
            *m_iteratorContext = *Source.m_iteratorContext;
            m_iteratorContext->IncrementRefCount();
            m_pTableDescriptor = new (allocator) Catalog_TableDescriptor(*Source.m_pTableDescriptor);
        }
        else
        {
            destroy (m_pTableDescriptor, allocator); 
            m_pContext->SetNoMoreMemoryError (Msg_List(Msg_List::Error, 
                SDBMSG_SQLMAN_NO_MEMORY, Msg_OptArg("MEMREQUEST",sizeof(Catalog_IndexScanContext))));
        }
    }
 }

//-----------------------------------------------------------------------------

Catalog_IndexIterator::~Catalog_IndexIterator()
{
    SAPDBMem_IRawAllocator& allocator = m_pContext->GetAllocator();
    if (m_iteratorContext)
    {
        destroy (m_iteratorContext, allocator);
    }
    if (m_pTableDescriptor)
    {
        destroy (m_pTableDescriptor, allocator);
    }
}

//-----------------------------------------------------------------------------

void Catalog_IndexIterator::operator++()
{
    if (m_currIsKey)
    {
        m_currIsKey = false;
        return;
    }
    if (m_iteratorContext)
    {
        if (!a24next_named_index (*m_pContext, *m_iteratorContext))
        {
            destroy (m_iteratorContext, m_pContext->GetAllocator());
        }
    }
}

 //-----------------------------------------------------------------------------

Catalog_IndexIterator::operator bool() const
{
    if (m_currIsKey)
    {
        return true;
    }
    return (0 != m_iteratorContext) && (0 != m_iteratorContext->isr_buf);
}

//-----------------------------------------------------------------------------

const Catalog_IndexDescriptor Catalog_IndexIterator::operator*() const
{
    if (m_iteratorContext)
    {
        return Catalog_IndexDescriptor (*m_pContext, *m_pTableDescriptor, reinterpret_cast<tak_mindexrecord*>(m_iteratorContext->isr_buf), 
            m_iteratorContext->isr_index, m_currIsKey);
    }
    else
    {
        return Catalog_IndexDescriptor (*m_pContext, *m_pTableDescriptor, 0, 0, m_currIsKey);
    }
}

//-----------------------------------------------------------------------------

 void Catalog_IndexIterator::operator= (const Catalog_IndexIterator& Source)
 {
     this->~Catalog_IndexIterator();
     new ((void*) this) Catalog_IndexIterator(Source);
 }
