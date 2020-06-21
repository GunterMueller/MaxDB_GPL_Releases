/*****************************************************************************/
/*!
  @file         Data_IndexOrderList.hpp
  @author       MartinKi
  @ingroup      DataAccess

  @brief        Header file for class Data_IndexOrderList.

\if EMIT_LICENCE
  ========== licence begin  GPL
  Copyright (C) 2005 SAP AG

  This program is free software; you can redisytribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
  ========== licence end
\endif
*/
/*****************************************************************************/

#ifndef DATA_INDEXORDERLIST_HPP
#define DATA_INDEXORDERLIST_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "Container/Container_Stack.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"
#include "gbd00.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

class SAPDBMem_IRawAllocator;

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!
 * @brief Provides a stack container for objects of type
 * tbd00_TreeIndexOrder.
 */
class Data_IndexOrderList : public Container_Stack<tbd00_TreeIndexOrder>
{
public:
    /*!
     * @brief Constructor.
     */
    Data_IndexOrderList(SAPDBMem_IRawAllocator& allocator)
        : Container_Stack<tbd00_TreeIndexOrder>( allocator )
    {
        this->Initialize( 6 );
    }

    /*!
     * @brief Dumps contents of list to vtrace.
     *
     * @param force [in] ignore trace settings and always write to vtrace
     */
    void WriteToVtrace(const SAPDB_Bool force = false) const;

private:
    //no copy-construction or assignment
    Data_IndexOrderList(const Data_IndexOrderList&);
    Data_IndexOrderList operator=(const Data_IndexOrderList&);
};

#endif // DATA_INDEXORDERLIST_HPP
