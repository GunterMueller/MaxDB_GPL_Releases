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
#ifndef HPA111ODCOMPR2_H
#define HPA111ODCOMPR2_H
/*!**********************************************************************

  module: hpa111ODCompr2.h

  -----------------------------------------------------------------------

  responsible:  

  special area: ODBC

  description:  ODBC extension to compress/decompress data

  see also:

  -----------------------------------------------------------------------

  copyright:               Copyright (c) 1999-2005 SAP AG

************************************************************************/

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/
#include <WINDOWS.H>
#include "vpa00sql.h"
#include "vpa00sqlext.h"

#include "hpa110CmprFlt.h"

/*!**********************************************************************

  Chapter: Overview

  description: .

  The two classes tpa111_ODCompr2 and tpa111_ODDecompr2 can be used
  to compress and decompress Data at the client. The methods of
  tpa111_ODCompr2 are a replacement
  for the ODBC function SQLPutData and tpa111_ODDecompr2::GetData replaces
  SQLGetData. This makes the compression transparent to the application.

  The classes use the LZH-algorithm from the SAP car-tool. */
/*!
  EndChapter: Overview

************************************************************************/

class tpa111_ODBCOutStream;

/*!**********************************************************************

  Class: tpa111_ODCompr2

  description: Used to compress data before inserting with SQLPutData.
  
************************************************************************/

class tpa111_ODCompr2 {
public:
    // constructor
    tpa111_ODCompr2( tsp00_Int4  maxBlockSize );
    // desctructor
    ~tpa111_ODCompr2();

    
    /*!******************************************************************
      Function: InitCompr
      
      description: Initialize compress-object.

      arguments:
        hstmt         [IN]  ODBC Statement handle to use.
        totalDataSize [IN]  The total number of bytes that should be
                            compressed for this column.
        comprErr      [OUT] Contains error-code if function returns
                            SQL_ERROR.
                            
      return value: SQL_SUCCESS, SQL_ERROR */
    
    SQLRETURN InitCompr( SQLHSTMT    hstmt,
                         tsp00_Int4  totalDataSize,
                         tsp00_Int4 *bytesWritten,
                         tsp00_Int4 *comprErr );

    SQLRETURN InitComprAppend( SQLHSTMT    hstmt,
                               tsp00_Int4 *comprErr );
    
    /*!******************************************************************
      Function: CompressBuffer
      
      description: Compresses the buffer without sending data.

      arguments:
        buffer                [IN]  Uncompressed data.
        bufferLen             [IN]  Length of data in buffer.
        bytesReadPtr          [OUT] The bytes read from buffer. This may
                                    be greater than the number bytes that
                                    are already compressed because the LZH
                                    algorithm may keep some bytes in its
                                    internal buffer.
        bytesCompressedPtr    [OUT] The compressed size of the bytes that
                                    are already compressed.
        allBytesCompressedPtr [OUT] true if the whole buffer was
                                    compressed, else false. Only if this
                                    is true the number of bytes returned
                                    in bytesCompressedPtr is the compressed
                                    size of bufferLen. In this case
                                    bufferLen == *bytesReadPtr.
                                    
        comprErr              [OUT] Return code from compress function.
        
      return value: <UL>
      <LI> SQL_SUCCESS Everything is OK.
      <LI> SQL_ERROR Compression failed. comprErr contains the error-code
                      as defined in CSObject.h.
      </UL>      
     The compressed data is stored in an internal buffer. The function
     compresses as much data as fits into the internal buffer. To send
     the data to the DB you must call FlushBuffer. */

    SQLRETURN CompressBuffer( const SQLPOINTER  buffer,
                              SQLINTEGER        bufferLen,
                              tsp00_Int4       *bytesReadPtr,
                              tsp00_Int4       *bytesCompressedPtr,
                              tsp00_Bool       *allBytesCompressedPtr,
                              tsp00_Int4       *comprErr );


    /*!******************************************************************
      Function: FlushBuffer
      
      description: The data compressed by a previously call of
                   CompressBuffer is send with SQLPutData.

      arguments: none
      
      return value: Return code from SQLPutData. */
    
    SQLRETURN FlushBuffer();
    
    /*!******************************************************************
      Function: PutData
      
      description: Replacement for SQLPutData. The data from buffer is
                   compressed and then stored with SQLPutData.

      arguments:
        buffer        [IN]  Data to compress.
        strLenOrIndi  [IN]  Length of the data in buffer or SQL_NULL_DATA
        comprErr      [OUT] Error-/warning- code for compression.
        
      return value:
      <UL>
      <LI> SQL_ERROR: SQLPutData or the compression failed. If compression
           failed comprErr contains the a negative error code as defined
           in CSObject.h or ERR_NOT_INITIALIZED or ERR_NO_MEMORY.
           If SQLPutData failed comprErr contains CS_OK
           or a positive warning code as defined in CSObject.h.
      <LI> SQL_SUCCESS: Compression and SQLPutData were successfull.
           comprErr may contain a positive warning code:
           <UL>
           <LI> CS_END_INBUFFER: The total number of bytes (totalDataSize
                                 argument of InitCompr) hasn't yet been
                                 sent.
           <LI> CS_END_OF_STREAM: The total number of bytes has been sent.
                                  This column is finished.
           </UL>
      <LI> Any other possible return-code from SQLPutData.
      </UL>*/
    
    SQLRETURN PutData( const SQLPOINTER  buffer,
                       SQLINTEGER        strLenOrIndi,
                       tsp00_Int4       *comprErr );

    SQLRETURN PutBlock( const SQLPOINTER  buffer,
                        SQLINTEGER        strLenOrIndi,
                        tsp00_Int4       *comprErr );

    /*!******************************************************************
      Function: GetComprBuf
      
      description: Get pointer to compressed data.

      arguments:

      return-value: Pointer to compressed data that was compressed by a
                    call to CompressBuffer.
    */
    // unsigned char *GetComprBuf() { return this->comprBuf; };
    
    /*!******************************************************************
      Function: GetLen
      
      description: Get the uncompressed length for a compressed buffer.

      arguments:
        buffer        [IN]  Pointer to compressed data.

      return-value: The uncompressed length in bytes of the buffer.
    */
    long GetLen( const unsigned char *buffer )
        {
            return this->comprFilter->GetLen( buffer );
        };

    /*!******************************************************************
      Function: CheckHeader
      
      description: Check compressiontype, and get length from header

      arguments:
        buffer        [IN]  Pointer to compressed data.
        comprType     [OUT] SINGLE_BLOCK_PA110 or BLOCK_WISE_PA110.
        length        [OUT] Uncompressed length (taken from LZH header)
        comprErr      [OUT] Negative error code if function fails.
        
      return-value: true for success false for failure.
    */
     tsp00_Bool CheckHeader( const SQLPOINTER  buffer,
                             tpa110_ComprType *comprType,
                             tsp00_Int4       *length,
                             tsp00_Int4       *comprErr );
    
protected:
    tpa111_ODBCOutStream *odbcOutStream;
    tpa110_ComprFilter   *comprFilter;
}; /* class tpa111_ODCompr2 */

/*!**********************************************************************

  EndClass: tpa111_ODCompr2

************************************************************************/

class tpa111_ODBCInStream;

/*!**********************************************************************

  Class: tpa111_ODDecompr2

  description: Used to retrieve compressed data with ODBC.
  
************************************************************************/

class tpa111_ODDecompr2 {
public:
    tpa111_ODDecompr2();
    ~tpa111_ODDecompr2();

    /*!******************************************************************
      Function: InitCompr
      
      description: Initialize decompress-object.

      arguments:
        hstmt         [IN]  ODBC Statement handle to use.
        colNo         [IN]  The column number for which data should be
                            retrieved.
                            
      return value: void */
    
    void InitDecompr( SQLHSTMT      hstmt,
                      SQLUSMALLINT  colNo );

    
    /*!******************************************************************
      Function: GetData
      
      description: Get and decompress data from column. Replacement for
                   SQLGetData.

      arguments:
        buffer          [IN]  Application buffer for the uncompressed data.
        bufferLen       [IN]  Length of buffer.
        strLenOrIndiPtr [OUT] Number of bytes actually decompressed into
                              buffer (different meaning than in ODBC!).
        comprErr        [OUT] Error- or warning code from decompression.
        
      return value:
      <UL>
      <LI> SQL_ERROR: SQLGetData or the decompression failed. If
           decompression failed comprErr contains the a negative error
           code as defined in CSObject.h or ERR_NOT_INITIALIZED or
           ERR_NO_MEMORY. If SQLPutData failed comprErr
           contains CS_OK or a positive warning code as defined in
           CSObject.h.
      <LI> SQL_SUCCESS: Decompression and SQLPutData were successfull.
           comprErr may contain a positive warning code:
           <UL>
           <LI> CS_END_OUTBUFFER: buffer was too small to hold the whole
                                  uncompressed data. Further calls to
                                  GetData should follow.
           <LI> CS_END_OF_STREAM: The total number of bytes has been read.
                                  This column is finished.
           </UL>
      <LI> Any other possible return-code from SQLGetData.
      </UL>*/
    
    SQLRETURN GetData( SQLPOINTER  buffer,
                       SQLLEN      bufferLen,        /* PTS 1119292 */
                       SQLLEN     *strLenOrIndiPtr,
                       tsp00_Int4 *comprErr );

    /*!******************************************************************
      Function: SetGetDataPos
      
      description: Set start position (at uncompressed data) for next
                   reading with GetData. The position can only be set
                   once after InitDecompr and before the first GetData
                   call.

      arguments:
        pos         [IN]  The position (in bytes) where the next GetData
                          should start reading. If the uncompressed size
                          is less than pos the next GetData will return
                          SQL_NO_DATA_FOUND.
                            
      return value: false if InitDecompr hasn't been called or if GetData
                    was already called after the last InitDecompr.
                    true for success. */
    
    tsp00_Bool SetGetDataPos( tsp00_Int4 pos );


    /*!******************************************************************
      Function: CheckHeader
      
      description: Check compressiontype, and get length from header

      arguments:
        buffer        [IN]  Pointer to compressed data.
        comprType     [OUT] SINGLE_BLOCK_PA110 or BLOCK_WISE_PA110.
        length        [OUT] Uncompressed length (taken from LZH header)
        comprErr      [OUT] Negative error code if function fails.
        
      return-value: true for success false for failure.
    */
     tsp00_Bool CheckHeader( const SQLPOINTER  buffer,
                             tpa110_ComprType *comprType,
                             tsp00_Int4       *length,
                             tsp00_Int4       *comprErr );
    
protected:
    tpa111_ODBCInStream   *odbcInStream;
    tpa110_UncomprFilter  *uncomprFilter;
    tsp00_Bool             isNullValue;
}; /* class tpa111_ODDecompr2 */

/*!**********************************************************************

  EndClass: tpa111_ODDecompr2

************************************************************************/

#endif
