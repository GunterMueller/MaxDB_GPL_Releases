###########################################################
#
# XML Modelling HTML GUI
#
# Responsible: DanielK
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

###########################################################

# Interface of the xmlmodellinglib
#----------------------------------------------------------
XMLIDMLib_Definition.h	inc=gwd00.h

#----------------------------------------------------------
# Common Tools and Helpers
#----------------------------------------------------------
XMLIDMLib_Types.h	inc=SAPDB/ToolsCommon/Tools_DynamicUTF8String.hpp,SAPDB/ToolsCommon/Tools_Vector.hpp,XMLIMAPI_Definition.h

XMLIDMLib_Common_Include.hpp	inc=SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Types.h,SAPDB/ToolsCommon/Tools_DynamicUTF8String.hpp,SAPDB/ToolsCommon/Tools_Template.hpp,hsp77.h,hwd07dbc.h,hwd26err.h
									
XMLIDMLib_Common_Utils.hpp	inc=SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Common_TextConstants.h,SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Common_Include.hpp
XMLIDMLib_Common_Utils.cpp	inc=SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Common_Utils.hpp

#-----------------------------------------------------------
# HTML Templates - Embedded GUI only
#-----------------------------------------------------------
XMLIDMLib_HtmlTemplate_HeaderEmbedded.hpp	inc=SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Common_Utils.hpp,SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Common_Include.hpp
XMLIDMLib_HtmlTemplate_HeaderEmbedded.cpp	inc=XMLIDMLib_HtmlTemplate_HeaderEmbedded.hpp

XMLIDMLib_HtmlTemplate_XieHeaderEmbedded.hpp	inc=SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Common_Utils.hpp,SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Common_Include.hpp
XMLIDMLib_HtmlTemplate_XieHeaderEmbedded.cpp	inc=XMLIDMLib_HtmlTemplate_XieHeaderEmbedded.hpp

XMLIDMLib_HtmlTemplate_InitialFrameEmbedded.hpp	inc=SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Common_Utils.hpp,SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Common_Include.hpp
XMLIDMLib_HtmlTemplate_InitialFrameEmbedded.cpp	inc=XMLIDMLib_HtmlTemplate_InitialFrameEmbedded.hpp

XMLIDMLib_HtmlTemplate_InitialFrameXieEmbedded.hpp	inc=SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Common_Utils.hpp,SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Common_Include.hpp
XMLIDMLib_HtmlTemplate_InitialFrameXieEmbedded.cpp	inc=XMLIDMLib_HtmlTemplate_InitialFrameXieEmbedded.hpp

XMLIDMLib_HtmlTemplate_NavigationEmbedded.hpp	inc=SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Common_Utils.hpp,SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Common_Include.hpp
XMLIDMLib_HtmlTemplate_NavigationEmbedded.cpp	inc=XMLIDMLib_HtmlTemplate_NavigationEmbedded.hpp

XMLIDMLib_HtmlTemplate_XieNavigationEmbedded.hpp	inc=SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Common_Utils.hpp,SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Common_Include.hpp
XMLIDMLib_HtmlTemplate_XieNavigationEmbedded.cpp	inc=XMLIDMLib_HtmlTemplate_XieNavigationEmbedded.hpp

#-----------------------------------------------------------
# HTML Templates - Stand Alone GUI only
#-----------------------------------------------------------

#-----------------------------------------------------------
# HTML Templates - both GUI's
#-----------------------------------------------------------
XMLIDMLib_HtmlTemplate_ClientXmlModelling.hpp	inc=SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Common_Utils.hpp,SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Common_Include.hpp
XMLIDMLib_HtmlTemplate_ClientXmlModelling.cpp	inc=XMLIDMLib_HtmlTemplate_ClientXmlModelling.hpp

XMLIDMLib_HtmlTemplate_ClientService.hpp	inc=SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Common_Utils.hpp,SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Common_Include.hpp
XMLIDMLib_HtmlTemplate_ClientService.cpp	inc=XMLIDMLib_HtmlTemplate_ClientService.hpp

XMLIDMLib_HtmlTemplate_ClientXieAdmin.hpp	inc=SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Common_Utils.hpp,SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Common_Include.hpp
XMLIDMLib_HtmlTemplate_ClientXieAdmin.cpp	inc=XMLIDMLib_HtmlTemplate_ClientXieAdmin.hpp

XMLIDMLib_HtmlTemplate_XmlNavigation.hpp	inc=SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Common_Utils.hpp,SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Common_Include.hpp
XMLIDMLib_HtmlTemplate_XmlNavigation.cpp	inc=XMLIDMLib_HtmlTemplate_XmlNavigation.hpp

XMLIDMLib_HtmlTemplate_DocNavigation.hpp	inc=SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Common_Utils.hpp,SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Common_Include.hpp
XMLIDMLib_HtmlTemplate_DocNavigation.cpp	inc=XMLIDMLib_HtmlTemplate_DocNavigation.hpp

XMLIDMLib_HtmlTemplate_XieNavigation.hpp	inc=SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Common_Utils.hpp,SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Common_Include.hpp
XMLIDMLib_HtmlTemplate_XieNavigation.cpp	inc=XMLIDMLib_HtmlTemplate_XieNavigation.hpp,SAPDB_string.h

XMLIDMLib_HtmlTemplate_ServiceNavigation.hpp	inc=SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Common_Utils.hpp,SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Common_Include.hpp
XMLIDMLib_HtmlTemplate_ServiceNavigation.cpp	inc=XMLIDMLib_HtmlTemplate_ServiceNavigation.hpp

XMLIDMLib_HtmlTemplate_XmlIndex_New.hpp	inc=SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Common_Utils.hpp,SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Common_Include.hpp
XMLIDMLib_HtmlTemplate_XmlIndex_New.cpp	inc=XMLIDMLib_HtmlTemplate_XmlIndex_New.hpp

XMLIDMLib_HtmlTemplate_XmlIndex_Show.hpp	inc=SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Common_Utils.hpp,SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Common_Include.hpp
XMLIDMLib_HtmlTemplate_XmlIndex_Show.cpp	inc=XMLIDMLib_HtmlTemplate_XmlIndex_Show.hpp

XMLIDMLib_HtmlTemplate_Message.hpp	inc=SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Common_Utils.hpp,SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Common_Include.hpp
XMLIDMLib_HtmlTemplate_Message.cpp	inc=XMLIDMLib_HtmlTemplate_Message.hpp

XMLIDMLib_HtmlTemplate_DocClass_New.hpp	inc=SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Common_Utils.hpp,SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Common_Include.hpp
XMLIDMLib_HtmlTemplate_DocClass_New.cpp	inc=XMLIDMLib_HtmlTemplate_DocClass_New.hpp

XMLIDMLib_HtmlTemplate_DocClass_Show.hpp	inc=SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Common_Utils.hpp,SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Common_Include.hpp
XMLIDMLib_HtmlTemplate_DocClass_Show.cpp	inc=XMLIDMLib_HtmlTemplate_DocClass_Show.hpp

XMLIDMLib_HtmlTemplate_SessionPool_New.hpp	inc=SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Common_Utils.hpp,SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Common_Include.hpp
XMLIDMLib_HtmlTemplate_SessionPool_New.cpp	inc=XMLIDMLib_HtmlTemplate_SessionPool_New.hpp

XMLIDMLib_HtmlTemplate_SessionPool_Show.hpp	inc=SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Common_Utils.hpp,SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Common_Include.hpp
XMLIDMLib_HtmlTemplate_SessionPool_Show.cpp	inc=XMLIDMLib_HtmlTemplate_SessionPool_Show.hpp

XMLIDMLib_HtmlTemplate_IndexingService_New.hpp	inc=SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Common_Utils.hpp,SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Common_Include.hpp
XMLIDMLib_HtmlTemplate_IndexingService_New.cpp	inc=XMLIDMLib_HtmlTemplate_IndexingService_New.hpp

XMLIDMLib_HtmlTemplate_IndexingService_Show.hpp	inc=SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Common_Utils.hpp,SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Common_Include.hpp
XMLIDMLib_HtmlTemplate_IndexingService_Show.cpp	inc=XMLIDMLib_HtmlTemplate_IndexingService_Show.hpp

XMLIDMLib_HtmlTemplate_Xie_Show.hpp	inc=SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Common_Utils.hpp,SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Common_Include.hpp
XMLIDMLib_HtmlTemplate_Xie_Show.cpp	inc=XMLIDMLib_HtmlTemplate_Xie_Show.hpp

XMLIDMLib_HtmlTemplate_Xie_ShowAll.hpp	inc=SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Common_Utils.hpp,SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Common_Include.hpp
XMLIDMLib_HtmlTemplate_Xie_ShowAll.cpp	inc=XMLIDMLib_HtmlTemplate_Xie_ShowAll.hpp

#-----------------------------------------------------------
# Control 
#-----------------------------------------------------------
XMLIDMLib_SessionEmbedded.hpp	inc=SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Common_TextConstants.h,SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Common_Utils.hpp,SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_HtmlTemplate_InitialFrameEmbedded.hpp,XMLIDMLib_HtmlTemplate_HeaderEmbedded.hpp,SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_HtmlTemplate_NavigationEmbedded.hpp,SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_HtmlTemplate_ClientXmlModelling.hpp,XMLIDMLib_HtmlTemplate_ClientService.hpp,SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_HtmlTemplate_XmlNavigation.hpp,SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_HtmlTemplate_DocNavigation.hpp,XMLIDMLib_HtmlTemplate_ServiceNavigation.hpp,SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_HtmlTemplate_Message.hpp,XMLIDMLib_HtmlTemplate_XmlIndex_Show.hpp,XMLIDMLib_HtmlTemplate_XmlIndex_New.hpp,XMLIDMLib_HtmlTemplate_DocClass_New.hpp,XMLIDMLib_HtmlTemplate_DocClass_Show.hpp,XMLIDMLib_HtmlTemplate_SessionPool_New.hpp,XMLIDMLib_HtmlTemplate_SessionPool_Show.hpp,XMLIDMLib_HtmlTemplate_IndexingService_Show.hpp,XMLIDMLib_HtmlTemplate_IndexingService_New.hpp,XMLIDMLib_HtmlTemplate_XieNavigationEmbedded.hpp,XMLIDMLib_HtmlTemplate_ClientXieAdmin.hpp,XMLIDMLib_HtmlTemplate_XieNavigation.hpp,XMLIDMLib_HtmlTemplate_InitialFrameXieEmbedded.hpp,XMLIDMLib_HtmlTemplate_XieHeaderEmbedded.hpp,XMLIDMLib_HtmlTemplate_Xie_Show.hpp,XMLIDMLib_HtmlTemplate_Xie_ShowAll.hpp
XMLIDMLib_SessionEmbedded.cpp	inc=SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_SessionEmbedded.hpp,SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Definition.h

#-----------------------------------------------------------
# HTML Pages - Embedded GUI only
#-----------------------------------------------------------
XMLIDMLib_Html_InitialFrameEmbedded.htm ascii ->$DBROOT/Documents/WARoot/HTML/XMLIDMLib_Html_InitialFrameEmbedded.htm
XMLIDMLib_Html_InitialFrameXieEmbedded.htm ascii ->$DBROOT/Documents/WARoot/HTML/XMLIDMLib_Html_InitialFrameXieEmbedded.htm
XMLIDMLib_Html_HeaderEmbedded.htm ascii ->$DBROOT/Documents/WARoot/HTML/XMLIDMLib_Html_HeaderEmbedded.htm
XMLIDMLib_Html_XieHeaderEmbedded.htm ascii ->$DBROOT/Documents/WARoot/HTML/XMLIDMLib_Html_XieHeaderEmbedded.htm
XMLIDMLib_Html_NavigationEmbedded.htm ascii ->$DBROOT/Documents/WARoot/HTML/XMLIDMLib_Html_NavigationEmbedded.htm
XMLIDMLib_Html_XieNavigationEmbedded.htm ascii ->$DBROOT/Documents/WARoot/HTML/XMLIDMLib_Html_XieNavigationEmbedded.htm

#-----------------------------------------------------------
# HTML Pages - Stand Alone GUI only
#-----------------------------------------------------------

#-----------------------------------------------------------
# HTML Pages - both GUI's
#-----------------------------------------------------------
XMLIDMLib_Html_ClientXmlModelling.htm ascii ->$DBROOT/Documents/WARoot/HTML/XMLIDMLib_Html_ClientXmlModelling.htm
XMLIDMLib_Html_ClientService.htm ascii ->$DBROOT/Documents/WARoot/HTML/XMLIDMLib_Html_ClientService.htm
XMLIDMLib_Html_ClientXieAdmin.htm ascii ->$DBROOT/Documents/WARoot/HTML/XMLIDMLib_Html_ClientXieAdmin.htm
XMLIDMLib_Html_Empty.htm ascii ->$DBROOT/Documents/WARoot/HTML/XMLIDMLib_Html_Empty.htm
XMLIDMLib_Html_EmptyXie.htm ascii ->$DBROOT/Documents/WARoot/HTML/XMLIDMLib_Html_EmptyXie.htm
XMLIDMLib_Html_EmptyServ.htm ascii ->$DBROOT/Documents/WARoot/HTML/XMLIDMLib_Html_EmptyServ.htm
XMLIDMLib_Html_XmlNavigation.htm ascii ->$DBROOT/Documents/WARoot/HTML/XMLIDMLib_Html_XmlNavigation.htm
XMLIDMLib_Html_DocNavigation.htm ascii ->$DBROOT/Documents/WARoot/HTML/XMLIDMLib_Html_DocNavigation.htm
XMLIDMLib_Html_XieNavigation.htm ascii ->$DBROOT/Documents/WARoot/HTML/XMLIDMLib_Html_XieNavigation.htm
XMLIDMLib_Html_ServiceNavigation.htm ascii ->$DBROOT/Documents/WARoot/HTML/XMLIDMLib_Html_ServiceNavigation.htm
XMLIDMLib_Html_Message.htm ascii ->$DBROOT/Documents/WARoot/HTML/XMLIDMLib_Html_Message.htm
XMLIDMLib_Html_MessageEmpty.htm ascii ->$DBROOT/Documents/WARoot/HTML/XMLIDMLib_Html_MessageEmpty.htm
XMLIDMLib_Html_XmlIndex_Show.htm ascii ->$DBROOT/Documents/WARoot/HTML/XMLIDMLib_Html_XmlIndex_Show.htm
XMLIDMLib_Html_XmlIndex_New.htm ascii ->$DBROOT/Documents/WARoot/HTML/XMLIDMLib_Html_XmlIndex_New.htm
XMLIDMLib_Html_DocClass_New.htm ascii ->$DBROOT/Documents/WARoot/HTML/XMLIDMLib_Html_DocClass_New.htm
XMLIDMLib_Html_DocClass_Show.htm ascii ->$DBROOT/Documents/WARoot/HTML/XMLIDMLib_Html_DocClass_Show.htm
XMLIDMLib_Html_SessionPool_New.htm ascii ->$DBROOT/Documents/WARoot/HTML/XMLIDMLib_Html_SessionPool_New.htm
XMLIDMLib_Html_SessionPool_Show.htm ascii ->$DBROOT/Documents/WARoot/HTML/XMLIDMLib_Html_SessionPool_Show.htm
XMLIDMLib_Html_IndexingService_New.htm ascii ->$DBROOT/Documents/WARoot/HTML/XMLIDMLib_Html_IndexingService_New.htm
XMLIDMLib_Html_IndexingService_Show.htm ascii ->$DBROOT/Documents/WARoot/HTML/XMLIDMLib_Html_IndexingService_Show.htm
XMLIDMLib_Html_Xie_Show.htm ascii ->$DBROOT/Documents/WARoot/HTML/XMLIDMLib_Html_Xie_Show.htm
XMLIDMLib_Html_Xie_ShowAll.htm ascii ->$DBROOT/Documents/WARoot/HTML/XMLIDMLib_Html_Xie_ShowAll.htm

#-----------------------------------------------------------
# Images - both GUI's
#-----------------------------------------------------------
XMLIDMLib_webdbm.gif	binary ->$DBROOT/Documents/WARoot/Images/xml_webdbm.gif
XMLIDMLib_btnb1.gif	binary ->$DBROOT/Documents/WARoot/Images/xml_btnb1.gif
XMLIDMLib_btnb3.gif	binary ->$DBROOT/Documents/WARoot/Images/xml_btnb3.gif
XMLIDMLib_btnl1.gif	binary ->$DBROOT/Documents/WARoot/Images/xml_btnl1.gif
XMLIDMLib_btnl3.gif	binary ->$DBROOT/Documents/WARoot/Images/xml_btnl3.gif
XMLIDMLib_btnr1.gif	binary ->$DBROOT/Documents/WARoot/Images/xml_btnr1.gif
XMLIDMLib_btnr3.gif	binary ->$DBROOT/Documents/WARoot/Images/xml_btnr3.gif
XMLIDMLib_labelnotch.gif	binary ->$DBROOT/Documents/WARoot/Images/xml_labelnotch.gif
XMLIDMLib_msgcrit.gif	binary ->$DBROOT/Documents/WARoot/Images/xml_msgcrit.gif
XMLIDMLib_msgerrosmall.gif	binary ->$DBROOT/Documents/WARoot/Images/xml_msgerrosmall.gif
XMLIDMLib_msgoksmall.gif	binary ->$DBROOT/Documents/WARoot/Images/xml_msgoksmall.gif
XMLIDMLib_msgwarnsmall.gif	binary ->$DBROOT/Documents/WARoot/Images/xml_msgwarnsmall.gif
XMLIDMLib_tabbkoff.gif	binary ->$DBROOT/Documents/WARoot/Images/xml_tabbkoff.gif
XMLIDMLib_tabbkon.gif	binary ->$DBROOT/Documents/WARoot/Images/xml_tabbkon.gif
XMLIDMLib_tabfroff.gif	binary ->$DBROOT/Documents/WARoot/Images/xml_tabfroff.gif
XMLIDMLib_tabfron.gif	binary ->$DBROOT/Documents/WARoot/Images/xml_tabfron.gif
XMLIDMLib_taboff.gif	binary ->$DBROOT/Documents/WARoot/Images/xml_taboff.gif
XMLIDMLib_taboffoff.gif	binary ->$DBROOT/Documents/WARoot/Images/xml_taboffoff.gif
XMLIDMLib_taboffon.gif	binary ->$DBROOT/Documents/WARoot/Images/xml_taboffon.gif
XMLIDMLib_tabon.gif	binary ->$DBROOT/Documents/WARoot/Images/xml_tabon.gif
XMLIDMLib_tabonoff.gif	binary ->$DBROOT/Documents/WARoot/Images/xml_tabonoff.gif
XMLIDMLib_WANav11.gif	binary ->$DBROOT/Documents/WARoot/Images/xml_WANav11.gif
XMLIDMLib_WANav12.gif	binary ->$DBROOT/Documents/WARoot/Images/xml_WANav12.gif
XMLIDMLib_WANav3.gif	binary ->$DBROOT/Documents/WARoot/Images/xml_WANav3.gif
XMLIDMLib_WANav4.gif	binary ->$DBROOT/Documents/WARoot/Images/xml_WANav4.gif
XMLIDMLib_WANav5.gif	binary ->$DBROOT/Documents/WARoot/Images/xml_WANav5.gif
XMLIDMLib_WANav7.gif	binary ->$DBROOT/Documents/WARoot/Images/xml_WANav7.gif
XMLIDMLib_folder.gif	binary ->$DBROOT/Documents/WARoot/Images/xml_folder.gif
XMLIDMLib_folder_up.gif	binary ->$DBROOT/Documents/WARoot/Images/xml_folder_up.gif
XMLIDMLib_folder_refresh.gif	binary ->$DBROOT/Documents/WARoot/Images/xml_folder_refresh.gif
XMLIDMLib_folder_trash.gif	binary ->$DBROOT/Documents/WARoot/Images/xml_folder_trash.gif
XMLIDMLib_file.gif	binary ->$DBROOT/Documents/WARoot/Images/xml_file.gif
XMLIDMLib_xml.gif	binary ->$DBROOT/Documents/WARoot/Images/xml_xml.gif
XMLIDMLib_xquery.gif	binary ->$DBROOT/Documents/WARoot/Images/xml_xquery.gif
XMLIDMLib_home.gif	binary ->$DBROOT/Documents/WARoot/Images/xml_home.gif
XMLIDMLib_trash.gif	binary ->$DBROOT/Documents/WARoot/Images/xml_trash.gif
XMLIDMLib_trash_recycle.gif	binary ->$DBROOT/Documents/WARoot/Images/xml_trash_recycle.gif
XMLIDMLib_trash_empty.gif	binary ->$DBROOT/Documents/WARoot/Images/xml_trash_empty.gif
XMLIDMLib_props.gif	binary ->$DBROOT/Documents/WARoot/Images/xml_props.gif
XMLIDMLib_dotline1.gif	binary ->$DBROOT/Documents/WARoot/Images/xml_dotline1.gif
XMLIDMLib_dotline2.gif	binary ->$DBROOT/Documents/WARoot/Images/xml_dotline2.gif


