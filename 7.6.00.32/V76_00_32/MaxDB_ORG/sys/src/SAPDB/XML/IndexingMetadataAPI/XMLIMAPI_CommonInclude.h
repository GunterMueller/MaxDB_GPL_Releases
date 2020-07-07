#ifndef WDVHANDLER_COMMONINCLUDE_H
#define WDVHANDLER_COMMONINCLUDE_H

/****************************************************************************

  module      : WDVHandler_CommonInclude.h

  -------------------------------------------------------------------------

  responsible : Daniel Kirmse

  special area: 
  description : 


  last changed: 2001-03-01  9:35
  see also    : 

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2001-2005 SAP AG

    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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


*****************************************************************************/

	#define VSP100
	#include "gsp100.h"
	#include "hsp100.h"

	#include "gwd00.h"
	#include "heo102.h"
	#include "hsp77.h"
	#include "sapdbwa.h" /*nocheck*/
	#include "hwd03waapi.h"
	#include <time.h>
	#include "vpa00sql.h"
	#include "vpa00sqlext.h"


/*	#include "SAPDB/XML/ModellingAPI/XMLMdlAPI_Error.h"
*/
	/*------------------------------------------------------------------------
		XMLIMAPI HANDLE
	--------------------------------------------------------------------------*/
	struct st_xmlimapi_handle {
		XMLIMAPI_ErrorList			errorList;

		SQLHDBC						sqlHdbc;
		SQLHENV						sqlHenv;

		SQLHSTMT					stmtCommit;
		SQLHSTMT					stmtRollback;
		SQLHSTMT					stmtXmlIndexCreate;
		SQLHSTMT					stmtXmlIndexUpdate;
		SQLHSTMT					stmtXmlIndexDelete;
		SQLHSTMT					stmtGetXmlIndex;
		SQLHSTMT					stmtGetXmlIndices;
		SQLHSTMT					stmtGetXmlIndicesIncl;
		SQLHSTMT					stmtIndicesForDC;
		SQLHSTMT					stmtIndicesForDCs;
		SQLHSTMT					stmtIndicesForDCInfoExcl;
		SQLHSTMT					stmtXmlIndexCheckAssign;

		SQLHSTMT					stmtDocClassCreate;
		SQLHSTMT					stmtDocClassUpdate;
		SQLHSTMT					stmtDocClassDelete;
		SQLHSTMT					stmtGetDocClass;
		SQLHSTMT					stmtGetDocClassId;
		SQLHSTMT					stmtGetDocClassName;
		SQLHSTMT					stmtGetDocClasses;
		SQLHSTMT					stmtGetDocClassesExcl;
		SQLHSTMT					stmtGetDocClassesIncl;
		SQLHSTMT					stmtDocClassDeleteAssign;
		SQLHSTMT					stmtDocClassInsertAssign;

		SQLHSTMT					stmtSessionPoolCreate;
		SQLHSTMT					stmtSessionPoolUpdate;
		SQLHSTMT					stmtSessionPoolDelete;
		SQLHSTMT					stmtSessionPoolCheckIS;
		SQLHSTMT					stmtSessionPoolGet;
		SQLHSTMT					stmtSessionPoolGetList;
		SQLHSTMT					stmtSessionPoolGetAssign;

		SQLHSTMT					stmtServiceCreate;
		SQLHSTMT					stmtServiceUpdate;
		SQLHSTMT					stmtServiceDelete;
		SQLHSTMT					stmtServiceGet;
		SQLHSTMT					stmtServiceGetId;
		SQLHSTMT					stmtServiceGetList;
		SQLHSTMT					stmtServiceGetByName;

		SQLHSTMT					stmtXieRegisterInsertIdxLib;
		SQLHSTMT					stmtXieRegisterUpdateIdxLib;
		SQLHSTMT					stmtXieRegisterInsertXie;
		SQLHSTMT					stmtXieRegisterUpdateXie;
		SQLHSTMT					stmtXieSetAlive;
		SQLHSTMT					stmtXieGetRefresh;
		SQLHSTMT					stmtXieUnSetRefresh;
		SQLHSTMT					stmtXieUnRegister;
		SQLHSTMT					stmtXieGet;
		SQLHSTMT					stmtXieGetList;

		SQLLEN   					stmtIdIndicator;

		XMLIMAPI_Id					stmtXmlIndexID;
		XMLIMAPI_Id					stmtDocClassID;
		XMLIMAPI_Id					stmtXmlIndexID2;
		XMLIMAPI_Id					stmtXmlIndexID3;
		XMLIMAPI_Id					stmtXmlIndexID4;
		XMLIMAPI_Id					stmtXmlIndexID5;
		XMLIMAPI_Bool				flgNoDocClasses;

		XMLIMAPI_XmlIndex			stmtXmlIndexCol; /* bindCol */
		XMLIMAPI_XmlIndex			stmtXmlIndexParam; /* bindParam */

		XMLIMAPI_DocClass			stmtDocClassCol; /* bindCol */
		XMLIMAPI_DocClass			stmtDocClassParam; /* bindParam */

		XMLIMAPI_ServiceDesc		stmtServiceParam;
		XMLIMAPI_ServiceDesc		stmtServiceCol;

		XMLIMAPI_SessionPool		stmtSessionPoolParam;
		XMLIMAPI_SessionPool		stmtSessionPoolCol;

		XMLIMAPI_XieAdmin			stmtXieAdminParam;
		XMLIMAPI_XieAdmin			stmtXieAdminCol;

	};

	XMLIMAPI_Bool Commit(XMLIMAPI_Handle handle);
	XMLIMAPI_Bool Rollback(XMLIMAPI_Handle handle);

#endif