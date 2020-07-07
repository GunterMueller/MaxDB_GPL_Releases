/*
  @file         vbd495.cpp
  @author       TorstenS
  @ingroup      create index
  @brief        This modul implements temporary result files which
                which come into being during the parallel index
                creation to hold the temporary sorted index keys.
                There are two kinds of results double queue files and 
                temporary index tree files.

\if EMIT_LICENCE


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

\endif
*/


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

// Content of include files
#include "gbd495.h"

#include "hbd01.h"    // PASCAL: filesysteminterface_1
#include "hbd400.h"   // CPP   : Inverted Lists Add/Delete

#include "SAPDB/KernelCommon/Kernel_VTrace.hpp"
#include "SAPDB/SAPDBCommon/SAPDB_RangeCode.hpp" // Kernel_move_and_fill
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

#define ALIGN_BD495(LEN) (char *) (((tsp00_Longint ((char*) (LEN))-1) & ~(ALIGNMENT_BD495-1)) + ALIGNMENT_BD495)

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



/*===========================================================================*
 *  GLOBAL FUNCTIONS (CODE)                                                  *
 *===========================================================================*/

tgg00_FileId
bd495InitTempFileId (tsp00_PageNo  Root)
{
    tgg00_FileId    TempFileId;

    TempFileId  = b01niltree_id;
    TempFileId.fileRoot_gg00() = Root;
    TempFileId.fileType_gg00().addElement (ftsTemp_egg00);
    TempFileId.fileType_gg00().addElement (ftsShared_egg00);
    TempFileId.fileTfn_gg00().becomes(tgg00_Tfn::fromConst(tfnTempMulti_egg00));

    return TempFileId;
}

/*===========================================================================*
 *  LOCAL FUNCTIONS (CODE)                                                   *
 *===========================================================================*/




/*===========================================================================*
 *  DEFINITION OF PRIVATE METHODS DECLARED IN gbd495.h (CODE)                * 
 *===========================================================================*/

void
cbd495_DoubleReadQueue::bd495_ReadInvKeyInfo()
{
    //ROUTINE_DBG_MEO00 ("bd495_ReadInvKeyInfo");


    tgg00_BasisError    &TrError = m_SecKeyQueue.bd550GetBasisError();
    tsp00_BytePtr	    pField;
    tsp00_Int2          FieldLen;
    tsp00_Int4          MinRequiredPrimKeySpace;

    // Get number of primary keys which belong to the
    // next secondary key in queue

    m_SecKeyQueue.bd550Front (FieldLen, pField);
    if (0 == FieldLen) return;

    SAPDB_RangeMove( __FILE__, 2,
                     FieldLen, sizeof (m_NumPrimKeys),
                     pField, POS_OFF_DIFF_BD00,
                     &m_NumPrimKeys, POS_OFF_DIFF_BD00,
                     FieldLen, TrError);
    if (e_ok != TrError) return;

    m_SecKeyQueue.bd550Pop();

    // Get min amount of storage used by all primary keýs which
    // belong to the next secondary key in queue

    m_SecKeyQueue.bd550Front (FieldLen, pField);
    if (0 == FieldLen) return;

    SAPDB_RangeMove( __FILE__, 2,
                     FieldLen, sizeof (MinRequiredPrimKeySpace),
                     pField, POS_OFF_DIFF_BD00,
                     &MinRequiredPrimKeySpace, POS_OFF_DIFF_BD00,
                     FieldLen, TrError);
    if (e_ok != TrError) return;

    m_SecKeyQueue.bd550Pop();

    // Get secondary key length to determine if an InvListRef entry
    // must be created.

    m_SecKeyQueue.bd550Front (FieldLen, pField);
    m_bInvListRefNeeded =
        (
            cbd450_InvListArray::bd450NewSize (false, // TODO
                                               FieldLen, MinRequiredPrimKeySpace, m_NumPrimKeys) >
            MAX_INVLISTLENGTH_BD00
        );
    m_bNextSecKey = true; // Next secondary key
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void
cbd495_TempInvTree::bd495_NextPrimKeyIter (
    tgg00_RecPtr    pRec)
{
    switch (pRec->recInvListKind_gg00())
    {
    case ilkArray_egg00 :
        {
            m_PrimKeyIter = new (ALIGN_BD495(m_PrimKeyIterSpace))
                            cbd460_InvListArrayIterator (pRec, NULL, 0, NULL, 0,
                                                         ASCENDING_KEY_ORDER_BD510, INCLUDE_START_KEY_BD510, false /* TODO */);
            m_bInvListRefNeeded = false;
            m_NumPrimKeys       = m_PrimKeyIter->bd460NumPrimKeysInKeyScope();
        }
        break;
    case ilkSubTree_egg00 :
        {
            cbd450_InvListRefSubTree InvListRefSubTree (pRec);

            m_PrimKeyIter =
                new (ALIGN_BD495 (m_PrimKeyIterSpace))
                cbd460_InvListSubtreeIteratorNoBreak (m_Tree, InvListRefSubTree,
                                                      NULL, 0, NULL, 0, ASCENDING_KEY_ORDER_BD510,
                                                      INCLUDE_START_KEY_BD510);
            m_bInvListRefNeeded = true;
            m_NumPrimKeys       = InvListRefSubTree.bd450NumPrimKeys();
        }
        break;
    default:
        {
            m_TrError = e_invalid_invlistpos;
            m_LeafNode.bd600Dump (BD495_INVALID_INVLIST_POS_SP03,
                                  "ConstructPrimKeyIterator");
        }
        return;
    }
    m_bNextSecKey = true; // Next secondary key
}

/*===========================================================================*
 *  DEFINITION OF METHODS DECLARED IN gbd495.h (CODE)                        * 
 *===========================================================================*/

cbd495_InvQueue::~cbd495_InvQueue()
{
    // virtual destructors allways have an entry within the vtable and
    // therefore inline is not used (see Scott Meyers page 85).
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

cbd495_DoubleReadQueue::cbd495_DoubleReadQueue (
    tbd_current_tree	&QueueCurrent,
    tsp00_PageNo		 PrimQueuePno,
    tsp00_PageNo		 SecQueuePno)
        :
        m_bInvListRefNeeded (false),
        m_bNextSecKey       (true),
        m_NumPrimKeys		(0),
        m_PrimKeyQueue		(QueueCurrent, PrimQueuePno),
        m_SecKeyQueue		(QueueCurrent, SecQueuePno)
{
    ROUTINE_DBG_MEO00 ("cbd495_DoubleReadQueue");

    bd495_ReadInvKeyInfo();
}

/*---------------------------------------------------------------------------*/

bool
cbd495_DoubleReadQueue::bd495GetCurrentItem(
    tsp00_BytePtr	&pPrimKey,
    tsp00_BytePtr	&pSecKey,
    tsp00_Int2		&PrimKeyLen,
    tsp00_Int2		&SecKeyLen,
    tsp00_Bool       &bNextSecKey,
    tsp00_Bool       &bLastPrimKey,
    tsp00_Bool       &bInvListRefNeeded)
{
    //ROUTINE_DBG_MEO00 ("bd495GetCurrentItem");


    m_PrimKeyQueue.bd550Front (PrimKeyLen, pPrimKey);
    m_SecKeyQueue.bd550Front (SecKeyLen, pSecKey);

    bNextSecKey       = m_bNextSecKey;
    bLastPrimKey      = (1 == m_NumPrimKeys);
    bInvListRefNeeded = m_bInvListRefNeeded;

    return ((0 != PrimKeyLen) && (0 != SecKeyLen));
}

/*---------------------------------------------------------------------------*/

void
cbd495_DoubleReadQueue::bd495GotoNextItem()
{
    //ROUTINE_DBG_MEO00 ("bd495GotoNextItem");

    --m_NumPrimKeys;

    if (0== m_NumPrimKeys)
    {
        // Delete current secondary key if all primary keys are handled
        m_SecKeyQueue.bd550Pop();

        // Read inv key info for next secondary key
        bd495_ReadInvKeyInfo();
    }
    else
    {
        m_bNextSecKey = false; // The following primary keys belongs
        // to the same secondary key, i.e. the
        // secondary key is equal!
    }
    m_PrimKeyQueue.bd550Pop();
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

cbd495_TempInvTree::cbd495_TempInvTree (
    tgg00_TransContext  &Trans,
    tgg00_FileId        &InvFileId,
    bool                 bDropTree)
        :
        m_TrError		    (Trans.trError_gg00),
        m_bInvListRefNeeded (false),
        m_bNextSecKey       (true),
        m_NumPrimKeys       (0),
        m_InvCurrent        (Trans, InvFileId,
                             tbd_node_request::fromConst (nr_for_update),
                             tgg00_MessType::fromConst (m_create_index_parallel),
                             tgg00_VtraceType::fromConst (b03pcreate),
                             IGNORE_INV_ERROR_BD300),
        m_LeafNode          (m_InvCurrent, tbd_node_request::fromConst(nr_for_update)),
        m_Tree              (m_LeafNode),
        m_SecKeyIter        (m_Tree, NULL, 0, NULL, 0, ASCENDING_KEY_ORDER_BD510,
                             INCLUDE_START_KEY_BD510, m_bDummy),
        m_PrimKeyIter       (NULL)
{
    ROUTINE_DBG_MEO00 ("cbd495_TempInvTree");


    m_InvCurrent.bd300SkipErrorHandling();

    if ((e_ok != m_TrError) || bDropTree) return;

    tgg00_RecPtr   pRec = m_SecKeyIter.bd510GetRecord();

    if (NULL == pRec) return; // In case of no next record

    bd495_NextPrimKeyIter (pRec);
}

/*---------------------------------------------------------------------------*/

bool
cbd495_TempInvTree::bd495GetCurrentItem(
    tsp00_BytePtr	&pPrimKey,
    tsp00_BytePtr	&pSecKey,
    tsp00_Int2		&PrimKeyLen,
    tsp00_Int2		&SecKeyLen,
    tsp00_Bool       &bNextSecKey,
    tsp00_Bool       &bLastPrimKey,
    tsp00_Bool       &bInvListRefNeeded)
{
    //ROUTINE_DBG_MEO00 ("bd495GetCurrentItem");



    bNextSecKey       = m_bNextSecKey;
    bLastPrimKey      = (1 == m_NumPrimKeys);
    bInvListRefNeeded = m_bInvListRefNeeded;

    tgg00_RecPtr    pRec = m_SecKeyIter.bd510GetRecord();
    if (NULL == pRec) return false; // In case of no next record

    SecKeyLen = pRec->recKeyLen_gg00();
    pSecKey   = REINTERPRET_CAST (tsp00_BytePtr, (&pRec->recKey_gg00().keyVal_gg00()));

    bool hasDeletedFlag;
    m_PrimKeyIter->bd460Deref (pPrimKey, PrimKeyLen, hasDeletedFlag); // TODO T.A. think about deleted flag

    return ((NULL != pPrimKey));
}

/*---------------------------------------------------------------------------*/

void
cbd495_TempInvTree::bd495GotoNextItem()
{
    //ROUTINE_DBG_MEO00 ("bd495GotoNextItem");

    --m_NumPrimKeys;

    if (0== m_NumPrimKeys)
    {
        // Goto next InvList, i.e. Change the secondary key as
        // well as the primary key.

#ifdef SAPDB_SLOW 
        tsp00_Int2      AuxLen = 0;
        tsp00_BytePtr   pAuxKey = NULL;
        bool            HasDeletedFlag;

        ++(*m_PrimKeyIter);
        m_PrimKeyIter->bd460Deref (pAuxKey, AuxLen, HasDeletedFlag);

        if( 0 != pAuxKey )
        {
            Kernel_VTrace() << "> +++ ERROR: cbd495_TempInvTree::GotoNext - wrong primary key count";
            SAPDBERR_ASSERT_STATE( 0 == pAuxKey );
        }
#endif

        // Destroy old InvList Iterator and thereby free all resources
        m_PrimKeyIter->~cbd460_InvListIterator();

        // Goto the next InvList
        ++(m_SecKeyIter);

        tgg00_RecPtr    pRec = m_SecKeyIter.bd510GetRecord();

        if (NULL == pRec)
        {
            m_TrError = e_ok; return; // In case of no next record
        }

        //Goto first primray key within new InvList

        bd495_NextPrimKeyIter (pRec);
    }
    else
    {
        // Goto next primary key within current InvList
        ++(*m_PrimKeyIter);

        // The current primary keys belongs to the same
        // secondary key, i.e. the secondary key is equal!

        m_bNextSecKey = false;
    }
}

/*---------------------------------------------------------------------------*/

void
cbd495_TempInvTree::bd495Free(
    bool bInclusiveHeadPno)
{
    ROUTINE_DBG_MEO00 ("bd495Free");


    tgg00_BasisError AuxError = m_TrError;

    m_TrError = e_ok;

    // Call the engaged destructors explicit, because of the placement new

    m_LeafNode.~cbd600_Node();

    bd400DropInvTree (m_InvCurrent);

    m_InvCurrent.~cbd300_InvCurrent();

    m_TrError = AuxError;
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
