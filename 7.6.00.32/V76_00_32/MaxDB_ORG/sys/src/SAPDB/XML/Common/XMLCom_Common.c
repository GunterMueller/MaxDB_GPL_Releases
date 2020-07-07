/*!**********************************************************************

  module: XMLCom_Common.c

  -----------------------------------------------------------------------

  responsible:  Markus Oezgen

  special area: XML Common

  description:  Common functionality

  see also:

  -----------------------------------------------------------------------

                         Copyright (c) 2003-2005 SAP AG



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





************************************************************************/

/****************************************************************************
 ***
 *** Include
 ***
 ****************************************************************************/

#include "SAPDB/XML/Common/XMLCom_Common.h"

/*---------------------------------------------------------------------------*/

/****************************************************************************
 ***
 *** Defines
 ***
 ****************************************************************************/

/****************************************************************************
 ***
 *** Types & Structs
 ***
 ****************************************************************************/

/****************************************************************************
 ***
 *** Prototypes
 ***
 ****************************************************************************/

/****************************************************************************
 ***
 *** Variables
 ***
 ****************************************************************************/

/***********************************************************************

	Public Functions

 ***********************************************************************/

void Com_StrMaxCopy( char    *destination,
                     char    *source,
                     size_t   sizeToCopy )
{

    strncpy(destination, source, sizeToCopy);
    destination[sizeToCopy] = '\0';

}

/*===========================================================================*/

/***********************************************************************

	Private Functions

 ***********************************************************************/

/***********************************************************************

	End

 ***********************************************************************/
