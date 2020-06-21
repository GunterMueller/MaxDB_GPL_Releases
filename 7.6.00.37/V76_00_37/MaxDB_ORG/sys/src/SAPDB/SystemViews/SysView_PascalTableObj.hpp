/*!
  @file           SysView_PascalTableObj.hpp
  @author         ElkeZ
  @ingroup   SystemViews
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



#ifndef SYSVIEW_PASCALTABLEOBJ_HPP
#define SYSVIEW_PASCALTABLEOBJ_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "ggg00.h"
#include "gak40.h"
#include "vak001.h"
#include "gkb05.h"
#include "SAPDBCommon/SAPDB_UTF8Basis.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SystemViews/SysView_Defines.hpp"
#include "SystemViews/SysView_ITableObj.hpp"

 /*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*! @brief constant used with MoveToCol to show that the value is the NULL-value */
const pasbool						C_IS_UNDEF      = true;

/*! 
    @brief constant used with MoveToCol to show that 0 has to be handled 
    like the NULL-value 
*/
const pasbool						C_ZERO_AS_UNDEF = true;

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

class SQLMan_Context;

/*!
   @class    SysView_PascalTableObj
   @brief    This interface class provides an interface for the SystemView
             table object.
 */


class SysView_PascalTableObj : public SysView_ITableObj
{

public:

    /*-----------------------------------------------------------------------*/
    /*! @name Constructors and initialization */
    /*@{*/
    /*-----------------------------------------------------------------------*/

    /*!
       @brief   
       @return  none
     */
    SysView_PascalTableObj( SQLMan_Context  &context,
                            tak40_show_glob &a41v);

    /*!
       @brief   Class destructor. Is not used in the moment.
       @return  none
     */
    ~SysView_PascalTableObj()
    {}


    /*-----------------------------------------------------------------------*/
    /*@}*/
    /*-----------------------------------------------------------------------*/

    /*!
       @brief   Returns the underlying pascal object. Should be avoided!
       @return  [tak40_show_glob] - AKA a41v
     */
    tak40_show_glob * GetPascalObjPtr();

    /*-----------------------------------------------------------------------*/
    /*! @name Table creation */
    /*@{*/
    /*-----------------------------------------------------------------------*/

    /*!
      @brief   This function append a column to the table object. The
               type depends on the first parameter.
      @param iType        Type of the column to be added.
      @param sColName     Column title
      @param iColSize     Size of the column in bytes
      @return none
    */
    void AppendCol (SysView_ITOColType  iType,
                    const char          *sColName, 
                    int                 iColSize);
    
    /*-----------------------------------------------------------------------*/
    /*@}*/
    /*-----------------------------------------------------------------------*/

    /*-----------------------------------------------------------------------*/
    /*! @name Table access */
    /*@{*/
    /*-----------------------------------------------------------------------*/

    /*!
       @brief    This function initialises the table catalog. Must be called
                 before you can add data to columns.
       @return   none
     */
    void GetCatalogTable ( void );


    /*-----------------------------------------------------------------------*/
    /*@}*/
    /*-----------------------------------------------------------------------*/

    /*-----------------------------------------------------------------------*/
    /*! @name Table information */
    /*@{*/
    /*-----------------------------------------------------------------------*/

    /*!
       @brief   This function returns the showkind of the table object.
       @return  none
     */
    SAPDB_Int4 GetShowKind( void );

    /*-----------------------------------------------------------------------*/
    /*@}*/
    /*-----------------------------------------------------------------------*/

    /*-----------------------------------------------------------------------*/
    /*! @name Table filling */
    /*@{*/
    /*-----------------------------------------------------------------------*/

    void MoveToCol ( const SysView_ITOVarType iType,
                     const void               *pVal,
                     const SAPDB_Int          iLength);

   /*!
       @brief   This function moves a long record to a long col container, i.e. stores a long value into
                a sequence of rows.
       @param   pBuf pointer to the buffer containing the value
       @param   bufLength the length of the value in bytes
       @return  none
     */
    void MoveToLongColContainer ( const void* pBuf, int bufLenght);

    /*-----------------------------------------------------------------------*/
    /*@}*/
    /*-----------------------------------------------------------------------*/

private:

    /*-----------------------------------------------------------------------*/
    /*! @name Private tool functions */
    /*@{*/
    /*-----------------------------------------------------------------------*/

    /*!
       @brief   This function has many overloads and moves a variable of
              any kind to a table column.
       @return  none
     */
    void MoveInt8 ( SAPDB_Int8 &val );

    void MoveUTF8Ptr ( SAPDB_UTF8 *val,
                       SAPDB_Int  length );

    void MoveTransNo ( tgg91_TransNo &val );

    void MoveTransLockMode ( tkb05_TransLockMode_Enum &val );

    void MoveParseID ( tak_parsid &val );

    void MoveSQLMode ( tsp00_SqlMode_Enum &val );

    void MoveDateTimeFormat ( tgg00_DateTimeFormat_Enum &val ); 

    void MoveCodeType ( tsp00_CodeType &val ); 

    void MoveBool ( SAPDB_Bool &val );

    void MoveYesNo ( SAPDB_Bool &val );

    void MoveOnOff ( SAPDB_Bool &val );

    void MoveQueryRewriteMode ( tak_queryrewrite_mode_Enum &val );

    int HandleUTF8Error(SAPDB_UTF8Basis::ConversionResult &res);

    /*-----------------------------------------------------------------------*/
    /*@}*/
    /*-----------------------------------------------------------------------*/

    /*-----------------------------------------------------------------------*/
    /*! @name Local Variables */
    /*@{*/
    /*-----------------------------------------------------------------------*/

    SQLMan_Context      &m_Context;
    tak40_show_glob     &m_Show;

    /*-----------------------------------------------------------------------*/
    /*@}*/
    /*-----------------------------------------------------------------------*/

};


#endif  /* SYSVIEW_PASCALTABLEOBJ_HPP */

