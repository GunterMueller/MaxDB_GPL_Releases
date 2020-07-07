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
#ifndef PA102ODCOMPRC_H
#define PA102ODCOMPRC_H
/*!**********************************************************************

  module: hpa102odcompr.h

  -----------------------------------------------------------------------

  responsible:  

  special area: ODBC

  description:  C wrapper for ODBC extension to compress/decompress data

  see also: odcompr.h for specification

  -----------------------------------------------------------------------

  copyright:             Copyright (c) 1999-2005 SAP AG

************************************************************************/

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/
#include <WINDOWS.H>
#include <sql.h>
#include <sqlext.h>

#include "gsp00.h"

#if defined(__cplusplus)
extern "C" {
#endif

/*!**********************************************************************

  Chapter: Overview

  description: .
  
  The two types ODCompr and ODDecompr can be used to compress and
  decompress Data at the client. The functions for ODCompr are a
  replacement for the ODBC function SQLPutData and pa102GetData replaces
  SQLGetData. This makes the compression transparent to the application.

  The classes use the LZH-algorithm from the SAP car-tool.
*/  
/*!
  EndChapter: Overview

************************************************************************/
    
/*!**********************************************************************

  Chapter: tpa102_ODCompr

  description: Used to compress data before inserting with SQLPutData.
  
************************************************************************/

    typedef struct st_odcompr *tpa102_ODCompr;

    /*!******************************************************************
      Function: pa102AllocCompr
      
      description: Allocate memory for compress-object.

      arguments:
        compr         [out]  Pointer to compress-object
        maxBlockSize  [out]  Max. uncompressed size of one block for block
                             wise compresseion. Use SLONG_MAX if data
                             should be compressed in one block.
                             
      return value: true for success else false */    

    tsp00_Bool pa102AllocCompr( tpa102_ODCompr *compr,
                                tsp00_Int4      maxBlockSize );

    
    /*!******************************************************************
      Function: pa102FreeCompr
      
      description: Free memory for compress-object.

      arguments:
        compr  [in]  compress-object
                            
      return value: void */
    
    void pa102FreeCompr( tpa102_ODCompr compr );

    
    /*!******************************************************************
      Function: pa102InitCompr
      
      description: Initialize compress-object.

      arguments:
        compr         [in]  compress-object
        hstmt         [in]  ODBC Statement handle to use.
        totalDataSize [in]  The total number of bytes that should be
                            compressed for this column.
        comprErr      [OUT] Contains error-code if function returns
                            SQL_ERROR.
                            
      return value: SQL_SUCCESS, SQL_ERROR */
    
    SQLRETURN pa102InitCompr( tpa102_ODCompr  compr,
                              SQLHSTMT        hstmt,
                              tsp00_Int4      totalDataSize,
                              tsp00_Int4     *comprErr );

    
    /*!******************************************************************
      Function: pa102CompressBuffer
      
      description: Compresses the buffer without sending data.

      The compressed data is stored in an internal buffer. The function
      compresses as much data as fits into the internal buffer. To send
      the data to the DB you must call FlushBuffer.
    
      arguments:
        compr                 [in]  compress-object
        buffer                [in]  Uncompressed data.
        bufferLen             [in]  Length of data in buffer.
        bytesReadPtr          [out] The bytes read from buffer. This may
                                    be greater than the number bytes that
                                    are already compressed because the LZH
                                    algorithm may keep some bytes in its
                                    internal buffer.
        bytesCompressedPtr    [out] The compressed size of the bytes that
                                    are already compressed.
        allBytesCompressedPtr [out] true if the whole buffer was
                                    compressed, else false. Only if this
                                    is true the number of bytes returned
                                    in bytesCompressedPtr is the compressed
                                    size of bufferLen. In this case
                                    bufferLen == *bytesReadPtr.
                                    
        comprErr              [out] Return code from compress function.
        
      return value:
      <UL>
      <LI> SQL_SUCCESS: Everything is OK.
      <LI> SQL_ERROR: Compression failed. comprErr contains the error-code
                      as defined in CSObject.h.
                      </UL> */
      
    SQLRETURN pa102CompressBuffer( tpa102_ODCompr  compr,
                                   SQLPOINTER      buffer,
                                   SQLINTEGER      bufferLen,
                                   tsp00_Int4     *bytesReadPtr,
                                   tsp00_Int4     *bytesCompressedPtr,
                                   tsp00_Bool     *allBytesCompressedPtr,
                                   tsp00_Int4     *comprErr );

    
    /*!******************************************************************
      Function: pa102FlushBuffer
      
      description: The data compressed by a previous call of
                   CompressBuffer is send with SQLPutData.

      arguments:
        compr   [in]  compress-object
      
      return value: Return code from SQLPutData. */
    
    SQLRETURN pa102FlushBuffer( tpa102_ODCompr compr );

    
    /*!******************************************************************
      Function: PutData
      
      description: Replacement for SQLPutData. The data from buffer is
                   compressed and then stored with SQLPutData.

      arguments:
        compr         [in]  compress-object
        buffer        [in]  Data to compress.
        strLenOrIndi  [in]  Length of the data in buffer or SQL_NULL_DATA
        comprErr      [out] Error-/warning- code for compression.
        
      return value: <UL>
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
    
    SQLRETURN pa102PutData( tpa102_ODCompr  compr,
                            SQLPOINTER      buffer,
                            SQLINTEGER      strLenOrIndi,
                            tsp00_Int4     *comprErr );

    
    /*!******************************************************************
      Function: GetLen
      
      description: Get the uncompressed length for a compressed buffer.

      arguments:
        compr         [in]  compress-object
        buffer        [in]  Pointer to compressed data.

      return-value: The uncompressed length in bytes of the buffer.
    */
    tsp00_Int4 pa102GetLen( tpa102_ODCompr compr, unsigned char *buffer );

/*!**********************************************************************

  EndChapter: tpa102_ODCompr

************************************************************************/

/*!**********************************************************************

  Chapter: tpa102_ODDecompr

  description: Used to retrieve compressed data with ODBC.
  
************************************************************************/

    typedef struct st_oddecompr *tpa102_ODDecompr;

    /*!******************************************************************
      Function: pa102AllocDecompr
      
      description: Allocate memory for decompress-object.

      arguments:
        decompr  [out]  Pointer to decompress-object
                            
      return value: true for success else false */    
    
    tsp00_Bool pa102AllocDecompr( tpa102_ODDecompr *decompr );

    
    /*!******************************************************************
      Function: pa102FreeDeompr
      
      description: Free memory of a decompress-object.

      arguments:
        decompr  [in]  decompress-object
                            
      return value: void */
    
    void pa102FreeDecompr( tpa102_ODDecompr decompr );

    
    /*!******************************************************************
      Function: pa102InitDecompr
      
      description: Initialize decompress-object.

      arguments:
        decompr       [in]  decompress-object
        hstmt         [in]  ODBC Statement handle to use.
        colNo         [in]  The column number for which data should be
                            retrieved.
                            
      return value: void */
    
    void pa102InitDecompr( tpa102_ODDecompr  decompr,
                           SQLHSTMT          hstmt,
                           SQLUSMALLINT      colNo );

    
    /*!******************************************************************
      Function: pa102GetData
      
      description: Get and decompress data from column. Replacement for
                   SQLGetData.

      arguments:
        decompr         [in]  decompress-object
        buffer          [in]  Application buffer for the uncompressed data.
        bufferLen       [in]  Length of buffer.
        strLenOrIndiPtr [out] Number of bytes actually decompressed into
                              buffer (different meaning than in ODBC!).
        comprErr        [out] Error- or warning code from decompression.
        
      return value: <UL>
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
    
    SQLRETURN pa102GetData( tpa102_ODDecompr  decompr,
                            SQLPOINTER        buffer,
                            SQLINTEGER        bufferLen,
                            SQLINTEGER       *strLenOrIndiPtr,
                            tsp00_Int4       *comprErr );

/*!**********************************************************************

  EndChapter: tpa102_ODDecompr

************************************************************************/

#if defined(__cplusplus)
} // extern "C"
#endif

#endif
