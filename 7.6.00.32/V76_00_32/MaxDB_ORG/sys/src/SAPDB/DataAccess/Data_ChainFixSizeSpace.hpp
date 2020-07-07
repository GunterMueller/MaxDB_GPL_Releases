/*!
  @file           Data_ChainFixSizeSpace.hpp
  @author         UweH
  @brief          defines class Data_ChainFixSizeSpace

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
#ifndef Data_ChainFixSizeSpace_HPP
#define Data_ChainFixSizeSpace_HPP

#include "DataAccess/Data_Exceptions.hpp"
#include "DataAccess/Data_Messages.hpp"
#include "DataAccess/Data_PageFixSizeSpace.hpp"
#include "DataAccess/Data_Chain.hpp"

/// only to hide a simple boolean true
#define FIRST_RECORD true

/*!
   @class Data_ChainFixSizeSpace
   @brief A persistent page chain handler, based on data pages.
 */
template <class PAGE> class Data_ChainFixSizeSpace : public Data_Chain<PAGE>
{
public:
    /// abbreviation
    typedef SAPDB_Byte*                        RecordSpace;
    /// abbreviation
    typedef Data_PageFixSizeSpace::Iterator    RecordIterator;
    /// abbreviation
    typedef Data_ChainIterator<PAGE>           PageIterator;
    /// abbreviation
    typedef Data_PageFixSizeSpace::RecordCount RecordId;
public:
    /*!
        @class          Iterator
        @brief          Iterator for records in the chain.
     */
    class Iterator
    {
	/// The iterator accesses directly members of a chain it points into.
    friend class Data_ChainFixSizeSpace<PAGE>;
    public:
        /// The iterater is not initialized and at first invalid.
        Iterator (Data_PageAccessManager& PAM)
        : m_PAM        (PAM),
          m_PageIter   (PAM)
        {}
        /// This is used to copy given iterator
        Iterator (const Iterator& iter)
        : m_PAM        (iter.m_PAM),
          m_PageIter   (iter.m_PAM)
        {}
        /// Returns true, if the iterator points to a record.
        bool IsValid() const
        {
            return m_PageIter.IsValid()
                   &&
                   m_RecordIter.IsValid();
        }
        /*!
            @brief This invalidates all members.
            @param isOK [in] if set to false the pages are released as not changed
         */
        bool Invalidate(SAPDBErr_MessageList &errlist,
                        bool                  isOK = true)
        {
            m_RecordIter.Invalidate();
            if(!m_PageIter.Invalidate(errlist))
            {
                AppendContextInfo(__CONTEXT__,errlist);
                return false;
            }
            return true;
        }
        /// This returns a page reference of the position, where the iterator points to.
        void GetPosition (Data_PageNo &pno,
                          RecordId    &recordid)
        {
            // PTS 1115056 UH 2002-03-27 changed the assertion to return value
            if ( IsValid() )
            {
                pno      = (*m_PageIter).PageNo();
                recordid = m_RecordIter.GetRecordId();
            }
            else
            {
                pno      = Data_PageNo();
                recordid = 0;
            }
        }
        /*!
           @brief   The iterator is set to the given position.
           @returns true, if position is reached.
         */
        bool SetPosition (Data_PageNo           pno,
                          RecordId              recordid,
                          Data_AccessMode       accessmode,
                          SAPDBErr_MessageList &errlist)
        {
            if(!Assign (pno, true, accessmode, errlist))
            {
                AppendContextInfo(__CONTEXT__,errlist);
                return false;
            }
            if ( ! m_PageIter.IsValid() )
            {
                AppendContextInfo(__CONTEXT__,errlist);
                return false;
            }
            (*m_PageIter).SetToRecord (recordid, m_RecordIter);
            if(!m_RecordIter.IsValid())
            {
                AppendContextInfo(__CONTEXT__,errlist);
                return false;
            }
            return true;
        }
        /// The iterator releases its resources and saves current position.
        bool Break(SAPDBErr_MessageList &errlist)
        {
            GetPosition ( m_BreakInfo.lastPno,
                          m_BreakInfo.lastrecordid );
            m_BreakInfo.lastAccessMode = GetPage().AccessMode();
            if ( ! Invalidate(errlist) )
            {
                AppendContextInfo(__CONTEXT__,errlist);
                return false;
            }
            return true;
        }
        /*!
           @brief   The iterator takes and sets the saved position.
           @returns (bool) false, if the saved position could not be set.
         */
        bool Continue(SAPDBErr_MessageList &errlist)
        {
            bool result = SetPosition ( m_BreakInfo.lastPno,
                                        m_BreakInfo.lastrecordid,
                                        m_BreakInfo.lastAccessMode,
                                        errlist );
            if ( ! result )
                AppendContextInfo(__CONTEXT__,errlist);
            m_BreakInfo = BreakInfo();
            return result;
        }
        /// Gives the current page the iterator points to. The iterator must be valid.
        PageIterator& GetPageIterator()
        {
            return m_PageIter;
        }
        /// Gives the current page the iterator points to. The iterator must be valid.
        PAGE& GetPage()
        {
            SAPDBERR_ASSERT_STATE( m_PageIter.IsValid() );
            return *m_PageIter;
        }
        /// returns SAPDB_UInt, MAX_UINT if invalid
        RecordId GetRecordId () const
        {
            if ( ! IsValid() )
                return SAPDB_MAX_UINT4;
            return m_RecordIter.GetRecordId();
        }
        /// This sets the internal recordid of the iterator, which can then be dereferenced.
        void SetRecordId (RecordId recordid)
        {
            SAPDBERR_ASSERT_STATE( m_PageIter.IsValid() );
            m_RecordIter.SetRecordId(recordid);
        }
        /// This returns the record, which is currently referenced.
        RecordSpace operator * ()
        {
            SAPDBERR_ASSERT_STATE( IsValid() );
            return *m_RecordIter;
        }
        /// If the iterator is valid it is set to the next record.
        bool PlusPlus(SAPDBErr_MessageList &errlist)
        {
            if( ! IsValid() )
                return true;

            ++m_RecordIter;
            
            if ( ! m_RecordIter.IsValid() )
            {
                m_RecordIter.Invalidate();

                if(!m_PageIter.PlusPlus(errlist))
                {
                    AppendContextInfo(__CONTEXT__,errlist);
                    return false;
                }

                if ( m_PageIter.IsValid() )
                    (*m_PageIter).Begin (m_RecordIter);
            }
            return true;
        }
        /// If the iterator is valid it is set to the previous record.
        bool MinusMinus(SAPDBErr_MessageList &errlist)
        {
            if( ! IsValid() )
                return true;

            --m_RecordIter;
            
            if ( ! m_RecordIter.IsValid() )
            {
                if(!m_PageIter.MinusMinus(errlist))
                {
                    AppendContextInfo(__CONTEXT__,errlist);
                    return false;
                }

                if ( m_PageIter.IsValid() )
                    (*m_PageIter).Begin (m_RecordIter);
            }
            return true;
        }
        /// Write all important data to the knltrace.
        void WriteToTrace (const char* title = 0)
        {
            Kernel_VTrace trace;
            if ( title != 0 )
                trace << title << ": ";
            else
                trace << "fixiter: ";

            if ( m_PageIter.IsValid() )
                trace << "@" << (*m_PageIter).PageNo();
            else
                trace << "@invalid";
                
            trace << "." << m_RecordIter.GetPosition()
                  << "[" << m_RecordIter.GetRecordId() << "]";
            trace << ", break@" << m_BreakInfo.lastPno << "["
                  << m_BreakInfo.lastrecordid << "]";
            trace << (!IsValid()?" (not valid)":"");
        }
        void AppendContextInfo (SAPDB_Char  const * const  filename,
                                SAPDB_UInt4 const          linenumber,
                                SAPDBErr_MessageList      &errlist,
                                SAPDB_Char  const * const  title = 0) const
        {
            errlist = errlist +
                      Data_Exception(filename,linenumber,DATA_CHAIN_FIX_ITER_INFO,
                                     (title!=0?title:"Data_FixSizeSpace::Iterator"),
                                     (IsValid()?"valid":"not valid"),
                                     SAPDB_ToString(m_RecordIter.GetPosition()),
                                     SAPDB_ToString(m_RecordIter.GetRecordId()),
                                     SAPDB_ToString(m_BreakInfo.lastPno),
                                     SAPDB_ToString(m_BreakInfo.lastrecordid));
            (*m_PageIter).AppendContextInfo(__CONTEXT__,errlist,"page");
        }
    protected:
        /// this is used by the container. if an error occurred, the iterator is invalid.
        bool Assign (Data_PageNo           PageNo,
                     bool                  SetToFirstRecord,
                     Data_AccessMode       AccessMode,
                     SAPDBErr_MessageList &errlist)
        {
            if ( m_PageIter.IsValid() )
            {
                if ( PageNo != (*m_PageIter).PageNo()
                     ||
                     (*m_PageIter).AccessMode() != AccessMode )
                {
                    if(!Invalidate(errlist))
                        return false;
                    if ( ! m_PAM.GetPage ( (*m_PageIter),
                                           PageNo,
                                           (*m_PageIter).RecoveryMode(),
                                           AccessMode,
                                           errlist) )
                    {
                        Invalidate(errlist);
                        return false;
                    }
                }
            }
            else
            {
                if ( ! m_PAM.GetPage ( (*m_PageIter),
                                       PageNo,
                                       (*m_PageIter).RecoveryMode(),
                                       AccessMode,
                                       errlist) )
                {
                    Invalidate(errlist);
                    return false;
                }
            }
            if ( m_PageIter.IsValid() )
            {
                if ( SetToFirstRecord )
                    (*m_PageIter).Begin (m_RecordIter);
                else
                    (*m_PageIter).End (m_RecordIter);
            }
            return true;
        }
    private:
        /// the page access manager handle
        Data_PageAccessManager& m_PAM;
        /// internal record iterator
        RecordIterator m_RecordIter;
        /// internal page iterator
        PageIterator m_PageIter;
    private:
        /// definition of a break handle
        struct BreakInfo
        {
            /// continue on this page
            Data_PageNo     lastPno;
            /// continue at this record
            RecordId        lastrecordid;
            /// continue to get the page with this access mode
            Data_AccessMode lastAccessMode;
            /// construct an invalid breakinfo
            BreakInfo()
            {
                lastPno.Invalidate();
                lastrecordid   = 0;
                lastAccessMode = Data_ForRead;
            }
        };
        /// this is the iterators break info
        BreakInfo m_BreakInfo;
    };
private:
    /// Checks if the given and assumed fix space size is identical to that written into the root page.
    bool CheckFixRecordLength( const PageIterator   &iter,
                               SAPDBErr_MessageList &errlist)
    {
        if ( ! iter.IsValid() )
            return true;
            
        if ( (*iter).GetFixRecordLength() != m_FixRecordLength )
        {
            errlist = Data_Exception(__CONTEXT__, DATA_ERROR_CHAIN_FIX_SIZE_MISMATCH,
                                     SAPDB_ToString(m_FixRecordLength),
                                     SAPDB_ToString((*iter).GetFixRecordLength()));
            this->AppendContextInfo(__CONTEXT__,errlist);
            return false;
        }
        return true;
    }
public:
    /*!
       @param          PAM [in/out] must be used to access pages
       @param          RootId [in] this id defines the beginning of the page chain
       @param          FixRecordLength [in] 
       @brief          if the rootid is valid, the given fix-record-size is checked.
     */
    Data_ChainFixSizeSpace (Data_PageAccessManager &PAM,
                            Data_PageId             &RootId,
                            Data_RecordLength        FixRecordLength)
    : Data_Chain<PAGE>  (PAM, RootId),
      m_FixRecordLength (FixRecordLength)
    {}
    /*!
       @brief          if the rootid is valid, the given fix-record-size is checked.
       @param          PAM [in/out]  must be used to access pages
       @param          RootId [in] this id defines the beginning of the page chain
       @param          LastPageNo [in] this id defines the ending of the page chain
       @param          FixRecordLength [in] 
     */
    Data_ChainFixSizeSpace (Data_PageAccessManager &PAM,
                            Data_PageId             &RootId,
                            Data_PageNo              LastPageNo,
                            Data_RecordLength        FixRecordLength)
    : Data_Chain<PAGE>  (PAM, RootId, LastPageNo),
      m_FixRecordLength (FixRecordLength)
    {}
    /// A new Chain is created. The PageIter points to the root.
    bool Create (PageIterator         &RootPageIter,
                 SAPDBErr_MessageList &errlist)
    {
        if ( ! Data_Chain<PAGE>::Create (RootPageIter,errlist) )
            return false; // PTS 1121659 UH 2003-04-30
        
        PAGE &page = *RootPageIter;
        
        page.SetFixRecordLength ( m_FixRecordLength );
        page.SetLastPageNo      ( page.PageNo()     );
        return true; // PTS 1121659 UH 2003-04-30
    }
    /// A new Chain is created.
    bool Create (SAPDBErr_MessageList &errlist)
    {
        PageIterator RootPageIter (this->GetPageAccessManager());
        return Create (RootPageIter,errlist); // PTS 1121659 UH 2003-04-30
    }
    /// The RecordIterator is set to the first record of the given page.
    bool Set (Iterator&             iter,
              Data_PageNo           pageno,
              Data_AccessMode       accessmode,
              bool                  toFirstRecord,
              SAPDBErr_MessageList &errlist)
    {
        if ( ! iter.Assign( pageno, toFirstRecord, accessmode, errlist) )
            return false;
        #ifdef SAPDB_SLOW
        if ( ! CheckFixRecordLength(iter.GetPageIterator(), errlist) )
        {
            iter.Invalidate(errlist);
            return false;
        }
        #endif
        return true;
    }
    /// The RecordIterator is set to the first record in the chain.
    bool Begin (Iterator&             iter,
                Data_AccessMode       accessmode,
                SAPDBErr_MessageList &errlist)
    {
        return Set ( iter, this->RootId().PageNo(), accessmode, true, errlist);
    }
    /*!
       @param          iter [out] A RecordIterator, which points to the newly created space.
       @param          useForStructureChange [in] default is true
       @brief          Space is reserved in the chain.

       The record iterator points to the new space.
       If no space was allocated, the iterator is invalid.
       The Chain is locked here but must explicitely Unlock()'ed after the
       space has been written !!
     */
    bool ReserveSpace (Iterator             &iter,
                       SAPDBErr_MessageList &errlist,
                       bool                  useForStructureChange = true)
    {
        if ( ! this->Lock (useForStructureChange?Data_ForStructureChange:Data_ForUpdate,errlist) )
        {
            this->AppendContextInfo(__CONTEXT__,errlist);
            errlist = errlist + Data_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                       "Data_ChainFixSizeSpace::ReserveSpace: Lock()");
            return false;
        }

        if(!this->FindLastPageNo(errlist))
        {
            this->AppendContextInfo(__CONTEXT__,errlist);
            return false;
        }
            
        PageIterator &pageiter = iter.m_PageIter;
        
        if ( ! GetPage (this->LastPageNo(), Data_ForUpdate, pageiter, errlist) )
        {
            this->AppendContextInfo(__CONTEXT__,errlist);
            return false;
        }
        
        (*pageiter).ReserveSpace (iter.m_RecordIter);
        
        if ( ! iter.IsValid() )
        {
            PAGE newPage;
                        
            if ( !(*pageiter).Append (newPage, errlist) )
            {
                this->AppendContextInfo(__CONTEXT__,errlist);
                return false;
            }
            this->m_LastPageNo = newPage.PageNo();
            newPage.SetFixRecordLength (m_FixRecordLength);

            if(!ReleasePage (pageiter,errlist))
            {
                this->AppendContextInfo(__CONTEXT__,errlist);
                return false;
            }
            
            if(!pageiter.Set(newPage,errlist))
            {
                this->AppendContextInfo(__CONTEXT__,errlist);
                return false;
            }

            (*pageiter).ReserveSpace (iter.m_RecordIter);
            
            if ( this->m_RootPage.IsAssigned() )
            {
                this->m_RootPage.SetLastPageNo ((*pageiter).PageNo());
                this->m_RootPage.SetChanged();
            }
        }

        // PTS 1127083 UH 2004-01-14 removed Unlock()

        return true; // PTS 1121659 UH 2003-04-30
    }
    /*!
       @param iter [in] A RecordIterator, which points to the space to remove.
       @brief Space is removed in the chain.

       The iterator is set to the next valid position, if there is one.
       A page is only removed, if it is not the root or the last page.
     */
    bool RemoveSpace (Iterator             &iter,
                      SAPDBErr_MessageList &errlist)
    {
        PAGE &page = iter.GetPage();

        SAPDBERR_ASSERT_STATE( page.GetRecordCount() > 0 );

        page.DecrementRecordCount();

        if ( page.GetRecordCount() > 0
             ||
             this->RootId().PageNo() == page.PageNo() )
        {
            if(!iter.PlusPlus(errlist))
            {
                this->AppendContextInfo(__CONTEXT__,errlist);
                return false;
            }
        }
        else
        {
            PageIterator &pageiter = iter.GetPageIterator();

            Data_PageNo prevPageNo = page.PrevPageNo();

            if ( prevPageNo.IsInvalid() )
            {
                errlist = Data_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                           "Data_ChainFixSizeSpace::Removespace: prev pno is invalid.");
                this->AppendContextInfo(__CONTEXT__,errlist);
                return false;
            }

            if(!pageiter.Invalidate(errlist))
            {
                this->AppendContextInfo(__CONTEXT__,errlist);
                return false;
            }

            if ( ! this->Lock (Data_ForStructureChange,errlist) )
            {
                this->AppendContextInfo(__CONTEXT__,errlist);
                return false;
            }
            
            if ( ! GetPage(prevPageNo, Data_ForUpdate, pageiter, errlist) )
            {
                errlist = errlist + Data_Exception(__CONTEXT__,DATA_CHAIN_IS_INCONSISTENT,
                                                  SAPDB_ToString(prevPageNo),
                                                  SAPDB_ToString((*pageiter).PageNo()) );
                this->AppendContextInfo(__CONTEXT__,errlist);
                return false;
            }

            if(!Delete (pageiter,errlist))
            {
                this->AppendContextInfo(__CONTEXT__,errlist);
                return false;
            }

            if ( this->m_RootPage.IsAssigned() )
            {
                Data_PageNo NextPageNo = (*pageiter).NextPageNo();
                if(!ReleasePage (pageiter,errlist))
                {
                    this->AppendContextInfo(__CONTEXT__,errlist);
                    return false;
                }
                if ( NextPageNo.IsValid() )
                    (void) GetPage (NextPageNo, Data_ForUpdate, pageiter, errlist);
            }
            else
            {
                if(!pageiter.PlusPlus(errlist))
                {
                    this->AppendContextInfo(__CONTEXT__,errlist);
                    return false;
                }
            }

            if ( ! pageiter.IsValid() )
            {
                // The removed page was the last page in the chain.

                if ( this->m_RootPage.IsAssigned() )
                {
                    this->m_RootPage.SetLastPageNo (prevPageNo);
                    this->m_RootPage.SetChanged();
                }

                if(!iter.Invalidate(errlist))
                {
                    this->AppendContextInfo(__CONTEXT__,errlist);
                    return false;
                }
            }
            else
            {            
                // The page iter points now to the next page after the removed page.

                (*pageiter).SetPrevPageNo(prevPageNo);
                (*pageiter).Begin (iter.m_RecordIter);
            }
            if(!this->UnLock(errlist))
            {
            	this->AppendContextInfo(__CONTEXT__,errlist);
                return false;
            }
        }
        return true;
    }
    /*!
        @brief  This returns the actual used fix space size
        @return size in bytes
     */
    Data_RecordLength GetSpaceSize () const
    {
        return m_FixRecordLength;
    }
private:
    /// the internal cached fix space size
    const Data_RecordLength m_FixRecordLength;
};
#undef FIRST_RECORD
#endif // Data_ChainFixSizeSpace_HPP
