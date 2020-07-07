/*!
  @file    Msg_ProtFileInfo.cpp
  @author  Robin
  @ingroup Common
  @brief   Message file tag class
  
  handling of file tags

  -------------------------------------------------------------------------

  \if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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

  \endif
 */

#include "hcn42.h"
#include "Messages/Msg_ProtFileInfo.hpp"

#include "KernelCommon/ParameterNames/KernelParam_RunTime.h"

#include "RunTime/Configuration/RTEConf_ParameterAccessSimple.h"

#include "SAPDBCommon/SAPDB_Names.h"

#define DIAG_CLASS_PRT        "protocol"
#define DIAG_CLASS_BKP        "backup"
#define DIAG_CLASS_CFG        "config"
#define DIAG_CLASS_DUMP       "dump"
#define DIAG_CLASS_LVC        "lvc"

#define FGET_NAME_DB          ((const char *) 1)
#define FGET_ORG_EXT          ((const char *) 1)

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



#define FGET_SHOW_NEVER     1000
#define FGET_SHOW_ALWAYS    0

#ifdef WIN32
  #define LO_EXT      ".prt"
  #define LCICMD_EXT  ".bat"
#else
  #define LO_EXT      ".prot"
  #define LCICMD_EXT  ""
#endif

// fiel types
#define FGET_TYP_UNKNOWN      0
#define FGET_TYP_ASCII        1
#define FGET_TYP_BINARY       2
#define FGET_TYP_DIRECTORY    3
#define FGET_TYP_PROTCONV     4

#define FGET_TYPT_UNKNOWN     "UNKNOWN"
#define FGET_TYPT_ASCII       "ASCII"
#define FGET_TYPT_BINARY      "BINARY"
#define FGET_TYPT_DIRECTORY   "DIRECTORY"

class Msg_ProtFileInfoData
{
public:
    const char   * szType;
    long           nShow;
    long           nType;
    long           nOrgType;
    bool           bRemove;
    const SAPDB_UTF8 * szXParam;
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
    Msg_IOutput::OutputStyle outputStyle;
    bool           longColumns;
};

const SAPDB_UTF8 *Msg_ProtFileInfo::GetXParam()
{
    return (m_pProtFileInfoData ? m_pProtFileInfoData->szXParam : NULL);
}

const char   * Msg_ProtFileInfo::GetTypeString()
{
    return (m_pProtFileInfoData ? m_pProtFileInfoData->szType : NULL);
}

long           Msg_ProtFileInfo::GetShow()
{
    return (m_pProtFileInfoData ? m_pProtFileInfoData->nShow : NULL);
}

long           Msg_ProtFileInfo::GetType()
{
    return (m_pProtFileInfoData ? m_pProtFileInfoData->nType : NULL);
}

long           Msg_ProtFileInfo::GetOrgType()
{
    return (m_pProtFileInfoData ? m_pProtFileInfoData->nOrgType : NULL);
}

bool           Msg_ProtFileInfo::GetRemoveFlag()
{
    return (m_pProtFileInfoData ? m_pProtFileInfoData->bRemove : NULL);
}

const char   * Msg_ProtFileInfo::GetName()
{
    return (m_pProtFileInfoData ? m_pProtFileInfoData->szName : NULL);
}

const char   * Msg_ProtFileInfo::GetExt()
{
    return (m_pProtFileInfoData ? m_pProtFileInfoData->szExt : NULL);
}

long           Msg_ProtFileInfo::GetLocation()
{
    return (m_pProtFileInfoData ? m_pProtFileInfoData->nLocation : NULL);
}

const char   * Msg_ProtFileInfo::GetBaName()
{
    return (m_pProtFileInfoData ? m_pProtFileInfoData->szBaName : NULL);
}

const char   * Msg_ProtFileInfo::GetBaExt()
{
    return (m_pProtFileInfoData ? m_pProtFileInfoData->szBaExt : NULL);
}

long           Msg_ProtFileInfo::GetBaLocation()
{
    return (m_pProtFileInfoData ? m_pProtFileInfoData->nBaLocation : NULL);
}

const char   * Msg_ProtFileInfo::GetClass()
{
    return (m_pProtFileInfoData ? m_pProtFileInfoData->szClass : NULL);
}

const char   * Msg_ProtFileInfo::GetComment()
{
    return (m_pProtFileInfoData ? m_pProtFileInfoData->szComment : NULL);
}

long           Msg_ProtFileInfo::GetHeader()
{
    return (m_pProtFileInfoData ? m_pProtFileInfoData->nHeader : NULL);
}

long           Msg_ProtFileInfo::GetTimestampPos()
{
    return (m_pProtFileInfoData ? m_pProtFileInfoData->nTimestampPos : NULL);
}

const char   * Msg_ProtFileInfo::GetTimestampFmt()
{
    return (m_pProtFileInfoData ? m_pProtFileInfoData->szTimestampFmt : NULL);
}

bool           Msg_ProtFileInfo::RemoveEmptyLinesFlag()
{
    return (m_pProtFileInfoData ? m_pProtFileInfoData->bRemoveEmptyLines : NULL);
}

bool           Msg_ProtFileInfo::OperationFlag()
{
    return (m_pProtFileInfoData ? m_pProtFileInfoData->bOperation : NULL);
}

void           Msg_ProtFileInfo::GetCompleteFileName(RTE_Filename &fileName,RTE_DBName &dbName)
{
    if(m_pProtFileInfoData)
    {
        fileName[0] = 0;
        if(m_pProtFileInfoData->szXParam)
        {
            tsp00_ErrText   errText;
            tsp00_C256c     param; 
            RTEConf_ParamaterAccessGetParam  ( *((tsp00_DbNamec *)&dbName)       /*const tsp00_DbNamec &DBNameC */,
                                               *((tsp00_C256c *)m_pProtFileInfoData->szXParam)   /*const tsp00_C256c   &XpKeyC*/,
                                                param                           /*tsp00_C256c   &XpValueC*/,
                                                errText                         /*tsp00_ErrText &ErrText */);
            SAPDB_strncpy_zero_terminated((SAPDB_Char *)fileName,param,sizeof(RTE_Filename));
        }
        else if(m_pProtFileInfoData->szName)
        {
            SAPDB_strncpy_zero_terminated((SAPDB_Char *)fileName,m_pProtFileInfoData->szName,sizeof(RTE_Filename));
        }
        if(m_pProtFileInfoData->szExt)
        {
            SAPDB_strncpy_zero_terminated   ((SAPDB_Char *)fileName+strlen(fileName)
                                            ,m_pProtFileInfoData->szExt
                                            ,sizeof(RTE_Filename)-strlen(fileName));
        }
    }
}

void           Msg_ProtFileInfo::GetCompleteFileName(RTE_Filename &fileName,RTE_Path &path)
{
    if(m_pProtFileInfoData)
    {
        fileName[0] = 0;
        if(m_pProtFileInfoData->szName)
        {
            SAPDB_strncpy_zero_terminated((SAPDB_Char *)fileName,path,sizeof(RTE_Filename));
            SAPDB_strncpy_zero_terminated((SAPDB_Char *)fileName+strlen(fileName),m_pProtFileInfoData->szName,sizeof(RTE_Filename));
            if(m_pProtFileInfoData->szExt)
            {
                SAPDB_strncpy_zero_terminated   ((SAPDB_Char *)fileName+strlen(fileName)
                                                ,m_pProtFileInfoData->szExt
                                                ,sizeof(RTE_Filename)-strlen(fileName));
            }
        }
    }
}   
Msg_IOutput::OutputStyle    Msg_ProtFileInfo::GetStyle(void)
{
    return (m_pProtFileInfoData ? m_pProtFileInfoData->outputStyle : Msg_IOutput::Triple/*Single*/);
}

bool           Msg_ProtFileInfo::LongColumns()
{
    return (m_pProtFileInfoData ? m_pProtFileInfoData->longColumns : NULL);
}


// File                        show                type                orgtype            remove   XParam                           name                                ext             location          backupname     backupext       backuplocation     class             comment                                 Hea nTi szTimestampFmt         bRemoveLi bOperation   outputStyle             longColumns
Msg_ProtFileInfoData Msg_ProtFileInfo::protFileInfoData[] = {                            
  {MSG_PROTFILEINFO_KNLMSGARCHIVE, FGET_SHOW_ALWAYS  , FGET_TYP_ASCII ,FGET_TYP_ASCII     , true  , NULL                            , SAPDB_KERNEL_MESSAGES_ARCHIVE_FILE_NAME, NULL    , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , DIAG_CLASS_PRT  , "Database Errors"                    , 0 , 0 , NULL                  , false , false       , Msg_IOutput::Single   , true  },
  {MSG_PROTFILEINFO_KNLMSG   , FGET_SHOW_ALWAYS  , FGET_TYP_ASCII     ,FGET_TYP_ASCII     , true  , NULL                            , SAPDB_KERNEL_MESSAGES_FILE_NAME   , NULL         , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , DIAG_CLASS_PRT  , "Database Errors"                    , 0 , 0 , NULL                  , false , false       , Msg_IOutput::Single   , true  },
  {MSG_PROTFILEINFO_KNLMSGOLD, FGET_SHOW_ALWAYS  , FGET_TYP_ASCII     ,FGET_TYP_ASCII     , true  , NULL                            , SAPDB_KERNEL_MESSAGES_FILE_NAME   , ".old"       , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , DIAG_CLASS_PRT  , "Database Errors"                    , 0 , 0 , NULL                  , false , false       , Msg_IOutput::Single   , true  },
  {FGET_KNLMSG_CN42          , FGET_SHOW_ALWAYS  , FGET_TYP_ASCII     ,FGET_TYP_ASCII     , true  , NULL                            , "knldiag"                         , NULL         , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , DIAG_CLASS_PRT  , "Database Messages"                  , 0 , 0 , NULL                  , true  , false       , Msg_IOutput::Triple   , true  },
  {FGET_KNLDIAG_CN42         , FGET_SHOW_ALWAYS  , FGET_TYP_ASCII     ,FGET_TYP_ASCII     , true  , NULL                            , "knldiag"                         , NULL         , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , DIAG_CLASS_PRT  , "Database Messages"                  , 0 , 0 , NULL                  , true  , false       , Msg_IOutput::Triple   , true  },
  {FGET_KNLDIAGERR_CN42      , FGET_SHOW_ALWAYS  , FGET_TYP_ASCII     ,FGET_TYP_ASCII     , true  , NULL                            , "knldiag.err.comp"                , NULL         , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , DIAG_CLASS_PRT  , "Database Errors"                    , 0 , 0 , NULL                  , false , false       , Msg_IOutput::Single   , true  },
  {FGET_KNLDIAGOLD_CN42      , FGET_SHOW_ALWAYS  , FGET_TYP_ASCII     ,FGET_TYP_ASCII     , true  , NULL                            , SAPDB_DIAG_FILE_NAME              , ".old"       , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , DIAG_CLASS_PRT  , "Database Messages (OLD)"            , 0 , 0 , NULL                  , true  , false       , Msg_IOutput::Triple   , true  },
  {FGET_KNLMSGERR_CN42       , FGET_SHOW_ALWAYS  , FGET_TYP_ASCII     ,FGET_TYP_ASCII     , true  , NULL                            , "knldiag.err.comp"                , NULL         , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , DIAG_CLASS_PRT  , "Database Errors"                    , 0 , 0 , NULL                  , false , false       , Msg_IOutput::Single   , true  },
  {FGET_KNLMSGOLD_CN42       , FGET_SHOW_ALWAYS  , FGET_TYP_ASCII     ,FGET_TYP_ASCII     , true  , NULL                            , SAPDB_DIAG_FILE_NAME              , ".old"       , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , DIAG_CLASS_PRT  , "Database Messages (OLD)"            , 0 , 0 , NULL                  , true  , false       , Msg_IOutput::Triple   , true  },
  {FGET_KNLTRC_CN42          , FGET_SHOW_ALWAYS  , FGET_TYP_BINARY    ,FGET_TYP_BINARY    , true  , KERNELPARAM__KERNELTRACEFILE    , NULL                              , NULL         , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , DIAG_CLASS_DUMP , "Database Trace"                     , 0 , 0 , NULL                  , false , false       , Msg_IOutput::Triple   , false },
  {FGET_KNLTRCDAT_CN42       , FGET_SHOW_NEVER   , FGET_TYP_BINARY    ,FGET_TYP_BINARY    , true  , KERNELPARAM__KERNELTRACEFILE    , NULL                              , ".dat"       , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , NULL            , "Database Trace Dat"                 , 0 , 0 , NULL                  , false , false       , Msg_IOutput::Triple   , false },
  {FGET_KNLDUMP_CN42         , FGET_SHOW_ALWAYS  , FGET_TYP_BINARY    ,FGET_TYP_BINARY    , true  , KERNELPARAM__KERNELDUMPFILE     , NULL                              , NULL         , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , DIAG_CLASS_DUMP , "Database Dump"                      , 0 , 0 , NULL                  , false , false       , Msg_IOutput::Triple   , false },
  {FGET_KNLEVT_CN42          , FGET_SHOW_ALWAYS  , FGET_TYP_ASCII     ,FGET_TYP_ASCII     , true  , KERNELPARAM__EVENTFILE          , NULL                              , NULL         , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , DIAG_CLASS_PRT  , "Database Events"                    , 0 , 0 , NULL                  , false , false       , Msg_IOutput::Triple   , false },
  {FGET_RTEDUMP_CN42         , FGET_SHOW_ALWAYS  , FGET_TYP_ASCII     ,FGET_TYP_ASCII     , true  , KERNELPARAM__RTEDUMPFILE        , NULL                              , NULL         , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , DIAG_CLASS_DUMP , "Runtime Environment Dump"           , 0 , 0 , NULL                  , false , false       , Msg_IOutput::Triple   , false },
  {FGET_UTLPRT_CN42          , FGET_SHOW_ALWAYS  , FGET_TYP_ASCII     ,FGET_TYP_ASCII     , true  , NULL                            , SAPDB_ADMIN_COMMANDS_FILE_NAME    , ".comp"      , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , DIAG_CLASS_PRT  , "Utility Statements"                 , 0 , 0 , NULL                  , true  , false       , Msg_IOutput::Single   , false },
  {FGET_UTLMSG_CN42          , FGET_SHOW_ALWAYS  , FGET_TYP_ASCII     ,FGET_TYP_ASCII     , true  , NULL                            , SAPDB_ADMIN_COMMANDS_FILE_NAME    , ".comp"      , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , DIAG_CLASS_PRT  , "Utility Statements"                 , 0 , 0 , NULL                  , true  , false       , Msg_IOutput::Single   , false },
  {FGET_BACKHIST_CN42        , FGET_SHOW_ALWAYS  , FGET_TYP_ASCII     ,FGET_TYP_ASCII     , true  , KERNELPARAM__BACKUP_HISTFILE    , NULL                              , NULL         , FGET_LOC_RUNDIR  , FGET_NAME_DB , FGET_ORG_EXT   , FGET_LOC_WRK     , DIAG_CLASS_BKP  , "Backup History"                     , 0 , 0 , NULL                  , false , false       , Msg_IOutput::Triple   , false },
  {FGET_BACKMDF_CN42         , FGET_SHOW_ALWAYS  , FGET_TYP_ASCII     ,FGET_TYP_ASCII     , true  , KERNELPARAM__BACKUP_MED_DEF     , NULL                              , NULL         , FGET_LOC_RUNDIR  , FGET_NAME_DB , FGET_ORG_EXT   , FGET_LOC_WRK     , DIAG_CLASS_BKP  , "Backup Media History"               , 0 , 0 , NULL                  , false , false       , Msg_IOutput::Triple   , false },
  {FGET_BACKEBF_CN42         , FGET_SHOW_ALWAYS  , FGET_TYP_ASCII     ,FGET_TYP_ASCII     , true  , NULL                            , "dbm"                             , ".ebf"       , FGET_LOC_RUNDIR  , FGET_NAME_DB , FGET_ORG_EXT   , FGET_LOC_WRK     , DIAG_CLASS_BKP  , "External Backup History"            , 0 , 0 , NULL                  , false , false       , Msg_IOutput::Triple   , false },
  {FGET_BACKEBP_CN42         , FGET_SHOW_ALWAYS  , FGET_TYP_ASCII     ,FGET_TYP_ASCII     , true  , NULL                            , "dbm"                             , ".ebp"       , FGET_LOC_RUNDIR  , FGET_NAME_DB , FGET_ORG_EXT   , FGET_LOC_WRK     , DIAG_CLASS_BKP  , "External Backup Protocol"           , 0 , 0 , NULL                  , false , false       , Msg_IOutput::Triple   , false },
  {FGET_BACKEBL_CN42         , FGET_SHOW_ALWAYS  , FGET_TYP_ASCII     ,FGET_TYP_ASCII     , true  , NULL                            , "dbm"                             , ".ebl"       , FGET_LOC_RUNDIR  , FGET_NAME_DB , FGET_ORG_EXT   , FGET_LOC_WRK     , DIAG_CLASS_BKP  , "External Backup Log"                , 0 , 0 , NULL                  , false , false       , Msg_IOutput::Triple   , false },
  {FGET_LOADPRT_CN42         , FGET_SHOW_NEVER   , FGET_TYP_ASCII     ,FGET_TYP_ASCII     , true  , NULL                            , "load"                            , LO_EXT       , FGET_LOC_RUNDIR  , "dbm"        , ".ins"         , FGET_LOC_RUNDIR  , NULL            , "XLoad Protocol"                     , 0 , 0 , NULL                  , false , false       , Msg_IOutput::Triple   , false },
  {FGET_XSRVPRT_CN42         , FGET_SHOW_NEVER   , FGET_TYP_ASCII     ,FGET_TYP_ASCII     , false , NULL                            , "xserver_@"                       , ".prt"       , FGET_LOC_WRK     , NULL         , NULL           , 0                , DIAG_CLASS_PRT  , "XServer Messages"                   , 0 , 0 , NULL                  , true  , false       , Msg_IOutput::Triple   , false },
  {FGET_XSRVPRTOLD_CN42      , FGET_SHOW_NEVER   , FGET_TYP_ASCII     ,FGET_TYP_ASCII     , false , NULL                            , "xserver_@"                       , ".old"       , FGET_LOC_WRK     , NULL         , NULL           , 0                , DIAG_CLASS_PRT  , "XServer Messages (OLD)"             , 0 , 0 , NULL                  , true  , false       , Msg_IOutput::Triple   , false },
  {FGET_REPMAN_CN42          , FGET_SHOW_NEVER   , FGET_TYP_ASCII     ,FGET_TYP_ASCII     , false , NULL                            , "loader"                          , ".prt"       , FGET_LOC_WRK     , "dbm"        , ".ins"         , FGET_LOC_RUNDIR  , NULL            , "Replication Manager Protocol"       , 0 , 0 , NULL                  , false , false       , Msg_IOutput::Triple   , false },
  {FGET_DBMPRT_CN42          , FGET_SHOW_ALWAYS  , FGET_TYP_ASCII     ,FGET_TYP_ASCII     , true  , NULL                            , "dbm"                             , ".prt"       , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , DIAG_CLASS_PRT  , "Database Manager Protocol"          , 3 , 0 , "YYYY-MM-DD hh:mm:ss" , false , false       , Msg_IOutput::Triple   , false },
  {FGET_DBMMDF_CN42          , FGET_SHOW_ALWAYS  , FGET_TYP_ASCII     ,FGET_TYP_ASCII     , true  , NULL                            , "dbm"                             , ".mmm"       , FGET_LOC_RUNDIR  , FGET_NAME_DB , FGET_ORG_EXT   , FGET_LOC_CONFIG  , DIAG_CLASS_CFG  , "Database Manager Media"             , 0 , 0 , NULL                  , false , false       , Msg_IOutput::Triple   , false },
  {FGET_DBMPAHI_CN42         , FGET_SHOW_ALWAYS  , FGET_TYP_ASCII     ,FGET_TYP_ASCII     , true  , NULL                            , FGET_NAME_DB                      , ".pah"       , FGET_LOC_CONFIG  , NULL         , NULL           , 0                , DIAG_CLASS_PRT  , "Database Parameter History"         , 0 , 0 , NULL                  , false , false       , Msg_IOutput::Triple   , false },
  {FGET_CTRLMDF_CN42         , FGET_SHOW_ALWAYS  , FGET_TYP_ASCII     ,FGET_TYP_ASCII     , true  , NULL                            , "control"                         , ".med"       , FGET_LOC_RUNDIR  , FGET_NAME_DB , FGET_ORG_EXT   , FGET_LOC_CONFIG  , NULL            , "XControl Media"                     , 0 , 0 , NULL                  , false , false       , Msg_IOutput::Triple   , false },
  {FGET_CTRLLOG_CN42         , FGET_SHOW_ALWAYS  , FGET_TYP_ASCII     ,FGET_TYP_ASCII     , true  , NULL                            , "control"                         , ".log"       , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , NULL            , "XControl Protocol"                  , 0 , 0 , NULL                  , false , false       , Msg_IOutput::Triple   , false },
  {FGET_DBMCFG_CN42          , FGET_SHOW_NEVER   , FGET_TYP_BINARY    ,FGET_TYP_BINARY    , true  , NULL                            , "dbm"                             , ".cfg"       , FGET_LOC_RUNDIR  , FGET_NAME_DB , FGET_ORG_EXT   , FGET_LOC_CONFIG  , DIAG_CLASS_CFG  , "Database Manager Configuration"     , 0 , 0 , NULL                  , false , false       , Msg_IOutput::Triple   , false },
  {FGET_PARAM_CN42           , FGET_SHOW_NEVER   , FGET_TYP_BINARY    ,FGET_TYP_BINARY    , false , NULL                            , FGET_NAME_DB                      , NULL         , FGET_LOC_CONFIG  , "param"      , FGET_ORG_EXT   , FGET_LOC_RUNDIR  , DIAG_CLASS_CFG  , "Database Parameters"                , 0 , 0 , NULL                  , false , false       , Msg_IOutput::Triple   , false },
  {FGET_LCINIT_CN42          , FGET_SHOW_ALWAYS  , FGET_TYP_ASCII     ,FGET_TYP_ASCII     , false , NULL                            , "lcinit"                          , ".log"       , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , DIAG_CLASS_LVC  , "LiveCache Initialisation"           , 0 , 0 , NULL                  , false , false       , Msg_IOutput::Triple   , false },
  {FGET_LCINITCMD_CN42       , FGET_SHOW_ALWAYS  , FGET_TYP_ASCII     ,FGET_TYP_ASCII     , false , NULL                            , "lcinit"                          , LCICMD_EXT   , FGET_LOC_SAP     , NULL         , NULL           , 0                , DIAG_CLASS_LVC  , "LiveCache Initialisation Script"    , 0 , 0 , NULL                  , false , false       , Msg_IOutput::Triple   , false },
  {FGET_LCINITHIS_CN42       , FGET_SHOW_ALWAYS  , FGET_TYP_ASCII     ,FGET_TYP_ASCII     , false , NULL                            , "lcinit"                          , ".his"       , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , DIAG_CLASS_LVC  , "LiveCache Initialisation History"   , 0 , 0 , NULL                  , false , false       , Msg_IOutput::Triple   , false },
  {FGET_INSTPRT_CN42         , FGET_SHOW_ALWAYS  , FGET_TYP_ASCII     ,FGET_TYP_ASCII     , true  , NULL                            , "dbm"                             , ".ins"       , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , DIAG_CLASS_PRT  , "Installation Protocol"              , 0 , 0 , NULL                  , false , false       , Msg_IOutput::Triple   , false },
  {FGET_USER_CN42            , FGET_SHOW_NEVER   , FGET_TYP_BINARY    ,FGET_TYP_BINARY    , true  , NULL                            , FGET_NAME_DB                      , ".upc"       , FGET_LOC_CONFIG  , "dbm"        , FGET_ORG_EXT   , FGET_LOC_RUNDIR  , NULL            , "User Profiles"                      , 0 , 0 , NULL                  , false , false       , Msg_IOutput::Triple   , false },
  {FGET_KNLTRCPRT_CN42       , FGET_SHOW_ALWAYS  , FGET_TYP_ASCII     ,FGET_TYP_ASCII     , true  , NULL                            , FGET_NAME_DB                      , ".prt"       , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , DIAG_CLASS_PRT  , "Kernel Trace Protocol"              , 0 , 0 , NULL                  , false , false       , Msg_IOutput::Triple   , false },
  {FGET_WIZOVER_CN42         , 1                 , FGET_TYP_ASCII     ,FGET_TYP_ASCII     , false , NULL                            , "*"                               , ".wiz"       , FGET_LOC_SAP     , NULL         , NULL           , 0                , NULL            , "Wizard Overview"                    , 0 , 0 , NULL                  , false , true        , Msg_IOutput::Triple   , false },
  {FGET_WIZDETAIL_CN42       , 1                 , FGET_TYP_ASCII     ,FGET_TYP_ASCII     , false , NULL                            , "*"                               , ".wtc"       , FGET_LOC_SAP     , NULL         , NULL           , 0                , NULL            , "Wizard Detail"                      , 0 , 0 , NULL                  , false , true        , Msg_IOutput::Triple   , false },
  {FGET_DBAHIST_CN42         , FGET_SHOW_ALWAYS  , FGET_TYP_ASCII     ,FGET_TYP_ASCII     , true  , NULL                            , "dbahist"                         , ".prt"       , FGET_LOC_DBA     , NULL         , NULL           , 0                , DIAG_CLASS_PRT  , "DBA Action Log"                     , 3 , 15, "YYYYMMDDhhmmss"      , false , true        , Msg_IOutput::Triple   , false },
  {FGET_DBADTL_CN42          , 1                 , FGET_TYP_ASCII     ,FGET_TYP_ASCII     , true  , NULL                            , "dba*"                            , NULL         , FGET_LOC_DBA     , NULL         , NULL           , 0                , NULL            , "DBA Action Log Details"             , 0 , 0 , NULL                  , false , true        , Msg_IOutput::Triple   , false },
  {FGET_DBMSRV_CN42          , FGET_SHOW_NEVER   , FGET_TYP_ASCII     ,FGET_TYP_ASCII     , false , NULL                            , "dbmsrv_@"                        , ".prt"       , FGET_LOC_WRK     , NULL         , NULL           , 0                , DIAG_CLASS_PRT  , "Global Database Manager Protocol"   , 3 , 0 , "YYYY-MM-DD hh:mm:ss" , false , false       , Msg_IOutput::Triple   , false },
  {FGET_AKDMP_CN42           , 1                 , FGET_TYP_BINARY    ,FGET_TYP_BINARY    , true  , NULL                            , "AK"                              , ".dmp"       , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , DIAG_CLASS_DUMP , "AK Diagnose Dump"                   , 0 , 0 , NULL                  , false , false       , Msg_IOutput::Triple   , false },
  {FGET_AKBUF_CN42           , 1                 , FGET_TYP_BINARY    ,FGET_TYP_BINARY    , true  , NULL                            , "AK"                              , ".buf"       , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , DIAG_CLASS_DUMP , "AK Diagnose Buffer"                 , 0 , 0 , NULL                  , false , false       , Msg_IOutput::Triple   , false },
  {FGET_AKSTM_CN42           , 1                 , FGET_TYP_BINARY    ,FGET_TYP_BINARY    , true  , NULL                            , "AK"                              , ".stm"       , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , DIAG_CLASS_DUMP , "AK Diagnose Statement"              , 0 , 0 , NULL                  , false , false       , Msg_IOutput::Triple   , false },
  {FGET_KNLCOR_CN42          , FGET_SHOW_ALWAYS  , FGET_TYP_BINARY    ,FGET_TYP_BINARY    , true  , NULL                            , "*"                               , ".cor"       , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , DIAG_CLASS_DUMP , "\"cor\" file for problem analysis"  , 0 , 0 , NULL                  , false , false       , Msg_IOutput::Triple   , false },
  {FGET_KNLBAD_CN42          , FGET_SHOW_ALWAYS  , FGET_TYP_BINARY    ,FGET_TYP_BINARY    , true  , NULL                            , "*"                               , ".bad"       , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , DIAG_CLASS_DUMP , "\"bad\" file for problem analysis"  , 0 , 0 , NULL                  , false , false       , Msg_IOutput::Triple   , false },
  {FGET_DIAGPACK_CN42        , FGET_SHOW_ALWAYS  , FGET_TYP_BINARY    ,FGET_TYP_BINARY    , true  , NULL                            , "diagpkg"                         , ".tgz"       , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , NULL            , "Diagnose Package"                   , 0 , 0 , NULL                  , false , false       , Msg_IOutput::Triple   , false },
  {FGET_DIAGHIST_CN42        , FGET_SHOW_NEVER   , FGET_TYP_BINARY    ,FGET_TYP_BINARY    , true  , NULL                            , FGET_NAME_DB                      , "_*"         , FGET_LOC_HISTDIR , NULL         , NULL           , 0                , NULL            , "Diagnose History File"              , 0 , 0 , NULL                  , false , false       , Msg_IOutput::Triple   , false },
  {FGET_DIAGDIR_CN42         , FGET_SHOW_ALWAYS  , FGET_TYP_DIRECTORY ,FGET_TYP_DIRECTORY , false , NULL                            , ""                                , NULL         , FGET_LOC_HISTDIR , NULL         , NULL           , 0                , NULL            , "Diagnose History File"              , 0 , 0 , NULL                  , false , false       , Msg_IOutput::Triple   , false },
  {FGET_ANALYZER_CN42        , FGET_SHOW_ALWAYS  , FGET_TYP_DIRECTORY ,FGET_TYP_DIRECTORY , false , NULL                            , "analyzer"                        , NULL         , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , NULL            , "DB Analyzer File"                   , 0 , 21, "YYYY-MM-DD hh:mm:ss" , false , true        , Msg_IOutput::Triple   , false },
  {FGET_LCTRC_CN42           , FGET_SHOW_ALWAYS  , FGET_TYP_ASCII     ,FGET_TYP_ASCII     , true  , NULL                            , "lc*"                             , NULL         , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , NULL            , "LiveCache Trace (ASCII)"            , 0 , 0 , NULL                  , false , true        , Msg_IOutput::Triple   , false },
  {FGET_ATP_CN42             , FGET_SHOW_NEVER   , FGET_TYP_ASCII     ,FGET_TYP_ASCII     , true  , NULL                            , "atp*"                            , NULL         , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , NULL            , "LiveCache Trace (ASCII)"            , 0 , 0 , NULL                  , false , true        , Msg_IOutput::Triple   , false },
  {FGET_SDBINFO_CN42         , FGET_SHOW_ALWAYS  , FGET_TYP_ASCII     ,FGET_TYP_ASCII     , false , NULL                            , "sdbinfo"                         , ".prt"       , FGET_LOC_WRK     , NULL         , NULL           , 0                , DIAG_CLASS_PRT  , "Operating System Information"       , 0 , 0 , NULL                  , false , true        , Msg_IOutput::Triple   , false },
  {FGET_ANAPID_CN42          , FGET_SHOW_NEVER   , FGET_TYP_BINARY    ,FGET_TYP_BINARY    , false , NULL                            , "DBAN"                            , ".pid"       , FGET_LOC_ANALYZER, NULL         , NULL           , 0                , NULL            , "DBanalyzer process ID"              , 0 , 0 , NULL                  , false , false       , Msg_IOutput::Triple   , false },
  {FGET_ANASID_CN42          , FGET_SHOW_NEVER   , FGET_TYP_BINARY    ,FGET_TYP_BINARY    , false , NULL                            , "DBAN"                            , ".sid"       , FGET_LOC_ANALYZER, NULL         , NULL           , 0                , NULL            , "DBanalyzer session ID"              , 0 , 0 , NULL                  , false , false       , Msg_IOutput::Triple   , false },
  {FGET_EVTDISPCONF_CN42     , FGET_SHOW_ALWAYS  , FGET_TYP_ASCII     ,FGET_TYP_ASCII     , true  , NULL                            , "dbmevtdisp"                      , ".cfg"       , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , DIAG_CLASS_CFG  , "Event Dispatcher Configuration"     , 0 , 0 , NULL                  , false , false       , Msg_IOutput::Triple   , false },
  {FGET_EVTDISPCDEF_CN42     , FGET_SHOW_NEVER   , FGET_TYP_ASCII     ,FGET_TYP_ASCII     , false , NULL                            , "dbmevtdisp"                      , ".cfg"       , FGET_LOC_ENV     , NULL         , NULL           , 0                , DIAG_CLASS_CFG  , "Event Dispatcher Configuration"     , 0 , 0 , NULL                  , false , false       , Msg_IOutput::Triple   , false },
  {FGET_EVTDISPPRT_CN42      , FGET_SHOW_ALWAYS  , FGET_TYP_ASCII     ,FGET_TYP_ASCII     , true  , NULL                            , "dbmevtdisp"                      , ".prt"       , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , DIAG_CLASS_CFG  , "Event Dispatcher Protocol"          , 0 , 0 , NULL                  , false , false       , Msg_IOutput::Triple   , false },
  {FGET_SEVTDISPCFG_CN42     , FGET_SHOW_NEVER   , FGET_TYP_ASCII     ,FGET_TYP_ASCII     , true  , NULL                            , "dbmsilevt"                       , ".cfg"       , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , DIAG_CLASS_CFG  , "Silent Event Dispatcher Config"     , 0 , 0 , NULL                  , false , false       , Msg_IOutput::Triple   , false },
  {FGET_SHMDATA_CN42         , FGET_SHOW_NEVER   , FGET_TYP_BINARY    ,FGET_TYP_BINARY    , true  , NULL                            , "dbm"                             , ".shm"       , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , NULL            , "DBMServer shared memory data"       , 0 , 0 , NULL                  , false , false       , Msg_IOutput::Triple   , false },
  {FGET_SHMADMIN_CN42        , FGET_SHOW_NEVER   , FGET_TYP_BINARY    ,FGET_TYP_BINARY    , true  , NULL                            , "dbm"                             , ".shi"       , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , NULL            , "DBMServer shared memory admin data" , 0 , 0 , NULL                  , false , false       , Msg_IOutput::Triple   , false },
//  {FGET_SCHEDULERCFG_CN42    , FGET_SHOW_NEVER   , FGET_TYP_ASCII     ,FGET_TYP_ASCII     , true  , NULL                          , DBMSrvScd_Persistor::m_FileNameBase, DBMSrvScd_Persistor::m_FileNameExtension, FGET_LOC_RUNDIR, NULL, NULL, 0, DIAG_CLASS_CFG,"DBM Command Scheduler Configuration", 0, 0, NULL             , false , false},
  {FGET_SCHEDULERCFG_CN42    , FGET_SHOW_NEVER   , FGET_TYP_ASCII     ,FGET_TYP_ASCII     , true  , NULL                            , "dbmsrvscd"                       , ".cfg"       , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , DIAG_CLASS_CFG  ,"DBM Command Scheduler Configuration" , 0 , 0 , NULL                  , false , false       , Msg_IOutput::Triple   , false },
  {FGET_KNLDIAGERRNEW_CN42   , FGET_SHOW_NEVER   , FGET_TYP_ASCII     ,FGET_TYP_ASCII     , true  , NULL                            , SAPDB_DIAG_FILE_NAME              , "_err"       , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , DIAG_CLASS_PRT  , "Database Errors"                    , 0 , 0 , NULL                  , true  , false       , Msg_IOutput::Triple   , false },
  {FGET_KNLDIAGSTART_CN42    , FGET_SHOW_NEVER   , FGET_TYP_ASCII     ,FGET_TYP_ASCII     , true  , NULL                            , SAPDB_DIAG_FILE_NAME              , ".start"     , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , DIAG_CLASS_PRT  , "Database Messages"                  , 0 , 0 , NULL                  , true  , false       , Msg_IOutput::Triple   , false },
  {FGET_KNLDIAGRUN_CN42      , FGET_SHOW_NEVER   , FGET_TYP_ASCII     ,FGET_TYP_ASCII     , true  , NULL                            , SAPDB_DIAG_FILE_NAME              , ".run"       , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , DIAG_CLASS_PRT  , "Database Messages"                  , 0 , 0 , NULL                  , true  , false       , Msg_IOutput::Triple   , false },
  {FGET_KNLDIAGSTARTOLD_CN42 , FGET_SHOW_NEVER   , FGET_TYP_ASCII     ,FGET_TYP_ASCII     , true  , NULL                            , SAPDB_DIAG_FILE_NAME              , ".start.old" , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , DIAG_CLASS_PRT  , "Database Messages (OLD)"            , 0 , 0 , NULL                  , true  , false       , Msg_IOutput::Triple   , false },
  {FGET_KNLDIAGRUNOLD_CN42   , FGET_SHOW_NEVER   , FGET_TYP_ASCII     ,FGET_TYP_ASCII     , true  , NULL                            , SAPDB_DIAG_FILE_NAME              , ".run.old"   , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , DIAG_CLASS_PRT  , "Database Messages (OLD)"            , 0 , 0 , NULL                  , true  , false       , Msg_IOutput::Triple   , false },
//{FGET_APOCOMTRC_CN42       , FGET_SHOW_ALWAYS  , FGET_TYP_ASCII     , FGET_TYP_ASCII    , true  , NULL                            , "apo_com_trace*"                  , ".txt"       , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , DIAG_CLASS_LVC  , "APO Trace"                          , 0 , 0 , NULL                  , false , false},
//{FGET_LCBIN_CN42           , FGET_SHOW_ALWAYS  , FGET_TYP_ASCII     , FGET_TYP_ASCII    , true  , NULL                            , "lc*"                             , NULL         , FGET_LOC_RUNDIR  , NULL         , NULL           , 0                , DIAG_CLASS_LVC  , "LiveCache Trace (binary)"           , 0 , 0 , NULL                  , false , false},
  {NULL                      , FGET_SHOW_NEVER   , FGET_TYP_UNKNOWN   ,FGET_TYP_UNKNOWN   , false , NULL                            , NULL                              , NULL         , 0                , NULL         , NULL           , 0                , NULL            , ""                                   , 0 , 0 , NULL                  , false , false       , Msg_IOutput::Triple   , false }
};

Msg_ProtFileInfo Msg_ProtFileInfo::ByTagName(SAPDB_Char *tagName)
{
    SAPDB_UInt i=0;
    while(protFileInfoData[i].szType)
    {
        if(0 == strcmp(protFileInfoData[i].szType,tagName))
        {
            return Msg_ProtFileInfo(&protFileInfoData[i]);
        }
        i++;
    }
    return NULL;
}

Msg_ProtFileInfo Msg_ProtFileInfo::ByFileName(SAPDB_Char *fileName)
{
    SAPDB_UInt i=0;
    while(protFileInfoData[i].szType)
    {
        if(     protFileInfoData[i].szName 
            && (FGET_NAME_DB != protFileInfoData[i].szName ))
        {
            if( protFileInfoData[i].szExt )
            {
                SAPDB_UInt lengthOfBaseName = strlen(protFileInfoData[i].szName);
                if( strlen(fileName) > lengthOfBaseName )   // if the fileName given is shorter than the base name from the table, it cannot match base name + extension!
                {
                    if( 0 == strncmp(protFileInfoData[i].szName,fileName,lengthOfBaseName)
                    && 0 == strcmp(protFileInfoData[i].szExt,fileName+lengthOfBaseName))
                    {
                        return Msg_ProtFileInfo(&protFileInfoData[i]);
                    }
                }
            }
            else
            {
                if (0 == strcmp(protFileInfoData[i].szName,fileName))
                {
                    return Msg_ProtFileInfo(&protFileInfoData[i]);
                }
            }
        }
        i++;
    }
    return NULL;
}

