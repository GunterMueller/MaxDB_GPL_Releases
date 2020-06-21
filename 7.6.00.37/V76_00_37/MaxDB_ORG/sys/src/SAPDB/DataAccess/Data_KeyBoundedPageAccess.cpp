/*****************************************************************************/
/*!
 @file    Data_KeyBoundedPageAccess.cpp
 @author  MartinKi

 @brief Implementation of class SrvTasks_JobCollectTableStatistics.

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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
/*****************************************************************************/

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

// Content of include files
#include "DataAccess/Data_KeyBoundedPageAccess.hpp"
#include "DataAccess/Data_PageAccessManager.hpp"
#include "DataAccess/Data_PrimPage.hpp"
#include "DataAccess/Data_Types.hpp"
#include "DataAccess/Data_ITreeAccess.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"
#include  "SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp"

#include "Trace/Trace_Entry.hpp"

#include "hsp30.h"
#include "hbd13.h"
#include "hbd30.h"
#include "ggg00.h"
#include "gbd00.h"
#include "gsp00.h"

#if COMPILEMODE_MEO00 >= SLOW_MEO00
#include "hta99.h"
#endif

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*************************************************************************/

Data_KeyBoundedPageAccess::Data_KeyBoundedPageAccess(
    tbd_current_tree&  current,
    tsp00_Key&         startKey,
    const SAPDB_Int2   startKeyLen,
    tsp00_Key&         stopKey,
    const SAPDB_Int2   stopKeyLen,
    Data_LRUChainInfo  lruChainInfo)
    :
    m_Current( current ),
    m_pageAccessManager(*current.curr_trans, Data_TableFile,
                        Data_PageRecoveryMode(),
                        current.curr_tree_id.fileRoot_gg00() ),
    m_page(),
    m_startKeyLen(startKeyLen),
    m_stopKeyLen(stopKeyLen),
    m_lruChainInfo(m_lruChainInfo)
{
    SAPDBTRACE_METHOD_DEBUG(
        "Data_KeyBoundedPageAccess::Data_KeyBoundedPageAccess",
        DataTree_Trace, 5 );

    SAPDB_RangeMove(
        __FILE__, 1,
        startKeyLen, sizeof(m_startKey),
        startKey, 1,
        &m_startKey, 1,
        startKeyLen,
        m_Current.curr_trans->trError_gg00 );

    SAPDB_RangeMove(
        __FILE__, 2,
        stopKeyLen, sizeof(m_stopKey),
        stopKey, 1,
        &m_stopKey, 1,
        stopKeyLen,
        m_Current.curr_trans->trError_gg00 );
}

/*************************************************************************/

Data_KeyBoundedPageAccess::~Data_KeyBoundedPageAccess()
{
    SAPDBTRACE_METHOD_DEBUG(
        "Data_KeyBoundedPageAccess::~Data_KeyBoundedPageAccess",
        DataTree_Trace, 5 );

    SAPDBErr_MessageList errlist;
    m_page.Deassign( errlist, false );
}

/*************************************************************************/

Data_PrimPage& Data_KeyBoundedPageAccess::SetToStartPage()
{
    SAPDBTRACE_METHOD_DEBUG(
        "Data_KeyBoundedPageAccess::SetToStartPage",
        DataTree_Trace, 5 );

    if ( e_ok != m_Current.curr_trans->trError_gg00 )
        return m_page;

    tbd_searchresult        PrimKeySearchResult;
    /* */
    tsp00_Int4              PrimRecIndex;
    /* */
    tbd_node_ptrs           pPrimNptr;
    /* */
    tbd_neighbors           Neighbors;

    pPrimNptr.np_ptr()    = NULL;
    pPrimNptr.np_cbptr()  = NULL;

    if ( m_page.IsAssigned() )
    {
        SAPDBErr_MessageList errlist;
        this->ReleasePage( m_page, errlist );
    }

    // get start page
    bd30SearchRecord (
        m_Current,
        reinterpret_cast<tsp00_KeyPtr>( &m_startKey ), m_startKeyLen,
        ! LOCK_LEAF_EXCL_BD00, pPrimNptr, PrimRecIndex, Neighbors, PrimKeySearchResult);

    if ( e_ok != m_Current.curr_trans->trError_gg00 )
        return m_page;

    // Torsten fragen: was, wenn nur Vorgänger da?
    if ( nonefound == PrimKeySearchResult ) // e_no_next_record;
        return m_page;

    const Data_PageNo startPageNo = pPrimNptr.np_ptr()->nd_id();

    b13r_release_node( pPrimNptr, m_Current, lru_normal );

    SAPDBErr_MessageList errlist;
    m_pageAccessManager.GetPage( m_page, startPageNo, errlist );

#   if COMPILEMODE_MEO00 >= SLOW_MEO00
    t01basis_error( bi, "base error  ", m_Current.curr_trans->trError_gg00 );
#   endif

    return m_page;
}

/*************************************************************************/

Data_PrimPage& Data_KeyBoundedPageAccess::GetNext()
{
    SAPDBTRACE_METHOD_DEBUG(
        "Data_KeyBoundedPageAccess::GetLeafNode",
        DataTree_Trace, 5 );

    if ( e_ok != m_Current.curr_trans->trError_gg00 )
        return m_page;


    if ( ! m_page.IsAssigned() ) {
        this->SetToStartPage();
        return m_page;
    } else if ( ! this->IsPageInScope() ) {
        SAPDBErr_MessageList errlist;
        this->ReleasePage( m_page, errlist );
        return m_page;
    }

    do {
        this->GetNextLeafNode();
    } while ( m_page.IsAssigned()
              && (0 == m_page.GetRecordCount()) );

//     if ( ! this->IsPageInScope() ) {
//         Kernel_VTrace() << "GetLeafNode: stopped at pageNo = "
//                         << m_page.PageNo()
//                         << NewLine;
//         this->ReleasePage( m_page );
//     } else {
//         Kernel_VTrace() << "GetLeafNode: pageNo = " << m_page.PageNo()
//                         << NewLine;
//     }


#   if COMPILEMODE_MEO00 >= SLOW_MEO00
    t01basis_error( bi, "base error  ", m_Current.curr_trans->trError_gg00 );
#   endif

    return m_page;
}

/*************************************************************************/

/// Does *ONLY* check if page is past stop key!
SAPDB_Bool Data_KeyBoundedPageAccess::IsPageInScope()
{
    if ( 0 < m_page.GetRecordCount() )
    {
        tsp00_LcompResult compResult;

        tgg00_Rec& lastRecord = *m_page.GetLastRecord();

        s30cmp (lastRecord.recBody_gg00(), POS_OFF_DIFF_BD00,
                lastRecord.recKeyLen_gg00(),
                &m_stopKey, POS_OFF_DIFF_BD00, m_stopKeyLen,
                compResult);

        if ( (l_equal == compResult)
             || (l_greater == compResult) ) {
            return false;
        }
    }
    return true;
}

/*************************************************************************/

SAPDB_Bool Data_KeyBoundedPageAccess::GetNextLeafNode()
{
    tgg00_BasisError    &trError = m_Current.curr_trans->trError_gg00;

    SAPDBERR_ASSERT_STATE( m_page.IsAssigned() );
    SAPDBERR_ASSERT_STATE( e_ok == trError );

    const Data_PageNo nextLeafPage = m_page.Successor();
    Data_PageNo nextIndexPage;

    if(
        ( m_Current.curr_tree_id.fileType_gg00().includes( ftsPerm_egg00 )) &&
        ( m_Current.currRightBound_bd00 == m_page.PageNo()                ) &&
        ( m_Current.currIndexNptrs_bd00.np_ptr() != NULL                  )
    )
    {
        nextIndexPage = m_Current.currIndexNptrs_bd00.np_ptr()->nd_right();
    }

    SAPDBErr_MessageList errlist;
    this->ReleasePage( m_page, errlist );

    if( nextIndexPage.IsValid() )
    {
        bd30ReleaseSubTree( m_Current );
        bd30GetSubTree( m_Current, nextIndexPage );

        if( e_ok != trError )
            return( false );
    }

    if( nextLeafPage.IsValid() )
    {
        SAPDBErr_MessageList errlist;
        m_pageAccessManager.GetPage( m_page, nextLeafPage, errlist );
    }

    return( e_ok == trError );
}

/*************************************************************************/

void Data_KeyBoundedPageAccess::ReleasePage(
    Data_PrimPage& page,
    Msg_List&      errlist)
{
    page.SetLruChainInfo( m_lruChainInfo );
    m_pageAccessManager.ReleasePage( page, errlist );
}

/*************************************************************************/

FileDir_FileNo
Data_KeyBoundedPageAccess::GetFileNo() const
{
    return FileDir_FileNo( m_Current.curr_tree_id.fileTabId_gg00() );
}
/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
