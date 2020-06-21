/*!
  @file           gbd501.h
  @ingroup        Internal Tree Classes
  @author         UweH
  @brief          Types and Classes to restructure the tree

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
#ifndef GBD501_H
#define GBD501_H

#include "gsp00.h"
#include "ggg00.h"
#include "gbd00.h"
#include "gbd600.h"

#include "gsp03.h"   // opmsg
#include "gsp03_3.h" // opmsg
#include "hgg01_1.h" // g01abort /* PTS 1108839 UH 2000-12-27 */

#if COMPILEMODE_MEO00 >= SLOW_MEO00 
#include "hta99.h"
#endif

/* PTS 1108839 UH 2000-12-27 */
#define INVALID_KEY_LENGTH_GBD501 -1
/* PTS 1108839 UH 2000-12-27 */
#define MAX_ORDERS_GBD501          MAX_LEVEL_BD00 + 2

/*---------------------------------------------------------------------------*/
enum tbd501_NeighborKind
{
    rightNeighbor_ebd501,
    leftNeighbor_ebd501
};

/*---------------------------------------------------------------------------*/

enum tbd501_BalancingType
{
	btNone_ebd501,             // 0
	btAppend_ebd501,           // 1
	btDistToRight_ebd501,      // 2
	btDistToLeft_ebd501,       // 3
	btDistToNextRight_ebd501,  // 4
	btRemoveEmpty_ebd501,      // 5
	btDistFromRight_ebd501,    // 6
	btDistFromLeft_ebd501,     // 7
	btMergeFromRight_ebd501,   // 8
	btMergeFromLeft_ebd501,    // 9
	btPropagate_ebd501,        // 10
	btPropagateWithKey_ebd501, // 11
	btNewLevel_ebd501,         // 12
	btRemoveLevel_ebd501       // 13
};

/*---------------------------------------------------------------------------*/

enum tbd501_IndexOrderKind
{
	iokNil_ebd501,
	iokChange_ebd501,
	iokAdd_ebd501,
	iokDelete_ebd501
};

/*---------------------------------------------------------------------------*/
class cbd501_IndexOrders;

class cbd501_IndexOrder
{
friend class cbd501_IndexOrders;
public:
	tbd501_IndexOrderKind ioKind_bd501;
	char                  ioFiller1_bd501; /* PTS 1108839 UH 2000-12-27 */
	tsp00_Int2            ioSepKeyLen_bd501;
public:
    cbd501_IndexOrder(); // PTS ? UH 2001-11-23 new
    bool                  ioKeyIsInvalid();  /* PTS 1108839 UH 2000-12-27 */
    void                  ioSetKeyInvalid(); /* PTS 1108839 UH 2000-12-27 */
	tsp00_PageNo          ioSubNodeId_bd501();
	tsp00_Int4            ioLeafCount_bd501();
    tsp00_Int4            ioFromLevel_bd501();
	tsp00_KeyPtr          ioSepKey_bd501();
#   if COMPILEMODE_MEO00 >= SLOW_MEO00
	void bd501Print();
#   endif
protected:
    void Init(); // PTS ? UH 2001-11-23 new
protected:
	tsp00_PageNo m_ioSubNodeId_bd501;
	tsp00_Int4   m_ioLeafCount_bd501;
    tsp00_Int4   m_ioFromLevel_bd501;
	tsp00_KeyPtr m_ioSepKeyPtr_bd501;
    tbd_nodeptr  m_ioSubNode_bd501;
};


/*---------------------------------------------------------------------------*/

class cbd501_IndexOrders
{
public:
	cbd501_IndexOrders ();
    void               bd501CreateNew   (tbd501_IndexOrderKind  Kind,
		                                 tbd_nodeptr            pSubNode,
                                         tsp00_PageNo           neighborId,
                                         tbd501_NeighborKind    neighborKind,
						                 bool                   bSepKeyInNode,
		                                 tsp00_KeyPtr           pSepKey,
	                                     tsp00_Int4             SepKeyLen);
	cbd501_IndexOrder* bd501GetOrder    (tsp00_Int4 OrderNo);
#   if COMPILEMODE_MEO00 >= SLOW_MEO00
	inline void        bd501Print            ();
#   endif
	void               bd501Trace            (tgg00_TransContext &Trans);
private:
	cbd501_IndexOrder* bd501GetNewOrder      ();
	cbd501_IndexOrder* bd501NewOrderAt (int OrderNo); // PTS 1112713 UH 2001-11-22
    void               bd501RemoveOrder(int OrderNo); // PTS 1115899 UH 2002-05-24 end
public:
	tsp00_Int4         iosOrderCount_bd501;
private:
	cbd501_IndexOrder  iosOrders_bd501[MAX_ORDERS_GBD501];    /* PTS 1108839 UH 2000-12-27 */
};

/*---------------------------------------------------------------------------*/

inline void cbd501_IndexOrder::Init()
{
    // PTS ? UH 2001-11-23 new
    ioKind_bd501        = iokNil_ebd501;
    ioSepKeyLen_bd501   = 0;
    m_ioSubNodeId_bd501 = NIL_PAGE_NO_GG00;
    m_ioLeafCount_bd501 = 0;
    m_ioFromLevel_bd501 = 0;
    m_ioSepKeyPtr_bd501 = NULL;
    m_ioSubNode_bd501   = NULL;
}

/*---------------------------------------------------------------------------*/

inline cbd501_IndexOrder::cbd501_IndexOrder()
{
     // PTS ? UH 2001-11-23 new
     Init();
}

/*---------------------------------------------------------------------------*/

inline bool
cbd501_IndexOrder::ioKeyIsInvalid()
{
    /* PTS 1108839 UH 2000-12-27 new */
    return INVALID_KEY_LENGTH_GBD501 == ioSepKeyLen_bd501;
}

/*---------------------------------------------------------------------------*/

inline void
cbd501_IndexOrder::ioSetKeyInvalid()
{
    /* PTS 1108839 UH 2000-12-27 new */
    ioSepKeyLen_bd501 = INVALID_KEY_LENGTH_GBD501;
}

/*---------------------------------------------------------------------------*/

inline tsp00_PageNo
cbd501_IndexOrder::ioSubNodeId_bd501()
{
	return m_ioSubNodeId_bd501;
}

/*---------------------------------------------------------------------------*/

inline tsp00_Int4
cbd501_IndexOrder::ioFromLevel_bd501()
{
	return m_ioFromLevel_bd501;
}

/*---------------------------------------------------------------------------*/

inline tsp00_Int4
cbd501_IndexOrder::ioLeafCount_bd501()
{
	return m_ioLeafCount_bd501;
}

/*---------------------------------------------------------------------------*/

inline tsp00_KeyPtr
cbd501_IndexOrder::ioSepKey_bd501()
{
	/* change without key */

	if ( ioKeyIsInvalid() )
        return NULL;

	/* pSepKey can be used */

	if ( NULL != m_ioSepKeyPtr_bd501 )
        return m_ioSepKeyPtr_bd501;

	/* pSepKey must be determined out of Node again */
	 
    cbd610_RecordList recordList (m_ioSubNode_bd501);
    tsp00_Int2        keyLength;
    tsp00_KeyPtr      pKey;
    
    recordList.GetKey(FIRST_REC_INDEX_BD00, keyLength, pKey);
    
    return pKey;
}

/*---------------------------------------------------------------------------*/

#if COMPILEMODE_MEO00 >= SLOW_MEO00

inline void
cbd501_IndexOrder::bd501Print()
{
	t01sname  (bd_index, "IndexOrder: ");
	t01p2int4 (bd_index, "OrderKind:  ", ioKind_bd501,
		                 "SubNodeId:  ", ioSubNodeId_bd501());
	t01p2int4 (bd_index, "FromLevel:  ", ioFromLevel_bd501(),
		                 "LeafCount:  ", ioLeafCount_bd501());
	if ( ioKeyIsInvalid() || NULL == ioSepKey_bd501() )
		t01sname  (bd_index, "no Key      "                       );
	else
	{
		t01sname  (bd_index, "Key         "                       );
		t01moveobj(bd_index, ioSepKey_bd501(), POS_OFF_DIFF_BD00, ioSepKeyLen_bd501);
	}
}

#endif

/*---------------------------------------------------------------------------*/

inline
cbd501_IndexOrders::cbd501_IndexOrders()
: iosOrderCount_bd501 (0)
{
    /* PTS 1108839 UH 2000-12-27 */
    for (int OrderNo = 0; OrderNo < MAX_ORDERS_GBD501; ++OrderNo)
        iosOrders_bd501[OrderNo].ioSetKeyInvalid();
}


/*---------------------------------------------------------------------------*/

inline 	cbd501_IndexOrder*
cbd501_IndexOrders::bd501GetOrder (tsp00_Int4 OrderNo)
{
    /* PTS 1108839 UH 2000-12-27 rewritten */

    if ( OrderNo >= iosOrderCount_bd501 || OrderNo < 0 )
        g01abort (csp3_bd_msg, csp3_n_btree, "bd501GetOrder: invalid  ",
                  OrderNo);

	return &(iosOrders_bd501[OrderNo]);
}

/*---------------------------------------------------------------------------*/

inline 	cbd501_IndexOrder*
cbd501_IndexOrders::bd501GetNewOrder ()
{
    /* PTS 1108839 UH 2000-12-27 rewritten */

    if ( iosOrderCount_bd501 >= MAX_ORDERS_GBD501 )
        g01abort (csp3_bd_msg, csp3_n_btree, "bd501GetNewOrder:too man",
                  iosOrderCount_bd501);

    ++iosOrderCount_bd501;
	return iosOrders_bd501 + iosOrderCount_bd501-1;
}

/*---------------------------------------------------------------------------*/

inline cbd501_IndexOrder*
cbd501_IndexOrders::bd501NewOrderAt (int OrderNo)
{
    /* PTS ? UH 2001-11-23 new */
    // 1,2,3 : new at 2 => 1,2,3,4 => 1,2,3(2),4(3) => return 2
    // move all orders on position to the right, inclusively the given OrderNo
    
    if ( OrderNo == iosOrderCount_bd501 )
        // wanted is behind current orders => create a new one at the end
        return bd501GetNewOrder();
        
    (void) bd501GetNewOrder();                             // first reserve space for a new order
    cbd501_IndexOrder* newOrder = bd501GetOrder (OrderNo); // check if OrderNo is valid

    for ( int OrderNoToMove = iosOrderCount_bd501-2; OrderNoToMove >= OrderNo; --OrderNoToMove )
        iosOrders_bd501[OrderNoToMove+1] = iosOrders_bd501[OrderNoToMove];

    newOrder->Init();
    
    return newOrder;
}

/*---------------------------------------------------------------------------*/

inline void
cbd501_IndexOrders::bd501RemoveOrder ( int OrderNo )
{
    // PTS 1115899 UH 2002-05-24 new
    for ( int OrderNoToMoveOn = OrderNo; OrderNoToMoveOn < iosOrderCount_bd501-1; ++OrderNoToMoveOn )
        iosOrders_bd501[OrderNoToMoveOn] = iosOrders_bd501[OrderNoToMoveOn+1];
	--iosOrderCount_bd501;
    iosOrders_bd501[iosOrderCount_bd501].Init();
}

/*---------------------------------------------------------------------------*/

inline void
cbd501_IndexOrders::bd501CreateNew   (tbd501_IndexOrderKind Kind,
		                              tbd_nodeptr           pSubNode,
                                      tsp00_PageNo          neighborId,
                                      tbd501_NeighborKind   neighborKind,
						              bool                  bSepKeyInNode = false,
		                              tsp00_KeyPtr          pSepKey       = NULL,
	                                  tsp00_Int4            SepKeyLen     = INVALID_KEY_LENGTH_GBD501 /* PTS 1108839 UH 2000-12-27 */ )
{
	ROUTINE_DBG_MEO00 ("bd501CreateNew");
    
    // PTS ? UH 2001-11-23 completely rewritten
    
    int                OrderNo = -1;
    cbd501_IndexOrder* NewOrder = NULL;
    
    // search for an order which is already assigned to subnodeid
    
    for (OrderNo = 0; OrderNo < iosOrderCount_bd501; ++OrderNo)
    {
        if ( iosOrders_bd501[OrderNo].m_ioSubNodeId_bd501 == pSubNode->nd_id() )
        {
            NewOrder = &iosOrders_bd501[OrderNo];
            break;
        }
    }
    
    if ( NewOrder == NULL )
    {
        // This is a new order
        
        if ( iosOrderCount_bd501 == 0 || neighborId == NIL_PAGE_NO_GG00 )
            // The order is added at the end of the orderlist.
            NewOrder = bd501GetNewOrder(); 
        else
        {
            // find a position to insert the new order
            for (OrderNo = 0; OrderNo < iosOrderCount_bd501; ++OrderNo)
                if ( iosOrders_bd501[OrderNo].m_ioSubNodeId_bd501 == neighborId )
                {
                    NewOrder = bd501NewOrderAt (OrderNo + ((neighborKind == leftNeighbor_ebd501)?1:0) );
                    break;
                }
             if ( NewOrder == NULL )
                 // If the neighbor is not found in the orderlist before
                 // a new order at the end is taken.
                 NewOrder = bd501GetNewOrder(); 
        }

        // Initializes the new order
    
        if ( NewOrder == NULL )
            g01abort (csp3_bd_msg, csp3_n_btree, "bd501CreateNew: Ord nil2", pSubNode->nd_id());

        cbd501_IndexOrder& order = *NewOrder;
        
        order.ioKind_bd501        = Kind;
        order.ioSepKeyLen_bd501   = SepKeyLen;
        order.m_ioSubNodeId_bd501 = pSubNode->nd_id();
        order.m_ioLeafCount_bd501 = pSubNode->ndLeafCount_bd00();
        order.m_ioFromLevel_bd501 = pSubNode->nd_level();
        
        if ( iokDelete_ebd501 != Kind )
            order.m_ioSubNode_bd501 = pSubNode;
        else
            order.m_ioSubNode_bd501 = NULL;
    
        if ( bSepKeyInNode || (INVALID_KEY_LENGTH_GBD501 == SepKeyLen) )
            order.m_ioSepKeyPtr_bd501  = NULL;
        else
            order.m_ioSepKeyPtr_bd501 = pSepKey;
    }
    else
    {
        // There is an existing order found which operates on the same subnode as the current new order.

        cbd501_IndexOrder& order = *NewOrder;

        if ( (iokDelete_ebd501 != Kind && order.m_ioSubNode_bd501 != pSubNode)
             ||
             order.m_ioSubNodeId_bd501 != pSubNode->nd_id() )
        {
            // This is impossible
            // bd501Trace (tgg00_TransContext &Trans)
            g01abort (csp3_bd_msg, csp3_n_btree, "bd501CreateNew: inv ord3", pSubNode->nd_id());
        }

        // PTS 1115899 UH 2002-05-24 included in the next lines
        // PTS 1131880 UH 2004-10-15 structured with decision matrix
        
        switch ( order.ioKind_bd501 )
        {
            case iokAdd_ebd501 :
                switch ( Kind )
                {
                    case iokAdd_ebd501 :
                        g01abort (csp3_bd_msg, csp3_n_btree, "bd501CreateNew: inv ord1", pSubNode->nd_id());
                        break;
                    case iokDelete_ebd501 :
                        bd501RemoveOrder (OrderNo);
                        break;
                    case iokChange_ebd501 :
                        order.m_ioLeafCount_bd501 = pSubNode->ndLeafCount_bd00();
                        break;
                    default:
                        g01abort (csp3_bd_msg, csp3_n_btree, "bd501CreateNew: inv ord2", order.ioKind_bd501);
                }
                break;
            case iokDelete_ebd501 :
                switch ( Kind )
                {
                    case iokAdd_ebd501 :
                        order.ioKind_bd501        = Kind;
                        order.m_ioLeafCount_bd501 = pSubNode->ndLeafCount_bd00();
                        break;
                    case iokDelete_ebd501 :
                        g01abort (csp3_bd_msg, csp3_n_btree, "bd501CreateNew: inv ord3", pSubNode->nd_id());
                        break;
                    case iokChange_ebd501 :
                        g01abort (csp3_bd_msg, csp3_n_btree, "bd501CreateNew: inv ord4", pSubNode->nd_id());
                        break;
                    default:
                        g01abort (csp3_bd_msg, csp3_n_btree, "bd501CreateNew: inv ord5", order.ioKind_bd501);
                }
                break;
            case iokChange_ebd501 :
                switch ( Kind )
                {
                    case iokAdd_ebd501 :
                        g01abort (csp3_bd_msg, csp3_n_btree, "bd501CreateNew: inv ord6", pSubNode->nd_id());
                        break;
                    case iokDelete_ebd501 :
                        order.ioKind_bd501        = Kind;
                        order.m_ioSubNode_bd501   = 0;
                        order.m_ioLeafCount_bd501 = 0;
                        break;
                    case iokChange_ebd501 :
                        order.m_ioLeafCount_bd501 = pSubNode->ndLeafCount_bd00();
                        break;
                    default:
                        g01abort (csp3_bd_msg, csp3_n_btree, "bd501CreateNew: inv ord7", order.ioKind_bd501);
                }
                break;
            default:
                g01abort (csp3_bd_msg, csp3_n_btree, "bd501CreateNew: inv ord8", order.ioKind_bd501);
        }
        
        // PTS 1113462 UH 2001-01-17 begin
        if ( SepKeyLen != INVALID_KEY_LENGTH_GBD501 )
        {
            // There is another new key for the existing order.
            order.ioSepKeyLen_bd501 = SepKeyLen;
            if ( bSepKeyInNode )
                order.m_ioSepKeyPtr_bd501 = NULL;
            else
                order.m_ioSepKeyPtr_bd501 = pSepKey;
        }
        // PTS 1113462 UH 2001-01-17 end
    }
    
#   if COMPILEMODE_MEO00 >= SLOW_MEO00 
    NewOrder->bd501Print();
	bd501Print();
#   endif
}

/*---------------------------------------------------------------------------*/

#if COMPILEMODE_MEO00 >= SLOW_MEO00
inline void
cbd501_IndexOrders::bd501Print()
{
	t01int4 (bd_index, "IndexOrders:", iosOrderCount_bd501);
	t01name (bd_index, "------------------");
	for (tsp00_Int4 OrderNo = 0; OrderNo < iosOrderCount_bd501; ++OrderNo)
	{
		iosOrders_bd501[OrderNo].bd501Print();
		t01name (bd_index, "------------------");
	}
}
#endif


/*---------------------------------------------------------------------------*/

#endif  /* GBD501_H */
