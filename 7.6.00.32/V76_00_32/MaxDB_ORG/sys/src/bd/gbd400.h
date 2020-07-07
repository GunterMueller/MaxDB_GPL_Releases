/*!
  @file           gbd400.h
  @ingroup        InvTree Handling
  @author         UweH
  @brief          defines the class cbd400_InvTree

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
*/
#ifndef GBD400_H
#define GBD400_H

#include "gbd00.h"
#include "gbd500.h"
#include "gbd600.h"
#include "gbd601.h"

#define DO_NOT_CHECK_LOCK_BD400 false
/*!
@interface      cbd400_InvTree
@brief          This class offers methods to handle B*-Trees used for Indexes

TreeLock protocoll is implemented here and InvTree special tree functions.
*/
class cbd400_InvTree : public cbd500_Tree
{
public:
    /*!
    @param LeafNode [in/out] InternalTreeNode, TransContext and GlobalErrorFlag
    */
    cbd400_InvTree (cbd600_Node &LeafNode);
    /*!
    @param          SecKey [in] Specifies the InvList to append the PrimKey.
    @param          PrimKey [in] The PrimKey to be appended.
    @param          bNextSecKey [in] The incoming SecKey is greater than the prevoius
                                     SecKeys and therefore a new InvList must be created.
    @param          bInvListRefNeeded [in] Indicates that more PrimKeys will be inserted
                                     as an InvListArray can hold. Therefore the
                                     new InvList is created as an InvListRef. Note
                                     that an InvListArray Overflow is possible 
                                     furthermore.
    @param          RecIndex [in/out] If given, the SecKey is not searched.
    @brief          PrimKey is appended to the InvList specified by SecKey.

    If RecIndex is NIL_RECINDEX_BD00, the InvList ist searched in the InvTree,
    If InvList is not found, it is created.
    RecIndex specifies the current InvList.
    The internal Node points to the last LeafNode.
    */
    void bd400AppendPrimKey      (tsp00_KeyPtr  pSecKey,
                                  tsp00_Int4    SecKeyLen,
                                  tsp00_KeyPtr  pPrimKey,
                                  tsp00_Int4    PrimKeyLen,
                                  bool          bNextSecKey,       /* PTS 1104438 UH 03-12-1999 */
                                  bool          bInvListRefNeeded, /* PTS 1104438 UH 03-12-1999 */
                                  tsp00_Int4   &RecIndex);
protected:
    tgg00_BasisError        &m_TrError;
    cbd300_InvSubTreeCurrent m_SubTreeCurrent;
    cbd601_SubTreeNode       m_SubTreeLeaf;
    cbd500_Tree              m_SubTree;
    /// Releases m_SubTree, m_SubTreeLeaf and assigns them to a new tree.
    void bd400_SetToNewSubRoot ();
};

inline
cbd400_InvTree::cbd400_InvTree (cbd600_Node &LeafNode)
: cbd500_Tree      (LeafNode),
  m_TrError        (LeafNode.m_Current.curr_trans->trError_gg00),
  m_SubTreeCurrent (LeafNode.m_Current, false),
  m_SubTreeLeaf    (m_SubTreeCurrent, LeafNode.m_RequestKind),
  m_SubTree        (m_SubTreeLeaf)
{}

inline void
cbd400_InvTree::bd400_SetToNewSubRoot ()
{
    ROUTINE_DBG_MEO00 ("bd400_SetToNewSubRoot");
    tsp00_PageNo invroot = m_Node.bd600GetNodePtr() != 0
                           ? m_Node.bd600GetNodePtr()->nd_root()
                           : NIL_PAGE_NO_GG00;
    /* create a new SubTree */
    m_SubTreeLeaf.bd600Release       (IS_CHANGED_BD600);
    
    m_SubTreeCurrent.bd300UnlockTree ();
    
    m_SubTreeCurrent.bd300NewRoot    (invroot);
    if (e_ok != m_TrError) return;

    m_SubTree.bd500SetToRoot (DO_NOT_CHECK_LOCK_BD400); // set m_SubTreeLeaf to the root
    if (e_ok != m_TrError) return;
}

#endif  /* GBD400_H */
