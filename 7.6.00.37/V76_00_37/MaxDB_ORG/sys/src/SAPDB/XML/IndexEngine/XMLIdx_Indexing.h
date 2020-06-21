/*!**********************************************************************

  module: XMLIdx_Indexing.h

  -----------------------------------------------------------------------

  responsible:	Markus Oezgen

  special area: XML Index Engine

  description:  XML Indexing

  see also:

  -----------------------------------------------------------------------

                         Copyright (c) 2001-2005 SAP AG



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





************************************************************************/

#ifndef XMLIDX_INDEXING_H
#define XMLIDX_INDEXING_H

/***********************************************************************

	Includes

 ***********************************************************************/

#include "SAPDB/XML/IndexEngine/XMLIdx_Workers.h"
#include "SAPDB/XML/Error/XMLError_Error.h"

/*---------------------------------------------------------------------------*/

#include "SAPDB/XML/XPathEngine/XMLXPath_Interface.h"
#include "SAPDB/XML/XPathEngine/XMLXPath_Indexing.h"

/***********************************************************************

	Defines

 ***********************************************************************/

/***********************************************************************

	Structs & Types

 ***********************************************************************/

/*---------------------------------------------------------------------------*/

/***********************************************************************

	Prototypes

 ***********************************************************************/

SAPDB_Bool Indexing_Process( XMLIdx_Work    hWork,
                             XMLError_Error hError );

XMLXPath_Idx_RetCode Indexing_SetIndexValue( void             *parserUserData,
                                             void	          *indexUserData,
                                             const SAPDB_UTF8 *indexValue );

/***********************************************************************

	End

 ***********************************************************************/

#endif
