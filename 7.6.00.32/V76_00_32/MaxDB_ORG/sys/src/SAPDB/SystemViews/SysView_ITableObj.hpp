/*!
  @file           SysView_ITableObj.hpp
  @author         ElkeZ
  @ingroup        SystemViews
  @brief   This contains an abstract interface for the system view 
              table object class.
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



#ifndef SYSVIEW_ITABLEOBJ_HPP
#define SYSVIEW_ITABLEOBJ_HPP


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SystemViews/SysView_Defines.hpp"

 /*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

struct tak40_show_glob;
struct tgg00_Rec;

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!
   @interface  SysView_ITableObj
   @brief      This interface class provides an interface for the system view 
             table object class.
 */


class SysView_ITableObj
{

public:


    /*-----------------------------------------------------------------------*/
    /*! @name Constructors and initialization */
    /*@{*/
    /*-----------------------------------------------------------------------*/

    /*!
       @brief   Class destructor. Is not used in the moment.
       @return  none
     */
    virtual ~SysView_ITableObj()
    {}

    /*-----------------------------------------------------------------------*/
    /*@}*/
    /*-----------------------------------------------------------------------*/

    /*!
       @brief   Returns the underlying pascal object. Should be avoided!
       @return  [tak40_show_glob] - AKA a41v
     */
    virtual tak40_show_glob * GetPascalObjPtr() = 0;

    /*-----------------------------------------------------------------------*/
    /*! @name Table creation */
    /*@{*/
    /*-----------------------------------------------------------------------*/

    /*!
      @brief      These functions append a column to the table object. The
                  type depends on the first parameter.
      @param  iType        Type of the column to be added.
      @param  sColName     Column title
      @param  iColSize     Size of the column in bytes
      @return none
    */
    virtual void AppendCol (SysView_ITOColType  iType,
                            const char          *sColName, 
                            int                 iColSize) = 0;
    
    /*-----------------------------------------------------------------------*/
    /*@}*/
    /*-----------------------------------------------------------------------*/

    /*-----------------------------------------------------------------------*/
    /*! @name Table access */
    /*@{*/
    /*-----------------------------------------------------------------------*/

    /*!
       @brief   This function initialises the table catalog. Must be called
              before you can add data to columns.
       @return  none
     */
    virtual void GetCatalogTable ( void ) = 0;

    /*-----------------------------------------------------------------------*/
    /*@}*/
    /*-----------------------------------------------------------------------*/

    /*-----------------------------------------------------------------------*/
    /*! @name Table information */
    /*@{*/
    /*-----------------------------------------------------------------------*/

    /*!
       @brief   This function returns the showkind of the table object.
       @return  [SAPDB_Int4] - showkind
     */
    virtual SAPDB_Int4 GetShowKind( void ) = 0;

    /*-----------------------------------------------------------------------*/
    /*@}*/
    /*-----------------------------------------------------------------------*/

    /*-----------------------------------------------------------------------*/
    /*! @name Table filling */
    /*@{*/
    /*-----------------------------------------------------------------------*/

    /*!
       @brief   This function has many overloads and moves a variable of
              any kind to a table column.
       @return  none
     */
    virtual void MoveToCol ( const SysView_ITOVarType iType,
                             const void               *pVal,
                             const SAPDB_Int          iLength) = 0;

    /*!
       @brief   This function moves a long record to a long col container, i.e. stores a long value into
                a sequence of rows.
       @param   pBuf pointer to the buffer containing the value
       @param   bufLength the length of the value in bytes
       @param   londId the long id used to store the value
       @return  none
     */
    virtual void MoveToLongColContainer ( const void* pBuf, int bufLenght) = 0;
    
    /*-----------------------------------------------------------------------*/
    /*@}*/
    /*-----------------------------------------------------------------------*/

protected:

    /*!
       @brief   This constructor ensures that no-one will instanciate this
                class.
       @return  none
     */
	SysView_ITableObj()
    {}


};


#endif  /* SYSVIEW_ITABLEOBJ_HPP */

