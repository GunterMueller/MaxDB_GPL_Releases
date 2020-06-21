
#
# include dependencies (do not edit)
#
DBMSrvCmd_Apropos.cpp inc=hcn90.h,\
    :SAPDB/DBM/Srv/Commands/DBMSrvCmd_Apropos.hpp,\
    :SAPDB/DBM/Srv/Commands/DBMSrvCmd_Help.hpp
DBMSrvCmd_Apropos.hpp inc=gcn00.h,:SAPDB/DBM/Srv/DBMSrv_Command.hpp
DBMSrvCmd_BackupExtIds.cpp inc=hcn32.h,\
    :SAPDB/DBM/Srv/Commands/DBMSrvCmd_BackupExtIds.hpp
DBMSrvCmd_BackupExtIds.hpp inc=:SAPDB/DBM/Srv/DBMSrv_Command.hpp
DBMSrvCmd_BackupHistory.cpp inc=\
    :SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_HistorySession.hpp,\
    :SAPDB/DBM/Srv/Commands/DBMSrvCmd_BackupHistory.hpp
DBMSrvCmd_BackupHistory.hpp inc=:SAPDB/DBM/Srv/DBMSrv_Command.hpp
DBMSrvCmd_Bye.hpp inc=:SAPDB/DBM/Srv/Commands/DBMSrvCmd_Exit.hpp
DBMSrvCmd_DbSpeed.cpp inc=\
    :SAPDB/DBM/Srv/KernelAccess/DBMSrvKnl_State.hpp,\
    :SAPDB/DBM/Srv/Commands/DBMSrvCmd_DbSpeed.hpp
DBMSrvCmd_DbSpeed.hpp inc=:SAPDB/DBM/Srv/DBMSrv_Command.hpp
DBMSrvCmd_DbState.cpp inc=\
    :SAPDB/DBM/Srv/KernelAccess/DBMSrvKnl_State.hpp,\
    :SAPDB/DBM/Srv/Commands/DBMSrvCmd_DbState.hpp
DBMSrvCmd_DbState.hpp inc=:SAPDB/DBM/Srv/DBMSrv_Command.hpp
DBMSrvCmd_DbmVersion.cpp inc=hsp100.h,\
    :SAPDB/ToolsCommon/Tools_System.hpp,\
    :SAPDB/DBM/Srv/Commands/DBMSrvCmd_DbmVersion.hpp
DBMSrvCmd_DbmVersion.hpp inc=:SAPDB/DBM/Srv/DBMSrv_Command.hpp
DBMSrvCmd_DbmVersionCommercial.cpp inc=\
    :SAPDB/DBM/Srv/Commands/DBMSrvCmd_DbmVersion.hpp
DBMSrvCmd_DbmVersionFree.cpp inc=\
    :SAPDB/DBM/Srv/Commands/DBMSrvCmd_DbmVersion.hpp
DBMSrvCmd_EventAdministrationCommand.cpp inc=\
    :SAPDB/DBM/Srv/Commands/DBMSrvCmd_EventAdministrationCommand.hpp,\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_Error.hpp,\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_LineAccessorResourceLock.hpp,\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_InternalResource.hpp
DBMSrvCmd_EventAdministrationCommand.hpp inc=\
    :SAPDB/DBM/Srv/Parameters/DBMSrvPar_Parameters.hpp,\
    :SAPDB/SAPDBCommon/DatabaseEvents/SAPDBDBE_KernelParameterNames.hpp,\
    :SAPDB/DBM/Srv/Commands/DBMSrvCmd_EventAdministrationConstants.hpp,\
    :SAPDB/DBM/Srv/DBMSrv_Command.hpp
DBMSrvCmd_EventAdministrationConstants.cpp inc=\
    :SAPDB/DBM/Srv/Commands/DBMSrvCmd_EventAdministrationConstants.hpp
DBMSrvCmd_EventAdministrationConstants.hpp inc=\
    :SAPDB/ToolsCommon/Tools_DynamicUTF8String.hpp,vsp0031
DBMSrvCmd_Exit.cpp inc=:SAPDB/DBM/Srv/Commands/DBMSrvCmd_Exit.hpp,\
    :SAPDB/DBM/Srv/Framework/DBMSrvFrm_Server.hpp
DBMSrvCmd_Exit.hpp inc=:SAPDB/DBM/Srv/DBMSrv_Command.hpp
DBMSrvCmd_Explain.cpp inc=hcn10.h,hcn36.h,hcn90.h,\
    :SAPDB/DBM/Srv/Commands/DBMSrvCmd_Explain.hpp,\
    :SAPDB/DBM/Srv/Infrastructure/Commands/DBMSrvInfrCmd_CommandFactory.hpp,\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_Error.hpp
DBMSrvCmd_Explain.hpp inc=:SAPDB/DBM/Srv/DBMSrv_Command.hpp
DBMSrvCmd_GetDefault.cpp inc=hcn10.h,hcn36.h,hcn90.h,\
    :SAPDB/DBM/Srv/Commands/DBMSrvCmd_GetDefault.hpp,\
    :SAPDB/DBM/Srv/Infrastructure/Commands/DBMSrvInfrCmd_CommandFactory.hpp,\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_Error.hpp
DBMSrvCmd_GetDefault.hpp inc=:SAPDB/DBM/Srv/DBMSrv_Command.hpp
DBMSrvCmd_Headers.hpp inc=:SAPDB/DBM/Srv/Commands/DBMSrvCmd_Bye.hpp,\
    :SAPDB/DBM/Srv/Commands/DBMSrvCmd_Exit.hpp,\
    :SAPDB/DBM/Srv/Commands/DBMSrvCmd_Quit.hpp,\
    :SAPDB/DBM/Srv/Commands/DBMSrvCmd_Release.hpp
DBMSrvCmd_Help.cpp inc=:SAPDB/DBM/Srv/DBMSrv_Reply.hpp,\
    :SAPDB/DBM/Srv/Commands/DBMSrvCmd_Help.hpp,\
    :SAPDB/DBM/Srv/Infrastructure/Commands/DBMSrvInfrCmd_CommandFactory.hpp,\
    :SAPDB/DBM/Srv/ExplainHelp/DBMSrvExpHlp_SyntaxPart.hpp,\
    :SAPDB/DBM/Srv/ExplainHelp/DBMSrvExpHlp_FuzzyStringDifference.hpp,\
    hcn10.h,hcn36.h,hcn90.h
DBMSrvCmd_Help.hpp inc=gcn00.h,:SAPDB/DBM/Srv/DBMSrv_Command.hpp
DBMSrvCmd_HssCopyFile.cpp inc=hcn42.h,\
    :SAPDB/DBM/Srv/HSS/DBMSrvHSS_Nodes.hpp,\
    :SAPDB/DBM/Srv/Commands/DBMSrvCmd_HssCopyFile.hpp
DBMSrvCmd_HssCopyFile.hpp inc=:SAPDB/DBM/Srv/DBMSrv_Command.hpp
DBMSrvCmd_InstInfo.cpp inc=:cn/buildinfo/buildinfo.h,\
    :SAPDB/DBM/Srv/Commands/DBMSrvCmd_InstInfo.hpp
DBMSrvCmd_InstInfo.hpp inc=:SAPDB/DBM/Srv/DBMSrv_Command.hpp
DBMSrvCmd_Quit.hpp inc=:SAPDB/DBM/Srv/Commands/DBMSrvCmd_Exit.hpp
DBMSrvCmd_Release.hpp inc=:SAPDB/DBM/Srv/Commands/DBMSrvCmd_Exit.hpp
DBMSrvCmd_SilentDispatcherUsingCommand.cpp inc=hcn42.h,hcn50.h,hcn51.h,\
    :SAPDB/DBM/Cli/Eventing/Dispatcher/DBMCliEvtDisp_Properties.hpp,\
    :SAPDB/DBM/Srv/Commands/Auto/DBMSrvCmdAuto_AutoExtend.hpp,\
    :SAPDB/DBM/Srv/Commands/Auto/DBMSrvCmdAuto_AutoUpdateStatistics.hpp,\
    :SAPDB/DBM/Srv/Commands/DBMSrvCmd_EventAdministrationCommand.hpp,\
    :SAPDB/DBM/Srv/Commands/DBMSrvCmd_SilentDispatcherUsingCommand.hpp,\
    :SAPDB/DBM/Srv/Parameters/DBMSrvPar_Iterator.hpp,\
    :SAPDB/DBM/Srv/Process/DBMSrvProc_ServerProcess.hpp,\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_InternalResource.hpp,\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_LineAccessorResourceLock.hpp,\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_LineIteratorEventing.hpp,\
    :SAPDB/RunTime/System/RTESys_ProcessState.hpp
DBMSrvCmd_SilentDispatcherUsingCommand.hpp inc=\
    :SAPDB/DBM/Cli/Eventing/Dispatcher/DBMCliEvtDisp_Properties.hpp,\
    :SAPDB/DBM/Srv/DBMSrv_Command.hpp,\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_Error.hpp,\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_LineAccessorResourceLock.hpp,\
    :SAPDB/ToolsCommon/Tools_DynamicUTF8String.hpp,vsp0031

