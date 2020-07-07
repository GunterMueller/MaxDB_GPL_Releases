/*!**********************************************************************

  module: XMLIdx_Workers.h

  -----------------------------------------------------------------------

  responsible:	Markus Oezgen

  special area: XML Index Engine

  description:  Worker management

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

#ifndef XMLIDX_WORKER_H
#define XMLIDX_WORKER_H

/***********************************************************************

	Includes

 ***********************************************************************/

#include "SAPDB/SAPDBCommon/SAPDB_Types.h"

#include "SAPDB/XML/SessionPool/XMLSP_SessionPool.h"
#include "SAPDB/XML/IndexEngine/XMLIdx_Engine.h"
#include "SAPDB/XML/Library/XMLLib_Library.h"
#include "SAPDB/XML/DocClass/XMLDC_DocClass.h"
#include "SAPDB/XML/Error/XMLError_Error.h"

/***********************************************************************

	Defines

 ***********************************************************************/

/***********************************************************************

	Structs & Types

 ***********************************************************************/

typedef struct st_xmlidx_work       *XMLIdx_Work;

/*---------------------------------------------------------------------------*/

struct st_xmlidx_work {
    XMLSP_SessionPool   hSessionPoolDocStore;
    XMLSP_Session       hSessionDocStore;

    XMLLib_Function     funcSetIndexValue;

    XMLSP_SessionPool   hSessionPoolIndexStore;
    XMLSP_Session       hSessionIndexStore;

    XMLId_Id            docId;
    XMLId_Id            docClassId;

    XMLDC_DocClass      hDocClassList;

    XMLIdx_Work         next;

    SAPDB_Bool          stopWork;
};

/***********************************************************************

	Prototypes

 ***********************************************************************/

SAPDB_Bool Workers_Create( XMLError_Error hError );

SAPDB_Bool Workers_Start( SAPDB_UInt4 maxWorkers );

SAPDB_Bool Workers_Stop();

SAPDB_Bool Workers_ThreadAlive( SAPDB_UInt4   *threadCount );

SAPDB_Bool Workers_Resume( XMLError_Error  hError );

SAPDB_Bool Workers_CreateWork( XMLIdx_Work                  *hNewWork,
                               XMLSP_SessionPool             hSessionPoolIndexStore,
                               XMLSP_SessionPool             hSessionPoolDocStore,
                               XMLSP_Session                 hSessionDocStore,
                               XMLId_Id                      docId,
                               XMLDC_DocClass                docClassList,
                               XMLDC_DocClassId              docClassId,
                               XMLLib_Function               funcSetIndexValue,
                               XMLError_Error                hError );

SAPDB_Bool Workers_PutWorkIntoQueue( XMLIdx_Work    hWork,
                                     XMLError_Error hError );

/***********************************************************************

	End

 ***********************************************************************/

#endif
