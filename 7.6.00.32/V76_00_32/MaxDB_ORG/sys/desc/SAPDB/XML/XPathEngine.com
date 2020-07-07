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

#
# include dependencies (do not edit)
#
#
#

XMLXPath_Error.c inc=:SAPDB/SAPDBCommon/SAPDB_Types.h\
,:SAPDB/XML/XPathEngine/XMLXPath_Error.h\
,:SAPDB/XML/XPathEngine/XMLXPath_Types.h,SAPDB_string.h
XMLXPath_Error.h inc=:SAPDB/XML/XPathEngine/XMLXPath_Indexing.h
XMLXPath_Index.hxxx inc=:SAPDB/XML/XPathEngine/XMLXPath_Interface.h
XMLXPath_Indexing.c inc=:SAPDB/XML/XPathEngine/XMLXPath_Indexing.h\
,:SAPDB/XML/XPathEngine/XMLXPath_Error.h\
,:SAPDB/XML/XPathEngine/XMLXPath_Memory.h\
,:SAPDB/XML/XPathEngine/XMLXPath_ParseExpr.h\
,:SAPDB/XML/XPathEngine/XMLXPath_Tokenize.h\
,:SAPDB/XML/XPathEngine/XMLXPath_Types.h\
,:SAPDB/XML/XPathEngine/XMLXPath_ParseDocument.h
XMLXPath_Indexing.h inc=:SAPDB/XML/XPathEngine/XMLXPath_Interface.h
XMLXPath_Interface.h inc=:SAPDB/SAPDBCommon/SAPDB_Types.h
XMLXPath_Memory.c inc=:SAPDB/SAPDBCommon/SAPDB_Types.h\
,:SAPDB/XML/XPathEngine/XMLXPath_Memory.h,SAPDB_string.h
XMLXPath_Memory.h inc=
XMLXPath_ParseDocument.c inc=:SAPDB/SAPDBCommon/SAPDB_Types.h\
,:SAPDB/XML/XPathEngine/XMLXPath_ParseDocument.h\
,:SAPDB/XML/XPathEngine/XMLXPath_Error.h\
,:SAPDB/XML/XPathEngine/XMLXPath_Memory.h\
,:SAPDB/XML/XPathEngine/XMLXPath_ParseSaxHandling.h
XMLXPath_ParseDocument.h inc=:idl/expat/expat.h\
,:SAPDB/XML/XPathEngine/XMLXPath_Interface.h\
,:SAPDB/XML/XPathEngine/XMLXPath_Types.h
XMLXPath_ParseExpr.c inc=:SAPDB/XML/XPathEngine/XMLXPath_ParseExpr.h\
,:SAPDB/XML/XPathEngine/XMLXPath_Memory.h\
,:SAPDB/XML/XPathEngine/XMLXPath_Error.h\
,:SAPDB/XML/XPathEngine/XMLXPath_Tokenize.h,SAPDB_string.h
XMLXPath_ParseExpr.h inc=:SAPDB/XML/XPathEngine/XMLXPath_Interface.h\
,:SAPDB/XML/XPathEngine/XMLXPath_Types.h
XMLXPath_ParseSaxHandling.c inc=\
:SAPDB/XML/XPathEngine/XMLXPath_ParseSaxHandling.h\
,:SAPDB/XML/XPathEngine/XMLXPath_Memory.h\
,:SAPDB/XML/XPathEngine/XMLXPath_Error.h,:SAPDB/XML/XML_UTF8.h,SAPDB_string.h
XMLXPath_ParseSaxHandling.h inc=\
:SAPDB/XML/XPathEngine/XMLXPath_Interface.h\
,:SAPDB/XML/XPathEngine/XMLXPath_Types.h
XMLXPath_TestMain.c inc=:SAPDB/XML/XPathEngine/XMLXPath_Interface.h\
,:SAPDB/XML/XPathEngine/XMLXPath_Indexing.h\
,:SAPDB/XML/XPathEngine/XMLXPath_Error.h
XMLXPath_Tokenize.c inc=:SAPDB/XML/XPathEngine/XMLXPath_Tokenize.h\
,:SAPDB/XML/XPathEngine/XMLXPath_Error.h\
,:SAPDB/XML/XPathEngine/XMLXPath_Memory.h,:SAPDB/XML/XML_UTF8.h,SAPDB_string.h
XMLXPath_Tokenize.h inc=:SAPDB/XML/XPathEngine/XMLXPath_Types.h\
,:SAPDB/XML/XPathEngine/XMLXPath_Indexing.h
XMLXPath_Types.h inc=:SAPDB/SAPDBCommon/SAPDB_Types.h\
,:SAPDB/XML/XPathEngine/XMLXPath_Indexing.h


