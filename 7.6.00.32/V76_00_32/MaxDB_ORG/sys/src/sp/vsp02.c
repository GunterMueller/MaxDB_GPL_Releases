/****************************************************************************

  module      : vsp02.c

  -------------------------------------------------------------------------

  responsible : DanielD

  special area: Encrypting

  first created:2000-06-05  12:05
  last changed: 2000-06-05  12:07
  see also    : example.html ...

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2000-2005 SAP AG




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




*****************************************************************************/



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "hsp02.h"

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
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/



/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

static void
s02_encrypt (
    tsp00_Name    clearname,
    tsp00_CryptPw crypt,
    int p4,
    int p5,
    int p6)
{
    int         i;
    tsp00_Int4  left;
    tsp00_Int4  right;
    tsp00_CryptName aux;
    static const int lastCryptIndexC = csp_cryptname - 1;

    for (i = 0; i < csp_cryptname; ++i) {
        aux [i] =
            clearname [3 * i    ] * p6 +
            clearname [3 * i + 1] * p5 +
            clearname [3 * i + 2] * p4;
    }
    for (i = 0; i < csp_cryptname; ++i) {
        if (i > 0) {
            left = aux [i - 1];
        }
        else {
            left = p6;
        }
        aux [i] += (left % 61) * (p6 * 126 - 1);
    }
    for (i = lastCryptIndexC; i >= 0; --i) {
        if (i < lastCryptIndexC - 1) {
            right = aux [i + 1];
        }
        else {
            right = p5;
        }
        aux [i] += (right % 61) * (p6 * 128 - 1);
    }
    for (i = 0; i < csp_cryptname; ++i) {
        if ((aux [i] % 2) != 0) {
            aux [i] = - aux [i];
        }
    }
    memcpy (&crypt[0], &aux[0], sizeof (aux));
}

/*----------------------------------------*/

static void
s02_decrypt (
    tsp00_Name          clearname,
    const tsp00_CryptPw crypt,
    int               p4,
    int               p5,
    int               p6)
{
  int            i;
  tsp00_Int4       lleft;
  tsp00_Int4       rright;
  tsp00_Int4       exp1;
  tsp00_Int4       exp2;
  tsp00_Int4       exp3;
  bool           isNull = true;
  unsigned char* endPtr;
  tsp00_CryptName  crypted;

  memcpy(crypted, crypt, sizeof(crypted));

  for (i = 0; i < csp_cryptname; ++i) {
    if (crypted [i] != 0) {
      isNull = false;
      break;
    } /* end if */
  } /* end for */

  if (isNull) {
    clearname [0] = '\0';
  } else {

    for (i = 0; i < csp_cryptname; ++i) {
      if (crypted [i] % 2 != 0) {
        crypted [i] = - crypted [i];
      } /* end if */
    } /* end for */

    for (i = 1; i <= csp_cryptname; ++i) {
      if (i < csp_cryptname - 1) {
        rright = crypted [i];
      } else {
        rright = p5;
      } /* end if */
      crypted [i - 1] = crypted [i - 1] - ((rright % 61) * (p6 * 128 - 1));
    } /* end for */

    for (i = csp_cryptname; i >= 1; --i) {
      if (i > 1) {
        lleft = crypted [i - 2];
      } else {
        lleft = p6;
      } /* end if */
      crypted [i - 1] = crypted [i - 1] - ((lleft % 61) * (p6 * 126 - 1));
    } /* end for */

    for (i = 1; i <= csp_cryptname; ++i) {
      exp3 = crypted [i - 1];
      exp1 = exp3 / p6;
      exp2 = exp3 % p6;
      clearname [3 * i - 3] = (char) exp1;
      exp3 = exp2;
      exp1 = exp3 / p5;
      exp2 = exp3 % p5;
      clearname [3 * i - 2] = (char) exp1;
      exp3 = exp2;
      exp1 = exp3 / p4;
      exp2 = exp3 % p4;
      clearname [3 * i - 1] = (char) exp1;
    } /* end for */

    /* PTS 1110288 E.Z. */

  } /* end if */
} /* end s02_decrypt */

/*----------------------------------------*/

externC void
s02encrypt (
    tsp00_Name      clearname,
    tsp00_CryptPw   crypt)
{
    s02_encrypt (clearname, crypt, 2, 523, 133387);

}

/*----------------------------------------*/

externC void
s02applencrypt (
    tsp00_Name      clearname,
    tsp00_CryptPw   crypt)
{
    s02_encrypt (clearname, crypt, 2, 521, 133379);
}

/*----------------------------------------*/

externC void
s02decrypt (
    tsp00_Name           clearname,
    const tsp00_CryptPw  crypt)
{
    s02_decrypt (clearname, crypt, 2, 523, 133387);

} /* s02decrypt */

/*----------------------------------------*/

externC void
s02appldecrypt (
    tsp00_Name           clearname,
    const tsp00_CryptPw  crypt)
{
    s02_decrypt (clearname, crypt, 2, 521, 133379);
} /* end s02appldecrypt */

/*----------------------------------------*/

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/