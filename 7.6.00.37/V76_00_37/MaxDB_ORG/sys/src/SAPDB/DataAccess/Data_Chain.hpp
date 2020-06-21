/*!
  @file           Data_Chain.hpp
  @author         UweH
  @brief          class Data_Chain is defined here

  There is only a update iterator, but a readonly iterator may be usefull.

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
#ifndef Data_Chain_HPP
#define Data_Chain_HPP

#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "KernelCommon/Kernel_VTrace.hpp"
#include "DataAccess/Data_Types.hpp"
#include "DataAccess/Data_Exceptions.hpp"
#include "DataAccess/Data_Messages.hpp"
#include "DataAccess/Data_ChainPage.hpp"
#include "DataAccess/Data_PageAccessManager.hpp"
#include "RunTime/RTE_Message.hpp"

/*!
   @interface      Data_IBreakable
   @brief          This is a breakable resource.
 */
class Data_IBreakable // PTS 1114994 UH 2002-03-22 new
{
public:
    /// true is returned if successfull
    virtual bool Break(SAPDBErr_MessageList &errlist) = 0;
    /// true is returned if successfull. All resources are allocated like before Break().
    virtual bool Continue(SAPDBErr_MessageList &errlist) = 0;
    /// Writes relevant contents into vtrace.
    virtual void WriteToTrace (const char* title = NULL) const = 0;
    /// append some info to the errlist
    virtual void AppendContextInfo (SAPDB_Char  const * const  filename,
                                    SAPDB_UInt4 const          linenumber,
                                    SAPDBErr_MessageList      &errlist,
                                    SAPDB_Char  const * const  title = 0) const = 0;
};

/*!
   @class          Data_ChainIterator
   @brief          This cannot be defined within Data_Chain.
 */
template <class T> class Data_ChainIterator
{
public:
    /*!
       @brief The iterater is initialized.
       
       It points to the root of the given chain.
       If the chain is not created, the iterator is invalid.
     */
    Data_ChainIterator<T> (Data_PageAccessManager &PAM)
    : m_PAM(PAM)
    {}
    /*!
       @returns        true, if the iterator was successfully set to the given PageNo.
       @brief          The iterator is set to a new position.
                       If PageNo is invalid the iterator becomes invalid and true is returned.
     */
    bool Set (const Data_PageNo           &PageNo,
              const Data_PageRecoveryMode &RecoveryMode,
              Data_AccessMode              AccessMode,
              SAPDBErr_MessageList        &errlist)
    {
        if ( m_Page.IsAssigned() )
        {
            if ( m_Page.PageNo() == PageNo
                 &&
                 m_Page.RecoveryMode() == RecoveryMode
                 &&
                 m_Page.AccessMode() == AccessMode )
            {
                return true;
            }
            if ( ! m_PAM.ReleasePage (m_Page, errlist) )
            {
                m_Page.AppendContextInfo(__CONTEXT__,errlist);
                return false;
            }
        }
        if ( PageNo.IsInvalid() )
            return true;
        return m_PAM.GetPage (m_Page, PageNo, RecoveryMode, AccessMode, errlist);
    }
    /*!
       @brief The iterator is set to the given Page.
       
       If the iterator pointed to a page this was released before.
       An exception DATA_PAM_ERROR might be thrown.
     */
    bool Set (T                    &Page,
              SAPDBErr_MessageList &errlist)
    {
        if ( m_Page.IsAssigned()
             &&
             ! m_PAM.ReleasePage (m_Page, errlist) )
             return false;

        m_Page.Assign (Page);
        return true;
    }
    /*!
       @brief The iterator is set to the clone of the given Page.
       
       If the iterator pointed to a page this was released before.
     */
    bool Clone (T                    &Page,
                SAPDBErr_MessageList &errlist)
    {
        if ( m_Page.IsAssigned()
             &&
             ! m_PAM.ReleasePage (m_Page, errlist) )
             return false;

        m_Page.Clone(Page);
        return true;
    }
    /// This returns the Page, which is currently referenced. const
    T& operator * ()
    {
        return m_Page;
    }
    /// This returns the Page, which is currently referenced.
    const T& operator * () const
    {
        return m_Page;
    }
    /*!
       @brief The iterator is set to the next page in the chain.
       
       After the operation, IsValid() may return false.
       This indicates the end of the chain.
     */
    /*
    Data_ChainIterator<T>& operator ++ ()
    {
        SAPDBErr_MessageList errlist;
        if ( !PlusPlus(errlist) )
            RTE_Message(errlist);
        return *this;
    }
    */
    bool PlusPlus (SAPDBErr_MessageList &errlist)
    {
        const Data_PageNo NextPageNo = m_Page.NextPageNo();
        
        if ( ! m_PAM.ReleasePage (m_Page, errlist) )
            return false;

        if ( NextPageNo.IsValid()
             &&
             ! m_PAM.GetPage (m_Page,NextPageNo,m_Page.RecoveryMode(),m_Page.AccessMode(),errlist) )
        {
            return false;
        }
        return true;
    }
    /*!
       @brief The iterator is set to the prev page in the chain.
       
       After the operation, IsValid() may return false.
       This indicates the end of the chain.
     */
    /*
    Data_ChainIterator<T>& operator -- ()
    {
        SAPDBErr_MessageList errlist;
        if ( !MinusMinus(errlist) )
            RTE_Message(errlist);
        return *this;
    }
    */
    bool MinusMinus (SAPDBErr_MessageList &errlist)
    {
        const Data_PageNo PrevPageNo = m_Page.PrevPageNo();

        if ( ! m_PAM.ReleasePage (m_Page, errlist) )
            return false;

        if ( PrevPageNo.IsValid()
             &&
             ! m_PAM.GetPage (m_Page,PrevPageNo,m_Page.RecoveryMode(),m_Page.AccessMode(),errlist) )
        {
            return false;
        }
        return true;
    }
    /// returns true, if it can be dereferenced.
    bool IsValid() const
    {
        return m_Page.IsAssigned();
    }
    /// This releases all allocated internal members.
    bool Invalidate(SAPDBErr_MessageList &errlist)
    {
        if ( m_Page.IsAssigned()
             &&
             ! m_PAM.ReleasePage (m_Page, errlist) )
            return false;
        return true;
    }

private:

    /// This identifier is the main entry point to a persistent page chain.
    T m_Page;
    /// This is used to access pages.
    Data_PageAccessManager& m_PAM;
};

/// A persistent page chain handler, based on data pages. It is a double linked chain.
template <class T> class Data_Chain
{
public:
    /*!
       @param PAM    [in/out] This is the used page access manager
       @param RootId [in] this id defines the beginning of the page chain
       @brief If the RootId is invalid, than a new chain can be created.
       
       The RootId is saved internally for accessing the chain.
     */
    Data_Chain (Data_PageAccessManager &PAM,
                Data_PageId            &RootId)
    : m_PAM    (PAM),
      m_RootId (RootId)
    {}
    /*!
       @param PAM        [in/out] This is the used page access manager
       @param RootId     [in] this id defines the beginning of the page chain
       @brief If the RootId is invalid, than a new chain can be created.
       
       The RootId is saved internally for accessing the chain.
     */
    Data_Chain (Data_PageAccessManager &PAM,
                Data_PageId             &RootId,
                Data_PageNo              LastPageNo)
    : m_PAM        (PAM),
      m_RootId     (RootId),
      m_LastPageNo (LastPageNo)
    {}
    /// Resets the internal members.
    void Invalidate()
    {
        m_RootId.Invalidate();
        m_LastPageNo.Invalidate();
    }
    /// returns true if the rootid is valid.
    bool IsCreated() const
    {
        return m_RootId.IsValid();
    }
    /*!
       @param         Iter [out] This iterator points after to the root page.
       @param         errlist [out]
       @brief         A new page chain is created now. The Iterator points to root page.

       Precondition:  The RootId must not be valid
       Postcondition: RootId is valid and LastPageNo is equal to RootId.
       A new root page is created and the Id is stored internally.
       An exception DATA_PAM_ERROR might be thrown, if no Root page could be created.
     */
    bool Create (Data_ChainIterator<T> &Iter,
                 SAPDBErr_MessageList  &errlist)
    {
        if ( IsCreated() )
            return false;

        if ( ! m_PAM.NewPage (*Iter, m_RootId.PageRecoveryMode(), errlist) )
            return false;

        (*Iter).PageId       (m_RootId);
        (*Iter).SetLastPageNo(m_RootId.PageNo());

        m_LastPageNo = m_RootId.PageNo();

        return true;
    }
    /*!
       @brief All pages in the chain are dropped.
       
       There is no synchronization with the savepoint.
       If the RootId is invalid the drop does not fail.
       An exception DATA_PAM_ERROR might be thrown, if the chain is inconsistent.
     */
    bool Drop (SAPDBErr_MessageList &errlist)
    {
        if ( ! IsCreated() )
            return true;
            
        if ( m_RootPage.IsAssigned() )
            if(!UnLock(errlist))
                return false;

        Data_ChainIterator<T> Iter (m_PAM);
        Data_PageNo           NextPageNo;
        bool                  result = true;

        result = Iter.Set (m_RootId.PageNo(), m_RootId.PageRecoveryMode(),
                           Data_ForUpdate, errlist);
        if ( ! result )
            AppendContextInfo(__CONTEXT__,errlist);
        else
        {
            while ( Iter.IsValid() )
            {
                NextPageNo = (*Iter).NextPageNo ();
                if ( ! m_PAM.FreePage (*Iter, errlist) )
                {
                    RTE_Message(errlist);
                    errlist.ClearMessageList();
                }
                if ( NextPageNo.IsValid()
                     &&
                     ! Iter.Set (NextPageNo, m_RootId.PageRecoveryMode(), Data_ForUpdate, errlist) )
                {
                    result = false;
                    AppendContextInfo(__CONTEXT__,errlist);
                    break;
                }
            }
        }

        Invalidate();

        return result;
    }
    /*!
       @brief The iterator is set to the root page of the chain.
       
       If the Iterator is invalid after the operation, the chain was not created.
     */
    bool Begin (Data_ChainIterator<T> &Iter,
                Data_AccessMode        AccessMode,
                SAPDBErr_MessageList  &errlist)
    {
        if ( Iter.IsValid() )
        {
            if ( m_RootId.PageNo() != (*Iter).PageNo()
                 &&
                 ! Iter.Set (m_RootId.PageNo(),
                             m_RootId.PageRecoveryMode(),
                             AccessMode,
                             errlist ) )
            {
                AppendContextInfo(__CONTEXT__,errlist);
                return false;
            }
            return true;
        }
        if ( ! Iter.Set ( m_RootId.PageNo(),
                          m_RootId.PageRecoveryMode(),
                          AccessMode,
                          errlist ) )
        {
            AppendContextInfo(__CONTEXT__,errlist);
            return false;
        }
        return true;
    }
    /*!
       @brief The iterator is set to the last page of the chain.
       
       If the Iterator is invalid after the operation, the chain was not created.
     */
    bool End (Data_ChainIterator<T> &Iter,
              Data_AccessMode        AccessMode,
              SAPDBErr_MessageList  &errlist)
    {
        if ( ! FindLastPageNo(errlist) )
            return false;
            
        if ( Iter.IsValid()
             &&
             m_LastPageNo == (*Iter).PageNo()
             &&
             AccessMode == (*Iter).AccessMode() )
            return true;

        if ( ! Iter.Set ( m_LastPageNo,
                          m_RootId.PageRecoveryMode(),
                          AccessMode,
                          errlist ) )
        {
            AppendContextInfo(__CONTEXT__,errlist);
            return false;
        }
        return true;
    }
    /*!
       @brief A new page is inserted after the current page the iterator points to.
       
       After the operation, the iterator points to the new page.
     */
    bool Insert (Data_ChainIterator<T> &Iter,
                 SAPDBErr_MessageList  &errlist)
    {
        SAPDBERR_ASSERT_STATE( (*Iter).AccessMode() == Data_ForStructureChange );

        T NewPage;

        if ( ! (*Iter).Append (NewPage, errlist) )
        {
            AppendContextInfo(__CONTEXT__,errlist);
            return false;
        }
        if ( ! (*Iter).Deassign(errlist) )
        {
            AppendContextInfo(__CONTEXT__,errlist);
            return false;
        }
        (*Iter).Assign (NewPage);
        if ( (*Iter).NextPageNo().IsInvalid() )
            m_LastPageNo = (*Iter).PageNo();
        return true; // PTS 1121659 UH 2003-04-30
    }
    /// The page after the page referenced by the iterator is removed.
    /// If next page is nil nothing is removed.
    bool Delete (Data_ChainIterator<T> &Iter,
                 SAPDBErr_MessageList  &errlist)
    {
        SAPDBERR_ASSERT_STATE( (*Iter).AccessMode() != Data_ForRead );
                               
        if ( ! (*Iter).NextPageNo().IsValid() )
            return true;

        // THIS->DEL->NEXT

        T PageToDelete;

        if ( ! m_PAM.GetPage ( PageToDelete,
                               (*Iter).NextPageNo(),
                               (*Iter).RecoveryMode(),
                               Data_ForUpdate,
                               errlist ) )
        {
            AppendContextInfo(__CONTEXT__,errlist);
            return false;
        }

        // THIS->NEXT

        (*Iter).SetNextPageNo (PageToDelete.NextPageNo());

        if ( (*Iter).NextPageNo().IsInvalid() )
            m_LastPageNo = (*Iter).PageNo();

        if ( ! m_PAM.FreePage(PageToDelete, errlist) )
        {
            RTE_Message(errlist);
            errlist.ClearMessageList();
        }
        return true;
    }
    /// The rootid of the current chain is returned.
    const Data_PageId& RootId() const
    {
        return m_RootId;
    }
    /// The currently known last page no is returned.
    Data_PageNo LastPageNo() const
    {
        return m_LastPageNo;
    }
	/// This determines the last pageno of the chain and set the internal member.
    bool FindLastPageNo(SAPDBErr_MessageList &errlist,
                        bool                  forceReadAllPages = false)
    {
        if ( ! IsCreated() )
        {
            m_LastPageNo.Invalidate();
            return true;
        }

        if ( ! forceReadAllPages
             &&
             m_LastPageNo.IsValid() )
        {
            return true;
        }

        Data_ChainIterator<T> iter (m_PAM);

        if ( ! GetPage (m_RootId.PageNo(), Data_ForRead, iter, errlist) )
        {
            if ( forceReadAllPages )
                errlist = Data_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                          "Data_Chain::LastPageNo(): root not found / forced read");
            else
                errlist = Data_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                          "Data_Chain::LastPageNo(): root not found");
            AppendContextInfo(__CONTEXT__,errlist);
            return false;
        }

        if ( ! forceReadAllPages )
            m_LastPageNo = (*iter).LastPageNo();
        
        if ( m_LastPageNo.IsInvalid()
             &&
             (*iter).NextPageNo().IsValid() )
        {
            do
            {
                m_LastPageNo = (*iter).NextPageNo();
                if ( ! iter.PlusPlus(errlist) )
                {
                    if ( forceReadAllPages )
                    	errlist = Data_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                                  "Data_Chain::LastPageNo(): chain is corrupt / forced read");
                    else
                    	errlist = Data_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                                  "Data_Chain::LastPageNo(): chain is corrupt");
                    AppendContextInfo(__CONTEXT__,errlist);
                    return false;
                }
                if ( ! iter.IsValid() )
                {
                    if ( forceReadAllPages )
                    	errlist = Data_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                                  "Data_Chain::LastPageNo(): chain is corrupt / forced read");
                    else
                    	errlist = Data_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                                  "Data_Chain::LastPageNo(): chain is corrupt");
                    AppendContextInfo(__CONTEXT__,errlist);
                    return false;
                }
            }
            while ( (*iter).NextPageNo().IsValid() );
        }
        if ( m_LastPageNo.IsInvalid() )
            m_LastPageNo = m_RootId.PageNo();
        
        return ReleasePage (iter, errlist);
    }
    /*!
        @brief The internally stored LastPageNo is saved within the root page.
        
        Be carefull, if the internal last page no is not known it is determined !
     */
    bool StoreLastPageNo(SAPDBErr_MessageList &errlist)
    {
        Data_ChainIterator<T> rootIter (m_PAM);

        if ( ! GetPage (m_RootId.PageNo(),Data_ForUpdate,rootIter,errlist) )
        {
            AppendContextInfo(__CONTEXT__,errlist,"Data_Chain::StoreLastPageNo");
            return false;
        }
        
        if ( m_LastPageNo.IsInvalid()
             &&
             ! FindLastPageNo(errlist, true) )
            return false;

        (*rootIter).SetLastPageNo (m_LastPageNo);

        return ReleasePage(rootIter, errlist);
    }
    /// This returns  the internally used page access manager handler.
    Data_PageAccessManager& GetPageAccessManager()
    {
        return m_PAM;
    }
    /*!
        @brief The page chain is truncated from the given page no on until the end.
        
        The given page no identifies the new last page of the chain.
        All pages after that page are freed.
        The last page no in the root is updated too.
        PRE: The root page and all pages from given pno until the chains end
             must no be in access by the caller.
     */
    bool Truncate (Data_PageNo           newLastPageNo,
                   Data_PageOffset       newFirstFree,
                   SAPDBErr_MessageList &errlist)
    {
        SAPDBTRACE_METHOD_DEBUG ("Data_Chain::Truncate", DataChain_Trace, 5);

        if ( newLastPageNo.IsInvalid() )
        {
            errlist = Data_Exception(__CONTEXT__,DATA_CHAIN_TRUNCATE_ERROR,
                                     "invalid newLastPageNo",
                                     SAPDB_ToString(newLastPageNo),
                                     SAPDB_ToString(newFirstFree) );
            AppendContextInfo(__CONTEXT__,errlist);
            return false;
        }
        
        T rootPage;
        
        if ( ! m_PAM.GetPage ( rootPage,
                               m_RootId.PageNo(),
                               m_RootId.PageRecoveryMode(),
                               Data_ForStructureChange,
                               errlist) )
        {
            AppendContextInfo(__CONTEXT__,errlist,"Data_Chain::Truncate(): root not found");
            return false;
        }

        SAPDBTRACE_WRITELN (DataChain_Trace, 6, "rootPageNo: " << m_RootId.PageNo());
        
        // PTS 1111584 TS 2001-08-30 begin

        Data_ChainIterator<T>   iter (m_PAM);
        Data_PageNo             deletePageNo;

        if ( (rootPage.NextPageNo().IsInvalid() )
             ||
             (rootPage.PageNo() == newLastPageNo) )
        {
            if ( newLastPageNo != rootPage.PageNo() )
            {
                errlist = Data_Exception(__CONTEXT__,DATA_CHAIN_TRUNCATE_ERROR,
                                         "invalid newLastPageNo",
                                         SAPDB_ToString(newLastPageNo),
                                         SAPDB_ToString(newFirstFree) );
                AppendContextInfo(__CONTEXT__,errlist);
                return false;
            }
            if ( newFirstFree > 0 )
                rootPage.SetFirstFreeOffset (newFirstFree);

            if ( rootPage.NextPageNo().IsInvalid() )
                return true; // nothing more to do
            
            // prepare truncate of the remaining pages
            deletePageNo = rootPage.NextPageNo();
            rootPage.SetNextPageNo( Data_PageNo() );
        }
        else
        {   // the root page is not affected
            if ( ! iter.Set (newLastPageNo, m_RootId.PageRecoveryMode(), Data_ForUpdate, errlist) )
            {
                errlist = Data_Exception(__CONTEXT__,DATA_CHAIN_TRUNCATE_ERROR,
                                         "newLastPageNo not found",
                                         SAPDB_ToString(newLastPageNo),
                                         SAPDB_ToString(newFirstFree) );
                AppendContextInfo(__CONTEXT__,errlist);
                return false;
            }
            // PTS 1111344 UH 2001-08-10 begin
            
            deletePageNo = (*iter).NextPageNo();
            
            (*iter).SetNextPageNo          (Data_PageNo()); // invalidate next pointer
            if ( newFirstFree > 0 )
                (*iter).SetFirstFreeOffset (newFirstFree ); // truncate entries in page
            
            if ( ! iter.Invalidate(errlist) )
            {
                AppendContextInfo(__CONTEXT__,errlist);
                return false;
            }
        }
        m_LastPageNo = newLastPageNo; // update pointer to last page
        // PTS 1111584 TS 2001-08-30 end

        if ( ! iter.Set (deletePageNo, m_RootId.PageRecoveryMode(), Data_ForUpdate, errlist) )
        {
            errlist = Data_Exception(__CONTEXT__,DATA_CHAIN_TRUNCATE_ERROR,
                                     "first page to delete not found",
                                     SAPDB_ToString(newLastPageNo),
                                     SAPDB_ToString(newFirstFree) );
            AppendContextInfo(__CONTEXT__,errlist);
            return false;
        }

        while ( iter.IsValid() )
        {
            deletePageNo = (*iter).PageNo();
            const SAPDB_Int4 pageVersion = (*iter).PageConverterVersion();
            const Data_Level level       = (*iter).GetLevel();

            if ( ! iter.PlusPlus(errlist) )
            {
                errlist = Data_Exception(__CONTEXT__,DATA_CHAIN_TRUNCATE_ERROR,
                                         "page to delete not found / chain is corrupt",
                                         SAPDB_ToString(newLastPageNo),
                                         SAPDB_ToString(newFirstFree) );
                AppendContextInfo(__CONTEXT__,errlist);
                return false;
            }
            
            if ( ! m_PAM.FreePageNo (deletePageNo, RootId().PageRecoveryMode(), pageVersion, level, errlist) )
            {
                errlist = Data_Exception(__CONTEXT__,DATA_CHAIN_TRUNCATE_ERROR,
                                         "deleting page failed",
                                         SAPDB_ToString(newLastPageNo),
                                         SAPDB_ToString(newFirstFree) );
                AppendContextInfo(__CONTEXT__,errlist);
                return false;
            }
        }

        return iter.Invalidate(errlist);
        // PTS 1111344 UH 2001-08-10 end
    }
    /*!
        @brief All pages of the page are read and check individually.

        If isCold is true then the converter is updated.
     */
    virtual bool Verify (bool                  isCold,
                         SAPDBErr_MessageList &errlist)
    {
        SAPDBTRACE_METHOD_DEBUG ("Data_Chain::Verify", DataChain_Trace, 5);

        Data_ChainIterator<T>  iter (m_PAM);
        Data_PageId            currentPageId = RootId();

        if ( ! Begin (iter, Data_ForRead, errlist) )
        {
            AppendContextInfo(__CONTEXT__,errlist);
            return false;
        }

        bool        result = true;
        Data_PageNo prevPageNo;
        Data_PageNo firstErrorPageNo;
        
        while ( iter.IsValid() )
        {
            if ( ! (*iter).Verify(isCold,errlist) )
            {
                errlist = errlist + Data_Exception(__CONTEXT__, DATA_ERROR_VERIFY_PAGE,
                                            SAPDB_ToString((*iter).PageNo()),
                                            SAPDB_ToString((*iter).NextPageNo()),
                                            SAPDB_ToString((*iter).RootPageNo()),
                                            SAPDB_ToString(RootId().PageNo()),
                                            SAPDB_ToString(prevPageNo) );
                if ( firstErrorPageNo.IsInvalid() )
                    firstErrorPageNo = (*iter).PageNo();
            }

            if ( isCold )
            {            
                currentPageId.SetPageNo((*iter).PageNo());
                m_PAM.MarkPageIdAsUsed(currentPageId);
            }
            prevPageNo = (*iter).PageNo();
            if ( ! iter.PlusPlus(errlist) )
            {
                AppendContextInfo(__CONTEXT__,errlist);
                return false;
            }
        }
        
        if ( m_PAM.GetLastError() != 0 )
            errlist = errlist + Data_Exception(__CONTEXT__, DATA_ERROR_VERIFY_CHAIN,
                                        SAPDB_ToString(RootId().PageNo()),
                                        SAPDB_ToString(m_PAM.GetLastError()),
                                        SAPDB_ToString(firstErrorPageNo) );
        return result;
    }
    /// This locks the chain. returns false, on any error.
    bool Lock(Data_AccessMode       lockmode,
              SAPDBErr_MessageList &errlist)
    {
        if ( IsLocked(lockmode) ) // PTS 1127083 UH 2004-01-14 added
            return true;
            
        const bool result = m_PAM.GetPage ( m_RootPage,
                                            m_RootId.PageNo(),
                                            m_RootId.PageRecoveryMode(),
                                            lockmode,
                                            errlist );
        m_RootPage.SetUnChanged();
        
        SAPDBERR_ASSERT_STATE( ! result || IsLocked(lockmode) );
        
        return result;
    }
    /// This unlocks the chain.
    bool UnLock(SAPDBErr_MessageList &errlist)
    {
        return m_PAM.ReleasePage (m_RootPage, errlist);
    }
    /*!
        @brief returns true, if the chain is locked in the correct way.
        If forRead is requested, forUpdate will be ok too.
     */
    bool IsLocked(Data_AccessMode lockmode)
    {
        // PTS 1127083 UH 2004-01-14
        return m_RootPage.IsAssigned()
               &&
               m_RootPage.AccessMode() == lockmode;
    }
    /*!
        @brief Get a given page. If iter is invalid, the call has failed.
        The recoverymode is taken from the root.
     */
    bool GetPage (Data_PageNo            pageno,
                  Data_AccessMode        accessmode,
                  Data_ChainIterator<T> &iter,
                  SAPDBErr_MessageList  &errlist)
    {
        if ( m_RootPage.IsAssigned()
             &&
             m_RootPage.PageNo() == pageno )
        {
            if ( ! iter.Clone(m_RootPage, errlist) )
            {
                AppendContextInfo(__CONTEXT__,errlist);
                return false;
            }
            return true;
        }
        if ( ! iter.Set (pageno, m_RootId.PageRecoveryMode(), accessmode, errlist) )
        {
            AppendContextInfo(__CONTEXT__,errlist);
            return false;
        }
        if ( (*iter).NextPageNo().IsInvalid() )
            m_LastPageNo = (*iter).PageNo();
        return true;
    }
    /// release the given page by iter.
    bool ReleasePage (Data_ChainIterator<T> &iter,
                      SAPDBErr_MessageList  &errlist)
    {
        if ( m_RootPage.IsAssigned()
             &&
             m_RootPage.PageNo() == (*iter).PageNo()
             &&
             (*iter).AccessMode() != Data_ForRead )
            m_RootPage.SetChanged();
            
        return iter.Invalidate(errlist);
    }
    /// Write all known information of the checin to the knltrace.
    void WriteToTrace(const char* title = 0) const
    {
        if ( title != 0 )
            Kernel_VTrace() << title;
        Kernel_VTrace() << "Data_Chain: mode: " << m_RootId.PageRecoveryMode()
                        << ", root: "           << m_RootId.PageNo()
                        << ", last: "           << m_LastPageNo;
    }
    /// This appends an info errlist to given errlist
    virtual void AppendContextInfo (SAPDB_Char  const * const  filename,
                                    SAPDB_UInt4 const          linenumber,
                                    SAPDBErr_MessageList      &errlist,
                                    SAPDB_Char  const * const  title = 0) const
	{
        errlist =
            errlist
            +
            Data_Exception(filename,linenumber, DATA_CHAIN_INFO,
                           (title==NULL?"Chain":title),
                           SAPDB_ToString(m_RootId.PageNo()),
                           SAPDB_ToString(m_LastPageNo),
                           (m_RootId.PageRecoveryMode().UseStaticPageAddressing()?"static":"dyn"),
                           (m_RootId.PageRecoveryMode().PageIsRecoverable()?"perm":"temp"));
            m_RootPage.AppendContextInfo(filename,linenumber,errlist);
    }

protected:
    /// The first page of the chain is identified.
    Data_PageId& m_RootId;
    /// The last page of the chain is identified.
    /// This Id is stored within the first page of the chain.
    Data_PageNo m_LastPageNo;
    /// This is used to access pages.
    Data_PageAccessManager& m_PAM;
    /// This is used to realize a lock protocol.
    T m_RootPage;
};
#endif // Data_Chain_HPP
