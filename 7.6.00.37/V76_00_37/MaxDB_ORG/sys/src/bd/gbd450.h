/*!**************************************************************************

  module      : gbd450.h
  special area: InvList Record Handling
  responsible : UweH
  last changed: 2000-08-09  16:45
  see also    : example.html ...
  Copyright (c) 1998-2005 SAP AG
  description : 



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


#ifndef GBD450_H
#define GBD450_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "gsp03.h"
#include "gsp03_3.h"
#include "ggg00.h"
#include "ggg90.h"
#include "gbd00.h"
#include "hbd102.h"   // PTS 1103980 JA 1999-09-15
#include "hgg01_1.h"
#include "hsp30.h"
#include "SAPDB/SAPDBCommon/SAPDB_RangeCode.hpp"

#if COMPILEMODE_MEO00 >= SLOW_MEO00 
#include "hta99.h"
#endif
    
/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define FIRST_PRIM_KEY_INDEX_BD450      0
#define NIL_PRIM_KEY_INDEX_BD450       -1

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

inline static void bd450IndexKeyCompare (
    bool                                 ConsistentReadSupport,
    const void*                          buf1,
    tsp00_Int4                           fieldpos1,
    tsp00_Int4                           fieldlength1,
    const tgg91_TransNo*                 pWriteTrans,
    const void*                          buf2,
    tsp00_Int4                           fieldpos2,
    tsp00_Int4                           fieldlength2,
    tsp00_LcompResult&                   l_result)
{
    if (!ConsistentReadSupport)
    {
        s30cmp (buf1, fieldpos1, fieldlength1,
            buf2, fieldpos2, fieldlength2, l_result);
    }
    else
    {
        const SAPDB_Byte* p = reinterpret_cast<const SAPDB_Byte*>(buf2) + fieldpos2 - 1;
        s30cmp (buf1, fieldpos1, fieldlength1,
            buf2, fieldpos2, fieldlength2 - sizeof(tgg91_TransNo), l_result);
        if (l_equal == l_result)
        {
            if (pWriteTrans)
            {
                s30cmp (pWriteTrans, 1, sizeof(tgg91_TransNo), 
                    buf2, fieldpos2 + fieldlength2 - sizeof(tgg91_TransNo), sizeof(tgg91_TransNo), l_result);
            }
            else
            {
                l_result.becomes(l_less);
            }
        }
    }
}
 
/*---------------------------------------------------------------------------*/
/*!
    class:       cbd450_InvListArray
    description: 
*/
class cbd450_InvListArray
{
public:

	cbd450_InvListArray ();

    cbd450_InvListArray (bool ConsistentReadSupport, tgg00_RecPtr pRec, bool Convert = false);

    cbd450_InvListArray (tgg00_TransContext	&Trans,
                         bool                ConsistentReadSupport,
                         const tgg00_RecPtr  pNewRec,
                         tsp00_KeyPtr        SecKey,
						 tsp00_Int4          SecKeyLen,
                         tsp00_KeyPtr        PrimKey,
						 tsp00_Int4          PrimKeyLen);

#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
    ~cbd450_InvListArray ();
#   endif

    void              bd450CheckInvListArray   () const;

    tsp00_Int2        bd450PrimKeyLen          (tsp00_Int4 PrimKeyIndex) const;

    tsp00_Int2        bd450PrimKeyOffset       (tsp00_Int4 PrimKeyIndex) const;

    void              bd450InsertPrimKey       (tgg00_TransContext	&Trans,
                                                tsp00_Int4			 PrimKeyIndex, 
                                                tsp00_KeyPtr		 NewPrimKey,
						                        tsp00_Int4			 NewPrimKeyLen);

    void              bd450DeletePrimKey       (tgg00_TransContext	&Trans, 
		                                        tsp00_Int4			 PrimKeyIndex,
		                                        bool				&bRemoveRecord);

    void              bd450SearchPrimKey       (tsp00_KeyPtr         PrimKey,
						                        tsp00_Int4           PrimKeyLen,
                                                const tgg91_TransNo* pWriteTrans,
		                                        tsp00_Int4           &PrimKeyIndex,
		                                        tbd_searchresult     &SearchResult) const;

    void              bd450SearchPrimKey       (tsp00_KeyPtr      PrimKey,
						                        tsp00_Int4        PrimKeyLen,
                                                tgg91_TransNo    &WriteTrans,
		                                        tsp00_Int4       &PrimKeyIndex,
 		                                        tbd_searchresult &SearchResult) const;

    void              bd450SkipPrimKey         (tsp00_KeyPtr      PrimKey,
						                        tsp00_Int4        PrimKeyLen,
		                                        tsp00_Int4       &PrimKeyIndex,
		                                        tbd_searchresult &SearchResult);


    void              bd450GetPrimKey          (tsp00_Int4       PrimKeyIndex,
                                                tsp00_KeyPtr    &PrimKey,
						                        tsp00_Int2      &PrimKeyLen) const;

    void              bd450GetPrimKey          (tsp00_Int4       PrimKeyIndex,
                                                tsp00_KeyPtr    &PrimKey,
						                        tsp00_Int2      &PrimKeyLen,
                                                bool            &HasDeletedFlag) const;

    void              bd450GetSecKey           (tsp00_KeyPtr    &pSecKey,
						                        tsp00_Int2      &SecKeyLen) const; // PTS 1116651 TS 2002-07-15

    bool              bd450IsEqualKey          (tsp00_Int4       PrimKeyIndex,
                                                tsp00_KeyPtr    &PrimKey,
						                        int              PrimKeyLen) const;

    void              bd450MarkPrimKeyDeleted  (tsp00_Int4 PrimKeyIndex);

    static tsp00_Int4 bd450NewSize             (bool       ConsistentReadSupport,
                                                tsp00_Int4 SecKeyLen,
		                                        tsp00_Int4 SumPrimKeyLen,
		                                        tsp00_Int4 NumPrimKeys = 1);

	bool              bd450CheckPrimKey        (tsp00_Int4        PrimKeyLen,
		                                        tsp00_Int4        PrimKeyOffset,
		                                        tgg00_BasisError &TrError) const;

	void              bd450AppendPrimKeyOffset (tsp00_Int4 PrimKeyOffset,
	            	                            tsp00_Int4 PrimKeyLen);

    tgg00_RecPtr      bd450RecPtr              () {return m_pRec;};

    inline void       bd450RemoveDeletedFlag   (tsp00_Int4 PrimKeyIndex);

	tsp00_Int4        bd450Size                (tsp00_Int4 PrimKeyLen) const;

	void              bd450Update              (tgg00_RecPtr pNewRec);

private:

  struct tbd450_PrimKeyOffset
    {
        tsp00_Int2  pkoOffset_bd450;
        tsp00_Int2  pkoLen_bd450;
    };

    tgg00_RecPtr          m_pRec;
    tbd450_PrimKeyOffset *m_pMinPrimKeyOffset;
    bool                  m_ConsistentReadSupport;

	void                   bd450_CheckInvList (tgg00_RecPtr pRec) const;
#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
    void                   bd450_Check (tsp00_Int4 PrimKeyIndex) const;
#   endif

    void                   bd450_ConvertToInvListArray (tgg00_RecPtr pRec);

	tbd450_PrimKeyOffset * bd450_MinPrimKeyOffsetPtr ()
		/* this method can not be externally implemented,        */
		/* because the used return type is only known internally */
	{
		return ( (tbd450_PrimKeyOffset*)
			((tsp00_BytePtr)(m_pRec)            +
			 m_pRec->recLen_gg00()      -
			 (m_pRec->recNumPrimKeys_gg00() * sizeof(tbd450_PrimKeyOffset))
			) );
	}

};
/*! endclass: cbd450_InvListArray */

/*---------------------------------------------------------------------------*/

struct tbd450_InvListRefSubTree
{
		tgg90_Cint4    ilsSubRoot_bd450;
		tgg90_Cint4    ilsNumPrimKeys_bd450;
		tgg90_Cint4    ilsNumLeaves_bd450;
};

/*---------------------------------------------------------------------------*/
/*!
    class:       cbd450_InvListRefSubTree
    description: 
*/
class cbd450_InvListRefSubTree
{
public:
	cbd450_InvListRefSubTree          (){};

	cbd450_InvListRefSubTree          (tgg00_RecPtr         pRec); 

	cbd450_InvListRefSubTree          (tgg00_RecPtr         pRec,
                                       tsp00_PageNo         SubRoot,
                                       tsp00_Int4           NumPrimKeys); 

	cbd450_InvListRefSubTree          (cbd450_InvListArray &InvListArray,
                                       tsp00_PageNo         SubRoot,
                                       tsp00_Int4           SubTreeNumLeaves); 
	
	cbd450_InvListRefSubTree          (tgg00_TransContext	&Trans,
		                               tgg00_RecPtr			pNewRec,
                                       tsp00_KeyPtr			pSecKey,
						               tsp00_Int4			SecKeyLen,
									   tsp00_PageNo			SubRoot,
									   tsp00_Int4			NumPrimKeys,
									   tsp00_Int4           NumLeaves); 

	void         		bd450DecrNumPrimKeys  ();

	void				bd450IncrNumPrimKeys ();

	tsp00_PageNo		bd450SubRoot         () const;

	tsp00_Int4			bd450NumPrimKeys     () const;

	tsp00_Int4			bd450NumLeaves       () const; 

	void				bd450SetSubRoot      (tsp00_PageNo  SubRoot);

	void				bd450SetNumPrimKeys  (tsp00_Int4    NumPrimKeys);
	
	void				bd450SetNumLeaves    (tsp00_Int4    NumLeaves); 

    void				bd450Init            (tgg00_RecPtr	pRec);

	static tsp00_Int4	bd450NewSize	 	 (tsp00_Int4    SecKeyLen);

    void                bd450GetSecKey       (tsp00_KeyPtr    &pSecKey,
						                      tsp00_Int2      &SecKeyLen) const; // PTS 1116651 TS 2002-07-15
private:

    void				bd450_Init			(tgg00_RecPtr	pRec,
											 tsp00_PageNo	SubRoot,
											 tsp00_Int4	NumPrimKeys,
											 tsp00_Int4	SubTreeNumLeaves);	
private:

	tbd450_InvListRefSubTree * m_InvListRefSubTree;
    // PTS 1116651 TS 2002-07-15
    tsp00_KeyPtr               m_pSecKey; // points to a secondary key stored on a data page!
    tsp00_Int2                 m_SecKeyLen;
    // PTS 1116651
	
};
/*! endclass: cbd450_InvListRefSubTree */

inline static bool bd450IsSameKeyOfThisTrans (tgg91_TransNo    &WriteTrans,
                                         tsp00_KeyPtr      pPrimKey,
                                         tsp00_Int4        PrimKeyLen,
                                         tsp00_KeyPtr      pKeyInIndex,
                                         tsp00_Int4        KeyInIndexLen)
{
    // returns true, if the given key (without write trans) is equal to the
    // given key from an inv list
    if (PrimKeyLen + sizeof(tgg91_TransNo) != KeyInIndexLen)
    {
        return false;
    }
    tsp00_LcompResult compResult;
    s30cmp (pPrimKey, 1, PrimKeyLen, pKeyInIndex, 1, PrimKeyLen, compResult);
    if (l_equal != compResult)
    {
        return false;
    }
    for (int ix = 0; ix < sizeof(WriteTrans); ++ix)
    {
        if (pKeyInIndex[PrimKeyLen + ix] != 255 - WriteTrans[ix])
        {
            return false;
        }
    }
    return true;
}

/*===========================================================================*
 *  CLASS METHODS OF class cbd450_InvListArray                               *
 *===========================================================================*/

#if COMPILEMODE_MEO00 >= SLOW_MEO00 

inline void
cbd450_InvListArray::bd450_Check (tsp00_Int4 PrimKeyIndex) const
{
    if (PrimKeyIndex >= m_pRec->recNumPrimKeys_gg00() || PrimKeyIndex < 0)
		g01abort (csp3_bd_msg, csp3_n_btree, "BD450_CHECK: wrong Index", PrimKeyIndex);
}

#endif

/*---------------------------------------------------------------------------*/

inline void
cbd450_InvListArray::bd450MarkPrimKeyDeleted  (tsp00_Int4 PrimKeyIndex)
{
    ROUTINE_DBG_MEO00 ("bd450MarkPrimKeyDeleted");
    (m_pMinPrimKeyOffset + PrimKeyIndex)->pkoLen_bd450 |= 0x8000;
}

/*---------------------------------------------------------------------------*/

inline tsp00_Int4
cbd450_InvListArray::bd450NewSize (bool       ConsistentReadSupport,
                                   tsp00_Int4 SecKeyLen,
								   tsp00_Int4 SumPrimKeyLen,
								   tsp00_Int4 NumPrimKeys)
{
    if (ConsistentReadSupport)
    {
        SumPrimKeyLen += NumPrimKeys * sizeof(tgg91_TransNo);
    }
    return 
		cgg_rec_key_offset                           +
		bd102RecAlign (SecKeyLen + SumPrimKeyLen)    + 
		NumPrimKeys * sizeof (tbd450_PrimKeyOffset);
}

/*---------------------------------------------------------------------------*/

inline tsp00_Int4
cbd450_InvListArray::bd450Size (tsp00_Int4 PrimKeyLen) const
{
    if (m_ConsistentReadSupport)
    {
        PrimKeyLen += sizeof(tgg91_TransNo);
    }
    return m_pRec->recLen_gg00() + bd102RecAlign (PrimKeyLen - m_pRec->recAlignment_gg00()) + sizeof (tbd450_PrimKeyOffset);
}


/*---------------------------------------------------------------------------*/

inline void
cbd450_InvListArray::bd450_CheckInvList (tgg00_RecPtr pRec) const
{
	/* check InvListKind */
    switch ( pRec->recInvListKind_gg00 () )
    {
    case  ilkArray_egg00: ;
    case  ilkSubTree_egg00: break;
    default:
        g01abort (csp3_bd_msg, csp3_n_btree, "GBD450: Wrong List Kind ", pRec->recInvListKind_gg00 ());
    }
	
    /* check Alignment */
    if (pRec->recAlignment_gg00() > REC_ALIGN_BD00 - 1)
    {
        g01abort (csp3_bd_msg, csp3_n_btree, "GBD450: Wrong Alignment ", pRec->recAlignment_gg00() );
    }
	
	/* check number of prim keys */
	/* check Alignment */
    if (pRec->recNumPrimKeys_gg00() > MAX_RECLEN_GG00)
    {
        g01abort (csp3_bd_msg, csp3_n_btree, "GBD450:Wrong NumPrimKeys", pRec->recNumPrimKeys_gg00());
    }
}

/*---------------------------------------------------------------------------*/

inline bool 
cbd450_InvListArray::bd450CheckPrimKey (tsp00_Int4        PrimKeyLen, 
										tsp00_Int4        PrimKeyOffset,
										tgg00_BasisError &TrError) const
{
    ROUTINE_DBG_MEO00 ("bd450CheckPrimKey");

	const tsp00_Int4 MinPrimKeyOffset    = cgg_rec_key_offset + m_pRec->recKeyLen_gg00();
	const tsp00_Int4 MaxInvListArraySize = (tsp00_Int4)(tsp00_BytePtr(m_pMinPrimKeyOffset) - tsp00_BytePtr(m_pRec));
	
    if ((PrimKeyOffset < MinPrimKeyOffset) ||
		(PrimKeyOffset > MaxInvListArraySize))
    {
        TrError = e_illegal_entrypos;
        g01opmsg (sp3p_knldiag, sp3m_error, csp03_b31_5_illegal_entrypos,
            csp3_n_btree, "Illegal PrimKeyOffset   ", PrimKeyOffset);
		g01opmsg (sp3p_knldiag, sp3m_error, csp03_b31_5_illegal_entrypos,
            csp3_n_btree, "        PrimKey Length  ", PrimKeyLen);
        g01abort (csp3_bd_msg, csp3_n_btree, "GBD450: Illegal EntryPos", PrimKeyOffset);

#       if COMPILEMODE_MEO00 >= SLOW_MEO00 
        t01moveobj (bd_inv, m_pRec, POS_OFF_DIFF_BD00, m_pRec->recLen_gg00());
#       endif

        return (true);
    }        
    
    if ((PrimKeyLen < 1) || 
		(PrimKeyLen > PERM_KEY_MXSP00) || 
		(PrimKeyOffset + PrimKeyLen > MaxInvListArraySize))
    {
        TrError = e_illegal_keylength;
        g01opmsg (sp3p_knldiag, sp3m_error, csp03_b31_5_illegal_entrypos,
            csp3_n_btree, "Illegal PrimKey Length  ", PrimKeyLen);
		g01opmsg (sp3p_knldiag, sp3m_error, csp03_b31_5_illegal_entrypos,
            csp3_n_btree, "        PrimKeyOffset   ", PrimKeyOffset);
        g01abort (csp3_bd_msg, csp3_n_btree, "GBD450: Illegal EntryPos", PrimKeyOffset);

#       if COMPILEMODE_MEO00 >= SLOW_MEO00 
        t01moveobj (bd_inv, m_pRec, POS_OFF_DIFF_BD00, m_pRec->recLen_gg00());
#       endif

        return (true);
    }
    return (false);
}   

/*---------------------------------------------------------------------------*/

inline void 
cbd450_InvListArray::bd450DeletePrimKey (tgg00_TransContext	&Trans, 
										 tsp00_Int4          PrimKeyIndex,
										 bool               &bRemoveRecord)
{
    ROUTINE_DBG_MEO00 ("bd450DeletePrimKey");

#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
	t01int4 (bd_inv, "PrimKeyIndex", PrimKeyIndex);
#   endif

    bRemoveRecord = false;
	tsp00_Int4                    OldPrimKeyLen    = bd450PrimKeyLen(PrimKeyIndex);
	tsp00_Int4                    OldPrimKeyOffset = (m_pMinPrimKeyOffset + PrimKeyIndex)->pkoOffset_bd450;
	tsp00_Int4                    OldAlignment     = m_pRec->recAlignment_gg00();
    tgg00_BasisError             &TrError          = Trans.trError_gg00;
	
	/* return if the primary key to remove is the last entry in the list */
	if (1 == m_pRec->recNumPrimKeys_gg00())
	{
		bRemoveRecord = true;
		return;
	}
	
	/* shift primary key with offsets larger than the given primary key */
	tsp00_Int4  MoveLen      = (tsp00_Int4)(tsp00_BytePtr(m_pMinPrimKeyOffset) - tsp00_BytePtr(m_pRec) - OldPrimKeyLen - OldPrimKeyOffset);
	tsp00_Int4  TargetOffset = OldPrimKeyOffset;
	
	SAPDB_RangeOverlappingMove ( __FILE__, 1,    
        m_pRec->recLen_gg00(), m_pRec->recLen_gg00(),
        m_pRec, POS_OFF_DIFF_BD00 + TargetOffset + OldPrimKeyLen,
		m_pRec, POS_OFF_DIFF_BD00 + TargetOffset,
        MoveLen, TrError);
	
    /* shift offset list for offsets with indexes less than PrimKeyIndex */
	const tsp00_Int4 NewSumKeyLens   = (tsp00_Int4)(tsp00_BytePtr(m_pMinPrimKeyOffset) - tsp00_BytePtr(m_pRec)  
		- cgg_rec_key_offset - OldAlignment - OldPrimKeyLen); 
	tsp00_Int4       NewAlignment    =  bd102RecAlign(NewSumKeyLens) - NewSumKeyLens;

	MoveLen      = PrimKeyIndex * sizeof (tbd450_PrimKeyOffset);
	TargetOffset = (tsp00_Int4)(tsp00_BytePtr(m_pMinPrimKeyOffset) - tsp00_BytePtr(m_pRec)
		- OldPrimKeyLen - OldAlignment + NewAlignment); 
	
	if (MoveLen > 0) // in case of the first element there is no move required
	{
		SAPDB_RangeOverlappingMove( __FILE__, 2,    
			m_pRec->recLen_gg00(), m_pRec->recLen_gg00(),
			m_pMinPrimKeyOffset, POS_OFF_DIFF_BD00,
			m_pRec,              POS_OFF_DIFF_BD00 + TargetOffset,
			MoveLen, TrError);
	}	
	
	/* 	shift the offset list for indexes larger than PrimKeyIndex */
	MoveLen      = (m_pRec->recNumPrimKeys_gg00()-1-PrimKeyIndex) * sizeof (tbd450_PrimKeyOffset);
	TargetOffset += PrimKeyIndex * sizeof (tbd450_PrimKeyOffset);
	
	SAPDB_RangeOverlappingMove( __FILE__, 3,    
		m_pRec->recLen_gg00(), m_pRec->recLen_gg00(),
		m_pMinPrimKeyOffset, POS_OFF_DIFF_BD00 + ((PrimKeyIndex+1) * sizeof (tbd450_PrimKeyOffset)),
		m_pRec, POS_OFF_DIFF_BD00 + TargetOffset,
		MoveLen, TrError);
	
    /* update record header and member variables */
	m_pRec->recLen_gg00() -=  (tsp00_Int4)(OldPrimKeyLen + OldAlignment + sizeof (tbd450_PrimKeyOffset) - NewAlignment);
	m_pRec->recNumPrimKeys_gg00() --;
    m_pRec->recAlignment_gg00()   = tsp00_Uint1 (NewAlignment);
    m_pMinPrimKeyOffset   = bd450_MinPrimKeyOffsetPtr();
	
	/* update prim key offset list */
	for (tsp00_Int4 Index = 0; Index < m_pRec->recNumPrimKeys_gg00(); Index ++)
	{
		if ((m_pMinPrimKeyOffset+Index)->pkoOffset_bd450 > OldPrimKeyOffset)
			(m_pMinPrimKeyOffset+Index)->pkoOffset_bd450 -= OldPrimKeyLen;
	}
}  		

/*---------------------------------------------------------------------------*/

inline void
cbd450_InvListArray::bd450_ConvertToInvListArray (tgg00_RecPtr pRec)
{
    ROUTINE_DBG_MEO00 ("bd450_ConvertToInvListArray");

	const tsp00_Int4 AlignedLen = bd102RecAlign(pRec->recKeyLen_gg00());

    m_pRec = pRec;

	/* SecKey and KeyLen is already set */

	m_pRec->recLen_gg00() = cgg_rec_key_offset + AlignedLen;

	m_pRec->recInvListKind_gg00 ().becomes (ilkArray_egg00);
	m_pRec->recNumPrimKeys_gg00() = 0;
    m_pRec->recAlignment_gg00()   = tsp00_Uint1 (AlignedLen - m_pRec->recKeyLen_gg00());
    
	m_pMinPrimKeyOffset = bd450_MinPrimKeyOffsetPtr ();
}

/*---------------------------------------------------------------------------*/

inline void 
cbd450_InvListArray::bd450InsertPrimKey (tgg00_TransContext	&Trans, 
                                         tsp00_Int4			 PrimKeyIndex, 
                                         tsp00_KeyPtr		 NewPrimKey,
						                 tsp00_Int4			 NewPrimKeyLen)
{
	ROUTINE_DBG_MEO00 ("bd450InsertPrimKey");

#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
	t01int4 (bd_inv, "PrimKeyIndex", PrimKeyIndex);
#   endif

    tgg00_BasisError      &TrError       = Trans.trError_gg00;
	tsp00_Int4             OldAlignment  = m_pRec->recAlignment_gg00();   // in bytes
	tsp00_Int4             NewPrimKeyOffset;
	tsp00_Int4             MoveLen       = 
		((m_pRec->recNumPrimKeys_gg00() - PrimKeyIndex) * sizeof(tbd450_PrimKeyOffset));
    int                    HistorySpace  = (m_ConsistentReadSupport) ? sizeof(tgg91_TransNo) : 0;
	tsp00_Int4             TargetOffset  = bd102RecAlign(NewPrimKeyLen + HistorySpace - OldAlignment) ;

    m_pRec->recAlignment_gg00() = tsp00_Uint1 (TargetOffset - (NewPrimKeyLen + HistorySpace - OldAlignment));
	
	/* add space for new prim key */
    SAPDB_RangeOverlappingMove ("GBD450", 4,    
        sizeof (m_pRec->recBody_gg00()), sizeof (m_pRec->recBody_gg00()),
        m_pMinPrimKeyOffset + PrimKeyIndex, POS_OFF_DIFF_BD00,
		m_pMinPrimKeyOffset + PrimKeyIndex, POS_OFF_DIFF_BD00 + TargetOffset + sizeof(tbd450_PrimKeyOffset),
        MoveLen,
		TrError);

	if (TrError != e_ok) return;
	
	if (FIRST_PRIM_KEY_INDEX_BD450 < PrimKeyIndex)
	{
        SAPDB_RangeOverlappingMove( "GBD450", 5,    
			sizeof (m_pRec->recBody_gg00()), sizeof (m_pRec->recBody_gg00()),
			m_pMinPrimKeyOffset, POS_OFF_DIFF_BD00,
			m_pMinPrimKeyOffset, POS_OFF_DIFF_BD00 + TargetOffset,
			PrimKeyIndex * sizeof(tbd450_PrimKeyOffset),
			TrError);
		
		if (TrError != e_ok) return;
	}
	
	m_pRec->recLen_gg00() += (tsp00_Int4)(TargetOffset +  sizeof(tbd450_PrimKeyOffset));
	
	m_pRec->recNumPrimKeys_gg00() ++;
	
	/* move prim key */
	NewPrimKeyOffset = (tsp00_Int4)((char*)(m_pMinPrimKeyOffset) - OldAlignment - (char*)m_pRec);

    SAPDB_RangeMove ( __FILE__, 6,    
        sizeof (tsp00_Key), m_pRec->recLen_gg00(),
        NewPrimKey, POS_OFF_DIFF_BD00,
		m_pRec, POS_OFF_DIFF_BD00 + NewPrimKeyOffset,
        NewPrimKeyLen, TrError);

	if (TrError != e_ok) return;

    if (m_ConsistentReadSupport)
    {
        int offset = POS_OFF_DIFF_BD00 + NewPrimKeyOffset + NewPrimKeyLen - 1;
        for (int ix = 0; ix < sizeof(tgg91_TransNo); ++ix)
        {
            m_pRec->recBuf_gg00()[offset + ix] = 255 - Trans.trWriteTransId_gg00[ix];
                
        }
    }

	/* update OffsetList / add space and insert new Offset for Prim Key */
    m_pMinPrimKeyOffset = bd450_MinPrimKeyOffsetPtr ();
	(m_pMinPrimKeyOffset + PrimKeyIndex)->pkoLen_bd450    = NewPrimKeyLen + HistorySpace;
	(m_pMinPrimKeyOffset + PrimKeyIndex)->pkoOffset_bd450 = NewPrimKeyOffset;
}

/*---------------------------------------------------------------------------*/

inline void 
cbd450_InvListArray::bd450AppendPrimKeyOffset (tsp00_Int4 NewPrimKeyOffset,
											   tsp00_Int4 NewPrimKeyLen)
{													 
	ROUTINE_DBG_MEO00 ("bd450AppendPrimKeyOffset");

	m_pMinPrimKeyOffset                 -= 1;
	m_pMinPrimKeyOffset->pkoLen_bd450    = NewPrimKeyLen;
	m_pMinPrimKeyOffset->pkoOffset_bd450 = NewPrimKeyOffset;
}

/*---------------------------------------------------------------------------*/

inline tsp00_Int2
cbd450_InvListArray::bd450PrimKeyLen (tsp00_Int4 PrimKeyIndex) const
{
#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
    bd450_Check (PrimKeyIndex);
#   endif

    return (m_pMinPrimKeyOffset + PrimKeyIndex)->pkoLen_bd450 & 0x7FFF;
}

/*---------------------------------------------------------------------------*/

inline tsp00_Int2
cbd450_InvListArray::bd450PrimKeyOffset (tsp00_Int4 PrimKeyIndex) const
{
#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
    bd450_Check (PrimKeyIndex);
#   endif

    return (m_pMinPrimKeyOffset + PrimKeyIndex)->pkoOffset_bd450;
}

/*---------------------------------------------------------------------------*/

inline void 
cbd450_InvListArray::bd450GetPrimKey (tsp00_Int4    PrimKeyIndex,
                                      tsp00_KeyPtr &PrimKey,
						              tsp00_Int2   &PrimKeyLen) const
{
    ROUTINE_DBG_MEO00 ("bd450GetPrimKey");
	
	PrimKeyLen = bd450PrimKeyLen (PrimKeyIndex);
	PrimKey    = tsp00_KeyPtr ((tsp00_BytePtr)(m_pRec) + bd450PrimKeyOffset (PrimKeyIndex));
}

/*---------------------------------------------------------------------------*/

inline void 
cbd450_InvListArray::bd450GetPrimKey (tsp00_Int4    PrimKeyIndex,
                                      tsp00_KeyPtr &PrimKey,
						              tsp00_Int2   &PrimKeyLen,
                                      bool         &HasDeletedFlag) const
{
    ROUTINE_DBG_MEO00 ("bd450GetPrimKey");
	
	PrimKeyLen     = bd450PrimKeyLen (PrimKeyIndex);
	PrimKey        = tsp00_KeyPtr ((tsp00_BytePtr)(m_pRec) + bd450PrimKeyOffset (PrimKeyIndex));
    HasDeletedFlag = ((m_pMinPrimKeyOffset + PrimKeyIndex)->pkoLen_bd450 & 0x8000) != 0;
}

/*---------------------------------------------------------------------------*/

inline void
cbd450_InvListArray::bd450GetSecKey (tsp00_KeyPtr    &pSecKey,
						             tsp00_Int2      &SecKeyLen) const // PTS 1116651 TS 2002-07-15
{
    ROUTINE_DBG_MEO00 ("bd450GetSecKey");

    SecKeyLen = m_pRec->recKeyLen_gg00();
	pSecKey   = tsp00_KeyPtr (&(m_pRec->recKey_gg00().keyVal_gg00()));
}

/*---------------------------------------------------------------------------*/

inline
bool cbd450_InvListArray::bd450IsEqualKey (tsp00_Int4       PrimKeyIndex,
                                           tsp00_KeyPtr    &PrimKey,
						                   int              PrimKeyLen) const 
{
    tsp00_KeyPtr pPrimKey;
    tsp00_Int2   primKeyLen;
    this->bd450GetPrimKey(PrimKeyIndex, pPrimKey, primKeyLen);
    if (PrimKeyLen == primKeyLen - sizeof(tgg91_TransNo))
    {
        tsp00_LcompResult l_result;
        s30cmp (pPrimKey, 1, PrimKeyLen,
            PrimKey, 1, PrimKeyLen, l_result);
        if (l_equal == l_result)
        {
            return true;
        }
    }
    return false;
}

/*---------------------------------------------------------------------------*/

inline void
cbd450_InvListArray::bd450RemoveDeletedFlag  (tsp00_Int4 PrimKeyIndex)
{
    (m_pMinPrimKeyOffset + PrimKeyIndex)->pkoLen_bd450 &= 0x7FFF;
}

/*---------------------------------------------------------------------------*/

inline void
cbd450_InvListArray::bd450SearchPrimKey (tsp00_KeyPtr          PrimKey,
						                 tsp00_Int4            PrimKeyLen,
                                         const tgg91_TransNo*  pWriteTrans,
                                         tsp00_Int4            &PrimKeyIndex,
										 tbd_searchresult      &SearchResult) const
{
    ROUTINE_DBG_MEO00 ("bd450SearchPrimKey");
    
    tsp00_LcompResult       CompResult;
    tsp00_Int4              Count;
    tsp00_Int4              CurrPrimKeyOffset;
    tsp00_Int4              CurrPrimKeyLen;
    
    tsp00_Int4              MaxPKIndex = m_pRec->recNumPrimKeys_gg00() - 1;
    tsp00_Int4              MinPKIndex = FIRST_PRIM_KEY_INDEX_BD450;

	SearchResult.becomes(nonefound);
  
    /*  MaxPKIndex > MinPKIndex */
    do
    {
        Count = MaxPKIndex - MinPKIndex + 1;
      
        if (Count > 2)
        {
            PrimKeyIndex      = MaxPKIndex - (Count >> 1);
            CurrPrimKeyOffset = bd450PrimKeyOffset (PrimKeyIndex);
            CurrPrimKeyLen    = bd450PrimKeyLen    (PrimKeyIndex);
            
            bd450IndexKeyCompare (m_ConsistentReadSupport, PrimKey, 1, PrimKeyLen, pWriteTrans,
                m_pRec, CurrPrimKeyOffset + POS_OFF_DIFF_BD00, CurrPrimKeyLen, CompResult);
            
            if (l_equal == CompResult)
				SearchResult.becomes(thisfound);
            else
                if (l_less == CompResult)
                    MaxPKIndex = PrimKeyIndex;
                else
                    MinPKIndex = PrimKeyIndex;
        }
        else
        {
            if (Count == 2)
            {
                PrimKeyIndex      = MaxPKIndex;
                CurrPrimKeyOffset = bd450PrimKeyOffset (PrimKeyIndex);
                CurrPrimKeyLen    = bd450PrimKeyLen    (PrimKeyIndex);
                
                bd450IndexKeyCompare (m_ConsistentReadSupport, PrimKey, 1, PrimKeyLen, pWriteTrans,
                    m_pRec, CurrPrimKeyOffset + POS_OFF_DIFF_BD00, CurrPrimKeyLen, CompResult);
                
                if (l_equal == CompResult)
					SearchResult.becomes(thisfound);
                else  if (l_less == CompResult)
                    MaxPKIndex = MinPKIndex;
                else
                    MinPKIndex = MaxPKIndex;
            }
            else
            {
                PrimKeyIndex      = MaxPKIndex;
                CurrPrimKeyOffset = bd450PrimKeyOffset (PrimKeyIndex);
                CurrPrimKeyLen    = bd450PrimKeyLen    (PrimKeyIndex);
                
                bd450IndexKeyCompare (m_ConsistentReadSupport, PrimKey, 1, PrimKeyLen, pWriteTrans,
                    m_pRec, CurrPrimKeyOffset + POS_OFF_DIFF_BD00, CurrPrimKeyLen, CompResult);
                
                if (l_equal == CompResult)
					SearchResult.becomes(thisfound);
                else if  (l_less == CompResult)
					SearchResult.becomes(nextfound);
                else
					SearchResult.becomes(lastfound);
            }
        }
    }
    while (nonefound == SearchResult);              
}

/*---------------------------------------------------------------------------*/

inline void
cbd450_InvListArray::bd450SearchPrimKey (tsp00_KeyPtr      PrimKey,
   				                         tsp00_Int4        PrimKeyLen,
                                         tgg91_TransNo    &WriteTrans,
		                                 tsp00_Int4       &PrimKeyIndex,
 		                                 tbd_searchresult &SearchResult) const
{
    bd450SearchPrimKey (PrimKey, PrimKeyLen, &WriteTrans, PrimKeyIndex, SearchResult); // TODO still required ?
    //if (nextfound == SearchResult)
    //{
    //
    //    tsp00_KeyPtr pPrimKey;
    //    tsp00_Int2   primKeyLen;
    //    bd450GetPrimKey (PrimKeyIndex, pPrimKey, primKeyLen);
    //    if (bd450IsSameKeyOfThisTrans (WriteTrans, PrimKey, PrimKeyLen, pPrimKey, primKeyLen))
    //    {
    //        SearchResult.becomes(thisfound);
    //    }
    //}
}


/*---------------------------------------------------------------------------*/

inline void
cbd450_InvListArray::bd450SkipPrimKey     (tsp00_KeyPtr      pSkipPrimKey,
						                   tsp00_Int4        SkipPrimKeyLen,
		                                   tsp00_Int4       &PrimKeyIndex,
		                                   tbd_searchresult &SearchResult)
{
    SearchResult.becomes(nextfound);
    do
    {
        tsp00_KeyPtr pPrimKey;
        tsp00_Int2   primKeyLen;
        bd450GetPrimKey (PrimKeyIndex, pPrimKey, primKeyLen);
        tsp00_LcompResult compResult;
        s30cmp (pSkipPrimKey, 1, SkipPrimKeyLen, pPrimKey, 1, SkipPrimKeyLen, compResult);
        if (l_equal == compResult)
        {
            SearchResult.becomes(thisfound);
            ++PrimKeyIndex;
            if (PrimKeyIndex != m_pRec->recNumPrimKeys_gg00())
            {
                continue;
            }
        }
        if (thisfound == SearchResult)
        {
            --PrimKeyIndex;
        }
        break;
    }
    while (true);
}

/*---------------------------------------------------------------------------*/

inline void
cbd450_InvListArray::bd450CheckInvListArray() const
{
	 ROUTINE_DBG_MEO00 ("bd450CheckInvListArray");

    /* check the inv list trailer */
    if (NULL == m_pRec) return; // InvListArray Record was deleted

    tgg00_BasisError        TrError = e_ok; 
    tsp00_LcompResult       CompResult;
	
	if (ilkArray_egg00 == m_pRec->recInvListKind_gg00 ())
	{	
		tsp00_Int4  PrimKeyIndex;
	    tsp00_Int4  PrimKeyOffset;
        tsp00_Int4  PrimKeyLen;
		tsp00_Int4  PrevPrimKeyOffset =  bd450PrimKeyOffset (FIRST_PRIM_KEY_INDEX_BD450);
		tsp00_Int4  PrevPrimKeyLen    = bd450PrimKeyLen    (FIRST_PRIM_KEY_INDEX_BD450);	
		
		for (PrimKeyIndex = FIRST_PRIM_KEY_INDEX_BD450 + 1; PrimKeyIndex < m_pRec->recNumPrimKeys_gg00(); PrimKeyIndex++)
		{
			PrimKeyOffset = bd450PrimKeyOffset (PrimKeyIndex);
			PrimKeyLen    = bd450PrimKeyLen    (PrimKeyIndex);
            
            if (bd450CheckPrimKey (PrimKeyLen, PrimKeyOffset, TrError)) return;

            bd450IndexKeyCompare (false, m_pRec, PrimKeyOffset + POS_OFF_DIFF_BD00, PrimKeyLen, NULL, 
                m_pRec, PrevPrimKeyOffset + POS_OFF_DIFF_BD00, PrevPrimKeyLen, CompResult);
			if (CompResult != l_greater)
			{
				g01opmsg (sp3p_knldiag, sp3m_error, csp03_b31_1_illegal_entrypos, 
					csp3_n_btree, "GBD450: Wrong Key Order ", PrimKeyIndex);
				g01opmsg (sp3p_knldiag, sp3m_error,	csp03_b31_1_illegal_entrypos, 
					csp3_n_btree, "GBD450: Wrong Key Order ", PrimKeyIndex);
                g01abort (csp3_bd_msg, csp3_n_btree, "GBD450: Wrong Key Order ", PrimKeyIndex);
			}	
			if (e_ok != TrError) return;

            PrevPrimKeyOffset = PrimKeyOffset;
            PrevPrimKeyLen    = PrimKeyLen;
		}
	}
}

/*---------------------------------------------------------------------------*/

inline void
cbd450_InvListArray::bd450Update (tgg00_RecPtr pNewRec)
{
#if COMPILEMODE_MEO00 >= SLOW_MEO00 // Only for Destructor Check 
    if (NULL == pNewRec)
    {
        m_pRec              = NULL;
        m_pMinPrimKeyOffset = NULL;
    }
    else
    {
        m_pRec              = pNewRec;
        m_pMinPrimKeyOffset = bd450_MinPrimKeyOffsetPtr ();
    }
#else
    m_pRec              = pNewRec;
    m_pMinPrimKeyOffset = bd450_MinPrimKeyOffsetPtr ();
#endif   
}

/*---------------------------------------------------------------------------*/

inline
cbd450_InvListArray::cbd450_InvListArray ()
{
    ROUTINE_DBG_MEO00 ("cbd450_InvListArray1");

	m_pRec = NULL;
}

/*---------------------------------------------------------------------------*/

inline
cbd450_InvListArray::cbd450_InvListArray (bool ConsistentReadSupport, tgg00_RecPtr pRec, bool Convert)
: m_ConsistentReadSupport(ConsistentReadSupport)
{
    ROUTINE_DBG_MEO00 ("cbd450_InvListArray2");

	if (Convert)
		bd450_ConvertToInvListArray (pRec);
	else
		bd450Update (pRec);

#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
	/* check the inv list trailer */
    bd450_CheckInvList (pRec);
#   endif
}

/*---------------------------------------------------------------------------*/

inline
cbd450_InvListArray::cbd450_InvListArray (tgg00_TransContext   &Trans,
                                          bool                  ConsistentReadSupport,
                                          const tgg00_RecPtr    pNewRec,
                                          tsp00_KeyPtr          SecKey,
						                  tsp00_Int4            SecKeyLen,
                                          tsp00_KeyPtr          PrimKey,
						                  tsp00_Int4            PrimKeyLen)
: m_ConsistentReadSupport(ConsistentReadSupport)
{
    // Precondition: Space in page is available
    
    ROUTINE_DBG_MEO00 ("cbd450_InvListArray3");

    tgg00_BasisError &TrError    = Trans.trError_gg00;
	tsp00_Int4        AlignedLen = bd102RecAlign(SecKeyLen);
        
    m_pRec = pNewRec;

    SAPDB_RangeMove ( __FILE__, 7,
        sizeof (tsp00_Key), sizeof (m_pRec->recBody_gg00()),
        SecKey, POS_OFF_DIFF_BD00, m_pRec->recBody_gg00(), POS_OFF_DIFF_BD00,
        SecKeyLen, TrError);
    
    if  (TrError != e_ok) return;
    
    m_pRec->recKeyLen_gg00() = SecKeyLen;
	m_pRec->recLen_gg00()    = cgg_rec_key_offset + AlignedLen;

	m_pRec->recInvListKind_gg00 ().becomes (ilkArray_egg00);
	m_pRec->recAlignment_gg00()   = tsp00_Uint1 (AlignedLen - m_pRec->recKeyLen_gg00());
	m_pRec->recNumPrimKeys_gg00() = 0;
    
	m_pMinPrimKeyOffset = bd450_MinPrimKeyOffsetPtr ();

    bd450InsertPrimKey (Trans, FIRST_PRIM_KEY_INDEX_BD450, PrimKey, PrimKeyLen);
}

/*---------------------------------------------------------------------------*/

#if COMPILEMODE_MEO00 >= SLOW_MEO00 

inline
cbd450_InvListArray::~cbd450_InvListArray ()
{
    ROUTINE_DBG_MEO00 ("cbd450~_InvListArrayHandle");

	/* CHECK THE STRUCTURE */

	/* check the inv list trailer */
    if (NULL == m_pRec) return; // InvListArray Record was deleted

    bd450_CheckInvList (m_pRec);

    bd450CheckInvListArray ();
	
}

#endif

/*===========================================================================*
 *  CLASS METHODS class cbd450_InvListRefSubTree                             *
 *===========================================================================*/

inline void
cbd450_InvListRefSubTree:: bd450Init(tgg00_RecPtr pRec)
{
	m_InvListRefSubTree = (tbd450_InvListRefSubTree *) ((char *)(pRec) +  pRec->recLen_gg00()
		- sizeof (tbd450_InvListRefSubTree));

    m_pSecKey   = tsp00_KeyPtr (&(pRec->recKey_gg00().keyVal_gg00()));  // PTS 1116651 TS 2002-07-15
    m_SecKeyLen = pRec->recKeyLen_gg00();                               // PTS 1116651 TS 2002-07-15
}

/*---------------------------------------------------------------------------*/

inline void
cbd450_InvListRefSubTree::bd450GetSecKey (tsp00_KeyPtr    &pSecKey,
						                  tsp00_Int2      &SecKeyLen) const // PTS 1116651 TS 2002-07-15
{
    ROUTINE_DBG_MEO00 ("bd450GetSecKey");

    SecKeyLen = m_SecKeyLen;
	pSecKey   = m_pSecKey;
}

/*---------------------------------------------------------------------------*/

inline tsp00_Int4
cbd450_InvListRefSubTree::bd450NewSize (tsp00_Int4 SecKeyLen)
{
    return 
		cgg_rec_key_offset + SecKeyLen + sizeof (tbd450_InvListRefSubTree);
}

/*---------------------------------------------------------------------------*/

inline void
cbd450_InvListRefSubTree::bd450DecrNumPrimKeys()
{
	--m_InvListRefSubTree->ilsNumPrimKeys_bd450;
}

/*---------------------------------------------------------------------------*/

inline void
cbd450_InvListRefSubTree::bd450IncrNumPrimKeys()
{
	++m_InvListRefSubTree->ilsNumPrimKeys_bd450;
}

/*---------------------------------------------------------------------------*/

inline tsp00_PageNo
cbd450_InvListRefSubTree::bd450SubRoot() const
{
	return (tsp00_PageNo (m_InvListRefSubTree->ilsSubRoot_bd450.gg90GetInt4()));
}

/*---------------------------------------------------------------------------*/

inline void 
cbd450_InvListRefSubTree::bd450SetSubRoot(tsp00_PageNo SubRoot)
{
	m_InvListRefSubTree->ilsSubRoot_bd450.becomes (tsp00_Int4 (SubRoot));
}

/*---------------------------------------------------------------------------*/

inline tsp00_Int4 
cbd450_InvListRefSubTree::bd450NumPrimKeys () const
{
	return (m_InvListRefSubTree->ilsNumPrimKeys_bd450.gg90GetInt4());
};

/*---------------------------------------------------------------------------*/

inline void 
cbd450_InvListRefSubTree::bd450SetNumPrimKeys  (tsp00_Int4 NumPrimKeys)
{
	m_InvListRefSubTree->ilsNumPrimKeys_bd450.becomes (NumPrimKeys);
}

/*---------------------------------------------------------------------------*/

inline tsp00_Int4 
cbd450_InvListRefSubTree::bd450NumLeaves () const
{
	return (m_InvListRefSubTree->ilsNumLeaves_bd450.gg90GetInt4());
}

/*---------------------------------------------------------------------------*/

inline void 
cbd450_InvListRefSubTree::bd450SetNumLeaves  (tsp00_Int4 NumLeaves)
{
	m_InvListRefSubTree->ilsNumLeaves_bd450.becomes (NumLeaves);
}

/*---------------------------------------------------------------------------*/

inline cbd450_InvListRefSubTree::cbd450_InvListRefSubTree (tgg00_RecPtr pRec)
{
	bd450Init(pRec);
};

/*---------------------------------------------------------------------------*/

inline
cbd450_InvListRefSubTree::cbd450_InvListRefSubTree (tgg00_RecPtr pRec,
													tsp00_PageNo SubRoot,
  													tsp00_Int4 NumPrimKeys)
{
	bd450Init(pRec);
	bd450SetSubRoot (SubRoot);
	bd450SetNumPrimKeys (NumPrimKeys);
	bd450SetNumLeaves (1);
}

/*---------------------------------------------------------------------------*/

inline
cbd450_InvListRefSubTree::cbd450_InvListRefSubTree (cbd450_InvListArray &InvListArray,
													tsp00_PageNo         SubRoot,
                                                    tsp00_Int4           SubTreeNumLeaves)
{
	/* changes the incoming InvListArray into a InvListRefSubTree Record at the same place */

	      tgg00_RecPtr pRec        = InvListArray.bd450RecPtr();
		  tsp00_Int4   NumPrimKeys = pRec->recNumPrimKeys_gg00() + 1;

	pRec->recLen_gg00() = cgg_rec_key_offset + pRec->recKeyLen_gg00() +
		sizeof (*m_InvListRefSubTree);

	pRec->recInvListKind_gg00().becomes(ilkSubTree_egg00);
	pRec->recAlignment_gg00()   = 0;
    pRec->recNumPrimKeys_gg00() = 0;

	bd450_Init (pRec, SubRoot, NumPrimKeys, SubTreeNumLeaves);
}

/*---------------------------------------------------------------------------*/

inline
cbd450_InvListRefSubTree::cbd450_InvListRefSubTree (tgg00_TransContext	&Trans,
													tgg00_RecPtr		 pNewRec,
													tsp00_KeyPtr		 pSecKey,
													tsp00_Int4			 SecKeyLen,
													tsp00_PageNo		 SubRoot,
													tsp00_Int4			 NumPrimKeys,
													tsp00_Int4           SubTreeNumLeaves)
{
	ROUTINE_DBG_MEO00 ("cbd450_InvListRefSubTree");
	
	
	pNewRec->recLen_gg00()         = (tsp00_Int4)(cgg_rec_key_offset + SecKeyLen + sizeof (*m_InvListRefSubTree));
	pNewRec->recKeyLen_gg00()      = SecKeyLen;
	pNewRec->recInvListKind_gg00().becomes(ilkSubTree_egg00);
	pNewRec->recAlignment_gg00()   = 0;
    pNewRec->recNumPrimKeys_gg00() = 0;
	
    SAPDB_RangeMove( __FILE__, 8,
        sizeof (tsp00_Key), sizeof (pNewRec->recBody_gg00()),
        pSecKey, POS_OFF_DIFF_BD00, pNewRec->recBody_gg00(), POS_OFF_DIFF_BD00,
        SecKeyLen, Trans.trError_gg00);
    
    if  (Trans.trError_gg00 != e_ok) return;
    
	bd450_Init (pNewRec, SubRoot, NumPrimKeys, SubTreeNumLeaves);
}

/*---------------------------------------------------------------------------*/

inline void 
cbd450_InvListRefSubTree::bd450_Init (tgg00_RecPtr	pRec,
									  tsp00_PageNo	SubRoot,
									  tsp00_Int4	NumPrimKeys,
									  tsp00_Int4	SubTreeNumLeaves	
									  )
{
	bd450Init           (pRec);
	bd450SetSubRoot     (SubRoot);
	bd450SetNumPrimKeys (NumPrimKeys);
	bd450SetNumLeaves   (SubTreeNumLeaves);
}

#endif  /* GBD450_H */
