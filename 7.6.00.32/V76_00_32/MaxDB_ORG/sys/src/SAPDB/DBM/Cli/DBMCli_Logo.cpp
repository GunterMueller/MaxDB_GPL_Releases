/*!
  @file           DBMCli_Logo.cpp
  @author         Bernd Vorsprach - bernd.vorsprach@sap.com
  @brief          database manager client logo printer

\if EMIT_LICENCE

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



\endif
*/

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include "DBM/Cli/DBMCli_Logo.hpp"

#define VSP100_CPP
#include "hsp100.h"

/*! @brief Print the logo */
void DBMCli_Logo :: PrintLogo 
  ( const DBMCli_String & sComponent,
          SAPDB_Bool      bPrint )
{

  if (bPrint) {
    const char * pDate = __DATE__;

    printf((const char *) sComponent);
    printf(", Version "MAJOR_VERSION_NO_STR_SP100"."MINOR_VERSION_NO_STR_SP100"."CORRECTION_LEVEL_STR_SP100"\n");
    printf("Copyright (c) 1998 - ");
    printf(&pDate[7]);
    printf(" by SAP AG\n\n"); 
  } // end if
} // DBMCli_Logo :: PrintLogo 

/*! @brief Print the version */
void DBMCli_Logo :: PrintVersion
  ( const DBMCli_String & sComponent,
          SAPDB_Bool      bPrint )
{
  if (bPrint) {
    tsp00_Versionc VersionString;
    sp100_GetVersionString  ( (const char *) sComponent, s100buildnumber, VersionString );
    printf("%s\n\n", VersionString.asCharp());
  } // end if
} // DBMCli_Logo :: PrintVersion

