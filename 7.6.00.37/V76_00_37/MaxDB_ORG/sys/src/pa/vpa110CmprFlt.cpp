/*!**********************************************************************

  module: hpa110CmprFlt.h

  -----------------------------------------------------------------------

  responsible:  

  special area: Data-Compression

  description:  Filter for block wise compression of data

  see also:     

  -----------------------------------------------------------------------

  copyright:                 Copyright (c) 1999-2005 SAP AG




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

#include <string.h>

#include "hpa110CmprFlt.h"


/*!**********************************************************************

  Chapter: prototypes for local functions

************************************************************************/

#define COMPR_ALGORITHM CS_LZH // always use the LZH algorithm not LZC

/* set up 8 bytes header */
tpa110_ComprRet pa110_InitHeader (tsp00_Uint1 *outbuf,
                                  tsp00_Int4   len,
                                  tsp00_Uint1  type,
                                  tsp00_Uint1  special );

tpa110_ComprRet pa110_InitGeneralHeader (tsp00_Uint1 *outbuf, tsp00_Int4 len );

/* read header info */
tpa110_ComprRet pa110_GetHeader (const tsp00_Uint1  *inbuf,
                                 tsp00_Int4         &len,
                                 tsp00_Uint1        &type,
                                 tsp00_Uint1        &special );


/* check magic code in general header */
tpa110_ComprRet pa110_CheckGeneralHeader (const tsp00_Uint1  *inbuf,
                                          tsp00_Int4         &length );


/* helper function */
tpa110_ComprRet pa110_MyMemCopy( tpa110_DataPtr  buffer,
                                 tsp00_Int4      bufferLen,
                                 tpa110_DataPtr *sourceBufPtr,
                                 tsp00_Int4     *sourceLen );


tpa110_ComprRet pa110_CheckHeader( CsObject               &csObject,
                                   const unsigned char    *buffer,
                                   tpa110_ComprType       &comprType,
                                   tsp00_Int4             &length,
                                   tsp00_Int4             &comprErr );

/*!**********************************************************************

  Class: tpa110_InStream

************************************************************************/

tpa110_ComprRet tpa110_InStream::SkipData( tsp00_Int4      numBytes,
                                           tsp00_Bool     &moreData )
{
    tpa110_ComprRet comprRet = COMPR_SUCCESS_PA110;
    unsigned char  *garbageBuf = new unsigned char[numBytes];
    tsp00_Int4      bytesRead = 0;
    
    if (garbageBuf) {
        comprRet = this->ReadData( garbageBuf,
                                   numBytes,
                                   bytesRead,
                                   moreData );
        delete[] garbageBuf;
    } else {
        comprRet = COMPR_ERROR_PA110;
    };
    
    return comprRet;
}; // SkipData

/*!**********************************************************************

  Class: tpa110_DynBuf

************************************************************************/

// min. size of a memory chunk returned by GetChunk
// a buffer should be at least big enough for our header info
#define MIN_BUF_SIZE   (OWN_HEADER_SIZE_PA110 + CS_HEAD_SIZE +1)

struct MemChunk {
    MemChunk( tsp00_Int4 chunkSize )
        : size(chunkSize), used(0), nextChunk(NULL), buf(NULL)
    {
        buf = new unsigned char[this->size];
        if (buf == NULL) {
            // new failed
            this->size = 0;
        };
    }; // MemChunk

    ~MemChunk()
    {
        if (this->buf != NULL) {
            delete[] this->buf;
        };
    }; // ~MemChunk

    tpa110_DataPtr BufPtr() { return (&this->buf[used]); };

    tsp00_Int4 FreeSpace() { return (this->size - this->used); };
    tsp00_Int4 UsedSpace() { return (this->used); };

    tpa110_DataPtr  buf;
    tsp00_Int4      size;
    tsp00_Int4      used;
    MemChunk       *nextChunk;
}; // struct MemChunk


class tpa110_DynBuf
{
public:
    tpa110_DynBuf( tsp00_Int4 initialSize, tsp00_Int4 incrSize );
    ~tpa110_DynBuf();

    tsp00_Bool GetChunk( MemChunk *&chunkPtr );
    
    tsp00_Bool GetFirstChunk( MemChunk *&chunkPtr );

    void Reset();

    tsp00_Int4 TotalSize();

    tpa110_ComprRet WriteAll( tpa110_OutStream &outStream );

private:
    tsp00_Int4  initialSize; // size of first chunk elem
    tsp00_Int4  incrSize;    // base size of following chunk elems
    MemChunk   *chunkList;   // pointer to first chunk
    MemChunk   *actChunk;    // pointer to actual chunk
}; // tpa110_DynBuf

/*!**********************************************************************

  Class: tpa110_ComprFilter

************************************************************************/

////////////////////////////////////////////////////////////////////
// constructor
////////////////////////////////////////////////////////////////////


tpa110_ComprFilter::tpa110_ComprFilter( tsp00_Int4        maxBlockSize )
    : outStream(NULL), maxBlockSize(maxBlockSize),
      csObject(), totalDataSize( 0 ),
      initialized( false ), newBlock(true), comprBuf( NULL ),
      actBlockSize( 0 ), ownHeaderPos(NULL),
      totalWritten( 0 ), blockRestLen(0)
{
}; // tpa110_ComprFilter

////////////////////////////////////////////////////////////////////
// destructor
////////////////////////////////////////////////////////////////////


tpa110_ComprFilter::~tpa110_ComprFilter()
{
    if (this->comprBuf != NULL) {
        delete this->comprBuf;
    }; /* if */
}; // ~tpa110_ComprFilter


////////////////////////////////////////////////////////////////////
// public
////////////////////////////////////////////////////////////////////

tpa110_ComprRet tpa110_ComprFilter::Init( tpa110_OutStream *out,
                                          tsp00_Int4        totalSize,
                                          tsp00_Int4       &bytesWritten,
                                          tsp00_Int4       &comprErr )
{
    tpa110_ComprRet  comprRet = COMPR_SUCCESS_PA110;
    MemChunk        *memChunk = NULL;

    if (!this->AllocComprBuf()) {
        comprRet  = COMPR_ERROR_PA110;
        comprErr = ERR_NO_MEMORY_PA110;
    } else {
        this->comprBuf->Reset();
        this->InitAttr();
        // get first chunk and reserve 8 bytes for general header
        if (this->comprBuf->GetChunk( memChunk )) {
            /* set size in general header always to UKNOWN, because when
             * data is appended we have no chance to update the header
             * with ODBC stream. */
            pa110_InitGeneralHeader( memChunk->BufPtr(),
                                     UNKNOWN_SIZE_PA110 );
            memChunk->used += GENERAL_HEADER_SIZE_PA110;
            bytesWritten    = GENERAL_HEADER_SIZE_PA110;
            this->outStream        = out;
            this->totalDataSize    = totalSize;
        } else {
            comprRet = COMPR_ERROR_PA110;
            comprErr = ERR_NO_MEMORY_PA110;
        }; // else
    }; // else

    return comprRet;
}; // Init


tpa110_ComprRet tpa110_ComprFilter::InitAppend( tpa110_OutStream *out,
                                                tsp00_Int4       &comprErr )
{
    tpa110_ComprRet  comprRet = COMPR_SUCCESS_PA110;
    MemChunk        *memChunk = NULL;

    if (!this->AllocComprBuf()) {
        comprRet  = COMPR_ERROR_PA110;
        comprErr = ERR_NO_MEMORY_PA110;
    } else {
        this->comprBuf->Reset();
        this->InitAttr();
        this->outStream = out;
    }; // else

    return comprRet;
}; // InitAppend


tpa110_ComprRet
tpa110_ComprFilter::CompressData( const tpa110_DataPtr  buffer,
                                  tsp00_Int4            numBytes,
                                  tsp00_Int4           &bytesRead,
                                  tsp00_Int4           &bytesCompressed,
                                  tsp00_Bool           &allBytesCompressed,
                                  tsp00_Int4           &comprErr )
{
    tpa110_ComprRet       retcode  = COMPR_SUCCESS_PA110;
    tsp00_Int4            comprRetcode = 0;
    MemChunk             *memChunk = NULL;
    tsp00_Int4            option   = COMPR_ALGORITHM;
    tsp00_Int4            bytesWritten = 0;

    bytesRead       = 0;
    bytesCompressed = 0;
    if (!this->initialized) {
        comprErr = ERR_NOT_INITIALIZED_PA110;
        return COMPR_ERROR_PA110;
    }; /* if */

    if (this->newBlock) {
        // get block size before starting a new block
        this->CalcActualBlockSize( numBytes );
        if (!this->InitLZHHeader( comprErr, bytesCompressed )) {
            return COMPR_ERROR_PA110;
        }; // if
        this->blockRestLen = this->actBlockSize;
    }; // if

    // the header is already in the buffer
    // fill just one chunk
    if (this->comprBuf->GetChunk( memChunk )) {
        comprRetcode = this->csObject.CsCompr( this->totalDataSize,
                                               (unsigned char*) buffer,
                                               MIN( this->blockRestLen, numBytes),
                                               memChunk->BufPtr(),
                                               memChunk->FreeSpace(),
                                               option,
                                               &bytesRead,
                                               &bytesWritten );
        if (comprRetcode < 0) {
            retcode = COMPR_ERROR_PA110;
            comprErr = comprRetcode;
        } else {
            bytesCompressed    += bytesWritten;
            this->blockRestLen -= bytesRead;
            memChunk->used     += bytesWritten;
            this->totalWritten += bytesRead;
        }; /* else */
    } else {
        retcode = COMPR_ERROR_PA110;
        comprErr  = ERR_NO_MEMORY_PA110;
    }; // else
        
    allBytesCompressed = ((comprRetcode >= 0)
                          && (comprRetcode != CS_END_OUTBUFFER)
                          && (bytesRead >= numBytes) );

    return retcode;
}; // CompressData


tpa110_ComprRet tpa110_ComprFilter::FlushData()
{
    tpa110_ComprRet  retcode = COMPR_SUCCESS_PA110;

    if (this->blockRestLen == 0) {
        // first block is finished
        // now we know compressed size. set header info for this chunk
        pa110_InitHeader( this->ownHeaderPos,
                          this->comprBuf->TotalSize(),
                          /* type */ 0, /* special */ 0 );
        // flush data
        retcode = this->comprBuf->WriteAll( *this->outStream );
        // start next block
        this->comprBuf->Reset();
        this->newBlock = true;
    };

    return retcode;
}; // FlushData


tpa110_ComprRet tpa110_ComprFilter::WriteBlock( const tpa110_DataPtr  buffer,
                                                tsp00_Int4            numBytes,
                                                tsp00_Int4           &comprErr )
{
    tpa110_ComprRet  retcode = COMPR_SUCCESS_PA110;
    MemChunk        *memChunk = NULL;

    if (!this->initialized) {
        return COMPR_ERROR_PA110;
    }; // if
    if (this->comprBuf->GetChunk( memChunk )) {
        pa110_InitHeader( memChunk->BufPtr(),
                          numBytes,
                          /* type */ 0, /* special */ 0 );
        memChunk->used += OWN_HEADER_SIZE_PA110;
        // flush block header
        retcode = this->comprBuf->WriteAll( *this->outStream );
        if (retcode == COMPR_SUCCESS_PA110) {
            retcode = this->outStream->WriteData( buffer, numBytes );
        };
        // start next block
        this->comprBuf->Reset();
        this->newBlock = true;
    } else {
        retcode = COMPR_ERROR_PA110;
        comprErr  = ERR_NO_MEMORY_PA110;
    };

	return retcode;
}; // WriteBlock


tpa110_ComprRet tpa110_ComprFilter::WriteData( const tpa110_DataPtr  buffer,
                                               tsp00_Int4            numBytes,
                                               tsp00_Int4           &comprErr )
{
    tpa110_ComprRet retcode = COMPR_SUCCESS_PA110;
    tsp00_Int4      bytesWritten = 0;

    if (!this->initialized) {
        return COMPR_ERROR_PA110;
    }; // if
    
    if (this->newBlock) {
        // get block size before starting a new block
        this->CalcActualBlockSize( numBytes );
        this->blockRestLen = this->actBlockSize;
        if (!this->InitLZHHeader( comprErr, bytesWritten )) {
            retcode = COMPR_ERROR_PA110;
        }; // if
    }; // if
        
    if (retcode == COMPR_SUCCESS_PA110) {
        // compress and write data
        retcode = this->PutBuffer( buffer, numBytes, comprErr );
    }; // if

    return retcode;
}; // WriteData


////////////////////////////////////////////////////////////////////
// private
////////////////////////////////////////////////////////////////////

#define DYN_BUF_INCR_PA110 2048

tsp00_Bool tpa110_ComprFilter::AllocComprBuf()
{
    if (this->comprBuf == NULL) {
        this->comprBuf = new tpa110_DynBuf( this->maxBlockSize,
                                            DYN_BUF_INCR_PA110 );
    }; // if

    return (this->comprBuf != NULL);
}; // AllocComprBuf


void tpa110_ComprFilter::CalcActualBlockSize( tsp00_Int4    actDataSize )
{
    if (this->totalDataSize == UNKNOWN_SIZE_PA110) {
        this->actBlockSize = MIN( this->maxBlockSize, actDataSize );
    } else {
        this->actBlockSize = MIN( this->maxBlockSize,
                                  this->totalDataSize - this->totalWritten );
    }; // else        
}; // CalcActualBlockSize


void tpa110_ComprFilter::InitAttr()
{
    this->outStream        = NULL;
    this->totalDataSize    = UNKNOWN_SIZE_PA110;
    this->initialized      = true;
    this->newBlock         = true;
    this->newBlock         = true;
    this->actBlockSize     = 0; // is calculated for each call of
                                // CompressBuffer or WriteData
    this->ownHeaderPos     = NULL;
    this->totalWritten     = 0;
    this->blockRestLen     = 0;
}; // InitAttr


tsp00_Bool tpa110_ComprFilter::InitLZHHeader( tsp00_Int4 &comprErr, tsp00_Int4  &bytesWritten )
{
    tsp00_Int4  comprRetcode;
    tsp00_Bool  retcode;
    
    int             option = COMPR_ALGORITHM;
    MemChunk       *memChunk = NULL;
    tpa110_DataPtr  bufPtr   = NULL;

    if (this->comprBuf->GetChunk( memChunk )) {
        // leave space for own header. This header is set up when
        // compression finished because it contains the compressed
        // size. Save pointer to own header in ownHeaderPos
        this->ownHeaderPos = memChunk->BufPtr();
        pa110_InitHeader( memChunk->BufPtr(), 0, 0, 0 );
        bytesWritten    = OWN_HEADER_SIZE_PA110;
        memChunk->used += OWN_HEADER_SIZE_PA110;
        comprRetcode = this->csObject.CsInitCompr( memChunk->BufPtr(),
                                                   this->actBlockSize,
                                                   option );
        if (comprRetcode != 0) {
            comprErr = comprRetcode;
            retcode = false;
        } else {
            // init header was successful
            memChunk->used += CS_HEAD_SIZE;
            bytesWritten   += CS_HEAD_SIZE;
            this->newBlock  = false;
            retcode         = true;
        }; // else
    } else {
        comprErr  = ERR_NO_MEMORY_PA110;
        retcode   = false;
    }; // else

    return retcode;
}; // InitLZHHeader


tpa110_ComprRet tpa110_ComprFilter::PutBuffer( const tpa110_DataPtr   buffer,
                                               tsp00_Int4             dataSize,
                                               tsp00_Int4            &comprErr )
{
    tpa110_ComprRet       retcode = COMPR_SUCCESS_PA110;
    tsp00_Int4            comprRetcode;
    tsp00_Int4            bytesRead;
    tsp00_Int4            bytesCompressed;
    tsp00_Int4            option        = COMPR_ALGORITHM;
    tpa110_DataPtr        bufPos        = (tpa110_DataPtr) buffer;
    tsp00_Bool            blockFinished = false;
    tsp00_Int4            restLen       = dataSize; // rest of buffer
    MemChunk             *memChunk      = NULL;
    tsp00_Int4            bytesWritten  = 0;

    comprErr = CS_OK;
    if (dataSize == 0) {
        // special case empty block
        pa110_InitHeader( this->ownHeaderPos,
                          this->comprBuf->TotalSize(),
                          /* type */ 0, /* special */ 0 );
        // flush data
        retcode = this->comprBuf->WriteAll( *this->outStream );
        // start next block
        this->comprBuf->Reset();
        this->newBlock = true;
    }; // if
    while (retcode == COMPR_SUCCESS_PA110 && restLen > 0) {
        blockFinished = false;
        while (retcode == COMPR_SUCCESS_PA110 && !blockFinished) {
            if (this->comprBuf->GetChunk( memChunk )) {
                comprRetcode = this->csObject.CsCompr( this->actBlockSize,
                                                       bufPos,
													   MIN( this->blockRestLen, dataSize ),
                                                       memChunk->BufPtr(),
                                                       memChunk->FreeSpace(),
                                                       option,
                                                       &bytesRead,
                                                       &bytesCompressed );
                if (comprRetcode < 0) {
                    retcode = COMPR_ERROR_PA110;
                    comprErr = comprRetcode;
                } else {
                    if (comprRetcode == CS_END_OF_STREAM) {
                        blockFinished = true;
                    } else if (comprRetcode == CS_END_OUTBUFFER) {
                        // memChunk was too small
                        blockFinished = false;
                    } else {
                        comprErr = comprRetcode; // should be CS_END_INBUFFER
                        blockFinished = true;
                    }; // else
                    memChunk->used     += bytesCompressed;
                    restLen            -= bytesRead;
                    bufPos             += bytesRead;
                    this->blockRestLen -= bytesRead;
                    this->totalWritten += bytesRead;
                }; // else
            } else {
                // memory allocation failure
                retcode = COMPR_ERROR_PA110;
                comprErr = ERR_NO_MEMORY_PA110;
            }; // else
        }; // while
        if (retcode == COMPR_SUCCESS_PA110 && (comprRetcode == CS_END_OF_STREAM)) {
            // now we know compressed size. set header info for this chunk
            pa110_InitHeader( this->ownHeaderPos,
                              this->comprBuf->TotalSize(),
                              /* type */ 0, /* special */ 0 );
            // flush data
            retcode = this->comprBuf->WriteAll( *this->outStream );
            // start next block
            this->comprBuf->Reset();
            this->newBlock = true;
            if (restLen > 0) {
                // get next block size before starting a new block
                this->CalcActualBlockSize( restLen );
                this->blockRestLen = this->actBlockSize;
                if (!this->InitLZHHeader( comprErr, bytesWritten )) {
                    return COMPR_ERROR_PA110;
                }; // if
            }; // if
        }; // if
    }; // while

    return retcode;
}; // PutBuffer


tpa110_ComprRet tpa110_ComprFilter::CheckHeader( const unsigned char *buffer,
                                                 tpa110_ComprType    &comprType,
                                                 tsp00_Int4          &length,
                                                 tsp00_Int4          &comprErr )
{
    if (pa110_CheckHeader( this->csObject,
                           buffer,
                           comprType,
                           length,
                           comprErr )) {
		return COMPR_SUCCESS_PA110;
	} else {
		return COMPR_ERROR_PA110;
	};
}; // CheckHeader

/*!**********************************************************************

  EndClass: tpa110_ComprFilter

************************************************************************/
/*!**********************************************************************

  Class: tpa110_UncomprFilter

************************************************************************/

#define DECOMPR_BUF_SIZE 64000 // size of buffer for compressed data

////////////////////////////////////////////////////////////////////
// constructor
////////////////////////////////////////////////////////////////////

tpa110_UncomprFilter::tpa110_UncomprFilter()
    : inStream(NULL), csObject(), decomprBuf( NULL ),
      initialized( false ),
      compressed( false ), restPos(NULL), restLen(0), readDataPos(0),
      allDataRead(false), totalRead(0), actBlockLen(0), uncompressedLen(0),
      comprType(BLOCK_WISE_PA110), totalUncomprLen(UNKNOWN_SIZE_PA110)
{
}; // tpa110_UncomprFilter


////////////////////////////////////////////////////////////////////
// destructor
////////////////////////////////////////////////////////////////////

tpa110_UncomprFilter::~tpa110_UncomprFilter()
{
    if (this->decomprBuf != NULL) {
        delete this->decomprBuf;
    }; // if 
}; // ~tpa110_UncomprFilter


////////////////////////////////////////////////////////////////////
// public
////////////////////////////////////////////////////////////////////

tpa110_ComprRet tpa110_UncomprFilter::Init( tpa110_InStream  *in,
                                            tsp00_Int4       &comprErr )
{
    tpa110_ComprRet comprRetcode = COMPR_SUCCESS_PA110;
    tsp00_Bool      moreData     = false;

    if (!this->AllocDecomprBuf()) {
        comprRetcode  = COMPR_ERROR_PA110;
        comprErr = ERR_NO_MEMORY_PA110;
    } else {
        this->inStream           = in;
        this->initialized        = false;
        this->compressed         = false;
        this->restPos            = &this->decomprBuf[0];
        this->restLen            = 0;
        this->readDataPos        = 0;
        this->allDataRead        = false;
        this->totalRead          = 0;
        this->uncompressedLen    = 0;
        this->comprType          = BLOCK_WISE_PA110;
        this->totalUncomprLen    = UNKNOWN_SIZE_PA110;

        // skip general header
        comprRetcode = this->ReadGeneralHeader( comprErr );
        if (comprRetcode == COMPR_SUCCESS_PA110) {
            // try to read own header from inStream
            comprRetcode = this->ReadHeader( comprErr );
            if (comprRetcode == COMPR_SUCCESS_PA110) {
                // try to read first LZH header
                comprRetcode = this->ReadLZHHeader( comprErr, this->compressed );
                if (comprRetcode == COMPR_SUCCESS_PA110) {
                    this->initialized = true;
                }; // if
            }; // if
        } else {
            // assume we have single block compressed data
            this->comprType = SINGLE_BLOCK_PA110;
            // we already tried to read the general header
            // ASSERT( GENERAL_HEADER_SIZE_PA110 >= CS_HEAD_SIZE );
            comprRetcode = this->CheckLZHHeader( this->restPos,
                                                 this->compressed,
                                                 comprErr );
            if (comprRetcode == COMPR_SUCCESS_PA110) {
                this->initialized = true;
            }; // if
        }; // else
    }; // else

    return comprRetcode;
}; // Init


tpa110_ComprRet tpa110_UncomprFilter::ReadData( tpa110_DataPtr  buffer,
                                                tsp00_Int4      bufferLen,
                                                tsp00_Int4     &dataLen,
                                                tsp00_Int4     &comprErr )
{
    tpa110_ComprRet      comprRetcode = COMPR_SUCCESS_PA110;
    tsp00_Bool           isNull = false;
    
    comprErr = CS_OK;
    if (!this->initialized) {
        comprErr = ERR_NOT_INITIALIZED_PA110;

        return COMPR_ERROR_PA110;
    }; // if
    
    if (comprRetcode == COMPR_SUCCESS_PA110
        || comprRetcode == COMPR_SUCCESS_WITH_INFO_PA110) {
        dataLen = 0;
        // skip data if necessary
        if (this->readDataPos > 1) {
            comprRetcode = this->SkipData( this->readDataPos-1, comprErr );
            this->readDataPos = 0; 
        }; // if
        if (comprRetcode == COMPR_SUCCESS_PA110
            || comprRetcode == COMPR_SUCCESS_WITH_INFO_PA110) {
            if (this->compressed) {
                comprRetcode = this->GetCompressedData( buffer, 
                                                        bufferLen,
                                                        dataLen,
                                                        comprErr );
            } else {
                comprRetcode = this->GetUncompressedData( buffer,
                                                          bufferLen,
                                                          dataLen,
                                                          comprErr );
            }; // else
        }; // if
    }; // if
    
    return comprRetcode;
}; // ReadData


tpa110_ComprRet tpa110_UncomprFilter::SetReadDataPos( tsp00_Int4 pos )
{
    // SetReadDataPos is only allowed between InitDecompr (initialized)
    // and the first GetData (decomprInitialized)
    if (!this->initialized) {
        return COMPR_ERROR_PA110;
    } else {
        this->readDataPos = pos;

        return COMPR_SUCCESS_PA110;
    }; // else
}; // SetGetDataPos


tpa110_ComprRet
tpa110_UncomprFilter::CheckHeader( const unsigned char  *buffer,
                                   tpa110_ComprType     &hdrComprType,
                                   tsp00_Int4           &length,
                                   tsp00_Int4           &comprErr )
{
    return pa110_CheckHeader( this->csObject,
                              buffer,
                              hdrComprType,
                              length,
                              comprErr );
}; // CheckHeader

////////////////////////////////////////////////////////////////////
// private
////////////////////////////////////////////////////////////////////

tsp00_Bool tpa110_UncomprFilter::AllocDecomprBuf()
{
    if (this->decomprBuf == NULL) {
        this->decomprBuf = new unsigned char[DECOMPR_BUF_SIZE];
    }; /* if */
    return (this->decomprBuf != NULL);
}; // AllocDecomprBuf


tpa110_ComprRet
tpa110_UncomprFilter::ReadFromStream( tsp00_Int4   numBytes,
                                      tsp00_Int4  &bytesRead,
                                      tsp00_Bool  &moreData )
{
    tpa110_ComprRet  retcode = COMPR_SUCCESS_PA110;

    retcode = this->inStream->ReadData( this->restPos,
                                        numBytes,
                                        bytesRead,
                                        moreData );
    this->totalRead += bytesRead;
    this->restLen   += bytesRead;

    return retcode;
}; // ReadFromStream


tpa110_ComprRet
tpa110_UncomprFilter::ReadBlockHeader( tsp00_Bool &moreData,
                                       tsp00_Int4 &comprErr )
{
    tpa110_ComprRet comprRetcode = COMPR_SUCCESS_PA110;

    if (!this->allDataRead) {
        tsp00_Bool isCompressed = true;
        
        // a new block begins, read next own & LZH header
        comprRetcode = this->ReadHeader( comprErr );
        if (comprRetcode == COMPR_SUCCESS_PA110) {
            comprRetcode = this->ReadLZHHeader( comprErr,
                                                isCompressed );
        }; // if
        if (comprRetcode == COMPR_SUCCESS_PA110
            && this->restLen > 0
            // we do not allow a mixture of compressed and
            // uncompressed blocks
            && this->compressed != isCompressed) {
            comprRetcode = COMPR_ERROR_PA110;
            comprErr = ERR_INCONSISTENCE_PA110;
            moreData = false;
        }; // if
    } else {
        moreData = false;
    }; // else

    return comprRetcode;
}; // ReadBlockHeader


tpa110_ComprRet tpa110_UncomprFilter::ReadHeader( tsp00_Int4 &comprErr )
{
    tpa110_ComprRet  retcode      = COMPR_SUCCESS_PA110;
    tsp00_Int4       bytesRead    = 0;
    tsp00_Int4       comprRetcode = 0;
    tsp00_Uint1      type         = 0;
    tsp00_Uint1      special      = 0;
    tsp00_Bool       moreData     = true;

    comprErr = CS_OK;
    // try to read own head from inStream
    retcode = this->ReadFromStream( OWN_HEADER_SIZE_PA110,
                                    bytesRead,
                                    moreData );
    this->allDataRead = (!moreData);
    if (retcode == COMPR_SUCCESS_PA110 && bytesRead >= OWN_HEADER_SIZE_PA110) {
        retcode = pa110_GetHeader( this->restPos,
                                   this->actBlockLen,
                                   type, special );
        if (retcode == COMPR_SUCCESS_PA110) {
            // skip own header
            this->restPos += OWN_HEADER_SIZE_PA110;
            this->restLen -= OWN_HEADER_SIZE_PA110;
        } else {
            comprErr = ERR_INCONSISTENCE_PA110;
        };
    }; // if

    return retcode;
}; // ReadHeader


tpa110_ComprRet
tpa110_UncomprFilter::ReadGeneralHeader( tsp00_Int4 &comprErr )
{
    tpa110_ComprRet  retcode      = COMPR_SUCCESS_PA110;
    tsp00_Int4       bytesRead    = 0;
    tsp00_Int4       comprRetcode = 0;
    tsp00_Bool       moreData     = true;

    comprErr = CS_OK;
    // try to read own head from inStream
    retcode = this->ReadFromStream( GENERAL_HEADER_SIZE_PA110,
                                    bytesRead,
                                    moreData );
    this->allDataRead = (!moreData);
    if (retcode == COMPR_SUCCESS_PA110
        && bytesRead >= GENERAL_HEADER_SIZE_PA110) {
        retcode = pa110_CheckGeneralHeader( this->restPos,
                                            this->totalUncomprLen );
        if (retcode == COMPR_SUCCESS_PA110) {
            // skip general header
            this->restPos += GENERAL_HEADER_SIZE_PA110;
            this->restLen -= GENERAL_HEADER_SIZE_PA110;
        } else {
            comprErr = ERR_INCONSISTENCE_PA110;
        };
    }; // if

    return retcode;
}; // ReadGeneralHeader


tpa110_ComprRet
tpa110_UncomprFilter::ReadLZHHeader( tsp00_Int4 &comprErr,
                                     tsp00_Bool &isCompressed )
{
    tpa110_ComprRet  retcode      = COMPR_SUCCESS_PA110;
    tsp00_Int4       bytesRead    = 0;
    tsp00_Int4       comprRetcode = 0;
    tsp00_Bool       moreData     = true;

    comprErr = CS_OK;
    // read LZH header
    retcode = this->ReadFromStream( CS_HEAD_SIZE,
                                    bytesRead,
                                    moreData );
    this->allDataRead = (!moreData);
    if (retcode == COMPR_SUCCESS_PA110 && bytesRead >= CS_HEAD_SIZE) {
        // we could read LZH header
        retcode = this->CheckLZHHeader( this->restPos, isCompressed, comprErr );
    } else if (retcode == COMPR_SUCCESS_PA110 && bytesRead < CS_HEAD_SIZE) {
        // data length is less than header size:
        // assume that data is not compressed
        isCompressed = false;
    }; // else

    return retcode;
}; // ReadLZHHeader


tpa110_ComprRet tpa110_UncomprFilter::CheckLZHHeader( tpa110_DataPtr  buffer,
                                                      tsp00_Bool     &isCompressed,
                                                      tsp00_Int4     &comprErr )
{
    tsp00_Int4       comprRetcode = 0;
    tpa110_ComprRet  retcode = COMPR_SUCCESS_PA110;

    comprRetcode = this->csObject.CsInitDecompr( this->restPos );
    switch (comprRetcode) {
    case 0: // OK
        isCompressed     = true;
        this->uncompressedLen = this->csObject.CsGetLen( this->restPos );
        this->restPos   += CS_HEAD_SIZE;
        this->restLen   -= CS_HEAD_SIZE;
        break;
    case CS_E_FILENOTCOMPRESSED:
        isCompressed = false;
        break;
    default:
        retcode = COMPR_ERROR_PA110;
        comprErr = comprRetcode;
        break;
    }; // switch

    return retcode;
}; // CheckLZHHeader


tpa110_ComprRet
tpa110_UncomprFilter::DecomprBuf( unsigned char *inBuf,
                                  tsp00_Int4     inLen,
                                  tpa110_DataPtr outBuf,
                                  tsp00_Int4     outLen,
                                  tsp00_Int4    &bytesRead,
                                  tsp00_Int4    &bytesDecompressed,
                                  tsp00_Int4    &comprErr )
{
    tsp00_Int4       lzhRetcode = 0;
    tpa110_ComprRet  retcode = COMPR_SUCCESS_PA110;
    
    lzhRetcode = this->csObject.CsDecompr( inBuf,
                                           inLen,
                                           (unsigned char *) outBuf,
                                           outLen,
                                           0, /* option = 0 */
                                           &bytesRead,
                                           &bytesDecompressed );
    if (lzhRetcode < 0) {
        retcode = COMPR_ERROR_PA110;
    } else if (lzhRetcode == CS_END_OUTBUFFER) {
        /* buffer is too small */
        retcode = COMPR_SUCCESS_WITH_INFO_PA110;
    }; /* if */
    comprErr = lzhRetcode;

    return retcode;
}; // DecomprBuf


#define MAX_LOOP_COUNT 100000 /* prevent inifinite loop */

tpa110_ComprRet
tpa110_UncomprFilter::GetCompressedData( tpa110_DataPtr  buffer,
                                         tsp00_Int4      bufferLen,
                                         tsp00_Int4     &dataLen,
                                         tsp00_Int4     &comprErr )
{
    tpa110_ComprRet  comprRetcode = COMPR_SUCCESS_PA110;
    tsp00_Int4       bytesReadFromStream = 0;  // no of bytes read from instream
    tsp00_Int4       bytesRead = 0;            // no of compressed bytes
    tsp00_Int4       bytesDecompressed = 0;    // no of uncompressed bytes
    tpa110_DataPtr   bufPos = NULL;            // actual position in buffer
    tsp00_Int4       bufLen = 0;               // bufferLen + buffer - bufPos
    tsp00_Int4       loopCount = 0;
    tsp00_Bool       moreData = true;
    
    bufPos = buffer;
    bufLen = bufferLen;
    // first copy rest data
    if (this->restLen > 0) {
        comprRetcode = this->DecomprBuf( this->restPos,
                                         this->restLen,
                                         buffer,
                                         bufferLen,
                                         bytesRead,
                                         bytesDecompressed,
                                         comprErr );
        if (comprRetcode == COMPR_SUCCESS_PA110) {
            // buffer was big enough
            // start at the beginning of uncomprBuf in next loop
            this->NewBlock();
            bufPos += bytesDecompressed;
            bufLen -= bytesDecompressed;
            if (comprErr == CS_END_OF_STREAM) {
                // this block is finished see if there are more blocks
                comprRetcode = this->ReadBlockHeader( moreData, comprErr );
            }; // if
        } else if (comprRetcode == COMPR_SUCCESS_WITH_INFO_PA110) {
            // buffer was too small (SUCCESS_WITH_INFO)
            this->restPos += bytesRead;
            this->restLen -= bytesRead;
        }; // else
        dataLen += bytesDecompressed;
    }; // if
    if ((comprRetcode == COMPR_SUCCESS_PA110)
        && !this->allDataRead
        && moreData) {
        // there is more data to decompress
        loopCount = 0;
        while (!this->allDataRead
               && moreData
               && loopCount++ < MAX_LOOP_COUNT) {
            // read data in decomprBuf and decompress into
            // the application buffer until it is full
            // or until there is no more data to read

            tsp00_Int4 numReadBytes = 0;
            if (this->comprType == BLOCK_WISE_PA110) {
                // don't read data of the next block
                numReadBytes =
                    MIN( MAX( this->actBlockLen - this->totalRead, 0 ),
                         this->FreeSize() );
            } else { // SINGLE_BLOCK_PA110
                // we do not know the total size
                numReadBytes = this->FreeSize();
            };
            if (numReadBytes == 0) {
                // block of length 0
                // start at the beginning of decomprBuf in next loop
                this->NewBlock();
                // see if there are more blocks
                comprRetcode = this->ReadBlockHeader( moreData,
                                                      comprErr );
                if (comprRetcode != COMPR_SUCCESS_PA110) {
                    // no more blocks
                    moreData = false;
                    comprRetcode = COMPR_SUCCESS_PA110;
                }; // if
            } else {
                // block has length > 0
                comprRetcode = this->ReadFromStream( numReadBytes,
                                                     bytesReadFromStream,
                                                     moreData );
                this->allDataRead = (!moreData);
                if (comprRetcode != COMPR_SUCCESS_PA110) {
                    // an error occured; stop reading
                    moreData = false;
                } else {
                    bytesRead = 0;
                    bytesDecompressed = 0;
                    comprRetcode = this->DecomprBuf( this->restPos,
                                                     this->restLen,
                                                     bufPos,
                                                     bufLen,
                                                     bytesRead,
                                                     bytesDecompressed,
                                                     comprErr );
                    dataLen += bytesDecompressed; // total decompressed data len
                    if (comprRetcode < 0) {
                        // decompression failed
                        moreData = false;
                    } else {
                        // decompression succeeded
                        if (comprErr == CS_END_OUTBUFFER) {
                            // buffer was too small (SUCCESS_WITH_INFO)
                            this->restPos += bytesRead;
                            this->restLen -= bytesRead;
                            moreData = false;
                        } else if (comprErr == CS_END_INBUFFER) {
                            // there is more data to decompress for this block
                            bufPos += bytesDecompressed;
                            bufLen -= bytesDecompressed;
                            if (bufLen == 0) {
                                // buffer is exactly filled in this loop
                                moreData = false;
                            }; // if
                            // start at the beginning of decomprBuf in next loop
                            this->restPos    = &this->decomprBuf[0];
                            this->restLen    = 0;
                            // this->NewBlock();
                        } else if (this->comprType == BLOCK_WISE_PA110) {
                            // comprErr is CS_END_OF_STREAM. Check for next block
                            // this block is finished
                            // there may be a rest of the current block because
                            // LZH may have finished before the block ends
                            if (this->totalRead < this->actBlockLen) {
                                // rest of this block is not needed
                                comprRetcode =
                                    this->inStream->SkipData( this->actBlockLen-
                                                              this->totalRead,
                                                              moreData );
                                if (comprRetcode != COMPR_SUCCESS_PA110) {
                                    break;
                                };
                            }; // if
                            // calculate pos and len of target buffer for next loop
                            bufPos += bytesDecompressed;
                            bufLen -= bytesDecompressed;
                            if (bufLen == 0) {
                                // buffer is exactly filled in this loop
                                moreData = false;
                            }; // if
                            // start at the beginning of decomprBuf in next loop
                            this->NewBlock();
                            // see if there are more blocks
                            comprRetcode = this->ReadBlockHeader( moreData,
                                                                  comprErr );
                            if (comprRetcode != COMPR_SUCCESS_PA110) {
                                // no more blocks
                                comprRetcode = COMPR_SUCCESS_PA110;
                                moreData = false;
                            }; // if
                        }  else { // SINGLE_BLOCK_PA110 && (comprErr == CS_END_OF_STREAM)
                            moreData = false;
                        }; // else
                    }; // if
                }; // if
            }; // else
        }; // while
        if (loopCount >= MAX_LOOP_COUNT) {
            comprRetcode = COMPR_ERROR_PA110;
        }; // if
    }; // if
    
    return comprRetcode;
}; // GetCompressedData


tpa110_ComprRet
tpa110_UncomprFilter::GetUncompressedData( tpa110_DataPtr  buffer,
                                           tsp00_Int4      bufferLen,
                                           tsp00_Int4     &dataLen,
                                           tsp00_Int4     &comprErr )
{
    tpa110_ComprRet comprRetcode = COMPR_SUCCESS_PA110;
#if 0
    tsp00_Int4      strLen;                   /* as returned by SQLGetData */
    
    if (this->restPos != NULL) {
        /* the bytes in decomprBuf are real data */
        strLen = this->restLen;
        comprRetcode = MyMemCopy( buffer, bufferLen,
                                  &this->restPos, &this->restLen );
        if (comprRetcode == SQL_SUCCESS) {
            *strLenOrIndi += strLen;
        } else { /* compRetcode == SQL_SUCCESS_WITH_INFO */
            /* buffer was too small */
            *strLenOrIndi += bufferLen;
        }; /* else */                
    }; /* if */

    if (comprRetcode == SQL_SUCCESS || comprRetcode == SQL_SUCCESS_WITH_INFO) {
        /* read data direct in the application buffer */
        comprRetcode = SQLGetData( hstmt,
                                   this->colNo,
                                   SQL_C_BINARY,
                                   buffer,
                                   bufferLen,
                                   &strLen );
        this->allDataRead = (comprRetcode == SQL_SUCCESS);
        if (comprRetcode == SQL_SUCCESS
            || comprRetcode == SQL_SUCCESS_WITH_INFO) {
            *strLenOrIndi += strLen;
        }; // if
    }; // if
#endif        
    return comprRetcode;
}; // GetUncompressedData


#define MAX_GARBAGE_BUF_LEN (512*1024)

// assume we are at the beginning of the first block
// Init has just been called
tpa110_ComprRet tpa110_UncomprFilter::SkipData( int numBytes, tsp00_Int4 &comprErr )
{
    tpa110_ComprRet  comprRetcode = COMPR_SUCCESS_PA110;
    tsp00_Int4       bytesSkipped = 0;
    tsp00_Bool       moreData     = true;
    
    if (this->compressed) {
        // actBlockLen, restPos and restLen are set by Init
        while (moreData
               && (comprRetcode == COMPR_SUCCESS_PA110)
               && (bytesSkipped + this->uncompressedLen) < numBytes) {
            // skip whole block
            this->inStream->SkipData( this->actBlockLen - this->totalRead, moreData );
            bytesSkipped += this->uncompressedLen;
            // start new block
            this->NewBlock();
            // see if there are more blocks
            comprRetcode = this->ReadBlockHeader( moreData,
                                                  comprErr );
        };
        if (comprRetcode == COMPR_SUCCESS_PA110 && moreData) {
            tsp00_Int4 garbageBufLen = numBytes - bytesSkipped;
            
            if (garbageBufLen > 0) {
                // uncompress data into garbage buffer until desired position
                unsigned char *garbageBuf = new unsigned char[garbageBufLen];

                if (garbageBuf) {
                    tsp00_Int4 dataLen = 0;

                    comprRetcode = this->GetCompressedData( garbageBuf,
                                                            garbageBufLen,
                                                            dataLen,
                                                            comprErr );
                    delete[] garbageBuf;
                } else {
                    // memory allocation failure
                };
            }; // if
        }; // if
    } else {
        this->inStream->SkipData( numBytes, moreData );
    };

    return comprRetcode;
}; // SkipData


void tpa110_UncomprFilter::NewBlock()
{
    this->restPos         = &this->decomprBuf[0];
    this->restLen         = 0;
    this->totalRead       = 0;
    this->uncompressedLen = 0;
}; // NewBlock


/* return COMPR_SUCCESS_WITH_INFO_PA110 if buffer is too small, else
   return COMPR_SUCCESS_PA110 */
tpa110_ComprRet pa110_MyMemCopy( tpa110_DataPtr  buffer,
                                 tsp00_Int4      bufferLen,
                                 tpa110_DataPtr *sourceBufPtr,
                                 tsp00_Int4     *sourceLen )
{
    tpa110_ComprRet retcode = (*sourceLen <= bufferLen
                               ? COMPR_SUCCESS_PA110
                               : COMPR_SUCCESS_WITH_INFO_PA110);
    tsp00_Int4 copyLen = (*sourceLen <= bufferLen ? *sourceLen : bufferLen);

    memcpy( buffer, *sourceBufPtr, copyLen );

    *sourceLen    -= copyLen;
    *sourceBufPtr += copyLen;

    return retcode;
}; /* MyMemCopy */

tsp00_Int4 tpa110_UncomprFilter::FreeSize()
{
    return (DECOMPR_BUF_SIZE -
            ((tsp00_Int4) (this->restPos - this->decomprBuf) ));
}; // FreeSize

/*!**********************************************************************

  EndClass: tpa110_UncomprFilter

************************************************************************/
/*!**********************************************************************

  Class: tpa110_DynBuf

************************************************************************/

tpa110_DynBuf::tpa110_DynBuf( tsp00_Int4 initialSize, tsp00_Int4 incrSize )
    : initialSize( initialSize ), incrSize( incrSize ),
      chunkList( NULL ), actChunk( NULL )
{
}; // tpa110_DynBuf


tpa110_DynBuf::~tpa110_DynBuf()
{
    MemChunk *nextChunk = NULL;

    this->actChunk = this->chunkList;
    while (this->actChunk != NULL) {
        nextChunk = this->actChunk->nextChunk;
        delete this->actChunk;
        this->actChunk = nextChunk;
    }; // while
}; // ~tpa110_DynBuf


tsp00_Bool tpa110_DynBuf::GetChunk( MemChunk *&chunkPtr )
{
    tsp00_Bool  getChunkOk = true;

    if ((this->actChunk == NULL) ||
        (this->actChunk->FreeSpace() < MIN_BUF_SIZE)) {
        // we have to create a new chunk
        MemChunk *newChunk = NULL;

        if (this->chunkList == NULL) {
            // first chunk
            newChunk = new MemChunk( this->initialSize );
            this->chunkList = newChunk;
        } else {
            // not the first chunk
            // ASSERT( this->actChunk != NULL);
            newChunk = new MemChunk( this->incrSize );
            this->incrSize *= 2;
            this->actChunk->nextChunk = newChunk;
        }; // else
        if (newChunk == NULL || newChunk->buf == NULL) {
            // memory allocation failure
            getChunkOk = false;
        } else {
            this->actChunk = newChunk;
            chunkPtr  = newChunk;            
        };
    } else {
        chunkPtr  = this->actChunk;
    }; // else

    return getChunkOk;
}; // GetChunk


tsp00_Bool tpa110_DynBuf::GetFirstChunk( MemChunk *&chunkPtr )
{
    chunkPtr = this->chunkList;

    return (chunkPtr != NULL);
}; // GetFirstChunk


void tpa110_DynBuf::Reset()
{
    MemChunk        *nextChunk = NULL;
    tpa110_ComprRet  retcode = COMPR_SUCCESS_PA110;

    nextChunk = this->chunkList;
    while (nextChunk != NULL) {
        nextChunk->used = 0;
        nextChunk = nextChunk->nextChunk;
    }; // while

    this->actChunk = this->chunkList;
}; // Reset


tsp00_Int4 tpa110_DynBuf::TotalSize()
{
    MemChunk        *nextChunk = NULL;
    tsp00_Int4       totalSize = 0;

    nextChunk = this->chunkList;
    while (nextChunk != NULL) {
        totalSize += nextChunk->used;
        nextChunk = nextChunk->nextChunk;
    }; // while

    return totalSize;
}; // TotalSize


tpa110_ComprRet tpa110_DynBuf::WriteAll( tpa110_OutStream &outStream )
{
    MemChunk        *nextChunk = NULL;
    tpa110_ComprRet  retcode = COMPR_SUCCESS_PA110;

    nextChunk = this->chunkList;
    while (retcode == COMPR_SUCCESS_PA110 && nextChunk != NULL) {
        retcode = outStream.WriteData( nextChunk->buf, nextChunk->used );
        nextChunk = nextChunk->nextChunk;
    }; // while

    return retcode;
}; // WriteAll

/*!**********************************************************************

  EndClass: tpa110_DynBuf

************************************************************************/
/*!**********************************************************************

  Chapter: local functions

************************************************************************/

/* magic codes that identify headers */
static const tsp00_Uint1 ComprMagicHead[] = { "\137\017" };

static const tsp00_Uint1 GeneralMagicHead[] = { "\042\011" };

tpa110_ComprRet pa110_InitHeader (tsp00_Uint1 *outbuf,
                                  tsp00_Int4   len,
                                  tsp00_Uint1  type,
                                  tsp00_Uint1  special )
{
    tsp00_Uint4 l;

    if (len < 0) return COMPR_ERROR_PA110;

    l = (tsp00_Uint4) len;

    /* set length ......................................................*/
    outbuf[0] = (tsp00_Uint1) (l & 0xff);
    outbuf[1] = (tsp00_Uint1) ((l & 0xff00) >> 8);
    outbuf[2] = (tsp00_Uint1) ((l & 0xff0000L) >> 16);
    outbuf[3] = (tsp00_Uint1) ((l & 0xff000000L) >> 24);

    /* setup header - magic number             .........................*/
    outbuf[4] = type;
    outbuf[5] = ComprMagicHead[0];
    outbuf[6] = ComprMagicHead[1];
    outbuf[7] = special;

    return COMPR_SUCCESS_PA110;
} /* pa110_InitHeader */


tpa110_ComprRet pa110_InitGeneralHeader (tsp00_Uint1 *outbuf,
                                         tsp00_Int4   len )
{
    /* set length ......................................................*/
    outbuf[0] = (tsp00_Uint1) (len & 0xff);
    outbuf[1] = (tsp00_Uint1) ((len & 0xff00) >> 8);
    outbuf[2] = (tsp00_Uint1) ((len & 0xff0000L) >> 16);
    outbuf[3] = (tsp00_Uint1) ((len & 0xff000000L) >> 24);

    outbuf[4] = 0;
    outbuf[5] = GeneralMagicHead[0];
    outbuf[6] = GeneralMagicHead[1];
    outbuf[7] = 0;

    return COMPR_SUCCESS_PA110;
} /* pa110_InitGeneralHeader */


tpa110_ComprRet pa110_GetHeader (const tsp00_Uint1  *inbuf,
                                 tsp00_Int4         &len,
                                 tsp00_Uint1        &type,
                                 tsp00_Uint1        &special )
{
    if (ComprMagicHead[0] != inbuf[5]
        || ComprMagicHead[1] != inbuf[6]) {
        return COMPR_ERROR_PA110;
    };

    len = (tsp00_Int4) inbuf[0]        +
          ((tsp00_Int4) inbuf[1] <<  8) +
          ((tsp00_Int4) inbuf[2] << 16) +
          ((tsp00_Int4) inbuf[3] << 24);
    type = inbuf[4];
    special = inbuf[7];

    return COMPR_SUCCESS_PA110;
} /* pa110_GetHeader */


tpa110_ComprRet pa110_CheckGeneralHeader( const tsp00_Uint1  *inbuf,
                                          tsp00_Int4         &length )
{
    if (GeneralMagicHead[0] != inbuf[5]
        || GeneralMagicHead[1] != inbuf[6]) {

        return COMPR_ERROR_PA110;
    } else {
        length = (tsp00_Int4) inbuf[0]    +
            ((tsp00_Int4) inbuf[1] <<  8) +
            ((tsp00_Int4) inbuf[2] << 16) +
            ((tsp00_Int4) inbuf[3] << 24);

        return COMPR_SUCCESS_PA110;
    };
} // pa110_CheckGeneralHeader


tpa110_ComprRet pa110_CheckHeader( CsObject               &csObject,
                                   const unsigned char    *buffer,
                                   tpa110_ComprType       &comprType,
                                   tsp00_Int4             &length,
                                   tsp00_Int4             &comprErr )
{
    tpa110_ComprRet  retcode = COMPR_SUCCESS_PA110;
    tsp00_Uint1      type         = 0;
    tsp00_Uint1      special      = 0;
    tsp00_Int4       blockLen     = 0;

    retcode = pa110_CheckGeneralHeader( buffer, length );
    if (retcode == COMPR_SUCCESS_PA110) {
        comprType = BLOCK_WISE_PA110;
        /* check first block-header */
        retcode = pa110_GetHeader( buffer,
                                   blockLen,
                                   type,
                                   special );
    } else {
        /* assume that data is compressed in "old" single block style */
        comprType = SINGLE_BLOCK_PA110;
        // the LZH header must be present
        length = csObject.CsGetLen( (SAP_BYTE*) buffer );
		retcode = true;
    };

    return retcode;
}; // pa110_CheckHeader

/*!**********************************************************************

  EndChapter: local functions

************************************************************************/

void pa110SetOneBlockHeader( tpa110_DataPtr  buffer,
                             tsp00_Int4      totalLen )
{
    /* set length to unknown because data may be appended and we cannot
     * update the header with ODBC stream. */
    pa110_InitGeneralHeader( buffer, UNKNOWN_SIZE_PA110 );
    pa110_InitHeader( buffer + GENERAL_HEADER_SIZE_PA110,
                      totalLen, 0, 0 );
}; // pa110SetOneBlockHeader

void pa110SetLengthInHeader( tpa110_DataPtr  buffer,
                             tsp00_Int4      totalLen )
{
    pa110_InitGeneralHeader( buffer, totalLen );
}; // pa110SetLengthInHeader

