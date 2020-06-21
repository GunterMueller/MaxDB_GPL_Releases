/*!
    @ingroup        Restart
    @file           Rst_RedoFile.hpp
    @author         UweH
    @brief          defines the classes Rst_RedoFile
*/
/*
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
#ifndef Rst_Redo_H
#define Rst_Redo_H

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "DataAccess/Data_ChainSplitSpaceForwardRead.hpp"
#include "DataAccess/Data_BaseFile.hpp"
#include "Logging/Log_AfterImageSpace.hpp"
#include "Logging/Log_Types.hpp"

class tgg91_TransNo;
class Data_PageAccessManager;
class SAPDBMem_IRawAllocator;
class Data_SplitSpaceReader;
class Data_SplitSpaceWriter;

/// defines a page of Redo-Entries 
class Rst_RedoPage : public Data_PageSplitSpaceForwardRead
{
public:

    /// this method inializes the neccessary PageFrame 
    virtual void InitializeFrame ()
    {
        Data_PageSplitSpace::InitializeFrame();
        NodePtr()->ndRedoPage_bd00().transno.gg90SetNil();
        SetFirstEntrySequence (0);
        SetLastEntrySequence(Log_EntrySequence());
    }

    /// The TransNo is written to the page header.
    void SetTransNo (const tgg91_TransNo& TransNo)
    {
        SAPDBERR_ASSERT_STATE(IsAssigned());
        NodePtr()->ndRedoPage_bd00().transno = TransNo;
    }

    /// The TransNo is read from the page header.
    tgg91_TransNo GetTransNo () const
    {
        SAPDBERR_ASSERT_STATE(IsAssigned());
        return NodePtr()->ndRedoPage_bd00().transno;
    }

    /// The entry sequence of the first entry in the page is set in the page header.
    void SetFirstEntrySequence (Log_EntrySequence entrySequence)
    {
        SAPDBERR_ASSERT_STATE(IsAssigned());
        NodePtr()->ndRedoPage_bd00().firstEntrySequence = entrySequence;
    }

    /// The entry sequence of the first entry is returned.
    Log_EntrySequence GetFirstEntrySequence () const
    {
        SAPDBERR_ASSERT_STATE(IsAssigned());
        return NodePtr()->ndRedoPage_bd00().firstEntrySequence;
    }

    /// The entry sequence of the last entry in the page is set in the page header.
    void SetLastEntrySequence (Log_EntrySequence entrySequence)
    {
        SAPDBERR_ASSERT_STATE(IsAssigned());
        NodePtr()->ndRedoPage_bd00().lastEntrySequence = entrySequence;
    }

    /// The entry sequence of the last entry is returned.
    Log_EntrySequence GetLastEntrySequence () const
    {
        SAPDBERR_ASSERT_STATE(IsAssigned());
        return NodePtr()->ndRedoPage_bd00().lastEntrySequence;
    }
};


/*---------------------------------------------------------------------------*/
/*!
    @class          Rst_RedoFile
    @brief          This is the Interface to Redo log manager.
 */
class Rst_RedoFile : public Data_BaseFile
{

public:
    
    /// In this container all Redo-Etries of one Transaction are collected
    typedef Data_ChainSplitSpaceForwardRead<Rst_RedoPage>  Container;

    /// iterator upon an RedoFile; used for reading the redo-entries
    typedef Container::Iterator                            Iterator;

    /// iterator representing the PageChain of an RedoFile
    typedef Data_ChainIterator<Rst_RedoPage>               PageIterator;

    /// space which is referenced by iterator
    typedef Container::RecordSpace                         EntrySpace;
    
public:

    /*!
        @brief This constructs an Redo log handle for the given transaction.
        If the RootPageNo is valid, an existing file is assumed.
    */

    Rst_RedoFile (tgg00_TransContext &Trans,
                  const Data_PageNo   Root,
                  const Data_PageNo   Last);

    /// variant with other pam for verify
    Rst_RedoFile (Data_PageAccessManager &Pam,
                  SAPDBMem_IRawAllocator &Allocator,
                  const tgg91_TransNo    &TransNo,
                  const Data_PageNo       Root,
                  const Data_PageNo       Last);

    /// Releases all memory of this objectinstance, but the File
    /// remains persistent. PTS 1131934 mb 2004-10-14
    bool Delete (Msg_List &errlist);
    virtual bool Create (SAPDBErr_MessageList &errlist);
    virtual bool Drop   (SAPDBErr_MessageList &errlist);
    virtual bool Verify (bool                  isCold,
                         SAPDBErr_MessageList &errlist);
    virtual void WriteToTrace (const char * title = 0) const;

    /// returns the saved transno member
    const tgg91_TransNo& GetTransNo ()
    {
        return m_TransNo;
    }

    /// the io sequence is extracted.
    static bool GetEntryInfo (Data_SplitSpaceReader &Reader,
                              Log_IOSequenceNo      &IOSequence,
                              Log_EntrySequence     &EntrySequence,
                              SAPDBErr_MessageList  &errlist);
                                    
    /*!
        @brief          Space is reserved. the first part has minimum size of MinimalLength.

        The internal iterator is set to the space.
        You have to use ReleaseSpace() after writing everything into the reserved space.
     */
    Data_SplitSpaceWriter ReserveSpace (Data_SplitRecordLength Length,
                       Data_RecordLength      MinimalLength,
                       Log_EntrySequence      EntrySequence,
                       Log_IOSequenceNo       IOSequence,
                       SAPDBErr_MessageList  &errlist);

    /// sets up a SplitSpaceWriter for an already partially copied entry for continuing the copying
    Data_SplitSpaceWriter ReReserveSpace (Data_SplitRecordLength RestLength,
                                          SAPDBErr_MessageList  &errlist);

    /*!
        @brief The internal iterator is deassigned,
        So all resources are released.
        If undoChanges is true, all pareserved pages are released unchanged.
     */
    bool ReleaseSpace(bool                   undoChanges,
                      SAPDBErr_MessageList  &errlist);

    /// Sets the internal iterator to the first record in the file.
    Iterator& GetFirstRedoEntry (bool                 &result,
                                 SAPDBErr_MessageList &errlist)
    {
        SAPDBERR_ASSERT_STATE( IsCreated() );

        result = m_Container.Begin (m_Iterator, Data_ForUpdate, errlist); // not Data_ForRead !!

        return m_Iterator;
    }

	/// determinies the last entry sequence written to this file.
    bool GetLastEntrySequence (Log_EntrySequence    &entrySequence,
                               SAPDBErr_MessageList &errlist);

	/// returns the known last pageno
    Data_PageNo LastPageNo() const
    {
        SAPDBERR_ASSERT_STATE( IsCreated() );

        return m_Container.LastPageNo();
    }

    /// determines the last page no.
    bool FindLastPageNo (SAPDBErr_MessageList &errlist,
                         bool                  forceReadAllPages = false)
    {
        return m_Container.FindLastPageNo(errlist,forceReadAllPages);
    }
    
    /// this truncates the file, so the last entry has the given sequence no.
    bool Truncate (Log_EntrySequence     stopSequence,
                   SAPDBErr_MessageList &errlist);
                   
    /// release ressources in order to avoid a blocking og other tasks (e.g. savepoint)
    //  PTS 1134170 mb 2005-03-11
    bool Break( SAPDBErr_MessageList &errlist )
    {
        return m_Iterator.Break(errlist);
    }
    
    /// Get the ressources which has been releases by break() again for coninuation of processing 
    //  PTS 1134170 mb 2005-03-11
    bool Continue( SAPDBErr_MessageList &errlist )
    {
        return m_Iterator.Continue(errlist);
    }

private:

	/// this holds additional information for entry handling
    typedef struct EntryHead
    {
        /// the sequence-number of this LogEntry in this transaction 
        Log_EntrySequence Sequence;

        /// the global sequence-number of this LogEntry 
        Log_IOSequenceNo  IOSequence;
    } EntryHead;
    
	/// The container for this file.
    Container m_Container;

	/// The iterator for the container.
    Iterator m_Iterator;
    
    /// transno for this file
    const tgg91_TransNo &m_TransNo;
    
    /// The iosequence of this Logentry. It is written at first when the logentry is
    /// materialized (i.e. released) - PTS 1134170 mb 2005-03-10
    Log_IOSequenceNo    m_IOSequence;
};

#endif  /* Rst_Redo_H */
