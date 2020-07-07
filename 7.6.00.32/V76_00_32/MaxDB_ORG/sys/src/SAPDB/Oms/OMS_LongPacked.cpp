/*!
 * \file    OMS_LongPacked.cpp
 * \author  PeterG (GoertzP)
 * \brief   OMS: Long Packed
 */

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


/************************************************************************/
/* Includes                                                             */
/************************************************************************/

#include "Oms/OMS_LongPacked.hpp"
#include "Oms/OMS_DbpError.hpp"
#include "Oms/OMS_Globals.hpp"
#include "ggg00.h"
#include "ggg01.h" /* OMS errors */


/************************************************************************/
/* Constants                                                            */
/************************************************************************/

#if defined(WIN32)

#define MAX_LONGPACKED_HI 99999999999I64
#define MAX_LONGPACKED_LO 999999999999999999I64

static OmsTypeInt64 neg64_limits[] = {
  -1000000000000000000I64     /*  1 */
  ,-100000000000000000I64     /*  2 */
  , -10000000000000000I64     /*  3 */
  ,  -1000000000000000I64     /*  4 */
  ,   -100000000000000I64     /*  5 */
  ,    -10000000000000I64     /*  6 */
  ,     -1000000000000I64     /*  7 */
  ,      -100000000000I64     /*  8 */
  ,       -10000000000I64     /*  9 */
  ,        -1000000000I64     /* 10 */
  ,         -100000000I64     /* 11 */
  ,          -10000000I64     /* 12 */
  ,           -1000000I64     /* 13 */
  ,            -100000I64     /* 14 */ 
  ,             -10000I64     /* 15 */
  ,              -1000I64     /* 16 */
  ,               -100I64     /* 17 */
  ,                -10I64     /* 18 */
  ,                 -1I64     /* 19 */            
};   

#else

#define MAX_LONGPACKED_HI 99999999999LL
#define MAX_LONGPACKED_LO 999999999999999999LL

static OmsTypeInt64 neg64_limits[] = {
  -1000000000000000000LL      /*  1 */
  ,-100000000000000000LL      /*  2 */
  , -10000000000000000LL      /*  3 */
  ,  -1000000000000000LL      /*  4 */
  ,   -100000000000000LL      /*  5 */
  ,    -10000000000000LL      /*  6 */
  ,     -1000000000000LL      /*  7 */
  ,      -100000000000LL      /*  8 */
  ,       -10000000000LL      /*  9 */
  ,        -1000000000LL      /* 10 */
  ,         -100000000LL      /* 11 */
  ,          -10000000LL      /* 12 */
  ,           -1000000LL      /* 13 */
  ,            -100000LL      /* 14 */ 
  ,             -10000LL      /* 15 */
  ,              -1000LL      /* 16 */
  ,               -100LL      /* 17 */
  ,                -10LL      /* 18 */
  ,                 -1LL      /* 19 */            
};   

#endif


/************************************************************************/
/* Implementation of class OmsLongPacked                                */
/************************************************************************/

void OmsLongPacked::toInt16( OmsTypeInt8 & hi, OmsTypeUInt8 & lo ) {
  int digit;
  OmsTypeInt8  hi2, hi8;
  OmsTypeUInt8 lo2, lo8;
  hi = 0;
  lo = 0;

  /* calculate absolute value */
  for ( int i = 29; i > 0; i-- ) {

    /* multiplicate by 2 */
    hi2 = (hi << 1) | (lo >> 63);
    lo2 =  lo << 1;

    /* multiplicate by 8 */
    hi8 = (hi << 3) | (lo >> 61);
    lo8 =  lo << 3;

    /* add products */
    hi = hi2 + hi8;
    if ((lo2 + lo8) < lo2) {
      ++hi;
    }
    lo = lo2 + lo8;

    /* add next digit */
    digit = getDigit(i);
    if ((lo + digit) < lo) {
      ++hi;
    }
    lo += digit;

  }

  /* set sign */
  if (BCDMINUS == getSign()) {
    /* one complement */
    hi = ~hi;
    lo = ~lo;
    /* add 1 */
    if (0 == ++lo) {
      ++hi;
    }
  }
}

/*----------------------------------------------------------------------*/

void OmsLongPacked::toLongPacked( OmsTypeInt8 hi,
                                  OmsTypeInt8 lo,
                                  bool isNeg) {
  unsigned char * ptr;
  char            sign;
  int             digit;
  int             cnt;
  int             hiDigits;
  int             loDigits;
  OmsTypeInt64 *  limitptr ;

  /*-----------------*/
  /* Check the range */
  /*-----------------*/
  if ( (hi < 0) || (hi > MAX_LONGPACKED_HI) ||
       (lo < 0) || (lo > MAX_LONGPACKED_LO) ) {
     OMS_Globals::Throw(e_longpacked_out_of_range, "OmsLongPacked::toLongPacked", __MY_FILE__, __LINE__);
  }

  /*---------------------------------------*/
  /* Set digits, negate hi and lo, set ptr */
  /*---------------------------------------*/
  hiDigits = 11;
  loDigits = 18;
  hi = -hi;
  lo = -lo;
  ptr = (unsigned char *)this;

  /*-------------------------------------*/
  /* High part, 29th digit to 19th digit */
  /*-------------------------------------*/
  limitptr = &neg64_limits[19-hiDigits];
  for ( cnt = hiDigits; cnt > 0; cnt -- , limitptr++ ) {
    /*---------------------------------------------------------------*/
    /* The algorithm repeatingly subtracts the decimal value of the  */
    /* digit position until an underflow occurs and repairs the last */
    /* step. If the digit position value is to big anyway, this sub- */
    /* tract loop is skipped rightaway and the digit set to zero.    */
    /*---------------------------------------------------------------*/
    if (hi <= *limitptr) {
      digit = (-1);
      do {
        hi -= *limitptr;
        ++digit;
      }  while (hi <= 0);
      hi += *limitptr ;
    }
    else digit = 0;
    /*------------------------------------------------------------*/
    /* If the loop counter is odd, we will store the digit in the */
    /* left half of the BCD number otherwise the right half and   */
    /* advance the byte pointer into the BCD number.              */
    /*------------------------------------------------------------*/
    if (cnt & 1) {
      *ptr = digit << 4;
    }
    else {
      *ptr++ |= digit;
    }
  } /* for */

  /*-----------------------------------*/
  /* Low part, 18th digit to 1st digit */
  /*-----------------------------------*/
  limitptr = &neg64_limits[19-loDigits];
  for ( cnt = loDigits; cnt > 0; cnt -- , limitptr++ ) {
    /*---------------------------------------------------------------*/
    /* The algorithm repeatingly subtracts the decimal value of the  */
    /* digit position until an underflow occurs and repairs the last */
    /* step. If the digit position value is to big anyway, this sub- */
    /* tract loop is skipped rightaway and the digit set to zero.    */
    /*---------------------------------------------------------------*/
    if (lo <= *limitptr) {
      digit = (-1);
      do {
        lo -= *limitptr;
        ++digit;
      }  while (lo <= 0);
      lo += *limitptr ;
    }
    else digit = 0;
    /*------------------------------------------------------------*/
    /* If the loop counter is odd, we will store the digit in the */
    /* left half of the BCD number otherwise the right half and   */
    /* advance the byte pointer into the BCD number.              */
    /*------------------------------------------------------------*/
    if (cnt & 1) {
      *ptr = digit << 4;
    }
    else {
      *ptr++ |= digit ;
    }
  } /* for */

  /*-------------------------*/
  /* Finally, store the sign */
  /*-------------------------*/
  if (isNeg) {
    sign = BCDMINUS;
  }
  else {
    sign = BCDPLUS;
  }
  *ptr |= sign;

}

