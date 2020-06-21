
#
# include dependencies (do not edit)
#
DBMSrvProc_ArgumentList.hpp inc=hni33.h
DBMSrvProc_DeamonProcess.cpp inc=\
    :SAPDB/DBM/Srv/Process/DBMSrvProc_DeamonProcess.hpp,\
    :SAPDB/RunTime/System/RTESys_ProcessState.hpp,\
    :SAPDB/ToolsCommon/Tools_CommandlineToArgumentsConverter.hpp,\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_Error.hpp
DBMSrvProc_DeamonProcess.hpp inc=\
    :SAPDB/DBM/Srv/Process/DBMSrvProc_ArgumentList.hpp,\
    :SAPDB/Messages/Msg_List.hpp
DBMSrvProc_EnvironmentVariable.cpp inc=hcn36.h,\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_ErrorHandler.hpp,\
    :SAPDB/DBM/Srv/Process/DBMSrvProc_EnvironmentVariable.hpp
DBMSrvProc_EnvironmentVariable.hpp inc=\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_Error.hpp
DBMSrvProc_IOManagedProcess.cpp inc=gcn00.h,\
    :SAPDB/DBM/Logging/DBMLog_Logger.hpp,\
    :SAPDB/DBM/Srv/Process/DBMSrvProc_IOManagedProcess.hpp,\
    :SAPDB/RunTime/RTE_ISystem.hpp,\
    :SAPDB/ToolsCommon/MemoryManagement/ToolsMeMa_GuardedOperation.hpp
DBMSrvProc_IOManagedProcess.hpp inc=hni33.h,\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_Error.hpp,\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_ErrorHandler.hpp
DBMSrvProc_ProcessData.cpp inc=\
    :SAPDB/DBM/Srv/Process/DBMSrvProc_ProcessData.hpp,\
    :SAPDB/RunTime/System/RTESys_ProcessState.hpp
DBMSrvProc_ProcessData.hpp inc=\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_Error.hpp
DBMSrvProc_ServerProcess.cpp inc=gcn00.h,hcn50.h,heo01.h,\
    :SAPDB/DBM/Cli/DBMCli_Database.hpp,:SAPDB/DBM/Cli/DBMCli_Stuff.hpp,\
    :SAPDB/DBM/Srv/Process/DBMSrvProc_ServerProcess.hpp,\
    :SAPDB/DBM/Srv/Commands/Dbm/DBMSrvCmdDbm_DbmExecuteServerprocess.hpp,\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_Error.hpp
DBMSrvProc_ServerProcess.hpp inc=\
    :SAPDB/DBM/Srv/Process/DBMSrvProc_ArgumentList.hpp,\
    :SAPDB/DBM/Srv/Process/DBMSrvProc_ServerProcessCommandLineConverter.hpp
DBMSrvProc_ServerProcessCommandLineConverter.cpp inc=heo01.h,\
    :SAPDB/DBM/Srv/Process/DBMSrvProc_ServerProcessCommandLineConverter.hpp,\
    :SAPDB/ToolsCommon/CString/ToolsCStr_Str.hpp
DBMSrvProc_ServerProcessCommandLineConverter.hpp inc=\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_Error.hpp

