/*!
  @file           Converter_Lock.hpp
  @author         Henrik
  @ingroup        Converter
  @brief          simple wrapper for vbegexcl
*/

/*
    ========== licence begin  GPL
    Copyright (c) 2002-2004 SAP AG

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



#ifndef CONVERTER_LOCK_HPP
#define CONVERTER_LOCK_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 **/

#include "RunTime/Tasking/RTETask_ITask.hpp"


/*===========================================================================*
 *  DEFINES                                                                  *
 **/


/*===========================================================================*
 *  MACROS                                                                   *
 **/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 **/

/*!
  @class Converter_Lock
  @brief          simple wrapper for vbegexcl
*/
class Converter_Lock {

public:

    /*!
       @brief    Constructor
       @param    TaskId [in] my TaskId
    */
    Converter_Lock( const RTETask_ITask  &task );

    /*!
       @brief    Destructor
    */
    ~Converter_Lock();

private:

    /// my TaskId
    RTE_TaskId    m_Owner;

    /// my current region id
    RTE_RegionId  m_RegionId;
};

#endif  /* CONVERTER_LOCK_HPP */
