
#
# include dependencies (do not edit)
#
DBMLog_LogFile.cpp inc=:SAPDB/SAPDBCommon/SAPDB_string.h,\
    :SAPDB/DBM/Logging/DBMLog_LogFile.hpp
DBMLog_LogFile.hpp inc=hin105.h
DBMLog_Logger.cpp inc=:SAPDB/DBM/Logging/DBMLog_Logger.hpp,\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_Error.hpp,\
    :SAPDBCommon/Messages/SDBMsg_DBMLog.h,\
    :SAPDB/SAPDBCommon/SAPDB_string.h
DBMLog_Logger.hpp inc=hin100.h,:SAPDB/DBM/Srv/Message/DBMSrvMsg_Info.hpp,\
    :SAPDB/DBM/Logging/DBMLog_LogFile.hpp,:SAPDB/Messages/Msg_List.hpp,\
    :SAPDB/ToolsCommon/Tools_Vector.hpp
DBMLog_SynchronizedLogFile.cpp inc=\
    :SAPDB/DBM/Logging/DBMLog_SynchronizedLogFile.hpp,\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_LineAccessorResourceLock.hpp,\
    gcn002.h
DBMLog_SynchronizedLogFile.hpp inc=:SAPDB/DBM/Logging/DBMLog_LogFile.hpp

