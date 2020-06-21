/*!
  @file           SysView_SysViewBase.hpp
  @author         ElkeZ,JoergM
  @ingroup        SystemViews
  @brief   This contains the basic functionality for all system views.

\if EMIT_LICENCE

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



\endif
*/

#ifndef SYSVIEW_SYSVIEWBASE_HPP
#define SYSVIEW_SYSVIEWBASE_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "SystemViews/SysView_ISysView.hpp"
#include "SystemViews/SysView_ITableObj.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

struct tak40_show_glob;

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

class SQLMan_Context;
class SQLMan_Identifier;
class Catalog_ObjectId;

/*!
   @class    SysView_SysViewBase
   @brief    This class is designed to be an interface class for all system
             views. However, this class is a bit more than that, since it
             also provides some additional functions which are useful for
             all derived instances.
             Note: This class should be the only one being used from outside.
 */


class SysView_SysViewBase : public SysView_ISysView
{

public:

    /*-----------------------------------------------------------------------*/
    /*! @name Constructors and initialization */
    /*@{*/
    /*-----------------------------------------------------------------------*/

    /*!
       @brief   The only real existing constructor (all derived classes just 
              have overload constructors which call this one again) which 
              just sets some local variables.
       @param acv [in] SQL manager context
       @param a41v [in] show command context
       @return  none
     */
    SysView_SysViewBase( SQLMan_Context     &acv,
                         tak40_show_glob    &a41v);

    /*!
       @brief   Class destructor. Is not used in the moment.
       @return  none
     */
    virtual ~SysView_SysViewBase();

    /*-----------------------------------------------------------------------*/
    /*@}*/
    /*-----------------------------------------------------------------------*/

    /*-----------------------------------------------------------------------*/
    /*! @name Virtual functions */
    /*@{*/
    /*-----------------------------------------------------------------------*/

    /*!
       @brief   Creates columns. Has to be overloaded by its derived class.
       @return  none
     */
    virtual void    Create (void)
    {}

    /*!
       @brief   Returns no. of columns. Has to be overloaded by its derived class.
       @return    int  always 0 (dummy)
     */
    virtual SAPDB_Int   GetColCount (void)
    { return(0); }

    /*!
       @brief   Returns the estimated number of rows for this table. Has to be 
              overloaded by its derived class.
       @return int     always 0 (dummy)
     */
    virtual SAPDB_Int   EstimateRows (void)
    { return(0); }

    /*!
       @brief   Fills this table with information. Has to be overloaded by its 
              derived class.
       @return  none
     */
    virtual void    Execute (void);

    /*!
       @brief   Resets the counter for this specific SystemView. 
                Has to be overloaded by its derived class.
                \Attention This method has to be implemented only for NON-Resetable views.
                This has the reason that a command "reset logstatisiticsreset" sounds
                strange in opposite to "reset logstatistics". So the implementation
                of the ResetCounter of a class SysView_SomeClassReset is done in
                SysView_SomeClass.

       @return  none
     */
    virtual SAPDB_Bool ResetCounter (void);

    /*-----------------------------------------------------------------------*/
    /*@}*/
    /*-----------------------------------------------------------------------*/

protected:

    /*------------------------------------------------------------------------*/
    /*! @name Helper function to add column content in Execute implementation */
    /*@{*/
    /*------------------------------------------------------------------------*/
    /*!
       @brief   Add into table a NIL value column
       @return  none
     */
    void AddColumn()
    {
        m_Table->MoveToCol (ITOVT_NULL, 0, 0);
    }

    /*!
      @brief   Add into table a SAPDB_Int2 value column
      @param int2Value [in] value to add
     */
    void AddColumn(SAPDB_Int2 int2Value)
    {
        m_Table->MoveToCol (ITOVT_INT2, &int2Value, 0);
    }

    /*!
       @brief   Add into table a SAPDB_Int4 value column
       @param int4Value [in] value to add
     */
    void AddColumn(SAPDB_Int4 int4Value )
    {
        m_Table->MoveToCol (ITOVT_INT4, &int4Value, 0);
    }

    /*!
       @brief   Add into table a SAPDB_UInt4 value column
       @param uint4Value [in] value to add
     */
    void AddColumn(SAPDB_UInt4 uint4Value )
    {
        m_Table->MoveToCol (ITOVT_UINT4, &uint4Value, 0);
    }

    /*!
       @brief   Add into table a SAPDB_Int8 value column
       @param int8Value [in] value to add
       @return  none
     */
    void AddColumn(SAPDB_Int8 int8Value )
    {
        m_Table->MoveToCol (ITOVT_INT8, &int8Value, 0);
    }

    /*!
       @brief   Add into table a SAPDB_UInt8 value column
       @param uint8Value [in] value to add
       @return  none
     */
    void AddColumn(SAPDB_UInt8 uint8Value )
    {
        m_Table->MoveToCol (ITOVT_UINT8, &uint8Value, 0);
    }

    /*!
       @brief   Add into table a variable length string value column

       The given zero terminated C_string is filled in, using the streln() function to calculate its length
       @param valueString [in] the zero terminated string to fill in
       @return  none
     */
    void AddColumn(SAPDB_Char const *valueString)
    {
        m_Table->MoveToCol (ITOVT_CHARPTR, const_cast<SAPDB_Char *>(valueString), (SAPDB_Int4)strlen(valueString));
    }

    /*!
       @brief   Add into table a fixed length string value column

       The given string is filled in, using given length
       @param valueString [in] the zero terminated string to fill in
       @param stringLengthInBytes [in] the length in bytes
       @return  none
     */
    void AddColumn(SAPDB_Char const *valueString, SAPDB_Int4 stringLengthInBytes)
    {
        m_Table->MoveToCol (ITOVT_CHARPTR, const_cast<SAPDB_Char *>(valueString), stringLengthInBytes);
    }

    const SAPDB_Int4 AsTimestamp;

    /*!
       @brief   Add into table a timestamp value column

       Without the addiotinal parameter the AddColumn() function could not decide to convert into a timestamp
       first.

       A timestamp value of '0' is inserting a NIL value.

       @param timeStamp [in] the timestamp to show
       @param flagAsTimestamp [in] flag to indicatre it is a timestamp and not just a SAPDB_UInt8
     */
    void AddColumn(SAPDB_UInt8 timeStamp, const SAPDB_Int4 flagAsTimestamp);

    /*!
        @brief Add integer date and time timestamps
        @param date [in]
        @param time [in]
        
        If date or time is 0 then a NULL value is added.
    */
    void AddColumn(SAPDB_UInt4 date, SAPDB_UInt4 time);

    /*!
        @brief Adds an identifier to the current row
    */
    void AddColumn(const SQLMan_Identifier& Identifier);

    void AddProcedureSchemaAndName (const Catalog_ObjectId& DBProcId);

    /*-----------------------------------------------------------------------*/
    /*@}*/
    /*-----------------------------------------------------------------------*/

    SysView_ITableObj *m_Table; //!< pointer to the meta data describing the current system table 
    SQLMan_Context    &m_Context; //!< current sql manager context 

};


#endif  /* SYSVIEW_SYSVIEWBASE_HPP */

