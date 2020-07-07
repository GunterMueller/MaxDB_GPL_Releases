/*!
  @file           SysView_MultiVersionReadStatistics.hpp
  @author         ThomasA
  @ingroup        SystemViews
  @brief   This contains a class object representing the MULTIVERSIONREADSTATIS view.
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



#ifndef SYSVIEW_MULTIVERSIONREADSTATISTICS_HPP
#define SYSVIEW_MULTIVERSIONREADSTATISTICS_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SystemViews/SysView_SysViewBase.hpp"

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
   @class    SysView_MultiVersionReadStatistics
   @brief    This class is used to handle the system table named
             'MULTIVERSIONREADSTATISTICS'. It mainly provides the Create() and
             Execute() functions for this table.

 */


class SysView_MultiVersionReadStatistics : public SysView_SysViewBase
{

public:

    /*-----------------------------------------------------------------------*/
    /*! @name Constructors and initialization */
    /*@{*/
    /*-----------------------------------------------------------------------*/

    /*!
       @brief   constructs an instance and calls the base ISysView constructor.
     */
    SysView_MultiVersionReadStatistics(SQLMan_Context  &context,
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
       @brief      returns the number of columns in this table.
       @return SAPDB_Int  number of columns
     */
    SAPDB_Int       GetColCount (void);

    /*!
       @brief      returns the estimated number of rows for this table.
       @return SAPDB_Int  estimated number of rows

     */
    SAPDB_Int       EstimateRows (void);

    /*!
       @brief   fills this table with information
       @return  none
     */
    void    Execute (void);

    /*!
       @brief   resets the values of the corresponding ...RESET-systemview
       @return  [SAPDB_Bool] - correct working of the routine
     */
    SAPDB_Bool ResetCounter (void);

    /*-----------------------------------------------------------------------*/
    /*@}*/
    /*-----------------------------------------------------------------------*/

};



#endif  /* SYSVIEW_MULTIVERSIONREADSTATISTICS_HPP */
