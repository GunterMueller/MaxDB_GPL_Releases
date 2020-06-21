
#ifndef HLS31_ODBC_RESULT_H
#define HLS31_ODBC_RESULT_H

/*!**********************************************************************

  module: hls31_odbc_Result.h

  -----------------------------------------------------------------------

  responsible:  d033893 and SteffenS

  special area: odbc SAP DB LOADER
  description:  
  version:      7.6.

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

// includes
#include "gls00.h"
#include "hls98msg.h"
// ConnectionSet
#include "hwd07dbc.h"      // sapdbwa_DBCPool     sapdbwa_DBC
// StatementAttribute
#include "gpr00.h"
// ResultSet
#include "SAPDB/SQLStudio/OAL/StudioOAL_WResult.hpp"  
// Table
#include "SQLStudio/TableDefinition/StudioTD_TableDef.h"
// GUID_
#include "SAPDB/RunTime/RTE_UniqueId.h"

// forward declarations

class ResultSet          ;

class GUID_           ;

// classes derived from ResultSet
class MetaDataItem        ;
class TransformationDef   ;
class RelationalDef       ;
class UserConnectionDef   ;
class UserDef             ;
class RoleDef             ;
class SchemaDef           ;
class TableDef            ;
class ViewDef             ;
class SynonymDef          ;
class FunctionDef         ;
class TriggerDef          ;
class ProcedureDef        ;
class GrantProcedureDef   ;
class IndexDef            ;
class IndexColumnDefODBC  ;
class IndexColumnDef      ;
class ColumnTypeDef       ;
class SequenceValueDef    ;
class SequenceDef         ;
class ErrorHndl             ;

typedef struct definfo_ls31
{
    tsp00_Uint2       colnum;
    tsp00_Uint2       deflen;       // 1 Byte if type is a function, otherwise string length of user defined value
    tsp7_default_Enum deftype;      // 4 Byte value
    StudioOAL_String  defvalue;     // filled only if deftype = sp7_other_default
} definfo_ls31;

typedef enum
{
	    upd_Transformation_800   = 800,
        upd_Transformation_801,upd_Transformation_802,upd_Transformation_803,upd_Transformation_804,
        upd_Transformation_805,upd_Transformation_806,upd_Transformation_807,upd_Transformation_808,
        upd_Transformation_809,upd_Transformation_810,upd_Transformation_811,upd_Transformation_812,
		upd_Transformation_813,
        sel_Relational_1           = 1,
        sel_Relational_2 ,sel_Relational_3 ,sel_Relational_4 ,sel_Relational_5 ,sel_Relational_6,
        sel_Relational_7 ,sel_Relational_8 ,sel_Relational_9 ,sel_Relational_10,sel_Relational_11,
		sel_Relational_12,sel_Relational_13,sel_Relational_14,sel_Relational_15,sel_Relational_16,
		sel_Relational_17,
        sel_Relational_18,
        exist_Transformation_100 = 100,
        exist_Transformation_101,exist_Transformation_102,exist_Transformation_103,exist_Transformation_104,
        exist_Transformation_105,exist_Transformation_106,exist_Transformation_107,exist_Transformation_108,
        exist_Transformation_109,exist_Transformation_110,exist_Transformation_111,exist_Transformation_112,
        exist_Transformation_113,
        drop_Transformation_200  = 200, 
        drop_Transformation_201, 
        ddl_Transformation_300   = 300,
		ddl_Transformation_301,ddl_Transformation_302,call_Transformation_303,ddl_Transformation_304,
		ddl_Transformation_305,ddl_Transformation_306,ddl_Transformation_307,ddl_Transformation_308,
        ddl_Transformation_309,ddl_Transformation_310,ddl_Transformation_311,ddl_Transformation_312,
        ddl_Transformation_313,ddl_Transformation_314,ddl_Transformation_315,ddl_Transformation_316,
		trans_Transformation_316,trans_Transformation_317,trans_Transformation_318,
		dal_Transformation_400   = 400,
        dal_Transformation_401,dal_Transformation_402,dal_Transformation_403,dal_Transformation_404,
        dal_Transformation_405,dal_Transformation_406,dal_Transformation_407,dal_Transformation_408,
        ins_Transformation_500   = 500,
        ins_Transformation_501,ins_Transformation_502,ins_Transformation_503,ins_Transformation_504,
		ins_Transformation_505,ins_Transformation_506,ins_Transformation_507,
        del_Transformation_600   = 600,
        del_Transformation_601,del_Transformation_602,del_Transformation_603,del_Transformation_604,
        del_Transformation_605,
        sel_Transformation_700   = 700,
        sel_Transformation_701,sel_Transformation_702,sel_Transformation_703,sel_Transformation_704,
        sel_Transformation_705,sel_Transformation_706,sel_Transformation_707,sel_Transformation_708,
        sel_Transformation_709,sel_Transformation_710,sel_Transformation_711,sel_Transformation_712,
        sel_Transformation_713,sel_Transformation_714,sel_Transformation_715,sel_Transformation_716,
		sel_Transformation_717,sel_Transformation_718,sel_Transformation_719,sel_Transformation_720,
        sel_Transformation_721,sel_Transformation_722,sel_Transformation_723,sel_Transformation_724,
		stmt_EndOfArray_925      = 900
} StatementNumbers;

static RTE_UniqueId  hIdManagement;
static SAPDB_Bool    idManagementInitialized = SAPDB_FALSE;

#define FILE_EXTENSION_SIZE_LS31     4

#define ID_INITIAL_STRING            "000000000000000000000000000000000000000000000000"
#define ID_INITIAL                   "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"

#define ID_ROOT                      ID_INITIAL
#define ID_ROOT_STRING               ID_INITIAL_STRING

#define GUID_MAX_ID_LEN              24
#define GUID_MAX_ID_STRING_LEN       2*GUID_MAX_ID_LEN

typedef char                         GUID_Id[GUID_MAX_ID_LEN];
typedef char                         GUID_IdString[GUID_MAX_ID_STRING_LEN+1];

/*===========================================================================*
 *    class GUID_
 *===========================================================================*/
class GUID_ 
{
protected:
    GUID_();
public:
    static GUID_ *Instance();

    ~GUID_();
    SAPDB_Bool Id_Init               ( );
    SAPDB_Bool IdGetNext             ( GUID_Id id );
    void       IdAsString            ( GUID_Id  id , GUID_IdString idString );
    void       IdInitValue           ( GUID_Id  id );
    void       IdStringInitValue     ( GUID_IdString idString );
    SAPDB_Bool IdIsInitialValue      ( GUID_Id id );
    SAPDB_Bool IdStringIsInitialValue( GUID_IdString idString );
    SAPDB_Bool IdsAreIdentical       ( GUID_Id id1, GUID_Id id2 );
    SAPDB_Bool IdStringsAreIdentical ( GUID_IdString idString1, GUID_IdString idString2 );

private:
    static GUID_  *_guidinstance;

};

/*===========================================================================*
 *    class MetaDataDef 
 *===========================================================================*/
class MetaDataDef 
{
public:
	 MetaDataDef( tls00_TransformationCmd *pTransformationCmd , tls00_DBInfo * pDBInfo  );
    ~MetaDataDef();

	SAPDB_Int4       initResultSets            ( tls00_TransformationCmd *pTransformationCmd , tls00_DBInfo * pDBInfo, tsp00_Addr ErrText );

    // Transformation and Relational Model
    StudioOAL_String    Statement              (const StatementNumbers StatementNumber);
    TransformationDef * getTransformationModel () { return m_pTransformationDef;}
	RelationalDef     * getRelationalModel     () { return m_pRelationalDef;}

    // database connectionset
    void             setConnection        ( tls00_DBInfo *pDBInfo ) { m_pDBInfo   = pDBInfo; }
    tls00_DBInfo*    getConnection        (){ return m_pDBInfo; }
    SAPDB_Bool       setODBCDriver        ( tls00_DBInfo *pDBSession     );
    StudioOAL_String getOdbcDriver        (){ return m_odbcDriver    ; }

    // stream connection
    void            setMedium             ( tls00_FileInfo  &FileSession ){ m_FileSession = FileSession ;}
    tls00_FileInfo  getMedium             (){ return m_FileSession ;}
    SAPDB_Bool      readLine              ( StudioOAL_String &record  );
    SAPDB_Bool      writeLine             ( StudioOAL_String&  record  );

	// Error Handling
	void             setMaxErrorCount     (SAPDB_Int4 lMaxErrorCount){m_catMaxAdmissibleErrors = lMaxErrorCount;}
    SAPDB_Int4       getMaxErrorCount     (){return m_pDBInfo->dbiSession_ls00.siMaxAdmissibleErrors;}
    ErrorHndl *      getError             (){return m_pError; }
	ErrorHndl      * m_pError;

private:
	// Transformation and Relational Model
    TransformationDef    *m_pTransformationDef;
	RelationalDef        *m_pRelationalDef;
    UserConnectionDef    *m_pUserConnectionDef;
    // database connectionset for metadata
    StudioOAL_String      m_odbcDriver;
    // database connection for data
    tls00_DBInfo         *m_pDBInfo;
    SAPDB_Char            m_ReadWriteErrText[MAX_REPLY_LENGTH_LS00];
    SAPDB_Char            m_ReadBuffer[LINELENGTH_MXSP00];
    // stream connection for metadata and data
    tls00_FileInfo        m_FileSession;

public: 
    void              setSqlMode         ( tpr00_SQLModeEnum catSqlMode ){ m_catSqlMode = catSqlMode;}
    tpr00_SQLModeEnum getSqlMode         (){ return m_catSqlMode;}
    tpr00_SQLModeEnum m_catSqlMode;
    SAPDB_Int4        m_catMaxAdmissibleErrors;
};
/*===========================================================================*
 *    class ResultSet
 *===========================================================================*/
class ResultSet :  public StudioOAL_WResult
{
public:
    ResultSet ( sapdbwa_DBC & odbcConnection ); 
    virtual ~ResultSet ();
};
/*===========================================================================*
 *    class MetaDataItem 
 *===========================================================================*/
class MetaDataItem : public ResultSet
{
public:
    MetaDataItem( sapdbwa_DBC & odbcConnection, MetaDataDef * pMetaData ,tls00_TransformationCmd *pTransformationCmd );
    MetaDataItem( sapdbwa_DBC & odbcConnection );
    
    virtual ~MetaDataItem();

    void            setMetaData  ( MetaDataDef * pMetaData ){ m_pMetaDataDef = pMetaData;}
    MetaDataDef   * getMetaData(){ return m_pMetaDataDef  ;}
        
    virtual SAPDB_Bool initResultSet ( sapdbwa_DBC & odbcConnection ) = 0;

    StudioOAL_String Statement       ( const StatementNumbers StatementNumber);
    SAPDB_Bool executeSQL            ( StudioOAL_String& statement, SAPDB_Bool internal = false);
    SAPDB_Bool executeSQL            ( const char*       statement, SAPDB_Bool internal = false);
    SAPDB_Bool readResult            ( StudioOAL_String* record   );
    SAPDB_Bool writeLine             ( StudioOAL_String& record  );

	SAPDB_Bool isSchema()            { return m_bSchema   ;}
    SAPDB_Bool isSchemaMap()         { return m_bSchemaMap;}
    SAPDB_Int4 existsSchemaMapSchemas( StudioOAL_String schemamap  , tsp00_Addr ErrText  );
    SAPDB_Int4 existsSchemaMapDoubles( StudioOAL_String packageguid, StudioOAL_String schemamap,  tsp00_Addr ErrText );
    SAPDB_Int4 existsSchemaMap       ( StudioOAL_String schemamap  , tsp00_Addr ErrText );
    SAPDB_Int4 existsSchemaPrivileges( StudioOAL_String username   , StudioOAL_String schemaname, tsp00_Addr ErrText );
	SAPDB_Int4 existsSchema          ( StudioOAL_String schemaname , tsp00_Addr ErrText );
    SAPDB_Bool existsTable           ( StudioOAL_String tablename  );
    SAPDB_Bool existsColumn          ( StudioOAL_String& tablename , StudioOAL_String& columnname);
    SAPDB_Bool existsProc            ( StudioOAL_String dbprocname );
    
    StudioOAL_String  string2XML     ( StudioOAL_String  record  );
    StudioOAL_String& string2SQL     ( StudioOAL_String& record  );

private:
    MetaDataDef      *m_pMetaDataDef;

protected:
	tls00_TransformationCmd *m_pTransformationCmd;
	SAPDB_Bool               m_bSchema;
    SAPDB_Bool               m_bSchemaMap;

};

/*===========================================================================*
 *    class TransformationDef 
 *===========================================================================*/
class TransformationDef : public MetaDataItem
{
public:
   TransformationDef( sapdbwa_DBC & odbcConnection , MetaDataDef * pMetaData,  tls00_TransformationCmd *pTransformationCmd );
   TransformationDef( sapdbwa_DBC & odbcConnection  );
  ~TransformationDef();

   // 1.Direct SQL Statement
   SAPDB_Bool initResultSet              ( sapdbwa_DBC  &odbcConnection );
   SAPDB_Bool ExistPackage               ( StudioOAL_String  packageguid );
   SAPDB_Bool ExistPackageData           ( StudioOAL_String  packageguid );
   SAPDB_Int4 ExportPackage              ( MetaDataDef *&pMetaDataDef, tsp00_Addr ErrText);
   SAPDB_Int4 ImportPackage              ( MetaDataDef *&pMetaDataDef, tsp00_Addr ErrText);
   SAPDB_Bool ImportPackage              ( StudioOAL_String& packagestmt );
   SAPDB_Bool ImportTransformationTables ( );
   SAPDB_Int4 ExportStatistic            ( );
   SAPDB_Int4 ImportStatistic            ( );
   SAPDB_Int4 ImportSchemaMap            ( tsp00_Addr ErrText);
   SAPDB_Int4 ExportSchemaMap            ( tsp00_Addr ErrText);


   SAPDB_Bool readResult                 ( StudioOAL_String *record );
   SAPDB_Bool writeResult                ( StudioOAL_String  record );
   void             setSQLMode           ( StudioOAL_String& record );
   StudioOAL_String getSQLMode           ( ) {return m_sqlmode; }

   tls00_TransformationCmd *             getTransformationCmd() ;
   SAPDB_Bool initTransformationStep     ( );
   void       setTransformationStmt      ( StudioOAL_String statement ) { m_statement = statement;}
   SAPDB_Int4 initTransformationCmd      ( tls00_ScanResult Type );
   void       freeTransformationCmd      ( tls00_ScanResult Type );
   SAPDB_Int4 initTransformationCmd      ( );
   void       freeTransformationCmd      ( );
   void       genTransformationStmt      ( );
   void       genPreparedStmt            ( );
   StudioOAL_String getPreparedSelect    ( ) { return m_preparedSelect; }
   StudioOAL_String getPreparedInsert    ( ) { return m_preparedInsert; }
                                       
   tsp00_Int4 switchSchemaMap            ( StudioOAL_String currentSchemaMap, tsp00_Addr ErrText);
   tsp00_Int4 switchUserSchema           ( StudioOAL_String currentUser, StudioOAL_String currentSchema, bool silent = false, tsp00_Addr ErrText = NULL );
   StudioOAL_String currentUser          ( ){ return m_owner;}
   StudioOAL_String currentSchema        ( ){ return m_schemaname;}
   SAPDB_Int4 switchUser                 ( StudioOAL_String previousUser , StudioOAL_String currentUser, tsp00_Addr ErrText );
   SAPDB_Int4 switchUser                 ( StudioOAL_String previousUser , tsp00_Addr ErrText );
                                       
                                       
   SAPDB_Int4 getObjectCount()           { return  m_nObjectCount; }
                                       
   StudioOAL_String Statement            (const StatementNumbers StatementNumber);
                                       
   SAPDB_Bool update                     ( StudioOAL_String statement );
   SAPDB_Bool updateTaskStatistic        ( );
   SAPDB_Bool updateExclude              ( );
   SAPDB_Bool updateCatalogCmd           ( );
   SAPDB_Bool updateCatalog              ( );
   SAPDB_Bool updateDataCmd              ( );
   SAPDB_Bool updateHeader               ( );
   SAPDB_Bool updateDataStream           ( );
   SAPDB_Bool updateTrailer              ( );
   SAPDB_Bool updateBegin                ( );
   SAPDB_Bool updateEnd                  ( );
   SAPDB_Bool updateRestart              ( );
   SAPDB_Bool updatePerformance          ( );
   SAPDB_Bool updateExportData           ( );
   SAPDB_Bool updateImportData           ( );
   
   // transformation statement and transformation command
   tls00_TransformationCmd *m_pTransformationCmd;
   _TCHAR                   m_pTransformationStmt[1024];
   _TCHAR                   m_pRelationalStmt    [1024];
   tls00_Command           *m_pLoaderCmd;
   StudioOAL_String         m_statement;
   StudioOAL_String         m_preparedSelect;
   StudioOAL_String         m_preparedInsert;
   StudioOAL_String         m_StmtString;      // used for temp stmt generation

   // sql stament
   
   // transformationpackage
   SAPDB_Bool        m_existPackage;
   SAPDB_Bool        m_existPackageData;
   tls00_StreamType  m_currentStream;

   // transformationpackage table columns for catalog
   SAPDB_Int4        m_nTaskID;
   SAPDB_Int4        m_nSchemaCount;
   SAPDB_Int4        m_nObjectCount;
   SAPDB_Int4        m_nTableCount;
   SAPDB_Int4        m_nTableData;
   SAPDB_Int4        m_nViewCount;
   SAPDB_Int4        m_nSynonymCount;

   // transformationpackage table columns for data
   StudioOAL_String  m_packagepart[6];
   StudioOAL_String  m_package[19]       ; 
   StudioOAL_String  m_packageid         , m_taskid    , m_stepid;
   SAPDB_Bool        m_exclude;
   StudioOAL_String  m_servernode        , m_serverdb         , m_username  , m_codetype, m_storagedevice;
   StudioOAL_String  m_owner             , m_schemaname       , m_tablename ;
   StudioOAL_String  m_schemaname_value  , m_tablename_value;
   StudioOAL_String  m_transformation    , m_partspec         , m_tabletype;
   StudioOAL_String  m_catalogstreamname , m_catalogstreamtype, m_catalogstreamebid, m_catalogstreamformat;
   StudioOAL_String  m_datastreamname    , m_datastreamtype   , m_datastreamebid   , m_datastreamformat;
   StudioOAL_String  m_headerpos_block   , m_headerpos_byte; 
   StudioOAL_String	 m_datapos_block     , m_datapos_byte;
   StudioOAL_String  m_trailerpos_block  , m_trailerpos_byte;
   StudioOAL_String  m_rows_exported     , m_rows_imported;
   tls00_LargeNumber m_headerpos         , m_datapos    , m_trailerpos;
   tsp00_Longint     m_exportcount       , m_importcount;
   StudioOAL_String  m_starttime         , m_endtime, m_restart, m_restore;
private:
   StudioOAL_String  m_upd_Transformation_800;
   StudioOAL_String  m_upd_Transformation_801;
   StudioOAL_String  m_upd_Transformation_802;
   StudioOAL_String  m_upd_Transformation_803;
   StudioOAL_String  m_upd_Transformation_804;
   StudioOAL_String  m_upd_Transformation_805;
   StudioOAL_String  m_upd_Transformation_806;
   StudioOAL_String  m_upd_Transformation_807;
   StudioOAL_String  m_upd_Transformation_808;
   StudioOAL_String  m_upd_Transformation_809;
   StudioOAL_String  m_upd_Transformation_810;
   StudioOAL_String  m_upd_Transformation_811;
   StudioOAL_String  m_upd_Transformation_812;
   StudioOAL_String  m_upd_Transformation_813;

   ResultSet        *m_pUpdate;
   StudioOAL_String  m_sqlmode;


#ifdef ODBC_PREPARED
   ResultSet        *m_pUpdateCatalog;
   ResultSet        *m_pUpdatePages;
   ResultSet        *m_pUpdateRecords;
   ResultSet        *m_pUpdateRestart;
   ResultSet        *m_pUpdateBegin;
   ResultSet        *m_pUpdateHeader;
   ResultSet        *m_pUpdateData; 
   ResultSet        *m_pUpdateEnd;
#endif // ODBC_PREPARED
};

/*===========================================================================*
 *    class RelationalDef 
 *===========================================================================*/
class RelationalDef : public MetaDataItem
{
public:
   RelationalDef( sapdbwa_DBC & odbcConnection , MetaDataDef * pMetaData,  tls00_TransformationCmd *pTransformationCmd );
   RelationalDef( sapdbwa_DBC & odbcConnection  );
  ~RelationalDef();

   SAPDB_Int4 ExportPackageDb          ( StudioOAL_String& username  , tsp00_Addr ErrText );
   SAPDB_Int4 ExportPackageUser        ( StudioOAL_String& username  , tsp00_Addr ErrText );
   SAPDB_Int4 ExportPackageSchema      ( StudioOAL_String& schemaname, tsp00_Addr ErrText );
   SAPDB_Int4 ExportPackageTable       ( );

   SAPDB_Int4 ImportPackageDb          ( StudioOAL_String& username   );
   SAPDB_Int4 ImportPackageUser        ( StudioOAL_String& username   );
   SAPDB_Int4 ImportPackageSchema      ( StudioOAL_String& schemaname );
   SAPDB_Int4 ImportPackageTable       ( StudioOAL_String& schemaname );
   SAPDB_Bool initResultSet            ( sapdbwa_DBC & odbcConnection );

   // Transformation Model
   SAPDB_Bool ExistPackage             ( );
   SAPDB_Bool ExistPackageData         ( );
   void       SplitPackage             ( SAPDB_Bool splitPackage ) { m_bSplitPackage = splitPackage;}
   SAPDB_Bool SplitPackage             ( );

   // Relational Model
   SAPDB_Bool TransformHeader          ( );    

   SAPDB_Int4 TransformConnection      ( );
   SAPDB_Int4 TransformColumnTypes     ( );
   
   SAPDB_Int4 TransformUsers           ( );
   SAPDB_Int4 TransformRoles           ( );
   SAPDB_Int4 TransformSchemas         ( );
   SAPDB_Int4 TransformTables          ( );
   SAPDB_Int4 TransformTableTypes      ( );
   SAPDB_Int4 TransformForeignKeys     ( );
   SAPDB_Int4 TransformSequences       ( );
   SAPDB_Int4 TransformProcedures      ( );
   SAPDB_Int4 TransformTriggers        ( );
   SAPDB_Int4 TransformFunction        ( );
   SAPDB_Int4 TransformIndexes         ( );
    
   SAPDB_Int4 TransformTable           ( StudioOAL_String& schemaname, StudioOAL_String& tablename );
   SAPDB_Int4 TransformForeignKeys     ( StudioOAL_String  schemaname, StudioOAL_String  tablename );
   SAPDB_Int4 TransformView            ( StudioOAL_String& schemaname, StudioOAL_String& viewname  );
   SAPDB_Int4 TransformSynonym         ( StudioOAL_String  schemaname, StudioOAL_String  tablename );
   SAPDB_Int4 TransformIndexes         ( StudioOAL_String& schemaname, StudioOAL_String& tablename );
   SAPDB_Int4 TransformTrigger         ( StudioOAL_String& schemaname, StudioOAL_String& tablename );
              
   SAPDB_Int4 TransformView_           ( StudioOAL_String  schemaname, StudioOAL_String  viewname  );
   SAPDB_Int4 TransformSynonym_        ( StudioOAL_String  schemaname, StudioOAL_String  tablename );
   SAPDB_Int4 TransformIndexes_        ( StudioOAL_String  schemaname, StudioOAL_String  tablename );
   SAPDB_Int4 TransformTrigger_        ( StudioOAL_String  schemaname, StudioOAL_String  tablename );

   SAPDB_Bool TransformTrailer         ( );

   UserDef          *getUserDef        ( ) { return m_pUserDef;}
   RoleDef          *getRoleDef        ( ) { return m_pRoleDef;}
   TableDef         *getTableDef       ( ) { return m_pTableDef;}
   FunctionDef      *getFunctionDef    ( ) { return m_pFunctionDef;}
   TriggerDef       *getTriggerDef     ( ) { return m_pTriggerDef;}
   ViewDef          *getViewDef        ( ) { return m_pViewDef;}
   SynonymDef       *getSynonymDef     ( ) { return m_pSynonymDef;}
   ColumnTypeDef    *getColumnTypeDef  ( ) { return m_pColumnTypeDef;}
   IndexDef         *getIndexDef       ( ) { return m_pIndexDef;}
   SequenceDef      *getSequenceDef    ( ) { return m_pSequenceDef;}
   ProcedureDef     *getProcedureDef   ( ) { return m_pProcedureDef;}

   StudioOAL_String  m_package[7];
   StudioOAL_String  m_taskid, m_stepid;
   StudioOAL_String  m_schemaname      , m_tablename, m_tabletype, m_cmd, m_owner ;
   StudioOAL_String  m_schemaname_value, m_tablename_value ;

private:
   SAPDB_Int4        m_nTableCount;
   SAPDB_Int4        m_nViewCount;
   SAPDB_Int4        m_nSynonymCount;

   _TCHAR            m_pTransformationStmt[1024];
   _TCHAR            m_pRelationalStmt    [1024];

   SAPDB_Bool        m_bSplitPackage;
   UserDef         * m_pUserDef  ;
   RoleDef         * m_pRoleDef  ;
   SchemaDef       * m_pSchemaDef;
   TableDef        * m_pTableDef;
   FunctionDef     * m_pFunctionDef;
   TriggerDef      * m_pTriggerDef;
   ViewDef         * m_pViewDef;
   SynonymDef      * m_pSynonymDef;
   ColumnTypeDef   * m_pColumnTypeDef;
   IndexDef        * m_pIndexDef;
   SequenceDef     * m_pSequenceDef;
   ProcedureDef    * m_pProcedureDef;

}; 
/*===========================================================================*
 *    class ConnectionDef 
 *===========================================================================*/
class ConnectionDef :  public MetaDataItem
{
public:
   ConnectionDef( sapdbwa_DBC & odbcConnection, MetaDataDef * pMetaData, tls00_TransformationCmd *pTransformationCmd );
   ConnectionDef( sapdbwa_DBC & odbcConnection );
  ~ConnectionDef();

   SAPDB_Bool initResultSet ( sapdbwa_DBC & odbcConnection );
   SAPDB_Bool readResult    ( StudioOAL_String *record      );
};

/*===========================================================================*
 *    class UserDef 
 *===========================================================================*/
class UserDef :  public MetaDataItem
{
public:
   UserDef( sapdbwa_DBC & odbcConnection, MetaDataDef * pMetaData, tls00_TransformationCmd *pTransformationCmd );
   UserDef( sapdbwa_DBC & odbcConnection );
  ~UserDef();

   SAPDB_Bool initResultSet ( sapdbwa_DBC & odbcConnection );
   SAPDB_Bool readResult    ( StudioOAL_String *record );
   SAPDB_Bool writeResult   ( StudioOAL_String *record );

private:
   StudioOAL_String m_owner;
   StudioOAL_String m_groupName;
   StudioOAL_String m_userName;
   StudioOAL_String m_schemaName;
   StudioOAL_String m_userMode;
   StudioOAL_String m_permLimit;
   StudioOAL_String m_tempLimit;
   StudioOAL_String m_maxTimeOut;
   StudioOAL_String m_costWarning;
   StudioOAL_String m_costLimit;
   StudioOAL_String m_connectMode;
   StudioOAL_String m_comment;
};

/*===========================================================================*
 *    class UserConnectionDef 
 *===========================================================================*/
class UserConnectionDef :  public MetaDataItem
{
public:
   UserConnectionDef( sapdbwa_DBC & odbcConnection, MetaDataDef * pMetaData, tls00_TransformationCmd *pTransformationCmd );
   UserConnectionDef( sapdbwa_DBC & odbcConnection );
  ~UserConnectionDef();

   SAPDB_Bool initResultSet ( sapdbwa_DBC & odbcConnection );
   SAPDB_Bool writeResult   ( StudioOAL_String  record      );
private:
   StudioOAL_String  m_username;
};

/*===========================================================================*
 *    class RoleDef 
 *===========================================================================*/
class RoleDef :  public MetaDataItem 
{
public:
   RoleDef( sapdbwa_DBC & odbcConnection, MetaDataDef * pMetaData , tls00_TransformationCmd *pTransformationCmd);
   RoleDef( sapdbwa_DBC & odbcConnection );
  ~RoleDef();

   SAPDB_Bool initResultSet ( sapdbwa_DBC & odbcConnection );
   SAPDB_Bool readResult    ( StudioOAL_String *record  );
   SAPDB_Bool writeResult   ( StudioOAL_String *record  );
private:
    StudioOAL_String m_role;
    StudioOAL_String m_passwordrequired;
};

/*===========================================================================*
 *    class SchemaDef 
 *===========================================================================*/
class SchemaDef :  public MetaDataItem
{
public:
   SchemaDef( sapdbwa_DBC & odbcConnection, MetaDataDef * pMetaData, tls00_TransformationCmd *pTransformationCmd );
   SchemaDef( sapdbwa_DBC & odbcConnection );
  ~SchemaDef();

   SAPDB_Bool initResultSet ( sapdbwa_DBC & odbcConnection );
   SAPDB_Bool readResult    ( StudioOAL_String *record );
   SAPDB_Bool writeResult   ( StudioOAL_String *record );
private:
   StudioOAL_String m_owner     ;
   StudioOAL_String m_schemaname;
   StudioOAL_String m_grantor;
   StudioOAL_String m_grantee;
   StudioOAL_String m_createin;
   StudioOAL_String m_dropin;
   StudioOAL_String m_alterin;
   StudioOAL_String m_grantoption;
};

/*===========================================================================*
 *    class TableDef 
 *===========================================================================*/
class TableDef : public MetaDataItem ,
                 public StudioTD_TableDef      
{
public:
   TableDef( sapdbwa_DBC & odbcConnection, MetaDataDef * pMetaData, tls00_TransformationCmd *pTransformationCmd );
   TableDef( sapdbwa_DBC & odbcConnection );
  ~TableDef();

   SAPDB_Int4 Init(const StudioOAL_String &schemaname, const StudioOAL_String &tablename);
   // 1.Direct SQL Statement
   SAPDB_Int4 readTableDefinitionDirect ( StudioOAL_String &schemaname, StudioOAL_String &tablename);
   SAPDB_Bool executeSQL                ( StudioOAL_String &statement );
   SAPDB_Bool executeSQL                ( const char*      statement );
   SAPDB_Bool readResult                ( StudioOAL_String *record , SAPDB_Bool* bIsNull );
   
   // 2. Prepared SQL Statement
   SAPDB_Bool initResultSet             ( sapdbwa_DBC & odbcConnection );
   SAPDB_Bool readTableDefinition       ( StudioOAL_String schemaname, StudioOAL_String tablename);
   SAPDB_Bool writeTableDefinition( );
   SAPDB_Bool readColumnDefResult       ( StudioOAL_String *record ) ;
   SAPDB_Bool readConstraintDefResult   ( StudioOAL_String *record ) ;
   SAPDB_Bool readForeignKeyDefResult   ( StudioOAL_String *record ) ;
   SAPDB_Bool readTableAttrDefResult    ( StudioOAL_String *record ) ;
   SAPDB_Int4 readDefaultDef(); 
   SAPDB_Int4 readConstraintDef();
   SAPDB_Int4 readForeignKeyDef         ( StudioOAL_String schemaname, StudioOAL_String tablename);
   SAPDB_Int4 writeForeignKeyDef        ( StudioOAL_String schemaname, StudioOAL_String tablename);

   StudioOAL_String   genCreateStatement();
private:
	_TCHAR            m_pRelationalStmt[1024];
    SAPDB_Int4        existsDuplicateTable();

    ResultSet        *m_pColumnDefResult;     
    ResultSet        *m_pConstraintDefResult; 
    ResultSet        *m_pForeignKeyDefResult; 
    ResultSet        *m_pTableAttrDefResult;

	StudioOAL_String  m_sel_Relational_5;
	StudioOAL_String  m_sel_Relational_6;
	StudioOAL_String  m_sel_Relational_7;
	StudioOAL_String  m_sel_Relational_8;
	StudioOAL_String  m_sel_Relational_13;
	StudioOAL_String  m_sel_Relational_14;

	StudioOAL_String  m_owner;
	StudioOAL_String  m_schemaname;
    StudioOAL_String  m_tablename ;
};

/*===========================================================================*
 *    class IndexDef 
 *===========================================================================*/
class IndexDef      : public MetaDataItem
{
public:
   IndexDef( sapdbwa_DBC & odbcConnection, MetaDataDef * pMetaData, tls00_TransformationCmd *pTransformationCmd );
   IndexDef( sapdbwa_DBC & odbcConnection );
  ~IndexDef();

   // 1.Direct SQL Statement
   SAPDB_Bool readIndexDefinitionDirect    ( StudioOAL_String schemaname, StudioOAL_String tablename);
   SAPDB_Bool readIndexColDefinitionDirect ( );

   // 2. Prepared SQL Statement
   SAPDB_Bool initResultSet                ( sapdbwa_DBC & odbcConnection );
   SAPDB_Bool readIndexDefResult           ( StudioOAL_String * record );
   SAPDB_Bool readIndexColDefResult        ( StudioOAL_String * record );

   SAPDB_Bool setIndexParameters           ( StudioOAL_String schemaname, StudioOAL_String tablename );
   SAPDB_Bool readIndexDefinition          ( StudioOAL_String schemaname, StudioOAL_String tablename );
   SAPDB_Bool readIndexColDefinition       ( StudioOAL_String schemaname, StudioOAL_String tablename, StudioOAL_String indexname );
   
   SAPDB_Bool writeIndexDefinition         ( );

   StudioOAL_String      genCreateStatement( );
   StudioOAL_String      genCreateXML();
   
   IndexColumnDef       *addColumn ();
   void                  reset     ();

   IndexColumnDefODBC   *m_pIndexColDefODBC;
   StudioOAL_List        m_IndexColumns; 

private:
   ResultSet        *m_pIndexDefResult;  
   ResultSet        *m_pIndexColDefResult;

   StudioOAL_String  m_owner;
   StudioOAL_String  m_schemaname;
   StudioOAL_String  m_tablename ;
   StudioOAL_String  m_indexname;
   StudioOAL_String  m_indexType;
   StudioOAL_String  m_disabled;
   StudioOAL_String  m_comment;
};

/*===========================================================================*
 *    class IndexColumnDefODBC 
 *===========================================================================*/
class IndexColumnDefODBC : public MetaDataItem
{
public:
    IndexColumnDefODBC( sapdbwa_DBC & odbcConnection, MetaDataDef * pMetaData, tls00_TransformationCmd *pTransformationCmd );
    IndexColumnDefODBC( sapdbwa_DBC & odbcConnection );
   ~IndexColumnDefODBC();

   SAPDB_Bool initResultSet( sapdbwa_DBC & odbcConnection );

   void       setIndexDef( IndexDef *pIndexDef) { m_pIndexDef = pIndexDef;}  
   IndexDef  *getIndexDef() { return m_pIndexDef;}

   IndexDef  *m_pIndexDef;
};

/*===========================================================================*
 *    class IndexColumnDef
 *===========================================================================*/
class IndexColumnDef 
{
public:
   IndexColumnDef() {;}
   ~IndexColumnDef(){;}

   StudioOAL_String  genCreateStatement();
   StudioOAL_String  genCreateXML();

   void              setSchemaName( StudioOAL_String schemaname){m_schemaname= schemaname;}
   void              setTableName ( StudioOAL_String tableName ){m_tablename = tableName;}
   void              setIndexName ( StudioOAL_String indexName ){m_indexname = indexName;}
   void              setIndexType ( StudioOAL_String indextype ){m_indexType = indextype;}
   void              setSorting   ( StudioOAL_String sorting   ){m_sorting   = sorting  ;}
   StudioOAL_String  getSorting   ( ){ return m_sorting ;}
   void              setComment   ( StudioOAL_String comment );
   StudioOAL_String  getComment   ( ){ return m_comment ;}

   StudioTD_ColumnDef m_column;

   StudioOAL_String   m_schemaname;
   StudioOAL_String   m_tablename ;
   StudioOAL_String   m_indexname;
   StudioOAL_String   m_indexType;
   StudioOAL_String   m_sorting;
   StudioOAL_String   m_comment;
};
/*===========================================================================*
 *    class SynonymDef 
 *===========================================================================*/
class SynonymDef  :  public MetaDataItem
{
public:
   SynonymDef ( sapdbwa_DBC & odbcConnection, MetaDataDef * pMetaData, tls00_TransformationCmd *pTransformationCmd );
   SynonymDef ( sapdbwa_DBC & odbcConnection );
  ~SynonymDef ();

   // 1.Direct SQL Statement
   SAPDB_Bool readResult    ( StudioOAL_String *record );
   SAPDB_Bool writeResult   ( StudioOAL_String *record );
   // 2. Prepared SQL Statement
   
   SAPDB_Bool initResultSet        ( sapdbwa_DBC & odbcConnection );

   SAPDB_Bool readSynonymDefinition( StudioOAL_String schemaname, StudioOAL_String tablename );
   SAPDB_Bool writeSynonymDefinition( );
   SAPDB_Bool readSynonymDefResult ( StudioOAL_String *record ) ;

   StudioOAL_String  genCreateXML ( );
   StudioOAL_String  genCreateDDL ( );

private:
   ResultSet*        m_pSynonymDefResult ;
   StudioOAL_String  m_synonymDefinition;

   StudioOAL_String  m_owner;
   StudioOAL_String  m_schemaname;
   StudioOAL_String  m_synonymname ;
   StudioOAL_String  m_tableowner;
   StudioOAL_String  m_tableschemaname;
   StudioOAL_String  m_tablename;
   StudioOAL_String  m_public;
   StudioOAL_String  m_comment;

};
/*===========================================================================*
 *    class ViewDef 
 *===========================================================================*/
class ViewDef :  public MetaDataItem
{
public:
   ViewDef( sapdbwa_DBC & odbcConnection, MetaDataDef * pMetaData, tls00_TransformationCmd *pTransformationCmd );
   ViewDef( sapdbwa_DBC & odbcConnection );
  ~ViewDef();

   // 1.Direct SQL Statement
   SAPDB_Bool readResult    ( StudioOAL_String *record );
   SAPDB_Bool writeResult   ( StudioOAL_String *record );

   // 2. Prepared SQL Statement
   SAPDB_Bool initResultSet      ( sapdbwa_DBC & odbcConnection );

   SAPDB_Bool readViewDefinition ( StudioOAL_String schemaname, StudioOAL_String tablename );
   SAPDB_Bool writeViewDefinition( );
   SAPDB_Bool readViewDefResult  ( StudioOAL_String *record ) ;

   StudioOAL_String  genCreateXML ( );

private:
   ResultSet*        m_pViewDefResult ;
   StudioOAL_String  m_owner;
   StudioOAL_String  m_schemaname;
   StudioOAL_String  m_viewName ;
   StudioOAL_String  m_viewDefinition;

};
/*===========================================================================*
 *    class TriggerDef 
 *===========================================================================*/
class TriggerDef      : public MetaDataItem
{
public:
   TriggerDef( sapdbwa_DBC & odbcConnection, MetaDataDef * pMetaData, tls00_TransformationCmd *pTransformationCmd );
   TriggerDef( sapdbwa_DBC & odbcConnection );
  ~TriggerDef();

   // 1.Direct SQL Statement
   SAPDB_Bool readResult    (  StudioOAL_String *record );
   SAPDB_Bool writeResult   (  StudioOAL_String *record );

   // 2. Prepared SQL Statement
   SAPDB_Bool initResultSet ( sapdbwa_DBC & odbcConnection );

   SAPDB_Bool readTriggerDefinition ( StudioOAL_String owner, StudioOAL_String tablename );
   SAPDB_Bool writeTriggerDefinition();
   SAPDB_Bool readTriggerDefResult  ( StudioOAL_String *record ) ;
   void              reset();
      
   StudioOAL_String  genCreateXML();
   StudioOAL_String  genCreateStatement ();

private:
   ResultSet*        m_pTriggerDefResult ;
   StudioOAL_String  m_triggerName;
   StudioOAL_String  m_owner;
   StudioOAL_String  m_schemaname ;
   StudioOAL_String  m_triggerDefinition;

   StudioOAL_String  m_tableschemaname ;
   StudioOAL_String  m_tablename ;
   StudioOAL_String  m_comment;
};
/*===========================================================================*
 *    class FunctionDef 
 *===========================================================================*/
class FunctionDef  : public MetaDataItem
{
public:
   FunctionDef( sapdbwa_DBC & odbcConnection, MetaDataDef * pMetaData, tls00_TransformationCmd *pTransformationCmd );
   FunctionDef( sapdbwa_DBC & odbcConnection );
  ~FunctionDef();
   // 1.Direct SQL Statement
   SAPDB_Bool readResult        ( StudioOAL_String *record );
   SAPDB_Bool writeResult       ( );
   // 2. Prepared SQL Statement
   SAPDB_Bool initResultSet     ( sapdbwa_DBC & odbcConnection );
  
   StudioOAL_String        genCreateXML();

private:
   StudioOAL_String       m_owner;
   StudioOAL_String       m_schemaname;
   StudioOAL_String       m_dbfunctionName;
   StudioOAL_String       m_dbfunctionDefinition;
   StudioOAL_String       m_comment;
};
/*===========================================================================*
 *    class ProcedureDef 
 *===========================================================================*/
class ProcedureDef  : public MetaDataItem
{
public:
   ProcedureDef( sapdbwa_DBC & odbcConnection, MetaDataDef * pMetaData, tls00_TransformationCmd *pTransformationCmd );
   ProcedureDef( sapdbwa_DBC & odbcConnection );
  ~ProcedureDef();
   // 1.Direct SQL Statement
   SAPDB_Bool readResult        ( StudioOAL_String *record );
   SAPDB_Bool writeResult       ( );
   // 2. Prepared SQL Statement
   SAPDB_Bool initResultSet     ( sapdbwa_DBC & odbcConnection );
  
   StudioOAL_String        genCreateXML();

   SAPDB_Int4 grantDBProcedureExtrCmd  ( StudioOAL_String dbprocedureName );
   GrantProcedureDef  * m_pGrantProcedureDef;
 
private:
   StudioOAL_String       m_owner;
   StudioOAL_String       m_schemaname;
   StudioOAL_String       m_dbprocedureName;
   StudioOAL_String       m_dbprocedureDefinition;
   StudioOAL_String       m_comment;
};
/*===========================================================================*
 *    class GrantProcedureDef 
 *===========================================================================*/
class GrantProcedureDef : public MetaDataItem
{
public:
   GrantProcedureDef( sapdbwa_DBC & odbcConnection, MetaDataDef * pMetaData, tls00_TransformationCmd *pTransformationCmd );
   GrantProcedureDef( sapdbwa_DBC & odbcConnection );
  ~GrantProcedureDef();
   // 1.Direct SQL Statement
   SAPDB_Bool readResult    ( StudioOAL_String *record );
   SAPDB_Bool writeResult   ( StudioOAL_String  record );
   // 2. Prepared SQL Statement
   SAPDB_Bool initResultSet ( sapdbwa_DBC & odbcConnection );
   
private:
   StudioOAL_String        m_grantdbprocedureDefinition;
};
/*===========================================================================*
 *    class ColumnTypeDef
 *===========================================================================*/
class ColumnTypeDef : public MetaDataItem
{
public:
    ColumnTypeDef( sapdbwa_DBC & odbcConnection, MetaDataDef * pMetaData, tls00_TransformationCmd *pTransformationCmd );
    ColumnTypeDef( sapdbwa_DBC & odbcConnection );
   ~ColumnTypeDef();

   SAPDB_Bool initResultSet      ( sapdbwa_DBC & odbcConnection );
   SAPDB_Bool readResult         ( StudioOAL_String *record );
   SAPDB_Bool writeResult        ( StudioOAL_String *record );

private:
   StudioOAL_String m_typeName;
   StudioOAL_String m_dataType;
   StudioOAL_String m_precision;
   StudioOAL_String m_literalPrefix;
   StudioOAL_String m_literalSuffix;
   StudioOAL_String m_createParams;
   StudioOAL_String m_isNullable;
   StudioOAL_String m_isCaseSensitive;
   StudioOAL_String m_isSearchable;
   StudioOAL_String m_unsignedAttribute;
   StudioOAL_String m_money;
   StudioOAL_String m_autoIncrement;
   StudioOAL_String m_localTypeName;
   StudioOAL_String m_minimumScale;
   StudioOAL_String m_maximumScale;
};
/*===========================================================================*
 *    class SequenceDef 
 *===========================================================================*/
class SequenceDef :  public MetaDataItem
{
public:
   SequenceDef( sapdbwa_DBC & odbcConnection, MetaDataDef * pMetaData, tls00_TransformationCmd *pTransformationCmd );
   SequenceDef( sapdbwa_DBC & odbcConnection );
  ~SequenceDef();
 
   SAPDB_Bool initResultSet ( sapdbwa_DBC & odbcConnection );
   SAPDB_Bool readResult    ( StudioOAL_String *record  );
   SAPDB_Bool writeResult   ( StudioOAL_String *record  );

   SequenceValueDef * m_pSequenceValueDef;
   ResultSet        * m_pGrantSequenceDef;              

private:
   StudioOAL_String m_sequencename;
   StudioOAL_String m_owner;
   StudioOAL_String m_incrementby;
   StudioOAL_String m_lastnumber;
   StudioOAL_String m_nextvalue;
   StudioOAL_String m_maxvalue;
   StudioOAL_String m_minvalue;
   StudioOAL_String m_cycleflag;
   StudioOAL_String m_cachesize;
   StudioOAL_String m_orderflag;
};

/*===========================================================================*
 *    class SequenceValueDef
 *===========================================================================*/
class SequenceValueDef : public MetaDataItem
{
public:
    SequenceValueDef( sapdbwa_DBC & odbcConnection, MetaDataDef * pMetaData, tls00_TransformationCmd *pTransformationCmd );
    SequenceValueDef( sapdbwa_DBC & odbcConnection );
   ~SequenceValueDef();

   SAPDB_Bool initResultSet      ( sapdbwa_DBC & odbcConnection );
   SAPDB_Bool readResult         ( StudioOAL_String *record );

private:
	StudioOAL_String m_lastnumber;
};

/*===========================================================================*
 *    class ErrorHndl
 *===========================================================================*/
class ErrorHndl
{
public:
     ErrorHndl( );
    ~ErrorHndl( );

    inline void             setErrorText ( StudioOAL_String errorText ) { m_ErrorText = errorText; }
    inline StudioOAL_String getErrorText ()                             { return m_ErrorText ; }
    inline void             isError      ( SAPDB_Bool     isError )     { m_isError = isError; }
    inline SAPDB_Bool       isError      ()                             { return m_isError;}

    inline void             setReturnCode (SAPDB_Int4 lReturnCode)      { m_ReturnCode = lReturnCode;}
    inline SAPDB_Int4       getReturnCode ()                            { return m_ReturnCode;}

    inline void             setNativeError(SAPDB_Int4 lNativeError)     { m_NativeError = lNativeError;}
    inline SAPDB_Long       getNativeError()                            { return m_NativeError;}

    SAPDB_Bool       m_isError    ;
    SAPDB_Long       m_NativeError;
    SAPDB_Int4       m_ErrorPos  ;
    SQLRETURN        m_ReturnCode ;
    StudioOAL_String m_ErrorText;
    StudioOAL_String m_NativeErrorAsString;
};

#endif


/*===========================================================================*
 *     End
 *===========================================================================*/