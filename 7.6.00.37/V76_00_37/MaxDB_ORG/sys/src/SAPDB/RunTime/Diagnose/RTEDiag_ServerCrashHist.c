/*!***************************************************************************

  module      : RTEDiag_ServerCrashHist.c

  -------------------------------------------------------------------------

  responsible : StefanP

  special area: Diagnostic files
  description : Saving of diagnostic files at restart if the database
                was shutting down abnormally the last time.
                Functions for the server site.

  last changed: 2001-02-07  14:46
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
#include    "RunTime/RTE_Types.h"
#include    "gsp00.h"
#include    "geo00.h"
#include    "geo00_2.h"
#include    "geo002.h"
#include    "geo007_1.h"
#include    "heo11.h"
#include    "RunTime/Diagnose/RTEDiag_ServerCrashHist.h"
#include    "RunTime/Diagnose/RTEDiag_CommonCrashHist.h"
#include    "RunTime/System/RTESys_IO.h"
#include    "heo47.h"
#include    "heo52.h"
#include    "geo50_0.h"
#include    "hsp77.h"
#include    "geo57.h"
/*#include    "cn/zlib/zlib.h"*/

#include "KernelCommon/ParameterNames/KernelParam_RunTime.h"
#include "RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.h"

#include "SAPDBCommon/SAPDB_Names.h"
#include  "SAPDBCommon/SAPDB_string.h"

#if defined (_WIN32)    /*&if $OSSPEC = WIN32*/
#include  "gos00.h"     /* nocheck */
#else                   /*&endif*/
#include <sys/types.h>
#include <dirent.h>
#endif


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

/*--- Table of diagnostic files to be saved ---*/
/* PTS 1112247 */
typedef struct
{
  SAPDB_UTF8         *xparamFilename;
  RTE_Filename        diagFilename;
  RTE_Filename        extension;
  SAPDB_Bool          movefile;       /* true: move file    false: copy file */ /* 1110186 */
/*  SAPDB_Bool          compress; */
} DIAGNOSTIC_FILE;

static  DIAGNOSTIC_FILE   diag_file_table[] =
{
    {(SAPDB_UTF8*)"\0",             SAPDB_DIAG_FILE_NAME,           "\0",                                       false},  /*--- KNLDIAG  */
    {(SAPDB_UTF8*)"\0",             SAPDB_DIAG_FILE_NAME,           SAPDB_OUTPUTFILE_STARTING_PHASE_SUFFIX,     false},  /*--- KNLDIAG.start  */
    {(SAPDB_UTF8*)"\0",             SAPDB_DIAG_FILE_NAME,           SAPDB_OUTPUTFILE_RUNNING_PHASE_SUFFIX,      false},  /*--- KNLDIAG.run  */
    {(SAPDB_UTF8*)"\0",             SAPDB_DIAG_FILE_NAME,           SAPDB_OUTPUTFILE_TERMINATING_PHASE_SUFFIX,  false},  /*--- KNLDIAG.old  */
    {KERNELPARAM__KERNELTRACEFILE,  "\0",                           "\0",                                       false},  /*--- KNLTRACE */
    {KERNELPARAM__RTEDUMPFILE,      "\0",                           "\0",                                       true },  /*--- RTEDUMP  */
    {KERNELPARAM__KERNELDUMPFILE,   "\0",                           "\0",                                       true },  /*--- KNLDUMP  */
    {(SAPDB_UTF8*)"\0",             SAPDB_COMMUNICATION_DUMP_NAME,  "\0",                                       true },  /*--- Communication dump */
    {(SAPDB_UTF8*)"\0",             SAPDB_AK_CACHE_DUMP_NAME,       "\0",                                       true },
    {(SAPDB_UTF8*)"\0",             SAPDB_AK_BUFFER_DUMP_NAME,      "\0",                                       true },
    {(SAPDB_UTF8*)"\0",             SAPDB_AK_STATEMENT_DUMP_NAME,   "\0",                                       true },
#if defined(_WIN32)
    {(SAPDB_UTF8*)"\0",             SAPDB_KNL_MINI_DUMP,            "\0",                                       true }   /*--- Windows mini dump */
#else
    {(SAPDB_UTF8*)"\0",             "core",                         "\0",                                       true }   /*--- UNIX DB-core */  /* PTS 1109821 */
#endif
};

#define NUM_DIAG_FILES 	(sizeof(diag_file_table)/sizeof(DIAGNOSTIC_FILE))



/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/


static  void                RTEDiag_SaveDiagFiles           (SAPDB_Char        *destPath,
                                                             RTE_XPARAM_REC    *p_xparam);

static  SAPDB_Bool          RTEDiag_GenerateNewHistDir      (RTE_Filename       dirname,
                                                             RTE_DBName         dbname);

static  void                RTEDiag_MakeBackup              (SAPDB_Char        *source,
                                                             SAPDB_Char        *dest,
                                                             SAPDB_Bool         movefile); /* PTS 1110186 */
/*
                                                             SAPDB_Bool         compress);
*/

static  RTE_SystemRc        RTEDiag_CreateDirectory         (RTE_Path           directory);

static  SAPDB_Char         *RTEDiag_ExtractFilename         (RTE_Path           relPath);
/*
static  SAPDB_Bool          RTEDiag_CopyCompressFile        (SAPDB_Char        *source,
                                                             SAPDB_Char        *target);
*/

/*===========================================================================*
 *  EXTERN FUNCTIONS                                                         *
 *===========================================================================*/

/* For the documentation see header file */
extern	void	RTEDiag_SetCrashSem (SAPDB_UInt       state,
                                     RTE_DBName     dbname)
{
/*===========================================================================*
 *  Locals                                                                   *
 *===========================================================================*/
  SAPDB_Char *errText;
/*===========================================================================*
 *  Instructions                                                             *
 *===========================================================================*/
  if(!RTEConf_ParameterAccessSetCrashSem(state,&errText))
  {
    if (CRASH_SEM_RESET == state)
    {
      MSGD ((ERR_DIAG_HIST_SEM, "Resetting", errText, 0));
    }
    else
    {
      MSGD ((ERR_DIAG_HIST_SEM, "Posting", errText, 0));
    }
  }

  return;
}

/* For the documentation see header file */
extern  void  RTEDiag_MakeCrashHist (RTE_XPARAM_REC   *p_xparam,
                                     RTE_DBName      dbname)
{
/*===========================================================================*
 *  Locals                                                                   *
 *===========================================================================*/
    static  SAPDB_Char          routine[] = "RTEDiag_MakeCrashHist";
            RTE_Path            newHistPath;
            RTE_Filename        newHistDirname;
            RTE_SystemRc        rc = RTE_SYSTEMRC_NO_ERROR;

/*===========================================================================*
 *  Instructions                                                             *
 *===========================================================================*/
/*  if (p_xparam->uCrashSem == CRASH_SEM_RESET) */
  if (!RTEConf_ParameterAccessGetCrashSem())
    return;

  if (p_xparam->uCrashHistoryNum == 0)
  {
      MSGD ((WRN_DIAG_HIST_DEACTIVATED));
      return;
  }

  MSGD ((INFO_DIAG_HIST_START));

  rc = RTEDiag_CreateDirectory (p_xparam->szCrashHistoryPath);
  if (rc != RTE_SYSTEMRC_NO_ERROR && rc != RTE_SYSTEMRC_ALREADY_EXISTS)
  {
      RTEDiag_OsError (routine, "RTEDiag_CreateDirectory", p_xparam->szCrashHistoryPath, rc, ERR_TYPE);
      return;
  }

  /* Caution: uCrashHistoryNum is always > 0. This is checked by cserv.pcf */
  RTEDiag_DeleteOldHistories (p_xparam->uCrashHistoryNum - 1, dbname, p_xparam->szCrashHistoryPath);

  /*--- Generate new history path */
  if (!RTEDiag_GenerateNewHistDir (newHistDirname, dbname))
  {
    MSGD ((ERR_DIAG_HIST_DIRNAME));
    return;
  }

  memset (newHistPath, 0, sizeof (newHistPath));
  strcpy (newHistPath, p_xparam->szCrashHistoryPath);
  memset (newHistPath + strlen(newHistPath), SEPERATOR, 1);
  strcat (newHistPath, newHistDirname);


  rc = RTEDiag_CreateDirectory (newHistPath);
  if (rc != RTE_SYSTEMRC_NO_ERROR && rc != RTE_SYSTEMRC_ALREADY_EXISTS)
  {
      RTEDiag_OsError (routine, "RTEDiag_CreateDirectory", newHistPath, rc, ERR_TYPE);
      return;
  }

  /*--- Save diagnostic files */
  RTEDiag_SaveDiagFiles (newHistPath, p_xparam);

  MSGD ((INFO_DIAG_HIST_STOP));
  return;
}


/*===========================================================================*
 *  STATIC FUNCTIONS                                                         *
 *===========================================================================*/
/*!
  Chapter: RTE Diagnostic (Server, NT and UNIX)
 */
/*!
  Function:     RTEDiag_SaveDiagFiles (static)
  description:  This function executes the backup of the diagnostic files
                to be saved.

  arguments:
    path       [in] Path to the directory where the diagnostic files have to be saved
    p_xparam   [in] Pointer to XPARAM-Record

  return value: none
*/

static  void  RTEDiag_SaveDiagFiles (SAPDB_Char       *path,
                                     RTE_XPARAM_REC   *p_xparam)
{
/*===========================================================================*
 *  Locals                                                                   *
 *===========================================================================*/
  RTE_Filename    destFilename;
  RTE_Path        sourcePath;
  RTE_Path        destPath;
  SAPDB_UInt      ix;
  SAPDB_Char     *pfilename;

/*===========================================================================*
 *  Instructions                                                             *
 *===========================================================================*/

  for (ix = 0; ix < NUM_DIAG_FILES; ix++)
  {
    memset (sourcePath, 0, sizeof (sourcePath));
    memset (destPath, 0, sizeof (destPath));

    /*--- Create path and filename of the file to be saved */
    if (*diag_file_table[ix].xparamFilename != '\0')
    {/*--- Take filename from XParam */
      SAPDB_Char *errText;
      if(RTEConf_ParameterAccessGetString(diag_file_table[ix].xparamFilename , &pfilename , &errText))
      {
        /* PTS 1112247 */
        strcpy (sourcePath, pfilename);
        strcpy (destFilename, RTEDiag_ExtractFilename (sourcePath));
      }
      else
      {
        MSGD ((ERR_DIAG_HIST_GET_FILENAME, diag_file_table[ix].xparamFilename));
        continue;
      }

    }
    else /*--- Take filename from table */
    {
      /* PTS 1112247 */
      strcpy (sourcePath, diag_file_table[ix].diagFilename);
      strcpy (destFilename, RTEDiag_ExtractFilename (sourcePath));
      /*--- Special treatment for the KNLDIAG-file */
      if (!strcmp (diag_file_table[ix].diagFilename, SAPDB_DIAG_FILE_NAME))
      {
          strcat (sourcePath, KNLDIAG_EXT_OLD);
      }
    }

    /*--- Create path and filename of the file to be saved to */
    strcpy (destPath, path);
    memset (destPath + strlen(destPath), SEPERATOR, 1);
    strcat (destPath, destFilename);

    if(*diag_file_table[ix].extension != '\0')
    {
        strcat (sourcePath, diag_file_table[ix].extension);
        strcat (destPath, diag_file_table[ix].extension);
    }
    /*--- Make Backup of diagnostic file */
    RTEDiag_MakeBackup (sourcePath, destPath, diag_file_table[ix].movefile); /* 1110186 */
/*    RTEDiag_MakeBackup (sourcePath, destPath, diag_file_table[ix].compress); */

#   if defined (_WIN32)
     // Updating service hint counter to signal we are alive
     UPDATE_SERVICE_OR_CONSOLE_STATE(*kgs.pDBState);
#   endif
  }

  return;
}

/*!
  Function:     RTEDiag_GenerateNewHistDir (static)
  description:  This function executes the backup of the diagnostic files
                to be saved.

  arguments:
    dirname     [out]   Name of new history directory
    dbname      [in]    Name of the database

  return value: true    Generation of new history name succeeded
                false   Generation of new history name failed
*/


static  SAPDB_Bool  RTEDiag_GenerateNewHistDir (RTE_Filename  dirname,
                                                RTE_DBName    dbname)
{
/*===========================================================================*
 *  Locals                                                                   *
 *===========================================================================*/

    RTE_Filename      tmpDirname;
    tsp00_Date          dateValue;
    tsp00_Time          timeValue;

    #define     VDATTIME_OFF_YEAR       0
    #define     VDATTIME_OFF_MONTH      4
    #define     VDATTIME_OFF_DAY        6
    #define     VDATTIME_OFF_HOUR       2
    #define     VDATTIME_OFF_MINUTE     4
    #define     VDATTIME_OFF_SECOND     6

    #define     PUTDATETIME(dt_elem, len, dt, off)                                  \
                {                                                                   \
                    size_t              offset;                                     \
                    size_t              length;                                     \
                                                                                    \
                    offset = strcspn (HIST_NAME_SKEL, (dt_elem));                     \
                    length = strspn (HIST_NAME_SKEL + offset, (dt_elem));             \
                    if  (length != (len))                                           \
                    {                                                               \
                        return (false);                                             \
                    }                                                               \
                                                                                    \
                    if (offset <= strlen (HIST_NAME_SKEL) - (len))                  \
                    {                                                               \
                        SAPDB_memcpy (tmpDirname + offset, (dt) + (off), (len));          \
                    }                                                               \
                    else                                                            \
                    {                                                               \
                        return (false);                                             \
                    }                                                               \
                }                                                                   \


/*===========================================================================*
 *  Instructions                                                             *
 *===========================================================================*/

    strcpy (tmpDirname, HIST_NAME_SKEL);

    vdattime (dateValue, timeValue);  /* dateValue: YYYYMMDD     timeValue:   00HHTTSS */

    PUTDATETIME (HIST_YEAR, HIST_LEN_YEAR, dateValue, VDATTIME_OFF_YEAR);
    PUTDATETIME (HIST_MONTH, HIST_LEN_MONTH, dateValue, VDATTIME_OFF_MONTH);
    PUTDATETIME (HIST_DAY, HIST_LEN_DAY, dateValue, VDATTIME_OFF_DAY);
    PUTDATETIME (HIST_HOUR, HIST_LEN_HOUR, timeValue, VDATTIME_OFF_HOUR);
    PUTDATETIME (HIST_MINUTE, HIST_LEN_MINUTE, timeValue, VDATTIME_OFF_MINUTE);
    PUTDATETIME (HIST_SECOND, HIST_LEN_SECOND, timeValue, VDATTIME_OFF_SECOND);

    strcpy (dirname, dbname);
    strcat (dirname, tmpDirname);

    return (true);
}


/*!
  Function:     RTEDiag_ExtractFilename (static)
  description:  Extracts the filename of a given full or relative filename

  arguments:    Full or relative filename, maybe empty string

  return value: Extracted filename, maybe empty string
*/


static  SAPDB_Char    *RTEDiag_ExtractFilename (RTE_Path      relPath)
{
/*===========================================================================*
 *  Locals                                                                   *
 *===========================================================================*/
    SAPDB_ULong        ix;
/*===========================================================================*
 *  Instructions                                                             *
 *===========================================================================*/
    for (ix = strlen (relPath); ix > 0; --ix)
    {
        if (relPath[ix - 1] == SEPERATOR)
        {
            break;
        }
    }

    return (relPath + ix);
}



/*
  Function:     RTEDiag_CopyCompressFile (static)
  description:

  arguments:
    source      [in]    Name of the source file (full pathname)
    dest        [in]    Name of the destination file (full pathname)

  return value: none


static  SAPDB_Bool  RTEDiag_CopyCompressFile    (SAPDB_Char  *source,
                                                 SAPDB_Char  *target)
{
 *===========================================================================*
 *  Locals                                                                   *
 *===========================================================================*
    #define GZ_EXTENSION   ".gz"     //In einem zentralen Headerfile ablegen und auch in vcn54.cpp entfernen
    #define COPY_SIZE      ((16*1024)/sizeof(long))

    static  SAPDB_Char            routine[] = "RTEDiag_CopyCompressFile";

            SAPDB_Char           *gzTarget;
            SAPDB_UInt            lenTarget;
            SAPDB_UInt            lenGZExt;
            teo15_Position        bytesRead;
            long                  buffer[COPY_SIZE];
            gzFile                hGZTarget;
            teo15_Handle          hSource;
            tsp00_VfReturn_Param  rcStatus;

 *===========================================================================*
 *  Instructions                                                             *
 *===========================================================================*

    strcpy (gzTarget, target);
    lenTarget = strlen (target);
    lenGZExt = strlen (GZ_EXTENSION);

    * Adding GZ-Extension if necessary *
    if (lenTarget > lenGZExt)
    {
        if (strcmp (gzTarget + (lenTarget - lenGZExt), GZ_EXTENSION) != 0)
        {
            strcat(gzTarget, GZ_EXTENSION);
        }
    }

    eo15_Open (hSource, source, eo15_ReadOnly, false, DEFAULT_SECURITY_EO15, &rcStatus);
    if ( vf_ok == rcStatus )
    {
        * open the target *
        hGZTarget = gzopen (gzTarget, "wb");
        if (hGZTarget == NULL)
        {
            *Fehler*
            return (false);
        }

        do
        {
            eo15_Read (hSource, buffer, COPY_SIZE, &bytesRead, &rcStatus );
            if ( vf_ok == rcStatus )
            {
                * Create GZ-Targetfile *
                if (gzwrite (hGZTarget, buffer, (unsigned int) bytesRead) != bytesRead)
                {
                    *Fehler*
                    gzclose (hGZTarget);
                    return (false);
                }
            }
            else
            {
                RTEDiag_OsError (routine, "eo15_Read", source, RTE_UNDEF_ERROR, ERR_TYPE);
            }
        } while ( vf_ok == rcStatus && 0 != bytesRead );

        * Close source *
        eo15_Close (hSource, &rcStatus);
        if ( vf_ok == rcStatus )
        {
            RTEDiag_OsError (routine, "eo15_Close", source, RTE_UNDEF_ERROR, ERR_TYPE);
        }

        * Close Target *
        if (gzclose (hGZTarget) != Z_OK)
        {
            *Fehler*
            return (false);
        }

        if ( rcStatus == vf_eof && bytesRead == 0 )
        {
            rcStatus = vf_ok;
        }
    }
    else
    {
        RTEDiag_OsError (routine, "eo15_Open", source, RTE_UNDEF_ERROR, ERR_TYPE);
        return (false);

    }

    return (true);
}
*/

/*!
  EndChapter: RTE Diagnostic (Server, NT and UNIX)
 */


/***************************************************************/
#if defined(_WIN32)
/***************************************************************/
/*!
  Chapter: NT only
 */

/*!
  Function:     RTEDiag_CreateDirectory (static)
  description:  A directory is created

  arguments:
    directory   [in]    Name of the directory to be created (full pathname)

  return value: Creating of the directory failed    :   rc of GetLastError
                Creating of the directory succeeded :   RTE_SYSTEMRC_NO_ERROR
*/
static  RTE_SystemRc    RTEDiag_CreateDirectory (RTE_Path   directory)
{
/*===========================================================================*
 *  Locals                                                                   *
 *===========================================================================*/
    RTE_SystemRc            rc = RTE_SYSTEMRC_NO_ERROR;
    SECURITY_ATTRIBUTES     SA;
    ACCESS_RIGHTS_REC       Access;


/*===========================================================================*
 *  Instructions                                                             *
 *===========================================================================*/

    SA.nLength                = sizeof(SA);
    SA.bInheritHandle         = FALSE;
    SA.lpSecurityDescriptor   = NULL;

    memset ( &Access, 0, sizeof (Access) );
    Access.Creator.ulAccess       = SPECIFIC_RIGHTS_ALL | STANDARD_RIGHTS_ALL;
    Access.Creator.ulAceFlags     = CONTAINER_INHERIT_ACE | OBJECT_INHERIT_ACE ;
    Access.Admin.ulAccess         = SPECIFIC_RIGHTS_ALL | STANDARD_RIGHTS_ALL;
    Access.Admin.ulAceFlags       = CONTAINER_INHERIT_ACE | OBJECT_INHERIT_ACE ;
    Access.System.ulAccess        = SPECIFIC_RIGHTS_ALL | STANDARD_RIGHTS_ALL;
    Access.System.ulAceFlags      = CONTAINER_INHERIT_ACE | OBJECT_INHERIT_ACE ;
    Access.SAPDBOp.ulAccess       = SPECIFIC_RIGHTS_ALL | STANDARD_RIGHTS_ALL;
    Access.SAPDBOp.ulAceFlags     = CONTAINER_INHERIT_ACE | OBJECT_INHERIT_ACE ;
    Access.DomSAPDBOp.ulAccess    = SPECIFIC_RIGHTS_ALL | STANDARD_RIGHTS_ALL;
    Access.DomSAPDBOp.ulAceFlags  = CONTAINER_INHERIT_ACE | OBJECT_INHERIT_ACE ;
    Access.BackupOp.ulAccess      = GENERIC_READ;
    Access.BackupOp.ulAceFlags    = CONTAINER_INHERIT_ACE | OBJECT_INHERIT_ACE ;

    rc = sql49c_alloc_and_init_SD (&Access, &SA.lpSecurityDescriptor);

    if (rc == RTE_SYSTEMRC_NO_ERROR)
    {
        if (!CreateDirectory (directory, &SA))
        {
            rc = GetLastError ();
        }

        if (SA.lpSecurityDescriptor)
        {
            FREE_MEM ( SA.lpSecurityDescriptor );
        }
    }

    return (rc);
}

/*!
  Function:     RTEDiag_MakeBackup (static)
  description:  This functions moves a file even if the new destination is on another
                filesystem.

  arguments:
    source      [in]    Name of the source file (full pathname)
    dest        [in]    Name of the destination file (full pathname)
    movefile    [in]    true:   move file
                        false:  copy file


  return value: none

*/
static  void  RTEDiag_MakeBackup (SAPDB_Char  *source,
                                  SAPDB_Char  *dest,
                                  SAPDB_Bool   movefile) /* PTS 1110186 */
/*
                                  SAPDB_Bool   compress)
*/
{
/*===========================================================================*
 *  Locals                                                                   *
 *===========================================================================*/
    static  SAPDB_Char          routine[] = "RTEDiag_MakeBackup";
            RTE_SystemRc        rc = RTE_SYSTEMRC_NO_ERROR;

/*===========================================================================*
 *  Instructions                                                             *
 *===========================================================================*/
    if (movefile)
    {
        if (!MoveFile (source, dest))
        {
            rc = GetLastError ();
            if (ERROR_FILE_NOT_FOUND != rc)
            {
                RTEDiag_OsError (routine, "MoveFile", source, rc, ERR_TYPE);
            }
        }
    }
    else    /* PTS 1110186 */
    {
        if (!CopyFile (source, dest, true))
        {
            rc = GetLastError ();
            if (ERROR_FILE_NOT_FOUND != rc)
            {
                RTEDiag_OsError (routine, "CopyFile", source, rc, ERR_TYPE);
            }
        }

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
  Function:     RTEDiag_CreateDirectory (static)
  description:  A directory is created

  arguments:
    directory   [in]    Name of the directory to be created (full pathname)

  return value: Creating of the directory failed    :   errno
                Creating of the directory succeeded :   RTE_SYSTEMRC_NO_ERROR
*/
static  RTE_SystemRc    RTEDiag_CreateDirectory (RTE_Path    directory)
{
/*===========================================================================*
 *  Locals                                                                   *
 *===========================================================================*/
    RTE_SystemRc       rc = RTE_SYSTEMRC_NO_ERROR;
    int                umask_old;
/*===========================================================================*
 *  Instructions                                                             *
 *===========================================================================*/
    umask_old = umask(0);
    if (mkdir (directory, 0777) != 0)
    {
        rc = errno;
    }

    umask(umask_old);
    return (rc);
}

/*!
  Function:     RTEDiag_MakeBackup (static)
  description:  Moves or copies files. If the new destination is on the same
                filesystem the file is moved otherwise it is copied.
                .

  arguments:
    source      [in]    Name of the source file (full pathname)
    dest        [in]    Name of the destination file (full pathname)
    movefile    [in]    true:   move file
                        false:  copy file


  return value: none

*/
static  void  RTEDiag_MakeBackup (SAPDB_Char  *source,
                                  SAPDB_Char  *dest,
                                  SAPDB_Bool   movefile)    /* PTS 1110186 */
/*
                                  SAPDB_Bool   compress)
*/
{
/*===========================================================================*
 *  Locals                                                                   *
 *===========================================================================*/
    static  SAPDB_Char              routine[] = "RTEDiag_MakeBackup";
            tsp00_VfReturn_Param    rcStatus;
            tRTESys_IOHandle            hFile;


/*===========================================================================*
 *  Instructions                                                             *
 *===========================================================================*/
  /* the following 'open'is only performed to determine if the file exists */
    hFile = open (source, O_RDWR);
    if (hFile != INVALID_HANDLE_RTESYS_IO)
    {
        if (close (hFile) != 0)
        {
            RTEDiag_OsError (routine, "close", source, RTE_UNDEF_ERROR, ERR_TYPE);
            return;
        }


        if (movefile)
        {
            if (link (source, dest) != RTE_SYSTEMRC_NO_ERROR)
            {
                if (EXDEV == errno)   /* dest is located on a different file system */
                {
/*
                if (compress)
                {
                    if (RTEDiag_CopyCompressFile (source, dest))
                    {
                        unlink (source);
                    }
                }
                else
                {
*/
                    RTESys_IOCopyFile ( source, dest, &rcStatus, NULL );
                    if (vf_ok == rcStatus)
                    {
                        unlink (source);
                    }
/*
                }
*/
                }
                else
                {
                     RTEDiag_OsError (routine, "link", source, RTE_UNDEF_ERROR, ERR_TYPE);
                }
            }
            else
            {
                unlink (source);
            }
        }
        else /* PTS 1110186 */
        {
/*
            if (compress)
            {
                RTEDiag_CopyCompressFile (source, dest);
            }
            else
            {
*/
            RTESys_IOCopyFile ( source, dest, &rcStatus, NULL );
/*
            }
*/

        }
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
