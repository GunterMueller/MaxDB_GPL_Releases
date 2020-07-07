
#
# include dependencies (do not edit)
#
DBMSrvCmdScd_Header.hpp inc=\
    :SAPDB/DBM/Srv/Commands/Scheduler/DBMSrvCmdScd_SchedulerActivateJob.hpp,\
    :SAPDB/DBM/Srv/Commands/Scheduler/DBMSrvCmdScd_SchedulerDeactivateJob.hpp,\
    :SAPDB/DBM/Srv/Commands/Scheduler/DBMSrvCmdScd_SchedulerDeleteJob.hpp,\
    :SAPDB/DBM/Srv/Commands/Scheduler/DBMSrvCmdScd_SchedulerState.hpp,\
    :SAPDB/DBM/Srv/Commands/Scheduler/DBMSrvCmdScd_SchedulerListJobs.hpp,\
    :SAPDB/DBM/Srv/Commands/Scheduler/DBMSrvCmdScd_SchedulerCreateJob.hpp,\
    :SAPDB/DBM/Srv/Commands/Scheduler/DBMSrvCmdScd_SchedulerStart.hpp,\
    :SAPDB/DBM/Srv/Commands/Scheduler/DBMSrvCmdScd_SchedulerStop.hpp,\
    :SAPDB/DBM/Srv/Commands/Scheduler/DBMSrvCmdScd_SchedulerShowJoblog.hpp,\
    :SAPDB/DBM/Srv/Commands/Scheduler/DBMSrvCmdScd_SchedulerDeleteJoblog.hpp
DBMSrvCmdScd_SchedulerActivateJob.cpp inc=\
    :SAPDB/DBM/Srv/Commands/Scheduler/DBMSrvCmdScd_SchedulerActivateJob.hpp,\
    :SAPDB/DBM/Srv/Commands/Scheduler/DBMSrvCmdScd_SchedulerState.hpp,\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_LineAccessorScheduler.hpp,\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_Error.hpp
DBMSrvCmdScd_SchedulerActivateJob.hpp inc=\
    :SAPDB/DBM/Srv/Commands/Scheduler/DBMSrvCmdScd_SchedulerCommand.hpp
DBMSrvCmdScd_SchedulerCommand.cpp inc=hcn50.h,\
    :SAPDB/DBM/Srv/Commands/Scheduler/DBMSrvCmdScd_SchedulerCommand.hpp,\
    :SAPDB/DBM/Srv/Process/DBMSrvProc_ArgumentList.hpp,\
    :SAPDB/DBM/Srv/Process/DBMSrvProc_ServerProcess.hpp,\
    :SAPDB/DBM/Srv/Process/DBMSrvProc_ServerProcessCommandLineConverter.hpp,\
    :SAPDB/DBM/Srv/Scheduler/DBMSrvScd_Job.hpp,\
    :SAPDB/DBM/Srv/Scheduler/DBMSrvScd_Plan.hpp,\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_LineAccessorResourceLock.hpp,\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_InternalResource.hpp,\
    :SAPDB/RunTime/System/RTESys_ProcessState.hpp,\
    :SAPDB/ToolsCommon/Tools_Arguments.hpp,\
    :SAPDB/ToolsCommon/Tools_DynamicUTF8String.hpp
DBMSrvCmdScd_SchedulerCommand.hpp inc=:SAPDB/DBM/Srv/DBMSrv_Command.hpp,\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_LineAccessorScheduler.hpp,\
    :SAPDB/ToolsCommon/Tools_DynamicUTF8String.hpp
DBMSrvCmdScd_SchedulerCreateJob.cpp inc=hcn50.h,\
    :SAPDB/DBM/Srv/Commands/Scheduler/DBMSrvCmdScd_SchedulerCreateJob.hpp,\
    :SAPDB/DBM/Srv/Commands/Scheduler/DBMSrvCmdScd_SchedulerState.hpp,\
    :SAPDB/DBM/Srv/Scheduler/DBMSrvScd_Job.hpp,\
    :SAPDB/DBM/Srv/Scheduler/DBMSrvScd_Plan.hpp,\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_LineAccessorScheduler.hpp,\
    :SAPDB/ToolsCommon/Tools_Arguments.hpp,\
    :SAPDB/ToolsCommon/Tools_DynamicUTF8String.hpp
DBMSrvCmdScd_SchedulerCreateJob.hpp inc=\
    :SAPDB/DBM/Srv/Commands/Scheduler/DBMSrvCmdScd_SchedulerCommand.hpp,\
    :SAPDB/ToolsCommon/Tools_DynamicUTF8String.hpp
DBMSrvCmdScd_SchedulerDeactivateJob.cpp inc=\
    :SAPDB/DBM/Srv/Commands/Scheduler/DBMSrvCmdScd_SchedulerDeactivateJob.hpp,\
    :SAPDB/DBM/Srv/Commands/Scheduler/DBMSrvCmdScd_SchedulerState.hpp,\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_LineAccessorScheduler.hpp,\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_Error.hpp
DBMSrvCmdScd_SchedulerDeactivateJob.hpp inc=\
    :SAPDB/DBM/Srv/Commands/Scheduler/DBMSrvCmdScd_SchedulerCommand.hpp
DBMSrvCmdScd_SchedulerDeleteJob.cpp inc=\
    :SAPDB/DBM/Srv/Commands/Scheduler/DBMSrvCmdScd_SchedulerDeleteJob.hpp,\
    :SAPDB/DBM/Srv/Commands/Scheduler/DBMSrvCmdScd_SchedulerState.hpp,\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_LineAccessorScheduler.hpp,\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_Error.hpp
DBMSrvCmdScd_SchedulerDeleteJob.hpp inc=\
    :SAPDB/DBM/Srv/Commands/Scheduler/DBMSrvCmdScd_SchedulerCommand.hpp
DBMSrvCmdScd_SchedulerDeleteJoblog.cpp inc=\
    :SAPDB/DBM/Srv/Commands/Scheduler/DBMSrvCmdScd_SchedulerDeleteJoblog.hpp
DBMSrvCmdScd_SchedulerDeleteJoblog.hpp inc=\
    :SAPDB/DBM/Srv/DBMSrv_Command.hpp
DBMSrvCmdScd_SchedulerListJobs.cpp inc=hcn50.h,\
    :SAPDB/DBM/Srv/Commands/Scheduler/DBMSrvCmdScd_SchedulerListJobs.hpp,\
    :SAPDB/DBM/Srv/Commands/Scheduler/DBMSrvCmdScd_SchedulerState.hpp,\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_LineAccessorScheduler.hpp
DBMSrvCmdScd_SchedulerListJobs.hpp inc=\
    :SAPDB/DBM/Srv/Commands/Scheduler/DBMSrvCmdScd_SchedulerCommand.hpp,\
    :SAPDB/ToolsCommon/Tools_DynamicUTF8String.hpp
DBMSrvCmdScd_SchedulerShowJoblog.cpp inc=\
    :SAPDB/DBM/Srv/Commands/Scheduler/DBMSrvCmdScd_SchedulerShowJoblog.hpp
DBMSrvCmdScd_SchedulerShowJoblog.hpp inc=\
    :SAPDB/DBM/Srv/DBMSrv_Command.hpp
DBMSrvCmdScd_SchedulerStart.cpp inc=\
    :SAPDB/DBM/Srv/Commands/Scheduler/DBMSrvCmdScd_SchedulerStart.hpp,\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_LineAccessorScheduler.hpp
DBMSrvCmdScd_SchedulerStart.hpp inc=\
    :SAPDB/DBM/Srv/Commands/Scheduler/DBMSrvCmdScd_SchedulerCommand.hpp,\
    :SAPDB/ToolsCommon/Tools_DynamicUTF8String.hpp
DBMSrvCmdScd_SchedulerState.cpp inc=\
    :SAPDB/DBM/Srv/Commands/Scheduler/DBMSrvCmdScd_SchedulerState.hpp,\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_LineAccessorScheduler.hpp,\
    :SAPDB/RunTime/System/RTESys_ProcessState.hpp
DBMSrvCmdScd_SchedulerState.hpp inc=\
    :SAPDB/DBM/Srv/Commands/Scheduler/DBMSrvCmdScd_SchedulerCommand.hpp
DBMSrvCmdScd_SchedulerStop.cpp inc=\
    :SAPDB/DBM/Srv/Commands/Scheduler/DBMSrvCmdScd_SchedulerStop.hpp,\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_LineAccessorScheduler.hpp
DBMSrvCmdScd_SchedulerStop.hpp inc=\
    :SAPDB/DBM/Srv/Commands/Scheduler/DBMSrvCmdScd_SchedulerCommand.hpp,\
    :SAPDB/ToolsCommon/Tools_DynamicUTF8String.hpp

