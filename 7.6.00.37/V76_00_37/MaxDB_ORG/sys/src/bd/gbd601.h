/*!
  @file           gbd601.h
  @ingroup        Subtree Leaf Handling
  @author         UweH
  @brief          defines class cbd601_SubTreeNode

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


#ifndef GBD601_H
#define GBD601_H

#include "gbd600.h"

class cbd300_InvCurrentBasis;

/*!
@class          cbd601_SubTreeNode
@brief          methodes to handle Node-Pages
*/
class cbd601_SubTreeNode : public cbd600_Node
{
public:
/*!
   @brief this is for accessing the root node.
   @param Current [in/out] for TrError, TaskID.
   @param PageLockMode [in] How the Page of the node is locked. (default = CacheLock)
 */
cbd601_SubTreeNode (cbd300_InvCurrentBasis   &Current,
                    const tbd00_PageLockMode &PageLockMode = tbd00_PageLockMode::fromConst(plmLock_ebd00))
: cbd600_Node(Current,PageLockMode)
{}
/*!
   @brief This constructs an empty node, which can be used to access any node.
   @param Current [in/out] for TrError, TaskID.
   @param RequestKind [in] How pages are requested from the Datacache (for_read/for_update)
   @param PageLockMode [in] How the Page of the node is locked. (default = CacheLock)
 */
cbd601_SubTreeNode (cbd300_InvCurrentBasis   &Current,
                    const tbd_node_request   &RequestKind,
                    const tbd00_PageLockMode &PageLockMode = tbd00_PageLockMode::fromConst(plmLock_ebd00))
: cbd600_Node(Current,RequestKind,PageLockMode)
{}
};
#endif  /* GBD601_H */
