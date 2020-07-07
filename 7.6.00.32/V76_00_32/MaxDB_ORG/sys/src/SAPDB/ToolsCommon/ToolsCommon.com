# *******************************************************************************
#
#  module:       Tools.com
#
#  ------------------------------------------------------------------------------
#
#  responsible:  Bernd Vorsprach (D025588 - mailto:Bernd.Vorsprach@sap.com)
#
#  special area: SAPDB Tools Extensions
#
#  description:  Compilation description for Tools
#
#  see also:     
#
#  ------------------------------------------------------------------------------
#
#                         Copyright (c) 1998-2005 SAP AG
#
# *******************************************************************************
#
#
#    ========== licence begin  GPL
#    Copyright (c) 1998-2005 SAP AG
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

&if $OSSPEC == LINUX && $MACH == IA64 && $COMPILER != ICC
regex(\w+\.cpp) -OPT=-O0
&endif

&if $OS in [ WIN32 ]
* -FR    # MSDEV source browser
&endif
* -I$WRK/incl/SAPDB 

&if $OS in [ WIN32 ]
Tools_WinEvtLog_bin.mc  binary
Tools_WinEvtLog.rc inc=Tools_WinEvtLog_bin.mc
&endif

#
# include dependencies (do not edit)
#
Tools_Arguments.cpp inc=:SAPDB/ToolsCommon/Tools_Arguments.hpp
Tools_Arguments.hpp inc=:SAPDB/ToolsCommon/Tools_DynamicUTF8String.hpp,\
    :SAPDB/ToolsCommon/Tools_Array.hpp
Tools_Array.hpp inc=
Tools_CommandlineToArgumentsConverter.cpp inc=\
    :SAPDB/SAPDBCommon/SAPDB_string.h,\
    :SAPDB/ToolsCommon/Tools_CommandlineToArgumentsConverter.hpp
Tools_CommandlineToArgumentsConverter.hpp inc=
Tools_ConfigFile.cpp inc=:SAPDB/ToolsCommon/Tools_System.hpp,\
    :SAPDB/ToolsCommon/Tools_ConfigFile.hpp
Tools_ConfigFile.hpp inc=:SAPDB/Messages/Msg_List.hpp,\
    :SAPDB/ToolsCommon/Tools_DynamicUTF8String.hpp,\
    :SAPDB/ToolsCommon/Tools_Object.hpp
Tools_DynamicUTF8String.cpp inc=\
    :SAPDB/ToolsCommon/Tools_UnicodeCaseTable.hpp,\
    :SAPDB/ToolsCommon/Tools_DynamicUTF8String.hpp
Tools_DynamicUTF8String.hpp inc=\
    :SAPDB/ToolsCommon/Tools_DynamicUTF8StringBuffer.hpp
Tools_DynamicUTF8StringBuffer.hpp inc=:SAPDB/SAPDBCommon/SAPDB_string.h,\
    :SAPDB/ToolsCommon/Tools_UTF8StringElement.hpp
Tools_EventLogFile.cpp inc=heo02.h,\
    :SAPDB/ToolsCommon/Tools_EventLogFile.hpp,\
    :SAPDB/ToolsCommon/Tools_DynamicUTF8String.hpp,\
    :SAPDB/ToolsCommon/Tools_WinEvtLog.h
Tools_EventLogFile.hpp inc=hin105.h,\
    :SAPDB/ToolsCommon/Tools_DynamicUTF8String.hpp
Tools_Expression.cpp inc=:SAPDB/ToolsCommon/Tools_Expression.hpp
Tools_Expression.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/ToolsCommon/Tools_DynamicUTF8String.hpp,\
    :SAPDB/ToolsCommon/Tools_Array.hpp
Tools_Extest.cpp inc=:SAPDB/ToolsCommon/Tools_Expression.hpp
Tools_HexBuffer.cpp inc=:SAPDB/ToolsCommon/Tools_HexBuffer.hpp
Tools_HexBuffer.hpp inc=
Tools_List.hpp inc=
Tools_Object.hpp inc=:SAPDB/Messages/Msg_List.hpp
Tools_ParameterParser.cpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/ToolsCommon/Tools_Vector.hpp,\
    :SAPDB/ToolsCommon/Tools_ParameterParser.hpp,\
    :SAPDB/ToolsCommon/Tools_DynamicUTF8String.hpp
Tools_ParameterParser.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/ToolsCommon/Tools_Vector.hpp,\
    :SAPDB/ToolsCommon/Tools_DynamicUTF8String.hpp
Tools_PipeCall.cpp inc=heo02.h,heo13.h,heo06.h,hsp02.h,\
    :SAPDB/SAPDBCommon/SAPDB_string.h,\
    :SAPDB/ToolsCommon/Tools_PipeCall.hpp
Tools_PipeCall.hpp inc=:SAPDB/ToolsCommon/Tools_Array.hpp
Tools_PipeCallTest.cpp inc=:SAPDB/ToolsCommon/Tools_PipeCall.hpp
Tools_Properties.cpp inc=heo06.h,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_ToolsCommon.h,\
    :SAPDB/ToolsCommon/Tools_Properties.hpp,\
    :SAPDB/ToolsCommon/MemoryManagement/ToolsMeMa_GuardedOperation.hpp
Tools_Properties.hpp inc=:SAPDB/Messages/Msg_List.hpp,\
    :SAPDB/ToolsCommon/Tools_DynamicUTF8String.hpp,\
    :SAPDB/ToolsCommon/Tools_List.hpp,\
    :SAPDB/ToolsCommon/Time/ToolsTime_Time.hpp,\
    :SAPDB/ToolsCommon/Time/ToolsTime_TimeFormat.hpp
Tools_Session.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/ToolsCommon/Tools_DynamicUTF8String.hpp
Tools_System.cpp inc=:SAPDB/ToolsCommon/Tools_System.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_string.h
Tools_System.hpp inc=
Tools_Template.cpp inc=:SAPDB/ToolsCommon/Tools_Template.hpp
Tools_Template.hpp inc=hwd02waapi.h,:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/ToolsCommon/Tools_Vector.hpp,\
    :SAPDB/ToolsCommon/Tools_DynamicUTF8String.hpp
Tools_TemplateMsgBox.cpp inc=:SAPDB/ToolsCommon/Tools_TemplateMsgBox.hpp
Tools_TemplateMsgBox.hpp inc=:SAPDB/ToolsCommon/Tools_Template.hpp,\
    :SAPDB/Messages/Msg_List.hpp
Tools_TemplateSimpleTable.cpp inc=\
    :SAPDB/ToolsCommon/Tools_TemplateSimpleTable.hpp
Tools_TemplateSimpleTable.hpp inc=:SAPDB/ToolsCommon/Tools_Template.hpp,\
    :SAPDB/ToolsCommon/Tools_Array.hpp
Tools_UTF8Basis.cpp inc=:SAPDB/ToolsCommon/Tools_UTF8Basis.hpp
Tools_UTF8Basis.hpp inc=hsp81.h,:SAPDB/SAPDBCommon/SAPDB_Types.hpp
Tools_UTF8StringElement.hpp inc=:SAPDB/ToolsCommon/Tools_UTF8Basis.hpp
Tools_UnicodeCaseTable.cpp inc=\
    :SAPDB/ToolsCommon/Tools_UnicodeCaseTable.hpp
Tools_UnicodeCaseTable.hpp inc=:SAPDB/SAPDBCommon/SAPDB_Types.hpp
Tools_Vector.hpp inc=
Tools_WinEvtLog.h inc=
Tools_WinEvtLog.rc inc=gsp100.h
Tools_WinEvtLogDummy.c inc=

