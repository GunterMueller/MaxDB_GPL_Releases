/*!
  -----------------------------------------------------------------------------
  module: protocol.cpp
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

#include <time.h>
#include <stdlib.h>
#include <ctype.h>

#include "SAPDB/ToolsCommon/Tools_System.hpp"

#include "SAPDB/DBanalyzer/protocol.hpp"
#include "SAPDB/DBanalyzer/error.hpp"

extern DBA_error gError;

DBA_protocol::DBA_protocol (const DBA_string & sOutputDir,
                            const DBA_uint     nConsoleLevel,
                            const DBA_bool     bIgnore,
                            const time_t       tNow) 
             : m_sOutputDir(sOutputDir),
               m_nConsoleLevel(nConsoleLevel),
               m_bIgnore(bIgnore),
               m_bInfoInitialized(false)
{
  m_nParameterFiles = 0;
  
  m_tSnapshot = tNow;

  m_sParameterStatus = "";

  m_sHistoryFile = DBA_PRT_HISTORYFILE;
  m_sHistoryStatus = "";
  m_sHistoryText = "";

  m_sConsoleText  = "";

  if (m_bIgnore) {
    DBA_string sAct = GetActDir();
    DBA_string sDir = ToolsSys_joinpath(m_sOutputDir.CharPtr(), sAct.CharPtr());
    DBA_string sFilename = ToolsSys_joinpath(sDir.CharPtr(),m_sHistoryFile.CharPtr());
    remove(sFilename.CharPtr());
  } // end if

}

DBA_protocol::~DBA_protocol () 
{
}

//
//
//



//
// AddParameterClass
//
DBA_bool DBA_protocol::AddParameterClass (const DBA_string & sClass, DBA_int & nIndex)
{
  DBA_protocol_parameterfile * pFile = new DBA_protocol_parameterfile;
  if (!pFile) {
    gError.SetError(DBA_ERR_NOMEM);
    return false;
  } 

  pFile->sClass = sClass;
  pFile->sClass.ToUpper(); 

  pFile->sFile = DBA_PRT_PARAMFILEPREFIX;
  pFile->sFile.Append(pFile->sClass);
  pFile->sFile.Append(DBA_PRT_PARAMFILEXTENSION);

  pFile->sHeader = "";
  pFile->sTypes  = "";
  pFile->sLabels = "";
  
  m_aParameterFiles[m_nParameterFiles] = pFile;
  m_nParameterFiles++;

  nIndex = m_nParameterFiles;

  if (m_bIgnore) {
    DBA_string sAct = GetActDir();
    DBA_string sDir = ToolsSys_joinpath(m_sOutputDir.CharPtr(), sAct.CharPtr());
    DBA_string sFilename = ToolsSys_joinpath(sDir.CharPtr(), m_aParameterFiles[nIndex-1]->sFile.CharPtr());
    remove(sFilename.CharPtr());
  } // end if

  return true;
}

//
// AddParameterHeader
//
DBA_void DBA_protocol::AddParameterHeader 
      ( const DBA_int      nIndex, 
        const DBA_string & sName,
        const DBA_string & sType,
        const DBA_string & sLabel)
{
  if(!m_aParameterFiles[nIndex-1]->sHeader.Empty()) {
    m_aParameterFiles[nIndex-1]->sHeader.Append(";");
  }
  m_aParameterFiles[nIndex-1]->sHeader.Append(sName);

  if(!m_aParameterFiles[nIndex-1]->sTypes.Empty()) {
    m_aParameterFiles[nIndex-1]->sTypes.Append(";");
  }
  m_aParameterFiles[nIndex-1]->sTypes.Append(sType);

  if(!m_aParameterFiles[nIndex-1]->sLabels.Empty()) {
    m_aParameterFiles[nIndex-1]->sLabels.Append(";");
  }
  m_aParameterFiles[nIndex-1]->sLabels.Append(sLabel);
}

//
// ClearParameterValues
//
DBA_void DBA_protocol::ClearParameterValues (const DBA_int nIndex)
{
  m_aParameterFiles[nIndex-1]->sValues = "";
}

//
// AddParameterValue
//
DBA_void DBA_protocol::AddParameterValue (const DBA_int nIndex, const DBA_string & sValue)
{
  if(!m_aParameterFiles[nIndex-1]->sValues.Empty()) {
    m_aParameterFiles[nIndex-1]->sValues.Append(";");
  }
  m_aParameterFiles[nIndex-1]->sValues.Append(sValue);
}

//
// SetParameterStatus
//
DBA_void DBA_protocol::SetParameterStatus(DBA_ulong nCount, time_t tSnapshot, time_t tLastSnapshot, time_t tNow)
{
  m_tSnapshot  = tSnapshot;

  struct tm *tmSnapshot;
  tmSnapshot = localtime( &tSnapshot );
  
  DBA_char szBuffer[1024];

  sprintf(szBuffer,
          "%d;%04d%02d%02d;%02d%02d%02d;%d;%d;", 
          nCount,                        // count
          tmSnapshot->tm_year+1900,      // year
          tmSnapshot->tm_mon+1,          // month
          tmSnapshot->tm_mday,           // day
          tmSnapshot->tm_hour,           // hour
          tmSnapshot->tm_min,            // minute
          tmSnapshot->tm_sec,            // second
          tNow - tSnapshot,              // duration  
          tSnapshot - tLastSnapshot      // delta
         );   

  m_sParameterStatus = DBA_string(szBuffer);
}

//
// WriteParameters
//
DBA_bool DBA_protocol::WriteParameters (const DBA_int nIndex)
{
  DBA_string sActDir = GetActDir();
  DBA_string sDir = ToolsSys_joinpath(m_sOutputDir.CharPtr(), sActDir.CharPtr());

  if (!ToolsSys_createdir(sDir.CharPtr())) {
    gError.SetError(DBA_ERR_CREATEDIR, sDir);
    return false;
  }
  
  DBA_string sFilename = ToolsSys_joinpath(sDir.CharPtr(), m_aParameterFiles[nIndex-1]->sFile.CharPtr());

  if (!ToolsSys_fileexists(sFilename.CharPtr())) {
    if (!CreateParameterFile(nIndex, sFilename)) {
      return false;
    }
  }

  DBA_file hFile;

  hFile = fopen(sFilename.CharPtr(), "a");
  if (!hFile) {
    gError.SetError(DBA_ERR_OPENFILE, sFilename); 
    return false;
  }

  fputs(m_sParameterStatus.CharPtr(), hFile);
  fputs(m_aParameterFiles[nIndex-1]->sValues.CharPtr(), hFile);
  fputs("\n", hFile);

  fclose(hFile);

  return true;
}

//
// ClearHistoryText
//
DBA_void DBA_protocol::ClearHistoryText()
{
  m_sHistoryText = "";
  m_sConsoleText = "";
}

//
// SetHistoryStatus
//
DBA_void DBA_protocol::SetHistoryStatus(DBA_ulong nCount, time_t tSnapshot)
{
  m_tSnapshot  = tSnapshot;

  struct tm *tmSnapshot;
  tmSnapshot = localtime( &tSnapshot );

  DBA_char szBuffer[1024];

  sprintf(szBuffer,
  "===== #%-10d at %04d-%02d-%02d %02d:%02d:%02d\n", 
           nCount,                        // count
           tmSnapshot->tm_year+1900,      // year
           tmSnapshot->tm_mon+1,          // month
           tmSnapshot->tm_mday,           // day
           tmSnapshot->tm_hour,           // hour
           tmSnapshot->tm_min,            // minute
           tmSnapshot->tm_sec             // second
          );   

  m_sHistoryStatus = DBA_string(szBuffer);
}

//
// SetHistoryOK
//
DBA_void DBA_protocol::SetHistoryOK()
{
  m_sHistoryText = "* OK\n";

  if (m_nConsoleLevel != 0) {
    m_sConsoleText = m_sHistoryText;
  }
}


//
// SetHistoryNotConnected
//
DBA_void DBA_protocol::SetHistoryNotConnected()
{
  m_sHistoryText = "* !!  Not Connected\n";

  if (m_nConsoleLevel > 0) {
    m_sConsoleText = m_sHistoryText;
  }
}

//
// SetHistoryIgnore
//
DBA_void DBA_protocol::SetHistoryIgnore()
{
  m_sHistoryText = "* !!  Ignorable SQL error\n";

  if (m_nConsoleLevel > 0) {
    m_sConsoleText = m_sHistoryText;
  }
}

//
// AddHistoryWarning 
//
DBA_void DBA_protocol::AddHistoryWarning (DBA_int nLevel, 
                                          const DBA_string & sTitle)
{
  DBA_string sHistory;

  switch (nLevel) {
    case 0:   sHistory.Append("*  I  "); break;
    case 1:   sHistory.Append("* W1  "); break;
    case 2:   sHistory.Append("* W2  "); break;
    case 3:   sHistory.Append("* W3  "); break;
    case 4:   sHistory.Append("*  E  "); break;
    default:  sHistory.Append("*     "); break;
  }

  sHistory.Append(sTitle);
  sHistory.Append("\n");

  m_sHistoryText.Append(sHistory);
  if (m_nConsoleLevel > 0) {
    m_sConsoleText.Append(sHistory);
  }
}

//
// AddHistoryCondition
//
DBA_void DBA_protocol::AddHistoryCondition (const DBA_string & sCondition)
{
  m_sHistoryText.Append("      CON: ");
  m_sHistoryText.Append(sCondition);
  m_sHistoryText.Append("\n");

  if (m_nConsoleLevel > 1) {
    m_sConsoleText.Append("      CON: ");
    m_sConsoleText.Append(sCondition);
    m_sConsoleText.Append("\n");
  }
}

//
// AddHistoryValue
//
DBA_void DBA_protocol::AddHistoryValue (const DBA_string & sValue)
{
  m_sHistoryText.Append("      VAL: ");
  m_sHistoryText.Append(sValue);
  m_sHistoryText.Append("\n");

  if (m_nConsoleLevel > 1) {
    m_sConsoleText.Append("      VAL: ");
    m_sConsoleText.Append(sValue);
    m_sConsoleText.Append("\n");
  }
}

//
// AddHistoryAction
//
DBA_void DBA_protocol::AddHistoryAction (const DBA_string & sAction)
{
  m_sHistoryText.Append("      ACT: ");
  m_sHistoryText.Append(sAction);
  m_sHistoryText.Append("\n");

  if (m_nConsoleLevel > 2) {
    m_sConsoleText.Append("      ACT: ");
    m_sConsoleText.Append(sAction);
    m_sConsoleText.Append("\n");
  }
}

//
// AddHistoryDescription
//
DBA_void DBA_protocol::AddHistoryDescription (const DBA_string & sDescription)
{
  m_sHistoryText.Append("      DES: ");
  m_sHistoryText.Append(sDescription);
  m_sHistoryText.Append("\n");

  if (m_nConsoleLevel > 3) {
    m_sConsoleText.Append("      DES: ");
    m_sConsoleText.Append(sDescription);
    m_sConsoleText.Append("\n");
  }
}

//
// WriteHistory
//
DBA_bool DBA_protocol::WriteHistory ( )
{
  DBA_string sActDir = GetActDir();
  DBA_string sDir = ToolsSys_joinpath(m_sOutputDir.CharPtr(), sActDir.CharPtr());

  if (!ToolsSys_createdir(sDir.CharPtr())) {
    gError.SetError(DBA_ERR_CREATEDIR, sDir);
    return false;
  }
  
  DBA_string sFilename = ToolsSys_joinpath(sDir.CharPtr(), m_sHistoryFile.CharPtr());

  DBA_file hFile;

  hFile = fopen(sFilename.CharPtr(), "a");
  if (!hFile) {
    gError.SetError(DBA_ERR_OPENFILE, sFilename); 
    return false;
  }

  fputs(m_sHistoryStatus.CharPtr(), hFile);
  fputs(m_sHistoryText.CharPtr(), hFile);
  fputs("\n", hFile);

  fclose(hFile);

  // output text to the console
  if (m_nConsoleLevel > 0) {
    printf("%s%s\n", m_sHistoryStatus.CharPtr(), m_sConsoleText.CharPtr());
  } // end if

  return true;
}

//
// DBA_protocol::WriteError
//
DBA_void DBA_protocol::WriteError 
  ( DBA_string & sClass, 
    DBA_int      nError, 
    DBA_string & sError, 
    DBA_string & sExtText)
{
  DBA_file hFile;
  DBA_string sFile = DBA_PRT_ERRFILE;
  sFile = ToolsSys_joinpath(m_sOutputDir.CharPtr(), sFile.CharPtr());

  hFile = fopen(sFile.CharPtr(), "a");

  printf("%s %d: %s\n", sClass.CharPtr(), nError, sError.CharPtr());
  if (hFile != NULL) {
    time_t      tAct;
    struct tm * tmAct;
  
    tAct  = time(&tAct);
    tmAct = localtime( &tAct );
  

    fprintf(hFile, "%04d-%02d-%02d %02d:%02d:%02d\n", 
            tmAct->tm_year+1900,      // year
            tmAct->tm_mon+1,          // month
            tmAct->tm_mday,           // day
            tmAct->tm_hour,           // hour
            tmAct->tm_min,            // minute
            tmAct->tm_sec);           // second

    fprintf(hFile, "%s %d: %s\n", sClass.CharPtr(), nError, sError.CharPtr());
  } // end if
  if (!sExtText.Empty()) {
    printf("%s\n\n", sExtText.CharPtr());
    if (hFile != NULL) {
      fprintf(hFile, "%s\n\n", sExtText.CharPtr());
    } // end if
  } else {
    printf("\n");
    if (hFile != NULL) {
      fprintf(hFile, "\n");
    } // end if
  }  // end if

  if (hFile != NULL) {
    fclose(hFile);
  } // end if

  if (m_bInfoInitialized) {
    hFile = fopen(ToolsSys_joinpath(m_sOutputDir.CharPtr(), DBA_PRT_INFFILE), "a");
    if (hFile != NULL) {
      fprintf(hFile, "Error             = %s %d: %s\n", sClass.CharPtr(), nError, sError.CharPtr());
      if (!sExtText.Empty()) {
        fprintf(hFile, "                    %s\n", sExtText.CharPtr());
      }  // end if
      fclose(hFile);
    } // end if
  } // end if

} // end DBA_protocol::WriteError

//
// GetTodaysDir
//
DBA_string DBA_protocol::GetActDir() 
{
  DBA_char szDir[10];
  
  struct tm *tmAct;
  tmAct = localtime( &m_tSnapshot );

  sprintf(szDir,"%04d%02d%02d", tmAct->tm_year+1900, tmAct->tm_mon+1, tmAct->tm_mday);   

  return DBA_string(szDir);
}

//
// CreateParameterFile
//    
DBA_bool DBA_protocol::CreateParameterFile(const DBA_int nIndex, DBA_string & sFilename)
{
  DBA_file hFile;

  hFile = fopen(sFilename.CharPtr(), "w");
  if (hFile) {
    // parameter names
    fputs(DBA_PRT_TIMEHEADER, hFile);
    fputs(m_aParameterFiles[nIndex-1]->sHeader.CharPtr(), hFile);
    fputs("\n", hFile);
    
    // parameter labels
    fputs(DBA_PRT_TIMELABELS, hFile);
    fputs(m_aParameterFiles[nIndex-1]->sLabels.CharPtr(), hFile);
    fputs("\n", hFile);

    // parameter types
    fputs(DBA_PRT_TIMETYPES, hFile);
    fputs(m_aParameterFiles[nIndex-1]->sTypes.CharPtr(), hFile);
    fputs("\n", hFile);

    fclose(hFile);
    return true;
  } else {
    gError.SetError(DBA_ERR_OPENFILE, sFilename); 
    return false;
  }
}

//
// DBA_protocol::GetSid
//
DBA_uint DBA_protocol::GetSid( )
{
  return GetId(DBA_PRT_SIDFILE);
} // end DBA_protocol::GetSid

//
// DBA_protocol::GetPid
//
DBA_uint DBA_protocol::GetPid( )
{
  return GetId(DBA_PRT_PIDFILE);
} // end DBA_protocol::GetPid

//
// DBA_protocol::GetId
//
DBA_uint DBA_protocol::GetId(const char * szFile)
{
  DBA_uint nId = 0;
  
  // read Sid file
  DBA_file hFile;
  DBA_string sFile = szFile;
  sFile = ToolsSys_joinpath(m_sOutputDir.CharPtr(), sFile.CharPtr());
  hFile = fopen(sFile.CharPtr(), "r");
  if (hFile != NULL) {
    char pId[51];
    if (fgets(pId, 50, hFile) != NULL) {
      sscanf(pId, "%u", &nId);
    } // end if
    fclose(hFile);
  } // end if

  return nId;
} // DBA_protocol::GetId

//
// SetSid
//
DBA_bool DBA_protocol::SetSid(DBA_uint nSid)
{
  return SetId(nSid, DBA_PRT_SIDFILE);
} // DBA_protocol::SetSid

//
// SetPid
//
DBA_bool DBA_protocol::SetPid(DBA_uint nPid)
{
  return SetId(nPid, DBA_PRT_PIDFILE);
} // DBA_protocol::SetPid

//
// SetId
//
DBA_bool DBA_protocol::SetId(DBA_uint nId, const char * szFile)
{
  // write Sid file
  DBA_file hFile;
  DBA_string sFile = szFile;
  sFile = ToolsSys_joinpath(m_sOutputDir.CharPtr(), sFile.CharPtr());
  hFile = fopen(sFile.CharPtr(), "w");

  if (hFile != NULL) {
    fprintf(hFile, "%u", nId);
    fclose(hFile);
    return true;
  } else {
    gError.SetError(DBA_ERR_OPENFILE, sFile); 
  } // end if

  return false;
} // DBA_protocol::SetId

//
// CheckAndSaveInfo
//
DBA_bool DBA_protocol::CheckAndSaveInfo
      ( DBA_string  sNewServer,
        DBA_string  sNewDatabase,
        DBA_string  sNewRundir, 
        DBA_string  sNewConfig,
        DBA_int     nInterval,
        DBA_bool    bReconnect,
        DBA_uint    nSid,
        DBA_uint    nPid,
        DBA_string  sDriver)
{

  DBA_file hFile;
  DBA_string sInfFile = DBA_PRT_INFFILE;
  sInfFile = ToolsSys_joinpath(m_sOutputDir.CharPtr(), sInfFile.CharPtr());

  DBA_string sCheckDatabase;

  if (sNewServer.Length() > 0) {
    sCheckDatabase.Assign(sNewDatabase).Append("@").Append(sNewServer);
  } else {
    sCheckDatabase = sNewDatabase;
  } // end if

  if (!m_bIgnore) {
    // read inf file
    hFile = fopen(sInfFile.CharPtr(), "r");
    if (hFile != NULL) {
      char aLine[1001];
      if (fgets(aLine, 1000, hFile) != NULL) {
        DBA_string sDatabase = aLine;
        sDatabase.Trim();
        if (sDatabase != sCheckDatabase) {
          fclose(hFile);
          gError.SetError(DBA_ERR_OTHERDB, sDatabase, m_sOutputDir); 
          return false;        
        } // end if
      } // end if
      fclose(hFile);
    } // end if
  } // end if

  // write inf file
  hFile = fopen(sInfFile.CharPtr(), "w");
  if (hFile != NULL) {
    time_t      tAct;
    struct tm * tmAct;
  
    tAct  = time(&tAct);
    tmAct = localtime( &tAct );

    fprintf(hFile, 
"%s\n\
Database          = %s\n\
Node              = %s\n\
Rundirectory      = %s\n\
Configfile        = %s\n\
Protocoldirectory = %s\n\
Interval          = %d\n\
Reconnect         = %s\n\
ProcessID         = %u\n\
SessionID         = %u\n\
Driver            = %s\n\
Started           = %04d-%02d-%02d %02d:%02d:%02d\n", 
            sCheckDatabase.CharPtr(), 
            sNewDatabase.CharPtr(),
            sNewServer.CharPtr(),
            sNewRundir.CharPtr(), 
            sNewConfig.CharPtr(),
            m_sOutputDir.CharPtr(),
            (int) nInterval,
            bReconnect ? "true" : "false",
            (unsigned int) nPid,
            (unsigned int) nSid,
            sDriver.CharPtr(), 
            tmAct->tm_year+1900,      // year
            tmAct->tm_mon+1,          // month
            tmAct->tm_mday,           // day
            tmAct->tm_hour,           // hour
            tmAct->tm_min,            // minute
            tmAct->tm_sec);           // second

    m_bInfoInitialized = true;
    fclose(hFile);
  } else {
    gError.SetError(DBA_ERR_OPENFILE, sInfFile); 
    return false;
  } // end if

  if (!SetSid(nSid)) {
    return false;
  } // end if
  if (!SetPid(nPid)) {
    return false;
  } // end if

  return true;
} // DBA_protocol::CheckAndSaveInfo

//
// DBA_protocol::DeleteFiles
//
DBA_bool DBA_protocol::DeleteFiles
      (DBA_string sDate)
{
  int          nIndex = 0;
  DBA_string   sFile;
  void       * pDir;
  char         sDir[1024];

  // check sDate string
  if (!CheckDir(sDate)) {
    gError.SetError(DBA_ERR_ARGS); 
    gError.SetExtText("Invalid date!");
    return false;
  } // end if

  // iterate through directories

  if (ToolsSys_opendir(pDir, m_sOutputDir.CharPtr(), sDir, 1024)) {
    do {
      if (CheckDir(sDir) && strcmp(sDate.CharPtr(), sDir) >= 0) {
        // remove files
        for (nIndex = 0; nIndex < m_nParameterFiles; ++nIndex) {
          sFile = ToolsSys_joinpath(ToolsSys_joinpath(m_sOutputDir.CharPtr(), sDir), m_aParameterFiles[nIndex]->sFile.CharPtr());
          remove(sFile.CharPtr());
        } // end for

        sFile = ToolsSys_joinpath(ToolsSys_joinpath(m_sOutputDir.CharPtr(), sDir), DBA_PRT_HISTORYFILE);
        remove(sFile.CharPtr());
        
        // remove dir
        ToolsSys_removeemptydir(ToolsSys_joinpath(m_sOutputDir.CharPtr(), sDir));

      } // end if
    } while (ToolsSys_readdir(pDir, sDir, 1024));

    ToolsSys_closedir(pDir);
  } else {
    gError.SetError(DBA_ERR_OPENFILE, m_sOutputDir); 
    return false;
  } // end if

  return true;
} // end DBA_protocol::DeleteFiles

//
// DBA_protocol::CheckDir
//
DBA_bool DBA_protocol::CheckDir
      (DBA_string sDir)
{
  // check sDir string
  if (sDir.Length() != 8) {
    return false;
  } // end if
  for (unsigned int nIndex = 0; nIndex < sDir.Length(); ++nIndex) {
    if (!isdigit(sDir[nIndex])) {
      return false;
    } // end if
  } // end if
  return true;
} // DBA_protocol::CheckDir

//
// DBA_protocol::RemoveSessionData
//
void DBA_protocol::RemoveSessionData
      ( )
{
  remove(ToolsSys_joinpath(m_sOutputDir.CharPtr(), DBA_PRT_SIDFILE));
  remove(ToolsSys_joinpath(m_sOutputDir.CharPtr(), DBA_PRT_PIDFILE));

  FILE * hFile;

  hFile = fopen(ToolsSys_joinpath(m_sOutputDir.CharPtr(), DBA_PRT_INFFILE), "a");
  if (hFile != NULL) {
    time_t      tAct;
    struct tm * tmAct;
  
    tAct  = time(&tAct);
    tmAct = localtime( &tAct );

    fprintf(hFile, "Stopped           = %04d-%02d-%02d %02d:%02d:%02d\n", 
            tmAct->tm_year+1900,      // year
            tmAct->tm_mon+1,          // month
            tmAct->tm_mday,           // day
            tmAct->tm_hour,           // hour
            tmAct->tm_min,            // minute
            tmAct->tm_sec);           // second
    fclose(hFile);
  } // end if

} // DBA_protocol::RemoveSessionData

//
// DBA_protocol::GetInfo
//
DBA_string DBA_protocol::GetInfo
      ( )
{

  DBA_file hFile;
  DBA_string sInfFile = DBA_PRT_INFFILE;
  DBA_string sInfo;
  DBA_bool   bFirst = true;
  sInfFile = ToolsSys_joinpath(m_sOutputDir.CharPtr(), sInfFile.CharPtr());


  hFile = fopen(sInfFile.CharPtr(), "r");
  if (hFile != NULL) {
    char aLine[1001];
    while (fgets(aLine, 1000, hFile) != NULL) {
      if (!bFirst) {
        sInfo.Append(aLine);
      } // end if
      bFirst = false;
    } // end while
    fclose(hFile);
  } // end if

  return sInfo;
} // DBA_protocol::GetInfo
