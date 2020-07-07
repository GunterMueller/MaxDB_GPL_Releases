/*
  @file         vbd491.cpp
  @author       TorstenS
  @ingroup      create index
  @brief        This modul is used to sort and materialze the primary
                records of a table to be indexed.

\if EMIT_LICENCE


    ========== licence begin  GPL
    Copyright (c) 1999-2005 SAP AG

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



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

// Content of include files
#include "gbd490.h"
#include "gbd491.h"
#include "gbd492.h"
#include "ggg200.h"   // CPP   : Sort Templates

#include "hbd13.h"    // PASCAL: Nodehandling
#include "hbd30.h"    // PASCAL: Treehandling  
#include "hbd31.h"    // PASCAL: Leafhandling 
#include "hbd35.h"    // PASCAL: entryhandling 
#include "hbd50.h"    // PASCAL: Indexhandling
#include "hbd400.h"   // CPP   : Inverted Lists Add/Delete
#include "hgg04.h"    // PASCAL: Select_Help_Procedures
#include "hsp30.h"    // PASCAL: Buffer handling and comparison routines

#include "gbd300.h"  // CPP   : InvHandling
#include "gbd493.h"  // cbd493_CreateIndexBlockAllocator
#include "gbd495.h"  // CPP   : DoubleQueueHandling

#include "ConsistentRead/CnsRead_CachedRow.hpp"
#include "Container/Container_Vector.hpp"
#include "DataAccess/Data_Exceptions.hpp"
#include "DataAccess/Data_Messages.hpp"
#include "DataAccess/Data_Row.hpp"
#include "Logging/Log_InvDescMap.hpp"
#include "KernelCommon/Kernel_VTrace.hpp"
#include "KernelCommon/Kernel_Trace.hpp"
#include "RunTime/RTE_Message.hpp"
#include "SAPDBCommon/SAPDB_RangeCode.hpp" // Kernel_move_and_fill
#include "SAPDBCommon/Fields/SAPDBFields_Field.hpp"
#include "SAPDBCommon/Fields/SAPDBFields_FieldCollection.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  FORWARD DECLARATION                                                      *
 *===========================================================================*/

class cbd491_InvRecordCollection;


/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

/*---------------------------------------------------------------------------*/
/* CLASS cbd491_InvRecord / old cbd491_InvRecDescItem                        */
/*---------------------------------------------------------------------------*/

class cbd491_InvRecord
{
public:

    cbd491_InvRecord(
        SAPDBMem_IRawAllocator& allocator,
        tgg00_BasisError&       error,
        cbd497_StackDescInfo&   stackDescInfo,
        SAPDB_Byte*             pUndefNullKey = 0 )
    :
    m_Allocator( allocator ),
    m_TrError( error ),
    m_Stack( stackDescInfo ),
    m_pUndefNullKey( pUndefNullKey )
    {}

    ~cbd491_InvRecord()
    {
        if( ! m_Record.IsAssigned()){
            return;
        }
        m_Allocator.Deallocate( m_Record.GetDataPtr());
    }

    bool Initialize()
    {
        const SAPDB_UInt maxRecLen = sizeof( tgg00_Rec);
        SAPDB_Byte* pRecordSpace = reinterpret_cast<SAPDB_Byte*>( m_Allocator.Allocate( maxRecLen ));
        if( 0 == pRecordSpace ){
            m_TrError = e_no_more_memory;
            return false;
        }
        m_Record.Assign( pRecordSpace, maxRecLen );
        return true;
    }

    // Create a field collection for the given record
    void BuildVirtual(
        const tgg00_Rec* const  pPrimRecord,
        cbd492_StorageArea&     storageArea,
        SAPDBFields_FieldCollection&    fields );

    // Materialize the primary key in the given space
    void MaterializePrimKey (
        const SAPDBFields_FieldCollection&  fields,
        SAPDB_Byte*  const                  pPrimKeySpace,
        const SAPDB_UInt                    reservedSpace )
    {
        if( e_ok != m_TrError ){
            return;
        }
        SAPDBFields_Field primKeyField = GetPrimKeyField( fields );

        SAPDB_RangeMove( __FILE__, 1,
                         primKeyField.GetLength(), reservedSpace,
                         primKeyField.GetDataPtr(), POS_OFF_DIFF_BD00,
                         pPrimKeySpace, POS_OFF_DIFF_BD00,
                         primKeyField.GetLength(), m_TrError );
        SAPDBERR_ASSERT_STATE( e_ok == m_TrError );
    }

    // Materialize the secondary key in the given space
    void MaterializeSecKey (
        const SAPDBFields_FieldCollection&  fields,
        SAPDB_Byte* const                   pSecKeySpace,
        const SAPDB_UInt                    reservedSpace );

    // in case of an invalid field zero is returned as length
    SAPDB_UInt GetPrimKeyLen( const SAPDBFields_FieldCollection&    fields ) const{
        return this->GetPrimKeyField( fields ).GetLength();
    }

    //
    SAPDB_UInt GetSecKeyLen( const SAPDBFields_FieldCollection& fields ) const;

    // returns number of secondary key columns inclusive one primary key
    SAPDB_UInt MaxNumberOfColumns() const{
        return m_Stack.bd497NumColumns() + 1;
    }

    tsp00_LcompResult CompareVirtualInclusivePrimKey(
        const SAPDBFields_FieldCollection&  leftFields,
        const SAPDBFields_FieldCollection&  rightFields ) const
    {
        return CompareVirtual( true, leftFields, rightFields );
    }

    tsp00_LcompResult CompareVirtualExclusivePrimKey(
        const SAPDBFields_FieldCollection&  leftFields,
        const SAPDBFields_FieldCollection&  rightFields ) const
    {
        return CompareVirtual( false, leftFields, rightFields );
    }

    void CheckKeyOrder(
        const tsp00_LcompResult             secKeyCompResult,
        const cbd491_InvRecordCollection&   invRecCollection,
        const SAPDBFields_FieldCollection&  currFields,
        const SAPDBFields_FieldCollection&  nextFields ) const;

    void TraceFields(
        const SAPDBFields_FieldCollection&  fields,
        const SAPDB_Char* const             title = 0 ) const;

protected:

    SAPDB_UInt PrimKeyFieldNo() const{
        return 0;
    }

    SAPDBFields_Field GetPrimKeyField( const SAPDBFields_FieldCollection &fields ) const
    {
        SAPDBFields_Field   primKeyField;
        fields.GetField( this->PrimKeyFieldNo(), primKeyField );
        return primKeyField;
    }

    tsp00_LcompResult ColumnCompare(
        const bool                  bIsColumnDescending,
        const bool                  bLastColumn,
        bool&                       bAllLeftColumnsAreNull,  // PTS 1124759 TS 2003-10-20
        bool&                       bAllRightColumnsAreNull, // PTS 1124759 TS 2003-10-20
        const SAPDBFields_Field&    leftField,
        const SAPDBFields_Field&    rightField ) const;

private:

    tsp00_LcompResult CompareVirtual(
        const bool                          bInclusivePrimKey,
        const SAPDBFields_FieldCollection&  leftFields,
        const SAPDBFields_FieldCollection&  rightFields ) const;

private:

    // Assignment operator is not supported
    cbd491_InvRecord& operator=( const cbd491_InvRecord &ir );

    // Copy constructor is not supported
    cbd491_InvRecord( const cbd491_InvRecord &ir );

protected:

    // used to get memory for m_Record
    SAPDBMem_IRawAllocator& m_Allocator;

    tgg00_BasisError&       m_TrError;

    // Stack code used to find the columns to be indexed
    cbd497_StackDescInfo&   m_Stack;

    // space for temporary record copy, which is the base for the columns to be indexed
    SAPDBFields_Field	    m_Record;

    // could be const but field.Assign is not const!
    SAPDB_Byte*             m_pUndefNullKey;
};

/*---------------------------------------------------------------------------*/
/* CLASS cbd491_InvRecordComperator / old  cbd491_CompareInvRecDescItem      */
/*---------------------------------------------------------------------------*/

typedef SAPDBFields_Field* SAPDBFields_FieldPointer;

class cbd491_InvRecordComperator :
            public cbd491_InvRecord,
            public cgg200ComparisonOperator< SAPDBFields_FieldPointer >
{
public:

    cbd491_InvRecordComperator(
        SAPDBMem_IRawAllocator&     allocator,
        cbd491_InvRecordCollection& invRecordCollection,
        tgg00_BasisError&           error,
        cbd497_StackDescInfo&       stackDescInfo );

    bool Initialize()
    {
        if( ! m_LeftFields.Initialize( m_FieldsPerRecord )){
            return false;
        }
        if( ! m_RightFields.Initialize( m_FieldsPerRecord )){
            return false;
        }
        return true;
    }

    // Define a compare operator which is needed by the sort template

    tsp00_LcompResult_Enum gg200Compare (
        const SAPDBFields_FieldPointer& pFirstLeft,
        const SAPDBFields_FieldPointer& pFirstRight )
    {
        SAPDBFields_FieldCollection::IndexType fieldNo;
        for( fieldNo = 0; fieldNo < m_FieldsPerRecord; fieldNo++ )
        {
            m_LeftFields.Add( fieldNo, *( pFirstLeft + fieldNo ));
            m_RightFields.Add( fieldNo, *( pFirstRight + fieldNo ));
        }

        const tsp00_LcompResult_Enum compResult =
            cbd491_InvRecord::CompareVirtualInclusivePrimKey( m_LeftFields, m_RightFields );
        return compResult;
    }
    
private:

        // Assignment operator is not supported
    cbd491_InvRecordComperator& operator=( const cbd491_InvRecordComperator& irc );

        // Copy constructor is not supported
    cbd491_InvRecordComperator( const cbd491_InvRecordComperator& irc );
        
private:

    const SAPDB_UInt                m_FieldsPerRecord;
    cbd491_InvRecordCollection&     m_InvRecordCollection;

    SAPDBFields_FieldCollection     m_LeftFields;
    SAPDBFields_FieldCollection     m_RightFields;
};

/*---------------------------------------------------------------------------*/
/* CLASS cbd491_InvRecordCollection / old cbd491_InvRecDescList              */
/*---------------------------------------------------------------------------*/

class cbd491_InvRecordCollection
{
public:

    cbd491_InvRecordCollection(
        SAPDBMem_IRawAllocator& allocator,
        const SAPDB_UInt        numSecKeyColumns )
    :
    m_NumPrimKeyFields( 1 ),
    m_NumSecKeyFields( numSecKeyColumns ),
    m_FieldsPerRecord( m_NumPrimKeyFields + m_NumSecKeyFields ),
    m_InvRecordAnchorSortList( allocator ),
    m_FieldCollection( allocator )
    {}
    
    ~cbd491_InvRecordCollection()
    {
        m_InvRecordAnchorSortList.Delete();
        m_FieldCollection.Delete();
    }

    bool Initialize( const SAPDB_UInt   numPrimaryRecords )
    {
        if( ! m_InvRecordAnchorSortList.Reserve( numPrimaryRecords )){
            return false;
        }
        if( ! m_FieldCollection.Reserve( numPrimaryRecords * m_FieldsPerRecord )){
            return false;
        }
        return true;
    }

    SAPDB_UInt NumberOfFieldsPerRecord() const{
        return m_FieldsPerRecord;
    }

    // bd491_NumRecDescUsed
    SAPDB_UInt  GetUsedCount() const{
        return m_FieldCollection.GetSize()/m_FieldsPerRecord;
    }

    // bd491_NewRecDesc
    bool Append( const SAPDBFields_FieldCollection& fields )
    {
        SAPDBERR_ASSERT_STATE( fields.GetFieldCount() == m_FieldsPerRecord );

        if( m_FieldCollection.GetSize() + fields.GetFieldCount() > m_FieldCollection.GetCapacity())
        {
            const SAPDB_UInt   oldMaxPrimaryRecords = this->GetUsedCount();
            const SAPDB_UInt   newMaxPrimaryRecords = oldMaxPrimaryRecords + oldMaxPrimaryRecords/10;
            if( ! this->Initialize( newMaxPrimaryRecords )){
                return false;
            }
        }
        SAPDBFields_FieldCollection::IndexType fieldNo;
        SAPDBFields_Field field;

        for( fieldNo = 0; fieldNo < fields.GetFieldCount(); fieldNo++ )
        {
            if( ! fields.GetField( fieldNo, field )){
                return false;
            }
            m_FieldCollection.InsertEnd( field );
        }
        return true;
    }

    // This method provides the requested fields in append order!
    // The first valid index is zero and the last valid one is GetUsedCount()-1
    bool Get(
        const SAPDB_UInt                index,
        SAPDBFields_FieldCollection&    fields ) const
    {
        if( index >= this->GetUsedCount()){
            return false;
        }
        SAPDBERR_ASSERT_STATE( fields.GetMaxFieldCount() == this->NumberOfFieldsPerRecord());

        const SAPDBFields_FieldCollection::IndexType startIndex = m_FieldsPerRecord * index ;
        SAPDBFields_FieldCollection::IndexType       fieldNo;
        bool  ok = true;

        for( fieldNo = 0; fieldNo < m_FieldsPerRecord; fieldNo++ ){
            ok &= fields.Add( fieldNo, m_FieldCollection[ startIndex + fieldNo ]);
        }
        SAPDBERR_ASSERT_STATE( ok );
        return ok;
    }

    // This method provides the requested fields in ascending order! This requires
    // that the Sort() method was called before, otherwise no fields can be returned
    // which is indicated by returncode false. Note that per default the fields are
    // append and therefore the sort order is random. The first valid index is zero
    // and the last valid one is GetUsedCount()-1
    bool GetSorted(
        const SAPDB_UInt                index,
        SAPDBFields_FieldCollection&    fields ) const
    {

        if(( index >= this->GetUsedCount()) || ( index >= m_InvRecordAnchorSortList.GetSize())){
            return false;
        }
        SAPDBERR_ASSERT_STATE( fields.GetMaxFieldCount() == this->NumberOfFieldsPerRecord());

        const SAPDBFields_Field* pField = m_InvRecordAnchorSortList[ index ];
        SAPDBFields_FieldCollection::IndexType  fieldNo;
        bool  ok = true;

        for( fieldNo = 0; fieldNo < m_FieldsPerRecord; fieldNo++ ){
            ok &= fields.Add( fieldNo, *( pField + fieldNo ) );
        }
        SAPDBERR_ASSERT_STATE( ok );
        return ok;
    }

    bool Sort( cbd491_InvRecordComperator &comperator )
    {
        SAPDBFields_FieldCollection::IndexType fieldNo;

        for( fieldNo = 0; fieldNo < m_FieldCollection.GetSize(); fieldNo+=m_FieldsPerRecord )
        {
            Fields::Iterator    iter = m_FieldCollection.Position( fieldNo );
            m_InvRecordAnchorSortList.InsertEnd( iter );
        }
        const bool retCode = gg200QuickSort( m_InvRecordAnchorSortList.Position( 0 ),
                                             this->GetUsedCount(), comperator );
        return retCode;
    }

    void WriteToTrace(
        const bool               bSuppressFristField = false,
        const SAPDB_Char* const  title = 0 ) const
    {
        Kernel_VTrace   trace;

        if( 0 != title ){
            trace << title << FlushLine;
        }
        trace << "FieldList contains " << this->GetUsedCount()
        << " records with " << m_FieldsPerRecord << " fields per record" << FlushLine;
        SAPDBFields_FieldCollection::IndexType  fieldNo;
        for( fieldNo = 0; fieldNo < m_FieldCollection.GetSize(); fieldNo++ )
        {
            const SAPDBFields_Field& field = m_FieldCollection[ fieldNo ];
            if(( bSuppressFristField )&& ( 0 == ( fieldNo % m_FieldsPerRecord ))){
                continue;
            }
            trace << "Record#" << ( fieldNo / m_FieldsPerRecord ) << "/Field#"
            << ( fieldNo % m_FieldsPerRecord ) << "(" << field.GetLength()
            << "): " << FlushLine;
            field.WriteToTrace();
        }
    }

private:

    // Assignment operator is not supported
    cbd491_InvRecordCollection& operator=( const cbd491_InvRecordCollection &irc );

    // Copy constructor is not supported
    cbd491_InvRecordCollection( const cbd491_InvRecordCollection &irc );

private:

    typedef Container_Vector< SAPDBFields_Field*>   FieldAnchorPointers;
    typedef Container_Vector< SAPDBFields_Field>    Fields;

    // Number of fields the primary key consists of. Per default one, because
    // the primary key will be copied in one part from the primary page into
    // the auxilliary page resp. destination index page.
    const SAPDB_UInt    m_NumPrimKeyFields;

    // Number of fields the secondary key consists of. If a compound secondary key
    // consists of three columns m_NumSecKeyDesc is set to three.
    const SAPDB_UInt    m_NumSecKeyFields;

    // Is the number of column descriptions which are needed to address all columns
    // of one primary record which are relevant for the index creation
    const SAPDB_UInt    m_FieldsPerRecord;

    // Collection of anchors. Each anchor leads to the fields of a primary
    // record to be indexed.  So each anchor gives information about the primary
    // key and about all columns in the primary record which will be the new
    // secondary key. This list is used by the quick sort method!
    FieldAnchorPointers   m_InvRecordAnchorSortList; // m_InvRecDescList

    // Contains the information where each field which will be part of the new index
    // record is located. The strucuture of the field collection for ONE index
    // record looks like this: [primKeyColumn, firstSecKeyColumn, secondSecKeyColumn]
    // then the field collection of the next index record follows in the same manner.
    Fields  m_FieldCollection; // m_KeyDescPool
};

/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/



/*===========================================================================*
 *  GLOBAL VARIABLES                                                         *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL VARIABLES                                                          *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL FUNCTIONS (PROTOTYPES)                                             *
 *===========================================================================*/



/*===========================================================================*
 *  GLOBAL FUNCTIONS (CODE)                                                  *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL FUNCTIONS (CODE)                                                   *
 *===========================================================================*/

// PTS 1108452 TS 2000-11-24
inline tsp00_LcompResult
bd491_CompareASCIIColumn (
    const SAPDB_Byte	    DefineByte,
    const SAPDB_Byte* const	pKey,
    const SAPDB_Int4	    KeyLen,
    SAPDB_Int4	            iOffset )
{
    // ASCII defined column

    while ( iOffset < KeyLen )
    {
        if ( *(pKey + iOffset) == DefineByte ){
            ++iOffset;
        }
        else
        {
            if ( *(pKey + iOffset) > DefineByte )
                return tsp00_LcompResult::fromConst (l_greater);
            else
                return tsp00_LcompResult::fromConst (l_less);
        }
    }
    return tsp00_LcompResult::fromConst (l_equal);
}

/*---------------------------------------------------------------------------*/

// PTS 1108452 TS 2000-11-24
inline tsp00_LcompResult
bd491_CompareUnicodeColumn (
    const SAPDB_Byte* const pKey,
    const SAPDB_Int4	    KeyLen,
    SAPDB_Int4		        iOffset )
{
    // UNICODE (UCS-2) defined column

    // PTS 1108380 TS 2000-12-27
    // No more warnings on SUN because csp_unicode_blank is used inside the
    // the function instead of char *DefineBytes in parameter call list

    while ( iOffset < KeyLen )
    {
        if (
            ( *(pKey + iOffset  ) == csp_unicode_blank[0] ) &&
            ( *(pKey + iOffset+1) == csp_unicode_blank[1] )
        )
        {
            iOffset +=2;
        }
        else
        {
            if ( *(pKey + iOffset) == csp_unicode_blank[0] )
            {
                if ( *(pKey + iOffset +1) > csp_unicode_blank[1] )
                    return tsp00_LcompResult::fromConst (l_greater);
                else
                    return tsp00_LcompResult::fromConst (l_less);
            }

            if ( *(pKey + iOffset) > csp_unicode_blank[0] )
                return tsp00_LcompResult::fromConst (l_greater);
            else
                return tsp00_LcompResult::fromConst (l_less);
        }
    }
    return tsp00_LcompResult::fromConst (l_equal);
}

/*===========================================================================*
 *   DEFINITION OF METHODS OF LOCAL CLASSES (CODE)                           *
 *===========================================================================*/

void
cbd491_InvRecord::BuildVirtual(
    const tgg00_Rec* const          pPrimRecord,
    cbd492_StorageArea&				storageArea,
    SAPDBFields_FieldCollection&    fields )
{
    SAPDBERR_ASSERT_STATE( m_Record.IsAssigned());
    SAPDBERR_ASSERT_STATE( fields.GetMaxFieldCount() == this->MaxNumberOfColumns());
    SAPDBERR_ASSERT_STATE( 0 != m_pUndefNullKey ); // wrong constructor used

    const CnsRead_CachedRow* pRow = reinterpret_cast<const CnsRead_CachedRow*>( pPrimRecord );
    if(  pRow->IsDeleted()){ // not relevant for index creation
        return; // fields.GetFieldCount() == 0
    }
    const bool	bWithTransInfo = true;
    SAPDB_Int4	destOffset     = 0;
    // copy suitable record image - beware if consistent read and function based indexes - into
    // auxillary buffer, before the columns to be indexed are copied in the storage area to
    // make them comparable without building the complete index record first
    pRow->Copy( ! bWithTransInfo, m_Record.GetLength(), m_Record.GetDataPtr(), destOffset, m_TrError );
    if( e_ok != m_TrError )
    {
        Kernel_VTrace() << "ERROR: cbd491_InvRecord::BuildVirtual() - Could not copy "
        << pRow->GetRecLen() << " bytes into record buffer["
        << m_Record.GetLength() << "]" << FlushLine;
        SAPDBERR_ASSERT_STATE( e_ok == m_TrError );
        return;
    }
    const tgg00_RecPtr  pRec       = reinterpret_cast< tgg00_RecPtr >( m_Record.GetDataPtr());
    const SAPDB_Int4    primKeyLen = pRow->GetKeyLen();
    const SAPDB_Int4    lastColumn = m_Stack.bd497LastColumn();

    bool                bColumnIsEmpty;
    SAPDB_Int4          currColumn = m_Stack.bd497FirstColumn ();
    SAPDB_Int4          currColumnLen;
    SAPDB_Int4          currColumnPos;
    tgg00_VarColPosList varColPos;
    tgg00_StEntryAddr   pStackEntry = 0;

    varColPos.vpl_last = -1;

    SAPDBFields_Field   destField;
    SAPDB_Byte*         pCurrColumn = 0;

    do
    {
        pStackEntry = m_Stack.bd497GetStackEntryPtr( currColumn );
        if( 0 == pStackEntry ){
            return; // m_TrError = e_stack_type_illegal
        }
        g04locate_col( *pStackEntry, pRec, varColPos, currColumnPos, currColumnLen );

        if(( 1 < currColumnLen ) && ( currColumn == lastColumn ))
        {
            // Cut trailing define bytes of the last (!) indexed column by reducing the column length
            currColumnLen = 1 + // Define byte
                            s30lnr_defbyte( pRec,
                                            pRec->recBuf_gg00()[ currColumnPos - POS_OFF_DIFF_BD00 ],
                                            currColumnPos + 1, currColumnLen - 1 );
        }
        bColumnIsEmpty = (( 0 == currColumnLen ) ||
                          ( csp_undef_byte == pRec->recBuf_gg00()[ currColumnPos - POS_OFF_DIFF_BD00 ]));
        {
            // At first assign primary key column(s) as one to the given field collection
            if( 1 == currColumn )
            {
                SAPDBERR_ASSERT_STATE( primKeyLen == pRec->recKeyLen_gg00());
                if( ! storageArea.Add( &pRec->recBody_gg00()[0], primKeyLen, destField )){
                    m_TrError = e_no_more_memory;
                    return;
                }
                fields.Add( this->PrimKeyFieldNo(), destField );
            }
        }
        {
            // Set pointer on the columns representing the secondary key
            if( bColumnIsEmpty ) // Simulate "null-value" column for multiple indexes
            {
                if(( currColumn == lastColumn ) && (! m_Stack.bd497IsColumnDescending( currColumn ))){
                    currColumnLen = 1;
                }
                else{
                    currColumnLen = m_Stack.bd497GetColumnLength( currColumn );
                }
                pCurrColumn =  m_pUndefNullKey;
            }
            else{
                pCurrColumn = &pRec->recBuf_gg00()[ currColumnPos - POS_OFF_DIFF_BD00 ];
            }
            if( ! storageArea.Add( pCurrColumn, currColumnLen, destField )){
                m_TrError = e_no_more_memory;
                return;
            }
            fields.Add( currColumn, destField );
        }
        ++currColumn;
    }
    while(( e_ok == m_TrError ) && ( currColumn <= lastColumn ));
}

/*---------------------------------------------------------------------------*/

void
cbd491_InvRecord::MaterializeSecKey (
    const SAPDBFields_FieldCollection&  fields,
    SAPDB_Byte* const                   pSecKeySpace,
    const SAPDB_UInt                    reservedSpace )
{
    if( e_ok != m_TrError ){
        return;
    }
    const SAPDB_Int4 lastColumn  = m_Stack.bd497LastColumn();
    SAPDB_Int4  currColumn       = m_Stack.bd497FirstColumn();
    SAPDB_Int4  currSecKeyLength = 0;

    SAPDBERR_ASSERT_STATE( fields.GetMaxFieldCount() == this->MaxNumberOfColumns());

    SAPDBFields_Field   field;

    do
    {
        fields.GetField( currColumn, field );
        const SAPDB_UInt reservedSpaceOffset = currSecKeyLength;
        const SAPDB_UInt currColumnLength    = field.GetLength();

        SAPDB_RangeMove( __FILE__, 1,
                         currColumnLength, reservedSpace,
                         field.GetDataPtr(), POS_OFF_DIFF_BD00,
                         pSecKeySpace, reservedSpaceOffset + POS_OFF_DIFF_BD00,
                         currColumnLength, m_TrError );
        SAPDBERR_ASSERT_STATE( e_ok == m_TrError );
        if( e_ok != m_TrError ){
            return;
        }
        const bool bIsColumnDescending = m_Stack.bd497IsColumnDescending( currColumn );
        if(( currColumn == lastColumn ) &&( ! bIsColumnDescending )){
            currSecKeyLength += currColumnLength;
        }
        else
        {
            const SAPDB_UInt fixedColumnLength = m_Stack.bd497GetColumnLength (currColumn);
            if( currColumnLength < fixedColumnLength )
            {
                if( csp_unicode_def_byte == *( pSecKeySpace + reservedSpaceOffset )){
                    SAPDB_RangeUnicodeFill ("VBD491", 1,
                                            reservedSpace, pSecKeySpace,
                                            reservedSpaceOffset + currColumnLength + POS_OFF_DIFF_BD00,
                                            fixedColumnLength - currColumnLength,
                                            csp_unicode_blank, m_TrError );
                }
                else{
                    SAPDB_RangeFill( __FILE__, 3,
                                     reservedSpace, pSecKeySpace,
                                     reservedSpaceOffset + currColumnLength + POS_OFF_DIFF_BD00,
                                     fixedColumnLength - currColumnLength,
                                     *( pSecKeySpace + reservedSpaceOffset ), m_TrError );
                }
                SAPDBERR_ASSERT_STATE( e_ok == m_TrError );
            }
            if( e_ok != m_TrError ){
                return;
            }
            currSecKeyLength += fixedColumnLength;
        }
        if( bIsColumnDescending )
        {
            for( SAPDB_Int4 i = reservedSpaceOffset; i < currSecKeyLength; ++i ){
                *( pSecKeySpace + i ) = ~*( pSecKeySpace + i );
            }
        }
        ++currColumn;
    }
    while(( e_ok == m_TrError ) && ( currColumn <= lastColumn ));
}

/*--------------------------------------------------------------------------*/

tsp00_LcompResult
cbd491_InvRecord::ColumnCompare(
    const bool                  bIsColumnDescending,
    const bool                  bLastColumn,
    bool&                       bAllLeftColumnsAreNull,  // PTS 1124759 TS 2003-10-20
    bool&                       bAllRightColumnsAreNull, // PTS 1124759 TS 2003-10-20
    const SAPDBFields_Field&    leftField,
    const SAPDBFields_Field&    rightField ) const
{
    // Is one of both columns empty
    const SAPDB_Byte* pKeyLeft    = leftField.GetDataPtr();
    const SAPDB_Byte* pKeyRight   = rightField.GetDataPtr();
    const SAPDB_Int4  KeyLenLeft  = leftField.GetLength();
    const tsp00_Int4  KeyLenRight = rightField.GetLength();

    if ( 0 >= KeyLenLeft )
    {
        if ( 0 >= KeyLenRight )
            return tsp00_LcompResult::fromConst (l_equal);
        else
            return tsp00_LcompResult::fromConst (l_less);
    }
    else if ( 0 >= KeyLenRight )
        return tsp00_LcompResult::fromConst (l_greater);

    // KeyLenLeft > 0 and KeyLenRight > 0

    if( csp_undef_byte != *pKeyLeft ) { // PTS 1124759 TS 2003-10-20
        bAllLeftColumnsAreNull = false; // at least one column is not null
    }
    if( csp_undef_byte != *pKeyRight ){ // PTS 1124759 TS 2003-10-20
        bAllRightColumnsAreNull = false; // at least one column is not null
    }

    const SAPDB_Int4    PrefixLen = (KeyLenLeft > KeyLenRight) ? KeyLenRight : KeyLenLeft;
    SAPDB_Int4          iOffset = 0;

    while ( iOffset < PrefixLen )
    {
        if( *(pKeyLeft + iOffset) == *(pKeyRight + iOffset )){
            ++iOffset;
        }
        else
        {
            if ( *(pKeyLeft + iOffset) > *(pKeyRight + iOffset) )
                return tsp00_LcompResult::fromConst (l_greater);
            else
                return tsp00_LcompResult::fromConst (l_less);
        }
    }

    // iOffset >= PrefixLen and CompResult == l_equal

    if (KeyLenLeft == KeyLenRight){
        return tsp00_LcompResult::fromConst (l_equal);
    }

    if(( bLastColumn ) && (! bIsColumnDescending ))
    {
        if (KeyLenLeft > KeyLenRight)
            return tsp00_LcompResult::fromConst (l_greater);
        else
            return tsp00_LcompResult::fromConst (l_less);
    }
    else
    {
        // Handle 'NON-LAST' columns of a multiple index and
        // the LAST column of multiple or single index if it
        // is descending. Pay attention to bytes smaller than
        // the define byte!
        // Note that the longer column could be smaller than the
        // shorter column, namely if the longer one is filled with
        // bytes whose value is smaller than the define byte.
        // For example left column contains "<define byte>,A" and
        // right column contains "<define byte>,A,<BEL>. The right
        // column is smaller although is longer because the value of
        // the <BEL> byte is smaller than the define byte left column
        // is filled up.

        // KeyLenLeft <> KeyLenRight and iOffset 'points' to the
        // first different byte

        if (KeyLenLeft > KeyLenRight)
        {
            const SAPDB_Byte	DefineByte = *pKeyRight;

            if (csp_unicode_def_byte == DefineByte)
                return bd491_CompareUnicodeColumn (pKeyLeft, KeyLenLeft, iOffset);
            else
                return bd491_CompareASCIIColumn (DefineByte, pKeyLeft, KeyLenLeft, iOffset);
        }
        else
        {
            const SAPDB_Byte	DefineByte = *pKeyLeft;

            if (csp_unicode_def_byte == DefineByte)
            {
                const tsp00_LcompResult CompResult =
                    bd491_CompareUnicodeColumn (pKeyRight, KeyLenRight, iOffset);

                if (l_less == CompResult)
                    return tsp00_LcompResult::fromConst (l_greater);
                else if (l_greater == CompResult)
                    return tsp00_LcompResult::fromConst (l_less);
                else
                    return CompResult;
            }
            else
            {
                const tsp00_LcompResult CompResult =
                    bd491_CompareASCIIColumn (DefineByte, pKeyRight, KeyLenRight, iOffset);

                if (l_less == CompResult)
                    return tsp00_LcompResult::fromConst (l_greater);
                else if (l_greater == CompResult)
                    return tsp00_LcompResult::fromConst (l_less);
                else
                    return CompResult;
            }
        }
    }
}

/*---------------------------------------------------------------------------*/

tsp00_LcompResult
cbd491_InvRecord::CompareVirtual(
    const bool                          bInclusivePrimKey,
    const SAPDBFields_FieldCollection&  leftFields,
    const SAPDBFields_FieldCollection&  rightFields ) const
{
    if( e_ok != m_TrError ){
        return tsp00_LcompResult::fromConst( l_undef );
    }

    const SAPDB_Int4    lastColumn = m_Stack.bd497LastColumn();
    SAPDB_Int4          currColumn = m_Stack.bd497FirstColumn();
    tsp00_LcompResult   compResult;

    bool    bAllLeftColumnsAreNull  = true; // PTS 1124759 TS 2003-10-20
    bool    bAllRightColumnsAreNull = true; // PTS 1124759 TS 2003-10-20

    SAPDBFields_Field   leftField;
    SAPDBFields_Field   rightField;

    do
    {
        const bool bIsColumnDescending = m_Stack.bd497IsColumnDescending( currColumn );

        leftFields.GetField( currColumn, leftField );
        rightFields.GetField( currColumn, rightField );

        compResult = this->ColumnCompare( bIsColumnDescending, currColumn == lastColumn,
                                          bAllLeftColumnsAreNull, bAllRightColumnsAreNull, leftField, rightField );
        if ( bIsColumnDescending )
        {
            // Note that in the case of descending order the corresponding column
            // is temporary 'build' as an ascending column, because the complement of
            // the bytes is quit difficult to build with column pointers pointing
            // direct into the primary records. (Don't change primary data!)
            // Therefore descending columns handled as ascending columns and final
            // the compare result will be reversed.

            if( l_less == compResult ){
                compResult = ( tsp00_LcompResult::fromConst( l_greater ));
            }
            else if( l_greater == compResult ){
                compResult = ( tsp00_LcompResult::fromConst( l_less ));
            }
        }
        ++currColumn;
    }
    while(( l_equal == compResult ) && ( e_ok == m_TrError ) && ( currColumn <= lastColumn ));

    if(( e_ok == m_TrError ) && ( l_equal == compResult ) && ( bInclusivePrimKey ))
    {
        // secondary key columns are equal, therefore the primary keys have to be compared
        leftFields.GetField( this->PrimKeyFieldNo(), leftField );
        rightFields.GetField( this->PrimKeyFieldNo(), rightField );

        s30cmp( leftField.GetDataPtr(), POS_OFF_DIFF_BD00, leftField.GetLength(),
                rightField.GetDataPtr(), POS_OFF_DIFF_BD00, rightField.GetLength(), compResult );

        const bool bBothSecKeysAreNull = bAllLeftColumnsAreNull && bAllRightColumnsAreNull;

        if (
            (m_Stack.bd497IsInvUnique()) &&
            (l_equal != compResult     ) && // primary keys are different
            (! bBothSecKeysAreNull     )    // PTS 1124759 TS 2003-10-20
        )
        {
            m_TrError = e_duplicate_invkey;
        }
    }
    return compResult;
}

/*---------------------------------------------------------------------------*/

inline SAPDB_UInt
cbd491_InvRecord::GetSecKeyLen( const SAPDBFields_FieldCollection &fields ) const
{
    const SAPDB_Int4    lastColumn = m_Stack.bd497LastColumn();
    SAPDB_Int4          currColumn = m_Stack.bd497FirstColumn();
    SAPDB_Int4          secKeyLen  = 0;

    do
    {
        if(( currColumn == lastColumn ) && (! m_Stack.bd497IsColumnDescending( currColumn )))
        {
            SAPDBFields_Field   field;
            fields.GetField( currColumn, field );
            secKeyLen += field.GetLength();
        }
        else{
            secKeyLen += m_Stack.bd497GetColumnLength( currColumn );
        }
        ++currColumn;
    }
    while(( e_ok == m_TrError ) && ( currColumn <= lastColumn ));
    return secKeyLen;
}

/*---------------------------------------------------------------------------*/

inline void
cbd491_InvRecord::CheckKeyOrder(
    const tsp00_LcompResult             secKeyCompResult,
    const cbd491_InvRecordCollection&   invRecCollection,
    const SAPDBFields_FieldCollection&  currFields,
    const SAPDBFields_FieldCollection&  nextFields ) const
{
    Kernel_VTrace vTrace;
    if( l_greater == secKeyCompResult )
    {
        invRecCollection.WriteToTrace();
        vTrace << "+++ ERROR: cbd491_InvRecord::CheckKeyOrder - seckey order not ascending +++" << FlushLine;
        this->TraceFields( currFields, "Left record: " );
        this->TraceFields( nextFields, "Right record: " );

        SAPDBERR_ASSERT_STATE( l_greater != secKeyCompResult );
    }
    if( l_equal == secKeyCompResult)
    {
        tsp00_LcompResult   primCompResult( tsp00_LcompResult::fromConst( l_undef ));

        const SAPDBFields_Field currPrimKeyField = GetPrimKeyField( currFields );
        const SAPDBFields_Field nextPrimKeyField = GetPrimKeyField( nextFields );

        s30cmp( currPrimKeyField.GetDataPtr(), POS_OFF_DIFF_BD00, currPrimKeyField.GetLength(),
                nextPrimKeyField.GetDataPtr(), POS_OFF_DIFF_BD00, nextPrimKeyField.GetLength(),
                primCompResult );

        if( l_less != primCompResult )
        {
            invRecCollection.WriteToTrace();
            vTrace << "+++ ERROR: cbd491_InvRecord::CheckKeyOrder - primkey order not ascending +++" << FlushLine;
            currPrimKeyField.WriteToTrace( "Left primary key" );
            nextPrimKeyField.WriteToTrace( "Right primary key" );
            SAPDBERR_ASSERT_STATE( l_less != primCompResult );
        }
    }
}

/*---------------------------------------------------------------------------*/

void
cbd491_InvRecord::TraceFields(
    const SAPDBFields_FieldCollection&  fields,
    const SAPDB_Char*  const            title ) const
{
    Kernel_VTrace   trace;

    if( 0 != title ){
        trace << title << FlushLine;
    }
    SAPDBFields_FieldCollection::IndexType  fieldNo;
    SAPDBFields_Field   field;

    for( fieldNo = 0; fieldNo < fields.GetFieldCount(); fieldNo++ )
    {
        if( ! fields.GetField( fieldNo, field )){
            return;
        }
        if( this->PrimKeyFieldNo() == fieldNo ){
            trace << "PrimKey (" ;
        }
        else{
            trace << "SecKeyCol#" << fieldNo << " (";
        }
        trace << field.GetLength() << "): " << FlushLine;
        field.WriteToTrace();
    }
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

cbd491_InvRecordComperator::cbd491_InvRecordComperator(
    SAPDBMem_IRawAllocator&     allocator,
    cbd491_InvRecordCollection& invRecordCollection,
    tgg00_BasisError&           error,
    cbd497_StackDescInfo&       stackDescInfo )
        :
        cbd491_InvRecord( allocator, error, stackDescInfo ),
        m_FieldsPerRecord( invRecordCollection.NumberOfFieldsPerRecord()),
        m_InvRecordCollection( invRecordCollection ),
        m_LeftFields( allocator ),
        m_RightFields( allocator )
{}

/*===========================================================================*
 *  DEFINITION OF METHODS DECLARED IN gbd491.h (CODE)                        * 
 *===========================================================================*/

cbd491_CreateIndexParticipant::cbd491_CreateIndexParticipant(
    Trans_Context&      trans,
    tbd_current_tree&   primCurrent,
    tgg00_StackDesc&    stackDesc,
    tgg00_IndexCounter& indexCounter,
    const bool          bUseMemorySort )
        :
        m_Allocator( trans.Allocator()),
        m_PrimCurrent( primCurrent ),
        m_IndexCounter( indexCounter ),
        m_Stack( trans.OldTrans().trError_gg00, stackDesc ),
        m_pUndefNullKey( 0 )
{
    SAPDBTRACE_ROUTINE_DEBUG( "cbd491_CreateIndexParticipant::Ctor", DataIndex_Trace, 5 );

    bd490InitIndexCounter( m_IndexCounter ); // Reset Counter

    if(( e_ok != trans.OldTrans().trError_gg00 ) || ( ! bUseMemorySort )){
        return;
    }

    SAPDB_RangeFill( __FILE__, 8,
                     sizeof (m_UndefNullKey), m_UndefNullKey,
                     POS_OFF_DIFF_BD00, sizeof (m_UndefNullKey), '\00',
                     trans.OldTrans().trError_gg00);

    m_UndefNullKey [0] = csp_undef_byte;
    m_pUndefNullKey    = &m_UndefNullKey[0];
}

/*---------------------------------------------------------------------------*/

void
cbd491_CreateIndexParticipant::InsertionSort(
    Trans_Context&          trans,
    cbd300_InvCurrent&      invCurrent,
    tgg00_IndexMode         indexMode,
    SAPDB_Byte* const       pStartPrimKey,
    const SAPDB_Byte* const pStopPrimKey,
    const SAPDB_UInt        startPrimKeyLen,
    const SAPDB_UInt        stopPrimKeyLen,
    pasbool&                bEntriesExist )
{
    SAPDBTRACE_ROUTINE_DEBUG( "cbd491_CreateIndexParticipant::InsSort", DataIndex_Trace, 5 );

    // Precondition: Primary root page is in access with share lock

    if( e_ok != trans.OldTrans().trError_gg00 ){
        return;
    }

    tbd_searchresult        primKeySearchResult;
    tsp00_LcompResult       compResult;
    SAPDB_Int4              primRecIndex;
    tsp00_PageNo            oldLeafPno = NIL_PAGE_NO_GG00;
    tgg00_RecPtr            pRec = 0;
    tbd_node_ptrs           pPrimLeaf;
    tbd_neighbors           neighbors;

    pPrimLeaf.np_ptr()    = 0;
    pPrimLeaf.np_cbptr()  = 0;

    do
    {
        if( 0 == pPrimLeaf.np_ptr()) // first search with start key
        {
            bd30SearchRecord( m_PrimCurrent, pStartPrimKey, startPrimKeyLen,
                              ! LOCK_LEAF_EXCL_BD00, pPrimLeaf, primRecIndex,
                              neighbors, primKeySearchResult );

            if( nonefound == primKeySearchResult ){
                bd490Inc( m_IndexCounter.idc_prim_leafnodes, 1 );
                break; //no records found or fatal error
            }
        }
        else
        {
            oldLeafPno = pPrimLeaf.np_ptr()->nd_id();
            b31next_search( pPrimLeaf, primRecIndex, m_PrimCurrent, lru_mid );
        }
        if( e_ok != trans.OldTrans().trError_gg00){
            break;
        }

        pRec = bd50GetRecPtr( m_PrimCurrent, pPrimLeaf, primRecIndex );

        if( 0 == pRec )
            break; //  m_TrError = e_illegal_entrypos

        const CnsRead_CachedRow* pRow = reinterpret_cast<CnsRead_CachedRow*>( pRec );

        s30cmp( pRec->recBody_gg00(), POS_OFF_DIFF_BD00, pRow->GetKeyLen(),
                pStopPrimKey, POS_OFF_DIFF_BD00, stopPrimKeyLen, compResult );

        if( l_greater == compResult ){
            break; // no next records, i.e. stop key reached
        }

        if( oldLeafPno != pPrimLeaf.np_ptr()->nd_id())
        {
            bd490Inc( m_IndexCounter.idc_prim_leafnodes, 1 );

            if( trans.Task().CancelPending())
            {
                trans.OldTrans().trError_gg00 = e_cancelled;
                RTE_Message( Data_Exception( __CONTEXT__, DATA_CREATE_INDEX_CANCELED_1 ));
                break;
            }
        }
        if( ! pRow->IsDeleted()){
            Data_Row unpackedRow;
            if ( Data_Row::ToRow( *pRec ).IsPacked() ){ // PTS 1139184
                Data_Row::ToRow( *pRec ).Unpack(unpackedRow);
                pRec = reinterpret_cast< tgg00_RecPtr >( &unpackedRow );
            }
            this->AddOneInvRecord( trans, invCurrent, indexMode, pRec );
        }
    }
    while( e_ok == trans.OldTrans().trError_gg00 );

    if( 0 != pPrimLeaf.np_ptr()){
        b13r_release_node( pPrimLeaf, m_PrimCurrent, lru_last );
    }
    bd30ReleaseSubTree( m_PrimCurrent );

    if( e_no_next_record == trans.OldTrans().trError_gg00 ){
        trans.OldTrans().trError_gg00 = e_ok;
    }
    if( e_ok == trans.OldTrans().trError_gg00 )
    {
        bEntriesExist = (0 < m_IndexCounter.idc_prim_keycount);
        bd490Inc( m_IndexCounter.idc_prim_keycount, m_IndexCounter.idc_nullcount );
    }
}

/*---------------------------------------------------------------------------*/

void
cbd491_CreateIndexParticipant::MemorySort(
    Trans_Context&              trans,
    cbd495_DoubleWriteQueue&    resultInvQueue,
    SAPDB_Byte* const           pStartPrimKey,
    const SAPDB_Byte* const     pStopPrimKey,
    const SAPDB_UInt            startPrimKeyLen,
    const SAPDB_UInt            stopPrimKeyLen,
    const SAPDB_UInt            numLeavesToScan,
    const SAPDB_UInt            avgRecordsPerLeaf,
    pasbool&                    bEntriesExist )
{
    SAPDBTRACE_ROUTINE_DEBUG( "cbd491_CreateIndexParticipant::MemSort", DataIndex_Trace, 5 );

    if( e_ok != trans.OldTrans().trError_gg00 ){
        return;
    }
    const SAPDB_UInt estimatedRecordCount = avgRecordsPerLeaf * numLeavesToScan;
    const SAPDB_UInt numSecKeyColumns     = m_Stack.bd497NumColumns();

    cbd491_InvRecordCollection  invRecCollection( m_Allocator, numSecKeyColumns );
    cbd492_StorageArea          storageArea( m_Allocator );

    if(
        ( ! invRecCollection.Initialize( estimatedRecordCount )) ||
        ( ! storageArea.Initialize( numLeavesToScan )))
    {
        trans.OldTrans().trError_gg00 = e_no_more_memory;
        return;
    }
    this->ProcessPrimTreeRange( trans, invRecCollection, storageArea, pStartPrimKey,
                                pStopPrimKey, startPrimKeyLen, stopPrimKeyLen );

    bEntriesExist = ( 0 < invRecCollection.GetUsedCount());

    if(( e_ok != trans.OldTrans().trError_gg00 ) || ( ! bEntriesExist )){
        return;
    }
    cbd491_InvRecordComperator comperator( m_Allocator, invRecCollection, trans.OldTrans().trError_gg00, m_Stack );

    if( ! comperator.Initialize()){
        trans.OldTrans().trError_gg00 = e_no_more_memory;
        return;
    }
    if( ! invRecCollection.Sort( comperator )){
        trans.OldTrans().trError_gg00 = e_no_more_memory;
        return;
    }
    this->AppendInvRecords( trans, resultInvQueue, invRecCollection );
}

/*---------------------------------------------------------------------------*/

void
cbd491_CreateIndexParticipant::AddOneInvRecord(
    Trans_Context&          trans,
    cbd300_InvCurrent&      invCurrent,
    tgg00_IndexMode         indexMode,
    tgg00_RecPtr            pRec )
{
    SAPDBTRACE_ROUTINE_DEBUG( "cbd491_CreateIndexParticipant::AddOneInvRec", DataIndex_Trace, 5 );

    const SAPDB_Int4    primKeyLen = CnsRead_CachedRow::GetKeyLen( pRec );
    SAPDB_Byte* const   pPrimKey   = reinterpret_cast< SAPDB_Byte* >( &pRec->recBody_gg00());

    tgg00_Lkey          secKey;         // substitute Lkey by pKey and KeyLen
    SAPDB_Byte* const   pSecKey  = reinterpret_cast< SAPDB_Byte* >( &secKey.keyVal_gg00());

    bool                bAllColumnsUndef = false;
    bool                bSecKeyFound;

    Log_InvDescId  invDescId;
    Log_InvDescMap invDescMap;

    // build secondary key in given memory
    invDescMap.Assign( m_Stack.bd497GetStackListPtr(), m_Stack.bd497NumColumns(), false );
    invDescMap.GetFirstKey( trans.OldTrans(), pRec, invDescId, secKey, bAllColumnsUndef );

    if( e_ok != trans.OldTrans().trError_gg00 ){
        return;
    }
    const SAPDB_Int4 secKeyLen = secKey.keyLen_gg00();

    if ( ! bAllColumnsUndef // PTS 1121337 UH 2003-03-27 do not check if is null
            &&
            m_Stack.bd497IsInvUnique() )
    {
        bd400TestUniqueInv( invCurrent, pSecKey, secKeyLen, pPrimKey, primKeyLen );
        if( e_ok != trans.OldTrans().trError_gg00 ){
            return;
        }
    }
    bd400AddToInvTree (invCurrent, pSecKey, secKeyLen, pPrimKey, primKeyLen, bSecKeyFound);

    switch (indexMode)
    {
    case create_index:
        {
            if ((tfnTempMulti_egg00 != invCurrent.curr_tree_id.fileTfn_gg00()) && (!bSecKeyFound))
                bd490Inc (m_IndexCounter.idc_sec_keycount, 1);
            bd490Inc (m_IndexCounter.idc_prim_keycount, 1);
            return;
        }
    case test_index:
        {
            if (e_duplicate_key == trans.OldTrans().trError_gg00)
            {
                trans.OldTrans().trError_gg00 = e_ok; // Normal case, because the primary key must exists in the inv tree
            }
            else if (e_ok == trans.OldTrans().trError_gg00)
            {
                // Error, because the primary key doesn't exist in the inv tree

                bd490Inc (m_IndexCounter.idc_prim_keycount, 1); // count missing primary keys

                if (g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdIndex_gg00)
                {
                    bd03KeyVtrace (trans.OldTrans(), pSecKey, secKeyLen, pPrimKey, primKeyLen, b03test);
                }
            }
            return;
        }
    default:
        trans.OldTrans().trError_gg00 = e_not_implemented; // wrong index mode!
    }
    SAPDBERR_ASSERT_STATE( indexMode != create_index && indexMode != test_index );
}

/*---------------------------------------------------------------------------*/

void
cbd491_CreateIndexParticipant::AppendInvRecords(
    Trans_Context&              trans,
    cbd495_DoubleWriteQueue&    resultInvQueue,
    cbd491_InvRecordCollection& invRecCollection )
{
    SAPDBTRACE_ROUTINE_DEBUG( "cbd491_CreateIndexParticipant::AppendInvRecords", DataIndex_Trace, 5 );

    cbd491_InvRecord    invRecord( m_Allocator, trans.OldTrans().trError_gg00, m_Stack, m_pUndefNullKey );

    const SAPDB_UInt    maxRecords = invRecCollection.GetUsedCount();
    const SAPDB_UInt    maxColumns = invRecord.MaxNumberOfColumns();

    SAPDBERR_ASSERT_STATE( 0 < maxRecords );

#ifdef SAPDB_SLOW
    const bool bDoCheck = true;
#else
    const bool bDoCheck = DataIndex_Check.ChecksLevel( 6 );
#endif

    tsp00_LcompResult   compResult( tsp00_LcompResult::fromConst( l_undef ));

    SAPDB_Byte* pPrimKeySpace = 0;
    SAPDB_Byte* pSecKeySpace  = 0;

    SAPDB_UInt  minRequiredPrimKeySpace = 0;
    SAPDB_UInt  currPrimKeyCount        = 0;
    SAPDB_UInt  currRecordCount         = 0;

    SAPDBFields_FieldCollection currFields( m_Allocator );
    SAPDBFields_FieldCollection nextFields( m_Allocator );

    if(( ! currFields.Initialize( maxColumns )) || ( ! nextFields.Initialize( maxColumns )))
    {
        trans.OldTrans().trError_gg00 = e_no_more_memory;
        return;
    }
    do
    {
        // Insert primary key into result inv queue
        invRecCollection.GetSorted( currRecordCount, currFields );
        const SAPDB_UInt requestedSpace = invRecord.GetPrimKeyLen( currFields );

        resultInvQueue.bd495ReservePrimKeySpace( pPrimKeySpace, requestedSpace );
        invRecord.MaterializePrimKey( currFields, pPrimKeySpace, requestedSpace );

        minRequiredPrimKeySpace += requestedSpace;
        ++currPrimKeyCount;
        ++currRecordCount;

        if( currRecordCount < maxRecords )
        {
            // Check if the next inv record belongs to the same inversion
            // list, i.e. is the secondary key equal?
            invRecCollection.GetSorted( currRecordCount, nextFields );

            compResult = invRecord.CompareVirtualExclusivePrimKey( currFields, nextFields );

            if( bDoCheck ){
                invRecord.CheckKeyOrder( compResult, invRecCollection, currFields, nextFields );
            }
        }
        if(( l_less == compResult ) || ( currRecordCount >= maxRecords ))
        {
            // Materialize current secondary key, because next inv record
            // belongs to the next inversion list or no next inv record exist.

            // Insert number of a) primary keys, b) min amount of storage
            // used by primary keys and c) secondary key into inv queue

            // Insert number of primary keys which belongs to the inversion
            // list of the current secondray key
            resultInvQueue.bd495PushNumPrimKeys( currPrimKeyCount );
            currPrimKeyCount = 0;

            // Insert the size of all primary keys which belongs to the
            // inversion list of the current secondray key
            resultInvQueue.bd495PushMinRequiredPrimKeySpace( minRequiredPrimKeySpace );
            minRequiredPrimKeySpace = 0;

            // Insert secondary key into result inv queue
            const SAPDB_UInt requestedSpace = invRecord.GetSecKeyLen( currFields );
            resultInvQueue.bd495ReserveSecKeySpace( pSecKeySpace, requestedSpace );
            invRecord.MaterializeSecKey( currFields, pSecKeySpace, requestedSpace );
        }
    }
    while(( e_ok == trans.OldTrans().trError_gg00 ) && ( currRecordCount < maxRecords ));
}

/*---------------------------------------------------------------------------*/

void
cbd491_CreateIndexParticipant::ProcessPrimTreeRange(
    Trans_Context&              trans,
    cbd491_InvRecordCollection& invRecCollection,
    cbd492_StorageArea&         storageArea,
    SAPDB_Byte* const           pStartPrimKey,
    const SAPDB_Byte* const     pStopPrimKey,
    const SAPDB_UInt            startPrimKeyLen,
    const SAPDB_UInt            stopPrimKeyLen )
{
    SAPDBTRACE_ROUTINE_DEBUG( "cbd491_CreateIndexParticipant::Process", DataIndex_Trace, 5 );

    // Precondition: Primary root page is in access with share lock

    bool                    bCompareEachKey = true;
    tbd_searchresult        primKeySearchResult;
    tsp00_LcompResult       compResult;
    SAPDB_Int4              primRecIndex;
    tsp00_PageNo            oldLeafPno = NIL_PAGE_NO_GG00;
    tgg00_RecPtr            pPrimRec = 0;
    tbd_node_ptrs           pPrimLeaf;
    tbd_neighbors           neighbors;

    pPrimLeaf.np_ptr()    = 0;
    pPrimLeaf.np_cbptr()  = 0;

    const SAPDB_UInt numSecKeyColumns = m_Stack.bd497NumColumns();

    cbd491_InvRecord            invRecord( m_Allocator, trans.OldTrans().trError_gg00, m_Stack, m_pUndefNullKey );
    SAPDBFields_Field           primRecord;
    SAPDBFields_FieldCollection fields( m_Allocator );

    if(( ! fields.Initialize( invRecord.MaxNumberOfColumns())) || ( ! invRecord.Initialize())){
        trans.OldTrans().trError_gg00 = e_no_more_memory;
        return;
    }
    do
    {
        if( 0 == pPrimLeaf.np_ptr()) // first search with start key
        {
            bd30SearchRecord( m_PrimCurrent, pStartPrimKey, startPrimKeyLen,
                              ! LOCK_LEAF_EXCL_BD00, pPrimLeaf, primRecIndex,
                              neighbors, primKeySearchResult );

            if( nonefound == primKeySearchResult ){
                bd490Inc( m_IndexCounter.idc_prim_leafnodes, 1 );
                break; //no records found or fatal error
            }
        }
        else
        {
            oldLeafPno = pPrimLeaf.np_ptr()->nd_id();
            b31next_search( pPrimLeaf, primRecIndex, m_PrimCurrent, lru_mid );
        }
        if( e_ok != trans.OldTrans().trError_gg00 ){
            break;
        }
        pPrimRec = bd50GetRecPtr( m_PrimCurrent, pPrimLeaf, primRecIndex );

        if( 0 == pPrimRec ){
            break; // trans.OldTrans().trError_gg00 = e_illegal_entrypos
        }
        const bool bNextLeafReached = ( oldLeafPno != pPrimLeaf.np_ptr()->nd_id());
        if( bNextLeafReached )
        {
            const  SAPDB_Int4  maxPrimRecIndex = pPrimLeaf.np_ptr()->nd_record_cnt()-1;
            const tgg00_RecPtr pMaxPrimRec     = bd50GetRecPtr( m_PrimCurrent, pPrimLeaf, maxPrimRecIndex );

            s30cmp( pMaxPrimRec->recBody_gg00(), POS_OFF_DIFF_BD00,
                    CnsRead_CachedRow::GetKeyLen( pMaxPrimRec ),
                    pStopPrimKey, POS_OFF_DIFF_BD00, stopPrimKeyLen, compResult );
            // compare each key on current leaf , if the greatest key on the leaf
            // is not smaller than the stop key
            bCompareEachKey = (l_less != compResult );
        }
        if( bCompareEachKey )
        {
            s30cmp( pPrimRec->recBody_gg00(), POS_OFF_DIFF_BD00, CnsRead_CachedRow::GetKeyLen( pPrimRec ),
                    pStopPrimKey, POS_OFF_DIFF_BD00, stopPrimKeyLen, compResult );
            if( l_greater == compResult ){
                break; // no next records, i.e. stop key reached
            }
        }
        if( bNextLeafReached )
        {
            bd490Inc( m_IndexCounter.idc_prim_leafnodes, 1 );
            if( trans.Task().CancelPending())
            {
                trans.OldTrans().trError_gg00 = e_cancelled;
                RTE_Message( Data_Exception( __CONTEXT__, DATA_CREATE_INDEX_CANCELED_1 ));
                break;
            }
        }
        Data_Row unpackedRow;
        if ( Data_Row::ToRow( *pPrimRec ).IsPacked() ){ // PTS 1139184
            Data_Row::ToRow( *pPrimRec ).Unpack(unpackedRow);
            pPrimRec = reinterpret_cast< tgg00_RecPtr >( &unpackedRow );
        }
        invRecord.BuildVirtual( pPrimRec, storageArea, fields );

        if( 0 == fields.GetFieldCount()){
            continue; // current record is marked for delete and therefore not index relevant
        }
        if( e_ok != trans.OldTrans().trError_gg00 ){
            break;
        }
        if( ! invRecCollection.Append( fields )){
            trans.OldTrans().trError_gg00 = e_no_more_memory;
            break;
        }
        bd490Inc( m_IndexCounter.idc_prim_keycount, 1 );
    }
    while( e_ok == trans.OldTrans().trError_gg00);

    if( 0 != pPrimLeaf.np_ptr()){
        b13r_release_node( pPrimLeaf, m_PrimCurrent, lru_last );
    }
    bd30ReleaseSubTree( m_PrimCurrent );

    if( e_no_next_record == trans.OldTrans().trError_gg00 ){
        trans.OldTrans().trError_gg00 = e_ok;
    }
    if( e_ok == trans.OldTrans().trError_gg00 ){
        bd490Inc( m_IndexCounter.idc_prim_keycount, m_IndexCounter.idc_nullcount );
    }
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
