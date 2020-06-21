/*!	
    @ingroup   KernelAdministration
    @file      Kernel_IAdminRestartShutdown.hpp
    @author    UweH
    @brief     This interface provides methods to transfer the kernel from admin mode to online
               mode and to switch off the kernel to offline mode.

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
#ifndef Kernel_IAdminRestartShutdown_H
#define Kernel_IAdminRestartShutdown_H

#include "KernelCommon/Kernel_DateTime.hpp"
#include "Logging/Log_Types.hpp"

#include "gsp00.h"
#include "ggg00.h"

class  SAPDBErr_MessageList;
class  Rst_RedoManager;

/// This interface lets change the kernels running mode.
class Kernel_IAdminRestartShutdown
{
public:
	/// Access the singleton. Returns reference to singleton.
    static Kernel_IAdminRestartShutdown& Instance();
    ///
    enum RestartResult
    {
        RestartOk,
        RestartNotAllowed,
        RestartRestartInternFailed
    };
    /// The Data Access Manager is ready for online mode.
    virtual RestartResult Restart (tsp00_TaskId          taskid,
                                   tgg00_TransContext   &oldcontext,
                                   Kernel_Date           untildate,
                                   Kernel_Time           untiltime,
                                   Log_IOSequenceNo      untilIOSequence,
                                   SAPDBErr_MessageList &errlist) = 0;
	/// After calling the kernel is offline.
	/// If the autosave log is in standby an error is set.
    virtual void Shutdown (tgg00_TransContext &trans) = 0;
	/// After that the kernel is offline. The given Error Number is printed out.
    virtual void Offline ( const tgg00_BasisError error,
                                 bool             forceBacktrace = false,
                                 bool             writeCore      = false ) = 0;
	/// After that the kernel is offline. The given errorText is printed.
    virtual void Offline ( const char * errorText = 0,
                                 bool   forceBacktrace = false,
                                 bool   writeCore      = false ) = 0;
	/// After that the kernel is offline. The given ErrorList is printed in Textform.
    virtual void Offline ( const SAPDBErr_MessageList &errlist,
                                 bool                  forceBacktrace = false,
                                 bool                  writeCore      = false ) = 0;
	/// This sets the database in online mode. Now users can connect.
    virtual void SetKernelStateOnline() = 0;
    /// returns the redo manager for direct manipulation or access
    virtual Rst_RedoManager& GetRedoManager() = 0;
};
#endif  /* Kernel_IAdminRestartShutdown_H */
