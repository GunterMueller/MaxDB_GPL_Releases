
#
# include dependencies (do not edit)
#
#    ========== licence begin  GPL
#    Copyright (c) 2006 SAP AG
#
#    This program is free software; you can redistribute it and/or
#    modify it under the terms of the GNU General Public License
#    as published by the Free Software Foundation; either version 2
#    of the License, or (at your option) any later version.
#    
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#    
#    You should have received a copy of the GNU General Public License
#    along with this program; if not, write to the Free Software
#    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
#    ========== licence end

DBMSrvPar_Iterator.cpp inc=\
    :SAPDB/DBM/Srv/Parameters/DBMSrvPar_Iterator.hpp
DBMSrvPar_Iterator.hpp inc=\
    :SAPDB/DBM/Srv/Parameters/DBMSrvPar_Parameters.hpp
DBMSrvPar_Parameter.cpp inc=\
    :SAPDB/DBM/Srv/Parameters/DBMSrvPar_Parameter.hpp
DBMSrvPar_Parameter.hpp inc=gcn002.h,\
    :SAPDB/ToolsCommon/Tools_DynamicUTF8String.hpp
DBMSrvPar_Parameters.cpp inc=\
    :SAPDB/DBM/Srv/Infrastructure/Reply/DBMSrvInfrReply_ReplyBufferWritingIntoMsgList.hpp,\
    :SAPDB/DBM/Srv/Parameters/DBMSrvPar_Parameters.hpp,hcn20.h,hcn29.h
DBMSrvPar_Parameters.hpp inc=\
    :SAPDB/DBM/Srv/Parameters/DBMSrvPar_Parameter.hpp,\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_Error.hpp,\
    :SAPDB/ToolsCommon/Tools_Array.hpp
DBMSrvPar_RTEConfParameter.cpp inc=\
    :SAPDB/DBM/Srv/Parameters/DBMSrvPar_RTEConfParameter.hpp,\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_Error.hpp,\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_RTEError.hpp,\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_LineAccessorResourceLock.hpp,\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_InternalResource.hpp,\
    :SAPDB/KernelCommon/ParameterNames/KernelParam_RunTime.h
DBMSrvPar_RTEConfParameter.hpp inc=\
    :SAPDB/RunTime/Configuration/RTEConf_ParameterAccess.hpp,\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_Error.hpp

