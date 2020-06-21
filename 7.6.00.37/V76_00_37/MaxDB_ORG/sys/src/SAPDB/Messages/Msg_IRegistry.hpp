/*!
  @file           Msg_IRegistry.hpp
  @author         JoergM
  @ingroup        RunTime
  @brief          Interface for registring messaging message lists

  Implementation found in Msg_List.cpp

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
#ifndef MSG_IREGISTRY_HPP
#define MSG_IREGISTRY_HPP

#include "SAPDBCommon/SAPDB_Types.hpp"

// cannot use forward declaration here...
#include "Messages/Msg_List.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

class SAPDBMem_IRawAllocator;
class Msg_RegistrySlot;

/*!---------------------------------------------------------------------------
  @class Msg_IRegistry
  @brief Interface for registring and deregistring of Msg_List instances
  
  Provides Msg_List internal used allocator and allows to register and deregister message lists
*/
class Msg_IRegistry
{
    /*!
        @brief Register a message list into global array
        @return pointer to slot which must be used to deregister
     */
    virtual Msg_RegistrySlot const *Register(Msg_List const &msg) = 0;

    /*!
        @brief Deregister a message list from global array
        @param pSlot[in] pointer to slot where message is registered
        @return none
     */
    virtual void Deregister(Msg_RegistrySlot const *&pSlot) = 0;

    /*!
      @brief get the allocator to be used for all msg list allocation internally
      @return reference to message list emergency allocator
     */
    virtual SAPDBMem_IRawAllocator &Allocator() = 0;
}; /*! @endclass Msg_IRegistry */

#endif  /* MSG_IREGISTRY_HPP */
