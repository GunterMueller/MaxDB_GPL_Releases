/*!
  @file           Pager_Lock.hpp
  @author         Henrik
  @brief          simple wrapper for vbegexcl
  @see            


    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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



*/



#ifndef PAGER_LOCK_HPP
#define PAGER_LOCK_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!
  @brief          simple wrapper for vbegexcl
*/
class Pager_Lock {

public:

	/// constructor
    Pager_Lock( tsp00_TaskId TaskId );

	/// destructor
    ~Pager_Lock();

	/// explicit remove lock 
    void unlock();

private:

    tsp00_TaskId    m_Owner;
    tsp00_RegionId  m_RegionId;
};

#endif  /* PAGER_LOCK_HPP */
