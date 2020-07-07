/*!
  @file           Data_ChainSplitSpaceBackwardRead.hpp
  @author         UweH
  @brief          Defines classes to organize Split records over page chains.

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
#ifndef Data_ChainSplitSpaceBackwardRead_HPP
#define Data_ChainSplitSpaceBackwardRead_HPP

#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "DataAccess/Data_Exceptions.hpp"
#include "DataAccess/Data_Chain.hpp"
#include "DataAccess/Data_PageSplitSpace.hpp"
#include "DataAccess/Data_SplitSpace.hpp"

/// hide simple boolean expression
#define KEEP_FIRST_PART true

/*!
   @class Data_ChainSplitSpaceBackwardRead
   @brief This chain of Split record supports backward reading.
 */
template <class PAGE>
class Data_ChainSplitSpaceBackwardRead : public Data_Chain<PAGE>
{
public:
    /// define the record space
    typedef Data_SplitSpace<PAGE> RecordSpace;
public:
    /*!
        @class Iterator
        @brief Iterator for records in the chain.
     */
    class Iterator : public Data_IBreakable // PTS 1114994 2002-03-22
    {
    public:
        /// define the container on which the iterator operates
        typedef Data_ChainSplitSpaceBackwardRead<PAGE> Container;
	/// The container class is the friend of the iterator because the iterator
    /// access directly members of the container.
    friend class Data_ChainSplitSpaceBackwardRead<PAGE>;
    public:
        /*!
           @brief The iterater is not initialized and at first invalid.
         */
        Iterator (Data_PageAccessManager& PAM,
                  SAPDBMem_IRawAllocator&  Allocator)
        : m_PAM                   (PAM),
          m_RecordIterator        (),
          m_RecordSpace           (Allocator),
          m_RecordSpaceIsComplete (false)
        {}
        /// deallocates everything
        ~Iterator ()
        {
            SAPDBErr_MessageList errlist;
            if(!Delete(errlist))
                RTE_Crash(errlist);
        }
        /*!
           @brief  The iterater is initialized.
           @return false, if the iterator could not be used
         */
        bool Initialize (Data_AccessMode AccessMode = Data_ForUpdate)
        {
            bool initialized = m_RecordSpace.Initialize();
            if ( initialized )
                m_RecordSpace.SetAccessMode(AccessMode);
            return initialized;
        }
        /// The resources of the iterater are deleted.
        bool Delete(SAPDBErr_MessageList &errlist)
        {
            if(!Invalidate(errlist))
                return false;
            m_RecordSpace.Delete();
            return true;
        }
        /*!
           @brief  if the part is complete must be tested outside.
           @return true, if a part of a split space is available.
         */
        bool IsValid() const
        {
            return m_RecordSpace.IsAssigned();
        }
        /*!
           @brief This releases all allocated internal members.
           @param errlist [out]
           @param isOK [in] if set to false the pages are released without update
         */
        bool Invalidate(SAPDBErr_MessageList &errlist,
                        bool                  isOK = true)
        {
            SAPDBTRACE_METHOD_DEBUG ("Data_ChainSplitSpaceBackwardRead::Iterator::Invalidate", DataChain_Trace, 5);
            m_RecordSpaceIsComplete = false;
            m_RecordIterator.Invalidate();
            // PTS 1115979 UH 2002-05-29
            // PTS 1117126 UH 2002-08-07 removed UndoLastReservedSpace()
            if(!m_RecordSpace.Deassign(errlist,isOK))
            {
                this->AppendContextInfo(__CONTEXT__,errlist);
                return false;
            }
            return true;
        }
        /*!
           @brief   The iterator is set to the specified position.
           @return true, if successfull
         */
        bool SetPosition (Data_PageNo           pno,
                          Data_PageOffset       offset,
                          Data_AccessMode       accessmode,
                          SAPDBErr_MessageList &errlist)
        {
            return Assign (pno, offset, accessmode, errlist);
        }
        /// This returns the position where the iterator points to.
        void GetPosition (Data_PageNo     &pno,
                          Data_PageOffset &offset)
        {
            // PTS 1115056 UH 2002-03-27 changed the assertion to return value
            if ( IsValid() )
            {
                PAGE       &page = m_RecordSpace.GetCurrentPage();
                SAPDB_UInt  length;
                SAPDB_Byte *valueptr;
                m_RecordSpace.GetPart (m_RecordSpace.CurrentPart(), length, valueptr);
    
                pno    = page.PageNo();
                offset = page.GetOffset(valueptr, length);
            }
            else
            {
                pno    = Data_PageNo();
                offset = 0;
            }
        }

        /*!
            @brief  This releases all resources (pages in datacache)
            @return false on any error
            
            The context (actual position) is saved in m_BreakInfo.
         */
        virtual bool Break(SAPDBErr_MessageList &errlist) // PTS 1114994 2002-03-22 returns bool
        {
            GetPosition ( m_BreakInfo.lastPno,
                          m_BreakInfo.lastOffset );
            m_BreakInfo.lastAccessMode = GetCurrentPage().AccessMode();
            if ( ! Invalidate(errlist) )
            {
                this->AppendContextInfo(__CONTEXT__,errlist,"Break");
                return false;
            }
            return true;
        }
        /*!
            @brief  This resumes the state of a previous Break().
            @return false on any error

            m_BreakInfo is used to reset the iterators previously breaked position.
         */
        virtual bool Continue(SAPDBErr_MessageList &errlist) // PTS 1114994 2002-03-22 returns bool
        {
            if ( IsValid() )
            {
                if ( DataChain_Trace.TracesLevel (6) )
                     WriteToTrace ("Continue(): IsValid() is true");
                if ( ! Invalidate(errlist) )
                {
                    this->AppendContextInfo(__CONTEXT__,errlist,"BackwardRead::Continue() could not invalidate record space");
                    return false;
                }
                m_BreakInfo = BreakInfo();
                this->AppendContextInfo(__CONTEXT__,errlist,"BackwardRead::Continue() call without break");
                return false; // PTS 1134170 mb 2005-03-22
            }
 
            if ( ! SetPosition ( m_BreakInfo.lastPno,
                                 m_BreakInfo.lastOffset,
                                 m_BreakInfo.lastAccessMode,
                                 errlist ) )
            {
                this->AppendContextInfo(__CONTEXT__,errlist);
                m_BreakInfo = BreakInfo();
                (void)Invalidate(errlist);
                return false;
            }
            // assign the record space again
            bool result;
            (void)Deref(result,errlist);
            if ( ! result )
            {
                this->AppendContextInfo(__CONTEXT__,errlist);
                m_BreakInfo = BreakInfo();
                (void)Invalidate(errlist);
                return false;
            }
            m_BreakInfo = BreakInfo();
            return true;
        }
        /// Writes the important members to the knltrace.
        virtual void WriteToTrace (const char *title) const
        {
            Kernel_VTrace() << "backReadChainIter: Recordspace: "
                            << (m_RecordSpaceIsComplete?"complete":"not complete");
            Kernel_VTrace() << "BreakInfo: " << m_BreakInfo.lastPno << "." << m_BreakInfo.lastOffset
                            << ", accessmode: " << m_BreakInfo.lastAccessMode;
            m_RecordSpace.WriteToTrace(title);
        }
        virtual void AppendContextInfo (SAPDB_Char  const * const  filename,
                                        SAPDB_UInt4 const          linenumber,
                                        SAPDBErr_MessageList      &errlist,
                                        SAPDB_Char  const * const  title = 0) const
        {
            errlist = errlist + Data_Exception(__CONTEXT__,DATA_BACK_CHAIN_ITER_INFO,
                                               (title!=0?title:"Data_ChainSplitSpaceBackwardReadChain::Iterator"),
                                               (m_RecordSpaceIsComplete?"complete":"not complete"),
                                               SAPDB_ToString(m_BreakInfo.lastPno),
                                               SAPDB_ToString(m_BreakInfo.lastOffset),
                                               Data_AccessModeStrings[m_BreakInfo.lastAccessMode]);
            m_PAM.AppendContextInfo(filename,linenumber,errlist,title);
            m_RecordSpace.AppendContextInfo(filename,linenumber,errlist,title);
        }
        /*!
            @brief  This returns the handle of the current page.
            @return The currently referenced page.
         */
        PAGE& GetCurrentPage ()
        {
            return m_RecordSpace.GetCurrentPage();
        }
        /// This returns the record, which is currently referenced.
        // RecordSpace& operator * ()
        RecordSpace& Deref (bool                 &result,
                            SAPDBErr_MessageList &errlist)
        {
            result = true;
            if ( ! m_RecordSpaceIsComplete )
            {
                result = AssignRecordspace (m_RecordIterator, errlist);
                if ( ! result )
                    this->AppendContextInfo(__CONTEXT__,errlist);
            }
            return m_RecordSpace;
        }
        /*!
            @brief  Without assigning the complete record space.
            @return Data_SplitRecordLength
            The complete size is returned. 0 is returned, if the iterator is not valid.
         */
        Data_SplitRecordLength GetSize()
        {
            // PTS 1117126 UH 2002-08-07 new
            if ( ! IsValid() )
                return 0;
            return m_RecordIterator.RestLength();
        }
        /// The iterator is set to the previous record.
        // Iterator& operator -- ()
        bool MinusMinus (SAPDBErr_MessageList &errlist)
        {
            SAPDBTRACE_METHOD_DEBUG ("Data_ChainSplitSpaceBackwardRead::Iterator::--", DataChain_Trace, 5);

            SAPDBERR_ASSERT_STATE( IsValid() );

            // reorder and reassign the split record space
            if ( ! m_RecordSpace.Reassign (errlist, KEEP_FIRST_PART) )
            {
                this->AppendContextInfo(__CONTEXT__,errlist);
                return false;
            }
            m_RecordSpaceIsComplete = false;
            
            SAPDBERR_ASSERT_STATE( m_RecordSpace.CurrentPart() == 0 );
            
            PAGE& Page = m_RecordSpace.GetCurrentPage();
            Page.Use (m_RecordIterator);

            --m_RecordIterator;
            
            if ( ! m_RecordIterator.IsValid() )
            {
                Data_PageNo PrevPageNo = Page.PrevPageNo();

                if ( PrevPageNo.IsValid() )
                {
                    const Data_PageNo auxPageNo = Page.PageNo();
                    
                    if ( ! m_PAM.ReleasePage(Page, errlist) )
                    {
                        this->AppendContextInfo(__CONTEXT__,errlist);
                        return false;
                    }

                    if ( ! m_PAM.GetPage (Page, PrevPageNo, errlist) )
                    {
                        this->AppendContextInfo(__CONTEXT__,errlist);
                        m_RecordSpace.WriteToTrace("--");
                        errlist = errlist + Data_Exception(__CONTEXT__,
                                                  DATA_CHAIN_IS_INCONSISTENT,
                                                  SAPDB_ToString(auxPageNo),
                                                  SAPDB_ToString(Page.PageNo()));
                        return false;
                    }
                    Page.End (m_RecordIterator);
                }
                else
                {
                    if ( ! m_RecordSpace.Deassign(errlist) )
                    {
                        this->AppendContextInfo(__CONTEXT__,errlist);
                        return false;
                    }
                }
            }
            return true;
        }
        /*!
            @brief  Check the given access mode of all referenced pages.        
            @return false on any error
         */
        bool CheckAccessMode(Data_AccessMode accessmode) const
        {
            return m_RecordSpace.CheckAccessMode(accessmode);
        }
    private:
        /*!
            @brief  The Iterator is set to the last record in the given page.
            @return false on any error
            The internal record space is not assigned.
         */
        virtual bool Assign (Data_PageNo           PageNo,
                             Data_PageOffset       Offset,
                             Data_AccessMode       AccessMode,
                             SAPDBErr_MessageList &errlist)
        {
            SAPDBTRACE_METHOD_DEBUG ("Data_ChainSplitSpaceBackwardRead::Iterator::Assign", DataChain_Trace, 5);

            if ( m_RecordSpace.IsAssigned() )
            {
                if ( m_RecordSpace.GetCurrentPage().IsAssigned()
                     &&
                     m_RecordSpace.GetCurrentPage().PageNo()     == PageNo
                     &&
                     m_RecordSpace.GetCurrentPage().AccessMode() == AccessMode )
                {
                    if ( ! m_RecordSpace.Reassign(errlist) )
                    {
                        this->AppendContextInfo(__CONTEXT__,errlist);
                        return false;
                    }
                }
                else
                {
                    if ( ! m_RecordSpace.Deassign(errlist) )
                    {
                        this->AppendContextInfo(__CONTEXT__,errlist);
                        return false;
                    }
                    m_RecordSpace.SetAccessMode(AccessMode);
                    if ( ! m_RecordSpace.UseNextPart(errlist) )
                    {
                        this->AppendContextInfo(__CONTEXT__,errlist);
                        return false;
                    }
                    if ( ! m_PAM.GetPage (m_RecordSpace.GetCurrentPage(),
                                          PageNo,
                                          AccessMode,
                                          errlist ) )
                    {
                        this->AppendContextInfo(__CONTEXT__,errlist);
                        return false;
                    }
                }
            }
            else
            {
                m_RecordSpace.SetAccessMode(AccessMode);
                if ( ! m_RecordSpace.UseNextPart(errlist) )
                {
                    this->AppendContextInfo(__CONTEXT__,errlist);
                    return false;
                }
                if ( ! m_PAM.GetPage (m_RecordSpace.GetCurrentPage(),
                                      PageNo,
                                      AccessMode,
                                      errlist) )
                {
                    this->AppendContextInfo(__CONTEXT__,errlist);
                    return false;
                }
            }
                    
            SAPDBERR_ASSERT_STATE( m_RecordSpace.CurrentPart() == 0 );

            if ( Offset == 0 )            
                m_RecordSpace.GetCurrentPage().End (m_RecordIterator);
            else
                m_RecordSpace.GetCurrentPage().Use (m_RecordIterator, Offset);

            m_RecordSpaceIsComplete = false;
            
            return true;
        }

        /*!
            @brief The internal record space of the Split record is initialized.
            
            If the iterator is invalid the entry can not be read.
         */
        bool AssignRecordspace(Data_PageSplitSpaceBackwardReadIterator &RecordIter,
                               SAPDBErr_MessageList                &errlist)
        {
            SAPDBTRACE_METHOD_DEBUG ("Data_ChainSplitSpaceBackwardRead::Iterator::AssignRecordspace", DataChain_Trace, 5);

            if ( ! RecordIter.IsValid()
                 ||
                 RecordIter.PageIsEmpty() )
                 return true;

            Data_SplitRecordLength TotalRestLength = 0;
            if ( RecordIter.LengthIsValid() )
                TotalRestLength = RecordIter.RestLength();
            else
            {
                errlist = Data_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"the entry is corrupted or a wrong page was read");
                this->AppendContextInfo(__CONTEXT__,errlist);
                return false;
            }
                

                
            while ( TotalRestLength > 0 )
            {
                if ( ! RecordIter.LengthIsValid() ) // PTS 1113270 UH 2001-12-28
                {
                    errlist = Data_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"the entry is corrupted or a wrong page was read");
                    this->AppendContextInfo(__CONTEXT__,errlist);
                    Invalidate(errlist,false);
                    return false;
                }
                
                TotalRestLength -= RecordIter.Length();
 
                m_RecordSpace.SetPart (RecordIter.Length() - sizeof(Data_SplitRecordLength),
                                       *RecordIter);
                                       
                if ( TotalRestLength > 0 )
                {
                    --RecordIter;

                    if ( ! RecordIter.IsValid() )
                    {
                        Data_PageNo PrevPageNo = m_RecordSpace.GetCurrentPage().PrevPageNo();

                        if ( PrevPageNo.IsValid() )
                        {
                            if ( ! m_RecordSpace.UseNextPart(errlist) )
                            {
                                this->AppendContextInfo(__CONTEXT__,errlist);
                                return false;
                            }
                            if ( m_PAM.GetPage (m_RecordSpace.GetCurrentPage(), PrevPageNo, errlist) )
                                m_RecordSpace.GetCurrentPage().End (RecordIter);
                            else
                            {
                                // The caller has to decide if an uncomplete logentry
                                // is acceptable or not.
                                errlist = errlist +
                                          Data_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                                                         "incomplete backread image");
                                this->AppendContextInfo(__CONTEXT__,errlist);
                                Invalidate(errlist, false);
                                return false;
                            }
                        }
                        else
                        {
                            // The caller has to decide if an uncomplete logentry
                            // is acceptable or not.
                            errlist = errlist +
                                      Data_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                                                     "incomplete backread image (prevpno): nil");
                            this->AppendContextInfo(__CONTEXT__,errlist);
                            Invalidate(errlist, false);
                            return false;
                        }
                    }
                }
            }
            if ( TotalRestLength != 0 )
            {
                // The caller has to decide if an uncomplete logentry
                // is acceptable or not.
                errlist = errlist +
                          Data_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                                         "incomplete backread image (totalrestlength>0)");
                Kernel_VTrace() << "TotalRestLength: " << TotalRestLength;
                this->AppendContextInfo(__CONTEXT__,errlist);
                Invalidate(errlist, false);
                return false;
            }
                
            m_RecordSpace.Reorder();
            m_RecordSpaceIsComplete = true;

            SAPDBTRACE_IF (DataChain_Trace, 6, m_RecordSpace.WriteToTrace("AssignRecordSpace"));
            return true;
        }
        /// Set the isAssigned flag. Is Needed for Reserve.
        void SetAssigned (bool IsAssigned)
        {
            m_RecordSpaceIsComplete = IsAssigned;
        }
        /// returns a reference to the internal record space.
        RecordSpace &GetRecordSpace()
        {
            return m_RecordSpace;
        }
    private:
        /// page access manager
        Data_PageAccessManager& m_PAM;
        /// internally used record iterator
        Data_PageSplitSpaceBackwardReadIterator m_RecordIterator;
        /// This refers to the space of the current record.
        RecordSpace m_RecordSpace;
        /// if this is true, the record space is correctly assigned.
        bool m_RecordSpaceIsComplete;
        /// defines BreakInfo
        struct BreakInfo
        {
            /// last page no
            Data_PageNo     lastPno;
            /// page offset
            Data_PageOffset lastOffset;
            /// access mode
            Data_AccessMode lastAccessMode;
            /// nil constructor
            BreakInfo()
            {
                lastPno.Invalidate();
                lastOffset     = 0;
                lastAccessMode = Data_ForRead;
            }
        };
        /// This is used by Break() and Continue().
        BreakInfo m_BreakInfo;
    };
public:
    /*!
        @brief Prepare to access the chain identified by RootId
        @param PAM    [in/out] this is the page access manager
        @param RootId [in/out] this specifies the first page of the page chain
     */
    Data_ChainSplitSpaceBackwardRead (Data_PageAccessManager &PAM,
                                      Data_PageId             &RootId)
    : Data_Chain<PAGE> (PAM, RootId)
    {}
    /*!
       @param          RootId [in] this id defines the beginning of the page chain
       @param          LastPageNo [in] 
       @brief          The parameter are stored.

     */

    /*!
        @brief Prepare to access the chain identified by RootId
        @param PAM    [in/out] this is the page access manager
        @param RootId [in/out] this specifies the first page of the page chain
        @param LastPageNo [in] this specifies the last page of the page chain
     */
    Data_ChainSplitSpaceBackwardRead (Data_PageAccessManager &PAM,
                                      Data_PageId             &RootId,
                                      Data_PageNo              LastPageNo)
    : Data_Chain<PAGE> (PAM, RootId, LastPageNo)
    {}
    /// A new Chain is created. The PageIter points to the root.
    bool Create (Data_ChainIterator<PAGE> &RootPageIter,
                 SAPDBErr_MessageList     &errlist)
    {
        return Data_Chain<PAGE>::Create (RootPageIter, errlist); // PTS 1121659 UH 2003-04-30
    }
    /// A new Chain is created.
    bool Create (SAPDBErr_MessageList &errlist)
    {
        SAPDBTRACE_METHOD_DEBUG ("Data_ChainSplitSpaceBackwardRead::Create", DataChain_Trace, 5);
        Data_ChainIterator<PAGE> RootPageIter (this->GetPageAccessManager());
        return Create (RootPageIter, errlist); // PTS 1121659 UH 2003-04-30
    }
    /*!
       @brief  Space is reserved in the chain. The record iterator points to the Split space.
       @param  WantedSize [in] The amount of space needed.
       @param  FirstPartMiniumSize [in] if set to 0, no minimum is given
       @param  RecordIterator [out] A RecordIterator, which points to the knewly created space.
       @param  numberOfAddedPages [out]
       @return false of no more pages available
     */
    bool ReserveSpace (Data_SplitRecordLength WantedSize,
                       Data_RecordLength      FirstPartMiniumSize,
                       Iterator&              RecordIterator,
                       SAPDB_UInt             &numberOfAddedPages, // PTS 1121659 UH 2003-04-30
                       SAPDBErr_MessageList   &errlist) 
    {
        SAPDBTRACE_METHOD_DEBUG ("Data_ChainSplitSpaceBackwardRead::ReserveSpace", DataChain_Trace, 5);

        SAPDBERR_ASSERT_ARGUMENT( WantedSize >= FirstPartMiniumSize );
        
        SAPDBTRACE_WRITELN (DataChain_Trace, 6, "Wanted: " << WantedSize << \
                                           ", FirstPartMin: " << FirstPartMiniumSize);
                        
        // prepare the record space
        RecordSpace &Space = RecordIterator.GetRecordSpace();

        numberOfAddedPages = 0; // PTS 1121659 UH 2003-04-30
            
        if ( ! RecordIterator.Invalidate(errlist) )
            return false;

        if ( ! this->FindLastPageNo(errlist) )
            return false;
            
        if ( ! RecordIterator.Assign( this->LastPageNo(), 0, Data_ForUpdate, errlist ) )
        {
            this->AppendContextInfo(__CONTEXT__,errlist);
            return false;
        }
        
        bool AllFits;
        bool FirstPartFits;
        
        Space.GetCurrentPage().SpaceInfo (WantedSize, FirstPartMiniumSize,
                                          FirstPartFits, AllFits);
                                          
        if ( ! AllFits )
        {
            if ( ! RecordIterator.Invalidate(errlist) )
            {
                this->AppendContextInfo(__CONTEXT__,errlist);
                return false;
            }
            if ( ! RecordIterator.Assign( this->LastPageNo(), 0,
                                          Data_ForStructureChange,
                                          errlist ) )
            {
                this->AppendContextInfo(__CONTEXT__,errlist);
                return false;
            }
        }
        
        // fill the internal record space of the record iterator

        Data_SplitRecordLength   RestWantedSize = WantedSize;
        Data_SplitRecordLength   ReservedSize = 0;
        Data_RecordLength        CurrentReservedSize;
        SAPDB_Byte              *CurrentSpace;

        do
        {
            Space.GetCurrentPage().
                ReserveSpace (ReservedSize, RestWantedSize, FirstPartMiniumSize,
                              CurrentReservedSize, CurrentSpace);

            SAPDBTRACE_WRITELN (DataChain_Trace, 6, \
                                "ReserveSpace(" << Space.CurrentPart() \
                                << "): pno/Reserved/RestWanted/CurrReserved: " \
                                << Space.GetCurrentPage().PageNo() << "/" \
                                << ReservedSize << "/" << RestWantedSize << "/" \
                                << CurrentReservedSize);

            if ( CurrentReservedSize > 0)
            {
                SAPDBERR_ASSERT_STATE( CurrentSpace != NULL );
                Space.SetPart (CurrentReservedSize, CurrentSpace);
            }

            if ( RestWantedSize > 0 )
            {
                Space.SetNewPageWasAppended();

                if ( ReservedSize > 0 )
                {
					// PTS 1117126 UH 2002-08-07
					// UseNextPart() calls Resize() which reallocates the memory
					// So before UseNextPart() no reference into the memory of Space
					// may be saved.
                    const Data_ISplitSpace::PartNo partNoOfCurrentPage = Space.CurrentPart();
                    if(!Space.UseNextPart(errlist))
                    {
                        this->AppendContextInfo(__CONTEXT__,errlist);
                        return false;
                    }
                    PAGE &CurrentPage = Space.GetPage(partNoOfCurrentPage);
                    PAGE &NextPage    = Space.GetCurrentPage();
                    
                    if ( ! CurrentPage.Append (NextPage, errlist) )
                    {
                        this->AppendContextInfo(__CONTEXT__,errlist);
                        return false;
                    }
                    this->m_LastPageNo = NextPage.PageNo();
                    NextPage.End(RecordIterator.m_RecordIterator);
                    
                    if ( CurrentPage.RootPageNo() != NextPage.RootPageNo() )
                    {
                        errlist = Data_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"invalid root 1");
                        this->AppendContextInfo(__CONTEXT__,errlist);
                        return false;
                    }
                }
                else
                {
                    // the space was not big enough for FirstPartMiniumSize
                    PAGE& CurrentPage = Space.GetCurrentPage();
                    // PTS 1133123 UH 2004-12-22 keep old last page
                    PAGE& OldLastPage = RecordIterator.GetRecordSpace().GetPrevPage();
                    
                    OldLastPage.Assign (CurrentPage);
                    
                    if(!OldLastPage.Append (CurrentPage,errlist))
                    {
                        this->AppendContextInfo(__CONTEXT__,errlist);
                        return false;
                    }
                    this->m_LastPageNo = CurrentPage.PageNo();
                    
                    CurrentPage.End(RecordIterator.m_RecordIterator);
                }
                ++numberOfAddedPages; // PTS 1121659 UH 2003-04-30
            }
        }
        while ( RestWantedSize > 0 );        
        
        RecordIterator.SetAssigned (true);

        SAPDBTRACE_IF (DataChain_Trace, 6, Space.WriteToTrace("ReserveSpace"));

        return true;
    }
    /// All changes done be ReserveSpace() are undone.
    void UndoReserveSpace (const RecordSpace &space)
    {
		// PTS 1117126 UH 2002-08-07 new
        // PTS 1133123 UH 2004-12-22 rewritten, input of iterator changed to space
        SAPDBTRACE_METHOD_DEBUG ("Data_ChainSplitSpaceBackwardRead::UndoReserveSpace", DataChain_Trace, 5);
        this->m_LastPageNo = space.GetFirstPageNo();
    }
    /// The RecordIterator is set to the last record in the chain.
    bool End (Iterator&             RecordIterator,
              Data_AccessMode       AccessMode,
              SAPDBErr_MessageList &errlist)
    {
        if ( ! this->FindLastPageNo(errlist) )
            return false;

        if ( ! RecordIterator.Assign( this->LastPageNo(), 0, AccessMode,errlist ) )
        {
            errlist = Data_Exception(__CONTEXT__, DATA_CHAIN_IS_INCONSISTENT,
                                      "-",
                                      SAPDB_ToString(this->LastPageNo()) );
            this->AppendContextInfo(__CONTEXT__,errlist);
            return false;
        }
        return true;
    }
    /*!
        @brief The RecordIterator is set to the specified record in the chain.

        If the address could not be set, the iterator becomes invalid.
     */
    bool Set (Iterator&             recordIter,
              Data_PageNo           pageno,
              Data_PageOffset       offset,
              Data_AccessMode       accessmode,
              SAPDBErr_MessageList &errlist)
    {
        if ( ! recordIter.Assign( pageno, 0, accessmode, errlist ) )
        {
            RTE_Crash( Data_Exception(__CONTEXT__,
                                      DATA_CHAIN_IS_INCONSISTENT,
                                      "-",
                                      SAPDB_ToString(this->LastPageNo()) ));
            this->AppendContextInfo(__CONTEXT__,errlist);
            return false;
        }
        return true;
    }
    /// This writes the important information into the knltrace.
    virtual void WriteToTrace (const char *title = NULL) const
    {
        if ( title != NULL )
            Kernel_VTrace() << title;
        Kernel_VTrace() << "RootPageNo/LastPageNo: " << this->m_RootId.PageNo() << "/" << this->m_LastPageNo;
    }
};
#undef KEEP_FIRST_PART
#endif // Data_ChainSplitSpaceBackwardRead_HPP
