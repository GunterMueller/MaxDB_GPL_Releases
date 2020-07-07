#
# :SAPDB/XML/DocClass.com
#
# XML Document Class
#
# Responsible: Markus Oezgen
#
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

# Error Codes
XMLDC_ErrorCodes.h  inc=XMLError_ErrorCodeRanges.h

# Document Class
XMLDC_DocClass.h    inc=SAPDB_Types.h,XMLId_Id.h,XMLLib_Library.h,XMLSP_SessionPool.h,XMLSet_Settings.h,XMLIMAPI_Definition.h,XMLXPath_Indexing.h
XMLDC_DocClass.c	inc=SAPDB_Types.h,SAPDB_string.h,XMLDC_DocClass.h,XMLDC_ErrorCodes.h,XMLLock_Lock.h,XMLSet_Settings.h,XMLMem_Memory.h,XMLXPath_Interface.h,XMLXPath_Indexing.h

