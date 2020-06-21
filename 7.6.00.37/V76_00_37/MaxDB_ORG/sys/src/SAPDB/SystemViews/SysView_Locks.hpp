/*!
  @file           SysView_Locks.hpp
  @author         ElkeZ
  @ingroup   SystemViews
  @brief   This contains a class object representing the LOCKS view.
  @see            
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



#ifndef SYSVIEW_LOCKS_HPP
#define SYSVIEW_LOCKS_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "ggg00.h"
#include "SystemViews/SysView_Defines.hpp"
#include "SystemViews/SysView_SysViewBase.hpp"
#include "SystemViews/SysView_ITableObj.hpp"

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
   @class    SysView_Locks
   @brief    This class is used to handle the system table named
             'LOCKS'. It mainly provides the Create() and 
             Execute() functions for this table.
 */


class SysView_Locks : public SysView_SysViewBase
{

public:

    /*-----------------------------------------------------------------------*/
    /*! @name Constructors and initialization */
    /*@{*/
    /*-----------------------------------------------------------------------*/

    /*!
       @brief   constructs an instance and calls the base ISysView constructor.
       @return  none
     */
    SysView_Locks(SQLMan_Context  &context,
                  tak40_show_glob &a41v)
        : SysView_SysViewBase (context, a41v)
    {}

    /*-----------------------------------------------------------------------*/
    /*@}*/
    /*-----------------------------------------------------------------------*/

    /*-----------------------------------------------------------------------*/
    /*! @name Table creation */
    /*@{*/
    /*-----------------------------------------------------------------------*/

    /*!
       @brief   fills an empty table object with the column definitions.
       @return  none
     */
    void    Create (void);

    /*-----------------------------------------------------------------------*/
    /*@}*/
    /*-----------------------------------------------------------------------*/

    /*-----------------------------------------------------------------------*/
    /*! @name Information access */
    /*@{*/
    /*-----------------------------------------------------------------------*/

    /*!
       @brief   returns the number of columns in this table.
       @return  [SAPDB_Int] - number of columns
     */
    SAPDB_Int       GetColCount (void);

    /*!
       @brief   returns the estimated number of rows for this table.
       @return  [SAPDB_Int] - estimated number of rows
     */
    SAPDB_Int       EstimateRows (void);

    /*!
       @brief   fills this table with information
       @return  none
     */
    void    Execute (void);

    /*-----------------------------------------------------------------------*/
    /*@}*/
    /*-----------------------------------------------------------------------*/
protected:

    /*!
       @brief   fetches the owner and tablename from the system information
       stored for the given surrogate and fills those two values into the
       corresponding columns of the system view
       @return  none
     */

    void SetOwnerTableName (tgg00_Surrogate      &TblID);
};



#endif  /* SYSVIEW_LOCKS_HPP */

