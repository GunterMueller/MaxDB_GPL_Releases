/*!
  @file           heo47.h
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



#ifndef HEO47_H
#define HEO47_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "geo00.h"

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

/*
 * PROCEDURE sql47_ptoc(cout, pin, len)
 *
 *+Verbal Description:
 *
 * This function converts a Pascal type string to C, by eliminating
 * trailing blanks and adding a null terminator.
 *
 *+Parameter description:
 *
 *      void     * charout ;     -- output string, C format
 *      void     * pin ;         -- input string, pascal format
 *      teo00_Int4 len ;         -- length of Pascal buffer, C buffer will
 *                                  have room for one extra character (the null)
 *
 */
externC void sql47_ptoc ( void *charout , void *pin , teo00_Int4 len );

/*
 * PROCEDURE sql47_ctop(pout, cin, len)
 *
 *+Verbal Description:
 *
 * This function converts a C type string to Pascal, by eliminating
 * the null terminator and padding with trailing blanks.
 *
 *+Parameter description:
 *
 *      void     * pout ;        -- output string, Pascal format
 *      void     * charin ;      -- input string, C format
 *      teo00_Int4 len ;         -- length of Pascal buffer, C buffer will
 *                                  have room for one extra character (the null)
 *
 *+No Globals
 *
 *+No Return Value
 */

externC void sql47_ctop ( void *pout , void *charin , teo00_Int4 len );

/*
 * int sql47_itoa(n, buffer, buflen)
 *
 *+Verbal Description:
 *
 * This function converts an integer into a digit string.
 * The result string is passed back in the supplied buffer.  If the buffer is of 
 * insufficient length, a value < 0 is returned.  If the operation is successful, the length
 * of the result string (not including the terminating null character) is returned.
 * If this function is called with buflen = 0, only the length of the needed buffer
 * is returned but no conversion takes place. In that case the buffer is not needed and NULL can
 * be supplied.
 *
 *+Parameter description:
 *
 *      int        n   ;         -- integer to convert
 *      char     * buffer;       -- caller supplied output buffer or NULL (if buflen = 0)
 *      int        buflen;       -- length of caller supplied buffer
 *                                  have room for one extra character (the null) or 0
 *+Return value
 *		length of converted/to be converted string (excluding ZERO byte) or a value < 0 if the buflen
 *      is to small to hold the string
 */
externC int sql47_itoa(int  n, char *buffer, int buflen);

externC int sql47_ltoa(long n, char *buffer, int buflen);
#endif  /* HEO47_H */
