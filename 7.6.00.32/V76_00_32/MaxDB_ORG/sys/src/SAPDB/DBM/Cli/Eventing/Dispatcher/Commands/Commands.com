#    ========== licence begin  GPL
#    Copyright (c) 2005-2006 SAP AG
#
#    This program is free software; you can redistribute it and/or
#    modify it under the terms of the GNU General Public License
#    as published by the Free Software Foundation; either version 2
#    of the License, or (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#    ========== licence end

#
# include dependencies (do not edit)
#
DBMCliEvtDispCmd_Add.cpp inc=heo06.h,\
    :SAPDB/DBM/Cli/Eventing/Common/DBMCliEvtCm_Event.hpp,\
    :SAPDB/DBM/Cli/Eventing/Dispatcher/Commands/DBMCliEvtDispCmd_Add.hpp,\
    :SAPDB/DBM/Cli/Eventing/Dispatcher/DBMCliEvtDisp_Properties.hpp,\
    :SAPDB/DBM/Cli/Eventing/Dispatcher/KeyMatch/DBMCliEvtDispKeyMatch_Expression.hpp,\
    :SAPDB/DBM/VersionInfo/DBMVI_Strings.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_DBMEd.h,\
    :SAPDB/ToolsCommon/Tools_DynamicUTF8String.hpp
DBMCliEvtDispCmd_Add.hpp inc=\
    :SAPDB/DBM/Cli/Eventing/Dispatcher/Commands/DBMCliEvtDispCmd_List.hpp,\
    :SAPDB/ToolsCommon/Parsers/Old/ToolsParsersOld_ProgramParameterParser.hpp
DBMCliEvtDispCmd_Base.hpp inc=
DBMCliEvtDispCmd_Delete.cpp inc=\
    :SAPDB/DBM/Cli/Eventing/Dispatcher/Commands/DBMCliEvtDispCmd_Delete.hpp,\
    :SAPDB/DBM/Cli/Eventing/Dispatcher/DBMCliEvtDisp_Properties.hpp,\
    :SAPDB/DBM/Cli/Eventing/Dispatcher/KeyMatch/DBMCliEvtDispKeyMatch_Expression.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_DBMEd.h,\
    :SAPDB/ToolsCommon/Tools_DynamicUTF8String.hpp
DBMCliEvtDispCmd_Delete.hpp inc=\
    :SAPDB/DBM/Cli/Eventing/Dispatcher/Commands/DBMCliEvtDispCmd_List.hpp
DBMCliEvtDispCmd_Headers.hpp inc=\
    :SAPDB/DBM/Cli/Eventing/Dispatcher/Commands/DBMCliEvtDispCmd_Base.hpp,\
    :SAPDB/DBM/Cli/Eventing/Dispatcher/Commands/DBMCliEvtDispCmd_Add.hpp,\
    :SAPDB/DBM/Cli/Eventing/Dispatcher/Commands/DBMCliEvtDispCmd_Delete.hpp,\
    :SAPDB/DBM/Cli/Eventing/Dispatcher/Commands/DBMCliEvtDispCmd_List.hpp,\
    :SAPDB/DBM/Cli/Eventing/Dispatcher/Commands/DBMCliEvtDispCmd_Start.hpp,\
    :SAPDB/DBM/Cli/Eventing/Dispatcher/Commands/DBMCliEvtDispCmd_State.hpp,\
    :SAPDB/DBM/Cli/Eventing/Dispatcher/Commands/DBMCliEvtDispCmd_Stop.hpp
DBMCliEvtDispCmd_List.cpp inc=\
    :SAPDB/DBM/Cli/Eventing/Dispatcher/Commands/DBMCliEvtDispCmd_List.hpp,\
    :SAPDB/DBM/Cli/Eventing/Dispatcher/DBMCliEvtDisp_MsgDumper.hpp,\
    :SAPDB/DBM/Cli/Eventing/Dispatcher/DBMCliEvtDisp_Properties.hpp,\
    :SAPDB/DBM/Cli/Eventing/Dispatcher/KeyMatch/DBMCliEvtDispKeyMatch_Expression.hpp,\
    :SAPDB/DBM/Srv/Commands/DBMSrvCmd_EventAdministrationConstants.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_DBMEd.h,\
    :SAPDB/ToolsCommon/Tools_DynamicUTF8String.hpp
DBMCliEvtDispCmd_List.hpp inc=\
    :SAPDB/DBM/Cli/Eventing/Dispatcher/Commands/DBMCliEvtDispCmd_Base.hpp,\
    :SAPDB/Messages/Msg_List.hpp,:SAPDB/ToolsCommon/Tools_Properties.hpp
DBMCliEvtDispCmd_Start.cpp inc=\
    :SAPDB/DBM/Cli/Eventing/Dispatcher/Commands/DBMCliEvtDispCmd_Start.hpp,\
    :SAPDB/DBM/Cli/Eventing/Dispatcher/DBMCliEvtDisp_Main.hpp,\
    :SAPDB/DBM/Cli/Eventing/Dispatcher/DBMCliEvtDisp_MsgDumper.hpp,\
    :SAPDB/DBM/Logging/DBMLog_Logger.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_DBMEd.h
DBMCliEvtDispCmd_Start.hpp inc=\
    :SAPDB/DBM/Cli/Eventing/Dispatcher/Commands/DBMCliEvtDispCmd_Base.hpp,\
    :SAPDB/DBM/Cli/Eventing/Dispatcher/DBMCliEvtDisp_Dispatcher.hpp,\
    :SAPDB/Messages/Msg_List.hpp
DBMCliEvtDispCmd_State.cpp inc=\
    :SAPDB/DBM/Cli/Eventing/Dispatcher/Commands/DBMCliEvtDispCmd_State.hpp,\
    :SAPDB/DBM/Cli/Eventing/Dispatcher/DBMCliEvtDisp_MsgDumper.hpp,\
    :SAPDB/DBM/Cli/Eventing/Dispatcher/DBMCliEvtDisp_SrvCommand.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_DBMEd.h
DBMCliEvtDispCmd_State.hpp inc=:SAPDB/DBM/Cli/DBMCli_Session.hpp,\
    :SAPDB/DBM/Cli/Eventing/Dispatcher/Commands/DBMCliEvtDispCmd_Base.hpp,\
    :SAPDB/Messages/Msg_List.hpp
DBMCliEvtDispCmd_Stop.cpp inc=\
    :SAPDB/DBM/Cli/Eventing/Dispatcher/Commands/DBMCliEvtDispCmd_Stop.hpp,\
    :SAPDB/DBM/Cli/Eventing/Dispatcher/DBMCliEvtDisp_MsgDumper.hpp,\
    :SAPDB/DBM/Cli/Eventing/Dispatcher/DBMCliEvtDisp_SrvCommand.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_DBMEd.h
DBMCliEvtDispCmd_Stop.hpp inc=:SAPDB/DBM/Cli/DBMCli_Session.hpp,\
    :SAPDB/DBM/Cli/Eventing/Dispatcher/Commands/DBMCliEvtDispCmd_Base.hpp,\
    :SAPDB/Messages/Msg_List.hpp

