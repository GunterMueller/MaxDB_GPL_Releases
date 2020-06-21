/*!
  \file    DBMSrvCmdAuto_AutoExtend.hpp
  \author  MarcW
  \ingroup DBMServer commands
  \brief   declaration of classes for handling DBMServer
           commands dealing with the autoextender

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

#include "DBM/Srv/Commands/DBMSrvCmd_SilentDispatcherUsingCommand.hpp"

#if !defined(DBMSrvCmdAuto_AutoExtend_hpp)
#define DBMSrvCmdAuto_AutoExtend_hpp


/*! 
    @brief Class for handling the DBMServer command auto_extend
*/
class DBMSrvCmdAuto_AutoExtend: public DBMSrvCmd_SilentDispatcherUsingCommand {
public:
    /*! @brief the mode keys */
    enum Mode {
        ON,                           /*!< key for ON */
        OFF,                          /*!< key for OFF */
        SHOW,                         /*!< key for SHOW */
        UNDEFINED                     /*!< key for undefined mode */
    }; 

    /*! @brief constructor */
    DBMSrvCmdAuto_AutoExtend();

    /*! 
        @brief implementation of the command execution

        @param vcontrol [INOUT] the dbmserver object
        @param command [IN] command line
        @param theReply [OUT] the object to write command response into
        @return execution result status
    */
    tcn00_Error runLockProtected(VControlDataT* vcontrol,
        CommandT* command,
        DBMSrv_Reply& theReply);

    /*! @brief default filling degree, at which a new volume should be added */
    static const int m_PercentageDefault;

    static void buildConfigLine(
            Tools_DynamicUTF8String& eventName,
            int percentage,
            Tools_DynamicUTF8String& autoExtendCondition,
            Tools_DynamicUTF8String& autoExtendHandler);

private:
    tcn00_Error analyzeParameters(Tools_Arguments& args, DBMSrv_Reply& theReply);
    void analyzeConfigLine(
        Tools_DynamicUTF8String& condition,
        Tools_DynamicUTF8String& handler,
        bool& bLineExists,
        int& iPercent);

    void handleON(
        DBMSrvMsg_Error& errOut,
        Tools_DynamicUTF8String& autoExtendCondition,
        Tools_DynamicUTF8String& autoExtendHandler,
        bool bConfigLineExists,
        int iPercent);

    void handleOFF(
        DBMSrvMsg_Error& errOut,
        Tools_DynamicUTF8String& autoExtendCondition,
        bool bConfigLineExists);
    
    void handleSHOW(
        DBMSrv_Reply& theReply,
        int iPercent,
        bool bConfigLineExists,
        bool bDispatcherUp);

    Mode m_Mode;
    int  m_Percentage;

    static const char* const m_ModeTextON;
    static const char* const m_ModeTextOFF;
    static const char* const m_ModeTextSHOW;

    Tools_DynamicUTF8String m_EventName;

    static const char* const m_StatusTextON;
    static const char* const m_StatusTextOFF;
    static const char* const m_StatusTextUNKNOWN;

    static const char* const m_ConfigLineTemplate;
};
#endif //DBMSrvCmdAuto_AutoExtend_hpp
