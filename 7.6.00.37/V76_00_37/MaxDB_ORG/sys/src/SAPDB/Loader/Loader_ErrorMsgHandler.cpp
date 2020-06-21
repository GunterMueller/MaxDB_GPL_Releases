/*!
  \file    Loader_ErrorMsgHandler.cpp
  \author  SteffenS
  \brief   Implements the Loader error message handler

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (C) 2002-2004 SAP AG

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

#include "Loader/Loader_Types.hpp"
#include "Loader/Loader_ErrorCode.hpp"
#include "Loader/Loader_ErrorMsgHandler.hpp"


typedef struct
{
    LOADER_ErrorCode    ErrorCode;
    char                pszErrorText[1024];
} Loader_ErrorData;

// Numbers come from Loader_ErrorCode.hpp
static Loader_ErrorData ErrorData[] =
{
    
    {LOA_ERR_UNKNOWN_DEFAULT,              "Unknown default value for column (%s)"},

    
    {LOA_ERR_UNKNOWN_PART,                  "Internal error: Error reading data from part (unknown part kind %d)"},

    // how is the internal error msg built: [file name (where error was thrown), line no (in this file), error no]
    {LOA_ERR_INTERNAL,                       "Internal error [%s, %d, %d]; contact technical support"},
    {LOA_ERR_COLUMN_SPEC_MISSING,            "Column specification parameter missing"},
    {LOA_ERR_CMD_NOT_INITIALIZED,            "Fastload command not initialized"},
    {LOA_ERR_INVALID_PARAMETERINDEX,         "Invalid parameter index %d"},
    {LOA_ERR_INVALID_HOSTTYPE,               "Invalid host type (%s) for parameter (%d)"},
    {LOA_ERR_NULL_PARAMETERADDR,             "Parameter (%d) contains null pointer"},
    {LOA_ERR_NEGATIVE_BUFFERLEN,             "Negative buffer length for parameter (%d) not allowed"},
    {LOA_ERR_INVALID_LENGTHINDICATOR,        "Invalid length or indicator value for parameter (%d)"},
    {LOA_ERR_INVALID_ROWARRAYSIZE,           "Invalid row array size 0"},
    {LOA_ERR_PARAMETER_NOT_SET,              "Parameter/Column (%d) not bound."},
    {LOA_ERR_TOO_MANY_FLCMD_OBJECTS,         "Only one FastloadCommand Object per Connection allowed"},

    {LOA_ERR_SQL_CMD_EXECUTION_FAILED,      "Execution of sql statement (%s) failed: %s"},

    {LOA_ERR_CONVERTING_ENCODING,           "Error converting data from %s to %s; source contains invalid characters or character sequences"},

    {LOA_ERR_EMPTY_NOT_ALLOWED,             "Insert empty data not allowed for datatype '%s' (parameter %d)"},
    {LOA_ERR_MISSING_DATA,                  "Missing data for column %s."},
    {LOA_ERR_SYSKEY_EXCEEDED,               "Syskey violation (the computed value exceeds the highest possible syskey value)."},
    // "Constant '<data>' must be compatible with column type and length (column: '<column name>')"
    {LOA_ERR_INCOMPATIBLE_CONST_LENGTH,     "Constant (val = '%.*s', len = %d) must be compatible with column type and length (column: '%s')."},
    {LOA_ERR_LONG_COL_NOT_ALLOWED,          "Table contains at least one column defined as LONG; IMPORT in PAGE format rejected"
                                            "(the only way to load LONG columns is a one-table IMPORT statement in ROW format)"},

    {LOA_ERR_NUMBER_OVERFLOW,               "Numeric overflow for parameter/column (%d)"},
    {LOA_ERR_NUMBER_TRUNCATED,              "Data truncated for parameter/column (%d)"},


    //  "No Data for NOT NULL - column without default (<column name>).
    //   You either have not specified this column in the command or
    //   there is no data in the data file for this column."
    {LOA_ERR_MISSING_DATA_NN_COL,           "No Data for NOT NULL - column without default (%s)."},

    {LOA_ERR_NULL_NOT_ALLOWED,               "Column %s must not contain NULL values."},

    {LOA_ERR_INVALID_HEX,                   "The value for column (%d) is not a valid HEX constant."},
    {LOA_ERR_KEY_LENGTH,                    "Data for key columns exceeds allowed key length."},

    {LOA_ERR_RECOVER_KEY_ORDER_VIOLATION,   "Attempt to insert single record after key sequence error failed with error %d = '%s'"},
    {LOA_ERR_KEY_ORDER,                     "Key sequence error\n"
                                            "last inserted key: '%s' (binary: '%s')\n"
                                            "new key:           '%s' (binary: '%s')."},

    {LOA_ERR_WRONG_DATE_TIME,               "Invalid input for date or time column (%d): '%.*s' - %s"},

    {LOA_ERR_GET_USERNAME_FAILED,           "Getting user name failed after successfully established connection (XUSER connect): %s"},
    {LOA_ERR_SQLDBC_CONNECT_FAILED,         "%s"},
    {LOA_ERR_CONNECTION_ALREADY_OPEN,       "Close open connection first"},
    {LOA_ERR_NO_CONNECTION,                 "No database connection established."},
    {LOA_ERR_INTERNAL_SQL_FAILED,           "Error executing internal SQL statement '%s': '%s'."},

    // Bsp: Failure in db communication (request): <fehlernummer> = <fehlertext>.
    // Fehlernummer und fehlertext werden von der LZU geliefert.
    {LOA_ERR_DB_COMMUNICATION_FAILURE,       "Failure in db communication (%s): %i = %.*s."},

    // do NOT use
    {LOA_END_OF_ARRAY,                   "Unknown Error - please report to SAP Basis Entwicklung Berlin\n"}
};



void
Loader_ErrorMsgHandler::setLoaderError(const LOADER_ErrorCode lErrorCode, ...)
{
    // Initialize error text
    clearError();

    m_ErrorCode = lErrorCode;

    va_list         arglist;
    va_start (arglist, lErrorCode);

    this->formatErrorText(lErrorCode, arglist);

    va_end (arglist);
}


void
Loader_ErrorMsgHandler::setLoaderErrorWSQL(const LOADER_ErrorCode lErrorCode,
                                           const LOADER_Int4       lSQLError,
                                           const LOADER_Char*      pszSQLErrorText,
                                           ...)
{
    // Initialize Loader error text
    clearError();

    m_SQLErrorCode    = lSQLError;

    LOADER_UInt4 _len = strlen(pszSQLErrorText);
    if (_len >= LOA_MAX_REPLY_LENGTH)
    {
        _len = LOA_MAX_REPLY_LENGTH - sizeof(LOADER_Char);
    }
    SAPDB_memcpy(m_SQLErrorText, pszSQLErrorText, _len);
    m_SQLErrorText[_len] = 0;

    m_ErrorCode = lErrorCode;

    va_list         arglist;
    va_start (arglist, pszSQLErrorText);

    this->formatErrorText(lErrorCode, arglist);

    va_end (arglist);
}
// setLoaderErrorWSQL()


void
Loader_ErrorMsgHandler::insertLoaderError(const LOADER_ErrorCode lErrorCode, ...)
{

    // keep history
    LOADER_Int4  _lAuxErrorCode = m_ErrorCode;
    LOADER_UInt4 _len           = strlen(m_ErrorText);
    LOADER_Char* _pAuxText      = new SAPDB_Char[_len + sizeof(LOADER_Char)];
    SAPDB_memcpy(_pAuxText, m_ErrorText, _len);
    _pAuxText[_len] = 0;

    // Initialize error text
    clearError();

    m_ErrorCode = lErrorCode;

    va_list         arglist;
    va_start (arglist, lErrorCode);

    this->formatErrorText(lErrorCode, arglist);

    va_end (arglist);


    // Append history to error text
    _len = strlen(m_ErrorText);

    sp77sprintf(m_ErrorText + _len, LOA_MAX_REPLY_LENGTH - _len, "\n%d = %s", _lAuxErrorCode, _pAuxText);

    if (NULL != _pAuxText)
    {
        delete [] _pAuxText;
    }
}
// insertLoaderError()


void 
Loader_ErrorMsgHandler::formatErrorText(const LOADER_ErrorCode lErrorCode, va_list arglist)
{
    LOADER_Char* _psztext = getMessageText(lErrorCode);

    sp77vsprintf(m_ErrorText, LOA_MAX_REPLY_LENGTH , _psztext, arglist);
    return;
}
// formatErrorText()


LOADER_Char*
Loader_ErrorMsgHandler::getMessageText(const LOADER_ErrorCode lErrorCode)
{
    LOADER_Int4 i = 0;

    while( (ErrorData[i].ErrorCode != lErrorCode) &&
           (ErrorData[i].ErrorCode != LOA_END_OF_ARRAY) )
    {
        i++;
    }
    return ErrorData[i].pszErrorText;
}