
/*!***************************************************************************

  module      : Catalog_TableDescriptor.cpp

  -------------------------------------------------------------------------

  responsible : ThomasA

  special area: Catalog Manager
  description : Descriptor for a base or view table

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

#include "SAPDB/SQLManager/SQLMan_Context.hpp"
#include "SAPDB/SQLManager/Catalog/Catalog_TableDescriptor.hpp"
#include "SAPDB/SQLManager/Catalog/Catalog_TableObject.hpp"
#include "SAPDB/SQLManager/Catalog/Catalog_IAuthorization.hpp"
#include "SAPDB/SQLManager/Catalog/Catalog_Instance.hpp"
#include "SAPDB/SQLManager/Catalog/Catalog_Instance.hpp"
#include "SQLManager/ErrorHandling/SQLManErr_Interface.hpp"
#include "SAPDB/FileDirectory/FileDir_FileNo.hpp"
#include "FileDirectory/FileDir_IDirectory.hpp"
#include "FileDirectory/FileDir_Table.hpp"
#include "Transaction/Trans_Context.hpp"
#include "SAPDB/SAPDBCommon/SAPDB_MemCopyMoveNoCheck.hpp"
#include "SAPDB/SAPDBCommon/Messages/SDBMsg_Catalog.h"

#include "hak06.h"
#include "hak10.h"
#include "hak24.h"
#include "hak663.h"
#include "hbd01.h" // bd01CalculateRecordsAndLeaves
#include "ggg00.h"

#define TAK_SYSBUFFERADDRESS(X) reinterpret_cast<tak_sysbufferaddress>(const_cast<Catalog_TableObject*>(X))

//----------------------------------------------------------------------

bool Catalog_TableDescriptor::TableValid (SQLMan_Context* pContext, Catalog_TableObject* pTable)
{
    if (!pTable)
    {
        if (!pContext)
        {
            pContext = SQLMan_Context::GetContext();
        }
        Msg_List msg(Msg_List::Error, SDBMSG_CATALOG_INVALID_TABLE_DESCRIPTOR);
        Msg_List& list = pContext->GetErrorList();
        list.AppendNewMessage(msg);
        pContext->ThrowError (e_invalid);
        return false;
    }
    return true;
}

//-----------------------------------------------------------------------------

inline static tak00_columninfo* GetColPtr(Catalog_TableObject& Table, int Index)
{
    if (Index > Table.bcolumnSplitIdx)
    {
        return (*Table.bcolumnExtension)[Index - Table.bcolumnSplitIdx - 1];
    }
    else
    {
        return Table.bcolumn()[Index - 1];
    }
}

//-----------------------------------------------------------------------------

void Catalog_TableDescriptor::ColumnIterator::operator++()
{
   ++m_current;
}

//-----------------------------------------------------------------------------

Catalog_TableDescriptor::ColumnIterator::operator bool() const
{
    return (m_current > 0) && (m_current <= m_tableDesc.m_pTable->bmaxcol);
}

//-----------------------------------------------------------------------------

bool Catalog_TableDescriptor::ColumnIterator::operator== (const ColumnIterator& Compare) const
{
    return m_current == Compare.m_current;
}

//-----------------------------------------------------------------------------

bool Catalog_TableDescriptor::ColumnIterator::operator!= (const ColumnIterator& Compare) const
{
    return m_current != Compare.m_current;
}

//-----------------------------------------------------------------------------

const Catalog_ColumnDescriptor Catalog_TableDescriptor::ColumnIterator::operator*() const
{
    int colIndex = m_tableDesc.m_pTable->bextcolindex()[m_current-1] + m_tableDesc.m_pTable->bfirstindex;
    return Catalog_ColumnDescriptor(GetColPtr(*m_tableDesc.m_pTable, colIndex));
}

//-----------------------------------------------------------------------------

Catalog_TableDescriptor::ColumnIterator::ColumnIterator(const Catalog_TableDescriptor& TableDesc, int First)
: m_current(First)
, m_tableDesc(TableDesc)
{
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//SAPDB_Int2 Catalog_TableDescriptor::ExtColNo2LogicalColNo( SAPDB_Int2 ExtColNo ) const
//{
//	SAPDB_Int2 current = 0;
//	SAPDB_Int2 endpos  = TableValid(m_pContext, m_pTable) ? m_pTable->bmaxcol : -1;
//
//	SAPDB_Int2 logical = 0;
//    while ( current<endpos )
//    {
//        ++current;
//        int colIndex = m_pTable->bextcolindex()[current-1] + m_pTable->bfirstindex;
//        Catalog_ColumnDescriptor ColDescr(GetColPtr(*m_pTable, colIndex));
//        if ( ColDescr.IsVisible() 
//            && !ColDescr.IsDropped() 
//            && !ColDescr.IsSysKey() 
//            && HasSelectPrivilege(ColDescr) )
//			++logical;
//		if ( ColDescr.ExtColNo() == ExtColNo )
//			return logical;
//    }
//	return 0;
//}
//
//SAPDB_Int2 Catalog_TableDescriptor::LogicalColNo2ExtColNo( SAPDB_Int2 LogicalColNo ) const
//{
//	SAPDB_Int2 current = 0;
//	SAPDB_Int2 endpos  = TableValid(m_pContext, m_pTable) ? m_pTable->bmaxcol : -1;
//
//	SAPDB_Int2 logical = 0;
//    while ( current<endpos )
//    {
//        ++current;
//        int colIndex = m_pTable->bextcolindex()[current-1] + m_pTable->bfirstindex;
//        Catalog_ColumnDescriptor ColDescr(GetColPtr(*m_pTable, colIndex));
//        if ( ColDescr.IsVisible() 
//            && !ColDescr.IsDropped() 
//            && !ColDescr.IsSysKey() 
//            && HasSelectPrivilege(ColDescr) )
//			++logical;
//		if ( logical == LogicalColNo )
//			return ColDescr.ExtColNo();
//    }
//	return 0;
//}

//-----------------------------------------------------------------------------

//void Catalog_TableDescriptor::ColumnIteratorForStarResolving::find()
//{
//    while ( m_current<m_endpos )
//    {
//        int colIndex = m_tableDesc.m_pTable->bextcolindex()[m_current-1] + m_tableDesc.m_pTable->bfirstindex;
//        Catalog_ColumnDescriptor ColDescr(GetColPtr(*m_tableDesc.m_pTable, colIndex));
//        if ( ColDescr.IsVisible()
//			&& !ColDescr.IsDropped()
//			&& !ColDescr.IsSysKey()
//			&& m_tableDesc.HasSelectPrivilege(ColDescr) )
//            break;
//        ++m_current;
//    }
//}

//-----------------------------------------------------------------------------

//void Catalog_TableDescriptor::ColumnIteratorForStarResolving::operator++()
//{
//    ++m_current;
//    find();
//}

//-----------------------------------------------------------------------------

//Catalog_TableDescriptor::ColumnIteratorForStarResolving::operator bool() const
//{
//    return m_current > 0;
//}

//-----------------------------------------------------------------------------

//bool Catalog_TableDescriptor::ColumnIteratorForStarResolving::operator== (const ColumnIteratorForStarResolving& Compare) const
//{
//    return m_current == Compare.m_current;
//}

//-----------------------------------------------------------------------------

//bool Catalog_TableDescriptor::ColumnIteratorForStarResolving::operator!= (const ColumnIteratorForStarResolving& Compare) const
//{
//    return m_current != Compare.m_current;
//}

//-----------------------------------------------------------------------------

//const Catalog_ColumnDescriptor Catalog_TableDescriptor::ColumnIteratorForStarResolving::operator*() const
//{
//    int colIndex = m_tableDesc.m_pTable->bextcolindex()[m_current-1] + m_tableDesc.m_pTable->bfirstindex;
//    return Catalog_ColumnDescriptor(GetColPtr(*m_tableDesc.m_pTable, colIndex));
//}

//-----------------------------------------------------------------------------

//Catalog_TableDescriptor::ColumnIteratorForStarResolving::ColumnIteratorForStarResolving(const Catalog_TableDescriptor& TableDesc, int First, int Last)
//: m_current(First)
//, m_endpos(Last)
//, m_tableDesc(TableDesc)
//{
//    find();
//}

//-----------------------------------------------------------------------------

void Catalog_TableDescriptor::KeyColumnIterator::operator++()
{
    if (m_current == m_tableDesc.m_pTable->blastkeyind)
    {
        m_current = 0;
    }
    else
    {
        m_current = GetColPtr(*m_tableDesc.m_pTable, m_current)->cnextind();
    }
}

//-----------------------------------------------------------------------------

Catalog_TableDescriptor::KeyColumnIterator::operator bool() const
{
    return m_current > 0;
}

//-----------------------------------------------------------------------------

bool Catalog_TableDescriptor::KeyColumnIterator::operator== (const KeyColumnIterator& Compare) const
{
    return m_current == Compare.m_current;
}

//-----------------------------------------------------------------------------

bool Catalog_TableDescriptor::KeyColumnIterator::operator!= (const KeyColumnIterator& Compare) const
{
    return m_current != Compare.m_current;
}

//-----------------------------------------------------------------------------

const Catalog_ColumnDescriptor Catalog_TableDescriptor::KeyColumnIterator::operator*() const
{
    return Catalog_ColumnDescriptor(GetColPtr(*m_tableDesc.m_pTable, m_current));
}

//-----------------------------------------------------------------------------

Catalog_TableDescriptor::KeyColumnIterator::KeyColumnIterator(const Catalog_TableDescriptor& TableDesc, int End)
: m_current(End)
, m_tableDesc(TableDesc)
{
}

//-----------------------------------------------------------------------------

Catalog_TableDescriptor::Catalog_TableDescriptor ()
: m_pTable(0)
, m_pContext(0)
, m_synonym()
, m_currentAuthorisationId()
{
}

//-----------------------------------------------------------------------------

Catalog_TableDescriptor::Catalog_TableDescriptor (const Catalog_TableDescriptor& Source)
: m_pTable(Source.m_pTable)
, m_pContext(Source.m_pContext)
, m_synonym(Source.m_synonym)
, m_currentAuthorisationId(Source.m_currentAuthorisationId)
{
    if (Source.m_pTable)
    {
        a10Lock (*m_pContext, TAK_SYSBUFFERADDRESS(m_pTable));
    }
}

//-----------------------------------------------------------------------------

Catalog_TableDescriptor::Catalog_TableDescriptor (SQLMan_Context&          Context,
                                                  const SQLMan_Identifier& Schema,
                                                  const SQLMan_Identifier& TableName,
                                                  bool                     ConsiderResultTables,
                                                  bool                     ConsiderSYSDDTables)
{
    new ((void*) this) Catalog_TableDescriptor (Context, Schema, TableName, Context.a_curr_user_id, ConsiderResultTables, ConsiderSYSDDTables);
}

 //-----------------------------------------------------------------------------

Catalog_TableDescriptor::Catalog_TableDescriptor (SQLMan_Context&          Context,
                                                  const SQLMan_Identifier& Schema,
                                                  const SQLMan_Identifier& TableName,
                                                  const Catalog_ObjectId&  CurrentAuthorizationId,
                                                  bool                     ConsiderResultTables,
                                                  bool                     ConsiderSYSDDTables)
: m_pTable(0)
, m_pContext(&Context)
, m_synonym()
, m_currentAuthorisationId(CurrentAuthorizationId)
{
    tak_syspointerarr p;
    if (ConsiderResultTables)
    {
        if ((Catalog_Instance::Instance().BlankIdentifier() == Schema) ||
            (Context.GetCurrentUserName() == Schema))
        {
            pasbool ok;
            a663restable_get (Context, d_fix, *const_cast<SQLMan_Identifier*>(&TableName), p, ok);
            if (ok)
            {
                m_pTable = reinterpret_cast<Catalog_TableObject*>(p.pbasep);
                a10Lock (Context, p.pbasep);
                return;
            }
        }
    }
    pasbool inDDLTrigger     =  Context.a_in_ddl_trigger;
    Context.a_in_ddl_trigger = ConsiderSYSDDTables;
    SQLMan_Identifier schema(Schema);
    if (a06_table_exist (Context, d_fix, schema, *const_cast<SQLMan_Identifier*>(&TableName), p, true))
    {
        m_pTable = reinterpret_cast<Catalog_TableObject*>(p.pbasep);
        if (p.psynfound)
        {
            m_synonym = p.psynid;
        }
        a10Lock (Context, p.pbasep);
    }
    Context.a_in_ddl_trigger = inDDLTrigger;
}

 //-----------------------------------------------------------------------------

Catalog_TableDescriptor::Catalog_TableDescriptor (SQLMan_Context& Context, const Catalog_ObjectId& TableId)
: m_pTable(0)
, m_pContext(&Context)
, m_synonym()
, m_currentAuthorisationId(Context.a_curr_user_id)
{
    pasbool ok;
    tak_sysbufferaddress p;
    a06_systable_get (Context, d_release, *const_cast<Catalog_ObjectId*>(&TableId), p, true, ok);
    if (ok)
    {
         m_pTable = reinterpret_cast<Catalog_TableObject*>(p);
         a10Lock (Context, p);
    }
}

//-----------------------------------------------------------------------------

Catalog_TableDescriptor::Catalog_TableDescriptor (SQLMan_Context& Context, const FileDir_FileNo& FileNo)
: m_pTable(0)
, m_pContext(&Context)
, m_synonym()
, m_currentAuthorisationId(Context.a_curr_user_id)
{
    pasbool ok;
    tgg00_Surrogate tableid = *reinterpret_cast<const tgg00_Surrogate*>(FileNo.GetCharArray());
    tak_sysbufferaddress p;
    a06_systable_get (Context, d_release, tableid, p, true, ok);
    if (ok)
    {
         m_pTable = reinterpret_cast<Catalog_TableObject*>(p);
         a10Lock (Context, p);
    }
}

//-----------------------------------------------------------------------------

Catalog_TableDescriptor::~Catalog_TableDescriptor()
{
    this->Release();
}

//-----------------------------------------------------------------------------

bool Catalog_TableDescriptor::AnyIndexExists() const
{
    if (TableValid(m_pContext, m_pTable))
    {
        return (0 != m_pTable->bindexexist);
    }
    return false;
}

//-----------------------------------------------------------------------------

Catalog_TableDescriptor::ColumnIterator Catalog_TableDescriptor::Begin() const
{
    return  ColumnIterator (*this, (TableValid(m_pContext, m_pTable)) ? 1 : 0);
}

//-----------------------------------------------------------------------------

//Catalog_TableDescriptor::ColumnIteratorForStarResolving Catalog_TableDescriptor::BeginStarResolving() const
//{
//    return  ColumnIteratorForStarResolving (*this, (TableValid(m_pContext, m_pTable)) ? 1 : 0, (TableValid(m_pContext, m_pTable)) ? m_pTable->bmaxcol+1 : -1);
//}

//-----------------------------------------------------------------------------

Catalog_TableDescriptor::KeyColumnIterator Catalog_TableDescriptor::BeginKey() const
{
    return KeyColumnIterator (*this, (TableValid(m_pContext, m_pTable)) ? m_pTable->bfirstcolind : 0);
}

//-----------------------------------------------------------------------------

Catalog_TableDescriptor::ColumnIterator Catalog_TableDescriptor::End() const
{
    return ColumnIterator (*this, (TableValid(m_pContext, m_pTable)) ? m_pTable->bmaxcol + 1 : -1);
}

//-----------------------------------------------------------------------------

//Catalog_TableDescriptor::ColumnIteratorForStarResolving Catalog_TableDescriptor::EndStarResolving() const
//{
//    return ColumnIteratorForStarResolving (*this, (TableValid(m_pContext, m_pTable)) ? m_pTable->bmaxcol+1 : -1, (TableValid(m_pContext, m_pTable)) ? m_pTable->bmaxcol+1 : -1);
//}

//-----------------------------------------------------------------------------

Catalog_TableDescriptor::KeyColumnIterator Catalog_TableDescriptor::EndKey() const
{
    return KeyColumnIterator (*this, 0);
}

//-----------------------------------------------------------------------------

Catalog_TableDescriptor::ColumnIterator Catalog_TableDescriptor::FindColumn(const SQLMan_Identifier& ColumnName) const
{
    if (TableValid(m_pContext, m_pTable))
    {
        ColumnIterator last = this->End();
        for (ColumnIterator curr = this->Begin(); curr != last; ++curr)
        {
            if ((*curr).IsColumnnameEqual(ColumnName))
            {
                return curr;
            }
        }
    }
    return ColumnIterator(*this, 0);
}

//-----------------------------------------------------------------------------

SAPDB_Bool Catalog_TableDescriptor::ContainsColumn(const SQLMan_Identifier& ColumnName) const
{
    if (TableValid(m_pContext, m_pTable))
    {
        ColumnIterator last = this->End();
        for (ColumnIterator curr = this->Begin(); curr != last; ++curr)
        {
            if ((*curr).IsColumnnameEqual(ColumnName))
            {
                return true;
            }
        }
    }
    return false;
}

//-----------------------------------------------------------------------------

Catalog_ObjectId Catalog_TableDescriptor::GetAuthorizationId() const
{
    return m_currentAuthorisationId;
}

//-----------------------------------------------------------------------------

Catalog_TableDescriptor::ColumnIterator
Catalog_TableDescriptor::GetColumn(
    const int ColumnNumber) const
{
    if (TableValid(m_pContext, m_pTable))
    {
        //idx = m_pTable->bextcolindex[ColumnNumber-1] + m_pTable->bfirstindex;
        //tak00_colinfo_ptr pCol = a103GetColumn (*m_pTable, idx);
        //if (pCol)
        //{
        //    Catalog_ColumnDescriptor ColDesc(pCol);
        //}
        return ColumnIterator(*this, ColumnNumber);
    }
    return ColumnIterator(*this, 0);
}

//-----------------------------------------------------------------------------

int Catalog_TableDescriptor::GetColumnCount() const
{
    return (TableValid(m_pContext, m_pTable)) ? m_pTable->bmaxcol : 0;
}

//-----------------------------------------------------------------------------

void Catalog_TableDescriptor::GetFileNo (FileDir_FileNo& FileNo) const
{
    if (TableValid(m_pContext, m_pTable))
    {
        new ((void*) &FileNo) FileDir_FileNo(m_pTable->btreeid.fileTabId_gg00());
    }
}

//-----------------------------------------------------------------------------

int Catalog_TableDescriptor::GetKeyColumnCount() const
{
    return (TableValid(m_pContext, m_pTable)) ? m_pTable->bkeycolcount : 0;

}

//-----------------------------------------------------------------------------

//int Catalog_TableDescriptor::GetColumnCountForStarResolving() const
//{
////    return (TableValid(m_pContext, m_pTable)) ? (m_pTable->bmaxcol - m_pTable->bcntdropcol) : 0 ;
//    // DDT : invisible columns must be taken into account too
//
//    SAPDB_Int2 Cnt = 0;
//    if (TableValid(m_pContext, m_pTable))
//    {
//        ColumnIteratorForStarResolving last = this->EndStarResolving();
//        for ( ColumnIteratorForStarResolving curr = this->BeginStarResolving();
//              curr != last; ++curr)
//        {
//            ++Cnt;
//        }
//    }
//    return Cnt;
//}

//-----------------------------------------------------------------------------

Catalog_IndexIterator Catalog_TableDescriptor::GetIndexIterator(bool WithKey)
{
    return Catalog_IndexIterator (*m_pContext, *this, WithKey);
}

//-----------------------------------------------------------------------------

void Catalog_TableDescriptor::GetPriv (tak_privilege& Priv, bool selectRequiredOnly) const
{
    if (TableValid(m_pContext, m_pTable))
    {
        tak00_PrivilegeSet requiredPriv;
        if (selectRequiredOnly)
        {
            requiredPriv.clear();
            requiredPriv.addElement(r_sel);
        }
        else
        {
            SAPDB_MemFillNoCheck( &requiredPriv, 0xFF, sizeof(requiredPriv));
        }
        tak_sysbufferaddress p = TAK_SYSBUFFERADDRESS(m_pTable);
        tgg00_Surrogate currUser   = m_pContext->a_curr_user_id;
        m_pContext->a_curr_user_id = m_currentAuthorisationId;
        a06get_priv (*m_pContext, p, requiredPriv, Priv);
        m_pContext->a_curr_user_id = currUser;
    }
    else
    {
        SAPDB_MemFillNoCheck( &Priv, 0, sizeof(Priv));
    }
}

//-----------------------------------------------------------------------------

 void Catalog_TableDescriptor::GetOwnerName (SQLMan_Identifier& Owner) const
 {
     if (TableValid(m_pContext, m_pTable))
     {
         Owner = Catalog_IAuthorization::GetInstance().GetSchemaName (*m_pContext, m_pTable->bauthid);
     }
     else
     {
         Owner = Catalog_Instance::Instance().BlankIdentifier();
     }
 }

 //-----------------------------------------------------------------------------

SQLMan_Identifier Catalog_TableDescriptor::GetOwnerName () const
{
    return (TableValid(m_pContext, m_pTable))
    ? Catalog_IAuthorization::GetInstance().GetSchemaName (*m_pContext, m_pTable->bauthid)
    : Catalog_Instance::Instance().BlankIdentifier();
}

Catalog_ObjectId Catalog_TableDescriptor::GetOwnerId () const
{
    return (TableValid(m_pContext, m_pTable))
        ? (Catalog_ObjectId)(m_pTable->bauthid) : Catalog_ObjectId();
}

 //-----------------------------------------------------------------------------

void Catalog_TableDescriptor::GetSchemaName (SQLMan_Identifier& SchemaName) const
{
    if (TableValid(m_pContext, m_pTable))
    {
        SchemaName = Catalog_IAuthorization::GetInstance().GetSchemaName (*m_pContext, m_pTable->bschema);
    }
    else
    {
        SchemaName = Catalog_Instance::Instance().BlankIdentifier();
    }
}

//-----------------------------------------------------------------------------

SQLMan_Identifier Catalog_TableDescriptor::GetSchemaName () const
{
    return (TableValid(m_pContext, m_pTable))
        ? Catalog_IAuthorization::GetInstance().GetSchemaName (*m_pContext, m_pTable->bschema)
        : Catalog_Instance::Instance().BlankIdentifier();
}

//-----------------------------------------------------------------------------

Catalog_ObjectId Catalog_TableDescriptor::GetSchemaId() const
{
    return m_pTable->bschema;
}

//-----------------------------------------------------------------------------

void Catalog_TableDescriptor::GetTableName (SQLMan_Identifier& TableName) const
{
    if (TableValid(m_pContext, m_pTable))
    {
        TableName = *m_pTable->btablen;
    }
}

//-----------------------------------------------------------------------------

SQLMan_Identifier Catalog_TableDescriptor::GetTableName () const
{
    if (TableValid(m_pContext, m_pTable))
    {
        return *m_pTable->btablen;
    }
    else
    {
        return Catalog_Instance::Instance().BlankIdentifier();
    }
}

//-----------------------------------------------------------------------------

SAPDB_Int4 Catalog_TableDescriptor::GetStatisticsSamplingInfo() const
{
    if (TableValid(m_pContext, m_pTable))
    {
        return m_pTable->bsample;
    }
    else
    {
        return 0;
    }
}

//-----------------------------------------------------------------------------

bool Catalog_TableDescriptor::GetTableStatisticsDescriptor(bool ForUpdate, Catalog_TableStatisticsDescriptor& Descriptor) const
{
    if (TableValid(m_pContext, m_pTable))
    {
        new ((void*) &Descriptor) Catalog_TableStatisticsDescriptor (*m_pContext, m_pTable, ForUpdate);
        return Descriptor;
    }
    return false;
}

//-----------------------------------------------------------------------------

SAPDB_Int Catalog_TableDescriptor::GetAverageRowLength() const
{
    if (TableValid(m_pContext, m_pTable))
    {
        return m_pTable->bavgrowlen;
    }
    else
    {
        return 0;
    }
}

//-----------------------------------------------------------------------------

Catalog_IndexDescriptor Catalog_TableDescriptor::GetIndexDescriptor(int IndexId) const
{
    if (TableValid(m_pContext, m_pTable))
    {
        tak_index_scan_record indexScanRec;
        a24fnd_indexno (*m_pContext, this->GetTableId(), IndexId, indexScanRec);
        return Catalog_IndexDescriptor (*m_pContext, *this, reinterpret_cast<tak_mindexrecord*>(indexScanRec.isr_buf), indexScanRec.isr_index, false);
    }
    return Catalog_IndexDescriptor (*m_pContext, *this, 0, 0, false);
}

//-----------------------------------------------------------------------------

Catalog_ObjectId Catalog_TableDescriptor::GetTableId() const
{
    return m_pTable->GetObjectId();
}

//-----------------------------------------------------------------------------

FileDir_FileNo Catalog_TableDescriptor::GetTableFileNo() const
{
    return m_pTable->btreeid.fileTabId_gg00();
}

//-----------------------------------------------------------------------------

 bool Catalog_TableDescriptor::HasAnyPrivilege() const
 {
    tak_privilege priv;
    const bool cSelectOnly = true; 
    this->GetPriv(priv, !cSelectOnly);
    tak00_PrivilegeSet emptyAllPriv;
    emptyAllPriv.clear();
    tak_priv_desc emptyColPriv;
    emptyColPriv.clear();
    return (priv.priv_all_set() != emptyAllPriv) || (priv.priv_col_exist() != emptyColPriv);
 }

 //-----------------------------------------------------------------------------

 bool Catalog_TableDescriptor::HasAnySelectPrivilege() const
 {
     tak_privilege priv;
     const bool cSelectOnly = true; 
     this->GetPriv(priv, cSelectOnly);
     return priv.priv_all_set().includes(r_sel) || priv.priv_col_exist().includes(priv_col_sel);
 }

 //-----------------------------------------------------------------------------

 bool Catalog_TableDescriptor::HasAllSelectPrivileges() const
 {
     tak_privilege priv;
     const bool cSelectOnly = true; 
     this->GetPriv(priv, cSelectOnly);
     return priv.priv_all_set().includes(r_sel);
 }

 //-----------------------------------------------------------------------------

bool Catalog_TableDescriptor::HasSelectPrivilege(const Catalog_ColumnDescriptor& ColumnDescriptor) const
{
    if (ColumnDescriptor.IsDropped() || !ColumnDescriptor.IsVisible())
    {
        return false;
    }
    tak_privilege priv;
    const bool cSelectOnly = true; 
    this->GetPriv(priv, cSelectOnly);
    return priv.priv_all_set().includes(r_sel) ||
        (priv.priv_col_exist().includes(priv_col_sel) && (priv.priv_sel_set().includes(ColumnDescriptor.ExtColNo())));
}

//-----------------------------------------------------------------------------

 bool Catalog_TableDescriptor::HasUserDefinedKey() const
 {
     return (TableValid(m_pContext, m_pTable)) ? (twithkey == m_pTable->btablekind) : false;
 }

//-----------------------------------------------------------------------------

 bool Catalog_TableDescriptor::HasVirtualKey() const
 {
     return (TableValid(m_pContext, m_pTable)) ? (m_pTable->battributes.includes(ta_cluster_key)) : false;
 }

 //-----------------------------------------------------------------------------

bool Catalog_TableDescriptor::IsComplexView() const
{
    return (TableValid(m_pContext, m_pTable)) ? (tcomplexview == m_pTable->btablekind) : false;
}

//-----------------------------------------------------------------------------

bool Catalog_TableDescriptor::IsEqualTablename(const SQLMan_Identifier& TableName) const
{
    return (TableValid(m_pContext, m_pTable)) ? TableName == *m_pTable->btablen : false;
}

//-----------------------------------------------------------------------------

bool Catalog_TableDescriptor::IsEqualTable(const SQLMan_Identifier& Schema, const SQLMan_Identifier& TableName) const
{
    if (TableValid(m_pContext, m_pTable))
    {
        SQLMan_Identifier S = Catalog_IAuthorization::GetInstance().GetSchemaName (*m_pContext, m_pTable->bschema);
        return ( (TableName == *m_pTable->btablen) && (S==Schema) );
    }
    return false;
}


//-----------------------------------------------------------------------------

bool Catalog_TableDescriptor::IsJoinView() const
{
    return (TableValid(m_pContext, m_pTable)) ? (tview == m_pTable->btablekind) : false;
}

//-----------------------------------------------------------------------------

bool Catalog_TableDescriptor::IsResultTable() const
{
    return (TableValid(m_pContext, m_pTable)) ? (tresult == m_pTable->btablekind) : false;
}

//-----------------------------------------------------------------------------

bool Catalog_TableDescriptor::IsArchiveTable() const
{
    return m_pTable->btreeid.fileType_gg00().includes (ftsArchive_egg00);
}

//-----------------------------------------------------------------------------

bool Catalog_TableDescriptor::IsView() const
{
    return (TableValid(m_pContext, m_pTable))   
        ? ((tview == m_pTable->btablekind)                                                                   || 
        ((tonebase == m_pTable->btablekind) && (m_pTable->bsurrogate != m_pTable->btreeid.fileTabId_gg00())) || 
        (tcomplexview == m_pTable->btablekind))
        : false;
}

//-----------------------------------------------------------------------------

bool Catalog_TableDescriptor::IsSynonym() const
{
    return (TableValid(m_pContext, m_pTable)) ? (tsynonym == m_pTable->btablekind) : false;
}

//-----------------------------------------------------------------------------

bool Catalog_TableDescriptor::IsTempTable() const
{
    SAPDB_Byte temp0 = cak_temp_table_site[0];
    SAPDB_Byte temp1 = cak_temp_table_site[1];
    return (TableValid(m_pContext, m_pTable)) ? ((m_pTable->bsurrogate[0] == temp0) && (m_pTable->bsurrogate[1] == temp1)) : false;
}

//-----------------------------------------------------------------------------

bool Catalog_TableDescriptor::IsCatalogTable() const
{
    return (TableValid(m_pContext, m_pTable)) ? ((tonebase == m_pTable->btablekind) && (tcatalog_table == m_pTable->bv_tablekind)) : false;
}

//-----------------------------------------------------------------------------

bool Catalog_TableDescriptor::IsUniqueColumnSet(const Catalog_TableDescriptor::ColumnNoVector& columnSet) 
{
    if (!TableValid(m_pContext, m_pTable)) 
    {
        return false;
    }
	if ((twithkey != m_pTable->btablekind) && (twithoutkey != m_pTable->btablekind) && (tonebase != m_pTable->btablekind))
	{
		return false;
	}
    Catalog_TableDescriptor  baseDesc;
    Catalog_TableDescriptor* pBaseDesc = this;

    if (tonebase == m_pTable->btablekind)
    {
        // current descriptor decribes a view, read corresponding base table
        pBaseDesc = new((void*) &baseDesc) Catalog_TableDescriptor (*m_pContext, *reinterpret_cast<Catalog_ObjectId*>(&m_pTable->btreeid.fileTabId_gg00()));
        if (!(*pBaseDesc))
        {
            return false;
        }
    }
    static const bool cWithKey = true;
    Catalog_IndexIterator indexIter = pBaseDesc->GetIndexIterator(cWithKey);
    while (indexIter)
    {
        if ((*indexIter).IsUniqueIndex())
        {
            tak_columnset uniqueColSet;
            uniqueColSet.clear();
            Catalog_IndexDescriptor::ColumnIterator colIter = (*indexIter).Begin();
            bool mayBeUnique = true;
            while (colIter)
            {
                if (tonebase == m_pTable->btablekind)
                {
                    ColumnIterator viewColIter = Begin();
                    bool indexColInView = false;
                    int tableColNo = (*colIter).ExtColNo();
                    while (viewColIter)
                    {
                        int viewColNo = (*viewColIter).RecColNo();
                        if (viewColNo == tableColNo)
                        {
                            uniqueColSet.addElement((*viewColIter).ExtColNo());
                            indexColInView = true;
                            break;
                        }
                        ++viewColIter;
                    }
                    if (!indexColInView)
                    {
                        // column not in view ==> don't check this unique constraint any more
                        mayBeUnique = false;
                        break;
                    }
                }
                else
                {
                    uniqueColSet.addElement((*colIter).ExtColNo());
                }
                ++colIter;
            }
            if (mayBeUnique)
            {
                int size = columnSet.GetSize();
                for (int ix = 0; ix < size; ++ix) 
                {
                    uniqueColSet.delElement(columnSet[ix]);
                }
                if (uniqueColSet.isEmpty())
                {
                    return true;
                }
            }
        }
        ++indexIter;
    }
	return false;
}

//-----------------------------------------------------------------------------

void Catalog_TableDescriptor::LocateColumnValue (const SQLMan_Identifier& ColumnName, const tgg00_Rec& Row, const SAPDB_Byte* &pValue, int &ValueLength) const
{
    pValue = 0;
    if (TableValid(m_pContext, m_pTable))
    {
        ColumnIterator colDesc = this->FindColumn(ColumnName);
        if (colDesc)
        {
            (*colDesc).LocateColumnValue (Row, pValue, ValueLength);
        }
    }
}

//-----------------------------------------------------------------------------

bool Catalog_TableDescriptor::HasSysKey() const
{
    int colIndex = m_pTable->bfirstcolind;
    Catalog_ColumnDescriptor ColDesc(GetColPtr(*m_pTable, colIndex));

	return ColDesc.IsSysKey();
}

//-----------------------------------------------------------------------------

bool Catalog_TableDescriptor::ReferencedViaSynonym() const
{
    return !m_synonym.IsNull();
}

//-----------------------------------------------------------------------------

void Catalog_TableDescriptor::Release()
{
    if (m_pTable)
    {
        a10Unlock (*m_pContext, TAK_SYSBUFFERADDRESS(m_pTable));
    }
}

//-----------------------------------------------------------------------------

Catalog_TableDescriptor::operator bool() const
{
    return (0 != m_pTable);
}

//-----------------------------------------------------------------------------

void Catalog_TableDescriptor::operator= (const Catalog_TableDescriptor& Source)
{
    this->Release();
    new ((void*) this) Catalog_TableDescriptor (Source);
}

//-----------------------------------------------------------------------------

Catalog_TableDescriptor::BuildFileIdRc 
Catalog_TableDescriptor::BuildFileId(
    SQLMan_FileId& fileId,
    Msg_List&      errMsg) const
{
    FileDir_IDirectory& fDir = FileDir_IDirectory::SessionInstance(
        m_pContext->GetTransContext().OldTrans() );

    FileDir_Table tableEntry;
    const FileDir_ReturnCode rc = fDir.GetTableFile(
        m_pContext->GetTransContext().TaskId(),
        this->GetTableFileNo(),
        tableEntry,
        errMsg );

    if ( rc != FileDir_Okay )
        switch ( rc )
        {
            case FileDir_FileNoNotFound:
                return bfRcFileNotFound;
            default:
                errMsg.Overrule(
                    Msg_List(
                        Msg_List::Error,
                        SDBMSG_CATALOG_TABLEDESC_BUILDFILEID_FAILED,
                        Msg_Arg( SDBMSGTAG_CATALOG_TABLEDESC_BUILDFILEID_FAILED__FILENO,
                                 FileDir_FileNoToString( this->GetTableFileNo() ) ),
                        Msg_Arg( SDBMSGTAG_CATALOG_TABLEDESC_BUILDFILEID_FAILED__ERRORTEXT,
                                 rc )
                        ) );
                return bfRcSystemError;
        }

    tableEntry.BuildFileId( fileId );

    return bfRcOk;
}

//-----------------------------------------------------------------------------

Catalog_TableDescriptor::GetFileCounterRc
Catalog_TableDescriptor::GetRecordCount(
    SAPDB_Int& recordCount,
    Msg_List&  errMsg) const
{
    SQLMan_FileId fileId;

    const BuildFileIdRc rc = this->BuildFileId( fileId, errMsg );
    if ( rc != bfRcOk )
        return (GetFileCounterRc) ( rc );

    SQLMan_TransContext& trans = m_pContext->GetTransContext().OldTrans();

    SAPDB_Int4 dummyPageCount;
    bd01CalculateRecordsAndLeaves(trans, fileId, dummyPageCount, recordCount);

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
                        SDBMSG_CATALOG_TABLEDESC_GETCOUNT_SYSERR,
                        Msg_Arg( SDBMSGTAG_CATALOG_TABLEDESC_GETCOUNT_SYSERR__FILENO,
                                 FileDir_FileNoToString( this->GetTableFileNo() ) ),

                        Msg_Arg( SDBMSGTAG_CATALOG_TABLEDESC_GETCOUNT_SYSERR__BASIS_ERROR,
                                 SQLManErr_Interface::GetInstance().GetBaseErrorText(
                                     trans.trError_gg00) )
                        ) );
                return gfcRcSystemError;
        }

    return gfcRcOk;
}

//-----------------------------------------------------------------------------

Catalog_TableDescriptor::GetFileCounterRc
Catalog_TableDescriptor::GetLeafPageCount(
    SAPDB_Int& leafPageCount,
    Msg_List&  errMsg) const
{
    SQLMan_FileId fileId;

    const BuildFileIdRc rc = this->BuildFileId( fileId, errMsg );
    if ( rc != bfRcOk )
        return (GetFileCounterRc) ( rc );

    SQLMan_TransContext& trans = m_pContext->GetTransContext().OldTrans();

    SAPDB_Int4 dummyRecordCount;
    bd01CalculateRecordsAndLeaves(trans, fileId, leafPageCount, dummyRecordCount);

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
                        SDBMSG_CATALOG_TABLEDESC_GETCOUNT_SYSERR,
                        Msg_Arg( SDBMSGTAG_CATALOG_TABLEDESC_GETCOUNT_SYSERR__FILENO,
                                 FileDir_FileNoToString( this->GetTableFileNo() ) ),

                        Msg_Arg( SDBMSGTAG_CATALOG_TABLEDESC_GETCOUNT_SYSERR__BASIS_ERROR,
                                 SQLManErr_Interface::GetInstance().GetBaseErrorText(
                                     trans.trError_gg00) )
                        ) );
                return gfcRcSystemError;
        }

    return gfcRcOk;
}

//-----------------------------------------------------------------------------

Catalog_TableDescriptor::GetFileCounterRc
Catalog_TableDescriptor::GetLeafPageAndRecordCount(
    SAPDB_Int& leafPageCount,
    SAPDB_Int& recordCount,
    Msg_List&  errMsg) const
{
    SQLMan_FileId fileId;

    const BuildFileIdRc rc = this->BuildFileId( fileId, errMsg );
    if ( rc != bfRcOk )
        return (GetFileCounterRc) ( rc );

    SQLMan_TransContext& trans = m_pContext->GetTransContext().OldTrans();

    bd01CalculateRecordsAndLeaves(trans, fileId, leafPageCount, recordCount);

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
                        SDBMSG_CATALOG_TABLEDESC_GETCOUNT_SYSERR,
                        Msg_Arg( SDBMSGTAG_CATALOG_TABLEDESC_GETCOUNT_SYSERR__FILENO,
                                 FileDir_FileNoToString( this->GetTableFileNo() ) ),

                        Msg_Arg( SDBMSGTAG_CATALOG_TABLEDESC_GETCOUNT_SYSERR__BASIS_ERROR,
                                 SQLManErr_Interface::GetInstance().GetBaseErrorText(
                                     trans.trError_gg00) )
                        ) );
                return gfcRcSystemError;
        }

    return gfcRcOk;
}

//-----------------------------------------------------------------------------

bool Catalog_TableDescriptor::GetSysUpdStatWantedInsertedFlag () const
{
	if (TableValid(m_pContext, m_pTable))
    {
        return (0 != m_pTable->bupdstatinserted);
    }
	return false;
}

//-----------------------------------------------------------------------------

void Catalog_TableDescriptor::SetSysUpdStatWantedInsertedFlag (bool DoSet) 
{
    if (TableValid(m_pContext, m_pTable))
    {
        m_pTable->bupdstatinserted = DoSet;
    }
}

