# *******************************************************************************
#  module:       Web.com
#  ------------------------------------------------------------------------------
#  responsible:  BerndV
#  special area: Database Manager WebAgent Extension
#  description:  Compilation description for DBMWeb
#  see also:
#  ------------------------------------------------------------------------------
# *******************************************************************************
#
#
#    ========== licence begin  GPL
#    Copyright (c) 2000-2005 SAP AG
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

&if $OS in [ WIN32 ]
* -FR    # MSDEV source browser
&endif
* -DR73
* -I$WRK/incl/SAPDB

#
# include dependencies (do not edit)
#
DBMWeb_DBMWeb.cpp inc=:SAPDB/DBM/Web/DBMWeb_DBMWeb.hpp,\
    :SAPDB/DBM/Web/DBMWeb_Web.hpp,\
    :SAPDB/DBM/Web/DBMWeb_TemplateMsgBox.hpp
DBMWeb_DBMWeb.hpp inc=gsp00,hwd02waapi.h,\
    :SAPDB/DBM/Cli/DBMCli_Database.hpp,:SAPDB/DBM/Cli/DBMCli_Wizard.hpp,\
    :SAPDB/DBM/Web/DBMWeb_DBMWebCore.hpp,\
    :SAPDB/DBM/Web/DBMWeb_DBMWebTest.hpp,\
    :SAPDB/DBM/Web/DBMWeb_DBMWebState.hpp,\
    :SAPDB/DBM/Web/DBMWeb_DBMWebInfo.hpp,\
    :SAPDB/DBM/Web/DBMWeb_DBMWebBackup.hpp,\
    :SAPDB/DBM/Web/DBMWeb_DBMWebRecover.hpp,\
    :SAPDB/DBM/Web/DBMWeb_DBMWebTuning.hpp,\
    :SAPDB/DBM/Web/DBMWeb_DBMWebCheck.hpp,\
    :SAPDB/DBM/Web/DBMWeb_DBMWebConfig.hpp,\
    :SAPDB/DBM/Web/DBMWeb_DBMWebWizard.hpp
DBMWeb_DBMWebBackup.cpp inc=:SAPDB/DBM/Web/DBMWeb_TemplateBackup.hpp,\
    :SAPDB/DBM/Web/DBMWeb_TemplateRecovery.hpp,\
    :SAPDB/DBM/Web/DBMWeb_TemplateMsgBox.hpp,\
    :SAPDB/DBM/Web/DBMWeb_DBMWeb.hpp,:SAPDB/DBM/Web/DBMWeb_Web.hpp
DBMWeb_DBMWebBackup.hpp inc=
DBMWeb_DBMWebCheck.cpp inc=:SAPDB/DBM/Web/DBMWeb_TemplateVerify.hpp,\
    :SAPDB/DBM/Web/DBMWeb_TemplateFiles.hpp,\
    :SAPDB/DBM/Web/DBMWeb_TemplateFile.hpp,\
    :SAPDB/DBM/Web/DBMWeb_TemplateShows.hpp,\
    :SAPDB/DBM/Web/DBMWeb_TemplateShow.hpp,\
    :SAPDB/DBM/Web/DBMWeb_TemplateCommand.hpp,\
    :SAPDB/DBM/Web/DBMWeb_TemplateKernelTrace.hpp,\
    :SAPDB/DBM/Web/DBMWeb_DBMWeb.hpp,:SAPDB/DBM/Web/DBMWeb_Web.hpp,\
    :SAPDB/DBM/Web/DBMWeb_TemplateMsgBox.hpp
DBMWeb_DBMWebCheck.hpp inc=
DBMWeb_DBMWebConfig.cpp inc=:SAPDB/DBM/Web/DBMWeb_TemplateParams.hpp,\
    :SAPDB/DBM/Web/DBMWeb_TemplateParam.hpp,\
    :SAPDB/DBM/Web/DBMWeb_TemplateDevspaces.hpp,\
    :SAPDB/DBM/Web/DBMWeb_TemplateDevspace.hpp,\
    :SAPDB/DBM/Web/DBMWeb_TemplateMedia.hpp,\
    :SAPDB/DBM/Web/DBMWeb_TemplateMedium.hpp,\
    :SAPDB/DBM/Web/DBMWeb_TemplateParmed.hpp,\
    :SAPDB/DBM/Web/DBMWeb_TemplateSysTabs.hpp,\
    :SAPDB/DBM/Web/DBMWeb_TemplateLogMode.hpp,\
    :SAPDB/DBM/Web/DBMWeb_TemplateUsers.hpp,\
    :SAPDB/DBM/Web/DBMWeb_TemplateUser.hpp,\
    :SAPDB/DBM/Web/DBMWeb_TemplateMsgBox.hpp,\
    :SAPDB/DBM/Web/DBMWeb_DBMWeb.hpp,:SAPDB/DBM/Web/DBMWeb_Web.hpp
DBMWeb_DBMWebConfig.hpp inc=
DBMWeb_DBMWebCore.cpp inc=hsp100.h,:SAPDB/DBM/Web/DBMWeb_DBMWeb.hpp,\
    :SAPDB/DBM/Web/DBMWeb_Web.hpp,\
    :SAPDB/DBM/Web/DBMWeb_TemplateMsgBox.hpp,\
    :SAPDB/DBM/Web/DBMWeb_TemplateFrame.hpp,\
    :SAPDB/DBM/Web/DBMWeb_TemplateHeader.hpp,\
    :SAPDB/DBM/Web/DBMWeb_TemplateLogon.hpp,\
    :SAPDB/DBM/Web/DBMWeb_TemplateLogoff.hpp,\
    :SAPDB/DBM/Web/DBMWeb_TemplateDatabases.hpp,\
    :SAPDB/DBM/Web/DBMWeb_TemplateMenu.hpp,\
    :SAPDB/DBM/Web/DBMWeb_TemplateWizMenu.hpp,\
    :SAPDB/DBM/Web/DBMWeb_TemplateSize.hpp,\
    :SAPDB/DBM/Web/DBMWeb_TemplateVersion.hpp
DBMWeb_DBMWebCore.hpp inc=
DBMWeb_DBMWebInfo.cpp inc=:SAPDB/DBM/Web/DBMWeb_TemplateInfoTable.hpp,\
    :SAPDB/DBM/Web/DBMWeb_TemplateHistory.hpp,\
    :SAPDB/DBM/Web/DBMWeb_DBMWeb.hpp,:SAPDB/DBM/Web/DBMWeb_Web.hpp
DBMWeb_DBMWebInfo.hpp inc=
DBMWeb_DBMWebRecover.cpp inc=\
    :SAPDB/DBM/Web/DBMWeb_TemplateBadDevspaces.hpp,\
    :SAPDB/DBM/Web/DBMWeb_DBMWeb.hpp,:SAPDB/DBM/Web/DBMWeb_Web.hpp
DBMWeb_DBMWebRecover.hpp inc=
DBMWeb_DBMWebState.cpp inc=:SAPDB/DBM/Web/DBMWeb_TemplateState.hpp,\
    :SAPDB/DBM/Web/DBMWeb_DBMWeb.hpp,:SAPDB/DBM/Web/DBMWeb_Web.hpp
DBMWeb_DBMWebState.hpp inc=
DBMWeb_DBMWebTest.cpp inc=\
    :SAPDB/ToolsCommon/Tools_TemplateSimpleTable.hpp,\
    :SAPDB/DBM/Web/DBMWeb_DBMWeb.hpp,:SAPDB/DBM/Web/DBMWeb_Web.hpp
DBMWeb_DBMWebTest.hpp inc=
DBMWeb_DBMWebTuning.cpp inc=:SAPDB/DBM/Web/DBMWeb_TemplateUpdStat.hpp,\
    :SAPDB/DBM/Web/DBMWeb_TemplateIndexes.hpp,\
    :SAPDB/DBM/Web/DBMWeb_DBMWeb.hpp,:SAPDB/DBM/Web/DBMWeb_Web.hpp
DBMWeb_DBMWebTuning.hpp inc=
DBMWeb_DBMWebWizard.cpp inc=:SAPDB/DBM/Web/DBMWeb_TemplateWizard.hpp,\
    :SAPDB/DBM/Web/DBMWeb_TemplateMsgBox.hpp,\
    :SAPDB/DBM/Web/DBMWeb_DBMWeb.hpp,:SAPDB/DBM/Web/DBMWeb_Web.hpp
DBMWeb_DBMWebWizard.hpp inc=
DBMWeb_Service.cpp inc=gsp00,geo47.h,:SAPDB/DBM/Web/DBMWeb_DBMWeb.hpp,\
    :SAPDB/DBM/Web/DBMWeb_TemplateMsgBox.hpp,\
    :SAPDB/ToolsCommon/Tools_TemplateMsgBox.hpp,\
    :SAPDB/ToolsCommon/Tools_Session.hpp
DBMWeb_TemplateBackup.cpp inc=:SAPDB/DBM/Web/DBMWeb_TemplateBackup.hpp,\
    :SAPDB/DBM/Cli/DBMCli_DateTime.hpp
DBMWeb_TemplateBackup.hpp inc=:SAPDB/ToolsCommon/Tools_Template.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Database.hpp,:SAPDB/DBM/Cli/DBMCli_Backup.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Media.hpp
DBMWeb_TemplateBadDevspaces.cpp inc=\
    :SAPDB/DBM/Web/DBMWeb_TemplateBadDevspaces.hpp
DBMWeb_TemplateBadDevspaces.hpp inc=\
    :SAPDB/ToolsCommon/Tools_Template.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Database.hpp
DBMWeb_TemplateCommand.cpp inc=:SAPDB/DBM/Web/DBMWeb_TemplateCommand.hpp
DBMWeb_TemplateCommand.hpp inc=:SAPDB/ToolsCommon/Tools_Template.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Database.hpp
DBMWeb_TemplateDatabases.cpp inc=\
    :SAPDB/DBM/Web/DBMWeb_TemplateDatabases.hpp
DBMWeb_TemplateDatabases.hpp inc=:SAPDB/ToolsCommon/Tools_Template.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Node.hpp
DBMWeb_TemplateDevspace.cpp inc=\
    :SAPDB/DBM/Web/DBMWeb_TemplateDevspace.hpp
DBMWeb_TemplateDevspace.hpp inc=:SAPDB/ToolsCommon/Tools_Template.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Database.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Devspace.hpp
DBMWeb_TemplateDevspaces.cpp inc=\
    :SAPDB/DBM/Web/DBMWeb_TemplateDevspaces.hpp
DBMWeb_TemplateDevspaces.hpp inc=:SAPDB/ToolsCommon/Tools_Template.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Database.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Devspace.hpp
DBMWeb_TemplateFile.cpp inc=:SAPDB/DBM/Web/DBMWeb_TemplateFile.hpp
DBMWeb_TemplateFile.hpp inc=:SAPDB/ToolsCommon/Tools_Template.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Database.hpp,:SAPDB/DBM/Cli/DBMCli_File.hpp
DBMWeb_TemplateFiles.cpp inc=:SAPDB/DBM/Web/DBMWeb_TemplateFiles.hpp
DBMWeb_TemplateFiles.hpp inc=:SAPDB/ToolsCommon/Tools_Template.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Database.hpp,:SAPDB/DBM/Cli/DBMCli_File.hpp
DBMWeb_TemplateFrame.cpp inc=:SAPDB/DBM/Web/DBMWeb_TemplateFrame.hpp
DBMWeb_TemplateFrame.hpp inc=:SAPDB/ToolsCommon/Tools_Template.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Stuff.hpp
DBMWeb_TemplateHeader.cpp inc=:SAPDB/DBM/Web/DBMWeb_TemplateHeader.hpp
DBMWeb_TemplateHeader.hpp inc=:SAPDB/ToolsCommon/Tools_Template.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Stuff.hpp
DBMWeb_TemplateHistory.cpp inc=:SAPDB/DBM/Web/DBMWeb_TemplateHistory.hpp
DBMWeb_TemplateHistory.hpp inc=:SAPDB/ToolsCommon/Tools_Template.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Database.hpp,:SAPDB/DBM/Cli/DBMCli_History.hpp
DBMWeb_TemplateIndexes.cpp inc=:SAPDB/DBM/Web/DBMWeb_TemplateIndexes.hpp
DBMWeb_TemplateIndexes.hpp inc=:SAPDB/ToolsCommon/Tools_Template.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Database.hpp
DBMWeb_TemplateInfoTable.cpp inc=\
    :SAPDB/DBM/Web/DBMWeb_TemplateInfoTable.hpp
DBMWeb_TemplateInfoTable.hpp inc=:SAPDB/ToolsCommon/Tools_Template.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Database.hpp,:SAPDB/DBM/Cli/DBMCli_Info.hpp
DBMWeb_TemplateKernelTrace.cpp inc=\
    :SAPDB/DBM/Web/DBMWeb_TemplateKernelTrace.hpp
DBMWeb_TemplateKernelTrace.hpp inc=:SAPDB/ToolsCommon/Tools_Template.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Database.hpp,\
    :SAPDB/DBM/Cli/DBMCli_KernelTrace.hpp
DBMWeb_TemplateLogMode.cpp inc=:SAPDB/DBM/Web/DBMWeb_TemplateLogMode.hpp
DBMWeb_TemplateLogMode.hpp inc=:SAPDB/ToolsCommon/Tools_Template.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Database.hpp
DBMWeb_TemplateLogoff.cpp inc=:SAPDB/DBM/Web/DBMWeb_TemplateLogoff.hpp
DBMWeb_TemplateLogoff.hpp inc=:SAPDB/ToolsCommon/Tools_Template.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Stuff.hpp
DBMWeb_TemplateLogon.cpp inc=:SAPDB/DBM/Web/DBMWeb_TemplateLogon.hpp
DBMWeb_TemplateLogon.hpp inc=:SAPDB/ToolsCommon/Tools_Template.hpp,\
    :SAPDB/Messages/Msg_List.hpp,:SAPDB/DBM/Cli/DBMCli_Stuff.hpp
DBMWeb_TemplateMedia.cpp inc=:SAPDB/DBM/Web/DBMWeb_TemplateMedia.hpp
DBMWeb_TemplateMedia.hpp inc=:SAPDB/ToolsCommon/Tools_Template.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Database.hpp,:SAPDB/DBM/Cli/DBMCli_Media.hpp
DBMWeb_TemplateMedium.cpp inc=:SAPDB/DBM/Web/DBMWeb_TemplateMedium.hpp
DBMWeb_TemplateMedium.hpp inc=:SAPDB/ToolsCommon/Tools_Template.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Database.hpp,:SAPDB/DBM/Cli/DBMCli_Media.hpp
DBMWeb_TemplateMenu.cpp inc=:SAPDB/DBM/Web/DBMWeb_TemplateMenu.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Stuff.hpp
DBMWeb_TemplateMenu.hpp inc=:SAPDB/ToolsCommon/Tools_Template.hpp
DBMWeb_TemplateMsgBox.cpp inc=:SAPDB/DBM/Web/DBMWeb_TemplateMsgBox.hpp
DBMWeb_TemplateMsgBox.hpp inc=:SAPDB/ToolsCommon/Tools_Template.hpp,\
    :SAPDB/Messages/Msg_List.hpp,:SAPDB/DBM/Cli/DBMCli_Stuff.hpp
DBMWeb_TemplateParam.cpp inc=:SAPDB/DBM/Web/DBMWeb_TemplateParam.hpp
DBMWeb_TemplateParam.hpp inc=:SAPDB/ToolsCommon/Tools_Template.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Database.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Parameter.hpp
DBMWeb_TemplateParams.cpp inc=:SAPDB/DBM/Web/DBMWeb_TemplateParams.hpp
DBMWeb_TemplateParams.hpp inc=:SAPDB/ToolsCommon/Tools_Template.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Database.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Parameter.hpp
DBMWeb_TemplateParmed.cpp inc=:SAPDB/DBM/Web/DBMWeb_TemplateParmed.hpp
DBMWeb_TemplateParmed.hpp inc=:SAPDB/ToolsCommon/Tools_Template.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Database.hpp,:SAPDB/DBM/Cli/DBMCli_Media.hpp
DBMWeb_TemplateRecovery.cpp inc=\
    :SAPDB/DBM/Web/DBMWeb_TemplateRecovery.hpp,\
    :SAPDB/DBM/Cli/DBMCli_DateTime.hpp
DBMWeb_TemplateRecovery.hpp inc=:SAPDB/ToolsCommon/Tools_Template.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Database.hpp
DBMWeb_TemplateShow.cpp inc=:SAPDB/DBM/Web/DBMWeb_TemplateShow.hpp
DBMWeb_TemplateShow.hpp inc=:SAPDB/ToolsCommon/Tools_Template.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Database.hpp,:SAPDB/DBM/Cli/DBMCli_File.hpp
DBMWeb_TemplateShows.cpp inc=:SAPDB/DBM/Web/DBMWeb_TemplateShows.hpp
DBMWeb_TemplateShows.hpp inc=:SAPDB/ToolsCommon/Tools_Template.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Database.hpp,:SAPDB/DBM/Cli/DBMCli_Show.hpp
DBMWeb_TemplateSize.cpp inc=:SAPDB/DBM/Web/DBMWeb_TemplateSize.hpp
DBMWeb_TemplateSize.hpp inc=:SAPDB/ToolsCommon/Tools_Template.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Stuff.hpp
DBMWeb_TemplateState.cpp inc=:SAPDB/DBM/Web/DBMWeb_TemplateState.hpp
DBMWeb_TemplateState.hpp inc=:SAPDB/ToolsCommon/Tools_Template.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Database.hpp,:SAPDB/DBM/Cli/DBMCli_State.hpp
DBMWeb_TemplateSysTabs.cpp inc=:SAPDB/DBM/Web/DBMWeb_TemplateSysTabs.hpp
DBMWeb_TemplateSysTabs.hpp inc=:SAPDB/ToolsCommon/Tools_Template.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Database.hpp
DBMWeb_TemplateUpdStat.cpp inc=:SAPDB/DBM/Web/DBMWeb_TemplateUpdStat.hpp
DBMWeb_TemplateUpdStat.hpp inc=:SAPDB/ToolsCommon/Tools_Template.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Database.hpp
DBMWeb_TemplateUser.cpp inc=:SAPDB/DBM/Web/DBMWeb_TemplateUser.hpp
DBMWeb_TemplateUser.hpp inc=:SAPDB/ToolsCommon/Tools_Template.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Database.hpp,:SAPDB/DBM/Cli/DBMCli_User.hpp
DBMWeb_TemplateUsers.cpp inc=:SAPDB/DBM/Web/DBMWeb_TemplateUsers.hpp
DBMWeb_TemplateUsers.hpp inc=:SAPDB/ToolsCommon/Tools_Template.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Database.hpp,:SAPDB/DBM/Cli/DBMCli_User.hpp
DBMWeb_TemplateVerify.cpp inc=:SAPDB/DBM/Web/DBMWeb_TemplateVerify.hpp
DBMWeb_TemplateVerify.hpp inc=:SAPDB/ToolsCommon/Tools_Template.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Database.hpp
DBMWeb_TemplateVersion.cpp inc=:SAPDB/DBM/Web/DBMWeb_TemplateVersion.hpp
DBMWeb_TemplateVersion.hpp inc=:SAPDB/ToolsCommon/Tools_Template.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Stuff.hpp
DBMWeb_TemplateWizMenu.cpp inc=:SAPDB/DBM/Web/DBMWeb_TemplateWizMenu.hpp
DBMWeb_TemplateWizMenu.hpp inc=:SAPDB/ToolsCommon/Tools_Template.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Wizard.hpp
DBMWeb_TemplateWizard.cpp inc=:SAPDB/DBM/Web/DBMWeb_TemplateWizard.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Parameter.hpp
DBMWeb_TemplateWizard.hpp inc=:SAPDB/ToolsCommon/Tools_Template.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Wizard.hpp
DBMWeb_Web.hpp inc=

