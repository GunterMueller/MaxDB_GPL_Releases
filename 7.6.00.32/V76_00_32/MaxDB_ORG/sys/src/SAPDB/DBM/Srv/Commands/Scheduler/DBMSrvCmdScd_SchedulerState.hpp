/*!
  \file    DBMSrvCmdScd_SchedulerState.hpp
  \author  MarcW
  \ingroup DBMServer commands
  \brief   return the state of the scheduler

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

#if !defined(DBMSrvCmdScd_SchedulerState_hpp)
#define DBMSrvCmdScd_SchedulerState_hpp

/*! 
    @brief class for handling the DBMServer command scheduler_list
*/
class DBMSrvCmdScd_SchedulerState: public DBMSrvCmdScd_SchedulerCommand {
public:
    /*! @brief constructor */
    DBMSrvCmdScd_SchedulerState();

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

    /*! @brief call only from a section in code that is lock protected

        Otherwise exclusive access cannot be guaranteed.
        @return true if scheduler is up, false otherwise
    */
    bool isSchedulerUp(DBMSrvMsg_Error& errOut) const;

private:
    bool getSchedulerPid(RTE_OSPid& aPid, DBMSrvMsg_Error& errOut) const;

    static const char* const m_SchedulerStateText[4];
};
#endif //DBMSrvCmdScd_SchedulerState_hpp
