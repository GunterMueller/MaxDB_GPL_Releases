/*! 
  -----------------------------------------------------------------------------
 
  module: vcn53.cpp
 
  -----------------------------------------------------------------------------
 
  responsible:  BerndV
 
  special area: DBA History File
 
  description:  implementation of the DBA History file
 
  version:      min 7.2.

  Copyright (c) 1998-2005 SAP AG
 
  -----------------------------------------------------------------------------

    ========== licence begin  GPL
    Copyright (c) 1998-2005 SAP AG

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
 

/* 
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/

#include <stdio.h>

#include "heo06.h"    // virtual file api
#include "heo02.h"    // rte
#include "gcn00_1.h"  // DBMServer error handling
#include "hcn42.h"    // filename functions
#include "hcn53.h"    // class specification

/* 
  -----------------------------------------------------------------------------
  implementation class tcn53_DBAActionDetailLogFile
  -----------------------------------------------------------------------------
*/

/* 
  -----------------------------------------------------------------------------
  protected function tcn53_DBAActionDetailLogFile :: writeOneEventList
  -----------------------------------------------------------------------------
*/
bool tcn53_DBAActionDetailLogFile :: writeOneEventList
      ( const Msg_List & EvtLst )
{
  const Msg_List * pNextEvt = &EvtLst;
  bool                     bStatus = true;
  tsp05_RteFileError       fileError;
  tsp00_Int4               hFile;
  const char             * pCurrent = NULL;
  const char             * pNewLine = NULL;
  int                      nLength;
  // open file
  sqlfopenc (m_szCurrentFile.asCharp(), sp5vf_text, sp5vf_append, sp5bk_disksynced, &hFile, &fileError);

  // write messages
  if (fileError.sp5fe_result == vf_ok) {

    while (pNextEvt != NULL) {
      pCurrent = pNextEvt->Message();

      while ((pCurrent != NULL) && (fileError.sp5fe_result == vf_ok)) {

        // CR-LF handling
        pNewLine = strstr(pCurrent, "\r");
        if (pNewLine == NULL) {
          pNewLine = strstr(pCurrent, "\n");
          if (pNewLine != NULL) {
            nLength = (int) (pNewLine - pCurrent);
            ++pNewLine;
          } else {
            nLength = -1;
          } // end if
        } else {
          nLength = (int) (pNewLine - pCurrent);
          ++pNewLine;
          if (*pNewLine == '\n') {
            ++pNewLine;
          } // end if
        } // end if

        sqlfwritec (hFile, pCurrent, nLength, &fileError);
        pCurrent = pNewLine;

      } // end while

      bStatus = bStatus && (fileError.sp5fe_result == vf_ok);

      pNextEvt  = pNextEvt->NextMessage();
    } // end while

    sqlfclosec (hFile, sp5vf_close_normal, &fileError);

  } else {

    bStatus = false;
  
  } // end if
    
  return bStatus;
} // end tcn53_DBAActionDetailLogFile :: writeOneEventList

/* 
  -----------------------------------------------------------------------------
  implementation class tcn53_DBAAction
  -----------------------------------------------------------------------------
*/

/* 
  -----------------------------------------------------------------------------
  static memeber
  -----------------------------------------------------------------------------
*/
tcn53_DBAAction  * tcn53_DBAAction :: m_pMySelf  = NULL;
bool               tcn53_DBAAction :: m_bEnabled = false;

/* 
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/
#define TXT_TITLESEPARATOR "------------------------------------------------------------------------------"
#define LINE_LEN           1024

#define LEN_MONTH             2
#define POS_MONTH             4
#define LEN_DAY               2
#define POS_DAY               6
#define LEN_HOUR              2
#define POS_HOUR              8
#define LEN_MINUTE            2
#define POS_MINUTE           10
#define LEN_SECOND            2
#define POS_SECOND           12

#define LEN_DATE              (LEN_MONTH + 1 + LEN_DAY)
#define LEN_TIME              (LEN_HOUR + 1 + LEN_MINUTE + 1 + LEN_SECOND)
#define LEN_BEG              14
#define LEN_FID               3
#define LEN_SYSID             8
#define LEN_OBJECT           16
#define LEN_RC                4
#define LEN_ENDE             14

#ifdef _WIN32
#define LEN_NEWLINE           2
#else
#define LEN_NEWLINE           1
#endif
#define LEN_RECORD            (LEN_DATE + 1 + LEN_TIME + 1 +  LEN_BEG + 1 + LEN_FID + 1 + LEN_SYSID + 1 + LEN_OBJECT + 1 + LEN_RC + 1 + LEN_ENDE + LEN_NEWLINE)

/* 
  -----------------------------------------------------------------------------
  public function tcn53_DBAAction :: start
  -----------------------------------------------------------------------------
*/
void tcn53_DBAAction :: start
      ( const tsp00_DbNamec    & szDbName,
        const char *             szFuncID,
        const char *             szObject )
{
  tsp00_Timestampc szTimestamp;
  start(szDbName, timestamp(szTimestamp), szFuncID, szObject);
} // end tcn53_DBAAction :: start
/* 
  -----------------------------------------------------------------------------
*/
void tcn53_DBAAction :: start
      ( const tsp00_DbNamec    & szDbName,
        const tsp00_Timestampc & szBegin,
        const char *             szFuncID,
        const char *             szObject )
{
  if (!m_bEnabled) {
    return;
  } // end if

  if (m_pMySelf != NULL) {
    delete m_pMySelf;
    m_pMySelf = NULL;
  } // end if

  m_pMySelf = new tcn53_DBAAction(szDbName);
  
  if (m_pMySelf != NULL) {
    m_pMySelf->startAction(szBegin, szFuncID, szObject);
  } // end if 

} // end tcn53_DBAAction :: start

/* 
  -----------------------------------------------------------------------------
  public function tcn53_DBAAction :: finish
  -----------------------------------------------------------------------------
*/
void tcn53_DBAAction :: finish
    ( bool                     bOK )
{
  tsp00_Timestampc szTimestamp;
  finish(timestamp(szTimestamp), bOK);
} // end tcn53_DBAAction :: finish
/* 
  -----------------------------------------------------------------------------
*/
void tcn53_DBAAction :: finish
    ( const tsp00_Timestampc & szEnd,
      bool                     bOK )
{
  if (!m_bEnabled) {
    return;
  } // end if

  if (m_pMySelf == NULL) {
    return;
  } // end if

  m_pMySelf->finishAction (szEnd, bOK);

  delete m_pMySelf;
  m_pMySelf = NULL;
  
} // end tcn53_DBAAction :: finish

/* 
  -----------------------------------------------------------------------------
  public function tcn53_DBAAction :: writeDetail
  -----------------------------------------------------------------------------
*/
void tcn53_DBAAction :: writeDetail
      ( const Msg_List & EventList)
{
  if (!m_bEnabled || (m_pMySelf == NULL)) {
    return;
  } // end if

  m_pMySelf->m_oDetailLog.writeEntry (EventList);
} // end tcn53_DBAAction :: writeDetail
/* 
  -----------------------------------------------------------------------------
*/
void tcn53_DBAAction :: writeDetail
      ( const char * szFormat,
        const char * p0,
        const char * p1,
        const char * p2,
        const char * p3,
        const char * p4,
        const char * p5,
        const char * p6,
        const char * p7,
        const char * p8,
        const char * p9)
{
  if (!m_bEnabled || (m_pMySelf == NULL)) {
    return;
  } // end if

  m_pMySelf->m_oDetailLog.writeEntry (szFormat, p0,p1,p2,p3,p4,p5,p6,p7,p8,p9);
} // end tcn53_DBAAction :: writeDetail

/* 
  -----------------------------------------------------------------------------
  public function tcn53_DBAAction :: enable
  -----------------------------------------------------------------------------
*/
void tcn53_DBAAction :: enable
    (const bool bEnabled ) 
{
  m_bEnabled = bEnabled;
} // end tcn53_DBAAction :: enable

/* 
  -----------------------------------------------------------------------------
  private constructor tcn53_DBAAction :: tcn53_DBAAction
  -----------------------------------------------------------------------------
*/
tcn53_DBAAction :: tcn53_DBAAction 
                   ( const tsp00_DbNamec    & szDbName )
                 : m_oDetailLogFile(),
                   m_oDetailLog()
{
  m_szDbName = szDbName;
  cn42GetFileName(szDbName, FGET_DBAHIST_CN42, m_szFile);

  m_oDetailLog.addFile(&m_oDetailLogFile, false);
} // end tcn53_DBAAction :: tcn53_DBAAction

/* 
  -----------------------------------------------------------------------------
  private function tcn53_DBAAction :: startAction
  -----------------------------------------------------------------------------
*/
void tcn53_DBAAction :: startAction  
      ( const tsp00_Timestampc & szBegin,
        const char *             szFuncID,
        const char *             szObject )
{
  // save data
  m_szBegin   = szBegin;
  m_szFuncID.rawAssign(szFuncID);
  m_szObject.rawAssign(szObject); 

  // generate and set detail file name
  tsp00_Pathc szFileKey;
  tsp00_Pathc szFile;

  szFileKey.rawAssign(FGET_DBADTL_CN42);
  strcat(szFileKey, FGET_TOKENDEL_CN42);
  strcat(szFileKey, szBegin);
  strcat(szFileKey, ".");
  strcat(szFileKey, szFuncID);

  cn42GetFileName(m_szDbName, szFileKey, szFile);

  m_oDetailLogFile.setFile(szFile);

  m_pMySelf->writeAction (szBegin, false, false);

  // write first detail log entry
  m_oDetailLog.writeEntry (DBADTL_HEADER_CN53, szBegin, szFuncID, szObject);

} // end tcn53_DBAAction :: startAction

/* 
  -----------------------------------------------------------------------------
  private function tcn53_DBAAction :: finishAction
  -----------------------------------------------------------------------------
*/
void tcn53_DBAAction :: finishAction  
      ( const tsp00_Timestampc & szEnd,
        bool                     bOK )
{

  m_oDetailLog.writeEntry (DBADTL_FOOTER_CN53, szEnd, (bOK) ? "Success" : "Failure");
  writeAction (szEnd, bOK, true);

} // end tcn53_DBAAction :: finishAction  

/* 
  -----------------------------------------------------------------------------
  private function tcn53_DBAAction :: writeAction
  -----------------------------------------------------------------------------
*/
void tcn53_DBAAction :: writeAction  
      ( const tsp00_Timestampc & szEnd,
        bool                     bOK,
        bool                     bFinish )
{
  FUNCTION_DBG_MCN00_1("tcn53_DBAAction :: writeEntry");

  tsp00_Int4               hFile;

  // open file
  if (openFile(m_szFile, hFile)) {
    tsp05_RteFileError fileError;
    char               szBuffer[LINE_LEN];
    char               szDate  [LINE_LEN];
    char               szTime  [LINE_LEN];

    fileError.sp5fe_result.becomes(vf_ok);

    if (bFinish) {
      sqlfseekc (hFile, LEN_RECORD * -1,  sp5vf_seek_end, &fileError);
    } // end if

    if (fileError.sp5fe_result == vf_ok) {
      sprintf(szDate, "%.*s-%.*s",
                      LEN_MONTH,  &(m_szBegin[POS_MONTH]),
                      LEN_DAY,    &(m_szBegin[POS_DAY]));

      sprintf(szTime, "%.*s:%.*s:%.*s",
                      LEN_HOUR,   &(m_szBegin[POS_HOUR])   ,
                      LEN_MINUTE, &(m_szBegin[POS_MINUTE]) ,
                      LEN_SECOND, &(m_szBegin[POS_SECOND]));

      sprintf(szBuffer, "%-*s %-*s %-*s %-*s %-*s %-*s %*d %-*s",
                         LEN_DATE,   szDate,
                         LEN_TIME,   szTime,
                         LEN_BEG,    m_szBegin.asCharp(),
                         LEN_FID,    m_szFuncID.asCharp(),
                         LEN_SYSID,  m_szDbName.asCharp(),
                         LEN_OBJECT, m_szObject.asCharp(),
                         LEN_RC,     (bFinish) ? ((bOK) ? 0 : 2) : 9999,                         LEN_ENDE,   szEnd.asCharp());


      sqlfwritec (hFile, szBuffer, -1, &fileError);
    } // end if
    sqlfclosec (hFile, sp5vf_close_normal, &fileError);

  } // end if

} // end tcn53_DBAAction :: writeAction

/* 
  -----------------------------------------------------------------------------
  private function tcn53_DBAAction :: openFile
  -----------------------------------------------------------------------------
*/
bool tcn53_DBAAction :: openFile
      (const tsp00_Pathc  szFilename,
       tsp00_Int4       & hFile)
{
  FUNCTION_DBG_MCN00_1("tcn53_DBAAction :: openFile");

  tsp05_RteFileInfo   fileInfo;
  tsp05_RteFileError  fileError;

  // check existence of file
  sqlfinfoc (szFilename.asCharp(), &fileInfo, &fileError);
  if (fileError.sp5fe_result == vf_ok) {
    sqlfopenc (szFilename.asCharp(), sp5vf_text, sp5vf_append, sp5bk_disksynced, &hFile, &fileError);
    if (fileError.sp5fe_result == vf_ok) {
      if (!fileInfo.sp5fi_exists) {
        sqlfwritec (hFile, TXT_TITLESEPARATOR, -1, &fileError);
        if (fileError.sp5fe_result == vf_ok) {
          char szBuffer[LINE_LEN];
          sprintf(szBuffer, "%-*s %-*s %-*s %-*s %-*s %-*s %-*s %-*s",
                             LEN_DATE,   "Date",
                             LEN_TIME,   "Time",
                             LEN_BEG,    "Beg",
                             LEN_FID,    "Fid",
                             LEN_SYSID,  "Sysid",
                             LEN_OBJECT, "Object",
                             LEN_RC,     "Rc",
                             LEN_ENDE,   "Ende");
          sqlfwritec (hFile, szBuffer, -1, &fileError);
          if (fileError.sp5fe_result == vf_ok) {
            sqlfwritec (hFile, TXT_TITLESEPARATOR, -1, &fileError);
          } // end if
        } // end if
      } // end if
      if (fileError.sp5fe_result != vf_ok) {
        sqlfclosec (hFile, sp5vf_close_normal, &fileError);
      } // end if
    } // end if
  } // end if

  return (fileError.sp5fe_result == vf_ok);
} // end tcn53_DBAAction :: openFile

/* 
  -----------------------------------------------------------------------------
  protected function tcn53_DBAAction :: timestamp
  -----------------------------------------------------------------------------
*/
const tsp00_Timestampc & tcn53_DBAAction :: timestamp
    ( tsp00_Timestampc & szTimestamp )
{
  tsp00_Date  DateP;
  tsp00_Time  TimeP;
  tsp00_Datec DateC;
  tsp00_Timec TimeC;
    
  sqldattime(DateP, TimeP);
  DateC.p2c(DateP);
  TimeC.p2c(TimeP);

  _stprintf(szTimestamp, "%.8s%.6s", DateC.asCharp(), (TimeC.asCharp() + 2));

  return szTimestamp;

} // end tcn53_DBAAction :: timestamp
