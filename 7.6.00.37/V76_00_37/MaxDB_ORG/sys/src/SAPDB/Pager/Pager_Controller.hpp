/*!
  @file           Pager_Controller.hpp
  @author         Henrik
  @brief          Pager instantiation and savepoint control
  @see            


    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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



#ifndef PAGER_CONTROLLER_HPP
#define PAGER_CONTROLLER_HPP

/*!
  @class          Pager_Controller
  @brief          Pager instantiation and savepoint control.
                  Pager_Controller creates Pager_Writer instances and triggers
                  startup of Pager_Writers
 */
class Pager_Controller;
class RTETask_ITask;
class Msg_List;

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "gbd00.h"
#include "gbd02.h"
#include "gbd201.h"


#include "KernelCommon/Kernel_Common.hpp"
#include "RunTime/Synchronisation/RTESync_NamedSpinlock.hpp"
#include "RunTime/Synchronisation/RTESync_InterlockedCounter.hpp"
#include "Container/Container_Vector.hpp"

#include "Pager/Pager_IRestart.hpp"
#include "Pager/Pager_ISavepoint.hpp"
#include "Pager/Pager_IDump.hpp"
#include "Pager/Pager_Dump.hpp"
#include "Pager/Pager_Writer.hpp"
#include "Pager/Pager_Types.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

class Kernel_Dump;

class Pager_Controller : public Pager_ISavepoint,
    public Pager_IDump, public Pager_IRestart {
public:
    // singleton
    /*!
       @brief          returns instance of Pager_Controller.
       @return         (new) singleton instance of Pager_Controller

       If no instance exists a new one will be created
     */

    static Pager_Controller& GetInstance();

    /*!
       @brief          initiate Savepoint
       @param          taskId [in] own taskID
       @return         none

       wait for running pager_writer to finish
     */

    virtual void BeginSavepoint (const tsp00_TaskId taskId);

    /*!
       @brief          flushes DataCache to disk
       @param          taskId [in] own taskID
       @param          numPagesWritten [out] number of written pages
       @param          numIOActions [out] number of clustered I/O transactions
       @return         none

     */

    virtual void FlushDataCacheBeforePrepare (const tsp00_TaskId taskId,
            SAPDB_UInt &numPagesWritten,
            SAPDB_UInt &numIOActions);

    /*!
       @brief          loops as long as the last operation which is synchronized
                       with the savepoint has finished.
       @param          taskId           [in] own taskID
       @param          numPagesWritten [out] number of written pages
       @param          numIOActions    [out] number of clustered I/O transactions
       @return         none

     */

    virtual void WaitForLastSvpSyncOperation (
        const tsp00_TaskId taskId,
        SAPDB_UInt &numPagesWritten,
        SAPDB_UInt &numIOActions);

    /*!
       @brief          flushes DataCache to disk
       @param          taskId [in] own taskID
       @param          numPagesWritten [out] number of written pages
       @param          numIOActions [out] number of clustered I/O transactions
       @return         none

     */

    virtual void FlushDataCacheAfterPrepare (const tsp00_TaskId taskId,
            SAPDB_UInt &numPagesWritten,
            SAPDB_UInt &numIOActions);

    /*!
       @brief          Savepoint finished,
       @param          taskId [in] own taskID
       @return         none

     */

    virtual void EndSavepoint (const tsp00_TaskId taskId);

	/*!
	   @brief          Savepoint finished, disable Pager_Writer
	   @param          taskId [in] own taskID
	   @return         none

	 */

	virtual void EndSavepointForShutdown (const tsp00_TaskId taskId);
	
    /*!
       @brief          Dump info
       @param          taskId [in] taskID
       @param          dump [out] dump structure
       @return         none

     */

    virtual void Dump(const tsp00_TaskId    taskId,
                      Kernel_Dump           &dump );

    /*!
       @brief          Pager_Writer registers itself with the Pager_Controller
       @param          taskId [in] own taskID
       @param          writer [in] Pager_Writer
       @return         Pager_Writer ID


         the controller assigns a unique ID to each writer object

     */

    SAPDB_Int RegisterNextPagerWriter(const tsp00_TaskId taskId,
                                      Pager_Writer       *writer);

    /*!
       @brief          Read converter from data area for restart
       @param          taskId                 [in] own taskID
       @param          bReadSnapShotConverter [in] the converter to be read
                                                   is the snapshot converter
                                                   (Default = SAPDB_FALSE )
       @return         none

     */

    void ReadConverter(
        const tsp00_TaskId    taskId,
        const SAPDB_Bool      bReadSnapShotConverter = SAPDB_FALSE);

    /*!
       @brief         add number of i/o transactions for data pages to total counter
       @param         io [IN] number of i/o transactions
    */
    void AddDataIOTotal(const SAPDB_Int io)
    {
        SAPDBERR_ASSERT_ARGUMENT(io >= 0);
        m_dataIOTotal.Increment(io);
    }

    /*!
       @brief         add number of data pages written to total counter
       @param         pages [IN] number of data pages written
    */
    void AddDataPagesTotal(const SAPDB_Int pages)
    {
        SAPDBERR_ASSERT_ARGUMENT(pages >= 0);
        m_dataPagesTotal.Increment(pages);
    }

    /*!
       @brief         add number of i/o transactions for converter pages to total counter
       @param         io [IN] number of i/o transactions
    */
    void AddConverterIOTotal(const SAPDB_Int io)
    {
        SAPDBERR_ASSERT_ARGUMENT(io >= 0);
        m_convIOTotal.Increment(io);
    }

    /*!
       @brief         add number of converter pages written to total counter
       @param         pages [IN] number of converter pages written
    */
    void AddConverterPagesTotal(const SAPDB_Int pages)
    {
        SAPDBERR_ASSERT_ARGUMENT(pages >= 0);
        m_convPagesTotal.Increment(pages);
    }

    /*!
       @brief         suspend one pager writer
       @param         pagerIndex [IN] id of pager writer
       @param         taskId [IN] taskId of pager writer
    */
    void SuspendPagerWriter(
        SAPDB_UInt      pagerIndex,
        tsp00_TaskId    taskId);

    /// query prepare savepoint state
    SAPDB_Bool IsPreparePagerWriter() const
    {
        return m_preparePagerWriter;
    }

    /*!
       @brief   query current pager order
       @return  pager order
    */
    ePagerOrder GetPagerOrder() const
    {
        return m_pagerOrder;
    }
	
	SAPDB_Int GetClusterWriteThreshold() const
	{
		return m_clusterWriteThreshold;
	}
	
	bd201_Clusterer &GetClusterer()
	{
			return m_clusterer;
	}

    /// enable pager_writer
    void EnablePagerWriter ();

    /*!
       @brief          flush one region of the data cache, unclustered
       @param          taskId    [in] own taskID
       @param          regOffset [in] region number
       @return         none
     */
    void FlushDataCacheRegion (
        const tsp00_TaskId taskId,
        const SAPDB_Int   regOffset);

    /*!
       @brief          flush all regions of the data cache, clustered pages
       @param          taskId    [in] own taskID
       @return         none

    flushes all pages listed in the ioQueue
     */
    void FlushDataCache (
        const tsp00_TaskId taskId);

    /*!
       @brief          flush temp pages from the LRU rechain area of all data cache regions, unclustered
       @param          taskId    [in] own taskID
       @return         none
     */
    void FlushDataCacheWithTempFromLRURechain(
        const tsp00_TaskId taskId);

    /*!
       @brief          triggers parallel flush of converter segments
       @param          taskId           [in] own taskID
       @param          numPagesWritten [out] number of written pages
       @param          numIOActions    [out] number of clustered I/O transactions
       @return         none
     */
    void FlushConverterParallel (
        const tsp00_TaskId  taskId,
        SAPDB_UInt          &numPagesWritten,
        SAPDB_UInt          &numIOActions);

#ifdef CONSTRUCTOR_MAY_NOT_BE_PRIVATE_AND_NO_FRIEND_DEFINED
protected:
#else
private:
#endif

    Pager_Controller(SAPDBMem_IRawAllocator &allocator);

private:

    /// const value for FBM checking
    #define SET_CHECK_FBM  SAPDB_TRUE

    // copy and assignment operators are not supported
    Pager_Controller& operator=( const Pager_Controller &controller);

    Pager_Controller(const Pager_Controller &controller);


    static void CreateInstance();

    void Initialize();
	
	SAPDB_Bool RegisterParameters (SAPDBErr_MessageList  & msgList);

    void ResumeAllPagerWriter (
        const tsp00_TaskId  taskId,
        const SAPDB_Bool1   set_check_fbm,
        const SAPDB_Bool    resumeForConverter);

    void CleanupActiveCaches();

    void ClearConverterCache (
        tgg00_TransContext &trans,
        SAPDB_Int          &total_io,
        SAPDB_Int          &total_pages);

    void WaitForPagerWriterReply (const tsp00_TaskId taskId);

    void WriteDataPage (
        const tsp00_TaskId    taskId,
        tbd_nodeptr           pNode,
        tbd02_pDataCBlock     pCBlock ) const;

	void EndSavepointUnSynch (const tsp00_TaskId taskId);
	
	static Pager_Controller  *m_Instance;

    RTESync_NamedSpinlock            m_SpinLock;
    Container_Vector<Pager_Writer *> m_pagerWriter;
    Container_Vector<SAPDB_Bool>     m_activePagerWriter;
    const SAPDB_UInt                 m_numPagerWriter;
    SAPDB_UInt                       m_registeredPagerWriter;
	SAPDB_Int                        m_clusterWriteThreshold;
    tsp00_TaskId                     m_svpCoordinator;
    tsp00_TaskId                     m_waitingForFlush;
    SAPDB_Bool                       m_preparePagerWriter;
    SAPDB_Bool                       m_svpActive;
    SAPDB_Bool                       m_flushActive;
    SAPDB_Bool                       m_pagerWriterDisabled;
    ePagerOrder                      m_pagerOrder;
    eClusteredIO                     m_isClusteredIO;
	bd201_Clusterer                  m_clusterer;

    RTESync_InterlockedCounter<SAPDB_UInt> m_suspendCnt;
    RTESync_InterlockedCounter<SAPDB_UInt> m_dataIOTotal;
    RTESync_InterlockedCounter<SAPDB_UInt> m_dataPagesTotal;
    RTESync_InterlockedCounter<SAPDB_UInt> m_convIOTotal;
    RTESync_InterlockedCounter<SAPDB_UInt> m_convPagesTotal;
};

#endif  /* PAGER_CONTROLLER_HPP */


