
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

DBMSrvCmdBHist_BackupHistory.cpp inc=\
    :SAPDB/DBM/Srv/Commands/DBMSrvCmd_BackupHistory.hpp
DBMSrvCmdBHist_BackupHistory.hpp inc=
DBMSrvCmdBHist_BackupHistoryAppend.cpp inc=hcn36.h,hcn42.h,hcn90.h,\
    :SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_Part.hpp,\
    :SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_FileWriterEBF.hpp,\
    :SAPDB/DBM/Srv/Commands/BackupHistory/DBMSrvCmdBHist_BackupHistoryAppend.hpp,\
    :SAPDB/ToolsCommon/Tools_List.hpp
DBMSrvCmdBHist_BackupHistoryAppend.hpp inc=\
    :SAPDB/DBM/Srv/DBMSrv_Command.hpp

