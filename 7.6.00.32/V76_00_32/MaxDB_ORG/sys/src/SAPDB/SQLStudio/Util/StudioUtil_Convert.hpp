  /*!***************************************************************************

  module:       StudioUtil_Convert.hpp

  ------------------------------------------------------------------------------

  responsible:  MarkusS

  special area: SQL Studio

  description:  functions to convert between different char formats

  see also:     

  ------------------------------------------------------------------------------

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




  *****************************************************************************/

#ifndef STUDIOUTIL_CONVERT_H
#define STUDIOUTIL_CONVERT_H


#include "hsp78_0.h"
#include "SAPDBCommon/SAPDB_Types.hpp"

#if defined(WIN32)

SAPDB_Bool __stdcall convertVB_UCS2ToUTF8 (const SAPDB_UCS2 *srcUCS2,
										SAPDB_UInt4 srcUCS2Len,
										SAPDB_UTF8 *destUTF8,
										SAPDB_UInt4 destUTF8Len,
										SAPDB_UInt4 *destUsed);




SAPDB_Bool __stdcall convertUTF8ToVB_UCS2 (const SAPDB_UTF8 *srcUTF8,
											SAPDB_UInt4 srcUTF8Len,
											SAPDB_UCS2 *destUCS2,
											SAPDB_UInt4 destUCS2Len,
											SAPDB_UInt4 *destUsed);
#else

SAPDB_Bool convertVB_UCS2ToUTF8 (const SAPDB_UCS2 *srcUCS2,
										SAPDB_UInt4 srcUCS2Len,
										SAPDB_UTF8 *destUTF8,
										SAPDB_UInt4 destUTF8Len,
										SAPDB_UInt4 *destUsed);




SAPDB_Bool convertUTF8ToVB_UCS2 (const SAPDB_UTF8 *srcUTF8,
											SAPDB_UInt4 srcUTF8Len,
											SAPDB_UCS2 *destUCS2,
											SAPDB_UInt4 destUCS2Len,
											SAPDB_UInt4 *destUsed);
#endif

#endif
