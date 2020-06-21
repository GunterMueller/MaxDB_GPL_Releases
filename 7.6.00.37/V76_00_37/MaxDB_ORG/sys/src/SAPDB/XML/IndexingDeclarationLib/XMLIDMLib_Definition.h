	
	/*===================================================================
	/ Includes
\if EMIT_LICENCE


    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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


\endif
	/====================================================================*/
	#include "gwd00.h"
	#include "sapdbwa.h" /* nocheck */

	#if defined(__cplusplus)
	extern "C"	{
	#endif
		SAPDB_Bool XMLIDMLIB_doServiceEmbedded(sapdbwa_Handle    wa,
								  		  	   sapdbwa_HttpRequestP req,
								 		  	   sapdbwa_HttpReplyP   rep,
											   XMLIMAPI_Handle	xml);
	#if defined(__cplusplus)
	}
	#endif