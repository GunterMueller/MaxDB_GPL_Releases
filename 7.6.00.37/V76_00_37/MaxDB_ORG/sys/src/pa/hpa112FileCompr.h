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
#ifndef HPA112FILECOMPR_H
#define HPA112FILECOMPR_H
/*!**********************************************************************

  module: hpa110CmprFlt.h

  -----------------------------------------------------------------------

  responsible:  

  special area: Data-Compression

  description:  Classes to read and write LZH compressed files

  see also:     

  -----------------------------------------------------------------------

  copyright:                 Copyright (c) 1999-2005 SAP AG

************************************************************************/

/*
TODO:

- error handling (GetError methods for both classes)
*/

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include "hpa110CmprFlt.h"

#define LEN_NTS_PA112  (-3)

class tpa112_FileOutStream;

/*!**********************************************************************

  Class: tpa112_FileWriter

  description: This class can be used to compress and write data to a
               file. The output is buffered. It may be useful to write
               large trace-files.

************************************************************************/

class tpa112_FileWriter
{
public:
/*!**********************************************************************
  Function: tpa112_FileWriter

  description: Create new FileWriter with given blocksize.

  arguments:
    maxBlockSize  [in]  The maximum size of a compressed block. This
                        argument also determines the size of the internal
                        buffer.

  return value: void
*/
    tpa112_FileWriter( tsp00_Int4 maxBlockSize );

    ~tpa112_FileWriter();


/*!**********************************************************************
  Function: Open

  description: Open file for writing

  arguments:
    fileName  [in]  Name of file. An existing file is overwritten.

  return value: true for success, false for failure.
*/
    tsp00_Bool Open( const char *fileName );


/*!**********************************************************************
  Function: Close

  description: Flush rest data and close file.

  arguments:

  return value: true for success, false for failure.
*/
    tsp00_Bool Close();


/*!**********************************************************************
  Function: Write

  description: Compress and write data to the file.

  arguments:
    data  [in]  Pointer to buffer containing the data.
    len   [in]  Length of data in buffer. For character data LEN_NTS_PA112
                may be used to indicate that data is a null terminated
                string.

  return value: true for success, false for failure.
*/
    tsp00_Bool Write( char *data, tsp00_Int4 len = LEN_NTS_PA112 );


/*!**********************************************************************
  Function: Flush

  description: Flush all data that is in the buffers.

  arguments:

  return value: true for success, false for failure.
*/
    tsp00_Bool Flush();

private:
    tpa110_ComprFilter    comprFilter;
    unsigned char        *buffer;
    tsp00_Int4            bufLen;
    tsp00_Int4            bufPos;
    tpa112_FileOutStream *outStream;
}; // class tpa112_FileWriter

/*!**********************************************************************

  EndClass: tpa112_FileWriter

************************************************************************/

class tpa112_FileInStream;

/*!**********************************************************************

  Class: tpa112_FileReader

  description: This class is used to read compressed data written by
               tpa112_FileWriter from a file.

************************************************************************/

class tpa112_FileReader
{
public:
/*!**********************************************************************
  Function: tpa112_FileReader

  description: Create FileReader.

  arguments:

  return value:
*/
    tpa112_FileReader();
    
    ~tpa112_FileReader();

/*!**********************************************************************
  Function: Open

  description: Open compressed file for reading.

  arguments:
    fileName  [in]  Name of compressed file.

  return value: true for success, false for failure.
*/

    tsp00_Bool Open( const char *fileName );


/*!**********************************************************************
  Function: Close

  description: Close file.

  arguments:

  return value: true for success, false for failure.
*/
    tsp00_Bool Close();


/*!**********************************************************************
  Function: Read

  description: Read data from compressed file

  arguments:
    buffer     [in]  Buffer to write data to.
    buffLen    [in]  Length of buffer.
    bytesRead  [out] Number of bytes written to buffer.
    moreData   [out] true if more data is available.

  return value: true for success, false for failure.
*/
    tsp00_Bool Read( tpa110_DataPtr  buffer,
                     tsp00_Int4      buffLen,
                     tsp00_Int4     &bytesRead,
                     tsp00_Bool     &moreData );
private:
    tpa110_UncomprFilter  uncomprFilter;
    tpa112_FileInStream  *inStream;
}; // class tpa112_FileReader

/*!**********************************************************************

  EndClass: tpa112_FileReader

************************************************************************/

#endif
