/****************************************************************************

  module      : vbd401.cpp

  -------------------------------------------------------------------------

  responsible : UweH

  special area: Inverted Lists Accesses And Statistics
  description : 


  last changed: 1999-09-15  10:17
  see also    : example.html ...

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 1998-2005 SAP AG



    ========== licence begin  GPL
    Copyright (c) 1998-2005 SAP AG

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

#include "gbd600.h"
#include "gbd300.h"
#include "gbd450.h"
#include "gbd460.h"
#include "gbd500.h"
#include "gbd520.h"


#include "hbd52.h"
#include "hbd401.h"
#include "hbd72.h"
#include "hkb53.h"
#include "hkb71.h"
#include "hgg01_1.h"  

#include "SAPDB/SAPDBCommon/SAPDB_RangeCode.hpp" // Kernel_move_and_fill
#include "IOManager/IOMan_ClusterAddress.hpp"
#include "Converter/Converter_PageCollector.hpp"
#include "KernelCommon/Kernel_VTrace.hpp"

#include "gsp03.h"
#include "gsp03_3.h" 
#include "hgg01_1.h" 

#if COMPILEMODE_MEO00 >= SLOW_MEO00 
#include "hta99.h"
#endif

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#ifdef NO_INLINES
# define _INLINE
#else
# define _INLINE    inline
#endif

#define CONCURRENT          true
#define FOR_UPDATE          true
#define ASC_KEY_ORDER       true
#define NODE_IS_NOT_CHANGED false
#define FILLCHAR            '\0'
#define NUM_PATHS            3
#define UNKNOWN_RECORD_CNT  -1

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/

/*===========================================================================*
 *  GLOBAL VARIABLES                                                         *
 *===========================================================================*/

/*===========================================================================*
 *  LOCAL VARIABLES                                                          *
 *===========================================================================*/

/*===========================================================================*
 *  LOCAL FUNCTIONS (PROTOTYPES)                                             *
 *===========================================================================*/

static inline tsp00_Longreal
bd401_AveNumberOfEntries (cbd300_InvCurrent  &InvCurrent,
                              tsp00_Int2          Level,
                              tsp00_Int2          Bottom     [MAX_LEVEL_BD00+1] [NUM_PATHS],
                              tsp00_Int2          NumEntries [MAX_LEVEL_BD00+1] [NUM_PATHS], 
                              tsp00_PageNo        PageNo     [MAX_LEVEL_BD00+1] [NUM_PATHS]);

/*---------------------------------------------------------------------------*/

static void
bd401_CopyKeysFromInvListToBuffer (cbd300_InvCurrent        &InvCurrent,
                                   cbd460_InvListIterator   &KeyIter,
                                   tsp00_KeyPtr              pCurrPrimKey,
                                   tsp00_Int2               &CurrPrimKeyLen,
                                   bool                      bCountOnly,
                                   tgg00_SelectFieldsParam  *pSelectFields,
                                   tgg00_StackDesc          *pStackDescription,
                                   tsp00_Int4               &NumPrimKeys, 
                                   tgg00_LockReqMode         WantedLock,
                                   tgg00_KeylistBuf         &KeyListBuf, 
                                   tsp00_Int4               &ListLen);

/*---------------------------------------------------------------------------*/

static void
bd401_CopyKeyToBuffer (tgg00_TransContext     &Trans,
                       tsp00_KeyPtr            pPrimKey,
                       tsp00_Int4              PrimKeyLen,
                       tgg00_KeylistBuf       &KeyListBuf, 
                       tsp00_Int4             &ListLen,
                       tsp00_Int4             &NumPrimKeys);

/*---------------------------------------------------------------------------*/

static void
bd401_CountKeys (tgg00_BasisError    &TrError,
                 cbd600_Node         &Node,
                 tsp00_KeyPtr         pStartKey,
                 tsp00_Int2           StartKeyLen,
                 tsp00_KeyPtr         pStopKey,
                 tsp00_Int4           StopKeyLen,
                 const tsp00_PageNo   StartLeafPno,
                 const tsp00_PageNo   StopLeafPno,
                 const tsp00_Int4     CalculatedPageCnt, 
                 tsp00_Int4          &PageCnt,
                 tsp00_Int4          &SecKeyCnt,
                 tsp00_Int4          &PrimKeyCnt);

/*---------------------------------------------------------------------------*/

inline tsp00_Longreal
bd401_EvalAveNumberOfEntries(tsp00_Int2 NumEntries, 
                             tsp00_Int2 Bottom , 
                             int        AverageFilling);

/*---------------------------------------------------------------------------*/

static inline void
bd401_FindNextRightSubroot (cbd600_Node       &Node,
                            tsp00_Int4         StopRecIndex,
                            bool               bCountKeys,
                            tsp00_PageNo      &StopLeafPno,
                            tsp00_Int4        &PageCnt,
                            tsp00_PageNo      &NextRightSubRoot);

/*---------------------------------------------------------------------------*/

static inline bool
bd401_KeyIsFullKey (tsp00_KeyPtr      pKey, 
                    const tsp00_Int4  KeyLen);

/*---------------------------------------------------------------------------*/

static inline bool
bd401_KeyIsZeroKey (tsp00_KeyPtr      pKey, 
                    const tsp00_Int4  KeyLen);

/*---------------------------------------------------------------------------*/

static inline void 
bd401_MoveToNextSubRoot (tgg00_BasisError  &TrError,
                         cbd600_Node       &Node,
                         tsp00_Int4         RecIndex,
                         bool               bCountKeys,
                         bool               bForkBetweenLeftAndRightWayFound, 
                         bool              &bStopDescent,
                         tsp00_PageNo      &LeafPno,
                         tsp00_Int4        &PageCnt);

/*---------------------------------------------------------------------------*/

static inline void
bd401_StatisticsUpdateNode (cbd300_InvCurrent &InvCurrent,
                            cbd600_Node        &Node,
                            tsp00_Int4        &NumLeaves,
                            tsp00_Int4        &NumPrimKeys);

/*---------------------------------------------------------------------------*/

inline bool 
bd401_SpecialKeyComp (tsp00_KeyPtr      pKey,
                      const tsp00_Int4  KeyLen,
                      tsp00_BytePtr     SpecialKeyChar);

/*===========================================================================*
 *  DEFINITION OF METHODS DECLARED IN gbd401.h (CODE)                        * 
 *===========================================================================*/

/*===========================================================================*
 *  GLOBAL FUNCTIONS (CODE)                                                  *
 *===========================================================================*/

/* Diese Funktion findet momentan keine Verwendung. Sie kann aber */
/* nach Umstellung der Primärbäume benutzt werden.      Alexander */
 
externCpp void 
b401EvaluatePageCount (cbd300_InvCurrent   &InvCurrent,
                       tsp00_KeyPtr         pStartKey,
                       tsp00_Int4           StartKeyLen,
                       tsp00_KeyPtr         pStopKey,
                       tsp00_Int4           StopKeyLen,
                       bool                 bCountKeys,
                       tsp00_Int4          &PageCnt,
                       tsp00_Int4          &AllPageCnt,
                       tsp00_Int4          &PrimKeyCnt)
{
    ROUTINE_DBG_MEO00 ("b401EvaluatePageCount");

    /*
    The number of leaf nodes in the current B* tree from *pStartKey to
    *pStopKey is estimated. If the tree consists of one leaf only, 
    PageCnt == 1 and AllPageCnt == 1 is returned. If the tree consists 
    of 3 or more index level 1 nodes. The counting starts with *pStartKey and 
    finishes with *pStopKey. AllPageCnt contains the approximate  number 
    of leaf nodes in this tree.
    */
    
    tgg00_BasisError  &TrError          = InvCurrent.curr_trans->trError_gg00;
    const tsp00_PageNo Root             = InvCurrent.curr_tree_id.fileRoot_gg00();
    cbd600_Node        Node (InvCurrent, tbd_node_request::fromConst(nr_for_read), Root);
    if ( e_ok != TrError ) return; 
    const int          RootLevel        = Node.bd600GetNodePtr()->nd_level();
    tsp00_Int4         SecKeyCnt;       

    PrimKeyCnt = UNKNOWN_RECORD_CNT;
    SecKeyCnt  = UNKNOWN_RECORD_CNT;    

    if (LEAF_LEVEL_BD00 == RootLevel) // IndexTree consists of one level only
    {
        PageCnt    = 1;
        AllPageCnt = 1;
        
        if (bCountKeys)
        {
            Node.bd600CountRecords (pStartKey, StartKeyLen, pStopKey, StopKeyLen, SecKeyCnt);
            // ??? hier fehlt noch die Bestimmung der Anzahl der Prim keys
            if (0 == SecKeyCnt) PageCnt = 0;
        }
    }
    else              // IndexTree consists of more than one level 
    {
        /* declare and initialize counters */
        tsp00_Int2     Bottom             [MAX_LEVEL_BD00+1] [NUM_PATHS]; 
        tsp00_Int2     NumLeftEntries     [MAX_LEVEL_BD00+1] [NUM_PATHS];
        tsp00_Int2     NumEntries         [MAX_LEVEL_BD00+1] [NUM_PATHS];
        tsp00_PageNo   PageNo             [MAX_LEVEL_BD00+1] [NUM_PATHS];
        tsp00_Longreal AvgNumEntries      [MAX_LEVEL_BD00+1];
        bool           bRightPageIsRightMostPage [MAX_LEVEL_BD00+1];    // shows if the page on the right way down is the rightmost page in this level
        tsp00_Longreal NumLeftPages   [NUM_PATHS];
        tsp00_PageNo   SubRoot          = NIL_PAGE_NO_GG00;
        tsp00_PageNo   StartLeafPno     = NIL_PAGE_NO_GG00;
        tsp00_PageNo   StopLeafPno      = NIL_PAGE_NO_GG00;
        tsp00_PageNo   NextMiddlSubRoot = NIL_PAGE_NO_GG00;
        tsp00_PageNo   NextRightSubRoot = NIL_PAGE_NO_GG00;
        int            iPath;
        int            CurrLevel;
        int            RecIndex         = NIL_RECINDEX_BD00;
        int            StopRecIndex     = NIL_RECINDEX_BD00;
        int            MiddlRecIndex    = NIL_RECINDEX_BD00;
        tsp00_Longreal NumAllPages      = 0;
        
        /* go down from root to the index page on level one containing the startkey (left path 1)  */
        /* update counters also for the path to the stopkey (right path 3) and the middle path (2) */
        /* down as long as these ways dont differ from the path to the startkey                    */ 
        do
        {  
            cbd500_Tree::bd500FindBranch (Node, pStartKey, StartKeyLen, RecIndex);

            tbd_nodeptr pPage = Node.bd600GetNodePtr(); 
            CurrLevel = pPage->nd_level();
            PageNo         [CurrLevel] [0] = pPage->nd_id();
            Bottom         [CurrLevel] [0] = pPage->nd_bottom();
            NumEntries     [CurrLevel] [0] = pPage->nd_record_cnt();
            NumLeftEntries [CurrLevel] [0] = RecIndex;
            
            /* find the index node where right, middle and left paths start */ 
            /* to differ, store the first nodes which are different in the  */ 
            /* right and middle path from the left path                     */
            if (NIL_PAGE_NO_GG00 == NextMiddlSubRoot)
            {
                PageNo         [CurrLevel] [1] = PageNo         [CurrLevel] [0];
                Bottom         [CurrLevel] [1] = Bottom         [CurrLevel] [0];  
                NumEntries     [CurrLevel] [1] = NumEntries     [CurrLevel] [0]; 
                
                if (NIL_PAGE_NO_GG00 == NextRightSubRoot)
                {
                    PageNo         [CurrLevel] [2] = PageNo         [CurrLevel] [0];
                    Bottom         [CurrLevel] [2] = Bottom         [CurrLevel] [0];  
                    NumEntries     [CurrLevel] [2] = NumEntries     [CurrLevel] [0]; 
                
                    if (NIL_PAGE_NO_GG00 == pPage->nd_right())
                        bRightPageIsRightMostPage [CurrLevel] = true;
                    else
                        bRightPageIsRightMostPage [CurrLevel] = false;
                    
                    cbd500_Tree::bd500FindBranch (Node, pStopKey, StopKeyLen, StopRecIndex);
                    NumLeftEntries [CurrLevel] [2] = StopRecIndex ;
                    
                    if (StopRecIndex != RecIndex)
                    {
                        if (FIRST_INDEX_LEVEL_BD00 != CurrLevel)
                        {
                            MiddlRecIndex      = (RecIndex + StopRecIndex)/2;
                            NextRightSubRoot = bd52SubtreePno (pPage, StopRecIndex);
                        }
                        else
                            if (bCountKeys) StopLeafPno = bd52SubtreePno (pPage, StopRecIndex); 
                    }               
                }
                else
                    MiddlRecIndex = NumEntries [CurrLevel] [0] - 1; 
                
                if ((NIL_RECINDEX_BD00 != MiddlRecIndex) && 
                    (RecIndex          != MiddlRecIndex) &&
                    (FIRST_INDEX_LEVEL_BD00 != CurrLevel))
                {
                    NextMiddlSubRoot = bd52SubtreePno (pPage, MiddlRecIndex);
                }
            }
            
            /* goto next subroot on the left way down */
            SubRoot = bd52SubtreePno (pPage, RecIndex); 
            if (CurrLevel > FIRST_INDEX_LEVEL_BD00)
                Node.bd600Move (SubRoot);
            else
                if (bCountKeys) 
                    StartLeafPno  = SubRoot;
        }
        while ((CurrLevel > FIRST_INDEX_LEVEL_BD00) && (e_ok == TrError)); 

        /* follow the the middle path if it is not already passed and update counters */
        if (NIL_PAGE_NO_GG00 != NextMiddlSubRoot)
        {
            Node.bd600Move (NextMiddlSubRoot);
            
            do
            {
                tbd_nodeptr pPage = Node.bd600GetNodePtr(); 
                CurrLevel = pPage->nd_level();
                PageNo         [CurrLevel] [1] = pPage->nd_id();
                Bottom         [CurrLevel] [1] = pPage->nd_bottom();
                NumEntries     [CurrLevel] [1] = pPage->nd_record_cnt();
                
                /* goto next subroot on the middle way down */
                if (CurrLevel > FIRST_INDEX_LEVEL_BD00)
                {
                    MiddlRecIndex = NumEntries [CurrLevel] [1] - 1;
                    Node.bd600Move (bd52SubtreePno (pPage, MiddlRecIndex));         
                }
            }
            while ((CurrLevel > FIRST_INDEX_LEVEL_BD00) && (e_ok == TrError)); 
        }
        
        /* go down the right path if it is not already passed and update counters */
        if (NIL_PAGE_NO_GG00 != NextRightSubRoot)
        {
            Node.bd600Move (NextRightSubRoot);
            
            do
            {
                cbd500_Tree::bd500FindBranch (Node, pStopKey, StopKeyLen, StopRecIndex);

                tbd_nodeptr pPage = Node.bd600GetNodePtr();                 
                CurrLevel = pPage->nd_level();
                PageNo         [CurrLevel] [2] = pPage->nd_id();
                Bottom         [CurrLevel] [2] = pPage->nd_bottom();
                NumEntries     [CurrLevel] [2] = pPage->nd_record_cnt();
                NumLeftEntries [CurrLevel] [2] = StopRecIndex;
                
                if (NIL_PAGE_NO_GG00 == pPage->nd_right())
                    bRightPageIsRightMostPage [CurrLevel] = true;
                else
                    bRightPageIsRightMostPage [CurrLevel] = false;
                
                /* goto next subroot on the right way down */
                SubRoot = bd52SubtreePno (pPage, StopRecIndex); 
                if (CurrLevel > FIRST_INDEX_LEVEL_BD00)
                    Node.bd600Move (SubRoot);
                else
                    if (bCountKeys) StopLeafPno = SubRoot;      
            }
            while ((CurrLevel > FIRST_INDEX_LEVEL_BD00) && (e_ok == TrError)); 
        }

        /* estimation of average number of entries per page in each index level (stored in  AvgNumEntries)   */
        /* if the right page is the the right most page of an index level it is not included in the          */
        /* calculation of the average number of entries since due to the append optimization the right most  */
        /* page need not be representative for the other pages                                               */

        /* (a) treat root level */
        AvgNumEntries [RootLevel] = NumEntries [RootLevel] [0];

        /* (b) treat level below root level */
        if (FIRST_INDEX_LEVEL_BD00 < RootLevel)
        {
            /* For the level just below the root there is a special treatment since     */
            /* the assumption that the pages are filled by ~75% is not very reliable    */
            /* if there are less than 6 pages at an index level. For lower index levels */
            /* it is reasonable to assume that they consist of more than 6 leaves       */
            /* and therefore their filling level will be at about 75%                   */
            CurrLevel = RootLevel - 1;
            if (NumEntries [RootLevel] [0] > 6)
            {
                AvgNumEntries [CurrLevel] = bd401_AveNumberOfEntries (InvCurrent, CurrLevel, Bottom, NumEntries, PageNo); 
            }
            else if (PageNo [CurrLevel] [0] != PageNo [CurrLevel] [1]) //all three pages are different 
            {
                if (! bRightPageIsRightMostPage [CurrLevel])
                    AvgNumEntries [CurrLevel] = (NumEntries [CurrLevel] [0] + NumEntries [CurrLevel] [1] + NumEntries [CurrLevel] [2])/3.0;
                else
                    AvgNumEntries [CurrLevel] = (NumEntries [CurrLevel] [0] + NumEntries [CurrLevel] [1])/2.0;
            }
            else if (NumEntries [RootLevel] [0] > 3)
            {
                AvgNumEntries [CurrLevel] = bd401_AveNumberOfEntries (InvCurrent, CurrLevel , Bottom, NumEntries, PageNo);
            }
            else if (PageNo [CurrLevel] [0] != PageNo [CurrLevel] [2])
            {   
                if (! bRightPageIsRightMostPage [CurrLevel])
                    AvgNumEntries [CurrLevel] = (NumEntries [CurrLevel] [0] + NumEntries [CurrLevel] [2])/2.0;
                else
                    AvgNumEntries [CurrLevel] = NumEntries [CurrLevel] [0];
            }
            else
                AvgNumEntries [CurrLevel] = bd401_AveNumberOfEntries (InvCurrent, CurrLevel, Bottom, NumEntries, PageNo);

            /* (c) treatment of remaining index levels <= RootLevel - 2 */
            for (CurrLevel = RootLevel - 2; FIRST_INDEX_LEVEL_BD00 <= CurrLevel; --CurrLevel)
                AvgNumEntries [CurrLevel] = bd401_AveNumberOfEntries (InvCurrent, CurrLevel, Bottom, NumEntries, PageNo);
        }
        
        /* calculation of the number of all pages and of those  pages which are  on the left of             */ 
        /* the left and the right path i.e. the number of pages left from the startkey and from the stopkey */                  
        NumAllPages = AvgNumEntries [RootLevel];
        for (iPath = 0; iPath < NUM_PATHS; iPath +=2) //skip the middle path)
            NumLeftPages [iPath] = NumLeftEntries [RootLevel] [iPath];
        
        for (CurrLevel = RootLevel-1; FIRST_INDEX_LEVEL_BD00 <= CurrLevel; --CurrLevel)
        {
            if (bRightPageIsRightMostPage [CurrLevel])
                NumAllPages = (NumAllPages - 1.0) * AvgNumEntries [CurrLevel] + NumEntries [CurrLevel] [2];
            else
                NumAllPages = NumAllPages * AvgNumEntries [CurrLevel];

            for (iPath = 0; iPath < NUM_PATHS; iPath +=2) //skip the middle path
                NumLeftPages [iPath] = NumLeftPages [iPath] * AvgNumEntries [CurrLevel] + NumLeftEntries [CurrLevel] [iPath]; 
        }

        /* set return values */
        AllPageCnt = tsp00_Int4 (NumAllPages + 0.5);
        PageCnt    = tsp00_Int4 (NumLeftPages [2] - NumLeftPages [0] + 1);

        /* count records */
        if (bCountKeys && NIL_PAGE_NO_GG00 != StartLeafPno) // ++++ Uwe
        {
            /*
            bd401_CountKeys (pStartKey, StartKeyLen, pStopKey, StopKeyLen, StartLeafPno, StopLeafPno, 
                PageCnt, SecKeyCnt, PrimKeyCnt, InvCurrent.curr_trans->trError_gg00, Node); 
            */
        }
    }
}

/*---------------------------------------------------------------------------*/
/* CRS 1103446 AK 22/07/99 */
externCpp void 
bd401CalculatePageCount (cbd300_InvCurrent   &InvCurrent,
                         tsp00_KeyPtr         pStartKey,
                         tsp00_Int4           StartKeyLen,
                         tsp00_KeyPtr         pStopKey,
                         tsp00_Int4           StopKeyLen,
                         const bool           bCountKeys,  
                         tsp00_Int4          &PageCnt,
                         tsp00_Int4          &AllPageCnt,
                         tsp00_Int4          &PrimKeyCnt)
{
    ROUTINE_DBG_MEO00 ("bd401CalculatePageCount");
    
    /*
    The number of leaf nodes in the current tree from *pStartKey to
    *pStopKey is calculated and returned in PageCnt. The tree need 
    not to be a B* tree. However, it is required that every node  
    contains the number of subnodes which are referenced by itself.
    AllPageCnt contains the number of all leaf nodes in this tree.
    PTS 1106934 16/06/2000 AK   (strongly changed)                                
    */
    
    tgg00_BasisError   &TrError           = InvCurrent.curr_trans->trError_gg00;
    const tsp00_PageNo &Root              = InvCurrent.curr_tree_id.fileRoot_gg00();
    cbd600_Node         Node (InvCurrent, tbd_node_request::fromConst(nr_for_read), Root);
    if ( e_ok != TrError ) return; 
    const tsp00_Int2   &RootLevel         = Node.bd600GetNodePtr()->nd_level(); 
    tsp00_PageNo        StartLeafPno      = NIL_PAGE_NO_GG00;
    tsp00_PageNo        StopLeafPno       = NIL_PAGE_NO_GG00;
    tsp00_Int4          StartRecIndex     = NIL_RECINDEX_BD00;
    tsp00_Int4          StopRecIndex      = NIL_RECINDEX_BD00;
    tsp00_PageNo        NextRightSubRoot  = NIL_PAGE_NO_GG00;
    bool                bStopDescent      = false;
    bool                bStopLeafIsNeighborOfStartLeaf   = false;
    bool                bForkBetweenLeftAndRightWayFound = false;
    tbd_searchresult    SearchResult;
    tsp00_Int4          SecKeyCnt;

    /* calculate number of all tree leafes */
    AllPageCnt = Node.bd600LeafCount ();
    
    /* set initial value for page and record count */
    PageCnt    = 0;
    PrimKeyCnt = UNKNOWN_RECORD_CNT;
    SecKeyCnt  = UNKNOWN_RECORD_CNT;    

    /* Go down the way from root to the startkey until the leaf level is reached or an record  */
    /* points to less than two leaves. While going down the page number of the first page is   */
    /* stored which is different in the way down from the root to the startkey and the stopkey */ 
    
    if (!bCountKeys && bd401_KeyIsZeroKey (pStartKey,StartKeyLen))
    {   /* (a) optimization in case the startkey is the smallest (=zero) key */
        
        if (LEAF_LEVEL_BD00 == Node.bd600Level ())
            ++PageCnt;

        Node.bd600SearchRecord (pStopKey, StopKeyLen, StopRecIndex, SearchResult);
        if ( e_ok != TrError ) return;
        
        /* analyse search result */
        switch (SearchResult)
        {
        case nextfound: 
            if (FIRST_REC_INDEX_BD00 == StopRecIndex)
            {// only in case (LEAF_LEVEL_BD00 == Node.bd600Level () == RootLevel)
                PageCnt    = 0;
                PrimKeyCnt = 0; 
                SecKeyCnt  = 0;
                return;
            }
            else
                --StopRecIndex; 
            break;

        case thisfound: break;
        case lastfound: PageCnt = AllPageCnt; return;         
        case nonefound: return;     
        }   
        
        PageCnt += Node.bd600LeafCount (FIRST_REC_INDEX_BD00, StopRecIndex-1);
        bd401_FindNextRightSubroot (Node, StopRecIndex, bCountKeys, StopLeafPno, PageCnt, NextRightSubRoot);
    }
    else
    {   /* (b) default case which is the startkey is not equal to the the zero key */

        const bool bStartKeyIsEqualtoStopKey = ((pStartKey == pStopKey) && (StartKeyLen == StopKeyLen)); // PTS 1106934 16/06/2000 AK 

        do
        {           
            /* find start key in node */
            Node.bd600SearchRecord (pStartKey, StartKeyLen, StartRecIndex, SearchResult);
            
            /* analyse search result */
            switch (SearchResult)
            {
            case nextfound:
                if (LEAF_LEVEL_BD00 != Node.bd600Level ())
                    --StartRecIndex;
                else
                {
                    if (!bStartKeyIsEqualtoStopKey)
                        ++PageCnt;
                    else
                    {
                        PageCnt    = 0;
                        PrimKeyCnt = 0;
                        SecKeyCnt  = 0;
                        return;
                    }
                }       
                break;
                
            case lastfound: 
                if (LEAF_LEVEL_BD00 == Node.bd600Level ())
                {
                    StartLeafPno = Node.bd600GetId();
                    bStopDescent = true;
                }
                break;
                
            case thisfound:
                if (LEAF_LEVEL_BD00 == Node.bd600Level ())
                    ++PageCnt;
                
                if (bForkBetweenLeftAndRightWayFound && !bCountKeys )
                {
                    PageCnt += Node.bd600LeafCount (StartRecIndex, Node.bd600MaxRecIndex());
                    bStopDescent = true;
                }
                break;
                
            case nonefound:
                bStopDescent = true;
                StartLeafPno = Node.bd600GetId();
            }
            
            if ((SearchResult != lastfound)             &&
                (LEAF_LEVEL_BD00 != Node.bd600Level ()) &&
                (bForkBetweenLeftAndRightWayFound))
                    bStopLeafIsNeighborOfStartLeaf = false;
    
            if (!bStopDescent)
            {   
                /* find the index node where right and left path start */ 
                /* to differ, store the first node which is different  */
                /* in the right path from this in the the left path    */
                if (bForkBetweenLeftAndRightWayFound)
                    PageCnt += Node.bd600LeafCount (StartRecIndex+1, Node.bd600MaxRecIndex());
                else
                {
                    if (!bCountKeys && bd401_KeyIsFullKey (pStopKey, StopKeyLen))
                    {
                        bForkBetweenLeftAndRightWayFound = true;
                        PageCnt += Node.bd600LeafCount (StartRecIndex+1, Node.bd600MaxRecIndex());
                    }
                    else
                    {
                        /* find stop key in node */
                        if (bStartKeyIsEqualtoStopKey)
                            StopRecIndex = StartRecIndex;
                        else
                        {
                            Node.bd600SearchRecord (pStopKey, StopKeyLen, StopRecIndex, SearchResult);
                            if ( e_ok != TrError ) return;
                            
                            /* analyse search result */
                            if (nextfound == SearchResult)
                                --StopRecIndex;
                        }
                        
                        /* check if the point where the ways to start and stop key differ is reached */     
                        if (StopRecIndex > StartRecIndex)
                        {
                            bForkBetweenLeftAndRightWayFound = true;
                            
                            if (((StopRecIndex-StartRecIndex) == 1) &&
                                (LEAF_LEVEL_BD00 != Node.bd600Level ()))
                                bStopLeafIsNeighborOfStartLeaf = true;
                            
                            PageCnt += Node.bd600LeafCount (StartRecIndex+1, StopRecIndex-1);
                            bd401_FindNextRightSubroot (Node, StopRecIndex, bCountKeys, StopLeafPno, PageCnt, NextRightSubRoot);
                        }
                        else if (StopRecIndex < StartRecIndex)
                        {
                            PageCnt    = 0;
                            PrimKeyCnt = 0;
                            SecKeyCnt  = 0;
                            return;
                        }
                    }                   
                }
                /* goto next subroot on the left way down */
                bd401_MoveToNextSubRoot (TrError, Node, StartRecIndex, bCountKeys, 
                    bForkBetweenLeftAndRightWayFound, bStopDescent, StartLeafPno, PageCnt);
            }
        }
        while ((!bStopDescent) && (e_ok == TrError)); 
    }
    
    /* go down the right path if it is not already passed and update counters */
    if (NIL_PAGE_NO_GG00 != NextRightSubRoot)
    {
        bStopDescent = false;
        Node.bd600Move (NextRightSubRoot);
        
        do
        {   
            Node.bd600SearchRecord (pStopKey, StopKeyLen, StopRecIndex, SearchResult);
            if ( e_ok != TrError ) return;
            
            if (LEAF_LEVEL_BD00 == Node.bd600Level ()) 
                if ((StopRecIndex != FIRST_REC_INDEX_BD00) || 
                    (thisfound == SearchResult)            ||     // PTS 1107207 AK 18/07/2000
                    (lastfound == SearchResult))                  // PTS 1107261 AK 25/07/2000 
                    ++PageCnt;

            switch (SearchResult)
            {
            case nextfound: 
                if (Node.bd600Level () > LEAF_LEVEL_BD00)
                {
                    /* The separator may have a smaller key then the smallest key in the leaf */
                    /* If the subnode is an index-node the separator key must be equal to the */
                    /* smallest key in the subnode.  30.7.99, Uwe                            */
                    --StopRecIndex;
                    if (StopRecIndex < FIRST_REC_INDEX_BD00)
                        g01abort (csp3_bd_msg, csp3_n_btree, "BD401CalcPa: odd branch ", Node.bd600GetId());
                }
                else if (StopRecIndex == FIRST_REC_INDEX_BD00)
                {
                    bStopDescent = true;
                    if (bCountKeys) 
                        StopLeafPno = Node.bd600GetId(); // the left neighbour page would be better but is unknown 
                }
                else
                    --StopRecIndex;
                
                break;
                
            case thisfound: break;
            case lastfound: break;

            case nonefound:
                bStopDescent = true;
                if (bCountKeys) 
                    StopLeafPno = Node.bd600GetId(); 
                break;
            }

            if ((LEAF_LEVEL_BD00 != Node.bd600Level ()) &&
                (StopRecIndex > FIRST_REC_INDEX_BD00))
                    bStopLeafIsNeighborOfStartLeaf = false;
            
            if (!bStopDescent)
            {
                PageCnt += Node.bd600LeafCount (FIRST_REC_INDEX_BD00, StopRecIndex-1);          
                
                /* goto next subroot on the right way down */
                bd401_MoveToNextSubRoot (TrError, Node, StopRecIndex, bCountKeys, 
                    bForkBetweenLeftAndRightWayFound, bStopDescent, StopLeafPno, PageCnt);
            }
        }
        while ((!bStopDescent) &&  (e_ok == TrError)); 
    }
    
    /* count primary keys and secondary keys CRS 1106431 Alexander 27/04/2000 */
    if (bCountKeys)
    {   
        if (PageCnt == 0)
        {
            SecKeyCnt  = 0; 
            PrimKeyCnt = 0;
        }
        else
        {
            if (!bForkBetweenLeftAndRightWayFound)
                StopLeafPno = StartLeafPno;
            
           /* check if the boolean bStopLeafIsNeighborOfStartLeaf was determined correctly */  
#           if COMPILEMODE_MEO00 >= QUICK_MEO00 
            Node.bd600Move (StartLeafPno);
            if (((Node.bd600RightNeighbor() != StopLeafPno) && bStopLeafIsNeighborOfStartLeaf) ||
                ((Node.bd600RightNeighbor() == StopLeafPno) && !bStopLeafIsNeighborOfStartLeaf))
            {
                g01opmsg (sp3p_knldiag, sp3m_error, csp3_eval, csp3_n_index, 
                    "bd401CalcPC:Wrong Neig. ", StartLeafPno );
                g01abort (csp3_eval, csp3_n_index, 
                    "bd401CalcPC:Wrong Neig. ", StopLeafPno );
            }
#           endif
            
            /* the number of secondary and primary keys is calculated      */
            /* only if it cannot cause any more i/o operation for the      */
            /* quick and fast kernels for the slow it is always calculated */
#           if COMPILEMODE_MEO00 < SLOW_MEO00 
            if ((StopLeafPno != StartLeafPno) && (!bStopLeafIsNeighborOfStartLeaf))
            {
                SecKeyCnt  = UNKNOWN_RECORD_CNT; 
                PrimKeyCnt = UNKNOWN_RECORD_CNT;
            }
            else
#           endif
            {           
                /* due to the lazy propagation of the sub tree leaf count the so far    */ 
                /* calculated page count need not be correct, however while counting    */ 
                /* the records bd401_CountKeys counts also the number of leafes between */
                /* start and stop key and returns the correct value                     */
                
                tsp00_Int4 CorrectPageCnt;

                bd401_CountKeys (InvCurrent.curr_trans->trError_gg00, Node, 
                    pStartKey, StartKeyLen, pStopKey, StopKeyLen, StartLeafPno, StopLeafPno,
                    PageCnt, CorrectPageCnt, SecKeyCnt, PrimKeyCnt); 
                
                PageCnt = CorrectPageCnt;
            }
        }
    }   
}

/*---------------------------------------------------------------------------*/

externCpp void 
bd401GetFromInvTree (cbd300_InvCurrent         &InvCurrent,
                     tsp00_KeyPtr               pSecKey,
                     tsp00_Int4                 SecKeyLen,
                     tsp00_KeyPtr               pStartPrimKey,
                     tsp00_Int2                 StartPrimKeyLen,
                     tsp00_KeyPtr               pStopPrimKey,
                     tsp00_Int4                 StopPrimKeyLen,
                     tsp00_KeyPtr               pCurrPrimKey,
                     tsp00_Int2                &CurrPrimKeyLen,
                     bool                       bIncludeStartKey,
                     bool                       bCountOnly,
                     tgg00_SelectFieldsParam   *pSelectFields,
                     tgg00_StackDesc           *pStackDescription,
                     tgg00_LockReqMode          WantedLock, 
                     tgg00_KeylistBuf          &KeyListBuf,
                     tsp00_Int4                &NumPrimKeys,
                     tsp00_Int4                &ListLen)
{
    ROUTINE_DBG_MEO00 ("bd401GetFromInvTree");
    
    tgg00_BasisError &TrError = InvCurrent.curr_trans->trError_gg00;
    
    /*
    This routine searches the current B* tree for the inversion list
    with the key SecKey.  If it is present, all record keys that are
    higher or (depending on the value of 'inclusive') the same as the record
    key StartPrimKey and less than StopPrimKey are copied from the list to record 
    buffer KeyListBuf. For the structure of KeyListBuf see the decribtion of 
    bd03GetInv. If not all of the keys to be transferred can be accomodated in 
    KeyListBuf, as many as possible are transferred and TrError is set to
    b_buffer_limit. The number of used bytes in KeyListBuf is always displayed in ListLen.
    
      Some possible acknowledgements in TrError are:
      - e_ok
      - e_inv_list_not_found
      - e_invalid_leaves_structure
      - e_buffer_limit
    */

    const bool          bAscendingKeyOrder = true;
    tbd_searchresult    SecKeySearchResult;
    tsp00_Int4          SecKeyIndex;
    cbd600_Node         LeafNode (InvCurrent, tbd_node_request::fromConst(nr_for_read));
    cbd500_Tree Tree (LeafNode);
    if (TrError != e_ok) return;

    /* search secondary key in index */
    Tree.bd500FindNode (pSecKey, SecKeyLen, LEAF_LEVEL_BD00);
    if (TrError != e_ok) return;

    SecKeySearchResult.becomes  (nonefound);
    LeafNode.bd600SearchRecord (pSecKey, SecKeyLen, SecKeyIndex, SecKeySearchResult);

    if (thisfound != SecKeySearchResult)
    {
        TrError = e_inv_list_not_found;
        return;
    }
    else
    {
        const tgg00_RecPtr pRec = LeafNode.bd600RecPtr (SecKeyIndex);
        
        switch (pRec->recInvListKind_gg00())
        {
        case ilkArray_egg00:
            {   /* get primkey iterator on arrays */
                cbd460_InvListArrayIterator  InvListIter (pRec, pStartPrimKey, StartPrimKeyLen,
                    pStopPrimKey, StopPrimKeyLen, bAscendingKeyOrder, bIncludeStartKey, InvCurrent.bd300ConsistentReadSupport());
                
                bd401_CopyKeysFromInvListToBuffer (InvCurrent, InvListIter, pCurrPrimKey, 
                    CurrPrimKeyLen, bCountOnly, pSelectFields, pStackDescription, NumPrimKeys,  
                    WantedLock, KeyListBuf, ListLen);
                
            }
            break;
        case ilkSubTree_egg00:
            {   /* get primkey iterator on subtrees */
                cbd450_InvListRefSubTree      InvListRefSubTree (pRec);

                cbd460_InvListSubtreeIteratorNoBreak InvListIter( Tree, InvListRefSubTree,
                    pStartPrimKey, StartPrimKeyLen, pStopPrimKey, // PTS 1116651 TS 2002-07-16
                    StopPrimKeyLen, bAscendingKeyOrder, bIncludeStartKey );
                
                bd401_CopyKeysFromInvListToBuffer (InvCurrent, InvListIter, pCurrPrimKey, 
                    CurrPrimKeyLen, bCountOnly, pSelectFields, pStackDescription, NumPrimKeys, 
                    WantedLock, KeyListBuf, ListLen);
            }
            break;
            
        default:
            TrError = e_illegal_record;

            LeafNode.bd600Dump (BD401_C1_ILLEGAL_RECORD_SP03, "bd401GetFromInvTree     ");
        }
    }               
}   
        
/*---------------------------------------------------------------------------*/

externCpp void 
bd401InvStatistics (cbd300_InvCurrent &InvCurrent,
                    bool               bWithSelectivity,
                    tgg00_SampleInfo  &SampleInfo)
{
    ROUTINE_DBG_MEO00 ("bd401InvStatistics");

#   define ONLY_STRING_COLUMNS true
#   define WITH_SELECTIVITY    true

    tgg00_BasisError &TrError = InvCurrent.curr_trans->trError_gg00;

    tbd_stat_aux_vars StatAuxVars;

    bd72InitSampleInfo (InvCurrent.curr_tree_id, SampleInfo, StatAuxVars);

	cbd600_Node CurrNode (InvCurrent);
	cbd500_Tree Tree     (CurrNode, true); // PTS 1121770 TS 2003-04-25

    Trans_Context           &trans = Trans_Context::GetContext(*InvCurrent.curr_trans);
    Converter_PageCollector  pageCollection (trans.Allocator(), Data_PageRecoveryMode());
    Data_PageNoList          pageNoList     (trans.Allocator());
    tsp00_Int4               checkLeafNodes = 0;

    if( ! pageCollection.Initialize() || ! pageNoList.Reserve(pageCollection.GetMaxClusterSize()) )
    {
        TrError = e_no_more_memory;
        return;
	}

    Tree.bd500SetToRoot();

    while ( e_ok == TrError
            &&
            CurrNode.bd600IsAccessable() )
    {
        CurrNode.bd600CountClusteredPages (Tree, pageCollection, pageNoList,
                                           checkLeafNodes, SampleInfo.sam_clustercount);

        if ( TrError != e_ok )
            break;

        CurrNode.bd600GetStatistic (SampleInfo, StatAuxVars, ! WITH_SELECTIVITY);
    
        if ( TrError != e_ok )
            break;
        
        Tree.bd500NextNode();
    }

    if (e_ok != TrError)
        return;

    #ifdef SAPDB_SLOW
    Kernel_VTrace() << "Root: "                          << InvCurrent.bd300GetRootId()
                    << ", checkLeafNodes: "              << checkLeafNodes
                    << ", SampleInfo.leafnodes: "        << SampleInfo.leafnodes
                    << ", SampleInfo.sam_clustercount: " << SampleInfo.sam_clustercount
                    << FlushLine;
    // TODO ++++++ this should not become false but it is: SAPDBERR_ASSERT_STATE( checkLeafNodes == SampleInfo.leafnodes );
    #endif

    if (bWithSelectivity)
    {
        /* current node is not accessable and was on rightmost leaf */
        Tree.bd500SetToLeftMostPageOnSameLevel();

        while ((e_ok == TrError) && CurrNode.bd600IsAccessable())
        {
            CurrNode.bd600GetStatistic (SampleInfo, StatAuxVars, bWithSelectivity);
            Tree.bd500NextNode();
        }
    }

    if (e_ok != TrError)
        return;

#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
    t01statistics (bd_inv, "befor result", SampleInfo);
#   endif

    bd72SampleInfoEpilog (SampleInfo, ONLY_STRING_COLUMNS, StatAuxVars);

#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
    t01statistics (bd_inv, "after result", SampleInfo);
#   endif
}

/*---------------------------------------------------------------------------*/

externCpp void
bd401CountClusteredPages(
    cbd300_InvCurrent     &  InvCurrent,
    tsp00_Int4            &  LeafPageCount,
    tsp00_Int4            &  ClusteredBlockCount)
{
    ROUTINE_DBG_MEO00 ("bd401CountClusteredPages");
    
    cbd600_Node node (InvCurrent, tbd_node_request::fromConst(nr_for_read));
    cbd500_Tree tree (node);
    
    if( e_ok != InvCurrent.curr_trans->trError_gg00 ) 
        return;

    tree.bd500CountClusteredPages( LeafPageCount, ClusteredBlockCount );
}

/*---------------------------------------------------------------------------*/

externCpp void
bd401StatisticsUpdate (cbd300_InvCurrent &InvCurrent,
                       tsp00_Int4        &NumLeaves,
                       tsp00_Int4        &AvgPrimKeyCountPerPage,
                       const bool         bOnlyCountLeaves)         /* PTS 1107153 AK 17/07/2000 */
{
    ROUTINE_DBG_MEO00 ("bd401StatisticsUpdate");
    
    tgg00_BasisError &TrError = InvCurrent.curr_trans->trError_gg00;
	
	cbd600_Node CurrNode (InvCurrent); // PTS 1121589 UH 2003-04-11 rePositionKeys removed

	/* PTS 1107153 AK 17/07/2000 */
	if (bOnlyCountLeaves)
	{
		NumLeaves              = CurrNode.bd600LeafCount ();
		AvgPrimKeyCountPerPage = 0;
		return;
	}
	
	NumLeaves              = 0;
	AvgPrimKeyCountPerPage = 0;
	
	if ( CurrNode.bd600IsLeaf() )
	{
	    ++NumLeaves;
		bd401_StatisticsUpdateNode (InvCurrent, CurrNode, NumLeaves, AvgPrimKeyCountPerPage);
	}
	else
	{
		cbd500_Tree Tree (CurrNode, true); // PTS 1121589 UH 2003-04-11 rePositionKeys removed + PTS 1121770 TS 2003-04-25
		
		Tree.bd500SetToFirstLeaf ();
		if ( e_ok != TrError ) return;
		
		while ( CurrNode.bd600IsAccessable() )
		{
	        ++NumLeaves;
			bd401_StatisticsUpdateNode (InvCurrent, CurrNode, NumLeaves, AvgPrimKeyCountPerPage);
			if ( e_ok != TrError ) break;
			Tree.bd500NextNode();
			if ( e_ok != TrError ) break;
		}
	}
	
	if ( e_ok == TrError )
		AvgPrimKeyCountPerPage = (AvgPrimKeyCountPerPage + (NumLeaves >> 1)) / NumLeaves;
}

/*===========================================================================*
 *  LOCAL FUNCTIONS (CODE)                                                   *
 *===========================================================================*/

static inline tsp00_Longreal
bd401_AveNumberOfEntries (cbd300_InvCurrent  &InvCurrent,
                              tsp00_Int2          Level,
                              tsp00_Int2          Bottom     [MAX_LEVEL_BD00+1] [NUM_PATHS],
                              tsp00_Int2          NumEntries [MAX_LEVEL_BD00+1] [NUM_PATHS], 
                              tsp00_PageNo        PageNo     [MAX_LEVEL_BD00+1] [NUM_PATHS])
{
    ROUTINE_DBG_MEO00 ("bd401_AveNumberOfEntries");
    
    int AveFilling = 0;

    if (InvCurrent.curr_tree_id.fileType_gg00().includes(ftsDynamic_egg00))
        AveFilling = 50;
    else
        AveFilling = 75;

    if (PageNo [Level] [0] == PageNo [Level] [2])
    { // all three pages are equal
        return (bd401_EvalAveNumberOfEntries(NumEntries [Level] [0], Bottom [Level] [0], AveFilling));  
    } 
    else if (PageNo [Level] [0] == PageNo [Level] [1])
    {
        return ((bd401_EvalAveNumberOfEntries(NumEntries [Level] [0], Bottom [Level] [0], AveFilling) +
            bd401_EvalAveNumberOfEntries(NumEntries [Level] [2], Bottom [Level] [2], AveFilling))/2.0);     
    }
    else
    { // all pages are different
        return ((bd401_EvalAveNumberOfEntries(NumEntries [Level] [0], Bottom [Level] [0], AveFilling) +
            bd401_EvalAveNumberOfEntries(NumEntries [Level] [1], Bottom [Level] [1], AveFilling) +
            bd401_EvalAveNumberOfEntries(NumEntries [Level] [2], Bottom [Level] [2], AveFilling))/3.0);
    }
}

/*---------------------------------------------------------------------------*/

static void
bd401_CopyKeysFromInvListToBuffer (cbd300_InvCurrent        &InvCurrent,
                                   cbd460_InvListIterator   &KeyIter,
                                   tsp00_KeyPtr              pCurrPrimKey,
                                   tsp00_Int2               &CurrPrimKeyLen,
                                   bool                      bCountOnly,
                                   tgg00_SelectFieldsParam  *pSelectFields,
                                   tgg00_StackDesc          *pStackDescription,
                                   tsp00_Int4               &NumPrimKeys, 
                                   tgg00_LockReqMode         WantedLock,
                                   tgg00_KeylistBuf         &KeyListBuf, 
                                   tsp00_Int4               &ListLen)
{
    ROUTINE_DBG_MEO00 ("bd401_CopyKeysFromInvListToBuffer");
    

    tgg00_BasisError    &TrError = InvCurrent.curr_trans->trError_gg00;

    ListLen      = 0;
    NumPrimKeys  = 0;

    if (bCountOnly)
        NumPrimKeys = KeyIter.bd460NumPrimKeysInKeyScope();
    else
    {

        const bool      bIsMoreQualification = (pSelectFields != NULL && pStackDescription != NULL);
        tsp00_Bool      bUnqualified = false; // is qualified!

        tsp00_Int2      PrimKeyLen;
        tsp00_Int2      SecKeyLen;
        tsp00_KeyPtr    pPrimKey;
        tsp00_KeyPtr    pSecKey;

        if( bIsMoreQualification ){
            KeyIter.bd460GetSecKey( pSecKey, SecKeyLen );
        }

        do
        {
            bool hasDeletedFlag;
            KeyIter.bd460Deref (pPrimKey, PrimKeyLen, hasDeletedFlag); // TODO T.A.

            if ( NULL == pPrimKey ) 
                break;
         
            if( bIsMoreQualification )
            {
                k71inv_col_select (*(InvCurrent.curr_trans), *pSelectFields, *pStackDescription, 
                    pSecKey, SecKeyLen, pPrimKey, PrimKeyLen, bUnqualified);
            }

            if( ! bUnqualified )
            {
                bd401_CopyKeyToBuffer (*(InvCurrent.curr_trans), pPrimKey, PrimKeyLen, 
                    KeyListBuf, ListLen, NumPrimKeys);
                
                if  (lckFree_egg00 != WantedLock)
                {
                    k53bd_keylist_lock (*(InvCurrent.curr_trans), InvCurrent.curr_tree_id, 
                        KeyListBuf, ListLen);
                }
            }

            if (e_ok == TrError)
                ++ KeyIter;
            else
            {
                if( e_buffer_limit == TrError )
                {
                    /* StartKey becomes current PrimKey */
                    TrError        = e_ok;
                    CurrPrimKeyLen = PrimKeyLen;
                    
                    SAPDB_RangeMove( __FILE__, 2,
                        sizeof( tsp00_Key ), sizeof( tsp00_Key ),
                        pPrimKey, POS_OFF_DIFF_BD00, pCurrPrimKey, POS_OFF_DIFF_BD00,  
                        PrimKeyLen, TrError );
                    
                    if( e_ok == TrError ) 
                        TrError = e_buffer_limit;
                }
                break;
            }
        }
        while ((e_ok == TrError) && (NULL != pPrimKey));
    }
}

/*---------------------------------------------------------------------------*/

static void
bd401_CopyKeyToBuffer (tgg00_TransContext     &Trans,
                       tsp00_KeyPtr            pPrimKey,
                       tsp00_Int4              PrimKeyLen,
                       tgg00_KeylistBuf       &KeyListBuf, 
                       tsp00_Int4             &ListLen,
                       tsp00_Int4             &NumPrimKeys)
{
    ROUTINE_DBG_MEO00 ("bd401_CopyKeyToBuffer");

    /* buffer KeyListBuf. If there are more keys to be transferred           */
    /* than the buffer can hold, the buffer is filled to its capacity        */
    /* with (complete) record keys and 'e_buffer_limit' is sent to m_TrError */
    /* check if the next pair of keylength and key suits into the buffer     */

    tgg00_BasisError &TrError = Trans.trError_gg00;

    const tsp00_Int4 RequiredBufferSize = (ListLen + sizeof (tsp00_Int2) + PrimKeyLen);

    if (RequiredBufferSize > sizeof(KeyListBuf))
    {
        TrError = e_buffer_limit;
        return;
    }
        
    /* move length of key into the buffer */
    tgg00_KeyLen AuxLen = (tgg00_KeyLen) PrimKeyLen;
    void * pKeyLen = (void *) ( (char *) (&KeyListBuf) + ListLen);
    SAPDB_MemCopyNoCheck (pKeyLen, &AuxLen, sizeof(AuxLen));
    
    /* move key into the buffer */
    SAPDB_RangeMove ( __FILE__, 1,    
        PERM_KEY_MXSP00, sizeof(KeyListBuf),
        pPrimKey,   POS_OFF_DIFF_BD00,
        KeyListBuf, POS_OFF_DIFF_BD00 + ListLen + sizeof (AuxLen),
        PrimKeyLen, TrError);
    if (e_ok != TrError) return;

    /* update counters */
    ListLen += sizeof(AuxLen) + PrimKeyLen;
    ++NumPrimKeys; 
}

/*---------------------------------------------------------------------------*/
/* CRS 1106431 Alexander 27/04/2000                                          */

static void
bd401_CountKeys (tgg00_BasisError    &TrError,
                 cbd600_Node         &Node,
                 tsp00_KeyPtr         pStartKey,
                 tsp00_Int2           StartKeyLen,
                 tsp00_KeyPtr         pStopKey,
                 tsp00_Int4           StopKeyLen,
                 const tsp00_PageNo   StartLeafPno,
                 const tsp00_PageNo   StopLeafPno,
                 const tsp00_Int4     CalculatedPageCnt, 
                 tsp00_Int4          &PageCnt,
                 tsp00_Int4          &SecKeyCnt,
                 tsp00_Int4          &PrimKeyCnt)
{
    ROUTINE_DBG_MEO00 ("bd401_CountKeys");
    
    /* the number SecKeyCnt of Seckeys S with  StartKey <= S <= StopKey is             */
    /* determined. Moreover the number of the corresponding primary keys is calculated */                                                 
    /* PTS 1106934 16/06/2000 AK   (completely changed)                                */
    // removed leaf count check
    
    tsp00_Int4        StartRecIndex;
    tsp00_Int4        StopRecIndex;
    tbd_searchresult  SearchResult;
    const bool        bStartKeyIsEqualtoStopKey       = ((pStartKey == pStopKey) && (StartKeyLen == StopKeyLen));

    /* initialize output varaiables */
    PageCnt    = 0;
    SecKeyCnt  = 0;
    PrimKeyCnt = 0;
    
    /* position node on start leaf (which contains the start key) */
    if (!Node.bd600IsAccessable() || (StartLeafPno != Node.bd600GetId()))
        Node.bd600Move (StartLeafPno);
    if (e_ok != TrError) return; 
    
    Node.bd600SearchRecord (pStartKey, StartKeyLen, StartRecIndex, SearchResult);
    
    if (nonefound != SearchResult)
    {
        if (bStartKeyIsEqualtoStopKey)
            StopRecIndex = StartRecIndex;

        if (lastfound == SearchResult)
            ++StartRecIndex;
        
        while (StopLeafPno != Node.bd600GetId())
        {
            StopRecIndex   = Node.bd600NumRecords() - 1;
            SecKeyCnt     += (StopRecIndex - StartRecIndex + 1);
            PrimKeyCnt    += Node.bd600PrimKeyCount (StartRecIndex, StopRecIndex); 
            
            if (StopRecIndex >= StartRecIndex)                                          // PTS 1106934 16/06/2000 AK 
                PageCnt       += (Node.bd600LeafCount(StartRecIndex,StopRecIndex) + 1); 
            
            /* goto next leaf and set Startrecindex to first record */
            Node.bd600Move(Node.bd600RightNeighbor());
            if (e_ok != TrError) return;

            StartRecIndex  = FIRST_REC_INDEX_BD00;
        }
        
        if (!bStartKeyIsEqualtoStopKey)
            Node.bd600SearchRecord (pStopKey, StopKeyLen, StopRecIndex, SearchResult);
        if (nextfound == SearchResult)
            --StopRecIndex;
        
        SecKeyCnt  += (StopRecIndex - StartRecIndex + 1);
        PrimKeyCnt += Node.bd600PrimKeyCount (StartRecIndex, StopRecIndex); 
        if (StopRecIndex >= StartRecIndex)                                     // PTS 1106934 16/06/2000 AK 
            PageCnt += (Node.bd600LeafCount(StartRecIndex,StopRecIndex) + 1); 
    }
    
#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
    t01p2int4 (bd_index, "StartLeaf : ",StartLeafPno, "StopLeaf :  ",StopLeafPno);
    t01p2int4 (bd_index, "PrimKeyCnt: ",PrimKeyCnt ,  "SecKeyCnt:  ",SecKeyCnt);
    t01int4   (bd_index, "PageCnt   : ",PageCnt); 
#   endif               
}

/*---------------------------------------------------------------------------*/

inline tsp00_Longreal
bd401_EvalAveNumberOfEntries(tsp00_Int2 NumEntries, 
                             tsp00_Int2 Bottom , 
                             int        AverageFilling)
{
    return (FULLCOVERING_BD00 * AverageFilling * NumEntries / 100 /
        (Bottom + NumEntries * POINTERSIZE_BD00));
}

/*---------------------------------------------------------------------------*/

static inline void
bd401_FindNextRightSubroot (cbd600_Node       &Node,
                            tsp00_Int4         StopRecIndex,
                            bool               bCountKeys,
                            tsp00_PageNo      &StopLeafPno,
                            tsp00_Int4        &PageCnt,
                            tsp00_PageNo      &NextRightSubRoot)
{
    ROUTINE_DBG_MEO00 ("bd401_FindNextRightSubroot");
    
    const tsp00_Int2 &CurrLevel = Node.bd600GetNodePtr()->nd_level();
    
    NextRightSubRoot = NIL_PAGE_NO_GG00;
    
    if (LEAF_LEVEL_BD00 == CurrLevel)
    {
        if (ilkSubTree_egg00 == Node.bd600RecPtr (StopRecIndex)->recInvListKind_gg00())
        {
            cbd450_InvListRefSubTree InvListRefSubTree (Node.bd600RecPtr (StopRecIndex));
            PageCnt    += InvListRefSubTree.bd450NumLeaves ();
        }
        
        StopLeafPno = Node.bd600GetId(); 
    }
    else
    {
        cbd520_Separator Separator;
        Separator.Assign(Node.bd600SepPtr(StopRecIndex));
        
        if (FIRST_INDEX_LEVEL_BD00 < CurrLevel)
            NextRightSubRoot = Separator.bd520SubNodeId(); 
        else
        { //CurrLevel == FIRST_INDEX_LEVEL_BD00
            
            if ((Separator.bd520LeafCount() > 1) || bCountKeys )  //PTS 1107207 AK 18/07/2000
                NextRightSubRoot = Separator.bd520SubNodeId();
            else
                ++PageCnt;
            
            StopLeafPno = Separator.bd520SubNodeId(); 
        }
    }
}

/*---------------------------------------------------------------------------*/

static inline bool
bd401_KeyIsFullKey (tsp00_KeyPtr      pKey, 
                    const tsp00_Int4  KeyLen)
{
    tsp00_Byte FullKeyChar[1] = {255};
    return  bd401_SpecialKeyComp (pKey, KeyLen, FullKeyChar); 
};

/*---------------------------------------------------------------------------*/

static inline bool
bd401_KeyIsZeroKey (tsp00_KeyPtr      pKey, 
                    const tsp00_Int4  KeyLen)
{
    tsp00_Byte ZeroKeyChar[1] = {0};
    return  bd401_SpecialKeyComp (pKey, KeyLen, ZeroKeyChar); 
};

/*---------------------------------------------------------------------------*/

static inline void 
bd401_MoveToNextSubRoot (tgg00_BasisError  &TrError,
                         cbd600_Node       &Node,
                         tsp00_Int4         RecIndex,
                         bool               bCountKeys,
                         bool               bForkBetweenLeftAndRightWayFound, 
                         bool              &bStopDescent,
                         tsp00_PageNo      &LeafPno,
                         tsp00_Int4        &PageCnt)
{
    ROUTINE_DBG_MEO00 ("bd401_MoveToNextSubRoot");
    
    /* goto next subroot below the record with the index RecIndex */
    const tsp00_Int2 &CurrLevel = Node.bd600GetNodePtr()->nd_level();
    
    if (LEAF_LEVEL_BD00 == CurrLevel)
    {
        bStopDescent = true;
        if (ilkSubTree_egg00 == Node.bd600RecPtr (RecIndex)->recInvListKind_gg00())
        {
            cbd450_InvListRefSubTree InvListRefSubTree (Node.bd600RecPtr (RecIndex));
            PageCnt    += InvListRefSubTree.bd450NumLeaves ();
        }
        
        LeafPno = Node.bd600GetId();    
    }
    else
    {
        bStopDescent = false;
        cbd520_Separator Separator;
        Separator.Assign(Node.bd600SepPtr (RecIndex));
        
        if (FIRST_INDEX_LEVEL_BD00 < CurrLevel)
            Node.bd600Move (Separator.bd520SubNodeId());
        else 
        {   // CurrLevel == FIRST_INDEX_LEVEL_BD00
            
            if ((Separator.bd520LeafCount() > 1) || !bForkBetweenLeftAndRightWayFound || bCountKeys )
            {
                Node.bd600Move (Separator.bd520SubNodeId());
                if( e_ok != TrError)
                    return;
                LeafPno = Node.bd600GetId(); 
            }
            else
            {   // optimization: it is not necessary to continue the 
                // descent since there are no more leaves below the next page 
                ++PageCnt;
                bStopDescent = true;
            }
        }
    }
}

/*---------------------------------------------------------------------------*/

inline bool 
bd401_SpecialKeyComp (tsp00_KeyPtr      pKey,
                      const tsp00_Int4  KeyLen,
                      tsp00_BytePtr     SpecialKeyChar)
{
    for (int i=0; i<KeyLen; ++i)
        if (0 != memcmp ((char *) pKey + i, SpecialKeyChar, 1)) {return false;};
    return true;
}

/*---------------------------------------------------------------------------*/

static inline void
bd401_StatisticsUpdateNode (cbd300_InvCurrent &InvCurrent,
                            cbd600_Node        &Node,
                            tsp00_Int4        &NumLeaves,
                            tsp00_Int4        &NumPrimKeys)
{
    ROUTINE_DBG_MEO00 ("bd401_StatisticsUpdateNode");
    
    tgg00_BasisError &TrError = InvCurrent.curr_trans->trError_gg00;
    
    tsp00_Int4  MaxRecIndex = Node.bd600MaxRecIndex();
    tsp00_Int4  RecIndex;
    
    RecIndex = FIRST_REC_INDEX_BD00;
    
    while (RecIndex <= MaxRecIndex) //PTS 1103687 AK 1999-0817
    {
        switch (Node.bd600RecPtr (RecIndex)->recInvListKind_gg00())
        {
        case ilkArray_egg00:
            NumPrimKeys += Node.bd600RecPtr (RecIndex)->recNumPrimKeys_gg00();
            break;
            
        case  ilkSubTree_egg00:
            {
                cbd450_InvListRefSubTree InvListRefSubTree (Node.bd600RecPtr (RecIndex));
                
                NumPrimKeys  += InvListRefSubTree.bd450NumPrimKeys ();
                NumLeaves    += InvListRefSubTree.bd450NumLeaves ();
            }
            break;
            
        default:
            TrError = e_illegal_record;
        }
        
        if (e_ok != TrError) return;;
        
        ++RecIndex;
    }
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
