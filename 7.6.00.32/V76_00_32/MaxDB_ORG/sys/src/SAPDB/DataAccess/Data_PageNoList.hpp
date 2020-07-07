/*!
  @file     Data_PageNoList.hpp
  @author   TorstenS
  @ingroup  DataAccess
  @brief    This module is used to handle a collection of Data_PageNos
  @see            

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2000-2006 SAP AG

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


#ifndef DATA_PAGENOLIST_HPP
#define DATA_PAGENOLIST_HPP


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "Container/Container_Vector.hpp"
#include "DataAccess/Data_Types.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  FORWARD DECLARATION                                                      *
 *===========================================================================*/


/*===========================================================================*
 *  DECLARATION OF CLASSES, STRUCTURES, TYPES, UNIONS ...                    *
 *===========================================================================*/

    /// Defines abbreviation of a container vector which consists of Data_Pages 

typedef Container_Vector< Data_PageNo >  Data_PageNoList; 


/*===========================================================================*
 *  DEFINITION OF METHODS                                                    *
 *===========================================================================*/


#endif  /* DATA_PAGENOLIST_HPP */

