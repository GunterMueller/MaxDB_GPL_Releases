/*
  -----------------------------------------------------------------------------
  module: vls51.cpp
  -----------------------------------------------------------------------------
  responsible:  SteffenS and d033893

  special area: SAP DB LOADER

  description:  Support class for TABLELOAD functionality.

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
#include "gls00.h"
#include "hls07.h"
#include "hls30.h"
#include "hls51.h"
#include "hls98msg.h"
#include "hls99.h"

/*
  -----------------------------------------------------------------------------
  function:     Constructor
  -----------------------------------------------------------------------------
*/
ReadBuffer::ReadBuffer(tls00_VFile *VFile, int SingleBufSize, int SingleBufCount):
m_lUpperBoundFilePos(0),
m_lLowerBoundFilePos(0)
{
    ROUTINE_DBG_MEO00 ("ReadBuffer");

    m_FileStruct = VFile;

    m_BufferSize     = SingleBufSize * SingleBufCount;
    m_BufferPartSize = SingleBufSize;
    m_Buffer         = new _TCHAR[m_BufferSize];      // no trailing NULL
    
    m_UpperBound     = SingleBufCount - 1;            // we're in the times of C
    
    m_Index          = m_UpperBound;                  // force first buffering of data pages
    
    m_ErrorCode      = errOK_els00;
    m_ErrorText[0]   = 0;
    m_EOFReached     = false;

    m_notUsed        = false;
}


/*
  -----------------------------------------------------------------------------
  function:     Destructor
  -----------------------------------------------------------------------------
*/
ReadBuffer::~ReadBuffer()
{
    if (NULL != m_Buffer)
    {
        delete [] m_Buffer;
        m_Buffer = NULL;
    }
}


/*
  -----------------------------------------------------------------------------
  function:     ls51GetNext
  -----------------------------------------------------------------------------
*/
_TCHAR *
ReadBuffer::ls51GetNext()
{
    if (false == m_notUsed)
    {
        if (m_Index == m_UpperBound)
        {
            if  (true == m_EOFReached)
            {
                m_ErrorCode = EOF_LS00;
                return NULL;
            }

            // Not reached EOF -> get new data from file
            m_lLowerBoundFilePos = m_FileStruct->vfFilePos_ls00.lnu2GB_Blocks_ls00*MAX_INT4_SP00 +
                                    m_FileStruct->vfFilePos_ls00.lnuRemainder_ls00;

            tsp00_Longint ReadLen = ls30VFReadLongBin(m_Buffer,
                                                      *m_FileStruct,
                                                      m_BufferSize,
                                                      &m_FileStruct->vfFilePos_ls00,
                                                      m_ErrorText);
            if (ReadLen >= 0)
            {
                if (ReadLen < m_BufferSize)
                {
                    m_UpperBound = STATIC_CAST(tsp00_Int4, ReadLen) / m_BufferPartSize - 1;
                    m_EOFReached = true;
                }
                // Reinitialize
                m_Index = 0;

                m_lUpperBoundFilePos = m_FileStruct->vfFilePos_ls00.lnu2GB_Blocks_ls00*MAX_INT4_SP00 +
                                        m_FileStruct->vfFilePos_ls00.lnuRemainder_ls00;
            }
            else
            {
                m_ErrorCode = STATIC_CAST(tsp00_Int4, ReadLen);      // ReadLen = -1 signals EOF
                return NULL;
            }
        } //if (m_Index == m_UpperBound)
        else
        {
            m_Index++;
        }
    }
    else
    {
        m_notUsed = false;
    }

    // Achtung: eine schwaeche - wir geben hier einen pointer in einen puffer zurueck; damit ist
    // allerdings auch noch der rest des puffers vom aufrufer zugreifbar!!!

    return &m_Buffer[m_Index * m_BufferPartSize];
}
// ls51GetNext()


/*
  -----------------------------------------------------------------------------
  function:     ls51GetError
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ReadBuffer::ls51GetError(_TCHAR *pszErrText)
{
    if (EOF_LS00 != m_ErrorCode)        // error
    {
        size_t Len = _tcslen(m_ErrorText);
        Len = (Len < MAX_REPLY_LENGTH_LS00) ? Len : (MAX_REPLY_LENGTH_LS00 - 1);
        SAPDB_memcpy(pszErrText, m_ErrorText, Len);
        pszErrText[Len] = 0;
    }

    return m_ErrorCode;
}
// ls51GetError()


/*
  -----------------------------------------------------------------------------
  function:     ls51ResetFilePos
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ReadBuffer::ls51ResetFilePos()
{
    ROUTINE_DBG_MEO00 ("ls51ResetFilePos");
    
    tsp00_Int4 rc = errOK_els00;

    tsp00_Longint lDistance  = ls51GetCountOfUnusedPages();

    TRACE_PRNF_MLS99(("ls51ResetFilePos", "Unused pages = %d", lDistance) );

    if (NULL != m_FileStruct)
    {
        if ( (dtypeFile_ls00 == m_FileStruct->vfFileType_ls00) && (0 != lDistance) )
        {
            lDistance *= (-m_BufferPartSize);
            rc =ls30VFSeekLong(*m_FileStruct, lDistance, m_ErrorText);
        
            // Force reloading buffer after resetting file pos with next request of buffer
            m_Index = m_UpperBound;

            // In case the EOF marker had been set to true we have to reset it because with
            // resetting we've not reached the end yet.
            if (true == m_EOFReached)
            {
                m_EOFReached = false;
            }
        }
    }
    return rc;
}
// ls51ResetFilePos()


/*
  -----------------------------------------------------------------------------
  function:     ls51SetFilePos
  -----------------------------------------------------------------------------
*/
tsp00_Int4
ReadBuffer::ls51SetFilePos(tls00_LargeNumber NewPos)
{
    ROUTINE_DBG_MEO00 ("ls51SetFilePos");
    
    tsp00_Int4 rc = errOK_els00;

    if (NULL != m_FileStruct)
    {
        tsp00_Int8 _newfilepos = NewPos.lnu2GB_Blocks_ls00 * MAX_INT4_SP00 + NewPos.lnuRemainder_ls00;

        // We can't seek backwards to a point in file before the first read portion in buffer
        if (_newfilepos <= m_lLowerBoundFilePos)
        {
            if (_newfilepos == m_lLowerBoundFilePos)
            {
                m_Index = 0;
            }
            else
            {
                rc = -1;    // Error
            }
        }
        else
        {
            if (_newfilepos >= m_lUpperBoundFilePos)
            {
                // Seek to new position outside the current read part and force reloading at next request
                if (_newfilepos > m_lUpperBoundFilePos)
                {
                    rc = ls30VFSeekLong(*m_FileStruct, &NewPos, m_ErrorText);
                }
                m_Index   = m_UpperBound;
                m_notUsed = false;
            }
            else
            {
                // Seems the pos is within the buffer
                tsp00_Longint _cnt     = ls51GetCountOfUnusedPages();
                tsp00_Int8    _readpos = m_lUpperBoundFilePos - (_cnt * m_BufferPartSize);
                if (_newfilepos != _readpos)
                {
                    // m_Index is the index of the last read/delivered page. It will be
                    // increase by 1 before delivering the next page with ls51GetNext - thus we
                    // have to decrease the index here by 1.
                    // Additionally we must make sure that the index is really increased upon
                    // delivery - reset the m_notUsed flag.
                    m_Index = STATIC_CAST(tsp00_Int4, (_newfilepos - m_lLowerBoundFilePos))/m_BufferPartSize - 1;
                    m_notUsed = false;
                }
                // in case _newfilepos equals _readpos we're simply done
            }
        }   // end else of if (_newfilepos <= this->m_lLowerBoundFilePos)
    }   // end if (NULL != m_FileStruct)
    return rc;
}
// ls51SetFilePos()


/*
  -----------------------------------------------------------------------------
  function:     ls51RecalculateFilePos
  -----------------------------------------------------------------------------
*/
void
ReadBuffer::ls51RecalculateFilePos(tls00_LargeNumber &NewPos)
{
    ROUTINE_DBG_MEO00 ("ls51RecalculateFilePos");
    
    if (NULL != m_FileStruct)
    {
        NewPos = m_FileStruct->vfFilePos_ls00;

        tsp00_Longint lDistance  = ls51GetCountOfUnusedPages();
        
        if (0 != lDistance)
        {
            TRACE_PRNF_MLS99(("ls51RecalculateFilePos", "Unused pages = %d", lDistance) );
    
            tls00_LargeNumber Subtrahend;
            Subtrahend.lnu2GB_Blocks_ls00 = 0;
            Subtrahend.lnuRemainder_ls00  = lDistance * m_BufferPartSize;

            NewPos = ls07SubtractLargeNumbers(m_FileStruct->vfFilePos_ls00, Subtrahend);
        }
    }
    else
    {
        NewPos.lnu2GB_Blocks_ls00 = -1;
        NewPos.lnuRemainder_ls00  = -1;
    }
}
// ls51RecalculateFilePos(()
