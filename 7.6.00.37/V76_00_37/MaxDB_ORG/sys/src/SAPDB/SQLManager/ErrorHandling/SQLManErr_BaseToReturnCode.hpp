/*!
  @file           SQLManErr_BaseToReturnCode.hpp
  @author         ElkeZ
  @brief          combination of base error, sqlmode and resulting returncode

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

#ifndef SQLMANERR_BASETORETURNCODE_HPP
#define SQLMANERR_BASETORETURNCODE_HPP


/*===========================================================================*
 *  INCLUDES                                                                 *
 **/

#include "SAPDBCommon/SAPDB_Types.h"
#include "gsp00.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 **/


/*===========================================================================*
 *  MACROS                                                                   *
 **/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 **/

/*!
  @class          SQLManErr_BaseToReturnCode
  @brief          combination of base error, sqlmode and resulting returncod
 */
class SQLManErr_BaseToReturnCode
{
public:

/*!
    @name Constructors
*/

    /*!
       @brief     Constructor
       @param     baseError [in] base error
       @param     returnCode [in] returncode corresponding to base error and sqlmode
       @param     sqlmodeDependent [in] if there are different returncodes/sqlmode
       @param     sqlmode [in] sqlmode resp. default sqlm_internal
       @param     errorIndex [in] arrayindex in the arry of SQLManErrorObject
        
        
    */
    SQLManErr_BaseToReturnCode
    (const SAPDB_Int2   baseError,
     const SAPDB_Int2   returnCode,
     const SAPDB_Bool1  sqlmodeDependent,
     const tsp00_Byte   sqlmode,
	 const SAPDB_Int2   errorIndex)
    :
    m_BaseError(baseError), 
    m_ReturnCode(returnCode),  
	m_ErrorIndex(errorIndex),
    m_SqlmodeDependent(sqlmodeDependent),
    m_Sqlmode(sqlmode)
    {}

    /*!
       @brief     Constructor
    */
    SQLManErr_BaseToReturnCode()
    {}

    /*!
       @brief          Compares with other SQLManErr_BaseToReturnCode
       @param          baseret [in] other SQLManErr_BaseToReturnCode
       @return         (SAPDB_Bool) true if unequal
     */
    SAPDB_Bool operator!=(const SQLManErr_BaseToReturnCode &baseret) const
    {
        return !
            (
            (m_BaseError        == baseret.m_BaseError)        &&
            (m_SqlmodeDependent == baseret.m_SqlmodeDependent) &&
            (m_Sqlmode          == baseret.m_Sqlmode         ) &&
            (m_ReturnCode       == baseret.m_ReturnCode)
            );
    }

    /*!
       @brief          Compares with other SQLManErr_BaseToReturnCode
       @param          baseret [in] other SQLManErr_BaseToReturnCode
       @return         (SAPDB_Bool) true if equal
     */
    SAPDB_Bool operator==(const SQLManErr_BaseToReturnCode &baseret) const
    {
        return 
            (
            (m_BaseError        == baseret.m_BaseError)        &&
            (m_SqlmodeDependent == baseret.m_SqlmodeDependent) &&
            (m_Sqlmode          == baseret.m_Sqlmode         ) &&
            (m_ReturnCode       == baseret.m_ReturnCode)
            );
    }

    /*!
       @brief          Compares with other SQLManErr_BaseToReturnCode
       @param          baseret [in] other SQLManErr_BaseToReturnCode
       @return         (SAPDB_Bool) true if less or equal
     */
    SAPDB_Bool operator<=(const SQLManErr_BaseToReturnCode &baseret) const
    {
        return 
            (
            (m_BaseError        <= baseret.m_BaseError) 
            ||
            (
            (m_BaseError        == baseret.m_BaseError)        &&
            (m_SqlmodeDependent <= baseret.m_SqlmodeDependent)
            )
            ||
            (
            (m_BaseError        == baseret.m_BaseError)        &&
            (m_SqlmodeDependent == baseret.m_SqlmodeDependent) &&
            (m_Sqlmode          <= baseret.m_Sqlmode         ) 
            )
            );
    }

    /*!
       @brief          Compares with other SQLManErr_BaseToReturnCode
       @param          baseret [in] other SQLManErr_BaseToReturnCode
       @return         (SAPDB_Bool) true if less
     */
    SAPDB_Bool operator<(const SQLManErr_BaseToReturnCode &baseret) const
    {
        return 
            (
            (m_BaseError        < baseret.m_BaseError) 
            ||
            (
            (m_BaseError        == baseret.m_BaseError)        &&
            (m_SqlmodeDependent <  baseret.m_SqlmodeDependent)
            )
            ||
            (
            (m_BaseError        == baseret.m_BaseError)        &&
            (m_SqlmodeDependent == baseret.m_SqlmodeDependent) &&
            (m_Sqlmode          <  baseret.m_Sqlmode         ) 
            )
            );
    }

    /*!
       @brief          Compares with other SQLManErr_BaseToReturnCode
       @param          baseret [in] other SQLManErr_BaseToReturnCode
       @return         (SAPDB_Bool) true if greater or equal
     */
    SAPDB_Bool operator>=(const SQLManErr_BaseToReturnCode &baseret) const
    {
        return
            (
            (m_BaseError        >= baseret.m_BaseError) 
            ||
            (
            (m_BaseError        == baseret.m_BaseError)        &&
            (m_SqlmodeDependent >= baseret.m_SqlmodeDependent)
            )
            ||
            (
            (m_BaseError        == baseret.m_BaseError)        &&
            (m_SqlmodeDependent == baseret.m_SqlmodeDependent) &&
            (m_Sqlmode          >= baseret.m_Sqlmode         ) 
            )
            );
    }

    /*!
       @brief          Compares with other SQLManErr_BaseToReturnCode
       @param          baseret [in] other SQLManErr_BaseToReturnCode
       @return         (SAPDB_Bool) true if greater
     */
    SAPDB_Bool operator>(const SQLManErr_BaseToReturnCode &baseret) const
    {
        return
            (
            (m_BaseError        >  baseret.m_BaseError) 
            ||
            (
            (m_BaseError        == baseret.m_BaseError)        &&
            (m_SqlmodeDependent >  baseret.m_SqlmodeDependent)
            )
            ||
            (
            (m_BaseError        == baseret.m_BaseError)        &&
            (m_SqlmodeDependent == baseret.m_SqlmodeDependent) &&
            (m_Sqlmode          >  baseret.m_Sqlmode         ) 
            )
            );
    }

    /*!
       @brief        get base error
       @return       (SAPDB_Int2) base error
    */
    SAPDB_Int2  GetBaseError() const {
        return m_BaseError;
    }
    /*!
       @brief        get returncode 
       @return       (SAPDB_Int2) returncode
    */
    SAPDB_Int2  GetReturncode() const {
        return m_ReturnCode;
    }
    /*!
       @brief        get info about sqlmode dependency
       @return       (SAPDB_Bool1) sqlmode-dependency
    */
    SAPDB_Bool1  GetSqlmodeDependant() const {
        return m_SqlmodeDependent;
    }
    /*!
       @brief        get sqlmode
       @return       (tsp00_Byte) sqlmode
    */
    tsp00_Byte  GetSqlmode() const {
        return m_Sqlmode;
    }
    /*!
       @brief        get index in errors
       @return       (SAPDB_Int2) error index
    */
    SAPDB_Int2  GetErrorIndex() const {
        return m_ErrorIndex;
    }

private:

    SAPDB_Int2          m_BaseError; 
    SAPDB_Int2          m_ReturnCode;  
	SAPDB_Int2          m_ErrorIndex;
    SAPDB_Bool1         m_SqlmodeDependent;
    tsp00_Byte          m_Sqlmode; // tsp00_sqlmode not possible because of NT-compiler, JörgM tried hard,
                                   // but did not find any solution

};


#endif  /* SQLMANERR_BASETORETURNCODE_HPP */
