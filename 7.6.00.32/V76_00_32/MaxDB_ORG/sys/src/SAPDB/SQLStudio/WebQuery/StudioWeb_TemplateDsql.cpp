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

  module: StudioWeb_Template.cpp

  -----------------------------------------------------------------------

  responsible:  WolfgangA

  special area: WebQuery

  description:  sql window template class for WebQuery

************************************************************************/

/*-----------------------------------------------------------------------------
  includes
 -----------------------------------------------------------------------------*/

#include "SAPDB/SQLStudio/WebQuery/StudioWeb_Definitions.hpp"
#include "SAPDB/SQLStudio/WebQuery/StudioWeb_TemplateDsql.hpp"



/*-----------------------------------------------------------------------------
  defines
 -----------------------------------------------------------------------------*/
#define TEMPLATE_NAME     "wqsql.htm"

#define OBJ_PREVDISABLED       "PrevDisabled"
#define OBJ_NEXTDISABLED       "NextDisabled"
#define OBJ_DSQLTEXT		   "DSqlText"
#define OBJ_STOREITEM		   "StoreItem"
#define OBJ_STOREDSTATEMENTPATH "StoredStatementPath*"
#define OBJ_STOREDSTATEMENTPATH1 "StoredStatementPath1"
#define OBJ_STOREDSTATEMENTPATH2 "StoredStatementPath2"

#define OBJ_STATEMENTPROP		"StoredStatementProperties*"


#define OBJ_STATEMENTPROP1		"StoredStatementProperties1"
#define OBJ_STATEMENTPROP2		"StoredStatementProperties2"
#define OBJ_STATEMENTPROP3		"StoredStatementProperties3"
#define OBJ_STATEMENTPROP4		"StoredStatementProperties4"
#define OBJ_STATEMENTPROP5		"StoredStatementProperties5"
#define OBJ_STATEMENTPROP6		"StoredStatementProperties6"
#define OBJ_STATEMENTPROP7		"StoredStatementProperties7"
#define OBJ_STATEMENTPROP8		"StoredStatementProperties8"
#define OBJ_STATEMENTPROP9		"StoredStatementProperties9"
#define OBJ_STATEMENTPROP10		"StoredStatementProperties10"
#define OBJ_STATEMENTPROP11		"StoredStatementProperties11"


#define OBJ_STATEMENTPROPVALUE  " selected "
#define OBJ_STATEMENTTYPESQL   "SQL Window"
#define OBJ_STATEMENTTYPEQBE   "Query By Example"
#define OBJ_STATEMENTTYPEVIQ   "Visual Query"
#define OBJ_STATEMENTTYPETDEF   "Table Defintion"





StudioWeb_TemplateDsql::StudioWeb_TemplateDsql (sapdbwa_WebAgent & wa) 
: Tools_Template ( wa, _Tools_UTF8Ptr(TEMPLATE_NAME) )
                      
{ 
	m_bNextDisabled = SAPDB_TRUE;
	m_bPrevDisabled = SAPDB_TRUE;
	m_bStoreItemVisible = SAPDB_FALSE;
	m_nAutoCommitOn = -1;
	m_bEnableSave = SAPDB_TRUE;
} 




SAPDB_Int2 StudioWeb_TemplateDsql::askForWriteCount(const Tools_DynamicUTF8String &szName)
{
  SAPDB_Int2 nReturn = 0;

  if (szName.Compare(OBJ_PREVDISABLED) == 0) {
    nReturn = 1;
  }
  else if (szName.Compare( OBJ_NEXTDISABLED) == 0) {
    nReturn = 1;
  }
  else if (szName.Compare( OBJ_DSQLTEXT) == 0) {
    nReturn = 1;
  }
  else if (szName.Compare( OBJ_STOREITEM) == 0) {
	  return(m_bStoreItemVisible ? 1 : 0);  
  }

  else if (szName.Compare( OBJ_STATEMENTPROP) == 0) {
	  	  return(m_bStoreItemVisible ? 1 : 0);  
  }
  else if (szName.Compare( OBJ_STOREDSTATEMENTPATH) == 0) {
	  	  return(m_bStoreItemVisible ? 1 : 0);  
  } 

  return nReturn;
} 


Tools_DynamicUTF8String StudioWeb_TemplateDsql::askForValue (const Tools_DynamicUTF8String &szName)
{
  //init return buffer
  m_sBuffer.Erase();

  if (szName.Compare( OBJ_PREVDISABLED) == 0) {

	  return(m_bPrevDisabled ? DISABLED_TRUE : DISABLED_FALSE);	  
  } 
  else if (szName.Compare( OBJ_NEXTDISABLED) == 0) {

    return(m_bNextDisabled ? DISABLED_TRUE : DISABLED_FALSE);	  
  } 
  else if (szName.Compare( OBJ_DSQLTEXT) == 0) {
		return m_sDSqlText;
	
  } 
  else if (szName.Compare( OBJ_STOREITEM) == 0) {
		
	  if(m_sSOPath.Size() > 0 && m_sSOPath != WQ_STOREDSTATEMENTPATH_TAG  && m_bEnableSave)

		return OBJ_STOREITEMTEXT2;
	  else	
		return OBJ_STOREITEMTEXT1;
  } 
  else if (szName.Compare( OBJ_STOREDSTATEMENTPATH1) == 0)
  {		
	  if(m_bStoreItemVisible){

		m_sBuffer = WQ_STOREDSTATEMENTPATH_TAG;
		m_sBuffer +=m_sSOPath;																	  
	  }
	  return (m_sBuffer);	
  } 

  else if (szName.Compare( OBJ_STOREDSTATEMENTPATH2) == 0){		
		
	    if(m_bStoreItemVisible && m_bEnableSave)
			m_sBuffer = m_sSOPath;

		return m_sBuffer;	
  } 

  else if (szName.Compare( OBJ_STATEMENTPROP1) == 0){		

	  if(m_nAutoCommitOn == SQL_AUTOCOMMIT_ON) return  OBJ_STATEMENTPROPVALUE;
	  else return "";
  } 
  
  else if (szName.Compare( OBJ_STATEMENTPROP2) == 0){		

	  if(m_nAutoCommitOn == SQL_AUTOCOMMIT_OFF) return  OBJ_STATEMENTPROPVALUE;
	  else return "";
  } 
  /*
		  typedef enum tpr00_SQLModeEnum {
		  cpr_kind_empty         =   0,
		  cpr_kind_internal      =   1,
		  cpr_kind_db2           =   2,
		  cpr_kind_ansi          =   3,
		  cpr_kind_oracle        =   4,
		  cpr_kind_sapr3         =   5
		} tpr00_SQLModeEnum;
  */
  else if (szName.Compare( OBJ_STATEMENTPROP3) == 0){		

	  return (m_eSQLMode == cpr_kind_internal ? OBJ_STATEMENTPROPVALUE : "");
  } 
  else if (szName.Compare( OBJ_STATEMENTPROP4) == 0){		
		return (m_eSQLMode == cpr_kind_oracle ? OBJ_STATEMENTPROPVALUE : "");
  } 

  else if (szName.Compare( OBJ_STATEMENTPROP5) == 0){		

		return (m_eSQLMode == cpr_kind_db2 ? OBJ_STATEMENTPROPVALUE : "");
  } 

  else if (szName.Compare( OBJ_STATEMENTPROP6) == 0){		

		return (m_eSQLMode == cpr_kind_ansi ? OBJ_STATEMENTPROPVALUE : "");
  } 

  /*
  /// SQL_TXN_ISOLATION_OPTION bitmasks 
	#define SQL_TXN_READ_UNCOMMITTED            0x00000001L
	#define SQL_TRANSACTION_READ_UNCOMMITTED	SQL_TXN_READ_UNCOMMITTED
	#define SQL_TXN_READ_COMMITTED              0x00000002L
	#define SQL_TRANSACTION_READ_COMMITTED		SQL_TXN_READ_COMMITTED
	#define SQL_TXN_REPEATABLE_READ             0x00000004L
	#define SQL_TRANSACTION_REPEATABLE_READ		SQL_TXN_REPEATABLE_READ
	#define SQL_TXN_SERIALIZABLE                0x00000008L
	#define SQL_TRANSACTION_SERIALIZABLE		SQL_TXN_SERIALIZABLE
  */
  else if (szName.Compare( OBJ_STATEMENTPROP7) == 0){		

		return (m_nIsoLevel == SQL_TXN_READ_UNCOMMITTED ? OBJ_STATEMENTPROPVALUE : "");
  } 

  else if (szName.Compare( OBJ_STATEMENTPROP8) == 0){		

		return (m_nIsoLevel == SQL_TXN_READ_COMMITTED ? OBJ_STATEMENTPROPVALUE : "");
  } 

  else if (szName.Compare( OBJ_STATEMENTPROP9) == 0){		

		return (m_nIsoLevel == SQL_TXN_REPEATABLE_READ ? OBJ_STATEMENTPROPVALUE : "");
  } 

  else if (szName.Compare( OBJ_STATEMENTPROP10) == 0){		

		return (m_nIsoLevel == SQL_TXN_SERIALIZABLE ? OBJ_STATEMENTPROPVALUE : "");
  } 

  else if (szName.Compare( OBJ_STATEMENTPROP11) == 0){		

		if (m_emObjectType == Studio_StoredFileContent::storedObjectTypeStatement) return OBJ_STATEMENTTYPESQL;
		else if(m_emObjectType == Studio_StoredFileContent::storedObjectTypeQueryByExample) return OBJ_STATEMENTTYPEQBE;
		else if(m_emObjectType == Studio_StoredFileContent::storedObjectTypeVisualQuery) return OBJ_STATEMENTTYPEVIQ;
		else if(m_emObjectType == Studio_StoredFileContent::storedObjectTypeDraftTable) return OBJ_STATEMENTTYPETDEF;
		else return"";
  } 


  return "";
}
