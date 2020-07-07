
#
# include dependencies (do not edit)
#
DBMSrvCmdDbm_DbmConfigGet.cpp inc=hcn51.h,hcn90.h,\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_Error.hpp,\
    :SAPDB/DBM/Srv/Commands/Dbm/DBMSrvCmdDbm_DbmConfigGet.hpp
DBMSrvCmdDbm_DbmConfigGet.hpp inc=:SAPDB/DBM/Srv/DBMSrv_Command.hpp
DBMSrvCmdDbm_DbmConfigSet.cpp inc=hcn51.h,hcn90.h,\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_Error.hpp,\
    :SAPDB/DBM/Srv/Commands/Dbm/DBMSrvCmdDbm_DbmConfigSet.hpp
DBMSrvCmdDbm_DbmConfigSet.hpp inc=:SAPDB/DBM/Srv/DBMSrv_Command.hpp
DBMSrvCmdDbm_DbmExecuteServerprocess.cpp inc=hni33.h,\
    :SAPDB/DBM/Srv/Commands/Dbm/DBMSrvCmdDbm_DbmExecuteServerprocess.hpp,\
    :SAPDB/DBM/Srv/Process/DBMSrvProc_DeamonProcess.hpp,\
    :SAPDB/ToolsCommon/Tools_PipeCall.hpp,\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_Error.hpp
DBMSrvCmdDbm_DbmExecuteServerprocess.hpp inc=\
    :SAPDB/DBM/Srv/DBMSrv_Command.hpp,\
    :SAPDB/DBM/Srv/Process/DBMSrvProc_ServerProcessCommandLineConverter.hpp
DBMSrvCmdDbm_DbmGetmemoryusage.cpp inc=\
    :SAPDB/DBM/Srv/Commands/Dbm/DBMSrvCmdDbm_DbmGetmemoryusage.hpp,\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_Error.hpp,\
    :SAPDB/DBM/Srv/Process/DBMSrvProc_ProcessData.hpp,\
    :SAPDB/RunTime/System/RTESys_ProcessID.hpp
DBMSrvCmdDbm_DbmGetmemoryusage.hpp inc=:SAPDB/DBM/Srv/DBMSrv_Command.hpp
DBMSrvCmdDbm_DbmGetpid.cpp inc=\
    :SAPDB/RunTime/System/RTESys_ProcessID.hpp,\
    :SAPDB/DBM/Srv/Commands/Dbm/DBMSrvCmdDbm_DbmGetpid.hpp
DBMSrvCmdDbm_DbmGetpid.hpp inc=:SAPDB/DBM/Srv/DBMSrv_Command.hpp
DBMSrvCmdDbm_DbmListMemoryerror.cpp inc=hsp77.h,\
    :SAPDB/DBM/Srv/Commands/Dbm/DBMSrvCmdDbm_DbmListMemoryerror.hpp,\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_Info.hpp
DBMSrvCmdDbm_DbmListMemoryerror.hpp inc=\
    :SAPDB/DBM/Srv/DBMSrv_Command.hpp,\
    :SAPDB/ToolsCommon/MemoryManagement/ToolsMeMa_GuardedOperation.hpp
DBMSrvCmdDbm_Headers.hpp inc=\
    :SAPDB/DBM/Srv/Commands/Dbm/DBMSrvCmdDbm_DbmConfigGet.hpp,\
    :SAPDB/DBM/Srv/Commands/Dbm/DBMSrvCmdDbm_DbmConfigSet.hpp,\
    :SAPDB/DBM/Srv/Commands/Dbm/DBMSrvCmdDbm_DbmExecuteServerprocess.hpp,\
    :SAPDB/DBM/Srv/Commands/Dbm/DBMSrvCmdDbm_DbmGetmemoryusage.hpp,\
    :SAPDB/DBM/Srv/Commands/Dbm/DBMSrvCmdDbm_DbmGetpid.hpp,\
    :SAPDB/DBM/Srv/Commands/Dbm/DBMSrvCmdDbm_DbmListMemoryerror.hpp

