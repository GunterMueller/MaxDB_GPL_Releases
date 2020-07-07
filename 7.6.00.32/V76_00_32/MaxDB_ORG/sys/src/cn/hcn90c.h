/*! 
  -----------------------------------------------------------------------------
 
  module: hcn90c.h
 
  -----------------------------------------------------------------------------
 
  responsible:  BerndV
 
  special area: DBMServer
 
  description:  specification of useful utility functions
                for the cn (Control) layer

  version:      7.2. / 6.2 

  Copyright (c) 1998-2005 SAP AG
 
  -----------------------------------------------------------------------------

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



  -----------------------------------------------------------------------------
*/
 

#ifndef _HCN90C_H_
#define _HCN90C_H_

/* 
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include "gsp00.h"

/* SPECIFICATION PUBLIC FUNCTION cn90NumericValue
 * ------------------------------------------------------------------
 * purpose:
 *    This fucntion returns the numreric value of a given digit 
 *    character. The character may be {0...9,a...f,A...F}.
 *
 * parameters:
 *    char              c
 *         - character to analyzer
 * return:
 *    int  - the integer value of the digit character, 0 for invalid digits
 */
int cn90NumericValue(char c);

/* SPECIFICATION PUBLIC FUNCTION cn90Uncrypt
 * ------------------------------------------------------------------
 * purpose:
 *    Uncrypt a password. Source representation must be a sequence
 *    of 6 (MAX_CRYPT) numbers consisting each of 8 digits in hex
 *    representation. There is no space or other delimiter between
 *    two numbers. -> source length ist 52 = 6 * 8
 *
 * parameters:
 *    char              * szPassword
 *       - buffer with the crypted source, will be used for uncrypted
 *         result
 *    bool             bForDatabase
 *       - use APP or DB crypt
 *
 * return:
 *    char *  - the password buffer (equal to szPassword)
 */
char * cn90Uncrypt 
      ( char      * szPassword,
        bool        bForDatabase);

/* IMPLEMENTATION PUBLIC FUNCTION cn90CalculateUncrypt
 * ------------------------------------------------------------------
 * purpose:
 *    Uncrypt a password. Source ist the binary crypted representation
 *
 * parameters:
 *    tsp00_CryptName       pwCrypted
 *       - crypted representation of password
 *    char              * passwd
 *       - buffer for the uncryptedresult
 *    bool             bForDatabase
 *       - use APP or DB crypt
 *
 * return:
 *    char *  - the uncrypted password (equal to passwd)
 */
char * cn90CalculateUncrypt
      ( const tsp00_CryptName   pwCrypted,
        char                * passwd,
        bool                  bForDatabase);

/* IMPLEMENTATION PUBLIC FUNCTION cn90CalculateUncrypt
 * ------------------------------------------------------------------
 * purpose:
 *    Uncrypt a password. Source ist the binary crypted representation
 *
 * parameters:
 *    tsp00_CryptPw         pwCrypted
 *       - crypted representation of password
 *    char              * passwd
 *       - buffer for the uncryptedresult
 *    bool             bForDatabase
 *       - use APP or DB crypt
 *
 * return:
 *    char *  - the uncrypted password (equal to passwd)
 */
char * cn90CalculateUncrypt
      ( const tsp00_CryptPw     pwCrypted,
        char                * passwd,
        bool                  bForDatabase);

/*!
  -----------------------------------------------------------------------------
 
  function:     cn90ClearStringToCryptString

  -----------------------------------------------------------------------------

  description:  convert string from clear to crypt

                The representation of a crypt string consist of six hexnumbers
                with 8 digits per number.

  arguments:    szClearString [IN]  - like "mypassword"
                szCryptString [OUT] - like "d4ff56676749993a4ad39362c62fc1a152eb694858ee061e"

  -----------------------------------------------------------------------------
*/
void cn90ClearStringToCryptString
      ( tsp00_Namec    szClearString, 
        tsp00_C64c   & szCryptString);

/*!
  -----------------------------------------------------------------------------
 
  function:     cn90CryptStringToClearString

  -----------------------------------------------------------------------------

  description:  convert string from crypt to clear

                The representation of a crypt string consist of six hexnumbers
                with 8 digits per number.

  arguments:    szCryptString [IN]  - like "d4ff56676749993a4ad39362c62fc1a152eb694858ee061e"
                szClearString [OUT] - like "mypassword"
                bForDatabase  [IN]  - use database encryption instead of application
                                      encryption
  -----------------------------------------------------------------------------
*/
void cn90CryptStringToClearString
      ( tsp00_C64c     szCryptString,
        tsp00_Namec  & szClearString,
        bool           bForDatabase = false);

/*!
  -----------------------------------------------------------------------------
 
  function:     cn90CryptToCryptString

  -----------------------------------------------------------------------------

  description:  convert crypt from binary to string representation

                The representation of a crypt string consist of six hexnumbers
                with 8 digits per number.

  arguments:    stCrypt       [IN]  - crypt structure
                szCryptString [OUT] - like "d4ff56676749993a4ad39362c62fc1a152eb694858ee061e"

  -----------------------------------------------------------------------------
*/
void cn90CryptToCryptString
      ( tsp00_CryptName   stCrypt, 
        tsp00_C64c      & szCryptString);
void cn90CryptToCryptString
      ( tsp00_CryptPw     stCrypt, 
        tsp00_C64c      & szCryptString);

#endif /* _HCN90C_H_ */
