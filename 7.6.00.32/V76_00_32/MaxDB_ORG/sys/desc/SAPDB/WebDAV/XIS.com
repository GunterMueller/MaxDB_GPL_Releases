#
# :SAPDB/WebDAV/XIS.com
#
# WebDAV XML Indexing Service
#
# Responsible: Markus Oezgen
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

# Create log directory
WDVXIS_Dummy.c				ascii ->$INSTROOT/Log/WDVXIS_Dummy.c

# Error Handling
WDVXIS_ErrorCodes.h			inc=XMLError_ErrorCodeRanges.h

# Service
WDVXIS_Service.h			inc=SAPDB_Types.h,XMLError_Error.h,XMLSP_SessionPool.h,XMLId_Id.h,XMLQSLib_Query.h,XMLQSLib_Interface.h
WDVXIS_Service.c			inc=WDVXIS_Service.h,WDVXIS_ErrorCodes.h,XMLMem_Memory.h,SAPDB_string.h
