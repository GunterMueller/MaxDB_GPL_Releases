/*!---------------------------------------------------------------------
  @file           RTE_MySQLProxy.cpp
  @author         JoergM
  @ingroup        Runtime
  @brief          Password Scrambling

  This contains the implementation of the MySql protocol specific
  Haskey creation and scrambling methods.

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

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "RunTime/RTE_MySQLProxy.hpp"
#include "RunTime/RTE_MySQLProxy.h"
#include "RunTime/System/RTESys_Time.h"
#include "SAPDBCommon/SAPDB_MemCopyMoveNoCheck.hpp"
#include <stdlib.h>

static SAPDB_UInt8 generatorStartTime = 0;

RTE_MySQLCrypting * RTE_MySQLCrypting::m_Instance = 0;

RTE_MySQLCrypting & RTE_MySQLCrypting::Instance()
{
      SAPDBMEM_STATIC_RAW_ALLOCATE(Space, sizeof(RTE_MySQLCrypting));
  
  if ( !RTE_MySQLCrypting::m_Instance )
  {
    RTE_MySQLCrypting::m_Instance = new (Space) RTE_MySQLCrypting();
  }
  return *m_Instance;
}

RTE_MySQLCrypting::RTE_MySQLCrypting()
{
    generatorStartTime = RTESys_Time();
    m_Generator = RTE_MySQLPseudoRandomGenerator((SAPDB_UInt4) generatorStartTime
                                                ,(SAPDB_UInt4) generatorStartTime/2);

}

extern "C" void RTE_GetMySQLScramble(tsp00_C8 &pascalScramble)
{
    RTE_MySQLScramble scramble;
    RTE_MySQLCrypting::Instance().GenerateScramble(scramble);

    pascalScramble.c2p(&scramble[0]);
}

extern "C" void RTE_GetMySQLHash( tsp00_Int4 & hash0,
                                  tsp00_Int4 & hash1,
                                  tsp00_Pw & password )
{
    tsp00_Pwc cPassword;
    cPassword.p2c(password);
    RTE_MySQLHash result;
    RTE_MySQLCrypting::Instance().GenerateHash(cPassword.asCharp(), result);
    hash0 = result[0];
    hash1 = result[1];
}

extern "C" tsp00_Bool RTE_CheckMySQLPassword( tsp00_C8   & receivedScrambledPasswordHash,
                                              tsp00_Bool   oldVersion,
                                              tsp00_C8   & scramble,
                                              tsp00_Int4   hash0,
                                              tsp00_Int4   hash1 )
{
    RTE_MySQLScramble givenScrambledPasswordHash;

    SAPDB_MemFillNoCheck( &givenScrambledPasswordHash[0], 0, sizeof(RTE_MySQLScramble));
    
    SAPDB_MemCopyNoCheck( &givenScrambledPasswordHash[0], 
            receivedScrambledPasswordHash.asCharp(),
            receivedScrambledPasswordHash.length() );

    RTE_MySQLScramble connectionScramble;

    SAPDB_MemFillNoCheck( &connectionScramble[0], 0, sizeof(RTE_MySQLScramble));
    
    SAPDB_MemCopyNoCheck( &connectionScramble[0], 
            scramble.asCharp(),
            scramble.length() );

    RTE_MySQLHash passwordHash;

    passwordHash[0] = hash0;
    passwordHash[1] = hash1;

    return !RTE_MySQLCrypting::Instance().CheckScramble( givenScrambledPasswordHash,
                                                         oldVersion != 0,
                                                         connectionScramble,
                                                         passwordHash );
}
