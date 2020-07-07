/*!**********************************************************************

  module: hpa111ODCompr2.h

  -----------------------------------------------------------------------

  responsible:  

  special area: ODBC

  description:  ODBC extension to compress/decompress data

  see also:

  -----------------------------------------------------------------------

  copyright:               Copyright (c) 1999-2005 SAP AG




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

#include "hpa111ODCompr2.h"

/*!**********************************************************************

  Class: tpa111_ODBCOutStream

************************************************************************/

tpa110_ComprRet pa111_ConvOdbcRet( SQLRETURN sqlRetcode )
{
    switch (sqlRetcode ) {
    case SQL_SUCCESS:
    case SQL_SUCCESS_WITH_INFO:
        return COMPR_SUCCESS_PA110;
    default:
        return COMPR_ERROR_PA110;
    }; // switch
}; // pa111_ConvOdbcRet


SQLRETURN pa111_ConvComprRet( tpa110_ComprRet comprRet )
{
    switch (comprRet ) {
    case COMPR_SUCCESS_PA110:
    case COMPR_SUCCESS_WITH_INFO_PA110:
        return SQL_SUCCESS;
    default:
        return SQL_ERROR;
    }; // switch
}; // pa111_ConvComprRet


class tpa111_ODBCOutStream : public tpa110_OutStream
{
public:
    tpa111_ODBCOutStream()
        : hstmt(SQL_NULL_HSTMT) {};

    void Init( SQLHSTMT outHstmt )
    {
        this->hstmt = outHstmt;
    }; // Init

    tpa110_ComprRet WriteData( const tpa110_DataPtr  buffer,
                               tsp00_Int4            numBytes )
    {
        return pa111_ConvOdbcRet( SQLPutData( this->hstmt,
                                              buffer,
                                              (long) numBytes ));
    }; // WriteData

    // used to write the NULL value
    SQLRETURN PutData( SQLPOINTER buffer, SQLINTEGER strLenOrIndi )
    {
        return SQLPutData( this->hstmt, buffer, (long) strLenOrIndi );
    }; // PutData

private:
    SQLHSTMT  hstmt;
}; // tpa111_ODBCOutStream


class tpa111_ODBCInStream : public tpa110_InStream
{
public:
    tpa111_ODBCInStream()
        : hstmt(SQL_NULL_HSTMT), colNo(0),
          haveTermByte(false), firstReadData(true){};

    void Init( SQLHSTMT inHstmt, SQLUSMALLINT targetColNo )
    {
        this->hstmt = inHstmt;
        this->colNo = targetColNo;
        this->firstReadData = true;
        this->haveTermByte = false;
    }; // Init

    tpa110_ComprRet ReadData( tpa110_DataPtr  buffer,
                              tsp00_Int4      bufSize,
                              tsp00_Int4     &bytesRead,
                              tsp00_Bool     &moreData );

    tsp00_Bool IsNullValue();

private:
    SQLHSTMT      hstmt;
    SQLUSMALLINT  colNo;
    tsp00_Bool    haveTermByte; // is true for "old" single block
                                // compressed data. For new block
                                // wise compression we fill up with
                                // 0 up to the requested size
    tsp00_Bool    firstReadData; // true between Init and first ReadData
                                 // call
}; // tpa111_ODBCInStream

// from CsObjectInt.cpp !!!
static unsigned char wd111CsMagicHead[] = { "\037\235" };  /* 1F 9D */

tpa110_ComprRet tpa111_ODBCInStream::ReadData( tpa110_DataPtr  buffer,
                                               tsp00_Int4      bufSize,
                                               tsp00_Int4     &bytesRead,
                                               tsp00_Bool     &moreData )
{
    SQLRETURN       sqlRetcode   = SQL_SUCCESS;
    tpa110_ComprRet comprRetcode = COMPR_SUCCESS_PA110;
    SQLLEN     strLenOrIndi      = 0;

    sqlRetcode = SQLGetData( this->hstmt,
                             this->colNo,
                             SQL_C_BINARY,
                             buffer, bufSize,
                             &strLenOrIndi );
    if (firstReadData
        && (sqlRetcode == SQL_SUCCESS
            || sqlRetcode == SQL_SUCCESS_WITH_INFO)) {
        // check for compression type (old or new)
        // ASSERT( strLenOrIndi > CS_HEAD_SIZE )
        if (buffer[5] == wd111CsMagicHead[0]
            && buffer[6] == wd111CsMagicHead[1]) {
            // old single block compression
            this->haveTermByte = true;
        };
        firstReadData = false;
    };
    if (sqlRetcode == SQL_SUCCESS) {
        if (strLenOrIndi == SQL_NULL_DATA) {
            // null values can't be handled by ComprFilter
            comprRetcode = COMPR_ERROR_PA110;
        } else {
            if (this->haveTermByte) {
                strLenOrIndi--; // ignore termByte
                bytesRead = (tsp00_Int4) strLenOrIndi;
            } else {
                if (bufSize > strLenOrIndi) {
                    // assume that the remaining bytes are 0 and were truncated
                    // by ODBC.
                    // fill up 0 bytes until bufSize
                    memset( buffer + strLenOrIndi, 0, bufSize-strLenOrIndi );
                };
                bytesRead = bufSize;
            };
            moreData = false;
        }; // else
    } else if (sqlRetcode == SQL_SUCCESS_WITH_INFO) {
        UCHAR       sqlState[10];
        SQLINTEGER  nativeErr;
        UCHAR       errorMsg[100];
        SQLSMALLINT msgLen;

        // check sqlstate value
        sqlRetcode = SQLError( SQL_NULL_HENV, SQL_NULL_HDBC, hstmt,
                               sqlState,
                               &nativeErr,
                               errorMsg,
                               sizeof(errorMsg),
                               &msgLen );

        if (sqlRetcode != SQL_SUCCESS) {
            // unexpected return code
            comprRetcode = COMPR_ERROR_PA110;
            moreData = false;
        } else if (strcmp( (char*) sqlState, "01004" ) == 0) {
            // Data truncated
            moreData = true;
            bytesRead = bufSize;
        } else {
            // ignore warning
            moreData = false;
            bytesRead = (tsp00_Int4) strLenOrIndi;
        }; // else             
    }; // else

    return comprRetcode;
}; // ReadData


tsp00_Bool tpa111_ODBCInStream::IsNullValue()
{
    SQLRETURN    sqlRetcode   = SQL_SUCCESS;
    SQLLEN       strLenOrIndi = 0;
    char         dummy = 0;

    sqlRetcode = SQLGetData( this->hstmt,
                             this->colNo,
                             SQL_C_BINARY,
                             &dummy,
                             0,
                             &strLenOrIndi );

    if (sqlRetcode == SQL_ERROR) {
        UCHAR       sqlState[10];
        SQLINTEGER  nativeErr;
        UCHAR       errorMsg[100];
        SQLSMALLINT msgLen;

        sqlRetcode = SQLError( SQL_NULL_HENV, SQL_NULL_HDBC, hstmt,
                               sqlState,
                               &nativeErr,
                               errorMsg,
                               sizeof(errorMsg),
                               &msgLen );
    };

    return (sqlRetcode == SQL_SUCCESS && strLenOrIndi == SQL_NULL_DATA);
}; // IsNullValue

/*!**********************************************************************

  EndClass: ODBCOutStream

************************************************************************/

/*!**********************************************************************

  Class: ODCompr2

************************************************************************/

// constructor

tpa111_ODCompr2::tpa111_ODCompr2( tsp00_Int4 maxBlockSize )
    : odbcOutStream(NULL), comprFilter(NULL)
{
    this->odbcOutStream = new tpa111_ODBCOutStream();
    this->comprFilter   = new tpa110_ComprFilter( maxBlockSize );
}; // ODCompr2


// desctructor

tpa111_ODCompr2::~tpa111_ODCompr2()
{
    delete this->odbcOutStream;
    delete this->comprFilter;
}; // ODCompr2


SQLRETURN tpa111_ODCompr2::CompressBuffer( const SQLPOINTER  buffer,
                                           SQLINTEGER  bufferLen,
                                           tsp00_Int4 *bytesReadPtr,
                                           tsp00_Int4 *bytesCompressedPtr,
                                           tsp00_Bool *allBytesCompressedPtr,
                                           tsp00_Int4 *comprErr )
{
    tpa110_ComprRet comprRet = COMPR_SUCCESS_PA110;

    comprRet = this->comprFilter->CompressData( (unsigned char*) buffer,
                                                bufferLen,
                                                *bytesReadPtr,
                                                *bytesCompressedPtr,
                                                *allBytesCompressedPtr,
                                                *comprErr );

    return pa111_ConvComprRet( comprRet );
}; // CompressBuffer


SQLRETURN tpa111_ODCompr2::FlushBuffer()
{
    tpa110_ComprRet comprRet = COMPR_SUCCESS_PA110;

    comprRet = this->comprFilter->FlushData();

    return pa111_ConvComprRet( comprRet );
}; // FlushBuffer

 
SQLRETURN tpa111_ODCompr2::InitCompr( SQLHSTMT    hstmt,
                                      tsp00_Int4  totalDataSize,
                                      tsp00_Int4 *bytesWritten,
                                      tsp00_Int4 *comprErr )
{
    tpa110_ComprRet comprRet = COMPR_SUCCESS_PA110;

    this->odbcOutStream->Init( hstmt );
    comprRet = this->comprFilter->Init( this->odbcOutStream,
                                        totalDataSize,
                                        *bytesWritten,
                                        *comprErr );

    return pa111_ConvComprRet( comprRet );
}; // InitCompr


SQLRETURN tpa111_ODCompr2::InitComprAppend( SQLHSTMT    hstmt,
                                            tsp00_Int4 *comprErr )
{
    tpa110_ComprRet comprRet = COMPR_SUCCESS_PA110;

    this->odbcOutStream->Init( hstmt );
    comprRet = this->comprFilter->InitAppend( this->odbcOutStream,
                                              *comprErr );

    return pa111_ConvComprRet( comprRet );
}; // InitComprAppend


SQLRETURN tpa111_ODCompr2::PutData( const SQLPOINTER  buffer,
                                    SQLINTEGER        strLenOrIndi,
                                    tsp00_Int4       *comprErr )
{
    tpa110_ComprRet comprRet = COMPR_SUCCESS_PA110;
    SQLRETURN       sqlRetcode = SQL_SUCCESS;

    if (strLenOrIndi == SQL_NULL_DATA) {
        // don't use comprFilter
        sqlRetcode = this->odbcOutStream->PutData( buffer, strLenOrIndi );
    } else {
        comprRet = this->comprFilter->WriteData( (tpa110_DataPtr) buffer,
                                                 strLenOrIndi,
                                                 *comprErr );
        sqlRetcode = pa111_ConvComprRet( comprRet );
    }; // else

    return sqlRetcode;
}; // PutData


SQLRETURN tpa111_ODCompr2::PutBlock( const SQLPOINTER  buffer,
                                     SQLINTEGER        strLenOrIndi,
                                     tsp00_Int4       *comprErr )
{
    tpa110_ComprRet comprRet = COMPR_SUCCESS_PA110;
    SQLRETURN       sqlRetcode = SQL_SUCCESS;

    if (strLenOrIndi == SQL_NULL_DATA) {
        // don't use comprFilter
        sqlRetcode = this->odbcOutStream->PutData( buffer, strLenOrIndi );
    } else {
        comprRet = this->comprFilter->WriteBlock( (tpa110_DataPtr) buffer,
                                                  strLenOrIndi,
                                                  *comprErr );
        sqlRetcode = pa111_ConvComprRet( comprRet );
    }; // else

    return sqlRetcode;
}; // PutBlock


tsp00_Bool tpa111_ODCompr2::CheckHeader( const SQLPOINTER  buffer,
                                         tpa110_ComprType *comprType,
                                         tsp00_Int4       *length,
                                         tsp00_Int4       *comprErr )
{
    tpa110_ComprRet comprRet = COMPR_SUCCESS_PA110;

    comprRet = this->comprFilter->CheckHeader( (tpa110_DataPtr) buffer,
                                               *comprType,
                                               *length,
                                               *comprErr );

    return (comprRet == COMPR_SUCCESS_PA110);
}; // CheckHeader

/*!**********************************************************************

  EndClass: ODCompr2

************************************************************************/
/*!**********************************************************************

  Class: ODDecompr2

************************************************************************/

tpa111_ODDecompr2::tpa111_ODDecompr2()
    : odbcInStream( NULL ), uncomprFilter( NULL ), isNullValue(false)
{
    this->odbcInStream  = new tpa111_ODBCInStream();
    this->uncomprFilter = new tpa110_UncomprFilter();
}; // ODDecompr2


tpa111_ODDecompr2::~tpa111_ODDecompr2()
{
    delete this->odbcInStream;
    delete this->uncomprFilter;
}; // ~ODDecompr2


void tpa111_ODDecompr2::InitDecompr( SQLHSTMT      hstmt,
                                     SQLUSMALLINT  colNo )
{
    tsp00_Int4  comprErr = 0;

    this->odbcInStream->Init( hstmt, colNo );
    this->isNullValue = this->odbcInStream->IsNullValue();
    if (!this->isNullValue) {
        this->uncomprFilter->Init( this->odbcInStream, comprErr );
    };
}; // InitDecompr


SQLRETURN tpa111_ODDecompr2::GetData( SQLPOINTER  buffer,
                                      SQLLEN      bufferLen,
                                      SQLLEN     *strLenOrIndiPtr,
                                      tsp00_Int4 *comprErr )
{
    tpa110_ComprRet comprRet = COMPR_SUCCESS_PA110;

    if (this->isNullValue) {
        // the NULL value is not compressed
        *strLenOrIndiPtr = SQL_NULL_DATA;
    } else {
        // read data and uncompress
        tsp00_Int4 bytesRead = 0;
        tsp00_Int4 bufferLen_in = (tsp00_Int4) bufferLen;

        comprRet = this->uncomprFilter->ReadData( (tpa110_DataPtr) buffer,
                                                  bufferLen_in,
                                                  bytesRead,
                                                  *comprErr );
        *strLenOrIndiPtr = (SQLLEN) bytesRead;
    }; // else

    return pa111_ConvComprRet( comprRet );
}; // GetData


tsp00_Bool tpa111_ODDecompr2::SetGetDataPos( tsp00_Int4 pos )
{
    tpa110_ComprRet comprRet = this->uncomprFilter->SetReadDataPos( pos );

    return ( comprRet == COMPR_SUCCESS_PA110);
}; // SetGetDataPos



tsp00_Bool tpa111_ODDecompr2::CheckHeader( const SQLPOINTER  buffer,
                                           tpa110_ComprType *comprType,
                                           tsp00_Int4       *length,
                                           tsp00_Int4       *comprErr )
{
    tpa110_ComprRet comprRet = COMPR_SUCCESS_PA110;

    comprRet = this->uncomprFilter->CheckHeader( (tpa110_DataPtr) buffer,
                                                 *comprType,
                                                 *length,
                                                 *comprErr );

    return (comprRet == COMPR_SUCCESS_PA110);
}; // CheckHeader

/*!**********************************************************************

  EndClass: ODDecompr2

************************************************************************/

