/****************************************************************************

  module      : Data_TraverseTreeAccess.cpp

  -------------------------------------------------------------------------

  responsible : TorstenS

  special area:
  description : 

  last changed: 2001-01-20
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

#include "ggg90.h"

#include "hbd30.h"

#include "DataAccess/Data_TraverseTreeAccess.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"
#include "RunTime/RTE_Message.hpp"


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
 *  METHODS                                                                  *
 *===========================================================================*/

Data_TraverseTreeAccess::~Data_TraverseTreeAccess()
{
    SAPDBErr_MessageList errlist;
    if ( ! m_Page.Deassign( errlist, false ) ) // release the page for read
        RTE_Message(errlist);
}

/*---------------------------------------------------------------------------*/

Data_PrimPage&
Data_TraverseTreeAccess::GetNext()
{
    SAPDBTRACE_ROUTINE_DEBUG( "Data_TraverseTreeAccess::GetLeafNode", DataTree_Trace, 5 );

    if( ! m_Page.IsAssigned() )
    {
        SetToRootNode();
        return( m_Page );
    }

    const Data_PageNo currentPageNo = m_Page.PageNo();
    const Data_PageNo nextPageNo    = m_Page.Successor();
    const Data_Level  currentLevel  = m_Page.GetLevel();

    SAPDBErr_MessageList errlist;

    if ( ! m_PageAccessManager.ReleasePage( m_Page, errlist ) )
    {
        RTE_Message(errlist);
        errlist.ClearMessageList();
    }

    if( nextPageNo.IsValid() )
    {
        if( currentLevel == LEAF_LEVEL_BD00 )
        {
            ReplaceLeafNodeLock( currentPageNo );
        }
        else if(
            ( currentLevel == FIRST_INDEX_LEVEL_BD00                          ) &&
            ( m_Current.curr_tree_id.fileType_gg00().includes( ftsPerm_egg00 ))
        )
        {
            ReplaceFirstLevelIndexNodeLock( nextPageNo );
        }

        if( e_ok == m_TrError )
        {
            if ( ! m_PageAccessManager.GetPage( m_Page, nextPageNo, errlist ) )
            {
                RTE_Message(errlist);
                errlist.ClearMessageList();
            }
            return( m_Page );
        }
    }
    else if( LEAF_LEVEL_BD00 < currentLevel )
    {
        if( GetLeftMostNodeAtNextLevel( currentLevel -1 ))
            return( m_Page );
    }
    // error handling
    if( m_Page.IsAssigned() )
        if ( ! m_PageAccessManager.ReleasePage( m_Page, errlist ) )
        {
            RTE_Message(errlist);
            errlist.ClearMessageList();
        }

    return m_Page;
}

/*---------------------------------------------------------------------------*/
/*                            private methods                                */
/*---------------------------------------------------------------------------*/

SAPDB_Bool
Data_TraverseTreeAccess::SetToRootNode()
{
    SAPDBERR_ASSERT_STATE( ! m_Page.IsAssigned() );
    SAPDBERR_ASSERT_STATE( NULL != m_Current.currRootNptrs_bd00.np_ptr() );

    SAPDBErr_MessageList errlist;

    if ( ! m_PageAccessManager.GetPage( m_Page, m_Current.curr_tree_id.fileRoot_gg00(), errlist ) )
        RTE_Message(errlist);

    return( e_ok == m_TrError );
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
Data_TraverseTreeAccess::GetLeftMostNodeAtNextLevel( const Data_Level   stopLevel )
{
    SAPDBERR_ASSERT_STATE( ! m_Page.IsAssigned() );

    bd30ReleaseSubTree( m_Current );

    SetToRootNode();

    SAPDBErr_MessageList errlist;

    while( e_ok == m_TrError )
    {
        const tgg00_Rec     *pSeparator     = m_Page.GetFirstRecord();
        const Data_PageNo   childPagePageNo = pSeparator->recPno_gg00().gg90GetInt4();
        const Data_Level    parentLevel     = m_Page.GetLevel();


        if ( ! m_PageAccessManager.ReleasePage( m_Page, errlist ) )
        {
            RTE_Message(errlist);
            errlist.ClearMessageList();
        }

        if(
            ( m_Current.curr_tree_id.fileType_gg00().includes( ftsPerm_egg00 )) &&
            ( SECOND_INDEX_LEVEL_BD00 == parentLevel                          )
        )
        {
            bd30GetSubTree( m_Current, childPagePageNo );

            if( e_ok != m_TrError )
                return( false );
        }
        if( ! m_PageAccessManager.GetPage( m_Page, childPagePageNo, errlist ))
        {
            RTE_Message(errlist);
            return( false );
        }

        if( stopLevel ==  m_Page.GetLevel() )
            return( true );
    }
    return( false );
}

/*---------------------------------------------------------------------------*/

void
Data_TraverseTreeAccess::ReplaceLeafNodeLock( const Data_PageNo currentPageNo )
{
    SAPDBERR_ASSERT_STATE( ! m_Page.IsAssigned() );

    if(
        ( m_Current.curr_tree_id.fileType_gg00().includes( ftsPerm_egg00 )) &&
        ( m_Current.currRightBound_bd00 == currentPageNo                  ) &&
        ( m_Current.currIndexNptrs_bd00.np_ptr() != NULL                  )
    )
    {
        const Data_PageNo nextIndexPage = m_Current.currIndexNptrs_bd00.np_ptr()->nd_right();

        ReplaceFirstLevelIndexNodeLock( nextIndexPage );
    }
}

/*---------------------------------------------------------------------------*/

void
Data_TraverseTreeAccess::ReplaceFirstLevelIndexNodeLock( const Data_PageNo nextPageNo )
{
    SAPDBERR_ASSERT_STATE( ! m_Page.IsAssigned() );
    SAPDBERR_ASSERT_STATE( nextPageNo.IsValid() );
    SAPDBERR_ASSERT_STATE( NULL != m_Current.currIndexNptrs_bd00.np_ptr() );
    SAPDBERR_ASSERT_STATE( FIRST_INDEX_LEVEL_BD00 == m_Current.currIndexNptrs_bd00.np_ptr()->nd_level() );

    bd30ReleaseSubTree( m_Current );
    bd30GetSubTree( m_Current, nextPageNo );
}

/*---------------------------------------------------------------------------*/

FileDir_FileNo
Data_TraverseTreeAccess::GetFileNo() const
{
    return FileDir_FileNo( m_Current.curr_tree_id.fileTabId_gg00() );
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
