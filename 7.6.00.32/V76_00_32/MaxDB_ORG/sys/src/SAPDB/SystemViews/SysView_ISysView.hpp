/*!
  @defgroup SystemViews System Views
*/
/*!
  @file           SysView_ISysView.hpp
  @author         ElkeZ
  @ingroup        SystemViews
  @brief   This contains an abstract interface of all system view classes.
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



#ifndef SYSVIEW_ISYSVIEW_HPP
#define SYSVIEW_ISYSVIEW_HPP


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

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
  @interface  SysView_ISysView
  @brief      This class is designed to be an interface class for all system
             views. However, this class is a bit more than that, since it
             also provides some additional functions which are useful for
             all derived instances.
             Note: This class should be the only one being used from outside.

 */


class SysView_ISysView 
{

public:

    /*-----------------------------------------------------------------------*/
    /*! @name Virtual functions */
    /*@{*/
    /*-----------------------------------------------------------------------*/

    /*!
       @brief   Creates columns. Has to be overloaded by its derived class.
       @return  none
     */
    virtual void	Create (void)
    {}

    /*!
       @brief   Returns no. of columns. Has to be overloaded by its derived 
              class.
       @return  [int] - always 0 (dummy)
     */
    virtual SAPDB_Int	GetColCount (void)
    {
        return(0); 
    }

    /*!
       @brief   Returns the estimated number of rows for this table. Has to be 
              overloaded by its derived class.
       @return  [int] - always 0 (dummy)
     */
    virtual SAPDB_Int	EstimateRows (void)
    {
        return(0); 
    }

    /*!
       @brief   Fills this table with information. Has to be overloaded by its 
              derived class.
       @return  none
     */
    virtual void	Execute (void)
    {}

    /*!
       @brief   Resets the counter for this specific SystemView. This method
                needs to be overwritten in the derived class for SystemViews,
                which has this feature.
       @return  none
     */
    virtual SAPDB_Bool	ResetCounter (void)
    { 
        return (false);
    }

    virtual ~SysView_ISysView()
    {}

    /*-----------------------------------------------------------------------*/
    /*@}*/
    /*-----------------------------------------------------------------------*/

protected:

    SysView_ISysView()
    {}

};


#endif  /* SYSVIEW_ISYSVIEW_HPP */

