
#
# include dependencies (do not edit)
#
DBMSrvCmdAuto_AutoExtend.cpp inc=\
    :SAPDB/DBM/Srv/Commands/Auto/DBMSrvCmdAuto_AutoExtend.hpp,\
    :SAPDB/DBM/Srv/KernelAccess/DBMSrvKnl_State.hpp,\
    :SAPDB/DBM/VersionInfo/DBMVI_Strings.hpp,hcn50.h,hcn51.h
DBMSrvCmdAuto_AutoExtend.hpp inc=\
    :SAPDB/DBM/Srv/Commands/DBMSrvCmd_SilentDispatcherUsingCommand.hpp
DBMSrvCmdAuto_AutoRecover.hpp inc=\
    :SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_RestorePlanner.hpp,\
    :SAPDB/DBM/Srv/Infrastructure/Commands/DBMSrvInfrCmd_ClassicCommand.hpp
DBMSrvCmdAuto_AutoUpdateStatistics.cpp inc=vsp0031,hcn50.h,hcn51.h,\
    :SAPDB/DBM/Srv/Commands/Auto/DBMSrvCmdAuto_AutoUpdateStatistics.hpp,\
    :SAPDB/DBM/Srv/Commands/SQL/DBMSrvCmdSQL_SqlUpdatestatPerSystemtable.hpp,\
    :SAPDB/DBM/Srv/KernelAccess/DBMSrvKnl_State.hpp,\
    :SAPDB/DBM/VersionInfo/DBMVI_Strings.hpp
DBMSrvCmdAuto_AutoUpdateStatistics.hpp inc=\
    :SAPDB/DBM/Srv/Commands/DBMSrvCmd_SilentDispatcherUsingCommand.hpp
DBMSrvCmdAuto_Headers.hpp inc=\
    :SAPDB/DBM/Srv/Commands/Auto/DBMSrvCmdAuto_AutoExtend.hpp,\
    :SAPDB/DBM/Srv/Commands/Auto/DBMSrvCmdAuto_AutoRecover.hpp,\
    :SAPDB/DBM/Srv/Commands/Auto/DBMSrvCmdAuto_AutoUpdateStatistics.hpp

