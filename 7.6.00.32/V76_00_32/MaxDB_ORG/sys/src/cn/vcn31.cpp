/* @lastChanged: "1998-06-05  15:55"
 *
 * @filename:    vcn31.cpp
 * @purpose:     "backup management"
 * @release:     7.1.0.0
 * @see:         "-.-"
 *
 * ==================================================================
 *
 * responsible:   MartinR, TiloH
 *
 * created:       1998-03-18 by MartinR
 *
 * purpose:       basic module for backup
 *
 * ==================================================================



    ========== licence begin  GPL
    Copyright (c) 1998-2006 SAP AG

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

/* ------------------------------------------------------------------
 * includes
 * ------------------------------------------------------------------
 */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "heo02.h"
#include "heo06.h"
#include "heo11.h"
#include "geo002xp.h"
#include "gkb03.h"
#include "hsp77.h"

#include "hcn20.h"
#include "hcn30.h"
#include "hcn31.h"
#include "hcn35b.h"
#include "hcn35c.h"
#include "hcn50.h"
#include "hcn51.h"
#include "hcn53.h"
#include "hcn90.h"
#include "DBM/Srv/BackupHistory/DBMSrvBHist_History.hpp"
#include "DBM/Srv/Backup/Media/DBMSrvBMedia_MediaGroup.hpp"
#include "DBM/Srv/DBMSrv_Reply.hpp"
#include "DBM/Srv/KernelAccess/DBMSrvKnl_State.hpp"
#include "DBM/Srv/KernelAccess/DBMSrvKnl_GlobalSessions.hpp"
#include "DBM/Srv/Message/DBMSrvMsg_RTEError.hpp"

#define SYS_SYSTEM_ERR        -1

/* ------------------------------------------------------------------
 * PRIVATE PROTOTYPES
 * ------------------------------------------------------------------
 */
void cn31_InitResult (
      tcn003_BackupResult  * pResult );

static void cn31_DBAActionStart(
      const tsp00_DbNamec       & szDbName,
      tcn003_BackupSession      * pBackupSession,
      const bool                  bRecovery);

static void cn31_DBAActionComplete(
      const tcn00_Error           nFuncReturn);

/* ------------------------------------------------------------------
 * PRIVATE FUNCTION cn31_CheckBackupType
 * ------------------------------------------------------------------
 * purpose: check the backuptype parameter
 *
 */
static tcn00_Error cn31_CheckBackupType (
       char          * replyData,
       int           * replyLen,
       const char    * szBackupType )
{
  tcn00_Error         nFuncStatus  = OK_CN00;

  /**/

  if (nFuncStatus == OK_CN00) {
    if (szBackupType[0] != '\0') {
      if ( (cn90Stricmp(szBackupType, BA_KEY_DATA_CN003) != 0)  &&
           (cn90Stricmp(szBackupType, BA_KEY_PAGES_CN003) != 0) &&
           (cn90Stricmp(szBackupType, BA_KEY_LOG_CN003) != 0)      ) {
        nFuncStatus = ERR_BANOVALIDTYPE_CN00;
        cn90AnswerIError(replyData, replyLen, nFuncStatus);
      }
    }
  }

  return nFuncStatus;
} /* end cn31_CheckBackupType */

/* PRIVATE FUNCTION cn31_InitBackupSession
 * ------------------------------------------------------------------
 * purpose: init backup session
 *
 */
void cn31_InitBackupSession (
      tcn003_BackupSession  * pBackupSession )
{

  if (pBackupSession != NULL) {
    pBackupSession->nSessionType = 0;
    pBackupSession->szMediaName[0] = '\0';
    pBackupSession->szBackupType[0] = '\0';

    pBackupSession->szFileNumber[0] = '\0';
    pBackupSession->szUntilDate[0] = '\0';
    pBackupSession->szUntilTime[0] = '\0';

    pBackupSession->szCommand[0] = '\0';

    pBackupSession->tMedia.pFirstGroup = NULL;
    pBackupSession->tMedia.pLastGroup = NULL;

    pBackupSession->bCheckOnly = false;
    pBackupSession->bCancelActive = false;
    pBackupSession->bAutoignore = false;

    cn31_InitResult(&(pBackupSession->tBackupResult));

    pBackupSession->pBackupTool = NULL;
    pBackupSession->StageController=0;
  }
} /* end cn31_InitBackupSession */

/* PRIVATE FUNCTION cn31_AllocBackupSession
 * ------------------------------------------------------------------
 * purpose: allocate new memory for backup session
 *
 */
static tcn00_Error cn31_AllocBackupSession (
      VControlDataT         * vcontrol,
      char                  * replyData,
      int                   * replyLen,
      tcn003_BackupSession  ** pBackupSession )
{
  tcn00_Error         nFuncStatus  = OK_CN00;

  bool                bMallocOK = true;

  /**/

  if (nFuncStatus == OK_CN00) {

    if (*pBackupSession == NULL) {
      cn00MyMalloc (sizeof (tcn003_BackupSession), pBackupSession, &bMallocOK);
    }

    if ((*pBackupSession == NULL) || (!bMallocOK)) {
      nFuncStatus = ERR_MEM_CN00;
      cn90AnswerIError(replyData, replyLen, nFuncStatus);
    } else {
      vcontrol->pBackupSession = (void *) *pBackupSession;
      cn31_InitBackupSession(*pBackupSession);
    }
  }

  return nFuncStatus;
} /* end cn31_AllocBackupSession */

/* PRIVATE FUNCTION cn31_AllocBackupStageSession
 * ------------------------------------------------------------------
 * purpose: allocate new memory for backup stage session
 *
 */
static tcn00_Error cn31_AllocBackupStageSession(VControlDataT         * vcontrol,
                                                char                  * replyData,
                                                int                   * replyLen,
                                                tcn003_BackupSession ** pBackupSession)
{
    tcn00_Error rc=OK_CN00;
    bool        bMallocOK = true;

    if(0==(*pBackupSession))
        cn00MyMalloc(sizeof(tcn003_BackupSession), pBackupSession, &bMallocOK);

    if((0==(*pBackupSession)) || (!bMallocOK))
        rc=cn90AnswerIError(replyData, replyLen, ERR_MEM_CN00);
    else
    {
        vcontrol->pBackupStageSession=(void *) *pBackupSession;

        cn31_InitBackupSession(*pBackupSession);
    }

    return rc;
} /* end cn31_AllocBackupStageSession */

/* ------------------------------------------------------------------
 * PRIVATE FUNCTION cn31_CheckBackupTypeMedium
 * ------------------------------------------------------------------
 * purpose: check the savetype in medium definition with backuptype
 *
 */
static tcn00_Error cn31_CheckBackupTypeMedium (
      char           * replyData,
      int            * replyLen,
      char           * szBackupType,
      tcn003_Medium  * pMedium)
{
  tcn00_Error          nFuncStatus  = OK_CN00;

  /**/

  if (pMedium != NULL) {
    if (cn90Stricmp(pMedium->szSaveType, ME_SAVETYPE_UNK_CN003) != 0) {
      if (szBackupType[0] != '\0') {

/* PTS 1104923
        if (cn90Stricmp(pMedium->szSaveType, ME_SAVETYPE_AUT_CN003) == 0) {
          if (cn90Stricmp(szBackupType, ME_SAVETYPE_LOG_CN003) != 0) {
            nFuncStatus = ERR_BASAVETYPECON_CN00;
            cn90AnswerIError(replyData, replyLen, nFuncStatus);
          }
        } else {
          if (cn90Stricmp(pMedium->szSaveType, szBackupType) != 0) {
            nFuncStatus = ERR_BASAVETYPECON_CN00;
            cn90AnswerIError(replyData, replyLen, nFuncStatus);
          }
        }
*/
      }
      else {
        if (cn90Stricmp(pMedium->szSaveType, ME_SAVETYPE_AUT_CN003) == 0) {
          SAPDB_strcpy(szBackupType, ME_SAVETYPE_LOG_CN003);
        } else {
          SAPDB_strcpy(szBackupType, pMedium->szSaveType);
        }
      }
    }
    else {
      if (szBackupType[0] == '\0') {
        nFuncStatus = ERR_BANOSAVETYPE_CN00;
        cn90AnswerIError(replyData, replyLen, nFuncStatus);
      }
    }
  }

  return nFuncStatus;
} /* end cn31_CheckBackupTypeMedium */

/* ------------------------------------------------------------------
 * PRIVATE FUNCTION cn31_CheckBackupTypeMediaGroup
 * ------------------------------------------------------------------
 * purpose: check each medium in group for backuptype
 *
 *
 */
static tcn00_Error cn31_CheckBackupTypeMediaGroup (
      char                * replyData,
      int                 * replyLen,
      char                * szBackupType,
      tcn003_MediaGroup   * pMediaGroup)
{
  tcn00_Error       nFuncStatus  = OK_CN00;
  tcn003_Medium   * pMedium = NULL;

  /**/

  if (pMediaGroup != NULL) {

    pMedium = pMediaGroup->pFirst;

    while ( (pMedium != NULL)        &&
            (nFuncStatus == OK_CN00)    ) {

      nFuncStatus = cn31_CheckBackupTypeMedium(replyData, replyLen, szBackupType, pMedium);
      pMedium = pMedium->pNext;
    }
  }

  return nFuncStatus;
} /* end cn31_CheckBackupTypeMediaGroup */


/* ------------------------------------------------------------------
 * PRIVATE FUNCTION cn31_CheckOverwriteMedium
 * ------------------------------------------------------------------
 * purpose: if medium exists and overwrite is false then
 *          return an error
 *
 */
static tcn00_Error cn31_CheckOverwriteMedium (
      char           * replyData,
      int            * replyLen,
      tcn003_Medium  * pMedium)
{
  tcn00_Error          nFuncStatus  = OK_CN00;
  tsp05_RteFileError   rteFileError;
  tsp05_RteFileInfo    rteFileInfo;
  tcn002_XpValueString szRunDir;
  tsp01_RteError       rteError;

  /**/

  // check pointer
  if (pMedium != NULL) {
    // check medium properties
    if ( (cn90Stricmp(pMedium->szType, ME_TYPE_FILE_CN003) == 0)    &&
         (cn90Stricmp(pMedium->szOverwrite, ME_PROP_NO_CN003) == 0)    ) {
      // get rundir
      DBMSrvMsg_Error oParamGetMsg;
      if (cn20XParamGetValue(cn00DBMServerData::vcontrol()->dbname, PAN_RUNDIR, szRunDir, oParamGetMsg)) {
        // change to rundir
        if (sqlchdirc(szRunDir.asCharp(), &rteError)) {
          // check file
          sqlfinfoc (pMedium->szLocation, &rteFileInfo, &rteFileError);
          if (rteFileError.sp5fe_result == vf_ok) {
            if (rteFileInfo.sp5fi_exists) {
              nFuncStatus = ERR_BAOVERWRITE_CN00;
              cn90AnswerIError(replyData, replyLen, nFuncStatus);
            } // end if
          }  // end if
        // change back to dbroot
        sqlchdirc(cn00DBMServerData::vcontrol()->dbroot.asCharp(), &rteError);
        } // end if
      } // end if
    } // end if
  } // end if

  return nFuncStatus;
} /* end cn31_CheckOverwriteMedium */

/* ------------------------------------------------------------------
 * PRIVATE FUNCTION cn31_CheckOverwriteMediaGroup
 * ------------------------------------------------------------------
 * purpose: check each medium in group for overwrite
 *
 *
 */
static tcn00_Error cn31_CheckOverwriteMediaGroup (
      char                * replyData,
      int                 * replyLen,
      tcn003_MediaGroup   * pMediaGroup)
{
  tcn00_Error       nFuncStatus  = OK_CN00;
  tcn003_Medium   * pMedium = NULL;

  /**/

  if (pMediaGroup != NULL) {

    pMedium = pMediaGroup->pFirst;

    while ( (pMedium != NULL)        &&
            (nFuncStatus == OK_CN00)    ) {

      nFuncStatus = cn31_CheckOverwriteMedium(replyData, replyLen, pMedium);
      pMedium = pMedium->pNext;
    }
  }

  return nFuncStatus;
} /* end cn31_CheckOverwriteMediaGroup */

/* PRIVATE FUNCTION cn31_FileSpec
 * ------------------------------------------------------------------
 * purpose: build file spec for backup command
 *
 */
static tcn00_Error cn31_FileSpec (
      const char     * szBackupType,
      tcn003_Medium  * pMedium,
      char           * szFileSpec,
      bool             bWithCount,
      bool             bCheck)
{
  tcn00_Error       nFuncStatus  = OK_CN00;
  tcn002_XpValueString XpValueC;

  /**/

  XpValueC.Init();

  if (bCheck &&
      pMedium->szLocation[0] != '/'  &&
      pMedium->szLocation[0] != '\\' &&
#ifdef _WIN32
      pMedium->szLocation[1] != ':'  &&
#endif
      cn90Stricmp(pMedium->szType, ME_TYPE_FILE_CN003) == 0) {
    // read run dir
    DBMSrvMsg_Error oParamGetMsg;
    cn20XParamGetValue (cn00DBMServerData::vcontrol()->dbname, PAN_RUNDIR, XpValueC, oParamGetMsg);
  } // end if

  SAPDB_strcpy(szFileSpec, BA_KEY_QUOTE_CN003);                 /* '                              */
  if (strlen(XpValueC) > 0) {
    strcat(szFileSpec, XpValueC);                         /* rundirectory                   */
#ifdef _WIN32
    strcat(szFileSpec, "\\");
#else
    strcat(szFileSpec, "/");
#endif
  } // end if
  strcat(szFileSpec, pMedium->szLocation);                /* <host_file_name>               */
  strcat(szFileSpec, BA_KEY_QUOTE_CN003);                 /* '                              */
  strcat(szFileSpec, BA_KEY_BLANK_CN003);                 /*                                */

  if ( (cn90Stricmp(szBackupType, BA_KEY_LOG_CN003) == 0)      &&
       (cn90Stricmp(pMedium->szType, ME_TYPE_TAPE_CN003) == 0)    ) {

    strcat(szFileSpec, ME_TYPE_NORE_CN003);               /* <file_type                     */

  } else {

    strcat(szFileSpec, pMedium->szType);                   /* <file_type                     */

  }

  if (bWithCount) {
    if (cn90Stricmp(pMedium->szSize,"0") != 0) {
      strcat(szFileSpec, BA_KEY_BLANK_CN003);             /*                                */
      strcat(szFileSpec, BE_KEY_COUNT_CN003);             /* COUNT                          */
      strcat(szFileSpec, BA_KEY_BLANK_CN003);             /*                                */
      strcat(szFileSpec, pMedium->szSize);                /* <count>                        */
    }
  }

  return nFuncStatus;
} /* end cn31_FileSpec */

/* PRIVATE FUNCTION cn31_FileSpecList
 * ------------------------------------------------------------------
 * purpose: build file spec list for backup command
 *
 */
static tcn00_Error cn31_FileSpecList (
      const char          * szBackupType,
      tcn003_MediaGroup   * pMediaGroup,
      char                * szFileSpecList,
      bool                  bWithCount,
      bool                  bCheck)
{
  tcn00_Error         nFuncStatus  = OK_CN00;

  tcn003_Medium       * pMedium = NULL;

  bool             bFirstFileSpec = true;

  char                szFileSpec[BA_FILESPEC_LENGTH_MXCN003];


  /**/

  szFileSpecList[0] = '\0';

  if (pMediaGroup != NULL) {

    pMedium = pMediaGroup->pFirst;

    while ( (pMedium != NULL)        &&
            (nFuncStatus == OK_CN00)    ) {

      nFuncStatus = cn31_FileSpec(szBackupType, pMedium, szFileSpec, bWithCount, bCheck);

      if (nFuncStatus == OK_CN00) {
        if (bFirstFileSpec) {
          strcat(szFileSpecList, BA_KEY_BLANK_CN003);          /*                                */
          bFirstFileSpec = false;
        } else {
          strcat(szFileSpecList, BA_KEY_COMMA_CN003);          /* ,                              */
        }
        strcat(szFileSpecList, szFileSpec);                    /* <filespec>                     */
      }

      pMedium = pMedium->pNext;
    }
  }

  return nFuncStatus;
} /* end cn31_FileSpecList */

/* PRIVATE FUNCTION cn31_FileSpecWithNumber
 * ------------------------------------------------------------------
 * purpose: build file spec for restore command with numbered files
 *
 */
static tcn00_Error cn31_FileSpecWithNumber (
      tcn003_Medium  * pMedium,
      char           * szFileNumber,
      char           * szFileSpec,
      bool             bWithCount,
      bool             bCheck)
{
  tcn00_Error       nFuncStatus  = OK_CN00;
  tcn002_XpValueString        XpValueC;

  /**/

  XpValueC.Init();

  if (bCheck &&
      pMedium->szLocation[0] != '/'  &&
      pMedium->szLocation[0] != '\\' &&
#ifdef _WIN32
      pMedium->szLocation[1] != ':'  &&
#endif
      cn90Stricmp(pMedium->szType, ME_TYPE_FILE_CN003) == 0) {
    // read run dir
    DBMSrvMsg_Error oParamGetMsg;
    cn20XParamGetValue (cn00DBMServerData::vcontrol()->dbname, PAN_RUNDIR, XpValueC, oParamGetMsg);
  } // end if

  if (cn90Stricmp(pMedium->szType, ME_TYPE_FILE_CN003) == 0) {

    SAPDB_strcpy(szFileSpec, BA_KEY_QUOTE_CN003);                 /* '                              */
  if (strlen(XpValueC) > 0) {
    strcat(szFileSpec, XpValueC);                           /* rundirectory                   */
#ifdef _WIN32
    strcat(szFileSpec, "\\");
#else
    strcat(szFileSpec, "/");
#endif
  } // end if
    strcat(szFileSpec, pMedium->szLocation);                /* <host_file_name>               */
    strcat(szFileSpec, ".");                                /* .                              */
    strcat(szFileSpec, szFileNumber);                        /* <filenumberextension>          */
    strcat(szFileSpec, BA_KEY_QUOTE_CN003);                 /* '                              */

    sprintf(szFileNumber, "%03d", atoi(szFileNumber) + 1);

  } else {

    SAPDB_strcpy(szFileSpec, BA_KEY_QUOTE_CN003);                 /* '                              */
    strcat(szFileSpec, pMedium->szLocation);                /* <host_file_name>               */
    strcat(szFileSpec, BA_KEY_QUOTE_CN003);                 /* '                              */

  }

  strcat(szFileSpec, BA_KEY_BLANK_CN003);                   /*                                */
  strcat(szFileSpec, pMedium->szType);                      /* <file_type                     */

  if (bWithCount) {
    if (cn90Stricmp(pMedium->szSize,"0") != 0) {
      strcat(szFileSpec, BA_KEY_BLANK_CN003);               /*                                */
      strcat(szFileSpec, BE_KEY_COUNT_CN003);               /* COUNT                          */
      strcat(szFileSpec, BA_KEY_BLANK_CN003);               /*                                */
      strcat(szFileSpec, pMedium->szSize);                  /* <count>                        */
    }
  }

  return nFuncStatus;
} /* end cn31_FileSpecWithNumber */

/* PRIVATE FUNCTION cn31_BuildSaveCommand
 * ------------------------------------------------------------------
 * purpose: build the save command
 *
 */
static tcn00_Error cn31_BuildSaveCommand (
      char           * szBackupType,
      tcn003_Medium  * pMedium,
      bool             bRecovery,
      char           * szCommand )
{
  tcn00_Error         nFuncStatus  = OK_CN00;

  char                szFileSpec[BA_FILESPEC_LENGTH_MXCN003];

  /**/

  szCommand[0] = '\0';

  if (pMedium != NULL) {
    SAPDB_strcpy(szCommand, BA_KEY_SAVE_CN003);                 /* SAVE                           */
    strcat(szCommand, BA_KEY_BLANK_CN003);                /*                                */
    strcat(szCommand, szBackupType);                      /* <save_type>                    */
    strcat(szCommand, BA_KEY_BLANK_CN003);                /*                                */
    strcat(szCommand, BA_KEY_QUICK_CN003);                /* QUICK                          */
    strcat(szCommand, BA_KEY_BLANK_CN003);                /*                                */
    strcat(szCommand, BA_KEY_TO_CN003);                   /* TO                             */

    nFuncStatus = cn31_FileSpec(szBackupType, pMedium, szFileSpec, true, false);

    if (nFuncStatus == OK_CN00) {
      strcat(szCommand, BA_KEY_BLANK_CN003);              /*                                */
      strcat(szCommand, szFileSpec);                      /* <filespec>                     */

      if ( (cn90Stricmp(pMedium->szOverwrite, ME_PROP_VERSION_CN003) == 0) ||       //(Overwrite=="Version") or
           (cn90Stricmp(szBackupType, BA_KEY_LOG_CN003) == 0 &&                     //(BackupType=="Log" and
            0==cn90Stricmp(pMedium->szType, ME_TYPE_FILE_CN003))              ) {   // MediumType=="FILE"   )
        strcat(szCommand, BA_KEY_BLANK_CN003);            /*                                */
        strcat(szCommand, BA_KEY_FVERSION_CN003);         /* FVERSION                       */
      }

      strcat(szCommand, BA_KEY_BLANK_CN003);            /*                                */
      strcat(szCommand, BA_KEY_BLOCKSIZE_CN003);        /* BLOCKSIZE                      */
      strcat(szCommand, BA_KEY_BLANK_CN003);            /*                                */
      strcat(szCommand, ('\0'==pMedium->szBlocksize[0])?ME_DEFAULT_BLOCKSIZE_CN003:pMedium->szBlocksize);  /* "8" or <blocksize>                 */

      if (cn90Stricmp(pMedium->szAutoLoader, ME_PROP_YES_CN003) == 0) {
        strcat(szCommand, BA_KEY_BLANK_CN003);            /*                                */
        strcat(szCommand, BA_KEY_CASCADE_CN003);          /* CASCADE                        */
      }

      if ( (bRecovery)                                        &&
           (cn90Stricmp(szBackupType, BA_KEY_LOG_CN003) != 0)    ) {
        strcat(szCommand, BA_KEY_BLANK_CN003);            /*                                */
        strcat(szCommand, BA_KEY_NOCHECKPOINT_CN003);     /* NO CHECKPOINT                  */
      }

      strcat(szCommand, BA_KEY_BLANK_CN003);              /*                                */
      strcat(szCommand, BA_KEY_MEDIANAME_CN003);          /* MEDIANAME                      */
      strcat(szCommand, BA_KEY_BLANK_CN003);              /*                                */
      strcat(szCommand, BA_KEY_QUOTE_CN003);              /* '                              */
      strcat(szCommand, pMedium->szName);                 /* <medianame>                    */
      strcat(szCommand, BA_KEY_QUOTE_CN003);              /* '                              */
    }
  }

  return nFuncStatus;
} /* end cn31_BuildSaveCommand */

/* PRIVATE FUNCTION cn31_BuildParallelSaveCommand
 * ------------------------------------------------------------------
 * purpose: build the save command for parallel backup
 *
 */
static tcn00_Error cn31_BuildParallelSaveCommand (
      char                * szBackupType,
      tcn003_MediaGroup   * pMediaGroup,
      bool                  bRecovery,
      char                * szCommand )
{
    tcn00_Error nFuncStatus  = OK_CN00;
    bool        bFirstFileSpec = true;
    char        szFileSpecList[BA_FILESPEC_LENGTH_MXCN003];

    szCommand[0] = '\0';

    if (pMediaGroup != NULL)
    {
        int             numberOfMediaInGroup = 0;
        tcn003_Medium * medium = pMediaGroup->pFirst;

        while(0!=medium)
        {
            numberOfMediaInGroup++;
            medium=medium->pNext;
        }

        if(1==numberOfMediaInGroup)
        {
            tcn003_Medium fakeMediumForGroup=*(pMediaGroup->pFirst);

            SAPDB_strcpy(fakeMediumForGroup.szName, pMediaGroup->szName); //change the medium name to the group name (to have the same medium name as during backup)

            cn31_BuildSaveCommand (
                szBackupType,
                &fakeMediumForGroup,
                bRecovery,
                szCommand );
        }
        else
        {
            SAPDB_strcpy(szCommand, BA_KEY_SAVE_CN003);                 /* SAVE                           */
            strcat(szCommand, BA_KEY_BLANK_CN003);                /*                                */
            strcat(szCommand, szBackupType);                      /* <save_type>                    */
            strcat(szCommand, BA_KEY_BLANK_CN003);                /*                                */
            strcat(szCommand, BA_KEY_QUICK_CN003);                /* QUICK                          */
            strcat(szCommand, BA_KEY_BLANK_CN003);                /*                                */
            strcat(szCommand, BA_KEY_TO_CN003);                   /* TO                             */

            nFuncStatus = cn31_FileSpecList(szBackupType, pMediaGroup, szFileSpecList, true, false);

            if (nFuncStatus == OK_CN00)
            {
                strcat(szCommand, szFileSpecList);                  /* <filespeclist>                 */

                strcat(szCommand, BA_KEY_BLANK_CN003);                  /*                                */
                strcat(szCommand, BA_KEY_BLOCKSIZE_CN003);              /* BLOCKSIZE                      */
                strcat(szCommand, BA_KEY_BLANK_CN003);                  /*                                */
                if(0==pMediaGroup->pFirst || '\0'==pMediaGroup->pFirst->szBlocksize[0])
                    strcat(szCommand, ME_DEFAULT_BLOCKSIZE_CN003);      /* "8"                            */
                else
                    strcat(szCommand, pMediaGroup->pFirst->szBlocksize);/* <blocksize>                    */

                if (bRecovery)
                {
                    strcat(szCommand, BA_KEY_BLANK_CN003);            /*                                */
                    strcat(szCommand, BA_KEY_NOCHECKPOINT_CN003);     /* NO CHECKPOINT                  */
                }

                strcat(szCommand, BA_KEY_BLANK_CN003);              /*                                */
                strcat(szCommand, BA_KEY_MEDIANAME_CN003);          /* MEDIANAME                      */
                strcat(szCommand, BA_KEY_BLANK_CN003);              /*                                */
                strcat(szCommand, BA_KEY_QUOTE_CN003);              /* '                              */
                strcat(szCommand, pMediaGroup->szName);             /* <medianame>                    */
                strcat(szCommand, BA_KEY_QUOTE_CN003);              /* '                              */
            }
        }
    }

    return nFuncStatus;
} /* end cn31_BuildParallelSaveCommand */

/* PRIVATE FUNCTION cn31_BuildSaveReplaceCommand
 * ------------------------------------------------------------------
 * purpose: build the save replace command
 *
 */
static tcn00_Error cn31_BuildSaveReplaceCommand (
      char           * szBackupType,
      tcn003_Medium  * pMedium,
      char           * szCommand )
{
  tcn00_Error         nFuncStatus  = OK_CN00;

  char                szFileSpec[BA_FILESPEC_LENGTH_MXCN003];

  /**/

  szCommand[0] = '\0';

  if (pMedium != NULL) {
    SAPDB_strcpy(szCommand, BA_KEY_SAVE_CN003);                 /* SAVE                           */
    strcat(szCommand, BA_KEY_BLANK_CN003);                /*                                */
    strcat(szCommand, szBackupType);                      /* <save_type>                    */
    strcat(szCommand, BA_KEY_BLANK_CN003);                /*                                */
    strcat(szCommand, BA_KEY_REPLACE_CN003);              /* REPLACE                        */

    nFuncStatus = cn31_FileSpec(szBackupType, pMedium, szFileSpec, true, false);

    if (nFuncStatus == OK_CN00) {
      strcat(szCommand, BA_KEY_BLANK_CN003);              /*                                */
      strcat(szCommand, szFileSpec);                      /* <filespec>                     */
    }
  }

  return nFuncStatus;
} /* end cn31_BuildSaveReplaceCommand */

/* PRIVATE FUNCTION cn31_BuildSaveIgnoreCommand
 * ------------------------------------------------------------------
 * purpose: build the save ignore command
 *
 */
tcn00_Error cn31_BuildSaveIgnoreCommand (
      const char     * szBackupType,
      char           * szCommand )
{
  tcn00_Error         nFuncStatus  = OK_CN00;

  /**/

  szCommand[0] = '\0';

  SAPDB_strcpy(szCommand, BA_KEY_SAVE_CN003);                 /* SAVE                           */
  strcat(szCommand, BA_KEY_BLANK_CN003);                /*                                */
  strcat(szCommand, szBackupType);                      /* <save_type>                    */
  strcat(szCommand, BA_KEY_BLANK_CN003);                /*                                */
  strcat(szCommand, BA_KEY_IGNORE_CN003);               /* IGNORE                         */

  return nFuncStatus;
} /* end cn31_BuildSaveIgnoreCommand */

/* PRIVATE FUNCTION cn31_BuildSaveCancelCommand
 * ------------------------------------------------------------------
 * purpose: build the save cancel command
 *
 */
static tcn00_Error cn31_BuildSaveCancelCommand (
      char           * szBackupType,
      char           * szCommand )
{
  tcn00_Error         nFuncStatus  = OK_CN00;

  /**/

  szCommand[0] = '\0';

  SAPDB_strcpy(szCommand, BA_KEY_SAVE_CN003);                 /* SAVE                           */
  strcat(szCommand, BA_KEY_BLANK_CN003);                /*                                */
  strcat(szCommand, szBackupType);                      /* <save_type>                    */
  strcat(szCommand, BA_KEY_BLANK_CN003);                /*                                */
  strcat(szCommand, BA_KEY_CANCEL_CN003);               /* CANCEL                         */

  return nFuncStatus;
} /* end cn31_BuildSaveCancelCommand */

/* PRIVATE FUNCTION cn31_AppendLabelToCheck
 * ------------------------------------------------------------------
 * purpose: append LABEL '<LabelToCheck>' to the restore command, if LabelToCheck is a non empty string
 */
static void cn31_AppendLabelToCheck(char       * szCommand,          //the command string
                                    const char * LabelToCheck,       //the label string
                                    size_t       LabelToCheckLength) //length of the label string
{
    if( 0!=LabelToCheck && 0<LabelToCheckLength)
    {
        strcat(szCommand, BA_KEY_BLANK_CN003);                                      /*                                */
        strcat(szCommand, BA_KEY_LABEL_CN003);                                      /* LABEL                          */
        strcat(szCommand, BA_KEY_BLANK_CN003);                                      /*                                */
        strcat(szCommand, BA_KEY_QUOTE_CN003);                                      /* '                              */

        char * currentPos=szCommand+strlen(szCommand);
        
        if(LabelToCheckLength>BA_MAXLABELLENGTH_MXCN003)  //prevent a buffer overflow
            LabelToCheckLength=BA_MAXLABELLENGTH_MXCN003;

        strncat(szCommand, LabelToCheck, LabelToCheckLength);                       /* <label_to_check>               */

        SAPDB_strcpy(currentPos+LabelToCheckLength, BA_KEY_QUOTE_CN003);                  /* '                              */
    }
}

/* PRIVATE FUNCTION cn31_BuildRestoreCommand
 * ------------------------------------------------------------------
 * purpose: build the restore command
 *
 */
static tcn00_Error cn31_BuildRestoreCommand (
      char           * szBackupType,
      bool             bCheck,
      tcn003_Medium  * pMedium,
      char           * szFileNumber,
      char           * szUntilDate,
      char           * szUntilTime,
      char           * szCommand,
      bool             bCreate,
      const char     * LabelToCheck,
      size_t           LabelToCheckLength)
{
  tcn00_Error         nFuncStatus  = OK_CN00;

  char                szFileSpec[BA_FILESPEC_LENGTH_MXCN003];
  /**/

  szCommand[0] = '\0';

  if (pMedium != NULL) {
    if (bCreate) {
      SAPDB_strcpy(szCommand, BA_KEY_CREATE_CN003);               /* CREATE                         */
      strcat(szCommand, BA_KEY_BLANK_CN003);                /*                                */
      strcat(szCommand, BA_KEY_INSTANCE_CN003);             /* INSTANCE                       */
      strcat(szCommand, BA_KEY_BLANK_CN003);                /*                                */
      strcat(szCommand, BA_KEY_WITH_CN003);                 /* WITH                           */
      strcat(szCommand, BA_KEY_BLANK_CN003);                /*                                */
      strcat(szCommand, BA_KEY_RESTORE_CN003);              /* RESTORE                        */
    } else {
      SAPDB_strcpy(szCommand, BA_KEY_RESTORE_CN003);              /* RESTORE                        */
    } // end if
    strcat(szCommand, BA_KEY_BLANK_CN003);                /*                                */
    strcat(szCommand, szBackupType);                      /* <restore_type>                 */
    strcat(szCommand, BA_KEY_BLANK_CN003);                /*                                */
//    strcat(szCommand, BA_KEY_QUICK_CN003);                /* QUICK                          */
//    strcat(szCommand, BA_KEY_BLANK_CN003);                /*                                */
    strcat(szCommand, BA_KEY_FROM_CN003);                 /* FROM                           */

    if (szFileNumber[0] != '\0') {
      nFuncStatus = cn31_FileSpecWithNumber(pMedium, szFileNumber, szFileSpec, false, bCheck);
    } else {
      nFuncStatus = cn31_FileSpec(szBackupType, pMedium, szFileSpec, false, bCheck);
    }

    if (nFuncStatus == OK_CN00) {
      strcat(szCommand, BA_KEY_BLANK_CN003);              /*                                */
      strcat(szCommand, szFileSpec);                      /* <filespec>                     */
      strcat(szCommand, BA_KEY_BLANK_CN003);              /*                                */
      if ((cn90Stricmp(szBackupType, BA_KEY_LOG_CN003) == 0) &&
          (szUntilDate[0] != '\0')                           &&
          (szUntilTime[0] != '\0')                              ) {
        strcat(szCommand, BA_KEY_UNTIL_CN003);            /* UNTIL                          */
        strcat(szCommand, BA_KEY_BLANK_CN003);            /*                                */
        strcat(szCommand, BA_KEY_QUOTE_CN003);            /* '                              */
        strcat(szCommand, szUntilDate);                   /* <date>                         */
        strcat(szCommand, BA_KEY_QUOTE_CN003);            /* '                              */
        strcat(szCommand, BA_KEY_BLANK_CN003);            /*                                */
        strcat(szCommand, BA_KEY_QUOTE_CN003);            /* '                              */
        strcat(szCommand, szUntilTime);                   /* <time>                         */
        strcat(szCommand, BA_KEY_QUOTE_CN003);            /* '                              */
        strcat(szCommand, BA_KEY_BLANK_CN003);            /*                                */
      }

      strcat(szCommand, BA_KEY_BLANK_CN003);            /*                                */
      strcat(szCommand, BA_KEY_BLOCKSIZE_CN003);        /* BLOCKSIZE                      */
      strcat(szCommand, BA_KEY_BLANK_CN003);            /*                                */
      strcat(szCommand, ('\0'==pMedium->szBlocksize[0])?ME_DEFAULT_BLOCKSIZE_CN003:pMedium->szBlocksize);          /* "8" or <blocksize>              */
      strcat(szCommand, BA_KEY_BLANK_CN003);            /*                                */

      if (cn90Stricmp(pMedium->szAutoLoader, ME_PROP_YES_CN003) == 0) {
        strcat(szCommand, BA_KEY_BLANK_CN003);            /*                                */
        strcat(szCommand, BA_KEY_CASCADE_CN003);          /* CASCADE                        */
        strcat(szCommand, BA_KEY_BLANK_CN003);            /*                                */
      }

      strcat(szCommand, BA_KEY_MEDIANAME_CN003);          /* MEDIANAME                      */
      strcat(szCommand, BA_KEY_BLANK_CN003);              /*                                */
      strcat(szCommand, BA_KEY_QUOTE_CN003);              /* '                              */
      strcat(szCommand, pMedium->szName);                 /* <medianame>                    */
      strcat(szCommand, BA_KEY_QUOTE_CN003);              /* '                              */

      cn31_AppendLabelToCheck(szCommand, LabelToCheck, LabelToCheckLength);  /* LABEL '<LabelToCheck>' , if LabelToCheck is a non empty string */
    }
  }

  return nFuncStatus;
} /* end cn31_BuildRestoreCommand */

/* PRIVATE FUNCTION cn31_BuildParallelRestoreCommand
 * ------------------------------------------------------------------
 * purpose: build the restore command for parallel backup
 *
 */
static tcn00_Error cn31_BuildParallelRestoreCommand (
      char                * szBackupType,
      bool                  bCheck,
      tcn003_MediaGroup   * pMediaGroup,
      char                * szFileNumber,
      char                * szUntilDate,
      char                * szUntilTime,
      char                * szCommand,
      bool                  bCreate,
      const char          * LabelToCheck,
      size_t                LabelToCheckLength)
{
    tcn00_Error nFuncStatus  = OK_CN00;
    bool        bFirstFileSpec = true;
    char        szFileSpecList[BA_FILESPEC_LENGTH_MXCN003];

    szCommand[0] = '\0';

    if (pMediaGroup != NULL)
    {
        int             numberOfMediaInGroup = 0;
        tcn003_Medium * medium = pMediaGroup->pFirst;

        while(0!=medium)
        {
            numberOfMediaInGroup++;
            medium=medium->pNext;
        }

        if(1==numberOfMediaInGroup)
        {
            tcn003_Medium fakeMediumForGroup=*(pMediaGroup->pFirst);

            SAPDB_strcpy(fakeMediumForGroup.szName, pMediaGroup->szName); //change the medium name to the group name (to have the same medium name as during backup)

            cn31_BuildRestoreCommand(
                szBackupType,
                bCheck,
                &fakeMediumForGroup,
                szFileNumber,
                szUntilDate,
                szUntilTime,
                szCommand,
                bCreate,
                LabelToCheck,
                LabelToCheckLength);
        }
        else
        {
            if (bCreate)
            {
                SAPDB_strcpy(szCommand, BA_KEY_CREATE_CN003);               /* CREATE                         */
                strcat(szCommand, BA_KEY_BLANK_CN003);                /*                                */
                strcat(szCommand, BA_KEY_INSTANCE_CN003);             /* INSTANCE                       */
                strcat(szCommand, BA_KEY_BLANK_CN003);                /*                                */
                strcat(szCommand, BA_KEY_WITH_CN003);                 /* WITH                           */
                strcat(szCommand, BA_KEY_BLANK_CN003);                /*                                */
                strcat(szCommand, BA_KEY_RESTORE_CN003);              /* RESTORE                        */
            }
            else
            {
                SAPDB_strcpy(szCommand, BA_KEY_RESTORE_CN003);              /* RESTORE                        */
            } // end if

            strcat(szCommand, BA_KEY_BLANK_CN003);                /*                                */
            strcat(szCommand, szBackupType);                      /* <restore_type>                 */
            strcat(szCommand, BA_KEY_BLANK_CN003);                /*                                */
            strcat(szCommand, BA_KEY_QUICK_CN003);                /* QUICK                          */
            strcat(szCommand, BA_KEY_BLANK_CN003);                /*                                */
            strcat(szCommand, BA_KEY_FROM_CN003);                 /* FROM                           */

            nFuncStatus = cn31_FileSpecList(szBackupType, pMediaGroup, szFileSpecList, false, bCheck);

            if (nFuncStatus == OK_CN00)
            {
                strcat(szCommand, szFileSpecList);                  /* <filespeclist>                 */

                strcat(szCommand, BA_KEY_BLANK_CN003);                  /*                                */
                strcat(szCommand, BA_KEY_BLOCKSIZE_CN003);              /* BLOCKSIZE                      */
                strcat(szCommand, BA_KEY_BLANK_CN003);                  /*                                */
                if(0==pMediaGroup->pFirst || '\0'==pMediaGroup->pFirst->szBlocksize[0])
                    strcat(szCommand, ME_DEFAULT_BLOCKSIZE_CN003);      /* "8"                            */
                else
                    strcat(szCommand, pMediaGroup->pFirst->szBlocksize);/* <blocksize>                    */

                strcat(szCommand, BA_KEY_BLANK_CN003);              /*                                */
                strcat(szCommand, BA_KEY_MEDIANAME_CN003);          /* MEDIANAME                      */
                strcat(szCommand, BA_KEY_BLANK_CN003);              /*                                */
                strcat(szCommand, BA_KEY_QUOTE_CN003);              /* '                              */
                strcat(szCommand, pMediaGroup->szName);             /* <medianame>                    */
                strcat(szCommand, BA_KEY_QUOTE_CN003);              /* '                              */

                cn31_AppendLabelToCheck(szCommand, LabelToCheck, LabelToCheckLength);  /* LABEL '<LabelToCheck>' , if LabelToCheck is a non empty string */
            }
        }
    }

    return nFuncStatus;
} /* end cn31_BuildParallelRestoreCommand */

/* PRIVATE FUNCTION cn31_BuildRestoreConfigCommand
 * ------------------------------------------------------------------
 * purpose: build the restore config command
 *
 */
static tcn00_Error cn31_BuildRestoreConfigCommand (
      tcn003_Medium  * pMedium,
      const char     * szDatabase,
      char           * szCommand )
{
  tcn00_Error         nFuncStatus  = OK_CN00;

  char                szFileSpec[BA_FILESPEC_LENGTH_MXCN003];
  /**/

  szCommand[0] = '\0';

  if (pMedium != NULL) {
    SAPDB_strcpy(szCommand, BA_KEY_RESTORE_CN003);              /* RESTORE                        */
    strcat(szCommand, BA_KEY_BLANK_CN003);                /*                                */
    strcat(szCommand, BA_KEY_CONFIG_CN003);               /* CONFIG                         */
    strcat(szCommand, BA_KEY_BLANK_CN003);                /*                                */
    strcat(szCommand, BA_KEY_QUICK_CN003);                /* QUICK                          */
    strcat(szCommand, BA_KEY_BLANK_CN003);                /*                                */
    strcat(szCommand, BA_KEY_FROM_CN003);                 /* FROM                           */

    nFuncStatus = cn31_FileSpec(BA_KEY_CONFIG_CN003, pMedium, szFileSpec, false, true);

    if (nFuncStatus == OK_CN00) {
      strcat(szCommand, BA_KEY_BLANK_CN003);              /*                                */
      strcat(szCommand, szFileSpec);                      /* <filespec>                     */
      strcat(szCommand, BA_KEY_BLANK_CN003);              /*                                */
      strcat(szCommand, BA_KEY_TO_CN003);                 /* TO                             */
      strcat(szCommand, BA_KEY_BLANK_CN003);              /*                                */
      strcat(szCommand, BA_KEY_QUOTE_CN003);              /* "                              */
      strcat(szCommand, szDatabase);                      /* '<databasename'                */
      strcat(szCommand, BA_KEY_QUOTE_CN003);              /* "                              */
      strcat(szCommand, BA_KEY_BLANK_CN003);              /*                                */
      strcat(szCommand, BA_KEY_BLOCKSIZE_CN003);          /* BLOCKSIZE                      */
      strcat(szCommand, BA_KEY_BLANK_CN003);              /*                                */
      strcat(szCommand, ('\0'==pMedium->szBlocksize[0])?ME_DEFAULT_BLOCKSIZE_CN003:pMedium->szBlocksize);          /* "8" or <blocksize>             */
    }
  }

  return nFuncStatus;
} /* end cn31_BuildRestoreConfigCommand */

/* PRIVATE FUNCTION cn31_BuildParallelRestoreConfigCommand
 * ------------------------------------------------------------------
 * purpose: build the restore config command for parallel backup
 *
 */
static tcn00_Error cn31_BuildParallelRestoreConfigCommand (
      tcn003_MediaGroup   * pMediaGroup,
      const char          * szDatabase,
      char                * szCommand )
{
    tcn00_Error nFuncStatus  = OK_CN00;
    bool        bFirstFileSpec = true;
    char        szFileSpec[BA_FILESPEC_LENGTH_MXCN003];

    szCommand[0] = '\0';

    if (pMediaGroup != NULL && 0!=pMediaGroup->pFirst) //we have a media group with at least one medium
    {
        SAPDB_strcpy(szCommand, BA_KEY_RESTORE_CN003);              /* RESTORE                        */
        strcat(szCommand, BA_KEY_BLANK_CN003);                /*                                */
        strcat(szCommand, BA_KEY_CONFIG_CN003);               /* CONFIG                         */
        strcat(szCommand, BA_KEY_BLANK_CN003);                /*                                */
        strcat(szCommand, BA_KEY_QUICK_CN003);                /* QUICK                          */
        strcat(szCommand, BA_KEY_BLANK_CN003);                /*                                */
        strcat(szCommand, BA_KEY_FROM_CN003);                 /* FROM                           */

        nFuncStatus = cn31_FileSpec(BA_KEY_CONFIG_CN003, pMediaGroup->pFirst, szFileSpec, false, true); //without any COUNT , but add rundirectory for relative file names
      
        if (nFuncStatus == OK_CN00)
        {
            strcat(szCommand, BA_KEY_BLANK_CN003);              /*                                */
            strcat(szCommand, szFileSpec);                      /* <filespec>                     */
            strcat(szCommand, BA_KEY_BLANK_CN003);              /*                                */
            strcat(szCommand, BA_KEY_TO_CN003);                 /* TO                             */
            strcat(szCommand, BA_KEY_BLANK_CN003);              /*                                */
            strcat(szCommand, BA_KEY_QUOTE_CN003);              /*                                */
            strcat(szCommand, szDatabase);                      /* '<databasename>'               */
            strcat(szCommand, BA_KEY_QUOTE_CN003);              /*                                */

            strcat(szCommand, BA_KEY_BLANK_CN003);                  /*                                */
            strcat(szCommand, BA_KEY_BLOCKSIZE_CN003);              /* BLOCKSIZE                      */
            strcat(szCommand, BA_KEY_BLANK_CN003);                  /*                                */
            if(0==pMediaGroup->pFirst || '\0'==pMediaGroup->pFirst->szBlocksize[0])
                strcat(szCommand, ME_DEFAULT_BLOCKSIZE_CN003);      /* "8"                            */
            else
                strcat(szCommand, pMediaGroup->pFirst->szBlocksize);/* <blocksize>                    */
        }
    }

    return nFuncStatus;
} /* end cn31_BuildParallelRestoreConfigCommand */

/* PRIVATE FUNCTION cn31_BuildRestoreReplaceCommand
 * ------------------------------------------------------------------
 * purpose: build the restore replace command
 *
 */
static tcn00_Error cn31_BuildRestoreReplaceCommand (
      char           * szBackupType,
      bool             bCheck,
      tcn003_Medium  * pMedium,
      char           * szFileNumber,
      char           * szCommand )
{
  tcn00_Error         nFuncStatus  = OK_CN00;

  char                szFileSpec[BA_FILESPEC_LENGTH_MXCN003];

  /**/

  szCommand[0] = '\0';

  if (pMedium != NULL) {
    SAPDB_strcpy(szCommand, BA_KEY_RESTORE_CN003);              /* RESTORE                        */
    strcat(szCommand, BA_KEY_BLANK_CN003);                /*                                */
    strcat(szCommand, szBackupType);                      /* <restore_type>                 */
    strcat(szCommand, BA_KEY_BLANK_CN003);                /*                                */
    strcat(szCommand, BA_KEY_REPLACE_CN003);              /* REPLACE                        */

    if (szFileNumber[0] != '\0') {
      nFuncStatus = cn31_FileSpecWithNumber(pMedium, szFileNumber, szFileSpec, false, bCheck);
    } else {
      nFuncStatus = cn31_FileSpec(szBackupType, pMedium, szFileSpec, false, bCheck);
    }

    if (nFuncStatus == OK_CN00) {
      strcat(szCommand, BA_KEY_BLANK_CN003);              /*                                */
      strcat(szCommand, szFileSpec);                      /* <filespec>                     */
    }
  }

  return nFuncStatus;
} /* end cn31_BuildRestoreReplaceCommand */

/* PRIVATE FUNCTION cn31_BuildRestoreIgnoreCommand
 * ------------------------------------------------------------------
 * purpose: build the restore ignore command
 *
 */
tcn00_Error cn31_BuildRestoreIgnoreCommand (
      const char     * szBackupType,
      char           * szCommand )
{
  tcn00_Error         nFuncStatus  = OK_CN00;

  /**/

  szCommand[0] = '\0';

  SAPDB_strcpy(szCommand, BA_KEY_RESTORE_CN003);              /* RESTORE                        */
  strcat(szCommand, BA_KEY_BLANK_CN003);                /*                                */
  strcat(szCommand, szBackupType);                      /* <restore_type>                 */
  strcat(szCommand, BA_KEY_BLANK_CN003);                /*                                */
  strcat(szCommand, BA_KEY_IGNORE_CN003);               /* IGNORE                         */

  return nFuncStatus;
} /* end cn31_BuildRestoreIgnoreCommand */

/* PRIVATE FUNCTION cn31_BuildRestoreCancelCommand
 * ------------------------------------------------------------------
 * purpose: build the restore cancel command
 *
 */
static tcn00_Error cn31_BuildRestoreCancelCommand (
      char           * szBackupType,
      char           * szCommand )
{
  tcn00_Error         nFuncStatus  = OK_CN00;

  /**/

  szCommand[0] = '\0';

  SAPDB_strcpy(szCommand, BA_KEY_RESTORE_CN003);              /* RESTORE                        */
  strcat(szCommand, BA_KEY_BLANK_CN003);                /*                                */
  strcat(szCommand, szBackupType);                      /* <restore_type>                 */
  strcat(szCommand, BA_KEY_BLANK_CN003);                /*                                */
  strcat(szCommand, BA_KEY_CANCEL_CN003);               /* CANCEL                         */

  return nFuncStatus;
} /* end cn31_BuildRestoreCancelCommand */

/* PRIVATE FUNCTION cn31_BuildLabelCommand
 * ------------------------------------------------------------------
 * purpose: build the label command
 *
 */
static tcn00_Error cn31_BuildLabelCommand (
      tcn003_Medium  * pMedium,
    char           * szFileNumber,
      char           * szCommand )
{
  tcn00_Error         nFuncStatus  = OK_CN00;

  char                szFileSpec[BA_FILESPEC_LENGTH_MXCN003];

  /**/

  szCommand[0] = '\0';

  SAPDB_strcpy(szCommand, BA_KEY_READLABEL_CN003);              /* READ LABEL                     */

  /*
  nFuncStatus = cn31_FileSpec(pMedium->szSaveType, pMedium, szFileSpec, false);
  */

  if (szFileNumber[0] != '\0') {
    nFuncStatus = cn31_FileSpecWithNumber(pMedium, szFileNumber, szFileSpec, false, false);
  } else {
    nFuncStatus = cn31_FileSpec(pMedium->szSaveType, pMedium, szFileSpec, false, false);
  }

  if (nFuncStatus == OK_CN00) {
    strcat(szCommand, BA_KEY_BLANK_CN003);                /*                                */
    strcat(szCommand, szFileSpec);                        /* <filespec>                     */
  }

  return nFuncStatus;
} /* end cn31_BuildLabelCommand */

/* PRIVATE FUNCTION cn31_BuildAutosaveOnCommand
 * ------------------------------------------------------------------
 * purpose: build the autosave on command
 *
 */
static tcn00_Error cn31_BuildAutosaveOnCommand (
      tcn003_Medium  * pMedium,
      char           * szCommand )
{
  tcn00_Error         nFuncStatus  = OK_CN00;

  char                szFileSpec[BA_FILESPEC_LENGTH_MXCN003];
  /**/

  szCommand[0] = '\0';

  if (pMedium != NULL) {
    SAPDB_strcpy(szCommand, BA_KEY_AUTOSAVE_CN003);             /* AUTOSAVE                       */
    strcat(szCommand, BA_KEY_BLANK_CN003);                /*                                */
    strcat(szCommand, BA_KEY_ON_CN003);                   /* ON                             */

    nFuncStatus = cn31_FileSpec(BA_KEY_LOG_CN003, pMedium, szFileSpec, false, false);

    if (nFuncStatus == OK_CN00) {
      strcat(szCommand, BA_KEY_BLANK_CN003);              /*                                */
      strcat(szCommand, szFileSpec);                      /* <filespec>                     */

      if (cn90Stricmp(pMedium->szAutoLoader, ME_PROP_YES_CN003) == 0) {
        strcat(szCommand, BA_KEY_BLANK_CN003);            /*                                */
        strcat(szCommand, BA_KEY_CASCADE_CN003);          /* CASCADE                        */
      }

      strcat(szCommand, BA_KEY_BLANK_CN003);              /*                                */
      strcat(szCommand, BA_KEY_MEDIANAME_CN003);          /* MEDIANAME                      */
      strcat(szCommand, BA_KEY_BLANK_CN003);              /*                                */
      strcat(szCommand, BA_KEY_QUOTE_CN003);              /* '                              */
      strcat(szCommand, pMedium->szName);                 /* <medianame>                    */
      strcat(szCommand, BA_KEY_QUOTE_CN003);              /* '                              */

      strcat(szCommand, BA_KEY_BLANK_CN003);              /*                                */
      strcat(szCommand, BA_KEY_BLOCKSIZE_CN003);          /* BLOCKSIZE                      */
      strcat(szCommand, BA_KEY_BLANK_CN003);              /*                                */
      strcat(szCommand, ('\0'==pMedium->szBlocksize[0])?ME_DEFAULT_BLOCKSIZE_CN003:pMedium->szBlocksize);  /* "8" or <blocksize>                 */
    }
  }

  return nFuncStatus;
} /* end cn31_BuildAutosaveOnCommand */

/* PRIVATE FUNCTION cn31_BuildParallelAutosaveOnCommand
 * ------------------------------------------------------------------
 * purpose: build the autosave on command from parallel backup
 *
 */
static tcn00_Error cn31_BuildParallelAutosaveOnCommand (
      tcn003_MediaGroup   * pMediaGroup,
      char                * szCommand )
{
  tcn00_Error         nFuncStatus  = OK_CN00;

  char                szFileSpecList[BA_FILESPEC_LENGTH_MXCN003];
  /**/

  szCommand[0] = '\0';

  if (pMediaGroup != NULL) {
    SAPDB_strcpy(szCommand, BA_KEY_AUTOSAVE_CN003);             /* AUTOSAVE                       */
    strcat(szCommand, BA_KEY_BLANK_CN003);                /*                                */
    strcat(szCommand, BA_KEY_ON_CN003);                   /* ON                             */

    nFuncStatus = cn31_FileSpecList(BA_KEY_LOG_CN003, pMediaGroup, szFileSpecList, false, false);

    if (nFuncStatus == OK_CN00) {
      strcat(szCommand, szFileSpecList);                  /* <filespec>                     */
      strcat(szCommand, BA_KEY_BLANK_CN003);              /*                                */
      strcat(szCommand, BA_KEY_MEDIANAME_CN003);          /* MEDIANAME                      */
      strcat(szCommand, BA_KEY_BLANK_CN003);              /*                                */
      strcat(szCommand, BA_KEY_QUOTE_CN003);              /* '                              */
      strcat(szCommand, pMediaGroup->szName);             /* <medianame>                    */
      strcat(szCommand, BA_KEY_QUOTE_CN003);              /* '                              */
    }
  }

  return nFuncStatus;
} /* end cn31_BuildParallelAutosaveOnCommand */

/* PRIVATE FUNCTION cn31_BuildAutosaveEndCommand
 * ------------------------------------------------------------------
 * purpose: build the autosave end command
 *
 */
static tcn00_Error cn31_BuildAutosaveEndCommand (
      char           * szCommand )
{
  tcn00_Error         nFuncStatus  = OK_CN00;

  /**/

  SAPDB_strcpy(szCommand, BA_KEY_AUTOSAVE_CN003);             /* AUTOSAVE                       */
  strcat(szCommand, BA_KEY_BLANK_CN003);                /*                                */
  strcat(szCommand, BA_KEY_END_CN003);                  /* END                             */

  return nFuncStatus;
} /* end cn31_BuildAutosaveEndCommand */

/* PRIVATE FUNCTION cn31_BuildAutosaveCancelCommand
 * ------------------------------------------------------------------
 * purpose: build the autosave Cancel command
 *
 */
static tcn00_Error cn31_BuildAutosaveCancelCommand (
      char           * szCommand )
{
  tcn00_Error         nFuncStatus  = OK_CN00;

  /**/

  SAPDB_strcpy(szCommand, BA_KEY_AUTOSAVE_CN003);             /* AUTOSAVE                       */
  strcat(szCommand, BA_KEY_BLANK_CN003);                /*                                */
  strcat(szCommand, BA_KEY_CANCEL_CN003);               /* CANCEL                         */

  return nFuncStatus;
} /* end cn31_BuildAutosaveCancelCommand */

/* PRIVATE FUNCTION cn31_BuildAutosaveShowCommand
 * ------------------------------------------------------------------
 * purpose: build the autosave Cancel command
 *
 */
static tcn00_Error cn31_BuildAutosaveShowCommand (
      char           * szCommand )
{
  tcn00_Error         nFuncStatus  = OK_CN00;

  /**/

  SAPDB_strcpy(szCommand, BA_KEY_AUTOSAVE_CN003);            /* AUTOSAVE                       */
  strcat(szCommand, BA_KEY_BLANK_CN003);               /*                                */
  strcat(szCommand, BA_KEY_SHOW_CN003);                /* SHOW                           */

  return nFuncStatus;
} /* end cn31_BuildAutosaveShowCommand */

/* PRIVATE FUNCTION cn31_BuildRestoreDevspaceCommand
 * ------------------------------------------------------------------
 * purpose: build the autosave Cancel command
 *
 */
static tcn00_Error cn31_BuildRestoreDevspaceCommand (
      const char     * szDevspace,
      char           * szCommand )
{
  tcn00_Error         nFuncStatus  = OK_CN00;

  /**/

  SAPDB_strcpy(szCommand, BA_KEY_RESTORE_CN003);             /* RESTORE                        */
  strcat(szCommand, BA_KEY_BLANK_CN003);               /*                                */
  strcat(szCommand, ME_SAVETYPE_LOG_CN003);            /* LOG                            */
  strcat(szCommand, BA_KEY_BLANK_CN003);               /*                                */
  strcat(szCommand, BA_KEY_VOLUME_CN003);              /* VOLUME                         */
  strcat(szCommand, BA_KEY_BLANK_CN003);               /*                                */
  strcat(szCommand, BA_KEY_QUOTE_CN003);               /* '                              */
  strcat(szCommand, szDevspace);                       /*                                */
  strcat(szCommand, BA_KEY_QUOTE_CN003);               /* '                              */

  return nFuncStatus;
} /* end cn31_BuildRestoreDevspaceCommand */

  /* PRIVATE FUNCTION cn31_InitResult
 * ------------------------------------------------------------------
 * purpose: initilize label struct
 *
 */
void cn31_InitResult (
      tcn003_BackupResult  * pResult )
{
  pResult->nBackupRC = 0;
  pResult->szDate[0] = '\0';
  pResult->szTime[0] = '\0';
  pResult->szDatabase[0] = '\0';
  pResult->szServer[0] = '\0';
  pResult->szKernelVersion[0] = '\0';
  pResult->szPagesTransfered[0] = '\0';
  pResult->szPagesLeft[0] = '\0';
  pResult->szVolumes[0] = '\0';
  pResult->szMediaName[0] = '\0';
  pResult->szLocation[0] = '\0';
  pResult->szErrortext[0] = '\0';
  pResult->szLabel[0] = '\0';
  pResult->szIsConsistent[0] = '\0';
  pResult->szFirstLogPageNo[0] = '\0';
  pResult->szLastLogPageNo[0] = '\0';
  pResult->szDBStamp1Date[0] = '\0';
  pResult->szDBStamp1Time[0] = '\0';
  pResult->szDBStamp2Date[0] = '\0';
  pResult->szDBStamp2Time[0] = '\0';
  pResult->szBDPageCount[0] = '\0';
  pResult->szDevicesUsed[0] = '\0';
  pResult->szDatabaseID[0] = '\0';
  pResult->szMaxUsedDataPage[0] = '\0';
  pResult->szConvPageCnt[0]='\0';
  pResult->szRedoTransRead[0] = '\0';
  pResult->szRedoTransDone[0] = '\0';
} /* end cn31_InitResult */

/* PRIVATE FUNCTION cn31_PrintBackupAnswer
 * ------------------------------------------------------------------
 */
void cn31_PrintBackupAnswer (
        char                * replyData,
        tcn003_BackupResult * tResult,
    bool                  bState)
{
  char * pCurrPos      = NULL;
  static int nMaxPages = 0;
  int nPagesTransf     = atoi(tResult->szPagesTransfered);
  int nPagesLeft       = atoi(tResult->szPagesLeft);
  int nPageCount       = atoi(tResult->szBDPageCount);

  if (bState) {
    // fake the answer for log backup
    if ((nPagesTransf == 0) && (nPagesLeft == 0) && (nPageCount == 0)) {
      nMaxPages = 0;
    } else {
      nMaxPages    = (nMaxPages < (nPagesTransf + nPagesLeft)) ? (nPagesTransf + nPagesLeft) : nMaxPages;
      nPagesTransf = nMaxPages - nPagesLeft;
      nPageCount   = nMaxPages;
    } // end if
  } // end if

  sprintf(replyData, "%s%s", ANSWER_OK_CN00, LINE_SEPSTRING_CN00);
  pCurrPos = replyData + strlen(replyData);

  sprintf(pCurrPos, "%-20s%s%d%s",BA_RESULT_RC_CN003, VALUE_SEPSTRING_CN00, tResult->nBackupRC, LINE_SEPSTRING_CN00);
  pCurrPos = replyData + strlen(replyData);

  sprintf(pCurrPos, "%-20s%s%s%s",BA_RESULT_DATE_CN003, VALUE_SEPSTRING_CN00, tResult->szDate, LINE_SEPSTRING_CN00);
  pCurrPos = replyData + strlen(replyData);

  sprintf(pCurrPos, "%-20s%s%s%s",BA_RESULT_TIME_CN003, VALUE_SEPSTRING_CN00, tResult->szTime, LINE_SEPSTRING_CN00);
  pCurrPos = replyData + strlen(replyData);

  sprintf(pCurrPos, "%-20s%s%s%s",BA_RESULT_SERVER_CN003, VALUE_SEPSTRING_CN00, tResult->szServer, LINE_SEPSTRING_CN00);
  pCurrPos = replyData + strlen(replyData);

  sprintf(pCurrPos, "%-20s%s%s%s",BA_RESULT_DATABASE_CN003, VALUE_SEPSTRING_CN00, tResult->szDatabase, LINE_SEPSTRING_CN00);
  pCurrPos = replyData + strlen(replyData);

  sprintf(pCurrPos, "%-20s%s%s%s",BA_RESULT_KNLVER_CN003, VALUE_SEPSTRING_CN00, tResult->szKernelVersion, LINE_SEPSTRING_CN00);
  pCurrPos = replyData + strlen(replyData);

  sprintf(pCurrPos, "%-20s%s%d%s",BA_RESULT_PGSTRAN_CN003, VALUE_SEPSTRING_CN00, nPagesTransf, LINE_SEPSTRING_CN00);
  pCurrPos = replyData + strlen(replyData);

  sprintf(pCurrPos, "%-20s%s%d%s",BA_RESULT_PGSLEFT_CN003, VALUE_SEPSTRING_CN00, nPagesLeft, LINE_SEPSTRING_CN00);
  pCurrPos = replyData + strlen(replyData);

  sprintf(pCurrPos, "%-20s%s%s%s",BA_RESULT_VOLUMES_CN003, VALUE_SEPSTRING_CN00, tResult->szVolumes, LINE_SEPSTRING_CN00);
  pCurrPos = replyData + strlen(replyData);

  sprintf(pCurrPos, "%-20s%s%s%s",BA_RESULT_MEDNAME_CN003, VALUE_SEPSTRING_CN00, tResult->szMediaName, LINE_SEPSTRING_CN00);
  pCurrPos = replyData + strlen(replyData);

  sprintf(pCurrPos, "%-20s%s%s%s",BA_RESULT_LOCATION_CN003, VALUE_SEPSTRING_CN00, tResult->szLocation, LINE_SEPSTRING_CN00);
  pCurrPos = replyData + strlen(replyData);

  sprintf(pCurrPos, "%-20s%s%s%s",BA_RESULT_ERRTEXT_CN003, VALUE_SEPSTRING_CN00, tResult->szErrortext, LINE_SEPSTRING_CN00);
  pCurrPos = replyData + strlen(replyData);

  sprintf(pCurrPos, "%-20s%s%s%s",BA_RESULT_LABEL_CN003, VALUE_SEPSTRING_CN00, tResult->szLabel, LINE_SEPSTRING_CN00);
  pCurrPos = replyData + strlen(replyData);

  sprintf(pCurrPos, "%-20s%s%s%s",BA_RESULT_ISCONS_CN003, VALUE_SEPSTRING_CN00, tResult->szIsConsistent, LINE_SEPSTRING_CN00);
  pCurrPos = replyData + strlen(replyData);

  sprintf(pCurrPos, "%-20s%s%s%s",BA_RESULT_FLOGPG_CN003, VALUE_SEPSTRING_CN00, tResult->szFirstLogPageNo, LINE_SEPSTRING_CN00);
  pCurrPos = replyData + strlen(replyData);

  sprintf(pCurrPos, "%-20s%s%s%s",BA_RESULT_LLOGPG_CN003, VALUE_SEPSTRING_CN00, tResult->szLastLogPageNo, LINE_SEPSTRING_CN00);
  pCurrPos = replyData + strlen(replyData);

  sprintf(pCurrPos, "%-20s%s%s%s",BA_RESULT_DBSTMD1_CN003, VALUE_SEPSTRING_CN00, tResult->szDBStamp1Date, LINE_SEPSTRING_CN00);
  pCurrPos = replyData + strlen(replyData);

  sprintf(pCurrPos, "%-20s%s%s%s",BA_RESULT_DBSTMT1_CN003, VALUE_SEPSTRING_CN00, tResult->szDBStamp1Time, LINE_SEPSTRING_CN00);
  pCurrPos = replyData + strlen(replyData);

  sprintf(pCurrPos, "%-20s%s%s%s",BA_RESULT_DBSTMD2_CN003, VALUE_SEPSTRING_CN00, tResult->szDBStamp2Date, LINE_SEPSTRING_CN00);
  pCurrPos = replyData + strlen(replyData);

  sprintf(pCurrPos, "%-20s%s%s%s",BA_RESULT_DBSTMT2_CN003, VALUE_SEPSTRING_CN00, tResult->szDBStamp2Time, LINE_SEPSTRING_CN00);
  pCurrPos = replyData + strlen(replyData);

  sprintf(pCurrPos, "%-20s%s%d%s",BA_RESULT_PGCNT_CN003, VALUE_SEPSTRING_CN00, nPageCount, LINE_SEPSTRING_CN00);
  pCurrPos = replyData + strlen(replyData);

  sprintf(pCurrPos, "%-20s%s%s%s",BA_RESULT_DEVUSE_CN003, VALUE_SEPSTRING_CN00, tResult->szDevicesUsed, LINE_SEPSTRING_CN00);
  pCurrPos = replyData + strlen(replyData);

  sprintf(pCurrPos, "%-20s%s%s%s",BA_RESULT_DBID_CN003, VALUE_SEPSTRING_CN00, tResult->szDatabaseID, LINE_SEPSTRING_CN00);
  pCurrPos = replyData + strlen(replyData);

  sprintf(pCurrPos, "%-20s%s%s%s",BA_RESULT_MUSEDATAPG_CN003, VALUE_SEPSTRING_CN00, tResult->szMaxUsedDataPage, LINE_SEPSTRING_CN00);
  pCurrPos = replyData + strlen(replyData);

  sprintf(pCurrPos, "%-20s%s%s%s",BA_RESULT_CONVPGCNT_CN003, VALUE_SEPSTRING_CN00, tResult->szConvPageCnt, LINE_SEPSTRING_CN00);
  pCurrPos = replyData + strlen(replyData);

  if(bState == true)
  {
    sprintf(pCurrPos, "%-20s%s%s%s",BA_RESULT_REDOTRANSR_CN003, VALUE_SEPSTRING_CN00, tResult->szRedoTransRead, LINE_SEPSTRING_CN00);
    pCurrPos = replyData + strlen(replyData);

    sprintf(pCurrPos, "%-20s%s%s%s",BA_RESULT_REDOTRANSD_CN003, VALUE_SEPSTRING_CN00, tResult->szRedoTransDone, LINE_SEPSTRING_CN00);
    pCurrPos = replyData + strlen(replyData);
  }
} /* end cn31_PrintBackupAnswer */

/* PRIVATE FUNCTION cn31_PrintLogBackupAnswer
 * ------------------------------------------------------------------
 */
static void cn31_PrintLogBackupAnswer (
        char                * replyData,
        tcn003_BackupResult * tResult,
        bool                  bState)
{
  char                * pCurrPos=0;
  DBMSrvBHist_History   History;

  History.AnalyzeLogBackup(tResult);

  static int nMaxPages = 0;
  int nPagesTransf     = atoi(tResult->szPagesTransfered);
  int nPagesLeft       = atoi(tResult->szPagesLeft);
  int nPageCount       = atoi(tResult->szBDPageCount);

  nPageCount = nPagesTransf + nPagesLeft;

  sprintf(replyData, "%s%s", ANSWER_OK_CN00, LINE_SEPSTRING_CN00);
  pCurrPos = replyData + strlen(replyData);

  sprintf(pCurrPos, "%-20s%s%d%s",BA_RESULT_RC_CN003, VALUE_SEPSTRING_CN00, tResult->nBackupRC, LINE_SEPSTRING_CN00);
  pCurrPos = replyData + strlen(replyData);

  sprintf(pCurrPos, "%-20s%s%s%s",BA_RESULT_DATE_CN003, VALUE_SEPSTRING_CN00, tResult->szDate, LINE_SEPSTRING_CN00);
  pCurrPos = replyData + strlen(replyData);

  sprintf(pCurrPos, "%-20s%s%s%s",BA_RESULT_TIME_CN003, VALUE_SEPSTRING_CN00, tResult->szTime, LINE_SEPSTRING_CN00);
  pCurrPos = replyData + strlen(replyData);

  sprintf(pCurrPos, "%-20s%s%s%s",BA_RESULT_SERVER_CN003, VALUE_SEPSTRING_CN00, tResult->szServer, LINE_SEPSTRING_CN00);
  pCurrPos = replyData + strlen(replyData);

  sprintf(pCurrPos, "%-20s%s%s%s",BA_RESULT_DATABASE_CN003, VALUE_SEPSTRING_CN00, tResult->szDatabase, LINE_SEPSTRING_CN00);
  pCurrPos = replyData + strlen(replyData);

  sprintf(pCurrPos, "%-20s%s%s%s",BA_RESULT_KNLVER_CN003, VALUE_SEPSTRING_CN00, tResult->szKernelVersion, LINE_SEPSTRING_CN00);
  pCurrPos = replyData + strlen(replyData);

  sprintf(pCurrPos, "%-20s%s%d%s",BA_RESULT_PGSTRAN_CN003, VALUE_SEPSTRING_CN00, nPagesTransf, LINE_SEPSTRING_CN00);
  pCurrPos = replyData + strlen(replyData);

  sprintf(pCurrPos, "%-20s%s%d%s",BA_RESULT_PGSLEFT_CN003, VALUE_SEPSTRING_CN00, nPagesLeft, LINE_SEPSTRING_CN00);
  pCurrPos = replyData + strlen(replyData);

  sprintf(pCurrPos, "%-20s%s%s%s",BA_RESULT_VOLUMES_CN003, VALUE_SEPSTRING_CN00, tResult->szVolumes, LINE_SEPSTRING_CN00);
  pCurrPos = replyData + strlen(replyData);

  sprintf(pCurrPos, "%-20s%s%s%s",BA_RESULT_MEDNAME_CN003, VALUE_SEPSTRING_CN00, tResult->szMediaName, LINE_SEPSTRING_CN00);
  pCurrPos = replyData + strlen(replyData);

  sprintf(pCurrPos, "%-20s%s%s%s",BA_RESULT_LOCATION_CN003, VALUE_SEPSTRING_CN00, tResult->szLocation, LINE_SEPSTRING_CN00);
  pCurrPos = replyData + strlen(replyData);

  sprintf(pCurrPos, "%-20s%s%s%s",BA_RESULT_ERRTEXT_CN003, VALUE_SEPSTRING_CN00, tResult->szErrortext, LINE_SEPSTRING_CN00);
  pCurrPos = replyData + strlen(replyData);

  sprintf(pCurrPos, "%-20s%s%s%s",BA_RESULT_LABEL_CN003, VALUE_SEPSTRING_CN00, tResult->szLabel, LINE_SEPSTRING_CN00);
  pCurrPos = replyData + strlen(replyData);

  sprintf(pCurrPos, "%-20s%s%s%s",BA_RESULT_ISCONS_CN003, VALUE_SEPSTRING_CN00, tResult->szIsConsistent, LINE_SEPSTRING_CN00);
  pCurrPos = replyData + strlen(replyData);

  sprintf(pCurrPos, "%-20s%s%s%s",BA_RESULT_FLOGPG_CN003, VALUE_SEPSTRING_CN00, tResult->szFirstLogPageNo, LINE_SEPSTRING_CN00);
  pCurrPos = replyData + strlen(replyData);

  sprintf(pCurrPos, "%-20s%s%s%s",BA_RESULT_LLOGPG_CN003, VALUE_SEPSTRING_CN00, tResult->szLastLogPageNo, LINE_SEPSTRING_CN00);
  pCurrPos = replyData + strlen(replyData);

  sprintf(pCurrPos, "%-20s%s%s%s",BA_RESULT_DBSTMD1_CN003, VALUE_SEPSTRING_CN00, tResult->szDBStamp1Date, LINE_SEPSTRING_CN00);
  pCurrPos = replyData + strlen(replyData);

  sprintf(pCurrPos, "%-20s%s%s%s",BA_RESULT_DBSTMT1_CN003, VALUE_SEPSTRING_CN00, tResult->szDBStamp1Time, LINE_SEPSTRING_CN00);
  pCurrPos = replyData + strlen(replyData);

  sprintf(pCurrPos, "%-20s%s%s%s",BA_RESULT_DBSTMD2_CN003, VALUE_SEPSTRING_CN00, tResult->szDBStamp2Date, LINE_SEPSTRING_CN00);
  pCurrPos = replyData + strlen(replyData);

  sprintf(pCurrPos, "%-20s%s%s%s",BA_RESULT_DBSTMT2_CN003, VALUE_SEPSTRING_CN00, tResult->szDBStamp2Time, LINE_SEPSTRING_CN00);
  pCurrPos = replyData + strlen(replyData);

  sprintf(pCurrPos, "%-20s%s%d%s",BA_RESULT_PGCNT_CN003, VALUE_SEPSTRING_CN00, nPageCount, LINE_SEPSTRING_CN00);
  pCurrPos = replyData + strlen(replyData);

  sprintf(pCurrPos, "%-20s%s%s%s",BA_RESULT_DEVUSE_CN003, VALUE_SEPSTRING_CN00, tResult->szDevicesUsed, LINE_SEPSTRING_CN00);
  pCurrPos = replyData + strlen(replyData);

  sprintf(pCurrPos, "%-20s%s%s%s",BA_RESULT_DBID_CN003, VALUE_SEPSTRING_CN00, tResult->szDatabaseID, LINE_SEPSTRING_CN00);
  pCurrPos = replyData + strlen(replyData);

  sprintf(pCurrPos, "%-20s%s%s%s",BA_RESULT_MUSEDATAPG_CN003, VALUE_SEPSTRING_CN00, tResult->szMaxUsedDataPage, LINE_SEPSTRING_CN00);
  pCurrPos = replyData + strlen(replyData);

  sprintf(pCurrPos, "%-20s%s%s%s",BA_RESULT_CONVPGCNT_CN003, VALUE_SEPSTRING_CN00, tResult->szConvPageCnt, LINE_SEPSTRING_CN00);
  pCurrPos = replyData + strlen(replyData);

  if(bState == true)
  {
    sprintf(pCurrPos, "%-20s%s%s%s",BA_RESULT_REDOTRANSR_CN003, VALUE_SEPSTRING_CN00, tResult->szRedoTransRead, LINE_SEPSTRING_CN00);
    pCurrPos = replyData + strlen(replyData);

    sprintf(pCurrPos, "%-20s%s%s%s",BA_RESULT_REDOTRANSD_CN003, VALUE_SEPSTRING_CN00, tResult->szRedoTransDone, LINE_SEPSTRING_CN00);
    pCurrPos = replyData + strlen(replyData);
  }
} /* end cn31_PrintLogBackupAnswer */

/* PRIVATE FUNCTION cn31_BackupCommandRequest
 * ------------------------------------------------------------------
 * purpose: send backup command request to kernel
 *
 */
static tcn00_Error cn31_BackupCommandRequest (
        char                * replyData,
        int                 * replyLen,
        int                   replyLenMax,
        DBMSrvKnl_Session   & oSession,
        const char          * szBackupCommand,
        tcn003_BackupResult * pResult)
{
  tcn00_Error           nFuncStatus  = OK_CN00;

  // request
  tcn53_DBAAction::writeDetail(DBADTL_BACMD_CN53, szBackupCommand);

  // answer
  DBMSrvMsg_Error oError;
  if (!oSession.Request(szBackupCommand, oError)) {
    nFuncStatus = cn90AnswerEvent(replyData, replyLen, replyLenMax, oError);
    // if we have a kernel RC, we report it (otherwise general error)
    if( oError == DBMSrvMsg_Error::DBMSrv_SQL ) {
        pResult->nBackupRC = oError.getTagValueAsInteger(SDBMSGTAG_DBMSRV_SQL__RETURNCODE);
    }
    else 
        pResult->nBackupRC = DBMSrvMsg_Error::get_tcn00_Error(DBMSrvMsg_Error::DBMSrv_ERR);

    tcn53_DBAAction::writeDetail(DBADTL_BACMDERR_CN53, replyData);
  } else {
    nFuncStatus = cn90AnswerOK (replyData, replyLen, NULL);
    tcn53_DBAAction::writeDetail(DBADTL_BACMDOK_CN53, replyData);
  } // end if

  return nFuncStatus;
} /* end cn31_BackupCommandRequest */

/* PRIVATE FUNCTION cn31BackupResultReceive
 * ------------------------------------------------------------------
 * purpose: receives the backup result from kernel
 *
 */
int cn31BackupResultReceive
  ( DBMSrvKnl_Session   & oSession,
    bool                & bBackupRc,
    tcn003_BackupResult & oResult,
    DBMSrvMsg_Error     & oError )
{
  DBMSrvKnl_Session::ResultType oResultType;
  int                           nResult = 0;

  memset(&oResult, 0 , sizeof(oResult));
  bBackupRc = true;

  // receive answer
  if (!oSession.Receive(oResultType, oError)) {
    // look for kernel return code
    if (oError == DBMSrvMsg_Error::DBMSrv_SQL) {
      nResult = oError.getTagValueAsInteger(SDBMSGTAG_DBMSRV_SQL__RETURNCODE);
      // check for special backup codes
      if ((nResult != BA_NEXTMEDIUM_RC_CN003) &&
          (nResult != BA_SAVESKIPPED_CN003  ) &&
          (nResult != BA_NOMOREDATA_CN003   )     ) {
        // here we have a real error
        bBackupRc = false;
      } // end if
    } else {
      // here we have a real error
      nResult = -1;
      bBackupRc = false;
    } // end if
  } // end if

  // if we have a backup answer copy result
  if (bBackupRc) {
    DBMSrvKnl_SQLRecord & oRecord = oSession.GetRecordObject();
    if (!oRecord.isNull(BA_RESULTKNL_DATE_CN003      )) strncpy(oResult.szDate,            oRecord.fieldAsChar(BA_RESULTKNL_DATE_CN003      ).CharPtr(), sizeof(oResult.szDate            )-1);
    if (!oRecord.isNull(BA_RESULTKNL_TIME_CN003      )) strncpy(oResult.szTime,            oRecord.fieldAsChar(BA_RESULTKNL_TIME_CN003      ).CharPtr(), sizeof(oResult.szTime            )-1);
    if (!oRecord.isNull(BA_RESULTKNL_DATABASE_CN003  )) strncpy(oResult.szDatabase,        oRecord.fieldAsChar(BA_RESULTKNL_DATABASE_CN003  ).CharPtr(), sizeof(oResult.szDatabase        )-1);
    if (!oRecord.isNull(BA_RESULTKNL_SERVER_CN003    )) strncpy(oResult.szServer,          oRecord.fieldAsChar(BA_RESULTKNL_SERVER_CN003    ).CharPtr(), sizeof(oResult.szServer          )-1);
    if (!oRecord.isNull(BA_RESULTKNL_KNLVER_CN003    )) strncpy(oResult.szKernelVersion,   oRecord.fieldAsChar(BA_RESULTKNL_KNLVER_CN003    ).CharPtr(), sizeof(oResult.szKernelVersion   )-1);
    if (!oRecord.isNull(BA_RESULTKNL_PGSTRAN_CN003   )) strncpy(oResult.szPagesTransfered, oRecord.fieldAsChar(BA_RESULTKNL_PGSTRAN_CN003   ).CharPtr(), sizeof(oResult.szPagesTransfered )-1);
    if (!oRecord.isNull(BA_RESULTKNL_PGSLEFT_CN003   )) strncpy(oResult.szPagesLeft,       oRecord.fieldAsChar(BA_RESULTKNL_PGSLEFT_CN003   ).CharPtr(), sizeof(oResult.szPagesLeft       )-1);
    if (!oRecord.isNull(BA_RESULTKNL_VOLUMES_CN003   )) strncpy(oResult.szVolumes,         oRecord.fieldAsChar(BA_RESULTKNL_VOLUMES_CN003   ).CharPtr(), sizeof(oResult.szVolumes         )-1);
    if (!oRecord.isNull(BA_RESULTKNL_MEDNAME_CN003   )) strncpy(oResult.szMediaName,       oRecord.fieldAsChar(BA_RESULTKNL_MEDNAME_CN003   ).CharPtr(), sizeof(oResult.szMediaName       )-1);
    if (!oRecord.isNull(BA_RESULTKNL_LOCATION_CN003  )) strncpy(oResult.szLocation,        oRecord.fieldAsChar(BA_RESULTKNL_LOCATION_CN003  ).CharPtr(), sizeof(oResult.szLocation        )-1);
    if (!oRecord.isNull(BA_RESULTKNL_ERRTEXT_CN003   )) strncpy(oResult.szErrortext,       oRecord.fieldAsChar(BA_RESULTKNL_ERRTEXT_CN003   ).CharPtr(), sizeof(oResult.szErrortext       )-1);
    if (!oRecord.isNull(BA_RESULTKNL_LABEL_CN003     )) strncpy(oResult.szLabel,           oRecord.fieldAsChar(BA_RESULTKNL_LABEL_CN003     ).CharPtr(), sizeof(oResult.szLabel           )-1);
    if (!oRecord.isNull(BA_RESULTKNL_ISCONS_CN003    )) strncpy(oResult.szIsConsistent,    oRecord.fieldAsChar(BA_RESULTKNL_ISCONS_CN003    ).CharPtr(), sizeof(oResult.szIsConsistent    )-1);
    if (!oRecord.isNull(BA_RESULTKNL_FLOGPG_CN003    )) strncpy(oResult.szFirstLogPageNo,  oRecord.fieldAsChar(BA_RESULTKNL_FLOGPG_CN003    ).CharPtr(), sizeof(oResult.szFirstLogPageNo  )-1);
    if (!oRecord.isNull(BA_RESULTKNL_LLOGPG_CN003    )) strncpy(oResult.szLastLogPageNo,   oRecord.fieldAsChar(BA_RESULTKNL_LLOGPG_CN003    ).CharPtr(), sizeof(oResult.szLastLogPageNo   )-1);
    if (!oRecord.isNull(BA_RESULTKNL_DBSTMD1_CN003   )) strncpy(oResult.szDBStamp1Date,    oRecord.fieldAsChar(BA_RESULTKNL_DBSTMD1_CN003   ).CharPtr(), sizeof(oResult.szDBStamp1Date    )-1);
    if (!oRecord.isNull(BA_RESULTKNL_DBSTMT1_CN003   )) strncpy(oResult.szDBStamp1Time,    oRecord.fieldAsChar(BA_RESULTKNL_DBSTMT1_CN003   ).CharPtr(), sizeof(oResult.szDBStamp1Time    )-1);
    if (!oRecord.isNull(BA_RESULTKNL_DBSTMD2_CN003   )) strncpy(oResult.szDBStamp2Date,    oRecord.fieldAsChar(BA_RESULTKNL_DBSTMD2_CN003   ).CharPtr(), sizeof(oResult.szDBStamp2Date    )-1);
    if (!oRecord.isNull(BA_RESULTKNL_DBSTMT2_CN003   )) strncpy(oResult.szDBStamp2Time,    oRecord.fieldAsChar(BA_RESULTKNL_DBSTMT2_CN003   ).CharPtr(), sizeof(oResult.szDBStamp2Time    )-1);
    if (!oRecord.isNull(BA_RESULTKNL_PGCNT_CN003     )) strncpy(oResult.szBDPageCount,     oRecord.fieldAsChar(BA_RESULTKNL_PGCNT_CN003     ).CharPtr(), sizeof(oResult.szBDPageCount     )-1);
    if (!oRecord.isNull(BA_RESULTKNL_DEVUSE_CN003    )) strncpy(oResult.szDevicesUsed,     oRecord.fieldAsChar(BA_RESULTKNL_DEVUSE_CN003    ).CharPtr(), sizeof(oResult.szDevicesUsed     )-1);
    if (!oRecord.isNull(BA_RESULTKNL_DBID_CN003      )) strncpy(oResult.szDatabaseID,      oRecord.fieldAsChar(BA_RESULTKNL_DBID_CN003      ).CharPtr(), sizeof(oResult.szDatabaseID      )-1);
    if (!oRecord.isNull(BA_RESULTKNL_MUSEDATAPG_CN003)) strncpy(oResult.szMaxUsedDataPage, oRecord.fieldAsChar(BA_RESULTKNL_MUSEDATAPG_CN003).CharPtr(), sizeof(oResult.szMaxUsedDataPage )-1);
    if (!oRecord.isNull(BA_RESULTKNL_CONVPGCNT_CN003 )) strncpy(oResult.szConvPageCnt,     oRecord.fieldAsChar(BA_RESULTKNL_CONVPGCNT_CN003 ).CharPtr(), sizeof(oResult.szConvPageCnt     )-1);
    if (!oRecord.isNull(BA_RESULTKNL_REDOTRANSR_CN003)) strncpy(oResult.szRedoTransRead,   oRecord.fieldAsChar(BA_RESULTKNL_REDOTRANSR_CN003).CharPtr(), sizeof(oResult.szRedoTransRead   )-1);
    if (!oRecord.isNull(BA_RESULTKNL_REDOTRANSD_CN003)) strncpy(oResult.szRedoTransDone,   oRecord.fieldAsChar(BA_RESULTKNL_REDOTRANSD_CN003).CharPtr(), sizeof(oResult.szRedoTransDone   )-1);
  } // end if

  return nResult;
} // end cn31BackupResultReceive

/* PRIVATE FUNCTION cn31_BackupCommandReceive
 * ------------------------------------------------------------------
 * purpose: receives the backup result from kernel
 *
 */
static tcn00_Error cn31_BackupCommandReceive (
        char                 * replyData,
        int                  * replyLen,
        int                    replyLenMax,
        DBMSrvKnl_Session    & oSession,
        tcn003_BackupResult  * pResult,
        bool                   bState,
        tcn003_BackupSession * pBackupSession)
{
  tcn00_Error nFuncStatus  = OK_CN00;
  bool        bBackupRC;

  // receive
  DBMSrvMsg_Error oError;
  pResult->nBackupRC = cn31BackupResultReceive (oSession, bBackupRC, *pResult, oError);

  // answer
  if ( (pBackupSession != NULL) && 
       (oError == DBMSrvMsg_Error::DBMSrv_SHUTDOWN) &&
       (pBackupSession->bCancelActive) &&
       (cn90Stricmp(pBackupSession->szBackupType, BA_KEY_LOG_CN003) == 0 ) ) {
    pResult->nBackupRC = 0;
  } // end if
  if ( (pResult->nBackupRC != 0) &&  !bBackupRC) {
    nFuncStatus = cn90AnswerEvent(replyData, replyLen, replyLenMax, oError);
  } else {
    if ((pBackupSession != NULL                                           ) && 
        (pBackupSession->nSessionType == BA_SESSION_TYPE_SAVE_CN003       ) &&
        (cn90Stricmp(pBackupSession->szBackupType, BA_KEY_LOG_CN003) == 0 )    ) {
      cn31_PrintLogBackupAnswer(replyData, pResult, bState);
    } else {
      cn31_PrintBackupAnswer(replyData, pResult, bState);
    } // end if
  } // end if

  tcn53_DBAAction::writeDetail(DBADTL_BARESULT_CN53, replyData);

  return nFuncStatus;
} /* end cn31_BackupCommandReceive */

/* PRIVATE FUNCTION cn31_BackupCommand
 * ------------------------------------------------------------------
 * purpose: execute the backup command and return result
 *
 */
static tcn00_Error cn31_BackupCommand (
        char                * replyData,
        int                 * replyLen,
        int                   replyLenMax,
        DBMSrvKnl_Session   & oSession,
        const char          * szBackupCommand,
        bool                  bBackupState,
        tcn003_BackupResult * pResult)
{
  tcn00_Error           nFuncStatus  = OK_CN00;

  nFuncStatus = cn31_BackupCommandRequest (replyData,
                                           replyLen,
                                           replyLenMax,
                                           oSession,
                                           szBackupCommand,
                                           pResult);

  if (nFuncStatus == OK_CN00) {
    nFuncStatus = cn31_BackupCommandReceive (replyData,
                                             replyLen,
                                             replyLenMax,
                                             oSession,
                                             pResult,
                                             bBackupState,
                                             NULL);
  } // end if

  return nFuncStatus;
} /* end cn31_BackupCommand */

/* PRIVATE FUNCTION cn31_ExecuteOSCommand
 * ------------------------------------------------------------------
 * purpose: execute an OS command
 *
 */
static tcn00_Error cn31_ExecuteOSCommandMedium (
        char          * replyData,
        int           * replyLen,
        tcn003_Medium * pMedium )
{
  tcn00_Error         nFuncStatus  = OK_CN00;
  int                 nSysReturn  = 0;

  /**/

  if (pMedium != NULL) {
    if ((pMedium->szOSCommand != NULL) && (strlen(pMedium->szOSCommand) > 0)) {
      nSysReturn = system(cn90Strip(pMedium->szOSCommand));
      if (nSysReturn == SYS_SYSTEM_ERR) {
        nFuncStatus = ERR_SYSTEM_CN00;
        cn90AnswerErrnoError(replyData, replyLen, nFuncStatus, errno);
      } else if (nSysReturn != 0) {
        nFuncStatus = ERR_EXECUTE_CN00;
        cn90AnswerIError(replyData, replyLen, nFuncStatus);
      } // end if
    } // end if
  } // end if

  return nFuncStatus;
} /* end cn31_ExecuteOSCommandMedium */

/* PRIVATE FUNCTION cn31_BackupOSCommandForMedium
 * ------------------------------------------------------------------
 * purpose: execute the OS command for the media from result
 *
 */
static tcn00_Error cn31_BackupOSCommandForMedium (
        char                        * replyData,
        int                         * replyLen,
        tcn003_BackupSession        * pBackupSession,
        tcn003_Medium              ** pMedium)
{
  tcn00_Error           nFuncStatus  = OK_CN00;
  tcn003_MediaGroup     * pMediaGroup = NULL;
  char                  * pCurrPos = NULL;

  /**/

  pCurrPos = replyData + strlen(replyData);

  if (pBackupSession->tMedia.pFirstGroup != NULL) {
    nFuncStatus = cn30FindMediumByName(pCurrPos, replyLen,
                      &(pBackupSession->tMedia), pBackupSession->tBackupResult.szMediaName, false, &pMediaGroup, pMedium);

    if (nFuncStatus == OK_CN00  && pMedium != NULL) {
      nFuncStatus = cn31_ExecuteOSCommandMedium(pCurrPos, replyLen, *pMedium);
    } else if (nFuncStatus == ERR_MENOTFOUND_CN00) {

      nFuncStatus = cn30FindMediumInGroupByLocation(pCurrPos, replyLen,
                        &(pBackupSession->tMedia), pBackupSession->tBackupResult.szMediaName, pBackupSession->tBackupResult.szLocation,
                        &pMediaGroup, pMedium);

      if (nFuncStatus == OK_CN00 && pMedium != NULL) {
        nFuncStatus = cn31_ExecuteOSCommandMedium(pCurrPos, replyLen, *pMedium);
      } else if (nFuncStatus == ERR_MENOTFOUND_CN00) {
        nFuncStatus = OK_CN00;
      } // end if

    } // end if
  } // end if

  if (nFuncStatus == OK_CN00) {
    *pCurrPos = '\0';
    *replyLen = (int) strlen(replyData);
  } // end if

  return nFuncStatus;
} /* end cn31_BackupOSCommandForMedium */

/* PRIVATE FUNCTION cn31_BackupState
 * ------------------------------------------------------------------
 * purpose: get state of save/restore
 *
 */
static tcn00_Error cn31_BackupState (
        VControlDataT * vcontrol,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax,
        const char    * szCommand,
        bool            bCheck = false)
{
  tcn00_Error           nFuncStatus = OK_CN00;
  tcn003_BackupResult   tResult;
  tsp00_DbNamec         szDBName;
  tcn00_UserNamec       szUserName;
  tsp00_Namec           szPassword;

  DBMSrvKnl_Session     oSession;

  /**/

  // real restore or check only with service kernel?
  if (bCheck) {
    nFuncStatus = cn20XParamGetServiceValues ( szDBName, vcontrol->dbroot, szUserName, szPassword);
    if (nFuncStatus != OK_CN00) {
      cn90AnswerIError(replyData, replyLen, nFuncStatus);
    } // end if
  } else {
    szDBName   = vcontrol->dbname;
    cn50DBMUser       usrDBM    ( szDBName, cn50DBMUser::getUser(szDBName, FIRSTDBM_CN50));
    szUserName = usrDBM.getUserName();
    szPassword = usrDBM.getClearMasterPwd();
  } // end if

  // connect to kernel with user tasks
  if (nFuncStatus == OK_CN00) {
     DBMSrvMsg_Error oError;
    if (!oSession.NormalConnect(szDBName.asCharp(),
                                szUserName.asCharp(),
                                szPassword.asCharp(),
                                oError)) {
      nFuncStatus = cn90AnswerEvent(replyData, replyLen, replyLenMax, oError);
    } // end if
  } // end if

  if (nFuncStatus == OK_CN00) {
    nFuncStatus = cn31_BackupCommand (replyData,
                                      replyLen,
                                      replyLenMax,
                                      oSession,
                                      szCommand,
                                      true,
                                      &tResult);
  } // end if

  /**/

  *replyLen = (int)strlen(replyData);

  return nFuncStatus;
} /* end cn31_BackupState */

/* PRIVATE FUNCTION cn31_RemoveDoublequotes
 * ------------------------------------------------------------------
 * purpose: remove doublequotes from a string
 *
 */
void cn31_RemoveDoublequotes(char * szValue)
{
    size_t nLength=0;

    nLength=strlen(szValue);

    if(1<nLength)   //at least two characters
    {
        if((szValue[0]=='"') && (szValue[nLength-1]=='"'))  //first and last character are double quotes
        {
            for(size_t i=0; i<nLength-2; i++)                      //copy character for character do not use strncpy, as source and destination overlap
                szValue[i]=szValue[i+1];

            szValue[nLength-2] = '\0';
        }
    }
} /* end cn31_RemoveDoublequotes */


/* PRIVATE FUNCTION cn31_FindAutosaveMedia
 * ------------------------------------------------------------------
 * purpose: search the autosave media/medium
 *
 */
tcn00_Error cn31_FindAutosaveMedia (
      char              * replyData,
      int               * replyLen,
      tcn003_Media      * pMedia,
      tcn003_MediaGroup ** pMediaGroupFound,
      tcn003_Medium     ** pMediumFound )
{
  tcn00_Error         nFuncStatus = OK_CN00;

  tcn003_MediaGroup   * pMediaGroup;
  tcn003_Medium       * pMedium;

  /**/

  *pMediaGroupFound = NULL;
  *pMediumFound = NULL;

  pMediaGroup = pMedia->pFirstGroup;
  while ( (pMediaGroup != NULL) && (*pMediaGroupFound == NULL) ) {
    pMedium = pMediaGroup->pFirst;
    while ( (pMedium != NULL) && (*pMediumFound == NULL) ) {
      if (pMedium->szSaveType[0] != '\0') {
        if ( (strcmp(pMedium->szSaveType, ME_SAVETYPE_AUT_CN003) == 0) ) {
          *pMediaGroupFound = pMediaGroup;
          *pMediumFound = pMedium;
        }
      }
      pMedium = pMedium->pNext;
    }
    pMediaGroup = pMediaGroup->pNext;
  }

  if ( (*pMediaGroupFound == NULL) && (*pMediumFound == NULL) ) {
    nFuncStatus = ERR_BANOAUTOMED_CN00;
    cn90AnswerIError(replyData, replyLen, nFuncStatus);
  }

  return nFuncStatus;
} /* cn31_FindAutosaveMedia  */

/*! \brief helper function needed because SDBMSG_DBMSRV_* defines can not be part of a ()?():().
    The function constructs the correct error text for a missing service or db session.

    \param replyData [IN/OUT] the error text
    \param replyLen  [IN/OUT] the length of the error text
    \param replyLenMax [IN]   max length of the error text
    \param isServiceSession [IN] must be true, if service session is missing, false otherwise
    \return classical error number */

void cn31_overruleWithErrorMissingSession(
    DBMSrvMsg_Error & error,
    bool              isServiceSession)
{
    if(isServiceSession)
        error.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_NOSERVICESESSION));
    else
        error.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_NODBSESSION));
}

tcn00_Error cn31_ConstructAnswerMissingSession(
    DBMSrv_Reply & reply,
    bool           isServiceSession)
{
    DBMSrvMsg_Error error;

    cn31_overruleWithErrorMissingSession(error, isServiceSession);
    
    return reply.startWithMessageList(error);
}

/* SPECIFICATION PUBLIC FUNCTIONS
 * ------------------------------------------------------------------
 */

/* PUBLIC FUNCTION cn31BackupReplyAvailable
 * ------------------------------------------------------------------
 * purpose: receives answer from kernel
 *
 */
tcn00_Error cn31BackupReplyAvailable (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax )
{
  tcn00_Error            nFuncStatus    = OK_CN00;
  DBMSrvKnl_Session    * pSession       = NULL;
  tcn003_BackupSession * pBackupSession = NULL;
  DBMSrv_Reply           reply(replyData, replyLen, replyLenMax);

  nFuncStatus = cn31getExistingBackupAndKernelSession(vcontrol, reply, pBackupSession, pSession, ForBackup_cn31);

  /* check reply state */
  if (nFuncStatus == OK_CN00) {
      if(0==pBackupSession->pBackupTool)
      {
        if (pSession->ReplyAvailable()) {
          nFuncStatus = cn90AnswerOK(replyData, replyLen, NULL);
        } else {
          nFuncStatus = cn90AnswerIError(replyData, replyLen, ERR_NOREPLY_CN00);
        } // end if
      }
      else
      {
          pBackupSession->pBackupTool->ChangeReplyBuffer(replyData, replyLen); //the reply buffer might have changed

          int NoReplyAvailable=pBackupSession->pBackupTool->BackupOperationIsRunning();

          if(pBackupSession->pBackupTool->IsBad())
              nFuncStatus=pBackupSession->pBackupTool->GiveLastError();
          else
          {
              if(NoReplyAvailable)
                  nFuncStatus=cn90AnswerIError(replyData, replyLen, ERR_NOREPLY_CN00);
              else
                  nFuncStatus=cn90AnswerOK(replyData, replyLen, NULL);
          }
      }
  } // end if

  return nFuncStatus;
} // end cn31BackupReplyAvailable

/* PUBLIC FUNCTION cn31BackupReplyReceive
 * ------------------------------------------------------------------
 * purpose: receives answer from kernel
 *
 */
tcn00_Error cn31BackupReplyReceive (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax )
{
  tcn00_Error            nFuncStatus    = OK_CN00;
  DBMSrvKnl_Session    * pSession       = NULL;
  tcn003_BackupSession * pBackupSession = NULL;
  DBMSrv_Reply           reply(replyData, replyLen, replyLenMax);

  nFuncStatus = cn31getExistingBackupAndKernelSession(vcontrol, reply, pBackupSession, pSession, ForBackup_cn31);

  /* receive reply */
  if (nFuncStatus == OK_CN00) {
    if(0==pBackupSession->pBackupTool) {
      nFuncStatus = cn31_BackupCommandReceive(replyData, replyLen, replyLenMax, *pSession, &(pBackupSession->tBackupResult), false, pBackupSession);

      // Handling of autoignore and autoreplace
      if (nFuncStatus == OK_CN00 && pBackupSession->tBackupResult.nBackupRC == BA_NEXTMEDIUM_RC_CN003) {
        tcn003_Medium * pMedium;

        // execute os command
        nFuncStatus = cn31_BackupOSCommandForMedium(replyData, replyLen, pBackupSession, &pMedium);

        // execute autoreplace
        if ( (nFuncStatus                  == OK_CN00) &&
             (pMedium                      != NULL   ) &&
             (pMedium->szOSCommand         != NULL   ) &&
             (strlen(pMedium->szOSCommand) >  0      )    ) {

          char szBuffer[1024];
          const char * tmpArgs = command->args;

          sp77sprintf(szBuffer, 1024, "%s %s", pBackupSession->tBackupResult.szMediaName,
                                                 pBackupSession->tBackupResult.szLocation);
          command->args = &szBuffer[0];

          if (pBackupSession->nSessionType == BA_SESSION_TYPE_SAVE_CN003) {
            nFuncStatus = cn31BackupSaveReplace(vcontrol, command, replyData, replyLen, replyLenMax);
          } else if (pBackupSession->nSessionType == BA_SESSION_TYPE_RESTORE_CN003) {
            nFuncStatus = cn31BackupRestoreReplace(vcontrol, command, replyData, replyLen, replyLenMax);
          } // end if

          command->args = tmpArgs;

        } else if (pBackupSession->bAutoignore) {

          // autoignore (if required) when oscommand failed
          if (pBackupSession->nSessionType == BA_SESSION_TYPE_SAVE_CN003) {
            nFuncStatus = cn31BackupSaveIgnore(vcontrol, command, replyData, replyLen, replyLenMax);
          } else if (pBackupSession->nSessionType == BA_SESSION_TYPE_RESTORE_CN003) {
            nFuncStatus = cn31BackupRestoreIgnore(vcontrol, command, replyData, replyLen, replyLenMax);
          } // end if

        } // end if

      } // end if

    } else {
      pBackupSession->pBackupTool->ChangeReplyBuffer(replyData, replyLen); //the reply buffer might have changed

      pBackupSession->pBackupTool->EndBackupOperation();    // use the backup tool

      nFuncStatus=pBackupSession->pBackupTool->GiveLastError(); // get the results form the backup/restore
      pBackupSession->tBackupResult.nBackupRC=pBackupSession->pBackupTool->GiveSQLResult();
    } // end if
  } // end if

  /* execute os command */
//  if (nFuncStatus == OK_CN00 && !pBackupSession->bCancelActive && !pBackupSession->bAutoreplace) {
//    nFuncStatus = cn31_BackupOSCommandForMedium(replyData, replyLen, pBackupSession);
//  } // end if

  if ((nFuncStatus == OK_CN00) && (pBackupSession->tBackupResult.nBackupRC == 0)) {
    cn31_DBAActionComplete(nFuncStatus);
  } // end if

  /* free session */
  if (nFuncStatus == OK_CN00 && pBackupSession->bCancelActive) {
    cn31FreeBackupSession(vcontrol);
    cn31_DBAActionComplete(nFuncStatus);
  } // end if

  /**/

  *replyLen = (int)strlen(replyData);

  return nFuncStatus;
} // end cn31BackupReplyReceive

/* PUBLIC FUNCTION cn31BackupCommandRequest
 * ------------------------------------------------------------------
 * purpose: request low level save/restore command
 *
 */
tcn00_Error cn31BackupCommandRequest (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax )
{
  tcn00_Error            nFuncStatus    = OK_CN00;
  DBMSrvKnl_Session    * pSession       = NULL;
  tcn003_BackupSession * pBackupSession = NULL;
  DBMSrv_Reply           reply(replyData, replyLen, replyLenMax);

    //get database session (formerly known as utility session)
    pSession = DBMSrvKnl_GlobalSessions::GetSession(DBMSrvKnl_GlobalSessions::SessionDatabase);

    if(0==pSession)
        nFuncStatus = cn90AnswerEvent(replyData, replyLen, replyLenMax, DBMSrvMsg_Error(SDBMSG_DBMSRV_NODBSESSION));
    else
        pSession->allowReconnectAtNextExecute(DBMSrvKnl_Session::backupRestoreCheck);

  /* get backup session */
  if (nFuncStatus == OK_CN00) {
    nFuncStatus = cn31BackupSession(vcontrol, reply, &pBackupSession, ForBackup_cn31);

    /* reset backup session*/
    if (nFuncStatus == OK_CN00) {
      cn31FreeBackupSession(vcontrol);
      pBackupSession = NULL;
    } else {
      nFuncStatus = OK_CN00;
    }

    if (nFuncStatus == OK_CN00) {
      nFuncStatus = cn31_AllocBackupSession(vcontrol, replyData, replyLen, &pBackupSession);
    }
  }

  if (nFuncStatus == OK_CN00) {

    pBackupSession->nSessionType = BA_SESSION_TYPE_COMMAND_CN003;

    if (cn90GetToken(command->args, pBackupSession->szCommand, 1, BA_COMMAND_LENGTH_MXCN003)) {
      cn31_RemoveDoublequotes(pBackupSession->szCommand);
    } else {
      nFuncStatus = ERR_PARAM_CN00;
      cn90AnswerIError(replyData, replyLen, nFuncStatus);
    }
  }

  /* execute command */
  if (nFuncStatus == OK_CN00) {
    nFuncStatus = cn31_BackupCommandRequest(replyData, replyLen, replyLenMax, *pSession,
                                            pBackupSession->szCommand, &(pBackupSession->tBackupResult));
  }

  *replyLen = (int)strlen(replyData);

    if(0!=pSession)
        pSession->disallowReconnectAtNextExecute();

  return nFuncStatus;
}

/* PUBLIC FUNCTION cn31BackupCommand
 * ------------------------------------------------------------------
 * purpose: low level save/restore command
 *
 */
tcn00_Error cn31BackupCommand (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax )
{
  tcn00_Error           nFuncStatus = OK_CN00;

  nFuncStatus = cn31BackupCommandRequest (vcontrol,
                                          command,
                                          replyData,
                                          replyLen,
                                          replyLenMax);

  if (nFuncStatus == OK_CN00) {
    nFuncStatus = cn31BackupReplyReceive (vcontrol,
                                          command,
                                          replyData,
                                          replyLen,
                                          replyLenMax);
  } // end if

  return nFuncStatus;
}

/* PUBLIC FUNCTION cn31BackupSaveRequest
 * ------------------------------------------------------------------
 * purpose: requests save data/pages/log to specified medium
 *
 */
tcn00_Error cn31BackupSaveRequest (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax )
{
  tcn00_Error           nFuncStatus = OK_CN00;
  DBMSrvKnl_Session     * pSession  = NULL;
  tcn003_BackupSession  * pBackupSession = NULL;
  tcn003_MediaGroup     * pMediaGroup = NULL;
  tcn003_Medium         * pMedium     = NULL;
  char                  szSaveCommand[BA_COMMAND_LENGTH_MXCN003];
  bool                  bRecovery = true;
  char                  szSaveFor[32] = "";
  char                  szParam[255]  = "";
  int                   nParam = 1;
  int                   nToken = 0;
  DBMSrv_Reply          reply(replyData, replyLen, replyLenMax);

    // get database session (formerly known as utility session)
    pSession = DBMSrvKnl_GlobalSessions::GetSession(DBMSrvKnl_GlobalSessions::SessionDatabase);
  
    if(0==pSession)
        nFuncStatus = cn90AnswerEvent(replyData, replyLen, replyLenMax, DBMSrvMsg_Error(SDBMSG_DBMSRV_NODBSESSION));
    else
        pSession->allowReconnectAtNextExecute(DBMSrvKnl_Session::backupRestoreCheck);

  /* get backup session */
  if (nFuncStatus == OK_CN00) {
    nFuncStatus = cn31BackupSession(vcontrol, reply, &pBackupSession, ForBackup_cn31);

    /* reset backup session*/
    if (nFuncStatus == OK_CN00) {
      cn31FreeBackupSession(vcontrol);
      pBackupSession = NULL;
    } else {
      nFuncStatus = OK_CN00;
    }

    if (nFuncStatus == OK_CN00) {
      nFuncStatus = cn31_AllocBackupSession(vcontrol, replyData, replyLen, &pBackupSession);
    }
  }

  /* get media name */
  if (nFuncStatus == OK_CN00) {

    pBackupSession->nSessionType = BA_SESSION_TYPE_SAVE_CN003;

    if (cn90GetToken(command->args, pBackupSession->szMediaName, 1, ME_NAME_LENGTH_MXCN003)) {
      cn31_RemoveDoublequotes(pBackupSession->szMediaName);
    } else {
      nFuncStatus = ERR_PARAM_CN00;
      cn90AnswerIError(replyData, replyLen, nFuncStatus);
    }
  }

  /* get save for option (RECOVERY/MIGRATION) */
  /* get type to save (DATA/PAGES/LOG) */
  /* get autoignore flag (AUTOIGNORE) */

  // read next tokens
  nToken = 2;
  while ((nFuncStatus == OK_CN00) && cn90GetToken(command->args, szParam, nToken, 255)) {

    if (cn31_CheckBackupType(replyData, replyLen, szParam) == OK_CN00) {
      SAPDB_strcpy(pBackupSession->szBackupType, szParam);
    } else if (cn90Stricmp(szParam, BA_KEY_MIGRATION_CN003) == 0) {
      bRecovery = false;
    } else if (cn90Stricmp(szParam, BA_KEY_RECOVERY_CN003) == 0) {
      bRecovery = true;
    } else if (cn90Stricmp(szParam, BA_KEY_AUTOIGNORE_CN003) == 0) {
      pBackupSession->bAutoignore = true;
    } else {
      nFuncStatus = cn90AnswerIError(replyData, replyLen, ERR_PARAM_CN00);
    } // end if

    ++nToken;
  } // end while

  /**/

  /* read the media definitions */
  if (nFuncStatus == OK_CN00) {
    nFuncStatus = cn30ReadMedia(vcontrol->dbname,  replyData, replyLen, replyLenMax, &(pBackupSession->tMedia));
  }

  /* find medium/mediagroup in media definition and build command */
  if (nFuncStatus == OK_CN00) {

    nFuncStatus = cn30FindMediumByName(replyData, replyLen,
                    &(pBackupSession->tMedia), pBackupSession->szMediaName, true,
                    &pMediaGroup, &pMedium);

    if (nFuncStatus == OK_CN00) {
      nFuncStatus = cn31_CheckBackupTypeMedium(replyData, replyLen, pBackupSession->szBackupType, pMedium);
    }

    if (nFuncStatus == OK_CN00) {

      if (cn90Stricmp(pBackupSession->szBackupType, BA_KEY_LOG_CN003) != 0) {
        nFuncStatus = cn31_CheckOverwriteMedium(replyData, replyLen, pMedium);
      }
      if (nFuncStatus == OK_CN00) {
        nFuncStatus = cn31_BuildSaveCommand(pBackupSession->szBackupType, pMedium, bRecovery, szSaveCommand);
      }

    } else {

      if (nFuncStatus == ERR_MENOTFOUND_CN00) {
        nFuncStatus = OK_CN00;

        nFuncStatus = cn30FindMediaGroupByName(replyData, replyLen,
                        &(pBackupSession->tMedia), pBackupSession->szMediaName,
                        &pMediaGroup);

        if (nFuncStatus == OK_CN00) {
          nFuncStatus = cn31_CheckBackupTypeMediaGroup(replyData, replyLen, pBackupSession->szBackupType, pMediaGroup);
        }

        if (nFuncStatus == OK_CN00) {

          if (cn90Stricmp(pBackupSession->szBackupType, BA_KEY_LOG_CN003) != 0) {
            nFuncStatus = cn31_CheckOverwriteMediaGroup(replyData, replyLen, pMediaGroup);
          }
          if (nFuncStatus == OK_CN00) {
            nFuncStatus = cn31_BuildParallelSaveCommand(pBackupSession->szBackupType, pMediaGroup, bRecovery, szSaveCommand);
          }

        }
      }
    }

    cn31_DBAActionStart(vcontrol->dbname, pBackupSession, bRecovery);

  }

    if(OK_CN00==nFuncStatus)
    {
        // at first set timeout off for this utility session
        DBMSrvMsg_Error oError;
        if(DBMSrvKnl_State::StateOnline==DBMSrvKnl_State::State(vcontrol->dbname))
            pSession->Execute(BA_KEY_TIMEOUTOFF_CN003, oError);

        if(DBMSrvBMedia_MediaGroup::NeedsBackupTool(pMedium,pMediaGroup))   // check medium properties, if backup tool is needed
        {
            nFuncStatus=TheDBMExternalBackupControllerFactory_cn35b.ConstructADBMExternalBackupController(pBackupSession->pBackupTool,
                                                                                                          replyData,
                                                                                                          replyLen,
                                                                                                          tcn35d_BackupInfo::ABackup,
                                                                                                          pBackupSession->szBackupType,
                                                                                                          bRecovery,
                                                                                                          pSession,
                                                                                                          szSaveCommand,
                                                                                                          pMedium,
                                                                                                          pMediaGroup,
                                                                                                          (char *)vcontrol->dbname);

            if(OK_CN00==nFuncStatus)   //BackupController was constructed successfully
            {
                // use the backup tool
                pBackupSession->pBackupTool->BeginBackupOperation();

                // get the results form the backup
                nFuncStatus=pBackupSession->pBackupTool->GiveLastError();
                pBackupSession->tBackupResult.nBackupRC=pBackupSession->pBackupTool->GiveSQLResult();

                if(0==pBackupSession->tBackupResult.nBackupRC && OK_CN00==nFuncStatus)
                    nFuncStatus=cn90AnswerOK(replyData, replyLen, NULL);
            }
        }
        else
            nFuncStatus=cn31_BackupCommandRequest(replyData, replyLen, replyLenMax, *pSession, szSaveCommand, &(pBackupSession->tBackupResult));
    }

    if (nFuncStatus != OK_CN00) {
      cn31_DBAActionComplete(nFuncStatus);
    } // end if

  *replyLen = (int)strlen(replyData);

  /**/
    if(0!=pSession)
        pSession->disallowReconnectAtNextExecute();

  return nFuncStatus;
} /* end cn31BackupSaveRequest */

class ReplyBuffer
{
  public:
    ReplyBuffer(char * ReplyData, int * ReplyLen, int ReplyLenMax)
        :replyData(ReplyData), replyLen(ReplyLen), replyLenMax(ReplyLenMax)
    {
    }

    char * ReplyData() {return replyData;}
    int  * ReplyLen() {return replyLen;}
    int    ReplyLenMax() const {return replyLenMax;}

    const tcn00_Error & SetToIError(const tcn00_Error & err) {cn90AnswerIError(replyData, replyLen, err); return err;}

  private:
    char * replyData;
    int  * replyLen;
    int    replyLenMax;
};

/* PRIVATE FUNCTION cn31_BackupStageRequest
 * ------------------------------------------------------------------
 * purpose: starts save of staged log backups to specified medium
 *
 */
tcn00_Error cn31_BackupStageRequest(
      VControlDataT * vcontrol,
      CommandT      * command,
      ReplyBuffer   & TheReplyBuffer)
{
    tcn00_Error rc=OK_CN00;

    tcn003_BackupSession * pBackupSession=0;
    tcn003_MediaGroup    * pMediaGroup=0;
    tcn003_Medium        * pMedium=0;
    char                   szParam[255]="";
    int                    nToken = 0;
    char                   StageName[256+1]="";
    tcn003_Medium        * StageMedium=0;
    tcn003_MediaGroup    * StageMediaGroup=0;
    char                   StageBackupType[BA_TYPE_LENGTH_MXCN003]="";
    int                    WithVerify=1;
    int                    RemoveStageFiles=1;
    const char           * StartOfFileNumberList=0,
                         * EndOfFileNumberList=0;
    DBMSrv_Reply           reply(TheReplyBuffer.ReplyData(), TheReplyBuffer.ReplyLen(), TheReplyBuffer.ReplyLenMax());

    // get backup session
    if(OK_CN00==rc)
    {
        rc=cn31BackupStageSession(vcontrol, reply, &pBackupSession);

        // reset backup session
        if(OK_CN00==rc)
        {
            cn31FreeBackupStageSession(vcontrol);
            pBackupSession=0;
        }

        rc=cn31_AllocBackupStageSession(vcontrol, TheReplyBuffer.ReplyData(), TheReplyBuffer.ReplyLen(), &pBackupSession);
    }

    // get media name and stage name
    if(OK_CN00==rc)
    {
        pBackupSession->nSessionType=BA_SESSION_TYPE_SAVE_CN003;

        if(cn90GetToken(command->args, pBackupSession->szMediaName, 1, ME_NAME_LENGTH_MXCN003) &&
           cn90GetToken(command->args, StageName, 2, 256))
        {
            cn31_RemoveDoublequotes(pBackupSession->szMediaName);
            cn31_RemoveDoublequotes(StageName);
        }
        else
            rc=TheReplyBuffer.SetToIError(ERR_PARAM_CN00);
    }

    // get verify/noverify option (if it is given)
    // get remove/keep option (if it is given)

    nToken=3;   // medianame and stage name are token 1 and 2, skip them now

    while(OK_CN00==rc && cn90GetToken(command->args, szParam, nToken, 255))
    {
        if(0==cn90Stricmp(szParam, BA_KEY_VERIFY_CN003))
            WithVerify=1;
        else
            if(0==cn90Stricmp(szParam, BA_KEY_NOVERIFY_CN003))
                WithVerify=0;
            else
                if(0==cn90Stricmp(szParam, BA_KEY_REMOVE_CN003))
                    RemoveStageFiles=1;
                else
                    if(0==cn90Stricmp(szParam, BA_KEY_KEEP_CN003))
                        RemoveStageFiles=0;
                    else
                        if(0==cn90Stricmp(szParam, BA_KEY_FILENUMBERLIST_CN003) ||
                           0==cn90Stricmp(szParam, BA_KEY_FNL_CN003))
                        {
                            if(cn90GetStartAndEndOfToken(command->args, StartOfFileNumberList, EndOfFileNumberList, nToken+1))
                                nToken++;
                            else
                                rc=TheReplyBuffer.SetToIError(ERR_PARAM_CN00);
                        }
                        else
                            rc=TheReplyBuffer.SetToIError(ERR_PARAM_CN00);

        nToken++;
    }

    // read all media definitions from the media definitions file
    if(OK_CN00==rc)
        rc=cn30ReadMedia(vcontrol->dbname, TheReplyBuffer.ReplyData(), TheReplyBuffer.ReplyLen(), TheReplyBuffer.ReplyLenMax(), &(pBackupSession->tMedia));
  
    // find medium/mediagroup in media definition and build command
    if(OK_CN00==rc)
    {
        rc=cn30FindMediumByName(TheReplyBuffer.ReplyData(), TheReplyBuffer.ReplyLen(),
                                &(pBackupSession->tMedia), pBackupSession->szMediaName, true,
                                &pMediaGroup, &pMedium);

        if(OK_CN00==rc)
            rc=cn31_CheckBackupTypeMedium(TheReplyBuffer.ReplyData(), TheReplyBuffer.ReplyLen(), pBackupSession->szBackupType, pMedium);
        else
            if(ERR_MENOTFOUND_CN00==rc)
            {
                rc=cn30FindMediaGroupByName(TheReplyBuffer.ReplyData(), TheReplyBuffer.ReplyLen(),
                                            &(pBackupSession->tMedia), pBackupSession->szMediaName,
                                            &pMediaGroup);

                if(OK_CN00==rc)
                    rc=cn31_CheckBackupTypeMediaGroup(TheReplyBuffer.ReplyData(), TheReplyBuffer.ReplyLen(), pBackupSession->szBackupType, pMediaGroup);
            }

        // find stage medium
        if(OK_CN00==rc)
        {
            rc=cn30FindMediumByName(TheReplyBuffer.ReplyData(), TheReplyBuffer.ReplyLen(), &(pBackupSession->tMedia), StageName, true, &StageMediaGroup, &StageMedium);

            if(OK_CN00==rc)
                rc=cn31_CheckBackupTypeMedium(TheReplyBuffer.ReplyData(), TheReplyBuffer.ReplyLen(), StageBackupType, StageMedium);
            else
                if(ERR_MENOTFOUND_CN00==rc)
                {
                    rc=cn30FindMediaGroupByName(TheReplyBuffer.ReplyData(), TheReplyBuffer.ReplyLen(), &(pBackupSession->tMedia), StageName, &StageMediaGroup);

                    if(OK_CN00==rc)
                        rc=cn31_CheckBackupTypeMediaGroup(TheReplyBuffer.ReplyData(), TheReplyBuffer.ReplyLen(), StageBackupType, StageMediaGroup);
                }
        }
    }

    tsp00_Timestampc szTimeStamp;

    cn90Timestamp(szTimeStamp);
    tcn53_DBAAction::start(vcontrol->dbname, szTimeStamp, DBAOP_SAVESTAGE_CN53, DBA_BAOBJ_LOG_CN53);

    if(OK_CN00==rc &&
       (0!=cn90Stricmp(pBackupSession->szBackupType, ME_SAVETYPE_LOG_CN003) ||
        0!=cn90Stricmp(StageBackupType, ME_SAVETYPE_LOG_CN003)                ))
    {
        rc=cn90AnswerIError(TheReplyBuffer.ReplyData(), TheReplyBuffer.ReplyLen(), ERR_BAWRONGSAVETYPE_CN00);
    }

    if(OK_CN00==rc)
    {
        if(DBMSrvBMedia_MediaGroup::NeedsBackupTool(pMedium, pMediaGroup))  // check stage medium properties, if backup tool is needed, as we currently allow staging only with external backup tools
        {
            rc=TheDBMStageBackupControllerFactory_cn35c.ConstructADBMStageBackupController(pBackupSession->StageController,
                                                                                           TheReplyBuffer.ReplyData(),
                                                                                           TheReplyBuffer.ReplyLen(),
                                                                                           pMedium,
                                                                                           pMediaGroup,
                                                                                           StageMedium,
                                                                                           StageMediaGroup,
                                                                                           (char *)vcontrol->dbname,
                                                                                           WithVerify,
                                                                                           RemoveStageFiles,
                                                                                           StartOfFileNumberList,
                                                                                           EndOfFileNumberList-StartOfFileNumberList);

            if(OK_CN00==rc)
            {
                // use the backup tool
                pBackupSession->StageController->BeginBackupOperation();

                // get the results form the backup
                rc=pBackupSession->StageController->GiveLastError();
                pBackupSession->tBackupResult.nBackupRC=pBackupSession->StageController->GiveSQLResult();
            }
        }
        else
            rc=cn90AnswerIError(TheReplyBuffer.ReplyData(), TheReplyBuffer.ReplyLen(), ERR_BAWRONGMEDIUM_CN00);
    }

    if(OK_CN00!=rc)
    {
        cn90Timestamp(szTimeStamp);
        tcn53_DBAAction::finish(szTimeStamp, false);   //not successful
    }

    (*TheReplyBuffer.ReplyLen())=(int)strlen(TheReplyBuffer.ReplyData()); //it should hold already

    return rc;
} /* end cn31_BackupStageRequest */

/* PRIVATE FUNCTION cn31_BackupStageReplyReceive
 * ------------------------------------------------------------------
 * purpose: ends save of staged log backups to specified medium
 *
 */
tcn00_Error cn31_BackupStageReplyReceive(
      VControlDataT * vcontrol,
      CommandT      * command,
      ReplyBuffer   & TheReplyBuffer)
{
    tcn00_Error            rc=OK_CN00;
    tcn003_BackupSession * pBackupSession=0;
    DBMSrv_Reply           reply(TheReplyBuffer.ReplyData(), TheReplyBuffer.ReplyLen(), TheReplyBuffer.ReplyLenMax());

    // get backup session
    rc=cn31BackupStageSession(vcontrol, reply, &pBackupSession);

    if(OK_CN00==rc)
    {
        pBackupSession->StageController->EndBackupOperation();    // use the backup tool

        rc=pBackupSession->StageController->GiveLastError(); // get the results form the backup/restore
        pBackupSession->tBackupResult.nBackupRC=pBackupSession->StageController->GiveSQLResult();
    }

    tsp00_Timestampc szTimeStamp;
    
    cn90Timestamp(szTimeStamp);
    tcn53_DBAAction::finish(szTimeStamp, (OK_CN00==rc));

    // free session
    if(OK_CN00==rc && pBackupSession->bCancelActive)
        cn31FreeBackupStageSession(vcontrol);

    (*TheReplyBuffer.ReplyLen())=(int)strlen(TheReplyBuffer.ReplyData()); //it should hold already

    return rc;
} /* end cn31_BackupStageReplyReceive */

/* PUBLIC FUNCTION cn31BackupStage
 * ------------------------------------------------------------------
 * purpose: executes save of staged log backups to specified medium
 *
 */
tcn00_Error cn31BackupStage (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax )
{
    tcn00_Error rc;
    ReplyBuffer TheReplyBuffer(replyData, replyLen, replyLenMax);

    rc=cn31_BackupStageRequest(vcontrol, command, TheReplyBuffer);

    if(OK_CN00==rc)
        rc=cn31_BackupStageReplyReceive(vcontrol, command, TheReplyBuffer);

    return rc;
} /* end cn31BackupStage */

/* PRIVATE FUNCTION cn31_BackupStageRepeatRequest
 * ------------------------------------------------------------------
 * purpose: starts to repeat a previous save of staged log backups to specified medium
 *
 */
tcn00_Error cn31_BackupStageRepeatRequest(
      VControlDataT * vcontrol,
      CommandT      * command,
      ReplyBuffer   & TheReplyBuffer)
{
    tcn00_Error rc=OK_CN00;

    tcn003_BackupSession  * pBackupSession=0;
    tcn003_MediaGroup     * pMediaGroup=0;
    tcn003_Medium         * pMedium=0;
    char                    szParam[255]  = "";
    int                     nToken = 0;
    tcn003_Medium         * StageMedium=0;
    tcn003_MediaGroup     * StageMediaGroup=0;
    char                    StageBackupType[BA_TYPE_LENGTH_MXCN003]="";
    int                     WithVerify=1;
    int                     RemoveStageFiles=1;
    DBMSrv_Reply            reply(TheReplyBuffer.ReplyData(), TheReplyBuffer.ReplyLen(), TheReplyBuffer.ReplyLenMax());

    // get backup stage session
    rc=cn31BackupStageSession(vcontrol, reply, &pBackupSession);

    tsp00_Timestampc szTimeStamp;

    cn90Timestamp(szTimeStamp);
    tcn53_DBAAction::start(vcontrol->dbname, szTimeStamp, DBAOP_SAVESTAGE_CN53, DBA_BAOBJ_LOG_CN53);

    //check if there was a stage backup, that we can repeat now
    if(0==pBackupSession || 0==pBackupSession->StageController)
        rc=TheReplyBuffer.SetToIError(ERR_NOPREVSTAGEBACKUP_CN00);

    // get media name and stage name
    if(OK_CN00==rc)
    {
        pBackupSession->nSessionType=BA_SESSION_TYPE_SAVE_CN003;

        if(cn90GetToken(command->args, pBackupSession->szMediaName, 1, ME_NAME_LENGTH_MXCN003))
            cn31_RemoveDoublequotes(pBackupSession->szMediaName);
        else
            rc=TheReplyBuffer.SetToIError(ERR_PARAM_CN00);
    }

    // get verify/noverify option (if it is given)
    // get remove/keep option (if it is given)

    nToken=2;   // medianame is token 1, skip it now

    while(OK_CN00==rc && cn90GetToken(command->args, szParam, nToken, 255))
    {
        if(0==cn90Stricmp(szParam, BA_KEY_VERIFY_CN003))
            WithVerify=1;
        else
            if(0==cn90Stricmp(szParam, BA_KEY_NOVERIFY_CN003))
                WithVerify=0;
            else
                if(0==cn90Stricmp(szParam, BA_KEY_REMOVE_CN003))
                    RemoveStageFiles=1;
                else
                    if(0==cn90Stricmp(szParam, BA_KEY_KEEP_CN003))
                        RemoveStageFiles=0;
                    else
                        rc=TheReplyBuffer.SetToIError(ERR_PARAM_CN00);

        ++nToken;
    }

    // read all media definitions from the media definitions file
    if(OK_CN00==rc)
        rc=cn30ReadMedia(vcontrol->dbname, TheReplyBuffer.ReplyData(), TheReplyBuffer.ReplyLen(), TheReplyBuffer.ReplyLenMax(), &(pBackupSession->tMedia));
  
    // find medium/mediagroup in media definition and build command
    if(OK_CN00==rc)
    {
        rc=cn30FindMediumByName(TheReplyBuffer.ReplyData(),
                                TheReplyBuffer.ReplyLen(),
                                &(pBackupSession->tMedia),
                                pBackupSession->szMediaName,
                                true,
                                &pMediaGroup, &pMedium);

        if(OK_CN00==rc)
            rc=cn31_CheckBackupTypeMedium(TheReplyBuffer.ReplyData(), TheReplyBuffer.ReplyLen(), pBackupSession->szBackupType, pMedium);
        else
            if(ERR_MENOTFOUND_CN00==rc)
            {
                rc=cn30FindMediaGroupByName(TheReplyBuffer.ReplyData(), TheReplyBuffer.ReplyLen(),
                                            &(pBackupSession->tMedia), pBackupSession->szMediaName,
                                            &pMediaGroup);

                if(OK_CN00==rc)
                    rc=cn31_CheckBackupTypeMediaGroup(TheReplyBuffer.ReplyData(), TheReplyBuffer.ReplyLen(), pBackupSession->szBackupType, pMediaGroup);
            }
    }

    if(OK_CN00==rc &&
       (0!=cn90Stricmp(pBackupSession->szBackupType, ME_SAVETYPE_LOG_CN003)))
    {
        rc=cn90AnswerIError(TheReplyBuffer.ReplyData(), TheReplyBuffer.ReplyLen(), ERR_BAWRONGSAVETYPE_CN00);
    }

    if(OK_CN00==rc)
    {
        if(DBMSrvBMedia_MediaGroup::NeedsBackupTool(pMedium, pMediaGroup))  // check medium properties, if backup tool is needed, as we currently allow staging only with external backup tools
        {
            rc=pBackupSession->StageController->RecycleForRepeat(TheReplyBuffer.ReplyData(),
                                                                 TheReplyBuffer.ReplyLen(),
                                                                 pMedium,
                                                                 pMediaGroup,
                                                                 WithVerify,
                                                                 RemoveStageFiles);

            if(OK_CN00==rc)
            {
                // use the backup tool
                pBackupSession->StageController->BeginBackupOperation();

                // get the results form the backup
                rc=pBackupSession->StageController->GiveLastError();
                pBackupSession->tBackupResult.nBackupRC=pBackupSession->StageController->GiveSQLResult();
            }
        }
        else
            rc=cn90AnswerIError(TheReplyBuffer.ReplyData(), TheReplyBuffer.ReplyLen(), ERR_BAWRONGMEDIUM_CN00);
    }

    if(OK_CN00!=rc)
    {
        cn90Timestamp(szTimeStamp);
        tcn53_DBAAction::finish(szTimeStamp, false);   //not successful
    }

    return rc;
} /* end cn31_BackupStageRepeatRequest */

/* PUBLIC FUNCTION cn31BackupStageRepeat
 * ------------------------------------------------------------------
 * purpose: repeats a previously save of staged log backups to
 *          specified medium
 *
 */
tcn00_Error cn31BackupStageRepeat (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax )
{
    tcn00_Error rc;
    ReplyBuffer TheReplyBuffer(replyData, replyLen, replyLenMax);

    rc=cn31_BackupStageRepeatRequest(vcontrol, command, TheReplyBuffer);

    if(OK_CN00==rc)
        rc=cn31_BackupStageReplyReceive(vcontrol, command, TheReplyBuffer);

    return rc;
} /* end cn31BackupStageRepeat */

/* PUBLIC FUNCTION cn31BackupSave
 * ------------------------------------------------------------------
 * purpose: executes save data/pages/log to specified medium
 *
 */
tcn00_Error cn31BackupSave (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax )
{
  tcn00_Error           nFuncStatus = OK_CN00;

  nFuncStatus = cn31BackupSaveRequest (vcontrol,
                                       command,
                                       replyData,
                                       replyLen,
                                       replyLenMax);

  if (nFuncStatus == OK_CN00) {
    nFuncStatus = cn31BackupReplyReceive (vcontrol,
                                          command,
                                          replyData,
                                          replyLen,
                                          replyLenMax);
  } // end if

  return nFuncStatus;
} // end cn31BackupSave


/* PUBLIC FUNCTION cn31BackupSaveReplaceRequest
 * ------------------------------------------------------------------
 * purpose: requests continue save data/pages/log after a replace
 *
 */
tcn00_Error cn31BackupSaveReplaceRequest (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax )
{
  tcn00_Error           nFuncStatus = OK_CN00;
  DBMSrvKnl_Session     * pSession  = NULL;
  tcn003_BackupSession  * pBackupSession = NULL;
  tcn003_MediaGroup     * pMediaGroup = NULL;
  tcn003_Medium         * pMedium     = NULL;
  char                  szSaveCommand[BA_COMMAND_LENGTH_MXCN003];
  char                  szLocation[ME_LOCATION_LENGTH_MXCN003];   /* new location 19990519 */
  DBMSrv_Reply          reply(replyData, replyLen, replyLenMax);

    nFuncStatus = cn31getExistingBackupAndKernelSession(vcontrol, reply, pBackupSession, pSession, ForBackup_cn31);

  /* get media name */
  if (nFuncStatus == OK_CN00) {
    if (cn90GetToken(command->args, pBackupSession->szMediaName, 1, ME_NAME_LENGTH_MXCN003)) {
      cn31_RemoveDoublequotes(pBackupSession->szMediaName);
    } else {
      nFuncStatus = ERR_PARAM_CN00;
      cn90AnswerIError(replyData, replyLen, nFuncStatus);
    }
  }

  /* get new location for medium 19990519 */
  if (nFuncStatus == OK_CN00) {
    szLocation[0] = '\0';
    if (cn90GetToken(command->args, szLocation, 2, ME_LOCATION_LENGTH_MXCN003)) {
      cn31_RemoveDoublequotes(szLocation);
    }
  }

  /**/

    /* read the media definitions for new media 19990518 */
    if(OK_CN00==nFuncStatus)
    {
        cn30FreeMedia(&(pBackupSession->tMedia));
        nFuncStatus = cn30ReadMedia(vcontrol->dbname, replyData, replyLen, replyLenMax, &(pBackupSession->tMedia));
    }

  /**/

  /* find medium in media definition and build command */
  if (nFuncStatus == OK_CN00) {

    nFuncStatus = cn30FindMediumByName(replyData, replyLen,
                    &(pBackupSession->tMedia), pBackupSession->szMediaName, false,
                    &pMediaGroup, &pMedium);

    if (nFuncStatus == OK_CN00) {

      /* set new location for medíum 19990519 */
      if (szLocation[0] != '\0') {
      SAPDB_strcpy(pMedium->szLocation, szLocation);
    }

      if (cn90Stricmp(pBackupSession->szBackupType, BA_KEY_LOG_CN003) != 0) {
        nFuncStatus = cn31_CheckOverwriteMedium(replyData, replyLen, pMedium);
      }
      if (nFuncStatus == OK_CN00) {
        nFuncStatus = cn31_BuildSaveReplaceCommand(pBackupSession->szBackupType, pMedium, szSaveCommand);
      }
    }
  }

  /* execute command */
  if (nFuncStatus == OK_CN00) {
    nFuncStatus = cn31_BackupCommandRequest(replyData, replyLen, replyLenMax, *pSession, szSaveCommand, &(pBackupSession->tBackupResult));
  }

  /**/
  if (nFuncStatus != OK_CN00) {
    cn31_DBAActionComplete(nFuncStatus);
  } // end if

  *replyLen = (int)strlen(replyData);

  return nFuncStatus;
} /* end cn31BackupSaveReplaceRequest */

/* PUBLIC FUNCTION cn31BackupSaveReplace
 * ------------------------------------------------------------------
 * purpose: executes continue save data/pages/log after a replace
 *
 */
tcn00_Error cn31BackupSaveReplace (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax )
{
  tcn00_Error           nFuncStatus = OK_CN00;

  nFuncStatus = cn31BackupSaveReplaceRequest (vcontrol,
                                              command,
                                              replyData,
                                              replyLen,
                                              replyLenMax);

  if (nFuncStatus == OK_CN00) {
    nFuncStatus = cn31BackupReplyReceive (vcontrol,
                                          command,
                                          replyData,
                                          replyLen,
                                          replyLenMax);
  } // end if

  return nFuncStatus;
} // end cn31BackupSaveReplace

/* PUBLIC FUNCTION cn31BackupSaveIgnoreRequest
 * ------------------------------------------------------------------
 * purpose: requests ignore replace
 *
 */
tcn00_Error cn31BackupSaveIgnoreRequest (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax )
{
  tcn00_Error            nFuncStatus = OK_CN00;
  DBMSrvKnl_Session    * pSession  = NULL;
  tcn003_BackupSession * pBackupSession = NULL;
  char                   szSaveCommand[BA_COMMAND_LENGTH_MXCN003];
  DBMSrv_Reply           reply(replyData, replyLen, replyLenMax); 

    nFuncStatus=cn31getExistingBackupAndKernelSession(vcontrol, reply, pBackupSession, pSession, ForBackup_cn31);

  // get rid of any previously used backup tool object, it is causing error if still present during "backup_cancel"
  if(OK_CN00==nFuncStatus && 0!=pBackupSession->pBackupTool)  //If we already used a backup tool, forget it.
  {
      delete pBackupSession->pBackupTool;
      pBackupSession->pBackupTool=0;
  }

  /* build command */
  if (nFuncStatus == OK_CN00) {
    nFuncStatus = cn31_BuildSaveIgnoreCommand(pBackupSession->szBackupType,  szSaveCommand);
  }

  /* execute command */
  if (nFuncStatus == OK_CN00) {
    nFuncStatus = cn31_BackupCommandRequest(replyData, replyLen, replyLenMax, *pSession, szSaveCommand, &(pBackupSession->tBackupResult));
  }

  /**/

  if (nFuncStatus != OK_CN00) {
    cn31_DBAActionComplete(nFuncStatus);
  } // end if

  *replyLen = (int)strlen(replyData);

  return nFuncStatus;
} /* end cn31BackupSaveIgnoreRequest */

/* PUBLIC FUNCTION cn31BackupSaveIgnore
 * ------------------------------------------------------------------
 * purpose: executes ignore replace
 *
 */
tcn00_Error cn31BackupSaveIgnore (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax )
{
  tcn00_Error           nFuncStatus = OK_CN00;

  nFuncStatus = cn31BackupSaveIgnoreRequest (vcontrol,
                                             command,
                                             replyData,
                                             replyLen,
                                             replyLenMax);

  if (nFuncStatus == OK_CN00) {
    nFuncStatus = cn31BackupReplyReceive (vcontrol,
                                          command,
                                          replyData,
                                          replyLen,
                                          replyLenMax);
  } // end if

  return nFuncStatus;
} // end cn31BackupSaveIgnore

/* PUBLIC FUNCTION cn31BackupSaveCancelRequest
 * ------------------------------------------------------------------
 * purpose: requests cancel save data/pages/log
 *
 */
tcn00_Error cn31BackupSaveCancelRequest (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax )
{
  tcn00_Error            nFuncStatus = OK_CN00;
  DBMSrvKnl_Session    * pSession  = NULL;
  tcn003_BackupSession * pBackupSession = NULL;
  char                   szSaveCommand[BA_COMMAND_LENGTH_MXCN003];
  DBMSrv_Reply           reply(replyData, replyLen, replyLenMax);

    nFuncStatus = cn31getExistingBackupAndKernelSession(vcontrol, reply, pBackupSession, pSession, ForBackup_cn31);

  // get rid of any previously used backup tool object, it is causing error if still present during "backup_cancel"
  if(OK_CN00==nFuncStatus && 0!=pBackupSession->pBackupTool)  //If we already used a backup tool, forget it.
  {
      delete pBackupSession->pBackupTool;
      pBackupSession->pBackupTool=0;
  }

  /* build command */
  if (nFuncStatus == OK_CN00) {
    nFuncStatus = cn31_BuildSaveCancelCommand(pBackupSession->szBackupType,  szSaveCommand);
  } // end if

  /* execute command */
  if (nFuncStatus == OK_CN00) {
    nFuncStatus = cn31_BackupCommandRequest(replyData, replyLen, replyLenMax, *pSession, szSaveCommand, &(pBackupSession->tBackupResult));
  } // end if

  /* set cancel flag for receive-Function */
  if (nFuncStatus == OK_CN00) {
    pBackupSession->bCancelActive = true;
  } // end if

  /**/

  if (nFuncStatus != OK_CN00) {
    cn31_DBAActionComplete(nFuncStatus);
  } // end if

  *replyLen = (int)strlen(replyData);

  return nFuncStatus;
} /* end cn31BackupSaveCancelRequest */

/* PUBLIC FUNCTION cn31BackupSaveCancel
 * ------------------------------------------------------------------
 * purpose: executes cancel save data/pages/log
 *
 */
tcn00_Error cn31BackupSaveCancel (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax )
{
  tcn00_Error           nFuncStatus = OK_CN00;

  nFuncStatus = cn31BackupSaveCancelRequest (vcontrol,
                                             command,
                                             replyData,
                                             replyLen,
                                             replyLenMax);

  if (nFuncStatus == OK_CN00) {
    nFuncStatus = cn31BackupReplyReceive (vcontrol,
                                          command,
                                          replyData,
                                          replyLen,
                                          replyLenMax);
  } // end if

  return nFuncStatus;
} // end cn31BackupSaveCancel

/* PUBLIC FUNCTION cn31BackupSaveState
 * ------------------------------------------------------------------
 * purpose: get state of save
 *
 */
tcn00_Error cn31BackupSaveState (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax )
{
  tcn00_Error           nFuncStatus = OK_CN00;

  /**/

  nFuncStatus = cn31_BackupState(vcontrol, replyData, replyLen, replyLenMax, BA_KEY_STATE_SAVE_CN003);

  /**/

  *replyLen = (int)strlen(replyData);

  return nFuncStatus;
} /* end cn31BackupSaveState */

/* PUBLIC FUNCTION cn31_BackupRestoreRequest
 * ------------------------------------------------------------------
 * purpose: requests restore or check restore data/pages/log from specified
 *          medium
 *
 */
static tcn00_Error cn31_BackupRestoreRequest (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax,
      bool            bCheck,
      bool            bCreate)
{
  tcn00_Error           nFuncStatus = OK_CN00;
  DBMSrvKnl_Session     * pSession  = NULL;
  tcn003_BackupSession  * pBackupSession = NULL;
  tcn003_MediaGroup     * pMediaGroup = NULL;
  tcn003_Medium         * pMedium  = NULL;
  const char            * StartLabelToCheck=0;
  const char            * EndLabelToCheck=0;
  char                  szRestoreCommand[BA_COMMAND_LENGTH_MXCN003];
  char                  szParam[255];
  int                   nParam = 1;
  DBMSrv_Reply          reply(replyData, replyLen, replyLenMax);

  const char * StartOfExternalBackupID=0;
  const char * EndOfExternalBackupID=0;

    // get database session (formerly known as utility session) to database (recovery) or to service db (recover_check)
    pSession = DBMSrvKnl_GlobalSessions::GetSession(bCheck ? DBMSrvKnl_GlobalSessions::SessionService 
                                                         : DBMSrvKnl_GlobalSessions::SessionDatabase);
    if(0==pSession)
        nFuncStatus = cn31_ConstructAnswerMissingSession(reply, bCheck);
    else
        if(!bCheck)
            pSession->allowReconnectAtNextExecute(DBMSrvKnl_Session::backupRestoreCheck);

  /* get backup session */
  if (nFuncStatus == OK_CN00) {
    nFuncStatus = cn31BackupSession(vcontrol, reply, &pBackupSession, bCheck?ForCheck_cn31:ForRestore_cn31);

    /* reset backup session*/
    if (nFuncStatus == OK_CN00) {
      cn31FreeBackupSession(vcontrol);
      pBackupSession = NULL;
    } else {
      nFuncStatus = OK_CN00;
    }

    if (nFuncStatus == OK_CN00) {
      nFuncStatus = cn31_AllocBackupSession(vcontrol, replyData, replyLen, &pBackupSession);
    }
  }

  /* get media name */
  if (nFuncStatus == OK_CN00) {

    pBackupSession->bCheckOnly   = bCheck;
    pBackupSession->nSessionType = BA_SESSION_TYPE_RESTORE_CN003;

    if (cn90GetToken(command->args, pBackupSession->szMediaName, nParam, ME_NAME_LENGTH_MXCN003)) {
      cn31_RemoveDoublequotes(pBackupSession->szMediaName);
      nParam++;
    } else {
      nFuncStatus = ERR_PARAM_CN00;
      cn90AnswerIError(replyData, replyLen, nFuncStatus);
    }
  }

  // read parameters
  while ((nFuncStatus == OK_CN00) && cn90GetToken(command->args, szParam, nParam, 255)) {

    // get type to restore (DATA/PAGES/LOG)
    if (cn31_CheckBackupType(replyData, replyLen, szParam) == OK_CN00) {
      SAPDB_strcpy(pBackupSession->szBackupType, szParam);

    // get autoignore flag (AUTOIGNORE)
    } else if (cn90Stricmp(szParam, BA_KEY_AUTOIGNORE_CN003) == 0) {
      pBackupSession->bAutoignore = true;

    // get "ExternalBackupID/EBID"
    } else if( cn90Stricmp(szParam, BA_KEY_EBID_CN003)     == 0  ||
               cn90Stricmp(szParam, BA_KEY_EBIDLONG_CN003) == 0     ) {
      if(cn90GetStartAndEndOfToken(command->args, StartOfExternalBackupID, EndOfExternalBackupID, nParam+1)) {
        ++nParam;
      } else {
        nFuncStatus = cn90AnswerIError(replyData, replyLen, ERR_PARAM_CN00);
      } // end if

    // get until time (UNTIL)
    } else if (cn90Stricmp(szParam, BA_KEY_UNTIL_CN003) == 0) {
      // until time is only valid for log restore
      if (cn90Stricmp(pBackupSession->szBackupType, BA_KEY_LOG_CN003) != 0) {
        nFuncStatus = cn90AnswerIError(replyData, replyLen, ERR_PARAM_CN00);
      } else {
        if (cn90GetToken(command->args, pBackupSession->szUntilDate, nParam + 1, BA_UNTIL_LENGTH_MXCN003) &&
            cn90GetToken(command->args, pBackupSession->szUntilTime, nParam + 2, BA_UNTIL_LENGTH_MXCN003)     ) {
            nParam += 2;
        } else {
          nFuncStatus = cn90AnswerIError(replyData, replyLen, ERR_PARAM_CN00);
        } // end if
      } // end if

    } else if(0==cn90Stricmp(szParam, BA_KEY_LABEL_CN003)) {
        //get start and end of label to check
        if(cn90GetStartAndEndOfToken(command->args, StartLabelToCheck, EndLabelToCheck, nParam+1))
            ++nParam;
        else
            nFuncStatus=cn90AnswerIError(replyData, replyLen, ERR_PARAM_CN00);

    } else {
      // it might be a filenumber
      SAPDB_strcpy(pBackupSession->szFileNumber, szParam);
    } // end if

    ++nParam;
  } // end while

  /**/

  /* get type to restore (DATA/PAGES/LOG) */
/*
  if (nFuncStatus == OK_CN00) {
    if (cn90GetToken(command->args, pBackupSession->szBackupType, nParam)) {
      nParam++;
      nFuncStatus = cn31_CheckBackupType(replyData, replyLen, pBackupSession->szBackupType);
    }
  }


  ExternalSaveID[0]='\0';

  if(OK_CN00==nFuncStatus)
  {
      if(cn90GetToken(command->args, szParam, nParam))
      {
          int ProceedWithAnalysis=0;
          nParam++;
          if(0==cn90Stricmp(szParam, BA_KEY_EBID_CN003) ||
              0==cn90Stricmp(szParam, BA_KEY_EBIDLONG_CN003))
          {
              if(cn90GetToken(command->args, ExternalSaveID, nParam))
              {
                  nParam++;
                  if(cn90GetToken(command->args, szParam, nParam))
                  {
                      nParam++;
                      ProceedWithAnalysis=1;
                  }
              }
              else
              {
                  nFuncStatus = ERR_CN00;
                  cn90AnswerIError(replyData, replyLen, nFuncStatus);
              }
          }
          else
              ProceedWithAnalysis=1;

          if(ProceedWithAnalysis)
          {
              SAPDB_strcpy(pBackupSession->szFileNumber, szParam);

              if (cn90Stricmp(pBackupSession->szBackupType, BA_KEY_LOG_CN003) == 0) {
                if (cn90GetToken(command->args, szParam, nParam)) {                             // UNTIL
                  nParam++;
                  if (cn90Stricmp(szParam, BA_KEY_UNTIL_CN003) == 0) {
                    if (cn90GetToken(command->args, pBackupSession->szUntilDate, nParam)) {
                      nParam++;
                      if (cn90GetToken(command->args, pBackupSession->szUntilTime, nParam)) {
                        nParam++;
                      } else {
                        nFuncStatus = ERR_PARAM_CN00;
                        cn90AnswerIError(replyData, replyLen, nFuncStatus);
                      }
                    } else {
                      nFuncStatus = ERR_PARAM_CN00;
                      cn90AnswerIError(replyData, replyLen, nFuncStatus);
                    }
                  } else {
                    nFuncStatus = ERR_PARAM_CN00;
                    cn90AnswerIError(replyData, replyLen, nFuncStatus);
                  }
                }
              }
          }
      }
  }
*/

  /**/

  /* read the media definitions */
  if (nFuncStatus == OK_CN00) {
    nFuncStatus = cn30ReadMedia(vcontrol->dbname, replyData, replyLen, replyLenMax, &(pBackupSession->tMedia));
  }

  /* find medium/mediagroup in media definition and build command */
  if (nFuncStatus == OK_CN00) {

    nFuncStatus = cn30FindMediumByName(replyData, replyLen,
                    &(pBackupSession->tMedia), pBackupSession->szMediaName, true,
                    &pMediaGroup, &pMedium);

    if (nFuncStatus == OK_CN00) {
      nFuncStatus = cn31_CheckBackupTypeMedium(replyData, replyLen, pBackupSession->szBackupType, pMedium);
    }

    cn31_DBAActionStart(vcontrol->dbname, pBackupSession, false);

    if (nFuncStatus == OK_CN00) {
      nFuncStatus = cn31_BuildRestoreCommand(pBackupSession->szBackupType,
                                             pBackupSession->bCheckOnly, 
                                             pMedium,
                                             pBackupSession->szFileNumber,
                                             pBackupSession->szUntilDate,
                                             pBackupSession->szUntilTime,
                                             szRestoreCommand,
                                             bCreate,
                                             StartLabelToCheck,
                                             EndLabelToCheck-StartLabelToCheck);
    } else {
      if (nFuncStatus == ERR_MENOTFOUND_CN00) {
        nFuncStatus = OK_CN00;

        nFuncStatus = cn30FindMediaGroupByName(replyData, replyLen,
                        &(pBackupSession->tMedia), pBackupSession->szMediaName,
                        &pMediaGroup);

        if (nFuncStatus == OK_CN00) {
          nFuncStatus = cn31_CheckBackupTypeMediaGroup(replyData, replyLen, pBackupSession->szBackupType, pMediaGroup);
        }

        if (nFuncStatus == OK_CN00) {
          nFuncStatus = cn31_BuildParallelRestoreCommand(pBackupSession->szBackupType,                    
                                                         pBackupSession->bCheckOnly, 
                                                         pMediaGroup,
                                                         pBackupSession->szFileNumber,
                                                         pBackupSession->szUntilDate,
                                                         pBackupSession->szUntilTime,
                                                         szRestoreCommand,
                                                         bCreate,
                                                         StartLabelToCheck,
                                                         EndLabelToCheck-StartLabelToCheck);
        }
      }
    }
  }

    if(OK_CN00==nFuncStatus)
    {
        // at first set timeout off for this utility session
        DBMSrvMsg_Error oError;
        if(DBMSrvKnl_State::StateOnline==DBMSrvKnl_State::State(vcontrol->dbname))
            pSession->Execute(BA_KEY_TIMEOUTOFF_CN003, oError);

        if(DBMSrvBMedia_MediaGroup::NeedsBackupTool(pMedium, pMediaGroup))  // check medium properties, if backup tool is needed
        {
            nFuncStatus=TheDBMExternalBackupControllerFactory_cn35b.ConstructADBMExternalBackupController(pBackupSession->pBackupTool,
                                                                                                          replyData,
                                                                                                          replyLen,
                                                                                                          tcn35d_BackupInfo::ARestore,
                                                                                                          pBackupSession->szBackupType,
                                                                                                          int(0),
                                                                                                          pSession,
                                                                                                          szRestoreCommand,
                                                                                                          pMedium,
                                                                                                          pMediaGroup,
                                                                                                          (char *)vcontrol->dbname,
                                                                                                          0,
                                                                                                          0,
                                                                                                          StartOfExternalBackupID,
                                                                                                          EndOfExternalBackupID);

            if(OK_CN00==nFuncStatus)   //BackupController was constructed successfully
            {
                // use the backup tool
                pBackupSession->pBackupTool->BeginBackupOperation();

                // get the results form the backup
                nFuncStatus=pBackupSession->pBackupTool->GiveLastError();   // get the results form the restore
                pBackupSession->tBackupResult.nBackupRC=pBackupSession->pBackupTool->GiveSQLResult();

                if(0==pBackupSession->tBackupResult.nBackupRC && OK_CN00==nFuncStatus)
                    nFuncStatus=cn90AnswerOK(replyData, replyLen, NULL);
            }
        }
        else    // no backup tool required
            nFuncStatus=cn31_BackupCommandRequest(replyData, replyLen, replyLenMax, *pSession, szRestoreCommand, &(pBackupSession->tBackupResult)); // execute command
    }

  /**/

  if (nFuncStatus != OK_CN00) {
    cn31_DBAActionComplete(nFuncStatus);
  } // end if

  *replyLen = (int)strlen(replyData);

    if(0!=pSession && !bCheck)
        pSession->disallowReconnectAtNextExecute();

  return nFuncStatus;
} /* end cn31_BackupRestoreRequest */

// PUBLIC FUNCTION cn31BackupRestoreRequest
// ------------------------------------------------------------------
// purpose: requests restore or check restore data/pages/log from specified
//          device
//

tcn00_Error cn31BackupRestoreRequest(VControlDataT     * vcontrol,
                                     char              * replyData,
                                     int               * replyLen,
                                     int                 replyLenMax,
                                     const char        * MediumName,
                                     const char        * BackupType,
                                     const char        * ExternalBackupID,
                                     tcn003_MediaGroup * pMediaGroup,
                                     tcn003_Medium     * pMedium,
                                     tcn003_Media      * Media,
                                     int               & MediaUsedByBackupSession,
                                     const char        * UntilDate,
                                     const char        * UntilTime,
                                     const char        * LabelToCheck)
{
    bool bCheck=false;   //could be made a function parameter
    bool bCreate=false;  //could be made a function parameter

    tcn00_Error            rc=OK_CN00;
    DBMSrvKnl_Session    * pSession=0;
    tcn003_BackupSession * pBackupSession=0;
    char                   szRestoreCommand[BA_COMMAND_LENGTH_MXCN003];

    const char * StartOfExternalBackupID=ExternalBackupID;
    const char * EndOfExternalBackupID=ExternalBackupID+strlen(ExternalBackupID);

    DBMSrv_Reply reply(replyData, replyLen, replyLenMax);

    // get database session (formerly known as utility session) to database (recovery) or to service db (recover_check)
    pSession = DBMSrvKnl_GlobalSessions::GetSession(bCheck 
                                                    ? DBMSrvKnl_GlobalSessions::SessionService 
                                                    : DBMSrvKnl_GlobalSessions::SessionDatabase);

    if(0==pSession)
        rc = cn31_ConstructAnswerMissingSession(reply, bCheck);
    else
        if(!bCheck)
            pSession->allowReconnectAtNextExecute(DBMSrvKnl_Session::backupRestoreCheck);

    // get backup session
    if(OK_CN00==rc)
    {
        rc=cn31BackupSession(vcontrol, reply, &pBackupSession, bCheck?ForCheck_cn31:ForRestore_cn31);

        // reset backup session
        if(OK_CN00==rc)
        {
            cn31FreeBackupSession(vcontrol);
            pBackupSession=0;
        }
        else
            rc=OK_CN00;

        if(OK_CN00==rc)
            rc=cn31_AllocBackupSession(vcontrol, replyData, replyLen, &pBackupSession);
    }

    if(rc==OK_CN00)
    {
        pBackupSession->bCheckOnly  =bCheck;
        pBackupSession->nSessionType=BA_SESSION_TYPE_RESTORE_CN003;

        const char * EndMediumName=cn36_StartOfTrailingWhiteSpaces(MediumName);
        cn36_StrNCpy(pBackupSession->szMediaName, MediumName, EndMediumName-MediumName); //skip trailing white spaces

        pBackupSession->tMedia=(*Media);
        MediaUsedByBackupSession=1;

        rc=cn31_CheckBackupType(replyData, replyLen, BackupType);

        if(OK_CN00==rc)
        {
            SAPDB_strcpy(pBackupSession->szBackupType, BackupType); // set type to restore (DATA/PAGES/LOG)

            pBackupSession->bAutoignore=false; //autoignore is done by calling function as needed

            SAPDB_strcpy(pBackupSession->szUntilDate, UntilDate);
            SAPDB_strcpy(pBackupSession->szUntilTime, UntilTime);

            // build backup command
            if(0!=pMedium)
            {
                rc=cn31_BuildRestoreCommand(pBackupSession->szBackupType,
                                            pBackupSession->bCheckOnly, 
                                            pMedium,
                                            pBackupSession->szFileNumber,
                                            pBackupSession->szUntilDate,
                                            pBackupSession->szUntilTime,
                                            szRestoreCommand,
                                            bCreate,
                                            LabelToCheck,
                                            strlen(LabelToCheck));
            }
            else
            {
                rc=cn31_BuildParallelRestoreCommand(pBackupSession->szBackupType,                    
                                                    pBackupSession->bCheckOnly, 
                                                    pMediaGroup,
                                                    pBackupSession->szFileNumber,
                                                    pBackupSession->szUntilDate,
                                                    pBackupSession->szUntilTime,
                                                    szRestoreCommand,
                                                    bCreate,
                                                    LabelToCheck,
                                                    strlen(LabelToCheck));
            }
        }
    }

    cn31_DBAActionStart(vcontrol->dbname, pBackupSession, false);

    if(OK_CN00==rc)
    {
        // at first set timeout off for this utility session
        DBMSrvMsg_Error oError;
        if(DBMSrvKnl_State::StateOnline==DBMSrvKnl_State::State(vcontrol->dbname))
            pSession->Execute(BA_KEY_TIMEOUTOFF_CN003, oError);

        if(DBMSrvBMedia_MediaGroup::NeedsBackupTool(pMedium, pMediaGroup))  // check medium properties, if backup tool is needed
        {
            rc=TheDBMExternalBackupControllerFactory_cn35b.ConstructADBMExternalBackupController(pBackupSession->pBackupTool,
                                                                                                 replyData,
                                                                                                 replyLen,
                                                                                                 tcn35d_BackupInfo::ARestore,
                                                                                                 pBackupSession->szBackupType,
                                                                                                 int(0),
                                                                                                 pSession,
                                                                                                 szRestoreCommand,
                                                                                                 pMedium,
                                                                                                 pMediaGroup,
                                                                                                 (char *)vcontrol->dbname,
                                                                                                 0,
                                                                                                 0,
                                                                                                 StartOfExternalBackupID,
                                                                                                 EndOfExternalBackupID);

            if(OK_CN00==rc)   //BackupController was constructed successfully
            {
                // use the backup tool
                pBackupSession->pBackupTool->BeginBackupOperation();

                // get the results form the backup
                rc=pBackupSession->pBackupTool->GiveLastError();   // get the results form the restore
                pBackupSession->tBackupResult.nBackupRC=pBackupSession->pBackupTool->GiveSQLResult();

                if(0==pBackupSession->tBackupResult.nBackupRC && OK_CN00==rc)
                    rc=cn90AnswerOK(replyData, replyLen, NULL);
            }
        }
        else    // no backup tool required
            rc=cn31_BackupCommandRequest(replyData, replyLen, replyLenMax, *pSession, szRestoreCommand, &(pBackupSession->tBackupResult)); // execute command
    }

    if (rc != OK_CN00) {
      cn31_DBAActionComplete(rc);
    } // end if

    *replyLen=(int)strlen(replyData);

    if(0!=pSession && !bCheck)
        pSession->disallowReconnectAtNextExecute();

    return rc;
} // end cn31BackupRestoreRequest

/* PUBLIC FUNCTION cn31BackupRestoreRequest
 * ------------------------------------------------------------------
 * purpose: requests restore data/pages/log from specified medium
 *
 */
tcn00_Error cn31BackupRestoreRequest (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax )
{
  return cn31_BackupRestoreRequest (vcontrol,
                                    command,
                                    replyData,
                                    replyLen,
                                    replyLenMax,
                                    false,
                                    false);

} // end cn31BackupRestoreRequest

/* PUBLIC FUNCTION cn31BackupRestore
 * ------------------------------------------------------------------
 * purpose: executes restore data/pages/log from specified medium
 *
 */
tcn00_Error cn31BackupRestore (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax )
{
  tcn00_Error           nFuncStatus = OK_CN00;

  nFuncStatus = cn31BackupRestoreRequest (vcontrol,
                                          command,
                                          replyData,
                                          replyLen,
                                          replyLenMax);

  if (nFuncStatus == OK_CN00) {
    nFuncStatus = cn31BackupReplyReceive (vcontrol,
                                          command,
                                          replyData,
                                          replyLen,
                                          replyLenMax);
  } // end if

  return nFuncStatus;
} // end cn31BackupRestore

/* PUBLIC FUNCTION cn31BackupRestoreCheckRequest
 * ------------------------------------------------------------------
 * purpose: requests check restore data/pages/log from specified medium with
 *          service kernel
 *
 */
tcn00_Error cn31BackupRestoreCheckRequest (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax )
{
  return cn31_BackupRestoreRequest (vcontrol,
                                    command,
                                    replyData,
                                    replyLen,
                                    replyLenMax,
                                    true,
                                    false);

} // end cn31BackupRestoreCheckRequest

/* PUBLIC FUNCTION cn31BackupRestoreCheck
 * ------------------------------------------------------------------
 * purpose: executes check restore data/pages/log from specified medium with
 *          service kernel
 */
tcn00_Error cn31BackupRestoreCheck (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax )
{
  tcn00_Error           nFuncStatus = OK_CN00;

  nFuncStatus = cn31BackupRestoreCheckRequest (vcontrol,
                                               command,
                                               replyData,
                                               replyLen,
                                               replyLenMax);

  if (nFuncStatus == OK_CN00) {
    nFuncStatus = cn31BackupReplyReceive (vcontrol,
                                          command,
                                          replyData,
                                          replyLen,
                                          replyLenMax);
  } // end if

  return nFuncStatus;
} // end cn31BackupRestoreCheck

/* PUBLIC FUNCTION cn31BackupRestoreCreateRequest
 * ------------------------------------------------------------------
 * purpose: requests create instance restore data/pages/log from specified 
 *          medium
 *
 */
tcn00_Error cn31BackupRestoreCreateRequest (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax )
{
  return cn31_BackupRestoreRequest (vcontrol,
                                    command,
                                    replyData,
                                    replyLen,
                                    replyLenMax,
                                    false,
                                    true);

} // end cn31BackupRestoreCreateRequest

/* PUBLIC FUNCTION cn31BackupRestoreCreate
 * ------------------------------------------------------------------
 * purpose: executes create instance restore data/pages/log from specified 
 *          medium
 */
tcn00_Error cn31BackupRestoreCreate (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax )
{
  tcn00_Error           nFuncStatus = OK_CN00;

  nFuncStatus = cn31BackupRestoreCreateRequest (vcontrol,
                                               command,
                                               replyData,
                                               replyLen,
                                               replyLenMax);

  if (nFuncStatus == OK_CN00) {
    nFuncStatus = cn31BackupReplyReceive (vcontrol,
                                          command,
                                          replyData,
                                          replyLen,
                                          replyLenMax);
  } // end if

  return nFuncStatus;
} // end cn31BackupRestoreCreate

/* PUBLIC FUNCTION cn31BackupRestoreConfig
 * ------------------------------------------------------------------
 * purpose: restore config with service kernel
 *
 */
tcn00_Error cn31BackupRestoreConfig (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax )
{
  tcn00_Error            nFuncStatus = OK_CN00;
  DBMSrvKnl_Session    * pSession = NULL;
  tcn003_BackupSession * pBackupSession = NULL;
  tcn003_MediaGroup    * pMediaGroup = NULL;
  tcn003_Medium        * pMedium  = NULL;
  char                   szRestoreCommand[BA_COMMAND_LENGTH_MXCN003];
  int                    nParam = 1;
  DBMSrv_Reply           reply(replyData, replyLen, replyLenMax);

  /**/

  /* get utility session */
  pSession = DBMSrvKnl_GlobalSessions::GetSession(DBMSrvKnl_GlobalSessions::SessionService);
  if (pSession == NULL) {
    nFuncStatus = cn90AnswerEvent(replyData, replyLen, replyLenMax, DBMSrvMsg_Error(SDBMSG_DBMSRV_NOSERVICESESSION));
  } // end if

  /* get backup session */
  if (nFuncStatus == OK_CN00) {
    nFuncStatus = cn31BackupSession(vcontrol, reply, &pBackupSession, ForCheck_cn31);

    /* reset backup session*/
    if (nFuncStatus == OK_CN00) {
      cn31FreeBackupSession(vcontrol);
      pBackupSession = NULL;
    } else {
      nFuncStatus = OK_CN00;
    }

    if (nFuncStatus == OK_CN00) {
      nFuncStatus = cn31_AllocBackupSession(vcontrol, replyData, replyLen, &pBackupSession);
    }
  }

  /* get media name */
  if (nFuncStatus == OK_CN00) {

    pBackupSession->nSessionType = BA_SESSION_TYPE_RESTORE_CN003;

    if (cn90GetToken(command->args, pBackupSession->szMediaName, nParam, ME_NAME_LENGTH_MXCN003)) {
      cn31_RemoveDoublequotes(pBackupSession->szMediaName);
      nParam++;
    } else {
      nFuncStatus = ERR_PARAM_CN00;
      cn90AnswerIError(replyData, replyLen, nFuncStatus);
    }
  }

  /**/

  /* read the media definitions */
  if (nFuncStatus == OK_CN00) {
    nFuncStatus = cn30ReadMedia(vcontrol->dbname, replyData, replyLen, replyLenMax, &(pBackupSession->tMedia));
  }

  /* find medium/mediagroup in media definition and build command */
  if (nFuncStatus == OK_CN00) {

    nFuncStatus = cn30FindMediumByName(replyData, replyLen,
                    &(pBackupSession->tMedia), pBackupSession->szMediaName, true,
                    &pMediaGroup, &pMedium);

    if (nFuncStatus == OK_CN00) {
      nFuncStatus = cn31_CheckBackupTypeMedium(replyData, replyLen, pBackupSession->szBackupType, pMedium);
    }

    if (nFuncStatus == OK_CN00) {
      nFuncStatus = cn31_BuildRestoreConfigCommand(pMedium, vcontrol->dbname, szRestoreCommand);
    } else {
      if (nFuncStatus == ERR_MENOTFOUND_CN00) {
        nFuncStatus = OK_CN00;

        nFuncStatus = cn30FindMediaGroupByName(replyData, replyLen,
                        &(pBackupSession->tMedia), pBackupSession->szMediaName,
                        &pMediaGroup);

        if (nFuncStatus == OK_CN00) {
          nFuncStatus = cn31_CheckBackupTypeMediaGroup(replyData, replyLen, pBackupSession->szBackupType, pMediaGroup);
        }

        if (nFuncStatus == OK_CN00) {
          nFuncStatus = cn31_BuildParallelRestoreConfigCommand(pMediaGroup, vcontrol->dbname, szRestoreCommand);
        }
      }
    }
  }

  /* execute command */
  if (nFuncStatus == OK_CN00) {
    nFuncStatus = cn31_BackupCommand(replyData, replyLen, replyLenMax, *pSession, szRestoreCommand, false, &(pBackupSession->tBackupResult));
  }

  // save colduser
  if (nFuncStatus == OK_CN00) {
    tcn002_XpValueString  XpValueC;
    tcn002_XpValueName    XpKeyC;

    cn90StrUpperCopy(XpValueC, vcontrol->pCurrentUser->getUserName(), false);
    XpKeyC  .rawAssign(PAN_CONTROLID);
    cn20XParamPutValue(vcontrol->dbname, XpKeyC, XpValueC);

    XpValueC.rawAssign(vcontrol->pCurrentUser->getClearMasterPwd());
    XpKeyC  .rawAssign(PAN_CONTROLPW);
    cn20XParamPutValue(vcontrol->dbname, XpKeyC, XpValueC);

    vcontrol->pCurrentUser->setColdUser();
    vcontrol->pCurrentUser->save();
  } // end if

  /* execuet os command */
//  if (nFuncStatus == OK_CN00) {
//    nFuncStatus = cn31_BackupOSCommand(replyData, replyLen, pBackupSession, &(pBackupSession->tBackupResult));
//  }

  /**/

  *replyLen = (int)strlen(replyData);

  return nFuncStatus;
} /* end cn31BackupRestoreConfig */

// PUBLIC FUNCTION cn31BackupRestoreReplaceRequest
// ------------------------------------------------------------------
// purpose: requests continue restore data/pages/log after a replace
//

tcn00_Error cn31BackupRestoreReplaceRequest (
      VControlDataT     * vcontrol,
      char              * replyData,
      int               * replyLen,
      int                 replyLenMax,
      const char        * MediumName,
      const char        * ExternalBackupID,
      tcn003_Medium     * pMedium,
      tcn003_MediaGroup * pMediaGroup,
      tcn003_Media      * Media,
      int               & MediaUsedByBackupSession)
{
    tcn00_Error            rc=OK_CN00;
    DBMSrvKnl_Session    * pSession=0;
    tcn003_BackupSession * pBackupSession=0;
    char                   szRestoreCommand[BA_COMMAND_LENGTH_MXCN003];
    DBMSrv_Reply           reply(replyData, replyLen, replyLenMax);

    const char * StartOfExternalBackupID=ExternalBackupID;
    const char * EndOfExternalBackupID=ExternalBackupID+strlen(ExternalBackupID);

    rc=cn31getExistingBackupAndKernelSession(vcontrol, reply, pBackupSession, pSession, ForRestore_cn31);

    // get media name
    if(OK_CN00==rc)
    {
        const char * EndMediumName=cn36_StartOfTrailingWhiteSpaces(MediumName);
        cn36_StrNCpy(pBackupSession->szMediaName, MediumName, EndMediumName-MediumName); //skip trailing white spaces

        pBackupSession->tMedia=(*Media);
        MediaUsedByBackupSession=1;

        rc=cn31_BuildRestoreReplaceCommand(pBackupSession->szBackupType,
                                           pBackupSession->bCheckOnly, 
                                           pMedium,
                                           pBackupSession->szFileNumber,
                                           szRestoreCommand);
    }

    // execute command
    if(OK_CN00==rc)
    {
        if(0!=pBackupSession->pBackupTool)  //If we already used a backup tool, forget it. The backuptool can change with every
        {                                   //backup/restore/replace command (unlikely but not impossible)
            delete pBackupSession->pBackupTool;
            pBackupSession->pBackupTool=0;
        }

        if(DBMSrvBMedia_MediaGroup::NeedsBackupTool(pMedium, pMediaGroup))  // check medium properties, if backup tool is needed
        {
            rc=TheDBMExternalBackupControllerFactory_cn35b.ConstructADBMExternalBackupController(pBackupSession->pBackupTool,
                                                                                                 replyData,
                                                                                                 replyLen,
                                                                                                 tcn35d_BackupInfo::ARestore,
                                                                                                 pBackupSession->szBackupType,
                                                                                                 int(0),
                                                                                                 pSession,
                                                                                                 szRestoreCommand,
                                                                                                 pMedium,
                                                                                                 pMediaGroup,
                                                                                                 (char *)vcontrol->dbname,
                                                                                                 0,
                                                                                                 0,
                                                                                                 StartOfExternalBackupID,
                                                                                                 EndOfExternalBackupID);

            if(OK_CN00==rc)   //BackupController was constructed successfully
            {
                // use the backup tool
                pBackupSession->pBackupTool->BeginBackupOperation();

                // get the results form the backup
                rc=pBackupSession->pBackupTool->GiveLastError();   // get the results form the restore
                pBackupSession->tBackupResult.nBackupRC=pBackupSession->pBackupTool->GiveSQLResult();

                if(0==pBackupSession->tBackupResult.nBackupRC && OK_CN00==rc)
                    rc=cn90AnswerOK(replyData, replyLen, NULL);
            }
        }
        else    // no backup tool required
            rc=cn31_BackupCommandRequest(replyData,                          // execute command
                                         replyLen,
                                         replyLenMax,
                                         *pSession,
                                         szRestoreCommand,
                                         &(pBackupSession->tBackupResult));
    }

    if (rc != OK_CN00) {
      cn31_DBAActionComplete(rc);
    } // end if

    *replyLen=(int)strlen(replyData);

    return rc;
} // end cn31BackupRestoreReplaceRequest

/* PUBLIC FUNCTION cn31BackupRestoreReplaceRequest
 * ------------------------------------------------------------------
 * purpose: requests continue restore data/pages/log after a replace
 *
 */
tcn00_Error cn31BackupRestoreReplaceRequest (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax )
{
  tcn00_Error            nFuncStatus = OK_CN00;
  DBMSrvKnl_Session    * pSession = NULL;
  tcn003_BackupSession * pBackupSession = NULL;
  tcn003_MediaGroup    * pMediaGroup = NULL;
  tcn003_Medium        * pMedium     = NULL;
  char                   szRestoreCommand[BA_COMMAND_LENGTH_MXCN003];
  char                   szLocation[ME_LOCATION_LENGTH_MXCN003];   /* new location 19990519 */
  int                    nParam = 1;
  DBMSrv_Reply           reply(replyData, replyLen, replyLenMax);

  const char * StartOfExternalBackupID=0;
  const char * EndOfExternalBackupID=0;

    nFuncStatus = cn31getExistingBackupAndKernelSession(vcontrol, reply, pBackupSession, pSession, ForRestore_cn31);

  /* get media name */
  if (nFuncStatus == OK_CN00) {
    if (cn90GetToken(command->args, pBackupSession->szMediaName, nParam, ME_NAME_LENGTH_MXCN003)) {
      cn31_RemoveDoublequotes(pBackupSession->szMediaName);
      nParam++;
    } else {
      nFuncStatus = ERR_PARAM_CN00;
      cn90AnswerIError(replyData, replyLen, nFuncStatus);
    }
  }

    if(OK_CN00==nFuncStatus)
    {
        const char *StartOfArgument;
        const char *EndOfArgument;

        if(cn90GetStartAndEndOfToken(command->args, StartOfArgument, EndOfArgument, nParam))
        {
            if(cn90StrNicmp(StartOfArgument, BA_KEY_EBID_CN003, EndOfArgument-StartOfArgument)     == 0  ||
               cn90StrNicmp(StartOfArgument, BA_KEY_EBIDLONG_CN003, EndOfArgument-StartOfArgument) == 0     )
            {
                ++nParam;

                if(cn90GetStartAndEndOfToken(command->args, StartOfExternalBackupID, EndOfExternalBackupID, nParam))
                    ++nParam;
            }
        }
    }

  /* get new location for medium 19990519 */
  if (nFuncStatus == OK_CN00) {
    szLocation[0] = '\0';
    if (cn90GetToken(command->args, szLocation, nParam, ME_LOCATION_LENGTH_MXCN003)) {
      cn31_RemoveDoublequotes(szLocation);
    nParam++;
    }
  }

  /* get number of file for LOG */
  if (nFuncStatus == OK_CN00) {
    if (cn90GetToken(command->args, pBackupSession->szFileNumber, nParam, BA_FILENUM_LENGTH_MXCN003)) {
      nParam++;
  }
  }

  /**/

    /* read the media definitions for new media 19990518 */
    if(OK_CN00==nFuncStatus)
    {
        cn30FreeMedia(&(pBackupSession->tMedia));
        nFuncStatus = cn30ReadMedia(vcontrol->dbname, replyData, replyLen, replyLenMax, &(pBackupSession->tMedia));
    }

  /* find medium in media definition and build command */
  if (nFuncStatus == OK_CN00) {

    nFuncStatus = cn30FindMediumByName(replyData, replyLen,
                    &(pBackupSession->tMedia), pBackupSession->szMediaName, true,
                    &pMediaGroup, &pMedium);

    if (nFuncStatus == OK_CN00) {

      /* set new location for medíum 19990519 */
      if (szLocation[0] != '\0') {
      SAPDB_strcpy(pMedium->szLocation, szLocation);
      }

      nFuncStatus = cn31_BuildRestoreReplaceCommand(pBackupSession->szBackupType,
                                                    pBackupSession->bCheckOnly, 
                                                    pMedium,
                                                    pBackupSession->szFileNumber,
                                                    szRestoreCommand);
    } else
        if (nFuncStatus == ERR_MENOTFOUND_CN00)
        {
            nFuncStatus = cn30FindMediaGroupByName(replyData, replyLen,
                              &(pBackupSession->tMedia), pBackupSession->szMediaName,
                              &pMediaGroup);

            if (nFuncStatus == OK_CN00) {

                pMedium = pMediaGroup->pFirst;

                /* set new location for medíum 19990625 */
                if (szLocation[0] != '\0') {
                SAPDB_strcpy(pMedium->szLocation, szLocation);
                }

                nFuncStatus = cn31_BuildRestoreReplaceCommand(pBackupSession->szBackupType,
                                                       pBackupSession->bCheckOnly, 
                                                       pMedium,
                                                       pBackupSession->szFileNumber,
                                                       szRestoreCommand);
            }
        }
  }

  /* execute command */
    if (nFuncStatus == OK_CN00)
    {
        if(0!=pBackupSession->pBackupTool)  //If we already used a backup tool, forget it. The backuptool can change with every
        {                                   //backup/restore/replace command (unlikely but not impossible)
            delete pBackupSession->pBackupTool;
            pBackupSession->pBackupTool=0;
        }

        if(DBMSrvBMedia_MediaGroup::NeedsBackupTool(pMedium, pMediaGroup))  // check medium properties, if backup tool is needed
        {
            nFuncStatus=TheDBMExternalBackupControllerFactory_cn35b.ConstructADBMExternalBackupController(pBackupSession->pBackupTool,
                                                                                                          replyData,
                                                                                                          replyLen,
                                                                                                          tcn35d_BackupInfo::ARestore,
                                                                                                          pBackupSession->szBackupType,
                                                                                                          int(0),
                                                                                                          pSession,
                                                                                                          szRestoreCommand,
                                                                                                          pMedium,
                                                                                                          pMediaGroup,
                                                                                                          (char *)vcontrol->dbname,
                                                                                                          0,
                                                                                                          0,
                                                                                                          StartOfExternalBackupID,
                                                                                                          EndOfExternalBackupID);

            if(OK_CN00==nFuncStatus)   //BackupController was constructed successfully
            {
                // use the backup tool
                pBackupSession->pBackupTool->BeginBackupOperation();

                // get the results form the backup
                nFuncStatus=pBackupSession->pBackupTool->GiveLastError();   // get the results form the restore
                pBackupSession->tBackupResult.nBackupRC=pBackupSession->pBackupTool->GiveSQLResult();

                if(0==pBackupSession->tBackupResult.nBackupRC && OK_CN00==nFuncStatus)
                    nFuncStatus=cn90AnswerOK(replyData, replyLen, NULL);
            }
        }
        else    // no backup tool required
            nFuncStatus = cn31_BackupCommandRequest(replyData,                          // execute command
                                                    replyLen,
                                                    replyLenMax,
                                                    *pSession,
                                                    szRestoreCommand,
                                                    &(pBackupSession->tBackupResult));
    }

  /**/

  if (nFuncStatus != OK_CN00) {
    cn31_DBAActionComplete(nFuncStatus);
  } // end if

  *replyLen = (int)strlen(replyData);

  return nFuncStatus;

} /* end cn31BackupRestoreReplaceRequest */

/* PUBLIC FUNCTION cn31BackupRestoreReplace
 * ------------------------------------------------------------------
 * purpose: executes continue restore data/pages/log after a replace
 */
tcn00_Error cn31BackupRestoreReplace (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax )
{
  tcn00_Error           nFuncStatus = OK_CN00;

  nFuncStatus = cn31BackupRestoreReplaceRequest (vcontrol,
                                                 command,
                                                 replyData,
                                                 replyLen,
                                                 replyLenMax);

  if (nFuncStatus == OK_CN00) {
    nFuncStatus = cn31BackupReplyReceive (vcontrol,
                                          command,
                                          replyData,
                                          replyLen,
                                          replyLenMax);
  } // end if

  return nFuncStatus;
} // end cn31BackupRestoreReplace

/* PUBLIC FUNCTION cn31BackupRestoreIgnoreRequest
 * ------------------------------------------------------------------
 * purpose: requests ignore replace
 *
 */
tcn00_Error cn31BackupRestoreIgnoreRequest (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax )
{
  tcn00_Error            nFuncStatus = OK_CN00;
  DBMSrvKnl_Session    * pSession = NULL;
  tcn003_BackupSession * pBackupSession = NULL;
  char                   szRestoreCommand[BA_COMMAND_LENGTH_MXCN003];
  DBMSrv_Reply           reply(replyData, replyLen, replyLenMax);

    nFuncStatus = cn31getExistingBackupAndKernelSession(vcontrol, reply, pBackupSession, pSession, ForRestore_cn31);

  // get rid of any previously used backup tool object, it is causing error if still present during "recover_ignore"
  if(OK_CN00==nFuncStatus && 0!=pBackupSession->pBackupTool)  //If we already used a backup tool, forget it.
  {
      delete pBackupSession->pBackupTool;
      pBackupSession->pBackupTool=0;
  }

  /* build command */
  if (nFuncStatus == OK_CN00) {
    nFuncStatus = cn31_BuildRestoreIgnoreCommand(pBackupSession->szBackupType, szRestoreCommand);
  }

  /* execute command */
  if (nFuncStatus == OK_CN00) {
    nFuncStatus = cn31_BackupCommandRequest(replyData, replyLen, replyLenMax, *pSession, szRestoreCommand, &(pBackupSession->tBackupResult));
  }

  /**/

  if (nFuncStatus != OK_CN00) {
    cn31_DBAActionComplete(nFuncStatus);
  } // end if

  *replyLen = (int)strlen(replyData);

  return nFuncStatus;

} /* end cn31BackupRestoreIgnoreRequest */

/* PUBLIC FUNCTION cn31BackupRestoreIgnore
 * ------------------------------------------------------------------
 * purpose: executes ignore replace
 */
tcn00_Error cn31BackupRestoreIgnore (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax )
{
  tcn00_Error           nFuncStatus = OK_CN00;

  nFuncStatus = cn31BackupRestoreIgnoreRequest (vcontrol,
                                                 command,
                                                 replyData,
                                                 replyLen,
                                                 replyLenMax);

  if (nFuncStatus == OK_CN00) {
    nFuncStatus = cn31BackupReplyReceive (vcontrol,
                                          command,
                                          replyData,
                                          replyLen,
                                          replyLenMax);
  } // end if

  return nFuncStatus;
} // end cn31BackupRestoreIgnore

/* PUBLIC FUNCTION cn31BackupRestoreCancelRequest
 * ------------------------------------------------------------------
 * purpose: requests cancel restore data/pages/log
 *
 */
tcn00_Error cn31BackupRestoreCancelRequest (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax )
{
  tcn00_Error            nFuncStatus = OK_CN00;
  DBMSrvKnl_Session    * pSession = NULL;
  tcn003_BackupSession * pBackupSession = NULL;
  char                   szRestoreCommand[BA_COMMAND_LENGTH_MXCN003];
  DBMSrv_Reply           reply(replyData, replyLen, replyLenMax);

    nFuncStatus = cn31getExistingBackupAndKernelSession(vcontrol, reply, pBackupSession, pSession, ForRestore_cn31);

  // get rid of any previously used backup tool object, it is causing error if still present during "recover_cancel"
  if(OK_CN00==nFuncStatus && 0!=pBackupSession->pBackupTool)  //If we already used a backup tool, forget it.
  {
      delete pBackupSession->pBackupTool;
      pBackupSession->pBackupTool=0;
  }

  /* build command */
  if (nFuncStatus == OK_CN00) {
    nFuncStatus = cn31_BuildRestoreCancelCommand(pBackupSession->szBackupType, szRestoreCommand);
  }

  /* execute command */
  if (nFuncStatus == OK_CN00) {
    nFuncStatus = cn31_BackupCommandRequest(replyData, replyLen, replyLenMax, *pSession, szRestoreCommand, &(pBackupSession->tBackupResult));
  }

  /* set cancel flag for receive-Function */
  if (nFuncStatus == OK_CN00) {
    pBackupSession->bCancelActive = true;
  } // end if

  /**/

  if (nFuncStatus != OK_CN00) {
    cn31_DBAActionComplete(nFuncStatus);
  } // end if

  *replyLen = (int)strlen(replyData);

  return nFuncStatus;

} /* end cn31BackupRestoreCancelRequest */

/* PUBLIC FUNCTION cn31BackupRestoreCancel
 * ------------------------------------------------------------------
 * purpose: executes cancel restore data/pages/log
 */
tcn00_Error cn31BackupRestoreCancel (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax )
{
  tcn00_Error           nFuncStatus = OK_CN00;

  nFuncStatus = cn31BackupRestoreCancelRequest (vcontrol,
                                                 command,
                                                 replyData,
                                                 replyLen,
                                                 replyLenMax);

  if (nFuncStatus == OK_CN00) {
    nFuncStatus = cn31BackupReplyReceive (vcontrol,
                                          command,
                                          replyData,
                                          replyLen,
                                          replyLenMax);
  } // end if

  return nFuncStatus;
} // end cn31BackupRestoreCancel

/* PUBLIC FUNCTION cn31BackupRestoreState
 * ------------------------------------------------------------------
 * purpose: get state of restore
 *
 */
tcn00_Error cn31BackupRestoreState (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax )
{
  tcn00_Error             nFuncStatus = OK_CN00;

  /**/

  nFuncStatus = cn31_BackupState(vcontrol, replyData, replyLen, replyLenMax, BA_KEY_STATE_RESTORE_CN003, false);

  /**/

  *replyLen = (int)strlen(replyData);

  return nFuncStatus;
} /* end cn31BackupRestoreState */

/* PUBLIC FUNCTION cn31BackupRestoreStateCheck
 * ------------------------------------------------------------------
 * purpose: get state of restore
 *
 */
tcn00_Error cn31BackupRestoreStateCheck (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax )
{
  tcn00_Error             nFuncStatus = OK_CN00;

  /**/

  nFuncStatus = cn31_BackupState(vcontrol, replyData, replyLen, replyLenMax, BA_KEY_STATE_RESTORE_CN003, true);

  /**/

  *replyLen = (int)strlen(replyData);

  return nFuncStatus;
} /* end cn31BackupRestoreStateCheck */

/* PUBLIC FUNCTION cn31BackupRestartInfo
 * ------------------------------------------------------------------
 * purpose: get restart informations after restore
 *
 */
tcn00_Error cn31BackupRestartInfo(
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax )
{
  tcn00_Error               nFuncStatus = OK_CN00;
  char                    * pCurrPos    = NULL;
  tcn003_BackupRestartInfo  tRestartInfo;

  nFuncStatus = cn31BackupRestartInfo(vcontrol, replyData, replyLen, replyLenMax, &tRestartInfo);

  if (nFuncStatus == OK_CN00) {
    sprintf(replyData, "%s%s", ANSWER_OK_CN00, LINE_SEPSTRING_CN00);
    pCurrPos = replyData + strlen(replyData);

    sprintf(pCurrPos, "%-30s%s%s%s", "Used LOG Page"            , VALUE_SEPSTRING_CN00, tRestartInfo.FirstIoSequenceNeeded,    LINE_SEPSTRING_CN00);
    pCurrPos = replyData + strlen(replyData);

    sprintf(pCurrPos, "%-30s%s%s%s", "First LOG Page"           , VALUE_SEPSTRING_CN00, tRestartInfo.FirstIoSequenceOnLog,     LINE_SEPSTRING_CN00);
    pCurrPos = replyData + strlen(replyData);

    sprintf(pCurrPos, "%-30s%s%s%s", "Restartable"              , VALUE_SEPSTRING_CN00, tRestartInfo.Restartable,              LINE_SEPSTRING_CN00);
    pCurrPos = replyData + strlen(replyData);

    sprintf(pCurrPos, "%-30s%s%s%s", "Id Restart Record"        , VALUE_SEPSTRING_CN00, tRestartInfo.DbIdentFromRestartrecord, LINE_SEPSTRING_CN00);
    pCurrPos = replyData + strlen(replyData);

    sprintf(pCurrPos, "%-30s%s%s%s", "Id LOG Info"              , VALUE_SEPSTRING_CN00, tRestartInfo.DbIdentFromLoginfo,       LINE_SEPSTRING_CN00);
    pCurrPos = replyData + strlen(replyData);

    sprintf(pCurrPos, "%-30s%s%s%s", "Consistent"               , VALUE_SEPSTRING_CN00, tRestartInfo.RestartableWithoutLog,    LINE_SEPSTRING_CN00);
    pCurrPos = replyData + strlen(replyData);

    sprintf(pCurrPos, "%-30s%s%s%s", "Log Volume Enabled"       , VALUE_SEPSTRING_CN00, tRestartInfo.LogVolumeEnabled,         LINE_SEPSTRING_CN00);
    pCurrPos = replyData + strlen(replyData);

    sprintf(pCurrPos, "%-30s%s%s%s", "Log Auto Overwrite"       , VALUE_SEPSTRING_CN00, tRestartInfo.LogAutoOverwrite,         LINE_SEPSTRING_CN00);
    pCurrPos = replyData + strlen(replyData);

    sprintf(pCurrPos, "%-30s%s%s%s", "Master Servernode"        , VALUE_SEPSTRING_CN00, tRestartInfo.MasterServernode,         LINE_SEPSTRING_CN00);
    pCurrPos = replyData + strlen(replyData);

    sprintf(pCurrPos, "%-30s%s%s%s", "Converter Version"        , VALUE_SEPSTRING_CN00, tRestartInfo.ConverterVersion,         LINE_SEPSTRING_CN00);
    pCurrPos = replyData + strlen(replyData); 

    sprintf(pCurrPos, "%-30s%s%s%s", "Oldest Compatible Version", VALUE_SEPSTRING_CN00, tRestartInfo.OldestCompatibleVersion,  LINE_SEPSTRING_CN00);
    pCurrPos = replyData + strlen(replyData);

    sprintf(pCurrPos, "%-30s%s%s%s", "Queue Count"              , VALUE_SEPSTRING_CN00, tRestartInfo.QueueCount,               LINE_SEPSTRING_CN00);
    pCurrPos = replyData + strlen(replyData);

  } // end if

  *replyLen = (int)strlen(replyData);

  return nFuncStatus;
} /* end cn31BackupRestartInfo */

/* PUBLIC FUNCTION cn31BackupRestartInfo
 * ------------------------------------------------------------------
 * purpose: get restart informations after restore
 *
 */
tcn00_Error cn31BackupRestartInfo(
      VControlDataT            * vcontrol,
      char                     * replyData,
      int                      * replyLen,
      int                        replyLenMax,
      tcn003_BackupRestartInfo * pRestartInfo)
{
  tcn00_Error       nFuncStatus = OK_CN00;
  cn50DBMUser       usrDBM    ( vcontrol->dbname, cn50DBMUser::getUser(vcontrol->dbname, FIRSTDBM_CN50));
  DBMSrvKnl_Session oSession;

  memset(pRestartInfo, 0 , sizeof(tcn003_BackupRestartInfo));

  DBMSrvMsg_Error oError;
  if (!oSession.NormalConnect(vcontrol->dbname.asCharp(),
                              usrDBM.getUserName().asCharp(),
                              usrDBM.getClearMasterPwd().asCharp(),
                              oError)) {
    nFuncStatus = cn90AnswerEvent(replyData, replyLen, replyLenMax, oError);
  } // end if

  if (nFuncStatus == OK_CN00) {
    if (!oSession.Execute(BA_KEY_RESTART_INFO_CN003, oError)) {
      nFuncStatus = cn90AnswerEvent(replyData, replyLen, replyLenMax, oError);
    } // end if
  } // end if

  if (nFuncStatus == OK_CN00) {
    DBMSrvKnl_SQLRecord & oRecord = oSession.GetRecordObject();
    if (!oRecord.isNull("FIRST IO SEQUENCE NEEDED"   )) strncpy(pRestartInfo->FirstIoSequenceNeeded   , oRecord.fieldAsChar("FIRST IO SEQUENCE NEEDED"   ).CharPtr(), sizeof(pRestartInfo->FirstIoSequenceNeeded   )-1);
    if (!oRecord.isNull("FIRST IO SEQUENCE ON LOG"   )) strncpy(pRestartInfo->FirstIoSequenceOnLog    , oRecord.fieldAsChar("FIRST IO SEQUENCE ON LOG"   ).CharPtr(), sizeof(pRestartInfo->FirstIoSequenceOnLog    )-1);
    if (!oRecord.isNull("RESTARTABLE"                )) strncpy(pRestartInfo->Restartable             , oRecord.fieldAsChar("RESTARTABLE"                ).CharPtr(), sizeof(pRestartInfo->Restartable             )-1);
    if (!oRecord.isNull("DB IDENT FROM RESTARTRECORD")) strncpy(pRestartInfo->DbIdentFromRestartrecord, oRecord.fieldAsChar("DB IDENT FROM RESTARTRECORD").CharPtr(), sizeof(pRestartInfo->DbIdentFromRestartrecord)-1);
    if (!oRecord.isNull("DB IDENT FROM LOGINFO"      )) strncpy(pRestartInfo->DbIdentFromLoginfo      , oRecord.fieldAsChar("DB IDENT FROM LOGINFO"      ).CharPtr(), sizeof(pRestartInfo->DbIdentFromLoginfo      )-1);
    if (!oRecord.isNull("RESTARTABLE WITHOUT LOG"    )) strncpy(pRestartInfo->RestartableWithoutLog   , oRecord.fieldAsChar("RESTARTABLE WITHOUT LOG"    ).CharPtr(), sizeof(pRestartInfo->RestartableWithoutLog   )-1);
    if (!oRecord.isNull("LOG VOLUME ENABLED"         )) strncpy(pRestartInfo->LogVolumeEnabled        , oRecord.fieldAsChar("LOG VOLUME ENABLED"         ).CharPtr(), sizeof(pRestartInfo->LogVolumeEnabled        )-1);
    if (!oRecord.isNull("LOG AUTO OVERWRITE"         )) strncpy(pRestartInfo->LogAutoOverwrite        , oRecord.fieldAsChar("LOG AUTO OVERWRITE"         ).CharPtr(), sizeof(pRestartInfo->LogAutoOverwrite        )-1);
    if (!oRecord.isNull("MASTER SERVERNODE"          )) strncpy(pRestartInfo->MasterServernode        , oRecord.fieldAsChar("MASTER SERVERNODE"          ).CharPtr(), sizeof(pRestartInfo->MasterServernode        )-1);
    if (!oRecord.isNull("CONVERTER VERSION"          )) strncpy(pRestartInfo->ConverterVersion        , oRecord.fieldAsChar("CONVERTER VERSION"          ).CharPtr(), sizeof(pRestartInfo->ConverterVersion        )-1);
    if (!oRecord.isNull("OLDEST COMPATIBLE VERSION"  )) strncpy(pRestartInfo->OldestCompatibleVersion , oRecord.fieldAsChar("OLDEST COMPATIBLE VERSION"  ).CharPtr(), sizeof(pRestartInfo->OldestCompatibleVersion )-1);
    if (!oRecord.isNull("QUEUE COUNT"                )) strncpy(pRestartInfo->QueueCount              , oRecord.fieldAsChar("QUEUE COUNT"                ).CharPtr(), sizeof(pRestartInfo->QueueCount              )-1);
  } // end if

  return nFuncStatus;
} /* end cn31BackupRestartInfo */

/* PUBLIC FUNCTION cn31BackupLabel
 * ------------------------------------------------------------------
 * purpose: read label from specified medium
 *
 */
tcn00_Error cn31BackupLabel (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax )
{
  tcn00_Error           nFuncStatus = OK_CN00;
  char                  szMediumName[ME_NAME_LENGTH_MXCN003];
  tcn003_Media          tMedia;
  tcn003_Medium       * pMedium            = NULL;
  tcn003_MediaGroup   * pMediaGroupFound   = NULL;
  char                  szLabelCommand[BA_COMMAND_LENGTH_MXCN003];
  DBMSrvKnl_Session     oSession;
  tcn003_BackupResult   tResult;
  char                  szFileNumber[255];
  int                   nParam = 1;

  cn50DBMUser       usrDBM    ( vcontrol->dbname, cn50DBMUser::getUser(vcontrol->dbname, FIRSTDBM_CN50));
  DBMSrvMsg_Error oError;
  if (!oSession.NormalConnect(vcontrol->dbname.asCharp(),
                              usrDBM.getUserName().asCharp(),
                              usrDBM.getClearMasterPwd().asCharp(),
                              oError)) {
    nFuncStatus = cn90AnswerEvent(replyData, replyLen, replyLenMax, oError);
  } // end if

  if (nFuncStatus == OK_CN00) {
    if (cn90GetToken(command->args, szMediumName, nParam, ME_NAME_LENGTH_MXCN003)) {
      nParam++;
      cn31_RemoveDoublequotes(szMediumName);
    } else {
      nFuncStatus = ERR_PARAM_CN00;
      cn90AnswerIError(replyData, replyLen, nFuncStatus);
    } // end if
  } // end if

  szFileNumber[0] = '\0';
  if (nFuncStatus == OK_CN00) {
    if(cn90GetToken(command->args, szFileNumber, nParam, 255)) {
      nParam++;
    } // end if
  } // end if

  /**/

  if (nFuncStatus == OK_CN00) {
    nFuncStatus = cn30ReadMedia(vcontrol->dbname, replyData, replyLen, replyLenMax, &tMedia);
    if (nFuncStatus == OK_CN00) {
      nFuncStatus = cn30FindMediumByName(replyData, replyLen, &tMedia, szMediumName, false, &pMediaGroupFound, &pMedium);
      if (nFuncStatus == OK_CN00) {
        if (pMedium) {
          nFuncStatus = cn31_BuildLabelCommand(pMedium, szFileNumber, szLabelCommand);
          if (nFuncStatus == OK_CN00) {
            nFuncStatus = cn31_BackupCommand (replyData,
                                              replyLen,
                                              replyLenMax,
                                              oSession,
                                              szLabelCommand,
                                              false,
                                              &tResult);
          } // end if
        } // end if
      } // end if
    } // end if

    cn30FreeMedia(&tMedia);
  } // end if

  *replyLen = (int)strlen(replyData);

  return nFuncStatus;
} /* end cn31BackupLabel */

/* PUBLIC FUNCTION cn31BackupRestoreDevspace
 * ------------------------------------------------------------------
 * purpose: restores the süecified devspace
 *
 */
tcn00_Error cn31BackupRestoreDevspace (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax )
{
  tcn00_Error           nFuncReturn = OK_CN00;
  DBMSrvKnl_Session   * pSession    = NULL;
  char                  szDevspaceName[PARAMETER_MAXLEN_CN90];
  char                  szCommand[BA_COMMAND_LENGTH_MXCN003];

    pSession = DBMSrvKnl_GlobalSessions::GetSession(DBMSrvKnl_GlobalSessions::SessionDatabase);

    if(0==pSession)
        nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, DBMSrvMsg_Error(SDBMSG_DBMSRV_NODBSESSION));
    else
        pSession->allowReconnectAtNextExecute(DBMSrvKnl_Session::backupRestoreCheck);

  if (nFuncReturn == OK_CN00) {
    cn90GetToken(command->args, szDevspaceName, 1, PARAMETER_MAXLEN_CN90);
    cn31_RemoveDoublequotes(szDevspaceName);
    cn31_BuildRestoreDevspaceCommand(szDevspaceName, szCommand);
  } // end if

  /**/

  if (nFuncReturn == OK_CN00) {
    DBMSrvMsg_Error oError;
    if (!pSession->Execute(szCommand, oError)) {
      nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, oError);
    } else {
      cn90AnswerOK (replyData, replyLen, NULL);
    } // end if
  } // end if

  if(0!=pSession)
    pSession->disallowReconnectAtNextExecute();

  *replyLen = (int)strlen(replyData);

  return nFuncReturn;
} /* end cn31BackupRestoreDevspace */

/* PUBLIC FUNCTION cn31AutosaveOn
 * ------------------------------------------------------------------
 * purpose: automatic log save on
 *
 */
tcn00_Error cn31AutosaveOn (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax )
{
  char                        szMediaName[ME_NAME_LENGTH_MXCN003];
  tcn00_Error                 nFuncStatus = OK_CN00;
  tsp00_KnlIdentifierc szTmp;
  szTmp.rawAssign(vcontrol->pCurrentUser->getUserName());

  tsp00_Timestampc            szTimeStamp;
  cn90Timestamp(szTimeStamp);
  tcn53_DBAAction::start(vcontrol->dbname, szTimeStamp, DBAOP_ENAUTO_CN53, DBA_BAOBJ_LOG_CN53);

  szMediaName[0] = '\0';

  if (cn90GetToken(command->args, szMediaName, 1, ME_NAME_LENGTH_MXCN003)) {
      cn31_RemoveDoublequotes(szMediaName);
  } // end if

  nFuncStatus = cn31AutosaveOn(vcontrol, szMediaName, replyData, replyLen, replyLenMax);

  if (nFuncStatus == OK_CN00) {
    cn51DBMConfigValue          cfgAuto (vcontrol->dbname, CFG_AUTOSSAVE_CN51);

    cfgAuto = 1;
  } // end if

//  aXDB.complete((nFuncStatus == OK_CN00) ? XDBST_OK_IN101 : XDBST_ERROR_IN101);
  tcn53_DBAAction::finish(cn90Timestamp(szTimeStamp), (nFuncStatus == OK_CN00));

  return nFuncStatus;
} // end cn31AutosaveOn

/* PUBLIC FUNCTION cn31AutosaveOn
 * ------------------------------------------------------------------
 * purpose: automatic log save on
 *
 */
tcn00_Error cn31AutosaveOn (
      VControlDataT * vcontrol,
      const char    * szMediaName,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax  )
{
  tcn00_Error                 nFuncStatus = OK_CN00;
  tcn003_Media                tMedia;
  tcn003_MediaGroup         * pMediaGroup = NULL;
  tcn003_Medium             * pMedium     = NULL;
  char                        szAutosaveCommand[BA_COMMAND_LENGTH_MXCN003];
  bool                        bSaveMedium = false;
  char                        replyBuffer [PARAMETER_MAXLEN_CN90];
  int                         nLen;

  /**/

  if (replyData == NULL) {
    replyData = &replyBuffer[0];
    replyLen  = &nLen;
  } // end if

  if (DBMSrvKnl_State::State(vcontrol->dbname) != DBMSrvKnl_State::StateOnline) {
    nFuncStatus = cn90AnswerIError(replyData, replyLen, ERR_DBNORUN_CN00);
  } // end if

  /**/

  if (nFuncStatus == OK_CN00) {
    nFuncStatus = cn30ReadMedia(vcontrol->dbname, replyData, replyLen, replyLenMax, &tMedia);

    if (nFuncStatus == OK_CN00) {
      if (szMediaName[0] != '\0') {
        nFuncStatus = cn30FindMediumByName(replyData, replyLen, &tMedia, szMediaName, false, &pMediaGroup, &pMedium);

        if (nFuncStatus == OK_CN00) {
          if ((cn90Stricmp(pMedium->szSaveType, ME_SAVETYPE_AUT_CN003) != 0) &&
              (cn90Stricmp(pMedium->szSaveType, ME_SAVETYPE_LOG_CN003) != 0)) {

            nFuncStatus = ERR_BASAVETYPECON_CN00;
            cn90AnswerIError(replyData, replyLen, nFuncStatus);
          }

          if (nFuncStatus == OK_CN00) {
            if (cn90Stricmp(pMedium->szSaveType, ME_SAVETYPE_LOG_CN003) == 0) {
              bSaveMedium = true;

              tcn003_MediaGroup   * pMediaGroupItem;
              tcn003_Medium       * pMediumItem;

              pMediaGroupItem = tMedia.pFirstGroup;
              while (pMediaGroupItem != NULL) {
                pMediumItem = pMediaGroupItem->pFirst;
                while (pMediumItem != NULL) {
                  if (pMediumItem->szSaveType[0] != '\0') {
                    if ( (strcmp(pMediumItem->szSaveType, ME_SAVETYPE_AUT_CN003) == 0) ) {
                      SAPDB_strcpy(pMediumItem->szSaveType, ME_SAVETYPE_LOG_CN003);
                    }
                  }
                  pMediumItem = pMediumItem->pNext;
                }
                pMediaGroupItem = pMediaGroupItem->pNext;
              }

              SAPDB_strcpy(pMedium->szSaveType, ME_SAVETYPE_AUT_CN003);

              nFuncStatus = cn30WriteMedia(vcontrol->dbname, replyData, replyLen, &(tMedia));
            }
          }

        }
      } else {
        nFuncStatus =  cn31_FindAutosaveMedia(replyData, replyLen, &tMedia, &pMediaGroup, &pMedium);
      }
    }

    if (nFuncStatus == OK_CN00) {
      if ( (pMediaGroup != NULL) && (pMedium != NULL) ) {
        if (pMediaGroup->szName[0] == '\0') {
          nFuncStatus = cn31_BuildAutosaveOnCommand(pMedium, szAutosaveCommand);
        } else {
          nFuncStatus = cn31_BuildParallelAutosaveOnCommand(pMediaGroup, szAutosaveCommand);
        }
      }
    }

    if (nFuncStatus == OK_CN00) {
      tcn53_DBAAction::writeDetail(DBADTL_BACMD_CN53, szAutosaveCommand);
      // execute kernel command
      cn50DBMUser     oUser(vcontrol->dbname, cn50DBMUser::getColdUser(vcontrol->dbname));
      DBMSrvMsg_Error oError;
      if (!DBMSrvKnl_Session::Execute                (vcontrol->dbname.asCharp(),
                                                      oUser.getUserName().asCharp(),
                                                      oUser.getClearMasterPwd().asCharp(),
                                                      szAutosaveCommand,
                                                      oError)) {
        nFuncStatus = cn90AnswerEvent(replyData, replyLen, replyLenMax, oError);
      } // end if
    }

    cn30FreeMedia(&tMedia);
  }

  /**/

  if (nFuncStatus == OK_CN00) {
    sprintf(replyData, "%s%s", ANSWER_OK_CN00, LINE_SEPSTRING_CN00);
  } // end if

  tcn53_DBAAction::writeDetail(DBADTL_BARESULT_CN53, replyData);

  *replyLen = (int)strlen(replyData);

  return nFuncStatus;
} /* end cn31AutosaveOn */

/* PUBLIC FUNCTION cn31AutosaveEnd
 * ------------------------------------------------------------------
 * purpose: automatic log save end
 *
 */
tcn00_Error cn31AutosaveEnd (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax )
{
  tcn00_Error                 nFuncStatus = OK_CN00;
  tsp00_KnlIdentifierc szTmp;
  szTmp.rawAssign(vcontrol->pCurrentUser->getUserName());
  tsp00_Timestampc            szTimeStamp;

  cn90Timestamp(szTimeStamp);
  tcn53_DBAAction::start(vcontrol->dbname, szTimeStamp, DBAOP_DISAUTO_CN53, DBA_BAOBJ_LOG_CN53);

  nFuncStatus = cn31AutosaveEnd ( vcontrol, replyData, replyLen, replyLenMax );

  if (nFuncStatus == OK_CN00) {
    cn51DBMConfigValue          cfgAuto (vcontrol->dbname, CFG_AUTOSSAVE_CN51);

    cfgAuto = 0;
  } // end if

  tcn53_DBAAction::finish(cn90Timestamp(szTimeStamp), (nFuncStatus == OK_CN00));

  return nFuncStatus;
} // end cn31AutosaveEnd

/* PUBLIC FUNCTION cn31AutosaveEnd
 * ------------------------------------------------------------------
 * purpose: automatic log save end
 *
 */
tcn00_Error cn31AutosaveEnd (
      VControlDataT * vcontrol,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax )
{
  tcn00_Error                 nFuncStatus = OK_CN00;
  char                        szAutosaveCommand[BA_COMMAND_LENGTH_MXCN003];
  char                        replyBuffer [PARAMETER_MAXLEN_CN90];
  int                         nLen;

  /**/

  if (replyData == NULL) {
    replyData = &replyBuffer[0];
    replyLen  = &nLen;
  } // end if

  if (DBMSrvKnl_State::State(vcontrol->dbname) != DBMSrvKnl_State::StateOnline) {
    nFuncStatus = cn90AnswerIError(replyData, replyLen, ERR_DBNORUN_CN00);
  } // end if

  /**/

  if (nFuncStatus == OK_CN00) {
    nFuncStatus = cn31_BuildAutosaveEndCommand(szAutosaveCommand);

    if (nFuncStatus == OK_CN00) {
      tcn53_DBAAction::writeDetail(DBADTL_BACMD_CN53, szAutosaveCommand);
      // execute kernel command
      cn50DBMUser     oUser(vcontrol->dbname, cn50DBMUser::getColdUser(vcontrol->dbname));
      DBMSrvMsg_Error oError;
      if (!DBMSrvKnl_Session::Execute                (vcontrol->dbname.asCharp(),
                                                      oUser.getUserName().asCharp(),
                                                      oUser.getClearMasterPwd().asCharp(),
                                                      szAutosaveCommand,
                                                      oError)) {
        nFuncStatus = cn90AnswerEvent(replyData, replyLen, replyLenMax, oError);
      } // end if
    }
  }

  /**/

  if (nFuncStatus == OK_CN00) {
    sprintf(replyData, "%s%s", ANSWER_OK_CN00, LINE_SEPSTRING_CN00);
  } // end if

  tcn53_DBAAction::writeDetail(DBADTL_BARESULT_CN53, replyData);

  *replyLen = (int)strlen(replyData);

  return nFuncStatus;
} /* end cn31AutosaveEnd */

/* PUBLIC FUNCTION cn31AutosaveCancel
 * ------------------------------------------------------------------
 * purpose: cancel a running log save or ending autosave
 *
 */
tcn00_Error cn31AutosaveCancel (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax )
{
  tcn00_Error         nFuncStatus = OK_CN00;
  char                szAutosaveCommand[BA_COMMAND_LENGTH_MXCN003];

  /**/

  if (nFuncStatus == OK_CN00) {
    nFuncStatus = cn31_BuildAutosaveCancelCommand(szAutosaveCommand);

    if (nFuncStatus == OK_CN00) {
      // execute kernel command
      cn50DBMUser     oUser(vcontrol->dbname, cn50DBMUser::getColdUser(vcontrol->dbname));
      DBMSrvMsg_Error oError;
      if (!DBMSrvKnl_Session::Execute                (vcontrol->dbname.asCharp(),
                                                      oUser.getUserName().asCharp(),
                                                      oUser.getClearMasterPwd().asCharp(),
                                                      szAutosaveCommand,
                                                      oError)) {
        nFuncStatus = cn90AnswerEvent(replyData, replyLen, replyLenMax, oError);
      } // end if
    }
  }

  /**/

  if (nFuncStatus == OK_CN00) {
    sprintf(replyData, "%s%s", ANSWER_OK_CN00, LINE_SEPSTRING_CN00);
  }

  *replyLen = (int)strlen(replyData);

  return nFuncStatus;
} /* end cn31AutosaveCancel */

/* PUBLIC FUNCTION cn31AutosaveShow
 * ------------------------------------------------------------------
 * purpose: show state of autosave
 *
 */
tcn00_Error cn31AutosaveShow (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax )
{
  tcn00_Error         nFuncStatus = OK_CN00;
  char                szAutosaveCommand[BA_COMMAND_LENGTH_MXCN003];

  nFuncStatus = cn31_BuildAutosaveShowCommand(szAutosaveCommand);

  if (nFuncStatus == OK_CN00) {
    // execute kernel command
    cn50DBMUser     oUser(vcontrol->dbname, cn50DBMUser::getColdUser(vcontrol->dbname));
    DBMSrvMsg_Error oError;
    if (!DBMSrvKnl_Session::Execute                (vcontrol->dbname.asCharp(),
                                                    oUser.getUserName().asCharp(),
                                                    oUser.getClearMasterPwd().asCharp(),
                                                    szAutosaveCommand,
                                                    oError)) {

      if (oError == DBMSrvMsg_Error::DBMSrv_SQL) {
        int nResult = oError.getTagValueAsInteger(SDBMSGTAG_DBMSRV_SQL__RETURNCODE);

        if (nResult == BA_AUTOSAVE_RC_OFF_CN003) {
          sprintf(replyData, "%s%s%s%s", ANSWER_OK_CN00, 
                                        LINE_SEPSTRING_CN00,
                                        BA_AUTOSAVE_RESULT_OFF_CN003, 
                                        LINE_SEPSTRING_CN00);
        } else if (nResult == BA_AUTOSAVE_RC_NEXT_CN003) {
          sprintf(replyData, "%s%s%s%s", ANSWER_OK_CN00, 
                                        LINE_SEPSTRING_CN00,
                                        BA_AUTOSAVE_RESULT_NEXT_CN003, 
                                        LINE_SEPSTRING_CN00);
        } else {
          nFuncStatus = cn90AnswerEvent(replyData, replyLen, replyLenMax, oError);
        } // end if
      } else {
        nFuncStatus = cn90AnswerEvent(replyData, replyLen, replyLenMax, oError);
      } // end if
    } else {
      sprintf(replyData, "%s%s%s%s", ANSWER_OK_CN00, 
                                     LINE_SEPSTRING_CN00,
                                     BA_AUTOSAVE_RESULT_ON_CN003, 
                                     LINE_SEPSTRING_CN00);
    } // end if
  } // end if

  *replyLen = (int)strlen(replyData);

  return nFuncStatus;
} /* end cn31AutosaveShow */

/*
 * ------------------------------------------------------------------
 * FOR INETRNAL USE ONLY
 * ------------------------------------------------------------------
 */

const char * cn31BackupSessionTypeAsString(cn31BackupSessionType sessionFor)
{
    switch( sessionFor )
    {
        case ForRestore_cn31: return "restore";
        case ForCheck_cn31:   return "service";
        default :             return "backup";
    }
}

/* INTERN FUNCTION cn31BackupSession
 * ------------------------------------------------------------------
 * purpose: get the backup session from global struct and check
 *
 */
tcn00_Error cn31BackupSession (
      VControlDataT            * vcontrol,
      DBMSrv_Reply             & reply,
      tcn003_BackupSession    ** pBackupSession,
      cn31BackupSessionType      sessionFor )
{
    tcn00_Error  rc=OK_CN00;

    *pBackupSession = (tcn003_BackupSession *) vcontrol->pBackupSession;

    // is there an active backup session ?
    if(0==(*pBackupSession))
        rc=reply.startWithMessageList(DBMSrvMsg_Error(
                                            SDBMSG_DBMSRV_NOBACKUPSESSION,
                                            Msg_Arg(SDBMSGTAG_DBMSRV_NOBACKUPSESSION__SESSIONTYPE, cn31BackupSessionTypeAsString(sessionFor))));

  return rc;
}

tcn00_Error cn31getExistingBackupAndKernelSession (
    VControlDataT            * vcontrol,
    DBMSrv_Reply             & reply,
    tcn003_BackupSession    *& backupSession,
    DBMSrvKnl_Session       *& kernelSession,
    cn31BackupSessionType      sessionFor)
{
    tcn00_Error rc=cn31BackupSession(vcontrol, reply, &backupSession, sessionFor);

    // get utility session
    if( OK_CN00==rc )
    {
        kernelSession = DBMSrvKnl_GlobalSessions::GetSession(backupSession->bCheckOnly 
                                                    ? DBMSrvKnl_GlobalSessions::SessionService 
                                                    : DBMSrvKnl_GlobalSessions::SessionDatabase);

        if( 0==kernelSession )
            rc=cn31_ConstructAnswerMissingSession(reply, backupSession->bCheckOnly);
    }

    //was that backup session interrupted by an implicit reconnect?
    if( OK_CN00==rc )  
    {
        DBMSrvMsg_Info info;

        if(kernelSession->hasInterruptedSubsession(DBMSrvKnl_Session::backupRestoreCheck, info))
        {
            if(ForRestore_cn31==sessionFor && backupSession->bCheckOnly) //now that we have a backup session, we can improve the session type
                sessionFor=ForCheck_cn31;

            DBMSrvMsg_Error error(
                                info,
                                SDBMSG_DBMSRV_BACKUPSESSIONLOST,
                                Msg_Arg(SDBMSGTAG_DBMSRV_BACKUPSESSIONLOST__SESSIONTYPE, cn31BackupSessionTypeAsString(sessionFor)));

            error.Overrule(DBMSrvMsg_Error(
                                SDBMSG_DBMSRV_NOBACKUPSESSION,
                                Msg_Arg(SDBMSGTAG_DBMSRV_NOBACKUPSESSION__SESSIONTYPE, cn31BackupSessionTypeAsString(sessionFor))));

            rc=reply.startWithMessageList(error);
        }
    }

    return rc;
}

/* INTERN FUNCTION cn31BackupStageSession
 * ------------------------------------------------------------------
 * purpose: get the backup stage session from global struct and check
 *
 */
tcn00_Error cn31BackupStageSession(
    VControlDataT         * vcontrol,
    DBMSrv_Reply             & reply,
    tcn003_BackupSession ** pBackupSession)
{
    tcn00_Error rc=OK_CN00;

    *pBackupSession=(tcn003_BackupSession *)vcontrol->pBackupStageSession;

    if(0==(*pBackupSession))    // is there an active backup session
        rc=reply.startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_NOBACKUPSESSION, Msg_Arg(SDBMSGTAG_DBMSRV_NOBACKUPSESSION__SESSIONTYPE, "backup stage")));

    return rc;
} /* end cn31BackupStageSession */

/* INTERN FUNCTION cn31FreeBackupSession
 * ------------------------------------------------------------------
 * purpose: frees the backup session from the VControlDataT structure
 *
 */
void cn31FreeBackupSession(
      VControlDataT     * vcontrol)
{
  tcn003_BackupSession  * pBackupSession=0;

  /**/

  if (vcontrol != NULL) {
    pBackupSession =  (tcn003_BackupSession *) vcontrol->pBackupSession;
  }

  if (pBackupSession != NULL) {

    // is there an active backup in error state?
    if (pBackupSession->tBackupResult.nBackupRC != 0) {
      char                    szCommand[BA_COMMAND_LENGTH_MXCN003];
      tcn00_Error             nFuncStatus = OK_CN00;
      char                    replyData[2000];
      int                     replyLen = 0;
      int                     replyLenMax=2000;
      DBMSrvKnl_Session     * pSession = NULL;
      DBMSrv_Reply            reply(replyData, &replyLen, replyLenMax);

      if (pBackupSession->nSessionType == BA_SESSION_TYPE_SAVE_CN003) {
        nFuncStatus = cn31_BuildSaveCancelCommand(pBackupSession->szBackupType, szCommand);
      } else if (pBackupSession->nSessionType == BA_SESSION_TYPE_RESTORE_CN003) {
        nFuncStatus = cn31_BuildRestoreCancelCommand(pBackupSession->szBackupType, szCommand);
      } else {
        nFuncStatus = ERR_CN00;
      } // end if

      if (nFuncStatus == OK_CN00) {
        pSession = DBMSrvKnl_GlobalSessions::GetSession(pBackupSession->bCheckOnly 
                                                        ? DBMSrvKnl_GlobalSessions::SessionService 
                                                        : DBMSrvKnl_GlobalSessions::SessionDatabase);
        if (pSession == NULL) {
          nFuncStatus = cn31_ConstructAnswerMissingSession(reply, pBackupSession->bCheckOnly);
        } // end if
      } // end if

      if (nFuncStatus == OK_CN00) {
        nFuncStatus = cn31_BackupCommand(replyData, &replyLen, replyLenMax, *pSession, szCommand, false, &(pBackupSession->tBackupResult));
      } // end if
    } // end if

    cn31_DBAActionComplete(DBMSrvMsg_Error::get_tcn00_Error(DBMSrvMsg_Error::DBMSrv_SQL));

    cn30FreeMedia(&(pBackupSession->tMedia));

    if(NULL != pBackupSession->pBackupTool) {
      delete pBackupSession->pBackupTool;
    } // end if
    if(0!=pBackupSession->StageController) {
      delete pBackupSession->StageController;
    } // end if

    cn00MyFree(pBackupSession);
    vcontrol->pBackupSession = NULL;
  }
} /* end cn31FreeBackupSession */

/* INTERN FUNCTION cn31FreeBackupStageSession
 * ------------------------------------------------------------------
 * purpose: frees the backup stage session from the VControlDataT structure
 *
 */
void cn31FreeBackupStageSession(VControlDataT * vcontrol)
{
    tcn003_BackupSession * pBackupSession=0;

    if(0!=vcontrol)
        pBackupSession=(tcn003_BackupSession *)vcontrol->pBackupStageSession;

    if(0!=pBackupSession)
    {
        tsp00_Timestampc szTimeStamp;

        cn90Timestamp(szTimeStamp);
        tcn53_DBAAction::finish(szTimeStamp, false);    //if we really finish it here, something went wrong

        cn30FreeMedia(&(pBackupSession->tMedia));

        if(0!=pBackupSession->pBackupTool)
            delete pBackupSession->pBackupTool;

        if(0!=pBackupSession->StageController)
            delete pBackupSession->StageController;

        cn00MyFree(pBackupSession);
        vcontrol->pBackupStageSession=0;
    }
} /* end cn31FreeBackupStageSession */

/* PRIVATE FUNCTION cn31_DBAActionStart
 * ------------------------------------------------------------------
 * purpose: compute start parameters for the XDB-Interface
 *
 */
static void cn31_DBAActionStart(
      const tsp00_DbNamec       & szDbName,
      tcn003_BackupSession      * pBackupSession,
      const bool                  bRecovery)
{

  if (pBackupSession != NULL) {
    tsp00_Timestampc szTimeStamp;

    const _TCHAR * pOperation  = "";
    const _TCHAR * pObject     = "";

    szTimeStamp.Init();

    if (pBackupSession->nSessionType == BA_SESSION_TYPE_SAVE_CN003) {
      if (bRecovery) {
        if (stricmp(pBackupSession->szBackupType, BA_KEY_DATA_CN003) == 0) {
          pOperation = DBAOP_SAVEDATA_CN53;
          pObject    = DBA_BAOBJ_DATA_CN53;
        } else if (stricmp(pBackupSession->szBackupType, BA_KEY_PAGES_CN003) == 0) {
          pOperation = DBAOP_SAVEPAGES_CN53;
          pObject    = DBA_BAOBJ_PAGES_CN53;
        } else if (stricmp(pBackupSession->szBackupType, BA_KEY_LOG_CN003) == 0) {
          pOperation = DBAOP_SAVELOG_CN53;
          pObject    = DBA_BAOBJ_LOG_CN53;
        } // end if

      } else {
        if (stricmp(pBackupSession->szBackupType, BA_KEY_DATA_CN003) == 0) {
          pOperation = DBAOP_SAVEDATAC_CN53;
          pObject    = DBA_BAOBJ_DATA_CN53;
        } else if (stricmp(pBackupSession->szBackupType, BA_KEY_PAGES_CN003) == 0) {
          pOperation = DBAOP_SAVEPAGESC_CN53;
          pObject    = DBA_BAOBJ_PAGES_CN53;
        } else if (stricmp(pBackupSession->szBackupType, BA_KEY_LOG_CN003) == 0) {
          pOperation = DBAOP_SAVELOG_CN53;
          pObject    = DBA_BAOBJ_LOG_CN53;
        } // end if
      } // end if
    } else {
      if (stricmp(pBackupSession->szBackupType, BA_KEY_DATA_CN003) == 0) {
        if (pBackupSession->bCheckOnly) {
          pOperation = DBAOP_CHECKDATA_CN53;
        } else {
          pOperation = DBAOP_RESTDATA_CN53;
        } // end if
        pObject    = DBA_BAOBJ_DATA_CN53;
      } else if (stricmp(pBackupSession->szBackupType, BA_KEY_PAGES_CN003) == 0) {
        if (pBackupSession->bCheckOnly) {
          pOperation = DBAOP_CHECKPAGES_CN53;
        } else {
          pOperation = DBAOP_RESTPAGES_CN53;
        } // end if
        pObject    = DBA_BAOBJ_PAGES_CN53;
      } else if (stricmp(pBackupSession->szBackupType, BA_KEY_LOG_CN003) == 0) {
        if (pBackupSession->bCheckOnly) {
          pOperation = DBAOP_CHECKLOG_CN53;
        } else {
          pOperation = DBAOP_RESTLOG_CN53;
        } // end if
        pObject    = DBA_BAOBJ_LOG_CN53;
      } // end if
    } // end if

    cn90Timestamp(szTimeStamp);
    tcn53_DBAAction::start(szDbName, szTimeStamp, pOperation, pObject);

  } // end if

} // cn31_DBAActionStart

/* PRIVATE FUNCTION cn31_DBAActionComplete
 * ------------------------------------------------------------------
 * purpose: compute start parameters for the XDB-Interface
 *
 */
static void cn31_DBAActionComplete 
  ( const tcn00_Error nFuncReturn)
{
  tsp00_Timestampc szTimeStamp;

  cn90Timestamp(szTimeStamp);
  tcn53_DBAAction::finish(szTimeStamp, (nFuncReturn == OK_CN00));

} // end cn31_DBAActionComplete
