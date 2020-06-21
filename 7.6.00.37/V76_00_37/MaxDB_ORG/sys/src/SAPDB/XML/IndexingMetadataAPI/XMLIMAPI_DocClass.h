#ifndef XMLIMAPI_DOCCLASS_H
#define XMLIMAPI_DOCCLASS_H
/****************************************************************************

  module      : XMLIMAPI_DocClass.h

  -------------------------------------------------------------------------

  responsible : Daniel Kirmse

  special area: 
  description : 


  last changed: 2001-08-22  9:44
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

	#include <XMLIMAPI_Definition.h>
	#include "SAPDB/XML/IndexingMetadataAPI/XMLIMAPI_CommonInclude.h"
	#include "SAPDB/XML/IndexingMetadataAPI/XMLIMAPI_Error.h"

	/*------------------------------------------------------------------------
		Function:	docClassAssignXmlIndex
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool docClassAssignXmlIndex(XMLIMAPI_Handle		handle,
										  XMLIMAPI_Id			docClassId,
										  XMLIMAPI_XmlIndexList	assigned);

	/*------------------------------------------------------------------------
		Function:	deleteAssignedXmlIndices
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool deleteAssignedXmlIndices(XMLIMAPI_Handle    handle,
											XMLIMAPI_Id		docClassId);

	/*------------------------------------------------------------------------
		Function:	insertAssignedXmlIndices
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool insertAssignedXmlIndices(XMLIMAPI_Handle    handle,
											XMLIMAPI_Id		docClassId,
											XMLIMAPI_XmlIndexList	list);


#endif