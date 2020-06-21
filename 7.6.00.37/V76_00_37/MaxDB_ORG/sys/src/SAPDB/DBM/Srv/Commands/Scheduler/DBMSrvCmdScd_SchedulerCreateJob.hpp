/*!
  \file    DBMSrvCmdScd_SchedulerCreateJob.hpp
  \author  MarcW
  \ingroup DBMServer commands
  \brief   schedule a job

\if EMIT_LICENCE

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


\endif
*/

#include "DBM/Srv/Commands/Scheduler/DBMSrvCmdScd_SchedulerCommand.hpp"
#include "ToolsCommon/Tools_DynamicUTF8String.hpp"

#if !defined(DBMSrvCmdScd_SchedulerCreateJob_hpp)
#define DBMSrvCmdScd_SchedulerCreateJob_hpp


/*! 
    @brief Class for handling the DBMServer command scheduler_schedule
*/
class DBMSrvCmdScd_SchedulerCreateJob: public DBMSrvCmdScd_SchedulerCommand {
public:
    /*! @brief constructor */
    DBMSrvCmdScd_SchedulerCreateJob();

    /*!
        @brief implementation of the lock protected run method as required by base class.

        @param vcontrol [INOUT] the dbmserver object
        @param command [IN] command line
        @param theSchedReply [OUT] the object to write command response into
        @return execution result status
    */
    tcn00_Error runLockProtected(VControlDataT * vcontrol,
        CommandT      * command,
        DBMSrvCmdScd_SchedulerReply& theSchedReply);

private:
    static const char* const m_TextOnce;
    static const char* const m_TextO;

    static const char* const m_SchedulerStartCommandTemplate;
};
#endif //DBMSrvCmdScd_SchedulerCreateJob
