
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
dbmevtdisp.cfg ascii ->$INSTROOT/env/

#
# include dependencies (do not edit)
#
DBMCliEvtDisp_Dispatcher.cpp inc=:SAPDB/RunTime/RTE_ISystem.hpp,\
    :SAPDB/ToolsCommon/Tools_PipeCall.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_DBMEd.h,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_UniqueMessageNumber.hpp,\
    :SAPDB/DBM/Cli/Eventing/Dispatcher/DBMCliEvtDisp_Dispatcher.hpp,\
    :SAPDB/DBM/Cli/Eventing/Dispatcher/DBMCliEvtDisp_EventListener.hpp,\
    :SAPDB/DBM/Cli/Eventing/Dispatcher/DBMCliEvtDisp_MsgDumper.hpp,\
    :SAPDB/DBM/Cli/Eventing/Dispatcher/DBMCliEvtDisp_Permissions.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Stuff.hpp,\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_Error.hpp,\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_ErrorHandler.hpp,\
    :SAPDB/DBM/Srv/Process/DBMSrvProc_DeamonProcess.hpp,\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_LineAccessorEventing.hpp
DBMCliEvtDisp_Dispatcher.hpp inc=\
    :SAPDB/DBM/Cli/Eventing/Dispatcher/DBMCliEvtDisp_EventListener.hpp,\
    :SAPDB/DBM/Cli/Eventing/Dispatcher/DBMCliEvtDisp_Properties.hpp,\
    :SAPDB/DBM/Cli/Eventing/Dispatcher/DBMCliEvtDisp_SrvCommand.hpp,\
    :SAPDB/DBM/Cli/Eventing/Dispatcher/KeyMatch/DBMCliEvtDispKeyMatch_Expression.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Stuff.hpp,:SAPDB/DBM/Logging/DBMLog_Logger.hpp
DBMCliEvtDisp_EventListener.cpp inc=\
    :SAPDB/DBM/Cli/Eventing/Dispatcher/DBMCliEvtDisp_EventListener.hpp
DBMCliEvtDisp_EventListener.hpp inc=:SAPDB/DBM/Cli/DBMCli_Database.hpp,\
    :SAPDB/DBM/Cli/Eventing/Common/DBMCliEvtCm_Event.hpp
DBMCliEvtDisp_Main.cpp inc=heo670.h,hni34.h,hsp100.h,\
    :SAPDB/DBM/Cli/Eventing/Dispatcher/Commands/DBMCliEvtDispCmd_Headers.hpp,\
    :SAPDB/DBM/Cli/Eventing/Dispatcher/DBMCliEvtDisp_Main.hpp,\
    :SAPDB/DBM/Cli/Eventing/Dispatcher/DBMCliEvtDisp_MsgDumper.hpp,\
    :SAPDB/DBM/Logging/DBMLog_LogFile.hpp,\
    :SAPDB/DBM/Logging/DBMLog_Logger.hpp,\
    :SAPDB/DBM/Srv/Commands/DBMSrvCmd_EventAdministrationConstants.hpp,\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_Error.hpp,\
    :SAPDB/DBM/VersionInfo/DBMVI_Strings.hpp,\
    :SAPDB/RunTime/Communication/RTEComm_URIBuilder.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_DBMEd.h,\
    :SAPDB/ToolsCommon/Parsers/Old/ToolsParsersOld_ProgramParameterParser.hpp,\
    :SAPDB/ToolsCommon/Tools_PipeCall.hpp
DBMCliEvtDisp_Main.hpp inc=\
    :SAPDB/DBM/Cli/Eventing/Dispatcher/DBMCliEvtDisp_Dispatcher.hpp
DBMCliEvtDisp_MsgDumper.cpp inc=\
    :SAPDB/DBM/Cli/Eventing/Dispatcher/DBMCliEvtDisp_MsgDumper.hpp,\
    :SAPDB/DBM/Logging/DBMLog_Logger.hpp
DBMCliEvtDisp_MsgDumper.hpp inc=:SAPDB/Messages/Msg_List.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_DBMEd.h
DBMCliEvtDisp_Permissions.hpp inc=
DBMCliEvtDisp_Properties.cpp inc=\
    :SAPDB/DBM/Cli/Eventing/Dispatcher/DBMCliEvtDisp_Properties.hpp,\
    :SAPDB/DBM/VersionInfo/DBMVI_Strings.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_DBMEd.h,hsp100.h
DBMCliEvtDisp_Properties.hpp inc=:SAPDB/ToolsCommon/Tools_Properties.hpp
DBMCliEvtDisp_Resource.rc inc=gsp100.h,:SAPDB/DBM/DBM_Resource.rc
DBMCliEvtDisp_SrvCommand.cpp inc=\
    :SAPDB/DBM/Cli/Eventing/Dispatcher/DBMCliEvtDisp_SrvCommand.hpp
DBMCliEvtDisp_SrvCommand.hpp inc=

