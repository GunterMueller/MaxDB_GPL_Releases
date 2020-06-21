/*! @ingroup   KernelAdministration
    @file      Kernel_IAdminDiagnose.hpp
    @author    UweH
    @brief     This interface provides methods to manipulate kernel data for support.
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
#ifndef Kernel_IAdminDiagnose_H
#define Kernel_IAdminDiagnose_H

#include "gsp00.h"
#include "SAPDBCommon/SAPDB_Types.hpp"
#include "DataAccess/Data_Types.hpp"
class IOMan_BlockAddress;
class RTETask_ITask;
/*!
    @class Kernel_IAdminDiagnose
    @brief This is the interface for all diagnose functions of the kernel.
 */
class Kernel_IAdminDiagnose
{
public:
	/// Access the interface.
    static Kernel_IAdminDiagnose& Instance();
    /// This attaches all data volumes, calls the converter function and detaches all volumes.
    virtual void GetBlockAddress ( 
        const RTETask_ITask         &task,
        const Data_PageNo           &pageno,
        const Data_PageRecoveryMode &recMode,
        IOMan_BlockAddress          &block ) = 0;
    /// This attaches all data volumes, calls the converter function and detaches all volumes.
    virtual bool SetNewBlockAddress ( 
        const RTETask_ITask         &task,
        const Data_PageNo           &pageno,
        const Data_PageRecoveryMode &recMode,
        const IOMan_BlockAddress    &block ) = 0;
    /// This attaches all data volumes, calls the converter function and detaches all volumes.
    virtual bool DeleteBlockAddress( 
        const RTETask_ITask         &task,
        const Data_PageNo           &pageno,
        const Data_PageRecoveryMode &recMode ) = 0;
	/// This is used to access the restartrecord. k57restartrec is used.
    virtual bool ReadRestartRecord ( tsp00_TaskId taskid) = 0;
	/// This writes k57restartrec to disc
    virtual bool FlushRestartRecord ( tsp00_TaskId taskid) = 0;

    /// Checks the data consistence in state online
    virtual void CheckData( 
        tgg00_TransContext  &trans,
        const SAPDB_Bool    bWithExtendedCheck,
        const SAPDB_Bool    bWithIndexes,
        const SAPDB_Bool    bWithDeleteUnreferencedBlobs, // PTS 1133300 UH 2005-01-11
        SAPDB_Int4          &badIndexCount,
        SAPDB_Int4          &totalBadCount ) = 0;

    /// Checks the data consistence in state admin
    virtual void CheckDataWithUpdate( 
        tgg00_TransContext  &trans,
        const SAPDB_Bool    bWithExtendedCheck,
        SAPDB_Int4          &pagesUsed,
        SAPDB_Int4          &blocksReleased,
        SAPDB_Int4          &badIndexCount,
        SAPDB_Int4          &totalBadCount ) = 0;

    /// Diagnose repair commands
    virtual void Diagnose( tgg00_MessBlock    &messBlock ) = 0;

};
#endif  /* Kernel_IAdminDiagnose_H */
