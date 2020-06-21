
#
# include dependencies (do not edit)
#
DBMSrvDep_DirtyReply.cpp inc=\
    :SAPDB/DBM/Srv/Deprecated/DBMSrvDep_DirtyReply.hpp
DBMSrvDep_DirtyReply.hpp inc=:SAPDB/DBM/Srv/DBMSrv_Reply.hpp

