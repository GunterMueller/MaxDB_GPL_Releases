/*! 
  -----------------------------------------------------------------------------
  module: hls51.h
  -----------------------------------------------------------------------------
  responsible:  SteffenS and d033893

  special area: SAP DB LOADER
 
  description:  Support class for TABLELOAD functionality.</br>
                Reads and buffers data pages (db pages) from data file created
                with TABLEEXTRACT.
                
  version:      7.6.
  -----------------------------------------------------------------------------
    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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
#ifndef HLS51_H
#define HLS51_H

#include "gls00.h"      // includes stdio.h, too


class ReadBuffer
{
public:
    
    /*!
    -----------------------------------------------------------------------------
      function:     Constructor
    -----------------------------------------------------------------------------
      description:  Constructs page buffer object.

      arguments:    VFile          [IN] - file structure for data file to read from;
                                           must already been initialized
                    SingleBufSize  [IN] - size of a single db page to read
                    SingleBufCount [IN] - count of pages to buffer

      returnvalue:  None
    -----------------------------------------------------------------------------
    */
    ReadBuffer(tls00_VFile *VFile, int SingleBufSize, int SingleBufCount);


    /*!
    -----------------------------------------------------------------------------
      function:     Destructor
    -----------------------------------------------------------------------------
      description:  Destructs page buffer object.

      arguments:    None

      returnvalue:  None
    -----------------------------------------------------------------------------
    */
    ~ReadBuffer();


    /*!
    -----------------------------------------------------------------------------
      function:     ls51GetNext
    -----------------------------------------------------------------------------
      description:  Returns next (not read) page from buffer.
                    If all buffered pages were requested next 3 pages are read
                    from data file.

      arguments:    None

      returnvalue:  - char pointer to page
                    - NULL in case of error or eof
    -----------------------------------------------------------------------------
    */
    _TCHAR     *ls51GetNext();


    /*!
    -----------------------------------------------------------------------------
      function:     ls51GetError
    -----------------------------------------------------------------------------
      description:  Returns error code and error text.
                    Should be called if ls51GetNext returned NULL.

      arguments:    pszErrText  [OUT] - error text for returned error code

      returnvalue:  error code
    -----------------------------------------------------------------------------
    */
    tsp00_Int4  ls51GetError(_TCHAR *pszErrText);

    
    /*!
    -----------------------------------------------------------------------------
      function:     ls51NotUsed
    -----------------------------------------------------------------------------
      description:  Switches a read page (already in buffer) to state not read.

      arguments:    None

      returnvalue:  None
    -----------------------------------------------------------------------------
    */
    void          ls51NotUsed() { m_notUsed = true;}

    /*!
    -----------------------------------------------------------------------------
      function:     ls51GetCountOfUnusedPages
    -----------------------------------------------------------------------------
      description:  Returns count of unused pages still in buffer.

      arguments:    None

      returnvalue:  Count of unused pages.
    -----------------------------------------------------------------------------
    */
    tsp00_Longint ls51GetCountOfUnusedPages()
    {
        tsp00_Longint Count = m_UpperBound - m_Index;
        if (true == m_notUsed)
        {
            Count += 1;
        }
        return Count;
    }


    /*!
    -----------------------------------------------------------------------------
      function:     ls51SetFilePos
    -----------------------------------------------------------------------------
      description:  Sets the buffer to a new read position in file.

      arguments:    NewPos              - new position in file

      returnvalue:  errOK_els00         - no error
                    -1                  - position is set to a value < lowerBound of
                                           buffer requesting to seek backwards
                    errFileRead_els98   - error seeking file
    -----------------------------------------------------------------------------
    */
    tsp00_Int4 ls51SetFilePos(tls00_LargeNumber NewPos);


    /*!
    -----------------------------------------------------------------------------
      function:     ls51ResetFilePos
    -----------------------------------------------------------------------------
      description:  Resets file pos in file (and file structure) by count of
                    unused buffer parts.

      arguments:    None

      returnvalue:  None
    -----------------------------------------------------------------------------
    */
    tsp00_Int4 ls51ResetFilePos();


    /*!
    -----------------------------------------------------------------------------
      function:     ls51RecalculateFilePos
    -----------------------------------------------------------------------------
      description:  Recalculates file pos in file (and file structure) using 
                    the count of unused buffer parts;
                    Does NOT manipulate the file structure but returns the new
                    file pos as parameter

      arguments:    NewPos   [OUT]  - calculated file pos

      returnvalue:  None
    -----------------------------------------------------------------------------
    */
    void ls51RecalculateFilePos(tls00_LargeNumber &NewPos);


// Attributes
public:
    tls00_VFile *m_FileStruct;

private:
    _TCHAR        *m_Buffer;
    tsp00_Longint  m_BufferSize;
    int            m_BufferPartSize;
    int            m_Index;
    int            m_UpperBound;

    tsp00_Int8     m_lUpperBoundFilePos;
    tsp00_Int8     m_lLowerBoundFilePos;

    bool           m_EOFReached;
    bool           m_notUsed;
    tsp00_Int4     m_ErrorCode;
    _TCHAR         m_ErrorText[MAX_REPLY_LENGTH_LS00];


};

#endif  /* HLS51_H */