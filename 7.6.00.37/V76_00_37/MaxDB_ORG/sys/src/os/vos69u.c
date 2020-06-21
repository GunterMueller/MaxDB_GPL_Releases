/*!
  @file           vos69u.c
  @author         RaymondR
  @brief          accounting
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




/*
 * INCLUDE FILES
 */
#include "gos00.h"
#include "heo46.h"


/*
 *  DEFINES
 */
#define MOD__  "VOS69uc : "
#define MF__   MOD__"UNDEFINED"

/*
 *  MACROS
 */


/*
 *  LOCAL TYPE AND STRUCT DEFINITIONS
 */


/*
 * EXTERNAL VARIABLES
 */


/*
 *  EXPORTED VARIABLES
 */

static  PULONG            pulStackBegin;

/*
 * LOCAL VARIABLES
 */
static ULONG    ulStartTime;

/*
 * LOCAL FUNCTION PROTOTYPES
 */

/*
 * ========================== GLOBAL FUNCTIONS ================================
 */

VOID vcinit ( PROCESS_ID pid )
  {
  #undef  MF__
  #define MF__ MOD__"vcinit"

  DBGPAS;

  ulStartTime   = clock () / (CLOCKS_PER_SEC / 1000);

  return;
  }

/*------------------------------*/


VOID vccheck ( PROCESS_ID  pid,
               INT4       *cputime)
  {
  #undef  MF__
  #define MF__ MOD__"vccheck"
  DBGPAS;


  *cputime = (clock () / (CLOCKS_PER_SEC / 1000)) - ulStartTime;

  return;
  }

/*------------------------------*/

VOID vsinit ( VOID )
  {
  #undef  MF__
  #define MF__ MOD__"vsinit"
  ULONG      ulCurrStack;

  DBGPAS;

  pulStackBegin = &ulCurrStack;

  return;
  }

/*------------------------------*/

VOID  vscheck ( INT4 *stackused )
  {
  #undef  MF__
  #define MF__ MOD__"vscheck"
  ULONG      ulCurrStack;

  DBGPAS;

  *stackused = (ULONG)((tsp00_Longuint)pulStackBegin - (tsp00_Longuint) &ulCurrStack);

  return;
  }

/*
 * ========================== LOCAL FUNCTIONS =================================
 */


/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
