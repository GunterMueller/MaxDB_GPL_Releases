/*
    Copyright (c) 2000-2005 SAP AG

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA



    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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


/*!**********************************************************************

  module: StudioOAL_WParameters.hpp

  -----------------------------------------------------------------------

  responsible: WolfgangA, Hans-GeorgB

  special area: WebQuery, RepMan, DBAnalyzer

  description:  Unicode Version of OAL

  version     : 7.3


************************************************************************/

#ifndef StudioOAL_WPARAMETERS_HPP
#define StudioOAL_WPARAMETERS_HPP

#include	  "SAPDB/SQLStudio/OAL/StudioOAL_WCommon.h"
#include	  "SAPDB/SAPDBCommon/SAPDB_Types.hpp"

/*

SQLRETURN SQLBindParameter(
     SQLHSTMT         StatementHandle,
     SQLUSMALLINT     ParameterNumber,
     SQLSMALLINT      InputOutputType,
     SQLSMALLINT      ValueType,
     SQLSMALLINT      ParameterType,
     SQLUINTEGER      ColumnSize,
     SQLSMALLINT      DecimalDigits,
     SQLPOINTER       ParameterValuePtr,
     SQLINTEGER       BufferLength,
     SQLINTEGER *     StrLen_or_IndPtr);

*/


/*===========================================================================*
 *    class StudioOAL_WParameters
 *===========================================================================*/
class StudioOAL_WParameters
{
public:
		/*!-------------------------------------------------------------------------
		  function:     StudioOAL_WParameters
		  description:  Constructor
		  arguments:	[in] hStmt, handle of param statement
						[in] nParameterNo, number of param
		  										
      
		  return value: / 
		--------------------------------------------------------------------------*/
		StudioOAL_WParameters(SQLHANDLE	hStmt,SAPDB_Int2  nParameterNo);

		 /*!-------------------------------------------------------------------------
		  function:     ~StudioOAL_WParameters
		  description:  Destructor
		  arguments:    /

		  return value: / 
		--------------------------------------------------------------------------*/
		~StudioOAL_WParameters();


		enum  emParameterType{

		  ptUnknown = 0,
		  ptIn = 1,
		  ptInOut = 2,
		  ptResultCol = 3,
		  ptOut = 4,
		  ptReturnValue = 5
		};

		/*!-------------------------------------------------------------------------
		  function:     setParameterType
		  description:  set the paramtype of parameter
		  arguments:    [in] emParameterType, paramtype of parameter

		  return value: / 
		--------------------------------------------------------------------------*/
		void setParameterType(emParameterType parameterType){m_emParameterType = parameterType;}

		/*!-------------------------------------------------------------------------
		  function:     getParameterType
		  description:  get the type of the parameter
		  arguments:    /

		  return value: m_emParameterType,type of parameter
		--------------------------------------------------------------------------*/
		SAPDB_Int2 getParameterType(){return m_emParameterType;}

		/*!-------------------------------------------------------------------------
		  function:     setColumnType
		  description:  set the column type of parameter
		  arguments:    [in] nColumnType, columntype of parameter

		  return value: / 
		--------------------------------------------------------------------------*/
		void setColumnType(SAPDB_Int2 nColumnType){m_nColumnType = nColumnType;}

		/*!-------------------------------------------------------------------------
		  function:     getColumnType
		  description:  get the column type of parameter
		  arguments:    /

		  return value: m_nColumnType, columntype of parameter 
		--------------------------------------------------------------------------*/
		SAPDB_Int2 getColumnType(){return m_nColumnType;}
		
		/*!-------------------------------------------------------------------------
		  function:     getParameterNo
		  description:  returns parameter number
		  arguments:    /

		  return value: m_nParameterNo
		--------------------------------------------------------------------------*/
		SAPDB_Int2 getParameterNo ()	{return m_nParameterNo;}

		/*!-------------------------------------------------------------------------
		  function:     setValue
		  description:  set paramvalue
		  arguments:    sValue, reference to string class

		  return value: /
		-------------------------------------------------------------------------*/
		void setValue ( StudioOAL_String strValue) ;

		/*!-------------------------------------------------------------------------
		  function:     getValue
		  description:  get paramvalue
		  arguments:    sValue, reference to string class

		  return value: /
		-------------------------------------------------------------------------*/
		void getValue (StudioOAL_String &strValue);

		/*!-------------------------------------------------------------------------
		  function:     setValueLength
		  description:  set paramvaluelength
		  arguments:    lValueLength

		  return value: /
		-------------------------------------------------------------------------*/
		inline void setValueLength (SAPDB_Long lValueLength) { m_lValueLength = lValueLength;}

		/*!-------------------------------------------------------------------------
		  function:     getValueLength
		  description:  get paramvaluelength
		  arguments:    sValue, reference to string class

		  return value: /
		-------------------------------------------------------------------------*/
		inline SAPDB_Long getValueLength ( ) {  return m_lValueLength;}

		/*!-------------------------------------------------------------------------
		  function:     bindParameter
		  description:  bind parameters
		  arguments:    /

		  return value: true if success
		--------------------------------------------------------------------------*/
		SAPDB_Bool bindParameter();
		SAPDB_Bool bindSQLParameter();
		
private:

	SQLHANDLE  m_hStmt;
	SAPDB_Int2 m_nParameterNo;
	SAPDB_Int2 m_nColumnType;

	emParameterType m_emParameterType;	

	tsp81_UCS2Char m_pValue[MX_BYTEVALUE];
	SAPDB_Long     m_lValueLength;

};

#endif

/*===========================================================================*
 *  END
 *===========================================================================*/

