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
/* @lastChanged: "1998-06-16  9:50"
 *
 * @filename:    vcn30.cpp
 * @purpose:     "media management"
 * @release:     7.1.0.0
 * @see:         "-.-"
 *
 * @copyright:   "Copyright by SAP AG, 1998-2004"
 *
 * ==================================================================
 *
 * responsible:   MartinR, TiloH
 *
 * created:       1998-03-12 by MartinR
 *
 * purpose:       basic module for media management 
 *                               
 * ==================================================================
 */

/* ------------------------------------------------------------------
 * includes
 * ------------------------------------------------------------------
 */

#include <stdio.h>

#include "heo02.h"
#include "heo06.h"

#include "hcn20.h"
#include "hcn30.h"
#include "hcn36.h"
#include "hcn42.h"
#include "hcn50.h"
#include "hcn51.h"
#include "hcn90.h"
#include "DBM/Srv/BackupTools/DBMSrvBTools_ToolEnum.hpp"
#include "DBM/Srv/Deprecated/DBMSrvDep_DirtyReply.hpp"
#include "DBM/Srv/Message/DBMSrvMsg_RTEError.hpp"

/* ------------------------------------------------------------------
 * PRIVATE FUNCTION cn30_CheckMediaFile
 * ------------------------------------------------------------------
 * purpose: exists the media file for specified database
 *
 */
static tcn00_Error cn30_CheckMediaFile (
      const tsp00_DbNamec & szDbName,
      char                * replyData,
      int                 * replyLen,
      bool                * bExists,
      bool                * bOldExists,
      char                * szDate)
{
  tcn00_Error          nFuncStatus  = OK_CN00;
  tsp05_RteFileError   rteFileError;

  tsp00_Pathc          szMediaFile;
  tsp00_Pathc          szBackupFile;

  tsp05_RteFileInfo    rteFileInfo;

  char                 * szPos = NULL;

  DBMSrvMsg_Error      oError;
  /**/

  *bExists = false;
  *bOldExists = false;

  /* info media file */
  if (nFuncStatus == OK_CN00) {
    nFuncStatus = cn42GetFileName(szDbName, FGET_DBMMDF_CN42, szMediaFile, szBackupFile);
  } // end if
  if (nFuncStatus == OK_CN00) {
    // try to open original
    sqlfinfoc (szMediaFile, &rteFileInfo, &rteFileError);
    if (rteFileError.sp5fe_result == vf_ok && !rteFileInfo.sp5fi_exists) {
      // restore backup to original
      cn42RestoreFile(szDbName, FGET_DBMMDF_CN42, oError);
      // try to open original
      sqlfinfoc (szMediaFile, &rteFileInfo, &rteFileError);
      if (rteFileError.sp5fe_result == vf_ok && !rteFileInfo.sp5fi_exists) {
        // try to open backup
        sqlfinfoc (szBackupFile, &rteFileInfo, &rteFileError);
      }  // end if
    }  // end if


    if (rteFileError.sp5fe_result == vf_ok) {
      if (rteFileInfo.sp5fi_exists) {
        
        *bExists = true;

        szPos = szDate;        
        SAPDB_memcpy(szPos, rteFileInfo.sp5fi_date_modified, sizeof(rteFileInfo.sp5fi_date_modified));
        szPos += sizeof(rteFileInfo.sp5fi_date_modified);
        SAPDB_memcpy(szPos, rteFileInfo.sp5fi_time_modified, sizeof(rteFileInfo.sp5fi_time_modified));
        szPos += sizeof(rteFileInfo.sp5fi_time_modified);
        *szPos = '\0';

      }
    } else {
      nFuncStatus = DBMSrvDep_DirtyReply(replyData, replyLen).startWithMessageList(DBMSrvMsg_RTEError(rteFileError));
    } // end if
  } else {
    nFuncStatus = DBMSrvDep_DirtyReply(replyData, replyLen).startWithError(nFuncStatus);
  }  // end if

    /* info old media file */
    if (nFuncStatus == OK_CN00) {
        nFuncStatus = cn42GetFileName(szDbName, FGET_CTRLMDF_CN42, szMediaFile);
        if (nFuncStatus == OK_CN00) {
            sqlfinfoc (szMediaFile, &rteFileInfo, &rteFileError);
            if (rteFileError.sp5fe_result == vf_ok) {
                if (rteFileInfo.sp5fi_exists) {
                    *bOldExists = true;
                } // end if
            } else {
                nFuncStatus = DBMSrvDep_DirtyReply(replyData, replyLen).startWithMessageList(DBMSrvMsg_RTEError(rteFileError));
            } // end if
        } else if (nFuncStatus == ERR_XPNOTFOUND_CN00) {
            nFuncStatus = OK_CN00; // Parameter RUNDIRECTORY not available -> not an error
        } else {
            nFuncStatus = DBMSrvDep_DirtyReply(replyData, replyLen).startWithError(nFuncStatus);
        }  // end if
    }  // end if

    return nFuncStatus;
} /* end cn30_CheckMediaFile */

/* PRIVATE FUNCTION cn30_OpenMediaFile
 * ------------------------------------------------------------------
 * purpose: open the media file for specified database
 *
 */
static tcn00_Error cn30_OpenMediaFile ( 
      const tsp00_DbNamec & szDbName,
      char                * replyData,
      int                 * replyLen,
      bool                  bForWrite, 
      tsp00_Int4            * hMediaFile,
      char                *& mediaFileName)
{
  tcn00_Error          nFuncStatus  = OK_CN00;
  tsp05_RteFileError   rteFileError;

  tsp00_Pathc          szMediaFile;
  tsp00_Pathc          szBackupFile;

  DBMSrvMsg_Error      oError;

  /**/
  
  /* open media file */
  if (nFuncStatus == OK_CN00) {
    cn42GetFileName(szDbName, FGET_DBMMDF_CN42, szMediaFile, szBackupFile);

    // try to open original
    if (bForWrite) {
      sqlfopenc (szMediaFile, sp5vf_text, sp5vf_write, sp5bk_buffered, hMediaFile, &rteFileError);
    } else {
      sqlfopenc (szMediaFile, sp5vf_text, sp5vf_read, sp5bk_buffered, hMediaFile, &rteFileError);
    } // end if

    if (rteFileError.sp5fe_result != vf_ok) {
      // restore backup to original
      cn42RestoreFile(szDbName, FGET_DBMMDF_CN42, oError);

      // try to open original
      if (bForWrite) {
        sqlfopenc (szMediaFile, sp5vf_text, sp5vf_write, sp5bk_buffered, hMediaFile, &rteFileError);
      } else {
        sqlfopenc (szMediaFile, sp5vf_text, sp5vf_read, sp5bk_buffered, hMediaFile, &rteFileError);
      } // end if

      if (rteFileError.sp5fe_result != vf_ok) {
        // try to open backup
        if (bForWrite) {
          sqlfopenc (szBackupFile, sp5vf_text, sp5vf_write, sp5bk_buffered, hMediaFile, &rteFileError);
        } else {
          sqlfopenc (szBackupFile, sp5vf_text, sp5vf_read, sp5bk_buffered, hMediaFile, &rteFileError);
        } // end if

        cn36_StrAllocCpy(mediaFileName, szBackupFile.asCharp());
      }
      else
        cn36_StrAllocCpy(mediaFileName, szMediaFile.asCharp());

      if (rteFileError.sp5fe_result != vf_ok) {
        nFuncStatus = DBMSrvDep_DirtyReply(replyData, replyLen).startWithMessageList(DBMSrvMsg_RTEError(rteFileError));
      } // end if
    }
    else
        cn36_StrAllocCpy(mediaFileName, szMediaFile.asCharp());
  } // end if

  return nFuncStatus;
} /* end cn30_OpenMediaFile */

/* PRIVATE FUNCTION cn30_OpenOldMediaFile
 * ------------------------------------------------------------------
 * purpose: open the old media file for specified database
 *
 */
static tcn00_Error cn30_OpenOldMediaFile ( 
      const tsp00_DbNamec & szDbName,
      char                * replyData,
      int                 * replyLen,
      bool                  bForWrite, 
      tsp00_Int4            * hMediaFile )
{
  tcn00_Error          nFuncStatus  = OK_CN00;
  tsp05_RteFileError   rteFileError;

  tsp00_Pathc          szMediaFile;

  /**/

  /* open media file */
  if (nFuncStatus == OK_CN00) {
    cn42GetFileName(szDbName, FGET_CTRLMDF_CN42, szMediaFile);

    if (bForWrite) {
      sqlfopenc (szMediaFile, sp5vf_text, sp5vf_write, sp5bk_buffered, hMediaFile, &rteFileError);
    } else {
      sqlfopenc (szMediaFile, sp5vf_text, sp5vf_read, sp5bk_buffered, hMediaFile, &rteFileError);
    }

    if (rteFileError.sp5fe_result != vf_ok) {
      nFuncStatus = DBMSrvDep_DirtyReply(replyData, replyLen).startWithMessageList(DBMSrvMsg_RTEError(rteFileError));
    }
  }

  return nFuncStatus;
} /* end cn30_OpenOldMediaFile */

/* PRIVATE FUNCTION cn30_CloseMediaFile
 * ------------------------------------------------------------------
 * purpose: close the media file
 *
 */
static tcn00_Error cn30_CloseMediaFile (
      char           * replyData,
      int            * replyLen,
      tsp00_Int4       hMediaFile )
{
  tcn00_Error          nFuncStatus  = OK_CN00;
  tsp05_RteFileError   rteFileError;

  /**/

  sqlfclosec (hMediaFile, sp5vf_close_normal, &rteFileError);
  if (rteFileError.sp5fe_result != vf_ok) {
    nFuncStatus = DBMSrvDep_DirtyReply(replyData, replyLen).startWithMessageList(DBMSrvMsg_RTEError(rteFileError));
  }

  return nFuncStatus;
} /* end cn30_CloseMediaFile */

/* PRIVATE FUNCTION cn30_InitMedia
 * ------------------------------------------------------------------
 * purpose: initialize the media struct
 *
 */
void cn30_InitMedia (
      tcn003_Media   * pMedia )
{
  pMedia->pFirstGroup = NULL;
  pMedia->pLastGroup  = NULL;
} /* end cn30_InitMedia */

/* PRIVATE FUNCTION cn30_InitMedium
 * ------------------------------------------------------------------
 * purpose: initialize the medium struct
 *
 */
void cn30_InitMedium(
      tcn003_Medium   * pMedium)
{
  SAPDB_strcpy(pMedium->szName, ME_DEFAULT_NAME_CN003);
  SAPDB_strcpy(pMedium->szLocation, ME_DEFAULT_LOCATION_CN003);
  SAPDB_strcpy(pMedium->szType, ME_DEFAULT_TYPE_CN003);
  SAPDB_strcpy(pMedium->szSaveType, ME_DEFAULT_SAVETYPE_CN003);
  SAPDB_strcpy(pMedium->szSize, ME_DEFAULT_SIZE_CN003);
  SAPDB_strcpy(pMedium->szBlocksize, ME_DEFAULT_BLOCKSIZE_CN003);
  SAPDB_strcpy(pMedium->szOverwrite, ME_DEFAULT_OVERWRITE_CN003);
  SAPDB_strcpy(pMedium->szAutoLoader, ME_DEFAULT_AUTOLOADER_CN003);
  SAPDB_strcpy(pMedium->szOSCommand, ME_DEFAULT_OSCMD_CN003);
  SAPDB_strcpy(pMedium->szCreated, ME_DEFAULT_DATE_CN003);
  SAPDB_strcpy(pMedium->szModified, ME_DEFAULT_DATE_CN003);
  SAPDB_strcpy(pMedium->szExternal, DBMSrvBTools_ToolEnum(DBMSrvBTools_ToolEnum::None).AsString());
  pMedium->bOld = false;
  pMedium->pNext = NULL;
  pMedium->pPrev = NULL;
} /* end cn30_InitMedium */

/* PRIVATE FUNCTION cn30_InitMediaGroup
 * ------------------------------------------------------------------
 * purpose: initialize the mediagroup struct
 *
 */
void cn30_InitMediaGroup(
      tcn003_MediaGroup   * pMediaGroup)
{
  pMediaGroup->szName[0] = '\0';
  pMediaGroup->pFirst = NULL;
  pMediaGroup->pLast = NULL;
  pMediaGroup->pNext = NULL;
  pMediaGroup->pPrev = NULL;
} /* end cn30_InitMediaGroup */

/* PRIVATE FUNCTION cn30_AdjustExternalProperty
 * ------------------------------------------------------------------
 * purpose: adjusts the external property of a medium after it is
 *          read from the media defenition file
 */
static tcn00_Error cn30_AdjustExternalProperty(tcn003_Medium * Medium,
                                               char          * replyData,
                                               int           * replyLen,
                                               int             FromMediaDefinitionFile=0)
{
    tcn00_Error rc=OK_CN00;

    if('\0'==Medium->szExternal[0])                                                                     //if backup tool is not explicitly specified
    {
        if(FromMediaDefinitionFile)                                                                     //and if medium comes from media definition file (->old medium definition)
            SAPDB_strcpy(Medium->szExternal, DBMSrvBTools_ToolEnum(Medium->szName).AsString());               //use the tool specified through the medium name
        else
            SAPDB_strcpy(Medium->szExternal, DBMSrvBTools_ToolEnum(DBMSrvBTools_ToolEnum::None).AsString());  // otherwise (e.g., new medium is defined) use no backup tool at all
    }
    else
    {
        cn90StrUpperCopy(Medium->szExternal, Medium->szExternal, false);

        if(!DBMSrvBTools_ToolEnum::IsAValidToolEnumString(Medium->szExternal))
            rc=cn90AnswerIError(replyData, replyLen, ERR_MENOVALID_CN00);
    }

    return rc;
} // end cn30_AdjustExternalProperty

/* PRIVATE FUNCTION cn30_CheckMedium
 * ------------------------------------------------------------------
 * purpose: check the medium properties
 *
 */
static tcn00_Error cn30_CheckMedium (
      tcn003_Medium  * pMedium,
      char           * replyData,
      int            * replyLen,
      int              replyLenMax )
{
  FUNCTION_DBG_MCN00_1("cn30_CheckMedium");

  tcn00_Error         nFuncStatus  = OK_CN00;

  int                 nIndex = 0;

  long                lSize = 0;
  long                lBlocksize = 0;

  /**/

  // NAME
  
  if (nFuncStatus == OK_CN00) {
    if (pMedium->szName[0] == '\0') {
      nFuncStatus = ERR_MENOVALID_CN00;
      cn90AnswerIError(replyData, replyLen, nFuncStatus);
    }
  }

  // TYPE

  if (nFuncStatus == OK_CN00) {
    if (pMedium->szType[0] == '\0') {
      SAPDB_strcpy(pMedium->szType, ME_DEFAULT_TYPE_CN003);
    } else {
      cn90StrUpperCopy(pMedium->szType, pMedium->szType, false);
      if ( (strcmp(pMedium->szType, ME_TYPE_FILE_CN003)  != 0) &&
           (strcmp(pMedium->szType, ME_TYPE_TAPE_CN003) != 0) &&  
           (strcmp(pMedium->szType, ME_TYPE_PIPE_CN003)   != 0) && 
           (strcmp(pMedium->szType, ME_TYPE_UNKN_CN003)  != 0)    ) {

        nFuncStatus = ERR_MENOVALID_CN00;
        cn90AnswerIError(replyData, replyLen, nFuncStatus);
      }
    }
  }

  // LOCATION

  // Does value exist?
  if (nFuncStatus == OK_CN00) {
    if (pMedium->szLocation[0] == '\0') {
      nFuncStatus = ERR_MENOVALID_CN00;
      cn90AnswerIError(replyData, replyLen, nFuncStatus);
    } // end if
  } // end if
  // Is ASCII only?
  if (nFuncStatus == OK_CN00) {
    for (nIndex = 0; pMedium->szLocation[nIndex] != 0; ++nIndex) {
      nFuncStatus = (((unsigned char) pMedium->szLocation[nIndex]) < 128) ?  nFuncStatus : cn90AnswerIError(replyData, replyLen, ERR_ASCII_CN00);
    } // end if
  } // end if
  // check file/directory
  if (nFuncStatus == OK_CN00) {
    if (strcmp(pMedium->szType, ME_TYPE_FILE_CN003) == 0) {
      
      tsp05_RteFileError       rteFileErr;
      tsp05_RteFileInfo        rteFInfo;
      tcn002_XpValueString     szRunDir;
      tsp01_RteError           rteError;

      DBMSrvMsg_Error oParamGetMsg;
      if (cn20XParamGetValue(cn00DBMServerData::vcontrol()->dbname, PAN_RUNDIR, szRunDir, oParamGetMsg)) {
        // change to rundir
        if (sqlchdirc(szRunDir.asCharp(), &rteError)) {

          sqlfinfoc (pMedium->szLocation, &rteFInfo, &rteFileErr );
          if (rteFileErr.sp5fe_result   == vf_ok) {
            if (rteFInfo.sp5fi_exists && (rteFInfo.sp5fi_media_kind == vf_t_directory)) {
              // do not specify directory
              DBMSrvMsg_Error oRTEEvent(SDBMSG_DBMSRV_LOCISDIR, Msg_Arg(SDBMSGTAG_DBMSRV_LOCISDIR__FILENAME,pMedium->szLocation));
              oRTEEvent.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_MENOVALID));
              nFuncStatus = cn90AnswerEvent(replyData, replyLen, replyLenMax, oRTEEvent);
            } // end if
          } // end if
          
        // change back to dbroot
        sqlchdirc(cn00DBMServerData::vcontrol()->dbroot.asCharp(), &rteError);
        } // end if
      } // end if
    } // end if
  } // end if

  // SAVETYPE

  if (nFuncStatus == OK_CN00) {
    if (pMedium->szSaveType[0] == '\0') {
      SAPDB_strcpy(pMedium->szSaveType, ME_DEFAULT_SAVETYPE_CN003);
    } else {
      cn90StrUpperCopy(pMedium->szSaveType, pMedium->szSaveType, false);
      
      if ( (strcmp(pMedium->szSaveType, ME_SAVETYPE_DAT_CN003) != 0) &&
           (strcmp(pMedium->szSaveType, ME_SAVETYPE_PAG_CN003) != 0) &&  
           (strcmp(pMedium->szSaveType, ME_SAVETYPE_LOG_CN003) != 0) && 
           (strcmp(pMedium->szSaveType, ME_SAVETYPE_AUT_CN003) != 0) && 
           (strcmp(pMedium->szSaveType, ME_SAVETYPE_UNK_CN003) != 0)    ) {

        nFuncStatus = ERR_MENOVALID_CN00;
        cn90AnswerIError(replyData, replyLen, nFuncStatus);
      }
    }
  }

  // SIZE

  if (nFuncStatus == OK_CN00) {
    if (pMedium->szSize[0] == '\0') {
      SAPDB_strcpy(pMedium->szSize, "0");
    } else {
      if (cn90AnalyzeNumber(pMedium->szSize, &lSize) == false) {
        nFuncStatus = ERR_MENOVALID_CN00;
        cn90AnswerIError(replyData, replyLen, nFuncStatus);
      }
    }
  }
  
  // BLOCKSIZE

  if (nFuncStatus == OK_CN00) {
    if (pMedium->szBlocksize[0] == '\0') {
      SAPDB_strcpy(pMedium->szBlocksize, ME_DEFAULT_BLOCKSIZE_CN003);
    } else {
      if (cn90AnalyzeNumber(pMedium->szBlocksize, &lBlocksize) == false) {
        nFuncStatus = ERR_MENOVALID_CN00;
        cn90AnswerIError(replyData, replyLen, nFuncStatus);
      } else {
        if (lBlocksize == 0) {
          SAPDB_strcpy(pMedium->szBlocksize, ME_DEFAULT_BLOCKSIZE_CN003);
        } 
      }
    }
  }

  // OVERWRITE

  if (nFuncStatus == OK_CN00) {
    if (pMedium->szOverwrite[0] == '\0') {
      SAPDB_strcpy(pMedium->szOverwrite, ME_PROP_NO_CN003);
    } else {
      cn90StrUpperCopy(pMedium->szOverwrite, pMedium->szOverwrite, false);
      if ( (strcmp(pMedium->szOverwrite, ME_PROP_YES_CN003) != 0) &&
           (strcmp(pMedium->szOverwrite, ME_PROP_NO_CN003) != 0)     ) {
        nFuncStatus = ERR_MENOVALID_CN00;
        cn90AnswerIError(replyData, replyLen, nFuncStatus);
      }
    }
  }
  
  // AUTOLOADER

  if (nFuncStatus == OK_CN00) {
    if (pMedium->szAutoLoader[0] == '\0') {
      SAPDB_strcpy(pMedium->szAutoLoader, ME_DEFAULT_AUTOLOADER_CN003);
    } else {
      cn90StrUpperCopy(pMedium->szAutoLoader, pMedium->szAutoLoader, false);
      if ( (strcmp(pMedium->szAutoLoader, ME_PROP_YES_CN003) != 0) &&
           (strcmp(pMedium->szAutoLoader, ME_PROP_NO_CN003)  != 0)    ) {
        nFuncStatus = ERR_MENOVALID_CN00;
        cn90AnswerIError(replyData, replyLen, nFuncStatus);
      }
    }
  }

    // EXTERN
    if(OK_CN00==nFuncStatus)
        nFuncStatus=cn30_AdjustExternalProperty(pMedium, replyData, replyLen);

  return nFuncStatus;
} /* end cn30_CheckMedium */

/* PRIVATE FUNCTION cn30_CheckAutosave
 * ------------------------------------------------------------------
 * purpose: check autosave savetype for all media, only one medium
 *          can have AUTO as savetype
 *
 */
static tcn00_Error cn30_CheckAutosave (
      tcn003_Medium  * pMediumCheck,
      tcn003_Media   * pMedia )     
{
  tcn00_Error         nFuncStatus  = OK_CN00;

  tcn003_MediaGroup   * pMediaGroup;
  tcn003_Medium       * pMedium;

  /**/

  if (pMediumCheck->szSaveType[0] != '\0') {
    if ( (strcmp(pMediumCheck->szSaveType, ME_SAVETYPE_AUT_CN003) == 0) ) {
      pMediaGroup = pMedia->pFirstGroup;
      while (pMediaGroup != NULL) {
        pMedium = pMediaGroup->pFirst;
        while (pMedium != NULL) {
          if (pMedium->szSaveType[0] != '\0') {
            if ( (strcmp(pMedium->szSaveType, ME_SAVETYPE_AUT_CN003) == 0) ) {
              SAPDB_strcpy(pMedium->szSaveType, ME_SAVETYPE_LOG_CN003);
            }
          }
          pMedium = pMedium->pNext;
        }
        pMediaGroup = pMediaGroup->pNext;
      }
    }
  }

  return nFuncStatus;
} /* end cn30_CheckAutosave*/

/* PRIVATE FUNCTION cn30_CheckMediaGroup
 * ------------------------------------------------------------------
 * purpose: check savetype for all media in a group, media must have
 *          the same savetype in a group
 *
 */
static tcn00_Error cn30_CheckMediaGroup (
      tcn003_Medium     * pMediumCheck,
      tcn003_MediaGroup * pMediaGroup )     
{
  tcn00_Error         nFuncStatus  = OK_CN00;

  tcn003_Medium       * pMedium;

  /**/

  pMedium = pMediaGroup->pFirst;
  while (pMedium != NULL) {
    SAPDB_strcpy(pMedium->szSaveType, pMediumCheck->szSaveType);
    pMedium = pMedium->pNext;
  }

  return nFuncStatus;
} /* end cn30_CheckMediaGroup*/

/* PRIVATE FUNCTION cn30_ReadMediumProperties
 * ------------------------------------------------------------------
 * purpose: read the properties for a medium from media file
 *
 */
static tcn00_Error cn30_ReadMediumProperties (
      tcn003_Medium  * pMedium,
      char           * replyData,
      int            * replyLen,
      int              replyLenMax,
      tsp00_Int4       hMediaFile,
      int            & line,
      const char     * mediaFileName)
{
  tcn00_Error          nFuncStatus  = OK_CN00;
  tsp05_RteFileError   rteFileError;

  tsp00_Longint          nFilePos;

  char                 szBuffer[ME_LINE_LENGTH_MXCN003 + 1];
  char                 szLine[ME_LINE_LENGTH_MXCN003 + 1];
  tsp00_Longint          nRead;

  long                 nLen = 0;

  bool                 bNext = false;

  char                 szProperty[ME_PROPERTY_NAME_LENGTH_MXCN003 + 1];
  char                 szValue[ME_PROPERTY_VAL_LENGTH_MXCN003 +1];

  char                 * szEqual;
  long                 nEqualPos =0;
  
  /**/

  do {
    sqlftellc (hMediaFile, &nFilePos, &rteFileError);
    if (rteFileError.sp5fe_result == vf_ok) {
    
      sqlfreadc (hMediaFile, szBuffer, ME_LINE_LENGTH_MXCN003, &nRead, &rteFileError);
      if (rteFileError.sp5fe_result == vf_ok) {

        line++;
        SAPDB_strcpy(szLine, szBuffer);
        cn90Strip(szLine);
        nLen = (long)strlen(szLine);

        if (nLen > 0 && szLine[0] != ME_COMMENT_CN003)
        {
            if ( (szLine[0] == ME_NAME_BEGIN_CN003) )
            {
                if(szLine[nLen -1] == ME_NAME_END_CN003)
                {
                    bNext = true;
                    sqlfseekc (hMediaFile, nFilePos, sp5vf_seek_begin, &rteFileError);
                    line--;
                }
                else
                {
                    char nameEndAsString[2]={ME_NAME_END_CN003,'\0'};

                    DBMSrvMsg_Error error(
                                        SDBMSG_DBMSRV_FORMAT_DELIMISS,
	                                    Msg_Arg(SDBMSGTAG_DBMSRV_FORMAT_DELIMISS__DELIMITER, nameEndAsString),
	                                    Msg_Arg(SDBMSGTAG_DBMSRV_FORMAT_DELIMISS__STRING, szLine));

                    error.Overrule(
                        DBMSrvMsg_Error(
                            SDBMSG_DBMSRV_MEFDIRTY,
	                        Msg_Arg(SDBMSGTAG_DBMSRV_MEFDIRTY__LINE, line),
                            Msg_Arg(SDBMSGTAG_DBMSRV_MEFDIRTY__FILENAME, mediaFileName?mediaFileName:"unknown")));

                    nFuncStatus=DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(error);
                }
            }
            else {
              szEqual = strchr(szLine, ME_PROP_EQUAL_CN003);

              if(0==szEqual)
              {
                char eqalAsString[2]={ME_PROP_EQUAL_CN003,'\0'};

                DBMSrvMsg_Error error(
                                    SDBMSG_DBMSRV_FORMAT_SEPARATORMISSING,
	                                Msg_Arg(SDBMSGTAG_DBMSRV_FORMAT_SEPARATORMISSING__STRING, szLine),
	                                Msg_Arg(SDBMSGTAG_DBMSRV_FORMAT_SEPARATORMISSING__SEPARATOR, eqalAsString));

                error.Overrule(
                        DBMSrvMsg_Error(
                            SDBMSG_DBMSRV_MEFDIRTY,
	                        Msg_Arg(SDBMSGTAG_DBMSRV_MEFDIRTY__LINE, line),
                            Msg_Arg(SDBMSGTAG_DBMSRV_MEFDIRTY__FILENAME, mediaFileName?mediaFileName:"unknown")));

                nFuncStatus=DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(error);
              }
              else
              {
                nEqualPos = (long) (szEqual - szLine);

                strncpy(szProperty, szLine, nEqualPos);
                szProperty[nEqualPos] = '\0';
                cn90Strip(szProperty);

                SAPDB_strcpy(szValue, szEqual + 1);
                cn90Strip(szValue);
              
                if ( strcmp(szProperty, ME_KEY_LOCATION_CN003) == 0 ) {
                    SAPDB_strcpy(pMedium->szLocation, szValue);
                } else if ( strcmp(szProperty, ME_KEY_TYPE_CN003) == 0 ) {
                    SAPDB_strcpy(pMedium->szType, szValue);
                } else if ( strcmp(szProperty, ME_KEY_SAVETYPE_CN003) == 0 ) {
                    SAPDB_strcpy(pMedium->szSaveType, szValue);
                } else if ( strcmp(szProperty, ME_KEY_SIZE_CN003) == 0 ) {
                    SAPDB_strcpy(pMedium->szSize, szValue);
                } else if ( strcmp(szProperty, ME_KEY_BLOCKSIZE_CN003) == 0 ) {
                    SAPDB_strcpy(pMedium->szBlocksize, szValue);
                } else if ( strcmp(szProperty, ME_KEY_OVERWRITE_CN003) == 0 ) {
                    SAPDB_strcpy(pMedium->szOverwrite, szValue);
                } else if ( strcmp(szProperty, ME_KEY_AUTOLOADER_CN003) == 0 ) {
                    SAPDB_strcpy(pMedium->szAutoLoader, szValue);
                } else if ( strcmp(szProperty, ME_KEY_OSCOMMAND_CN003) == 0 ) {
                    SAPDB_strcpy(pMedium->szOSCommand, szValue);
                } else if ( strcmp(szProperty, ME_KEY_BACKUPTOOL_CN003) == 0 ) {
                    SAPDB_strcpy(pMedium->szExternal, szValue);
                } else if ( strcmp(szProperty, ME_KEY_CREATED_CN003) == 0 ) {
                    SAPDB_strcpy(pMedium->szCreated, szValue);
                } else if ( strcmp(szProperty, ME_KEY_MODIFIED_CN003) == 0 ) {
                    SAPDB_strcpy(pMedium->szModified, szValue);
              }
              }
            }
        }
      }
    }
  
  } while ( ( bNext == false ) &&
            ( rteFileError.sp5fe_result  == vf_ok             ) &&
            ( rteFileError.sp5fe_warning == sp5vfw_no_warning ) &&
            ( nFuncStatus                == OK_CN00           )     );

  if (rteFileError.sp5fe_result != vf_ok) {
    if (rteFileError.sp5fe_result != vf_eof) {
      nFuncStatus = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(DBMSrvMsg_RTEError(rteFileError));
    }
  }

  return nFuncStatus;
} /* end cn30_ReadMediumProperties */

/* PRIVATE FUNCTION cn30_ReadNextMedium
 * ------------------------------------------------------------------
 * purpose: read the next medium from media file
 *
 */
static tcn00_Error cn30_ReadNextMedium (
      tcn003_Medium  * pMedium,
      char           * replyData,
      int            * replyLen,
      int              replyLenMax,
      tsp00_Int4       hMediaFile,
      bool           bProperties,
      bool           * bFound,
      int            & line,
      const char     * mediaFileName)
{
  tcn00_Error          nFuncStatus  = OK_CN00;
  tsp05_RteFileError   rteFileError;

  char                 szBuffer[ME_LINE_LENGTH_MXCN003 + 1];
  char                 szLine[ME_LINE_LENGTH_MXCN003 + 1];
  tsp00_Longint          nRead;

  long                 nLen = 0;

  /**/
  
  *bFound = false;

  do {
    sqlfreadc(hMediaFile, szBuffer, ME_LINE_LENGTH_MXCN003, &nRead, &rteFileError);
    if (rteFileError.sp5fe_result == vf_ok) {
      line++;

      SAPDB_strcpy(szLine, szBuffer);
      cn90Strip(szLine);
      nLen = (long)strlen(szLine);

      if (nLen > 0) {
  		  if (szLine[0] != ME_COMMENT_CN003) {
          if ( (szLine[0] == ME_NAME_BEGIN_CN003) && 
               (szLine[nLen -1] == ME_NAME_END_CN003) ) {
        
            strncpy(pMedium->szName, szLine + 1, nLen - 2);
            pMedium->szName[nLen-2] = '\0';

            if (bProperties) {
              nFuncStatus = cn30_ReadMediumProperties(pMedium, replyData, replyLen, replyLenMax, hMediaFile, line, mediaFileName);
            }

            *bFound = true;
          }
		    }
      }
    }
  } while ( ( rteFileError.sp5fe_result  == vf_ok             ) &&
            ( rteFileError.sp5fe_warning == sp5vfw_no_warning ) &&
            ( nFuncStatus                == OK_CN00           ) &&
            ( *bFound                    == false             )    );

  if (rteFileError.sp5fe_result != vf_ok) {
    if (rteFileError.sp5fe_result != vf_eof) {
      nFuncStatus = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(DBMSrvMsg_RTEError(rteFileError));
    }
  }

  return nFuncStatus;
} /* end cn30_ReadNextMedium */


/* PRIVATE FUNCTION cn30_ReadParallelIDFromOldMediaFile
 * ------------------------------------------------------------------
 * purpose: read the parallelid from old media file
 *
 */
static tcn00_Error cn30_ReadParallelIDFromOldMediaFile ( 
      char                * replyData,
      int                 * replyLen,
      tsp00_Int4            hMediaFile,
      char                * szParallelID )
{
  tcn00_Error          nFuncStatus  = OK_CN00;
  tsp05_RteFileError   rteFileError;

  tsp00_Longint          nFilePos;
  char                 szBuffer[ME_LINE_LENGTH_MXCN003 + 1];
  char                 szLine[ME_LINE_LENGTH_MXCN003 + 1];
  tsp00_Longint          nRead;

  long                 nLen = 0;

  char                 szProperty[ME_LINE_LENGTH_MXCN003 + 1];
  char                 szValue[ME_LINE_LENGTH_MXCN003 + 1];
  char                 szName[ME_NAME_LENGTH_MXCN003 + 1];
  char                 szLocation[ME_LINE_LENGTH_MXCN003 + 1];
  char                 * szPos1;

  bool                 bFound = false;

  szParallelID[0] = '\0';

  sqlftellc (hMediaFile, &nFilePos, &rteFileError);

  if (rteFileError.sp5fe_result == vf_ok) {

    do {
      sqlfreadc(hMediaFile, szBuffer, ME_LINE_LENGTH_MXCN003, &nRead, &rteFileError);
      if (rteFileError.sp5fe_result == vf_ok) {

        SAPDB_strcpy(szLine, szBuffer);
        cn90Strip(szLine);
        nLen = (long)strlen(szLine);

        if (nLen > 0) {
  		    if (szLine[0] != ME_OLDCOMMENT_CN003) {

            szPos1 = strchr(szLine, ME_PROP_EQUAL_CN003);

            if (szPos1) {
              strncpy(szProperty, szLine, szPos1 - szLine);
              szProperty[szPos1 - szLine] = '\0';
              cn90Strip(szProperty);

              SAPDB_strcpy(szValue, szPos1 + 1);
              cn90Strip(szValue);
              
              if ( strcmp(szProperty, ME_KEY_MEDIAID_CN003) == 0 ) {
                szPos1 = strchr(szValue, ME_KEY_BLANK_CN003);
                if (szPos1) {
                  strncpy(szName, szValue, szPos1 - szValue);
                  szName[szPos1 - szValue] = '\0';

                  SAPDB_strcpy(szLocation, szPos1 + 1);
                  cn90Strip(szLocation);

                  if (strncmp(szLocation, ME_KEY_PARALLEL_CN003, strlen(ME_KEY_PARALLEL_CN003)) == 0) {
                    SAPDB_strcpy(szParallelID, szName);
                    bFound = true;
                  }
                }
              } 
            }
          }
        }
      }
    } while ( ( rteFileError.sp5fe_result  == vf_ok             ) &&
              ( rteFileError.sp5fe_warning == sp5vfw_no_warning ) &&
              ( nFuncStatus                == OK_CN00           ) &&
              ( bFound                     == false             )    );
  }

  if (rteFileError.sp5fe_result == vf_ok) {
    sqlfseekc (hMediaFile, nFilePos, sp5vf_seek_begin, &rteFileError);
  }

  if (rteFileError.sp5fe_result != vf_ok) {
    if (rteFileError.sp5fe_result != vf_eof) {
      nFuncStatus = DBMSrvDep_DirtyReply(replyData, replyLen).startWithMessageList(DBMSrvMsg_RTEError(rteFileError));
    }
  }

  return nFuncStatus;
} /* end cn30_ReadParallelIDFromOldMediaFile */



/* PRIVATE FUNCTION cn30_ReadNextOldMedium
 * ------------------------------------------------------------------
 * purpose: read the next old medium from media file
 *
 */
static tcn00_Error cn30_ReadNextOldMedium (
      tcn003_Medium  * pMedium,
      char           * replyData,
      int            * replyLen,
      tsp00_Int4         hMediaFile,
      bool             bProperties,
      bool           * bFound, 
      char           * szParallelID)
{
  tcn00_Error          nFuncStatus  = OK_CN00;
  tsp05_RteFileError   rteFileError;

  char                 szBuffer[ME_LINE_LENGTH_MXCN003 + 1];
  char                 szLine[ME_LINE_LENGTH_MXCN003 + 1];
  tsp00_Longint          nRead;

  long                 nLen = 0;

  char                 szProperty[ME_PROPERTY_NAME_LENGTH_MXCN003 + 1];
  char                 szValue[ME_LINE_LENGTH_MXCN003 +1];

  char                 * szPos1;
  char                 * szPos2;

  bool                 bIgnore = false;

  /**/
  
  *bFound = false;

  do {
    sqlfreadc(hMediaFile, szBuffer, ME_LINE_LENGTH_MXCN003, &nRead, &rteFileError);
    if (rteFileError.sp5fe_result == vf_ok) {

      SAPDB_strcpy(szLine, szBuffer);
      cn90Strip(szLine);
      nLen = (long)strlen(szLine);

      if (nLen > 0) {
  		  if (szLine[0] != ME_OLDCOMMENT_CN003) {

          szPos1 = strchr(szLine, ME_PROP_EQUAL_CN003);

          if (szPos1) {
            /* Name + Loacation */
            strncpy(szProperty, szLine, szPos1 - szLine);
            szProperty[szPos1 - szLine] = '\0';
            cn90Strip(szProperty);

            SAPDB_strcpy(szValue, szPos1 + 1);
            cn90Strip(szValue);
              
            if ( strcmp(szProperty, ME_KEY_MEDIAID_CN003) == 0 ) {
              szPos1 = strchr(szValue, ME_KEY_BLANK_CN003);
              if (szPos1) {
                strncpy(pMedium->szName, szValue, szPos1 - szValue);
                pMedium->szName[szPos1 - szValue] = '\0';

                SAPDB_strcpy(pMedium->szLocation, szPos1 + 1);
                cn90Strip(pMedium->szLocation);

                if (strncmp(pMedium->szLocation, ME_KEY_PARALLEL_CN003, strlen(ME_KEY_PARALLEL_CN003)) == 0) {
                  bIgnore = true;
                } else {
                  bIgnore = false;
                }
              }
            } 
          } else {
            if (!bIgnore) {
              /* Overwrite */
              szPos1 = szLine;
              szPos2 = strchr(szPos1, ME_KEY_PROPSEP_CN003);
              if (szPos2) {
                strncpy(szValue, szPos1, szPos2 - szPos1);
                szValue[szPos2 - szPos1] = '\0';
                if (strcmp(szValue, ME_OLD_PROP_YES_CN003) == 0 ) {
                  SAPDB_strcpy(pMedium->szOverwrite, ME_PROP_YES_CN003);
                } else {
                  if (strcmp(szValue, ME_OLD_PROP_VERSION_CN003) == 0 ) {
                    SAPDB_strcpy(pMedium->szOverwrite, ME_PROP_VERSION_CN003);
                  }
                }
              }
              szPos1 = szPos2 + 1;
              /* Type */
              szPos2 = strchr(szPos1, ME_KEY_PROPSEP_CN003);
              if (szPos2) {
                strncpy(szValue, szPos1, szPos2 - szPos1);
                szValue[szPos2 - szPos1] = '\0';
                if (strcmp(szValue, ME_OLD_TYPE_TAPE_CN003) == 0 ) {
                  SAPDB_strcpy(pMedium->szType, ME_TYPE_TAPE_CN003);

                } else if (strcmp(szValue, ME_OLD_TYPE_FILE_CN003) == 0 ) {
                  SAPDB_strcpy(pMedium->szType, ME_TYPE_FILE_CN003);

                } else if (strcmp(szValue, ME_OLD_TYPE_NORE_CN003) == 0 ) {
                  SAPDB_strcpy(pMedium->szType, ME_TYPE_NORE_CN003);

                } else if (strcmp(szValue, ME_OLD_TYPE_PIPE_CN003) == 0 ) {
                  SAPDB_strcpy(pMedium->szType, ME_TYPE_PIPE_CN003);

                } else if (strcmp(szValue, ME_OLD_TYPE_AUTO_CN003) == 0 ) {
                  SAPDB_strcpy(pMedium->szAutoLoader, ME_PROP_YES_CN003);

                } else if (strcmp(szValue, ME_OLD_TYPE_AUTL_CN003) == 0 ) {
                  SAPDB_strcpy(pMedium->szAutoLoader, ME_PROP_YES_CN003);

                } else if (strcmp(szValue, ME_OLD_TYPE_UNKN_CN003) == 0 ) {
                  SAPDB_strcpy(pMedium->szType, ME_TYPE_UNKN_CN003);
                } 
              }
              szPos1 = szPos2 + 1;
              /* Size */
              szPos2 = strchr(szPos1, ME_KEY_PROPSEP_CN003);
              if (szPos2) {
                strncpy(szValue, szPos1, szPos2 - szPos1);
                szValue[szPos2 - szPos1] = '\0';
                SAPDB_strcpy(pMedium->szSize, szValue);
              }
              szPos1 = szPos2 + 1;
              /* Usage */
              szPos2 = strchr(szPos1, ME_KEY_PROPSEP_CN003);
              szPos1 = szPos2 + 1;
              /* Blocksize */
              szPos2 = strchr(szPos1, ME_KEY_PROPSEP_CN003);
              szPos1 = szPos2 + 1;
              /* Next Medium */
              szPos2 = strchr(szPos1, ME_KEY_PROPSEP_CN003);
              szPos1 = szPos2 + 1;
              /* Parallel */
              szPos2 = strchr(szPos1, ME_KEY_PROPSEP_CN003);
              if (szPos2) {
                strncpy(szValue, szPos1, szPos2 - szPos1);
                szValue[szPos2 - szPos1] = '\0';
                if (strcmp(szValue, ME_OLD_PROP_YES_CN003) == 0 ) {
                  SAPDB_strcpy(szValue, szParallelID);
                  strcat(szValue, BA_KEY_BAKSLASH_CN003);
                  strcat(szValue, pMedium->szName);
                  SAPDB_strcpy(pMedium->szName, szValue);
                }
              }
              szPos1 = szPos2 + 1;
              /* Description */
              szPos2 = strchr(szPos1, ME_KEY_PROPSEP_CN003);
              szPos1 = szPos2 + 1;
              /* OS Command */
              SAPDB_strcpy(pMedium->szOSCommand, szPos1);
              
              pMedium->bOld = true;

              *bFound = true;
            } else {
              bIgnore = false;
            }
          }
		    }
      }
    }
  } while ( ( rteFileError.sp5fe_result  == vf_ok             ) &&
            ( rteFileError.sp5fe_warning == sp5vfw_no_warning ) &&
            ( nFuncStatus                == OK_CN00           ) &&
            ( *bFound                    == false             )    );

  if (rteFileError.sp5fe_result != vf_ok) {
    if (rteFileError.sp5fe_result != vf_eof) {
      nFuncStatus = DBMSrvDep_DirtyReply(replyData, replyLen).startWithMessageList(DBMSrvMsg_RTEError(rteFileError));
    }
  }

  return nFuncStatus;
} /* end cn30_ReadNextOldMedium */

/* PRIVATE FUNCTION cn30_NewMedium
 * ------------------------------------------------------------------
 * purpose: alloc memory for new medium
 *
 */
static tcn00_Error cn30_NewMedium (
      char           * replyData,
      int            * replyLen,
      tcn003_Medium  ** pMedium )
{
    tcn00_Error rc=cn30NewMedium(*pMedium);
    
    if(OK_CN00!=rc)
        cn90AnswerIError(replyData, replyLen, rc);

    return rc;
} /* end cn30_NewMedium */

/* PRIVATE FUNCTION cn30_NewMediaGroup
 * ------------------------------------------------------------------
 * purpose: alloc memory for new mediagroup
 *
 */
static tcn00_Error cn30_NewMediaGroup (
      char                * replyData,
      int                 * replyLen,
      tcn003_MediaGroup   ** pMediaGroup )
{
    tcn00_Error rc=cn30NewMediaGroup(*pMediaGroup);

    if(OK_CN00!=rc)
        cn90AnswerIError(replyData, replyLen, rc);

    return rc;
} /* end cn30_NewMediaGroup */

/* PRIVATE FUNCTION cn30_FindMediumByName
 * ------------------------------------------------------------------
 * purpose: search a medium in a medialist by name
 *
 */
static tcn00_Error cn30_FindMediumByName (
      char                * replyData,
      int                 * replyLen,
      tcn003_Media        * pMedia,
      const char          * szMediumName,
      tcn003_MediaGroup   ** pMediaGroupFound,
      tcn003_Medium       ** pMediumFound )
{
  tcn00_Error	        nFuncStatus = OK_CN00;

  tcn003_MediaGroup   * pMediaGroup = NULL;
  tcn003_Medium       * pMedium = NULL;
  
  bool                bFound = false;

  /**/

  *pMediaGroupFound = NULL;
  *pMediumFound = NULL;

  if (pMedia != NULL) {

    pMediaGroup = pMedia->pFirstGroup;

    while ( (pMediaGroup != NULL) &&
            (bFound == false)    ) {

      pMedium = pMediaGroup->pFirst;

      while ( (pMedium != NULL) &&
              (bFound == false)    ) {

        if (cn90Stricmp(pMedium->szName,szMediumName) == 0) {
          bFound = true;
          *pMediaGroupFound = pMediaGroup;
          *pMediumFound = pMedium;
        } else {
          pMedium = pMedium->pNext;
        }
      }
      pMediaGroup = pMediaGroup->pNext;
    }
  }

  if (!bFound) {
    nFuncStatus = ERR_MENOTFOUND_CN00;
    cn90AnswerIError(replyData, replyLen, nFuncStatus);
  }

  *replyLen = (int)strlen(replyData);

  return nFuncStatus;
} /* cn30_FindMediumByName */

/* PRIVATE FUNCTION cn30_FindMediumInGroupByName
 * ------------------------------------------------------------------
 * purpose: search a medium in a mediagroup by name
 *
 */
static tcn00_Error cn30_FindMediumInGroupByName (
      char                * replyData,
      int                 * replyLen,
      tcn003_Media        * pMedia,
      tcn003_MediaGroup   * pMediaGroup,
      char                * szMediumName,
      tcn003_Medium       ** pMediumFound )
{
  tcn00_Error	        nFuncStatus = OK_CN00;

  tcn003_Medium       * pMedium = NULL;
  
  bool                bFound = false;

  /**/

  *pMediumFound = NULL;

  if (pMedia != NULL) {

    pMedium = pMediaGroup->pFirst;

    while ( (pMedium != NULL) &&
            (bFound == false)    ) {

      if (cn90Stricmp(pMedium->szName,szMediumName) == 0) {
        bFound = true;
        *pMediumFound = pMedium;
      } else {
        pMedium = pMedium->pNext;
      }
    }
  }

  if (!bFound) {
    nFuncStatus = ERR_MENOTFOUND_CN00;
    cn90AnswerIError(replyData, replyLen, nFuncStatus);
  }

  *replyLen = (int)strlen(replyData);

  return nFuncStatus;
} /* cn30_FindMediumInGroupByName */

/* PRIVATE FUNCTION cn30_FindMediaGroupByName
 * ------------------------------------------------------------------
 * purpose: search a mediagroup by name
 *
 */
static tcn00_Error cn30_FindMediaGroupByName (
      char                * replyData,
      int                 * replyLen,
      tcn003_Media        * pMedia,
      const char          * szGroupName,
      tcn003_MediaGroup   ** pMediaGroupFound )
{
  tcn00_Error	        nFuncStatus = OK_CN00;

  tcn003_MediaGroup   * pMediaGroup = NULL;
  
  bool                bFound = false;

  /**/

  *pMediaGroupFound = NULL;

  if (pMedia != NULL) {

    pMediaGroup = pMedia->pFirstGroup;
 
    while ( (pMediaGroup != NULL) &&
            (bFound == false)    ) {

      if (cn90Stricmp(pMediaGroup->szName,szGroupName) == 0) {
        bFound = true;
        *pMediaGroupFound = pMediaGroup;
      } else {
        pMediaGroup = pMediaGroup->pNext;
      }

    }
  }

  if (!bFound) {
    nFuncStatus = ERR_MENOTFOUND_CN00;
    cn90AnswerIError(replyData, replyLen, nFuncStatus);
  }

  *replyLen = (int)strlen(replyData);

  return nFuncStatus;
} /* cn30_FindMediaGroupByName */


/* PRIVATE FUNCTION cn30_SplitName
 * ------------------------------------------------------------------
 * purpose: split full medium name in to portions
 *
 */
void cn30_SplitName (
      char  * szFullName,
      char  * szGroupName,
      char  * szMediumName )
{
  char * pSep = NULL;

  /**/

  szGroupName[0] = '\0';
  szMediumName[0] = '\0';
  
  pSep = strchr(szFullName, ME_GROUP_NAME_SEP1_CN003);
  if (pSep == NULL) {
    pSep = strchr(szFullName, ME_GROUP_NAME_SEP2_CN003);
    if (pSep) {
      *pSep = ME_GROUP_NAME_SEP1_CN003;
    }
  }
  if (pSep) {
    strncpy(szGroupName, szFullName, pSep - szFullName);
    szGroupName[pSep - szFullName] = '\0';
    SAPDB_strcpy(szMediumName, pSep + 1);
  } else {
    SAPDB_strcpy(szMediumName, szFullName);
  }
} /* end cn30_SplitName */

/* PRIVATE FUNCTION cn30_AddMedium
 * ------------------------------------------------------------------
 * purpose: add medium to media struct
 *
 */
static tcn00_Error cn30_AddMedium (
      char           * replyData,
      int            * replyLen,
      tcn003_Media   * pMedia,
      tcn003_Medium  * pMedium )
{
  tcn00_Error	        nFuncStatus = OK_CN00;

  tcn003_MediaGroup   * pMediaGroup = NULL;

  bool                bMallocOK = true;

  bool                bOK = false;

  char                szGroupName[ME_NAME_LENGTH_MXCN003];
  char                szMediumName[ME_NAME_LENGTH_MXCN003];

  /**/

  cn30_SplitName(pMedium->szName, szGroupName, szMediumName);

  nFuncStatus= cn30_FindMediaGroupByName(replyData, replyLen, pMedia, szGroupName, &pMediaGroup);

  if ((nFuncStatus == ERR_MENOTFOUND_CN00) && 
     (pMediaGroup == NULL)                    ) {

    nFuncStatus = OK_CN00;
    nFuncStatus = cn30_NewMediaGroup(replyData, replyLen, &pMediaGroup);

    if (nFuncStatus == OK_CN00) {
      SAPDB_strcpy(pMediaGroup->szName,szGroupName);

      /* add the mediagroup to the list of media */
      if (pMedia->pFirstGroup == NULL) {
        pMedia->pFirstGroup = pMediaGroup;
      } 
      if (pMedia->pLastGroup != NULL) {
        pMedia->pLastGroup->pNext = pMediaGroup;
      } 
      pMediaGroup->pNext = NULL;
      pMediaGroup->pPrev = pMedia->pLastGroup;
      pMedia->pLastGroup = pMediaGroup;
    }
  }

  if (nFuncStatus == OK_CN00) {
    /* add the medium to the mediagroup */
    if (pMediaGroup->pFirst == NULL) {
      pMediaGroup->pFirst = pMedium;
    } 
    if (pMediaGroup->pLast != NULL) {
      pMediaGroup->pLast->pNext = pMedium;
    } 
    pMedium->pNext = NULL;
    pMedium->pPrev = pMediaGroup->pLast;
    pMediaGroup->pLast = pMedium;
  }
  
  return nFuncStatus;
} /* end cn30_AddMedium */

/* PRIVATE FUNCTION cn30_ReadMedia
 * ------------------------------------------------------------------
 * purpose: read the media file to media struct
 *
 */
static tcn00_Error cn30_ReadMedia (
      const tsp00_DbNamec & szDbName,
      char                * replyData,
      int                 * replyLen,
      int                   replyLenMax,
      tcn003_Media        * pMedia,
      bool                  bNewMedia,
      bool                  bOldMedia )
{
  tcn00_Error	          nFuncStatus       = OK_CN00;
  tsp00_Int4              hMediaFile        = 0;
  tcn003_Medium       * pMedium           = NULL;
  tcn003_Medium       * pMediumExists     = NULL;
  tcn003_MediaGroup   * pMediaGroupExists = NULL;
  char                  szParallelID[ME_NAME_LENGTH_MXCN003] = "";
  bool                  bMallocOK         = true;
  bool                  bOK               = false;

  /**/

  cn30_InitMedia(pMedia);

  if (bNewMedia) {
    char * mediaFileName=0;

    /* read new media file */
    nFuncStatus = cn30_OpenMediaFile(szDbName, replyData, replyLen, false, &hMediaFile, mediaFileName);

    if (nFuncStatus == OK_CN00) {
        int line=0;

      do {
        nFuncStatus = cn30_NewMedium(replyData, replyLen, &pMedium);
        SAPDB_strcpy(pMedium->szExternal, ""); //make sure medium name is used for those media, where external property is not set (e.g. with old "new" media file)

        if (nFuncStatus == OK_CN00) {
          nFuncStatus = cn30_ReadNextMedium(pMedium, replyData, replyLen, replyLenMax, hMediaFile, true, &bOK, line, mediaFileName);
        
          if ( (nFuncStatus == OK_CN00) && 
               (bOK)                ) {

            cn30_AdjustExternalProperty(pMedium, replyData, replyLen, 1);
            nFuncStatus = cn30_AddMedium(replyData, replyLen, pMedia, pMedium);

          } else {
            if (pMedium != NULL) {
              cn00MyFree(pMedium);
            }
          }
        }
      } while ( (nFuncStatus == OK_CN00) &&
                (bOK)               );

      if (nFuncStatus == OK_CN00)
         nFuncStatus = cn30_CloseMediaFile(replyData, replyLen, hMediaFile);
      else
         cn30_CloseMediaFile(replyData, replyLen, hMediaFile);
    }

    cn36_StrDealloc(mediaFileName);
  }

  /* read old media file */
  if ( (nFuncStatus == OK_CN00) && 
       (bOldMedia)                 )
  {
      bool mediaFileIsOpen=false;

    nFuncStatus = cn30_OpenOldMediaFile(szDbName, replyData, replyLen, false, &hMediaFile);

    mediaFileIsOpen=(nFuncStatus==OK_CN00);

    if (nFuncStatus == OK_CN00) {
      nFuncStatus = cn30_ReadParallelIDFromOldMediaFile(replyData, replyLen, hMediaFile, szParallelID);
      if (nFuncStatus == OK_CN00) {
        if (szParallelID[0] == '\0') {
          SAPDB_strcpy(szParallelID, ME_KEY_PARALLEL_CN003);
        }
      }
    }

    if (nFuncStatus == OK_CN00) {

      do {
  
        nFuncStatus = cn30_NewMedium(replyData, replyLen, &pMedium);
        if (nFuncStatus == OK_CN00) {
  
          nFuncStatus = cn30_ReadNextOldMedium(pMedium, replyData, replyLen, hMediaFile, true, &bOK, szParallelID);
        
          if ( (nFuncStatus == OK_CN00) && 
               (bOK)                ) {

            nFuncStatus = cn30_FindMediumByName(replyData, replyLen, pMedia, pMedium->szName, &pMediaGroupExists, &pMediumExists);

            if (nFuncStatus == ERR_MENOTFOUND_CN00) {
              nFuncStatus = cn30_AddMedium(replyData, replyLen, pMedia, pMedium);
            }
          }

          if (nFuncStatus != OK_CN00) {
            if (pMedium != NULL) {
              cn00MyFree(pMedium);
            }
          }

        }
      } while ( (nFuncStatus == OK_CN00) &&
                (bOK)               );
    }

    if(mediaFileIsOpen)
    {
        if (nFuncStatus == OK_CN00)
            nFuncStatus = cn30_CloseMediaFile(replyData, replyLen, hMediaFile);
        else
            cn30_CloseMediaFile(replyData, replyLen, hMediaFile);
    }
  }
  
  return nFuncStatus;
} /* end cn30_ReadMedia */

/* PRIVATE FUNCTION cn30_WriteMedium
 * ------------------------------------------------------------------
 * purpose: write medium to media file
 *
 */
static tcn00_Error cn30_WriteMedium (
      char           * replyData,
      int            * replyLen,
      tsp00_Int4       hMediaFile,
      tcn003_Medium  * pMedium )
{
  tcn00_Error          nFuncStatus  = OK_CN00;
  tsp05_RteFileError   rteFileError;

  char                 szLine[ME_LINE_LENGTH_MXCN003 + 1];

  /**/

  /* [<name>] */
  SAPDB_strcpy(szLine, "[");
  strcat(szLine, pMedium->szName);
  strcat(szLine, "]");
  strcat(szLine, "\0");
  sqlfwritec(hMediaFile, szLine, -1, &rteFileError);

  /* LOCATION */
  if (rteFileError.sp5fe_result == vf_ok) {
    SAPDB_strcpy(szLine, ME_KEY_LOCATION_CN003);
    strcat(szLine, "=");
    strcat(szLine, pMedium->szLocation);
    strcat(szLine, "\0");
    sqlfwritec(hMediaFile, szLine, -1, &rteFileError);
  }
  /* TYPE */
  if (rteFileError.sp5fe_result == vf_ok) {
    SAPDB_strcpy(szLine, ME_KEY_TYPE_CN003);
    strcat(szLine, "=");
    strcat(szLine, pMedium->szType);
    strcat(szLine, "\0");
    sqlfwritec(hMediaFile, szLine, -1, &rteFileError);
  }
  /* SAVETYPE */
  if (rteFileError.sp5fe_result == vf_ok) {
    SAPDB_strcpy(szLine, ME_KEY_SAVETYPE_CN003);
    strcat(szLine, "=");
    strcat(szLine, pMedium->szSaveType);
    strcat(szLine, "\0");
    sqlfwritec(hMediaFile, szLine, -1, &rteFileError);
  }
  /* SIZE */
  if (rteFileError.sp5fe_result == vf_ok) {
    SAPDB_strcpy(szLine, ME_KEY_SIZE_CN003);
    strcat(szLine, "=");
    strcat(szLine, pMedium->szSize);
    strcat(szLine, "\0");
    sqlfwritec(hMediaFile, szLine, -1, &rteFileError);
  }
  /* BLOCKSIZE */
  if (rteFileError.sp5fe_result == vf_ok) {
    SAPDB_strcpy(szLine, ME_KEY_BLOCKSIZE_CN003);
    strcat(szLine, "=");
    strcat(szLine, pMedium->szBlocksize);
    strcat(szLine, "\0");
    sqlfwritec(hMediaFile, szLine, -1, &rteFileError);
  }
  /* OVERWRITE */
  if (rteFileError.sp5fe_result == vf_ok) {
    SAPDB_strcpy(szLine, ME_KEY_OVERWRITE_CN003);
    strcat(szLine, "=");
    strcat(szLine, pMedium->szOverwrite);
    strcat(szLine, "\0");
    sqlfwritec(hMediaFile, szLine, -1, &rteFileError);
  }
  /* AUTOLOADER */
  if (rteFileError.sp5fe_result == vf_ok) {
    SAPDB_strcpy(szLine, ME_KEY_AUTOLOADER_CN003);
    strcat(szLine, "=");
    strcat(szLine, pMedium->szAutoLoader);
    strcat(szLine, "\0");
    sqlfwritec(hMediaFile, szLine, -1, &rteFileError);
  }
  /* OSCOMMAND */
  if (rteFileError.sp5fe_result == vf_ok) {
    SAPDB_strcpy(szLine, ME_KEY_OSCOMMAND_CN003);
    strcat(szLine, "=");
    strcat(szLine, pMedium->szOSCommand);
    strcat(szLine, "\0");
    sqlfwritec(hMediaFile, szLine, -1, &rteFileError);
  }
  /* EXTERNAL */
  if (rteFileError.sp5fe_result == vf_ok) {
    SAPDB_strcpy(szLine, ME_KEY_BACKUPTOOL_CN003);
    strcat(szLine, "=");
    strcat(szLine, pMedium->szExternal);
    strcat(szLine, "\0");
    sqlfwritec(hMediaFile, szLine, -1, &rteFileError);
  }
  /* CREATED */
  if (rteFileError.sp5fe_result == vf_ok) {
    SAPDB_strcpy(szLine, ME_KEY_CREATED_CN003);
    strcat(szLine, "=");
    strcat(szLine, pMedium->szCreated);
    strcat(szLine, "\0");
    sqlfwritec(hMediaFile, szLine, -1, &rteFileError);
  }
  /* MODIFIED */
  if (rteFileError.sp5fe_result == vf_ok) {
    SAPDB_strcpy(szLine, ME_KEY_MODIFIED_CN003);
    strcat(szLine, "=");
    strcat(szLine, pMedium->szModified);
    strcat(szLine, "\0");
    sqlfwritec(hMediaFile, szLine, -1, &rteFileError);
  }
  if (rteFileError.sp5fe_result == vf_ok) {
    SAPDB_strcpy(szLine, "\0");
    sqlfwritec(hMediaFile, szLine, -1, &rteFileError);
  }

  if (rteFileError.sp5fe_result != vf_ok) {
    nFuncStatus = DBMSrvDep_DirtyReply(replyData, replyLen).startWithMessageList(DBMSrvMsg_RTEError(rteFileError));
  }

  return nFuncStatus;
} /* end cn30_WriteMedium */

/* PRIVATE FUNCTION cn30_WriteMediaGroup
 * ------------------------------------------------------------------
 * purpose: write the mediagroup struct to media file
 *
 */
static tcn00_Error cn30_WriteMediaGroup (
      char                * replyData,
      int                 * replyLen,
      tsp00_Int4            hMediaFile,
      tcn003_MediaGroup   * pMediaGroup,
      bool                bMigrate)
{
  tcn00_Error	        nFuncStatus = OK_CN00;

  tcn003_Medium       * pMedium = NULL;

  /**/

  if (nFuncStatus == OK_CN00) {

    if (pMediaGroup != NULL) {
      pMedium = pMediaGroup->pFirst;
      while ( (pMedium != NULL)          &&
              (nFuncStatus == OK_CN00) )    {
   
        if (!pMedium->bOld) {
          nFuncStatus = cn30_WriteMedium(replyData, replyLen, hMediaFile, pMedium);
        } else {
          if (bMigrate) {
            nFuncStatus = cn30_WriteMedium(replyData, replyLen, hMediaFile, pMedium);
          }
        }

        if (nFuncStatus == OK_CN00) {
          pMedium = pMedium->pNext;
        }
      } 
    } 
  }

  return nFuncStatus;
} /* end cn30_WriteMediaGroup */

/* PRIVATE FUNCTION cn30_WriteMedia
 * ------------------------------------------------------------------
 * purpose: write the media struct to media file
 *
 */
static tcn00_Error cn30_WriteMedia (
      const tsp00_DbNamec  & szDbName,
      char                 * replyData,
      int                  * replyLen,
      tcn003_Media         * pMedia,
      bool                 bMigrate)
{
  tcn00_Error	        nFuncStatus = OK_CN00;

  tsp00_Int4            hMediaFile;
  tcn003_MediaGroup   * pMediaGroup = NULL;
  char                * mediaFileName=0;
  /**/

  nFuncStatus = cn30_OpenMediaFile(szDbName, replyData, replyLen, true, &hMediaFile, mediaFileName);

  cn36_StrDealloc(mediaFileName);

  if (nFuncStatus == OK_CN00) {

    if (pMedia != NULL) {
      
      pMediaGroup = pMedia->pFirstGroup;
      while ( (pMediaGroup != NULL)          &&
              (nFuncStatus == OK_CN00) )    {
   
        nFuncStatus = cn30_WriteMediaGroup(replyData, replyLen, hMediaFile, pMediaGroup, bMigrate);

        if (nFuncStatus == OK_CN00) {
          pMediaGroup = pMediaGroup->pNext;
        }
      } 
    } 
  }
    
  if (nFuncStatus == OK_CN00) {
    nFuncStatus = cn30_CloseMediaFile(replyData, replyLen, hMediaFile);
  } // end if

  // backup the file
  DBMSrvMsg_Error      oError;
  cn42BackupFile(szDbName, FGET_DBMMDF_CN42, oError);

  return nFuncStatus;
} /* end cn30_WriteMedia */

/* PRIVATE FUNCTION cn30_FreeMediaGroup
 * ------------------------------------------------------------------
 * purpose: frees the mediagroup structure
 *
 */
void cn30_FreeMediaGroup (
      tcn003_MediaGroup   * pMediaGroup )
{
  tcn003_Medium * pMedium  = NULL;
  tcn003_Medium * pNext    = NULL;

  if (pMediaGroup != NULL) {
    pMedium = pMediaGroup->pFirst;
    while (pMedium != NULL) {
      pNext = pMedium->pNext;
      cn00MyFree(pMedium);
      pMedium = pNext;
    } 
  } 
} /* end cn30_FreeMediaGroup */

/* PRIVATE FUNCTION cn30_FreeMedia
 * ------------------------------------------------------------------
 * purpose: frees the media structure
 *
 */
void cn30_FreeMedia (
      tcn003_Media   * pMedia )
{
  tcn003_MediaGroup * pMediaGroup = NULL;
  tcn003_MediaGroup * pNextMediaGroup = NULL;

  if (pMedia != NULL) {
    pMediaGroup = pMedia->pFirstGroup;

    while (pMediaGroup != NULL) {
      cn30_FreeMediaGroup(pMediaGroup);
      pNextMediaGroup = pMediaGroup->pNext;
      cn00MyFree(pMediaGroup);
      pMediaGroup = pNextMediaGroup;
    } 
  } 
} /* end cn30_FreeMedia */

/* PRIVATE FUNCTION cn30_RemoveDoublequotes
 * ------------------------------------------------------------------
 * purpose: remove doublequotes from a string
 *
 */
void cn30_RemoveDoublequotes(
      char * szValue)
{
  long nLength = 0;

  nLength = (long)strlen(szValue);

  if (nLength > 1) {
    if ((szValue[0]           == '"') &&
        (szValue[nLength - 1] == '"')    ) {
      strncpy(szValue, szValue+1, nLength-2);
      szValue[nLength-2] = '\0';
    } 
  }
} /* end cn30_RemoveDoublequotes */

/* PRIVATE FUNCTION cn30_GetDateString
 * ------------------------------------------------------------------
 * purpose: get string from actual date
 *
 */
void cn30_GetDateString(
      char * szDateTime)
{
  tsp00_Date date;
	tsp00_Time time;

  sqldattime(date, time);

  strncpy(szDateTime, date, 8);
  strncpy(szDateTime + 8, (const char *)time + 2, 6);
  szDateTime[14] = '\0';
} /* end cn30_DateString */

/* PRIVATE FUNCTION cn30_PrintMediumString
 * ------------------------------------------------------------------
 * purpose: prints a medium as string into the reply buffer
 * \return 1 if replyData not full, 0 otherwise
 */

int cn30_PrintMediumString(tcn003_Medium *  Medium,
                           char          *& CurrentPosition,
                           char          *  replyData,
                           int              replyLenMax )
{
    int  rc=0;
    char szOld[4];

    if(Medium->bOld)
        SAPDB_strcpy(szOld ,"OLD");
    else
        SAPDB_strcpy(szOld,"");

    if((ME_NAME_LENGTH_MXCN003       +     // name
        ME_LOCATION_LENGTH_MXCN003   +     // location
        ME_TYPE_LENGTH_MXCN003       +     // type
        ME_SAVETYPE_LENGTH_MXCN003   +     // savetype
        ME_SIZE_LENGTH_MXCN003       +     // size
        ME_BLOCKSIZE_LENGTH_MXCN003  +     // blocksize
        ME_OVERWRITE_LENGTH_MXCN003  +     // overwrite
        ME_AUTOLOADER_LENGTH_MXCN003 +     // autoloader
        ME_OSCMD_LENGTH_MXCN003      +     // os command
        ME_DATE_LENGTH_MXCN003       +     // created
        ME_DATE_LENGTH_MXCN003       +     // modified
        3                            +     // OLD
        ME_BACKUPTOOL_LENGTH_MXCN003 +     // backup tool
        11*strlen(VALUE_SEPSTRING_CN00) +
        strlen(LINE_SEPSTRING_CN00)  +
        1 + (CurrentPosition - replyData)) <= (size_t)replyLenMax)
    {
        rc=1;

        sprintf(CurrentPosition, "%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s",
                Medium->szName, VALUE_SEPSTRING_CN00,
                Medium->szLocation, VALUE_SEPSTRING_CN00,
                Medium->szType, VALUE_SEPSTRING_CN00,
                Medium->szSaveType, VALUE_SEPSTRING_CN00,
                Medium->szSize, VALUE_SEPSTRING_CN00,
                Medium->szBlocksize, VALUE_SEPSTRING_CN00,
                Medium->szOverwrite, VALUE_SEPSTRING_CN00,
                Medium->szAutoLoader, VALUE_SEPSTRING_CN00,
                Medium->szOSCommand, VALUE_SEPSTRING_CN00,
                Medium->szCreated, VALUE_SEPSTRING_CN00,
                Medium->szModified, VALUE_SEPSTRING_CN00,
                szOld, VALUE_SEPSTRING_CN00,
                Medium->szExternal, LINE_SEPSTRING_CN00);

        CurrentPosition+=strlen(CurrentPosition);
    }

    return rc;
} // end cn30_PrintMediumString

/* SPECIFICATION PUBLIC FUNCTIONS
 * ------------------------------------------------------------------
 */

/* PUBLIC FUNCTION cn30BackupMediaGet
 * ------------------------------------------------------------------
 * purpose: get a specified medium from media file
 *
 */
tcn00_Error cn30BackupMediaGet (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax )
{
  tcn00_Error	        nFuncStatus = OK_CN00;

  char                * pCurrPos;

  char                szFullName[ME_NAME_LENGTH_MXCN003];
  char                szGroupName[ME_NAME_LENGTH_MXCN003];
  char                szMediumName[ME_NAME_LENGTH_MXCN003];

  bool                bExists;
  bool                bOldExists;
  char                szDate[64];

  tcn003_Media        tMedia;

  tcn003_MediaGroup   * pMediaGroup = NULL;
  tcn003_Medium       * pMedium = NULL;

  bool                bFound = false;
  
  /**/

  cn30_InitMedia(&tMedia);

  if (nFuncStatus == OK_CN00) {
    if (cn90GetToken(command->args, szFullName, 1, ME_NAME_LENGTH_MXCN003)) {
      cn30_RemoveDoublequotes(szFullName);
    } else {
      nFuncStatus = ERR_PARAM_CN00;
      cn90AnswerIError(replyData, replyLen, nFuncStatus);
    }
  }

  if (nFuncStatus == OK_CN00) {
    nFuncStatus = cn30_CheckMediaFile(vcontrol->dbname, replyData, replyLen, &bExists, &bOldExists, szDate);
  }

  if (nFuncStatus == OK_CN00) {
    if ( (!bExists) && (!bOldExists) ) {
      nFuncStatus = ERR_NOMEDIA_CN00;
      cn90AnswerIError(replyData, replyLen, nFuncStatus);
    } else {
      nFuncStatus = cn30_ReadMedia(vcontrol->dbname, replyData, replyLen, replyLenMax, &tMedia, bExists, bOldExists);
    }
  }

  if (nFuncStatus == OK_CN00) {
  
    cn30_SplitName(szFullName, szGroupName, szMediumName);
    
    sprintf(replyData, "%s%s", ANSWER_OK_CN00, LINE_SEPSTRING_CN00);
    pCurrPos = replyData + strlen(replyData);
    
    pMediaGroup = tMedia.pFirstGroup;

    while ( (pMediaGroup != NULL) && 
            (bFound == false)        )
    {
        pMedium=pMediaGroup->pFirst;

        while((pMedium != NULL) && 
              (bFound == false)    )
        {
            if(cn90Stricmp(szFullName, pMedium->szName) == 0)
            {
                if(!cn30_PrintMediumString(pMedium, pCurrPos, replyData, replyLenMax))
                    nFuncStatus=cn90AnswerIError(replyData, replyLen, ERR_TOOSMALL_CN00);

                bFound=true;
            }

            pMedium=pMedium->pNext;
        }

        pMediaGroup=pMediaGroup->pNext;
    }
  }

  if (nFuncStatus == OK_CN00) {
    if (bFound == false) {
      nFuncStatus = ERR_MENOTFOUND_CN00;
      cn90AnswerIError(replyData, replyLen, nFuncStatus);
    }
  }
  
  cn30_FreeMedia(&tMedia);

  /**/

  *replyLen = (int)strlen(replyData);

  return nFuncStatus;
} /* end cn30BackupMediaGet */

/* PUBLIC FUNCTION cn30BackupMediaGetAll
 * ------------------------------------------------------------------
 * purpose: get media from media file
 *
 */
tcn00_Error cn30BackupMediaGetAll (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax )
{
  tcn00_Error	        nFuncStatus = OK_CN00;

  char                * pCurrPos;

  bool                bExists;
  bool                bOldExists;
  char                szDate[64];

  tcn003_Media        tMedia;

  tcn003_MediaGroup   * pMediaGroup = NULL;
  tcn003_Medium       * pMedium = NULL;

  /**/

  cn30_InitMedia(&tMedia);

  if (nFuncStatus == OK_CN00) {
    nFuncStatus = cn30_CheckMediaFile(vcontrol->dbname, replyData, replyLen, &bExists, &bOldExists, szDate);
  }

  if (nFuncStatus == OK_CN00) {
    if ( (!bExists) && (!bOldExists) ) {
      nFuncStatus = ERR_NOMEDIA_CN00;
      cn90AnswerIError(replyData, replyLen, nFuncStatus);
    } else {
      nFuncStatus = cn30_ReadMedia(vcontrol->dbname, replyData, replyLen, replyLenMax, &tMedia, bExists, bOldExists);
    }
  }

  if (nFuncStatus == OK_CN00) {
    sprintf(replyData, "%s%s", ANSWER_OK_CN00, LINE_SEPSTRING_CN00);
    pCurrPos = replyData + strlen(replyData);

    pMediaGroup = tMedia.pFirstGroup;

    while ((nFuncStatus == OK_CN00) && (pMediaGroup != NULL)) {

      pMedium = pMediaGroup->pFirst;

      while ((nFuncStatus == OK_CN00) && (pMedium != NULL)) {
      
        if(!cn30_PrintMediumString(pMedium, pCurrPos, replyData, replyLenMax))
            nFuncStatus=cn90AnswerIError(replyData, replyLen, ERR_TOOSMALL_CN00);

        pMedium = pMedium->pNext;

      }

      pMediaGroup = pMediaGroup->pNext;

    }
  }

  cn30_FreeMedia(&tMedia);

  /**/

  *replyLen = (int)strlen(replyData);

  return nFuncStatus;
} /* end cn30BackupMediaGetAll */

/* PUBLIC FUNCTION cn30BackupMediaMigrate
 * ------------------------------------------------------------------
 * purpose: migrate the old media in the new media format
 *
 */
tcn00_Error cn30BackupMediaMigrate (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax )
{
  tcn00_Error	        nFuncStatus = OK_CN00;

  bool                bExists;
  bool                bOldExists;
  char                szDate[64];

  tcn003_Media        tMedia;

  tcn003_MediaGroup   * pMediaGroup = NULL;
  tcn003_Medium       * pMedium = NULL;

  /**/

  cn30_InitMedia(&tMedia);

  if (nFuncStatus == OK_CN00) {
    nFuncStatus = cn30_CheckMediaFile(vcontrol->dbname, replyData, replyLen, &bExists, &bOldExists, szDate);
  }

  if (nFuncStatus == OK_CN00) {
    if ( (!bExists) && (!bOldExists) ) {
      nFuncStatus = ERR_NOMEDIA_CN00;
      cn90AnswerIError(replyData, replyLen, nFuncStatus);
    } else {
      nFuncStatus = cn30_ReadMedia(vcontrol->dbname, replyData, replyLen, replyLenMax, &tMedia, bExists, bOldExists);
    }
  }

  if (nFuncStatus == OK_CN00) {
    if (bOldExists) {
      nFuncStatus = cn30_WriteMedia(vcontrol->dbname, replyData, replyLen, &tMedia, true);
    }
  }

  if (nFuncStatus == OK_CN00) {
    sprintf(replyData, "%s%s", ANSWER_OK_CN00, LINE_SEPSTRING_CN00);
  }

  cn30_FreeMedia(&tMedia);

  /**/

  *replyLen = (int)strlen(replyData);

  return nFuncStatus;
} /* end cn30BackupMediaMigrate */
 

/* PUBLIC FUNCTION cn30BackupMediaPut
 * ------------------------------------------------------------------
 * purpose: write medium definition to media file
 *
 */
tcn00_Error cn30BackupMediaPut (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax )
{
  tcn00_Error	        nFuncStatus = OK_CN00;

  bool                bExists;
  bool                bOldExists;
  char                szDate[64];

  tcn003_Medium       * pMediumPut = NULL;
  tcn003_MediaGroup   * pMediaGroup = NULL;
  tcn003_Medium       * pMedium = NULL;
  tcn003_Medium       * pMediumOld = NULL;

  tcn003_Media        tMedia;

  char                szGroupName[ME_NAME_LENGTH_MXCN003];
  char                szMediumName[ME_NAME_LENGTH_MXCN003];
  
  bool                bFound = false;

  /**/

  cn30_InitMedia(&tMedia);

  nFuncStatus = cn30_NewMedium(replyData, replyLen, &pMediumPut);

  if (nFuncStatus == OK_CN00) {
    if (cn90GetToken(command->args, pMediumPut->szName, 1, ME_NAME_LENGTH_MXCN003 + 1)) {
      cn30_RemoveDoublequotes(pMediumPut->szName);
    } else {
      nFuncStatus = ERR_PARAM_CN00;
      cn90AnswerIError(replyData, replyLen, nFuncStatus);
    }
  }
  if (nFuncStatus == OK_CN00) {
    if (cn90GetToken(command->args, pMediumPut->szLocation, 2, ME_LOCATION_LENGTH_MXCN003 + 1)) {
      cn30_RemoveDoublequotes(pMediumPut->szLocation);
    } else {
      nFuncStatus = ERR_PARAM_CN00;
      cn90AnswerIError(replyData, replyLen, nFuncStatus);
    }
  }  

  if (nFuncStatus == OK_CN00) {
    cn90GetToken(command->args, pMediumPut->szType, 3, ME_TYPE_LENGTH_MXCN003 + 1);
    cn90GetToken(command->args, pMediumPut->szSaveType, 4, ME_SAVETYPE_LENGTH_MXCN003 + 1);
    cn90GetToken(command->args, pMediumPut->szSize, 5, ME_SIZE_LENGTH_MXCN003 + 1);
    cn90GetToken(command->args, pMediumPut->szBlocksize, 6, ME_BLOCKSIZE_LENGTH_MXCN003 + 1);
    cn90GetToken(command->args, pMediumPut->szOverwrite, 7, ME_OVERWRITE_LENGTH_MXCN003 + 1);
    cn90GetToken(command->args, pMediumPut->szAutoLoader, 8, ME_AUTOLOADER_LENGTH_MXCN003 + 1);
    if (cn90GetToken(command->args, pMediumPut->szOSCommand, 9, ME_OSCMD_LENGTH_MXCN003 + 1)) {
      cn30_RemoveDoublequotes(pMediumPut->szOSCommand);
    }
    cn90GetToken(command->args, pMediumPut->szExternal, 10, ME_BACKUPTOOL_LENGTH_MXCN003 + 1);
  }

  /**/

  if (nFuncStatus == OK_CN00) {
    nFuncStatus = cn30_CheckMedium(pMediumPut, replyData, replyLen, replyLenMax);
  }

  /**/

  if (nFuncStatus == OK_CN00) {
    nFuncStatus = cn30_CheckMediaFile(vcontrol->dbname, replyData, replyLen, &bExists, &bOldExists, szDate);
  }

  if (nFuncStatus == OK_CN00) {
    if ( (bExists) || (bOldExists) ) {
      nFuncStatus = cn30_ReadMedia(vcontrol->dbname, replyData, replyLen, replyLenMax, &tMedia, bExists, bOldExists);
    }
  }

  if (nFuncStatus == OK_CN00) {
    nFuncStatus = cn30_CheckAutosave(pMediumPut, &tMedia);
  }

  if (nFuncStatus == OK_CN00) {

    cn30_SplitName(pMediumPut->szName, szGroupName, szMediumName);

    if('\0'==szGroupName[0]) //if new medium is not part of a group
    {
        //check if medium name is already used for a group
        nFuncStatus=cn30_FindMediaGroupByName(replyData, replyLen, &tMedia, szMediumName, &pMediaGroup);

        if(nFuncStatus==OK_CN00 && 0!=pMediaGroup)
            nFuncStatus=cn90AnswerIError(replyData, replyLen, ERR_MEEXISTSASGROUP_CN00);
        else
            nFuncStatus=OK_CN00; // must have been ERR_MENOTFOUND_CN00, which is perfect
    }
    else    //the new medium is part of a group
    {
        //check if the group name is already used by a medium
        nFuncStatus=cn30_FindMediaGroupByName(replyData, replyLen, &tMedia, "", &pMediaGroup);

        if(nFuncStatus==OK_CN00 && 0!=pMediaGroup)
        {
            nFuncStatus=cn30_FindMediumInGroupByName(replyData, replyLen, &tMedia, pMediaGroup, szGroupName, &pMedium);

            if(nFuncStatus==OK_CN00 && 0!=pMedium)
                nFuncStatus=cn90AnswerIError(replyData, replyLen, ERR_MEEXISTSASMEDIUM_CN00);
            else
                nFuncStatus=OK_CN00; // must have been ERR_MENOTFOUND_CN00, which is perfect
        }
        else
            nFuncStatus=OK_CN00;
    }

    pMedium=0;      //reinit 
    pMediaGroup=0;  //reinit

    if(nFuncStatus == OK_CN00)
    {
        nFuncStatus= cn30_FindMediaGroupByName(replyData, replyLen, &tMedia, szGroupName, &pMediaGroup);

        if ((nFuncStatus == ERR_MENOTFOUND_CN00) && 
           (pMediaGroup == NULL)                    ) {

          nFuncStatus = OK_CN00;
          nFuncStatus = cn30_NewMediaGroup(replyData, replyLen, &pMediaGroup);

          if (nFuncStatus == OK_CN00) {
            SAPDB_strcpy(pMediaGroup->szName,szGroupName);

            /* add the mediagroup to the list of media */
            if (tMedia.pFirstGroup == NULL) {
              tMedia.pFirstGroup = pMediaGroup;
            } 
            if (tMedia.pLastGroup != NULL) {
              tMedia.pLastGroup->pNext = pMediaGroup;
            } 
            pMediaGroup->pNext = NULL;
            pMediaGroup->pPrev = tMedia.pLastGroup;
            tMedia.pLastGroup = pMediaGroup;
          }
        } else {
          if (szGroupName[0] != '\0') {
            nFuncStatus = cn30_CheckMediaGroup(pMediumPut, pMediaGroup);
          }
        }

        nFuncStatus = cn30_FindMediumInGroupByName(replyData, replyLen, &tMedia, pMediaGroup, pMediumPut->szName, &pMedium);
    }

    if (nFuncStatus == OK_CN00) {

      /* replace medium */

      cn30_GetDateString(pMediumPut->szModified);

      SAPDB_strcpy(pMediumPut->szCreated, pMedium->szCreated);
 
      if (pMedium->pPrev != NULL) {
        pMedium->pPrev->pNext = pMediumPut;
        pMediumPut->pPrev = pMedium->pPrev;
      } else {
        pMediaGroup->pFirst = pMediumPut;
      }
      if (pMedium->pNext != NULL) {
        pMedium->pNext->pPrev = pMediumPut;
        pMediumPut->pNext = pMedium->pNext;
      } else {
        pMediaGroup->pLast = pMediumPut;
      }
      pMediumPut = pMedium;
    } else {
      if (nFuncStatus == ERR_MENOTFOUND_CN00) {
        nFuncStatus = OK_CN00;

        /* append medium */

        cn30_GetDateString(pMediumPut->szCreated);
        cn30_GetDateString(pMediumPut->szModified);
        
        if (pMediaGroup->pLast != NULL) {
          pMediaGroup->pLast->pNext = pMediumPut;
          pMediumPut->pPrev = pMediaGroup->pLast;
        } else {
          pMediaGroup->pFirst = pMediumPut;
          pMediaGroup->pLast = pMediumPut;
        }
        pMediumPut = NULL;
      }
    } 
  }

  if (nFuncStatus == OK_CN00) {
    nFuncStatus = cn30_WriteMedia(vcontrol->dbname, replyData, replyLen, &tMedia, false);
  }

  if (pMediumPut != NULL) {
      cn00MyFree(pMediumPut);
  }

  cn30_FreeMedia(&tMedia);

  /**/

  if (nFuncStatus == OK_CN00) {
    sprintf(replyData, "%s%s", ANSWER_OK_CN00, LINE_SEPSTRING_CN00);
  }

  *replyLen = (int)strlen(replyData);

  return nFuncStatus;
} /* end cn30BackupMediaPut */

/* PUBLIC FUNCTION cn30BackupMediaDelete
 * ------------------------------------------------------------------
 * purpose: delete medium definition in media file
 *
 */
tcn00_Error cn30BackupMediaDelete (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax )
{
  tcn00_Error	        nFuncStatus = OK_CN00;

  char                * pCurrPos;

  bool                bExists;
  bool                bOldExists;
  char                szDate[64];

  char                szFullName[ME_NAME_LENGTH_MXCN003];
  char                szGroupName[ME_NAME_LENGTH_MXCN003];
  char                szMediumName[ME_NAME_LENGTH_MXCN003];

  tcn003_MediaGroup   * pMediaGroup = NULL;
  tcn003_Medium       * pMedium = NULL;
  tcn003_Medium       * pMediumOld = NULL;

  tcn003_Media        tMedia;

  bool                bFound = false;

  /**/

  cn30_InitMedia(&tMedia);

  if (nFuncStatus == OK_CN00) {
    if (cn90GetToken(command->args, szFullName, 1, ME_NAME_LENGTH_MXCN003)) {
      cn30_RemoveDoublequotes(szFullName);
    } else {
      nFuncStatus = ERR_PARAM_CN00;
      cn90AnswerIError(replyData, replyLen, nFuncStatus);
    }
  }

  if (nFuncStatus == OK_CN00) {
    nFuncStatus = cn30_CheckMediaFile(vcontrol->dbname, replyData, replyLen, &bExists, &bOldExists, szDate);
  }

  if (nFuncStatus == OK_CN00) {
    if (!bExists) {
      nFuncStatus = ERR_NOMEDIA_CN00;
      cn90AnswerIError(replyData, replyLen, nFuncStatus);
    } else {
      nFuncStatus = cn30_ReadMedia(vcontrol->dbname, replyData, replyLen, replyLenMax, &tMedia, bExists, bOldExists);
    }
  }

  if (nFuncStatus == OK_CN00) {
    
    cn30_SplitName(szFullName, szGroupName, szMediumName);

    nFuncStatus = cn30_FindMediumByName(replyData, replyLen, &tMedia, szFullName, &pMediaGroup, &pMedium);

    if (nFuncStatus == OK_CN00) {

      /* delete medium */
      if (pMedium->pPrev != NULL) {
        pMedium->pPrev->pNext = pMedium->pNext;
      } else {
        pMediaGroup->pFirst = pMedium->pNext;
      }
      if (pMedium->pNext != NULL) {
        pMedium->pNext->pPrev = pMedium->pPrev;
      } else {
        pMediaGroup->pLast = pMedium->pPrev;
      }
      cn00MyFree(pMedium);
    }
  }

  if (nFuncStatus == OK_CN00) {
    nFuncStatus = cn30_WriteMedia(vcontrol->dbname, replyData, replyLen, &tMedia, false);
  }

  cn30_FreeMedia(&tMedia);

  /**/

  if (nFuncStatus == OK_CN00) {
    sprintf(replyData, "%s%s", ANSWER_OK_CN00, LINE_SEPSTRING_CN00);
    pCurrPos = replyData + strlen(replyData);
  }

  *replyLen = (int)strlen(replyData);

  return nFuncStatus;
} /* end cn30BackupMediaDelete */

/* PUBLIC FUNCTION cn30BackupMediaDate
 * ------------------------------------------------------------------
 * purpose: get date of media file
 *
 */
tcn00_Error cn30BackupMediaDate (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax )
{
  tcn00_Error	        nFuncStatus = OK_CN00;

  char                * pCurrPos;

  bool                bExists;
  bool                bOldExists;
  char                szDate[64];

  /**/

  if (nFuncStatus == OK_CN00) {
    nFuncStatus = cn30_CheckMediaFile(vcontrol->dbname, replyData, replyLen, &bExists, &bOldExists, szDate);
  }

  if (nFuncStatus == OK_CN00) {
    if (!bExists) {
      nFuncStatus = ERR_NOMEDIA_CN00;
      cn90AnswerIError(replyData, replyLen, nFuncStatus);
    } else {
      sprintf(replyData, "%s%s", ANSWER_OK_CN00, LINE_SEPSTRING_CN00);
      pCurrPos = replyData + strlen(replyData);
       
      sprintf(pCurrPos, "%s%s", szDate, LINE_SEPSTRING_CN00);
      pCurrPos = replyData + strlen (replyData);
    }
  }

  /**/

  *replyLen = (int)strlen(replyData);

  return nFuncStatus;
} /* end cn30BackupMediaDate */


/* SPECIFICATION INTERN FUNCTIONS
 * ------------------------------------------------------------------
 */

// INTERN FUNCTION cn30NewMedium
// ------------------------------------------------------------------
// purpose: 
//

tcn00_Error cn30NewMedium(tcn003_Medium *& pMedium)
{
    tcn00_Error rc       =OK_CN00;
    bool        bMallocOK=true;

    cn00MyMalloc(sizeof(tcn003_Medium), &pMedium, &bMallocOK);

    if(0==pMedium || (!bMallocOK))
        rc=ERR_MEM_CN00;
    else
        cn30_InitMedium(pMedium);

    return rc;
} // end cn30NewMedium


// INTERN FUNCTION cn30NewMediaGroup
// ------------------------------------------------------------------
// purpose: 
//

tcn00_Error cn30NewMediaGroup(tcn003_MediaGroup *& pMediaGroup)
{
    tcn00_Error	rc       =OK_CN00;
    bool        bMallocOK=true;

    cn00MyMalloc(sizeof(tcn003_MediaGroup), &pMediaGroup, &bMallocOK);

    if(0==pMediaGroup || !bMallocOK)
        rc=ERR_MEM_CN00;
    else
        cn30_InitMediaGroup(pMediaGroup);

    return rc;
} // end cn30NewMediaGroup


// INTERN FUNCTION cn30InitMedia
// ------------------------------------------------------------------
// purpose: 
//

void cn30InitMedia(tcn003_Media * pMedia)
{
    cn30_InitMedia(pMedia);
} // end cn30InitMedia


// INTERN FUNCTION cn30AddMedium
// ------------------------------------------------------------------
// purpose: 
//

tcn00_Error cn30AddMedium(
      char           * replyData,
      int            * replyLen,
      tcn003_Media   * pMedia,
      tcn003_Medium  * pMedium)
{
    return cn30_AddMedium(replyData, replyLen, pMedia, pMedium);
} // end cn30AddMedium
 
/* INTERN FUNCTION cn30ReadMedia
 * ------------------------------------------------------------------
 * purpose: 
 *
 */
tcn00_Error cn30ReadMedia (
      const tsp00_DbNamec   szDbName,
      char                * replyData,
      int                 * replyLen,
      int                   replyLenMax,
      tcn003_Media        * pMedia )
{
  tcn00_Error	        nFuncStatus = OK_CN00;

  bool                bExists;
  bool                bOldExists;
  char                szDate[64];

  bool                bFound = false;

  /**/

  pMedia->pFirstGroup = NULL;
  pMedia->pLastGroup = NULL;
  
  if (nFuncStatus == OK_CN00) {
    nFuncStatus = cn30_CheckMediaFile(szDbName, replyData, replyLen, &bExists, &bOldExists, szDate);
  }

  if (nFuncStatus == OK_CN00) {
    if ( (!bExists) && (!bOldExists) ) {
      nFuncStatus = ERR_NOMEDIA_CN00;
      cn90AnswerIError(replyData, replyLen, nFuncStatus);
    } else {
      nFuncStatus = cn30_ReadMedia(szDbName, replyData, replyLen, replyLenMax, pMedia, bExists, bOldExists);
    }
  }

  *replyLen = (int)strlen(replyData);

  return nFuncStatus;
} /* end cn30ReadMedia */

/* INTERN FUNCTION cn30WriteMedia
 * ------------------------------------------------------------------
 * purpose: 
 *
 */
tcn00_Error cn30WriteMedia (
      const tsp00_DbNamec & szDbName,
      char                * replyData,
      int                 * replyLen,
      tcn003_Media        * pMedia )
{
  tcn00_Error	        nFuncStatus = OK_CN00;

  /**/

  nFuncStatus = cn30_WriteMedia(szDbName, replyData, replyLen, pMedia, false);

  *replyLen = (int)strlen(replyData);

  return nFuncStatus;
} /* end cn30WriteMedia */

/* INTERN FUNCTION cn30FreeMedia
 * ------------------------------------------------------------------
 * purpose: 
 *
 */
void cn30FreeMedia(
      tcn003_Media  * pMedia)
{
  cn30_FreeMedia(pMedia);
} /* end cn30FreeMedia */

/* INTERN FUNCTION cn30FindMediumByName
 * ------------------------------------------------------------------
 * purpose: search a medium in a medialist by name
 *
 */
tcn00_Error cn30FindMediumByName (
      char              * replyData,
      int               * replyLen,
      tcn003_Media      * pMedia,
      const char        * szMediumName,
      bool                CheckMediaGroups,
      tcn003_MediaGroup ** pMediaGroupFound,
      tcn003_Medium     ** pMediumFound,
      bool                 forbidMatchingGroups )
{
  tcn00_Error	        nFuncStatus = OK_CN00;

  tcn00_Error           FindMediaGroupResult=ERR_MENOTFOUND_CN00;
  tcn003_MediaGroup    *AMediaGroupWithSameName=0;

  if(CheckMediaGroups || forbidMatchingGroups)
      FindMediaGroupResult=cn30_FindMediaGroupByName(replyData, replyLen, pMedia, szMediumName, &AMediaGroupWithSameName);

  nFuncStatus = cn30_FindMediumByName(replyData, replyLen, pMedia, szMediumName, pMediaGroupFound, pMediumFound);

  if(nFuncStatus==OK_CN00 && CheckMediaGroups && FindMediaGroupResult==OK_CN00)
      nFuncStatus=cn90AnswerIError(replyData, replyLen, ERR_MEAMBIGUOUS_CN00); //we have found a group and a medium with the name szMediumName, but we should not!

  if(ERR_MENOTFOUND_CN00==nFuncStatus && forbidMatchingGroups && OK_CN00==FindMediaGroupResult)
      nFuncStatus=cn90AnswerIError(replyData, replyLen, ERR_BAWRONGMEDIUM_CN00);

  return nFuncStatus;
} /* cn30FindMediumByName */

/* INTERN FUNCTION cn30FindMediaGroupByName
 * ------------------------------------------------------------------
 * purpose: search a mediagroup by name
 *
 */
tcn00_Error cn30FindMediaGroupByName (
      char                * replyData,
      int                 * replyLen,
      tcn003_Media        * pMedia,
      const char          * szGroupName,
      tcn003_MediaGroup   ** pMediaGroupFound )
{
  tcn00_Error	        nFuncStatus = OK_CN00;

  /**/

  nFuncStatus = cn30_FindMediaGroupByName(replyData, replyLen, pMedia, szGroupName, pMediaGroupFound);

  return nFuncStatus;
} /* cn30FindMediaGroupByName */

/* INTERN FUNCTION cn30FindMediumInGroupByLocation
 * ------------------------------------------------------------------
 * purpose: find a medium in a goup by location
 *          
 *
 */
tcn00_Error cn30FindMediumInGroupByLocation (
      char                * replyData,
      int                 * replyLen,
      tcn003_Media        * pMedia,
      const char          * szGroupName,
      const char          * szLocation,
      tcn003_MediaGroup  ** pMediaGroupFound,
      tcn003_Medium      ** pMediumFound )
{
  tcn00_Error	        nFuncStatus = OK_CN00;

  // find media group
  nFuncStatus = cn30_FindMediaGroupByName(replyData, replyLen, pMedia, szGroupName, pMediaGroupFound);

  // find medium in media group
  if (nFuncStatus == OK_CN00) {
    if (*pMediaGroupFound != NULL) {
      *pMediumFound = (*pMediaGroupFound)->pFirst;

      nFuncStatus = ERR_MENOTFOUND_CN00;
      while ( (*pMediumFound != NULL               ) &&
              (nFuncStatus  == ERR_MENOTFOUND_CN00)    ) {
        if (strcmp(szLocation, (*pMediumFound)->szLocation) == 0) {
          nFuncStatus = OK_CN00;
        } else {
          *pMediumFound = (*pMediumFound)->pNext;
        } // end if
      } // end while
    } // end if
  } // end if

  return nFuncStatus;
} // end cn30FindMediumInGroupByLocation

tcn00_Error cn30SetBackupBlockCountInMediaFile(const char * backupBlockCount, DBMSrv_Reply & reply)
{
    tcn00_Error rc=OK_CN00;

    VControlDataT * vcontrol=cn00DBMServerData::vcontrol();

    if(0!=vcontrol)
    {
        bool exists=false;
        bool oldExists=false;
        char date[64];

        rc=cn30_CheckMediaFile(vcontrol->dbname, reply.giveData(), reply.giveLength(), &exists, &oldExists, date);

        if(OK_CN00==rc && exists)
        {
            tcn003_Media media;

            cn30_InitMedia(&media);

            rc=cn30_ReadMedia(vcontrol->dbname, reply.giveData(), reply.giveLength(), reply.giveMaximalLength(), &media, exists, oldExists);

            if(OK_CN00==rc)
            {
                //substitute all block sizes of 0
                bool changedSomething=false;
                tcn003_MediaGroup * mediaGroup=media.pFirstGroup;

                while(0!=mediaGroup)
                {
                    tcn003_Medium * medium=mediaGroup->pFirst;

                    while(0!=medium)
                    {
                        if(0==strcmp(medium->szBlocksize, "0"))
                        {
                            SAPDB_strcpy(medium->szBlocksize, backupBlockCount);
                            changedSomething=true;
                        }

                        medium=medium->pNext;
                    }

                    mediaGroup=mediaGroup->pNext;
                }

                if(changedSomething)
                    rc=cn30_WriteMedia(vcontrol->dbname, reply.giveData(), reply.giveLength(), &media, false);
            }

            cn30_FreeMedia(&media);
        }
    }

    return rc;
}
