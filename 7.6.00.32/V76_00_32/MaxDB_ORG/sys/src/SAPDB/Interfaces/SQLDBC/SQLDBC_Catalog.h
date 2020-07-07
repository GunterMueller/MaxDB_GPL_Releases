/*!
  @file           SQLDBC_Catalog.h
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
#ifndef SQLDBC_CATALOG_H
#define SQLDBC_CATALOG_H

#include "SAPDB/Interfaces/SQLDBC/SQLDBC.h"


namespace SQLDBC {

/**
 * @internal 
 * @brief class containing static functions for building SQL statements
 * that are designed for retrieving catalog informations from the kernel.
 */ 
class SQLDBC_Catalog
{
public:
// cmp sqldbc_sqlext.h
  enum SQLDBC_Version {
    ODBC2 = 2,
    ODBC3 = 3
  };

    /**
     * Returns information about data types supported by the data source.
     * @param dataType SQL data type
     */
  static
    SQLDBC_Retcode SQLGetTypeInfo (SQLDBC_Statement *stmt,
                                   SQLDBC_Int2       dataType,
                                   SQLDBC_Version odbcVersion,
                                   SQLDBC_StringEncoding  encoding);


    /**
     * Executes a query in order to get a result set as demanded for the ODBC API call for SQLSpecialColumns.
     * @param szCatalogName  Name of catalog
     * @param cbCatalogName  Length of catalog name
     * @param szSchemaName   Name of schema
     * @param cbSchemaName   Length of schema name
     * @param szTableName    Name of table
     * @param cbTableName    Length of table name
     * @param szColumnName   Name of column
     * @param cbColumnName   Length of column name
     * @param odbcVersion    Version of ODBC (result set differ between version 2 and 3)
     * @param pattern        Flag, wether names are treated as identifiers or patterns.
     * @param encoding       Encoding of names.
     */
  static
    SQLDBC_Retcode SQLColumns (SQLDBC_Statement *stmt,
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
                               SQLDBC_StringEncoding  encoding);

    /**
     * Returns a list of columns and associated privileges for the specified table.
     * @param szCatalogName  Name of catalog
     * @param cbCatalogName  Length of catalog name
     * @param szSchemaName   Name of schema
     * @param cbSchemaName   Length of schema name
     * @param szTableName    Name of table
     * @param cbTableName    Length of table name
     * @param szColumnName   Name of column
     * @param cbColumnName   Length of column name
     * @param odbcVersion    Version of ODBC (result set differ between version 2 and 3)
     * @param pattern        Flag, wether names are treated as identifiers or patterns.
     * @param encoding       Encoding of names.
     */
  static
  SQLDBC_Retcode SQLColumnPrivileges (SQLDBC_Statement      *stmt,
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
                                      SQLDBC_StringEncoding  encoding);


    /**
     * Returns either 
     * - a list of foreign keys in the specified table (columns in the specified table that refer to primary keys in other tables), or
     * - a list of foreign keys in other tables that refer to the primary key in the specified table.
     * @param szPkCatalogName  Name of catalog
     * @param cbPkCatalogName  Length of catalog name (primary key)
     * @param szPkSchemaName   Name of schema
     * @param cbPkSchemaName   Length of schema name (primary key)
     * @param szPkTableName    Name of table
     * @param cbPkTableName    Length of table name (primary key)
     * @param szFkCatalogName  Name of catalog
     * @param cbFkCatalogName  Length of catalog name (foreign key)
     * @param szFkSchemaName   Name of schema
     * @param cbFkSchemaName   Length of schema name (foreign key)
     * @param szFkTableName    Name of table
     * @param cbFkTableName    Length of table name (foreign key)
     * @param odbcVersion      Version of ODBC (result set differ between version 2 and 3)
     * @param pattern          Flag, wether names are treated as identifiers or patterns.
     * @param encoding         Encoding of names.
     */

  static
  SQLDBC_Retcode SQLForeignKeys (SQLDBC_Statement      *stmt,
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
                                 SQLDBC_StringEncoding  encoding);

    /**
     * Returns the list of input and output parameters, as well as the columns that make up the result set for the specified procedures.
     * @param szCatalogName  Name of catalog
     * @param cbCatalogName  Length of catalog name
     * @param szSchemaName   Name of schema
     * @param cbSchemaName   Length of schema name
     * @param szProcName     Name of procedure
     * @param cbProcName     Length of procedure name
     * @param szColumnName   Name of column
     * @param cbColumnName   Length of column name
     * @param odbcVersion    Version of ODBC (result set differ between version 2 and 3)
     * @param pattern        Flag, wether names are treated as identifiers or patterns.
     * @param encoding       Encoding of names.
     */
  static
  SQLDBC_Retcode SQLProcedureColumns (SQLDBC_Statement      *stmt,
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
                                      SQLDBC_StringEncoding  encoding);


    /**
     * Returns the list of procedure names stored in a specific data source.
     * @param szCatalogName  Name of catalog
     * @param cbCatalogName  Length of catalog name
     * @param szSchemaName   Name of schema
     * @param cbSchemaName   Length of schema name
     * @param szProcName     Name of procedure
     * @param cbProcName     Length of procedure name
     * @param odbcVersion    Version of ODBC (result set differ between version 2 and 3)
     * @param pattern        Flag, wether names are treated as identifiers or patterns.
     * @param encoding       Encoding of names.
     */
  static
  SQLDBC_Retcode SQLProcedures (SQLDBC_Statement      *stmt,
                                void                  *szCatalogName,
                                SQLDBC_Int2            cbCatalogName,
                                void                  *szSchemaName,
                                SQLDBC_Int2            cbSchemaName,
                                void                  *szProcName,
                                SQLDBC_Int2            cbProcName,
                                SQLDBC_Version         odbcVersion,
                                SQLDBC_Bool            pattern,
                                SQLDBC_StringEncoding  encoding);

    /**
     * Returns the column names that make up the primary key for a table.
     * @param szCatalogName  Name of catalog
     * @param cbCatalogName  Length of catalog name
     * @param szSchemaName   Name of schema
     * @param cbSchemaName   Length of schema name
     * @param szTableName    Name of table
     * @param cbTableName    Length of table name
     * @param odbcVersion    Version of ODBC (result set differ between version 2 and 3)
     * @param pattern        Flag, wether names are treated as identifiers or patterns.
     * @param encoding       Encoding of names.
     */
  static
  SQLDBC_Retcode SQLPrimaryKeys (SQLDBC_Statement      *stmt,
                                 void                  *szCatalogName,
                                 SQLDBC_Int2            cbCatalogName,
                                 void                  *szSchemaName,
                                 SQLDBC_Int2            cbSchemaName,
                                 void                  *szTableName,
                                 SQLDBC_Int2            cbTableName,
                                 SQLDBC_Version         odbcVersion,
                                 SQLDBC_Bool            pattern,
                                 SQLDBC_StringEncoding  encoding);

    /**
     * Executes a query in order to get a result set as demanded for the ODBC API call for SQLSpecialColumns.
     * @param fColType       Type of column to return: SQL_BEST_ROWID or SQL_ROWVER
     * @param szCatalogName  Name of catalog
     * @param cbCatalogName  Length of catalog name
     * @param szSchemaName   Name of schema
     * @param cbSchemaName   Length of schema name
     * @param szTableName    Name of table
     * @param cbTableName    Length of table name
     * @param fScope         Minimum required scope: SQL_SCOPE_CURROW, SQL_SCOPE_TRANSACTION or SQL_SCOPE_SESSION
     * @param fNullable      Flag for nullable columns: SQL_NO_NULLS or SQL_NULLABLE
     * @param odbcVersion    Version of ODBC (result set differ between version 2 and 3)
     * @param pattern        Flag, wether names are treated as identifiers or patterns.
     * @param encoding       Encoding of names.
     */
  static
    SQLDBC_Retcode SQLSpecialColumns (SQLDBC_Statement *stmt,
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
                                      SQLDBC_StringEncoding  encoding);

    /**
     * Executes a query in order to get a result set as demanded for the ODBC API call for SQLStatistics.
     * @param szCatalogName  Name of catalog
     * @param cbCatalogName  Length of catalog name
     * @param szSchemaName   Name of schema
     * @param cbSchemaName   Length of schema name
     * @param szTableName    Name of table
     * @param cbTableName    Length of table name
     * @param fUnique        Type of index: SQL_INDEX_UNIQUE or SQL_INDEX_ALL
     * @param fAccuracy      Type of accuracy: SQL_QUICK or SQL_ENSURE
     * @param odbcVersion    Version of ODBC (result set differ between version 2 and 3)
     * @param pattern        Flag, wether names are treated as identifiers or patterns.
     * @param encoding       Encoding of names.
     */
  static
    SQLDBC_Retcode SQLStatistics (SQLDBC_Statement      *stmt,
                                  void                  *szCatalogName,
                                  SQLDBC_Int2            cbCatalogName,
                                  void                  *szSchemaName,
                                  SQLDBC_Int2            cbSchemaName,
                                  void                  *szTableName,
                                  SQLDBC_Int2            cbTableName,
                                  SQLDBC_UInt2           fUnique,
                                  SQLDBC_UInt2           fAccuracy,
                                  SQLDBC_Version         odbcVersion = ODBC3,
                                  SQLDBC_Bool            pattern     = SQLDBC_FALSE,
                                  SQLDBC_StringEncoding  encoding    = SQLDBC_StringEncodingAscii);

    /**
     * Returns a list of tables and the privileges associated with each table.
     * @param szCatalogName  Name of catalog
     * @param cbCatalogName  Length of catalog name
     * @param szSchemaName   Name of schema
     * @param cbSchemaName   Length of schema name
     * @param szTableName    Name of table
     * @param cbTableName    Length of table name
     * @param odbcVersion    Version of ODBC (result set differ between version 2 and 3)
     * @param pattern        Flag, wether names are treated as identifiers or patterns.
     * @param encoding       Encoding of names.
     */
  static
    SQLDBC_Retcode SQLTablePrivileges (SQLDBC_Statement      *stmt,
                                       void                  *szCatalogName,
                                       SQLDBC_Int2            cbCatalogName,
                                       void                  *szSchemaName,
                                       SQLDBC_Int2            cbSchemaName,
                                       void                  *szTableName,
                                       SQLDBC_Int2            cbTableName,
                                       SQLDBC_Version         odbcVersion = ODBC3,
                                       SQLDBC_Bool            pattern     = SQLDBC_FALSE,
                                       SQLDBC_StringEncoding  encoding    = SQLDBC_StringEncodingAscii);

    /**
     * Executes a query in order to get a result set as demanded for the ODBC API call for SQLTables.
     * @param szCatalogName  Name of catalog
     * @param cbCatalogName  Length of catalog name
     * @param szSchemaName   Name of schema
     * @param cbSchemaName   Length of schema name
     * @param szTableName    Name of table
     * @param cbTableName    Length of table name
     * @param szTableType    List of table types to match
     * @param cbTableType    Length of table types
     * @param odbcVersion    Version of ODBC (result set differ between version 2 and 3)
     * @param pattern        Flag, wether names are treated as identifiers or patterns.
     * @param encoding       Encoding of names.
     */
  static
    SQLDBC_Retcode SQLTables (SQLDBC_Statement      *stmt,
                              void                  *szCatalogName,
                              SQLDBC_Int2            cbCatalogName,
                              void                  *szSchemaName,
                              SQLDBC_Int2            cbSchemaName,
                              void                  *szTableName,
                              SQLDBC_Int2            cbTableName,
                              void                  *szTableType,
                              SQLDBC_Int2            cbTableType,
                              SQLDBC_Version         odbcVersion = ODBC3,
                              SQLDBC_Bool            pattern     = SQLDBC_FALSE,
                              SQLDBC_StringEncoding  encoding    = SQLDBC_StringEncodingAscii);


private:
    /**
     * Allocates a buffer of size length from the heap using the approprate allocator.
     * @param size  Size of the buffer in bytes.
     */
    static char * getBuffer (unsigned long size);

    /** Returns the table prefix needed to distinguish between the old
     * system tables/views (<= 7.5) used by ODBC and the new schema
     * based ones (>= 7.6). The static SQL statements have a %s at the
     * right place, where the table prefix will be complemented.
     * @param size Size of the buffer in bytes.  */
    static const char* getTablePrefix (SQLDBC_Int4 kernelVersion);

  /**
   */
  static const char* getOldPrefix () { return "SYSDBA.SYSODBC"; }
  static const char* getNewPrefix () { return "SYSSQLDBC."; }
  


};

} // namespace SQLDBC

#endif     // SQLDBC_CATALOG_H
