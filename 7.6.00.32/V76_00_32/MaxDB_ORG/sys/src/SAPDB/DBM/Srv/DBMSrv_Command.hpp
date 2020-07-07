/*!
    \file    DBMSrvCmd_Command.hpp
    \author  MarcW
    \ingroup common classes for the DBMServer
    \brief   declaration of base class for DBMServer commands

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

#ifndef _DBMSrv_Command_HPP_
#define _DBMSrv_Command_HPP_


#include "gcn00.h"
#include "gcn002.h"

#include "DBM/Srv/DBMSrv_Reply.hpp"


/*!
    @brief base class for DBMServer commands

    All DBMServer commands should be derived from this one. At the moment, command
    execution, printing of command syntax and command explanation is possible. In
    the future, more tasks e.g. parameter checking should be moved into this
    class. The array in vcn10.cpp should then only contain a mapping of
    command strings entered by user and command keys, defined here (or that
    array should disappear all together).
    @see DBMSrv_Command::CommandKey
*/
class DBMSrv_Command {
public:
    /*!
        @brief the command keys

        Each command has its unique key.
    */
    enum CommandKey {
        KeyApropos,                             /*!< key for command class DBMSrvCmd_Apropos */
        KeyAutoExtend,                          /*!< handled in DBMSrvCmdAuto_AutoExtend */
        KeyAutoUpdateStatistics,                /*!< handled in DBMSrvCmdAuto_AutoUpdateStatistics */
        KeyBackupExtIdsGet,                     /*!< handled in DBMSrvCmd_BackupExtIds */
        KeyBackupExtIdsGetFromTool,             /*!< handled in DBMSrvCmd_BackupExtIds */
        KeyBackupHistoryAppend,                 /*!< key for command class DBMSrvCmdBHist_BackupHistoryAppend */
        KeyBackupHistoryClose,                  /*!< handled in DBMSrvCmd_BackupHistory */
        KeyBackupHistoryDate,                   /*!< handled in DBMSrvCmd_BackupHistory */
        KeyBackupHistoryList,                   /*!< handled in DBMSrvCmd_BackupHistory */
        KeyBackupHistoryListNext,               /*!< handled in DBMSrvCmd_BackupHistory */
        KeyBackupHistoryOpen,                   /*!< handled in DBMSrvCmd_BackupHistory */
        KeyDbmExecuteServerprocess,             /*!< handled in DBMSrvCmdDbm_DbmExecuteServerprocess */
        KeyDbmGetpid,                           /*!< key for class DBMSrvCmdDbm_DbmGetPid */
        KeyDbmListMemoryerror,                  /*!< handled in DBMSrvCmdDbm_DbmListMemoryerror */
        KeyDbmShmInfo,                          /*!< handled in DBMSrvCmdDbmShm_DbmShmInfo */
        KeyDbmShmReset,                         /*!< handled in DBMSrvCmdDbmShm_DbmShmReset */
        KeyDbmShmUnlock,                        /*!< handled in DBMSrvCmdDbmShm_DbmShmUnlock */
        KeyDbmVersion,                          /*!< handled in DBMSrvCmd_DbmVersion */
        KeyDbSpeed,                             /*!< handled in DBMSrvCmd_DbSpeed */
        KeyDbState,                             /*!< handled in DBMSrvCmd_DbState */
        KeyEventAvailable,                      /*!< handled in DBMSrvCmdEvent_EventAvailable */
        KeyEventDelete,                         /*!< handled in DBMSrvCmdEvent_EventDelete */
        KeyEventList,                           /*!< handled in DBMSrvCmdEvent_EventList */
        KeyEventListCategories,                 /*!< handled in DBMSrvCmdEvent_EventListCategories */
        KeyEventReceive,                        /*!< handled in DBMSrvCmdEvent_EventReceive */
        KeyEventRelease,                        /*!< handled in DBMSrvCmdEvent_EventRelease */
        KeyEventSet,                            /*!< handled in DBMSrvCmdEvent_EventSet */
        KeyEventWait,                           /*!< handled in DBMSrvCmdEvent_EventWait */
        KeyExplain,                             /*!< handled in DBMSrvCmd_Explain */
        KeyGetDefault,                          /*!< key for command class DBMSrvCmd_GetDefault */
        KeyHelp,                                /*!< key for command class DBMSrvCmd_Help */
        KeyHssCopyFile,                         /*!< handled in DBMSrvCmd_HssCopyFile        */
        KeyInstInfo,                            /*!< handled in DBMSrvCmd_InstInfo */
        KeyMediumDelete,                        /*!< key for command class DBMSrvCmdMedium_MediumDelete */
        KeyMediumDeleteNoDistribute,            /*!< key for command class DBMSrvCmdMedium_MediumDeleteNoDistrib */
        KeyMediumPut,                           /*!< key for command class DBMSrvCmdMedium_MediumPut */
        KeyMediumPutNoDistribute,               /*!< key for command class DBMSrvCmdMedium_MediumPutNoDistrib */
        KeySchedulerActivateJob,                /*!< handled in DBMSrvCmdScd_DeactivateJob */
        KeySchedulerDeactivateJob,              /*!< handled in DBMSrvCmdScd_DeactivateJob */
        KeySchedulerDeleteJob,                  /*!< handled in DBMSrvCmdScd_DeleteJob */
        KeySchedulerState,                      /*!< handled in DBMSrvCmdScd_SchedulerState */
        KeySchedulerListJobs,                   /*!< handled in DBMSrvCmdScd_SchedulerListJobs */
        KeySchedulerCreateJob,                  /*!< handled in DBMSrvCmdScd_SchedulerCreateJob */
        KeySchedulerShowJoblog,                 /*!< handled in DBMSrvCmdScd_SchedulerShowJoblog */
        KeySchedulerDeleteJoblog,               /*!< handled in DBMSrvCmdScd_SchedulerShowDeletelog */
        KeySchedulerStart,                      /*!< handled in DBMSrvCmdScd_SchedulerStart */
        KeySchedulerStop,                       /*!< handled in DBMSrvCmdScd_SchedulerStop */
        KeyTraceClear,                          /*!< handled in DBMSrvCmdTrc_TraceClear */
        KeyTraceFlush,                          /*!< handled in DBMSrvCmdTrc_TraceFlush */
        KeyTraceOff,                            /*!< handled in DBMSrvCmdTrc_TraceOff */
        KeyTraceOn,                             /*!< handled in DBMSrvCmdTrc_TraceOn */
        KeyTraceProt,                           /*!< handled in DBMSrvCmdTrc_TraceProt */
        KeyTraceProtOpt,                        /*!< handled in DBMSrvCmdTrc_TraceProtOpt */
        KeyTraceShow,                           /*!< handled in DBMSrvCmdTrc_TraceShow */
        
        /* stop inserting new keys here, command keys are dead, use the command factory class
           DBMSrvCmdInfr_CommandFactory instead, start deleting keys from above */

        KeyUsingCommandFactory,                 /*!< used by every command handeled by the command factory */
        KeyUnknownCommand                       /*!< the unknown command, no implementation! */
    }; 

    /* \brief dispatching stuff
    
       get a command object for the command key 
       \param aCommandKey [IN] the key for the searched command object
       \param theReply    [IN] reply object holding the reply buffer
       \return pointer to a object representing the command, 0 if error occured */
    static DBMSrv_Command* getCommand(
                CommandKey     aCommandKey,
                const char   * commandName,
                DBMSrv_Reply & theReply);

    // constructor
    DBMSrv_Command( CommandKey commandKey, bool isObsolete, const char * longHelp=0, bool isHidden=false )
        : m_isObsolete(isObsolete),
          m_isHidden(isHidden),
          m_longHelp(longHelp),
          m_RundirectoryNotCalculated(true),
          m_UnicodeNotCalculated(true),
          m_InstanceTypeNotCalculated(true),
          m_commandKey(commandKey) {
        m_Rundirectory.Init();
        m_Unicode.Init();
        m_InstanceType.Init();
    }

    // constructor to be used from factory and classic command
    DBMSrv_Command( bool isObsolete, const char * longHelp=0, bool isHidden=false )
        : m_isObsolete(isObsolete),
          m_isHidden(isHidden),
          m_longHelp(longHelp),
          m_RundirectoryNotCalculated(true),
          m_UnicodeNotCalculated(true),
          m_InstanceTypeNotCalculated(true),
          m_commandKey(DBMSrv_Command::KeyUsingCommandFactory) {
        m_Rundirectory.Init();
        m_Unicode.Init();
        m_InstanceType.Init();
    }

    virtual ~DBMSrv_Command() {}

    // base class stuff
    /*! function representing the wish of the command to be an obsolete command
        in the DBM Server command help. The help command should honor this wish.
        \return true, if the DBM server command is an osolete command, false otherwise */
    bool isObsolete() { return m_isObsolete;}
    /*! function representing the wish of the command to be hidden by the DBM Server command help
        \return true, if the DBM server command should be hidden, false otherwise */
    bool isHidden() { return m_isHidden;}
    
    /*!
        @brief property of a command object
        
        Those commands, which clean up the DBMServer and make it ready for stopping, must override this method
        to return true.
        @return true if this is an exit command, false otherwise
    */
    virtual bool isExitCommand() {return false;}
    
    CommandKey getCommandKey() { return m_commandKey;}

    virtual bool getExplainText(CommandT* command, DBMSrv_Reply& theReply);
    virtual bool getSyntaxText(DBMSrv_Reply& theReply);
    virtual bool getUsageText(DBMSrv_Reply& theReply);

    virtual bool         hasOption(const char *option, size_t optionLength);
    /*! determine the default for a command option as string
        \param option       [IN]  the option name
        \param optionLength [IN]  the length of the option name
        \param defaultStart [OUT] start of the default, 0 if option has no default (in it's m_longHelp)
        \param defaultEnd   [OUT] end of the default (one character behind), 0 if option has no default (in it's m_longHelp)
        \return true if command has the option, false otherwise */
    virtual bool getDefaultForOption(
        const char  * option,
        size_t        optionLength,
        const char *& defaultStart,
        const char *& defaultEnd);

    /*! \brief the virtual function implementing the command
        (you have to overload this function in order to implement a new command) */
    virtual tcn00_Error run(
                    VControlDataT* vcontrol,
                    CommandT* command,
                    DBMSrv_Reply& theReply) = 0;

    /*! \brief static function providing the run function of a command for vcn10.cpp
        \TODO should be eliminated together with vcn10.cpp in the long run */
    static tcn00_Error runCommand
        (VControlDataT * vcontrol,
         CommandT      * command,
         char          * replyData,
         int           * replyLen,
         int             replyLenMax);

protected:
    // members
    CommandKey   m_commandKey;
    bool         m_isObsolete;
    bool         m_isHidden;   //!< true if command would like to be hidden in the output of the help command
    const char * m_longHelp;

    // helpers
    const char* getRundirectory(bool ReadAgain = false);
    const char* getUnicode     (bool ReadAgain = false);
    const char* getInstanceType(bool ReadAgain = false);

    static void formatExplainText(CommandT* command, DBMSrv_Reply& theReply, const char* longHelp);
    static void formatSyntaxText(DBMSrv_Reply& theReply, const char* longHelp);

private:
    bool getParameter   (const char * szName, tcn002_XpValueString & oValue);

    bool m_RundirectoryNotCalculated;
    tcn002_XpValueString m_Rundirectory;
    bool m_UnicodeNotCalculated;
    tcn002_XpValueString m_Unicode;
    bool m_InstanceTypeNotCalculated;
    tcn002_XpValueString m_InstanceType;

    static const char * const defaultSentencePart1; //!< start of sentence used by explain for default values (up to the default value)
    static const char * const defaultSentencePart2; //!< end of the sentence used by explain for default values (starting behind the default value)
    static const char * const allowedValuesSentencePart1; //!< sentence used by explain for allowed parameter values (up to option name)
    static const char * const allowedValuesSentencePart2; //!< sentence used by explain for allowed parameter values (starting behind the option name)

    static bool parameterHasName(const char * parameterName, size_t parameterNameLength, const char * name, size_t nameLength);

    static tcn00_Error appendWrappedText(
        DBMSrv_Reply   reply,
        const char   * string1,
        const char   * string2,
        size_t         lengthString2,
        const char   * string3,
        size_t         indent,
        size_t         lineSize,
        size_t       & postionInOutputLine);
};
#endif //_DBMSrv_Cmd_HPP_
