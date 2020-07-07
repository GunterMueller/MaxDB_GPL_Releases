/*!**********************************************************************

  module: StudioUtil_Functions.hpp

  -----------------------------------------------------------------------

  responsible:  WolfgangA

  special area: SQLStudoUtilities

  description:  Utility functions for SQLStudio

  see also:		/

  -----------------------------------------------------------------------

  copyright:                Copyright (c) 2000-2005 SAP AG




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




*/

#ifndef STUDIOUTIL_FUNCTIONS
#define STUDIOUTIL_FUNCTIONS

#include <stdio.h>
#include <string.h>
#include "SAPDBCommon/SAPDB_Types.hpp"

/*===========================================================================*/
/*                         S T R U C T U R E S                               */
/*===========================================================================*/
typedef struct _conv_data
{
  int      iSegmentSize;
  int      iCurrentPos;
  int      iLength;
  char    *sInString;
  char    *sOutString;

} CONV_DATA;


#endif

