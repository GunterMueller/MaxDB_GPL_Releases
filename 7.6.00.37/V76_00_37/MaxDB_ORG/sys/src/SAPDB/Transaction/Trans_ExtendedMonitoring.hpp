/*!
  @file           Trans_ExtendedMonitoring.hpp
  @author         FerdiF
  @ingroup        Transaction
  @since          2004-18-19

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
#ifndef Trans_ExtendedMonitoring_HPP
#define Trans_ExtendedMonitoring_HPP

//--------------------------------------------------------------------------------------
#include "ggg00.h"
#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "Transaction/Trans_ExtendedMonitoringContext.hpp"
//--------------------------------------------------------------------------------------

///
class RTESync_IRWRegion;
class RTESync_SpinlockPool;

/*!
   @class    Trans_ExtendedMonitoring
   @brief    This class is used to handle monitoring information needed
             for systemview 'TRANSACTIONHISTORY'.
             The transaction comment string is filled using the OMS API Method
             OmsSetTransactionComment.
             Only Transactions using 'consistent read' isolation will be monitored.
 */
class Trans_ExtendedMonitoring 
{
private:
    /*!
       @brief   constructs memory structure used for monitoring 
     */ 
    Trans_ExtendedMonitoring(
        SAPDBMem_IRawAllocator& alloc, 
        SAPDB_UInt4             size);

    /*!
       @brief   moves monitoring information on transaction end into the wrap around history buffer
     */ 
    Trans_ExtendedMonitoringContext  ReplaceOldest(tsp00_TaskId  TaskId, Trans_ExtendedMonitoringContext &newCtx);

    void GetFirstCtx(
        tsp00_TaskId                     TaskId, 
        Trans_ExtendedMonitoringContext &ctx, 
        SAPDB_UInt4                     &pos);
    
    void GetNextCtx(
        tsp00_TaskId                     TaskId, 
        Trans_ExtendedMonitoringContext &ctx, 
        SAPDB_UInt4                     &pos);
public:
    /*!
       @brief   called every time after initializing a new session and transaction
     */ 
    static void StartTransaction(
        tgg00_TransContext&   Trans, 
        Trans_ExtendedMonitoringContext*& ExtMon_ContextPtr);

    /*!
       @brief   called every time a transaction terminates
     */ 
    static void EndTransaction(
        tsp00_TaskId     TaskId, 
        tgg00_TransIndex TransIndex, 
        SAPDB_Bool       committed);

    /*!
       @brief   called every time after a transaction terminated and a new transaction 
                is started implicitly on the same session.
     */ 
    static void RestartTransaction(Trans_ExtendedMonitoringContext*& ExtMon_ContextPtr);
    
    /*!
       @brief   used to initialize create and initialize monitoring data structure 
     */ 
    static void InitializeMonitoring();
    
    /*!
       @brief   stores a comment string for the actual running transaction 
     */ 
    static void SetTransactionComment(
        tsp00_TaskId     TaskId, 
        tgg00_TransIndex TransIndex, 
        tsp00_Int2       commentLength, 
        SAPDB_Char*      pComment);

    /*!
       @brief   used by Trans_ExtendedMonitoringIter to start iteration 
                get oldest transaction history information entry
     */ 
    static Trans_ExtendedMonitoringContext* GetFirstHistoryContext(
        tsp00_TaskId                     TaskId,
        Trans_ExtendedMonitoringContext &ctx, 
        SAPDB_UInt4                     &pos);

    /*!
       @brief   used by Trans_ExtendedMonitoringIter  
                get next transaction history information entry
     */ 
    static Trans_ExtendedMonitoringContext* GetNextHistoryContext(
        tsp00_TaskId                     TaskId,
        Trans_ExtendedMonitoringContext &ctx, 
        SAPDB_UInt4                     &pos);

    /*!
       @brief   return allocator used by Trans_ExtendedMonitoring
     */ 
    static  SAPDBMem_IRawAllocator& GetAllocator();

private:
    SAPDBMem_IRawAllocator& m_alloc;
    Trans_ExtendedMonitoringContext*    m_historyList;
    RTESync_SpinlockPool   *m_SpinLockPool;
    RTESync_IRWRegion      *m_IRWLockRegion;
    SAPDB_UInt4             m_size;
    SAPDB_UInt4             m_next;
    bool                    m_full;
};
#endif // Trans_ExtendedMonitoring_HPP
