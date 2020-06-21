#
# :SAPDB/XML/IndexEngine.com
#
# XML Index Engine
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
XMLIdx_Dummy.c			ascii ->$INSTROOT/log/XMLIdx_Dummy.c

# Registry files
XMLIdx_Registry.reg		ascii ->$INSTROOT/config/sapdbxie.reg.org
XMLIdx_Registry.ini		ascii ->$INSTROOT/config/sapdbxie.ini.org


# Conf files
XMLIdx_Conf.win			ascii ->$INSTROOT/pgm/sapdbxie.conf
XMLIdx_Conf.unix		ascii ->$INSTROOT/pgm/sapdbxie.conf


# API Headers
#XMLIdx_Interface.h		inc=vpa00sql.h,vpa00sqlext.h \
#							->$INSTROOT/incl/SAPDB_XmlIndexing.h

# Create log directory
XMLIdx_Log.log			ascii ->$INSTROOT/Log/XMLIdx_Log.log

# Error
XMLIdx_ErrorCodes.h		inc=XMLError_ErrorCodeRanges.h

# Main
XMLIdx_Main.c			inc=SAPDB_Types.h,XMLCom_Common.h

# Service
XMLIdx_Service.h		inc=SAPDB_Types.h
XMLIdx_Service.c		inc=gsp05,\
							XMLIdx_Service.h,XMLCom_Common.h,XMLMem_Memory.h,XMLIdx_Engine.h,XMLError_Error.h,\
							XMLSP_SessionPool.h,XMLSet_Settings.h,XMLIMAPI_Definition.h

# Doc Store
XMLIdx_DocStore.h		inc=SAPDB_Types.h,XMLIdx_Workers.h,XMLId_Id.h,XMLError_Error.h,XMLSP_SessionPool.h
XMLIdx_DocStore.c		inc=XMLMem_Memory.h,XMLIdx_DocStore.h,XMLIdx_ErrorCodes.h,XMLCom_Common.h,SAPDB_string.h

# Engine
XMLIdx_Engine.h			inc=XMLCom_Common.h
XMLIdx_Engine.c			inc=heo07.h,XMLIdx_DocStore.h,\
							XMLCom_Common.h,XMLIdx_ErrorCodes.h,XMLMem_Memory.h,XMLIdx_Engine.h,XMLIdx_Workers.h,\
							XMLError_Error.h,XMLDC_DocClass.h,XMLLib_Library.h,XMLSP_SessionPool.h,XMLSet_Settings.h,\
							XMLIMAPI_Definition.h,XMLIdx_Indexing.h

# Workers
XMLIdx_Workers.h		inc=SAPDB_Types.h,XMLSP_SessionPool.h,XMLIdx_Engine.h,XMLLib_Library.h,XMLDC_DocClass.h,XMLError_Error.h
XMLIdx_Workers.c		inc=heo07.h,\
							XMLIdx_Workers.h,XMLLock_Lock.h,XMLSem_Semaphore.h,XMLCom_Common.h,XMLMem_Memory.h,XMLIdx_ErrorCodes.h,SAPDB_string.h

# Indexing
XMLIdx_Indexing.h		inc=XMLIdx_Workers.h,XMLError_Error.h,\
							XMLXPath_Interface.h,XMLXPath_Indexing.h
XMLIdx_Indexing.c		inc=XMLIdx_Indexing.h,XMLIdx_ErrorCodes.h,XMLCom_Common.h,XMLError_Error.h,XMLIdx_Workers.h,\
							XMLDC_DocClass.h,XMLSP_SessionPool.h,XMLIdx_DocStore.h


