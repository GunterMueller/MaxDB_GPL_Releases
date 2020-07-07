/*!**********************************************************************

  module: hpa90info.h

  -----------------------------------------------------------------------

  responsible:  

  special area: ODBC-Driver

  description:  Convert ODBC types and codes to string (e.g for tracing)

  see also:     http://www.microsoft.com/data/odbc/

  -----------------------------------------------------------------------

  copyright:             Copyright (c) 1999-2005 SAP AG




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




************************************************************************/

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include "hpa90info.h"

#define WHCASESTRING(buffer, x) \
case (x): {\
API_STRCPY(buffer, #x);\
break;\
}

char* pa90DescTypeAsString( SWORD type, char *s )
{
    switch( type )
    {
      case SQL_DESC_ALLOC_TYPE:
        sprintf( s, "SQL_DESC_ALLOC_TYPE" );
        break;

      case SQL_DESC_ARRAY_SIZE:
        sprintf( s, "SQL_DESC_ARRAY_SIZE" );
        break;

      case SQL_DESC_ARRAY_STATUS_PTR:
        sprintf( s, "SQL_DESC_ARRAY_STATUS_PTR" );
        break;

      case SQL_DESC_BIND_OFFSET_PTR:
        sprintf( s, "SQL_DESC_BIND_OFFSET_PTR" );
        break;

      case SQL_DESC_BIND_TYPE:
        sprintf( s, "SQL_DESC_BIND_TYPE" );
        break;

      case SQL_DESC_COUNT:
        sprintf( s, "SQL_DESC_COUNT" );
        break;

      case SQL_DESC_ROWS_PROCESSED_PTR:
        sprintf( s, "SQL_DESC_ROWS_PROCESSED_PTR" );
        break;

      case SQL_DESC_AUTO_UNIQUE_VALUE:
        sprintf( s, "SQL_DESC_AUTO_UNIQUE_VALUE" );
        break;

      case SQL_DESC_BASE_COLUMN_NAME:
        sprintf( s, "SQL_DESC_BASE_COLUMN_NAME" );
        break;

      case SQL_DESC_BASE_TABLE_NAME:
        sprintf( s, "SQL_DESC_BASE_TABLE_NAME" );
        break;

      case SQL_DESC_CASE_SENSITIVE:
        sprintf( s, "SQL_DESC_CASE_SENSITIVE" );
        break;

      case SQL_DESC_CATALOG_NAME:
        sprintf( s, "SQL_DESC_CATALOG_NAME" );
        break;

      case SQL_DESC_CONCISE_TYPE:
        sprintf( s, "SQL_DESC_CONCISE_TYPE" );
        break;

      case SQL_DESC_DATA_PTR:
        sprintf( s, "SQL_DESC_DATA_PTR" );
        break;

      case SQL_DESC_DATETIME_INTERVAL_CODE:
        sprintf( s, "SQL_DESC_DATETIME_INTERVAL_CODE" );
        break;

      case SQL_DESC_DATETIME_INTERVAL_PRECISION:
        sprintf( s, "SQL_DESC_DATETIME_INTERVAL_PRECISION" );
        break;

      case SQL_DESC_DISPLAY_SIZE:
        sprintf( s, "SQL_DESC_DISPLAY_SIZE" );
        break;

      case SQL_DESC_FIXED_PREC_SCALE:
        sprintf( s, "SQL_DESC_FIXED_PREC_SCALE" );
        break;

      case SQL_DESC_INDICATOR_PTR:
        sprintf( s, "SQL_DESC_INDICATOR_PTR" );
        break;

      case SQL_DESC_LABEL:
        sprintf( s, "SQL_DESC_LABEL" );
        break;

      case SQL_DESC_LENGTH:
        sprintf( s, "SQL_DESC_LENGTH" );
        break;

      case SQL_DESC_LITERAL_PREFIX:
        sprintf( s, "SQL_DESC_LITERAL_PREFIX" );
        break;

      case SQL_DESC_LITERAL_SUFFIX:
        sprintf( s, "SQL_DESC_LITERAL_SUFFIX" );
        break;

      case SQL_DESC_LOCAL_TYPE_NAME:
        sprintf( s, "SQL_DESC_LOCAL_TYPE_NAME" );
        break;

      case SQL_DESC_NAME:
        sprintf( s, "SQL_DESC_NAME" );
        break;

      case SQL_DESC_NULLABLE:
        sprintf( s, "SQL_DESC_NULLABLE" );
        break;

      case SQL_DESC_NUM_PREC_RADIX:
        sprintf( s, "SQL_DESC_NUM_PREC_RADIX" );
        break;

      case SQL_DESC_OCTET_LENGTH:
        sprintf( s, "SQL_DESC_OCTET_LENGTH" );
        break;

      case SQL_DESC_OCTET_LENGTH_PTR:
        sprintf( s, "SQL_DESC_OCTET_LENGTH_PTR" );
        break;

      case SQL_DESC_PARAMETER_TYPE:
        sprintf( s, "SQL_DESC_PARAMETER_TYPE" );
        break;

      case SQL_DESC_PRECISION:
        sprintf( s, "SQL_DESC_PRECISION" );
        break;

      case SQL_DESC_SCALE:
        sprintf( s, "SQL_DESC_SCALE" );
        break;

      case SQL_DESC_SCHEMA_NAME:
        sprintf( s, "SQL_DESC_SCHEMA_NAME" );
        break;

      case SQL_DESC_SEARCHABLE:
        sprintf( s, "SQL_DESC_SEARCHABLE" );
        break;

      case SQL_DESC_TABLE_NAME:
        sprintf( s, "SQL_DESC_TABLE_NAME" );
        break;

      case SQL_DESC_TYPE:
        sprintf( s, "SQL_DESC_TYPE" );
        break;

      case SQL_DESC_TYPE_NAME:
        sprintf( s, "SQL_DESC_TYPE_NAME" );
        break;

      case SQL_DESC_UNNAMED:
        sprintf( s, "SQL_DESC_UNNAMED" );
        break;

      case SQL_DESC_UNSIGNED:
        sprintf( s, "SQL_DESC_UNSIGNED" );
        break;

      case SQL_DESC_UPDATABLE:
        sprintf( s, "SQL_DESC_UPDATABLE" );
        break;

      default:
        sprintf( s, "%d", (int)type );
    }

    return s;
} /* pa90DescTypeAsString */


char* pa90ColAttrAsString( UWORD type, char *s )
{
    switch( type )
    {
      case SQL_DESC_AUTO_UNIQUE_VALUE:
        API_SPRINTF( s, "SQL_DESC_AUTO_UNIQUE_VALUE" );
        break;

      case SQL_DESC_BASE_COLUMN_NAME:
        API_SPRINTF( s, "SQL_DESC_BASE_COLUMN_NAME" );
        break;

      case SQL_DESC_BASE_TABLE_NAME:
        API_SPRINTF( s, "SQL_DESC_BASE_TABLE_NAME" );
        break;

      case SQL_DESC_CASE_SENSITIVE:
        API_SPRINTF( s, "SQL_DESC_CASE_SENSITIVE" );
        break;

      case SQL_DESC_CATALOG_NAME:
        API_SPRINTF( s, "SQL_DESC_CATALOG_NAME" );
        break;

      case SQL_DESC_CONCISE_TYPE:
        API_SPRINTF( s, "SQL_DESC_CONCISE_TYPE" );
        break;

      case SQL_DESC_DISPLAY_SIZE:
        API_SPRINTF( s, "SQL_DESC_DISPLAY_SIZE" );
        break;

      case SQL_DESC_FIXED_PREC_SCALE:
        API_SPRINTF( s, "SQL_DESC_FIXED_PREC_SCALE" );
        break;

      case SQL_DESC_LABEL:
        API_SPRINTF( s, "SQL_DESC_LABEL" );
        break;

      case SQL_COLUMN_NAME:
        API_SPRINTF( s, "SQL_COLUMN_NAME" );
        break;

      case SQL_DESC_LENGTH:
        API_SPRINTF( s, "SQL_DESC_LENGTH" );
        break;

      case SQL_COLUMN_LENGTH:
        API_SPRINTF( s, "SQL_COLUMN_LENGTH" );
        break;

      case SQL_DESC_LITERAL_PREFIX:
        API_SPRINTF( s, "SQL_DESC_LITERAL_PREFIX" );
        break;

      case SQL_DESC_LITERAL_SUFFIX:
        API_SPRINTF( s, "SQL_DESC_LITERAL_SUFFIX" );
        break;

      case SQL_DESC_LOCAL_TYPE_NAME:
        API_SPRINTF( s, "SQL_DESC_LOCAL_TYPE_NAME" );
        break;

      case SQL_DESC_NAME:
        API_SPRINTF( s, "SQL_DESC_NAME" );
        break;

      case SQL_DESC_NULLABLE:
        API_SPRINTF( s, "SQL_DESC_NULLABLE" );
        break;

      case SQL_COLUMN_NULLABLE:
        API_SPRINTF( s, "SQL_COLUMN_NULLABLE" );
        break;

      case SQL_DESC_NUM_PREC_RADIX:
        API_SPRINTF( s, "SQL_DESC_NUM_PREC_RADIX" );
        break;

      case SQL_DESC_OCTET_LENGTH:
        API_SPRINTF( s, "SQL_DESC_OCTET_LENGTH" );
        break;

      case SQL_DESC_PRECISION:
        API_SPRINTF( s, "SQL_DESC_PRECISION" );
        break;

      case SQL_COLUMN_PRECISION:
        API_SPRINTF( s, "SQL_COLUMN_PRECISION" );
        break;

      case SQL_DESC_SCALE:
        API_SPRINTF( s, "SQL_DESC_SCALE" );
        break;

      case SQL_COLUMN_SCALE:
        API_SPRINTF( s, "SQL_COLUMN_SCALE" );
        break;

      case SQL_DESC_SCHEMA_NAME:
        API_SPRINTF( s, "SQL_DESC_SCHEMA_NAME" );
        break;

      case SQL_DESC_SEARCHABLE:
        API_SPRINTF( s, "SQL_DESC_SEARCHABLE" );
        break;

      case SQL_DESC_TABLE_NAME:
        API_SPRINTF( s, "SQL_DESC_TABLE_NAME" );
        break;

      case SQL_DESC_TYPE:
        API_SPRINTF( s, "SQL_DESC_TYPE" );
        break;

      case SQL_DESC_TYPE_NAME:
        API_SPRINTF( s, "SQL_DESC_TYPE_NAME" );
        break;

      case SQL_DESC_UNNAMED:
        API_SPRINTF( s, "SQL_DESC_UNNAMED" );
        break;

      case SQL_DESC_UNSIGNED:
        API_SPRINTF( s, "SQL_DESC_UNSIGNED" );
        break;

      case SQL_DESC_UPDATABLE:
        API_SPRINTF( s, "SQL_DESC_UPDATABLE" );
        break;

      default:
        API_SPRINTF( s, "%d", (int)type );
    } /* switch */

    return s;    
} /* pa90ColAttrAsString */


char* pa90GetInfoAttrAsString( UWORD attr, char *literal )
{
    switch (attr) {
        WHCASESTRING(literal, SQL_ODBC_SAG_CLI_CONFORMANCE);
        WHCASESTRING(literal, SQL_ODBC_SQL_OPT_IEF);
        WHCASESTRING(literal, SQL_ODBC_VER);
        WHCASESTRING(literal, SQL_DATA_SOURCE_NAME);
        WHCASESTRING(literal, SQL_DRIVER_NAME);
        WHCASESTRING(literal, SQL_SERVER_NAME);
        WHCASESTRING(literal, SQL_USER_NAME);
        WHCASESTRING(literal, SQL_ODBC_SQL_CONFORMANCE);
        WHCASESTRING(literal, SQL_IDENTIFIER_CASE);
        WHCASESTRING(literal, SQL_FETCH_DIRECTION);
        WHCASESTRING(literal, SQL_SCROLL_CONCURRENCY);
        WHCASESTRING(literal, SQL_SCROLL_OPTIONS);
        WHCASESTRING(literal, SQL_DRIVER_VER);
        WHCASESTRING(literal, SQL_ODBC_API_CONFORMANCE);
        WHCASESTRING(literal, SQL_SEARCH_PATTERN_ESCAPE);
        WHCASESTRING(literal, SQL_DATABASE_NAME);
        WHCASESTRING(literal, SQL_DBMS_NAME);
        WHCASESTRING(literal, SQL_DBMS_VER);
        WHCASESTRING(literal, SQL_PROCEDURES);
        WHCASESTRING(literal, SQL_CURSOR_COMMIT_BEHAVIOR);
        WHCASESTRING(literal, SQL_CURSOR_ROLLBACK_BEHAVIOR);
        WHCASESTRING(literal, SQL_ACTIVE_CONNECTIONS);
        WHCASESTRING(literal, SQL_ACTIVE_STATEMENTS);
        WHCASESTRING(literal, SQL_MAX_COLUMN_NAME_LEN);
        WHCASESTRING(literal, SQL_MAX_CURSOR_NAME_LEN);
        WHCASESTRING(literal, SQL_MAX_TABLE_NAME_LEN);
        WHCASESTRING(literal, SQL_MAX_OWNER_NAME_LEN);
        WHCASESTRING(literal, SQL_MAX_QUALIFIER_NAME_LEN);
        WHCASESTRING(literal, SQL_NUMERIC_FUNCTIONS);
        WHCASESTRING(literal, SQL_STRING_FUNCTIONS);
        WHCASESTRING(literal, SQL_SYSTEM_FUNCTIONS);
        WHCASESTRING(literal, SQL_TIMEDATE_FUNCTIONS);
        WHCASESTRING(literal, SQL_EXPRESSIONS_IN_ORDERBY);
        WHCASESTRING(literal, SQL_OUTER_JOINS);
        WHCASESTRING(literal, SQL_DATA_SOURCE_READ_ONLY);
        WHCASESTRING(literal, SQL_IDENTIFIER_QUOTE_CHAR);
        WHCASESTRING(literal, SQL_OWNER_TERM);
        WHCASESTRING(literal, SQL_QUALIFIER_NAME_SEPARATOR);
        WHCASESTRING(literal, SQL_QUALIFIER_TERM);
        WHCASESTRING(literal, SQL_TABLE_TERM);
        WHCASESTRING(literal, SQL_TXN_CAPABLE);
        WHCASESTRING(literal, SQL_CONCAT_NULL_BEHAVIOR);
        WHCASESTRING(literal, SQL_MAX_ROW_SIZE_INCLUDES_LONG);
        WHCASESTRING(literal, SQL_MAX_CHAR_LITERAL_LEN);
        WHCASESTRING(literal, SQL_TIMEDATE_ADD_INTERVALS);
        WHCASESTRING(literal, SQL_TIMEDATE_DIFF_INTERVALS);
        WHCASESTRING(literal, SQL_NEED_LONG_DATA_LEN);
        WHCASESTRING(literal, SQL_MAX_BINARY_LITERAL_LEN);
        WHCASESTRING(literal, SQL_LIKE_ESCAPE_CLAUSE);
        WHCASESTRING(literal, SQL_QUALIFIER_LOCATION);
        WHCASESTRING(literal, SQL_ACTIVE_ENVIRONMENTS);
        WHCASESTRING(literal, SQL_ALTER_DOMAIN);
        WHCASESTRING(literal, SQL_SQL_CONFORMANCE);
        WHCASESTRING(literal, SQL_DATETIME_LITERALS);
        WHCASESTRING(literal, SQL_ASYNC_MODE);
        WHCASESTRING(literal, SQL_BATCH_ROW_COUNT );
        WHCASESTRING(literal, SQL_BATCH_SUPPORT);
        WHCASESTRING(literal, SQL_CONVERT_WCHAR);
        WHCASESTRING(literal, SQL_CONVERT_INTERVAL_DAY_TIME);
        WHCASESTRING(literal, SQL_CONVERT_INTERVAL_YEAR_MONTH );
        WHCASESTRING(literal, SQL_CONVERT_WLONGVARCHAR );
        WHCASESTRING(literal, SQL_CONVERT_WVARCHAR );
        WHCASESTRING(literal, SQL_CREATE_ASSERTION);
        WHCASESTRING(literal, SQL_CREATE_CHARACTER_SET);
        WHCASESTRING(literal, SQL_CREATE_COLLATION);
        WHCASESTRING(literal, SQL_CREATE_DOMAIN );
        WHCASESTRING(literal, SQL_CREATE_SCHEMA);
        WHCASESTRING(literal, SQL_CREATE_TABLE);
        WHCASESTRING(literal, SQL_CREATE_TRANSLATION);
        WHCASESTRING(literal, SQL_CREATE_VIEW );
        WHCASESTRING(literal, SQL_DRIVER_HDESC);
        WHCASESTRING(literal, SQL_DROP_ASSERTION);
        WHCASESTRING(literal, SQL_DROP_CHARACTER_SET);
        WHCASESTRING(literal, SQL_DROP_COLLATION);
        WHCASESTRING(literal, SQL_DROP_DOMAIN );
        WHCASESTRING(literal, SQL_DROP_SCHEMA );
        WHCASESTRING(literal, SQL_DROP_TABLE );
        WHCASESTRING(literal, SQL_DROP_TRANSLATION);
        WHCASESTRING(literal, SQL_DROP_VIEW );
        WHCASESTRING(literal, SQL_DYNAMIC_CURSOR_ATTRIBUTES1);
        WHCASESTRING(literal, SQL_DYNAMIC_CURSOR_ATTRIBUTES2);
        WHCASESTRING(literal, SQL_FORWARD_ONLY_CURSOR_ATTRIBUTES1);
        WHCASESTRING(literal, SQL_FORWARD_ONLY_CURSOR_ATTRIBUTES2);
        WHCASESTRING(literal, SQL_INDEX_KEYWORDS);
        WHCASESTRING(literal, SQL_INFO_SCHEMA_VIEWS               );
        WHCASESTRING(literal, SQL_KEYSET_CURSOR_ATTRIBUTES1       );
        WHCASESTRING(literal, SQL_KEYSET_CURSOR_ATTRIBUTES2       );
        WHCASESTRING(literal, SQL_MAX_ASYNC_CONCURRENT_STATEMENTS );
        WHCASESTRING(literal, SQL_ODBC_INTERFACE_CONFORMANCE      );
        WHCASESTRING(literal, SQL_PARAM_ARRAY_ROW_COUNTS          );
        WHCASESTRING(literal, SQL_PARAM_ARRAY_SELECTS             );
        WHCASESTRING(literal, SQL_SQL92_DATETIME_FUNCTIONS        );
        WHCASESTRING(literal, SQL_SQL92_FOREIGN_KEY_DELETE_RULE   );
        WHCASESTRING(literal, SQL_SQL92_FOREIGN_KEY_UPDATE_RULE   );
        WHCASESTRING(literal, SQL_SQL92_GRANT                     );
        WHCASESTRING(literal, SQL_SQL92_NUMERIC_VALUE_FUNCTIONS   );
        WHCASESTRING(literal, SQL_SQL92_PREDICATES                );
        WHCASESTRING(literal, SQL_SQL92_RELATIONAL_JOIN_OPERATORS );
        WHCASESTRING(literal, SQL_SQL92_REVOKE                    );
        WHCASESTRING(literal, SQL_SQL92_ROW_VALUE_CONSTRUCTOR     );
        WHCASESTRING(literal, SQL_SQL92_STRING_FUNCTIONS          );
        WHCASESTRING(literal, SQL_SQL92_VALUE_EXPRESSIONS         );
        WHCASESTRING(literal, SQL_STANDARD_CLI_CONFORMANCE        );
        WHCASESTRING(literal, SQL_STATIC_CURSOR_ATTRIBUTES1       );
        WHCASESTRING(literal, SQL_STATIC_CURSOR_ATTRIBUTES2       );
        WHCASESTRING(literal, SQL_AGGREGATE_FUNCTIONS             );
        WHCASESTRING(literal, SQL_DDL_INDEX                       );
        WHCASESTRING(literal, SQL_DM_VER                          );   
        WHCASESTRING(literal, SQL_INSERT_STATEMENT                );
        WHCASESTRING(literal, SQL_CORRELATION_NAME                );
        WHCASESTRING(literal, SQL_MAX_PROCEDURE_NAME_LEN          );
        WHCASESTRING(literal, SQL_MAX_STATEMENT_LEN               );
        WHCASESTRING(literal, SQL_MAX_USER_NAME_LEN               );
        WHCASESTRING(literal, SQL_QUOTED_IDENTIFIER_CASE          );
        WHCASESTRING(literal, SQL_NON_NULLABLE_COLUMNS            );
        WHCASESTRING(literal, SQL_LOCK_TYPES );
        WHCASESTRING(literal, SQL_GETDATA_EXTENSIONS );
        WHCASESTRING(literal, SQL_POS_OPERATIONS );
        WHCASESTRING(literal, SQL_STATIC_SENSITIVITY );
        WHCASESTRING(literal, SQL_TXN_ISOLATION_OPTION );
        WHCASESTRING(literal, SQL_BOOKMARK_PERSISTENCE );        
        WHCASESTRING(literal, SQL_DRIVER_ODBC_VER );
        WHCASESTRING(literal, SQL_DEFAULT_TXN_ISOLATION );
        WHCASESTRING(literal, SQL_COLUMN_ALIAS );
    default: {
            API_STRCPY(literal,"** unknown! **");
            break;
        }
    }

    return literal;
} /* p90GetInfoAttrAsString */


char* pa90StmtAttrAsString( SDWORD type, char *s )
{
    switch( type )
    {
      case SQL_ATTR_APP_PARAM_DESC:
        sprintf( s, "SQL_ATTR_APP_PARAM_DESC" );
        break;

      case SQL_ATTR_APP_ROW_DESC:
        sprintf( s, "SQL_ATTR_APP_ROW_DESC" );
        break;

      case SQL_ATTR_ASYNC_ENABLE:
        sprintf( s, "SQL_ATTR_ASYNC_ENABLE" );
        break;

      case SQL_ATTR_CONCURRENCY:
        sprintf( s, "SQL_ATTR_CONCURRENCY" );
        break;

      case SQL_ATTR_CURSOR_SCROLLABLE:
        sprintf( s, "SQL_ATTR_CURSOR_SCROLLABLE" );
        break;

      case SQL_ATTR_CURSOR_SENSITIVITY:
        sprintf( s, "SQL_ATTR_CURSOR_SENSITIVITY" );
        break;

      case SQL_ATTR_CURSOR_TYPE:
        sprintf( s, "SQL_ATTR_CURSOR_TYPE" );
        break;

      case SQL_ATTR_ENABLE_AUTO_IPD:
        sprintf( s, "SQL_ATTR_ENABLE_AUTO_IPD" );
        break;

      case SQL_ATTR_FETCH_BOOKMARK_PTR:
        sprintf( s, "SQL_ATTR_FETCH_BOOKMARK_PTR" );
        break;

      case SQL_ATTR_IMP_PARAM_DESC:
        sprintf( s, "SQL_ATTR_IMP_PARAM_DESC" );
        break;

      case SQL_ATTR_IMP_ROW_DESC:
        sprintf( s, "SQL_ATTR_IMP_ROW_DESC" );
        break;

      case SQL_ATTR_KEYSET_SIZE:
        sprintf( s, "SQL_ATTR_KEYSET_SIZE" );
        break;

      case SQL_ATTR_MAX_LENGTH:
        sprintf( s, "SQL_ATTR_MAX_LENGTH" );
        break;

      case SQL_ATTR_MAX_ROWS:
        sprintf( s, "SQL_ATTR_MAX_ROWS" );
        break;

      case SQL_ATTR_METADATA_ID:
        sprintf( s, "SQL_ATTR_METADATA_ID" );
        break;

      case SQL_ATTR_NOSCAN:
        sprintf( s, "SQL_ATTR_NOSCAN" );
        break;

      case SQL_ATTR_PARAM_BIND_OFFSET_PTR:
        sprintf( s, "SQL_ATTR_PARAM_BIND_OFFSET_PTR" );
        break;

      case SQL_ATTR_PARAM_BIND_TYPE:
        sprintf( s, "SQL_ATTR_PARAM_BIND_TYPE" );
        break;

      case SQL_ATTR_PARAM_OPERATION_PTR:
        sprintf( s, "SQL_ATTR_PARAM_OPERATION_PTR" );
        break;

      case SQL_ATTR_PARAM_STATUS_PTR:
        sprintf( s, "SQL_ATTR_PARAM_STATUS_PTR" );
        break;

      case SQL_ATTR_PARAMS_PROCESSED_PTR:
        sprintf( s, "SQL_ATTR_PARAMS_PROCESSED_PTR" );
        break;

      case SQL_ATTR_PARAMSET_SIZE:
        sprintf( s, "SQL_ATTR_PARAMSET_SIZE" );
        break;

      case SQL_ATTR_QUERY_TIMEOUT:
        sprintf( s, "SQL_ATTR_QUERY_TIMEOUT" );
        break;

      case SQL_ATTR_RETRIEVE_DATA:
        sprintf( s, "SQL_ATTR_RETRIEVE_DATA" );
        break;

      case SQL_ROWSET_SIZE:
        sprintf( s, "SQL_ROWSET_SIZE" );
        break;

      case SQL_ATTR_ROW_ARRAY_SIZE:
        sprintf( s, "SQL_ATTR_ROW_ARRAY_SIZE" );
        break;

      case SQL_ATTR_ROW_BIND_OFFSET_PTR:
        sprintf( s, "SQL_ATTR_ROW_BIND_OFFSET_PTR" );
        break;

      case SQL_ATTR_ROW_BIND_TYPE:
        sprintf( s, "SQL_ATTR_ROW_BIND_TYPE" );
        break;

      case SQL_ATTR_ROW_NUMBER:
        sprintf( s, "SQL_ATTR_ROW_NUMBER" );
        break;

      case SQL_ATTR_ROW_OPERATION_PTR:
        sprintf( s, "SQL_ATTR_ROW_OPERATION_PTR" );
        break;

      case SQL_ATTR_ROW_STATUS_PTR:
        sprintf( s, "SQL_ATTR_ROW_STATUS_PTR" );
        break;

      case SQL_ATTR_ROWS_FETCHED_PTR:
        sprintf( s, "SQL_ATTR_ROWS_FETCHED_PTR" );
        break;

      case SQL_ATTR_SIMULATE_CURSOR:
        sprintf( s, "SQL_ATTR_SIMULATE_CURSOR" );
        break;

      case SQL_ATTR_USE_BOOKMARKS:
        sprintf( s, "SQL_ATTR_USE_BOOKMARKS" );
        break;

      default:
        sprintf( s, "%d", (int)type );
    }

    return s;
} /* pa90StmtAttrAsString */


char* pa90SQLTypeAsString( SWORD type, char *s )
{
    switch( type )
    {
      case SQL_DOUBLE:
        sprintf( s, "SQL_DOUBLE" );
        break;

      case SQL_FLOAT:
        sprintf( s, "SQL_FLOAT" );
        break;

      case SQL_REAL:
        sprintf( s, "SQL_REAL" );
        break;

      case SQL_BIT:
        sprintf( s, "SQL_BIT" );
        break;

      case SQL_CHAR:
        sprintf( s, "SQL_CHAR" );
        break;

      case SQL_VARCHAR:
        sprintf( s, "SQL_VARCHAR" );
        break;

      case SQL_LONGVARCHAR:
        sprintf( s, "SQL_LONGVARCHAR" );
        break;

      case SQL_BINARY:
        sprintf( s, "SQL_BINARY" );
        break;

      case SQL_VARBINARY:
        sprintf( s, "SQL_VARBINARY" );
        break;

      case SQL_LONGVARBINARY:
        sprintf( s, "SQL_LONGVARBINARY" );
        break;

      case SQL_DECIMAL:
        sprintf( s, "SQL_DECIMAL" );
        break;

      case SQL_NUMERIC:
        sprintf( s, "SQL_NUMERIC" );
        break;

      case SQL_BIGINT:
        sprintf( s, "SQL_BIGINT" );
        break;

      case SQL_INTEGER:
        sprintf( s, "SQL_INTEGER" );
        break;

      case SQL_SMALLINT:
        sprintf( s, "SQL_SMALLINT" );
        break;

      case SQL_TINYINT:
        sprintf( s, "SQL_TINYINT" );
        break;

      case SQL_TYPE_DATE:
        sprintf( s, "SQL_TYPE_DATE" );
        break;

      case SQL_TYPE_TIME:
        sprintf( s, "SQL_TYPE_TIME" );
        break;

      case SQL_TYPE_TIMESTAMP:
        sprintf( s, "SQL_TYPE_TIMESTAMP" );
        break;

      case SQL_DATE:
        sprintf( s, "SQL_DATE" );
        break;

      case SQL_TIME:
        sprintf( s, "SQL_TIME" );
        break;

      case SQL_TIMESTAMP:
        sprintf( s, "SQL_TIMESTAMP" );
        break;

      case SQL_INTERVAL_YEAR:
        sprintf( s, "SQL_INTERVAL_YEAR" );
        break;

      case SQL_INTERVAL_YEAR_TO_MONTH:
        sprintf( s, "SQL_INTERVAL_YEAR_TO_MONTH" );
        break;

      case SQL_INTERVAL_MONTH:
        sprintf( s, "SQL_INTERVAL_MONTH" );
        break;

      case SQL_INTERVAL_DAY_TO_SECOND:
        sprintf( s, "SQL_INTERVAL_DAY_TO_SECOND" );
        break;

      case SQL_INTERVAL_DAY_TO_MINUTE:
        sprintf( s, "SQL_INTERVAL_DAY_TO_MINUTE" );
        break;

      case SQL_INTERVAL_DAY:
        sprintf( s, "SQL_INTERVAL_DAY" );
        break;

      case SQL_INTERVAL_HOUR_TO_SECOND:
        sprintf( s, "SQL_INTERVAL_HOUR_TO_SECOND" );
        break;

      case SQL_INTERVAL_HOUR_TO_MINUTE:
        sprintf( s, "SQL_INTERVAL_HOUR_TO_MINUTE" );
        break;

      case SQL_INTERVAL_HOUR:
        sprintf( s, "SQL_INTERVAL_HOUR" );
        break;

      case SQL_INTERVAL_MINUTE_TO_SECOND:
        sprintf( s, "SQL_INTERVAL_MINUTE_TO_SECOND" );
        break;

      case SQL_INTERVAL_MINUTE:
        sprintf( s, "SQL_INTERVAL_MINUTE" );
        break;

      case SQL_INTERVAL_SECOND:
        sprintf( s, "SQL_INTERVAL_SECOND" );
        break;

      case SQL_ALL_TYPES:
        sprintf( s, "SQL_ALL_TYPES" );
        break;

      default:
        sprintf( s, "Unknown(%d)", (int)type );
        break;
    }

    return s;
} /* pa90SQLTypeAsString */


char* pa90CTypeAsString( SWORD type, char *str )
{
    switch (type) {
        WHCASESTRING(str, SQL_C_CHAR   );
        WHCASESTRING(str, SQL_C_LONG   );
        WHCASESTRING(str, SQL_C_SHORT  );
        WHCASESTRING(str, SQL_C_FLOAT  );
        WHCASESTRING(str, SQL_C_DOUBLE );
        WHCASESTRING(str, SQL_C_NUMERIC);
        WHCASESTRING(str, SQL_C_DEFAULT);
        WHCASESTRING(str, SQL_C_DATE);
        WHCASESTRING(str, SQL_C_TIME);
        WHCASESTRING(str, SQL_C_TIMESTAMP  );
        WHCASESTRING(str, SQL_C_TYPE_DATE     );
        WHCASESTRING(str, SQL_C_TYPE_TIME     );
        WHCASESTRING(str, SQL_C_TYPE_TIMESTAMP);
        WHCASESTRING(str, SQL_C_BINARY  );   
        WHCASESTRING(str, SQL_C_BIT     );   
        WHCASESTRING(str, SQL_C_SBIGINT);
        WHCASESTRING(str, SQL_C_UBIGINT);
        WHCASESTRING(str, SQL_C_TINYINT );   
        WHCASESTRING(str, SQL_C_SLONG   );   
        WHCASESTRING(str, SQL_C_SSHORT  );   
        WHCASESTRING(str, SQL_C_STINYINT);   
        WHCASESTRING(str, SQL_C_ULONG  );        
        WHCASESTRING(str, SQL_C_USHORT );    
        WHCASESTRING(str, SQL_C_UTINYINT);   
        WHCASESTRING(str, SQL_C_GUID);        
    default: {
            API_STRCPY(str,"** unknown! **");
            break;
        }
    }
    
    return str;
} /* pa90CTypeAsString */


char* pa90EnvAttrAsString( SWORD type, char *s ) 
{
    switch( type )
    {
      case SQL_ATTR_CONNECTION_POOLING:
        sprintf( s, "SQL_ATTR_CONNECTION_POOLING" );
        break;

      case SQL_ATTR_CP_MATCH:
        sprintf( s, "SQL_ATTR_CP_MATCH" );
        break;

      case SQL_ATTR_ODBC_VERSION:
        sprintf( s, "SQL_ATTR_ODBC_VERSION" );
        break;

      case SQL_ATTR_OUTPUT_NTS:
        sprintf( s, "SQL_ATTR_OUTPUT_NTS" );
        break;

      default:
        sprintf( s, "%d", (int)type );
    }

    return s;
} /* pa90EnvAttrAsString */


char* pa90ConnAttrAsString( SDWORD type, char *s )
{
    switch( type )
    {
      case SQL_ATTR_ACCESS_MODE:
        sprintf( s, "SQL_ATTR_ACCESS_MODE" );
        break;

      case SQL_ATTR_ASYNC_ENABLE:
        sprintf( s, "SQL_ATTR_ASYNC_ENABLE" );
        break;

      case SQL_ATTR_AUTO_IPD:
        sprintf( s, "SQL_ATTR_AUTO_IPD" );
        break;

      case SQL_ATTR_AUTOCOMMIT:
        sprintf( s, "SQL_ATTR_AUTOCOMMIT" );
        break;

      case SQL_ATTR_CONNECTION_TIMEOUT:
        sprintf( s, "SQL_ATTR_CONNECTION_TIMEOUT" );
        break;

      case SQL_ATTR_CURRENT_CATALOG:
        sprintf( s, "SQL_ATTR_CURRENT_CATALOG" );
        break;

      case SQL_ATTR_LOGIN_TIMEOUT:
        sprintf( s, "SQL_ATTR_LOGIN_TIMEOUT" );
        break;

      case SQL_ATTR_METADATA_ID:
        sprintf( s, "SQL_ATTR_METADATA_ID" );
        break;

      case SQL_ATTR_ODBC_CURSORS:
        sprintf( s, "SQL_ATTR_ODBC_CURSORS" );
        break;

      case SQL_ATTR_PACKET_SIZE:
        sprintf( s, "SQL_ATTR_PACKET_SIZE" );
        break;

      case SQL_ATTR_QUIET_MODE:
        sprintf( s, "SQL_ATTR_QUIET_MODE" );
        break;

      case SQL_ATTR_TRACE:
        sprintf( s, "SQL_ATTR_TRACE" );
        break;

      case SQL_ATTR_TRACEFILE:
        sprintf( s, "SQL_ATTR_TRACEFILE" );
        break;

      case SQL_ATTR_TRANSLATE_LIB:
        sprintf( s, "SQL_ATTR_TRANSLATE_LIB" );
        break;

      case SQL_ATTR_TRANSLATE_OPTION:
        sprintf( s, "SQL_ATTR_TRANSLATE_OPTION" );
        break;

      case SQL_ATTR_TXN_ISOLATION:
        sprintf( s, "SQL_ATTR_TXN_ISOLATION" );
        break;

      default:
          pa90StmtAttrAsString( type, s );
    }

    return s;
} /* pa90ConnAttrAsString */


char* pa90UpdateableAsString( SDWORD attr, char *literal )
{
    switch (attr) {
        WHCASESTRING(literal, SQL_ATTR_READONLY);
        WHCASESTRING(literal, SQL_ATTR_WRITE);
        WHCASESTRING(literal, SQL_ATTR_READWRITE_UNKNOWN);
    default: {
            API_STRCPY(literal,"** unknown! **");
            break;
        }
    }; /* switch */

    return literal;
} /* pa90UpdateableAsString */

char* pa90DiagFieldAsString( SDWORD type, char *s )
{
    switch( type )
    {
      case SQL_DIAG_CURSOR_ROW_COUNT:
        sprintf( s, "SQL_DIAG_CURSOR_ROW_COUNT" );
        break;

      case SQL_DIAG_DYNAMIC_FUNCTION:
        sprintf( s, "SQL_DIAG_DYNAMIC_FUNCTION" );
        break;

      case SQL_DIAG_DYNAMIC_FUNCTION_CODE:
        sprintf( s, "SQL_DIAG_DYNAMIC_FUNCTION_CODE" );
        break;

      case SQL_DIAG_NUMBER:
        sprintf( s, "SQL_DIAG_NUMBER" );
        break;

      case SQL_DIAG_RETURNCODE:
        sprintf( s, "SQL_DIAG_RETURNCODE" );
        break;

      case SQL_DIAG_ROW_COUNT:
        sprintf( s, "SQL_DIAG_ROW_COUNT" );
        break;

      case SQL_DIAG_CLASS_ORIGIN:
        sprintf( s, "SQL_DIAG_CLASS_ORIGIN" );
        break;

      case SQL_DIAG_COLUMN_NUMBER:
        sprintf( s, "SQL_DIAG_COLUMN_NUMBER" );
        break;

      case SQL_DIAG_CONNECTION_NAME:
        sprintf( s, "SQL_DIAG_CONNECTION_NAME" );
        break;

      case SQL_DIAG_MESSAGE_TEXT:
        sprintf( s, "SQL_DIAG_MESSAGE_TEXT" );
        break;

      case SQL_DIAG_NATIVE:
        sprintf( s, "SQL_DIAG_NATIVE" );
        break;

      case SQL_DIAG_ROW_NUMBER:
        sprintf( s, "SQL_DIAG_ROW_NUMBER" );
        break;

      case SQL_DIAG_SERVER_NAME:
        sprintf( s, "SQL_DIAG_SERVER_NAME" );
        break;

      case SQL_DIAG_SQLSTATE:
        sprintf( s, "SQL_DIAG_SQLSTATE" );
        break;

      case SQL_DIAG_SUBCLASS_ORIGIN:
        sprintf( s, "SQL_DIAG_SUBCLASS_ORIGIN" );
        break;

      default:
        sprintf( s, "%d", (int)type );
    }

    return s;
} /* pa90DiagFieldAsString */


char* pa90CursorTypeAsString( UDWORD attr, char *str )
{
   switch(attr) {
       WHCASESTRING(str, SQL_CURSOR_KEYSET_DRIVEN);
       WHCASESTRING(str, SQL_CURSOR_DYNAMIC);
       WHCASESTRING(str, SQL_CURSOR_STATIC);
       WHCASESTRING(str, SQL_CURSOR_FORWARD_ONLY);
       WHCASESTRING(str, SQL_ROW_ADDED);
   default: {
           API_STRCPY(str, "## INVALID ##");
           break;
       }
   }

   return str;
} /* pa90CursorTypeAsString */


char* pa90FetchTypeAsString( UWORD attr, char *str )
{
    switch(attr) {
        WHCASESTRING(str, SQL_FETCH_NEXT);
        WHCASESTRING(str, SQL_FETCH_FIRST);
        WHCASESTRING(str, SQL_FETCH_LAST);
        WHCASESTRING(str, SQL_FETCH_PRIOR);
        WHCASESTRING(str, SQL_FETCH_ABSOLUTE);
        WHCASESTRING(str, SQL_FETCH_RELATIVE);
        /*WHCASESTRING(str, SQL_FETCH_RESUME); */
        WHCASESTRING(str, SQL_FETCH_BOOKMARK);
    default: {
            API_SPRINTF(str, "SQL_FETCH_(%d)", attr);
            break;
        }
    }

    return str;
} /* pa90FetchTypeAsString */


char* pa90OpTypeAsString( UWORD opType, char *literal )
{
    switch (opType) {
        WHCASESTRING( literal, SQL_POSITION );
        WHCASESTRING( literal, SQL_ADD );
        WHCASESTRING( literal, SQL_REFRESH );
        WHCASESTRING( literal, SQL_DELETE );
        WHCASESTRING( literal, SQL_UPDATE );
    default: {
            API_SPRINTF( literal, "unknown opType %d", opType );
            break;
        }
    }; /* switch */

    return literal;
} /* pa90OpTypeAsString */
