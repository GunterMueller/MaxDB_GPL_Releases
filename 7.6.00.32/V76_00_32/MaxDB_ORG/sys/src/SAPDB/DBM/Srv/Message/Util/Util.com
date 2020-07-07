
#
# include dependencies (do not edit)
#
DBMSrvMsgUtil_RTEErrorID.cpp inc=\
    :SAPDB/DBM/Srv/Message/Util/DBMSrvMsgUtil_RTEErrorID.hpp
DBMSrvMsgUtil_RTEErrorID.hpp inc=\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_Error.hpp,\
    :SAPDB/ToolsCommon/CString/ToolsCStr_Str.hpp
DBMSrvMsgUtil_RTEErrorIDMapper.cpp inc=\
    :SAPDB/DBM/Srv/Message/Util/DBMSrvMsgUtil_RTEErrorIDMapper.hpp
DBMSrvMsgUtil_RTEErrorIDMapper.hpp inc=gsp00
DBMSrvMsgUtil_RTEErrorText.cpp inc=\
    :SAPDB/DBM/Srv/Message/Util/DBMSrvMsgUtil_RTEErrorText.hpp
DBMSrvMsgUtil_RTEErrorText.hpp inc=\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_RTEError.hpp,\
    :SAPDB/ToolsCommon/CString/ToolsCStr_Str.hpp,gsp00_2.h

