/*!
  @file           vos80.c
  @author         RaymondR
  @brief          getopt
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

#include    <stdarg.h>


//
//  DEFINES
//

#define MOD__  "VOS80C : "
#define MF__   MOD__"UNDEFINED"



//
//  MACROS
//


//
//  LOCAL TYPE AND STRUCT DEFINITIONS
//


//
//  EXPORTED VARIABLES
//

int     sql80_OptInd  = 1;
int     sql80_OptErr  = 0;
char*   sql80_OptArg  = NULL;


//
//  LOCAL VARIABLES
//


//
//  LOCAL FUNCTION PROTOTYPES
//


//
// ========================== GLOBAL FUNCTIONS ================================
//


int sql80_GetOpt ( int     nArgc,
                   char**  pszArgv,
                   char*   pszOptionString )
  {
  #undef  MF__
  #define MF__ MOD__"sql80_GetOpt"


  static int    Cnt = 1;
  char          OptChar;
  char          *pOptChar;

  if(Cnt == 1)
    {
    if ((sql80_OptInd >= nArgc)             ||
        (pszArgv[sql80_OptInd][0] != '-' &&
         pszArgv[sql80_OptInd][0] != '/')   ||
        (pszArgv[sql80_OptInd][1] == '\0'))
      {
      return(-1);
      }

    if(strcmp(pszArgv[sql80_OptInd], "--") == 0)
      {
      sql80_OptInd++;
      return(-1);
      }
    }

  OptChar = pszArgv[sql80_OptInd][Cnt];

  if(OptChar == ':' || (pOptChar=strchr(pszOptionString, OptChar)) == NULL)
    {
    if(pszArgv[sql80_OptInd][++Cnt] == '\0')
      {
      sql80_OptInd++;
      Cnt = 1;
      }
    return(( int )'?');
    }

  if(*++pOptChar == ':')
    {
    if(pszArgv[sql80_OptInd][Cnt + 1] != '\0')
      sql80_OptArg = &pszArgv[sql80_OptInd++][Cnt + 1];
    else if(++sql80_OptInd >= nArgc)
      {
      Cnt = 1;
      return(( int )'?');
      }
    else
      sql80_OptArg = pszArgv[sql80_OptInd++];

    Cnt = 1;
    }
  else
    {
    if(pszArgv[sql80_OptInd][++Cnt] == '\0')
      {
      Cnt = 1;
      sql80_OptInd++;
      }
    sql80_OptArg = NULL;
    }

  DBGOUT;
  return(OptChar);
  }


//
// ========================== LOCAL FUNCTIONS =================================
//

//
// =============================== END ========================================
//
