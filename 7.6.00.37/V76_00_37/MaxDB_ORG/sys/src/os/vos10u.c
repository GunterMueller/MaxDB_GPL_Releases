/*!
  @file           vos10u.c
  @author         RaymondR
  @brief          SQLREAD/SQLWRITE
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
// INCLUDE FILES
//
#include "gos00.h"
#include "heo46.h"


//
//  DEFINES
//

#define MOD__  "VOS10UC : "
#define MF__   MOD__"UNDEFINED"


//
//  MACROS
//


//
//  LOCAL TYPE AND STRUCT DEFINITIONS
//


//
//  EXTERNAL VARIABLES
//


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
// ========================== LOCAL FUNCTIONS =================================
//


//
// ========================== GLOBAL FUNCTIONS ================================
//


VOID sqlread ( tsp00_Line  acLine, PBOOLEAN pbOk )
  {
  #undef  MF__
  #define MF__ MOD__"sqlread"
  APIRET  rc = NO_ERROR;
  ULONG   ulRead;
  CHAR    acLineC [ sizeof ( tsp00_Line ) + 1];

  #if defined(_WIN32)
   HANDLE  StdIn;
  #endif

  DBGIN;

  //
  // --- Read string from stdin
  //
  #if defined(_WIN32)
   StdIn = GetStdHandle(STD_INPUT_HANDLE);

   if ( StdIn == INVALID_HANDLE_VALUE )
     rc = GetLastError();
   else if ( !ReadFile( StdIn, acLineC, sizeof ( acLineC ), &ulRead, NULL ))
     rc = GetLastError();
  #else
   rc = DosRead ( STDIN, acLineC, sizeof ( acLineC ), &ulRead );
  #endif


  if ((rc != NO_ERROR) || ( ulRead == 0 ))
    {
    *pbOk = FALSE;
    memset ( acLine,  ' ', sizeof ( acLine  ));
    }
  else
    {
    *pbOk = TRUE;
    eo46CtoP ( acLine, acLineC, sizeof ( acLine ));
    }

  DBGOUT;
  return;
  }


// ----------------------------------------------------------------------------


VOID sqlwrite ( tsp00_Line acLine )
  {
  #undef  MF__
  #define MF__ MOD__"sqlwrite"


  ULONG   ulWritten;
  char    acLineC [sizeof(tsp00_Line) + 3];


  DBGIN;

  acLineC[0]                  = '\0';
  eo46PtoC ( acLineC, acLine, sizeof ( tsp00_Line ));
  strcat ( acLineC, "\r\n" );

  //
  // --- Write string to stdout
  //
  #if defined(_WIN32)
   WriteFile( GetStdHandle(STD_OUTPUT_HANDLE),
              acLineC, (ULONG)strlen ( acLineC ), &ulWritten, NULL );
  #else
   DosWrite ( STDOUT, acLineC, strlen ( acLineC ), &ulWritten );
  #endif


  DBGOUT;
  return;
  }


//
// ================================= END ======================================
//

