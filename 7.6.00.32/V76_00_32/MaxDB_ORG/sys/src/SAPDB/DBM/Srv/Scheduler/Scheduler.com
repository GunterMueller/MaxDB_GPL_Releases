
#
# include dependencies (do not edit)
#
DBMSrvScd_Job.cpp inc=hcn50.h,:SAPDB/DBM/Srv/Scheduler/DBMSrvScd_Job.hpp
DBMSrvScd_Job.hpp inc=:SAPDB/DBM/Srv/Scheduler/DBMSrvScd_Plan.hpp,\
    :SAPDB/DBM/Srv/Process/DBMSrvProc_IOManagedProcess.hpp,\
    :SAPDB/ToolsCommon/Tools_List.hpp,\
    :SAPDB/ToolsCommon/Tools_DynamicUTF8String.hpp
DBMSrvScd_Main.cpp inc=hcn42.h,heo670.h,\
    :SAPDB/DBM/Logging/DBMLog_Logger.hpp,\
    :SAPDB/DBM/Logging/DBMLog_LogFile.hpp,\
    :SAPDB/DBM/Logging/DBMLog_SynchronizedLogFile.hpp,\
    :SAPDB/DBM/Srv/Scheduler/DBMSrvScd_Scheduler.hpp,\
    :SAPDB/ToolsCommon/Tools_PipeCall.hpp
DBMSrvScd_Persistor.cpp inc=\
    :SAPDB/DBM/Srv/Scheduler/DBMSrvScd_Persistor.hpp
DBMSrvScd_Persistor.hpp inc=:SAPDB/DBM/Srv/Scheduler/DBMSrvScd_Job.hpp,\
    :SAPDB/ToolsCommon/Tools_Properties.hpp
DBMSrvScd_Plan.cpp inc=:SAPDB/DBM/Srv/Scheduler/DBMSrvScd_Plan.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_string.h
DBMSrvScd_Plan.hpp inc=
DBMSrvScd_Resource.rc inc=gsp100.h,:SAPDB/DBM/DBM_Resource.rc
DBMSrvScd_Scheduler.cpp inc=hcn50.h,hni35.h,\
    :SAPDB/DBM/Srv/Scheduler/DBMSrvScd_Scheduler.hpp,\
    :SAPDB/RunTime/RTE_ISystem.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_DBMScd.h
DBMSrvScd_Scheduler.hpp inc=:SAPDB/DBM/Logging/DBMLog_Logger.hpp,\
    :SAPDB/DBM/Srv/Scheduler/DBMSrvScd_Persistor.hpp,\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_LineAccessorScheduler.hpp

