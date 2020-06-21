/****************************************************************************

  module      : Pager_Writer.cpp

  -------------------------------------------------------------------------

  responsible : Henrik

  special area:
  description : This description is currently not used, as
    documentation is only generated from header files
-
  last changed: 2002-03-07
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

#include "Pager/Pager_Writer.hpp"
#include "Pager/Pager_Types.hpp"
#include "Pager/Pager_Controller.hpp"
#include "Pager/Pager_Exception.hpp"
#include "Pager/Pager_Messages.hpp"
#include "RunTime/RTE_Message.hpp"
#include "KernelCommon/Kernel_IAdminRestartShutdown.hpp"
#include "IOManager/IOMan_IDataIO.hpp"
#include "IOManager/IOMan_IDataInfo.hpp"
#include "FreeBlockManagement/FBM_IManager.hpp"
#include "Converter/Converter_IPageIO.hpp"


#include "heo56.h"          // vsleep
#include "hbd20_5.h"        // bd20*
#include "hgg04.h"          // gg04isStaticPage
#include "gbd201.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/



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
 *  METHODS                                                                  *
 *===========================================================================*/


Pager_Writer::Pager_Writer( Trans_Context &trans )
        :
        m_blockCount(IOMan_IDataInfo::GetInstance().GetDataIOBlockCount()),
        m_trans( trans ),
        m_error(trans.OldTrans().trError_gg00),
        m_task( m_trans.Task()),
        m_pagerController(Pager_Controller::GetInstance()),
        m_pages( trans.Allocator(), UTF8( "Pager:WriteDataPages" ))
{};

/*---------------------------------------------------------------------------*/

void Pager_Writer::Initialize(){
    SAPDBTRACE_ROUTINE_DEBUG("Pager_Writer::Initialize", Pager_Trace, 5 );

    const SAPDB_Int numCaches = g01region_cnt (rgnData_egg00);

    m_pages.Initialize(m_blockCount);

    m_myIndex = m_pagerController.RegisterNextPagerWriter( m_task.ID(), this);

    PagerWriterIterator current = Begin();
    const PagerWriterIterator& iterEnd = End();

    {
        Pager_Exception StartTaskID(__CONTEXT__, PAGER_WRITER_START_TASKID,
                                    SAPDB_ToString(m_task.ID(), _T_d));
        RTE_Message (StartTaskID);

        Pager_Exception FirstCache(__CONTEXT__, PAGER_WRITER_FIRST_CACHE,
                                   SAPDB_ToString(current.value(), _T_d));
        RTE_Message (FirstCache);
    }

    current++;

    while (current != iterEnd)
    {
        Pager_Exception NextCache(__CONTEXT__, PAGER_WRITER_NEXT_CACHE,
                                  SAPDB_ToString(current.value(), _T_d));
        RTE_Message (NextCache);

        current++;
    }
    Run();
}

/*---------------------------------------------------------------------------*/

void Pager_Writer::Run() {

    SAPDBTRACE_ROUTINE_DEBUG("Pager_Writer::Run", Pager_Trace, 5 );

    const SAPDB_Bool isArchive = g01is_archive();

    while(true) {
        if ((e_ok != m_error ) &&
                (e_shutdown != m_error)) {        // PTS 1107037 2000-06-26
            Kernel_IAdminRestartShutdown::Instance().Offline(m_error, true);
            break;
        }

        if ( e_shutdown == m_error) {          // PTS 1107037 TS 2000-06-26
            m_error  = e_ok; // PTS 1107037 TS 2000-06-26 *)
        }

        m_pagerController.SuspendPagerWriter(m_myIndex, m_task.ID());  // sleep

        switch(m_pagerController.GetPagerOrder()) {
        case poFlushDataCache :
        case poFlushDataCacheBeforePrepare :
            if (isArchive)
            {
                FlushDataCacheBeforePrepareSequential();
            }
            else
            {
                FlushClusteredFiles();
                FlushDataCacheBeforePrepare();
            }
            break;
        case poFlushDataCacheAfterPrepare :
            if (isArchive)
            {
                FlushDataCacheAfterPrepareSequential();
            }
            else
            {
                FlushClusteredFiles();
                FlushDataCacheAfterPrepare();
            }
            break;
        case poFlushDataCacheWithTempFromLRURechain:
			FlushTempPagesFromLRURechain();
			m_pagerController.GetClusterer().PrepareFlush(m_task.ID());  // first one wins
			FlushClusteredFiles();
			FlushDataCacheBeforePrepare();
            break;
        case poReadConverterCache:
            ReadConverter( SAPDB_FALSE );
            break;
        case poReadSnapShotConverter:
            ReadConverter( SAPDB_TRUE );
            break;
        case poFlushConverterCache:
            FlushConverter();
            break;
        }
    }
}

/*---------------------------------------------------------------------------*/

void Pager_Writer::FlushTempPagesFromLRURechain() {

    SAPDBTRACE_ROUTINE_DEBUG("Pager_Writer::FlushTempPagesFromLRURechain", Pager_Trace, 5 );

    SAPDB_Int2              curPageCnt;
    tbd00_NodePtrList       nodePtrList;
    tbd2_data_cb_flush_list cbptr_list;
    SAPDB_Int               totalIO;
    SAPDB_Int               totalPages;
    SAPDB_Int               dummy = 0;

    IOMan_BlockAddress startBlock;
    IOMan_ClusterAddress cluster;

    totalIO    = 0;
    totalPages = 0;

    PagerWriterIterator iter = Begin();
    const PagerWriterIterator& iterEnd = End();

    while ((e_ok == m_error) &&
            (iter != iterEnd) &&
            !m_pagerController.IsPreparePagerWriter()){

        curPageCnt = m_blockCount;
        bd20GetTempFromLRURechain (m_task.ID(), iter.value(), m_error, curPageCnt, nodePtrList, cbptr_list);

        if (e_ok == m_error) {
            if (curPageCnt > 0) {
                GetFreeBlocksAndWriteDataPages(nodePtrList, cbptr_list, curPageCnt, 
                    SAPDB_FALSE, totalIO, totalPages, dummy);

                if (e_ok == m_error){
                    bd20ResetIOState(m_task.ID(), iter.value(), curPageCnt, cbptr_list);
                }
            }
            else
            {
                iter++;
            }
        }
    }

    m_pagerController.AddDataIOTotal(totalIO);
    m_pagerController.AddDataPagesTotal(totalPages);
}

/*---------------------------------------------------------------------------*/

void Pager_Writer::GetFreeBlocksAndWriteDataPages(
    tbd00_NodePtrList       &pNodes,
    tbd2_data_cb_flush_list &cbptrList,
    SAPDB_Int               numBlocks,
    SAPDB_Bool1             isSequential,
    SAPDB_Int               &totalIO,
    SAPDB_Int               &totalPages,
    SAPDB_Int               &totalIOElems){

    SAPDB_Int pagesLeftToWrite = numBlocks;
    SAPDB_Int pagesWritten = 0;
    IOMan_ClusterAddress cluster;

    while ((e_ok == m_error) && (pagesLeftToWrite > 0) )
    {
        cluster = FBM_IManager::Instance().GetMultFreeBlocks(
                      m_task.ID(), pagesLeftToWrite, isSequential );

        if( 0 == cluster.GetBlockCount())
        {
            m_error = e_shutdown;
            break;
        }

        pagesWritten = cluster.GetBlockCount();

        if( 0 == pagesWritten )
        {
            m_error = e_shutdown;
            break;
        }

        WriteDataPages (pNodes, cbptrList, numBlocks-pagesLeftToWrite, cluster);

        pagesLeftToWrite -= pagesWritten;

        totalIO++;
        totalPages += pagesWritten;
        totalIOElems -= pagesWritten;
    }
}

/*---------------------------------------------------------------------------*/

void Pager_Writer::FlushClusteredFiles () { 
    SAPDB_Bool        completed = false;
    IOMan_BlockCount  pageCount;
    IOMan_BlockCount  startPage;
    IOMan_BlockCount  clusterThreshold;
    bd201_Clusterer  &clusterer = m_pagerController.GetClusterer();
    IOMan_ClusterAddress cluster;
    SAPDB_Int         totalIO;
    SAPDB_Int         totalPages;
        
    totalPages = 0;
    totalIO    = 0;
    
    clusterThreshold = static_cast< IOMan_BlockCount >(m_blockCount * m_pagerController.GetClusterWriteThreshold() / 100);
    
    while (!completed)
    {
        m_pages.Clear();
        startPage = 0;
        clusterer.GetPagesForIO(m_task.ID(), m_pages, m_blockCount, SAPDB_FALSE );
        if (0 == m_pages.GetPageCount())
        {
            completed = true;
        }
        else
        {       
            while (0 != (pageCount = (m_pages.GetPageCount() - startPage)))
            {
                if (pageCount <= clusterThreshold)
                {
                    // write unclustered
                    FBM_IManager::Instance().GetMultFreeBlocks_2(m_task, m_trans.ErrorList(), pageCount, cluster);
                }
                else
                {
                    // get clustered blocks
                    pageCount = SAPDB_MIN(pageCount, m_blockCount);
                    FBM_IManager::Instance().GetFreeCluster(
                                  m_task, m_trans.ErrorList(), pageCount, cluster);
                }
                pageCount = SAPDB_MIN(pageCount, cluster.GetBlockCount());
                cluster.SetBlockCount( pageCount );
                IOMan_IDataIO::GetInstance().WriteDataPages( m_task.ID(), m_pages, cluster, startPage );
                totalIO++;
                totalPages += pageCount;
                         
                if (e_ok == m_error){
                    clusterer.ResetIOState(m_task.ID(), m_pages, startPage, pageCount);
                }
                startPage += pageCount;
            }
        }
    }
    
    m_pagerController.AddDataIOTotal(totalIO);
    m_pagerController.AddDataPagesTotal(totalPages); 
}

/*---------------------------------------------------------------------------*/

void Pager_Writer::FlushDataCacheBeforePrepare () {  // non sequential

    SAPDBTRACE_ROUTINE_DEBUG("Pager_Writer::FlushDataCacheBeforePrepare", Pager_Trace, 5 );

    SAPDB_Bool1             dummy;
    IOMan_BlockCount        pageCnt;
    SAPDB_Int2              curPageCnt;
    SAPDB_Int               totalIO;
    SAPDB_Int               totalPages;
    SAPDB_Int               totalIOElems;
    tbd00_NodePtrList       nodePtrList;
    tbd2_data_cb_flush_list cbptrList;

    PagerWriterIterator current = Begin();
    const PagerWriterIterator& iterEnd = End();

    IOMan_BlockAddress  startBlock;
    IOMan_ClusterAddress cluster;

    m_error      = e_ok;
    totalPages = 0;
    totalIO    = 0;

    do {  // all assigned cache segments
        totalIOElems = bd20IOElemCount (m_task.ID(), current.value() );
        
        while ((e_ok == m_error) && (totalIOElems > 0 ))
        {
            pageCnt = SAPDB_MIN(m_blockCount, totalIOElems);

            if (!FBM_IManager::Instance().GetMultFreeBlocks_2(m_task, m_trans.ErrorList(), pageCnt, cluster))
            {
                m_error = e_shutdown;
                break;
            }

            startBlock = cluster.GetBlockAddress(0);
            pageCnt    = cluster.GetBlockCount();

            while ((e_ok == m_error) && (pageCnt > 0) && (totalIOElems > 0)){
                curPageCnt = pageCnt;
                bd20GetPageForIO (m_task.ID(), current.value(), m_error,
                                  curPageCnt, nodePtrList, cbptrList, dummy);
                if  (0 == curPageCnt){
                    totalIOElems = 0;
                }
                else {
                    totalIOElems -= curPageCnt;
                }

                if ((e_ok == m_error) && (curPageCnt >0))
                {
                    totalIO++;
                    totalPages += curPageCnt;

                    cluster.SetCluster(startBlock, curPageCnt);
                    WriteDataPages (nodePtrList, cbptrList, 0, cluster);

                    if (e_ok == m_error){
                        bd20ResetIOState (m_task.ID(), current.value(), curPageCnt, cbptrList);
                        pageCnt   -= curPageCnt;
                        startBlock.SetBlockNo(startBlock.GetBlockNo() + curPageCnt);
                        cluster.SetCluster(startBlock, pageCnt);
                    }
                }
            }
        }

        FBM_IManager::Instance().SetClusterStateToFree(m_task.ID(), cluster);

        current++;
    } while ((e_ok == m_error) && (current != iterEnd));

    m_pagerController.AddDataIOTotal(totalIO);
    m_pagerController.AddDataPagesTotal(totalPages);
}

/*---------------------------------------------------------------------------*/

void Pager_Writer::FlushDataCacheBeforePrepareSequential () {

    SAPDBTRACE_ROUTINE_DEBUG("Pager_Writer::FlushDataCacheBeforePrepareSequential", Pager_Trace, 5 );

    SAPDB_Bool1             dummy;
    SAPDB_Int2              iPageCnt;
    SAPDB_Int               totalIO;
    SAPDB_Int               totalPages;
    SAPDB_Int               totalIOElems;
    tbd00_NodePtrList       nodePtrList;
    tbd00_NodePtrList       normalNodePtrList;
    tbd00_NodePtrList       seqNodePtrList;
    tbd2_data_cb_flush_list normalCbptrList;
    tbd2_data_cb_flush_list seqCbptrList;
    tbd2_data_cb_flush_list cbptrList;

    PagerWriterIterator current = Begin();
    const PagerWriterIterator& iterEnd = End();

    m_error      = e_ok;
    totalPages = 0;
    totalIO    = 0;

    do {  // all assigned cache segments
        totalIOElems = bd20IOElemCount (m_task.ID(), current.value() );
        do {
            iPageCnt = m_blockCount;

            bd20GetPageForIO (m_task.ID(), current.value(), m_error,
                              iPageCnt, nodePtrList, cbptrList, dummy);
            if  (0 == iPageCnt){
                break;
            }

            SAPDB_Int normalIdx = 0;
            SAPDB_Int seqIdx = 0;
            for (SAPDB_Int i = 0; i < iPageCnt; i++)
            {
                Data_BasePage page = Data_BasePage(nodePtrList[i]);
                if (Data_VolumeSequential != page.VolumeMode())
                {
                    normalNodePtrList[normalIdx] = nodePtrList[i];
                    normalCbptrList[normalIdx]   = cbptrList[i];
                    normalIdx++;
                }
                else
                {
                    seqNodePtrList[seqIdx] = nodePtrList[i];
                    seqCbptrList[seqIdx]   = cbptrList[i];
                    seqIdx++;
                }
            }

            GetFreeBlocksAndWriteDataPages(normalNodePtrList, normalCbptrList, normalIdx, 
                SAPDB_FALSE, totalIO, totalPages, totalIOElems);

            if (e_ok == m_error){
                GetFreeBlocksAndWriteDataPages(seqNodePtrList, seqCbptrList, seqIdx, 
                    SAPDB_TRUE, totalIO, totalPages, totalIOElems);
            }

            if (e_ok == m_error){
                bd20ResetIOState (m_task.ID(), current.value(), iPageCnt, cbptrList);
            }
        } while ((e_ok == m_error) && (totalIOElems > 0 ));

        current++;
    } while ((e_ok == m_error) && (current != iterEnd));

    m_pagerController.AddDataIOTotal(totalIO);
    m_pagerController.AddDataPagesTotal(totalPages);
}

/*---------------------------------------------------------------------------*/

void Pager_Writer::FlushDataCacheAfterPrepare() {

    SAPDBTRACE_ROUTINE_DEBUG("Pager_Writer::FlushDataCacheAfterPrepare", Pager_Trace, 5 );

    SAPDB_Bool1             svpCompleted;
    SAPDB_Bool              stopLoop;
    IOMan_BlockCount        pageCnt;
    SAPDB_Int2              curPageCnt;
    SAPDB_Int               loopCnt;
    SAPDB_Int               totalIO;
    SAPDB_Int               totalPages;
    tbd00_NodePtrList       nodePtrList;
    tbd2_data_cb_flush_list cbptrList;

    PagerWriterIterator current = Begin();
    const PagerWriterIterator& iterEnd = End();

    IOMan_BlockAddress  startBlock;
    IOMan_ClusterAddress cluster;

    m_error      = e_ok;
    totalPages = 0;
    totalIO    = 0;

    do {
        svpCompleted = SAPDB_FALSE;
        loopCnt      = 0;
        while ((e_ok == m_error) && !svpCompleted){
            pageCnt = m_blockCount;

            IOMan_ClusterAddress cluster;
            if (!FBM_IManager::Instance().GetMultFreeBlocks_2(m_task, m_trans.ErrorList(), pageCnt, cluster))
            {
                m_error = e_shutdown;
                break;
            }
            startBlock = cluster.GetBlockAddress(0);
            pageCnt   = cluster.GetBlockCount();

            stopLoop = false;
            while ((e_ok == m_error) && (pageCnt > 0) && (!stopLoop)){
                curPageCnt = pageCnt;
                bd20GetPageForIO (m_task.ID(), current.value(), m_error, curPageCnt,
                                  nodePtrList, cbptrList, svpCompleted);
                if (e_ok == m_error){
                    if (curPageCnt > 0){
                        totalIO++;
                        totalPages += curPageCnt;

                        cluster.SetCluster(startBlock, curPageCnt);
                        WriteDataPages (nodePtrList, cbptrList, 0, cluster);

                        if (e_ok == m_error){
                            bd20ResetIOState (m_task.ID(), current.value(), curPageCnt, cbptrList);
                            pageCnt -= curPageCnt;
                            startBlock.SetBlockNo(startBlock.GetBlockNo() + curPageCnt);
                            cluster.SetCluster(startBlock, pageCnt);
                        }
                    }
                    else {
                        stopLoop = true;
                    }
                }
            }

            FBM_IManager::Instance().SetClusterStateToFree(m_task.ID(), cluster);

            if ((e_ok == m_error) && (!svpCompleted) && stopLoop) {
                if (loopCnt >= 5){
                    vsleep (m_task.ID(), 1);
                    loopCnt = 0;
                }
                else {
                    loopCnt++;
                    vsleep (m_task.ID(), 0);
                }
            }
        }
        current++;
    }
    while ((e_ok == m_error) && (current != iterEnd));

    m_pagerController.AddDataIOTotal(totalIO);
    m_pagerController.AddDataPagesTotal(totalPages);
}

/*---------------------------------------------------------------------------*/

void Pager_Writer::FlushDataCacheAfterPrepareSequential() {

    SAPDBTRACE_ROUTINE_DEBUG("Pager_Writer::FlushDataCacheAfterPrepareSequential", Pager_Trace, 5 );

    SAPDB_Bool1             svpCompleted;
    SAPDB_Bool              stopLoop;
    SAPDB_Int2              iPageCnt;
    SAPDB_Int               loopCnt;
    SAPDB_Int               totalIO;
    SAPDB_Int               totalPages;
    tbd00_NodePtrList       nodePtrList;
    tbd2_data_cb_flush_list cbptrList;
    SAPDB_Int                dummy;
    tbd00_NodePtrList       normalNodePtrList;
    tbd00_NodePtrList       seqNodePtrList;
    tbd2_data_cb_flush_list normalCbptrList;
    tbd2_data_cb_flush_list seqCbptrList;

    PagerWriterIterator current = Begin();
    const PagerWriterIterator& iterEnd = End();

    m_error      = e_ok;
    totalPages = 0;
    totalIO    = 0;

    do {
        svpCompleted = SAPDB_FALSE;
        loopCnt      = 0;
        stopLoop = false;

        while ((e_ok == m_error) && !svpCompleted){

            iPageCnt = m_blockCount;
            bd20GetPageForIO (m_task.ID(), current.value(), m_error, iPageCnt,
                              nodePtrList, cbptrList, svpCompleted);

            if (0 == iPageCnt)
            {
                stopLoop = true;
            }

            SAPDB_Int normalIdx = 0;
            SAPDB_Int seqIdx = 0;
            for (SAPDB_Int i = 0; i < iPageCnt; i++)
            {
                Data_BasePage page(nodePtrList[i]);
                if (Data_VolumeSequential != page.VolumeMode())
                {
                    normalNodePtrList[normalIdx] = nodePtrList[i];
                    normalCbptrList[normalIdx]   = cbptrList[i];
                    normalIdx++;
                }
                else
                {
                    seqNodePtrList[seqIdx] = nodePtrList[i];
                    seqCbptrList[seqIdx]   = cbptrList[i];
                    seqIdx++;
                }
            }

            GetFreeBlocksAndWriteDataPages(normalNodePtrList, normalCbptrList,
                normalIdx, SAPDB_FALSE, totalIO, totalPages, dummy);

            if (e_ok == m_error){
                GetFreeBlocksAndWriteDataPages(seqNodePtrList, seqCbptrList,
                    seqIdx, SAPDB_TRUE, totalIO, totalPages, dummy);
            }

            if (e_ok == m_error){
                bd20ResetIOState (m_task.ID(), current.value(), iPageCnt, cbptrList);
            }

            if ((e_ok == m_error) && (!svpCompleted) && stopLoop)
            {
                if (loopCnt >= 5){
                    vsleep (m_task.ID(), 1);
                    loopCnt = 0;
                }
                else {
                    loopCnt++;
                    vsleep (m_task.ID(), 0);
                }
            }
        }
        current++;
    }
    while ((e_ok == m_error) && (current != iterEnd));

    m_pagerController.AddDataIOTotal(totalIO);
    m_pagerController.AddDataPagesTotal(totalPages);
}

/*---------------------------------------------------------------------------*/

void Pager_Writer::WriteDataPages(
    tbd00_NodePtrList       &pNodes,
    tbd2_data_cb_flush_list &cbptrList,
    SAPDB_Int               Offset,
    IOMan_ClusterAddress    Cluster) {

    SAPDBTRACE_ROUTINE_DEBUG("Pager_Writer::WriteDataPages", Pager_Trace, 5 );

    const IOMan_BlockCount BlockCount = Cluster.GetBlockCount();

    m_pages.Clear();

    Data_BasePage Page;
    for(SAPDB_Int Index = Offset ; Index < (BlockCount + Offset); ++Index )
    {
        Page.Assign( pNodes[ Index ], !gg04IsStaticPage( pNodes[ Index ]->nd_pt2() ),
                     pNodes[ Index ]->nd_pmode().includes( pmTemp_egg00 ),
                     cbptrList[ Index ]);

        m_pages.Append( Page );
    }
    IOMan_IDataIO::GetInstance().WriteDataPages( m_task.ID(), m_pages, Cluster );
}

/*---------------------------------------------------------------------------*/

void Pager_Writer::FlushConverter()
{
    SAPDBTRACE_ROUTINE_DEBUG("Pager_Writer::FlushConverter", Pager_Trace, 5 );

    const SAPDB_Int numPagerWriter = g01maxdatawriter();
    const SAPDB_Int numConverter   = g01region_cnt (rgnConvert_egg00);
	SAPDB_Int   section = m_myIndex;

	while(section < numConverter)
	{
		SAPDB_UInt  totalIO    = 0;
		SAPDB_UInt  totalPages = 0;

		Converter_IPageIO::Instance().WritePages( m_task, section, totalPages, totalIO );
		section += numPagerWriter;

		m_pagerController.AddConverterIOTotal( totalIO );
		m_pagerController.AddConverterPagesTotal( totalPages );
	}
}

/*---------------------------------------------------------------------------*/

void Pager_Writer::ReadConverter( const SAPDB_Bool bReadSnapShotConverter )
{
    SAPDBTRACE_ROUTINE_DEBUG("Pager_Writer::ReadConverter", Pager_Trace, 5 );

    SAPDB_UInt  totalIO    = 0;
    SAPDB_UInt  totalPages = 0;

    if( bReadSnapShotConverter )
        Converter_IPageIO::Instance().ReadSnapShotPages( m_task, m_myIndex, 
        totalPages, totalIO );
    else
        Converter_IPageIO::Instance().ReadPages( m_task,  m_myIndex, 
            totalPages, totalIO );

    m_pagerController.AddConverterIOTotal( totalIO );
    m_pagerController.AddConverterPagesTotal( totalPages );
}

/*===========================================================================*
*  END OF CODE                                                              *
*===========================================================================*/
