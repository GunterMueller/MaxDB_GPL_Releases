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

# Interface of the xmlqueryserverlib
#----------------------------------------------------------
XMLQSLib_Definition.h	inc=gwd00.h,XMLQSLib_Query.h,hwd03waapi.h
XMLQSLib_Query.h		inc=XMLQSLib_Types.h,XMLSP_SessionPool.h,XMLError_Error.h,SAPDB/XML/Library/XMLLib_Library.h

#----------------------------------------------------------
# Common Tools and Helpers
#----------------------------------------------------------
XMLQSLib_Types.h	inc=XMLIMAPI_Definition.h,SAPDB/WebDAV/XIS/WDVXIS_ErrorCodes.h,hwd03waapi.h

XMLQSLib_Common_Include.h	inc=XMLQSLib_Query.h,hsp77.h
									
XMLQSLib_Connection.h	inc=XMLQSLib_Common_Include.h,XMLQSLib_Common_TextConstants.h
XMLQSLib_Connection.c	inc=XMLQSLib_Connection.h,XMLQSLib_Definition.h

#----------------------------------------------------------
# Interface
#----------------------------------------------------------
XMLQSLib_Interface.h	inc=XMLQSLib_Common_Include.h,XMLQSLib_Common_TextConstants.h,XMLQSLib_Connection.h
XMLQSLib_Interface.c	inc=XMLQSLib_Interface.h,XMLQSLib_Definition.h,WDVHandler_Types.h