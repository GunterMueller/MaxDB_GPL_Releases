/*!**************************************************************************

  module      : gbd550.h
  special area: PageChainHandling and QueueHandling on the basis of PageChains
  see also    :
  author      : AlexanderK
  responsible : UweH
  last changed: 1999-09-21  17:23
  copyright:    Copyright (c) 1999-2005 SAP AG
  description : This module offers methods to handle page chains




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


#ifndef GBD550_H
#define GBD550_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

// Content of include files
#include "gsp00.h"
#include "ggg00.h"
#include "gbd00.h"
#include "hbd13.h"
#include "SAPDB/SAPDBCommon/SAPDB_RangeCode.hpp" // Kernel_move_and_fill
#include "gsp03.h"
#include "gsp03_3.h"
#include "hgg01_1.h"


#if COMPILEMODE_MEO00 >= SLOW_MEO00
#include "hta99.h"
#endif

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define WITH_COPY_OPERATION_GBD550  true

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  FORWARD DECLARATION                                                      *
 *===========================================================================*/

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/
/*!
    class:       cbd550_FieldQueue
    description:
*/

class cbd550_FieldQueue
{
public:

    cbd550_FieldQueue  (tbd_current_tree & Current);
    ~cbd550_FieldQueue ();

    void bd550Pop  ();  // remove topmost Field from queue
    int  bd550Size ();  // returns number of fields in queue
    void bd550Push(
        const tsp00_Int2  FieldLen,  
        const char * const  pField );   // insert field behind pField into queue

    bool bd550Front(
        tsp00_Int2 &FieldLen,  
        char *     &pField );   // get pointer and length of the top most field in queue

private:
    tbd_current_tree  &m_Current;
    tgg00_BasisError  &m_TrError;
    tbd_node_ptrs      m_FirstNptrs;
    tbd_node_ptrs      m_LastNptrs;
    tsp00_Int4         m_AllocPageCnt;

    void               bd550_NewLastNode (tsp00_Int4 NumberOfAllFieldsInQueue = 0);
};

/*! endclass:       cbd550_FieldQueue */

/*---------------------------------------------------------------------------*/
/*!
class:       cbd550_ReadQueue
description: methodes to handle ready only once queues
*/
class cbd550_ReadQueue
{
public:

    cbd550_ReadQueue  (tbd_current_tree & Current, tsp00_PageNo HeadPno);
    ~cbd550_ReadQueue ();

    void                bd550Free (bool bInclusiveHeadPno = true);
    tgg00_BasisError   &bd550GetBasisError () const;
    tsp00_PageNo        bd550GetPno () const;
    void                bd550Pop ();
    void                bd550Front (tsp00_Int2 &FieldLen, tsp00_BytePtr &pField);

private:

    tbd_current_tree    &m_Current;
    tbd_node_ptrs       m_pFirst;
};

/*! endclass:       cbd550_ReadQueue */
/*---------------------------------------------------------------------------*/
/*!
    class:       cbd550_WriteQueue
    description: methodes to handle write queues
*/
class cbd550_WriteQueue
{
public:

    cbd550_WriteQueue  (tbd_current_tree &Current);
    cbd550_WriteQueue  (tbd_current_tree &Current, tsp00_PageNo HeadPno);

    ~cbd550_WriteQueue ();

    void                bd550Free (bool bInclusiveHeadPno = true);
    tgg00_BasisError   &bd550GetBasisError () const;
    tsp00_PageNo        bd550GetPno () const;

    void bd550Push(
        const tsp00_Int2         FieldLen,
        const tsp00_Byte* const  pField );

    void bd550PushWithoutCopy(
        tsp00_Int2      FieldLen, 
        tsp00_BytePtr&  pField,
        bool&           bNewPageAdded );

private:

    void bd550_Push (
        const bool                  bWithCopyOperation,
        const tsp00_Int2            FieldLen,
        const tsp00_Byte* const     pField,
        bool&                       bNewPageAdded,
        tsp00_Int4*                 pOldFirstFreeOffset = NULL);

private:

    tbd_current_tree&   m_Current;
    tbd_node_ptrs       m_pLast;
};
/*! endclass:       cbd550_WriteQueue */

/*===========================================================================*
 *  DEFINITION OF INLINE METHODS                                             *
 *===========================================================================*/

inline
cbd550_FieldQueue::cbd550_FieldQueue (tbd_current_tree & Current)
        : m_Current       (Current),
        m_TrError       (m_Current.curr_trans->trError_gg00),
        m_AllocPageCnt  (0)
{
    bd550_NewLastNode();
    if (e_ok != m_TrError) return;

    bd13GetNode (m_Current, m_LastNptrs.np_ptr()->nd_id(), plmLock_ebd00, nr_for_read, m_FirstNptrs);   /* PTS 1107099 AK 3/06/2000 */
}

/*---------------------------------------------------------------------------*/

inline
cbd550_FieldQueue::~cbd550_FieldQueue()
{
    const bool isCancelled = e_cancelled == m_TrError; 

    if ( isCancelled )
        m_TrError = e_ok;

    tsp00_PageNo RightNeighbourPageNo;
    b13w_release_node (m_LastNptrs,  m_Current);

    do
    {
        RightNeighbourPageNo = m_FirstNptrs.np_ptr()->nd_right();
        b13free_node (m_FirstNptrs, m_Current);
        -- m_AllocPageCnt;

        if (NIL_PAGE_NO_GG00 != RightNeighbourPageNo)
            bd13GetNode (m_Current, RightNeighbourPageNo, plmLock_ebd00, nr_for_read, m_FirstNptrs);  /* PTS 1107099 AK 3/06/2000 */
        else
            break;
    }
    while (e_ok == m_TrError);  //PTS 1107099 AK 04/07/2000

    /* check that all allocated pages are returned PTS 1107099 AK 04/07/2000 */
    if (0 != m_AllocPageCnt)
    {
#     if COMPILEMODE_MEO00 >= QUICK_MEO00
        g01abort (csp3_bd_msg, csp3_n_obj,
                  "BD550FieldQueue:PageLeak", m_AllocPageCnt);
#     else
        g01opmsg (sp3p_knldiag, sp3m_error, csp3_bd_msg, csp3_n_obj,
        "BD550FieldQueue:PageLeak", m_AllocPageCnt);
#     endif
    }

    if ( isCancelled )
        m_TrError = e_cancelled;
}

/*---------------------------------------------------------------------------*/

inline bool
cbd550_FieldQueue::bd550Front(tsp00_Int2 &FieldLen,  char * &pField)
{
    if ( 0 >= m_FirstNptrs.np_ptr()->ndFieldCount_bd00())
    {
        pField = NULL;
        return false;
    }

    FieldLen = *((tsp00_Uint2 *) ((char * ) m_FirstNptrs.np_ptr() + m_FirstNptrs.np_ptr()->ndFirstOffset_bd00()));
    pField   = (char * ) m_FirstNptrs.np_ptr() + m_FirstNptrs.np_ptr()->ndFirstOffset_bd00() + sizeof (tsp00_Uint2);
    return true;
}

/*---------------------------------------------------------------------------*/

inline void
cbd550_FieldQueue::bd550Pop  ()
{
    const tsp00_Int4 FieldLen = *((tsp00_Uint2 *) ((char * ) m_FirstNptrs.np_ptr() + m_FirstNptrs.np_ptr()->ndFirstOffset_bd00()));
    m_FirstNptrs.np_ptr()->ndFirstOffset_bd00() += ALIGN_2BYTE_EO00 (sizeof (tsp00_Uint2) + FieldLen) ;
    -- m_FirstNptrs.np_ptr()->ndFieldCount_bd00();
    -- m_LastNptrs.np_ptr()->ndAllFieldInQueueCount_bd00();

    if (m_FirstNptrs.np_ptr()->ndFieldCount_bd00() <= 0)
    {
        const tsp00_PageNo  RightNeighbourPageNo = m_FirstNptrs.np_ptr()->nd_right();

        if (NIL_PAGE_NO_GG00 != RightNeighbourPageNo)
        {
            b13free_node (m_FirstNptrs, m_Current);
            -- m_AllocPageCnt;
            bd13GetNode (m_Current, RightNeighbourPageNo, plmLock_ebd00, nr_for_read, m_FirstNptrs);    /* PTS 1107099 AK 3/06/2000 */
        }
    }
}

/*---------------------------------------------------------------------------*/

inline void
cbd550_FieldQueue::bd550Push(
    const tsp00_Int2   FieldLen,
    const char * const pField)
{
    /* insert a new page into the queue if the place is not sufficient */
    if ((m_LastNptrs.np_ptr()->ndFirstFreeOffset_bd00() + sizeof (tsp00_Uint2) + FieldLen) >= BODY_END_BD00)
    {
        tbd_node_ptrs AuxNptrs = m_LastNptrs;
        bd550_NewLastNode (m_LastNptrs.np_ptr()->ndAllFieldInQueueCount_bd00());
        if (e_ok != m_TrError) return;

        AuxNptrs.np_ptr()->nd_right() = m_LastNptrs.np_ptr()->nd_id();
        b13w_release_node (AuxNptrs, m_Current);
        if (e_ok != m_TrError) return;
    }

    /* append new field to queue */
    *((tsp00_Uint2 *) ((char * ) m_LastNptrs.np_ptr() + m_LastNptrs.np_ptr()->ndFirstFreeOffset_bd00())) = FieldLen;

    SAPDB_RangeMove( __FILE__, 1,
                     FieldLen,  sizeof (tbd_node),
                     pField,    POS_OFF_DIFF_BD00,
                     m_LastNptrs.np_ptr(), m_LastNptrs.np_ptr()->ndFirstFreeOffset_bd00() +  sizeof (tsp00_Uint2) + POS_OFF_DIFF_BD00,
                     FieldLen, m_TrError);

    m_LastNptrs.np_ptr()->ndFirstFreeOffset_bd00() += (ALIGN_2BYTE_EO00 (sizeof (tsp00_Uint2) + FieldLen));
    ++ m_LastNptrs.np_ptr()->ndFieldCount_bd00();
    ++ m_LastNptrs.np_ptr()->ndAllFieldInQueueCount_bd00();
}

/*---------------------------------------------------------------------------*/

inline void
cbd550_FieldQueue::bd550_NewLastNode (tsp00_Int4 NumberOfAllFieldsInQueue)
{
    /* get new raw node */
    b13new_node (LEAF_LEVEL_BD00, m_LastNptrs, m_Current);
    if (e_ok != m_TrError) return;

    ++m_AllocPageCnt;

    /* initialize node */
    m_LastNptrs.np_ptr()->ndFieldCount_bd00()           = 0;
    m_LastNptrs.np_ptr()->ndFirstOffset_bd00()          = BODY_BEG_BD00 - POS_OFF_DIFF_BD00;
    m_LastNptrs.np_ptr()->ndFirstFreeOffset_bd00()      = m_LastNptrs.np_ptr()->ndFirstOffset_bd00();
    m_LastNptrs.np_ptr()->ndAllFieldInQueueCount_bd00() = NumberOfAllFieldsInQueue;
}

/*---------------------------------------------------------------------------*/

inline  int
cbd550_FieldQueue::bd550Size ()
{
    return m_LastNptrs.np_ptr()->ndAllFieldInQueueCount_bd00();
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

inline
cbd550_ReadQueue::cbd550_ReadQueue (tbd_current_tree    &Current,
                                    tsp00_PageNo         HeadPno)
        :
        m_Current (Current)
{
    m_pFirst.np_ptr()   = NULL;
    m_pFirst.np_cbptr() = NULL;

    if ((e_ok != m_Current.curr_trans->trError_gg00) || (NIL_PAGE_NO_GG00 == HeadPno))
        return;

    bd13GetNode (m_Current, HeadPno, plmLock_ebd00, nr_for_update, m_pFirst);
}

/*---------------------------------------------------------------------------*/

inline
cbd550_ReadQueue::~cbd550_ReadQueue()
{
    if (NULL != m_pFirst.np_ptr())
        b13w_release_node (m_pFirst, m_Current);
}

/*---------------------------------------------------------------------------*/

inline void
cbd550_ReadQueue::bd550Free(bool bInclusiveHeadPno)
{
    tgg00_BasisError    AuxError = m_Current.curr_trans->trError_gg00;
    tsp00_PageNo        HeadPno = (NULL == m_pFirst.np_ptr()) ? NIL_PAGE_NO_GG00 : m_pFirst.np_ptr()->nd_id();
    tsp00_PageNo        RightNeighbour;

    m_Current.curr_trans->trError_gg00 = e_ok;

    while ((e_ok == m_Current.curr_trans->trError_gg00) && (m_pFirst.np_ptr() != NULL))
    {
        RightNeighbour = m_pFirst.np_ptr()->nd_right();

        if (HeadPno == m_pFirst.np_ptr()->nd_id() && !bInclusiveHeadPno)
        {
            m_pFirst.np_ptr()->ndFieldCount_bd00()      = 0;
            m_pFirst.np_ptr()->nd_right()               = NIL_PAGE_NO_GG00;
            m_pFirst.np_ptr()->ndFirstOffset_bd00()     = BODY_BEG_BD00 - POS_OFF_DIFF_BD00;
            m_pFirst.np_ptr()->ndFirstFreeOffset_bd00() = BODY_BEG_BD00 - POS_OFF_DIFF_BD00;
            b13w_release_node (m_pFirst,  m_Current);
        }
        else
            b13free_node (m_pFirst, m_Current);

        if (NIL_PAGE_NO_GG00 == RightNeighbour) break;

        bd13GetNode (m_Current, RightNeighbour, plmLock_ebd00, nr_for_update, m_pFirst);
    }
    m_Current.curr_trans->trError_gg00 = AuxError;
}

/*---------------------------------------------------------------------------*/

inline tgg00_BasisError &
cbd550_ReadQueue::bd550GetBasisError () const
{
    return m_Current.curr_trans->trError_gg00;
}

/*---------------------------------------------------------------------------*/

inline tsp00_PageNo
cbd550_ReadQueue::bd550GetPno () const
{
    if (NULL == m_pFirst.np_ptr())
        return NIL_PAGE_NO_GG00;
    else
        return m_pFirst.np_ptr()->nd_id();
}
/*---------------------------------------------------------------------------*/

inline void
cbd550_ReadQueue::bd550Pop ()
{
    if (
        (e_ok != m_Current.curr_trans->trError_gg00 ) ||
        (NULL == m_pFirst.np_ptr()                  ) ||
        (0 >= m_pFirst.np_ptr()->ndFieldCount_bd00())
    )
        return;

    tsp00_Int4 FieldLen = *(REINTERPRET_CAST (tsp00_Uint2*,
                            (REINTERPRET_CAST (tsp00_BytePtr, m_pFirst.np_ptr()) +
                             m_pFirst.np_ptr()->ndFirstOffset_bd00())));

    m_pFirst.np_ptr()->ndFirstOffset_bd00() += ALIGN_2BYTE_EO00 (sizeof (tsp00_Uint2) + FieldLen);

    -- m_pFirst.np_ptr()->ndFieldCount_bd00();

    if (0 >= m_pFirst.np_ptr()->ndFieldCount_bd00())
    {
        const tsp00_PageNo  RightNeighbour = m_pFirst.np_ptr()->nd_right();

        b13free_node (m_pFirst, m_Current);

        if (NIL_PAGE_NO_GG00 == RightNeighbour) return;

        bd13GetNode (m_Current, RightNeighbour, plmLock_ebd00, nr_for_update, m_pFirst);
    }
}

/*---------------------------------------------------------------------------*/

inline void
cbd550_ReadQueue::bd550Front (tsp00_Int2    &FieldLen,
                              tsp00_BytePtr &pField)
{
    if (
        (e_ok != m_Current.curr_trans->trError_gg00 ) ||
        (NULL == m_pFirst.np_ptr()                  ) ||
        (0 >= m_pFirst.np_ptr()->ndFieldCount_bd00())
    )
    {
        FieldLen = 0;
        pField   = NULL;
        return;
    }

    FieldLen =
        *(REINTERPRET_CAST (tsp00_Uint2*,
                            (REINTERPRET_CAST (tsp00_BytePtr, m_pFirst.np_ptr()) +
                             m_pFirst.np_ptr()->ndFirstOffset_bd00())));

    pField =
        REINTERPRET_CAST (tsp00_BytePtr, m_pFirst.np_ptr()) +
        m_pFirst.np_ptr()->ndFirstOffset_bd00() +
        sizeof (tsp00_Uint2);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

inline
cbd550_WriteQueue::cbd550_WriteQueue (tbd_current_tree &Current)
        :
        m_Current (Current)
{
    m_pLast.np_ptr()   = NULL;
    m_pLast.np_cbptr() = NULL;

    if (e_ok != m_Current.curr_trans->trError_gg00) return;

    b13new_node (LEAF_LEVEL_BD00, m_pLast, m_Current);
    if (e_ok != m_Current.curr_trans->trError_gg00) return;

    m_pLast.np_ptr()->ndFieldCount_bd00()           = 0;
    m_pLast.np_ptr()->ndFirstOffset_bd00()          = BODY_BEG_BD00 - POS_OFF_DIFF_BD00;
    m_pLast.np_ptr()->ndFirstFreeOffset_bd00()      = BODY_BEG_BD00 - POS_OFF_DIFF_BD00;
    m_pLast.np_ptr()->ndAllFieldInQueueCount_bd00() = 0;
}

/*---------------------------------------------------------------------------*/

inline
cbd550_WriteQueue::cbd550_WriteQueue (tbd_current_tree &Current,
                                      tsp00_PageNo      HeadPno)
        :
        m_Current   (Current)
{
    m_pLast.np_ptr()   = NULL;
    m_pLast.np_cbptr() = NULL;

    if ((e_ok != m_Current.curr_trans->trError_gg00) || (NIL_PAGE_NO_GG00 == HeadPno))
        return;

    bd13GetNode (m_Current, HeadPno, plmLock_ebd00, nr_for_update, m_pLast);
}

/*---------------------------------------------------------------------------*/

inline
cbd550_WriteQueue::~cbd550_WriteQueue()
{
    if (NULL != m_pLast.np_ptr())
        b13w_release_node (m_pLast, m_Current);
}

/*---------------------------------------------------------------------------*/

inline void
cbd550_WriteQueue::bd550Free (
    bool  bInclusiveHeadPno)
{
    tgg00_BasisError    AuxError = m_Current.curr_trans->trError_gg00;
    tsp00_PageNo        HeadPno = (NULL == m_pLast.np_ptr()) ? NIL_PAGE_NO_GG00 : m_pLast.np_ptr()->nd_id();
    tsp00_PageNo        RightNeighbour;

    m_Current.curr_trans->trError_gg00 = e_ok;

    while ((e_ok == m_Current.curr_trans->trError_gg00) && (m_pLast.np_ptr () != NULL))
    {
        RightNeighbour = m_pLast.np_ptr()->nd_right();

        if (HeadPno == m_pLast.np_ptr()->nd_id() && !bInclusiveHeadPno)
        {
            m_pLast.np_ptr()->ndFieldCount_bd00()      = 0;
            m_pLast.np_ptr()->nd_right()               = NIL_PAGE_NO_GG00;
            m_pLast.np_ptr()->ndFirstOffset_bd00()     = BODY_BEG_BD00 - POS_OFF_DIFF_BD00;
            m_pLast.np_ptr()->ndFirstFreeOffset_bd00() = BODY_BEG_BD00 - POS_OFF_DIFF_BD00;
            b13w_release_node (m_pLast,  m_Current);
        }
        else
            b13free_node (m_pLast, m_Current);

        if (NIL_PAGE_NO_GG00 == RightNeighbour) break;

        bd13GetNode (m_Current, RightNeighbour, plmLock_ebd00, nr_for_update, m_pLast);
    }
    m_Current.curr_trans->trError_gg00 = AuxError;
}

/*---------------------------------------------------------------------------*/

inline tgg00_BasisError &
cbd550_WriteQueue::bd550GetBasisError () const
{
    return m_Current.curr_trans->trError_gg00;
}

/*---------------------------------------------------------------------------*/

inline tsp00_PageNo
cbd550_WriteQueue::bd550GetPno () const
{
    if (NULL == m_pLast.np_ptr())
        return NIL_PAGE_NO_GG00;
    else
        return m_pLast.np_ptr()->nd_id();
}

/*---------------------------------------------------------------------------*/

inline void
cbd550_WriteQueue::bd550Push(
    const tsp00_Int2          FieldLen,
    const tsp00_Byte* const  pField)
{
    bool bNewPageAdded;
    bd550_Push (WITH_COPY_OPERATION_GBD550, FieldLen, pField, bNewPageAdded );
}

/*---------------------------------------------------------------------------*/

inline void
cbd550_WriteQueue::bd550PushWithoutCopy(
    tsp00_Int2      FieldLen,
    tsp00_BytePtr&  pField,
    bool&           bNewPageAdded )
{
    tsp00_Int4  OldFirstFreeOffset ;

    bd550_Push (!WITH_COPY_OPERATION_GBD550, FieldLen, pField, bNewPageAdded, &OldFirstFreeOffset);

    pField = REINTERPRET_CAST (tsp00_BytePtr, m_pLast.np_ptr()) +
             OldFirstFreeOffset + sizeof (tsp00_Uint2);
}

/*---------------------------------------------------------------------------*/

inline void
cbd550_WriteQueue::bd550_Push(
    const bool              bWithCopyOperation,
    const tsp00_Int2        FieldLen,
    const tsp00_Byte* const pField,
    bool&                   bNewPageAdded,
    tsp00_Int4*             pOldFirstFreeOffset )
{
    // Insert a new page into the queue if the place is not sufficient
    bNewPageAdded = false;
    if ((m_pLast.np_ptr()->ndFirstFreeOffset_bd00() + sizeof (tsp00_Uint2) + FieldLen)
            >= BODY_END_BD00)
    {
        tbd_node_ptrs AuxNptrs = m_pLast;
        m_pLast.np_ptr()   = NULL;
        m_pLast.np_cbptr() = NULL;

        b13new_node (LEAF_LEVEL_BD00, m_pLast, m_Current);
        if (e_ok != m_Current.curr_trans->trError_gg00)
        {
            b13w_release_node (AuxNptrs, m_Current);
            return;
        }
        bNewPageAdded = true;
        m_pLast.np_ptr()->ndFieldCount_bd00()           = 0;
        m_pLast.np_ptr()->ndFirstOffset_bd00()          = BODY_BEG_BD00 - POS_OFF_DIFF_BD00;
        m_pLast.np_ptr()->ndFirstFreeOffset_bd00()      = m_pLast.np_ptr()->ndFirstOffset_bd00();
        m_pLast.np_ptr()->ndAllFieldInQueueCount_bd00() = 0;

        AuxNptrs.np_ptr()->nd_right() = m_pLast.np_ptr()->nd_id();
        b13w_release_node (AuxNptrs, m_Current);
    }

    *(REINTERPRET_CAST (tsp00_Uint2*,
                        (REINTERPRET_CAST (tsp00_BytePtr, m_pLast.np_ptr()) +
                         m_pLast.np_ptr()->ndFirstFreeOffset_bd00()))) = FieldLen;

    if (bWithCopyOperation)
    {
        SAPDB_RangeMove( __FILE__, 2,
                         FieldLen, sizeof (tbd_node),
                         pField,   POS_OFF_DIFF_BD00,
                         m_pLast.np_ptr(), (m_pLast.np_ptr()->ndFirstFreeOffset_bd00() +
                                            sizeof (tsp00_Uint2) + POS_OFF_DIFF_BD00),
                         FieldLen, m_Current.curr_trans->trError_gg00);
    }
    else
    {
        *pOldFirstFreeOffset = m_pLast.np_ptr()->ndFirstFreeOffset_bd00();
    }

    m_pLast.np_ptr()->ndFirstFreeOffset_bd00() +=
        (ALIGN_2BYTE_EO00 (sizeof (tsp00_Uint2) + FieldLen));

    ++ m_pLast.np_ptr()->ndFieldCount_bd00();
}

#endif  /* GBD550_H */
