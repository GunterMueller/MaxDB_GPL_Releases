/*!**********************************************************************

  module: hwd11strseq.h

  -----------------------------------------------------------------------

  responsible:	Holger Bischoff
				Markus Oezgen

  special area: WebAgent

  description:  String sequence

  see also:

  -----------------------------------------------------------------------

  copyright:               Copyright (c) 1999-2005 SAP AG



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

#ifndef HWD11STRSEQ_H
#define HWD11STRSEQ_H

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include "gwd00.h"
#include "hwd03waapi.h"

SAPDB_Bool wd11ContainsString( sapdbwa_StringSeqP strSeq, const char *string );

void wd11FreeElems( sapdbwa_StringSeqP strSeq );

SAPDB_Bool wd11InsertString( sapdbwa_StringSeqP strSeq, const char *string );

#endif
