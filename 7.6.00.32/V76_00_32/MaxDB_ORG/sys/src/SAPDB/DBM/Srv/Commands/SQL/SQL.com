
#
# include dependencies (do not edit)
#
DBMSrvCmdSQL_Headers.hpp inc=\
    :SAPDB/DBM/Srv/Commands/SQL/DBMSrvCmdSQL_SqlConnect.hpp,\
    :SAPDB/DBM/Srv/Commands/SQL/DBMSrvCmdSQL_SqlExecute.hpp,\
    :SAPDB/DBM/Srv/Commands/SQL/DBMSrvCmdSQL_SqlFetch.hpp,\
    :SAPDB/DBM/Srv/Commands/SQL/DBMSrvCmdSQL_SqlExecuteNice.hpp,\
    :SAPDB/DBM/Srv/Commands/SQL/DBMSrvCmdSQL_SqlFetchNice.hpp,\
    :SAPDB/DBM/Srv/Commands/SQL/DBMSrvCmdSQL_SqlInfo.hpp,\
    :SAPDB/DBM/Srv/Commands/SQL/DBMSrvCmdSQL_SqlRelease.hpp,\
    :SAPDB/DBM/Srv/Commands/SQL/DBMSrvCmdSQL_SqlUpdatestat.hpp,\
    :SAPDB/DBM/Srv/Commands/SQL/DBMSrvCmdSQL_SqlUpdatestatPerSystemtable.hpp
DBMSrvCmdSQL_SqlConnect.cpp inc=\
    :SAPDB/DBM/Srv/KernelAccess/DBMSrvKnl_DbAndSqlCommands.hpp,\
    :SAPDB/DBM/Srv/Commands/SQL/DBMSrvCmdSQL_SqlConnect.hpp
DBMSrvCmdSQL_SqlConnect.hpp inc=:SAPDB/DBM/Srv/DBMSrv_Command.hpp
DBMSrvCmdSQL_SqlExecute.cpp inc=\
    :SAPDB/DBM/Srv/KernelAccess/DBMSrvKnl_DbAndSqlCommands.hpp,\
    :SAPDB/DBM/Srv/Commands/SQL/DBMSrvCmdSQL_SqlExecute.hpp
DBMSrvCmdSQL_SqlExecute.hpp inc=:SAPDB/DBM/Srv/DBMSrv_Command.hpp
DBMSrvCmdSQL_SqlExecuteNice.cpp inc=\
    :SAPDB/DBM/Srv/KernelAccess/DBMSrvKnl_DbAndSqlCommands.hpp,\
    :SAPDB/DBM/Srv/Commands/SQL/DBMSrvCmdSQL_SqlExecuteNice.hpp
DBMSrvCmdSQL_SqlExecuteNice.hpp inc=:SAPDB/DBM/Srv/DBMSrv_Command.hpp
DBMSrvCmdSQL_SqlFetch.cpp inc=\
    :SAPDB/DBM/Srv/KernelAccess/DBMSrvKnl_DbAndSqlCommands.hpp,\
    :SAPDB/DBM/Srv/Commands/SQL/DBMSrvCmdSQL_SqlFetch.hpp
DBMSrvCmdSQL_SqlFetch.hpp inc=:SAPDB/DBM/Srv/DBMSrv_Command.hpp
DBMSrvCmdSQL_SqlFetchNice.cpp inc=\
    :SAPDB/DBM/Srv/KernelAccess/DBMSrvKnl_DbAndSqlCommands.hpp,\
    :SAPDB/DBM/Srv/Commands/SQL/DBMSrvCmdSQL_SqlFetchNice.hpp
DBMSrvCmdSQL_SqlFetchNice.hpp inc=:SAPDB/DBM/Srv/DBMSrv_Command.hpp
DBMSrvCmdSQL_SqlInfo.cpp inc=\
    :SAPDB/DBM/Srv/KernelAccess/DBMSrvKnl_DbAndSqlCommands.hpp,\
    :SAPDB/DBM/Srv/Commands/SQL/DBMSrvCmdSQL_SqlInfo.hpp
DBMSrvCmdSQL_SqlInfo.hpp inc=:SAPDB/DBM/Srv/DBMSrv_Command.hpp
DBMSrvCmdSQL_SqlRelease.cpp inc=\
    :SAPDB/DBM/Srv/KernelAccess/DBMSrvKnl_DbAndSqlCommands.hpp,\
    :SAPDB/DBM/Srv/Commands/SQL/DBMSrvCmdSQL_SqlRelease.hpp
DBMSrvCmdSQL_SqlRelease.hpp inc=:SAPDB/DBM/Srv/DBMSrv_Command.hpp
DBMSrvCmdSQL_SqlUpdatestat.cpp inc=\
    :SAPDB/DBM/Srv/Commands/SQL/DBMSrvCmdSQL_SqlUpdatestat.hpp,\
    :SAPDB/DBM/Srv/KernelAccess/DBMSrvKnl_Session.hpp,\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_Error.hpp,\
    :SAPDB/ToolsCommon/Tools_DynamicUTF8String.hpp,hcn50.h
DBMSrvCmdSQL_SqlUpdatestat.hpp inc=:SAPDB/DBM/Srv/DBMSrv_Command.hpp
DBMSrvCmdSQL_SqlUpdatestatPerSystemtable.cpp inc=\
    :SAPDB/DBM/Srv/Commands/SQL/DBMSrvCmdSQL_SqlUpdatestatPerSystemtable.hpp,\
    :SAPDB/DBM/Srv/KernelAccess/DBMSrvKnl_Session.hpp,\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_Error.hpp,\
    :SAPDB/DBM/Srv/Process/DBMSrvProc_ServerProcess.hpp,\
    :SAPDB/ToolsCommon/Tools_DynamicUTF8String.hpp,hcn50.h
DBMSrvCmdSQL_SqlUpdatestatPerSystemtable.hpp inc=\
    :SAPDB/DBM/Srv/DBMSrv_Command.hpp

