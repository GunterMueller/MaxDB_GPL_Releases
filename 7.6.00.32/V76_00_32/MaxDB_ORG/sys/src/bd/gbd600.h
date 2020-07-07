/*!
  @file           gbd600.h
  @ingroup        Node Handling
  @author         UweH
  @brief          defines class cbd600_Node

    ========== licence begin  GPL
    Copyright (c) 1999-2005 SAP AG

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
*/


#ifndef GBD600_H
#define GBD600_H

#include "gsp00.h"
#include "ggg00.h"
#include "gbd00.h"
#include "gsp03.h"
#include "gsp03_3.h"

#include "gbd300.h"
#include "gbd610.h" // cbd610_RecordList
#include "hbd13.h"
#include "hgg01_1.h"

#if COMPILEMODE_MEO00 >= SLOW_MEO00 
#include "hta99.h"
#endif

#include "SAPDB/SAPDBCommon/SAPDB_RangeCode.hpp"
#include "ConsistentRead/CnsRead_Manager.hpp"
#include "Transaction/Trans_Context.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "DataAccess/Data_PageNoList.hpp"

class cbd500_Tree;
class cbd500_SubTree;
class cbd800_PrimaryTree;
class Converter_PageCollector;

#define UNDER_FLOW_LIMIT_BD600                HALF_COVERING_BD00
#define IS_CHANGED_BD600                      true
#define IS_LEFT_NODE_GBD600                   true
#define IS_RIGHT_NODE_GBD600                  false
#define TRUE                                  1
#define VERSION_COMPRESSED_SUBTREE_LEAF_BD600 1

/*!
@class          cbd600_Node
@brief          methodes to handle Node-Pages
*/
class cbd600_Node
{
	friend class cbd500_Tree;
	friend class cbd500_SubTree;
	friend class cbd800_PrimaryTree;
	friend class cbd400_InvTree;
public:

    /*!
       @brief          copy constructor.
       @param          Node [in] Node to copy.
     */

	cbd600_Node (const cbd600_Node &Node);

    /*!
       @brief          this is for accessing the root node.
       @param          Current [in/out] for TrError, TaskID.
       @param          PageLockMode [in] How the Page of the node is locked. (default = CacheLock)
     */

	cbd600_Node (cbd300_InvCurrentBasis   &Current,
		         const tbd00_PageLockMode &PageLockMode = tbd00_PageLockMode::fromConst(plmLock_ebd00));
    /*!
       @brief          This constructs an empty node, which can be used to access any node.
       @param          Current [in/out] for TrError, TaskID.
       @param          RequestKind [in] How pages are requested from the Datacache (for_read/for_update)
       @param          PageLockMode [in] How the Page of the node is locked. (default = CacheLock)
       @param          isCached     [in] If this is true every GetNode only retrieves the page into a local buffer
     */

	cbd600_Node (cbd300_InvCurrentBasis   &Current,
		         const tbd_node_request   &RequestKind,
		         const tbd00_PageLockMode &PageLockMode  = tbd00_PageLockMode::fromConst(plmLock_ebd00),
                 const tbd_nodeptr         pCache        = 0);
    /*!
       @brief          create a nodehandle and get specified page.
       @param          Current [in/out] for TrError, TaskID.
       @param          RequestKind [in] How pages are requested from the Datacache (for_read/for_update)
       @param          Pno [in] Page Number
       @param          PageLockMode [in] How the Page of the node is locked. (default = CacheLock)
     */

	cbd600_Node (cbd300_InvCurrentBasis   &Current,
		         const tbd_node_request   &RequestKind,
				 tsp00_PageNo              Pno,
		         const tbd00_PageLockMode &PageLockMode = tbd00_PageLockMode::fromConst(plmLock_ebd00));
    /*!
       @brief          create a nodehandle and use the given CacheHandle NPTR.
       @param          Current [in/out] for TrError, TaskID.
       @param          RequestKind [in] How pages are requested from the Datacache (for_read/for_update)
       @param          Nptr [in/out] DataCacheHandle
       @param          PageLockMode [in] How the Page of the node is locked. (default = CacheLock)

     */

	cbd600_Node (cbd300_InvCurrentBasis   &Current,
		         const tbd_node_request   &RequestKind,
				 tbd_node_ptrs            &Nptr,
		         const tbd00_PageLockMode &PageLockMode = tbd00_PageLockMode::fromConst(plmLock_ebd00));
    /*!
       @brief          create a nodehandle and use the given buffer, no DataCache is used.
       @param          Current [in/out] for TrError, TaskID.
       @param          Buffer [in/out] Node Buffer
       @param          PageLockMode [in] How the Page of the node is locked. (default = CacheLock)
     */

	cbd600_Node (cbd300_InvCurrentBasis   &Current,
		         tbd_node                 &Buffer,
		         const tbd00_PageLockMode &PageLockMode = tbd00_PageLockMode::fromConst(plmLock_ebd00));

    /// releases the DataCache Page if there is one, checks, if updated

	~cbd600_Node ();
    /*!
       @brief  Given length is reserved in the page, at the given position, if there
               is not enough space e_no_more_space is set.
       @param  RequiredLen [in] Total record length
       @param  RecIndex [in] position, where the new record must be inserted
       @param  pSpace [out] Points to the ByteString, where the requested space is available
       @param  withConsistentReadInfo [in]
       @param  withSpaceCheck [in] default=true
     */
    void              bd600AddRecordSpace (SAPDB_Int4    RequiredLen,
                                           SAPDB_Int4    RecIndex,
                                           SAPDB_Byte*  &pSpace,
                                           bool          withConsistentReadInfo,
                                           bool          withSpaceCheck = true);
	void              bd600BuildSeparatorKey (cbd600_Node  &RightNeighbor,
								              tsp00_KeyPtr &pSepKey,
								              tsp00_Int2   &SepKeyLen) const;
	void              bd600BuildSeparatorKey (bool          bNodeIsLeft,
								              tsp00_KeyPtr  pNeighborKey,
                                              tsp00_Int4    NeighborKeyLen,
                                              tsp00_KeyPtr &pNewSepKey,
                                              tsp00_Int2   &NewSepKeyLen) const;
    void              bd600Check             (const bool bWithExtendedCheck) const;
    void              bd600CheckNode         ( bool bExtendedCheck ) const;
    void              bd600CheckWithErrorHandling() const;
	void              bd600CheckIndex        () const;
	void              bd600CopyBody          (cbd600_Node &SrcNode);
	void              bd600CountRecords      (tsp00_KeyPtr  pStartKey, 
		                                      tsp00_Int4    StartKeyLen, 
										      tsp00_KeyPtr  pStopKey,
										      tsp00_Int4    StopKeyLen,
		                                      tsp00_Int4   &RecordCnt) const;
	tsp00_Int4        bd600Covering          () const;
	void              bd600DelRecordSpace    (tsp00_Int4 RecIndex);
	void              bd600Dump              (tsp00_Int4 ErrorId, const char *MsgText /*tsp00_C24*/) const;
    void              bd600EvalSepKeyLen     (tsp00_KeyPtr pKey,
							                  tsp00_Int4   KeyLen,
							                  tsp00_KeyPtr pNewSepKey,
							                  tsp00_Int2   &NewSepKeyLen) const;
	void              bd600Exchange          (cbd600_Node &Neighbor);
	void              bd600Free              ();
    tsp00_PageNo      bd600GetId             () const;
    tsp00_Int4        bd600PageConverterVersion () const;
    tbd_nodeptr       bd600GetNodePtr        ();
    void              bd600GetNode           (tsp00_PageNo            PageNo,
					 			              const tbd_node_request &RequestKind);
    void              bd600GetNode           (tsp00_PageNo PageNo);
    void              bd600GetStatistic      (tgg00_SampleInfo  &Info,
							                  tbd_stat_aux_vars &StatAuxVars,
							                  bool               bWithSelectivity) const;
	bool              bd600IsAccessable      () const;
    bool              bd600IsLeaf            () const;
    bool              bd600IsSubTreeLeaf     () const;
    bool              bd600IsRoot            () const;
    bool              bd600IsUpdateable      () const;
    bool              IsSubTreeLeaf          () const; 

	tsp00_Int4       &bd600LeafCount         ();
	tsp00_Int4        bd600LeafCount         (const tsp00_Int4 StartRecIndex,
		                                      const tsp00_Int4 StopRecIndex) const;
    tsp00_Int4   	  bd600Level             () const;
	tsp00_PageNo      bd600RightNeighbor     () const;
    tsp00_PageNo      bd600LeftNeighbor      () const;
	tsp00_Int4   	  bd600MaxRecIndex       () const;
    void              bd600Move              (tsp00_PageNo NewId);
	void			  bd600New               (tsp00_Int4 Level);
	bool              bd600NodeIsRequested   () const; 
	tsp00_Int4        bd600NumRecords        () const;
    tsp00_Int4        bd600PrimKeyCount      (const tsp00_Int4 StartRecIndex, 
						                      const tsp00_Int4 StopRecIndex) const;
    tsp00_Int2        bd600GetKeyLength      (tsp00_Int4 RecIndex) const;
	tsp00_BytePtr     bd600SpacePtr          (tsp00_Int4 RecIndex);
    const tsp00_BytePtr bd600SpacePtr        (tsp00_Int4 RecIndex) const;
    tgg00_RecPtr      bd600RecPtr            (tsp00_Int4 RecIndex);
    const tgg00_RecPtr bd600RecPtr           (tsp00_Int4 RecIndex) const;
    tgg00_RecPtr      bd600SepPtr            (tsp00_Int4 RecIndex);
    const tgg00_RecPtr bd600SepPtr           (tsp00_Int4 RecIndex) const;
    bool              bd600GetKey            (tsp00_Int4     RecIndex,
                                              tsp00_KeyPtr  &pKey,
                                              tsp00_Int2    &Length);
    bool              bd600GetSpace          (tsp00_Int4     RecIndex,
                                              tsp00_BytePtr &pSpace,
                                              tsp00_Int4    &Length);
    tsp00_Int4          bd600RecOffset       (tsp00_Int4 RecIndex) const;
    tgg00_RecPtr        bd600FirstRecPtr     () const;
    void                bd600SetDeleted      (tsp00_Int4 RecIndex,
                                              bool       isDeleted);

    static tgg00_RecPtr bd600FirstRecPtr     (tbd_nodeptr pNode);
	
    void              bd600Release           (bool IsChanged);
	void              bd600SearchRecord      (tsp00_KeyPtr      pKey,
		                                      tsp00_Int4        KeyLen,
										      tsp00_Int4       &RecIndex,
										      tbd_searchresult &SearchResult) const;
   	void             bd600SearchRecordConsistent     (tsp00_KeyPtr          pKey,
                                                      tsp00_Int4            KeyLen,
                                                      const tgg91_TransNo*  pWriteTrans,
                                                      tsp00_Int4            &RecIndex,
										              tbd_searchresult      &SearchResult) const;
   	void              bd600SearchStopKey      (tsp00_KeyPtr     pKey,
		                                      tsp00_Int4        KeyLen,
										      tsp00_Int4       &RecIndex,
										      tbd_searchresult &SearchResult) const;

 	bool              bd600SpaceIsAvailable  (tsp00_Int4 RequiredLen) const;
	tsp00_Int4        bd600SumKeyLen         () const;
	tgg00_BasisError &bd600TrError           ();
	tgg00_BasisError  bd600TrError           () const;
	void              bd600UpdRecordSpace    (tsp00_Int4   RecSizeDelta,
										      tsp00_Int4   RecIndex,
									          tgg00_RecPtr pRec);
    void              bd600Print             (bool withBody = false) const;
    void              bd600DisableCaching    (tbd_nodeptr &pCache);
    void              bd600EnableCaching     (tbd_nodeptr  pCache);
    bool              bd600IsCached() const;

    void              bd600CountClusteredPages ( cbd500_Tree             &invTree,
                                                 Converter_PageCollector &pageCollection,
                                                 Data_PageNoList         &pageNoList,
                                                 tsp00_Int4              &LeafPageCount,
                                                 tsp00_Int4              &ClusteredBlockCount) const;

public:

    class Iterator
    {
    public:
        Iterator(const cbd600_Node & node,
                 tsp00_Int4          recindex = -1);
        bool IsValid() const;
        Iterator& operator ++ ();
        const tgg00_RecPtr operator * () const;
        tsp00_Int4 Index() const;
    private:
        const cbd600_Node &m_Node;
        tsp00_Int4         m_RecIndex;
    };
    
    Iterator bd600GetFirstRecord() const;

protected:

	tgg00_BasisError       &m_TrError;       ///< For a convenient access
	tbd_nodeptr            &m_pNode;         ///< For a convenient access
	cbd300_InvCurrentBasis &m_Current;       ///< Transactions context, necessary for
	                                         ///< DataCache operations and errorhandling
    tbd_node_ptrs           m_NodePtrs;      ///< DataCache: Anchor to the page and the Caches Control Block
	tbd_node_request        m_RequestKind;   ///< DataCache: for_update = locked exclusive for_read = locked shared
	tbd00_PageLockMode      m_PageLockMode;  ///< DataCache: tbd00_PageLockMode::fromConst(plmLock_ebd00) means the DC controls the concurrent page access
	cbd610_RecordList       m_RecordList;    ///< The OffsetList for the Records
	tbd_nodeptr             m_pCachedPage;   ///< If this is set, every node is read from the data cache into this local buffer

	void             bd600_GetPageFromDataCache    (tsp00_PageNo      PageNo);
    void             bd600_CheckLeafCount          () const;
    void             bd600_GetIndexNodeStatistic   (tgg00_SampleInfo  &SampleInfo,
		                                            tbd_stat_aux_vars &StatAuxVars) const;
	void             bd600_GetInvLeafStatistic     (tgg00_SampleInfo  &SampleInfo) const;
	void             bd600_GetInvLeafSelectivity   (tgg00_SampleInfo  &SampleInfo) const;
	void             bd600_GetSubTreeLeafStatistic (tgg00_SampleInfo  &SampleInfo) const;
    void             bd600_GetSubTreeStatistics    (tgg00_RecPtr       pRec,
									                tgg00_SampleInfo  &SampleInfo) const;
    void             bd600SearchRecordNonConsistent  (tsp00_KeyPtr      pKey,
                                                      tsp00_Int4        KeyLen,
                                                      tsp00_Int4       &RecIndex,
                                                      tbd_searchresult &SearchResult) const;
    void             bd600MigrateSubTreeLeaf();

    void             bd600CountClusteredPagesOfLeafNode ( cbd500_Tree &invTree,
                                                         tsp00_Int4  &LeafPageCount,
                                                         tsp00_Int4  &ClusteredBlockCount) const;

    tsp00_Int4       bd620_CalcBlockLenAndSetIndexToNextBlock (const cbd610_RecordList &AuxRecordList,
										                       tsp00_Int4  &CurrIndex, 
										                       tsp00_Int4   MaxIndex);
 	void			 bd620_DeleteRecordsFromPage     (bool       bRightDistribution,
												      tsp00_Int4 RecIndex);
	void			 bd620_FindOptimDistribForDelete (cbd600_Node &Neighbor,
													  bool         bRightDistribution,
													  tsp00_Int4   Covering,
													  tsp00_Int4   NeighborCovering,
													  bool        &bDistributionFound,
													  tsp00_Int4  &OptimRecIndex);
	void			 bd620_FindOptimDistribForInsert (bool              bRightDistribution,
													  tsp00_Int4        RequiredLen,
													  tsp00_Int4        NeighborCovering,
													  tsp00_Int4        RecIndex,
													  bool             &bDistributionFound,
													  tsp00_Int4       &OptimRecIndex,
													  bool             &bMoveNewRecordIntoNeighbor);
	void			 bd620_FindOptimDistribForUpdate (bool              bRightDistribution,
													  tsp00_Int4        RequiredLen,
													  tsp00_Int4        NeighborCovering,
													  tsp00_Int4        RecIndex,
													  bool             &bDistributionFound,
													  tsp00_Int4       &OptimRecIndex,
    												  bool             &bMoveRecordIntoNeighbor);
	void             bd620_MergeNodes                (cbd600_Node &LeftNode);
};

/*---------------------------------------------------------------------------*/

inline void
cbd600_Node::bd600CopyBody (cbd600_Node &SrcNode)
{
	ROUTINE_DBG_MEO00 ("bd600CopyBody");

	SAPDB_RangeMove( __FILE__, 1,    
		sizeof (*SrcNode.m_pNode), sizeof(*m_pNode),
		SrcNode.m_pNode, BODY_BEG_BD00, 
		m_pNode,         BODY_BEG_BD00,
		SrcNode.m_pNode->nd_bottom() - BODY_BEG_BD00, m_TrError);
	if ( e_ok != m_TrError ) return;

    if ( ! m_RecordList.Copy(SrcNode.m_RecordList,
                             Trans_Context::GetContext(*m_Current.curr_trans).ErrorList()) )
    {
        m_TrError = e_move_error;
        return;
    }
    
#   ifdef SAPDB_SLOW 
	t01p2int4 (bd_inv, "CopyNodeBody", SrcNode.m_pNode->nd_id(),
		               "to Node  #> ", m_pNode->nd_id());
#   endif
}

/*---------------------------------------------------------------------------*/

inline void
cbd600_Node::bd600CountRecords (tsp00_KeyPtr  pStartKey, 
								tsp00_Int4    StartKeyLen, 
								tsp00_KeyPtr  pStopKey,
								tsp00_Int4    StopKeyLen,
								tsp00_Int4   &RecordCnt) const
{
	ROUTINE_DBG_MEO00 ("bd600CountRecords");
	
	tsp00_Int4       StartRecIndex;
	tbd_searchresult KeySearchResult;
	
	bd600SearchRecord (pStartKey, StartKeyLen, StartRecIndex, KeySearchResult);
	if (lastfound == KeySearchResult || nonefound == KeySearchResult)
		RecordCnt = 0;
	else
	{
		tsp00_Int4       StopRecIndex;
		
		bd600SearchRecord (pStopKey, StopKeyLen, StopRecIndex, KeySearchResult);
		if (nextfound != KeySearchResult)
			RecordCnt = StopRecIndex - StartRecIndex + 1;
		else
			RecordCnt = StopRecIndex - StartRecIndex;
	}			
}

/*---------------------------------------------------------------------------*/

inline tsp00_Int4
cbd600_Node::bd600RecOffset (tsp00_Int4 RecIndex) const
{
    return m_RecordList.GetOffset(RecIndex);
}

/*---------------------------------------------------------------------------*/

inline tsp00_BytePtr
cbd600_Node::bd600SpacePtr (tsp00_Int4 RecIndex)
{
	return reinterpret_cast<tsp00_BytePtr>(m_pNode) + bd600RecOffset(RecIndex);
}

/*---------------------------------------------------------------------------*/

inline const tsp00_BytePtr
cbd600_Node::bd600SpacePtr (tsp00_Int4 RecIndex) const
{
    return reinterpret_cast<const tsp00_BytePtr>(m_pNode) + bd600RecOffset(RecIndex);
}

/*---------------------------------------------------------------------------*/

inline tgg00_RecPtr
cbd600_Node::bd600RecPtr (tsp00_Int4 RecIndex)
{
    if ( bd600IsSubTreeLeaf() )
        g01abort (csp3_bd_msg, csp3_n_btree, "Operation not possible: ", m_pNode->nd_id());
    return reinterpret_cast<tgg00_RecPtr>(bd600SpacePtr(RecIndex));
}

/*---------------------------------------------------------------------------*/

inline const tgg00_RecPtr
cbd600_Node::bd600RecPtr (tsp00_Int4 RecIndex) const
{
    if ( bd600IsSubTreeLeaf() )
        g01abort (csp3_bd_msg, csp3_n_btree, "Operation not possible: ", m_pNode->nd_id());
    return reinterpret_cast<const tgg00_RecPtr>(bd600SpacePtr(RecIndex));
}

/*---------------------------------------------------------------------------*/

inline tgg00_RecPtr
cbd600_Node::bd600SepPtr (tsp00_Int4 RecIndex)
{
    if ( LEAF_LEVEL_BD00 == m_pNode->nd_level() )
        g01abort (csp3_bd_msg, csp3_n_btree, "Operation not possible: ", m_pNode->nd_id());
    return reinterpret_cast<tgg00_RecPtr>(bd600SpacePtr(RecIndex));
}

/*---------------------------------------------------------------------------*/

inline const tgg00_RecPtr
cbd600_Node::bd600SepPtr (tsp00_Int4 RecIndex) const
{
    if ( LEAF_LEVEL_BD00 == m_pNode->nd_level() )
        g01abort (csp3_bd_msg, csp3_n_btree, "Operation not possible: ", m_pNode->nd_id());
    return reinterpret_cast<const tgg00_RecPtr>(bd600SpacePtr(RecIndex));
}

/*---------------------------------------------------------------------------*/

inline bool
cbd600_Node::bd600GetKey (tsp00_Int4    RecIndex,
                          tsp00_KeyPtr &pKey,
                          tsp00_Int2   &Length)
{
    return m_RecordList.GetKey (RecIndex, Length, pKey);
}

/*---------------------------------------------------------------------------*/

inline bool
cbd600_Node::bd600GetSpace (tsp00_Int4     RecIndex,
                            tsp00_BytePtr &pSpace,
                            tsp00_Int4    &Length)
{
    bool withConsistentReadInfo;
    return m_RecordList.GetRecord (RecIndex, withConsistentReadInfo, Length, pSpace);
}

/*---------------------------------------------------------------------------*/

inline tgg00_RecPtr
cbd600_Node::bd600FirstRecPtr (tbd_nodeptr pNode)
{
    if ( pt2InvSubtree_egg00 == pNode->nd_pt2()
         &&
         LEAF_LEVEL_BD00 == pNode->nd_level() )
        g01abort (csp3_bd_msg, csp3_n_btree, "Operation not possible: ", pNode->nd_id());

	cbd610_RecordList auxList ( pNode );

    return reinterpret_cast<tgg00_RecPtr>( reinterpret_cast<tsp00_BytePtr>(pNode)
                                           +
                                           auxList.GetOffset(FIRST_REC_INDEX_BD00) );
}

/*---------------------------------------------------------------------------*/

inline void
cbd600_Node::bd600SetDeleted (tsp00_Int4 RecIndex,
                              bool       isDeleted)
{
    m_RecordList.SetDeleted (RecIndex, isDeleted);
}

/*---------------------------------------------------------------------------*/

inline tgg00_RecPtr
cbd600_Node::bd600FirstRecPtr() const
{
     return bd600FirstRecPtr( m_pNode );
}

/*---------------------------------------------------------------------------*/

inline bool
cbd600_Node::bd600IsAccessable () const
{
	return NULL != m_NodePtrs.np_ptr();
}

/*---------------------------------------------------------------------------*/

inline void
cbd600_Node::bd600_GetPageFromDataCache (tsp00_PageNo PageNo)
{
    ROUTINE_DBG_MEO00 ("bd600_GetPageFromDataCache");

    if ( bd600IsAccessable() )
        g01abort (csp3_bd_msg, csp3_n_btree, "bd600GetNode: np_ptr!=NIL", m_pNode->nd_id());

	bd13GetNode (m_Current, PageNo, m_PageLockMode, m_RequestKind, m_NodePtrs);
	if (e_ok != m_TrError)
        return;
        
    if ( m_pCachedPage != 0 // PTS 1136998 UH 2006-01-23
         &&
         bd600IsLeaf() && ! bd600IsRoot()
         &&
         nr_for_read == m_RequestKind )
    {
        SAPDB_RangeMove( __FILE__, 1,    
            sizeof (*m_pNode), sizeof(*m_pNode),
            m_pNode,           POS_OFF_DIFF_BD00, 
            m_pCachedPage,     POS_OFF_DIFF_BD00,
            sizeof (*m_pNode), m_TrError);
        b13r_release_node (m_NodePtrs, m_Current, lru_normal);
        m_NodePtrs.np_ptr()   = m_pCachedPage;
        m_NodePtrs.np_cbptr() = 0;
    }

    m_RecordList.Assign(m_pNode);

    if ( pt2InvSubtree_egg00 == m_pNode->nd_pt2()
         &&
         nr_for_update == m_RequestKind
         &&
         LEAF_LEVEL_BD00 == m_pNode->nd_level()
         &&
         m_pNode->ndPageVersion_bd00() < VERSION_COMPRESSED_SUBTREE_LEAF_BD600 )
    {
        bd600MigrateSubTreeLeaf();
    }
}

/*---------------------------------------------------------------------------*/

inline void
cbd600_Node::bd600GetNode (tsp00_PageNo            PageNo,
					       const tbd_node_request &RequestKind)
{
    ROUTINE_DBG_MEO00 ("bd600GetNode");

    m_RequestKind = RequestKind;

	bd600_GetPageFromDataCache (PageNo);
}

/*---------------------------------------------------------------------------*/

inline void
cbd600_Node::bd600GetNode (tsp00_PageNo PageNo)
{
    ROUTINE_DBG_MEO00 ("bd600GetNode");

    bd600_GetPageFromDataCache (PageNo);
}

/*---------------------------------------------------------------------------*/

inline void
cbd600_Node::bd600New (tsp00_Int4 Level)
{
    ROUTINE_DBG_MEO00 ("bd600New");

	if (NULL != m_pNode)
        g01abort (csp3_bd_msg, csp3_n_btree, "GBD600: New not allowed  ", m_pNode->nd_id());

	b13new_node (Level, m_NodePtrs, m_Current);
	if (e_ok != m_TrError)
        return;
    
	if ( LEAF_LEVEL_BD00 == Level )
    {
        m_pNode->ndLeafCount_bd00() = 1;
        if ( pt2InvSubtree_egg00 == m_pNode->nd_pt2() )
            m_pNode->ndPageVersion_bd00() = 1;
    }

    m_RecordList.Assign(m_pNode);
}

/*---------------------------------------------------------------------------*/

inline tsp00_Int4
cbd600_Node::bd600Covering () const
{
    ROUTINE_DBG_MEO00 ("bd600Covering");

	return m_NodePtrs.np_ptr()->nd_bottom() - BODY_BEG_BD00 +
		   (m_NodePtrs.np_ptr()->nd_record_cnt() << DIVISOR_POINTERSIZE_BD00);
}


/*---------------------------------------------------------------------------*/

inline void
cbd600_Node::bd600Exchange (cbd600_Node &Neighbor)
{
	ROUTINE_DBG_MEO00 ("bd600Exchange");

#   ifdef SAPDB_SLOW 
	t01p2int4 (bd_inv, "Exchange:   ", (NULL!=m_pNode)?(m_pNode->nd_id()):(-1),
		               "with Node:  ", (NULL!=Neighbor.m_pNode)?(Neighbor.m_pNode->nd_id()):(-1));
#   endif

    tbd_node_ptrs     AuxNodePtrs   = m_NodePtrs;
    cbd610_RecordList AuxRecordList = m_RecordList;

    m_NodePtrs            = Neighbor.m_NodePtrs;
    m_RecordList          = Neighbor.m_RecordList;

    Neighbor.m_NodePtrs   = AuxNodePtrs;
	Neighbor.m_RecordList = AuxRecordList;
}

/*---------------------------------------------------------------------------*/

inline bool
cbd600_Node::bd600IsLeaf() const
{
	return LEAF_LEVEL_BD00 == m_NodePtrs.np_ptr()->nd_level();
}

/*---------------------------------------------------------------------------*/

inline bool
cbd600_Node::bd600IsUpdateable () const
{
	return nr_for_update == m_RequestKind;
}

/*---------------------------------------------------------------------------*/

inline bool
cbd600_Node::bd600IsRoot() const
{
	return m_NodePtrs.np_ptr()->nd_id()== m_NodePtrs.np_ptr()->nd_root();
}

/*---------------------------------------------------------------------------*/
inline bool
cbd600_Node::bd600IsSubTreeLeaf () const
{
    return pt2InvSubtree_egg00 == m_pNode->nd_pt2()
           &&
           LEAF_LEVEL_BD00 == m_pNode->nd_level();
}

/*---------------------------------------------------------------------------*/

inline tsp00_Int4
cbd600_Node::bd600Level() const
{
	return m_NodePtrs.np_ptr()->nd_level();
}

/*---------------------------------------------------------------------------*/

inline tsp00_PageNo
cbd600_Node::bd600RightNeighbor() const
{
	return m_NodePtrs.np_ptr()->nd_right();
}

/*---------------------------------------------------------------------------*/

inline tsp00_PageNo
cbd600_Node::bd600LeftNeighbor() const
{
	return m_NodePtrs.np_ptr()->nd_left();
}

/*---------------------------------------------------------------------------*/

inline tsp00_PageNo
cbd600_Node::bd600GetId () const
{
	return m_NodePtrs.np_ptr()->nd_id();
}

/*---------------------------------------------------------------------------*/

inline tsp00_Int4
cbd600_Node::bd600PageConverterVersion () const
{
	return m_NodePtrs.np_ptr()->nd_conv_version();
}

/*---------------------------------------------------------------------------*/

inline tsp00_Int2
cbd600_Node::bd600GetKeyLength(tsp00_Int4 RecIndex) const
{
    return m_RecordList.GetKeyLength(RecIndex);
}

/*---------------------------------------------------------------------------*/

inline void
cbd600_Node::bd600Free ()
{
    ROUTINE_DBG_MEO00 ("bd600Free");

    if ( NULL == m_NodePtrs.np_ptr() ) return;

	const tsp00_PageNo FreePno = bd600GetId();

	if ( FreePno == m_NodePtrs.np_ptr()->nd_root() )
	{
		m_Current.bd300FreeRoot ();

		m_NodePtrs.np_ptr()   = NULL;
		m_NodePtrs.np_cbptr() = NULL;
	}
	else
	{
        const SAPDB_Int4 freePageVersion = bd600PageConverterVersion();
        const SAPDB_Int4 freeLevel       = bd600Level();

		bd600Release (nr_for_update == m_RequestKind);
		bd13FreePageNo (FreePno, freePageVersion, freeLevel, m_Current);
	}
}


/*---------------------------------------------------------------------------*/

inline tbd_nodeptr
cbd600_Node::bd600GetNodePtr ()
{
	return m_NodePtrs.np_ptr();
}

/*---------------------------------------------------------------------------*/

inline tsp00_Int4 &
cbd600_Node::bd600LeafCount()
{
	return m_NodePtrs.np_ptr()->ndLeafCount_bd00();
}

/*---------------------------------------------------------------------------*/

inline tsp00_Int4
cbd600_Node::bd600NumRecords () const
{
	return m_pNode->nd_record_cnt();
}

/*---------------------------------------------------------------------------*/

inline bool
cbd600_Node::bd600SpaceIsAvailable (tsp00_Int4 RequiredLen) const
{
    ROUTINE_DBG_MEO00 ("bd600SpaceIsAvailable");
    
    // In case of an insert RequiredLen contains the recordlength as well
    // as the bytes for the pointerlist entry
    
    return bd102RecAlign(RequiredLen) + m_pNode->nd_bottom() <= MAX_BOTTOM_BD00 - 
        (m_pNode->nd_record_cnt() << DIVISOR_POINTERSIZE_BD00);
}

/*---------------------------------------------------------------------------*/
inline tsp00_Int4
cbd600_Node::bd600SumKeyLen () const
{
	tsp00_Int4 ResultLen = 0;
	for (tsp00_Int4 RecIndex = FIRST_REC_INDEX_BD00;
	     RecIndex < m_pNode->nd_record_cnt();
		 RecIndex ++)
	{
		ResultLen += bd600GetKeyLength (RecIndex);
	}
	return ResultLen;
}


/*---------------------------------------------------------------------------*/
inline void cbd600_Node::bd600DisableCaching(tbd_nodeptr &pCache)
{
    pCache        = m_pCachedPage; 
    m_pCachedPage = 0;
}

/*---------------------------------------------------------------------------*/
inline void cbd600_Node::bd600EnableCaching(tbd_nodeptr pCache)
{
    m_pCachedPage = pCache;
}

/*---------------------------------------------------------------------------*/
inline bool cbd600_Node::bd600IsCached() const
{
    return m_pCachedPage != 0;
}

/*---------------------------------------------------------------------------*/
inline tgg00_BasisError
cbd600_Node::bd600TrError() const
{
	return m_TrError;
}

/*---------------------------------------------------------------------------*/
inline tgg00_BasisError &
cbd600_Node::bd600TrError()
{
	return m_TrError;
}

/*---------------------------------------------------------------------------*/

inline tsp00_Int4   	
cbd600_Node::bd600MaxRecIndex() const
{
  return m_pNode->nd_record_cnt() - 1;
}

/*---------------------------------------------------------------------------*/

inline void 
cbd600_Node::bd600Move (tsp00_PageNo NewId)
{
	ROUTINE_DBG_MEO00 ("bd600Move");

	if ( bd600IsAccessable() )
    {
        if ( NewId == bd600GetId() )
            return;
        
        bd600Release(nr_for_update == m_RequestKind);
    }

    bd600_GetPageFromDataCache (NewId);
}

/*---------------------------------------------------------------------------*/
 
inline void
cbd600_Node::bd600EvalSepKeyLen (tsp00_KeyPtr pKey,
							     tsp00_Int4   KeyLen,
							     tsp00_KeyPtr pNewSepKey,
							     tsp00_Int2   &NewSepKeyLen) const
{
	ROUTINE_DBG_MEO00 ("bd600EvalSepKeyLen");

	tsp00_Int4 ByteOffset;
	tsp00_Int4 MinSepKeyLen;
 
    if (CnsRead_Manager::IsConsistentRead_Configurated() && (pt2InvSubtree_egg00 == m_NodePtrs.np_ptr()->nd_pt2()))
    {
         /* predsep < sep */
        if ( KeyLen <= NewSepKeyLen )
            MinSepKeyLen = KeyLen - sizeof(tgg91_TransNo);
        else
            MinSepKeyLen = NewSepKeyLen - sizeof(tgg91_TransNo);
        for ( ByteOffset=0; (*(pKey+ByteOffset) == *(pNewSepKey+ByteOffset)) && (ByteOffset < MinSepKeyLen); ++ByteOffset)
        {
        }
        if ( ByteOffset < MinSepKeyLen )
        {
            if ( *(pKey+ByteOffset) > *(pNewSepKey+ByteOffset))
            {
                g01opmsg (sp3p_console, sp3m_error,
                    BD600_X1_INVALID_SEPARATOR_SP03, csp3_n_btree,
                    "BD600: predsep.k > sep.k", 0);
                m_TrError = e_invalid_leaves_structure;
                return;
            }
        }
        else
        {
            if (ByteOffset == MinSepKeyLen)
            {
                // key differs in write trans info only
                MinSepKeyLen += sizeof(tgg91_TransNo);
                while ( (*(pKey+ByteOffset) == *(pNewSepKey+ByteOffset) ) && (ByteOffset < MinSepKeyLen))
                {
                    ++ByteOffset;
                }
                if ( MinSepKeyLen == NewSepKeyLen )
                {
                    g01opmsg (sp3p_console, sp3m_error,
                        BD600_X2_INVALID_SEPARATOR_SP03, csp3_n_btree,
                        "BD600: min_len = sep.len", 0);
                    m_TrError = e_invalid_leaves_structure;
                    return;
                }
            }
        }
    }
    else
    {       
        /* predsep < sep */
        if ( KeyLen <= NewSepKeyLen )
            MinSepKeyLen = KeyLen;
        else
            MinSepKeyLen = NewSepKeyLen;
        
        for ( ByteOffset=0;
        (*(pKey+ByteOffset) == *(pNewSepKey+ByteOffset)) &&
            (ByteOffset < MinSepKeyLen);
        ++ByteOffset)
            /*empty*/;
            
            if ( ByteOffset < MinSepKeyLen )
            {
                if ( *(pKey+ByteOffset) > *(pNewSepKey+ByteOffset))
                {
                    g01opmsg (sp3p_console, sp3m_error,
                        BD600_X1_INVALID_SEPARATOR_SP03, csp3_n_btree,
                        "BD600: predsep.k > sep.k", 0);
                    m_TrError = e_invalid_leaves_structure;
                    return;
                }
            }
            else
            {
                if ( MinSepKeyLen == NewSepKeyLen )
                {
                    g01opmsg (sp3p_console, sp3m_error,
                        BD600_X2_INVALID_SEPARATOR_SP03, csp3_n_btree,
                        "BD600: min_len = sep.len", 0);
                    m_TrError = e_invalid_leaves_structure;
                    return;
                }
            }
    }

	/* include the next nonzero byte: */
	if ( ByteOffset < sizeof (tsp00_Key) )
	{
		for ( /*empty*/;
		      (0 == *(pNewSepKey+ByteOffset)) && (ByteOffset < NewSepKeyLen);
			  ++ByteOffset)
			/*empty*/ ;
		NewSepKeyLen = ByteOffset+1;
	}
}
 

/*---------------------------------------------------------------------------*/
 
inline void
cbd600_Node::bd600BuildSeparatorKey (bool          bNodeIsLeft,
								     tsp00_KeyPtr  pNeighborKey,
                                     tsp00_Int4    NeighborKeyLen,
                                     tsp00_KeyPtr &pNewSepKey,
                                     tsp00_Int2   &NewSepKeyLen) const
{
	ROUTINE_DBG_MEO00 ("bd600BuildSeparatorKey1");

    tsp00_KeyPtr pKey;
    tsp00_Int2   keyLength;

	if ( bNodeIsLeft )
	{
        m_RecordList.GetConstKey (bd600MaxRecIndex(), keyLength, pKey);
		pNewSepKey   = pNeighborKey;
		NewSepKeyLen = NeighborKeyLen;
        #ifdef SAPDB_SLOW
        t01moveobj (bd_index, pKey,         POS_OFF_DIFF_BD00, keyLength);
        t01moveobj (bd_index, pNeighborKey, POS_OFF_DIFF_BD00, NeighborKeyLen);
        #endif
		if ( LEAF_LEVEL_BD00 == m_pNode->nd_level() )
			bd600EvalSepKeyLen (pKey, keyLength, pNewSepKey, NewSepKeyLen);
	}
	else
	{
        m_RecordList.GetConstKey (FIRST_REC_INDEX_BD00, keyLength, pKey);
		pNewSepKey   = pKey;
		NewSepKeyLen = keyLength;
        #ifdef SAPDB_SLOW
        t01moveobj (bd_index, pNeighborKey, POS_OFF_DIFF_BD00, NeighborKeyLen);
        t01moveobj (bd_index, pKey,         POS_OFF_DIFF_BD00, keyLength);
        #endif
		if ( LEAF_LEVEL_BD00 == m_pNode->nd_level() )
			bd600EvalSepKeyLen (pNeighborKey, NeighborKeyLen, pNewSepKey, NewSepKeyLen);
	}

    #ifdef SAPDB_SLOW
	t01moveobj (bd_index, pNewSepKey, POS_OFF_DIFF_BD00, NewSepKeyLen);
    #endif

	if ( e_invalid_leaves_structure == m_TrError )
		bd600Dump (csp3_bd_msg, "bd600BuildNewSeparator: ");
}

/*---------------------------------------------------------------------------*/
 
inline void
cbd600_Node::bd600BuildSeparatorKey (cbd600_Node  &RightNeighbor,
								     tsp00_KeyPtr &pSepKey,
								     tsp00_Int2   &SepKeyLen) const
{
	ROUTINE_DBG_MEO00 ("bd600BuildSeparatorKey2");

    tsp00_KeyPtr pKey;
    tsp00_Int2   keyLength;

    RightNeighbor.m_RecordList.GetConstKey (FIRST_REC_INDEX_BD00, SepKeyLen, pSepKey);
    m_RecordList.GetConstKey (bd600MaxRecIndex(), keyLength, pKey);
	
    #ifdef SAPDB_SLOW
    t01moveobj (bd_index, pKey, POS_OFF_DIFF_BD00, keyLength);
	t01moveobj (bd_index, pSepKey, POS_OFF_DIFF_BD00, SepKeyLen);
    #endif

	if ( LEAF_LEVEL_BD00 == m_pNode->nd_level() )
		bd600EvalSepKeyLen (pKey, keyLength, pSepKey, SepKeyLen);

	if ( e_invalid_leaves_structure == m_TrError )
	{
		bd600Dump (csp3_bd_msg, "bd600BuildSeparator: lef");
		RightNeighbor.bd600Dump (csp3_bd_msg, "bd600BuildSeparator: rig");
	}
    #ifdef SAPDB_SLOW
    t01moveobj (bd_index, pSepKey, POS_OFF_DIFF_BD00, SepKeyLen);
    #endif
}

/*---------------------------------------------------------------------------*/

inline void
cbd600_Node::bd600SearchRecord (tsp00_KeyPtr     pKey,
	                           tsp00_Int4        KeyLen,
							   tsp00_Int4       &RecIndex,
							   tbd_searchresult &SearchResult) const
{
    if ( bd600IsSubTreeLeaf()
         &&
         m_Current.bd300ConsistentReadSupport() )
    {
        bd600SearchRecordConsistent (pKey, KeyLen, NULL, RecIndex, SearchResult);
    }
    else
    {
        bd600SearchRecordNonConsistent (pKey, KeyLen, RecIndex, SearchResult);
    }
}
 
/*---------------------------------------------------------------------------*/

inline void
cbd600_Node::bd600SearchStopKey      (tsp00_KeyPtr     pKey,
		                              tsp00_Int4        KeyLen,
								      tsp00_Int4       &RecIndex,
								      tbd_searchresult &SearchResult) const
{
    if ((pt2InvSubtree_egg00 == m_pNode->nd_pt2()) & m_Current.bd300ConsistentReadSupport())
    {
        static SAPDB_Byte MaxTrans[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }; // will be complemented to 0xFFFFFFFFFFFF by search
        this->bd600SearchRecordConsistent (pKey, KeyLen, reinterpret_cast<const tgg91_TransNo*>(&MaxTrans[0]), RecIndex, SearchResult);
    }
    else
    {
        this->bd600SearchRecordNonConsistent (pKey, KeyLen, RecIndex, SearchResult);
    }
}

/*---------------------------------------------------------------------------*/

inline
cbd600_Node::cbd600_Node (const cbd600_Node &Node)
: m_TrError       (Node.m_TrError),
  m_Current       (Node.m_Current),
  m_RequestKind   (Node.m_RequestKind),
  m_PageLockMode  (Node.m_PageLockMode),
  m_RecordList    (Node.m_RecordList),
  m_pCachedPage   (0),
  m_pNode (m_NodePtrs.np_ptr())
{
  if (e_ok != m_TrError) return; // PTS 1104721 TS 1999-11-23

  bd13GetNode (m_Current, Node.bd600GetId(), m_PageLockMode, m_RequestKind, m_NodePtrs);
}

/*---------------------------------------------------------------------------*/

inline
cbd600_Node::cbd600_Node (cbd300_InvCurrentBasis   &Current,
		                  const tbd00_PageLockMode &PageLockMode)
: m_TrError       (Current.curr_trans->trError_gg00),
  m_pNode         (m_NodePtrs.np_ptr()),
  m_Current       (Current),
  m_RequestKind   (Current.bd300RootRequestKind()),
  m_pCachedPage   (0),
  m_PageLockMode  (PageLockMode)
{
    ROUTINE_DBG_MEO00 ("cbd600_Node_Root");

    if (e_ok != m_TrError) return; // PTS 1104721 TS 1999-11-23

    m_NodePtrs.np_ptr()   = NULL;
    m_NodePtrs.np_cbptr() = NULL;
	bd600GetNode (m_Current.curr_tree_id.fileRoot_gg00());
}

/*---------------------------------------------------------------------------*/

inline
cbd600_Node::cbd600_Node (cbd300_InvCurrentBasis   &Current,
						  const tbd_node_request   &RequestKind,
						  const tbd00_PageLockMode &PageLockMode,
                          const tbd_nodeptr        pCache)
: m_TrError       (Current.curr_trans->trError_gg00),
  m_pNode         (m_NodePtrs.np_ptr()),
  m_Current       (Current),
  m_RequestKind   (RequestKind),
  m_pCachedPage   (pCache),
  m_PageLockMode  (PageLockMode)
{
    ROUTINE_DBG_MEO00 ("cbd600_Node");

    m_NodePtrs.np_ptr()   = NULL;
    m_NodePtrs.np_cbptr() = NULL;
}

/*---------------------------------------------------------------------------*/

inline
cbd600_Node::cbd600_Node (cbd300_InvCurrentBasis   &Current,
						  const tbd_node_request   &RequestKind,
						  tsp00_PageNo              Pno,
						  const tbd00_PageLockMode &PageLockMode)
: m_TrError       (Current.curr_trans->trError_gg00),
  m_pNode         (m_NodePtrs.np_ptr()),
  m_Current       (Current),
  m_RequestKind   (RequestKind),
  m_pCachedPage   (0),
  m_PageLockMode  (PageLockMode)
{
    ROUTINE_DBG_MEO00 ("cbd600_Node_Pno");

   if (e_ok != m_TrError) return; // PTS 1104721 TS 1999-11-23

    m_NodePtrs.np_ptr()   = NULL;
    m_NodePtrs.np_cbptr() = NULL;
	bd600GetNode (Pno);
}

/*---------------------------------------------------------------------------*/

inline
cbd600_Node::cbd600_Node (cbd300_InvCurrentBasis   &Current,
						  const tbd_node_request   &RequestKind,
						  tbd_node_ptrs            &Nptr,
 						  const tbd00_PageLockMode &PageLockMode)
: m_TrError       (Current.curr_trans->trError_gg00),
  m_pNode         (m_NodePtrs.np_ptr()),
  m_Current       (Current),
  m_NodePtrs      (Nptr),
  m_RequestKind   (RequestKind),
  m_PageLockMode  (PageLockMode),
  m_pCachedPage   (0),
  m_RecordList    (m_pNode)
{
    ROUTINE_DBG_MEO00 ("cbd600_Node_PascalInterface");
}


/*---------------------------------------------------------------------------*/

inline
cbd600_Node::cbd600_Node (cbd300_InvCurrentBasis   &Current,
						  tbd_node                 &Buffer,
						  const tbd00_PageLockMode &PageLockMode)
: m_TrError       (Current.curr_trans->trError_gg00),
  m_pNode         (m_NodePtrs.np_ptr()),
  m_Current       (Current),
  m_RequestKind   (tbd_node_request::fromConst(nr_for_update)),
  m_PageLockMode  (PageLockMode),
  m_pCachedPage   (0),
  m_RecordList    (&Buffer)
{
    ROUTINE_DBG_MEO00 ("cbd600_Node_Buffer");

    if (e_ok != m_TrError) return; // PTS 1104721 TS 1999-11-23

    m_NodePtrs.np_ptr()   = &Buffer;
    m_NodePtrs.np_cbptr() = NULL;
}

/*---------------------------------------------------------------------------*/

inline cbd600_Node::Iterator::Iterator(const cbd600_Node & node,
                                       tsp00_Int4          recindex)
: m_Node(node)
, m_RecIndex(recindex)
{}

/*---------------------------------------------------------------------------*/

bool inline cbd600_Node::Iterator::IsValid() const
{
    return m_RecIndex >= FIRST_REC_INDEX_BD00 && m_RecIndex < m_Node.bd600NumRecords();
}

/*---------------------------------------------------------------------------*/

inline cbd600_Node::Iterator& cbd600_Node::Iterator::operator ++ ()
{
    ++m_RecIndex;
    return *this;
}

/*---------------------------------------------------------------------------*/

inline const tgg00_RecPtr cbd600_Node::Iterator::operator * () const
{
    return m_Node.bd600RecPtr(m_RecIndex);
}

/*---------------------------------------------------------------------------*/

inline cbd600_Node::Iterator cbd600_Node::bd600GetFirstRecord() const
{
    return Iterator(*this,FIRST_REC_INDEX_BD00);
}

/*---------------------------------------------------------------------------*/

inline tsp00_Int4 cbd600_Node::Iterator::Index() const
{
	return m_RecIndex;
}

#endif  /* GBD600_H */
