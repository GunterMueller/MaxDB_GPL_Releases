/*!
  @file    RTE_ClientInterfaceInstance.hpp
  @author  JoergM
  @ingroup Runtime
  @brief   Session-free kernel runtime interface implementation for use in liboms.

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

#ifndef RTE_CLIENTINTERFACEINSTANCE_HPP
#define RTE_CLIENTINTERFACEINSTANCE_HPP

#include "RunTime/RTE_CommonInterfaceInstance.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!
  @class RTE_ClientInterfaceInstance
  @brief The exported kernel runtime interface
  
  This class contains all functions provided to all threads or tasks inside a livecache kernel or liboms
 */
class RTE_ClientInterfaceInstance : public RTE_CommonInterfaceInstance
{
public:
    /*!
      @brief used as singleton
     */
    RTE_ClientInterfaceInstance() {}

    /*!
      @brief get a procedure address from given shared library handle
     */
    virtual RTE_SharedLibProcAddress GetProcAddress( RTE_SharedLibHandle hLib,
                                                     SAPDB_Char const *szSqlFunction, 
                                                     SAPDB_Char *pErrText,
                                                     SAPDB_UInt4 lenErrTextInBytes);
    /*! 
        @brief diagnostic meassage output function
        @param msg [in] message to show
     */
    virtual void DiagMessage(Msg_List const &msg);
private:
    /*! @brief dtor */
    virtual ~RTE_ClientInterfaceInstance() {}
};

/*!
  endclass:       RTE_ClientInterfaceInstance
*/

#endif  /* RTE_CLIENTINTERFACEINSTANCE_HPP */
