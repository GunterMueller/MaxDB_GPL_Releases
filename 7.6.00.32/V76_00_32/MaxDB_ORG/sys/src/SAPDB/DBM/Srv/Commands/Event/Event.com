#    ========== licence begin  GPL
#    Copyright (c) 2005 SAP AG
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
DBMSrvCmdEvent_EventAvailable.cpp inc=\
    :SAPDB/DBM/Srv/Commands/Event/DBMSrvCmdEvent_EventAvailable.hpp,\
    :SAPDB/DBM/Srv/KernelAccess/DBMSrvKnl_GlobalSessions.hpp,\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_Error.hpp
DBMSrvCmdEvent_EventAvailable.hpp inc=vsp0031,\
    :SAPDB/DBM/Srv/DBMSrv_Command.hpp
DBMSrvCmdEvent_EventDelete.cpp inc=\
    :SAPDB/DBM/Srv/Commands/Event/DBMSrvCmdEvent_EventDelete.hpp,\
    :SAPDB/DBM/Srv/Infrastructure/Reply/DBMSrvInfrReply_ReplyBufferWritingIntoMsgList.hpp,\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_Error.hpp,\
    :SAPDB/SAPDBCommon/DatabaseEvents/SAPDBDBE_EventCategory.hpp
DBMSrvCmdEvent_EventDelete.hpp inc=\
    :SAPDB/DBM/Srv/Commands/DBMSrvCmd_EventAdministrationCommand.hpp
DBMSrvCmdEvent_EventList.cpp inc=\
    :SAPDB/DBM/Srv/Commands/Event/DBMSrvCmdEvent_EventList.hpp,\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_Error.hpp,\
    :SAPDB/DBM/Srv/Parameters/DBMSrvPar_Iterator.hpp,\
    :SAPDB/SAPDBCommon/DatabaseEvents/SAPDBDBE_EventCategory.hpp
DBMSrvCmdEvent_EventList.hpp inc=\
    :SAPDB/DBM/Srv/Commands/DBMSrvCmd_EventAdministrationCommand.hpp
DBMSrvCmdEvent_EventListCategories.cpp inc=\
    :SAPDB/DBM/Srv/Commands/Event/DBMSrvCmdEvent_EventListCategories.hpp,\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_Error.hpp,\
    :SAPDB/DBM/Srv/Parameters/DBMSrvPar_Iterator.hpp
DBMSrvCmdEvent_EventListCategories.hpp inc=\
    :SAPDB/DBM/Srv/Commands/DBMSrvCmd_EventAdministrationCommand.hpp
DBMSrvCmdEvent_EventReceive.cpp inc=\
    :SAPDB/DBM/Srv/Commands/Event/DBMSrvCmdEvent_EventWait.hpp,\
    :SAPDB/DBM/Srv/Commands/Event/DBMSrvCmdEvent_EventReceive.hpp,\
    :SAPDB/DBM/Srv/KernelAccess/DBMSrvKnl_GlobalSessions.hpp
DBMSrvCmdEvent_EventReceive.hpp inc=:SAPDB/DBM/Srv/DBMSrv_Command.hpp
DBMSrvCmdEvent_EventRelease.cpp inc=\
    :SAPDB/DBM/Srv/Commands/Event/DBMSrvCmdEvent_EventRelease.hpp,\
    :SAPDB/DBM/Srv/KernelAccess/DBMSrvKnl_GlobalSessions.hpp
DBMSrvCmdEvent_EventRelease.hpp inc=:SAPDB/DBM/Srv/DBMSrv_Command.hpp
DBMSrvCmdEvent_EventSet.cpp inc=\
    :SAPDB/DBM/Srv/Commands/Event/DBMSrvCmdEvent_EventSet.hpp,\
    :SAPDB/DBM/Srv/Infrastructure/Reply/DBMSrvInfrReply_ReplyBufferWritingIntoMsgList.hpp,\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_Error.hpp,\
    :SAPDB/SAPDBCommon/DatabaseEvents/SAPDBDBE_EventCategory.hpp
DBMSrvCmdEvent_EventSet.hpp inc=\
    :SAPDB/DBM/Srv/Commands/DBMSrvCmd_EventAdministrationCommand.hpp
DBMSrvCmdEvent_EventWait.cpp inc=\
    :SAPDB/DBM/Srv/Commands/Event/DBMSrvCmdEvent_EventWait.hpp,\
    :SAPDB/DBM/Srv/Commands/DBMSrvCmd_EventAdministrationCommand.hpp,\
    :SAPDB/DBM/Srv/KernelAccess/DBMSrvKnl_GlobalSessions.hpp,\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_Error.hpp,\
    :SAPDB/DBM/Srv/Parameters/DBMSrvPar_Iterator.hpp
DBMSrvCmdEvent_EventWait.hpp inc=vsp0031,\
    :SAPDB/DBM/Srv/DBMSrv_Command.hpp,\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_Error.hpp,\
    :SAPDB/ToolsCommon/Tools_Vector.hpp,\
    :SAPDB/DBM/Srv/DatabaseEvents/DBMSrvDBE_EventMapping.hpp
DBMSrvCmdEvent_Header.hpp inc=\
    :SAPDB/DBM/Srv/Commands/Event/DBMSrvCmdEvent_EventAvailable.hpp,\
    :SAPDB/DBM/Srv/Commands/Event/DBMSrvCmdEvent_EventDelete.hpp,\
    :SAPDB/DBM/Srv/Commands/Event/DBMSrvCmdEvent_EventList.hpp,\
    :SAPDB/DBM/Srv/Commands/Event/DBMSrvCmdEvent_EventListCategories.hpp,\
    :SAPDB/DBM/Srv/Commands/Event/DBMSrvCmdEvent_EventReceive.hpp,\
    :SAPDB/DBM/Srv/Commands/Event/DBMSrvCmdEvent_EventRelease.hpp,\
    :SAPDB/DBM/Srv/Commands/Event/DBMSrvCmdEvent_EventSet.hpp,\
    :SAPDB/DBM/Srv/Commands/Event/DBMSrvCmdEvent_EventWait.hpp

