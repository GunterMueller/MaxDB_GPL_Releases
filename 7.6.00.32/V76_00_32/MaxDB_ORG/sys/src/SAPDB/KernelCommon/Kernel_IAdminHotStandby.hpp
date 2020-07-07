/*!
    @ingroup   KernelAdministration
    @file      Kernel_IAdminHotStandby.hpp
    @author    UweH
    @brief     This interface provides methods to handle the hot standby configuration.

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
#ifndef Kernel_IAdminHotStandby_H
#define Kernel_IAdminHotStandby_H

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "KernelCommon/Kernel_DateTime.hpp"
#include "Logging/Log_Types.hpp"
#include "gsp00.h"

struct tgg00_TransContext;
class  SAPDBErr_MessageList;
class  SAPDBMem_IRawAllocator;

/// Hot Standby interface
class Kernel_IAdminHotStandby
{
public:
    /// returns the reference to interface.
    static Kernel_IAdminHotStandby& Instance();
	/// This uses the parameter information and stores the result in a state
    virtual void DetermineHotStandbyRole() = 0;
	/// returns true if the kernel is configured to be a standby node
    virtual bool RoleIsStandby() = 0;
	/// returns false on error alse the master node id from the log info page.
    virtual bool GetMaster( RTE_Nodename         &masterNodeName,
                            SAPDBErr_MessageList &errlist) = 0;
	/// result of InitStandby()
    enum InitStandbyResult
    {
        InitStandbyOk,
        InitStandbyWrongRole,
        InitStandbyNotAllowed,
        InitStandbyCannotReadMaster,
        InitStandbyCannotReadOwnName,
        InitStandbyCreateDataVolumesFailed,
        InitStandbyCommunicationError,
        InitStandbyEstablishMirrorFailed,
        InitStandbyFreeMemoryFailed
    };
    /*!
        @li    This is called by the cluster coordinator as a first step to create a new standby node.
        @li    The command \c INFO \c STANDBY is sent to the master to get an internal data packet which
               is used to establish the data mirror. If the communication fails the error
               \c InitStandbyCommunicationError is returned.
        @li    The command \c PREPARE \c STANDBY is sent to the master.
        @brief  This initializes the current node as a standby node.
        @param  taskid is used for synchronization.
        @param  errlist may be filled with any additional error message.
        @return \c InitStandbyResult
        @pre
        @li		The parameters must be set up for a standby
                configuration otherwise \c InitStandbyWrongRole is returned
        @li     If the log info page can be read and the current node is the master node
                or the current node is the default master then the error \c InitStandbyNotAllowed
                is returned.
    */
    virtual InitStandbyResult InitStandby (tsp00_TaskId          taskid,
                                           SAPDBErr_MessageList &errlist) = 0;
    /// result of PrepareStandby()
    enum PrepareStandbyResult
    {
        PrepareStandbyOk,
        PrepareStandbyWrongRole,
        PrepareStandbyAddNodeFailed,
        PrepareStandbySendInfoToStandbyFailed,
        PrepareStandbyStartSavepointFailed,
        PrepareStandbySplitMirrorFailed,
        PrepareStandbyFreeMemoryFailed
    };
    /*!
        @li     The given standby node is added to the internal node list.
        @li     \c INFO \c STANDBY is sent to the standby node to get info for the mirror.
        @li     A savepoint is started with reason \c SplitMirror.
        @brief  This prepares a new standby in the master.
        @param  taskid is used for synchronization.
        @param  savepointStartContext this is only used to start the savepoint.
        @param  errlist may be filled with any additional error message.
        @param  newStandbyNode is the name of the new standby node.
        @return \c PrepareStandbyResult
        @pre
        @li     The parameters must be set up for a standby configuration.
        @li     The role must be master.
    */
    virtual PrepareStandbyResult PrepareStandby (      tsp00_TaskId          taskid,
                                                       tgg00_TransContext   &savepointStartContext,
                                                       SAPDBErr_MessageList &errlist,
                                                 const SAPDB_Char           *newStandbyNode) = 0;
    /// result of RestartStandby()
    enum RestartStandbyResult
    {
        RestartStandbyOk,
        RestartStandbyWrongRole,
        RestartStandbyNotAllowed,
        RestartStandbyRestartPrepareFailed,
        RestartStandbyTriggerServerFailed
    };
    /*!
        A server task is started which is doing the restart for this standby.
        @brief  This initiates the standby role (permanent restart until TakeOver)
        @param  taskid is used for synchronization.
        @param  serverStartContext this is only used to start the restart server task.
        @param  errlist may be filled with any additional error message.
        @return \c RestartStandbyResult
        @pre
        @li     The parameters must be set up for a standby configuration.
        @li     The role must be standby.
        @li     The admin state must not be online or redo.
    */
    virtual RestartStandbyResult RestartStandby (tsp00_TaskId          taskid,
                                                 tgg00_TransContext   &serverStartContext,
                                                 SAPDBErr_MessageList &errlist) = 0;
    /// result of TakeOver()
    enum TakeOverResult
    {
        TakeOverOk,
        TakeOverWrongRole,
        TakeOverNotAllowed,
        TakeOverSetRoleToMasterFailed,
        TakeOverOpenLogAreaFailed
    };
    /*!
        @li     The redo reader is stopped so that the log volume can be closed and reopened
                for read and write access.
        @li     The current standby node becomes the master.
        @li     If date and time is set it is used like RestartUntil.
        @li     The caller is suspended until the restart server task has ended.
        @brief  This forces a node running in standby to become the new master.
        @param  taskid is used for synchronization.
        @param  date  for point in time restart
        @param  time  for point in time restart
        @param  errlist may be filled with any additional error message.
        @return \c TakeOverResult
        @pre
        @li     The parameters must be set up for a standby configuration.
        @li     The role must be standby.
        @li     The state must not be online but must be redo.
        @li     There must not exist another task which has triggered a takeover.
        @post   The node is online.
    */
    virtual TakeOverResult TakeOver (tsp00_TaskId          taskid,
                                     Kernel_Date           date,
                                     Kernel_Time           time,
                                     SAPDBErr_MessageList &errlist) = 0;
    /// result of RegisterStandby()
    enum RegisterStandbyResult
    {
        RegisterStandbyOk,
        RegisterStandbyWrongRole,
        RegisterStandbyNotAllowed,
        RegisterStandbyAddNodeFailed,
        RegisterStandbyPropagationFailed
    };
    /*!
        @li     The given standby node is added to the internal node list if not found there.
        @li     All other standbys are informed about the new standby node.
        @li     The new standby tells its oldest log position needed to restart.
        @li		The master tells the caller (the standby) the youngest log position which can be read.
        @brief  This is called on the master. If a standby is in standby mode.
        @param  taskid the callers task id
        @param  allocator is used to fill a local copy of the node list
        @param  standbyNodeId the name of the new standby node
        @param  firstNeededOffset is the log position the standby needs for restart
        @param  maxValidOffset this is the last readable offset for the standby node
        @param  errlist may be filled with any additional error message.
        @return \c RegisterStandbyResult
        @pre
        @li     The parameters must be set up for a standby configuration.
        @li     The role must be master.
        @li     The state must be online.
    */
	virtual RegisterStandbyResult RegisterStandby (      tsp00_TaskId            taskid,
                                                         SAPDBMem_IRawAllocator &allocator,
                                                   const SAPDB_Char             *standbyNodeId,
                                                         Log_RawDeviceOffset     firstNeededOffset,
                                                         Log_RawDeviceOffset    &maxValidOffset,
                                                         SAPDBErr_MessageList   &errlist ) = 0;
    /// result of InsertStandby()
    enum InsertStandbyResult
    {
        InsertStandbyOk,
        InsertStandbyWrongRole,
        InsertStandbyAddNodeFailed,
        InsertStandbyNotAllowed //PTS 1125942 mb 2003-02-12
    };
    /*!
        @li     This is called by the master to propagate a new standby node.
        @li     The new node is added to the internal node list.
        @brief  A new standby has registered to the master and this is called at every standby
        @param  taskid is used for synchronization.
        @param  standbyNodeId is the name of the new standby node
        @param  errlist may be filled with any additional error message.
        @return \c InsertStandbyResult
        @pre    The parameters must be set up for a standby configuration otherwise
                \c InsertStandbyWrongRole is returned
    */
    virtual InsertStandbyResult InsertStandby (      tsp00_TaskId          taskid,
                                               const SAPDB_Char           *standbyNodeId,
                                                     SAPDBErr_MessageList &errlist) = 0;
    /// result of RemoveStandby()
    enum RemoveStandbyResult
    {
        RemoveStandbyOk,
        RemoveStandbyWrongRole,
        RemoveStandbyNodeNotFound,
        RemoveStandbyNotAllowed //PTS 1125942 mb 2003-02-12
    };
    /*!
        @li     This is called by the master to propagate the a standby node no longer is enabled.
        @li     The node name is removed from the internal node list.
        @brief  A standby was removed by the master and this is called at every standby
        @param  taskid is used for synchronization.
        @param  standbyNodeId is the name of the new standby node
        @param  errlist may be filled with any additional error message.
        @return \c RemoveStandbyResult
        @pre    The parameters must be set up for a standby configuration otherwise
                \c RemoveStandbyWrongRole is returned
    */
    virtual RemoveStandbyResult RemoveStandby (      tsp00_TaskId          taskid,
                                               const SAPDB_Char           *standbyNodeId,
                                                     SAPDBErr_MessageList &errlist) = 0;
    /// result of Synchronize()
    enum SynchronizeResult
    {
        SynchronizeOk,
        SynchronizeWrongRole,
        SynchronizeNotAllowed
    };
    /*!
        @brief  This is called on the standby side to synch with master.
        @param  taskid is used for synchronization.
        @param  newMaxOffset is given by the master.
        @param  lastRedoOffset is set by this standby and ist used by the master for FreeLog()
        @param  errlist may be filled with any additional error message.
        @return \c SynchronizeResult
        @pre    The parameters must be set up for a standby configuration.
        @pre    The role must be standby.
    */
	virtual SynchronizeResult Synchronize ( tsp00_TaskId          taskid,
                                            Log_RawDeviceOffset   newMaxOffset,
                                            Log_RawDeviceOffset  &lastRedoOffset,
                                            SAPDBErr_MessageList &errlist ) = 0;
    /// result of TriggerSynchronize()
    enum TriggerSynchronizeResult
    {
        TriggerSynchronizeOk,
        TriggerSynchronizeNotNeeded,
        TriggerSynchronizeStartSyncServerFailed
    };
    /*!
        @brief  The timeout task uses this to start the synchronization server task
        @param  serverTaskStartContext is used to start the synch server task
        @return \c TriggerSynchronizeResult
    */
    virtual TriggerSynchronizeResult TriggerSynchronize (tgg00_TransContext &serverTaskStartContext) = 0;
    /*!
        Sends to all known standbys the next maximum offset until they can continue their recovery.
    	The oldest used offset is returned and must be used to free the log.
        @brief  This is the synchronize server task code which executes the sync for all standbys
    */
    virtual void SynchronizeServer ( tsp00_TaskId            taskid,
                                     SAPDBMem_IRawAllocator &allocator) = 0;

    /*!
        Send a new entry in the backup history (dbm.knl) to all known standbys.
        If operation returns an error for a standby, this standby will be removed.
        (New with PTS 1129960)
    */
    virtual void BroadcastBackupHistoryEntry (
                            tsp00_TaskId            taskid,
                            const char * line,
                            const SAPDB_UInt lineLength) = 0;
    
    /*!
        appends one entry to the backup history (dbm.knl).
        (New with PTS 1129960)
    */
    // virtual void AppendBackupHistoryEntry () = 0;
    /*!
        Send a new entry in the backup medium definition file (dbm.mdf) to all known standbys.
        If operation returns an error for a standby, this standby will be removed.
        (New with PTS 1129960)
    */
    virtual void BroadcastBackupMediumDefEntry (
                            tsp00_TaskId            taskid,
                            const char * line,
                            const SAPDB_UInt lineLength) = 0;
    
    /*!
        appends one entry to the backup medium definition file (dbm.mdf).
        (New with PTS 1129960)
    */
    // virtual void AppendBackupMediumDefEntry () = 0;

    /*!
        @li     On any error the kernel will be aborted.
        @li     At the end the task which called \c TakeOver() is resumed.
        @brief  This is the server task which is doing the restart of a standby.
    */
	virtual bool RestartStandbyServer (tsp00_TaskId          taskid,
                                       tgg00_TransContext   &oldcontext,
                                       SAPDBErr_MessageList &errlist) = 0;
    /// The Redo Iterator must get the delay time stamp if one is defined.
    virtual void GetDelayTimeStamp (Kernel_Date &date,
                                    Kernel_Time &time) = 0;
};
#endif  /* Kernel_IAdminHotStandby_H */