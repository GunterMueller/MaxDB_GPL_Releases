/*!**********************************************************************

  module: hwd111getinode.h

  -----------------------------------------------------------------------

  responsible:  Holger Bischoff
				Markus Oezgen

  special area: WebAgent

  description:  DB-Filesystem GetInodeByName function

  see also:

  -----------------------------------------------------------------------

                         Copyright (c) 1998-2005 SAP AG



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




************************************************************************/

#ifndef HWD111GETINODE_H
#define HWD111GETINODE_H

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include "gwd00.h"
#include "hwd26err.h"
#include "hwd103inode.h"

/*!**********************************************************************

  Chapter: twd111GetInodeDescP

  description:
  
************************************************************************/

typedef struct st_get_inode_desc *twd111GetInodeDescP;

twd111GetInodeDescP wd111CreateGetInodeDesc( twd104FSDescP    fsDesc );

void wd111DestroyGetInodeDesc(  twd111GetInodeDescP desc );

sapdbwa_Bool wd111DbGetInodeByName( twd111GetInodeDescP  desc,
                                  twd103InodeId       *Directory,
                                  char                *Name,
                                  twd103InodeP         Inode,
                                  twd26ErrP            err );

sapdbwa_Bool wd111GetInodeById( twd111GetInodeDescP  desc,
                              twd103InodeId       *inodeId,
                              twd103InodeP         inode,
                              twd26ErrP            err );

/*!**********************************************************************

  EndChapter: twd111GetInodeDescP

************************************************************************/
#endif
