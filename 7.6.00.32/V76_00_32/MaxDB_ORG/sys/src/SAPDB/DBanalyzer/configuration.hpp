/*!
  -----------------------------------------------------------------------------
  module: configuration.hpp
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

#ifndef _CONFIGURATION_HPP_
#define _CONFIGURATION_HPP_

#include "SAPDB/DBanalyzer/types.hpp"
#include "SAPDB/ToolsCommon/Tools_Array.hpp"
#include "SAPDB/ToolsCommon/Tools_Vector.hpp"
#include "SAPDB/ToolsCommon/Tools_Expression.hpp"
#include "SAPDB/DBanalyzer/database.hpp"
#include "SAPDB/DBanalyzer/snapshot.hpp"
#include "SAPDB/DBanalyzer/protocol.hpp"

#define DBA_CNF_MAXLINELEN 4096

#define DBA_CNFTXT_LSC  '['
#define DBA_CNFTXT_RSC  ']'
#define DBA_CNFTXT_COM  '#'
#define DBA_CNFTXT_EQU  '='
#define DBA_CNFTXT_NEL  '\\'

// valid configuration file sections
#define DBA_CNFTXT_CON  "[CONSTANT]"
#define DBA_CNFTXT_ENV  "[ENVIRONMENT]"
#define DBA_CNFTXT_MON  "[MONITOR]"
#define DBA_CNFTXT_PAR  "[PARAMETER]"
#define DBA_CNFTXT_ARR  "[ARRAY]"
#define DBA_CNFTXT_SET  "[SETTINGS]"
#define DBA_CNFTXT_VIE  "[VIEW]"

// valid configuration keys
#define DBA_CNFTXT_CLS  "CLASS"
#define DBA_CNFTXT_CND  "CONDITION"
#define DBA_CNFTXT_COL  "COLUMN"
#define DBA_CNFTXT_CUR  "CURVE"
#define DBA_CNFTXT_DEF  "DEFAULT"
#define DBA_CNFTXT_DES  "DESCRIPTION"
#define DBA_CNFTXT_FOR  "FORMULA"
#define DBA_CNFTXT_ID   "ID"
#define DBA_CNFTXT_IDX  "INDEX"
#define DBA_CNFTXT_INF  "INFORMATION"
#define DBA_CNFTXT_ITY  "INSTANCETYPE"
#define DBA_CNFTXT_LBL  "LABEL"
#define DBA_CNFTXT_NAM  "NAME"
#define DBA_CNFTXT_NUR  "NULLREP"
#define DBA_CNFTXT_PRE  "PRECISION"
#define DBA_CNFTXT_PRT  "PROTOCOL"
#define DBA_CNFTXT_QUE  "QUERY"
#define DBA_CNFTXT_SCA  "SCALING"
#define DBA_CNFTXT_SK1  "SKIP1"
#define DBA_CNFTXT_SK2  "SKIP2"
#define DBA_CNFTXT_SK3  "SKIP3"
#define DBA_CNFTXT_TAB  "TABLE"
#define DBA_CNFTXT_TYP  "TYPE"
#define DBA_CNFTXT_UNI  "UNIT"
#define DBA_CNFTXT_UAC  "USERACTION"
#define DBA_CNFTXT_VAL  "VALUE"
#define DBA_CNFTXT_WA1  "WARNING1"
#define DBA_CNFTXT_WA2  "WARNING2"
#define DBA_CNFTXT_WA3  "WARNING3"
#define DBA_CNFTXT_ERR  "ERROR"
#define DBA_CNFTXT_EAC  "FOREACH"
#define DBA_CNFTXT_STP  "STARTUP"
#define DBA_CNFTXT_IMP  "IMPORTANT"
#define DBA_CNFTXT_MIV  "MINVERSION"
#define DBA_CNFTXT_MAV  "MAXVERSION"
#define DBA_CNFTXT_ADD  "PROTOCOLINFO"

#define DBA_CNFTXT_IER  "IGNOREERROR"


#define DBA_CNFVAL_TSTR "STRING"
#define DBA_CNFVAL_TNUM "NUMERIC"

#define DBA_CNFCHR_SEP  ","

// enumeration of valid keys
typedef enum {
  DBA_CNFKEY_UNK,
  DBA_CNFKEY_CLS,
  DBA_CNFKEY_CND,
  DBA_CNFKEY_COL,
  DBA_CNFKEY_LBL,
  DBA_CNFKEY_TAB,
  DBA_CNFKEY_DEF,
  DBA_CNFKEY_DES,
  DBA_CNFKEY_CUR,
  DBA_CNFKEY_PRT,
  DBA_CNFKEY_FOR,
  DBA_CNFKEY_ID ,
  DBA_CNFKEY_IDX,
  DBA_CNFKEY_INF,
  DBA_CNFKEY_ITY,
  DBA_CNFKEY_PRE,
  DBA_CNFKEY_NUR,
  DBA_CNFKEY_SCA,
  DBA_CNFKEY_NAM,
  DBA_CNFKEY_QUE,
  DBA_CNFKEY_SK1,
  DBA_CNFKEY_SK2,
  DBA_CNFKEY_SK3,
  DBA_CNFKEY_TYP,
  DBA_CNFKEY_WA1,
  DBA_CNFKEY_WA2,
  DBA_CNFKEY_WA3,
  DBA_CNFKEY_ERR,
  DBA_CNFKEY_EAC,
  DBA_CNFKEY_UAC,
  DBA_CNFKEY_UNI,
  DBA_CNFKEY_VAL,
  DBA_CNFKEY_STP,
  DBA_CNFKEY_IMP,
  DBA_CNFKEY_MIV,
  DBA_CNFKEY_MAV,
  DBA_CNFKEY_ADD
} DBA_config_key;

//
// forward declarations
//
class DBA_config_parameter;
class DBA_config_array;
class DBA_config_class;

typedef Tools_Vector<DBA_config_parameter*>   DBA_config_parameter_list;
typedef Tools_Vector<DBA_config_array*>       DBA_config_array_list;
typedef Tools_Array<DBA_config_class*>      DBA_config_class_array;

/*
----------------------------------------------------------------------------
Class:        DBA_config_ValueProvider
-----------------------------------------------------------------------------
*/
class DBA_config_ValueProvider : public Tools_ValueProvider {

  public:
    DBA_config_ValueProvider(DBA_config_parameter_list & lParameters,
                             DBA_config_array_list     & lArrays)
      : m_lParameters(lParameters),
        m_lArrays(lArrays) 
    {}

    Tools_ExpressionValue      * AskForValue(const Tools_DynamicUTF8String & szName);
    Tools_ExpressionValueArray * AskForArray(const Tools_DynamicUTF8String & szName);
                                        
  private:
    DBA_config_parameter_list & m_lParameters;
    DBA_config_array_list     & m_lArrays;

}; /* EndClass: DBA_config_ValueProvider */

/*
----------------------------------------------------------------------------
Class:        DBA_config_Version
-----------------------------------------------------------------------------
*/
class DBA_config_Version {
public:
  DBA_config_Version(DBA_string sVersion) 
  {
    *this = sVersion;
  } // DBA_config_Version

  DBA_config_Version & setVersion(DBA_int    nMajorI,
                                  DBA_int    nMinorI,
                                  DBA_int    nLevelI,
                                  DBA_int    nBuildI)
  { nMajor   = nMajorI;  
    nMinor   = nMinorI;  
    nLevel   = nLevelI;  
    nBuild   = nBuildI;
    computeVersion();
    return *this;
  }

  DBA_config_Version & operator = (DBA_string sVersion)
  { sscanf (sVersion.CharPtr(), "%d.%d.%d.%d", &nMajor, &nMinor, &nLevel, &nBuild);
    computeVersion();
    return *this;
  }

  DBA_config_Version & operator = (const DBA_config_Version& oVersion)
  { nMajor = oVersion.nMajor;
    nMinor = oVersion.nMinor;
    nLevel = oVersion.nLevel;
    nBuild = oVersion.nBuild;
    computeVersion();
    return *this;
  }

  bool operator == (const DBA_config_Version& oVersion) const
  { return (nVersion == oVersion.nVersion); }

  bool operator != (const DBA_config_Version& oVersion) const
  { return (nVersion != oVersion.nVersion); }

  bool operator >= (const DBA_config_Version& oVersion) const
  { return (nVersion >= oVersion.nVersion); }

  bool operator <= (const DBA_config_Version& oVersion) const
  { return (nVersion <= oVersion.nVersion); }

  bool operator >  (const DBA_config_Version& oVersion) const
  { return (nVersion >  oVersion.nVersion); }

  bool operator <  (const DBA_config_Version& oVersion) const
  { return (nVersion <  oVersion.nVersion); }

  static const DBA_config_Version & MAXVersion(const DBA_config_Version & a, const DBA_config_Version & b) 
  { return (a>b?a:b);}; 
  static const DBA_config_Version & MINVersion(const DBA_config_Version & a, const DBA_config_Version & b) 
  { return (a<b?a:b);}; 

  static const char * const MaxVersion;
  static const char * const MinVersion;

private:
  void computeVersion() 
  { nVersion = (nMajor == 0) ? 0 : nMajor * 1000000 + nMinor * 10000 + nLevel * 100 + nBuild; }

  DBA_int    nMajor;
  DBA_int    nMinor;
  DBA_int    nLevel;
  DBA_int    nBuild;
  DBA_int    nVersion;

}; /* EndClass: DBA_config_Version */

/*
----------------------------------------------------------------------------
Class:        DBA_config_setting
-----------------------------------------------------------------------------
description:  [SETTINGS] 
-----------------------------------------------------------------------------
*/
class DBA_config_setting {
public:
  DBA_string sName;
  DBA_string sValue;
}; /* EndClass: DBA_config_setting */

typedef Tools_Vector<DBA_config_setting*> DBA_config_setting_list;

/*
----------------------------------------------------------------------------
Class:        DBA_config_view
-----------------------------------------------------------------------------
description:  [VIEW] 
-----------------------------------------------------------------------------
*/
class DBA_config_view {
public:
  DBA_config_view() 
     : oMinVersion(DBA_config_Version::MinVersion),
       oMaxVersion(DBA_config_Version::MaxVersion)

  {
    nInstanceType = DBA_INSTANCETYPE_ALL;
  }
  DBA_string         sName;
  DBA_string         sQuery;
  DBA_int            nInstanceType;
  DBA_config_Version oMinVersion;
  DBA_config_Version oMaxVersion;
};  /* EndClass: DBA_config_view */

typedef Tools_Vector<DBA_config_view*> DBA_config_view_list;

/*
----------------------------------------------------------------------------
Class:        DBA_config_monitor
-----------------------------------------------------------------------------
description:  [MONITOR] 
-----------------------------------------------------------------------------
*/
class DBA_config_monitor {
public:

  typedef enum {
    SkipNone,
    Skip1,
    Skip2,
    Skip3
  } SkipValue;

  typedef enum {
    WrnNone,
    WrnInf,
    Wrn1,
    Wrn2,
    Wrn3,
    Err
  } WrnValue;

  // constructor
  DBA_config_monitor (DBA_config_ValueProvider & oProvider) 
     : oLabel(oProvider),
       oSkip1(oProvider),
       oSkip2(oProvider),
       oSkip3(oProvider),
       oWarning1(oProvider),
       oWarning2(oProvider),
       oWarning3(oProvider),
       oError(oProvider),
       oInformation(oProvider),
       nInstanceType(DBA_INSTANCETYPE_ALL),
       bStartup(false),
       oMinVersion(DBA_config_Version::MinVersion),
       oMaxVersion(DBA_config_Version::MaxVersion)
  { }

  DBA_string       sID;
  DBA_string       sForEach;
  DBA_string       sLabel;
  DBA_string       sClass;
  DBA_string       sSkip1;
  DBA_string       sSkip2;
  DBA_string       sSkip3;
  DBA_string       sInformation;
  DBA_string       sWarning1;
  DBA_string       sWarning2;
  DBA_string       sWarning3;
  DBA_string       sError;
  DBA_string       sUserAction;
  DBA_string       sDescription;
  DBA_int          nInstanceType;
  DBA_bool         bStartup;

  Tools_Expression oLabel;
  Tools_Expression oSkip1;
  Tools_Expression oSkip2;
  Tools_Expression oSkip3;
  Tools_Expression oWarning1;
  Tools_Expression oWarning2;
  Tools_Expression oWarning3;
  Tools_Expression oError;
  Tools_Expression oInformation;

  class monitor_value {
    public:
      monitor_value () 
        : nSkip(SkipNone),
          nWarning(WrnNone)
      {}

      DBA_string       sValLabel;
      DBA_string       sValSkip1;
      DBA_string       sValSkip2;
      DBA_string       sValSkip3;
      DBA_string       sValWarning1;
      DBA_string       sValWarning2;
      DBA_string       sValWarning3;
      DBA_string       sValError;
      DBA_string       sValInformation;
      DBA_string       sOrgExpression;
      DBA_string       sValExpression;
      SkipValue        nSkip;
      WrnValue         nWarning;
  };

  typedef Tools_Array<monitor_value> monitor_value_array;
  
  monitor_value_array oValues;
   
  DBA_config_Version oMinVersion;
  DBA_config_Version oMaxVersion;
}; /* EndClass: DBA_config_monitor */

typedef Tools_Vector<DBA_config_monitor*> DBA_config_monitor_list;

/*
----------------------------------------------------------------------------
Class:        DBA_config_table
-----------------------------------------------------------------------------
description:  table with snapshot and list of all used tables
-----------------------------------------------------------------------------
*/
class DBA_config_table {
public:
  DBA_config_table () 
     : oMinVersion(DBA_config_Version::MaxVersion),
       oMaxVersion(DBA_config_Version::MinVersion)
  {
    nInstanceType  = 0;
    bStartup       = true;
    pPreparedQuery = NULL;
  }
  ~DBA_config_table () 
  {
    if (pPreparedQuery != NULL) {
      delete pPreparedQuery;
    } // end if
  }

  DBA_string                  sName;
  DBA_snapshot              * pSnapshot;
  StudioOAL_WResult         * pPreparedQuery;
  DBA_int                     nInstanceType;

  DBA_bool                    bStartup;

  DBA_config_Version          oMinVersion;
  DBA_config_Version          oMaxVersion;

}; /* EndClass: DBA_config_table */

typedef Tools_Vector<DBA_config_table*> DBA_config_table_list;

/*
----------------------------------------------------------------------------
Class:        DBA_config_class
-----------------------------------------------------------------------------
description:  class
-----------------------------------------------------------------------------
*/
class DBA_config_class {
public:
  DBA_config_class () 
  {
    nInstanceType = 0;
  }
  DBA_string                sName;
  DBA_config_parameter_list lParameters;
  DBA_int                   nFileIndex;
  DBA_int                   nInstanceType;
}; /* EndClass: DBA_config_class */

typedef Tools_Vector<DBA_config_class*> DBA_config_class_list;

//
// type of parameter
//
typedef enum {
  DBA_CNFPTYPE_NONE,
  DBA_CNFPTYPE_CONSTANT,
  DBA_CNFPTYPE_ENVIRONMENT,
  DBA_CNFPTYPE_PARAMETER
} DBA_config_parametertype;

/*
----------------------------------------------------------------------------
Class:        DBA_config_parameter
-----------------------------------------------------------------------------
description:  [Parameter] also used for [CONSTANTS] and [ENVIRONMENT]
-----------------------------------------------------------------------------
*/
class DBA_config_parameter : public Tools_ExpressionValue {
public:
  // constructor
  DBA_config_parameter (DBA_config_parametertype nInType, DBA_config_ValueProvider & oProvider) 
     : oFormula(oProvider),
       oMinVersion(DBA_config_Version::MinVersion),
       oMaxVersion(DBA_config_Version::MaxVersion)
  {
    nType         = nInType;
    bProtocol     = false;
    bCurve        = false;
    bImportant    = false;
    pTable        = NULL;
    nInstanceType = DBA_INSTANCETYPE_ALL;
    SetNewValue(0, Tools_ExpressionValue::ExTypeNumeric); // currently we know only numeric parameter
    SetPrecision(0);
    bStartup      = false;
  } // end DBA_config_parameter

  DBA_config_parametertype       nType;

  DBA_string sID;
  DBA_string sLabel;
  DBA_string sClass;
  DBA_string sTable;
  DBA_string sColumn;
  DBA_string sConditionColumn;
  DBA_string sConditionValue;
  DBA_string sFormula;
  DBA_string sUnit;
  DBA_string sNullRep;
  DBA_string sScaling;
  DBA_bool   bProtocol;
  DBA_bool   bCurve;
  DBA_string sDefault;
  DBA_string sDescription;
  DBA_string sProtocolInfo;
  DBA_bool   bImportant;
  DBA_bool   bStartup;

  DBA_int nInstanceType;

  Tools_Expression oFormula;

  DBA_config_table       * pTable;
  DBA_config_class_array   oClasses;

  DBA_config_Version oMinVersion;
  DBA_config_Version oMaxVersion;
}; /* EndClass: DBA_config_parameter */

/*
----------------------------------------------------------------------------
Class:        DBA_config_array
-----------------------------------------------------------------------------
description:  [Array]
-----------------------------------------------------------------------------
*/
class DBA_config_array {
public:
  // constructor
  DBA_config_array (DBA_config_ValueProvider & oProvider) 
     : oFormula(oProvider),
       oMinVersion(DBA_config_Version::MinVersion),
       oMaxVersion(DBA_config_Version::MaxVersion)
  {
    bCurve        = false;
    pTable        = NULL;
    nInstanceType = DBA_INSTANCETYPE_ALL;
    nType         = Tools_ExpressionValue::ExTypeNumeric;
    nPrecision    = -1;
    bStartup      = false;

  } // end DBA_config_array

  DBA_string         sID;
  DBA_string         sLabel;
  DBA_string         sClass;
  DBA_string         sTable;
  DBA_string         sColumn;
  DBA_string         sIndex;
  DBA_string         sConditionColumn;
  DBA_string         sConditionValue;
  DBA_string         sUnit;
  DBA_string         sNullRep;
  DBA_string         sScaling;
  DBA_string         sDefault;
  DBA_string         sDescription;
  DBA_string         sFormula;
  DBA_string         sForEach;
  DBA_bool           bStartup;

  DBA_bool           bCurve;
  DBA_int            nInstanceType;
  DBA_config_table * pTable;

  Tools_Expression   oFormula;

  Tools_ExpressionValue::ExType nType;
  Tools_ExpressionValue::ExInt  nPrecision;
  Tools_ExpressionValueArray    oValues;

  DBA_config_Version oMinVersion;
  DBA_config_Version oMaxVersion;

}; /* EndClass: DBA_config_array */

/*
----------------------------------------------------------------------------
Class:        DBA_configuration
-----------------------------------------------------------------------------
*/
class DBA_configuration {
  public:
  DBA_configuration  ();
  ~DBA_configuration ();

  DBA_bool Open ( const DBA_string & sFilename,
                  DBA_int            nMajor,
                  DBA_int            nMinor,
                  DBA_int            nLevel,
                  DBA_int            nBuild,
                  DBA_int            nInstance );

  DBA_bool SetParameterValue(const DBA_string & sName, 
                             const DBA_string & sValue);

  DBA_string GetConfigFile( ) {return m_sFilename;} 

  DBA_string GetParameterValue(const DBA_string & sName);

  DBA_config_parameter * GetParameter(const DBA_string & sName);
  DBA_config_monitor   * GetMonitor  (const DBA_string & sName);

  DBA_string GetSetting(const DBA_string & sName);

  DBA_bool InitViews(DBA_database * pDatabase);
  DBA_bool InitEnvironment(DBA_database * pDatabase);
  DBA_bool InitProtocolFiles(DBA_protocol * pProtocol);
  DBA_bool GetSnapshot(DBA_database * pDatabase,
                       time_t       & tSnapshot,
                       DBA_bool     & bStartup);

  DBA_bool CalculateParameters (const DBA_bool bStartup);
  DBA_bool CalculateArrays     (const DBA_bool bStartup);
  DBA_bool CalculateMonitors   (const DBA_bool bStartup);
  
  DBA_bool ProtocolParameters(DBA_protocol * pProtocol,
                              DBA_ulong      nCount,
                              time_t         tAct,
                              time_t         tLast,
                              time_t         tNow);
  DBA_bool ParseExpressions();
  DBA_bool ProtocolHistory(DBA_protocol * pProtocol,
                           DBA_ulong      nCount,
                           time_t         tAct,
                           DBA_bool       bStartup);

  private:
  DBA_string GetEnvPath();

  DBA_bool Read();
  DBA_bool ReadConstants();
  DBA_bool ReadEnvironment();
  DBA_bool ReadMonitor();
  DBA_bool ReadParameter();
  DBA_bool ReadArray();
  DBA_bool ReadSettings();
  DBA_bool ReadView();
 
  DBA_bool IsSectionBegin(const DBA_string & sLine);
  DBA_bool SplitNameValue(const DBA_string & sLine,
                          DBA_string & sName,
                          DBA_string & sValue);
  DBA_bool GetConfigKey(const DBA_string & sName,
                        DBA_config_key & nKey);
  DBA_int GetInstanceType(const DBA_string & sName);
  DBA_bool GetNextLine(DBA_string & sLine);
  DBA_string GetLineNumStr();

  DBA_bool AddTable(const DBA_string &  sName,
                    DBA_config_table ** ppTable);

  DBA_bool AddClass(const DBA_string & sName,
                    DBA_config_class ** ppClass);
  DBA_bool CalculateMonitorExpression( DBA_string                                         & sMonitor,
                                       DBA_string                                           sErrorText,
                                       Tools_Expression                                   & oFormula, 
                                       DBA_string                                         & sValue,
                                       DBA_config_monitor::monitor_value_array::size_type   nIndex);
  Tools_ExpressionValue::ExType GetType(DBA_string sName);

  private:
  DBA_string m_sFilename;
  DBA_file   m_hFile;
  DBA_int    m_nLine;

  DBA_config_setting_list     m_lSettings;
  DBA_config_view_list        m_lViews;
  DBA_config_parameter_list   m_lParameters;
  DBA_config_array_list       m_lArrays;
  DBA_config_monitor_list     m_lMonitors;

  DBA_config_table_list       m_lTables;
  DBA_config_class_list       m_lClasses;

  DBA_config_ValueProvider    m_oValueProvider;

  DBA_config_Version          m_oDBVersion;
  DBA_int                     m_nInstance;

  DBA_string                  m_sDate;
}; /* EndClass: DBA_configuration */

#endif
