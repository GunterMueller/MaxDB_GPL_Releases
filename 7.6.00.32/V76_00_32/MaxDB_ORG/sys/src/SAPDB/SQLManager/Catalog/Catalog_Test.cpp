/*
    @file Catalog_Test.cpp

    ========== licence begin  GPL
    Copyright (c) 2005 SAP AG

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
#include "SQLManager/Catalog/Catalog_Test.hpp"
#include "SQLManager/Catalog/Catalog_TableDescriptor.hpp"
#include "SQLManager/Catalog/Catalog_ViewDescriptor.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "SQLManager/SQLMan_Types.hpp"
#include "SAPDBCommon/Fields/SAPDBFields_IncrementalMemorySequence.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"

#include "hak06.h"
#include "hak10.h"

void Catalog_DescriptorTest()
{
    SQLMan_Context* pContext = SQLMan_Context::GetContext();
    Catalog_TableDescriptor desc (*pContext, SQLMan_Identifier(), SQLMan_Identifier(), false, false);
    if (desc)
    {
        pContext->ThrowError(-4711);
    }
    Catalog_TableDescriptor m (*pContext, SQLMan_Identifier ("SUT", /*ASCIISource*/true), 
		                                  SQLMan_Identifier ("MONITOR", /*ASCIISource*/true), false, false);
    Catalog_ViewDescriptor monitor (*pContext, m.GetTableId());
    if (monitor)
    {
        int length    = monitor.GetDefinitionLength(false);
        SAPDB_Byte* p = reinterpret_cast<SAPDB_Byte*>(pContext->GetAllocator().Allocate(length));
        if (p)
        {
            if (!monitor.CopyDefinition(false, true, p, length))
            {
                pContext->ThrowError(-4711);
            }
            pContext->GetAllocator().Deallocate(p);
        }
    }
    Catalog_TableDescriptor tables (*pContext, SQLMan_Identifier ("DOMAIN", /*ASCIISource*/true), 
		                                       SQLMan_Identifier ("TABLES", /*ASCIISource*/true), false, false);
    {
        Catalog_ObjectId tabid;
        FileDir_FileNo fileNo(tabid);
        {
            Catalog_TableDescriptor dummy (*pContext, fileNo);
        }
        {
            Catalog_TableDescriptor dummy (*pContext, fileNo);
        }
    }
    Catalog_ViewDescriptor  view (*pContext, tables.GetTableId());
    int length    = view.GetDefinitionLength(false);
    SAPDB_Byte* p = reinterpret_cast<SAPDB_Byte*>(pContext->GetAllocator().Allocate(length));
    if (p)
    {
        if (!view.CopyDefinition(false, true, p, length))
        {
            pContext->ThrowError(-4711);
        }
        pContext->GetAllocator().Deallocate(p);
    }
    length = view.GetDefinitionLength(true);
    p = reinterpret_cast<SAPDB_Byte*>(pContext->GetAllocator().Allocate(length));
    if (p)
    {
        if (!view.CopyDefinition(true, true, p, length))
        {
            pContext->ThrowError(-4711);
        }
        pContext->GetAllocator().Deallocate(p);
    }
    Catalog_TableDescriptor sysAuth (*pContext, SQLMan_Identifier ("SYSCATALOG", /*ASCIISource*/true), 
		                                        SQLMan_Identifier ("AUTHORIZATION", /*ASCIISource*/true), false, false);
    if (sysAuth)
    {
        Catalog_IndexDescriptor indexDesc = sysAuth.GetIndexDescriptor(1);
        if (indexDesc)
        {
            SQLMan_Identifier indexName = indexDesc.GetName();
        }
        Catalog_ObjectId tableid = sysAuth.GetTableId();
        Catalog_TableDescriptor sysAuthCopy (*pContext, tableid);
        if (!sysAuthCopy)
        {
             pContext->ThrowError(-4711);
        }
        for (Catalog_TableDescriptor::ColumnIterator iter = sysAuth.Begin(); iter != sysAuth.End(); ++iter)
        {
              Catalog_TableDescriptor::ColumnIterator col = sysAuth.FindColumn((*iter).GetColumnName());
              if (!col)
              {
                  pContext->ThrowError(-4711);
              }
        }
        for (Catalog_TableDescriptor::KeyColumnIterator keyIter = sysAuth.BeginKey(); keyIter != sysAuth.EndKey(); ++keyIter)
        {
              Catalog_TableDescriptor::ColumnIterator col = sysAuth.FindColumn((*keyIter).GetColumnName());
              if (!col)
              {
                  pContext->ThrowError(-4711);
              }
        }
        const bool withKey = true;
        Catalog_IndexIterator indexIter = sysAuth.GetIndexIterator(withKey);
        while (indexIter)
        {
            Catalog_IndexDescriptor d = *indexIter;
            Catalog_IndexDescriptor::ColumnIterator colIter = d.Begin();
            while (colIter)
            {
                Catalog_ColumnDescriptor colDesc = *colIter;
                ++colIter;
            }
            ++indexIter;
        }
        Catalog_TableDescriptor sysUpdStat (*pContext, SQLMan_Identifier ("SUT", /*ASCIISource*/true), 
			                                           SQLMan_Identifier ("SYSUPDSTATWANTED", /*ASCIISource*/true), false, false);
        if (sysUpdStat)
        {
             indexIter = sysUpdStat.GetIndexIterator(withKey);
             while (indexIter)
             {
                 Catalog_IndexDescriptor d = *indexIter;
                 Catalog_IndexDescriptor::ColumnIterator colIter = d.Begin();
                 while (colIter)
                 {
                     Catalog_ColumnDescriptor colDesc = *colIter;
                     ++colIter;
                 }
                 ++indexIter;
             }
        }
    }
    else
    {
        pContext->ThrowError(-4711);
    }
}

//-----------------------------------------------------------------------------

void Catalog_MessBlockTest()
{
   SQLMan_Context* pContext = SQLMan_Context::GetContext();
   tgg00_FileId fileId;
   a06a_mblock_init (*pContext, m_select, mm_nil, fileId);
   SAPDBMem_IRawAllocator& allocator = pContext->GetAllocator(); 
   SAPDBFields_IncrementalMemorySequence * p =
        new (allocator) SAPDBFields_IncrementalMemorySequence (allocator, 
                                            SAPDBFields_MemorySequence::sharedCount);
   pContext->a_mblock.mb_fieldlists()[cgg_idx_literal_valuefieldlist] = (tsp00_MoveObjPtr) p;
   int ix;
   for (ix = 0; ix < 100; ++ix)
   {
       tsp00_Byte* pByte = p->CreateFixedField (1);
       *pByte = ix;
   }
   tak_sysbufferaddress pSys;
   tgg00_SysInfoKey     sysk;
   tgg00_BasisError     e;
   for (ix = 0; ix < sizeof(sysk.stableid()); ++ix)
   {
        sysk.stableid()[ix] = 0xFF;
   }
   sysk.sentrytyp()[0] = 0;
   sysk.sentrytyp()[1] = cak_emessblock[1];
   sysk.slinkage()[0]  = 0;
   sysk.slinkage()[1]  = 0;
   sysk.skeylen        = 12;
   a10mblock_into_cache (*pContext, sysk, pContext->a_mblock, d_release, pSys, e);
   if (e != e_ok)
   {
       pContext->ThrowError(e);
       return;
   }
   a10add_sysinfo (*pContext, pSys, e);
   if (e != e_ok)
   {
       pContext->ThrowError(e);
       return;
   }
   a10key_del (*pContext, pSys);
   a10get_sysinfo (*pContext, sysk, d_release, pSys, e);
   if (e != e_ok)
   {
       pContext->ThrowError(e);
       return;
   }
   a10repl_sysinfo (*pContext, pSys, e);
   if (e != e_ok)
   {
       pContext->ThrowError(e);
       return;
   }
   a10key_del (*pContext, pSys);
   a10del_sysinfo (*pContext, sysk, e);
   if (e != e_ok)
   {
       pContext->ThrowError(e);
       return;
   }
}

//-----------------------------------------------------------------------------

void Catalog_Test::Execute (int TestNo)
{
    switch (TestNo)
    {
    case 1 :
        Catalog_DescriptorTest();
        break;
    case 2 :
        Catalog_MessBlockTest();
        break;
    }
}