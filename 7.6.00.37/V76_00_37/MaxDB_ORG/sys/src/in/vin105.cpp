/*



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





*/
/*! 
  -----------------------------------------------------------------------------
 
  module: vin105.cpp
 
  -----------------------------------------------------------------------------
 
  responsible:  BerndV
 
  special area: protocol file class
 
  description:  implementation of a protocol file class

  version:      min 7.2.

  -----------------------------------------------------------------------------
 
                          Copyright (c) 1998-2005 SAP AG
 
  -----------------------------------------------------------------------------
*/
 

/* 
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/

#include "heo02.h"   // virtual file api
#include "heo06.h"   // virtual file api

#include "gin100.h"  // error messages
#include "hin105.h"  // class specification

/* 
  -----------------------------------------------------------------------------
  implementation class tin105_LogFile
  -----------------------------------------------------------------------------
*/

/* 
  -----------------------------------------------------------------------------
  public function tin105_LogFile :: enable
  -----------------------------------------------------------------------------
*/
void tin105_LogFile :: enable(const bool bEnabled)
{
  m_bEnabled = bEnabled;
} // end tin105_LogFile :: enable

/* 
  -----------------------------------------------------------------------------
  public function tin105_LogFile :: disable
  -----------------------------------------------------------------------------
*/
void tin105_LogFile :: disable()
{
  m_bEnabled = false;
} // end tin105_LogFile :: disable

/* 
  -----------------------------------------------------------------------------
  public function tin105_LogFile :: isEnabled
  -----------------------------------------------------------------------------
*/
bool tin105_LogFile :: isEnabled() const
{
  return m_bEnabled;
} // end tin105_LogFile :: isEnabled

  /* 
  -----------------------------------------------------------------------------
  protected constructor tin105_LogFile :: tin105_LogFile
  -----------------------------------------------------------------------------
*/
tin105_LogFile :: tin105_LogFile(const int nEventsToLog)
                : m_pNextFile(NULL),
                  m_nEventsToLog(nEventsToLog),
                  m_bEnabled(true),
                  m_bDestruct(false)
{

} // end tin105_LogFile :: tin105_LogFile

/* 
  -----------------------------------------------------------------------------
  protected function tin105_LogFile :: setNext
  -----------------------------------------------------------------------------
*/
void tin105_LogFile :: setNext(tin105_LogFile * pLogFile) 
{ 
  m_pNextFile = pLogFile;
} // end tin105_LogFile :: setNext

/* 
  -----------------------------------------------------------------------------
  protected function tin105_LogFile :: next
  -----------------------------------------------------------------------------
*/
tin105_LogFile * tin105_LogFile :: next() const
{ 
  return m_pNextFile;
} // end tin105_LogFile :: next

/* 
  -----------------------------------------------------------------------------
  protected function tin105_LogFile :: setToDestroy
  -----------------------------------------------------------------------------
*/
void tin105_LogFile :: setToDestroy(const bool bDestroy) 
{ 
  m_bDestruct = bDestroy;
} // end tin105_LogFile :: setToDestroy

/* 
  -----------------------------------------------------------------------------
  protected function tin105_LogFile :: isToDestroy
  -----------------------------------------------------------------------------
*/
bool tin105_LogFile :: isToDestroy() const
{ 
  return m_bDestruct;
} // end tin105_LogFile :: isToDestroy

/* 
  -----------------------------------------------------------------------------
  protected function tin105_LogFile :: writeEvents
  -----------------------------------------------------------------------------
*/
bool tin105_LogFile :: writeEvents(const SAPDBErr_MessageList & EvtLst)
{

  bool                     bReturn = true;

  if ( isEnabled () && 
       ( ( (EvtLst.Type() == SAPDBErr_MessageList::Error  ) && 
           ((m_nEventsToLog & LOG_ERR_MIN105) == LOG_ERR_MIN105          )    ) ||
         ( (EvtLst.Type() == SAPDBErr_MessageList::Warning) && 
           ((m_nEventsToLog & LOG_WRN_MIN105) == LOG_WRN_MIN105          )    ) ||
         ( (EvtLst.Type() == SAPDBErr_MessageList::Info   ) && 
           ((m_nEventsToLog & LOG_INF_MIN105) == LOG_INF_MIN105          )    )    ) ) {
    bReturn = writeOneEventList(EvtLst);
  } // end if

  return bReturn;
} // tin105_LogFile :: writeEvents

/* 
  -----------------------------------------------------------------------------
  implementation class tin105_SequentialLogFile
  -----------------------------------------------------------------------------
*/

#define LOG_TITLE1 _T("------------------------------------------------------------------------------")
#define LINE_LEN   1024
#define MAX_FILE   (800 * 1024)
#define COPY_LINES (MAX_FILE / 80 / 2)

#define LEN_YEAR   4
#define POS_YEAR   0
#define LEN_MONTH  2
#define POS_MONTH  (POS_YEAR + LEN_YEAR)
#define LEN_DAY    2
#define POS_DAY    (POS_MONTH + LEN_MONTH)
#define LEN_DSEP   2
#define LEN_DATE   (LEN_YEAR + LEN_MONTH + LEN_DAY + LEN_DSEP)

#define LEN_HOUR   2
#define POS_HOUR   2
#define LEN_MINUTE 2
#define POS_MINUTE (POS_HOUR + LEN_HOUR)
#define LEN_SECOND 2
#define POS_SECOND (POS_MINUTE + LEN_MINUTE)
#define LEN_TSEP   2
#define LEN_TIME   (LEN_HOUR + LEN_MINUTE + LEN_SECOND + LEN_TSEP)
#define OFF_TIME   2

#define LEN_TID    10
#define LEN_MSGID  10
#define LEN_COMPONENT  8

/* 
  -----------------------------------------------------------------------------
  public constructor tin105_SequentialLogFile :: tin105_SequentialLogFile
  -----------------------------------------------------------------------------
*/
tin105_SequentialLogFile :: tin105_SequentialLogFile
      (const tsp00_Pathc  szFilename, 
       SAPDBErr_MessageList * pEvtList)
       : tin105_LogFile(LOG_WRN_MIN105 + LOG_ERR_MIN105)
{
  m_szFile = szFilename;
} // end tin105_SequentialLogFile :: tin105_SequentialLogFile
/* 
  -----------------------------------------------------------------------------
*/
tin105_SequentialLogFile :: tin105_SequentialLogFile
      (const tsp00_Pathc  szFilename, 
       const int          nEventsToLog,
       SAPDBErr_MessageList * pEvtList)
       : tin105_LogFile(nEventsToLog)
{
  m_szFile = szFilename;
} // end tin105_SequentialLogFile :: tin105_SequentialLogFile
/* 
  -----------------------------------------------------------------------------
*/
tin105_SequentialLogFile :: tin105_SequentialLogFile
      (const char*  szFilename, 
       const int          nEventsToLog,
       SAPDBErr_MessageList * pEvtList)
       : tin105_LogFile(nEventsToLog)
{
  m_szFile.rawAssign(szFilename);
} // end tin105_SequentialLogFile :: tin105_SequentialLogFile
/* 
  -----------------------------------------------------------------------------
*/
tin105_SequentialLogFile :: tin105_SequentialLogFile
      (const char*  szFilename, 
       SAPDBErr_MessageList * pEvtList)
       : tin105_LogFile(LOG_WRN_MIN105 + LOG_ERR_MIN105)
{
  m_szFile.rawAssign(szFilename);
} // end tin105_SequentialLogFile :: tin105_SequentialLogFile

/* 
  -----------------------------------------------------------------------------
  public function tin105_SequentialLogFile :: checkFile
  -----------------------------------------------------------------------------
*/
bool tin105_SequentialLogFile :: checkFile
      ( ) 
{
  tsp00_Int4               hFile;
  bool                     bRc = false;
  // open file
  if (openFile(m_szFile, hFile)) {
    bRc = true;
    closeFile(hFile);
  } // end if
  return bRc;
} // end 

/* 
  -----------------------------------------------------------------------------
  protected function tin105_SequentialLogFile :: openFile
  -----------------------------------------------------------------------------
*/
bool tin105_SequentialLogFile :: openFile
      (const tsp00_Pathc  szFilename,
       tsp00_Int4       & hFile) const
{
  FUNCTION_DBG_MIN100("tin105_SequentialLogFile :: openFile");

  tsp05_RteFileInfo   fileInfo;
  tsp05_RteFileError  fileError;

  // check existence of file
  sqlfinfoc (szFilename.asCharp(), &fileInfo, &fileError);
  if (fileError.sp5fe_result != vf_ok) {
    return false;
  } // end if

  // garbage collection
  if (fileInfo.sp5fi_exists && fileInfo.sp5fi_size > MAX_FILE) {
    shrinkFile(szFilename);
  } // end if

  // open
  sqlfopenc (szFilename.asCharp(), sp5vf_text, sp5vf_append, sp5bk_disksynced, &hFile, &fileError);
  if (fileError.sp5fe_result != vf_ok) {
    return false;
  } // end if

  // write header (if new)
  if (!fileInfo.sp5fi_exists) {
    sqlfwritec (hFile, LOG_TITLE1, -1, &fileError);
    
    _TCHAR  szBuffer[LINE_LEN];
    _stprintf(szBuffer, "%-*s %-*s %-*s %-*s %-*s %-*s %s",
                        LEN_DATE, "Date",
                        LEN_TIME, "Time",
                        LEN_TID,  "TID(hex)",
                        tin105_ToShortTextConverter::getShortTextLength(), "Typ",
                        LEN_MSGID, "MsgID",
                        LEN_COMPONENT, "Label",
                        "Message");

    sqlfwritec (hFile, szBuffer, -1, &fileError);

    sqlfwritec (hFile, LOG_TITLE1, -1, &fileError);
  } // end if

  return true;
} // end tin105_SequentialLogFile :: openFile

/* 
  -----------------------------------------------------------------------------
  protected function tin105_SequentialLogFile :: closeFile
  -----------------------------------------------------------------------------
*/
void tin105_SequentialLogFile :: closeFile
      (const tsp00_Int4 hFile) const
{
  FUNCTION_DBG_MIN100("tin105_SequentialLogFile :: closeFile");

  tsp05_RteFileError   fileError;

  sqlfclosec (hFile, sp5vf_close_normal, &fileError);

} // end tin105_SequentialLogFile :: closeFile

/* 
  -----------------------------------------------------------------------------
  protected function tin105_SequentialLogFile :: shrinkFile
  -----------------------------------------------------------------------------
*/
void tin105_SequentialLogFile :: shrinkFile
      (const tsp00_Pathc  szFilename) const
{
  FUNCTION_DBG_MIN100("tin105_SequentialLogFile :: shrinkFile");

  tsp00_Int4            hSource;
  tsp00_Int4            hTarget;
  tsp00_Pathc         szTemp = szFilename;
  tsp00_Pathc         szOrg = szFilename;
  tsp01_RteError      rteError;
  tsp05_RteFileError  fileError;
  bool                bState = true;
  tsp00_Int4            nLine = 0;
  _TCHAR              szBuffer[LINE_LEN];
  tsp00_Longint         nOutSize;


  strcat(szTemp.asCharp(), ".1");

    if (sqlfilecopyc(szOrg.asCharp(), szTemp.asCharp(), &rteError)) {

    // open source
    sqlfopenc (szTemp.asCharp(), sp5vf_text, sp5vf_read, sp5bk_buffered, &hSource, &fileError);
    bState = (fileError.sp5fe_result == vf_ok);

    // open target
    if (bState) {
      sqlfopenc (szOrg.asCharp(), sp5vf_text, sp5vf_write, sp5bk_buffered, &hTarget, &fileError);
      bState = (fileError.sp5fe_result == vf_ok);
    } // end if

    // copy file
    while (bState) {
      sqlfreadc (hSource, szBuffer, LINE_LEN, &nOutSize, &fileError);
      bState = (fileError.sp5fe_result == vf_ok);
      ++nLine;
      if (bState && (nLine < 4 || nLine > COPY_LINES)) {
        sqlfwritec (hTarget, szBuffer, -1, &fileError);
        bState = (fileError.sp5fe_result == vf_ok);
      } // end if
    } // end while

    sqlfclosec (hTarget, sp5vf_close_normal, &fileError);
    bState = (fileError.sp5fe_result == vf_ok);
    sqlfclosec (hSource, sp5vf_close_normal, &fileError);
    
    // restore source
    if (!bState) {
      sqlfilecopyc(szTemp.asCharp(), szOrg.asCharp(), &rteError);
      sqlferasec(szTemp, &fileError);
    } // end if
    sqlferasec(szTemp, &fileError);

  } // end if
  

} // end tin105_SequentialLogFile :: shrinkFile

/* 
  -----------------------------------------------------------------------------
  protected function tin105_SequentialLogFile :: writeOneEventList
  -----------------------------------------------------------------------------
*/
bool tin105_SequentialLogFile :: writeOneEventList
      ( const SAPDBErr_MessageList & EvtLst )
{
  const SAPDBErr_MessageList * pNextEvt = &EvtLst;
  _TCHAR                   szBuffer[LINE_LEN];
  _TCHAR                 * pBuffer;
  bool                     bStatus = true;
  tsp05_RteFileError       fileError;
  SAPDBErr_MessageList     fileEvent;
  tsp00_Int4               hFile;
  char                     szDateTime[LEN_DATE + LEN_TIME + 5];

  // open file
  if (openFile(m_szFile, hFile)) {

    tsp00_Date         DateP;
    tsp00_Time         TimeP;
    sqldattime(DateP, TimeP);

    _stprintf(szDateTime, "%.*s-%.*s-%.*s %.*s:%.*s:%.*s",
                           LEN_YEAR,   &(DateP[POS_YEAR])   ,
                           LEN_MONTH,  &(DateP[POS_MONTH])  ,
                           LEN_DAY,    &(DateP[POS_DAY])    ,
                           LEN_HOUR,   &(TimeP[POS_HOUR])   ,
                           LEN_MINUTE, &(TimeP[POS_MINUTE]) ,
                           LEN_SECOND, &(TimeP[POS_SECOND]));

    while (pNextEvt != NULL) {
      tsp00_Int4 nPid;
      sqlgetpid(&nPid);

      _stprintf(szBuffer, "%-*s %#0*x %-*s %*d %-*s ",
                          LEN_DATE + LEN_TIME + 1, szDateTime,
                          LEN_TID, (int) nPid,
                          tin105_ToShortTextConverter::getShortTextLength(), tin105_ToShortTextConverter::getShortText(pNextEvt->Type()),
                          LEN_MSGID, (int) pNextEvt->ID(),
                          LEN_COMPONENT, pNextEvt->Component()); //@TODO msg_list: length of component

      pBuffer = &szBuffer[_tcslen(szBuffer)];
      _tcsncpy(pBuffer, pNextEvt->Message(), LINE_LEN -_tcslen(szBuffer) - 1);
      szBuffer[LINE_LEN - 1] = '\0';

      pBuffer = strstr(szBuffer, "\n");
      if (pBuffer != NULL) {
        *pBuffer = '\0';
      } // end if

      sqlfwritec (hFile, &szBuffer[0], -1, &fileError);

      bStatus = bStatus && (fileError.sp5fe_result == vf_ok);

      strcpy(szDateTime, "");
    
      pNextEvt  = pNextEvt->NextMessage();
    } // end while

    closeFile(hFile);
  } // end if
    
  return bStatus;
} // end tin105_SequentialLogFile :: writeOneEventList

const char* tin105_ToShortTextConverter::getShortText(
        const SAPDBErr_MessageList::MessageType msgType ) {

    static const char * const shortText[4] = { "UNK", "ERR", "WRN", "INF" };

    switch( msgType ) {
        case SAPDBErr_MessageList::Error:
            return shortText[1];
        case SAPDBErr_MessageList::Warning:
            return shortText[2];
        case SAPDBErr_MessageList::Info:
            return shortText[3];
        default:
            return shortText[0];
    }
}

size_t tin105_ToShortTextConverter::getShortTextLength() {
    return 3;
}
