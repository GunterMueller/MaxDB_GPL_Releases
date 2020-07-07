/****************************************************************************

  module      : Data_ScanTreeAccess.cpp

  -------------------------------------------------------------------------

  responsible : TorstenS

  special area:
  description : 

  last changed: 2002-04-18
  see also    :

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2001-2005 SAP AG


    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "hbd13.h"
#include "hbd20_2.h"
#include "hbd30.h"
#include "hbd50.h"
#include "heo56.h"

#include "DataAccess/Data_ScanTreeAccess.hpp"
#include "DataAccess/Data_Messages.hpp"
#include "DataAccess/Data_Exceptions.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"
#include "KernelCommon/Kernel_FileIO.hpp"
#include "RunTime/RTE_Message.hpp"
#include "SAPDBCommon/SAPDB_ToString.hpp"
#include "SQLManager/ErrorHandling/SQLManErr_Interface.hpp"
#include "SAPDBCommon/SAPDB_RangeCode.hpp"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/



/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  GLOBAL VARIABLES                                                         *
 *===========================================================================*/


/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/

inline const char*
BasisErrorToString( const tgg00_BasisError error )
{
    return( SQLManErr_Interface::GetInstance().GetErrorText(
                SQLManErr_Interface::GetInstance().GetReturnCode( error, sqlm_internal )));
}

/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

Data_ScanTreeAccess::Data_ScanTreeAccess(
    tbd_current_tree  &curr,
    Data_LRUChainInfo lruChainInfo)
        :
        m_RootPageNo( curr.curr_tree_id.fileRoot_gg00()),
        m_RecMode( Data_Dynamic, curr.curr_tree_id.fileType_gg00().includes( ftsPerm_egg00 ) ? Data_Recoverable : Data_NotRecoverable),
        m_Current( curr ),
        m_PageAccessManager( *curr.curr_trans, Data_TableFile, m_RecMode, m_RootPageNo ),
        m_LeafPage(),
        m_lruChainInfo( lruChainInfo )
{}

/*---------------------------------------------------------------------------*/

Data_ScanTreeAccess::~Data_ScanTreeAccess()
{
    ReleaseLeafPage();
}

/*---------------------------------------------------------------------------*/

void Data_ScanTreeAccess::SetLruChainInfo(const Data_LRUChainInfo lruChainInfo)
{
    m_lruChainInfo = lruChainInfo;
}

/*---------------------------------------------------------------------------*/

void Data_ScanTreeAccess::ReleaseLeafPage()
{
    if( ! m_LeafPage.IsAssigned()){
        return;
    }
    if( m_RootPageNo == m_LeafPage.PageNo()) // root page access will be releases by bd30ReleaseTree
    {
        m_LeafPage.DiscardRoot();
        return;
    }

    m_LeafPage.SetLruChainInfo( m_lruChainInfo );

    SAPDBErr_MessageList msgList;

    if ( ! m_PageAccessManager.ReleasePage( m_LeafPage, msgList )){
        RTE_Message( msgList );
    }
}

/*---------------------------------------------------------------------------*/

void Data_ScanTreeAccess::AssignToLeafPage( tbd_node_ptrs &pNodes )
{
    SAPDBERR_ASSERT_STATE( NULL != pNodes.np_ptr());

    const Data_PageNo   nextLeafPage = pNodes.np_ptr()->nd_id();

    if( m_RootPageNo == nextLeafPage ) // root page access is still made by bd30GetTree
    {
        m_LeafPage.AssignRoot( pNodes );
        return;
    }

    // release leaf and get it again with the PAM interface
    b13r_release_node( pNodes, m_Current, lru_normal );

    SAPDBErr_MessageList msgList;

    if( m_PageAccessManager.GetPage( m_LeafPage, nextLeafPage, m_RecMode, Data_ForRead, msgList ))
    {
        SAPDBERR_ASSERT_STATE( LEAF_LEVEL_BD00 == m_LeafPage.GetLevel());
    }
    else
    {
         RTE_Message( msgList );
    }
}

/*---------------------------------------------------------------------------*/

Data_PrimPage&
Data_ScanTreeAccess::GetNext()
{
    SAPDBTRACE_ROUTINE_DEBUG( "Data_ScanTreeAccess::GetNext", DataTree_Trace, 5 );

    SAPDBERR_ASSERT_STATE( e_ok == m_Current.curr_trans->trError_gg00 );

    if( m_LeafPage.IsAssigned() )
    {
        if( GetNextLeafNode() )
            return( m_LeafPage );
    }
    else if( GetLeftMostLeafNode() )
        return( m_LeafPage );

    // error handling
    if( m_LeafPage.IsAssigned()){
        ReleaseLeafPage();
    }
    return( m_LeafPage ); // return empty page to indicate termination of iterator
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

Data_PrimPage&
Data_SerializableScanTreeAccess::GetNext()
{
    SAPDBTRACE_ROUTINE_DEBUG( "Data_SerializableScanTreeAccess::GetNext", DataTree_Trace, 5 );

    SAPDBERR_ASSERT_STATE( e_ok == m_Current.curr_trans->trError_gg00 );

    if( m_LeafPage.IsAssigned() ) // subsequent access
    {
        if(
            ( m_NumberOfReadLeafNodes > MAX_NODE_SCAN_BD00                ) &&
            bd20IsPageRequested ( m_Current.currRootNptrs_bd00.np_cbptr() )
        )
        {
            if( ! Break() )
                return( m_LeafPage );
            if( ! Continue() ) // go to the old leaf page and then to the successor
                return( m_LeafPage );
        }
        if( Data_SerializableScanTreeAccess::GetNextLeafNode() )
            return( m_LeafPage );
    }
    else if( Data_SerializableScanTreeAccess::GetLeftMostLeafNode() ) // first access to the given file
        return( m_LeafPage );

    // error handling
    if( m_LeafPage.IsAssigned()){
        ReleaseLeafPage();    
    }

    return( m_LeafPage ); // return empty page to indicate termination of iterator
}

/*---------------------------------------------------------------------------*/
/*                            private methods                                */
/*---------------------------------------------------------------------------*/

SAPDB_Bool
Data_ScanTreeAccess::GetLeftMostLeafNode()
{
    tgg00_BasisError &trError = m_Current.curr_trans->trError_gg00;

    SAPDBERR_ASSERT_STATE( e_ok == trError );
    SAPDBERR_ASSERT_STATE( ! m_LeafPage.IsAssigned());

    tsp00_Int4          recIndexForLeaf;
    tbd_node_ptrs       pNodes;
    tbd_neighbors       neighbors;
    SAPDB_Byte          *pZeroKey = NULL;
    const SAPDB_Int2    zeroKeyLength = 0;

    pNodes.np_ptr()   = NULL;
    pNodes.np_cbptr() = NULL;

    bd50FindLeaf( pZeroKey, zeroKeyLength, ! LOCK_LEAF_EXCL_BD00,
        pNodes, neighbors, recIndexForLeaf, m_Current); 

    if( e_ok == trError ){
        AssignToLeafPage( pNodes );
    }
    else if( NULL != pNodes.np_ptr()){
        b13r_release_node( pNodes, m_Current, lru_normal );
    }
    return( e_ok == trError );
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
Data_ScanTreeAccess::GetNextLeafNode()
{
    tgg00_BasisError    &trError = m_Current.curr_trans->trError_gg00;

    SAPDBERR_ASSERT_STATE( m_LeafPage.IsAssigned() );
    SAPDBERR_ASSERT_STATE( e_ok == trError );

    const Data_PageNo   nextLeafPage = m_LeafPage.Successor();
    Data_PageNo         nextIndexPage;

    if(
        ( m_Current.curr_tree_id.fileType_gg00().includes( ftsPerm_egg00 )) &&
        ( m_Current.currRightBound_bd00 == m_LeafPage.PageNo()            ) &&
        ( m_Current.currIndexNptrs_bd00.np_ptr() != NULL                  )
    )
    {
        nextIndexPage = m_Current.currIndexNptrs_bd00.np_ptr()->nd_right();
    }

    ReleaseLeafPage();

    if( nextIndexPage.IsValid() )
    {
        bd30ReleaseSubTree( m_Current );
        bd30GetSubTree( m_Current, nextIndexPage );

        if( e_ok != trError )
            return( false );
    }

    if( nextLeafPage.IsValid() )
    {
        SAPDBErr_MessageList msgList;

        if ( ! m_PageAccessManager.GetPage( m_LeafPage, nextLeafPage, m_RecMode, Data_ForRead, msgList ))
        {
            RTE_Message( msgList );
        }
    }

    return( e_ok == trError );
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

SAPDB_Bool
Data_SerializableScanTreeAccess::Break()
{
    tgg00_BasisError    &trError = m_Current.curr_trans->trError_gg00;
    const tsp00_TaskId  &taskId  = m_Current.curr_trans->trTaskId_gg00;

    SAPDBERR_ASSERT_STATE( m_LeafPage.IsAssigned() );
    SAPDBERR_ASSERT_STATE( e_ok == trError );

    if( NULL == m_pReEntranceKey )
    {
        // This is the first interruption of the iterator, therefore no
        // memory is available to store the re-entrance key. After the
        // break, the memory will not deallocated because it could be reused
        // for the new break, if it occures. The allocation and deallocation
        // of memory is more expensive than holding the allocated memory
        // until the end of the scan.
        if ( m_Current.curr_tree_id.fileType_gg00().includes( ftsPerm_egg00 ) ) {
            m_pReEntranceKey = reinterpret_cast< SAPDB_Byte* >( m_Allocator.Allocate( PERM_KEY_MXSP00 ));
        }
    else
        {
            m_pReEntranceKey = reinterpret_cast< SAPDB_Byte* >( m_Allocator.Allocate( KEY_MXSP00 ));
        }
    }

    if( NULL == m_pReEntranceKey )
    {
        trError = e_no_more_memory;
        return( false );
    }

    tgg00_Rec* pRecord = m_LeafPage.GetLastRecord();
    SAPDB_Byte* pKey   = reinterpret_cast< SAPDB_Byte* >( pRecord ) + cgg_rec_key_offset;

    Data_Exception  errMsg;

    SAPDB_RangeMove( __FILE__, 1,
        sizeof( tsp00_Key ), sizeof( tsp00_Key ),
        pKey, POS_OFF_DIFF_BD00, m_pReEntranceKey, POS_OFF_DIFF_BD00,  
        pRecord->recKeyLen_gg00(), trError );

    if( e_ok == trError ){
        m_ReEntranceKeyLength = pRecord->recKeyLen_gg00();
    }
    else
    {
        Data_Exception errMsg( __CONTEXT__, DATA_ERR_SCAN_TREE,
                                BasisErrorToString( trError ),
                                SAPDB_ToString( m_LeafPage.GetRecordCount()-1, _T_d ),
                                SAPDB_ToString( m_LeafPage.PageNo(), _T_d ),
                                SAPDB_ToString( m_LeafPage.RootPageNo(), _T_d ));

        RTE_Message( errMsg );

        Kernel_FileIO   dumpFile( KERNEL_DATA_FILE, KERNEL_COR_FILE_EXTENSION,
                                  SAPDB_ToString( m_LeafPage.PageNo(), _T_d ));
        dumpFile.Write( m_LeafPage );
    }

    ReleaseLeafPage();

    bd30ReleaseTree( m_Current );

    m_NumberOfReadLeafNodes = 0;

    vsleep(taskId, 0 ); // reschedule

    return( e_ok == trError );
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
Data_SerializableScanTreeAccess::Continue()
{
    tgg00_BasisError &trError = m_Current.curr_trans->trError_gg00;

    SAPDBERR_ASSERT_STATE( ! m_LeafPage.IsAssigned() );
    SAPDBERR_ASSERT_STATE( e_ok == m_Current.curr_trans->trError_gg00 );
    SAPDBERR_ASSERT_STATE( NULL != m_pReEntranceKey );
    SAPDBERR_ASSERT_STATE( 0 != m_ReEntranceKeyLength );

    tsp00_Int4          recIndexForLeaf;
    tbd_neighbors       neighbors;
    tbd_node_ptrs       pNodes;
    tgg00_FileId        auxFileId = m_Current.curr_tree_id;

    pNodes.np_ptr()   = NULL;
    pNodes.np_cbptr() = NULL;

    bd30GetTree (*m_Current.curr_trans, auxFileId, m_Current, 
        m_select, ! LOCK_TREE_EXCL_BD00, ! SYNC_TREE_LOCK_BD00 );

    if( e_ok != trError )
        return( false );

    bd50FindLeaf (m_pReEntranceKey, m_ReEntranceKeyLength, ! LOCK_LEAF_EXCL_BD00,
        pNodes, neighbors, recIndexForLeaf, m_Current);

    if( e_ok == trError ){
        AssignToLeafPage( pNodes );
    }
    else if( NULL != pNodes.np_ptr()){
        b13r_release_node( pNodes, m_Current, lru_normal );
    }
    return( e_ok == trError );
}

/*---------------------------------------------------------------------------*/

FileDir_FileNo
Data_ScanTreeAccess::GetFileNo() const
{
    return FileDir_FileNo( m_Current.curr_tree_id.fileTabId_gg00() );
}
/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/

