&ifndef DOXYGEN
/*!
  @file           IFR_ErrorHndl.cpp
  @author         DL SAP DB INTERFACES
  @ingroup        IFR_Common
  @brief          Storage for error messages and error codes
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
&endif
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#include "SAPDBCommon/SAPDB_ReuseLib.h"
#include "Interfaces/Runtime/IFR_ErrorHndl.h"
#include "Interfaces/Runtime/IFR_Trace.h"
#include "Interfaces/Runtime/IFR_TraceContext.h"

IFR_BEGIN_NAMESPACE

&ifndef DOXYGEN
//----------------------------------------------------------------------
struct IFR_ErrorData
{
    IFR_ErrorCode applcode;
    IFR_Int4      errorcode;
    const char*   sqlstate;
    const char   *msgformat;
};

//----------------------------------------------------------------------
// All the error messages.

/*

  To insert a new error it is necessary to change the file
  src/xx/PRECOMM.de und src/xx/PRECOMM.en immediatly.

  Please use the error codes from the following ranges.
  Some error codes may match with old errocode from the precompiler runtime
  -807 'connection down' matches to an already defined error in
  the range from -700 to -899 these please map these code into the range
  from -10700 to -10899 the messages from -9800 to -9999 are not mapped.

*/
&endif
/*! @page ErrorAndMessages SQLDBC Errorcodes and messages
    @par SQLDBC range for error codes

    The range from -10000 until -10999 is reserved for errocodes from SQLDBC.

    This range is splitted into groups for easier handle.

    -10000 - -10099 Environment, Tracing, Filehandling, Misc\n
    -10100 - -10199 Connection\n
    -10200 - -10299 API handling\n
    -10300 - -10399 Parameter, Column handling\n
    -10400 - -10499 Data Conversion\n
    -10500 - -10599 Retrieving results and array commands\n
    -10600 - -10699 Statement execution\n
    -10700 - -10799 Mapped Precompiler and Precompiler Runtime errors\n
    -10800 - -10899  as above\n
    -10900 - -10999 Fatal Errors\n

    Details are available <a HREF="http://dev.mysql.com/doc/maxdb/en/83/3e7540540ed362e10000000a155106/frameset.htm">here</a> 

*/

static IFR_ErrorData errordata[] =
{
    { IFR_ERR_UNKNOWN,                 -10899,     "",     "Unknown error"    },
    //**********************************************************************
    //******** Environment, Tracing, Filehandling 10000 - 10099 *********

    { IFR_ERR_ALLOCSHMEM_FAILED_SI,              -10803, "",      "Allocation of shared memory failed \"%s\" of size %d failed" },
    { IFR_ERR_ALLOCSHMEM_FAILED_S,               -10803, "",      "Allocation of shared memory failed (%s)" },
    // Use the macro IFR_ERR_NOT_IMPLEMENTED(x)
    { IFR_ERR_NOT_IMPLEMENTED_SSI,               -10808, "",      "Feature (%s) not implemented yet in module: %s line: %d" },
    { IFR_ERR_SYSTEM_ERROR_S,                    -10001, "",      "System error: %s." },
    { IFR_ERR_WRONG_KERNEL_VERSION_I,            -10003, "",      "Feature not supported for kernel version %d" },
    { IFR_ERR_SESSION_ALREADY_IN_USE,            -10810, "",      "Session already in use" },
    //**********************************************************************
    //******** Connection 10100 - 10199 *********

    { IFR_ERR_CONNECTION_DOWN_IIS,               -10807, "",      "Connection down: [%d] %.*s"  },
    { IFR_ERR_SERVERDB_MISSING,                  -10857, "",      "Missing database name for connection" },
    { IFR_ERR_USERNAME_MISSING,                  -10715, "",      "Missing user name for connection" },
    { IFR_ERR_PASSWORD_MISSING,                  -10715, "",      "Missing password for connection" },
    { IFR_ERR_CONNECTFAILED_INTERNAL_SS,         -10709, "",      "Connection failed (%s:%s)" },
    { IFR_ERR_INVALID_CONNECT_PROPERTY_S,        -10100, "",      "Invalid value for option \'%s\' of SQLDBC_ConnectProperties" },
    { IFR_ERR_INVALID_ISOLATIONLEVEL_S,          -10101, "",      "Invalid value for ISOLATIONLEVEL (%s)" },
    { IFR_ERR_INVALID_ISOLATIONLEVEL_I,          -10101, "",      "Invalid value for ISOLATIONLEVEL (%d)" },
    { IFR_ERR_INVALID_PACKETCOUNT_S,             -10102, "",      "Invalid value for PACKETCOUNT (%s)" },
    { IFR_ERR_INVALID_STATEMENTCACHESIZE_S,      -10103, "",      "Invalid value for STATEMENTCACHESIZE (%s)" },
    { IFR_ERR_INVALID_XUSERKEY,                  -10104, "",      "Invalid value for KEY" },
    { IFR_ERR_INVALID_CLIENTAPPLICATION_S,       -10106, "",      "Invalid client application id (%s)" },
    { IFR_ERR_XUSER_ERROR_S,                     -10757, "",      "XUSER error (%s)"},

    //***********************************************************
    //******** API handling 10200 - 10299               *********

    { IFR_ERR_SQLCMD_DATA_EXPECTED,              -10200, "HY010", "Function sequence error (data for execution expected)" },
    { IFR_ERR_SQLCMD_NO_DATA_EXPECTED,           -10201, "HY010", "Function sequence error (no data for execution expected)" },
    { IFR_ERR_SQLCMD_RECONNECT,                  -10202, "HY010", "Function sequence error (operation was aborted by disconnect)" },
    { IFR_ERR_SQLCMD_NOTPREPARED,                -10210, "HY010", "Invalid command state (No prepared SQL command)" },
    { IFR_ERR_METHOD_NOT_ALLOWED,                -10220, "HY010", "Calling %s is not allowed for %s." },
    { IFR_ERR_COMMAND_INFO_TOOLONG,              -10221, "HY010", "Command info to long [length is %d maximum length %d]" },

    //***********************************************************
    //******** Parameter, Column handling 10300 - 10399 *********
    { IFR_ERR_INVALID_PARAMETERINDEX,            -10300, "07009", "Invalid parameter index" },
    { IFR_ERR_INVALID_PARAMETERINDEX_I,          -10300, "07009", "Invalid parameter index %d" },
    { IFR_ERR_PARAMETERINDEX_NULL,               -10300, "07009", "Invalid parameter index 0" },
    { IFR_ERR_INVALID_HOSTTYPE_SI,               -10301, ""     , "Invalid host type (%s) for parameter (%d)" },
    { IFR_ERR_PARAMETER_NOT_SET_I,               -10304, "07002", "Parameter/Column (%d) not bound." },
    { IFR_ERR_NULL_PARAMETERADDR_I,              -10305, "HY090", "Parameter (%d) contains null pointer" },
    { IFR_ERR_NEGATIVE_BUFFERLEN_II,             -10306, "HY090", "Negative buffer length for parameter (%d) not allowed (found %d)" },
    { IFR_ERR_INVALID_LENGTHINDICATOR_I,         -10307, "HY090", "Invalid length or indicator value for parameter (%d)" },
    { IFR_ERR_NOPIECEWISE_DATA_WRITE_I,          -10308, "HY019", "Non-character and non-binary data for parameter (%d) for piece wise insert not allowed" },

    { IFR_ERR_INVALID_COLUMNINDEX_I,             -10310, "07009", "Invalid column index (%d)" },
    { IFR_ERR_NONULLDATAAPPEND_I,                -10312, "HY020", "Attempt to concatenate to a NULL or DEFAULT value for parameter (%d)" },
    { IFR_ERR_PARAMETER_ALREADY_PROCESSED_I,     -10313, "HY010", "Requested parameter (%d) was already processed" },
    { IFR_ERR_INVALID_PARAMETER_SEQUENCE_I,      -10314, "HY010", "Invalid parameter processing sequence, process"
      " non-LONG data before parameter (%d)" },
    { IFR_ERR_INVALID_STARTPOSITION_I,           -10315, ""     , "Invalid start position (%d)" },
    { IFR_ERR_DATA_AT_EXECUTE_NOT_ALLOWED_I,     -10316, "HY010", "DATA AT EXECUTE for parameter (%d) not allowed (conflicts "
      "with use of LOB parameter)" },
    { IFR_ERR_LOB_NOT_ALLOWED_I,                 -10317, "HY010", "LOB type for parameter (%d) not allowed (conflicts "
      "with use of DATA AT EXECUTE parameter)" },
    { IFR_ERR_INVALID_LOB,                       -10318, "HY010", "Invalid LOB object" },
    { IFR_ERR_LOB_READONLY_I,                    -10319, "HY010", "Could not read LOB for parameter/column (%d)" },
    { IFR_ERR_LOB_WRITEONLY_I,                   -10319, "HY010", "Could not write write LOB for parameter/column (%d)" },
    { IFR_ERR_LOB_CLOSED_I,                      -10320, "HY010", "LOB already closed LOB for parameter/column (%d)" },
    { IFR_ERR_LOB_INVALID_I,                     -10321, "HY010", "LOB invalidated by a previous error for "
      "parameter/column (%d)" },
    { IFR_ERR_LOB_NULLDEFAULT_I,                 -10322, "HY010", "Could not insert LOB."
      " Parameter/column (%d) has a NULL/DEFAULT value" },
    { IFR_ERR_NOPIECEWISE_DATA_READ_I,           -10323, "HY019", "Non-character and non-binary data for parameter/column (%d) was "
      "tried to read piecewise" },
    { IFR_ERR_NULL_STREAMHANDLE_I,               -10861, "HY010", "Invalid use of null pointer for stream handle in parameter (%d)" },
    { IFR_ERR_STREAMHANDLE_NOTFOUND_I,          -10324, "HY010", "Stream with id (%d) not found" },
    { IFR_ERR_STREAMHANDLE_NOTINPUT_I,          -10325, "HY010", "Stream parameter (%d) is not an input parameter" },
    { IFR_ERR_STREAMHANDLE_NOTOUTPUT_I,         -10325, "HY010", "Stream parameter (%d) is not an input parameter" },
    { IFR_ERR_STREAM_ERROR_INPUT_I,             -10862, "HY010", "Stream parameter (%d), error in read procedure" },
    { IFR_ERR_STREAM_ERROR_OUTPUT_I,            -10862, "HY010", "Stream parameter (%d), error in write procedure" },
    { IFR_ERR_STREAM_NO_DATA_I,                 -10326, "HY010", "No data found for stream (%d), but data was expected" },
    { IFR_ERR_NULL_STREAMPARAM_I,               -10327, "HY010", "Invalid use of null pointer in stream parameter field "
                                                                 "of stream descriptor for parameter/column (%d)" },
    { IFR_ERR_STREAM_MEMBERCOUNT_MISMATCH_III,  -10328, "HY010", "Mismatch of number of stream members for parameter (%d) "
                                                                  "(application %d, database %d)." },
    { IFR_ERR_INVALID_SERIAL_VALUE_TAG,         -10329, "HY010", "Invalid serial value requested, must be first or last value." },
    { IFR_ERR_STREAM_OUTPUT_SEQUENCE_ERROR_S,   -10330, "HY010", "Multiple stream output sequence error, unexpected '%s' part." },
    { IFR_ERR_STREAM_OVERFLOW_III,              -10331, "HY010", "Input stream callback buffer overflow for table id %d, "
      "allowed %d, written %d bytes." },
    { IFR_ERR_STREAM_NO_FUNCTION_I,             -10332, "HY010", "No callback function supplied for stream parameter %d." },
    { IFR_ERR_UNSUPPORTED_PARAMETERTYPE_I,      -10333, "",      "Unsupported database type in parameter/column (%d)." },
    //***********************************************************
    //******** Data Conversion 10400 - 10499 *********

    { IFR_ERR_PARAM_CONVERSION_LOOSEDIGITS_I,    -10400, "",      "Conversion looses significant digits for parameter/column (%d)" },
    { IFR_ERR_PARAM_CONVERSION_TRUNCATEDATA_I,   -10401, "",      "Conversion of parameter/column (%d) would truncate data" },
    { IFR_ERR_NUMERIC_OVERFLOW_I,                -10811, "",      "Numeric overflow for parameter/column (%d)" },
    { IFR_ERR_INVALID_NUMERIC_VALUE_I,           -10802, "",      "Invalid numeric value for parameter/column (%d)" },
    { IFR_ERR_CONVERSION_NOT_SUPPORTED,          -10404, "",      "Conversion not supported" },
    { IFR_ERR_CONVERSION_NOT_SUPPORTED_I,        -10404, "",      "Conversion not supported for parameter/column (%d)" },
    { IFR_ERR_CORRUPTED_UCS2ASCIIDATA_I,         -10405, ""     , "Cannot convert UCS2 data to ASCII for parameter/column (%d)" },
    { IFR_ERR_ODD_DATALENGTH_I,                  -10406, "",      "Invalid data length for UCS2 data for parameter/column (%d)" },
    { IFR_ERR_CORRUPTED_UTF8DATA_I,              -10407, ""     , "Corrupted UTF8 data for parameter/column (%d)" },
    { IFR_ERR_STRING_CONVERSION,                 -10408, ""     , "Character conversion failed" },
    { IFR_ERR_DATETIMEFORMAT_UNSUPPORTED_I,      -10410, "22007", "Date/Time format is not supported for parameter (%d)" },
    { IFR_ERR_ILLEGAL_DATE_VALUE_I,              -10411, "",      "Illegal DATE value for parameter/column (%d)" },
    { IFR_ERR_ILLEGAL_TIME_VALUE_I,              -10412, "",      "Illegal TIME value for parameter/column (%d)" },
    { IFR_ERR_ILLEGAL_TIMESTAMP_VALUE_I,         -10413, "",      "Illegal TIMESTAMP value for parameter/column (%d)" },
    { IFR_ERR_BINARY_TO_ENCODED_UNSUPPORTED_I,   -10414, "",      "Conversion from binary value to string is not enabled for "
      "parameter/column (%d)" },
    { IFR_ERR_NULL_DECIMAL_INDICATOR_I,          -10415, "",      "Indicator must not be a null pointer for DECIMAL input value "
      " for parameter/column (%d)" },
    { IFR_ERR_DECIMAL_INDICATOR_CORRUPT_I,       -10416, "",      "Invalid indicator value for DECIMAL input value for "
      " paramter/column (%d)" },
    { IFR_ERR_INVALID_DECIMAL_SPECIFICATION_III, -10417, "",      "Invalid definition for DECIMAL input value for paramater/column "
      " (%d), specified a DECIMAL(%d, %d)" },
    { IFR_ERR_DECIMAL_BUFFER_TOO_SHORT_I,        -10418, "",      "Buffer for DECIMAL too short for parameter/column (%d)" },
    { IFR_ERR_NOT_ASCII_CHARACTERS_I,            -10419, "",      "Character value contains non-ASCII characters (%d)" }, 
    { IFR_ERR_BYTE_CONVERSION_FAILED_I,          -10420, "",      "Could not convert byte buffer to hexadecimal string for "
      "parameter/column (%d)" },
    { IFR_ERR_LONG_LONG_I,                       -10421, "",      "Could not convert too large LONG data for parameter/column (%d)" },
    { IFR_ERR_BINARY_TO_ENCODED_UNSUPPORTED_I,   -10422, "01004", "Appending data to binary output not supported for parameter/column (%d)" },
    { IFR_ERR_BINARY_APPEND_I        ,           -10423, "22003", "Appending data to binary output not supported for numeric parameter/column (%d)" },
    { IFR_ERR_BINARY_TRUNCATION_SI,              -10424, "22003", "Conversion from %s data to binary would truncate data for parameter/column (%d)" },

    //**********************************************************************
    //******** Retrieving results and array commands 10500 - 10599 *********
    
    { IFR_ERR_ROW_NOT_FOUND,                      100,   "02000", "Row not found" },
    { IFR_ERR_RESULTSET_IS_CLOSED,               -10500, "",      "Result set is closed" },
    { IFR_ERR_RESULTSET_BEFOREFIRST,             -10501, "S1109", "Invalid position in resultset: before first row" },
    { IFR_ERR_RESULTSET_AFTERLAST,               -10502, "S1109", "Invalid position in resultset: after last row" },
    { IFR_ERR_RESULTSET_IS_FORWARD_ONLY,         -10503, "",      "The operation is not allowed for result set type FORWARD_ONLY" },
    { IFR_ERR_INVALID_ROWSET_POS_I,              -10504, "",      "Invalid position (%d) for rowset" },
    { IFR_ERR_ROWSET_IS_READ_ONLY,               -10505, "",      "Result set type is UPDATABLE but row set type is READ_ONLY." },
    { IFR_ERR_ROWSET_IS_UPDATABLE,               -10506, "",      "Result set type is READ_ONLY but row set type is UPDATABLE" },
    { IFR_ERR_ROWSET_NO_METADATA,                -10507, "",      "Internal error: No metadata available for rowset operation" },
    { IFR_ERR_ROWSET_WRONG_COLUMN_ENCODING_I,    -10508, "",      "Invalid encoding of column name for column %d for rowset operation"},
    { IFR_ERR_NO_UPDATABLE_ROWSET,               -10509, "",      "The statement was not executed with updatable cursor concurrency" },
    { IFR_ERR_INVALID_ROWARRAYSIZE,              -10856, "",      "Invalid row array size 0" },
    { IFR_ERR_NO_DATA_FOR_UPDATE,                -10510, "21S02", "Degree of derived table does not match column list" },


    //**********************************************************************
    //******** Statement execution 10600 - 10699 *********

    { IFR_ERR_SESSION_NOT_CONNECTED,             -10821,  "",     "Session not connected" },
    { IFR_ERR_EMPTY_SQL_STATEMENT,               -10601,  "",     "SQL command is the empty string" },
    { IFR_ERR_SQLCMD_NOT_ASCII,                  -10602,  "",     "SQL command contains UNICODE character" },
    { IFR_ERR_SQLCMD_RESULTSET,                  -10603,  "",     "SQL command would generate a result set" },
    { IFR_ERR_SQLCMD_OUTPUTPARAM,                -10604,  "",     "SQL command contains output parameters" },
    { IFR_ERR_STATEMENT_TOOLONG,                 -10706,  "",     "Statement too long" },
    { IFR_ERR_SCHEMAMODIFIED,                    -10605,  "",     "Illegal schema modification, parameters/columns have changed type/number incompatibly" },

    //**********************************************************************
    //******** Mapped Error codes 10700 - 10899 *********

    // Do not use any new values here. Please check for existing values.

    //**********************************************************************
    //******** Fatal errors 10900 - 10999 *********

    { IFR_ERR_PACKET_EXHAUSTED,                  -10901,  "",      "No space left in request packet" },
    { IFR_ERR_INVALID_REQUESTPACKET,             -10902,  "",      "Invalid request packet structure" },
    { IFR_ERR_MISSINGLONGDATAPART,               -10903,  "",      "Internal Error: LONGDATA part expected" },
    { IFR_ERR_MISSINGRESULTCOUNT,                -10904,  "",      "Internal error: Expected result count" },
    { IFR_ERR_INVALID_REPLYPACKET,               -10905,  "",      "Invalid reply packet structure" },
    { IFR_ERR_INVALID_VALINDEX_I,                -10906,  "",      "Internal Error: Entry for valindex %d not found"},
    { IFR_ERR_ERROR_LONGDESCRIPTOR,              -10907,  "",      "Internal Error: Long descriptor has error flag set"},
    { IFR_ERR_GETVAL_REQUIRED,                   -10908,  "",      "Internal Error: GETVAL operation required before data could "
      "be read"},
    //                                           -10909
    { IFR_ERR_INVALID_OBJECT,                    SQLDBC_INVALID_OBJECT,  "",      "Invalid object reference" }
#undef EERROR_START
#undef EERROR
};

&ifndef DOXYGEN

//----------------------------------------------------------------------
// get the error data for the code
// if the error is not found, the text of IFR_ERR_UNKNOWN is returned.
static IFR_ErrorData *geterrordata(IFR_ErrorCode code)
{
    int num_errors=sizeof(errordata)/sizeof(IFR_ErrorData);
    IFR_ErrorData *d=errordata;
    while(num_errors > 0) {
        if(d->applcode==code) {
            return d;
        }
        ++d;
        --num_errors;
    }
    return (IFR_ErrorData*)errordata;
}

//----------------------------------------------------------------------
char *
IFR_ErrorHndl::m_memory_allocation_failed
  = (char *)"Memory allocation failed.";



//----------------------------------------------------------------------
IFR_ErrorHndl::~IFR_ErrorHndl()
{
    if(m_allocator && m_message && m_message!=m_memory_allocation_failed) {
        IFR_ALLOCATOR(*m_allocator).Deallocate(m_message);
    }
}


//----------------------------------------------------------------------
void
IFR_ErrorHndl::assign(const IFR_ErrorHndl& assign, IFR_Bool counterAdd)
{
    if(this != &assign) {
        m_errorcode=assign.m_errorcode;
//        m_errorcount=assign.m_errorcount;
        memcpy(m_sqlstate,assign.m_sqlstate, 6);
        if(!m_allocator) {
            m_allocator=assign.m_allocator;
        }
        if(m_errorcode == 0) {
            if(m_message && m_message!=m_memory_allocation_failed) {
                IFR_ALLOCATOR(*m_allocator).Deallocate(m_message);
            }
            m_message = 0;
            m_byteslength = 0;
        }
        if(assign.m_message != m_memory_allocation_failed) {
            if(m_byteslength < assign.m_byteslength || m_message==m_memory_allocation_failed) {
                if(m_message && m_message!=m_memory_allocation_failed) {
                    IFR_ALLOCATOR(*m_allocator).Deallocate(m_message);
                }
                m_message=(char *) IFR_ALLOCATOR(*m_allocator).Allocate(assign.m_byteslength+1);
                if(m_message == 0) {
                    setMemoryAllocationFailed();
                }
            }
            if(assign.m_byteslength > 0 && assign.m_message) {
                m_byteslength=assign.m_byteslength;
                memcpy(m_message, assign.m_message, m_byteslength+1);  
            } else {
                m_byteslength = 0;
                m_message = 0;
            }
        } else {
            if(m_message && m_message!=m_memory_allocation_failed) {
                IFR_ALLOCATOR(*m_allocator).Deallocate(m_message);
            }
            m_message = assign.m_message;
            m_byteslength = assign.m_byteslength;
        }
        if(counterAdd && m_errorcode != 0) {
          m_errorcount++;
        }
    }
}

//----------------------------------------------------------------------
void
IFR_ErrorHndl::setRuntimeError(IFR_ErrorCode error, ...)
{
    DBUG_METHOD_ENTER(IFR_ErrorHndl, setRuntimeError);

    
    if ( !m_allocator ) {
      setMemoryAllocationFailed();
      return;
    }
    this->m_errorcount++;
    if(m_message!=0 && m_message!=m_memory_allocation_failed) {
        IFR_ALLOCATOR(*m_allocator).Deallocate(m_message);
    }

    va_list  ap;
    va_start(ap, error);

    IFR_ErrorData *data=geterrordata(error);

    m_errorcode=data->errorcode;
    strcpy(m_sqlstate,data->sqlstate);

    int fmtlen = (int) strlen(data->msgformat);
    int buflen = fmtlen*2+1;

    if(buflen < 512)
        buflen = 512;

    m_message=(char*) IFR_ALLOCATOR(*m_allocator).Allocate(buflen);

    if(m_message == 0) {
        setMemoryAllocationFailed();
        return;
    }

    memset(m_message, 0, buflen);

    int bytesprinted=sp77vsprintfUnicode(IFR_ENCODING(IFR_StringEncodingUTF8),
                                         m_message,
                                         buflen,
                                         data->msgformat,
                                         ap);
    if(bytesprinted < 0) {
        m_message[buflen-1]=0;
        m_byteslength = (IFR_Int4) strlen(m_message);
    } else {
        m_byteslength=bytesprinted;
    }
    va_end(ap);

    {
        IFR_TraceStream *s = IFR_GetTraceStream(m_connection);
        if(s) {
            *s << *this;
        }
    }
    
    return;
}

//----------------------------------------------------------------------
void
IFR_ErrorHndl::setMemoryAllocationFailed()
{
    // no DBUG as we are in a critical situation, DBUG might enter tracing,
    // and requests memory from system (e.g. for stream opening).
    if(m_allocator && m_message && m_message != m_memory_allocation_failed) {
        IFR_ALLOCATOR(*m_allocator).Deallocate(m_message);
        // m_message = 0;
    }
    m_errorcount++;
    m_message   = m_memory_allocation_failed;
    m_errorcode = -10760;
    strcpy(m_sqlstate, "HY001");
    m_byteslength = (IFR_Int4)strlen(m_memory_allocation_failed);
    return;
}

//----------------------------------------------------------------------
void
IFR_ErrorHndl::setSQLError(const IFR_Int4 errorcode,
                           const char* sqlstate,
                           const IFR_String& message,
                           IFR_Bool notrace)
{
    setSQLError(errorcode, sqlstate, message.getBuffer(), message.getEncoding(), notrace);
}

//----------------------------------------------------------------------
void
IFR_ErrorHndl::setFromRuntimeError(SQLDBC_IRuntime::Error& error)
{
    DBUG_METHOD_ENTER(IFR_ErrorHndl, setFromRuntimeError);

    if(error.errorcode == 0) {
        clear();
        return;
    }
    if (!m_allocator ) {
      setMemoryAllocationFailed();
      return;
    }
    
    if(m_message && m_message!=m_memory_allocation_failed) {
        IFR_ALLOCATOR(*m_allocator).Deallocate(m_message);
    }
    m_errorcount++;
    m_errorcode=error.errorcode;
    m_sqlstate[0]='\0';

    m_byteslength = (IFR_Int4) strlen(error.errortext);
    m_message=(char *) IFR_ALLOCATOR(*m_allocator).Allocate(m_byteslength+1);
    if(m_message == 0) {
        setMemoryAllocationFailed();
        return;
    }
    memcpy(m_message, error.errortext, m_byteslength+1);

    {
        IFR_TraceStream *s = IFR_GetTraceStream(m_connection);
        if(s) {
            *s << *this;
        }
    }
    
    return;
}

//----------------------------------------------------------------------
void
IFR_ErrorHndl::setSQLError(const IFR_Int4 errorcode,
                           const char *sqlstate,
                           const char *message,
                           IFR_StringEncoding encoding,
                           IFR_Bool notrace)

{
    DBUG_METHOD_ENTER(IFR_ErrorHndl, setSQLError);

    if ( !m_allocator ) {
      setMemoryAllocationFailed();
      return;
    }

    if(m_message && m_message!=m_memory_allocation_failed) {
        IFR_ALLOCATOR(*m_allocator).Deallocate(m_message);
    }
    m_errorcode=errorcode;

    if(sqlstate) {
        strcpy(m_sqlstate, sqlstate);
    } else {
        m_sqlstate[0]='\0';
    }
    if(encoding==IFR_StringEncodingUTF8) {
        m_byteslength = (IFR_Int4) strlen(message);
        m_message=(char *) IFR_ALLOCATOR(*m_allocator).Allocate(m_byteslength+1);
        if(m_message == 0) {
            setMemoryAllocationFailed();
            return;
        }
        memcpy(m_message, message, m_byteslength+1);
    } else {
        IFR_UInt charLength;
        IFR_UInt byteLength = 0;
        IFR_Int isTerminated, isCorrupted, isExhausted;
        IFR_ENCODING(encoding)->stringInfo(message, MAX_IFR_INT4, 0,
                                           &charLength, &byteLength, &isTerminated,
                                           &isCorrupted, &isExhausted);
        int buflen = (int) byteLength*4+1;
        m_message=(char *) IFR_ALLOCATOR(*m_allocator).Allocate(buflen);
        if(m_message == 0) {
            setMemoryAllocationFailed();
            return;
        }
        IFR_UInt4 destbyteswritten;
        IFR_UInt4 bytesparsed;
        tsp78ConversionResult cres=sp78convertString(sp77encodingUTF8,
                                                     m_message,
                                                     buflen,
                                                     &destbyteswritten,
                                                     true,
                                                     IFR_ENCODING(encoding),
                                                     message,
                                                     byteLength,
                                                     &bytesparsed);
        if(cres==sp78_Ok) {
            m_byteslength=(IFR_Int4) strlen(m_message);
        } else {
            m_message[0]=0;
            m_byteslength=0;
        }
    }
    if(!notrace) {
      if (errorcode == 100){
          m_RowNotFoundCount++;
        } else {
          m_errorcount++;
        }
        IFR_TraceStream *s = IFR_GetTraceStream(m_connection);
        if(s) {
            *s << *this;
        }
    }
}


//----------------------------------------------------------------------
void
IFR_ErrorHndl::clear()
{
    m_errorcode = 0;
    m_sqlstate[0] = '\0';
    if(m_allocator && m_message && m_message!=m_memory_allocation_failed) {
        IFR_ALLOCATOR(*m_allocator).Deallocate(m_message);
    }
    m_message = 0;
    m_byteslength = 0;
}


//----------------------------------------------------------------------
IFR_TraceStream&
operator << (IFR_TraceStream& s, const IFR_ErrorHndl& e)
{
  SQLDBC_IRuntime::TaskTraceContext *trace_context = IFR_GetTraceContext(&s);
  if (trace_context && trace_context->flags & IFR_Trace::SQLTrace) {
      e.sqlTrace(s);
  } else {
      if (trace_context && trace_context->flags & (IFR_Trace::CallTrace|IFR_Trace::DebugTrace)) {
          if(*(e.getSQLState())) {
            s << "[" << e.getErrorCode() << ", " << e.getSQLState() << ", "<< inputencoding(IFR_StringEncodingUTF8) << e.getErrorText() << "]";
          } else {
              s << "[" << e.getErrorCode() <<  ", " << inputencoding(IFR_StringEncodingUTF8) << e.getErrorText() << "]";
          }
      }
      if(e.getErrorCode()) {
          s.traceError(e.getErrorCode(), e.getConnection());
      }
  }
  return s;
}

//----------------------------------------------------------------------
void
IFR_ErrorHndl::sqlTrace(IFR_TraceStream& s) const
{
    if (getErrorCode()) {
        s <<   "SQL ERROR  :" << endl;
        s <<   "  CODE     : " << getErrorCode() << endl;
        if(*(getSQLState()))
            s << "  SQLSTATE : " << getSQLState() << endl;
        s <<   "  MESSAGE  : " << inputencoding(IFR_StringEncodingUTF8) << getErrorText() << endl;
        s.traceError(getErrorCode(), getConnection());
    }
}
&endif


IFR_END_NAMESPACE
