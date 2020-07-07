
/*!**********************************************************************

  module: vls31_odbc_Result.cpp

  -----------------------------------------------------------------------

  responsible : d033893 and SteffenS

  special area: SAP DB LOADER

  description : odbc

  version     : 7.6

  see also:

  -----------------------------------------------------------------------


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

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gls00.h"
#include "hls05.h"
#include "hls10.h"
#include "hls11.h"
#include "hls15.h"
#include "hls30.h"
#include "hls99.h"
#include "heo01.h"
#include "hls31_odbc_Result.h"
#include "hls29_dataupdate.h"
#include "hls27_dataextract.h"

#define SEPARATOR    ""CMD_SEPARATOR_TOKEN_LS00""NEW_LINE""
#define SEPARATOR_   ""NEW_LINE""CMD_SEPARATOR_TOKEN_LS00""NEW_LINE""

_TCHAR const *pszSQLModeString_ls31[6] = {_T("NOT USED"),
                                          _T("SQLMODE INTERNAL"NEW_LINE""),
                                          _T("SQLMODE DB2"NEW_LINE""),
                                          _T("SQLMODE ANSI"NEW_LINE""),
                                          _T("SQLMODE ORACLE"NEW_LINE""),
                                          _T("SQLMODE SAPR3"NEW_LINE"")};

_TCHAR const *pszLoadCmdBeginString2[6] = {_T("NOT USED"),
                                          _T("DATALOAD TABLE "),
                                          _T("FASTLOAD WITH 80%% USAGE TABLE "),
                                          _T("DATAUPDATE TABLE "),
                                          _T("DATALOAD TABLE "),
                                          _T("FASTLOAD WITH 80%% USAGE TABLE ")};

_TCHAR const *pszDateTimeType2[6] = {_T("FREE MASK"),
                                    _T("ISO"),
                                    _T("USA"),
                                    _T("EUR"),
                                    _T("JIS"),
                                    _T("INTERNAL")};

_TCHAR const *pszEncoding2[8] = {_T("NOT USED"),
                                _T("ASCII"),
                                _T("EBCDIC"),
                                _T("UTF8"),
                                _T("BINARY"),
                                _T("UCS2"),
                                _T("UCS2"),
                                _T("UCS2")};

GUID_* GUID_::_guidinstance = NULL;

/*===========================================================================*
 *    GUID_::Instance()
 *===========================================================================*/
GUID_* GUID_::Instance()
{
    if  (_guidinstance == NULL)
         _guidinstance = new GUID_();
    return _guidinstance;
};
/*===========================================================================*
 *    GUID_::GUID_()
 *===========================================================================*/
GUID_::GUID_()
{
    Id_Init();
}
/*===========================================================================*
 *    GUID_::~GUID_()
 *===========================================================================*/
GUID_::~GUID_()
{
}
/*===========================================================================*
 *    GUID_::Id_Init
 *===========================================================================*/
SAPDB_Bool GUID_::Id_Init()
{
    if (idManagementInitialized == SAPDB_FALSE)
        RTE_InitIdGenerator();
    return SAPDB_TRUE;
}
/*===========================================================================*
 *    GUID_::IdGetNext
 *===========================================================================*/
SAPDB_Bool GUID_::IdGetNext( GUID_Id id )
{
    if (!RTE_FillUniqueId(&hIdManagement))
        return SAPDB_FALSE;
    SAPDB_memcpy(id, &hIdManagement, GUID_MAX_ID_LEN);

    return SAPDB_TRUE;
}
/*===========================================================================*
 *    GUID_::IdAsString
 *===========================================================================*/
void GUID_::IdAsString( GUID_Id id, GUID_IdString idString )
{
    SAPDB_UInt2 i = 0;

    for (i = 0; i < GUID_MAX_ID_LEN; i++ )
        sprintf( idString + (i*2), "%02X", (unsigned char)id[i] );

    idString[GUID_MAX_ID_STRING_LEN] = '\0';
}
/*===========================================================================*
 *    GUID_::IdInitValue
 *===========================================================================*/
void GUID_::IdInitValue( GUID_Id  id )
{
     memset(id, 0, GUID_MAX_ID_LEN);
}
/*===========================================================================*
 *    GUID_::IdStringInitValue
 *===========================================================================*/
void GUID_::IdStringInitValue( GUID_IdString idString )
{
     strncpy(idString, ID_ROOT_STRING, GUID_MAX_ID_STRING_LEN);
     idString[GUID_MAX_ID_STRING_LEN] = '\0';
}
/*===========================================================================*
 *    GUID_::IdIsInitialValue
 *===========================================================================*/
SAPDB_Bool GUID_::IdIsInitialValue( GUID_Id id )
{
    if (memcmp(id, ID_INITIAL, GUID_MAX_ID_LEN) == 0)
        return SAPDB_TRUE;
    else
        return SAPDB_FALSE;
}
/*===========================================================================*
 *    GUID_::IdStringIsInitialValue
 *===========================================================================*/
SAPDB_Bool GUID_::IdStringIsInitialValue( GUID_IdString idString )
{
    if (memcmp(idString, ID_INITIAL_STRING, GUID_MAX_ID_STRING_LEN) == 0)
        return SAPDB_TRUE;
    else
        return SAPDB_FALSE;
}
/*===========================================================================*
 *    GUID_::IdsAreIdentical
 *===========================================================================*/
SAPDB_Bool GUID_::IdsAreIdentical( GUID_Id id1, GUID_Id id2 )
{
    if (memcmp(id1, id2, GUID_MAX_ID_LEN) == 0)
        return SAPDB_TRUE;
    else
        return SAPDB_FALSE;
}
/*===========================================================================*
 *    GUID_::IdStringsAreIdentical
 *===========================================================================*/
SAPDB_Bool GUID_::IdStringsAreIdentical( GUID_IdString idString1, GUID_IdString idString2 )
{
    if (memcmp(idString1, idString2, GUID_MAX_ID_STRING_LEN) == 0)
        return SAPDB_TRUE;
    else
        return SAPDB_FALSE;
}
/*===========================================================================*
 *     MetaDataDef::MetaDataDef()
 *===========================================================================*/
MetaDataDef::MetaDataDef( tls00_TransformationCmd *pTransformationCmd , tls00_DBInfo * pDBInfo )
            :m_pDBInfo(pDBInfo),
             m_catMaxAdmissibleErrors( pDBInfo->dbiSession_ls00.siMaxAdmissibleErrors)
{
    // RelationalDef
    m_pRelationalDef  = NULL;
    switch(m_pDBInfo->dbiSession_ls00.siSQLMode_ls00)
    {
        case sp1sm_db2:
        {
            m_catSqlMode = cpr_kind_db2;
            break;
        }
        case sp1sm_ansi:
        {
            m_catSqlMode = cpr_kind_ansi;
            break;
        }
        case sp1sm_oracle:
        {
            m_catSqlMode = cpr_kind_oracle;
            break;
        }
        case sp1sm_internal:
        default:
        {
            m_catSqlMode = cpr_kind_internal;
        }
    }
    // TransforamtionDef
    m_pTransformationDef = NULL;
    //Authorization
    m_pUserConnectionDef = NULL;
    //Error Handling
    m_pError = new ErrorHndl();
}
/*===========================================================================*
 *     MetaDataDef::Statement()
 *===========================================================================*/
StudioOAL_String MetaDataDef::Statement(const StatementNumbers StatementNumber)
{
    typedef struct
    {
        StatementNumbers    Type;
        char                pStatement[3328];
    } StatementStruct;

    static StatementStruct StatementDesc_SysLoader[] =
    {
        {upd_Transformation_800,   _T("UPDATE SYSLOADER.TRANSFORMATIONMODEL ") },
        {upd_Transformation_801,   _T("UPDATE SYSLOADER.TRANSFORMATIONMODEL "\
                                      "SET    EXCLUDE     =    %s  "\
                                      "WHERE  PACKAGEGUID =  x'%s' ")},
        {upd_Transformation_802,   _T("UPDATE SYSLOADER.TRANSFORMATIONMODEL "\
                                      "SET CATALOGSTREAMFORMAT = 'DDL'      "\
                                      "WHERE PACKAGEGUID       =  x'%s' AND \"SCHEMANAME\" = '%s' AND \"TABLENAME\" = '%s' AND TABLETYPE = '%s'")},
        {upd_Transformation_803,   _T("UPDATE SYSLOADER.TRANSFORMATIONMODEL "\
                                      "SET DATASTREAMNAME  =   '%s' "\
                                      "WHERE PACKAGEGUID   =  x'%s'     AND \"SCHEMANAME\" = '%s' AND \"TABLENAME\" = '%s' AND TABLETYPE IN ('TABLE') ")},
        {upd_Transformation_804,   _T("UPDATE SYSLOADER.TRANSFORMATIONMODEL "\
                                      "SET   \"STARTTIME\"   = TIMESTAMP "\
                                      "WHERE \"PACKAGEGUID\" = x'%s'   AND \"SCHEMANAME\" = '%s' AND \"TABLENAME\" = '%s'  AND TABLETYPE IN ('TABLE') ")},
        {upd_Transformation_805,   _T("UPDATE SYSLOADER.TRANSFORMATIONMODEL "\
                                      "SET   \"ENDTIME\"  =  TIMESTAMP "\
                                      "WHERE PACKAGEGUID  =  x'%s'     AND  \"SCHEMANAME\" = '%s' AND \"TABLENAME\" = '%s'  AND TABLETYPE IN ('TABLE') ")},
        {upd_Transformation_806,   _T("UPDATE SYSLOADER.TRANSFORMATIONMODEL  "\
                                      "SET   \"ROWCOUNT\"              = %s, "\
                                      "      \"DATASTREAMHEADERBLOCK\" = %s, "\
                                      "      \"DATASTREAMHEADERPOS\"   = %s  "\
                                      "WHERE \"PACKAGEGUID\" = x'%s'   AND \"SCHEMANAME\" = '%s' AND \"TABLENAME\" = '%s'  AND TABLETYPE IN ('TABLE') ")},
        {upd_Transformation_807,   _T("UPDATE SYSLOADER.TRANSFORMATIONMODEL "\
                                      "SET "\
                                      "\"ROWCOUNT\" = %s, "\
                                      "\"DATASTREAMDATABLOCK\" = %s, "\
                                      "\"DATASTREAMDATAPOS\"   = %s  "\
                                      "WHERE \"PACKAGEGUID\"   = x'%s' AND \"SCHEMANAME\" = '%s' AND \"TABLENAME\" = '%s'  AND TABLETYPE IN ('TABLE') ")},
        {upd_Transformation_808,   _T("UPDATE SYSLOADER.TRANSFORMATIONMODEL "\
                                      "SET   \"RESTART\"       = FALSE "\
                                      "WHERE \"PACKAGEGUID\"   = x'%s' AND \"SCHEMANAME\" = '%s' AND \"TABLENAME\" = '%s'  AND TABLETYPE IN ('TABLE') ")},
        {upd_Transformation_809,   _T("UPDATE DATA TABLE "\
                                      "SYSLOADER.TRANSFORMATIONMODEL "\
                                      "    \"PACKAGEGUID\"           '%s' %s     \"SUBSTR(CMD,1,5)\"  'TABLE'                             \"TABLENAME\"              2              SET \"STEPID\"                 3 "\
                                      "SET \"CATALOGSTREAMNAME\"       4  DEFAULT NULL SET \"CATALOGSTREAMTYPE\"     5  DEFAULT NULL SET \"CATALOGSTREAMEBID\"       6 DEFAULT NULL SET \"CATALOGSTREAMFORMAT\"    7 DEFAULT NULL "\
                                      "SET \"DATASTREAMNAME\"          8  DEFAULT NULL SET \"DATASTREAMTYPE\"        9  DEFAULT NULL SET \"DATASTREAMEBID\"         10 DEFAULT NULL SET \"DATASTREAMFORMAT\"      11 DEFAULT NULL SET \"CODETYPE\"               12 DEFAULT NULL "\
                                      "SET \"DATASTREAMHEADERBLOCK\"  13  DEFAULT NULL SET \"DATASTREAMHEADERPOS\"  14  DEFAULT NULL SET \"DATASTREAMDATABLOCK\"    15 DEFAULT NULL SET \"DATASTREAMDATAPOS\"     16 DEFAULT NULL SET \"DATASTREAMTRAILERBLOCK\" 17 DEFAULT NULL SET \"DATASTREAMTRAILERPOS\"  18 DEFAULT NULL  "\
                                      "INSTREAM %s %s '%s' COMPRESSED INTERNAL")},
        //sel_Transformation_715, pszPackageQualifier_ls00
        {upd_Transformation_810,   _T("UPDATE DATA TABLE "\
                                      "SYSLOADER.TRANSFORMATIONMODEL "\
                                      "    \"PACKAGEGUID\"           '%s'   \"SUBSTR(CMD,1,5)\"  'TABLE'   \"SCHEMANAME\"  1  \"TABLENAME\"                2 "\
                                      "SET \"EXCLUDE\"                 2  NULL IF POS 2 <> \'_\'  "\
                                      "INSTREAM FILE '%s' COMPRESSED %s INTERNAL")},
        {upd_Transformation_811,   _T("UPDATE SYSLOADER.TRANSFORMATIONMODEL "\
                                      "SET   \"EXCLUDE\"     = TRUE  "\
                                      "WHERE \"PACKAGEGUID\" = x'%s' AND \"SCHEMANAME\" = '%s' AND \"TABLENAME\" = '%s' ")},
        {upd_Transformation_812,   _T("UPDATE SYSLOADER.TRANSFORMATIONMODEL "\
                                      "SET   \"REQUESTTIME\" = %s , \"RECEIVETIME\" = %s , \"IOTIME\" = %s, \"LOGTIME\" = %s "\
                                      "WHERE \"PACKAGEGUID\" = x'%s' AND \"SCHEMANAME\" = '%s' AND \"TABLENAME\" = '%s' ")},
        {upd_Transformation_813,   _T("UPDATE SYSLOADER.TRANSFORMATIONMODEL "\
                                      "SET "\
                                      "DATASTREAMTRAILERBLOCK  = %s, "\
                                      "DATASTREAMTRAILERPOS    = %s  "\
                                      "WHERE \"PACKAGEGUID\" = x'%s'  AND \"SCHEMANAME\" = '%s' AND \"TABLENAME\" = '%s' AND TABLETYPE IN ('TABLE') ")},
        {sel_Relational_1,         _T("SELECT S.OWNER, U.GROUPNAME, U.USERNAME, S.SCHEMANAME, U.USERMODE, U.MAXTIMEOUT, U.COSTWARNING, U.COSTLIMIT, U.CONNECTMODE, U.COMMENT "\
                                      "FROM   DOMAIN.USERS U, DOMAIN.SCHEMAS S "\
                                      "WHERE  U.USERNAME   <> S.SCHEMANAME "\
                                      "AND    S.OWNER      <> S.SCHEMANAME "\
                                      "AND    U.USERMODE          NOT IN ('ADMIN','SYSDBA') "\
                                      "AND    S.SCHEMANAME        NOT IN ('DBM', 'OMS', 'DOMAIN', 'SYSINFO')"\
                                      "AND    U.ISREPLICATIONUSER NOT IN ('YES')"\
                                      "AND    U.USERNAME  = '%s'  AND  S.SCHEMANAME  = '%s' "\
                                      "UNION "\
                                      "SELECT S.OWNER, U.GROUPNAME, U.USERNAME, S.SCHEMANAME, U.USERMODE, U.MAXTIMEOUT, U.COSTWARNING, U.COSTLIMIT, U.CONNECTMODE, U.COMMENT "\
                                      "FROM   DOMAIN.USERS U, DOMAIN.SCHEMAS S "\
                                      "WHERE  U.USERNAME   = S.SCHEMANAME "\
                                      "AND    S.OWNER      = S.SCHEMANAME "\
                                      "AND    U.USERMODE          NOT IN ('ADMIN','SYSDBA') "\
                                      "AND    S.SCHEMANAME        NOT IN ('DBM', 'OMS', 'DOMAIN', 'SYSINFO')"\
                                      "AND    U.ISREPLICATIONUSER NOT IN ('YES')"\
                                      "AND    U.USERNAME  = '%s'  AND  S.SCHEMANAME  = '%s' ")},
        {sel_Relational_2,         _T("SELECT ROLE, PASSWORD_REQUIRED "\
                                      "FROM   DOMAIN.ROLES "\
                                      "WHERE  OWNER = '%s' ")},
        {sel_Relational_3,         _T("SELECT SCHEMANAME, SEQUENCE_NAME,INCREMENT_BY,LAST_NUMBER,MAX_VALUE,MIN_VALUE,CYCLE_FLAG,CACHE_SIZE,ORDER_FLAG "\
                                      "FROM   DOMAIN.SEQUENCES "\
                                      "WHERE  SCHEMANAME = '%s' ")},
        {sel_Relational_4,         _T("SELECT OWNER, SCHEMANAME, DBPROCNAME, DEFINITION, COMMENT "\
                                      "FROM   DOMAIN.DBPROCEDURES "\
                                      "WHERE  SCHEMANAME = '%s' AND DEFINITION IS NOT NULL AND DBPROCNAME NOT IN ('TRANSFORMATIONPACKAGE','DEFINE_SERVER','SHOW_SERVER','SHOW_SESSIONS','UNDEFINE_SERVER') "\
									  "ORDER  BY CREATEDATE, CREATETIME ASC ")},
        {sel_Relational_5,         _T("SELECT COLUMNNAME, DATATYPE, LEN, DEC, CODETYPE, MODE, KEYPOS, NULLABLE, \"DEFAULT\",COMMENT  "\
                                      "FROM   DOMAIN.COLUMNS "\
                                      "WHERE  SCHEMANAME = '%s' AND TABLENAME = '%s' ") },
        {sel_Relational_6,         _T("SELECT CONSTRAINTNAME, DEFINITION "\
                                      "FROM   DOMAIN.CONSTRAINTS "\
                                      "WHERE  SCHEMANAME = '%s' AND TABLENAME = '%s' ")      },
        {sel_Relational_7,         _T("SELECT FKEYNAME, COLUMNNAME, REFTABLENAME, REFSCHEMANAME, REFCOLUMNNAME, RULE, COMMENT "\
                                      "FROM   DOMAIN.FOREIGNKEYCOLUMNS "\
                                      "WHERE  SCHEMANAME = '%s' AND TABLENAME = '%s' ") },
        {sel_Relational_8,         _T("SELECT COMMENT, SAMPLE_PERCENT, SAMPLE_ROWS, VARIABLE_COLUMNS "\
                                      "FROM   DOMAIN.TABLES "\
                                      "WHERE  SCHEMANAME = '%s' AND TABLENAME = '%s' ")},
        {sel_Relational_9,         _T("SELECT TYPE, INDEXNAME, COMMENT, DISABLED "\
                                      "FROM   DOMAIN.INDEXES "\
                                      "WHERE  SCHEMANAME = '%s' AND TABLENAME = '%s' ")},
        {sel_Relational_10,        _T("SELECT DEFINITION, TRIGGERNAME, TABLENAME, COMMENT, OWNER "\
                                      "FROM   DOMAIN.TRIGGERS "\
                                      "WHERE  SCHEMANAME = '%s' AND TABLENAME = '%s' AND DEFINITION IS NOT NULL  ")},
        {sel_Relational_11,        _T("SELECT VIEWNAME, OWNER, DEFINITION "\
                                      "FROM   DOMAIN.VIEWDEFS "\
                                      "WHERE  SCHEMANAME = '%s' AND VIEWNAME = '%s' " )},
        {sel_Relational_12,        _T("SELECT OWNER, SYNONYMNAME, PUBLIC, TABLESCHEMANAME, TABLENAME, COMMENT "\
                                      "FROM   DOMAIN.SYNONYMS "\
                                      "WHERE  OWNER = '%s' AND SYNONYMNAME = '%s' AND TABLESCHEMANAME NOT IN ('SYS') " ) },
        {sel_Relational_13,        _T("SELECT COUNT(DISTINCT SCHEMANAME) "\
                                      "FROM   DOMAIN.COLUMNS "\
                                      "WHERE  TABLENAME = '%s' ")},
        {sel_Relational_14,        _T("SELECT \"DEFAULT\", POS, DATATYPE "\
                                      "FROM   DOMAIN.COLUMNS "\
                                      "WHERE  SCHEMANAME = '%s' AND TABLENAME = '%s' ORDER BY POS ") },
        {sel_Relational_15,        _T("SELECT TYPE, COLUMNNAME, SORT, COMMENT "\
                                      "FROM   DOMAIN.INDEXCOLUMNS "\
                                      "WHERE  SCHEMANAME = '%s' AND TABLENAME = '%s' AND INDEXNAME = '%s' ORDER BY COLUMNNO" )},
        {sel_Relational_16,        _T("SELECT OWNER, GROUPNAME, USERNAME, USERNAME, USERMODE, MAXTIMEOUT, COSTWARNING, COSTLIMIT, CONNECTMODE, COMMENT "\
                                      "FROM   DOMAIN.USERS "\
                                      "%s "\
                                      "ORDER  BY CREATEDATE,CREATETIME ASC ")},
        {sel_Relational_17,        _T("SELECT OWNER, SCHEMANAME "\
		                              "FROM   DOMAIN.SCHEMAS "\
									  "WHERE  OWNER = '%s' AND OWNER <> SCHEMANAME AND SCHEMANAME NOT IN ('DBM', SYSDBA , 'OMS', 'DOMAIN', 'SYSINFO' )")},
        {sel_Relational_18,        _T("SELECT COLUMNNAME FROM DOMAIN.COLUMNS WHERE SCHEMANAME = '%s' AND TABLENAME = '%s' ORDER BY POS")},
        {exist_Transformation_100, _T("SYSLOADER.TRANSFORMATIONMODEL")},
        {exist_Transformation_101, _T("SYSLOADER.TRANSFORMATIONPACKAGES")},
        {exist_Transformation_102, _T("SYSLOADER.TRANSFORMATIONSTEPS" )},
        {exist_Transformation_103, _T("SYSLOADER.TRANSFORMATIONRESOURCES")},
        {exist_Transformation_104, _T("SYSLOADER.TRANSFORMATIONSTATISTIC" )},
        {exist_Transformation_105, _T("SYSLOADER.TRANSFORMATIONMONITOR")},
        {exist_Transformation_106, _T("TEMP.LOA_SAVE_CATALOG_LS00")  },
        {exist_Transformation_107, _T("TRANSFORMATIONPACKAGE")},
        {exist_Transformation_108, _T("SELECT COUNT(*) FROM %s.DBPROCEDURES %s ")},
        {exist_Transformation_109, _T("LOA_MAP_CURSOR")},
        {exist_Transformation_110, _T("LOA_CATALOG_CURSOR")},
        {exist_Transformation_111, _T("EXISTS TABLE \"%s\".\"%s\"")},
        {exist_Transformation_112, _T("SET CURRENT_SCHEMA=SYSLOADER")},
        {exist_Transformation_113, _T("SELECT SOURCESCHEMANAME, TARGETSCHEMANAME FROM SYSLOADER.MAPSCHEMANAMES WHERE MAPPINGNAME = '%s' ")},
        {drop_Transformation_200,  _T("DROP TABLE  TEMP.LOA_SAVE_CATALOG_LS00 ")},
        {drop_Transformation_201,  _T("DROP DBPROC SYSLOADER.TRANSFORMATIONPACKAGE ")},
        {ddl_Transformation_300 ,  _T("CREATE TABLE SYSLOADER.TRANSFORMATIONMODEL( "\
            /*01*/                    "PACKAGEGUID            CHAR(24) BYTE, "\
            /*02*/                    "TASKID                 FIXED(6,0)  DEFAULT 0, "\
            /*03*/                    "SERVERNODE             CHAR(32)    DEFAULT NULL, "\
            /*04*/                    "SERVERDB               CHAR(32)    DEFAULT NULL, "\
            /*05*/                    "CODETYPE               CHAR(32)    DEFAULT NULL, "\
            /*06*/                    "USERNAME               CHAR(32)    DEFAULT NULL, "\
            /*07*/                    "CODEPAGEID             CHAR(32)    DEFAULT NULL, "\
            /*08*/                    "STORAGEDEVICE          CHAR(32)    DEFAULT NULL, "\
            /*09*/                    "TRANSACTIONSIZE        FIXED(10,0) DEFAULT 0, "\
            /*10*/                    "TRANSFORMATION         CHAR(32)    DEFAULT NULL, "\
            /*11*/                    "PART                   CHAR(8)     DEFAULT NULL, "\
            /*12*/                    "STEPID                 FIXED(6,0)  DEFAULT 0, "\
            /*13*/                    "OWNER                  CHAR(32),    "\
            /*14*/                    "SCHEMANAME             CHAR(32)    DEFAULT NULL, "\
            /*15*/                    "TABLENAME              CHAR(32),    "\
            /*16*/                    "TABLETYPE              CHAR(8 )     ASCII, "\
            /*17*/                    "CATALOGSTREAMNAME      CHAR(254)    DEFAULT NULL, "\
            /*18*/                    "CATALOGSTREAMTYPE      CHAR(32)     DEFAULT NULL, "\
            /*19*/                    "CATALOGSTREAMEBID      CHAR(32)     DEFAULT NULL, "\
            /*20*/                    "CATALOGSTREAMFORMAT    CHAR(32)     DEFAULT NULL, "\
            /*21*/                    "DATASTREAMNAME         CHAR(254)    DEFAULT NULL, "\
            /*22*/                    "DATASTREAMTYPE         CHAR(32)     DEFAULT NULL, "\
            /*23*/                    "DATASTREAMEBID         CHAR(32)     DEFAULT NULL, "\
            /*24*/                    "DATASTREAMFORMAT       CHAR(32)     DEFAULT NULL, "\
            /*25*/                    "DATASTREAMHEADERBLOCK  FIXED(6,0)   DEFAULT 0, "\
            /*26*/                    "DATASTREAMHEADERPOS    FIXED(10,0)  DEFAULT 0, "\
            /*27*/                    "DATASTREAMDATABLOCK    FIXED(6,0)   DEFAULT 0, "\
            /*28*/                    "DATASTREAMDATAPOS      FIXED(10,0)  DEFAULT 0, "\
            /*29*/                    "DATASTREAMTRAILERBLOCK FIXED(6,0)   DEFAULT 0, "\
            /*30*/                    "DATASTREAMTRAILERPOS   FIXED(10,0)  DEFAULT 0, "\
            /*31*/                    "EXCLUDE                BOOLEAN      DEFAULT FALSE, "\
            /*32*/                    "RESTART                BOOLEAN      DEFAULT TRUE, "\
            /*33*/                    "RESTORE                BOOLEAN      DEFAULT FALSE, "\
            /*34*/                    "PAGECOUNT              FIXED(10,0)  DEFAULT 0, "\
            /*35*/                    "ROWCOUNT               FIXED(10,0)  DEFAULT 0, "\
            /*36*/                    "STARTTIME              TIMESTAMP    DEFAULT NULL, "\
            /*37*/                    "ENDTIME                TIMESTAMP    DEFAULT NULL, "\
            /*38*/                    "REQUESTTIME            FIXED(15,3)  DEFAULT 0, "\
            /*39*/                    "RECEIVETIME            FIXED(15,3)  DEFAULT 0, "\
            /*40*/                    "IOTIME                 FIXED(15,3)  DEFAULT 0, "\
            /*41*/                    "LOGTIME                FIXED(15,3)  DEFAULT 0, "\
            /*42*/                    "SEQ_NO                 FIXED(6,0)   DEFAULT 0, "\
            /*43*/                    "CMD                    CHAR(254)    ASCII, "\
                                      "PRIMARY KEY ( PACKAGEGUID, TASKID, STEPID, SCHEMANAME, TABLENAME, SEQ_NO), "\
                                      "CONSTRAINT CODETYPE CHECK CODETYPE IS NULL OR CODETYPE IN ('ASCII', 'UCS2', 'UTF8' ), "\
                                      "CONSTRAINT STORAGEDEVICE  CHECK STORAGEDEVICE IS NULL OR STORAGEDEVICE IN ('NONE', 'ADSM', 'BACK', 'NSR') , "\
                                      "CONSTRAINT TRANSFORMATION CHECK TRANSFORMATION IS NULL OR TRANSFORMATION IN ('EXPORT' , 'IMPORT', 'EXTRACT', 'LOAD', 'UPDATE') , "\
                                      "CONSTRAINT PART CHECK PART IS NULL OR PART IN ('ALL', 'USER', 'TABLE') , "\
                                      "CONSTRAINT TABLETYPE CHECK TABLETYPE IS NULL OR TABLETYPE IN ('TABLE', 'VIEW', 'SYNONYM'), "\
                                      "CONSTRAINT CATALOGSTREAMTYPE CHECK CATALOGSTREAMTYPE IS NULL OR CATALOGSTREAMTYPE IN ('FILE', 'PIPE', 'TAPE'), "\
                                      "CONSTRAINT CATALOGSTREAMFORMAT CHECK CATALOGSTREAMFORMAT IS NULL OR CATALOGSTREAMFORMAT IN ('DDL', 'XML'), "\
                                      "CONSTRAINT DATASTREAMTYPE CHECK DATASTREAMTYPE IS NULL OR DATASTREAMTYPE IN ('FILE', 'PIPE', 'TAPE'), "\
                                      "CONSTRAINT DATASTREAMFORMAT CHECK DATASTREAMFORMAT IS NULL OR DATASTREAMFORMAT IN ('PAGES', 'RECORDS', 'COMPRESSED', 'FORMATTED') ) "\
                                      "INTERNAL")},
        {ddl_Transformation_301,   _T("ALTER  TABLE SYSLOADER.TRANSFORMATIONMODEL "\
                                      "ALTER  CONSTRAINT \"CODETYPE\"  CHECK \"CODETYPE\" IS NULL OR \"CODETYPE\"  IN ('ASCII','UCS2', 'UTF8') ")},
        {ddl_Transformation_302,   _T("CREATE DBPROC SYSLOADER.TRANSFORMATIONPACKAGE ( IN packageguid CHAR(24) BYTE )  "\
                                      "AS "\
                                      "    VAR  rc INTEGER; errmsg CHAR(80);"\
                                      "         task_id  FIXED(6,0);step_id  FIXED(6,0); temp_step_id FIXED(6,0); seq_no FIXED(6,0); "\
                                      "         owner CHAR(32); schemaname CHAR(32); tablename CHAR(32);"\
                                      "         step_id_ FIXED(6,0); "\
                                      "TRY "\
                                      "    SET    rc = 0;"\
                                      "    SET    task_id = 0; SET  step_id_ = 0; SET  temp_step_id = 0;"\
                                      "    SELECT MAX(TASKID)+1,\"STEPID\", \"SEQ_NO\", \"OWNER\", \"SCHEMANAME\", \"TABLENAME\" "\
                                      "           FROM SYSLOADER.TRANSFORMATIONMODEL "\
                                      "    WHERE  \"PACKAGEGUID\" = :packageguid "\
                                      "           GROUP BY \"STEPID\", \"SEQ_NO\", \"OWNER\", \"SCHEMANAME\", \"TABLENAME\" "\
                                      "           ORDER BY \"STEPID\", \"SEQ_NO\"  ASC ;"\
                                      "    WHILE $rc = 0 DO "\
                                      "          BEGIN "\
                                      "                FETCH INTO :task_id, :step_id, :seq_no, :owner, :schemaname, :tablename;"\
                                      "                      IF step_id = temp_step_id  THEN "\
                                      "                         SET step_id_ = step_id_ "\
                                      "                      ELSE "\
                                      "                         SET step_id_ = step_id_+1; "\
                                      "                      UPDATE SYSLOADER.TRANSFORMATIONMODEL "\
                                      "                      SET    \"TASKID\" = :task_id ,"\
                                      "                             \"STEPID\" = :step_id_,"\
                                      "                             \"SEQ_NO\" = :seq_no  "\
                                      "                      WHERE  \"PACKAGEGUID\" = :packageguid AND "\
                                      "                             \"OWNER\"       = :owner       AND \"SCHEMANAME\" = :schemaname AND \"TABLENAME\" = :tablename AND "\
                                      "                             \"STEPID\"      = :step_id     AND \"SEQ_NO\"     = :seq_no    AND  \"TASKID\"    = 0 ; "\
                                      "                SET temp_step_id = step_id;"\
                                      "          END; "\
                                      "CATCH "\
                                      "    IF $rc <> 100 THEN "\
                                      "       STOP ($rc, 'Unexpected error');")},
        {call_Transformation_303,  _T("CALL SYSLOADER.TRANSFORMATIONPACKAGE ( x'%s' )")},
        {ddl_Transformation_304,   _T("CREATE VIEW TRANSFORMATIONPACKAGES AS "\
            /*01*/                    "SELECT PACKAGEGUID, TASKID, STEPID, OWNER, SCHEMANAME, TABLENAME, EXCLUDE, RESTART "\
            /*02*/                    "FROM SYSLOADER.TRANSFORMATIONMODEL "\
                                      "WHERE TABLETYPE = \'TABLE\' AND CMD NOT LIKE 'GRANT %%' INTERNAL")},
        {ddl_Transformation_305,   _T("CREATE VIEW TRANSFORMATIONSTEPS AS "\
            /*01*/                    "SELECT TRANSFORMATION, OWNER, SCHEMANAME, TABLENAME, CATALOGSTREAMNAME, DATASTREAMNAME, DATASTREAMHEADERBLOCK, DATASTREAMHEADERPOS, DATASTREAMDATABLOCK, DATASTREAMDATAPOS, ROWCOUNT AS START "\
            /*02*/                    "FROM   SYSLOADER.TRANSFORMATIONMODEL "\
                                      "WHERE  TABLETYPE = \'TABLE\' AND CMD NOT LIKE 'GRANT %%' INTERNAL")},
        {ddl_Transformation_306,   _T("CREATE VIEW TRANSFORMATIONRESOURCES AS "\
            /*01*/                    "SELECT DISTINCT PACKAGEGUID, SERVERNODE, SERVERDB, USERNAME, CODEPAGEID, STORAGEDEVICE, CATALOGSTREAMNAME, CATALOGSTREAMTYPE, CATALOGSTREAMEBID, DATASTREAMNAME, DATASTREAMTYPE, DATASTREAMEBID "\
            /*02*/                    "FROM   SYSLOADER.TRANSFORMATIONMODEL INTERNAL")},
        {ddl_Transformation_307,   _T("CREATE VIEW TRANSFORMATIONSTATISTIC AS "\
                                      "SELECT PACKAGEGUID, STEPID, OWNER, SCHEMANAME, TABLENAME, "\
                                      "DATASTREAMTRAILERPOS-DATASTREAMHEADERPOS AS BYTECOUNT, ROWCOUNT, TRANSACTIONSIZE, "\
                                      "(HOUR         (ENDTIME) - HOUR        (STARTTIME))  *3600000 + "\
                                      "(MINUTE       (ENDTIME) - MINUTE      (STARTTIME))    *60000 + "\
                                      "(SECOND       (ENDTIME) - SECOND      (STARTTIME))     *1000 + "\
                                      "((MICROSECOND (ENDTIME) - MICROSECOND (STARTTIME))    /1000)  AS SYSTEMTIME, "\
                                      "(REQUESTTIME+RECEIVETIME) *1000 AS KERNELTIME, "\
                                      "(HOUR         (ENDTIME) - HOUR        (STARTTIME))  *3600000 +  "\
                                      "(MINUTE       (ENDTIME) - MINUTE      (STARTTIME))    *60000 +  "\
                                      "(SECOND       (ENDTIME) - SECOND      (STARTTIME))     *1000 +  "\
                                      "((MICROSECOND (ENDTIME) - MICROSECOND (STARTTIME))     /1000) - "\
                                      "(REQUESTTIME+RECEIVETIME)*1000 AS LOADERTIME, "\
                                      "(HOUR         (ENDTIME) - HOUR        (STARTTIME))  *3600000 +  "\
                                      "(MINUTE       (ENDTIME) - MINUTE      (STARTTIME))    *60000 +  "\
                                      "(SECOND       (ENDTIME) - SECOND      (STARTTIME))     *1000 +  "\
                                      "((MICROSECOND (ENDTIME) - MICROSECOND (STARTTIME))     /1000) - "\
                                      "(REQUESTTIME+RECEIVETIME)*1000 - (IOTIME) *1000 - (LOGTIME) *1000 AS DATATIME, "\
                                      "(LOGTIME)*1000 AS LOGTIME, "\
                                      "(IOTIME) *1000 AS IOTIME "\
                                      "FROM SYSLOADER.TRANSFORMATIONMODEL"\
                                      "WHERE EXCLUDE = FALSE AND RESTART = FALSE AND TABLETYPE = 'TABLE' AND ( CMD LIKE 'TABLE %%' OR CMD IS NULL ) "\
                                      "GROUP BY PACKAGEGUID, STEPID, OWNER, SCHEMANAME, TABLENAME, "\
                                      "DATASTREAMTRAILERPOS, DATASTREAMHEADERPOS, ROWCOUNT, TRANSACTIONSIZE, "\
                                      "ENDTIME, STARTTIME, REQUESTTIME, RECEIVETIME, IOTIME, LOGTIME INTERNAL" ) },
        {ddl_Transformation_308,   _T("CREATE VIEW TRANSFORMATIONMONITOR AS "\
        /*01*/                        "SELECT PACKAGEGUID,COUNT(*) AS TABLECOUNT, SUM(ROWCOUNT) AS ROWCOUNT, SUM(SYSTEMTIME) AS SYSTEMDURATION, SUM(KERNELTIME) AS KERNELDURATION, SUM(LOADERTIME) AS LOADERDURATION, SUM(DATATIME) AS DATADURATION, SUM(LOGTIME) AS LOGDURATION, SUM(IOTIME) AS IODURATION "\
        /*02*/                        "FROM   SYSLOADER.TRANSFORMATIONSTATISTIC  "\
        /*03*/                        "GROUP  BY PACKAGEGUID INTERNAL ")},
        {ddl_Transformation_309,   _T("CREATE TABLE SYSLOADER.CODEPAGE "\
                                      "(CODEPAGEID    CHAR (32)  NOT NULL,"\
                                      "CODEPAGETABLE CHAR (512) BYTE NOT NULL,"\
                                      "PRIMARY KEY (CODEPAGEID)) INTERNAL")},
        {ddl_Transformation_310,   _T("ALTER TABLE SYSLOADER.TRANSFORMATIONMODEL MODIFY TRANSACTIONSIZE   FIXED(10,0) ")},
        {ddl_Transformation_311,   _T("ALTER TABLE SYSLOADER.TRANSFORMATIONMODEL MODIFY REQUESTTIME       FIXED(15,3) ")},
        {ddl_Transformation_312,   _T("ALTER TABLE SYSLOADER.TRANSFORMATIONMODEL MODIFY RECEIVETIME       FIXED(15,3) ")},                             
        {ddl_Transformation_313,   _T("ALTER TABLE SYSLOADER.TRANSFORMATIONMODEL MODIFY IOTIME            FIXED(15,3) ")},    
        {ddl_Transformation_314,   _T("ALTER TABLE SYSLOADER.TRANSFORMATIONMODEL MODIFY LOGTIME           FIXED(15,3) ")},    
        {ddl_Transformation_315,   _T("ALTER TABLE SYSLOADER.TRANSFORMATIONMODEL "\
                                      "ALTER CONSTRAINT \"TRANSFORMATION\"      CHECK \"TRANSFORMATION\"  IS NULL OR \"TRANSFORMATION\"  IN ('EXPORT', 'IMPORT', 'EXTRACT', 'LOAD', 'UPDATE' )" )},
        {ddl_Transformation_316,   _T("ALTER  TABLE SYSLOADER.TRANSFORMATIONMODEL "\
                                      "ALTER  CONSTRAINT \"PACKAGEPART\"       CHECK \"PACKAGEPART\"     IS NULL OR \"PACKAGEPART\"     IN ('ALL'   , 'DB', 'USER', 'SCHEMA', 'TABLE' )" ) },
        {trans_Transformation_316, _T("LOCK  TABLE SYSLOADER.CODEPAGE IN EXCLUSIVE MODE") },
        {trans_Transformation_317, _T("TRANSLATE SCHEMA USING x'%s' ")},
        {trans_Transformation_318, _T("TRANSLATE SCHEMA OFF ")},
        {dal_Transformation_400,   _T("GRANT ALTER,INSERT,UPDATE,DELETE,SELECT ON SYSLOADER.TRANSFORMATIONMODEL TO PUBLIC")},
        {dal_Transformation_401,   _T("GRANT SELECT  ON TRANSFORMATIONPACKAGES  TO PUBLIC ")},
        {dal_Transformation_402,   _T("GRANT EXECUTE ON SYSLOADER.TRANSFORMATIONPACKAGE TO PUBLIC")},
        {dal_Transformation_403,   _T("GRANT SELECT  ON TRANSFORMATIONSTEPS     TO PUBLIC ")},
        {dal_Transformation_404 ,  _T("GRANT SELECT  ON TRANSFORMATIONRESOURCES TO PUBLIC ")},
        {dal_Transformation_405,   _T("GRANT SELECT  ON TRANSFORMATIONSTATISTIC TO PUBLIC ")},
        {dal_Transformation_406,   _T("GRANT SELECT  ON TRANSFORMATIONMONITOR   TO PUBLIC ")},
        {dal_Transformation_407,   _T("GRANT SELECT,DELETE ON TEMP.LOA_SAVE_CATALOG_LS00 TO PUBLIC ")},
        {dal_Transformation_408,   _T("GRANT ALL ON SYSLOADER.CODEPAGE TO PUBLIC")},
        {ins_Transformation_500,   _T("INSERT INTO SYSLOADER.TRANSFORMATIONMODEL ( PACKAGEGUID, STEPID  ,   CMD,    SEQ_NO,   OWNER,   SCHEMANAME,   TABLENAME,   TABLETYPE ) ( "\
                                      "SELECT                                      x'%s'     ,  C.CMD_NO, C.CMD,  C.SEQ_NO, C.OWNER, C.SCHEMA    , C.TABLENAME, C.TABLETYPE "\
                                      "FROM  TEMP.LOA_SAVE_CATALOG_LS00 C, DOMAIN.USERS U  WHERE   "\
                                      "U.ISREPLICATIONUSER NOT IN ('YES')      AND "\
                                      "C.OWNER         =  U.USERNAME               "\
                                      " %s                                     AND "\
                                      "C.SCHEMA    NOT IN   ('PUBLIC','SYS','SYSLOADER')   AND "\
                                      "C.CMD_NO        IN   ( SELECT C.CMD_NO  FROM TEMP.LOA_SAVE_CATALOG_LS00 C WHERE "\ 
                                      "                            ( C.CMD NOT LIKE 'GRANT %%'     AND C.TABLETYPE IN  ( 'TABLE','VIEW','SYNONYM' ) AND C.SEQ_NO = 0 ) OR  "\
                                      "                            ( C.CMD     LIKE 'GRANT %%' ) ) AND "\
                                      "C.TABLENAME NOT IN   ('SYSUPGRADEHISTORY','POSTUPGRADECHECKLIST','POSTUPGRADECHECKLISTUNICODE') ) ")},
        {ins_Transformation_501,   _T("INSERT INTO SYSLOADER.TRANSFORMATIONMODEL ( PACKAGEGUID, STEPID, CMD, SEQ_NO, OWNER, SCHEMANAME, TABLENAME, TABLETYPE ) "\
                                      "(SELECT x'%s' , CMD_NO, CMD, SEQ_NO, OWNER, SCHEMA, TABLENAME, TABLETYPE "\
                                      "FROM TEMP.LOA_SAVE_CATALOG_LS00 "\
                                      "WHERE NOT (TABLETYPE IN ('VIEW', 'SYNONYM') AND SEQ_NO > 0) )")},
        {ins_Transformation_502,   _T("SAVE CATALOG ALL  INTO TEMP.LOA_SAVE_CATALOG_LS00")},
        {ins_Transformation_503,   _T("SAVE CATALOG USER INTO TEMP.LOA_SAVE_CATALOG_LS00")},
        {ins_Transformation_504,   _T("IMPORT COLUMNS TABLE SYSLOADER.TRANSFORMATIONMODEL  "\
                                      "SCHEMANAME             1  TABLENAME           2  "\
                                      "DATASTREAMHEADERBLOCK  3  DATASTREAMHEADERPOS 4 "\
                                      "ROWCOUNT 6 "\
                                      "INSTREAM '%s' COMPRESSED DELIMITER '\"' SEPARATOR ',' ")},
        {ins_Transformation_505,   _T("INSERT INTO SYSLOADER.CODEPAGE VALUES('%s', x'%.*s')")},
        {ins_Transformation_506,   _T("SAVE CATALOG OF SCHEMA \"%s\" INTO TEMP.LOA_SAVE_CATALOG_LS00")},
        {ins_Transformation_507,   _T("INSERT INTO SYSLOADER.MAPSCHEMANAMES VALUES (x'%s','%s','%s','%s') ")},
        {del_Transformation_600,   _T("DELETE FROM SYSLOADER.TRANSFORMATIONMODEL WHERE PACKAGEGUID = x'%s'")},
        {del_Transformation_601,   _T("DELETE FROM SYSLOADER.TRANSFORMATIONMODEL WHERE PACKAGEGUID = x'%s' AND CMD NOT LIKE 'TABLE %%' ")},
        {del_Transformation_602,   _T("DELETE FROM SYSLOADER.TRANSFORMATIONMODEL WHERE EXCLUDE = TRUE OR ROWCOUNT = 0  ")},
        {del_Transformation_603,   _T("DELETE FROM SYSLOADER.TRANSFORMATIONMODEL WHERE RESTART = FALSE ")},
        {del_Transformation_604,   _T("DELETE FROM TEMP.LOA_SAVE_CATALOG_LS00 "\
                                      "WHERE TYPE = 'SYSTEM' OR OWNER IN ( 'SYS','PUBLIC' ) OR OWNER = SYSDBA ")},
        {del_Transformation_605,   _T("DELETE FROM SYSLOADER.MAPSCHEMANAMES WHERE %s MAPPINGNAME = '%s' ")},
        {sel_Transformation_700,   _T("SELECT COUNT (*) FROM "\
                                      "SYSLOADER.TRANSFORMATIONMODEL "\
                                      "WHERE CMD NOT LIKE 'GRANT %%' "\
                                      "AND TABLETYPE = 'TABLE' AND STEPID > 0 AND EXCLUDE = %s AND  "\
                                      "PACKAGEGUID   = x'%s'")},
        {sel_Transformation_701,   _T("SELECT MAX(TASKID) FROM SYSLOADER.TRANSFORMATIONMODEL")},
        {sel_Transformation_702,   _T("SELECT STEPID, OWNER, SCHEMANAME, TABLENAME, TABLETYPE, CMD, MAX(TASKID) AS TASKID "\
                                      "FROM SYSLOADER.TRANSFORMATIONMODEL "\
                                      "WHERE PACKAGEGUID = x'%s' AND OWNER NOT IN ('SYS','PUBLIC') %s "\
                                      "GROUP BY STEPID, OWNER, SCHEMANAME, TABLENAME, TABLETYPE, CMD "\
                                      "ORDER BY %s STEPID, SEQ_NO ASC ")},
        {sel_Transformation_703,   _T("SELECT STEPID, OWNER, SCHEMANAME, TABLENAME, TABLETYPE, CMD, MAX(TASKID) AS TASKID "\
                                      "FROM SYSLOADER.TRANSFORMATIONMODEL "\
                                      "WHERE PACKAGEGUID = x'%s' AND OWNER NOT IN ('SYS','PUBLIC') AND TABLETYPE IN ('TABLE') %s "\
                                      "GROUP BY STEPID, OWNER, SCHEMANAME, TABLENAME, TABLETYPE, CMD "\
                                      "ORDER BY %s STEPID, SEQ_NO ASC ")},
        {sel_Transformation_704,   _T("SELECT STEPID, OWNER, SCHEMANAME, TABLENAME, TABLETYPE, CMD "\
                                      "FROM SYSLOADER.TRANSFORMATIONMODEL "\
                                      "WHERE PACKAGEGUID = x'%s' AND OWNER NOT IN ('SYS','PUBLIC') AND TABLETYPE IN ('VIEW','SYNONYM') "\
                                      "GROUP BY STEPID, OWNER, SCHEMANAME, TABLENAME, TABLETYPE, CMD "\
                                      "ORDER BY STEPID, SEQ_NO ASC ")},
        {sel_Transformation_705,   _T("SELECT STEPID, OWNER, SCHEMANAME, TABLENAME "\
                                      "FROM SYSLOADER.TRANSFORMATIONMODEL "\
                                      "WHERE PACKAGEGUID = x'%s' AND OWNER NOT IN ('SYS','PUBLIC') AND TABLETYPE IN ('TABLE')  AND CMD LIKE '%%FOREIGN%%' "\
                                      "GROUP BY STEPID, OWNER, SCHEMANAME, TABLENAME "\
                                      "ORDER BY STEPID, SEQ_NO ASC ")},
        {sel_Transformation_706,   _T("SELECT DISTINCT 0, OWNER, SCHEMANAME "\
                                      "FROM   SYSLOADER.TRANSFORMATIONMODEL "\
                                      "WHERE  PACKAGEGUID = x'%s' AND SCHEMANAME NOT IN ('SYS','PUBLIC')")},
        {sel_Transformation_707,   _T("SELECT DISTINCT 0, OWNER, SCHEMANAME FROM SYSLOADER.TRANSFORMATIONMODEL "\
                                      "WHERE  PACKAGEGUID = x'%s' AND SCHEMANAME NOT IN ('SYS','PUBLIC') AND OWNER = '%s'" )},
        {sel_Transformation_708,   _T("SELECT "\
                                      "TRANSFORMATION, OWNER, SCHEMANAME, TABLENAME,  "\
                                      "DATASTREAMTYPE, DATASTREAMNAME, DATASTREAMFORMAT, CODETYPE, DATASTREAMEBID, "\
                                      "DATASTREAMHEADERBLOCK, DATASTREAMHEADERPOS, DATASTREAMDATABLOCK, DATASTREAMDATAPOS, DATASTREAMTRAILERBLOCK, DATASTREAMTRAILERPOS, "\
                                      "PAGECOUNT, ROWCOUNT, EXCLUDE, STEPID  "\
                                      "FROM SYSLOADER.TRANSFORMATIONMODEL "\
                                      "WHERE PACKAGEGUID = x'%s' AND EXCLUDE = FALSE AND STEPID > 0 AND RESTART = TRUE AND TABLETYPE = 'TABLE' AND CMD LIKE 'TABLE %%'"\
                                      "GROUP BY "\
                                      "TRANSFORMATION, OWNER, SCHEMANAME, TABLENAME, "\
                                      "DATASTREAMTYPE, DATASTREAMNAME, DATASTREAMFORMAT, CODETYPE, DATASTREAMEBID, DATASTREAMHEADERBLOCK, DATASTREAMHEADERPOS, DATASTREAMDATABLOCK, DATASTREAMDATAPOS, DATASTREAMTRAILERBLOCK, DATASTREAMTRAILERPOS, "\
                                      "PAGECOUNT, ROWCOUNT, EXCLUDE, STEPID "\
                                      "ORDER BY  STEPID, SEQ_NO  ASC " )},
        {sel_Transformation_709,   _T("SELECT COUNT (*) FROM SYSLOADER.TRANSFORMATIONMODEL "\
                                      "WHERE PACKAGEGUID = x'%s' AND STEPID > 0 ")},
        {sel_Transformation_710,   _T("SELECT COUNT (*) FROM SYSLOADER.TRANSFORMATIONMODEL "\
                                      "WHERE PACKAGEGUID = x'%s' AND EXCLUDE = FALSE AND STEPID > 0 AND CMD LIKE 'TABLE %%' AND TABLETYPE = 'TABLE'   ")},
        {sel_Transformation_711,   _T("SELECT COUNT (*) FROM SYSLOADER.TRANSFORMATIONMODEL "\
                                      "WHERE PACKAGEGUID = x'%s' AND STEPID > 0 AND CMD LIKE 'CREATE VIEW %%' " )},
        {sel_Transformation_712,   _T("SELECT COUNT (*) FROM SYSLOADER.TRANSFORMATIONMODEL "\
                                      "WHERE PACKAGEGUID = x'%s' AND STEPID > 0 AND CMD LIKE 'CREATE SYNONYM %%' ")},
        {sel_Transformation_713,   _T("SELECT COUNT(*) FROM \"%s\".\"%s\" ") },
        {sel_Transformation_714,   _T("EXPORT COLUMNS "\
            /*01*/                    "PACKAGEGUID          , TASKID              , SERVERNODE             , SERVERDB            , "\
            /*05*/                    "CODETYPE             , USERNAME            , CODEPAGEID             , STORAGEDEVICE       , "\
            /*09*/                    "TRANSACTIONSIZE      , TRANSFORMATION      , PART                   , STEPID              , "\
            /*13*/                    "OWNER                , SCHEMANAME          , TABLENAME              , TABLETYPE           , "\
                                      "CATALOGSTREAMNAME    , CATALOGSTREAMTYPE   , CATALOGSTREAMEBID      , CATALOGSTREAMFORMAT , "\
            /*17*/                    "DATASTREAMNAME       , DATASTREAMTYPE      , DATASTREAMEBID         , DATASTREAMFORMAT    , "\
            /*21*/                    "DATASTREAMHEADERBLOCK, DATASTREAMHEADERPOS , DATASTREAMTRAILERBLOCK , DATASTREAMTRAILERPOS, DATASTREAMDATABLOCK, DATASTREAMDATAPOS, "\
            /*25*/                    "\"EXCLUDE\"          , \"RESTART\"         , RESTORE                , PAGECOUNT           , "\
            /*29*/                    "ROWCOUNT             , STARTTIME           , ENDTIME                , REQUESTTIME         , "\
            /*33*/                    "RECEIVETIME          , IOTIME              , LOGTIME               "\
                                      "FROM SYSLOADER.TRANSFORMATIONMODEL "\
                                      "WHERE PACKAGEGUID = x'%s'  AND STEPID > 0 AND TABLETYPE = \'TABLE\' AND CMD NOT LIKE 'GRANT %%' ORDER BY ABS(STEPID) ASC WITH LOCK ISOLATION LEVEL 0 "\
                                      "OUTFIELDS "\
                                      "\"OWNER\"                 1  \"SCHEMANAME\"           2 \"TABLENAME\"               3 "\
                                      "\"DATASTREAMNAME\"        4 "\
                                      "\"DATASTREAMHEADERBLOCK\" 5  \"DATASTREAMHEADERPOS\"  6 \"DATASTREAMTRAILERBLOCK\"  7 \"DATASTREAMTRAILERPOS\"  8  \"DATASTREAMDATABLOCK\"  9 \"DATASTREAMDATAPOS\"  10  "\
                                      "\"EXCLUDE\"               11 "\
                                      "\"PACKAGEGUID\"           12 \"TASKID\"              13 \"STEPID\"                 14 \"SERVERNODE\"           15 "\
                                      "\"SERVERDB\"              16 \"USERNAME\"            17 \"CODETYPE\"               18 \"CODEPAGEID\"           19 "\
                                      "\"STORAGEDEVICE\"         20 \"TRANSACTIONSIZE\"     21 \"TRANSFORMATION\"         22 \"PART\"                 23 \"TABLETYPE\"            24 "\
                                      "\"CATALOGSTREAMNAME\"     25 \"CATALOGSTREAMTYPE\"   26 \"CATALOGSTREAMEBID\"      27 \"CATALOGSTREAMFORMAT\"  28 \"DATASTREAMTYPE\"       29 \"DATASTREAMEBID\"     30 "\
                                      "\"DATASTREAMFORMAT\"      31 \"RESTART\"             32 \"RESTORE\"                33 \"PAGECOUNT\"            34 "\
                                      "\"ROWCOUNT\"              35 \"STARTTIME\"           36 \"ENDTIME\"                37 \"REQUESTTIME\"          38 "\
                                      "\"RECEIVETIME\"           39 \"IOTIME\"              40 \"LOGTIME\"                41 "\
                                      "OUTSTREAM FILE '%s' COMPRESSED %s %s INTERNAL" )},
        {sel_Transformation_715,   _T("EXPORT COLUMNS "\
                                      "ROWNO, SCHEMANAME, TABLENAME ,  "\
                                      "CATALOGSTREAMNAME, CATALOGSTREAMTYPE, CATALOGSTREAMEBID, CATALOGSTREAMFORMAT, "\
                                      "DATASTREAMNAME   , DATASTREAMTYPE,    DATASTREAMEBID,    DATASTREAMFORMAT,        CODETYPE, "\
                                      "DATASTREAMHEADERBLOCK, DATASTREAMHEADERPOS , DATASTREAMTRAILERBLOCK , DATASTREAMTRAILERPOS, DATASTREAMDATABLOCK, DATASTREAMDATAPOS "\
                                      "FROM SYSLOADER.TRANSFORMATIONMODEL "\
                                      "WHERE PACKAGEGUID = x'%s'  AND STEPID > 0 AND EXCLUDE = FALSE AND TABLETYPE = \'TABLE\' AND CMD NOT LIKE 'GRANT %%' ORDER BY ABS(STEPID) ASC WITH LOCK ISOLATION LEVEL 0 "\
                                      "OUTFIELDS "\
                                      "\"SCHEMANAME\"            1  \"TABLENAME\"            2    ROWNO                      3 "\
                                      "\"CATALOGSTREAMNAME\"     4  \"CATALOGSTREAMTYPE\"    5  \"CATALOGSTREAMEBID\"        6 \"CATALOGSTREAMFORMAT\"    7 "\
                                      "\"DATASTREAMNAME\"        8  \"DATASTREAMTYPE\"       9  \"DATASTREAMEBID\"          10 \"DATASTREAMFORMAT\"      11  \"CODETYPE\"                12 "\
                                      "\"DATASTREAMHEADERBLOCK\" 13 \"DATASTREAMHEADERPOS\"  14 \"DATASTREAMDATABLOCK\"     15 \"DATASTREAMDATAPOS\"     16  \"DATASTREAMTRAILERBLOCK\"  17 \"DATASTREAMTRAILERPOS\"  18 "\
                                      "OUTSTREAM %s '%s' COMPRESSED INTERNAL" )
        },// upd_Transformation_809
        {sel_Transformation_716,   _T("SELECT CODEPAGETABLE "\
                                      "FROM SYSLOADER.CODEPAGE  "\
                                      "WHERE CODEPAGEID = '%s'")},
        {sel_Transformation_717,   _T("SELECT COUNT(*) "\
                                      "FROM SYSLOADER.TRANSFORMATIONPACKAGES "\
                                      "WHERE PACKAGEGUID = ")},
        {sel_Transformation_718,   _T("SELECT COUNT(*) "\
                                      "FROM SYSLOADER.TRANSFORMATIONPACKAGES "\
                                      "WHERE RESTART = FALSE AND PACKAGEGUID = ")},
        {sel_Transformation_719,   _T("SELECT COUNT(*)     "\
                                      "FROM SYSLOADER.TRANSFORMATIONMODEL T , DOMAIN.INDEXCOLUMNS IC , DOMAIN.FOREIGNKEYCOLUMNS FK "\
                                      "WHERE PACKAGEGUID = x'%s' AND "\
                                      "T.CMD LIKE 'TABLE %%' AND T.CMD LIKE '%%INDEX%%' AND  ( T.OWNER NOT IN ('SYSINFO') OR T.OWNER <> SYSDBA ) AND "\
                                      "IC.SCHEMANAME    = T.SCHEMANAME AND IC.OWNER    = T.OWNER AND IC.TABLENAME    = T.TABLENAME AND "\
                                      "FK.REFSCHEMANAME = T.SCHEMANAME AND FK.REFOWNER = T.OWNER AND FK.REFTABLENAME = T.TABLENAME AND "\
                                      "FK.REFCOLUMNNAME = IC.COLUMNNAME ")},
        {sel_Transformation_720,   _T("SELECT DISTINCT 0, OWNER, SCHEMANAME FROM SYSLOADER.TRANSFORMATIONMODEL "\
                                      "WHERE  PACKAGEGUID = x'%s' AND OWNER NOT IN ('SYS','PUBLIC') AND SCHEMANAME = '%s'" )},
        {sel_Transformation_721,   _T("SELECT DISTINCT 0, T.OWNER, T.SCHEMANAME, P.GRANTOR, P.GRANTEE, P.CREATEIN, P.DROPIN,P.ALTERIN, P.GRANTOPTION FROM SYSLOADER.TRANSFORMATIONMODEL T, DOMAIN.SCHEMAPRIVILEGES P "\
                                      "WHERE    T.PACKAGEGUID = x'%s' AND T.OWNER NOT IN ('SYS','PUBLIC') AND T.SCHEMANAME = P.SCHEMANAME AND T.SCHEMANAME <> T.OWNER %s '%s' "\
                                      "GROUP BY T.OWNER, T.SCHEMANAME, P.GRANTOR, P.GRANTEE, P.CREATEIN, P.DROPIN,P.ALTERIN, P.GRANTOPTION" )},
        {sel_Transformation_722,   _T(" SELECT PACKAGEGUID, MAPPINGNAME, SOURCESCHEMANAME, TARGETSCHEMANAME FROM SYSLOADER.MAPSCHEMANAMES O WHERE EXISTS "\
                                      "(SELECT PACKAGEGUID, MAPPINGNAME, SOURCESCHEMANAME, TARGETSCHEMANAME FROM SYSLOADER.MAPSCHEMANAMES I WHERE "\
                                      " I.PACKAGEGUID  = x'%s' AND O.PACKAGEGUID  = x'%s' AND "\
                                      " I.MAPPINGNAME  =  '%s' AND O.MAPPINGNAME  =  '%s' AND "\
                                      " I.SOURCESCHEMANAME  = O.SOURCESCHEMANAME AND "\
                                      " I.TARGETSCHEMANAME <> O.TARGETSCHEMANAME ) ")},
        {sel_Transformation_723,   _T("SELECT PACKAGEGUID, MAPPINGNAME, SOURCESCHEMANAME, TARGETSCHEMANAME FROM SYSLOADER.MAPSCHEMANAMES WHERE %s "\
                                      "       MAPPINGNAME        =  '%s'                ")},
        {sel_Transformation_724,   _T("SELECT "\
                                      "TRANSFORMATION, OWNER, VALUE(SYSLOADER.MAPSCHEMANAMES.TARGETSCHEMANAME,SYSLOADER.TRANSFORMATIONMODEL.SCHEMANAME), TABLENAME,  "\
                                      "DATASTREAMTYPE, DATASTREAMNAME, DATASTREAMFORMAT, CODETYPE, DATASTREAMEBID, "\
                                      "DATASTREAMHEADERBLOCK, DATASTREAMHEADERPOS, DATASTREAMDATABLOCK, DATASTREAMDATAPOS, DATASTREAMTRAILERBLOCK, DATASTREAMTRAILERPOS, "\
                                      "PAGECOUNT, ROWCOUNT, EXCLUDE, STEPID  "\
                                      "FROM     SYSLOADER.TRANSFORMATIONMODEL LEFT OUTER JOIN SYSLOADER.MAPSCHEMANAMES "\
                                      "ON       SYSLOADER.TRANSFORMATIONMODEL.SCHEMANAME    = SYSLOADER.MAPSCHEMANAMES.SOURCESCHEMANAME %s "\
                                      "WHERE    SYSLOADER.TRANSFORMATIONMODEL.PACKAGEGUID   = x'%s' %s "\
                                      "AND EXCLUDE = FALSE AND STEPID > 0 AND RESTART = TRUE AND TABLETYPE = 'TABLE' AND CMD LIKE 'TABLE %%' "\
                                      "GROUP BY "\
                                      "TRANSFORMATION, OWNER, VALUE(SYSLOADER.MAPSCHEMANAMES.TARGETSCHEMANAME,SYSLOADER.TRANSFORMATIONMODEL.SCHEMANAME), TABLENAME, "\
                                      "DATASTREAMTYPE, DATASTREAMNAME, DATASTREAMFORMAT, CODETYPE, DATASTREAMEBID, DATASTREAMHEADERBLOCK, DATASTREAMHEADERPOS, DATASTREAMDATABLOCK, DATASTREAMDATAPOS, DATASTREAMTRAILERBLOCK, DATASTREAMTRAILERPOS, "\
                                      "PAGECOUNT, ROWCOUNT, EXCLUDE, STEPID "\
                                      "ORDER BY  STEPID, SEQ_NO  ASC " )},
        {stmt_EndOfArray_925,      _T("Unknown Statement - please report to SAP Basis Entwicklung Berlin\n")}
    };

    static StatementStruct StatementDesc_SysDba[] =
    {
        {upd_Transformation_800,   _T("UPDATE SYSDBA.TRANSFORMATIONMODEL ") },
        {upd_Transformation_801,   _T("UPDATE SYSDBA.TRANSFORMATIONMODEL "\
                                      "SET    EXCLUDE     =    %s  "\
                                      "WHERE  PACKAGEGUID =  x'%s' ")},
        {upd_Transformation_802,   _T("UPDATE SYSDBA.TRANSFORMATIONMODEL "\
                                      "SET   CATALOGSTREAMFORMAT = 'DDL'      "\
                                      "WHERE PACKAGEGUID =  x'%s'   AND \"OWNER\" = '%s' AND \"TABLENAME\" = '%s'  AND TABLETYPE = '%s'")},
        {upd_Transformation_803,   _T("UPDATE SYSDBA.TRANSFORMATIONMODEL "\
                                      "SET DATASTREAMNAME = '%s' "\
                                      "WHERE PACKAGEGUID  =  x'%s'  AND \"OWNER\" = '%s' AND \"TABLENAME\" = '%s'  AND TABLETYPE IN ('TABLE') ")},
        {upd_Transformation_804,   _T("UPDATE SYSDBA.TRANSFORMATIONMODEL "\
                                      "SET   \"STARTTIME\" = TIMESTAMP "\
                                      "WHERE PACKAGEGUID   =  x'%s' AND \"OWNER\" = '%s' AND \"TABLENAME\" = '%s'  AND TABLETYPE IN ('TABLE') ")},
        {upd_Transformation_805,   _T("UPDATE SYSDBA.TRANSFORMATIONMODEL "\
                                      "SET   \"ENDTIME\"   =  TIMESTAMP "\
                                      "WHERE PACKAGEGUID   =  x'%s' AND \"OWNER\" = '%s' AND \"TABLENAME\" = '%s'  AND TABLETYPE IN ('TABLE') ")},
        {upd_Transformation_806,   _T("UPDATE SYSDBA.TRANSFORMATIONMODEL  "\
                                      "SET   \"ROWCOUNT\"              = %s, "\
                                      "      \"DATASTREAMHEADERBLOCK\" = %s, "\
                                      "      \"DATASTREAMHEADERPOS\"   = %s  "\
                                      "WHERE PACKAGEGUID      =  x'%s' AND \"OWNER\" = '%s' AND \"TABLENAME\" = '%s' AND TABLETYPE IN ('TABLE') ")},
        {upd_Transformation_807,   _T("UPDATE SYSDBA.TRANSFORMATIONMODEL "\
                                      "SET "\
                                      "\"ROWCOUNT\" = %s, "\
                                      "\"DATASTREAMDATABLOCK\" = %s, "\
                                      "\"DATASTREAMDATAPOS\"   = %s  "\
                                      "WHERE \"PACKAGEGUID\"   = x'%s' AND \"OWNER\" = '%s' AND \"TABLENAME\" = '%s' AND TABLETYPE IN ('TABLE') ")},
        {upd_Transformation_808,   _T("UPDATE SYSDBA.TRANSFORMATIONMODEL "\
                                      "SET   \"RESTART\"       = FALSE "\
                                      "WHERE \"PACKAGEGUID\"   = x'%s' AND \"OWNER\" = '%s' AND \"TABLENAME\" = '%s' AND TABLETYPE IN ('TABLE') ")},
        {upd_Transformation_809,   _T("UPDATE DATA TABLE "\
                                      "SYSDBA.TRANSFORMATIONMODEL "\
                                      "    \"PACKAGEGUID\"           '%s' %s   \"SUBSTR(CMD,1,5)\"  'TABLE'                              \"TABLENAME\"               2              SET \"STEPID\"                  3 "\
                                      "SET \"CATALOGSTREAMNAME\"       4  DEFAULT NULL SET \"CATALOGSTREAMTYPE\"     5  DEFAULT NULL SET \"CATALOGSTREAMEBID\"       6 DEFAULT NULL SET \"CATALOGSTREAMFORMAT\"    7 DEFAULT NULL "\
                                      "SET \"DATASTREAMNAME\"          8  DEFAULT NULL SET \"DATASTREAMTYPE\"        9  DEFAULT NULL SET \"DATASTREAMEBID\"         10 DEFAULT NULL SET \"DATASTREAMFORMAT\"      11 DEFAULT NULL SET \"CODETYPE\"               12 DEFAULT NULL "\
                                      "SET \"DATASTREAMHEADERBLOCK\"  13  DEFAULT NULL SET \"DATASTREAMHEADERPOS\"  14  DEFAULT NULL SET \"DATASTREAMDATABLOCK\"    15 DEFAULT NULL SET \"DATASTREAMDATAPOS\"     16 DEFAULT NULL SET \"DATASTREAMTRAILERBLOCK\" 17 DEFAULT NULL SET \"DATASTREAMTRAILERPOS\"  18 DEFAULT NULL  "\
                                      "INSTREAM %s %s '%s' COMPRESSED INTERNAL")
        },//sel_Transformation_715, pszPackageQualifier_ls00
        {upd_Transformation_810,   _T("UPDATE DATA TABLE "\
                                      "SYSDBA.TRANSFORMATIONMODEL "\
                                      "    \"PACKAGEGUID\"           '%s'   \"SUBSTR(CMD,1,5)\"  'TABLE'  \"OWNER\"  1 \"TABLENAME\" 2 "\
                                      "SET \"EXCLUDE\"                 2  NULL IF POS 2 <> \'_\'  "\
                                      "INSTREAM FILE '%s' COMPRESSED %s INTERNAL")},
        {upd_Transformation_811,   _T("UPDATE SYSDBA.TRANSFORMATIONMODEL "\
                                      "SET   \"EXCLUDE\"     = TRUE  "\
                                      "WHERE \"PACKAGEGUID\" = x'%s' AND \"OWNER\" = '%s' AND \"TABLENAME\" = '%s' ")},
        {upd_Transformation_812,   _T("UPDATE SYSDBA.TRANSFORMATIONMODEL "\
                                      "SET   \"REQUESTTIME\" = %s , \"RECEIVETIME\" = %s , \"IOTIME\" = %s, \"LOGTIME\" = %s "\
                                      "WHERE \"PACKAGEGUID\" = x'%s'  AND \"OWNER\" = '%s' AND \"TABLENAME\" = '%s' ")},
        {upd_Transformation_813,   _T("UPDATE SYSDBA.TRANSFORMATIONMODEL "\
                                      "SET "\
                                      "DATASTREAMTRAILERBLOCK  = %s, "\
                                      "DATASTREAMTRAILERPOS    = %s  "\
                                      "WHERE \"PACKAGEGUID\"   = x'%s'  AND \"OWNER\" = '%s' AND \"TABLENAME\" = '%s' AND TABLETYPE IN('TABLE') ")},
        {sel_Relational_1,         _T("SELECT OWNER, GROUPNAME, USERNAME, USERNAME AS SCHEMANAME, USERMODE, MAXTIMEOUT, COSTWARNING, COSTLIMIT, CONNECTMODE, COMMENT "\
                                      "FROM   DOMAIN.USERS "\
                                      "WHERE  USERNAME = '%s' AND USERNAME = '%s' AND USERNAME = '%s' AND USERNAME = '%s' ")},
        {sel_Relational_2,         _T("SELECT ROLE, PASSWORD_REQUIRED "\
                                      "FROM   DOMAIN.ROLES "\
                                      "WHERE  OWNER = '%s' ")},
        {sel_Relational_3,         _T("SELECT OWNER , SEQUENCE_NAME,INCREMENT_BY,LAST_NUMBER,MAX_VALUE,MIN_VALUE,CYCLE_FLAG,CACHE_SIZE,ORDER_FLAG "\
                                      "FROM   DOMAIN.SEQUENCES "\
                                      "WHERE  OWNER = '%s' ")},
        {sel_Relational_4,         _T("SELECT OWNER, OWNER AS SCHEMANAME, DBPROCNAME, DEFINITION, COMMENT "\
                                      "FROM   DOMAIN.DBPROCEDURES "\
                                      "WHERE  OWNER = '%s' AND DEFINITION IS NOT NULL AND DBPROCNAME NOT IN ('TRANSFORMATIONPACKAGE','DEFINE_SERVER','SHOW_SERVER','SHOW_SESSIONS','UNDEFINE_SERVER') "\
									  "ORDER  BY CREATEDATE, CREATETIME ASC ")},
        {sel_Relational_5,         _T("SELECT COLUMNNAME, DATATYPE, LEN, DEC, CODETYPE, MODE, KEYPOS, NULLABLE, \"DEFAULT\",COMMENT  "\
                                      "FROM   DOMAIN.COLUMNS "\
                                      "WHERE  OWNER = '%s' AND TABLENAME = '%s' ") },
        {sel_Relational_6,         _T("SELECT CONSTRAINTNAME, DEFINITION "\
                                      "FROM   DOMAIN.CONSTRAINTS "\
                                      "WHERE  OWNER = '%s' AND TABLENAME = '%s' ")      },
        {sel_Relational_7,         _T("SELECT FKEYNAME, COLUMNNAME, REFTABLENAME, REFOWNER, REFCOLUMNNAME, RULE, COMMENT "\
                                      "FROM   DOMAIN.FOREIGNKEYCOLUMNS "\
                                      "WHERE  OWNER = '%s' AND TABLENAME = '%s' ") },
        {sel_Relational_8,         _T("SELECT COMMENT, SAMPLE_PERCENT, SAMPLE_ROWS, 'NO' "\
                                      "FROM   DOMAIN.TABLES "\
                                      "WHERE  OWNER = '%s' AND TABLENAME = '%s' ")},
        {sel_Relational_9,         _T("SELECT TYPE, INDEXNAME, COMMENT, DISABLED "\
                                      "FROM   DOMAIN.INDEXES "\
                                      "WHERE  OWNER = '%s' AND TABLENAME = '%s' ")},
        {sel_Relational_10,        _T("SELECT DEFINITION, TRIGGERNAME, TABLENAME, COMMENT, OWNER "\
                                      "FROM   DOMAIN.TRIGGERS "\
                                      "WHERE  OWNER = '%s' AND TABLENAME = '%s' AND DEFINITION IS NOT NULL  ")},
        {sel_Relational_11,      __T( "SELECT VIEWNAME, OWNER, DEFINITION "\
                                      "FROM   DOMAIN.VIEWDEFS "\
                                      "WHERE  OWNER = '%s' AND VIEWNAME = '%s' " )},
        {sel_Relational_12,        _T("SELECT OWNER, SYNONYMNAME, PUBLIC, TABLEOWNER, TABLENAME, COMMENT "\
                                      "FROM   DOMAIN.SYNONYMS "\
                                      "WHERE  OWNER = '%s' AND SYNONYMNAME = '%s' AND OWNER NOT IN ('SYS') " )},
        {sel_Relational_13,        _T("SELECT COUNT(DISTINCT OWNER) "\
                                      "FROM   DOMAIN.COLUMNS "\
                                      "WHERE  TABLENAME = '%s' ")},
        {sel_Relational_14,        _T("SELECT \"DEFAULT\", POS, DATATYPE "\
                                      "FROM   DOMAIN.COLUMNS "\
                                      "WHERE  OWNER = '%s'  AND TABLENAME = '%s' ORDER BY POS ") },
        {sel_Relational_15,        _T("SELECT TYPE, COLUMNNAME, SORT, COMMENT "\
                                      "FROM   DOMAIN.INDEXCOLUMNS "\
                                      "WHERE  OWNER = '%s' AND TABLENAME = '%s' AND INDEXNAME = '%s' ORDER BY COLUMNNO" )},
        {sel_Relational_16,        _T("SELECT OWNER, GROUPNAME, USERNAME, USERNAME, USERMODE, MAXTIMEOUT, COSTWARNING, COSTLIMIT, CONNECTMODE, COMMENT "\
                                      "FROM   DOMAIN.USERS "\
                                      "%s "\
                                      "ORDER BY CREATEDATE,CREATETIME ASC ")},
        {sel_Relational_17,        _T("")},
        {sel_Relational_18,        _T("SELECT COLUMNNAME FROM DOMAIN.COLUMNS WHERE OWNER = '%s' AND TABLENAME = '%s' ORDER BY POS")},
        {exist_Transformation_100, _T("SYSDBA.TRANSFORMATIONMODEL")},
        {exist_Transformation_101, _T("SYSDBA.TRANSFORMATIONPACKAGES")},
        {exist_Transformation_102, _T("SYSDBA.TRANSFORMATIONSTEPS" )},
        {exist_Transformation_103, _T("SYSDBA.TRANSFORMATIONRESOURCES")},
        {exist_Transformation_104, _T("SYSDBA.TRANSFORMATIONSTATISTIC" )},
        {exist_Transformation_105, _T("SYSDBA.TRANSFORMATIONMONITOR")},
        {exist_Transformation_106, _T("TEMP.LOA_SAVE_CATALOG_LS00")  },
        {exist_Transformation_107, _T("TRANSFORMATIONPACKAGE")},
        {exist_Transformation_108, _T("SELECT COUNT(*) FROM %s.DBPROCEDURES %s ")},
        {exist_Transformation_109, _T("LOA_MAP_CURSOR")},
        {exist_Transformation_110, _T("LOA_CATALOG_CURSOR")},
        {exist_Transformation_111, _T("EXISTS TABLE \"%s\".\"%s\"")},
        {exist_Transformation_112, _T("CURRENT USER SYSDBA")},
        {exist_Transformation_113, _T("SELECT SOURCESCHEMANAME, TARGETSCHEMANAME FROM SYSLOADER.MAPSCHEMANAMES WHERE MAPPINGNAME = '%s' ")},
        {drop_Transformation_200,  _T("DROP TABLE  TEMP.LOA_SAVE_CATALOG_LS00 ")},
        {drop_Transformation_201,  _T("DROP DBPROC SYSDBA.TRANSFORMATIONPACKAGE ")},
        {ddl_Transformation_300 ,  _T("CREATE TABLE SYSDBA.TRANSFORMATIONMODEL( "\
            /*01*/                    "PACKAGEGUID            CHAR(24) BYTE, "\
            /*02*/                    "TASKID                 FIXED(6,0)  DEFAULT 0, "\
            /*03*/                    "SERVERNODE             CHAR(32)    DEFAULT NULL, "\
            /*04*/                    "SERVERDB               CHAR(32)    DEFAULT NULL, "\
            /*05*/                    "CODETYPE               CHAR(32)    DEFAULT NULL, "\
            /*06*/                    "USERNAME               CHAR(32)    DEFAULT NULL, "\
            /*07*/                    "CODEPAGEID             CHAR(32)    DEFAULT NULL, "\
            /*08*/                    "STORAGEDEVICE          CHAR(32)    DEFAULT NULL, "\
            /*09*/                    "TRANSACTIONSIZE        FIXED(10,0) DEFAULT 0, "\
            /*10*/                    "TRANSFORMATION         CHAR(32)    DEFAULT NULL, "\
            /*11*/                    "PART                   CHAR(8)     DEFAULT NULL, "\
            /*12*/                    "STEPID                 FIXED(6,0)  DEFAULT 0, "\
            /*13*/                    "OWNER                  CHAR(32),    "\
            /*14*/                    "TABLENAME              CHAR(32),    "\
            /*15*/                    "TABLETYPE              CHAR(8 )     ASCII, "\
            /*16*/                    "CATALOGSTREAMNAME      CHAR(254)    DEFAULT NULL, "\
            /*17*/                    "CATALOGSTREAMTYPE      CHAR(32)     DEFAULT NULL, "\
            /*18*/                    "CATALOGSTREAMEBID      CHAR(32)     DEFAULT NULL, "\
            /*19*/                    "CATALOGSTREAMFORMAT    CHAR(32)     DEFAULT NULL, "\
            /*20*/                    "DATASTREAMNAME         CHAR(254)    DEFAULT NULL, "\
            /*21*/                    "DATASTREAMTYPE         CHAR(32)     DEFAULT NULL, "\
            /*22*/                    "DATASTREAMEBID         CHAR(32)     DEFAULT NULL, "\
            /*23*/                    "DATASTREAMFORMAT       CHAR(32)     DEFAULT NULL, "\
            /*24*/                    "DATASTREAMHEADERBLOCK  FIXED(6,0)   DEFAULT 0, "\
            /*25*/                    "DATASTREAMHEADERPOS    FIXED(10,0)  DEFAULT 0, "\
            /*26*/                    "DATASTREAMDATABLOCK    FIXED(6,0)   DEFAULT 0, "\
            /*27*/                    "DATASTREAMDATAPOS      FIXED(10,0)  DEFAULT 0, "\
            /*28*/                    "DATASTREAMTRAILERBLOCK FIXED(6,0)   DEFAULT 0, "\
            /*29*/                    "DATASTREAMTRAILERPOS   FIXED(10,0)  DEFAULT 0, "\
            /*30*/                    "EXCLUDE                BOOLEAN      DEFAULT FALSE, "\
            /*31*/                    "RESTART                BOOLEAN      DEFAULT TRUE, "\
            /*32*/                    "RESTORE                BOOLEAN      DEFAULT FALSE, "\
            /*33*/                    "PAGECOUNT              FIXED(10,0)  DEFAULT 0, "\
            /*34*/                    "ROWCOUNT               FIXED(10,0)  DEFAULT 0, "\
            /*35*/                    "STARTTIME              TIMESTAMP    DEFAULT NULL, "\
            /*36*/                    "ENDTIME                TIMESTAMP    DEFAULT NULL, "\
            /*37*/                    "REQUESTTIME            FIXED(15,3)  DEFAULT 0, "\
            /*38*/                    "RECEIVETIME            FIXED(15,3)  DEFAULT 0, "\
            /*39*/                    "IOTIME                 FIXED(15,3)  DEFAULT 0, "\
            /*40*/                    "LOGTIME                FIXED(15,3)  DEFAULT 0, "\
            /*41*/                    "SEQ_NO                 FIXED(6,0)   DEFAULT 0, "\
            /*42*/                    "CMD                    CHAR(254)    ASCII, "\
                                      "PRIMARY KEY ( PACKAGEGUID, TASKID, STEPID, OWNER, TABLENAME, SEQ_NO), "\
                                      "CONSTRAINT CODETYPE CHECK CODETYPE IS NULL OR CODETYPE IN ('ASCII', 'UCS2', 'UTF8' ), "\
                                      "CONSTRAINT STORAGEDEVICE  CHECK STORAGEDEVICE IS NULL OR STORAGEDEVICE IN ('NONE', 'ADSM', 'BACK', 'NSR') , "\
                                      "CONSTRAINT TRANSFORMATION CHECK TRANSFORMATION IS NULL OR TRANSFORMATION IN ( 'EXPORT', 'IMPORT', 'EXTRACT', 'LOAD', 'UPDATE') , "\
                                      "CONSTRAINT PART CHECK PART IS NULL OR PART IN ('ALL', 'USER', 'TABLE') , "\
                                      "CONSTRAINT TABLETYPE CHECK TABLETYPE IS NULL OR TABLETYPE IN ('TABLE', 'VIEW', 'SYNONYM'), "\
                                      "CONSTRAINT CATALOGSTREAMTYPE CHECK CATALOGSTREAMTYPE IS NULL OR CATALOGSTREAMTYPE IN ('FILE', 'PIPE', 'TAPE'), "\
                                      "CONSTRAINT CATALOGSTREAMFORMAT CHECK CATALOGSTREAMFORMAT IS NULL OR CATALOGSTREAMFORMAT IN ('DDL', 'XML'), "\
                                      "CONSTRAINT DATASTREAMTYPE CHECK DATASTREAMTYPE IS NULL OR DATASTREAMTYPE IN ('FILE', 'PIPE', 'TAPE'), "\
                                      "CONSTRAINT DATASTREAMFORMAT CHECK DATASTREAMFORMAT IS NULL OR DATASTREAMFORMAT IN ('PAGES', 'RECORDS', 'COMPRESSED', 'FORMATTED') ) "\
                                      "INTERNAL")},
        {ddl_Transformation_301, _T( "ALTER  TABLE SYSDBA.TRANSFORMATIONMODEL "\
                                     "ALTER  CONSTRAINT \"CODETYPE\"  CHECK \"CODETYPE\" IS NULL OR \"CODETYPE\"  IN ('ASCII','UCS2', 'UTF8')" )},
        {ddl_Transformation_302, _T(" CREATE DBPROC SYSDBA.TRANSFORMATIONPACKAGE ( IN packageguid CHAR(24) BYTE )  "\
                                    " AS "\
                                    "     VAR  rc INTEGER; errmsg CHAR(80);"\
                                    "          task_id  FIXED(6,0);step_id  FIXED(6,0); temp_step_id FIXED(6,0); seq_no FIXED(6,0); owner CHAR(32); tablename CHAR(32);"\
                                    "          step_id_ FIXED(6,0); "\
                                    " TRY "\
                                    "     SET    rc = 0;"\
                                    "     SET    task_id = 0; SET  step_id_ = 0; SET  temp_step_id = 0;"\
                                    "     SELECT MAX(TASKID)+1,\"STEPID\", \"SEQ_NO\", \"OWNER\", \"TABLENAME\" "\
                                    "            FROM SYSDBA.TRANSFORMATIONMODEL "\
                                    "     WHERE  \"PACKAGEGUID\" = :packageguid "\
                                    "            GROUP BY \"STEPID\", \"SEQ_NO\", \"OWNER\", \"TABLENAME\" "\
                                    "            ORDER BY \"STEPID\", \"SEQ_NO\"  ASC ;"\
                                    "     WHILE $rc = 0 DO "\
                                    "           BEGIN "\
                                    "                 FETCH INTO :task_id, :step_id, :seq_no, :owner, :tablename;"\
                                    "                       IF step_id = temp_step_id  THEN "\
                                    "                          SET step_id_ = step_id_ "\
                                    "                       ELSE "\
                                    "                          SET step_id_ = step_id_+1; "\
                                    "                       UPDATE SYSDBA.TRANSFORMATIONMODEL "\
                                    "                       SET    \"TASKID\" = :task_id ,"\
                                    "                              \"STEPID\" = :step_id_,"\
                                    "                              \"SEQ_NO\" = :seq_no  "\
                                    "                       WHERE  \"PACKAGEGUID\" = :packageguid AND "\
                                    "                              \"OWNER\"       = :owner       AND \"TABLENAME\" = :tablename AND "\
                                    "                              \"STEPID\"      = :step_id     AND \"SEQ_NO\"    = :seq_no    AND "\
                                    "                              \"TASKID\" = 0 ; "\
                                    "                 SET temp_step_id = step_id;"\
                                    "           END;"\
                                    " CATCH "\
                                    "     IF $rc <> 100 THEN "\
                                    "        STOP ($rc, 'Unexpected error');")},
        {call_Transformation_303,  _T( "CALL SYSDBA.TRANSFORMATIONPACKAGE ( x'%s' )")},
        {ddl_Transformation_304,   _T( "CREATE VIEW TRANSFORMATIONPACKAGES AS "\
            /*01*/                     "SELECT PACKAGEGUID, TASKID, STEPID, OWNER, TABLENAME, EXCLUDE, RESTART "\
            /*02*/                     "FROM SYSDBA.TRANSFORMATIONMODEL "\
                                       "WHERE TABLETYPE = \'TABLE\' AND CMD NOT LIKE 'GRANT %%' INTERNAL")},
        {ddl_Transformation_305,  _T( "CREATE VIEW TRANSFORMATIONSTEPS AS "\
            /*01*/                    "SELECT TRANSFORMATION,OWNER,TABLENAME,CATALOGSTREAMNAME,DATASTREAMNAME, DATASTREAMHEADERBLOCK,DATASTREAMHEADERPOS,DATASTREAMDATABLOCK,DATASTREAMDATAPOS,ROWCOUNT AS START "\
            /*02*/                    "FROM SYSDBA.TRANSFORMATIONMODEL "\
                                      "WHERE TABLETYPE = \'TABLE\' AND CMD NOT LIKE 'GRANT %%' INTERNAL")},
        {ddl_Transformation_306,   _T("CREATE VIEW TRANSFORMATIONRESOURCES AS "\
            /*01*/                    "SELECT DISTINCT PACKAGEGUID, SERVERNODE,SERVERDB,USERNAME ,CODEPAGEID, STORAGEDEVICE, CATALOGSTREAMNAME,CATALOGSTREAMTYPE,CATALOGSTREAMEBID,DATASTREAMNAME,DATASTREAMTYPE,DATASTREAMEBID "\
            /*02*/                    "FROM SYSDBA.TRANSFORMATIONMODEL INTERNAL")
        },
       {ddl_Transformation_307,   _T("CREATE VIEW TRANSFORMATIONSTATISTIC AS "\
                                     "SELECT PACKAGEGUID, STEPID, OWNER, TABLENAME, "\
                                     "DATASTREAMTRAILERPOS-DATASTREAMHEADERPOS AS BYTECOUNT, ROWCOUNT, TRANSACTIONSIZE, "\
                                     "(HOUR         (ENDTIME) - HOUR        (STARTTIME))  *3600000 + "\
                                     "(MINUTE       (ENDTIME) - MINUTE      (STARTTIME))    *60000 + "\
                                     "(SECOND       (ENDTIME) - SECOND      (STARTTIME))     *1000 + "\
                                     "((MICROSECOND  (ENDTIME) - MICROSECOND (STARTTIME))    /1000)  AS SYSTEMTIME, "\
                                     "(REQUESTTIME+RECEIVETIME) *1000 AS KERNELTIME, "\
                                     "(HOUR         (ENDTIME) - HOUR        (STARTTIME))  *3600000 +  "\
                                     "(MINUTE       (ENDTIME) - MINUTE      (STARTTIME))    *60000 +  "\
                                     "(SECOND       (ENDTIME) - SECOND      (STARTTIME))     *1000 +  "\
                                     "((MICROSECOND  (ENDTIME) - MICROSECOND (STARTTIME))     /1000) - "\
                                     "(REQUESTTIME+RECEIVETIME)*1000 AS LOADERTIME, "\
                                     "(HOUR         (ENDTIME) - HOUR        (STARTTIME))  *3600000 +  "\
                                     "(MINUTE       (ENDTIME) - MINUTE      (STARTTIME))    *60000 +  "\
                                     "(SECOND       (ENDTIME) - SECOND      (STARTTIME))     *1000 +  "\
                                     "((MICROSECOND  (ENDTIME) - MICROSECOND (STARTTIME))     /1000) - "\
                                     "(REQUESTTIME+RECEIVETIME)*1000 - (IOTIME) *1000 - (LOGTIME) *1000 AS DATATIME, "\
                                     "(LOGTIME)*1000 AS LOGTIME, "\
                                     "(IOTIME) *1000 AS IOTIME "\
                                     "FROM SYSDBA.TRANSFORMATIONMODEL"\
                                     "WHERE EXCLUDE = FALSE AND RESTART = FALSE AND TABLETYPE = 'TABLE' AND ( CMD LIKE 'TABLE %%' OR CMD IS NULL ) "\
                                     "GROUP BY PACKAGEGUID, STEPID, OWNER, TABLENAME, "\
                                     "DATASTREAMTRAILERPOS, DATASTREAMHEADERPOS, ROWCOUNT, TRANSACTIONSIZE, "\
                                     "ENDTIME, STARTTIME, REQUESTTIME, RECEIVETIME, IOTIME, LOGTIME INTERNAL ")},
        {ddl_Transformation_308,  _T("CREATE VIEW TRANSFORMATIONMONITOR AS "\
        /*01*/                       "SELECT PACKAGEGUID,COUNT(*) AS TABLECOUNT, SUM(ROWCOUNT) AS ROWCOUNT, SUM(SYSTEMTIME) AS SYSTEMDURATION, SUM(KERNELTIME) AS KERNELDURATION, SUM(LOADERTIME) AS LOADERDURATION, SUM(DATATIME) AS DATADURATION, SUM(LOGTIME) AS LOGDURATION, SUM(IOTIME) AS IODURATION "\
        /*02*/                       "FROM SYSDBA.TRANSFORMATIONSTATISTIC  "\
        /*03*/                       "GROUP BY PACKAGEGUID INTERNAL ")},
        {ddl_Transformation_309,  _T("CREATE TABLE SYSDBA.CODEPAGE "\
                                     "(CODEPAGEID    CHAR (32)  NOT NULL,"\
                                     " CODEPAGETABLE CHAR (512) BYTE NOT NULL,"\
                                     " PRIMARY KEY (CODEPAGEID)) INTERNAL")},
        {ddl_Transformation_310,   _T("ALTER  TABLE SYSDBA.TRANSFORMATIONMODEL MODIFY TRANSACTIONSIZE   FIXED(10,0) ")},
        {ddl_Transformation_311,   _T("ALTER  TABLE SYSDBA.TRANSFORMATIONMODEL MODIFY REQUESTTIME       FIXED(15,3) ")},
        {ddl_Transformation_312,   _T("ALTER  TABLE SYSDBA.TRANSFORMATIONMODEL MODIFY RECEIVETIME       FIXED(15,3) ")},                             
        {ddl_Transformation_313,   _T("ALTER  TABLE SYSDBA.TRANSFORMATIONMODEL MODIFY IOTIME            FIXED(15,3) ")},    
        {ddl_Transformation_314,   _T("ALTER  TABLE SYSDBA.TRANSFORMATIONMODEL MODIFY LOGTIME           FIXED(15,3) ")}, 
        {ddl_Transformation_315,   _T("ALTER  TABLE SYSDBA.TRANSFORMATIONMODEL "\
                                      "ALTER  CONSTRAINT \"TRANSFORMATION\"    CHECK \"TRANSFORMATION\"  IS NULL OR \"TRANSFORMATION\"  IN ('EXPORT' , 'IMPORT', 'EXTRACT', 'LOAD', 'UPDATE' )" )},
        {ddl_Transformation_316,   _T("ALTER  TABLE SYSDBA.TRANSFORMATIONMODEL "\
                                      "ALTER  CONSTRAINT \"PART\"              CHECK \"PART\"            IS NULL OR \"PART\"            IN ('ALL'   , 'DB', 'USER', 'SCHEMA', 'TABLE' )" ) },
        {trans_Transformation_316, _T("LOCK  TABLE  SYSDBA.CODEPAGE IN EXCLUSIVE MODE") },
        {trans_Transformation_317, _T("TRANSLATE SCHEMA USING x'%s' ")},
        {trans_Transformation_318, _T("TRANSLATE SCHEMA OFF ")},
        {dal_Transformation_400,   _T("GRANT ALTER,INSERT,UPDATE,DELETE,SELECT ON SYSDBA.TRANSFORMATIONMODEL TO PUBLIC ")},
        {dal_Transformation_401,   _T("GRANT SELECT  ON TRANSFORMATIONPACKAGES TO PUBLIC ")},
        {dal_Transformation_402,   _T("GRANT EXECUTE ON SYSDBA.TRANSFORMATIONPACKAGE TO PUBLIC")},
        {dal_Transformation_403,   _T("GRANT SELECT  ON TRANSFORMATIONSTEPS TO PUBLIC ")},
        {dal_Transformation_404 ,  _T("GRANT SELECT  ON TRANSFORMATIONRESOURCES TO PUBLIC ")},
        {dal_Transformation_405,   _T("GRANT SELECT  ON TRANSFORMATIONSTATISTIC TO PUBLIC ")},
        {dal_Transformation_406,   _T("GRANT SELECT  ON TRANSFORMATIONMONITOR   TO PUBLIC ")},
        {dal_Transformation_407,   _T("GRANT SELECT,DELETE ON TEMP.LOA_SAVE_CATALOG_LS00 TO PUBLIC ")},
        {dal_Transformation_408,   _T("GRANT ALL ON SYSDBA.CODEPAGE TO PUBLIC")},
        {ins_Transformation_500,   _T("INSERT INTO SYSDBA.TRANSFORMATIONMODEL ( PACKAGEGUID, STEPID  ,   CMD,   SEQ_NO,   OWNER,   TABLENAME,   TABLETYPE ) ( "\
                                      "SELECT                                   x'%s'      , C.CMD_NO, C.CMD, C.SEQ_NO, C.OWNER, C.TABLENAME, C.TABLETYPE  "\
                                      "FROM  TEMP.LOA_SAVE_CATALOG_LS00 C   WHERE "\
                                      "C.OWNER     NOT  IN ('SYS', SYSDBA, 'PUBLIC' ) "\
                                      " %s                   AND "\
                                      "C.CMD_NO         IN ( SELECT C.CMD_NO  FROM TEMP.LOA_SAVE_CATALOG_LS00 C WHERE   "\
                                      "                           ( C.CMD NOT LIKE 'GRANT %%' AND C.TABLETYPE IN ( 'TABLE','VIEW','SYNONYM' ) AND C.SEQ_NO = 0 ) OR  "\
                                      "                           ( C.CMD     LIKE 'GRANT %%' )) AND "\
                                      "C.TABLENAME NOT  IN ('SYSUPGRADEHISTORY','POSTUPGRADECHECKLIST','POSTUPGRADECHECKLISTUNICODE') ) ")},
        {ins_Transformation_501,   _T("INSERT INTO SYSDBA.TRANSFORMATIONMODEL ( PACKAGEGUID, STEPID, CMD, SEQ_NO, OWNER, TABLENAME, TABLETYPE ) "\
                                      "(SELECT x'%s' , CMD_NO, CMD, SEQ_NO, OWNER, TABLENAME, TABLETYPE "\
                                      "FROM TEMP.LOA_SAVE_CATALOG_LS00 "\
                                      "WHERE NOT (TABLETYPE IN ('VIEW', 'SYNONYM') AND SEQ_NO > 0) )")},
        {ins_Transformation_502,   _T("SAVE CATALOG ALL  INTO TEMP.LOA_SAVE_CATALOG_LS00")},
        {ins_Transformation_503,   _T("SAVE CATALOG USER INTO TEMP.LOA_SAVE_CATALOG_LS00")},
        {ins_Transformation_504,   _T("IMPORT COLUMNS TABLE SYSDBA.TRANSFORMATIONMODEL  "\
                                      "OWNER                  1  TABLENAME           2  "\
                                      "DATASTREAMHEADERBLOCK  3  DATASTREAMHEADERPOS 4 "\
                                      "ROWCOUNT 6 "\
                                      "INSTREAM '%s' COMPRESSED DELIMITER '\"' SEPARATOR ',' ")},
        {ins_Transformation_505,   _T("INSERT INTO SYSDBA.CODEPAGE VALUES('%s', x'%.*s')")},
        {ins_Transformation_506,   _T("SAVE CATALOG USER INTO TEMP.LOA_SAVE_CATALOG_LS00")},
        {ins_Transformation_507,   _T("INSERT INTO SYSDBA.MAPSCHEMANAMES VALUES (x'%s','%s','%s','%s') ")},
        {del_Transformation_600,   _T("DELETE FROM SYSDBA.TRANSFORMATIONMODEL WHERE PACKAGEGUID = x'%s'")},
        {del_Transformation_601,   _T("DELETE FROM SYSDBA.TRANSFORMATIONMODEL WHERE PACKAGEGUID = x'%s' AND CMD NOT LIKE 'TABLE %%' ")},
        {del_Transformation_602,   _T("DELETE FROM SYSDBA.TRANSFORMATIONMODEL WHERE EXCLUDE = TRUE OR ROWCOUNT = 0  ")},
        {del_Transformation_603,   _T("DELETE FROM SYSDBA.TRANSFORMATIONMODEL WHERE RESTART = FALSE ")},
        {del_Transformation_604,   _T("DELETE FROM TEMP.LOA_SAVE_CATALOG_LS00 "\
                                      "WHERE TYPE = 'SYSTEM' OR OWNER IN ( 'SYS','PUBLIC' ) OR OWNER = SYSDBA ")},
        {del_Transformation_605,   _T("DELETE FROM SYSLOADER.MAPSCHEMANAMES WHERE %s MAPPINGNAME = '%s' ")},
        {sel_Transformation_700,   _T("SELECT COUNT (*) FROM "\
                                      "SYSDBA.TRANSFORMATIONMODEL "\
                                      "WHERE CMD NOT LIKE 'GRANT %%' "\
                                      "AND TABLETYPE = 'TABLE' AND STEPID > 0 AND EXCLUDE = %s AND  "\
                                      "PACKAGEGUID   = x'%s'")},
        {sel_Transformation_701,   _T("SELECT   MAX(TASKID) FROM SYSDBA.TRANSFORMATIONMODEL")},
        {sel_Transformation_702,   _T("SELECT   STEPID, OWNER, OWNER AS SCHEMANAME, TABLENAME, TABLETYPE, CMD, MAX(TASKID) AS TASKID "\
                                      "FROM     SYSDBA.TRANSFORMATIONMODEL "\
                                      "WHERE    PACKAGEGUID = x'%s' AND OWNER NOT IN ('SYS','PUBLIC') %s "\
                                      "GROUP BY STEPID, OWNER, OWNER, TABLENAME, TABLETYPE, CMD "\
                                      "ORDER BY %s STEPID, SEQ_NO ASC ")},
        {sel_Transformation_703,   _T("SELECT   STEPID, OWNER, OWNER AS SCHEMANAME, TABLENAME, TABLETYPE, CMD, MAX(TASKID) AS TASKID "\
                                      "FROM     SYSDBA.TRANSFORMATIONMODEL "\
                                      "WHERE    PACKAGEGUID = x'%s' AND OWNER NOT IN ('SYS','PUBLIC') AND TABLETYPE IN ('TABLE') %s "\
                                      "GROUP BY STEPID, OWNER, OWNER, TABLENAME, TABLETYPE, CMD "\
                                      "ORDER BY %s STEPID, SEQ_NO ASC ")},
        {sel_Transformation_704,   _T("SELECT   STEPID, OWNER, OWNER AS SCHEMANAME, TABLENAME, TABLETYPE, CMD "\
                                      "FROM     SYSDBA.TRANSFORMATIONMODEL "\
                                      "WHERE    PACKAGEGUID = x'%s' AND OWNER NOT IN ('SYS','PUBLIC') AND TABLETYPE IN ('VIEW','SYNONYM') "\
                                      "GROUP BY STEPID, OWNER, OWNER, TABLENAME, TABLETYPE, CMD "\
                                      "ORDER BY STEPID, SEQ_NO ASC ")},
        {sel_Transformation_705,   _T("SELECT   STEPID, OWNER, OWNER AS SCHEMANAME, TABLENAME "\
                                      "FROM     SYSDBA.TRANSFORMATIONMODEL "\
                                      "WHERE    PACKAGEGUID = x'%s' AND OWNER NOT IN ('SYS','PUBLIC') AND TABLETYPE IN ('TABLE')  AND CMD LIKE '%%FOREIGN%%' "\
                                      "GROUP BY STEPID, OWNER, OWNER, TABLENAME "\
                                      "ORDER BY STEPID, SEQ_NO ASC ")},
        {sel_Transformation_706,   _T("SELECT DISTINCT 0, OWNER, OWNER AS SCHEMANAME FROM SYSDBA.TRANSFORMATIONMODEL "\
                                      "WHERE  PACKAGEGUID = x'%s' AND OWNER NOT IN ('SYS','PUBLIC')")},
        {sel_Transformation_707,   _T("SELECT DISTINCT 0, OWNER, OWNER AS SCHEMANAME FROM SYSDBA.TRANSFORMATIONMODEL "\
                                      "WHERE  PACKAGEGUID = x'%s' AND OWNER NOT IN ('SYS','PUBLIC') AND OWNER = '%s'" )},
        {sel_Transformation_708,   _T("SELECT "\
                                      "TRANSFORMATION, OWNER, OWNER AS SCHEMANAME, TABLENAME,  "\
                                      "DATASTREAMTYPE, DATASTREAMNAME, DATASTREAMFORMAT, CODETYPE, DATASTREAMEBID, "\
                                      "DATASTREAMHEADERBLOCK, DATASTREAMHEADERPOS, DATASTREAMDATABLOCK, DATASTREAMDATAPOS, DATASTREAMTRAILERBLOCK, DATASTREAMTRAILERPOS, "\
                                      "PAGECOUNT, ROWCOUNT, EXCLUDE , STEPID "\
                                      "FROM  SYSDBA.TRANSFORMATIONMODEL "\
                                      "WHERE PACKAGEGUID = x'%s' AND EXCLUDE = FALSE AND STEPID > 0 AND RESTART = TRUE AND TABLETYPE = 'TABLE' AND CMD LIKE 'TABLE %%'"\
                                      "GROUP BY "\
                                      "TRANSFORMATION, OWNER, OWNER, TABLENAME, "\
                                      "DATASTREAMTYPE, DATASTREAMNAME, DATASTREAMFORMAT, CODETYPE, DATASTREAMEBID, DATASTREAMHEADERBLOCK, DATASTREAMHEADERPOS, DATASTREAMDATABLOCK, DATASTREAMDATAPOS, DATASTREAMTRAILERBLOCK, DATASTREAMTRAILERPOS, "\
                                      "PAGECOUNT, ROWCOUNT, EXCLUDE, STEPID "\
                                      "ORDER BY STEPID, SEQ_NO  ASC " )},
        {sel_Transformation_709,   _T("SELECT COUNT (*) FROM SYSDBA.TRANSFORMATIONMODEL "\
                                       "WHERE  PACKAGEGUID = x'%s' AND STEPID > 0 ")},
        {sel_Transformation_710,   _T("SELECT COUNT (*) FROM SYSDBA.TRANSFORMATIONMODEL "\
                                      "WHERE PACKAGEGUID = x'%s' AND EXCLUDE = FALSE AND STEPID > 0 AND CMD LIKE 'TABLE %%' AND TABLETYPE = 'TABLE'   ")},
        {sel_Transformation_711,   _T("SELECT COUNT (*) FROM SYSDBA.TRANSFORMATIONMODEL "\
                                      "WHERE PACKAGEGUID = x'%s' AND STEPID > 0 AND CMD LIKE 'CREATE VIEW %%' " )},
        {sel_Transformation_712,   _T("SELECT COUNT (*) FROM SYSDBA.TRANSFORMATIONMODEL "\
                                      "WHERE PACKAGEGUID = x'%s' AND STEPID > 0 AND CMD LIKE 'CREATE SYNONYM %%' ")},
        {sel_Transformation_713,   _T("SELECT COUNT(*) FROM \"%s\".\"%s\" ") },
        {sel_Transformation_714,   _T("EXPORT COLUMNS "\
            /*01*/                    "PACKAGEGUID          , TASKID              , SERVERNODE             , SERVERDB            , "\
            /*05*/                    "CODETYPE             , USERNAME            , CODEPAGEID             , STORAGEDEVICE       , "\
            /*09*/                    "TRANSACTIONSIZE      , TRANSFORMATION      , PART                   , STEPID              , "\
            /*13*/                    "OWNER                , OWNER AS SCHEMANAME , TABLENAME              , TABLETYPE           , "\
                                      "CATALOGSTREAMNAME    , CATALOGSTREAMTYPE   , CATALOGSTREAMEBID      , CATALOGSTREAMFORMAT , "\
            /*17*/                    "DATASTREAMNAME       , DATASTREAMTYPE      , DATASTREAMEBID         , DATASTREAMFORMAT    , "\
            /*21*/                    "DATASTREAMHEADERBLOCK, DATASTREAMHEADERPOS , DATASTREAMTRAILERBLOCK , DATASTREAMTRAILERPOS, DATASTREAMDATABLOCK, DATASTREAMDATAPOS, "\
            /*25*/                    "\"EXCLUDE\"          , \"RESTART\"         , RESTORE                , PAGECOUNT           , "\
            /*29*/                    "ROWCOUNT             , STARTTIME           , ENDTIME                , REQUESTTIME         , "\
            /*33*/                    "RECEIVETIME          , IOTIME              , LOGTIME               "\
                                      "FROM SYSDBA.TRANSFORMATIONMODEL "\
                                      "WHERE PACKAGEGUID = x'%s'  AND STEPID > 0 AND TABLETYPE = \'TABLE\' AND CMD NOT LIKE 'GRANT %%' ORDER BY ABS(STEPID) ASC WITH LOCK ISOLATION LEVEL 0 "\
                                      "OUTFIELDS "\
                                      "\"OWNER\"                 1  \"OWNER\"                2 \"TABLENAME\"               3 "\
                                      "\"DATASTREAMNAME\"        4 "\
                                      "\"DATASTREAMHEADERBLOCK\" 5  \"DATASTREAMHEADERPOS\"  6 \"DATASTREAMTRAILERBLOCK\"  7 \"DATASTREAMTRAILERPOS\"  8  \"DATASTREAMDATABLOCK\"  9 \"DATASTREAMDATAPOS\"  10  "\
                                      "\"EXCLUDE\"               11 "\
                                      "\"PACKAGEGUID\"           12 \"TASKID\"              13 \"STEPID\"                 14 \"SERVERNODE\"           15 "\
                                      "\"SERVERDB\"              16 \"USERNAME\"            17 \"CODETYPE\"               18 \"CODEPAGEID\"           19 "\
                                      "\"STORAGEDEVICE\"         20 \"TRANSACTIONSIZE\"     21 \"TRANSFORMATION\"         22 \"PART\"                 23 \"TABLETYPE\"            24 "\
                                      "\"CATALOGSTREAMNAME\"     25 \"CATALOGSTREAMTYPE\"   26 \"CATALOGSTREAMEBID\"      27 \"CATALOGSTREAMFORMAT\"  28 \"DATASTREAMTYPE\"       29 \"DATASTREAMEBID\"     30 "\
                                      "\"DATASTREAMFORMAT\"      31 \"RESTART\"             32 \"RESTORE\"                33 \"PAGECOUNT\"            34 "\
                                      "\"ROWCOUNT\"              35 \"STARTTIME\"           36 \"ENDTIME\"                37 \"REQUESTTIME\"          38 "\
                                      "\"RECEIVETIME\"           39 \"IOTIME\"              40 \"LOGTIME\"                41 "\
                                      "OUTSTREAM FILE '%s' COMPRESSED %s %s INTERNAL" )},
        {sel_Transformation_715,   _T("EXPORT COLUMNS "\
                                      "ROWNO, OWNER, TABLENAME ,  "\
                                      "CATALOGSTREAMNAME, CATALOGSTREAMTYPE, CATALOGSTREAMEBID, CATALOGSTREAMFORMAT, "\
                                      "DATASTREAMNAME   , DATASTREAMTYPE,    DATASTREAMEBID,    DATASTREAMFORMAT,        CODETYPE, "\
                                      "DATASTREAMHEADERBLOCK, DATASTREAMHEADERPOS , DATASTREAMTRAILERBLOCK , DATASTREAMTRAILERPOS, DATASTREAMDATABLOCK, DATASTREAMDATAPOS "\
                                      "FROM  SYSDBA.TRANSFORMATIONMODEL "\
                                      "WHERE PACKAGEGUID = x'%s'  AND STEPID > 0 AND EXCLUDE = FALSE AND TABLETYPE = \'TABLE\' AND CMD NOT LIKE 'GRANT %%' ORDER BY STEPID ASC WITH LOCK ISOLATION LEVEL 0 "\
                                      "OUTFIELDS "\
                                      "\"OWNER\"                 1  \"TABLENAME\"            2    ROWNO                      3 "\
                                      "\"CATALOGSTREAMNAME\"     4  \"CATALOGSTREAMTYPE\"    5  \"CATALOGSTREAMEBID\"        6 \"CATALOGSTREAMFORMAT\"    7 "\
                                      "\"DATASTREAMNAME\"        8  \"DATASTREAMTYPE\"       9  \"DATASTREAMEBID\"          10 \"DATASTREAMFORMAT\"      11  \"CODETYPE\"                12 "\
                                      "\"DATASTREAMHEADERBLOCK\" 13 \"DATASTREAMHEADERPOS\"  14 \"DATASTREAMDATABLOCK\"     15 \"DATASTREAMDATAPOS\"     16  \"DATASTREAMTRAILERBLOCK\"  17 \"DATASTREAMTRAILERPOS\"  18 "\
                                      "OUTSTREAM %s '%s' COMPRESSED INTERNAL" )
        },//upd_Transformation_809
        {sel_Transformation_716,   _T("SELECT CODEPAGETABLE "\
                                      "FROM SYSDBA.CODEPAGE "\
                                      "WHERE CODEPAGEID = '%s'")},
        {sel_Transformation_717,   _T("SELECT COUNT(*) "\
                                      "FROM SYSDBA.TRANSFORMATIONPACKAGES "\
                                      "WHERE PACKAGEGUID = ")},
        {sel_Transformation_718,   _T("SELECT COUNT(*) "\
                                      "FROM SYSDBA.TRANSFORMATIONPACKAGES "\
                                      "WHERE RESTART = FALSE AND PACKAGEGUID = ")},
        {sel_Transformation_719,   _T("SELECT COUNT(*) "\
                                      "FROM   SYSDBA.TRANSFORMATIONMODEL T, DOMAIN.INDEXCOLUMNS IC , DOMAIN.FOREIGNKEYCOLUMNS FK "\
                                      "WHERE  PACKAGEGUID = x'%s' AND "\
                                      "T.CMD  LIKE 'TABLE %%' AND T.CMD LIKE '%%INDEX%%' AND  ( T.OWNER NOT IN ('SYS') OR T.OWNER <> SYSDBA ) AND "\
                                      "IC.OWNER    = T.OWNER AND IC.TABLENAME    = T.TABLENAME AND "\
                                      "FK.REFOWNER = T.OWNER AND FK.REFTABLENAME = T.TABLENAME AND "\
                                      "FK.REFCOLUMNNAME = IC.COLUMNNAME ")},
        {sel_Transformation_720,   _T("SELECT DISTINCT 0, OWNER, OWNER AS SCHEMANAME FROM SYSDBA.TRANSFORMATIONMODEL "\
                                      "WHERE PACKAGEGUID = x'%s' AND OWNER NOT IN ('SYS','PUBLIC') AND OWNER = '%s'" )},
        {sel_Transformation_721,   _T("SELECT DISTINCT 0, OWNER, OWNER AS SCHEMANAME FROM SYSDBA.TRANSFORMATIONMODEL "\
                                      "WHERE PACKAGEGUID = x'%s' AND OWNER NOT IN ('SYS','PUBLIC') AND OWNER = '%s'" )},
        {sel_Transformation_722,   _T(" SELECT PACKAGEGUID, MAPPINGNAME, SOURCESCHEMANAME, TARGETSCHEMANAME FROM SYSLOADER.MAPSCHEMANAMES O WHERE EXISTS "\
                                      "(SELECT PACKAGEGUID, MAPPINGNAME, SOURCESCHEMANAME, TARGETSCHEMANAME FROM SYSLOADER.MAPSCHEMANAMES I WHERE "\
                                      " I.PACKAGEGUID  = x'%s' AND O.PACKAGEGUID  = x'%s' AND "\
                                      " I.MAPPINGNAME  =  '%s' AND O.MAPPINGNAME  =  '%s' AND "\
                                      " I.SOURCESCHEMANAME  = O.SOURCESCHEMANAME AND "\
                                      " I.TARGETSCHEMANAME <> O.TARGETSCHEMANAME ) ")},
        {sel_Transformation_723,   _T("SELECT PACKAGEGUID, MAPPINGNAME, SOURCESCHEMANAME, TARGETSCHEMANAME FROM SYSLOADER.MAPSCHEMANAMES WHERE %s "\
                                      "       MAPPINGNAME        =  '%s'                ")},
        {sel_Transformation_724,   _T("SELECT "\
                                      "TRANSFORMATION, OWNER, OWNER AS SCHEMANAME, TABLENAME,  "\
                                      "DATASTREAMTYPE, DATASTREAMNAME, DATASTREAMFORMAT, CODETYPE, DATASTREAMEBID, "\
                                      "DATASTREAMHEADERBLOCK, DATASTREAMHEADERPOS, DATASTREAMDATABLOCK, DATASTREAMDATAPOS, DATASTREAMTRAILERBLOCK, DATASTREAMTRAILERPOS, "\
                                      "PAGECOUNT, ROWCOUNT, EXCLUDE , STEPID "\
                                      "FROM  SYSDBA.TRANSFORMATIONMODEL "\
                                      "WHERE PACKAGEGUID = x'%s' AND EXCLUDE = FALSE AND STEPID > 0 AND RESTART = TRUE AND TABLETYPE = 'TABLE' AND CMD LIKE 'TABLE %%'"\
                                      "GROUP BY "\
                                      "TRANSFORMATION, OWNER, OWNER, TABLENAME, "\
                                      "DATASTREAMTYPE, DATASTREAMNAME, DATASTREAMFORMAT, CODETYPE, DATASTREAMEBID, DATASTREAMHEADERBLOCK, DATASTREAMHEADERPOS, DATASTREAMDATABLOCK, DATASTREAMDATAPOS, DATASTREAMTRAILERBLOCK, DATASTREAMTRAILERPOS, "\
                                      "PAGECOUNT, ROWCOUNT, EXCLUDE, STEPID "\
                                      "ORDER BY STEPID, SEQ_NO  ASC " )},
        {stmt_EndOfArray_925,      _T("Unknown Statement - please report to SAP Basis Entwicklung Berlin\n")}
    };
	StudioOAL_String statement;
    short i = 0;
    tsp100_VersionResult vres = ls15CheckKernelVersion(getConnection(), 7, 6, 0, 0);
    if (IsEqual_esp100 == vres || IsNewer_esp100 == vres )
    {
        while( (StatementDesc_SysLoader[i].Type != StatementNumber) &&
               (StatementDesc_SysLoader[i].Type != stmt_EndOfArray_925) )
        {
            i++;
        }
		statement = StatementDesc_SysLoader[i].pStatement;
    }
    else
    {
        while( (StatementDesc_SysDba[i].Type != StatementNumber) &&
               (StatementDesc_SysDba[i].Type != stmt_EndOfArray_925) )
        {
            i++;
        }
        statement = StatementDesc_SysDba[i].pStatement;
    }
	tsp100_VersionID1 Version75;
    Version75.MajorVersion_sp100 = 7;
    Version75.MinorVersion_sp100 = 5;
    Version75.CorrLevel_sp100    = 0;
    Version75.BuildNumberPrefix_sp100  = 0;
    vres    = sp100_Compare2VersionIDs(VersionIDType1_esp100, &m_pDBInfo->dbiDBVersion_ls00, &Version75);
    if (IsEqual_esp100 == vres || IsNewer_esp100 == vres )
    {
		if ( sel_Relational_8 == StatementNumber )
			 statement = _T("SELECT COMMENT, SAMPLE_PERCENT, SAMPLE_ROWS, VARIABLE_COLUMNS "\
                            "FROM   DOMAIN.TABLES "\
                            "WHERE  OWNER = '%s' AND TABLENAME = '%s' ");
	}
	return statement;
}
/*===========================================================================*
 *     MetaDataDef::~MetaDataDef
 *===========================================================================*/
MetaDataDef::~MetaDataDef()
{
    ROUTINE_DBG_MEO00 ("MetaDataDef::~MetaDataDef()");

    if( m_pTransformationDef  != NULL )
    {
        delete m_pTransformationDef;
        m_pTransformationDef = NULL;
    }
    if( m_pRelationalDef  != NULL )
    {
        delete m_pRelationalDef;
        m_pRelationalDef = NULL;
    }
    if ( m_pUserConnectionDef  != NULL )
    {
         delete m_pUserConnectionDef;
         m_pUserConnectionDef = NULL;
    }
    if ( m_pError != NULL )
    {
        delete m_pError;
        m_pError = NULL;
    }
}
/*===========================================================================*
 *     MetaDataDef::setODBCDriver
 *===========================================================================*/
SAPDB_Bool MetaDataDef::setODBCDriver( tls00_DBInfo *pDBSession )
{
    ROUTINE_DBG_MEO00 ("MetaDataDef::setODBCDriver");

    SAPDB_Bool isOk = SAPDB_FALSE;

    StudioOAL_String odbcDriver;

    if ( 7 == pDBSession->dbiDBVersion_ls00.MajorVersion_sp100 &&
         2 == pDBSession->dbiDBVersion_ls00.MinorVersion_sp100  )
    {
        m_odbcDriver = "LiveCache 7.2";
        isOk         = SAPDB_TRUE;
    }
    if ( 7 == pDBSession->dbiDBVersion_ls00.MajorVersion_sp100 &&
         3 == pDBSession->dbiDBVersion_ls00.MinorVersion_sp100  )
    {
        m_odbcDriver = "SAP DB 7.3";
        isOk         = SAPDB_TRUE;
    }
    if ( 7 == pDBSession->dbiDBVersion_ls00.MajorVersion_sp100 &&
         4 == pDBSession->dbiDBVersion_ls00.MinorVersion_sp100  )
    {
        m_odbcDriver = "SAP DB";
        isOk         = SAPDB_TRUE;
    }
    return isOk;
}

/*===========================================================================*
 *     MetaDataDef::initResultSets
 *===========================================================================*/
SAPDB_Int4 MetaDataDef::initResultSets( tls00_TransformationCmd *pTransformationCmd , 
                                        tls00_DBInfo            *pDBSession,
                                        tsp00_Addr              ErrText)
{
    ROUTINE_DBG_MEO00 ("MetaDataDef::initResultSets");
    LoaderMessages *pMsgInstance = LoaderMessages::Instance();

    SAPDB_Int4 rc = errOK_els00;
    
    setODBCDriver( pDBSession );
    //*
    //*   check packagepart
    //*
    if (ptAll_els00 == pTransformationCmd->trPartSpec_ls00.psPartType_ls00 &&
        pTransformationCmd->trCatalogFile_ls00.fsFormat_ls00.ffoXml_ls00 == false )
    {
        rc = ls15CheckUserMode(pDBSession,
                               REINTERPRET_CAST(SAPDB_UTF8*, pDBSession->dbiSession_ls00.siUserName_ls00.asCharp()),
                               dbuSysdba_els00, ErrText);
        if (dbRowNotFound_els00 == rc) {
            rc = errRestrictedStmt_els98 ;
            pMsgInstance->ls98Msg(layTableExtract_els98, ErrText, errRestrictedStmt_els98);
        }
    }
    //*
    //*  check stream encoding and database encoding.
    //*
    if (errOK_els00 == rc && ptTable_els00 < pTransformationCmd->trPartSpec_ls00.psPartType_ls00 &&
        pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoPage_ls00      == false)
    {
        if ( pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoCodeType_ls00 != ctUnknown_els00 )
        {
            rc = ls05AreEncodingsCompatible(pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoCodeType_ls00,
                                            ls00IsDBUnicode(pDBSession) ? ctUCS2_els00 : ctAscii_els00);
            if (errOK_els00 != rc)
                pMsgInstance->ls98Msg(layDataLoad_els98, ErrText, errIncompatibleEncodings_els98);
        }
    }
    //*
    //*  init transformation and relational model
    //*
    if (errOK_els00 == rc)
    {
         if ( NULL == m_pTransformationDef )
              m_pTransformationDef = new TransformationDef(pDBSession->dbiOdbcConnection, this, pTransformationCmd );
         if ( NULL == m_pRelationalDef     )
              m_pRelationalDef     = new RelationalDef    (pDBSession->dbiOdbcConnection, this, pTransformationCmd);
         if ( NULL ==  m_pUserConnectionDef )
              m_pUserConnectionDef = new UserConnectionDef(pDBSession->dbiOdbcConnection, this, pTransformationCmd );
    }
    return rc;
}
/*===========================================================================*
 *  MetaDataDef::readLine
 *===========================================================================*/
SAPDB_Bool MetaDataDef::readLine( StudioOAL_String &record  )
{
    SAPDB_Int4 rc = ls30VFReadLnEx(m_FileSession.vFile, m_ReadBuffer, m_ReadWriteErrText, 0);
    if (rc > 0)
    {
        record = StudioOAL_String( (SAPDB_UTF8* )m_ReadBuffer );
        m_pError->isError(SAPDB_FALSE);
        m_pError->setReturnCode(errOK_els00);
    }
    else
    {
        if (EOF_LS00 != rc)
        {
            m_pError->isError(SAPDB_TRUE);
            m_pError->setReturnCode(rc);
            m_pError->setErrorText(m_ReadWriteErrText);
        }
        if (EOF_LS00 == rc)
        {
             m_pError->isError(SAPDB_FALSE);
             m_pError->setReturnCode(errOK_els00);
        }
    }
    return (rc > 0);
}
/*===========================================================================*
 *     MetaDataDef::writeLine
 *===========================================================================*/
SAPDB_Bool MetaDataDef::writeLine (StudioOAL_String& record )
{
    SAPDB_Int4 rc = ls30VFWriteLn(m_FileSession.vFile,
                                  CONST_CAST(SAPDB_Char*, record.CharPtr()),
                                  record.BasisSize(),
                                  m_ReadWriteErrText);
    if (errOK_els00 != rc)
    {
        m_pError->isError(SAPDB_TRUE);
        m_pError->setReturnCode(rc);
        m_pError->setErrorText(m_ReadWriteErrText);
    }
    record.Erase();
    return (rc > 0);
}
/*===========================================================================*
 *   ResultSet::ResultSet
 *===========================================================================*/
ResultSet::ResultSet( sapdbwa_DBC & odbcConnection )
          :StudioOAL_WResult ( odbcConnection.HDBC(),odbcConnection.HENV() )
{
}
/*===========================================================================*
 *    ResultSet::~ResultSet
 *===========================================================================*/
ResultSet::~ResultSet ()
{
}
/*===========================================================================*
 *    MetaDataItem::MetaDataItem()
 *===========================================================================*/
MetaDataItem::MetaDataItem( sapdbwa_DBC & odbcConnection, MetaDataDef  * pMetaData , tls00_TransformationCmd * pTransformationCmd )
             :ResultSet( odbcConnection ),m_pMetaDataDef( pMetaData ), m_pTransformationCmd( pTransformationCmd )
{
    ROUTINE_DBG_MEO00 ("MetaDataItem::MetaDataItem()");

    tsp100_VersionResult vres = ls15CheckKernelVersion(getMetaData()->getConnection(), 7, 6, 0, 0);

    if ((IsEqual_esp100 == vres || IsNewer_esp100 == vres)  )
         m_bSchema = true;
    else
         m_bSchema = false;

    vres = ls15CheckKernelVersion(getMetaData()->getConnection(), 7, 6, 0, 26);

    if ((IsEqual_esp100 == vres || IsNewer_esp100 == vres)  )
         m_bSchemaMap = true;
    else
         m_bSchemaMap = false;
}
/*===========================================================================*
 *    MetaDataItem::MetaDataItem()
 *===========================================================================*/
MetaDataItem::MetaDataItem( sapdbwa_DBC & odbcConnection )
             :ResultSet( odbcConnection )
{
    ROUTINE_DBG_MEO00 ("MetaDataItem::MetaDataItem()");
    m_pMetaDataDef = NULL;
}
/*===========================================================================*
 *    MetaDataItem::~MetaDataItem()
 *===========================================================================*/
MetaDataItem::~MetaDataItem()
{
    ROUTINE_DBG_MEO00 ("MetaDataItem::~MetaDataItem()");
}
/*===========================================================================*
 *     MetaDataItem::executeSQL
 *===========================================================================*/
SAPDB_Bool MetaDataItem::executeSQL (StudioOAL_String& statement, SAPDB_Bool internal)
{
    ROUTINE_DBG_MEO00 ("MetaDataItem::executeSQL()");

    SAPDB_Bool isOk = SAPDB_TRUE;

    if ( SAPDB_TRUE == openResult(statement) )
    {
        if (NULL != getMetaData())
        {
            getMetaData()->m_pError->isError(SAPDB_FALSE);
        }
    }
    else
    {
        StudioOAL_String errorText = getErrorText();
        if (NULL != getMetaData())
        {
            ErrorHndl* pErrorObj = getMetaData()->m_pError;
            if (NULL != pErrorObj)
            {
                pErrorObj->setErrorText(errorText);
                pErrorObj->setReturnCode(getReturnCode());
                pErrorObj->setNativeError(getNativeError());
                pErrorObj->isError(SAPDB_TRUE);
            }
        }
        if (false == internal)
        {
            LoaderMessages *pMsg = LoaderMessages::Instance();
            pMsg->ls98PrintMessage(layCatalogExtract_els98, msgSimpleMsg_els98 , statement.StrPtr() );
            pMsg->ls98PrintMessage(layCatalogExtract_els98, errODBCErrors_els98, errorText.StrPtr() );
        }
        isOk = SAPDB_FALSE;
    }
    return isOk;
}
/*===========================================================================*
 *     MetaDataItem::executeSQL 
 *===========================================================================*/
SAPDB_Bool MetaDataItem::executeSQL (const char* statement, SAPDB_Bool internal)
{
    ROUTINE_DBG_MEO00 ("MetaDataItem::executeSQL() II");
    // EVERY STRING coming here IS ALREADY UTF-8 ENCODED !!!
    StudioOAL_String stmt = REINTERPRET_CAST(const SAPDB_UTF8*, statement);
    return executeSQL(stmt, internal);
}
/*===========================================================================*
 *    MetaDataItem::Statement
 *===========================================================================*/
StudioOAL_String MetaDataItem::Statement(const StatementNumbers StatementNumber)
{
    ROUTINE_DBG_MEO00 ("MetaDataItem::Statement");

    MetaDataDef*      pMetaData       = getMetaData();
    return  pMetaData->Statement(StatementNumber);
}

/*===========================================================================*
 *     MetaDataItem::existsSchemaMap
 *===========================================================================*/
SAPDB_Int4 MetaDataItem::existsSchemaMap (StudioOAL_String schemamap , tsp00_Addr ErrText)
{
    ROUTINE_DBG_MEO00 ("MetaDataItem::existsSchemaMap");

    MetaDataDef*      pMetaData = getMetaData();
    tls00_DBInfo*     pDBInfo   = pMetaData->getConnection();

    tsp100_VersionResult vres = ls15CheckKernelVersion(getMetaData()->getConnection(), 7, 6, 0, 26);

    SAPDB_Int4 exists = 0;
    SAPDB_Bool _bExists = SAPDB_TRUE;

    if ((IsEqual_esp100 == vres || IsNewer_esp100 == vres) && (0 != schemamap.Compare("ID")) )
    {
        char       szStatement[1024];
        sp77sprintf(szStatement, 1024,  "SELECT 1 FROM SYSLOADER.MAPSCHEMANAMES WHERE MAPPINGNAME = '%s' ",
                    schemamap.StrPtr() );
        if ( executeSQL(szStatement, true)) {
            _bExists = !noRows();
        }
    }
    return _bExists;
}

/*===========================================================================*
 *     MetaDataItem::existsSchemaMapDoubles
 *===========================================================================*/
SAPDB_Int4 MetaDataItem::existsSchemaMapDoubles( StudioOAL_String packageguid, StudioOAL_String schemamap, tsp00_Addr ErrText )
{
    ROUTINE_DBG_MEO00 ("MetaDataItem::existsSchemaMapDoubles");

    MetaDataDef*     pMetaData    = getMetaData();
    tls00_DBInfo*    pDBInfo      = pMetaData->getConnection();
    LoaderMessages*  pMsgInstance = LoaderMessages::Instance();

    tsp100_VersionResult vres = ls15CheckKernelVersion(pDBInfo, 7, 6, 0, 26);

    SAPDB_Int4 exists = 0;
    tsp00_Int4  rc = errOK_els00;
    SAPDB_Bool _bExists = SAPDB_FALSE;

    if ((IsEqual_esp100 == vres || IsNewer_esp100 == vres) )
    {
        char      szStatement[1024];
        StudioOAL_String  record[4];
        sp77sprintf(szStatement, 1024, CONST_CAST(SAPDB_Char*, Statement(sel_Transformation_722).CharPtr()),
                    packageguid.StrPtr(), packageguid.StrPtr(), schemamap.StrPtr(), schemamap.StrPtr()  );
        if ( executeSQL (szStatement,true) ) {
             StudioOAL_String  MapSchemas= "";
            _bExists = !noRows();
            if ( _bExists ) {
                 while( readResult(record)) {
                        MapSchemas +=  record[2];
                        MapSchemas += " TO ";
                        MapSchemas +=  record[3];
                        MapSchemas += ", ";
               }
               pMsgInstance->ls98Msg(layCatalogExtract_els98, ErrText, errSchemaMapDefinition, schemamap.StrPtr(), MapSchemas.StrPtr());
               rc = errSchemaMapDefinition;
            }
        }//if ( executeSQL(szStatement) )
    }
    return rc;
}

/*===========================================================================*
 *     MetaDataItem::existsSchemaMapSchemas
 *===========================================================================*/
SAPDB_Int4 MetaDataItem::existsSchemaMapSchemas (StudioOAL_String schemamap, tsp00_Addr ErrText )
{
    ROUTINE_DBG_MEO00 ("MetaDataItem::existsSchemaMapSchemas");
    LoaderMessages*   pMsgInstance = LoaderMessages::Instance();
    SAPDB_Int4        rc = errOK_els00;

    MetaDataDef*      pMetaData = getMetaData();
    tls00_DBInfo*     pDBInfo   = pMetaData->getConnection();

    tsp100_VersionResult vres = ls15CheckKernelVersion(pDBInfo, 7, 6, 0, 26);

    if ((IsEqual_esp100 == vres || IsNewer_esp100 == vres) ) {
         StudioOAL_String  statement;
         _TCHAR            szStatement[2048];
         szStatement[0]    = 0; 
         StudioOAL_String  record[2];
         StudioOAL_String  SourceSchemas= "";
         StudioOAL_String  TargetSchemas= "";
    
         sp77sprintf(szStatement, 2048, CONST_CAST(SAPDB_Char*, Statement(exist_Transformation_113).CharPtr()), schemamap.StrPtr());
         statement.Assign(REINTERPRET_CAST(SAPDB_UTF8*, szStatement));
    
         if ( executeSQL(szStatement) ) {
              StudioOAL_String  SourceSchemas= "";
              StudioOAL_String  TargetSchemas= "";
              while ( readResult( record ) ) {
                    if (!existsSchema( record[0],ErrText)) {
                        SourceSchemas += " ";
                        SourceSchemas += record[0];
                        rc = errSchemaMapSchemasNotExists_els98;
                    }
                    if (!existsSchema( record[1],ErrText)) {
                        TargetSchemas += " ";
                        TargetSchemas += record[1] ;
                        rc = errSchemaMapSchemasNotExists_els98;
                    }
              }
              if ( rc == errSchemaMapSchemasNotExists_els98 ) {
                   SourceSchemas += TargetSchemas;
                   pMsgInstance->ls98Msg(layCatalogExtract_els98, ErrText, errSchemaMapSchemasNotExists_els98, SourceSchemas.StrPtr(), 
                                         schemamap.StrPtr());
              }
         }//if ( executeSQL(szStatement) )
     }
    return rc ;
}
/*===========================================================================*
 *     MetaDataItem::existsSchemaPrivileges
 *===========================================================================*/
SAPDB_Int4 MetaDataItem::existsSchemaPrivileges (StudioOAL_String username, StudioOAL_String schemaname, tsp00_Addr ErrText )
{
    ROUTINE_DBG_MEO00 ("MetaDataItem::existsSchemaPrivileges");

    MetaDataDef*      pMetaData = getMetaData();
    tls00_DBInfo*     pDBInfo   = pMetaData->getConnection();

    tsp100_VersionResult vres = ls15CheckKernelVersion(pDBInfo, 7, 6, 0, 0);

    SAPDB_Int4 exists = 0;
    SAPDB_Bool _bExists = SAPDB_FALSE;

    if ((IsEqual_esp100 == vres || IsNewer_esp100 == vres) ) {
         
         char       szStatement[1024];
         sp77sprintf(szStatement, 1024, 
                     "SELECT 1 FROM DOMAIN.SCHEMAPRIVILEGES WHERE CREATEIN IN ('YES') AND GRANTEE IN ('PUBLIC','%s') AND SCHEMANAME = '%s' " ,
                     username.StrPtr(), schemaname.StrPtr() );
         if ( executeSQL(szStatement, true)) {
             _bExists = !noRows();
         }
    }
    return _bExists;
}

/*===========================================================================*
 *     MetaDataItem::existsSchema
 *===========================================================================*/
SAPDB_Int4 MetaDataItem::existsSchema (StudioOAL_String schemaname, tsp00_Addr ErrText )
{
    ROUTINE_DBG_MEO00 ("MetaDataItem::existsSchema");

    MetaDataDef*      pMetaData = getMetaData();
    tls00_DBInfo*     pDBInfo   = pMetaData->getConnection();

    tsp100_VersionResult vres = ls15CheckKernelVersion(pDBInfo, 7, 6, 0, 0);

    SAPDB_Int4 exists = 0;
    SAPDB_Bool _bExists = SAPDB_FALSE;

    if ((IsEqual_esp100 == vres || IsNewer_esp100 == vres) )
    {
        char       szStatement[1024];
        sp77sprintf(szStatement, 1024,  "SELECT 1 FROM DOMAIN.SCHEMAS WHERE SCHEMANAME = '%s' ",
                    schemaname.StrPtr() );
        if ( executeSQL(szStatement, true)) {
            _bExists = !noRows();
        }
    }
    return _bExists;
}
/*===========================================================================*
 *     MetaDataItem::existsTable
 *===========================================================================*/
SAPDB_Bool MetaDataItem::existsTable (StudioOAL_String tablename )
{
    ROUTINE_DBG_MEO00 ("MetaDataItem::existsTable");
    SAPDB_Bool isOk = SAPDB_TRUE;

    StudioOAL_String exists = " EXISTS TABLE ";

    exists += tablename;

    if ( !openResult (exists) )
    {
        StudioOAL_String nativeError;

        nativeError = getNativeErrorAsString();

        if ( nativeError.Compare("-4004") == 0 )
             isOk = SAPDB_FALSE;
    }
    return  isOk;
}

/*===========================================================================*
 *     MetaDataItem::existsColumn
 *===========================================================================*/
SAPDB_Bool MetaDataItem::existsColumn(StudioOAL_String& TableName, StudioOAL_String& ColumnName)
{
  ROUTINE_DBG_MEO00 ("MetaDataItem::existsColumn");

  char       szStatement[1024];
  SAPDB_Bool _bExists = SAPDB_FALSE;

  sp77sprintf(szStatement, 1024, "SELECT 1 FROM COLUMNS WHERE TABLENAME = '%s' AND COLUMNNAME = '%s'",
                                 TableName.StrPtr(), ColumnName.StrPtr());
  if ( executeSQL(szStatement, true)) {
      _bExists = !noRows();
  }
  return _bExists;
}

/*===========================================================================*
 *     MetaDataItem::existsProc
 *===========================================================================*/
SAPDB_Bool MetaDataItem::existsProc (StudioOAL_String dbprocname )
{
  ROUTINE_DBG_MEO00 ("MetaDataItem::existsProc");

  tls00_DBInfo*     pDBInfo   = getMetaData()->getConnection();
  StudioOAL_String  statement;
  StudioOAL_String  where;
  _TCHAR            szStatement[2048];
  szStatement[0]    = 0; 
  StudioOAL_String  record[1];

  SAPDB_Int4 exists = 0;

  StudioOAL_String  dbaUser("SYSDD");

  if (pDBInfo->dbiSession_ls00.siInstallOn_ls00 == true)
  {
      where   = " WHERE DBPROCEDURE = '";
      where  += dbprocname;
      where  += "'";
  }
  else
  {
      dbaUser = "DOMAIN";
      where   = " WHERE DBPROCNAME = '";
      where  += dbprocname;
      where  += "'";
  }
  sp77sprintf(szStatement, 2048, CONST_CAST(SAPDB_Char*, Statement(exist_Transformation_108).CharPtr()), dbaUser.StrPtr(), where.StrPtr());
  statement.Assign(REINTERPRET_CAST(SAPDB_UTF8*, szStatement));

  if ( executeSQL( statement ) )
  {
      readResult( record ) ;
      exists = AtoI( record[0] );
  }
  return (exists > 0);
}

/*===========================================================================*
 *     MetaDataItem::readResult                                        *
 *===========================================================================*/
SAPDB_Bool MetaDataItem::readResult (StudioOAL_String *record)
{
   ROUTINE_DBG_MEO00 ("MetaDataItem::readResult()");

   SAPDB_Int2       i;
   StudioOAL_String  s;

  /*-------------------------------------------------------------------------*
   * Fetch the next record.                                                  *
   *-------------------------------------------------------------------------*/
  if (!fetch())
  {
    closeResult();
    return SAPDB_FALSE;
  }

  /*-------------------------------------------------------------------------*
   * If there are no more records, reading the result is finished.           *
   *-------------------------------------------------------------------------*/
  if ( noRows())
  {
    closeResult();
    return SAPDB_FALSE;
  }
  /*-------------------------------------------------------------------------*
   * Read all columns of the fetched record.                                 *
   *-------------------------------------------------------------------------*/
  for (i = 0; i < getColCount(); i++)
  {
    getColumnDataAsString (i + 1, s);
    record[i] = s;
  }
  return SAPDB_TRUE;
}
/*===========================================================================*
 *     MetaDataItem::writeLine
 *===========================================================================*/
SAPDB_Bool MetaDataItem::writeLine (StudioOAL_String& record )
{
    return getMetaData()->writeLine(record);
}
/*===========================================================================*
 *     MetaDataItem::
 *===========================================================================*/
StudioOAL_String MetaDataItem::string2XML( StudioOAL_String string )
{
   StudioOAL_String  patterns((SAPDB_UTF8*)"&\"'<>");
   StudioOAL_String  constraintexpression;

   constraintexpression += string;
   SAPDB_UInt  start = 0, end ;

   while ( (end = constraintexpression.FindFirstOf (patterns, start) ) != StudioOAL_String::NPos )
   {
       switch ( constraintexpression[end] )
       {
           case ( '&' ):
               {
               constraintexpression.Replace(end,1,"&amp;");
               end += 5;
               break;
               }
           case ( '\"' ):
               {
               constraintexpression.Replace(end,1,"&quot;");
               end += 6;
               break;
               }
           case ( '\'' ):
               {
               constraintexpression.Replace(end,1,"&apos;");
               end += 6;
               break;
               }
           case ( '<' ):
               {
               constraintexpression.Replace(end,1,"&lt;");
               end += 4;
               break;
               }
           case ( '>' ):
               {
               constraintexpression.Replace(end,1,"&gt;");
               end += 4;
               break;
               }

           default:
               break;
       }
       start = end + 1;
   }
   return constraintexpression;
}
/*===========================================================================*
 *     MetaDataItem::string2SQL
 *===========================================================================*/
StudioOAL_String& MetaDataItem::string2SQL(StudioOAL_String& string)
{
    StudioOAL_String  patterns((SAPDB_UTF8*)"'");

    SAPDB_UInt  start = 0;
    SAPDB_UInt  end   = 0;

    while ( StudioOAL_String::NPos != (end = string.FindFirstOf(patterns, start) ) )
    {
        if ('\'' == string[end])
        {
            string.Replace(end, 1, "\'\'");
            end += 1;
        }
        start = end + 1;
    }
    return string;
}
/*===========================================================================*
 *    RelationalDef::RelationalDef
 *===========================================================================*/
RelationalDef::RelationalDef( sapdbwa_DBC & odbcConnection,  MetaDataDef * pMetaData, tls00_TransformationCmd *pTransformationCmd ) 
              :MetaDataItem ( odbcConnection, pMetaData, pTransformationCmd ),
               m_nTableCount(0) , m_nSynonymCount(0)  , m_nViewCount(0),
               m_pUserDef (NULL), m_pRoleDef    (NULL), m_pSchemaDef  (NULL), m_pTableDef(NULL), m_pColumnTypeDef(NULL), 
               m_pViewDef (NULL), m_pSynonymDef (NULL), m_pTriggerDef (NULL), m_pProcedureDef (NULL),
               m_pIndexDef(NULL), m_pSequenceDef(NULL), m_bSplitPackage(SAPDB_FALSE)
{
    ROUTINE_DBG_MEO00 ("RelationalDef::RelationalDef()");
    m_pRelationalStmt[0] = 0;
    if (pTransformationCmd->trTable_ls00.tsTabOwner_ls00.length() != 0 )
        m_owner = UTF8(pTransformationCmd->trTable_ls00.tsTabOwner_ls00.asCharp());
    else
        m_owner = UTF8(pMetaData->getConnection()->dbiSession_ls00.siUserName_ls00.asCharp());

    if (pTransformationCmd->trTable_ls00.tsTabSchema_ls00.length() != 0 )
        m_schemaname = UTF8(pTransformationCmd->trTable_ls00.tsTabSchema_ls00.asCharp());
    else 
        m_schemaname = UTF8(pMetaData->getConnection()->dbiSession_ls00.siSchemaName_ls00);

    if( pTransformationCmd->trTable_ls00.tsTabName_ls00.length() != 0 )
        m_tablename = UTF8(pTransformationCmd->trTable_ls00.tsTabName_ls00.asCharp());
    
    if ( NULL ==  m_pUserDef)
         m_pUserDef         = new UserDef      ( odbcConnection, pMetaData, pTransformationCmd );
    if ( NULL ==  m_pRoleDef)
         m_pRoleDef         = new RoleDef      ( odbcConnection, pMetaData, pTransformationCmd );
    if ( NULL ==  m_pSchemaDef)
         m_pSchemaDef       = new SchemaDef    ( odbcConnection, pMetaData, pTransformationCmd );
    if ( NULL ==  m_pTableDef)
         m_pTableDef        = new TableDef     ( odbcConnection, pMetaData, pTransformationCmd );
    if ( NULL == m_pViewDef )
         m_pViewDef         = new ViewDef      ( odbcConnection, pMetaData, pTransformationCmd );
    if ( NULL ==  m_pSynonymDef)
         m_pSynonymDef      = new SynonymDef   ( odbcConnection, pMetaData, pTransformationCmd );
    if ( NULL ==  m_pTriggerDef )
         m_pTriggerDef      = new TriggerDef   ( odbcConnection, pMetaData, pTransformationCmd );
    if ( NULL == m_pProcedureDef )
         m_pProcedureDef    = new ProcedureDef ( odbcConnection, pMetaData, pTransformationCmd );
    if ( NULL ==  m_pColumnTypeDef )
         m_pColumnTypeDef   = new ColumnTypeDef( odbcConnection, pMetaData, pTransformationCmd );
    if ( NULL == m_pIndexDef )
         m_pIndexDef        = new IndexDef     ( odbcConnection, pMetaData, pTransformationCmd );
    if ( NULL == m_pSequenceDef )
         m_pSequenceDef     = new SequenceDef  ( odbcConnection, pMetaData, pTransformationCmd );

#ifdef ODBC_PREPARED
    if (SAPDB_TRUE == rc)
        rc = m_pTransformationDef->initResultSet(odbcConnection);
    if (SAPDB_TRUE == rc)
    {
        if ( m_pTransformationCmd->trCatalogFile_ls00.fsFormat_ls00.ffoDdl_ls00 == true || 
             m_pTransformationCmd->trCatalogFile_ls00.fsFormat_ls00.ffoXml_ls00 == true ) {
             rc = m_pRelationalDefImport->initResultSet(odbcConnection);
        }
        else
        {
            if (SAPDB_TRUE == rc)
                rc                 = m_pUserDef->initResultSet(odbcConnection);
            if (SAPDB_TRUE == rc)
                rc                 = m_pRoleDef->initResultSet(odbcConnection);
            if (SAPDB_TRUE == rc)
                rc                 = m_pTableDef->initResultSet(odbcConnection);
            if (SAPDB_TRUE == rc)
                rc                 = m_pViewDef->initResultSet(odbcConnection);
            if (SAPDB_TRUE == rc)
                rc                 = m_pSynonymDef->initResultSet(odbcConnection);
            if (SAPDB_TRUE == rc)
                rc                 = m_pTriggerDef->initResultSet(odbcConnection);
            if (SAPDB_TRUE == rc)
                rc                 = m_pProcedureDef->initResultSet(odbcConnection);
            if (SAPDB_TRUE == rc)
                rc                 = m_pColumnTypeDef->initResultSet(odbcConnection);
            if (SAPDB_TRUE == rc)
                rc                 = m_pIndexDef->initResultSet(odbcConnection);
            if (SAPDB_TRUE == rc)
                rc                 = m_pSequenceDef->initResultSet(odbcConnection);
        }
    }
#endif
}
/*===========================================================================*
 *     RelationalDef::initResultSet
 *===========================================================================*/
SAPDB_Bool RelationalDef::initResultSet ( sapdbwa_DBC & odbcConnection )
{
    SAPDB_Bool isOk = SAPDB_FALSE;

    if ( odbcConnection.HDBC() && odbcConnection.HENV() )
         isOk = SAPDB_TRUE;
    return isOk;
}
/*===========================================================================*
 *    RelationalDef::~RelationalDef
 *===========================================================================*/
RelationalDef::~RelationalDef()
{
    if ( NULL != m_pUserDef )
    {
        delete m_pUserDef;
        m_pUserDef = NULL;
    }
    if ( NULL != m_pRoleDef )
    {
        delete m_pRoleDef;
        m_pRoleDef = NULL;
    }
    if( NULL != m_pTableDef )
    {
        delete m_pTableDef;
        m_pTableDef = NULL;
    }
    if( NULL != m_pViewDef )
    {
        delete m_pViewDef;
        m_pViewDef  = NULL;
    }
    if( NULL != m_pSynonymDef )
    {
        delete m_pSynonymDef;
        m_pSynonymDef = NULL;
    }
    if( NULL != m_pTriggerDef )
    {
        delete m_pTriggerDef;
        m_pTriggerDef = NULL;
    }
    if( NULL != m_pColumnTypeDef )
    {
        delete m_pColumnTypeDef;
        m_pColumnTypeDef = NULL;
    }
    if( NULL != m_pIndexDef )
    {
        delete m_pIndexDef ;
        m_pIndexDef = NULL;
    }
    if( NULL != m_pSequenceDef )
    {
        delete m_pSequenceDef;
        m_pSequenceDef = NULL;
    }
    if( NULL != m_pProcedureDef )
    {
        delete m_pProcedureDef;
        m_pProcedureDef = NULL;
    }
    if( NULL != m_pTriggerDef )
    {
        delete m_pTriggerDef;
        m_pTriggerDef = NULL;
    }
}
/*===========================================================================*
 *     RelationalDef::TransformHeader
 *===========================================================================*/
SAPDB_Bool RelationalDef::TransformHeader ()
{
    SAPDB_Bool isOk = SAPDB_TRUE;
    return isOk;
}
/*===========================================================================*
 *     RelationalDef::SplitPackage
 *===========================================================================*/
SAPDB_Bool RelationalDef::SplitPackage()
{
    if ( tsExport_ls00 == m_pTransformationCmd->trTransSpec_ls00 )
    {
        if( NULL          != m_pTransformationCmd->trCatalogFile_ls00.fsFileName_ls00 &&
            ptTable_els00  < m_pTransformationCmd->trPartSpec_ls00.psPartType_ls00 && m_bSplitPackage == SAPDB_TRUE )
            m_bSplitPackage  = SAPDB_TRUE;
        else
            m_bSplitPackage  = SAPDB_FALSE;
    }
    else if ( tsImport_ls00 == m_pTransformationCmd->trTransSpec_ls00 )
    {
         if( NULL           != m_pTransformationCmd->trCatalogFile_ls00.fsFileName_ls00 &&
             ptTable_els00   < m_pTransformationCmd->trPartSpec_ls00.psPartType_ls00    &&
			 dtypeFile_ls00 == m_pTransformationCmd->trCatalogFile_ls00.fsDeviceType_ls00 )
         {
             m_bSplitPackage  = SAPDB_FALSE;
             StudioOAL_String catalogfilename(m_pTransformationCmd->trCatalogFile_ls00.fsFileName_ls00);
             catalogfilename += "0001";
             tls00_LargeNumber ignoreFileSize;
             ignoreFileSize.lnu2GB_Blocks_ls00 = 0;
             ignoreFileSize.lnuRemainder_ls00  = 0;
             if (errOK_els00 == ls30VFGetFileSize( CONST_CAST(SAPDB_Char*, catalogfilename.CharPtr()), &ignoreFileSize))
             {
                 if ( (0 != ignoreFileSize.lnuRemainder_ls00) || (0 != ignoreFileSize.lnu2GB_Blocks_ls00) )
                 {
                     m_bSplitPackage  = SAPDB_TRUE;
                     delete [] m_pTransformationCmd->trCatalogFile_ls00.fsFileName_ls00;
                     m_pTransformationCmd->trCatalogFile_ls00.fsFileName_ls00 = new char [catalogfilename.BasisSize()+1];
                     sp77sprintf(m_pTransformationCmd->trCatalogFile_ls00.fsFileName_ls00, PATH_MXSP00, "%s", CONST_CAST(SAPDB_Char*, catalogfilename.CharPtr()) );
                 }
             }
         }
    }
    return m_bSplitPackage;
}
/*===========================================================================*
 *     RelationalDef::ExistPackage
 *===========================================================================*/
SAPDB_Bool RelationalDef::ExistPackage()
{
    TransformationDef  *pTransformationDef = getMetaData()->getTransformationModel();
    return  pTransformationDef->ExistPackage(m_pTransformationCmd->trPackageid_ls00);
}
/*===========================================================================*
 *     RelationalDef::ExistPackageData
 *===========================================================================*/
SAPDB_Bool RelationalDef::ExistPackageData( )
{
    TransformationDef  *pTransformationDef = getMetaData()->getTransformationModel();
    return  pTransformationDef->ExistPackageData(m_pTransformationCmd->trPackageid_ls00);
}
/*===========================================================================*
 *     RelationalDef::TransformTrailer
 *===========================================================================*/
SAPDB_Bool RelationalDef::TransformTrailer ()
{
    SAPDB_Bool isOk = SAPDB_FALSE;

    TransformationDef  *pTransformationDef = getMetaData()->getTransformationModel();
    StudioOAL_String catalogObjects = "";

    if (true == m_pTransformationCmd->trCatalogFile_ls00.fsFormat_ls00.ffoXml_ls00 )
    {
        if ( pTransformationDef->m_nObjectCount != 0 )
        {
            catalogObjects  = "</uml:NamespaceOwnedElement>\n"
                              "</sdbm:DeployedSchema>\n"
                              "</uml:NamespaceOwnedElement>\n";
        }
        catalogObjects +=     "</dbm:CatalogSchemas>\n"
                              "</dbm:DeployedCatalog>\n"
                              "</dbm:DataSourceDeployedCatalogs>\n"
                              "</dbm:DataSource>\n"
                              "</uml:ModelPackages>\n"
                              "</uml:Model>\n"
                              "</uml:SystemModels>\n"
                              "</uml:System>\n"
                              "</oim:Transfer>\n";
    }
    else if (true == m_pTransformationCmd->trCatalogFile_ls00.fsFormat_ls00.ffoDdl_ls00) {
        catalogObjects = SEPARATOR;
    }

    return getMetaData()->writeLine ( catalogObjects );
}
/*===========================================================================*
 *  RelationalDef::ExportPackageDb                                          *
 *===========================================================================*/
SAPDB_Int4 RelationalDef::ExportPackageDb( StudioOAL_String& username, tsp00_Addr ErrText )
{
     ROUTINE_DBG_MEO00 ("RelationalDef::ExportPackageDb");
     LoaderMessages *pMsgInstance = LoaderMessages::Instance();
     SAPDB_Int4 rc = errOK_els00;

     StudioOAL_String  currentuser;
     StudioOAL_String  ErrorText;

     TransformationDef  *pTransformationDef = getMetaData()->getTransformationModel();

     StudioOAL_String sessionUser, currentUser, sessionSchema, currentSchema;
     if (errOK_els00 == rc)
     {
         sessionUser.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer, getMetaData()->getConnection()->dbiSession_ls00.siUserName_ls00.asCharp()));
         pTransformationDef->m_owner      = currentUser   = sessionUser;
         sessionSchema.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer, getMetaData()->getConnection()->dbiSession_ls00.siSchemaName_ls00));
         pTransformationDef->m_schemaname = currentSchema = sessionSchema;
     }
     //*
     //*  catalog objects before exporting data
     //*
     if ( ExistPackageData() == SAPDB_FALSE ||
          m_pTransformationCmd->trDataFile_ls00.fsFileName_ls00 == NULL)
     {
          //*
          //*  connection object
          //*
          if ( errOK_els00 != TransformConnection()) {
              ErrorText = getErrorText();
              ErrText   = CONST_CAST(SAPDB_Char*, ErrorText.CharPtr());
          }
          if ( errOK_els00 != TransformUsers()) {
              ErrorText = getErrorText();
              ErrText   = CONST_CAST(SAPDB_Char*, ErrorText.CharPtr());
          }
          if ( errOK_els00 != TransformRoles( )) {
              ErrorText = getErrorText();
              ErrText   = CONST_CAST(SAPDB_Char*, ErrorText.CharPtr());
          }
		  if ( errOK_els00 != TransformSchemas()) {
              ErrorText = getErrorText();
              ErrText   = CONST_CAST(SAPDB_Char*, ErrorText.CharPtr());
          }
          if ( errOK_els00 !=  TransformSequences( )) {
              ErrorText = getErrorText();
              ErrText   = CONST_CAST(SAPDB_Char*, ErrorText.CharPtr());
          }
          if ( errOK_els00 != TransformTables( )) {
              ErrorText = getErrorText();
              ErrText   = CONST_CAST(SAPDB_Char*, ErrorText.CharPtr());
          }
     }
     //*
     //*  catalog objects after exporting data
     //*
     if ( (ExistPackageData() == SAPDB_TRUE  || SplitPackage() == SAPDB_FALSE) ||
           m_pTransformationCmd->trDataFile_ls00.fsFileName_ls00 == NULL      )
     {
          if ( errOK_els00 != TransformIndexes( ))    {
              ErrorText = getErrorText();
              ErrText   = CONST_CAST(SAPDB_Char*, ErrorText.CharPtr());
          }
          if ( errOK_els00 != TransformForeignKeys( )){
              ErrorText = getErrorText();
              ErrText   = CONST_CAST(SAPDB_Char*, ErrorText.CharPtr());
          }
          if ( errOK_els00 != TransformTableTypes( )) {
              ErrorText = getErrorText();
              ErrText   = CONST_CAST(SAPDB_Char*, ErrorText.CharPtr());
          }
          if ( errOK_els00 != TransformTriggers( )){
              ErrorText = getErrorText();
              ErrText   = CONST_CAST(SAPDB_Char*, ErrorText.CharPtr());
          }
          if ( errOK_els00 != TransformProcedures( )) {
              ErrorText = getErrorText();
              ErrText   = CONST_CAST(SAPDB_Char*, ErrorText.CharPtr());
          }
     }
     TransformTrailer();
     //*
     //* statistic of exported catalog objects
     //*
     if ( ExistPackageData() == SAPDB_FALSE )
          pMsgInstance->ls98PrintMessage(layCatalogExtract_els98, msgNumTabTransformed_els98     , m_nTableCount   );
     else
     {
          pMsgInstance->ls98PrintMessage(layCatalogExtract_els98, msgNumViewsTransformed_els98   , m_nViewCount    );
          pMsgInstance->ls98PrintMessage(layCatalogExtract_els98, msgNumSynonymsTransformed_els98, m_nSynonymCount );
     }
     //*
     //*  export transformationpackage
     //*
     pTransformationDef->m_owner      = sessionUser;
     pTransformationDef->m_schemaname = sessionSchema;
    
     if (errOK_els00 != pTransformationDef->switchUserSchema(currentUser,currentSchema,true))
         pMsgInstance->ls98PrintMessage(layCatalogLoad_els98, errODBCErrors_els98, getMetaData()->m_pError->getErrorText().StrPtr());

     return rc;
}
/*===========================================================================*
 *  RelationalDef::ExportPackageUser                                         *
 *===========================================================================*/
SAPDB_Int4 RelationalDef::ExportPackageUser(StudioOAL_String& username, tsp00_Addr ErrText)
{
     ROUTINE_DBG_MEO00 ("RelationalDef::ExportPackageUser");

     LoaderMessages *pMsgInstance = LoaderMessages::Instance();

     SAPDB_Int4 rc = errOK_els00;
     StudioOAL_String  ErrorText;

     TransformationDef  *pTransformationDef = getMetaData()->getTransformationModel();
     
     StudioOAL_String sessionUser, currentUser, sessionSchema, currentSchema;
     if (errOK_els00 == rc)
     {
         sessionUser.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer, getMetaData()->getConnection()->dbiSession_ls00.siUserName_ls00.asCharp()));
         pTransformationDef->m_owner      = currentUser   = sessionUser;
         sessionSchema.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer, getMetaData()->getConnection()->dbiSession_ls00.siSchemaName_ls00));
         pTransformationDef->m_schemaname = currentSchema = sessionSchema;
     }

     TransformHeader();
     //*
     //*  catalog objects before exporting data
     //*
     if ( ExistPackageData() == SAPDB_FALSE ||
          m_pTransformationCmd->trDataFile_ls00.fsFileName_ls00 == NULL)
     {
          //*
          //*  connection object
          //*
          if ( errOK_els00 != TransformConnection()){
              ErrorText = getErrorText();
              ErrText   = CONST_CAST(SAPDB_Char*, ErrorText.CharPtr());
          }
          if ( errOK_els00 != TransformUsers( )){
              ErrorText = getErrorText();
              ErrText   = CONST_CAST(SAPDB_Char*, ErrorText.CharPtr());
          }
          if ( errOK_els00 != TransformRoles( )){
              ErrorText = getErrorText();
              ErrText   = CONST_CAST(SAPDB_Char*, ErrorText.CharPtr());
          }
		  if ( errOK_els00 != TransformSchemas()) {
              ErrorText = getErrorText();
              ErrText   = CONST_CAST(SAPDB_Char*, ErrorText.CharPtr());
          }
          if ( errOK_els00 !=  TransformSequences( )){
              ErrorText = getErrorText();
              ErrText   = CONST_CAST(SAPDB_Char*, ErrorText.CharPtr());
          }
          if ( errOK_els00 != TransformTables( )){
              ErrorText = getErrorText();
              ErrText   = CONST_CAST(SAPDB_Char*, ErrorText.CharPtr());
          }
     }
     //*
     //*  catalog objects after exporting data
     //*
     if ( (ExistPackageData() == SAPDB_TRUE  || SplitPackage() == SAPDB_FALSE) ||
           m_pTransformationCmd->trDataFile_ls00.fsFileName_ls00 == NULL     )
     {
          if ( errOK_els00 != TransformIndexes( )) {
              ErrorText = getErrorText();
              ErrText   = CONST_CAST(SAPDB_Char*, ErrorText.CharPtr());
          }
          if ( errOK_els00 != TransformForeignKeys( )){
              ErrorText = getErrorText();
              ErrText   = CONST_CAST(SAPDB_Char*, ErrorText.CharPtr());
          }
          if ( errOK_els00 != TransformTableTypes(  )){
              ErrorText = getErrorText();
              ErrText   = CONST_CAST(SAPDB_Char*, ErrorText.CharPtr());
          }
          if ( errOK_els00 != TransformTriggers( )){
              ErrorText = getErrorText();
              ErrText   = CONST_CAST(SAPDB_Char*, ErrorText.CharPtr());
          }
          if ( errOK_els00 != TransformProcedures( )){
              ErrorText = getErrorText();
              ErrText   = CONST_CAST(SAPDB_Char*, ErrorText.CharPtr());
          }
     }
     TransformTrailer();

     pTransformationDef->m_owner      = sessionUser;
     pTransformationDef->m_schemaname = sessionSchema;
    
     if (errOK_els00 != pTransformationDef->switchUserSchema(currentUser,currentSchema,true))
         pMsgInstance->ls98PrintMessage(layCatalogLoad_els98, errODBCErrors_els98, getMetaData()->m_pError->getErrorText().StrPtr());
     //*
     //* statistic of exported catalog objects
     //*
     if ( ExistPackageData() == SAPDB_FALSE )
          pMsgInstance->ls98PrintMessage(layCatalogExtract_els98, msgNumTabTransformed_els98     , m_nTableCount   );
     else
     {
          pMsgInstance->ls98PrintMessage(layCatalogExtract_els98, msgNumViewsTransformed_els98   , m_nViewCount    );
          pMsgInstance->ls98PrintMessage(layCatalogExtract_els98, msgNumSynonymsTransformed_els98, m_nSynonymCount );
     }
     return rc;
}
/*===========================================================================*
 *  RelationalDef::ExportPackageSchema                                       *
 *===========================================================================*/
SAPDB_Int4 RelationalDef::ExportPackageSchema(StudioOAL_String& schemaname, tsp00_Addr ErrText)
{
     ROUTINE_DBG_MEO00 ("RelationalDef::ExportPackageSchema");

     LoaderMessages *pMsgInstance = LoaderMessages::Instance();

     SAPDB_Int4 rc = errOK_els00;
     StudioOAL_String  ErrorText;

     TransformationDef  *pTransformationDef = getMetaData()->getTransformationModel();

     TransformHeader();
     //*
     //*  catalog objects before exporting data
     //*
     if ( ExistPackageData() == SAPDB_FALSE ||
          m_pTransformationCmd->trDataFile_ls00.fsFileName_ls00 == NULL)
     {
          //*
          //*  connection object
          //*
          if ( errOK_els00 != TransformConnection()){
              ErrorText = getErrorText();
              ErrText   = CONST_CAST(SAPDB_Char*, ErrorText.CharPtr());
          }
          if ( errOK_els00 !=  TransformSequences( )){
              ErrorText = getErrorText();
              ErrText   = CONST_CAST(SAPDB_Char*, ErrorText.CharPtr());
          }
          if ( errOK_els00 != TransformTables( )){
              ErrorText = getErrorText();
              ErrText   = CONST_CAST(SAPDB_Char*, ErrorText.CharPtr());
          }
     }
     //*
     //*  catalog objects after exporting data
     //*
     if ( (ExistPackageData() == SAPDB_TRUE  || SplitPackage()   == SAPDB_FALSE) ||
           m_pTransformationCmd->trDataFile_ls00.fsFileName_ls00 == NULL       )
     {
          if ( errOK_els00 != TransformIndexes( )) {
              ErrorText = getErrorText();
              ErrText   = CONST_CAST(SAPDB_Char*, ErrorText.CharPtr());
          }
          if ( errOK_els00 != TransformForeignKeys( )){
              ErrorText = getErrorText();
              ErrText   = CONST_CAST(SAPDB_Char*, ErrorText.CharPtr());
          }
          if ( errOK_els00 != TransformTableTypes(  )){
              ErrorText = getErrorText();
              ErrText   = CONST_CAST(SAPDB_Char*, ErrorText.CharPtr());
          }
          if ( errOK_els00 != TransformTriggers( )){
              ErrorText = getErrorText();
              ErrText   = CONST_CAST(SAPDB_Char*, ErrorText.CharPtr());
          }
          if ( errOK_els00 != TransformProcedures( )){
              ErrorText = getErrorText();
              ErrText   = CONST_CAST(SAPDB_Char*, ErrorText.CharPtr());
          }
     }
     TransformTrailer();
     //*
     //* statistic of exported catalog objects
     //*
     if ( ExistPackageData() == SAPDB_FALSE )
          pMsgInstance->ls98PrintMessage(layCatalogExtract_els98, msgNumTabTransformed_els98     , m_nTableCount   );
     else
     {
          pMsgInstance->ls98PrintMessage(layCatalogExtract_els98, msgNumViewsTransformed_els98   , m_nViewCount    );
          pMsgInstance->ls98PrintMessage(layCatalogExtract_els98, msgNumSynonymsTransformed_els98, m_nSynonymCount );
     }
     return rc;
}
/*===========================================================================*
 *  RelationalDef::ExportPackageTable
 *===========================================================================*/
SAPDB_Int4 RelationalDef::ExportPackageTable ( )
{
    ROUTINE_DBG_MEO00 ("RelationalDef::ExportPackageTable");

    LoaderMessages *pMsgInstance = LoaderMessages::Instance();

    SAPDB_Int4 rc = errOK_els00;
    //*
    //*  connection object
    //*
    TransformConnection( );
    //*
    //* catalog objects
    //*
    TransformTables      ( );
    TransformForeignKeys ( );
    TransformTableTypes  ( );

    TransformTrailer();
    //*
    //*  statistic
    //*
    pMsgInstance->ls98PrintMessage(layCatalogExtract_els98, msgNumTabTransformed_els98     , m_nTableCount  );
    pMsgInstance->ls98PrintMessage(layCatalogExtract_els98, msgNumViewsTransformed_els98   , m_nViewCount   );
    pMsgInstance->ls98PrintMessage(layCatalogExtract_els98, msgNumSynonymsTransformed_els98, m_nSynonymCount);

    return rc;
}
/*===========================================================================*
 *  RelationalDef::ImportPackageDb                                              *
 *===========================================================================*/
SAPDB_Int4 RelationalDef::ImportPackageDb( StudioOAL_String& username )
{
    ROUTINE_DBG_MEO00 ("RelationalDef::ImportPackageDb");
    SAPDB_Int4 rc = ImportPackageUser(username);
    return rc;
}
/*===========================================================================*
 *  RelationalDef::ImportPackageUser                                           *
 *===========================================================================*/
SAPDB_Int4 RelationalDef::ImportPackageUser( StudioOAL_String& username )
{
    ROUTINE_DBG_MEO00 ("RelationalDef::ImportPackageUser");

    SAPDB_Bool rcBool = SAPDB_TRUE;
    SAPDB_Bool rcRead = SAPDB_TRUE;
    SAPDB_Int4 rc     = errOK_els00;

    StudioOAL_String  line;
    StudioOAL_String  definition;
    StudioOAL_String  _keptDefinition;
    StudioOAL_String  _kwSqlmode = "SQLMODE";
    StudioOAL_String  _kwUse     = "USE";
    StudioOAL_String  _kwUser    = "USER";
    StudioOAL_String  _kwSchema  = "SCHEMA";
    StudioOAL_String  whitespace((SAPDB_UTF8*)" ");
    StudioOAL_String  token      = CMD_SEPARATOR_TOKEN_LS00;

    SAPDB_Bool        _bSwitchNeeded = SAPDB_FALSE;
    SAPDB_Int4        _index         = 0;

    SAPDB_Int4 lMaxFailedStmt = getMetaData()->getMaxErrorCount();
    TransformationDef *pTransformationDef = getMetaData()->getTransformationModel();

    StudioOAL_String sessionUser, currentUser, sessionSchema, currentSchema;
    if (errOK_els00 == rc)
    {
        sessionUser.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer, getMetaData()->getConnection()->dbiSession_ls00.siUserName_ls00.asCharp()));
        pTransformationDef->m_owner      = currentUser   = sessionUser;
        sessionSchema.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer, getMetaData()->getConnection()->dbiSession_ls00.siSchemaName_ls00));
        pTransformationDef->m_schemaname = currentSchema = sessionSchema;
    }
    pTransformationDef->m_currentStream = stCatalog_els00;

    // read token
    while ( (errOK_els00 == rc) && (SAPDB_TRUE == (rcRead = getMetaData()->readLine ( line )) ) )
    {
        line.Trim();
        if ( 0 != token.Compare(line.SubStr(0,CMD_SEPARATOR_TOKEN_LEN_LS00) ) )
        {
            definition += NEW_LINE;
            definition += line;
        }
        else
        {
            if ( SAPDB_FALSE == definition.Empty() )
            {
                definition.Trim();
                // SQLMODE statement
                if (0 == _kwSqlmode.Compare(definition.SubStr(0,7) ) )
                {
                    definition = definition.SubStr(7, definition.Length());
                    definition.Trim();
                    pTransformationDef->setSQLMode( definition );
                }
                else
                {
                    // - USE USER   statement to CURRENT USER       <name> statement
                    // - USE SCHEMA statement to SET CURRENT_SCHEMA=<name> statement
                    if ( 0 == _kwUse.Compare(definition.SubStr(0,3)) )
                    {
                        _keptDefinition = definition;       // used for error messages

                        definition = definition.SubStr(3, definition.Length());
                        definition.Trim();
                        if ( 0 == _kwUser.Compare(definition.SubStr(0,4)) )
                        {
                            // Separate user name from definition
                            definition   = definition.SubStr(4, definition.Length());
                            definition.Trim();
                            _index       = definition.FindFirstOf(whitespace);
                            pTransformationDef->m_owner = definition.SubStr(0, _index);

                            // Separate schema name from definition (skip password and keyword SCHEMA)
                            definition = definition.SubStr(_index, definition.Length());
                            definition.Trim();

                            _index = definition.Find("SCHEMA", 0);
                            if (StudioOAL_String::NPos != _index)
                            {
                                _index += 6; 
                                definition = definition.SubStr(_index, definition.Length());
                                definition.Trim();
                                pTransformationDef->m_schemaname = definition.SubStr(0, definition.FindFirstOf(whitespace));
                            }
                            _bSwitchNeeded = SAPDB_TRUE;
                        }
                        else if ( 0 == _kwSchema.Compare(definition.SubStr(0,6)) )
                        {
                            definition = definition.SubStr(6, definition.Length());
                            definition.Trim();
                            pTransformationDef->m_schemaname = definition.SubStr(0, definition.FindFirstOf(whitespace));
                            _bSwitchNeeded = SAPDB_TRUE;
                        }
                    }
                    if (SAPDB_FALSE == _bSwitchNeeded)
                    {
                        rcBool = pTransformationDef->executeSQL( definition );
                        if ( SAPDB_FALSE == rcBool )
                        {
                            // Check count of failed statements and report last error to client
                            if (0 == --lMaxFailedStmt)
                            {
                                // Error is already logged in error object by executeSQL
                                rc = errODBCErrors_els98;
                            }
                        }
                    }
                    else
                    {
                        rc = pTransformationDef->switchUserSchema( currentUser, currentSchema);
                        if (errOK_els00 == rc )
                        {
                            currentUser   = pTransformationDef->m_owner;
                            currentSchema = pTransformationDef->m_schemaname;
                        }
                        else
                        {
                            LoaderMessages *pMsg = LoaderMessages::Instance();
                            pMsg->ls98PrintMessage(layCatalogLoad_els98, msgSimpleMsg_els98 , _keptDefinition.StrPtr() );
                            pMsg->ls98PrintMessage(layCatalogLoad_els98, errODBCErrors_els98, getMetaData()->m_pError->getErrorText().StrPtr());
                            rc = errODBCErrors_els98;
                        }
                    }
                    _bSwitchNeeded = SAPDB_FALSE;
                }
                definition.Erase();
            }//if ( SAPDB_FALSE == definition.Empty() )

        }//else of if ( 0 != token.Compare(line.SubStr(0,CMD_SEPARATOR_TOKEN_LEN_LS00) ) )
        line.Erase();
    }//while ( readLine ( line ) == SAPDB_TRUE )

    if (errOK_els00 == rc)
    {
        if ( SAPDB_FALSE == rcRead && errOK_els00 != getMetaData()->m_pError->getReturnCode() )
            rc = getMetaData()->m_pError->getReturnCode();
    }
    // Reset the connected user to the user issuing the command - in any case
    pTransformationDef->m_owner      = sessionUser;
    pTransformationDef->m_schemaname = sessionSchema;

    if (errOK_els00 != pTransformationDef->switchUserSchema(currentUser,currentSchema))
    {
        _keptDefinition = "USE USER ";
        _keptDefinition += "\"";
        _keptDefinition += currentUser;
        _keptDefinition += "\"";
        LoaderMessages *pMsg = LoaderMessages::Instance();
        pMsg->ls98PrintMessage(layCatalogLoad_els98, msgSimpleMsg_els98 , _keptDefinition.StrPtr() );
        pMsg->ls98PrintMessage(layCatalogLoad_els98, errODBCErrors_els98, getMetaData()->m_pError->getErrorText().StrPtr());
        rc = errODBCErrors_els98;
    }
    return rc;
}
/*===========================================================================*
 *  RelationalDef::ImportPackageSchema                                           *
 *===========================================================================*/
SAPDB_Int4 RelationalDef::ImportPackageSchema( StudioOAL_String& schemaname )
{
    ROUTINE_DBG_MEO00 ("RelationalDef::ImportPackageSchema");

    SAPDB_Bool rcBool = SAPDB_TRUE;
    SAPDB_Bool rcRead = SAPDB_TRUE;
    SAPDB_Int4 rc     = errOK_els00;

    StudioOAL_String  line;
    StudioOAL_String  definition;
    StudioOAL_String  _keptDefinition;
    StudioOAL_String  _kwSqlmode = "SQLMODE";
    StudioOAL_String  _kwUse     = "USE";
    StudioOAL_String  _kwUser    = "USER";
    StudioOAL_String  _kwSchema  = "SCHEMA";
    StudioOAL_String  whitespace((SAPDB_UTF8*)" ");
    StudioOAL_String  token      = CMD_SEPARATOR_TOKEN_LS00;

    SAPDB_Bool        _bSwitchNeeded = SAPDB_FALSE;
    SAPDB_Int4        _index         = 0;

    SAPDB_Int4 lMaxFailedStmt = getMetaData()->getMaxErrorCount();
    TransformationDef       *pTransformationDef = getMetaData()->getTransformationModel();
    tls00_TransformationCmd *pTransformationCmd = pTransformationDef->getTransformationCmd();

    pTransformationDef->m_currentStream = stCatalog_els00;

    StudioOAL_String sessionUser, currentUser, sessionSchema, currentSchema, Schemaname;

    if (errOK_els00 == rc)
    {
        sessionUser.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer, getMetaData()->getConnection()->dbiSession_ls00.siUserName_ls00.asCharp()));
        pTransformationDef->m_owner = currentUser = sessionUser;
        sessionSchema.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer, getMetaData()->getConnection()->dbiSession_ls00.siSchemaName_ls00));
        currentSchema = sessionSchema;
        Schemaname.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer,pTransformationCmd->trPartSpec_ls00.psTabSchema_ls00.asCharp()));
        pTransformationDef->m_schemaname = Schemaname;
        rc = pTransformationDef->switchUserSchema( currentUser, currentSchema);
    }
    // read token
    while ( (errOK_els00 == rc) && (SAPDB_TRUE == (rcRead = getMetaData()->readLine ( line )) ) )
    {
        line.Trim();
        if ( 0 != token.Compare(line.SubStr(0,CMD_SEPARATOR_TOKEN_LEN_LS00) ) )
        {
            definition += NEW_LINE;
            definition += line;
        }
        else
        {
            if ( SAPDB_FALSE == definition.Empty() )
            {
                definition.Trim();
                // SQLMODE statement
                if (0 == _kwSqlmode.Compare(definition.SubStr(0,7) ) )
                {
                    definition = definition.SubStr(7, definition.Length());
                    definition.Trim();
                    pTransformationDef->setSQLMode( definition );
                }
                else
                {
                    // - USE USER   statement to CURRENT USER       <name> statement
                    // - USE SCHEMA statement to SET CURRENT_SCHEMA=<name> statement
                    if ( 0 == _kwUse.Compare(definition.SubStr(0,3)) )
                    {
                        _keptDefinition = definition;     

                        definition = definition.SubStr(3, definition.Length());
                        definition.Trim();
                        if ( 0 == _kwUser.Compare(definition.SubStr(0,4)) )
                        {
                            // Separate user name from definition
                            definition   = definition.SubStr(4, definition.Length());
                            definition.Trim();
                            _index       = definition.FindFirstOf(whitespace);
                            pTransformationDef->m_owner = definition.SubStr(0, _index);

                            // Separate schema name from definition (skip password and keyword SCHEMA)
                            definition = definition.SubStr(_index, definition.Length());
                            definition.Trim();

                            _index = definition.Find("SCHEMA", 0);
                            if (StudioOAL_String::NPos != _index)
                            {
                                _index += 6; 
                                definition = definition.SubStr(_index, definition.Length());
                                definition.Trim();
                                pTransformationDef->m_schemaname = definition.SubStr(0, definition.FindFirstOf(whitespace));
                            }
                            _bSwitchNeeded = SAPDB_TRUE;
                        }
                        else if ( 0 == _kwSchema.Compare(definition.SubStr(0,6)) )
                        {
                            definition = definition.SubStr(6, definition.Length());
                            definition.Trim();
                            pTransformationDef->m_schemaname = definition.SubStr(0, definition.FindFirstOf(whitespace));
                            _bSwitchNeeded = SAPDB_TRUE;
                        }
                    }
                    if (SAPDB_FALSE == _bSwitchNeeded)
                    {
                        rcBool = pTransformationDef->executeSQL( definition );
                        if ( SAPDB_FALSE == rcBool )
                        {
                            if (0 == --lMaxFailedStmt) {
                                rc = errODBCErrors_els98;
                            }
                        }
                    }
                    _bSwitchNeeded = SAPDB_FALSE;
                }
                definition.Erase();
            }// if ( SAPDB_FALSE == definition.Empty() )
        } 
        line.Erase();
    }// while ( readLine ( line ) == SAPDB_TRUE )

    // Reset the connected user to the user issuing the command - in any case
    pTransformationDef->m_owner      = sessionUser;
    pTransformationDef->m_schemaname = sessionSchema;

    if (errOK_els00 != pTransformationDef->switchUserSchema(currentUser,currentSchema))
    {
        _keptDefinition = "USE USER ";
        _keptDefinition += "\"";
        _keptDefinition += currentUser;
        _keptDefinition += "\"";
        LoaderMessages *pMsg = LoaderMessages::Instance();
        pMsg->ls98PrintMessage(layCatalogLoad_els98, msgSimpleMsg_els98 , _keptDefinition.StrPtr() );
        pMsg->ls98PrintMessage(layCatalogLoad_els98, errODBCErrors_els98, getMetaData()->m_pError->getErrorText().StrPtr());
        rc = errODBCErrors_els98;
    }

    return rc;
}
/*===========================================================================*
 *  RelationalDef::ImportPackageTable
 *===========================================================================*/
SAPDB_Int4
RelationalDef::ImportPackageTable(StudioOAL_String& schemaname )
{
    ROUTINE_DBG_MEO00 ("RelationalDef::ImportPackageTable");

    SAPDB_Bool rcBool = SAPDB_TRUE;
    SAPDB_Bool rcRead = SAPDB_TRUE;
    SAPDB_Int4 rc     = errOK_els00;

    StudioOAL_String  line;
    StudioOAL_String  definition;
    StudioOAL_String  _keptDefinition;
    StudioOAL_String  _kwSqlmode = "SQLMODE";
    StudioOAL_String  _kwUse     = "USE";
    StudioOAL_String  _kwSchema  = "SCHEMA";
    StudioOAL_String  whitespace((SAPDB_UTF8*)" ");
    StudioOAL_String  token      = CMD_SEPARATOR_TOKEN_LS00;

    SAPDB_Bool        _bSwitchNeeded = SAPDB_FALSE;
    SAPDB_Int4        _index         = 0;

    SAPDB_Int4 lMaxFailedStmt = getMetaData()->getMaxErrorCount();
    TransformationDef *pTransformationDef = getMetaData()->getTransformationModel();

    StudioOAL_String sessionUser, currentUser, sessionSchema, currentSchema;
    if (errOK_els00 == rc)
    {
        sessionUser.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer, getMetaData()->getConnection()->dbiSession_ls00.siUserName_ls00.asCharp()));
        pTransformationDef->m_owner      = currentUser   = sessionUser;
        sessionSchema.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer, getMetaData()->getConnection()->dbiSession_ls00.siSchemaName_ls00));
        pTransformationDef->m_schemaname = currentSchema = sessionSchema;
        if( isSchema() == true )
        {
            definition  = "SET CURRENT_SCHEMA=";
            definition += sessionSchema;
            executeSQL(definition);
            definition = "";
        }
    }
    pTransformationDef->m_currentStream = stCatalog_els00;

    // read token
    while ( (errOK_els00 == rc) && (SAPDB_TRUE == (rcRead = getMetaData()->readLine ( line )) ) )
    {
        line.Trim();
        if ( 0 != token.Compare(line.SubStr(0,CMD_SEPARATOR_TOKEN_LEN_LS00) ) )
        {
            definition += NEW_LINE;
            definition += line;
        }
        else
        {
            if ( SAPDB_FALSE == definition.Empty() )
            {
                definition.Trim();
                // SQLMODE statement
                if (0 == _kwSqlmode.Compare(definition.SubStr(0,7) ) )
                {
                    definition = definition.SubStr(7, definition.Length());
                    definition.Trim();
                    pTransformationDef->setSQLMode( definition );
                }
                else
                {
                    // - USE USER   statement to CURRENT USER       <name> statement
                    // - USE SCHEMA statement to SET CURRENT_SCHEMA=<name> statement
                    if ( 0 == _kwUse.Compare(definition.SubStr(0,3)) )
                    {
                        _keptDefinition = definition;       // used for error messages

                        definition = definition.SubStr(3, definition.Length());
                        definition.Trim();
                        if ( 0 == _kwSchema.Compare(definition.SubStr(0,6)) )
                        {
                            definition = definition.SubStr(6, definition.Length());
                            definition.Trim();
                            pTransformationDef->m_schemaname = definition.SubStr(0, definition.FindFirstOf(whitespace));
                            _bSwitchNeeded = SAPDB_TRUE;
                        }
                    }
                    if (SAPDB_FALSE == _bSwitchNeeded)
                    {
                        rcBool = pTransformationDef->executeSQL( definition );
                        if ( SAPDB_FALSE == rcBool )
                        {
                            // Check count of failed statements and report last error to client
                            if (0 == --lMaxFailedStmt)
                            {
                                // Error is already logged in error object by executeSQL
                                rc = errODBCErrors_els98;
                            }
                        }
                    }
                    else
                    {
                        rc = pTransformationDef->switchUserSchema( currentUser, currentSchema);
                        if (errOK_els00 == rc )
                        {
                            currentUser   = pTransformationDef->m_owner;
                            currentSchema = pTransformationDef->m_schemaname;
                        }
                        else
                        {
                            LoaderMessages *pMsg = LoaderMessages::Instance();
                            pMsg->ls98PrintMessage(layCatalogLoad_els98, msgSimpleMsg_els98 , _keptDefinition.StrPtr() );
                            pMsg->ls98PrintMessage(layCatalogLoad_els98, errODBCErrors_els98, getMetaData()->m_pError->getErrorText().StrPtr());
                            rc = errODBCErrors_els98;
                        }
                    }
                    _bSwitchNeeded = SAPDB_FALSE;
                }
                definition.Erase();
            }// if ( SAPDB_FALSE == definition.Empty() )
        }// if ( 0 == token.Compare(line.SubStr(0,CMD_SEPARATOR_TOKEN_LEN_LS00) ) )
        line.Erase();
    }// while ( readLine ( line ) == SAPDB_TRUE )


    // Check for a last command to be executed (may happen in case the file content does 
    // end with an empty line or with the command itself).
    // Setting SQLMODE is necessary only if other commands follow. So we sort SQLMODE out here.
    if ( (errOK_els00 == rc) && 
         (SAPDB_FALSE == definition.Empty()) && (0 != _kwSqlmode.Compare(definition.SubStr(0,7) ) ) )
    {
        definition.TrimLeft();

        rcBool = pTransformationDef->executeSQL(definition);
        if (SAPDB_FALSE == rcBool)
        {
            if (0 == --lMaxFailedStmt)
            {
                rc = errODBCErrors_els98;
            }
        }
    }

    // Check for errors during stream read
    if (errOK_els00 == rc)
    {
        if ( SAPDB_FALSE == rcRead && errOK_els00 != getMetaData()->m_pError->getReturnCode() )
            rc = getMetaData()->m_pError->getReturnCode();
    }
    return rc;
}
/*===========================================================================*
 *     RelationalDef::TransformConnection
 *===========================================================================*/
SAPDB_Int4 RelationalDef::TransformConnection( )
{
    ROUTINE_DBG_MEO00 ("RelationalDef::TransformConnection");

    SAPDB_Int4 rc = errOK_els00;

    if ( m_pTransformationCmd->trCatalogFile_ls00.fsFormat_ls00.ffoDdl_ls00 == true)
    {
        StudioOAL_String CATALOG("SQLMODE ");

        switch(getMetaData()->getSqlMode())
        {
            case cpr_kind_db2:
            {
                CATALOG += "DB2";
                break;
            }
            case cpr_kind_ansi:
            {
                CATALOG += "ANSI";
                break;
            }
            case cpr_kind_oracle:
            {
                CATALOG += "ORACLE";
                break;
            }
            case cpr_kind_internal:
            default:
            {
                CATALOG += "INTERNAL";
            }
        }
        getMetaData()->writeLine( CATALOG );
    }
    if (m_pTransformationCmd->trCatalogFile_ls00.fsFormat_ls00.ffoXml_ls00 == true)
    {
        // SQLGetConnectAttr
        StudioOAL_String  Version;
        StudioOAL_String  Date;
        StudioOAL_String  Time;
        StudioOAL_String  Driver;
        StudioOAL_String  DataSource;
        StudioOAL_String  ServerNode;
        StudioOAL_String  DBName;
        StudioOAL_String  UserName;
        StudioOAL_String  Password;
        StudioOAL_String  SchemaName;
        StudioOAL_String  SQLMode(" sdbm:Sql_Mode=");
        StudioOAL_String  ConnectString;
        StudioOAL_String  OpenQuoteString  = " [ ";
        StudioOAL_String  CloseQuoteString = " ] ";

        tsp00_Versionc szVersion;
        tsp00_Datec    szDate;
        tsp00_Timec    szTime;

        tsp00_Date     pDate;
        tsp00_Time     pTime;
        sqldattime(pDate, pTime);
        szDate.p2c(pDate);
        szTime.p2c(pTime);
        //Version
        sp100_GetVersionString( COMP_NAME_REPL_MAN_SP100, s100buildnumber, szVersion );
        Version.Assign(REINTERPRET_CAST(SAPDB_UTF8*, szVersion.asCharp()));
        Version = Version.SubStr(7,Version.Length());
        Version.Trim();
        //Date
        Date.Assign(REINTERPRET_CAST(SAPDB_UTF8*, szDate.asCharp()));
        //Time
        Time.Assign(REINTERPRET_CAST(SAPDB_UTF8*, szTime.asCharp()));
        SAPDB_Char   destBeg[1000] ;
        SAPDB_UInt4  destAt  = 1000;
        //Driver
        memset(destBeg,0,sizeof(destBeg));
        getMetaData()->getConnection()->dbiOdbcConnection.GetDriver( REINTERPRET_CAST(unsigned char*,destBeg ),destAt);
        Driver.ConvertFromASCII_Latin1(destBeg,destBeg+strlen(destBeg));
        //ServerNode
        memset(destBeg,0,sizeof(destBeg));
        getMetaData()->getConnection()->dbiOdbcConnection.GetServerNode( REINTERPRET_CAST(unsigned char*, destBeg ), destAt);
        ServerNode.ConvertFromASCII_Latin1( destBeg, destBeg+strlen(destBeg));
        //DatabaseName
        memset(destBeg,0,sizeof(destBeg));
        getMetaData()->getConnection()->dbiOdbcConnection.GetServerDb ( REINTERPRET_CAST(unsigned char*,destBeg ),destAt);
        DBName.ConvertFromASCII_Latin1(destBeg,destBeg+strlen(destBeg));
        //UserName
        memset(destBeg,0,sizeof(destBeg));
        getMetaData()->getConnection()->dbiOdbcConnection.GetUser( REINTERPRET_CAST(unsigned char*,destBeg ),destAt);
        UserName.ConvertFromASCII_Latin1(destBeg,destBeg+strlen(destBeg));
        //Password
        memset(destBeg,0,sizeof(destBeg));
        getMetaData()->getConnection()->dbiOdbcConnection.GetPassword( REINTERPRET_CAST(unsigned char*,destBeg ),destAt);
        Password.ConvertFromASCII_Latin1(destBeg,destBeg+strlen(destBeg));
        Password = "*";
        // "DRIVER=SAPDB 7.3;SERVERNODE=P52837;SERVERDB=SUTDB73;UID=SUT;PWD=SUT;"
        ConnectString  = "DRIVER=";
        ConnectString += Driver;
        ConnectString += ";SERVERNODE=";
        ConnectString += ServerNode;
        ConnectString += ";SERVERDB=";
        ConnectString += DBName;
        ConnectString += ";UID=";
        ConnectString += UserName;
        ConnectString += ";PWD=";
        ConnectString += Password;
        ConnectString += ";";

        StudioOAL_String CATALOG;
        CATALOG = "<?xml version='1.0' encoding='UTF-8'?>\n"
                  "<oim:Transfer "
                  " xmlns:oim='REPOSITORY:oim.dtd' "
                  " xmlns:umx='REPOSITORY:umx.dtd' "
                  " xmlns:uml='REPOSITORY:uml.dtd' "
                  " xmlns:dbm='REPOSITORY:dbm.dtd' "
                  " xmlns:dtm='REPOSITORY:dtm.dtd' "
                  " xmlns:sdbm='REPOSITORY:sdbm.dtd'>\n";
        CATALOG += "<oim:TransferHeader Exporter=\"MAXDB Loader\" ExporterVersion=\"";
        CATALOG += Version;
        CATALOG += "\" TransferDateTime=\"";
        CATALOG += Date;
        CATALOG += "   ";
        CATALOG += Time;
        CATALOG += "\" TransferPackageGuid=\"";
        CATALOG += m_pTransformationCmd->trPackageid_ls00;
        CATALOG += "\"/>";
        CATALOG += "<uml:System oim:id=\"_0\"  name=\"MAXDB Data Warehousing Framework\">\n"
                   "<uml:SystemModels>\n"
                   "<uml:Model  oim:id=\"_1\" oim:label=\"MAXDB Database Schema\" name=\"MAXDB Database Schema\">\n"
                   "<uml:ModelPackages>\n";
        // Dbm:ConnectionSet
        CATALOG += "<dbm:ConnectionSet oim:id=\"_2\" oim:label=\"MAXDB WEB Agent ODBC Connection Pooling\""
                   " name=\"MAXDB WEB Agent ODBC Connection Pooling\""
                   ">\n";
        // Dbm:ConnectionSetConnections
        CATALOG += "<dbm:ConnectionSetConnections>\n";
        // sdbm:Connection
        CATALOG += "<sdbm:Connection oim:id=\"_4\" oim:supertype=\"Dbm:Connection\" oim:label=\"MAXDB ODBC Connection\" oim:name =\"";
        CATALOG += Driver;
        CATALOG += "\" dbm:UserName = \"";
        CATALOG += UserName;
        CATALOG += "\" dbm:Password = \"";
        CATALOG += Password;
        CATALOG += "\" dbm:ConnectString = \"";
        CATALOG += ConnectString;
        CATALOG += "\" dbm:ConnectionTimeOut = \" \""
                   "  dbm:Mode = \"sapdbwa_DBC_EXCL\""
                   "  dbm:IsReusable = \"0\""
                   "  dbm:OpenQuoteString = \"";
        CATALOG += OpenQuoteString;
        CATALOG += "\" dbm:CloseQuoteString = \"";
        CATALOG += CloseQuoteString;
        CATALOG += "\" dbm:DBName = \"";
        CATALOG += DBName;
        CATALOG += "\" dbm:DSN = \"\""
                   "  dbm:IsReadOnly = \"1\""
                   "  sdbm:Sql_Producer=\"sp1pr_internal_cmd\" sdbm:Sql_Component=\"sql_csp_comp_load\" ";
        switch(getMetaData()->getSqlMode())
        {
            case cpr_kind_db2:
            {
                SQLMode += "\"db2\"/>";
                break;
            }
            case cpr_kind_ansi:
            {
                SQLMode += "\"ansi\"/>";
                break;
            }
            case cpr_kind_oracle:
            {
                SQLMode += "\"oracle\"/>";
                break;
            }
            case cpr_kind_internal:
            default:
            {
                SQLMode += "\"internal\"/>";
            }
        }
        CATALOG += SQLMode;
        CATALOG += "</dbm:ConnectionSetConnections>"
                   "</dbm:ConnectionSet>";
        // Dbm:DataSource
        CATALOG += "<dbm:DataSource oim:id=\"_3\"  oim:label=\"";
        CATALOG += DataSource;
        CATALOG += "\" name=\"";
        CATALOG += DataSource;
        CATALOG += "\">\n";
        // Dbm:DataSourceDBMS
        CATALOG += "<dbm:DataSourceDBMS oim:id=\"_5\" ";
        CATALOG += "  name=\"";
        CATALOG += DataSource;
        CATALOG += "\" version=\"";
        CATALOG += "";
        CATALOG += "\">\n";
        // Dbm:ColumnTypeSet
        CATALOG += "<dbm:ColumnTypeSet oim:id=\"8\"";
        CATALOG += "  name=\"";
        CATALOG += DataSource;
        CATALOG += "\" version=\"";
        CATALOG += "";
        CATALOG += "\">\n";
        // Dtm:TypeSetTypes
        CATALOG += "<dtm:TypeSetTypes>\n";
        getMetaData()->writeLine ( CATALOG );
        CATALOG.Erase();
        TransformColumnTypes( );
        CATALOG += "</dtm:TypeSetTypes>\n";
        CATALOG += "</dbm:ColumnTypeSet>\n";
        CATALOG += "</dbm:DataSourceDBMS>\n";
        // Dbm:DataSourceConnections
        CATALOG += "<dbm:DataSourceConnections>\n";
        // sdbm:Connection
        CATALOG += "<sdbm:Connection oim:id=\"_6\" oim:supertype=\"Dbm:Connection\" oim:label=\"MAXDB ODBC Connection\" oim:name =\"";
        CATALOG += getMetaData()->getOdbcDriver();
        CATALOG += "\" dbm:UserName = \"";
        CATALOG += UserName;
        CATALOG += "\" dbm:Password = \"";
        CATALOG += Password;
        CATALOG += "\" dbm:ConnectString = \"";
        CATALOG += ConnectString;
        CATALOG += "\" dbm:ConnectionTimeOut = \" \""
                   "  dbm:Mode = \"sapdbwa_DBC_EXCL\""
                   "  dbm:IsReusable = \"0\""
                   "  dbm:OpenQuoteString = \"";
        CATALOG += OpenQuoteString;
        CATALOG += "\" dbm:CloseQuoteString = \"";
        CATALOG += CloseQuoteString;
        CATALOG += "\" dbm:DBName = \"";
        CATALOG += DBName;
        CATALOG += "\" dbm:DSN = \"\""
                   "  dbm:IsReadOnly = \"1\""
                   "  sdbm:Sql_Producer=\"sp1pr_internal_cmd\" sdbm:Sql_Component=\"sql_csp_comp_load\" ";
        CATALOG += SQLMode;
        CATALOG += "</dbm:DataSourceConnections>\n";
        // Dbm:DataSourceDeployedCatalogs
        CATALOG += "<dbm:DataSourceDeployedCatalogs>\n";
        // Dbm:DeployedCatalog
        CATALOG += "<dbm:DeployedCatalog oim:id=\"_7\" ";
        CATALOG += " RepositoryUpdatedBy=\"";
        CATALOG += UserName;
        CATALOG += "\" RepositoryUpdated=\"";
        CATALOG += Date;
        CATALOG += "   ";
        CATALOG += Time;
        CATALOG += "\" URL=\"";
        CATALOG += DBName;
        CATALOG += "\" name=\"";
        CATALOG += DBName;
        CATALOG += "\">\n";
        // Dbm:CatalogSchemas
        CATALOG += "<dbm:CatalogSchemas>\n";

        if (m_pTransformationCmd->trPartSpec_ls00.psPartType_ls00 == ptSchema_els00 &&
            m_pTransformationCmd->trCatalogFile_ls00.fsFormat_ls00.ffoXmlNoTable_ls00 == true)
        {
            SchemaName.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer,m_pTransformationCmd->trPartSpec_ls00.psTabSchema_ls00.asCharp()));
            // Uml:NamespaceOwnedElement
            CATALOG += "<uml:NamespaceOwnedElement>\n";
            // sdbm:Schema
            CATALOG += "<sdbm:DeployedSchema oim:id=\"_";
            CATALOG += UserName;
            CATALOG += ".";
            CATALOG += SchemaName;
            CATALOG += "\" oim:supertype=\"dbm:DeployedSchema\"";
            CATALOG += " oim:label=\"";
            CATALOG += "SCHEMA";
            CATALOG += "\" oim:name=\"";
            CATALOG += SchemaName;
            CATALOG += "\" sdbm:Owner=\"";
            CATALOG += "";
            CATALOG += "\" sdbm:Groupname=\"";
            CATALOG += "";
            CATALOG += "\" sdbm:Username=\"";
            CATALOG += "";
            CATALOG += "\" sdbm:Usermode=\"";
            CATALOG += "";
            CATALOG += "\" sdbm:MaxTimeOut=\"";
            CATALOG += "";
            CATALOG += "\" sdbm:Costwarning=\"";
            CATALOG += "";
            CATALOG += "\" sdbm:Costlimit=\"";
            CATALOG += "";
            CATALOG += "\" sdbm:Connectmode=\"";
            CATALOG += "";
            CATALOG += "\" sdbm:Comment=\"";
            CATALOG += "";
            CATALOG += "\">\n";
            CATALOG += "</sdbm:DeployedSchema>\n";
            CATALOG += "</uml:NamespaceOwnedElement>\n";
        }

        getMetaData()->writeLine ( CATALOG );
    }
    return rc;
}
/*===========================================================================*
 *  RelationalDef::TransformColumnTypes                                      *
 *===========================================================================*/
SAPDB_Int4 RelationalDef::TransformColumnTypes( )
{
    LoaderMessages *pMsgInstance = LoaderMessages::Instance();
    SAPDB_Int4 rc = errOK_els00;

    // SYSODBCTYPES  :  0.TYPE_NAME, 1.DATA_TYPE, 2.PRECISION, 3.LITERAL_PREFIX, 4.LITERAL_SUFFIX, 5.CREATE_PARAMS, 6.NULLABLE, 7.CASE_SENSITIVE, 8.SEARCHABLE, 9.UNSIGNED_ATTRIBUTE, 10.MONEY, 11.AUTO_INCREMENT
    //                 12.LOCAL_TYPE_NAME, 13.MINIMUM_SCALE, 14.MAXIMUM_SCALE, 15.SQL_DATA_TYPE, 16.SQL_DATETIME_SUB, 17.NUM_PREC_RADIX, 18.INTERVAL_PRECISION, 19.REMARKS

    // SQLGetTypeInfo:  0.TYPE_NAME, 1.DATA_TYPE, 2.PRECISION, 3.LITERAL_PREFIX, 4.LITERAL_SUFFIX, 5.CREATE_PARAMS, 6.NULLABLE, 7.CASE_SENSITIVE, 8.SEARCHABLE, 9.UNSIGNED_ATTRIBUTE, 10.MONEY, 11.AUTO_INCREMENT
    //                  12.LOCAL_TYPE_NAME, 13.MINIMUM_SCALE, 14.MAXIMUM_SCALE
    if ( m_pTransformationCmd->trCatalogFile_ls00.fsFormat_ls00.ffoXml_ls00 == true )
    {
        StudioOAL_String  record[15];
        if( !m_pColumnTypeDef->getDataSourceTypeInfo() )
        {
            StudioOAL_String statement = "SQLGetTypeInfo";
            StudioOAL_String errorText;

            errorText   = m_pColumnTypeDef->getErrorText();

            SAPDB_Char        ErrText[MAX_REPLY_LENGTH_LS00];
            const SAPDB_Char* ErrTextEnd = ErrText + MAX_REPLY_LENGTH_LS00;
            SAPDB_Char*       destAt;

            errorText.ConvertToASCII_Latin1(ErrText, ErrTextEnd, destAt);

            pMsgInstance->ls98PrintMessage(layCatalogExtract_els98, msgSimpleMsg_els98 , statement.StrPtr() );
            pMsgInstance->ls98PrintMessage(layCatalogExtract_els98, errODBCErrors_els98, errorText.StrPtr() );
            rc = errODBCErrors_els98;
        }
        else
        {
            /*-------------------------------------------------------------------------------*
             * Read/Write the CATALOG definition of the specified object from/to resources   *
             *-------------------------------------------------------------------------------*/
            while ( m_pColumnTypeDef->readResult (record) )
                    m_pColumnTypeDef->writeResult(record);
        }
    }
    return rc;
}
/*===========================================================================*
 *  RelationalDef::TransformUsers                                                 *
 *===========================================================================*/
SAPDB_Int4 RelationalDef::TransformUsers( )
{
    ROUTINE_DBG_MEO00 ("RelationalDef::TransformUsers");

    LoaderMessages *pMsgInstance = LoaderMessages::Instance();
    SAPDB_Int4 rc   = errOK_els00;
    SQLRETURN         ReturnCode  = 0;
    StudioOAL_String  NativeErrorAsString;
    StudioOAL_String  whereClause;
    StudioOAL_String  record[10];

    TransformationDef *pTransformationDef = getMetaData()->getTransformationModel();
    tls00_DBInfo      *pDBInfo            = getMetaData()->getConnection();

    _TCHAR statement[1024];
    statement       [0] = 0;
    if ( m_pTransformationCmd->trCatalogFile_ls00.fsFormat_ls00.ffoDdl_ls00 == true )
    {
        /*-------------------------------------------------------------------------*
         * Prepare the WHERE clause for the SQL statements.                        *
         *-------------------------------------------------------------------------*/
        if ( m_pTransformationCmd->trPartSpec_ls00.psPartType_ls00 == ptAll_els00 )
        {
            // WHERE contained in sel_Relational_16
            whereClause = " WHERE USERMODE <> 'SYSDBA'  "
                          " AND   USERMODE <> 'COLDUSER'"
                          " AND   USERMODE <> 'ADMIN'   "
                          " AND   USERMODE <> 'CONTROL' "
                          " AND   USERNAME <> 'DOMAIN'  "
                          " AND   USERNAME <> 'SYS'     "
                          " AND   USERNAME <> 'SYSINFO' ";
           if ( isSchema() == true )
                whereClause += " AND   ISREPLICATIONUSER <> 'YES'";
        }
        if ( m_pTransformationCmd->trPartSpec_ls00.psPartType_ls00 == ptUser_els00 )
        {
            whereClause  = "WHERE OWNER = '";
            whereClause  += pTransformationDef->currentUser();
            whereClause  +="'";
        }
		/*-------------------------------------------------------------------------------*
         * Read/Write the definition of the specified object from/to resources           *
         *-------------------------------------------------------------------------------*/
        sp77sprintf(statement, 1024, CONST_CAST(SAPDB_Char*, Statement(sel_Relational_16).CharPtr()), whereClause.StrPtr() );
        if ( !m_pUserDef->executeSQL(statement) )
        {
            NativeErrorAsString = pTransformationDef->getNativeErrorAsString();
            pMsgInstance->ls98PrintMessage(layCatalogExtract_els98, errODBCErrors_els98, NativeErrorAsString.StrPtr());
            rc = pTransformationDef->getReturnCode();
        }
        else
        {
            StudioOAL_String sessionUser, currentUser, sessionSchema, currentSchema;
            sessionUser.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer, pDBInfo->dbiSession_ls00.siUserName_ls00.asCharp()));
            currentUser = sessionUser;
            sessionSchema.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer, pDBInfo->dbiSession_ls00.siSchemaName_ls00));
            currentSchema = sessionSchema;
            pTransformationDef->m_owner      = currentUser;
            pTransformationDef->m_schemaname = currentSchema;
            pTransformationDef->m_currentStream = stCatalog_els00;
            while( m_pUserDef->readResult(record) )
            {
                pTransformationDef->m_owner =  record[0];
                pTransformationDef->switchUserSchema( currentUser, currentSchema );
                currentUser = pTransformationDef->m_owner;
                m_pUserDef->writeResult(record );
            }
        }
    }
    return rc;
}
/*===========================================================================*
 *  RelationalDef::TransformRoles                                                 *
 *===========================================================================*/
SAPDB_Int4 RelationalDef::TransformRoles( )
{
    ROUTINE_DBG_MEO00 ("RelationalDef::TransformRoles");
    SAPDB_Int4 rc = errOK_els00;

    TransformationDef *pTransformationDef = getMetaData()->getTransformationModel();

    if ( m_pTransformationCmd->trCatalogFile_ls00.fsFormat_ls00.ffoDdl_ls00 == true )
    {
        // <grantee> ::= PUBLIC | <user_name> | <usergroup_name> | <role_name>
        _TCHAR statement  [1024];
        statement         [0]=0;
        StudioOAL_String  owner;
        StudioOAL_String  schema_name;
        if( m_pTransformationCmd->trPartSpec_ls00.psPartType_ls00 == ptUser_els00 )
            schema_name = pTransformationDef->currentUser();
        else 
            schema_name = '%';
        sp77sprintf(statement, 1024, CONST_CAST(SAPDB_Char*, Statement(sel_Relational_2).CharPtr()), schema_name.StrPtr());
        /*-------------------------------------------------------------------------------*
         * Read/Write the role definition of the specified table from/to resources
         *-------------------------------------------------------------------------------*/
        if ( m_pRoleDef->executeSQL(statement) )
        {
            StudioOAL_String  record[2];
            while ( m_pRoleDef->readResult (record) )
                    m_pRoleDef->writeResult(record);
        }
    }
    return rc;
}
/*===========================================================================*
 *  RelationalDef::TransformSchemas                                          *
 *===========================================================================*/
SAPDB_Int4 RelationalDef::TransformSchemas( )
{
    ROUTINE_DBG_MEO00 ("RelationalDef::TransformSchemas");
    LoaderMessages *pMsgInstance = LoaderMessages::Instance();

	TransformationDef *pTransformationDef = getMetaData()->getTransformationModel();
    tls00_DBInfo      *pDBInfo            = getMetaData()->getConnection();
    m_pRelationalStmt[0] = 0;
    SAPDB_Int4  lPos = 0;

    SAPDB_Int4        rc          = errOK_els00;
    SQLRETURN         ReturnCode  = 0;
    StudioOAL_String  NativeErrorAsString;
    StudioOAL_String  owner;

    if ( isSchema() == true &&
		 m_pTransformationCmd->trCatalogFile_ls00.fsFormat_ls00.ffoDdl_ls00 == true )
    {
        m_pRelationalStmt[0] = 0;
        if( ptAll_els00 == m_pTransformationCmd->trPartSpec_ls00.psPartType_ls00 ) {
            sp77sprintf(m_pRelationalStmt, 1024, CONST_CAST(SAPDB_Char*, Statement(sel_Transformation_721).CharPtr()),
                        m_pTransformationCmd->trPackageid_ls00.StrPtr(), _T(" AND T.OWNER LIKE "), _T("%")  );
        }
        else if( ptUser_els00 == m_pTransformationCmd->trPartSpec_ls00.psPartType_ls00 ) {
            sp77sprintf(m_pRelationalStmt, 1024, CONST_CAST(SAPDB_Char*, Statement(sel_Transformation_721).CharPtr()), 
                        m_pTransformationCmd->trPackageid_ls00.StrPtr(), _T(" AND T.OWNER LIKE "), 
                        m_pTransformationCmd->trPartSpec_ls00.psTabOwner_ls00.asCharp());
        }
        else if( ptSchema_els00 == m_pTransformationCmd->trPartSpec_ls00.psPartType_ls00 ) {
            sp77sprintf(m_pRelationalStmt, 1024, CONST_CAST(SAPDB_Char*, Statement(sel_Transformation_721).CharPtr()),
                        m_pTransformationCmd->trPackageid_ls00.StrPtr(), _T(" AND T.SCHEMANAME LIKE "),
                        m_pTransformationCmd->trPartSpec_ls00.psTabSchema_ls00.asCharp());
        }
        if ( !m_pSchemaDef->executeSQL(m_pRelationalStmt) )
        {
            NativeErrorAsString = pTransformationDef->getNativeErrorAsString();
            pMsgInstance->ls98PrintMessage(layCatalogExtract_els98, errODBCErrors_els98, NativeErrorAsString.StrPtr());
            rc = pTransformationDef->getReturnCode();
        }
        else
        {
            StudioOAL_String record[9];
            StudioOAL_String sessionUser, currentUser, sessionSchema, currentSchema;
            sessionUser.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer, pDBInfo->dbiSession_ls00.siUserName_ls00.asCharp()));
            currentUser = sessionUser;
            sessionSchema.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer, pDBInfo->dbiSession_ls00.siSchemaName_ls00));
            currentSchema = sessionSchema;
            pTransformationDef->m_owner      = currentUser   ;
            pTransformationDef->m_schemaname = currentSchema ;
            pTransformationDef->m_currentStream = stCatalog_els00;
            /*-------------------------------------------------------------------------------*
            * Read/Write the CATALOG definition of the specified object from/to resources   *
            *-------------------------------------------------------------------------------*/
            while( m_pSchemaDef->readResult(record) )
            {
                   pTransformationDef->m_owner      = record[1] ;
                   pTransformationDef->switchUserSchema ( currentUser, currentSchema);
                   m_pSchemaDef ->writeResult(record) ;
                   pTransformationDef->m_schemaname = record[2] ;
                   currentUser   = pTransformationDef->m_owner ;
                   currentSchema = pTransformationDef->m_schemaname;
            }// while (m_pSchemaDef->readResult(record))
        }
     }
     return rc;
}
/*===========================================================================*
 *  RelationalDef::TransformTables                                  *
 *===========================================================================*/
SAPDB_Int4 RelationalDef::TransformTables( )
{
    ROUTINE_DBG_MEO00 ("RelationalDef::TransformTables");
    LoaderMessages *pMsgInstance = LoaderMessages::Instance();
    
    TransformationDef *pTransformationDef = getMetaData()->getTransformationModel();
    tls00_DBInfo      *pDBInfo            = getMetaData()->getConnection();
    SAPDB_Int4         rc     = errOK_els00;
    SAPDB_Bool         rcBOOL = SAPDB_TRUE;
    SQLRETURN         ReturnCode = 0;
    StudioOAL_String  NativeErrorAsString;
    SAPDB_Bool bGrant = SAPDB_FALSE;

    StudioOAL_String  sql;
    StudioOAL_String  where      = " ";
    StudioOAL_String  schemaname = " ";
    StudioOAL_String  orderby    = " ";
    StudioOAL_String  definition;

    m_pTransformationStmt[0]=0;
    if ( m_pTransformationCmd->trCatalogFile_ls00.fsFormat_ls00.ffoDdl_ls00 == true ) {
         sp77sprintf(m_pTransformationStmt, 1024, CONST_CAST(SAPDB_Char*, Statement(sel_Transformation_703).CharPtr()),
                                                  m_pTransformationCmd->trPackageid_ls00.StrPtr(), 
                                                  where.StrPtr(), orderby.StrPtr() );
    }
    else 
    if (m_pTransformationCmd->trCatalogFile_ls00.fsFormat_ls00.ffoXml_ls00        == true &&
        m_pTransformationCmd->trCatalogFile_ls00.fsFormat_ls00.ffoXmlNoTable_ls00 == false   )
    {
        where   = " AND EXCLUDE = FALSE ";
        orderby = " OWNER, SCHEMANAME, ";
        if( ptSchema_els00 == m_pTransformationCmd->trPartSpec_ls00.psPartType_ls00 )
        {
            schemaname.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer, m_pTransformationCmd->trPartSpec_ls00.psTabSchema_ls00.asCharp()));
            where  += " AND SCHEMANAME = '";
            where  += schemaname;
            where  += "'";
        }
        sp77sprintf(m_pTransformationStmt, 1024, CONST_CAST(SAPDB_Char*, Statement(sel_Transformation_702).CharPtr()),
                                                 m_pTransformationCmd->trPackageid_ls00.StrPtr(),
                                                 where.StrPtr(), orderby.StrPtr() );
    }
    else 
    if (m_pTransformationCmd->trCatalogFile_ls00.fsFormat_ls00.ffoXml_ls00        == true &&
        m_pTransformationCmd->trCatalogFile_ls00.fsFormat_ls00.ffoXmlNoTable_ls00 == true   )
    {
        if( ptAll_els00 == m_pTransformationCmd->trPartSpec_ls00.psPartType_ls00 ) {
            sp77sprintf(m_pTransformationStmt, 1024, CONST_CAST(SAPDB_Char*, Statement(sel_Transformation_706).CharPtr()),
                        m_pTransformationCmd->trPackageid_ls00.StrPtr());
        }
        else if( ptUser_els00 == m_pTransformationCmd->trPartSpec_ls00.psPartType_ls00 ) {
            sp77sprintf(m_pTransformationStmt, 1024, CONST_CAST(SAPDB_Char*, Statement(sel_Transformation_707).CharPtr()),
                        m_pTransformationCmd->trPackageid_ls00.StrPtr(),m_pTransformationCmd->trPartSpec_ls00.psTabOwner_ls00.asCharp());
        }
        else if( ptSchema_els00 == m_pTransformationCmd->trPartSpec_ls00.psPartType_ls00 ) {
            sp77sprintf(m_pTransformationStmt, 1024, CONST_CAST(SAPDB_Char*, Statement(sel_Transformation_720).CharPtr()),
                        m_pTransformationCmd->trPackageid_ls00.StrPtr(),m_pTransformationCmd->trPartSpec_ls00.psTabSchema_ls00.asCharp());
        }
    }
    if ( !pTransformationDef->executeSQL(m_pTransformationStmt) )
    {
        ReturnCode          = pTransformationDef->getReturnCode();
        NativeErrorAsString = pTransformationDef->getNativeErrorAsString();
        pMsgInstance->ls98PrintMessage(layCatalogExtract_els98, errODBCErrors_els98, NativeErrorAsString.StrPtr());
        return ReturnCode;
    }
    StudioOAL_String sessionUser, currentUser, sessionSchema, currentSchema;
    sessionUser.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer, pDBInfo->dbiSession_ls00.siUserName_ls00.asCharp()));
    currentUser = sessionUser;
    sessionSchema.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer, pDBInfo->dbiSession_ls00.siSchemaName_ls00));
    currentSchema = sessionSchema;

    pTransformationDef->m_currentStream = stCatalog_els00;
    /*-------------------------------------------------------------------------------*
    * Read/Write the CATALOG definition of the specified object from/to resources   *
    *-------------------------------------------------------------------------------*/
    while ( pTransformationDef->initTransformationStep() )
    {
        /*-------------------------------------------------------------------------*
         * Close the previous Schema                                               *
         *-------------------------------------------------------------------------*/
        pTransformationDef->switchUserSchema ( currentUser ,currentSchema );
        
        currentUser   = pTransformationDef->m_owner ;
        currentSchema = pTransformationDef->m_schemaname ;

        if ( m_pTransformationCmd->trCatalogFile_ls00.fsFormat_ls00.ffoXmlNoTable_ls00 == false ) 
        {

            if ( !pTransformationDef->updateBegin())
            {
                ReturnCode          = pTransformationDef->getReturnCode();
                NativeErrorAsString = pTransformationDef->getNativeErrorAsString();
                pMsgInstance->ls98PrintMessage(layCatalogExtract_els98, errODBCErrors_els98, NativeErrorAsString.StrPtr());
                return ReturnCode;
            }
            if ( StudioOAL_String("TABLE").Compare( m_cmd.SubStr(0,5) ) == 0)
            {
                TransformTable  ( m_schemaname, m_tablename );
                if ( m_pTransformationCmd->trCatalogFile_ls00.fsFormat_ls00.ffoDdlNoIndex_ls00 == false && (
                    m_pTransformationCmd->trCatalogFile_ls00.fsFormat_ls00.ffoXml_ls00         == true  ||
                    m_pTransformationCmd->trPartSpec_ls00.psPartType_ls00 == ptTable_els00) )
                TransformIndexes( m_schemaname, m_tablename );
                //TransformTrigger( m_schemaname, m_tablename );
               /*-------------------------------------------------------------------------*
                * Close the previous Table                                                *
                *-------------------------------------------------------------------------*/
                if ( m_pTransformationCmd->trCatalogFile_ls00.fsFormat_ls00.ffoXml_ls00 == true )
                {
                    StudioOAL_String   deployedTable;
                    deployedTable  = "</sdbm:DeployedTable>";
                    pTransformationDef->writeResult(deployedTable);
                }
                bGrant = SAPDB_FALSE;
                
                rcBOOL = pTransformationDef->updateCatalog();
                if (SAPDB_TRUE == rcBOOL)
                {
                    rcBOOL = pTransformationDef->updateEnd();
                }
                if (SAPDB_FALSE == rcBOOL)
                {
                    ReturnCode          = pTransformationDef->getReturnCode();
                    NativeErrorAsString = pTransformationDef->getNativeErrorAsString();
                    pMsgInstance->ls98PrintMessage(layCatalogExtract_els98, errODBCErrors_els98, NativeErrorAsString.StrPtr());
                    return ReturnCode;
                }
                continue;
            }
            if (StudioOAL_String("CREATE").Compare( m_cmd.SubStr(0,6) ) == 0)
            {
                definition    = m_cmd.SubStr(7,m_cmd.Length());
                definition.Trim()  ;
                if( definition.SubStr(0,4).Compare ("VIEW") == 0   )
                {
                    TransformView( m_schemaname, m_tablename );

                    rcBOOL = pTransformationDef->updateCatalog();
                    if (SAPDB_TRUE == rcBOOL)
                    {
                        rcBOOL = pTransformationDef->updateEnd();
                    }
                    if (SAPDB_FALSE == rcBOOL)
                    {
                        ReturnCode          = pTransformationDef->getReturnCode();
                        NativeErrorAsString = pTransformationDef->getNativeErrorAsString();
                        pMsgInstance->ls98PrintMessage(layCatalogExtract_els98, errODBCErrors_els98, NativeErrorAsString.StrPtr());
                        return ReturnCode;
                    }
                }
                if (definition.SubStr(0,7).Compare ("SYNONYM")  == 0 ||
                    definition.SubStr(0,6).Compare ("PUBLIC" )  == 0    )
                {
                    TransformSynonym( m_schemaname, m_tablename );
                    rcBOOL = pTransformationDef->updateCatalog();
                    if (SAPDB_TRUE == rcBOOL)
                    {
                        pTransformationDef->updateEnd();
                    }
                    if (SAPDB_FALSE == rcBOOL)
                    {
                        ReturnCode          = pTransformationDef->getReturnCode();
                        NativeErrorAsString = pTransformationDef->getNativeErrorAsString();
                        pMsgInstance->ls98PrintMessage(layCatalogExtract_els98, errODBCErrors_els98, NativeErrorAsString.StrPtr());
                        return ReturnCode;
                    }
                }
                bGrant = SAPDB_FALSE;
                continue;
            }
            if (StudioOAL_String("GRANT").Compare( m_cmd.SubStr(0,5) ) == 0 )
            {
                if ( m_pTransformationCmd->trCatalogFile_ls00.fsFormat_ls00.ffoDdl_ls00 == true )
                {
                    sql    = SEPARATOR;
                    sql   += m_cmd;
                    pTransformationDef->writeResult(sql);
                }
                bGrant = SAPDB_TRUE;
                continue;
            }
            if ( bGrant == SAPDB_TRUE )
            {
                if ( m_pTransformationCmd->trCatalogFile_ls00.fsFormat_ls00.ffoDdl_ls00 == true )
                {
                    sql = m_cmd;
                    pTransformationDef->writeResult(sql);
                }
            }
        }
    }// while ( pTransformationDef->initTransformationStep() )
    if ( !pTransformationDef->executeSQL("COMMIT WORK") )
    {
        ReturnCode          = pTransformationDef->getReturnCode();
        NativeErrorAsString = pTransformationDef->getNativeErrorAsString();
        pMsgInstance->ls98PrintMessage(layCatalogExtract_els98, errODBCErrors_els98, NativeErrorAsString.StrPtr());
        return ReturnCode;
    }
    return rc;
}
/*===========================================================================*
 *  RelationalDef::TransformForeignKeys
 *===========================================================================*/
SAPDB_Int4 RelationalDef::TransformForeignKeys( )
{
    ROUTINE_DBG_MEO00 ("RelationalDef::TransformForeignKeys");
    LoaderMessages *pMsgInstance = LoaderMessages::Instance();
    SAPDB_Int4 rc = errOK_els00;

    if ( m_pTransformationCmd->trCatalogFile_ls00.fsFormat_ls00.ffoDdl_ls00 == true )
    {
        TransformationDef *pTransformationDef = getMetaData()->getTransformationModel();
        tls00_DBInfo      *pDBInfo            = getMetaData()->getConnection();
        SQLRETURN         ReturnCode = 0;
        StudioOAL_String  NativeErrorAsString;

        _TCHAR statement[1024];
        statement       [0]=0;
        sp77sprintf(statement, 1024, CONST_CAST(SAPDB_Char*, Statement(sel_Transformation_705).CharPtr()),
                                     m_pTransformationCmd->trPackageid_ls00.StrPtr());

        if ( !pTransformationDef->executeSQL(statement) )
        {
            ReturnCode          = pTransformationDef->getReturnCode();
            NativeErrorAsString = pTransformationDef->getNativeErrorAsString();
            pMsgInstance->ls98PrintMessage(layCatalogExtract_els98, errODBCErrors_els98, NativeErrorAsString.StrPtr());
            return ReturnCode;
        }
        StudioOAL_String sessionUser, currentUser, sessionSchema, currentSchema;
        sessionUser.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer , pDBInfo->dbiSession_ls00.siUserName_ls00.asCharp()));
        currentUser = sessionUser;
        sessionSchema.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer, pDBInfo->dbiSession_ls00.siSchemaName_ls00));
        currentSchema = sessionSchema;

        pTransformationDef->m_currentStream = stCatalog_els00;

        /*-------------------------------------------------------------------------------*
        * Read/Write the CATALOG definition of the specified object from/to resources   *
        *-------------------------------------------------------------------------------*/
        while( pTransformationDef->initTransformationStep() )
        {
            pTransformationDef->switchUserSchema ( currentUser, currentSchema );
            currentUser   = pTransformationDef->m_owner ;
            currentSchema = pTransformationDef->m_schemaname ;
            TransformForeignKeys(m_schemaname,m_tablename);
        }
    }
    return rc;
}
/*===========================================================================*
 *  RelationalDef::TransformTable
 *===========================================================================*/
SAPDB_Int4 RelationalDef::TransformTable(StudioOAL_String& schemaname, StudioOAL_String& tablename )
{
    ROUTINE_DBG_MEO00 ("RelationalDef::TransformTable");
    SAPDB_Int4 rc = errOK_els00;
   /*-------------------------------------------------------------------------*
    * Reset the table definition to its initial values.                       *
    *-------------------------------------------------------------------------*/
    rc = m_pTableDef->Init(schemaname, tablename);
    /*-------------------------------------------------------------------------*
     * Read/Write the table definition of the specified table from/to resources*
     *-------------------------------------------------------------------------*/
    if (errOK_els00 == rc)
    {
        rc = m_pTableDef->readTableDefinitionDirect(schemaname, tablename);
        if (errOK_els00 == rc)
        {
            ++m_nTableCount;
            m_pTableDef->writeTableDefinition();
        }
    }
    return rc;
}

/*===========================================================================*
 *  RelationalDef::TransformTableTypes
 *===========================================================================*/
SAPDB_Int4 RelationalDef::TransformTableTypes( )
{
    ROUTINE_DBG_MEO00 ("RelationalDef::TransformTableTypes");
    LoaderMessages *pMsgInstance = LoaderMessages::Instance();

    TransformationDef *pTransformationDef = getMetaData()->getTransformationModel();
    tls00_DBInfo      *pDBInfo            = getMetaData()->getConnection();

    SAPDB_Bool bGrant = SAPDB_FALSE;

    SAPDB_Int4 rc = errOK_els00;

    if ( m_pTransformationCmd->trCatalogFile_ls00.fsFormat_ls00.ffoDdl_ls00 == true )
    {
        StudioOAL_String  grant;
        SAPDB_Bool        bGrant = SAPDB_FALSE;
        SQLRETURN         ReturnCode = 0;
        StudioOAL_String  NativeErrorAsString;

        sp77sprintf(m_pTransformationStmt, 1024, CONST_CAST(SAPDB_Char*, Statement(sel_Transformation_704).CharPtr()),
                                                 m_pTransformationCmd->trPackageid_ls00.StrPtr());
        if ( !pTransformationDef->executeSQL(m_pTransformationStmt) )
        {
            ReturnCode          = pTransformationDef->getReturnCode();
            NativeErrorAsString = pTransformationDef->getNativeErrorAsString();
            pMsgInstance->ls98PrintMessage(layCatalogExtract_els98, errODBCErrors_els98, NativeErrorAsString.StrPtr());

            return ReturnCode;
        }

        StudioOAL_String sessionUser, currentUser, sessionSchema, currentSchema;
        sessionUser.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer, pDBInfo->dbiSession_ls00.siUserName_ls00.asCharp()));
        currentUser = sessionUser;
        sessionSchema.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer, pDBInfo->dbiSession_ls00.siSchemaName_ls00));
        currentSchema = sessionSchema;

        pTransformationDef->m_currentStream = stCatalog_els00;

        /*-------------------------------------------------------------------------------*
        * Read/Write the CATALOG definition of the specified object from/to resources   *
        *-------------------------------------------------------------------------------*/
        while( pTransformationDef->initTransformationStep() )
        {
            pTransformationDef->switchUserSchema ( currentUser ,currentSchema );

            currentUser   = pTransformationDef->m_owner ;
            currentSchema = pTransformationDef->m_schemaname ;

            if ( StudioOAL_String("VIEW").Compare( m_tabletype.SubStr(0,4)) == 0)
            {
                if (StudioOAL_String("CREATE").Compare( m_cmd.SubStr(0,6)) == 0 )
                {
                    TransformView   ( m_schemaname, m_tablename );
                    bGrant = SAPDB_FALSE;
                }
                else if (StudioOAL_String("GRANT").Compare( m_cmd.SubStr(0,5)) == 0)
                { 
                    grant  = SEPARATOR;
                    grant += m_cmd;
                    pTransformationDef->writeResult(grant);
                    bGrant = SAPDB_TRUE;
                    continue;
                }
            }
            else if (StudioOAL_String("SYNONYM").Compare( m_tabletype.SubStr(0,7)) == 0)
            {
                if (StudioOAL_String("CREATE").Compare( m_cmd.SubStr(0,6)) == 0 )
                {  
                    TransformSynonym( m_schemaname, m_tablename );
                    bGrant = SAPDB_FALSE;
                }
                else if (StudioOAL_String("GRANT").Compare( m_cmd.SubStr(0,5)) == 0)
                { 
                    grant  = SEPARATOR;
                    grant += m_cmd;
                    pTransformationDef->writeResult(grant);
                    bGrant = SAPDB_TRUE;
                    continue;
                }
            }
            if ( bGrant == SAPDB_TRUE )
            {
                if ( m_pTransformationCmd->trCatalogFile_ls00.fsFormat_ls00.ffoDdl_ls00 == true )
                {
                    grant = m_cmd;
                    pTransformationDef->writeResult(grant);
                }
            }
        }
    }
    return rc;
}
/*===========================================================================*
 *  RelationalDef::TransformView
 *===========================================================================*/
SAPDB_Int4 RelationalDef::TransformView( StudioOAL_String& schemaname, StudioOAL_String& viewname )
{
    ROUTINE_DBG_MEO00 ("RelationalDef::TransformView");
    SAPDB_Int4 rc = errOK_els00;
    /*-------------------------------------------------------------------------*
     * Prepare the SQL statements.                                             *
     *-------------------------------------------------------------------------*/
    m_pRelationalStmt[0] = 0;
    sp77sprintf(m_pRelationalStmt, 1024,  CONST_CAST(SAPDB_Char*, Statement(sel_Relational_11).CharPtr()),
                                          schemaname.StrPtr(), viewname.StrPtr());
    if ( !m_pViewDef->executeSQL( m_pRelationalStmt ) )
    {
        StudioOAL_String NativeErrorAsString = m_pViewDef->getNativeErrorAsString();
        LoaderMessages::Instance()->ls98PrintMessage(layCatalogExtract_els98, errODBCErrors_els98, NativeErrorAsString.StrPtr());
        rc = m_pViewDef->getReturnCode();
    }
    else
    {
        StudioOAL_String  record[3];
        if ( m_pViewDef->readResult ( record ) )
        {
            m_nViewCount++;
            m_pViewDef->writeResult( record );
        }
    }
    return rc;
}
/*===========================================================================*
 *  RelationalDef::TransformView_
 *===========================================================================*/
SAPDB_Int4 RelationalDef::TransformView_( StudioOAL_String owner, StudioOAL_String viewname )
{
    ROUTINE_DBG_MEO00 ("RelationalDef::TransformView_");

    SAPDB_Int4 rc = errOK_els00;

    /*-------------------------------------------------------------------------*
     * Read/Write the view definition of the specified table from/to resources
     *-------------------------------------------------------------------------*/
    if ( m_pViewDef->readViewDefinition( owner, viewname ) )
    {
        m_nViewCount ++ ;
        m_pViewDef->writeViewDefinition();
    }
    return rc;
}
/*===========================================================================*
 *  RelationalDef::TransformSynonym
 *===========================================================================*/
SAPDB_Int4 RelationalDef::TransformSynonym( StudioOAL_String schemaname, StudioOAL_String synonymname )
{
    ROUTINE_DBG_MEO00 ("RelationalDef::TransformSynonym");
    SAPDB_Int4 rc = errOK_els00;
   /*-------------------------------------------------------------------------*
    * Prepare the SQL statements.                                             *
    *-------------------------------------------------------------------------*/
    m_pRelationalStmt[0]=0;
    sp77sprintf(m_pRelationalStmt, 1024, CONST_CAST(SAPDB_Char*, Statement(sel_Relational_12).CharPtr()),
                                         schemaname.StrPtr(), synonymname.StrPtr());
    if ( !m_pSynonymDef->executeSQL(m_pRelationalStmt) )
    {
        SAPDB_Long       NativeError         = m_pSynonymDef->getNativeError();
        StudioOAL_String NativeErrorAsString = m_pSynonymDef->getNativeErrorAsString();
        LoaderMessages::Instance()->ls98PrintMessage(layCatalogExtract_els98, errODBCErrors_els98, NativeErrorAsString.StrPtr());
        rc = STATIC_CAST(SAPDB_Int4, NativeError);
    }
    else
    {
        StudioOAL_String  record[6];
        while ( m_pSynonymDef->readResult( record ) )
        {
            m_nSynonymCount++;
            m_pSynonymDef->writeResult( record );
        }
    }
    return rc;
}
/*===========================================================================*
 *  RelationalDef::TransformSynonym_
 *===========================================================================*/
SAPDB_Int4 RelationalDef::TransformSynonym_( StudioOAL_String schemaname, StudioOAL_String synonymname )
{
    ROUTINE_DBG_MEO00 ("RelationalDef::TransformSynonym_");
    SAPDB_Int4 rc = errOK_els00;
    /*-------------------------------------------------------------------------*
     * Read/Write the synonym definition of the specified table from/to resources
     *-------------------------------------------------------------------------*/
    if ( m_pSynonymDef->readSynonymDefinition  ( schemaname, synonymname ) )
    {
        m_nSynonymCount++;
        m_pSynonymDef->writeSynonymDefinition();
    }
    return rc;
}
/*===========================================================================*
 *  RelationalDef::TransformForeignKeys
 *===========================================================================*/
SAPDB_Int4 RelationalDef::TransformForeignKeys( StudioOAL_String schemaname, StudioOAL_String tablename )
{
    ROUTINE_DBG_MEO00 ("RelationalDef::TransformForeignKeys");
    SAPDB_Int4 rc = errOK_els00;
    /*-------------------------------------------------------------------------*
     * Read/Write the foreignkey definition of the specified table from/to resources
     *-------------------------------------------------------------------------*/
    if (m_pTransformationCmd->trCatalogFile_ls00.fsFormat_ls00.ffoDdl_ls00 == true)
    {
        m_pTableDef->readForeignKeyDef (schemaname, tablename);
        m_pTableDef->writeForeignKeyDef(schemaname, tablename);
    }
    return rc;
}
/*===========================================================================*
 *  RelationalDef::TransformIndexes
 *===========================================================================*/
SAPDB_Int4 RelationalDef::TransformIndexes( )
{
    ROUTINE_DBG_MEO00 ("RelationalDef::TransformIndexes");
    LoaderMessages *pMsgInstance = LoaderMessages::Instance();
    SAPDB_Int4 rc = errOK_els00;

    if ( m_pTransformationCmd->trCatalogFile_ls00.fsFormat_ls00.ffoDdl_ls00 == true )
    {
        TransformationDef *pTransformationDef = getMetaData()->getTransformationModel();
        tls00_DBInfo      *pDBInfo            = getMetaData()->getConnection();
        SQLRETURN          ReturnCode = 0;
        StudioOAL_String   NativeErrorAsString;

        m_pRelationalStmt[0]=0;
        sp77sprintf(m_pRelationalStmt, 1024, CONST_CAST(SAPDB_Char*, Statement(sel_Transformation_703).CharPtr()),
                                            m_pTransformationCmd->trPackageid_ls00.StrPtr(),
                                            _T(" AND CMD LIKE 'TABLE %' "),
                                            _T(""));

        if ( !pTransformationDef->executeSQL(m_pRelationalStmt) )
        {
            ReturnCode          = pTransformationDef->getReturnCode();
            NativeErrorAsString = pTransformationDef->getNativeErrorAsString();
            pMsgInstance->ls98PrintMessage(layCatalogExtract_els98, errODBCErrors_els98, NativeErrorAsString.StrPtr());
            return ReturnCode;
        }
        StudioOAL_String sessionUser, currentUser, sessionSchema, currentSchema;
        sessionUser.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer , pDBInfo->dbiSession_ls00.siUserName_ls00.asCharp()));
        currentUser = sessionUser;
        sessionSchema.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer, pDBInfo->dbiSession_ls00.siSchemaName_ls00));
        currentSchema = sessionSchema;
        pTransformationDef->m_currentStream = stCatalog_els00;
        /*-------------------------------------------------------------------------------*
        * Read/Write the CATALOG definition of the specified object from/to resources   *
        *-------------------------------------------------------------------------------*/
        while( pTransformationDef->initTransformationStep() )
        {
            pTransformationDef->switchUserSchema ( currentUser ,currentSchema );
            currentUser   = pTransformationDef->m_owner ;
            currentSchema = pTransformationDef->m_schemaname ;
            TransformIndexes(m_schemaname,m_tablename);
        }
    }
    return rc;
}
/*===========================================================================*
 *  RelationalDef::TransformIndexes
 *===========================================================================*/
SAPDB_Int4 RelationalDef::TransformIndexes( StudioOAL_String& schemaname, StudioOAL_String& tablename )
{
    ROUTINE_DBG_MEO00 ("RelationalDef::TransformIndexes");
    LoaderMessages *pMsgInstance = LoaderMessages::Instance();
    SAPDB_Int4 rc = errOK_els00;
    
    m_pRelationalStmt[0]=0;
    StudioOAL_String record[1];
    StudioOAL_String schema_name;

    if (SAPDB_TRUE == schemaname.Empty())
    {
        sp77sprintf(m_pRelationalStmt, 1024,  CONST_CAST(SAPDB_Char*, Statement(sel_Relational_13).CharPtr()), tablename.StrPtr());
        if (SAPDB_FALSE == m_pIndexDef->executeSQL(m_pRelationalStmt))
        {
            rc = errODBCErrors_els98;
        }
        else
        {
            m_pIndexDef->readResult(record);
            if (AtoI(record[0]) > 1)
            {
                pMsgInstance->ls98PrintMessage(layCatalogExtract_els98, errDEAmbiguousTableName_els98);
                rc = errDEAmbiguousTableName_els98;
            }
        }
    }
    if (errOK_els00 == rc)
    {
        /*-------------------------------------------------------------------------*
         * Prepare the WHERE clause for the SQL statements.
         *-------------------------------------------------------------------------*/
        if (SAPDB_FALSE == schemaname.Empty())
            schema_name  = schemaname;
        else
            schema_name  = "%s";
        //<create_index_statement> ::=
        // CREATE [UNIQUE] INDEX <index_name> ON <table_name> (<column_name> [ASC | DESC],...)
        m_pRelationalStmt[0]=0;
        sp77sprintf(m_pRelationalStmt, 1024,  CONST_CAST(SAPDB_Char*, Statement(sel_Relational_9).CharPtr()),
                                              schema_name.StrPtr(),tablename.StrPtr());
        if ( !m_pIndexDef->executeSQL(m_pRelationalStmt) )
             rc = STATIC_CAST(SAPDB_Int4, m_pIndexDef->getNativeError());
        if (errOK_els00 == rc)
        {
            /*-------------------------------------------------------------------------*
             * Read/Write the index definition of the specified table from/to resources
             *-------------------------------------------------------------------------*/
            if (m_pTransformationCmd->trCatalogFile_ls00.fsFormat_ls00.ffoDdl_ls00 == true||
                m_pTransformationCmd->trCmdFile_ls00.fsFileName_ls00 != NULL )
            {
                while ( m_pIndexDef->readIndexDefinitionDirect( schemaname, tablename ) )
                        m_pIndexDef->writeIndexDefinition();
            }
            if (m_pTransformationCmd->trCatalogFile_ls00.fsFormat_ls00.ffoXml_ls00 == true)
            {
                StudioOAL_String tableIndices;
                SAPDB_Bool       bHasIndex = SAPDB_FALSE;

                while ( m_pIndexDef->readIndexDefinitionDirect(schemaname, tablename) )
                {
                    if (SAPDB_FALSE == bHasIndex)
                    {
                        tableIndices = "<dbm:TableIndices>\n";
                        m_pIndexDef->writeLine( tableIndices );
                        bHasIndex = SAPDB_TRUE;
                    }
                    m_pIndexDef->writeIndexDefinition();
                }
                if (SAPDB_TRUE == bHasIndex)
                {
                    tableIndices = "</dbm:TableIndices>\n";
                    m_pIndexDef->writeLine( tableIndices );
                }
            }
        }
    }// if (errOK_els00 == rc)
    return rc;
}
/*===========================================================================*
 *  RelationalDef::TransformIndexes_
 *===========================================================================*/
SAPDB_Int4 RelationalDef::TransformIndexes_ ( StudioOAL_String schemaname, StudioOAL_String tablename )
{
    ROUTINE_DBG_MEO00 ("RelationalDef::TransformIndexes_");

    SAPDB_Int4 rc = errOK_els00;

    /*-------------------------------------------------------------------------*
     * Read/Write the index definition of the specified table from/to resources
     *-------------------------------------------------------------------------*/
    m_pIndexDef->setIndexParameters( schemaname, tablename ) ;

    if (m_pTransformationCmd->trCatalogFile_ls00.fsFormat_ls00.ffoDdl_ls00 == true)
    {
         while ( m_pIndexDef->readIndexDefinition( schemaname, tablename ) )
                 m_pIndexDef->writeIndexDefinition();
    }
    if (m_pTransformationCmd->trCatalogFile_ls00.fsFormat_ls00.ffoXml_ls00 == true)
    {
        StudioOAL_String tableIndices;
        SAPDB_Bool       bHasIndex = SAPDB_FALSE;

        while ( m_pIndexDef->readIndexDefinition(schemaname, tablename) )
        {
            if (SAPDB_FALSE == bHasIndex)
            {
                tableIndices = "<dbm:TableIndices>\n";
                m_pIndexDef->writeLine( tableIndices );
                bHasIndex = SAPDB_TRUE;
            }
            m_pIndexDef->writeIndexDefinition ();
        }
        if (SAPDB_TRUE == bHasIndex)
        {
            tableIndices   = "</dbm:TableIndices>\n";
            m_pIndexDef->writeLine ( tableIndices );
        }
    }
    return rc;
}
/*===========================================================================*
 *  RelationalDef::TransformSequences                                        *
 *===========================================================================*/
SAPDB_Int4 RelationalDef::TransformSequences( )
{
    ROUTINE_DBG_MEO00 ("RelationalDef::TransformSequences");
    LoaderMessages *pMsgInstance = LoaderMessages::Instance();
    SAPDB_Int4 rc = errOK_els00;

    TransformationDef *pTransformationDef = getMetaData()->getTransformationModel();
    tls00_DBInfo      *pDBInfo            = getMetaData()->getConnection();
    StudioOAL_String   record[9];
	StudioOAL_String   NativeErrorAsString;

    if ( m_pTransformationCmd->trCatalogFile_ls00.fsFormat_ls00.ffoDdl_ls00 == true )
    {
        /*-------------------------------------------------------------------------*
         * Prepare the the SQL statements.                                         *
         *-------------------------------------------------------------------------*/
        m_pRelationalStmt[0] = 0;
        if( ptAll_els00 == m_pTransformationCmd->trPartSpec_ls00.psPartType_ls00 ) {
            sp77sprintf(m_pRelationalStmt, 1024, CONST_CAST(SAPDB_Char*, Statement(sel_Transformation_706).CharPtr()),
                        m_pTransformationCmd->trPackageid_ls00.StrPtr());
        }
        else if( ptUser_els00 == m_pTransformationCmd->trPartSpec_ls00.psPartType_ls00 ) {
            sp77sprintf(m_pRelationalStmt, 1024, CONST_CAST(SAPDB_Char*, Statement(sel_Transformation_707).CharPtr()),
                        m_pTransformationCmd->trPackageid_ls00.StrPtr(),m_pTransformationCmd->trPartSpec_ls00.psTabOwner_ls00.asCharp());
        }
        else if( ptSchema_els00 == m_pTransformationCmd->trPartSpec_ls00.psPartType_ls00 ) {
            sp77sprintf(m_pRelationalStmt, 1024, CONST_CAST(SAPDB_Char*, Statement(sel_Transformation_720).CharPtr()), 
                        m_pTransformationCmd->trPackageid_ls00.StrPtr(),m_pTransformationCmd->trPartSpec_ls00.psTabSchema_ls00.asCharp());
        }
        
        if ( pTransformationDef->executeSQL( m_pRelationalStmt ) )
        {
            /*-------------------------------------------------------------------------------*
             * Read/Write the definition of the specified object from/to resources           *
             *-------------------------------------------------------------------------------*/
            StudioOAL_String sessionUser, currentUser, sessionSchema, currentSchema;
            sessionUser.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer, pDBInfo->dbiSession_ls00.siUserName_ls00.asCharp()));
            currentUser = sessionUser;
            sessionSchema.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer, pDBInfo->dbiSession_ls00.siSchemaName_ls00));
            currentSchema = sessionSchema;

            pTransformationDef->m_currentStream = stCatalog_els00;

            /*-------------------------------------------------------------------------------*
            * Read/Write the CATALOG definition of the specified object from/to resources   *
            *-------------------------------------------------------------------------------*/
            while( pTransformationDef->initTransformationStep() )
            {
                pTransformationDef->switchUserSchema ( currentUser ,currentSchema );

                currentUser   = pTransformationDef->m_owner ;
                currentSchema = pTransformationDef->m_schemaname ;
            
                /*-------------------------------------------------------------------------*
                 * Prepare the the SQL statements.                                         *
                 *-------------------------------------------------------------------------*/
                m_pRelationalStmt[0] = 0;
                if (ptAll_els00 == m_pTransformationCmd->trPartSpec_ls00.psPartType_ls00) {
                    sp77sprintf(m_pRelationalStmt, 1024, CONST_CAST(SAPDB_Char*, Statement(sel_Relational_3).CharPtr()),
                                                         m_schemaname.StrPtr());
                }
                if (ptUser_els00 == m_pTransformationCmd->trPartSpec_ls00.psPartType_ls00) {
                    sp77sprintf(m_pRelationalStmt, 1024, CONST_CAST(SAPDB_Char*, Statement(sel_Relational_3).CharPtr()),
                                                         m_owner.StrPtr());
                }
                if (ptSchema_els00 == m_pTransformationCmd->trPartSpec_ls00.psPartType_ls00) {
                    sp77sprintf(m_pRelationalStmt, 1024, CONST_CAST(SAPDB_Char*, Statement(sel_Relational_3).CharPtr()),
                                                         m_schemaname.StrPtr());
                }
                if ( ! m_pSequenceDef->executeSQL( m_pRelationalStmt )  )
                {
                    NativeErrorAsString = m_pSequenceDef->getNativeErrorAsString();
                    pMsgInstance->ls98PrintMessage(layCatalogExtract_els98, errODBCErrors_els98, NativeErrorAsString.StrPtr());
                    return m_pSequenceDef->getReturnCode();
                }
                /*----------------------------------------------------------------------------*
                 * Read/Write the sequence definition of the specified table from/to resources*
                 *----------------------------------------------------------------------------*/
                while ( m_pSequenceDef->readResult (record) )
                        m_pSequenceDef->writeResult(record);
            }// while ( pTransformationDef->initTransformationStep() )
        }// if ( pTransformationDef->executeSQL( statement ) )
    }
    return rc;
}
/*===========================================================================*
 *  RelationalDef::TransformProcedures
 *===========================================================================*/
SAPDB_Int4 RelationalDef::TransformProcedures( )
{
    ROUTINE_DBG_MEO00 ("RelationalDef::TransformProcedures");
    LoaderMessages *pMsgInstance = LoaderMessages::Instance();

    SAPDB_Int4        rc          = errOK_els00;
    SQLRETURN         ReturnCode  = 0;
    StudioOAL_String  NativeErrorAsString;
    StudioOAL_String  record[5];

    TransformationDef *pTransformationDef = getMetaData()->getTransformationModel();
    tls00_DBInfo      *pDBInfo            = getMetaData()->getConnection();
    m_pRelationalStmt[0] = 0;

    if ( m_pTransformationCmd->trCatalogFile_ls00.fsFormat_ls00.ffoDdl_ls00 == true )
    {
        m_pRelationalStmt[0] = 0;
        if( ptAll_els00 == m_pTransformationCmd->trPartSpec_ls00.psPartType_ls00 ) {
            sp77sprintf(m_pRelationalStmt, 1024, CONST_CAST(SAPDB_Char*, Statement(sel_Transformation_706).CharPtr()),
                        m_pTransformationCmd->trPackageid_ls00.StrPtr());
        }
        else if( ptUser_els00 == m_pTransformationCmd->trPartSpec_ls00.psPartType_ls00 ) {
            sp77sprintf(m_pRelationalStmt, 1024, CONST_CAST(SAPDB_Char*, Statement(sel_Transformation_707).CharPtr()),
                        m_pTransformationCmd->trPackageid_ls00.StrPtr(),m_pTransformationCmd->trPartSpec_ls00.psTabOwner_ls00.asCharp());
        }
        else if( ptSchema_els00 == m_pTransformationCmd->trPartSpec_ls00.psPartType_ls00 ) {
            sp77sprintf(m_pRelationalStmt, 1024, CONST_CAST(SAPDB_Char*, Statement(sel_Transformation_720).CharPtr()),
                        m_pTransformationCmd->trPackageid_ls00.StrPtr(),m_pTransformationCmd->trPartSpec_ls00.psTabSchema_ls00.asCharp());
        }
        if ( !pTransformationDef->executeSQL(m_pRelationalStmt) )
        {
            NativeErrorAsString = pTransformationDef->getNativeErrorAsString();
            pMsgInstance->ls98PrintMessage(layCatalogExtract_els98, errODBCErrors_els98, NativeErrorAsString.StrPtr());
            rc = pTransformationDef->getReturnCode();
        }
        else
        {
            StudioOAL_String sessionUser, currentUser, sessionSchema, currentSchema;
            sessionUser.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer, pDBInfo->dbiSession_ls00.siUserName_ls00.asCharp()));
            currentUser = sessionUser;
            sessionSchema.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer, pDBInfo->dbiSession_ls00.siSchemaName_ls00));
            currentSchema = sessionSchema;

            pTransformationDef->m_currentStream = stCatalog_els00;

            /*-------------------------------------------------------------------------------*
            * Read/Write the CATALOG definition of the specified object from/to resources   *
            *-------------------------------------------------------------------------------*/
            while( pTransformationDef->initTransformationStep() && errOK_els00 == rc)
            {
                pTransformationDef->switchUserSchema ( currentUser ,currentSchema );

                currentUser   = pTransformationDef->m_owner ;
                currentSchema = pTransformationDef->m_schemaname ;
          
                m_pRelationalStmt [0] = 0;
          
                sp77sprintf(m_pRelationalStmt, 1024, CONST_CAST(SAPDB_Char*, Statement(sel_Relational_4).CharPtr()), m_owner.StrPtr());
                if ( !m_pProcedureDef->executeSQL( m_pRelationalStmt ) )
                {
                    NativeErrorAsString = m_pProcedureDef->getNativeErrorAsString();
                    pMsgInstance->ls98PrintMessage(layCatalogExtract_els98, errODBCErrors_els98, NativeErrorAsString.StrPtr());
                    rc = m_pProcedureDef->getReturnCode();
                }
                else
                {
                    /*-------------------------------------------------------------------------------*
                     * Read/Write the dbprocedure definition of the specified table from/to resources*
                     *-------------------------------------------------------------------------------*/
                    while ( m_pProcedureDef->readResult (record) )
                            m_pProcedureDef->writeResult() ;
                }
            } // while( pTransformationDef->initTransformationStep())
        }
     }
     return rc;
}
/*===========================================================================*
 *  RelationalDef::TransformTriggers
 *===========================================================================*/
SAPDB_Int4 RelationalDef::TransformTriggers( )
{
    ROUTINE_DBG_MEO00 ("RelationalDef::TransformTriggers");
    LoaderMessages *pMsgInstance = LoaderMessages::Instance();
    SAPDB_Int4 rc = errOK_els00;

    if ( m_pTransformationCmd->trCatalogFile_ls00.fsFormat_ls00.ffoDdl_ls00 == true )
    {
        TransformationDef *pTransformationDef = getMetaData()->getTransformationModel();
        tls00_DBInfo      *pDBInfo            = getMetaData()->getConnection();
        SQLRETURN          ReturnCode = 0;
        StudioOAL_String   NativeErrorAsString;

        m_pRelationalStmt[0]=0;
        sp77sprintf(m_pRelationalStmt, 1024, CONST_CAST(SAPDB_Char*, Statement(sel_Transformation_703).CharPtr()),
                                            m_pTransformationCmd->trPackageid_ls00.StrPtr(),
                                            _T(" AND CMD LIKE 'TABLE %' "),
                                            _T(""));

        if ( !pTransformationDef->executeSQL(m_pRelationalStmt) )
        {
            ReturnCode          = pTransformationDef->getReturnCode();
            NativeErrorAsString = pTransformationDef->getNativeErrorAsString();
            pMsgInstance->ls98PrintMessage(layCatalogExtract_els98, errODBCErrors_els98, NativeErrorAsString.StrPtr());
            return ReturnCode;
        }
        StudioOAL_String sessionUser, currentUser, sessionSchema, currentSchema;
        sessionUser.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer , pDBInfo->dbiSession_ls00.siUserName_ls00.asCharp()));
        currentUser = sessionUser;
        sessionSchema.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer, pDBInfo->dbiSession_ls00.siSchemaName_ls00));
        currentSchema = sessionSchema;
        pTransformationDef->m_currentStream = stCatalog_els00;
        /*-------------------------------------------------------------------------------*
        * Read/Write the CATALOG definition of the specified object from/to resources   *
        *-------------------------------------------------------------------------------*/
        while( pTransformationDef->initTransformationStep() )
        {
            pTransformationDef->switchUserSchema ( currentUser ,currentSchema );
            currentUser   = pTransformationDef->m_owner ;
            currentSchema = pTransformationDef->m_schemaname ;
            TransformTrigger(m_schemaname,m_tablename);
        }
    }
    return rc;
}
/*===========================================================================*
 *  RelationalDef::TransformFunction
 *===========================================================================*/
SAPDB_Int4 RelationalDef::TransformFunction()
{
    ROUTINE_DBG_MEO00 ("RelationalDef::TransformFunction");
    LoaderMessages *pMsgInstance = LoaderMessages::Instance();

    SAPDB_Int4        rc          = errOK_els00;
    SQLRETURN         ReturnCode  = 0;
    StudioOAL_String  NativeErrorAsString;
    StudioOAL_String  record[5];

    TransformationDef *pTransformationDef = getMetaData()->getTransformationModel();
    tls00_DBInfo      *pDBInfo            = getMetaData()->getConnection();
    m_pRelationalStmt[0] = 0;

    if ( m_pTransformationCmd->trCatalogFile_ls00.fsFormat_ls00.ffoDdl_ls00 == true )
    {
        m_pRelationalStmt[0] = 0;
        if( ptAll_els00 == m_pTransformationCmd->trPartSpec_ls00.psPartType_ls00 ) {
            sp77sprintf(m_pRelationalStmt, 1024, CONST_CAST(SAPDB_Char*, Statement(sel_Transformation_706).CharPtr()),
                        m_pTransformationCmd->trPackageid_ls00.StrPtr());
        }
        else if( ptUser_els00 == m_pTransformationCmd->trPartSpec_ls00.psPartType_ls00 ) {
            sp77sprintf(m_pRelationalStmt, 1024, CONST_CAST(SAPDB_Char*, Statement(sel_Transformation_707).CharPtr()),
                        m_pTransformationCmd->trPackageid_ls00.StrPtr(),m_pTransformationCmd->trPartSpec_ls00.psTabOwner_ls00.asCharp());
        }
        else if( ptSchema_els00 == m_pTransformationCmd->trPartSpec_ls00.psPartType_ls00 ) {
            sp77sprintf(m_pRelationalStmt, 1024, CONST_CAST(SAPDB_Char*, Statement(sel_Transformation_720).CharPtr()),
                        m_pTransformationCmd->trPackageid_ls00.StrPtr(),m_pTransformationCmd->trPartSpec_ls00.psTabSchema_ls00.asCharp());
        }
        if ( !pTransformationDef->executeSQL(m_pRelationalStmt) )
        {
            NativeErrorAsString = pTransformationDef->getNativeErrorAsString();
            pMsgInstance->ls98PrintMessage(layCatalogExtract_els98, errODBCErrors_els98, NativeErrorAsString.StrPtr());
            rc = pTransformationDef->getReturnCode();
        }
        else
        {
            StudioOAL_String sessionUser, currentUser, sessionSchema, currentSchema;
            sessionUser.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer, pDBInfo->dbiSession_ls00.siUserName_ls00.asCharp()));
            currentUser = sessionUser;
            sessionSchema.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer, pDBInfo->dbiSession_ls00.siSchemaName_ls00));
            currentSchema = sessionSchema;

            pTransformationDef->m_currentStream = stCatalog_els00;

            /*-------------------------------------------------------------------------------*
            * Read/Write the CATALOG definition of the specified object from/to resources   *
            *-------------------------------------------------------------------------------*/
            while( pTransformationDef->initTransformationStep() && errOK_els00 == rc)
            {
                pTransformationDef->switchUserSchema ( currentUser ,currentSchema );

                currentUser   = pTransformationDef->m_owner ;
                currentSchema = pTransformationDef->m_schemaname ;
          
                m_pRelationalStmt [0] = 0;
          
                sp77sprintf(m_pRelationalStmt, 1024, CONST_CAST(SAPDB_Char*, Statement(sel_Relational_4).CharPtr()), m_owner.StrPtr());
                if ( !m_pProcedureDef->executeSQL( m_pRelationalStmt ) )
                {
                    NativeErrorAsString = m_pProcedureDef->getNativeErrorAsString();
                    pMsgInstance->ls98PrintMessage(layCatalogExtract_els98, errODBCErrors_els98, NativeErrorAsString.StrPtr());
                    rc = m_pProcedureDef->getReturnCode();
                }
                else
                {
                    /*-------------------------------------------------------------------------------*
                     * Read/Write the dbprocedure definition of the specified table from/to resources*
                     *-------------------------------------------------------------------------------*/
                    while ( m_pProcedureDef->readResult (record) )
                            m_pProcedureDef->writeResult() ;
                }
            } // while( pTransformationDef->initTransformationStep())
        }
     }
     return rc;
}
/*===========================================================================*
 *  RelationalDef::TransformTrigger
 *===========================================================================*/
SAPDB_Int4 RelationalDef::TransformTrigger( StudioOAL_String& schemaname, StudioOAL_String& tablename )
{
    ROUTINE_DBG_MEO00 ("RelationalDef::TransformTrigger");
    LoaderMessages *pMsgInstance = LoaderMessages::Instance();

    SAPDB_Int4       rc = errOK_els00;
    StudioOAL_String NativeErrorAsString;

    StudioOAL_String  record[5];
    /*-------------------------------------------------------------------------*
     * Prepare the SQL statement.
     *-------------------------------------------------------------------------*/
    m_pRelationalStmt [0] = 0;
    sp77sprintf(m_pRelationalStmt, 1024, CONST_CAST(SAPDB_Char*, Statement(sel_Relational_10).CharPtr()),
                                         schemaname.StrPtr(),tablename.StrPtr());
    
    if ( !m_pTriggerDef->executeSQL( m_pRelationalStmt )  )
    {
        NativeErrorAsString = m_pTriggerDef->getNativeErrorAsString();
        pMsgInstance->ls98PrintMessage(layCatalogExtract_els98, errODBCErrors_els98, NativeErrorAsString.StrPtr());
        rc = m_pTriggerDef->getReturnCode();
    }
    else
    {
        /*-------------------------------------------------------------------------------*
         * Read/Write the trigger definition of the specified table from/to resources    *
         *-------------------------------------------------------------------------------*/
        while ( m_pTriggerDef->readResult(record) )
                m_pTriggerDef->writeResult(record);
    }
    return rc;
}
/*===========================================================================*
 *  RelationalDef::TransformTrigger
 *===========================================================================*/
SAPDB_Int4 RelationalDef::TransformTrigger_( StudioOAL_String schemaname, StudioOAL_String tablename )
{
    ROUTINE_DBG_MEO00 ("RelationalDef::TransformTrigger_");

    SAPDB_Int4 rc = errOK_els00;

    /*-------------------------------------------------------------------------*
     * Read/Write the synonym definition of the specified table from/to resources
     *-------------------------------------------------------------------------*/
    if ( m_pTriggerDef->readTriggerDefinition( schemaname, tablename ) )
         m_pTriggerDef->writeTriggerDefinition();
     return rc;
}
/*===========================================================================*
 *    TransformationDef::TransformationDef
 *===========================================================================*/
TransformationDef::TransformationDef( sapdbwa_DBC & odbcConnection , MetaDataDef * pMetaData, tls00_TransformationCmd *pTransformationCmd   )
                  :MetaDataItem     ( odbcConnection , pMetaData, pTransformationCmd ) , 
                   m_pTransformationCmd( pTransformationCmd ), m_preparedSelect(""),m_preparedInsert(""),
                   m_existPackage(false), m_existPackageData(false), m_exclude(false),
                   m_nSchemaCount(0), m_nObjectCount(0), m_nTableCount(0), m_nTableData(0), m_nSynonymCount(0), m_nViewCount(0), 
                   m_exportcount (0), m_importcount (0)
{
    ROUTINE_DBG_MEO00 ("TransformationDef::TransformationDef()");

    m_pTransformationStmt[0] = 0;
    if ( pTransformationCmd->trTable_ls00.tsTabOwner_ls00.length() != 0 )
        m_owner = REINTERPRET_CAST(SAPDB_UTF8*, pTransformationCmd->trTable_ls00.tsTabOwner_ls00.asCharp());
    else
        m_owner  = REINTERPRET_CAST(SAPDB_UTF8*, pMetaData->getConnection()->dbiSession_ls00.siUserName_ls00.asCharp());
    
    if ( pTransformationCmd->trTable_ls00.tsTabSchema_ls00.length() != 0 )
        m_schemaname = REINTERPRET_CAST(SAPDB_UTF8*, pTransformationCmd->trTable_ls00.tsTabSchema_ls00.asCharp());
    else 
         m_schemaname = REINTERPRET_CAST(SAPDB_UTF8*, pMetaData->getConnection()->dbiSession_ls00.siSchemaName_ls00);

    m_tablename = REINTERPRET_CAST(SAPDB_UTF8*, pTransformationCmd->trTable_ls00.tsTabName_ls00.asCharp());

	if ( isSchema() == true )
		for( int i = 0 ; i < 6 ; ++ i)
			 m_packagepart[i] = pszPackageSchema_ls00[i];
	else
		for( int i = 0 ; i < 6 ; ++ i)
			 m_packagepart[i] = pszPackageOwner_ls00[i];

    m_pLoaderCmd  = new tls00_Command ;
    memset (m_pLoaderCmd, DEF_BYTE_LS00, sizeof  (tls00_Command));
    m_pLoaderCmd->cmdRpyDat_ls00    = new char[MAX_REPLY_LENGTH_LS00];
    memset (m_pLoaderCmd->cmdRpyDat_ls00, DEF_BYTE_LS00, MAX_REPLY_LENGTH_LS00);
    m_pLoaderCmd->cmdInternal_ls00  = SAPDB_TRUE;
    m_pLoaderCmd->cmdLogging_ls00   = SAPDB_TRUE;

    GUID_           *GuidInstance = GUID_::Instance();
    GUID_IdString    packageidString;
    GUID_Id          packageid;

    if ( true == m_pTransformationCmd->trRestart_ls00.rstRestart_ls00 )
         m_pTransformationCmd->trPackageid_ls00  = pTransformationCmd->trRestart_ls00.rstPackageid_ls00;
	else
    if ( false == m_pTransformationCmd->trInternal_ls00 ) 
    {
        GuidInstance->IdGetNext (packageid);
        GuidInstance->IdAsString(packageid, packageidString);
        m_pTransformationCmd->trRestart_ls00.rstPackageid_ls00.AssignRaw(REINTERPRET_CAST(unsigned char*,packageidString),STATIC_CAST(tsp00_Uint4,_tcslen(packageidString)));
        m_pTransformationCmd->trPackageid_ls00  = pTransformationCmd->trRestart_ls00.rstPackageid_ls00;
    }
    memset (&m_headerpos ,0, sizeof(tls00_LargeNumber));
    memset (&m_datapos   ,0, sizeof(tls00_LargeNumber));
    memset (&m_trailerpos,0, sizeof(tls00_LargeNumber));
    
    m_upd_Transformation_800 = Statement(upd_Transformation_800);
    m_upd_Transformation_801 = Statement(upd_Transformation_801);
    m_upd_Transformation_802 = Statement(upd_Transformation_802);
    m_upd_Transformation_803 = Statement(upd_Transformation_803);
    m_upd_Transformation_804 = Statement(upd_Transformation_804);
    m_upd_Transformation_805 = Statement(upd_Transformation_805);
    m_upd_Transformation_806 = Statement(upd_Transformation_806);
    m_upd_Transformation_807 = Statement(upd_Transformation_807);
    m_upd_Transformation_808 = Statement(upd_Transformation_808);
    m_upd_Transformation_809 = Statement(upd_Transformation_809);
    m_upd_Transformation_810 = Statement(upd_Transformation_810);
    m_upd_Transformation_811 = Statement(upd_Transformation_811);
    m_upd_Transformation_812 = Statement(upd_Transformation_812);
    m_upd_Transformation_813 = Statement(upd_Transformation_813);
                          
    m_pUpdate          = new ResultSet ( odbcConnection );

#ifdef ODBC_PREPARED
    m_pUpdateCatalog   = new ResultSet ( odbcConnection );
    m_pUpdatePages     = new ResultSet ( odbcConnection );
    m_pUpdateRecords   = new ResultSet ( odbcConnection );
    m_pUpdateRestart   = new ResultSet ( odbcConnection );
    m_pUpdateBegin     = new ResultSet ( odbcConnection );
    m_pUpdateHeader    = new ResultSet ( odbcConnection );
    m_pUpdateData      = new ResultSet ( odbcConnection );
    m_pUpdateEnd       = new ResultSet ( odbcConnection );
#endif
}
/*===========================================================================*
 *    TransformationDef::TransformationDef
 *===========================================================================*/
TransformationDef::TransformationDef( sapdbwa_DBC & odbcConnection   )
                  :MetaDataItem ( odbcConnection ),
                   m_existPackage(SAPDB_FALSE), m_existPackageData(SAPDB_FALSE), m_nSchemaCount(0), m_nObjectCount(0), m_nTableCount(0), m_nTableData(0), m_nSynonymCount(0), m_nViewCount(0), m_exclude(SAPDB_FALSE), m_exportcount(0), m_importcount(0)
{
    ROUTINE_DBG_MEO00 ("TransformationDef::TransformationDef()");

    m_pLoaderCmd  = new tls00_Command ;
    m_pLoaderCmd->cmdRpyDat_ls00    = new char[MAX_REPLY_LENGTH_LS00];
    m_pLoaderCmd->cmdInternal_ls00  = SAPDB_TRUE;
    m_pLoaderCmd->cmdLogging_ls00   = SAPDB_TRUE;
    memset (m_pLoaderCmd, DEF_BYTE_LS00, sizeof  (tls00_Command));
    memset (m_pLoaderCmd->cmdRpyDat_ls00, DEF_BYTE_LS00, MAX_REPLY_LENGTH_LS00);
    memset (&m_headerpos ,0, sizeof(tls00_LargeNumber));
    memset (&m_datapos   ,0, sizeof(tls00_LargeNumber));
    memset (&m_trailerpos,0, sizeof(tls00_LargeNumber));

}
/*===========================================================================*
 *    TransformationDef::TransformationDef
 *===========================================================================*/
TransformationDef::~TransformationDef()
{
    ROUTINE_DBG_MEO00 ("TransformationDef::~TransformationDef()");

    freeTransformationCmd();
    if (NULL != m_pLoaderCmd->cmdRpyDat_ls00)
    {
        delete [] m_pLoaderCmd->cmdRpyDat_ls00;
        m_pLoaderCmd->cmdRpyDat_ls00 = NULL;
    }
    if (NULL != m_pLoaderCmd)
    {
        delete [] m_pLoaderCmd;
        m_pLoaderCmd = NULL;
    }
    if ( NULL != m_pUpdate  )
    {
         delete  m_pUpdate;
         m_pUpdate  = NULL;
    }
#ifdef ODBC_PREPARED
    if ( NULL != m_pupdateCat )
    {
        delete  m_pupdateCat;
        m_pupdateCat = NULL;
    }
    if ( NULL != m_pUpdatePages    )
    {
        delete m_pUpdatePages;
        m_pUpdatePages = NULL;
    }
    if ( NULL != m_pUpdateRecords  )
    {
        delete m_pUpdateRecords;
        m_pUpdateRecords = NULL;
    }
    if ( NULL != m_pUpdateRestart  )
    {
        delete m_pUpdateRestart;
        m_pUpdateRestart = NULL;
    }
    if ( NULL != m_pUpdateBegin    )
    {
        delete m_pUpdateBegin;
        m_pUpdateBegin = NULL;
    }
    if ( NULL != m_pUpdateHeader   )
    {
        delete m_pUpdateHeader;
        m_pUpdateHeader = NULL;
    }
    if ( NULL != m_pUpdateData     )
    {
        delete m_pUpdateData;
        m_pUpdateData = NULL;
    }
    if ( NULL != m_pUpdateEnd      )
    {
        delete m_pUpdateEnd;
        m_pUpdateEnd = NULL;
    }
#endif
}
/*===========================================================================*
 *     TransformationDef::ImportPackage
 *===========================================================================*/
tsp00_Int4 TransformationDef::ImportPackage ( MetaDataDef *&pMetaDataDef, tsp00_Addr  ErrText)
{
    ROUTINE_DBG_MEO00 ("TransformationDef::ImportPackage");

    LoaderMessages *pMsgInstance = LoaderMessages::Instance();
    SAPDB_Long rc     = errOK_els00;
    SAPDB_Bool rcBool = SAPDB_TRUE;

    if ( getTransformationCmd()->trInternal_ls00 == false && NULL == getenv ("maxdbloaderserver")) 
    {
        RelationalDef  *pRelationalDef = pMetaDataDef->getRelationalModel();
        tls00_DBInfo   *pDBInfo        = pMetaDataDef->getConnection();
        
        if ( errOK_els00 == ls15CheckConnection(pDBInfo, ErrText ) )
        {
            SQLRETURN        ReturnCode = 0;
            StudioOAL_String NativeErrorAsString;
            Tools_DynamicUTF8String  transformation;
            _TCHAR sztransformation[MAX_STMT_LENGTH_LS00];
            sztransformation[0] = 0;
            _TCHAR sztransformationfile[PATH_MXSP00];
            sztransformationfile[0] = 0;
            _TCHAR sztransformationstream[PATH_MXSP00];
            sztransformationstream[0] = 0;

		    m_currentStream = stPackage_els00;
            m_packageid     = m_pTransformationCmd->trPackageid_ls00;


            if( SAPDB_FALSE == ExistPackage(m_pTransformationCmd->trPackageid_ls00) &&
                SAPDB_FALSE == m_pTransformationCmd->trRestart_ls00.rstRestart_ls00 )
            {
                //*
                //*  transformation, catalog
                //*
                rcBool = initResultSet(pDBInfo->dbiOdbcConnection);
                if (SAPDB_FALSE == rcBool)
                {
                    // initResultSet fills the error object
                    ErrorHndl*       pErrorObj = pMetaDataDef->getError();
                    StudioOAL_String s         = pErrorObj->getErrorText();
                    pMsgInstance->ls98MsgToString(layTableExtract_els98, ErrText, errODBCErrors_els98, s.StrPtr());
                    rc = pErrorObj->getReturnCode();
                }
                //*
                //*  storagedevice, servernode, serverdb, username, codetype, codepage, transformation, part, streamformat, exclude
                //*
                // catalogstream
                if ( errOK_els00 == rc && 
                    NULL        != m_pTransformationCmd->trCatalogFile_ls00.fsFileName_ls00 &&
                    SAPDB_TRUE  == m_pTransformationCmd->trCatalogFile_ls00.fsCurrentStream_ls00 )
                {
                    if ( !updateCatalogCmd())
                    {
                        ReturnCode          = getReturnCode();
                        NativeErrorAsString = getNativeErrorAsString();

                        pMsgInstance->ls98PrintMessage(layTableExtract_els98, errODBCErrors_els98, NativeErrorAsString.StrPtr());
                    }
                    //*
                    //*  catalog mapping stream
                    //*
                    if( tsExport_ls00 == m_pTransformationCmd->trTransSpec_ls00 && 
                        ptTable_els00 <  m_pTransformationCmd->trPartSpec_ls00.psPartType_ls00 &&
                        m_pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoPage_ls00 == true )
                    {
                        StudioOAL_String  record[3];

                        if( ptAll_els00 == m_pTransformationCmd->trPartSpec_ls00.psPartType_ls00 ) {
                            sp77sprintf(sztransformation, 1024, CONST_CAST(SAPDB_Char*, Statement(sel_Transformation_706).CharPtr()),
                                                                m_pTransformationCmd->trPackageid_ls00.StrPtr());
                        }
                        else if( ptUser_els00 == m_pTransformationCmd->trPartSpec_ls00.psPartType_ls00 ) {
                            sp77sprintf(sztransformation, 1024, CONST_CAST(SAPDB_Char*, Statement(sel_Transformation_707).CharPtr()),
                                                                m_pTransformationCmd->trPackageid_ls00.StrPtr(),
                                                                currentUser().StrPtr());
                        }
                        else if( ptSchema_els00 == m_pTransformationCmd->trPartSpec_ls00.psPartType_ls00 ) {
                            sp77sprintf(sztransformation, 1024, CONST_CAST(SAPDB_Char*, Statement(sel_Transformation_720).CharPtr()),
                                                                m_pTransformationCmd->trPackageid_ls00.StrPtr(),
                                                                currentSchema().StrPtr());
                        }
                        transformation.Assign(REINTERPRET_CAST(SAPDB_UTF8*, sztransformation));

                        if (executeSQL( transformation ) )
                        {
                            while ( readResult(record) && pRelationalDef->SplitPackage() == false )
                            {
                                transformation  = "CURRENT USER ";
                                transformation += record[1];
                                pRelationalDef->executeSQL (transformation);
                                sztransformation[0] = 0;
                                sp77sprintf(sztransformation, MAX_STMT_LENGTH_LS00, CONST_CAST(SAPDB_Char*, Statement(sel_Transformation_719).CharPtr()),
                                                                                    m_pTransformationCmd->trPackageid_ls00.StrPtr());
                                transformation.Assign(REINTERPRET_CAST(SAPDB_UTF8*, sztransformation));
                                if ( pRelationalDef->executeSQL( transformation ) )
                                {
                                    pRelationalDef->readResult( record ) ;
                                    if ( AtoI ( record[0] ) > 0 )
                                        pRelationalDef->SplitPackage(true);
                                }
                            }
                            record[1].Assign(REINTERPRET_CAST(SAPDB_UTF8*, pDBInfo->dbiSession_ls00.siUserName_ls00.asCharp()));
                            transformation  = "CURRENT USER ";
                            transformation += "\"";
                            transformation += record[1];
                            transformation += "\"";
                            executeSQL (transformation);
                        }
                    }// if( tsExport_ls00 == m_pTransformationCmd->trTransSpec_ls00 )
                }
                // datastream
                if ( rc         == errOK_els00 && 
                    NULL       != m_pTransformationCmd->trDataFile_ls00.fsFileName_ls00 &&
                    SAPDB_TRUE == m_pTransformationCmd->trDataFile_ls00.fsCurrentStream_ls00  )
                {
                    if ( !updateDataCmd() )
                    {
                        ReturnCode          = getReturnCode();
                        NativeErrorAsString = getNativeErrorAsString();

                        pMsgInstance->ls98PrintMessage(layTableExtract_els98, errODBCErrors_els98, NativeErrorAsString.StrPtr());
                    }
                    //*
                    //*  data mapping stream
                    //*
                    if( tsExport_ls00 == m_pTransformationCmd->trTransSpec_ls00 && 
                        ptTable_els00 <  m_pTransformationCmd->trPartSpec_ls00.psPartType_ls00  )
                    {
                        if ( dtypePipe_ls00 == m_pTransformationCmd->trDataFile_ls00.fsDeviceType_ls00 )
                        {
                            sztransformation[0]     = 0;
                            sztransformationstream[0] = 0;
                            transformation.Erase();
                            sp77sprintf(sztransformationstream, PATH_MXSP00, "%s%0*d", m_pTransformationCmd->trDataFile_ls00.fsFileName_ls00, FILE_EXTENSION_SIZE_LS27, 0);
                            sp77sprintf(sztransformation, MAX_STMT_LENGTH_LS00, CONST_CAST(SAPDB_Char*, Statement(sel_Transformation_715).CharPtr()),
                                                                                m_pTransformationCmd->trPackageid_ls00.StrPtr(),
                                                                                pszDeviceTypeName_ls00[m_pTransformationCmd->trDataFile_ls00.fsDeviceType_ls00],
                                                                                sztransformationstream);
                            transformation.Assign(REINTERPRET_CAST(SAPDB_UTF8*, sztransformation));
                            setTransformationStmt(transformation);
                            initTransformationCmd();
                            rc = ExportPackageDataRecord( pMetaDataDef, pDBInfo, ErrText);
                            freeTransformationCmd(srFctColExtract_els00);
                        }
                    }//
                    if( tsImport_ls00 == m_pTransformationCmd->trTransSpec_ls00 && 
                        ptTable_els00 <  m_pTransformationCmd->trPartSpec_ls00.psPartType_ls00  )
                    {
                        _TCHAR sztransformationebid[PATH_MXSP00];
                        sztransformationstream[0] = 0;
                        sztransformationebid[0] = 0;
                        sztransformation[0] = 0;

                        {
                            sp77sprintf(sztransformation, MAX_STMT_LENGTH_LS00, CONST_CAST(SAPDB_Char*, Statement(del_Transformation_601).CharPtr()),
                                                                                m_pTransformationCmd->trPackageid_ls00.StrPtr());
                            executeSQL(sztransformation);
                        }
                        if ( dtypePipe_ls00 == m_pTransformationCmd->trDataFile_ls00.fsDeviceType_ls00 )
                        {
                            sp77sprintf(sztransformationstream, PATH_MXSP00, "%s%0*d", m_pTransformationCmd->trDataFile_ls00.fsFileName_ls00, FILE_EXTENSION_SIZE_LS31, 0);
                            sp77sprintf(sztransformation, MAX_STMT_LENGTH_LS00, CONST_CAST(SAPDB_Char*, m_upd_Transformation_809.CharPtr()),
                                                                                m_pTransformationCmd->trPackageid_ls00.StrPtr(),
                                                                                m_packagepart[m_pTransformationCmd->trPartSpec_ls00.psPartType_ls00].StrPtr(),
                                                                                pszDeviceTypeName_ls00[m_pTransformationCmd->trDataFile_ls00.fsDeviceType_ls00],
                                                                                _T(" "),
                                                                                sztransformationstream);
                            transformation.Assign(REINTERPRET_CAST(SAPDB_UTF8*, sztransformation));
                        }
                        if ( dtypeFile_ls00 == m_pTransformationCmd->trDataFile_ls00.fsDeviceType_ls00 )
                        {
                            sp77sprintf(sztransformationstream, PATH_MXSP00, "%s%0*d", m_pTransformationCmd->trDataFile_ls00.fsFileName_ls00, FILE_EXTENSION_SIZE_LS31, 0);
                            sp77sprintf(sztransformation, MAX_STMT_LENGTH_LS00, CONST_CAST(SAPDB_Char*, m_upd_Transformation_809.CharPtr()),
                                                                                m_pTransformationCmd->trPackageid_ls00.StrPtr(),
                                                                                m_packagepart[m_pTransformationCmd->trPartSpec_ls00.psPartType_ls00].StrPtr(),
                                                                                pszDeviceTypeName_ls00[m_pTransformationCmd->trDataFile_ls00.fsDeviceType_ls00],
                                                                                _T(" "),
                                                                                sztransformationstream);
                            transformation.Assign(REINTERPRET_CAST(SAPDB_UTF8*, sztransformation));
                        }
                        else if ( dtypeTape_ls00 == m_pTransformationCmd->trDataFile_ls00.fsDeviceType_ls00 )
                        {
                            sp77sprintf(sztransformationebid  , PATH_MXSP00, "EBID '%s' ", m_pTransformationCmd->trDataFile_ls00.fsBackupId_ls00.bBackupIdName_ls00 );
                            sp77sprintf(sztransformationstream, PATH_MXSP00, "%s"        , m_pTransformationCmd->trDataFile_ls00.fsFileName_ls00 );
                            sp77sprintf(sztransformation, MAX_STMT_LENGTH_LS00, CONST_CAST(SAPDB_Char*, m_upd_Transformation_809.CharPtr()),
                                                                                m_pTransformationCmd->trPackageid_ls00.StrPtr(),
                                                                                m_packagepart[m_pTransformationCmd->trPartSpec_ls00.psPartType_ls00].StrPtr(),
                                                                                pszDeviceTypeName_ls00[m_pTransformationCmd->trDataFile_ls00.fsDeviceType_ls00],
                                                                                sztransformationebid,
                                                                                sztransformationstream);
                            transformation.Assign(REINTERPRET_CAST(SAPDB_UTF8*, sztransformation));
                        }
                        setTransformationStmt(transformation);
                        initTransformationCmd();
                        rc = UpdatePackageDataRecord( pMetaDataDef, pDBInfo, ErrText);
                        freeTransformationCmd(srFctColUpdate_els00);
                    }// if ( tsImport_ls00 == m_pTransformationCmd->trTransSpec_ls00 )
                }// if ( NULL != m_pTransformationCmd->trDataFile_ls00.fsFileName_ls00 )
                //*
                //*  configuration file
                //*
                if ( NULL != m_pTransformationCmd->trConfigurationFile_ls00.fsFileName_ls00 && errOK_els00 == rc  )
                {
                    //*
                    //* transformation command
                    //*
                    if ( ptTable_els00 < m_pTransformationCmd->trPartSpec_ls00.psPartType_ls00 )
                        pMsgInstance->ls98PrintMessage(layDataExtract_els98, msgCommandExecuting_els98, "CONFIGURE PACKAGE TO TRANSFORM");

                    sp77sprintf(sztransformation, MAX_STMT_LENGTH_LS00, CONST_CAST(SAPDB_Char*, m_upd_Transformation_810.CharPtr()),
                                                                        m_pTransformationCmd->trPackageid_ls00.StrPtr(),
                                                                        m_pTransformationCmd->trConfigurationFile_ls00.fsFileName_ls00,
                                                                        pszCodeTypeName_ls00[m_pTransformationCmd->trConfigurationFile_ls00.fsFormat_ls00.ffoCodeType_ls00] );
                    transformation.Assign(REINTERPRET_CAST(SAPDB_UTF8*, sztransformation));
                    setTransformationStmt(transformation);
                    initTransformationCmd();
                    UpdatePackageDataRecord( pMetaDataDef, pDBInfo, ErrText);
                    freeTransformationCmd(srFctColUpdate_els00);

                    if ( pDBInfo->dbiSession_ls00.siConfigurationType_ls00 == ctInclude_els00 ) {
                         sp77sprintf(sztransformation, MAX_STMT_LENGTH_LS00, CONST_CAST(SAPDB_Char*, m_upd_Transformation_801.CharPtr()),
                                                                        " DECODE(EXCLUDE,FALSE,TRUE,NULL,FALSE) ", m_pTransformationCmd->trPackageid_ls00.StrPtr());
                    }
                    if ( pDBInfo->dbiSession_ls00.siConfigurationType_ls00 == ctExclude_els00 ) {
                         sp77sprintf(sztransformation, MAX_STMT_LENGTH_LS00, CONST_CAST(SAPDB_Char*, m_upd_Transformation_801.CharPtr()),
                                                                        " DECODE(EXCLUDE,NULL,TRUE,FALSE,FALSE) ", m_pTransformationCmd->trPackageid_ls00.StrPtr());
                    }
                    transformation.Assign(REINTERPRET_CAST(SAPDB_UTF8*, sztransformation));
                    executeSQL( transformation );

                    if ( pDBInfo->dbiSession_ls00.siConfigurationType_ls00 == ctInclude_els00 ) {
                         sp77sprintf(sztransformation, MAX_STMT_LENGTH_LS00, CONST_CAST(SAPDB_Char*, Statement(sel_Transformation_700).CharPtr()),
                                                                        "FALSE", m_pTransformationCmd->trPackageid_ls00.StrPtr());
                    }
                    if ( pDBInfo->dbiSession_ls00.siConfigurationType_ls00 == ctExclude_els00 ) {
                         sp77sprintf(sztransformation, MAX_STMT_LENGTH_LS00, CONST_CAST(SAPDB_Char*, Statement(sel_Transformation_700).CharPtr()),
                                                                        "TRUE", m_pTransformationCmd->trPackageid_ls00.StrPtr());
                    }
                    transformation.Assign(REINTERPRET_CAST(SAPDB_UTF8*, sztransformation));

                    if ( executeSQL( transformation ) )
                    {
                        StudioOAL_String  record[1];
                        readResult( record ) ;
                        m_nTableData = AtoI( record[0] );
                        pMsgInstance->ls98PrintMessage(layDataExtract_els98, msgNumTabExIncludedTransport_els98, pszConfigurationTypeName_ls00[pDBInfo->dbiSession_ls00.siConfigurationType_ls00], record[0].StrPtr() );
                    }
                }// if ( NULL != m_pTransformationCmd->trConfigurationFile_ls00.fsFileName_ls00 )
                //*
                //*  protocol file
                //*
                updateTaskStatistic();
            }
            else if 
            (SAPDB_TRUE  == ExistPackage    (m_pTransformationCmd->trPackageid_ls00) &&
             SAPDB_FALSE == ExistPackageData(m_pTransformationCmd->trPackageid_ls00) &&
             SAPDB_FALSE == m_pTransformationCmd->trRestart_ls00.rstRestart_ls00   )
            {
                // datastream
                if( NULL       != m_pTransformationCmd->trDataFile_ls00.fsFileName_ls00 &&
                    SAPDB_TRUE == m_pTransformationCmd->trDataFile_ls00.fsCurrentStream_ls00)
                {
                    if ( !updateDataCmd())
                    {
                        //ReturnCode          = getReturnCode();
                        NativeErrorAsString = getNativeErrorAsString();
                        pMsgInstance->ls98PrintMessage(layTableExtract_els98, errODBCErrors_els98, NativeErrorAsString.StrPtr());
                    }
                }
                //*
                //*  data mapping stream
                //*
                if( tsExport_ls00 == m_pTransformationCmd->trTransSpec_ls00 && 
                    ptTable_els00 <  m_pTransformationCmd->trPartSpec_ls00.psPartType_ls00  )
                {
                    if ( dtypePipe_ls00 == m_pTransformationCmd->trDataFile_ls00.fsDeviceType_ls00 )
                    {
                        sztransformation[0]     = 0;
                        sztransformationstream[0] = 0;
                        transformation.Erase();
                        sp77sprintf(sztransformationstream, PATH_MXSP00, "%s%0*d", m_pTransformationCmd->trDataFile_ls00.fsFileName_ls00, FILE_EXTENSION_SIZE_LS27, 0);
                        sp77sprintf(sztransformation, MAX_STMT_LENGTH_LS00, CONST_CAST(SAPDB_Char*, Statement(sel_Transformation_715).CharPtr()),
                                                                            m_pTransformationCmd->trPackageid_ls00.StrPtr(),
                                                                            pszDeviceTypeName_ls00[m_pTransformationCmd->trDataFile_ls00.fsDeviceType_ls00],
                                                                            sztransformationstream);
                        transformation.Assign(REINTERPRET_CAST(SAPDB_UTF8*, sztransformation));
                        setTransformationStmt(transformation);
                        initTransformationCmd();
                        rc = ExportPackageDataRecord( pMetaDataDef, pDBInfo, ErrText);
                        freeTransformationCmd(srFctColExtract_els00);
                    }
                }
                if( tsImport_ls00 == m_pTransformationCmd->trTransSpec_ls00 && 
                    ptTable_els00 <  m_pTransformationCmd->trPartSpec_ls00.psPartType_ls00  )
                {
                    {
                        sp77sprintf(sztransformation, MAX_STMT_LENGTH_LS00, CONST_CAST(SAPDB_Char*, Statement(del_Transformation_601).CharPtr()),
                                                                            m_pTransformationCmd->trPackageid_ls00.StrPtr());
                        executeSQL(sztransformation);
                    }

                    _TCHAR sztransformationebid[PATH_MXSP00];

                    if ( dtypePipe_ls00 == m_pTransformationCmd->trDataFile_ls00.fsDeviceType_ls00 )
                    {
                        sp77sprintf(sztransformationstream, PATH_MXSP00, "%s%0*d", m_pTransformationCmd->trDataFile_ls00.fsFileName_ls00, FILE_EXTENSION_SIZE_LS31, 0);
                        sp77sprintf(sztransformation, MAX_STMT_LENGTH_LS00, CONST_CAST(SAPDB_Char*, m_upd_Transformation_809.CharPtr()),
                                                                            m_pTransformationCmd->trPackageid_ls00.StrPtr(),
                                                                            m_packagepart[m_pTransformationCmd->trPartSpec_ls00.psPartType_ls00].StrPtr(),
                                                                            pszDeviceTypeName_ls00[m_pTransformationCmd->trDataFile_ls00.fsDeviceType_ls00],
                                                                            _T(" "),
                                                                            sztransformationstream);
                        transformation.Assign(REINTERPRET_CAST(SAPDB_UTF8*, sztransformation));
                    }
                    else if ( dtypeFile_ls00 == m_pTransformationCmd->trDataFile_ls00.fsDeviceType_ls00 )
                    {
                        rc = sp77sprintf(sztransformationstream, PATH_MXSP00, "%s%0*d", m_pTransformationCmd->trDataFile_ls00.fsFileName_ls00, FILE_EXTENSION_SIZE_LS31, 0);
                        rc = sp77sprintf(sztransformation, MAX_STMT_LENGTH_LS00, CONST_CAST(SAPDB_Char*, m_upd_Transformation_809.CharPtr()),
                                                                                m_pTransformationCmd->trPackageid_ls00.StrPtr(),
                                                                                m_packagepart[m_pTransformationCmd->trPartSpec_ls00.psPartType_ls00].StrPtr(),
                                                                                pszDeviceTypeName_ls00[m_pTransformationCmd->trDataFile_ls00.fsDeviceType_ls00],
                                                                                _T(" "),
                                                                                sztransformationstream);
                        transformation.Assign(REINTERPRET_CAST(SAPDB_UTF8*, sztransformation));
                    }
                    else if ( dtypeTape_ls00 == m_pTransformationCmd->trDataFile_ls00.fsDeviceType_ls00 )
                    {
                        sp77sprintf(sztransformationebid,  PATH_MXSP00, "EBID '%s' ", m_pTransformationCmd->trDataFile_ls00.fsBackupId_ls00.bBackupIdName_ls00 );
                        sp77sprintf(sztransformationstream,  PATH_MXSP00, "%s"        , m_pTransformationCmd->trDataFile_ls00.fsFileName_ls00 );
                        sp77sprintf(sztransformation, MAX_STMT_LENGTH_LS00, CONST_CAST(SAPDB_Char*, m_upd_Transformation_809.CharPtr()),
                                                                            m_pTransformationCmd->trPackageid_ls00.StrPtr(),
                                                                            m_packagepart[m_pTransformationCmd->trPartSpec_ls00.psPartType_ls00].StrPtr(),
                                                                            pszDeviceTypeName_ls00[m_pTransformationCmd->trDataFile_ls00.fsDeviceType_ls00],
                                                                            sztransformationebid,
                                                                            sztransformationstream );
                        transformation.Assign(REINTERPRET_CAST(SAPDB_UTF8*, sztransformation));
                    }
                    setTransformationStmt(transformation);
                    initTransformationCmd();
                    rc = UpdatePackageDataRecord( pMetaDataDef, pDBInfo, ErrText);
                    freeTransformationCmd(srFctColUpdate_els00);
                }
                //*
                //*  protocol file
                //*
                updateTaskStatistic();
            } 
            else 
            {
                rc = pMetaDataDef->getConnection()->dbiOdbcConnection.GetErr().GetErrId();
                if (errOK_els00 != rc)
                {
                    ErrorHndl *pE = pMetaDataDef->getError();
                    rc = pE->getReturnCode();
                    StudioOAL_String ErrorText = pE->getErrorText();

                    pMsgInstance->ls98MsgToString(layTableExtract_els98, ErrText, STATIC_CAST(ErrorNumbers, rc), ErrorText.StrPtr());
                }
            }
        }//  rc = ls15CheckConnection(pDBInfo, ErrText );
    }//  if ( getTransformationCmd()->trInternal_ls00 == false && NULL == getenv ("maxdbloaderserver"))

    return STATIC_CAST(tsp00_Int4, rc);
}
/*===========================================================================*
 *     TransformationDef::ExportPackage
 *===========================================================================*/
tsp00_Int4 TransformationDef::ExportPackage(MetaDataDef *&pMetaDataDef, tsp00_Addr ErrText)
{
    ROUTINE_DBG_MEO00 ("TransformationDef::ExportPackage");
    LoaderMessages *pMsgInstance = LoaderMessages::Instance();
    SAPDB_Long      rc           = errOK_els00;

    if ( getTransformationCmd()->trInternal_ls00 == false && NULL == getenv ("maxdbloaderserver"))
    {
        tls00_DBInfo *pDBInfo = pMetaDataDef->getConnection();

        m_currentStream       = stPackage_els00;

        Tools_DynamicUTF8String  transformation;
        _TCHAR sztransformation[MAX_STMT_LENGTH_LS00];
        sztransformation[0] = 0;
        _TCHAR sztransformationfile[PATH_MXSP00];
        sztransformationfile[0] = 0;

        if ( ptTable_els00  <= m_pTransformationCmd->trPartSpec_ls00.psPartType_ls00 )
        {
            //*
            //*  data mapping file
            //*
            if ( NULL != m_pTransformationCmd->trDataFile_ls00.fsFileName_ls00 &&
                tsExport_ls00 == m_pTransformationCmd->trTransSpec_ls00       && 
                ptTable_els00  < m_pTransformationCmd->trPartSpec_ls00.psPartType_ls00)
            {
                tls00_FileSpec* _pDataFileSpec = &m_pTransformationCmd->trDataFile_ls00;
                if ( (dtypeFile_ls00 == _pDataFileSpec->fsDeviceType_ls00) ||
                     (dtypeTape_ls00 == _pDataFileSpec->fsDeviceType_ls00) )
                {
                    if ( dtypeFile_ls00 == _pDataFileSpec->fsDeviceType_ls00 )
                    {
                        sp77sprintf(sztransformationfile, PATH_MXSP00, "%s%0*d", _pDataFileSpec->fsFileName_ls00, FILE_EXTENSION_SIZE_LS27, 0);
                    }
                    else
                    {
                        updateDataCmd();
                        sp77sprintf(sztransformationfile, PATH_MXSP00, "%s", _pDataFileSpec->fsFileName_ls00 );
                    }
                    sp77sprintf(sztransformation, MAX_STMT_LENGTH_LS00,
                                                  CONST_CAST(SAPDB_Char*, Statement(sel_Transformation_715).CharPtr()),
                                                  m_pTransformationCmd->trPackageid_ls00.StrPtr(),
                                                  pszDeviceTypeName_ls00[_pDataFileSpec->fsDeviceType_ls00],
                                                  sztransformationfile);
                    transformation.Assign(REINTERPRET_CAST(SAPDB_UTF8*, sztransformation));
                    setTransformationStmt(transformation);
                    initTransformationCmd();
                    rc = ExportPackageDataRecord( pMetaDataDef, pDBInfo, ErrText);
                    freeTransformationCmd(srFctColExtract_els00);
                }
            }
            //*
            //*  package file
            //*
            if ( NULL   != m_pTransformationCmd->trPackageFile_ls00.fsFileName_ls00 && 
                 (false == m_pTransformationCmd->trPackageFile_ls00.fsFormat_ls00.ffoDtl_ls00  || NULL != getenv ("maxdbloaderpackagepath") ) && errOK_els00 == rc )
            {
                sztransformation[0]     = 0;
                sztransformationfile[0] = 0;
                transformation.Erase();
                if (true == m_pTransformationCmd->trPackageFile_ls00.fsFormat_ls00.ffoDtl_ls00 && NULL != getenv ("maxdbloaderpackageexecution") )
                {
                    StudioOAL_String packagefilename(getenv ("maxdbloaderpackageexecution"));
                    packagefilename += ".PACKAGE";
                    delete [] m_pTransformationCmd->trPackageFile_ls00.fsFileName_ls00;
                    m_pTransformationCmd->trPackageFile_ls00.fsFileName_ls00 = new char [packagefilename.BasisSize()+1];
                    sp77sprintf(m_pTransformationCmd->trPackageFile_ls00.fsFileName_ls00, PATH_MXSP00, "%s", CONST_CAST(SAPDB_Char*, packagefilename.CharPtr()) );
                }
                sp77sprintf(sztransformation, MAX_STMT_LENGTH_LS00, CONST_CAST(SAPDB_Char*, Statement(sel_Transformation_714).CharPtr()),
                                                                    m_pTransformationCmd->trPackageid_ls00.StrPtr(),
                                                                    m_pTransformationCmd->trPackageFile_ls00.fsFileName_ls00,
                                                                    pszCodeTypeName_ls00[m_pTransformationCmd->trPackageFile_ls00.fsFormat_ls00.ffoCodeType_ls00],
                                                                    pszAppend_ls00[m_pTransformationCmd->trPackageFile_ls00.fsAppend_ls00]);
                transformation.Assign(REINTERPRET_CAST(SAPDB_UTF8*, sztransformation));
                setTransformationStmt(transformation);
                initTransformationCmd();
                rc = ExportPackageDataRecord( pMetaDataDef, pDBInfo, ErrText);
                freeTransformationCmd(srFctColExtract_els00);
                if ( errOK_els00 == rc )
                {
                    sztransformation[0] = 0;
                    sp77sprintf(sztransformation, MAX_STMT_LENGTH_LS00, CONST_CAST(SAPDB_Char*, Statement(del_Transformation_600).CharPtr()),
                                                                        m_pTransformationCmd->trPackageid_ls00.StrPtr());
                    executeSQL(sztransformation);
                }
            }
            //*
            //*  catalog file
            //*
        }
        if ( ptTable_els00 ==  m_pTransformationCmd->trPartSpec_ls00.psPartType_ls00 )
        {
            if ( (errOK_els00 == rc) && (SAPDB_FALSE == m_pTransformationCmd->trInternal_ls00) )  
            {
                sztransformation[0] = 0;
                sp77sprintf(sztransformation, MAX_STMT_LENGTH_LS00, CONST_CAST(SAPDB_Char*, Statement(del_Transformation_600).CharPtr()),
                                                                    m_pTransformationCmd->trPackageid_ls00.StrPtr());
                executeSQL(sztransformation);
            }
        }
        if (ptTable_els00  <= m_pTransformationCmd->trPartSpec_ls00.psPartType_ls00)
        {
            if ( ( errOK_els00 <= rc || -25362 == rc ) && (SAPDB_FALSE == m_pTransformationCmd->trInternal_ls00) )
            {
                Tools_DynamicUTF8String transformationpackage("Export    PACKAGE x'");
                transformationpackage += m_pTransformationCmd->trPackageid_ls00;
                transformationpackage += "'";
                pMsgInstance->ls98PrintMessage(layTableExtract_els98, msgCommandExecuting_els98,transformationpackage.StrPtr());
            }
        }
    }   // end if ( getTransformationCmd()->trInternal_ls00 == false && NULL == getenv ("maxdbloaderserver"))
    return STATIC_CAST(tsp00_Int4, rc);
}
/*===========================================================================*
 *     TransformationDef::initResultSet
 *===========================================================================*/
SAPDB_Bool TransformationDef::initResultSet( sapdbwa_DBC & odbcConnection )
{
    ROUTINE_DBG_MEO00 ("TransformationDef::initResultSet ()");
    LoaderMessages *pMsgInstance = LoaderMessages::Instance();

    SAPDB_Bool       rcBool   = SAPDB_TRUE;
    StudioOAL_String NativeErrorAsString;

    if ( odbcConnection.HDBC() && odbcConnection.HENV() )
    {
        MetaDataDef*      pMetaData = getMetaData();
        StudioOAL_String  schemaname;
        StudioOAL_String  tabname;
        StudioOAL_String  SaveCatalogStmt;

        if (ptAll_els00         == m_pTransformationCmd->trPartSpec_ls00.psPartType_ls00)
            SaveCatalogStmt = Statement(ins_Transformation_502);
        else if (ptUser_els00   == m_pTransformationCmd->trPartSpec_ls00.psPartType_ls00)
            SaveCatalogStmt = Statement(ins_Transformation_503);
        else if (ptSchema_els00   == m_pTransformationCmd->trPartSpec_ls00.psPartType_ls00)
        {
            schemaname.Assign(REINTERPRET_CAST(SAPDB_UTF8*, m_pTransformationCmd->trPartSpec_ls00.psTabSchema_ls00.asCharp()));
            sp77sprintf(m_pTransformationStmt, 1024, Statement(ins_Transformation_506).CharPtr(), 
                                                     m_pTransformationCmd->trPartSpec_ls00.psTabSchema_ls00.asCharp());
            SaveCatalogStmt = StudioOAL_String(m_pTransformationStmt);
        }
        else if (ptTable_els00  == m_pTransformationCmd->trPartSpec_ls00.psPartType_ls00)
        {
            tabname.Assign(REINTERPRET_CAST(SAPDB_UTF8*, m_pTransformationCmd->trPartSpec_ls00.psTabName_ls00.asCharp()));

            if( tabname.Compare("TRANSFORMATIONMODEL") == 0 || tabname.Compare("") == 0 )
                SaveCatalogStmt = "";
            else
            {
                if (0 != m_pTransformationCmd->trPartSpec_ls00.psTabSchema_ls00.length())
                {
                    SaveCatalogStmt = "SAVE CATALOG OF \"";
                    SaveCatalogStmt += REINTERPRET_CAST(SAPDB_UTF8*, m_pTransformationCmd->trPartSpec_ls00.psTabSchema_ls00.asCharp());
                    SaveCatalogStmt += "\".\"";
                }
                else
                {
                    SaveCatalogStmt = "SAVE CATALOG OF \"";
                    SaveCatalogStmt += REINTERPRET_CAST(SAPDB_UTF8*, pMetaData->getConnection()->dbiSession_ls00.siSchemaName_ls00);
                    SaveCatalogStmt += "\".\"";
                }
                SaveCatalogStmt += tabname;
                SaveCatalogStmt += "\" INTO ";
                SaveCatalogStmt += Statement(exist_Transformation_106);
            }
        }
        if ( m_pTransformationCmd->trRestart_ls00.rstRestart_ls00 == false && 
             m_pTransformationCmd->trInternal_ls00  == false                 )
        {
            StudioOAL_String  record[10];
            StudioOAL_String  dbaUser;
            dbaUser.Assign(REINTERPRET_CAST(SAPDB_UTF8*, pMetaData->getConnection()->dbiSYSDBA_Name_ls00.asCharp()));

            if ( pMetaData->getConnection()->dbiSession_ls00.siInstallOn_ls00 == false )
            {
                rcBool = ImportTransformationTables();

                if(  SaveCatalogStmt.Compare("") != 0 )
                     rcBool = ImportPackage(SaveCatalogStmt);
                if ( SAPDB_FALSE == rcBool)
                {
                    ErrorHndl *pErrorObj = pMetaData->getError();
                    pErrorObj->isError(SAPDB_TRUE);
                    pErrorObj->setReturnCode(errODBCErrors_els98);
                    pErrorObj->setNativeError(getNativeError());
                    pErrorObj->setErrorText(getErrorText());
                }
                else
                {
                    m_StmtString = Statement(sel_Transformation_701);
                    rcBool = executeSQL(m_StmtString);
                    if (SAPDB_TRUE == rcBool)
                    {
                        readResult( record );
                        m_nTaskID = AtoI( record[0] );
                    }
                    else
                    {
                        NativeErrorAsString = getNativeErrorAsString();
                        pMsgInstance->ls98PrintMessage(layCatalogExtract_els98, errODBCErrors_els98, NativeErrorAsString.StrPtr());
                    }
                }
            }
    }
#ifdef ODBC_PREPARED
        /*-------------------------------------------------------------------------*
         * Prepare the WHERE clause for the SQL statements.                        *
         *-------------------------------------------------------------------------*/
        StudioOAL_String whereClause(" WHERE  owner = ? AND tablename = ? ");
        StudioOAL_String append(" AND TABLETYPE IN('TABLE') ");
        // SQLAllocStmt     SQLPrepare
        // SQLNumParams     SQLDescribeParam SQLBindParameter
        // SQLNumResultCols SQLDescribeCol   SQLBindCol
        StudioOAL_String update("UPDATE SYSDBA.TRANSFORMATIONMODEL SET CATALOGSTREAMFORMAT = 'DDL' ");
        update += whereClause;
        update += " AND TABLETYPE = ?";

        rcBool = m_pupdateCat->init(update);

        if (SAPDB_TRUE == rcBool)
        {
            update   = " UPDATE SYSDBA.TRANSFORMATIONMODEL SET DATASTREAMFORMAT = 'PAGES' ";
            update  += whereClause;
            update  += append;
            rcBool = m_pUpdatePages->init(update);
        }
        if (SAPDB_TRUE == rcBool)
        {
            update  = " UPDATE SYSDBA.TRANSFORMATIONMODEL SET DATASTREAMFORMAT = 'RECORDS' ";
            update += whereClause;
            update += append;
            rcBool = m_pUpdateRecords->init(update);
        }
        if (SAPDB_TRUE == rcBool)
        {
            update  = " UPDATE SYSDBA.TRANSFORMATIONMODEL SET RESTART = FALSE ";
            update += whereClause;
            update += append;
            rcBool = m_pUpdateRestart->init(update);
        }
        if (SAPDB_TRUE == rcBool)
        {
            update   = " UPDATE SYSDBA.TRANSFORMATIONMODEL SET STARTTIME = TIMESTAMP ";
            update  += whereClause;
            update  += append;
            rcBool = m_pUpdateBegin->init(update);
        }
        if (SAPDB_TRUE == rcBool)
        {
            update   = " UPDATE SYSDBA.TRANSFORMATIONMODEL SET "
                       " ROWCOUNT                = ?, "
                       " DATASTREAMHEADERBLOCK   = ?, "
                       " DATASTREAMHEADERPOS     = ?  ";
            update  += whereClause;
            update  += append;
            rcBool = m_pUpdateHeader->init(update);
        }
        if (SAPDB_TRUE == rcBool)
        {
            update   = " UPDATE SYSDBA.TRANSFORMATIONMODEL SET "
                       " ROWCOUNT             = ?, "
                       " DATASTREAMDATABLOCK  = ?, "
                       " DATASTREAMDATAPOS    = ?  ";
            update  += whereClause;
            update  += append;
            rcBool = m_pUpdateData->init(update);
        }
        if (SAPDB_TRUE == rcBool)
        {
            update   = " UPDATE SYSDBA.TRANSFORMATIONMODEL SET ENDTIME = TIMESTAMP ";
            update  += whereClause;
            update  += append;
            rcBool = m_pUpdateEnd->init(update);
        }
        if (SAPDB_FALSE == rcBool)
        {
            pMetaData->getError()->setErrorText(getErrorText());
            pMetaData->getError()->isError(isError());
            NativeErrorAsString = getNativeErrorAsString();
            pMsgInstance->ls98PrintMessage(layCatalogExtract_els98, errODBCErrors_els98, NativeErrorAsString.StrPtr());
        }
#endif 
  }
  return rcBool;
}
/*===========================================================================*
 *    TransformationDef::setSQLMode
 *===========================================================================*/
void TransformationDef::setSQLMode( StudioOAL_String&  record )
{
    //typedef enum tpr00_SQLModeEnum {
    //            cpr_kind_empty         =   0,
    //            cpr_kind_internal      =   1,
    //            cpr_kind_db2           =   2,
    //            cpr_kind_ansi          =   3,
    //            cpr_kind_oracle        =   4,
    //            cpr_kind_sapr3         =   5
    //          } tpr00_SQLModeEnum;
    StudioOAL_String internal = "INTERNAL";
    StudioOAL_String db2      = "DB2";
    StudioOAL_String ansi     = "ANSI";
    StudioOAL_String oracle   = "ORACLE";
    StudioOAL_String sapr3    = "SAPR3";

    if ( internal.Compare(record.SubStr(0,8) ) == 0 )
    {
         setSqlMode( cpr_kind_internal );
         m_sqlmode = internal;
    }
    if ( db2     .Compare(record.SubStr(0,3) ) == 0 )
    {
         setSqlMode( cpr_kind_db2 );
         m_sqlmode = db2;
    }
    if ( ansi    .Compare(record.SubStr(0,4) ) == 0 )
    {
         setSqlMode( cpr_kind_ansi );
         m_sqlmode = ansi ;
    }
    if ( oracle  .Compare(record.SubStr(0,6) ) == 0 )
    {
         setSqlMode( cpr_kind_oracle );
         m_sqlmode = oracle;
    }
    if ( sapr3   .Compare(record.SubStr(0,5) ) == 0 )
    {
         setSqlMode( cpr_kind_sapr3 );
         m_sqlmode = sapr3;
    }
}

/*===========================================================================*
 *    TransformationDef::switchSchemaMap
 *===========================================================================*/
tsp00_Int4 TransformationDef::switchSchemaMap(StudioOAL_String currentSchemaMap, tsp00_Addr ErrText)
{
      ROUTINE_DBG_MEO00 ("TransformationDef::switchSchemaMap");
      LoaderMessages*  pMsgInstance  = LoaderMessages::Instance();
      SAPDB_Int4  rc     = errOK_els00;
      SAPDB_Bool  rcBool = SAPDB_TRUE;
    
      MetaDataDef   *pMetaDataDef     = getMetaData();
      RelationalDef *pRelationalDef   = pMetaDataDef->getRelationalModel();
      tls00_DBInfo  *pDBInfo          = pMetaDataDef->getConnection ();
      StudioOAL_String  Packageid     = "";
      StudioOAL_String  SourceSchemas = "";
      StudioOAL_String  TargetSchemas = "";
      
      tsp100_VersionResult vres = ls15CheckKernelVersion(pDBInfo, 7, 6, 0, 26);

      if ((IsEqual_esp100 == vres || IsNewer_esp100 == vres) )
      {
           tls00_TransformationCmd * pTransformationCmd  = getTransformationCmd();
           m_pTransformationStmt[0] = 0;
           StudioOAL_String  record[4];

           if (0 == SAPDB_strcasecmp( pTransformationCmd->trSchemaMap_ls00.msSchemaMapName_ls00, "ID")) {
               m_pTransformationStmt[0] = 0;
               sp77sprintf(m_pTransformationStmt, 1024, CONST_CAST(SAPDB_Char*, Statement(trans_Transformation_318).CharPtr()) );
               if (!executeSQL(m_pTransformationStmt)) {
                   pMsgInstance->ls98Msg(layCatalogExtract_els98, ErrText, errODBCErrors_els98, CONST_CAST(SAPDB_Char*, pMetaDataDef->getError()->m_ErrorText.CharPtr()));
                   rc = pMetaDataDef->getError()->getReturnCode();
               } else {
                 pDBInfo->dbiSession_ls00.siSchemaMapName_ls00[0] = 0;
               }
           }
           else {
              if (pTransformationCmd->trSchemaMap_ls00.msCount_ls00 == 0 && 
                  pTransformationCmd->trSchemaMap_ls00.msSchemaMapName_ls00[0] != 0 ) {
                  sp77sprintf(m_pTransformationStmt, 1024, CONST_CAST(SAPDB_Char*, Statement(sel_Transformation_723).CharPtr()), _T("") ,
                                pTransformationCmd->trSchemaMap_ls00.msSchemaMapName_ls00.asCharp());
                  if ( pRelationalDef->executeSQL(m_pTransformationStmt)) {
                       pRelationalDef->readResult(record);
                       Packageid = record[0];
                  }
              }
              else {
                  Packageid  = " PACKAGEGUID = x'";
                  Packageid += pTransformationCmd->trPackageid_ls00;
                  Packageid += "' AND ";
                  sp77sprintf(m_pTransformationStmt, 1024, CONST_CAST(SAPDB_Char*, Statement(sel_Transformation_723).CharPtr()), Packageid.StrPtr(),
                                pTransformationCmd->trSchemaMap_ls00.msSchemaMapName_ls00.asCharp());
              }
              if ( pRelationalDef->executeSQL(m_pTransformationStmt)) {
                   while ( pRelationalDef->readResult(record) )
                   {
                       if (!existsSchemaPrivileges( pDBInfo->dbiSession_ls00.siUserName_ls00.asCharp(),record[2],ErrText)){
                           SourceSchemas += " ";
                           SourceSchemas += record[2];
                           rc = errSchemaMapPrivilegesNotExists_els98;
                       }
                       if (!existsSchemaPrivileges( pDBInfo->dbiSession_ls00.siUserName_ls00.asCharp(),record[3],ErrText )){
                           TargetSchemas += " ";
                           TargetSchemas += record[3];
                           rc = errSchemaMapPrivilegesNotExists_els98;
                       }
                   }
              }
              if ( rc == errOK_els00 ) {
                   m_pTransformationStmt[0] = 0;
                   if (pTransformationCmd->trSchemaMap_ls00.msCount_ls00 == 0 && 
                       pTransformationCmd->trSchemaMap_ls00.msSchemaMapName_ls00[0] != 0 )
                       sp77sprintf(m_pTransformationStmt, 1024, CONST_CAST(SAPDB_Char*, Statement(trans_Transformation_317).CharPtr()), Packageid.StrPtr() );
                   else
                       sp77sprintf(m_pTransformationStmt, 1024, CONST_CAST(SAPDB_Char*, Statement(trans_Transformation_317).CharPtr()), pTransformationCmd->trPackageid_ls00.StrPtr() );
                   if (!executeSQL(m_pTransformationStmt)) {
                       pMsgInstance->ls98Msg(layCatalogExtract_els98, ErrText, errODBCErrors_els98, CONST_CAST(SAPDB_Char*, pMetaDataDef->getError()->m_ErrorText.CharPtr()));
                       rc = pMetaDataDef->getError()->getReturnCode();
                   } 
                   else if ( pTransformationCmd->trSchemaMap_ls00.msSchemaMapName_ls00[0] != 0) {
                        SAPDB_strcpy(pDBInfo->dbiSession_ls00.siSchemaMapName_ls00,pTransformationCmd->trSchemaMap_ls00.msSchemaMapName_ls00);
                   }
              }
              else if ( rc == errSchemaMapPrivilegesNotExists_els98 ) {
                  SourceSchemas += TargetSchemas;
                  pMsgInstance->ls98Msg(layCatalogExtract_els98, ErrText, errSchemaMapPrivilegesNotExists_els98, SourceSchemas.StrPtr(), 
                                          pTransformationCmd->trSchemaMap_ls00.msSchemaMapName_ls00.asCharp());
              }
           }
      }
      return rc;
}
/*===========================================================================*
 *    TransformationDef::switchUserSchema
 *===========================================================================*/
tsp00_Int4 TransformationDef::switchUserSchema(StudioOAL_String currentUser,
                                               StudioOAL_String currentSchema,
                                               bool silent, tsp00_Addr ErrText)
{
    ROUTINE_DBG_MEO00 ("TransformationDef::switchUserSchema");

    tsp00_Int4 rc = errOK_els00;
    SAPDB_Bool rcBool = SAPDB_TRUE;
    tls00_DBInfo      *pDBInfo        = getMetaData()->getConnection();
    RelationalDef     *pRelationalDef = getMetaData()->getRelationalModel();
    StudioOAL_String  statement;


    if ( tsImport_ls00 ==  m_pTransformationCmd->trTransSpec_ls00  )
    {
        if ( m_currentStream == stCatalog_els00 )
        {
            if ( (false == currentUser.Empty()) && (m_owner.Compare(currentUser) != 0 ) )
            {
                statement  = "CURRENT USER ";
                statement += m_owner;
                rcBool = executeSQL(statement, true);
                if ( rcBool == SAPDB_TRUE )
                     rc = errOK_els00;
            }
            if ( (errOK_els00 == rc) && (isSchema() == true ) &&
                 (false == currentSchema.Empty()) && (m_schemaname.Compare(currentSchema) != 0) )
            {
                statement  = "SET CURRENT_SCHEMA=";
                statement += m_schemaname;
                rcBool = pRelationalDef->executeSQL(statement, true);
                if ( rcBool == SAPDB_TRUE )
                     rc = errOK_els00;
            }
        }// if ( m_currentStream == stCatalog_els00 )
        if ( m_currentStream == stData_els00 )
        {
              if ( m_owner.Compare(currentUser) != 0 || m_schemaname.Compare(currentSchema) != 0  )
                   rc = ls15SwitchUserSchema(pDBInfo, m_owner, m_schemaname, ErrText);
        }// if ( m_currentStream == stData_els00 )
        if ( m_currentStream == stPackage_els00 )
        {
            if ( (false == currentUser.Empty()) && (m_owner.Compare(currentUser) != 0 ) )
            {
                statement  = "CURRENT USER ";
                statement += m_owner;
                rcBool = executeSQL(statement, true);
                if ( rcBool == SAPDB_TRUE )
                     rc = errOK_els00;
            }
            if ( (errOK_els00 == rc) && (isSchema() == true ) &&
                 (false == currentSchema.Empty()) && (m_schemaname.Compare(currentSchema) != 0) )
            {
                statement = "SET CURRENT_SCHEMA=";
                statement += m_schemaname;
                rcBool = pRelationalDef->executeSQL(statement, true);
                if ( rcBool == SAPDB_TRUE )
                     rc = errOK_els00;
            }
        }// if ( m_currentStream == stPackage_els00 )
    }//if ( tsImport_ls00 ==  m_pTransformationCmd->trTransSpec_ls00  )

    if ( tsExport_ls00 ==  m_pTransformationCmd->trTransSpec_ls00  )
    {
        if ( m_currentStream == stCatalog_els00 )
        {
            if ( m_owner.Compare(currentUser) != 0 )
            {
                /*-------------------------------------------------------------------------*
                * Prepare the security context for the SQL statements.
                *-------------------------------------------------------------------------*/
                statement   = "CURRENT USER \"";
                statement  += m_owner;
                statement  += "\"";
                /*-------------------------------------------------------------------------------*
                * Read/Write the definition of the specified object from/to resources           *
                *-------------------------------------------------------------------------------*/
                if( pRelationalDef->executeSQL( statement ) )
                {
                    if( m_pTransformationCmd->trCatalogFile_ls00.fsFormat_ls00.ffoDdl_ls00 == true )
                    {
                        statement = SEPARATOR;

                        if ( getSqlMode() != cpr_kind_internal )
                             statement += pszSQLModeString_ls31[cpr_kind_internal];
                        
                        statement += "USE USER \"";
                        statement += m_owner;
                        statement += "\" ";
                        statement += "initial";
					    statement += " ";
                        if (silent == false )
                            writeLine( statement );
                    }
                }
             }//if ( m_owner.Compare(currentUser) != 0 )
             if ( (false == currentSchema.Empty()) && (m_schemaname.Compare(currentSchema) != 0 || m_nSchemaCount == 0 ))
		     {
                 if( m_pTransformationCmd->trCatalogFile_ls00.fsFormat_ls00.ffoXml_ls00 == true )
                 {
                     if (silent == false )
                     {
                        /*------------------------------------------------------------------------*
                         * Close the previous Schema                                              *
                         *------------------------------------------------------------------------*/
                         if ( m_nSchemaCount != 0 )
                         {
                              statement = "</uml:NamespaceOwnedElement>\n"
                                          "</sdbm:DeployedSchema>\n"
                                          "</uml:NamespaceOwnedElement>\n";
                              getMetaData()->writeLine(statement);
                         }
                         m_nSchemaCount++;
                        _TCHAR szStatement [1024];
                        szStatement[0]=0;
                        StudioOAL_String record[11];
                        /*------------------------------------------------------------------------*
                        * Prepare the WHERE clause for the SQL statements.                        *
                        *-------------------------------------------------------------------------*/
                        sp77sprintf(szStatement, 1024, CONST_CAST(SAPDB_Char*, Statement(sel_Relational_1).CharPtr()),
                                                       m_owner.StrPtr(), m_schemaname.StrPtr(), m_owner.StrPtr(), m_schemaname.StrPtr() );
                        /*------------------------------------------------------------------------*
                        * Read/Write the definition of the specified object from/to resources     *
                        *-------------------------------------------------------------------------*/
                        if ( pRelationalDef->getUserDef()->executeSQL( szStatement ) )
                        {
                            if ( pRelationalDef->getUserDef()->readResult (record) )
                                 pRelationalDef->getUserDef()->writeResult(record);
                        }
                     }
                 }
                 if( isSchema() == true && (false == currentSchema.Empty()) &&
                     m_pTransformationCmd->trCatalogFile_ls00.fsFormat_ls00.ffoDdl_ls00 == true )
                 {
                     statement       = SEPARATOR;
			         statement      += "USE SCHEMA \"";
			         statement      += m_schemaname;
                     statement      += "\"";
                     if (silent == false )
                         writeLine( statement );
                     m_nSchemaCount++;
                 }
		     }
        }//if ( m_currentStream == stCatalog_els00 )
        if ( m_currentStream == stData_els00 )
        {
            if (m_pTransformationCmd->trPartSpec_ls00.psPartType_ls00 > ptTable_els00 ) 
            {
                if ( m_owner.Compare(currentUser) != 0 || m_schemaname.Compare(currentSchema) != 0  )
                     rc = ls15SwitchUserSchema(pDBInfo, m_owner, m_schemaname, ErrText);
            }
        }
        if ( m_currentStream == stPackage_els00 )
        {
            if ( (false == currentUser.Empty()) && (m_owner.Compare(currentUser) != 0 ) )
            {
                statement  = "CURRENT USER \"";
                statement += m_owner;
                statement += "\"";
                rcBool = executeSQL(statement, true);
                if ( rcBool == SAPDB_TRUE )
                     rc = errOK_els00;
            }
            if ( (errOK_els00 == rc) && (isSchema() == true ) &&
                 (false == currentSchema.Empty()) && (m_schemaname.Compare(currentSchema) != 0) )
            {
                statement = "SET CURRENT_SCHEMA=\"";
                statement += m_schemaname;
                statement += "\"";
                rcBool = pRelationalDef->executeSQL(statement, true);
                if ( rcBool == SAPDB_TRUE )
                     rc = errOK_els00;
            }
        }// if ( m_currentStream == stPackage_els00 )
    }//if ( tsExport_ls00 ==  m_pTransformationCmd->trTransSpec_ls00  )
    return rc;
}// TransformationDef::switchUserSchema
/*===========================================================================*
 *     TransformationDef::switchUser
 *===========================================================================*/
SAPDB_Int4 TransformationDef::switchUser( StudioOAL_String previousUser, tsp00_Addr ErrText )
{
     ROUTINE_DBG_MEO00 ("TransformationDef::switchUser");

     tsp00_Int4 rc         = errOK_els00;
     tls00_DBInfo *pDBInfo = getMetaData()->getConnection();

     if (m_owner.Compare(previousUser) != 0 )
         rc = ls15SwitchUser(pDBInfo, m_owner, ErrText);
     return rc;
}
/*===========================================================================*
 *     TransformationDef::switchUser
 *===========================================================================*/
SAPDB_Int4 TransformationDef::switchUser( StudioOAL_String previousUser, 
                                          StudioOAL_String currentUser , tsp00_Addr ErrText )
{
     ROUTINE_DBG_MEO00 ("TransformationDef::switchUser");

     tsp00_Int4 rc = errOK_els00;
     tls00_DBInfo  *pDBInfo  = getMetaData()->getConnection();

     if (currentUser.Compare(previousUser) != 0 )
         rc = ls15SwitchUser(pDBInfo, currentUser, ErrText);
     return rc;
}
/*===========================================================================*
 *     TransformationDef::readResult
 *===========================================================================*/
SAPDB_Bool TransformationDef::readResult( StudioOAL_String * record )
{
    ROUTINE_DBG_MEO00 ("TransformationDef::readResult");
    return MetaDataItem::readResult(record) ;
}
/*===========================================================================*
 *     TransformationDef::initTransformationStep
 *===========================================================================*/
SAPDB_Bool TransformationDef::initTransformationStep()
{
    ROUTINE_DBG_MEO00 ("TransformationDef::initTransformationStep");
    SAPDB_Bool rc = false;

    if ( m_currentStream == stCatalog_els00 )
    {
         RelationalDef  *pRelationalDef = getMetaData()->getRelationalModel();
         // sel_Transformation_702,sel_Transformation_703,sel_Transformation_704
         if( readResult(m_package) )
         {
            pRelationalDef->m_stepid           = m_package[0];
            pRelationalDef->m_owner            = m_package[1];
            pRelationalDef->m_schemaname       = m_package[2];
            pRelationalDef->m_schemaname_value = m_package[2];
            pRelationalDef->m_tablename        = m_package[3];
            pRelationalDef->m_tablename_value  = m_package[3];
            pRelationalDef->m_tabletype        = m_package[4];
            pRelationalDef->m_cmd              = m_package[5];
            pRelationalDef->m_taskid           = m_package[6];
            escapeQuoteChar(pRelationalDef->m_schemaname_value, "'");
            escapeQuoteChar(pRelationalDef->m_tablename_value , "'");
            m_owner                            = m_package[1];
            m_schemaname                       = m_package[2];
            m_schemaname_value                 = m_package[2];
            m_tablename                        = m_package[3];
            m_tablename_value                  = m_package[3];
            m_tabletype                        = m_package[4];
            escapeQuoteChar(m_schemaname_value, "'");
            escapeQuoteChar(m_tablename_value , "'");
            rc = true;
         }
    }
    if ( m_currentStream == stData_els00 )
    {
        // sel_Transformation_708
        if( readResult(m_package) )
        {
            m_transformation                = m_package[0];
            m_partspec                      = "TABLE";
            m_owner                         = m_package[1];
            m_schemaname                    = m_package[2];
            m_schemaname_value              = m_package[2];
            m_tablename                     = m_package[3];
            m_tablename_value               = m_package[3];
            m_datastreamtype                = m_package[4];
            m_datastreamname                = m_package[5];
            m_datastreamformat              = m_package[6];
            m_codetype                      = m_package[7];
            m_datastreamebid                = m_package[8];
            m_headerpos_block               = m_package[9];
            m_headerpos_byte                = m_package[10];
            m_datapos_block                 = m_package[11];
            m_datapos_byte                  = m_package[12];
            m_trailerpos_block              = m_package[13];
            m_trailerpos_byte               = m_package[14];
            m_headerpos.lnu2GB_Blocks_ls00  = _ttoi(m_package[9].CharPtr());
            m_headerpos.lnu2GB_Blocks_ls00  = _ttoi(m_package[9].CharPtr());
            m_headerpos.lnuRemainder_ls00   = _ttoi(m_package[10].CharPtr());
            m_datapos.lnu2GB_Blocks_ls00    = _ttoi(m_package[11].CharPtr());
            m_datapos.lnuRemainder_ls00     = _ttoi(m_package[12].CharPtr());
            m_trailerpos.lnu2GB_Blocks_ls00 = _ttoi(m_package[13].CharPtr());
            m_trailerpos.lnuRemainder_ls00  = _ttoi(m_package[14].CharPtr());
            m_exportcount                   = _ttoi(m_package[15].CharPtr());
            m_importcount                   = _ttoi(m_package[16].CharPtr());
            m_taskid                        = "1";
            m_package[17].Compare("TRUE")   == 0 ? m_exclude  = SAPDB_TRUE  : m_exclude = SAPDB_FALSE ;
            m_stepid                        = m_package[18];
            escapeQuoteChar(m_schemaname_value, "'");
            escapeQuoteChar(m_tablename_value , "'");
            genTransformationStmt();
            rc = true;
        }
    }
    return rc;
}
/*===========================================================================*
 *     TransformationDef::genTransformationStmt
 *===========================================================================*/
void TransformationDef::genTransformationStmt()
{
    if ( m_datastreamformat.Compare("PAGES") == 0 )
    {
        m_statement  = m_transformation;
        m_statement += " TABLE ";
        m_statement += "\"";
        m_statement += m_schemaname;
        m_statement += "\"";
        m_statement += ".";
        m_statement += "\"";
        m_statement += m_tablename;
        m_statement += "\" DATA ";

        if( 0 == m_transformation.Compare("EXPORT") )
        {
            m_statement += "OUTSTREAM ";
            m_statement += m_datastreamtype;
            m_statement += " '";
            m_statement += m_datastreamname;
            m_statement += "' ";
            m_statement += m_datastreamformat;
            m_statement += " ";
            m_statement += m_codetype;
        }
        else if( 0 == m_transformation.Compare("IMPORT") )
        {
            m_statement += "INSTREAM ";
            m_statement += m_datastreamtype;
            if ( m_datastreamtype.Compare("TAPE") == 0 )
            {
                 m_statement += " EBID ";
                 m_statement += "'";
                 m_statement += m_datastreamebid;
                 m_statement += "'";
            }
            m_statement += " '";
            m_statement += m_datastreamname;
            m_statement += "' ";
            m_statement += m_datastreamformat;
            m_statement += " ";
            m_statement += m_codetype;
            m_statement += " START ";
            m_statement += m_headerpos_block;
            m_statement += ":";
            m_statement += m_headerpos_byte;
            m_statement += " ";
            m_statement += m_datapos_block;
            m_statement += ":";
            m_statement += m_datapos_byte;;
        }
        m_statement += " INTERNAL ";
        if ( true == m_exclude )
             m_statement += " EXCLUDE ";
    }
    else if (0 == m_datastreamformat.Compare("RECORDS"))
    {
        if( m_transformation.Compare("EXPORT" ) == 0 )
        {
            m_statement  = m_transformation;
            m_statement += " TABLE ";
            m_statement += "\"";
            m_statement += m_schemaname;
            m_statement += "\"";
            m_statement += ".";
            m_statement += "\"";
            m_statement += m_tablename;
            m_statement += "\" ";
            m_statement += " DATA OUTSTREAM ";
            m_statement += m_datastreamtype;
            m_statement += " '";
            m_statement += m_datastreamname;
            m_statement += "' ";
            m_statement += m_datastreamformat;
            m_statement += " ";
            m_statement += m_codetype;
            m_statement += " START ";
            if ( m_headerpos_byte.Compare("0") == 0 )
                 m_statement += "1";
            else
                 m_statement += m_headerpos_byte;
            m_statement += " ";
            if ( m_datapos_byte.Compare("0") == 0 )
                 m_statement += "MAXROWS";
             else
                 m_statement += m_datapos_byte;
        }
        if( m_transformation.Compare("IMPORT" ) == 0 )
        {
            m_statement  = m_transformation;
            m_statement += " TABLE ";
            m_statement += "\"";
            m_statement += m_schemaname;
            m_statement += "\"";
            m_statement += ".";
            m_statement += "\"";
            m_statement += m_tablename;
            m_statement += "\" ";
            if ( m_pTransformationCmd->trDplMode_ls00 == dupUnknown_els00 )
                 m_statement += _T(" ");
            if ( m_pTransformationCmd->trDplMode_ls00 == dupIgnore_els00  )
                 m_statement +=  _T(" IGNORE DUPLICATES ");
            if ( m_pTransformationCmd->trDplMode_ls00 == dupReject_els00  )
                 m_statement += _T(" REJECT DUPLICATES ");
            if ( m_pTransformationCmd->trDplMode_ls00 == dupUpdate_els00  )
                 m_statement += _T(" UPDATE DUPLICATES ");
            m_statement += " DATA INSTREAM ";
            m_statement += m_datastreamtype;
            if ( m_datastreamtype.Compare("TAPE") == 0 )
            {
                 m_statement += " EBID ";
                 m_statement += "'";
                 m_statement += m_datastreamebid;
                 m_statement += "'";
            }
            m_statement += " '";
            m_statement += m_datastreamname;
            m_statement += "' ";
            m_statement += m_datastreamformat;
            m_statement += " ";
            m_statement += m_codetype;
            bool start = false ;
            if ( true == start )
            {
                m_statement += " START ";
                m_statement += m_headerpos_block;
                m_statement += ":";
                m_statement += m_headerpos_byte;
                m_statement += " ";
                m_statement += m_datapos_block;
                m_statement += ":";
                if ( m_datapos_byte.Compare("0") == 0 )
                    m_statement += "-2";
                else
                    m_statement += m_datapos_byte;
            }
        }
        m_statement += " INTERNAL ";
        if ( true == m_exclude )
             m_statement += " EXCLUDE ";
    }
}
/*===========================================================================*
 *     TransformationDef::createTransformationStatement
 *===========================================================================*/
SAPDB_Int4  TransformationDef::initTransformationCmd()
{
    ROUTINE_DBG_MEO00 (" TransformationDef::initTransformationCmd()");
    LoaderMessages *pMsgInstance = LoaderMessages::Instance();
    tsp00_Int4 rc = errOK_els00;

    tls00_DBInfo *pDBInfo  = getMetaData()->getConnection();
    // initialize the request string
    m_pLoaderCmd->cmdReqErr_ls00 = 0;
    m_pLoaderCmd->cmdReqPos_ls00 = 0;
    m_pLoaderCmd->cmdReqDat_ls00 = CONST_CAST(SAPDB_Char*, m_statement.CharPtr());

    // initialize the reply string
    m_pLoaderCmd->cmdScnDat_ls00 = NULL;
    memset(&m_pLoaderCmd->cmdScanRC_ls00 ,0,sizeof(tls00_ScanResult));
    memset(&m_pLoaderCmd->cmdSubFunc_ls00,0,sizeof(tls00_SubFuncClass));
    memset( m_pLoaderCmd->cmdRpyDat_ls00, DEF_BYTE_LS00, MAX_REPLY_LENGTH_LS00);
    // initialize logging behaviour
    m_pLoaderCmd->cmdInternal_ls00  = SAPDB_TRUE;
    m_pLoaderCmd->cmdLogging_ls00   = SAPDB_FALSE;

    rc = ls11ParseLoadCommand( pDBInfo, m_pLoaderCmd );
    return rc;
}
/*===========================================================================*
 *     TransformationDef::createTransformationStatement
 *===========================================================================*/
SAPDB_Int4  TransformationDef::initTransformationCmd(tls00_ScanResult Type)
{
    ROUTINE_DBG_MEO00 (" TransformationDef::initTransformationCmd()");
    freeTransformationCmd(Type);
    return initTransformationCmd();
}
/*===========================================================================*
 *    TransformationDef::freeTransformationCmd
 *===========================================================================*/
void TransformationDef::freeTransformationCmd(tls00_ScanResult Type )
{
    if ( m_pLoaderCmd->cmdScnDat_ls00 != NULL ) {
         ls10FreeTransformation( Type, m_pLoaderCmd->cmdScnDat_ls00);
         m_pLoaderCmd->cmdScnDat_ls00 = NULL;
    }
}
/*===========================================================================*
 *    TransformationDef::freeTransformationCmd
 *===========================================================================*/
void TransformationDef::freeTransformationCmd()
{
    if ( m_pTransformationCmd != NULL ) {
         ls10FreeTransformation(m_pTransformationCmd);
    }
}
/*===========================================================================*
 *    TransformationDef::genPreparedStmt
 *===========================================================================*/
void TransformationDef::genPreparedStmt()
{
    ROUTINE_DBG_MEO00 ("TransformationDef::genPreparedStmt");

    tls00_TransformationCmd *pTransformationCmd = getTransformationCmd();
    tls00_MultColSpec        mapping            = pTransformationCmd->trColSpec_ls00;
    SAPDB_UInt4              strimplength       = (mapping.mcsCount_ls00+12)*sizeof(tsp00_KnlIdentifier);
    SAPDB_UInt4              strexplength       = (mapping.mcsCount_ls00+12)*sizeof(tsp00_KnlIdentifier);
    SAPDB_UInt4              strimppos          = 0;
    SAPDB_UInt4              strexppos          = 0;
    SAPDB_Char              *strimp             = new char[strimplength]; strimp[0]=0;
    SAPDB_Char              *strexp             = new char[strexplength]; strexp[0]=0; 
    int i = 1;

    if ( 0 != pTransformationCmd->trPartSpec_ls00.psTabSchema_ls00.length()) {
        strimppos = sp77sprintf(strimp, strimplength, "insert into \"%s\".\"%s\" (\"%s",pTransformationCmd->trPartSpec_ls00.psTabSchema_ls00.asCharp(),pTransformationCmd->trPartSpec_ls00.psTabName_ls00.asCharp(),
                                mapping.mcsColumn_ls00[0]->colName_ls00.asCharp());
    }
    else {
        strimppos = sp77sprintf(strimp, strimplength, "insert into \"%s\" (\"%s", pTransformationCmd->trPartSpec_ls00.psTabName_ls00.asCharp(),
                                mapping.mcsColumn_ls00[0]->colName_ls00.asCharp());
    }
    strexppos = sp77sprintf(strexp, strexplength, "select \"%s ", mapping.mcsColumn_ls00[0]->colName_ls00.asCharp());
    for (i=1; i < mapping.mcsCount_ls00; i++){
        strimppos += sp77sprintf(strimp+strimppos, strimplength-strimppos, "\",\"%s", mapping.mcsColumn_ls00[i]->colName_ls00.asCharp());
        strexppos += sp77sprintf(strexp+strexppos, strexplength-strexppos, "\",\"%s", mapping.mcsColumn_ls00[i]->colName_ls00.asCharp());
    }
    strimppos += sp77sprintf(strimp+strimppos, strimplength-strimppos, "\") values(?");
    for (i=1; i < mapping.mcsCount_ls00; ++i) 
        strimppos += sp77sprintf(strimp+strimppos, strimplength-strimppos, ",?");
    strimppos += sp77sprintf(strimp+strimppos, strimplength-strimppos, ")");
    if ( 0 != pTransformationCmd->trPartSpec_ls00.psTabSchema_ls00.length()) {
        strexppos += sp77sprintf(strexp+strexppos, strexplength, "\" from \"%s\".\"%s\" ",pTransformationCmd->trPartSpec_ls00.psTabSchema_ls00.asCharp(),pTransformationCmd->trPartSpec_ls00.psTabName_ls00.asCharp());
    }
    else {
        strexppos += sp77sprintf(strexp+strexppos, strexplength, "\" from \"%s\" "       ,pTransformationCmd->trPartSpec_ls00.psTabName_ls00.asCharp(),
                                 mapping.mcsColumn_ls00[0]->colName_ls00.asCharp());
    }
    m_preparedInsert.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer, strimp));
    m_preparedSelect.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer, strexp));
    if ( NULL  != strimp ) {
         delete[] strimp;
         strimp = NULL;
    }
    if ( NULL  != strexp ) {
         delete[] strexp;
         strimp = NULL;
    }
}
/*===========================================================================*
 *    TransformationDef::getTransformationCmd
 *===========================================================================*/
tls00_TransformationCmd *  TransformationDef::getTransformationCmd() 
{ 
    tls00_TransformationCmd *pTransformationCmd = NULL;

    if ( NULL  != m_pLoaderCmd->cmdScnDat_ls00 &&
         true  == m_pLoaderCmd->cmdInternal_ls00  )
         pTransformationCmd = REINTERPRET_CAST(tls00_TransformationCmd*, m_pLoaderCmd->cmdScnDat_ls00);
    else
         pTransformationCmd = m_pTransformationCmd;
    return pTransformationCmd;
}
/*===========================================================================*
 *    TransformationDef::writeResult
 *===========================================================================*/
SAPDB_Bool TransformationDef::writeResult( StudioOAL_String record  )
{
    ROUTINE_DBG_MEO00 ("TransformationDef::writeResult ()");
    return MetaDataItem::writeLine ( record);
}

/*===========================================================================*
 *    TransformationDef::ImportSchemaMap
 *===========================================================================*/
SAPDB_Int4 TransformationDef::ImportSchemaMap(tsp00_Addr ErrText)
{
    ROUTINE_DBG_MEO00 ("TransformationDef::ImportSchemaMap");

    SAPDB_Int4  rc     = errOK_els00;
    SAPDB_Bool  rcBOOL = SAPDB_TRUE;
    LoaderMessages *pMsgInstance = LoaderMessages::Instance();
    MetaDataDef    *pMetaDataDef = getMetaData();
    tls00_DBInfo   *pDBInfo      = pMetaDataDef->getConnection ();
    
    tsp100_VersionResult vres = ls15CheckKernelVersion(pDBInfo, 7, 6, 0, 26);

    if ((IsEqual_esp100 == vres || IsNewer_esp100 == vres) )
    {
         tls00_TransformationCmd * pTransformationCmd  = getTransformationCmd();
         StudioOAL_String  SchemaMapName= "";
         StudioOAL_String  SourceSchemas= "";
         StudioOAL_String  TargetSchemas= "";
         for ( int i =   0;   i <  pTransformationCmd->trSchemaMap_ls00.msCount_ls00; i++ ) {
             if (!existsSchema(     REINTERPRET_CAST(SAPDB_UTF8*,pTransformationCmd->trSchemaMap_ls00.msSchemaMap_ls00[i]->smSourceSchema_ls00.asCharp()),ErrText)) {
                  SourceSchemas +=  REINTERPRET_CAST(SAPDB_UTF8*,pTransformationCmd->trSchemaMap_ls00.msSchemaMap_ls00[i]->smSourceSchema_ls00.asCharp());
                  SourceSchemas += " ";
                  rcBOOL = SAPDB_FALSE;
             }
             if (!existsSchema(     REINTERPRET_CAST(SAPDB_UTF8*,pTransformationCmd->trSchemaMap_ls00.msSchemaMap_ls00[i]->smTargetSchema_ls00.asCharp()),ErrText)) {
                  TargetSchemas +=  REINTERPRET_CAST(SAPDB_UTF8*,pTransformationCmd->trSchemaMap_ls00.msSchemaMap_ls00[i]->smTargetSchema_ls00.asCharp());
                  TargetSchemas += " ";
                  rcBOOL = SAPDB_FALSE;
             }
         }
         if ( rcBOOL ) {
              m_pRelationalStmt [0] = 0;
              for ( int i = 0;i < pTransformationCmd->trSchemaMap_ls00.msCount_ls00; i++ ) {
                    sp77sprintf(m_pRelationalStmt, 1024, CONST_CAST(SAPDB_Char*, Statement(ins_Transformation_507).CharPtr()),
                                  pTransformationCmd->trPackageid_ls00.StrPtr(), 
                                  pTransformationCmd->trSchemaMap_ls00.msSchemaMapName_ls00.asCharp(), 
                                  pTransformationCmd->trSchemaMap_ls00.msSchemaMap_ls00[i]->smSourceSchema_ls00.asCharp(),
                                  pTransformationCmd->trSchemaMap_ls00.msSchemaMap_ls00[i]->smTargetSchema_ls00.asCharp() );
                    executeSQL (m_pRelationalStmt,true);
                    m_pRelationalStmt [0] = 0;
              }
              SchemaMapName = REINTERPRET_CAST(SAPDB_UTF8*, pTransformationCmd->trSchemaMap_ls00.msSchemaMapName_ls00.asCharp());
              if ( existsSchemaMapDoubles(pTransformationCmd->trPackageid_ls00,SchemaMapName, ErrText) )  {
                   ExportSchemaMap(ErrText);
              }
         }
         else {
             SourceSchemas += TargetSchemas;
             pMsgInstance->ls98Msg(layCatalogExtract_els98, ErrText, errSchemaMapSchemasNotExists_els98, SourceSchemas.StrPtr(), 
                                   pTransformationCmd->trSchemaMap_ls00.msSchemaMapName_ls00.asCharp());
             rc = errSchemaMapSchemasNotExists_els98;
         }
    }
    return rc;
}

/*===========================================================================*
 *    TransformationDef::ExportSchemaMap
 *===========================================================================*/
SAPDB_Int4 TransformationDef::ExportSchemaMap(tsp00_Addr ErrText)
{
    ROUTINE_DBG_MEO00 ("TransformationDef::ExportSchemaMap");

    SAPDB_Int4  rc     = errOK_els00;
    SAPDB_Bool  rcBOOL = SAPDB_TRUE;

    MetaDataDef  *pMetaDataDef   = getMetaData();
    tls00_DBInfo *pDBInfo        = pMetaDataDef->getConnection ();

    tsp100_VersionResult vres = ls15CheckKernelVersion(pDBInfo, 7, 6, 0, 26);

    if ((IsEqual_esp100 == vres || IsNewer_esp100 == vres) )
    {
         tls00_TransformationCmd * pTransformationCmd  = getTransformationCmd();
         if (pTransformationCmd->trSchemaMap_ls00.msCount_ls00 == 0 )
             sp77sprintf(m_pTransformationStmt, 1024, CONST_CAST(SAPDB_Char*, Statement(del_Transformation_605).CharPtr()),
                   _T("") ,pTransformationCmd->trSchemaMap_ls00.msSchemaMapName_ls00.asCharp());
         else {
             StudioOAL_String Packageid;
             Packageid  = " PACKAGEGUID = x'";
             Packageid += pTransformationCmd->trPackageid_ls00;
             Packageid += "' AND ";
             sp77sprintf(m_pTransformationStmt, 1024, CONST_CAST(SAPDB_Char*, Statement(del_Transformation_605).CharPtr()),
                           Packageid.StrPtr(),pTransformationCmd->trSchemaMap_ls00.msSchemaMapName_ls00.asCharp());
         }
         executeSQL (m_pTransformationStmt,true);
         m_pTransformationStmt [0] = 0;
    }
    return rc;
}

/*===========================================================================*
 *    TransformationDef::ExportStatistic
 *===========================================================================*/
SAPDB_Int4 TransformationDef::ExportStatistic()
{
    ROUTINE_DBG_MEO00 ("TransformationDef::ExportStatistic");

    SAPDB_Int4        rc     =  errOK_els00;
    SAPDB_Bool        rcBOOL = SAPDB_FALSE;
    StudioOAL_String  _sysdba;
    char szErrText[MAX_REPLY_LENGTH_LS00];
    szErrText[0]             = 0;
    m_pTransformationStmt[0] = 0;

    rcBOOL = executeSQL("SELECT 1 FROM SYSDBA.SYSSTATISTICS WHERE ROWNO <= 1", true);
    if (SAPDB_FALSE == rcBOOL)
    {
        _sysdba = REINTERPRET_CAST(SAPDB_UTF8*, getMetaData()->getConnection()->dbiSYSDBA_Name_ls00.asCharp());
        switchUser(m_owner, _sysdba, szErrText);
        szErrText[0] = 0;

        m_StmtString = "CURRENT USER ";
        m_StmtString += _sysdba;
        executeSQL(m_StmtString, true);

        TRACE_PRNF_MLS99(("ExportStatistic", "switched user to %s", _sysdba.StrPtr() ));
    }

    sp77sprintf(m_pTransformationStmt, 1024, "UNLOAD STATISTICS");
    rcBOOL = executeSQL(m_pTransformationStmt, true);    
    if (SAPDB_TRUE == rcBOOL)
    {
        SAPDB_Bool _bSchemaColumnExists = SAPDB_FALSE;

        // Check for column schemaname in table SYSSTATISTICS available beginning with version 7.6.00.06
        StudioOAL_String _tabname   ("SYSSTATISTICS");
        StudioOAL_String _columnname("SCHEMANAME");
        _bSchemaColumnExists = existsColumn(_tabname, _columnname);

        m_StmtString = "DATAEXTRACT OWNER, TABLENAME, COLUMNNAME, INDEXNAME, \"PAGES\", DISTINCT_VALUES, AVG_LIST_LEN";
        if (SAPDB_TRUE == _bSchemaColumnExists) {
            m_StmtString += ", SCHEMANAME ";
        }
        m_StmtString += " FROM SYSDBA.SYSSTATISTICS ";

        if (m_pTransformationCmd->trPartSpec_ls00.psPartType_ls00 <= ptTable_els00) {
            m_StmtString += "WHERE OWNER = '";
            m_StmtString += m_owner;
            m_StmtString += "' AND TABLENAME = = '";
            m_StmtString += m_tablename;
            m_StmtString += "' ";
        }
        else if (ptAll_els00 == m_pTransformationCmd->trPartSpec_ls00.psPartType_ls00) {
            m_StmtString += "WHERE OWNER <> SYSDBA ";
        }
        else
        {
            if (ptSchema_els00 == m_pTransformationCmd->trPartSpec_ls00.psPartType_ls00)
            {
                if (SAPDB_TRUE == _bSchemaColumnExists) {
                    m_StmtString += "WHERE SCHEMANAME = '";
                }
                else {
                    m_StmtString += "WHERE OWNER = '";
                }
                m_StmtString += m_schemaname;
                m_StmtString += "' ";
            }
            else if (ptUser_els00 == m_pTransformationCmd->trPartSpec_ls00.psPartType_ls00) {
                m_StmtString += "WHERE OWNER = '";
                m_StmtString += m_owner;
                m_StmtString += "' ";
            }
        }
        m_StmtString += "OUTFILE '";
        m_StmtString += m_pTransformationCmd->trStatisticFile_ls00->fsFileName_ls00;
        m_StmtString += "' COMPRESSED ";
        m_StmtString += pszCodeTypeName_ls00[m_pTransformationCmd->trStatisticFile_ls00->fsFormat_ls00.ffoCodeType_ls00];
        m_StmtString += " INTERNAL";

        TRACE_PRNF_MLS99(("ExportStatistic", "Loader statement: %s", m_StmtString.StrPtr()));

        rc = ls11ParseDispatchLoadCommand(m_StmtString, getMetaData()->getConnection(), szErrText);
        if (errOK_els00 != rc)
        {
            ErrorHndl* pErrorObj = getMetaData()->m_pError;
            pErrorObj->setErrorText(REINTERPRET_CAST(SAPDB_UTF8*, szErrText));
            pErrorObj->isError(SAPDB_TRUE);
            pErrorObj->setNativeError(rc);
            rc = errLoadStatistics_els98;
        }
    }

    if (SAPDB_FALSE == _sysdba.Empty())
    {
        switchUser(_sysdba, m_owner, szErrText);
        szErrText[0] = 0;

        m_StmtString = "CURRENT USER ";
        m_StmtString += "\"";
        m_StmtString += m_owner;
        m_StmtString += "\"";
        executeSQL(m_StmtString, true);
    }

    return rc;
}
// TransformationDef::ExportStatistic()


/*===========================================================================*
 *    TransformationDef::ImportStatistic()
 *===========================================================================*/
SAPDB_Int4 TransformationDef::ImportStatistic()
{
    ROUTINE_DBG_MEO00 ("TransformationDef::ImportStatistic");

    SAPDB_Int4 rc = errOK_els00;
    SAPDB_Bool rcBOOL = SAPDB_FALSE;

    StudioOAL_String  _sysdba;

    char szErrText[MAX_REPLY_LENGTH_LS00];
    szErrText[0] = 0;
    m_pTransformationStmt[0] = 0;

    // Check visibility of table SYSDBA.SYSSTATISTICS (exists table doesn't work properly)
    rcBOOL = executeSQL("SELECT 1 FROM SYSDBA.SYSSTATISTICS WHERE ROWNO <= 1", true);
    if (SAPDB_FALSE == rcBOOL)
    {
        // Switch user to SYSDBA! (only for OI connection!)
        _sysdba = REINTERPRET_CAST(SAPDB_UTF8*, getMetaData()->getConnection()->dbiSYSDBA_Name_ls00.asCharp());
        switchUser(m_owner, _sysdba, szErrText);

        // user switch over ODBC!
        StudioOAL_String _userswitch_cmd("CURRENT USER ");
        _userswitch_cmd += _sysdba;
        executeSQL(_userswitch_cmd, true);

        szErrText[0] = 0;
    }

    // Empty table SYSSTATISTICS
    executeSQL("DELETE SYSDBA.SYSSTATISTICS", true);

    // Load table SYSSTATISTICS
    StudioOAL_String _tabname("SYSSTATISTICS");
    StudioOAL_String _columnname("SCHEMANAME");
    SAPDB_Bool _bSchemaColumnExists = existsColumn(_tabname, _columnname);

    StudioOAL_String Cmd("DATALOAD TABLE SYSDBA.SYSSTATISTICS "
                         "OWNER           1 CHAR DEFAULT NULL "
                         "TABLENAME       2 CHAR DEFAULT NULL "
                         "COLUMNNAME      3 CHAR DEFAULT NULL "
                         "INDEXNAME       4 CHAR DEFAULT NULL "
                         "\"PAGES\"       5 CHAR DEFAULT NULL "
                         "DISTINCT_VALUES 6 CHAR DEFAULT NULL "
                         "AVG_LIST_LEN    7 CHAR DEFAULT NULL ");
    
    if (SAPDB_TRUE == _bSchemaColumnExists)
    {
        Cmd += "SCHEMANAME 8 CHAR DEFAULT NULL ";
    }
    
    Cmd += "INFILE '";
    Cmd += m_pTransformationCmd->trStatisticFile_ls00->fsFileName_ls00;
    Cmd += "'";
    Cmd += " COMPRESSED ";
    Cmd += pszCodeTypeName_ls00[m_pTransformationCmd->trStatisticFile_ls00->fsFormat_ls00.ffoCodeType_ls00];
    Cmd += " INTERNAL";

    TRACE_PRNF_MLS99(("ImportStatistic", "Loader statement: %s", m_StmtString.StrPtr()));

    rc = ls11ParseDispatchLoadCommand(Cmd, getMetaData()->getConnection(), szErrText);
    if (errOK_els00 != rc)
    {
        ErrorHndl* pErrorObj = getMetaData()->m_pError;
        pErrorObj->setErrorText(REINTERPRET_CAST(SAPDB_UTF8*, szErrText));
        pErrorObj->isError(SAPDB_TRUE);
        pErrorObj->setNativeError(rc);
        rc = errLoadStatistics_els98;
    }
    else
    {
        // Switch user back: LOAD STATISTICS must run as user logged on
        if (SAPDB_FALSE == _sysdba.Empty())

        {
            switchUser(_sysdba, m_owner, szErrText);
            szErrText[0] = 0;

            // user switch over ODBC!
            StudioOAL_String _userswitch_cmd("CURRENT USER ");
            _userswitch_cmd += m_owner;
            executeSQL(_userswitch_cmd, true);
        }

        // Update table OPTIMIZERSTATISTICS
        sp77sprintf(m_pTransformationStmt, 1024, "LOAD STATISTICS");
        rcBOOL = executeSQL(m_pTransformationStmt, true);
        // Check error object
        if (SAPDB_FALSE == rcBOOL)
        {
            rc = errLoadStatistics_els98;
        }
    }

    return rc;
}
// TransformationDef::ImportStatistic()


/*===========================================================================*
 *    TransformationDef::updateTaskStatistic
 *===========================================================================*/
SAPDB_Bool TransformationDef::updateTaskStatistic()
{
    ROUTINE_DBG_MEO00 ("TransformationDef::updateTaskStatistic()");

    SAPDB_Bool       rcBool      = SAPDB_TRUE;
    if ( m_pTransformationCmd->trCatalogFile_ls00.fsFormat_ls00.ffoXmlNoTable_ls00 == false )
    {

        StudioOAL_String  record[1];

        _TCHAR sztransformation[4096];
        sp77sprintf(sztransformation, 4096, CONST_CAST(SAPDB_Char*, Statement(sel_Transformation_709).CharPtr()),
                                            m_pTransformationCmd->trPackageid_ls00.StrPtr());
        if ( executeSQL(sztransformation) )
        {
            readResult( record );
            m_nObjectCount = AtoI( record[0] );
        }
        sp77sprintf(sztransformation, 4096, CONST_CAST(SAPDB_Char*, Statement(sel_Transformation_710).CharPtr()),
                                            m_pTransformationCmd->trPackageid_ls00.StrPtr());
        if ( executeSQL( sztransformation ) )
        {
            readResult( record ) ;
            m_nTableCount  = AtoI( record[0] );
        }
        sp77sprintf(sztransformation, 4096, CONST_CAST(SAPDB_Char*, Statement(sel_Transformation_711).CharPtr()),
                                            m_pTransformationCmd->trPackageid_ls00.StrPtr());
        if ( executeSQL(sztransformation) )
        {
            readResult( record );
            m_nViewCount  = AtoI( record[0] );
        }
        sp77sprintf(sztransformation, 4096, CONST_CAST(SAPDB_Char*, Statement(sel_Transformation_712).CharPtr()),
                                            m_pTransformationCmd->trPackageid_ls00.StrPtr());
        if ( executeSQL(sztransformation) )
        {
            readResult( record );
            m_nSynonymCount = AtoI( record[0] );
        }
    }
    return rcBool;
}
/*===========================================================================*
 *    TransformationDef::update
 *===========================================================================*/
SAPDB_Bool TransformationDef::update( StudioOAL_String statement )
{
    ROUTINE_DBG_MEO00 ("TransformationDef::update");

    SAPDB_Bool       rcBool       = SAPDB_TRUE;

    rcBool = m_pUpdate->openResult(statement);
    if (SAPDB_FALSE == rcBool)
    {
        TRACE_PRNF_MLS99(("failed ", "%s\n", statement.StrPtr() ));
        LoaderMessages::Instance()->ls98PrintMessage(layCatalogExtract_els98, msgSimpleMsg_els98, statement.StrPtr() );
    }
    return rcBool;
}
/*===========================================================================*
 *    TransformationDef::updateDataCmd
 *===========================================================================*/
SAPDB_Bool TransformationDef::updateDataCmd()
{
    ROUTINE_DBG_MEO00 ("TransformationDef::updateDataCmd()");

    MetaDataDef    *pMetaDataDef = getMetaData();
    tls00_DBInfo   *pDBInfo      = pMetaDataDef->getConnection ();

    StudioOAL_String updateDataCmd;
    StudioOAL_String whereClause;

    whereClause  = " WHERE PACKAGEGUID = x'";
    whereClause += m_pTransformationCmd->trPackageid_ls00;
    whereClause += "' ";
   /*-------------------------------------------------------------------------*
    * Prepare the UPDATE statement.                                           *
    *-------------------------------------------------------------------------*/
    if ( m_pTransformationCmd->trPartSpec_ls00.psPartType_ls00 >= ptTable_els00 )
    {
        if ( NULL != m_pTransformationCmd->trDataFile_ls00.fsFileName_ls00 )
        {
           /*-------------------------------------------------------------------------*
            * Prepare the SET clause for the SQL statements.                          *
            *-------------------------------------------------------------------------*/
            StudioTD_String  servernode;
            StudioTD_String  serverdb;
            StudioTD_String  codetype;
            StudioTD_String  codepage;
            StudioTD_String  username;
            StudioTD_String  catalogstream;
            StudioTD_String  catalogstreamname;
            StudioTD_String  catalogstreamebid;
            StudioTD_String  datastreamebid;
            StudioOAL_String parameter[13];
            SAPDB_Char       destBeg[1000] ;
            SAPDB_UInt4      destAt  = 1000;
            // stepid
            if ( m_pTransformationCmd->trTransSpec_ls00 == tsExport_ls00 )
                parameter[0]  = " + ";
            if ( m_pTransformationCmd->trTransSpec_ls00 == tsImport_ls00 )
                parameter[0]  = " - ";
            // servernode
            parameter[1]  = "'";
            memset(destBeg,0,sizeof(destBeg));
            pMetaDataDef->getConnection()->dbiOdbcConnection.GetServerNode( REINTERPRET_CAST(unsigned char*, destBeg ), destAt);
            servernode.ConvertFromASCII_Latin1( destBeg, destBeg+strlen(destBeg));
            parameter[1] += servernode;
            parameter[1] += "'";
            // serverdb
            parameter[2]  = "'";
            memset(destBeg,0,sizeof(destBeg));
            pMetaDataDef->getConnection()->dbiOdbcConnection.GetServerDb ( REINTERPRET_CAST(unsigned char*,destBeg ),destAt);
            serverdb.ConvertFromASCII_Latin1( destBeg, destBeg+strlen(destBeg));
            parameter[2] += serverdb;
            parameter[2] += "'";
            // username
            parameter[3]  = "'";
            memset(destBeg,0,sizeof(destBeg));
            pMetaDataDef->getConnection()->dbiOdbcConnection.GetUser( REINTERPRET_CAST(unsigned char*,destBeg ),destAt);
            username.ConvertFromASCII_Latin1( destBeg, destBeg+strlen(destBeg));
            parameter[3] += username;
            parameter[3] += "'";
            // codepage
            if (ctAscii_els00 == pDBInfo->dbiSession_ls00.siCodeType_ls00)
            {
                parameter[4]  = "'";
                parameter[4] += pDBInfo->dbiSession_ls00.siCodePage_ls00.name.asCharp();
                parameter[4] += "'";
            }
            else
            {
                parameter[4]  = "NULL";
            }
            // storagedevice
            parameter[5]  =  pszBackupToolValue_ls00[pDBInfo->dbiSession_ls00.siBackupTool_ls00];
            // transformation
            parameter[6]  =  pszTransSpecValue_ls00[m_pTransformationCmd->trTransSpec_ls00];
            // part
            parameter[7]  =  pszPartTypeValue_ls00[m_pTransformationCmd->trPartSpec_ls00.psPartType_ls00];
            // datastreamtype
            parameter[8]  = pszDeviceTypeValue_ls00[m_pTransformationCmd->trDataFile_ls00.fsDeviceType_ls00];
            // datastreamebid
            if ( NULL == m_pTransformationCmd->trDataFile_ls00.fsBackupId_ls00.bBackupIdName_ls00 )
                parameter[9] = datastreamebid.Assign(REINTERPRET_CAST(SAPDB_UTF8*,
                                                     pDBInfo->dbiSession_ls00.siNullSpec_ls00.asCharp()));
            else
                parameter[9] = datastreamebid.Assign(REINTERPRET_CAST(SAPDB_UTF8*,
                                                     m_pTransformationCmd->trDataFile_ls00.fsBackupId_ls00.bBackupIdName_ls00));
            // datastreamformat
            if  ( true == m_pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoPage_ls00 )
                  parameter[10]   = "'PAGES'";
            if  ( true == m_pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoCompact_ls00 )
                  parameter[10]  =  "'RECORDS'";
            if  ( true == m_pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoFormatted_ls00 )
                  parameter[10]   = "'FORMATTED'";
            if  ( true == m_pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoCompress_ls00 )
                  parameter[10]  =  "'COMPRESSED'";
            // codetype
            parameter[11] = pszCodeTypeValue_ls00[m_pTransformationCmd->trDataFile_ls00.fsFormat_ls00.ffoCodeType_ls00];
            // transactionsize
            _TCHAR _transactionsize [256];
            _transactionsize        [0] = 0;
            _itot(pDBInfo->dbiSession_ls00.siTASize_ls00,_transactionsize,10);
            parameter[12].ConvertFromASCII_Latin1(_transactionsize,_transactionsize + _tcslen(_transactionsize));
            updateDataCmd   = m_upd_Transformation_800;
            updateDataCmd  += " SET STEPID =  ";
            updateDataCmd  += parameter[0];
            updateDataCmd  += "STEPID";
            updateDataCmd  += ", SEQ_NO = ";
            updateDataCmd  += parameter[0];
            updateDataCmd  += "SEQ_NO";
            updateDataCmd  += ", SERVERNODE =  ";
            updateDataCmd  += parameter[1];
            updateDataCmd  += ", SERVERDB =  ";
            updateDataCmd  += parameter[2];
            updateDataCmd  += ", USERNAME =  ";
            updateDataCmd  += parameter[3];
            updateDataCmd  += ", CODEPAGEID =  ";
            updateDataCmd  += parameter[4];
            updateDataCmd  += ", STORAGEDEVICE = ";
            updateDataCmd  += parameter[5];
            updateDataCmd  += ", TRANSFORMATION = ";
            updateDataCmd  += parameter[6];
            updateDataCmd  += ", PART = ";
            updateDataCmd  += parameter[7];
            updateDataCmd  += ", DATASTREAMTYPE = ";
            updateDataCmd  += parameter[8];
            updateDataCmd  += ", DATASTREAMEBID = '";
            if ( parameter[9].Compare("") == 0 )
            {
                StudioOAL_String datastreamebid;
                parameter[9] = datastreamebid.Assign(REINTERPRET_CAST(SAPDB_UTF8*,
                                                     getMetaData()->getConnection()->dbiSession_ls00.siNullSpec_ls00.asCharp()));
            }
            updateDataCmd  += parameter[9];
            updateDataCmd  += "'";
            updateDataCmd  += ", DATASTREAMFORMAT = ";
            updateDataCmd  += parameter[10];
            updateDataCmd  += ", CODETYPE =  ";
            updateDataCmd  += parameter[11];
            updateDataCmd  += ", TRANSACTIONSIZE = ";
            updateDataCmd  += parameter[12];
            updateDataCmd  += ", RESTART = TRUE ";
            updateDataCmd  += whereClause;

            m_pUpdate->openResult(updateDataCmd);
        }
        updateTaskStatistic();
    }
    return true;
}//TransformationDef::updateDataCmd

/*===========================================================================*
 *    TransformationDef::updateCatalogCmd
 *===========================================================================*/
SAPDB_Bool TransformationDef::updateCatalogCmd()
{
    ROUTINE_DBG_MEO00 ("TransformationDef::updateCatalogCmd()");

    MetaDataDef             *pMetaDataDef       = getMetaData();
    tls00_DBInfo            *pDBInfo            = pMetaDataDef->getConnection ();

    StudioOAL_String  whereClause;
    StudioOAL_String  updateCatalogCmd;

    whereClause  = " WHERE PACKAGEGUID = x'";
    whereClause += m_pTransformationCmd->trPackageid_ls00;
    whereClause += "' ";

    /*-------------------------------------------------------------------------*
     * Prepare the UPDATE statement.                                           *
     *-------------------------------------------------------------------------*/
    if ( NULL != m_pTransformationCmd->trCatalogFile_ls00.fsFileName_ls00 )
    {
       /*-------------------------------------------------------------------------*
        * Prepare the SET clause for the SQL statements.                        *
        *-------------------------------------------------------------------------*/
        StudioTD_String  servernode;
        StudioTD_String  serverdb;
        StudioTD_String  username;
        StudioTD_String  catalogstreamname;
        StudioTD_String  catalogstreamebid;
        StudioOAL_String parameter[13];
        SAPDB_Char       destBeg[1000] ;
        SAPDB_UInt4      destAt  = 1000;
        // stepid
        if ( m_pTransformationCmd->trTransSpec_ls00 == tsExport_ls00 )
             parameter[0]  = " + ";
        if ( m_pTransformationCmd->trTransSpec_ls00 == tsImport_ls00 )
             parameter[0]  = " - ";
        // servernode
        parameter[1]  = "'";
        memset(destBeg,0,sizeof(destBeg));
        pMetaDataDef->getConnection()->dbiOdbcConnection.GetServerNode( REINTERPRET_CAST(unsigned char*, destBeg ), destAt);
        servernode.ConvertFromASCII_Latin1( destBeg, destBeg+strlen(destBeg));
        parameter[1] += servernode;
        parameter[1] += "'";
        // serverdb
        parameter[2]  = "'";
        memset(destBeg,0,sizeof(destBeg));
        pMetaDataDef->getConnection()->dbiOdbcConnection.GetServerDb ( REINTERPRET_CAST(unsigned char*,destBeg ),destAt);
        serverdb.ConvertFromASCII_Latin1( destBeg, destBeg+strlen(destBeg));
        parameter[2] += serverdb;
        parameter[2] += "'";
        // codetype
        parameter[3] = pszCodeTypeValue_ls00[pDBInfo->dbiSession_ls00.siCodeType_ls00];
        // username
        parameter[4]  = "'";
        memset(destBeg,0,sizeof(destBeg));
        pMetaDataDef->getConnection()->dbiOdbcConnection.GetUser( REINTERPRET_CAST(unsigned char*,destBeg ),destAt);
        username.ConvertFromASCII_Latin1( destBeg, destBeg+strlen(destBeg));
        parameter[4] += username;
        parameter[4] += "'";
        // codepage
        if (ctAscii_els00 == pDBInfo->dbiSession_ls00.siCodeType_ls00)
        {
            parameter[5]  = "'";
            parameter[5] += pDBInfo->dbiSession_ls00.siCodePage_ls00.name.asCharp();
            parameter[5] += "'";
        }
        else
        {
            parameter[5]  = "NULL";
        }
        // storagedevice
        parameter[6]  =  pszBackupToolValue_ls00[pDBInfo->dbiSession_ls00.siBackupTool_ls00];
        // transformation
        parameter[7]  =  pszTransSpecValue_ls00[m_pTransformationCmd->trTransSpec_ls00];
        // part
        parameter[8]  =  pszPartTypeValue_ls00 [m_pTransformationCmd->trPartSpec_ls00.psPartType_ls00];
        // catalogstreamtype
        parameter[9]  = pszDeviceTypeValue_ls00[m_pTransformationCmd->trCatalogFile_ls00.fsDeviceType_ls00];
        // catalogstreamname
        _TCHAR* pszDest = strrchr(m_pTransformationCmd->trCatalogFile_ls00.fsFileName_ls00, SLASH_LS00);
        if (NULL == pszDest)
        pszDest = strrchr(m_pTransformationCmd->trCatalogFile_ls00.fsFileName_ls00, BACKSLASH_LS00);
        if (NULL == pszDest)
            pszDest = m_pTransformationCmd->trCatalogFile_ls00.fsFileName_ls00;
        pszDest = (pszDest == m_pTransformationCmd->trCatalogFile_ls00.fsFileName_ls00) ? pszDest : pszDest + 1;
        catalogstreamname.Assign(REINTERPRET_CAST(SAPDB_UTF8*, pszDest));
        parameter[10]  = "'";
        parameter[10] += catalogstreamname;
        parameter[10] += "'";
        // catalogstreamebid
        if ( tsImport_ls00  == m_pTransformationCmd->trTransSpec_ls00 &&
             dtypeTape_ls00 == m_pTransformationCmd->trCatalogFile_ls00.fsDeviceType_ls00  )
        {
            if ( NULL == m_pTransformationCmd->trCatalogFile_ls00.fsBackupId_ls00.bBackupIdName_ls00 ) {
                parameter[11] = catalogstreamebid.Assign(REINTERPRET_CAST(SAPDB_UTF8*,
                                                         pDBInfo->dbiSession_ls00.siNullSpec_ls00.asCharp()));
            }
            else {
                parameter[11] = catalogstreamebid.Assign(REINTERPRET_CAST(SAPDB_UTF8*,
                                                         m_pTransformationCmd->trCatalogFile_ls00.fsBackupId_ls00.bBackupIdName_ls00));
            }
        }
        // catalogstreamformat
        if  ( true == m_pTransformationCmd->trCatalogFile_ls00.fsFormat_ls00.ffoXml_ls00 )
            parameter[12] = "'XML'";
        else
            parameter[12] = "'DDL'";

        /*-------------------------------------------------------------------------*
        * Prepare the WHERE clause for the SQL statements.                        *
        *-------------------------------------------------------------------------*/
        whereClause        = " WHERE PACKAGEGUID = x'";
        whereClause       += m_pTransformationCmd->trPackageid_ls00;
        whereClause       += "' ";
        updateCatalogCmd   = m_upd_Transformation_800;
        updateCatalogCmd  += " SET SERVERNODE = ";
        updateCatalogCmd  += parameter[1];
        updateCatalogCmd  += ", SERVERDB =  ";
        updateCatalogCmd  += parameter[2];
        updateCatalogCmd  += ", CODETYPE =  ";
        updateCatalogCmd  += parameter[3];
        updateCatalogCmd  += ", USERNAME =  ";
        updateCatalogCmd  += parameter[4];
        updateCatalogCmd  += ", CODEPAGEID =  ";
        updateCatalogCmd  += parameter[5];
        updateCatalogCmd  += ", STORAGEDEVICE = ";
        updateCatalogCmd  += parameter[6];
        updateCatalogCmd  += ", TRANSFORMATION = ";
        updateCatalogCmd  += parameter[7];
        updateCatalogCmd  += ", PART = ";
        updateCatalogCmd  += parameter[8];
        updateCatalogCmd  += ", CATALOGSTREAMTYPE = ";
        updateCatalogCmd  += parameter[9];
        updateCatalogCmd  += ", CATALOGSTREAMNAME = ";
        updateCatalogCmd  += parameter[10];
        updateCatalogCmd  += ", CATALOGSTREAMEBID = '";
        if ( parameter[11].Compare("") == 0 )
        {
            parameter[11] = REINTERPRET_CAST(SAPDB_UTF8*,
                                             getMetaData()->getConnection()->dbiSession_ls00.siNullSpec_ls00.asCharp());
        }
        updateCatalogCmd  += parameter[11];
        updateCatalogCmd  += "', CATALOGSTREAMFORMAT = ";
        updateCatalogCmd  += parameter[12];
        updateCatalogCmd  += ", EXCLUDE = FALSE, RESTART = TRUE ";
        updateCatalogCmd  += whereClause;

        if ( m_pTransformationCmd->trPartSpec_ls00.psPartType_ls00 >= ptTable_els00 &&
             m_pUpdate->openResult(updateCatalogCmd) )
        
        updateTaskStatistic();
    }
    if ( m_pTransformationCmd->trPartSpec_ls00.psPartType_ls00 == ptTable_els00 )
         m_nObjectCount = 1;

    return true;
}//TransformationDef::updateCatalogCmd

/*===========================================================================*
 *    TransformationDef::updateCatalog
 *===========================================================================*/
SAPDB_Bool TransformationDef::updateCatalog()
{
    ROUTINE_DBG_MEO00 ("TransformationDef::updateCatalog()");
    LoaderMessages *pMsgInstance = LoaderMessages::Instance();
    /*-------------------------------------------------------------------------*
    * Prepare the SQL statement.                                              *
    *-------------------------------------------------------------------------*/
    StudioOAL_String updateCatalog;
    m_pTransformationStmt[0] = 0;
    sp77sprintf(m_pTransformationStmt, 1024, CONST_CAST(SAPDB_Char*, m_upd_Transformation_802.CharPtr()),
                                             m_pTransformationCmd->trPackageid_ls00.StrPtr(),
                                             m_schemaname.StrPtr(),
                                             m_tablename.StrPtr(), m_tabletype.StrPtr());
    updateCatalog = REINTERPRET_CAST(SAPDB_UTF8*, m_pTransformationStmt);

    if (!m_pUpdate->openResult (updateCatalog))
    {
        TRACE_PRNF_MLS99(("failed ", "%s\n", updateCatalog.StrPtr() ));
        pMsgInstance->ls98PrintMessage(layCatalogExtract_els98, msgSimpleMsg_els98 , updateCatalog.StrPtr() );
        return false;
    }
    return true;
}
/*===========================================================================*
 *    TransformationDef::updateBegin
 *===========================================================================*/
SAPDB_Bool TransformationDef::updateBegin()
{
    ROUTINE_DBG_MEO00 ("TransformationDef::updateBegin ()");
    LoaderMessages *pMsgInstance = LoaderMessages::Instance();

    if ( m_currentStream != stPackage_els00 )
    {
        /*-------------------------------------------------------------------------*
         * Prepare the SQL statement.                                              *
         *-------------------------------------------------------------------------*/
        StudioOAL_String updateBegin;
         m_pTransformationStmt[0] = 0;
         sp77sprintf(m_pTransformationStmt, 1024, CONST_CAST(SAPDB_Char*, m_upd_Transformation_804.CharPtr()),
                                                  m_pTransformationCmd->trPackageid_ls00.StrPtr(),
                                                  m_schemaname.StrPtr(), m_tablename.StrPtr() );
         updateBegin = REINTERPRET_CAST(SAPDB_UTF8*, m_pTransformationStmt);
         if ( !m_pUpdate->openResult(updateBegin) )
         {
             TRACE_PRNF_MLS99(("failed ", "%s\n", updateBegin.StrPtr()));
             pMsgInstance->ls98PrintMessage(layCatalogExtract_els98, msgSimpleMsg_els98 , updateBegin.StrPtr() );
             return false;
         }
    }
    return true;
}
/*===========================================================================*
 *    TransformationDef::updateDataStream
 *===========================================================================*/
SAPDB_Bool TransformationDef::updateDataStream()
{
    ROUTINE_DBG_MEO00 ("TransformationDef::updateDataStream()");
    LoaderMessages *pMsgInstance = LoaderMessages::Instance();

    if ( m_currentStream != stPackage_els00 )
    {
         StudioOAL_String updateDataStream;
         m_pTransformationStmt[0] = 0;
         sp77sprintf(m_pTransformationStmt, 1024, CONST_CAST(SAPDB_Char*, m_upd_Transformation_803.CharPtr()),
                                                  m_datastreamname.StrPtr(),
                                                  m_pTransformationCmd->trPackageid_ls00.StrPtr(),
                                                  m_schemaname.StrPtr(), m_tablename.StrPtr() );
         updateDataStream = REINTERPRET_CAST(SAPDB_UTF8*, m_pTransformationStmt);
         if ( !m_pUpdate->openResult (updateDataStream) )
         {
             TRACE_PRNF_MLS99(("failed ", "%s\n" , updateDataStream.StrPtr() ));
             pMsgInstance->ls98PrintMessage(layCatalogExtract_els98, msgSimpleMsg_els98 , updateDataStream.StrPtr() );
             return false;
         }
    }
    return true;
}
/*===========================================================================*
 *    TransformationDef::updateHeader
 *===========================================================================*/
SAPDB_Bool TransformationDef::updateHeader()
{
    ROUTINE_DBG_MEO00 ("TransformationDef::updateHeader()");
    LoaderMessages *pMsgInstance = LoaderMessages::Instance();

    if ( m_currentStream != stPackage_els00 )
    {
        /*-------------------------------------------------------------------------*
        * Prepare the SQL statement.                                              *
        *-------------------------------------------------------------------------*/
        StudioOAL_String updateHeader;
        m_pTransformationStmt[0] = 0;
        sp77sprintf(m_pTransformationStmt, 1024, CONST_CAST(SAPDB_Char*, m_upd_Transformation_806.CharPtr()),
                                                 "0",
                                                 m_headerpos_block.StrPtr(),
                                                 m_headerpos_byte.StrPtr(),
                                                 m_pTransformationCmd->trPackageid_ls00.StrPtr(),
                                                 m_schemaname.StrPtr(), m_tablename.StrPtr() );
        updateHeader = REINTERPRET_CAST(SAPDB_UTF8*, m_pTransformationStmt);

        if ( !m_pUpdate->openResult (updateHeader) )
        {
            TRACE_PRNF_MLS99(("failed ", "%s\n" , updateHeader.StrPtr() ));
            pMsgInstance->ls98PrintMessage(layCatalogExtract_els98, msgSimpleMsg_els98 , updateHeader.StrPtr() );
            return SAPDB_FALSE;
        }
    }
    return SAPDB_TRUE;
}
/*===========================================================================*
 *    TransformationDef::updateTrailer
 *===========================================================================*/
SAPDB_Bool TransformationDef::updateTrailer()
{
    ROUTINE_DBG_MEO00 ("TransformationDef::updateTrailer()");
    LoaderMessages *pMsgInstance = LoaderMessages::Instance();

    if ( m_currentStream != stPackage_els00 )
    {
        /*-------------------------------------------------------------------------*
        * Prepare the SQL statement.                                              *
        *-------------------------------------------------------------------------*/
        StudioOAL_String updateTrailer;
        m_pTransformationStmt[0] = 0;
        sp77sprintf(m_pTransformationStmt, 1024, CONST_CAST(SAPDB_Char*, m_upd_Transformation_813.CharPtr()),
                                                 m_trailerpos_block.StrPtr(),
                                                 m_trailerpos_byte.StrPtr(),
                                                 m_pTransformationCmd->trPackageid_ls00.StrPtr(),
                                                 m_schemaname.StrPtr(), m_tablename.StrPtr() );
        updateTrailer = REINTERPRET_CAST(SAPDB_UTF8*, m_pTransformationStmt);

        if ( !m_pUpdate->openResult (updateTrailer) )
        {
            TRACE_PRNF_MLS99(("failed ", "%s\n", updateTrailer.StrPtr() ));
            pMsgInstance->ls98PrintMessage(layCatalogExtract_els98, msgSimpleMsg_els98 , updateTrailer.StrPtr() );
            return SAPDB_FALSE;
        }
    }
    return SAPDB_TRUE;
}
/*===========================================================================*
 *    TransformationDef::updateExportData
 *===========================================================================*/
SAPDB_Bool TransformationDef::updateExportData()
{
    ROUTINE_DBG_MEO00 ("TransformationDef::updateExportData()");
    LoaderMessages *pMsgInstance = LoaderMessages::Instance();

    if ( m_currentStream != stPackage_els00 )
    {
        /*-------------------------------------------------------------------------*
         * Prepare the SQL statement.                                              *
         *-------------------------------------------------------------------------*/
         StudioOAL_String updateData;
         m_pTransformationStmt[0] = 0;
         sp77sprintf(m_pTransformationStmt, 1024, CONST_CAST(SAPDB_Char*, m_upd_Transformation_807.CharPtr()),
                                                  m_rows_exported.StrPtr(),
                                                  m_datapos_block.StrPtr(),
                                                  m_datapos_byte.StrPtr(),
                                                  m_pTransformationCmd->trPackageid_ls00.StrPtr(),
                                                  m_schemaname.StrPtr(), m_tablename.StrPtr() );
         updateData = REINTERPRET_CAST(SAPDB_UTF8*, m_pTransformationStmt);

         if ( !m_pUpdate->openResult (updateData) )
         {
             TRACE_PRNF_MLS99(("failed ", "%s\n" , updateData.StrPtr() ));
             pMsgInstance->ls98PrintMessage(layCatalogExtract_els98, msgSimpleMsg_els98 , updateData.StrPtr() );
             return SAPDB_FALSE;
         }
    }
    return  SAPDB_TRUE;
}

/*===========================================================================*
 *    TransformationDef::updateImportData
 *===========================================================================*/
SAPDB_Bool TransformationDef::updateImportData()
{
    ROUTINE_DBG_MEO00 ("TransformationDef::updateImportData()");
    LoaderMessages *pMsgInstance = LoaderMessages::Instance(); 

    if ( m_currentStream != stPackage_els00 )
    {
        /*-------------------------------------------------------------------------*
         * Prepare the SQL statement.                                              *
         *-------------------------------------------------------------------------*/
         StudioOAL_String updateData;
         m_pTransformationStmt[0] = 0;
         if ( m_datapos_block.Compare("-1") == 0)
             m_datapos_block = "0";

         sp77sprintf(m_pTransformationStmt, 1024, CONST_CAST(SAPDB_Char*, m_upd_Transformation_807.CharPtr()),
                                                  m_rows_imported.StrPtr(),
                                                  m_datapos_block.StrPtr(),
                                                  m_datapos_byte.StrPtr(),
                                                  m_pTransformationCmd->trPackageid_ls00.StrPtr(),
                                                  m_schemaname.StrPtr(), m_tablename.StrPtr() );
         updateData = REINTERPRET_CAST(SAPDB_UTF8*, m_pTransformationStmt);

         if ( !m_pUpdate->openResult (updateData) )
         {
             TRACE_PRNF_MLS99(("failed ", "%s\n", updateData.StrPtr() ));
             pMsgInstance->ls98PrintMessage(layCatalogExtract_els98, msgSimpleMsg_els98, updateData.StrPtr() );
             return SAPDB_FALSE;
         }
    }
    return  SAPDB_TRUE;
}
/*===========================================================================*
 *    TransformationDef::updateRestart
 *===========================================================================*/
SAPDB_Bool TransformationDef::updateRestart()
{
    ROUTINE_DBG_MEO00 ("TransformationDef::updateRestart()");
    LoaderMessages *pMsgInstance = LoaderMessages::Instance();

    if ( m_currentStream != stPackage_els00 )
    {
        /*-------------------------------------------------------------------------*
        * Prepare the SQL statement.                                              *
        *-------------------------------------------------------------------------*/
        StudioOAL_String updateRestart;
        m_pTransformationStmt[0] = 0;
        sp77sprintf(m_pTransformationStmt, 1024, CONST_CAST(SAPDB_Char*, m_upd_Transformation_808.CharPtr()),
                                                 m_pTransformationCmd->trPackageid_ls00.StrPtr(),
                                                 m_schemaname.StrPtr(), m_tablename.StrPtr());
        updateRestart = REINTERPRET_CAST(SAPDB_UTF8*, m_pTransformationStmt);

        if ( !m_pUpdate->openResult (updateRestart) )
        {
            TRACE_PRNF_MLS99(("failed ", "%s\n" , updateRestart.StrPtr() ));
            pMsgInstance->ls98PrintMessage(layCatalogExtract_els98, msgSimpleMsg_els98 , updateRestart.StrPtr() );
            return SAPDB_FALSE;
        }
    }
    return SAPDB_TRUE;
}
/*===========================================================================*
 *    TransformationDef::updateExclude
 *===========================================================================*/
SAPDB_Bool TransformationDef::updateExclude()
{
    ROUTINE_DBG_MEO00 ("TransformationDef::updateExclude");
    LoaderMessages *pMsgInstance = LoaderMessages::Instance();

    if ( m_currentStream != stPackage_els00 )
    {
        /*-------------------------------------------------------------------------*
        * Prepare the SQL statement.                                              *
        *-------------------------------------------------------------------------*/
        StudioOAL_String updateExclude;
        m_pTransformationStmt[0] = 0;
        sp77sprintf(m_pTransformationStmt, 1024, CONST_CAST(SAPDB_Char*, m_upd_Transformation_811.CharPtr()),
                                                 m_pTransformationCmd->trPackageid_ls00.StrPtr(),
                                                 m_schemaname.StrPtr(), m_tablename.StrPtr());
        updateExclude = REINTERPRET_CAST(SAPDB_UTF8*, m_pTransformationStmt);
        if ( !m_pUpdate->openResult (updateExclude) )
        {
            TRACE_PRNF_MLS99(("failed ", "%s\n" , updateExclude.StrPtr() ));
            pMsgInstance->ls98PrintMessage(layCatalogExtract_els98, msgSimpleMsg_els98 , updateExclude.StrPtr() );
            return SAPDB_FALSE;
        }
    }
    return SAPDB_TRUE;
}
/*===========================================================================*
 *    TransformationDef::updateEnd
 *===========================================================================*/
SAPDB_Bool
TransformationDef::updateEnd()
{
    ROUTINE_DBG_MEO00 ("TransformationDef::updateEnd");
    LoaderMessages *pMsgInstance = LoaderMessages::Instance();

    if ( m_currentStream != stPackage_els00 )
    {
        /*-------------------------------------------------------------------------*
        * Prepare the SQL statement.                                              *
        *-------------------------------------------------------------------------*/
        StudioOAL_String updateEnd;
        m_pTransformationStmt[0] = 0;
        sp77sprintf(m_pTransformationStmt, 1024, CONST_CAST(SAPDB_Char*, m_upd_Transformation_805.CharPtr()),
                                                 m_pTransformationCmd->trPackageid_ls00.StrPtr(),
                                                 m_schemaname.StrPtr(), m_tablename.StrPtr());
        updateEnd = REINTERPRET_CAST(SAPDB_UTF8*, m_pTransformationStmt);

        if ( !m_pUpdate->openResult (updateEnd) )
        {
            TRACE_PRNF_MLS99(("failed ", "%s\n", updateEnd.StrPtr() ));
            pMsgInstance->ls98PrintMessage(layCatalogExtract_els98, msgSimpleMsg_els98 , updateEnd.StrPtr() );
            return SAPDB_FALSE;
        }
    }
    return SAPDB_TRUE;
}
/*===========================================================================*
 *    TransformationDef::updatePerformance
 *===========================================================================*/
SAPDB_Bool TransformationDef::updatePerformance()
{
    ROUTINE_DBG_MEO00 ("TransformationDef::updatePerformance()");
    LoaderMessages *pMsgInstance = LoaderMessages::Instance();

    if ( m_currentStream != stPackage_els00 )
    {
        StudioOAL_String  updatePerformance;
        _TCHAR _request[64];
        _TCHAR _receive[64];
        _TCHAR _io     [64];
        _TCHAR _log    [64];

        extern tls00_Performance performance;
        sprintf(_request, "%f", (double)performance.perRequest_ls00/1000000);
        sprintf(_receive, "%f", (double)performance.perReceive_ls00/1000000 );
        sprintf(_io     , "%f", (double)performance.perIO_ls00 /1000000 );
        sprintf(_log    , "%f", (double)performance.perLog_ls00/1000000 ); 

       /*-------------------------------------------------------------------------*
        * Prepare the SQL statement.                                              *
        *-------------------------------------------------------------------------*/
        m_pTransformationStmt[0] = 0;
        sp77sprintf(m_pTransformationStmt, 1024, CONST_CAST(SAPDB_Char*, m_upd_Transformation_812.CharPtr()),
                                                 _request, _receive, _io, _log,
                                                 m_pTransformationCmd->trPackageid_ls00.StrPtr(),
                                                 m_schemaname.StrPtr(), m_tablename.StrPtr() );
        updatePerformance = REINTERPRET_CAST(SAPDB_UTF8*, m_pTransformationStmt);

        if ( !m_pUpdate->openResult (updatePerformance) )
        {
            TRACE_PRNF_MLS99(("failed ", "%s\n" , updatePerformance.StrPtr() ));
            pMsgInstance->ls98PrintMessage(layCatalogExtract_els98, msgSimpleMsg_els98 , updatePerformance.StrPtr() );
            return SAPDB_FALSE;
        }
    }
    return SAPDB_TRUE;
}
/*===========================================================================*
 *    TransformationDef::ImportTransformationTables
 *===========================================================================*/
SAPDB_Bool TransformationDef::ImportTransformationTables()
{
    ROUTINE_DBG_MEO00 ("TransformationDef::ImportTransformationTables");

    SAPDB_Bool rcBool   = SAPDB_TRUE;
    SAPDB_Bool rcExists = SAPDB_FALSE;

    MetaDataDef   *pMetaDataDef  = getMetaData();
    tls00_DBInfo  *pDBInfo       = pMetaDataDef->getConnection();

    // change current user or current schema
    StudioOAL_String sessionUser, currentuser, sessionSchema, currentSchema;
    sessionUser.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer, pDBInfo->dbiSession_ls00.siUserName_ls00.asCharp()));
    currentuser = sessionUser;
    sessionSchema.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer, pDBInfo->dbiSession_ls00.siSchemaName_ls00));

    m_StmtString = Statement(exist_Transformation_112);
    rcBool = executeSQL(m_StmtString);

    SQLSetConnectAttr(pDBInfo->dbiOdbcConnection.HDBC(), SQL_PRODUCER, REINTERPRET_CAST(SQLPOINTER, sp1pr_installation), SQL_IS_POINTER);

    // TRANSFORMATIONMODEL
    if (SAPDB_TRUE == rcBool)
    {
        rcExists = existsTable(Statement(exist_Transformation_100)); 
        if (SAPDB_FALSE == rcExists)
        {
            executeSQL("COMMIT");
            m_StmtString = Statement(trans_Transformation_316);
            executeSQL(m_StmtString, true);

            m_StmtString = Statement(ddl_Transformation_300);
            rcBool = executeSQL(m_StmtString); 
            if (SAPDB_TRUE == rcBool) {
                m_StmtString = Statement(dal_Transformation_400);
                rcBool = executeSQL(m_StmtString);
            }
            executeSQL("COMMIT");
        }
        else
        {
            executeSQL("COMMIT");
            m_StmtString = Statement(trans_Transformation_316);
            executeSQL(m_StmtString, true); 
            m_StmtString = Statement(dal_Transformation_400);
            executeSQL(m_StmtString, true);
            m_StmtString = Statement(ddl_Transformation_301);
            executeSQL(m_StmtString, true);
            m_StmtString = Statement(ddl_Transformation_311);
            executeSQL(m_StmtString, true);
            m_StmtString = Statement(ddl_Transformation_312);
            executeSQL(m_StmtString, true);
            m_StmtString = Statement(ddl_Transformation_313);
            executeSQL(m_StmtString, true);
            m_StmtString = Statement(ddl_Transformation_314);
            executeSQL(m_StmtString, true);
            m_StmtString = Statement(ddl_Transformation_315);
            executeSQL(m_StmtString, true);
            m_StmtString = Statement(ddl_Transformation_316);
            executeSQL(m_StmtString, true);
            executeSQL("COMMIT");
        }
    }
    // TRANSFORMATIONPACKAGES
    if (SAPDB_TRUE == rcBool)
    {
        rcExists = existsTable (Statement(exist_Transformation_101)); 
        if (SAPDB_FALSE == rcExists)
        {
            m_StmtString = Statement(ddl_Transformation_304);
            rcBool = executeSQL(m_StmtString); 
            if (SAPDB_TRUE == rcBool) {
                m_StmtString = Statement(dal_Transformation_401);
                rcBool = executeSQL(m_StmtString);
            }
        }
    }
    // TRANSFORMATIONSTEPS
    if (SAPDB_TRUE == rcBool)
    {
        rcExists = existsTable (Statement(exist_Transformation_102)); 
        if (SAPDB_FALSE == rcExists)
        {
            m_StmtString = Statement(ddl_Transformation_305);
            rcBool = executeSQL(m_StmtString); 
            if (SAPDB_TRUE == rcBool) {
                m_StmtString = Statement(dal_Transformation_403);
                rcBool = executeSQL(m_StmtString);
            }
        }
    }
    // TRANSFORMATIONRESOURCES
    if (SAPDB_TRUE == rcBool)
    {
        rcExists = existsTable (Statement(exist_Transformation_103));
        if (SAPDB_FALSE == rcExists)
        {
            m_StmtString = Statement(ddl_Transformation_306);
            rcBool = executeSQL(m_StmtString); 
            if (SAPDB_TRUE == rcBool) {
                m_StmtString = Statement(dal_Transformation_404);
                rcBool = executeSQL(m_StmtString);
            }
        }
    }
    // TRANSFORMATIONSTATISTIC
    if (SAPDB_TRUE == rcBool)
    {
        rcExists = existsTable (Statement(exist_Transformation_104)); 
        if (SAPDB_FALSE == rcExists)
        {
            m_StmtString = Statement(ddl_Transformation_307);
            rcBool = executeSQL(m_StmtString); 
            if (SAPDB_TRUE == rcBool) {
                m_StmtString = Statement(dal_Transformation_405);
                rcBool = executeSQL(m_StmtString);
            }
        }
    }
    // TRANSFORMATIONMONITOR
    if (SAPDB_TRUE == rcBool)
    {
        rcExists = existsTable (Statement(exist_Transformation_105)); 
        if (SAPDB_FALSE == rcExists)
        {
            m_StmtString = Statement(ddl_Transformation_308);
            rcBool = executeSQL(m_StmtString);
            if (SAPDB_TRUE == rcBool) {
                m_StmtString = Statement(dal_Transformation_406);
                rcBool = executeSQL(m_StmtString);
            }
        }
    }
    // TRANSFORMATIONPACKAGE
    if (SAPDB_TRUE == rcBool)
    {
        rcExists = existsProc(Statement(exist_Transformation_107)); 
        if (SAPDB_FALSE == rcExists)
        {
            StudioOAL_String  create;
            StudioOAL_String  dbaUser;
            _TCHAR            szCreate[2048];
            szCreate[0] = 0;

            dbaUser.Assign(REINTERPRET_CAST(SAPDB_UTF8*, pDBInfo->dbiSYSDBA_Name_ls00.asCharp()));

            sp77sprintf(szCreate, 2048, CONST_CAST(SAPDB_Char*, Statement(ddl_Transformation_302).CharPtr()),
                                        dbaUser.StrPtr(), dbaUser.StrPtr()); 
            create.Assign(REINTERPRET_CAST(SAPDB_UTF8*, szCreate));

            rcBool = executeSQL(create);
            if (SAPDB_TRUE == rcBool) {
                m_StmtString = Statement(dal_Transformation_402);
                rcBool = executeSQL(m_StmtString);
            }
        }
    }
    SQLSetConnectAttr(pDBInfo->dbiOdbcConnection.HDBC(), SQL_PRODUCER, REINTERPRET_CAST(SQLPOINTER, sp1pr_internal_cmd), SQL_IS_POINTER);

    StudioOAL_String CurrentUserCmd;
    CurrentUserCmd  = "CURRENT USER ";
    CurrentUserCmd += "\"";
    CurrentUserCmd += currentUser();
    CurrentUserCmd += "\"";
    rcBool = executeSQL(CurrentUserCmd);

    if (SAPDB_FALSE == rcBool)
    {
        StudioOAL_String NativeErrorAsString = getNativeErrorAsString();
        LoaderMessages::Instance()->ls98PrintMessage(layCatalogExtract_els98, errODBCErrors_els98, NativeErrorAsString.StrPtr());
    }

    return rcBool;
}
/*===========================================================================*
 *    TransformationDef::Statement
 *===========================================================================*/
StudioOAL_String TransformationDef::Statement(const StatementNumbers StatementNumber)
{
    ROUTINE_DBG_MEO00 ("TransformationDef::Statement");

    MetaDataDef*      pMetaData       = getMetaData();
    return pMetaData->Statement(StatementNumber);
}
/*===========================================================================*
 *    TransformationDef::ExistPackage
 *===========================================================================*/
SAPDB_Bool TransformationDef::ExistPackage(StudioOAL_String  packageguid )
{
    ROUTINE_DBG_MEO00 ("TransformationDef::ExistPackage");

    StudioOAL_String  statement;
    StudioOAL_String  record[1];
    SAPDB_Int4 exists = 0;

    if( m_existPackage == SAPDB_FALSE )
    {
        statement  = Statement(sel_Transformation_717);
        statement += "'";
        statement += packageguid;
        statement += "'";

        if ( executeSQL( statement ) )
        {
            readResult( record ) ;
            exists = AtoI( record[0] );
        }
        exists > 0 ? m_existPackage = SAPDB_TRUE : m_existPackage = SAPDB_FALSE ;
    }
    return m_existPackage;
}
/*===========================================================================*
 *    TransformationDef::ExistPackageData
 *===========================================================================*/
SAPDB_Bool TransformationDef::ExistPackageData(StudioOAL_String  packageguid )
{
    ROUTINE_DBG_MEO00 ("TransformationDef::ExistPackageData");

    StudioOAL_String  statement;
    StudioOAL_String  record[1];
    SAPDB_Int4 exists = 0;

    if( m_existPackageData == SAPDB_FALSE )
    {
        statement  = Statement(sel_Transformation_718);
        statement += "'";
        statement += packageguid;
        statement += "'";

        if ( executeSQL( statement ) )
        {
            readResult( record ) ;
            exists = AtoI( record[0] );
        }
        if (exists > 0) {
            m_existPackageData = SAPDB_TRUE;
        }
    }
    return m_existPackageData;
}
/*===========================================================================*
 *    TransformationDef::ImportPackage
 *===========================================================================*/
SAPDB_Bool TransformationDef::ImportPackage(StudioOAL_String& SaveCatalogStmt)
{
    ROUTINE_DBG_MEO00 ("TransformationDef::ImportPackage");
    LoaderMessages *pMsgInstance = LoaderMessages::Instance();

    MetaDataDef  *pMetaDataDef = getMetaData();
    tls00_DBInfo *pDBInfo      = pMetaDataDef->getConnection();
    tls00_TransformationCmd    * pTransformationCmd = getTransformationCmd();
    StudioOAL_String sessionUser, currentUser, sessionSchema, currentSchema ;
    SAPDB_Bool rcBool      = SAPDB_TRUE;
    StudioOAL_String  transformationpackage;
    executeSQL("COMMIT WORK"); 
    //*
    //*   save catalog all|user|schema|table into catalog
    //*
    m_StmtString = Statement(trans_Transformation_316);
    executeSQL(m_StmtString, true); 

    //!!revert
    if ( ptSchema_els00 == m_pTransformationCmd->trPartSpec_ls00.psPartType_ls00 ||
         ptTable_els00  == m_pTransformationCmd->trPartSpec_ls00.psPartType_ls00    )
    {
        sessionUser.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer, getMetaData()->getConnection()->dbiSession_ls00.siUserName_ls00.asCharp()));
        m_owner = currentUser = sessionUser;
        sessionSchema.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer, getMetaData()->getConnection()->dbiSession_ls00.siSchemaName_ls00));
        currentSchema.Assign(REINTERPRET_CAST(Tools_DynamicUTF8String::ConstPointer,pTransformationCmd->trPartSpec_ls00.psTabSchema_ls00.asCharp()));
        m_schemaname = currentSchema;
        switchUserSchema( sessionUser, sessionSchema);
    }
	transformationpackage  = "Import    PACKAGE ";
    transformationpackage += "x'";
    transformationpackage += m_pTransformationCmd->trPackageid_ls00;
    transformationpackage += "'";
    pMsgInstance->ls98PrintMessage(layCatalogExtract_els98, msgCommandExecuting_els98,transformationpackage.StrPtr());
    rcBool = executeSQL(SaveCatalogStmt);
    if ( ptAll_els00 == m_pTransformationCmd->trPartSpec_ls00.psPartType_ls00     && SAPDB_TRUE == rcBool ) 
    {
         m_StmtString = Statement(del_Transformation_604);
         rcBool = executeSQL(m_StmtString);
    }

    //!!revert
    if(( ptSchema_els00 == m_pTransformationCmd->trPartSpec_ls00.psPartType_ls00 ||
         ptTable_els00  == m_pTransformationCmd->trPartSpec_ls00.psPartType_ls00) && SAPDB_TRUE == rcBool )
    {
         m_owner      = sessionUser;
         m_schemaname = sessionSchema;
         switchUserSchema( currentUser, currentSchema);
    }
    //*
    //*  insert into sysloader.transformationmodel select from catalog 
    //*
    if (SAPDB_TRUE == rcBool)
    {
        if (ptTable_els00 == m_pTransformationCmd->trPartSpec_ls00.psPartType_ls00 ) 
        {
            sp77sprintf(m_pTransformationStmt, 1024, Statement(ins_Transformation_501).CharPtr(),
                                                     m_pTransformationCmd->trPackageid_ls00.StrPtr());
        }
        else if (m_pTransformationCmd->trPartSpec_ls00.psPartType_ls00 > ptTable_els00 )
        {
            StudioOAL_String  systemattribute;
            if (false == pDBInfo->dbiSession_ls00.siSystemAttribute_ls00)
                systemattribute = " AND C.TYPE NOT IN ('SYSTEM')";
            else
                systemattribute = " ";
            sp77sprintf(m_pTransformationStmt, 1024, Statement(ins_Transformation_500).CharPtr(), 
                                                     m_pTransformationCmd->trPackageid_ls00.StrPtr(),systemattribute.StrPtr() );
        }
        rcBool = executeSQL(m_pTransformationStmt);
        if (SAPDB_TRUE == rcBool )
        {
            sp77sprintf(m_pTransformationStmt, 1024, Statement(call_Transformation_303).CharPtr(),
                                                     m_pTransformationCmd->trPackageid_ls00.StrPtr());
            rcBool = executeSQL(m_pTransformationStmt);
        }
    }
    if (SAPDB_TRUE == rcBool)
    {
        SAPDB_Bool rcExists = existsTable(Statement(exist_Transformation_106));
        if (SAPDB_TRUE == rcExists) {
            m_StmtString = Statement(drop_Transformation_200);
            rcBool = executeSQL(m_StmtString);
        }
    }
    executeSQL("COMMIT WORK");
    return rcBool;
}
/*===========================================================================*
 *    UserDef::UserDef
 *===========================================================================*/
UserDef::UserDef( sapdbwa_DBC & odbcConnection , MetaDataDef * pMetaData, tls00_TransformationCmd *pTransformationCmd  )
        :MetaDataItem ( odbcConnection , pMetaData , pTransformationCmd)
{
    ROUTINE_DBG_MEO00 ("UserDef::UserDef()");
}
/*===========================================================================*
 *    UserDef::UserDef
 *===========================================================================*/
UserDef::UserDef( sapdbwa_DBC & odbcConnection )
        :MetaDataItem ( odbcConnection )
{
}
/*===========================================================================*
 *    UserDef::~UserDef
 *===========================================================================*/
UserDef::~UserDef()
{
   ROUTINE_DBG_MEO00 ("UserDef::~UserDef()");
}
/*===========================================================================*
 *     UserDef::initResultSet
 *===========================================================================*/
SAPDB_Bool UserDef::initResultSet ( sapdbwa_DBC & odbcConnection )
{
    SAPDB_Bool isOk = SAPDB_FALSE;

    if ( odbcConnection.HDBC() && odbcConnection.HENV() )
    {
        StudioOAL_String  userDef;
        StudioOAL_String  whereClause;
        StudioOAL_String  orderClause;

        userDef = "SELECT OWNER, GROUPNAME, USERNAME, USERMODE, MAXTIMEOUT, COSTWARNING, COSTLIMIT, CONNECTMODE, COMMENT "
                  "FROM DOMAIN.USERS ";
        /*-------------------------------------------------------------------------*
         * Prepare the WHERE clause for the SQL statements.                        *
         *-------------------------------------------------------------------------*/

        if  (ptAll_els00 == m_pTransformationCmd->trPartSpec_ls00.psPartType_ls00)
        {
            whereClause  = " WHERE USERMODE <> 'SYSDBA'"
                           " AND   USERMODE <> 'COLDUSER'"
                           " AND   USERMODE <> 'ADMIN'"
                           " AND   USERNAME <> 'DOMAIN'"
                           " AND   USERNAME <> 'SYS'";
            orderClause  = " ORDER BY OWNER, GROUPNAME, USERNAME ASC ";
       }
       if (ptUser_els00 == m_pTransformationCmd->trPartSpec_ls00.psPartType_ls00)
       {
            whereClause  = "WHERE OWNER = ? ";
            orderClause  = " ORDER BY GROUPNAME, USERNAME ASC ";
       }
       userDef += whereClause;
       userDef += orderClause;
       isOk = SAPDB_TRUE;
    }
    return isOk;
}
/*===========================================================================*
 *    UserDef::readResult
 *===========================================================================*/
SAPDB_Bool UserDef::readResult (StudioOAL_String *record)
{
    ROUTINE_DBG_MEO00 ("UserDef::readResult()");
    SAPDB_Bool isOk = SAPDB_FALSE;

    isOk = MetaDataItem::readResult( record);
    m_owner       = record[0];
    m_groupName   = record[1];
    m_userName    = record[2];
    m_schemaName  = record[3];
    m_userMode    = record[4];
    m_maxTimeOut  = record[5];
    m_costWarning = record[6];
    m_costLimit   = record[7];
    m_connectMode = record[8];
    m_comment     = record[9];
    return isOk;;
}
/*===========================================================================*
 *    UserDef::writeResult
 *===========================================================================*/
SAPDB_Bool UserDef::writeResult( StudioOAL_String *record )
{
    ROUTINE_DBG_MEO00 ("UserDef::writeResult ()");

    StudioOAL_String user;
    StudioOAL_String comment = SEPARATOR;
    // USERGROUP <=> if ( !record[1].Empty() &&  record[2].Empty() )
    // GROUPUSER <=> if ( !record[1].Empty() && !record[2].Empty() )
    // USER      <=> if (  record[1].Empty() && !record[2].Empty() )
    if ( m_pTransformationCmd->trCatalogFile_ls00.fsFormat_ls00.ffoDdl_ls00 == true )
    {
        // <create_usergroup_statement> ::=
        // CREATE USERGROUP <usergroup_name>
        // [<usergroup_mode>]
        // [TIMEOUT   <unsigned_integer>] [COSTWARNING <unsigned_integer>]
        // [COSTLIMIT <unsigned_integer>] [[NOT] EXCLUSIVE]
        // [<usergroup_mode>] ::= RESOURCE | STANDARD

        //<create_user_statement> ::=
        // CREATE USER <user_name> PASSWORD <password> USERGROUP <usergroup_name>

        //<create_user_statement> ::=
        // CREATE USER <user_name> PASSWORD <password>
        //[<user_mode>]
        //[TIMEOUT <unsigned_integer>] [COSTWARNING <unsigned_integer>]
        //[COSTLIMIT <unsigned_integer>] [[NOT] EXCLUSIVE]
        // <user_mode> :: = DBA | RESOURCE | STANDARD

        // GROUPUSER
        if ( !record[1].Empty() && !record[2].Empty() )
        {
             user  = SEPARATOR;

             if ( getMetaData()->getSqlMode() != cpr_kind_internal )
             {
                  user += pszSQLModeString_ls31[cpr_kind_internal];
                  user += SEPARATOR;
             }
             user += "CREATE USER ";
             user += "\"";
             // <user_name>
             user  += record[2];
             user += "\"";
             // <password>
             user  += " PASSWORD ";
             user  += "initial"   ;
             user  += " USERGROUP ";
             user  += record[1];
             writeLine( user );
             // COMMENT ON USER <user_name> IS <comment>
             if ( !record[9].Empty() &&  !getColumn(10)->getColIsNull() )
             {
                  comment += "COMMENT ON USER ";
                  comment += record[2];
                  comment += " IS ";
                  comment += string2SQL(record[9]);

                  writeLine( comment );
             }
             // <grant_user_statement> ::= GRANT USER  <granted_users> [FROM <user_name>] TO <user_name>
        }
        else
        {
            user  = SEPARATOR;

            if ( getMetaData()->getSqlMode()  != cpr_kind_internal )
            {
                  user += pszSQLModeString_ls31[cpr_kind_internal];
                  user += SEPARATOR;
            }
            // USERGROUP
            if ( !record[1].Empty() &&  record[2].Empty() )
            {
                 user += "CREATE USERGROUP ";
                 user += "\"";
                 // <usergroup_name>
                 user += record[1];
                 user += "\"";
            }
            // USER
            if (  record[1].Empty() && !record[2].Empty() )
            {
                 user += "CREATE USER ";
                 user += "\"";
                 // <user_name>
                 user  += record[2];
                 user += "\"";
                 // <password>
                 user  += " PASSWORD ";
                 user  += "initial" ;
            }
            // [<usergroup_mode>] ::= RESOURCE | STANDARD
            // [<user_mode>     ]:: = DBA | RESOURCE | STANDARD
            if ( record[4].Empty() || getColumn(5)->getColIsNull() )
                 user += "" ;
            else
            {
                 user += "  ";
                 user += record[4];
            }
            // [TIMEOUT]
            if (record[5].Empty() || getColumn(6)->getColIsNull() )
                user += "" ;
            else
            {
                user += " TIMEOUT   ";
                user += record[5];
            }
            // [COSTWARNING]
            if ( record[6].Empty() || getColumn(7)->getColIsNull() )
                 user += "" ;
            else
            {
                 user += " COSTWARNING ";
                 user += record[6];
            }
            // [COSTLIMIT]
            if ( record[7].Empty() || getColumn(8)->getColIsNull() )
                 user += "" ;
            else
            {
                 user += " COSTLIMIT ";
                 user += record[7];
            }
            // [[NOT] EXCLUSIVE]
            if ( record[8].Empty() || getColumn(9)->getColIsNull() )
                 user += "" ;
            else
            {
                if ( record[8].Compare("SINGLE") == 0 )
                     user += " EXCLUSIVE " ;
                else
                     user += " NOT EXCLUSIVE ";
            }
            writeLine(user);
            // USERGROUP
            if ( !record[1].Empty() &&  record[2].Empty() )
            {
                 // COMMENT ON USERGROUP <usergroup_name> IS <comment>
                 if ( !record[9].Empty() &&  !getColumn(10)->getColIsNull() )
                 {
                      comment += "COMMENT ON USERGROUP ";
                      comment += record[1];
                      comment += string2SQL(record[9]);
                      comment += " IS ";
                      writeLine(comment  );
                 }
            }
            // USER
            if (  record[1].Empty() && !record[2].Empty() && !getColumn(10)->getColIsNull() )
            {
                  // COMMENT ON USER <user_name> IS <comment>
                  comment += "COMMENT ON USER ";
                  comment += record[2];
                  comment += " IS ";
                  comment += string2SQL(record[9]);
                  writeLine(comment  );
            }
        }
        // <grant_usergroup_statement> ::= GRANT USERGROUP <granted_usergroups> [FROM <user_name>] TO <user_name>
        // <grant_user_statement>      ::= GRANT USER      <granted_users>      [FROM <user_name>] TO <user_name>

    }// if ( m_pTransformationCmd->trCatalogFile_ls00.fsFormat_ls00.ffoDdl_ls00 == true )

    if ( m_pTransformationCmd->trCatalogFile_ls00.fsFormat_ls00.ffoXml_ls00 == true )
    {
        StudioOAL_String schema;
        StudioOAL_String label;
        StudioOAL_String name;
        // USERGROUP
        if ( !record[1].Empty() &&  record[2].Empty() )
        {
            label = "USERGROUP";
            name  = record[1];
        }
        // GROUPUSER
        if ( !record[1].Empty() && !record[2].Empty() )
        {
            label = "GROUPUSER";
            name  = record[1];
        }
        // USER
        if (  record[1].Empty() && !record[2].Empty() )
        {
            label = "USER";
            name  = record[2];
        }
        // SCHEMA
        if (  record[1].Empty() && !record[3].Empty() )
        {
            label = "SCHEMA";
            name  = record[3];
        }
        // Uml:NamespaceOwnedElement
        schema  = "<uml:NamespaceOwnedElement>\n";
        // sdbm:Schema
        schema += "<sdbm:DeployedSchema oim:id=\"_";
        !getColumn(1)->getColIsNull() ? schema += record[0] : schema += "";
        schema += ".";
        if ( !record[1].Empty() )
        {
              schema += record[1];
              schema += ".";
        }
        schema += name;
        schema += "\" oim:supertype=\"dbm:DeployedSchema\"";
        schema += " oim:label=\"";
        schema += label;
        schema += "\" oim:name=\"";
        schema += name;
        schema += "\" sdbm:Owner=\"";
        !getColumn(1)->getColIsNull() ? schema += record[0] : schema += "";
        schema += "\" sdbm:Groupname=\"";
        !getColumn(2)->getColIsNull() ? schema += record[1] : schema += "";
        schema += "\" sdbm:Username=\"";
        !getColumn(3)->getColIsNull() ? schema += record[2] : schema += "";
        schema += "\" sdbm:Usermode=\"";
        !getColumn(4)->getColIsNull() ? schema += record[4] : schema += "";
        schema += "\" sdbm:MaxTimeOut=\"";
        !getColumn(5)->getColIsNull() ? schema += record[5] : schema += "";
        schema += "\" sdbm:Costwarning=\"";
        !getColumn(6)->getColIsNull() ? schema += record[6] : schema += "";
        schema += "\" sdbm:Costlimit=\"";
        !getColumn(7)->getColIsNull() ? schema += record[7] : schema += "";
        schema += "\" sdbm:Connectmode=\"";
        !getColumn(8)->getColIsNull() ? schema += record[8] : schema += "";
        schema += "\" sdbm:Comment=\"";
        !getColumn(9)->getColIsNull() ? schema += record[9] : schema += "";
        schema += "\">\n";
        schema += "<uml:NamespaceOwnedElement>\n";
        writeLine( schema );
    }// if ( m_pTransformationCmd->trCatalogFile_ls00.fsFormat_ls00.ffoXml_ls00 == true )
    return SAPDB_TRUE;
}
/*===========================================================================*
 *    ConnectionDef::ConnectionDef
 *===========================================================================*/
ConnectionDef::ConnectionDef( sapdbwa_DBC & odbcConnection , MetaDataDef * pMetaData, tls00_TransformationCmd *pTransformationCmd )
              :MetaDataItem ( odbcConnection , pMetaData, pTransformationCmd )
{
    ROUTINE_DBG_MEO00 ("ConnectionDef::ConnectionDef()");
}
/*===========================================================================*
 *    ConnectionDef
 *===========================================================================*/
ConnectionDef::ConnectionDef( sapdbwa_DBC & odbcConnection )
              :MetaDataItem ( odbcConnection )
{
    ROUTINE_DBG_MEO00 ("ConnectionDef::ConnectionDef()");
}
/*===========================================================================*
 *    ConnectionDef::ConnectionDef
 *===========================================================================*/
ConnectionDef::~ConnectionDef()
{
    ROUTINE_DBG_MEO00 ("ConnectionDef::~ConnectionDef()");
}
/*===========================================================================*
 *     ConnectionDef::initResultSet
 *===========================================================================*/
SAPDB_Bool ConnectionDef::initResultSet ( sapdbwa_DBC & odbcConnection )
{
    ROUTINE_DBG_MEO00 ("ConnectionDef::initResult ()");
    SAPDB_Bool rcBool = SAPDB_FALSE;
    return rcBool;
}
/*===========================================================================*
 *    ConnectionDef::readResult
 *===========================================================================*/
SAPDB_Bool ConnectionDef::readResult ( StudioOAL_String *record  )
{
    ROUTINE_DBG_MEO00 ("ConnectionDef::readResult ()");
    return MetaDataItem::readResult( record);
}
/*===========================================================================*
 *    UserConnectionDef::UserConnectionDef
 *===========================================================================*/
UserConnectionDef::UserConnectionDef( sapdbwa_DBC & odbcConnection , MetaDataDef * pMetaData, tls00_TransformationCmd *pTransformationCmd  )
           :MetaDataItem ( odbcConnection , pMetaData, pTransformationCmd )
{
    ROUTINE_DBG_MEO00 ("UserConnectionDef::UserConnectionDef()");
}
/*===========================================================================*
 *    UserConnectionDef::UserConnectionDef
 *===========================================================================*/
UserConnectionDef::UserConnectionDef( sapdbwa_DBC & odbcConnection )
           :MetaDataItem ( odbcConnection )
{
    ROUTINE_DBG_MEO00 ("UserConnectionDef::UserConnectionDef()");
}
/*===========================================================================*
 *    UserConnectionDef::UserConnectionDef
 *===========================================================================*/
UserConnectionDef::~UserConnectionDef()
{
    ROUTINE_DBG_MEO00 ("UserConnectionDef::~UserConnectionDef()");
}
/*===========================================================================*
 *     UserConnectionDef::initResultSet
 *===========================================================================*/
SAPDB_Bool UserConnectionDef::initResultSet ( sapdbwa_DBC & odbcConnection )
{
    ROUTINE_DBG_MEO00 ("UserConnectionDef::initResult ()");
    SAPDB_Bool isOk = SAPDB_FALSE;

    if (  odbcConnection.HDBC() && odbcConnection.HENV() )
    {
        isOk = SAPDB_TRUE;
    }
    return isOk;
}
/*===========================================================================*
 *    UserConnectionDef::writeResult
 *===========================================================================*/
SAPDB_Bool UserConnectionDef::writeResult( StudioOAL_String record  )
{
    ROUTINE_DBG_MEO00 ("UserConnectionDef::writeResult ()");
    return MetaDataItem::writeLine ( record);
}
/*===========================================================================*
 *    RoleDef::RoleDef
 *===========================================================================*/
RoleDef::RoleDef( sapdbwa_DBC & odbcConnection , MetaDataDef * pMetaData, tls00_TransformationCmd *pTransformationCmd )
            :MetaDataItem ( odbcConnection , pMetaData, pTransformationCmd )
{
    ROUTINE_DBG_MEO00 ("RoleDef::RoleDef()");
}
/*===========================================================================*
 *    RoleDef ::RoleDef
 *===========================================================================*/
RoleDef::RoleDef( sapdbwa_DBC & odbcConnection )
        :MetaDataItem ( odbcConnection )
{
    ROUTINE_DBG_MEO00 ("RoleDef::RoleDef()");
}
/*===========================================================================*
 *    RoleDef::RoleDef
 *===========================================================================*/
RoleDef::~RoleDef()
{
    ROUTINE_DBG_MEO00 ("RoleDef::~RoleDef()");
}
/*===========================================================================*
 *     RoleDef::initResultSet
 *===========================================================================*/
SAPDB_Bool RoleDef::initResultSet ( sapdbwa_DBC & odbcConnection )
{
    SAPDB_Bool isOk = SAPDB_FALSE;

    if (  odbcConnection.HDBC() && odbcConnection.HENV() )
    {
        // <grantee> ::= PUBLIC | <user_name> | <usergroup_name> | <role_name>
        isOk = SAPDB_TRUE;
    }
    return isOk;
}
/*===========================================================================*
 *    RoleDef::readResult
 *===========================================================================*/
SAPDB_Bool RoleDef::readResult (StudioOAL_String *record)
{
    return MetaDataItem::readResult( record);
}
/*===========================================================================*
 *    RoleDef::writeResult
 *===========================================================================*/
SAPDB_Bool RoleDef::writeResult( StudioOAL_String *record )
{
    ROUTINE_DBG_MEO00 ("RoleDef::writeResult ()");

    if ( m_pTransformationCmd->trCatalogFile_ls00.fsFormat_ls00.ffoDdl_ls00 == true  )
    {
        // <create_role_statement> ::= CREATE ROLE <role_name> [IDENTIFIED BY <password>]
        StudioOAL_String role;
        StudioOAL_String comment;

        role  = SEPARATOR;
        role +="CREATE ROLE ";
        // <role_name>
        role += record[0];
        // [IDENTIFIED BY <password>]
        if ( record[1].Empty() || getColumn(2)->getColIsNull() )
             role += "" ;
        else
        {
             role += " IDENTIFIED BY ";
             // <password>
             role += "initial" ;
        }
        return writeLine(role);
    }
    return SAPDB_TRUE;
}
/*===========================================================================*
 *    SequenceDef::SequenceDef
 *===========================================================================*/
SequenceDef::SequenceDef  ( sapdbwa_DBC & odbcConnection , MetaDataDef * pMetaData, tls00_TransformationCmd *pTransformationCmd   )
            :MetaDataItem ( odbcConnection , pMetaData, pTransformationCmd )
{
    ROUTINE_DBG_MEO00 ("SequenceDef::SequenceDef()");

    m_pSequenceValueDef  = new SequenceValueDef( odbcConnection, pMetaData, pTransformationCmd);
    m_pSequenceValueDef->initResultSet(odbcConnection);

    m_pGrantSequenceDef  = new ResultSet ( odbcConnection );
}
/*===========================================================================*
 *    SequenceDef::SequenceDef
 *===========================================================================*/
SequenceDef::SequenceDef(  sapdbwa_DBC & odbcConnection )
            :MetaDataItem ( odbcConnection )
{
    ROUTINE_DBG_MEO00 ("SequenceDef::SequenceDef()");
}
/*===========================================================================*
 *    SequenceDef::~SequenceDef
 *===========================================================================*/
SequenceDef::~SequenceDef()
{
    ROUTINE_DBG_MEO00 ("SequenceDef::~SequenceDef()");

    if ( m_pSequenceValueDef != NULL )
    {
         delete m_pSequenceValueDef;
         m_pSequenceValueDef = NULL;
    }
    if ( m_pGrantSequenceDef != NULL )
    {
         delete m_pGrantSequenceDef;
         m_pGrantSequenceDef = NULL;
    }
}
/*===========================================================================*
 *     SequenceDef::initResultSet
 *===========================================================================*/
SAPDB_Bool SequenceDef::initResultSet ( sapdbwa_DBC & odbcConnection )
{
    ROUTINE_DBG_MEO00 ("SequenceDef::initResult ()");
    SAPDB_Bool isOk = SAPDB_FALSE;

    if(  odbcConnection.HDBC() && odbcConnection.HENV() )
    {
         isOk = SAPDB_TRUE;
    }
    return isOk;
}
/*===========================================================================*
 *    SequenceDef::readResult
 *===========================================================================*/
SAPDB_Bool SequenceDef::readResult ( StudioOAL_String *record  )
{
    ROUTINE_DBG_MEO00 ("SequenceDef::readResult ()");
    LoaderMessages *pMsgInstance = LoaderMessages::Instance();

    SAPDB_Int4 rc        = errOK_els00;
    SAPDB_Int4 rcCompare = 0;
    SAPDB_Bool rcBool = SAPDB_FALSE;

    StudioOAL_String  NativeErrorAsString;
    StudioOAL_String  nextvalue,maxvalue,minvalue;
    if ( MetaDataItem::readResult( record) )
    {
        rcBool = SAPDB_TRUE;
        if ( record[0].Compare("?") == 0)
             record[0] = "";
        if ( record[1].Compare("?") == 0)
             record[1] = "";
        if ( record[2].Compare("?") == 0)
             record[2] = "";
        if ( record[3].Compare("?") == 0)
             record[3] = "";
        if ( record[4].Compare("?") == 0)
             record[4] = "";
        if ( record[5].Compare("?") == 0)
             record[5] = "";
        if ( record[6].Compare("?") == 0)
             record[6] = "";
        m_owner         = record[0];
        m_sequencename  = record[1];
        m_incrementby   = record[2];
        m_lastnumber    = record[3];
        m_maxvalue      = record[4];
          maxvalue      = record[4];
        m_minvalue      = record[5];
          minvalue      = record[5];
        m_cycleflag     = record[6];
        m_cachesize     = record[7];
        m_orderflag     = record[8];

        StudioOAL_String  statement;
        StudioOAL_String  whereClause;
        StudioOAL_String  record_nextvalue[1];

        statement  = " SELECT ";
        statement += m_owner;
        statement += ".";
        statement += m_sequencename;
        statement += ".NEXTVAL FROM DUAL";
        if ( ! m_pSequenceValueDef->executeSQL( statement )  )
        {
            NativeErrorAsString = m_pSequenceValueDef->getNativeErrorAsString();
            pMsgInstance->ls98PrintMessage(layCatalogExtract_els98, errODBCErrors_els98, NativeErrorAsString.StrPtr());
        }
        while ( m_pSequenceValueDef->readResult( record_nextvalue ) )
        {
             StudioOAL_String   patterns((SAPDB_UTF8*)"-");

             m_nextvalue =  record_nextvalue[0];
               nextvalue =  record_nextvalue[0];

             SAPDB_UInt  start = 0,end_incrementby = 0, end_nextvalue =0, end_minvalue = 0, end_maxvalue = 0 ;
             // ascending sequence
             if ( (end_incrementby = m_incrementby.Find ("-", start) ) == StudioOAL_String::NPos && m_cycleflag.Compare("N") == 0 )
             {
                 // lastnumber negativ and maxvalue positiv or null
                 if ( (end_nextvalue  = m_nextvalue.FindFirstOf (patterns, start) ) != StudioOAL_String::NPos &&
                      (end_maxvalue   = m_maxvalue .FindFirstOf (patterns, start) ) == StudioOAL_String::NPos    )
                 {
                     ;
                 }
                 // lastnumber positiv or null and maxvalue negativ
                 if ( (end_nextvalue  = m_nextvalue.FindFirstOf (patterns, start) ) == StudioOAL_String::NPos &&
                      (end_maxvalue   = m_maxvalue .FindFirstOf (patterns, start) ) != StudioOAL_String::NPos    )
                 {
                     pMsgInstance->ls98PrintMessage(layCatalogExtract_els98, msgSequenceRangeExhausted_els98, m_sequencename.StrPtr());
                 }
                 // lastnumber and maxvalue positiv or null
                 if ( (end_nextvalue  = m_nextvalue.FindFirstOf (patterns, start) ) == StudioOAL_String::NPos &&
                      (end_maxvalue   = m_maxvalue .FindFirstOf (patterns, start) ) == StudioOAL_String::NPos    )
                 {
                     if ( m_nextvalue.Length() >  m_maxvalue.Length() )
                          rcCompare =  1;
                     if ( m_nextvalue.Length() <  m_maxvalue.Length() )
                          rcCompare = -1;
                     if ( m_nextvalue.Length() == m_maxvalue.Length() )
                          rcCompare = m_nextvalue.Compare(m_maxvalue);
                     if ( rcCompare >= 0 )
                          pMsgInstance->ls98PrintMessage(layCatalogExtract_els98, msgSequenceRangeExhausted_els98, m_sequencename.StrPtr());
                 }
                 // lastnumber and maxvalue negativ
                 if ( (end_nextvalue = m_nextvalue.FindFirstOf (patterns, start) ) != StudioOAL_String::NPos &&
                      (end_maxvalue  = m_maxvalue .FindFirstOf (patterns, start) ) != StudioOAL_String::NPos    )
                 {
                     nextvalue.Replace(end_nextvalue,1,"");
                     maxvalue .Replace(end_maxvalue ,1,"");

                     nextvalue = maxvalue;
                     maxvalue  = nextvalue;

                     if ( nextvalue.Length() >  maxvalue.Length() )
                          rcCompare = +1;
                     if ( nextvalue.Length() <  maxvalue.Length() )
                          rcCompare = -1;
                     if ( nextvalue.Length() == maxvalue.Length() )
                          rcCompare = nextvalue.Compare(maxvalue);
                     if ( rcCompare >= 0 )
                          pMsgInstance->ls98PrintMessage(layCatalogExtract_els98, msgSequenceRangeExhausted_els98, m_sequencename.StrPtr());
                 }
             }
             start = 0,end_incrementby = 0, end_nextvalue =0, end_minvalue = 0, end_maxvalue = 0 ;
             // descending sequence
             if ( (end_incrementby = m_incrementby.Find ("-", start) ) != StudioOAL_String::NPos  && m_cycleflag.Compare("N") == 0 )
             {
                 // lastnumber negativ and minvalue positiv or null
                 if ( (end_nextvalue  = m_nextvalue.FindFirstOf (patterns, start) ) != StudioOAL_String::NPos &&
                      (end_minvalue   = m_minvalue .FindFirstOf (patterns, start) ) == StudioOAL_String::NPos    )
                 {
                     pMsgInstance->ls98PrintMessage(layCatalogExtract_els98, msgSequenceRangeExhausted_els98, m_sequencename.StrPtr());
                 }
                 // lastnumber positiv or null and minvalue negativ
                 if ( (end_nextvalue  = m_nextvalue.FindFirstOf (patterns, start) ) == StudioOAL_String::NPos &&
                      (end_minvalue   = m_minvalue .FindFirstOf (patterns, start) ) != StudioOAL_String::NPos    )
                 {
                     ;
                 }
                 // lastnumber and minvalue positiv or null
                 if ( (end_nextvalue  = m_nextvalue.FindFirstOf (patterns, start) ) == StudioOAL_String::NPos &&
                      (end_minvalue   = m_minvalue .FindFirstOf (patterns, start) ) == StudioOAL_String::NPos    )
                 {
                     if ( m_nextvalue.Length() >  m_minvalue.Length() )
                          rcCompare = -1;
                     if ( m_nextvalue.Length() <  m_minvalue.Length() )
                          rcCompare = +1;
                     if ( m_nextvalue.Length() == m_minvalue.Length() )
                          rcCompare = m_nextvalue.Compare(m_minvalue);
                     if ( rcCompare <= 0 )
                          pMsgInstance->ls98PrintMessage(layCatalogExtract_els98, msgSequenceRangeExhausted_els98, m_sequencename.StrPtr());
                 }
                 // lastnumber and maxvalue negativ
                 if ( (end_nextvalue  = m_nextvalue.FindFirstOf (patterns, start) ) != StudioOAL_String::NPos &&
                      (end_minvalue   = m_minvalue .FindFirstOf (patterns, start) ) != StudioOAL_String::NPos    )
                 {
                     nextvalue.Replace(end_nextvalue ,1,"");
                     minvalue .Replace(end_minvalue  ,1,"");

                     nextvalue = minvalue;
                     minvalue  = nextvalue;

                     if ( nextvalue.Length() >  minvalue.Length() )
                          rcCompare = -1;
                     if ( nextvalue.Length() <  minvalue.Length() )
                          rcCompare = +1;
                     if ( nextvalue.Length() == minvalue.Length() )
                          rcCompare = nextvalue.Compare(minvalue);
                     if ( rcCompare <= 0  )
                          pMsgInstance->ls98PrintMessage(layCatalogExtract_els98, msgSequenceRangeExhausted_els98, m_sequencename.StrPtr());
                 }
             }
        }//while ( m_pSequenceValueDef->readResult( record_lastnumber ) )
    }
    return rcBool;
}
/*===========================================================================*
 *    SequenceDef::writeResult
 *===========================================================================*/
SAPDB_Bool SequenceDef::writeResult( StudioOAL_String *record  )
{
    ROUTINE_DBG_MEO00 ("SequenceDef::writeResult ()");

    if ( m_pTransformationCmd->trCatalogFile_ls00.fsFormat_ls00.ffoDdl_ls00 == true  )
    {
        StudioOAL_String sequence;

        // SEQUENCES  : OWNER,SEQUENCE_NAME,INCREMENT_BY,LAST_NUMBER,MAX_VALUE,MIN_VALUE,
        //              CYCLE_FLAG,CACHE_SIZE,ORDER_FLAG,CREATEDATE,CREATETIME,COMMENT

        //<create_sequence_statement> ::= CREATE SEQUENCE [<owner>.]<sequence_name>
        //[INCREMENT BY <integer>] [START WITH <integer>]
        //[MAXVALUE <integer> | NOMAXVALUE] [MINVALUE <integer> | NOMINVALUE]
        //[CYCLE | NOCYCLE]
        //[CACHE <unsigned_integer> | NOCACHE]
        //[ORDER | NOORDER]
        sequence  =  SEPARATOR;
        // Special handling of 'foreign' sqlmodes
        if ( getMetaData()->getSqlMode() != cpr_kind_internal )
        {
             SAPDB_Int4 iSqlMode = getMetaData()->getSqlMode();
             sequence += pszSQLModeString_ls31[iSqlMode];
             sequence += SEPARATOR;
        }
        sequence += "CREATE SEQUENCE  ";
        // <sequence_name>
        if ( m_sequencename.Empty() )
             sequence += "" ;
        else
        {
             sequence += "\"";
             sequence += m_sequencename;
             sequence += "\"";
        }
        // [MINVALUE <integer> | NOMINVALUE]
        if ( m_minvalue.Compare("-99999999999999999999999999999999999999")== 0 )
             sequence += " NOMINVALUE ";
        else
        {
             sequence += " MINVALUE ";
             sequence += m_minvalue;
        }
        // [MAXVALUE <integer>  | NOMAXVALUE]
        if ( m_maxvalue.Compare("99999999999999999999999999999999999999") == 0 )
             sequence += " NOMAXVALUE ";
        else
        {
            sequence += " MAXVALUE " ;
            sequence += m_maxvalue;
        }
        // [START WITH <integer>]
        if ( m_lastnumber.Empty())
             sequence += " " ;
        else
        {
             sequence   += " START WITH ";
             sequence   += m_lastnumber;
        }
        // [INCREMENT BY <integer>]
        if ( m_incrementby.Empty() )
             sequence += " " ;
        else
        {
             sequence += " INCREMENT BY ";
             sequence += m_incrementby;
        }
        // [CYCLE | NOCYCLE]
        if ( m_cycleflag.Empty() )
             sequence += " NOCYCLE ";
        else
        {
             if ( m_cycleflag.Compare("Y") == 0 )
                  sequence += " CYCLE ";
             else
                  sequence += " NOCYCLE ";
        }
        // [CACHE <unsigned_integer> | NOCACHE]
        if ( m_cachesize.Empty() )
             sequence += " ";
        else
        {
             if ( m_cachesize.Compare("0") == 0 )
                 sequence += " NOCACHE ";
             else
             {
                 sequence += " CACHE " ;
                 sequence += m_cachesize;
             }
        }
        // [ORDER | NOORDER]
        if ( m_orderflag.Empty() )
             sequence += " NOORDER ";
        else
        {
            if ( m_orderflag.Compare("Y") == 0 )
                 sequence += " ORDER " ;
            else
                 sequence += " NOORDER ";
        }
        return writeLine(sequence);
    }
    // COMMENT ON not described in reference manual
    return SAPDB_TRUE;
}

/*===========================================================================*
 *    SequenceValueDef::SequenceValueDef
 *===========================================================================*/
SequenceValueDef::SequenceValueDef( sapdbwa_DBC & odbcConnection , MetaDataDef * pMetaData, tls00_TransformationCmd *pTransformationCmd )
                 :MetaDataItem ( odbcConnection , pMetaData, pTransformationCmd)
{
}
/*===========================================================================*
 *    SequenceValueDef::~SequenceDef
 *===========================================================================*/
SequenceValueDef::~SequenceValueDef()
{
    ROUTINE_DBG_MEO00 ("SequenceValueDef::~SequenceValueDef()");
}
/*===========================================================================*
 *    SequenceValueDef::initResult
 *===========================================================================*/
SAPDB_Bool SequenceValueDef::initResultSet ( sapdbwa_DBC & odbcConnection )
{
    ROUTINE_DBG_MEO00 ("SequenceValueDef::initResultSet ()");

    SAPDB_Bool isOk = SAPDB_FALSE;

    if (  odbcConnection.HDBC() && odbcConnection.HENV() )
          isOk = SAPDB_TRUE;
    return isOk;
}
/*===========================================================================*
 *    SequenceValueDef::readResult
 *===========================================================================*/
SAPDB_Bool SequenceValueDef::readResult ( StudioOAL_String *record  )
{
    ROUTINE_DBG_MEO00 ("SequenceValueDef::readResult ()");

    SAPDB_Bool isOk = SAPDB_FALSE;

    if ( MetaDataItem::readResult(record) )
    {
         m_lastnumber = record[0];
         isOk = SAPDB_TRUE;
    }

    return isOk;
}
/*===========================================================================*
 *    SchemaDef::SchemaDef
 *===========================================================================*/
SchemaDef::SchemaDef( sapdbwa_DBC & odbcConnection , MetaDataDef * pMetaData, tls00_TransformationCmd *pTransformationCmd  )
          :MetaDataItem ( odbcConnection , pMetaData , pTransformationCmd)
{
    ROUTINE_DBG_MEO00 ("SchemaDef::SchemaDef()");
}
/*===========================================================================*
 *    SchemaDef::SchemaDef
 *===========================================================================*/
SchemaDef::SchemaDef( sapdbwa_DBC & odbcConnection )
          :MetaDataItem ( odbcConnection )
{
}
/*===========================================================================*
 *    SchemaDef::~SchemaDef
 *===========================================================================*/
SchemaDef::~SchemaDef()
{
   ROUTINE_DBG_MEO00 ("SchemaDef::~SchemaDef()");
}
/*===========================================================================*
 *     SchemaDef::initResultSet
 *===========================================================================*/
SAPDB_Bool SchemaDef::initResultSet ( sapdbwa_DBC & odbcConnection )
{
    SAPDB_Bool isOk = SAPDB_FALSE;

    if ( odbcConnection.HDBC() && odbcConnection.HENV() )
         isOk = SAPDB_TRUE;
    return isOk;
}
/*===========================================================================*
 *    SchemaDef::readResult
 *===========================================================================*/
SAPDB_Bool SchemaDef::readResult (StudioOAL_String *record)
{
    ROUTINE_DBG_MEO00 ("SchemaDef::readResult()");
    SAPDB_Bool isOk = SAPDB_FALSE;

    if (MetaDataItem::readResult( record) )
	{
	    m_owner       = record[1];
		m_schemaname  = record[2];
        m_grantor     = record[3];
        m_grantee     = record[4];
        record[5] == "YES" ? m_createin    = " CREATEIN " : m_createin = "";
        record[6] == "YES" ? m_dropin      = " DROPIN "   : m_dropin   = "";
        record[7] == "YES" ? m_alterin     = " ALTERIN "  : m_alterin  = "";
		isOk = SAPDB_TRUE;
	}
    return isOk;;
}
/*===========================================================================*
 *    SchemaDef::writeResult
 *===========================================================================*/
SAPDB_Bool SchemaDef::writeResult( StudioOAL_String *record )
{
    ROUTINE_DBG_MEO00 ("SchemaDef::writeResult ()");

    TransformationDef *pTransformationDef = getMetaData()->getTransformationModel();
   	StudioOAL_String schema;
	schema  = SEPARATOR;
	schema += "CREATE SCHEMA ";
    schema += "\"";
	schema += record[2];
    schema += "\"";
    
    if ( !m_grantee.Empty() && m_grantor.Compare(m_grantee) != 0 )
    {
         if ( !m_createin.Empty() )
         {
             schema += SEPARATOR_;
             schema += "GRANT ";
             schema += m_createin;
             schema += " ON \"";
             schema += record[2];
             schema += "\" TO \"";
             schema += m_grantee;
             schema += "\"";
         }
         if ( !m_dropin.Empty() )
         {
             schema += SEPARATOR_;
             schema += "GRANT ";
             schema += m_dropin;
             schema += " ON \"";
             schema += record[2];
             schema += "\" TO \"";
             schema += m_grantee;
             schema += "\"";
         }
         if ( !m_alterin.Empty() )
         {
             schema += SEPARATOR_;
             schema += "GRANT ";
             schema += m_alterin;
             schema += " ON \"";
             schema += record[2];
             schema += "\" TO \"";
             schema += m_grantee;
             schema += "\"";
         }
    }
	return writeLine(schema);
}
/*===========================================================================*
 *    TableDef::TableDef
 *===========================================================================*/
TableDef::TableDef( sapdbwa_DBC & odbcConnection , MetaDataDef * pMetaData, tls00_TransformationCmd * pTransformationCmd  )
         :MetaDataItem ( odbcConnection , pMetaData, pTransformationCmd )
{
    ROUTINE_DBG_MEO00 ("TableDef::TableDef()");
    if (pTransformationCmd->trTable_ls00.tsTabOwner_ls00.length() != 0 )
        m_owner = REINTERPRET_CAST(SAPDB_UTF8*, pTransformationCmd->trTable_ls00.tsTabOwner_ls00.asCharp());
    else
        m_owner  = REINTERPRET_CAST(SAPDB_UTF8*, pMetaData->getConnection()->dbiSession_ls00.siUserName_ls00.asCharp());

    if (pTransformationCmd->trTable_ls00.tsTabSchema_ls00.length() != 0 )
        m_schemaname = REINTERPRET_CAST(SAPDB_UTF8*, pTransformationCmd->trTable_ls00.tsTabSchema_ls00.asCharp());
    else 
        m_schemaname = REINTERPRET_CAST(SAPDB_UTF8*, pMetaData->getConnection()->dbiSession_ls00.siSchemaName_ls00);

    if( pTransformationCmd->trTable_ls00.tsTabName_ls00.length() != 0 )
        m_tablename  = REINTERPRET_CAST(SAPDB_UTF8*, pTransformationCmd->trTable_ls00.tsTabName_ls00.asCharp());

    m_sel_Relational_5  = Statement(sel_Relational_5 );
    m_sel_Relational_6  = Statement(sel_Relational_6 );
    m_sel_Relational_7  = Statement(sel_Relational_7 );
    m_sel_Relational_8  = Statement(sel_Relational_8 );
    m_sel_Relational_13 = Statement(sel_Relational_13);
    m_sel_Relational_14 = Statement(sel_Relational_14);

    m_pColumnDefResult     = new ResultSet ( odbcConnection );
    m_pConstraintDefResult = new ResultSet ( odbcConnection );
    m_pForeignKeyDefResult = new ResultSet ( odbcConnection );
    m_pTableAttrDefResult  = new ResultSet ( odbcConnection );

	setCaseSensitive(SAPDB_TRUE);
	setType (ClassTableDef);
}
/*===========================================================================*
 *     TableDef::TableDef()
 *===========================================================================*/
TableDef::TableDef( sapdbwa_DBC & odbcConnection )
         :MetaDataItem ( odbcConnection )
{
    ROUTINE_DBG_MEO00 ("TableDef::TableDef()");

    setCaseSensitive(SAPDB_TRUE);
    m_pColumnDefResult     = new ResultSet ( odbcConnection );
    m_pConstraintDefResult = new ResultSet ( odbcConnection );
    m_pForeignKeyDefResult = new ResultSet ( odbcConnection );
    m_pTableAttrDefResult  = new ResultSet ( odbcConnection );
    setType (ClassTableDef);
}
/*===========================================================================*
 *     TableDef::~TableDef
 *===========================================================================*/
TableDef::~TableDef()
{
    ROUTINE_DBG_MEO00 ("TableDef::~TableDef()");

    if ( m_pColumnDefResult     != NULL )
    {
       delete m_pColumnDefResult;
       m_pColumnDefResult = NULL;
    }
    if ( m_pConstraintDefResult != NULL )
    {
       delete m_pConstraintDefResult;
       m_pConstraintDefResult = NULL;
    }
    if ( m_pForeignKeyDefResult != NULL )
    {
       delete m_pForeignKeyDefResult;
       m_pForeignKeyDefResult = NULL;
    }
    if ( m_pTableAttrDefResult  != NULL )
    {
       delete m_pTableAttrDefResult;
       m_pTableAttrDefResult = NULL;
    }

}
/*===========================================================================*
 *     TableDef::Init
 *===========================================================================*/
SAPDB_Int4
TableDef::Init(const StudioOAL_String &schemaname, const StudioOAL_String &tablename)
{
    SAPDB_Int4 rc = errOK_els00;
    StudioTD_TableDef::reset();

    setSchemaName(schemaname);
    setName      (tablename);
    setCaseSensitive(SAPDB_TRUE);

    m_schemaname  = schemaname;
    m_tablename   = tablename;
    if (SAPDB_TRUE == m_schemaname.Empty())
        rc = existsDuplicateTable();
    return rc;
}
// TableDef::Init()

/*===========================================================================*
 *     TableDef::executeSQL
 *===========================================================================*/
SAPDB_Bool TableDef::executeSQL (StudioOAL_String &statement)
{
    return MetaDataItem::executeSQL ( statement);
}
/*===========================================================================*
 *     TableDef::executeSQL - overloaded
 *===========================================================================*/
SAPDB_Bool TableDef::executeSQL (const char* statement)
{
    // EVERY STRING coming here IS ALREADY UTF-8 ENCODED !!!
    StudioOAL_String stmt = REINTERPRET_CAST(const SAPDB_UTF8*, statement);
    return MetaDataItem::executeSQL(stmt);
}

/*===========================================================================*
 *     TableDef::readResult
 *===========================================================================*/
SAPDB_Bool TableDef::readResult ( StudioOAL_String *record , SAPDB_Bool* bIsNull )
{
    return MetaDataItem::readResult ( record );
}
/*===========================================================================*
 *     TableDef::readTableDefinitionDirect
 *===========================================================================*/
SAPDB_Int4 TableDef::readTableDefinitionDirect(StudioOAL_String &schemaname, StudioOAL_String &tablename)
{
    ROUTINE_DBG_MEO00 ("TableDef::readTableDefinitionDirect()");
    SAPDB_Int4 rc = errOK_els00;

    StudioOAL_String        columnDef;
    StudioOAL_String        attributeDef;
    StudioOAL_String        name;
    StudioOAL_String        record[10];
    StudioOAL_String        s;
    StudioOAL_String        whereClause;
    SAPDB_Bool              bRead = SAPDB_FALSE;
    SAPDB_Bool             *pIsNull = NULL;
    StudioTD_ColumnDef     *col;

    StudioOAL_String         escaped_schemaname = schemaname;
    StudioOAL_String         escaped_tablename  = tablename;

    escapeQuoteChar(escaped_schemaname, "'");
    escapeQuoteChar(escaped_tablename , "'");

   /*-------------------------------------------------------------------------*
    *  Load the column definitions from the database.                         *
    *-------------------------------------------------------------------------*/
    m_pRelationalStmt[0] = 0;
    sp77sprintf(m_pRelationalStmt, 1024, CONST_CAST(SAPDB_Char*     , m_sel_Relational_5.CharPtr()), 
                                         escaped_schemaname.StrPtr(), escaped_tablename.StrPtr() );
    columnDef = REINTERPRET_CAST(SAPDB_UTF8*, m_pRelationalStmt);
    if (!executeSQL( columnDef ))
    {
        rc = errODBCErrors_els98;
    }
    else
    {
        while (readResult(record, pIsNull ) )
        {
            col = addColumn();
            col->setTableOwner(schemaname);
            col->setTableName (tablename);
            col->setName      (record[0]);
            col->setOldName   (record[0]);
            col->setDataType  (ProperCase (record[1]));
            if ((col->dataType() == "Char") || (col->dataType() == "Varchar") || (col->dataType() == "Float"))
            {
                col->setDimension(record[2]);
            }
            if (col->dataType() == "Fixed")
            {
                s  = record[2];
                s += ",";
                s += record[3];
                col->setDimension(s);
            }
            col->setCodeType (record[4]);
            if (record[5] == "KEY")
            {
                col->setKey (SAPDB_TRUE);
                if (!getColumn(6)->getColIsNull() )
                     col->setKeyPos(AtoI(record[6]));
            }
            if (record[7] == "NO")
                col->setNotNull (SAPDB_TRUE);
            if (record[7] == "YES")
                col->setNotNull (SAPDB_FALSE);
            if ( !getColumn(9)->getColIsNull() )
            {
                col->setDefaultValueEx(record[8]);
            }
            if ( !getColumn(10)->getColIsNull() )
                  col->setComment ( record[9] );
                  col->setUnique (SAPDB_FALSE);

            bRead = SAPDB_TRUE;
        }// while (readResult(record, pIsNull ) )
        if (!bRead)
        {
            rc = errODBCErrors_els98;
        }
    }
    /*-------------------------------------------------------------------------*
     * Load the constraint definitions from the database.                      *
     *-------------------------------------------------------------------------*/
    rc = readConstraintDef();
    /*-------------------------------------------------------------------------*
     * Load the foreign key definitions from the database.                     *
     *-------------------------------------------------------------------------*/
    StudioOAL_String        forkeyDef;
    StudioTD_ForeignKeyDef *forkey;

    if (errOK_els00 == rc)
    {
        m_pRelationalStmt[0] = 0;
        sp77sprintf(m_pRelationalStmt, 1024, CONST_CAST(SAPDB_Char*     , m_sel_Relational_7.CharPtr()),
                                             escaped_schemaname.StrPtr(), escaped_tablename.StrPtr() );
        forkeyDef = REINTERPRET_CAST(SAPDB_UTF8*, m_pRelationalStmt);
        if (!executeSQL (forkeyDef))
        {
            rc = errODBCErrors_els98;
        }
        else
        {
            while (readResult(record, pIsNull ))
            {
                if (name != record[0])
                {
                    forkey = addForeignKey();
                    forkey->setName(record[0]);

                    s.Erase();

                    if ( (  ptAll_els00 == m_pTransformationCmd->trPartSpec_ls00.psPartType_ls00) ||
                         ( (ptAll_els00 != m_pTransformationCmd->trPartSpec_ls00.psPartType_ls00) &&
                           (0 != m_schemaname.Compare(record[3])) ) )
                    {
                        s  = record[3];
                        s += ".";
                    }

                    s += record[2];
                    s += " (";
                    forkey->setReference(s);

                    if (record[5].SubStr(0, 6) == "DELETE")
                    {
                        forkey->setOnDelete (record[5].SubStr(6));
                    }
                    else
                    {
                        forkey->setOnDelete(record[5]);
                    }

                    if ( !getColumn(7)->getColIsNull() )
                    {
                        forkey->setComment(record[6]);
                    }
                    name = record[0];
                }

                if (!forkey->referencingColumns().Empty())
                {
                    s  = forkey->referencingColumns();
                    s += ",";
                    forkey->setReferencingColumns (s);
                    s  = forkey->reference().SubStr (0, forkey->reference().Length() - 1);
                    s += ",";
                    forkey->setReference (s);
                }

                s  = forkey->referencingColumns();
                s += record[1];
                forkey->setReferencingColumns (s);
                s  = forkey->reference();
                s += record[4];
                s += ")";
                forkey->setReference (s);
            }
        }
    }
   /*-------------------------------------------------------------------------*
    * Load the misc attributes of the table definition.                       *
    *-------------------------------------------------------------------------*/
    if (errOK_els00 == rc)
    {
        sp77sprintf(m_pRelationalStmt, 1024, CONST_CAST(SAPDB_Char*     , m_sel_Relational_8.CharPtr()),
                                             escaped_schemaname.StrPtr(), escaped_tablename.StrPtr  ());
        attributeDef  = REINTERPRET_CAST(SAPDB_UTF8*, m_pRelationalStmt);
        if (!executeSQL (attributeDef))
        {
            rc = errODBCErrors_els98;
        }
        else
        {
            if (readResult(record, pIsNull ))
            {
                if ( !getColumn(1)->getColIsNull() ) 
                     mComment = record[0];
                if ( !getColumn(2)->getColIsNull() ) 
                {
                    mSampleValue = AtoI (record[1]);
                    mSampleUnit  = "Percent";
                }
                else
                {
                    mSampleValue = AtoI (record[2]);
                    mSampleUnit  = "Rows";
                }
				if (!getColumn(4)->getColIsNull() && !record[3].Empty() )
					mVariableLength = record[3].Compare("NO") == 0  ?  false : true;
                if (schemaname.ToUpper() == "TEMP")
                    mTemporary =  true;
            }
            setAlterType (AlterNoChange);
        }
    }
    return rc;
}
// TableDef::readTableDefinitionDirect()


/*===========================================================================*
 *     TableDef::readDefaultDef
 *===========================================================================*/
SAPDB_Int4 TableDef::readDefaultDef()
{
    ROUTINE_DBG_MEO00 ("TableDef::readDefaultDef()");

    SAPDB_Bool rcBool = SAPDB_TRUE;
    SAPDB_Int4 rc     = errOK_els00;

    StudioOAL_String    record[3];
	StudioOAL_String    defaultDef;
    StudioOAL_String    schema_name;
    SAPDB_Bool         *pIsNull = NULL;
    StudioTD_ColumnDef *pCol    = NULL;

    if (SAPDB_TRUE   == m_schemaname.Empty())
        schema_name   = '%';
    else
        schema_name   = m_schemaname;
    _TCHAR statement[1024];
    statement[0]=0;
    sp77sprintf(statement, 1024, CONST_CAST(SAPDB_Char*, m_sel_Relational_14.CharPtr()), schema_name.StrPtr(), m_tablename.StrPtr());
    defaultDef = REINTERPRET_CAST(SAPDB_UTF8*, statement) ;

    if (!executeSQL(defaultDef))
        rc = errODBCErrors_els98;
    else
    {
        while ( SAPDB_TRUE == (rcBool = readResult(record, pIsNull)) )
        {
            pCol = addColumn();
            pCol->setDataType(ProperCase(record[2]));
            pCol->setColPos(STATIC_CAST(SAPDB_Int2, AtoI(record[1])));

            if ( !getColumn(1)->getColIsNull() )
                pCol->setDefaultValueEx(record[0]);
        }
    }
    return rc;
}
// TableDef::readDefaultDef()

/*===========================================================================*
 *     TableDef::readForeignKeyDef
 *===========================================================================*/
SAPDB_Int4 TableDef::readForeignKeyDef(StudioOAL_String schemaname, StudioOAL_String tablename)
{
    ROUTINE_DBG_MEO00 ("TableDef::readForeignKeyDef()");

    SAPDB_Bool rcBool = SAPDB_TRUE;
    SAPDB_Int4 rc     = errOK_els00;

    StudioTD_ForeignKeyDef *forkey;

    StudioOAL_String        name;
    StudioOAL_String        record[10];
    StudioOAL_String        s;
    StudioOAL_String        schema_name;
    SAPDB_Bool             *pIsNull = NULL;

    StudioTD_TableDef::reset();
    /*-------------------------------------------------------------------------*
     * Prepare the WHERE clause for the SQL statements.                        *
     *-------------------------------------------------------------------------*/
    if (SAPDB_TRUE == schemaname.Empty())
        schema_name = '%';
    else
        schema_name = schemaname;
    /*-------------------------------------------------------------------------*
    * Load the foreign key definitions from the database.                     *
    *-------------------------------------------------------------------------*/
    if (errOK_els00 == rc)
    {
        sp77sprintf(m_pRelationalStmt, 1024, CONST_CAST(SAPDB_Char*, m_sel_Relational_7.CharPtr()), schema_name.StrPtr(),tablename.StrPtr());
        if (!executeSQL (m_pRelationalStmt))
        {
            rc = errODBCErrors_els98;
        }
        else
        {
            while (readResult(record, pIsNull ))
            {
                if (name != record[0])
                {
                    forkey = addForeignKey();
                    forkey->setName(record[0]);

                    s.Erase();
                    s  = record[3];
                    s += ".";

                    s += record[2];
                    s += " (";
                    forkey->setReference(s);

                    if (record[5].SubStr(0, 6) == "DELETE")
                    {
                        forkey->setOnDelete (record[5].SubStr(6));
                    }
                    else
                    {
                        forkey->setOnDelete(record[5]);
                    }

                    if ( !getColumn(7)->getColIsNull() )
                    {
                        forkey->setComment(record[6]);
                    }
                    name = record[0];
                }

                if (!forkey->referencingColumns().Empty())
                {
                    s  = forkey->referencingColumns();
                    s += ",";
                    forkey->setReferencingColumns (s);
                    s  = forkey->reference().SubStr (0, forkey->reference().Length() - 1);
                    s += ",";
                    forkey->setReference (s);
                }

                s  = forkey->referencingColumns();
                s += record[1];
                forkey->setReferencingColumns (s);
                s  = forkey->reference();
                s += record[4];
                s += ")";
                forkey->setReference (s);
            }
        }
    }
    return rc;
}
// TableDef::readForeignKeyDef

/*===========================================================================*
 *     TableDef::writeForeignKeyDef
 *===========================================================================*/
SAPDB_Int4 TableDef::writeForeignKeyDef(StudioOAL_String schemaname, StudioOAL_String tablename)
{
    SAPDB_Int4 rc     = errOK_els00;

    StudioOAL_String    foreignkey;
    SAPDB_Bool          casesensitive = mCaseSensitive;

    StudioTD_ForeignKeyDef  *forkey = NULL;
   /*-----------------------------------------------------------------------*
    * For each item in the foreignkey list generate its part of the         *
    * statement.                                                            *
    *-----------------------------------------------------------------------*/
    if (mForeignKeys.first() != NULL )
    {
        schemaname = "";
        foreignkey += SEPARATOR;
        for (forkey = (StudioTD_ForeignKeyDef *)mForeignKeys.first();
             forkey; 
             forkey = (StudioTD_ForeignKeyDef *)mForeignKeys.next())
        {
             foreignkey += forkey->genAlterStatement ( _T(""), tablename, mCaseSensitive );
        }
        if (!foreignkey.Empty())
            rc = writeLine(foreignkey);
    }
    return rc;
}
// TableDef::writeForeignKeyDef

/*===========================================================================*
 *     TableDef::readConstraintDef
 *===========================================================================*/
SAPDB_Int4 TableDef::readConstraintDef()
{
    ROUTINE_DBG_MEO00 ("TableDef::readConstraintDef()");
    SAPDB_Int4 rc     = errOK_els00;

    StudioOAL_String        record[2];
    StudioOAL_String        schema_name;   
    SAPDB_Bool             *pIsNull = NULL;
    StudioTD_ConstraintDef *pconstr;
    /*-------------------------------------------------------------------------*
     * Load constraint definitions from db.                                    *
     *-------------------------------------------------------------------------*/
    if (SAPDB_TRUE == m_schemaname.Empty())
        schema_name = "%";
    else
        schema_name = m_schemaname;

    m_pRelationalStmt[0] = 0;
    sp77sprintf(m_pRelationalStmt, 1024, CONST_CAST(SAPDB_Char*, m_sel_Relational_6.CharPtr()), schema_name.StrPtr(), m_tablename.StrPtr());

    if (!executeSQL (m_pRelationalStmt))
    {
        rc = errODBCErrors_els98;
    }
    else
    {
        while (readResult(record, pIsNull ))
        {
            pconstr = addConstraint();
            pconstr->setName(record[0]);
            pconstr->setDefinition(record[1]);
            setConstraintName(record[0]);
            setConstraintExpression(record[1]);
        }
    }
    return rc;
}
// TableDef::readConstraintDef()

/*===========================================================================*
 *     TableDef::existsDuplicateTable
 *===========================================================================*/
SAPDB_Int4 TableDef::existsDuplicateTable()
{
    SAPDB_Int4 rc = errOK_els00;
    
    StudioOAL_String  record[1];
	
    m_pRelationalStmt[0] = 0;
    sp77sprintf(m_pRelationalStmt, 1024, CONST_CAST(SAPDB_Char*, m_sel_Relational_13.CharPtr()), m_tablename.StrPtr() );

    if (SAPDB_TRUE == executeSQL(m_pRelationalStmt))
    {
        MetaDataItem::readResult(record);
        if (AtoI( record[0] ) > 1)
        {
            LoaderMessages::Instance()->ls98PrintMessage(layCatalogExtract_els98, errDEAmbiguousTableName_els98);
            rc = errDEAmbiguousTableName_els98;
        }
    }
    else
    {
        rc = errODBCErrors_els98;
    }
    return rc;
}
// TableDef::existsDuplicateTable()


/*===========================================================================*
 *     TableDef::genCreateStatement
 *===========================================================================*/
StudioOAL_String TableDef::genCreateStatement()
{
    StudioOAL_String         sql,s,k,keyTemp;
    StudioTD_String          escapedTableName,escapedColumnName,escapedConstraint,escapedForeignKey;
    SAPDB_Bool               casesensitive = mCaseSensitive;
    SAPDB_Bool               bKey      = false;
    SAPDB_Bool               bFirstKey = false;
    StudioTD_String*         primaryKeys;
    char                     buf[256];

    StudioTD_ColumnDef      *col    = NULL;
    StudioTD_ConstraintDef  *constr = NULL;
    StudioTD_ForeignKeyDef  *forkey = NULL;

    StudioOAL_String::ConstIterator  iter;
    /*-------------------------------------------------------------------------*
     * Check if a table name was given. If not --> return an empty string.     *
     *-------------------------------------------------------------------------*/
    if (mName.Empty())
        return "";
    /*-------------------------------------------------------------------------*
     * Generate the intro of the CREATE TABLE statement.                       *
     *-------------------------------------------------------------------------*/
    sql += "CREATE TABLE \"";
    if (mTemporary)
        sql += "TEMP\".\"";
    
    escapedTableName = CASE(mName);
    escapeQuoteChar(escapedTableName,"\"");
    sql +=          escapedTableName;
    sql += "\""NEW_LINE_LS00"("NEW_LINE_LS00;
    /*-------------------------------------------------------------------------*
     * For each item in the columns list generate its part of the statement.   *
     * In a second run the PRIMARY KEY clause is generated.                    *
     *-------------------------------------------------------------------------*/
    for (col = (StudioTD_ColumnDef *)mColumns.first(); col; col = (StudioTD_ColumnDef *)mColumns.next())
    {
        sql += col->genCreateStatement (mMode, mCaseSensitive);
    }
    /*-------------------------------------------------------------------------*
     * A primary key, constraints and foreign keys are only inserted if the    *
     * mode is not CreateFromQuery (query as a template).                      *
     *-------------------------------------------------------------------------*/
    if (mMode != CreateFromQuery)
    {
        SAPDB_UInt keyCounter = keyCount();
        primaryKeys = new StudioTD_String[keyCounter];

        for (col = (StudioTD_ColumnDef *)mColumns.first(); col; col = (StudioTD_ColumnDef *)mColumns.next())
        {
            //the fist column will set the the key flag
            if (col->key())
            {
                if(col->keyPos() == 1) {
                    bKey      = true;
                    bFirstKey = true;
                }
                keyTemp = (bFirstKey ? "\tPRIMARY KEY (" : "");
                keyTemp  += "\"";
            
                escapedColumnName = CASE(col->name());
                escapeQuoteChar(escapedColumnName,"\"");
            
                keyTemp  += escapedColumnName;
                keyTemp  += "\", ";    
            
                bFirstKey = false;
            
                primaryKeys[col->keyPos()-1] = keyTemp;
            }   
        }
        for(SAPDB_UInt i = 0; i < keyCounter;i++)
            sql += primaryKeys[i];
        
        //delete all elements
        if(primaryKeys != NULL)
           delete[] primaryKeys;
        if (bKey)
        {
            iter = sql.End();
            iter -= 2;
            sql.Erase (iter, sql.End());
            sql += "),"NEW_LINE_LS00;
        }
        /*-----------------------------------------------------------------------*
         * For each item in the constraint list generate its part of the         *
         * statement.                                                            *
         *-----------------------------------------------------------------------*/
        for (constr = (StudioTD_ConstraintDef *)mConstraints.first(); constr; constr = (StudioTD_ConstraintDef *)mConstraints.next())
        {
            // CONSTRAINT <constraint_name> CHECK <search_condition>
            if ( !constr->name().Empty() )
            {
                sql += " CONSTRAINT ";
                sql += "\"";
                sql += constr->name();
                sql += "\"";
                sql += " CHECK ";
                sql += constr->definition();
                sql += ","NEW_LINE_LS00;
            }
            // CONSTRAINT <search_condition>
            else
            {
                sql += " CONSTRAINT ";
                sql += constr->definition();
                sql += ","NEW_LINE_LS00;
            }
        }

    }// if (mMode != CreateFromQuery)
    iter = sql.End();
    iter -= (1 + NEW_LINE_LEN_LS00);
    sql.Erase (iter, sql.End());
    sql += NEW_LINE_LS00")";
    /*-------------------------------------------------------------------------*
     * Insert misc table attributes.                                           *
     *-------------------------------------------------------------------------*/
    if (mTemporary && mIgnoreRollback)
        sql += NEW_LINE_LS00"IGNORE ROLLBACK";

    if ( (mSampleValue != 20000) || (mSampleUnit != "Rows"))
    {
        sprintf (buf, NEW_LINE_LS00"SAMPLE %d %s", mSampleValue, UpperCase(mSampleUnit).StrPtr());
        sql += buf;
    }
	if ( mVariableLength == true )
	{
		sql += NEW_LINE_LS00;
		sql += " NO FIXED LENGTH COLUMN ";
	}
    /*-------------------------------------------------------------------------*
     * If the table is based on a query template the SELECT statement is       *
     * appended in the AS clause.                                              *
     *-------------------------------------------------------------------------*/
    if (mMode == CreateFromQuery)
    {
        sql += NEW_LINE_LS00"AS ";
        sql += mCreateAs;

        if (!mDuplicates.Empty())
        {
            sql += " ";
            sql += UpperCase(mDuplicates);
            sql += " DUPLICATES";
        }
    }
    /*-------------------------------------------------------------------------*
     * Finally, append the comments.                                           *
     *-------------------------------------------------------------------------*/
    if (!mComment.Empty())
    {
        StudioOAL_String  escaped_Comment = mComment;
        escapeQuoteChar  (escaped_Comment, "'");
        sql += NEW_LINE;
        sql += SEPARATOR;
        sql += "COMMENT ON TABLE \"";
        sql += CASE(escapedTableName);
        sql += "\" IS '";
        sql += escaped_Comment;
        sql += "'";
    }
    StudioOAL_String TmpCommentString;
    for (col = (StudioTD_ColumnDef *)mColumns.first(); col; col = (StudioTD_ColumnDef *)mColumns.next())
    {
        if (!col->comment().Empty())
        {
            StudioOAL_String escaped_col_comment = col->comment();
            escapeQuoteChar (escaped_col_comment, "'");
            sql += NEW_LINE;
            sql += SEPARATOR;
            sql += "COMMENT ON COLUMN \"";
            sql += CASE(escapedTableName);
            sql += "\".\"";
            
            escapedColumnName = CASE(col->name());
            escapeQuoteChar(escapedColumnName,"\"");
	        sql += escapedColumnName;
            sql += "\" IS '";
            sql += escaped_col_comment;
            sql += "'";
        }
    }
    return sql;
}

/*===========================================================================*
 *     TableDef::writeTableDefinition
 *===========================================================================*/
SAPDB_Bool TableDef::writeTableDefinition ( )
{
   ROUTINE_DBG_MEO00 ("TableDef::writeTableDefinition ()");

   StudioOAL_String table;
   if ( m_pTransformationCmd->trCatalogFile_ls00.fsFormat_ls00.ffoDdl_ls00 == true ||
        m_pTransformationCmd->trCmdFile_ls00.fsFileName_ls00 != NULL )
   {
        table  = SEPARATOR;
        if ( getMetaData()->getSqlMode() != cpr_kind_internal )
        {
             table += pszSQLModeString_ls31[cpr_kind_internal];
             table += SEPARATOR;
        }
        table += genCreateStatement();
   }
   if ( m_pTransformationCmd->trCatalogFile_ls00.fsFormat_ls00.ffoXml_ls00 == true )
        table += StudioTD_TableDef::genCreateXML();
   return writeLine(table);
}
/*===========================================================================*
 *     TableDef::initResultSet
 *===========================================================================*/
SAPDB_Bool TableDef::initResultSet ( sapdbwa_DBC & odbcConnection )
{
    SAPDB_Bool isOk = SAPDB_FALSE;

    if( odbcConnection.HDBC() && odbcConnection.HENV() )
    {
        StudioOAL_String columnDef;
        StudioOAL_String constraintDef;
        StudioOAL_String foreignKeyDef;
        StudioOAL_String tableAttrDef;
        StudioOAL_String whereClause;

       /*-------------------------------------------------------------------------*
        * Prepare the WHERE clause for the SQL statements.                        *
        *-------------------------------------------------------------------------*/
        whereClause    = "WHERE OWNER = ?  AND TABLENAME = ? ";

        columnDef      = " SELECT A.columnname, A.datatype, A.len, A.dec, A.codetype, A.mode, A.default, A.comment "
                         " FROM DOMAIN.columns A ";
        columnDef     += whereClause;

        constraintDef = "SELECT constraintname, definition "
                        "FROM  DOMAIN.constraints ";
        constraintDef += whereClause;

        foreignKeyDef  = "SELECT fkeyname, columnname, reftablename, refowner, refcolumnname, rule, comment "
                         "FROM   DOMAIN.foreignkeycolumns ";
        foreignKeyDef += whereClause;

        tableAttrDef   = "SELECT comment, sample_percent, sample_rows "
                         "FROM   DOMAIN.tables ";
        tableAttrDef  += whereClause;

        // SQLAllocStmt     SQLPrepare
        // SQLNumParams     SQLDescribeParam SQLBindParameter
        // SQLNumResultCols SQLDescribeCol   SQLBindCol
        if ( !m_pColumnDefResult->init( columnDef ) )
        {
            getMetaData()->getError()->setErrorText(getErrorText() );
            getMetaData()->getError()->isError(isError     () );

            return SAPDB_FALSE;
        }
        if ( !m_pConstraintDefResult->init( constraintDef ) )
        {
             getMetaData()->getError()->setErrorText(getErrorText() );
             getMetaData()->getError()->isError(isError() );
             return SAPDB_FALSE;
        }
        if ( !m_pForeignKeyDefResult->init( foreignKeyDef ) )
        {
             getMetaData()->getError()->setErrorText(getErrorText() );
             getMetaData()->getError()->isError(isError() );
             return SAPDB_FALSE;
        }
        if ( !m_pTableAttrDefResult->init( tableAttrDef  ) )
        {
             getMetaData()->getError()->setErrorText(getErrorText() );
             getMetaData()->getError()->isError(isError() );
             return SAPDB_FALSE;
        }
    }// if ( odbcConnection.HDBC() && odbcConnection.HENV() )

    return SAPDB_TRUE;
}
/*===========================================================================*
 *     TableDef::readTableDefinition                                     *
 *===========================================================================*/
SAPDB_Bool TableDef::readTableDefinition ( StudioOAL_String schemaname, StudioOAL_String tablename )
{
    StudioOAL_String        sql;
    StudioOAL_String        name;
    StudioOAL_String        record[8];
    StudioOAL_String        s;
    SAPDB_Bool              bRead = false;

    StudioTD_ColumnDef     *col;
    StudioTD_ConstraintDef *constr;
    StudioTD_ForeignKeyDef *forkey;

    SAPDB_Bool isOk       = SAPDB_FALSE;

    StudioOAL_String parameters[2];
    parameters[0]   = m_owner;
    parameters[1]   = m_tablename;
    /*-------------------------------------------------------------------------*
     * Load the column definitions from the database.                          *
     *-------------------------------------------------------------------------*/
    m_pColumnDefResult->reset();

    if ( !m_pColumnDefResult->setParamValues( parameters ) )
    {
        getMetaData()->getError()->setErrorText(getErrorText() );
        getMetaData()->getError()->isError     (isError     () );
        return SAPDB_FALSE;
    }
    /*-------------------------------------------------------------------------*
     * Execute the prepared statement                                          *
     *-------------------------------------------------------------------------*/
    // SQLExecute
    if ( !m_pColumnDefResult->execute() )
    {
        MetaDataDef *pMetaData  = getMetaData();
        ErrorHndl    *pError    = pMetaData->getError();

        StudioOAL_String errorText = m_pColumnDefResult->getErrorText();
        SAPDB_Bool      isError   = m_pColumnDefResult->isError     ();

        pError->setErrorText( errorText );
        pError->isError     ( isError   );

        return SAPDB_FALSE;
    }
    // SQlFetch SQLFreeStatement
    while (readColumnDefResult(record))
    {
        col = addColumn();
        col->setTableOwner(schemaname);
        col->setTableName (tablename);
        col->setName      (record[0]);
        col->setOldName   (record[0]);
        col->setDataType  (ProperCase (record[1]));

        if ((col->dataType() == "Char") || (col->dataType() == "Varchar") || (col->dataType() == "Float"))
        {
            col->setDimension (record[2]);
        }
        if (col->dataType() == "Fixed")
        {
            s  = record[2];
            s += ",";
            s += record[3];
            col->setDimension (s);
        }
        col->setCodeType (record[4]);
        if (record[5] == "KEY")
        {
            col->setKey (SAPDB_TRUE);
        }
        if (record[5] == "MAN")
        {
            col->setNotNull (SAPDB_TRUE);
        }
        if ( !m_pColumnDefResult->getColumn(7)->getColIsNull() )
        {
            col->setDefaultValueEx(record[6].SubStr(8));
        }
        if ( !m_pColumnDefResult->getColumn(8)->getColIsNull() )
        {
            col->setComment (record[7]);
        }
        col->setUnique (SAPDB_FALSE);
        bRead = SAPDB_TRUE;
    }
    if (!bRead)
    {
        return isOk;
    }
    /*-------------------------------------------------------------------------*
     * Load the constraint definitions from the database.                      *
     *-------------------------------------------------------------------------*/
    m_pConstraintDefResult->reset();

    if ( !m_pConstraintDefResult->setParamValues( parameters ) )
    {
        getMetaData()->getError()->setErrorText(getErrorText() );
        getMetaData()->getError()->isError     (isError     () );
        return SAPDB_FALSE;
    }
    /*-------------------------------------------------------------------------*
     * Execute the prepared statement                                          *
     *-------------------------------------------------------------------------*/
    // SQLExecute
    if (!m_pConstraintDefResult->execute() )
    {
        MetaDataDef *pMetaData  = getMetaData();
        ErrorHndl    *pError    = pMetaData->getError();

        StudioOAL_String errorText = m_pConstraintDefResult->getErrorText();
        SAPDB_Bool      isError   = m_pConstraintDefResult->isError     ();

        pError->setErrorText( errorText );
        pError->isError     ( isError   );

        return SAPDB_FALSE;
    }
    // SQlFetch SQLFreeStatement
    while (readConstraintDefResult(record))
    {
        constr = addConstraint( );
        constr->setName         (record[0]);
        constr->setDefinition   (record[1]);
        setConstraintName       (record[0]);
        setConstraintExpression (record[1]);
        bRead = true;
    }
    /*-------------------------------------------------------------------------*
     * Load the foreign key definitions from the database.                     *
     *-------------------------------------------------------------------------*/
    m_pForeignKeyDefResult->reset();

    if ( !m_pForeignKeyDefResult->setParamValues( parameters ) )
    {
        getMetaData()->getError()->setErrorText(getErrorText() );
        getMetaData()->getError()->isError     (isError     () );
        return SAPDB_FALSE;
    }
    /*-------------------------------------------------------------------------*
     * Execute the prepared statement                                          *
     *-------------------------------------------------------------------------*/
    // SQLExecute
    if (!m_pForeignKeyDefResult->execute() )
    {
        MetaDataDef *pMetaData  = getMetaData();
        ErrorHndl *pError     = pMetaData->getError();

        StudioOAL_String errorText = m_pForeignKeyDefResult->getErrorText();
        SAPDB_Bool      isError   = m_pForeignKeyDefResult->isError     ();

        pError->setErrorText( errorText );
        pError->isError     ( isError   );

        return SAPDB_FALSE;
    }
    // SQlFetch SQLFreeStatement
    while (readForeignKeyDefResult(record))
    {
        bRead = true;

        if (name != record[0])
        {
            forkey = addForeignKey( );
            forkey->setName       (record[0]);
            s  = record[3];
            s += ".";
            s += record[2];
            s += " (";
            forkey->setReference (s);

            if (record[5].SubStr (0, 6) == "DELETE")
                forkey->setOnDelete (record[5].SubStr (6));
            else
                forkey->setOnDelete (record[5]);
            if ( !m_pForeignKeyDefResult->getColumn(7)->getColIsNull() ) 
                forkey->setComment (record[6]);
            name = record[0];
        }
        if (!forkey->referencingColumns().Empty())
        {
            s  = forkey->referencingColumns();
            s += ",";
            forkey->setReferencingColumns (s);
            s  = forkey->reference().SubStr (0, forkey->reference().Length() - 1);
            s += ",";
            forkey->setReference (s);
        }
        s  = forkey->referencingColumns();
        s += record[1];
        forkey->setReferencingColumns (s);
        s  = forkey->reference();
        s += record[4];
        s += ")";
        forkey->setReference (s);
    }
    /*-------------------------------------------------------------------------*
     * Load the misc attributes of the table definition.                       *
     *-------------------------------------------------------------------------*/
    m_pTableAttrDefResult->reset();
    if ( !m_pTableAttrDefResult->setParamValues( parameters ) )
    {
        getMetaData()->getError()->setErrorText(getErrorText() );
        getMetaData()->getError()->isError     (isError     () );
        return SAPDB_FALSE;
    }
    /*-------------------------------------------------------------------------*
     * Execute the prepared statement                                          *
     *-------------------------------------------------------------------------*/
    // SQLExecute
    if (!m_pTableAttrDefResult->execute() )
    {
        MetaDataDef *pMetaData  = getMetaData();
        ErrorHndl    *pError    = pMetaData->getError();

        StudioOAL_String errorText = m_pTableAttrDefResult->getErrorText();
        SAPDB_Bool      isError   = m_pTableAttrDefResult->isError     ();

        pError->setErrorText( errorText );
        pError->isError     ( isError   );

        return SAPDB_FALSE;
    }
    // SQlFetch SQLFreeStatement
    while (readTableAttrDefResult(record))
    {
        bRead = true;

        if ( !m_pTableAttrDefResult->getColumn(1)->getColIsNull() ) 
            mComment = record[0];
        if ( !m_pTableAttrDefResult->getColumn(2)->getColIsNull() ) 
        {
            mSampleValue = AtoI (record[1]);
            mSampleUnit  = "Percent";
        }
        else
        {
            mSampleValue = AtoI (record[2]);
            mSampleUnit  = "Rows";
        }
        if (schemaname.ToUpper() == "TEMP")
            mTemporary  = true;
	    if (!record[3].Empty() && !getColumn(4)->getColIsNull() )
			mVariableLength = record[3].Compare("NO") == 0  ?  false : true;
    }
    setAlterType (AlterNoChange);

    isOk = SAPDB_TRUE;

    return isOk;
}
/*===========================================================================*
 *     TableDef::readColumnDefResult
 *===========================================================================*/
SAPDB_Bool TableDef::readColumnDefResult(StudioOAL_String *record)
{
    SAPDB_Bool rc = SAPDB_TRUE;

    /*-------------------------------------------------------------------------*
     * Fetch the next record.                                                  *
     *-------------------------------------------------------------------------*/
    // SQLFetch
    rc = m_pColumnDefResult->fetch();
    if (SAPDB_TRUE == rc)
    {
        /*-------------------------------------------------------------------------*
         * If there are no more records, reading the result is finished.           *
         *-------------------------------------------------------------------------*/
        rc = m_pColumnDefResult->noRows();
        if (SAPDB_TRUE == rc)
        {
            /*-------------------------------------------------------------------------*
             * Read all columns of the fetched record.                                 *
             *-------------------------------------------------------------------------*/
            SAPDB_Int2       i;
            StudioOAL_String s;

            for (i = 0; i < m_pColumnDefResult->getColCount(); i++)
            {
                m_pColumnDefResult->bindColumnDataAsString (i + 1, s);
                record[i] = s;
            }
        }
    }
    if (SAPDB_FALSE == rc)
    {
        m_pColumnDefResult->closeCursor();
    }

    return rc;
}
/*===========================================================================*
 *     TableDef::readConstraintDefResult
 *===========================================================================*/
SAPDB_Bool TableDef::readConstraintDefResult(StudioOAL_String *record)
{
    SAPDB_Bool rc = SAPDB_TRUE;

    /*-------------------------------------------------------------------------*
     * Fetch the next record.                                                  *
     *-------------------------------------------------------------------------*/
    // SQLFetch
    rc = m_pConstraintDefResult->fetch();
    if (SAPDB_TRUE == rc)
    {
        /*-------------------------------------------------------------------------*
         * If there are no more records, reading the result is finished.           *
         *-------------------------------------------------------------------------*/
        rc = m_pConstraintDefResult->noRows();
        if (SAPDB_TRUE == rc)
        {
            /*-------------------------------------------------------------------------*
             * Read all columns of the fetched record.                                 *
             *-------------------------------------------------------------------------*/
            SAPDB_Int2       i = 0;
            StudioOAL_String s;

            for (i; i < m_pConstraintDefResult->getColCount(); i++)
            {
                m_pConstraintDefResult->bindColumnDataAsString (i + 1, s);
                record[i] = s;
            }
        }
    }
    if (SAPDB_FALSE == rc)
    {
        m_pConstraintDefResult->closeCursor();
    }

    return rc;
}
/*===========================================================================*
 *     TableDef::readForeignKeyDefResult
 *===========================================================================*/
SAPDB_Bool TableDef::readForeignKeyDefResult(StudioOAL_String *record)
{
    SAPDB_Bool rc = SAPDB_TRUE;

    /*-------------------------------------------------------------------------*
     * Fetch the next record.                                                  *
     *-------------------------------------------------------------------------*/
    // SQLFetch
    rc = m_pForeignKeyDefResult->fetch();
    if (SAPDB_TRUE == rc)
    {
        /*-------------------------------------------------------------------------*
         * If there are no more records, reading the result is finished.           *
         *-------------------------------------------------------------------------*/
        rc = m_pForeignKeyDefResult->noRows();
        if (SAPDB_TRUE == rc)
        {
            /*-------------------------------------------------------------------------*
             * Read all columns of the fetched record.                                 *
             *-------------------------------------------------------------------------*/
            SAPDB_Int2       i = 0;
            StudioOAL_String s;

            for (i; i < m_pForeignKeyDefResult->getColCount(); i++)
            {
                m_pForeignKeyDefResult->bindColumnDataAsString (i + 1, s);
                record[i] = s;
            }
        }
    }
    if (SAPDB_FALSE == rc)
    {
        m_pForeignKeyDefResult->closeCursor();
    }

    return rc;
}
/*===========================================================================*
 *     TableDef::readTableAttrDefResult
 *===========================================================================*/
SAPDB_Bool TableDef::readTableAttrDefResult(StudioOAL_String *record)
{
    SAPDB_Bool rc = SAPDB_TRUE;

    /*-------------------------------------------------------------------------*
     * Fetch the next record.                                                  *
     *-------------------------------------------------------------------------*/
    // SQLFetch
    rc = m_pTableAttrDefResult->fetch();
    if (SAPDB_TRUE == rc)
    {
        /*-------------------------------------------------------------------------*
         * If there are no more records, reading the result is finished.           *
         *-------------------------------------------------------------------------*/
        rc = m_pTableAttrDefResult->noRows();
        if (SAPDB_TRUE == rc)
        {
            /*-------------------------------------------------------------------------*
             * Read all columns of the fetched record.                                 *
             *-------------------------------------------------------------------------*/
            SAPDB_Int2       i = 0;
            StudioOAL_String s;

            for (i; i < m_pTableAttrDefResult->getColCount(); i++)
            {
                m_pTableAttrDefResult->bindColumnDataAsString (i + 1, s);
                record[i] = s;
            }
        }
    }
    if (SAPDB_FALSE == rc)
    {
        m_pTableAttrDefResult->closeCursor();
    }

    return rc;
}
/*===========================================================================*
 *    ViewDef::ViewDef
 *===========================================================================*/
ViewDef::ViewDef( sapdbwa_DBC & odbcConnection , MetaDataDef * pMetaData, tls00_TransformationCmd * pTransformationCmd )
        :MetaDataItem ( odbcConnection , pMetaData, pTransformationCmd )
{
    m_pViewDefResult = new ResultSet ( odbcConnection );
}
/*===========================================================================*
 *     ViewDef::ViewDef()
 *===========================================================================*/
ViewDef::ViewDef( sapdbwa_DBC & odbcConnection )
        :MetaDataItem ( odbcConnection )
{
    m_pViewDefResult = new ResultSet ( odbcConnection );
}
/*===========================================================================*
 *     ViewDef::~ViewDef()
 *===========================================================================*/
ViewDef::~ViewDef()
{
    if ( m_pViewDefResult != NULL )
    {
         delete m_pViewDefResult;
         m_pViewDefResult = NULL;
    }
}
/*===========================================================================*
 *     ViewDef::initResultSet
 *===========================================================================*/
SAPDB_Bool ViewDef::initResultSet( sapdbwa_DBC & odbcConnection )
{
    SAPDB_Bool isOk = SAPDB_FALSE;

    if (  odbcConnection.HDBC() && odbcConnection.HENV() )
    {
        StudioOAL_String  viewDef = "";
        // SQLAllocStmt     SQLPrepare
        // SQLNumParams     SQLDescribeParam SQLBindParameter
        // SQLNumResultCols SQLDescribeCol   SQLBindCol
        isOk = m_pViewDefResult->init(viewDef);
        if (SAPDB_FALSE == isOk)
        {
            getMetaData()->getError()->setErrorText(getErrorText());
            getMetaData()->getError()->isError(isError());
        }
    }
    return isOk;
}
/*===========================================================================*
 *     ViewDef::readViewDefinition
 *===========================================================================*/
SAPDB_Bool ViewDef::readViewDefinition( StudioOAL_String schemaname, StudioOAL_String viewname )
{
   ROUTINE_DBG_MEO00 ("ViewDef::readViewDefinition()");

   // VIEWS    : OWNER,VIEWNAME,PRIVILEGES,TYPE,CREATEDATE,CREATETIME,UPDSTATDATE,UPDSTATTIME,ALTERDATE,ALTERTIME,UNLOADED,COMMENT
   // VIEWDEFS : OWNER,VIEWNAME,LEN,DEFINITION

   SAPDB_Bool isOk = SAPDB_FALSE;

   StudioOAL_String  viewDef[2];
   StudioOAL_String  record[3];

   viewDef[0] = schemaname;
   viewDef[1] = viewname;

  /*-------------------------------------------------------------------------*
   * Load the index definitions from the database.                           *
   *-------------------------------------------------------------------------*/
   m_pViewDefResult->reset();

   if ( !m_pViewDefResult->setParamValues( viewDef ) )
   {
        getMetaData()->getError()->setErrorText(getErrorText() );
        getMetaData()->getError()->isError     (isError     () );
        return SAPDB_FALSE;
   }
   /*-------------------------------------------------------------------------*
    * Execute the prepared statement                                          *
    *-------------------------------------------------------------------------*/
   // SQLExecute
   if ( !m_pViewDefResult->execute() )
   {
        StudioOAL_String errorText = m_pViewDefResult->getErrorText();
        SAPDB_Bool       isError   = m_pViewDefResult->isError     ();

        getMetaData()->getError()->setErrorText( errorText );
        getMetaData()->getError()->isError     ( isError   );

        return SAPDB_FALSE;
   }
   // SQlFetch SQLFreeStatement
   while ( readViewDefResult (record) )
   {
         m_viewName       = record[0];
         m_owner          = record[1];
         m_viewDefinition = record[2];

         isOk = SAPDB_TRUE;
   }

   return isOk;

}
/*===========================================================================*
 *     ViewDef::writeViewDefinition
 *===========================================================================*/
SAPDB_Bool ViewDef::writeViewDefinition()
{
   ROUTINE_DBG_MEO00 ("ViewDef::writeViewDefinition");

   StudioOAL_String schemaview;

   if ( m_pTransformationCmd->trCatalogFile_ls00.fsFormat_ls00.ffoDdl_ls00 == true  )
   {
        schemaview  = SEPARATOR;

        if ( getMetaData()->getSqlMode() != cpr_kind_internal )
        {
             SAPDB_Int4 iSqlMode = getMetaData()->getSqlMode();
             schemaview += pszSQLModeString_ls31[iSqlMode];
             schemaview += SEPARATOR;
        }
        schemaview += m_viewDefinition;
   }

   if ( m_pTransformationCmd->trCatalogFile_ls00.fsFormat_ls00.ffoXml_ls00 == true )
        schemaview += genCreateXML();

   return writeLine( schemaview );

}
/*===========================================================================*
 *     ViewDef::readViewDefResult
 *===========================================================================*/
SAPDB_Bool ViewDef::readViewDefResult( StudioOAL_String *record )
{
    SAPDB_Bool rc = SAPDB_TRUE;
    /*-------------------------------------------------------------------------*
     * Fetch the next record.                                                  *
     *-------------------------------------------------------------------------*/
    // SQLFetch
    rc = m_pViewDefResult->fetch();
    if (SAPDB_TRUE == rc)
    {
        /*-------------------------------------------------------------------------*
         * If there are no more records, reading the result is finished.           *
         *-------------------------------------------------------------------------*/
        rc = m_pViewDefResult->noRows();
        if (SAPDB_TRUE == rc)
        {
            /*-------------------------------------------------------------------------*
             * Read all columns of the fetched record.                                 *
             *-------------------------------------------------------------------------*/
            SAPDB_Int2       i = 0;
            StudioOAL_String s;

            for (i; i < m_pViewDefResult->getColCount(); i++)
            {
                m_pViewDefResult->bindColumnDataAsString (i + 1, s);
                record[i] = s;
            }
        }
    }
    if (SAPDB_FALSE == rc)
    {
        m_pViewDefResult->closeCursor();
    }
    return rc;
}
/*===========================================================================*
 *     ViewDef::readResult()
 *===========================================================================*/
SAPDB_Bool ViewDef::readResult( StudioOAL_String *record )
{
    SAPDB_Bool isOk = SAPDB_FALSE;

    isOk = MetaDataItem::readResult( record);

    if ( isOk )
    {
         m_viewName       = record[0];
         m_owner          = record[1];
         // <create_view_statement> ::= CREATE [OR REPLACE] VIEW <table_name> [(<alias_name>,...)] AS <query_expression> [WITH CHECK OPTION]
         m_viewDefinition = record[2];
    }

    return isOk;
}
/*===========================================================================*
 *     ViewDef::writeResult()
 *===========================================================================*/
SAPDB_Bool ViewDef::writeResult( StudioOAL_String *record )
{
    StudioOAL_String schemaview;
    if ( m_pTransformationCmd->trCatalogFile_ls00.fsFormat_ls00.ffoDdl_ls00 == true  )
    {
        schemaview  = SEPARATOR;
        if ( getMetaData()->getSqlMode() != cpr_kind_internal )
        {
            SAPDB_Int4 iSqlMode = getMetaData()->getSqlMode();
            schemaview += pszSQLModeString_ls31[iSqlMode];
            schemaview += SEPARATOR;
        }
        schemaview += m_viewDefinition;
        //!! ToDo COMMENT ON -> PTS 1109219
        //definition  = record[3].SubStr(11,record[3].Length());
        //definition.Trim();
    }
    if ( m_pTransformationCmd->trCatalogFile_ls00.fsFormat_ls00.ffoXml_ls00 == true )
         schemaview += genCreateXML();
    return writeLine( schemaview );
}
/*===========================================================================*
 *     ViewDef::genCreateXML()
 *===========================================================================*/
StudioOAL_String ViewDef::genCreateXML()
{
   // <dbm:SchemaViews>
   StudioOAL_String  as   ((SAPDB_UTF8*)" AS ");
   StudioOAL_String  check((SAPDB_UTF8*)" WITH CHECK OPTION ");
   StudioOAL_String  schemaView;
   StudioOAL_String  viewQuery;
   StudioOAL_String  checkOption = "";
   viewQuery += m_viewDefinition;

   SAPDB_UInt  start = 0, end ;

   if ( (end = viewQuery.Find (as, start) ) != StudioOAL_String::NPos )
   {
       viewQuery = viewQuery.SubStr( end + as.Length(),viewQuery.Length() );
       start = end +1 ;
       if ( (end = viewQuery.Find (check, start) ) != StudioOAL_String::NPos )
       {
             viewQuery   = viewQuery.SubStr( 0, end );
             checkOption = check;
       }
   }
   schemaView  = "<dbm:DeployedView oim:id=\"_";
   schemaView += m_owner;
   schemaView += ".";
   schemaView += m_viewName;
   schemaView += "\" oim:supertype = \"Query\"";
   schemaView += "   oim:seqno     = \"";
   schemaView += "\" dbm:Body = \"";
   schemaView += string2XML ( viewQuery );
   schemaView += "\" dbm:CheckOption = \"";
   schemaView += checkOption;
   schemaView += "\" dbm:description = \"";
   schemaView += string2XML ( m_viewDefinition );
   schemaView += "\" dbm:isSpecification = \"1";
   schemaView += "\" dbm:comments = \"";
   schemaView += "\" dbm:EstimatedRows = \"";
   schemaView += "\" dbm:EstimatedSize = \"";
   schemaView += "\"/>\n";
   return schemaView;
}
/*===========================================================================*
 *    SynonymDef::SynonymDef
 *===========================================================================*/
SynonymDef::SynonymDef( sapdbwa_DBC & odbcConnection , MetaDataDef * pMetaData, tls00_TransformationCmd * pTransformationCmd )
           :MetaDataItem ( odbcConnection , pMetaData, pTransformationCmd )
{
    m_pSynonymDefResult = new ResultSet ( odbcConnection );
}
/*===========================================================================*
 *     SynonymDef::SynonymDef()
 *===========================================================================*/
SynonymDef::SynonymDef( sapdbwa_DBC & odbcConnection )
           :MetaDataItem        ( odbcConnection )
{
    m_pSynonymDefResult = new ResultSet  ( odbcConnection );
}
/*===========================================================================*
 *     SynonymDef::~SynonymDef()
 *===========================================================================*/
SynonymDef::~SynonymDef()
{
    if ( m_pSynonymDefResult != NULL )
    {
        delete m_pSynonymDefResult;
        m_pSynonymDefResult = NULL;
    }
}
/*===========================================================================*
 *     SynonymDef::initResultSet
 *===========================================================================*/
SAPDB_Bool SynonymDef::initResultSet ( sapdbwa_DBC & odbcConnection )
{
    SAPDB_Bool isOk = SAPDB_FALSE;

    if (  odbcConnection.HDBC() && odbcConnection.HENV() )
    {
        StudioOAL_String    synonymDef = "";
        // SQLAllocStmt     SQLPrepare
        // SQLNumParams     SQLDescribeParam SQLBindParameter
        // SQLNumResultCols SQLDescribeCol   SQLBindCol
        isOk = m_pSynonymDefResult->init(synonymDef);
        if (SAPDB_FALSE == isOk)
        {
            getMetaData()->getError()->setErrorText(getErrorText());
            getMetaData()->getError()->isError(isError());
        }
    }
    return isOk;
}
/*===========================================================================*
 *     SynonymDef::readSynonymDefinition
 *===========================================================================*/
SAPDB_Bool SynonymDef::readSynonymDefinition( StudioOAL_String schemaname, StudioOAL_String synonymname )
{
    ROUTINE_DBG_MEO00 ("SynonymDef::readSynonymDefinition");

    SAPDB_Bool isOk = SAPDB_FALSE;

    StudioOAL_String  synonymDef[2];
    StudioOAL_String  record[6];

    synonymDef[0] = schemaname;
    synonymDef[1] = synonymname;
    /*-------------------------------------------------------------------------*
     * Load the synonym definitions from the database.                         *
     *-------------------------------------------------------------------------*/
    if ( !m_pSynonymDefResult->setParamValues( synonymDef ) )
    {
        getMetaData()->getError()->setErrorText(getErrorText() );
        getMetaData()->getError()->isError     (isError     () );
        return SAPDB_FALSE;
    }
    /*-------------------------------------------------------------------------*
     * Execute the prepared statement                                          *
     *-------------------------------------------------------------------------*/
    // SQLExecute
    if ( !m_pSynonymDefResult->execute() )
    {
        StudioOAL_String errorText = m_pSynonymDefResult->getErrorText();
        SAPDB_Bool      isError   = m_pSynonymDefResult->isError     ();
        getMetaData()->getError()->setErrorText( errorText );
        getMetaData()->getError()->isError     ( isError   );
        return SAPDB_FALSE;
    }
    // SQlFetch SQLFreeStatement
    while ( readSynonymDefResult (record) )
    {
        m_owner        = record[0];
        m_synonymname  = record[1];
        m_public       = record[2];
        m_tableowner   = record[3];
        m_tablename    = record[4];
        m_comment      = record[5];

        isOk = SAPDB_TRUE;
    }
    return isOk;
}
/*===========================================================================*
 *     SynonymDef::writeSynonymDefinition
 *===========================================================================*/
SAPDB_Bool SynonymDef::writeSynonymDefinition()
{
   ROUTINE_DBG_MEO00 ("SynonymDef::writeSynonymDefinition");

   StudioOAL_String tableSynonym;

   if ( m_pTransformationCmd->trCatalogFile_ls00.fsFormat_ls00.ffoXml_ls00 == true )
        tableSynonym += genCreateXML();

   if ( m_pTransformationCmd->trCatalogFile_ls00.fsFormat_ls00.ffoDdl_ls00 == true  )
   {
        tableSynonym  = SEPARATOR;
        // Special handling of 'foreign' sqlmodes
        if ( getMetaData()->getSqlMode() != cpr_kind_internal )
        {
             SAPDB_Int4 iSqlMode = getMetaData()->getSqlMode();
             tableSynonym += pszSQLModeString_ls31[iSqlMode];
             tableSynonym += SEPARATOR;
        }
        tableSynonym += genCreateDDL();
   }
   return writeLine( tableSynonym );
}
/*===========================================================================*
 *     SynonymDef::readSynonymDefResult
 *===========================================================================*/
SAPDB_Bool SynonymDef::readSynonymDefResult( StudioOAL_String *record )
{
   SAPDB_Int2       i;
   StudioOAL_String  s;

  /*-------------------------------------------------------------------------*
   * Fetch the next record.                                                  *
   *-------------------------------------------------------------------------*/
  // SQLFetch
  if (!m_pSynonymDefResult->fetch())
  {
      m_pSynonymDefResult->closeCursor();
      return SAPDB_FALSE;
  }

  /*-------------------------------------------------------------------------*
   * If there are no more records, reading the result is finished.           *
   *-------------------------------------------------------------------------*/
  if (m_pSynonymDefResult->noRows())
  {
     m_pSynonymDefResult->closeCursor();
     return SAPDB_FALSE;
  }

  /*-------------------------------------------------------------------------*
   * Read all columns of the fetched record.                                 *
   *-------------------------------------------------------------------------*/
  for (i = 0; i < m_pSynonymDefResult->getColCount(); i++)
  {
    m_pSynonymDefResult->bindColumnDataAsString (i + 1, s);
    record[i] = s;
  }

  return SAPDB_TRUE;
}
/*===========================================================================*
 *     SynonymDef::readResult()
 *===========================================================================*/
SAPDB_Bool SynonymDef::readResult( StudioOAL_String *record )
{
    SAPDB_Bool isOk = SAPDB_FALSE;
    // SYNONYMS  : OWNER, SYNONYMNAME, PUBLIC, TABLEOWNER, TABLENAME, COMMENT
    isOk = MetaDataItem::readResult( record);

    if ( isOk )
    {
         m_owner              = record[0];
         m_synonymname        = record[1];
         m_public             = record[2];
         m_tableowner         = record[3];
         m_tablename          = record[4];
         m_comment            = record[5];
    }
    return isOk;
}
/*===========================================================================*
 *     SynonymDef::writeResult()
 *===========================================================================*/
SAPDB_Bool SynonymDef::writeResult( StudioOAL_String *record )
{
      StudioOAL_String tableSynonym;

      if ( m_pTransformationCmd->trCatalogFile_ls00.fsFormat_ls00.ffoXml_ls00 == true )
           tableSynonym += genCreateXML();
      if ( m_pTransformationCmd->trCatalogFile_ls00.fsFormat_ls00.ffoDdl_ls00 == true  )
      {
           tableSynonym  = SEPARATOR;
           tableSynonym += genCreateDDL();
      }
      return writeLine( tableSynonym );
}
/*===========================================================================*
 *     SynonymDef::genCreateXML()
 *===========================================================================*/
StudioOAL_String SynonymDef::genCreateXML()
{
    // <dbm:SchemaTableSynonyms>
   StudioOAL_String  schemaTableSynonym;

   schemaTableSynonym = "<dbm:TableSynonym oim:id=\"_";
   schemaTableSynonym += string2XML(m_owner);
   schemaTableSynonym += ".";
   schemaTableSynonym += string2XML(m_synonymname);
   schemaTableSynonym += "\" oim:href= \"#_";
   schemaTableSynonym += string2XML(m_tableowner);
   schemaTableSynonym += ".";
   schemaTableSynonym += string2XML(m_tablename);
   schemaTableSynonym += "\" oim:seqno  = \"";
   schemaTableSynonym += "\" dbm:name   = \"";
   schemaTableSynonym += string2XML(m_synonymname);
   schemaTableSynonym += "\" dbm:description = \"";
   schemaTableSynonym += string2XML(m_synonymDefinition);
   schemaTableSynonym += "\" dbm:isSpecification = \"1";
   schemaTableSynonym += "\" dbm:comments = \"";
   schemaTableSynonym += string2XML(m_comment);
   schemaTableSynonym += "\"/>\n";
   return schemaTableSynonym;
}
/*===========================================================================*
 *     SynonymDef::genCreateDDL()
 *===========================================================================*/
StudioOAL_String SynonymDef::genCreateDDL()
{
   // CREATE SYNONYM
   m_synonymDefinition  = "CREATE ";
   m_public.Compare( "NO" )           != 0 ? m_synonymDefinition += m_public : m_synonymDefinition += "";
   m_synonymDefinition += " SYNONYM ";
   m_synonymDefinition += " \"";
   m_synonymDefinition += m_synonymname;
   m_synonymDefinition += "\" FOR \"";
   m_tableowner.Compare( NULL_VALUE ) != 0 ? m_synonymDefinition += m_tableowner : m_synonymDefinition += ""; 
   m_synonymDefinition += "\".\"";
   m_synonymDefinition += m_tablename;
   m_synonymDefinition += "\"";
   return m_synonymDefinition;
}
/*===========================================================================*
 *    ColumnTypeDef::ColumnTypeDef
 *===========================================================================*/
ColumnTypeDef::ColumnTypeDef( sapdbwa_DBC & odbcConnection , MetaDataDef * pMetaData, tls00_TransformationCmd * pTransformationCmd  )
              :MetaDataItem ( odbcConnection , pMetaData, pTransformationCmd )
{
    
    ROUTINE_DBG_MEO00 ("ColumnTypeDef::ColumnTypeDef()");
}
/*===========================================================================*
 *     ColumnTypeDef::ColumnTypeDef
 *===========================================================================*/
ColumnTypeDef::ColumnTypeDef( sapdbwa_DBC & odbcConnection )
                  :MetaDataItem ( odbcConnection )
{
    
    ROUTINE_DBG_MEO00 ("ColumnTypeDef::ColumnTypeDef()");
}
/*===========================================================================*
 *     ColumnTypeDef::~ColumnTypeDef
 *===========================================================================*/
ColumnTypeDef::~ColumnTypeDef()
{
}
/*===========================================================================*
 *     ColumnTypeDef::initResultSet
 *===========================================================================*/
SAPDB_Bool ColumnTypeDef::initResultSet ( sapdbwa_DBC & odbcConnection )
{
    SAPDB_Bool isOk = SAPDB_FALSE;
    if( odbcConnection.HDBC() && odbcConnection.HENV() )
        isOk = SAPDB_TRUE;
    return isOk;
}
/*===========================================================================*
 *    ColumnTypeDef::readResult
 *===========================================================================*/
SAPDB_Bool ColumnTypeDef::readResult ( StudioOAL_String *record  )
{
    return MetaDataItem::readResult( record);
}
/*===========================================================================*
 *    ColumnTypeDef::writeResult
 *===========================================================================*/
SAPDB_Bool ColumnTypeDef::writeResult( StudioOAL_String *record  )
{
    ROUTINE_DBG_MEO00 ("ColumnTypeDef::writeResult ()");

    if ( m_pTransformationCmd->trCatalogFile_ls00.fsFormat_ls00.ffoXml_ls00 == true )
    {
        // SYSODBCTYPES  :  0.TYPE_NAME, 1.DATA_TYPE, 2.PRECISION, 3.LITERAL_PREFIX, 4.LITERAL_SUFFIX, 5.CREATE_PARAMS, 6.NULLABLE, 7.CASE_SENSITIVE, 8.SEARCHABLE, 9.UNSIGNED_ATTRIBUTE, 10.MONEY, 11.AUTO_INCREMENT
        //                 12.LOCAL_TYPE_NAME, 13.MINIMUM_SCALE, 14.MAXIMUM_SCALE, 15.SQL_DATA_TYPE, 16.SQL_DATETIME_SUB, 17.NUM_PREC_RADIX, 18.INTERVAL_PRECISION, 19.REMARKS

        // SQLGetTypeInfo( hStmt, SQL_ALL_TYPES )

        // <data_typ> ::=
        //  CHAR[ACTER] [(<unsigned_integer>)] [ASCII | BYTE | EBCDIC]
        //| VARCHAR     [(<unsigned_integer>)] [ASCII | BYTE | EBCDIC]
        //| LONG [VARCHAR]                     [ASCII | BYTE | EBCDIC]
        //| BOOLEAN
        //| FIXED (<unsigned_integer> [,<unsigned_integer>])
        //| FLOAT (<unsigned_integer>)
        //| INT[EGER] | SMALLINT
        //| DATE | TIME | TIMESTAMP
        StudioOAL_String columntype;
        StudioOAL_String columntypeid;
        StudioOAL_String strClob     = "CLOB";
        StudioOAL_String strLong     = "LONG";
        StudioOAL_String strDouble   = "DOUB";
        StudioOAL_String strUnicode  = "UN";

        columntype  = "<sdbm:";
        if ( strUnicode.Compare(record[12].SubStr(0,2)) == 0 && record[12].Length() != 7  )
             columntypeid = "UNICODE_VARCHAR" ;
        if ( strUnicode.Compare(record[12].SubStr(0,2)) == 0 && record[12].Length() == 7  )
             columntypeid = "UNICODE" ;

        if ( strDouble.Compare(record[12].SubStr(0,4)) == 0  )
             columntypeid = "DOUBLE_PRECISION";

        if ( strClob  .Compare(record[12].SubStr(0,4)) == 0  && record[12].Length() == 12 )
             columntypeid = "CLOB_UNICODE" ;
        if ( strLong  .Compare(record[12].SubStr(0,4)) == 0  && record[12].Length() == 12 )
             columntypeid = "LONG_UNICODE" ;
        if ( strLong  .Compare(record[12].SubStr(0,4)) == 0  && record[12].Length() == 9  )
             columntypeid = "LONG_BYTE" ;
        if ( strLong  .Compare(record[12].SubStr(0,4)) == 0  && record[12].Length() == 4  )
             columntypeid = record[12];

        if ( strClob   .Compare(record[12].SubStr(0,4)) != 0 &&
             strLong   .Compare(record[12].SubStr(0,4)) != 0 &&
             strDouble .Compare(record[12].SubStr(0,4)) != 0 &&
             strUnicode.Compare(record[12].SubStr(0,2)) != 0    )
             columntypeid = record[12];

        columntype += columntypeid;
        columntype += " oim:id=\"_";
        columntype += "sdbm:";
        columntype += columntypeid;
        columntype += "\" oim:label=\"";                                                    // SQLColAttribute
        !getColumn(13)->getColIsNull() ? columntype += record[12] : columntype+= "";        // record[12].Compare( NULL_VALUE )  != 0
        columntype += "\" oim:supertype=\"";
        columntype += "dbm:ColumnType";
        columntype += "\" name=\"";                                                         // SQL_DESC_TYPE(ODBC 3.0), SQL_DESC_TYPE_NAME(ODBC 1.0), SQL_DESC_NAME(ODBC 3.0)
        !getColumn(1)->getColIsNull() ? columntype += record[0]  : columntype+= "";         // SQL_DESC_LABEL(ODBC 2.0) record[0].Compare( NULL_VALUE )   != 0
        columntype += "\" ColumnSize=\"";                                                   // SQL_DESC_LENGTH (ODBC 3.0) ,SQLDescribeCol
        !getColumn(3)->getColIsNull() ? columntype += record[2]  : columntype+= "";         // record[2].Compare( NULL_VALUE )   != 0
        //columntype += " \"IsFixedLength=\"";
        //columntype += record[];
        columntype += "\" LiteralPrefix=\"";                                                // SQL_DESC_LITERAL_PREFIX(ODBC 3.0)
        !getColumn(4)->getColIsNull() ? columntype += record[3]  : columntype+= "";         // record[3].Compare( NULL_VALUE )  != 0
        columntype += "\" LiteralSuffix=\"";                                                // SQL_DESC_LITERAL_SUFFIX(ODBC 3.0)
        !getColumn(5)->getColIsNull() ? columntype += record[4]  : columntype+= "";         // record[4].Compare( NULL_VALUE )  != 0
        columntype += "\" CreateParams=\"";
        !getColumn(6)->getColIsNull() ? columntype += record[5] : columntype+= "";          // record[5].Compare( NULL_VALUE )  != 0
        columntype += "\" IsNullable=\"";                                                   // SQL_DESC_NULLABLE(ODBC 3.0)
        !getColumn(7)->getColIsNull() ? columntype += record[6]  : columntype+= "";         // record[6].Compare( NULL_VALUE )  != 0
        columntype += "\" IsCaseSensitive=\"";                                              // SQL_DESC_CASE_SENSITIVE(ODBC 1.0)
        !getColumn(8)->getColIsNull() ? columntype += record[7]  : columntype+= "";         // record[7].Compare( NULL_VALUE )  != 0
        columntype += "\" Searchable=\"";                                                   // SQL_DESC_SEARCHABLE(ODBC 1.0)
        !getColumn(9)->getColIsNull() ? columntype += record[8] : columntype+= "";          // record[8].Compare( NULL_VALUE )  != 0
        columntype += "\" IsUnsignedAttribute=\"";
        !getColumn(10)->getColIsNull()? columntype += record[9]  : columntype += "";        // record[9].Compare( NULL_VALUE )  != 0
        columntype += "\" IsAutoUniqueValue=\"";                                            // SQL_DESC_AUTO_UNIQUE_VALUE(ODBC 1.0)
        !getColumn(12)->getColIsNull()? columntype += record[11] : columntype+= "";         // record[11].Compare( NULL_VALUE ) != 0
        columntype += "\" MinimumScale=\"";                                                 // SQL_DESC_SCALE(ODBC 3.0)
        !getColumn(14)->getColIsNull()? columntype += record[13]: columntype+= "";          // SQL_DESC_UNSIGNED(ODBC 1.0) record[13].Compare( NULL_VALUE ) != 0
        columntype += "\" MaximumScale=\"";
        !getColumn(15)->getColIsNull()? columntype += record[14]: columntype+= "";          // SQL_DESC_FIXED_PREC_SCALE(ODBC 1.0) record[14].Compare( NULL_VALUE ) != 0
        columntype += "\" IsFixedPrecisionScale=\"";                                        // SQL_DESC_PRECISION(ODBC 3.0)
        !getColumn(3)->getColIsNull() ? columntype += record[2] : columntype+= "";          // record[2].Compare( NULL_VALUE )  != 0

        columntype += "\" IsLong=\"";
        !getColumn(13)->getColIsNull() && strLong.Compare(record[12].SubStr(0,4)) == 0 ? columntype += "1" : columntype += "0"; // record[12].Compare( NULL_VALUE ) != 0

        columntype += "\"/>";

        return writeLine( columntype );
    }
    return SAPDB_TRUE;
}
/*===========================================================================*
 *    TriggerDef::TriggerDef
 *===========================================================================*/
TriggerDef::TriggerDef( sapdbwa_DBC & odbcConnection , MetaDataDef * pMetaData, tls00_TransformationCmd * pTransformationCmd )
           :MetaDataItem ( odbcConnection , pMetaData, pTransformationCmd )
{
    ROUTINE_DBG_MEO00 ("TriggerDef::TriggerDef()");
    m_pTriggerDefResult = new ResultSet ( odbcConnection );
}
/*===========================================================================*
 *     TriggerDef::TriggerDef()
 *===========================================================================*/
TriggerDef::TriggerDef( sapdbwa_DBC & odbcConnection )
               :MetaDataItem ( odbcConnection )
{
    ROUTINE_DBG_MEO00 ("TriggerDef::TriggerDef()");
    m_pTriggerDefResult = new ResultSet ( odbcConnection );
}
/*===========================================================================*
 *     TriggerDef::~TriggerDef()
 *===========================================================================*/
TriggerDef::~TriggerDef( )
{
    ROUTINE_DBG_MEO00 ("TriggerDef::~TriggerDef()");
    if ( m_pTriggerDefResult != NULL )
    {
         delete m_pTriggerDefResult;
         m_pTriggerDefResult = NULL;
    }
}
/*===========================================================================*
 *     TriggerDef::initResultSet
 *===========================================================================*/
SAPDB_Bool TriggerDef::initResultSet ( sapdbwa_DBC & odbcConnection )
{
    SAPDB_Bool isOk = SAPDB_FALSE;
    if ( odbcConnection.HDBC() && odbcConnection.HENV() )
    {
        StudioOAL_String  triggerDef = "";
        // SQLAllocStmt     SQLPrepare
        // SQLNumParams     SQLDescribeParam SQLBindParameter
        // SQLNumResultCols SQLDescribeCol   SQLBindCol
        isOk = m_pTriggerDefResult->init(triggerDef);
        if (SAPDB_FALSE == isOk)
        {
            getMetaData()->getError()->setErrorText(getErrorText() );
            getMetaData()->getError()->isError     (isError     () );
        }
    }
    return isOk;
}
/*===========================================================================*
 *     TriggerDef::readTriggerDefinition
 *===========================================================================*/
SAPDB_Bool TriggerDef::readTriggerDefinition( StudioOAL_String schemaname, StudioOAL_String tablename )
{
   ROUTINE_DBG_MEO00 ("ViewDef::readTriggerDefinition()");

   SAPDB_Bool isOk = SAPDB_FALSE;

   StudioOAL_String  triggerDef[2];
   StudioOAL_String  record[5];

   triggerDef[0] = schemaname;
   triggerDef[1] = tablename;

  /*-------------------------------------------------------------------------*
   * Load the trigger definitions from the database.                           *
   *-------------------------------------------------------------------------*/

   if ( !m_pTriggerDefResult->setParamValues( triggerDef ) )
   {
        getMetaData()->getError()->setErrorText(getErrorText() );
        getMetaData()->getError()->isError     (isError     () );
        return SAPDB_FALSE;
   }
   /*-------------------------------------------------------------------------*
    * Execute the prepared statement                                          *
    *-------------------------------------------------------------------------*/
   // SQLExecute
   if ( !m_pTriggerDefResult->execute() )
   {
        StudioOAL_String errorText = m_pTriggerDefResult->getErrorText();
        SAPDB_Bool      isError   = m_pTriggerDefResult->isError     ();

        getMetaData()->getError()->setErrorText( errorText );
        getMetaData()->getError()->isError     ( isError   );

        return SAPDB_FALSE;
   }
   // SQlFetch SQLFreeStatement
   if ( readTriggerDefResult (record) )
   {
        m_triggerDefinition = record[0];
        m_triggerName       = record[1];
        m_tablename         = record[2];
        m_comment           = record[3];
        m_owner             = record[4];

        isOk = SAPDB_TRUE;
   }

   return isOk;

}
/*===========================================================================*
 *     TriggerDef::writeTriggerDefinition
 *===========================================================================*/
SAPDB_Bool TriggerDef::writeTriggerDefinition()
{
   ROUTINE_DBG_MEO00 ("TriggerDef::writeTriggerDefinition");

   SAPDB_Bool isOk = SAPDB_FALSE;

   StudioOAL_String tableTrigger;
   if( m_pTransformationCmd->trCatalogFile_ls00.fsFormat_ls00.ffoDdl_ls00 == true  )
   {
      tableTrigger  = SEPARATOR;

      if ( getMetaData()->getSqlMode() != cpr_kind_internal )
      {
           SAPDB_Int4 iSqlMode = getMetaData()->getSqlMode();
           tableTrigger += pszSQLModeString_ls31[iSqlMode];
           tableTrigger += SEPARATOR;
      }

      tableTrigger += m_triggerDefinition;

      // COMMENT ON TRIGGER <trigger_name> ON < table_name> IS <comment>
      if ( !m_comment.Empty() && m_comment.Compare( NULL_VALUE) != 0 )
      {
           tableTrigger += NEW_LINE;
           tableTrigger += SEPARATOR;
           tableTrigger += "COMMENT ON TRIGGER \"";
           tableTrigger += m_triggerName;
           tableTrigger += "\" ON \"";
           tableTrigger += m_tablename;
           tableTrigger += "\" IS ' ";
           tableTrigger += string2SQL(m_comment);
           tableTrigger += "'";
      }
   }
   if ( m_pTransformationCmd->trCatalogFile_ls00.fsFormat_ls00.ffoXml_ls00 == true )
        tableTrigger += genCreateXML();

   return writeLine( tableTrigger );

}
/*===========================================================================*
 *     TriggerDef::readTriggerDefResult
 *===========================================================================*/
SAPDB_Bool TriggerDef::readTriggerDefResult( StudioOAL_String *record )
{
   SAPDB_Int2       i;
   StudioOAL_String  s;
  /*-------------------------------------------------------------------------*
   * Fetch the next record.                                                  *
   *-------------------------------------------------------------------------*/
  // SQLFetch
  if (!m_pTriggerDefResult->fetch())
  {
      m_pTriggerDefResult->closeCursor();
      return SAPDB_FALSE;
  }
  /*-------------------------------------------------------------------------*
   * If there are no more records, reading the result is finished.           *
   *-------------------------------------------------------------------------*/
  if (m_pTriggerDefResult->noRows())
  {
     m_pTriggerDefResult->closeCursor();
     return SAPDB_FALSE;
  }
  /*-------------------------------------------------------------------------*
   * Read all columns of the fetched record.                                 *
   *-------------------------------------------------------------------------*/
  for (i = 0; i < m_pTriggerDefResult->getColCount(); i++)
  {
    m_pTriggerDefResult->bindColumnDataAsString (i + 1, s);
    record[i] = s;
  }
  return SAPDB_TRUE;
}
/*===========================================================================*
 *     TriggerDef::readResult()
 *===========================================================================*/
SAPDB_Bool TriggerDef::readResult( StudioOAL_String *record )
{
    SAPDB_Bool isOk = SAPDB_FALSE;

    isOk = MetaDataItem::readResult( record);

    if ( isOk )
    {
         m_triggerName       = record[1];
         m_owner             = record[4];
         m_triggerDefinition = record[0];
         m_tablename         = record[2] ;
         m_comment           = record[3];
    }
    return isOk;
}
/*===========================================================================*
 *     TriggerDef::writeResult()
 *===========================================================================*/
SAPDB_Bool TriggerDef::writeResult( StudioOAL_String *record )
{
      StudioOAL_String tableTrigger;

      if ( m_pTransformationCmd->trCatalogFile_ls00.fsFormat_ls00.ffoDdl_ls00 == true  )
      {
          tableTrigger  = SEPARATOR;
          tableTrigger += record[0];

          // COMMENT ON TRIGGER <trigger_name> ON < table_name> IS <comment>
          if ( !getColumn(4)->getColIsNull() && !record[3].Empty() ) // record[3]!= NULL_VALUE
          {
               tableTrigger += NEW_LINE;
               tableTrigger += SEPARATOR;
               tableTrigger += "COMMENT ON TRIGGER \"";
               tableTrigger += record[1];
               tableTrigger += "\" ON \"";
               tableTrigger += record[2];
               tableTrigger += "\" IS ' ";
               tableTrigger += string2SQL(record[3]);
               tableTrigger += "'";
          }
      }
      if ( m_pTransformationCmd->trCatalogFile_ls00.fsFormat_ls00.ffoXml_ls00 == true )
           tableTrigger += genCreateXML();
      return writeLine( tableTrigger );
}
/*===========================================================================*
 *     TriggerDef::genCreateXML()
 *===========================================================================*/
StudioOAL_String TriggerDef::genCreateXML()
{
   // <dbm:TableTrigger>
   StudioOAL_String tableTrigger;

   tableTrigger  = "<dbm:DeployedTrigger oim:id=\"_";
   tableTrigger += string2XML( m_owner );
   tableTrigger += ".";
   tableTrigger += string2XML( m_triggerName );
   tableTrigger += ".";
   tableTrigger += string2XML( m_tablename );
   tableTrigger += "\" oim:supertype = \"Trigger";
   tableTrigger += "\" dbm:StatementsLanguage = \"";
   tableTrigger += "\" dbm:Statements         = \"";
   tableTrigger += string2XML ( m_triggerDefinition );
   tableTrigger += "\" dbm:isSpecification = \"1";
   tableTrigger += "\" dbm:comments = \"";
   tableTrigger += string2XML ( m_comment );
   tableTrigger += "\"/>\n";
   return tableTrigger;
}
/*===========================================================================*
 *    ProcedureDef::ProcedureDef
 *===========================================================================*/
ProcedureDef::ProcedureDef( sapdbwa_DBC & odbcConnection , MetaDataDef * pMetaData, tls00_TransformationCmd * pTransformationCmd  )
             :MetaDataItem ( odbcConnection , pMetaData, pTransformationCmd )
{
    m_pGrantProcedureDef = new GrantProcedureDef( odbcConnection, pMetaData, pTransformationCmd );
}
/*===========================================================================*
 *     ProcedureDef::ProcedureDef()
 *===========================================================================*/
ProcedureDef::ProcedureDef(sapdbwa_DBC & odbcConnection )
             :MetaDataItem ( odbcConnection )
{
    m_pGrantProcedureDef = new GrantProcedureDef( odbcConnection );
}
/*===========================================================================*
 *     ProcedureDef::~ProcedureDef()
 *===========================================================================*/
ProcedureDef::~ProcedureDef()
{
    if( m_pGrantProcedureDef != NULL )
    {
        delete m_pGrantProcedureDef;
        m_pGrantProcedureDef = NULL;
    }
}
/*===========================================================================*
 *     ProcedureDef::initResultSet
 *===========================================================================*/
SAPDB_Bool ProcedureDef::initResultSet ( sapdbwa_DBC & odbcConnection )
{
    ROUTINE_DBG_MEO00 ("ProcedureDef::initResult ()");
    SAPDB_Bool isOk = SAPDB_FALSE;

    if ( odbcConnection.HDBC() && odbcConnection.HENV() )
         isOk = SAPDB_TRUE;
     return isOk;
}
/*===========================================================================*
 *     ProcedureDef::readResult
 *===========================================================================*/
SAPDB_Bool ProcedureDef::readResult(  StudioOAL_String *record )
{
    SAPDB_Bool isOk = SAPDB_FALSE;
    isOk = MetaDataItem::readResult( record);
    if( isOk )
    {
        m_owner                 = record[0];
		m_schemaname            = record[1];
		m_dbprocedureName       = record[2];
        m_dbprocedureDefinition = record[3];
        m_comment               = record[4];
    }
    return isOk;
}
/*===========================================================================*
 *     ProcedureDef::writeResult
 *===========================================================================*/
SAPDB_Bool ProcedureDef::writeResult()
{
      StudioOAL_String procedure;
      if ( m_pTransformationCmd->trCatalogFile_ls00.fsFormat_ls00.ffoDdl_ls00 == true  )
      {
          // OWNER, SCHEMANAME, DBPROCNAME, DEFINITION, COMMENT
          procedure  = SEPARATOR;
          if ( getMetaData()->getSqlMode() != cpr_kind_internal )
          {
               SAPDB_Int4 iSqlMode = getMetaData()->getSqlMode();
               procedure += pszSQLModeString_ls31[iSqlMode];
               procedure += SEPARATOR;
          }
          procedure += m_dbprocedureDefinition;
          // COMMENT ON DBPROC <dbproc_name> IS <comment>
          if ( !getColumn(5)->getColIsNull() && !m_dbprocedureDefinition.Empty() ) 
          {
               procedure += SEPARATOR;
               procedure += "COMMENT ON DBPROC \"";
               procedure += m_dbprocedureName;
               procedure += "\" IS '";
               procedure += string2SQL(m_dbprocedureDefinition);
               procedure += "'";
          }
          // GRANT EXECUTE ON <dbproc_name> TO <grantee>,...
          grantDBProcedureExtrCmd( m_dbprocedureName );
      }
      if ( m_pTransformationCmd->trCatalogFile_ls00.fsFormat_ls00.ffoXml_ls00 == true )
           procedure  = genCreateXML();
      return writeLine(procedure);
}
/*===========================================================================*
 *     ProcedureDef::genCreateXML()
 *===========================================================================*/
StudioOAL_String  ProcedureDef::genCreateXML()
{
    // <dbm:StoredProcedure>
    StudioOAL_String procedure;
    procedure  = "<dbm:StoredProcedure oim:id=\"_";
    procedure += string2XML( m_owner );
    procedure += ".";
    procedure += string2XML( m_dbprocedureName );
    procedure += "\" oim:label=\"";
    procedure += string2XML( m_dbprocedureName );
    procedure += "\" dbm:bodyLanguage = \"&lt;create_dbproc_statement&gt;::= see SAPDB reference manual";
    procedure += "\" dbm:body         = \"";
    procedure += string2XML( m_dbprocedureDefinition );
    procedure += "\" dbm:name= \"";
    procedure += string2XML( m_dbprocedureName );
    procedure += "\" dbm:isQuery =\"";
    procedure += "\" dbm:isSpecification = \"1";
    procedure += "\" dbm:comments = \"";
    procedure += string2XML ( m_comment );
    procedure += "\"/>\n";
    return procedure;
}
/*===========================================================================*
 *  ProcedureDef::grantDBProcedureExtrCmd
 *===========================================================================*/
SAPDB_Int4 ProcedureDef::grantDBProcedureExtrCmd( StudioOAL_String dbprocname )
{
    ROUTINE_DBG_MEO00 ("ProcedureDef::grantDBProcedureExtrCmd");
    SAPDB_Int4 rc = errOK_els00;

    // GRANT EXECUTE ON <dbproc_name> TO <grantee>,...
    // <grantee> ::= <usergroup_name> | <user_name> | <role_name> | PUBLIC

    // SYSDD.PRIVILEGES : OWNER, NAME1, NAME2, TYPE, GRANTEE, GRANTEEOWNER,
    //                    PRIVILEGES, GRANTOR, DATE, TIME
    StudioOAL_String  statement;
    StudioOAL_String  whereClause;
    StudioOAL_String  record[2];

    StudioOAL_String  grantprocedure;

    statement  = "SELECT NAME1, GRANTEE FROM SYSDD.PRIVILEGES ";
    /*-------------------------------------------------------------------------*
     * Prepare the WHERE clause for the SQL statements.
     *-------------------------------------------------------------------------*/
    whereClause  = " WHERE type   = 'DBPROC' "; //'SEQUENCE'
    whereClause += "  AND  name1  = '";
    whereClause +=  m_dbprocedureName ;
    whereClause += "' AND  grantee <> '";
    whereClause +=  m_owner;
    whereClause += "'";

    statement      += whereClause;
    if ( !m_pGrantProcedureDef->executeSQL( statement )  )
         rc = STATIC_CAST(SAPDB_Int4, m_pGrantProcedureDef->getNativeError());
    else
    {
        while ( m_pGrantProcedureDef->readResult (record ) )
        {
            grantprocedure  = SEPARATOR;
            grantprocedure += "GRANT EXECUTE ON \"";
            grantprocedure += record[0];
            grantprocedure += "\" TO ";
            grantprocedure += record[1];
            m_pGrantProcedureDef->writeResult(grantprocedure);
        }
    }
    return rc;
}
/*===========================================================================*
 *    GrantProcedureDef::GrantProcedureDef
 *===========================================================================*/
GrantProcedureDef::GrantProcedureDef( sapdbwa_DBC & odbcConnection , MetaDataDef * pMetaData,  tls00_TransformationCmd * pTransformationCmd  )
                  :MetaDataItem   ( odbcConnection , pMetaData, pTransformationCmd )
{
    ROUTINE_DBG_MEO00 ("GrantProcedureDef::GrantProcedureDef()");
}
/*===========================================================================*
 *    GrantProcedureDef::GrantProcedureDef
 *===========================================================================*/
GrantProcedureDef::GrantProcedureDef( sapdbwa_DBC & odbcConnection )
                    :MetaDataItem   ( odbcConnection )
{
    ROUTINE_DBG_MEO00 ("GrantProcedureDef::GrantProcedureDef()");
}
/*===========================================================================*
 *    GrantProcedureDef::GrantProcedureDef
 *===========================================================================*/
GrantProcedureDef::~GrantProcedureDef()
{
    ROUTINE_DBG_MEO00 ("GrantProcedureDef::~GrantProcedureDef()");
}
/*===========================================================================*
 *     GrantProcedureDef::initResultSet
 *===========================================================================*/
SAPDB_Bool GrantProcedureDef::initResultSet ( sapdbwa_DBC & odbcConnection )
{
    ROUTINE_DBG_MEO00 ("GrantProcedureDef::initResult ()");
    SAPDB_Bool isOk = SAPDB_FALSE;

    if( odbcConnection.HDBC() && odbcConnection.HENV() )
        isOk = SAPDB_TRUE;
    return isOk;
}
/*===========================================================================*
 *    GrantProcedureDef::readResult
 *===========================================================================*/
SAPDB_Bool GrantProcedureDef::readResult ( StudioOAL_String *record  )
{
    ROUTINE_DBG_MEO00 ("GrantProcedureDef::readResult ()");
    return MetaDataItem::readResult( record);
}
/*===========================================================================*
 *    GrantProcedureDef::writeResult
 *===========================================================================*/
SAPDB_Bool GrantProcedureDef::writeResult( StudioOAL_String record  )
{
    ROUTINE_DBG_MEO00 ("GrantProcedureDef::writeResult ()");
    return MetaDataItem::writeLine ( record);
}
/*===========================================================================*
 *    IndexDef::IndexDef
 *===========================================================================*/
IndexDef::IndexDef( sapdbwa_DBC & odbcConnection , MetaDataDef * pMetaData,  tls00_TransformationCmd * pTransformationCmd   )
         :MetaDataItem ( odbcConnection , pMetaData, pTransformationCmd )
{
    ROUTINE_DBG_MEO00 ("IndexDef::IndexDef()");
    m_pIndexDefResult    = new ResultSet ( odbcConnection );
    m_pIndexColDefResult = new ResultSet ( odbcConnection );
    m_pIndexColDefODBC   = new IndexColumnDefODBC( odbcConnection, pMetaData, pTransformationCmd );
    m_pIndexColDefODBC->setIndexDef( this );
}
/*===========================================================================*
 *     IndexDef::IndexDef()
 *===========================================================================*/
IndexDef::IndexDef( sapdbwa_DBC & odbcConnection )
         :MetaDataItem ( odbcConnection )
{
    ROUTINE_DBG_MEO00 ("IndexDef::IndexDef()");
    m_pIndexDefResult    = new ResultSet ( odbcConnection );
    m_pIndexColDefResult = new ResultSet ( odbcConnection );
    m_pIndexColDefODBC   = new IndexColumnDefODBC( odbcConnection );

    m_pIndexColDefODBC->setMetaData(getMetaData());
    m_pIndexColDefODBC->setIndexDef( this );

}
/*===========================================================================*
 *     IndexDef::~IndexDef
 *===========================================================================*/
IndexDef::~IndexDef()
{
   ROUTINE_DBG_MEO00 ("IndexDef::~IndexDef()");
   if( m_pIndexColDefODBC != NULL  )
   {
        delete m_pIndexColDefODBC;
        m_pIndexColDefODBC = NULL;
   }
  /*-------------------------------------------------------------------------*
   * Remove ResultSet
   *-------------------------------------------------------------------------*/
   if ( m_pIndexDefResult != NULL )
   {
       delete m_pIndexDefResult;
       m_pIndexDefResult = NULL;
   }
   if ( m_pIndexColDefResult != NULL )
   {
       delete m_pIndexColDefResult;
       m_pIndexColDefResult = NULL;
   }
  /*-------------------------------------------------------------------------*
   * Remove all columns
   *-------------------------------------------------------------------------*/
   IndexColumnDef  *pCol = (IndexColumnDef*)m_IndexColumns.first();

   while (pCol != NULL)
   {
        delete pCol;
        pCol = (IndexColumnDef*)m_IndexColumns.next();
   }
   m_IndexColumns.clear();
}
/*===========================================================================*
 *     IndexDef::initResultSet
 *===========================================================================*/
SAPDB_Bool IndexDef::initResultSet ( sapdbwa_DBC & odbcConnection )
{
    SAPDB_Bool isOk = SAPDB_FALSE;

    if (  odbcConnection.HDBC() && odbcConnection.HENV() )
    {
        //<create_index_statement> ::=
        // CREATE [UNIQUE] INDEX <index_name> ON <table_name> (<column_name> [ASC | DESC],...)
        StudioOAL_String  indexDef    = "";
        StudioOAL_String  indexColDef = "";
        // SQLAllocStmt     SQLPrepare
        // SQLNumParams     SQLDescribeParam SQLBindParameter
        // SQLNumResultCols SQLDescribeCol   SQLBindCol
        isOk = m_pIndexDefResult->init(indexDef);
        if (SAPDB_TRUE == isOk)
            isOk = m_pIndexColDefResult->init(indexColDef);
        if (SAPDB_FALSE == isOk)
        {
            getMetaData()->getError()->setErrorText(getErrorText());
            getMetaData()->getError()->isError(isError());
        }
    }
    return isOk;
}
/*===========================================================================*
 *     IndexDef::reset()
 *===========================================================================*/
void IndexDef::reset()
{
    ROUTINE_DBG_MEO00 ("IndexDef::reset()");
  /*-------------------------------------------------------------------------*
   * Remove all columns
   *-------------------------------------------------------------------------*/
   IndexColumnDef  *pCol = (IndexColumnDef *)m_IndexColumns.first();
   while (pCol != NULL)
   {
        delete pCol;
        pCol = (IndexColumnDef* )m_IndexColumns.next();
   }
   m_IndexColumns.clear();
}
/*===========================================================================*
 *     IndexDef::readIndexDefinitionDirect
 *===========================================================================*/
SAPDB_Bool IndexDef::readIndexDefinitionDirect(StudioOAL_String schemaname,
                                               StudioOAL_String tablename)
{
    ROUTINE_DBG_MEO00 ("IndexDef::readIndexDefinitionDirect ()");
    StudioOAL_String  record[4];
    // INDICES
    if ( !readResult( record ) )
         return SAPDB_FALSE;
    m_schemaname   = schemaname;
    m_tablename    = tablename;
    m_indexType    = record[0];
    m_indexname    = record[1];
    m_comment      = record[2];
    m_disabled     = record[3];
    // INDEXCOLUMNS
    readIndexColDefinitionDirect();
    return SAPDB_TRUE;
}
/*===========================================================================*
 *    IndexDef::readIndexColDefinitionDirect
 *===========================================================================*/
SAPDB_Bool IndexDef::readIndexColDefinitionDirect()
{
    ROUTINE_DBG_MEO00 ("IndexDef::readIndexColumnDefinitionDirect ()");
    SAPDB_Bool isOk = SAPDB_FALSE;

    StudioOAL_String  schema_name;
    StudioOAL_String  comment;
    StudioOAL_String  record[4];
    _TCHAR statement  [1024];
    statement         [0]=0;
    reset();
    /*-------------------------------------------------------------------------*
     * Prepare the WHERE clause for the SQL statements.
     *-------------------------------------------------------------------------*/
    //<create_index_statement> ::=
    // CREATE [UNIQUE] INDEX <index_name> ON <table_name> (<column_name> [ASC | DESC],...)
    if (SAPDB_TRUE == m_schemaname.Empty())
        schema_name = "%";
    else
        schema_name = m_schemaname;
    sp77sprintf(statement, 1024, CONST_CAST(SAPDB_Char*, Statement(sel_Relational_15).CharPtr()),
                                 schema_name.StrPtr(), m_tablename.StrPtr(), m_indexname.StrPtr() );

    IndexColumnDef* pIndexColDef;
    if ( m_pIndexColDefODBC->executeSQL( statement ) )
    {
        while ( m_pIndexColDefODBC->readResult(record) )
        {
            // INDEXCOLUMNS
            pIndexColDef = addColumn();
            pIndexColDef->setSchemaName( m_schemaname);
            pIndexColDef->setTableName ( m_tablename);
            pIndexColDef->setIndexName ( m_indexname);
            pIndexColDef->setIndexType ( record[0]);
            pIndexColDef->m_column.setName( record[1]);
            pIndexColDef->setSorting      ( record[2]);
            // INDEXCOLUMNS : COMMENT
            if( record[3].Compare( NULL_VALUE ) != 0 )
                pIndexColDef->setComment ( record[3] );
            isOk = SAPDB_TRUE;
        }
    }
    return isOk;
}
/*===========================================================================*
 *     IndexDef::setIndexParameters
 *===========================================================================*/
SAPDB_Bool IndexDef::setIndexParameters( StudioOAL_String schemaname ,
                                         StudioOAL_String tablename )
{
   ROUTINE_DBG_MEO00 ("IndexDef::setIndexParameters()");

   SAPDB_Bool isOk = SAPDB_TRUE;
   StudioOAL_String  indexDef[2];
   reset();
   m_schemaname = indexDef[0] = schemaname;
   m_tablename  = indexDef[1] = tablename;
   /*-------------------------------------------------------------------------*
   * Load the index definitions from the database.                           *
   *-------------------------------------------------------------------------*/
   if ( !m_pIndexDefResult->setParamValues( indexDef ) )
   {
        getMetaData()->getError()->setErrorText(getErrorText() );
        getMetaData()->getError()->isError     (isError     () );
        return SAPDB_FALSE;
   }
   /*-------------------------------------------------------------------------*
    * Execute the prepared statement                                          *
    *-------------------------------------------------------------------------*/
   // SQLExecute
   if ( !m_pIndexDefResult->execute() )
   {
        StudioOAL_String errorText = m_pIndexDefResult->getErrorText();
        SAPDB_Bool      isError    = m_pIndexDefResult->isError     ();

        getMetaData()->getError()->setErrorText( errorText );
        getMetaData()->getError()->isError     ( isError   );

        return SAPDB_FALSE;
   }
   return isOk;
}
/*===========================================================================*
 *     IndexDef::readIndexDefinition
 *===========================================================================*/
SAPDB_Bool IndexDef::readIndexDefinition( StudioOAL_String schemaname ,
                                          StudioOAL_String tablename )
{
   ROUTINE_DBG_MEO00 ("IndexDef::readIndexDefinition()");

   SAPDB_Bool isOk = SAPDB_FALSE;

   StudioOAL_String  record[6];

   // SQlFetch SQLFreeStatement
   if ( readIndexDefResult (record) )
   {
        m_indexType = record[0];
        m_indexname = record[1];
        m_comment   = record[2];

        isOk = readIndexColDefinition( schemaname, tablename, m_indexname );
   }
   return isOk;
}
/*===========================================================================*
 *     IndexDef::readIndexDefResult
 *===========================================================================*/
SAPDB_Bool IndexDef::readIndexDefResult(StudioOAL_String *record)
{
   SAPDB_Int2       i;
   StudioOAL_String  s;
  /*-------------------------------------------------------------------------*
   * Fetch the next record.                                                  *
   *-------------------------------------------------------------------------*/
  // SQLFetch
  if (!m_pIndexDefResult->fetch())
  {
      m_pIndexDefResult->closeCursor();
      return SAPDB_FALSE;
  }
  /*-------------------------------------------------------------------------*
   * If there are no more records, reading the result is finished.           *
   *-------------------------------------------------------------------------*/
  if (m_pIndexDefResult->noRows())
  {
     m_pIndexDefResult->closeCursor();
     return SAPDB_FALSE;
  }

  /*-------------------------------------------------------------------------*
   * Read all columns of the fetched record.                                 *
   *-------------------------------------------------------------------------*/
  for (i = 0; i < m_pIndexDefResult->getColCount(); i++)
  {
    m_pIndexDefResult->bindColumnDataAsString (i + 1, s);
    record[i] = s;
  }
  return SAPDB_TRUE;
}
/*===========================================================================*
 *    IndexDef::readIndexColDefinition
 *===========================================================================*/
SAPDB_Bool IndexDef::readIndexColDefinition( StudioOAL_String schemaname    ,
                                             StudioOAL_String tablename,
                                             StudioOAL_String indexname )
{
    ROUTINE_DBG_MEO00 ("IndexDef::readIndexColDefinition()");
    SAPDB_Bool rcBool = SAPDB_TRUE;

    reset();

    StudioOAL_String record[7];
    StudioOAL_String indexColDef[3];

    indexColDef[0] = schemaname;
    indexColDef[1] = tablename;
    indexColDef[2] = indexname;
    /*-------------------------------------------------------------------------*
     * Load the indexcolumn definitions from the database.                     *
     *-------------------------------------------------------------------------*/
    rcBool = m_pIndexColDefResult->setParamValues( indexColDef );
    if (SAPDB_TRUE == rcBool)
    {
        /*-------------------------------------------------------------------------*
         * Execute the prepared statement                                          *
         *-------------------------------------------------------------------------*/
        // SQLExecute
        rcBool = m_pIndexColDefResult->execute();
    }

    if (SAPDB_FALSE == rcBool)
    {
        getMetaData()->getError()->setErrorText(getErrorText() );
        getMetaData()->getError()->isError     (isError     () );
        rcBool = SAPDB_FALSE;
    }
    else
    {
        IndexColumnDef *pIndexColDef;
        // SQlFetch SQLFreeStatement
        while(SAPDB_TRUE == (rcBool = readIndexColDefResult(record)) )
        {
            // INDEXCOLUMNS
            pIndexColDef = addColumn();
            pIndexColDef->setSchemaName( record[0]);
            pIndexColDef->setTableName ( record[1]);
            pIndexColDef->setIndexName ( record[2]);
            pIndexColDef->setIndexType ( record[3]);
            pIndexColDef->m_column.setName( record[4]);
            pIndexColDef->setSorting      ( record[5]);
            // INDEXCOLUMNS : COMMENT
            if ( !m_pIndexColDefResult->getColumn(7)->getColIsNull() )
            {
                pIndexColDef->setComment ( record[6] );
            }
        }
    }
    return rcBool;
}
/*===========================================================================*
 *     IndexDef::readIndexColDefResult
 *===========================================================================*/
SAPDB_Bool IndexDef::readIndexColDefResult(StudioOAL_String *record)
{
   SAPDB_Int2       i;
   StudioOAL_String  s;

  /*------------------------------------------------------------------------*
   * Fetch the next record.                                                  *
   *-------------------------------------------------------------------------*/
  // SQLFetch
  if (!m_pIndexColDefResult->fetch())
  {
      m_pIndexColDefResult->closeCursor();
      return SAPDB_FALSE;
  }
  /*-------------------------------------------------------------------------*
   * If there are no more records, reading the result is finished.           *
   *-------------------------------------------------------------------------*/
  if (m_pIndexColDefResult->noRows())
  {
     m_pIndexColDefResult->closeCursor();
     return SAPDB_FALSE;
  }
  /*-------------------------------------------------------------------------*
   * Read all columns of the fetched record.                                 *
   *-------------------------------------------------------------------------*/
  for (i = 0; i < m_pIndexColDefResult->getColCount(); i++)
  {
    m_pIndexColDefResult->bindColumnDataAsString (i + 1, s);
    record[i] = s;
  }
  return SAPDB_TRUE;
}
/*===========================================================================*
 *     IndexDef::writeIndexDefinition
 *===========================================================================*/
SAPDB_Bool IndexDef::writeIndexDefinition()
{
    ROUTINE_DBG_MEO00 ("IndexDef::writeIndexDefinition()");

    StudioOAL_String tableIndices;

    if (m_pTransformationCmd->trCatalogFile_ls00.fsFormat_ls00.ffoDdl_ls00 == true||
        m_pTransformationCmd->trCmdFile_ls00.fsFileName_ls00 != NULL )
    {
        if ( m_IndexColumns.size() > 0 )
        {
            tableIndices  = SEPARATOR;
            if (cpr_kind_internal != getMetaData()->getSqlMode())
            {
                SAPDB_Int4 iSqlMode = getMetaData()->getSqlMode();
                tableIndices += pszSQLModeString_ls31[iSqlMode];
                tableIndices += SEPARATOR;
            }
            tableIndices += genCreateStatement();

            // COMMENT ON INDEX <index_name> ON <table_name> IS '  '
            if ( (!m_comment.Empty()) && (0 != m_comment.Compare( NULL_VALUE)) )
            {
                tableIndices += NEW_LINE;
                tableIndices += SEPARATOR;
                tableIndices += "COMMENT ON INDEX \"";
                tableIndices += m_indexname;
                tableIndices += "\" ON \"";

                if (SAPDB_FALSE == m_schemaname.Empty())
                {
                    tableIndices += m_schemaname;
                    tableIndices += "\".\"";
                }
                tableIndices += m_tablename;
                tableIndices += "\" IS '";
                tableIndices += string2SQL(m_comment);
                tableIndices += "'";
            }
            //ALTER INDEX <index_name> [ON <table_name>] ENABLE
            //ALTER INDEX <index_name> [ON <table_name>] DISABLE
            if (m_disabled .Compare("NO") == 0 )
            {
                tableIndices += NEW_LINE;
                tableIndices += SEPARATOR;
                tableIndices += "ALTER INDEX \"";
                tableIndices += m_indexname;
                tableIndices += "\" ON \"";

                if (SAPDB_FALSE == m_owner.Empty())
                {
                    tableIndices += m_schemaname;
                    tableIndices += "\".\"";
                }
                tableIndices += m_tablename;
                tableIndices += "\" ENABLE ";
            }
            if (m_disabled .Compare("YES") == 0 )
            {
                tableIndices += NEW_LINE;
                tableIndices += SEPARATOR;
                tableIndices += "ALTER INDEX \"";
                tableIndices += m_indexname;
                tableIndices += "\" ON \"";

                if (SAPDB_FALSE == m_owner.Empty())
                {
                    tableIndices += m_schemaname;
                    tableIndices += "\".\"";
                }
                tableIndices += m_tablename;
                tableIndices += "\" DISABLE ";
            }

        }
    }
    if ( m_pTransformationCmd->trCatalogFile_ls00.fsFormat_ls00.ffoXml_ls00 == true)
         tableIndices  += genCreateXML();

    return writeLine(tableIndices);
}
/*===========================================================================*
 *     IndexDef::addColumn
 *===========================================================================*/
IndexColumnDef * IndexDef::addColumn ()
{
    /*-------------------------------------------------------------------------*
     * Create a new column object.                                             *
     *-------------------------------------------------------------------------*/
    IndexColumnDef *pIndexColDef = new IndexColumnDef( );

    /*-------------------------------------------------------------------------*
     * Append the new column object to the columns list.                       *
     *-------------------------------------------------------------------------*/
    m_IndexColumns.append(pIndexColDef);

    return pIndexColDef;
}
/*===========================================================================*
 *     IndexDef::genCreateStatement
 *===========================================================================*/
StudioOAL_String IndexDef::genCreateStatement( )
{
    ROUTINE_DBG_MEO00 ("IndexDef::genCreateStatement ()");

    StudioOAL_String  index;
    IndexColumnDef   *pCol;

    // INDEXCOLUMNS: OWNER, TABLENAME, INDEXNAME, TYPE, COLUMNNAME, SORT, COLUMNNO, DATATYPE, LEN, DISTINCTVALUES,
    //               PAGECOUNT, AVGLISTLENGTH, CREATEDATE, CREATETIME, INDEX_USED, DISABLED, COMMENT

    //<create_index_statement> ::=
    // CREATE [UNIQUE] INDEX <index_name> ON <table_name> (<column_name> [ASC | DESC],...)
    index  = "CREATE ";
    index += m_indexType;
    index += " INDEX ";
    index += "\"";
    index += m_indexname;
    index += "\"";
    index += " ON \"";

    if ( (ptAll_els00 == m_pTransformationCmd->trPartSpec_ls00.psPartType_ls00) &&
         (SAPDB_FALSE == m_owner.Empty() ) )
    {
        index += m_owner;
        index += "\".\"";
    }
    index += m_tablename;
    index += "\" (";

    SAPDB_Int4 iIndexColSize  = m_IndexColumns.size();
    SAPDB_Int4 iIndexColCount = 0;

    // CREATE [UNIQUE] INDEX <index_name> ON <table_name> (<column_name> [ASC | DESC],...)  //!! ToDo
    for ( pCol = (IndexColumnDef *)m_IndexColumns.first();pCol; pCol = (IndexColumnDef *)m_IndexColumns.next())
    {
        iIndexColCount++;

        index += pCol->genCreateStatement ();

        if ( iIndexColCount != iIndexColSize ) {
            index += ",";
        }
        else {
            index += " )";
        }
    }

    return index;
}
/*===========================================================================*
 *     IndexDef::genCreateXML()
 *===========================================================================*/
StudioOAL_String IndexDef::genCreateXML()
{
   ROUTINE_DBG_MEO00 ("IndexDef::genCreateXML()");

   StudioOAL_String  index;

   IndexColumnDef  *pCol;

   // INDEXCOLUMNS : OWNER,TABLENAME,INDEXNAME,TYPE,COLUMNNAME,SORT,COLUMNNO,DATATYPE,LEN,DISTINCTVALUES,
   //                PAGECOUNT,AVGLISTLENGTH,CREATEDATE,CREATETIME,INDEX_USED, DISABLED, COMMENT

   //<create_index_statement> ::=
   // CREATE [UNIQUE] INDEX <index_name> ON <table_name> (<column_name> [ASC | DESC],...)

   // <dbm:DeployedTableIndices>
   index  = "<dbm:Index oim:id = \"_";
   index += string2XML( m_owner );
   index += ".";
   index += string2XML( m_tablename );
   index += ".";
   index += string2XML( m_indexname );
   index += "\" oim:href = \"#_";
   index += string2XML( m_owner );
   index += ".";
   index += string2XML( m_tablename );
   index += "\" dbm:name = \"";
   index += string2XML( m_indexname );
   index += "\" dbm:comment = \"";
   m_comment.Compare( NULL_VALUE ) != 0 ? index += string2XML( m_comment ) : index += string2XML("");
   index += "\" dbm:IsUnique = \"";
   m_indexType.Compare("UNIQUE")   == 0 ? index += "1\""     : index += "0\"";
   index += ">\n";

   index +=  "<dbm:IndexIndexColumns>\n";

   for ( pCol   = (IndexColumnDef *)m_IndexColumns.first();pCol; pCol = (IndexColumnDef *)m_IndexColumns.next())
         index += pCol->genCreateXML();

   index +=  "</dbm:IndexIndexColumns>\n";
   index +=  "</dbm:Index>\n";
   // </dbm:DeployedTableIndices>

   return index;
}
/*===========================================================================*
 *    IndexColumnDefODBC::IndexColumnDefODBC
 *===========================================================================*/
IndexColumnDefODBC::IndexColumnDefODBC( sapdbwa_DBC & odbcConnection , MetaDataDef * pMetaData, tls00_TransformationCmd * pTransformationCmd  )
                   :MetaDataItem ( odbcConnection , pMetaData, pTransformationCmd )
{
}
/*===========================================================================*
 *     IndexColumnDefODBC::IndexColumnDefODBC
 *===========================================================================*/
IndexColumnDefODBC::IndexColumnDefODBC(  sapdbwa_DBC & odbcConnection )
                   :MetaDataItem ( odbcConnection )
{
}
/*===========================================================================*
 *    IndexColumnDefODBC::~IndexColumnDefODBC
 *===========================================================================*/
IndexColumnDefODBC::~IndexColumnDefODBC()
{
}
/*===========================================================================*
 *     IndexColumnDefODBC::initResultSet
 *===========================================================================*/
SAPDB_Bool IndexColumnDefODBC::initResultSet ( sapdbwa_DBC & odbcConnection )
{
  ROUTINE_DBG_MEO00 ("IndexColumnDefODBC::initResult ()");

  SAPDB_Bool isOk = SAPDB_FALSE;

  if (  odbcConnection.HDBC() && odbcConnection.HENV() )
        isOk = SAPDB_TRUE;
  return isOk;
}
/*===========================================================================*
 *    IndexColumnDef::setComment
 *===========================================================================*/
void  IndexColumnDef::setComment ( StudioOAL_String comment )
{
      m_column.setComment(comment);
}
/*===========================================================================*
 *    IndexColumnDef::genCreateStatement
 *===========================================================================*/
StudioOAL_String IndexColumnDef::genCreateStatement()
{
     StudioOAL_String  indexColumn;

     indexColumn  = "\"";
     indexColumn += m_column.name ();
     indexColumn += "\"";
     indexColumn += "  ";
     indexColumn += m_sorting;

     return indexColumn;
}
/*===========================================================================*
 *    IndexColumnDef::genCreateXML
 *===========================================================================*/
StudioOAL_String IndexColumnDef::genCreateXML()
{
     ROUTINE_DBG_MEO00 ("IndexColumnDef::genCreateXML()");
     StudioOAL_String  indexColumn;

     // <dbm:IndexIndexColumns>
     indexColumn  =  "<dbm:IndexColumn oim:id = \"_";
     indexColumn += string2XML( m_schemaname );
     indexColumn += ".";
     indexColumn += string2XML( m_tablename );
     indexColumn += ".";
     indexColumn += string2XML( m_indexname );
     indexColumn += ".";
     indexColumn += string2XML( m_column.name () );
     indexColumn += "\" oim:href = \"#_";
     indexColumn += string2XML( m_schemaname);
     indexColumn += ".";
     indexColumn += string2XML( m_tablename );
     indexColumn += ".";
     indexColumn += string2XML( m_column.name () );
     indexColumn += "\" dbm:name = \"";
     indexColumn += string2XML( m_column.name () );
     indexColumn += "\" dbm:IsAscending = \"";
     m_sorting.Compare("ASC")   == 0 ? indexColumn += "1\"" : indexColumn += "0\"";
     indexColumn += "/>\n";
     // </dbm:IndexIndexColumns>
     return indexColumn;
}

/*===========================================================================*
 *    ErrorHndl::ErrorHndl()
 *===========================================================================*/
ErrorHndl::ErrorHndl()
{
    m_isError    = SAPDB_FALSE;
    m_NativeError= 0;
    m_ErrorPos   = 0;
    m_ReturnCode = 0;
}
/*===========================================================================*
 *    ~ErrorHndl::ErrorHndl()
 *===========================================================================*/
ErrorHndl::~ErrorHndl()
{
}
/*===========================================================================*
 *     END
 *===========================================================================*/
