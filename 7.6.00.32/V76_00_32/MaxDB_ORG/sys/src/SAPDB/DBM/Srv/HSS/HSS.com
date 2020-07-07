
#
# include dependencies (do not edit)
#
#    ========== licence begin  GPL
#    Copyright (c) 2005 SAP AG
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
#    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#    ========== licence end

#
# include dependencies (do not edit)
#
DBMSrvHSS_Node.cpp inc=:SAPDB/DBM/Srv/HSS/DBMSrvHSS_Node.hpp,\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_Error.hpp
DBMSrvHSS_Node.hpp inc=:SAPDB/DBM/Cli/DBMCli_Database.hpp,\
    :SAPDB/DBM/Srv/Commands/DBMSrvCmd_HssCopyFile.hpp
DBMSrvHSS_Nodes.cpp inc=\
    :SAPDB/KernelCommon/ParameterNames/KernelParam_RunTime.h,\
    :SAPDB/DBM/Srv/Parameters/DBMSrvPar_RTEConfParameter.hpp,hcn42.h,\
    :SAPDB/DBM/Srv/HSS/DBMSrvHSS_Nodes.hpp,\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_Error.hpp
DBMSrvHSS_Nodes.hpp inc=:SAPDB/ToolsCommon/Tools_DynamicUTF8String.hpp,\
    :SAPDB/ToolsCommon/Tools_List.hpp,\
    :SAPDB/DBM/Srv/HSS/DBMSrvHSS_Node.hpp

