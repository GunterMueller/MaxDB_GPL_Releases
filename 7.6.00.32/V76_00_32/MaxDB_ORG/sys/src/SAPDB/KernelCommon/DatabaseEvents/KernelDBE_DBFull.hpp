/*!
  @file           KernelDBE_DBFull.hpp
  @author         TorstenS
  @ingroup        KernelDBE_DBFull
  @brief          Database Events Management - DB Full eventing
*/

/*
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

*/


#ifndef KERNELDBE_DBFULL_HPP
#define KERNELDBE_DBFULL_HPP


/*===========================================================================*
 *  INCLUDES                                                                 *
 **/

/*!
  @class    KernelDBE_DBFull
  @brief    Is used to generate and send db full events
 */

class KernelDBE_DBFull
{
public:

    /// default constructor of data base full event
    KernelDBE_DBFull(){}

    /*!
       @brief   Generate and send a data base full event
       @return  none
     */

    void SendEvent();
};

#endif  /* KERNELDBE_DBFULL_HPP */
