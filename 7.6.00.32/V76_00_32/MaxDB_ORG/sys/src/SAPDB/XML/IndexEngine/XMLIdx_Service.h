/*!**********************************************************************

  module: XMLIdx_Service.h

  -----------------------------------------------------------------------

  responsible:	Markus Oezgen

  special area: XML Index Engine

  description:  Windows Service Control Manager support

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

#ifndef XMLIDX_SERVICE_H
#define XMLIDX_SERVICE_H

/***********************************************************************

	Includes

 ***********************************************************************/

#include "SAPDB/SAPDBCommon/SAPDB_Types.h"

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

int Service_Start( char         *execDir,
                   SAPDB_Bool    debug,
                   char         *iniFile,
                   char         *sectionName,
                   char         *errorText );

SAPDB_Bool Service_InstallAsService( char  *execDir,
                                     char  *registrationName );

SAPDB_Bool Service_RemoveAsService();

SAPDB_Bool Service_GetExecDirectory( char          *cmdArg,
									 char          *directory,
									 SAPDB_UInt2    size      );

/*===========================================================================*/

/***********************************************************************

	End

 ***********************************************************************/

#endif
