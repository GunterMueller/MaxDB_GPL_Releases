/*!
  @file           Catalog_IsoLevelScope.hpp
  @author         ThomasA
  @ingroup        Catalog
  @brief          sets the isolation level 

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
#ifndef CATALOG_ISOLEVELSCOPE_HPP
#define CATALOG_ISOLEVELSCOPE_HPP

#include "SQLManager/SQLMan_Context.hpp"

/*!
   @brief sets the isolation level in the scope of a variable of this type
 */
class Catalog_IsoLevelScope
{
public :
    /*!
       @brief constructor, determines required isolation level and stores the old one
     */
    Catalog_IsoLevelScope(SQLMan_Context& context, int RequiredLevel)
        : m_context(context)
    {
        tgg00_HandlingSet isolation;
        isolation.clear();
        if (no_ddl == context.a_is_ddl)
        {
            switch (RequiredLevel) 
            {
            case 10 : 
                isolation.addElement(hsConsistentLock_egg00);
                break;
            case 15 :
                isolation.addElement(hsConsistentLock_egg00);
                isolation.addElement(hsTempLock_egg00);
                break;
            case 20 :
                isolation.addElement(hsTempLock_egg00);
                break;
            case 30 :
                isolation.addElement(hsPermLock_egg00);
                break;
            default :
                isolation.addElement(hsCollisionTest_egg00);
            }
        }
        else
        {
            if (show_command == context.a_command_kind)
            {
                isolation.addElement(hsCollisionTest_egg00);
                isolation.addElement(hsNoWait_egg00);
            }
        }
        m_prevIsolation = context.SetIsolation(isolation);
    }
    /*!
       @brief restores the previous isolation level
     */
    ~Catalog_IsoLevelScope()
    {
       m_context.SetIsolation(m_prevIsolation);
    }
private :
    SQLMan_Context&   m_context;
    tgg00_HandlingSet m_prevIsolation; 
};

#endif