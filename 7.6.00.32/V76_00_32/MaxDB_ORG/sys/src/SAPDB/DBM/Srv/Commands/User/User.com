#
# include dependencies (do not edit)
#
DBMSrvCmdUsr_Headers.hpp inc=\
    :SAPDB/DBM/Srv/Commands/User/DBMSrvCmdUsr_UserGetChallenge.hpp,\
    :SAPDB/DBM/Srv/Commands/User/DBMSrvCmdUsr_UserRename.hpp,\
    :SAPDB/DBM/Srv/Commands/User/DBMSrvCmdUsr_UserResponse.hpp
DBMSrvCmdUsr_UserGetChallenge.cpp inc=\
    :SAPDB/DBM/Srv/User/DBMSrvUsr_ChallengeResponseServer.hpp,\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_Error.hpp,\
    :SAPDB/DBM/Srv/Commands/User/DBMSrvCmdUsr_UserGetChallenge.hpp
DBMSrvCmdUsr_UserGetChallenge.hpp inc=:SAPDB/DBM/Srv/DBMSrv_Command.hpp
DBMSrvCmdUsr_UserRename.cpp inc=hcn50.h,\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_Error.hpp,\
    :SAPDB/DBM/Srv/Commands/User/DBMSrvCmdUsr_UserRename.hpp
DBMSrvCmdUsr_UserRename.hpp inc=:SAPDB/DBM/Srv/DBMSrv_Command.hpp
DBMSrvCmdUsr_UserResponse.cpp inc=hcn20.h,hcn50.h,hcn51.h,hcn53.h,\
    :SAPDB/DBM/Srv/HSS/DBMSrvHSS_Nodes.hpp,\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_Error.hpp,\
    :SAPDB/DBM/Srv/User/DBMSrvUsr_ChallengeResponseServer.hpp,\
    :SAPDB/DBM/Srv/Commands/User/DBMSrvCmdUsr_UserResponse.hpp
DBMSrvCmdUsr_UserResponse.hpp inc=:SAPDB/DBM/Srv/DBMSrv_Command.hpp

