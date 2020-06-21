/*!**********************************************************************

  module: hwd41cpp.h

  -----------------------------------------------------------------------

  responsible:  Holger Bischoff
				Markus Oezgen

  special area: WebAgent

  description:  Calling init-/exit- and service-function in C++ UserDll

  see also:

  -----------------------------------------------------------------------

							Copyright (c) 1999-2005 SAP AG



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

#ifndef HWD41CPP_H
#define HWD41CPP_H

#include "heo02x.h" /* sqlPROC */

#include "hwd03waapi.h"
#include "hwd25log.h"

#if defined(__cplusplus)
extern "C" {
#endif

    sapdbwa_Bool wd41CallExitFunc( const char *name,
                                 twd25LogP   log,
                                 sqlPROC     funcPtr,
                                 sapdbwa_Handle   wa,
                                 sapdbwa_Int2 *ret );

    sapdbwa_Bool wd41CallInitFunc( const char *name,
                                 twd25LogP   log,
                                 sqlPROC     funcPtr,
                                 sapdbwa_Handle   wa,
                                 sapdbwa_Int2 *ret );

    
    sapdbwa_Bool wd41CallServiceFunc( const char     *name,
                                    twd25LogP       log,
                                    sqlPROC         funcPtr,
                                    sapdbwa_Handle       wa,
                                    sapdbwa_HttpRequestP req,
                                    sapdbwa_HttpReplyP   rep,
                                    sapdbwa_Int2     *ret );

    
    sapdbwa_Bool wd41CallCppExitFunc( const char *name,
                                    twd25LogP   log,
                                    sqlPROC     funcPtr,
                                    sapdbwa_Handle   wa,
                                    sapdbwa_Int2 *ret );

    
    sapdbwa_Bool wd41CallCppInitFunc( const char *name,
                                    twd25LogP   log,
                                    sqlPROC     funcPtr,
                                    sapdbwa_Handle   wa,
                                    sapdbwa_Int2 *ret );

    
    sapdbwa_Bool wd41CallCppServiceFunc( const char     *name,
                                       twd25LogP       log,
                                       sqlPROC         funcPtr,
                                       sapdbwa_Handle       wa,
                                       sapdbwa_HttpRequestP req,
                                       sapdbwa_HttpReplyP   rep,
                                       sapdbwa_Int2     *ret );

    
    sapdbwa_Bool wd41CallSessionDataDestr( const char *userDllName,
                                         twd25LogP   log,
                                         sqlPROC     funcPtr,
                                         void       *data,
                                         sapdbwa_Handle   wa );
                                      
    sapdbwa_Bool wd41CallCppSessionDataDestr( const char *userDllName,
                                            twd25LogP   log,
                                            sqlPROC     funcPtr,
                                            void       *data,
                                            sapdbwa_Handle   wa );
                                      
                                           
#if defined(__cplusplus)
} /* extern "C" */
#endif

#endif
