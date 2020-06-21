/*!
  @file           Log_SeqTapeReader.hpp
  @ingroup        Logging
  @author         MartinB
  @brief          defines class Log_SeqTapeReader

\if EMIT_LICENCE
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

\endif
*/



#ifndef Log_SeqTapeReader_H
#define Log_SeqTapeReader_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "Logging/Log_ISequentialReader.hpp"
#include "FrameControl/FrameCtrl_ICommon.hpp"
#include "hkb39.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!
   @brief          Implements a reader for reading from tape. Used by the
                   ReDoReader.
 */


class Log_SeqTapeReader : public Log_ISequentialReader
{
public:
    /// constructor
    Log_SeqTapeReader(Log_IOSequenceNo ioSeq)
    : m_currentIOSeq(ioSeq)
    {}
     
    /// sets the actual actual read position according to the given IOSequence
    virtual ReaderStatus  SetIOSeq(   Log_IOSequenceNo         ioSeq)
    {
        m_currentIOSeq = ioSeq;
        if (ioSeq.IsValid())
        {
            return reader_ok;
        }
        else
        {
            return invalid_IOSeq;
        }
    }
protected:

    /// reads the actual page and then increments the actual position
    virtual ReaderStatus  ReadNextPage(  tsp00_TaskId             taskId,
                                         boolean                 *RTECancelFlag,
                                         Log_Page                &logPage)
    {
        if (m_currentIOSeq.IsValid())
        {
            tkb00_PagePtr framePtr = NULL;
            tgg00_BasisError   trError;

            k39locate_logentries_in_page(   taskId,
                                            *RTECancelFlag,
                                            m_currentIOSeq.RawValue(),
                                            framePtr,
                                            trError);
            if (e_ok == trError)
            {
                Log_Page::PageFrame frame(framePtr,FrameCtrl_ICommon::GetInstance().GetPageSize());
                logPage.SetFrame(frame);
                return reader_ok;
            }
            else if (e_end_of_backup == trError
                     ||
                     // PTS1111525 mb 2002-02-19 write_task_crash is set when the restore is cancelled / ignored 
                     e_write_task_crash == trError) 
            {
                return end_of_file;
            }
        }
        else
        {
            return invalid_IOSeq;
        }

        // this statement should never be reached
        return not_implemented;
    }
    
    
private:
    ///  next IOSequence to be read
    Log_IOSequenceNo             m_currentIOSeq;
};


#endif  /* Log_SeqTapeReader_H */
