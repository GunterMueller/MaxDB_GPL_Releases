/*!
  @file    RTE_ClientInterfaceInstance.cpp
  @author  JoergM
  @ingroup Runtime
  @brief   Session-free kernel runtime interface for use in liboms.

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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

#include "RunTime/RTE_ClientInterfaceInstance.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#ifdef WANT_SHARED_LIB_SUPPORT
#include "heo02x.h"
#endif
#include "RunTime/RTE_Message.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

RTE_IInterface *RTE_IInterface::it = 0;

RTE_IInterface * RTE_IInterface::Initialize()
{
    SAPDBMEM_STATIC_RAW_ALLOCATE(Space, sizeof(RTE_ClientInterfaceInstance));
    if (0 == it )
    {
        it = new(Space) RTE_ClientInterfaceInstance;
    }
    return it;
}

    /*!
      @brief get a procedure address from given shared library handle
     */
RTE_SharedLibProcAddress RTE_ClientInterfaceInstance::GetProcAddress( RTE_SharedLibHandle hLib,
                                                                     SAPDB_Char const *szSqlFunction, 
                                                                     SAPDB_Char *pErrText,
                                                                     SAPDB_UInt4 lenErrTextInBytes)
{
#ifndef WANT_SHARED_LIB_SUPPORT
    return (RTE_SharedLibProcAddress)0;
#else
    return (RTE_SharedLibProcAddress)sqlGetProcAddress( hLib
                                                      , szSqlFunction
                                                      , pErrText
                                                      , lenErrTextInBytes );
#endif
}

void RTE_ClientInterfaceInstance::DiagMessage(Msg_List const &msg)
{
    RTE_Message(msg, MSG_DIAG_CONSOLE);
}
