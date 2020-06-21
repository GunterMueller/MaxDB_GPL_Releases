/* @(#)whoami.h		2.1.1  2001-01-15  15:09 SAP DB */
/*


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




 */

/*
 *      we assume one of the following will be defined by the preprocessor:
 *      vax     for vaxes
 *      pdp11   for pdp11's
 *      mc68000 for motorola mc68000's
 *      ...     according to the machine
 */

/*
 *      hardware characteristics:
 *      address size (16 or 32 or 64 bits)    and
 *      byte ordering (normal or dec11 family, only DEC11 is shown).
 */


#undef  ADDR16

#if defined (BIT64) || defined (_WIN64) 
# undef  ADDR32
# define ADDR64
# else
# define ADDR32
# undef  ADDR64
#endif

#if defined (i386) || defined (I386) || defined(_WIN32) || defined (_WIN64) 
# define DEC11
#else
# undef DEC11
#endif



/***************************************************************
 *                                                             *
 *   The following 'old style' should not longer be used!!!!   *
 *                                                             *
 ***************************************************************/

#ifdef _OLD_STYLE

# ifdef vax
#  undef  ADDR16
#  define ADDR32
#  undef  ADDR64
#  define DEC11
# endif

# if defined (MIPS) || defined (mips)
#  undef  ADDR16
#  define ADDR32
#  undef  ADDR64
#  undef  DEC11
# endif

# ifdef m68k
#  undef  ADDR16
#  define ADDR32
#  undef  ADDR64
#  undef  DEC11
# endif

# ifdef pdp11
#  define ADDR16
#  undef  ADDR32
#  undef  ADDR64
#  define DEC11
# endif

# ifdef _IBMR2   /* RS 6000 */
#  undef  ADDR16
#  define ADDR32
#  undef  ADDR64
#  undef  DEC11
# endif

# if defined ( SUN ) || defined (SOLARIS) || defined ( sun )   /* SUN */
#  undef  ADDR16
#  define ADDR32
#  undef  ADDR64
#  undef  DEC11
# endif

# ifdef __hp9000s700   /* HP 9000, series 700 and 800 */
#  undef  ADDR16
#  define ADDR32
#  undef  ADDR64
#  undef  DEC11
# endif

# ifdef __hp9000s700   /* HP 9000, series 700 and 800 */
#  undef  ADDR16
#  define ADDR32
#  undef  ADDR64
#  undef  DEC11
# endif

# ifdef pyr
#  undef  ADDR16
#  define ADDR32
#  undef  ADDR64
#  undef  DEC11
# endif

# if defined (__alpha)   /* DEC 3000/4000 ALPHA , running OSF1 */
#  undef  ADDR16
#  undef  ADDR32
#  define ADDR64
#  undef  DEC11
# endif

# if defined(_WIN64)
#  undef  ADDR16
#  undef  ADDR32
#  define ADDR64
#  define DEC11
# else
#  if defined (i386) || defined (I386) || defined(_WIN32)
#   undef  ADDR16
#   define ADDR32
#   undef  ADDR64
#   define DEC11
#  endif
# endif
#endif /* OLD_STYLE */
