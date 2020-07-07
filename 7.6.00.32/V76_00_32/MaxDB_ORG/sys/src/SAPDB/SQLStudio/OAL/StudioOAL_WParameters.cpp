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

  module: StudioOAL_WResult.cpp

  -----------------------------------------------------------------------

  responsible: WolfgangA, Hans-GeorgB

  special area: WebQuery, RepMan, DBAnalyzer

  description:  Unicode Version of OAL

  version     : 7.3


************************************************************************/


#include "SAPDB/SQLStudio/OAL/StudioOAL_WParameters.hpp"


/*===========================================================================*
 *    StudioOAL_WParameters::StudioOAL_WParameters
 *===========================================================================*/
StudioOAL_WParameters::StudioOAL_WParameters(SQLHANDLE   hStmt,
										     SAPDB_Int2  nParameterNo)
{
	m_hStmt           = hStmt;
	m_nParameterNo    = nParameterNo;
	m_emParameterType = ptUnknown;
	m_lValueLength    = SQL_DATA_AT_EXEC;
	memset( m_pValue,0,MX_BYTEVALUE );
}
/*===========================================================================*
 *    StudioOAL_WParameters::~StudioOAL_WParameters
 *===========================================================================*/
StudioOAL_WParameters::~StudioOAL_WParameters()
{
}
/*===========================================================================*
 *    StudioOAL_WParameters::bindParameter
 *===========================================================================*/
SAPDB_Bool StudioOAL_WParameters::bindParameter()
{
	//#define SQL_NTS                   (-3)
	SAPDB_Int2 lRC = 0;

	if(m_emParameterType == ptInOut || m_emParameterType == ptOut )
	{

		switch (m_nColumnType)
		{

			case SQL_BINARY:
			case SQL_VARBINARY:
			case SQL_LONGVARBINARY:
				  lRC = SQLBindParameter(m_hStmt,
										 m_nParameterNo,
										 m_emParameterType, 
										 SQL_BINARY,
										 m_nColumnType,
										 0,
										 0,
										 m_pValue, 
										 MX_BYTEVALUE,
										 &m_lValueLength); //!!IA64
			break;
			default:
				  m_lValueLength = SQL_NTS;
				  lRC = SQLBindParameter(m_hStmt,
										 m_nParameterNo,
										 m_emParameterType, 
										 SQL_WCHAR,
										 m_nColumnType, 
										 0,
										 0,
										 m_pValue, 
										 MX_BYTEVALUE, 
										 &m_lValueLength); //!!IA64
		}
	}			  
    else
	{
		switch (m_nColumnType)
		{

			case SQL_BINARY:
			case SQL_VARBINARY:
			case SQL_LONGVARBINARY:
			  lRC = SQLBindParameter(m_hStmt,
									 m_nParameterNo,
									 SQL_PARAM_INPUT, 
									 SQL_BINARY, 
									 m_nColumnType, 
									 0,
									 0,
									 &m_nParameterNo,
									 0, 
									 &m_lValueLength); //!!IA64
			break;
			default:
			  lRC = SQLBindParameter(m_hStmt,
									 m_nParameterNo,
									 SQL_PARAM_INPUT, 
									 SQL_WCHAR,
									 m_nColumnType, 
									 0,
									 0,
									 &m_nParameterNo,
									 0, 
									 &m_lValueLength); //!!IA64
		  }
	}
   
    if( lRC != SQL_SUCCESS ) 
		return SAPDB_FALSE;

	return SAPDB_TRUE;

}
/*===========================================================================*
 *    StudioOAL_WParameters::bindSQLParameter
 *===========================================================================*/
SAPDB_Bool StudioOAL_WParameters::bindSQLParameter()
{
	SAPDB_Int2 lRC = 0;

	m_lValueLength = SQL_NTS;

	if(m_emParameterType == ptIn  )
	{
		switch (m_nColumnType)
		{

			case SQL_BINARY:
			case SQL_VARBINARY:
			case SQL_LONGVARBINARY:
				  lRC = SQLBindParameter(m_hStmt,
										 m_nParameterNo,
										 SQL_PARAM_INPUT, 
										 SQL_C_BINARY,
										 m_nColumnType,
										 0,
										 0,
										 m_pValue, 
										 0,
										 &m_lValueLength); //!!IA64
			break;
			default:
				  lRC = SQLBindParameter(m_hStmt,
										 m_nParameterNo,
										 SQL_PARAM_INPUT, 
										 SQL_C_WCHAR,
										 m_nColumnType, 
										 0,
										 0,
										 m_pValue, 
										 0, 
										 &m_lValueLength); //!!IA64
		}
	}
	
    if( lRC != SQL_SUCCESS ) return SAPDB_FALSE;

	return SAPDB_TRUE;

}
/*===========================================================================*
 *    StudioOAL_WParameters::setValue
 *===========================================================================*/
void StudioOAL_WParameters::setValue (StudioOAL_String strValue) 
{

	tsp81_UCS2Char * pConvertOut = NULL;

	
   	/*-------------------------------------------------------------------------*
     * init conversion buffers								 			       *
     *-------------------------------------------------------------------------*/
	memset(m_pValue,0,sizeof(m_pValue));
		
	/*-------------------------------------------------------------------------*
     * convert utf8 to ucs2									 			       *
     *-------------------------------------------------------------------------*/
	strValue.ConvertToUCS2_Unaligned_Swap(m_pValue,
							             (m_pValue+(sizeof(m_pValue)/sizeof(SAPDB_UCS2))),
							              pConvertOut) ;

	setValueLength((SAPDB_Long) (pConvertOut-m_pValue) );

}
/*===========================================================================*
 *    StudioOAL_WParameters::getValue
 *===========================================================================*/
void StudioOAL_WParameters::getValue (StudioOAL_String &strValue)
{
	strValue.Erase();
	
   /*-------------------------------------------------------------------------*
    * convert ucs2 to utf8 								 			          *
    *-------------------------------------------------------------------------*/
	if(m_lValueLength > 0){

		strValue.ConvertFromUCS2_Unaligned_Swap(m_pValue, m_pValue + m_lValueLength );        

	}

	//SQL_NTS. The parameter value is a null-terminated string. 
	if(m_lValueLength == SQL_NTS){

		strValue.ConvertFromUCS2_Unaligned_Swap( m_pValue, m_pValue + _tcslen((const _TCHAR *) m_pValue) );

	}

}


/*===========================================================================*
 *    End
 *===========================================================================*/