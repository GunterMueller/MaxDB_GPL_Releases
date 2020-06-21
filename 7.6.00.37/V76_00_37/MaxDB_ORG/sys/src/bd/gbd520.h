/*!
  @file           gbd520.h
  @ingroup        Index
  @author         UweH
  @brief          class cbd520_Separator is defined here

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



#ifndef GBD520_H
#define GBD520_H

#include "gsp00.h"
#include "ggg00.h"
#include "gbd00.h"
#include "SAPDB/SAPDBCommon/SAPDB_RangeCode.hpp"

#if COMPILEMODE_MEO00 >= SLOW_MEO00 
#include "hta99.h" 
#endif

#include "hbd102.h"  // PTS 1103980 JA 1999-09-15
#include "hsp30.h"
/*---------------------------------------------------------------------------*/
/*!
@class          cbd520_Separator
@brief          This class offers methods to handle Separator records.
 */
class cbd520_Separator
{
public:
    /*!
     * @brief This is the placeholder for generic record operations.
     * @param pSpace [in]
     */
    void Assign (tgg00_RecPtr pSpace);
    /*!
     * @brief Returns the separator as a record
     */
    const tgg00_RecPtr bd520GetRecPtr() const;
    /*!
     * @brief sets the leafcount for this separator
     * @param LeafCount [in] 
     */
    void bd520SetLeafCount (tsp00_Int4 LeafCount);
    /*!
     * @brief          The key from the separator is removed.
     * @param          SepLenDelta [out] amount of bytes the record became smaller.
     */
    void bd520ConvertToFirstSepOnLevel (tsp00_Int4 &SepLenDelta);
    /*!
     * @brief          A new SeparatorRecord with the given values is created.
     * @param          Key [in]
     * @param          SubNodeId [in]
     * @param          LeafCount [in]
     * 
     * PRE: m_pRec must point to space which is big enough.
     * 
     */
    void bd520CreateNew (tsp00_KeyPtr pKey,
                         tsp00_Int2   KeyLen,
                         tsp00_PageNo SubNodeId,
                         tsp00_Int4   LeafCount);
    /*!
     * @brief          Check if the separator points to the right node and LeafCount is valid.
     * @param          Current [in/out] to set the TrError and get a page.
     */
    void bd520Check (cbd300_InvCurrentBasis Current);
    /*!
     * @brief          for accessing the Separator's key
     * @returns        a pointer to the key
     */
    tsp00_KeyPtr bd520KeyPtr ();
    /*!
     * @brief          for accessing the Separator's keylength
     * @returns        reference to the Key length.
     */
    tsp00_Int2 &bd520KeyLen ();
    /*!
     * @brief          the current used length of the separator is returned
     * @returns        Separator length
     */
    tsp00_Int4  bd520Length();
    /*!
     * @brief          for accessing the Separator's subnodeid
     * @returns        SubNodeId
     */
    tsp00_PageNo bd520SubNodeId ();
    /*!
     * @brief          for accessing the Separator's LeafCount
     * @returns        LeafCount
     */
    tsp00_Int4 bd520LeafCount ();
    /*!
     * @brief          This calculates the total space the separator needs with the given Keylength.
     * @returns        total separator record length
     */
    static  tsp00_Int4 bd520RequiredLen (tsp00_Int4 KeyLen);
#if COMPILEMODE_MEO00 >= SLOW_MEO00
    ///This can, in slow kernels, print the separator.
    void bd520Print ();
#endif
private:
    tgg00_RecPtr m_pRec;
};


/*---------------------------------------------------------------------------*/
inline void cbd520_Separator::Assign (tgg00_RecPtr pSpace)
{
    m_pRec = pSpace;
}

/*---------------------------------------------------------------------------*/
inline const tgg00_RecPtr cbd520_Separator::bd520GetRecPtr() const
{
    return m_pRec;
}

/*---------------------------------------------------------------------------*/

void inline
cbd520_Separator::bd520Check (cbd300_InvCurrentBasis Current)
{
    tgg00_BasisError  &TrError = Current.curr_trans->trError_gg00;
    cbd600_Node        SubNode (Current, tbd_node_request::fromConst (nr_for_read));    
    tsp00_LcompResult  Lresult;
    tsp00_KeyPtr       pFirstKey;
    tsp00_Int2         keyLength;

    
    SubNode.bd600Move(bd520SubNodeId());
    if ( e_ok != TrError ) return;;
    
    if ( bd520KeyLen() > 0 && SubNode.bd600NumRecords() > 0 || ! SubNode.bd600IsLeaf() )
    {
        SubNode.bd600GetKey(FIRST_REC_INDEX_BD00, pFirstKey, keyLength);
        
        s30cmp (bd520KeyPtr(), POS_OFF_DIFF_BD00, bd520KeyLen(),
                pFirstKey, POS_OFF_DIFF_BD00, bd520KeyLen(), Lresult);
        
        if ( Lresult == l_greater || Lresult != l_equal && ! SubNode.bd600IsLeaf() )
        {
#           if COMPILEMODE_MEO00 >= SLOW_MEO00 
            bd520Print();
            t01sname   (bd_index, "FirstKey:   ");
            t01moveobj (bd_index, pFirstKey, POS_OFF_DIFF_BD00, POS_OFF_DIFF_BD00+keyLength);
#           endif
            TrError = e_invalid_index_structure;
            SubNode.bd600Dump (csp3_bd_msg, "bd520CheckSepKeys  Sn  ");
        }
        
    }

    if (! SubNode.bd600IsLeaf() // If subtrees are there, then propagation will happen at a later time
        &&
        bd520LeafCount() != SubNode.bd600LeafCount() )
    {
        TrError = e_invalid_index_structure;
        SubNode.bd600Dump (csp3_bd_msg, "bd520CheckIndex:LeafC Sn");
    }
}

/*---------------------------------------------------------------------------*/

inline tsp00_Int4
cbd520_Separator::bd520RequiredLen (tsp00_Int4 KeyLen)
{
    return sizeof (tgg00_HeaderRec) + KeyLen + sizeof (tbd00_SepLeafCount);
}


/*---------------------------------------------------------------------------*/

inline tsp00_PageNo
cbd520_Separator::bd520SubNodeId()
{
    return STATIC_CAST(tsp00_PageNo,m_pRec->recPno_gg00().gg90GetInt4());
}

/*---------------------------------------------------------------------------*/

inline tsp00_KeyPtr
cbd520_Separator::bd520KeyPtr()
{
    return REINTERPRET_CAST(tsp00_KeyPtr,(REINTERPRET_CAST(tsp00_BytePtr,m_pRec)+cgg_rec_key_offset));
}

/*---------------------------------------------------------------------------*/

inline tsp00_Int2 &
cbd520_Separator::bd520KeyLen()
{
    return m_pRec->recKeyLen_gg00();
}

/*---------------------------------------------------------------------------*/

inline tsp00_Int4
cbd520_Separator::bd520Length()
{
    return m_pRec->recLen_gg00();
}

/*---------------------------------------------------------------------------*/

inline tsp00_Int4
cbd520_Separator::bd520LeafCount ()
{
    return 
    ( REINTERPRET_CAST( tgg90_Cint4*, (REINTERPRET_CAST(tsp00_BytePtr,m_pRec)+cgg_rec_key_offset+m_pRec->recKeyLen_gg00()))
    )->gg90GetInt4 ();
}

/*---------------------------------------------------------------------------*/

inline void
cbd520_Separator::bd520SetLeafCount (tsp00_Int4 LeafCount)
{
    ROUTINE_DBG_MEO00 ("bd520SetLeafCount");
    ( REINTERPRET_CAST( tgg90_Cint4*, (REINTERPRET_CAST(tsp00_BytePtr,m_pRec)+cgg_rec_key_offset+m_pRec->recKeyLen_gg00()))
    )->becomes (LeafCount);
}


/*---------------------------------------------------------------------------*/

#if COMPILEMODE_MEO00 >= SLOW_MEO00

inline void
cbd520_Separator::bd520Print ()
{
    if ( NULL == m_pRec )
        t01name   (bd_index, "SEP: is NIL       ");
    else
    {
        t01p2int4 (bd_index,  "SEP: SubNode", bd520SubNodeId(),
                              "SEP: LeafCnt", bd520LeafCount());
        t01name    (bd_index, "SEP: Key          ");
        t01moveobj (bd_index, bd520KeyPtr(), POS_OFF_DIFF_BD00, bd520KeyLen());
    }
}

#endif

/*---------------------------------------------------------------------------*/

inline void
cbd520_Separator::bd520CreateNew (tsp00_KeyPtr pKey,
                                  tsp00_Int2   KeyLen,
                                  tsp00_PageNo SubNodeId,
                                  tsp00_Int4   LeafCount = 0)
{
    ROUTINE_DBG_MEO00 ("bd520CreateNew");

    tgg00_BasisError TrError = e_ok;

    m_pRec->recKeyLen_gg00() = KeyLen;
    m_pRec->recLen_gg00()    = sizeof (tgg00_HeaderRec) + KeyLen + sizeof (tbd00_SepLeafCount);
    m_pRec->recPno_gg00().becomes (SubNodeId);
    if ( NULL != pKey )
    {
        SAPDB_RangeMove( __FILE__, 1,
            sizeof(tsp00_Key), sizeof(m_pRec->recBody_gg00()),
            pKey,   POS_OFF_DIFF_BD00,
            m_pRec, cgg_rec_key_offset + POS_OFF_DIFF_BD00,
            KeyLen, TrError);
        if ( e_ok != TrError ) return;
    }
    bd520SetLeafCount (LeafCount);
#   if COMPILEMODE_MEO00 >= SLOW_MEO00
    bd520Print();
#   endif
}

/*---------------------------------------------------------------------------*/

inline void
cbd520_Separator::bd520ConvertToFirstSepOnLevel (tsp00_Int4 &SepLenDelta)
{
    /* PTS 1104481 UH 04-11-1999 SepLenDelta built in */

    if ( 0 == m_pRec->recKeyLen_gg00() )
    {
        SepLenDelta = 0;
        return;
    }
    else
    {
        tsp00_Int4 AuxLeafCount   = bd520LeafCount();
        SepLenDelta               = m_pRec->recKeyLen_gg00(); /* PTS 1106119 UH 24-03-2000 */
        m_pRec->recLen_gg00()    -= SepLenDelta;
        m_pRec->recKeyLen_gg00()  = 0;
        bd520SetLeafCount (AuxLeafCount);
        /* SepLenDelta is used outside to update the aligned Recordspace */
        SepLenDelta = -1 * SepLenDelta; /* PTS 1108210 UH 06-11-2000 */
    }
}


/*---------------------------------------------------------------------------*/

#endif  /* GBD520_H */
