/*!**********************************************************************

  module: hwd22rep.h

  -----------------------------------------------------------------------

  responsible:  Holger Bischoff
				Markus Oezgen

  special area: WebAgent

  description:  HttpReply functions

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

#ifndef HWD22REP_H
#define HWD22REP_H

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include "gwd00.h"
#include "hwd03waapi.h"
#include "hwd26err.h"
#include "hwd28dict.h"

#if defined(__cplusplus)
extern "C" {
#endif

/*!**********************************************************************

  Chapter: sapdbwa_HttpReplyP

  description:
  
************************************************************************/


sapdbwa_Bool wd22CreateReplyHandlePool();

void wd22DestroyReplyHandlePool();    
    
sapdbwa_HttpReplyP wd22CreateReply( twd00ServerHandle   handle,
                               twd00SendHeaderFunc SendHeader,
                               twd00SendBodyFunc   SendBody );

void wd22DestroyReply( sapdbwa_HttpReplyP rep );

sapdbwa_Int2  wd22GetStatus( sapdbwa_HttpReplyP rep );

twd00ServerHandle wd22GetServerHandle( sapdbwa_HttpReplyP rep );

char* wd22GetLocation( sapdbwa_HttpReplyP rep );

char* wd22GetLastModified( sapdbwa_HttpReplyP rep );

char* wd22GetContentLength( sapdbwa_HttpReplyP rep );

char* wd22GetContentType( sapdbwa_HttpReplyP rep );

char* wd22GetExpires( sapdbwa_HttpReplyP rep );

twd28DictionaryP wd22GetHeaders( sapdbwa_HttpReplyP rep );

void wd22SetErr( sapdbwa_HttpReplyP rep, twd26ErrP err );

    /* only used by testapi */
void wd22FreeHeader( sapdbwa_HttpReplyP rep );
    
/*!**********************************************************************

  EndChapter: sapdbwa_HttpReplyP

************************************************************************/
#if defined(__cplusplus)
}
#endif

#endif

