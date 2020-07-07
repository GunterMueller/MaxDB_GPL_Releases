/*!**********************************************************************

  module: XMLCom_Common.c

  -----------------------------------------------------------------------

  responsible:  Markus Oezgen

  special area: XML Common

  description:  Common defines, types, ...

  see also:

  -----------------------------------------------------------------------

                         Copyright (c) 2000-2005 SAP AG




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




************************************************************************/

#ifndef XMLCOM_COMMON_H
#define XMLCOM_COMMON_H

/***********************************************************************

	Includes

 ***********************************************************************/

#ifdef WIN32
    #include <windows.h>
#endif

#include <string.h>

/*---------------------------------------------------------------------------*/

/***********************************************************************

	Defines

 ***********************************************************************/

/***********************************************************************

	Structs & Types

 ***********************************************************************/

#define XMLCOM_MAX_FILENAME_LEN     1024

/*---------------------------------------------------------------------------*/

#ifdef WIN32
#define XMLCOM_DIRPATH_SEP  '\\'
#else
#define XMLCOM_DIRPATH_SEP	'/'
#endif

#define XMLCOM_SERVICE_INTERNAL_NAME        "SAPDBXIE"
#define XMLCOM_SERVICE_DISPLAY_NAME         "SAP DB XML Index Engine"

#define XMLCOM_THREAD_STACK_SIZE            1024*256

/***********************************************************************

	Prototypes

 ***********************************************************************/

void Com_StrMaxCopy( char    *destination,
                     char    *source,
                     size_t   sizeToCopy );

/*---------------------------------------------------------------------------*/

/***********************************************************************

	End

 ***********************************************************************/

#endif
