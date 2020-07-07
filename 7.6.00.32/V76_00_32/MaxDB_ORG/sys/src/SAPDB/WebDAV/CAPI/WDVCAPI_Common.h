/*!**********************************************************************

  module: WDVCAPI_Common.h

  -----------------------------------------------------------------------

  responsible:  Markus Oezgen

  special area: WebDAV

  description:  Common functions

  see also:

  -----------------------------------------------------------------------

						Copyright (c) 2000-2005 SAP AG



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

#ifndef WDVCAPI_COMMON_H
#define WDVCAPI_COMMON_H

/***********************************************************************

	Includes

 ***********************************************************************/

#include <WDVCAPI_Definition.h>

#include "SAPDB/WebDAV/CAPI/WDVCAPI_ErrorInternal.h"

/***********************************************************************

	Defines

 ***********************************************************************/

/* Tracing */
#define TRACE_LEVEL_OFF 0
#define TRACE_LEVEL_1   1

#define WDVCAPI_TRACE_LEVEL TRACE_LEVEL_OFF

#if WDVCAPI_TRACE_LEVEL==TRACE_LEVEL_1
    #define TRACE_OUT(v)                printf(v)
    #define TRACE_OUT_ARG1(v,a1)        printf(v, a1)
    #define TRACE_OUT_ARG2(v,a1,a2)     printf(v, a1, a2)
    #define TRACE_OUT_ARG3(v,a1,a2,a3)  printf(v, a1, a2, a3)
#else
    #define TRACE_OUT(v)
    #define TRACE_OUT_ARG1(v,a1)
    #define TRACE_OUT_ARG2(v,a1,a2)
    #define TRACE_OUT_ARG3(v,a1,a2,a3)
#endif

/***********************************************************************

	Types

 ***********************************************************************/

typedef WDV_UInt4   WDVCAPI_NameSpaceShortcutCounter;

/***********************************************************************

	Prototypes

 ***********************************************************************/

WDV_Bool SplitURI( WDVCAPI_WDV          wdv,
                   WDVCAPI_URI          uri,
				   WDVCAPI_URI          parent,
				   WDVCAPI_ResourceName child );

WDV_Bool Common_IsUriChild( WDVCAPI_WDV wdv,
                            WDVCAPI_URI srcUri,
                            WDVCAPI_URI destUri );

void Common_StrMaxCopy( char    *destination,
                        char    *source,
                        size_t   sizeToCopy );

/***********************************************************************

	End

 ***********************************************************************/

#endif
