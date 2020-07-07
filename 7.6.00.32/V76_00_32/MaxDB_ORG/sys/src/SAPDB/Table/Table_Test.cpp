/*!
  @file           Table_Test.cpp
  @ingroup		  Table
  @author         UweH

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

#include <memory.h> // memset
#include "SAPDBCommon/Algorithms/SAPDBAlgo_FunnelMerge.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "SAPDBCommon/Fields/SAPDBFields_FieldList.hpp"
#include "RunTime/RTE_Message.hpp"
#include "KernelCommon/Kernel_VTrace.hpp"
#include "KernelCommon/Kernel_BlobDescription.hpp"
#include "Logging/Log_Transaction.hpp"
#include "Table/Table_Exceptions.hpp"
#include "Table/Table_TempHashTable.hpp"
#include "Table/Table_Table.hpp"
#include "Table/Table_UnsortedResult.hpp"
#include "Table/Table_Messages.hpp"
#include "Table/Table_Test.hpp"
#include "Transaction/Trans_Context.hpp"
#include "hkb53.h"
#include "ggg200.h"
#include "heo52.h"

/* --------------------------------------------------------------------------- */

class TestDataMerger : public Table_TempHashTableAggregate_IDataMerger
{
public:
    TestDataMerger( void ) { }

    virtual void MergeData(const SAPDB_Byte* Source, SAPDB_Byte* Destination, SAPDB_UInt2 Length, bool Found, bool SourceIsPartialAggregate)
    {
        if (!Found)
            memcpy(Destination, Source, Length);
        else
            *((SAPDB_UInt2*)&Destination[2]) += *((SAPDB_UInt2*)&Source[2]);
    }
};

/* --------------------------------------------------------------------------- */

bool TempHashTable_Test (tgg00_TransContext& TransContext, Msg_List& errList)
{
    SAPDB_Byte  Data[512];
    SAPDB_Byte* DataPtr;

    SAPDBMem_IRawAllocator& Allocator = *( REINTERPRET_CAST(SAPDBMem_IRawAllocator*, TransContext.trAllocator_gg00) );

    Table_FixSizeEntry_FieldDescription     FieldDescription(Allocator);
    FieldDescription.Reserve(2);
    FieldDescription.InsertEnd(Table_OffsetLengthTuple(0,                  sizeof(SAPDB_Int4)));    // Data
    FieldDescription.InsertEnd(Table_OffsetLengthTuple(sizeof(SAPDB_Int4), sizeof(SAPDB_UInt4)));   // Key

    Table_FixSizeEntry_KeyNoList    KeyNoList(Allocator);
    KeyNoList.Reserve(1);
    KeyNoList.InsertEnd(1);

    SAPDB_UInt4 DataCnt = 1000; // changed from 100000 to 1000 UH 18-01-2005 because SUT is running into DBFULL
    SAPDB_UInt4 KeyCnt  =   59; // must be smaller than DataCnt for testing

    TestDataMerger  DataMerger;

    TransContext.trError_gg00 = 0;

    Table_FixSizeTempHashTableAggregate TempHashTable
                                (   
                                    TransContext, 
                                    DataMerger, 
                                    512,
                                    FieldDescription, 
                                    KeyNoList,
                                    (SAPDB_UInt8) KeyCnt/2,
                                    errList
                                );

    if ( !TempHashTable.CompletelyConstructed() )
        return false;

    for (SAPDB_UInt4 i=0; i<DataCnt; i++)
    {
        *((SAPDB_Int4* )&Data[0]) = i%KeyCnt;
        *((SAPDB_UInt4*)&Data[4]) = i;

        SAPDB_Byte* D = Data;
        if ( !TempHashTable.InsertOrReplace(D, errList) )
            return false;
    }

    SAPDB_UInt4 Count = 0, Sum=0;
    if ( !TempHashTable.First(errList) )
        return false;

    do
    {
        DataPtr = TempHashTable.Get();
        SAPDB_Int4 Key; SAPDB_UInt4 Val;
        Key = *((SAPDB_Int4* )&DataPtr[0]);
        Val = *((SAPDB_UInt4*)&DataPtr[4]);
        ++Count;
        Sum+=Val;
    } while ( TempHashTable.Next() );

    if ( Count == KeyCnt && Sum == (DataCnt*(DataCnt-1)) >> 1 )
        return true;
    else
    {
        errList = Table_Exception(__CONTEXT__,TABLE_COMPONENT_TEST_FAILED,"IncorrectData");
        return false;
    }
}

/* --------------------------------------------------------------------------- */

externCpp bool Table_Test (Trans_Context& trans)
{
    Table_Trace.SetTraceLevel(9);
    
    SAPDBTRACE_ROUTINE_DEBUG ("Table_Test", Table_Trace, 5);

    // 2005-01-18 UH use session errlist

    trans.OldTrans().trError_gg00 = e_ok;

    if ( 0 == trans.OldTrans().trLogContext_gg00 )
    {
        trans.OldTrans().trError_gg00 = e_not_implemented;
        return false;
    }

    bool                                  anyErrorOccured = false;
    Table_Id                              tableId;
    Kernel_IndexDescription::IndexNumber  indexno = 1;
    tgg00_TransContext                    AuxTrans;
    Msg_List                              localErrorList;
    
    k53StartInternalSession (trans.OldTrans(), AuxTrans, false, true);
    
    if ( trans.OldTrans().trError_gg00 != e_ok )
        return false;

    Log_Transaction &transContext = *(reinterpret_cast<Log_Transaction*>(trans.OldTrans().trLogContext_gg00));

    if ( ! anyErrorOccured )
    {
        trans.OldTrans().trError_gg00 = e_ok;

        Table_Table table (transContext);
        
        if ( table.Create( Table_Table::visibilityLocal,
                           Table_Table::removeKindEndOfTrans,
                           tableId,
                           localErrorList ) != Table_Table::createOk )
        {
            trans.ErrorList().AppendNewMessage(localErrorList);
            localErrorList.ClearMessageList();
            // ignored anyErrorOccured = true;
        }
    }
    
    if ( ! anyErrorOccured )
    {
        trans.OldTrans().trError_gg00 = e_ok;

        Kernel_IndexDescription indexDescr (trans.Allocator(), indexno, true);
        Table_Table             table      (transContext, tableId);
        
        if ( table.AddIndex(indexDescr, localErrorList) != Table_Table::addIndexOk )
        {
            trans.ErrorList().AppendNewMessage(localErrorList);
            localErrorList.ClearMessageList();
            // ignored anyErrorOccured = true;
        }
    }
    
    if ( ! anyErrorOccured )
    {
        trans.OldTrans().trError_gg00 = e_ok;

        SAPDB_Byte                  fieldlistBuffer [1024];
        SAPDB_Byte                  fieldBuffer     [64];
        Table_Table                 table  (transContext, tableId);
        SAPDBFields_AttributedField field  (fieldBuffer,     64  );
        SAPDBFields_FieldList       fields;
        
        fields.AssignNewBuffer(fieldlistBuffer, 1024);
        
        if ( ! fields.AppendField (field,localErrorList ) )
        {
            trans.ErrorList().AppendNewMessage(localErrorList);
            localErrorList.ClearMessageList();
            // anyErrorOccured = true;
        }

        if ( ! anyErrorOccured
             &&
             table.Insert(fields, localErrorList) != Table_Table::insertOk )
        {
            trans.ErrorList().AppendNewMessage(localErrorList);
            localErrorList.ClearMessageList();
            // anyErrorOccured = true;
        }
    }
    
    if ( ! anyErrorOccured )
    {
        trans.OldTrans().trError_gg00 = e_ok;

        Table_Table table (transContext, tableId);
        
        if ( table.DeleteIndex(indexno, localErrorList) != Table_Table::deleteIndexOk )
        {
            trans.ErrorList().AppendNewMessage(localErrorList);
            localErrorList.ClearMessageList();
            // anyErrorOccured = true;
        }
    }

    
    if ( ! anyErrorOccured )
    {
        trans.OldTrans().trError_gg00 = e_ok;

        Table_Table table (transContext, tableId);
        
        if ( table.DeleteIndex(indexno, localErrorList) != Table_Table::deleteIndexOk )
        {
            trans.ErrorList().AppendNewMessage(localErrorList);
            localErrorList.ClearMessageList();
            // anyErrorOccured = true;
        }
    }
    
    if ( ! anyErrorOccured )
    {
        trans.OldTrans().trError_gg00 = e_ok;

        Table_Table          table (transContext, tableId);
        Table_UnsortedResult result;
        
        if ( ! table.Select(result, localErrorList) )
        {
            trans.ErrorList().AppendNewMessage(localErrorList);
            localErrorList.ClearMessageList();
            // anyErrorOccured = true;
        }
    }

    if ( ! anyErrorOccured )
    {
        trans.OldTrans().trError_gg00 = e_ok;
        if ( !TempHashTable_Test(trans.OldTrans(), localErrorList) )
        {
            trans.ErrorList().AppendNewMessage(localErrorList);
            localErrorList.ClearMessageList();
            // anyErrorOccured = true;
        }
    }

    k53StopInternalSession  (trans.OldTrans(), AuxTrans, m_rollback);

    Table_Trace.SetTraceLevel(0);
    if ( anyErrorOccured )
        return false;
	else
    {
        trans.ErrorList().ClearMessageList(); // clear unneccessary messages
        return true;
    }
    return true;
}
