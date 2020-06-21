/*!**************************************************************************

  module      : Data_ScanTreeAccess.hpp

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


#ifndef DATA_SCAN_TREE_ACCESS_HPP
#define DATA_SCAN_TREE_ACCESS_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "ggg00.h"
#include "gbd00.h"

#include "DataAccess/Data_ITreeAccess.hpp"
#include "DataAccess/Data_Types.hpp"
#include "DataAccess/Data_PageAccessManager.hpp"
#include "DataAccess/Data_PrimPage.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"
#include "KernelCommon/Kernel_Common.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"


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

   class: Data_ScanTreeAccess

   description: This class is used to 
   
              
*******************************************************************************/

class Data_ScanTreeAccess : public Data_ITreeAccess
{
public:

    /* -----------------------------------------------------------------------*/
    /*! chapter: Constructors and initialization                              */
    /* -----------------------------------------------------------------------*/

    /*!----------------------------------------------------------------------
    function:     Data_ScanTreeAccess()
    description:  Constructs an iterator to a primary tree which is not
                  interruptible, i.e. the iterator will not break himself if 
                  an exclusive request to the same root node occures. 
    arguments:    Current [in/out] identifier for the file to be handled
    return value: none
    -----------------------------------------------------------------------*/

    Data_ScanTreeAccess( tbd_current_tree    &curr,
                         Data_LRUChainInfo lruChainInfo = Data_LruNormal );

    /*!----------------------------------------------------------------------
    function:     ~Data_ScanTreeAccess()
    description:  The destructor resumes the consumed resources, specifically
                  the requested pages and locks.
    arguments:    none
    return value: none
    -----------------------------------------------------------------------*/

    ~Data_ScanTreeAccess();

    /* -----------------------------------------------------------------------*/
    /*! endchapter: constructors and initialization                           */
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*! chapter: Leaf Node Access                                             */
    /* -----------------------------------------------------------------------*/

    Data_PrimPage& GetNext();

    /* -----------------------------------------------------------------------*/
    /*! endchapter:  Leaf Node Access                                         */
    /* -----------------------------------------------------------------------*/

    Data_PageCount GetNumberOfEstimatedLeafNodes() const{
        return 0;
    }

    /*!
     * @copydoc Data_ITreeAccess::GetFileNo
     */
    FileDir_FileNo GetFileNo() const;

    void SetLruChainInfo(const Data_LRUChainInfo lruChainInfo);

private:

    // Copy and assignment operator are not supported

    Data_ScanTreeAccess& operator=( const Data_ScanTreeAccess &sta );

    Data_ScanTreeAccess( const Data_ScanTreeAccess &sta );

    Data_LRUChainInfo m_lruChainInfo;

protected:

    SAPDB_Bool GetLeftMostLeafNode(); 

    SAPDB_Bool GetNextLeafNode();

    void ReleaseLeafPage();

    void AssignToLeafPage( tbd_node_ptrs &pNodes );


protected:

    /*-------------------------------------------------------------------------
    declaration: m_RootPageNo
    description: Root pageNo of the tree
    --------------------------------------------------------------------------*/

    const Data_PageNo m_RootPageNo;

    /*-------------------------------------------------------------------------
    declaration: m_RecMode
    description: recovery mode of the tree => (Data_Dynamic, Data_Recoverable)
    --------------------------------------------------------------------------*/

    const Data_PageRecoveryMode m_RecMode;

    /*-------------------------------------------------------------------------
    declaration: m_Current
    description: 
    --------------------------------------------------------------------------*/

    tbd_current_tree    &m_Current;

    /*-------------------------------------------------------------------------
    declaration: m_PageAccessManager
    description: page access manager to access the leaves
    --------------------------------------------------------------------------*/

    Data_PageAccessManager  m_PageAccessManager;

    /*-------------------------------------------------------------------------
    declaration: m_LeafPage
    description: Used to do leaf page accesses
    --------------------------------------------------------------------------*/

    Data_PrimPage   m_LeafPage;
};

/*!*****************************************************************************

   endclass: Data_ScanTreeAccess

*******************************************************************************/

/*!*****************************************************************************

   class: Data_SerializableScanTreeAccess

   description: This class is used to 
   
              
*******************************************************************************/

class Data_SerializableScanTreeAccess : public Data_ScanTreeAccess
{
public:

    /* -----------------------------------------------------------------------*/
    /*! chapter: Constructors and initialization                              */
    /* -----------------------------------------------------------------------*/

    /*!----------------------------------------------------------------------
    function:     Data_SerializableScanTreeAccess()
    description:  Constructs an iterator to a primary tree which is interruptible,
                  i.e. the iterator will break himself if an exclusive request
                  to the current root node occures. The iterator uses a break 
                  and continue technic which is hidden for the caller
                  
    arguments:    Current [in/out] identifier for the file to be handled
    return value: none
    -----------------------------------------------------------------------*/

    Data_SerializableScanTreeAccess( tbd_current_tree    &current,
                                     Data_LRUChainInfo lruChainInfo = Data_LruNormal )

    :
    Data_ScanTreeAccess( current, lruChainInfo ),
    m_Allocator( *( reinterpret_cast< SAPDBMem_IRawAllocator* >( current.curr_trans->trAllocator_gg00 ))),
    m_pReEntranceKey( NULL ),
    m_ReEntranceKeyLength( 0 ),
    m_NumberOfReadLeafNodes( 0 )
    {}

    /*!----------------------------------------------------------------------
    function:     ~Data_SerializableScanTreeAccess()
    description:  The destructor resumes the consumed resources, specifically
                  the allocated memory.
    arguments:    none
    return value: none
    -----------------------------------------------------------------------*/

    ~Data_SerializableScanTreeAccess()
    {
        if(( NULL != m_pReEntranceKey ) && ( 0 != m_ReEntranceKeyLength ))
            m_Allocator.Deallocate( m_pReEntranceKey );
    }

    /* -----------------------------------------------------------------------*/
    /*! endchapter: constructors and initialization                           */
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*! chapter: Leaf Node Access                                             */
    /* -----------------------------------------------------------------------*/

    Data_PrimPage& GetNext();

    /* -----------------------------------------------------------------------*/
    /*! endchapter:  Leaf Node Access                                         */
    /* -----------------------------------------------------------------------*/

private:

    SAPDB_Bool Break();

    SAPDB_Bool Continue();

    SAPDB_Bool GetLeftMostLeafNode()
    {
        m_NumberOfReadLeafNodes++;
        return( Data_ScanTreeAccess::GetLeftMostLeafNode());
    }

    SAPDB_Bool GetNextLeafNode()
    {
        m_NumberOfReadLeafNodes++;
        return( Data_ScanTreeAccess::GetNextLeafNode());
    }

private:

    /*-------------------------------------------------------------------------
    declaration: m_Allocator
    description: Allocator used to get memory for the ReEntranceKey 
    --------------------------------------------------------------------------*/

    SAPDBMem_IRawAllocator  &m_Allocator;

    /*-------------------------------------------------------------------------
    declaration: m_pReEntranceKey
    description: Key of the last looked record before the interruption
                 takes place.
    --------------------------------------------------------------------------*/

    SAPDB_Byte  *m_pReEntranceKey;

    /*-------------------------------------------------------------------------
    declaration: m_ReEntranceKeyLength
    description: Length of the ReEntranceKey
    --------------------------------------------------------------------------*/

    SAPDB_Int2  m_ReEntranceKeyLength;

    /*-------------------------------------------------------------------------
    declaration: m_NumberOfReadLeafNodes
    description: Number of read leaf nodes since the last break;
    --------------------------------------------------------------------------*/

    SAPDB_UInt m_NumberOfReadLeafNodes;
};

/*!*****************************************************************************

   endclass: Data_SerializableScanTreeAccess

*******************************************************************************/

#endif  /* DATA_SCAN_TREE_ACCESS_HPP */

