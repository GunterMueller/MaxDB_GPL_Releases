/****************************************************************************

  module      : vbd403.cpp

  -------------------------------------------------------------------------

  author      : AlexanderK
  responsible : UweH

  special area:  invlist_primitives
  description :  union and intersection of inverted lists.

  last changed: 1999-09-23  15:22
  see also    : example.html ...

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 1999-2005 SAP AG



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


*****************************************************************************/



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gbd300.h"
#include "gbd460.h"
#include "gbd500.h"
#include "gbd510.h"
#include "gbd550.h"  // queue classes
#include "gbd600.h"
#include "ggg200.h"
#include "gsp03.h"   //   PASCAL: SP_message_constants_and_types
#include "gsp03_3.h" //   PASCAL: SP_message_constants_and_types_for_bd_layer

#include "hbd06.h"   // PASCAL: Error_text_handling
#include "hbd401.h"
#include "hbd403.h"
#include "hgg01_1.h" //   PASCAL: Configuration_Parameter
#include "hsp30.h"
#include <new.h>     // required for placement new

#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDB/SAPDBCommon/SAPDB_RangeCode.hpp" // Kernel_move_and_fill


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

#define MAX_LEVEL_BD403         10
#define MIN_LEVEL_BD403         0

#ifdef BIT64
# define ALIGNMENT_BD403 8
#else
# define ALIGNMENT_BD403 4
#endif


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

#define ALIGN_BD403(LEN) (char *) (((tsp00_Longint ((char*) (LEN))-1) & ~(ALIGNMENT_BD403-1)) + ALIGNMENT_BD403)

/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

/* clases required for merge sort in bd403MergePrimKeysInInvLists*/

struct cbd403_SortElement
{
    tsp00_KeyPtr      soePrimKeyPtr_bd403;
    tsp00_Int2        soePrimKeyLen_bd403;
    tsp00_Int2        soeFiller1_bd403;
#   ifdef BIT64
    tsp00_Int4        soeFiller2_bd403;
#   endif
};

/*---------------------------------------------------------------------------*/

class cbd403_Comparator : public cgg200ComparisonOperator <cbd403_SortElement>
{
public:
    tsp00_LcompResult_Enum gg200Compare (const cbd403_SortElement &SortElement1,
                                         const cbd403_SortElement &SortElement2);
private:
    tsp00_LcompResult m_CompResult;
};

/*---------------------------------------------------------------------------*/

class cbd403_MergedInvList : public cgg200_MergeTarget <cbd403_SortElement>
{
public:
    cbd403_MergedInvList  (cbd300_InvCurrent &DestCurrent);

    tsp00_Int4    bd403NumberOfElements ();
    virtual bool  gg200SetNextElement   (cbd403_SortElement SortElement);

private:
    cbd600_Node       m_DestNode;
    cbd500_Tree       m_DestTree;
    tgg00_RecPtr      m_pDestRec;
    tsp00_Int4        m_RecordLen;
    tsp00_Int4        m_NumInsertedElements;

protected:
    tgg00_BasisError &m_TrError;
};

/*---------------------------------------------------------------------------*/

class cbd403_InvListSet : public cgg200_MergeSource <cbd403_SortElement>
{

public: //methods

    cbd403_InvListSet(
        SAPDBMem_IRawAllocator  &allocator,
        cbd500_Tree             &InvTree,
        tsp00_KeyPtr            &pStartPrimKey,
        tsp00_Int4              &StartPrimKeyLen,
        tsp00_KeyPtr            &pStopPrimKey,
        tsp00_Int4              &StopPrimKeyLen,
        bool                    &bErrorDueToThisObject);

    ~cbd403_InvListSet();

    void         bd403AddInvList            (cbd510_RecordIterator                   &RecordIterator);
    bool         bd403Merge                 (cgg200_MergeTarget <cbd403_SortElement> &MergedInvList);

private: //methods

    unsigned int gg200GetListCount  ();
    bool         gg200GetCurrentListElement    (unsigned int ListNo, cbd403_SortElement &SortElement);
    void         gg200GotoNextListElement      (unsigned int ListNo);
    void         bd403_RemoveLists             (tsp00_Int4 Level);
    void         bd403_CalcNumOfListsToMerge   (tsp00_Int4 Level);
    void         bd403_GetLevelWithMaxNumLists (tsp00_Int4 &LevelOfMaxLists);
    bool         bd403_MergeLists              (cgg200_MergeTarget <cbd403_SortElement> &MergeTarget);

private: //members

    SAPDBMem_IRawAllocator  &m_Allocator;


    tsp00_Uint2              m_NumOfLists [MAX_LEVEL_BD403 + 1];
    tsp00_Uint2              m_NumOfListsToMerge;
    tsp00_Uint2              m_NumOfAllLists;

    const tsp00_Int4         m_BuffSize; // has to be top of the buffer member datas -> allocation
    char* const              m_Buffer;
    char*                    m_BuffPosPtr;

    cbd500_Tree             &m_InvTree;
    tsp00_KeyPtr            &m_pStartPrimKey;
    tsp00_Int4              &m_StartPrimKeyLen;
    tsp00_KeyPtr            &m_pStopPrimKey;
    tsp00_Int4              &m_StopPrimKeyLen;
    const tsp00_Int4         m_MemoryForMergePerList;
    cbd460_InvListIterator **m_ppInvListIterator;
    cbd403_Comparator        m_Comparator;
    tbd_current_tree         m_OverFlowCurrent;
    tgg00_BasisError        &m_TrError;
    bool                    &m_ErrorDueToThisObject;

    /* the following variables are only working variables, i.e. */
    /* there defined here to avoid that there are created       */
    /* everytime the functions bd403AddInvList is called        */
    cbd450_InvListRefSubTree m_InvListRefSubTree;
};

/*---------------------------------------------------------------------------*/

class cbd403_OverFlowInvQueue : public cgg200_MergeTarget <cbd403_SortElement>, public cbd460_InvListIterator
{
public:
    EXPLICIT_MEO00            cbd403_OverFlowInvQueue  (tbd_current_tree & Current);
    bool                      gg200SetNextElement (cbd403_SortElement SortElement);
    void                      bd460SkipCurrentKey() { /* TODO */ };
    void                      bd460Deref (tsp00_KeyPtr &pPrimKey, tsp00_Int2 &PrimKeyLen, bool &HasDeletedFlag);
    cbd460_InvListIterator   &operator ++ ();
    tsp00_Int4                bd460NumPrimKeysInKeyScope();

    void bd460GetSecKey ( // PTS 1116651 TS 2002-07-15
        tsp00_KeyPtr &pSecKey,
        tsp00_Int2   &SecKeyLen) const
    {
        // empty because this method is not needed by the class cbd403_OverFlowInvQueue.
    }

private:
    cbd550_FieldQueue    m_FieldQueue;
};

/*===========================================================================*
 *  LOCAL FUNCTIONS (PROTOTYPES)                                             *
 *===========================================================================*/

static void
bd403_MergeInvLists (cbd300_InvCurrent                       &MergeCurrent,
                     cgg200_MergeTarget <cbd403_SortElement> &MergedInvList,
                     tsp00_KeyPtr                             pStartSecKey,
                     tsp00_Int4                               StartSecKeyLen,
                     tsp00_KeyPtr                             pStopSecKey,
                     tsp00_Int4                               StopSecKeyLen,
                     tsp00_KeyPtr                             pStartPrimKey,
                     tsp00_Int4                               StartPrimKeyLen,
                     tsp00_KeyPtr                             pStopPrimKey,
                     tsp00_Int4                               StopPrimKeyLen,
                     bool                                    &bErrorDueToMergeFile);

/*===========================================================================*
 *  CLASS METHODS (CODE)                                                     *
 *===========================================================================*/

inline
cbd403_OverFlowInvQueue::cbd403_OverFlowInvQueue  (tbd_current_tree & Current)
        :
        m_FieldQueue (Current)
{};

/*---------------------------------------------------------------------------*/

inline bool
cbd403_OverFlowInvQueue::gg200SetNextElement (cbd403_SortElement SortElement)
{
    m_FieldQueue.bd550Push (SortElement.soePrimKeyLen_bd403, (char *) SortElement.soePrimKeyPtr_bd403);
    return true;
}

/*---------------------------------------------------------------------------*/

inline cbd460_InvListIterator &
cbd403_OverFlowInvQueue:: operator ++ ()
{
    m_FieldQueue.bd550Pop();
    return (*this);
}

/*---------------------------------------------------------------------------*/
void
cbd403_OverFlowInvQueue::bd460Deref (tsp00_KeyPtr &pPrimKey, tsp00_Int2 &PrimKeyLen, bool &HasDeletedFlag)
{
    char * pTempKey;
    m_FieldQueue.bd550Front (PrimKeyLen,  pTempKey);
    pPrimKey = (tsp00_KeyPtr) pTempKey;
    HasDeletedFlag = false; // TODO T.A.
}

/*---------------------------------------------------------------------------*/

tsp00_Int4
cbd403_OverFlowInvQueue::bd460NumPrimKeysInKeyScope()
{
    return m_FieldQueue.bd550Size();
}

/*---------------------------------------------------------------------------*/

externCpp tsp00_LcompResult_Enum
cbd403_Comparator::gg200Compare (const cbd403_SortElement &SortElement1,
                                 const cbd403_SortElement &SortElement2)
{
    s30cmp (SortElement1.soePrimKeyPtr_bd403, 1, SortElement1.soePrimKeyLen_bd403,
            SortElement2.soePrimKeyPtr_bd403, 1, SortElement2.soePrimKeyLen_bd403,
            m_CompResult);

    return m_CompResult;
}

/*---------------------------------------------------------------------------*/

inline bool
cbd403_MergedInvList::gg200SetNextElement (cbd403_SortElement SortElement)
{
    m_RecordLen = SortElement.soePrimKeyLen_bd403 + cgg_rec_key_offset;
    m_DestTree.bd530AppendRecordSpace (SortElement.soePrimKeyPtr_bd403, SortElement.soePrimKeyLen_bd403,
                                       m_RecordLen, reinterpret_cast<tsp00_BytePtr&>(m_pDestRec));
    if (e_ok != m_TrError)
        return false;

    m_pDestRec->recLen_gg00()          = m_RecordLen;
    m_pDestRec->recKeyLen_gg00()       = SortElement.soePrimKeyLen_bd403;
    m_pDestRec->recVarcolOffset_gg00() = 0; // PTS 1105509 2000-02-01
    m_pDestRec->recVarcolCnt_gg00()    = 0; // PTS 1105509 2000-02-01

    SAPDB_RangeMove( __FILE__, 1,
                     PERM_KEY_MXSP00, sizeof(m_pDestRec->recBody_gg00()),
                     SortElement.soePrimKeyPtr_bd403 , POS_OFF_DIFF_BD00,
                     &(m_pDestRec->recBody_gg00())   , POS_OFF_DIFF_BD00 ,
                     SortElement.soePrimKeyLen_bd403, m_TrError);

    ++m_NumInsertedElements;

    return true;
}


/*---------------------------------------------------------------------------*/

inline tsp00_Int4
cbd403_MergedInvList::bd403NumberOfElements()
{
    return m_NumInsertedElements;
}

/*---------------------------------------------------------------------------*/

inline
cbd403_MergedInvList::cbd403_MergedInvList  (cbd300_InvCurrent &DestCurrent)
        :
        m_DestNode            (DestCurrent),
        m_DestTree            (m_DestNode),
        m_pDestRec            (NULL),
        m_RecordLen           (0),
        m_NumInsertedElements (0),
        m_TrError             (DestCurrent.curr_trans->trError_gg00)
{}

/*---------------------------------------------------------------------------*/

inline
cbd403_InvListSet::cbd403_InvListSet(
    SAPDBMem_IRawAllocator  &allocator,
    cbd500_Tree             &InvTree,
    tsp00_KeyPtr            &pStartPrimKey,
    tsp00_Int4              &StartPrimKeyLen,
    tsp00_KeyPtr            &pStopPrimKey,
    tsp00_Int4              &StopPrimKeyLen,
    bool                     &bErrorDueToThisObject)
        :
        m_Allocator                (allocator),
        m_NumOfListsToMerge        (0),
        m_NumOfAllLists            (0),
        m_BuffSize                 (sizeof (tsp00_Page) * 5),
        m_Buffer                   ((char*) m_Allocator.Allocate (m_BuffSize)),
        m_BuffPosPtr               (NULL),
        m_InvTree                  (InvTree),
        m_pStartPrimKey            (pStartPrimKey),
        m_StartPrimKeyLen          (StartPrimKeyLen),
        m_pStopPrimKey             (pStopPrimKey),
        m_StopPrimKeyLen           (StopPrimKeyLen),
        m_MemoryForMergePerList    (sizeof(cgg200_MergeNode<cbd403_SortElement>)),
        m_ppInvListIterator        ((cbd460_InvListIterator **) m_Buffer),
        m_OverFlowCurrent          (InvTree.bd500GetTheCurrent()),
        m_TrError                  (InvTree.bd500GetTheCurrent().curr_trans->trError_gg00),
        m_ErrorDueToThisObject     (bErrorDueToThisObject)
{
    /* initialize file id for over flow queues */
    m_OverFlowCurrent.curr_tree_id.fileType_gg00().clear();
    m_OverFlowCurrent.curr_tree_id.fileType_gg00().addElement (ftsTemp_egg00);
    SAPDB_MemCopyNoCheck (&m_OverFlowCurrent.curr_tree_id.fileName_gg00(), cgg_zero_fn,
                          sizeof(m_OverFlowCurrent.curr_tree_id.fileName_gg00()));
    /* PTS 1107099 AK 30/06/2000 */
    m_OverFlowCurrent.curr_tree_id.fileTfn_gg00().becomes(tfnTemp_egg00);
    m_OverFlowCurrent.curr_tree_id.fileTfnTemp_gg00().becomes(ttfnTempInv_egg00);
    m_OverFlowCurrent.curr_tree_id.fileQueueInvRoot_gg00() = m_OverFlowCurrent.curr_tree_id.fileRoot_gg00();
    m_OverFlowCurrent.curr_tree_id.fileRoot_gg00()         = NIL_PAGE_NO_GG00;


    /* initialize table which contains the number of lists of each level */
    for (int iLevel = MIN_LEVEL_BD403; iLevel <= MAX_LEVEL_BD403; ++ iLevel)
        m_NumOfLists [iLevel] = 0;

    m_ErrorDueToThisObject = false;

    if( NULL == m_Buffer)
        m_TrError = e_no_more_memory;
    else
        m_BuffPosPtr = m_Buffer + m_BuffSize;
};

/*---------------------------------------------------------------------------*/

inline bool
cbd403_InvListSet::gg200GetCurrentListElement (unsigned int         ListNo,
        cbd403_SortElement  &SortElement)
{
    bool hasDeletedFlag;
    (*(m_ppInvListIterator + m_NumOfAllLists - ListNo - 1))->bd460Deref (SortElement.soePrimKeyPtr_bd403,
            SortElement.soePrimKeyLen_bd403,
            hasDeletedFlag); // TODO T.A.
    if (e_ok != m_TrError)
    {
        m_ErrorDueToThisObject = true;
        return false;
    }

    /* return with false if there are no more primary keys in this list  */
    if (NULL == SortElement.soePrimKeyPtr_bd403)
        return false;

    return true;
}

/*---------------------------------------------------------------------------*/

inline void
cbd403_InvListSet::gg200GotoNextListElement (unsigned int ListNo)
{
    /* go to next next primary key in this inv list */
    ++(**(m_ppInvListIterator + m_NumOfAllLists - ListNo - 1));

    if (e_ok != m_TrError)
        m_ErrorDueToThisObject = true;
}

/*---------------------------------------------------------------------------*/

inline unsigned int
cbd403_InvListSet::gg200GetListCount()
{
    return m_NumOfListsToMerge;
};

/*---------------------------------------------------------------------------*/

void
cbd403_InvListSet::bd403_RemoveLists(tsp00_Int4 Level)
{
    /* this routine removes all inv lists with levels less or equal to Level */

    int iListNo;
    int iLevel;

    for (iLevel = MIN_LEVEL_BD403; iLevel <= Level; ++ iLevel)
    {
        for (iListNo = 1; iListNo <= m_NumOfLists [iLevel]; ++ iListNo)
            (*(m_ppInvListIterator + m_NumOfAllLists - iListNo ))->~cbd460_InvListIterator();

        m_NumOfAllLists       -= m_NumOfLists [iLevel];
        m_NumOfLists [iLevel]  = 0;
    }

    /* determine last free position in buffer */
    if (0 == m_NumOfAllLists)
        m_BuffPosPtr = m_Buffer + m_BuffSize;
    else
        m_BuffPosPtr = (char *) ((cbd403_OverFlowInvQueue*)(*(m_ppInvListIterator + m_NumOfAllLists - 1)));
}

/*---------------------------------------------------------------------------*/

inline void
cbd403_InvListSet::bd403_CalcNumOfListsToMerge (tsp00_Int4 Level)
{
    m_NumOfListsToMerge = 0;
    for (int iLevel = MIN_LEVEL_BD403; iLevel <= Level; ++iLevel)
        m_NumOfListsToMerge += m_NumOfLists [iLevel];
}

/*---------------------------------------------------------------------------*/

inline void
cbd403_InvListSet::bd403_GetLevelWithMaxNumLists(tsp00_Int4 &LevelOfMaxLists)
{
    LevelOfMaxLists = MIN_LEVEL_BD403 + 1;
    for (int iLevel = MIN_LEVEL_BD403 + 2; iLevel <= MAX_LEVEL_BD403; ++iLevel)
        if (m_NumOfLists [iLevel] > m_NumOfLists [LevelOfMaxLists])
            LevelOfMaxLists = iLevel;
}

/*---------------------------------------------------------------------------*/

inline bool
cbd403_InvListSet::bd403_MergeLists (cgg200_MergeTarget <cbd403_SortElement> &MergeTarget)
{
    void * const      pWorkingSpace      = ALIGN_BD403 (m_ppInvListIterator + m_NumOfAllLists);
    const tsp00_Uint4 SizeOfWorkingSpace = m_MemoryForMergePerList * m_NumOfAllLists;
    if (gg200Merge (*this, MergeTarget, m_Comparator, pWorkingSpace , SizeOfWorkingSpace))
        return true;
    else
    {
        g01opmsg (sp3p_knldiag, sp3m_error, BD403_MERGE_NOT_ENOUGH_MEMORY_SP03,
                  csp3_n_index, "BD403:not enough memory " , SizeOfWorkingSpace);
        g01opmsg (sp3p_knldiag, sp3m_error, BD403_MERGE_NOT_ENOUGH_MEMORY_SP03,
                  csp3_n_index, "BD403:not enough memory " , m_NumOfListsToMerge);
        g01opmsg (sp3p_knldiag, sp3m_error, BD403_MERGE_NOT_ENOUGH_MEMORY_SP03,
                  csp3_n_index, "BD403:not enough memory " , m_MemoryForMergePerList);
        m_TrError = e_buffer_limit;
        return false;
    }
}

/*---------------------------------------------------------------------------*/

void
cbd403_InvListSet::bd403AddInvList  (cbd510_RecordIterator & RecordIterator)

{
    const tsp00_Int4 INV_QUEUE_ITERATOR_SIZE  = sizeof (cbd403_OverFlowInvQueue);
    const tsp00_Int4 MAX_INV_LIST_ITEROR_SIZE = MAX_EO00 (sizeof (cbd460_InvListArrayIteratorSave),
            sizeof (cbd460_InvListSubtreeIteratorNoBreak));

    /* check if there is still sufficient space to accomodate the next inv list           */
    /* iterator, an overflow queue and the working space needed for the merge             */
    /* which is requiered in case the next iterator would not fit anymore into the buffer */
    if ((m_Buffer +
            (m_NumOfAllLists + 2) * (sizeof (*m_ppInvListIterator) + m_MemoryForMergePerList) +
            MAX_INV_LIST_ITEROR_SIZE + INV_QUEUE_ITERATOR_SIZE + ALIGNMENT_BD403) >= m_BuffPosPtr)
    {
        /* not enough space for another inv list iterator */
        tsp00_Int4 MergeLevel;

        /* create an overflow queue in buffer */
        m_BuffPosPtr = ALIGN_BD403 (m_BuffPosPtr - sizeof (cbd403_OverFlowInvQueue) - ALIGNMENT_BD403 + 1);
        cbd403_OverFlowInvQueue * pOverFlowQueue = new (m_BuffPosPtr) cbd403_OverFlowInvQueue(m_OverFlowCurrent);

        /* merge all inv lists with a level less or equal MergeLevel into a new overflow of level MergeLevel + 1 */
        if (m_NumOfLists [0] < m_NumOfAllLists/2)
            bd403_GetLevelWithMaxNumLists (MergeLevel);
        else
            MergeLevel = MIN_LEVEL_BD403;

        bd403_CalcNumOfListsToMerge (MergeLevel);
        bd403_MergeLists (*pOverFlowQueue);
        if (m_TrError != e_ok) return;

        /* remove all merged inv lists */
        bd403_RemoveLists (MergeLevel);

        /* copy the new overflow queue to the end of the buffer      */
        /* and keep the pointer to it at the beginning of the buffer */
        /* note that the OverFlowQueue is an InvListIterator too     */
        m_BuffPosPtr = ALIGN_BD403 (m_BuffPosPtr - sizeof (cbd403_OverFlowInvQueue) - ALIGNMENT_BD403 + 1);
        *(m_ppInvListIterator + m_NumOfAllLists) = new (m_BuffPosPtr) cbd403_OverFlowInvQueue (*pOverFlowQueue);

        ++ m_NumOfAllLists;

        if (MergeLevel < MAX_LEVEL_BD403)
            ++ m_NumOfLists [MergeLevel + 1];
        else
            ++ m_NumOfLists [MergeLevel];
    }

    /* add new iterator to the set on in lists and overflow queues which are to merge */
    tgg00_RecPtr pRec = RecordIterator.bd510GetRecord();

    switch (pRec->recInvListKind_gg00())
    {
    case ilkArray_egg00:
        {   /* get primkey iterator on arrays */
            m_BuffPosPtr = ALIGN_BD403 (m_BuffPosPtr - sizeof (cbd460_InvListArrayIteratorSave) - ALIGNMENT_BD403 + 1);

            *(m_ppInvListIterator + m_NumOfAllLists) = new (m_BuffPosPtr)
                    cbd460_InvListArrayIteratorSave  (RecordIterator, m_pStartPrimKey, m_StartPrimKeyLen,
                                                      m_pStopPrimKey, m_StopPrimKeyLen, ASCENDING_KEY_ORDER_BD510, INCLUDE_START_KEY_BD510);
        }
        break;

    case ilkSubTree_egg00:
        {   /* get primkey iterator on subtrees */
            m_InvListRefSubTree.bd450Init(pRec);

            m_BuffPosPtr = ALIGN_BD403 (m_BuffPosPtr - sizeof (cbd460_InvListSubtreeIteratorNoBreak) - ALIGNMENT_BD403 + 1);

            *(m_ppInvListIterator + m_NumOfAllLists) = new (m_BuffPosPtr)
                    cbd460_InvListSubtreeIteratorNoBreak (m_InvTree, m_InvListRefSubTree,
                                                          m_pStartPrimKey, m_StartPrimKeyLen,  m_pStopPrimKey, m_StopPrimKeyLen,
                                                          ASCENDING_KEY_ORDER_BD510, INCLUDE_START_KEY_BD510);
        }
        break;

    default:
        m_TrError = e_illegal_record;
        break;
    }

    ++ m_NumOfAllLists;
    ++ m_NumOfLists [MIN_LEVEL_BD403];
}

/*---------------------------------------------------------------------------*/

bool inline
cbd403_InvListSet::bd403Merge (cgg200_MergeTarget <cbd403_SortElement> &MergedInvList)
{
    m_NumOfListsToMerge = m_NumOfAllLists;
    return bd403_MergeLists (MergedInvList);
}

/*---------------------------------------------------------------------------*/

inline
cbd403_InvListSet::~cbd403_InvListSet()
{
    if( NULL == m_Buffer )
        return;

    bd403_RemoveLists(MAX_LEVEL_BD403);

    // free memory after destruction of all c++ handler located in this memory
    m_Allocator.Deallocate (m_Buffer );
}

/*===========================================================================*
 *  GLOBAL FUNCTIONS (CODE)                                                  *
 *===========================================================================*/

externCpp void
bd403CountPrimKeysInInvLists (cbd300_InvCurrent &InvCurrent,
                              tsp00_KeyPtr       pStartSecKey,
                              tsp00_Int4         StartSecKeyLen,
                              tsp00_KeyPtr       pStopSecKey,
                              tsp00_Int4         StopSecKeyLen,
                              tsp00_KeyPtr       pStartPrimKey,
                              tsp00_Int4         StartPrimKeyLen,
                              tsp00_KeyPtr       pStopPrimKey,
                              tsp00_Int4         StopPrimKeyLen,
                              tsp00_Int4        &NumPrimKeys)
{
    ROUTINE_DBG_MEO00 ("bd403CountPrimKeysInInvLists");

    /* create inv list iterator */
    tgg00_BasisError     &TrError = InvCurrent.curr_trans->trError_gg00;
    bool                  bDummyStartSecKeyFound;
    cbd600_Node           Node     (InvCurrent);
    cbd500_Tree           InvTree  (Node, true); // PTS 1121589 UH 2003-04-11 rePositionKeys removed + PTS 1121770 TS 2003-04-25
    cbd510_RecordIterator InvListRecIter (InvTree, pStartSecKey, StartSecKeyLen,  pStopSecKey, StopSecKeyLen,
                                          ASCENDING_KEY_ORDER_BD510, INCLUDE_START_KEY_BD510, bDummyStartSecKeyFound);

    /* initialize output variables */
    NumPrimKeys = 0;

    const bool bConsistentReadSupport = InvCurrent.bd300ConsistentReadSupport();

    /*go through all inv lists and count the appropriate  primary keys */
    tgg00_RecPtr pRec = InvListRecIter.bd510GetRecord();
    while ( e_ok == TrError && pRec != 0 )
    {
        switch (pRec->recInvListKind_gg00())
        {
        case ilkArray_egg00:
            {
                cbd460_InvListArrayIterator InvListArrayIterator (pRec,
                        pStartPrimKey, StartPrimKeyLen, pStopPrimKey, StopPrimKeyLen,
                        ASCENDING_KEY_ORDER_BD510, INCLUDE_START_KEY_BD510,
                        bConsistentReadSupport );

                NumPrimKeys += InvListArrayIterator.bd460NumPrimKeysInKeyScope ();
            }
            break;

        case  ilkSubTree_egg00:
            {
                cbd450_InvListRefSubTree InvListRefSubTree (pRec);

                cbd460_InvListSubtreeIterator InvListIter (InvTree, InvListRecIter,
                        InvListRefSubTree, pStartPrimKey, StartPrimKeyLen,
                        pStopPrimKey, StopPrimKeyLen, ASCENDING_KEY_ORDER_BD510,
                        INCLUDE_START_KEY_BD510, bConsistentReadSupport );

                NumPrimKeys += InvListIter.bd460NumPrimKeysInKeyScope ();
            }
            break;

        default:
            TrError = e_illegal_record;
        }
        ++InvListRecIter;
        pRec = InvListRecIter.bd510GetRecord();
    }
}

/*---------------------------------------------------------------------------*/

externCpp void
bd403MergePrimKeysInInvLists (cbd300_InvCurrent  &MergeCurrent,
                              cbd300_InvCurrent  &DestCurrent,
                              tsp00_KeyPtr        pStartSecKey,
                              tsp00_Int4          StartSecKeyLen,
                              tsp00_KeyPtr        pStopSecKey,
                              tsp00_Int4          StopSecKeyLen,
                              tsp00_KeyPtr        pStartPrimKey,
                              tsp00_Int4          StartPrimKeyLen,
                              tsp00_KeyPtr        pStopPrimKey,
                              tsp00_Int4          StopPrimKeyLen,
                              tsp00_Int4         &NumPrimKeys)
{
    ROUTINE_DBG_MEO00 ("bd403MergePrimKeysInInvLists");

    tgg00_BasisError     &TrError = MergeCurrent.curr_trans->trError_gg00;
    bool                  bErrorDueToMergeFile;

    /* create handle of the file into the inv lists have to be merged */
    cbd403_MergedInvList  MergedInvList (DestCurrent);
    if (e_ok != TrError)
        return;

    bd403_MergeInvLists (MergeCurrent, MergedInvList, pStartSecKey, StartSecKeyLen,
                         pStopSecKey, StopSecKeyLen,  pStartPrimKey, StartPrimKeyLen, pStopPrimKey,
                         StopPrimKeyLen, bErrorDueToMergeFile);

    if (e_ok != TrError)
        return;

    /* get number of primary keys in the result file */
    NumPrimKeys = MergedInvList.bd403NumberOfElements();
}

/*===========================================================================*
 *  LOCAL FUNCTIONS (CODE)                                                   *
 *===========================================================================*/

static void
bd403_MergeInvLists (cbd300_InvCurrent                       &InvCurrent,
                     cgg200_MergeTarget <cbd403_SortElement> &MergedInvList,
                     tsp00_KeyPtr                             pStartSecKey,
                     tsp00_Int4                               StartSecKeyLen,
                     tsp00_KeyPtr                             pStopSecKey,
                     tsp00_Int4                               StopSecKeyLen,
                     tsp00_KeyPtr                             pStartPrimKey,
                     tsp00_Int4                               StartPrimKeyLen,
                     tsp00_KeyPtr                             pStopPrimKey,
                     tsp00_Int4                               StopPrimKeyLen,
                     bool                                    &bErrorDueToInvCurrent)
{
    ROUTINE_DBG_MEO00 ("bd403_MergeInvLists");

    /* create inv list iterator */
    tgg00_BasisError     &TrError = InvCurrent.curr_trans->trError_gg00;
    bool                  bDummyStartSecKeyFound;
    cbd600_Node           Node     (InvCurrent, tbd00_PageLockMode::fromConst( plmLockWithoutReschedule_ebd00)); // PTS 1115176 TS 2002-04-09
    cbd500_Tree           InvTree  (Node); // PTS 1110885 AK 11/06/2001
    cbd510_RecordIterator InvListRecIter (InvTree, pStartSecKey, StartSecKeyLen,  pStopSecKey, StopSecKeyLen,
                                          ASCENDING_KEY_ORDER_BD510, INCLUDE_START_KEY_BD510, bDummyStartSecKeyFound);
    if (e_ok != TrError)
        return;

    /* create handle of inv lists which are to merge */

    SAPDBMem_IRawAllocator  &allocator =
        *reinterpret_cast<SAPDBMem_IRawAllocator*>(InvCurrent.curr_trans->trAllocator_gg00);

    cbd403_InvListSet InvListSet (allocator, InvTree, pStartPrimKey, StartPrimKeyLen,
                                  pStopPrimKey, StopPrimKeyLen, bErrorDueToInvCurrent);

    if (e_ok != TrError)
        return;

    /* fill the inv list set with inv list */
    while ( InvListRecIter.bd510GetRecord() != 0 )
    {
        InvListSet.bd403AddInvList (InvListRecIter);
        if (e_ok != TrError)
        {
            bErrorDueToInvCurrent = true;
            return;
        }
        /* goto next inv list */
        ++ InvListRecIter;
        if (e_ok != TrError)
        {
            bErrorDueToInvCurrent = true;
            return;
        }
    }

    /* merge the set of inv lists into the file MergedInvList */
    InvListSet.bd403Merge(MergedInvList);
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
