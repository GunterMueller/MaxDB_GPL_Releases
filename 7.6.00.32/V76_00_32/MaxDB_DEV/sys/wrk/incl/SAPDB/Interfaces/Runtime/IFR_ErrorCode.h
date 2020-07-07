/*!
  @file           IFR_ErrorCode.h
  @author         DL SAP DB INTERFACES
  @ingroup        IFR_Common
  @brief          Error Codes for the Interface Runtime
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
#ifndef IFR_ERRORCODE_H
#define IFR_ERRORCODE_H

/**
 * @ingroup IFR_Common
 * Internal error code. These error codes are mapped to (not necessary unique)
 * external error codes and messages.
 * The error codes contain the needed arguments as suffix, e.g. <code>IFR_ERR_EXAMPLE_ISP</code>
 * takes an int, a nullterminated ascii string, and a pointer.
 * Valid suffixes are
 * <ul>
 *   <li><code>I</code> - integer (<code>IFR_Int4</code>)</li>
 *   <li><code>U</code> - unsigned integer (<code>IFR_UInt4</code>)</li>
 *   <li><code>S</code> - nullterminated ascii string (<code>char *</code>).</li>
 *   <li><code>P</code> - pointer</li>
 *   <li><code>E</code> - nullterminated character string with encoding (<code>IFR_StringEncoding, char*</code>)</li>
 *   <li><code>X</code> - binary data with a length (<code>int, char*</code>)</li>
 * </ul>
 */
enum IFR_ErrorCode
{
    IFR_ERR_UNKNOWN,                  //!< Unknown error.
    IFR_ERR_CONNECTION_DOWN_IIS,      //!< Connection down.
#define IFR_ERR_NOT_IMPLEMENTED(x) IFR_ERR_NOT_IMPLEMENTED_SSI, x, __FILE__, __LINE__
    IFR_ERR_NOT_IMPLEMENTED_SSI,       //!< Feature not implemented module: %s line %d.
    IFR_ERR_SESSION_NOT_CONNECTED,    //!< Session is not connected.
    IFR_ERR_SYSTEM_ERROR_S,           //!< A severe system error happened, which is described in the error text.
    IFR_ERR_WRONG_KERNEL_VERSION_I,   //!< Feature not supported for this kernel version.
    IFR_ERR_SESSION_ALREADY_IN_USE,   //!< Session already in use.

    IFR_ERR_INVALID_INDEX,                     //!< The value specified as vector index is invalid.
    IFR_ERR_PARAM_CONVERSION_LOOSEDIGITS_I,    //!< A number conversion would looses significant digits.
    IFR_ERR_NUMERIC_OVERFLOW,                  //!< A number is to large to be converted to the request number type.
    IFR_ERR_NUMERIC_OVERFLOW_I,                //!< A number is to large to be converted to the request number type (parameter index).
    IFR_ERR_INVALID_NUMERIC_VALUE_I,           //!< Illegal number value (parameter index).
    IFR_ERR_PARAM_CONVERSION_TRUNCATEDATA_I,     //!< A conversion would truncate significant data (parameter index).
    IFR_ERR_CONVERSION_NOT_SUPPORTED,          //!< A conversion is not supported by the API.
    IFR_ERR_CONVERSION_NOT_SUPPORTED_I,        //!< A conversion is not supported by the API (with parameter index).
    IFR_ERR_BINARY_TO_ENCODED_UNSUPPORTED_I,   //!< A conversion from (VAR)CHAR BYTE to ASCII/UCS2/UTF8 is not supported.
    IFR_ERR_ILLEGAL_DATE_VALUE_I,              //!< Illegal DATE value (format, or logic).
    IFR_ERR_ILLEGAL_TIME_VALUE_I,              //!< Illegal TIME value (format, or logic).
    IFR_ERR_ILLEGAL_TIMESTAMP_VALUE_I,         //!< Illegal TIMESTAMP value (format, or logic).
    IFR_ERR_NULL_DECIMAL_INDICATOR_I,          //!< NULL indicator for decimal value.
    IFR_ERR_DECIMAL_INDICATOR_CORRUPT_I,       //!< Corrupt/invalid decimal indicator value.
    IFR_ERR_INVALID_DECIMAL_SPECIFICATION_III, //!< Invalid decimal specifiction (fraction > digits)
    IFR_ERR_DECIMAL_BUFFER_TOO_SHORT_I,        //!< Buffer length too short for decimal value.
    IFR_ERR_NOT_ASCII_CHARACTERS_I,            //!< Parameter did contain characters that are not 7-bit ASCII.
    IFR_ERR_BYTE_CONVERSION_FAILED_I,          //!< A byte buffer could not be converted into a hexadecimal dump.
    IFR_ERR_LONG_LONG_I,                       //!< Conversion failed, to long LONG data.
    IFR_ERR_BINARY_APPEND_I,                   //!< Append to binary data is not supported.
    IFR_ERR_BINARY_TRUNCATION_SI,              //!< Conversion to binary would truncate data.
    
    IFR_ERR_SERVERDB_MISSING,                  //!< Database name was not specified.
    IFR_ERR_USERNAME_MISSING,                  //!< User name was not specified.
    IFR_ERR_PASSWORD_MISSING,                  //!< Password was not specified.
    IFR_ERR_INVALID_XUSERKEY,                  //!< The provided XUSER key is invalid.
    IFR_ERR_XUSER_ERROR_S,                     //!< Error in XUSER handling.
    IFR_ERR_CONNECTFAILED_INTERNAL_SS,         //!< Connect failed due to a runtime error.
    IFR_ERR_INVALID_CONNECT_PROPERTY_S,        //!< Illegal value for \'%s\' connect property.
    IFR_ERR_STATEMENT_TOOLONG,                 //!< Text of a statement is too long.
    IFR_ERR_DATETIMEFORMAT_UNSUPPORTED_I,      //!< Date/Time format is not supported.
    IFR_ERR_INVALID_CLIENTAPPLICATION_S,       //!< Invalid client application id supplied.
    IFR_ERR_PARAMETERINDEX_NULL,               //!< Parameter index starts with 1.
    IFR_ERR_INVALID_ROWARRAYSIZE,              //!< Array size for mass command must be >= 1.
    IFR_ERR_INVALID_ROWSETSIZE,                //!< Rowset size must be >= 1.
    IFR_ERR_INVALID_ROWSET_POS_I,              //!< Invalid position <var>pos</var> for rowset.
    IFR_ERR_EMPTY_SQL_STATEMENT,               //!< SQL statement is the empty string.
    IFR_ERR_SQLCMD_NOTPREPARED,                //!< No SQL command was prepared.
    IFR_ERR_SQLCMD_RESULTSET,                  //!< SQL command would create a result set.
    IFR_ERR_SQLCMD_OUTPUTPARAM,                //!< SQL command contains output parameters.
    IFR_ERR_SQLCMD_NOT_ASCII,                  //!< SQL command contains UNICODE character.
    IFR_ERR_PACKET_EXHAUSTED,                  //!< Internal error: No space left in request packet.
    IFR_ERR_METHOD_NOT_ALLOWED,                //!< Calling %s is not allowed for %s.
    IFR_ERR_COMMAND_INFO_TOOLONG,              //!< Command info to long [length is %d maximum length %d].

    IFR_ERR_PARAMETER_NOT_SET_I,               //!< Parameter <var>index</var> not set.
    IFR_ERR_INVALID_HOSTTYPE_SI,               //!< Invalid hosttype <var>type</var> for parameter/column <var>index</var>.
    IFR_ERR_INVALID_LENGTHINDICATOR_I,         //!< Value of length/indicator for parameter <var>index</var>
                                               //!< not allowed in this context.
    IFR_ERR_INVALID_STARTPOSITION_I,           //!< Start position to read non-integral data is not valid.
    IFR_ERR_ODD_DATALENGTH_I,                  //!< A data length in bytes that is odd is not allowed in this context.
    IFR_ERR_CORRUPTED_UTF8DATA_I,              //!< UTF8 data does not follow the rules of UTF
    IFR_ERR_CORRUPTED_UCS2ASCIIDATA_I,         //!< UCS2 to ASCII: Invalid characters in data.
    IFR_ERR_STRING_CONVERSION,                 //!< A string conversion (one encoding to another) failed.
    IFR_ERR_INVALID_PARAMETERINDEX,            //!< Invalid parameter index.
    IFR_ERR_INVALID_COLUMNINDEX_I,             //!< Invalid column index <var>index</var>.
    IFR_ERR_INVALID_REQUESTPACKET,             //!< Internal error: Valid request packet expected.
    IFR_ERR_INVALID_REPLYPACKET,               //!< Internal error: Valid reply packet expected.
    IFR_ERR_ERROR_LONGDESCRIPTOR,              //!< Internal error: Long descriptor has error flag set.
    IFR_ERR_INVALID_PARAMETERINDEX_I,          //!< Invalid parameter index.
    IFR_ERR_PARAMETER_ALREADY_PROCESSED_I,     //!< Next requested parameter was already processed.
    IFR_ERR_INVALID_PARAMETER_SEQUENCE_I,      //!< Next requested parameter is not valid in the sequence.
    IFR_ERR_INVALID_SERIAL_VALUE_TAG,          //!< Serial value requested is not first or last value.
    IFR_ERR_UNSUPPORTED_PARAMETERTYPE_I,       //!< Unsupported type of database parameter.
    IFR_ERR_NULL_PARAMETERADDR_I,              //!< Null value for parameter address.
    IFR_ERR_NEGATIVE_BUFFERLEN_II,             //!< Buffer length is negative.
    IFR_ERR_INVALID_PACKETCOUNT_S,             //!< An invalid packet count was given.
    IFR_ERR_ROW_NOT_FOUND,                     //!< (100) Row not found.
    IFR_ERR_RESULTSET_BEFOREFIRST,             //!< Position in resultset before first row.
    IFR_ERR_RESULTSET_AFTERLAST,               //!< Position in resultset after last row.
    IFR_ERR_RESULTSET_IS_FORWARD_ONLY,         //!< The operation is not allowed for result set type FORWARD_ONLY.
    IFR_ERR_INVALID_ISOLATIONLEVEL_S,          //!< An invalid isolation level was provided.
    IFR_ERR_INVALID_ISOLATIONLEVEL_I,          //!< An invalid isolation level was provided.
    IFR_ERR_INVALID_STATEMENTCACHESIZE_S,      //!< An invalid statement cache size was given.
    IFR_ERR_ALLOCSHMEM_FAILED_SI,              //!< The allocation of shared memory at the path failed.
    IFR_ERR_ALLOCSHMEM_FAILED_S,               //!< The allocation of shared memory at the path failed.
    IFR_ERR_RESULTSET_IS_CLOSED,               //!< The ResultSet is closed.
    IFR_ERR_SQLCMD_DATA_EXPECTED,              //!< A <code>nextParameter</code> - <code>putData</code> sequence was expected.
    IFR_ERR_SQLCMD_NO_DATA_EXPECTED,           //!< A <code>nextParameter</code> - <code>putData</code> sequence was not expected.
    IFR_ERR_NOPIECEWISE_DATA_WRITE_I,          //!< A parameter of an integral data type was tried to be written in pieces.
    IFR_ERR_NOPIECEWISE_DATA_READ_I,           //!< A parameter of an integral data type was tried to be read in pieces.
    IFR_ERR_NONULLDATAAPPEND_I,                //!< It was tried to append data to a parameter that was specified as NULL or DEFAULT.
    IFR_ERR_MISSINGLONGDATAPART,               //!< The application did want a LONGDATA part, but did not get one.
    IFR_ERR_SCHEMAMODIFIED,                    //!< The schema was modified in a way that was impossible to handle by the application.
    IFR_ERR_MISSINGRESULTCOUNT,                //!< A result count part was expected, but missing.
    IFR_ERR_INVALID_VALINDEX_I,                //!< An index in a long descriptor identified a non-existing LONG value.
    IFR_ERR_NULL_STREAMHANDLE_I,               //!< A stream handle bound for transmitting an ABAP stream can not be 0.
    IFR_ERR_STREAMHANDLE_NOTFOUND_I,           //!< The requested Stream Handle was not found in the parameters.
    IFR_ERR_STREAMHANDLE_NOTINPUT_I,           //!< The stream handle is not bound as input stream (there is
                                               //!< no read procedure).
    IFR_ERR_STREAMHANDLE_NOTOUTPUT_I,          //!< The stream handle is not bound as input stream (there is
                                               //!< no read procedure).
    IFR_ERR_STREAM_ERROR_INPUT_I,              //!< The read procedure produced an error.
    IFR_ERR_STREAM_ERROR_OUTPUT_I,             //!< The write procedure produced an error.
    IFR_ERR_STREAM_NO_DATA_I,                  //!< Data for an output stream was expected, but not found.
    IFR_ERR_NULL_STREAMPARAM_I,                //!< The stream parameter of the stream descriptor is null.
    IFR_ERR_STREAM_MEMBERCOUNT_MISMATCH_III,   //!< Member count for structured streams does not match the database information.
    IFR_ERR_STREAM_NO_FUNCTION,                //!< No function provided for ABAP stream parameter.         
    IFR_ERR_STREAM_OUTPUT_SEQUENCE_ERROR_S,    //!< The output sequence was not sent correctly by the kernel.
    IFR_ERR_STREAM_OVERFLOW_III,               //!< The read procedure reported that it wrote to much.
    IFR_ERR_STREAM_NO_FUNCTION_I,              //!< No function supplied for ABAP stream.
    IFR_ERR_ROWSET_IS_READ_ONLY,               //!< result set type UPDATABLE does not match to row set type READ_ONLY
    IFR_ERR_ROWSET_IS_UPDATABLE,               //!< result set type READ_ONLY does not match to row set type UPDATABLE
    IFR_ERR_ROWSET_NO_METADATA,                //!< Internal error - no meta data available for row set operation.
    IFR_ERR_ROWSET_WRONG_COLUMN_ENCODING_I,    //!< Invalid encoding of column name for row set operation.
    IFR_ERR_NO_UPDATABLE_ROWSET,               //!< The statement was not executed with updatable cursor concurrency.
    IFR_ERR_NO_DATA_FOR_UPDATE,                //!< Degree of derived table does not match column list.
    IFR_ERR_STREAM_ERROR_SI,                   //!< A general internal error happened in stream handling.
    
    IFR_ERR_DATA_AT_EXECUTE_NOT_ALLOWED_I,     //!< DATA AT EXECUTE not allowed for parameter (LOB was found before).
    IFR_ERR_LOB_NOT_ALLOWED_I,                 //!< LOB object not allowed for parameter (DATA AT EXECUTE found before).
    IFR_ERR_INVALID_LOB,                       //!< Invalid LOB object found.
    IFR_ERR_LOB_READONLY_I,                    //!< A LOB used for PUTVAL is read only.
    IFR_ERR_LOB_WRITEONLY_I,                   //!< A LOB used for GETVAL is write only.
    IFR_ERR_LOB_CLOSED_I,                      //!< A LOB that was closed before is used.
    IFR_ERR_LOB_INVALID_I,                     //!< A LOB invalidated by a previous error was used.
    IFR_ERR_LOB_NULLDEFAULT_I,                 //!< A LOB parameter that was used with a NULL or DEFAULT indicator was used.
    IFR_ERR_INVALID_OBJECT,                    //!< A invalid object reference was used or passed to a method.
    IFR_ERR_GETVAL_REQUIRED,                   //!< A GETVAL operation is required before data can be read.
    IFR_ERR_SQLCMD_RECONNECT,                  //!< A transactional operation was aborted by a disconnect/connect sequence.
    IFR_ERR_MAX                                //!< Max value for bounds check.
};


#endif
