/*!**********************************************************************

  module: hwd23wa.h

  -----------------------------------------------------------------------

  responsible:  Holger Bischoff
				Markus Oezgen

  special area: WebAgent

  description:  sapdbwa_Handle functions

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

#ifndef HWD23WA_H
#define HWD23WA_H

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include "gwd00.h"
#include "hwd03waapi.h"
#include "hwd26err.h"
#include "hwd37wses.h"
#include "hwd40user.h"

/*!**********************************************************************

  Chapter: sapdbwa_Handle

  description:
  
************************************************************************/


sapdbwa_Bool wd23CreateHandlePool();

void wd23DestroyHandlePool();

sapdbwa_Handle wd23CreateHandle( twd40UserDllP			 userDll,
								 twd37WebSessionContP	 sessionCont,
								 sapdbwa_WebSessionP	 webSession,
								 char					*sessionId,
								 sapdbwa_Bool			 timedOut		);


void wd23DestroyHandle( sapdbwa_Handle wa );

twd26ErrP wd23GetErr( sapdbwa_Handle wa );

/*!**********************************************************************

  EndChapter: sapdbwa_Handle

************************************************************************/
#endif
