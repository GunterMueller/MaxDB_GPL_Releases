
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

DBMCliEvtHndl_Base.cpp inc=\
    :SAPDB/Messages/Msg_List.hpp,\
    :SAPDB/ToolsCommon/Tools_ParameterParser.hpp,\
    :SAPDB/DBM/Cli/Eventing/Handler/DBMCliEvtHndl_Base.hpp,\
    :SAPDB/DBM/Cli/Eventing/Common/DBMCliEvtCm_Event.hpp
DBMCliEvtHndl_Base.hpp inc=\
    :SAPDB/Messages/Msg_List.hpp,\
    :SAPDB/ToolsCommon/Tools_ParameterParser.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Database.hpp,\
    :SAPDB/DBM/Cli/Eventing/Common/DBMCliEvtCm_Event.hpp
DBMCliEvtHndl_DisplayEventResource.rc inc=gsp100.h dep=DBM_Resource.rc
DBMCliEvtHndl_MainDisplayEvent.cpp inc=\
    :SAPDB/ToolsCommon/Tools_ParameterParser.hpp,\
    :SAPDB/ToolsCommon/Tools_PipeCall.hpp,\
    :SAPDB/DBM/Cli/Eventing/Handler/DBMCliEvtHndl_Base.hpp,\
    :SAPDB/DBM/Cli/Eventing/Common/DBMCliEvtCm_Branding.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Logo.hpp
DBMCliEvtHndl_MainWinLogger.cpp inc=\
    :SAPDB/ToolsCommon/Tools_ParameterParser.hpp,\
    :SAPDB/ToolsCommon/Tools_PipeCall.hpp,\
    :SAPDB/ToolsCommon/Tools_EventLogFile.hpp,\
    :SAPDB/ToolsCommon/Tools_DynamicUTF8String.hpp,\
    :SAPDB/DBM/Cli/Eventing/Handler/DBMCliEvtHndl_Base.hpp,\
    :SAPDB/DBM/Cli/Eventing/Common/DBMCliEvtCm_Branding.hpp,\
    :SAPDB/DBM/Cli/DBMCli_Logo.hpp
DBMCliEvtHndl_WinLoggerResource.rc inc=gsp100.h dep=DBM_Resource.rc
