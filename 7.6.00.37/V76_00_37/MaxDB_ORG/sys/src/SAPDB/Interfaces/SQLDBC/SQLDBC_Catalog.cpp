/*!
  @file           SQLDBC_Catalog.cpp
  @author         D024584
  @ingroup        SQLDBC
  @brief          Optional catalog functions according to the ODBC reference
  
  @see            

\if EMIT_LICENCE
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

\endif
*/

#include "SAPDB/Interfaces/SQLDBC/SQLDBC_IRuntime.h"
#include "SAPDB/Interfaces/SQLDBC/SQLDBC_ClientRuntime.h"
#include "SAPDB/Interfaces/SQLDBC/SQLDBC_Catalog.h"

namespace SQLDBC {

#define ESCAPE_CHAR '\\'

char * SQLDBC_Catalog::getBuffer (unsigned long size)
{
  SQLDBC_IRuntime *m_runtime;
  char errorText[512];

  m_runtime = SQLDBC::GetClientRuntime (errorText, sizeof(errorText));

  if (m_runtime != NULL)
    return new (m_runtime->getGlobalAllocator()) char[size];
  else
    return NULL;
}

const char * SQLDBC_Catalog::getTablePrefix (SQLDBC_Int4 kernelVersion)
{
  if (kernelVersion >= 70600)
    return getNewPrefix();       /* schema for all system views/tables */
  else
    return getOldPrefix();
}

SQLDBC_Retcode SQLDBC_Catalog::SQLGetTypeInfo (SQLDBC_Statement *stmt,
                                               SQLDBC_Int2       dataType,
                                               SQLDBC_Version odbcVersion,
                                               SQLDBC_StringEncoding encoding)
{
  const char SQLCommand_GetTypeInfoTemplate_v2[] =
"SELECT TYPE_NAME, DATA_TYPE, PRECISION, LITERAL_PREFIX, LITERAL_SUFFIX, \
CREATE_PARAMS, NULLABLE, CASE_SENSITIVE, SEARCHABLE, UNSIGNED_ATTRIBUTE, \
MONEY, AUTO_INCREMENT, LOCAL_TYPE_NAME, MINIMUM_SCALE, \
MAXIMUM_SCALE FROM %sTYPES ";

  const char SQLCommand_GetTypeInfoTemplate_v3[] =
"SELECT TYPE_NAME, decode (DATA_TYPE, \
%d, %d, %d, %d, %d, %d, \
DATA_TYPE) DATA_TYPE, PRECISION COLUMN_SIZE, LITERAL_PREFIX, \
LITERAL_SUFFIX, \
decode (CREATE_PARAMS, 'max length', 'length', CREATE_PARAMS) CREATE_PARAMS, \
NULLABLE, CASE_SENSITIVE, SEARCHABLE, UNSIGNED_ATTRIBUTE, \
MONEY FIXED_PREC_SCALE, AUTO_INCREMENT AUTO_UNIQUE_VALUE, LOCAL_TYPE_NAME, \
MINIMUM_SCALE, \
MAXIMUM_SCALE, SQL_DATA_TYPE, SQL_DATETIME_SUB, NUM_PREC_RADIX, \
INTERVAL_PRECISION FROM %sTYPES ";

  const char SQLCommand_Where[] = " WHERE DATA_TYPE = %d";

  const int SQL_ALL_TYPES = 0;
  const int SQL_DATE = 9;
  const int SQL_TIME = 10;
  const int SQL_TIMESTAMP = 11;
  const int SQL_TYPE_DATE = 91;
  const int SQL_TYPE_TIME = 92;
  const int SQL_TYPE_TIMESTAMP = 93;
    

  const tsp77encoding   *encodingType;
  unsigned int position = 0;
  unsigned int bufferSize = 0;
  SQLDBC_Int4  kernelVersion;
  char *buffer;

  if (stmt == NULL)
    return SQLDBC_NOT_OK;

  kernelVersion = stmt->getKernelVersion();

  if (encoding == SQLDBC_StringEncodingAscii)
    encodingType = sp77encodingAscii;
  else
    encodingType = sp77nativeUnicodeEncoding ();

  const int charSize = encodingType->fixedCharacterSize;

  // allocate maximal length of SQL statement
  bufferSize = charSize * (sizeof (SQLCommand_GetTypeInfoTemplate_v3) +
                           sizeof (SQLCommand_Where) + 50);
  buffer = getBuffer (bufferSize);

  /* map ODBC 3 types to ODBC 2 types */
  switch (dataType) {
  case (SQL_TYPE_DATE):
    dataType = SQL_DATE;       break;
  case (SQL_TYPE_TIME):
    dataType = SQL_TIME;       break;
  case (SQL_TYPE_TIMESTAMP):
    dataType = SQL_TIMESTAMP;  break;
  default: {}
  }

  if (odbcVersion == ODBC3)
    position = sp77sprintfUnicode (encodingType,
                                   buffer, bufferSize, 
                                   SQLCommand_GetTypeInfoTemplate_v3,
                                   SQL_DATE, SQL_TYPE_DATE,
                                   SQL_TIME, SQL_TYPE_TIME,
                                   SQL_TIMESTAMP, SQL_TYPE_TIMESTAMP,
                                   getTablePrefix (kernelVersion));
  else
    position = sp77sprintfUnicode (encodingType,
                                   buffer, bufferSize, 
                                   SQLCommand_GetTypeInfoTemplate_v2,
                                   getTablePrefix (kernelVersion));


  if (dataType != SQL_ALL_TYPES)
    position += sp77sprintfUnicode (encodingType,
                                    buffer+position, bufferSize-position, 
                                    SQLCommand_Where,
                                    dataType);

  return stmt->execute (buffer, position, encoding);

}

SQLDBC_Retcode SQLDBC_Catalog::SQLColumns (SQLDBC_Statement *stmt,
                                           void             *szCatalogName,
                                           SQLDBC_Int2       cbCatalogName,
                                           void             *szSchemaName,
                                           SQLDBC_Int2       cbSchemaName,
                                           void             *szTableName,
                                           SQLDBC_Int2       cbTableName,
                                           void             *szColumnName,
                                           SQLDBC_Int2       cbColumnName,
                                           SQLDBC_Version         odbcVersion,
                                           SQLDBC_Bool            pattern,
                                           SQLDBC_StringEncoding  encoding)
{
  const char colname1_v2[]  = "TABLE_QUALIFIER";
  const char colname2_v2[]  = "TABLE_OWNER";
  const char colname7_v2[]  = "PRECISION";
  const char colname8_v2[]  = "\"LENGTH\"";
  const char colname9_v2[]  = "SCALE";
  const char colname10_v2[] = "RADIX";

  const char colname1_v3[]  = "TABLE_CAT";
  const char colname2_v3[]  = "TABLE_SCHEM";
  const char colname7_v3[]  = "COLUMN_SIZE";
  const char colname8_v3[]  = "BUFFER_LENGTH";
  const char colname9_v3[]  = "DECIMAL_DIGITS";
  const char colname10_v3[] = "NUM_PREC_RADIX";

  const char SQLCommand_ColumnsTemplate[] =
"SELECT TABLE_QUALIFIER %s, TABLE_OWNER %s, TABLE_NAME, COLUMN_NAME, DATA_TYPE, \
TYPE_NAME, PRECISION %s, BUFFER_LENGTH %s, DECIMAL_DIGITS %s, NUM_PREC_RADIX %s, NULLABLE, REMARKS %s\
FROM %sCOLUMNS WHERE TABLE_SCHEM LIKE '%'=.*S' ESCAPE '%c' AND \ 
TABLE_NAME LIKE '%'=.*S' ESCAPE '%c' AND COLUMN_NAME LIKE '%'=.*S' ESCAPE '%c' \
ORDER BY 2, 3, 13";

  const char SQLCommand_ExtraColumnsTemplate[] = 
"COLUMN_DEF, DATA_TYPE AS SQL_DATA_TYPE, \
NULL AS SQL_DATETIME_SUB, \
COLUMN_SIZE AS CHAR_OCTET_LENGTH, ORDINAL_POSITION, \
IS_NULLABLE ";


  ///////////////////////////////////////////////////////////////////////

  SQLDBC_Retcode retcode = SQLDBC_OK;
  const tsp77encoding   *encodingType;
  unsigned int position = 0;
  unsigned int bufferSize = 0;
  SQLDBC_Int4  kernelVersion;

  char *buffer;
  char percent[2];
  const char *column1;      // column names due to differences between version 2 and 3
  const char *column2, *column7, *column8, *column9, *column10;
  const char *extraColumns;

  if (stmt == NULL)
    return SQLDBC_NOT_OK;

  kernelVersion = stmt->getKernelVersion();

  if (encoding == SQLDBC_StringEncodingAscii)
    encodingType = sp77encodingAscii;
  else
    encodingType = sp77nativeUnicodeEncoding ();

  const int charSize = encodingType->fixedCharacterSize;

  // allocate maximal length of SQL statement
  bufferSize = charSize * (sizeof (SQLCommand_ColumnsTemplate) + 200 +
                           sizeof (SQLCommand_ExtraColumnsTemplate) + 6*sizeof(colname1_v2));
  buffer = getBuffer (bufferSize);

  // set column names for first and second column regarding ODBC version
  if (odbcVersion == ODBC3)  {
    column1  = colname1_v3;
    column2  = colname2_v3;
    column7  = colname7_v3;
    column8  = colname8_v3;
    column9  = colname9_v3;
    column10 = colname10_v3;
    extraColumns = SQLCommand_ExtraColumnsTemplate;
  } else {
    column1  = colname1_v2;
    column2  = colname2_v2;
    column7  = colname7_v2;
    column8  = colname8_v2;
    column9  = colname9_v2;
    column10 = colname10_v2;
    extraColumns = "";
  }

  // percent in the right encoding
  sp77sprintfUnicode (encodingType,
                      percent, sizeof(percent), 
                      "%%");

  if (szSchemaName == NULL  ||  cbSchemaName == 0) {
    szSchemaName = percent;
    cbSchemaName = charSize;
  }

  if (szTableName == NULL  ||  cbTableName == 0) {
    szTableName = percent;
    cbTableName = charSize;
  }

  if (szColumnName == NULL  ||  cbColumnName == 0) {
    szColumnName = percent;
    cbColumnName = charSize;
  }

  position = sp77sprintfUnicode (encodingType,
                                 buffer, bufferSize, 
                                 SQLCommand_ColumnsTemplate,
                                 column1, column2, column7,
                                 column8, column9, column10,
                                 extraColumns,
                                 getTablePrefix (kernelVersion),
                                 encodingType,
                                 cbSchemaName * charSize,
                                 szSchemaName,
                                 ESCAPE_CHAR,
                                 encodingType,
                                 cbTableName * charSize,
                                 szTableName,
                                 ESCAPE_CHAR,
                                 encodingType,
                                 cbColumnName * charSize,
                                 szColumnName,
                                 ESCAPE_CHAR);

  return stmt->execute (buffer, position, encoding);
}

SQLDBC_Retcode SQLDBC_Catalog::SQLColumnPrivileges(SQLDBC_Statement      *stmt,
                                                   void                  *szCatalogName,
                                                   SQLDBC_Int2            cbCatalogName,
                                                   void                  *szSchemaName,
                                                   SQLDBC_Int2            cbSchemaName,
                                                   void                  *szTableName,
                                                   SQLDBC_Int2            cbTableName,
                                                   void                  *szColumnName,
                                                   SQLDBC_Int2            cbColumnName,
                                                   SQLDBC_Version         odbcVersion,
                                                   SQLDBC_Bool            pattern,
                                                   SQLDBC_StringEncoding  encoding)
{
  const char colname1_v2[]  = "TABLE_QUALIFIER";
  const char colname2_v2[]  = "TABLE_OWNER";

  const char colname1_v3[]  = "TABLE_CAT";
  const char colname2_v3[]  = "TABLE_SCHEM";

  const char SQLCommand_ColumnPrivilegesTemplate[] =
"SELECT TABLE_QUALIFIER %s, TABLE_OWNER %s, TABLE_NAME, COLUMN_NAME, GRANTOR, \
GRANTEE, PRIVILEGE, IS_GRANTABLE FROM %sCOLUMNPRIVILEGES \
WHERE TABLE_OWNER = '%'=.*S' AND TABLE_NAME = '%'=.*S' AND COLUMN_NAME LIKE '%'=.*S' \
ESCAPE '%c' ORDER BY 2, 3, 4, 7 ";

  ///////////////////////////////////////////////////////////////////////

  SQLDBC_Retcode retcode = SQLDBC_OK;
  const tsp77encoding   *encodingType;
  unsigned int position = 0;
  unsigned int bufferSize = 0;
  SQLDBC_Int4  kernelVersion;

  char *buffer;
  char percent[2];
  const char *column1;      // column names due to differences between version 2 and 3
  const char *column2;

  if (stmt == NULL)
    return SQLDBC_NOT_OK;

  kernelVersion = stmt->getKernelVersion();

  if (encoding == SQLDBC_StringEncodingAscii)
    encodingType = sp77encodingAscii;
  else
    encodingType = sp77nativeUnicodeEncoding ();

  const int charSize = encodingType->fixedCharacterSize;

  // allocate maximal length of SQL statement
  bufferSize = charSize * (sizeof (SQLCommand_ColumnPrivilegesTemplate) + 200 +
                           2*sizeof(colname1_v2));
  buffer = getBuffer (bufferSize);

  // set column names for first and second column regarding ODBC version
  if (odbcVersion == ODBC3)  {
    column1  = colname1_v3;
    column2  = colname2_v3;
  } else {
    column1  = colname1_v2;
    column2  = colname2_v2;
  }

  // percent in the right encoding
  sp77sprintfUnicode (encodingType,
                      percent, sizeof(percent), 
                      "%%");

  if (szSchemaName == NULL  ||  cbSchemaName == 0) {
    szSchemaName = percent;
    cbSchemaName = charSize;
  }

  if (szTableName == NULL  ||  cbTableName == 0) {
    szTableName = percent;
    cbTableName = charSize;
  }

  if (szColumnName == NULL  ||  cbColumnName == 0) {
    szColumnName = percent;
    cbColumnName = charSize;
  }

  position = sp77sprintfUnicode (encodingType,
                                 buffer, bufferSize, 
                                 SQLCommand_ColumnPrivilegesTemplate,
                                 column1, column2,
                                 getTablePrefix (kernelVersion),
                                 encodingType,
                                 cbSchemaName * charSize,
                                 szSchemaName,
                                 encodingType,
                                 cbTableName * charSize,
                                 szTableName,
                                 encodingType,
                                 cbColumnName * charSize,
                                 szColumnName,
                                 ESCAPE_CHAR);

  return stmt->execute (buffer, position, encoding);
}

SQLDBC_Retcode SQLDBC_Catalog::SQLForeignKeys (SQLDBC_Statement      *stmt,
                                 void                  *szPkCatalogName,
                                 SQLDBC_Int2            cbPkCatalogName,
                                 void                  *szPkSchemaName,
                                 SQLDBC_Int2            cbPkSchemaName,
                                 void                  *szPkTableName,
                                 SQLDBC_Int2            cbPkTableName,
                                 void                  *szFkCatalogName,
                                 SQLDBC_Int2            cbFkCatalogName,
                                 void                  *szFkSchemaName,
                                 SQLDBC_Int2            cbFkSchemaName,
                                 void                  *szFkTableName,
                                 SQLDBC_Int2            cbFkTableName,
                                 SQLDBC_Version         odbcVersion,
                                 SQLDBC_Bool            pattern,
                                 SQLDBC_StringEncoding  encoding)
{
  const char colname1_v2[]  = "PKTABLE_QUALIFIER";
  const char colname2_v2[]  = "PKTABLE_OWNER";
  const char colname5_v2[]  = "FKTABLE_QUALIFIER";
  const char colname6_v2[]  = "FKTABLE_OWNER";
  const char colname14_v2[] = "";

  const char colname1_v3[]  = "PKTABLE_CAT";
  const char colname2_v3[]  = "PKTABLE_SCHEM";
  const char colname5_v3[]  = "FK_TABLE_CAT";
  const char colname6_v3[]  = "FKTABLE_SCHEM";
  const char colname14_v3[] = ", DEFERRABILITY";

  const char SQLCommand_ForeignKeysTemplate[] =
"SELECT PKTABLE_CAT %s, PKTABLE_SCHEM %s, \
PKTABLE_NAME, PKCOLUMN_NAME, FK_TABLE_CAT %s, FKTABLE_SCHEM %s, \
FKTABLE_NAME, FKCOLUMN_NAME, KEY_SEQ, \
UPDATE_RULE, DELETE_RULE, FK_NAME, PK_NAME, %s \
FROM %sFOREIGNKEYS WHERE ";

const char SQLCommand_WherePK[] =
"PKTABLE_OWNER = '%'=.*S' AND PKTABLE_NAME = '%'=.*S' ";
const char SQLCommand_WhereFK[] =
"FKTABLE_OWNER = '%'=.*S' AND FKTABLE_NAME = '%'=.*S' ";

 const int PK_Scan = 1;
 const int FK_Scan = 2;
 const int PKandFK_Scan = 3;

const char SQLCommand_OrderPK[] = "ORDER BY 2, 3, 9";
const char SQLCommand_OrderFK[] = "ORDER BY 6, 7 ";

  ///////////////////////////////////////////////////////////////////////
 int scanMode = PK_Scan;
  SQLDBC_Retcode retcode = SQLDBC_OK;
  const tsp77encoding   *encodingType;
  unsigned int position = 0;
  unsigned int bufferSize = 0;
  SQLDBC_Int4  kernelVersion;

  char *buffer;
  char percent[2];
  const char *column1;      // column names due to differences between version 2 and 3
  const char *column2;
  const char *column5;
  const char *column6;
  const char *column14;

  if (stmt == NULL)
    return SQLDBC_NOT_OK;

  kernelVersion = stmt->getKernelVersion();

  if (encoding == SQLDBC_StringEncodingAscii)
    encodingType = sp77encodingAscii;
  else
    encodingType = sp77nativeUnicodeEncoding ();

  const int charSize = encodingType->fixedCharacterSize;

  // allocate maximal length of SQL statement
  bufferSize = charSize * (sizeof (SQLCommand_ForeignKeysTemplate) +
                           sizeof (SQLCommand_WherePK) +
                           sizeof (SQLCommand_WhereFK) +
                           sizeof (SQLCommand_OrderPK) +
                           sizeof (SQLCommand_OrderFK) +
                           5*sizeof(colname1_v2) + 20);
  buffer = getBuffer (bufferSize);

  // set column names for first and second column regarding ODBC version
  if (odbcVersion == ODBC3)  {
    column1  = colname1_v3;
    column2  = colname2_v3;
    column5  = colname5_v3;
    column6  = colname6_v3;
    column14 = colname14_v3;
  } else {
    column1  = colname1_v2;
    column2  = colname2_v2;
    column5  = colname5_v2;
    column6  = colname6_v2;
    column14 = colname14_v2;
  }

  // determine where clause
  if (szPkTableName != NULL  &&  szFkTableName == NULL)
    scanMode = PK_Scan;

  if (szPkTableName == NULL  && szFkTableName != NULL)
    scanMode = FK_Scan;

  if (szPkTableName != NULL  && szFkTableName != NULL)
    scanMode = PKandFK_Scan;

  // percent in the right encoding
  sp77sprintfUnicode (encodingType,
                      percent, sizeof(percent), 
                      "%%");

  if (szPkSchemaName == NULL  ||  cbPkSchemaName == 0) {
    szPkSchemaName = percent;
    cbPkSchemaName = charSize;
  }

  if (szPkTableName == NULL  ||  cbPkTableName == 0) {
    szPkTableName = percent;
    cbPkTableName = charSize;
  }

  if (szFkSchemaName == NULL  ||  cbFkSchemaName == 0) {
    szFkSchemaName = percent;
    cbFkSchemaName = charSize;
  }

  if (szFkTableName == NULL  ||  cbFkTableName == 0) {
    szFkTableName = percent;
    cbFkTableName = charSize;
  }

  position = sp77sprintfUnicode (encodingType,
                                 buffer, bufferSize, 
                                 SQLCommand_ForeignKeysTemplate,
                                 column1, column2,
                                 column5, column6, column14,
                                 getTablePrefix (kernelVersion));


  switch (scanMode) {
  case PK_Scan:
    position += sp77sprintfUnicode (encodingType,
                                    buffer+position, bufferSize-position, 
                                    SQLCommand_WherePK,
                                    encodingType,
                                    cbPkSchemaName * charSize,
                                    szPkSchemaName,
                                    encodingType,
                                    cbPkTableName * charSize,
                                    szPkTableName,
                                    encodingType);
    position += sp77sprintfUnicode (encodingType,
                                    buffer+position, bufferSize-position, 
                                    SQLCommand_OrderPK);
    break;
  case FK_Scan:
    position += sp77sprintfUnicode (encodingType,
                                    buffer+position, bufferSize-position, 
                                    SQLCommand_WhereFK,
                                    encodingType,
                                    cbFkSchemaName * charSize,
                                    szFkSchemaName,
                                    encodingType,
                                    cbFkTableName * charSize,
                                    szFkTableName,
                                    encodingType);
    position += sp77sprintfUnicode (encodingType,
                                    buffer+position, bufferSize-position, 
                                    SQLCommand_OrderFK);
    break;
  case PKandFK_Scan:
    position += sp77sprintfUnicode (encodingType,
                                    buffer+position, bufferSize-position, 
                                    SQLCommand_WherePK,
                                    encodingType,
                                    cbPkSchemaName * charSize,
                                    szPkSchemaName,
                                    encodingType,
                                    cbPkTableName * charSize,
                                    szPkTableName,
                                    encodingType);
    position += sp77sprintfUnicode (encodingType,
                                    buffer+position, bufferSize-position, 
                                    " AND ");
    position += sp77sprintfUnicode (encodingType,
                                    buffer+position, bufferSize-position, 
                                    SQLCommand_WhereFK,
                                    encodingType,
                                    cbFkSchemaName * charSize,
                                    szFkSchemaName,
                                    encodingType,
                                    cbFkTableName * charSize,
                                    szFkTableName,
                                    encodingType);
    position += sp77sprintfUnicode (encodingType,
                                    buffer+position, bufferSize-position, 
                                    SQLCommand_OrderPK);
    break;
  }

  return stmt->execute (buffer, position, encoding);
}

SQLDBC_Retcode SQLDBC_Catalog::SQLProcedureColumns (SQLDBC_Statement      *stmt,
                                      void                  *szCatalogName,
                                      SQLDBC_Int2            cbCatalogName,
                                      void                  *szSchemaName,
                                      SQLDBC_Int2            cbSchemaName,
                                      void                  *szProcName,
                                      SQLDBC_Int2            cbProcName,
                                      void                  *szColumnName,
                                      SQLDBC_Int2            cbColumnName,
                                      SQLDBC_Version         odbcVersion,
                                      SQLDBC_Bool            pattern,
                                      SQLDBC_StringEncoding  encoding)
{
  const char colname1_v2[]  = "PROCEDURE_QUALIFIER";
  const char colname2_v2[]  = "PROCEDURE_OWNER";
  const char colname8_v2[]  = "PRECISION";
  const char colname9_v2[]  = "\"LENGTH\"";
  const char colname10_v2[] = "SCALE";
  const char colname11_v2[] = "RADIX";

  const char colname1_v3[]  = "PROCEDURE_CAT";
  const char colname2_v3[]  = "PROCEDURE_SCHEM";
  const char colname8_v3[]  = "COLUMN_SIZE";
  const char colname9_v3[]  = "BUFFER_LENGTH";
  const char colname10_v3[] = "DECIMAL_DIGITS";
  const char colname11_v3[] = "NUM_PREC_RADIX";
  const char SQLCommand_ProceduresTemplate[] =
"SELECT PROCEDURE_CAT %s, PROCEDURE_SCHEM %s, PROCEDURE_NAME, COLUMN_NAME, \
COLUMN_TYPE, DATA_TYPE, TYPE_NAME, PRECISION %s, \"LENGTH\" %s, SCALE %s, RADIX %s, \
NULLABLE, REMARKS, CODETYPE %s FROM %sPROCCOLUMNS WHERE \
PROCEDURE_OWNER LIKE '%'=.*S' ESCAPE '%c' AND PROCEDURE_NAME LIKE '%'=.*S' \
ESCAPE '%c' AND COLUMN_NAME LIKE '%'=.*S' ESCAPE '%c' ORDER BY 2, 3, ORDINAL_POSITION ";

  const char SQLCommand_ExtraColumns_v3[] =
", COLUMN_DEF, SQL_DATA_TYPE, SQL_DATETIME_SUB, \
CHAR_OCTET_LENGTH, ORDINAL_POSITION, IS_NULLABLE ";

  ///////////////////////////////////////////////////////////////////////

  SQLDBC_Retcode retcode = SQLDBC_OK;
  const tsp77encoding   *encodingType;
  unsigned int position = 0;
  unsigned int bufferSize = 0;
  SQLDBC_Int4  kernelVersion;

  char *buffer;
  char percent[2];
  const char *column1;      // column names due to differences between version 2 and 3
  const char *column2;
  const char *column8,  *column9;
  const char *column10, *column11;
  const char *extraColumns;

  if (stmt == NULL)
    return SQLDBC_NOT_OK;

  kernelVersion = stmt->getKernelVersion();

  if (encoding == SQLDBC_StringEncodingAscii)
    encodingType = sp77encodingAscii;
  else
    encodingType = sp77nativeUnicodeEncoding ();

  const int charSize = encodingType->fixedCharacterSize;

  // allocate maximal length of SQL statement
  bufferSize = charSize * (sizeof (SQLCommand_ProceduresTemplate) + 200 +
                           sizeof (SQLCommand_ExtraColumns_v3) +
                           6*sizeof(colname1_v2));
  buffer = getBuffer (bufferSize);

  // set column names for first and second column regarding ODBC version
  if (odbcVersion == ODBC3)  {
    column1  = colname1_v3;
    column2  = colname2_v3;
    column8  = colname8_v3;
    column9  = colname9_v3;
    column10 = colname10_v3;
    column11 = colname11_v3;
    extraColumns = SQLCommand_ExtraColumns_v3;
  } else {
    column1  = colname1_v2;
    column2  = colname2_v2;
    column8  = colname8_v2;
    column9  = colname9_v2;
    column10 = colname10_v2;
    column11 = colname11_v2;
    extraColumns = "";
  }

  // percent in the right encoding
  sp77sprintfUnicode (encodingType,
                      percent, sizeof(percent), 
                      "%%");

  if (szSchemaName == NULL  ||  cbSchemaName == 0) {
    szSchemaName = percent;
    cbSchemaName = charSize;
  }

  if (szProcName == NULL  ||  cbProcName == 0) {
    szProcName = percent;
    cbProcName = charSize;
  }

  position = sp77sprintfUnicode (encodingType,
                                 buffer, bufferSize, 
                                 SQLCommand_ProceduresTemplate,
                                 column1, column2,
                                 column8, column9,
                                 column10, column11, extraColumns,
                                 getTablePrefix (kernelVersion),
                                 encodingType,
                                 cbSchemaName * charSize,
                                 szSchemaName,
                                 ESCAPE_CHAR,
                                 encodingType,
                                 cbProcName * charSize,
                                 szProcName,
                                 encodingType,
                                 ESCAPE_CHAR);

  return stmt->execute (buffer, position, encoding);
}

SQLDBC_Retcode SQLDBC_Catalog::SQLProcedures (SQLDBC_Statement      *stmt,
                                              void                  *szCatalogName,
                                              SQLDBC_Int2            cbCatalogName,
                                              void                  *szSchemaName,
                                              SQLDBC_Int2            cbSchemaName,
                                              void                  *szProcName,
                                              SQLDBC_Int2            cbProcName,
                                              SQLDBC_Version         odbcVersion,
                                              SQLDBC_Bool            pattern,
                                              SQLDBC_StringEncoding  encoding)
{
  const char colname1_v2[]  = "PROCEDURE_QUALIFIER";
  const char colname2_v2[]  = "PROCEDURE_OWNER";

  const char colname1_v3[]  = "PROCEDURE_CAT";
  const char colname2_v3[]  = "PROCEDURE_SCHEM";

  const char SQLCommand_ProceduresTemplate[] =
"SELECT PROCEDURE_CAT %s,  PROCEDURE_SCHEM %s, PROCEDURE_NAME, \
NUM_INPUT_PARAMS, NUM_OUTPUT_PARAMS, NUM_RESULT_SETS, REMARKS, \
PROCEDURE_TYPE FROM %sPROCEDURES \ 
WHERE PROCEDURE_OWNER LIKE '%'=.*S' ESCAPE '%c' AND \ 
PROCEDURE_NAME LIKE '%'=.*S' ESCAPE '%c' ORDER BY 2, 3 ";

  ///////////////////////////////////////////////////////////////////////

  SQLDBC_Retcode retcode = SQLDBC_OK;
  const tsp77encoding   *encodingType;
  unsigned int position = 0;
  unsigned int bufferSize = 0;
  SQLDBC_Int4  kernelVersion;

  char *buffer;
  char percent[2];
  const char *column1;      // column names due to differences between version 2 and 3
  const char *column2;

  if (stmt == NULL)
    return SQLDBC_NOT_OK;

  kernelVersion = stmt->getKernelVersion();

  if (encoding == SQLDBC_StringEncodingAscii)
    encodingType = sp77encodingAscii;
  else
    encodingType = sp77nativeUnicodeEncoding ();

  const int charSize = encodingType->fixedCharacterSize;

  // allocate maximal length of SQL statement
  bufferSize = charSize * (sizeof (SQLCommand_ProceduresTemplate) + 200 +
                           2*sizeof(colname1_v2));
  buffer = getBuffer (bufferSize);

  // set column names for first and second column regarding ODBC version
  if (odbcVersion == ODBC3)  {
    column1  = colname1_v3;
    column2  = colname2_v3;
  } else {
    column1  = colname1_v2;
    column2  = colname2_v2;
  }

  // percent in the right encoding
  sp77sprintfUnicode (encodingType,
                      percent, sizeof(percent), 
                      "%%");

  if (szSchemaName == NULL  ||  cbSchemaName == 0) {
    szSchemaName = percent;
    cbSchemaName = charSize;
  }

  if (szProcName == NULL  ||  cbProcName == 0) {
    szProcName = percent;
    cbProcName = charSize;
  }

  position = sp77sprintfUnicode (encodingType,
                                 buffer, bufferSize, 
                                 SQLCommand_ProceduresTemplate,
                                 column1, column2,
                                 getTablePrefix (kernelVersion),
                                 encodingType,
                                 cbSchemaName * charSize,
                                 szSchemaName,
                                 ESCAPE_CHAR,
                                 encodingType,
                                 cbProcName * charSize,
                                 szProcName,
                                 encodingType,
                                 ESCAPE_CHAR);

  return stmt->execute (buffer, position, encoding);
}


SQLDBC_Retcode SQLDBC_Catalog::SQLPrimaryKeys (SQLDBC_Statement      *stmt,
                                               void                  *szCatalogName,
                                               SQLDBC_Int2            cbCatalogName,
                                               void                  *szSchemaName,
                                               SQLDBC_Int2            cbSchemaName,
                                               void                  *szTableName,
                                               SQLDBC_Int2            cbTableName,
                                               SQLDBC_Version         odbcVersion,
                                               SQLDBC_Bool            pattern,
                                               SQLDBC_StringEncoding  encoding)
{
  const char colname1_v2[]  = "TABLE_QUALIFIER";
  const char colname2_v2[]  = "TABLE_OWNER";

  const char colname1_v3[]  = "TABLE_CAT";
  const char colname2_v3[]  = "TABLE_SCHEM";

  const char SQLCommand_PrimaryKeysTemplate[] =
"SELECT TABLE_QUALIFIER %s, TABLE_OWNER %s, TABLE_NAME, COLUMN_NAME, KEY_SEQ, \
NULL PK_NAME FROM %sCOLUMNS \
WHERE KEY_SEQ IS NOT NULL AND TABLE_OWNER = '%'=.*S' AND TABLE_NAME = '%'=.*S' \
ORDER BY 2, 3, 5";

  ///////////////////////////////////////////////////////////////////////

  SQLDBC_Retcode retcode = SQLDBC_OK;
  const tsp77encoding   *encodingType;
  unsigned int position = 0;
  unsigned int bufferSize = 0;
  SQLDBC_Int4  kernelVersion;

  char *buffer;
  char percent[2];
  const char *column1;      // column names due to differences between version 2 and 3
  const char *column2;

  if (stmt == NULL)
    return SQLDBC_NOT_OK;

  kernelVersion = stmt->getKernelVersion();

  if (encoding == SQLDBC_StringEncodingAscii)
    encodingType = sp77encodingAscii;
  else
    encodingType = sp77nativeUnicodeEncoding ();

  const int charSize = encodingType->fixedCharacterSize;

  // allocate maximal length of SQL statement
  bufferSize = charSize * (sizeof (SQLCommand_PrimaryKeysTemplate) + 200 +
                           2*sizeof(colname1_v2));
  buffer = getBuffer (bufferSize);

  // set column names for first and second column regarding ODBC version
  if (odbcVersion == ODBC3)  {
    column1  = colname1_v3;
    column2  = colname2_v3;
  } else {
    column1  = colname1_v2;
    column2  = colname2_v2;
  }

  // percent in the right encoding
  sp77sprintfUnicode (encodingType,
                      percent, sizeof(percent), 
                      "%%");

  if (szSchemaName == NULL  ||  cbSchemaName == 0) {
    szSchemaName = percent;
    cbSchemaName = charSize;
  }

  if (szTableName == NULL  ||  cbTableName == 0) {
    szTableName = percent;
    cbTableName = charSize;
  }

  position = sp77sprintfUnicode (encodingType,
                                 buffer, bufferSize, 
                                 SQLCommand_PrimaryKeysTemplate,
                                 column1, column2,
                                 getTablePrefix (kernelVersion),
                                 encodingType,
                                 cbSchemaName * charSize,
                                 szSchemaName,
                                 encodingType,
                                 cbTableName * charSize,
                                 szTableName,
                                 encodingType);

  return stmt->execute (buffer, position, encoding);
}

SQLDBC_Retcode SQLDBC_Catalog::SQLSpecialColumns (SQLDBC_Statement *stmt,
                                                  SQLDBC_UInt2      fColType,
                                                  void             *szCatalogName,
                                                  SQLDBC_Int2       cbCatalogName,
                                                  void             *szSchemaName,
                                                  SQLDBC_Int2       cbSchemaName,
                                                  void             *szTableName,
                                                  SQLDBC_Int2       cbTableName,
                                                  SQLDBC_UInt2      fScope,
                                                  SQLDBC_UInt2      fNullable,
                                                  SQLDBC_Version         odbcVersion,
                                                  SQLDBC_Bool            pattern,
                                                  SQLDBC_StringEncoding  encoding)
{
  const SQLDBC_UInt2 SQL_BEST_ROWID = 1;
  const SQLDBC_UInt2 SQL_SCOPE_CURROW = 1;
  const SQLDBC_UInt2 SQL_SCOPE_TRANSACTION = 1;

  ///////////////////////////////////////////////////////////////////////
  // SQL statements for SQLSpecialColumns
  ///////////////////////////////////////////////////////////////////////

  const char colname5_v2[]  = "PRECISION";
  const char colname6_v2[]  = "\"LENGTH\"";
  const char colname7_v2[]  = "SCALE";

  const char colname5_v3[]  = "COLUMN_SIZE";
  const char colname6_v3[]  = "BUFFER_LENGTH";
  const char colname7_v3[]  = "DECIMAL_DIGITS";

  const char SQLCommand_SpecColEmptyTemplate[] =
"SELECT NULL SCOPE, NULL COLUMN_NAME, NULL DATA_TYPE, NULL TYPE_NAME, \
NULL %s, NULL %s, NULL %s, NULL PSEUDO_COLUMN \
FROM SYSDBA.DUAL WHERE NULL <> NULL";

  const char SQLCommand_SpecColSyskeyTemplate[] =
"SELECT NUM(1) SCOPE, 'SYSKEY' COLUMN_NAME, NUM(-2) DATA_TYPE, \
'CHAR() BYTE' TYPE_NAME, NUM(8) %s, NUM(8) %s, \
NULL %s, NUM(2) PSEUDO_COLUMN, 'BYTE' CODETYPE FROM SYSDBA.DUAL %s";

  const char SQLCommand_SpecColPrimarykeyTemplate[] =
"SELECT NUM(1) SCOPE, COLUMN_NAME, DATA_TYPE, \
TYPE_NAME, COLUMN_SIZE %s, \ 
BUFFER_LENGTH %s, DECIMAL_DIGITS %s, NUM(2) PSEUDO_COLUMN FROM %sCOLUMNS \
WHERE KEYCOLUMNNO IS NOT NULL AND TABLE_SCHEM = '%'=.*S' AND TABLE_NAME = '%'=.*S' ";

  const char SQLCommand_ExistsTableTemplate[] = "EXISTS TABLE \"%'=.*S\".\"%'=.*S\" ";

  char SQLCommand_SQLStatistics[sizeof(SQLCommand_SpecColPrimarykeyTemplate) + 3*sizeof(colname7_v3) + 1000];

  ///////////////////////////////////////////////////////////////////////

  SQLDBC_Retcode retcode = SQLDBC_OK;
  const tsp77encoding   *encodingType;
  unsigned int position = 0;
  unsigned int bufferSize = 0;
  SQLDBC_Int4  kernelVersion;

  char *buffer;
  const char *column5;      // column names due to differences between version 2 and 3
  const char *column6, *column7;

  if (stmt == NULL)
    return SQLDBC_NOT_OK;

  kernelVersion = stmt->getKernelVersion();

  if (encoding == SQLDBC_StringEncodingAscii)
    encodingType = sp77encodingAscii;
  else
    encodingType = sp77nativeUnicodeEncoding ();

  const int charSize = encodingType->fixedCharacterSize;

  // allocate maximal length of SQL statement
  bufferSize = charSize * (sizeof(SQLCommand_SQLStatistics));
  buffer = getBuffer (bufferSize);

  // set column names for first and second column regarding ODBC version
  if (odbcVersion == ODBC3)  {
    column5  = colname5_v3;
    column6  = colname6_v3;
    column7  = colname7_v3;
  } else {
    column5  = colname5_v2;
    column6  = colname6_v2;
    column7  = colname7_v2;
  }

  // check whether table exists
  position = sp77sprintfUnicode (encodingType,
                                 buffer, bufferSize, 
                                 SQLCommand_ExistsTableTemplate,
                                 encodingType,
                                 cbSchemaName * charSize,
                                 szSchemaName,
                                 encodingType,
                                 cbTableName * charSize,
                                 szTableName);

  if (stmt->execute (buffer, position, encoding) != SQLDBC_OK) {
    // empty result set
    position = sp77sprintfUnicode (encodingType,
                                   buffer, bufferSize, 
                                   SQLCommand_SpecColEmptyTemplate,
                                   column5, column6, column7);

    retcode = stmt->execute (buffer, position, encoding);
    return retcode;
  }

  if (fColType == SQL_BEST_ROWID  &&  (fScope == SQL_SCOPE_CURROW  ||  fScope == SQL_SCOPE_TRANSACTION)) {
    // primary
    position = sp77sprintfUnicode (encodingType,
                                   buffer, bufferSize,
                                   SQLCommand_SpecColPrimarykeyTemplate,
                                   column5, column6, column7,
                                   getTablePrefix (kernelVersion),
                                   encodingType,
                                   cbSchemaName * charSize,
                                   szSchemaName,
                                   encodingType,
                                   cbTableName * charSize,
                                   szTableName);
    retcode = stmt->execute (buffer, position, encoding);
  } else {
    retcode = SQLDBC_NOT_OK;
  }

  if (retcode != SQLDBC_OK) {
    // syskey
    position += sp77sprintfUnicode (encodingType,
                                    buffer, bufferSize, 
                                    SQLCommand_SpecColSyskeyTemplate,
                                    column5, column6, column7,
                                    "ORDER BY 1 ");
    retcode = stmt->execute (buffer, position, encoding);
  }

  return retcode;
}

SQLDBC_Retcode SQLDBC_Catalog::SQLStatistics (SQLDBC_Statement      *stmt,
                                              void                  *szCatalogName,
                                              SQLDBC_Int2            cbCatalogName,
                                              void                  *szSchemaName,
                                              SQLDBC_Int2            cbSchemaName,
                                              void                  *szTableName,
                                              SQLDBC_Int2            cbTableName,
                                              SQLDBC_UInt2           fUnique,
                                              SQLDBC_UInt2           fAccuracy,
                                              SQLDBC_Version         odbcVersion,
                                              SQLDBC_Bool            pattern,
                                              SQLDBC_StringEncoding  encoding)
{
  // from sqldbc_sqlext.h
  const SQLDBC_UInt2 SQL_ENSURE       = 1;
  const SQLDBC_UInt2 SQL_INDEX_UNIQUE = 0;

  ///////////////////////////////////////////////////////////////////////
  // SQL statements for SQLStatistics
  ///////////////////////////////////////////////////////////////////////

  const char colname1_v2[]  = "TABLE_QUALIFIER";
  const char colname2_v2[]  = "TABLE_OWNER";
  const char colname8_v2[]  = "SEQ_IN_INDEX";
  const char colname10_v2[] = "COLLATION";

  const char colname1_v3[]  = "TABLE_CAT";
  const char colname2_v3[]  = "TABLE_SCHEM";
  const char colname8_v3[]  = "ORDINAL_POSITION";
  const char colname10_v3[] = "ASC_OR_DESC";

  const char SQLCommand_OrderBy[] = " ORDER BY  4,7,5,6,8";

  const char SQLCommand_TableStatEnsureTemplate[] = 
"SELECT NULL %s, TABLE_SCHEM %s, TABLENAME TABLE_NAME, \
NUM(NULL) NON_UNIQUE, NULL INDEX_QUALIFIER, NULL INDEX_NAME, 0 TYPE, \
NULL %s, NULL COLUMN_NAME, NULL %s, CARDINALITY, \
PAGES, NULL FILTER_CONDITION FROM %sTABLESTAT \
WHERE OWNER = '%'=.*S' AND TABLENAME = '%'=.*S' ";

  const char SQLCommand_IndexEnsureTemplate[] = 
"UNION SELECT TABLE_CAT %s, TABLE_SCHEM %s, TABLE_NAME, NON_UNIQUE, \
INDEX_QUALIFIER, INDEX_NAME, TYPE, ORDINAL_POSITION %s, COLUMN_NAME, ASC_OR_DESC %s, \
%sINDEXSTAT.CARDINALITY, %sINDEXSTAT.PAGES, NULL FROM %sINDEXES, %sINDEXSTAT \
WHERE TABLE_SCHEM = '%'=.*S' AND TABLESCHEM = TABLE_SCHEM \
AND TABLE_NAME = '%'=.*S' AND TABLENAME = TABLE_NAME \
AND (INDEXNAME = INDEX_NAME OR COLUMNNAME = INDEX_NAME) ";

  const char SQLCommand_PKEnsureTemplate[] = 
"UNION SELECT TABLE_CAT %s, TABLE_SCHEM %s, TABLE_NAME, 0, NULL, INDEX_NAME, \
TYPE, ORDINAL_POSITION %s, COLUMN_NAME, ASC_OR_DESC %s, %sTABLESTAT.CARDINALITY, %sTABLESTAT.PAGES, NULL \
FROM %sINDEXES, %sTABLESTAT \
WHERE TABLE_OWNER = '%'=.*S' AND TABLESCHEM = TABLE_OWNER \
AND TABLE_NAME = '%'=.*S' AND TABLENAME = TABLE_NAME \
AND INDEX_NAME = 'SYSPRIMARYKEYINDEX' AND COLUMN_NAME <> 'SYSKEY' ";

  const char SQLCommand_TableStatQuickTemplate[] = 
"SELECT TABLE_CAT %s, TABLE_SCHEM %s, TABLE_NAME, NUM(NULL) NON_UNIQUE, \
NULL INDEX_QUALIFIER, NULL INDEX_NAME, 0 TYPE, NULL %s, \
NULL COLUMN_NAME, NULL %s, CARDINALITY, PAGES, FILTER_CONDITION \ 
FROM %sINDEXES WHERE TABLE_SCHEM='%'=.*S' AND TABLE_NAME='%'=.*S' \
AND INDEX_NAME = 'SYSPRIMARYKEYINDEX' AND ORDINAL_POSITION=1 ";

  const char SQLCommand_IndexQuickTemplate[] = 
"UNION ALL SELECT TABLE_CAT %s, TABLE_SCHEM %s, TABLE_NAME, NON_UNIQUE, \
INDEX_QUALIFIER, INDEX_NAME, TYPE, ORDINAL_POSITION %s, COLUMN_NAME, ASC_OR_DESC %s, \
CARDINALITY, PAGES, FILTER_CONDITION FROM %sINDEXES \ 
WHERE TABLE_SCHEM='%'=.*S' AND TABLE_NAME='%'=.*S' AND COLUMN_NAME <> 'SYSKEY' ";


  char SQLCommand_SQLStatistics[sizeof(SQLCommand_TableStatEnsureTemplate) +
                                sizeof(SQLCommand_IndexEnsureTemplate) +
                                sizeof(SQLCommand_PKEnsureTemplate) + 12*sizeof(colname8_v3) + 100]; // be generous

  ///////////////////////////////////////////////////////////////////////

  const tsp77encoding   *encodingType;
  unsigned int position = 0;
  unsigned int bufferSize = 0;
  SQLDBC_Int4  kernelVersion;

  char *buffer;
  const char *column1;      // column names due to differences between version 2 and 3
  const char *column2, *column8, *column10;

  if (stmt == NULL)
    return SQLDBC_NOT_OK;

  kernelVersion = stmt->getKernelVersion();

  if (encoding == SQLDBC_StringEncodingAscii)
    encodingType = sp77encodingAscii;
  else
    encodingType = sp77nativeUnicodeEncoding ();

  const int charSize = encodingType->fixedCharacterSize;

  // allocate maximal length of SQL statement
  bufferSize = charSize * (sizeof(SQLCommand_SQLStatistics));
  buffer = getBuffer (bufferSize);

  // set column names for first and second column regarding ODBC version
  if (odbcVersion == ODBC3)  {
    column1  = colname1_v3;
    column2  = colname2_v3;
    column8  = colname8_v3;
    column10 = colname10_v3;    
  } else {
    column1  = colname1_v2;
    column2  = colname2_v2;
    column8  = colname8_v2;
    column10 = colname10_v2;    
  }

  if (fAccuracy == SQL_ENSURE) {
    position += sp77sprintfUnicode (encodingType,
                                    buffer, bufferSize - position, 
                                    SQLCommand_TableStatEnsureTemplate,
                                    column1, column2,
                                    column8, column10,
                                    getTablePrefix (kernelVersion),
                                    encodingType,
                                    cbSchemaName * charSize,
                                    szSchemaName,
                                    encodingType,
                                    cbTableName * charSize,
                                    szTableName);
    position += sp77sprintfUnicode (encodingType,
                                    buffer+position, bufferSize - position,
                                    SQLCommand_IndexEnsureTemplate,
                                    column1, column2,
                                    column8, column10,
                                    getTablePrefix (kernelVersion),
                                    getTablePrefix (kernelVersion),
                                    getTablePrefix (kernelVersion),
                                    getTablePrefix (kernelVersion),
                                    encodingType,
                                    cbSchemaName * charSize,
                                    szSchemaName,
                                    encodingType,
                                    cbTableName * charSize,
                                    szTableName);
    if (fUnique == SQL_INDEX_UNIQUE) {
      position += sp77sprintfUnicode (encodingType,
                                      buffer+position, bufferSize - position,
                                      "AND INDEX_TYPE = 'UNIQUE'");
    }
    position += sp77sprintfUnicode (encodingType,
                                    buffer+position, bufferSize - position,
                                    SQLCommand_PKEnsureTemplate,
                                    column1, column2,
                                    column8, column10,
                                    getTablePrefix (kernelVersion),
                                    getTablePrefix (kernelVersion),
                                    getTablePrefix (kernelVersion),
                                    getTablePrefix (kernelVersion),
                                    encodingType,
                                    cbSchemaName * charSize,
                                    szSchemaName,
                                    encodingType,
                                    cbTableName * charSize,
                                    szTableName);
  } else {
    position += sp77sprintfUnicode (encodingType,
                                    buffer, bufferSize - position, 
                                    SQLCommand_TableStatQuickTemplate,
                                    column1, column2,
                                    column8, column10,
                                    getTablePrefix (kernelVersion),
                                    encodingType,
                                    cbSchemaName * charSize,
                                    szSchemaName,
                                    encodingType,
                                    cbTableName * charSize,
                                    szTableName);
    position += sp77sprintfUnicode (encodingType,
                                    buffer+position, bufferSize - position,
                                    SQLCommand_IndexQuickTemplate,
                                    column1, column2,
                                    column8, column10,
                                    getTablePrefix (kernelVersion),
                                    encodingType,
                                    cbSchemaName * charSize,
                                    szSchemaName,
                                    encodingType,
                                    cbTableName * charSize,
                                    szTableName);
    if (fUnique == SQL_INDEX_UNIQUE) {
      position += sp77sprintfUnicode (encodingType,
                                      buffer+position, bufferSize - position,
                                      "AND INDEX_TYPE = 'UNIQUE'");
    }
  }

  position += sp77sprintfUnicode (encodingType,
                                  buffer+position, bufferSize - position,
                                  SQLCommand_OrderBy);

  return stmt->execute (buffer, position, encoding);
}

SQLDBC_Retcode SQLDBC_Catalog::SQLTablePrivileges (SQLDBC_Statement      *stmt,
                                                   void                  *szCatalogName,
                                                   SQLDBC_Int2            cbCatalogName,
                                                   void                  *szSchemaName,
                                                   SQLDBC_Int2            cbSchemaName,
                                                   void                  *szTableName,
                                                   SQLDBC_Int2            cbTableName,
                                                   SQLDBC_Version         odbcVersion,
                                                   SQLDBC_Bool            pattern,
                                                   SQLDBC_StringEncoding  encoding)
{
  const char colname1_v2[]  = "TABLE_QUALIFIER";
  const char colname2_v2[]  = "TABLE_OWNER";

  const char colname1_v3[]  = "TABLE_CAT";
  const char colname2_v3[]  = "TABLE_SCHEM";

  const char SQLCommand_TablePrivilegesTemplate[] =
"SELECT TABLE_QUALIFIER %s, TABLE_OWNER %s, TABLE_NAME, GRANTOR, GRANTEE, \
PRIVILEGE, IS_GRANTABLE FROM %sTABLEPRIVILEGES WHERE \
TABLE_OWNER LIKE '%'=.*S'  ESCAPE '%c' AND TABLE_NAME LIKE '%'=.*S' ESCAPE '%c' \
ORDER BY 2, 3, 6 ";

  ///////////////////////////////////////////////////////////////////////

  SQLDBC_Retcode retcode = SQLDBC_OK;
  const tsp77encoding   *encodingType;
  unsigned int position = 0;
  unsigned int bufferSize = 0;
  SQLDBC_Int4  kernelVersion;

  char *buffer;
  char percent[2];
  const char *column1;      // column names due to differences between version 2 and 3
  const char *column2;

  if (stmt == NULL)
    return SQLDBC_NOT_OK;

  kernelVersion = stmt->getKernelVersion();

  if (encoding == SQLDBC_StringEncodingAscii)
    encodingType = sp77encodingAscii;
  else
    encodingType = sp77nativeUnicodeEncoding ();

  const int charSize = encodingType->fixedCharacterSize;

  // allocate maximal length of SQL statement
  bufferSize = charSize * (sizeof (SQLCommand_TablePrivilegesTemplate) + 200 +
                           6*sizeof(colname1_v2));
  buffer = getBuffer (bufferSize);

  // set column names for first and second column regarding ODBC version
  if (odbcVersion == ODBC3)  {
    column1  = colname1_v3;
    column2  = colname2_v3;
  } else {
    column1  = colname1_v2;
    column2  = colname2_v2;
  }

  // percent in the right encoding
  sp77sprintfUnicode (encodingType,
                      percent, sizeof(percent), 
                      "%%");

  if (szSchemaName == NULL  ||  cbSchemaName == 0) {
    szSchemaName = percent;
    cbSchemaName = charSize;
  }

  if (szTableName == NULL  ||  cbTableName == 0) {
    szTableName = percent;
    cbTableName = charSize;
  }

  position = sp77sprintfUnicode (encodingType,
                                 buffer, bufferSize, 
                                 SQLCommand_TablePrivilegesTemplate,
                                 column1, column2,
                                 getTablePrefix (kernelVersion),
                                 encodingType,
                                 cbSchemaName * charSize,
                                 szSchemaName,
                                 ESCAPE_CHAR,
                                 encodingType,
                                 cbTableName * charSize,
                                 szTableName,
                                 ESCAPE_CHAR);

  return stmt->execute (buffer, position, encoding);
}

SQLDBC_Retcode SQLDBC_Catalog::SQLTables (SQLDBC_Statement *stmt,
                                          void        *szCatalogName,
                                          SQLDBC_Int2  cbCatalogName,
                                          void        *szSchemaName,
                                          SQLDBC_Int2  cbSchemaName,
                                          void        *szTableName,
                                          SQLDBC_Int2  cbTableName,
                                          void        *szTableType,
                                          SQLDBC_Int2  cbTableType,
                                          SQLDBC_Version odbcVersion,
                                          SQLDBC_Bool  pattern,
                                          SQLDBC_StringEncoding encoding)
{
  ///////////////////////////////////////////////////////////////////////
  // SQL statements for SQLTables
  ///////////////////////////////////////////////////////////////////////

  // in ODBC version 3.0 the fields TABLE_QUALIFIER and TABLE_OWNER are renamed
  const char SQLCommand_SchemaWhere[] = "TABLE_SCHEM LIKE '%'=.*S' ESCAPE '\\'";
  const char SQLCommand_TableWhere[]  = "TABLE_NAME LIKE '%'=.*S' ESCAPE '\\'";
  const char SQLCommand_TypeWhere[]   = "TABLE_TYPE IN (%'=.*S)";

  const char colname1_v2[] = "TABLE_QUALIFIER";
  const char colname2_v2[] = "TABLE_OWNER";
  const char colname1_v3[] = "TABLE_CAT";
  const char colname2_v3[] = "TABLE_SCHEM";

  const char SQLCommand_SQLTables_Template[] =
"SELECT TABLE_CAT %s, TABLE_SCHEM %s, TABLE_NAME, TABLE_TYPE, REMARKS FROM %sTABLES ";

  char SQLCommand_SQLTables[sizeof(SQLCommand_SQLTables_Template) + 2*sizeof(colname1_v2) + 100];  // be generous

  const char SQLCommand_CatalogScan[] =
"SELECT NULL %s, NULL %s, NULL TABLE_NAME, NULL TABLE_TYPE, NULL REMARKS FROM DUAL WHERE FALSE";

  const char SQLCommand_SchemaScan[] = 
"SELECT NULL %s, TABLE_SCHEM %s, NULL TABLE_NAME, NULL TABLE_TYPE, NULL REMARKS FROM %sSCHEMAS ORDER BY 2";

  const char SQLCommand_TypeScan[] =
"SELECT NULL %s, NULL %s, NULL TABLE_NAME, TABLE_TYPE, NULL REMARKS FROM %sTABLETYPES ORDER BY 4";

  ///////////////////////////////////////////////////////////////////////

  const tsp77encoding   *encodingType;
  unsigned int position = 0;
  unsigned int bufferSize = 0;
  SQLDBC_Int4  kernelVersion;

  char *buffer;
  char percent[2];
  const char *column1;      // column names due to differences between version 2 and 3
  const char *column2;

  if (stmt == NULL)
    return SQLDBC_NOT_OK;

  kernelVersion = stmt->getKernelVersion();

  if (encoding == SQLDBC_StringEncodingAscii)
    encodingType = sp77encodingAscii;
  else
    encodingType = sp77nativeUnicodeEncoding ();

  const int charSize = encodingType->fixedCharacterSize;

  // allocate maximal length of SQL statement
  bufferSize = charSize * (sizeof(SQLCommand_SchemaScan) + 3*sizeof(SQLCommand_SchemaWhere) + 2*sizeof(colname1_v2) +
                           3*sizeof(SQLCommand_SchemaWhere) + 20);
  buffer = getBuffer (bufferSize);

  // set column names for first and second column regarding ODBC version
  if (odbcVersion == ODBC3)  {
    column1 = colname1_v3;
    column2 = colname2_v3;
  } else {
    column1 = colname1_v2;
    column2 = colname2_v2;
  }

  sprintf (SQLCommand_SQLTables, SQLCommand_SQLTables_Template, column1, column2, getTablePrefix(kernelVersion));

  // percent in the right encoding
  sp77sprintfUnicode (encodingType,
                      percent, sizeof(percent), 
                      "%%");

  // determine special behaviour of SQLTables regarding SQL_ALL_[CATALOG|SCHEMAS|TABLE_TYPES]
  if (szCatalogName != NULL  &&  memcmp (percent, szCatalogName, charSize) == 0  &&
      cbSchemaName  == 0   && cbTableName == 0) {
    position += sp77sprintfUnicode (encodingType,
                                    buffer, bufferSize - position, 
                                    SQLCommand_CatalogScan,
                                    column1,
                                    column2,
                                    getTablePrefix (kernelVersion));
  }

  if (szSchemaName != NULL  &&  memcmp (percent, szSchemaName, charSize) == 0  &&
      cbCatalogName  == 0   && cbTableName == 0) {
    position += sp77sprintfUnicode (encodingType,
                                    buffer, bufferSize - position, 
                                    SQLCommand_SchemaScan,
                                    column1,
                                    column2,
                                    getTablePrefix (kernelVersion));
  }

  if (szTableType != NULL  &&  memcmp (percent, szTableType, charSize) == 0  &&
      cbCatalogName  == 0  &&  cbSchemaName == 0  &&  cbTableName == 0  ) {
    position += sp77sprintfUnicode (encodingType,
                                    buffer, bufferSize - position, 
                                    SQLCommand_TypeScan,
                                    column1,
                                    column2,
                                    getTablePrefix (kernelVersion));
  }


  // build select command for regular SQLTables call, if buffer is not filled by a scan statement
  if (position == 0) {
    position += sp77sprintfUnicode (encodingType,
                                    buffer, bufferSize - position, 
                                    SQLCommand_SQLTables,
                                    column1,
                                    column2);

    if (cbSchemaName > 0  ||  cbTableName > 0  ||  cbTableType > 0) {
      position += sp77sprintfUnicode (encodingType,
                                      buffer+position, bufferSize - position, 
                                      " WHERE ");
    }

    if (cbSchemaName > 0) {
      position += sp77sprintfUnicode (encodingType,
                                      buffer+position, bufferSize - position, 
                                      SQLCommand_SchemaWhere,
                                      encodingType,
                                      cbSchemaName * charSize,
                                      szSchemaName);

      if (cbTableName > 0  ||  cbTableType > 0) {
        position += sp77sprintfUnicode (encodingType,
                                        buffer+position, bufferSize - position, 
                                        " AND ");
      }
    }

    if (cbTableName > 0) {
      position += sp77sprintfUnicode (encodingType,
                                      buffer+position, bufferSize - position, 
                                      SQLCommand_TableWhere,
                                      encodingType,
                                      cbTableName * charSize,
                                      szTableName);

      if (cbTableType > 0) {
        position += sp77sprintfUnicode (encodingType,
                                        buffer+position, bufferSize - position, 
                                        " AND ");
      }
    }

    if (cbTableType > 0) {
      position += sp77sprintfUnicode (encodingType,
                                      buffer+position, bufferSize - position, 
                                      SQLCommand_TypeWhere,
                                      encodingType,
                                      cbTableType * charSize,
                                      szTableType);
  }

    position += sp77sprintfUnicode (encodingType,
                                    buffer+position, bufferSize - position, 
                                    " ORDER BY 4, 2, 3");
  }

  return stmt->execute (buffer, position, encoding);

}


} // namespace SQLDBC
