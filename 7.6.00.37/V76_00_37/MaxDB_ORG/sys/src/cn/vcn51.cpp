
/* 
  -----------------------------------------------------------------------------
 
  module: vcn51.cpp
 
  -----------------------------------------------------------------------------
 
  responsible:  BerndV
 
  special area: DBMServer Configuration Value Class
   
  description:  DBMServer Configuration Value Class - Implementation

  -----------------------------------------------------------------------------

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

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
 */
#include <stdlib.h>
#include <stdio.h>

#include "heo06.h"
#include "hcn42.h"
#include "hcn90.h"
#include "hcn51.h"

#include "DBM/Srv/Message/DBMSrvMsg_RTEError.hpp"

/*
  =============================================================================
  class:        tcn51ConfigEntry
  =============================================================================
 */
class tcn51ConfigEntry {
  private:
    char               szConfigString[SIZE_OF_CONFIG_BUFFER];
    char               szConfigName  [SIZE_OF_CONFIG_BUFFER];
    char               szConfigValue [SIZE_OF_CONFIG_BUFFER];
    tcn51ConfigEntry * pNext;

    tcn51ConfigEntry () 
    {
      szConfigString[0] = CHAR_STRINGTERM_CN90; 
      szConfigName  [0] = CHAR_STRINGTERM_CN90; 
      szConfigValue [0] = CHAR_STRINGTERM_CN90; 
      pNext             = NULL;
    }

    friend class cn51DBMConfigValue;
    friend class cn51DBMConfigValueIterator;
};

/*
  =============================================================================
  class:        cn51DBMConfigValue
  =============================================================================
 */

Msg_List   cn51DBMConfigValue::m_LastEvent;

/*
  -----------------------------------------------------------------------------
  public function:     cn51DBMConfigValue::cn51DBMConfigValue
  -----------------------------------------------------------------------------
 */
cn51DBMConfigValue::cn51DBMConfigValue 
  ( const char        * szDbName, 
    const char        * szValueName,
    ConfigurationMode   oMode )
{
  m_szName.rawAssign(szValueName);
  m_szDbName.rawAssign(szDbName);

  m_oReadMode = oMode;
  m_oSaveMode = (oMode == ModeAll) ? ModeInstance : oMode;

} // cn51DBMConfigValue::cn51DBMConfigValue 

/*
  -----------------------------------------------------------------------------
  public function:     cn51DBMConfigValue::operator = 
  -----------------------------------------------------------------------------
 */
cn51DBMConfigValue cn51DBMConfigValue::operator = ( const tsp00_Int4 cfgValue )
{
  tsp00_C512c      szValue;
  sprintf(szValue, "%ld", (long) cfgValue);
  saveValue(m_szDbName, m_szName, szValue);
  return *this;
} // cn51DBMConfigValue::operator = 

/*
  -----------------------------------------------------------------------------
  public function:     cn51DBMConfigValue::operator = 
  -----------------------------------------------------------------------------
 */
cn51DBMConfigValue cn51DBMConfigValue::operator = ( const char * cfgValue )
{
  tsp00_C512c      szValue;
  szValue.rawAssign(cfgValue);
  saveValue(m_szDbName, m_szName, szValue);
  return *this;
} // cn51DBMConfigValue::operator = 

/*
  -----------------------------------------------------------------------------
  public function:     cn51DBMConfigValue::operator tsp00_Int4
  -----------------------------------------------------------------------------
 */
cn51DBMConfigValue::operator tsp00_Int4 ( )
{
  return atol(readValue(m_szDbName, m_szName, m_szValue));
} // cn51DBMConfigValue::operator tsp00_Int4

/*
  -----------------------------------------------------------------------------
  public function:     cn51DBMConfigValue::operator const char *
  -----------------------------------------------------------------------------
 */
cn51DBMConfigValue::operator const char * ( )
{
  return readValue(m_szDbName, m_szName, m_szValue);
} // cn51DBMConfigValue::operator const char *

/*
  -----------------------------------------------------------------------------
  private function:     cn51DBMConfigValue::saveValue
  -----------------------------------------------------------------------------
 */
void cn51DBMConfigValue::saveValue ( const tsp00_DbNamec   szDbName,
                                     const tsp00_Namec     szName, 
                                     const tsp00_C512c     szValue ) const
{
  tcn51ConfigEntry  * pAnchor   = NULL;
  tcn51ConfigEntry  * pEntry    = NULL;
  tcn51ConfigEntry  * pLast     = NULL;
  char                szNewConfigString[600];
  char                bWritten  = false;

  // prepare config file contents
  pAnchor = readFile(szDbName, m_oSaveMode, false);
    
  // prepare config string
  if (strlen(szValue) > 0) {
    sprintf ( szNewConfigString , "%-20s=%s", (const char   *) szName,
                                              (const char   *) szValue);
  } else {
    szNewConfigString[0] = CHAR_STRINGTERM_CN90;
  } // end if

  // search entry
  pEntry = pAnchor;
  while (pEntry != NULL) {
    if (cn90CheckKeyword(pEntry->szConfigString, szName, false)) {
      // wow match found, replace config string
      strcpy(pEntry->szConfigString, szNewConfigString);
      bWritten = true;
    } // end if

    pLast = pEntry;
    pEntry = pEntry->pNext;
  } // end while

  // add new entry if not found
  if (!bWritten) {
    pEntry = new tcn51ConfigEntry;
    if (pEntry != NULL) {
      strcpy(pEntry->szConfigString, szNewConfigString);
    } // end if
    if (pLast != NULL) {
      pLast->pNext = pEntry;
    } else {
      pAnchor = pEntry;
    } // end if
  } // end if

  // save file
  saveFile(szDbName, m_oSaveMode, pAnchor);

} // cn51DBMConfigValue::saveValue
    
/*
  -----------------------------------------------------------------------------
  private function:     cn51DBMConfigValue::readValue
  -----------------------------------------------------------------------------
 */
char * cn51DBMConfigValue::readValue ( const tsp00_DbNamec   szDbName,
                                       const tsp00_Namec     szName, 
                                       char  szValue[SIZE_OF_CONFIG_BUFFER]) const
{
  tcn51ConfigEntry  * pEntry    = NULL;
  tcn51ConfigEntry  * pDelete   = NULL;
  const char        * pValue    = NULL;

  // initialize return value
  szValue[0] = 0;

  // read file content
  for (int i = 0; (i < 3) && (strlen(szValue) == 0); ++i) {

    // reset values
    pEntry  = NULL;

    // activate file
    switch(i) {
      case 0:
        // check instance specific file
        if ((m_oReadMode == ModeAll) || (m_oReadMode == ModeInstance)) {
          pEntry  = readFile(szDbName, ModeInstance, false);
        } // end if
        break;
      case 1:
        // check installation specific file
        if ((m_oReadMode == ModeAll) || (m_oReadMode == ModeInstallation)) {
          pEntry  = readFile(szDbName, ModeInstallation, false);
        } // end if
        break;
      case 2:
        // check installation specific file
        if ((m_oReadMode == ModeAll) || (m_oReadMode == ModeGlobal)) {
          pEntry  = readFile(szDbName, ModeGlobal, false);
        } // end if
        break;
    } // end switch

    // now we have a list from the current active file
    // search entry
    while (pEntry != NULL) {
      if ((pValue == NULL) && cn90CheckKeyword(pEntry->szConfigString, szName, false)) {
        pValue = strchr(pEntry->szConfigString, '=');
        if (pValue != NULL) {
          // entry found
          pValue++;
          strcpy(szValue, pValue);
        } // end if
      } // end if

      // delete allocated memory
      pDelete = pEntry;
      pEntry = pEntry->pNext;
      delete pDelete;
      
    } // end while

  } // end for

  return &szValue[0];
} // cn51DBMConfigValue::readValue

/*
  -----------------------------------------------------------------------------
  private function:     cn51DBMConfigValue::readFile
  -----------------------------------------------------------------------------
 */
tcn51ConfigEntry * cn51DBMConfigValue::readFile 
  ( const tsp00_DbNamec szDbName,
    ConfigurationMode   oMode,
    bool                bMigrate )
{
  FUNCTION_DBG_MCN00_1("cn51DBMConfigValue::readFile");

  tsp00_Pathc         szConfigFile;
  tsp00_Pathc         szBackupFile;
  tsp00_Int4     	    hFile;
  tsp05_RteFileError  errFile;
  tsp00_Longint       nOut;
  tcn51ConfigEntry  * pEntry    = NULL;
  tcn51ConfigEntry  * pLast     = NULL;
  tcn51ConfigEntry  * pAnchor   = NULL;

  m_LastEvent.ClearMessageList();

  // prepare config file name
  if (oMode == ModeGlobal) {
    cn42GetFileName(szDbName, FGET_DBMGLOBCFG_CN42, szConfigFile, szBackupFile);
  } else if (oMode == ModeInstallation) {
    cn42GetFileName(szDbName, FGET_DBMINSTCFG_CN42, szConfigFile, szBackupFile);
  } else {
    cn42GetFileName(szDbName, FGET_DBMCFG_CN42, szConfigFile, szBackupFile);
  } // end if

  // init list
  pAnchor = new tcn51ConfigEntry;
  pEntry  = pAnchor;
  // open file for read
  sqlfopenc(szConfigFile, sp5vf_text, sp5vf_read, sp5bk_buffered, &hFile, &errFile );
  if (oMode == ModeInstance) {
    if (errFile.sp5fe_result != vf_ok) {
      DBMSrvMsg_Error oError;
      cn42RestoreFile(szDbName, FGET_DBMCFG_CN42, oError);
      sqlfopenc(szConfigFile, sp5vf_text, sp5vf_read, sp5bk_buffered, &hFile, &errFile );
    }  // end if
    if (errFile.sp5fe_result != vf_ok) {
      // !!!!! FH: szBackupFile instead szConfigFile !!!!! PTS 1103341
      sqlfopenc(szBackupFile, sp5vf_text, sp5vf_read, sp5bk_buffered, &hFile, &errFile );
    }  // end if
  }  // end if

  // read file
  if (errFile.sp5fe_result == vf_ok) {
    while (errFile.sp5fe_result == vf_ok && pEntry != NULL) {
      sqlfreadc (hFile, pEntry->szConfigString, sizeof(pEntry->szConfigString), &nOut, &errFile);
      cn90StripTrailingChar(pEntry->szConfigString, '\n');
      cn90StripTrailingChar(pEntry->szConfigString, '\r');
      cn90Strip(pEntry->szConfigString);
      if (bMigrate) {
        if (migrateValue(pEntry->szConfigString)) {
          pEntry->pNext = new tcn51ConfigEntry;
          pLast = pEntry;
          pEntry = pEntry->pNext;
        } else {
          pEntry->szConfigString[0] = 0;
        } // end if
      } else {
        if (strlen(pEntry->szConfigString) > 0) {
          pEntry->pNext = new tcn51ConfigEntry;
          pLast = pEntry;
          pEntry = pEntry->pNext;
        } // end if
      } // end if
    } // end while
    sqlfclosec(hFile, sp5vf_close_normal, &errFile);
  } else {
    // write error to protocol
    m_LastEvent.Overrule(DBMSrvMsg_RTEError(errFile));
    m_LastEvent.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_FILE));
  } // end if

  // cleanup 
  if (pEntry != NULL) {
    if (pAnchor == pEntry) {
      pAnchor = NULL;
    } // end if
    delete pEntry;
  } // end if
  if (pLast != NULL) {
    pLast->pNext = NULL;
  } // end if

  return pAnchor;
} // end cn51DBMConfigValue::readFile

/*
  -----------------------------------------------------------------------------
  private function:     cn51DBMConfigValue::saveFile
  -----------------------------------------------------------------------------
 */
void cn51DBMConfigValue::saveFile 
  ( const tsp00_DbNamec szDbName,
    ConfigurationMode   oMode,
    tcn51ConfigEntry  * pAnchor )
{
  FUNCTION_DBG_MCN00_1("cn51DBMConfigValue::saveFile");

  tsp00_Pathc         szConfigFile;
  tsp00_Pathc         szBackupFile;
  tsp00_Int4     	    hFile;
  tsp05_RteFileError  errFile;
  tcn51ConfigEntry  * pEntry    = NULL;
  tcn51ConfigEntry  * pDelete   = NULL;

  m_LastEvent.ClearMessageList();

  // prepare config file name
  if (oMode == ModeGlobal) {
    cn42GetFileName(szDbName, FGET_DBMGLOBCFG_CN42, szConfigFile, szBackupFile);
  } else if (oMode == ModeInstallation) {
    cn42GetFileName(szDbName, FGET_DBMINSTCFG_CN42, szConfigFile, szBackupFile);
  } else {
    cn42GetFileName(szDbName, FGET_DBMCFG_CN42, szConfigFile, szBackupFile);
  } // end if

  // open file for write
  sqlfopenc(szConfigFile, sp5vf_text, sp5vf_write, sp5bk_buffered, &hFile, &errFile );
  if (errFile.sp5fe_result != vf_ok) {
    DBMSrvMsg_Error oError;
    cn42RestoreFile(szDbName, FGET_DBMCFG_CN42, oError);
    sqlfopenc(szConfigFile, sp5vf_text, sp5vf_write, sp5bk_buffered, &hFile, &errFile );
  }  // end if
  if (errFile.sp5fe_result != vf_ok) {
    sqlfopenc(szBackupFile, sp5vf_text, sp5vf_write, sp5bk_buffered, &hFile, &errFile ); 
  }  // end if

  // write file
  if (errFile.sp5fe_result == vf_ok) {
    pEntry = pAnchor;
    while (errFile.sp5fe_result == vf_ok && pEntry != NULL) {
      if (strlen(pEntry->szConfigString) > 0) {
        sqlfwritec (hFile, pEntry->szConfigString, -1, &errFile);
      } // end if
      pDelete = pEntry;
      pEntry = pEntry->pNext;
      delete pDelete;
    } // end while
    if (errFile.sp5fe_result != vf_ok) {
      // write error to protocol
      m_LastEvent.Overrule(DBMSrvMsg_RTEError(errFile));
      m_LastEvent.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_FILE));

      sqlfclosec(hFile, sp5vf_close_normal, &errFile);
      DBMSrvMsg_Error oError;
      cn42RestoreFile(szDbName, FGET_DBMCFG_CN42, oError);
    } else {
      sqlfclosec(hFile, sp5vf_close_normal, &errFile);
      if (errFile.sp5fe_result != vf_ok) {
        // write error to protocol
        m_LastEvent.Overrule(DBMSrvMsg_RTEError(errFile));
        m_LastEvent.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_FILE));

        DBMSrvMsg_Error oError;
        cn42RestoreFile(szDbName, FGET_DBMCFG_CN42, oError);
      } else {
        DBMSrvMsg_Error oError;
        cn42BackupFile(szDbName, FGET_DBMCFG_CN42, oError);
      } // end if
    } // end if
  } else {
    // write error to protocol
    m_LastEvent.Overrule(DBMSrvMsg_RTEError(errFile));
    m_LastEvent.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_FILE));
  } // end if

} // end cn51DBMConfigValue::saveFile

/*
  -----------------------------------------------------------------------------
  public function:     cn51DBMConfigValue::migrateFile
  -----------------------------------------------------------------------------
 */
void cn51DBMConfigValue::migrateFile ( const tsp00_DbNamec        szDbName )
{
  tcn51ConfigEntry * pAnchor = NULL;
  
  pAnchor = readFile(szDbName, ModeInstance, true);

  if (pAnchor != NULL) {
    saveFile(szDbName, ModeInstance, pAnchor);
  } // end if

} // cn51DBMConfigValue::migrateFile

/*
  -----------------------------------------------------------------------------
  private function:     cn51DBMConfigValue::migrateValue
  -----------------------------------------------------------------------------
 */
bool cn51DBMConfigValue::migrateValue ( char * pValue )
{
  // remove obsolete values (trace and events)
  int nIndex = 0;
  const char * szObsolete[] = 
    {"DEFAULT","INDEX","LOCK","LONG","PAGES","STANDARD","TABLE",
    "OBJECT","OBJECTADD","OBJECTGET","OBJECTALTER","OBJECTFREE",
    "SELECT","INSERT","UPDATE","DELETE","ORDER","OPTIMIZER",
    "TIME","CHECK","COMMANDS","TOPICCONVERTER","TOPICDATA",
    "TOPICDATACHAIN","TOPICDATAPAM","TOPICDATATREE","TOPICFRAMECTRL",
    "TOPICLOG","TOPICLOGVOLUME","TOPICLOGHISTORY","TOPICLOGTRANS",
    "TOPICLOGACTION","TOPICIOMAN","TOPICMEMORY","TOPICVOLUME",
    "TOPICRUNTIME","TOPICALLOCATOR","TOPICDATAPAGEPRIM","TOPICDATAPAGEINV",
    "TOPICDATAPAGELOG","TOPICLOGPAGE","TOPICCONVERTERPAGE","TOPICTABLE",
    "TOPICCOMMON","CHECKCONVERTER","CHECKDATA","CHECKDATACHAIN",
    "CHECKDATAPAM","CHECKDATATREE","CHECKFRAMECTRL","CHECKLOG",
    "CHECKLOGVOLUME","CHECKLOGHISTORY","CHECKLOGTRANS","CHECKLOGACTION",
    "CHECKIOMAN","CHECKMEMORY","CHECKVOLUME","CHECKRUNTIME","CHECKALLOCATOR",
    "CHECKDATAPAGEPRIM","CHECKDATAPAGEINV","CHECKDATAPAGELOG","CHECKLOGPAGE",
    "CHECKCONVERTERPAGE","CHECKTABLE","CHECKCOMMON","DB_ABOVE_LIMIT",
    "DB_BELOW_LIMIT","LOG_ABOVE_LIMIT",NULL};

  while (szObsolete[nIndex] != NULL) {
    if (cn90CheckKeyword(pValue, szObsolete[nIndex], false)) {
      return false;
    } // end if
    ++nIndex;
  } // end if

  // uncrypt crypted values
  char * pAssign = strchr(pValue, '=');
  char * pCurrent;
  if (pAssign != NULL) {
    ++pAssign;
    if (strlen(pAssign) != 48) {
      return true;
    } // end if

    pCurrent = pAssign;
    while (*pCurrent!=0) {
      switch (*pCurrent) {
        case 'a':case 'b':case 'c':case 'd':case 'e':case 'f':
        case 'A':case 'B':case 'C':case 'D':case 'E':case 'F':
        case '0':case '1':case '2':case '3':case '4':
        case '5':case '6':case '7':case '8':case '9':
          break;
        default:
          return  true;
          break;
      } // end switch
      ++pCurrent;
    } // end while

    // yeah this must be a crypted value!
    tsp00_C64c    szCrypt;
    tsp00_Namec   szValue;
    szCrypt.rawAssign(pAssign);
    cn90CryptStringToClearString(szCrypt, szValue);
    strcpy(pAssign, szValue);
  } // end if

  return true;
} // cn51DBMConfigValue::migrateValue

/*
  =============================================================================
  class:        cn51DBMConfigValueIterator
  =============================================================================
 */

/*
  -----------------------------------------------------------------------------
  public function:     cn51DBMConfigValueIterator::cn51DBMConfigValueIterator
  -----------------------------------------------------------------------------
 */
cn51DBMConfigValueIterator::cn51DBMConfigValueIterator 
  ( const char                          * szDbName, 
    cn51DBMConfigValue::ConfigurationMode oMode )
{
  tsp00_DbNamec lszDbName;
  lszDbName.rawAssign(szDbName);
  
  m_oReadMode = oMode;
  m_oCurrentMode = (oMode == cn51DBMConfigValue::ModeAll) ? cn51DBMConfigValue::ModeInstance : oMode;

  tcn51ConfigEntry * pLast                     = NULL;
  tcn51ConfigEntry * pEntryToCheck             = NULL;
  tcn51ConfigEntry * pEntryBeforeEntryToCheck  = NULL;
  tcn51ConfigEntry * pEntryValid               = NULL;

  m_pAnchor           = NULL;
  m_pCurrent          = NULL;

  // if required read instance file
  if ((oMode == cn51DBMConfigValue::ModeAll) || (oMode == cn51DBMConfigValue::ModeInstance)) {
    m_pAnchor = cn51DBMConfigValue::readFile(lszDbName, cn51DBMConfigValue::ModeInstance, false);
  } // end if

  // if required read installation file
  if ((oMode == cn51DBMConfigValue::ModeAll) || (oMode == cn51DBMConfigValue::ModeInstallation)) {
    if (m_pAnchor == NULL) {
      m_pAnchor = cn51DBMConfigValue::readFile(lszDbName, cn51DBMConfigValue::ModeInstallation, false);;
    } else {
      pLast = m_pAnchor;
      while (pLast->pNext != NULL) {
        pLast = pLast->pNext;
      } // end while
      pLast->pNext = cn51DBMConfigValue::readFile(lszDbName, cn51DBMConfigValue::ModeInstallation, false);;
    } // end if
  } // end if

  // if required read global file
  if ((oMode == cn51DBMConfigValue::ModeAll) || (oMode == cn51DBMConfigValue::ModeGlobal)) {
    if (m_pAnchor == NULL) {
      m_pAnchor = cn51DBMConfigValue::readFile(lszDbName, cn51DBMConfigValue::ModeGlobal, false);;
    } else {
      pLast = m_pAnchor;
      while (pLast->pNext != NULL) {
        pLast = pLast->pNext;
      } // end while
      pLast->pNext = cn51DBMConfigValue::readFile(lszDbName, cn51DBMConfigValue::ModeGlobal, false);;
    } // end if
  } // end if

  // prepare entries for analysis
  pEntryToCheck = m_pAnchor;
  const char * pData = NULL;
  while (pEntryToCheck != NULL) {
    cn90GetToken(pEntryToCheck->szConfigString, pEntryToCheck->szConfigName, 1, sizeof(pEntryToCheck->szConfigName));
    pData = strchr(pEntryToCheck->szConfigString, '=');
    if (pData != NULL) {
      pData++;
      strcpy(pEntryToCheck->szConfigValue, pData);
    } // end if
    pEntryToCheck            = pEntryToCheck->pNext;
  } // end while

  // now we have the whole list with duplicates in m_pAnchor
  
  // remove duplicates
  pEntryToCheck = m_pAnchor;
  while (pEntryToCheck != NULL) {
    pEntryValid = m_pAnchor;
    while (pEntryValid != pEntryToCheck) {
      if (stricmp(GetCurrentName(pEntryToCheck), GetCurrentName(pEntryValid)) == 0) {
        // remove the entry which is checked
        break;
      } // end if
      pEntryValid = pEntryValid->pNext;
    } // end if

    if (pEntryValid != pEntryToCheck) {
      // maintain list
      pEntryBeforeEntryToCheck->pNext = pEntryToCheck->pNext;
      // delete entry 
      delete pEntryToCheck;
      // goto next entry 
      pEntryToCheck  = pEntryBeforeEntryToCheck->pNext;
    } else {
      pEntryBeforeEntryToCheck = pEntryToCheck;
      pEntryToCheck            = pEntryToCheck->pNext;
    } // end if
  } // end while

  m_pCurrent = m_pAnchor;
} // cn51DBMConfigValueIterator::cn51DBMConfigValueIterator 

/*
  -----------------------------------------------------------------------------
  public function:     cn51DBMConfigValueIterator::~cn51DBMConfigValueIterator
  -----------------------------------------------------------------------------
 */
cn51DBMConfigValueIterator::~cn51DBMConfigValueIterator 
  ( )
{
  tcn51ConfigEntry *  pEntry  = NULL;
  tcn51ConfigEntry *  pDelete = NULL;

  // delete chain
  pEntry  = m_pAnchor;
  pDelete = NULL;
  while (pEntry != NULL) {
    pDelete = pEntry;
    pEntry = pEntry->pNext;
    delete pDelete;
  } // end while
} // cn51DBMConfigValueIterator::~cn51DBMConfigValueIterator 

/*
  -----------------------------------------------------------------------------
  public function:     cn51DBMConfigValueIterator::Next
  -----------------------------------------------------------------------------
 */
void cn51DBMConfigValueIterator::Next
  ( )
{
  if (m_pCurrent != NULL) {
    m_pCurrent = m_pCurrent->pNext;
  } // end if
} // end cn51DBMConfigValueIterator::Next

/*
  -----------------------------------------------------------------------------
  public function:     cn51DBMConfigValueIterator::GotoFirst
  -----------------------------------------------------------------------------
 */
void cn51DBMConfigValueIterator::GotoFirst
  ( )
{
  m_pCurrent = m_pAnchor;
} // end cn51DBMConfigValueIterator::GotoFirst

/*
  -----------------------------------------------------------------------------
  public function:     cn51DBMConfigValueIterator::Goto
  -----------------------------------------------------------------------------
 */
void cn51DBMConfigValueIterator::Goto
  ( const char * szName)
{
  GotoFirst();

  while (m_pCurrent != NULL) {
    if (stricmp(GetCurrentName(m_pCurrent), szName) == 0) {
      break;
    } // end if
    m_pCurrent = m_pCurrent->pNext;
  } // end if

} // end cn51DBMConfigValueIterator::Goto

/*
  -----------------------------------------------------------------------------
  public function:     cn51DBMConfigValueIterator::GetCurrentName
  -----------------------------------------------------------------------------
 */
const char * cn51DBMConfigValueIterator::GetCurrentName
  ( ) const
{
  return GetCurrentName(m_pCurrent);
} // end cn51DBMConfigValueIterator::GetCurrentName

/*
  -----------------------------------------------------------------------------
  private function:     cn51DBMConfigValueIterator::GetCurrentName
  -----------------------------------------------------------------------------
 */
const char * cn51DBMConfigValueIterator::GetCurrentName
  ( tcn51ConfigEntry * pCurrent ) const
{
  if (pCurrent != NULL) {
    return pCurrent->szConfigName;
  } // end if

  return "";
} // end cn51DBMConfigValueIterator::GetCurrentName

/*
  -----------------------------------------------------------------------------
  public function:     cn51DBMConfigValueIterator::GetCurrentValueAsString
  -----------------------------------------------------------------------------
 */
const char * cn51DBMConfigValueIterator::GetCurrentValueAsString
  ( ) const
{
  if (m_pCurrent != NULL) {
    return m_pCurrent->szConfigValue;
  } // end if

  return "";
} // end cn51DBMConfigValueIterator::GetCurrentValueAsString

/*
  -----------------------------------------------------------------------------
  public function:     cn51DBMConfigValueIterator::GetCurrentValueAsInt
  -----------------------------------------------------------------------------
 */
tsp00_Int4 cn51DBMConfigValueIterator::GetCurrentValueAsInt
  ( ) const
{
  return atol(GetCurrentValueAsString());
} // end cn51DBMConfigValueIterator::GetCurrentValueAsInt