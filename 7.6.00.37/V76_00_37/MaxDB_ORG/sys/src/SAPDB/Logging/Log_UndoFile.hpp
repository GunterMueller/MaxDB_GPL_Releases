/*!**************************************************************************

  module      : Log_UndoFile.hpp
  special area: Logging
  responsible : UweH
  created     : 2000-09-25  12:00
  last changed: 2000-10-27  12:00
  copyright:    Copyright (c) 2000-2005 SAP AG
  description : defines the classes Log_UndoFile, Log_UndoFileEntry



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


*****************************************************************************/


#ifndef Log_UndoFile_H
#define Log_UndoFile_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "ggg00.h"

#include "SAPDBCommon/SAPDB_Types.hpp"

#include "DataAccess/Data_PageAccessManager.hpp"
#include "DataAccess/Data_ChainSplitSpaceBackwardRead.hpp"
#include "DataAccess/Data_BaseFile.hpp"

#include "Logging/Log_Exceptions.hpp"
#include "Logging/Log_IAction.hpp"

#include "RunTime/RTE_Message.hpp"

class SAPDBMem_IRawAllocator;
class Log_BeforeImage;

/*! 
    @brief A page containing undo information
*/

class Log_UndoPage : public Data_PageSplitSpaceBackwardRead
{

public:

    /// verifies an Undo_Page
    virtual SAPDB_Bool Verify(bool                  isAdmin,
                              SAPDBErr_MessageList &errlist) const
    {
        if ( ! this->IsAssigned() )
            return SAPDB_TRUE;
            
        const SAPDB_Byte* pFrame = this->FramePtr();
        const Data_PageOffset firstFreeOffset = this->FirstFreeOffset(pFrame);
    	if ( firstFreeOffset < this->MinSpaceOffset()
             ||
             firstFreeOffset > this->MaxSpaceOffset() )
        {
            errlist = Log_Exception(__CONTEXT__,
                                     LOG_UNDOPAGE_VERIFY,
                                     SAPDB_ToString(this->PageNo()),
                                     SAPDB_ToString(firstFreeOffset) );
            return SAPDB_FALSE;
        }
        else
            return SAPDB_TRUE;
	}

#ifdef SAPDB_SLOW // PTS 1133711 UH 2005-02-02
    /// destructor. verifies the page before destroying it
    ~Log_UndoPage()
    {
        SAPDBErr_MessageList errlist;
        if ( this->Verify(false,errlist) != SAPDB_TRUE )
        {
            errlist = errlist + Log_Exception(__CONTEXT__,
                                          SAPDBERR_ASSERT_STATE_FAILED,
                                          "Verify() failed");
            RTE_Crash(errlist);
        }
    }
#endif
    
    /// initializes frame. Only relevant parts are initialized
    virtual void InitializeFrame ()
    {
        Data_PageSplitSpace::InitializeFrame();
        NodePtr()->ndUndoPage_bd00().transno.gg90SetNil();
        SetLastEntrySequence(Log_EntrySequence());
    }

    /// The TransNo is written to the page header.
    void SetTransNo (const tgg91_TransNo& TransNo)
    {
        SAPDBERR_ASSERT_STATE(IsAssigned());
        NodePtr()->ndUndoPage_bd00().transno = TransNo;
    }

    ///  The TransNo is read from the page header.
    tgg91_TransNo GetTransNo ()
    {
        SAPDBERR_ASSERT_STATE(IsAssigned());
        return NodePtr()->ndUndoPage_bd00().transno;
    }
    
    ///  This saves the given pagecount in the pageheader.
    void SetPageCount (SAPDB_UInt pagecount)
    {
        SAPDBERR_ASSERT_STATE(IsAssigned());
        NodePtr()->ndUndoPage_bd00().pagecount = SAPDB_Int4(pagecount);
    }

    ///  reads out the saved pagecount
    SAPDB_UInt GetPageCount ()
    {
        SAPDBERR_ASSERT_STATE(IsAssigned());
        return SAPDB_UInt(NodePtr()->ndUndoPage_bd00().pagecount);
    }
    
    ///  This saves the given pagecount in the pageheader.
    void SetIsRelevantForGC (bool isRelevantForGC)
    {
        SAPDBERR_ASSERT_STATE(IsAssigned());
        NodePtr()->ndUndoPage_bd00().relevantForGC = isRelevantForGC;
    }

    ///  reads out the saved pagecount
    bool GetIsRelevantForGC ()
    {
        SAPDBERR_ASSERT_STATE(IsAssigned());
        return SAPDB_UInt(NodePtr()->ndUndoPage_bd00().relevantForGC);
    }
    
    /// The entry sequence of the last entry in the page is set in the page header.
    void SetLastEntrySequence (Log_EntrySequence entrySequence)
    {
        SAPDBERR_ASSERT_STATE(IsAssigned());
        NodePtr()->ndUndoPage_bd00().lastEntrySequence = entrySequence;
    }
    
    /// The entry sequence of the last entry is returned.
    Log_EntrySequence GetLastEntrySequence () const
    {
        SAPDBERR_ASSERT_STATE(IsAssigned());
        return NodePtr()->ndUndoPage_bd00().lastEntrySequence;
    }
    
    /// Save the position of the GarbageCollector in the UndoPage (PTS ? UH 2002-03-15 new)
    void SaveLastGarbageCollectorPosition (Data_PageNo     pageno,
                                           Data_PageOffset offset)
    {
        SAPDBERR_ASSERT_STATE(IsAssigned());
        NodePtr()->ndUndoPage_bd00().lastGC_offset = offset;
        NodePtr()->ndUndoPage_bd00().lastGC_pageno = pageno; // PTS 1120695 UH 2003-10-27
    }

    /// Read the Position of the GarbageCollector out of the page (PTS ? UH 2002-03-15 new)
    void GetLastGarbageCollectorPosition (Data_PageNo     &pageno,
                                          Data_PageOffset &offset)
    {
        SAPDBERR_ASSERT_STATE(IsAssigned());
        offset = NodePtr()->ndUndoPage_bd00().lastGC_offset;
        pageno = NodePtr()->ndUndoPage_bd00().lastGC_pageno; // PTS 1120695 UH 2003-10-27
    }

};

/*!
    @brief  This is the Interface to undo log manager.
*/
class Log_UndoFile : public Data_BaseFile
{

public:
    
    typedef Data_ChainSplitSpaceBackwardRead<Log_UndoPage>  Container;      ///< The container of undo-information
    typedef Container::Iterator                             Iterator;       ///< shorter notation
    typedef Data_ChainIterator<Log_UndoPage>                PageIterator;   ///< iterator for moving in an Log_UndoPage
    typedef Container::RecordSpace                          EntrySpace;     ///< This is object which is referenced by the iterator.

public:

    /// This constructs an undo log handle for the given transaction.
    /// If the RootPageNo is valid, an existing file is assumed.
    Log_UndoFile (tgg00_TransContext &Trans,
                  const Data_PageNo   Root,
                  const Data_PageNo   Last);
              
    /// This constructs an undo log handle for the garbage collector.
    /// If the RootPageNo must be valid.
    Log_UndoFile (tgg00_TransContext  &Trans,
                  const tgg91_TransNo &TransNo,
                  const Data_PageNo    Root,
                  const Data_PageNo    Last);
              
    /// This constructs an undo log handle for Verify().
    /// If the RootPageNo muast be valid.
    /// This should not be used if a new file should be created.
    Log_UndoFile (Data_PageAccessManager &Pam,
                  SAPDBMem_IRawAllocator &Allocator,
                  const tgg91_TransNo    &TransNo,
                  const Data_PageNo       Root,
                  const Data_PageNo       Last);
              
    /// A new UndoLogfile is created, the transNo is written into the root.
    /// @returns true, if successfull
    virtual bool Create (SAPDBErr_MessageList &errlist);

    /// A UndoLogfile is dropped.
    virtual bool Drop (SAPDBErr_MessageList &errlist);

    /// returns true, if the file is consistent
    virtual bool Verify(bool                  isCold,
                        SAPDBErr_MessageList &errlist);

    /// This traces some basic information about the file to the trace.
    virtual void WriteToTrace (const char * title = 0) const;

    /// The file is not dropped, but all resources are released
    bool Invalidate (SAPDBErr_MessageList &errlist)
    {
        if ( ! m_Iterator.Invalidate(errlist) )
            return false;
        if ( ! m_Iterator.Delete(errlist) )
            return false;
        m_Container.Invalidate();
        m_RootId.Invalidate();
        return true;
    }
    /// This type defines alle return values from WriteBeforeImage()
    enum WriteBeforeImageResult // PTS 1121659 UH 2003-04-30 new
    {
        writeBeforeImageOk,
        writeBeforeImageError,
        writeBeforeImageNoSpaceAvailable
    };
    
    /// The given BeforeImage is written into the undo file.
    ///          m_LastEntrySequence is incremented and written into the before image.
    ///          The internal iterator is set to the write position.
    ///          At the end the resoruce the iterator points to is deassigned.
    ///          These are the pages of the undo log file.
	/// @return  WriteBeforeImageResult
    WriteBeforeImageResult WriteUndoEntry (      Log_BeforeImage &beforeimage,
                                           const Log_IAction     &action,
                                           Data_PageNo           &pageno,
                                           Data_PageOffset       &offset,
                                           SAPDBErr_MessageList  &errlist);

    /// Only the iterator is given. It may be invalid.
    /// @return     The internal Iterator.
    Iterator& GetIterator ()
    {
        return m_Iterator;
    }

    /// The internal RecordIterator is set to the last record in the chain. The internal
    /// RecordSpace is assigned.
    bool GetLastUndoEntry (Data_AccessMode       accessmode,
                           Iterator             &iter,
                           SAPDBErr_MessageList &errlist);

    /*!
    @brief This is used by consistent read and DeleteBeforeImage.
           The call was successfull if true is returned and the iterator is valid.

    @param  pageno     belongs to the pageref
    @param  offset     belongs to the pageref
    @param  accessmode forRead only for consistent read otherwise forUpdate
    @param  maxsize    maximum allowed size for the read entry
    @param  iter       if is valid it references a split space which is an undo entry
    @param  errlist    is filled if errors occured
    @return false on any error, means not that entry was found.
    
    If the iterator is valid, the split space is completly assigned.
    If the size at the found position is greater then maxsize the iterator is invalidated.
    If maxsize is 0 it is ignored.
    */
    bool GetUndoEntry (Data_PageNo             pageno,
                       Data_PageOffset         offset,
                       Data_AccessMode         accessmode,
                       Data_SplitRecordLength  maxsize,
                       Iterator               &iter,
                       SAPDBErr_MessageList   &errlist);

    /// determinies the last entry sequence written to this file.
    bool GetLastEntrySequence (Log_EntrySequence    &LastEntrySequence,
                               SAPDBErr_MessageList &errlist);

    /// If the container does not know the last page no it is determined.
    Data_PageNo LastPageNo () const
    {
        return m_Container.LastPageNo();
    }

    /// determines the last page no.
    bool FindLastPageNo (SAPDBErr_MessageList &errlist,
                         bool                  forceReadAllPages = false)
    {
        return m_Container.FindLastPageNo(errlist,forceReadAllPages);
    }
    
    /// TransNo of the file.
    const tgg91_TransNo& GetTransNo () const
    {
        return m_TransNo;
    }

    SAPDB_UInt GetPageCount() const
    {
        return m_PageCount;
    }

    /// The transient data is stored in the root page
    bool SavePageCountAndRelevanceForGC (SAPDBErr_MessageList &errlist);

    /// The transient data is restored fromthe root page
    bool RestorePageCountAndRelevanceForGC (SAPDBErr_MessageList &errlist);

    /// saves the current position of the garbagecollector in the undofile
    bool SaveLastGarbageCollectorPosition (Data_PageNo           pageno,
                                           Data_PageOffset       offset,
                                           SAPDBErr_MessageList &errlist);

    /// retrieves the last saved position of the garbage collector from the undofile
    /// In addition it does the same like calling RestorePageCountAndRelevanceForGC()
    bool GetLastGarbageCollectorPosition (Data_PageNo          &pageno,
                                          Data_PageOffset      &offset,
                                          SAPDBErr_MessageList &errlist);

    bool GarbageCollectionNeeded() const
    {
        return m_GarbageCollectionNeeded;
    }
    
private:

    /// The container for this file.
    Container m_Container;

    /// The iterator for the container.
    Iterator m_Iterator;

    /// The transaction which owns this file.
    const tgg91_TransNo &m_TransNo;
    
    /// The number of pages used by this container.
    SAPDB_UInt m_PageCount;
    
    /// This flag is set if any action is appended which needs garbage collection.
    /// This can only be used in online mode directly after writing the undo file.
    bool m_GarbageCollectionNeeded;
};


/*===========================================================================*
 *  DEFINITION OF INLINE METHODS                                              *
 *===========================================================================*/
                                         
#endif  /* Log_UndoFile_H */
