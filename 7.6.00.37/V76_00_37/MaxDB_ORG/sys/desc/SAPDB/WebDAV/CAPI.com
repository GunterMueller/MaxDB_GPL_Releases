#
# :SAPDB/WebDAV/CAPI.com
#
# WebDAV C API
#
# Responsible: MarkusO
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
WDVCAPI_Log.log			 ascii ->$INSTROOT/log/WDVCAPI_Log.log

# API Headers
WDVCAPI_Definition.h     inc=vpa00sql.h,vpa00sqlext.h,SAPDB_Types.h,XMLIMAPI_Definition.h \
                         ->$INSTROOT/incl/WDVCAPI_Definition.h

# Error handling
WDVCAPI_ErrorInternal.h  dep=WDVCAPI_Definition.h
WDVCAPI_Error.c          dep=WDVCAPI_Definition.h \
                         inc=WDVCAPI_Memory.h,WDVCAPI_ErrorInternal.h,WDVCAPI_WDV.h

# Admin
WDVCAPI_Admin.h			 dep=WDVCAPI_Definition.h
WDVCAPI_Admin.c			 dep=WDVCAPI_Definition.h	\
                         inc=WDVCAPI_Admin.h,WDVCAPI_WDV.h,WDVCAPI_Id.h,WDVCAPI_Memory.h,WDVCAPI_Property.h

# Id handling
WDVCAPI_Id.h			 dep=WDVCAPI_Definition.h
WDVCAPI_Id.c			 dep=WDVCAPI_Definition.h	\
                         inc=WDVCAPI_Id.h,WDVCAPI_Common.h,WDVCAPI_ErrorInternal.h,RTE_CompilerFeatures.h,RTE_UniqueId.h,SAPDB_string.h

# Memory Managent
WDVCAPI_Memory.h		 dep=WDVCAPI_Definition.h
WDVCAPI_Memory.c		 dep=WDVCAPI_Definition.h	\
                         inc=WDVCAPI_Memory.h

# XML Indexing
WDVCAPI_XMLIndexing.h	 dep=WDVCAPI_Definition.h
WDVCAPI_XMLIndexing.c	 dep=WDVCAPI_Definition.h	\
                         inc=WDVCAPI_WDV.h,WDVCAPI_Memory.h,WDVCAPI_Property.h,\
						     XMLXPath_Interface.h,XMLXPath_Indexing.h,SAPDB_string.h

# WebDAV handling
WDVCAPI_WDV.h            dep=WDVCAPI_Definition.h \
                         inc=WDVCAPI_ErrorInternal.h,WDVCAPI_Get.h,WDVCAPI_Put.h,\
						     WDVCAPI_Delete.h,WDVCAPI_Head.h,WDVCAPI_MkCol.h,WDVCAPI_Property.h,WDVCAPI_Resource.h,\
							 WDVCAPI_XMLIndexing.h,WDVCAPI_Container.h,\
							 XMLIMAPI_Definition.h
WDVCAPI_WDV.c            inc=WDVCAPI_Memory.h,WDVCAPI_Common.h,WDVCAPI_ErrorInternal.h,\
							 WDVCAPI_WDV.h,WDVCAPI_Id.h,WDVCAPI_Resource.h,WDVCAPI_Admin.h

# Resource Management
WDVCAPI_Resource.h       dep=WDVCAPI_Definition.h
WDVCAPI_Resource.c       inc=WDVCAPI_WDV.h,WDVCAPI_Memory.h,WDVCAPI_Common.h,WDVCAPI_ErrorInternal.h,WDVCAPI_Resource.h,\
							 WDVCAPI_Id.h,WDVCAPI_Container.h,WDVCAPI_Property.h,WDVCAPI_Delete.h,SAPDB_string.h

# Container Management
WDVCAPI_Container.c      dep=WDVCAPI_Definition.h \
						 inc=WDVCAPI_WDV.h,WDVCAPI_Memory.h,WDVCAPI_Common.h,WDVCAPI_ErrorInternal.h,\
							 WDVCAPI_Resource.h,WDVCAPI_Container.h,SAPDB_string.h

# Property Management
WDVCAPI_Property.h       dep=WDVCAPI_Definition.h \
						 inc=XMLXPath_Interface.h,XMLXPath_Indexing.h
WDVCAPI_Property.c       dep=WDVCAPI_Definition.h \
						 inc=WDVCAPI_WDV.h,WDVCAPI_Memory.h,WDVCAPI_Common.h,WDVCAPI_ErrorInternal.h,\
							 WDVCAPI_Resource.h,WDVCAPI_Property.h,WDVCAPI_NameSpace.h,WDVCAPI_Id.h,\
							 WDVCAPI_XMLIndexing.h,\
							 XMLXPath_Interface.h,XMLXPath_Indexing.h,SAPDB_string.h

# Name Space Management
WDVCAPI_NameSpace.c      dep=WDVCAPI_Definition.h \
						 inc=WDVCAPI_WDV.h,WDVCAPI_Common.h,WDVCAPI_ErrorInternal.h,WDVCAPI_Resource.h,\
							 WDVCAPI_NameSpace.h

# Common functions
WDVCAPI_Common.h         dep=WDVCAPI_Definition.h \
                         inc=WDVCAPI_ErrorInternal.h
WDVCAPI_Common.c         dep=WDVCAPI_Definition.h \
                         inc=WDVCAPI_Common.h,WDVCAPI_ErrorInternal.h
# WebDAV: MkCol
WDVCAPI_MkCol.h          inc=vpa00sql.h,vpa00sqlext.h
WDVCAPI_MkCol.c          dep=WDVCAPI_Definition.h \
                         inc=WDVCAPI_WDV.h,WDVCAPI_Memory.h,WDVCAPI_Common.h,WDVCAPI_ErrorInternal.h,\
							 WDVCAPI_Resource.h,WDVCAPI_Id.h

# WebDAV: Get
WDVCAPI_Get.h            inc=
WDVCAPI_Get.c            dep=WDVCAPI_Definition.h \
                         inc=WDVCAPI_WDV.h,WDVCAPI_Memory.h,WDVCAPI_Common.h,WDVCAPI_ErrorInternal.h,WDVCAPI_HandleManager.h,\
						     WDVCAPI_Resource.h,WDVCAPI_Get.h,WDVCAPI_Id.h,WDVCAPI_ZLibIO.h,SAPDB_string.h \
                         dep=OMS_LiveCacheTypes.xml

# WebDAV: Propfind
WDVCAPI_Propfind.c       dep=WDVCAPI_Definition.h \
                         inc=WDVCAPI_WDV.h,WDVCAPI_Memory.h,WDVCAPI_Common.h,WDVCAPI_ErrorInternal.h,WDVCAPI_Resource.h,\
						     WDVCAPI_Id.h,SAPDB_string.h

# WebDAV: Put
WDVCAPI_Put.h            dep=WDVCAPI_Definition.h	\
                         inc=WDVCAPI_XMLIndexing.h,WDVCAPI_ZLibIO.h
WDVCAPI_Put.c            dep=WDVCAPI_Definition.h	\
                         inc=WDVCAPI_Memory.h,WDVCAPI_Common.h,WDVCAPI_Resource.h,WDVCAPI_Property.h,\
							 WDVCAPI_ErrorInternal.h,WDVCAPI_Id.h,WDVCAPI_Put.h,WDVCAPI_WDV.h,WDVCAPI_HandleManager.h,\
							 XMLXPath_Interface.h,XMLXPath_Indexing.h,SAPDB_string.h \
                         dep=OMS_LiveCacheTypes.xml

# WebDAV: Delete
WDVCAPI_Delete.h         dep=WDVCAPI_Definition.h	\
                         inc=WDVCAPI_Resource.h
WDVCAPI_Delete.c         dep=WDVCAPI_Definition.h	\
                         inc=WDVCAPI_Memory.h,WDVCAPI_Common.h,WDVCAPI_ErrorInternal.h,\
                             WDVCAPI_WDV.h,WDVCAPI_Delete.h,WDVCAPI_Id.h,SAPDB_string.h

# WebDAV: Head
WDVCAPI_Head.h           dep=WDVCAPI_Definition.h	\
                         inc=
WDVCAPI_Head.c           dep=WDVCAPI_Definition.h	\
                         inc=WDVCAPI_Memory.h,WDVCAPI_Common.h,WDVCAPI_ErrorInternal.h,\
                             WDVCAPI_Resource.h,WDVCAPI_WDV.h,WDVCAPI_Head.h,WDVCAPI_Id.h

# WebDAV: Copy
WDVCAPI_Copy.h			 dep=WDVCAPI_Definition.h
WDVCAPI_Copy.c			 dep=WDVCAPI_Definition.h \
						 inc=WDVCAPI_WDV.h,WDVCAPI_Copy.h,WDVCAPI_Common.h,WDVCAPI_ErrorInternal.h,\
						     WDVCAPI_Resource.h,WDVCAPI_Id.h

# WebDAV: Move
WDVCAPI_Move.c			 dep=WDVCAPI_Definition.h \
						 inc=WDVCAPI_WDV.h,WDVCAPI_Common.h,WDVCAPI_ErrorInternal.h,WDVCAPI_Resource.h,\
							 WDVCAPI_Copy.h

# WebDAV: Proppatch
WDVCAPI_Proppatch.c      dep=WDVCAPI_Definition.h \
						 inc=WDVCAPI_WDV.h,WDVCAPI_Memory.h,WDVCAPI_Common.h,WDVCAPI_Id.h,WDVCAPI_ErrorInternal.h,\
							 WDVCAPI_Resource.h

# WebDAV: Lock
WDVCAPI_Lock.h           dep=WDVCAPI_Definition.h \
                         inc=WDVCAPI_Resource.h
WDVCAPI_Lock.c           dep=WDVCAPI_Definition.h \
						 inc=WDVCAPI_WDV.h,WDVCAPI_Memory.h,WDVCAPI_Lock.h,WDVCAPI_Common.h,\
							 WDVCAPI_ErrorInternal.h,WDVCAPI_Resource.h,WDVCAPI_Id.h,WDVCAPI_Property.h,SAPDB_string.h

# WebDAV: Handle Manager
WDVCAPI_HandleManager.h		dep=WDVCAPI_Definition.h
WDVCAPI_HandleManager.cpp	dep=WDVCAPI_Definition.h \
							inc=RTE_HandleManager.hpp,WDVCAPI_HandleManager.h


# WebDAV: ZLib compression/decompression
WDVCAPI_ZLibIO.h		 dep=WDVCAPI_Definition.h \
						 inc=:extern/zlib/zutil.h
WDVCAPI_ZLibIO.c		 dep=WDVCAPI_Definition.h \
						 inc=WDVCAPI_ZLibIO.h


# WebDAV schema
WDVCAPI_DBSchema.txt	 ascii ->$DBROOT/env/webdav.ins

# Test modules
WDVCAPI_TestMain.c       dep=WDVCAPI_Definition.h \
						 inc=vpa00sql.h,vpa00sqlext.h,\
                             WDVCAPI_Test.h

WDVCAPI_TestMove.c       dep=WDVCAPI_Definition.h \
						 inc=WDVCAPI_Test.h

WDVCAPI_TestLock.c       dep=WDVCAPI_Definition.h \
						 inc=WDVCAPI_Test.h

WDVCAPI_TestMkCol.c      dep=WDVCAPI_Definition.h \
						 inc=WDVCAPI_Test.h

WDVCAPI_TestDelete.c     dep=WDVCAPI_Definition.h \
						 inc=WDVCAPI_Test.h

#-----------------------------------------------------------------------------

WDVCAPI_ZLibTestMain.c   inc=:extern/zlib/zlib.h,SAPDB_string.h


