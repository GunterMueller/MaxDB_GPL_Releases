/*****************************************************************************/
/*!

  @file   Data_RandomTreeAccess.cpp
  @author TorstenS

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

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "hbd30.h"
#include "ggg90.h"

#include "DataAccess/Data_RandomTreeAccess.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"
#include "RunTime/RTE_Message.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/



/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  GLOBAL VARIABLES                                                         *
 *===========================================================================*/

BitMapSlot BIT_MAP_SLOT_MASK[] = { 0x80, 0x40, 0x20, 0x10, 0x8, 0x4, 0x2, 0x1 };

// bitNo 0: 0x80 == 10000000
// bitNo 1: 0x40 == 01000000
// bitNo 2: 0x20 == 00100000

/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

class EstimateLeafCount
{
public:

    EstimateLeafCount()
    {
        for( Data_Level level = 0; level <= MAX_LEVEL_BD00; level++ )
        {
            m_Item[ level ].aliRecordCount        = 0;
            m_Item[ level ].aliNumberOfLevelPages = 0;
        }
    }

    void AddValue(
        const Data_PageCount    records,
        const Data_Level        level )
    {
        SAPDBERR_ASSERT_STATE( MAX_LEVEL_BD00 >= level );

        m_Item[ level ].aliRecordCount        += records;
        m_Item[ level ].aliNumberOfLevelPages +=1;
    }

    Data_PageCount Calculate() const
    {
        SAPDB_Real8  result = 1.0;

        for( Data_Level level = 0; level <= MAX_LEVEL_BD00; level++ )
        {
            if(  0 < m_Item[ level ].aliNumberOfLevelPages )
            {
                result *= (m_Item[ level ].aliRecordCount /
                           m_Item[ level ].aliNumberOfLevelPages);
            }
        }
        return( static_cast< Data_PageCount >( result ));
    }

private:

    struct AverageLevelItem
    {
        Data_PageCount  aliRecordCount;
        SAPDB_UInt4     aliNumberOfLevelPages;
    };

private:

    AverageLevelItem  m_Item[ MAX_LEVEL_BD00 +1 ];
};

/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/



/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

Data_RandomTreeAccess::Data_RandomTreeAccess(
    tgg00_TransContext      &trans,
    tbd_current_tree        &current,
    const SAPDB_UInt4       numberOfSamples,
    const SAPDB_Bool        bNoDuplicates )
        :
        m_Current( current ),
        m_TrError( current.curr_trans->trError_gg00 ),
        m_NoDuplicates( bNoDuplicates ),
        m_FinaleNumberOfSamples( numberOfSamples ),
        m_CurrentNumberOfSamples( 0 ),
        m_PageAccessManager( current ),
        m_Page(),
        m_BitMap( *( reinterpret_cast< SAPDBMem_IRawAllocator* >( trans.trAllocator_gg00 )) ),
        m_IndexNodes( *( reinterpret_cast< SAPDBMem_IRawAllocator* >( trans.trAllocator_gg00 )) ),
        m_HashAnchors( *( reinterpret_cast< SAPDBMem_IRawAllocator* >( trans.trAllocator_gg00 )) )
{
}

/*---------------------------------------------------------------------------*/

Data_RandomTreeAccess::~Data_RandomTreeAccess()
{
    m_BitMap.Delete();
    m_IndexNodes.Delete();
    m_HashAnchors.Delete();

    SAPDBErr_MessageList errlist;
    if (! m_Page.Deassign( errlist, false )) // release the page for read
        RTE_Message(errlist);
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
Data_RandomTreeAccess::Initialize( const Data_PageCount estimatedLeafNodes )
{
    if( ! SetToRootNode() )
        return( false );

    // Assumption that n leaf nodes needs n / 2 index node to address
    // The lower limit is 100 and the upper limit is SAPDB_MAX_UINT2 -1,
    // because SAPDB_MAX_UINT2 is the undefined value.

    SAPDB_UInt4 numEstimatedIndexNodes = SAPDB_MAX( 101, estimatedLeafNodes / 2 );
    // Do not use SAPDB_MIN because linux has problems with SAPDB_UInt2
    numEstimatedIndexNodes =
        numEstimatedIndexNodes > HashLink() ? HashLink() -1 : numEstimatedIndexNodes;

    // Assumption that an index node is filled about 90 percent and the
    // average separator length is about 40 bytes. Note that each index
    // node will be represented by one byte within the m_BitMap.
    // Upper limit is SAPDB_MAX_UINT4 -1, because SAPDB_MAX_UINT4 is the
    // undefined value.

    const SAPDB_UInt4 numBitMapSlots =
        SAPDB_MIN( BitMapLink() -1,(( NINETY_PERCENT_COV_BD00 / 40 ) /
                                    SAPDB_BITS_PER_BYTE ) * numEstimatedIndexNodes );

    // Hash size should be 20 percent of the amount of index nodes.
    // Lower limit is 10 and upper limit is SAPDB_MAX_UINT2 -1, because
    // SAPDB_MAX_UINT2 is the undefined value.

    SAPDB_UInt4 numHashAnchors = SAPDB_MAX( 11, ( numEstimatedIndexNodes / 5 ));
    // Do not use SAPDB_MIN because linux has problems with SAPDB_UInt2
    numHashAnchors = numHashAnchors > HashLink() ? HashLink() -1 : numHashAnchors;

    m_TrError = e_no_more_memory;

    if( ! m_BitMap.Reserve( numBitMapSlots ))
        return( SAPDB_FALSE );

    if( ! m_IndexNodes.Reserve( numEstimatedIndexNodes ))
        return( SAPDB_FALSE );

    if( ! m_HashAnchors.Initialize( numHashAnchors, HashLink() ))
        return( SAPDB_FALSE );

    m_TrError = e_ok;

    SAPDB_Bool retCode = AddIndexNode( m_Page.PageNo(), m_Page.GetRecordCount(),
                                       m_Page.GetLevel());

    SAPDBErr_MessageList errlist;
    if (! m_PageAccessManager.ReleasePage( m_Page, errlist ) ) // release the page for read
        RTE_Message(errlist);
    return( retCode );
}

/*---------------------------------------------------------------------------*/

Data_PrimPage&
Data_RandomTreeAccess::GetNext()
{
    if( m_Page.IsAssigned())
    {
        if( m_NoDuplicates )
            m_Page.SetLruChainInfo( Data_LruMiddle ); // for leaf pages
        else
            m_Page.SetLruChainInfo( Data_LruNormal );

        SAPDBErr_MessageList errlist;
        if (! m_PageAccessManager.ReleasePage( m_Page, errlist ) )
            RTE_Message(errlist);

        bd30ReleaseSubTree( m_Current );
    }

    if( EndIsReached() ) // => termination
        return( m_Page );

    if( ! SetToRootNode() )
        return( m_Page ); // page is empty => termination

    if( LEAF_LEVEL_BD00 == m_Page.GetLevel()) // root is leaf
    {
        SetToEnd();
        return( m_Page );
    }
    else
    {   // tree has at least one index level

        SAPDB_Bool  bAllSubNodesInUse = false;

        if( ! DetermineLeaf( bAllSubNodesInUse ))
        {
            if( m_Page.IsAssigned())
            {
                // no more pages available
                SAPDBErr_MessageList errlist;
                if (! m_PageAccessManager.ReleasePage( m_Page, errlist ) )
                    RTE_Message(errlist);
            }
        }
        else
        {
            m_CurrentNumberOfSamples ++;

            SAPDBERR_ASSERT_STATE( m_Page.IsAssigned() );
            SAPDBERR_ASSERT_STATE( LEAF_LEVEL_BD00 == m_Page.GetLevel() );
        }
        return( m_Page );
    }
}

/*---------------------------------------------------------------------------*/

Data_PageCount
Data_RandomTreeAccess::GetNumberOfEstimatedLeafNodes() const
{
    EstimateLeafCount   estimateLeafNodes;
    SAPDB_UInt4         nodeCount = 0;

    Container_Vector< HashLink >::ConstIterator iter    = m_HashAnchors.Begin();
    Container_Vector< HashLink >::ConstIterator endIter = m_HashAnchors.End();

    while( iter != endIter ) // loop over hash list
    {
        if(( *iter ).IsValid())
        {
            HashLink link = *iter;

            while( link.IsValid())
            {
                nodeCount++;
                const Data_IndexNode &node = GetNode( link );

                estimateLeafNodes.AddValue( node.GetRecordCount(), node.GetLevel());

                link = node.GetSuccessor();
            }
        }
        iter++;
    }
    SAPDBERR_ASSERT_STATE( nodeCount == m_IndexNodes.GetSize() );

    return( estimateLeafNodes.Calculate() );
}

/*---------------------------------------------------------------------------*/

FileDir_FileNo
Data_RandomTreeAccess::GetFileNo() const
{
    return FileDir_FileNo( m_Current.curr_tree_id.fileTabId_gg00() );
}

/*---------------------------------------------------------------------------*/
/*                        class Index Node                                   */
/*---------------------------------------------------------------------------*/

SAPDB_Bool
Data_IndexNode::Initialize()
{
    m_BitMapSlotsUsed = ( 0 == ( m_RecordCount % SAPDB_BITS_PER_BYTE )) ?
                        m_RecordCount / SAPDB_BITS_PER_BYTE : m_RecordCount / SAPDB_BITS_PER_BYTE + 1;


    const SAPDB_UInt4 bitMapSlotsAvailable =
        m_pBitMap->GetCapacity() - m_pBitMap->GetSize();

    if( bitMapSlotsAvailable < m_BitMapSlotsUsed )
    {
        const SAPDB_UInt4 newCapacity =
            m_pBitMap->GetCapacity() + ( m_pBitMap->GetCapacity() / 10 + bitMapSlotsAvailable );

        if( ! m_pBitMap->Reserve( newCapacity ))
            return( SAPDB_FALSE );
    }

    m_BitMapBaseIndex = m_pBitMap->GetSize(); // index of first free slot used by InsertEnd

    for( SAPDB_UInt slot = 0; slot < m_BitMapSlotsUsed; slot++ )
        m_pBitMap->InsertEnd( SAPDB_FALSE );

    return( SAPDB_TRUE );
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
Data_IndexNode::GetUnusedSubNodeIndex( Data_RecordIndex   &recordIndex ) const
{
    if( AllSubNodesUsed())
        return( SAPDB_FALSE ); // all sub nodes already used

    recordIndex = RandomValue( m_RecordCount -1 );

    const SAPDB_UInt4   stopRecordIndex = recordIndex;

    do
    {
        if( ! SubNodeIndexInUse( recordIndex ))
            return( SAPDB_TRUE );

        recordIndex = ( recordIndex + 1 ) % m_RecordCount;
    }
    while( recordIndex != stopRecordIndex );

    return( SAPDB_FALSE );
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
Data_IndexNode::SetSubNodeIndexToUsed( const Data_RecordIndex   recordIndex )
{
    if( recordIndex >= m_RecordCount )
        return( SAPDB_FALSE );

    if( SubNodeIndexInUse( recordIndex ))
        return( SAPDB_FALSE ); // sub node already set to used

    const SAPDB_UInt bitNo  = GetBitNo( recordIndex );
    BitMapSlot &bitMapSlot  = GetSlot( recordIndex );

    bitMapSlot = bitMapSlot | BIT_MAP_SLOT_MASK[ bitNo ];

    m_UsedCount += 1;

    return( SAPDB_TRUE );
}

/*---------------------------------------------------------------------------*/
/*                            private methods                                */
/*---------------------------------------------------------------------------*/

SAPDB_Bool
Data_RandomTreeAccess::AddIndexNode(
    const Data_PageNo       pageNo,
    const Data_RecordIndex  recordCount,
    const Data_Level        level )
{
    if( SearchNode( pageNo ).IsValid() )
        return( SAPDB_TRUE ); //already inserted

    HashLink        &anchor = GetHashAnchor( pageNo );
    Data_IndexNode  node( pageNo, recordCount, level, m_BitMap );

    if( ! node.Initialize())
    {
        m_TrError = e_no_more_memory;
        return( SAPDB_FALSE );
    }

    if( anchor.IsValid() ) // hash rechain before the node will be copied
        node.SetSuccessor( anchor );

    if( ! m_IndexNodes.InsertEnd( node ))
    {
        const SAPDB_UInt4 newCapacity =
            m_IndexNodes.GetCapacity() + ( m_IndexNodes.GetCapacity() / 10 + 1);

        if( ! m_IndexNodes.Reserve( newCapacity ))
        {
            m_TrError = e_no_more_memory;
            return( SAPDB_FALSE );
        }

        m_IndexNodes.InsertEnd( node );
    }
    anchor = ( m_IndexNodes.GetSize() -1 );

    return( SAPDB_TRUE );
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
Data_RandomTreeAccess::GetUnusedSubNodeIndex(
    const Data_PageNo   pageNo,
    Data_RecordIndex    &recordIndex ) const
{
    HashLink link = SearchNode( pageNo );

    if( link.IsInvalid() )
        return( SAPDB_FALSE );

    SAPDBERR_ASSERT_STATE( pageNo == GetNode( link ).GetPageNo() );

    return( GetNode( link ).GetUnusedSubNodeIndex( recordIndex ));
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
Data_RandomTreeAccess::SetSubNodeIndexToUsed(
    const Data_PageNo       pageNo,
    const Data_RecordIndex  recordIndex )
{
    HashLink link = SearchNode( pageNo );

    if( link.IsInvalid() )
        return( SAPDB_FALSE );

    SAPDBERR_ASSERT_STATE( pageNo == GetNode( link ).GetPageNo() );

    return( GetNode( link ).SetSubNodeIndexToUsed( recordIndex ));
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
Data_RandomTreeAccess::AllSubNodesUsed( const Data_PageNo   pageNo ) const
{
    HashLink link = SearchNode( pageNo );

    if( link.IsInvalid() )
        return( SAPDB_FALSE );

    SAPDBERR_ASSERT_STATE( pageNo == GetNode( link ).GetPageNo() );

    return( GetNode( link ).AllSubNodesUsed());
}

/*---------------------------------------------------------------------------*/

HashLink
Data_RandomTreeAccess::SearchNode(  const Data_PageNo   pageNo ) const
{
    HashLink  link = GetHashAnchor( pageNo );

    while( link.IsValid() )
    {
        const Data_IndexNode &node = GetNode( link );

        if( node.GetPageNo() == pageNo )
            return( link );
        else
            link = node.GetSuccessor();
    }
    HashLink hashLink;
    return( hashLink );
}
/*---------------------------------------------------------------------------*/

SAPDB_Bool
Data_RandomTreeAccess::DetermineLeaf( SAPDB_Bool     &bAllSubNodesInUse )
{
    SAPDBERR_ASSERT_STATE( m_Page.IsAssigned());

    if( LEAF_LEVEL_BD00 == m_Page.GetLevel() )
    {
        bAllSubNodesInUse = true;
        return( true );
    }
    {
        Data_RecordIndex    childIndex;
        tgg00_Rec           *pSeparator;
        Data_PageNo         childPagePageNo;
        const Data_PageNo   parentPageNo = m_Page.PageNo();
        const Data_Level    parentLevel  = m_Page.GetLevel();

        if( ! GetUnusedSubNodeIndex( parentPageNo, childIndex ))
        {
            SAPDBERR_ASSERT_STATE( AllSubNodesUsed( parentPageNo ));
            return( false );
        }

        pSeparator      = m_Page.GetRecord( childIndex );
        childPagePageNo = pSeparator->recPno_gg00().gg90GetInt4();

        SAPDBErr_MessageList errlist;
        if (! m_PageAccessManager.ReleasePage( m_Page, errlist ) )
            RTE_Message(errlist);

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
        
        if( LEAF_LEVEL_BD00 < m_Page.GetLevel() ) // leaf pages are used one time only
        {
            if( ! AddIndexNode( childPagePageNo, m_Page.GetRecordCount(), m_Page.GetLevel()))
                return( false );
        }
        if( ! DetermineLeaf( bAllSubNodesInUse ))
            return( false );

        if( bAllSubNodesInUse && m_NoDuplicates )
            SetSubNodeIndexToUsed( parentPageNo, childIndex );

        bAllSubNodesInUse = AllSubNodesUsed( parentPageNo );
        return( true );
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
Data_RandomTreeAccess::SetToRootNode()
{
    SAPDBERR_ASSERT_STATE( ! m_Page.IsAssigned() );
    SAPDBERR_ASSERT_STATE( NULL != m_Current.currRootNptrs_bd00.np_ptr() );

    SAPDBErr_MessageList errlist;
    if (! m_PageAccessManager.GetPage( m_Page, m_Current.curr_tree_id.fileRoot_gg00(), errlist ) )
        RTE_Message(errlist);
    return( e_ok == m_TrError );
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

SAPDB_Bool
Data_IndexNode::SubNodeIndexInUse( const Data_RecordIndex  recordIndex ) const
{
    const SAPDB_UInt bitNo       = GetBitNo( recordIndex );
    const BitMapSlot &bitMapSlot = GetSlot( recordIndex );

    return( 0 != ( bitMapSlot & BIT_MAP_SLOT_MASK[ bitNo ] ));
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/

