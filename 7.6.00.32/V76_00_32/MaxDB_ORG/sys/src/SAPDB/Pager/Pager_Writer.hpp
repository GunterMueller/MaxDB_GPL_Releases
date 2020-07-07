/*!
  @file           Pager_Writer.hpp
  @author         Henrik
  @brief          Pager IO
  @see            


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



*/



#ifndef PAGER_WRITER_HPP
#define PAGER_WRITER_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "ggg00.h"
#include "gbd00.h"  // tbd00_NodePtrList 
#include "gbd02.h"
#include "hgg01.h" 

#include "KernelCommon/Kernel_Common.hpp"
#include "Container/Container_Vector.hpp"

#include "RunTime/MemoryManagement/RTEMem_Allocator.hpp"
#include "RunTime/Tasking/RTETask_ITask.hpp"

#include "IOManager/IOMan_ClusterAddress.hpp"
#include "IOManager/IOMan_DataPages.hpp"

#include "Transaction/Trans_Context.hpp"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/
class Pager_Controller;

/*!
  @class          Pager_Writer
  @brief          Pager IO
*/
class Pager_Writer {

public:

    /*!
       @brief   Constructor
       @param   t global transaction context
    */
    Pager_Writer( Trans_Context &trans );

    /// initialize pager object
    void Initialize();

    /*!
       @brief  returns taskId of current pager task
       @return RTE_TaskId
    */
    RTE_TaskId GetTaskId() const{
        return m_task.ID();
    }


private:

    const SAPDB_Int      m_blockCount;

    Trans_Context       &m_trans;
    SAPDB_Int            m_myIndex;
    tgg00_BasisError    &m_error;
    RTETask_ITask       &m_task;
    Pager_Controller    &m_pagerController;
    IOMan_DataPages      m_pages;     // container used by WriteDataPages - PTS 1117889

    void Run();

	/// displace temp pages in the rechain area of the LRU chain
    void FlushTempPagesFromLRURechain();
	
    void FlushDataCacheBeforePrepare ();

    void FlushClusteredFiles ();

    void FlushDataCacheBeforePrepareSequential ();

    void FlushDataCacheAfterPrepare() ;

    void FlushDataCacheAfterPrepareSequential();

    void WriteDataPages(
        tbd00_NodePtrList       &pNodes,
        tbd2_data_cb_flush_list &cbptrList,
        SAPDB_Int               Offset,
        IOMan_ClusterAddress    Cluster);

    void GetFreeBlocksAndWriteDataPages(
        tbd00_NodePtrList       &pNodes,
        tbd2_data_cb_flush_list &cbptrList,
        SAPDB_Int               numBlocks,
        SAPDB_Bool1             isSequential,
        SAPDB_Int               &totalIO,
        SAPDB_Int               &totalPages,
        SAPDB_Int               &totalIOElems);

    void GetFreeBlocksAndWriteDataPages(
        tbd00_NodePtrList       &pNodes,
        tbd2_data_cb_flush_list &cbptrList,
        SAPDB_Int               numBlocks,
        SAPDB_Bool1             isSequential)
    {
        SAPDB_Int stats;

        GetFreeBlocksAndWriteDataPages( pNodes, cbptrList,
            numBlocks, isSequential, stats, stats, stats);
    }

    void FlushConverter();

    void ReadConverter( const SAPDB_Bool bReadSnapShotConverter );

    class PagerWriterIterator; // Declaration required
    friend class PagerWriterIterator; // Make it a friend

    class PagerWriterIterator {
        SAPDB_Int m_pos;
        SAPDB_Int m_nextCache;
        SAPDB_Int m_endCache;

    public:

        PagerWriterIterator(const Pager_Writer& pager) {
            Initialize(pager);
        }

        PagerWriterIterator(const Pager_Writer& pager, SAPDB_Bool) {
            Initialize(pager);
            m_pos = m_endCache;
        }

        SAPDB_Int operator++(int) { // Postfix form
            SAPDB_Int ret = m_pos;
            m_pos += m_nextCache;
            return ret;
        }

        SAPDB_Int value() const {
            return m_pos;
        }

        PagerWriterIterator& operator=(const PagerWriterIterator& right) {   // Handle self-assignment:
            if(this == &right){
                return *this;
            }
            m_pos = right.m_pos;
            m_nextCache = right.m_nextCache;
            m_endCache = right.m_endCache;
            return *this;
        }


        SAPDB_Bool operator==(const PagerWriterIterator& other) const {
            return m_pos == other.m_pos;
        }

        SAPDB_Bool operator!=(const PagerWriterIterator& other) const {
            return m_pos != other.m_pos;
        }

    private:

        void Initialize(const Pager_Writer& pager) {
            const SAPDB_Int numPagerWriter = g01maxdatawriter();
            const SAPDB_Int numCaches = g01region_cnt (rgnData_egg00);

            if (numPagerWriter < numCaches){
                m_pos = pager.m_myIndex;
                m_nextCache  = numPagerWriter;
            } else {
                m_pos = pager.m_myIndex % numCaches;
                m_nextCache  = numCaches;
            }

            m_endCache = m_pos;
            while (m_endCache < numCaches) {
                m_endCache += m_nextCache;
            }
        }
    };

    PagerWriterIterator Begin() const {
        return PagerWriterIterator(*this);
    }

    PagerWriterIterator End() const {
        return PagerWriterIterator(*this,true);
    }
};

#endif  /* PAGER_WRITER_HPP */
