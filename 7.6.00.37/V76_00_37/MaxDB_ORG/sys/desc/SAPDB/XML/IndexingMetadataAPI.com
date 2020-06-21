
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

XMLIMAPI_Definition.h	inc=vpa00sql.h,vpa00sqlext.h \
                         ->$INSTROOT/incl/XMLIMAPI_Definition.h

XMLIMAPI_Error.h	dep=XMLIMAPI_Definition.h \
					inc=
XMLIMAPI_Error.c	dep=XMLIMAPI_Definition.h \
					inc=XMLIMAPI_CommonInclude.h,XMLIMAPI_Error.h

XMLIMAPI_CommonInclude.h	inc=gsp100.h,hsp100.h,gwd00.h,heo102.h,hsp77.h,hwd03waapi.h,vpa00sql.h,vpa00sqlext.h

XMLIMAPI_Id.h	dep=XMLIMAPI_Definition.h \
				inc=XMLIMAPI_CommonInclude.h,XMLIMAPI_Error.h,SAPDB/RunTime/RTE_UniqueId.h
XMLIMAPI_Id.c	inc=XMLIMAPI_Id.h,SAPDB_string.h

XMLIMAPI_CommonUtils.h	dep=XMLIMAPI_Definition.h \
						inc=XMLIMAPI_CommonInclude.h
XMLIMAPI_CommonUtils.c	inc=XMLIMAPI_CommonUtils.h

XMLIMAPI_XieAdmin.h	dep=XMLIMAPI_Definition.h \
						inc=XMLIMAPI_CommonInclude.h,XMLIMAPI_Error.h
XMLIMAPI_XieAdmin.c	inc=XMLIMAPI_XieAdmin.h,SAPDB_string.h

XMLIMAPI_XmlIndex.h	dep=XMLIMAPI_Definition.h \
						inc=XMLIMAPI_CommonInclude.h,XMLIMAPI_Error.h
XMLIMAPI_XmlIndex.c	inc=XMLIMAPI_XmlIndex.h,SAPDB_string.h

XMLIMAPI_DocClass.h	dep=XMLIMAPI_Definition.h \
						inc=XMLIMAPI_CommonInclude.h,XMLIMAPI_Error.h
XMLIMAPI_DocClass.c	inc=XMLIMAPI_DocClass.h,SAPDB_string.h

XMLIMAPI_SessionPools.h	dep=XMLIMAPI_Definition.h \
							inc=XMLIMAPI_CommonInclude.h,XMLIMAPI_Error.h
XMLIMAPI_SessionPools.c	inc=XMLIMAPI_SessionPools.h,SAPDB_string.h

XMLIMAPI_ServiceLib.h	dep=XMLIMAPI_Definition.h \
						inc=XMLIMAPI_CommonInclude.h,XMLIMAPI_Error.h
XMLIMAPI_ServiceLib.c	inc=XMLIMAPI_ServiceLib.h

XMLIMAPI_Service.h	dep=XMLIMAPI_Definition.h \
					inc=XMLIMAPI_CommonInclude.h,XMLIMAPI_Error.h
XMLIMAPI_Service.c	inc=XMLIMAPI_Service.h,SAPDB_string.h


XMLIMAPI_Test.c	inc=SAPDB/XML/ModellingAPI/XMLIMAPI_Definition.h,SAPDB/XML/IndexEngine/XMLIdx_Connection.h

# XML Modelling schema
XMLIMAPI_DBSchema.txt	 ascii ->$DBROOT/env/xmlmodelling.ins

