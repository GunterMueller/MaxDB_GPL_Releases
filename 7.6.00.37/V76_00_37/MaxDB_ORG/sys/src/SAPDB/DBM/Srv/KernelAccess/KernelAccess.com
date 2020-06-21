
#
# include dependencies (do not edit)
#
DBMSrvKnl_DbAndSqlCommands.cpp inc=hcn50.h,hcn53.h,hcn90.h,\
    :SAPDB/DBM/Srv/KernelAccess/DBMSrvKnl_GlobalSessions.hpp,\
    :SAPDB/DBM/Srv/KernelAccess/DBMSrvKnl_DbAndSqlCommands.hpp
DBMSrvKnl_DbAndSqlCommands.hpp inc=:SAPDB/DBM/Srv/DBMSrv_Command.hpp,\
    :SAPDB/DBM/Srv/KernelAccess/DBMSrvKnl_GlobalSessions.hpp
DBMSrvKnl_GlobalSessions.cpp inc=hcn31.h,hcn50.h,hcn90.h,vsp0031,\
    :SAPDB/DBM/Srv/KernelAccess/DBMSrvKnl_GlobalSessions.hpp,\
    :SAPDB/RunTime/RTE_DBRegister.hpp
DBMSrvKnl_GlobalSessions.hpp inc=\
    :SAPDB/DBM/Srv/KernelAccess/DBMSrvKnl_Session.hpp
DBMSrvKnl_SQLField.cpp inc=\
    :SAPDB/DBM/Srv/KernelAccess/DBMSrvKnl_SQLField.hpp
DBMSrvKnl_SQLField.hpp inc=vsp001,\
    :SAPDB/ToolsCommon/Tools_DynamicUTF8String.hpp
DBMSrvKnl_SQLRecord.cpp inc=\
    :SAPDB/DBM/Srv/KernelAccess/DBMSrvKnl_Session.hpp
DBMSrvKnl_SQLRecord.hpp inc=vsp001,\
    :SAPDB/ToolsCommon/Tools_DynamicUTF8String.hpp
DBMSrvKnl_Session.cpp inc=\
    :SAPDB/ToolsCommon/Parsers/Utilities/ToolsParsersUtil_IdentifierCheck.hpp,\
    heo02.h,heo03.h,hsp02.h,:SAPDB/PacketInterface/PIn_Packet.h,\
    :SAPDB/Messages/Msg_List.hpp,\
    :SAPDB/PacketInterface/PIn_SpecialParts.h,\
    :SAPDB/DBM/Srv/KernelAccess/DBMSrvKnl_Session.hpp,\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_RTEError.hpp,\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_SQLError.hpp,\
    :SAPDB/RunTime/System/RTESys_Time.h
DBMSrvKnl_Session.hpp inc=:SAPDB/DBM/Srv/Message/DBMSrvMsg_Error.hpp,\
    :SAPDB/DBM/Srv/KernelAccess/DBMSrvKnl_State.hpp,\
    :SAPDB/DBM/Srv/KernelAccess/DBMSrvKnl_SQLRecord.hpp,\
    :SAPDB/DBM/Srv/KernelAccess/DBMSrvKnl_SQLField.hpp
DBMSrvKnl_State.cpp inc=:SAPDB/RunTime/RTE_DBRegister.hpp,\
    :SAPDB/DBM/Srv/KernelAccess/DBMSrvKnl_State.hpp,\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_RTEError.hpp,\
    :SAPDB/DBM/Logging/DBMLog_Logger.hpp
DBMSrvKnl_State.hpp inc=geo00_2.h,\
    :SAPDB/ToolsCommon/Tools_DynamicUTF8String.hpp,\
    :SAPDB/RunTime/RTE_ActiveDatabase.hpp,\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_Error.hpp

