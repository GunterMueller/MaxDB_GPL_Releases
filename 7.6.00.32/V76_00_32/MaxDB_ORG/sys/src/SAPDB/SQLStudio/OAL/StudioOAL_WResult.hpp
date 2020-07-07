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

  module: StudioOAL_WResult.hpp

  -----------------------------------------------------------------------

  responsible: WolfgangA, Hans-GeorgB

  special area: WebQuery, RepMan, DBAnalyzer

  description: Unicode Version of OAL

  version    : 7.6

  see also:

  -----------------------------------------------------------------------
 
  copyright:                Copyright (c) 2000-2005 SAP AG

************************************************************************/

#ifndef STUDIOOAL_WRESULT_HPP
#define STUDIOOAL_WRESULT_HPP


/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

//Includes different enum definitons
#include "SAPDB/SQLStudio/OAL/StudioOAL_WCommon.h"
#include "SAPDB/SQLStudio/OAL/StudioOAL_WColumn.hpp"
#include "SAPDB/SQLStudio/OAL/StudioOAL_WParameters.hpp"
#include "SAPDB/SQLStudio/OAL/StudioOAL_WError.hpp"
 

/*===========================================================================*
 *    class StudioOAL_WResult
 *===========================================================================*/
class StudioOAL_WResult  
{
public:
	StudioOAL_WResult(SQLHANDLE hDbc,SQLHENV hEnv);
	virtual ~StudioOAL_WResult();

	void reset();

	//table type definition
	enum StudioWeb_TableType
	{
		stoalTableTypeUnknown = 0,
		stoalTableTypeResult = 1,
		stoalTableTypeSystem = 2,
		stoalTableTypeView = 3,
		stoalTableTypeTable = 4
	};

	enum StudioOAL_WColumnType
	{
		stoalColTypeUnknown = 0,
		stoalColTypeChar = 1,
		stoalColTypeVarChar = 2,
		stoalColTypeLong = 3,
		stoalColTypeBoolean = 4,
		stoalColTypeFixed = 5,
		stoalColTypeFloat = 6,
		stoalColTypeInt = 7,
		stoalColTypeSmallInt = 8,
		stoalColTypeDate = 9,
		stoalColTypeTime = 10,
		stoalColTypeTimeStamp = 11,
		stoalColTypeDouble = 12    //wird von DB auf float abgebildet
	};

	enum StudioOAL_WColumnCodeType
	{
		stoalColCodeTypeUnknown = 0,
		stoalColCodeTypeASCII = 1,
		stoalColCodeTypeBYTE = 2,
		stoalColCodeTypeEBCDIC = 3,
		stoalColCodeTypeUNICODE = 4
	};


	enum StudioOAL_WResultCursorType
	{
		stoalCursorForwardOnly = 0,
		stoalCursorStatic = 1,
		stoalOptimizeCursor = 2
	};

	enum StudioWeb_AutoCommit
	{
		AutoCommitOff = 0,
		AutoCommitOn = 1   
	};

	/*!-------------------------------------------------------------------------
		function:     init
		description:  initializes a result object for execute
		arguments:	  [in] sCmdStr, parametrized sql command					  
					  
					  
		return value: SAPDB_TRUE if succesfully
    --------------------------------------------------------------------------*/
	SAPDB_Bool init (const StudioOAL_String &sCmdStr);
	SAPDB_Bool bindParameters( );
	SAPDB_Bool setParamValues( StudioOAL_String *sValue );

	SAPDB_Bool bindColumnDataAsString( );
	SAPDB_Bool bindColumnDataAsString( SAPDB_UInt2 nColNo,StudioOAL_String &sValue);
	
	SAPDB_Bool getDataSourceTypeInfo();

	/*!-------------------------------------------------------------------------
		function:     execute
		description:  execute a prepared parametrized statement of a result object 
		arguments:	  /
					  
					  
		return value: SAPDB_TRUE if succesfully
    --------------------------------------------------------------------------*/
	SAPDB_Bool execute();


	void closeCursor();

	void clearParameters() ;
	
	SAPDB_Bool isDBProcedureResult() { return 	m_bDBProcedureResult;}

	SAPDB_Bool getCursorPos(SAPDB_Int4 &nCursorPos);	

	SAPDB_ULong getRowCount()		{return m_lRowCount;}
	SQLSMALLINT getColCount()		{return m_nColCount;}

	//limit for data in long column
	void setLimitForLong(SAPDB_ULong lLimitForLong) {m_lLimitForLong = lLimitForLong;}

	//limit for data in long column
	SAPDB_ULong getLimitForLong() {return m_lLimitForLong;}

	//get statement handle
	SQLHANDLE getStmt(){return m_hStmt;}
	
	//which kind of cursor should be used in result object
	void setResultCursorType  (StudioOAL_WResultCursorType ResultCursorType){m_enumResultCursorType = ResultCursorType;}

	SAPDB_Bool fullExecStmt   (const StudioOAL_String &sCmdStr);
	SAPDB_Bool openResult     (const StudioOAL_String &sCmdStr);
        SAPDB_Bool openResultPrepare     (const StudioOAL_String &sCmdStr);  // TTK
	SAPDB_Bool openParamResult(const StudioOAL_String &sCmdStr);

   
	/*!-------------------------------------------------------------------------
		function:     getParamData
		description:  
		arguments:	  [in/out] **nParamTag, value associated with paramter
					  
					  
		return value: SAPDB_TRUE if succesfully
    --------------------------------------------------------------------------*/
	SAPDB_Bool getParamData(SAPDB_Int2 *&nParamData);

	SAPDB_Bool getParamValue(SAPDB_Int2 nParamNo,StudioOAL_String &sValue);

	/*!-------------------------------------------------------------------------
		function:     putParamData
		description:  put the data for a parameter
		arguments:	  [in] pValue, value for parameter
					  [in] nValueLength, length of value
					  
					  
		return value: sql return code
    --------------------------------------------------------------------------*/
	SAPDB_Bool  putParamData(SQLPOINTER pValue,SQLINTEGER nValueLength);

	SAPDB_Bool  getColumnDescription();

	void closeResult();
	

	SAPDB_Bool fetch();
	SAPDB_Bool extendedFetch(SAPDB_UInt2 lFetchType,SAPDB_ULong lRow,SAPDB_ULong *NoRowsFetched,SAPDB_UInt2 *nRowStatus);

	SAPDB_Bool getColumnDataAsString        (SAPDB_UInt2 nCol,StudioOAL_String &Value);
	
	/*!-------------------------------------------------------------------------
		function:     getODBCColType
		description:  return the sql (odbc) data type of the column
		arguments:	  [in] nCol, number of column
					  [out] nColType, data type of the column
					  
					  
		return value: SAPDB_TRUE if succesfully
    --------------------------------------------------------------------------*/
	SAPDB_Bool getODBCColType (SAPDB_UInt2 nCol,SQLSMALLINT &nColType);

	StudioOAL_WColumn *getColumn     ( SAPDB_UInt2 lCol );
	SAPDB_Bool         getColumnName ( SAPDB_UInt2 lCol, StudioOAL_String &sColName);
	SAPDB_Bool         getLongData   ( SAPDB_UInt2 lCol, SAPDB_Byte *pValue,SAPDB_Long lBufferSize,SAPDB_Long* lDataSize,SAPDB_Bool* bDataTrunc);
	/*!-------------------------------------------------------------------------
		function:     isColNull
		description:  get if col of result row contains null value
		arguments:	  [in] nCol, number of column
					  [out] bIsNull, flag if col of result row contains null value
					  
					  
		return value: SAPDB_TRUE if succesfully
    --------------------------------------------------------------------------*/
	SAPDB_Bool isColNull(SAPDB_UInt2 nCol,SAPDB_Bool &bIsNull);

	//return statement string
	const StudioOAL_String &getStatement() const {return m_sStatement;}	

	void              setSqlMode       ( tpr00_SQLModeEnum SqlStmtMode ){ m_StmtSqlMode  = SqlStmtMode;}
	tpr00_SQLModeEnum getSqlMode       (){ return m_StmtSqlMode;}

	//are there any rows in the result
	SAPDB_Bool noRows()		   {return m_bNoRows;}
	
   /*!-------------------------------------------------------------------------
		function:     isError
		description:  get if executing result failed
		arguments:	  /
					  
					  
		return value: SAPDB_TRUE if executing result failed
    --------------------------------------------------------------------------*/
	// make a copy constructor for StudioOAL_AError, overload the
	// assignment operator and include a getError in StudioOAL_WResult

	SAPDB_Bool  isError()        { return m_pError->isError();       }
	SAPDB_Long  getNativeError() ;
	SAPDB_Int4  getErrorPos()    { return m_pError->getErrorPos();   }
    SQLRETURN 	getReturnCode()  { return m_pError->getReturnCode(); }

	const StudioOAL_String& getErrorText();
	const StudioOAL_String& getSQLState();
	const StudioOAL_String& getSQLStateErrorNumber();
	const StudioOAL_String& getNativeErrorAsString();

        SAPDB_Bool executePreparedStmt();     // TTK

private:

	// methods
	void setDbc(SQLHANDLE hDbc) {m_hDbc = hDbc;}
	void setEnv(SQLHENV hEnv);

	SAPDB_Bool execStmt();
        SAPDB_Bool prepareStmt();     // TTK

	//columns and rows
	SAPDB_Bool getColDescriptions();
	SAPDB_Long rowCount(SQLHANDLE *hStmt);

	//parameters
	SAPDB_Bool fullParameterExecute();
	SAPDB_Bool getParameterDescription();
	SAPDB_Bool prepareForParameters();

	// attributes
	
	//tpr00_SQLModeEnum	m_enumSQLMode;
	StudioOAL_WResultCursorType	m_enumResultCursorType;

	SQLHENV				    m_hEnv ;
	SQLHDBC				    m_hDbc; 
	SQLHANDLE			    m_hStmt;
	tpr00_SQLModeEnum       m_StmtSqlMode;

	SAPDB_Bool				m_bNoRows;
	SAPDB_ULong				m_lFetchedRows;
	SAPDB_Long				m_lRowCount;
	SQLSMALLINT				m_nColCount;
	SAPDB_Long				m_lLimitForLong;

	//statement to execute
	StudioOAL_String	    m_sStatement;
	
	StudioOAL_WError	   *m_pError;
	StudioOAL_WColumn	   *m_pColumn[1024];

	//flag which shows that the result is build to represent the values of a DB procedure
	SAPDB_Bool			    m_bDBProcedureResult;
	StudioOAL_String		m_strEmty;	
	StudioOAL_List          m_ParamList;

	tsp81_UCS2Char		    m_strCmdBuffer [128*1024];	
	tsp81_UCS2Char			m_sRepNullValue[2];
	tsp81_UCS2Char			m_sTrue[5];	
	tsp81_UCS2Char			m_sFalse[6];
	
};

#endif

/*===========================================================================*
 *  END
 *===========================================================================*/
