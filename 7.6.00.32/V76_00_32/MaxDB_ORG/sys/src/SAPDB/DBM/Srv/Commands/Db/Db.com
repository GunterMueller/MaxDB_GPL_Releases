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

DBMSrvCmdDb_DbReg.cpp setbuildinfo

#
# include dependencies (do not edit)
#
DBMSrvCmdDb_DBDeleteVolume.cpp inc=hcn40.h,hcn50.h,hcn90.h,\
    :SAPDB/DBM/Srv/Commands/Db/DBMSrvCmdDb_DBDeleteVolume.hpp,\
    :SAPDB/DBM/Srv/KernelAccess/DBMSrvKnl_Session.hpp,\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_InternalResource.hpp,\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_LineAccessorResourceLock.hpp,\
    :SAPDB/DBM/Srv/Volumes/DBMSrvVol_Volume.hpp
DBMSrvCmdDb_DBDeleteVolume.hpp inc=gcn00.h,\
    :SAPDB/DBM/Srv/DBMSrv_Command.hpp
DBMSrvCmdDb_DbAddVolume.cpp inc=hcn40.h,hcn50.h,hcn90.h,\
    :SAPDB/DBM/Srv/Commands/Db/DBMSrvCmdDb_DbAddVolume.hpp,\
    :SAPDB/DBM/Srv/KernelAccess/DBMSrvKnl_Session.hpp,\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_InternalResource.hpp,\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_LineAccessorResourceLock.hpp
DBMSrvCmdDb_DbAddVolume.hpp inc=:SAPDB/DBM/Srv/DBMSrv_Command.hpp,\
    gcn00.h
DBMSrvCmdDb_DbAdmin.cpp inc=hcn40.h,\
    :SAPDB/DBM/Srv/Commands/Db/DBMSrvCmdDb_DbAdmin.hpp
DBMSrvCmdDb_DbAdmin.hpp inc=:SAPDB/DBM/Srv/DBMSrv_Command.hpp
DBMSrvCmdDb_DbCold.hpp inc=\
    :SAPDB/DBM/Srv/Commands/Db/DBMSrvCmdDb_DbAdmin.hpp
DBMSrvCmdDb_DbConnect.cpp inc=\
    :SAPDB/DBM/Srv/KernelAccess/DBMSrvKnl_DbAndSqlCommands.hpp,\
    :SAPDB/DBM/Srv/Commands/Db/DBMSrvCmdDb_DbConnect.hpp
DBMSrvCmdDb_DbConnect.hpp inc=:SAPDB/DBM/Srv/DBMSrv_Command.hpp
DBMSrvCmdDb_DbDrop.cpp inc=hcn20.h,hcn42.h,heo06.h,heo11.h,gsp09.h,\
    :SAPDB/DBM/Logging/DBMLog_Logger.hpp,\
    :SAPDB/DBM/Srv/Commands/Db/DBMSrvCmdDb_DbDrop.hpp,\
    :SAPDB/DBM/Srv/Commands/Scheduler/DBMSrvCmdScd_SchedulerStop.hpp,\
    :SAPDB/DBM/Srv/Commands/DBMSrvCmd_SilentDispatcherUsingCommand.hpp,\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_ShmInfo.hpp,\
    :SAPDB/DBM/Srv/HSS/DBMSrvHSS_Nodes.hpp,\
    :SAPDB/DBM/Srv/KernelAccess/DBMSrvKnl_State.hpp,\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_RTEError.hpp,\
    :SAPDB/RunTime/Diagnose/RTEDiag_ClientCrashHist.h,\
    :SAPDB/RunTime/RTE_DBRegister.hpp,\
    :SAPDB/RunTime/RTE_UserProfileContainer.hpp,\
    :SAPDB/ToolsCommon/Tools_System.hpp
DBMSrvCmdDb_DbDrop.hpp inc=:SAPDB/DBM/Srv/DBMSrv_Command.hpp
DBMSrvCmdDb_DbExecute.cpp inc=\
    :SAPDB/DBM/Srv/KernelAccess/DBMSrvKnl_DbAndSqlCommands.hpp,\
    :SAPDB/DBM/Srv/Commands/Db/DBMSrvCmdDb_DbExecute.hpp
DBMSrvCmdDb_DbExecute.hpp inc=:SAPDB/DBM/Srv/DBMSrv_Command.hpp
DBMSrvCmdDb_DbExecuteNice.cpp inc=\
    :SAPDB/DBM/Srv/KernelAccess/DBMSrvKnl_DbAndSqlCommands.hpp,\
    :SAPDB/DBM/Srv/Commands/Db/DBMSrvCmdDb_DbExecuteNice.hpp
DBMSrvCmdDb_DbExecuteNice.hpp inc=:SAPDB/DBM/Srv/DBMSrv_Command.hpp
DBMSrvCmdDb_DbFetch.cpp inc=\
    :SAPDB/DBM/Srv/KernelAccess/DBMSrvKnl_DbAndSqlCommands.hpp,\
    :SAPDB/DBM/Srv/Commands/Db/DBMSrvCmdDb_DbFetch.hpp
DBMSrvCmdDb_DbFetch.hpp inc=:SAPDB/DBM/Srv/DBMSrv_Command.hpp
DBMSrvCmdDb_DbFetchNice.cpp inc=\
    :SAPDB/DBM/Srv/KernelAccess/DBMSrvKnl_DbAndSqlCommands.hpp,\
    :SAPDB/DBM/Srv/Commands/Db/DBMSrvCmdDb_DbFetchNice.hpp
DBMSrvCmdDb_DbFetchNice.hpp inc=:SAPDB/DBM/Srv/DBMSrv_Command.hpp
DBMSrvCmdDb_DbOffline.cpp inc=hcn40.h,\
    :SAPDB/DBM/Srv/Commands/Db/DBMSrvCmdDb_DbOffline.hpp
DBMSrvCmdDb_DbOffline.hpp inc=:SAPDB/DBM/Srv/DBMSrv_Command.hpp
DBMSrvCmdDb_DbOnline.cpp inc=hcn40.h,\
    :SAPDB/DBM/Srv/Commands/Db/DBMSrvCmdDb_DbOnline.hpp
DBMSrvCmdDb_DbOnline.hpp inc=:SAPDB/DBM/Srv/DBMSrv_Command.hpp
DBMSrvCmdDb_DbReg.cpp inc=hsp100.h,:SAPDB/SAPDBCommon/SAPDB_string.h,\
    :SAPDB/RunTime/RTE_DBRegister.hpp,\
    :SAPDB/RunTime/RTE_UserProfileContainer.hpp,\
    :SAPDB/ToolsCommon/Tools_Arguments.hpp,\
    :SAPDB/ToolsCommon/Parsers/Utilities/ToolsParsersUtil_IdentifierCheck.hpp,\
    :SAPDB/RunTime/Configuration/RTEConf_ParameterAccess.hpp,hcn50.h,\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_ShmAdmin.hpp,\
    :SAPDB/DBM/Srv/HSS/DBMSrvHSS_Nodes.hpp,\
    :SAPDB/DBM/Srv/KernelAccess/DBMSrvKnl_State.hpp,\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_RTEError.hpp,\
    :SAPDB/DBM/Srv/User/DBMSrvUsr_Password.hpp,\
    :SAPDB/DBM/Srv/Commands/Db/DBMSrvCmdDb_DbReg.hpp
DBMSrvCmdDb_DbReg.hpp inc=:SAPDB/DBM/Srv/DBMSrv_Command.hpp
DBMSrvCmdDb_DbRegister.hpp inc=\
    :SAPDB/DBM/Srv/Commands/Db/DBMSrvCmdDb_DbReg.hpp
DBMSrvCmdDb_DbRelease.cpp inc=hcn31.h,\
    :SAPDB/DBM/Srv/KernelAccess/DBMSrvKnl_DbAndSqlCommands.hpp,\
    :SAPDB/DBM/Srv/Commands/Db/DBMSrvCmdDb_DbRelease.hpp
DBMSrvCmdDb_DbRelease.hpp inc=:SAPDB/DBM/Srv/DBMSrv_Command.hpp
DBMSrvCmdDb_DbUnreg.cpp inc=:SAPDB/RunTime/RTE_DBRegister.hpp,\
    :SAPDB/DBM/Srv/KernelAccess/DBMSrvKnl_State.hpp,\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_RTEError.hpp,\
    :SAPDB/DBM/Srv/Commands/Db/DBMSrvCmdDb_DbDrop.hpp,\
    :SAPDB/DBM/Srv/Commands/Db/DBMSrvCmdDb_DbUnreg.hpp
DBMSrvCmdDb_DbUnreg.hpp inc=:SAPDB/DBM/Srv/DBMSrv_Command.hpp
DBMSrvCmdDb_DbWarm.hpp inc=\
    :SAPDB/DBM/Srv/Commands/Db/DBMSrvCmdDb_DbOnline.hpp
DBMSrvCmdDb_Headers.hpp inc=\
    :SAPDB/DBM/Srv/Commands/Db/DBMSrvCmdDb_DbAddVolume.hpp,\
    :SAPDB/DBM/Srv/Commands/Db/DBMSrvCmdDb_DbAdmin.hpp,\
    :SAPDB/DBM/Srv/Commands/Db/DBMSrvCmdDb_DbCold.hpp,\
    :SAPDB/DBM/Srv/Commands/Db/DBMSrvCmdDb_DbConnect.hpp,\
    :SAPDB/DBM/Srv/Commands/Db/DBMSrvCmdDb_DBDeleteVolume.hpp,\
    :SAPDB/DBM/Srv/Commands/Db/DBMSrvCmdDb_DbDrop.hpp,\
    :SAPDB/DBM/Srv/Commands/Db/DBMSrvCmdDb_DbExecute.hpp,\
    :SAPDB/DBM/Srv/Commands/Db/DBMSrvCmdDb_DbExecuteNice.hpp,\
    :SAPDB/DBM/Srv/Commands/Db/DBMSrvCmdDb_DbFetch.hpp,\
    :SAPDB/DBM/Srv/Commands/Db/DBMSrvCmdDb_DbFetchNice.hpp,\
    :SAPDB/DBM/Srv/Commands/Db/DBMSrvCmdDb_DbOffline.hpp,\
    :SAPDB/DBM/Srv/Commands/Db/DBMSrvCmdDb_DbOnline.hpp,\
    :SAPDB/DBM/Srv/Commands/Db/DBMSrvCmdDb_DbRelease.hpp,\
    :SAPDB/DBM/Srv/Commands/Db/DBMSrvCmdDb_DbReg.hpp,\
    :SAPDB/DBM/Srv/Commands/Db/DBMSrvCmdDb_DbRegister.hpp,\
    :SAPDB/DBM/Srv/Commands/Db/DBMSrvCmdDb_DbWarm.hpp,\
    :SAPDB/DBM/Srv/Commands/Db/DBMSrvCmdDb_DbUnreg.hpp

