/*!
  -----------------------------------------------------------------------------
  module: protocol.hpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: DBanalyzer - The performance analyse tool for SAPDB
  description:  protocol the data for the dbanalyzer
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

#ifndef _PROTOCOL_HPP_
#define _PROTOCOL_HPP_

#include <time.h>

#include "SAPDB/ToolsCommon/Tools_Array.hpp"
#include "SAPDB/ToolsCommon/Tools_Vector.hpp"
#include "SAPDB/DBanalyzer/types.hpp"

#define DBA_PRT_TYPENUMRIC          "P"
#define DBA_PRT_TYPESTRING          "C"
#define DBA_PRT_TYPEDATE            "D"
#define DBA_PRT_TYPETIME            "T"
#define DBA_PRT_TYPEIMPORTANT       "@"

#define DBA_PRT_TIMEHEADER          "COUNT;DATE;TIME;DURATION;DELTA;"
#define DBA_PRT_TIMELABELS          DBA_PRT_TIMEHEADER
#define DBA_PRT_TIMETYPES           DBA_PRT_TYPENUMRIC";"DBA_PRT_TYPEDATE";"DBA_PRT_TYPETIME";"DBA_PRT_TYPENUMRIC";"DBA_PRT_TYPENUMRIC";"
#define DBA_PRT_PARAMFILEPREFIX     "DBAN_"
#define DBA_PRT_PARAMFILEXTENSION   ".csv"
#define DBA_PRT_HISTORYFILE         "DBAN.prt"
#define DBA_PRT_SIDFILE             "DBAN.sid"
#define DBA_PRT_PIDFILE             "DBAN.pid"
#define DBA_PRT_INFFILE             "DBAN.inf"
#define DBA_PRT_ERRFILE             "DBAN.err"

typedef struct {
  DBA_string sClass;
  DBA_string sFile;
  DBA_string sHeader;
  DBA_string sLabels;
  DBA_string sTypes;
  DBA_string sValues;
} DBA_protocol_parameterfile;

typedef Tools_Array<DBA_protocol_parameterfile*> DBA_protocol_parameterfile_array;

class DBA_protocol {
  public:
  DBA_protocol (const DBA_string & sOutputDir,
                const DBA_uint     nConsoleLevel,
                const DBA_bool     bIgnore,
                const time_t       tNow);
  ~DBA_protocol ();

  DBA_bool AddParameterClass (const DBA_string & sClass, DBA_int & nIndex);
  DBA_void AddParameterHeader (const DBA_int nIndex, const DBA_string & sName, const DBA_string & sType, const DBA_string & sLabel);
  DBA_void ClearParameterValues (const DBA_int nIndex);
  DBA_void AddParameterValue (const DBA_int nIndex, const DBA_string & sValue);
  DBA_void SetParameterStatus (DBA_ulong nCount, time_t tSnapshot, time_t tLastSnapshot, time_t tNow);
  DBA_bool WriteParameters (const DBA_int nIndex);

  DBA_void SetHistoryStatus (DBA_ulong nCount, time_t tSnapshot);
  DBA_void ClearHistoryText();
  DBA_void SetHistoryOK();
  DBA_void SetHistoryNotConnected();
  DBA_void SetHistoryIgnore();
  DBA_void AddHistoryWarning (DBA_int nLevel, const DBA_string & sTitle);
  DBA_void AddHistoryCondition (const DBA_string & sCondition);
  DBA_void AddHistoryValue (const DBA_string & sValue);
  DBA_void AddHistoryAction (const DBA_string & sAction);
  DBA_void AddHistoryDescription (const DBA_string & sDescription);
  DBA_bool WriteHistory ( );
  DBA_void WriteError ( DBA_string & sClass, DBA_int nError, DBA_string & sError, DBA_string & m_sExtText);

  DBA_uint GetSid  ( );
  DBA_uint GetPid  ( );
  DBA_bool SetSid  ( DBA_uint nSid );
  DBA_bool SetPid  ( DBA_uint nPid );

  DBA_bool CheckAndSaveInfo ( DBA_string sNewServer,
                              DBA_string sNewDatabase,
                              DBA_string sNewRundir, 
                              DBA_string sNewConfig,
                              DBA_int    nIntervall,
                              DBA_bool   bReconnect,
                              DBA_uint   nSid,
                              DBA_uint   nPid,
                              DBA_string sDriver);
  void RemoveSessionData();

  DBA_bool DeleteFiles ( DBA_string sDate );
  DBA_bool CheckDir    ( DBA_string sDir  );

  DBA_string GetInfo();

  private:
  DBA_string GetActDir();

  DBA_bool CreateParameterFile(const DBA_int nIndex, DBA_string & sFilename);

  DBA_bool SetId  ( DBA_uint nPid, const char * szFile );
  DBA_uint GetId  ( const char * szFile );

  private:
  DBA_string m_sOutputDir;

  DBA_protocol_parameterfile_array m_aParameterFiles;
  DBA_int m_nParameterFiles;

  time_t m_tSnapshot;

  DBA_string m_sParameterStatus;
  
  DBA_string m_sHistoryStatus;
  DBA_string m_sHistoryFile;
  DBA_string m_sHistoryText;

  DBA_uint   m_nConsoleLevel;
  DBA_string m_sConsoleText;

  DBA_bool   m_bIgnore;

  DBA_bool   m_bInfoInitialized;

};

#endif