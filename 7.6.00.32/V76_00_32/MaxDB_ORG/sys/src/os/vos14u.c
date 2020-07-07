/*!
  @file           vos14u.c
  @author         RaymondR
  @brief          SQLTERMID
  @see            

\if EMIT_LICENCE

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



\endif
*/




//
//  INCLUDE FILES
//
#include "gos00.h"
#include "heo46.h"
#include "geo43_0.h"


//
//  DEFINES
//

#define MOD__  "VOS14UC : "
#define MF__   MOD__"UNDEFINED"

#define TERMID                  C18


//
//  MACROS
//


//
//  LOCAL TYPE AND STRUCT DEFINITIONS
//


//
//  EXTERNAL VARIABLES
//
externC SQL_NODEIDC                szLocalNodeName;   // --- VEN03UC


//
//  EXPORTED VARIABLES
//


//
//  LOCAL VARIABLES
//


//
//  LOCAL FUNCTION PROTOTYPES
//


//
// ========================== GLOBAL FUNCTIONS ================================
//

VOID    sqltermid   (   tsp00_TermId acTerminalId    )
  {
  #undef  MF__
  #define MF__ MOD__"sqltermid"

  CHAR         szPid[10];
  CHAR         szTermid[sizeof (tsp00_TermId) + 1];
  PID          pid;
  SQL_NODEIDC  szHostName;
  PSZ          pszNode;
  LONG         rc;

  DBGPAS;

  memset ( szTermid, 0, sizeof (szTermid));

  GETPROCESSID(&pid);
  _ultoa ( pid, szPid, 16 );
  strupr ( szPid );

  if ( szLocalNodeName[0] == '\0' )
    {
    rc = sql43_get_my_host_name ( szHostName, sizeof(szHostName) );

    if ( rc == NO_ERROR )
      pszNode = szHostName;
    else
      pszNode = NULL;
    }
  else
    pszNode = szLocalNodeName;

  if ( pszNode == NULL )
    {
    strcpy ( szTermid, "PID:" );
    }
  else if ( strlen (pszNode) + strlen (szPid)  > sizeof (tsp00_TermId) - 1)
    {
    strncpy ( szTermid, pszNode, sizeof (tsp00_TermId) - strlen (szPid) - 2 );
    strcat  ( szTermid, "..");
    }
  else
    {
    strcpy ( szTermid, pszNode );
    strcat ( szTermid, " ");
    }

  strcat ( szTermid, szPid);

  eo46CtoP ( acTerminalId, szTermid, sizeof ( tsp00_TermId ));
  }

// ----------------------------------------------------------------------------


//
// ========================== LOCAL FUNCTIONS =================================
//


//
// =============================== END ========================================
//
