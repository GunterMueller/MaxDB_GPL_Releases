/*
  @file     Data_IndexScanTreeAccess.cpp
  @author   TorstenS
  @ingroup  DataAccess
  @brief    Primary tree iterator on index level one
  @see            

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2001-2006 SAP AG

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



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "hbd01_1.h"
#include "hbd30.h"
#include "DataAccess/Data_FileTypes.hpp"
#include "DataAccess/Data_IndexScanTreeAccess.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/



/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/



/*===========================================================================*
 *  METHODS -lll-                                                            *
 *===========================================================================*/

Data_IndexScanTreeAccess::Data_IndexScanTreeAccess(
    Trans_Context&  trans,
    tgg00_FileId&   fileId )
        :
        m_Trans( trans ),
        m_FileId( fileId ),
        m_CallCount( 0 ),
        m_Current( b01nilcurrent ),
        m_PageAccessManager( trans.OldTrans(), Data_TableFile, Data_PageRecoveryMode(), Data_PageNo()),
        m_RootPage(),
        m_IndexPage()
{}

/*---------------------------------------------------------------------------*/

Data_IndexScanTreeAccess::~Data_IndexScanTreeAccess()
{
    m_IndexPage.Deassign( m_Trans.ErrorList(), false ); // release the page for read
    m_RootPage.DiscardRoot();
    if(  0 == m_Current.currRootNptrs_bd00.np_ptr()){
        return; // root page access was given to this class
    }
    // return root page access, because it was requested by this class
    bd30ReleaseTree( m_Current );
}

/*---------------------------------------------------------------------------*/

Data_PrimPage&
Data_IndexScanTreeAccess::GetNext()
{
    SAPDBTRACE_ROUTINE_DEBUG( "Data_IndexScanTreeAccess::GetNext", DataTree_Trace, 5 );

    ++m_CallCount;

    if( m_IndexPage.IsAssigned())
    {
        // second and all further accesses to primary tree
        return this->GetNextIndexNodeOnLevelOne();
    }
    if( ! m_RootPage.IsAssigned())
    { // first access to primary tree and root access was not given
        bd30GetTree( m_Trans.OldTrans(), m_FileId, m_Current, m_select,
                     ! LOCK_TREE_EXCL_BD00, ! SYNC_TREE_LOCK_BD00 );

        if( ! m_Trans.IsOk()){
            return m_RootPage;
        }
        m_RootPage.AssignRoot( m_Current.currRootNptrs_bd00 );
    }
    switch( m_RootPage.GetLevel())
    {
    case LEAF_LEVEL_BD00:
        m_Trans.SetErrorCode( e_no_next_record );
        return m_RootPage;
    case FIRST_INDEX_LEVEL_BD00:
        if( 1 < m_CallCount  ){
            m_Trans.SetErrorCode( e_no_next_record );
        }
        return m_RootPage;
    default:
        return this->GetLeftMostIndexNodeOnLevelOne();
    }
}

/*---------------------------------------------------------------------------*/
/* PRIVATE METHODS                                                           */
/*---------------------------------------------------------------------------*/

Data_PrimPage&
Data_IndexScanTreeAccess::GetLeftMostIndexNodeOnLevelOne()
{
    SAPDBTRACE_ROUTINE_DEBUG( "Data_IndexScanTreeAccess::GetLeftMostLeafNode", DataTree_Trace, 5 );

    SAPDBERR_ASSERT_STATE( m_RootPage.IsAssigned());
    SAPDBERR_ASSERT_STATE( m_RootPage.GetLevel() > FIRST_INDEX_LEVEL_BD00 );
    SAPDBERR_ASSERT_STATE( ! m_IndexPage.IsAssigned());

    Data_PrimPage page;
    page.Clone( m_RootPage ); // start search at root page, caused by the clone method
    // the root page will not be deassigned!
    do
    {
        const tgg00_Rec*    pSeparator      = page.GetRecord( FIRST_REC_INDEX_BD00 );
        const Data_PageNo   childPagePageNo = pSeparator->recPno_gg00().gg90GetInt4();
        const Data_Level    oldLevel        = page.GetLevel();

        if( ! m_PageAccessManager.ReleasePage( page, m_Trans.ErrorList())){
            break;
        }
        if( oldLevel > SECOND_INDEX_LEVEL_BD00  )
        {
            if( m_PageAccessManager.GetPage( page, childPagePageNo, m_Trans.ErrorList())){
                continue;
            }
            break;
        }
        if( SECOND_INDEX_LEVEL_BD00 == oldLevel ){
            m_PageAccessManager.GetPage( m_IndexPage, childPagePageNo, m_Trans.ErrorList());
            break; // level one is reached ore an error occur
        }
        SAPDBERR_ASSERT_STATE( SECOND_INDEX_LEVEL_BD00 >= oldLevel );
        m_Trans.SetErrorCode( e_system_error );

    } while( m_Trans.IsOk());
    return m_IndexPage;
}

/*---------------------------------------------------------------------------*/

Data_PrimPage&
Data_IndexScanTreeAccess::GetNextIndexNodeOnLevelOne()
{
    SAPDBTRACE_ROUTINE_DEBUG( "Data_IndexScanTreeAccess::GetNextNodeOnLevelOne", DataTree_Trace, 7 );

    SAPDBERR_ASSERT_STATE( m_RootPage.IsAssigned());
    SAPDBERR_ASSERT_STATE( m_RootPage.GetLevel() > FIRST_INDEX_LEVEL_BD00 );
    SAPDBERR_ASSERT_STATE( m_RootPage.PageNo() != m_IndexPage.PageNo());

    const Data_PageNo   nextPageNo = m_IndexPage.Successor();

    if( ! m_PageAccessManager.ReleasePage( m_IndexPage, m_Trans.ErrorList())){
        return m_IndexPage;
    }
    if( nextPageNo.IsInvalid()){
        m_Trans.SetErrorCode( e_no_next_record );
        return m_IndexPage;
    }
    if( ! m_PageAccessManager.GetPage( m_IndexPage, nextPageNo, m_Trans.ErrorList())){
        return m_IndexPage;
    }
    return m_IndexPage;
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
