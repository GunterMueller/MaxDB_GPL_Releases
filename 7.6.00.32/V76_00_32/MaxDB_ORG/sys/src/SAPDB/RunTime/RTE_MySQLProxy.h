/*!---------------------------------------------------------------------
  @file           RTE_MySQLProxy.h
  @author         JoergM
  @ingroup        Runtime
  @brief          Password Scrambling PASCAL Kernel Interfacing

    What the kernel has to do
    
      Use RTE_GetMySQLScramble(tsp00_C8)
      to initialize the tsp00_C8 stored in acv for later usage in password check. 
      The scramble text (fixed size) is also result for special GETCHALLENGE
      command issued before CONNECT.

      Use RTE_GetMySQLHash(ARRAY or tsp00_Int4[1...2], tsp00_Pw)
      to create the hash from uncrypted password.

      Use RTE_CheckMySQLPassword() to check received scrambled password hash
      which is transferred instead of encrypted password in  CONNECT command.
        
\if EMIT_LICENCE

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



\endif
 */
#ifndef RTE_MYSQLPROXY_H
#define RTE_MYSQLPROXY_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "RunTime/RTE_CompilerFeatures.h"


/*!
  @brief Get scramble text valid for MySQL connection

  @param scramble [out] connection specific scramble output
 */
externC void RTE_GetMySQLScramble(tsp00_C8 VAR_ARRAY_REF scramble);

/*!
  @brief Get hash from uncrypted password
  @param hash0 [out] first hash word
  @param hash1 [out] second hash word
  @param password [in] password to use
 */
externC void RTE_GetMySQLHash(tsp00_Int4 VAR_VALUE_REF hash0,
                              tsp00_Int4 VAR_VALUE_REF hash1,
                              tsp00_Pw VAR_ARRAY_REF password);

/*!
  @brief Check received scrambled password hash

  This call needs receivedScrambledPasswordHash and oldVersion from CONNECT datapart.
  Scramble is found in acv and hash0 and hash1 are either stored in parallel with 
  encrypted password are recalculated from decrypted password.

  @param receivedScrambledPasswordHash [in] received 'password' from CONNECT
  @param oldVersion [in] 0 if new mySQL scrmbling, else old MySQL scrambling
  @param scramble [in] connection specific scramble output
  @param hash0 [in] first password hash word
  @param hash1 [in] second password hash word
  @return 0(false) if password does not match, else password matches
 */
externC tsp00_Bool RTE_CheckMySQLPassword( tsp00_C8 VAR_ARRAY_REF receivedScrambledPasswordHash,
                                           tsp00_Bool             oldVersion,
                                           tsp00_C8 VAR_ARRAY_REF scramble,
                                           tsp00_Int4             hash0,
                                           tsp00_Int4             hash1
                                         );
#endif /* RTE_MYSQLPROXY_H */

