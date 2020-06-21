/*!**********************************************************************

  module: hwd114seldir.h

  -----------------------------------------------------------------------

  responsible:  Holger Bischoff
				Markus Oezgen

  special area: WebAgent

  description:  DB-Filesystem directory listings

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

#ifndef HWD114SELDIR_H
#define HWD114SELDIR_H

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include "gwd00.h"
#include "hwd26err.h"
#include "hwd101dbfs.h"
#include "hwd103inode.h"

/*!**********************************************************************

  Chapter: twd114SelectDirDescP

  description:
  
************************************************************************/

typedef struct st_select_dir_desc *twd114SelectDirDescP;

twd114SelectDirDescP wd114CreateSelectDirDesc( twd104FSDescP fsDesc );

void wd114DestroySelectDirDesc(  twd114SelectDirDescP desc );


sapdbwa_Bool wd114DbDeselectInodeByDirectory( twd114SelectDirDescP desc,
                                            twd26ErrP            err );

sapdbwa_Bool wd114DbFetchInodeByDirectory( twd114SelectDirDescP desc,
                                         twd103InodeP         inode,
                                         twd26ErrP            err );

sapdbwa_Bool wd114DbSelectInodeByDirectory( twd114SelectDirDescP  desc,
                                          twd103InodeId        *inodeId,
                                          twd101OrderFlag       orderFlag,
                                          twd26ErrP             err );

/*!**********************************************************************

  EndChapter: twd114SelectDirDescP

************************************************************************/
#endif
