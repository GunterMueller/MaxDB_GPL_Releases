/*!**************************************************************************

  module      : Data_TraverseTreeAccess.hpp

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


#ifndef DATA_TRAVERSE_TREE_ACCESS_HPP
#define DATA_TRAVERSE_TREE_ACCESS_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "ggg00.h"
#include "gbd00.h"

#include "DataAccess/Data_Types.hpp"
#include "DataAccess/Data_ITreeAccess.hpp"
#include "DataAccess/Data_PageAccessManager.hpp"
#include "DataAccess/Data_PrimPage.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"
#include "KernelCommon/Kernel_Common.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!*****************************************************************************

   class: Data_TraverseTreeAccess

   description: This class is used to 
   
              
*******************************************************************************/

class Data_TraverseTreeAccess : public Data_ITreeAccess
{
public:

    /* -----------------------------------------------------------------------*/
    /*! chapter: Constructors and initialization                              */
    /* -----------------------------------------------------------------------*/

    /*!----------------------------------------------------------------------
    function:     Data_TraverseTreeAccess()
    description:  Constructs an iterator to a primary tree which is not
                  interruptible, i.e. the iterator will not break himself if 
                  an exclusive request to the same root node occures. The
                  iterator traverse the complete tree and provides each
                  node of the tree -- leaf and non leaf node.
    arguments:    Current [in/out] identifier for the file to be handled
    return value: none
    -----------------------------------------------------------------------*/

    Data_TraverseTreeAccess( tbd_current_tree    &current )
    :
    m_Current( current ),
    m_TrError( current.curr_trans->trError_gg00 ),
    m_PageAccessManager( current ),
    m_Page()
    {}

    /*!----------------------------------------------------------------------
    function:     ~Data_TraverseTreeAccess()
    description:  The destructor resumes the consumed resources, specifically
                  the requested pages and locks.
    arguments:    none
    return value: none
    -----------------------------------------------------------------------*/

    ~Data_TraverseTreeAccess();

    /* -----------------------------------------------------------------------*/
    /*! endchapter: constructors and initialization                           */
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*! chapter: Leaf Node Access                                             */
    /* -----------------------------------------------------------------------*/

    Data_PrimPage& GetNext();


    Data_PageCount GetNumberOfEstimatedLeafNodes() const{
        return 0;
    }

    /* -----------------------------------------------------------------------*/
    /*! endchapter:  Leaf Node Access                                         */
    /* -----------------------------------------------------------------------*/

    /*!
     * @copydoc Data_ITreeAccess::GetFileNo
     */
    FileDir_FileNo GetFileNo() const;

private:

    // Copy and assignment operator are not supported

    Data_TraverseTreeAccess& operator=( const Data_TraverseTreeAccess &tta );

    Data_TraverseTreeAccess( const Data_TraverseTreeAccess &tta );


    SAPDB_Bool GetLeftMostNodeAtNextLevel( const Data_Level stopLevel );

    SAPDB_Bool SetToRootNode();

    void ReplaceLeafNodeLock( 
        const Data_PageNo currentPageNo );

    void ReplaceFirstLevelIndexNodeLock( const Data_PageNo nextPageNo );

private:

    /*-------------------------------------------------------------------------
    declaration: m_Current
    description: Current file handle
    --------------------------------------------------------------------------*/

    tbd_current_tree    &m_Current;

    /*-------------------------------------------------------------------------
    declaration: m_TrError
    description: Reference to the base error
    --------------------------------------------------------------------------*/

    tgg00_BasisError    &m_TrError;

    /*-------------------------------------------------------------------------
    declaration: m_PageAccessManager
    description: Access manager for the current file
    --------------------------------------------------------------------------*/

    Data_PageAccessManager  m_PageAccessManager;

    /*-------------------------------------------------------------------------
    declaration: m_Page
    description: Current accessed page
    --------------------------------------------------------------------------*/

    Data_PrimPage   m_Page;
};

/*!*****************************************************************************

   endclass: Data_TraverseTreeAccess

*******************************************************************************/

#endif  /* DATA_TRAVERSE_TREE_ACCESS_HPP */
