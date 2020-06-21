# *******************************************************************************
#  module:       Cli.com
#  ------------------------------------------------------------------------------
#  responsible:  MartinR
#  special area: Database Manager Client Library
#  description:  Compilation description
#  see also:
#  ------------------------------------------------------------------------------
# *******************************************************************************
#
#
#    ========== licence begin  GPL
#    Copyright (c) 2003-2005 SAP AG
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

* -DR73
* -I$WRK/incl/SAPDB


#
# make description for non c++ files
#
dbmevtdisp.cfg  ascii  concom=yes  ->$OWN/usr/env/dbmevtdisp.cfg

#
# include dependencies (do not edit)
#
DBMCli_Autolog.cpp inc=:SAPDB/DBM/Cli/DBMCli_Database.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Autolog.hpp
DBMCli_Autolog.hpp inc=:SAPDB/DBM/Cli/DBMCli_Common.hpp
DBMCli_Backup.cpp inc=:SAPDB/DBM/Cli/DBMCli_Database.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Backup.hpp,:SAPDB/DBM/Cli/DBMCli_DateTime.hpp
DBMCli_Backup.hpp inc=:SAPDB/DBM/Cli/DBMCli_Common.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Result.hpp
DBMCli_Common.cpp inc=:SAPDB/DBM/Cli/DBMCli_Common.hpp
DBMCli_Common.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/Messages/Msg_List.hpp
DBMCli_Config.cpp inc=:SAPDB/DBM/Cli/DBMCli_Database.hpp,\
    :SAPDB/DBM/Cli/DBMCli_State.hpp
DBMCli_Config.hpp inc=:SAPDB/DBM/Cli/DBMCli_Common.hpp
DBMCli_Database.cpp inc=:SAPDB/DBM/Cli/DBMCli_Database.hpp
DBMCli_Database.hpp inc=:SAPDB/DBM/Cli/DBMCli_Common.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Node.hpp,:SAPDB/DBM/Cli/DBMCli_State.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Info.hpp,:SAPDB/DBM/Cli/DBMCli_History.hpp,\
    :SAPDB/DBM/Cli/DBMCli_File.hpp,:SAPDB/DBM/Cli/DBMCli_Show.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Parameter.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Devspace.hpp,:SAPDB/DBM/Cli/DBMCli_Media.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Backup.hpp,:SAPDB/DBM/Cli/DBMCli_Recover.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Autolog.hpp,\
    :SAPDB/DBM/Cli/DBMCli_KernelTrace.hpp,\
    :SAPDB/DBM/Cli/DBMCli_UpdStat.hpp,:SAPDB/DBM/Cli/DBMCli_Indexes.hpp,\
    :SAPDB/DBM/Cli/DBMCli_LogModeObj.hpp,:SAPDB/DBM/Cli/DBMCli_User.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Config.hpp
DBMCli_DateTime.cpp inc=:SAPDB/DBM/Cli/DBMCli_DateTime.hpp
DBMCli_DateTime.hpp inc=:SAPDB/DBM/Cli/DBMCli_Common.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Stuff.hpp
DBMCli_Devspace.cpp inc=:SAPDB/DBM/Cli/DBMCli_Database.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Devspace.hpp
DBMCli_Devspace.hpp inc=:SAPDB/DBM/Cli/DBMCli_Common.hpp
DBMCli_EnumDatabase.cpp inc=:SAPDB/DBM/Cli/DBMCli_EnumDatabase.hpp
DBMCli_EnumDatabase.hpp inc=:SAPDB/DBM/Cli/DBMCli_Common.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Stuff.hpp,:SAPDB/DBM/Cli/DBMCli_Version.hpp
DBMCli_EnumInstallation.cpp inc=\
    :SAPDB/DBM/Cli/DBMCli_EnumInstallation.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Version.hpp
DBMCli_EnumInstallation.hpp inc=:SAPDB/DBM/Cli/DBMCli_Common.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Stuff.hpp,:SAPDB/DBM/Cli/DBMCli_Version.hpp
DBMCli_File.cpp inc=:SAPDB/DBM/Cli/DBMCli_Database.hpp,\
    :SAPDB/DBM/Cli/DBMCli_File.hpp,:SAPDB/DBM/Cli/DBMCli_DateTime.hpp
DBMCli_File.hpp inc=:SAPDB/DBM/Cli/DBMCli_Common.hpp
DBMCli_History.cpp inc=:SAPDB/DBM/Cli/DBMCli_Database.hpp,\
    :SAPDB/DBM/Cli/DBMCli_History.hpp
DBMCli_History.hpp inc=:SAPDB/DBM/Cli/DBMCli_Common.hpp,\
    :SAPDB/DBM/Cli/DBMCli_DateTime.hpp,:SAPDB/DBM/Cli/DBMCli_Stuff.hpp,\
    :SAPDB/DBM/Cli/DBMCli_ResultBuf.hpp
DBMCli_Indexes.cpp inc=:SAPDB/DBM/Cli/DBMCli_Database.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Indexes.hpp
DBMCli_Indexes.hpp inc=:SAPDB/DBM/Cli/DBMCli_Common.hpp
DBMCli_Info.cpp inc=:SAPDB/DBM/Cli/DBMCli_Database.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Info.hpp
DBMCli_Info.hpp inc=:SAPDB/DBM/Cli/DBMCli_Common.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Stuff.hpp,:SAPDB/DBM/Cli/DBMCli_ResultBuf.hpp
DBMCli_KernelTrace.cpp inc=:SAPDB/DBM/Cli/DBMCli_Database.hpp,\
    :SAPDB/DBM/Cli/DBMCli_KernelTrace.hpp,\
    :SAPDB/DBM/Cli/DBMCli_DateTime.hpp
DBMCli_KernelTrace.hpp inc=:SAPDB/DBM/Cli/DBMCli_Common.hpp
DBMCli_LogFiller.cpp inc=
DBMCli_LogModeObj.cpp inc=:SAPDB/DBM/Cli/DBMCli_LogModeObj.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Database.hpp
DBMCli_LogModeObj.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Stuff.hpp,:SAPDB/DBM/Cli/DBMCli_Devspace.hpp
DBMCli_Logo.cpp inc=:SAPDB/DBM/Cli/DBMCli_Logo.hpp,hsp100.h
DBMCli_Logo.hpp inc=:SAPDB/DBM/Cli/DBMCli_Stuff.hpp
DBMCli_Media.cpp inc=:SAPDB/DBM/Cli/DBMCli_Database.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Media.hpp,:SAPDB/DBM/Cli/DBMCli_DateTime.hpp
DBMCli_Media.hpp inc=:SAPDB/DBM/Cli/DBMCli_Common.hpp
DBMCli_Node.cpp inc=:SAPDB/DBM/Cli/DBMCli_Node.hpp
DBMCli_Node.hpp inc=:SAPDB/DBM/Cli/DBMCli_Common.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Session.hpp,:SAPDB/DBM/Cli/DBMCli_NodeInfo.hpp,\
    :SAPDB/DBM/Cli/DBMCli_EnumInstallation.hpp,\
    :SAPDB/DBM/Cli/DBMCli_EnumDatabase.hpp
DBMCli_NodeInfo.cpp inc=:SAPDB/DBM/Cli/DBMCli_Node.hpp,\
    :SAPDB/DBM/Cli/DBMCli_NodeInfo.hpp
DBMCli_NodeInfo.hpp inc=:SAPDB/DBM/Cli/DBMCli_Common.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Version.hpp
DBMCli_Parameter.cpp inc=:SAPDB/DBM/Cli/DBMCli_Database.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Parameter.hpp
DBMCli_Parameter.hpp inc=:SAPDB/DBM/Cli/DBMCli_Common.hpp
DBMCli_Recover.cpp inc=:SAPDB/DBM/Cli/DBMCli_Recover.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Database.hpp
DBMCli_Recover.hpp inc=:SAPDB/DBM/Cli/DBMCli_History.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Backup.hpp,:SAPDB/DBM/Cli/DBMCli_Media.hpp
DBMCli_Result.cpp inc=:SAPDB/DBM/Cli/DBMCli_Result.hpp
DBMCli_Result.hpp inc=:SAPDB/DBM/Cli/DBMCli_Common.hpp,\
    :SAPDB/DBM/Cli/DBMCli_ResultBuf.hpp
DBMCli_ResultBuf.cpp inc=:SAPDB/DBM/Cli/DBMCli_ResultBuf.hpp
DBMCli_ResultBuf.hpp inc=:SAPDB/DBM/Cli/DBMCli_Common.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Stuff.hpp
DBMCli_Session.cpp inc=hcn14.h,:SAPDB/DBM/Cli/DBMCli_Session.hpp
DBMCli_Session.hpp inc=gsp00,:SAPDB/DBM/Cli/DBMCli_Common.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Result.hpp,:SAPDB/DBM/Cli/DBMCli_Version.hpp
DBMCli_Show.cpp inc=:SAPDB/DBM/Cli/DBMCli_Database.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Show.hpp
DBMCli_Show.hpp inc=:SAPDB/DBM/Cli/DBMCli_Common.hpp
DBMCli_State.cpp inc=:SAPDB/DBM/Cli/DBMCli_Database.hpp,\
    :SAPDB/DBM/Cli/DBMCli_State.hpp
DBMCli_State.hpp inc=:SAPDB/DBM/Cli/DBMCli_Common.hpp
DBMCli_Stuff.cpp inc=hsp77.h,:SAPDB/DBM/Cli/DBMCli_Stuff.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_string.h
DBMCli_Stuff.hpp inc=:SAPDB/DBM/Cli/DBMCli_Common.hpp,\
    :SAPDB/ToolsCommon/Tools_DynamicUTF8String.hpp
DBMCli_Test.cpp inc=:SAPDB/DBM/Cli/DBMCli_Common.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Database.hpp,:SAPDB/DBM/Cli/DBMCli_Wizard.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Version.hpp
DBMCli_UpdStat.cpp inc=:SAPDB/DBM/Cli/DBMCli_Database.hpp,\
    :SAPDB/DBM/Cli/DBMCli_UpdStat.hpp
DBMCli_UpdStat.hpp inc=:SAPDB/DBM/Cli/DBMCli_Common.hpp
DBMCli_User.cpp inc=:SAPDB/DBM/Cli/DBMCli_Database.hpp,\
    :SAPDB/DBM/Cli/DBMCli_User.hpp
DBMCli_User.hpp inc=:SAPDB/DBM/Cli/DBMCli_Common.hpp
DBMCli_Version.cpp inc=:SAPDB/DBM/Cli/DBMCli_Version.hpp
DBMCli_Version.hpp inc=:SAPDB/DBM/Cli/DBMCli_Stuff.hpp
DBMCli_Wizard.cpp inc=:SAPDB/DBM/Cli/DBMCli_Wizard.hpp
DBMCli_Wizard.hpp inc=:SAPDB/DBM/Cli/DBMCli_Common.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Stuff.hpp,:SAPDB/DBM/Cli/DBMCli_Node.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Version.hpp,:SAPDB/DBM/Cli/DBMCli_Database.hpp

