#
# :SAPDB/XML/SessionPool.com
#
# XML SessionPool
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
XMLSP_ErrorCodes.h  inc=XMLError_ErrorCodeRanges.h
							
# Session Pool
XMLSP_SessionPool.h	inc=SAPDB_Types.h,XMLSP_ErrorCodes.h,XMLError_Error.h,XMLLib_Library.h
XMLSP_SessionPool.c	inc=SAPDB_Types.h,XMLSP_SessionPool.h,XMLError_Error.h,XMLLock_Lock.h,XMLSem_Semaphore.h,XMLMem_Memory.h

