/*!
  @file           veo47.c
  @author         JoergM
  @special area   ptoc
  @brief          Pascal to C String and Other Conversions
  @see            

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

#include "heo47.h"
#include "SAPDBCommon/SAPDB_string.h"
/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



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
 *  LOCAL FUNCTIONS (PROTOTYPES)                                             *
 *===========================================================================*/



/*===========================================================================*
 *  GLOBAL FUNCTIONS (CODE)                                                  *
 *===========================================================================*/

externC void sql47_ptoc ( void *charout , void *pin , teo00_Int4 len )
{
	teo00_Int1 * pptr ;       /* pascal string ptr */
	teo00_Int1 * cptr ;       /* C string ptr */
	
	
	pptr = (teo00_Int1 *)pin;
	pptr += len - 1 ;
	cptr = (teo00_Int1 *)charout;
	cptr += len ;
	
	* cptr -- = '\0' ;
	
	while ( pptr != pin && * pptr == ' ' )
	{
		* cptr -- = '\0' ;
		pptr -- ;
	}
	
	if ( pptr == pin && * pptr == ' ' )
	{
		* cptr -- = '\0' ;
	}
	else
	{
		while ( pptr != pin )
		{
			* cptr -- = * pptr -- ;
		}
		* cptr = * pptr ; /* don't forget last char */
	}
}

/*-----------------------------------------------------------*/

externC void sql47_ctop ( void *pout , void *charin , teo00_Int4 len )
{
	teo00_Int1 * pptr ;       /* pascal string ptr */
	teo00_Int1 * cptr ;       /* C string ptr */

	pptr = (teo00_Int1 *) pout;
	cptr = (teo00_Int1 *) charin;

	while ( len -- > 0 )
	{
		if ( * cptr == '\0' )
		{
			* pptr ++ = ' ' ;
		}
		else
		{
			* pptr ++ = * cptr ++ ;
		}
	}
}

/*-----------------------------------------------------------*/

/*
 * Microsoft (tm) compatible interface '_itoa'
 * !! Buffer length expected to be large enough !!
 * Be prepared to receive at most 33 characters + ZERO.
 *
 * Each of these routines return a pointer to string! There is no error return (oTon MSDN...)
 */
externC char * sql47_MS_itoa(int n, char *buffer, int radix)
{
#define MAX_BINARY_PRESENTATION 32

  char number[MAX_BINARY_PRESENTATION+1];
  unsigned long m;
  int  i;
  int  digits;

  if ( n == 0 )
  {
    *buffer     = '0';
    *(buffer+1) = 0;
    return buffer;
  }

  switch( radix )
  {
  case 16:
    digits = 8;
    break;
  case 8:
    digits = 11;
    break;
  case 4:
    digits = 16;
    break;
  case 2:
    digits = 32;
    break;
  default:
    sql47_ltoa((long)n, buffer, MAX_BINARY_PRESENTATION);
    return buffer;
  }

  m = 0xFFFFFFFFUL & ((unsigned int)n);

  for ( i = digits; m != 0; )
  {
    if ( --i < 0 ) break;
    number[i] = "0123456789abcdef"[m%radix];
    m /= radix;
  }

  SAPDB_memcpy(buffer, &number[i], digits - i);
  buffer[digits-i] = 0;

  return buffer;
}

/*-----------------------------------------------------------*/

externC int sql47_itoa(int n, char *buffer, int buflen)
{
  return ( sql47_ltoa((int)n, buffer, buflen) );
}

/*-----------------------------------------------------------*/

externC int sql47_ltoa(long n, char *buffer, int buflen)
{
#define MAX_DECIMAL_PRESENTATION 32

	char number[MAX_DECIMAL_PRESENTATION];
	char *pN;
	long v;
	int len;

	if ( n == 0 ) 
	{
        if (0 != buflen  && 2 > buflen) 
        {
            len = 0;
        }
        else
        {
            if ( 0 != buflen && 2 <= buflen)
            {
                buffer[0] = '0';
                buffer[1] = 0;
            }

            len = 2;
        }
	}
	else
	{
		number[MAX_DECIMAL_PRESENTATION - 1] = 0;
		pN = number + MAX_DECIMAL_PRESENTATION - 1;

		if ( n > 0 ) 
		{
			v = n;
			while ( v > 0 ) 
			{
				*--pN = "0123456789"[v%10];
				v /= 10;
			}
		}
		else
		{
			v = - n;
			while ( v > 0 ) 
			{
				*--pN = "0123456789"[v%10];
				v /= 10;
			}
			*--pN = '-';
		}

		len = (int)((number + MAX_DECIMAL_PRESENTATION) - pN);

        if (0 != buflen)
        {
            if (len > buflen)
            {
                len = 0;
            }
            else
            {
                SAPDB_memcpy( buffer, pN, len);
            }
        }
	}
    return len - 1;
}

/*===========================================================================*
 *  LOCAL FUNCTIONS (CODE)                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/