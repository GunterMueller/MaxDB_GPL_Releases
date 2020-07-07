/*!**********************************************************************

  module: XMLIdx_Engine.h

  -----------------------------------------------------------------------

  responsible:	Markus Oezgen

  special area: XML Index Engine

  description:  Engine

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

#ifndef XMLIDX_ENGINE_H
#define XMLIDX_ENGINE_H

/***********************************************************************

	Includes

 ***********************************************************************/

#include "SAPDB/XML/Common/XMLCom_Common.h"

/***********************************************************************

	Defines

 ***********************************************************************/

/***********************************************************************

	Structs & Types

 ***********************************************************************/

typedef struct st_xmlidx_service_description    *XMLIdx_ServiceDescription;

/*---------------------------------------------------------------------------*/

/***********************************************************************

	Prototypes

 ***********************************************************************/

SAPDB_Bool  Engine_Create( char *exeDir,
                           char *iniFile,
                           char *sectionName,
                           char *errorOutText );

SAPDB_Bool Engine_Destroy();

SAPDB_Bool Engine_Start();

/***********************************************************************

	End

 ***********************************************************************/

#endif
