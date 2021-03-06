
	/*===================================================================
	/ Includes
\if EMIT_LICENCE


    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
    ========== licence end


\endif
	/====================================================================*/
	#include "gwd00.h"
	#include "hwd03waapi.h"
	#include "SAPDB/XML/QueryServerLib/XMLQSLib_Query.h"


	#if defined(__cplusplus)
	extern "C"	{
	#endif

	/* free search by providing a where clause */
	SAPDB_Bool XMLQSLIB_FreeSearchEmbedded(XMLQSLib_Handle			qsHandle,
										   XMLQSLib_WhereClause		whereClause,
										   XMLError_Error			hError);

	SAPDB_Bool XMLQSLIB_FreeSearchEmbeddedGetDocs(XMLQSLib_Handle		qsHandle,
												 XMLQSLIB_Char			*docUri,
												 SAPDB_UInt				len,
												 XMLError_Error			hError);

	XMLQSLIB_Bool XMLQSLib_CreateHandle(XMLQSLib_Handle *handle, SQLHENV henv, SQLHDBC hDBC,XMLQSLIB_Char *libname);

	XMLQSLIB_Bool XMLQSLib_DestroyHandle(XMLQSLib_Handle handle);

	#if defined(__cplusplus)
	}
	#endif