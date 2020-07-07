#
# :SAPDB/WebDAV/Handler.com
#
# WebDAV Handler
#
# Responsible: DanielK
#
#
#
#    ========== licence begin  GPL
#    Copyright (c) 2002-2005 SAP AG
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

WDVHandler_CommonInclude.h	inc=gsp100.h,hsp100.h,gwd00.h,heo102.h,hsp77.h,hwd03waapi.h,WDVHandler_Types.h,WDVCAPI_Definition.h,WDVHandler_Memory.h,XMLIMAPI_Definition.h,SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Definition.h,XMLQCLib_Definition.h

WDVHandler_Types.h	inc=WDVCAPI_Definition.h,:idl/expat/expat.h,XMLIMAPI_Definition.h,XMLQSLib_Definition.h

WDVHandler_Error.h	inc=WDVHandler_CommonInclude.h,WDVHandler_ErrorCodes.h,WDVHandler_ErrorPages.h
WDVHandler_Error.c	inc=WDVHandler_Error.h

WDVHandler_ErrorInclude.h	inc=WDVHandler_ErrorCodes.h,WDVHandler_ErrorPages.h,WDVHandler_Error.h

WDVHandler_Memory.h	inc=
WDVHandler_Memory.c	inc=WDVHandler_Memory.h

WDVHandler_Connection.h	inc=WDVHandler_CommonInclude.h
WDVHandler_Connection.c	inc=WDVHandler_Connection.h,WDVHandler_CommonUtils.h

WDVHandler_CommonUtils.h	inc=WDVHandler_CommonInclude.h,WDVHandler_DAVMethods.h,WDVHandler_ErrorInclude.h,WDVHandler_XmlFragments.h,WDVHandler_XmlParser.h,WDVHandler_Connection.h,WDVHandler_StringBuffer.h
WDVHandler_CommonUtils.c	inc=WDVHandler_CommonUtils.h,hwd21req.h,SAPDB_string.h

WDVHandler_AdminUtils.h	inc=WDVHandler_CommonUtils.h,WDVHandler_DAVMethods.h,WDVHandler_ErrorInclude.h,WDVHandler_XmlFragments.h,WDVHandler_XmlParser.h,WDVHandler_Connection.h
WDVHandler_AdminUtils.c	inc=WDVHandler_AdminUtils.h,WAHTTP_IniPar.h,gsp100.h,hsp100.h

WDVHandler_StringBuffer.h	inc=WDVHandler_CommonInclude.h,WDVHandler_DAVMethods.h,WDVHandler_ErrorInclude.h,WDVHandler_XmlFragments.h,WDVHandler_XmlParser.h,WDVHandler_Connection.h
WDVHandler_StringBuffer.c	inc=WDVHandler_StringBuffer.h

WDVHandler_BufferList.h	inc=WDVHandler_CommonUtils.h
WDVHandler_BufferList.c	inc=WDVHandler_BufferList.h

WDVHandler_XmlParser.h	inc=WDVHandler_CommonInclude.h,:idl/expat/expat.h
WDVHandler_XmlParser.c	inc=WDVHandler_XmlParser.h

WDVHandler_MKCOLHandler.h	inc=WDVHandler_CommonUtils.h,WDVHandler_DAVMethods.h,WDVHandler_ErrorInclude.h,WDVHandler_Connection.h
WDVHandler_MKCOLHandler.c	inc=WDVHandler_MKCOLHandler.h

WDVHandler_DeleteHandler.h	inc=WDVHandler_CommonUtils.h,WDVHandler_DAVMethods.h,WDVHandler_ErrorInclude.h,WDVHandler_Connection.h
WDVHandler_DeleteHandler.c	inc=WDVHandler_DeleteHandler.h

WDVHandler_GetHandler.h	inc=WDVHandler_CommonUtils.h,WDVHandler_DAVMethods.h,WDVHandler_ErrorInclude.h,WDVHandler_Connection.h
WDVHandler_GetHandler.c	inc=WDVHandler_GetHandler.h

WDVHandler_CopyHandler.h	inc=WDVHandler_CommonUtils.h,WDVHandler_DAVMethods.h,WDVHandler_ErrorInclude.h,WDVHandler_Connection.h,WDVHandler_XmlFragments.h
WDVHandler_CopyHandler.c	inc=WDVHandler_CopyHandler.h

WDVHandler_MoveHandler.h	inc=WDVHandler_CommonUtils.h,WDVHandler_DAVMethods.h,WDVHandler_ErrorInclude.h,WDVHandler_Connection.h,WDVHandler_XmlFragments.h
WDVHandler_MoveHandler.c	inc=WDVHandler_MoveHandler.h

WDVHandler_HeadHandler.h	inc=WDVHandler_CommonUtils.h,WDVHandler_DAVMethods.h,WDVHandler_ErrorInclude.h,WDVHandler_Connection.h
WDVHandler_HeadHandler.c	inc=WDVHandler_HeadHandler.h

WDVHandler_OptionsHandler.h	inc=WDVHandler_CommonUtils.h,WDVHandler_DAVMethods.h,WDVHandler_ErrorInclude.h,WDVHandler_Connection.h
WDVHandler_OptionsHandler.c	inc=WDVHandler_OptionsHandler.h

WDVHandler_PropfindHandler.h	inc=WDVHandler_CommonUtils.h,WDVHandler_DAVMethods.h,WDVHandler_ErrorInclude.h,WDVHandler_Connection.h,WDVHandler_XmlParser.h,WDVHandler_XmlFragments.h,WDVHandler_StringBuffer.h
WDVHandler_PropfindHandler.c	inc=WDVHandler_PropfindHandler.h

WDVHandler_ProppatchHandler.h	inc=WDVHandler_CommonUtils.h,WDVHandler_DAVMethods.h,WDVHandler_ErrorInclude.h,WDVHandler_Connection.h,WDVHandler_XmlParser.h,WDVHandler_XmlFragments.h,WDVHandler_StringBuffer.h
WDVHandler_ProppatchHandler.c	inc=WDVHandler_ProppatchHandler.h,SAPDB_string.h

WDVHandler_PostHandler.h	inc=WDVHandler_CommonUtils.h,WDVHandler_DAVMethods.h,WDVHandler_ErrorInclude.h,WDVHandler_Connection.h
WDVHandler_PostHandler.c	inc=WDVHandler_PostHandler.h,SAPDB_string.h

WDVHandler_LockHandler.h	inc=WDVHandler_CommonUtils.h,WDVHandler_DAVMethods.h,WDVHandler_ErrorInclude.h,WDVHandler_Connection.h,WDVHandler_XmlParser.h,WDVHandler_XmlFragments.h
WDVHandler_LockHandler.c	inc=WDVHandler_LockHandler.h,SAPDB_string.h

WDVHandler_UnlockHandler.h	inc=WDVHandler_CommonUtils.h,WDVHandler_DAVMethods.h,WDVHandler_ErrorInclude.h,WDVHandler_Connection.h
WDVHandler_UnlockHandler.c	inc=WDVHandler_UnlockHandler.h

WDVHandler_DAVMethodHandler.h	inc=WDVHandler_CommonUtils.h,WDVHandler_DAVMethods.h,WDVHandler_ErrorInclude.h,WDVHandler_MKCOLHandler.h,WDVHandler_DeleteHandler.h,WDVHandler_GetHandler.h,WDVHandler_OptionsHandler.h,WDVHandler_PropfindHandler.h,WDVHandler_PostHandler.h,WDVHandler_XmlFragments.h,WDVHandler_XmlParser.h,WDVHandler_HeadHandler.h,WDVHandler_CopyHandler.h
WDVHandler_DAVMethodHandler.c	inc=WDVHandler_DAVMethodHandler.h,hwd21req.h

WDVHandler_DAVLockingHandler.h	inc=WDVHandler_CommonUtils.h,WDVHandler_DAVMethods.h,WDVHandler_ErrorInclude.h,WDVHandler_LockHandler.h,WDVHandler_UnlockHandler.h,WDVHandler_XmlFragments.h,WDVHandler_XmlParser.h
WDVHandler_DAVLockingHandler.c	inc=WDVHandler_DAVLockingHandler.h

WDVHandler_VersionControlHandler.h	inc=WDVHandler_CommonUtils.h,WDVHandler_DAVMethods.h,WDVHandler_ErrorInclude.h,WDVHandler_Connection.h,WDVHandler_XmlParser.h,WDVHandler_XmlFragments.h
WDVHandler_VersionControlHandler.c	inc=WDVHandler_VersionControlHandler.h

WDVHandler_CheckoutHandler.h	inc=WDVHandler_CommonUtils.h,WDVHandler_DAVMethods.h,WDVHandler_ErrorInclude.h,WDVHandler_Connection.h,WDVHandler_XmlParser.h,WDVHandler_XmlFragments.h
WDVHandler_CheckoutHandler.c	inc=WDVHandler_CheckoutHandler.h

WDVHandler_CheckinHandler.h	inc=WDVHandler_CommonUtils.h,WDVHandler_DAVMethods.h,WDVHandler_ErrorInclude.h,WDVHandler_Connection.h,WDVHandler_XmlParser.h,WDVHandler_XmlFragments.h
WDVHandler_CheckinHandler.c	inc=WDVHandler_CheckinHandler.h

WDVHandler_UncheckoutHandler.h	inc=WDVHandler_CommonUtils.h,WDVHandler_DAVMethods.h,WDVHandler_ErrorInclude.h,WDVHandler_Connection.h,WDVHandler_XmlParser.h,WDVHandler_XmlFragments.h
WDVHandler_UncheckoutHandler.c	inc=WDVHandler_UncheckoutHandler.h

WDVHandler_DAVVersioningHandler.h	inc=WDVHandler_CommonUtils.h,WDVHandler_DAVMethods.h,WDVHandler_ErrorInclude.h,WDVHandler_XmlFragments.h,WDVHandler_XmlParser.h,WDVHandler_VersionControlHandler.h,WDVHandler_CheckoutHandler.h,WDVHandler_CheckinHandler.h,WDVHandler_UncheckoutHandler.h
WDVHandler_DAVVersioningHandler.c	inc=WDVHandler_DAVVersioningHandler.h

WDVHandler_Explorer.h	inc=WDVHandler_CommonUtils.h,WDVHandler_ErrorInclude.h,WDVHandler_DAVMethods.h,WDVHandler_XmlFragments.h,WDVHandler_XmlParser.h
WDVHandler_Explorer.c	inc=WDVHandler_Explorer.h,hwd21req.h,SAPDB_string.h

WDVHandler_Interface.h   inc=WDVHandler_CommonUtils.h,WDVHandler_DAVMethods.h,WDVHandler_ErrorInclude.h,WDVHandler_DAVMethodHandler.h,WDVHandler_DAVLockingHandler.h,WDVHandler_AdminUtils.h
WDVHandler_Interface.c   inc=WDVHandler_Interface.h,WDVCAPI_Definition.h

WDVHandler_DefaultAdmin.xsl ascii ->$DBROOT/WebAgent/Documents/WARoot/Styles/defaultwebdavadmin.xsl
WDVHandler_Stylesheet.css ascii ->$DBROOT/Documents/WARoot/Styles/WDVHandler_Stylesheet.css
WDVHandler_Html_Message.htm ascii ->$DBROOT/Documents/WARoot/HTML/WDVHandler_Html_Message.htm
WDVHandler_Html_Header.htm ascii ->$DBROOT/Documents/WARoot/HTML/WDVHandler_Html_Header.htm


