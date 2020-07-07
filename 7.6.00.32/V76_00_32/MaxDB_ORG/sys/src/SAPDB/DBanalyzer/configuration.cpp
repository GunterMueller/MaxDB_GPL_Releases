/*!
  -----------------------------------------------------------------------------
  module: configuration.cpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: DBanalyzer - The performance analyse tool for SAPDB
  description:  the configuration for the dbanalyzer
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

#include <stdlib.h>

#include "heo01.h"

#include "SAPDB/ToolsCommon/Tools_System.hpp"
#include "SAPDB/DBanalyzer/dbanalyzer.hpp"
#include "SAPDB/DBanalyzer/dbanalyzer.hpp"
#include "SAPDB/DBanalyzer/configuration.hpp"
#include "SAPDB/DBanalyzer/error.hpp"

extern DBA_error gError;

#define DBA_CONFIG_FILE_NAME "dbanalyzer"
#define DBA_CONFIG_FILE_EXT  ".cfg"

const char * const DBA_config_Version::MaxVersion = "999.9.9.9";
const char * const DBA_config_Version::MinVersion = "0.0.0.0";

DBA_configuration::DBA_configuration () 
                  : m_oValueProvider(m_lParameters, m_lArrays),
                    m_oDBVersion(DBA_config_Version::MinVersion),
                    m_nInstance(DBA_INSTANCETYPE_OLTP)
{
  m_sFilename = "";
  m_hFile = NULL;
  m_nLine = 0;
  m_sDate = "";
}

DBA_configuration::~DBA_configuration () {
}

//
// GetEnvPath
//
// get the path to the environment directory
//
DBA_string DBA_configuration::GetEnvPath()
{
  tsp00_Pathc     szEnvPath;
  tsp01_RteError  aRteError;

  sqlGetIndependentEnvPath(szEnvPath, TERM_WITH_DELIMITER_EO01, &aRteError);

  return DBA_string(szEnvPath);
} // end DBA_configuration::GetEnvPath

//
// Open
//
// open and read the configuration file
//
DBA_bool DBA_configuration::Open
  ( const DBA_string & sFilename,
    DBA_int            nMajor,
    DBA_int            nMinor,
    DBA_int            nLevel,
    DBA_int            nBuild,
    DBA_int            nInstance )
{
  DBA_bool   bRC = false;

  m_nLine     = 0;
  m_sFilename = sFilename;

  m_oDBVersion.setVersion(nMajor, nMinor, nLevel, nBuild);
  m_nInstance = nInstance;

  if (m_sFilename.Empty()) {
    DBA_string sDir = ToolsSys_getcwd(true);
    DBA_string sMajor;
    DBA_string sMinor;
    DBA_string sLevel;
    DBA_string sBuild;

    sMajor.ConvertFromInt(nMajor);
    sMinor.ConvertFromInt(nMinor);
    sLevel.ConvertFromInt(nLevel);
    sBuild.ConvertFromInt(nBuild);

    // 1. <local>dbanalyzer<major><minor><level><build>.cfg
    m_sFilename.Assign(sDir)
               .Append(DBA_CONFIG_FILE_NAME)
               .Append(sMajor)
               .Append(sMinor)
               .Append(sLevel)
               .Append(sBuild)
               .Append(DBA_CONFIG_FILE_EXT);
    m_hFile = fopen(m_sFilename.CharPtr(), "r");
    if (m_hFile == NULL) {
      // 2. <local>dbanalyzer<major><minor><level>.cfg
      m_sFilename.Assign(sDir)
                 .Append(DBA_CONFIG_FILE_NAME)
                 .Append(sMajor)
                 .Append(sMinor)
                 .Append(sLevel)
                 .Append(DBA_CONFIG_FILE_EXT);
      m_hFile = fopen(m_sFilename.CharPtr(), "r");
      if (m_hFile == NULL) {
        // 3. <local>dbanalyzer<major><minor>.cfg
        m_sFilename.Assign(sDir)
                   .Append(DBA_CONFIG_FILE_NAME)
                   .Append(sMajor)
                   .Append(sMinor)
                   .Append(DBA_CONFIG_FILE_EXT);
        m_hFile = fopen(m_sFilename.CharPtr(), "r");
        if (m_hFile == NULL) {
          // 4. <local>dbanalyzer.cfg
          m_sFilename.Assign(sDir)
                     .Append(DBA_CONFIG_FILE_NAME)
                     .Append(DBA_CONFIG_FILE_EXT);
          m_hFile = fopen(m_sFilename.CharPtr(), "r");
          if (m_hFile == NULL) {
            sDir = GetEnvPath();

            // 5. <env>dbanalyzer<major><minor><level><build>.cfg
            m_sFilename.Assign(sDir)
                       .Append(DBA_CONFIG_FILE_NAME)
                       .Append(sMajor)
                       .Append(sMinor)
                       .Append(sLevel)
                       .Append(sBuild)
                       .Append(DBA_CONFIG_FILE_EXT);
            m_hFile = fopen(m_sFilename.CharPtr(), "r");
            if (m_hFile == NULL) {
              // 6. <env>dbanalyzer<major><minor><level>.cfg
              m_sFilename.Assign(sDir)
                         .Append(DBA_CONFIG_FILE_NAME)
                         .Append(sMajor)
                         .Append(sMinor)
                         .Append(sLevel)
                         .Append(DBA_CONFIG_FILE_EXT);
              m_hFile = fopen(m_sFilename.CharPtr(), "r");
              if (m_hFile == NULL) {
                // 7. <env>dbanalyzer<major><minor>.cfg
                m_sFilename.Assign(sDir)
                           .Append(DBA_CONFIG_FILE_NAME)
                           .Append(sMajor)
                           .Append(sMinor)
                           .Append(DBA_CONFIG_FILE_EXT);
                m_hFile = fopen(m_sFilename.CharPtr(), "r");
                if (m_hFile == NULL) {
                  // 8. <env>dbanalyzer.cfg
                  m_sFilename.Assign(sDir)
                             .Append(DBA_CONFIG_FILE_NAME)
                             .Append(DBA_CONFIG_FILE_EXT);
                  m_hFile = fopen(m_sFilename.CharPtr(), "r");
                } // end if
              } // end if
            } // end if
          } // end if
        } // end if
      } // end if
    } // end if
 
  } else {
    m_sFilename = ToolsSys_fullpath(m_sFilename.CharPtr());
    m_hFile = fopen(m_sFilename.CharPtr(), "r");
  } // end if

  if (m_hFile) {
    if (Read()) {
      bRC = true;
    }
    fclose(m_hFile);
    m_hFile = NULL;
  } else {
    gError.SetError(DBA_ERR_OPENFILE, m_sFilename); 
  }

  return bRC;
}

//
// SetParameterValue
//
DBA_bool DBA_configuration::SetParameterValue(const DBA_string & sName, 
                                              const DBA_string & sValue)
{
  Tools_ExpressionValue * pParameter = m_oValueProvider.AskForValue(sName);
  
  if (pParameter != NULL) {
    pParameter->SetNewValue(sValue);
    return true;
  } // end if

  gError.SetError(DBA_ERR_UNKNOWNOBJ, sName);
  return false;
} // DBA_configuration::SetParameterValue

//
// GetParameterValue
//
DBA_string DBA_configuration::GetParameterValue(const DBA_string & sName)
{
  Tools_ExpressionValue * pParameter = m_oValueProvider.AskForValue(sName);

  if (pParameter != NULL) {
    return pParameter->GetStringValue();
  } // end if

  return "";
} // end DBA_configuration::GetParameterValue

//
// GetParameter
//
DBA_config_parameter * DBA_configuration::GetParameter(const DBA_string & sName)
{
  DBA_config_parameter_list::iterator iter = m_lParameters.begin();
  while (iter != m_lParameters.end()) {
    if ((*iter)->sID == sName) {
      return (*iter);
    }
    ++iter;
  }

  return NULL;
} // end DBA_configuration::GetParameter

//
// GetMonitor
//
DBA_config_monitor * DBA_configuration::GetMonitor(const DBA_string & sName)
{
  DBA_config_monitor_list::iterator iter = m_lMonitors.begin();
  while (iter != m_lMonitors.end()) {
    if ((*iter)->sID == sName) {
      return (*iter);
    }
    ++iter;
  }

  return NULL;
} // end DBA_configuration::GetMonitor

//
// GetSetting
//
DBA_string DBA_configuration::GetSetting(const DBA_string & sName)
{
  DBA_string sKey = sName;
  sKey.ToUpper();

  DBA_config_setting_list::iterator iter = m_lSettings.begin();
  while (iter != m_lSettings.end()) {
    if ((*iter)->sName == sKey) {
      return (*iter)->sValue;
    }
    ++iter;
  }

  return "";
}

//
// InitViews
//
// create the necessary views for the dbanalyzer
//
DBA_bool DBA_configuration::InitViews(DBA_database * pDatabase)
{
  DBA_config_view_list::iterator iter = m_lViews.begin();
  while (iter != m_lViews.end()) {
    // drop view
    DBA_string sStmt = "DROP VIEW ";
    sStmt.Append((*iter)->sName);
    pDatabase->Execute(sStmt, NULL);

    // build the statement
    sStmt = "CREATE OR REPLACE VIEW ";
    sStmt.Append((*iter)->sName);
    sStmt.Append(" AS ");
    sStmt.Append((*iter)->sQuery);
    sStmt.Append(" INTERNAL ");

    // create view
    if (!pDatabase->Execute(sStmt, NULL)) {
      return false;
    }
    ++iter;
  }
  
    // commit changes
    if (!pDatabase->Commit()) {
        return false;
    } // end if

  return true;
}

//
// InitEnvironment
//
// select values for the environment parameters
// from the database or colculate the expression
// for an environment parameter
//
DBA_bool DBA_configuration::InitEnvironment(DBA_database * pDatabase)
{
  DBA_config_parameter_list::iterator iter = m_lParameters.begin();
  while (iter != m_lParameters.end()) {
    if ((*iter)->nType == DBA_CNFPTYPE_ENVIRONMENT) {
      if ((*iter)->sFormula.Empty()) {
        DBA_string sStmt = "SELECT ";
        sStmt.Append((*iter)->sColumn);
        sStmt.Append(" FROM ");
        sStmt.Append((*iter)->sTable);
        if (!(*iter)->sConditionColumn.Empty()) {
          sStmt.Append(" WHERE ")
               .Append((*iter)->sConditionColumn)
               .Append(" = ")
               .Append((*iter)->sConditionValue);
        }

        DBA_snapshot oSnapshot;
        if (pDatabase->Execute(sStmt, &oSnapshot)) {
          if (!oSnapshot.Empty()) {
            (*iter)->SetNewValue(oSnapshot.GetField(0,0));
          }
        } else {
          return false;
        }
      }
    }
    ++iter;
  }
  
  return true;
}

//
// InitProtocolFiles
//
DBA_bool DBA_configuration::InitProtocolFiles(DBA_protocol * pProtocol)
{
  DBA_config_class_list::iterator iterClass = m_lClasses.begin();
  while (iterClass != m_lClasses.end()) {

    if (((*iterClass)->nInstanceType & m_nInstance) == m_nInstance) {
      if (!pProtocol->AddParameterClass((*iterClass)->sName, (*iterClass)->nFileIndex)) {
        return false;
      }

      DBA_config_parameter_list::iterator iterParameter = (*iterClass)->lParameters.begin();
      DBA_string                          sType;
      while (iterParameter != (*iterClass)->lParameters.end()) {
        // add parameter name to header
        if ((*iterParameter)->bProtocol) {
          switch ((*iterParameter)->GetType()) {
          case Tools_ExpressionValue::ExTypeNumeric:
            sType = DBA_PRT_TYPENUMRIC;
            break;
          case Tools_ExpressionValue::ExTypeString:
            sType = DBA_PRT_TYPESTRING;
            break;
          default:
            sType = DBA_PRT_TYPENUMRIC;
          } // end switch
          if ((*iterParameter)->bImportant) {
            sType.Append(DBA_PRT_TYPEIMPORTANT);
          } else if ((*iterParameter)->sProtocolInfo.Length() > 0) {
            sType.Append(" ");
          } // end if
          sType.Append((*iterParameter)->sProtocolInfo);
          pProtocol->AddParameterHeader((*iterClass)->nFileIndex, (*iterParameter)->sID, sType, (*iterParameter)->sLabel);
        }
        ++iterParameter;
      }
    } // end if

    ++iterClass;
  }
  return true;
}

//
// GetSnapshot
//
DBA_bool DBA_configuration::GetSnapshot
  (DBA_database * pDatabase,
   time_t       & tSnapshot,
   DBA_bool     & bStartup)
{
  DBA_string     sStmt;
  DBA_string     sValue;
  DBA_snapshot * pSnapshot = NULL;
  
  if (!pDatabase->GetTimestamp(tSnapshot)) {
    return false;
  } else {
    if (gError.GetReconnect() || gError.GetIgnore() ) {
        return true;
    } // end if
    char szDate[11];
    strftime(szDate, 10, "%Y%m%d", localtime(&tSnapshot));
    bStartup = (m_sDate.Compare(szDate) != 0);
    m_sDate = szDate;
  } // end if

  // select the snapshots from the database
  DBA_config_table_list::iterator iterTable = m_lTables.begin();
  while (iterTable != m_lTables.end()) {

    if (((((*iterTable)->nInstanceType & m_nInstance)) ==  m_nInstance    ) &&
         (((*iterTable)->bStartup && bStartup) || !(*iterTable)->bStartup )    ) {
      sStmt = "SELECT * FROM ";
      sStmt.Append((*iterTable)->sName);

      if (!(*iterTable)->pSnapshot) {
        (*iterTable)->pSnapshot = new DBA_snapshot;
        if (!(*iterTable)->pSnapshot) {
          gError.SetError(DBA_ERR_NOMEM);
          return false;
        }
      }

      if (pDatabase->Execute(sStmt, (*iterTable)->pSnapshot, (*iterTable)->pPreparedQuery)) {
        if (gError.GetReconnect() || gError.GetIgnore() ) {
          return true;
        }
      } else {
        return false;
      }
    } // end if

    ++iterTable;
  } // end while

  // assign values from snapshot to parameters
  DBA_config_parameter_list::iterator iterParameter = m_lParameters.begin();
  while (iterParameter != m_lParameters.end()) {
    if (((*iterParameter)->bStartup && bStartup) || !(*iterParameter)->bStartup) {

      if ((*iterParameter)->pTable) {
        if ((*iterParameter)->pTable->pSnapshot) {
          pSnapshot = (*iterParameter)->pTable->pSnapshot;

          sValue = pSnapshot->GetFieldByCondition((*iterParameter)->sConditionColumn,
                                                  (*iterParameter)->sConditionValue,
                                                  (*iterParameter)->sColumn);
          if (sValue == DBA_SNAPSHOT_NULLVAL) {
            if (!(*iterParameter)->sNullRep.Empty()) {
              (*iterParameter)->SetNewValue((*iterParameter)->sNullRep);
            } else {
              (*iterParameter)->SetNewValue(sValue);
            }
          } else {
            (*iterParameter)->SetNewValue(sValue);
          }
        }
      }

    } // end if
    ++iterParameter;
  } // end while
  
  // assign values from snapshot to arrays
  DBA_config_array_list::iterator        iterArray = m_lArrays.begin();
  Tools_ExpressionValueArray::size_type  nIndex    = 0;
  DBA_int                                nRow      = 0;
  DBA_int                                nCol      = 0;
  DBA_string                             sKey;
  
  while (iterArray != m_lArrays.end()) {
    if (((*iterArray)->bStartup && bStartup) || !(*iterArray)->bStartup) {

      if ((*iterArray)->pTable) {
        if ((*iterArray)->pTable->pSnapshot) {
          pSnapshot = (*iterArray)->pTable->pSnapshot;

          // evaluate snapshot
          for (nRow = 0; nRow < pSnapshot->GetRowCount(); ++nRow) {
            nCol = pSnapshot->GetColumnByName((*iterArray)->sColumn);
            if (nCol == -1) {
              gError.SetError(DBA_ERR_UNKNOWNVAL, (*iterArray)->sColumn, (*iterArray)->sID);
              return false;
            } // end if
            sValue = pSnapshot->GetField(nCol, nRow);
            nCol = pSnapshot->GetColumnByName((*iterArray)->sIndex);
            if (nCol == -1) {
              gError.SetError(DBA_ERR_UNKNOWNVAL, (*iterArray)->sIndex, (*iterArray)->sID);
              return false;
            } // end if
            sKey = pSnapshot->GetField(nCol, nRow);

            if (!sKey.Empty()) {
              // find index matching to key
              for (nIndex = 0; nIndex < (*iterArray)->oValues.size(); ++nIndex) {
                if ((*iterArray)->oValues[nIndex].GetKey() == sKey) {
                  break;
                } // end if
              } // end for

              if ((sValue == DBA_SNAPSHOT_NULLVAL) && !(*iterArray)->sNullRep.Empty()) {
                sValue = (*iterArray)->sNullRep;
              } // end if

              (*iterArray)->oValues[nIndex].SetNewValue(sValue, (*iterArray)->nType);
              (*iterArray)->oValues[nIndex].SetPrecision((*iterArray)->nPrecision);
              (*iterArray)->oValues[nIndex].SetKey(sKey);

            } // end if
          } // end for

          // remove values are not in snapshot
          for (nIndex = 0; nIndex < (*iterArray)->oValues.size(); ++nIndex) {
            sKey  = (*iterArray)->oValues[nIndex].GetKey();
            sValue = pSnapshot->GetFieldByCondition((*iterArray)->sIndex, sKey, (*iterArray)->sColumn);
            if (sValue == DBA_SNAPSHOT_NOTEXIST) {
              (*iterArray)->oValues[nIndex].SetUndefined();
            } // end if
          } // end for

        } // end if
      } // end if
    } // end if

    ++iterArray;
  } // end while

  return true;
} // end DBA_configuration::GetSnapshot

//
// CalculateParameters
//
DBA_bool DBA_configuration::CalculateParameters
  (const DBA_bool bStartup)
{
  Tools_ExpressionValue   oValue;

  DBA_config_parameter_list::iterator iterParameter = m_lParameters.begin();
  while (iterParameter != m_lParameters.end()) {
    // check the instance type
    if (((*iterParameter)->bStartup && bStartup) || !(*iterParameter)->bStartup) {


      if (!(*iterParameter)->sFormula.Empty()) {
        if (!(*iterParameter)->oFormula.Value(oValue)) {
          // error make parameter undefined
          (*iterParameter)->SetUndefined();

          // check for warnings
          if (((*iterParameter)->oFormula.LastError() != Tools_Expression::ExError_DIV0     ) &&
              ((*iterParameter)->oFormula.LastError() != Tools_Expression::ExError_UNDEFVAL )    ) {
            // prepare error and return
            char szExtError[100];
            sprintf(szExtError, "%s at position %d in Parameter::Formula", (char *) (*iterParameter)->oFormula.LastErrorText().CharPtr(), (int) (*iterParameter)->oFormula.LastErrorPos());
            gError.SetError(DBA_ERR_CALC, (*iterParameter)->sID); 
            gError.SetExtText(szExtError); 
            return false;
          } // end if

        } else {
          if ((*iterParameter)->GetType() != oValue.GetType()) {
            gError.SetError(DBA_ERR_TYPE, (*iterParameter)->sID); 
          } else {
            if ((*iterParameter)->GetType() == Tools_ExpressionValue::ExTypeNumeric) {
              if (oValue.GetNumericValue() < 0) { 
                // Assign -1 if calculation has a negative result. A negative result may caused by 
                // a reset of the used kernel counters for this calculation.
                // see http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1141500
                (*iterParameter)->SetNewValue(-1, (*iterParameter)->GetType());
              } else {
                (*iterParameter)->SetNewValue(oValue.GetNumericValue(), (*iterParameter)->GetType());
              } // end if
            } else if ((*iterParameter)->GetType() == Tools_ExpressionValue::ExTypeString) {
              (*iterParameter)->SetNewValue(oValue.GetStringValue(), (*iterParameter)->GetType());
            } else {
              gError.SetError(DBA_ERR_TYPE, (*iterParameter)->sID); 
            } // end if
          } // end if
        } // end if
      }
    }
    ++iterParameter;
  }

  return true;
} // end CalculateParameters

//
// CalculateArrays
//
DBA_bool DBA_configuration::CalculateArrays
  (const DBA_bool bStartup)
{

  // iterate through arrays
  DBA_config_array_list::iterator         iterArr = m_lArrays.begin();
  Tools_ExpressionValueArray::size_type   nIndex;
  Tools_ExpressionValueArray::size_type   nMax;
  Tools_ExpressionValueArray            * pArray;
  Tools_ExpressionValue                   oValue;

  while (iterArr != m_lArrays.end()) {
    if (((*iterArr)->bStartup && bStartup) || !(*iterArr)->bStartup) {

      if (!(*iterArr)->sFormula.Empty()) {

        // find Index
        if (!(*iterArr)->sForEach.Empty()) {
          pArray = m_oValueProvider.AskForArray((*iterArr)->sForEach);
          if (pArray != NULL) {
            nMax = pArray->size();
          } else {
            char szExtError[100];
            sprintf(szExtError, "FOREACH object in array %s not found.", (*iterArr)->sID.CharPtr());
            gError.SetError(DBA_ERR_UNKNOWNOBJ, (*iterArr)->sForEach);
            gError.SetExtText(szExtError); 
            return false;
          } // end if
        } else {
          pArray = NULL;
          nMax   = 1;
        } // end if

        for (nIndex = 0; nIndex < nMax; ++nIndex) {

          if (pArray != NULL) {
            if (!(*pArray)[nIndex].IsDefined()) {
              (*iterArr)->oValues[nIndex].SetUndefined();
              continue; // for
            } // end if
          } // end if

          if (!(*iterArr)->oFormula.Value(oValue, nIndex)) {

            // error make parameter undefined
            (*iterArr)->oValues[nIndex].SetUndefined();

            // check for warnings
            if (((*iterArr)->oFormula.LastError() != Tools_Expression::ExError_DIV0     ) &&
                ((*iterArr)->oFormula.LastError() != Tools_Expression::ExError_UNDEFVAL )    ) {
              // prepare error and return
              char szExtError[100];
              sprintf(szExtError, "%s at position %d in Array::Formula", (char *) (*iterArr)->oFormula.LastErrorText().CharPtr(), (int) (*iterArr)->oFormula.LastErrorPos());
              gError.SetError(DBA_ERR_CALC, (*iterArr)->sID); 
              gError.SetExtText(szExtError); 
              return false;
            } // end if

          } else {
            if ((*iterArr)->nType != oValue.GetType()) {
              gError.SetError(DBA_ERR_TYPE, (*iterArr)->sID); 
            } else {
              if ((*iterArr)->nType == Tools_ExpressionValue::ExTypeNumeric) {
                (*iterArr)->oValues[nIndex].SetNewValue(oValue.GetNumericValue(), (*iterArr)->nType);
                (*iterArr)->oValues[nIndex].SetPrecision((*iterArr)->nPrecision);
              } else if ((*iterArr)->nType == Tools_ExpressionValue::ExTypeString) {
                (*iterArr)->oValues[nIndex].SetNewValue(oValue.GetStringValue(), (*iterArr)->nType);
                (*iterArr)->oValues[nIndex].SetPrecision((*iterArr)->nPrecision);
              } else {
                gError.SetError(DBA_ERR_TYPE, (*iterArr)->sID); 
              } // end if
            } // end if
          } // end if

        } // end for

      } // end if
    } // end if
    ++iterArr;
  } // end while

  return true;
} // end CalculateArrays

//
// CalculateMonitorExpression
//
DBA_bool DBA_configuration::CalculateMonitorExpression
   ( DBA_string                                         & sMonitor,
     DBA_string                                           sErrorText,
     Tools_Expression                                   & oFormula, 
     DBA_string                                         & sValue,
     DBA_config_monitor::monitor_value_array::size_type   nIndex )
{
  Tools_ExpressionValue   oValue;

  sValue.Erase();

  if (!oFormula.Value(oValue, nIndex)) {
    char szExtError[100];
    sprintf(szExtError, "%s at position %d in Monitor::%s", 
                        (char *) oFormula.LastErrorText().CharPtr(), 
                        (int)    oFormula.LastErrorPos(),
                        (char *) sErrorText.CharPtr());

    // check for warnings
    if ((oFormula.LastError() != Tools_Expression::ExError_DIV0     ) &&
        (oFormula.LastError() != Tools_Expression::ExError_UNDEFVAL )    ) {
      // prepare error and return
      gError.SetError(DBA_ERR_CALC, sMonitor); 
      gError.SetExtText(szExtError); 
      return false;
    } // end if
  } else {

    if (oValue.IsDefined()) {
      if (oValue.GetType() == Tools_ExpressionValue::ExTypeNumeric) {
        sValue = (oValue.GetNumericValue() != 0) ? oValue.GetStringValue().CharPtr() : "";
      } else  {
        sValue = oValue.GetStringValue();
      } // end if
    } // end if

  } // end if

  return true;
} // end DBA_configuration::CalculateMonitorExpression

//
// CalculateMonitors
//
DBA_bool DBA_configuration::CalculateMonitors
  (const DBA_bool bStartup)
{

  // iterate through monitors
  DBA_config_monitor_list::iterator                    iterMon = m_lMonitors.begin();
  DBA_config_monitor::monitor_value_array::size_type   nIndex;
  DBA_config_monitor::monitor_value_array::size_type   nMax;
  Tools_ExpressionValueArray                         * pArray;
  while (iterMon != m_lMonitors.end()) {
    // check the instance type
    if (((*iterMon)->bStartup && bStartup) || !(*iterMon)->bStartup) {

      // find Index
      if (!(*iterMon)->sForEach.Empty()) {
        pArray = m_oValueProvider.AskForArray((*iterMon)->sForEach);
        if (pArray != NULL) {
          nMax = pArray->size();
        } else {
          char szExtError[100];
          sprintf(szExtError, "FOREACH object in monitor %s not found.", (*iterMon)->sID.CharPtr());
          gError.SetError(DBA_ERR_UNKNOWNOBJ, (*iterMon)->sForEach);
          gError.SetExtText(szExtError); 
          return false;
        } // end if
      } else {
        pArray = NULL;
        nMax   = 1;
      } // end if


      // init values
      (*iterMon)->oValues.clear();

      for (nIndex = 0; nIndex < nMax; ++nIndex) {

        if (pArray != NULL) {
          if (!(*pArray)[nIndex].IsDefined()) {
            continue; // for
          } // end if
        } // end if

        // calculate expressions
        if (!(*iterMon)->sSkip1.Empty()) {
          if (!CalculateMonitorExpression((*iterMon)->sID, "Skip1", (*iterMon)->oSkip1, (*iterMon)->oValues[nIndex].sValSkip1, nIndex)) {
            return false;
          } else {
            (*iterMon)->oValues[nIndex].nSkip = ((*iterMon)->oValues[nIndex].sValSkip1.Length() > 0) ? DBA_config_monitor::Skip1 : DBA_config_monitor::SkipNone;
          } // end if
        } // end if

        if (!(*iterMon)->sSkip2.Empty() && ((*iterMon)->oValues[nIndex].nSkip == DBA_config_monitor::SkipNone)) {
          if (!CalculateMonitorExpression((*iterMon)->sID, "Skip2", (*iterMon)->oSkip2, (*iterMon)->oValues[nIndex].sValSkip2, nIndex)) {
            return false;
          } else {
            (*iterMon)->oValues[nIndex].nSkip = ((*iterMon)->oValues[nIndex].sValSkip2.Length() > 0) ? DBA_config_monitor::Skip2 : DBA_config_monitor::SkipNone;
          } // end if
        } // end if

        if (!(*iterMon)->sSkip3.Empty() && ((*iterMon)->oValues[nIndex].nSkip == DBA_config_monitor::SkipNone)) {
          if (!CalculateMonitorExpression((*iterMon)->sID, "Skip3", (*iterMon)->oSkip3, (*iterMon)->oValues[nIndex].sValSkip3, nIndex)) {
            return false;
          } else {
            (*iterMon)->oValues[nIndex].nSkip = ((*iterMon)->oValues[nIndex].sValSkip3.Length() > 0) ? DBA_config_monitor::Skip3 : DBA_config_monitor::SkipNone;
          } // end if
        } // end if

        if (!(*iterMon)->sError.Empty() && 
            ((*iterMon)->oValues[nIndex].nSkip    == DBA_config_monitor::SkipNone) && 
            ((*iterMon)->oValues[nIndex].nWarning == DBA_config_monitor::WrnNone )    ) {
          if (!CalculateMonitorExpression((*iterMon)->sID, "Error", (*iterMon)->oError, (*iterMon)->oValues[nIndex].sValError, nIndex)) {
            return false;
          } else {
            (*iterMon)->oValues[nIndex].nWarning = ((*iterMon)->oValues[nIndex].sValError.Length() > 0) ? DBA_config_monitor::Err : DBA_config_monitor::WrnNone;
            (*iterMon)->oError.ExplainValue((*iterMon)->oValues[nIndex].sOrgExpression, (*iterMon)->oValues[nIndex].sValExpression, nIndex);
          } // end if
        } // end if

        if (!(*iterMon)->sWarning3.Empty() && 
            ((*iterMon)->oValues[nIndex].nSkip    == DBA_config_monitor::SkipNone) && 
            ((*iterMon)->oValues[nIndex].nWarning == DBA_config_monitor::WrnNone )    ) {
          if (!CalculateMonitorExpression((*iterMon)->sID, "Warning3", (*iterMon)->oWarning3, (*iterMon)->oValues[nIndex].sValWarning3, nIndex)) {
            return false;
          } else {
            (*iterMon)->oValues[nIndex].nWarning = ((*iterMon)->oValues[nIndex].sValWarning3.Length() > 0) ? DBA_config_monitor::Wrn3 : DBA_config_monitor::WrnNone;
            (*iterMon)->oWarning3.ExplainValue((*iterMon)->oValues[nIndex].sOrgExpression, (*iterMon)->oValues[nIndex].sValExpression, nIndex);
          } // end if
        } // end if

        if (!(*iterMon)->sWarning2.Empty() && 
            ((*iterMon)->oValues[nIndex].nSkip    == DBA_config_monitor::SkipNone) && 
            ((*iterMon)->oValues[nIndex].nWarning == DBA_config_monitor::WrnNone )    ) {
          if (!CalculateMonitorExpression((*iterMon)->sID, "Warning2", (*iterMon)->oWarning2, (*iterMon)->oValues[nIndex].sValWarning2, nIndex)) {
            return false;
          } else {
            (*iterMon)->oValues[nIndex].nWarning = ((*iterMon)->oValues[nIndex].sValWarning2.Length() > 0) ? DBA_config_monitor::Wrn2 : DBA_config_monitor::WrnNone;
            (*iterMon)->oWarning2.ExplainValue((*iterMon)->oValues[nIndex].sOrgExpression, (*iterMon)->oValues[nIndex].sValExpression, nIndex);
          } // end if
        } // end if

        if (!(*iterMon)->sWarning1.Empty() && 
            ((*iterMon)->oValues[nIndex].nSkip    == DBA_config_monitor::SkipNone) && 
            ((*iterMon)->oValues[nIndex].nWarning == DBA_config_monitor::WrnNone )    ) {
          if (!CalculateMonitorExpression((*iterMon)->sID, "Warning1", (*iterMon)->oWarning1, (*iterMon)->oValues[nIndex].sValWarning1, nIndex)) {
            return false;
          } else {
            (*iterMon)->oValues[nIndex].nWarning = ((*iterMon)->oValues[nIndex].sValWarning1.Length() > 0) ? DBA_config_monitor::Wrn1 : DBA_config_monitor::WrnNone;
            (*iterMon)->oWarning1.ExplainValue((*iterMon)->oValues[nIndex].sOrgExpression, (*iterMon)->oValues[nIndex].sValExpression, nIndex);
          } // end if
        } // end if

        if (!(*iterMon)->sInformation.Empty() && 
            ((*iterMon)->oValues[nIndex].nSkip    == DBA_config_monitor::SkipNone) && 
            ((*iterMon)->oValues[nIndex].nWarning == DBA_config_monitor::WrnNone )    ) {
          if (!CalculateMonitorExpression((*iterMon)->sID, "Information", (*iterMon)->oInformation, (*iterMon)->oValues[nIndex].sValInformation, nIndex)) {
            return false;
          } else {
            (*iterMon)->oValues[nIndex].nWarning = ((*iterMon)->oValues[nIndex].sValInformation.Length() > 0) ? DBA_config_monitor::WrnInf : DBA_config_monitor::WrnNone;
            (*iterMon)->oInformation.ExplainValue((*iterMon)->oValues[nIndex].sOrgExpression, (*iterMon)->oValues[nIndex].sValExpression, nIndex);
          } // end if
        } // end if

        if (!(*iterMon)->sLabel.Empty() && 
            ((*iterMon)->oValues[nIndex].nWarning != DBA_config_monitor::WrnNone )    ) {
          if (!CalculateMonitorExpression((*iterMon)->sID, "Label", (*iterMon)->oLabel, (*iterMon)->oValues[nIndex].sValLabel, nIndex)) {
            return false;
          } // end if
        } // end if

      } // end for
    } // end if
    ++iterMon;
  } // end while

  return true;
} // end CalculateMonitors

//
// ProtocolParameters
//
DBA_bool DBA_configuration::ProtocolParameters(DBA_protocol * pProtocol,
                                               DBA_ulong      nCount,
                                               time_t         tAct,
                                               time_t         tLast,
                                               time_t         tNow)
{
  pProtocol->SetParameterStatus(nCount, tAct, tLast, tNow);

  DBA_config_class_list::iterator iterClass = m_lClasses.begin();
  while (iterClass != m_lClasses.end()) {
    if (((*iterClass)->nInstanceType & m_nInstance) == m_nInstance) {

      pProtocol->ClearParameterValues((*iterClass)->nFileIndex);  
      DBA_config_parameter_list::iterator iterParameter = (*iterClass)->lParameters.begin();
      while (iterParameter != (*iterClass)->lParameters.end()) {
        // add the parameter
        if ((*iterParameter)->bProtocol) {
          if ((*iterParameter)->IsDefined()) {
            pProtocol->AddParameterValue((*iterClass)->nFileIndex, (*iterParameter)->GetStringValue());  
          } else {
            if ((*iterParameter)->GetType() == Tools_ExpressionValue::ExTypeNumeric) {
              (*iterParameter)->SetNewValue(0, (*iterParameter)->GetType());
            } else if ((*iterParameter)->GetType() == Tools_ExpressionValue::ExTypeString) {
              (*iterParameter)->SetNewValue("", (*iterParameter)->GetType());
            } // end if
            pProtocol->AddParameterValue((*iterClass)->nFileIndex, (*iterParameter)->GetStringValue());  
            (*iterParameter)->SetUndefined();
          } // end if
        }
        ++iterParameter;
      }

      if (!pProtocol->WriteParameters((*iterClass)->nFileIndex)) { 
        return false;
      }

    } // end if

    ++iterClass;
  }

  return true;
}

//
// ProtocolHistory
//
DBA_bool DBA_configuration::ProtocolHistory(DBA_protocol * pProtocol,
                                            DBA_ulong      nCount,
                                            time_t         tAct,
                                            DBA_bool       bStartup)
{
  pProtocol->SetHistoryStatus(nCount, tAct);
  pProtocol->ClearHistoryText();

  DBA_bool bHistoryOK = true;

  DBA_config_monitor_list::iterator                  iterMonitor = m_lMonitors.begin();
  DBA_config_monitor::monitor_value_array::size_type nIndex;

  while (iterMonitor != m_lMonitors.end()) {
    // check the instance type
    if ((*iterMonitor)->bStartup == bStartup) {

      for (nIndex = 0; nIndex < (*iterMonitor)->oValues.size() ; ++nIndex) {

        DBA_string sTitle = (*iterMonitor)->oValues[nIndex].sValLabel;
        if (sTitle.Empty()) {
          sTitle = (*iterMonitor)->sID;
        } // end if

        if ((*iterMonitor)->oValues[nIndex].nWarning != DBA_config_monitor::WrnNone ) {
          bHistoryOK = false;

          switch ((*iterMonitor)->oValues[nIndex].nWarning) {
            case  DBA_config_monitor::Err:
              pProtocol->AddHistoryWarning(4, sTitle);
              break;
            case  DBA_config_monitor::Wrn3:
              pProtocol->AddHistoryWarning(3, sTitle);
              break;
            case  DBA_config_monitor::Wrn2:
              pProtocol->AddHistoryWarning(2, sTitle);
              break;
            case  DBA_config_monitor::Wrn1:
              pProtocol->AddHistoryWarning(1, sTitle);
              break;
            case  DBA_config_monitor::WrnInf:
              pProtocol->AddHistoryWarning(0, sTitle);
              break;
          } // end switch

          pProtocol->AddHistoryCondition((*iterMonitor)->oValues[nIndex].sOrgExpression);
          pProtocol->AddHistoryValue((*iterMonitor)->oValues[nIndex].sValExpression);

          if (!(*iterMonitor)->sUserAction.Empty()) {
            pProtocol->AddHistoryAction((*iterMonitor)->sUserAction);
          }
          if (!(*iterMonitor)->sDescription.Empty()) {
            pProtocol->AddHistoryDescription((*iterMonitor)->sDescription);
          }
        }

      } // end for

    } // end if
    ++iterMonitor;
  }

  if (bHistoryOK) {
    pProtocol->SetHistoryOK();
  }

  if (!pProtocol->WriteHistory()) { 
    return false;
  }

  return true;
}

//
// ParseExpressions
//
DBA_bool DBA_configuration::ParseExpressions()
{
  // expression are in m_lParameters (Environment, Constants, Parameters)
  // and in m_lMonitor (Monitor)
  
  // iterate through parameters
  DBA_config_parameter_list::iterator iterPar = m_lParameters.begin();
  while (iterPar != m_lParameters.end()) {
    // parse expression
    if (!(*iterPar)->sFormula.Empty()) {
      // formula found 
      if (!(*iterPar)->oFormula.ParseExpression((*iterPar)->sFormula)) {
        // parse error
        gError.SetError(DBA_ERR_PARSE, (*iterPar)->sID); 
        char szTmp[100];
        sprintf(szTmp, "%s at position %d in Parameter::Formula", (char *) (*iterPar)->oFormula.LastErrorText().CharPtr(), (int) (*iterPar)->oFormula.LastErrorPos());
        gError.SetExtText(szTmp); 
        return false;
      } // end if
    } // end if

    ++iterPar;
  } // end while

  // iterate through arrays
  DBA_config_array_list::iterator iterArr = m_lArrays.begin();
  while (iterArr != m_lArrays.end()) {
    // parse expression
    if (!(*iterArr)->sFormula.Empty()) {
      // formula found 
      if (!(*iterArr)->oFormula.ParseExpression((*iterArr)->sFormula)) {
        // parse error
        gError.SetError(DBA_ERR_PARSE, (*iterArr)->sID); 
        char szTmp[100];
        sprintf(szTmp, "%s at position %d in Array::Formula", (char *) (*iterArr)->oFormula.LastErrorText().CharPtr(), (int) (*iterArr)->oFormula.LastErrorPos());
        gError.SetExtText(szTmp); 
        return false;
      } // end if
    } // end if

    ++iterArr;
  } // end while

  // iterate through monitors
  DBA_config_monitor_list::iterator iterMon = m_lMonitors.begin();
  while (iterMon != m_lMonitors.end()) {
    // parse expressions
    if (!(*iterMon)->sLabel.Empty()) {
      // formula found 
      if (!(*iterMon)->oLabel.ParseExpression((*iterMon)->sLabel)) {
        // parse error
        gError.SetError(DBA_ERR_PARSE, (*iterMon)->sID); 
        char szTmp[100];
        sprintf(szTmp, "%s at position %d in Monitor::Label", (char *) (*iterMon)->oLabel.LastErrorText().CharPtr(), (int) (*iterMon)->oLabel.LastErrorPos());
        gError.SetExtText(szTmp); 
        return false;
      } // end if
    } // end if

    if (!(*iterMon)->sSkip1.Empty()) {
      // formula found 
      if (!(*iterMon)->oSkip1.ParseExpression((*iterMon)->sSkip1)) {
        // parse error
        gError.SetError(DBA_ERR_PARSE, (*iterMon)->sID); 
        char szTmp[100];
        sprintf(szTmp, "%s at position %d in Monitor::Skip1", (char *) (*iterMon)->oSkip1.LastErrorText().CharPtr(), (int) (*iterMon)->oSkip1.LastErrorPos());
        gError.SetExtText(szTmp); 
        return false;
      } // end if
    } // end if

    if (!(*iterMon)->sSkip2.Empty()) {
      // formula found 
      if (!(*iterMon)->oSkip2.ParseExpression((*iterMon)->sSkip2)) {
        // parse error
        gError.SetError(DBA_ERR_PARSE, (*iterMon)->sID); 
        char szTmp[100];
        sprintf(szTmp, "%s at position %d in Monitor::Skip2", (char *) (*iterMon)->oSkip2.LastErrorText().CharPtr(), (int) (*iterMon)->oSkip2.LastErrorPos());
        gError.SetExtText(szTmp); 
        return false;
      } // end if
    } // end if

    if (!(*iterMon)->sSkip3.Empty()) {
      // formula found 
      if (!(*iterMon)->oSkip3.ParseExpression((*iterMon)->sSkip3)) {
        // parse error
        gError.SetError(DBA_ERR_PARSE, (*iterMon)->sID); 
        char szTmp[100];
        sprintf(szTmp, "%s at position %d in Monitor::Skip3", (char *) (*iterMon)->oSkip3.LastErrorText().CharPtr(), (int) (*iterMon)->oSkip3.LastErrorPos());
        gError.SetExtText(szTmp); 
        return false;
      } // end if
    } // end if

    if (!(*iterMon)->sWarning1.Empty()) {
      // formula found 
      if (!(*iterMon)->oWarning1.ParseExpression((*iterMon)->sWarning1)) {
        // parse error
        gError.SetError(DBA_ERR_PARSE, (*iterMon)->sID); 
        char szTmp[100];
        sprintf(szTmp, "%s at position %d in Monitor::Warning1", (char *) (*iterMon)->oWarning1.LastErrorText().CharPtr(), (int) (*iterMon)->oWarning1.LastErrorPos());
        gError.SetExtText(szTmp); 
        return false;
      } // end if
    } // end if

    if (!(*iterMon)->sWarning2.Empty()) {
      // formula found 
      if (!(*iterMon)->oWarning2.ParseExpression((*iterMon)->sWarning2)) {
        // parse error
        gError.SetError(DBA_ERR_PARSE, (*iterMon)->sID); 
        char szTmp[100];
        sprintf(szTmp, "%s at position %d in Monitor::Warning2", (char *) (*iterMon)->oWarning2.LastErrorText().CharPtr(), (int) (*iterMon)->oWarning2.LastErrorPos());
        gError.SetExtText(szTmp); 
        return false;
      } // end if
    } // end if

    if (!(*iterMon)->sError.Empty()) {
      // formula found 
      if (!(*iterMon)->oError.ParseExpression((*iterMon)->sError)) {
        // parse error
        gError.SetError(DBA_ERR_PARSE, (*iterMon)->sID); 
        char szTmp[100];
        sprintf(szTmp, "%s at position %d in Monitor::Error", (char *) (*iterMon)->oError.LastErrorText().CharPtr(), (int) (*iterMon)->oError.LastErrorPos());
        gError.SetExtText(szTmp); 
        return false;
      } // end if
    } // end if

    if (!(*iterMon)->sWarning3.Empty()) {
      // formula found 
      if (!(*iterMon)->oWarning3.ParseExpression((*iterMon)->sWarning3)) {
        // parse error
        gError.SetError(DBA_ERR_PARSE, (*iterMon)->sID); 
        char szTmp[100];
        sprintf(szTmp, "%s at position %d in Monitor::Warning3", (char *) (*iterMon)->oWarning3.LastErrorText().CharPtr(), (int) (*iterMon)->oWarning3.LastErrorPos());
        gError.SetExtText(szTmp); 
        return false;
      } // end if
    } // end if

    if (!(*iterMon)->sInformation.Empty()) {
      // formula found 
      if (!(*iterMon)->oInformation.ParseExpression((*iterMon)->sInformation)) {
        // parse error
        gError.SetError(DBA_ERR_PARSE, (*iterMon)->sID); 
        char szTmp[100];
        sprintf(szTmp, "%s at position %d in Monitor::Information", (char *) (*iterMon)->oInformation.LastErrorText().CharPtr(), (int) (*iterMon)->oInformation.LastErrorPos());
        gError.SetExtText(szTmp); 
        return false;
      } // end if
    } // end if

    ++iterMon;
  } // end while


  return true;
} // end ParseExpressions

//
// Read
//
// read the configuration file
//
DBA_bool DBA_configuration::Read()
{
  DBA_bool   bRead = false;
  DBA_string sLine;

  while (!feof(m_hFile)) {
    bRead = false;
    if (GetNextLine(sLine)) {
  
      if (IsSectionBegin(sLine)) {
        sLine.ToUpper();

        if (sLine == DBA_CNFTXT_CON) {
          bRead = ReadConstants();
        } else if (sLine == DBA_CNFTXT_ENV) {
          bRead = ReadEnvironment();
        } else if (sLine == DBA_CNFTXT_MON) {
          bRead = ReadMonitor();
        } else if (sLine == DBA_CNFTXT_PAR) {
          bRead = ReadParameter();
        } else if (sLine == DBA_CNFTXT_ARR) {
          bRead = ReadArray();
        } else if (sLine == DBA_CNFTXT_SET) {
          bRead = ReadSettings();
        } else if (sLine == DBA_CNFTXT_VIE) {
          bRead = ReadView();
        }
      } else {
        bRead = true;
      }

    }
    if (!bRead) {
      return false;
    }
  }
  
  return true;
}

//
// ReadConstants
//
// read all in the constants section - [Constants]
//
DBA_bool DBA_configuration::ReadConstants()
{
  DBA_bool   bRC = false;
  DBA_string sLine;
  DBA_string sName;
  DBA_string sValue;
  fpos_t     nPos;
  DBA_int    nLine;

  DBA_config_parameter * pConstant = NULL;
  
  fgetpos(m_hFile, &nPos);
  nLine = m_nLine;

  while (!feof(m_hFile)) {
    if (GetNextLine(sLine)) {
      if (IsSectionBegin(sLine)) {
        fsetpos(m_hFile, &nPos);
        m_nLine = nLine;
        bRC = true;
        break;
      } else {
        if (!pConstant) {
          pConstant = new DBA_config_parameter(DBA_CNFPTYPE_CONSTANT, m_oValueProvider);
          if (pConstant == NULL) {
            gError.SetError(DBA_ERR_NOMEM);
            bRC = false;
            break;
          } // end if
        }

        SplitNameValue(sLine, sName, sValue);

        DBA_config_key nKey = DBA_CNFKEY_UNK;
        DBA_bool       bInvalidKey = false;

        if (GetConfigKey(sName, nKey)) {
          switch (nKey) {
            case DBA_CNFKEY_ID:  pConstant->sID         = sValue; break;
            case DBA_CNFKEY_TYP: pConstant->SetNewValue("", GetType(sValue)); break;
            case DBA_CNFKEY_VAL: pConstant->SetNewValue(sValue);  break;
            case DBA_CNFKEY_MIV: pConstant->oMinVersion = sValue; break;
            case DBA_CNFKEY_MAV: pConstant->oMaxVersion = sValue; break;
            case DBA_CNFKEY_ITY: 
              pConstant->nInstanceType  = GetInstanceType(sValue); 
              break;
            default: 
              bInvalidKey = true; break;
          }  
        } else {
          bInvalidKey = true;
        }

        if (bInvalidKey) {
          gError.SetError(DBA_ERR_INVCONFIGKEY, GetLineNumStr());
          bRC = false;
          break;
        } // end if
      }
    }

  }  

  if (!bRC && feof(m_hFile)) {
    bRC = true;
  } // end if

  // ready handling
    if (bRC && pConstant) {
    // check version
    if (((pConstant->nInstanceType & m_nInstance) == m_nInstance ) &&
        (pConstant->oMinVersion                   <= m_oDBVersion) &&
        (pConstant->oMaxVersion                   >= m_oDBVersion)    ) {
      // check for duplicates
      if (GetParameter(pConstant->sID) != NULL) {
        gError.SetError(DBA_ERR_DUPLICATE, pConstant->sID, GetLineNumStr());
        delete pConstant;
        bRC = false;
      } else {
        m_lParameters.push_back(pConstant);
      } // end if
    } else {
      delete pConstant;
    } // end if
  } // end if

  return bRC;
} // end DBA_configuration::ReadConstants

//
// ReadEnvironment
//
// read all in a environment section - [Environment]
//
DBA_bool DBA_configuration::ReadEnvironment()
{
  DBA_bool   bRC = false;
  DBA_string sLine;
  DBA_string sName;
  DBA_string sValue;
  fpos_t     nPos;
  DBA_int    nLine;

  DBA_config_parameter * pEnvironment = NULL;
  
  fgetpos(m_hFile, &nPos);
  nLine = m_nLine;

  while (!feof(m_hFile)) {
    if (GetNextLine(sLine)) {
      if (IsSectionBegin(sLine)) {
        fsetpos(m_hFile, &nPos);
        m_nLine = nLine;
        bRC = true;
        break;
      } else {
        if (!pEnvironment) {
          pEnvironment = new DBA_config_parameter(DBA_CNFPTYPE_ENVIRONMENT, m_oValueProvider);
          if (pEnvironment == NULL) {
            gError.SetError(DBA_ERR_NOMEM);
            bRC = false;
            break;
          } // end if
        }

        SplitNameValue(sLine, sName, sValue);

        DBA_config_key nKey = DBA_CNFKEY_UNK;
        DBA_bool       bInvalidKey = false;

        if (GetConfigKey(sName, nKey)) {
          switch (nKey) {
            case DBA_CNFKEY_ID:  pEnvironment->sID            = sValue; break;
            case DBA_CNFKEY_LBL: pEnvironment->sLabel         = sValue; break;
            case DBA_CNFKEY_CLS: 
              sValue.ToUpper();
              pEnvironment->sClass = sValue; 
              AddClass(sValue,&(pEnvironment->oClasses[0]));
              break;
            case DBA_CNFKEY_TAB: pEnvironment->sTable         = sValue; break;
            case DBA_CNFKEY_COL: pEnvironment->sColumn        = sValue; break;
            case DBA_CNFKEY_CND: {
              DBA_string sConditionValue;
              DBA_bool bString = false;
              SplitNameValue(sValue, pEnvironment->sConditionColumn, sConditionValue);
              if (!sConditionValue.Empty()) {
                if ((sConditionValue[0] == '\'') &&
                    (sConditionValue[sConditionValue.Length()-1] == '\'')) {
                  sConditionValue = sConditionValue.SubStr(1, sConditionValue.Length()-2);
                  bString = true;
                } // end if

                if (sConditionValue[0] == '$') {
                  if (sConditionValue[1] != '$') {
                    DBA_config_parameter * pCondition = GetParameter(sConditionValue.SubStr(1, sConditionValue.Length()-1));
                    if (pCondition != NULL) {
                      sConditionValue = pCondition->GetStringValue();
                      bString = bString || (pCondition->GetType() == DBA_config_parameter::ExTypeString);
                    } else {
                      gError.SetError(DBA_ERR_UNKNOWNVAL, sConditionValue.SubStr(1, sConditionValue.Length()-1), pEnvironment->sID);
                      return false;
                    } // end if
                  } else {
                    sConditionValue = sConditionValue.SubStr(1, sConditionValue.Length()-1);
                  } // end if
                } // end if

                if (bString) {
                  pEnvironment->sConditionValue.Assign("'")
                                               .Append(sConditionValue)
                                               .Append("'");
                } else {
                  pEnvironment->sConditionValue = sConditionValue;
                } // end if

              } // end if
            } // end case
            break;
            case DBA_CNFKEY_DES: pEnvironment->sDescription   = sValue; break;
            case DBA_CNFKEY_FOR: pEnvironment->sFormula       = sValue; break;
            case DBA_CNFKEY_UNI: pEnvironment->sUnit          = sValue; break;
            case DBA_CNFKEY_MIV: pEnvironment->oMinVersion    = sValue; break;
            case DBA_CNFKEY_MAV: pEnvironment->oMaxVersion    = sValue; break;
            case DBA_CNFKEY_PRT: pEnvironment->bProtocol      = (atoi(sValue.CharPtr()) != 0); break;
            case DBA_CNFKEY_TYP: pEnvironment->SetNewValue("", GetType(sValue)); break;
            case DBA_CNFKEY_ITY: 
              pEnvironment->nInstanceType  = GetInstanceType(sValue); 
              break;
            default: 
              bInvalidKey = true; break;
          }  
        } else {
          bInvalidKey = true;
        }

        if (bInvalidKey) {
          gError.SetError(DBA_ERR_INVCONFIGKEY, GetLineNumStr());
          bRC = false;
          break;
        } // end if
      }
    }

  } // end while

  if (!bRC && feof(m_hFile)) {
    bRC = true;
  }

  if (bRC && pEnvironment) {
    // check version
    if (((pEnvironment->nInstanceType & m_nInstance) == m_nInstance ) &&
        (pEnvironment->oMinVersion                   <= m_oDBVersion) &&
        (pEnvironment->oMaxVersion                   >= m_oDBVersion)    ) {
      // check for duplicates
      if (GetParameter(pEnvironment->sID) != NULL) {
        gError.SetError(DBA_ERR_DUPLICATE, pEnvironment->sID, GetLineNumStr());
        delete pEnvironment;
        bRC = false;
      } else {
        // success
        if ((pEnvironment->oClasses[0] != NULL) && pEnvironment->bProtocol) {
          pEnvironment->oClasses[0]->nInstanceType = pEnvironment->oClasses[0]->nInstanceType | pEnvironment->nInstanceType;
        } // end if
        if (pEnvironment->oClasses[0]) {
          if (pEnvironment->bProtocol) {
            pEnvironment->oClasses[0]->lParameters.push_back(pEnvironment);
          }
        }
        m_lParameters.push_back(pEnvironment);
      } // end if
    } else {
      delete pEnvironment;
    } // end if
  } // end if

  return bRC;
} // end DBA_configuration::ReadEnvironment

//
// ReadMonitor
//
// read all in a monitor section - [Monitor]
//
DBA_bool DBA_configuration::ReadMonitor()
{
  DBA_bool   bRC = false;
  DBA_string sLine;
  DBA_string sName;
  DBA_string sValue;
  fpos_t     nPos;
  DBA_int    nLine;

  DBA_config_monitor * pMonitor = NULL;
  
  fgetpos(m_hFile, &nPos);
  nLine = m_nLine;

  while (!feof(m_hFile)) {
    if (GetNextLine(sLine)) {
      if (IsSectionBegin(sLine)) {
        fsetpos(m_hFile, &nPos);
        m_nLine = nLine;
        bRC = true;
        break;
      } else {
        if (!pMonitor) {
          pMonitor = new DBA_config_monitor(m_oValueProvider);
          if (!pMonitor) {
            gError.SetError(DBA_ERR_NOMEM);
            bRC = false;
            break;
          }
        }

        SplitNameValue(sLine, sName, sValue);

        DBA_config_key nKey = DBA_CNFKEY_UNK;
        DBA_bool       bInvalidKey = false;

        if (GetConfigKey(sName, nKey)) {
          switch (nKey) {
            case DBA_CNFKEY_ID:  pMonitor->sID            = sValue; break;
            case DBA_CNFKEY_LBL: pMonitor->sLabel         = sValue; break;
            case DBA_CNFKEY_EAC: pMonitor->sForEach       = sValue; break;
            case DBA_CNFKEY_CLS: pMonitor->sClass         = sValue; break;
            case DBA_CNFKEY_DES: pMonitor->sDescription   = sValue; break;
            case DBA_CNFKEY_MIV: pMonitor->oMinVersion    = sValue; break;
            case DBA_CNFKEY_MAV: pMonitor->oMaxVersion    = sValue; break;
            case DBA_CNFKEY_SK1: pMonitor->sSkip1         = sValue; break;
            case DBA_CNFKEY_SK2: pMonitor->sSkip2         = sValue; break;
            case DBA_CNFKEY_SK3: pMonitor->sSkip3         = sValue; break;
            case DBA_CNFKEY_INF: pMonitor->sInformation   = sValue; break;
            case DBA_CNFKEY_WA1: pMonitor->sWarning1      = sValue; break;
            case DBA_CNFKEY_WA2: pMonitor->sWarning2      = sValue; break;
            case DBA_CNFKEY_WA3: pMonitor->sWarning3      = sValue; break;
            case DBA_CNFKEY_ERR: pMonitor->sError         = sValue; break;
            case DBA_CNFKEY_UAC: pMonitor->sUserAction    = sValue; break;
            case DBA_CNFKEY_STP: pMonitor->bStartup       = (atoi(sValue.CharPtr()) != 0); break;
            case DBA_CNFKEY_ITY: 
              pMonitor->nInstanceType  = GetInstanceType(sValue); 
              break;
            default: 
              bInvalidKey = true; break;
          }  
        } else {
          bInvalidKey = true;
        }

        if (bInvalidKey) {
          gError.SetError(DBA_ERR_INVCONFIGKEY, GetLineNumStr());
          bRC = false;
          break;
        }
      }
    }
  } // end while

  if (!bRC && feof(m_hFile)) {
    bRC = true;
  }

  if (bRC && pMonitor) {
    // check version
    if (((pMonitor->nInstanceType & m_nInstance) == m_nInstance ) &&
        (pMonitor->oMinVersion                   <= m_oDBVersion) &&
        (pMonitor->oMaxVersion                   >= m_oDBVersion)    ) {
      // check for duplicates
      if (GetMonitor(pMonitor->sID) != NULL) {
        gError.SetError(DBA_ERR_DUPLICATE, pMonitor->sID, GetLineNumStr());
        delete pMonitor;
        bRC = false;
      } else {
        m_lMonitors.push_back(pMonitor);
      } // end if
    } else {
      delete pMonitor;
    } // end if
  } // end if

  return bRC;
} // end DBA_configuration::ReadMonitor

//
// ReadParameter
//
// read all in a parameter section - [Parameter]
//
DBA_bool DBA_configuration::ReadParameter()
{
  DBA_bool   bRC = false;
  DBA_string sLine;
  DBA_string sName;
  DBA_string sValue;
  fpos_t     nPos;
  DBA_int    nLine;
  DBA_string sClass;
  DBA_string::BasisElementIndex nStart = 0;
  DBA_string::BasisElementIndex nEnd   = 0;
  DBA_config_class_array::size_type nIndex;

  DBA_config_parameter * pParameter = NULL;
  
  fgetpos(m_hFile, &nPos);
  nLine = m_nLine;

  while (!feof(m_hFile)) {
    if (GetNextLine(sLine)) {
      // parameter ready handling
      if (IsSectionBegin(sLine)) {
        // reset file and return
        fsetpos(m_hFile, &nPos);
        m_nLine = nLine;
        bRC = true;
        break;
      } else {
        if (!pParameter) {
          pParameter = new DBA_config_parameter(DBA_CNFPTYPE_PARAMETER, m_oValueProvider);
          if (pParameter == NULL) {
            gError.SetError(DBA_ERR_NOMEM);
            bRC = false;
            break;
          } // end if
        }

        SplitNameValue(sLine, sName, sValue);

        DBA_config_key nKey = DBA_CNFKEY_UNK;
        DBA_bool       bInvalidKey = false;

        if (GetConfigKey(sName, nKey)) {
          switch (nKey) {
            case DBA_CNFKEY_ID:  pParameter->sID          = sValue; break;
            case DBA_CNFKEY_LBL: pParameter->sLabel       = sValue; break;
            case DBA_CNFKEY_CLS: 
              sValue.ToUpper();
              pParameter->sClass = sValue;
              nStart = 0;
              nEnd   = sValue.Find(DBA_CNFCHR_SEP);
              while (nEnd != DBA_string::NPos) {
                sClass = sValue.SubStr(nStart, nEnd);
                AddClass(sClass,&(pParameter->oClasses[pParameter->oClasses.size()]));
                nStart = nEnd + 1;
                nEnd   = sValue.Find(DBA_CNFCHR_SEP, nStart);
              } // end while
              sClass = sValue.SubStr(nStart);
              AddClass(sClass,&(pParameter->oClasses[pParameter->oClasses.size()]));
              break;
            case DBA_CNFKEY_TAB: 
              if (!pParameter->sFormula.Empty()) {
                gError.SetError(DBA_ERR_FORMTAB, pParameter->sID, GetLineNumStr());
                return false;
              } else {
                sValue.ToUpper();
                pParameter->sTable = sValue; 
                AddTable(sValue,&(pParameter->pTable));
              } // end if
              break;
            case DBA_CNFKEY_COL: pParameter->sColumn        = sValue; break;
            case DBA_CNFKEY_CND: {
              DBA_string sConditionValue;
              DBA_bool bString = false;
              SplitNameValue(sValue, pParameter->sConditionColumn, sConditionValue);
              if (!sConditionValue.Empty()) {
                if ((sConditionValue[0] == '\'') &&
                    (sConditionValue[sConditionValue.Length()-1] == '\'')) {
                  sConditionValue = sConditionValue.SubStr(1, sConditionValue.Length()-2);
                  bString = true;
                } // end if

                if (sConditionValue[0] == '$') {
                  if (sConditionValue[1] != '$') {
                    DBA_config_parameter * pCondition = GetParameter(sConditionValue.SubStr(1, sConditionValue.Length()-1));
                    if (pCondition != NULL) {
                      sConditionValue = pCondition->GetStringValue();
                      bString = bString || (pCondition->GetType() == DBA_config_parameter::ExTypeString);
                    } else {
                      gError.SetError(DBA_ERR_UNKNOWNVAL, sConditionValue.SubStr(1, sConditionValue.Length()-1), pParameter->sID);
                      return false;
                    } // end if
                  } else {
                    sConditionValue = sConditionValue.SubStr(1, sConditionValue.Length()-1);
                  } // end if
                } // end if

                if (bString) {
                  pParameter->sConditionValue.Assign("'")
                                             .Append(sConditionValue)
                                             .Append("'");
                } else {
                  pParameter->sConditionValue = sConditionValue;
                } // end if

              } // end if
            } // end case
            break;

            case DBA_CNFKEY_DEF: pParameter->sDefault       = sValue; break;
            case DBA_CNFKEY_DES: pParameter->sDescription   = sValue; break;
            case DBA_CNFKEY_ADD: pParameter->sProtocolInfo  = sValue; break;
            case DBA_CNFKEY_CUR: pParameter->bCurve         = (atoi(sValue.CharPtr()) != 0); break;
            case DBA_CNFKEY_PRT: pParameter->bProtocol      = (atoi(sValue.CharPtr()) != 0); break;
            case DBA_CNFKEY_IMP: pParameter->bImportant     = (atoi(sValue.CharPtr()) != 0); break;
            case DBA_CNFKEY_STP: pParameter->bStartup       = (atoi(sValue.CharPtr()) != 0); break;
            case DBA_CNFKEY_FOR: 
              if (!pParameter->sTable.Empty()) {
                gError.SetError(DBA_ERR_FORMTAB, pParameter->sID, GetLineNumStr());
                return false;
              } else {
                pParameter->sFormula       = sValue; 
              } // end if
              break;
            case DBA_CNFKEY_UNI: pParameter->sUnit          = sValue; break;
            case DBA_CNFKEY_MIV: pParameter->oMinVersion    = sValue; break;
            case DBA_CNFKEY_MAV: pParameter->oMaxVersion    = sValue; break;
            case DBA_CNFKEY_PRE: 
              pParameter->SetPrecision(atoi(sValue.CharPtr())); 
              break;
            case DBA_CNFKEY_NUR: pParameter->sNullRep       = sValue; break;
            case DBA_CNFKEY_TYP: pParameter->SetNewValue("", GetType(sValue)); break;
            case DBA_CNFKEY_SCA: pParameter->sScaling       = sValue; break;
            case DBA_CNFKEY_ITY: 
              pParameter->nInstanceType  = GetInstanceType(sValue); 
              break;
            default: 
              bInvalidKey = true; break;
          }  
        } else {
          bInvalidKey = true;
        }

        if (bInvalidKey) {
          gError.SetError(DBA_ERR_INVCONFIGKEY, GetLineNumStr());
          bRC = false;
          break;
        } // end if
      }
    }

  } // end while

  if (!bRC && feof(m_hFile)) {
    bRC = true;
  } // end if

  if (bRC && pParameter) {
    // check version
    if (((pParameter->nInstanceType & m_nInstance) == m_nInstance ) &&
        (pParameter->oMinVersion                   <= m_oDBVersion) &&
        (pParameter->oMaxVersion                   >= m_oDBVersion)    ) {
      // check for duplicates
      if (GetParameter(pParameter->sID) != NULL) {
        gError.SetError(DBA_ERR_DUPLICATE, pParameter->sID, GetLineNumStr());
        delete pParameter;
        bRC = false;
      } else {
        if (pParameter->pTable != NULL) {
          pParameter->pTable->nInstanceType = pParameter->pTable->nInstanceType | pParameter->nInstanceType;
          pParameter->pTable->bStartup      = pParameter->pTable->bStartup     && pParameter->bStartup;
          pParameter->pTable->oMinVersion   = DBA_config_Version::MINVersion(pParameter->pTable->oMinVersion, pParameter->oMinVersion);;
          pParameter->pTable->oMaxVersion   = DBA_config_Version::MAXVersion(pParameter->pTable->oMaxVersion, pParameter->oMaxVersion);;
        } // end if
        if ((pParameter->oClasses.size() > 0) && pParameter->bProtocol) {
          for (nIndex = 0; nIndex < pParameter->oClasses.size(); ++nIndex) {
            pParameter->oClasses[nIndex]->nInstanceType = pParameter->oClasses[nIndex]->nInstanceType | pParameter->nInstanceType;
          } // end if
        } // end if
        if ((pParameter->oClasses.size() > 0) && pParameter->bProtocol) {
          for (nIndex = 0; nIndex < pParameter->oClasses.size(); ++nIndex) {
            pParameter->oClasses[nIndex]->lParameters.push_back(pParameter);
          } // end if
        } // end if
        m_lParameters.push_back(pParameter);
      } // end if
    } else {
      delete pParameter;
    } // end if
  } // end if

  return bRC;
} // DBA_configuration::ReadParameter

//
// ReadArray
//
// read all in a array section - [Array]
//
DBA_bool DBA_configuration::ReadArray()
{
  DBA_bool   bRC = false;
  DBA_string sLine;
  DBA_string sName;
  DBA_string sValue;
  fpos_t     nPos;
  DBA_int    nLine;

  DBA_config_array * pArray = NULL;
  
  fgetpos(m_hFile, &nPos);
  nLine = m_nLine;

  while (!feof(m_hFile)) {
    if (GetNextLine(sLine)) {
      if (IsSectionBegin(sLine)) {
        // reset file and return
        fsetpos(m_hFile, &nPos);
        m_nLine = nLine;
        bRC = true;
        break;
      } else {
        if (!pArray) {
          pArray = new DBA_config_array(m_oValueProvider);
          if (pArray == NULL) {
            gError.SetError(DBA_ERR_NOMEM);
            bRC = false;
            break;
          } // end if
        }

        SplitNameValue(sLine, sName, sValue);

        DBA_config_key nKey = DBA_CNFKEY_UNK;
        DBA_bool       bInvalidKey = false;

        if (GetConfigKey(sName, nKey)) {
          switch (nKey) {
            case DBA_CNFKEY_TAB: 
              if (!pArray->sFormula.Empty()) {
                gError.SetError(DBA_ERR_FORMTAB, pArray->sID, GetLineNumStr());
                return false;
              } else {
                pArray->sTable = sValue.ToUpper(); 
                AddTable(sValue,&(pArray->pTable));
              } // end if
              break;
            case DBA_CNFKEY_FOR: 
              if (!pArray->sTable.Empty()) {
                gError.SetError(DBA_ERR_FORMTAB, pArray->sID, GetLineNumStr());
                return false;
              } else {
                pArray->sFormula       = sValue; 
              } // end if
              break;
            case DBA_CNFKEY_ID:  pArray->sID            = sValue;                        break;
            case DBA_CNFKEY_EAC: pArray->sForEach       = sValue;                        break;
            case DBA_CNFKEY_LBL: pArray->sLabel         = sValue;                        break;
            case DBA_CNFKEY_COL: pArray->sColumn        = sValue;                        break;
            case DBA_CNFKEY_CLS: pArray->sClass         = sValue.ToUpper();              break;
            case DBA_CNFKEY_IDX: pArray->sIndex         = sValue;                        break;
            case DBA_CNFKEY_DEF: pArray->sDefault       = sValue;                        break;
            case DBA_CNFKEY_DES: pArray->sDescription   = sValue;                        break;
            case DBA_CNFKEY_CUR: pArray->bCurve         = (atoi(sValue.CharPtr()) != 0); break;
            case DBA_CNFKEY_UNI: pArray->sUnit          = sValue;                        break;
            case DBA_CNFKEY_MIV: pArray->oMinVersion    = sValue;                        break;
            case DBA_CNFKEY_MAV: pArray->oMaxVersion    = sValue;                        break;
            case DBA_CNFKEY_NUR: pArray->sNullRep       = sValue;                        break;
            case DBA_CNFKEY_SCA: pArray->sScaling       = sValue;                        break;
            case DBA_CNFKEY_ITY: pArray->nInstanceType  = GetInstanceType(sValue);       break;
            case DBA_CNFKEY_TYP: pArray->nType          = GetType(sValue);               break;
            case DBA_CNFKEY_PRE: pArray->nPrecision     = atoi(sValue.CharPtr());        break;
            case DBA_CNFKEY_STP: pArray->bStartup       = (atoi(sValue.CharPtr()) != 0); break;

            default: 
              bInvalidKey = true; break;
          } // end switch
        } else {
          bInvalidKey = true;
        } // end if

        if (bInvalidKey) {
          gError.SetError(DBA_ERR_INVCONFIGKEY, GetLineNumStr());
          bRC = false;
          break;
        } // end if
      } // end if
    } // end if

  } // end while

  if (!bRC && feof(m_hFile)) {
    bRC = true;
  }

  if (bRC && pArray) {
    // check version
    if (((pArray->nInstanceType & m_nInstance) == m_nInstance ) &&
        (pArray->oMinVersion                   <= m_oDBVersion) &&
        (pArray->oMaxVersion                   >= m_oDBVersion)    ) {
      // check for duplicates
      if (m_oValueProvider.AskForArray(pArray->sID) != NULL) {
        gError.SetError(DBA_ERR_DUPLICATE, pArray->sID, GetLineNumStr());
        delete pArray;
        bRC = false;
      } else {
        // parameter ready handling
        if (pArray->pTable != NULL) {
          pArray->pTable->nInstanceType = pArray->pTable->nInstanceType | pArray->nInstanceType;
          pArray->pTable->bStartup      = pArray->pTable->bStartup     && pArray->bStartup;
          pArray->pTable->oMinVersion   = DBA_config_Version::MINVersion(pArray->pTable->oMinVersion, pArray->oMinVersion);;
          pArray->pTable->oMaxVersion   = DBA_config_Version::MAXVersion(pArray->pTable->oMaxVersion, pArray->oMaxVersion);;
        } // end if
        m_lArrays.push_back(pArray);
      } // end if
    } else {
      delete pArray;
    } // end if
  } // end if

  return bRC;
} // end DBA_configuration::ReadArray

//
// ReadSettings
//
// read all in the setting section - [Settings]
//
DBA_bool DBA_configuration::ReadSettings()
{
  DBA_bool   bRC = false;
  DBA_string sLine;
  DBA_string sName;
  DBA_string sValue;
  fpos_t     nPos;
  DBA_int    nLine;
  
  fgetpos(m_hFile, &nPos);
  nLine = m_nLine;

  while (!feof(m_hFile)) {
    if (GetNextLine(sLine)) {
      if (IsSectionBegin(sLine)) {
        fsetpos(m_hFile, &nPos);
        m_nLine = nLine;
        bRC = true;
        break;
      } else {
        SplitNameValue(sLine, sName, sValue);
        sName.ToUpper();

        DBA_config_setting * pSetting = new DBA_config_setting;
        if (!pSetting) {
          gError.SetError(DBA_ERR_NOMEM);
          break;
        }

        bRC = true;

        pSetting->sName = sName.ToUpper();
        pSetting->sValue = sValue;

        m_lSettings.push_back(pSetting);
      }
    }
  }  

  if (!bRC && feof(m_hFile)) {
    bRC = true;
  }

  return bRC;
}

//
// ReadView
//
// read all in a view section - [View]
//
DBA_bool DBA_configuration::ReadView()
{
  DBA_bool   bRC = false;
  DBA_string sLine;
  DBA_string sName;
  DBA_string sValue;
  fpos_t     nPos;
  DBA_int    nLine;

  DBA_config_view * pView = NULL;
  
  fgetpos(m_hFile, &nPos);
  nLine = m_nLine;

  while (!feof(m_hFile)) {
    if (GetNextLine(sLine)) {
      if (IsSectionBegin(sLine)) {
        fsetpos(m_hFile, &nPos);
        m_nLine = nLine;
        bRC = true;
        break;
      } else {
        if (!pView) {
          pView = new DBA_config_view;
          if (!pView) {
            gError.SetError(DBA_ERR_NOMEM);
            bRC = false;
            break;
          }
        }

        SplitNameValue(sLine, sName, sValue);

        DBA_config_key nKey = DBA_CNFKEY_UNK;
        DBA_bool       bInvalidKey = false;

        if (GetConfigKey(sName, nKey)) {
          switch (nKey) {
            case DBA_CNFKEY_NAM: pView->sName       = sValue; break;
            case DBA_CNFKEY_QUE: pView->sQuery      = sValue; break;
            case DBA_CNFKEY_MIV: pView->oMinVersion = sValue; break;
            case DBA_CNFKEY_MAV: pView->oMaxVersion = sValue; break;
            case DBA_CNFKEY_ITY: 
              pView->nInstanceType  = GetInstanceType(sValue); 
              break;
            default: 
              bInvalidKey = true; break;
          }  
        } else {
          bInvalidKey = true;
        }

        if (bInvalidKey) {
          gError.SetError(DBA_ERR_INVCONFIGKEY, GetLineNumStr());
          bRC = false;
          break;
        }
      }
    }

  }  

  if (!bRC && feof(m_hFile)) {
    bRC = true;
  }

  if (bRC && pView) {
    // check version
    if (((pView->nInstanceType & m_nInstance) == m_nInstance ) &&
        (pView->oMinVersion                   <= m_oDBVersion) &&
        (pView->oMaxVersion                   >= m_oDBVersion)    ) {
      m_lViews.push_back(pView);
    } else {
      delete pView;
    } // end if
  } // end if

  return bRC;
} // end DBA_configuration::ReadView

//
// IsSectionBegin
//
// check the line if there a new section
// with [...]
//
DBA_bool DBA_configuration::IsSectionBegin(const DBA_string & sLine) 
{
  if (!sLine.Empty()) {
    if ((sLine[0] == DBA_CNFTXT_LSC) && 
        (sLine[sLine.Length()-1] == DBA_CNFTXT_RSC)) {
      return true;
    }
  }
  return false;
}

//
// SplitNameValue
//
// split the line in the name and value part
// at the equal sign
//
DBA_bool DBA_configuration::SplitNameValue(const DBA_string & sLine,
                                           DBA_string & sName,
                                           DBA_string & sValue)
{
  DBA_int n = sLine.FindFirstOf(DBA_CNFTXT_EQU);
  if (n >= 0) {
    sName = sLine.SubStr(0, n).Trim();
    sValue = sLine.SubStr(n+1).Trim();
  } else {
    sName = sLine;
    sValue = "";
  }
  return true;
}

//
// GetConfigKey
//
// check the name if it a valid
// keyword for the configuration file
//
DBA_bool DBA_configuration::GetConfigKey(const DBA_string & sName,
                                         DBA_config_key & nKey)
{
  DBA_string sKey = sName;
  sKey.ToUpper();

  nKey = DBA_CNFKEY_UNK;
  
  if (sKey == DBA_CNFTXT_ID) nKey = DBA_CNFKEY_ID;
  else if (sKey == DBA_CNFTXT_LBL) nKey = DBA_CNFKEY_LBL;
  else if (sKey == DBA_CNFTXT_CLS) nKey = DBA_CNFKEY_CLS;
  else if (sKey == DBA_CNFTXT_TAB) nKey = DBA_CNFKEY_TAB;
  else if (sKey == DBA_CNFTXT_COL) nKey = DBA_CNFKEY_COL;
  else if (sKey == DBA_CNFTXT_CND) nKey = DBA_CNFKEY_CND;
  else if (sKey == DBA_CNFTXT_DEF) nKey = DBA_CNFKEY_DEF;
  else if (sKey == DBA_CNFTXT_DES) nKey = DBA_CNFKEY_DES;
  else if (sKey == DBA_CNFTXT_CUR) nKey = DBA_CNFKEY_CUR;
  else if (sKey == DBA_CNFTXT_PRT) nKey = DBA_CNFKEY_PRT;
  else if (sKey == DBA_CNFTXT_FOR) nKey = DBA_CNFKEY_FOR;
  else if (sKey == DBA_CNFTXT_UNI) nKey = DBA_CNFKEY_UNI;
  else if (sKey == DBA_CNFTXT_PRE) nKey = DBA_CNFKEY_PRE;
  else if (sKey == DBA_CNFTXT_NUR) nKey = DBA_CNFKEY_NUR;
  else if (sKey == DBA_CNFTXT_SCA) nKey = DBA_CNFKEY_SCA;
  else if (sKey == DBA_CNFTXT_NAM) nKey = DBA_CNFKEY_NAM;
  else if (sKey == DBA_CNFTXT_QUE) nKey = DBA_CNFKEY_QUE;
  else if (sKey == DBA_CNFTXT_SK1) nKey = DBA_CNFKEY_SK1;
  else if (sKey == DBA_CNFTXT_SK2) nKey = DBA_CNFKEY_SK2;
  else if (sKey == DBA_CNFTXT_SK3) nKey = DBA_CNFKEY_SK3;
  else if (sKey == DBA_CNFTXT_INF) nKey = DBA_CNFKEY_INF;
  else if (sKey == DBA_CNFTXT_WA1) nKey = DBA_CNFKEY_WA1;
  else if (sKey == DBA_CNFTXT_WA2) nKey = DBA_CNFKEY_WA2;
  else if (sKey == DBA_CNFTXT_WA3) nKey = DBA_CNFKEY_WA3;
  else if (sKey == DBA_CNFTXT_ERR) nKey = DBA_CNFKEY_ERR;
  else if (sKey == DBA_CNFTXT_EAC) nKey = DBA_CNFKEY_EAC;
  else if (sKey == DBA_CNFTXT_UAC) nKey = DBA_CNFKEY_UAC;
  else if (sKey == DBA_CNFTXT_VAL) nKey = DBA_CNFKEY_VAL;
  else if (sKey == DBA_CNFTXT_IDX) nKey = DBA_CNFKEY_IDX;
  else if (sKey == DBA_CNFTXT_ITY) nKey = DBA_CNFKEY_ITY;
  else if (sKey == DBA_CNFTXT_TYP) nKey = DBA_CNFKEY_TYP;
  else if (sKey == DBA_CNFTXT_STP) nKey = DBA_CNFKEY_STP;
  else if (sKey == DBA_CNFTXT_IMP) nKey = DBA_CNFKEY_IMP;
  else if (sKey == DBA_CNFTXT_MIV) nKey = DBA_CNFKEY_MIV;
  else if (sKey == DBA_CNFTXT_MAV) nKey = DBA_CNFKEY_MAV;
  else if (sKey == DBA_CNFTXT_ADD) nKey = DBA_CNFKEY_ADD;

  return (nKey != DBA_CNFKEY_UNK);
}

//
// GetInstanceType
//
DBA_int DBA_configuration::GetInstanceType(const DBA_string & sName)
{
  DBA_string sType = sName;
  sType.ToUpper();

  DBA_int nType = 0;
  
  if (sType.Find(DBA_INSTANCETYPE_TXT_OLTP) != DBA_string::NPos) nType += DBA_INSTANCETYPE_OLTP;
  if (sType.Find(DBA_INSTANCETYPE_TXT_LVC ) != DBA_string::NPos) nType += DBA_INSTANCETYPE_LVC;

  return nType;
}

//
// GetType
//
Tools_ExpressionValue::ExType DBA_configuration::GetType(DBA_string sName)
{
  if      (sName.ToUpper() == DBA_CNFVAL_TNUM) return Tools_ExpressionValue::ExTypeNumeric;
  else if (sName.ToUpper() == DBA_CNFVAL_TSTR) return Tools_ExpressionValue::ExTypeString;
  return Tools_ExpressionValue::ExTypeUnknown;
} // DBA_configuration::GetType

//
// GetNextLine
//
// read the next line form the config file, 
// ignore empty and comment lines, 
// concatenate lines with the \-character at 
// the end with the next line
//
DBA_bool DBA_configuration::GetNextLine(DBA_string & sLine) 
{
  DBA_char sBuffer[DBA_CNF_MAXLINELEN];
  DBA_string sNext;

  memset(sBuffer, 0, DBA_CNF_MAXLINELEN);

  while (!feof(m_hFile)) {
    fgets(sBuffer ,DBA_CNF_MAXLINELEN, m_hFile);
    m_nLine++;
    sLine.ConvertFromASCII_Latin1(sBuffer, sBuffer + strlen(sBuffer));
    sLine.Trim();
    if (!sLine.Empty()) {
      if (sLine[0] != DBA_CNFTXT_COM) {
        if (sLine[sLine.Length()-1] == DBA_CNFTXT_NEL) {
          while (!feof(m_hFile)) {
            fgets(sBuffer ,DBA_CNF_MAXLINELEN, m_hFile);
            m_nLine++;
            sNext.ConvertFromASCII_Latin1(sBuffer, sBuffer + strlen(sBuffer));
            sNext.Trim();
            if (!sNext.Empty()) {
              sLine = sLine.SubStr(0, sLine.Length() - 1);
              sLine.Append(sNext);
              if (sNext[sNext.Length()-1] != DBA_CNFTXT_NEL) {
                break;
              }
            } else {
              break;
            }
          }
        }
        break;
      }
    }
  }

  return (!sLine.Empty());
}

//
// GetLineNumStr
//
DBA_string DBA_configuration::GetLineNumStr() 
{
  DBA_char sBuffer[32];

  sprintf(sBuffer, "%d", m_nLine);

  return DBA_string(sBuffer);
}

//
// AddTable
//
DBA_bool DBA_configuration::AddTable(const DBA_string & sName,
                                     DBA_config_table ** ppTable)
{
  *ppTable = NULL;

  DBA_config_table_list::iterator iter = m_lTables.begin();
  while (iter != m_lTables.end()) {
    if ((*iter)->sName == sName) {
      *ppTable = (*iter);
      return true;
    }
    ++iter;
  }
  
  DBA_config_table * pTable = new DBA_config_table;
  
  if (!pTable) {
    gError.SetError(DBA_ERR_NOMEM);
    return false;
  }

  pTable->sName = sName;
  pTable->pSnapshot = NULL;

  m_lTables.push_back(pTable);

  *ppTable = pTable;

  return true;
}

//
// AddClass
//
DBA_bool DBA_configuration::AddClass(const DBA_string & sName,
                                     DBA_config_class ** ppClass)
{
  *ppClass = NULL;

  DBA_config_class_list::iterator iter = m_lClasses.begin();
  while (iter != m_lClasses.end()) {
    if ((*iter)->sName == sName) {
      *ppClass = (*iter);
      return true;
    }
    ++iter;
  }
  
  DBA_config_class * pClass = new DBA_config_class;
  
  if (!pClass) {
    gError.SetError(DBA_ERR_NOMEM);
    return false;
  }

  pClass->sName = sName;
  pClass->nFileIndex = 0;

  m_lClasses.push_back(pClass);

  *ppClass = pClass;

  return true;
}

/*
  -----------------------------------------------------------------------------
  public function DBA_config_ValueProvider::AskForValue
  -----------------------------------------------------------------------------
 */
Tools_ExpressionValue * DBA_config_ValueProvider::AskForValue
      ( const Tools_DynamicUTF8String & szName)
{
  DBA_config_parameter_list::iterator iter = m_lParameters.begin();
  while (iter != m_lParameters.end()) {
    if ((*iter)->sID == szName) {
      return (*iter);
    } // end if
    ++iter;
  } // end while

  return NULL;
} // end DBA_config_ValueProvider::AskForValue

/*
  -----------------------------------------------------------------------------
  public function DBA_config_ValueProvider::AskForArray
  -----------------------------------------------------------------------------
 */
Tools_ExpressionValueArray * DBA_config_ValueProvider::AskForArray
      ( const Tools_DynamicUTF8String & szName)
{
  DBA_config_array_list::iterator iter = m_lArrays.begin();
  while (iter != m_lArrays.end()) {
    if ((*iter)->sID == szName) {
      return &((*iter)->oValues);
    } // end if
    ++iter;
  } // end while

  return NULL;
} // end DBA_config_ValueProvider::AskForArray
