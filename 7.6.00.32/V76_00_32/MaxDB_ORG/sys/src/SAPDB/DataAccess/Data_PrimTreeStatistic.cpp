/****************************************************************************

  module      : Data_PrimTreeStatistic.cpp

  -------------------------------------------------------------------------

  responsible : TorstenS

  special area: File statistic
  description : Implementation of table statistics and update statistic column

  last changed: 2002-04-24
  see also    :

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2001-2005 SAP AG


    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "hbd01_1.h"
#include "hbd02.h"
#include "hbd05.h"
#include "hbd30.h" // bd30AddToTempTree
#include "hbd998.h" // bd998GetPageAndRecordCount
#include "hgg01_3.h"
#include "hgg04.h"
#include "ggg90.h"
#include "heo50.h"
#include "hsp30.h"    // PASCAL: Buffer handling and comparison routines
#include <math.h>   // sqrt
#include <stdlib.h> // rand


#include "ConsistentRead/CnsRead_CachedRow.hpp"
#include "Converter/Converter_PageCollector.hpp"
#include "Container/Container_Vector.hpp"
#include "DataAccess/Data_PrimTreeStatistic.hpp"
#include "DataAccess/Data_PageAccessManager.hpp"
#include "DataAccess/Data_ScanTreeAccess.hpp"
#include "DataAccess/Data_IndexScanTreeAccess.hpp"
#include "DataAccess/Data_ITreeAccess.hpp"
#include "DataAccess/Data_Messages.hpp"
#include "DataAccess/Data_Exceptions.hpp"
#include "DataAccess/Data_Row.hpp"
#include "KernelCommon/Kernel_FileIO.hpp"
#include "KernelCommon/Kernel_VTrace.hpp"
#include "RunTime/RTE_Message.hpp"
#include "Transaction/Trans_Context.hpp"
#include "SAPDBCommon/SAPDB_ToString.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDBCommon/SAPDB_RangeCode.hpp"
#include "SQLManager/SQLMan_UpdateStatistics.hpp"
#include "SQLManager/ErrorHandling/SQLManErr_Interface.hpp"
#include "SAPDBCommon/Algorithms/SAPDBAlgo_QuickSort.hpp"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/

inline SAPDB_Bool
IsColumnUnique( const tgg00_StEntryAddr   pColumnStackEntry )
{
    return
        (( st_varkey == ( pColumnStackEntry )->etype())     &&
         (1          == ( pColumnStackEntry )->epos()))
        ||
        (( op_unique      == ( pColumnStackEntry )->eop())  ||
         ( op_unique_desc == ( pColumnStackEntry )->eop()));
}

/*---------------------------------------------------------------------------*/

inline SAPDBMem_IRawAllocator&
GetAllocator( tgg00_TransContext &trans )
{
    return( *( reinterpret_cast< SAPDBMem_IRawAllocator* >( trans.trAllocator_gg00 )));
}

/*---------------------------------------------------------------------------*/

inline SAPDB_Real8
myAbsolute( SAPDB_Real8 myValue )
{
    if( myValue < 0 )
        return( -1.0 * myValue );
    else
        return( myValue );
}

/*---------------------------------------------------------------------------*/

inline const char*
BasisErrorToString( const tgg00_BasisError error )
{
    return( SQLManErr_Interface::GetInstance().GetErrorText(
                SQLManErr_Interface::GetInstance().GetReturnCode( error, sqlm_internal )));
}

/*---------------------------------------------------------------------------*/

inline void
CalculateClusterStatistic(
    RTETask_ITask&              task,
    const Data_PrimPage&        page,
    Converter_PageCollector&    pageCollection,
    Data_PageCount&             numberOfLeaves,
    Data_PageCount&             numberOfClusters )
{
    SAPDBERR_ASSERT_STATE( FIRST_INDEX_LEVEL_BD00 == page.GetLevel());

    const Data_RecordCount recCount = page.GetRecordCount();
    numberOfLeaves += recCount;

    for( Data_RecordCount recIndex = 0; recIndex < recCount; ++recIndex )
    {
        const tgg00_Rec*    pRec       = page.GetRecord( recIndex );
        const Data_PageNo   leafPageNo = pRec->recPno_gg00().gg90GetInt4();
        const bool          bOkay      = pageCollection.AppendPageNo( leafPageNo );
        if(( bOkay )&&( pageCollection.GetPageCount() < 100000 )){
            continue;
        }
        // determine cluster and reset memory to avoid too big memory consumption
        // for the complete cluster statistic. It will be a small fault if an
        // existing cluster is handled as two
        pageCollection.DetermineCluster( task );
        numberOfClusters += pageCollection.GetClusterCount();
        pageCollection.Initialize(); // reset memory
    }
}

/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

namespace {

class ColumnList
{
public:

    ColumnList( tgg00_TransContext  &trans )
    :
    m_TrError( trans.trError_gg00 ),
    m_Columns( GetAllocator( trans ))
    {}

    ~ColumnList(){
        m_Columns.Delete();
    }

    SAPDB_Bool Initialize( const SAPDB_UInt4 recordCount );

    void Clear(){
        m_Columns.Clear();
    }

    SAPDB_Bool  Add(
        tgg00_Rec*          pRec,
        const SAPDB_Int4    position,
        const SAPDB_Int4    length );

    SAPDB_Bool  AllUnique() const;

private:

    struct ColumnPosition
    {
        tgg00_Rec*  cpl_pRec;
        SAPDB_Int4  cpl_Position;
        SAPDB_Int4  cpl_Length;

        ColumnPosition(){}
    };

    typedef Container_Vector<ColumnPosition>    Columns;

private:

    tgg00_BasisError    &m_TrError;

    Columns m_Columns;
};

/*---------------------------------------------------------------------------*/

class TempStatisticRecord
{
public:

    TempStatisticRecord( tgg00_TransContext  &trans )
    :
    m_Allocator( GetAllocator( trans )),
    m_TrError( trans.trError_gg00 ),
    m_pRec( NULL ),
    m_Capacity( 0 )
    {}

    TempStatisticRecord(
        SAPDBMem_IRawAllocator  &allocator,
        tgg00_BasisError        &trError )
    :
    m_Allocator( allocator ),
    m_TrError( trError ),
    m_pRec( NULL ),
    m_Capacity( 0 )
    {}

    ~TempStatisticRecord(){ Delete(); }

    SAPDB_Bool Initialize( const SAPDB_UInt4 capacity );

    void AssignSpecialKey( SAPDB_Int2 columnId );

    void AssignKey(
        const SAPDB_Int2    columnId,
        SAPDB_Int2          keyLength,
        const SAPDB_Byte*   pKey );


    void AssignRecord( tgg00_Rec* pRecord )
    {
        SAPDBERR_ASSERT_STATE( 0 == m_Capacity );
        m_pRec = pRecord;
    }

    tgg00_RecPtr GetRecordPtr() const
    {
        SAPDBERR_ASSERT_STATE( NULL != m_pRec );
        return( m_pRec );
    }

    SAPDB_UInt4 GetDuplicateCount() const
    {
        SAPDBERR_ASSERT_STATE( NULL != m_pRec );
        return( m_pRec->recPno_gg00().gg90GetInt4() );
    }

    SAPDB_Int2 GetColumnId() const;

private:

    void InitRecordHeader();

    void Delete();

private:

    SAPDBMem_IRawAllocator& m_Allocator;
    tgg00_BasisError&       m_TrError;
    tgg00_Rec*              m_pRec;
    SAPDB_UInt4             m_Capacity;
};

/*---------------------------------------------------------------------------*/

struct DuplicateFrequencyInfo
{
    SAPDB_Int duplicateCount;
    SAPDB_Int frequency;

    bool operator<(const DuplicateFrequencyInfo& si)
    {
        return this->duplicateCount < si.duplicateCount;
    }
};

typedef Container_Vector< DuplicateFrequencyInfo > FrequencyList;

/*---------------------------------------------------------------------------*/

class EstimateAlgorithm
{
public:
    virtual SAPDB_Int8 EstimateDistinctValueCount(
        const FrequencyList &dl,
        const SAPDB_Int8     ssize,
        const SAPDB_Int8     psize,
        const SAPDB_Int8     D) = 0;

    virtual ~EstimateAlgorithm()
    {}
};

/*---------------------------------------------------------------------------*/

class EstimateAlgoOriginal : public EstimateAlgorithm
{
public:

    virtual SAPDB_Int8 EstimateDistinctValueCount(
        const FrequencyList& dl,
        const SAPDB_Int8     ssize,
        const SAPDB_Int8     psize,
        const SAPDB_Int8     D)
    {
        return Data_PrimTreeStatistic::EstimateDistinctValuesPerColumn(
                   static_cast<SAPDB_Real8>( psize ),
                   static_cast<SAPDB_Real8>( ssize ),
                   static_cast<SAPDB_Real8>( D ));
    }
};

/*---------------------------------------------------------------------------*/

class EstimateAlgoShlosser : public EstimateAlgorithm
{
public:

    virtual SAPDB_Int8 EstimateDistinctValueCount(
        const FrequencyList& dl,
        const SAPDB_Int8     ssize,
        const SAPDB_Int8     psize,
        const SAPDB_Int8     D)
    {
        SAPDB_Int8  result;
        SAPDB_Real8 q    = ssize / (SAPDB_Real8) psize;
        SAPDB_Real8 pj   = 1.0 - q;
        SAPDB_Real8 p    = 1.0 - q;
        SAPDB_Int   j    = 1;
        SAPDB_Real8 sum1 = 0.0;
        SAPDB_Real8 sum2 = 0.0;
        SAPDB_Int8  m1   = 0;

        if ( dl[0].duplicateCount == 1 )
            m1 = dl[0].frequency;

        for ( SAPDB_UInt i = 0; i < dl.GetSize(); i++ ) {
            pj = nextpower( pj, j, dl[i].duplicateCount );
            sum1 += pj * dl[i].frequency;

            sum2 += dl[i].duplicateCount * q * pj/p *dl[i].frequency;
        }

        if ( sum2 != 0 )
            result = (SAPDB_Int8) (D + m1 * sum1 / sum2);
        else
            result = D;

        return result;
    }

private:
    double nextpower (SAPDB_Real8 pj, SAPDB_Int8 j, SAPDB_Int8 base) const
    {
        SAPDB_Real8 result = pj;

        for ( SAPDB_Int8 i = j; i < base; i++ )
            result *= pj;

        return result;
    }
};

/*---------------------------------------------------------------------------*/

class EstimateAlgoSpreadTTK : public EstimateAlgorithm
{
public:

    virtual SAPDB_Int8 EstimateDistinctValueCount(
        const FrequencyList& dl,
        const SAPDB_Int8     ssize,
        const SAPDB_Int8     psize,
        const SAPDB_Int8     D)
    {
        const SAPDB_Int8 limit  = ssize * 5 / 100;       // 5% threshold
        SAPDB_Int8       sum    = 0;
        int              length = dl.GetSize();
        SAPDB_Int8       cpsize = psize;
        int              stop;
        double           dist   = 0;
        double           bucketsize;

        int i;
        for ( i = length - 1; i > 0; i-- )
        {
            long amount = dl[i].frequency * dl[i].duplicateCount;
            if ( amount > limit )
                sum += amount;
            else
                break;
        }
        stop = i+1;

        if ( sum > 0 )
            cpsize = (long) (psize * (double)(ssize-sum)/(double)ssize);

        bucketsize = cpsize / (double) stop;

        for ( i = 0; i < stop; i++ )
        {
            dist += bucketsize / dl[i].duplicateCount + dl[i].frequency;
        }

        return (SAPDB_Int8) dist;
    }
};

/*---------------------------------------------------------------------------*/

class EstimateAlgoHybridSO1 : public EstimateAlgorithm
{
public:

    SAPDB_Int8 EstimateDistinctValueCount(
        const FrequencyList& dl,
        const SAPDB_Int8     ssize,
        const SAPDB_Int8     psize,
        const SAPDB_Int8     D)
    {
        // mix result from original algorithm with result from
        // Shlosser algorithm
        if ( D < ssize/1000 )
        {
            return m_original.EstimateDistinctValueCount(
                       dl, ssize, psize, D );
        }

        double weight1;
        double weight2;

        double ind = (double) ssize / (double) psize;

        if ( ind < 0.02 )
        {
            weight1 = 0.57666;
            weight2 = 0.42334;
        }
        else if ( ind < 0.1 )
        {
            weight1 = 0.32167;
            weight2 = 0.67833;
        }
        else
        {
            weight1 = 0.11572;
            weight2 = 0.88428;
        }

        double hybrid =
            weight1 * m_original.EstimateDistinctValueCount(
                dl, ssize, psize, D )
            + weight2 * m_shlosser.EstimateDistinctValueCount(
                dl, ssize, psize, D );

        return (SAPDB_Int8) (hybrid + .5);
    }
private:
    EstimateAlgoOriginal m_original;
    EstimateAlgoShlosser m_shlosser;
};

/*---------------------------------------------------------------------------*/

class EstimateAlgoHybridTTK1 : public EstimateAlgorithm
{
public:

    SAPDB_Int8 EstimateDistinctValueCount(
        const FrequencyList& dl,
        const SAPDB_Int8     ssize,
        const SAPDB_Int8     psize,
        const SAPDB_Int8     D)
    {
        // mix result from original with result from
        // spreadTTK algorithm
        if ( D < ssize/1000 )
        {
            return m_original.EstimateDistinctValueCount(
                       dl, ssize, psize, D );
        }

        double weight1;
        double weight2;

        double ind = (double) ssize / (double) psize;

        if ( ind < 0.02 )
        {
            weight1 = 0.57666;
            weight2 = 0.42334;
        }
        else if ( ind < 0.1 )
        {
            weight1 = 0.32167;
            weight2 = 0.67833;
        }
        else
        {
            weight1 = 0.11572;
            weight2 = 0.88428;
        }

        double hybrid =
            weight1 * m_original.EstimateDistinctValueCount(
                dl, ssize, psize, D )
            + weight2 * m_spreadTTK.EstimateDistinctValueCount(
                dl, ssize, psize, D );

        return (SAPDB_Int8) (hybrid + .5);
    }
private:
    EstimateAlgoOriginal  m_original;
    EstimateAlgoSpreadTTK m_spreadTTK;
};

/*---------------------------------------------------------------------------*/

class EstimateAlgoHybridMK1 : public EstimateAlgorithm
{
public:

    SAPDB_Int8 EstimateDistinctValueCount(
        const FrequencyList& dl,
        const SAPDB_Int8     ssize,
        const SAPDB_Int8     psize,
        const SAPDB_Int8     D)
    {
        // mix result from original with result from
        // spreadTTK algorithm
        if ( D < ssize/1000 )
        {
            return m_original.EstimateDistinctValueCount(
                       dl, ssize, psize, D );
        }

        double weight1;
        double weight2;

        double ind = (double) ssize / (double) psize;

        if ( ind < 0.02 )
        {
            weight1 = 0.57666;
            weight2 = 0.42334;
        }
        else if ( ind < 0.1 )
        {
            weight1 = 0.32167;
            weight2 = 0.67833;
        }
        else
        {
            weight1 = 0.11572;
            weight2 = 0.88428;
        }

        const SAPDB_Int8 distEstimateShlo =
            m_shlosser.EstimateDistinctValueCount( dl, ssize, psize, D );
        const SAPDB_Int8 distEstimateTTK  =
            m_spreadTTK.EstimateDistinctValueCount( dl, ssize, psize, D );

        const SAPDB_Int8 maxDistEstimate =
            distEstimateShlo > distEstimateTTK
            ? distEstimateShlo : distEstimateTTK;

        double hybrid =
            weight1 * m_original.EstimateDistinctValueCount(
                dl, ssize, psize, D )
            + weight2 * maxDistEstimate;

        return (SAPDB_Int8) (hybrid + .5);
    }
private:
    EstimateAlgoOriginal  m_original;
    EstimateAlgoShlosser  m_shlosser;
    EstimateAlgoSpreadTTK m_spreadTTK;
};

/*---------------------------------------------------------------------------*/

class EstimateDistinctValues
{
public:
    EstimateDistinctValues(
        SAPDBMem_IRawAllocator& allocator,
        const SAPDB_UInt8       sampleSize,
        const SAPDB_UInt8       tableRecordCount)
    : m_alloc( allocator ),
    m_frequencyList( allocator ),
    m_sampleSize( sampleSize ),
    m_tableRecordCount( tableRecordCount ),
    m_numberOfDistinctValuesFound( -1 )
    {
        SAPDBERR_ASSERT_ARGUMENT( m_sampleSize < m_tableRecordCount );

        // take 10 percent of maximum array size determined by Gauß'
        // summation formula
        SAPDB_UInt minArraySize = static_cast<SAPDB_UInt>(
                                      (-.5 + sqrt( 1.0 + 8.0 * m_sampleSize ) * .5) * 0.1 );

        minArraySize = minArraySize < 10 ? 10 : minArraySize;

        m_frequencyList.Reserve( minArraySize );

        switch ( SQLMan_UpdateStatistics::GetSampleAlgoNo() )
        {
        case 1:
            m_pEstimateAlgo = new ( allocator )
                              EstimateAlgoHybridSO1();
            break;
        case 2:
            m_pEstimateAlgo = new ( allocator )
                              EstimateAlgoHybridTTK1();
            break;
        case 4:
            m_pEstimateAlgo = new ( allocator )
                              EstimateAlgoHybridMK1();
            break;
        case 3:
        default:
            m_pEstimateAlgo = new ( allocator )
                              EstimateAlgoOriginal();
            break;
        }
    }

    ~EstimateDistinctValues()
    {
        destroy ( m_pEstimateAlgo, m_alloc );
    }

    SAPDB_Bool Initialize(
        const SAPDB_UInt numberOfDistinctValuesFound )
    {
        m_frequencyList.Clear();
        m_numberOfDistinctValuesFound = numberOfDistinctValuesFound;

        return m_pEstimateAlgo != 0;
    }

    SAPDB_Bool AddValue(const SAPDB_UInt   duplicateCount)
    {
        SAPDB_Bool bInList = false;
        for ( SAPDB_UInt i = 0; i < m_frequencyList.GetSize(); ++i ) {
            if ( m_frequencyList[ i ].duplicateCount == duplicateCount ) {
                m_frequencyList[ i ].frequency++;
                bInList = true;
                break;
            }
        }

        if ( ! bInList ) {
            DuplicateFrequencyInfo si;
            si.duplicateCount = duplicateCount;
            si.frequency      = 1;

            if ( m_frequencyList.IsFull() ) {
                SAPDB_Bool bResizeOk =
                    m_frequencyList.Reserve( m_frequencyList.GetSize() * 2 );
                // ignore if resize failed
            }

            return m_frequencyList.InsertEnd( si );
        }

        return true;
    }

    SAPDB_Int4 Calculate()
    {
        if ( m_sampleSize >= m_tableRecordCount )
        {
            // don't estimate if our data is not sound
            if ( m_numberOfDistinctValuesFound < SAPDB_MAX_INT4 )
                return static_cast<SAPDB_Int4>( m_numberOfDistinctValuesFound );
            else
                return SAPDB_MAX_INT4;
        }

        if ( m_frequencyList.GetSize() == 0 )
            return 0;

        SAPDBAlgo_QuickSort(
            &m_frequencyList[0], m_frequencyList.GetSize() );

        SAPDB_Int8 verifyCounter = 0;
        for ( SAPDB_UInt i = 0; i < m_frequencyList.GetSize(); ++i ) {
            verifyCounter +=
                m_frequencyList[ i ].duplicateCount
                * m_frequencyList[ i ].frequency;
        }

        if ( verifyCounter != m_sampleSize ) {

            Kernel_VTrace() << "sampleCount not matched; is "
            << verifyCounter
            << ", should be " << m_sampleSize;
            SAPDBERR_ASSERT_STATE( verifyCounter == m_sampleSize );
            return -1;
        }

        SAPDB_Int8 result =
            m_pEstimateAlgo->EstimateDistinctValueCount(
                m_frequencyList,
                m_sampleSize,
                m_tableRecordCount,
                m_numberOfDistinctValuesFound );

        if ( result < m_numberOfDistinctValuesFound )
            result = m_numberOfDistinctValuesFound;

        if ( result > SAPDB_MAX_INT4 )
            result = SAPDB_MAX_INT4;

        return static_cast<SAPDB_Int4>( result );
    }

private:
    const SAPDB_Int8        m_sampleSize;
    const SAPDB_Int8        m_tableRecordCount;
    SAPDB_Int8              m_numberOfDistinctValuesFound;
    EstimateAlgorithm      *m_pEstimateAlgo;
    SAPDBMem_IRawAllocator &m_alloc;
    FrequencyList           m_frequencyList;
};

} // end anonymous namespace

/*==========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

void
Data_PrimTreeStatistic::CalculateColumnStatistic(
    Data_ITreeAccess            &treeAccessor,
    tbd_current_tree            &tempCurrent,
    const SAPDB_Int2            columnCount,
    SAPDB_Int2                  columnFirstPosition,
    SAPDB_Int2                  columnLastPosition,
    const tgg00_StEntryAddr     pColumnDescription,
    Data_PageCount              &numberOfLeaves,
    Data_PageCount              &numberOfRecords,
    tgg00_ColumnDistinctValues  &distinctValues )
{
    SAPDBTRACE_METHOD_DEBUG(
        "Data_PrimTreeStatistic::CalculateColumnStatistic",
        DataTree_Trace, 5 );

    const SAPDB_Bool                bCountDuplicates = true;
    TempStatisticRecord             tempRecord( m_Trans );

    if( !tempRecord.Initialize( MAX_RECLEN_GG00 ))
        return; // e_no_more_memory is set

    numberOfLeaves    = 0;
    numberOfRecords   = 0;

    columnFirstPosition -= POS_OFF_DIFF_BD00; // necessary because of pascal and C++ mix
    columnLastPosition  -= POS_OFF_DIFF_BD00; // and the usage as an array index

    for( SAPDB_Int2 columnIndex = 0; columnIndex < columnCount; columnIndex++ )
        distinctValues[ columnIndex ] = 0;

    while( e_ok == m_TrError )
    {
        Data_PrimPage &primPage = treeAccessor.GetNext();

        if(( e_ok != m_TrError ) || (! primPage.IsAssigned()))
            break; // common loop termination

        if( ! ColumnStatisticForSinglePage( ! bCountDuplicates, tempCurrent, tempRecord,
                                            primPage, columnCount, columnFirstPosition,
                                            columnLastPosition, pColumnDescription,
                                            distinctValues ))
            return;

        if( CommandCanceled())
            return;

        numberOfLeaves  += 1;
        numberOfRecords += primPage.GetRecordCount();
    }
}

/*---------------------------------------------------------------------------*/

void
Data_PrimTreeStatistic::SampleColumnStatistic(
    Data_ITreeAccess            &treeAccessor,
    tbd_current_tree            &tempCurrent,
    const SAPDB_Int2            columnCount,
    SAPDB_Int2                  columnFirstPosition,
    SAPDB_Int2                  columnLastPosition,
    const tgg00_StEntryAddr     pColumnDescription,
    Data_PageCount              &numberOfLeaves,
    Data_PageCount              &numberOfRecords,
    tgg00_ColumnDistinctValues  &distinctValues )
{
    SAPDBTRACE_METHOD_DEBUG(
        "Data_PrimTreeStatistic::SampleColumnStatistic", DataTree_Trace, 5 );

    const SAPDB_Bool bCountDuplicates       = true;
    SAPDB_Real8      sampledNumberOfRecords = 0;
    SAPDB_Real8      sampledNumberOfLeaves  = 0;

    TempStatisticRecord         tempRecord( m_Trans );

    columnFirstPosition -= POS_OFF_DIFF_BD00; // necessary because of pascal and C++ mix
    columnLastPosition  -= POS_OFF_DIFF_BD00; // and the usage as an array index

    for( SAPDB_Int2 columnIndex = 0; columnIndex < columnCount; columnIndex++ )
        distinctValues[ columnIndex ] = 0;

    if( ! tempRecord.Initialize( MAX_RECLEN_GG00 ))
        return; // e_no_more_memory is set

    while( e_ok == m_TrError )
    {
        Data_PrimPage &primPage = treeAccessor.GetNext();

        if(( e_ok != m_TrError ) || (! primPage.IsAssigned()))
            break; // common loop termination

        if( ! ColumnStatisticForSinglePage( ! bCountDuplicates, tempCurrent, tempRecord,
                                            primPage, columnCount, columnFirstPosition,
                                            columnLastPosition, pColumnDescription,
                                            distinctValues ))
            return;

        if( CommandCanceled())
            return;

        sampledNumberOfLeaves  += 1.0;
        sampledNumberOfRecords += primPage.GetRecordCount();
    }

    if( e_ok == m_TrError )
    {
        // estimate records and leaf nodes
        const SAPDB_Real8 averageNumberOfRecords   = sampledNumberOfRecords / sampledNumberOfLeaves;
        const SAPDB_Real8 estimatedNumberOfLeaves  = treeAccessor.GetNumberOfEstimatedLeafNodes();
        const SAPDB_Real8 estimatedNumberOfRecords = averageNumberOfRecords * estimatedNumberOfLeaves;

        numberOfRecords    = static_cast< Data_PageCount >( estimatedNumberOfRecords );
        numberOfLeaves     = static_cast< Data_PageCount >( estimatedNumberOfLeaves );

        // Old algorithmen from H.Bayen resp. K.-D. Reinsch

        EstimateNumberOfDistinctValues( estimatedNumberOfRecords, sampledNumberOfRecords,
                                        columnCount, distinctValues );
    }
}

/*---------------------------------------------------------------------------*/

void
Data_PrimTreeStatistic::SampleNewColumnStatistic(
    Data_ITreeAccess            &treeAccessor,
    tbd_current_tree            &tempCurrent,
    const SAPDB_Int2            columnCount,
    SAPDB_Int2                  columnFirstPosition,
    SAPDB_Int2                  columnLastPosition,
    const tgg00_StEntryAddr     pColumnDescription,
    Data_PageCount              &numberOfLeaves,
    Data_PageCount              &numberOfRecords,
    tgg00_ColumnDistinctValues  &distinctValues )
{
    SAPDBTRACE_METHOD_DEBUG( "Data_PrimTreeStatistic::SampleNewColumnStatistic", DataTree_Trace, 5 );

    const SAPDB_Bool bCountDuplicates       = true;
    SAPDB_Int8       sampledNumberOfRecords = 0;
    SAPDB_Int4       sampledNumberOfLeaves  = 0;

    TempStatisticRecord     tempRecord( m_Trans );

    columnFirstPosition -= POS_OFF_DIFF_BD00; // necessary because of pascal and C++ mix
    columnLastPosition  -= POS_OFF_DIFF_BD00; // and the usage as an array index

    for( SAPDB_Int2 columnIndex = 0; columnIndex < columnCount; columnIndex++ )
        distinctValues[ columnIndex ] = 0;

    if( ! tempRecord.Initialize( MAX_RECLEN_GG00 ))
        return; // e_no_more_memory is set

    while( e_ok == m_TrError )
    {
        Data_PrimPage &primPage = treeAccessor.GetNext();

        if(( e_ok != m_TrError ) || (! primPage.IsAssigned()))
            break; // common loop termination

        if( ! ColumnStatisticForSinglePage( bCountDuplicates, tempCurrent, tempRecord,
                                            primPage, columnCount, columnFirstPosition,
                                            columnLastPosition, pColumnDescription,
                                            distinctValues ))
            return;

        if( CommandCanceled())
            return;

        sampledNumberOfLeaves  += 1;
        sampledNumberOfRecords += primPage.GetRecordCount();
    }

    if( e_ok == m_TrError )
    {
        tgg00_TransContext& trans = *tempCurrent.curr_trans;

        SAPDB_Bool     skipEstimation = false;
        SAPDB_Int4     leafPageCount  = 0;
        SAPDB_Int4     indexPageCount;
        SAPDB_Int4     recordCount;

        tgg00_Surrogate tabSurr;
        tabSurr.rawAssign( treeAccessor.GetFileNo().GetCharArray() );
        bd998GetPageAndRecordCount (
            trans, tabSurr, leafPageCount, indexPageCount, recordCount);

        if ( leafPageCount == 0 )
        {
            // estimate records and leaf nodes
            const SAPDB_Int8 avgRecordsPerPage   =
                static_cast<SAPDB_Int8>(
                    .5 + static_cast<SAPDB_Real8>( sampledNumberOfRecords )
                    / static_cast<SAPDB_Real8>(sampledNumberOfLeaves )
                );
            leafPageCount  = treeAccessor.GetNumberOfEstimatedLeafNodes();

            if ( avgRecordsPerPage * leafPageCount < SAPDB_MAX_INT4 )
                recordCount =
                    static_cast<SAPDB_Int4>( avgRecordsPerPage * leafPageCount );
            else
                recordCount = SAPDB_MAX_INT4;
        }

        if ( sampledNumberOfLeaves >= leafPageCount )
        {
            leafPageCount  = sampledNumberOfLeaves;
            skipEstimation = true;
        }

        if ( sampledNumberOfRecords >= static_cast<SAPDB_Int8>( recordCount ) )
        {
            if ( sampledNumberOfRecords < SAPDB_MAX_INT4 )
                recordCount = static_cast<SAPDB_Int4>( sampledNumberOfRecords );
            else
                recordCount = SAPDB_MAX_INT4;
            skipEstimation = true;
        }

        if ( ! skipEstimation )
            NewEstimateNumberOfDistinctValues(
                leafPageCount,
                recordCount,
                sampledNumberOfRecords,
                columnCount, tempCurrent, distinctValues );

        numberOfLeaves  = leafPageCount;
        numberOfRecords = recordCount;
    }
}

/*---------------------------------------------------------------------------*/

void
Data_PrimTreeStatistic::CalculateRecordsAndLeaves(
    Data_PageCount  &numberOfLeaves,
    Data_PageCount  &numberOfRecords )
{
    SAPDBTRACE_METHOD_DEBUG( "Data_PrimTreeStatistic::CalculateRecordsAndLeaves", DataTree_Trace, 5 );

    Data_SerializableScanTreeAccess scanTreeAccessor( m_Current, Data_LruLast );

    numberOfLeaves    = 0;
    numberOfRecords   = 0;

    while( e_ok == m_TrError )
    {
        const Data_PrimPage &primPage = scanTreeAccessor.GetNext();

        if(( e_ok != m_TrError ) || (! primPage.IsAssigned()))
            break; // common loop termination

        if( CommandCanceled())
            return;

        numberOfLeaves  += 1;
        numberOfRecords += primPage.GetRecordCount();
    }
}

/*---------------------------------------------------------------------------*/

void
Data_PrimTreeStatistic::CalculateCluster(
    Data_PageCount&     numberOfLeaves,
    Data_PageCount&     numberOfClusters )
{
    SAPDBTRACE_METHOD_DEBUG( "Data_PrimTreeStatistic::CalculateCluster", DataTree_Trace, 5 );

    Trans_Context&  trans  = Trans_Context::GetContext( m_Trans );
    tgg00_FileId&   fileId = m_Current.curr_tree_id;

    Converter_PageCollector pageCollection( trans.Allocator(), Data_PageRecoveryMode(), 10000 ); // start with estimated 10000 pageNos

    if( ! pageCollection.Initialize()){
        trans.SetErrorCode( e_no_more_memory );
        return;
    }
    Data_IndexScanTreeAccess    treeAccessor( trans, fileId );
    treeAccessor.SetToRoot( m_Current.currRootNptrs_bd00 );

    numberOfLeaves   = 0;
    numberOfClusters = 0;

    while( trans.IsOk())
    {
        const Data_PrimPage&   page = treeAccessor.GetNext();

        if( ! trans.IsOk()){
            break; // common loop termination
        }
        CalculateClusterStatistic( trans.Task(), page, pageCollection, numberOfLeaves, numberOfClusters );
        if( this->CommandCanceled())
            return;
    }
    if( e_no_next_record == trans.OldTrans().trError_gg00 )
    {
        trans.ResetErrorCode();
        if( 0 == numberOfLeaves ){
            numberOfLeaves   = 1; // root is on leaf level
            numberOfClusters = 1;
        }
        else
        {
            pageCollection.DetermineCluster( trans.Task());
            numberOfClusters += pageCollection.GetClusterCount();
        }
    }
}

/*---------------------------------------------------------------------------*/

void
Data_PrimTreeStatistic::SampleRecordsAndLeaves(
    Data_ITreeAccess        &treeAccessor,
    Data_PageCount          &numberOfLeaves,
    Data_PageCount          &numberOfRecords )
{
    SAPDBTRACE_METHOD_DEBUG( "Data_PrimTreeStatistic::SampleRecordsAndLeaves", DataTree_Trace, 5 );

    SAPDB_Real8 sampledNumberOfRecords = 0;
    SAPDB_Real8 sampledNumberOfLeaves  = 0;

    while( e_ok == m_TrError )
    {
        const Data_PrimPage &primPage = treeAccessor.GetNext();

        if(( e_ok != m_TrError ) || (! primPage.IsAssigned()))
            break; // common loop termination

        if( CommandCanceled())
            return;

        sampledNumberOfLeaves  += 1.0;
        sampledNumberOfRecords += primPage.GetRecordCount();
    }

    if( e_ok == m_TrError )
    {
        // estimate records and leaf nodes
        const SAPDB_Real8 averageNumberOfRecords   = sampledNumberOfRecords / sampledNumberOfLeaves;
        const SAPDB_Real8 estimatedNumberOfLeaves  = treeAccessor.GetNumberOfEstimatedLeafNodes();
        const SAPDB_Real8 estimatedNumberOfRecords = averageNumberOfRecords * estimatedNumberOfLeaves;

        numberOfRecords    = static_cast< Data_PageCount >( estimatedNumberOfRecords );
        numberOfLeaves     = static_cast< Data_PageCount >( estimatedNumberOfLeaves );
    }
}

/*---------------------------------------------------------------------------*/

void
Data_PrimTreeStatistic::CalculateTableStatistic(
    Data_ITreeAccess        &treeAccessor,
    const SAPDB_UInt        numberOfLongColumns,
    Kernel_TableStatistic   &tableStatistic )
{
    SAPDBTRACE_METHOD_DEBUG( "Data_PrimTreeStatistic::SampleRecordsAndLeaves", DataTree_Trace, 5 );

    Trans_Context&  trans  = Trans_Context::GetContext( m_Trans );
    //  start with estimated 10000 pageNos
    Converter_PageCollector pageCollection( trans.Allocator(), Data_PageRecoveryMode(), 10000 );

    const bool bDoDetermineCluster = pageCollection.Initialize();
    Data_PageCount numberOfLeaves   = 0;
    Data_PageCount numberOfClusters = 0;

    while( trans.IsOk())
    {
        Data_PrimPage &primPage = treeAccessor.GetNext();

        if(( ! trans.IsOk())||( ! primPage.IsAssigned())){
            break; // common loop termination
        }
        if( LEAF_LEVEL_BD00 == primPage.GetLevel() )
        {
            if( 0 == numberOfLongColumns ){
                TableStatisticForSingleLeafPage( primPage, tableStatistic );
            }
            else{
                TableStatisticForSingleLeafPageWithLongColumns(
                    numberOfLongColumns, primPage, tableStatistic );
            }
        }
        else{
            TableStatisticForSingleNonLeafPage( primPage, tableStatistic );
            if(( bDoDetermineCluster )&&( FIRST_INDEX_LEVEL_BD00 == primPage.GetLevel())){
                CalculateClusterStatistic( trans.Task(), primPage, pageCollection,
                                           numberOfLeaves, numberOfClusters );
            }
        }
        if( this->CommandCanceled()){
            return;
        }
    }
    if( ! trans.IsOk()){
        return;
    }
    if( bDoDetermineCluster )
    {
        if( 0 == numberOfLeaves ){ // root is on leaf level
            tableStatistic.SetClusterCount( 1 );
        }
        else
        {
            pageCollection.DetermineCluster( trans.Task());
            numberOfClusters += pageCollection.GetClusterCount();
            tableStatistic.SetClusterCount( numberOfClusters );
        }
    }
    tableStatistic.CalculateResult();
}

/*---------------------------------------------------------------------------*/
/*                             private methods                               */
/*---------------------------------------------------------------------------*/

SAPDB_Bool
Data_PrimTreeStatistic::ColumnStatisticForSinglePage(
    const SAPDB_Bool             bCountDuplicates,
    tbd_current_tree             &tempCurrent,
    TempStatisticRecord          &tempRecord,
    Data_PrimPage                &primPage,
    const SAPDB_Int2             columnCount,
    const SAPDB_Int2             columnFirstPosition,
    const SAPDB_Int2             columnLastPosition,
    const tgg00_StEntryAddr      pColumnDescription,
    tgg00_ColumnDistinctValues   &distinctValues )
{
    SAPDBTRACE_METHOD_DEBUG( "Data_PrimTreeStatistic::ColumnStatisticForSinglePage", DataTree_Trace, 6 );

    const Data_PageCount    recordCount        = primPage.GetRecordCount();
    SAPDB_Int4              currLength;
    SAPDB_Int4              currPosition;
    SAPDB_Int2              currColumnPosition = columnFirstPosition;
    SAPDB_Int2              currColumnId       = 0;
    Data_RecordIndex        startRecordIndex   = FIRST_REC_INDEX_BD00;
    Data_RecordIndex        stopRecordIndex    = recordCount;

    ColumnList  columnList( m_Trans );

    if( ! bCountDuplicates ){
        if( ! columnList.Initialize( recordCount )) // e_no_more_memory is set
            return( SAPDB_FALSE );
    }

    for( ; currColumnPosition <= columnLastPosition; currColumnPosition++, currColumnId++ )
    {
        if( IsColumnUnique( pColumnDescription + currColumnPosition ))
        {
            // do not test the uniqueness of a unique column
            distinctValues[ currColumnId ] += recordCount;
            continue;
        }

        columnList.Clear();
        tgg00_VarColPosList varColumnPositionList;
        Data_Row            unpackedRow;
        bool                packedRowFound = false;
        // take a look at the not unique columns and count duplicate values
        for( Data_RecordIndex recordIndex = startRecordIndex; recordIndex < stopRecordIndex; recordIndex++ )
        {
            tgg00_Rec* pPrimRecord = primPage.GetRecord( recordIndex );

            if (reinterpret_cast<Data_Row*>(pPrimRecord)->Unpack( unpackedRow ))
            {
                pPrimRecord    = &unpackedRow;
                packedRowFound = true;
            }

            varColumnPositionList.vpl_last = -1;

            g04locate_col( *( pColumnDescription + currColumnPosition ),
                           pPrimRecord, varColumnPositionList, currPosition, currLength );

            if ( !packedRowFound && !bCountDuplicates )
            {
                columnList.Add( pPrimRecord, currPosition, currLength );

                if( ! columnList.AllUnique()) // column content still exist
                    continue;                 // do not test the uniqueness
            }

            if(( 0 == currLength ) || // empty column
                    ( csp_undef_byte == pPrimRecord->recBuf_gg00()[ currPosition - POS_OFF_DIFF_BD00 ] ))
                tempRecord.AssignSpecialKey( currColumnId );
            else
                tempRecord.AssignKey( currColumnId, currLength,
                                      &pPrimRecord->recBuf_gg00()[ currPosition - POS_OFF_DIFF_BD00 ] );
            if( e_ok == m_TrError )
            {
                if ( bCountDuplicates ) {
                    bd30AddToTempTree( bCountDuplicates, *(tempRecord.GetRecordPtr()), tempCurrent );
                } else {
                    b02add_record (m_Trans, tempCurrent.curr_tree_id, *(tempRecord.GetRecordPtr()));
                }
            }
            else
            {
                Data_Exception errMsg( __CONTEXT__, DATA_ERR_PRIM_STATISTIC,
                                       BasisErrorToString( m_TrError ),
                                       SAPDB_ToString( recordIndex, _T_d ),
                                       SAPDB_ToString( primPage.PageNo(), _T_d ),
                                       SAPDB_ToString( primPage.RootPageNo(), _T_d ));
                RTE_Message( errMsg );

                Kernel_FileIO   dumpFile( KERNEL_DATA_FILE, KERNEL_COR_FILE_EXTENSION,
                                          SAPDB_ToString( primPage.PageNo(), _T_d ));
                dumpFile.Write( primPage );
                return( SAPDB_FALSE );

            }

            if( e_ok == m_TrError )
                distinctValues[ currColumnId ] +=1;
            else if( e_duplicate_key == m_TrError )
                m_TrError = e_ok;
            else
                return( SAPDB_FALSE );
        }
    }
    return( e_ok == m_TrError );
}

/*---------------------------------------------------------------------------*/

void
Data_PrimTreeStatistic::TableStatisticForSingleLeafPage(
    Data_PrimPage           &primPage,
    Kernel_TableStatistic   &tableStatistic )
{
    SAPDBTRACE_METHOD_DEBUG( "Data_PrimTreeStatistic::TableStatisticForSingleLeafPage", DataTree_Trace, 6 );

    const Data_PageCount    recordCount = primPage.GetRecordCount();

    tableStatistic.AddLeafCovering( primPage.GetCovering() );
    tableStatistic.AddRecordCount( primPage.GetRecordCount() );

    for( Data_RecordIndex recordIndex = FIRST_REC_INDEX_BD00; recordIndex < recordCount; recordIndex++ )
    {
        const tgg00_Rec*  pPrimRecord = primPage.GetRecord( recordIndex );
        Data_Row unpackedRow;
        if (reinterpret_cast< const Data_Row* >( pPrimRecord )->Unpack( unpackedRow )){
            pPrimRecord = &unpackedRow;
        }
        int recLen;
        int keyLen;
        CnsRead_CachedRow::GetRecAndKeyLen ( pPrimRecord, recLen, keyLen );
        tableStatistic.AddRecord( recLen, keyLen );
    }
}

/*---------------------------------------------------------------------------*/

void
Data_PrimTreeStatistic::TableStatisticForSingleLeafPageWithLongColumns(
    const SAPDB_UInt        numberOfLongColumns,
    Data_PrimPage           &primPage,
    Kernel_TableStatistic   &tableStatistic )
{
    SAPDBTRACE_METHOD_DEBUG( "Data_PrimTreeStatistic::TableStatisticForSingleLeafPageWithLongColumns", DataTree_Trace, 6 );

    const Data_PageCount    recordCount = primPage.GetRecordCount();
    tgg00_FileId            blobFileId  = b01niltree_id;
    SAPDB_Int4              blobLength;
    SAPDB_Int4              blobPages;
    SAPDB_Int4              blobVersion;

    blobFileId.fileType_gg00().clear();
    blobFileId.fileType_gg00().addElement( ftsByteStr_egg00 );
    blobFileId.fileType_gg00().addElement( ftsConcurrent_egg00 );
    blobFileId.fileTfn_gg00().becomes( tfnColumn_egg00 );

    tableStatistic.AddLeafCovering( primPage.GetCovering() );
    tableStatistic.AddRecordCount( primPage.GetRecordCount() );

    for( Data_RecordIndex recordIndex = FIRST_REC_INDEX_BD00; recordIndex < recordCount; recordIndex++ )
    {
        tgg00_Rec*  pPrimRecord = primPage.GetRecord( recordIndex );
        int recLen;
        int keyLen;
        CnsRead_CachedRow::GetRecAndKeyLen ( pPrimRecord, recLen, keyLen );

        SAPDB_UInt  currColumnPosition = cgg_rec_key_offset + keyLen;

        tableStatistic.AddRecord( recLen, keyLen );

        for( SAPDB_UInt currColumn = 0; currColumn < numberOfLongColumns;
                currColumn++, currColumnPosition+= cgg_col_fd_size )
        {
            if( csp_undef_byte == pPrimRecord->recBuf_gg00()[ currColumnPosition ] )
                continue;

            blobFileId.fileRoot_gg00() = NIL_PAGE_NO_GG00;

            SAPDB_RangeMove( __FILE__, 1,
                             sizeof( pPrimRecord->recBuf_gg00()), sizeof( blobFileId.fileName_gg00()),
                             (pPrimRecord->recBuf_gg00()),
                             currColumnPosition + 1 + POS_OFF_DIFF_BD00, // do not copy define byte
                             &blobFileId.fileName_gg00(), 2 + POS_OFF_DIFF_BD00,
                             sizeof( tgg00_Surrogate ), m_TrError );

            if( e_ok != m_TrError )
                return;

            b05length_bytestr( m_Trans, blobFileId, blobLength, blobPages, blobVersion );

            if( e_ok == m_TrError )
                tableStatistic.AddLongColumn( blobLength, blobPages );
            else
            {
                if( e_file_not_found == m_TrError ) // column is assumed to be in short column file
                    m_TrError = e_ok;
                else
                    return;
            }
        }
    }
}

/*---------------------------------------------------------------------------*/

void
Data_PrimTreeStatistic::TableStatisticForSingleNonLeafPage(
    Data_PrimPage            &primPage,
    Kernel_TableStatistic    &tableStatistic )
{
    SAPDBTRACE_METHOD_DEBUG( "Data_PrimTreeStatistic::TableStatisticForSingleNonLeafPage", DataTree_Trace, 6 );

    const Data_PageCount    recordCount = primPage.GetRecordCount();

    if( primPage.RootPageNo() == primPage.PageNo() )
        tableStatistic.AddRootCovering( primPage.GetCovering(), primPage.GetLevel() );
    else
        tableStatistic.AddIndexCovering( primPage.GetCovering() );

    for( Data_RecordIndex recordIndex = FIRST_REC_INDEX_BD00; recordIndex < recordCount; recordIndex++ )
    {
        const tgg00_Rec*  pPrimRecord = primPage.GetRecord( recordIndex );

        tableStatistic.AddSeparator( pPrimRecord->recKeyLen_gg00() );
    }
}


/*---------------------------------------------------------------------------*/

void
Data_PrimTreeStatistic::NewEstimateNumberOfDistinctValues(
    const SAPDB_Int8             estimatedNumberOfLeafPages,
    const SAPDB_Int8             estimatedNumberOfTableRecords,
    const SAPDB_Int8             sampleSize,
    const SAPDB_Int2             columnCount,
    tbd_current_tree             &tempCurrent,
    tgg00_ColumnDistinctValues   &distinctValues )
{
    SAPDBTRACE_ROUTINE_DEBUG( "Data_PrimTreeStatistic::NewEstimateNumberOfDistinctValues", DataTree_Trace, 6 );

    Data_ScanTreeAccess     scanTreeAccessor( tempCurrent, Data_LruMiddle );
    TempStatisticRecord     tempRecord( m_Trans );

    EstimateDistinctValues  estimateDistinctValues(
        GetAllocator( m_Trans ),
        sampleSize,
        estimatedNumberOfTableRecords );

#   ifdef SAPDB_SLOW
    if( DataTree_Trace.TracesLevel( 6 ))
    {
        Kernel_VTrace() << "ESTIMATE DISTINCT VALUES " << NewLine <<
        "#sampled records (n): " << sampleSize <<
        " estimated records (N): " << estimatedNumberOfTableRecords << NewLine;
    }
#   endif

    SAPDB_Int curColumnId = -1;
    while( e_ok == m_TrError )
    {
        Data_PrimPage &tempPage = scanTreeAccessor.GetNext();

        if(( e_ok != m_TrError ) || (! tempPage.IsAssigned()))
            break; // common loop termination

        const Data_PageCount recordCount = tempPage.GetRecordCount();

        for( Data_RecordIndex recordIndex = FIRST_REC_INDEX_BD00;
                recordIndex < recordCount;
                recordIndex++ )
        {
            tgg00_Rec* pTempRecord = tempPage.GetRecord( recordIndex );

            tempRecord.AssignRecord( pTempRecord );

            SAPDB_Int columnId       = tempRecord.GetColumnId();
            SAPDB_Int duplicateCount = tempRecord.GetDuplicateCount();

            if ( columnId != curColumnId ) {
                if ( curColumnId != -1 ) {
                    distinctValues[ curColumnId ] =
                        estimateDistinctValues.Calculate();
                    if ( distinctValues[ curColumnId ] == -1 )
                        m_TrError = e_system_error;

#                   ifdef SAPDB_SLOW
                    if( DataTree_Trace.TracesLevel( 6 ))
                    {
                        Kernel_VTrace()
                        << "Distinct value: "
                        << distinctValues[ curColumnId ] << NewLine;
                    }
#                   endif
                }

                curColumnId = columnId;

                estimateDistinctValues.Initialize(
                    distinctValues[ curColumnId ] );
            }

            if ( ! estimateDistinctValues.AddValue( duplicateCount ) )
                m_TrError = e_no_more_memory;
        }
    }

    if ( curColumnId != -1 ) {
        distinctValues[ curColumnId ] =
            estimateDistinctValues.Calculate();
        if ( distinctValues[ curColumnId ] == -1 )
            m_TrError = e_system_error;
#       ifdef SAPDB_SLOW
        if( DataTree_Trace.TracesLevel( 6 ))
        {
            Kernel_VTrace() << "Distinct value: " << distinctValues[ curColumnId ] << NewLine;
        }
#       endif
    }
}

/*---------------------------------------------------------------------------*/

void
Data_PrimTreeStatistic::EstimateNumberOfDistinctValues(
    const SAPDB_Real8           estimatedNumberOfRecords,
    const SAPDB_Real8           sampledNumberOfRecords,
    const SAPDB_Int2            columnCount,
    tgg00_ColumnDistinctValues  &distinctValues )
{
    SAPDBTRACE_METHOD_DEBUG( "Data_PrimTreeStatistic::EstimateDistinctValues", DataTree_Trace, 6 );

    if( DataTree_Trace.TracesLevel( 6 ))
    {
        Kernel_VTrace() << "ESTIMATE DISTINCT VALUES " << NewLine <<
        " sampled records : "   << sampledNumberOfRecords   <<
        " estimated records : " << estimatedNumberOfRecords << NewLine;
    }

    for( SAPDB_Int2 columnNo = 0; columnNo < columnCount; columnNo++  )
    {
        SAPDB_Real8 distinctInSample = distinctValues[ columnNo ];

        distinctValues[ columnNo ] = EstimateDistinctValuesPerColumn(
                                         estimatedNumberOfRecords,
                                         sampledNumberOfRecords,
                                         distinctInSample );

        if( DataTree_Trace.TracesLevel( 6 ))
        {
            Kernel_VTrace() << "columnNo: " << columnNo << NewLine <<
            " sampled distinct : "   << distinctInSample <<
            " estimated distinct : " << distinctValues[ columnNo ] << NewLine;
        }
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_Int4
Data_PrimTreeStatistic::EstimateDistinctValuesPerColumn(
    const SAPDB_Real8   estimatedNumberOfRecords,
    const SAPDB_Real8   sampledNumberOfRecords,
    const SAPDB_Real8   distinctInSample )
{
    // The input variables should meet the following unequations:
    // 0 <= distinctInSample <= sampledNumberOfRecords <= estimatedNumberOfRecords

    if( distinctInSample <= 1 )
    {
        if( distinctInSample < 0 ){
            return( 0 );
        }
        else
            return( (SAPDB_Int4) distinctInSample );
    }

    if( sampledNumberOfRecords < distinctInSample ){
        return( (SAPDB_Int4) distinctInSample );
    }

    if( estimatedNumberOfRecords < sampledNumberOfRecords ){
        return( (SAPDB_Int4) distinctInSample );
    }

    // Start destinct value determination

    const SAPDB_Real8 qpr = distinctInSample / sampledNumberOfRecords;

    if( sampledNumberOfRecords <= 1.05 * distinctInSample ){
        return( (SAPDB_Int4) (0.5 + qpr * estimatedNumberOfRecords ));
    }

    const SAPDB_Real8   atol = sqrt( 2.2204460492503131e-16 );
    const SAPDB_Real8   qps  = (distinctInSample - 1.0) / (sampledNumberOfRecords - 1.0);
    const SAPDB_Real8   qpp  = (sampledNumberOfRecords - 1.0) / sampledNumberOfRecords;

    SAPDB_Real8         rtj  = (distinctInSample - 1.0) /  distinctInSample;
    SAPDB_Real8         rtjn = 0.0;
    SAPDB_Real8         h    = 0.0;
    tsp00_NumError      numError;

    for( SAPDB_UInt loopCount = 0; loopCount < 40; loopCount++ )
    {
        if( rtj <= 0 )
            h = 0;
        else
        { // TODO
            h = ( sampledNumberOfRecords - 1.0 ) * vln( rtj, numError );  // < 0
            h = vexp( h, numError);                                       // 0 <= h < 1
        }

        rtjn = qpp * ( h * rtj - qps ) / ( h - qpr );

        if( myAbsolute( rtjn - rtj ) > atol * myAbsolute( 1.0 - rtj ))
            rtj = rtjn;
        else
            break;
    }

    if(( 0.0 >= rtjn ) || ( rtjn >= 1.0 ))
    {
        return( (SAPDB_Int4) (0.5 + qpr * estimatedNumberOfRecords ));
    }

    h = ( rtjn - 1.0 ) * estimatedNumberOfRecords;
    h = 1.0 - vexp( h, numError );

    if( h > (( 1.0 - rtjn ) * qpr * estimatedNumberOfRecords ))
    {
        return( (SAPDB_Int4) (0.5 + qpr * estimatedNumberOfRecords ));
    }
    return( (SAPDB_Int4) (0.5 + h / ( 1.0 - rtjn )));
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
Data_PrimTreeStatistic::CommandCanceled()
{
    if( e_ok != m_TrError )
        return( true );

    if( ! m_Trans.trRteCommPtr_gg00->to_cancel )
        return( false );

    m_TrError = e_cancelled;

    Data_Exception errMsg( __CONTEXT__, DATA_PRIM_STATISTIC_CANCELED,
                           SAPDB_ToString( m_Current.curr_tree_id.fileRoot_gg00(), _T_d ),
                           SAPDB_ToString( m_Trans.trTaskId_gg00, _T_d ));
    RTE_Message( errMsg );

    return( true );
}

/*===========================================================================*
 *  LOCAL CLASSES (CODE)                                                     *
 *===========================================================================*/

SAPDB_Bool
ColumnList::Initialize( const SAPDB_UInt4 recordCount )
{
    if( m_Columns.Reserve( recordCount ))
        return( SAPDB_TRUE );

    m_TrError = e_no_more_memory;

    return( SAPDB_FALSE );
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
ColumnList::Add(
    tgg00_Rec*          pRec,
    const SAPDB_Int4    position,
    const SAPDB_Int4    length )
{
    ColumnPosition  item;

    item.cpl_pRec     = pRec;
    item.cpl_Position = position;
    item.cpl_Length   = length;

    if( m_Columns.InsertEnd( item ))
        return( SAPDB_TRUE );

    m_TrError = e_no_more_memory;

    return( SAPDB_FALSE );
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
ColumnList::AllUnique() const
{
    if( 2 > m_Columns.GetSize())
        return( SAPDB_TRUE ); // one entry is unique

    tsp00_LcompResult   compareResult;

    const SAPDB_UInt last = m_Columns.GetSize() -1;

    for( SAPDB_UInt first = 0; first < last; first++ )
    {
        s30luc(
            m_Columns[first].cpl_pRec,
            m_Columns[first].cpl_Position,
            m_Columns[first].cpl_Length,
            m_Columns[last].cpl_pRec,
            m_Columns[last].cpl_Position,
            m_Columns[last].cpl_Length,
            compareResult
        );
        if( l_equal == compareResult )
            return( SAPDB_FALSE ); // not unique on this page
    }
    return( SAPDB_TRUE ); // is unique on this page
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

SAPDB_Bool
TempStatisticRecord::Initialize( const SAPDB_UInt4 capacity )
{
    if(( cgg_rec_key_offset + PERM_KEY_MXSP00 ) <= capacity )
    {
        if( capacity > m_Capacity )
        {
            if( 0 != m_Capacity )
                Delete();
            m_pRec = reinterpret_cast< tgg00_RecPtr > (m_Allocator.Allocate( capacity ));
            if( NULL != m_pRec )
                m_Capacity = capacity;
        }
    }
    if( NULL == m_pRec )
    {
        m_TrError = e_no_more_memory;
        return( false );
    }
    InitRecordHeader();
    return( true );
}

/*---------------------------------------------------------------------------*/

void
TempStatisticRecord::AssignSpecialKey( SAPDB_Int2 columnId )
{
    SAPDBERR_ASSERT_STATE( NULL != m_pRec );
    SAPDBERR_ASSERT_STATE( 0 != m_Capacity );

    m_pRec->recLen_gg00()          = sizeof( columnId ) + cgg_rec_key_offset;
    m_pRec->recKeyLen_gg00()       = sizeof( columnId );
    m_pRec->recPno_gg00().becomes( 1 );

    // Copy logical column identifier to the begining of the key.
    // This is necessary to make equal values of different columns
    // in a temporary destination file distinguishable

    SAPDB_RangeMove( __FILE__, 2,
                     sizeof( columnId ), m_Capacity,
                     &columnId, POS_OFF_DIFF_BD00,
                     &( m_pRec->recBody_gg00()), POS_OFF_DIFF_BD00,
                     sizeof( columnId ), m_TrError );

    if( e_move_error == m_TrError )
        m_TrError = e_data_page_corrupted;
}

/*---------------------------------------------------------------------------*/

void
TempStatisticRecord::AssignKey(
    const SAPDB_Int2    columnId,
    SAPDB_Int2          keyLength,
    const SAPDB_Byte*   pKey )
{
    SAPDBERR_ASSERT_STATE( NULL != m_pRec );
    SAPDBERR_ASSERT_STATE( 0 != m_Capacity );

    const SAPDB_Int cMaxRecKeyLength = PERM_KEY_MXSP00 - sizeof( columnId );
    if( keyLength > cMaxRecKeyLength )
        keyLength = cMaxRecKeyLength; // TODO LATER handle columns longer than 1024

    m_pRec->recLen_gg00()          = cgg_rec_key_offset + sizeof( columnId ) + keyLength;
    m_pRec->recKeyLen_gg00()       =  sizeof( columnId ) + keyLength;
    m_pRec->recPno_gg00().becomes( 1 );

    // Copy logical column identifier to the begining of the key.
    // This is necessary to make equal values of different columns
    // in a temporary destination file distinguishable

    SAPDB_RangeMove( __FILE__, 3,
                     sizeof( columnId ), m_Capacity,
                     &columnId, POS_OFF_DIFF_BD00,
                     &( m_pRec->recBody_gg00()), POS_OFF_DIFF_BD00,
                     sizeof( columnId ), m_TrError );

    SAPDB_RangeMove( __FILE__, 4,
                     keyLength, m_Capacity,
                     pKey, POS_OFF_DIFF_BD00,
                     &( m_pRec->recBody_gg00()[ sizeof( columnId ) ]), POS_OFF_DIFF_BD00,
                     keyLength, m_TrError );

    if( e_move_error == m_TrError )
        m_TrError = e_data_page_corrupted;

}
/*---------------------------------------------------------------------------*/

SAPDB_Int2
TempStatisticRecord::GetColumnId() const
{
    SAPDBERR_ASSERT_STATE( NULL != m_pRec );

    SAPDB_Int2  columnId;

    SAPDB_RangeMove( __FILE__, 5,
                     m_pRec->recKeyLen_gg00(), sizeof( columnId ),
                     &( m_pRec->recBody_gg00()), POS_OFF_DIFF_BD00,
                     &columnId, POS_OFF_DIFF_BD00,
                     sizeof( columnId ), m_TrError );

    return( columnId );
}

/*---------------------------------------------------------------------------*/

void
TempStatisticRecord::InitRecordHeader()
{
    if( NULL == m_pRec)
        return;

    m_pRec->recLen_gg00()    = 0;
    m_pRec->recKeyLen_gg00() = 0;
    m_pRec->recPno_gg00().becomes( 0 );
}

/*---------------------------------------------------------------------------*/

void
TempStatisticRecord::Delete()
{
    if(( NULL != m_pRec ) && ( 0 < m_Capacity ))
        m_Allocator.Deallocate( m_pRec );

    m_pRec     = NULL;
    m_Capacity = 0;
}
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/


/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
