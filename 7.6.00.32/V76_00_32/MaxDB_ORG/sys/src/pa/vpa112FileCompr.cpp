/*!**********************************************************************

  module: hpa110CmprFlt.h

  -----------------------------------------------------------------------

  responsible:  

  special area: Data-Compression

  description:  Classes to read and write LZH compressed files

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

#include <stdio.h>

#include "hpa112FileCompr.h"

/*!**********************************************************************

  Class: tpa112_FileOutStream

************************************************************************/

class tpa112_FileOutStream : public tpa110_OutStream
{
public:

    tpa112_FileOutStream()
        : outFile(NULL) { };

    virtual tpa110_ComprRet WriteData( const tpa110_DataPtr  buffer,
                                       tsp00_Int4            numBytes )
    {
        tsp00_Int4 written = (tsp00_Int4) fwrite( buffer, 1, numBytes, this->outFile );

        if (written == numBytes) {
            return COMPR_SUCCESS_PA110;
        } else {
            return COMPR_ERROR_PA110;
        };
    }; // WriteData


    tsp00_Bool Open( const char *fileName )
    {
        this->outFile = fopen( fileName, "wb");

        return (this->outFile != NULL);
    }; // Open


    void Close()
    {            
        if (this->outFile) {
            fclose( this->outFile );
            this->outFile = NULL;
        };
    }; // Close

    void Flush()
    {
        if (this->outFile) {
            fflush( this->outFile );
        };
    }; // Flush
private:
    FILE *outFile;
}; // class tpa112_FileOutStream

/*!**********************************************************************

  Class: tpa112_FileInStream

************************************************************************/

class tpa112_FileInStream : public tpa110_InStream
{
public:

    tpa112_FileInStream()
        : inFile(NULL) { };

    tsp00_Bool Open( const char *fileName )
    {
        this->inFile = fopen( fileName, "rb");

        return (this->inFile != NULL);
    }; // Open

    tpa110_ComprRet ReadData( tpa110_DataPtr  buffer,
                              tsp00_Int4      bufSize,
                              tsp00_Int4     &bytesRead,
                              tsp00_Bool     &moreData )
    {
        tpa110_ComprRet  comprRet = COMPR_SUCCESS_PA110;
         tsp00_Int4      read = (tsp00_Int4) fread( buffer, 1, bufSize, this->inFile );

         if (read > 0) {
             // success
             bytesRead = read;
             moreData = (bytesRead >= bufSize);
         } else if (feof(this->inFile)) {
             // end of file
             moreData = false;
         } else {
             // read error
             comprRet = COMPR_ERROR_PA110;
         }; // else

         return comprRet;
    }; // ReadData

    void Close()
    {            
        if (this->inFile) {
            fclose( this->inFile );
            this->inFile = NULL;
        };
    }; // Close

private:
    FILE *inFile;
}; // class tpa112_FileOutStream

/*!**********************************************************************

  Class: tpa112_FileWriter

************************************************************************/

tpa112_FileWriter::tpa112_FileWriter( tsp00_Int4 maxBlockSize )
    : comprFilter( maxBlockSize ), buffer(NULL), bufLen(0), bufPos(0),
      outStream(NULL)
{
    this->buffer = new unsigned char[maxBlockSize];
    if (this->buffer) {
        this->bufLen = maxBlockSize;
    };
    this->outStream = new tpa112_FileOutStream();
}; // tpa112_FileWriter


tpa112_FileWriter::~tpa112_FileWriter()
{
    delete[] this->buffer;
    delete this->outStream;
}; // ~tpa112_FileWriter


tsp00_Bool tpa112_FileWriter::Open( const char *fileName )
{
    tpa110_ComprRet comprRet = COMPR_SUCCESS_PA110;
    tsp00_Int4      comprErr = 0;
    tsp00_Int4      written  = 0;
    tsp00_Bool      ok = true;

    if (this->outStream->Open( fileName )) {
        comprRet = this->comprFilter.Init( this->outStream,
                                           UNKNOWN_SIZE_PA110,
                                           written,
                                           comprErr );
        ok = (comprRet == COMPR_SUCCESS_PA110);
    } else {
        ok = false;
    };

    return ok;
}; // Open


tsp00_Bool tpa112_FileWriter::Close()
{
    this->Flush();
    this->outStream->Close();

    return true;
}; // Close


tsp00_Bool tpa112_FileWriter::Write( char *data, tsp00_Int4 len )
{
    tsp00_Int4      restLen = (tsp00_Int4) ((len == LEN_NTS_PA112) ? strlen(data) : len);
    tsp00_Int4      copyLen = MIN( this->bufLen - this->bufPos, restLen );
    char           *dataPos = data;
    tpa110_ComprRet comprRet = COMPR_SUCCESS_PA110;
    tsp00_Int4      comprErr = 0;

    while (copyLen > 0) {
        memcpy( &this->buffer[this->bufPos], dataPos, copyLen );
        this->bufPos += copyLen;
        restLen -= copyLen;
        if (this->bufPos >= this->bufLen) {
            // compress buffer
            comprRet = this->comprFilter.WriteData( this->buffer,
                                                    this->bufPos,
                                                    comprErr );
            this->bufPos = 0;
            dataPos += copyLen;
        }; // if
        copyLen = MIN( this->bufLen - this->bufPos, restLen );
    }; // while

    return (comprRet >= 0);
}; // Write


tsp00_Bool tpa112_FileWriter::Flush()
{
    tpa110_ComprRet comprRet = COMPR_SUCCESS_PA110;
    tsp00_Int4      comprErr = 0;

    if (this->bufPos > 0) {
        comprRet = this->comprFilter.WriteData( this->buffer,
                                                this->bufPos,
                                                comprErr );
        this->bufPos = 0;
    }; // if
    this->outStream->Flush();

    return (comprRet >= 0);
}; // Flush

/*!**********************************************************************

  Class: tpa112_FileReader

************************************************************************/

tpa112_FileReader::tpa112_FileReader()
    : uncomprFilter(), inStream(NULL)
{
    this->inStream = new tpa112_FileInStream();
}; // tpa112_FileReader


tpa112_FileReader::~tpa112_FileReader()
{
    delete this->inStream;
}; // ~tpa112_FileReader


tsp00_Bool tpa112_FileReader::Open( const char *fileName )
{
    if (this->inStream->Open( fileName )) {
        tsp00_Int4      comprErr = 0;
        tpa110_ComprRet comprRet = this->uncomprFilter.Init( this->inStream,
                                                             comprErr );

        return (comprRet >= 0);
    } else {
        return false;
    };
}; // Open


tsp00_Bool tpa112_FileReader::Close()
{
    this->inStream->Close();

    return true;
}; // Close


tsp00_Bool tpa112_FileReader::Read( tpa110_DataPtr  buffer,
                                    tsp00_Int4      buffLen,
                                    tsp00_Int4     &bytesRead,
                                    tsp00_Bool     &moreData )
{
    tpa110_ComprRet comprRet = COMPR_SUCCESS_PA110;
    tsp00_Int4      comprErr = 0;

    comprRet = this->uncomprFilter.ReadData( buffer,
                                             buffLen,
                                             bytesRead,
                                             comprErr );
    moreData = (comprRet >= 0
                && bytesRead == buffLen);

    return (comprRet >= COMPR_SUCCESS_PA110);
}; // Read

