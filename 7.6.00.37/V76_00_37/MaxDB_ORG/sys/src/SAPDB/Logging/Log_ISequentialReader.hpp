/*!
  @file           Log_ISequentialReader.hpp
  @author         UweH
  @author         MartinB
  @ingroup        LogVolume
  @brief          Manages the log volume with queue and writer

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

#ifndef Log_ISequentialReader_H
#define Log_ISequentialReader_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h" // tsp00_TaskId, tsp00_PageNo, tsp00_Line
#include "SAPDBCommon/SAPDB_Types.hpp"
#include "Logging/Log_Types.hpp"
#include "Logging/Log_RawDeviceIterator.hpp"

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
    @brief virtual base class for implementing a reader for the ReDoReader
*/

class Log_ISequentialReader 
{

friend class Log_VolumeIterator;

public:
    /// defines the possible states of the iterator
    enum ReaderStatus {
        reader_ok,
        end_of_file,
        invalid_IOSeq,
        invalid_Offset,
        not_implemented        
    };
    

    /*!
    @brief        the destructors of the derived classes has to be called
    */
    virtual ~Log_ISequentialReader() {} ; 
    /*!
    @brief        sets the actual actual read position according to the given IOSequence
                  returns not_implemented as default, beacause not all devices supports
                  such a set-operation 
    @param        ioSeq          ioSeq, which shall be read next 
    @return       ReaderStatus
    */
    virtual ReaderStatus  SetIOSeq(   Log_IOSequenceNo         ioSeq)
    {
        return not_implemented;
    }
    
    /*!
    @brief        sets the actual page and IOSeq according to the given offset
                  returns not_implemented as default, beacause not all devices supports
                  such a set-operation 
    @param        offset         offset, which shall be read next
    @return       ReaderStatus
    */
    virtual ReaderStatus  SetPosition(   Log_RawDeviceOffset      offset)
    {
        return not_implemented;
    }
    
    /*!
    @brief        gets the actual offset of the last page read (invalid for a tape)
                  returns the InvalidValue if the device doesn't support a position
    @return       the offset of the last page read in form of an Iterator
    */
    virtual Log_RawDeviceIterator  GetPositionIter() const
    {
        return Log_RawDeviceIterator() ;
    }
    
    /*!
    @brief        gets the actual offset of the last page read (invalid for a tape)
                  returns the InvalidValue if the device doesn't support a position
    @return       the offset of the last page read
    */
    virtual Log_RawDeviceOffset GetPosition() const
    {
        return Log_RawDeviceOffset() ;
    }
    
    /*!
    @brief        deallocates all dynamically allocated memory (PTS 1126187)
    */
    virtual void Delete()
    {
    }
    

protected:
    /*!
    @brief        reads the actual page and then increments the actual position 
    @param        taskId        identification of the running task
    @param        RTECancelFlag pointer to the cancel flag of the TransContext
    @param        logPage       location where the read page will be stored
    @returns      ReaderStatus
    */
    virtual ReaderStatus  ReadNextPage(  tsp00_TaskId             taskId,
                                         boolean                 *RTECancelFlag,
                                         Log_Page                &logPage) = 0;
    
};

#endif  /* Log_ISequentialReader_H */
