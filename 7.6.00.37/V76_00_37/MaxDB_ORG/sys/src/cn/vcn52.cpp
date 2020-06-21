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
 
  module: vcn52.cpp
 
  -----------------------------------------------------------------------------
 
  responsible:  BerndV
 
  special area: DBMServer Parameter History Class
   
  description:  DBMServer Parameter History Class - Implementation

  version:      7.2.

  -----------------------------------------------------------------------------
 
                          Copyright (c) 1998-2005 SAP AG
 
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
 */
#include <stdlib.h>
#include <stdio.h>

#include "heo02.h"
#include "heo06.h"
#include "hcn42.h"
#include "hcn90.h"
#include "hcn52.h"

#include "DBM/Srv/Message/DBMSrvMsg_RTEError.hpp"
#include "DBM/Srv/Message/DBMSrvMsg_NewRTEError.hpp"


/*
  -----------------------------------------------------------------------------
  private macros
  -----------------------------------------------------------------------------
 */
#define TXT_VALUE_DELETED "<<parameter inactive>>"
#define TXT_STATE_DELETED "D"
#define TXT_STATE_CHANGED "C"
#define TXT_STATE_ACTIVE  "A"
#define TXT_STATE_UNKNOWN " "

#define OFF_DATE            0
#define LEN_DATE            9
#define OFF_TIME            (OFF_DATE + LEN_DATE)
#define LEN_TIME            9
#define OFF_NAME            (OFF_TIME + LEN_TIME)
#define LEN_NAME           33
#define OFF_NEWVALUE        (OFF_NAME + LEN_NAME)
#define LEN_NEWVALUE       65
#define OFF_OLDVALUE        (OFF_NEWVALUE + LEN_NEWVALUE)
#define LEN_OLDVALUE       65
#define OFF_STATE           (OFF_OLDVALUE + LEN_OLDVALUE)
#define LEN_STATE           2
#define OFF_GROUP           (OFF_STATE + LEN_STATE)
#define LEN_GROUP           9
#define OFF_COMMENT         (OFF_GROUP + LEN_GROUP)
#define LEN_COMMENT        81
#define LEN_RECORD_VXX      (OFF_COMMENT + LEN_COMMENT)

#define OFF_DATE_V02            0
#define LEN_DATE_V02            9
#define OFF_TIME_V02            (OFF_DATE + LEN_DATE)
#define LEN_TIME_V02            9
#define OFF_NAME_V02            (OFF_TIME + LEN_TIME)
#define LEN_NAME_V02           33
#define OFF_NEWVALUE_V02        (OFF_NAME + LEN_NAME)
#define LEN_NEWVALUE_V02       65
#define OFF_OLDVALUE_V02        (OFF_NEWVALUE + LEN_NEWVALUE)
#define LEN_OLDVALUE_V02       65
#define OFF_STATE_V02           (OFF_OLDVALUE + LEN_OLDVALUE)
#define LEN_STATE_V02           2
#define OFF_GROUP_V02           (OFF_STATE + LEN_STATE)
#define LEN_GROUP_V02           9
#define LEN_RECORD_V02          (OFF_GROUP_V02 + LEN_GROUP_V02)

#define OFF_DATE_V01            0
#define LEN_DATE_V01            9
#define OFF_TIME_V01            (OFF_DATE_V01 + LEN_DATE_V01)
#define LEN_TIME_V01            9
#define OFF_NAME_V01            (OFF_TIME_V01 + LEN_TIME_V01)
#define LEN_NAME_V01           19
#define OFF_NEWVALUE_V01        (OFF_NAME_V01 + LEN_NAME_V01)
#define LEN_NEWVALUE_V01       65
#define OFF_OLDVALUE_V01        (OFF_NEWVALUE_V01 + LEN_NEWVALUE_V01)
#define LEN_OLDVALUE_V01       65
#define OFF_STATE_V01           (OFF_OLDVALUE_V01 + LEN_OLDVALUE_V01)
#define LEN_STATE_V01           2
#define OFF_GROUP_V01           (OFF_STATE_V01 + LEN_STATE_V01)
#define LEN_GROUP_V01           9
#define LEN_RECORD_V01          (OFF_GROUP_V01 + LEN_GROUP_V01)

#define TXT_DATE            "DATE"
#define ID_DATE             1
#define TXT_TIME            "TIME"
#define ID_TIME             2
#define TXT_NAME            "NAME"
#define ID_NAME             4
#define TXT_NEWVALUE        "NEWVALUE"
#define ID_NEWVALUE         8
#define TXT_OLDVALUE        "OLDVALUE"
#define ID_OLDVALUE         16
#define TXT_STATE           "S"
#define TXT_STATEFULL       "STATE"
#define ID_STATE            32
#define TXT_GROUP           "GROUP"
#define ID_GROUP            64
#define TXT_COMMENT         "COMMENT"
#define ID_COMMENT          128

#define ID_FULL             (ID_DATE + ID_TIME + ID_NAME + ID_NEWVALUE + ID_OLDVALUE + ID_STATE + ID_GROUP + ID_COMMENT)
#define ID_EMPTY            0

#ifdef _WIN32
#define LEN_RECORD          (LEN_RECORD_VXX + 2)
#else
#define LEN_RECORD          (LEN_RECORD_VXX + 1)
#endif

#define KEY_NAME            "NAME"
#define KEY_GROUP           "GROUP"
#define KEY_FIELDS          "FIELDS"
#define KEY_DATE            "DATE"
#define KEY_STATE           "STATE"

#define CHAR_ASSIGN         '='
#define CHAR_DELIMETER      ','
#define TXT_MARK            "%"

/*
  -------------------------------------------------------------------------
  public constructor cn52ParamHistoryRecord :: cn52ParamHistoryRecord
  -------------------------------------------------------------------------
*/
cn52ParamHistoryRecord :: cn52ParamHistoryRecord
                           ( const tsp00_DbNamec        & szDBName,
                             const tcn002_XpValueName   & szName,
                             const tcn002_XpValueString & szNewValue,
                             const tcn002_XpValueString & szOldValue,
                             const tsp00_C8c            & szGroup,
                             const tsp00_C80c           & szComment,
                             const HistoryState           nState) 
{
  initMembers();

  m_szName     = szName;
  m_szNewValue = szNewValue;
  m_szOldValue = szOldValue;
  m_nState     = nState;
  m_szDBName   = szDBName;
  m_szGroup    = szGroup;
  m_szComment  = szComment;

  tsp00_Date         DateP;
	tsp00_Time         TimeP;
  sqldattime(DateP, TimeP);
  m_szDate.p2c(DateP);
  m_szTime.p2c(TimeP);

  switch (m_nState) {
    case StDeleted:
      m_szState.rawAssign(TXT_STATE_DELETED);
      break;
    case StChanged:
      m_szState.rawAssign(TXT_STATE_CHANGED);
      break;
    case StActive:
      m_szState.rawAssign(TXT_STATE_ACTIVE);
      break;
    default:
      m_szState.rawAssign(TXT_STATE_UNKNOWN);
  } // end switch

  if (m_nState == StDeleted) {
    m_szNewValue.rawAssign(TXT_VALUE_DELETED);
  } // end if 

} // end cn52ParamHistoryRecord :: cn52ParamHistoryRecord
/*
  -------------------------------------------------------------------------
*/
cn52ParamHistoryRecord :: cn52ParamHistoryRecord
                           ( const tsp00_DbNamec    & szDBName ) 
{
  initMembers();

  m_szDBName   = szDBName;
} // end cn52ParamHistoryRecord :: cn52ParamHistoryRecord

/*
  -------------------------------------------------------------------------
  public destructor cn52ParamHistoryRecord :: ~cn52ParamHistoryRecord
  -------------------------------------------------------------------------
*/
cn52ParamHistoryRecord :: ~cn52ParamHistoryRecord()
{
  if (m_bFileOpen) {
    tsp05_RteFileError rteFileErr;
    sqlfclosec ( m_hFile, sp5vf_close_normal, &rteFileErr );
  } // end if
} // end cn52ParamHistoryRecord :: ~cn52ParamHistoryRecord

/*
  -------------------------------------------------------------------------
  private member function cn52ParamHistoryRecord :: migrateFile
  -------------------------------------------------------------------------
*/
bool cn52ParamHistoryRecord :: migrateFile(DBMSrvMsg_Error & oError)
{
  oError.ClearMessageList();

  bool bOk    = true;
  bool bValid = true;

  tsp05_RteFileError rteFileErr;
  tsp01_RteError     rteRTEError;
  tsp05_RteFileInfo  rteFileInfo;
  tsp00_Pathc        szFile;
  tsp00_Pathc        szTemp1;
  tsp00_Pathc        szTemp2;
  tsp00_Int4         hFile;
  tsp00_Int4         hTemp1;

  tcn52_C1024c       szLine;
  tcn52_C1024c       szOrgRecord;
  tsp00_Longint      nOut;
  tsp00_Longint      nOrgLen;

  // save original record
  szOrgRecord = getRecord();

  bOk = (cn42GetFileName(m_szDBName, FGET_DBMPAHI_CN42, szFile) == OK_CN00);

  sqlfinfoc (szFile, &rteFileInfo, &rteFileErr );
  if (!rteFileInfo.sp5fi_exists) {
    return true;
  } // end if

  if (bOk) {
    szTemp1 = szFile;
    strcat(szTemp1, "1");
    szTemp2 = szFile;
    strcat(szTemp2, "2");

    sqlfopenc(szFile, sp5vf_text, sp5vf_read, sp5bk_buffered, &hFile, &rteFileErr );
    bOk = (rteFileErr.sp5fe_result == vf_ok);
  } // end if

  if (bOk) {
    // read the record
    sqlfreadc (hFile, szLine.asCharp(), szLine.size(), &nOrgLen, &rteFileErr);

    if (nOrgLen == LEN_RECORD_VXX) {
      sqlfclosec ( hFile, sp5vf_close_normal, &rteFileErr );
      return true;
    } // end if

    sqlfopenc(szTemp1, sp5vf_text, sp5vf_write, sp5bk_buffered, &hTemp1, &rteFileErr );
    bOk = (rteFileErr.sp5fe_result == vf_ok);
  } // end if

  while (bOk) {

    bValid = true;
    if (nOrgLen == LEN_RECORD_V01) {
      analyzeRecV01(szLine);
    } else if (nOrgLen == LEN_RECORD_V02) {
      analyzeRecV02(szLine);
    } else {
      // line maybe corrupted -> ignore it
      bValid = false;
    } // end if

    if (bValid) {
      // write file
      sqlfwritec (hTemp1, getRecord().asCharp(), -1, &rteFileErr);
      bOk = (rteFileErr.sp5fe_result == vf_ok);
    } // end if

    if (bOk) {
      sqlfreadc (hFile, szLine.asCharp(), szLine.size(), &nOut, &rteFileErr);
      bOk = (rteFileErr.sp5fe_result == vf_ok);
    } // end if

  } // end while

  bOk =  (rteFileErr.sp5fe_result == vf_eof) || (rteFileErr.sp5fe_result == vf_ok);

  sqlfclosec ( hFile,  sp5vf_close_normal, &rteFileErr );
  sqlfclosec ( hTemp1, sp5vf_close_normal, &rteFileErr );

  if (bOk) {
    if (sqlfilecopyc(szFile, szTemp2, &rteRTEError)) {
      if (!sqlfilecopyc(szTemp1, szFile, &rteRTEError)) {
        bOk = false;
        oError = DBMSrvMsg_NewRTEError(rteRTEError);
        sqlfilecopyc(szTemp2, szFile, &rteRTEError);
      } // end if
    } else {
      bOk = false;
      oError = DBMSrvMsg_NewRTEError(rteRTEError);
    } // end if
  } else {
    oError = DBMSrvMsg_RTEError(rteFileErr);
  } // end if

  sqlferasec(szTemp1, &rteFileErr);
  sqlferasec(szTemp2, &rteFileErr);

  // restore original record
  analyzeRecord(szOrgRecord);

  return bOk;
} /// end cn52ParamHistoryRecord :: migrateFile

/*
  -------------------------------------------------------------------------
  public member function cn52ParamHistoryRecord :: appendToFile
  -------------------------------------------------------------------------
*/
bool cn52ParamHistoryRecord :: appendToFile(DBMSrvMsg_Error & oError)
{
    oError.ClearMessageList();

    tsp00_Int4         hFile;
    tsp05_RteFileError rteFileErr;
    tsp00_Pathc        szFileName;

    if (!migrateFile(oError)) 
        return false;

    if (!cn42GetFileName( m_szDBName, FGET_DBMPAHI_CN42, szFileName, oError))
        return false;

    sqlfopenc(szFileName, sp5vf_text, sp5vf_append, sp5bk_buffered, &hFile, &rteFileErr );
    if (rteFileErr.sp5fe_result != vf_ok) {
        oError = DBMSrvMsg_RTEError(rteFileErr);
        return false;
    } // end if

    // write file
    sqlfwritec (hFile, getRecord().asCharp(), -1, &rteFileErr);

    // close file on error
    if (rteFileErr.sp5fe_result != vf_ok) {
        oError = DBMSrvMsg_RTEError(rteFileErr);
        sqlfclosec ( hFile, sp5vf_close_normal, &rteFileErr );
        return false;
    } // end if

    // close file on success
    sqlfclosec ( hFile, sp5vf_close_normal, &rteFileErr );
    if (rteFileErr.sp5fe_result != vf_ok) {
        oError = DBMSrvMsg_RTEError(rteFileErr);
        return false;
    } // end if

    return true;
} // end cn52ParamHistoryRecord :: appendToFile

/*
  -------------------------------------------------------------------------
  public member function cn52ParamHistoryRecord :: readFirst
  -------------------------------------------------------------------------
*/
bool cn52ParamHistoryRecord :: readFirst
  ( DBMSrvMsg_Error & oError, 
    bool            & bEnd ) 
{
    oError.ClearMessageList();
    bEnd = false;

    tsp05_RteFileError rteFileErr;
    tsp00_Pathc        szFileName;

    for (int i = 0;i < MAX_KEYS_CN52; ++i) m_NameArray[i].Init(); 

    if (m_bFileOpen) {
        sqlfclosec ( m_hFile, sp5vf_close_normal, &rteFileErr );
        m_bFileOpen = false;
    } // end if

    if (!migrateFile(oError))
        return false;

    if (!cn42GetFileName( m_szDBName, FGET_DBMPAHI_CN42, szFileName, oError))
        return false;

    sqlfopenc(szFileName, sp5vf_text, sp5vf_read, sp5bk_buffered, &m_hFile, &rteFileErr );
    if (rteFileErr.sp5fe_result != vf_ok) {
        oError = DBMSrvMsg_RTEError(rteFileErr);
        return false;
    } // end if

    m_bFileOpen = true;

    sqlfseekc (m_hFile, LEN_RECORD * (-1),  sp5vf_seek_end, &rteFileErr);
    if (rteFileErr.sp5fe_result != vf_ok) {
        if (rteFileErr.sp5fe_result != vf_noseek) {
            oError = DBMSrvMsg_RTEError(rteFileErr);
        } else {
            bEnd = true;
        } // end if
    } // end if

    tcn52_C1024c       szRecord;
    tsp00_Longint      nOut;

    while ((rteFileErr.sp5fe_result == vf_ok) && !bEnd) {
                
        // read the record
        sqlfreadc (m_hFile, szRecord.asCharp(), szRecord.size(), &nOut, &rteFileErr);
        if (rteFileErr.sp5fe_result != vf_ok) {
            oError = DBMSrvMsg_RTEError(rteFileErr);
            break;
        } // end if

         // analayze the record
         if (analyzeRecord (szRecord))
            break;

         // go to next (that means previuos) record
         sqlfseekc (m_hFile, LEN_RECORD * (-1) * 2,  sp5vf_seek_cur, &rteFileErr);
        if (rteFileErr.sp5fe_result != vf_ok) {
            if (rteFileErr.sp5fe_result != vf_noseek) {
                oError = DBMSrvMsg_RTEError(rteFileErr);
            } else {
                bEnd = true;
            } // end if
            break;
        } // end if
   
    } // end while

    if ( rteFileErr.sp5fe_result != vf_ok) {
        sqlfclosec ( m_hFile, sp5vf_close_normal, &rteFileErr );
        m_bFileOpen = false;
        if (!bEnd) 
            return false;
    } // end if

    return true;
} // end cn52ParamHistoryRecord :: readFirst

/*
  -------------------------------------------------------------------------
  public member function cn52ParamHistoryRecord :: readNext
  -------------------------------------------------------------------------
*/
bool cn52ParamHistoryRecord :: readNext
  ( DBMSrvMsg_Error & oError, 
    bool            & bEnd ) 
{
    oError.ClearMessageList();

    bool               bOk = false;
    bEnd = false;

    if (m_bFileOpen) {

        tsp05_RteFileError rteFileErr;

        // go to next (that means previuos) record
        sqlfseekc (m_hFile, LEN_RECORD * (-1) * 2,  sp5vf_seek_cur, &rteFileErr);
        if (rteFileErr.sp5fe_result != vf_ok) {
            if (rteFileErr.sp5fe_result != vf_noseek) {
                oError = DBMSrvMsg_RTEError(rteFileErr);
            } else {
                bEnd = true;
            } // end if
        } // end if

        tcn52_C1024c       szRecord;
        tsp00_Longint      nOut;

        while ((rteFileErr.sp5fe_result == vf_ok) && !bEnd) {
                    
            // read the record
            sqlfreadc (m_hFile, szRecord.asCharp(), szRecord.size(), &nOut, &rteFileErr);
            if (rteFileErr.sp5fe_result != vf_ok) {
                oError = DBMSrvMsg_RTEError(rteFileErr);
                break;
            } // end if

            // analayze the record
            if (analyzeRecord (szRecord))
                break;

            // go to next (that means previuos) record
            sqlfseekc (m_hFile, LEN_RECORD * (-1) * 2,  sp5vf_seek_cur, &rteFileErr);
            if (rteFileErr.sp5fe_result != vf_ok) {
                if (rteFileErr.sp5fe_result != vf_noseek) {
                    oError = DBMSrvMsg_RTEError(rteFileErr);
                } else {
                    bEnd = true;
                } // end if
                break;
            } // end if

        } // end while

        if ( rteFileErr.sp5fe_result != vf_ok) {
            sqlfclosec ( m_hFile, sp5vf_close_normal, &rteFileErr );
            m_bFileOpen = false;
            if (!bEnd) 
                return false;
        } // end if

    } // end if

    return true;
} // end cn52ParamHistoryRecord :: readNext

/*
  -------------------------------------------------------------------------
  public member function cn52ParamHistoryRecord :: getRecord
  -------------------------------------------------------------------------
*/
const tcn52_C1024c & cn52ParamHistoryRecord :: getRecord() 
{
  // create record     DATE  TIME  NAME  NEW   OLD   STATE GROUP COMM
  sprintf(m_szRecord, "%-*.*s%-*.*s%-*.*s%-*.*s%-*.*s%-*.*s%-*.*s%-*.*s", 
                      ((m_nFields & ID_DATE)     == 0) ? 0  : LEN_DATE,
                      ((m_nFields & ID_DATE)     == 0) ? 0  : LEN_DATE - 1,
                      ((m_nFields & ID_DATE)     == 0) ? "" : m_szDate.asCharp(),
                      ((m_nFields & ID_TIME)     == 0) ? 0  : LEN_TIME,
                      ((m_nFields & ID_TIME)     == 0) ? 0  : LEN_TIME - 1,
                      ((m_nFields & ID_TIME)     == 0) ? "" : m_szTime.asCharp(),
                      ((m_nFields & ID_NAME)     == 0) ? 0  : LEN_NAME,
                      ((m_nFields & ID_NAME)     == 0) ? 0  : LEN_NAME - 1,
                      ((m_nFields & ID_NAME)     == 0) ? "" : m_szName.asCharp(),
                      ((m_nFields & ID_NEWVALUE) == 0) ? 0  : LEN_NEWVALUE,
                      ((m_nFields & ID_NEWVALUE) == 0) ? 0  : LEN_NEWVALUE - 1,
                      ((m_nFields & ID_NEWVALUE) == 0) ? "" : m_szNewValue.asCharp(),
                      ((m_nFields & ID_OLDVALUE) == 0) ? 0  : LEN_OLDVALUE,
                      ((m_nFields & ID_OLDVALUE) == 0) ? 0  : LEN_OLDVALUE - 1,
                      ((m_nFields & ID_OLDVALUE) == 0) ? "" : m_szOldValue.asCharp(),
                      ((m_nFields & ID_STATE)    == 0) ? 0  : LEN_STATE,
                      ((m_nFields & ID_STATE)    == 0) ? 0  : LEN_STATE - 1,
                      ((m_nFields & ID_STATE)    == 0) ? "" : m_szState.asCharp(),
                      ((m_nFields & ID_GROUP)    == 0) ? 0  : LEN_GROUP,
                      ((m_nFields & ID_GROUP)    == 0) ? 0  : LEN_GROUP - 1,
                      ((m_nFields & ID_GROUP)    == 0) ? "" : m_szGroup.asCharp(),
                      ((m_nFields & ID_COMMENT)  == 0) ? 0  : LEN_COMMENT,
                      ((m_nFields & ID_COMMENT)  == 0) ? 0  : LEN_COMMENT - 1,
                      ((m_nFields & ID_COMMENT)  == 0) ? "" : m_szComment.asCharp() );

    return m_szRecord;
} // end cn52ParamHistoryRecord :: getRecord

/*
  -------------------------------------------------------------------------
  public member function cn52ParamHistoryRecord :: getHeader
  -------------------------------------------------------------------------
*/
const tcn52_C1024c & cn52ParamHistoryRecord :: getHeader() 
{
 
  // create record     DAT TIM NAM NEW OLD STA GRP COM
  sprintf(m_szRecord, "%-*s%-*s%-*s%-*s%-*s%-*s%-*s%-*s", 
                        ((m_nFields & ID_DATE)     == 0) ? 0  : LEN_DATE,
                        ((m_nFields & ID_DATE)     == 0) ? "" : TXT_DATE,
                        ((m_nFields & ID_TIME)     == 0) ? 0  : LEN_TIME,
                        ((m_nFields & ID_TIME)     == 0) ? "" : TXT_TIME,
                        ((m_nFields & ID_NAME)     == 0) ? 0  : LEN_NAME,
                        ((m_nFields & ID_NAME)     == 0) ? "" : TXT_NAME,
                        ((m_nFields & ID_NEWVALUE) == 0) ? 0  : LEN_NEWVALUE,
                        ((m_nFields & ID_NEWVALUE) == 0) ? "" : TXT_NEWVALUE,
                        ((m_nFields & ID_OLDVALUE) == 0) ? 0  : LEN_OLDVALUE,
                        ((m_nFields & ID_OLDVALUE) == 0) ? "" : TXT_OLDVALUE,
                        ((m_nFields & ID_STATE)    == 0) ? 0  : LEN_STATE,
                        ((m_nFields & ID_STATE)    == 0) ? "" : TXT_STATE,
                        ((m_nFields & ID_GROUP)    == 0) ? 0  : LEN_GROUP,
                        ((m_nFields & ID_GROUP)    == 0) ? "" : TXT_GROUP,
                        ((m_nFields & ID_COMMENT)  == 0) ? 0  : LEN_COMMENT,
                        ((m_nFields & ID_COMMENT)  == 0) ? "" : TXT_COMMENT);

  return m_szRecord;
} // end cn52ParamHistoryRecord :: getHeader

/*
  -------------------------------------------------------------------------
  public member function cn52ParamHistoryRecord :: setSelection
  -------------------------------------------------------------------------
*/
bool cn52ParamHistoryRecord :: setSelection(const tcn52_C1024c & aSelection) 
{
  tcn52_C1024c  szSelection = aSelection;
  bool          bOk         = true;
  _TCHAR        szToken[1024];
  _TCHAR      * pValue      = NULL;
  long          nToken      = 1;

  // init selection members
  m_szSelName.Init();
  m_szSelDate.Init();
  m_szSelGroup.Init();
  m_szSelState.Init();
  m_nFields = ID_FULL;

  // analyze tokens
  while (cn90GetToken(szSelection, szToken, nToken, 1024) && bOk) {
    pValue = _tcschr(szToken, CHAR_ASSIGN);

    if (pValue != NULL) {
      *pValue = CHAR_STRINGTERM_CN90;
      pValue++;

      if        ( _tcsicmp(szToken, KEY_NAME    ) == 0 ) {
        m_szSelName.rawAssign(pValue);
      } else if ( _tcsicmp(szToken, KEY_DATE    ) == 0 ) {
        m_szSelDate.rawAssign(pValue);
      } else if ( _tcsicmp(szToken, KEY_GROUP   ) == 0 ) {
        bOk = readStrings(pValue, m_szSelGroup);
      } else if ( _tcsicmp(szToken, KEY_STATE   ) == 0 ) {
        bOk = readStrings(pValue, m_szSelState);
      } else if ( _tcsicmp(szToken, KEY_FIELDS  ) == 0 ) { 
        bOk = readFields(pValue, m_nFields);
      } else {
        bOk = false;
      } // end if    

    } // end if

    nToken++;
  } // end while
  
  return bOk;
} // end cn52ParamHistoryRecord :: setSelection

/*
  -------------------------------------------------------------------------
  protected function cn52ParamHistoryRecord :: readFields
  -------------------------------------------------------------------------
*/
bool cn52ParamHistoryRecord :: readFields
                           ( char         * pValue,
                             tsp00_Int4   & nFields) const
{
  _TCHAR   * pField     = pValue;
  _TCHAR   * pNextField = NULL;
  bool       bOk        = true;

  nFields = ID_EMPTY;

  while ((pField != NULL) && bOk) {

    // Terminate right entry
    pNextField = _tcschr(pField, CHAR_DELIMETER);
    if (pNextField != NULL) {
      *pNextField = CHAR_STRINGTERM_CN90;
      pNextField++;
    } // end if

    if        ( _tcsicmp(pField, TXT_DATE    ) == 0 ) {
      nFields = nFields + ID_DATE;
    } else if   ( _tcsicmp(pField, TXT_TIME    ) == 0 ) {
      nFields = nFields + ID_TIME;
    } else if   ( _tcsicmp(pField, TXT_NAME    ) == 0 ) {
      nFields = nFields + ID_NAME;
    } else if   ( _tcsicmp(pField, TXT_NEWVALUE) == 0 ) {
      nFields = nFields + ID_NEWVALUE;
    } else if   ( _tcsicmp(pField, TXT_OLDVALUE) == 0 ) {
      nFields = nFields + ID_OLDVALUE;
    } else if   ( _tcsicmp(pField, TXT_STATEFULL) == 0 ) {
      nFields = nFields + ID_STATE;
    } else if   ( _tcsicmp(pField, TXT_GROUP   ) == 0 ) {
      nFields = nFields + ID_GROUP;
    } else if   ( _tcsicmp(pField, TXT_COMMENT   ) == 0 ) {
      nFields = nFields + ID_COMMENT;
    } else {
      bOk = false;
    } // end if

    pField = pNextField;
  } // end while

  return bOk;

} // end cn52ParamHistoryRecord :: readFields

/*
  -------------------------------------------------------------------------
  protected function cn52ParamHistoryRecord :: readStrings
  -------------------------------------------------------------------------
*/
bool cn52ParamHistoryRecord :: readStrings
                           ( char         * pValue,
                             tcn52_C1024c & szString ) const
{
  _TCHAR   * pString     = pValue;
  _TCHAR   * pNextString = NULL;
  bool       bOk         = true;

  szString.Init();
  
  while ((pString != NULL) && bOk) {

    // Terminate right entry
    pNextString = _tcschr(pString, CHAR_DELIMETER);
    if (pNextString != NULL) {
      *pNextString = CHAR_STRINGTERM_CN90;
      pNextString++;
    } // end if

    strcat(szString, TXT_MARK);
    strcat(szString, pString);
    strcat(szString, TXT_MARK);

    pString = pNextString;
  } // end while

  return bOk;
} // end cn52ParamHistoryRecord :: readStrings

/*
  -------------------------------------------------------------------------
  protected function cn52ParamHistoryRecord :: analyzeRecord
  -------------------------------------------------------------------------
*/
bool cn52ParamHistoryRecord :: analyzeRecord
                           ( tcn52_C1024c & szRecord ) 
{
  bool bMatch = true;
  
  szRecord[OFF_DATE + LEN_DATE - 1]     = 0;
  szRecord[OFF_TIME + LEN_TIME - 1]     = 0;
  szRecord[OFF_NAME + LEN_NAME - 1]     = 0;
  szRecord[OFF_NEWVALUE + LEN_NEWVALUE - 1] = 0;
  szRecord[OFF_OLDVALUE + LEN_OLDVALUE - 1] = 0;
  szRecord[OFF_STATE + LEN_STATE - 1] = 0;

  m_szDate.rawAssign(cn90Strip(&szRecord[OFF_DATE]));
  m_szTime.rawAssign(cn90Strip(&szRecord[OFF_TIME]));
  m_szName.rawAssign(cn90Strip(&szRecord[OFF_NAME]));
  m_szNewValue.rawAssign(cn90Strip(&szRecord[OFF_NEWVALUE]));
  m_szOldValue.rawAssign(cn90Strip(&szRecord[OFF_OLDVALUE]));
  m_szState.rawAssign(cn90Strip(&szRecord[OFF_STATE]));
  m_szGroup.rawAssign(cn90Strip(&szRecord[OFF_GROUP]));
  m_szComment.rawAssign(cn90Strip(&szRecord[OFF_COMMENT]));

  if (_tcsicmp(m_szState, TXT_STATE_DELETED) == 0) {
    m_nState = StDeleted;
  } else if (_tcsicmp(m_szState, TXT_STATE_CHANGED) == 0) {
    m_nState = StChanged;
  } else if (_tcsicmp(m_szState, TXT_STATE_ACTIVE) == 0) {
    m_nState = StActive;
  } else {
    m_nState = StUnknown;
  } // end if

  if (!checkNameArray(m_szName)) {
    if (m_nState == StChanged) {
      m_nState = StActive;
      m_szState.rawAssign(TXT_STATE_ACTIVE);
    } // end if
  } // end if

  // check selcetion criteria
  if (m_szSelName.length() > 0) {
    bMatch = bMatch && (_tcsicmp(m_szSelName, m_szName) == 0);
  } // end if
  if (m_szSelDate.length() > 0) {
    bMatch = bMatch && (_tcsicmp(m_szSelDate, m_szDate) <= 0);
  } // end if
  if (m_szSelGroup.length() > 0) {
    tcn52_C1024c szLocal;
    szLocal.Init();
    strcat(szLocal, TXT_MARK);
    strcat(szLocal, m_szGroup);
    strcat(szLocal, TXT_MARK);
    bMatch = bMatch && (strstr(m_szSelGroup, szLocal) != NULL);
  } // end if
  if (m_szSelState.length() > 0) {
    tcn52_C1024c szLocal;
    szLocal.Init();
    strcat(szLocal, TXT_MARK);
    strcat(szLocal, m_szState);
    strcat(szLocal, TXT_MARK);
    bMatch = bMatch && (strstr(m_szSelState, szLocal) != NULL);
  } // end if

  return bMatch;
} // end cn52ParamHistoryRecord :: analyzeRecord

/*
  -------------------------------------------------------------------------
  protected function cn52ParamHistoryRecord :: analyzeRecV01
  -------------------------------------------------------------------------
*/
void cn52ParamHistoryRecord :: analyzeRecV01
                           ( tcn52_C1024c & szRecord ) 
{
  szRecord[OFF_DATE_V01     + LEN_DATE_V01     - 1] = 0;
  szRecord[OFF_TIME_V01     + LEN_TIME_V01     - 1] = 0;
  szRecord[OFF_NAME_V01     + LEN_NAME_V01     - 1] = 0;
  szRecord[OFF_NEWVALUE_V01 + LEN_NEWVALUE_V01 - 1] = 0;
  szRecord[OFF_OLDVALUE_V01 + LEN_OLDVALUE_V01 - 1] = 0;
  szRecord[OFF_STATE_V01    + LEN_STATE_V01    - 1] = 0;

  m_szDate.rawAssign(cn90Strip(&szRecord[OFF_DATE_V01]));
  m_szTime.rawAssign(cn90Strip(&szRecord[OFF_TIME_V01]));
  m_szName.rawAssign(cn90Strip(&szRecord[OFF_NAME_V01]));
  m_szNewValue.rawAssign(cn90Strip(&szRecord[OFF_NEWVALUE_V01]));
  m_szOldValue.rawAssign(cn90Strip(&szRecord[OFF_OLDVALUE_V01]));
  m_szState.rawAssign(cn90Strip(&szRecord[OFF_STATE_V01]));
  m_szGroup.rawAssign(cn90Strip(&szRecord[OFF_GROUP_V01]));
  m_szComment.rawAssign("");

  if (_tcsicmp(m_szState, TXT_STATE_DELETED) == 0) {
    m_nState = StDeleted;
  } else if (_tcsicmp(m_szState, TXT_STATE_CHANGED) == 0) {
    m_nState = StChanged;
  } else if (_tcsicmp(m_szState, TXT_STATE_ACTIVE) == 0) {
    m_nState = StActive;
  } else {
    m_nState = StUnknown;
  } // end if
} // end cn52ParamHistoryRecord :: analyzeRecV01

/*
  -------------------------------------------------------------------------
  protected function cn52ParamHistoryRecord :: analyzeRecV02
  -------------------------------------------------------------------------
*/
void cn52ParamHistoryRecord :: analyzeRecV02
                           ( tcn52_C1024c & szRecord ) 
{
  szRecord[OFF_DATE_V02     + LEN_DATE_V02     - 1] = 0;
  szRecord[OFF_TIME_V02     + LEN_TIME_V02     - 1] = 0;
  szRecord[OFF_NAME_V02     + LEN_NAME_V02     - 1] = 0;
  szRecord[OFF_NEWVALUE_V02 + LEN_NEWVALUE_V02 - 1] = 0;
  szRecord[OFF_OLDVALUE_V02 + LEN_OLDVALUE_V02 - 1] = 0;
  szRecord[OFF_STATE_V01    + LEN_STATE_V02    - 1] = 0;

  m_szDate.rawAssign(cn90Strip(&szRecord[OFF_DATE_V02]));
  m_szTime.rawAssign(cn90Strip(&szRecord[OFF_TIME_V02]));
  m_szName.rawAssign(cn90Strip(&szRecord[OFF_NAME_V02]));
  m_szNewValue.rawAssign(cn90Strip(&szRecord[OFF_NEWVALUE_V02]));
  m_szOldValue.rawAssign(cn90Strip(&szRecord[OFF_OLDVALUE_V02]));
  m_szState.rawAssign(cn90Strip(&szRecord[OFF_STATE_V02]));
  m_szGroup.rawAssign(cn90Strip(&szRecord[OFF_GROUP_V02]));
  m_szComment.rawAssign("");

  if (_tcsicmp(m_szState, TXT_STATE_DELETED) == 0) {
    m_nState = StDeleted;
  } else if (_tcsicmp(m_szState, TXT_STATE_CHANGED) == 0) {
    m_nState = StChanged;
  } else if (_tcsicmp(m_szState, TXT_STATE_ACTIVE) == 0) {
    m_nState = StActive;
  } else {
    m_nState = StUnknown;
  } // end if

} // end cn52ParamHistoryRecord :: analyzeRecV02

/*
  -------------------------------------------------------------------------
  protected member function cn52ParamHistoryRecord :: initMembers
  -------------------------------------------------------------------------
*/
void cn52ParamHistoryRecord :: initMembers()
{
  m_szName.Init();
  m_szNewValue.Init();  
  m_szOldValue.Init();  
  m_nState = StUnknown;
  m_szDate.Init();  
  m_szTime.Init();  
  m_szState.Init();
  m_szDBName.Init();
  m_szGroup.Init();
  m_szComment.Init();
  m_szRecord.Init();

  m_bFileOpen = false;

  m_hFile = 0;

  // init selection members
  m_szSelName.Init();
  m_szSelDate.Init();
  m_szSelGroup.Init();
  m_szSelState.Init();
  m_nFields = ID_FULL;

} // end cn52ParamHistoryRecord :: initMembers

/*
  -------------------------------------------------------------------------
  protected function cn52ParamHistoryRecord :: checkNameArray
  -------------------------------------------------------------------------
*/
bool cn52ParamHistoryRecord :: checkNameArray
                           ( const tcn002_XpValueName & szName ) 
{
  bool bFound     = true;
  int  nKey       = 0;
  int  nBaseKey   = 0;

  for (int i = 0; i <  szName.length(); ++i) nKey = nKey + szName[i];
  
  nKey     = (nKey * 4711) % MAX_KEYS_CN52;
  nBaseKey = nKey;

  while ( ( m_NameArray[nKey].length()                           > 0 ) &&
          ( strcmp(m_NameArray[nKey].asCharp(), szName.asCharp()) != 0 )    ) {

    ++nKey;
    nKey = nKey % MAX_KEYS_CN52;
    if (nKey == nBaseKey) break;
  } // while

  if ( m_NameArray[nKey].length() == 0 ) {
    m_NameArray[nKey] = szName;
    bFound = false;
  } // end if

  return bFound;
} // end cn52ParamHistoryRecord :: checkNameArray
