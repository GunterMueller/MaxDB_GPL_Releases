/*!
  -----------------------------------------------------------------------------
  module: database.cpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: DBanalyzer - The performance analyse tool for SAPDB
  description:  odbc database connection 
  version:      7.4 and higher 
  -----------------------------------------------------------------------------

  Copyright (c) 2002-2005 SAP AG


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




  -----------------------------------------------------------------------------
*/

#include "SAPDB/SQLStudio/OAL/StudioOAL_WCommon.h"
#include "SAPDB/SQLStudio/OAL/StudioOAL_WResult.hpp"
#include "SAPDB/SQLStudio/OAL/StudioOAL_WError.hpp"
#include "SAPDB/DBanalyzer/dbanalyzer.hpp"
#include "SAPDB/DBanalyzer/database.hpp"
#include "SAPDB/DBanalyzer/error.hpp"

extern DBA_error gError;

#define DBA_NEW_SQL_PARAM_INSTANCETYPE  "SELECT VALUE FROM SYSINFO.ACTIVECONFIGURATION  WHERE PARAMETERNAME = 'INSTANCE_TYPE'"
#define DBA_NEW_SQL_INFO_KERNELVERSION  "SELECT ID FROM  SYSINFO.VERSION"
#define DBA_NEW_SQL_PARAM_RUNDIRECTORY  "SELECT VALUE FROM SYSINFO.ACTIVECONFIGURATION  WHERE PARAMETERNAME = 'RUNDIRECTORY'"
#define DBA_NEW_SQL_INFO_SESSION        "SELECT SESSIONID FROM SYSINFO.SESSIONS WHERE OWN='YES'"
#define DBA_NEW_SQL_CHECK_SESSION       "SELECT SESSIONID FROM SYSINFO.SESSIONS WHERE OWN='YES' AND SESSIONID=%d"
#define DBA_NEW_SQL_INFO_PID            "SELECT APPLICATIONPROCESS FROM SYSINFO.SESSIONS WHERE OWN='YES'"
#define DBA_NEW_SQL_CHECK_ANALYZER      "SELECT APPLICATIONPROCESS FROM SYSINFO.SESSIONS WHERE SESSIONID=%u AND APPLICATIONPROCESS=%u"
#define DBA_NEW_SQL_INFO_DATETIME       "SELECT YEAR(NOW()),MONTH(NOW()),DAY(NOW()),HOUR(NOW()),MINUTE(NOW()),SECOND(NOW()) FROM DUAL"

#define DBA_OLD_SQL_PARAM_INSTANCETYPE  "SELECT VALUE FROM DBPARAMETERS WHERE DESCRIPTION = 'INSTANCE_TYPE'"
#define DBA_OLD_SQL_INFO_KERNELVERSION  "SELECT KERNEL FROM VERSIONS"
#define DBA_OLD_SQL_PARAM_RUNDIRECTORY  "SELECT VALUE FROM DBPARAMETERS WHERE DESCRIPTION = 'RUNDIRECTORY'"
#define DBA_OLD_SQL_INFO_SESSION        "SELECT SESSION FROM CONNECTPARAMETERS"
#define DBA_OLD_SQL_CHECK_SESSION       "SELECT SESSION FROM CONNECTPARAMETERS WHERE SESSION=%d"
#define DBA_OLD_SQL_INFO_PID            "SELECT T.APPLPROCESS FROM TRANSACTIONS T,CONNECTPARAMETERS C WHERE T.SESSION = C.SESSION"
#define DBA_OLD_SQL_CHECK_ANALYZER      "SELECT T.APPLPROCESS FROM TRANSACTIONS T,CONNECTPARAMETERS C WHERE T.SESSION = %u AND T.APPLPROCESS = %u"
#define DBA_OLD_SQL_INFO_DATETIME       DBA_NEW_SQL_INFO_DATETIME

#define DBA_SQL_PARAM_INSTANCETYPE     (m_bUseOldSysTables ? DBA_OLD_SQL_PARAM_INSTANCETYPE : DBA_NEW_SQL_PARAM_INSTANCETYPE)
#define DBA_SQL_INFO_KERNELVERSION     (m_bUseOldSysTables ? DBA_OLD_SQL_INFO_KERNELVERSION : DBA_NEW_SQL_INFO_KERNELVERSION)
#define DBA_SQL_PARAM_RUNDIRECTORY     (m_bUseOldSysTables ? DBA_OLD_SQL_PARAM_RUNDIRECTORY : DBA_NEW_SQL_PARAM_RUNDIRECTORY)
#define DBA_SQL_INFO_SESSION           (m_bUseOldSysTables ? DBA_OLD_SQL_INFO_SESSION       : DBA_NEW_SQL_INFO_SESSION      )
#define DBA_SQL_CHECK_SESSION          (m_bUseOldSysTables ? DBA_OLD_SQL_CHECK_SESSION      : DBA_NEW_SQL_CHECK_SESSION     )
#define DBA_SQL_INFO_PID               (m_bUseOldSysTables ? DBA_OLD_SQL_INFO_PID           : DBA_NEW_SQL_INFO_PID          )
#define DBA_SQL_CHECK_ANALYZER         (m_bUseOldSysTables ? DBA_OLD_SQL_CHECK_ANALYZER     : DBA_NEW_SQL_CHECK_ANALYZER    )
#define DBA_SQL_INFO_DATETIME          (m_bUseOldSysTables ? DBA_OLD_SQL_INFO_DATETIME      : DBA_NEW_SQL_INFO_DATETIME     )

#define DBA_SQL_LOCK_CHECK             "EXISTS TABLE SYSDBANA.DBANLOCK"
#define DBA_SQL_LOCK_LOCK              "LOCK (WAIT) TABLE SYSDBANA.DBANLOCK IN EXCLUSIVE MODE"
#define DBA_SQL_COMMIT                 "COMMIT"

class DBA_handles {
private:
  DBA_handles()
    : hEnv(NULL),
      hDbc(NULL)
  {}
  SQLHANDLE  hEnv;
  SQLHANDLE  hDbc;
  friend class DBA_database; 
}; // end class DBA_handles

// ========================================================
// DBA_database::DBA_database
// ========================================================
DBA_database::DBA_database (const DBA_string & sServer,
                            const DBA_string & sDatabase,
                            const DBA_string & sUser,
                            const DBA_string & sPassword) 
             : m_sServer(sServer),
               m_sDatabase(sDatabase),
               m_sUser(sUser),
               m_sPassword(sPassword)
{
  m_pError = NULL;

  m_pHandles = new DBA_handles();

  m_nInstanceType  = DBA_INSTANCETYPE_OLTP;
  m_sKernelVersion = "";
  m_sRundirectory  = "";
  m_nSid           = 0;
  m_nPid           = 0;

  m_bUseOldSysTables = false;
} // end DBA_database::DBA_database

// ========================================================
// DBA_database::~DBA_database
// ========================================================
DBA_database::~DBA_database () 
{
  delete m_pHandles;
} // end DBA_database::~DBA_database

// ========================================================
// DBA_database::Connect
// ========================================================
DBA_bool DBA_database::Connect()
{
  gError.Clear(); 

  if (SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_pHandles->hEnv) != SQL_SUCCESS) {
    return false;
  }
  
  if (SQLSetEnvAttr(m_pHandles->hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER) SQL_OV_ODBC3, 0) != SQL_SUCCESS) {
    SQLFreeHandle(SQL_HANDLE_ENV, m_pHandles->hEnv);
    m_pHandles->hEnv = NULL;
    return false;
  }

  if (SQLAllocHandle(SQL_HANDLE_DBC, m_pHandles->hEnv, &m_pHandles->hDbc) != SQL_SUCCESS) {
    SQLFreeHandle(SQL_HANDLE_ENV, m_pHandles->hEnv);
    m_pHandles->hEnv = NULL;
    return false;
  }

  if (SQLSetConnectAttr( m_pHandles->hDbc, SQL_ATTR_SESSION_TIMEOUT, (SQLPOINTER) 0, 0 ) != SQL_SUCCESS) {
    SQLFreeHandle(SQL_HANDLE_DBC, m_pHandles->hDbc);
    SQLFreeHandle(SQL_HANDLE_ENV, m_pHandles->hEnv);
    m_pHandles->hEnv = NULL;
    m_pHandles->hDbc = NULL;
    return false;
  } // end if

  if (SQLSetConnectAttr( m_pHandles->hDbc, SQL_TXN_ISOLATION, (SQLPOINTER) SQL_TXN_READ_UNCOMMITTED, 0 ) != SQL_SUCCESS) {
    SQLFreeHandle(SQL_HANDLE_DBC, m_pHandles->hDbc);
    SQLFreeHandle(SQL_HANDLE_ENV, m_pHandles->hEnv);
    m_pHandles->hEnv = NULL;
    m_pHandles->hDbc = NULL;
    return false;
  } // end if
  
  if (SQLSetConnectAttr( m_pHandles->hDbc, SQL_PRODUCER, (SQLPOINTER) sp1pr_internal_cmd, 0 ) != SQL_SUCCESS) {
    SQLFreeHandle(SQL_HANDLE_DBC, m_pHandles->hDbc);
    SQLFreeHandle(SQL_HANDLE_ENV, m_pHandles->hEnv);
    m_pHandles->hEnv = NULL;
    m_pHandles->hDbc = NULL;
    return false;
  } // end if

  // create the error object for odbc
  m_pError = new StudioOAL_WError(m_pHandles->hDbc, m_pHandles->hEnv);
  if (!m_pError) {
    SQLFreeHandle(SQL_HANDLE_DBC, m_pHandles->hDbc);
    SQLFreeHandle(SQL_HANDLE_ENV, m_pHandles->hEnv);
    m_pHandles->hEnv = NULL;
    m_pHandles->hDbc = NULL;
    gError.SetError(DBA_ERR_NOMEM);
    return false;
  }

  SQLCHAR   sConnect[1024];
  SQLCHAR   sOut[1024];
  DBA_short nOut;
  RETCODE   nRC;

  if (m_sUser[0] != '"') {
    m_sUser.ToUpper();
  } // end if
  if (m_sPassword[0] != '"') {
    m_sPassword.ToUpper();
  } // end if

  m_sDriver = "MaxDB <MAJOR_VERSION>.<MINOR_VERSION>";
  sprintf((char *)sConnect, "DRIVER=%s;SERVERDB=%s;SERVERNODE=%s;UID=%s;PWD=%s",
          m_sDriver.CharPtr(),
          m_sDatabase.CharPtr(),
          m_sServer.CharPtr(),
          m_sUser.CharPtr(),
          m_sPassword.CharPtr());
  

  nRC = SQLDriverConnect(m_pHandles->hDbc, (SQLHWND)NULL, sConnect, SQL_NTS, sOut, 
                         sizeof(sOut), &nOut, SQL_DRIVER_NOPROMPT);

  bool bError =  (!m_pError->checkSQLReturnCode(nRC, NULL));

  if (bError && (m_pError->getSQLStateErrorNumber() == "IM002")) {
    m_sDriver = "MaxDB";
    sprintf((char *)sConnect, "DRIVER=%s;SERVERDB=%s;SERVERNODE=%s;UID=%s;PWD=%s",
            m_sDriver.CharPtr(),
            m_sDatabase.CharPtr(),
            m_sServer.CharPtr(),
            m_sUser.CharPtr(),
            m_sPassword.CharPtr());
    nRC = SQLDriverConnect(m_pHandles->hDbc, (SQLHWND)NULL, sConnect, SQL_NTS, sOut, 
                          sizeof(sOut), &nOut, SQL_DRIVER_NOPROMPT);
    bError = (!m_pError->checkSQLReturnCode(nRC, NULL));
  } // end if

  if (bError) {
    // create error object
    gError.SetReconnect(CheckReconnectError(m_pError->getNativeError()));
    gError.SetError(DBA_ERR_CONNECT);
    gError.SetExtText(GetErrorMsg());

    // free resources
    SQLFreeHandle(SQL_HANDLE_DBC, m_pHandles->hDbc);
    SQLFreeHandle(SQL_HANDLE_ENV, m_pHandles->hEnv);
    m_pHandles->hEnv = NULL;
    m_pHandles->hDbc = NULL;

    return false;
  } // end if

  if (SQLSetConnectAttr( m_pHandles->hDbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER) SQL_AUTOCOMMIT_OFF, SQL_IS_UINTEGER ) != SQL_SUCCESS) {
    SQLFreeHandle(SQL_HANDLE_DBC, m_pHandles->hDbc);
    SQLFreeHandle(SQL_HANDLE_ENV, m_pHandles->hEnv);
    m_pHandles->hEnv = NULL;
    m_pHandles->hDbc = NULL;
    return false;
  } // end if

  return true;
} // DBA_database::Connect

// ========================================================
// DBA_database::Execute
// ========================================================
DBA_bool DBA_database::Execute(const DBA_string & sSQL, DBA_snapshot * pSnapshot) 
{
  StudioOAL_WResult   oResult(m_pHandles->hDbc, m_pHandles->hEnv);
  StudioOAL_WResult * pResult = &oResult;
  if (!PrepareQuery(sSQL, pResult)) {
    return false;
  } // end if
  DBA_bool bReturn = Execute(sSQL, pSnapshot, pResult);
  return bReturn;
} // end DBA_database::Execute

// ========================================================
// DBA_database::PrepareQuery
// ========================================================
DBA_bool DBA_database::PrepareQuery (const DBA_string & sSQL, StudioOAL_WResult * & pPreparedQuery)
{
  gError.Clear(); 
  bool bNew = false;

  if (pPreparedQuery == NULL) {
    pPreparedQuery = new StudioOAL_WResult(m_pHandles->hDbc, m_pHandles->hEnv);
    if (pPreparedQuery == NULL) {
      gError.SetError(DBA_ERR_NOMEM);
      return false;
    } // end if
    bNew = true;
  } // end if

  if (!pPreparedQuery->openResultPrepare (sSQL)) {
    gError.SetReconnect(CheckReconnectError(pPreparedQuery->getNativeError()));
    gError.SetIgnore(CheckIgnoreError(pPreparedQuery->getNativeError()));
    gError.SetError(DBA_ERR_PREPARE, pPreparedQuery->getNativeError());
    DBA_string sError = pPreparedQuery->getErrorText();
    gError.SetExtText(sError.Append("\n").Append(sSQL));

    if (bNew) {
      delete pPreparedQuery;
      pPreparedQuery = NULL;
    } // end if

    return false;
  } // end if

  return true;
} // end DBA_database::PrepareQuery

// ========================================================
// DBA_database::Execute
// ========================================================
DBA_bool DBA_database::Execute (const DBA_string & sSQL, DBA_snapshot * pSnapshot, StudioOAL_WResult * & pPreparedQuery)
{
  gError.Clear(); 

  if (pPreparedQuery == NULL) {
    if (!PrepareQuery(sSQL, pPreparedQuery)) {
      return false;
    } // end if
  } // end if

  if (!pPreparedQuery->executePreparedStmt()) {
    // prepare again
    if (!PrepareQuery(sSQL, pPreparedQuery)) {
      return false;
    } // end if

    // execute again
    gError.Clear(); 
    if (!pPreparedQuery->executePreparedStmt()) {
      gError.SetReconnect(CheckReconnectError(pPreparedQuery->getNativeError()));
      gError.SetIgnore(CheckIgnoreError(pPreparedQuery->getNativeError()));
      gError.SetError(DBA_ERR_EXECUTE, pPreparedQuery->getNativeError());
      DBA_string sError = pPreparedQuery->getErrorText();
      gError.SetExtText(sError.Append("\n").Append(sSQL));
      return false;
    } // end if
  } // end if

  if (pSnapshot) {
    pSnapshot->Clear();  

    DBA_int nCols = pPreparedQuery->getColCount();
    DBA_int nCol;
    
    for (nCol = 0; nCol < nCols; nCol++) {
      DBA_string sName;
      pPreparedQuery->getColumnName(nCol+1, sName);
      if (!pSnapshot->AddColumn(sName)) {
        return false;
      }
    }

    DBA_bool bFetch = true;
  
    while (bFetch) {
      bFetch = pPreparedQuery->fetch();
      if(!bFetch) {
        gError.SetReconnect(CheckReconnectError(pPreparedQuery->getNativeError()));
        gError.SetIgnore(CheckIgnoreError(pPreparedQuery->getNativeError()));

        gError.SetError(DBA_ERR_EXECUTE, pPreparedQuery->getNativeError());
        DBA_string sError = pPreparedQuery->getErrorText();
        gError.SetExtText(sError.Append("\n").Append(sSQL));

        pPreparedQuery->closeCursor ();
        return false;
      } else {
        if (pPreparedQuery->noRows()) {
          pPreparedQuery->closeCursor ();
          return true;
        } else {
          if (pSnapshot->AddRow()) {
            for (nCol = 0; nCol < nCols; nCol++) {
              DBA_string sValue;
              pPreparedQuery->getColumnDataAsString(nCol+1, sValue);
              if (!pSnapshot->AddField(sValue)) {
                return false;
              } // end if
            } // end for
          } // end if
        }  // end if
      } // end if
    } // end while
  } // end if

  pPreparedQuery->closeCursor ();
  return true;
} // end DBA_database::Execute

// ========================================================
// DBA_database::Disconnect
// ========================================================
DBA_bool DBA_database::Disconnect ()
{
  SQLDisconnect(m_pHandles->hDbc);
  SQLFreeHandle(SQL_HANDLE_DBC, m_pHandles->hDbc);
  SQLFreeHandle(SQL_HANDLE_ENV, m_pHandles->hEnv);

  m_pHandles->hEnv = NULL;
  m_pHandles->hDbc = NULL;

  if(m_pError) {
    delete m_pError;
    m_pError = NULL;
  }

  return true;
} // end DBA_database::Disconnect

// ========================================================
// DBA_database::GetTimestamp
// ========================================================
DBA_bool DBA_database::GetTimestamp(time_t & tTime)
{
  gError.Clear(); 

  StudioOAL_WResult oResult(m_pHandles->hDbc, m_pHandles->hEnv);

  // select date and time
  if (!oResult.openResult(DBA_SQL_INFO_DATETIME)) {
    gError.SetReconnect(CheckReconnectError(oResult.getNativeError()));
    gError.SetError(DBA_ERR_EXECUTE);
    DBA_string sError = oResult.getErrorText();
    gError.SetExtText(sError.Append("\n").Append(DBA_SQL_INFO_DATETIME));
    return false;
  } // end if

  if (!oResult.fetch()) {
    gError.SetReconnect(CheckReconnectError(oResult.getNativeError()));
    gError.SetError(DBA_ERR_EXECUTE);
    DBA_string sError = oResult.getErrorText();
    gError.SetExtText(sError.Append("\n").Append(DBA_SQL_INFO_DATETIME));
    return false;
  } // end if
  
  if (!oResult.noRows()) {
    struct tm oTime;
    memset(&oTime, 0, sizeof(oTime));
    DBA_string sColumn = "";
    oResult.getColumnDataAsString(1, sColumn);
    oTime.tm_year = atoi(sColumn.CharPtr()) - 1900;
    oResult.getColumnDataAsString(2, sColumn);
    oTime.tm_mon  = atoi(sColumn.CharPtr()) - 1;
    oResult.getColumnDataAsString(3, sColumn);
    oTime.tm_mday = atoi(sColumn.CharPtr());
    oResult.getColumnDataAsString(4, sColumn);
    oTime.tm_hour = atoi(sColumn.CharPtr());
    oResult.getColumnDataAsString(5, sColumn);
    oTime.tm_min  = atoi(sColumn.CharPtr());
    oResult.getColumnDataAsString(6, sColumn);
    oTime.tm_sec  = atoi(sColumn.CharPtr());
    oTime.tm_isdst = -1;
    tTime = mktime(&oTime);
  } else {
    gError.SetReconnect(CheckReconnectError(oResult.getNativeError()));
    gError.SetError(DBA_ERR_EXECUTE);
    DBA_string sError = oResult.getErrorText();
    gError.SetExtText(sError.Append("\n").Append(DBA_SQL_INFO_DATETIME));
    tTime = 0;
    return false;
  } // end if

  return true;
} // end DBA_database::GetTimestamp

// ========================================================
// DBA_database::GetInfos
// ========================================================
// get some infos from the database
//
// INSTANCE_TYPE
// KERNELVERSION
// RUNDIRECTORY
// SESSION
// APPLPROCESS
//
DBA_bool DBA_database::GetInfos()
{
  StudioOAL_WResult oResult(m_pHandles->hDbc, m_pHandles->hEnv);

  gError.Clear(); 

  // check for new systemtables
  if (!oResult.openResult(DBA_NEW_SQL_PARAM_INSTANCETYPE)) {
        if (oResult.getNativeError() == -4004) {
            m_bUseOldSysTables = true;
        } else {
            gError.SetError(DBA_ERR_EXECUTE);
            DBA_string sError = oResult.getErrorText();
            gError.SetExtText(sError.Append("\n").Append(DBA_SQL_PARAM_INSTANCETYPE));
            return false;
        } 
  }

  // get INSTANCE_TYPE from table DBPARAMETERS
  if (!oResult.openResult(DBA_SQL_PARAM_INSTANCETYPE)) {
    gError.SetError(DBA_ERR_EXECUTE);
    DBA_string sError = oResult.getErrorText();
    gError.SetExtText(sError.Append("\n").Append(DBA_SQL_PARAM_INSTANCETYPE));
    return false;
  }

  if (!oResult.fetch()) {
    gError.SetError(DBA_ERR_EXECUTE);
    gError.SetExtText(oResult.getErrorText());
    return false;
  }
  
  if (!oResult.noRows()) {
    DBA_string sInstanceType;
    oResult.getColumnDataAsString(1, sInstanceType);
    if (sInstanceType.Find(DBA_INSTANCETYPE_TXT_OLTP) != DBA_string::NPos) {
      m_nInstanceType = DBA_INSTANCETYPE_OLTP;
    } else if (sInstanceType.Find(DBA_INSTANCETYPE_TXT_LVC) != DBA_string::NPos) {
      m_nInstanceType = DBA_INSTANCETYPE_LVC;
    }
  }

  oResult.closeResult();

  // get KERNEL from table VERSIONS
  if (!oResult.openResult(DBA_SQL_INFO_KERNELVERSION)) {
    gError.SetError(DBA_ERR_EXECUTE);
    DBA_string sError = oResult.getErrorText();
    gError.SetExtText(sError.Append("\n").Append(DBA_SQL_INFO_KERNELVERSION));
    return false;
  }

  if (!oResult.fetch()) {
    gError.SetError(DBA_ERR_EXECUTE);
    gError.SetExtText(oResult.getErrorText());
    return false;
  }
  
  if (!oResult.noRows()) {
    oResult.getColumnDataAsString(1, m_sKernelVersion);
//    sscanf(m_sKernelVersion.CharPtr(), "Kernel    %d.%d.%d    Build %d-000-000-000", &m_nMajor, &m_nMinor, &m_nLevel, &m_nBuild);
    sscanf(m_sKernelVersion.CharPtr(), "%*s %d.%d.%d Build %d-000-000-000", &m_nMajor, &m_nMinor, &m_nLevel, &m_nBuild);
  }

  oResult.closeResult();

  // get RUNDIRECTORY from table DBPARAMETERS
  if (!oResult.openResult(DBA_SQL_PARAM_RUNDIRECTORY)) {
    gError.SetError(DBA_ERR_EXECUTE);
    DBA_string sError = oResult.getErrorText();
    gError.SetExtText(sError.Append("\n").Append(DBA_SQL_PARAM_RUNDIRECTORY));
    return false;
  }

  if (!oResult.fetch()) {
    gError.SetError(DBA_ERR_EXECUTE);
    gError.SetExtText(oResult.getErrorText());
    return false;
  }
  
  if (!oResult.noRows()) {
    oResult.getColumnDataAsString(1, m_sRundirectory);
  }

  oResult.closeResult();

  // get SESSION from table CONNECTPARAMETERS
  if (!oResult.openResult(DBA_SQL_INFO_SESSION)) {
    gError.SetError(DBA_ERR_EXECUTE);
    DBA_string sError = oResult.getErrorText();
    gError.SetExtText(sError.Append("\n").Append(DBA_SQL_INFO_SESSION));
    return false;
  }

  if (!oResult.fetch()) {
    gError.SetError(DBA_ERR_EXECUTE);
    gError.SetExtText(oResult.getErrorText());
    return false;
  }
  
  if (!oResult.noRows()) {
    DBA_string sSession;
    oResult.getColumnDataAsString(1, sSession);
    sscanf(sSession.CharPtr(), "%u", &m_nSid);
  }

  oResult.closeResult();

  // get APPLPROCESS  from table TRANSACTIONS
  if (!oResult.openResult(DBA_SQL_INFO_PID)) {
    gError.SetError(DBA_ERR_EXECUTE);
    DBA_string sError = oResult.getErrorText();
    gError.SetExtText(sError.Append("\n").Append(DBA_SQL_INFO_PID));
    return false;
  }

  if (!oResult.fetch()) {
    gError.SetError(DBA_ERR_EXECUTE);
    gError.SetExtText(oResult.getErrorText());
    return false;
  }
  
  if (!oResult.noRows()) {
    DBA_string sSession;
    oResult.getColumnDataAsString(1, sSession);
    sscanf(sSession.CharPtr(), "%u", &m_nPid);
  }

  oResult.closeResult();

  return true;
} // DBA_database::GetInfos

//
// CheckReconnectError
//
DBA_bool DBA_database::CheckReconnectError
  ( DBA_long nError ) const
{
  switch (nError) {
    case    -708:
    case    -709:
    case    -807:
    case    -906:
    case    -907:
    case   -9807:
    case     710:
    case  -10708:
    case  -10709:
    case  -10807:
    case  -10906:
    case  -10907:
      return true;
  } // end switch

  return false;
} // end DBA_database::CheckReconnectError

//
// DBA_database::SetIgnoreErrors
//
void DBA_database::SetIgnoreErrors(DBA_string sIgnoreErrors)
{
  DBA_string::BasisElementIndex nKomma;
  DBA_string::BasisElementIndex nCurrent = 0;
  DBA_long                      nError;

  nKomma = sIgnoreErrors.Find(',', nCurrent);
  while (nKomma != DBA_string::NPos) {
    nError = atol(sIgnoreErrors.SubStr(nCurrent, nKomma - nCurrent).CharPtr());
    m_oIgnoreErrors.push_back(nError);
    nCurrent = nKomma + 1;
    nKomma = sIgnoreErrors.Find(',', nCurrent);
  } // end while
  // last Element
  if (sIgnoreErrors.SubStr(nCurrent).Length() > 0) {
    nError = atol(sIgnoreErrors.SubStr(nCurrent).CharPtr());
    m_oIgnoreErrors.push_back(nError);
  } // end if

} // end DBA_database::SetIgnoreErrors

//
// DBA_database::CheckIgnoreError
//
DBA_bool DBA_database::CheckIgnoreError(DBA_long nError)
{
  ErrorList::iterator iter = m_oIgnoreErrors.begin();
  while (iter != m_oIgnoreErrors.end()) {
    if ((*iter) == nError) {
      return true;;
    } // end if
    ++iter;
  } // end while

  return false;
} // end DBA_database::CheckIgnoreError

//
// GetErrorMsg
//
DBA_string DBA_database::GetErrorMsg()
{
  DBA_string sMsg = "";

  if (m_pError->getNativeError() != 0) {
    sMsg.Append(m_pError->getNativeErrorAsString());
    sMsg.Append(" ");
    sMsg.Append(m_pError->getErrorText());
  } else {
    sMsg.Append(m_pError->getErrorText());
  }
  return sMsg;
}

//
// CheckAnalyzer
// 
DBA_bool DBA_database::CheckAnalyzer(DBA_uint nSid, DBA_uint nPid, const DBA_string & sDir)
{
  char              szStatement[200];
  StudioOAL_WResult oResult(m_pHandles->hDbc, m_pHandles->hEnv);
  
  gError.Clear(); 

  sprintf(szStatement, DBA_SQL_CHECK_ANALYZER, nSid, nPid);

  // select a record
  if (!oResult.openResult(szStatement)) {
    gError.SetError(DBA_ERR_EXECUTE);
    DBA_string sError = oResult.getErrorText();
    gError.SetExtText(sError.Append("\n").Append(szStatement));
    return false;
  } // end if

  if (!oResult.fetch()) {
    gError.SetError(DBA_ERR_EXECUTE);
    gError.SetExtText(oResult.getErrorText());
    return false;
  } // end if
  
  if (!oResult.noRows()) {
    // active session found
    gError.SetError(DBA_ERR_ACTIVE, sDir);
    return false;
  } else {
    gError.SetError(DBA_ERR_NOTACTIVE, sDir);
  } // end if

  oResult.closeResult();

  return true;
} // end DBA_database::CheckAnalyzer

//
// CheckSession
// 
DBA_bool DBA_database::CheckSession( )
{
    if (!LockSystemTables()) {
        return false;
    } // end if

    char              szStatement[200];
    StudioOAL_WResult oResult(m_pHandles->hDbc, m_pHandles->hEnv);
  
    gError.Clear(); 

    sprintf(szStatement, DBA_SQL_CHECK_SESSION, GetSid());

    // select a record
    if (!oResult.openResult(szStatement)) {
        gError.SetError(DBA_ERR_EXECUTE);
        DBA_string sError = oResult.getErrorText();
        gError.SetExtText(sError.Append("\n").Append(szStatement));
        return false;
    } // end if

    if (!oResult.fetch()) {
        gError.SetError(DBA_ERR_EXECUTE);
        gError.SetExtText(oResult.getErrorText());
        return false;
    } // end if
  
    if (oResult.noRows()) {
        // active session found
        gError.SetError(DBA_ERR_DISCONNECT);
        return false;
    } // end if

    oResult.closeResult();

    if (!Commit()) {
        return false;
    } // end if

    return true;
} // end DBA_database::CheckSession

//
// LockSystemTables
// 
DBA_bool DBA_database::LockSystemTables( )
{
    // check for locktable
    if (!Execute(DBA_SQL_LOCK_CHECK, NULL)) {
        if (gError.GetNativeError() == -4004) {
            gError.Clear(); 
            return true;
        } else {
            return false;
        } // end if
    } // end if
 
    // use the lock table
    if (!Execute(DBA_SQL_LOCK_LOCK, NULL)) {
        return false;
    } // end if

    return true;
} // end DBA_database::LockSystemTables

//
// Commit
// 
DBA_bool DBA_database::Commit( )
{
    // send COMMIT statement
    if (!Execute(DBA_SQL_COMMIT, NULL)) {
        return false;
    } // end if

    return true;
} // end DBA_database::Commit
