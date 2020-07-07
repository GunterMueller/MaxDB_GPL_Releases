/*



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
#if !defined(CONTENTSTORAGEDEFINES_H)
#define CONTENTSTORAGEDEFINES_H
#define ERR_MSG_LEN 100

// ******************************************************************************
// * version info

#define CONTENT_STORAGE_VERSION  "015"
#define CONTENT_STORAGE_BUILD    "04"
#define CONTENT_STORAGE_VERSION_NO 15
#define CONTENT_STORAGE_BUILD_NO   4
#define CONTENT_STORAGE_FILEVERSION CONTENT_STORAGE_VERSION_NO,\
                                    CONTENT_STORAGE_BUILD_NO
#define CONTENT_STORAGE_FILEVERSION_STR CONTENT_STORAGE_VERSION "."\
                                        CONTENT_STORAGE_BUILD

#define CONTENT_STORAGE_DATE     "Date 2000-05-02"
#define CS_COPYRIGHT             "Copyright (c) 1999-2005 SAP AG"
#define CS_COMPANY_NAME          "SAP AG"
#define CS_PROD_NAME             "SAP Content-Server"

// ******************************************************************************
// * ODBC error code
const char TABLE_NOT_FOUND[] = "S0002";

// ******************************************************************************
// * error code
const int SAPCS_ERR_DUPREC = 200;
const int SAPCS_ERR_NOT_FOUND = 100;
const int SAPCS_ERR_TO_MANY_USERS = -9807;


// ******************************************************************************
// * max data length of a short content
/*
const int MAX_SHORT_LENGTH_DOC = 3200;
const int MAX_SHORT_LENGTH_COMP = 3600;
const int MAX_SHORT_LENGTH_PROP = 512;
const int MAX_COMP_NAME_LENGTH = 254;
const int MAX_DOC_NAME_LENGTH = 64;
*/

// *****************************************************************************
// * SQL statement types
const int NUMBER_STMT_TYPES = 12;

typedef enum
{
	SEL_DOC = 0,
	SEL_DOC_FOR_UPD,
	SEL_COMP,
	SEL_COMP_FOR_UPD,
	INS_DOC,
	INS_COMP,
	INS_COMP_WITH_DUP,
	DEL_DOC,
	DEL_COMP,
	SEL_ID,
    UPD_COMP,
    APP_COMP
}	StmtType;

class DbHdlItem; /* defined in SAPCSConHdl.cpp */

typedef DbHdlItem *DbConnection;

#endif // !defined(CONTENTSTORAGEDEFINES_H)
