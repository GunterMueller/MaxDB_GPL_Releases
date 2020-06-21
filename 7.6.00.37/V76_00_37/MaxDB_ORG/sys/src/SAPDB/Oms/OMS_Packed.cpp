/*


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



*/
#include "Oms/OMS_Packed.hpp"
#include "Oms/OMS_DbpError.hpp"
#include "Oms/OMS_Globals.hpp"
#include "ggg00.h"
#include "ggg01.h" // OMS errors

#if defined(WIN32)

#define MIN_PACKED -999999999999999I64
#define MAX_PACKED  999999999999999I64

static OmsTypeInt64 neg64_limits[] = {
  -1000000000000000000I64     /* 1 */
  ,-100000000000000000I64     /* 2 */
  , -10000000000000000I64     /* 3 */
  ,  -1000000000000000I64     /* 4 */
  ,   -100000000000000I64     /* 5 */
  ,    -10000000000000I64     /* 6 */
  ,     -1000000000000I64     /* 7 */
  ,      -100000000000I64     /* 8 */
  ,       -10000000000I64     /* 9 */
  ,        -1000000000I64     /*10 */
  ,         -100000000I64     /*11 */
  ,          -10000000I64     /*12 */
  ,           -1000000I64     /*13 */
  ,            -100000I64     /*14 */ 
  ,             -10000I64     /*15 */
  ,              -1000I64     /*16 */
  ,               -100I64     /*17 */
  ,                -10I64     /*18 */
  ,                 -1I64     /*19 */            
};   

#else

#define MIN_PACKED -999999999999999LL
#define MAX_PACKED  999999999999999LL

static OmsTypeInt64 neg64_limits[] = {
  -1000000000000000000LL     /* 1 */
  ,-100000000000000000LL     /* 2 */
  , -10000000000000000LL     /* 3 */
  ,  -1000000000000000LL     /* 4 */
  ,   -100000000000000LL     /* 5 */
  ,    -10000000000000LL     /* 6 */
  ,     -1000000000000LL     /* 7 */
  ,      -100000000000LL     /* 8 */
  ,       -10000000000LL     /* 9 */
  ,        -1000000000LL     /*10 */
  ,         -100000000LL     /*11 */
  ,          -10000000LL     /*12 */
  ,           -1000000LL     /*13 */
  ,            -100000LL     /*14 */ 
  ,             -10000LL     /*15 */
  ,              -1000LL     /*16 */
  ,               -100LL     /*17 */
  ,                -10LL     /*18 */
  ,                 -1LL     /*19 */            
};   

#endif

/* ================================================================================ */
/* ==========                 DECLARATION OF FUNCTIONS                    ========= */
/* ================================================================================ */

/* ================================================================================ */
/* ==========                 DEFINITION OF GLOBAL FUNCTIONS              ========= */
/* ================================================================================ */

#if defined(WIN32)

OmsTypeInt64 OmsPacked::toInt64_16()
{
  OmsTypeInt64 result;
  result = ((((((((((((((((((((((((((((( digit15()) * 10I64 )
    + digit14()) * 10I64 )
    + digit13()) * 10I64 )
    + digit12()) * 10I64 )
    + digit11()) * 10I64 )
    + digit10()) * 10I64 ) 
    + digit9() ) * 10I64 )
    + digit8() ) * 10I64 )
    + digit7() ) * 10I64 )
    + digit6() ) * 10I64 )
    + digit5() ) * 10I64 )
    + digit4() ) * 10I64 )
    + digit3() ) * 10I64 )
    + digit2() ) * 10I64 )
    + digit1() );
  if (BCDMINUS == sign()) {
    return -(result); /* PTS 1103949 */
  }
  else {
    return result; /* PTS 1103949 */
  }
}

#else

OmsTypeInt64 OmsPacked::toInt64_16()
{
  OmsTypeInt64 result;
  result = 
     digit1() + 
    ( digit2()   * 10LL ) +
    ( digit3()   * 100LL ) +
    ( digit4()   * 1000LL ) +
    ( digit5()   * 10000LL ) +
    ( digit6()   * 100000LL ) +
    ( digit7()   * 1000000LL ) +
    ( digit8()   * 10000000LL ) +
    ( digit9()   * 100000000LL ) +
    ( digit10()  * 1000000000LL ) +
    ( digit11()  * 10000000000LL ) +
    ( digit12()  * 100000000000LL ) +
    ( digit13()  * 1000000000000LL ) +
    ( digit14()  * 10000000000000LL ) +
    ( digit15()  * 100000000000000LL );

  if (BCDMINUS == sign()) {
    return -(result); /* PTS 1103949 */
  }
  else {
    return result; /* PTS 1103949 */
  }
}

#endif

/*----------------------------------------------------------------------*/

void OmsPacked::toPacked(OmsTypeInt64 val, int packedBytes)
{
  unsigned char * ptr;
  char            sign ;
  int             digit ;
  int             cnt;
  int             digits;
  OmsTypeInt64 *  limitptr ;
  
  if ((val < MIN_PACKED) || (val > MAX_PACKED))
  {
     OMS_Globals::Throw(e_packed_out_of_range, "OmsPacked::toPacked", __MY_FILE__, __LINE__);
  }
  
  /*-------------------------------------------------------------------*/
  /* Remember the sign.                                                */
  /*-------------------------------------------------------------------*/
  digits = packedBytes * 2 - 1;
  if ( val < 0 ) {
    sign = BCDMINUS ;
  }
  else {
    val = -val ;
    sign = BCDPLUS ;
  }

  /*-------------------------------------------------------------------*/
  /* Initialize the conversion loop.                                   */
  /*-------------------------------------------------------------------*/
  limitptr = &neg64_limits[19 - digits];
  ptr      = (unsigned char *) this;
  for ( cnt = digits; cnt > 0; cnt -- , limitptr++ ) {
    /*----------------------------------------------------------------*/
    /* The algorithm repeatingly subtracts the decimal value of the   */
    /* digit position until an underflow occurs and repairs the last  */
    /* step. If the digit position value is to big anyway, this sub-  */
    /* tract loop is skipped rightaway and the digit set to zero.     */
    /*----------------------------------------------------------------*/
    if ( val <= *limitptr ) {
      digit = (-1) ;
      do {
        val -= *limitptr ;
        ++digit ;
      }  while ( val <= 0 ) ;
      
      val += *limitptr ;
    }
    else digit = 0;
    /*----------------------------------------------------------------*/
    /* If the loop counter is odd, we will store the digit in the     */
    /* left half of the BCD number otherwise the right half and       */
    /* advance the byte pointer into the BCD number.                  */
    /*----------------------------------------------------------------*/
    if ( cnt & 1 ) {
      *ptr = digit << 4 ;
    }
    else {
      *ptr++ |= digit ;
    }
  } /* for */
  /*-------------------------------------------------------------------*/
  /* Finally, store the sign and return.                               */
  /*-------------------------------------------------------------------*/
  *ptr |= sign ;
}

