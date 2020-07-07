/*! 
  -----------------------------------------------------------------------------
  module: hls30.h
  -----------------------------------------------------------------------------
 
  responsible:  SteffenS and d033893

  special area: SAP DB LOADER
 
  description:  Functions for Virtual File access
                
  version:      7.6.
  -----------------------------------------------------------------------------

    ========== licence begin  GPL
    Copyright (c) 1997-2005 SAP AG

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

#ifndef HLS30
#define HLS30

#include "gls00.h"


#if defined(_WIN32)
#define NEW_LINE_LENGTH   2
#else
#define NEW_LINE_LENGTH   1
#endif


/*! Declaration: Header structure */

typedef struct tls30_DatafileHeader
{
    tsp00_C64c dhHeaderLength_ls30;
    tsp00_C64c dhIdentifier_ls30;
    tsp00_C32c dhRecordLength_ls30;

    tsp00_Addr dhCommand_ls30;
    tsp00_C4c  dhFiller_ls30;

} tls30_DatafileHeader;

// integers will always be swapped to unix swap
typedef struct tls30_FileMetaDataHeader
{
    tsp00_Uint4              mdRecordLength_ls30;
    tsp00_Uint4              mdFieldCount_ls30;
    tsp00_Uint4              mdLongCount_ls30;
    tls00_FileFormat         mdMetaData_ls30;
    tsp00_Uint4              mdTableEmpty_ls30;
} tls30_FileMetaDataHeader;

// integers will always be swapped to unix swap
typedef struct tls30_FileMetaDataTrailer
{
    tsp00_Uint4              mdRecordLength_ls30;
    Tools_DynamicUTF8String  mdCommand_ls30;
} tls30_FileMetaDataTrailer;


/*! EndDeclaration: Header structure */

/*!  
  -----------------------------------------------------------------------------
  Chapter: Global functions
  -----------------------------------------------------------------------------
 */

/*!
  -----------------------------------------------------------------------------
 
  function:     ls30VFOpen
  -----------------------------------------------------------------------------

  description:  Open file in given (vfile structure) mode.
                Sets file position to 0 and file length to the right value.
                In case of error file position member remainder is set to EOF_LS00.

                Prior to this function ls30VFInitialize must be called.

  arguments:    Vfile    [IN]   - virtual file structure
                ErrText  [OUT]  - error text

  returnvalue:  errOK_els00       - in case of NO errors
                errFileOpen_els98 - error opening file
                
                In case of error the function logs an error message in the error
                text string AND in the log file.
  -----------------------------------------------------------------------------
*/
int
ls30VFOpen(tls00_VFile &VFile, tsp00_Addr ErrText);
 

/*!
  -----------------------------------------------------------------------------
 
  function:     ls30VFReadLn
  -----------------------------------------------------------------------------

  description:  Reads data up to newline character (files as text) or a given
                count of bytes (binary files) from the file specified
                in the VFile structure.
                In case of text file carriage return and line feed are removed
                and the line is terminated with 0.
                Is count of bytes to read set to 0 the maximum read length count
                of bytes is read.
                Reading is alwaay reading ahead!
                The space for buffer is allocated by this function and has to be
                freed by the caller.

  arguments:    Vfile           [IN]    - virtual file structure
                pszLineBuffer   [OUT]   - data buffer (space allocated here)
                lCount          [IN]    - Count of bytes to read; defaults to 0
                ErrText         [OUT]   - error text
                MaxReadLen      [IN]    - max. number of characters to read;
                                           defaults to LINELENGTH_MXSP00

  returnvalue:  Read Length     - number of bytes read
                EOF = -1        - reached end of file
                errFileRead     - error reading data from file

                In case of error the function logs an error message in the error
                text string AND in the log file.
  -----------------------------------------------------------------------------
*/
int ls30VFReadLn(tls00_VFile   &VFile,
                 tsp00_Addr    *pszLineBuffer,
                 tsp00_Addr     ErrText,
                 tsp00_Longint  lCount     = 0,
                 tsp00_Longint  MaxReadLen = LINELENGTH_MXSP00);


/*!
  -----------------------------------------------------------------------------
 
  function:     ls30VFReadLnEx
  -----------------------------------------------------------------------------

  description:  Reads data up to newline character (files as text) or a given
                count of bytes (binary files) from the file specified
                in the VFile structure.
                In case of text file carriage return and line feed are removed
                and the line is terminated with 0.
                Is count of bytes to read set to 0 the maximum read length count
                of bytes is read.
                Reading is alwaay reading ahead!
                Increases the count of read lines and adapts the file position
                in VFile structure.

  attention:    The caller must have allocated enough memory for the buffer to hold
                the read data (in case of text files the buffer must have enough
                memory to hold the trailing 0, too).
                
  arguments:    Vfile           [IN]    - virtual file structure
                pszLineBuffer   [OUT]   - data buffer
                lCount          [IN]    - size of buffer = Count of bytes to read
                ErrText         [OUT]   - error text
                MaxReadLen      [IN]    - max. number of characters to read
                                          (defaults to LINELENGTH_MXSP00)

  returnvalue:  Read Length     - number of bytes read
                EOF = -1        - reached end of file
                errFileRead     - error reading data from file

                In case of error the function logs an error message in the error
                text string AND in the log file.
  -----------------------------------------------------------------------------
*/
int ls30VFReadLnEx(tls00_VFile   &VFile,
                   tsp00_Addr     pszLineBuffer,
                   tsp00_Addr     pszErrText,
                   tsp00_Longint  lCount,
                   tsp00_Longint  MaxReadLen = LINELENGTH_MXSP00);


/*!
  -----------------------------------------------------------------------------
 
  function:     ls30VFReadLongBin
  -----------------------------------------------------------------------------

  description:  Reads requested count of bytes from a binary open file. Seeks to
                the given position.
                Returns count of read bytes.

  arguments:    pszBuffer     [OUT]  - data buffer; allocated by caller
                Vfile         [IN]   - virtual file structure
                lBytes        [IN]   - Count of bytes to read
                lReadPosition [IN]
                pszErrorText  [OUT]  - error text

  returnvalue:  Read Length  - number of bytes read
                EOF = -1     - reached end of file
                errFileRead  - error reading data from file

                In case of error the function logs an error message in the error
                text string AND in the log file.
  -----------------------------------------------------------------------------
*/
tsp00_Longint
ls30VFReadLongBin(tsp00_Addr                pszBuffer,
                  tls00_VFile              &VFile,
                  tsp00_Longint             lBytes,
                  const tls00_LargeNumber  *plReadPosition,
                  tsp00_Addr                pszErrText);
/*!
  -----------------------------------------------------------------------------
 
  function:     ls30VFReadHeader

  -----------------------------------------------------------------------------

  description:  Reads header of binary data file (for dataload and fastload).
                If no header is found (header contains a replication server id)
                an error is returned.
                If a header is found the function returns length of one line in
                file and adjusts position in vfile structure to the first byte
                of data.

  arguments:    Vfile         [IN/OUT]  - virtual file structure
                lRecordLength [OUT]     - record length
                pszErrText    [OUT]     - error text

  returnvalue:  errOK_els00              - in case of NO errors
                errNoDatafileHeader_ls98 - kein header in datendatei
                errFileRead_ls98         - error reading data from file
  -----------------------------------------------------------------------------
*/
int
ls30VFReadHeader(tls00_VFile &VFile,
                 tsp00_Int4  &lRecordLength,
                 tsp00_Addr   pszErrText);


/*!
  -----------------------------------------------------------------------------
 
  function:     ls30VFWriteLn
  -----------------------------------------------------------------------------

  description:  Write one line (text mode) or the given number of bytes (binary
                mode) to the given file.
                Increases number of written lines in vfile structure by 1 (if
                successfully written) and adapts the file length in vfile structure.

  arguments:    Vfile      [IN]  - virtual file structure
                pszBuffer  [IN]  - data to write.
                lCount     [IN]  - Count of characters to write. If negative the
                                    function uses c-function strlen to calculate
                                    the length of the string to write.
                pszErrText [OUT] - error text

  returnvalue:  errOK_els00         - in case of NO errors
                errFileWrite_els98  - error writing data to storage medium

                In case of error the function logs an error message in the error
                text string AND in the log file.
  -----------------------------------------------------------------------------
*/
int
ls30VFWriteLn(tls00_VFile   &VFile,
              tsp00_Addr     pszBuffer,
              tsp00_Longint  lCount,
              tsp00_Addr     pszErrText);


/*!
  -----------------------------------------------------------------------------
 
  function:     ls30VFWriteHeader
  -----------------------------------------------------------------------------

  description:  Writes replication server header to binary data file.

                header structure
                -------------------

                  64 byte          64 byte         32 byte   N byte           
                |------------------------------------------------------------ 
                |                |                | 4 byte |  dataextract   | aligned
                | 4 byte         | identifier     | record |  command       | to full
                | header length  |                | length |                |  kB
                |------------------------------------------------------------


  arguments:    Vfile         [IN]  - virtual file structure
                lRecordLength [IN]  - record length
                pszCommand    [IN]  - dataextract command that generated data file
                pszErrText    [OUT] - error text

  returnvalue:  errOK_els00              - in case of NO errors
                errFileWrite_ls98        - error writing data to file
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls30VFWriteHeader(tls00_VFile &VFile,
                  tsp00_Int4   lRecordLength,
                  tsp00_Addr   pszCommand,
                  tsp00_Addr   pszErrText);


/*!
  -----------------------------------------------------------------------------
 
  function:     ls30VFClose
  -----------------------------------------------------------------------------

  description:  Closes open file.
                If successful the file handle is reset to 0 and position to  EOF.

  arguments:    Vfile    [IN]   - virtual file structure
                ErrText  [OUT]  - error text

  returnvalue:  errOK_els00         - in case of NO errors
                errFileClose_els98  - error closing file

                In case of error the function logs an error message in the error
                text string AND in the log file.
  -----------------------------------------------------------------------------
*/
int ls30VFClose(tls00_VFile &VFile, tsp00_Addr ErrText);


/*!
  -----------------------------------------------------------------------------
 
  function:     ls30VFInitialize
  -----------------------------------------------------------------------------

  description:  Initialize virtual file structure. Set file position to 0.

  arguments:    NewFileStruct  [IN|OUT]  - file structure to initialize
                pszFileName    [IN]      - file name
                DataType       [IN]      - type of file to initialize (text or
                                            binary (defaults to BINARY)
                FileMode       [IN]      - open mode: read, write, append 
                                            (defaults to SP5VF_READ)
                Encoding       [IN]      - encoding of data file (UCS2, UTF8, ASCII);
                                            defaults to ASCII
                BufferingKind  [IN]      - buffer? (defaults to SP5BK_UNBUFFERED)
                CloseOption    [IN]      - closing option
                                            (defaults to SP5VF_CLOSE_NORMAL)
                LineNumber     [IN]      - line number where the action on the
                                            file will start (actually internally
                                            used for counting read or written 
                                            data rows - no real connection to the
                                            physical file) (defaults to 0)
                FilePos        [IN]      - physical file position in file where the
                                            action on the file will start
                                            (defaults to 0)

  returnvalue:  None
  -----------------------------------------------------------------------------
*/
void ls30VFInitialize(tls00_VFile&                  NewFileStruct,
                      tsp00_Addr                    pszFileName,
                      tsp05_RteDataKind_Param       DataType        = sp5vf_binary,
                      tsp05_RteFileMode_Param       FileMode        = sp5vf_read,
                      tls00_CodeType                Encoding        = ctAscii_els00,
                      tsp05_RteBufferingKind_Param  BufferingKind   = sp5bk_unbuffered,
                      tsp05_RteCloseOption_Param    CloseOption     = sp5vf_close_normal);


/*!
  -----------------------------------------------------------------------------
 
  function:     ls30VFSeek
  -----------------------------------------------------------------------------

  description:  Seek to given position

  arguments:    Vfile    [IN]   - virtual file structure
                FilePos  [IN]   - seek position
                ErrText  [OUT]  - error text

  returnvalue:  errOK_els00         - in case of NO errors
                errFileRead_els98   - error seeking file
  -----------------------------------------------------------------------------
*/
int ls30VFSeek(tls00_VFile &VFile, teo00_Longint FilePos, tsp00_Addr ErrText);

/*!
   -----------------------------------------------------------------------------
  
   function:     ls30VFSeekLong
   -----------------------------------------------------------------------------
 
   description:  Seek to given absolute file position starting at a given pos.
 
   arguments:    Vfile    [IN]  - virtual file structure
                 pFilePos [IN]  - position in file to seek to from the beginning
                 ErrText  [OUT] - error text
                 lOrigin  [IN]  - Initial position; defaults to SP5VF_SEEK_CUR<br>
                                  possible additional values are: 
                                  SP5VF_SEEK_END and SP5VF_SEEK_BEGIN
 
   returnvalue:  errOK_els00         - in case of NO errors
                 errFileRead_els98   - error seeking file
   -----------------------------------------------------------------------------
 */
 tsp00_Int4
 ls30VFSeekLong(tls00_VFile             &VFile,
                const tls00_LargeNumber *pFilePos,
                tsp00_Addr               pszErrText,
                tsp05_RteSeekKind_Param  lOrigin = sp5vf_seek_cur);
 
 /*!
   -----------------------------------------------------------------------------
  
   function:     ls30VFSeekLong
   -----------------------------------------------------------------------------
 
   description:  Seek to from given file position by a given number of bytes
   
   arguments:    Vfile     [IN]  - virtual file structure
                 lDistance [IN]  - position in file to seek to from the beginning
                 ErrText   [OUT] - error text
                 lOrigin   [IN]  - Initial position
 
   returnvalue:  errOK_els00         - in case of NO errors
                 errFileRead_els98   - error seeking file
   -----------------------------------------------------------------------------
 */
 tsp00_Int4
 ls30VFSeekLong(tls00_VFile            &VFile,
                const tsp00_Longint     lDistance,
                tsp00_Addr              pszErrText,
                tsp05_RteSeekKind_Param lOrigin = sp5vf_seek_cur);
 
 
 /*!
   -----------------------------------------------------------------------------

  function:     ls30VFTruncate

  -----------------------------------------------------------------------------

  description:  Truncates given file.
                If lNewSize is larger than current size, file is expanded but
                the new space is left uninitialized.
                Otherwise it is truncated and additional space is freed.

  arguments:    Vfile    [IN]    - virtual file structure
                ErrText  [OUT]   - error text
                lNewSize [IN]    - new size (defaults to 0)

  returnvalue:  errOK_els00         - in case of NO errors
                errFileRead_els98   - error seeking file
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls30VFTruncate(tls00_VFile &VFile, tsp00_Addr ErrText, tsp00_Longint lNewSize);


/*!
  -----------------------------------------------------------------------------
 
  function:     ls30VFGetFileSize
  -----------------------------------------------------------------------------

  description:  Gets the file size of a certain file. In case the file size is
                bigger than 2GB on a 32bit machine, we need additionally the blocks
                of 2GB.
                Example: Windows NT file with size 10.010.000.000 byte (~ 10 GB)
                         --> 2GB Blocks  = 4  (= 8.589.934.592 byte = 8 GB)
                         --> Remainder   = 1.420.065.408
                Example 2: Windows NT file with size 512 byte
                            --> 2GB Blocks   = 0
                            --> Remainder    = 512

  arguments:    pszFileName  [IN]  - Filename
                plFileSize   [OUT] - File size structure (tls00_LargeNumber) <br>
                                     containing the count of 2GB blocks plus the
                                     remaining length.

  returnvalue:  errOK_els00  - in case of NO errors
                error code   - in case of errors

  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls30VFGetFileSize(tsp00_Addr pszFileName, tls00_LargeNumber *plFileSize);


/*!
  -----------------------------------------------------------------------------
 
  function:     ls30VFMoveToStartPosition
  -----------------------------------------------------------------------------

  description:  Moves a file pointer to a given position depending on file type:
                 - text file: lStartPos gives the number of the record in file
                   to start with; that means that the function reads (number - 1)
                   records and then returns
                 - binary file: lStartPos is the physical position in file

                Function adapts file position in VFile structure.
                Function starts to move from the beginning of the file.

  arguments:    Vfile      [IN]   - virtual file structure
                lStartPos  [IN]   - start position
                pszErrText [OUT]  - error text

  returnvalue:  errOK_els00        - in case of NO errors
                errFileRead_els98  - error moving to right position
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls30VFMoveToStartPosition(tls00_VFile             &VFile,
                          const tls00_LargeNumber *plStartPos,
                          tsp00_Addr               pszErrText);

/*!
  -----------------------------------------------------------------------------
   function:     ls30Detect_BinaryFile_Encoding
  -----------------------------------------------------------------------------

  description:  Detects encoding in binary files. Mainly used for LONG data files.
                If the file encoding is set upon calling it is compared to
                the real encoding of the file. In case of a mismatch the function
                returns an error.

  arguments:    Vfile      [IN]   - virtual file structure
                pszErrText [OUT]  - error text

  returnvalue:  errOK_els00                - in case of NO errors
                errFileWrongEncoding_els98 - specified and 'real' encoding don't match
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls30Detect_BinaryFile_Encoding(tls00_VFile& VFile, tsp00_Addr pszErrText);

/*!
  -----------------------------------------------------------------------------
   function:     ls30Mark_BinaryFile_Encoding
  -----------------------------------------------------------------------------

  description:  Marks a binary file as encoded by writing magic bytes for UTF8/UCS2
                encoding.

  arguments:    Vfile      [IN]   - virtual file structure
                pszErrText [OUT]  - error text

  returnvalue:  errOK_els00         - in case of NO errors
                errFileWrite_els98  - error writing magic bytes
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ls30Mark_BinaryFile_Encoding(tls00_VFile& VFile, tsp00_Addr pszErrText);


/*!
  -----------------------------------------------------------------------------
   function:     ls30VFEmptyMagicBytesCache
  -----------------------------------------------------------------------------

  description:  Sets Bytes in magic bytes 'cache' in structure tls00_VFile
                to binary 0.
                (Removes UNICODE marker)

  arguments:    Vfile      [IN]   - virtual file structure

  returnvalue:  None
  -----------------------------------------------------------------------------
*/
inline void
ls30VFEmptyMagicBytesCache(tls00_VFile* pFile, bool bAll = false)
{
    if (true == bAll)
    {
        pFile->vfMagicBytesCache_ls00.lBufPos_ls00 = pFile->vfMagicBytesCache_ls00.lBufLen_ls00;
    }
    else
    {
        pFile->vfMagicBytesCache_ls00.lBufPos_ls00 =
            pEncodings_ls00[pFile->vfFileEncoding_ls00]->charTable->indicator.byteCount;
    }
}

/*!
  -----------------------------------------------------------------------------
   function:     ls30VFIsMagicBytesCacheEmpty
  -----------------------------------------------------------------------------

  description:  Detects if magic bytes 'cache' in structure tls00_VFile is filled.

  arguments:    Vfile      [IN]   - virtual file structure

  returnvalue:  None
  -----------------------------------------------------------------------------
*/
inline bool
ls30VFIsMagicBytesCacheEmpty(tls00_VFile* pFile)
{
    return (pFile->vfMagicBytesCache_ls00.lBufPos_ls00 == pFile->vfMagicBytesCache_ls00.lBufLen_ls00);
}

inline void
ls30VFGetMagicBytes(unsigned char* pszBuffer, tls00_VFile *pFile, tsp00_Int4 &lCount)
{
    tsp00_Int4 _avail = pFile->vfMagicBytesCache_ls00.lBufLen_ls00 - pFile->vfMagicBytesCache_ls00.lBufPos_ls00;
    if (lCount > _avail)
    {
        lCount = _avail;
    }
    for (tsp00_Int4 i = 0; i < lCount; ++i, ++pFile->vfMagicBytesCache_ls00.lBufPos_ls00)
    {
        pszBuffer[i] = REINTERPRET_CAST(unsigned char*, pFile->vfMagicBytesCache_ls00.pBufPtr_ls00)[pFile->vfMagicBytesCache_ls00.lBufPos_ls00];
    }
}

/*!  
  -----------------------------------------------------------------------------
  EndChapter: Global functions
  -----------------------------------------------------------------------------
 */

#endif /*HLS30*/