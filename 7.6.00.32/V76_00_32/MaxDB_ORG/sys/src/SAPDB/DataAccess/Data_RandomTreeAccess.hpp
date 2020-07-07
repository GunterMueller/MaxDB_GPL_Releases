/*****************************************************************************/
/*!

  @file   Data_RandomTreeAccess.hpp
  @author TorstenS

  @brief  Header file for class Data_RandomTreeAccess.

\if EMIT_LICENCE
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

\endif
*/
/*****************************************************************************/

#ifndef DATA_RANDOM_TREE_ACCESS_HPP
#define DATA_RANDOM_TREE_ACCESS_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gbd00.h"

#include "Container/Container_Vector.hpp"
#include "DataAccess/Data_ITreeAccess.hpp"
#include "DataAccess/Data_Types.hpp"
#include "DataAccess/Data_PageAccessManager.hpp"
#include "DataAccess/Data_PrimPage.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"
#include "KernelCommon/Kernel_Common.hpp"
#include "SAPDBCommon/SAPDB_EnhancedTypes.hpp"

#include <stdlib.h>

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

typedef SAPDB_Invalidatable< SAPDB_UInt2, SAPDB_MAX_UINT2>  HashLink;
typedef SAPDB_Invalidatable< SAPDB_UInt4, SAPDB_MAX_UINT4>  BitMapLink;
typedef SAPDB_Byte                                          BitMapSlot;


/*!
 * @class  Data_IndexNode
 * @author TorstenS
 * @brief  This class is used to 
 */   
class Data_IndexNode
{
public:
    
    /*!
     * @name Constructors and initialization
     */
    /*@{*/
      
    Data_IndexNode(
        const Data_PageNo               pageNo,
        const Data_PageCount            recordCount,
        const Data_Level                level,
        Container_Vector< BitMapSlot > &bitMap )
    :
    m_PageNo( pageNo ),
    m_BitMapBaseIndex(),
    m_Successor(),
    m_RecordCount( (SAPDB_UInt2) recordCount ),
    m_UsedCount( 0 ),
    m_Level( level ),
    m_BitMapSlotsUsed( 0 ),
    m_pBitMap( &bitMap )
    {}

    SAPDB_Bool Initialize();
    /*@}*/

    Data_PageNo GetPageNo() const
    {
        return( m_PageNo );
    }

    void SetSuccessor( const HashLink successor )
    {
        SAPDBERR_ASSERT_STATE( m_Successor != successor );

        m_Successor = successor;
    }

    HashLink GetSuccessor() const
    {
        return( m_Successor );
    }

    SAPDB_Bool GetUnusedSubNodeIndex( Data_RecordIndex   &recordIndex ) const;

    SAPDB_Bool SetSubNodeIndexToUsed( const Data_RecordIndex recordIndex );

    SAPDB_Bool AllSubNodesUsed() const
    {
        return( m_UsedCount == m_RecordCount );
    }

    Data_Level GetLevel() const
    {
        return( m_Level );
    }

    Data_PageCount GetRecordCount() const
    {
        return( m_RecordCount );
    }

private:

    SAPDB_UInt4 RandomValue( const SAPDB_UInt4 upperLimit ) const
    {
        if( 0 == upperLimit )
            return( 0 );
        else
            return(( rand() % upperLimit ));
    }

    BitMapSlot& GetSlot( const Data_RecordIndex  recordIndex )
    {
        const BitMapLink bitMapIndex =
            ( recordIndex / SAPDB_BITS_PER_BYTE ) + m_BitMapBaseIndex;

        return( ( *m_pBitMap )[ bitMapIndex ] );
    }

    const BitMapSlot& GetSlot( const Data_RecordIndex  recordIndex ) const
    {
        const BitMapLink bitMapIndex =
            ( recordIndex / SAPDB_BITS_PER_BYTE ) + m_BitMapBaseIndex;

        return( ( *m_pBitMap )[ bitMapIndex ] );
    }

    SAPDB_UInt GetBitNo( const Data_RecordIndex  recordIndex ) const
    {
        return( recordIndex % SAPDB_BITS_PER_BYTE );
    }

    SAPDB_Bool SubNodeIndexInUse( const Data_RecordIndex  recordIndex ) const;

    /*!
     * reference to the BitMap list of Data_RandomTreeAccess
     */
    Container_Vector< BitMapSlot >   *m_pBitMap; // PTS 1132314 M.Ki.

    /*!
     * Defines the page number of the index node which is described by
     * this handler.
     */
    const Data_PageNo m_PageNo;

    /*!
     * Defines the position within the bit list, where the "already
     * used information" of the current index node is stored.
     */
    BitMapLink m_BitMapBaseIndex;

    /*!
     * Links the current index node to another, i.e. they have the
     * same hash value.
     */
    HashLink m_Successor;

    /*!
     * Defines the number of records being stored on the coresponding
     * index node at the begining.  Caused by concurrent access to the
     * b* tree, it's possible that the number of separator-records
     * within one node is changed since the start of the random tree
     * access.
     */
    const SAPDB_UInt2 m_RecordCount;

    /*!
     * Defines how many separators of the current index node are used
     * and therefore never will be used again to avoid duplicates.  If
     * the number of m_UsedCount is equal to m_RecordCount all sub
     * nodes are used one time!
     */
    SAPDB_UInt2 m_UsedCount;

    /*!
     * Defines the b* tree index level of the current index node.
     * Note that the level is always greater than LEAF_LEVEL_BD00.
     */
    const Data_Level m_Level;

    /*!
     * Defines the number of bit slots used by this handler to manage
     * the already used sub nodes. If all sub nodes of the current
     * index node are used the corresponding bit is set. Thus this
     * brunch of the tree never will be used again.
     */
    SAPDB_UInt1 m_BitMapSlotsUsed;
};


/*!
 * @class  Data_RandomTreeAccess
 * @author TorstenS
 *
 * @brief This class is used to 
 */ 
class Data_RandomTreeAccess : public Data_ITreeAccess
{
public:
    /*!
     * @name Constructors and initialization
     */
    /*@{*/

    Data_RandomTreeAccess(
        tgg00_TransContext      &trans,
        tbd_current_tree        &current,
        const Data_PageCount    leafNodesToSample,
        const SAPDB_Bool        bNoDuplicates = true );

    SAPDB_Bool Initialize( const Data_PageCount estimatedLeafNodes );

    ~Data_RandomTreeAccess();
    /*@}*/


    /*!
     * @copydoc::Data_ITreeAccess::GetLeafNode
     */
    Data_PrimPage& GetNext();

    /*!
     * This method estimates the number of leaf nodes of the current
     * b*tree. The number will be estimated by the number of index
     * nodes used during the calls of GetLeafNode(). The more the
     * number of distinct index nodes used by the random accessor the
     * better is estimated leaf count is.
     */
    Data_PageCount GetNumberOfEstimatedLeafNodes() const;

    /*!
     * @copydoc Data_ITreeAccess::GetFileNo
     */
    FileDir_FileNo GetFileNo() const;

private:

    // Copy and assignment operator are not supported

    Data_RandomTreeAccess& operator=( const Data_RandomTreeAccess &rta );

    Data_RandomTreeAccess( const Data_RandomTreeAccess &rta );


    SAPDB_Bool AddIndexNode(
        const Data_PageNo       pageNo,
        const Data_RecordIndex  recordCount,
        const Data_Level        level );

    SAPDB_Bool GetUnusedSubNodeIndex(
        const Data_PageNo   pageNo,
        Data_RecordIndex    &recordIndex ) const;

    SAPDB_Bool SetSubNodeIndexToUsed(
        const Data_PageNo       pageNo,
        const Data_RecordIndex  recordIndex );

    SAPDB_Bool AllSubNodesUsed( const Data_PageNo   pageNo ) const;

    const HashLink& GetHashAnchor( const Data_PageNo    pageNo ) const
    {
        return( m_HashAnchors[ pageNo % m_HashAnchors.GetCapacity() ]);
    }

    HashLink& GetHashAnchor( const Data_PageNo  pageNo )
    {
        return( m_HashAnchors[ pageNo % m_HashAnchors.GetCapacity() ]);
    }

    const Data_IndexNode& GetNode( HashLink link ) const
    {
        return( m_IndexNodes[ link() ]);
    }

    Data_IndexNode& GetNode( HashLink link )
    {
        return( m_IndexNodes[ link() ]);
    }

    HashLink SearchNode(  const Data_PageNo   pageNo ) const;

    SAPDB_Bool DetermineLeaf( SAPDB_Bool    &bAllSubNodesInUse );

    SAPDB_Bool SetToRootNode();

    /*!
     * The randomTreeAccessor will stop if the number of examined leaf
     * nodes is greater than a user given limit. If the end is reached
     * true is returned.
     *
     * @return 
     */
    SAPDB_Bool EndIsReached() const
    {
        return( m_CurrentNumberOfSamples >= m_FinaleNumberOfSamples );
    }

    /*!
     * This method is used to set the randomTreeAccessor to the end.
     * The subsequent call of GetLeafNode provides an empty page and
     * the accessor will stop.
     */
    void SetToEnd()
    {
        m_CurrentNumberOfSamples = m_FinaleNumberOfSamples;
    }

private:


    /*!
     * TEMPORARY SOLUTION as long as treelocklist exist for primary data!
     */
    tbd_current_tree    &m_Current;

    /*!
     * base error
     */
    tgg00_BasisError    &m_TrError;

    /*!
     * If m_NoDuplicates is defined each leaf of the current tree will
     * be accessed at most one time. Duplicates are prohibited.
     */
    const SAPDB_Bool    m_NoDuplicates;

    /*!
     * Defines the maximum number of samples to be made before the
     * randomTreeAccessor will stop.
     */
    const SAPDB_UInt4 m_FinaleNumberOfSamples;

    /*!
     * Defines the current number of samples.
     */
    SAPDB_UInt4 m_CurrentNumberOfSamples;

    /*!
     * Handle for the page access manager to get pages.
     */
    Data_PageAccessManager  m_PageAccessManager;

    /*!
     * Handle for a data base page.
     */
    Data_PrimPage   m_Page;

    /*!
     * collection of bit slots to store the "already used information"
     * of a single index node. This member data is some kind of an
     * allocator used by the the class Data_IndexNode.
     */
    Container_Vector< BitMapSlot >  m_BitMap;

    /*!
     * collection of index nodes used to store the "already used
     * information". This member data is some kind of an allocator
     * used by the the class Data_IndexNode.
     */
    Container_Vector< Data_IndexNode >   m_IndexNodes;

    /*!
     * Hashlist which contains a hash anchors for the index nodes
     * stored within m_IndexNodes and being accessed by the
     * randomTreeAccessor.
     */
    Container_Vector< HashLink >   m_HashAnchors;
};

#endif  /* DATA_RANDOM_TREE_ACCESS_HPP */

