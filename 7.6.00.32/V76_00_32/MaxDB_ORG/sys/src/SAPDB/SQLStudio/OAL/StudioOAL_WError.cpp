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

  module: StudioOAL_WError.cpp

  -----------------------------------------------------------------------

  responsible: WolfgangA, Hans-GeorgB

  special area: WebQuery, ReplMan, DBAnalyzer

  description:  Error Class for ODBC/SQL Errors

  version     : 7.4


************************************************************************/

#include "SAPDB/SQLStudio/OAL/StudioOAL_WError.hpp"

/*===========================================================================*
 *  StudioOAL_WError::StudioOAL_WError
 *===========================================================================*/
StudioOAL_WError::StudioOAL_WError(SQLHANDLE hDhc,SQLHENV hEnv)
{
    //Init some members 
    m_hEnv         = hEnv;
    m_hDbc         = hDhc; 
    m_bIsError     = SAPDB_FALSE;  
    m_bIsWarning   = SAPDB_FALSE;
    m_lRc          = 0;
    m_lErrPos      = 0;
    m_nNativeError = 0;
    
}
/*===========================================================================*
 *  StudioOAL_WError::checkSQLReturnCode
 *===========================================================================*/
SAPDB_Bool StudioOAL_WError::checkSQLReturnCode(SQLRETURN lReturnCode, SQLHSTMT hStmt)  
{
    SQLRETURN lErrorReturnCode;

    SQLWCHAR SQLState[MX_SQL_STATE+1];    
    SQLWCHAR ErrorMsg[MX_ERROR_MSG+1];

    StudioOAL_String strSQLState;
    StudioOAL_String strErrorText;

    SQLINTEGER  nNativeError ;
    SAPDB_Int2  nErrorMsgLen;

    SAPDB_Char  szErrorNo [256];
    SAPDB_Char* szpConvert = NULL;

    StudioOAL_String strCompBuffer;
    
    //Inits
    m_bIsError     = SAPDB_FALSE;
    m_bIsWarning   = SAPDB_FALSE;
    m_lRc          = lReturnCode;
    m_strNativeError.Erase();
    m_strErrorText = (SAPDB_UTF8*)CANNOT_ANALYZE_ERROR;
    m_strSQLState.Erase();
    m_lErrPos      = 0;
    m_nNativeError = 0;

    //Everything ok
    if (lReturnCode == SQL_SUCCESS)  return SAPDB_TRUE; 
    
    //Invalid handle
    if (lReturnCode == SQL_INVALID_HANDLE){
        
        m_bIsError = SAPDB_TRUE;         
        m_strErrorText = (SAPDB_UTF8*)INVALID_ODBC_HANDLE;
        return SAPDB_FALSE;
    }        

    //get more error information
    if ((lReturnCode == SQL_ERROR) || (lReturnCode == SQL_SUCCESS_WITH_INFO)) {


        /*==========================================================================*/

        //Returns error or status information about the last error. 
        //The driver maintains a stack or list of errors that can be returned for the hstmt, hdbc, and henv arguments, depending on how the call to SQLError is made.
        //The error queue is flushed after each statement.

        //The following table describes the SQLError arguments and return values used by the driver.

        //SQLError argument Return value description 
        //szSQLState The value for the SQLSTATE represented by the error. 
        //pfNativeError A nonzero value indicates a Visual FoxPro ODBC Driver Native Error Message. A value of zero indicates the error has been detected by the driver and mapped to the appropriate ODBC Error Code. 
        //szErrorMsg The text for the native error or ODBC error. 
        //pcbErrorMsg The length of the message text plus the length of the identifiers 

        /*==========================================================================*/

        //get the error information
        lErrorReturnCode = SQLErrorW(m_hEnv,
                                     m_hDbc,
                                     hStmt, 
                                     (SQLWCHAR*)SQLState, 
                                     &nNativeError, 
                                     (SQLWCHAR*)ErrorMsg,
                                     MX_ERROR_MSG, 
                                     &nErrorMsgLen);

        if ((lErrorReturnCode == SQL_SUCCESS) || (lErrorReturnCode == SQL_SUCCESS_WITH_INFO)) {
            
            strSQLState.ConvertFromUCS2((StudioOAL_String::UCS2Pointer)SQLState, (StudioOAL_String::UCS2Pointer)(SQLState+SQLSTATE_LEN));
            strErrorText.ConvertFromUCS2((StudioOAL_String::UCS2Pointer)ErrorMsg,(StudioOAL_String::UCS2Pointer)(ErrorMsg+nErrorMsgLen));

            //save SQL ErrorNumber
            m_strSQLStateErrorNumber = strSQLState;         
    
            //if noSQLState == SQL Error Number then return
            if (strSQLState != (SAPDB_UTF8*)SQLSTATE_SUCCESS) {
            
                if (strSQLState == m_strNoSQLState) {

                    m_lRc = SQL_SUCCESS;
                    m_bIsError = SAPDB_FALSE;
                    return SAPDB_TRUE;
                };
            };

            //if no native error then return
            if (nNativeError != NATIVEERROR_SUCCESS) {

                if (!m_strNoNativeError.Empty()){

                    m_strNoNativeError.ConvertToASCII_Latin1(szErrorNo,szErrorNo+sizeof(szErrorNo),szpConvert);
                    if (nNativeError == atol(szErrorNo)) {

                        m_lRc = SQL_SUCCESS;
                        m_bIsError = SAPDB_FALSE;
                        return SAPDB_TRUE;
                    };
                }
            };          
            
            strCompBuffer = (SAPDB_UTF8*)OPERATION_CANCELED;                                        
            if ((lReturnCode == SQL_SUCCESS_WITH_INFO) || (strSQLState == strCompBuffer)) {            
            
                //Store SQL state and native error
                m_strSQLState.ConvertFromUCS2((StudioOAL_String::UCS2Pointer)SQLState,(StudioOAL_String::UCS2Pointer)(SQLState+SQLSTATE_LEN));
                m_strErrorText.ConvertFromUCS2((StudioOAL_String::UCS2Pointer)ErrorMsg,(StudioOAL_String::UCS2Pointer)(ErrorMsg+nErrorMsgLen));

                m_lRc = SQL_SUCCESS;
                m_bIsWarning = SAPDB_TRUE;
                m_bIsError = SAPDB_FALSE;                
                return SAPDB_TRUE;
            };

            if (nErrorMsgLen != 0  && lReturnCode != SQL_SUCCESS_WITH_INFO) {
                                
                scanErrorText(strErrorText,
                              strSQLState,
                              nNativeError,
                              m_strErrorText,
                              m_strSQLState,
                              m_strNativeError,
                              &m_lErrPos);                                             
            }
            else{        
                m_strErrorText = (SAPDB_UTF8*)CANNOT_ANALYZE_ERROR;        
            };
            m_bIsError = SAPDB_TRUE;
            return SAPDB_FALSE;
        };
    }
    else{
        if (lReturnCode == SQL_NO_DATA)  m_strErrorText = (SAPDB_UTF8*)ROW_NOT_FOUND;
        return SAPDB_TRUE;
    }   
    
    return SAPDB_TRUE;




};
/*===========================================================================*
 *  StudioOAL_WError::scanErrorText
 *===========================================================================*/
void StudioOAL_WError::scanErrorText(const StudioOAL_String &strErrorText,
                                     const StudioOAL_String &strSQLState,
                                     SQLINTEGER  nNativeError,
                                     StudioOAL_String &strOutErrorText,
                                     StudioOAL_String &strOutSQLState,
                                     StudioOAL_String &strOutNativeError,
                                     SAPDB_Int4 *lOutErrPos)
{ 
    StudioOAL_String::BasisElementIndex  i = 0;
    StudioOAL_String::BasisElementIndex Pos1 = 0;
    StudioOAL_String::BasisElementIndex Pos2 = 0;
    StudioOAL_String::BasisElementIndex Pos3 = 0;
    StudioOAL_String::BasisElementIndex Pos4 = 0;
    StudioOAL_String strErrNo; 
    StudioOAL_String strErrMsg;
    StudioOAL_String strErrState;
    StudioOAL_String strErrPos;     

    SAPDB_Char  szErrorPos [256];
    memset(szErrorPos,0,sizeof(szErrorPos));
    SAPDB_Char* szpConvert = NULL;

    /*Init*/    
    m_nNativeError = nNativeError;        

    //Search the SQL state indicator
    if (strSQLState != (SAPDB_UTF8*)SQLSTATE_SUCCESS){

        Pos3 = 0;
        do
        {
            Pos1 = strErrorText.Find((SAPDB_UTF8*)BREAKCLOSE,Pos3 + 1); 
            if (Pos1 == StudioOAL_String::NPos) break;
            Pos2 = strErrorText.Find((SAPDB_UTF8*)SEMICOLON,Pos1 + 1);
            if (Pos2 == StudioOAL_String::NPos) break;
            Pos3 = strErrorText.Find((SAPDB_UTF8*)BREAKOPEN,Pos1 + 1);
            if (Pos3 == StudioOAL_String::NPos) break;
        }
        while (Pos3 > Pos1 && Pos3 < Pos2);
    };

    /*if a native error exists then split the error message
      into state and native error message*/
    if(nNativeError != 0){

        if(Pos2 != StudioOAL_String::NPos){

            strErrState = strErrorText.SubStr(Pos1 + 1,Pos2-Pos1-1);
            strErrMsg   = strErrorText.SubStr(Pos2 + 1);

            Pos4 = strErrMsg.Find((SAPDB_UTF8*)ONESPACE); 
            if(Pos4 != StudioOAL_String::NPos){

                strErrNo = strErrMsg.SubStr(0,Pos4);
            }

            //get error position
            Pos1 = strErrMsg.Find((SAPDB_UTF8*)SUBSTRING1);
            if(Pos1 != StudioOAL_String::NPos){
                
                Pos2 = strErrMsg.Find((SAPDB_UTF8*)SUBSTRING2,Pos1);
                if(Pos2 != StudioOAL_String::NPos){

                    strErrPos = strErrMsg.SubStr(Pos1+4,Pos2-Pos1-4);
                    strErrMsg = strErrMsg.SubStr(Pos2+2);
                };
            };
        };
    }
    else{
        //get the the state error message
        strErrState = strErrorText.SubStr(Pos1+1);
    };

    
    //Set error output params
    strOutErrorText = strErrorText; 
    strOutNativeError = strErrNo;
    strOutSQLState = (SAPDB_UTF8*)SUBSTRING3;
    strOutSQLState.Append(strSQLState);
    strOutSQLState.Append ((SAPDB_UTF8*)BREAKCLOSE);
    strOutSQLState.Append ((SAPDB_UTF8*)ONESPACE);
    strOutSQLState.Append(strErrState);
    strErrPos.ConvertToASCII_Latin1(szErrorPos,szErrorPos+sizeof(szErrorPos),szpConvert);
    *lOutErrPos = atol(szErrorPos);
  
    
};
/*===========================================================================*
 *  StudioOAL_WError::clear
 *===========================================================================*/
void StudioOAL_WError::clear()
{
    
    m_bIsError   = SAPDB_FALSE;
    m_bIsWarning = SAPDB_FALSE;
    m_lRc = 0;
    m_strNativeError = "";
    m_strErrorText = "";
    m_strSQLState = "";
    m_lErrPos = 0;
    m_strNoNativeError = "";
    m_strNoSQLState = "";

}

/*===========================================================================*
 *  END
 *===========================================================================*/


