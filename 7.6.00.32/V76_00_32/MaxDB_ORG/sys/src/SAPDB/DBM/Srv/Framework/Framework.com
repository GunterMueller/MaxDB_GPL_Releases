
#
# include dependencies (do not edit)
#
DBMSrvFrm_ExceptionHandler.cpp inc=hcn42.h,heo670.h,\
    :SAPDB/DBM/Srv/Framework/DBMSrvFrm_ExceptionHandler.hpp,\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_ErrorHandler.hpp,\
    :SAPDB/RunTime/System/RTESys_SecurityAttributes.h,\
    :SAPDB/RunTime/System/RTESys_AtomicOperation.h,\
    :SAPDB/RunTime/System/RTESys_ProcessID.hpp
DBMSrvFrm_ExceptionHandler.hpp inc=gsp00,\
    :SAPDB/DBM/Srv/IO/DBMSrvIO_SharedTextFile.hpp,\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_Error.hpp
DBMSrvFrm_Server.cpp inc=hcn20.h,hcn31.h,hcn32.h,hcn40.h,hcn50.h,\
    :SAPDB/DBM/Logging/DBMLog_Logger.hpp,\
    :SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_HistorySession.hpp,\
    :SAPDB/DBM/Srv/Framework/DBMSrvFrm_Server.hpp,\
    :SAPDB/DBM/Srv/KernelAccess/DBMSrvKnl_GlobalSessions.hpp,\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_ShmAdmin.hpp,\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_RTEError.hpp,\
    :SAPDB/RunTime/RTE_UNIXAuthenticate.hpp
DBMSrvFrm_Server.hpp inc=\
    :SAPDB/DBM/Srv/Framework/DBMSrvFrm_ExceptionHandler.hpp,\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_Error.hpp,\
    :SAPDB/RunTime/System/RTESys_Time.h

