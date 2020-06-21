/*!***************************************************************************

  module      : RTEDiag_CommonCrashHist.c

  -------------------------------------------------------------------------

  responsible : StefanP

  special area: Diagnostic files
  description : Saving of diagnostic files at restart if the database
                was shutting down abnormally the last time.
                Functions for the client and server site.


  last changed: 2001-02-08  10:58
  see also    : 

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2001-2005 SAP AG

    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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


*****************************************************************************/



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include  "gsp00.h"
#include  "geo007_1.h"
#include  "RunTime/RTE_Types.h"
#include  "RunTime/Diagnose/RTEDiag_CommonCrashHist.h"
#include  "SAPDBCommon/SAPDB_string.h"

#include  "geo50_0.h"
#include  "hsp77.h"
#include  "geo57.h"
#include  "heo15.h"

#if !(defined(_WIN32))  /*&if $OSSPEC not in [ WIN32 ]*/ 
#include <sys/types.h>
#include <dirent.h>
#include "hen40.h"      /* nocheck */
#endif                  /*&endif*/


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/
#define     CURRENT_DIRECTORY           "."
#define     PARENT_DIRECTORY            ".."

#define     ARBITRARY_FILE              "*"

#define     LAST_ERROR_MESSAGE_SIZE     64

#define     ERROR_TEXT_LEN              80

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/
/*--- Structure of the array which has to be generated to clean up  ---*/
/*--- the history entries on disk                                   ---*/
typedef struct
{
    RTE_Path      path;
    time_t          gentime;
}   HISTORY; 

/*---------------------------------------------------------------------------*/


/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/
static  void                RTEDiag_DeleteDirectory         (SAPDB_Char        *path);
  

static  int                 RTEDiag_CompareHistDate         (const void        *history1,
                                                             const void        *history2); 

static  SAPDB_Bool          RTEDiag_GetDateTimeFromHistory  (RTE_DBName       dbname,
                                                             SAPDB_Char        *history,
                                                             time_t            *p_time);

/*===========================================================================*
 *  EXTERN FUNCTIONS                                                         *
 *===========================================================================*/

/***************************************************************/
#if defined(_WIN32)
/***************************************************************/

/* For the documentation see header file */
extern void RTEDiag_OsError    (SAPDB_Char         *routine,
                                SAPDB_Char         *callFunction,
                                SAPDB_Char         *zusText,
                                RTE_SystemRc        rc,
                                SAPDB_UInt         type)  
{
   /*===========================================================================*
    *  Locals                                                                   *
    *===========================================================================*/
    SAPDB_Char    lastErrorMessage[LAST_ERROR_MESSAGE_SIZE];
    SAPDB_Char    buf[ERROR_TEXT_LEN];


   /*===========================================================================*
    *  Instructions                                                             *
    *===========================================================================*/
    /* Error message on Win32 are sometimes much much too long so we only report Error code... */
    sp77sprintf(lastErrorMessage, sizeof(lastErrorMessage), "[%d], %s", rc == RTE_UNDEF_ERROR ? 
                GetLastError() : rc, zusText == NULL ? "" : zusText);

    switch (type)
    {
        case INFO_TYPE:
            MSGD((INFO_DIAG_HIST_OS_ERROR, routine, callFunction == NULL ? "" : callFunction, lastErrorMessage));
            break;

        case ERR_TYPE:
            MSGD((ERR_DIAG_HIST_OS_ERROR, routine, callFunction == NULL ? "" : callFunction, lastErrorMessage));
            break;

        default:
            sp77sprintf (buf, sizeof(buf) - 1, "RTEDiag_OsError: Unknown error type %u", type);
            MSGD((ERR_ERROR_TEXT, buf));
    }

    return;
}


/* For the documentation see header file */
extern  void  RTEDiag_DeleteOldHistories (SAPDB_UInt        numRemainHist,
                                          RTE_DBName      dbname,
                                          RTE_Path        crashHistoryPath)
{
/*===========================================================================*
 *  Locals                                                                   *
 *===========================================================================*/

    static  SAPDB_Char          routine[] = "RTEDiag_DeleteOldHistories";
            RTE_Path            searchFile;
            RTE_Path            tmpPath;
            HISTORY            *pHistory = NULL; 
            SAPDB_UInt          ixHistory = 0;
            RTE_SystemRc        rc = RTE_SYSTEMRC_NO_ERROR;
            BOOL                nextFile = true;
            WIN32_FIND_DATA     fileData;
            SAPDB_UInt          numHist;
            teo15_Handle        hFile;


/*===========================================================================*
 *  Instructions                                                             *
 *===========================================================================*/
    memset (tmpPath, 0, sizeof (tmpPath));
    strcpy (tmpPath, crashHistoryPath);
    memset (tmpPath + strlen (tmpPath), SEPERATOR, 1);
        
    strcpy (searchFile, tmpPath);
    strcat (searchFile, dbname);
    strcat (searchFile, ARBITRARY_FILE);
  
    hFile = FindFirstFile (searchFile, &fileData);
    if (hFile == INVALID_HANDLE_EO15)
    {
        RTEDiag_OsError (routine, "FindFirstFile", searchFile, RTE_UNDEF_ERROR, INFO_TYPE);
        return;
    }

    numHist = numRemainHist + 1;

    /*--- Allocate Memory for the history list 'pHistory' ---*/
    if (ALLOC_MEM_EO57 (&pHistory, numHist * sizeof(HISTORY)) != RTE_SYSTEMRC_NO_ERROR)
    {
        MSGD ((ERR_MALLOC_FAILED_FOR ,"Diagnose history list"));
    }
    
    
    /* Collecting the histories which should remain on disk */
    if (numRemainHist > 0)  /* Of course, this is only to be done if at least one history should remain */
    {
        do
        {
            if (0 != (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            {
                if (RTEDiag_GetDateTimeFromHistory (dbname, fileData.cFileName, &pHistory[ixHistory].gentime))
                {
                    strcpy (pHistory[ixHistory].path, tmpPath);
                    strcat (pHistory[ixHistory].path, fileData.cFileName);
                    ++ixHistory;
                }
            }
        }
        while ((nextFile = FindNextFile (hFile, &fileData)) && ixHistory < numRemainHist);
    }

    if (!nextFile)
    {
        rc = GetLastError ();
        if (rc != ERROR_SUCCESS && rc != ERROR_NO_MORE_FILES)
        {
            RTEDiag_OsError (routine, "FindNextFile", searchFile, rc, ERR_TYPE);
        }

        if (!FindClose (hFile))
        {
            RTEDiag_OsError (routine, "FindClose", searchFile, RTE_UNDEF_ERROR, ERR_TYPE);
        }

        if ((rc = FREE_MEM_EO57 (pHistory)) != RTE_SYSTEMRC_NO_ERROR)
        {
            MSGD ((ERR_CANT_FREE_MEMORY ,rc));
        }
        return;
    }

    /* ixHistory == numRemainHist and next file found */
    do
    {
        if (0 != (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
        {
            /* The following conversion of the date and time makes even sense for numHist = 1 because  */
            /* there also takes place an check of the rigth format of the directory name. Therewith we */
            /* can be sure to really delete an history directory                                       */
            if (RTEDiag_GetDateTimeFromHistory (dbname, fileData.cFileName, &pHistory[numRemainHist].gentime))
            {
                strcpy (pHistory[numRemainHist].path, tmpPath);
                strcat (pHistory[numRemainHist].path, fileData.cFileName);
                if (numHist > 1) /* we need no sorting for one element */
                {
                    /* Since the first numHist-1 elements are already sorted it may be not the      */
                    /* quickest way of sorting here. But at this point it surely does not matter.   */
                    qsort (pHistory, numHist, sizeof(HISTORY), RTEDiag_CompareHistDate);
                }

                /* delete the oldest history in the list */
                RTEDiag_DeleteDirectory (pHistory[numRemainHist].path);
            }
        }
    }
    while (FindNextFile (hFile, &fileData));

    rc = GetLastError ();
    if (rc != ERROR_SUCCESS && rc != ERROR_NO_MORE_FILES)
    {
        RTEDiag_OsError (routine, "FindNextFile", searchFile, rc, ERR_TYPE);
    }

    if (!FindClose (hFile))
    {
        RTEDiag_OsError (routine, "FindClose", searchFile, RTE_UNDEF_ERROR, ERR_TYPE);
    }

    if ((rc = FREE_MEM_EO57 (pHistory)) != RTE_SYSTEMRC_NO_ERROR)
    {
        MSGD ((ERR_CANT_FREE_MEMORY, rc));
    }
    return;
}

/***************************************************************/
#else /* UNIX, LINUX */
/***************************************************************/
/* For the documentation see header file */
extern void RTEDiag_OsError    (SAPDB_Char     *routine, 
                                SAPDB_Char     *callFunction,
                                SAPDB_Char     *zusText,
                                RTE_SystemRc    dummy,
                                SAPDB_UInt      type) 
{
/*===========================================================================*
 *  Locals                                                                   *
 *===========================================================================*/
    SAPDB_Char        lastErrorMessage[LAST_ERROR_MESSAGE_SIZE];
    SAPDB_Char        buf[ERROR_TEXT_LEN];

/*===========================================================================*
 *  Instructions                                                             *
 *===========================================================================*/
    dummy = dummy;

    sp77sprintf (lastErrorMessage, sizeof(lastErrorMessage), "[%d] %s, %s", 
                 errno, sqlerrs(), zusText == NULL ? "" : zusText);

    switch (type)
    {
        case INFO_TYPE:
            MSGD((INFO_DIAG_HIST_OS_ERROR, routine, callFunction == NULL ? "" : callFunction, lastErrorMessage));
            break;

        case ERR_TYPE:
            MSGD((ERR_DIAG_HIST_OS_ERROR, routine, callFunction == NULL ? "" : callFunction, lastErrorMessage));
            break;

        default:
            sp77sprintf (buf, sizeof(buf) - 1, "RTEDiag_OsError: Unknown error type %u", type);
            MSGD((ERR_ERROR_TEXT, buf));
    }

    return;
}

/* For the documentation see header file */
extern  void  RTEDiag_DeleteOldHistories (SAPDB_UInt       numRemainHist,
                                          RTE_DBName     dbname,
                                          RTE_Path       crashHistoryPath)
{
/*===========================================================================*
 *  Locals                                                                   *
 *===========================================================================*/

    static  SAPDB_Char          routine[] = "RTEDiag_DeleteOldHistories";
            RTE_Path          tmpPath;
            SAPDB_UInt          numHist;
            DIR                *hFile = NULL;
            struct dirent      *pfileData = NULL;
            size_t              len_dbname;
            HISTORY            *pHistory = NULL;
            SAPDB_UInt          ixHistory = 0;
            RTE_SystemRc        rc = RTE_SYSTEMRC_NO_ERROR;
            struct stat         fileInfo;
            RTE_Path          histDirectory;

/*===========================================================================*
 *  Instructions                                                             *
 *===========================================================================*/
    memset (tmpPath, 0, sizeof (tmpPath));
    strcpy (tmpPath, crashHistoryPath);
    memset (tmpPath + strlen (tmpPath), SEPERATOR, 1);

    len_dbname = strlen (dbname);

    hFile = opendir(tmpPath);

    if (NULL == hFile)
    {
        RTEDiag_OsError (routine, "opendir", crashHistoryPath, RTE_UNDEF_ERROR, INFO_TYPE);
        return;
    }

    numHist = numRemainHist + 1;

    /*--- Allocate Memory for the history list 'pHistory' ---*/
    if (ALLOC_MEM_EO57 ((void**)(&pHistory), numHist * sizeof(HISTORY)) != RTE_SYSTEMRC_NO_ERROR)
    {                   
        MSGD ((ERR_MALLOC_FAILED_FOR ,"Diagnose history list"));
    }

    errno = RTE_SYSTEMRC_NO_ERROR;
    /* Collecting the histories which should remain on disk */
    if (numRemainHist > 0)  /* Of course, this is only to be done if at least one history should remain */
    {
        do
        {
            errno = RTE_SYSTEMRC_NO_ERROR;
            pfileData = readdir(hFile);
            if (pfileData != NULL && strlen (pfileData->d_name) >= len_dbname 
                && !strncmp (pfileData->d_name, dbname, len_dbname)) 
            {
                strcpy (histDirectory, tmpPath);
                strcat (histDirectory, pfileData->d_name);
                if (lstat(histDirectory, &fileInfo) != 0)
                {
                    RTEDiag_OsError (routine, "lstat", histDirectory, RTE_UNDEF_ERROR, ERR_TYPE);
                    return;
                }

                if (S_ISDIR(fileInfo.st_mode))
                {
                    if (RTEDiag_GetDateTimeFromHistory (dbname, pfileData->d_name, &pHistory[ixHistory].gentime))
                    {
                        strcpy (pHistory[ixHistory].path, histDirectory);
                        ++ixHistory;
                    }
                }
            }
        }
        while (pfileData != NULL && ixHistory < numRemainHist);
    }

    if (RTE_SYSTEMRC_NO_ERROR != errno)
    {
        RTEDiag_OsError (routine, "readdir", "", RTE_UNDEF_ERROR, ERR_TYPE);
        if (closedir(hFile) != 0)
        {
            RTEDiag_OsError (routine, "closedir", crashHistoryPath, RTE_UNDEF_ERROR, INFO_TYPE);
        }

        if ((rc = FREE_MEM_EO57 (pHistory)) != RTE_SYSTEMRC_NO_ERROR)
        {
            MSGD ((ERR_CANT_FREE_MEMORY, rc));
        }
        return;
    }

    if (numRemainHist > 0 && pfileData == NULL)
    {
        if (closedir(hFile) != 0)
        {
            RTEDiag_OsError (routine, "closedir", crashHistoryPath, RTE_UNDEF_ERROR, INFO_TYPE);
        }

        if ((rc = FREE_MEM_EO57 (pHistory)) != RTE_SYSTEMRC_NO_ERROR)
        {
            MSGD ((ERR_CANT_FREE_MEMORY, rc));
        }
        return;
    }


    /* ixHistory == numRemainHist and next file found */
    do
    {
        errno = RTE_SYSTEMRC_NO_ERROR;
        pfileData = readdir(hFile);
        if (pfileData != NULL && strlen (pfileData->d_name) >= len_dbname 
            && !strncmp (pfileData->d_name, dbname, len_dbname)) 
        {
            strcpy (histDirectory, tmpPath);
            strcat (histDirectory, pfileData->d_name);
            if (lstat(histDirectory, &fileInfo) != 0)
            {
                RTEDiag_OsError (routine, "lstat", histDirectory, RTE_UNDEF_ERROR, ERR_TYPE);
                return;
            }

            if (S_ISDIR(fileInfo.st_mode))
            {
                /* The following conversion of the date and time makes even sense for numHist = 1 because  */
                /* there also takes place an check of the rigth format of the directory name. Therewith we */
                /* can be sure to really delete an history directory                                       */
                if (RTEDiag_GetDateTimeFromHistory (dbname, pfileData->d_name, &pHistory[numRemainHist].gentime))
                {
                    strcpy (pHistory[numRemainHist].path, histDirectory);
                    if (numHist > 1) /* we need no sorting for one element */
                    {
                        /* Since the first numHist-1 elements are already sorted it may be not the              */
                        /* quickest way of sorting at this point. But at this point it surely does not matter.  */
                        qsort (pHistory, numHist, sizeof(HISTORY), RTEDiag_CompareHistDate);
                    }

                    /* delete the oldest history in the list */
                    RTEDiag_DeleteDirectory (pHistory[numRemainHist].path);
                }
            }
        }
    }
    while (pfileData != NULL);

    if (RTE_SYSTEMRC_NO_ERROR != errno)
    {
        RTEDiag_OsError (routine, "readdir", "", RTE_UNDEF_ERROR, ERR_TYPE);
        if (closedir(hFile) != 0)
        {
            RTEDiag_OsError (routine, "closedir", crashHistoryPath, RTE_UNDEF_ERROR, INFO_TYPE);
        }
    }

    if ((rc = FREE_MEM_EO57 (pHistory)) != RTE_SYSTEMRC_NO_ERROR)
    {
        MSGD ((ERR_CANT_FREE_MEMORY, rc));
    }
    return;
}


/*!
  EndChapter: UNIX only
 */ 

/***************************************************************/
#endif
/***************************************************************/

/*===========================================================================*
 *  STATIC FUNCTIONS                                                         *
 *===========================================================================*/

/*!
  Chapter: RTE Diagnostic (Common, NT and UNIX)
 */
/*!
  Function:     RTEDiag_CompareHistDate (static)
  description:  Input function for qsort. Compares the time of two elements of type HISTORY.
                .
  arguments:
    history1    [in]    Pointer to a HISTORY-Element 
    history2    [in]    Pointer to a HISTORY-Element 


  return value: -1      history1 > history2
                 0      history1 = history2
                 1      history1 < history2
*/
static  int  RTEDiag_CompareHistDate    (const void  *history1,
                                         const void  *history2) 
{
/*===========================================================================*
 *  Locals                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  Instructions                                                             *
 *===========================================================================*/
    if (((HISTORY *)history1)->gentime > ((HISTORY *)history2)->gentime)
    {
        return (-1);
    }
    else if (((HISTORY *)history1)->gentime < ((HISTORY *)history2)->gentime)
    {
        return (1);
    }
    
    return (0);
}


/*!
  Function:     RTEDiag_GetDateTimeFromHistory (static)
  description:  The function converts the date and time of a history name of the following format
                    
                      <dbname>_YYYYMMDD_HH-MM-SS

                into a value of type time_t.
                .
  arguments:
    dbname      [in]    Name of the database
    history     [in]    Pointer to the name of the history directory
    p_time      [out]   Pointer to converted date and time

  return value:     true    Format of the history is correct and conversion succeeded
                    false   Conversion failed
*/
static  SAPDB_Bool  RTEDiag_GetDateTimeFromHistory    (RTE_DBName         dbname,
                                                       SAPDB_Char          *history,
                                                       time_t              *p_time) 
{
/*===========================================================================*
 *  Locals                                                                   *
 *===========================================================================*/
    struct tm       timestruct;
    SAPDB_Char     *phistory;
    SAPDB_Char      buf[HIST_LEN_YEAR + 1];

#define     OFFSET_YEAR                 1900    /* Offset is needed to determinate local time */
#define     OFFSET_MONTH                1

#define     PUTDATETIMEVALUE(dt_elem, dt_len, dt_value, dt_off)             \  
            {                                                               \    
                    size_t          offset = 0;                             \
                    size_t          length = 0;                             \
                                                                            \
                                                                            \
                    offset = strcspn (HIST_NAME_SKEL, (dt_elem));           \
                    length = strspn (HIST_NAME_SKEL + offset, (dt_elem));   \ 
                    if  (length != (dt_len))                                \
                    {                                                       \
                        return (false);                                     \
                    }                                                       \   
                                                                            \
                    if (offset <= strlen (HIST_NAME_SKEL) - (dt_len))       \
                    {                                                       \
                        SAPDB_memcpy (buf, phistory + offset, (dt_len));          \
                        buf[(dt_len)] = '\0';                               \
                        (dt_value) = atoi(buf) - (dt_off);                  \
                    }                                                       \
                    else                                                    \
                    {                                                       \
                        return (false);                                     \
                    }                                                       \
            }                                                               \    

        

/*===========================================================================*
 *  Instructions                                                             *
 *===========================================================================*/
    memset (buf, '\0', sizeof (buf));
    memset (&timestruct, 0, sizeof (timestruct));

    phistory = history + strlen (dbname);

    /* Checking the length of the History name */
    if (strlen (phistory) != strlen (HIST_NAME_SKEL))
    {
        return (false);
    }

    PUTDATETIMEVALUE (HIST_YEAR, HIST_LEN_YEAR,     timestruct.tm_year,     OFFSET_YEAR);
    PUTDATETIMEVALUE (HIST_MONTH, HIST_LEN_MONTH,   timestruct.tm_mon,      OFFSET_MONTH);
    PUTDATETIMEVALUE (HIST_DAY, HIST_LEN_DAY,       timestruct.tm_mday,     0);
    PUTDATETIMEVALUE (HIST_HOUR, HIST_LEN_HOUR,     timestruct.tm_hour,     0);
    PUTDATETIMEVALUE (HIST_MINUTE, HIST_LEN_MINUTE, timestruct.tm_min,      0);
    PUTDATETIMEVALUE (HIST_SECOND, HIST_LEN_SECOND, timestruct.tm_sec,      0);

    *p_time = mktime (&timestruct);

    if ((time_t) -1 == *p_time)
    {
        MSGD ((ERR_TIME_CONV, history));
        return (false);
    }

    return (true);
}

/*!
  EndChapter: RTE Diagnostic
 */

/***************************************************************/
#if defined(_WIN32)
/***************************************************************/
/*!
  Chapter: NT only 
 */

/*!
  Function:     RTEDiag_DeleteDirectory (static)
  description:  The specified directory is deleted. All files and subdirectories
                are beeing deleted also.
                .
  arguments:
    path    [in]    Pathname of the directory to be deleted


  return value: none 
  
*/
static  void  RTEDiag_DeleteDirectory (SAPDB_Char *path)
{
/*===========================================================================*
 *  Locals                                                                   *
 *===========================================================================*/
    static  SAPDB_Char          routine[] = "RTEDiag_DeleteDirectory";
            teo15_Handle        hFile;
            WIN32_FIND_DATA     fileData;
            RTE_Path          searchFile;
            RTE_Path          tmpPath;
            RTE_Path          delFile;
            RTE_SystemRc        rc = RTE_SYSTEMRC_NO_ERROR;

/*===========================================================================*
 *  Instructions                                                             *
 *===========================================================================*/
  memset (tmpPath, 0, sizeof (tmpPath));
  strcpy (tmpPath, path);
  memset (tmpPath + strlen (tmpPath), SEPERATOR, 1);
  
  strcpy (searchFile, tmpPath);
  strcat (searchFile, ARBITRARY_FILE);
  hFile = FindFirstFile (searchFile, &fileData);
  if (hFile == INVALID_HANDLE_EO15)
  {
    RTEDiag_OsError (routine, "FindFirstFile", path, RTE_UNDEF_ERROR, INFO_TYPE);
    return;
  }
  
  do
  {
    strcpy (delFile, tmpPath);
    strcat (delFile, fileData.cFileName);
    if (0 != (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
    {
      /* Ignore '.' and '..' */
      if (strcmp (fileData.cFileName, CURRENT_DIRECTORY) &&  
          strcmp (fileData.cFileName, PARENT_DIRECTORY))
      {
        RTEDiag_DeleteDirectory (delFile);
      }
    }
    else
    {
      if(!DeleteFile(delFile))
      {
          RTEDiag_OsError (routine, "DeleteFile", fileData.cFileName, RTE_UNDEF_ERROR, ERR_TYPE);
      }
    }
  }
  while (FindNextFile (hFile, &fileData));

  rc = GetLastError ();
  if (rc != ERROR_SUCCESS && rc != ERROR_NO_MORE_FILES)
  {
      RTEDiag_OsError (routine, "FindNextFile", path, rc, ERR_TYPE);
  }

  if (!FindClose (hFile))
  {
      RTEDiag_OsError (routine, "FindClose", path, RTE_UNDEF_ERROR, ERR_TYPE);
  }

  if(!RemoveDirectory (path))
  {
      RTEDiag_OsError (routine, "RemoveDirectory", path, RTE_UNDEF_ERROR, ERR_TYPE);
  }

  return;
}


/*!
  EndChapter: NT only
 */

/***************************************************************/
#else /* UNIX, LINUX */
/***************************************************************/
/*!
  Chapter: UNIX only
 */

/*!
  Function:     RTEDiag_DeleteDirectory (static)
  description:  The specified directory is deleted. All files and subdirectories
                are beeing deleted also (symbolic links are beeing ignored).
                .
  arguments:
    path    [in]    Pathname of the directory to be deleted


  return value: none 
  
*/
static  void  RTEDiag_DeleteDirectory (SAPDB_Char *path)
{
/*===========================================================================*
 *  Locals                                                                   *
 *===========================================================================*/
    static  SAPDB_Char      routine[] = "RTEDiag_DeleteDirectory";
            DIR            *hFile = NULL;
            struct dirent  *pfileData = NULL;
            RTE_Path      delFile;
            RTE_Path      tmpPath;
            struct stat     fileInfo;

/*===========================================================================*
 *  Instructions                                                             *
 *===========================================================================*/
    memset (tmpPath, 0, sizeof (tmpPath));
    strcpy (tmpPath, path);
    memset (tmpPath + strlen (tmpPath), SEPERATOR, 1);

    hFile = opendir(path);

    if (NULL == hFile)
    {
        RTEDiag_OsError (routine, "opendir", path, RTE_UNDEF_ERROR, INFO_TYPE);
        return;
    }

    do
    {
        errno = RTE_SYSTEMRC_NO_ERROR;
        pfileData = readdir(hFile);
        if (pfileData != NULL) 
        {
            strcpy (delFile, tmpPath);
            strcat (delFile, pfileData->d_name);
            if (lstat(delFile, &fileInfo) != 0)
            {
                RTEDiag_OsError (routine, "lstat", delFile, RTE_UNDEF_ERROR, ERR_TYPE);
                
                if (closedir(hFile) != 0)
                {
                    RTEDiag_OsError (routine, "closedir", path, RTE_UNDEF_ERROR, INFO_TYPE);
                }
                return;
            }

            if (S_ISDIR(fileInfo.st_mode))
            {
                /* Ignore '.' and '..' */
                if (strcmp (pfileData->d_name, CURRENT_DIRECTORY) &&  
                    strcmp (pfileData->d_name, PARENT_DIRECTORY))
                {
                    RTEDiag_DeleteDirectory (delFile);
                }
            }
            else if (S_ISLNK(fileInfo.st_mode))
            {   /* Ignore symbolic links */
                ; 
            }
            else
            {
                if (unlink (delFile) != 0)
                {
                    RTEDiag_OsError (routine, "unlink", delFile, RTE_UNDEF_ERROR, ERR_TYPE);
                    if (closedir(hFile) != 0)
                    {
                        RTEDiag_OsError (routine, "closedir", path, RTE_UNDEF_ERROR, INFO_TYPE);
                    }
                    return;                
                }
            }
        }
    }
    while (pfileData != NULL);

    if (RTE_SYSTEMRC_NO_ERROR != errno)
    {
        RTEDiag_OsError (routine, "readdir", "", RTE_UNDEF_ERROR, ERR_TYPE);
        if (closedir(hFile) != 0)
        {
            RTEDiag_OsError (routine, "closedir", path, RTE_UNDEF_ERROR, INFO_TYPE);
        }
        return;
    }

    if (rmdir(path) != 0)
    {
        RTEDiag_OsError (routine, "rmdir", path, RTE_UNDEF_ERROR, ERR_TYPE);
    }

    if (closedir(hFile) != 0)
    {
        RTEDiag_OsError (routine, "closedir", path, RTE_UNDEF_ERROR, INFO_TYPE);
    }

    return;
}

/*!
  EndChapter: UNIX only
 */ 

/***************************************************************/
#endif
/***************************************************************/

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
