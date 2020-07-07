
#
# include dependencies (do not edit)
#
DBMSrvMsg_Base.hpp inc=
DBMSrvMsg_CompatibilityArgument.cpp inc=\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_CompatibilityArgument.hpp
DBMSrvMsg_CompatibilityArgument.hpp inc=:SAPDB/Messages/Msg_Arg.hpp
DBMSrvMsg_Error.cpp inc=:SAPDB/DBM/Srv/DBMSrv_Reply.hpp,\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_Error.hpp
DBMSrvMsg_Error.hpp inc=:SAPDB/DBM/Srv/Message/DBMSrvMsg_Base.hpp,\
    :SAPDB/Messages/Msg_List.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_DBMSrv.h,gcn00.h
DBMSrvMsg_ErrorHandler.cpp inc=\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_Error.hpp,\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_ErrorHandler.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_string.h
DBMSrvMsg_ErrorHandler.hpp inc=hni35.h,:SAPDB/Messages/Msg_List.hpp
DBMSrvMsg_Info.cpp inc=:SAPDB/DBM/Srv/Message/DBMSrvMsg_Info.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_DBMSrv.h
DBMSrvMsg_Info.hpp inc=:SAPDB/Messages/Msg_List.hpp,\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_Base.hpp
DBMSrvMsg_NewRTEError.cpp inc=:SAPDB/DBM/Srv/DBMSrv_Reply.hpp,\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_NewRTEError.hpp
DBMSrvMsg_NewRTEError.hpp inc=:SAPDB/DBM/Srv/Message/DBMSrvMsg_Error.hpp,\
    gsp01.h
DBMSrvMsg_RTEError.cpp inc=:SAPDB/DBM/Srv/Message/DBMSrvMsg_RTEError.hpp,\
    :SAPDB/DBM/Srv/Message/Util/DBMSrvMsgUtil_RTEErrorText.hpp
DBMSrvMsg_RTEError.hpp inc=:SAPDB/DBM/Srv/Message/DBMSrvMsg_Error.hpp,\
    gsp01.h,gsp05,gsp09.h
DBMSrvMsg_SQLError.cpp inc=:SAPDB/DBM/Srv/DBMSrv_Reply.hpp,\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_SQLError.hpp
DBMSrvMsg_SQLError.hpp inc=:SAPDB/DBM/Srv/Message/DBMSrvMsg_RTEError.hpp
DBMSrvMsg_Warning.cpp inc=:SAPDB/DBM/Srv/Message/DBMSrvMsg_Warning.hpp
DBMSrvMsg_Warning.hpp inc=:SAPDB/Messages/Msg_List.hpp,\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_Base.hpp

