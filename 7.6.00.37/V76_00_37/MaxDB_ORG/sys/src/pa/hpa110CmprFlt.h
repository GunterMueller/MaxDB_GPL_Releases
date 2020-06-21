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
#ifndef HPA110CMPRFLT_H
#define HPA110CMPRFLT_H
/*!**********************************************************************

  module: hpa110CmprFlt.h

  -----------------------------------------------------------------------

  responsible:  

  special area: Data-Compression

  description:  Filter for block wise compression of data

  see also:     

  -----------------------------------------------------------------------

  copyright:                 Copyright (c) 1999-2005 SAP AG

************************************************************************/

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include <limits.h>

#include "gsp00.h"

#include "hpa101saptype.h"
#include "hpa104CsObject.h"

typedef tsp00_Int4 tpa110_ComprRet;
typedef unsigned char *tpa110_DataPtr;

#define UNKNOWN_SIZE_PA110           -1
#define DEFAULT_MAX_BLOCK_SIZE_PA110 MAX_INT4_SP00

/* return codes */
#define COMPR_ERROR_PA110              -1 
#define COMPR_SUCCESS_PA110             0
#define COMPR_SUCCESS_WITH_INFO_PA110   1

/* values for comprError. more are in hpa104CsObject.h */
#define ERR_NO_MEMORY_PA110       -100 // memory allocation failed
#define ERR_NOT_INITIALIZED_PA110 -101 // Init was not called
#define ERR_INCONSISTENCE_PA110   -102 // data to decompress is inconsistent

#define OWN_HEADER_SIZE_PA110      8  // size in bytes of own block header
#define GENERAL_HEADER_SIZE_PA110  8  // size of general header

/*!**********************************************************************

  Chapter: error-codes

  description: .

   The following error codes are returned in the comprErr
   argument of several methods. If a method returns
   COMPR_ERROR_PA110 the error code (comprErr argument) is negative.
   Any of the methods returning tpa110_ComprRet may return one of
   the positive codes listed below. In this case the return code and
   comprErr are the same.

  <TABLE>
  <TH> constant          <TH> value <TH> description <TR>
  <TD> CS_END_INBUFFER   <TD> 3 <TD> End of input buffer <TR>
  <TD> CS_END_OUTBUFFER  <TD> 2 <TD> End of output buffer <TR>
  <TD> CS_END_OF_STREAM  <TD> 1 <TD> End of data <TR>
  <TD> CS_OK             <TD> 0 <TD> OK <TR>
  <TD> ...               <TD> -71,...,-1 <TD> internal error <TR>
  <TD> ERR_NO_MEMORY_PA110 <TD> -100 <TD> memory allocation error <TR>
  <TD> ERR_NOT_INITIALIZED_PA110 <TD> -101 <TD> ReadData or WriteData was
                                                called before Init. <TR>
  <TD> ERR_INCONSISTENCE_PA110 <TD> -102 <TD> data is corrupted <TR>
  </TABLE> */

/*!
  EndChapter: error-codes

************************************************************************/


/*!**********************************************************************

  Class: tpa110_InStream

  description: This abstract base class defines an interface for an
               input stream. This is used by tpa110_UncomprFilter to read
               compressed data.

************************************************************************/

class tpa110_InStream
{
public:
    virtual ~tpa110_InStream() {};
/*!**********************************************************************
  Function: ReadData

  description: Read data into a buffer with given length.

  arguments:
    buffer    [out]  The buffer the data is written to.
    bufSize   [in]   The size of buffer.
    bytesRead [out]  Number of bytes written to buffer.
    moreData  [out]  true if more data is available.

  return value:
    COMPR_SUCCESS_PA110 or COMPR_ERROR_PA110.
*/
    virtual tpa110_ComprRet ReadData( tpa110_DataPtr  buffer,
                                      tsp00_Int4      bufSize,
                                      tsp00_Int4     &bytesRead,
                                      tsp00_Bool     &moreData ) = 0;

/*!**********************************************************************
  Function: SkipData

  description: Skip some bytes of the input stream. The default
               implementation uses ReadData. You may overwrite this method
               to implement it more efficient.

  arguments:
    numBytes  [in]   The number of bytes to skip.
    moreData  [out]  true if more data is available.

  return value:
    COMPR_SUCCESS_PA110 or COMPR_ERROR_PA110.
*/
    virtual tpa110_ComprRet SkipData( tsp00_Int4      numBytes,
                                      tsp00_Bool     &moreData );

}; // class tpa110_InStream

/*!**********************************************************************

  EndClass: tpa110_InStream

************************************************************************/
/*!**********************************************************************

  Class: tpa110_OutStream

  description: This abstract base class defines the interface for an
               output stream. It is used by tpa110_ComprFilter to write
               compressed data.

************************************************************************/

class tpa110_OutStream
{
public:
    virtual ~tpa110_OutStream() {};
/*!**********************************************************************
  Function: WriteData

  description: Write data.

  arguments:
    buffer   [in]  The buffer that holds the data.
    numBytes [in]  The number of bytes available in buffer.

  return value: COMPR_SUCCESS_PA110 or COMPR_ERROR_PA110.
*/
    virtual tpa110_ComprRet WriteData( const tpa110_DataPtr  buffer,
                                       tsp00_Int4      numBytes ) = 0;
}; // class tpa110_OutStream

/*!**********************************************************************

  EndClass: tpa110_OutStream

************************************************************************/

class tpa110_DynBuf;

/*!**********************************************************************

  Class: tpa110_ComprFilter

  description: You use this class to compress data. The data may be
               compressed en block or block wise. If you choose block
               wise compression you can set the maximum size of a block.
               You need an outstream class derived from tpa110_OutStream
               that is used to write compressed data.

************************************************************************/

typedef enum {
    NOT_COMPRESSED_PA110,
    SINGLE_BLOCK_PA110, /* "old" single block LZH compressed */
    BLOCK_WISE_PA110    /* "new" block wise LZH compressed */
} tpa110_ComprType;            


class tpa110_ComprFilter
{
public:
/*!**********************************************************************
  Function: tpa110_ComprFilter

  description: Constructor

  arguments:
    maxBlockSize  [in]  maximum uncompressed size of a block that is
                        compressed into a single compressed block.
                        The default is LONG_MAX which means that data
                        can be compressed into one single block.
                        The actual block size may be less. For example
                        if you do not specify the totalDataSize in the
                        Init method the block size is directly determined
                        by the buffer size in each WriteData call.
  return value:
*/
    tpa110_ComprFilter( tsp00_Int4 maxBlockSize
                        = DEFAULT_MAX_BLOCK_SIZE_PA110 );

    ~tpa110_ComprFilter();


/*!**********************************************************************
  Function: Init

  description: Initialization for a new value. This has to be done before
               WriteData is called.

  arguments:
    outStream     [in]  This stream is used to write compressed data.
                        So a call of WriteData in this class results in
                        one or more calls of WriteData in outStream.
    totalDataSize [in]  If you know the total uncompressed size of your
                        data specify it here. If it is unknown use
                        UNKNOWN_SIZE_PA110. If you use UNKNOWN_SIZE_PA110
                        each call of WriteData will start a new block.
    comprErr      [out] Contains error code if function returns
                        COMPR_ERROR_PA110

  return value: COMPR_ERROR_PA110 for failure. A positive code for success.
*/
    tpa110_ComprRet Init( tpa110_OutStream  *outStream,
                          tsp00_Int4         totalDataSize,
                          tsp00_Int4        &bytesWritten,
                          tsp00_Int4        &comprErr );


    tpa110_ComprRet InitAppend( tpa110_OutStream  *outStream,
                                tsp00_Int4        &comprErr );


    tpa110_ComprRet CompressData( const tpa110_DataPtr  buffer,
                                  tsp00_Int4            numBytes,
                                  tsp00_Int4           &bytesRead,
                                  tsp00_Int4           &bytesCompressed,
                                  tsp00_Bool           &allBytesCompressed,
                                  tsp00_Int4           &comprErr );

    tpa110_ComprRet FlushData();

    /* write one block of compressed data */
    tpa110_ComprRet WriteBlock( const tpa110_DataPtr  buffer,
                                tsp00_Int4            numBytes,
                                tsp00_Int4           &comprErr );

/*!**********************************************************************
  Function: WriteData

  description: Compress and write data.

  arguments:
    buffer   [in]  Pointer to buffer that holds the data.
    numBytes [in]  The number of bytes in buffer.
    comprErr [out] Contains one of the error codes defined above.

  return value: COMPR_ERROR_PA110 for failure. A positive code for success.
*/
    tpa110_ComprRet WriteData( const tpa110_DataPtr  buffer,
                               tsp00_Int4            numBytes,
                               tsp00_Int4           &comprErr );


    long GetLen( const unsigned char *buffer ) {
        return this->csObject.CsGetLen( (unsigned char*) buffer );
    };


    /*!******************************************************************
      Function: CheckHeader
      
      description: Check compressiontype, and get length from header

      arguments:
        buffer        [IN]  Pointer to compressed data.
        comprType     [OUT] SINGLE_BLOCK_PA110 or BLOCK_WISE_PA110.
        length        [OUT] Uncompressed length (taken from LZH header)
        comprErr      [OUT] Negative error code if function fails.
        
      return-value: COMPR_ERROR_PA110 for failure. A positive code for
                    success.
    */
    tpa110_ComprRet CheckHeader( const unsigned char    *buffer,
                                 tpa110_ComprType       &comprType,
                                 tsp00_Int4             &length,
                                 tsp00_Int4             &comprErr );
    
protected:
    tpa110_OutStream *outStream;     // stream to write compressed data
    tsp00_Int4        maxBlockSize;  // max uncompressed size of one data
                                     // block for block wise compression
    CsObject          csObject;      // compression object
    tsp00_Int4        totalDataSize; // total size or UNKNOWN_SIZE_PA110
    tsp00_Bool        initialized;   // was Init called?
    tsp00_Bool        newBlock;      // do we have to start a new block?
    tpa110_DynBuf    *comprBuf;      // buffer to compress data into
    tsp00_Int4        actBlockSize;  // the calculated actual blockSize
    tpa110_DataPtr    ownHeaderPos;  // pointer to own header in comprBuf
    tsp00_Int4        totalWritten;  // uncompressed bytes written after last init
    tsp00_Int4        blockRestLen;  // size of rest data for current block
private:
    tsp00_Bool AllocComprBuf();

    void CalcActualBlockSize( tsp00_Int4    actDataSize );

    void InitAttr();

    tsp00_Bool InitLZHHeader( tsp00_Int4   &comprErr, tsp00_Int4  &bytesWritten );

    tpa110_ComprRet PutBuffer( const tpa110_DataPtr  buffer,
                               tsp00_Int4            dataSize,
                               tsp00_Int4           &comprErr );
}; // class tpa110_ComprFilter

/*!**********************************************************************

  EndClass: tpa110_ComprFilter

************************************************************************/
/*!**********************************************************************

  Class: tpa110_UncomprFilter

  description: This class is used to uncompress data that was compressed
               by tpa110_ComprFilter. The data can be read in pieces of
               any size. A read-data-position can be set to skip data.

************************************************************************/


class tpa110_UncomprFilter
{
public:
/*!**********************************************************************
  Function: tpa110_UncomprFilter

  description: Constructor

  arguments:

  return value:
*/
    tpa110_UncomprFilter();

    ~tpa110_UncomprFilter();


/*!**********************************************************************
  Function: Init

  description: Initialization for a new value. This has to be called before
               the first ReadData.

  arguments:
    inStream  [in]  This stream is used by the ReadData method to read
                    compressed data. So every call of ReadData in this
                    class results in one or more calls of ReadData in
                    inStream.
    comprErr  [out] One of the error codes defined above.

  return value: COMPR_ERROR_PA110 for failure. A positive code for success.
*/
    tpa110_ComprRet Init( tpa110_InStream  *inStream, tsp00_Int4 &comprErr );


/*!**********************************************************************
  Function: ReadData

  description: Read and uncompress data.

  arguments:
    buffer       [out]  The uncompressed data is written to this buffer.
    bufferSize   [in]   The size of buffer.
    bytesWritten [out]  The number of bytes written to buffer.
    comprErr     [out]  One of the error codes defined above.

  return value: COMPR_ERROR_PA110 for failure. A positive code for success.
*/
    tpa110_ComprRet ReadData( tpa110_DataPtr  buffer,
                              tsp00_Int4      bufferSize,
                              tsp00_Int4     &bytesWritten,
                              tsp00_Int4     &comprErr );


/************************************************************************
  Function: SetReadDataPos

  description: Set the absolute start position for the next ReadData call.
               The position can only be set forward.

  arguments:
    readDataPos  [in]  The absolute read position (in uncompressed data)
                       for next ReadData call.

  return value: COMPR_SUCCESS_PA110 or COMPR_ERROR_PA110.
*/
    tpa110_ComprRet SetReadDataPos( tsp00_Int4  readDataPos );


    
    /*!******************************************************************
      Function: CheckHeader
      
      description: Check compressiontype, and get length from header

      arguments:
        buffer        [IN]  Pointer to compressed data.
        comprType     [OUT] SINGLE_BLOCK_PA110 or BLOCK_WISE_PA110.
        length        [OUT] Uncompressed length (taken from general or
                            LZH header)
        comprErr      [OUT] Negative error code if function fails.
        
      return-value: COMPR_ERROR_PA110 for failure. A positive code for
                    success.
    */
    tpa110_ComprRet CheckHeader( const unsigned char    *buffer,
                                 tpa110_ComprType       &comprType,
                                 tsp00_Int4             &length,
                                 tsp00_Int4             &comprErr );

protected:
    CsObject          csObject;
    tpa110_DataPtr    decomprBuf;
    tsp00_Bool        initialized;
    tsp00_Bool        compressed;  // is the data compresssed ?
    tpa110_DataPtr    restPos;     // pos of rest data in decomprBuf
    tsp00_Int4        restLen;     // length of rest data in decomprBuf
    tsp00_Bool        allDataRead; // did we read the whole data from inStream
    tpa110_InStream  *inStream;    // stream to read compressed data
    tsp00_Int4        readDataPos; // Set by SetReadDataPos for positional
                                   // reading
    tsp00_Int4        totalRead;   // total bytes read from inStream for
                                   // the current block including the headers
    tsp00_Int4        actBlockLen; // total length of actual block including
                                   // the headers
    tsp00_Int4        uncompressedLen; // length of actual block from LZH header
    tpa110_ComprType  comprType;       // is determined in Init method
    tsp00_Int4        totalUncomprLen; // if available in general header
private:
    tsp00_Bool AllocDecomprBuf();

    tpa110_ComprRet DecomprBuf( unsigned char *inBuf,
                                tsp00_Int4     inLen,
                                tpa110_DataPtr outBuf,
                                tsp00_Int4     outLen,
                                tsp00_Int4    &bytesReadPtr,
                                tsp00_Int4    &bytesDecompressedPtr,
                                tsp00_Int4    &comprErr );

    tpa110_ComprRet GetCompressedData( tpa110_DataPtr  buffer,
                                       tsp00_Int4      bufferLen,
                                       tsp00_Int4     &dataLen,
                                       tsp00_Int4     &comprErr );

    tpa110_ComprRet GetUncompressedData( tpa110_DataPtr  buffer,
                                         tsp00_Int4      bufferLen,
                                         tsp00_Int4     &dataLen,
                                         tsp00_Int4     &comprErr );

    // read data from instream into decomprBuf and set restLen
    tpa110_ComprRet ReadFromStream( tsp00_Int4   numBytes,
                                    tsp00_Int4  &bytesRead,
                                    tsp00_Bool  &moreData );

    // read general header
    tpa110_ComprRet ReadGeneralHeader( tsp00_Int4 &comprErr );

    // read own header
    tpa110_ComprRet ReadHeader( tsp00_Int4 &comprErr );

    // read LZH header and initialize CsObject
    tpa110_ComprRet ReadLZHHeader( tsp00_Int4 &comprErr,
                                   tsp00_Bool &isCompressed );

    tpa110_ComprRet CheckLZHHeader( tpa110_DataPtr  buffer,
                                    tsp00_Bool     &isCompressed,
                                    tsp00_Int4     &comprErr );

    // read own + LZH header
    tpa110_ComprRet ReadBlockHeader( tsp00_Bool &moreData,
                                     tsp00_Int4 &comprErr );

    // skip numBytes of uncompressed data
    tpa110_ComprRet SkipData( int  numBytes, tsp00_Int4  &comprErr );

    // reset state for new block
    void NewBlock();

    // get free size of decomprbuf
    tsp00_Int4 FreeSize();
}; // class UncomprFilter

/*!**********************************************************************

  EndClass: tpa110_UncomprFilter

************************************************************************/

/* init general and one block-header. buffer must be at least
 * OWN_HEADER_SIZE_PA110 + GENERAL_HEADER_SIZE_PA110 big.
 * totalLen is the size of stored compressed data including
 * OWN_HEADER_SIZE_PA110 and GENERAL_HEADER_SIZE_PA110 */
void pa110SetOneBlockHeader( tpa110_DataPtr  buffer,
                             tsp00_Int4      totalLen );

/* set total uncompressed data len in general header */
void pa110SetLengthInHeader( tpa110_DataPtr  buffer,
                             tsp00_Int4      totalLen );
#endif
