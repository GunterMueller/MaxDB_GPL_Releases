/*!
  @file           Pager_IRestart.hpp
  @author         Henrik
  @brief          Interface for Pager instantiation and Converter restart
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



#ifndef PAGER_IRESTART_HPP
#define PAGER_IRESTART_HPP

class Pager_IRestart;

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
	@class          Pager_IRestart
    @brief          Interface for Pager instantiation and Converter restart
*/
class Pager_IRestart {
public:
    // singleton

    /*!
       @brief          returns instance of Pager_IRestart/Pager_Controller. If
                       no instance exists a new one will be created
       @return         (new) singleton instance of Pager_IRestart

     */

    static Pager_IRestart& GetInstance();

    /*!
       @brief          Read converter from data area for restart
       @param          taskId                 [in] own taskID
       @param          bReadSnapShotConverter [in] the converter to be read
                                                   is the snapshot converter
                                                   (Default = SAPDB_FALSE)
       @return         none

     */

    virtual void ReadConverter(        
        const tsp00_TaskId    taskId,
        const SAPDB_Bool      bReadSnapShotConverter = SAPDB_FALSE ) = 0;
};

#endif  /* PAGER_IRESTART_HPP */


