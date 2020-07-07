
#
# include dependencies (do not edit)
#
DBMSrvInfrReply_Reply2MsgList.cpp inc=hcn36.h,\
    :SAPDB/DBM/Srv/Infrastructure/Reply/DBMSrvInfrReply_Reply2MsgList.hpp,\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_SQLError.hpp
DBMSrvInfrReply_Reply2MsgList.hpp inc=:SAPDB/DBM/Srv/DBMSrv_Reply.hpp,\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_Error.hpp
DBMSrvInfrReply_ReplyBuffer.cpp inc=hcn36.h,\
    :SAPDB/DBM/Srv/Infrastructure/Reply/DBMSrvInfrReply_ReplyBuffer.hpp,\
    :SAPDB/DBM/Srv/Infrastructure/Reply/DBMSrvInfrReply_Reply2MsgList.hpp
DBMSrvInfrReply_ReplyBuffer.hpp inc=\
    :SAPDB/DBM/Srv/Infrastructure/Reply/DBMSrvInfrReply_Reply2MsgList.hpp
DBMSrvInfrReply_ReplyBufferWritingIntoMsgList.cpp inc=hcn36.h,\
    :SAPDB/DBM/Srv/Infrastructure/Reply/DBMSrvInfrReply_ReplyBufferWritingIntoMsgList.hpp
DBMSrvInfrReply_ReplyBufferWritingIntoMsgList.hpp inc=\
    :SAPDB/DBM/Srv/Infrastructure/Reply/DBMSrvInfrReply_ReplyBuffer.hpp

