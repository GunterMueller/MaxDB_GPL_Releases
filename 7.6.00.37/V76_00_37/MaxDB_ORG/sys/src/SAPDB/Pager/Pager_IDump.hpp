/*!
  @file           Pager_IDump.hpp
  @author         Henrik
  @brief          abstract dump interface class to the pager
  @see            

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



#ifndef PAGER_IDUMP_HPP
#define PAGER_IDUMP_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"

#include "KernelCommon/Kernel_Common.hpp"

 /*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

class Kernel_Dump;

/*!
  @interface      Pager_IDump
  @brief          abstract dump interface class to the pager
 */
class Pager_IDump
{
public:

    /*!
       @brief  This method returns the access to the Pager_IDump interface of component Pager. 
       @return Access to the Pager_IDump interface
    */ 
    static Pager_IDump& GetInstance();

    /*!
       @brief          This method is used to dump the main memory structures of the component Pager.
       @param          TaskId [in] identification of the calling task
       @param          Dump [in/out] kernel dump file
       @return         none
     */
    virtual void Dump(
        const tsp00_TaskId    TaskId,
        Kernel_Dump     &Dump ) = 0;
};

#endif  /* PAGER_IDUMP_HPP */
