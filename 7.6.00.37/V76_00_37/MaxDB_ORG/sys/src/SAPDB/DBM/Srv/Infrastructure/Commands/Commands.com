#    ========== licence begin  GPL
#    Copyright (c) 2004-2005 SAP AG
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
DBMSrvInfrCmd_ClassicCommand.hpp inc=gcn00.h,\
    :SAPDB/DBM/Srv/DBMSrv_Command.hpp,\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_Error.hpp
DBMSrvInfrCmd_CommandFactory.cpp inc=\
    :SAPDB/DBM/Srv/Commands/DBMSrvCmd_Headers.hpp,\
    :SAPDB/DBM/Srv/Commands/Auto/DBMSrvCmdAuto_Headers.hpp,\
    :SAPDB/DBM/Srv/Commands/Backup/DBMSrvCmdB_Headers.hpp,\
    :SAPDB/DBM/Srv/Commands/Db/DBMSrvCmdDb_Headers.hpp,\
    :SAPDB/DBM/Srv/Commands/Dbm/DBMSrvCmdDbm_Headers.hpp,\
    :SAPDB/DBM/Srv/Commands/Dbmevtdisp/Internal/DBMSrvCmdEvtdispInt_Headers.hpp,\
    :SAPDB/DBM/Srv/Commands/Medium/DBMSrvCmdMedium_Headers.hpp,\
    :SAPDB/DBM/Srv/Commands/SQL/DBMSrvCmdSQL_Headers.hpp,\
    :SAPDB/DBM/Srv/Commands/User/DBMSrvCmdUsr_Headers.hpp,\
    :SAPDB/DBM/Srv/Infrastructure/Commands/DBMSrvInfrCmd_CommandFactory.hpp,\
    :SAPDB/ToolsCommon/Tools_List.hpp
DBMSrvInfrCmd_CommandFactory.hpp inc=:SAPDB/ToolsCommon/Tools_List.hpp,\
    :SAPDB/DBM/Srv/DBMSrv_Command.hpp,\
    :SAPDB/DBM/Srv/Message/DBMSrvMsg_Error.hpp

