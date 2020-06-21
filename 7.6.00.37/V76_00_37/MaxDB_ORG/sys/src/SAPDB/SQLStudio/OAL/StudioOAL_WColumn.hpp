/*

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

  module: StudioOAL_WColumn.hpp

  -----------------------------------------------------------------------

  responsible: WolfgangA, Hans-GeorgB

  special area: WebQuery, RepMan, DBAnalyzer

  description: Unicode Version of OAL

  version    : 7.3

  see also:

  -----------------------------------------------------------------------
 
  copyright:                Copyright (c) 2000-2005 SAP AG

************************************************************************/

#ifndef STUDIOOAL_WCOLUMN_HPP
#define STUDIOOAL_WCOLUMN_HPP

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include "SAPDB/SQLStudio/OAL/StudioOAL_WCommon.h"


/*===========================================================================*
 *    class StudioOAL_WColumn
 *===========================================================================*/
class StudioOAL_WColumn  
{
public:
	StudioOAL_WColumn();
	virtual ~StudioOAL_WColumn();

	//set column name
	void setColName(const StudioOAL_String& ColName){m_sColName = ColName;}
	//get column name
	void getColName(StudioOAL_String& ColName)      {ColName = m_sColName;}

	//set a value that indicates whether the column allows NULL values
	void setColNullable(SAPDB_Bool ColNullable) {m_bColNullable = ColNullable;}
	//get a value that indicates whether the column allows NULL values
	SAPDB_Bool getColNullable() { return m_bColNullable;}

	//sets the number of decimal digits of the column on the data source
	void setColDecimal(SQLSMALLINT nColDecimal) {m_nColDecimal = nColDecimal;}

	//set the size of the column on the data source
	void setColLength(SAPDB_Long lColLength){	m_lColLength = lColLength;}

	//get the size of the column on the data source
	SAPDB_Long  getColLength(){return m_lColLength;}

	//set if data is NULL on the data source
	void setColIsNull(SAPDB_Bool ColIsNull){m_bColIsNull = ColIsNull;}

	//set if data is NULL on the data source
	SAPDB_Bool getColIsNull() { return m_bColIsNull;}

	//set the sql (odbc) data type of the column
	void setODBCColType (SQLSMALLINT nODBCColType)	{m_enumODBCColType = nODBCColType;}

	/*!-------------------------------------------------------------------------
		function:     getODBCColType
		description:  return the sql (odbc) data type of the column
		arguments:	  /
					  
					  
		return value: odbc data type of the column
    --------------------------------------------------------------------------*/
	 SQLSMALLINT getODBCColType ()	{return m_enumODBCColType;}

   /*!-------------------------------------------------------------------------
        function:     setColValue
        description:  Set the value of the value attribute to the given value.
    --------------------------------------------------------------------------*/
	inline void setColValue( tsp81_UCS2Char *pValue ) { m_pValue = pValue;}

  /*!-------------------------------------------------------------------------
        function:     getColValue
        description:  get the value of the value attribute .
    --------------------------------------------------------------------------*/
	inline tsp81_UCS2Char  * getColValue( ) { return m_pValue; }

  /*!-------------------------------------------------------------------------
        function:     setValueLength
        description:  Set the value of the valueLength attribute to the given value.
     --------------------------------------------------------------------------*/
	 inline void setColValueLength ( SQLLEN ValueLength ) { m_ValueLength = ValueLength; }

 /*!-------------------------------------------------------------------------
    function:     getValueLength
    description:  get the value of the valueLength attribute 
  --------------------------------------------------------------------------*/
	 inline SQLLEN getColValueLength ( ) { return m_ValueLength ; }

	tsp81_UCS2Char  *m_pValue;
    SQLLEN		    m_ValueLength;

private:		

	//name of the column
	StudioOAL_String	m_sColName;        

	SQLSMALLINT		m_enumODBCColType;
	SAPDB_Long		m_lColLength;       
	SQLSMALLINT		m_nColDecimal;

	//private properties
	SAPDB_Bool		m_bColNullable;
	SAPDB_Bool		m_bColIsNull;
	SAPDB_Bool		m_bColIsKey;
	SAPDB_Bool		m_bColIsUpdatable;


};
#endif

/*===========================================================================*
 *  END
 *===========================================================================*/
