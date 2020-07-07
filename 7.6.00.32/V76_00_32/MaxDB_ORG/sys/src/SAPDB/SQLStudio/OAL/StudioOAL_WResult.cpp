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

  module: StudioOAL_WResult.cpp

  -----------------------------------------------------------------------

  responsible: WolfgangA and d033893

  special area: WebQuery, SAP DB LOADER, DBAnalyzer

  description:  Unicode Version of OAL

  version     : 7.5.1


************************************************************************/
#include "SAPDB/SAPDBCommon/SAPDB_string.h"
#include "SAPDB/SQLStudio/OAL/StudioOAL_WResult.hpp"

/*===========================================================================*
 *  StudioOAL_WResult::StudioOAL_WResult
 *===========================================================================*/
StudioOAL_WResult::StudioOAL_WResult(SQLHANDLE hDbc,SQLHENV hEnv)
{
	m_hDbc                 = hDbc;
	m_hEnv                 = hEnv;
	m_enumResultCursorType = stoalCursorForwardOnly;

    //init statement handle
	m_hStmt                = 0;
	m_StmtSqlMode          = cpr_kind_internal;
    m_bDBProcedureResult   = SAPDB_FALSE;
	m_lLimitForLong        = 32768;
	m_bNoRows              = SAPDB_TRUE;	
		
	m_pError = new StudioOAL_WError(m_hDbc,m_hEnv);

	memset(m_pColumn, 0, sizeof(m_pColumn));	// initialize all pointers to null pointers

	tsp81_UCS2Char * pConvertOut = NULL;

	/*-------------------------------------------------------------------------*
     * init utf8 strings
     *-------------------------------------------------------------------------*/
	StudioOAL_String strRepOfNull (REPOFNULLVALUE);
	StudioOAL_String strRepOfTrue ("TRUE");
	StudioOAL_String strRepOfFalse("FALSE");
	
   	/*-------------------------------------------------------------------------*
     * init conversion buffers								 			       *
     *-------------------------------------------------------------------------*/
	memset(m_sRepNullValue,0,sizeof(m_sRepNullValue));
	memset(m_sTrue        ,0,sizeof(m_sTrue ));
	memset(m_sFalse       ,0,sizeof(m_sFalse));
	
	/*-------------------------------------------------------------------------*
     * convert utf8 to ucs2									 			       *
     *-------------------------------------------------------------------------*/
	strRepOfNull.ConvertToUCS2_Unaligned_Swap(m_sRepNullValue,
							                 (m_sRepNullValue+(sizeof(m_sRepNullValue)/sizeof(SAPDB_UCS2))),
							                  pConvertOut) ;

	strRepOfTrue.ConvertToUCS2_Unaligned_Swap(m_sTrue,
							                 (m_sTrue+(sizeof(m_sTrue)/sizeof(SAPDB_UCS2))),
							                  pConvertOut) ;

	strRepOfFalse.ConvertToUCS2_Unaligned_Swap(m_sFalse,
							                  (m_sFalse+(sizeof(m_sFalse)/sizeof(SAPDB_UCS2))),
							                  pConvertOut) ;

}
/*===========================================================================*
 *  StudioOAL_WResult::~StudioOAL_WResult
 *===========================================================================*/
StudioOAL_WResult::~StudioOAL_WResult()
{
	SAPDB_ULong lRun = 0;

	clearParameters();

	if( m_pError != NULL) {

		delete	m_pError;
		m_pError = NULL;
	}

	for( lRun; lRun <= 1023; ++lRun )
	{
		if (m_pColumn[lRun] != NULL ){

			delete m_pColumn[lRun];
			m_pColumn[lRun] = NULL;
		}
	}

	closeResult();
}
/*===========================================================================*
 *  StudioOAL_WResult::init
 *===========================================================================*/
SAPDB_Bool StudioOAL_WResult::init (const StudioOAL_String &sCmdStr)
{
	clearParameters();

	//if there is a open result the close it
    if(m_hStmt != SQL_NULL_HSTMT)
	   closeResult();

	StudioOAL_WParameters* poParameterDefinitions  = NULL;    
    
    //inits
	m_nColCount   = 0;
    m_sStatement  = sCmdStr;    
	SQLRETURN nRc = SQL_SUCCESS;
    
    m_lFetchedRows = 0;
    m_bNoRows = SAPDB_TRUE;	
 	
    // SQLAllocHandle
	nRc = SQLAllocHandle(SQL_HANDLE_STMT,m_hDbc,&m_hStmt);
	if(nRc != SQL_SUCCESS)
	{	
		SQLFreeStmt(m_hStmt, SQL_DROP);
		m_hStmt = SQL_NULL_HSTMT;
		return SAPDB_FALSE;	
	}	
	
    // SQLPrepareW SQLNumParams SQLDescribeParam
    if (!prepareForParameters()) {
    
        SQLFreeStmt(m_hStmt, SQL_DROP);
		m_hStmt = SQL_NULL_HSTMT;
        return SAPDB_FALSE;
	}
	
	// SQLBindParameter
    if(!bindParameters()){

	    SQLFreeStmt(m_hStmt, SQL_DROP);
		m_hStmt = SQL_NULL_HSTMT;
        return SAPDB_FALSE;
	}

	// SQLNumResultCols SQLDescribeColW
	if (!getColumnDescription() ) {
		
		SQLFreeStmt(m_hStmt, SQL_DROP);
		m_hStmt = SQL_NULL_HSTMT;
        return SAPDB_FALSE;
	}

	// SQLBindCol
	if (!bindColumnDataAsString()){

		SQLFreeStmt(m_hStmt, SQL_DROP);
		m_hStmt = SQL_NULL_HSTMT;
        return SAPDB_FALSE;
	}

	return SAPDB_TRUE;
}
/*===========================================================================*
 *  StudioOAL_WResult::bindParameters()
 *===========================================================================*/
SAPDB_Bool StudioOAL_WResult::bindParameters() 
{
    
    StudioOAL_WParameters  *poCurrentParam;
    
	poCurrentParam = (StudioOAL_WParameters*)m_ParamList.first();
    while (poCurrentParam != NULL){

		if(!poCurrentParam->bindSQLParameter()) return SAPDB_FALSE;
		poCurrentParam = (StudioOAL_WParameters*)m_ParamList.next();
	}    
    
    return SAPDB_TRUE;
}
/*===========================================================================*
 *  StudioOAL_WResult::setParamValues
 *===========================================================================*/
SAPDB_Bool StudioOAL_WResult::setParamValues( StudioOAL_String *record )
{
	// assert (record.size() == m_ParamList.size() )
	SAPDB_Int4 nParamNo = 0;

	StudioOAL_WParameters  *poCurrentParam;
    
	poCurrentParam = (StudioOAL_WParameters*)m_ParamList.first();
	while (poCurrentParam != NULL)
	{
		poCurrentParam->setValue( record[nParamNo] );
		poCurrentParam = (StudioOAL_WParameters*)m_ParamList.next();
		nParamNo++;
	}    
    
    return SAPDB_TRUE;
}
/*===========================================================================*
 *  StudioOAL_WResult::bindColumnDataAsString
 *===========================================================================*/
SAPDB_Bool StudioOAL_WResult::bindColumnDataAsString()
{
    SQLRETURN			nRc = 0;

	tsp81_UCS2Char     *pValue = NULL;
	SAPDB_Long		    lValueLen = 0;  //!!IA64
	SAPDB_Long			lGetDataLen;
    
	//SQLRETURN SQLBindCol( SQLHSTMT StatementHandle, SQLUSMALLINT ColumnNumber, SQLSMALLINT TargetType, SQLPOINTER TargetValuePtr, SQLINTEGER BufferLength, SQLLEN     * StrLen_or_Ind);
	//SQLRETURN SQLGetData( SQLHSTMT StatementHandle, SQLUSMALLINT ColumnNumber, SQLSMALLINT TargetType, SQLPOINTER TargetValuePtr, SQLINTEGER BufferLength, SQLINTEGER * StrLen_or_IndPtr);

    if( m_hDbc == SQL_NULL_HDBC || m_hEnv == SQL_NULL_HENV || m_hStmt == SQL_NULL_HSTMT ) 
		return SAPDB_FALSE;
	if( m_pError == NULL)	
		return SAPDB_FALSE;	

	SAPDB_UInt2 nCol;

   	for (nCol = 1; nCol <= getColCount(); nCol++)
	{
	
		if (m_pColumn[nCol-1] == NULL)	
			return SAPDB_FALSE;	
		
		//get the data for the current col
		switch (m_pColumn[nCol-1]->getODBCColType())
		{    
			case SQL_DATE:
						
				pValue = new tsp81_UCS2Char[MX_COLUMN_GENERIC_NUMERIC];
				if (pValue == NULL) return SAPDB_FALSE;	
				
				m_pColumn[nCol-1]->setColValue(pValue);
				m_pColumn[nCol-1]->setColLength((SAPDB_Long)MX_COLUMN_GENERIC_NUMERIC);
				
				nRc = SQLBindCol(m_hStmt, nCol, SQL_C_TCHAR, 
					            (SQLPOINTER  ) m_pColumn[nCol-1]->m_pValue, MX_COLUMN_GENERIC_NUMERIC, 
								&m_pColumn[nCol-1]->m_ValueLength); //!!IA64
				//nRc = SQLBindCol(m_hStmt, nCol, SQL_C_CHAR, &m_pColumn[nCol-1]->m_pValue, MX_COLUMN_GENERIC_NUMERIC, &m_pColumn[nCol-1]->m_ValueLength);
			
				if( nRc != SQL_SUCCESS) {
    
					if(! (m_pError->checkSQLReturnCode(nRc,m_hStmt))) {    

						delete[] pValue;	
						return SAPDB_FALSE;
					}
				}    
                            
				break;

			case SQL_TIME:
						
				pValue = new tsp81_UCS2Char[MX_COLUMN_GENERIC_NUMERIC];
				if (pValue == NULL) return SAPDB_FALSE;

				m_pColumn[nCol-1]->setColValue(pValue);
				m_pColumn[nCol-1]->setColLength((SAPDB_Long)MX_COLUMN_GENERIC_NUMERIC);
				
				nRc = SQLBindCol(m_hStmt, nCol, SQL_C_TCHAR, 
					            (SQLPOINTER)   m_pColumn[nCol-1]->m_pValue, MX_COLUMN_GENERIC_NUMERIC, 
								&m_pColumn[nCol-1]->m_ValueLength);  //!!IA64
			
				if( nRc != SQL_SUCCESS) {

    				if(! (m_pError->checkSQLReturnCode(nRc,m_hStmt))) {    
						delete[] pValue;
						return SAPDB_FALSE;
					}
				}    
            
				break;

		   case SQL_TIMESTAMP:
													
				pValue = new tsp81_UCS2Char [MX_COLUMN_GENERIC_NUMERIC];
				if (pValue == NULL) return SAPDB_FALSE;

				m_pColumn[nCol-1]->setColValue(pValue);
				m_pColumn[nCol-1]->setColLength((SAPDB_Long)MX_COLUMN_GENERIC_NUMERIC);
				
				nRc = SQLBindCol(m_hStmt, nCol, SQL_C_TCHAR, 
					            (SQLPOINTER ) m_pColumn[nCol-1]->m_pValue, MX_COLUMN_GENERIC_NUMERIC , 
								&m_pColumn[nCol-1]->m_ValueLength); //!!IA64

				if( nRc != SQL_SUCCESS) {
    			
				if(! (m_pError->checkSQLReturnCode(nRc,m_hStmt))) {    

						delete[] pValue;	
						return SAPDB_FALSE;
					}
				}    
				
				break;
                                   
			case SQL_LONGVARBINARY:
			case SQL_LONGVARCHAR:
			case SQL_WLONGVARCHAR : 
							
				if( m_lLimitForLong >= 0 )
				{

					 lGetDataLen = m_lLimitForLong;                 
					 //Limit for long column can overwrite max value for long column
					 if( m_lLimitForLong > MX_COLUMN_LONG_VALUE ) 
						 lGetDataLen = MX_COLUMN_LONG_VALUE;   
					 
				}
				else
					lGetDataLen = MX_COLUMN_LONG_VALUE;
            
					pValue = new tsp81_UCS2Char [lGetDataLen];
					if (pValue == NULL) return SAPDB_FALSE;	
					
					m_pColumn[nCol-1]->setColValue(pValue);
					m_pColumn[nCol-1]->setColLength(lGetDataLen);

					m_pError->setNoSQLState(SQLSTATE_DATA_TRUNCATED);
				

					nRc = SQLBindCol(m_hStmt, nCol, SQL_C_TCHAR, 
						            (SQLPOINTER  )  m_pColumn[nCol-1]->m_pValue, 
									 lGetDataLen*sizeof(tsp81_UCS2Char) , 
									 &m_pColumn[nCol-1]->m_ValueLength); //!!IA64
					
					if( nRc != SQL_SUCCESS ) {            
						
						if(! (m_pError->checkSQLReturnCode(nRc,m_hStmt))) {    

							m_pError->setNoSQLState (SQLSTATE_SUCCESS);

							delete[] pValue;
							return SAPDB_FALSE;
						}
						m_pError->setNoSQLState (SQLSTATE_SUCCESS);                
					 }

				break;

			case SQL_DECIMAL:
			case SQL_NUMERIC:
			case SQL_REAL:
			case SQL_FLOAT:
			case SQL_DOUBLE:
			case SQL_INTEGER:
        
				//get the current value			
				pValue = new tsp81_UCS2Char[MX_COLUMN_GENERIC_NUMERIC];
				if (pValue == NULL) return SAPDB_FALSE;

				m_pColumn[nCol-1]->setColValue(pValue);
				m_pColumn[nCol-1]->setColLength((SAPDB_Long)MX_COLUMN_GENERIC_NUMERIC);
				
				nRc = SQLBindCol(m_hStmt, nCol, SQL_C_TCHAR, 
					            (SQLPOINTER  ) m_pColumn[nCol-1]->m_pValue, MX_COLUMN_GENERIC_NUMERIC , 
								&m_pColumn[nCol-1]->m_ValueLength); //!!IA64

				if( nRc != SQL_SUCCESS) {
    
					if(! (m_pError->checkSQLReturnCode(nRc,m_hStmt)))
					{    
						delete[] pValue;	
						return SAPDB_FALSE;
					}
				}
				
				break;

			case SQL_BIT:

				//get the current value	
				
				lGetDataLen = 6;

				pValue = new tsp81_UCS2Char [lGetDataLen];
				if (pValue == NULL) return SAPDB_FALSE;	
				
				m_pColumn[nCol-1]->setColValue(pValue);
				m_pColumn[nCol-1]->setColLength((SAPDB_Long)6);
            
				nRc = SQLBindCol(m_hStmt, nCol, SQL_C_TCHAR, 
					            (SQLPOINTER)   m_pColumn[nCol-1]->m_pValue, lGetDataLen , 
								&m_pColumn[nCol-1]->m_ValueLength); //!!IA64
				 			
				if( nRc != SQL_SUCCESS) 
					if(! (m_pError->checkSQLReturnCode(nRc,m_hStmt))) 
						return SAPDB_FALSE;				
				
				break;

			default:
				//SQL_VARBINARY

				if( m_pColumn[nCol-1]->getODBCColType() == SQL_VARBINARY ||
					m_pColumn[nCol-1]->getODBCColType() == SQL_BINARY )
					//will be transformed from hex to ascii 
					lGetDataLen = m_pColumn[nCol-1]->getColLength() * sizeof(tsp81_UCS2Char) + 1;
				else
					lGetDataLen = m_pColumn[nCol-1]->getColLength() + 1 ;

				pValue = new tsp81_UCS2Char [lGetDataLen];
				if (pValue == NULL) return SAPDB_FALSE;

				m_pColumn[nCol-1]->setColValue(pValue);
				m_pColumn[nCol-1]->setColLength((SAPDB_Long)lGetDataLen);
				
				nRc = SQLBindCol(m_hStmt, nCol, SQL_C_TCHAR, 
								 (SQLPOINTER  ) m_pColumn[nCol-1]->m_pValue , 
								 lGetDataLen , 
								 &m_pColumn[nCol-1]->m_ValueLength); //!!IA64

				if( nRc != SQL_SUCCESS) 
				{
    				if(! (m_pError->checkSQLReturnCode(nRc,m_hStmt)))
					{    
						delete[] pValue;
						return SAPDB_FALSE;
					}
				}

		}//switch
	}// for (nCol = 1; nCol <= getColCount(); nCol++)

	//if data was truncated then get the possible size lGetDataLen
  	//if(m_pColumn[nCol-1]->getColValueLength() == SQL_NO_TOTAL) 
  	  // m_pColumn[nCol-1]->setColValueLength(lGetDataLen);
	
	return SAPDB_TRUE;
}
/*===========================================================================*
 *  StudioOAL_WResult::bindColumnDataAsString
 *===========================================================================*/
SAPDB_Bool StudioOAL_WResult::bindColumnDataAsString (SAPDB_UInt2 nCol,StudioOAL_String &Value)
{
	Value.Erase();

    if( m_hDbc == SQL_NULL_HDBC || 
		m_hEnv == SQL_NULL_HENV ||
		m_hStmt == SQL_NULL_HSTMT) 	return SAPDB_FALSE;	
        
    if (m_pColumn[nCol-1] == NULL)	return SAPDB_FALSE;	
	if( m_pError == NULL)	        return SAPDB_FALSE;	

	//get the data for the current col
	switch (m_pColumn[nCol-1]->getODBCColType())
	{    
			case SQL_DATE:
		
				if( m_pColumn[nCol-1]->getColValueLength() >= 0 )
            		m_pColumn[nCol-1]->setColIsNull(SAPDB_FALSE);
				else
				{
					sp81UCS2strcpy( m_pColumn[nCol-1]->getColValue() ,m_sRepNullValue);				
					m_pColumn[nCol-1]->setColValueLength(sp81UCS2strlen(m_sRepNullValue));
					m_pColumn[nCol-1]->setColIsNull(SAPDB_TRUE);
				}
				
				break;

			case SQL_TIME:

				if( m_pColumn[nCol-1]->getColValueLength() >= 0 )
					m_pColumn[nCol-1]->setColIsNull(SAPDB_FALSE);
				else
				{
					sp81UCS2strcpy( m_pColumn[nCol-1]->getColValue() ,m_sRepNullValue);				
					m_pColumn[nCol-1]->setColValueLength(sp81UCS2strlen(m_sRepNullValue));
					m_pColumn[nCol-1]->setColIsNull(SAPDB_TRUE);
				}

				break;

			case SQL_TIMESTAMP:

				if( m_pColumn[nCol-1]->getColValueLength() >= 0 )
					m_pColumn[nCol-1]->setColIsNull(SAPDB_FALSE);
				else
				{
					sp81UCS2strcpy( m_pColumn[nCol-1]->getColValue() ,m_sRepNullValue);				
					m_pColumn[nCol-1]->setColValueLength(sp81UCS2strlen(m_sRepNullValue));
					m_pColumn[nCol-1]->setColIsNull(SAPDB_TRUE);
				}

				break;


		   case SQL_LONGVARBINARY:
		   case SQL_LONGVARCHAR  :		   
		
			   if( m_pColumn[nCol-1]->getColValueLength() == SQL_NO_TOTAL )            
			       m_pColumn[nCol-1]->setColIsNull(SAPDB_FALSE);                				
			   else
			   {
					if( m_pColumn[nCol-1]->getColValueLength() >= 0 )                    
						m_pColumn[nCol-1]->setColIsNull(SAPDB_FALSE);					
					else
					{
						sp81UCS2strcpy( m_pColumn[nCol-1]->getColValue() ,m_sRepNullValue);				
					    m_pColumn[nCol-1]->setColValueLength(sp81UCS2strlen(m_sRepNullValue));
						m_pColumn[nCol-1]->setColIsNull(SAPDB_TRUE);
					}
				}

			   break;

		   case SQL_DECIMAL:
		   case SQL_NUMERIC:
		   case SQL_REAL:
		   case SQL_FLOAT:
		   case SQL_DOUBLE:
		   case SQL_INTEGER:

			   	if( m_pColumn[nCol-1]->getColValueLength() >= 0 )            
					m_pColumn[nCol-1]->setColIsNull(SAPDB_FALSE);            
				else
				{
					sp81UCS2strcpy( m_pColumn[nCol-1]->getColValue() ,m_sRepNullValue);				
					m_pColumn[nCol-1]->setColValueLength(sp81UCS2strlen(m_sRepNullValue));
					m_pColumn[nCol-1]->setColIsNull(SAPDB_TRUE);
				}

			    break;

			   
		   case SQL_BIT:

			   if(m_pColumn[nCol-1]->getColValueLength() > 0)
			      m_pColumn[nCol-1]->setColIsNull(SAPDB_FALSE);
			   else
			   {
					sp81UCS2strcpy( m_pColumn[nCol-1]->getColValue() ,m_sRepNullValue);				
					m_pColumn[nCol-1]->setColValueLength(sp81UCS2strlen(m_sRepNullValue));
					m_pColumn[nCol-1]->setColIsNull(SAPDB_TRUE);
			   }

			   break;

		   default:

			   if( m_pColumn[nCol-1]->getColValueLength() > 0 )
				   m_pColumn[nCol-1]->setColIsNull(SAPDB_FALSE);
			   else
			   {
				    if( m_pColumn[nCol-1]->getColValueLength() != SQL_NULL_DATA )
					{
                		//delete[] pValue;
						//pValue  = 0;
						m_pColumn[nCol-1]->setColValueLength(0);
						m_pColumn[nCol-1]->setColIsNull(SAPDB_FALSE);
					}
					else 
					{
						sp81UCS2strcpy( m_pColumn[nCol-1]->getColValue() ,m_sRepNullValue);				
					    m_pColumn[nCol-1]->setColValueLength(sp81UCS2strlen(m_sRepNullValue));
						m_pColumn[nCol-1]->setColIsNull(SAPDB_TRUE);
					}
			   }	

			   break;

	}

  	//if data was truncated then get the possible size lGetDataLen
  	//if(m_pColumn[nCol-1]->getColValueLength() == SQL_NO_TOTAL) 
 	  // m_pColumn[nCol-1]->setColValueLength(lGetDataLen); //!! ToDo  
	
    if (m_pColumn[nCol-1]->getColValue() != NULL && m_pColumn[nCol-1]->getColValueLength() > 0)
	{


		if(Value.ConvertFromUCS2(
					(StudioOAL_String::UCS2ConstPointer)m_pColumn[nCol-1]->getColValue(),
					(StudioOAL_String::UCS2ConstPointer)m_pColumn[nCol-1]->getColValue() + sp81UCS2strlen(m_pColumn[nCol-1]->getColValue())) 
					!= Tools_UTF8Basis::Success)

 		   return SAPDB_FALSE;
	}
	else
	 	 return SAPDB_FALSE;


	return SAPDB_TRUE;
}
/*===========================================================================*
 *  StudioOAL_WResult::getDataSourceTypeInfo
 *===========================================================================*/
SAPDB_Bool StudioOAL_WResult::getDataSourceTypeInfo()
{ 
 
    //check it
	if( m_pError == NULL) return SAPDB_FALSE;
    if( m_hDbc == SQL_NULL_HDBC || m_hEnv == SQL_NULL_HENV)  return SAPDB_FALSE;
	
    //init	variables
	SQLRETURN		nRc = SQL_SUCCESS;    
    m_sStatement   =  "";
    m_lFetchedRows = 0;
    m_bNoRows = SAPDB_TRUE;	

    //if there is a open result the close it
    if(m_hStmt != SQL_NULL_HSTMT) 
	   closeResult();

    //allocate handle for statement    
	nRc = SQLAllocHandle(SQL_HANDLE_STMT,m_hDbc,&m_hStmt);
	if( nRc != SQL_SUCCESS)
	{
		if(! (m_pError->checkSQLReturnCode(nRc,m_hStmt))) {

			SQLFreeStmt(m_hStmt, SQL_DROP);
			m_hStmt = SQL_NULL_HSTMT;
			return SAPDB_FALSE;
		}
	}		
	
    nRc = SQLSetStmtOption(m_hStmt, SQL_ATTR_ROW_BIND_TYPE, SQL_BIND_BY_COLUMN);
    if( nRc != SQL_SUCCESS ) {
    
		if(! (m_pError->checkSQLReturnCode(nRc,m_hStmt))) {
    
            SQLFreeStmt(m_hStmt, SQL_DROP);
			m_hStmt = SQL_NULL_HSTMT;
            return SAPDB_FALSE;
        }
    }
    
    switch (m_enumResultCursorType)
    {
        case stoalCursorForwardOnly:
    
            nRc = SQLSetStmtOption(m_hStmt, SQL_ATTR_CONCURRENCY, SQL_CONCUR_READ_ONLY);
            if( nRc != SQL_SUCCESS ) {
                
                if(! (m_pError->checkSQLReturnCode(nRc,m_hStmt))) {
        
                    SQLFreeStmt(m_hStmt, SQL_DROP);
					m_hStmt = SQL_NULL_HSTMT;
                    return SAPDB_FALSE;
                }
            }
            nRc = SQLSetStmtOption(m_hStmt, SQL_ATTR_CURSOR_TYPE, SQL_CURSOR_FORWARD_ONLY);
            if( nRc != SQL_SUCCESS ) {
              if(! (m_pError->checkSQLReturnCode(nRc,m_hStmt))) {
    
                    SQLFreeStmt(m_hStmt, SQL_DROP);
					m_hStmt = SQL_NULL_HSTMT;
                    return SAPDB_FALSE;
                }
            }

			break;
        
        case stoalCursorStatic:

            nRc = SQLSetStmtOption(m_hStmt, SQL_ATTR_CONCURRENCY, SQL_CONCUR_ROWVER);
            if( nRc != SQL_SUCCESS ) {
                if(! (m_pError->checkSQLReturnCode(nRc,m_hStmt))) {
        
                    SQLFreeStmt(m_hStmt, SQL_DROP);
					m_hStmt = SQL_NULL_HSTMT;
                    return SAPDB_FALSE;
                }
            }
            
            nRc = SQLSetStmtOption(m_hStmt, SQL_ATTR_CURSOR_TYPE, SQL_CURSOR_STATIC);
            if( nRc != SQL_SUCCESS ) {
                
                if(! (m_pError->checkSQLReturnCode(nRc,m_hStmt))) {
                
                    SQLFreeStmt(m_hStmt, SQL_DROP);
					m_hStmt = SQL_NULL_HSTMT;
                    return SAPDB_FALSE;
                }
            }
			break;
            
		case stoalOptimizeCursor:
    
			nRc = SQLSetStmtOption(m_hStmt, SQL_ATTR_CONCURRENCY, SQL_CONCUR_READ_ONLY);
			if( nRc != SQL_SUCCESS ) {
            
				if(! (m_pError->checkSQLReturnCode(nRc,m_hStmt))) {
    
					SQLFreeStmt(m_hStmt, SQL_DROP);
					m_hStmt = SQL_NULL_HSTMT;
					return SAPDB_FALSE;
				}
			}
        
			nRc = SQLSetStmtOption(m_hStmt, SQL_ATTR_OPTIMIZE_CURSOR, 1);
			if( nRc != SQL_SUCCESS ) {
        
				if(! (m_pError->checkSQLReturnCode(nRc,m_hStmt))) {

					//Die Option ExtendedFetch ohne for reuse gibts erst ab 6.2.10
					if(m_pError->getSQLStateErrorNumber() == "S1092") {  // Out of Option range
							
                
						SQLFreeStmt(m_hStmt, SQL_DROP);
						m_hStmt = SQL_NULL_HSTMT;
						return SAPDB_FALSE;
					}
				}
			}
			nRc = SQLSetStmtOption(m_hStmt, SQL_ATTR_CURSOR_TYPE, SQL_CURSOR_KEYSET_DRIVEN);
			if( nRc != SQL_SUCCESS ) {
        
				if(! (m_pError->checkSQLReturnCode(nRc,m_hStmt))) {

					SQLFreeStmt(m_hStmt, SQL_DROP);
					m_hStmt = SQL_NULL_HSTMT;
					return SAPDB_FALSE;
				}
			}
			break;
         
		default:
			return SAPDB_FALSE;
			break;   
   }

   nRc = SQLSetStmtOption(m_hStmt, SQL_ATTR_USE_BOOKMARKS, SQL_UB_ON);
   if( nRc != SQL_SUCCESS ) {

        if(! (m_pError->checkSQLReturnCode(nRc,m_hStmt))) {
    
            SQLFreeStmt(m_hStmt, SQL_DROP);
			m_hStmt = SQL_NULL_HSTMT;
            return SAPDB_FALSE;
        }
    }
    
    nRc = SQLSetStmtOption(m_hStmt, SQL_ROWSET_SIZE, 1);
    if( nRc != SQL_SUCCESS ) {

        if(! (m_pError->checkSQLReturnCode(nRc,m_hStmt))) {
    
            SQLFreeStmt(m_hStmt, SQL_DROP);
			m_hStmt = SQL_NULL_HSTMT;
            return SAPDB_FALSE;
        }
    }
    
    //open the result set
    nRc = SQLGetTypeInfo(m_hStmt,SQL_ALL_TYPES);
	if( nRc != SQL_SUCCESS ) {

        if(! (m_pError->checkSQLReturnCode(nRc,m_hStmt))) {
    
            SQLFreeStmt(m_hStmt, SQL_DROP);
			m_hStmt = SQL_NULL_HSTMT;
            return SAPDB_FALSE;
        }
    }

	if(!getColDescriptions()) 	
		return SAPDB_FALSE;
    
    //is there a result
	m_lRowCount = rowCount(&m_hStmt);
    if( m_lRowCount == 0 ) {
        
        return SAPDB_TRUE;
	}
	else{
        m_bNoRows = SAPDB_FALSE;
    }    
    return SAPDB_TRUE;

}
/*===========================================================================*
 *  StudioOAL_WResult::closeCursor()
 *===========================================================================*/
void StudioOAL_WResult::closeCursor()
{       
    
    //check it
    if( m_hStmt == SQL_NULL_HSTMT) return;	    
	SQLFreeStmt(m_hStmt, SQL_CLOSE);    	

}
/*===========================================================================*
 *  StudioOAL_WResult::reset()
 *===========================================================================*/
void StudioOAL_WResult::reset()
{
	m_enumResultCursorType = stoalCursorForwardOnly;
    m_bDBProcedureResult   = SAPDB_FALSE;
	m_lLimitForLong        = 32768;	
	m_lFetchedRows         = 0;
	m_bNoRows              = SAPDB_TRUE;	
	m_lRowCount            = 0;

}
/*===========================================================================*
 *  StudioOAL_WResult::fullExecStmt
 *===========================================================================*/
SAPDB_Bool StudioOAL_WResult::fullExecStmt(const StudioOAL_String& sCmdStr)
{

	//check it
	if( m_pError == NULL) return SAPDB_FALSE;
    if( m_hDbc == SQL_NULL_HDBC || m_hEnv == SQL_NULL_HENV)  return SAPDB_FALSE;
	
    //Init	
	SQLRETURN		nRc = SQL_SUCCESS;
    
    m_sStatement = sCmdStr;
    m_lFetchedRows = 0;
    m_bNoRows = SAPDB_TRUE;	

    //if there is a open result the close it
    if(m_hStmt != SQL_NULL_HSTMT )
	   closeResult();

    //allocate handle for statement    
	nRc = SQLAllocHandle(SQL_HANDLE_STMT,m_hDbc,&m_hStmt);
	if( nRc != SQL_SUCCESS)
	{
		if(! (m_pError->checkSQLReturnCode(nRc,m_hStmt))) {

			SQLFreeStmt(m_hStmt, SQL_DROP);
			m_hStmt = SQL_NULL_HSTMT;
			return SAPDB_FALSE;
		}
	}		
    
    //open the result set
    if(!execStmt()) {

        //free statement
        SQLFreeStmt(m_hStmt, SQL_DROP);
		m_hStmt = SQL_NULL_HSTMT;
        return SAPDB_FALSE;
    }
    
    m_bNoRows = SAPDB_TRUE;
    
    return SAPDB_TRUE;
	
}
/*===========================================================================*
 *  StudioOAL_WResult::openResult
 *===========================================================================*/
SAPDB_Bool StudioOAL_WResult::openResult(const StudioOAL_String &sCmdStr)
{ 
 
    //check it
	if( m_pError == NULL) return SAPDB_FALSE;
    if( m_hDbc == SQL_NULL_HDBC || m_hEnv == SQL_NULL_HENV)  return SAPDB_FALSE;
	
    //init	variables
	SQLRETURN  nRc = SQL_SUCCESS;    
    m_sStatement   = sCmdStr;
    m_lFetchedRows = 0;
    m_bNoRows      = SAPDB_TRUE;	

    //if there is a open result the close it
    if(m_hStmt != SQL_NULL_HSTMT) 
	   closeResult();

    //allocate handle for statement    
	nRc = SQLAllocHandle(SQL_HANDLE_STMT,m_hDbc,&m_hStmt);
	if( nRc != SQL_SUCCESS)
	{
		if(! (m_pError->checkSQLReturnCode(nRc,m_hStmt))) {

			SQLFreeStmt(m_hStmt, SQL_DROP);
			m_hStmt = SQL_NULL_HSTMT;
			return SAPDB_FALSE;
		}
	}
	
	nRc = SQLSetStmtOption(m_hStmt,SQL_SQLMODE,getSqlMode()); 
    if( nRc != SQL_SUCCESS ) {
    
		if(! (m_pError->checkSQLReturnCode(nRc,m_hStmt))) {
    
            SQLFreeStmt(m_hStmt, SQL_DROP);
			m_hStmt = SQL_NULL_HSTMT;
            return SAPDB_FALSE;
        }
    }

    nRc = SQLSetStmtOption(m_hStmt, SQL_ATTR_ROW_BIND_TYPE, SQL_BIND_BY_COLUMN);
    if( nRc != SQL_SUCCESS ) {
    
		if(! (m_pError->checkSQLReturnCode(nRc,m_hStmt))) {
    
            SQLFreeStmt(m_hStmt, SQL_DROP);
			m_hStmt = SQL_NULL_HSTMT;
            return SAPDB_FALSE;
        }
    }
    
    switch (m_enumResultCursorType)
    {
        case stoalCursorForwardOnly:
    
            nRc = SQLSetStmtOption(m_hStmt, SQL_ATTR_CONCURRENCY, SQL_CONCUR_READ_ONLY);
            if( nRc != SQL_SUCCESS ) {
                
                if(! (m_pError->checkSQLReturnCode(nRc,m_hStmt))) {
        
                    SQLFreeStmt(m_hStmt, SQL_DROP);
					m_hStmt = SQL_NULL_HSTMT;
                    return SAPDB_FALSE;
                }
            }
            nRc = SQLSetStmtOption(m_hStmt, SQL_ATTR_CURSOR_TYPE, SQL_CURSOR_FORWARD_ONLY);
            if( nRc != SQL_SUCCESS ) {
              if(! (m_pError->checkSQLReturnCode(nRc,m_hStmt))) {
    
                    SQLFreeStmt(m_hStmt, SQL_DROP);
					m_hStmt = SQL_NULL_HSTMT;
                    return SAPDB_FALSE;
                }
            }

			break;
        
        case stoalCursorStatic:

            nRc = SQLSetStmtOption(m_hStmt, SQL_ATTR_CONCURRENCY, SQL_CONCUR_ROWVER);
            if( nRc != SQL_SUCCESS ) {
                if(! (m_pError->checkSQLReturnCode(nRc,m_hStmt))) {
        
                    SQLFreeStmt(m_hStmt, SQL_DROP);
					m_hStmt = SQL_NULL_HSTMT;
                    return SAPDB_FALSE;
                }
            }
            
            nRc = SQLSetStmtOption(m_hStmt, SQL_ATTR_CURSOR_TYPE, SQL_CURSOR_STATIC);
            if( nRc != SQL_SUCCESS ) {
                
                if(! (m_pError->checkSQLReturnCode(nRc,m_hStmt))) {
                
                    SQLFreeStmt(m_hStmt, SQL_DROP);
					m_hStmt = SQL_NULL_HSTMT;
                    return SAPDB_FALSE;
                }
            }
			break;
            
		case stoalOptimizeCursor:
    
			nRc = SQLSetStmtOption(m_hStmt, SQL_ATTR_CONCURRENCY, SQL_CONCUR_READ_ONLY);
			if( nRc != SQL_SUCCESS ) {
            
				if(! (m_pError->checkSQLReturnCode(nRc,m_hStmt))) {
    
					SQLFreeStmt(m_hStmt, SQL_DROP);
					m_hStmt = SQL_NULL_HSTMT;
					return SAPDB_FALSE;
				}
			}
        
			nRc = SQLSetStmtOption(m_hStmt, SQL_ATTR_OPTIMIZE_CURSOR, 1);
			if( nRc != SQL_SUCCESS ) {
        
				if(! (m_pError->checkSQLReturnCode(nRc,m_hStmt))) {

					//Die Option ExtendedFetch ohne for reuse gibts erst ab 6.2.10
					if(m_pError->getSQLStateErrorNumber() == "S1092") {  // Out of Option range
							
                
						SQLFreeStmt(m_hStmt, SQL_DROP);
						m_hStmt = SQL_NULL_HSTMT;
						return SAPDB_FALSE;
					}
				}
			}
			nRc = SQLSetStmtOption(m_hStmt, SQL_ATTR_CURSOR_TYPE, SQL_CURSOR_KEYSET_DRIVEN);
			if( nRc != SQL_SUCCESS ) {
        
				if(! (m_pError->checkSQLReturnCode(nRc,m_hStmt))) {

					SQLFreeStmt(m_hStmt, SQL_DROP);
					m_hStmt = SQL_NULL_HSTMT;
					return SAPDB_FALSE;
				}
			}
			break;
         
		default:
			return SAPDB_FALSE;
			break;   
   }

   nRc = SQLSetStmtOption(m_hStmt, SQL_ATTR_USE_BOOKMARKS, SQL_UB_ON);
   if( nRc != SQL_SUCCESS ) {

        if(! (m_pError->checkSQLReturnCode(nRc,m_hStmt))) {
    
            SQLFreeStmt(m_hStmt, SQL_DROP);
			m_hStmt = SQL_NULL_HSTMT;
            return SAPDB_FALSE;
        }
    }
    
    nRc = SQLSetStmtOption(m_hStmt, SQL_ROWSET_SIZE, 1);
    if( nRc != SQL_SUCCESS ) {

        if(! (m_pError->checkSQLReturnCode(nRc,m_hStmt))) {
    
            SQLFreeStmt(m_hStmt, SQL_DROP);
			m_hStmt = SQL_NULL_HSTMT;
            return SAPDB_FALSE;
        }
    }
    
    //open the result set
    if(!execStmt()){

        //free statement
        SQLFreeStmt(m_hStmt, SQL_DROP);
		m_hStmt = SQL_NULL_HSTMT;
        return SAPDB_FALSE;
    }
    
    if(!getColDescriptions()) 	return SAPDB_FALSE;
	
    //is there a result
	m_lRowCount = rowCount(&m_hStmt);
    if( m_lRowCount == 0 ) {
        
        return SAPDB_TRUE;
	}
	else{
        m_bNoRows = SAPDB_FALSE;
    }    
    return SAPDB_TRUE;

}
/*===========================================================================*
 *  StudioOAL_WResult::openResultPrepare
 *===========================================================================*/
SAPDB_Bool StudioOAL_WResult::openResultPrepare(const StudioOAL_String &sCmdStr)
{ 
 
    //check it
	if( m_pError == NULL) return SAPDB_FALSE;
    if( m_hDbc == SQL_NULL_HDBC || m_hEnv == SQL_NULL_HENV)  return SAPDB_FALSE;
	
    //init	variables
	SQLRETURN  nRc = SQL_SUCCESS;    
    m_sStatement   = sCmdStr;
    m_lFetchedRows = 0;
    m_bNoRows      = SAPDB_TRUE;	

    //if there is a open result the close it
    if(m_hStmt != SQL_NULL_HSTMT) 
	   closeResult();

    //allocate handle for statement    
	nRc = SQLAllocHandle(SQL_HANDLE_STMT,m_hDbc,&m_hStmt);
	if( nRc != SQL_SUCCESS)
	{
		if(! (m_pError->checkSQLReturnCode(nRc,m_hStmt))) {

			SQLFreeStmt(m_hStmt, SQL_DROP);
			m_hStmt = SQL_NULL_HSTMT;
			return SAPDB_FALSE;
		}
	}
	
	nRc = SQLSetStmtOption(m_hStmt,SQL_SQLMODE,getSqlMode()); 
    if( nRc != SQL_SUCCESS ) {
    
		if(! (m_pError->checkSQLReturnCode(nRc,m_hStmt))) {
    
            SQLFreeStmt(m_hStmt, SQL_DROP);
			m_hStmt = SQL_NULL_HSTMT;
            return SAPDB_FALSE;
        }
    }

    nRc = SQLSetStmtOption(m_hStmt, SQL_ATTR_ROW_BIND_TYPE, SQL_BIND_BY_COLUMN);
    if( nRc != SQL_SUCCESS ) {
    
		if(! (m_pError->checkSQLReturnCode(nRc,m_hStmt))) {
    
            SQLFreeStmt(m_hStmt, SQL_DROP);
			m_hStmt = SQL_NULL_HSTMT;
            return SAPDB_FALSE;
        }
    }
    
    switch (m_enumResultCursorType)
    {
        case stoalCursorForwardOnly:
    
            nRc = SQLSetStmtOption(m_hStmt, SQL_ATTR_CONCURRENCY, SQL_CONCUR_READ_ONLY);
            if( nRc != SQL_SUCCESS ) {
                
                if(! (m_pError->checkSQLReturnCode(nRc,m_hStmt))) {
        
                    SQLFreeStmt(m_hStmt, SQL_DROP);
					m_hStmt = SQL_NULL_HSTMT;
                    return SAPDB_FALSE;
                }
            }
            nRc = SQLSetStmtOption(m_hStmt, SQL_ATTR_CURSOR_TYPE, SQL_CURSOR_FORWARD_ONLY);
            if( nRc != SQL_SUCCESS ) {
              if(! (m_pError->checkSQLReturnCode(nRc,m_hStmt))) {
    
                    SQLFreeStmt(m_hStmt, SQL_DROP);
					m_hStmt = SQL_NULL_HSTMT;
                    return SAPDB_FALSE;
                }
            }

			break;
        
        case stoalCursorStatic:

            nRc = SQLSetStmtOption(m_hStmt, SQL_ATTR_CONCURRENCY, SQL_CONCUR_ROWVER);
            if( nRc != SQL_SUCCESS ) {
                if(! (m_pError->checkSQLReturnCode(nRc,m_hStmt))) {
        
                    SQLFreeStmt(m_hStmt, SQL_DROP);
					m_hStmt = SQL_NULL_HSTMT;
                    return SAPDB_FALSE;
                }
            }
            
            nRc = SQLSetStmtOption(m_hStmt, SQL_ATTR_CURSOR_TYPE, SQL_CURSOR_STATIC);
            if( nRc != SQL_SUCCESS ) {
                
                if(! (m_pError->checkSQLReturnCode(nRc,m_hStmt))) {
                
                    SQLFreeStmt(m_hStmt, SQL_DROP);
					m_hStmt = SQL_NULL_HSTMT;
                    return SAPDB_FALSE;
                }
            }
			break;
            
		case stoalOptimizeCursor:
    
			nRc = SQLSetStmtOption(m_hStmt, SQL_ATTR_CONCURRENCY, SQL_CONCUR_READ_ONLY);
			if( nRc != SQL_SUCCESS ) {
            
				if(! (m_pError->checkSQLReturnCode(nRc,m_hStmt))) {
    
					SQLFreeStmt(m_hStmt, SQL_DROP);
					m_hStmt = SQL_NULL_HSTMT;
					return SAPDB_FALSE;
				}
			}
        
			nRc = SQLSetStmtOption(m_hStmt, SQL_ATTR_OPTIMIZE_CURSOR, 1);
			if( nRc != SQL_SUCCESS ) {
        
				if(! (m_pError->checkSQLReturnCode(nRc,m_hStmt))) {

					//Die Option ExtendedFetch ohne for reuse gibts erst ab 6.2.10
					if(m_pError->getSQLStateErrorNumber() == "S1092") {  // Out of Option range
							
                
						SQLFreeStmt(m_hStmt, SQL_DROP);
						m_hStmt = SQL_NULL_HSTMT;
						return SAPDB_FALSE;
					}
				}
			}
			nRc = SQLSetStmtOption(m_hStmt, SQL_ATTR_CURSOR_TYPE, SQL_CURSOR_KEYSET_DRIVEN);
			if( nRc != SQL_SUCCESS ) {
        
				if(! (m_pError->checkSQLReturnCode(nRc,m_hStmt))) {

					SQLFreeStmt(m_hStmt, SQL_DROP);
					m_hStmt = SQL_NULL_HSTMT;
					return SAPDB_FALSE;
				}
			}
			break;
         
		default:
			return SAPDB_FALSE;
			break;   
   }

   nRc = SQLSetStmtOption(m_hStmt, SQL_ATTR_USE_BOOKMARKS, SQL_UB_ON);
   if( nRc != SQL_SUCCESS ) {

        if(! (m_pError->checkSQLReturnCode(nRc,m_hStmt))) {
    
            SQLFreeStmt(m_hStmt, SQL_DROP);
			m_hStmt = SQL_NULL_HSTMT;
            return SAPDB_FALSE;
        }
    }
    
    nRc = SQLSetStmtOption(m_hStmt, SQL_ROWSET_SIZE, 1);
    if( nRc != SQL_SUCCESS ) {

        if(! (m_pError->checkSQLReturnCode(nRc,m_hStmt))) {
    
            SQLFreeStmt(m_hStmt, SQL_DROP);
			m_hStmt = SQL_NULL_HSTMT;
            return SAPDB_FALSE;
        }
    }
    
    //prepare the result set
    if(!prepareStmt()){

        //free statement
        SQLFreeStmt(m_hStmt, SQL_DROP);
		m_hStmt = SQL_NULL_HSTMT;
        return SAPDB_FALSE;
    }
    
    if(!getColDescriptions()) 	return SAPDB_FALSE;
	
    //is there a result
	m_lRowCount = rowCount(&m_hStmt);
    if( m_lRowCount == 0 ) {
        
        return SAPDB_TRUE;
	}
	else{
        m_bNoRows = SAPDB_FALSE;
    }    
    return SAPDB_TRUE;

}
/*===========================================================================*
 *  StudioOAL_WResult::rowCount()
 *===========================================================================*/
SAPDB_Long StudioOAL_WResult::rowCount(SQLHANDLE *hStmt)
{

    SQLRETURN	 nRc = 0;
	SQLLEN		 lRowCount = 0;

     //check it
    if( m_hDbc == SQL_NULL_HDBC || m_hEnv == SQL_NULL_HENV || m_pError == NULL) return 0;
	
    // Try to get the number of rows
    nRc = SQLRowCount(*hStmt, &lRowCount);  //!!IA64
	if( nRc != SQL_SUCCESS){
    
        if(!(m_pError->checkSQLReturnCode(nRc,*hStmt)))  return 0;        
    }
    
	return lRowCount;

}
/*===========================================================================*
 *  StudioOAL_WResult::closeResult()
 *===========================================================================*/
void StudioOAL_WResult::closeResult()
{       
    
    //check it
    if( m_hStmt == SQL_NULL_HSTMT) return;	    
	SQLFreeStmt(m_hStmt, SQL_DROP);    	
    m_hStmt = SQL_NULL_HSTMT;

}
/*===========================================================================*
 *  StudioOAL_WResult::getColDescriptions
 *===========================================================================*/
SAPDB_Bool StudioOAL_WResult::getColDescriptions() 
{

    SQLUSMALLINT		lRun;    
    SQLSMALLINT			nColNameLen;
    SQLSMALLINT			nColType;
	SQLULEN		        lColLength;   //!!IA64
    SQLRETURN			nRc;
    SQLSMALLINT			nColDecimal;
    SQLSMALLINT			nColNullable;

	SQLWCHAR  			    szColNameBuffer[MX_COLUMN_NAME];
	StudioOAL_String strColName;
    
    //check it
	if( m_pError == NULL) return SAPDB_FALSE;
    if( m_hDbc == SQL_NULL_HDBC || m_hEnv == SQL_NULL_HENV) return SAPDB_FALSE;	
	

    //get number of columns
    nRc = SQLNumResultCols(m_hStmt, &m_nColCount);
    if( nRc != SQL_SUCCESS) {
    
        if(! (m_pError->checkSQLReturnCode(nRc,m_hStmt))) return SAPDB_FALSE;		
	}    
    
    //get the column descriptions
    for( lRun = 1; lRun <= m_nColCount;lRun++ )
	{
    
        m_pColumn[lRun-1] = new StudioOAL_WColumn;

 		//get the individual column description
        nRc = SQLDescribeColW(m_hStmt,
							 lRun,
							 (SQLWCHAR *)szColNameBuffer, 
							 MX_COLUMN_NAME,
							 &nColNameLen, 
							 &nColType,
							 &lColLength,
							 &nColDecimal,
							 &nColNullable);
        if( nRc != SQL_SUCCESS ){        

               if( !(m_pError->checkSQLReturnCode(nRc,m_hStmt)) ) {    

				   return SAPDB_FALSE;
			   }
        }
        
        strColName.ConvertFromUCS2((SAPDB_UCS2*)szColNameBuffer, (SAPDB_UCS2*)(szColNameBuffer+nColNameLen));        

		//set column description
		m_pColumn[lRun-1]->setColName (strColName); 
        m_pColumn[lRun-1]->setODBCColType(nColType);
        m_pColumn[lRun-1]->setColLength((SQLUINTEGER) lColLength);
        m_pColumn[lRun-1]->setColDecimal(nColDecimal);
        
        if (nColNullable != 0)        
            m_pColumn[lRun-1]->setColNullable(SAPDB_TRUE);		        
		else
            m_pColumn[lRun-1]->setColNullable(SAPDB_FALSE);
    }

	for (lRun = 1023; lRun > m_nColCount; --lRun)
	{
		if(m_pColumn[lRun] != NULL )
		{
			delete m_pColumn[lRun];
			m_pColumn[lRun] = NULL;
		}
	}

	return SAPDB_TRUE;
}
/*===========================================================================*
 *  StudioOAL_WResult::getColumn
 *===========================================================================*/
StudioOAL_WColumn  * StudioOAL_WResult::getColumn( SAPDB_UInt2 nCol )
{ 
	assert (nCol <= m_nColCount && nCol+1 >= 1) ;
	return m_pColumn[nCol-1] ;
}
/*===========================================================================*
 *  StudioOAL_WResult::getColumnName
 *===========================================================================*/
SAPDB_Bool StudioOAL_WResult::getColumnName(SAPDB_UInt2 nCol,StudioOAL_String &sColName)
{
	
	//init
	sColName.Erase();
	if (nCol > m_nColCount || nCol < 1)	return SAPDB_FALSE;	
	m_pColumn[nCol-1]->getColName (sColName); 

	return SAPDB_TRUE;
	
}
/*===========================================================================*
 *  StudioOAL_WResult::execStmt()
 *===========================================================================*/
SAPDB_Bool StudioOAL_WResult::execStmt() 
{    
	/*-------------------------------------------------------------------------*
     * check if pointer to error class is not null 						       *
     *-------------------------------------------------------------------------*/
	if (m_pError == NULL)	return SAPDB_FALSE;	

	/*-------------------------------------------------------------------------*
     * check if dbc handle and enviroment handle are not null 			       *
     *-------------------------------------------------------------------------*/
    if( m_hDbc == SQL_NULL_HDBC || m_hEnv == SQL_NULL_HENV) return SAPDB_FALSE;	

	/*-------------------------------------------------------------------------*
     * local variables										 			       *
     *-------------------------------------------------------------------------*/
	SQLRETURN	nRc = 0;	
	tsp81_UCS2Char * pConvertOut = NULL;
	
   /*-------------------------------------------------------------------------*
    * use a copy for conversion	to ucs2pointer								  *
    *-------------------------------------------------------------------------*/
	StudioOAL_String strTemp = m_sStatement;

	/*-------------------------------------------------------------------------*
     * init conversion buffer								 			       *
     *-------------------------------------------------------------------------*/
	memset(m_strCmdBuffer,0,sizeof(m_strCmdBuffer));

	/*-------------------------------------------------------------------------*
     * convert utf8 to ucs2									 			       *
     *-------------------------------------------------------------------------*/
	if (strTemp.ConvertToUCS2_Unaligned_Swap(m_strCmdBuffer,
							                (m_strCmdBuffer+(sizeof(m_strCmdBuffer)/sizeof(SAPDB_UCS2))),
							                 pConvertOut) != Tools_UTF8Basis::Success )
	{
		return SAPDB_FALSE;		
	}
	
	/*-------------------------------------------------------------------------*
     * check if something was converted						 			       *
     *-------------------------------------------------------------------------*/
	if(pConvertOut<=m_strCmdBuffer) return SAPDB_FALSE;

	/*-------------------------------------------------------------------------*
     * execute statement									 			       *
     *-------------------------------------------------------------------------*/
	nRc = SQLExecDirectW(m_hStmt,(SQLWCHAR*)m_strCmdBuffer, ((SQLINTEGER)(pConvertOut-m_strCmdBuffer)));
	if (nRc != SQL_SUCCESS) {
    
        if (!(m_pError->checkSQLReturnCode(nRc,m_hStmt))) {    

			return SAPDB_FALSE;
		}
	}    

	return SAPDB_TRUE;	
}
/*===========================================================================*
 *  StudioOAL_WResult::prepareStmt()
 *===========================================================================*/
SAPDB_Bool StudioOAL_WResult::prepareStmt() 
{    
	/*-------------------------------------------------------------------------*
     * check if pointer to error class is not null 						       *
     *-------------------------------------------------------------------------*/
	if (m_pError == NULL)	return SAPDB_FALSE;	

	/*-------------------------------------------------------------------------*
     * check if dbc handle and enviroment handle are not null 			       *
     *-------------------------------------------------------------------------*/
    if( m_hDbc == SQL_NULL_HDBC || m_hEnv == SQL_NULL_HENV) return SAPDB_FALSE;	

	/*-------------------------------------------------------------------------*
     * local variables										 			       *
     *-------------------------------------------------------------------------*/
	SQLRETURN	nRc = 0;	
	tsp81_UCS2Char * pConvertOut = NULL;
	
   /*-------------------------------------------------------------------------*
    * use a copy for conversion	to ucs2pointer								  *
    *-------------------------------------------------------------------------*/
	StudioOAL_String strTemp = m_sStatement;

	/*-------------------------------------------------------------------------*
     * init conversion buffer								 			       *
     *-------------------------------------------------------------------------*/
	memset(m_strCmdBuffer,0,sizeof(m_strCmdBuffer));

	/*-------------------------------------------------------------------------*
     * convert utf8 to ucs2									 			       *
     *-------------------------------------------------------------------------*/
	if (strTemp.ConvertToUCS2_Unaligned_Swap(m_strCmdBuffer,
                                                 (m_strCmdBuffer+(sizeof(m_strCmdBuffer)/sizeof(SAPDB_UCS2))),
                                                 pConvertOut) != Tools_UTF8Basis::Success )
	{
		return SAPDB_FALSE;		
	}
	
	/*-------------------------------------------------------------------------*
     * check if something was converted						 			       *
     *-------------------------------------------------------------------------*/
	if(pConvertOut<=m_strCmdBuffer) return SAPDB_FALSE;

	/*-------------------------------------------------------------------------*
         * prepare statement									 	       *
         *-------------------------------------------------------------------------*/
	nRc = SQLPrepareW(m_hStmt,(SQLWCHAR*)m_strCmdBuffer, ((SQLINTEGER)(pConvertOut-m_strCmdBuffer)));
	if (nRc != SQL_SUCCESS) {
    
        if (!(m_pError->checkSQLReturnCode(nRc,m_hStmt))) {    

			return SAPDB_FALSE;
		}
	}    

	return SAPDB_TRUE;	
}
/*===========================================================================*
 *  StudioOAL_WResult::executePreparedStmt()
 *===========================================================================*/
SAPDB_Bool StudioOAL_WResult::executePreparedStmt() 
{
  if (m_hStmt == SQL_NULL_HSTMT)
    return SAPDB_FALSE;
  else {
    SQLRETURN nRc = SQLExecute (m_hStmt);
    if (nRc != SQL_SUCCESS) {
      if (!(m_pError->checkSQLReturnCode(nRc,m_hStmt))) {    
        return SAPDB_FALSE;
      }
    } else {
      //is there a result
      m_lRowCount = rowCount(&m_hStmt);
      if( m_lRowCount == 0 ) {
        return SAPDB_TRUE;
      }
      else{
        m_bNoRows = SAPDB_FALSE;
      }    
    }
  }
  return SAPDB_TRUE;	
}
/*===========================================================================*
 *  StudioOAL_WResult::fetch() 
 *===========================================================================*/
SAPDB_Bool StudioOAL_WResult::fetch() 
{

	/*-------------------------------------------------------------------------*
     * check if pointer to error class is not null 						       *
     *-------------------------------------------------------------------------*/
	if (m_pError == NULL)	return SAPDB_FALSE;

	/*-------------------------------------------------------------------------*
     * check if dbc handle and enviroment handle are not null 			       *
     *-------------------------------------------------------------------------*/
    if (m_hDbc == SQL_NULL_HDBC || m_hEnv == SQL_NULL_HENV || m_hStmt == SQL_NULL_HSTMT) return SAPDB_FALSE;	
    
	/*-------------------------------------------------------------------------*
     * local variable										 			       *
     *-------------------------------------------------------------------------*/ 
	SQLRETURN	nRc = 0;
    
    /*-------------------------------------------------------------------------*
     * init member											 			       *
     *-------------------------------------------------------------------------*/ 
    m_bNoRows = SAPDB_FALSE;
    
	/*-------------------------------------------------------------------------*
     * fetch row											 			       *
     *-------------------------------------------------------------------------*/ 
	nRc = SQLFetch(m_hStmt);
    
    if (!(m_pError->checkSQLReturnCode(nRc,m_hStmt))) {    

		m_bNoRows = SAPDB_TRUE;			
		return SAPDB_FALSE;
	}
	    
	if (m_pError->getReturnCode() == SQL_NO_DATA_FOUND)
    
        m_bNoRows = SAPDB_TRUE;				
	else
        m_lFetchedRows ++;
	

	return SAPDB_TRUE;


}
/*===========================================================================*
 *  StudioOAL_WResult::extendedFetch
 *===========================================================================*/
SAPDB_Bool StudioOAL_WResult::extendedFetch(SAPDB_UInt2 lFetchType,SAPDB_ULong lRow,SAPDB_ULong *NoRowsFetched,SAPDB_UInt2 *nRowStatus) 
{
	SQLRETURN	nRc;
    
    //inits
    m_bNoRows = SAPDB_FALSE;
	*NoRowsFetched = 0;

	//check it
	if( m_pError == NULL) 
		return SAPDB_FALSE;	
	
    if( m_hDbc == SQL_NULL_HDBC || 
		m_hEnv == SQL_NULL_HENV || 
		m_hStmt == SQL_NULL_HSTMT)  
		return SAPDB_FALSE;
	

	if (lFetchType == SQL_FETCH_LAST  || 
		lFetchType == SQL_FETCH_FIRST ||
		lFetchType == SQL_FETCH_ABSOLUTE )	
		m_lFetchedRows = 0;

	SQLULEN * pNoRowsFetched = NoRowsFetched;

		    
    //fetch
	nRc = SQLExtendedFetch(m_hStmt, lFetchType, lRow, pNoRowsFetched, nRowStatus); //!! IA64
    
    if (!(m_pError->checkSQLReturnCode(nRc,m_hStmt))) {    

		m_bNoRows = SAPDB_TRUE;			
		return SAPDB_FALSE;
	}
        
	if( m_pError->getReturnCode() == SQL_NO_DATA_FOUND)
    
        m_bNoRows = SAPDB_TRUE;			
	else
        m_lFetchedRows ++;	

	return SAPDB_TRUE;

}
/*===========================================================================*
 *  StudioOAL_WResult::getLongData
 *===========================================================================*/
SAPDB_Bool StudioOAL_WResult::getLongData(SAPDB_UInt2 nCol,
										   SAPDB_Byte *pValue,
										   SAPDB_Long lBufferSize,
										   SAPDB_Long* lDataSize,
										   SAPDB_Bool* bDataTrunc)
{

	SQLRETURN			nRc = 0;
	SQLLEN				lValueLen = 0;	    
	
    if( m_hDbc == SQL_NULL_HDBC || 
		m_hEnv == SQL_NULL_HENV ||
		m_hStmt == SQL_NULL_HSTMT) 	return SAPDB_FALSE;	
        
    if (m_pColumn[nCol-1] == NULL)	return SAPDB_FALSE;	
	if( m_pError == NULL)	return SAPDB_FALSE;

	//get the data for the current col
    if(m_pColumn[nCol-1]->getODBCColType() != SQL_LONGVARBINARY &&
	   m_pColumn[nCol-1]->getODBCColType() != SQL_WLONGVARCHAR  &&
       m_pColumn[nCol-1]->getODBCColType() != SQL_LONGVARCHAR) 	return SAPDB_FALSE;
	
    m_pError->setNoSQLState(SQLSTATE_DATA_TRUNCATED);
    nRc = SQLGetData(m_hStmt, nCol, SQL_BINARY,pValue, 
		            ( SQLINTEGER ) lBufferSize, 
					&lValueLen); //!!IA64
	
	if( nRc != SQL_SUCCESS ){
    
		
        if(! (m_pError->checkSQLReturnCode(nRc,m_hStmt))) {    

			m_pError->setNoSQLState (SQLSTATE_SUCCESS);
			return SAPDB_FALSE;
		}
		m_pError->setNoSQLState (SQLSTATE_SUCCESS);                
     }
	

    //set value
    if (lValueLen > lBufferSize){
		
        m_pColumn[nCol-1]->setColIsNull(SAPDB_FALSE);   
		*lDataSize  = lBufferSize;
		*bDataTrunc = SAPDB_TRUE;
    }
	else if(lValueLen <= lBufferSize  && lValueLen > 0){                        
        m_pColumn[nCol-1]->setColIsNull(SAPDB_FALSE);
		*lDataSize = lValueLen;
		*bDataTrunc = SAPDB_FALSE;

	}else if (lValueLen == 0){
            m_pColumn[nCol-1]->setColIsNull(SAPDB_FALSE);
			*lDataSize = 0;
			*bDataTrunc = SAPDB_FALSE;

    }else if (lValueLen == -1){
            SAPDB_strcpy((char*)pValue,REPOFNULLVALUE);
			m_pColumn[nCol-1]->setColIsNull(SAPDB_TRUE);
			*bDataTrunc = SAPDB_FALSE;
			*lDataSize = 0;
    }	

	return SAPDB_TRUE;
}
/*===========================================================================*
 *  StudioOAL_WResult::getColumnDataAsString
 *===========================================================================*/
SAPDB_Bool StudioOAL_WResult::getColumnDataAsString(SAPDB_UInt2 nCol,StudioOAL_String &Value)
{
 
    SAPDB_Bool			bBoolValue;
    SQLRETURN			nRc = 0;

	tsp81_UCS2Char      *pValue = NULL;
	SQLLEN	  		    lValueLen = 0;  
	SQLLEN	    		lGetDataLen = 0;
	

	Value.Erase();

    if( m_hDbc == SQL_NULL_HDBC || 
		m_hEnv == SQL_NULL_HENV ||
		m_hStmt == SQL_NULL_HSTMT) 	return SAPDB_FALSE;	
        
    if ( m_pColumn[nCol-1] == NULL)	return SAPDB_FALSE;	
	if ( m_pError == NULL)	        return SAPDB_FALSE;	

    //get the data for the current col
    switch (m_pColumn[nCol-1]->getODBCColType())
	{    
		case SQL_DATE:
		case SQL_TIMESTAMP:
        case SQL_TIME:
		case SQL_DECIMAL:
		case SQL_NUMERIC:
        case SQL_REAL:
		case SQL_FLOAT:
        case SQL_DOUBLE:
		case SQL_INTEGER:
			        
            //get the current value            
			pValue = new tsp81_UCS2Char[MX_COLUMN_GENERIC_NUMERIC];
			if (pValue == NULL) return SAPDB_FALSE;
			
			nRc = SQLGetData(m_hStmt, nCol, SQL_WCHAR, pValue, MX_COLUMN_GENERIC_NUMERIC * sizeof(tsp81_UCS2Char), &lValueLen); //!!IA64
            if( nRc != SQL_SUCCESS) {
    
				if(! (m_pError->checkSQLReturnCode(nRc,m_hStmt))) {    

					delete[] pValue;
					return SAPDB_FALSE;
				}
			}                
            lGetDataLen = lValueLen; 

			break;       
                                   
	    case SQL_LONGVARBINARY:
        case SQL_LONGVARCHAR  : 
		case SQL_WLONGVARCHAR : 
			            
            if( m_lLimitForLong >= 0 ){

                 lGetDataLen = m_lLimitForLong;                 
				 //Limit for long column can overwrite max value for long column
                 if( m_lLimitForLong > MX_COLUMN_LONG_VALUE ) 
					 lGetDataLen = MX_COLUMN_LONG_VALUE;   
				 
            }
			else
                lGetDataLen = MX_COLUMN_LONG_VALUE;
            
				pValue = new tsp81_UCS2Char[lGetDataLen];
				if (pValue == NULL) return SAPDB_FALSE;			

				m_pError->setNoSQLState(SQLSTATE_DATA_TRUNCATED);
				nRc = SQLGetData(m_hStmt, nCol, SQL_WCHAR,pValue, lGetDataLen * sizeof(tsp81_UCS2Char), &lValueLen); //!!IA64
				if( nRc != SQL_SUCCESS ){            
					
					if(! (m_pError->checkSQLReturnCode(nRc,m_hStmt))) {    

						m_pError->setNoSQLState (SQLSTATE_SUCCESS);

						delete[] pValue;
						return SAPDB_FALSE;
					}
					m_pError->setNoSQLState (SQLSTATE_SUCCESS);                
				 }				

			break;

        case SQL_BIT:

			//get the current value
			lGetDataLen = 6; 

			pValue = new tsp81_UCS2Char[6];
			if (pValue == NULL) return SAPDB_FALSE;	
			
            nRc = SQLGetData(m_hStmt, nCol, SQL_BIT, &bBoolValue,sizeof(SAPDB_Bool), &lValueLen); 
            if( nRc != SQL_SUCCESS) {
    
				if(! (m_pError->checkSQLReturnCode(nRc,m_hStmt))) {    

					delete[] pValue;
					return SAPDB_FALSE;
				}
			}
            //for bit values put special value TRUE and FALSE
            if(lValueLen > 0){
                if(bBoolValue){
                
                    sp81UCS2strcpy(pValue,m_sTrue);
					lValueLen = sp81UCS2strlen(m_sTrue);
                }
				else{
                    sp81UCS2strcpy(pValue,m_sFalse);
					lValueLen = sp81UCS2strlen(m_sFalse);
                }
			}
			break;

        default:

            if( m_pColumn[nCol-1]->getODBCColType() == SQL_VARBINARY ||
				m_pColumn[nCol-1]->getODBCColType() == SQL_BINARY ){

                //will be transformed from hex to ascii 
                lGetDataLen = m_pColumn[nCol-1]->getColLength() * 2 + 1;
            }
			else
                lGetDataLen = m_pColumn[nCol-1]->getColLength() + 1;  

			pValue = new tsp81_UCS2Char[lGetDataLen];
			if (pValue == NULL) return SAPDB_FALSE;			
            
            //get the current value
            nRc = SQLGetData(m_hStmt, nCol, SQL_WCHAR, pValue, lGetDataLen * sizeof(tsp81_UCS2Char), &lValueLen); 
            if( nRc != SQL_SUCCESS) {
    
				if(! (m_pError->checkSQLReturnCode(nRc,m_hStmt))) {    

					delete[] pValue;
					return SAPDB_FALSE;
				}
			}
            
	}//switch

	//set value
	if (lValueLen > lGetDataLen){         
		m_pColumn[nCol-1]->setColIsNull(SAPDB_FALSE);                				

	}else if(lValueLen <= lGetDataLen && lValueLen > 0){   				
		m_pColumn[nCol-1]->setColIsNull(SAPDB_FALSE);					

	}else if (lValueLen == 0){
		m_pColumn[nCol-1]->setColIsNull(SAPDB_FALSE);			 

	}else if (lValueLen == -1){
		sp81UCS2strcpy(pValue,m_sRepNullValue);
		lValueLen = sp81UCS2strlen(m_sRepNullValue);
		m_pColumn[nCol-1]->setColIsNull(SAPDB_TRUE);
	}

	//if data was truncated then get the possible size lGetDataLen
	if(lValueLen > lGetDataLen) 
	   lValueLen = lGetDataLen; 

	//return value
	if (pValue != NULL && lValueLen > 0){
		if(Value.ConvertFromUCS2(
					(StudioOAL_String::UCS2ConstPointer)pValue,
					(StudioOAL_String::UCS2ConstPointer)pValue+sp81UCS2strlen(pValue)) 
					!= Tools_UTF8Basis::Success){

			delete[] pValue;
			return SAPDB_FALSE;
		}
	}

	delete[] pValue;
	return SAPDB_TRUE;

}
/*===========================================================================*
 *  StudioOAL_WResult::getErrorText
 *===========================================================================*/
const StudioOAL_String& StudioOAL_WResult::getErrorText()
{
	if( m_pError == NULL) return m_strEmty;	
	return m_pError->getErrorText();
}
/*===========================================================================*
 *  StudioOAL_WResult::getSQLState
 *===========================================================================*/
const StudioOAL_String& StudioOAL_WResult::getSQLState()
{
	if( m_pError == NULL) return m_strEmty;	
	return m_pError->getSQLState();
}
/*===========================================================================*
 *  StudioOAL_WResult::getSQLStateErrorNumber
 *===========================================================================*/
const StudioOAL_String& StudioOAL_WResult::getSQLStateErrorNumber()
{
	if( m_pError == NULL) return m_strEmty;	
	return m_pError->getSQLStateErrorNumber();
}
/*===========================================================================*
 *  StudioOAL_WResult::getNativeError
 *===========================================================================*/
SAPDB_Long StudioOAL_WResult::getNativeError()
{
	if( m_pError == NULL) return 0;	
	return m_pError->getNativeError();
}
/*===========================================================================*
 *  StudioOAL_WResult::getNativeErrorAsString
 *===========================================================================*/
const StudioOAL_String& StudioOAL_WResult::getNativeErrorAsString()
{
	if( m_pError == NULL) return m_strEmty;	
	return m_pError->getNativeErrorAsString();
}
/*===========================================================================*
 *  StudioOAL_WResult::execute
 *===========================================================================*/
SAPDB_Bool StudioOAL_WResult::execute()
{
	if (m_pError == NULL)	
		return SAPDB_FALSE;		
    if( m_hDbc == SQL_NULL_HDBC || m_hEnv == SQL_NULL_HENV || m_hStmt == SQL_NULL_HSTMT ) 
		return SAPDB_FALSE;	
	
	// SQLExecute
	SQLRETURN nRc = SQLExecute(m_hStmt);

	if (nRc != SQL_SUCCESS) 
	{
    
        if (!(m_pError->checkSQLReturnCode(nRc,m_hStmt))) 
			return SAPDB_FALSE;
	}    
	return SAPDB_TRUE;
}
/*===========================================================================*
 *  StudioOAL_WResult::getParamData
 *===========================================================================*/
SAPDB_Bool StudioOAL_WResult::getParamData(SAPDB_Int2 *&nParamData)
{
	//check it
	if (m_pError == NULL)	
		return SAPDB_FALSE;		
    if( m_hDbc == SQL_NULL_HDBC || m_hEnv == SQL_NULL_HENV || m_hStmt == SQL_NULL_HSTMT ) 
		return SAPDB_FALSE;	

	SQLRETURN nRc =  SQLParamData(m_hStmt,(SQLPOINTER*)&nParamData);

	if (nRc != SQL_SUCCESS) 
	{
        if (!(m_pError->checkSQLReturnCode(nRc,m_hStmt))) 
			return SAPDB_FALSE;
	}
	
	return SAPDB_TRUE;
}
/*===========================================================================*
 *  StudioOAL_WResult::getParamData
 *===========================================================================*/
SAPDB_Bool StudioOAL_WResult::putParamData(SQLPOINTER pValue,SQLINTEGER nValueLength)
{

	//check it
	if (m_pError == NULL)	
		return SAPDB_FALSE;		
    if( m_hDbc == SQL_NULL_HDBC || m_hEnv == SQL_NULL_HENV || m_hStmt == SQL_NULL_HSTMT  ) 
		return SAPDB_FALSE;	

	SQLRETURN nRc = SQLPutData(m_hStmt, pValue, nValueLength);
	
	if (nRc != SQL_SUCCESS) 
	{
        if (!(m_pError->checkSQLReturnCode(nRc,m_hStmt))) {    

			return SAPDB_FALSE;
		}
	} 

	return SAPDB_TRUE;
}
/*===========================================================================*
 *  StudioOAL_WResult::getColumnDescription
 *===========================================================================*/
SAPDB_Bool  StudioOAL_WResult::getColumnDescription()
{
	SQLUSMALLINT		    lRun;    
    SQLSMALLINT			    nColNameLen;
    SQLSMALLINT			    nColType;
	SQLULEN   		        lColLength;
    SQLRETURN			    nRc;
    SQLSMALLINT			    nColDecimal;
    SQLSMALLINT			    nColNullable;
	SQLWCHAR  			    szColNameBuffer[MX_COLUMN_NAME];

	StudioOAL_String strColName;
    
    //check it
	if( m_pError == NULL) return SAPDB_FALSE;
    if( m_hDbc == SQL_NULL_HDBC || m_hEnv == SQL_NULL_HENV) return SAPDB_FALSE;	
	
    //get number of columns
    nRc = SQLNumResultCols(m_hStmt, &m_nColCount);
    if( nRc != SQL_SUCCESS)
	{    
        if(! (m_pError->checkSQLReturnCode(nRc,m_hStmt))) return SAPDB_FALSE;		
	}
    
    //get the column descriptions
    for( lRun = 1; lRun <= m_nColCount;lRun++ )
	{
    
        m_pColumn[lRun-1] = new StudioOAL_WColumn;

 		//get the individual column description
        nRc = SQLDescribeColW(m_hStmt,
							 lRun,
							 (SQLWCHAR *)szColNameBuffer, 
							 MX_COLUMN_NAME,
							 &nColNameLen, 
							 &nColType,
							 &lColLength,
							 &nColDecimal,
							 &nColNullable);
        if( nRc != SQL_SUCCESS )
		{
			if( !(m_pError->checkSQLReturnCode(nRc,m_hStmt)) )
			{
				return SAPDB_FALSE;
			}
        }

        //set column description
		strColName.ConvertFromUCS2((SAPDB_UCS2*)szColNameBuffer, (SAPDB_UCS2*)(szColNameBuffer+nColNameLen));        

		//set column description
		m_pColumn[lRun-1]->setColName    (strColName); 
        m_pColumn[lRun-1]->setODBCColType(nColType);
        m_pColumn[lRun-1]->setColLength  (lColLength);
        m_pColumn[lRun-1]->setColDecimal (nColDecimal);
        
        if (nColNullable != 0)        
            m_pColumn[lRun-1]->setColNullable(SAPDB_TRUE);		        
		else
            m_pColumn[lRun-1]->setColNullable(SAPDB_FALSE);
    }

	for (lRun = 1023; lRun > m_nColCount;lRun-- )
	{
		if(m_pColumn[lRun] != NULL )
		{
			delete m_pColumn[lRun];
			m_pColumn[lRun] = NULL;
		}
	}
    
	return SAPDB_TRUE;
}
/*===========================================================================*
 *  StudioOAL_WResult::openParamResult
 *===========================================================================*/
SAPDB_Bool StudioOAL_WResult::openParamResult(const StudioOAL_String &sCmdStr)
{
	
	//if there is a open result the close it
    if(m_hStmt != SQL_NULL_HSTMT)
	   closeResult();

	m_bDBProcedureResult = SAPDB_TRUE;
	StudioOAL_WParameters* poParameterDefinitions  = NULL;    
    
    //inits
	m_nColCount = 0;
    m_sStatement = sCmdStr;    

    if(fullParameterExecute()){                

        if(m_ParamList.size() < 1){
        
            m_lRowCount = 0;
            m_nColCount = 0;
            m_bNoRows = SAPDB_TRUE;
            return SAPDB_FALSE;
		}
	}
    else
        return SAPDB_FALSE;                        
                    
    m_nColCount = 0;
    m_bNoRows = SAPDB_FALSE;
    m_lRowCount = 1;
	StudioOAL_String sColName;
	char sParamNo[8];
   
	poParameterDefinitions = (StudioOAL_WParameters*)m_ParamList.first();
    while (poParameterDefinitions != NULL){
    
        m_pColumn[m_nColCount] = new StudioOAL_WColumn;
		
        //build colname
		 switch (poParameterDefinitions->getParameterType())
		 {        
		 case StudioOAL_WParameters::ptUnknown:
                sColName = PARAM_UNKNOWN;
				break;
            case StudioOAL_WParameters::ptIn:
                sColName = PARAM_IN;
				break;
            case StudioOAL_WParameters::ptInOut:
                sColName = PARAM_INOUT;
				break;
            case StudioOAL_WParameters::ptResultCol:
                sColName = PARAM_RESULTCOL;
				break;
            case StudioOAL_WParameters::ptOut:
                sColName = PARAM_OUT;
				break;
            case StudioOAL_WParameters::ptReturnValue:
                sColName = PARAM_RETURNVALUE;
				break;
			default:
				break;
        }
        
        sColName +=  " (";
		memset(sParamNo,0,8);	
		sprintf(sParamNo,"%d",poParameterDefinitions->getParameterNo());				
		sColName += sParamNo;
		sColName += ")";
		
        m_pColumn[m_nColCount]->setColName (sColName); 
		m_pColumn[m_nColCount]->setColNullable(SAPDB_FALSE);		                

		m_nColCount = m_nColCount + 1;        

		poParameterDefinitions = (StudioOAL_WParameters*)m_ParamList.next();
    }

	return SAPDB_TRUE;
}
/*===========================================================================*
 *  StudioOAL_WResult::fullParameterExecute()
 *===========================================================================*/
SAPDB_Bool StudioOAL_WResult::fullParameterExecute()
{    
    
    SQLRETURN		nRc = SQL_SUCCESS;
    
    m_lFetchedRows = 0;
    m_bNoRows = SAPDB_TRUE;	

    //if there is a open result the close it
    if(m_hStmt != SQL_NULL_HSTMT)
	   closeResult();

	clearParameters();

    //allocate handle for statement    
	nRc = SQLAllocHandle(SQL_HANDLE_STMT,m_hDbc,&m_hStmt);
	if(nRc != SQL_SUCCESS)
	{	
		SQLFreeStmt(m_hStmt, SQL_DROP);
		m_hStmt = SQL_NULL_HSTMT;
		return SAPDB_FALSE;	
	}		
    if (!prepareForParameters()) {
    
        //free statement
        SQLFreeStmt(m_hStmt, SQL_DROP);
		m_hStmt = SQL_NULL_HSTMT;
        return SAPDB_FALSE;
	}

    //open the result set
    if(!execStmt() ) {

        //free statement
        SQLFreeStmt(m_hStmt, SQL_DROP);
		m_hStmt = SQL_NULL_HSTMT;
        return SAPDB_FALSE;
    }

    //free statement
    SQLFreeStmt(m_hStmt, SQL_DROP);
    m_hStmt = SQL_NULL_HSTMT;
    
    return SAPDB_TRUE;
}
/*===========================================================================*
 *  StudioOAL_WResult::prepareForParameters
 *===========================================================================*/
SAPDB_Bool StudioOAL_WResult::prepareForParameters()
{
    
    /*-------------------------------------------------------------------------*
     * check if pointer to error class is not null 						       *
     *-------------------------------------------------------------------------*/
	if (m_pError == NULL)	       return SAPDB_FALSE;	

	/*-------------------------------------------------------------------------*
     * check if dbc handle and enviroment handle are not null 			       *
     *-------------------------------------------------------------------------*/
    if( m_hDbc == SQL_NULL_HDBC || m_hEnv == SQL_NULL_HENV) return SAPDB_FALSE;	
	if( m_hStmt == SQL_NULL_HSTMT) return SAPDB_FALSE;

	/*-------------------------------------------------------------------------*
     * local variables										 			       *
     *-------------------------------------------------------------------------*/
	SQLRETURN	     nRc = 0;	
	tsp81_UCS2Char * pConvertOut = NULL;
	
   /*-------------------------------------------------------------------------*
    * use a copy for conversion	to ucs2pointer								  *
    *-------------------------------------------------------------------------*/
	StudioOAL_String strTemp = m_sStatement;

	/*-------------------------------------------------------------------------*
     * init conversion buffer								 			       *
     *-------------------------------------------------------------------------*/
	memset(m_strCmdBuffer,0,sizeof(m_strCmdBuffer));

	/*-------------------------------------------------------------------------*
     * convert utf8 to ucs2									 			       *
     *-------------------------------------------------------------------------*/
	if (strTemp.ConvertToUCS2_Unaligned_Swap(m_strCmdBuffer,
							                (m_strCmdBuffer+(sizeof(m_strCmdBuffer)/sizeof(SAPDB_UCS2))),
							                 pConvertOut) != Tools_UTF8Basis::Success ) return SAPDB_FALSE;		
	
	/*-------------------------------------------------------------------------*
     * check if something was converted						 			       *
     *-------------------------------------------------------------------------*/
	if(pConvertOut<=m_strCmdBuffer) return SAPDB_FALSE;

	/*-------------------------------------------------------------------------*
     * prepare statement									 			       *
     *-------------------------------------------------------------------------*/
	nRc = SQLPrepareW(m_hStmt,(SQLWCHAR*)m_strCmdBuffer, ((SQLINTEGER)(pConvertOut-m_strCmdBuffer)));
	if (nRc != SQL_SUCCESS) {
    
        if (!(m_pError->checkSQLReturnCode(nRc,m_hStmt))) {    

			return SAPDB_FALSE;
		}
	}    

    // SQLNumParams SQLDescribeParam
    if(!getParameterDescription()){

		return SAPDB_FALSE;
	}
    return SAPDB_TRUE;
}
/*===========================================================================*
 *  StudioOAL_WResult::getParameterDescription
 *===========================================================================*/
SAPDB_Bool StudioOAL_WResult::getParameterDescription() 
{
    SAPDB_Int2  nRc;
    SAPDB_Int2  nParameterCount;
    SAPDB_Int2  nParam;
    SAPDB_Int2  nColType;
    SQLULEN		lColLength; 
	SAPDB_Int2  nColDecimal;
    SAPDB_Int2  nColNullable;
    
	StudioOAL_WParameters  *poNewParamDefinition = NULL;
        
    nRc = SQLNumParams(m_hStmt, &nParameterCount);
    if( nRc != SQL_SUCCESS) return SAPDB_FALSE;
    
    for (nParam = 1 ;nParam <= nParameterCount; nParam ++)
	{
        nRc = SQLDescribeParam(m_hStmt, nParam, &nColType, &lColLength, &nColDecimal, &nColNullable); //!!IA64
        if( nRc != SQL_SUCCESS) return SAPDB_FALSE;
        
        poNewParamDefinition = new StudioOAL_WParameters(m_hStmt,nParam);                
        
        // -99 <= nColType >= +99) then parameter type is input
        // -9900 <= nColType >= -100 Or 100 <= nColType >= 9900) then parameter type is output
        // -10099 <= nColType >= -10001 Or 10001 <= nColType >= 10099) then parameter type is input/output
        poNewParamDefinition->setParameterType(StudioOAL_WParameters::ptUnknown);
        nColType *= (nColType > 0) ? 1 :-1;
        if( nColType > 99){

          if( nColType > 9900){

            poNewParamDefinition->setParameterType(StudioOAL_WParameters::ptInOut);
            if( nColType < 0)
              poNewParamDefinition->setColumnType(nColType + 10000);
            else
              poNewParamDefinition->setColumnType (nColType - 10000);
		  }
		  else{

			poNewParamDefinition->setParameterType(StudioOAL_WParameters::ptOut);
            poNewParamDefinition->setColumnType( nColType / 100);
          }
		}
		else{

          poNewParamDefinition->setParameterType(StudioOAL_WParameters::ptIn);
          poNewParamDefinition->setColumnType(nColType);
        }
		m_ParamList.append(poNewParamDefinition);
	}    	
    
    return SAPDB_TRUE;
}
/*===========================================================================*
 *  StudioOAL_WResult::clearParameters
 *===========================================================================*/
void StudioOAL_WResult::clearParameters() 
{
    
    StudioOAL_WParameters  *poCurrentParam;
    
	poCurrentParam = (StudioOAL_WParameters*)m_ParamList.first();
    while (poCurrentParam != NULL){
		
		delete poCurrentParam;
		poCurrentParam = (StudioOAL_WParameters*)m_ParamList.next();
	} 

	m_ParamList.clear();
		
    return;
}
/*===========================================================================*
 *  StudioOAL_WResult::getParamValue
 *===========================================================================*/
SAPDB_Bool StudioOAL_WResult::getParamValue(SAPDB_Int2 nParamNo,StudioOAL_String &sValue)
{
	StudioOAL_WParameters  *poCurrentParam;
    
	poCurrentParam = (StudioOAL_WParameters*)m_ParamList.at(nParamNo);
    if (poCurrentParam != NULL)

		poCurrentParam->getValue(sValue);
	else		
		return SAPDB_FALSE;

	return SAPDB_TRUE;

}
/*===========================================================================*
 *  StudioOAL_WResult::getCursorPos
 *===========================================================================*/
SAPDB_Bool StudioOAL_WResult::getCursorPos(SAPDB_Int4 &nCursorPos)
{
	if( m_hDbc == SQL_NULL_HDBC || 
		m_hEnv == SQL_NULL_HENV ||
		m_hStmt == SQL_NULL_HSTMT) 	return SAPDB_FALSE;	

	nCursorPos = 0;
	SQLINTEGER StringLengthPtr;
    SQLRETURN nRc = SQLGetStmtAttr(m_hStmt, SQL_ROW_NUMBER,&nCursorPos,SQL_IS_INTEGER,&StringLengthPtr);
    
    if (nRc != SQL_SUCCESS)	{
    
        if(!(m_pError->checkSQLReturnCode(nRc,m_hStmt))) {
        
            //invalid cursor state => cursor is behind last or before first row
            if( m_pError->getSQLStateErrorNumber() == (SAPDB_UTF8*)"24000") 
                return SAPDB_FALSE;
            else
                nCursorPos = 0;
            
        }
	}

	return SAPDB_TRUE;
}
/*===========================================================================*
 *  StudioOAL_WResult::isColNull
 *===========================================================================*/
SAPDB_Bool StudioOAL_WResult::isColNull(SAPDB_UInt2 nCol,SAPDB_Bool &bIsNull)
{
	if (nCol > m_nColCount || nCol < 1)	return SAPDB_FALSE;	
	bIsNull = m_pColumn[nCol-1]->getColIsNull();

	return SAPDB_TRUE;	
}
/*===========================================================================*
 *  StudioOAL_WResult::getODBCColType
 *===========================================================================*/
SAPDB_Bool StudioOAL_WResult::getODBCColType (SAPDB_UInt2 nCol,SQLSMALLINT &nColType)	
{
	if (nCol > m_nColCount || nCol < 1)	return SAPDB_FALSE;	
	nColType = m_pColumn[nCol-1]->getODBCColType();
	return SAPDB_TRUE;	
}

/*===========================================================================*
 *  END
 *===========================================================================*/
