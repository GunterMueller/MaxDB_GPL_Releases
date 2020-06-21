/*!
  @file           veo50.cpp
  @author         JoergM
  @special area   Mathematic function
  @brief          Implementation of mathematical functions for kernel.
              Allows kernel to check for invalid results in OS independend
              manner.
  @see            example.html ...

\if EMIT_LICENCE

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



\endif
*/




/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#define MOD__ "veo50.cpp :"

#include "heo50.h"

#include <math.h>
#include <float.h>
#include <errno.h>

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#if defined(_WIN32)
#define isnan _isnan
#endif /* _WIN32 */

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

#define MATH_FUNC( _func, _mathfunc) \
externC tsp00_Longreal _func ( tsp00_Longreal  value, \
                               tsp00_NumError &error ) \
{ \
    tsp00_Longreal result; \
\
    errno = 0; \
    result = _mathfunc ( value ) ; \
    if ( errno != 0 || isnan(result) ) \
        error.becomes( num_invalid ) ; \
    else \
        error.becomes( num_ok ) ; \
\
    return ( result ) ; \
}


/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

/*===========================================================================*
 *  GLOBAL VARIABLES                                                         *
 *===========================================================================*/

/*===========================================================================*
 *  LOCAL VARIABLES                                                          *
 *===========================================================================*/

/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/

/*===========================================================================*
 *  PUBLIC FUNCTIONS (CODE)                                                  *
 *===========================================================================*/

MATH_FUNC(vasin,asin)
MATH_FUNC(vsin,sin)
MATH_FUNC(vsinh,sinh)
MATH_FUNC(vacos,acos)
MATH_FUNC(vcos,cos)
MATH_FUNC(vcosh,cosh)
MATH_FUNC(vatan,atan)
MATH_FUNC(vtan,tan)
MATH_FUNC(vtanh,tanh)
MATH_FUNC(vlog10,log10)
MATH_FUNC(vln,log)

/*
 *============================================================================
 */

#undef MF__
#define MF__ MOD__"vatan2"
externC tsp00_Longreal vatan2 ( tsp00_Longreal  y, 
                                tsp00_Longreal  x, 
                                tsp00_NumError &error )
{
    tsp00_Longreal result ;

    errno = 0 ;
    result = atan2 ( y,x ) ;
    if ( errno != 0 || isnan(result) )
        error.becomes( num_invalid ) ;
    else
        error.becomes( num_ok ) ;
    return ( result ) ;
}

/*
 *============================================================================
 */
#undef MF__
#define MF__ MOD__"vexp"
externC tsp00_Longreal vexp ( tsp00_Longreal  value, 
                              tsp00_NumError &error )
{
    tsp00_Longreal result ;
    tsp00_Longreal chk_value = value ;

#if !defined (M_LN2)

# define M_LN2 0.69314718055994530942 
  /* M_LN2 = 0.69314718055994530942 = logarithm of 2 for base e, where e = */
  /* M_E   = 2.7182818284590452354 */

#endif
  /* 1024        = DBL_MAX_EXP         = maximum exponent for base 2 */
  /* 709.7827133 = DBL_MAX_EXP * M_LN2 = maximum exponent for base e */
  if ( chk_value > DBL_MAX_EXP * M_LN2)
       chk_value = DBL_MAX_EXP * M_LN2 ;

  else
    /* It should not be necessary to avoid big negative numbers. */
    /* According to 'man exp' any possible underflow should result on 0 */
    /* with out any warning or error: */
    /* "If x is -INFINITY, exp() returns zero." */
    /* "If the correct value after rounding would be smaller in magnitude */
    /* than MINDOUBLE, exp() returns zero." */

    /*-1021        = DBL_MIN_EXP         = minimum exponent for base 2 */
    /*-707.7032714 = DBL_MIN_EXP * M_LN2 = minimum exponent for base e */

    if ( chk_value < DBL_MIN_EXP * M_LN2)
         chk_value = DBL_MIN_EXP * M_LN2 ;
    errno = 0 ;
    result = exp ( chk_value ) ;

    if ( errno != 0 || isnan(result) )
        error.becomes( num_invalid );
    else
        error.becomes( num_ok );

    return ( result ) ;
}

/*===========================================================================*
 *  LOCAL/PRIVATE/PROTECTED FUNCTIONS (CODE)                                 *
 *===========================================================================*/



/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/