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
DBMSrv_Command.cpp inc=hcn10.h,hcn20.h,hcn36.h,hcn90.h,\
    :SAPDB/DBM/Srv/DBMSrv_Command.hpp,\
    :SAPDB/DBM/Srv/ExplainHelp/DBMSrvExpHlp_CommandPart.hpp,\
    :SAPDB/DBM/Srv/ExplainHelp/DBMSrvExpHlp_DefaultValuePart.hpp,\
    :SAPDB/DBM/Srv/ExplainHelp/DBMSrvExpHlp_ParameterPart.hpp,\
    :SAPDB/DBM/Srv/ExplainHelp/DBMSrvExpHlp_PreconditionsPart.hpp,\
    :SAPDB/DBM/Srv/ExplainHelp/DBMSrvExpHlp_ReplyPart.hpp,\
    :SAPDB/DBM/Srv/ExplainHelp/DBMSrvExpHlp_ReplyValuePart.hpp,\
    :SAPDB/DBM/Srv/ExplainHelp/DBMSrvExpHlp_SyntaxPart.hpp,\
    :SAPDB/DBM/Srv/ExplainHelp/DBMSrvExpHlp_ValuePart.hpp,\
    :SAPDB/DBM/Srv/Commands/Auto/DBMSrvCmdAuto_Headers.hpp,\
    :SAPDB/DBM/Srv/Commands/BackupHistory/DBMSrvCmdBHist_BackupHistoryAppend.hpp,\
    :SAPDB/DBM/Srv/Commands/Db/DBMSrvCmdDb_DbAddVolume.hpp,\
    :SAPDB/DBM/Srv/Commands/Dbm/DBMSrvCmdDbm_DbmExecuteServerprocess.hpp,\
    :SAPDB/DBM/Srv/Commands/Dbm/DBMSrvCmdDbm_DbmGetpid.hpp,\
    :SAPDB/DBM/Srv/Commands/Dbm/DBMSrvCmdDbm_DbmListMemoryerror.hpp,\
    :SAPDB/DBM/Srv/Commands/Dbm/Shm/DBMSrvCmdDbmShm_DbmShmReset.hpp,\
    :SAPDB/DBM/Srv/Commands/Dbm/Shm/DBMSrvCmdDbmShm_DbmShmInfo.hpp,\
    :SAPDB/DBM/Srv/Commands/Dbm/Shm/DBMSrvCmdDbmShm_DbmShmUnlock.hpp,\
    :SAPDB/DBM/Srv/Commands/DBMSrvCmd_Apropos.hpp,\
    :SAPDB/DBM/Srv/Commands/DBMSrvCmd_BackupExtIds.hpp,\
    :SAPDB/DBM/Srv/Commands/DBMSrvCmd_BackupHistory.hpp,\
    :SAPDB/DBM/Srv/Commands/DBMSrvCmd_DbmVersion.hpp,\
    :SAPDB/DBM/Srv/Commands/DBMSrvCmd_DbSpeed.hpp,\
    :SAPDB/DBM/Srv/Commands/DBMSrvCmd_DbState.hpp,\
    :SAPDB/DBM/Srv/Commands/DBMSrvCmd_Explain.hpp,\
    :SAPDB/DBM/Srv/Commands/DBMSrvCmd_GetDefault.hpp,\
    :SAPDB/DBM/Srv/Commands/DBMSrvCmd_Help.hpp,\
    :SAPDB/DBM/Srv/Commands/DBMSrvCmd_HssCopyFile.hpp,\
    :SAPDB/DBM/Srv/Commands/DBMSrvCmd_InstInfo.hpp,\
    :SAPDB/DBM/Srv/Commands/Event/DBMSrvCmdEvent_Header.hpp,\
    :SAPDB/DBM/Srv/Commands/Medium/DBMSrvCmdMedium_MediumDelete.hpp,\
    :SAPDB/DBM/Srv/Commands/Medium/DBMSrvCmdMedium_MediumDeleteNoDistribute.hpp,\
    :SAPDB/DBM/Srv/Commands/Medium/DBMSrvCmdMedium_MediumPut.hpp,\
    :SAPDB/DBM/Srv/Commands/Medium/DBMSrvCmdMedium_MediumPutNoDistribute.hpp,\
    :SAPDB/DBM/Srv/Commands/Scheduler/DBMSrvCmdScd_Header.hpp,\
    :SAPDB/DBM/Srv/Commands/Trace/DBMSrvCmdTrc_Header.hpp
DBMSrv_Command.hpp inc=gcn00.h,gcn002.h,:SAPDB/DBM/Srv/DBMSrv_Reply.hpp
DBMSrv_Reply.cpp inc=:SAPDB/DBM/Srv/DBMSrv_Reply.hpp,\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_Error.hpp,\
    :SAPDB/ToolsCommon/MemoryManagement/ToolsMeMa_GuardedOperation.hpp,\
    :SAPDB/ToolsCommon/Tools_List.hpp,hcn36.h
DBMSrv_Reply.hpp inc=gcn00.h,gsp01.h

