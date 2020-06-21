/*!
  @file     Data_IndexScanTreeAccess.hpp
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


#ifndef DATA_INDEX_SCAN_TREE_ACCESS_HPP
#define DATA_INDEX_SCAN_TREE_ACCESS_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "ggg00.h"
#include "gbd00.h"

#include "DataAccess/Data_Types.hpp"
#include "DataAccess/Data_PageAccessManager.hpp"
#include "DataAccess/Data_PrimPage.hpp"
#include "DataAccess/Data_ITreeAccess.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"
#include "KernelCommon/Kernel_Common.hpp"
#include "Transaction/Trans_Context.hpp"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

class Data_IndexScanTreeAccess : public Data_ITreeAccess
{
public:

    Data_IndexScanTreeAccess(
        Trans_Context&  trans,
        tgg00_FileId&   fileId );

    ~Data_IndexScanTreeAccess();

    /*!
        @brief  Set the accessor to tree, whereby the root page is still in 
                access by the calling task. If the root page access will be
                given no root access will be made implicit by the accessor!
                Note that in this case the accessor will not release the
                given root page. Note that the accessor could not break in
                GetNext() execution if a concurrent running task want to lock
                the tree exlsuive.
        @param pRoot [in] valid page handle for the root page
    */ 
    void SetToRoot( tbd_node_ptrs& pRoot )
    {
        SAPDBERR_ASSERT_STATE( 0 != pRoot.np_ptr());
        m_RootPage.AssignRoot( pRoot );
    }

    /*!
        @brief  Get access to the next node on tree level one, so that the 
                tree will be read from the leftmost edge to the rightmost edge.
                If no right neighbor exist e_no_next_record is returned. If the
                given tree consists of one node only e_no_next_record is returned
                immediately.
    */
    Data_PrimPage& GetNext();

    /// Estimation of leaf nodes is not supported. Therefore 0 is returned
    Data_PageCount GetNumberOfEstimatedLeafNodes() const {
        return 0;
    }

    /// Returns the file no of the file the tree belongs to
    FileDir_FileNo GetFileNo() const{
        return FileDir_FileNo( m_FileId.fileTabId_gg00());
    }

private:

    // Copy and assignment operator are not supported

    Data_IndexScanTreeAccess& operator=( const Data_IndexScanTreeAccess &ista );

    Data_IndexScanTreeAccess( const Data_IndexScanTreeAccess &ista );

private:

    Data_PrimPage& GetLeftMostIndexNodeOnLevelOne();

    Data_PrimPage& GetNextIndexNodeOnLevelOne();

private:

    // encapslulated classic transaction context
    Trans_Context&  m_Trans;

    // classic fileId
    tgg00_FileId&   m_FileId;

    // number of GetNext calls
    SAPDB_UInt  m_CallCount;

    // old known current
    tbd_current_tree    m_Current;

    // Manager for the page access
    Data_PageAccessManager  m_PageAccessManager;

    // root page; not requested by the PAM
    Data_PrimPage   m_RootPage;

    // current index level one page; requested by PAM
    Data_PrimPage   m_IndexPage;
};

#endif  /* DATA_INDEX_SCAN_TREE_ACCESS_HPP */

