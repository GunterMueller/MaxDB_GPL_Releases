/* @lastChanged: "1998-10-22  9:23"

  * @filename:   ven36.cpp
  * @purpose:    "DCOM load dynamic library"
  * @release:    7.2.0.0
  * @see:        " "
  * @author:     Ferdinand Flohe (D024849 - Ferdinand.Flohe@SAP-AG.de)
  * @Copyright (c) 1998-2005 SAP AG"



    ========== licence begin  GPL
    Copyright (c) 1998-2005 SAP AG

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

/* ================================================================================ */
/* ==========              REPLACEMENTS BY THE PREPROCESSOR               ========= */
/* ================================================================================ */

#define MAXPARAMS_EN36 256

/* ========================================================================== */
/* ==========              DECLARATION OF TYPES                     ========= */
/* ========================================================================== */

#include <stdlib.h>
#include "heo38.h"
#include "vsp001.h"
#include "geo00d.h"
#include "heo78.h"
#include "heo02x.h"
#include "heo01.h"

#include "geo60.h"
#include "gen500.h"
#include "liveCache/LVC_LoadLibrary.hpp"

typedef char t_part_data[sizeof(tsp00_KnlIdentifier) + 2];
typedef tsp1_dcom_param_info *dcom_param_info_ptr;
typedef BSTR                  ParamNames_en36[MAXPARAMS_EN36];

#define ERROR_NO_STREAM ERROR_FILE_NOT_FOUND

/* ========================================================================== */
/* ==========              DECLARATION OF CONSTANTS                 ========= */
/* ========================================================================== */

/* ========================================================================== */
/* ==========               EXTERN FUNCTION PROTOTYPES              ========= */
/* ========================================================================== */


/* ========================================================================== */
/* ==========               LOCAL FUNCTION PROTOTYPES               ========= */
/* ========================================================================== */


static unsigned int hex2int( char x );
static HRESULT WINAPI CLSIDFromString(teo78_ClsID vStrClsID, LPCLSID pclsid);

/* ========================================================================== */
/* ==========              DEFINITION OF GLOBAL FUNCTIONS           ========= */
/* ========================================================================== */

EXTERN_C PROC WINAPI sql36GetProcAddress(
    HMODULE hModule,
    LPCSTR lpProcName
    )
{
  tsp00_ErrText       ErrText;
  return ((PROC)(sqlGetProcAddress(hModule, (char*)lpProcName, ErrText, sizeof(tsp00_ErrText)-1)));
};

EXTERN_C BOOL WINAPI sql36FreeLibrary(
    HMODULE hLibModule
    )
{
  tsp00_ErrText       ErrText;
  return sqlFreeLibrary(hLibModule, ErrText, sizeof(tsp00_ErrText)-1);
};


static unsigned int hex2int( char x )
{
  unsigned int i = 0;
    if (('0' <= x) && ( x <= '9'))
      i = x - '0';
    if (('A' <= x) && ( x <= 'F'))
      i = (x - 'A') + 10;
    if (('a' <= x) && ( x <= 'f'))
      i = (x - 'a') + 10;
  return i;
}

static HRESULT WINAPI CLSIDFromString(teo78_ClsID vStrClsID, LPCLSID pclsid)
{
  int i;

  pclsid->Data1 = 0;
  for ( i=0; i<4; i++ ) 
  {
    pclsid->Data1 = ( pclsid->Data1 * 256) + ( hex2int( vStrClsID[(i*2)+1] ) * 16 ) +
      hex2int( vStrClsID[(i*2)+2] );
  }

  pclsid->Data2 = 0;
  for ( i=0; i<2; i++ ) 
  {
    pclsid->Data2 = ( pclsid->Data2 * 256) + ( hex2int( vStrClsID[(i*2)+10] ) * 16 ) +
      hex2int( vStrClsID[(i*2)+11] );
  }

  pclsid->Data3 = 0;
  for ( i=0; i<2; i++ ) 
  {
    pclsid->Data3 = ( pclsid->Data3 * 256) + ( hex2int( vStrClsID[(i*2)+15] ) * 16 ) +
      hex2int( vStrClsID[(i*2)+16] );
  }

  for ( i=0; i<2; i++ ) 
  {
    pclsid->Data4[i] = ( hex2int( vStrClsID[(i*2)+20] ) * 16 ) +
      hex2int( vStrClsID[(i*2)+21] );
  }
  
  for ( i=2; i<8; i++ ) 
  {
    pclsid->Data4[i] = ( hex2int( vStrClsID[(i*2)+21] ) * 16 ) + 
      hex2int( vStrClsID[(i*2)+22] );
  }
  return 0;
};

