/*! 
  -----------------------------------------------------------------------------
  module: hls14_global.h
  -----------------------------------------------------------------------------
  responsible:  SteffenS and d033893

  special area: SAP DB LOADER
 
  description:  Definition of flex return structure (used by scanner/parser) and
                some error definitions

  version:      7.6.
  -----------------------------------------------------------------------------

    ========== licence begin  GPL
    Copyright (c) 1999-2005 SAP AG

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

#ifndef HLS14_GLOBAL_H
#define HLS14_GLOBAL_H

#include "gls00.h"

typedef struct {
    
	/* complex types */
	tls00_TransformationCmd     *C_TransformationCmd;
    tls00_SessionInfo   	    *C_SessionInfo;
    tls00_DBSqlCmd      	    *C_DBSqlCmd;
    tls00_RemarkCmd     	    *C_RemarkCmd;   

    /* Simple types */
	tls00_ConfigurationType      C_ConfigurationType;
	tls00_PartSpec      	    *C_PartSpec;
    tls00_MultSchemaSpec        *C_MultSchemaSpec;
    tls00_SchemaMap             *C_SchemaMap;
    tls00_TableSpec     	    *C_TableSpec;
	tls00_MultColSpec   	    *C_MultColSpec;
    tls00_Column        	    *C_Column;

	tls00_MultFileSpec  	    *C_MultFileSpec;
	tls00_FileSpec      	    *C_FileSpec;
    tls00_FileFormat    	    *C_FileFormat;
	tls00_FormatSpec    	    *C_FormatSpec;

	tls00_FileExtract   	    *C_FileExtract;

    tls00_FieldFormat   	    *C_FieldFormat;
    tls00_FieldPos      	    *C_FieldPos;
    tls00_FieldOption   	    *C_FieldOption;
	tls00_CondSpec      	    *C_CondSpec;
	tls00_Condition     	    *C_Condition;
	tls00_CodeSpec      	    *C_CodeSpec;
	tls00_BoolSpec      	    *C_BoolSpec;
    tls00_DateTimeSpec  	    *C_DateTimeSpec;

    tls00_FieldAsgn     	    *C_FieldAsgn;
   
	tls00_Restart               *C_Restart;
    tls00_Usage                 *C_Usage;

	bool                	    C_Bool;
    tls00_Assign        	    C_Assign;
	tls00_BackupTool    	    C_BackupTool;
	tls00_BackupIdName*         C_BackupIdName;
    tls00_DateTime      	    C_DateTime;
    tls00_DeviceType    	    C_DeviceType;
    tls00_Duplicate     	    C_Duplicate;
    tls00_InOutType     	    C_InOutType;
    tls00_OpType        	    C_OpType;
    tls00_Restore       	    C_Restore;
    tls00_CodeType      	    C_CodeType;
    tsp00_SwapKind      	    C_SwapKind;
    tsp1_sqlmode_Enum   	    C_SqlMode;
    tls00_EncryptionProtocol    C_EncryptionProtocol;

	/* return values of scanner */
    int             		C_Int;
    char            		C_Char;
    char            		C_Char2  [2];
    tls00_String    		C_String;
    
} ScanReturn;

typedef ScanReturn YYSTYPE; /* return value of function yylex */


#define SCANNER_ERROR  -111

#endif //HLS14_GLOBAL_H