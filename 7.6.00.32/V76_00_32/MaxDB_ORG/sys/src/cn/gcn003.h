/*
  -----------------------------------------------------------------------------

  module: gcn003.h

  -----------------------------------------------------------------------------

  responsible:  BerndV, MartinR, TiloH

  special area: DBMServer

  description:  general header for backup/recovery area

  -----------------------------------------------------------------------------



    ========== licence begin  GPL
    Copyright (c) 2000-2006 SAP AG

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
#ifndef _GCN003_H_
#define _GCN003_H_

/* syntax key words */
#define ME_KEY_LOCATION_CN003       "LOCATION"
#define ME_KEY_TYPE_CN003           "TYPE"
#define ME_KEY_SAVETYPE_CN003       "SAVETYPE"
#define ME_KEY_SIZE_CN003           "SIZE"
#define ME_KEY_OVERWRITE_CN003      "OVERWRITE"
#define ME_KEY_BLOCKSIZE_CN003      "BLOCKSIZE"
#define ME_KEY_AUTOLOADER_CN003     "AUTOLOADER"
#define ME_KEY_OSCOMMAND_CN003      "OSCOMMAND"
#define ME_KEY_CREATED_CN003        "CREATED"
#define ME_KEY_MODIFIED_CN003       "MODIFIED"
#define ME_KEY_BACKUPTOOL_CN003     "BACKUPTOOL"

#define ME_KEY_MEDIAID_CN003        "MEDIA_ID"    /* for old media file */
#define ME_KEY_BLANK_CN003          ' '           /* for old media file */
#define ME_KEY_PROPSEP_CN003        '|'           /* for old media file */
#define ME_KEY_PARALLEL_CN003       "PARALLEL"    /* for old media file */

/* special characters and strings */
#define ME_COMMENT_CN003			    '\''
#define ME_OLDCOMMENT_CN003			  '#'
#define ME_NAME_BEGIN_CN003			  '['
#define ME_GROUP_NAME_SEP1_CN003  '\\'
#define ME_GROUP_NAME_SEP2_CN003  '/'
#define ME_NAME_END_CN003		      ']'
#define ME_PROP_EQUAL_CN003		    '='

/* property value keywords */
#define ME_PROP_YES_CN003           "YES"
#define ME_PROP_NO_CN003            "NO"
#define ME_PROP_VERSION_CN003       "VERSION"

/* old property value keywords */
#define ME_OLD_PROP_YES_CN003           "Y"
#define ME_OLD_PROP_NO_CN003            "N"
#define ME_OLD_PROP_VERSION_CN003       "V"

/* type property value keywords */
#define ME_TYPE_TAPE_CN003          "TAPE"
#define ME_TYPE_FILE_CN003          "FILE"
#define ME_TYPE_NORE_CN003          "NOREWIND"
#define ME_TYPE_PIPE_CN003          "PIPE"
#define ME_TYPE_UNKN_CN003          "UNKNOWN"

/* savetype property value keywords */
#define ME_SAVETYPE_DAT_CN003       "DATA"
#define ME_SAVETYPE_PAG_CN003       "PAGES"
#define ME_SAVETYPE_LOG_CN003       "LOG"
#define ME_SAVETYPE_AUT_CN003       "AUTO"
#define ME_SAVETYPE_UNK_CN003       "UNKNOWN"

/* type property value keywords for old media file*/
#define ME_OLD_TYPE_TAPE_CN003      "T"
#define ME_OLD_TYPE_FILE_CN003      "F"
#define ME_OLD_TYPE_NORE_CN003      "N"
#define ME_OLD_TYPE_PIPE_CN003      "P"
#define ME_OLD_TYPE_AUTO_CN003      "A"
#define ME_OLD_TYPE_AUTL_CN003      "L"
#define ME_OLD_TYPE_UNKN_CN003      "U"

/* default values for medium */
#define ME_DEFAULT_NAME_CN003           ""
#define ME_DEFAULT_LOCATION_CN003       ""
#define ME_DEFAULT_TYPE_CN003           ME_TYPE_UNKN_CN003
#define ME_DEFAULT_SAVETYPE_CN003       ME_SAVETYPE_UNK_CN003
#define ME_DEFAULT_SIZE_CN003           "0"
#define ME_DEFAULT_OVERWRITE_CN003      ME_PROP_NO_CN003
#define ME_DEFAULT_BLOCKSIZE_CN003      "8"
#define ME_DEFAULT_AUTOLOADER_CN003     ME_PROP_NO_CN003
#define ME_DEFAULT_OSCMD_CN003          ""
#define ME_DEFAULT_DATE_CN003           "19980101000000"
#define ME_DEFAULT_EXTERN_CN003         ME_EXT_NONE_CN003

#define ME_PROPERTY_NAME_LENGTH_MXCN003 64
#define ME_PROPERTY_VAL_LENGTH_MXCN003  256

#define ME_LINE_LENGTH_MXCN003          4096

#define ME_NAME_LENGTH_MXCN003          64
#define ME_LOCATION_LENGTH_MXCN003      256
#define ME_TYPE_LENGTH_MXCN003          32
#define ME_SIZE_LENGTH_MXCN003          16
#define ME_OVERWRITE_LENGTH_MXCN003     16
#define ME_BLOCKSIZE_LENGTH_MXCN003     16
#define ME_AUTOLOADER_LENGTH_MXCN003    16
#define ME_OSCMD_LENGTH_MXCN003         256
#define ME_SAVETYPE_LENGTH_MXCN003      16
#define ME_DATE_LENGTH_MXCN003          16
#define ME_BACKUPTOOL_LENGTH_MXCN003    16

/* medium structure */
typedef struct tcn003_Medium{
  char                  szName[ME_NAME_LENGTH_MXCN003 + 1];             /* name of medium                */
  char                  szLocation[ME_LOCATION_LENGTH_MXCN003 + 1];     /* location for medium           */
  char                  szType[ME_TYPE_LENGTH_MXCN003 + 1];             /* type of medium                */
  char                  szSaveType[ME_SAVETYPE_LENGTH_MXCN003 + 1];     /* use medium for DATA/PAGES/LOG */
  char                  szSize[ME_SIZE_LENGTH_MXCN003 + 1];             /* max size in pages of medium   */
  char                  szOverwrite[ME_OVERWRITE_LENGTH_MXCN003 + 1];   /* medium can overwrite          */
  char                  szBlocksize[ME_BLOCKSIZE_LENGTH_MXCN003 + 1];   /* blocksize for read/write      */
  char                  szAutoLoader[ME_AUTOLOADER_LENGTH_MXCN003 + 1]; /* is it a autoloder             */
  char                  szOSCommand[ME_OSCMD_LENGTH_MXCN003 + 1];       /* execute os command            */
  char                  szCreated[ME_DATE_LENGTH_MXCN003 + 1];          /* create date                   */
  char                  szModified[ME_DATE_LENGTH_MXCN003 + 1];         /* modify date                   */
  char                  szExternal[ME_BACKUPTOOL_LENGTH_MXCN003 + 1];   /* external backup tool          */

  bool                  bOld;                                       /* medium from old definition   */

  struct tcn003_Medium  * pNext;                                    /* pointer to next medium       */
  struct tcn003_Medium  * pPrev;                                    /* pointer to previous medium   */
} tcn003_Medium;

/* mediagroup structure */
typedef struct tcn003_MediaGroup{
  char                      szName[ME_NAME_LENGTH_MXCN003];         /* name of mediagroup           */
  tcn003_Medium             * pFirst;                               /* pointer to first medium      */
  tcn003_Medium             * pLast;                                /* pointer to last medium       */
  struct tcn003_MediaGroup  * pNext;                                /* pointer to next mediagroup   */
  struct tcn003_MediaGroup  * pPrev;                                /* pointer to previous mediagr. */

  static const char * ExternProperty();
} tcn003_MediaGroup;

/* media structure */
typedef struct tcn003_Media{
  tcn003_MediaGroup     * pFirstGroup;                              /* pointer to first mediagroup  */
  tcn003_MediaGroup     * pLastGroup;                               /* pointer to last mediagroup   */
} tcn003_Media;

/* key words for save/restore statements   */
#define BA_KEY_SAVE_CN003           "SAVE"
#define BA_KEY_RESTORE_CN003        "RESTORE"
#define BA_KEY_STATE_SAVE_CN003     "STATE SAVE"
#define BA_KEY_STATE_RESTORE_CN003  "STATE RESTORE"
#define BA_KEY_CREATE_CN003         "CREATE"
#define BA_KEY_INSTANCE_CN003       "INSTANCE"
#define BA_KEY_WITH_CN003           "WITH"

#define BA_KEY_RESTART_INFO_CN003   "GET RESTART VERSION"
#define BA_KEY_READLABEL_CN003      "READ LABEL"

#define BA_KEY_DATA_CN003           "DATA"
#define BA_KEY_PAGES_CN003          "PAGES"
#define BA_KEY_LOG_CN003            "LOG"
#define BA_KEY_CONFIG_CN003         "CONFIG"
#define BA_KEY_VOLUME_CN003         "VOLUME"

#define BA_KEY_QUICK_CN003          "QUICK"
#define BA_KEY_TO_CN003             "TO"
#define BA_KEY_FROM_CN003           "FROM"

#define BA_KEY_UNTIL_CN003          "UNTIL"
#define BA_KEY_EBIDLONG_CN003       "ExternalBackupID"
#define BA_KEY_EBID_CN003           "EBID"
#define BA_KEY_IBID_CN003           "IBID"
#define BA_KEY_KEY_CN003            "KEY"
#define BA_KEY_LABEL_CN003          "LABEL"
#define BA_KEY_STAGE_CN003          "STAGE"
#define BA_KEY_FILENUMBERLIST_CN003 "FILENUMBERLIST"
#define BA_KEY_FNL_CN003            "FNL"
#define BA_KEY_VERIFY_CN003         "VERIFY"
#define BA_KEY_NOVERIFY_CN003       "NOVERIFY"
#define BA_KEY_REMOVE_CN003         "REMOVE"
#define BA_KEY_KEEP_CN003           "KEEP"

#define BA_KEY_REPLACE_CN003        "REPLACE"
#define BA_KEY_AUTOIGNORE_CN003     "AUTOIGNORE"
#define BA_KEY_IGNORE_CN003         "IGNORE"
#define BA_KEY_CANCEL_CN003         "CANCEL"

#define BE_KEY_COUNT_CN003          "COUNT"

#define BA_KEY_BLOCKSIZE_CN003      "BLOCKSIZE"
#define BA_KEY_CASCADE_CN003        "CASCADE"
#define BA_KEY_NOCHECKPOINT_CN003   "NO CHECKPOINT"
#define BA_KEY_FVERSION_CN003       "FVERSION"
#define BA_KEY_MEDIANAME_CN003      "MEDIANAME"

#define BA_KEY_RECOVERY_CN003       "RECOVERY"
#define BA_KEY_MIGRATION_CN003      "MIGRATION"

#define BA_KEY_BLANK_CN003          " "
#define BA_KEY_QUOTE_CN003          "\'"
#define BA_KEY_COMMA_CN003          ","
#define BA_KEY_DBLQUOTE_CN003       "\""

#define BA_KEY_BAKSLASH_CN003       "\\"
#define BA_KEY_FORSLASH_CN003       "/"

#define BA_KEY_AUTOSAVE_CN003       "AUTOSAVE"
#define BA_KEY_ON_CN003             "ON"
#define BA_KEY_END_CN003            "END"
#define BA_KEY_CANCEL_CN003         "CANCEL"
#define BA_KEY_SHOW_CN003           "SHOW"

#define BA_KEY_TIMEOUTOFF_CN003     "SET TIMEOUT OFF"

#define BA_TYPE_LENGTH_MXCN003      64
#define BA_FILENUM_LENGTH_MXCN003   32
#define BA_UNTIL_LENGTH_MXCN003     32

const int BA_MAXLABELLENGTH_MXCN003=32;

#define BA_FILESPEC_LENGTH_MXCN003  4096
#define BA_COMMAND_LENGTH_MXCN003   5120

#define BA_NEXTMEDIUM_RC_CN003      (-8020)
#define BA_SAVESKIPPED_CN003        (-7075)
#define BA_NOMOREDATA_CN003         (-7076)

#define BA_AUTOSAVE_RC_ON_CN003       0
#define BA_AUTOSAVE_RC_OFF_CN003      100
#define BA_AUTOSAVE_RC_NEXT_CN003     BA_NEXTMEDIUM_RC_CN003


#define BA_AUTOSAVE_RESULT_ON_CN003   "AUTOSAVE IS ON"
#define BA_AUTOSAVE_RESULT_OFF_CN003  "AUTOSAVE IS OFF"
#define BA_AUTOSAVE_RESULT_NEXT_CN003 "NEXT VOLUME REQUIRED"

/* backup result structur */
typedef struct tcn003_BackupResult{
  int   nBackupRC;
  char  szDate[18+1];
  char  szTime[18+1];
  char  szDatabase[18+1];
  char  szServer[64+1];
  char  szKernelVersion[40+1];
  char  szPagesTransfered[10+1];
  char  szPagesLeft[10+1];
  char  szVolumes[10+1];
  char  szMediaName[40+1];
  char  szLocation[256+1];
  char  szErrortext[40+1];
  char  szLabel[14+1];
  char  szIsConsistent[5+1];
  char  szFirstLogPageNo[10+1];
  char  szLastLogPageNo[10+1];
  char  szDBStamp1Date[8+1];
  char  szDBStamp1Time[8+1];
  char  szDBStamp2Date[8+1];
  char  szDBStamp2Time[8+1];
  char  szBDPageCount[10+1];
  char  szDevicesUsed[10+1];
  char  szDatabaseID[132+1];
  char  szMaxUsedDataPage[10+1];
  char  szConvPageCnt[10+1];
  char  szRedoTransRead[10+1];
  char  szRedoTransDone[10+1];
} tcn003_BackupResult;

#define BA_RESULT_PARAM_COUNT_CN003   25

#define BA_RESULT_RC_CN003         "Returncode"

#define BA_RESULT_DATE_CN003       "Date"
#define BA_RESULT_TIME_CN003       "Time"
#define BA_RESULT_DATABASE_CN003   "Database"
#define BA_RESULT_SERVER_CN003     "Server"
#define BA_RESULT_KNLVER_CN003     "Kernel Version"
#define BA_RESULT_PGSTRAN_CN003    "Pages Transferred"
#define BA_RESULT_PGSLEFT_CN003    "Pages Left"
#define BA_RESULT_VOLUMES_CN003    "Volumes"
#define BA_RESULT_MEDNAME_CN003    "Medianame"
#define BA_RESULT_LOCATION_CN003   "Location"
#define BA_RESULT_ERRTEXT_CN003    "Errortext"
#define BA_RESULT_LABEL_CN003      "Label"
#define BA_RESULT_ISCONS_CN003     "Is Consistent"
#define BA_RESULT_FLOGPG_CN003     "First LOG Page"
#define BA_RESULT_LLOGPG_CN003     "Last LOG Page"
#define BA_RESULT_DBSTMD1_CN003    "DB Stamp 1 Date"
#define BA_RESULT_DBSTMT1_CN003    "DB Stamp 1 Time"
#define BA_RESULT_DBSTMD2_CN003    "DB Stamp 2 Date"
#define BA_RESULT_DBSTMT2_CN003    "DB Stamp 2 Time"
#define BA_RESULT_PGCNT_CN003      "Page Count"
#define BA_RESULT_DEVUSE_CN003     "Devices Used"
#define BA_RESULT_DBID_CN003       "Database ID"
#define BA_RESULT_MUSEDATAPG_CN003 "Max Used Data Page"
#define BA_RESULT_CONVPGCNT_CN003  "Converter Page Count"
#define BA_RESULT_REDOTRANSR_CN003 "Redo Transactions Read"
#define BA_RESULT_REDOTRANSD_CN003 "Redo Transactions Done"

#define BA_RESULTKNL_DATE_CN003        "DATE"              
#define BA_RESULTKNL_TIME_CN003        "TIME"              
#define BA_RESULTKNL_DATABASE_CN003    "SERVERDB"          
#define BA_RESULTKNL_SERVER_CN003      "SERVERNODE"        
#define BA_RESULTKNL_KNLVER_CN003      "KERNEL VERSION"    
#define BA_RESULTKNL_PGSTRAN_CN003     "PAGES TRANSFERRED" 
#define BA_RESULTKNL_PGSLEFT_CN003     "PAGES LEFT"        
#define BA_RESULTKNL_VOLUMES_CN003     "NO OF VOLUMES"     
#define BA_RESULTKNL_MEDNAME_CN003     "MEDIA NAME"        
#define BA_RESULTKNL_LOCATION_CN003    "TAPE NAME"         
#define BA_RESULTKNL_ERRTEXT_CN003     "TAPE ERRORTEXT"    
#define BA_RESULTKNL_LABEL_CN003       "TAPE LABEL"        
#define BA_RESULTKNL_ISCONS_CN003      "IS CONSISTENT"     
#define BA_RESULTKNL_FLOGPG_CN003      "FIRST IO SEQUENCE" 
#define BA_RESULTKNL_LLOGPG_CN003      "LAST IO SEQUENCE"  
#define BA_RESULTKNL_DBSTMD1_CN003     "DBSTAMP1 DATE"     
#define BA_RESULTKNL_DBSTMT1_CN003     "DBSTAMP1 TIME"     
#define BA_RESULTKNL_DBSTMD2_CN003     "DBSTAMP2 DATE"     
#define BA_RESULTKNL_DBSTMT2_CN003     "DBSTAMP2 TIME"     
#define BA_RESULTKNL_PGCNT_CN003       "BD PAGE COUNT"     
#define BA_RESULTKNL_DEVUSE_CN003      "TAPEDEVICES USED"  
#define BA_RESULTKNL_DBID_CN003        "DB_IDENT"          
#define BA_RESULTKNL_MUSEDATAPG_CN003  "MAX USED DATA PNO" 
#define BA_RESULTKNL_CONVPGCNT_CN003   "CONV PAGE COUNT"   
#define BA_RESULTKNL_REDOTRANSR_CN003  "REDO TRANS READ"   
#define BA_RESULTKNL_REDOTRANSD_CN003  "REDO TRANS DONE"   

#define BA_RESULT_STAGEFILES_PROCESSED_CN003   "Processed Stage Files"
#define BA_RESULT_STAGEFILES_UNPROCESSED_CN003 "Remaining Stage Files"
#define BA_RESULT_STAGEFILES_DELETED_CN003     "Deleted Stage Files"
#define BA_RESULT_STAGEFILES_BYTES_CN003       "Bytes processed"

/* backup restart info */
typedef struct tcn003_BackupRestartInfo{
  char  FirstIoSequenceNeeded   [ 10+1]; // szUsedLogPage     1.  fixed(10)     FIRST IO SEQUENCE NEEDED   
  char  FirstIoSequenceOnLog    [ 10+1]; // szFirstLogPage    2.  fixed(10)     FIRST IO SEQUENCE ON LOG   
  char  Restartable             [ 10+1]; // szRestartable     3.  fixed(10)     RESTARTABLE                
  char  DbIdentFromRestartrecord[132+1]; // szDBIdentRestart  4.  char_asc(132) DB IDENT FROM RESTARTRECORD
  char  DbIdentFromLoginfo      [132+1]; // szDBIdentLog      5.  char_asc(132) DB IDENT FROM LOGINFO      
  char  RestartableWithoutLog   [ 10+1]; // szConsistent      6.  fixed(10)     RESTARTABLE WITHOUT LOG    
  char  LogVolumeEnabled        [ 10+1]; //                   7.  fixed(10)     LOG VOLUME ENABLED         
  char  LogAutoOverwrite        [ 10+1]; //                   8.  fixed(10)     LOG AUTO OVERWRITE         
  char  MasterServernode        [ 64+1]; //                   9.  char_asc(64)  MASTER SERVERNODE          
  char  ConverterVersion        [ 10+1]; //                  10.  fixed(10)     CONVERTER VERSION          
  char  OldestCompatibleVersion [ 11+1]; //                  11.  char_asc(11)  OLDEST COMPATIBLE VERSION  
  char  QueueCount              [ 10+1]; //                  12.  fixed(10)     QUEUE COUNT                
} tcn003_BackupRestartInfo;

class tcn35b_DBMExternalBackupController;   //a forward declaration to avoid include of hcn35b.h
class tcn35c_DBMStageBackupController;      //a forward declaration to avoid include of hcn35c.h
//class tin101_XDBBackupOperation;  //a forward declaration to avoid include of hin101.h

/* backup session structur */
typedef struct tcn003_BackupSesion{
  int                     nSessionType;
  char                    szMediaName[ME_NAME_LENGTH_MXCN003];
  char                    szBackupType[BA_TYPE_LENGTH_MXCN003];

  char                    szFileNumber[BA_FILENUM_LENGTH_MXCN003];

  char                    szUntilDate[BA_UNTIL_LENGTH_MXCN003];
  char                    szUntilTime[BA_UNTIL_LENGTH_MXCN003];

  tcn003_Media            tMedia;

  char                    szCommand[BA_COMMAND_LENGTH_MXCN003];

  bool                    bCheckOnly; // only check with service kernel
  bool                    bCancelActive;
  bool                    bAutoignore;

  tcn003_BackupResult     tBackupResult;

  tcn35b_DBMExternalBackupController * pBackupTool;
  tcn35c_DBMStageBackupController * StageController;
//  tin101_XDBBackupOperation * pXDB;
} tcn003_BackupSession;

#define BA_SESSION_TYPE_SAVE_CN003      1
#define BA_SESSION_TYPE_RESTORE_CN003   2
#define BA_SESSION_TYPE_COMMAND_CN003   3

/* backup history */
// strings for output of the type of an EBID
#define BA_EBIDS_TYPE_DAT_REC_CN003C    "DATA  RECOVERY "
#define BA_EBIDS_TYPE_DAT_MIG_CN003C    "DATA  MIGRATION"
#define BA_EBIDS_TYPE_PAG_REC_CN003C    "PAGES RECOVERY "
#define BA_EBIDS_TYPE_PAG_MIG_CN003C    "PAGES MIGRATION"
#define BA_EBIDS_TYPE_LOG_REC_CN003C    "LOG   RECOVERY "

#endif /* _GCN003_H_ */
