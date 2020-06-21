#    ========== licence begin  GPL
#    Copyright (c) 2005-2006 SAP AG
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
DBMSrvShMAdm_Main.cpp inc=\
    :SAPDB/DBM/Srv/SharedMemory/Admin/DBMSrvShMAdm_Manager.hpp
DBMSrvShMAdm_Manager.cpp inc=heo01.h,heo06.h,\
    :SAPDB/DBM/Srv/Infrastructure/Reply/DBMSrvInfrReply_ReplyBuffer.hpp,\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_RTEError.hpp,\
    :SAPDB/DBM/Srv/SharedMemory/Admin/DBMSrvShMAdm_Manager.hpp,\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_BaseUnlocker.hpp,\
    :SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_Persistence.hpp,\
    :SAPDB/RunTime/RTE_Database.hpp,:SAPDB/RunTime/RTE_DBRegister.hpp,\
    :SAPDB/ToolsCommon/Parsers/Old/ToolsParsersOld_ProgramParameterParser.hpp
DBMSrvShMAdm_Manager.hpp inc=:SAPDB/DBM/Srv/Message/DBMSrvMsg_Error.hpp,\
    :SAPDB/ToolsCommon/Tools_DynamicUTF8String.hpp
DBMSrvShMAdm_Resource.rc inc=gsp100.h,:SAPDB/DBM/DBM_Resource.rc

