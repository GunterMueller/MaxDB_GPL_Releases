/*!
  @file           Log_OpenTransFile.hpp
  @ingroup        Logging
  @author         UweH
  @brief          defines the classes Log_OpenTransFile

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



#ifndef Log_OpenTransFile_H
#define Log_OpenTransFile_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "DataAccess/Data_ChainFixSizeSpace.hpp"
#include "DataAccess/Data_BaseFile.hpp"

class Log_Transaction;
class SAPDBMem_IRawAllocator;

/*!
    @brief      defines a page containing open transactions. Thus pages
                will be written into the openTransFile during a savepoint    
 */
class Log_OpenTransPage : public Data_PageFixSizeSpace
{

public:

    /// set the savepoint-IOSequence in a filler of the page
    void SetSavepointSequence (Log_SavepointSequence SavepointSequence)
    {
        SAPDBERR_ASSERT_STATE(IsAssigned());
        NodePtr()->ndOpenTransPage_bd00().savepointSequence = SavepointSequence; // PTS 1120695 UH 2003-10-27
    }

    /// extract the savepoint-IOSequence from a filler of the page
    Log_SavepointSequence GetSavepointSequence () const
    {
        SAPDBERR_ASSERT_STATE(IsAssigned());
        return NodePtr()->ndOpenTransPage_bd00().savepointSequence; // PTS 1120695 UH 2003-10-27
    }

    /// set the oldest iosequence of all transactions which have an eot sequence
    void SetOldestKnownIOSequence (Log_IOSequenceNo oldestEOTSequence)
    {
		// PTS 1124684 UH 2003-10-16 new
        SAPDBERR_ASSERT_STATE(IsAssigned());
        NodePtr()->ndOpenTransPage_bd00().oldestEOTSequence = oldestEOTSequence.RawValue();
    }

    /// read the oldest iosequence of all finished transactions
    Log_IOSequenceNo GetOldestKnownIOSequence () const
    {
		// PTS 1124684 UH 2003-10-16 new
        SAPDBERR_ASSERT_STATE(IsAssigned());
        return NodePtr()->ndOpenTransPage_bd00().oldestEOTSequence != 0
               ? Log_IOSequenceNo(NodePtr()->ndOpenTransPage_bd00().oldestEOTSequence)
               : Log_IOSequenceNo();
    }
    
    /// sets a flag which indicates that this open trans file was created during redo
    /// and not in online mode
    void SetCreatedDuringRedo(bool isCreatedDuringSavepoint)
    {
        SAPDBERR_ASSERT_STATE(IsAssigned());
        NodePtr()->ndOpenTransPage_bd00().isCreatedDuringSavepoint = isCreatedDuringSavepoint ? 1 : 0;
    }
    
    /// returns true if this open trans file was created during redo and in online mode
    bool GetCreatedDuringRedo()
    {
        SAPDBERR_ASSERT_STATE(IsAssigned());
        return NodePtr()->ndOpenTransPage_bd00().isCreatedDuringSavepoint != 0;
    }
};


/*---------------------------------------------------------------------------*/
/*!
   @brief          This defines the class which handles transaction, which are
                   open in a savepoint
 */

class Log_OpenTransFile : public Data_BaseFile
{

public:
    
    /// a chain of pages containing OpenTransEntries
    typedef Data_ChainFixSizeSpace<Log_OpenTransPage>  Container;

    /// an iterator for accessing entries in the Container
    typedef Container::Iterator                        Iterator;

    /// an iterator for accessing whole pages of a page-chain
    typedef Data_ChainIterator<Log_OpenTransPage>      PageIterator;
    
    /// defines whether a filelayout with or without queue-id's has to be assumed
    enum EntryLayout { HasQueueId, HasNoQueueId };
public:

    /// This is used to create a new open trans file
    Log_OpenTransFile (tgg00_TransContext          &Trans,
                       const Log_SavepointSequence  SavepointSequence,
                       bool                         bIsRedoSavepoint);

    /// This is used for reading
    Log_OpenTransFile (tgg00_TransContext  &Trans,
                       const Data_PageNo    Root,
                       const EntryLayout    entryLayout);

    /// This is used for reading 
    Log_OpenTransFile (tgg00_TransContext  &Trans,
                       const Data_PageNo    Root );

    virtual bool Create (SAPDBErr_MessageList &errlist);
    virtual bool Drop   (SAPDBErr_MessageList &errlist);
    virtual bool Verify (bool                  isCold,
                         SAPDBErr_MessageList &errlist);
    virtual void AppendContextInfo (SAPDB_Char  const * const  filename,
                                    SAPDB_UInt4 const          linenumber,
                                    SAPDBErr_MessageList      &errlist,
                                    SAPDB_Char  const * const  title = 0) const;

    /// The given Space is written into the redo file.
    bool WriteOpenTrans (Log_Transaction      &OpenTrans,
                         SAPDBErr_MessageList &errlist);

    /// set the oldest iosequence of all transactions which have an eot sequence
    bool SetOldestKnownIOSequence (Log_IOSequenceNo      oldestEOTSequence,
                                   SAPDBErr_MessageList &errlist);

    /// read the oldest iosequence of all finished transactions
    Log_IOSequenceNo GetOldestKnownIOSequence (SAPDBErr_MessageList &errlist);

    /// the iterator points to the first open trans in the file.
    /// the given savepoint sequence is checked.
    Iterator& GetFirstOpenTrans (Log_SavepointSequence  SavepointSequence,
                                 SAPDBErr_MessageList  &errlist);

    /// returns size in bytes of the container
    Data_RecordLength GetSpaceSize () const
    {
        return m_Container.GetSpaceSize();
    }

    /// returns true if this file belongs to a redo savepoint
    bool CreatedDuringRedo() const
    {
        return m_CreatedDuringRedo;
    }
    
private:

    /// The container for this file.
    Container m_Container;

    /// The iterator for the container.
    Iterator m_Iterator;
    
	/// This sequence is written into the root during Create().
    Log_SavepointSequence m_SavepointSequence;

    /// if this is true the transactions within this file were last executed during redo.
    bool m_CreatedDuringRedo;

    /// The allocator is only used by the verify method to construct an undo file.
    SAPDBMem_IRawAllocator &m_Allocator;
};



/*===========================================================================*
 *  DEFINITION OF INLINE METHODS                                              *
 *===========================================================================*/

/*===========================================================================*
 *  DEFINITION OF INLINE METHODS                                              *
 *===========================================================================*/
                                         
#endif  /* Log_OpenTransFile_H */
