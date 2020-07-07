#ifndef WDVHANDLER_COMMONINCLUDE_H
#define WDVHANDLER_COMMONINCLUDE_H

/****************************************************************************

  module      : WDVHandler_CommonInclude.h

  -------------------------------------------------------------------------

  responsible : Daniel Kirmse

  special area: 
  description : 


  last changed: 2001-03-01  9:35
  see also    : 

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2001-2005 SAP AG

    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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


*****************************************************************************/

	#define VSP100
	#define RESOURCE_FILE
	#include "gsp100.h"
	#include "hsp100.h"

	#include "gwd00.h"
	#include "heo102.h"
	#include "hsp77.h"
	#include "sapdbwa.h" /*nocheck*/
	#include "hwd03waapi.h"
	#include "SAPDB/WebDAV/CAPI/WDVCAPI_Definition.h"
	#include "SAPDB/XML/IndexingMetadataAPI/XMLIMAPI_Definition.h"
	#include "SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Definition.h"
	#include "SAPDB/WebDAV/Handler/WDVHandler_Types.h"
	#include "SAPDB/XML/QueryClientLib/XMLQCLib_Definition.h"
	#include <time.h>

/*  Profiling */
/*	#define PROFILING_OVERALL

	#define PROFILING_DETAIL  
*/

/*  Logging */
/*	#define LOG_LEVEL_1 */

/*  Memory checking */
/*	#define WDVHANDLER_MEMCHECK */

/*	Versioning */
/*  - Core (incl.: VERSION-CONTROL, CHECKIN, CHECKOUT, UNCHECKOUT, REPORT)*/
  #define VERSIONING_CORE 

/*  - Base Server Package */
/*  #define VERSIONING_BASE_SERVER */

/*  - Advanced Server Package */
/*  #define VERSIONING_ADV_SERVER */

/* - XQuery Extensions */
	#define XQUERY_EXTENSION

	#include "SAPDB/WebDAV/Handler/WDVHandler_Memory.h"
#endif