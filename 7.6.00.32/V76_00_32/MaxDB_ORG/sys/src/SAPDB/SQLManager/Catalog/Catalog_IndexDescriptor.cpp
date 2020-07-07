/*!
  @file           Catalog_IndexDescriptor.cpp
  @author         ThomasA
  @ingroup        Catalog
  @brief          yields information about an index

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


#include "SQLManager/Catalog/Catalog_IndexDescriptor.hpp"
#include "SQLManager/Catalog/Catalog_TableDescriptor.hpp"
#include "SQLManager/SQLMan_Types.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "SQLManager/ErrorHandling/SQLManErr_Interface.hpp"
#include "SAPDBCommon/Messages/SDBMsg_Catalog.h"

#include "FileDirectory/FileDir_IDirectory.hpp"
#include "FileDirectory/FileDir_Table.hpp"

#include "Transaction/Trans_Context.hpp"

#include "hak24.h"
#include "hbd03.h"

Catalog_IndexDescriptor::ColumnIterator::ColumnIterator (const Catalog_TableDescriptor& Table, const tak_multindex* Index, bool IsKey)
: m_table(Table)
, m_index(Index)
, m_curr (0)
, m_isKey(IsKey)
{
}

//------------------------------------------------------------------------------

 void Catalog_IndexDescriptor::ColumnIterator::operator++()
 {
     ++m_curr;
 }

//------------------------------------------------------------------------------

 Catalog_IndexDescriptor::ColumnIterator::operator bool() const
 {
     if (m_isKey)
     {
         return m_curr < m_table.GetKeyColumnCount();
     }
     return m_curr < m_index->icount;
 }

//------------------------------------------------------------------------------

const Catalog_ColumnDescriptor Catalog_IndexDescriptor::ColumnIterator::operator*() const
{
    if (m_isKey)
    {
        Catalog_TableDescriptor::KeyColumnIterator keyIter = m_table.BeginKey();
        for (int ix = 0; ix < m_curr; ++ix)
        {
           ++keyIter;
        }
        return *keyIter;
    }
    Catalog_TableDescriptor::ColumnIterator iter = m_table.GetColumn(m_index->icolseq[m_curr]);
    return *iter;
}

//------------------------------------------------------------------------------

Catalog_IndexDescriptor::ColumnIterator Catalog_IndexDescriptor::Begin() const
{
    return ColumnIterator (*m_table, (m_pIndex) ? &m_pIndex->indexdef[m_index-1] : 0, m_isKey);
}

//------------------------------------------------------------------------------

Catalog_IndexDescriptor::Catalog_IndexDescriptor (SQLMan_Context& Context, 
                                                  const Catalog_TableDescriptor& TableDescriptor, 
                                                  tak_mindexrecord* pIndex, 
                                                  int               Idx,
                                                  bool              IsKey)
: m_pContext(&Context)
, m_pIndex(pIndex)
, m_index(Idx)
, m_table(&TableDescriptor)
, m_isKey(IsKey)
{
}

//------------------------------------------------------------------------------

int Catalog_IndexDescriptor::GetIndexNo() const
{
    if (m_pIndex)
    {
        return m_pIndex->indexdef[m_index-1].indexno;
    }
    return -1;
}

//------------------------------------------------------------------------------

Catalog_IndexDescriptor::GetIndexFileNoRc
Catalog_IndexDescriptor::GetIndexFileNo(
    FileDir_FileNo& indexFileNo,
    Msg_List&       errMsg) const
{
    indexFileNo.Invalidate();

    if ( (m_pIndex == 0) || (m_table == 0) )
    {
        return gifnRcSystemError;
    }

    FileDir_IDirectory& fDir = FileDir_IDirectory::SessionInstance(
        m_pContext->GetTransContext().OldTrans() );

    const FileDir_FileNo tableFileNo( m_table->GetTableFileNo() );
    FileDir_Table tableEntry;
    FileDir_ReturnCode rc = fDir.GetTableFile(
        m_pContext->GetTransContext().TaskId(),
        tableFileNo,
        tableEntry,
        errMsg );
    if ( rc != FileDir_Okay )
    {
        switch ( rc ) {
            case FileDir_FileNoNotFound:
                errMsg = Msg_List(
                    Msg_List::Error,
                    SDBMSG_CATALOG_INDDESC_FILE_NOT_FOUND,
                    Msg_Arg(SDBMSGTAG_CATALOG_INDDESC_FILE_NOT_FOUND__FILENO,
                            FileDir_FileNoToString( tableFileNo ))
                    );
                return gifnRcFileNotFound;
            default:
                errMsg.Overrule(
                    Msg_List(
                        Msg_List::Error,
                        SDBMSG_CATALOG_INDDESC_FILE_NOT_FOUND,
                        Msg_Arg(SDBMSGTAG_CATALOG_INDDESC_FILE_NOT_FOUND__FILENO,
                                FileDir_FileNoToString( tableFileNo ))
                        ));
                return gifnRcSystemError;
        }
    }

    // returned value can be invalid if index is not registered in fdir
    indexFileNo = tableEntry.GetIndexFileNo( GetIndexNo() );

    if ( indexFileNo.IsInvalid() )
        return gifnRcFileNotFound;

    return gifnRcOk;
}

 //------------------------------------------------------------------------------

 SQLMan_Identifier Catalog_IndexDescriptor::GetName() const
{
    SQLMan_Identifier indexName;
    if (m_pIndex)
    {
        a24get_indexname (*m_pContext, reinterpret_cast<tak_sysbufferaddress>(m_pIndex), m_index, indexName); 
    }
    return indexName;
}

//------------------------------------------------------------------------------

bool Catalog_IndexDescriptor::IsFunctionBaseIndex() const
{
    return (st_func == m_pIndex->indexdef[m_index-1].icolstack[0].etype());
}

//------------------------------------------------------------------------------

bool Catalog_IndexDescriptor::IsKey() const
{
    return m_isKey;
}

//------------------------------------------------------------------------------

bool Catalog_IndexDescriptor::IsUniqueIndex() const
{
    if (m_isKey)
    {
        return true;
    }
    return (m_pIndex->indexdef[m_index-1].iunique != 0);
}

//------------------------------------------------------------------------------

Catalog_IndexDescriptor::operator bool() const
{
    return (m_isKey || (0 != m_pIndex));
}

//------------------------------------------------------------------------------

Catalog_IndexDescriptor::GetFileCounterRc
Catalog_IndexDescriptor::GetSecondaryKeyCount(
    SAPDB_Int& secondaryKeyCount,
    Msg_List&  errMsg) const
{
    FileDir_FileNo indexFileNo;
    const GetIndexFileNoRc gifnRc =
        this->GetIndexFileNo( indexFileNo, errMsg );

    if ( gifnRc != gifnRcOk )
        return (GetFileCounterRc) ( gifnRc );

    SAPDB_Int       dummyPageCount;
    tgg00_Surrogate indexSurr;
    indexSurr.rawAssign( indexFileNo.GetCharArray() );

    SQLMan_TransContext& trans = m_pContext->GetTransContext().OldTrans();
    bd03GetInvPageAndRecordCount(
        trans,
        indexSurr,
        dummyPageCount,
        secondaryKeyCount);

    if ( trans.trError_gg00 != e_ok )
        switch ( trans.trError_gg00 )
        {
            case e_file_not_found:
            case e_old_fileversion:
                return gfcRcFileNotFound;
            default:
                errMsg.Overrule(
                    Msg_List(
                        Msg_List::Error,
                        SDBMSG_CATALOG_INDDESC_GETCOUNT_SYSERR,
                        Msg_Arg( SDBMSGTAG_CATALOG_INDDESC_GETCOUNT_SYSERR__FILENO,
                                 FileDir_FileNoToString( indexFileNo) ),
                        Msg_Arg(SDBMSGTAG_CATALOG_INDDESC_GETCOUNT_SYSERR__BASIS_ERROR,
                                SQLManErr_Interface::GetInstance().GetBaseErrorText(
                                    trans.trError_gg00) )
                        ) );
                return gfcRcSystemError;
        }

    return gfcRcOk;
}

//------------------------------------------------------------------------------
Catalog_IndexDescriptor::GetFileCounterRc
Catalog_IndexDescriptor::GetLeafPageCount(
    SAPDB_Int& leafPageCount,
    Msg_List&  errMsg) const
{
    FileDir_FileNo indexFileNo;
    const GetIndexFileNoRc gifnRc =
        this->GetIndexFileNo( indexFileNo, errMsg );

    if ( gifnRc != gifnRcOk )
        return (GetFileCounterRc) ( gifnRc );

    tgg00_Surrogate indexSurr;
    indexSurr.rawAssign( indexFileNo.GetCharArray() );

    SQLMan_TransContext& trans = m_pContext->GetTransContext().OldTrans();
    bd03GetInvPageCount(
        trans,
        indexSurr,
        leafPageCount);

    if ( trans.trError_gg00 != e_ok )
        switch ( trans.trError_gg00 )
        {
            case e_file_not_found:
            case e_old_fileversion:
                return gfcRcFileNotFound;
            default:
                errMsg.Overrule(
                    Msg_List(
                        Msg_List::Error,
                        SDBMSG_CATALOG_INDDESC_GETCOUNT_SYSERR,
                        Msg_Arg( SDBMSGTAG_CATALOG_INDDESC_GETCOUNT_SYSERR__FILENO,
                                 FileDir_FileNoToString( indexFileNo) ),
                        Msg_Arg(SDBMSGTAG_CATALOG_INDDESC_GETCOUNT_SYSERR__BASIS_ERROR,
                                SQLManErr_Interface::GetInstance().GetBaseErrorText(
                                    trans.trError_gg00) )
                        ) );
                return gfcRcSystemError;
        }

    return gfcRcOk;
}

//------------------------------------------------------------------------------
