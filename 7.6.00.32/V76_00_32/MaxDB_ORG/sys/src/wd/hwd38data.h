/*!**********************************************************************

  module: hwd38data.h

  -----------------------------------------------------------------------

  responsible:  Holger Bischoff
				Markus Oezgen

  special area: WebAgent

  description:  Managing list of user-data.

  see also:

  -----------------------------------------------------------------------

  copyright:            Copyright (c) 2000-2005 SAP AG



    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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

#ifndef WD38DATA_H
#define WD38DATA_H

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include "gwd00.h"
#include "hwd01wadef.h"

/*!**********************************************************************

  Chapter: twd38UserDataP

  description:
  
************************************************************************/

typedef struct st_wa_user_data *twd38UserDataP;

sapdbwa_Bool wd38NewUserData( twd38UserDataP		*userDataList,
							  sapdbwa_UInt4			 id,
							  void					*data,
							  sapdbwa_Destructor	 destructor   );

void* wd38GetUserData( twd38UserDataP	userDataList,
                       sapdbwa_UInt4		id );

sapdbwa_Bool wd38FreeUserData( twd38UserDataP	*userDataList,
							   sapdbwa_UInt4	 id            );

sapdbwa_Bool wd38FreeAllUserDatas( twd38UserDataP	*userDataList );

/*!**********************************************************************

  EndChapter: twd38UserDataP

************************************************************************/
#endif
