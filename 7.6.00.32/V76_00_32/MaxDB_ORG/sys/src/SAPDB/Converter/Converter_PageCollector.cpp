/*
  @file           Converter_PageCollector.cpp
  @author         TorstenS
  @ingroup        Converter
  @brief          This module is used to ...

\if EMIT_LICENCE
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

\endif
*/



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "Converter/Converter_Types.hpp"
#include "Converter/Converter_PageCollector.hpp"
#include "Converter/Converter_IPageNoMapping.hpp"
#include "DataAccess/Data_BasePage.hpp"
#include "IOManager/IOMan_IDataInfo.hpp"
#include "IOManager/IOMan_ClusterAddress.hpp"
#include "KernelCommon/Kernel_VTrace.hpp"
#include "RunTime/Tasking/RTETask_ITask.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDBCommon/SAPDB_EnhancedTypes.hpp"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

typedef SAPDB_Invalidatable<SAPDB_UInt, SAPDB_MAX_UINT4> StartIndex;

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/



/*===========================================================================*
 *  METHODS -lll-                                                            *
 *===========================================================================*/

Converter_PageCollector::Converter_PageCollector(
    SAPDBMem_IRawAllocator&         allocator,
    const Data_PageRecoveryMode&    recMode,
    const SAPDB_UInt                initialCapacity )
        :
        m_Allocator( allocator ),
        m_RecMode( recMode ),
        m_DataIOBlockCount( IOMan_IDataInfo::GetInstance().GetDataIOBlockCount()),
        m_InitialCapacity( SAPDB_MAX( m_DataIOBlockCount, initialCapacity )),
        m_ClusterCount( 0 ),
        m_ClusterItems( m_Allocator )
{}

/*---------------------------------------------------------------------------*/

void
Converter_PageCollector::DetermineCluster( RTETask_ITask&   task )
{
    m_ClusterCount = 0;
    if( 0 == m_ClusterItems.GetSize()){ // no cluster item => no cluster
        return;
    }
    Converter_IPageNoMapping&   iConv = Converter_IPageNoMapping::Instance();

    SAPDB_UInt    currClusterId = 0;
    StartIndex    startClusterIndex; // SAPDB_Invalidatable !

    ClusterItemList::Iterator       iter    = m_ClusterItems.Begin();
    ClusterItemList::ConstIterator  endIter = m_ClusterItems.End();

    for( ; iter != endIter; iter++ )
    {
        const SAPDB_UInt    index  = m_ClusterItems.GetIndex( iter );
        const Data_PageNo   pageNo = iter->GetPageNo();
        IOMan_BlockAddress  block;

        const Converter_ReturnCode  retCode = iConv.GetPositionForReading( task, pageNo, m_RecMode, block );

        if( Converter_Ok != retCode ){ // page  could be reserved or corrupted etc.
            block.Invalidate();
        }
        iter->SetBlock( block );
        if( ! block.IsValid())
        {
            startClusterIndex.Invalidate();  // cluster end
            iter->SetClusterId( 0 );         // belongs to no cluster
            continue;
        }
        if( startClusterIndex.IsInvalid()){
            startClusterIndex = this->AssignNewClusterHead( *iter, ++currClusterId, index );
            continue;
        }
        const IOMan_BlockAddress& prevBlock = m_ClusterItems[ index -1 ].GetBlock();
        SAPDBERR_ASSERT_STATE( prevBlock.IsValid()); // because startClusterIndex is valid
        if(
            ( prevBlock.GetVolumeNo()  != block.GetVolumeNo()) ||
            ( prevBlock.GetBlockNo()+1 != block.GetBlockNo())  ||
            ( m_ClusterItems[ startClusterIndex ].GetBlockCount() >= m_DataIOBlockCount )
        )
        {
            startClusterIndex = this->AssignNewClusterHead( *iter, ++currClusterId, index );
            continue;
        }
        // belongs to the same cluster as the previous item
        iter->SetClusterId( currClusterId );
        iter->SetBlockCount( 0 );  // count blocks only in cluster head item
        m_ClusterItems[ startClusterIndex ].IncrementBlockCount();
    }
}

/*---------------------------------------------------------------------------*/

bool
Converter_PageCollector::GetCluster(
    const SAPDB_UInt        clusterId,
    Data_PageNoList&        pageNoList,
    IOMan_ClusterAddress&   cluster )
{
    SAPDBERR_ASSERT_STATE( 0 != this->GetClusterCount()); // first call DetermineCluster

    if( 0 == this->GetClusterCount()){
        return false;
    }
    SAPDB_UInt startIndex = 0;
    if( ! this->SearchClusterHead( clusterId, startIndex )){
        return false;
    }
    const IOMan_BlockAddress&   block      = m_ClusterItems[ startIndex ].GetBlock();
    const IOMan_BlockCount      blockCount = m_ClusterItems[ startIndex ].GetBlockCount();
    const SAPDB_UInt            stopIndex  = startIndex + blockCount;

    cluster.SetCluster( block, blockCount ); // beginning of cluster
    pageNoList.Clear();

    for( SAPDB_UInt index = startIndex; index < stopIndex; index++ )
    { // add pageNo's
        const Data_PageNo pageNo = m_ClusterItems[ index ].GetPageNo();
        if( ! pageNoList.InsertEnd( pageNo )){
            return false;
        }
    }
    if( Converter_Trace.TracesLevel( 6 ))
    {
        Kernel_VTrace trace;
        trace << "Conv_Cluster::Get - ClusterId: " << clusterId
        << " start block: " << cluster.GetVolumeNo() << "/"
        << cluster.GetBlockNo( 0 ) << "("
        << cluster.GetBlockCount() << ")"
        << " start pageNo: " <<  pageNoList[ 0 ] << FlushLine;
    }
    return true;
}


/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/

