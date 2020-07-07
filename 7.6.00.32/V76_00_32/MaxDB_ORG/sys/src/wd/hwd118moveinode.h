/*!**********************************************************************

  module: hwd118moveinode.h

  -----------------------------------------------------------------------

  responsible:  Holger Bischoff
				Markus Oezgen

  special area: WebAgent

  description:  DB-Filesystem MoveInode function

  see also:

  -----------------------------------------------------------------------

  copyright:               Copyright (c) 1999-2005 SAP AG



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




************************************************************************/

#ifndef HWD118MOVEINODE_H
#define HWD118MOVEINODE_H

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include "gwd00.h"
#include "hwd26err.h"
#include "hwd103inode.h"

/*!**********************************************************************

  Chapter: twd118MoveInodeDescP

  description:
  
************************************************************************/

typedef struct st_move_inode_desc *twd118MoveInodeDescP;

twd118MoveInodeDescP wd118CreateMoveInodeDesc( twd104FSDescP    fsDesc );

void wd118DestroyMoveInodeDesc(  twd118MoveInodeDescP desc );

sapdbwa_Bool wd118MoveInode( twd118MoveInodeDescP  desc,
							 twd103InodeId        *inodeId,
							 twd103InodeId        *newParentInodeId,
							 const char           *newName,
							 twd26ErrP             err );
 
/*!**********************************************************************

  EndChapter: twd117RenameInodeDescP

************************************************************************/
#endif
