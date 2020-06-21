#
# include dependencies (do not edit)
#
DBMSrvUsr_AuthenticateIterator.cpp inc=\
    :SAPDB/DBM/Srv/Parameters/DBMSrvPar_Parameters.hpp,\
    :SAPDB/DBM/Srv/User/DBMSrvUsr_AuthenticateIterator.hpp
DBMSrvUsr_AuthenticateIterator.hpp inc=\
    :SAPDB/ToolsCommon/Tools_Array.hpp,\
    :SAPDB/ToolsCommon/Tools_DynamicUTF8String.hpp,\
    :SAPDB/RunTime/Security/RTESec_Authentication.hpp
DBMSrvUsr_Authentications.cpp inc=\
    :SAPDB/DBM/Srv/User/DBMSrvUsr_Authentications.hpp
DBMSrvUsr_Authentications.hpp inc=:SAPDB/ToolsCommon/Tools_Array.hpp,\
    :SAPDB/ToolsCommon/Tools_DynamicUTF8String.hpp,\
    :SAPDB/RunTime/Security/RTESec_Authentication.hpp
DBMSrvUsr_ChallengeResponseClient.cpp inc=hcn90c.h,hsp02.h,\
    :SAPDB/RunTime/Security/RTESec_Authentication.hpp,\
    :SAPDB/ToolsCommon/Parsers/Utilities/ToolsParsersUtil_IdentifierCheck.hpp,\
    :SAPDB/DBM/Srv/User/DBMSrvUsr_Authentications.hpp,\
    :SAPDB/DBM/Srv/User/DBMSrvUsr_ChallengeResponseClient.hpp
DBMSrvUsr_ChallengeResponseClient.hpp inc=\
    :SAPDB/ToolsCommon/Tools_DynamicUTF8String.hpp,\
    :SAPDB/DBM/Srv/User/DBMSrvUsr_Authentications.hpp,\
    :SAPDB/ToolsCommon/Tools_HexBuffer.hpp
DBMSrvUsr_ChallengeResponseServer.cpp inc=hcn20.h,hcn50.h,hcn90.h,\
    :SAPDB/RunTime/RTE_UserProfileContainer.hpp,\
    :SAPDB/ToolsCommon/Parsers/Utilities/ToolsParsersUtil_IdentifierCheck.hpp,\
    :SAPDB/DBM/Srv/User/DBMSrvUsr_ChallengeResponseServer.hpp
DBMSrvUsr_ChallengeResponseServer.hpp inc=\
    :SAPDB/ToolsCommon/Tools_DynamicUTF8String.hpp,\
    :SAPDB/DBM/Srv/User/DBMSrvUsr_AuthenticateIterator.hpp,\
    :SAPDB/ToolsCommon/Tools_HexBuffer.hpp
DBMSrvUsr_Password.cpp inc=:SAPDB/DBM/Srv/User/DBMSrvUsr_Password.hpp,\
    :SAPDB/ToolsCommon/Tools_HexBuffer.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_string.h,hsp02.h
DBMSrvUsr_Password.hpp inc=gsp00

