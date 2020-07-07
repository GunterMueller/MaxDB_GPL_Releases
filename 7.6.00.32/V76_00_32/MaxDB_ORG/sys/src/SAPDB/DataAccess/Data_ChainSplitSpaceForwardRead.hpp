/*!
  @file           Data_ChainSplitSpaceForwardRead.hpp
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
#ifndef Data_ChainSplitSpaceForwardRead_HPP
#define Data_ChainSplitSpaceForwardRead_HPP

#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "DataAccess/Data_Exceptions.hpp"
#include "DataAccess/Data_Chain.hpp"
#include "DataAccess/Data_PageSplitSpace.hpp"
#include "DataAccess/Data_SplitSpace.hpp"

/// define simple boolean expression
#define FIRST_FREE   true

/*!
   @brief This chain of Split record supports forward reading.
 */
template <class PAGE>
class Data_ChainSplitSpaceForwardRead : public Data_Chain<PAGE>
{
public:
    /// define the record space
    typedef Data_SplitSpace<PAGE> RecordSpace;

    // --------------- Begin of subclass Iterator -----------------
    /*!
        @brief Iterator for records in the chain.
     */
    class Iterator : public Data_IBreakable // PTS 1114994 2002-03-22
    {
    public:

        /// define the used container
        typedef Data_ChainSplitSpaceForwardRead<PAGE> Container;

        /// The container is a friend of its iterator so the iterator may access members of the container.
        friend class Data_ChainSplitSpaceForwardRead<PAGE>;

        /*!
            @brief          The iterater is not initialized and at first invalid.
            @param          PAM [in] The AccessManager to use by the Iterator.
            @param          Allocator [in] Is used for the Split space.
         */

        Iterator (
                  Data_PageAccessManager& PAM,
                  SAPDBMem_IRawAllocator&  Allocator);

        /// deallocates everything
        ~Iterator ();

        /*!
           @brief  The iterater is initialized.
           @param  errlist [out]
           @param  AccessMode [in] default ForUpdate
           @return false, if the iterator could not be used
         */
        bool Initialize(
                        SAPDBErr_MessageList &errlist,
                        Data_AccessMode       AccessMode = Data_ForUpdate);

        /// The resources of the iterater are deleted.
        bool Delete(SAPDBErr_MessageList &errlist);

        /*!
            @return true, if a part of a split space is available.

            If the part is complete must be tested outside.
         */
        bool IsValid() const
        {
            return m_RecordSpace.IsAssigned() && m_RecordIterator.IsValid();
        }

        /*!
            @brief This releases all allocated internal members.
            @param  errlist [out]
            @param isOK [in] if set to false the pages are released without update
         */
        bool Invalidate(
                        SAPDBErr_MessageList &errlist,
                        bool                  isOK = true);

        /*!
            @brief  Set the iterator to the given position.
            @return true, if successfull
         */
        bool SetPosition (
                          Data_PageNo           pno,
                          Data_PageOffset       offset,
                          Data_AccessMode       accessmode,
                          SAPDBErr_MessageList &errlist)
        {
            return Assign (pno, offset, accessmode, errlist);
        }

        /// This returns the position, where the first page of the iterator points to.
        void GetPosition (Data_PageNo     &pno,
                          Data_PageOffset &offset)
        {
            // PTS 1115056 UH 2002-03-27 changed the assertion to return value
            if ( IsValid() )
            {
                PAGE       &page = m_RecordSpace.GetPage(0);
                SAPDB_UInt  length;
                SAPDB_Byte *valueptr;
                m_RecordSpace.GetPart (0, length, valueptr);

                pno    = page.PageNo();
                offset = page.GetOffset(valueptr, length);
            }
            else
            {
                pno    = Data_PageNo();
                offset = 0;
            }
        }

        /// This releases all resources (pages in datacache)
        virtual bool Break(SAPDBErr_MessageList &errlist); // PTS 1114994 2002-03-22 returns bool

        /// This resumes the state of a previous Break().
        virtual bool Continue(SAPDBErr_MessageList &errlist);

        /// This returns the handle of the current page.
        PAGE& GetCurrentPage ();

        /// return the internal record space. Made public for PTS 1137932 in order to allow the
        /// SplitSpaceWriter to be asigned to an existing RecordSpace
        RecordSpace &GetRecordSpace()
        {
            return m_RecordSpace;
        }

        /// The internal record space of the Split record is initialized.
        bool AssignRecordspace(
                               Data_PageSplitSpaceForwardReadIterator &RecordIter,
                               SAPDBErr_MessageList               &errlist)
        {
            SAPDBTRACE_METHOD_DEBUG ("Data_ChainSplitSpaceForwardRead::Iterator::AssignRecordSpace", DataChain_Trace, 5);

            if ( ! RecordIter.IsValid()
                 ||
                 RecordIter.PageIsEmpty() )
                 return true;

            Data_SplitRecordLength TotalRestLength = RecordIter.RestLength();

            while ( TotalRestLength > 0 )
            {
                if ( m_RecordSpace.CurrentPart() > 0 )
                    TotalRestLength -= (RecordIter.Length() - sizeof(Data_SplitRecordLength));
                else
                    TotalRestLength -= RecordIter.Length();

                m_RecordSpace.SetPart (RecordIter.Length() - sizeof(Data_SplitRecordLength),
                                       *RecordIter);

                if ( TotalRestLength > 0 )
                {
                    ++RecordIter;

                    if ( ! RecordIter.IsValid() )
                    {
                        Data_PageNo NextPageNo = m_RecordSpace.GetCurrentPage().NextPageNo();

                        if ( ! NextPageNo.IsValid() )
                        {
                            errlist = Data_Exception(__CONTEXT__,DATA_CHAIN_IS_INCONSISTENT,
                                      SAPDB_ToString(m_RecordSpace.GetCurrentPage().PageNo()),
                                      SAPDB_ToString(NextPageNo) );
                            this->AppendContextInfo(__CONTEXT__,errlist);
                            return false;
                        }
                        if ( ! m_RecordSpace.UseNextPart(errlist) )
                        {
                            this->AppendContextInfo(__CONTEXT__,errlist);
                            return false;
                        }
                        if ( ! m_PAM.GetPage (m_RecordSpace.GetCurrentPage(), NextPageNo, errlist) )
                        {
                            errlist = Data_Exception(__CONTEXT__,DATA_CHAIN_IS_INCONSISTENT,
                                      SAPDB_ToString(m_RecordSpace.GetCurrentPage().PageNo()),
                                      SAPDB_ToString(NextPageNo) );
                            this->AppendContextInfo(__CONTEXT__,errlist);
                            return false;
                        }
                        m_RecordSpace.GetCurrentPage().Begin (RecordIter);
                    }
                }
            }
            if ( TotalRestLength != 0 )
            {
                errlist = Data_Exception(__CONTEXT__,DATA_CHAIN_IS_INCONSISTENT,
                          "TotalRestLength",
                          SAPDB_ToString(TotalRestLength));
                this->AppendContextInfo(__CONTEXT__,errlist);
                return false;
            }

            m_RecordSpaceIsComplete = true;
            
            SAPDBTRACE_IF (DataChain_Trace, 6, m_RecordSpace.WriteToTrace("AssignRecordSpace"));
            return true;
        }
        /// This returns the record, which is currently referenced. Without assigning the complete record space.
        RecordSpace& Deref( bool                 &result,
                                   SAPDBErr_MessageList &errlist);

        /// The complete size is returned. 0 is returned, if the iterator is not valid.
        Data_SplitRecordLength GetSize();

        /// The iterator is set to the previous record and the internal record space is assigned.
        bool PlusPlus(SAPDBErr_MessageList &errlist);

        /// Write the important values from this class to the knltrace.
        virtual void WriteToTrace (const char* title = NULL) const;

        /// Write the important values from this class to the knltrace.
        virtual void AppendContextInfo (SAPDB_Char  const * const  filename,
                                        SAPDB_UInt4 const          linenumber,
                                        SAPDBErr_MessageList      &errlist,
                                        SAPDB_Char  const * const  title = 0) const;
    private:
        /*!
            @brief  Set the iterator to a specified position.
            @return true, if the given PageNo was found.
            The record space is newly assigned to the specified page
            and dependent on the offset the record is set to the first, last or
            any other record in the page.
         */
        bool Assign (
                     Data_PageNo           PageNo,
                     Data_PageOffset       Offset,
                     Data_AccessMode       AccessMode,
                     SAPDBErr_MessageList &errlist)
        {
            SAPDBTRACE_METHOD_DEBUG ("Data_ChainSplitSpaceForwardRead::Iterator::Assign", DataChain_Trace, 5);

            if ( m_RecordSpace.IsAssigned() )
            {
                if ( m_RecordSpace.GetCurrentPage().PageNo() == PageNo 
                     &&
                     m_RecordSpace.GetCurrentPage().AccessMode() == AccessMode )
                {
                    if(!m_RecordSpace.Reassign(errlist))
                    {
                        this->AppendContextInfo(__CONTEXT__,errlist);
                        return false;
                    }
                }
                else
                {
                    if (!m_RecordSpace.Deassign(errlist))
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
                    if ( ! m_PAM.GetPage ( m_RecordSpace.GetCurrentPage(),
                                           PageNo,
                                           AccessMode,
                                           errlist) )
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
                if ( ! m_PAM.GetPage ( m_RecordSpace.GetCurrentPage(),
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
                m_RecordSpace.GetCurrentPage().Begin (m_RecordIterator);
            else if ( Offset == SAPDB_MAX_UINT2 )
                m_RecordSpace.GetCurrentPage().End (m_RecordIterator);
            else
                m_RecordSpace.GetCurrentPage().Use (m_RecordIterator, Offset);

            m_RecordSpaceIsComplete = false;
            
            return true;
        }
        /// Set the isAssigned flag. Is Needed for Reserve.
        void SetAssigned (bool IsAssigned)
        {
            m_RecordSpaceIsComplete = IsAssigned;
        }
    private:
        /// The used page access manager
        Data_PageAccessManager& m_PAM;
        /// The internally used record iterator.
        Data_PageSplitSpaceForwardReadIterator m_RecordIterator;
        /// This refers to the space of the current record.
        RecordSpace m_RecordSpace;
        /// if this is true, the record space is correctly assigned.
        bool m_RecordSpaceIsComplete;
        /// the breakinfo
        struct BreakInfo
        {
            /// last pageno
            Data_PageNo     lastPno;
            /// last offset
            Data_PageOffset lastOffset;
            /// last accessmode
            Data_AccessMode lastAccessMode;
            /// nil constructor
            BreakInfo()
            {
                lastPno.Invalidate();
                lastOffset     = 0;
                lastAccessMode = Data_ForRead;
            }
        };
        /// this is used by Break() and Continue()
        BreakInfo m_BreakInfo;
    // --------------- End of subclass Iterator -----------------
    };
public:
    /*!
       @brief The parameter are stored.
       @param PAM [in] the page access manager which must be used
       @param RootId [in] this id defines the beginning of the page chain
     */
    Data_ChainSplitSpaceForwardRead (
                                      Data_PageAccessManager &PAM,
                                      Data_PageId             &RootId);
    /*!
       @brief The parameter are stored.
       @param PAM [in] the page access manager which must be used
       @param RootId [in] this id defines the beginning of the page chain
       @param LastPageNo [in] this id defines the known last page no.
     */
    Data_ChainSplitSpaceForwardRead (
                                     Data_PageAccessManager &PAM,
                                     Data_PageId             &RootId,
                                     Data_PageNo              LastPageNo);

    /// A new Chain is created. The PageIter points to the root.
    bool Create (
                 Data_ChainIterator<PAGE> &RootPageIter,
                 SAPDBErr_MessageList     &errlist);
    /// A new Chain is created.
    bool Create (SAPDBErr_MessageList &errlist);

    /*!
       @brief  Space is reserved in the chain. The record iterator points to the Split space.
       @param  WantedSize [in] The amount of space needed.
       @param  FirstPartMiniumSize [in] if set to 0, no minimum is given
       @param  RecordIterator [out] A RecordIterator, which points to the knewly created space.
       @param  numberOfAddedPages [out]
       @param  errlist [out]
       @return false if no space is available.
     */
    bool ReserveSpace (
                       Data_SplitRecordLength  WantedSize,
                       Data_RecordLength       FirstPartMiniumSize,
                       Iterator&               RecordIterator,
                       SAPDB_UInt             &numberOfAddedPages,
                       SAPDBErr_MessageList   &errlist ); // PTS 1121659 UH 2003-04-30
    /// All changes done be ReserveSpace() are undone.
    void UndoReserveSpace (const RecordSpace &space);
    /// The RecordIterator is set to the first record in the chain. The internal RecordSpace is assigned.
    bool Begin (
                Iterator             &RecordIterator,
                Data_AccessMode       AccessMode,
                SAPDBErr_MessageList &errlist);
};



// ------------------------------------------------------------------------
// 
// I M P L E M E N T A T I O N   P A R T
// 
// ------------------------------------------------------------------------


template <class PAGE>
inline Data_ChainSplitSpaceForwardRead<PAGE>::Iterator::Iterator (
          Data_PageAccessManager& PAM,
          SAPDBMem_IRawAllocator&  Allocator)
: m_PAM                   (PAM),
  m_RecordIterator        (),
  m_RecordSpace           (Allocator),
  m_RecordSpaceIsComplete (false)
{}

// ---------------------------------------------------------------------------
template <class PAGE>
inline Data_ChainSplitSpaceForwardRead<PAGE>::Iterator::~Iterator ()
{
    SAPDBErr_MessageList errlist;
    if(!Delete(errlist))
        RTE_Crash(errlist);
}

// ---------------------------------------------------------------------------
template <class PAGE>
inline bool Data_ChainSplitSpaceForwardRead<PAGE>::Iterator::Initialize(
                SAPDBErr_MessageList &errlist,
                Data_AccessMode       AccessMode )
{
    m_RecordSpace.Initialize();
    m_RecordSpace.SetAccessMode(AccessMode);
    return true;
}

// ---------------------------------------------------------------------------
template <class PAGE>
inline bool Data_ChainSplitSpaceForwardRead<PAGE>::Iterator::Delete( SAPDBErr_MessageList &errlist)
{
    if(!Invalidate(errlist))
    {
        this->AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }
    m_RecordSpace.Delete();
    return true;
}

// ---------------------------------------------------------------------------
template <class PAGE>
inline bool Data_ChainSplitSpaceForwardRead<PAGE>::Iterator::Invalidate(
                SAPDBErr_MessageList &errlist,
                bool                  isOK )
{
    SAPDBTRACE_METHOD_DEBUG ("Data_ChainSplitSpaceForwardRead::Iterator::Invalidate", DataChain_Trace, 5);
    m_RecordSpaceIsComplete = false;
    m_RecordIterator.Invalidate();
    // PTS 1115979 UH 2002-05-29
    // PTS 1117126 UH 2002-08-07 removed UndoLastReservedSpace
    if(!m_RecordSpace.Deassign(errlist,isOK))
    {
        this->AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }
    return true;
}

// ---------------------------------------------------------------------------
template <class PAGE>
bool Data_ChainSplitSpaceForwardRead<PAGE>::Iterator::Break(SAPDBErr_MessageList &errlist) // PTS 1114994 2002-03-22 returns bool
{
    GetPosition ( m_BreakInfo.lastPno,
                  m_BreakInfo.lastOffset );
    m_BreakInfo.lastAccessMode = m_RecordSpace.GetPage(0).AccessMode();
    if ( ! Invalidate(errlist) )
    {
        this->AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }
    return true;
}

// ---------------------------------------------------------------------------
template <class PAGE>
bool Data_ChainSplitSpaceForwardRead<PAGE>::Iterator::Continue(SAPDBErr_MessageList &errlist)
{
    if ( IsValid() ) // PTS 1134170 mb 2005-03-22
    {
        if ( DataChain_Trace.TracesLevel (6) )
             WriteToTrace ("Continue(): IsValid() is true");
        if ( ! Invalidate(errlist) )
        {
            this->AppendContextInfo(__CONTEXT__,errlist,"ForwardRead::Continue() could not invalidate record space");
            return false;
        }
        m_BreakInfo = BreakInfo();
        this->AppendContextInfo(__CONTEXT__,errlist,"ForwardRead::Continue() call without break");
        return false;
    }

    if ( ! SetPosition ( m_BreakInfo.lastPno,
                         m_BreakInfo.lastOffset,
                         m_BreakInfo.lastAccessMode,
                         errlist ) )
    {
        this->AppendContextInfo(__CONTEXT__,errlist);
        m_BreakInfo = BreakInfo();
        (void)Invalidate(errlist);
        return false; // PTS 1115056 UH 2002-03-27 changed the assertion to return value
    }
    // assign the record space again
    bool result;
    (void) this->Deref(result,errlist);
    if ( ! result )
    {
        this->AppendContextInfo(__CONTEXT__,errlist);
        m_BreakInfo = BreakInfo();
        return false; // PTS 1115056 UH 2002-03-27 changed the assertion to return value
    }
    m_BreakInfo = BreakInfo();
    return true; // PTS 1115056 UH 2002-03-27 changed the assertion to return value
}

// ---------------------------------------------------------------------------
template <class PAGE>
inline PAGE& Data_ChainSplitSpaceForwardRead<PAGE>::Iterator::GetCurrentPage ()
{
    return m_RecordSpace.GetCurrentPage();
}

// ---------------------------------------------------------------------------
template <class PAGE>
inline 
typename Data_ChainSplitSpaceForwardRead<PAGE>::RecordSpace&
Data_ChainSplitSpaceForwardRead<PAGE>::Iterator::Deref(
                   bool                 &result,
                   SAPDBErr_MessageList &errlist)
{
    result = true;
    if ( ! m_RecordSpaceIsComplete )
    {
        result = AssignRecordspace (m_RecordIterator, errlist);
        if (! result)
            this->AppendContextInfo(__CONTEXT__,errlist);
    }
    return m_RecordSpace;
}

// ---------------------------------------------------------------------------
template <class PAGE>
inline Data_SplitRecordLength Data_ChainSplitSpaceForwardRead<PAGE>::Iterator::GetSize()
{
    // PTS 1117126 UH 2002-08-07 new
    if ( ! IsValid() )
        return 0;
    return m_RecordIterator.RestLength();
}

// ---------------------------------------------------------------------------
template <class PAGE>
inline bool Data_ChainSplitSpaceForwardRead<PAGE>::Iterator::PlusPlus(SAPDBErr_MessageList &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Data_ChainSplitSpaceForwardRead::Iterator::++", DataChain_Trace, 5);

    SAPDBERR_ASSERT_STATE( IsValid() );

    m_RecordSpaceIsComplete = false;

    // reorder and reassign the split record space
    if ( ! m_RecordSpace.Reassign(errlist))
    {
        this->AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }
    
    SAPDBERR_ASSERT_STATE( m_RecordSpace.CurrentPart() == 0 );
    
    PAGE& Page = m_RecordSpace.GetCurrentPage();
    Page.Use (m_RecordIterator);

    ++m_RecordIterator;
    
    if ( ! m_RecordIterator.IsValid() )
    {
        Data_PageNo NextPageNo = Page.NextPageNo();

        if ( NextPageNo.IsValid() )
        {
            const Data_PageNo auxPageNo = Page.PageNo();

            if ( ! m_PAM.ReleasePage(Page, errlist) )
            {
                this->AppendContextInfo(__CONTEXT__,errlist);
                return false;
            }

            if ( ! m_PAM.GetPage (Page, NextPageNo, errlist) )
            {
                errlist = errlist + Data_Exception(__CONTEXT__,
                                          DATA_CHAIN_IS_INCONSISTENT,
                                          SAPDB_ToString(auxPageNo),
                                          SAPDB_ToString(Page.PageNo()) );
                this->AppendContextInfo(__CONTEXT__,errlist);
                return false;
            }

            Page.Begin (m_RecordIterator);
        }
        else
        {
            if ( ! m_RecordSpace.Deassign (errlist) )
            {
                this->AppendContextInfo(__CONTEXT__,errlist);
                return false;
            }
        }
    }
    return true;
}

// ---------------------------------------------------------------------------
template <class PAGE>
void Data_ChainSplitSpaceForwardRead<PAGE>::Iterator::WriteToTrace (const char* title ) const
{
    Kernel_VTrace() << "forwReadChainIter: Recordspace: "
                    << (m_RecordSpaceIsComplete?"complete":"not complete");
    Kernel_VTrace() << "BreakInfo: " << m_BreakInfo.lastPno << "." << m_BreakInfo.lastOffset
                    << ", accessmode: " << m_BreakInfo.lastAccessMode;
    m_RecordSpace.WriteToTrace(title);
}

// ---------------------------------------------------------------------------
template <class PAGE>
void Data_ChainSplitSpaceForwardRead<PAGE>::Iterator::AppendContextInfo (
                        SAPDB_Char  const * const  filename,
                        SAPDB_UInt4 const          linenumber,
                        SAPDBErr_MessageList      &errlist,
                        SAPDB_Char  const * const  title ) const
{
    errlist = errlist + Data_Exception(filename,linenumber,DATA_BACK_CHAIN_ITER_INFO,
                                       (title!=0?title:"Data_ChainSplitSpaceForwardReadChain::Iterator"),
                                       (m_RecordSpaceIsComplete?"complete":"not complete"),
                                       SAPDB_ToString(m_BreakInfo.lastPno),
                                       SAPDB_ToString(m_BreakInfo.lastOffset),
                                       Data_AccessModeStrings[m_BreakInfo.lastAccessMode]);
    m_PAM.AppendContextInfo(filename,linenumber,errlist,title);
    m_RecordSpace.AppendContextInfo(filename,linenumber,errlist,title);
}

// ---------------------------------------------------------------------------
//
// IMPLEMENTATION of Data_ChainSplitSpaceForwardRead
//
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
template <class PAGE>
inline Data_ChainSplitSpaceForwardRead<PAGE>::Data_ChainSplitSpaceForwardRead (
                             Data_PageAccessManager  &PAM,
                             Data_PageId             &RootId)
: Data_Chain<PAGE> (PAM, RootId)
{}

// ---------------------------------------------------------------------------
template <class PAGE>
inline Data_ChainSplitSpaceForwardRead<PAGE>::Data_ChainSplitSpaceForwardRead (
                             Data_PageAccessManager  &PAM,
                             Data_PageId             &RootId,
                             Data_PageNo              LastPageNo)
: Data_Chain<PAGE> (PAM, RootId, LastPageNo)
{}

// ---------------------------------------------------------------------------
template <class PAGE>
inline bool Data_ChainSplitSpaceForwardRead<PAGE>::Create (
                     Data_ChainIterator<PAGE> &RootPageIter,
                     SAPDBErr_MessageList     &errlist)
{
    return Data_Chain<PAGE>::Create (RootPageIter,errlist); // PTS 1121659 UH 2003-04-30
}

// ---------------------------------------------------------------------------
template <class PAGE>
inline bool Data_ChainSplitSpaceForwardRead<PAGE>::Create (SAPDBErr_MessageList &errlist)
{
    Data_ChainIterator<PAGE> RootPageIter (this->GetPageAccessManager());
    return Create (RootPageIter,errlist); // PTS 1121659 UH 2003-04-30
}

// ---------------------------------------------------------------------------
template <class PAGE>
inline bool Data_ChainSplitSpaceForwardRead<PAGE>::ReserveSpace (
                          Data_SplitRecordLength  WantedSize,
                          Data_RecordLength       FirstPartMiniumSize,
                          Iterator               &RecordIterator,
                          SAPDB_UInt             &numberOfAddedPages,
                          SAPDBErr_MessageList   &errlist ) // PTS 1121659 UH 2003-04-30
{
    SAPDBTRACE_METHOD_DEBUG ("Data_ChainSplitSpaceForwardRead::ReserveSpace", DataChain_Trace, 5);
    
    SAPDBERR_ASSERT_STATE( WantedSize >= FirstPartMiniumSize );

    SAPDBTRACE_WRITELN (DataChain_Trace, 6, "Wanted: " << WantedSize << ", FirstPartMin: " << FirstPartMiniumSize);

    // PTS 1115593 UH 2002-05-07

    numberOfAddedPages = 0;
    // prepare the record space

    if ( ! RecordIterator.Invalidate(errlist) )
    {
        this->AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }

    if ( ! this->FindLastPageNo(errlist) )
    {
        this->AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }
        
    if ( ! RecordIterator.Assign( this->LastPageNo(), SAPDB_MAX_UINT2, Data_ForUpdate, errlist ) )
    {
        this->AppendContextInfo(__CONTEXT__,errlist);
        errlist = errlist + Data_Exception(__CONTEXT__,DATA_CHAIN_IS_INCONSISTENT,
                                           "-",
                                           SAPDB_ToString(this->LastPageNo()) );
        return false;
    }

    bool AllFits;
    bool FirstPartFits;
    
    RecordIterator.GetRecordSpace().GetCurrentPage().
        SpaceInfo (WantedSize, FirstPartMiniumSize, FirstPartFits, AllFits);
                                      
    if ( ! AllFits )
    {
        if(!RecordIterator.Invalidate(errlist))
        {
            this->AppendContextInfo(__CONTEXT__,errlist);
            return false;
        }
        if ( ! RecordIterator.Assign( this->LastPageNo(), SAPDB_MAX_UINT2, Data_ForStructureChange, errlist ) )
        {
            this->AppendContextInfo(__CONTEXT__,errlist);
            errlist = errlist + Data_Exception(__CONTEXT__,DATA_CHAIN_IS_INCONSISTENT,
                                               "-",
                                               SAPDB_ToString(this->LastPageNo()) );
            return false;
        }
    }
    
    // fill the internal record space of the record iterator

    Data_RecordLength   CurrentReservedSize = 0;
    SAPDB_Byte         *CurrentSpace;

    do
    {
        RecordIterator.GetRecordSpace().GetCurrentPage().
            ReserveSpace (WantedSize, FirstPartMiniumSize,
                          CurrentReservedSize, CurrentSpace);

        SAPDBTRACE_WRITELN (DataChain_Trace, 6, \
                            "ReserveSpace(" << RecordIterator.GetRecordSpace().CurrentPart() \
                            << "): pno/CurrReserved/CurrReserved: " \
                            << RecordIterator.GetRecordSpace().GetCurrentPage().PageNo() << "/" \
                            << CurrentReservedSize << "/" << CurrentReservedSize);

        if ( CurrentReservedSize > 0 )
        {
            SAPDBERR_ASSERT_STATE( CurrentSpace != NULL );
            RecordIterator.GetRecordSpace().SetPart (CurrentReservedSize, CurrentSpace);
        }

        if ( WantedSize > 0 )
        {
            RecordIterator.GetRecordSpace().SetNewPageWasAppended();
            
            if ( CurrentReservedSize > 0 )
            {
                // PTS 1117126 UH 2002-08-07
                // UseNextPart() calls Resize() which reallocates the memory
                // So before UseNextPart() no reference into the memory of Space
                // may be saved.
                RecordSpace     &Space             = RecordIterator.GetRecordSpace();
                const SAPDB_Int  partOfCurrentPage = Space.CurrentPart();

                if(!Space.UseNextPart(errlist))
                {
                    this->AppendContextInfo(__CONTEXT__,errlist);
                    return false;
                }

                PAGE            &NextPage          = Space.GetCurrentPage();
                PAGE            &CurrentPage       = Space.GetPage(partOfCurrentPage);

                if(!CurrentPage.Append (NextPage, errlist))
                {
                    this->AppendContextInfo(__CONTEXT__,errlist);
                    return false;
                }
                this->m_LastPageNo = NextPage.PageNo();
                NextPage.Begin(RecordIterator.m_RecordIterator);
            }
            else
            {
                // the space was not big enough for FirstPartMiniumSize
                // This is only entered at the first time once
                PAGE& CurrentPage = RecordIterator.GetRecordSpace().GetCurrentPage();
                // PTS 1133123 UH 2004-12-22 keep old last page
                PAGE& OldLastPage = RecordIterator.GetRecordSpace().GetPrevPage();
                
                OldLastPage.Assign (CurrentPage);
                if(!OldLastPage.Append (CurrentPage,errlist))
                {
                    this->AppendContextInfo(__CONTEXT__,errlist);
                    return false;
                }
                this->m_LastPageNo = CurrentPage.PageNo();
                
                CurrentPage.Begin(RecordIterator.m_RecordIterator);
            }
            ++numberOfAddedPages;
        }
    }
    while ( WantedSize > 0 );

    RecordIterator.SetAssigned (true);

    SAPDBTRACE_IF (DataChain_Trace, 6, RecordIterator.GetRecordSpace().WriteToTrace("ReserveSpace"));

    return true;
}


// ---------------------------------------------------------------------------
template <class PAGE>
inline void Data_ChainSplitSpaceForwardRead<PAGE>::UndoReserveSpace (const RecordSpace &space)
{
    // PTS 1117126 UH 2002-08-07 new
    // PTS 1133123 UH 2004-12-22 rewritten, input of iterator changed to space
    SAPDBTRACE_METHOD_DEBUG ("Data_ChainSplitSpaceForwardRead::UndoReserveSpace", DataChain_Trace, 5);
    this->m_LastPageNo = space.GetFirstPageNo();
}

// ---------------------------------------------------------------------------
template <class PAGE>
inline bool Data_ChainSplitSpaceForwardRead<PAGE>::Begin (
                   Iterator             &RecordIterator,
                   Data_AccessMode       AccessMode,
                   SAPDBErr_MessageList &errlist)
{
    if ( ! RecordIterator.Assign( this->RootId().PageNo(), 0, AccessMode, errlist) )
    {
        this->AppendContextInfo(__CONTEXT__,errlist);
        errlist = errlist + Data_Exception(__CONTEXT__,DATA_CHAIN_IS_INCONSISTENT,
                                  "-",SAPDB_ToString(this->RootId().PageNo()));
        return false;
    }
    return true;
}


#undef FIRST_FREE
#endif // Data_ChainSplitSpaceForwardRead_HPP
