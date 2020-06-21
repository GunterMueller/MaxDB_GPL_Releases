/*
  -----------------------------------------------------------------------------

  module: vcn42.cpp

  -----------------------------------------------------------------------------

  responsible:  BerndV

  special area: DBMServer File Access

  description:  DBMServer File Access - Implementation

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

#include <stdio.h>
#include <stdlib.h>

#include "heo01.h"
#include "heo06.h"
#include "geo43_0.h"

#include "hcn20.h"
#include "hcn40.h"
#include "hcn54.h"
#include "hcn90.h"
#include "hcn42.h"

#include "SAPDBCommon/SAPDB_Names.h"
#include "DBM/Srv/Deprecated/DBMSrvDep_DirtyReply.hpp"
#include "DBM/Srv/Message/DBMSrvMsg_NewRTEError.hpp"
#include "DBM/Srv/Message/DBMSrvMsg_RTEError.hpp"
#include "DBM/Srv/Scheduler/DBMSrvScd_Persistor.hpp"
#include "DBM/Srv/SharedMemory/DBMSrvShM_Persistence.hpp"

/*
  -----------------------------------------------------------------------------
  specification private macros
  -----------------------------------------------------------------------------
 */
#define SEEK_MODE_CUR         "CUR"
#define OPEN_SWITCH           '-'
#define OPEN_MODE_BIN         'b'
#define OPEN_MODE_ASCII       'a'
#define OPEN_MODE_READ        'r'
#define OPEN_MODE_WRITE       'w'
#define OPEN_MODE_APPEND      'x'

// diagnose files handling
#define DIAG_HIST_PATTERN     "_????????_?\?-?\?-??"
#define DIAG_POS_DATE         1
#define DIAG_LEN_DATE         8
#define DIAG_POS_HOUR         10
#define DIAG_LEN_HOUR         2
#define DIAG_POS_MINUTE       13
#define DIAG_LEN_MINUTE       2
#define DIAG_POS_SECOND       16
#define DIAG_LEN_SECOND       2
#define DIAG_CLASS_PRT        "protocol"
#define DIAG_CLASS_BKP        "backup"
#define DIAG_CLASS_CFG        "config"
#define DIAG_CLASS_DUMP       "dump"
#define DIAG_CLASS_LVC        "lvc"
#define DIAG_CLASS_ALL        "all"
#define DIAG_CLASS_HIST       "hist"
#define DIAG_CLASS_SEP        ","
#define DIAG_FILE             "FILE="
#define DIAG_file             "file="
#define DIAG_FILE_L           5
#define DIAG_DATE             "DATE="
#define DIAG_date             "date="
#define DIAG_DATE_L           5
#define DIAG_CLASS            "CLASS="
#define DIAG_class            "class="
#define DIAG_CLASS_L          6

#define FILE_OPT_NOCLOSE      "NOCLOSE"

#define FILEINFO_MULTI        "-m"
#define FILEINFO_TITLE        "filename,exists,readable,writable,islink,size,date_modified,time_modified,media_kind\n"
#define FILELIST_TITLE        "keyname,mode,size,date,time,comment,filename\n"
#define FILEINFO_MEDIATYPE    { "Unknown",          \
                                "File",             \
                                "Pipe",             \
                                "Raw",              \
                                "Tape (Norewind)",  \
                                "Tape (Rewind)",    \
                                "Tape",             \
                                "Directory" }

#define CONT_FLAG_END         "     END"
#define CONT_FLAG_CONTINUE    "CONTINUE"

#define FGET_TOKENDEL         FGET_TOKENDEL_CN42
#define FGET_WILDCARD         "*"
#define FGET_WILDCARDSTAR     '*'
#define FGET_WILDCARDQUE      '?'
#define FGET_UPPER            ".."
#define FGET_SLASH            "/"
#define FGET_BACKSLASH        "\\"
#define FGET_DRIVE            ":"
#define FGET_NODE             "@"

#define FGET_MODE_LINE        "LINE="
#define FGET_MODE_DATE        "DATE="
#define FGET_MODE_TYPE        "TYPE="
#define FGET_MODE_OPERATION   "OP="

#define FGET_OP_SHRINK        "SHRINK"
#define FGET_OP_DELETE        "DELETE"

// directory IDs
#define FGET_LOC_DBROOT       1
#define FGET_LOC_WRK          2
#define FGET_LOC_RUNDIR       3
#define FGET_LOC_CONFIG       4
#define FGET_LOC_SAP          5
#define FGET_LOC_DBA          6
#define FGET_LOC_HISTDIR      7
#define FGET_LOC_ANALYZER     8
#define FGET_LOC_ENV          9

// file types
#define FGET_TYP_UNKNOWN      0
#define FGET_TYP_ASCII        1
#define FGET_TYP_BINARY       2
#define FGET_TYP_DIR          3
#define FGET_TYP_OBSOLETE     4
#define FGET_TYP_XML          5

#define FGET_IS_TEXT(a)      ((a == FGET_TYP_ASCII) || (a == FGET_TYP_XML))
#define FGET_IS_BINARY(a)    (a == FGET_TYP_BINARY)

#define FGET_TYPT_UNKNOWN     "UNKNOWN"
#define FGET_TYPT_ASCII       "ASCII"
#define FGET_TYPT_BINARY      "BINARY"
#define FGET_TYPT_DIRECTORY   "DIRECTORY"
#define FGET_TYPT_XML         "XML"

// special directories
#define FGET_LOC_SAP_TXT      "sap"
#define FGET_LOC_DBA_TXT      "dbahist"
#define FGET_LOC_ENV_TXT      "env"

#define FGET_NAME_DB          ((const char *) 1)
#define FGET_NAME_INST        ((const char *) 2)

#define FGET_ORG_EXT          ((const char *) 1)

// show this file in list (command file_getlist)
#define FGET_SHOW_NEVER     1000
#define FGET_SHOW_ALWAYS    0

#ifdef WIN32
  #define LO_EXT      ".prt"
  #define LCICMD_EXT  ".bat"
#else
  #define LO_EXT      ".prot"
  #define LCICMD_EXT  ""
#endif

// Timestampfeatures
#define CHAR_YEAR        'Y'
#define CHAR_MONTH       'M'
#define CHAR_DAY          'D'
#define CHAR_HOUR         'h'
#define CHAR_MINUTE       'm'
#define CHAR_SECOND       's'
#define MILLENIUM_PRE    2000
#define MILLENIUM_MIN     100
#define STAMP_FMT         "%04d%02d%02d%02d%02d%02d"

#define CHAR_SLASH            '/'
#define CHAR_BACKSLASH        '\\'
#define CHAR_DRIVE            ':'
#define REGEXP_ESCAPE         '\\'
#define REGEXP_CHAR           '.'
#define REGEXP_MULTIPLE       '*'

#ifdef _WIN32
  #define PROTCONV_CALL   "\"\"%sprotconv\" -p \"%s\" %s -o \"%s\"\""
#else
  #define PROTCONV_CALL   "%sprotconv -p %s %s -o %s"
#endif

#define FGET_KNLMSGHIST   FILE_HISTORY_DIRECTORY_NAME_PREFIX""SAPDB_KERNEL_MESSAGES_FILE_NAME

typedef struct tcn42FGetFiles {
    const char   * szType;
    long           nShow;
    long           nType;
    const long     nOrgType;
    bool           bRemove;
    const char   * szXParam;
    const char   * szName;
    const char   * szExt;
    long           nLocation;
    const char   * szBaName;
    const char   * szBaExt;
    long           nBaLocation;
    const char   * szClass;
    const char   * szComment;
    long           nHeader;
    long           nTimestampPos;
    const char   * szTimestampFmt;
    bool           bRemoveEmptyLines;
    bool           bOperation;
    tsp00_Int4     hFile;
    const char   * szProtconvOpt;
} tcn42FGetFiles;

// File                          show                nType                nOrgType            remove   XParam                name                ext             location          backupname     backupext       backuplocation     class             comment                                 Hea nTi szTimestampFmt         bRemoveLi bOperation  hFile szProtconvopt
static tcn42FGetFiles sFGetFiles[] = {                                                 
  {FGET_KNLDIAG_CN42           , FGET_SHOW_ALWAYS  , FGET_TYP_ASCII     , FGET_TYP_ASCII     , true  , NULL                , SAPDB_DIAG_FILE_NAME, NULL       , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , DIAG_CLASS_PRT  , "Database Messages"                  , 0 , 0 , NULL                  , true  , false,      -1,   NULL                 },
  {FGET_KNLDIAGERR_CN42        , FGET_SHOW_ALWAYS  , FGET_TYP_ASCII     , FGET_TYP_ASCII     , true  , NULL                , SAPDB_DIAG_FILE_NAME,".err"      , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , DIAG_CLASS_PRT  , "Database Errors"                    , 0 , 0 , NULL                  , false , false,      -1,   NULL                 },
  {FGET_KNLDIAGOLD_CN42        , FGET_SHOW_ALWAYS  , FGET_TYP_ASCII     , FGET_TYP_ASCII     , true  , NULL                , SAPDB_DIAG_FILE_NAME,".old"      , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , DIAG_CLASS_PRT  , "Database Messages (OLD)"            , 0 , 0 , NULL                  , true  , false,      -1,   NULL                 },
  {FGET_KNLMSG_CN42            , FGET_SHOW_ALWAYS  , FGET_TYP_ASCII     , FGET_TYP_ASCII     , true  , NULL                , "KnlMsg"          , NULL         , FGET_LOC_RUNDIR  , ".KnlMsg"    , ".txt"         , FGET_LOC_RUNDIR  , DIAG_CLASS_PRT  , "Database Messages (verbose)"        , 0 , 0 , NULL                  , true  , false,      -1,   "-f plain -verbose -t KNLDIAG"},
  {FGET_KNLMSGARC_CN42         , FGET_SHOW_NEVER   , FGET_TYP_ASCII     , FGET_TYP_ASCII     , true  , NULL                , "KnlMsgArchive"   , NULL         , FGET_LOC_RUNDIR  , ".KnlMsgArc" , ".txt"         , FGET_LOC_RUNDIR  , DIAG_CLASS_PRT  , "Database Errors (verbose)"          , 0 , 0 , NULL                  , true  , false,      -1,   "-f plain -verbose -t KNLDIAGERR"},
  {FGET_KNLMSGOLD_CN42         , FGET_SHOW_NEVER   , FGET_TYP_ASCII     , FGET_TYP_ASCII     , true  , NULL                , "KnlMsg"          , ".old"       , FGET_LOC_RUNDIR  , ".KnlMsgOld" , ".txt"         , FGET_LOC_RUNDIR  , DIAG_CLASS_PRT  , "Database Messages (verbose) (OLD)"  , 0 , 0 , NULL                  , true  , false,      -1,   "-f plain -verbose -t KNLDIAGOLD"},
  {FGET_KNLTRC_CN42            , FGET_SHOW_ALWAYS  , FGET_TYP_BINARY    , FGET_TYP_BINARY    , true  , "_KERNELTRACEFILE"  , NULL              , NULL         , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , DIAG_CLASS_DUMP , "Database Trace"                     , 0 , 0 , NULL                  , false , false,      -1,   NULL                 },
  {FGET_KNLTRCDAT_CN42         , FGET_SHOW_NEVER   , FGET_TYP_BINARY    , FGET_TYP_BINARY    , true  , "_KERNELTRACEFILE"  , NULL              , ".dat"       , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , NULL            , "Database Trace Dat"                 , 0 , 0 , NULL                  , false , false,      -1,   NULL                 },
  {FGET_KNLDUMP_CN42           , FGET_SHOW_ALWAYS  , FGET_TYP_BINARY    , FGET_TYP_BINARY    , true  , "_KERNELDUMPFILE"   , NULL              , NULL         , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , DIAG_CLASS_DUMP , "Database Dump"                      , 0 , 0 , NULL                  , false , false,      -1,   NULL                 },
  {FGET_KNLEVT_CN42            , FGET_SHOW_ALWAYS  , FGET_TYP_ASCII     , FGET_TYP_ASCII     , true  , "_EVENTFILE"        , NULL              , NULL         , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , DIAG_CLASS_PRT  , "Database Events"                    , 0 , 0 , NULL                  , false , false,      -1,   NULL                 },
  {FGET_RTEDUMP_CN42           , FGET_SHOW_ALWAYS  , FGET_TYP_ASCII     , FGET_TYP_ASCII     , true  , "_RTEDUMPFILE"      , NULL              , NULL         , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , DIAG_CLASS_DUMP , "Runtime Environment Dump"           , 0 , 0 , NULL                  , false , false,      -1,   NULL                 },
  {FGET_UTLPRT_CN42            , FGET_SHOW_ALWAYS  , FGET_TYP_ASCII     , FGET_TYP_ASCII     , true  , NULL                , SAPDB_ADMIN_COMMANDS_FILE_NAME,"", FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , DIAG_CLASS_PRT  , "Utility Statements"                 , 0 , 0 , NULL                  , true  , false,      -1,   NULL                 },
  {FGET_BACKHIST_CN42          , FGET_SHOW_ALWAYS  , FGET_TYP_ASCII     , FGET_TYP_ASCII     , true  , "_BACKUP_HISTFILE"  , NULL              , NULL         , FGET_LOC_RUNDIR  , FGET_NAME_DB , FGET_ORG_EXT   , FGET_LOC_WRK     , DIAG_CLASS_BKP  , "Backup History"                     , 0 , 0 , NULL                  , false , false,      -1,   NULL                 },
  {FGET_BACKMDF_CN42           , FGET_SHOW_ALWAYS  , FGET_TYP_ASCII     , FGET_TYP_ASCII     , true  , "_BACKUP_MED_DEF"   , NULL              , NULL         , FGET_LOC_RUNDIR  , FGET_NAME_DB , FGET_ORG_EXT   , FGET_LOC_WRK     , DIAG_CLASS_BKP  , "Backup Media History"               , 0 , 0 , NULL                  , false , false,      -1,   NULL                 },
  {FGET_BACKEBF_CN42           , FGET_SHOW_ALWAYS  , FGET_TYP_ASCII     , FGET_TYP_ASCII     , true  , NULL                , "dbm"             , ".ebf"       , FGET_LOC_RUNDIR  , FGET_NAME_DB , FGET_ORG_EXT   , FGET_LOC_WRK     , DIAG_CLASS_BKP  , "External Backup History"            , 0 , 0 , NULL                  , false , false,      -1,   NULL                 },
  {FGET_BACKEBP_CN42           , FGET_SHOW_ALWAYS  , FGET_TYP_ASCII     , FGET_TYP_ASCII     , true  , NULL                , "dbm"             , ".ebp"       , FGET_LOC_RUNDIR  , FGET_NAME_DB , FGET_ORG_EXT   , FGET_LOC_WRK     , DIAG_CLASS_BKP  , "External Backup Log File"           , 0 , 0 , NULL                  , false , false,      -1,   NULL                 },
  {FGET_BACKEBL_CN42           , FGET_SHOW_ALWAYS  , FGET_TYP_ASCII     , FGET_TYP_ASCII     , true  , NULL                , "dbm"             , ".ebl"       , FGET_LOC_RUNDIR  , FGET_NAME_DB , FGET_ORG_EXT   , FGET_LOC_WRK     , DIAG_CLASS_BKP  , "External Backup Log"                , 0 , 0 , NULL                  , false , false,      -1,   NULL                 },
  {FGET_LOADPRT_CN42           , FGET_SHOW_NEVER   , FGET_TYP_ASCII     , FGET_TYP_ASCII     , true  , NULL                , "load"            , LO_EXT       , FGET_LOC_RUNDIR  , "dbm"        , ".ins"         , FGET_LOC_RUNDIR  , NULL            , "XLoad Protocol"                     , 0 , 0 , NULL                  , false , false,      -1,   NULL                 },
  {FGET_XSRVPRT_CN42           , FGET_SHOW_NEVER   , FGET_TYP_ASCII     , FGET_TYP_ASCII     , false , NULL                , "xserver_@"       , ".prt"       , FGET_LOC_WRK     , NULL         , NULL           , 0                , DIAG_CLASS_PRT  , "XServer Messages"                   , 0 , 0 , NULL                  , true  , false,      -1,   NULL                 },
  {FGET_XSRVPRTOLD_CN42        , FGET_SHOW_NEVER   , FGET_TYP_ASCII     , FGET_TYP_ASCII     , false , NULL                , "xserver_@"       , ".old"       , FGET_LOC_WRK     , NULL         , NULL           , 0                , DIAG_CLASS_PRT  , "XServer Messages (OLD)"             , 0 , 0 , NULL                  , true  , false,      -1,   NULL                 },
  {FGET_REPMAN_CN42            , FGET_SHOW_NEVER   , FGET_TYP_ASCII     , FGET_TYP_ASCII     , false , NULL                , "loader"          , ".log"       , FGET_LOC_WRK     , "dbm"        , ".ins"         , FGET_LOC_RUNDIR  , NULL            , "Database Loader  Log File"          , 0 , 0 , NULL                  , false , false,      -1,   NULL                 },
  {FGET_DBMPRT_CN42            , FGET_SHOW_ALWAYS  , FGET_TYP_ASCII     , FGET_TYP_ASCII     , true  , NULL                , "dbm"             , ".prt"       , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , DIAG_CLASS_PRT  , "Database Manager Log File"          , 3 , 0 , "YYYY-MM-DD hh:mm:ss" , false , false,      -1,   NULL                 },
  {FGET_DBMMDF_CN42            , FGET_SHOW_ALWAYS  , FGET_TYP_ASCII     , FGET_TYP_ASCII     , true  , NULL                , "dbm"             , ".mmm"       , FGET_LOC_RUNDIR  , FGET_NAME_DB , FGET_ORG_EXT   , FGET_LOC_CONFIG  , DIAG_CLASS_CFG  , "Database Manager Media"             , 0 , 0 , NULL                  , false , false,      -1,   NULL                 },
  {FGET_DBMPAHI_CN42           , FGET_SHOW_ALWAYS  , FGET_TYP_ASCII     , FGET_TYP_ASCII     , true  , NULL                , FGET_NAME_DB      , ".pah"       , FGET_LOC_CONFIG  , NULL         , NULL           , 0                , DIAG_CLASS_PRT  , "Database Parameter History"         , 0 , 0 , NULL                  , false , false,      -1,   NULL                 },
  {FGET_CTRLMDF_CN42           , FGET_SHOW_ALWAYS  , FGET_TYP_ASCII     , FGET_TYP_ASCII     , true  , NULL                , "control"         , ".med"       , FGET_LOC_RUNDIR  , FGET_NAME_DB , FGET_ORG_EXT   , FGET_LOC_CONFIG  , NULL            , "XControl Media"                     , 0 , 0 , NULL                  , false , false,      -1,   NULL                 },
  {FGET_CTRLLOG_CN42           , FGET_SHOW_ALWAYS  , FGET_TYP_ASCII     , FGET_TYP_ASCII     , true  , NULL                , "control"         , ".log"       , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , NULL            , "XControl Log File"                  , 0 , 0 , NULL                  , false , false,      -1,   NULL                 },
  {FGET_DBMCFG_CN42            , FGET_SHOW_NEVER   , FGET_TYP_ASCII     , FGET_TYP_ASCII     , true  , NULL                , "dbm"             , ".cfg"       , FGET_LOC_RUNDIR  , FGET_NAME_DB , FGET_ORG_EXT   , FGET_LOC_CONFIG  , DIAG_CLASS_CFG  , "Database Manager Configuration"     , 0 , 0 , NULL                  , false , false,      -1,   NULL                 },
  {FGET_DBMINSTCFG_CN42        , FGET_SHOW_NEVER   , FGET_TYP_ASCII     , FGET_TYP_ASCII     , false , NULL                , FGET_NAME_INST    , ".cfg"       , FGET_LOC_WRK     , NULL         , NULL           , NULL             , DIAG_CLASS_CFG  , "Database Manager Configuration"     , 0 , 0 , NULL                  , false , false,      -1,   NULL                 },
  {FGET_DBMGLOBCFG_CN42        , FGET_SHOW_NEVER   , FGET_TYP_ASCII     , FGET_TYP_ASCII     , false , NULL                , ".global"         , ".cfg"       , FGET_LOC_WRK     , NULL         , NULL           , NULL             , DIAG_CLASS_CFG  , "Database Manager Configuration"     , 0 , 0 , NULL                  , false , false,      -1,   NULL                 },
  {FGET_PARAM_CN42             , FGET_SHOW_NEVER   , FGET_TYP_BINARY    , FGET_TYP_BINARY    , false , NULL                , FGET_NAME_DB      , NULL         , FGET_LOC_CONFIG  , "param"      , FGET_ORG_EXT   , FGET_LOC_RUNDIR  , DIAG_CLASS_CFG  , "Database Parameters"                , 0 , 0 , NULL                  , false , false,      -1,   NULL                 },
  {FGET_LCINIT_CN42            , FGET_SHOW_ALWAYS  , FGET_TYP_ASCII     , FGET_TYP_ASCII     , false , NULL                , "lcinit"          , ".log"       , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , DIAG_CLASS_LVC  , "LiveCache Initialisation"           , 0 , 0 , NULL                  , false , false,      -1,   NULL                 },
  {FGET_LCINITCMD_CN42         , FGET_SHOW_ALWAYS  , FGET_TYP_ASCII     , FGET_TYP_ASCII     , false , NULL                , "lcinit"          , LCICMD_EXT   , FGET_LOC_SAP     , NULL         , NULL           , 0                , DIAG_CLASS_LVC  , "LiveCache Initialisation Script"    , 0 , 0 , NULL                  , false , false,      -1,   NULL                 },
  {FGET_LCINITHIS_CN42         , FGET_SHOW_ALWAYS  , FGET_TYP_ASCII     , FGET_TYP_ASCII     , false , NULL                , "lcinit"          , ".his"       , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , DIAG_CLASS_LVC  , "LiveCache Initialisation History"   , 0 , 0 , NULL                  , false , false,      -1,   NULL                 },
  {FGET_INSTPRT_CN42           , FGET_SHOW_ALWAYS  , FGET_TYP_ASCII     , FGET_TYP_ASCII     , true  , NULL                , "dbm"             , ".ins"       , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , DIAG_CLASS_PRT  , "Installation Log File"              , 0 , 0 , NULL                  , false , false,      -1,   NULL                 },
  {FGET_USER_CN42              , FGET_SHOW_NEVER   , FGET_TYP_BINARY    , FGET_TYP_BINARY    , true  , NULL                , FGET_NAME_DB      , ".upc"       , FGET_LOC_CONFIG  , "dbm"        , FGET_ORG_EXT   , FGET_LOC_RUNDIR  , NULL            , "User Profiles"                      , 0 , 0 , NULL                  , false , false,      -1,   NULL                 },
  {FGET_KNLTRCPRT_CN42         , FGET_SHOW_ALWAYS  , FGET_TYP_ASCII     , FGET_TYP_ASCII     , true  , NULL                , FGET_NAME_DB      , ".prt"       , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , DIAG_CLASS_PRT  , "Kernel Trace Log File"              , 0 , 0 , NULL                  , false , false,      -1,   NULL                 },
  {FGET_WIZOVER_CN42           , 1                 , FGET_TYP_ASCII     , FGET_TYP_ASCII     , false , NULL                , "*"               , ".wiz"       , FGET_LOC_SAP     , NULL         , NULL           , 0                , NULL            , "Wizard Overview"                    , 0 , 0 , NULL                  , false , true ,      -1,   NULL                 },
  {FGET_WIZDETAIL_CN42         , 1                 , FGET_TYP_ASCII     , FGET_TYP_ASCII     , false , NULL                , "*"               , ".wtc"       , FGET_LOC_SAP     , NULL         , NULL           , 0                , NULL            , "Wizard Detail"                      , 0 , 0 , NULL                  , false , true ,      -1,   NULL                 },
  {FGET_DBAHIST_CN42           , FGET_SHOW_ALWAYS  , FGET_TYP_ASCII     , FGET_TYP_ASCII     , true  , NULL                , "dbahist"         , ".prt"       , FGET_LOC_DBA     , NULL         , NULL           , 0                , DIAG_CLASS_PRT  , "DBA Action Log"                     , 3 , 15, "YYYYMMDDhhmmss"      , false , true ,      -1,   NULL                 },
  {FGET_DBADTL_CN42            , FGET_SHOW_NEVER+1 , FGET_TYP_ASCII     , FGET_TYP_ASCII     , true  , NULL                , "dba*"            , NULL         , FGET_LOC_DBA     , NULL         , NULL           , 0                , NULL            , "DBA Action Log Details"             , 0 , 0 , NULL                  , false , true ,      -1,   NULL                 },
  {FGET_DBMSRV_CN42            , FGET_SHOW_NEVER   , FGET_TYP_ASCII     , FGET_TYP_ASCII     , false , NULL                , "dbmsrv_@"        , ".prt"       , FGET_LOC_WRK     , NULL         , NULL           , 0                , DIAG_CLASS_PRT  , "Global Database Manager Log File"   , 3 , 0 , "YYYY-MM-DD hh:mm:ss" , false , false,      -1,   NULL                 },
  {FGET_AKDMP_CN42             , 1                 , FGET_TYP_BINARY    , FGET_TYP_BINARY    , true  , NULL                , "AK"              , ".dmp"       , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , DIAG_CLASS_DUMP , "AK Diagnose Dump"                   , 0 , 0 , NULL                  , false , false,      -1,   NULL                 },
  {FGET_AKBUF_CN42             , 1                 , FGET_TYP_BINARY    , FGET_TYP_BINARY    , true  , NULL                , "AK"              , ".buf"       , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , DIAG_CLASS_DUMP , "AK Diagnose Buffer"                 , 0 , 0 , NULL                  , false , false,      -1,   NULL                 },
  {FGET_AKSTM_CN42             , 1                 , FGET_TYP_BINARY    , FGET_TYP_BINARY    , true  , NULL                , "AK"              , ".stm"       , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , DIAG_CLASS_DUMP , "AK Diagnose Statement"              , 0 , 0 , NULL                  , false , false,      -1,   NULL                 },
  {FGET_KNLCOR_CN42            , FGET_SHOW_ALWAYS  , FGET_TYP_BINARY    , FGET_TYP_BINARY    , true  , NULL                , "*"               , ".cor"       , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , DIAG_CLASS_DUMP , "\"cor\" file for problem analysis"  , 0 , 0 , NULL                  , false , false,      -1,   NULL                 },
  {FGET_KNLBAD_CN42            , FGET_SHOW_ALWAYS  , FGET_TYP_BINARY    , FGET_TYP_BINARY    , true  , NULL                , "*"               , ".bad"       , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , DIAG_CLASS_DUMP , "\"bad\" file for problem analysis"  , 0 , 0 , NULL                  , false , false,      -1,   NULL                 },
  {FGET_DIAGPACK_CN42          , FGET_SHOW_ALWAYS  , FGET_TYP_BINARY    , FGET_TYP_BINARY    , true  , NULL                , "diagpkg"         , ".tgz"       , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , NULL            , "Diagnose Package"                   , 0 , 0 , NULL                  , false , false,      -1,   NULL                 },
  {FGET_DIAGHIST_CN42          , FGET_SHOW_NEVER   , FGET_TYP_BINARY    , FGET_TYP_BINARY    , true  , NULL                , FGET_NAME_DB      , "_*"         , FGET_LOC_HISTDIR , NULL         , NULL           , 0                , NULL            , "Diagnose History File"              , 0 , 0 , NULL                  , false , false,      -1,   NULL                 },
  {FGET_DIAGDIR_CN42           , FGET_SHOW_ALWAYS  , FGET_TYP_DIR       , FGET_TYP_DIR       , false , NULL                , ""                , NULL         , FGET_LOC_HISTDIR , NULL         , NULL           , 0                , NULL            , "Diagnose History File"              , 0 , 0 , NULL                  , false , false,      -1,   NULL                 },
  {FGET_ANALYZER_CN42          , FGET_SHOW_ALWAYS  , FGET_TYP_DIR       , FGET_TYP_DIR       , false , NULL                , "analyzer"        , NULL         , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , NULL            , "DB Analyzer File"                   , 0 , 21, "YYYY-MM-DD hh:mm:ss" , false , true ,      -1,   NULL                 },
  {FGET_LCTRC_CN42             , FGET_SHOW_ALWAYS  , FGET_TYP_ASCII     , FGET_TYP_ASCII     , true  , NULL                , "lc*"             , NULL         , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , NULL            , "LiveCache Trace (ASCII)"            , 0 , 0 , NULL                  , false , true ,      -1,   NULL                 },
  {FGET_ATP_CN42               , FGET_SHOW_NEVER   , FGET_TYP_ASCII     , FGET_TYP_ASCII     , true  , NULL                , "atp*"            , NULL         , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , NULL            , "LiveCache Trace (ASCII)"            , 0 , 0 , NULL                  , false , true ,      -1,   NULL                 },
  {FGET_SDBINFO_CN42           , FGET_SHOW_ALWAYS  , FGET_TYP_ASCII     , FGET_TYP_ASCII     , false , NULL                , "sdbinfo"         , ".prt"       , FGET_LOC_WRK     , NULL         , NULL           , 0                , DIAG_CLASS_PRT  , "Operating System Information"       , 0 , 0 , NULL                  , false , true ,      -1,   NULL                 },
  {FGET_ANAPID_CN42            , FGET_SHOW_NEVER   , FGET_TYP_BINARY    , FGET_TYP_BINARY    , false , NULL                , "DBAN"            , ".pid"       , FGET_LOC_ANALYZER, NULL         , NULL           , 0                , NULL            , "DBanalyzer process ID"              , 0 , 0 , NULL                  , false , false,      -1,   NULL                 },
  {FGET_ANASID_CN42            , FGET_SHOW_NEVER   , FGET_TYP_BINARY    , FGET_TYP_BINARY    , false , NULL                , "DBAN"            , ".sid"       , FGET_LOC_ANALYZER, NULL         , NULL           , 0                , NULL            , "DBanalyzer session ID"              , 0 , 0 , NULL                  , false , false,      -1,   NULL                 },
  {FGET_EVTDISPCONF_CN42       , FGET_SHOW_ALWAYS  , FGET_TYP_ASCII     , FGET_TYP_ASCII     , true  , NULL                , "dbmevtdisp"      , ".cfg"       , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , DIAG_CLASS_CFG  , "Event Dispatcher Configuration"     , 0 , 0 , NULL                  , false , false,      -1,   NULL                 },
  {FGET_EVTDISPCDEF_CN42       , FGET_SHOW_NEVER   , FGET_TYP_ASCII     , FGET_TYP_ASCII     , false , NULL                , "dbmevtdisp"      , ".cfg"       , FGET_LOC_ENV     , NULL         , NULL           , 0                , DIAG_CLASS_CFG  , "Event Dispatcher Configuration"     , 0 , 0 , NULL                  , false , false,      -1,   NULL                 },
  {FGET_EVTDISPPRT_CN42        , FGET_SHOW_ALWAYS  , FGET_TYP_ASCII     , FGET_TYP_ASCII     , true  , NULL                , "dbmevtdisp"      , ".prt"       , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , DIAG_CLASS_CFG  , "Event Dispatcher Log File"          , 0 , 0 , NULL                  , false , false,      -1,   NULL                 },
  {FGET_SEVTDISPCFG_CN42       , FGET_SHOW_NEVER   , FGET_TYP_ASCII     , FGET_TYP_ASCII     , true  , NULL                , "dbmsilevt"       , ".cfg"       , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , DIAG_CLASS_CFG  , "Silent Event Dispatcher Config"     , 0 , 0 , NULL                  , false , false,      -1,   NULL                 },
  {FGET_SHMDATA_CN42           , FGET_SHOW_NEVER   , FGET_TYP_BINARY    , FGET_TYP_BINARY    , false , NULL                , DBMSrvShM_Persistence::m_FileNameBase, DBMSrvShM_Persistence::m_FileExtensionData, FGET_LOC_WRK, NULL, NULL, 0, NULL     , "DBMServer shared memory data"       , 0 , 0 , NULL                  , false , false,      -1,   NULL                 },
  {FGET_SHMADMIN_CN42          , FGET_SHOW_NEVER   , FGET_TYP_BINARY    , FGET_TYP_BINARY    , false , NULL                , DBMSrvShM_Persistence::m_FileNameBase, DBMSrvShM_Persistence::m_FileExtensionAdmin, FGET_LOC_WRK, NULL, NULL, 0, NULL    , "DBMServer shared memory admin data" , 0 , 0 , NULL                  , false , false,      -1,   NULL                 },
  {FGET_SCHEDULERCFG_CN42      , FGET_SHOW_NEVER   , FGET_TYP_ASCII     , FGET_TYP_ASCII     , true  , NULL                , DBMSrvScd_Persistor::m_FileNameBase, DBMSrvScd_Persistor::m_FileNameExtension, FGET_LOC_RUNDIR, NULL, NULL, 0, DIAG_CLASS_CFG,"DBM Command Scheduler Configuration", 0, 0,NULL                  , false , false,      -1,   NULL                 },  
  {FGET_DBMSRVMINIDUMP_CN42    , FGET_SHOW_NEVER   , FGET_TYP_BINARY    , FGET_TYP_BINARY    , false , NULL                , "dbmsrv_@"        , ".dmp"       , FGET_LOC_WRK     , NULL         , NULL           , 0                , DIAG_CLASS_DUMP , "Database Manager Mini Dump"         , 0 , 0 , NULL                  , false , false,      -1,   NULL                 },
  {FGET_DBMSRVSTKTRC_CN42      , FGET_SHOW_NEVER   , FGET_TYP_ASCII     , FGET_TYP_ASCII     , false , NULL                , "dbmsrv_@"        , ".err"       , FGET_LOC_WRK     , NULL         , NULL           , 0                , DIAG_CLASS_DUMP , "Database Manager Stack Trace"       , 0 , 0 , NULL                  , false , false,      -1,   NULL                 },
  {FGET_COMSEGDUMP_CN42        , FGET_SHOW_NEVER   , FGET_TYP_BINARY    , FGET_TYP_BINARY    , true  , NULL                , "comseg"          , ".dump"      , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , DIAG_CLASS_DUMP , "Communication Dump"                 , 0 , 0 , NULL                  , false , false,      -1,   NULL                 },
  {FGET_KNLMINIDUMP_CN42       , FGET_SHOW_NEVER   , FGET_TYP_BINARY    , FGET_TYP_BINARY    , true  , NULL                , "KnlMini"         , ".dmp"       , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , DIAG_CLASS_DUMP , "Kernel Mini Dump"                   , 0 , 0 , NULL                  , false , false,      -1,   NULL                 },

  // obsolete files to delete during migration                                                                                                                                                                                                                                                                                                                        
  {FOBS_KNLDIAGSTART_CN42      , FGET_SHOW_NEVER   , FGET_TYP_OBSOLETE  , FGET_TYP_OBSOLETE  , true  , NULL                , "knldiag"         , ".start"     , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , NULL            , "obsolete"                           , 0 , 0 , NULL                  , false , false,      -1,   NULL                 },
  {FOBS_KNLDIAGRUN_CN42        , FGET_SHOW_NEVER   , FGET_TYP_OBSOLETE  , FGET_TYP_OBSOLETE  , true  , NULL                , "knldiag"         , ".run"       , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , NULL            , "obsolete"                           , 0 , 0 , NULL                  , false , false,      -1,   NULL                 },
  {FOBS_KNLDIAGOLDSTART_CN42   , FGET_SHOW_NEVER   , FGET_TYP_OBSOLETE  , FGET_TYP_OBSOLETE  , true  , NULL                , "knldiag"         , ".old.start" , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , NULL            , "obsolete"                           , 0 , 0 , NULL                  , false , false,      -1,   NULL                 },
  {FOBS_KNLDIAGOLDRUN_CN42     , FGET_SHOW_NEVER   , FGET_TYP_OBSOLETE  , FGET_TYP_OBSOLETE  , true  , NULL                , "knldiag"         , ".old.run"   , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , NULL            , "obsolete"                           , 0 , 0 , NULL                  , false , false,      -1,   NULL                 },
  {FOBS_KNLDIAGERRCOMP_CN42    , FGET_SHOW_NEVER   , FGET_TYP_OBSOLETE  , FGET_TYP_OBSOLETE  , true  , NULL                , "knldiag"         , ".err.comp"  , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , NULL            , "obsolete"                           , 0 , 0 , NULL                  , false , false,      -1,   NULL                 },
  {FOBS_DBMUTLCOMP_CN42        , FGET_SHOW_NEVER   , FGET_TYP_OBSOLETE  , FGET_TYP_OBSOLETE  , true  , NULL                , "dbm"             , ".utl.comp"  , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , NULL            , "obsolete"                           , 0 , 0 , NULL                  , false , false,      -1,   NULL                 },
                                                                          
  {FOBS_KNLDIAGCLASSIC_CN42    , FGET_SHOW_NEVER   , FGET_TYP_OBSOLETE  , FGET_TYP_OBSOLETE  , true  , NULL                , "knldiag"         , ".classic"   , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , NULL            , "obsolete"                           , 0 , 0 , NULL                  , false , false,      -1,   NULL                 },
  {FOBS_KNLDIAGVERBOSE_CN42    , FGET_SHOW_NEVER   , FGET_TYP_OBSOLETE  , FGET_TYP_OBSOLETE  , true  , NULL                , "knldiag"         , ".verbose"   , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , NULL            , "obsolete"                           , 0 , 0 , NULL                  , false , false,      -1,   NULL                 },
  {FOBS_KNLDIAGOLDCLASSIC_CN42 , FGET_SHOW_NEVER   , FGET_TYP_OBSOLETE  , FGET_TYP_OBSOLETE  , true  , NULL                , "knldiag"         , ".old.classic",FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , NULL            , "obsolete"                           , 0 , 0 , NULL                  , false , false,      -1,   NULL                 },
  {FOBS_KNLDIAGOLDVERBOSE_CN42 , FGET_SHOW_NEVER   , FGET_TYP_OBSOLETE  , FGET_TYP_OBSOLETE  , true  , NULL                , "knldiag"         , ".old.verbose",FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , NULL            , "obsolete"                           , 0 , 0 , NULL                  , false , false,      -1,   NULL                 },
  {FOBS_KNLDIAGERRCLASSIC_CN42 , FGET_SHOW_NEVER   , FGET_TYP_OBSOLETE  , FGET_TYP_OBSOLETE  , true  , NULL                , "knldiag"         , ".err.classic",FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , NULL            , "obsolete"                           , 0 , 0 , NULL                  , false , false,      -1,   NULL                 },
  {FOBS_KNLDIAGERRVERBOSE_CN42 , FGET_SHOW_NEVER   , FGET_TYP_OBSOLETE  , FGET_TYP_OBSOLETE  , true  , NULL                , "knldiag"         , ".err.verbose",FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , NULL            , "obsolete"                           , 0 , 0 , NULL                  , false , false,      -1,   NULL                 },
  {FOBS_DBMUTLCLASSIC_CN42     , FGET_SHOW_NEVER   , FGET_TYP_OBSOLETE  , FGET_TYP_OBSOLETE  , true  , NULL                , "dbm"             , ".utl.classic",FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , NULL            , "obsolete"                           , 0 , 0 , NULL                  , false , false,      -1,   NULL                 },
  {FOBS_DBMUTLVERBOSE_CN42     , FGET_SHOW_NEVER   , FGET_TYP_OBSOLETE  , FGET_TYP_OBSOLETE  , true  , NULL                , "dbm"             , ".utl.verbose",FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , NULL            , "obsolete"                           , 0 , 0 , NULL                  , false , false,      -1,   NULL                 },
                                                                                                                                                                                                                                                                                                                                          
  {NULL                        , FGET_SHOW_NEVER   , FGET_TYP_UNKNOWN   , FGET_TYP_UNKNOWN   , false , NULL                , NULL              , NULL         , 0                , NULL         , NULL           , 0                , NULL            , ""                                   , 0 , 0 , NULL                  , false , false,      -1,   NULL                 }
};                                                                                                                                                                                                                                                                                                                                       

/*
  ------------------------------------------------------------------------------
  specification private functions
  ------------------------------------------------------------------------------
 */
static tcn00_Error cn42_FileReadNext
      ( char             * replyData,
        int              * replyLen,
        const int          replyLenMax,
        const tsp00_Int4     hFile,
        const tsp00_Longint  nSize,
        const bool         bNoClose,
        const bool         bAscii,
        const bool         bRemoveEmptyLines,
        const int          nIndex,
        const char *       szDeleteAfterClose);

static tcn00_Error cn42_FileReadFirst
      ( char             * replyData,
        int              * replyLen,
        const int          replyLenMax,
        const tsp00_Int4     hFile,
        const tsp00_Longint  nSize,
        const int          nIndex,
        const char *       pDate,
        int                nLine,
        const char *       szDeleteAfterClose);

static bool cn42_FileMatch
      ( tsp00_Pathc        szFileName,
        tsp00_Pathc        szRealName,
        tsp00_Pathc      & szToken);

static bool cn42_DiagFileMatch
      ( const char       * szDBName,
        const char       * szFile );

/*!   @brief  computes a filename
      @param  szDbName     [IN]  - name of database (null terminated string)
      @param  szType       [IN]  - ID of requested file (null terminated string)
      @param  nIndex       [OUT] - index of file found in the table
      @param  oError       [OUT] - general error (filled on return false)
      @param  szFileName   [OUT] - name of the original file (null terminated string)
      @return true if successfull, in case of false see oError
 */
static bool cn42_GetFileName
      ( const tsp00_DbNamec & szDbName,
        const tsp00_Pathc   & szType,
        int                 & nIndex,
        DBMSrvMsg_Error     & oError,
        tsp00_Pathc         & szFileName );

/*!   @brief  computes a filename
      @param  szDbName     [IN]  - name of database (null terminated string)
      @param  szType       [IN]  - ID of requested file (null terminated string)
      @param  nIndex       [OUT] - index of file found in the table
      @param  oError       [OUT] - general error (filled on return false)
      @param  szFileName   [OUT] - name of the original file (null terminated string)
      @param  szFileDir    [OUT] - directory of the original file (null terminated string)
      @return true if successfull, in case of false see oError
 */
static bool cn42_GetFileName
      ( const tsp00_DbNamec & szDbName,
        const tsp00_Pathc   & szType,
        int                 & nIndex,
        DBMSrvMsg_Error     & oError,
        tsp00_Pathc         & szFileName,
        tsp00_Pathc         & szFileDir );

/*!   @brief  computes a filename
      @param  szDbName     [IN]  - name of database (null terminated string)
      @param  szType       [IN]  - ID of requested file (null terminated string)
      @param  nIndex       [OUT] - index of file found in the table
      @param  oError       [OUT] - general error (filled on return false)
      @param  szFileName   [OUT] - name of the original file (null terminated string)
      @param  szFileDir    [OUT] - directory of the original file (null terminated string)
      @param  oFileError   [OUT] - error while computing original file (if the backup file
                                   was computed this will not cause return false)
      @param  szBackupName [OUT] - name of the backup file
      @param  oBackupError [OUT] - error while computing backup file (if the original file
                                   was computed this will not cause return false)
      @return true if successfull, in case of false see oError
 */
static bool cn42_GetFileName
      ( const tsp00_DbNamec & szDbName,
        const tsp00_Pathc   & szType,
        int                 & nIndex,
        DBMSrvMsg_Error     & oError,
        tsp00_Pathc         & szFileName,
        tsp00_Pathc         & szFileDir,
        DBMSrvMsg_Error     & oFileError,
        tsp00_Pathc         & szBackupName,
        DBMSrvMsg_Error     & oBackupError );

static void cn42_ReadTimeStamp
      ( const char *       szSource,
        const char *       szFormat,
        tsp00_Timestampc & sTimeStamp );

static bool cn42_IsLineEmpty
      ( const char *       pLine);

static tcn00_Error cn42_FileOperation
      ( char       *       replyData,
        int        *       replyLen,
        const char *       szFileName,
        const char *       szDate,
        int                nLine,
        int                nIndex,
        bool               bDelete);

static tcn00_Error cn42_FileDelete
      ( char       *       replyData,
        int        *       replyLen,
        const char *       szFileName,
        const char *       szDate);


static tcn00_Error cn42_FileShrink
      ( char       *       replyData,
        int        *       replyLen,
        const char *       szFileName,
        const char *       szDate,
        int                nLine,
        int                nIndex);

static tcn00_Error cn42_FileGetListDir 
      ( char               * replyData,
        long                 nIndex,
        tsp00_Path         & szFileDir,
        tsp00_Pathc        & szFileName,
        tsp00_Path         & szBaseDir,
        char              *& pCurrent,
        int                  replyLenMax);

static const char * cn42_FileType
      ( const tsp05_RteFileInfo  & rteFileInfo,
      long                       nType );

static bool cn42_GetLocation
      ( long                  nLocation,
        const tsp00_DbNamec & szDbName,
        tsp00_Pathc         & szLocation,
        DBMSrvMsg_Error     & oError );

static bool cn42_IsKnlMsg
      ( tsp00_Pathc & szFile );

// extern pascal functions
externPascal void s49build_pattern (
    void                  *  pat_buffer,
    tsp00_Int4                             start,
    tsp00_Int4                             stop,
    char                                 escape_char,
    pasbool                              escape,
    pasbool                              string,
    tsp00_SqlMode_Param                    sqlmode,
    pasbool               VAR_VALUE_REF  ok);

externPascal pasbool s49patmatch (
    void                  *  val,
    tsp00_Int4                             val_offset,
    tsp00_Int4                             val_len,
    void                  *  pat,
    tsp00_Int4                             pat_offset,
    tsp00_Int4                             pat_len,
    char                                 pat_defbyte);


/*
  -----------------------------------------------------------------------------
  implementation public functions
  -----------------------------------------------------------------------------
 */

/*
  -----------------------------------------------------------------------------
  function:     cn42FileGet
  -----------------------------------------------------------------------------
 */
tcn00_Error cn42FileGet
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  tcn00_Error         nFuncReturn = OK_CN00;
  tsp00_Pathc         szFilekey;
  tsp00_Pathc         szParam;
  tsp00_Pathc         szFileName;
  tsp00_Pathc         szBackupName;
  tsp00_Pathc         szFileDirC;
  const char        * pLine      = NULL;
  tsp00_Pathc         szLine;
  const char        * pDate      = NULL;
  tsp00_Pathc         szDate;
  const char        * pType      = NULL;
  tsp00_Pathc         szType;
  long                nType      = FGET_TYP_UNKNOWN;
  int                 nIndex;
  int                 nParam;
  tsp00_Int4          hFile;

  tsp05_RteFileError  rteFileErr;
  tsp05_RteFileInfo   rteFInfo;

  // read the parameters
  cn90GetToken(command->args, szFilekey, 1, szFilekey.size());
  cn90StripQuotes(szFilekey);

  // check for qualifier
  nParam = 2;
  while (cn90GetToken(command->args, szParam, nParam, szParam.size())) {
    if (strnicmp(szParam, FGET_MODE_LINE, strlen(FGET_MODE_LINE)) == 0) {
      szLine.rawAssign(szParam);
      pLine = szLine.asCharp();
      pLine = pLine + strlen(FGET_MODE_LINE);
    } else if (strnicmp(szParam, FGET_MODE_DATE, strlen(FGET_MODE_DATE)) == 0) {
      szDate.rawAssign(szParam);
      pDate = szDate.asCharp();
      pDate = pDate + strlen(FGET_MODE_DATE);
    } else if (strnicmp(szParam, FGET_MODE_TYPE, strlen(FGET_MODE_TYPE)) == 0) {
      szType.rawAssign(szParam);
      pType = szType.asCharp();
      pType = pType + strlen(FGET_MODE_TYPE);
      if (stricmp(pType, FGET_TYPT_ASCII) == 0) {
        nType = FGET_TYP_ASCII;
      } else if (stricmp(pType, FGET_TYPT_BINARY) == 0) {
        nType = FGET_TYP_BINARY;
      } // end if
    } else {
      nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_PARAM_CN00);
      break;
    } // end if
    ++nParam;
  } // end while

  // look for file
  if (nFuncReturn == OK_CN00) {
        DBMSrvMsg_Error oError;
        DBMSrvMsg_Error oFileError;
        DBMSrvMsg_Error oBackupError;

        if (!cn42_GetFileName(vcontrol->dbname, szFilekey, nIndex, oError, szFileName, szFileDirC, oFileError, szBackupName, oBackupError)) {
            oError.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_FILECHK, Msg_Arg(SDBMSGTAG_DBMSRV_FILEOPEN__FILENAME, szFilekey)));
            nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, oError);
        } else {
            if (!oFileError.IsEmpty()) {
                oFileError.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_FILEOPEN, Msg_Arg(SDBMSGTAG_DBMSRV_FILEOPEN__FILENAME, szFilekey)));
                nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, oFileError);
            } else {
                sFGetFiles[nIndex].nType = sFGetFiles[nIndex].nOrgType;
                if (nType != FGET_TYP_UNKNOWN) {
                    sFGetFiles[nIndex].nType = nType;
                } // end if
            } // end if
        } // end if
  } // end if

  // check file type
  if (nFuncReturn == OK_CN00) {
    if ( ( ((pDate != NULL) || (pLine != NULL)) && (!FGET_IS_TEXT(sFGetFiles[nIndex].nType)) ) ||
         ( (pDate != NULL) && (sFGetFiles[nIndex].szTimestampFmt == NULL)     )    ) {
      nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_PARAM_CN00);
    } // end if
  } // end if

  if (nFuncReturn == OK_CN00) {
    // execute fileopen
    sqlfinfoc (szFileName, &rteFInfo, &rteFileErr );
    if (rteFileErr.sp5fe_result != vf_ok) {
      nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(DBMSrvMsg_RTEError(rteFileErr));
    } // end if
  } // end if

  // read contents
  if (nFuncReturn == OK_CN00) {

    if (rteFInfo.sp5fi_media_kind == vf_t_directory) {

      char        * pCurrent  = replyData;
      char        * pLenPos   = NULL;
      char        * pLenStart = NULL;
      tsp00_Path    szFileDir;
      tsp00_Path    szBaseDir;
      char          szTmp[PARAMETER_MAXLEN_CN90];

      // init OK Answer
      sprintf (pCurrent, "%s%s%5d%s", ANSWER_OK_CN00,      LINE_SEPSTRING_CN00,
                                      1,                   LINE_SEPSTRING_CN00);
      pCurrent = pCurrent + strlen(pCurrent);

      sprintf ( pCurrent , "%s%s" , CONT_FLAG_END, LINE_SEPSTRING_CN00 );
      pCurrent = pCurrent + strlen(pCurrent);
      pLenPos  = pCurrent;

      sprintf ( pLenPos, "%20ld%20ld%s" , (long) 0, (long) 0, LINE_SEPSTRING_CN00 );
      pCurrent  = pCurrent + strlen(pCurrent);

      sprintf (pCurrent, "%s", (char   *) FILELIST_TITLE);
      pLenStart = pCurrent;
      pCurrent = pCurrent + strlen(pCurrent);

      SAPDB_strcpy(szBaseDir, szFileDirC);
      SAPDB_strcpy(szFileDir,  szFileName);
      #if defined(_WIN32) || defined(OS2) 
        strcat(szFileDir, "\\");
      #else
        strcat(szFileDir, "/");
      #endif
      SAPDB_strcpy(szFileName, szFileDirC);
      #if defined(_WIN32) || defined(OS2) 
        strcat(szFileName, "*");
      #else
        strcat(szFileName, "*");
      #endif

      nFuncReturn = cn42_FileGetListDir(replyData, nIndex, szFileDir, szFileName, szBaseDir, pCurrent, replyLenMax - (int)(pCurrent - replyData));

      if (nFuncReturn == OK_CN00) {
        sprintf ( szTmp, "%20ld%20ld%s" , (long) strlen(pLenStart), (long) strlen(pLenStart), LINE_SEPSTRING_CN00 );
        strncpy(pLenPos, szTmp, strlen(szTmp));
      } // end if

      *replyLen = (int)strlen(replyData);

    } else {

      if (sFGetFiles[nIndex].hFile != -1) {
        tsp05_RteFileError  rteCloseErr;
        sqlfclosec ( sFGetFiles[nIndex].hFile, sp5vf_close_normal, &rteCloseErr );
        sFGetFiles[nIndex].hFile = -1;
      } // end if

      // if protconv?
      if (sFGetFiles[nIndex].szProtconvOpt != NULL) {

        bool        bProtConv  = false;
        tsp00_Pathc szFileCopy;

        szFileCopy.Init();

        // if it is the DIAGDIR so check for the filename
        if (strcmp(sFGetFiles[nIndex].szType, FGET_DIAGDIR_CN42) == 0) {
          if (cn42_IsKnlMsg(szFileName)) {
            bProtConv = true;
            sprintf(szFileCopy, "%s%c%s", szFileDirC.asCharp(), PATH_DELIMITER_CN90, SAPDB_KERNEL_MESSAGES_FILE_NAME);
          } // end if
        } else {
          bProtConv  = true;
        } // end if

        // if this is a protconv file convert it
        if (bProtConv) {
          tsp00_Pathc        szBinDir;
          tsp01_RteError     aRteError;

          if(!sqlGetIndependentBinPath(szBinDir, TERM_WITH_DELIMITER_EO01, &aRteError)) {
            nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(DBMSrvMsg_NewRTEError(aRteError));
          } // end if


          if (nFuncReturn == OK_CN00) {
            // erase a existing protconv destination
            tsp05_RteFileError rteFileErr;
            sqlferasec (szBackupName, &rteFileErr);

            // copy file if required
            if (szFileCopy.length() > 0) {
              sqlferasec (szFileCopy, &rteFileErr);
              tsp01_RteError      aNewRteError;
              if (!sqlfilecopyc( szFileName, szFileCopy, &aNewRteError )) {
                nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(DBMSrvMsg_NewRTEError(aNewRteError));
              } // end if
            } // end if
          } // end if

          if (nFuncReturn == OK_CN00) {
            // generate the file
            char               szCommand [PARAMETER_MAXLEN_CN90];
            cn90StripTrailingChar(szFileDirC, PATH_DELIMITER_CN90);
            sprintf(szCommand, PROTCONV_CALL, szBinDir.asCharp(), szFileDirC.asCharp(), sFGetFiles[nIndex].szProtconvOpt, szBackupName.asCharp());
            nFuncReturn =  cn40ExecCommand(replyData, replyLen, replyLenMax, szCommand, szCommand, false, true);

            if (szFileCopy.length() > 0) {
              // erase the copy
              tsp05_RteFileError rteFileErr;
              sqlferasec (szFileCopy, &rteFileErr);
            } // end if

            if (nFuncReturn == OK_CN00) {
              // check existence of destination after protconv
              sqlfinfoc (szBackupName, &rteFInfo, &rteFileErr );
              if (rteFileErr.sp5fe_result != vf_ok) {
                nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(DBMSrvMsg_RTEError(rteFileErr));
              } else {
                szFileName = szBackupName;
                sFGetFiles[nIndex].nType = FGET_IS_TEXT(sFGetFiles[nIndex].nType) ? sFGetFiles[nIndex].nType : FGET_TYP_ASCII;
              } // end if
            } // end if
          } // end
        } // end
      } // end if

      if (nFuncReturn == OK_CN00) {
        sqlfopenc (szFileName, FGET_IS_TEXT(sFGetFiles[nIndex].nType) ? sp5vf_text : sp5vf_binary,
                    sp5vf_read, sp5bk_unbuffered, &hFile, &rteFileErr );

        if (rteFileErr.sp5fe_result != vf_ok) {
            nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(DBMSrvMsg_RTEError(rteFileErr));
        } else {
            sFGetFiles[nIndex].hFile = hFile;
            if ((pLine != NULL) || (pDate != NULL)) {
            nFuncReturn = cn42_FileReadFirst( replyData,
                                                replyLen,
                                                replyLenMax,
                                                hFile,
                                                rteFInfo.sp5fi_size,
                                                nIndex,
                                                pDate,
                                                (pLine == NULL) ? 0 : atoi(pLine),
                                                (sFGetFiles[nIndex].szProtconvOpt != NULL) ? szBackupName.asCharp() : NULL);
            } else {
            nFuncReturn = cn42_FileReadNext( replyData,
                                            replyLen,
                                            replyLenMax,
                                            hFile,
                                            rteFInfo.sp5fi_size,
                                            false,
                                            FGET_IS_TEXT(sFGetFiles[nIndex].nType),
                                            sFGetFiles[nIndex].bRemoveEmptyLines,
                                            nIndex,
                                            (sFGetFiles[nIndex].szProtconvOpt != NULL) ? szBackupName.asCharp() : NULL);
            } // end if
        } // end if
      } // end if

    } // end if
  } // end if


  return nFuncReturn;

} // end cn42FileGet

/*
  -----------------------------------------------------------------------------
  function:     cn42FileGetNext
  -----------------------------------------------------------------------------
 */
tcn00_Error cn42FileGetNext
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  tcn00_Error         nFuncReturn = OK_CN00;
  tsp00_Pathc         szType;
  tsp00_Pathc         szFileName;
  tsp00_Pathc         szBackupName;
  tsp00_Pathc         szFileDir;
  char                szHandle   [PARAMETER_MAXLEN_CN90] = "";
  int                 nIndex;

  // read the parameters
  if (!cn90GetToken(command->args, szType,   1, szType.size())    ||
      !cn90GetToken(command->args, szHandle, 2, PARAMETER_MAXLEN_CN90)    ||
       cn90GetToken(command->args, NULL,     3, -1)    ||
       vcontrol->dbname[0] == CHAR_STRINGTERM_CN90    ) {
    nFuncReturn = ERR_PARAM_CN00;
    cn90AnswerIError(replyData, replyLen, nFuncReturn);
  } // end if
  cn90StripQuotes(szType);

  // look for file
  if (nFuncReturn == OK_CN00) {
        DBMSrvMsg_Error oError;
        DBMSrvMsg_Error oFileError;
        DBMSrvMsg_Error oBackupError;

        if (!cn42_GetFileName(vcontrol->dbname, szType, nIndex, oError, szFileName, szFileDir, oFileError, szBackupName, oBackupError))
            nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, oError);
  } // end if

  // read contents
  if (nFuncReturn == OK_CN00) {
    nFuncReturn = cn42_FileReadNext( replyData,
                                     replyLen,
                                     replyLenMax,
                                     atol(szHandle),
                                     0,
                                     false,
                                     FGET_IS_TEXT(sFGetFiles[nIndex].nType),
                                     sFGetFiles[nIndex].bRemoveEmptyLines,
                                     nIndex,
                                     (sFGetFiles[nIndex].szProtconvOpt != NULL) ? szBackupName.asCharp() : NULL);
    if (nFuncReturn != OK_CN00)
      cn90AnswerIError(replyData, replyLen, nFuncReturn);
  } // end if

  return nFuncReturn;
} // end cn42FileGetNext

/*
  -----------------------------------------------------------------------------
  function:     cn42FileGetList
  -----------------------------------------------------------------------------
 */
tcn00_Error cn42FileGetList
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  tcn00_Error         nFuncReturn  = OK_CN00;
  long                nIndex       = 0;
  char              * pCurrent     = replyData;
  tsp05_RteFileInfo   rteFileInfo;
  tsp05_RteFileError  rteFileErr;
  tsp00_Pathc         szFileName;
  tsp00_Pathc         szFullReal;
  tsp00_Path          szFileDir;
  tsp00_Pathc         szTemp;
  DBMSrvMsg_Error     oError;
  char                szShow[10];
  int                 nShow = 0;
  int                 nFileIndex;
  tsp00_Pathc         szType;
  void              * hDirectory  = NULL;
  int                 nLength;

  if (cn90GetToken(command->args, szShow, 1, sizeof(szShow))) {
    nShow = atoi(szShow);
  } // end if

  // init OK Answer
  _stprintf (pCurrent, "%s%s%s", ANSWER_OK_CN00, LINE_SEPSTRING_CN00, (char   *) FILELIST_TITLE);
  pCurrent = pCurrent + strlen(pCurrent);

    // search for Type in file array
  while ((sFGetFiles[nIndex].szType != NULL) && (nFuncReturn == OK_CN00)) {
    SAPDB_strcpy(szType, sFGetFiles[nIndex].szType);
    if ((sFGetFiles[nIndex].nShow <= nShow) &&
        (sFGetFiles[nIndex].nType != FGET_TYP_OBSOLETE) &&
        cn42_GetFileName(vcontrol->dbname, szType, nFileIndex, oError, szFileName, szTemp)) {

      sFGetFiles[nIndex].nType = sFGetFiles[nIndex].nOrgType;
      SAPDB_strcpy(szFileDir, szTemp);

      if (strstr(szFileName, FGET_WILDCARD) != NULL) {
        nFuncReturn = cn42_FileGetListDir(replyData, nIndex, szFileDir, szFileName, szFileDir, pCurrent, replyLenMax - (int) (pCurrent - replyData));
      } else {
        sqlfinfoc (szFileName, &rteFileInfo, &rteFileErr );

        if (rteFileInfo.sp5fi_exists) {
          // check buffer size
          nLength = (int) (strlen(sFGetFiles[nIndex].szType) +
                           (6 * strlen(VALUE_SEPSTRING_CN00)) +
                           strlen(cn42_FileType(rteFileInfo, sFGetFiles[nFileIndex].nType)) +
                           20 + // rteFileInfo.sp5fi_size
                           sizeof (rteFileInfo.sp5fi_date_modified) +
                           sizeof (rteFileInfo.sp5fi_time_modified) +
                           strlen(sFGetFiles[nIndex].szComment) +
                           strlen(szFullReal  + _tcslen(szFileDir)) +
                           strlen(LINE_SEPSTRING_CN00) );
          if ((replyLenMax - (pCurrent - replyData)) < nLength) {
            // error
            nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, DBMSrvMsg_Error(SDBMSG_DBMSRV_TOOMANYFILES));
          } else {
            _stprintf (pCurrent, "%s%s%s%s%ld%s%.*s%s%.*s%s%s%s%s%s",
                                 (char   *) sFGetFiles[nIndex].szType,
                                 VALUE_SEPSTRING_CN00,
                                 (char   *) cn42_FileType(rteFileInfo, sFGetFiles[nFileIndex].nType),
                                 VALUE_SEPSTRING_CN00,
                                 (long) rteFileInfo.sp5fi_size,
                                 VALUE_SEPSTRING_CN00,
                                 sizeof (rteFileInfo.sp5fi_date_modified), (char   *) rteFileInfo.sp5fi_date_modified,
                                 VALUE_SEPSTRING_CN00,
                                 sizeof (rteFileInfo.sp5fi_time_modified), (char   *) rteFileInfo.sp5fi_time_modified,
                                 VALUE_SEPSTRING_CN00,
                                 (char   *) sFGetFiles[nIndex].szComment,
                                 VALUE_SEPSTRING_CN00,
                                 (char   *) szFileName + strlen(szFileDir),
                                 LINE_SEPSTRING_CN00);
            pCurrent = pCurrent + strlen(pCurrent);
          } // end if
        } // end if
      } // end if

    } // end if

    nIndex++;
  } // end while

  *replyLen = (int)strlen(replyData);

  return OK_CN00;
} // end cn42FileGetList

/*
  -----------------------------------------------------------------------------
  function:     cn42FileOperation
  -----------------------------------------------------------------------------
 */
tcn00_Error cn42FileOperation
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  tcn00_Error         nFuncReturn = OK_CN00;
  tsp00_Pathc         szType;
  tsp00_Pathc         szFileName;
  tsp00_Pathc         szParam;
  int                 nToken;
  int                 nIndex;
  tsp00_C40c          szDate;
  tsp00_C40c          szOperation;
  tsp00_C40c          szLine;

  // read file id
  cn90GetToken(command->args, szType, 1, szType.size());

  // read qualifiers
  nToken = 2;
  szDate.Init();
  szLine.Init();
  szOperation.Init();
  while (cn90GetToken(command->args, szParam, nToken, szParam.size())) {
    if (strncmp(szParam, FGET_MODE_DATE, strlen(FGET_MODE_DATE)) == 0) {
      szDate.rawAssign(szParam.asCharp() + strlen(FGET_MODE_DATE));
    } else if (strncmp(szParam, FGET_MODE_OPERATION, strlen(FGET_MODE_OPERATION)) == 0) {
      szOperation.rawAssign(szParam.asCharp() + strlen(FGET_MODE_OPERATION));
    } else if (strncmp(szParam, FGET_MODE_LINE, strlen(FGET_MODE_LINE)) == 0) {
      szLine.rawAssign(szParam.asCharp() + strlen(FGET_MODE_LINE));
    } else {
      nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_PARAM_CN00);
    } // end if
    ++nToken;
  } // end while

  // check operation
  if (nFuncReturn == OK_CN00) {
    if (szOperation.length() == 0 ||
        ((strcmp(szOperation, FGET_OP_SHRINK) != 0) && (strcmp(szOperation, FGET_OP_DELETE) != 0))) {
      nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_PARAM_CN00);
    } // end if
  } // end if

  // look for file
  if (nFuncReturn == OK_CN00) {
    tsp00_Pathc     szFileDir;
    DBMSrvMsg_Error oError;
    if (!cn42_GetFileName(vcontrol->dbname, szType, nIndex, oError, szFileName, szFileDir)) 
        nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, oError);
  } // end if

  // check operation flag
  if (nFuncReturn == OK_CN00) {
    if (!sFGetFiles[nIndex].bOperation ) {
      nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_PARAM_CN00);
    } // end if
  } // end if

  // check file type and timestamp settings
  if (nFuncReturn == OK_CN00) {
    if ( ( ((strlen(szDate) != 0) || (strlen(szLine) != 0)) && (!FGET_IS_TEXT(sFGetFiles[nIndex].nType)) ) ||
         ( (strlen(szDate) != 0) && (sFGetFiles[nIndex].szTimestampFmt == NULL) && (strcmp(szOperation, FGET_OP_SHRINK) == 0) ) ) {
      nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_PARAM_CN00);
    } // end if
  } // end if

  // execute operation
  if (nFuncReturn == OK_CN00) {
    if (strcmp(szOperation, FGET_OP_DELETE) == 0) {
      nFuncReturn = cn42_FileOperation( replyData, replyLen, szFileName, szDate, atoi(szLine), nIndex, true);
    } else if (strcmp(szOperation, FGET_OP_SHRINK) == 0) {
      nFuncReturn = cn42_FileOperation( replyData, replyLen, szFileName, szDate, atoi(szLine), nIndex, false);
    } // end if
  } // end if

  if (nFuncReturn == OK_CN00) {
    cn90AnswerOK (replyData, replyLen, NULL);
  } // end if

  *replyLen = (int) strlen(replyData);

  return nFuncReturn;

} // end cn42FileOperation

/*
  -----------------------------------------------------------------------------
  function:     cn42FileBackup
  -----------------------------------------------------------------------------
 */
tcn00_Error cn42FileBackup
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  tsp00_Pathc         szType;
  DBMSrvMsg_Error     oError;

  // read the parameters
  cn90GetToken(command->args, szType, 1, szType.size());

  if (!cn42BackupFile(vcontrol->dbname, szType, oError))
      return cn90AnswerEvent(replyData, replyLen, replyLenMax, oError);

  return cn90AnswerOK(replyData, replyLen, NULL);
} // end cn42FileBackup

/*
  -----------------------------------------------------------------------------
  function:     cn42FileRestore
  -----------------------------------------------------------------------------
 */
tcn00_Error cn42FileRestore
      ( VControlDataT * vcontrol,
        CommandT      * command,
        char          * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  tsp00_Pathc         szType;
  DBMSrvMsg_Error     oError;

  // read the parameters
  cn90GetToken(command->args, szType, 1, szType.size());

  if (!cn42RestoreFile(vcontrol->dbname, szType, oError))
      return cn90AnswerEvent(replyData, replyLen, replyLenMax, oError);

  return cn90AnswerOK(replyData, replyLen, NULL);
} // end cn42FileRestore

/*
  -----------------------------------------------------------------------------
  function:     cn42DiagHistList
  -----------------------------------------------------------------------------
 */
tcn00_Error cn42DiagHistList
      ( VControlDataT * vcontrol,
        CommandT      * command,
        _TCHAR        * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  FUNCTION_DBG_MCN00_1("vcn42.cpp :: cn42DiagHistList");

  tcn00_Error         nFuncReturn = OK_CN00;
  tcn002_XpValueString szDiagPath;
  tsp00_C256          szDiagPathP;
  tsp00_C256          szFile;
  tsp00_Pathc         szDiagFile;
  tsp05_RteFileError  rteFileErr;
  tsp05_RteFileInfo   rteFInfo;
  char *              pCurrent = replyData;
  int                 nOffset     = vcontrol->dbname.length();
  void              * hDirectory  = NULL;  
  char                szTimestamp [PARAMETER_MAXLEN_CN90] = "";
  unsigned int        i = 0;
  unsigned int        j = 0;
  DBMSrvMsg_Error     oParamGetMsg;

  // get the diag directory
  if (cn20XParamGetValue(vcontrol->dbname, PAN_CRASH_HISTORY_PATH, szDiagPath, oParamGetMsg)) {

    // init OK Answer
    _stprintf (pCurrent, "%s%s", ANSWER_OK_CN00, LINE_SEPSTRING_CN00);
    pCurrent = pCurrent + strlen(pCurrent);

    if (cn90GetToken(command->args, szTimestamp, 1, PARAMETER_MAXLEN_CN90)) {
      // list the directory 
      sprintf(szDiagPathP, "%s%c%s%s", szDiagPath.asCharp(), 
                                      PATH_DELIMITER_CN90,
                                      vcontrol->dbname.asCharp(),
                                      DIAG_HIST_PATTERN);
      for (i = 0,j= 0; i < strlen(szDiagPathP); ++i) {
        szDiagPathP[i] = (szDiagPathP[i] == '?') ? szTimestamp[j++] : szDiagPathP[i];
      } // end if

      sqlfopendirc (&szDiagPathP, &hDirectory, &szFile, &rteFileErr);
      if (rteFileErr.sp5fe_result == vf_ok) {
        while (rteFileErr.sp5fe_result == vf_ok) {

            if ((replyLenMax - (pCurrent - replyData)) > (2 + (int) strlen(szFile))) {
              sprintf(szDiagFile, "%s%c%s",
                                  szDiagPathP.asCharp(),
                                  PATH_DELIMITER_CN90,
                                  szFile.asCharp());

              sqlfinfoc (szDiagFile.asCharp(), &rteFInfo, &rteFileErr );
              if (rteFileErr.sp5fe_result == vf_ok) {
                if (rteFInfo.sp5fi_media_kind == vf_t_file) {
                  sprintf(pCurrent, "%s%s%s%c%s%s",
                                     FGET_DIAGHIST_CN42.asCharp(),
                                     FGET_TOKENDEL,
                                     &szDiagPathP[strlen(szDiagPathP) - strlen(DIAG_HIST_PATTERN) + 1],
                                     PATH_DELIMITER_CN90,
                                     szFile.asCharp(),
                                     LINE_SEPSTRING_CN00);
                  pCurrent = pCurrent + strlen(pCurrent);
                } // end if
              } // end if
            } // end if

          sqlfreaddirc(hDirectory, &szFile, &rteFileErr);
        } // end while

        sqlfclosedirc (hDirectory, &rteFileErr);
      } // end if

    } else {

      // list the directory 
      SAPDB_strcpy(szDiagPathP, szDiagPath);
      sqlfopendirc (&szDiagPathP, &hDirectory, &szFile, &rteFileErr);
      if (rteFileErr.sp5fe_result == vf_ok) {
        while (rteFileErr.sp5fe_result == vf_ok) {
      
          // search for the database directories an list them
          if (cn42_DiagFileMatch(vcontrol->dbname.asCharp(), szFile)) {

            if ((replyLenMax - (pCurrent - replyData)) > (18 + (int) strlen(szFile))) {
              sprintf(pCurrent, "%.*s%.*s%.*s%.*s %s%s%c%s%s",
                                 DIAG_LEN_DATE,
                                 &szFile[nOffset + DIAG_POS_DATE],
                                 DIAG_LEN_HOUR,
                                 &szFile[nOffset + DIAG_POS_HOUR],
                                 DIAG_LEN_MINUTE,
                                 &szFile[nOffset + DIAG_POS_MINUTE],
                                 DIAG_LEN_SECOND,
                                 &szFile[nOffset + DIAG_POS_SECOND],
                                 VALUE_SEPSTRING_CN00,
                                 szDiagPath.asCharp(),
                                 PATH_DELIMITER_CN90,
                                 szFile.asCharp(),
                                 LINE_SEPSTRING_CN00);
              pCurrent = pCurrent + strlen(pCurrent);
            } // end if
          } // end if

          sqlfreaddirc(hDirectory, &szFile, &rteFileErr);
        } // end while

        sqlfclosedirc (hDirectory, &rteFileErr);
      } // end if
    
    } // end if

    if (rteFileErr.sp5fe_result == vf_notok) {
      DBMSrvMsg_RTEError aEvent(rteFileErr);
      aEvent.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_FILEOPEN, 
                      Msg_Arg(SDBMSGTAG_DBMSRV_FILEOPEN__FILENAME,szDiagPath)));
      aEvent.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_FILE));
      nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, aEvent);
    } // end if

  } else {
    oParamGetMsg.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_XPNOTFOUND, Msg_Arg(SDBMSGTAG_DBMSRV_XPNOTFOUND__PARAMETER, PAN_CRASH_HISTORY_PATH)));
    nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, oParamGetMsg);
  } // end if

  *replyLen = (int) strlen(replyData);

  return nFuncReturn;
} // end cn42DiagHistList

/*
  -----------------------------------------------------------------------------
  function:     cn42DiagPack
  -----------------------------------------------------------------------------
 */
tcn00_Error cn42DiagPack
      ( VControlDataT * vcontrol,
        CommandT      * command,
        _TCHAR        * replyData,
        int           * replyLen,
        int             replyLenMax)
{
  FUNCTION_DBG_MCN00_1("vcn42.cpp :: cn42DiagPack");

  tcn00_Error         nFuncReturn = OK_CN00;
  char                szToken     [PARAMETER_MAXLEN_CN90] = "";
  const char *        pClass;
  tsp00_Timestampc    szTimestamp;
  tsp00_Pathc         szArchive;
  tsp00_Pathc         szDiagFile;
  tsp00_Pathc         szDiagFileTarget;
  int                 nToken = 1;
  bool                bPRT   = false;
  bool                bLVC   = false;
  bool                bBKP   = false;
  bool                bCFG   = false;
  bool                bDUMP  = false;
  bool                bHIST  = false;
  cn54DBMTgz        * pTGZ   = NULL;

  // read the command parameters
  szArchive.Init();
  szTimestamp.Init();
  while (cn90GetToken(command->args, szToken, nToken, PARAMETER_MAXLEN_CN90) && (nFuncReturn == OK_CN00)) {
    cn90StripQuotes(szToken);
    if (strncmp(szToken, DIAG_FILE, DIAG_FILE_L) == 0 || strncmp(szToken, DIAG_file, DIAG_FILE_L) == 0 ) { 
      // read filename
      szArchive.rawAssign(&szToken[DIAG_FILE_L]);
      cn90StripQuotes(szArchive);
    } else if (strncmp(szToken, DIAG_DATE, DIAG_DATE_L) == 0 || strncmp(szToken, DIAG_date, DIAG_DATE_L) == 0 ) { 
      // read timestamp
      szTimestamp.rawAssign(&szToken[DIAG_DATE_L]);
    } else if (strncmp(szToken, DIAG_CLASS, DIAG_CLASS_L) == 0 || strncmp(szToken, DIAG_class, DIAG_CLASS_L) == 0 ) { 
      // read classes
      pClass = strtok( &szToken[DIAG_CLASS_L], DIAG_CLASS_SEP );
      while( pClass != NULL ) {
        if (cn90Stricmp(pClass, DIAG_CLASS_PRT) == 0) {
          bPRT = true;
        } else if (cn90Stricmp(pClass, DIAG_CLASS_LVC) == 0) {
          bLVC = true;
        } else if (cn90Stricmp(pClass, DIAG_CLASS_BKP) == 0) {
          bBKP = true;
        } else if (cn90Stricmp(pClass, DIAG_CLASS_CFG) == 0) {
          bCFG = true;
        } else if (cn90Stricmp(pClass, DIAG_CLASS_DUMP) == 0) {
          bDUMP = true;
        } else if (cn90Stricmp(pClass, DIAG_CLASS_HIST) == 0) {
          bHIST = true;
        } else if (cn90Stricmp(pClass, DIAG_CLASS_ALL) == 0) {
          bPRT = true;
          bLVC = true;
          bBKP = true;
          bCFG = true;
          bDUMP = true;
        } else {
          nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_PARAM_CN00);
        } // end if
        pClass = strtok( NULL, DIAG_CLASS_SEP );
      } // end while
    } else  { 
      nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_PARAM_CN00);
    } // end if
    ++nToken;
  } // end while

  // default class
  if (nFuncReturn == OK_CN00) {
    if (!bPRT && !bLVC && !bBKP && !bCFG  && !bDUMP && !bHIST) {
      bPRT = true;
    } // end if
  } // end if

  // check hist + timestamp
  if (nFuncReturn == OK_CN00) {
    if ( (bHIST && (szTimestamp.length() == 0)) || (!bHIST && (szTimestamp.length() != 0)) ) {
      nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_PARAM_CN00);
    } // end if
  } // end if

  // check the archive name
  if ((nFuncReturn == OK_CN00) && (szArchive.length() == 0)) {
      int             nIndex;
      tsp00_Pathc     szFileDir;
      DBMSrvMsg_Error oError;
      if (!cn42_GetFileName(vcontrol->dbname, FGET_DIAGPACK_CN42, nIndex, oError, szArchive, szFileDir))
         nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, oError);
  } // end if

  // open the archive
  if (nFuncReturn == OK_CN00) {
    pTGZ = new cn54DBMTgz(szArchive);
    if (pTGZ == NULL) {
      nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_MEM_CN00);
    } else if (!pTGZ->lastEvent().IsEmpty()) {
      nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, pTGZ->lastEvent());
    } // end if
  } // end if

  // HISTORY files 
  if ((nFuncReturn == OK_CN00) && bHIST) {
    tsp05_RteFileError  rteFileErr;
    tsp05_RteFileInfo   rteFInfo;
    void              * hDirectory  = NULL;  
    tcn002_XpValueString szDiagPath;
    tsp00_C256          szDiagPathP;
    unsigned int        i      = 0;
    unsigned int        j      = 0;
    tsp00_C256          szFile;
    DBMSrvMsg_Error     oParamGetMsg;

    if (cn20XParamGetValue(vcontrol->dbname, PAN_CRASH_HISTORY_PATH, szDiagPath, oParamGetMsg)) {
      // create the directory name
      sprintf(szDiagPathP, "%s%c%s%s", szDiagPath.asCharp(), 
                                      PATH_DELIMITER_CN90,
                                      vcontrol->dbname.asCharp(),
                                      DIAG_HIST_PATTERN);
      for (i = 0,j= 0; i < strlen(szDiagPathP); ++i) {
        szDiagPathP[i] = (szDiagPathP[i] == '?') ? szTimestamp[j++] : szDiagPathP[i];
      } // end if

      // read the directory
      sqlfopendirc (&szDiagPathP, &hDirectory, &szFile, &rteFileErr);
      if (rteFileErr.sp5fe_result == vf_ok) {
        while ((rteFileErr.sp5fe_result == vf_ok) && (nFuncReturn == OK_CN00)) {
          sprintf(szDiagFile,       "%s%c%s", szDiagPathP.asCharp(), PATH_DELIMITER_CN90, szFile.asCharp());
          szDiagFileTarget.rawAssign(&szDiagFile[szDiagFile.length()       - 
                                                 strlen(szFile)            -
                                                 vcontrol->dbname.length() -
                                                 strlen(DIAG_HIST_PATTERN) -
                                                 1]);
          sqlfinfoc (szDiagFile.asCharp(), &rteFInfo, &rteFileErr );
          if (rteFileErr.sp5fe_result == vf_ok) {
            if (rteFInfo.sp5fi_media_kind == vf_t_file) {
              if (!pTGZ->append(szDiagFile, szDiagFileTarget)) {
                nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, pTGZ->lastEvent());
              } // end if
            } // end if
          } // end if
          sqlfreaddirc(hDirectory, &szFile, &rteFileErr);
        } // end while

        sqlfclosedirc (hDirectory, &rteFileErr);
      } // end if
      if (rteFileErr.sp5fe_result != vf_ok) {
        DBMSrvMsg_RTEError aEvent(rteFileErr);
        aEvent.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_FILEOPEN, 
                        Msg_Arg(SDBMSGTAG_DBMSRV_FILEOPEN__FILENAME,szDiagPath)));
        aEvent.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_FILE));
        nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, aEvent);
      } // end if

    } else {
      oParamGetMsg.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_XPNOTFOUND, Msg_Arg(SDBMSGTAG_DBMSRV_XPNOTFOUND__PARAMETER, PAN_CRASH_HISTORY_PATH)));
      nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, oParamGetMsg);
    } // end if
  } // end if

  // search for the other files
  if ((nFuncReturn == OK_CN00) && (bPRT || bLVC || bBKP || bCFG || bDUMP)) {
    int                 nIndex = 0;
    tsp00_Pathc         szType;
    tsp00_Pathc         szFileName;
    int                 nFileIndex;
    tsp00_Pathc         szFileDir;
    DBMSrvMsg_Error     oError;
    tsp00_C256          szFileDirP;
    tsp00_C256          szRealName;
    void              * hDirectory  = NULL;  
    tsp05_RteFileInfo   rteFileInfo;
    tsp05_RteFileError  rteFileErr;
    tsp00_Pathc         szToken;
    tsp00_Pathc         szFullReal;

    while ((sFGetFiles[nIndex].szType != NULL) && (nFuncReturn == OK_CN00)) {
      if (sFGetFiles[nIndex].szClass != NULL) {
        if ( ((strcmp(sFGetFiles[nIndex].szClass, DIAG_CLASS_PRT)  == 0) && bPRT)  ||
             ((strcmp(sFGetFiles[nIndex].szClass, DIAG_CLASS_LVC)  == 0) && bLVC)  ||
             ((strcmp(sFGetFiles[nIndex].szClass, DIAG_CLASS_BKP)  == 0) && bBKP)  ||
             ((strcmp(sFGetFiles[nIndex].szClass, DIAG_CLASS_CFG)  == 0) && bCFG)  ||
             ((strcmp(sFGetFiles[nIndex].szClass, DIAG_CLASS_DUMP) == 0) && bDUMP)    ) {
          // class match
          szType.rawAssign(sFGetFiles[nIndex].szType);
          if (cn42_GetFileName(vcontrol->dbname, szType, nFileIndex, oError, szFileName, szFileDir)) {

            if (strstr(szFileName, FGET_WILDCARD) != NULL) {

              // multi file append (scan directory for matches)
              SAPDB_strcpy(szFileDirP, szFileDir);
              sqlfopendirc (&szFileDirP, &hDirectory, &szRealName, &rteFileErr);
              if (rteFileErr.sp5fe_result == vf_ok) {
                while (rteFileErr.sp5fe_result == vf_ok) {
                  SAPDB_strcpy(szFullReal, szFileDir);
                  strcat(szFullReal, szRealName);
                  if (cn42_FileMatch(szFileName, szFullReal, szToken)) {
                    szDiagFile.rawAssign(szFullReal.asCharp());
                    szDiagFileTarget.rawAssign(&szFullReal[szFileDir.length()]);
                    if (!pTGZ->append(szDiagFile, szDiagFileTarget)) {
                      nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, pTGZ->lastEvent());
                    } // end if
                  } // end if
                  sqlfreaddirc(hDirectory, &szRealName, &rteFileErr);
                } // end while
                sqlfclosedirc (hDirectory, &rteFileErr);
              } // end if

            } else {

              // single file append
              sqlfinfoc (szFileName, &rteFileInfo, &rteFileErr );
              if (rteFileInfo.sp5fi_exists) {
                szDiagFile.rawAssign(szFileName.asCharp());
                szDiagFileTarget.rawAssign(&szFileName[szFileDir.length()]);
                if (!pTGZ->append(szDiagFile, szDiagFileTarget)) {
                  nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, pTGZ->lastEvent());
                } // end if
              } // end if

            } // end if

          } // end if
        } // end if
      } // end if
      ++nIndex;
    } // end while
  } // end if

  // answer
  if (pTGZ != NULL) {
    delete pTGZ;
  } // end if

  if (nFuncReturn == OK_CN00) {
    cn90AnswerOK (replyData, replyLen, NULL);
  } // end if

  return nFuncReturn;
} // end cn42DiagPack

/*
  -----------------------------------------------------------------------------
  function:     cn42BackupFile
  -----------------------------------------------------------------------------
 */
bool cn42BackupFile
      ( const tsp00_DbNamec & szDbName,
        const tsp00_Pathc   & szType,
        DBMSrvMsg_Error     & oError )
{
    int                 nIndex;
    tsp00_Pathc         szFileName;
    tsp00_Pathc         szBackupName;
    tsp00_Pathc         szFileDir;

    DBMSrvMsg_Error     oFileError;
    DBMSrvMsg_Error     oBackupError;

    tsp01_RteError      aNewRteError;
    tsp05_RteFileError  aRteFileError;
    tsp05_RteFileInfo   aRteFileInfo;

    // backup one file
    if (!cn42_GetFileName(szDbName, szType, nIndex, oError, szFileName, szFileDir, oFileError, szBackupName, oBackupError)) 
        return false;

    if (!oFileError.IsEmpty()) {
        oError = oFileError;
        return false;
    } // end if

    if (!oBackupError.IsEmpty()) {
        oError = oBackupError;
        return false;
    } // end if

    sqlfinfoc (szFileName, &aRteFileInfo, &aRteFileError);
    if (aRteFileError.sp5fe_result != vf_ok) {
        oError = DBMSrvMsg_RTEError(aRteFileError);
        return false;
    } // end if

    if (!aRteFileInfo.sp5fi_exists) {
       oError = DBMSrvMsg_Error(SDBMSG_DBMSRV_FILECHK, Msg_Arg(SDBMSGTAG_DBMSRV_FILECHK__FILENAME, szFileName));
       return false;
    } // end if

    // copy the file
    if (!sqlfilecopyc( szFileName, szBackupName, &aNewRteError )) {
      oError = DBMSrvMsg_RTEError(aRteFileError);
      return false;
    } // end if

    return true;
} // end cn42BackupFile

/*
  -----------------------------------------------------------------------------
  function:     cn42RestoreFile
  -----------------------------------------------------------------------------
 */
bool cn42RestoreFile
      ( const tsp00_DbNamec & szDbName,
        const tsp00_Pathc   & szType,
        DBMSrvMsg_Error     & oError )
{
    int                 nIndex;
    tsp00_Pathc         szFileName;
    tsp00_Pathc         szBackupName;
    tsp00_Pathc         szFileDir;

    DBMSrvMsg_Error     oFileError;
    DBMSrvMsg_Error     oBackupError;

    tsp01_RteError      aNewRteError;
    tsp05_RteFileError  aRteFileError;
    tsp05_RteFileInfo   aRteFileInfo;

    // backup one file
    if (!cn42_GetFileName(szDbName, szType, nIndex, oError, szFileName, szFileDir, oFileError, szBackupName, oBackupError)) 
        return false;

    if (!oFileError.IsEmpty()) {
        oError = oFileError;
        return false;
    } // end if

    if (!oBackupError.IsEmpty()) {
        oError = oBackupError;
        return false;
    } // end if

    sqlfinfoc (szBackupName, &aRteFileInfo, &aRteFileError);
    if (aRteFileError.sp5fe_result != vf_ok) {
        oError = DBMSrvMsg_RTEError(aRteFileError);
        return false;
    } // end if

    if (!aRteFileInfo.sp5fi_exists) {
       oError = DBMSrvMsg_Error(SDBMSG_DBMSRV_FILECHK, Msg_Arg(SDBMSGTAG_DBMSRV_FILECHK__FILENAME, szBackupName));
       return false;
    } // end if

    // copy the file
    if (!sqlfilecopyc( szBackupName, szFileName, &aNewRteError )) {
      oError = DBMSrvMsg_RTEError(aRteFileError);
      return false;
    } // end if

    return true;
} // end cn42RestoreFile

/*
  -----------------------------------------------------------------------------
  public function cn42GetFileName
  -----------------------------------------------------------------------------
 */
tcn00_Error cn42GetFileName
      ( const tsp00_DbNamec & szDbName,
        const tsp00_Pathc   & szType,
        tsp00_Pathc         & szFileName,
        tsp00_Pathc         & szBackupName)
{
    DBMSrvMsg_Error  oError; 
    DBMSrvMsg_Error  oFileError; 
    DBMSrvMsg_Error  oBackupError; 
    int              nIndex;
    tsp00_Pathc      szFileDir;

    if (!cn42_GetFileName(szDbName, szType, nIndex, oError, szFileName, szFileDir, oFileError, szBackupName, oBackupError)) {
        return oError.ID();
    } // end if

    return OK_CN00;
} // end cn42GetFileName
/*
  -----------------------------------------------------------------------------
 */
tcn00_Error cn42GetFileName
      ( const tsp00_DbNamec & szDbName,
        const tsp00_Pathc   & szType,
        tsp00_Pathc         & szFileName)
{
    DBMSrvMsg_Error  oError; 
    int              nIndex;

    if (!cn42_GetFileName(szDbName, szType, nIndex, oError, szFileName)) {
        return oError.ID();
    } // end if

    return OK_CN00;
} // end cn42GetFileName
/*
  -----------------------------------------------------------------------------
 */
bool cn42GetFileName
      ( const tsp00_DbNamec & szDbName,
        const tsp00_Pathc   & szType,  
        tsp00_Pathc         & szFileName,
        DBMSrvMsg_Error     & oError ) 
{
    int         nIndex;
    return cn42_GetFileName(szDbName, szType, nIndex, oError, szFileName);
} // end cn42GetFileName

/*
  -----------------------------------------------------------------------------
  private function cn42_GetFileName
  -----------------------------------------------------------------------------
 */
static bool cn42_GetFileName
      ( const tsp00_DbNamec & szDbName,
        const tsp00_Pathc   & szType,
        int                 & nIndex,
        DBMSrvMsg_Error     & oError,
        tsp00_Pathc         & szFileName )
{
    tsp00_Pathc     szFileDir;
    return cn42_GetFileName ( szDbName, szType, nIndex, oError, szFileName, szFileDir );
} // end cn42_GetFileName
/*
  -----------------------------------------------------------------------------
 */
static bool cn42_GetFileName
      ( const tsp00_DbNamec & szDbName,
        const tsp00_Pathc   & szType,
        int                 & nIndex,
        DBMSrvMsg_Error     & oError,
        tsp00_Pathc         & szFileName,
        tsp00_Pathc         & szFileDir )
{
    DBMSrvMsg_Error oFileError;
    DBMSrvMsg_Error oBackupError;
    tsp00_Pathc     szBackupName;

    if (cn42_GetFileName ( szDbName, szType, nIndex, oError, szFileName, szFileDir, oFileError, szBackupName, oBackupError)) {
        if (!oFileError.IsEmpty()) {
            oError = oFileError;
        } // end if
    } // end if

    return oError.IsEmpty();
} // end cn42_GetFileName
/*
  -----------------------------------------------------------------------------
 */
static bool cn42_GetFileName
      ( const tsp00_DbNamec & szDbName,
        const tsp00_Pathc   & szType,
        int                 & nIndex,
        DBMSrvMsg_Error     & oError,
        tsp00_Pathc         & szFileName,
        tsp00_Pathc         & szFileDir,
        DBMSrvMsg_Error     & oFileError,
        tsp00_Pathc         & szBackupName,
        DBMSrvMsg_Error     & oBackupError )
{
    char              * pExtension   = NULL;
    tcn002_XpValueString szValue;
    char                szTemp  [PARAMETER_MAXLEN_CN90] = "";
    const char        * pToken  = 0;
    char              * pToken2 = 0;
    int                 nLength = 0;
    bool                bOValid  = true;
    bool                bBValid  = true;

    SAPDB_strcpy(szFileName, "");
    SAPDB_strcpy(szFileDir,  "");
    SAPDB_strcpy(szBackupName, "");

    pToken = strstr(szType, FGET_TOKENDEL);
    if (pToken == NULL) {
        nLength = (int)strlen(szType);
    } else {
        nLength = (int)(pToken - &szType[0]);
        pToken++;
        if ( (strstr(pToken, FGET_UPPER)     != NULL) ||
             (strstr(pToken, FGET_DRIVE)     != NULL)    ) {
            oError = DBMSrvMsg_Error(SDBMSG_DBMSRV_PARAM);
        } // end if
    } // end if

    if (oError.IsEmpty()) {

        // search for Type in file array
        nIndex = 0;
        while (sFGetFiles[nIndex].szType != NULL) {

            // match?
            if ((strncmp(sFGetFiles[nIndex].szType, szType, nLength) == 0)) {

               // get original name
               if ( cn42_GetLocation(sFGetFiles[nIndex].nLocation, szDbName, szFileName, oFileError)) {

                    SAPDB_strcpy(szFileDir, szFileName);

                    // compute source filename
                    if (sFGetFiles[nIndex].szXParam != NULL) {
                        // look for xparam
                        if (cn20XParamGetValue(szDbName, *(tsp00_XpKeyTypec *) sFGetFiles[nIndex].szXParam, szValue, oFileError)) {
                            strcat(szFileName, szValue);
                        } // end if
                    } else if (sFGetFiles[nIndex].szName == FGET_NAME_DB) {
                        // use dbname
                        strcat(szFileName, szDbName);
                    } else if (sFGetFiles[nIndex].szName == FGET_NAME_INST) {
                        // use installation (service database name)
                        tsp00_DbNamec szService;
                        cn90ServiceDB(szService, cn00DBMServerData::vcontrol()->dbroot);
                        strcat(szFileName, szService);
                    } else if (sFGetFiles[nIndex].szName != NULL) {

                        const char * pNode = strstr(sFGetFiles[nIndex].szName, FGET_NODE);
                        if (pNode != NULL) {

                            int nNode    = pNode - &(sFGetFiles[nIndex].szName[0]);
                            if ((nNode > 0) && (sFGetFiles[nIndex].szName[nNode-1] == '\\')) {
                                // use szName
                                strcat(szFileName, sFGetFiles[nIndex].szName);
                            } else {
                                strcat(szFileName, sFGetFiles[nIndex].szName);
                                szFileName[strlen(szFileName) - (strlen(sFGetFiles[nIndex].szName) - nNode)] = CHAR_STRINGTERM_CN90;
                                char  szHostName [ sizeof ( tsp00_NodeId ) + 1 ];
                                int rc = sql43_get_my_host_name ( szHostName, sizeof(tsp00_NodeId) );
                                if ( rc == 0 ) {
                                    strcat(szFileName, szHostName);
                                } // end if
                                strcat(szFileName, pNode+1);
                            } // end if
                        }  else {
                            // check for DBNAME variable in file name
                            const char* dBNameToken(strstr(sFGetFiles[nIndex].szName, FGET_DBNAME));
                            if( dBNameToken != NULL ) {
                                strcat(szFileName, sFGetFiles[nIndex].szName);
                                size_t len(dBNameToken - sFGetFiles[nIndex].szName);
                                szFileName[strlen(szFileName)-strlen(sFGetFiles[nIndex].szName) + len] = CHAR_STRINGTERM_CN90;
                                strcat(szFileName, szDbName.asCharp());
                                strcat(szFileName, dBNameToken+1);
                            } else {
                                // use szName
                                strcat(szFileName, sFGetFiles[nIndex].szName);
                            } // end if
                        } // end if
                    } // end if

                    if (oFileError.IsEmpty()) {
                        // compute source extension
                        if (sFGetFiles[nIndex].szExt != NULL) {
                            // use szExt-Field
                            strcat(szFileName, sFGetFiles[nIndex].szExt);
                        } // end if

                        pToken2 = strstr(szFileName, FGET_WILDCARD);
                        if (pToken != NULL && pToken2 != NULL) {
                            *pToken2 = 0;
                            pToken2++;
                            SAPDB_strcpy(szTemp, szFileName);
                            strcat(szTemp, pToken);
                            strcat(szTemp, pToken2);
                            SAPDB_strcpy(szFileName, szTemp);
                        } else if ((pToken != NULL) && (sFGetFiles[nIndex].nOrgType == FGET_TYP_DIR)) {
                            SAPDB_strcpy(szTemp, szFileName);
                            strcat(szTemp, pToken);
                            SAPDB_strcpy(szFileName, szTemp);
                        } // end if

                        cn90ChangePathDelimiter(szFileName);
                        if ((strlen(szFileName) > 0) &&  (szFileName[strlen(szFileName) - 1] == PATH_DELIMITER_CN90)) {
                        szFileName[strlen(szFileName) - 1] = 0;
                        } // end if
                    } // end if
                } // end if

                // backupname required and possible
                if ((sFGetFiles[nIndex].szBaName != NULL) &&
                     cn42_GetLocation(sFGetFiles[nIndex].nBaLocation, szDbName, szBackupName, oBackupError)) {

                    // compute backup filename
                    if (sFGetFiles[nIndex].szBaName == FGET_NAME_DB) {
                        // use dbname
                        strcat(szBackupName, szDbName);
                    } else if (sFGetFiles[nIndex].szName == FGET_NAME_INST) {
                        // use installation (service database name)
                        tsp00_DbNamec szService;
                        cn90ServiceDB(szService, cn00DBMServerData::vcontrol()->dbroot);
                        strcat(szFileName, szService);
                    } else if (sFGetFiles[nIndex].szBaName != NULL) {
                        // use szName
                        strcat(szBackupName, sFGetFiles[nIndex].szBaName);
                    } // end if

                    // compute backup extension
                    if (sFGetFiles[nIndex].szBaExt == FGET_ORG_EXT) {
                        if (oFileError.IsEmpty()) {
                            // use extension form original file
                            pExtension = strrchr(szFileName, '.');
                            if (pExtension != NULL) {
                                strcat(szBackupName, pExtension);
                            } // end if
                        } else if (sFGetFiles[nIndex].szExt != NULL) {
                            strcat(szBackupName, sFGetFiles[nIndex].szExt);
                        } // end if
                    } else if (sFGetFiles[nIndex].szBaExt != NULL) {
                        // use szExt
                        strcat(szBackupName, sFGetFiles[nIndex].szBaExt);
                    } // end if

                    cn90ChangePathDelimiter(szBackupName);
                    if ((strlen(szBackupName) > 0)  &&   (szBackupName[strlen(szBackupName) - 1] == PATH_DELIMITER_CN90)) {
                        szBackupName[strlen(szBackupName) - 1] = 0;
                    } // end if
                } // end if

                break;
            } // end if

            nIndex++;
        } // end while

        if (sFGetFiles[nIndex].szType == NULL) {
            oError = DBMSrvMsg_Error(SDBMSG_DBMSRV_PARAM);
        } // end if

    } // end if

    if (!oFileError.IsEmpty() && !oBackupError.IsEmpty()) 
      oError = oFileError;

    return oError.IsEmpty();
} // end cn42_GetFileName

/*
  -----------------------------------------------------------------------------
  function:     cn42RemoveFiles
  -----------------------------------------------------------------------------
 */
void cn42RemoveFiles
      ( const tsp00_DbNamec & szDbName)
{
  int                 nIndex       = 0;
  tsp00_Pathc         szType;
  tsp00_Pathc         szFileName;
  tsp00_Pathc         szFileDir;
  tsp00_Pathc         szBackupName;
  DBMSrvMsg_Error     oError;
  DBMSrvMsg_Error     oFileError;
  DBMSrvMsg_Error     oBackupError;

  char                replyData[1024];
  int                 replyLen;
  tsp05_RteFileError  rteFileErr;

  while (sFGetFiles[nIndex].szType != NULL) {
    if (sFGetFiles[nIndex].bRemove) {
      SAPDB_strcpy(szType, sFGetFiles[nIndex].szType);
      strcat(szType, FGET_TOKENDEL);
      strcat(szType, FGET_WILDCARD);

      oError.ClearMessageList();
      oFileError.ClearMessageList();
      oBackupError.ClearMessageList();

      if (cn42_GetFileName(szDbName, szType, nIndex, oError, szFileName, szFileDir, oFileError, szBackupName, oBackupError)) {
          if (oFileError.IsEmpty() && (strlen(szFileName) > 0)) {
              cn42_FileOperation( replyData, &replyLen, szFileName, "", 0, nIndex, true);
          } // end if
          if (oBackupError.IsEmpty() && (strlen(szBackupName) > 0)) {
              sqlferasec(szBackupName, &rteFileErr);
          } // end if
      } // end if

    } // end if
    nIndex++;
  } // end while

} // end cn42RemoveFiles

/*
  -----------------------------------------------------------------------------
  public function:     cn42RemoveOneFile
  -----------------------------------------------------------------------------
 */
void cn42RemoveOneFile
      ( const tsp00_DbNamec & szDbName,
        const tsp00_Pathc   & szType )
{
  tsp00_Pathc         szFile;
  tsp00_Pathc         szBackupFile;
  tsp00_Pathc         szFileDir;
  int                 nIndex = 0;
  DBMSrvMsg_Error     oError;
  DBMSrvMsg_Error     oFileError;
  DBMSrvMsg_Error     oBackupError;


  // prepare file name
  if (cn42_GetFileName(szDbName, szType, nIndex, oError, szFile, szFileDir, oFileError, szBackupFile, oBackupError)) {
    remove(szFile);
    remove(szBackupFile);
  } // end if

} // end cn42RemoveOneFile

/*
  -----------------------------------------------------------------------------
  implementation private functions
  -----------------------------------------------------------------------------
 */

/*
  -----------------------------------------------------------------------------
  function:     cn42_FileReadNext
  -----------------------------------------------------------------------------
 */
static tcn00_Error cn42_FileReadNext
      ( char             * replyData,
        int              * replyLen,
        const int          replyLenMax,
        const tsp00_Int4     hFile,
        const tsp00_Longint  nSize,
        const bool         bNoClose,
        const bool         bAscii,
        const bool         bRemoveEmptyLines,
        const int          nIndex,
        const char *       szDeleteAfterClose)
{
  tcn00_Error            nFuncReturn = OK_CN00;
  char                 * pCurrPos;
  char                 * pFlagPos;
  char                 * pLenPos;
  tsp00_Longint            nLength = 0;
  tsp00_Longint            nOutLen = 0;
  tsp05_RteFileError     rteFileErr;
  char                   szTmp[PARAMETER_MAXLEN_CN90];

  /* print OK and handle of file */
  sprintf (replyData, "%s%s%5d%s", ANSWER_OK_CN00,      LINE_SEPSTRING_CN00,
                                   hFile,               LINE_SEPSTRING_CN00);

  pFlagPos = replyData + strlen(replyData);
  sprintf ( pFlagPos , "%s%s" , CONT_FLAG_CONTINUE, LINE_SEPSTRING_CN00 );

  pLenPos = replyData + strlen(replyData);
  sprintf ( pLenPos, "%20ld%20ld%s" , (long) 0, (long) 0, LINE_SEPSTRING_CN00 );

  pCurrPos  = replyData + strlen(replyData);
  *replyLen = (int)strlen(replyData);

  // for test only
  // replyLenMax = 150;

  // fill buffer
  do {
    sqlfreadc(hFile, pCurrPos, replyLenMax - *replyLen - ((bAscii) ? 2 : 0), &nOutLen, &rteFileErr);

    if (!(bAscii && bRemoveEmptyLines && cn42_IsLineEmpty(pCurrPos))) {
      *replyLen = *replyLen + (int)nOutLen;
      nLength   = nLength   + nOutLen;

      if  ( rteFileErr.sp5fe_warning == sp5vfw_no_warning &&
            rteFileErr.sp5fe_result  == vf_ok             &&
            bAscii) {
        *replyLen = *replyLen + 2;
        nLength   = nLength   + 2;
        replyData[*replyLen-2] = '\r';
        replyData[*replyLen-1] = '\n';
      } // end if

      pCurrPos = replyData + *replyLen;
    } // end if

  } while ( *replyLen + ((bAscii) ? 2 : 0) <  replyLenMax       &&
            rteFileErr.sp5fe_warning       == sp5vfw_no_warning &&
            rteFileErr.sp5fe_result        == vf_ok                );

  // end handling
  replyData[*replyLen] = '\0';
  sprintf ( szTmp, "%20ld%20ld", (long) nSize, (long) nLength);
  strncpy(pLenPos, szTmp, strlen(szTmp));

  if ( rteFileErr.sp5fe_result == vf_eof ) {
    sprintf ( szTmp, "%s", CONT_FLAG_END );
    strncpy(pFlagPos, szTmp, strlen(szTmp));

    if (!bNoClose) {
      sqlfclosec ( hFile, sp5vf_close_normal, &rteFileErr );
      sFGetFiles[nIndex].hFile = -1;
      if (szDeleteAfterClose != NULL) 
        remove(szDeleteAfterClose);
    } // end if
  } else {
    if (nIndex > -1) {
      // prepare next command for file get
      sprintf(cn00DBMServerData::vcontrol()->szNextCommand, "file_getnext %s %d", sFGetFiles[nIndex].szType ,hFile);
      cn00DBMServerData::vcontrol()->nNextCommandSkip = 3;
    } // end if
  } // end if

  if (rteFileErr.sp5fe_result == vf_notok) {
    nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(DBMSrvMsg_RTEError(rteFileErr));
    sqlfclosec ( hFile, sp5vf_close_normal, &rteFileErr );
    sFGetFiles[nIndex].hFile = -1;
    if (szDeleteAfterClose != NULL) 
      remove(szDeleteAfterClose);
  } // end if

  return nFuncReturn;

} // end cn42_FileReadNext

/*
  -----------------------------------------------------------------------------
  function:     cn42_FileReadFirst
  -----------------------------------------------------------------------------
 */
static tcn00_Error cn42_FileReadFirst
      ( char             * replyData,
        int              * replyLen,
        const int          replyLenMax,
        const tsp00_Int4     hFile,
        const tsp00_Longint  nSize,
        const int          nIndex,
        const char *       pDate,
        int                nLine,
        const char *       szDeleteAfterClose)
{
  tcn00_Error            nFuncReturn = OK_CN00;
  char                 * pCurrPos;
  char                 * pFlagPos;
  char                 * pLenPos;
  tsp00_Longint            nLength = 0;
  tsp00_Longint            nOutLen = 0;
  tsp05_RteFileError     rteFileErr;
  char                   szTmp[PARAMETER_MAXLEN_CN90];
  int                    nHeader = sFGetFiles[nIndex].nHeader;
  tsp00_Longint            nCurrPos   = 0;
  tsp00_Longint          * aLinePos   = NULL;
  int                    nLineIndex = 0;
  tsp00_Timestampc       sTimeStamp;
  tsp00_Timestampc       sReqTimeStamp;

  /* print OK and handle of file */
  sprintf (replyData, "%s%s%5d%s", ANSWER_OK_CN00,      LINE_SEPSTRING_CN00,
                                   hFile,               LINE_SEPSTRING_CN00);

  pFlagPos = replyData + strlen(replyData);
  sprintf ( pFlagPos , "%s%s" , CONT_FLAG_CONTINUE, LINE_SEPSTRING_CN00 );

  pLenPos = replyData + strlen(replyData);
  sprintf ( pLenPos, "%20ld%20ld%s" , (long) 0, (long) 0, LINE_SEPSTRING_CN00 );

  pCurrPos  = replyData + strlen(replyData);
  *replyLen = (int)strlen(replyData);

  // step 1 read the header
  if (nHeader > 0) {
    do {
      sqlfreadc(hFile, pCurrPos, replyLenMax - *replyLen - (FGET_IS_TEXT(sFGetFiles[nIndex].nType) ? 2 : 0), &nOutLen, &rteFileErr);
      *replyLen = *replyLen + (int)nOutLen;
      nLength   = nLength   + nOutLen;

      if  ( (rteFileErr.sp5fe_warning == sp5vfw_no_warning) &&
            (rteFileErr.sp5fe_result  == vf_ok            ) &&
            (FGET_IS_TEXT(sFGetFiles[nIndex].nType)       )    ) {
        *replyLen = *replyLen + 2;
        nLength   = nLength   + 2;
        replyData[*replyLen-2] = '\r';
        replyData[*replyLen-1] = '\n';
      } // end if

      pCurrPos = replyData + *replyLen;

      --nHeader;
    } while ( (*replyLen + (FGET_IS_TEXT(sFGetFiles[nIndex].nType) ? 2 : 0) <  replyLenMax)  &&
              (rteFileErr.sp5fe_warning       == sp5vfw_no_warning               )  &&
              (rteFileErr.sp5fe_result        == vf_ok                           )  &&
              (nHeader                         > 0                                )    );

    if (rteFileErr.sp5fe_result == vf_notok) {
      nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(DBMSrvMsg_RTEError(rteFileErr));
      sqlfclosec ( hFile, sp5vf_close_normal, &rteFileErr );
      sFGetFiles[nIndex].hFile = -1;
      if (szDeleteAfterClose != NULL) 
        remove(szDeleteAfterClose);
    } // end if
  } // end if

  // step 2 navigate to the requested line

  // make a array for position save
  if ((nFuncReturn == OK_CN00) && (nLine != 0)) {
    aLinePos = new tsp00_Longint[nLine];
    if (aLinePos == NULL) {
      nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_MEM_CN00);
    } // end if
  } // end if
  if ((nFuncReturn == OK_CN00) && (pDate != NULL)) {
    //                     YYYYMMDDhhmmss
    SAPDB_strcpy(sReqTimeStamp, "00000000000000");
    strncpy(sReqTimeStamp, pDate, strlen(pDate));
  } // end if

  // save first position
  if ((nFuncReturn == OK_CN00) && (nLine != 0)) {
    // read first position
    sqlftellc (hFile, &nCurrPos, &rteFileErr);
    if (rteFileErr.sp5fe_result == vf_notok) {
      nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(DBMSrvMsg_RTEError(rteFileErr));
      sqlfclosec ( hFile, sp5vf_close_normal, &rteFileErr );
      sFGetFiles[nIndex].hFile = -1;
      if (szDeleteAfterClose != NULL) 
        remove(szDeleteAfterClose);
    } else {
      for (nLineIndex = 0; nLineIndex < nLine; ++nLineIndex) {
        aLinePos[nLineIndex] = nCurrPos;
      } // end for
    } // end if
  } // end if

  if (nFuncReturn == OK_CN00) {
    nLineIndex = 0;
    do {
      //  read position
      sqlftellc (hFile, &nCurrPos, &rteFileErr);

      if  ( (rteFileErr.sp5fe_warning == sp5vfw_no_warning) &&
            (rteFileErr.sp5fe_result  == vf_ok            )    ) {
        // read line
        sqlfreadc(hFile, szTmp, PARAMETER_MAXLEN_CN90, &nOutLen, &rteFileErr);

        if  ( (rteFileErr.sp5fe_warning == sp5vfw_no_warning) &&
            (rteFileErr.sp5fe_result  == vf_ok            )    ) {
          // check line
          if (pDate != NULL) {
            // check the date
            if ( strlen(szTmp) >= (sFGetFiles[nIndex].nTimestampPos + strlen(sFGetFiles[nIndex].szTimestampFmt)) ) {
              cn42_ReadTimeStamp ( &szTmp[sFGetFiles[nIndex].nTimestampPos],
                                   sFGetFiles[nIndex].szTimestampFmt,
                                   sTimeStamp);
              if (strcmp(sTimeStamp, sReqTimeStamp) >= 0) {
              // if match then break do...while
                break;
              } // end if
            } // end if

          } else if (nLine != 0) {
            // save the line position (ignore empty lines if requested)
            if (!(sFGetFiles[nIndex].bRemoveEmptyLines && cn42_IsLineEmpty(szTmp))) {
              aLinePos[nLineIndex] = nCurrPos;
              nLineIndex = (nLineIndex + 1) % nLine;
            } // end if
          } // end if
        } // end if
      } // end if

    } while ( (rteFileErr.sp5fe_warning       == sp5vfw_no_warning               ) &&
              (rteFileErr.sp5fe_result        == vf_ok                           )    );

    if (rteFileErr.sp5fe_result == vf_notok) {
      nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(DBMSrvMsg_RTEError(rteFileErr));
      sqlfclosec ( hFile, sp5vf_close_normal, &rteFileErr );
      sFGetFiles[nIndex].hFile = -1;
      if (szDeleteAfterClose != NULL) 
        remove(szDeleteAfterClose);
    } else {
      // navigate to correct position
      if (nLine > 0) {
        nCurrPos = aLinePos[nLineIndex];
        delete [] aLinePos;
        aLinePos = NULL;
      } // end if
      sqlfseekc (hFile, nCurrPos, sp5vf_seek_begin, &rteFileErr);
      if (rteFileErr.sp5fe_result == vf_notok) {
        nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(DBMSrvMsg_RTEError(rteFileErr));
        sqlfclosec ( hFile, sp5vf_close_normal, &rteFileErr );
        sFGetFiles[nIndex].hFile = -1;
        if (szDeleteAfterClose != NULL) 
          remove(szDeleteAfterClose);
      } // end if
    } // end if
  } // end if

  // step 3 read the file content
  if ( (*replyLen + (FGET_IS_TEXT(sFGetFiles[nIndex].nType) ? 2 : 0) <  replyLenMax)  && (nFuncReturn == OK_CN00)) {

    do {
      sqlfreadc(hFile, pCurrPos, replyLenMax - *replyLen - (FGET_IS_TEXT(sFGetFiles[nIndex].nType) ? 2 : 0), &nOutLen, &rteFileErr);

      if (!(sFGetFiles[nIndex].bRemoveEmptyLines && cn42_IsLineEmpty(pCurrPos))) {

        *replyLen = *replyLen + (int)nOutLen;
        nLength   = nLength   + nOutLen;

        if  ( rteFileErr.sp5fe_warning == sp5vfw_no_warning &&
              rteFileErr.sp5fe_result  == vf_ok             &&
              FGET_IS_TEXT(sFGetFiles[nIndex].nType)           ) {
          *replyLen = *replyLen + 2;
          nLength   = nLength   + 2;
          replyData[*replyLen-2] = '\r';
          replyData[*replyLen-1] = '\n';
        } // end if

        pCurrPos = replyData + *replyLen;

      } // end if

    } while ( (*replyLen + (FGET_IS_TEXT(sFGetFiles[nIndex].nType) ? 2 : 0) <  replyLenMax) &&
              (rteFileErr.sp5fe_warning       == sp5vfw_no_warning               ) &&
              (rteFileErr.sp5fe_result        == vf_ok                           )    );

    if (rteFileErr.sp5fe_result == vf_notok) {
      nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(DBMSrvMsg_RTEError(rteFileErr));
      sqlfclosec ( hFile, sp5vf_close_normal, &rteFileErr );
      sFGetFiles[nIndex].hFile = -1;
      if (szDeleteAfterClose != NULL) 
        remove(szDeleteAfterClose);
    } // end if

  } // end if

  if (nFuncReturn == OK_CN00) {
    // end handling
    replyData[*replyLen] = '\0';
    sprintf ( szTmp, "%20ld%20ld", (long) nSize, (long) nLength);
    strncpy(pLenPos, szTmp, strlen(szTmp));

    if ( rteFileErr.sp5fe_result == vf_eof ) {
      sprintf ( szTmp, "%s", CONT_FLAG_END );
      strncpy(pFlagPos, szTmp, strlen(szTmp));

      sqlfclosec ( hFile, sp5vf_close_normal, &rteFileErr );
      sFGetFiles[nIndex].hFile = -1;
      if (szDeleteAfterClose != NULL) 
        remove(szDeleteAfterClose);
    } else {
      // prepare next command
      sprintf(cn00DBMServerData::vcontrol()->szNextCommand, "file_getnext %s %d", sFGetFiles[nIndex].szType ,hFile);
      cn00DBMServerData::vcontrol()->nNextCommandSkip = 3;
    } // end if
  } // end if

  return nFuncReturn;

} // end cn42_FileReadFirst

/*
  -----------------------------------------------------------------------------
  function:     cn42_FileMatch
  -----------------------------------------------------------------------------
 */
static bool cn42_FileMatch
      ( tsp00_Pathc        szFileName,
        tsp00_Pathc        szRealName,
        tsp00_Pathc      & szToken)
{

  char   * pPostfix   = strstr(szFileName, FGET_WILDCARD);
  char   * pPrefix    = szFileName;
  char   * pReal      = szRealName;
  bool     bReturn    = false;

  if (pPostfix != NULL) {
    pPostfix++;

    if (strncmp(pPrefix,  pReal,  pPostfix - pPrefix -1 )                                 == 0  &&
        strncmp(pPostfix, pReal + strlen(pReal) - strlen(pPostfix) , strlen(pPostfix)) == 0     ) {
      bReturn = true;
      memset(szToken, CHAR_STRINGTERM_CN90, szToken.size());
      strncpy(szToken, pReal + (pPostfix - pPrefix - 1), strlen(pReal) - strlen(pPrefix) + 1);
    } // end if
  } // end if

  return bReturn;
} // end cn42_FileMatch

/*
  -----------------------------------------------------------------------------
  function:     cn42_DiagFileMatch
  -----------------------------------------------------------------------------
 */
static bool cn42_DiagFileMatch
      ( const char       * szDBName,
        const char       * szFile )
{
  bool        bReturn    = false;
  tsp00_Pathc szPattern;

  SAPDB_strcpy(szPattern, szDBName);
  strcat(szPattern, DIAG_HIST_PATTERN);

  if (strlen(szPattern) == strlen(szFile) ) {
    bReturn = true;
    for (int nIndex = 0; szFile[nIndex] != 0; ++nIndex) {
      switch (szPattern[nIndex]) {
      case '?':
        bReturn = bReturn && (isdigit(szFile[nIndex]));
        break;
      default:
        bReturn = bReturn && (szPattern[nIndex] == szFile[nIndex]);
        break;
      } // end switch
    } // end if
  } // end if

  return bReturn;
} // end cn42_DiagFileMatch

/*
  -----------------------------------------------------------------------------
  private function cn42_ReadTimeStamp
  -----------------------------------------------------------------------------
*/
static void cn42_ReadTimeStamp
      ( const char *       szSource,
        const char *       szFormat,
        tsp00_Timestampc & sTimeStamp )
{
  int nIndex  = 0;

  int nYear   = 0;
  int nMonth  = 0;
  int nDay    = 0;

  int nHour   = 0;
  int nMinute = 0;
  int nSecond = 0;

  bool bValid = true;

  sTimeStamp.rawAssign("");

  if ( (szSource != NULL) && (szFormat != NULL) ) {
    while ((szSource[nIndex] != CHAR_STRINGTERM_CN90) &&
           (szFormat[nIndex] != CHAR_STRINGTERM_CN90) && bValid ) {
      switch(szFormat[nIndex]) {
      case CHAR_YEAR:
        if (isdigit(szSource[nIndex]) == 0) {
          bValid = false;
        } else {
          nYear   = (nYear   * 10 ) + (szSource[nIndex] - 0x30);
        } // end if
        break;
      case CHAR_MONTH:
        if (isdigit(szSource[nIndex]) == 0) {
          bValid = false;
        } else {
          nMonth  = (nMonth  * 10 ) + (szSource[nIndex] - 0x30);
        } // end if
        break;
      case CHAR_DAY:
        if (isdigit(szSource[nIndex]) == 0) {
          bValid = false;
        } else {
          nDay    = (nDay    * 10 ) + (szSource[nIndex] - 0x30);
        } // end if
        break;
      case CHAR_HOUR:
        if (isdigit(szSource[nIndex]) == 0) {
          bValid = false;
        } else {
          nHour   = (nHour   * 10 ) + (szSource[nIndex] - 0x30);
        } // end if
        break;
      case CHAR_MINUTE:
        if (isdigit(szSource[nIndex]) == 0) {
          bValid = false;
        } else {
          nMinute =  (nMinute * 10 ) + (szSource[nIndex] - 0x30);
        } // end if
        break;
      case CHAR_SECOND:
        if (isdigit(szSource[nIndex]) == 0) {
          bValid = false;
        } else {
          nSecond = (nSecond * 10 ) + (szSource[nIndex] - 0x30);
        } // end if
        break;
      default:
        if (szFormat[nIndex] != szSource[nIndex]) {
          bValid = false;
        } // end if
        break;
      } // end switch
      ++nIndex;
    } // end while
  } // end if

  nYear = (nYear < MILLENIUM_MIN) ? nYear + MILLENIUM_PRE : nYear;

  if (bValid) {
    sprintf(sTimeStamp, STAMP_FMT, nYear, nMonth, nDay, nHour, nMinute, nSecond);
  } else {
    sprintf(sTimeStamp, "00000000000000");
  } // end if

} // end cn42_ReadTimeStamp

/*
  -----------------------------------------------------------------------------
  private function cn42_IsLineEmpty
  -----------------------------------------------------------------------------
*/
static bool cn42_IsLineEmpty(const char * pLine)
{
  bool bEmpty = true;

  while (*pLine != 0) {
    if ((*pLine != ' ') && (*pLine != '\t')) {
      bEmpty = false;
      break;
    } // end if
    ++pLine;
  }

  return bEmpty;
} // end cn42_IsLineEmpty

/*
  -----------------------------------------------------------------------------
  private function cn42_FileOperation
  -----------------------------------------------------------------------------
*/
static tcn00_Error cn42_FileOperation
      ( char       *       replyData,
        int        *       replyLen,
        const char *       szFile,
        const char *       szDate,
        int                nLine,
        int                nIndex,
        bool               bDelete)
{
  tcn00_Error        nFuncReturn = OK_CN00;

  if ( (strchr(szFile, FGET_WILDCARDSTAR) != NULL) ||
       (strchr(szFile, FGET_WILDCARDQUE ) != NULL)    ) {

    tsp00_C256           szDir;
    tsp00_Pathc          szPattern;
    tsp00_C256           szFoundFile;
    tsp00_Pathc          szFullFile;
    int                  nIndex;
    tsp05_RteFileError   rteFileErr;
    void               * hDirectory  = NULL;
    pasbool              cPatOK;

    // extract path and filename
    for (nIndex = (int) strlen(szFile); nIndex >= 0; --nIndex) {
      if ((szFile[nIndex] == CHAR_SLASH    ) ||
          (szFile[nIndex] == CHAR_BACKSLASH)    ) {
        break;
      } // end if
    } // end for
    if (nIndex >= 0) {
      szDir.c2p(szFile);
      szDir[nIndex] = CHAR_STRINGTERM_CN90;
      szPattern.rawAssign(&(szFile[nIndex + 1]));
    } else {
      szDir.c2p(".");
      szPattern.rawAssign(szFile);
    } // end if

    // prepare reg expression
    s49build_pattern (szPattern.asCharp(),
                      1,
                      szPattern.length(),
                      '\0',
                      false,
                      true,
                      sqlm_internal,
                      cPatOK);

    // open and iterate trough dir
    sqlfopendirc ( &szDir, &hDirectory,  &szFoundFile, &rteFileErr);

    if ( rteFileErr.sp5fe_result == vf_ok) {

      while  ( rteFileErr.sp5fe_result == vf_ok) {

        if (s49patmatch (szFoundFile.asCharp(), 0, (tsp00_Int4) strlen(szFoundFile), szPattern.asCharp(), 0, szPattern.length(), '\0')) {
          SAPDB_strcpy(szFullFile, szDir);
#ifdef _WIN32
          strcat(szFullFile, FGET_BACKSLASH);
#else
          strcat(szFullFile, FGET_SLASH);
#endif
          strcat(szFullFile, szFoundFile);

          if (bDelete) {
            nFuncReturn = cn42_FileDelete( replyData, replyLen, szFullFile, szDate);
          } else {
            nFuncReturn = cn42_FileShrink( replyData, replyLen, szFullFile, szDate, nLine, nIndex);
          } // end if

        } // end if

        sqlfreaddirc(hDirectory, &szFoundFile, &rteFileErr);
      } // end while

      sqlfclosedirc (hDirectory, &rteFileErr);

    } else {
      DBMSrvMsg_RTEError rteError(rteFileErr);
      rteError.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_FILE));
      nFuncReturn = DBMSrvDep_DirtyReply(replyData, replyLen).startWithMessageList(rteError);
    } // end if

  } else {

    if (bDelete) {
      nFuncReturn = cn42_FileDelete( replyData, replyLen, szFile, szDate);
    } else {
      nFuncReturn = cn42_FileShrink( replyData, replyLen, szFile, szDate, nLine, nIndex);
    } // end if

  } // end if

  return nFuncReturn;
} // end cn42_FileOperation

/*
  -----------------------------------------------------------------------------
  private function cn42_FileDelete
  -----------------------------------------------------------------------------
*/
static tcn00_Error cn42_FileDelete( char       * replyData,
                                    int        * replyLen,
                                    const char * szFileName,
                                    const char * szDate)
{
  tcn00_Error        nFuncReturn = OK_CN00;
  bool               bDelete     = true;
  tsp05_RteFileError rteFileErr;

  if (strlen(szDate) > 0) {
    tsp00_Timestampc       sReqTimeStamp;
    tsp00_Timestampc       sTimeStamp;
    tsp05_RteFileInfo      rteFInfo;

    SAPDB_strcpy(sReqTimeStamp, "00000000000000");
    strncpy(sReqTimeStamp, szDate, strlen(szDate));

    sqlfinfoc (szFileName, &rteFInfo, &rteFileErr );

    if (rteFInfo.sp5fi_exists) {
      sprintf(sTimeStamp, "%.8s%.8s", rteFInfo.sp5fi_date_modified.asCharp(),
                                      rteFInfo.sp5fi_date_modified.asCharp());

      if (strcmp(sTimeStamp, sReqTimeStamp) > 0) {
        bDelete = false;
      } // end if
    } // end if
  } // end if

  if (bDelete) {
    sqlferasec (szFileName, &rteFileErr);
    if (rteFileErr.sp5fe_result != vf_ok) {
      DBMSrvMsg_RTEError rteError(rteFileErr);
      rteError.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_FILE));
      nFuncReturn = DBMSrvDep_DirtyReply(replyData, replyLen).startWithMessageList(rteError);
    } // end if
  } // end if

  return nFuncReturn;
} // end cn42_FileDelete

/*
  -----------------------------------------------------------------------------
  private function cn42_FileShrink
  -----------------------------------------------------------------------------
*/
static tcn00_Error cn42_FileShrink
      ( char       *       replyData,
        int        *       replyLen,
        const char *       szFileName,
        const char *       szDate,
        int                nLine,
        int                nIndex)
{
  tcn00_Error         nFuncReturn = OK_CN00;
  tsp00_Pathc         szTemp;
  tsp00_Pathc         szOrg;
  tsp01_RteError      rteError;
  tsp05_RteFileError  fileError;
  tsp00_Int4            hSource;
  tsp00_Int4            hTarget;
  int                 nHeader = sFGetFiles[nIndex].nHeader;
  char                szBuffer[PARAMETER_MAXLEN_CN90];
  tsp00_Longint         nOutLen    = 0;
  tsp00_Longint         nCurrPos   = 0;
  tsp00_Longint       * aLinePos   = NULL;
  int                 nLineIndex = 0;
  tsp00_Timestampc    sTimeStamp;
  tsp00_Timestampc    sReqTimeStamp;

  szTemp.rawAssign(szFileName);
  szOrg.rawAssign(szFileName);

  strcat(szTemp.asCharp(), ".1");

  // copy oldfile to temp
  if (sqlfilecopyc(szOrg.asCharp(), szTemp.asCharp(), &rteError)) {

    // open temp as source
    sqlfopenc (szTemp.asCharp(), sp5vf_text, sp5vf_read, sp5bk_buffered, &hSource, &fileError);

    if (fileError.sp5fe_result == vf_ok) {
      // open old file as target
      sqlfopenc (szOrg.asCharp(), sp5vf_text, sp5vf_write, sp5bk_buffered, &hTarget, &fileError);

      if (fileError.sp5fe_result == vf_ok) {
        // all files are open

        // step 1 read the header
        if (nHeader > 0) {
          do {
            sqlfreadc(hSource, szBuffer, PARAMETER_MAXLEN_CN90, &nOutLen, &fileError);
            if  ( (fileError.sp5fe_warning == sp5vfw_no_warning) &&
                  (fileError.sp5fe_result  == vf_ok            )    ) {
              sqlfwritec (hTarget, szBuffer, -1, &fileError);
            } // end if
            --nHeader;
          } while ( (fileError.sp5fe_warning == sp5vfw_no_warning ) &&
                    (fileError.sp5fe_result  == vf_ok             ) &&
                    (nHeader                  >  0                 )    );

          if (fileError.sp5fe_result == vf_notok) {
            DBMSrvMsg_RTEError rteError(fileError);
            rteError.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_FILE));
            nFuncReturn = DBMSrvDep_DirtyReply(replyData, replyLen).startWithMessageList(rteError);
          } // end if
        } // end if

        // step 2 navigate to the requested line

        // make a array for position save
        if ((nFuncReturn == OK_CN00) && (nLine != 0)) {
          aLinePos = new tsp00_Longint[nLine];
          if (aLinePos == NULL) {
            nFuncReturn = cn90AnswerIError(replyData, replyLen, ERR_MEM_CN00);
          } // end if
        } // end if
        if ((nFuncReturn == OK_CN00) && (strlen(szDate) > 0)) {
          //                     YYYYMMDDhhmmss
          SAPDB_strcpy(sReqTimeStamp, "00000000000000");
          strncpy(sReqTimeStamp, szDate, strlen(szDate));
        } // end if

        // save first position
        if ((nFuncReturn == OK_CN00) && (nLine != 0)) {
          // read first position
          sqlftellc (hSource, &nCurrPos, &fileError);
          if (fileError.sp5fe_result == vf_notok) {
            DBMSrvMsg_RTEError rteError(fileError);
            rteError.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_FILE));
            nFuncReturn = DBMSrvDep_DirtyReply(replyData, replyLen).startWithMessageList(rteError);
          } else {
            for (nLineIndex = 0; nLineIndex < nLine; ++nLineIndex) {
              aLinePos[nLineIndex] = nCurrPos;
            } // end for
          } // end if
        } // end if

        if (nFuncReturn == OK_CN00) {
          nLineIndex = 0;
          do {
            //  read position
            sqlftellc (hSource, &nCurrPos, &fileError);

            if  ( (fileError.sp5fe_warning == sp5vfw_no_warning) &&
                  (fileError.sp5fe_result  == vf_ok            )    ) {
              // read line
              sqlfreadc(hSource, szBuffer, PARAMETER_MAXLEN_CN90, &nOutLen, &fileError);

              if  ( (fileError.sp5fe_warning == sp5vfw_no_warning) &&
                    (fileError.sp5fe_result  == vf_ok            )    ) {
                // check line
                if (strlen(szDate) > 0) {
                  // check the date
                  if ( strlen(szBuffer) > (sFGetFiles[nIndex].nTimestampPos + strlen(sFGetFiles[nIndex].szTimestampFmt)) ) {
                    cn42_ReadTimeStamp ( &szBuffer[sFGetFiles[nIndex].nTimestampPos],
                                         sFGetFiles[nIndex].szTimestampFmt,
                                         sTimeStamp);
                    if (strcmp(sTimeStamp, sReqTimeStamp) >= 0) {
                      // if match then break do...while
                      break;
                    } // end if
                  } // end if

                } else if (nLine != 0) {
                  // save the line position
                  aLinePos[nLineIndex] = nCurrPos;
                  nLineIndex = (nLineIndex + 1) % nLine;
                } // end if
              } // end if
            } // end if

          } while ( (fileError.sp5fe_warning == sp5vfw_no_warning) &&
                    (fileError.sp5fe_result  == vf_ok            )    );

          if (fileError.sp5fe_result == vf_notok) {
            DBMSrvMsg_RTEError rteError(fileError);
            rteError.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_FILE));
            nFuncReturn = DBMSrvDep_DirtyReply(replyData, replyLen).startWithMessageList(rteError);
          } else {
            // navigate to correct position
            if (nLine > 0) {
              nCurrPos = aLinePos[nLineIndex];
              delete [] aLinePos;
              aLinePos = NULL;
            } // end if
            sqlfseekc (hSource, nCurrPos, sp5vf_seek_begin, &fileError);
            if (fileError.sp5fe_result == vf_notok) {
              DBMSrvMsg_RTEError rteError(fileError);
              rteError.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_FILE));
              nFuncReturn = DBMSrvDep_DirtyReply(replyData, replyLen).startWithMessageList(rteError);
            } // end if
          } // end if
        } // end if

        // step 3 read the file content
        if (nFuncReturn == OK_CN00) {
          do {
            sqlfreadc(hSource, szBuffer, PARAMETER_MAXLEN_CN90, &nOutLen, &fileError);
              if  ( (fileError.sp5fe_warning == sp5vfw_no_warning) &&
                    (fileError.sp5fe_result  == vf_ok            )    ) {
                sqlfwritec (hTarget, szBuffer, -1, &fileError);
              } // end if
          } while ( (fileError.sp5fe_warning == sp5vfw_no_warning ) &&
                    (fileError.sp5fe_result  == vf_ok             )    );

          if (fileError.sp5fe_result == vf_notok) {
            DBMSrvMsg_RTEError rteError(fileError);
            rteError.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_FILE));
            nFuncReturn = DBMSrvDep_DirtyReply(replyData, replyLen).startWithMessageList(rteError);
          } // end if
        } // end if

        if (nFuncReturn == OK_CN00) {
          sqlfclosec (hTarget, sp5vf_close_normal, &fileError);
          if (fileError.sp5fe_result == vf_notok) {
            DBMSrvMsg_RTEError rteError(fileError);
            rteError.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_FILE));
            nFuncReturn = DBMSrvDep_DirtyReply(replyData, replyLen).startWithMessageList(rteError);
          } // end if
        } else {
          sqlfclosec (hTarget, sp5vf_close_normal, &fileError);
        } // end if

      } else {
        DBMSrvMsg_RTEError rteError(fileError);
        rteError.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_FILE));
        nFuncReturn = DBMSrvDep_DirtyReply(replyData, replyLen).startWithMessageList(rteError);
      } // end if

      sqlfclosec (hSource, sp5vf_close_normal, &fileError);

    } else {
      DBMSrvMsg_RTEError rteError(fileError);
      rteError.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_FILE));
      nFuncReturn = DBMSrvDep_DirtyReply(replyData, replyLen).startWithMessageList(rteError);
    } // end if

    // restore old file from temp
    if (nFuncReturn != OK_CN00) {
      sqlfilecopyc(szTemp.asCharp(), szOrg.asCharp(), &rteError);
    } // end if
    // delete temp
    sqlferasec(szTemp, &fileError);

  } else {
    DBMSrvMsg_NewRTEError newRTEError(rteError);
    newRTEError.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_FILE));
    nFuncReturn = DBMSrvDep_DirtyReply(replyData, replyLen).startWithMessageList(newRTEError);
  } // end if

  return nFuncReturn;
} // end cn42_FileShrink

/*
  -----------------------------------------------------------------------------
  private function cn42_FileGetListDir
  -----------------------------------------------------------------------------
*/
static tcn00_Error cn42_FileGetListDir 
      ( char               * replyData,
        long                 nIndex,
        tsp00_Path         & szFileDir,
        tsp00_Pathc        & szFileName,
        tsp00_Path         & szBaseDir,
        char              *& pCurrent,
        int                  replyLenMax)
{
  tcn00_Error         nFuncReturn  = OK_CN00;
  void              * hDirectory  = NULL;
  tsp00_Path          szRealName;
  tsp00_Pathc         szFullReal;
  tsp05_RteFileInfo   rteFileInfo;
  tsp05_RteFileError  rteFileErr;
  tsp00_Pathc         szToken;
  char              * pFirst = pCurrent;
  int                 nLength;
  int                 replyLen;

  sqlfopendirc (&szFileDir, &hDirectory, &szRealName, &rteFileErr);

  if (rteFileErr.sp5fe_result == vf_ok) {
    while ((rteFileErr.sp5fe_result == vf_ok) && (nFuncReturn == OK_CN00)) {

      if ((strcmp(szRealName, ".") != 0) && (strcmp(szRealName, "..") != 0)) {

        SAPDB_strcpy(szFullReal, szFileDir);
        strcat(szFullReal, szRealName);
        if (cn42_FileMatch(szFileName, szFullReal, szToken)) {
          sqlfinfoc (szFullReal, &rteFileInfo, &rteFileErr );
        
          if (sFGetFiles[nIndex].nType == FGET_TYP_DIR) {
            size_t nBaseLength;
            nBaseLength = strlen(szBaseDir);
            nBaseLength += (sFGetFiles[nIndex].szName != NULL) ? strlen(sFGetFiles[nIndex].szName) : 0;
            nBaseLength += (sFGetFiles[nIndex].szExt  != NULL) ? strlen(sFGetFiles[nIndex].szExt) : 0;
            SAPDB_strcpy(szToken, &szFullReal[nBaseLength]);

          } // end if

          if (rteFileInfo.sp5fi_exists) {
            long nType = sFGetFiles[nIndex].nType;
            if (!FGET_IS_TEXT(nType) && cn42_IsKnlMsg(szFullReal)) {
              nType = FGET_TYP_ASCII;
            } // end if

            // check buffer size
            nLength = (int) (strlen(sFGetFiles[nIndex].szType) +
                             strlen(FGET_TOKENDEL) +
                             strlen(szToken) +
                             (6 * strlen(VALUE_SEPSTRING_CN00)) +
                             strlen(cn42_FileType(rteFileInfo, nType)) +
                             20 + // rteFileInfo.sp5fi_size
                             sizeof (rteFileInfo.sp5fi_date_modified) +
                             sizeof (rteFileInfo.sp5fi_time_modified) +
                             strlen(sFGetFiles[nIndex].szComment) +
                             strlen(szFullReal  + _tcslen(szFileDir)) +
                             strlen(LINE_SEPSTRING_CN00) );
            if ((replyLenMax - (pCurrent - pFirst)) < nLength) {
              nFuncReturn = cn90AnswerEvent(replyData, &replyLen, replyLenMax, DBMSrvMsg_Error(SDBMSG_DBMSRV_TOOMANYFILES));
              break;
            } else {

              _stprintf (pCurrent, "%s%s%s%s%s%s%ld%s%.*s%s%.*s%s%s%s%s%s",
                                   (char   *) sFGetFiles[nIndex].szType,
                                   (char   *) FGET_TOKENDEL,
                                   (char   *) szToken,
                                   VALUE_SEPSTRING_CN00,
                                   (char   *) cn42_FileType(rteFileInfo, nType),
                                   VALUE_SEPSTRING_CN00,
                                   (long) rteFileInfo.sp5fi_size,
                                   VALUE_SEPSTRING_CN00,
                                   sizeof (rteFileInfo.sp5fi_date_modified), (char   *) rteFileInfo.sp5fi_date_modified,
                                   VALUE_SEPSTRING_CN00,
                                   sizeof (rteFileInfo.sp5fi_time_modified), (char   *) rteFileInfo.sp5fi_time_modified,
                                   VALUE_SEPSTRING_CN00,
                                   (char   *) sFGetFiles[nIndex].szComment,
                                   VALUE_SEPSTRING_CN00,
                                   (char   *) szFullReal  + strlen(szFileDir),
                                   LINE_SEPSTRING_CN00);
              pCurrent = pCurrent + strlen(pCurrent);
            } // end if
          } // end if
        } // end if

      } // end if

      sqlfreaddirc(hDirectory, &szRealName, &rteFileErr);
    } /* end while */

    sqlfclosedirc (hDirectory, &rteFileErr);
  } // end if

  return nFuncReturn;
} // end cn42_FileGetListDir

/*
  -----------------------------------------------------------------------------
  private function cn42_FileType
  -----------------------------------------------------------------------------
*/
static const char * cn42_FileType
      ( const tsp05_RteFileInfo  & rteFileInfo,
        long                       nType )
{
  const char * pReturn = FGET_TYPT_UNKNOWN;

  if (rteFileInfo.sp5fi_media_kind == vf_t_directory) {
    pReturn = FGET_TYPT_DIRECTORY;
  } else {
    switch (nType) {
      case FGET_TYP_ASCII:
        pReturn = FGET_TYPT_ASCII;
        break;
      case FGET_TYP_XML:
        pReturn = FGET_TYPT_XML;
        break;
      case FGET_TYP_BINARY:
        pReturn = FGET_TYPT_BINARY;
        break;
      case FGET_TYP_DIR:
        pReturn = FGET_TYPT_UNKNOWN;
        break;
      default:
        pReturn = FGET_TYPT_UNKNOWN;
        break;

    } // end switch
  } // end if

  return pReturn;
} // end cn42_FileType

/*
  -----------------------------------------------------------------------------
  private function cn42_GetLocation
  -----------------------------------------------------------------------------
*/
static bool cn42_GetLocation
      ( long                  nLocation,
        const tsp00_DbNamec & szDbName,
        tsp00_Pathc         & szLocation,
        DBMSrvMsg_Error     & oError )
{
  bool                 bReturn = true;
  tcn002_XpValueString szValue;
  tsp01_RteError       aRteError;
  int                  nIndex;

  szLocation.Init();

  // compute source directory
  switch (nLocation) {
    case FGET_LOC_WRK:
        // work dir
        if (!sqlGetIndependentWrkPath(szLocation, TERM_WITH_DELIMITER_EO01, &aRteError)) {
            oError.Overrule(DBMSrvMsg_NewRTEError(aRteError));
            szLocation.Init();
            bReturn = false;
        } // end if
        break;
    case FGET_LOC_ENV:
        // env dir
        if (! sqlGetDbrootEnvPath(szLocation, TERM_WITH_DELIMITER_EO01, &aRteError)) {
            oError.Overrule(DBMSrvMsg_NewRTEError(aRteError));
            szLocation.Init();
            bReturn = false;
        } // end if
        break;
    case FGET_LOC_CONFIG:
        // config dir
        if (! sqlGetIndependentConfigPath(szLocation, TERM_WITH_DELIMITER_EO01, &aRteError)) {
            oError.Overrule(DBMSrvMsg_NewRTEError(aRteError));
            szLocation.Init();
            bReturn = false;
        } // end if
        break;

    case FGET_LOC_DBROOT:
      // dir of dbmserver
      SAPDB_strcpy(szLocation, "");
      break;
    case FGET_LOC_SAP:
      // sap dir
      SAPDB_strcpy(szLocation, FGET_LOC_SAP_TXT);
      strcat(szLocation, "/");
      break;
    case FGET_LOC_DBA:
      // dba protocol dir
      if (cn20XParamGetValue(szDbName, PAN_RUNDIR, szValue, oError)) {
        SAPDB_strcpy(szLocation, szValue);
        strcat(szLocation, "/");
        strcat(szLocation, FGET_LOC_DBA_TXT);
        if (cn90DirCreate(szLocation, &aRteError) != OK_CN00) {
            oError.Overrule(DBMSrvMsg_NewRTEError(aRteError));
            szLocation.Init();
            bReturn = false;
        } else {
            strcat(szLocation, "/");
        } // end if
      } else {
        bReturn = false;
      } // end if
      break;
    case FGET_LOC_RUNDIR:
      // rundirectory
      if (cn20XParamGetValue(szDbName, PAN_RUNDIR, szValue, oError)) {
        SAPDB_strcpy(szLocation, szValue);
        strcat(szLocation, "/");
      } else {
        bReturn = false;
      } // end if
      break;
    case FGET_LOC_HISTDIR:
      // rundirectory
      if (cn20XParamGetValue(szDbName, PAN_CRASH_HISTORY_PATH, szValue, oError)) {
        SAPDB_strcpy(szLocation, szValue);
        strcat(szLocation, "/");
      } else {
        bReturn = false;
      } // end if
      break;
    case FGET_LOC_ANALYZER:
      if (!cn42_GetFileName(szDbName, FGET_ANALYZER_CN42, nIndex, oError, szLocation)) {
        bReturn = false;
      } else {
        strcat(szLocation, "/");
      } // end if
      break;
    default:
      bReturn = false;
      break;
  } // end switch

  cn90ChangePathDelimiter(szLocation);

  return bReturn;
} // end cn42_GetLocation

static bool cn42_IsKnlMsg
      ( tsp00_Pathc & szFile ) 
{
  pasbool     cPatOK;
  tsp00_Pathc szPattern1;
  tsp00_Pathc szPattern2;

  szPattern1.Init();
  szPattern2.Init();

  sprintf(szPattern1, "*%c%s", PATH_DELIMITER_CN90, SAPDB_KERNEL_MESSAGES_FILE_NAME);
  sprintf(szPattern2, "*%c%s_?\??\??\??\?_?\?-?\?-?\?", PATH_DELIMITER_CN90, SAPDB_KERNEL_MESSAGES_FILE_NAME);

  s49build_pattern (szPattern1.asCharp(), 1, szPattern1.length(),'\0', false, true, sqlm_internal, cPatOK);
  s49build_pattern (szPattern2.asCharp(), 1, szPattern2.length(),'\0', false, true, sqlm_internal, cPatOK);

  if (s49patmatch (szFile.asCharp(), 0, szFile.length(), szPattern1.asCharp(), 0, szPattern1.length(), '\0') ||
      s49patmatch (szFile.asCharp(), 0, szFile.length(), szPattern2.asCharp(), 0, szPattern2.length(), '\0')    ) {
    return true;
  } // end if

  return false;
} // end cn42_IsKnlMsg


