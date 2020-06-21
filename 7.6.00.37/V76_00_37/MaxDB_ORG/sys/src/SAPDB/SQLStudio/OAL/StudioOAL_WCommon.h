/*



    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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





*/
#ifndef StudioOAL_WCOMMON_H
#define StudioOAL_WCOMMON_H

/*!**********************************************************************

  module: StudioOAL_WCommon.h

  -----------------------------------------------------------------------

  responsible:  WolfgangA and d033893

  special area: WebQuery, SAP DB LOADER, DBAnalyzer

  description:  Constants for OAL

  see also:

-----------------------------------------------------------------------

  copyright:                Copyright (c) 2000-2005 SAP AG

************************************************************************/

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include "gpr00.h"
#include "geo47.h"

/*must be included before vpa00..*/
#include <WINDOWS.H>

#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>
#include "vpa00sqlod32.h"


#include "SAPDB/ToolsCommon/Tools_DynamicUTF8String.hpp"
#include "SAPDB/ToolsCommon/Tools_UTF8Basis.hpp"
#include "SAPDB/SQLStudio/Studio_List.hpp"

/*messages*/
#define  CANNOT_ANALYZE_ERROR            "Cannot analyze error !"
#define  ROW_NOT_FOUND	      			 "Row not found"
#define  INVALID_ODBC_HANDLE			 "Invalid ODBC Handle !"
#define  STATEMENT_SUCCESSFULLY_EXECUTED "Statement successfully executed"

/*constants*/
const int SQL_PASS_THROUGH               = 1001;

/*define string constants*/
#define  SQLSTATE_SUCCESS				 "00000"
#define  SQLSTATE_DATA_TRUNCATED		 "01004"
#define  OPERATION_CANCELED				 "1008"
#define  REPOFNULLVALUE		             "?"
#define  S1092				             "S1092"	
#define  STRTRUE				         "TRUE"	
#define  STRFALSE			             "FALSE"	

/*-- column handling --*/

/*max length of column name*/
const SAPDB_Int4 MX_COLUMN_NAME			        = 256;
/*max length of date time column value*/
const SAPDB_Int4 MX_TIMESTAMP_COLUMN_VALUE      = 16;
const SAPDB_Int4 MX_TIME_COLUMN_VALUE           = 6;
const SAPDB_Int4 MX_DOUBLE_COLUMN_VALUE	        = 8;
const SAPDB_Int4 MX_COLUMN_GENERIC_NUMERIC      = 64;
const SAPDB_Int4 MX_COLUMN_VALUE			    = 4096;
/*number of bytes to fetch for a long value*/
const SAPDB_Int4 MX_COLUMN_LONG_VALUE		    = 32768;
/*number of bytes for copying into clipboard*/
const SAPDB_Int4 MX_COLUMN_LONG_VALUE_CLIPBOARD = 1048576;

/*-- parameter handling --*/

#define	 PARAM_UNKNOWN		             "Unknown"        
#define  PARAM_IN			             "In"
#define  PARAM_OUT			             "Out"
#define  PARAM_INOUT		             "InOut"
#define	 PARAM_RESULTCOL	             "ResultCol"
#define	 PARAM_RETURNVALUE               "ReturnValue"

const int  MX_BYTEVALUE                     = 32768;	

/*-- error handling --*/

const SAPDB_Int4 MX_SQL_STATE				= 16;
const SAPDB_Int4 MX_ERROR_MSG				= 1024;
const SAPDB_Int4 SQLSTATE_LEN				= 5;
const SAPDB_Int4 NATIVEERROR_SUCCESS		= 0;

typedef Studio_List				  StudioOAL_List;
typedef Tools_DynamicUTF8String   StudioOAL_String;


#define  BREAKOPEN    "["
#define  BREAKCLOSE   "]"
#define  SEMICOLON    ";"
#define  ONESPACE     " "
#define  SUBSTRING1   "POS("
#define  SUBSTRING2   ") "
#define  SUBSTRING3   "[ODBC] ["

#endif

/*===========================================================================*
 *  END
 *===========================================================================*/
