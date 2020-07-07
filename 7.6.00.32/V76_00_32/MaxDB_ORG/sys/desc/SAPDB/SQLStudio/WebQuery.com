
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

* -I$WRK/incl/SAPDB

WebQuery.rc					   inc=gsp100.h
StudioWeb_Definitions.hpp      inc=Tools_DynamicUTF8String.hpp
StudioWeb_Functions.hpp	       inc=StudioWeb_Definitions.hpp,Studio_StoredFileContent.hpp,StudioOAL_WResult.hpp,hwd08dbc.h,hwd02waapi.h
StudioWeb_Functions.cpp	       inc=StudioWeb_Functions.hpp,Tools_TemplateMsgBox.hpp,StudioWeb_HtmlFragments.h,StudioOAL_WCommon.h,SAPDBCommon/SAPDB_string.h
StudioWeb_WebRequest.cpp       inc=StudioWeb_WebRequest.hpp,StudioWeb_WebQuery.hpp,StudioWeb_Error.h,Tools_TemplateMsgBox.hpp,StudioWeb_HtmlFragments.h,Tools_DynamicUTF8String.hpp,Tools_Session.hpp
StudioWeb_StoredQueryTree.hpp  inc=StudioWeb_Functions.hpp,Studio_DBFSObject.hpp,Studio_StoredFileContent.hpp
StudioWeb_StoredQueryTree.cpp  inc=StudioWeb_StoredQueryTree.hpp,StudioWeb_Error.h,StudioWeb_HtmlFragments.h,SAPDBCommon/SAPDB_string.h
StudioWeb_TemplateDsql.hpp     inc=Tools_Template.hpp,Tools_DynamicUTF8String.hpp,StudioWeb_HtmlFragments.h,Studio_StoredFileContent.hpp
StudioWeb_TemplateDsql.cpp     inc=StudioWeb_TemplateDsql.hpp,StudioWeb_Definitions.hpp
StudioWeb_SQLWindow.hpp		   inc=StudioWeb_Definitions.hpp,StudioWeb_TemplateDsql.hpp,StudioWeb_Result.hpp,Studio_DBFSObject.hpp,Studio_StoredFileContent.hpp
StudioWeb_SQLWindow.cpp	       inc=StudioWeb_HtmlFragments.h,StudioWeb_SQLWindow.hpp,StudioWeb_Functions.hpp,StudioWeb_Error.h
StudioWeb_ResultTemplate.hpp   inc=Tools_Template.hpp,Tools_DynamicUTF8String.hpp,Tools_Array.hpp
StudioWeb_ResultTemplate.cpp   inc=StudioWeb_ResultTemplate.hpp,StudioWeb_Definitions.hpp
StudioWeb_Result.hpp		   inc=StudioWeb_Functions.hpp,StudioWeb_ResultTemplate.hpp,Studio_StoredFileContent.hpp,Studio_Connection.hpp,Studio_StoredFileContent.hpp,StudioWeb_TemplateParamQuery.hpp,Studio_List.hpp
StudioWeb_Result.cpp		   inc=StudioWeb_Result.hpp,StudioWeb_HtmlFragments.h,StudioWeb_Error.h,Studio_Statements.hpp
StudioWeb_TemplateLogon.hpp			inc=Tools_Template.hpp,Tools_DynamicUTF8String.hpp
StudioWeb_TemplateLogon.cpp			inc=StudioWeb_TemplateLogon.hpp
StudioWeb_TemplateHeader.hpp		inc=Tools_Template.hpp
StudioWeb_TemplateHeader.cpp		inc=StudioWeb_TemplateHeader.hpp
StudioWeb_TemplateParamQuery.hpp	inc=Tools_Template.hpp,Tools_DynamicUTF8String.hpp,Tools_Array.hpp
StudioWeb_TemplateParamQuery.cpp	inc=StudioWeb_TemplateParamQuery.hpp
StudioWeb_TemplateFrame.hpp		    inc=Tools_Template.hpp,Tools_DynamicUTF8String.hpp
StudioWeb_TemplateFrame.cpp			inc=StudioWeb_TemplateFrame.hpp 
StudioWeb_TemplateMainFrame.hpp		inc=Tools_Template.hpp,Tools_DynamicUTF8String.hpp
StudioWeb_TemplateMainFrame.cpp		inc=StudioWeb_TemplateMainFrame.hpp 
StudioWeb_WebQuery.hpp				inc=StudioWeb_HtmlFragments.h,StudioWeb_StoredQueryTree.hpp,gwd00.h,StudioWeb_SQLWindow.hpp,StudioWeb_Result.hpp,Studio_DBFSObject.hpp,Studio_Connection.hpp
StudioWeb_WebQuery.cpp				inc=StudioWeb_WebQuery.hpp,StudioWeb_Error.h,StudioWeb_Functions.hpp,StudioWeb_TemplateLogon.hpp,Tools_TemplateMsgBox.hpp,StudioWeb_TemplateHeader.hpp,StudioWeb_TemplateFrame.hpp,StudioWeb_TemplateMainFrame.hpp


