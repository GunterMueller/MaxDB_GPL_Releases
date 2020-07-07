/*!
  \file    DBMSrvCmdScd_SchedulerCommand.hpp
  \author  MarcW
  \ingroup DBMServer commands
  \brief   base class for scheduler commands, implements no command.
           Subclass of DBMSrv_Reply with special abilities.

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

#if !defined(DBMSrvCmdScd_SchedulerCommand_hpp)
#define DBMSrvCmdScd_SchedulerCommand_hpp

#include "DBM/Srv/DBMSrv_Command.hpp"
#include "DBM/Srv/SharedMemory/DBMSrvShM_LineAccessorScheduler.hpp"
#include "ToolsCommon/Tools_DynamicUTF8String.hpp"

/*!
    @brief reply class with special abilities

    An object of this class is created from a DBMSrv_Reply. The new object accesses
    the same buffer as the original object. So the  original reply object should
    not be used until the new one is deleted.
*/
class DBMSrvCmdScd_SchedulerReply : public DBMSrv_Reply {
public:
    /*!
        @brief constructor

        The new object assess the same buffer as the original object.
        @param aReply [IN] original reply object
    */
    DBMSrvCmdScd_SchedulerReply( DBMSrv_Reply& aReply )
        : m_TheReply(aReply),
          DBMSrv_Reply(aReply.giveData(), aReply.giveLength(), aReply.giveMaximalLength()) {}

    /*!
        @brief destructor
        
        Calls the adaptLength() method of the original reply object.
    */      
    ~DBMSrvCmdScd_SchedulerReply();

    /*!
        @brief append information from scheduler application

        The InfoType describes, how to handle the content of pVarPart. This
        method appends this content to the reply accordingly.
        @param infTy [IN] description how to handle pVarPart's content
        @param pVarPart [IN] 0-terminated string.
    */
    void appendSchedulerInfo(
        DBMSrvShM_LineAccessorScheduler::InfoType infTy,
        const char* pVarPart);

    /*!
        @brief return explanation of the command scheduler_list

        The explanation of the list command is stored in this class, because
        the command does nothing but requesting a list from the scheduler and passing
        it to this class. All interpretation is done here.
        @return help string to be used in scheduler_list's getHelpText() and getSyntaxText()
                methods.
        @see DBMSrvCmdScd_SchedulerListJobs
    */
    static const char* const getListExplanation() {return m_ListExplanation;}

private:
    void appendJobList(const char* aVarPart);

    DBMSrv_Reply& m_TheReply;

    static const char* const m_TextID;
    static const char* const m_TextOwner;
    static const char* const m_TextOnce;
    static const char* const m_TextStatus;
    static const char* const m_TextStatusActive;
    static const char* const m_TextStatusInactive;
    static const char* const m_TextRuns;
    static const char* const m_TextAfter;
    static const char* const m_TextAt;
    static const char* const m_TextCmd;
    static const char* const m_TextLastRun;
    static const char* const m_TextLastRunAt;
    static const char* const m_TextLastRunPid;
    static const char* const m_TextLastRunReturnCode;
    static const char* const m_TextLastRunUnknownReturnCode;
    static const char* const m_TextLastRunNever;
    static const char* const m_ListExplanation;
};

/*! 
    @brief base class for all scheduler commands

    Provides useful methods for starting and stopping the scheduler and implements the
    run() method. Communication with the scheduler must always be exclusive. So the run()
    method locks a resource "scheduler communication" at the start and unlocks it at
    the end. Between this, the pure virtual method runLockProtected() is called, which must
    be implemented by all scheduler commands.
*/
class DBMSrvCmdScd_SchedulerCommand: public DBMSrv_Command {
public:

    /*!
        @brief constructor

        Since this is an abstract class, the construtor is always called from
        a subclass.

        @param aCommandKey [IN] the key of this command
        \param longHelp    [IN] the long help of this command
    */
    DBMSrvCmdScd_SchedulerCommand(CommandKey aCommandKey, const char* longHelp=0, bool isHidden=false);

    /*! @brief desctructor */
    virtual ~DBMSrvCmdScd_SchedulerCommand() {}

    /*!
        @brief must no be overridden in subclasses
    */
    tcn00_Error run(
                    VControlDataT* vcontrol,
                    CommandT* command,
                    DBMSrv_Reply& theReply);

    /*!
        @brief must be overridden in subclasses

        This is called from run method after getting lock
        on communication with the scheduler.
        @param vcontrol [INOUT] the dbmserver object
        @param command [IN] command line
        @param theSchedReply [OUT] the object to write command response into
        @return execution result status
    */
    virtual tcn00_Error runLockProtected(
                    VControlDataT* vcontrol,
                    CommandT* command,
                    DBMSrvCmdScd_SchedulerReply& theSchedReply) = 0;

protected:
    bool isNumeric(const Tools_DynamicUTF8String& aString) const;

    tcn00_Error convertToTimeStruct(
        const Tools_DynamicUTF8String& aTimeString,
        struct tm& aTimeStruct, // out
        DBMSrvCmdScd_SchedulerReply& theSchedReply) const;

    tcn00_Error startScheduler(DBMSrvCmdScd_SchedulerReply& theSchedReply);
    tcn00_Error stopScheduler(
            DBMSrvCmdScd_SchedulerReply& theSchedReply);/*,
            bool stopJustIfSchedulerNotUpInitially = false);*/

    void resetTimeStruct( struct tm& aTimeStruct ) const {
        int* pInt=(int*)&aTimeStruct;
        for( int i=0; i<8; ++i )
            *(pInt+i) = 0;
    }

    const char* getUser() const {return m_User;}

    static const char* const m_SchedulerNotRunning;

private:
    tcn00_UserNamec m_User;
};
#endif //DBMSrvCmdScd_SchedulerCommandUpdate_hpp
