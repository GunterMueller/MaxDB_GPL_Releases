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
XMLQCLib_Definition.h	inc=gwd00.h,SAPDB/XML/QueryServerLib/XMLQSLib_Definition.h

#----------------------------------------------------------
# Common Tools and Helpers
#----------------------------------------------------------
XMLQCLib_Types.h	inc=SAPDB/ToolsCommon/Tools_DynamicUTF8String.hpp,XMLIMAPI_Definition.h,XMLQSLib_Definition.h

XMLQCLib_Common_Include.hpp	inc=SAPDB/XML/QueryClientLib/XMLQCLib_Types.h,SAPDB/ToolsCommon/Tools_DynamicUTF8String.hpp,SAPDB/ToolsCommon/Tools_Template.hpp,hsp77.h,hwd07dbc.h,hwd26err.h
									
XMLQCLib_Common_Utils.hpp	inc=SAPDB/XML/QueryClientLib/XMLQCLib_Common_TextConstants.h,SAPDB/XML/QueryClientLib/XMLQCLib_Common_Include.hpp
XMLQCLib_Common_Utils.cpp	inc=SAPDB/XML/QueryClientLib/XMLQCLib_Common_Utils.hpp

#-----------------------------------------------------------
# HTML Templates - Embedded GUI only
#-----------------------------------------------------------
XMLQCLib_HtmlTemplate_HeaderEmbedded.hpp	inc=SAPDB/XML/QueryClientLib/XMLQCLib_Common_Utils.hpp,SAPDB/XML/QueryClientLib/XMLQCLib_Common_Include.hpp
XMLQCLib_HtmlTemplate_HeaderEmbedded.cpp	inc=XMLQCLib_HtmlTemplate_HeaderEmbedded.hpp

XMLQCLib_HtmlTemplate_InitialFrameEmbedded.hpp	inc=SAPDB/XML/QueryClientLib/XMLQCLib_Common_Utils.hpp,SAPDB/XML/QueryClientLib/XMLQCLib_Common_Include.hpp
XMLQCLib_HtmlTemplate_InitialFrameEmbedded.cpp	inc=XMLQCLib_HtmlTemplate_InitialFrameEmbedded.hpp

XMLQCLib_HtmlTemplate_NavigationEmbedded.hpp	inc=SAPDB/XML/QueryClientLib/XMLQCLib_Common_Utils.hpp,SAPDB/XML/QueryClientLib/XMLQCLib_Common_Include.hpp
XMLQCLib_HtmlTemplate_NavigationEmbedded.cpp	inc=XMLQCLib_HtmlTemplate_NavigationEmbedded.hpp


#-----------------------------------------------------------
# HTML Templates - Stand Alone GUI only
#-----------------------------------------------------------

#-----------------------------------------------------------
# HTML Templates - both GUI's
#-----------------------------------------------------------
XMLQCLib_HtmlTemplate_ClientQuery.hpp	inc=SAPDB/XML/QueryClientLib/XMLQCLib_Common_Utils.hpp,SAPDB/XML/QueryClientLib/XMLQCLib_Common_Include.hpp
XMLQCLib_HtmlTemplate_ClientQuery.cpp	inc=XMLQCLib_HtmlTemplate_ClientQuery.hpp

XMLQCLib_HtmlTemplate_FreeQuery.hpp	inc=SAPDB/XML/QueryClientLib/XMLQCLib_Common_Utils.hpp,SAPDB/XML/QueryClientLib/XMLQCLib_Common_Include.hpp
XMLQCLib_HtmlTemplate_FreeQuery.cpp	inc=XMLQCLib_HtmlTemplate_FreeQuery.hpp


#-----------------------------------------------------------
# Control 
#-----------------------------------------------------------
XMLQCLib_SessionEmbedded.hpp	inc=XMLQCLib_Common_TextConstants.h,XMLQCLib_Common_Utils.hpp,XMLQCLib_HtmlTemplate_InitialFrameEmbedded.hpp,XMLQCLib_HtmlTemplate_HeaderEmbedded.hpp,XMLQCLib_HtmlTemplate_NavigationEmbedded.hpp,XMLQCLib_HtmlTemplate_ClientQuery.hpp,XMLQCLib_HtmlTemplate_FreeQuery.hpp
XMLQCLib_SessionEmbedded.cpp	inc=XMLQCLib_SessionEmbedded.hpp,XMLQCLib_Definition.h

#-----------------------------------------------------------
# HTML Pages - Embedded GUI only
#-----------------------------------------------------------
XMLQCLib_Html_InitialFrameEmbedded.htm ascii ->$DBROOT/Documents/WARoot/HTML/XMLQCLib_Html_InitialFrameEmbedded.htm
XMLQCLib_Html_HeaderEmbedded.htm ascii ->$DBROOT/Documents/WARoot/HTML/XMLQCLib_Html_HeaderEmbedded.htm
XMLQCLib_Html_Navigation.htm ascii ->$DBROOT/Documents/WARoot/HTML/XMLQCLib_Html_Navigation.htm
XMLQCLib_Html_Message.htm ascii ->$DBROOT/Documents/WARoot/HTML/XMLQCLib_Html_Message.htm

#-----------------------------------------------------------
# HTML Pages - Stand Alone GUI only
#-----------------------------------------------------------

#-----------------------------------------------------------
# HTML Pages - both GUI's
#-----------------------------------------------------------
XMLQCLib_Html_ClientQuery.htm ascii ->$DBROOT/Documents/WARoot/HTML/XMLQCLib_Html_ClientQuery.htm
XMLQCLib_Html_FreeQuery.htm ascii ->$DBROOT/Documents/WARoot/HTML/XMLQCLib_Html_FreeQuery.htm

#-----------------------------------------------------------
# Images - both GUI's
#-----------------------------------------------------------

