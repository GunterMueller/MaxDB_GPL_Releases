/*!
  @file           Log_ActionPartialRollback.hpp
  @ingroup        ActionHandling
  @author         UweH
  @brief          defines class Log_ActionPartialRollback

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



#ifndef Log_ActionPartialRollback_H
#define Log_ActionPartialRollback_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "DataAccess/Data_ISplitSpace.hpp"
#include "Logging/Log_Types.hpp"
#include "Logging/Log_IAction.hpp"

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
   @brief          Handle for manipulating OMS objects.
   @copydoc        Log_IAction
 */
class Log_ActionPartialRollback : public Log_IAction
{

public:

    /// default ctor
    Log_ActionPartialRollback()
    :Log_IAction(Log_PartialRollback)
    {}

    /// assigns the sequences for truncation
    void Initialize (Log_EntrySequence UndoStopSequence,
                     Log_EntrySequence RedoStopSequence)
    {
        m_Head.UndoStopSequence = UndoStopSequence;
        m_Head.RedoStopSequence = RedoStopSequence;
    }
    
    /// returns the minimal size of an entry
    static SAPDB_UInt GetMinimalLengthToCopyEntry()
    {
        return sizeof (Head);
    }

    
    virtual SAPDB_UInt GetMinimalPersistentLength() const
    {
        return GetMinimalLengthToCopyEntry();
    }
    
    virtual SAPDB_UInt GetPersistentLength() const
    {
        return GetMinimalPersistentLength();
    }
    
    virtual bool WritePersistentFormat (Data_SplitSpaceWriter &Writer,
                                        SAPDBErr_MessageList  &errlist) const
    {
        Head                                *pHead;
        const Data_SplitSpaceWriter::Result  result =
            Writer.Reserve (sizeof(Head), reinterpret_cast<SAPDB_Byte*&>(pHead));
        if ( Data_SplitSpaceWriter::ok != result )
        {
            errlist = Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                                    "WriteResult is ok");
            Writer.AppendContextInfo(__CONTEXT__,errlist);
            AppendContextInfo(__CONTEXT__,errlist);
            return false;
        }
        *pHead = m_Head;
        return true;
    }

    virtual bool ReadPersistentFormat (Data_SplitSpaceReader  &Reader,
                                       SAPDBMem_IRawAllocator &Allocator,
                                       SAPDBErr_MessageList   &errlist)
    {
        Head                                *pHead;
        const Data_SplitSpaceReader::Result  result =
            Reader.Reserve (sizeof(Head), reinterpret_cast<SAPDB_Byte*&>(pHead));
        if ( Data_SplitSpaceReader::ok != result )
        {
            errlist = Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                                    "ReadResult is ok");
            Reader.AppendContextInfo(__CONTEXT__,errlist);
            AppendContextInfo(__CONTEXT__,errlist);
            return false;
        }
        m_Head = *pHead;
        return true;
    }

    virtual void WriteToTrace (const char * Title) const
    {}

    /// if this returns false a local redo entry cash must be flushed
    virtual bool LocalRedoEntryCachingPossible() const
    {
        return false;
    }
	/// append context info for error messages
    virtual void AppendContextInfo (SAPDB_Char  const * const  filename,
                                    SAPDB_UInt4 const          linenumber,
                                    SAPDBErr_MessageList      &errlist,
                                    SAPDB_Char  const * const  title = 0) const
	{}

    /// returns the IOSequence, where the redo will stop
    Log_EntrySequence GetRedoStopSequence () const
    {
        return m_Head.RedoStopSequence;
    }
    
    /// returns the IOSequence where the Undo will stop 
    Log_EntrySequence GetUndoStopSequence () const
    {
        return m_Head.UndoStopSequence;
    }

private:

    /// pysical layout of the LogEntry in the undo/redo-file
    typedef struct
    {
        Log_EntrySequence UndoStopSequence; ///< IOsequence of for the Undo
        Log_EntrySequence RedoStopSequence; ///< IOSequqnce for the Redo
    } Head;
    
    /// in memory representation of the partial rollback
    Head m_Head;
};


#endif  /* Log_ActionPartialRollback_H */
