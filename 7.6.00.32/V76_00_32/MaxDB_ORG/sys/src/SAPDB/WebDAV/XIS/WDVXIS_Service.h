/*!**********************************************************************

  module: WDVXIS_Service.h

  -----------------------------------------------------------------------

  responsible:	Markus Oezgen

  special area: WebDAV XML Indexing Service

  description:  Service

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

#ifndef WDVXIS_SERVICE_H
#define WDVXIS_SERVICE_H

/***********************************************************************

	Includes

 ***********************************************************************/

#include "SAPDB/SAPDBCommon/SAPDB_Types.h"

#include "SAPDB/XML/Error/XMLError_Error.h"
#include "SAPDB/XML/SessionPool/XMLSP_SessionPool.h"
#include "SAPDB/XML/Id/XMLId_Id.h"
#include "SAPDB/XML/QueryServerLib/XMLQSLib_Interface.h"
#include "SAPDB/XML/QueryServerLib/XMLQSLib_Query.h"

/*---------------------------------------------------------------------------*/

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

SAPDB_Bool SAPDBXML_Init( void            **userData,
                          XMLError_Error    hError );

SAPDB_Bool SAPDBXML_Exit( void    *userData );

SAPDB_Bool SAPDBXML_SetIndexValue( XMLSP_Session   hSessionIndexStore,
                                   XMLId_Id        resourceId,
                                   char           *indexNameSpace,
                                   char           *indexName,
                                   char           *indexValue,
                                   XMLError_Error  hError );

SAPDB_Bool SAPDBXML_RetrieveDocuments( SQLHDBC                    hDBCDocStore,
                                       SQLHENV                    hEnvDocStore,
                                       XMLQSLib_WhereClause       hWhereClause,
                                       void                     **userData,
                                       XMLQSLib_DestroyUserData  *funcDestroyUserData,
                                       XMLError_Error             hError );

SAPDB_Bool SAPDBXML_GetRetrievedDocument( SQLHDBC          hDBCDocStore,
                                          SQLHENV          hEnvDocStore,
                                          void            *userData,
                                          char            *docURI,
                                          SAPDB_UInt       docURILen,
                                          XMLError_Error   hError );

/***********************************************************************

	End

 ***********************************************************************/

#endif
