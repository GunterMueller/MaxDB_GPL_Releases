/*! @ingroup   KernelAdministration
    @file      Kernel_IAdminConfig.hpp
    @author    UweH
    @brief     This interface provides methods to handle kernel configuration.
*/
/*
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
#ifndef Kernel_IAdminConfig_H
#define Kernel_IAdminConfig_H

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "Logging/Log_Types.hpp"
#include "gsp00.h"

struct tgg00_TransContext;
class  Kernel_Migration;
class  SAPDBErr_MessageList;
class  Msg_List;
/// configuration interface

class Kernel_IAdminConfig
{
public:
    /*!
       @brief          Access the interface.
       @returns        reference to interface.
     */
    static Kernel_IAdminConfig& Instance();
    /*!
       @brief          The Data Access Manager is ready for activation.
       @param          trans [in/out] 
       @returns        none
     */
    virtual void InitConfig (tgg00_TransContext &trans) = 0;
    /// result code of Activate()
    enum ActivateResult
    {
        ActivateOk,
        ActivateNotAllowed,
        ActivateRestartInternFailed
    };
    /*!
       @brief          The Data Access Manager is ready for online mode.
       @param          taskid     [in] 
       @param          oldcontext [in/out] 
       @param          errlist    [out] 
       @returns        ActivateResult
     */
    virtual ActivateResult Activate (tsp00_TaskId          taskid,
                                     tgg00_TransContext   &oldcontext,
                                     SAPDBErr_MessageList &errlist) = 0;
    /*!
       @brief          The new volume is added to the set of data volumes.
       @param          trans [in/out] 
       @param          volumeName [in] 
       @param          newVolumeSize [in] 
       @param          volumeId [in] 
       @returns        none
     */
    virtual void AddDataVolume (tgg00_TransContext &trans,
                                tsp00_VFilename    &volumeName,
                                SAPDB_Int4          newVolumeSize,
                                SAPDB_Int2          volumeId) = 0;

    /*!
       @brief          drop a volume from the set of data volumes.
       @param          trans [in/out] 
       @param          volumeName [in] 
       @returns        none
     */
    virtual void DropDataVolume (tgg00_TransContext &trans,
                                SAPDB_Int2          volumeId) = 0;
    /*!
       @brief          The new volume is added to the set of log volumes.
                       The volume is formatted with NilLogPages.
       @param          trans [in/out] 
       @param          volumeName [in] 
       @param          mirrorVolumeName [in] 
       @param          newVolumeSize [in] 
       @param          volumeId [in] 
       @returns        none
     */
    virtual void AddLogVolume (tgg00_TransContext &trans,
                               tsp00_VFilename    &volumeName,
                               tsp00_VFilename    &mirrorVolumeName,
                               SAPDB_Int4          newVolumeSize,
                               SAPDB_Int2          volumeId) = 0;
    /*!
       @param          trans [in/out] 
       @param          devicename [in] 
       @returns        none

     */
    virtual void ReIntegrateBadLogVolume (tgg00_TransContext &trans,
                                          tsp00_VFilename    &devicename) = 0;
    /*!
       @param          trans [in/out] 
       @param          devicename [in] 
       @returns        none

     */
    virtual void GetBadLogVolume (tgg00_TransContext &trans,
                                  tsp00_VFilename    &devicename) = 0;
    /*!
       @brief          on any error the kernel is aborted
                       After this call the log history is lost.
       @param          taskid     [in] 
       @returns        none
     */
    virtual void ClearLog (tsp00_TaskId taskid) = 0;
    /*!
       @brief          If on is false log writing to the log volumes is disabled.
                       After this call the log history is lost.
       @param          taskid [in] 
       @param          on [in] 
       @returns        none
     */
    virtual void SetLogWriting (tsp00_TaskId taskid,
                                bool         on) = 0;
    /*!
       @brief          If on is true the log is automatically overwritten.
                       This disables the need for log backups. 
                       After this call the log history is lost.
       @param          taskid [in] 
       @param          on [in] 
       @returns        none
     */
    virtual void SetLogAutoOverwrite (tsp00_TaskId taskid,
                                      bool         on) = 0;

    /// This hold the log writer. On error false is returned.
    virtual bool SuspendLogWriter (tsp00_TaskId      taskid,
                                   Log_IOSequenceNo &lastWrittenIOsequence) = 0;
    /// This resumes the log writer.
    virtual void ResumeLogWriter (tsp00_TaskId taskid) = 0;

    /*!
       @brief   This method is used in state admin to create a snapshot
                of the converter, i.e. the restart record is copied and
                frozen as long as the snapshot is dropped or a recovery
                is executed. 
       @param   trans [in|out] transaction context
       @returns none
     */

    virtual void CreateSnapShot (tgg00_TransContext &trans) = 0;

    /*!
       @brief   This method is used in state admin to activate a frozen 
                converter. This means that the frozen restart record 
                is moved to the current restart record and the log 
                will be cleared. All operations made after the create
                snapshot are rejected.
       @param   trans [in|out] transaction context
       @returns none
     */

    virtual void ActivateSnapShot (tgg00_TransContext &trans) = 0;

    /*!
       @brief   This method is used in state admin to drop a snapshot
                of the converter, i.e. the root of the frozen restart
                record is removed from the current restart record.
       @param   trans [in|out] transaction context
       @returns none
     */

    virtual void DropSnapShot (tgg00_TransContext &trans) = 0;
    
    /// returns the MigrationHandler
    virtual Kernel_Migration& MigrationHandler() = 0;

    /*!
       @brief   This method is used to adjust the capacity of the data
                and log volumes stored in the IOManInfoPage's. 
       @param   trans   [in|out] transaction context
       @param   msgList [in|out] messageList
       @returns none
     */

    virtual void MigrateVolumes(
        tgg00_TransContext      &trans,
        SAPDBErr_MessageList    &msgList ) = 0;
};


/*===========================================================================*
 *  DEFINITION OF INLINE METHODS                                              *
 *===========================================================================*/

#endif  /* Kernel_IAdminConfig_H */
