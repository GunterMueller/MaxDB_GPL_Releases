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

  module: StudioOAL_WError.hpp

  -----------------------------------------------------------------------

  responsible: WolfgangA, Hans-GeorgB

  special area: WebQuery, RepMan, DBAnalyzer

  description: Unicode Version of OAL

  version    : 7.3

  see also:

  -----------------------------------------------------------------------
 
  copyright:                Copyright (c) 2000-2005 SAP AG

************************************************************************/

#ifndef STUDIOOAL_WERROR_HPP
#define STUDIOOAL_WERROR_HPP

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/
#include <WINDOWS.H>
#include <sql.h>		// Main include for ODBC core functions
#include <sqlext.h>     // Used for applications using Microsoft SQL Extensions
#include <sqlucode.h>	// Unicode Types for ODBC

#include "SAPDB/SQLStudio/OAL/StudioOAL_WCommon.h"
#include "SAPDBCommon/SAPDB_Types.hpp"



/*===========================================================================*
 *    class StudioOAL_WError
 *===========================================================================*/
class StudioOAL_WError  
{
public:
	StudioOAL_WError(SQLHANDLE hDhc,SQLHENV hEnv);
	virtual ~StudioOAL_WError(){;}


//Variables
private:

	//EnvironmentHandle
	SQLHENV		m_hEnv ;

	//ConnectionHandle
	SQLHDBC		m_hDbc;
	

	//error object contains an error
	SAPDB_Bool		m_bIsError;  

	//contains a warning
	SAPDB_Bool		m_bIsWarning;

	//sql return code
	SQLRETURN	m_lRc;

	//position of the error in sql statement
	SAPDB_Int4		m_lErrPos;

	//native error number
	SQLINTEGER		m_nNativeError;

	StudioOAL_String		m_strNativeError;
	StudioOAL_String		m_strErrorText;
	StudioOAL_String		m_strSQLState;
	StudioOAL_String		m_strSQLStateErrorNumber;
	
	//if you want a SQLState to be ignored set member
	StudioOAL_String		m_strNoSQLState;

	//if you want a NativeError to be ignored set member
	StudioOAL_String		m_strNoNativeError;   


//Functions
private:
	//set ConnectionHandle
	void setDbc(SQLHANDLE hDhc);

	//set EnvironmentHandle
	void setEnv(SQLHENV hEnv);

	/*!-------------------------------------------------------------------------
    function:     scanErrorText
    description:  scans test which is returned by SQLError
	arguments:	  [in] strErrorText,input error text returned by SQLError
				  [in] strSQLState,input sql state text returned by SQLError
				  [in] nNativeError, native error number
				  [out] strOutErrorText, 
				  [out] strOutSQLState
				  [out] strOutNativeError, native error number
				  [out] *lOutErrPos,position of error in sql statement

				  
    return value: /
    --------------------------------------------------------------------------*/
	void scanErrorText(const StudioOAL_String &strErrorText,
  					   const StudioOAL_String &strSQLState,
					   SQLINTEGER  nNativeError,
					   StudioOAL_String &strOutErrorText,
					   StudioOAL_String &strOutSQLState,
					   StudioOAL_String &strOutNativeError,
					   SAPDB_Int4 *lOutErrPos);
public:
		
	SAPDB_Bool checkSQLReturnCode(SQLRETURN lReturnCode, SQLHSTMT hStmt);

	void clear();

	SAPDB_Bool isError(){	return m_bIsError;}
	SAPDB_Bool isWarning(){	return m_bIsWarning;}


	//returns error pos in error text
	SAPDB_Int4 getErrorPos(){	return m_lErrPos;}
	SQLRETURN  getReturnCode(){	return m_lRc;}


	//returns ConnectionHandle
	SQLHANDLE getDbc() {return m_hDbc;}

	//returns EnvironmentHandle
	SQLHENV getEnv(){return m_hEnv;}
	
	//returns SQL State
	//return SQL State
	const StudioOAL_String& getSQLState()		      {	return m_strSQLState;}
	const StudioOAL_String& getSQLStateErrorNumber()  {	return m_strSQLStateErrorNumber;}
	const StudioOAL_String& getNativeErrorAsString()  {	return m_strNativeError;}
	const StudioOAL_String& getErrorText()			  {	return m_strErrorText;}
	const StudioOAL_String& getNoSQLState()			  { return m_strNoSQLState;}

	SQLINTEGER getNativeError()	   {	return m_nNativeError;}

	void  setNoSQLState(const StudioOAL_String& NoSqlState){  m_strNoSQLState = NoSqlState;}
	
};

#endif

/*===========================================================================*
 *  END
 *===========================================================================*/
