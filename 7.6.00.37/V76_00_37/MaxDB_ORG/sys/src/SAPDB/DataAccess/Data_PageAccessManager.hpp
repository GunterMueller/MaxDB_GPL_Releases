/*!
  @file   Data_PageAccessManager.hpp
  @author UweH
  @brief  The one and only page access manager in the kernel. (c++ interface to c and pascal coding)

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
#ifndef DATA_PAGE_ACCESS_MANAGER_HPP
#define DATA_PAGE_ACCESS_MANAGER_HPP

#include "ggg00.h" // tgg00_TransContext
#include "gbd00.h" // tbd_current_tree
#include "DataAccess/Data_FileTypes.hpp"
#include "DataAccess/Data_BasePage.hpp"
#include "Converter/Converter_IVerify.hpp"
#include "RunTime/Tasking/RTETask_ITask.hpp"

/*!
    @class   Data_PageAccessManager
    @brief   encapsulates the page access manager (Data_Cache)
 */
class Data_PageAccessManager
{
public:
    /*!
       @brief This initializes a context to access pages of a special type.
       @param oldcontext [in/out] 
       @param FileType [in] 
       @param RecoveryMode [in] 
       @param RootPageNo [in] 
     */
    Data_PageAccessManager (tgg00_TransContext&             TransContext,
                            const Data_FileType             FileType,
                            const Data_PageRecoveryMode&    RecoveryMode,
                            const Data_PageNo               RootPageNo);
    
    /*!
       @brief This initializes a context to access pages for a given file
       @param current [ONLY IN] 
     */
    Data_PageAccessManager (tbd_current_tree &current)
    :m_Current( current )
    {}
    
	/// This initializes a context for and uses some internals from the given pam
    Data_PageAccessManager (Data_PageAccessManager  &Pam,
                            const Data_FileType             FileType,
                            const Data_PageRecoveryMode&    RecoveryMode,
                            const Data_PageNo               RootPageNo);
                            
    /*!
        @brief  The page handle is set to a cached frame which is specified.
        @param  Page [out] 
        @param  PageNo [in] 
        @param  RecoveryMode [in] 
        @param  AccessMode [in] 
        @param  errlist [out] 
        @return true, if page could be found, else false

        The pointer to the frame in memory is given and a generic hint
     */

    bool GetPage (Data_BasePage               &Page,
                  const Data_PageNo&           PageNo,
                  const Data_PageRecoveryMode &RecoveryMode,
                  Data_AccessMode              AccessMode,
                  SAPDBErr_MessageList        &errlist);
    /*!
        @brief  The page handle is set to a cached frame which is specified.
        @param  Page [out] 
        @param  PageNo [in] 
        @param  AccessMode [in] 
        @param  errlist [out] 
        @return true, if page could be found, else false
     */
    bool GetPage (Data_BasePage               &Page,
                  const Data_PageNo           &PageNo,
                  Data_AccessMode              AccessMode,
                  SAPDBErr_MessageList        &errlist);
    /*!
        @brief  All parameters needed for getting the page are taken from Page.
        @param  Page [out] 
        @param  PageNo [in] 
        @param  errlist [out]
        @return true, if page could be found, else false
     */
    bool GetPage (Data_BasePage                   &Page,
                  const Data_PageNo           &PageNo,
                  SAPDBErr_MessageList        &errlist);
    /// The specified page is released out of the cache.
    bool ReleasePage (Data_BasePage            &Page,
                      SAPDBErr_MessageList &errlist);
    /*!
        @brief This allocates a new page frame and assigns a new page id.
        @param Page [out] This is the handler for the new page frame.
        @param Mode [in] description: A new page with a unique PageId is given
        @param  errlist [out]
     */
    bool NewPage (Data_BasePage&             Page,
                  Data_PageRecoveryMode  Mode,
                  SAPDBErr_MessageList  &errlist);
    /// A frame with a given Page handler is removed. The Page handler may be reused.
    bool FreePage (Data_BasePage            &Page,
                   SAPDBErr_MessageList &errlist);
    /// A frame with a given page no is removed.
    bool FreePageNo (const Data_PageNo&           PageNo,
                     const Data_PageRecoveryMode& RecoveryMode,
                     const SAPDB_Int4             PageConverterVersion,
                     const Data_Level             Level,
                     SAPDBErr_MessageList        &errlist);
    /// This resets this handle
    void Invalidate()
    {
        m_Current.curr_tree_id.fileRoot_gg00() = NIL_PAGE_NO_GG00;
    }
    /// If this handle is invalid false is returned.
    bool IsValid() const
    {
        return Data_PageNo(m_Current.curr_tree_id.fileRoot_gg00()).IsValid();
    }
    /// This is used by cold verify
    void MarkPageIdAsUsed (const Data_PageId &pageid)
    {
        Converter_IVerify::Instance().MarkPageNoAsUsed ( Task(),
                                                         pageid.PageNo(),
                                                         pageid.PageRecoveryMode().GetAddressingMode() );
    }
    /// If any method fails this could offer some hints what was going wrong
    SAPDB_Int GetLastError () const
    {
        // 2003-05-09 UH added
        return m_Current.curr_trans->trError_gg00;
    }
    /// resets the last error to ok.
    void ResetLastError()
    {
        // 2003-05-09 UH added
        m_Current.curr_trans->trError_gg00 = e_ok;
    }
    /// Append some info's about the context of the PAM
    void AppendContextInfo (SAPDB_Char  const * const  filename,
                            SAPDB_UInt4 const          linenumber,
                            SAPDBErr_MessageList      &errlist,
                            SAPDB_Char  const * const  title = 0) const;

    /// writes current file information to the knldiag
    virtual void WriteBadFileInfo();

private:

    /// returns task context
    const RTETask_ITask& Task(){
        return *RTETask_ITask::ByTaskID( m_Current.curr_trans->trTaskId_gg00 );
    }

    /// The Accessmanagers can save themself here,so the page destructor can do a release.
    void RegisterToPage (Data_BasePage&             Page,
                         tbd_node_ptrs&               Nptrs,
                         const Data_PageRecoveryMode& RecoveryMode,
                         Data_AccessMode              AccessMode)
    {
    
        Page.m_pAccessManager = this;
        Page.m_AccessMode     = AccessMode;
        Page.SetRecoveryMode(RecoveryMode);
        Page.SetHint(Nptrs.np_cbptr());
        Page.SetNodePtr(Nptrs.np_ptr());
    
        if ( Data_ForUpdate          == Page.m_AccessMode ||
                Data_ForStructureChange == Page.m_AccessMode )
            Page.m_ReleaseMode = Data_Changed;
        else
            Page.m_ReleaseMode = Data_Unchanged;
    }
    /// this is used by bd13 interface
    tbd_current_tree m_Current;
};
#endif // DATA_PAGE_ACCESS_MANAGER_HPP
