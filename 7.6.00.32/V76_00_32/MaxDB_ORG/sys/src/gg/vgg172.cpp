/* @lastChanged: "2004-07-19"

  * @filename:   vgg172.cpp
  * @purpose:    "GG_edit_routines_2"
  * @release:    7.6.0.0
  * @see:        ""
  *
  * @Copyright (c) 1999-2005 SAP AG"


    ========== licence begin  GPL
    Copyright (c) 1999-2005 SAP AG

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

#include "SAPDBCommon/SAPDB_Types.h"
#include "SAPDBCommon/SAPDB_ToString.hpp"
#include "SAPDB/SAPDBCommon/SAPDB_RangeCode.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"
#include "gbd00.h"
#include "hgg172.h"

/* ========================================================================== */
/* ==========              DECLARATION OF TYPES                     ========= */
/* ========================================================================== */

static  void gg172_UInt8_to_line (SAPDB_UInt8           uint8,
                                  integer               &ln_pos,
                                  char*                 &ln)
{
    int i = 0;
    SAPDB_ToStringClass  convertedNumber (uint8, 20);
    tgg00_BasisError moveError = e_ok;
    while ( *((SAPDB_Char*)convertedNumber + i) == ' ' )
        i++;
    SAPDB_RangeMove (__FILE__, 8, 20, 20,
        (SAPDB_Char*)convertedNumber, i + 1, ln + ln_pos, 1,
        convertedNumber.Length() - i, moveError);
    ln_pos = ln_pos + convertedNumber.Length() - i;
}

/*-----------------------------------------------------------------------------------*/
static  void gg172_Int8_to_line (SAPDB_Int8             int8,
                                 integer               &ln_pos,
                                 char*                 &ln)
{
    int i = 0;
    SAPDB_ToStringClass  convertedNumber (int8, 20);
    tgg00_BasisError moveError = e_ok;
    while ( *((SAPDB_Char*)convertedNumber + i) == ' ' )
        i++;
    SAPDB_RangeMove (__FILE__, 8, 20, 20,
        (SAPDB_Char*)convertedNumber, i + 1, ln + ln_pos, 1,
        convertedNumber.Length() - i, moveError);
    ln_pos = ln_pos + convertedNumber.Length() - i;
}

/*-----------------------------------------------------------------------------------*/
externPascal void g172fileno_to_line (
    tsp00_C8              VAR_ARRAY_REF  fileno,
    integer               VAR_VALUE_REF  ln_pos,
    char*                                ln)
{
    FileDir_FileNo   fno = fileno;
    gg172_UInt8_to_line(fno.GetUInt8(), ln_pos, ln);
}

/*-----------------------------------------------------------------------------------*/
externPascal void g172Char8AsUint8_to_line (
    tsp00_C8              VAR_ARRAY_REF  Char8,
    integer               VAR_VALUE_REF  ln_pos,
    char*                                ln)
{
    gg172_UInt8_to_line(*reinterpret_cast<SAPDB_UInt8*>(&Char8[0]), ln_pos, ln);
}

/*-----------------------------------------------------------------------------------*/

externPascal void g172Char8AsInt8_to_line (
    tsp00_C8              VAR_ARRAY_REF  Char8,
    integer               VAR_VALUE_REF  ln_pos,
    char*                                ln)
{
    gg172_Int8_to_line(*reinterpret_cast<SAPDB_Int8*>(&Char8[0]), ln_pos, ln);
}

/*-----------------------------------------------------------------------------------*/

#define MAX_UINT4   STATIC_CAST(teo00_Uint8,STATIC_CAST(teo00_Uint4,-1))
#define MAX_UINT8   STATIC_CAST(teo00_Uint8,-1)

externPascal void g172Sequence_to_line (
    tbd00_LockRequestSeqNo               high,
    tbd00_LockRequestSeqNo               low,
    integer               VAR_VALUE_REF  ln_pos,
    char*                                ln)
{
    const SAPDB_Char* maxLabel = "MAX_UINT8";
    int i = 0;
    SAPDB_UInt8   seq = 
        ((STATIC_CAST(teo00_Uint4,high) * (MAX_UINT4 + 1)) + 
        STATIC_CAST(teo00_Uint4,low));

    tgg00_BasisError moveError = e_ok;
    if ( seq == MAX_UINT8 )
    {
        SAPDB_RangeMove (__FILE__, 8, strlen(maxLabel), strlen(maxLabel),
            maxLabel, 1, ln + ln_pos, 1,
            strlen(maxLabel), moveError);
        ln_pos = ln_pos + strlen(maxLabel);
    }
    else
        gg172_UInt8_to_line(seq, ln_pos, ln);
};

// end of program
