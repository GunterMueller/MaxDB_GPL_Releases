/*! ******************************************************************************

  module: vcn90c.cpp

  ------------------------------------------------------------------------------

  responsible:  BerndV

  special area: Control-Server

  description:  implementation of client relevant useful utility functions
                for the cn (Control) layer

  ------------------------------------------------------------------------------

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


  *******************************************************************************/

/* ------------------------------------------------------------------
 * IMPORT
 * ------------------------------------------------------------------
 */
#include <stdio.h>

#include "SAPDBCommon/SAPDB_string.h"
#include "hsp02.h"
#include "hcn90c.h"

#ifndef MAX_CRYPT
  #define MAX_CRYPT csp_cryptname
#endif
#define CRYPT_BASE      16

/* IMPLEMENTATION PUBLIC FUNCTION cn90NumericValue
 * ------------------------------------------------------------------
 */
int cn90NumericValue(char c) 
{
  switch (c) {
    case '0':case '1':case '2':case '3':case '4':
    case '5':case '6':case '7':case '8':case '9':
      return c - 48;
      break;
    case 'A':case 'B':case 'C':case 'D':case 'E':case 'F':
      return c - 55;
      break;
    case 'a':case 'b':case 'c':case 'd':case 'e':case 'f':
      return c - 87;
      break;
  } // end switch
  return 0;
} // end cn90NumericValue

/* IMPLEMENTATION PUBLIC FUNCTION cn90Uncrypt
 * ------------------------------------------------------------------
 */
char * cn90Uncrypt
      ( char      * szPassword,
        bool        bForDatabase)
{
  tsp00_CryptName pwCrypted;
  int           i;
  int           nLenNumber = sizeof(tsp00_CryptName) / MAX_CRYPT * 2;
  char          szNumber[2];

  memset(&pwCrypted, 0, sizeof(pwCrypted));

  if ((int) strlen(szPassword) == nLenNumber * MAX_CRYPT) {
    szNumber[1] = 0;
    for (i = 0; szPassword[i] != 0; i++) {
      pwCrypted[i / nLenNumber] = (pwCrypted[i / nLenNumber] * CRYPT_BASE) + cn90NumericValue(szPassword[i]);
    } /* end for */

    cn90CalculateUncrypt(pwCrypted, szPassword, bForDatabase);
  } /* end if */

  return szPassword;
} /* end cn90Uncrypt */

/* IMPLEMENTATION PUBLIC FUNCTION cn90CalculateUncrypt
 * ------------------------------------------------------------------
 */
char * cn90CalculateUncrypt
      ( const tsp00_CryptPw     pwCrypted,
        char                * passwd,
        bool                  bForDatabase)
{
  tsp00_Name  pwClear;
  tsp00_Namec pwClearc;

  if (bForDatabase) {
    s02decrypt(pwClear, pwCrypted);
  } else {
    s02appldecrypt(pwClear, pwCrypted);
  } // end if

  pwClearc.p2c(pwClear);
  SAPDB_strcpy(passwd, pwClearc);

  return passwd;
} // end cn90CalculateUncrypt

/* IMPLEMENTATION PUBLIC FUNCTION cn90CalculateUncrypt
 * ------------------------------------------------------------------
 */

char * cn90CalculateUncrypt
      ( const tsp00_CryptName   pwCrypted,
        char                * passwd,
        bool               bForDatabase)
{
  tsp00_CryptPw pwTemp;

  SAPDB_memcpy(&pwTemp[0], &pwCrypted[0], sizeof(tsp00_CryptPw));

  return cn90CalculateUncrypt(pwTemp, passwd, bForDatabase);
} // end cn90CalculateUncrypt

/*
  -----------------------------------------------------------------------------
  function cn90ClearStringToCryptString
  -----------------------------------------------------------------------------
*/
void cn90ClearStringToCryptString
      ( tsp00_Namec    szClearString,
        tsp00_C64c   & szCryptString)
{
  tsp00_CryptPw   stCrypt;
  tsp00_Name      pasString;

  s02applencrypt(pasString.c2p(szClearString), stCrypt);
  cn90CryptToCryptString(stCrypt, szCryptString);
} // end cn90CryptToString

/*
  -----------------------------------------------------------------------------
  function cn90CryptStringToClearString
  -----------------------------------------------------------------------------
*/
void cn90CryptStringToClearString
      ( tsp00_C64c     szCryptString,
        tsp00_Namec  & szClearString,
        bool           bForDatabase)
{
  tsp00_C64c szTemp;

  szTemp = szCryptString;

  szClearString.rawAssign(cn90Uncrypt(szTemp, bForDatabase));

} // end cn90CryptStringToClearString

/*
  -----------------------------------------------------------------------------
  function cn90CryptToCryptString
  -----------------------------------------------------------------------------
*/
void cn90CryptToCryptString
      ( tsp00_CryptPw     stCrypt,
        tsp00_C64c      & szCryptString)
{
  tsp00_CryptName stTemp;

  SAPDB_memcpy (&stTemp[0], &stCrypt[0], sizeof(stTemp));

  cn90CryptToCryptString(stTemp, szCryptString);
} // end cn90CryptToString
/*
  -----------------------------------------------------------------------------
*/
void cn90CryptToCryptString
      ( tsp00_CryptName   stCrypt,
        tsp00_C64c      & szCryptString)
{
  sprintf ( szCryptString , "%08x%08x%08x%08x%08x%08x",
                              (int) stCrypt[0],
                              (int) stCrypt[1],
                              (int) stCrypt[2],
                              (int) stCrypt[3],
                              (int) stCrypt[4],
                              (int) stCrypt[5]);
} // end cn90CryptToString
