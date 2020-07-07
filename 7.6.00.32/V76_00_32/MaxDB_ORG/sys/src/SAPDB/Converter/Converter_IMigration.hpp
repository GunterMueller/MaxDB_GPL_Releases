/*!
  @file           Converter_IMigration.hpp
  @author         TorstenS
  @author         TorstenS
  @ingroup        Converter
  @brief          Interface for for data base inplace migration from
                  7.2/7.3 to 7.4
\if EMIT_LICENCE


    ========== licence begin  GPL
    Copyright (c) 2003-2004 SAP AG

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

/*
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
  ========== licence end
*/



#ifndef CONVERTER_IMIGRATION_HPP
#define CONVERTER_IMIGRATION_HPP


/*===========================================================================*
 *  INCLUDES                                                                 *
 **/

#include "DataAccess/Data_Types.hpp"

class IOMan_BlockAddress;
class RTETask_ITask;

/*!

   @class          Converter_IMigration
   @brief          This interface is used for data base inplace migration from
                   7.2/7.3 to 7.4
*/
class Converter_IMigration
{
public:


    /*!
    @brief        This method returns the access to the Converter_IMigration interface
                  of component Converter. It is assumed that the Converter singleton
                  is available! No CreateInstance call will be done.
    @return       Access to the Converter_IMigration interface
    */
    static  Converter_IMigration & Instance();

    /*!
       @brief          Set converter entry and block state in FBM for the given
                       page to occupied. The page is identified by the pageNo and
                       the current position on the data devspace is defined by block.
                       Precondtion is that the addressing mode of the given page no
                       is dynamic. Additional it is not allowed to give a page id
                       with a block, that specifies one of the reserved block within
                       the data volumes. (IOMan_InfoPage or RestartPage).
       @param          task           [in] own task
       @param          block          [in] new block address which has to be inserted
                                           in the converter entry and the FBM of the 
                                           page defined by pageNo.
       @param          pageNo         [in] page number for which the converter entry
                                           has to be updated
       @param          bSetToOccupied [in]
       @return         none
     */
    virtual void SetBlockAddressForMigratedPage (
        const RTETask_ITask         &task,
        const IOMan_BlockAddress    &block,
        const Data_PageNo           &pageNo,
        const SAPDB_Bool            bSetToOccupied = SAPDB_TRUE ) = 0;

    /*!
       @brief          This method uses SetBlockAddressForMigratedPage, therefore
                       the same restrictions are valid. The new block address used
                       by the given page will be determined by this method and
                       returned to the caller. If no fre block is available or any
                       error occur the system will execute an emergency shutdown.
       @param          task   [in] own task
       @param          pageNo [in] page number of the converter page entry to be updated.
       @return         IOMan_BlockAddress
     */
    virtual IOMan_BlockAddress DetermineAndSetBlockAddressForMigratedPage(
        const RTETask_ITask &task,
        const Data_PageNo   &pageNo ) = 0;
};

#endif  /* CONVERTER_IMIGRATION_HPP */
