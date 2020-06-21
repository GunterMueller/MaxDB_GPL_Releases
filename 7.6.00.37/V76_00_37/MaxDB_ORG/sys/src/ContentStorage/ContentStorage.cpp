/*



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
#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif 

#ifdef WIN32
#define ZStringImport
#define OptionsImport
#endif

#include <ctype.h>
#include <string.h>
#include <stdio.h>

#include "ContentStorage/ZString.h"
#include "ContentStorage/Options.h"
#include "ContentStorage/ContentStorage.h"
#include "ContentStorage/autoptr.h"
// ******************************************************************************

/******************************************************************************
* changes:
*
* version 12:
*				new parameters for compression in functions StoreDoc, StoreComp, FetchDoc and FetchComp
*	build 01:	
*				meaning of parameter doCreate in functions StoreDoc and StoreComp changed:
*               if doCreate is set, insert without accepting duplicates
*	build 02:
*				in SAPCSConHdl::GetDbHdl: if connection to SAP DB fails, put SQL error message in err String
*				commit optimization in EndTransaction 
*
*	build 03:	
*				connect with isolationlevel 1 and always commit/rollback in EndTransaction
*				if a CsCompressed document or component should be stored, the stored...Len has to  
*				be incremented by one because of the TERM_BYTE
*
*
*	build 04:   in DynSQLGetData add buffer length to lvalue, if compressed lenght greater then uncompressed length
*
*	build 05:	use SQLTransact (with SQL_COMMIT or SQL_ROLLBACK) in DeleteContRep
*
* version 13:
*               Implementation of positional reading for components (fromOffset, toOffset parameters of FetchComp)
*
*               SAPCSConHdl::DbDisconnect: Lock GetDbHdl and give other threads some time to finish before
*                                        releasing all connections.
*   build 01:   remove SetCommitNecessary call, because COMMIT or ROLLBACK is always necessary
*               Prepare SEL...FOR_UPD Statement with extension WITH LOCK EXCLUSIVE
*   
*   build 02:   replace asserts by error return codes
*               initializing m_sessionTimeout in SAPCSConHdl
*               initializing returncode of DeleteContRep
*
* version 14: implmentation of append-flag in StoreComp.
*
*             Generate error in FetchComp, if SHORT_VALUE and LONG_VALUE are
*             both NULL.
*
* 16.08.1999 HBi: LOOP_COUNT to prevent infinite loops in SAPCSConHdl
*
* 09.09.1999 HBi: Error handling in FetchComp now checks comprErr.
*                 DECOMPR_BUF_SIZE instead of COMPR_BUF_SIZE in
*                 ODDecompr::AllocDecomprBuf.
*                 Included version info in DLL.
*
* 27.09.1999 HBi: Limit the number of open connections depending on DBPARAMETER
*                 MAXUSERTASKS.
*
* 12.10.1999 HBi: -CacheServer: document name length can be configured in ini-file.
*                 -Generate trace output for internal errors
*                 -Get DB version for APPEND and record size
*
* 02.12.1999 HBi: AppendComp: Wenn DB-Version < 7.2 => useAppend = false
*
*   build 01:   28.01.2000 (HoBi)
*               Allow empty servernode and map 'localhost' to '' to make use
*               use of shared memory communication with the DB kernel.
*
* version 15:   
*
* 30.03.2000 ToPf: 
*  
*   build 01:   in ContentStorage::Close: close db connection with SAPCSConHdl::DbDisconnect instead 
*               of using destructur of SAPCSConHdl. The instance of SAPCSConHdl will be possibly used in
*               ContentStorage::DeleteContRep 
*
* 14.04.2000 ToPf:
*   build 02:   use LOWER(name) in WHERE Clause of Select, Delete and Update statement  
*
* 28.04.2000 ToPf
*   build 03:   because of performance reasons don't use buid-in-function LOWER
*               in SQL Statement (table scan).
*               The caller of ContentStorage functions is now responsible for correct spelling 
*               of document and component name 
*
* 02.05.2000 HBi:
*   build 04: UNIX port
*/

#define BLOCKCOMPRESSED_FLAG '2'
#define CSCOMPRESSED_FLAG    '1'
#define UNCOMPRESSED_FLAG    '0'

// db version as returned by GteDbVersion
#define DB_VERSION_UNKNOWN 0
#define DB_VERSION_71      71
#define DB_VERSION_72      72

#define SQL_ATTR_READ_LONG_POS 3003
#define RESERVE_CONN           4  // number of user connections reserved for other applications (DBMGUI, ...)

// column numbers from select doc (see PrepareStmt)
#define DOC_LPROPERTY_COLNO         1
#define DOC_LVALUE_COLNO            2
#define DOC_SHORT_PROPERTY_COLNO    3
#define DOC_LONG_PROPERTY_COLNO     4
#define DOC_SHORT_VALUE_COLNO       5
#define DOC_LONG_VALUE_COLNO        6
#define DOC_COMPRESSED_COLNO        7

// column numbers from select comp (see PrepareStmt)
#define COMP_LVALUE_COLNO           1
#define COMP_SHORT_VALUE_COLNO      2
#define COMP_LONG_VALUE_COLNO       3
#define COMP_COMPRESSED_COLNO       4

// parameter number from insert doc (see PrepareStmt)
#define DOC_NAME_PARAMNO            1
#define DOC_LPROPERTY_PARAMNO       2
#define DOC_LVALUE_PARAMNO          3
#define DOC_SHORT_PROPERTY_PARAMNO  4
#define DOC_LONG_PROPERTY_PARAMNO   5
#define DOC_SHORT_VALUE_PARAMNO     6
#define DOC_LONG_VALUE_PARAMNO      7
#define DOC_DATEM_PARAMNO           8
#define DOC_TIMEM_PARAMNO           9
#define DOC_COMPRESSED_PARAMNO      10

// parameter number from insert doc (see PrepareStmt)
#define COMP_NAME_PARAMNO           1
#define COMP_LVALUE_PARAMNO         2
#define COMP_SHORT_VALUE_PARAMNO    3
#define COMP_LONG_VALUE_PARAMNO     4
#define COMP_COMPDATEM_PARAMNO      5
#define COMP_COMPTIMEM_PARAMNO      6
#define COMP_CONTENTTYPE_PARAMNO    7
#define COMP_COMPRESSED_PARAMNO     8

#define SUCCESS(r) ((r)==SQL_SUCCESS || (r)==SQL_SUCCESS_WITH_INFO)

// ******************************************************************************	
#define CONTREPDEF "CREATE TABLE CONTREP (CONTREPNAME VARCHAR(255), CONTREPNR INT, VERSION INT)"

/* the length of doc name may be configured in the ini-file */
#define DEFAULT_DOC_NAME_LEN 64
#define MIN_DOC_NAME_LEN     2
#define MAX_DOC_NAME_LEN     254

/* length of short_value column if name length is DEFAULT_DOC_NAME_LEN */
#define DEFAULT_DOC_SHORT_VALUE_LEN 3200
/* fix length of short_value in components table */
#define COMP_SHORT_VALUE_LEN        3600

static TableDef tableDef1(1, 
						 " (NAME VARCHAR(80) PRIMARY KEY, LPROPERTY FIXED(10), SHORT_PROPERTY VARCHAR(200) BYTE, \
						 LONG_PROPERTY LONG BYTE, LVALUE FIXED(10), SHORT_VALUE VARCHAR(3600) BYTE, \
						 LONG_VALUE LONGFILE BYTE, DATEM DATE, TIMEM TIME, COMPRESSED CHAR(1))",
						 " (NAME VARCHAR(120) PRIMARY KEY, LVALUE FIXED(10), SHORT_VALUE VARCHAR(3600) BYTE, \
						 LONG_VALUE LONGFILE BYTE, COMPDATEM DATE, COMPTIMEM TIME, CONTENTTYPE VARCHAR(128), \
						 COMPRESSED CHAR(1))");

static TableDef tableDef2(2, 
						 " (NAME VARCHAR(%d) PRIMARY KEY, LPROPERTY FIXED(10), SHORT_PROPERTY VARCHAR(512) BYTE, \
						 LONG_PROPERTY LONG BYTE, LVALUE FIXED(10), SHORT_VALUE VARCHAR(%d) BYTE, \
						 LONG_VALUE LONGFILE BYTE, DATEM DATE, TIMEM TIME, COMPRESSED CHAR(1))",
						 " (NAME VARCHAR(254) PRIMARY KEY, LVALUE FIXED(10), SHORT_VALUE VARCHAR(%d) BYTE, \
						 LONG_VALUE LONGFILE BYTE, COMPDATEM DATE, COMPTIMEM TIME, CONTENTTYPE VARCHAR(128), \
						 COMPRESSED CHAR(1))");

static TableDef* actTableDef = &tableDef2;
// ******************************************************************************
TableDef::TableDef(long iversion, const char* idocDef, const char* icompDef)
    :version(iversion), docDef(CS_CAST(char*) idocDef),compDef(CS_CAST(char*)icompDef)
{}

// ******************************************************************************

/*
      Function: DynSQLGetData
      
      description: Get the whole data from one column at once. A new bigger
                   buffer is dynamically allocated if necessary.

      arguments:
        targetValuePtrPtr [in/out]  Points to the pointer of the application-
                                    buffer. If the function returnes SQL_SUCCESS
                                    *targetValuePtrPtr points to the buffer that
                                    contains the whole data.
        (the other arguments are the same as for SQLGetData)
        
      return value: Return code from SQLGetData or SQL_ERROR for unexpected
                    return codes or memory allocation error.

      caution: targetType must be SQL_C_BINARY
*/

SQLRETURN DynSQLGetData( SQLHSTMT      hstmt,
                         SQLUSMALLINT  colNo,
                         SQLSMALLINT   targetType,
                         SQLPOINTER   *targetValuePtrPtr, /* different ! */
                         SQLLEN        bufferLength,
                         SQLLEN       *strLenOrIndiPtr );

// *******************************************************************************

/*
      Function: PosSQLGetData
      
      description: Read data from non long columns with offset. For long columns
                   the special statement attribut SQL_ATTR_READ_LONG_POS should
                   be used instead.

      arguments:
        readPos  [in]  Position (in bytes) where the reading should start.

        (the other arguments are the same as for SQLGetData)
        
      return value: Return code from SQLGetData or SQL_ERROR for unexpected
                    return codes or memory allocation error. If the column does
                    not contain readPos bytes the return code is
                    SQL_NO_DATA_FOUND. If the column contains the NULL value
                    and readPos is greater 1 then strLenOrIndiPtr is
                    SQL_NULL_DATA and the return code is SQL_NO_DATA_FOUND.

      caution: targetType must be SQL_C_BINARY */
  
SQLRETURN PosSQLGetData( SQLHSTMT      hstmt,
                         SQLUSMALLINT  colNo,
                         SQLSMALLINT   targetType,
                         SQLPOINTER    targetValuePtr,
                         SQLINTEGER    bufferLength,
                         SQLLEN       *strLenOrIndiPtr,
                         SQLLEN        readPos );

tpa110_ComprType FlagToComprType( char flag );

void UpperCase( char *str );

// *******************************************************************************

Storage::Error NewStorage( const char* name,Options* options, const ZString& homeDir,Storage*& storage,ZString& err)
{
	ContentStorage* strg = new ContentStorage();
	Storage::Error rc = Storage::FATAL_ERROR;
	if(strg)
	{
		rc = strg->Init(options,homeDir,err);
		if(rc)
		{
			delete strg;
			strg = 0;
		}
	}
	storage = strg;
	return rc;
}; // NewStorage
// ******************************************************************************
Storage::Error DeleteStorage(Storage* storage,ZString& err)
{
	delete storage;
	return Storage::OK;
}
// ******************************************************************************
ContentStorage::ContentStorage()
    : Storage(), m_Lock()
{
	m_Options		= 0;
	m_HomeDir		= 0;
	m_SAPCSConHdl   = 0;
	m_TableCheck	= false;
    m_DocNameLen    = DEFAULT_DOC_NAME_LEN;

	return;
}
// ******************************************************************************
ContentStorage::~ContentStorage()
{
	if (m_SAPCSConHdl)
		delete m_SAPCSConHdl;          
}
// ******************************************************************************
Storage::Error ContentStorage::Init(Options* options, const ZString& homeDir,ZString& err)
{
	m_Options		= options;
	m_HomeDir		= CS_CAST(ZString&) homeDir;

	m_SAPCSConHdl		= new SAPCSConHdl(options);
	if(!m_SAPCSConHdl) {
        err.SetBuf( "Init ContentStorage: new failed" );
        XTRACE( err );
		return Storage::INTERNAL_ERROR;
    };

	return Storage::OK;
}
// ******************************************************************************
Storage::Error ContentStorage::Version(ZString& version,ZString& identification,ZString& build,ZString& err)
{
	version			= CONTENT_STORAGE_VERSION;
	build			= CONTENT_STORAGE_BUILD;
	identification	= "ContentStorage";
	return OK;
}
// ******************************************************************************
Storage::Error ContentStorage::Create( const ZString& contRep, const ZString& contRepFullPath, const ZString& sectionName,ZString& err)
{
	long	sessionTimeout;
    long    maxUserSessions;
	Error   rc = OK;
    
	m_SectionName = CS_CAST(ZString&) sectionName;
	m_ContRepName = CS_CAST(ZString&) contRep;
	
    m_Lock.StartMutex();
	m_SAPCSConHdl->SetSectionName(sectionName);
	if (GetDbParam( "SESSION_TIMEOUT", sessionTimeout, err))
		m_SAPCSConHdl->SetSessionTimeout(sessionTimeout);
    /* set max. number of open datbase connections */
	if (GetDbParam( "MAXUSERTASKS", maxUserSessions, err))
		m_SAPCSConHdl->SetMaxOpenConn( (maxUserSessions > RESERVE_CONN) ?
                                       maxUserSessions-RESERVE_CONN : maxUserSessions );
	if (!m_TableCheck)
	{
        ZString nameLenStr( "" );
        long    nameLen    = 0;

		m_Options->GetOption((char*) CS_CAST(ZString&) sectionName, "DocNameLength", nameLenStr, "");
        if (strlen(nameLenStr) > 0) {
            nameLen = atoi( nameLenStr );
            if (nameLen >= MIN_DOC_NAME_LEN && nameLen <= MAX_DOC_NAME_LEN) {
                m_DocNameLen = nameLen;
            } else {
                err.Format( "Create ContentStorage Invalid name length %d", nameLen );
                return INTERNAL_ERROR;
            }; // else
        }; // if
		if((rc = TableCheck(err)) == OK)
			m_TableCheck = true;
	}

    m_Lock.EndMutex();

	return rc;
}; // Create


Storage::Error ContentStorage::SetDocNameLen( long docNameLen )
{
    if (docNameLen >= MIN_DOC_NAME_LEN && docNameLen <= MAX_DOC_NAME_LEN) {
        m_DocNameLen = docNameLen;
        return OK;
    } else {
        return INTERNAL_ERROR;
    }; // else
}; // SetDocNameLen

// ******************************************************************************
Storage::Error ContentStorage::Open( const ZString& contRep, const ZString& contRepFullPath, const ZString& sectionName,bool readOnly,ZString& err)
{
	return Create(contRep, contRepFullPath, sectionName, err);	
}
// ******************************************************************************
Storage::Error	ContentStorage::Close(ZString& err)
{
	if (m_SAPCSConHdl->DbDisconnect())
		return OK;
	else {
		err.Add("Close ContentStorage, database disconnect failed");
		return INTERNAL_ERROR;
	}

}
// ******************************************************************************
Storage::Error ContentStorage::DeleteContRep( const ZString& contRep, const ZString& contRepFullPath, const ZString& sectionName,ZString& err)
{

	RETCODE			retcode;
	DbConnection	connection;
	SQLHENV			henv;
	SQLHDBC			hdbc;
	SQLHSTMT        hstmt;
	ZString			sqlStmt;
	Error			rc = OK;
	long			contRepNr;
	long			version;
	bool			prepared;
	
	retcode = OK;
	if (GetContRepInfo(contRep, contRepNr, version, err))
	{
		if (m_SAPCSConHdl->GetDbHdl(connection, prepared, err) == false)
		{
			err.AddPrefix("DeleteContRep ContentStorage, connect error"); 
			return INTERNAL_ERROR;
		}
		if (m_SAPCSConHdl->GetOdbcHdl(connection, henv, hdbc, err) == false)
		{
			err.AddPrefix("DeleteContRep ContentStorage, GetOdbcHdl failed");
			return INTERNAL_ERROR;
		}
		if (SQLAllocStmt(hdbc, &hstmt) != SQL_SUCCESS)
		{
			m_SAPCSConHdl->PutDbHdl(connection, err);
			return INTERNAL_ERROR;
		}	
		sqlStmt.SetBuf("DROP TABLE DOCUMENTS");
		sqlStmt.Add(m_ContRepNr);
		retcode = SQLExecDirect(hstmt, (UCHAR*) (char*) sqlStmt, SQL_NTS);
		if (!SUCCESS(retcode))
		{
			err.Add("DeleteContRep ContentStorage, can not drop documents table of ContRep ");
			err.Add(CS_CAST(ZString&) contRep);
			rc = INTERNAL_ERROR;
			SQLTransact(henv, hdbc, SQL_ROLLBACK);
		}
		else
		{
			sqlStmt.SetBuf("DROP TABLE COMPONENTS");
			sqlStmt.Add(m_ContRepNr);
			retcode = SQLExecDirect(hstmt, (UCHAR*) (char*) sqlStmt, SQL_NTS);
			if (!SUCCESS(retcode))
			{
				err.Add("DeleteContRep ContentStorage, can not drop components table of ContRep ");
				err.Add(CS_CAST(ZString&) contRep);
				rc = INTERNAL_ERROR;
				SQLTransact(henv, hdbc, SQL_ROLLBACK);
			}
			else
			{
				sqlStmt.SetBuf("DELETE FROM CONTREP WHERE CONTREPNAME = '");
				sqlStmt.Add(CS_CAST(ZString&) contRep);
				sqlStmt.Add("'");
				retcode = SQLExecDirect(hstmt, (UCHAR*) (char*) sqlStmt, SQL_NTS);
				if (!SUCCESS(retcode == SQL_SUCCESS))
				{
					err.Add("DeleteContRep ContentStorage, can not delete ContRep entry of ContRep ");
					err.Add(CS_CAST(ZString&) contRep);
					rc = INTERNAL_ERROR;
					SQLTransact(henv, hdbc, SQL_ROLLBACK);
				}
				else
					SQLTransact(henv, hdbc, SQL_COMMIT);
			}  // else
		}  // else
		SQLFreeStmt(hstmt, SQL_CLOSE);
		m_SAPCSConHdl->PutDbHdl(connection, err);
	}
	return rc;
}; // DeleteContRep
// ******************************************************************************
Storage::Error ContentStorage::BeginTransaction(TransID& tid,ZString& err)
{
	bool prepared;

	if (!m_SAPCSConHdl->GetDbHdl((DbConnection&) tid, prepared, err))
	{
		err.AddPrefix("BeginTransaction ContentStorage, GetDbHdl failed: ");
		return INTERNAL_ERROR;
	}
    
	if (!prepared)
		if (!PrepareStmt((DbConnection) tid, err))
		{
			err.AddPrefix("BeginTransaction ContentStorage, PrepareStatement failed: ");
            XTRACE( err );
			return INTERNAL_ERROR;
		}
//XTRACEI1("Begin Transaction: ", (int) tid);
	return OK;
}; // BeginTransaction
// ******************************************************************************
Storage::Error ContentStorage::EndTransaction(TransID tid,bool commit,ZString& err)
{

	RETCODE		retcode;
	SQLHENV		henv;
	SQLHDBC		hdbc;
	Storage::Error rc = OK;
			
//XTRACEI1("End Transaction: ", (int) tid);
	if (m_SAPCSConHdl->GetOdbcHdl((DbConnection) tid, henv, hdbc, err))
	{
		if (commit)
			retcode = SQLTransact(henv, hdbc, SQL_COMMIT);
		else
			retcode = SQLTransact(henv, hdbc, SQL_ROLLBACK);

		if (!SUCCESS(retcode == SQL_SUCCESS))
		{
			err = "EndTransaction ContentStorage SQLTransact failed";
            XTRACE( err );
			rc = INTERNAL_ERROR;
		}
	}
	else
	{
		err.AddPrefix("EndTransaction ContentStorage, GetOdbcHdl failed");
        XTRACE( err );
		rc = INTERNAL_ERROR;
	}

	if (m_SAPCSConHdl->PutDbHdl((DbConnection) tid, err) == false)
	{
		err.AddPrefix("EndTransaction ContentStorage PutDbHdl failed");
        XTRACE( err );
		rc = INTERNAL_ERROR;
	}
	return rc;
}; // EndTransaction
// ******************************************************************************

Storage::Error ContentStorage::FetchDoc( TransID           tid,
                                         bool              forUpdate,
                                         const ZString    &docId,
                                         DataStatus       &docStatus, 
                                         bool              deCompress,
                                         char*            &property,
                                         int              &lProperty,
                                         char*            &value,
                                         int              &lValue,
                                         bool             &isCSCompressed,
                                         ZString          &err )
{
	DbConnection	connection = (DbConnection) tid;
	SQLHDBC			hdbc  = SQL_NULL_HDBC;
	SQLHSTMT        hstmt = SQL_NULL_HSTMT;
	SQLHENV			henv  = SQL_NULL_HENV;
	ZString         name;
	RETCODE         retcode = SQL_SUCCESS;
	SQLLEN          lenName = SQL_NTS;
    SQLLEN            lenLProperty = 0;
    SQLLEN            lenProperty  = 0;
    SQLLEN            lenLValue    = 0;
    SQLLEN            lenValue     = 0;
    SQLLEN            lenCompressFlag = 0;
    SQLLEN            localValueLen = 0;
    SQLLEN            localPropLen = 0;
	StmtType        stmtType;
	SQLLEN            propAllocLen = 0;
    SQLLEN            valAllocLen = 0;
    tpa111_ODDecompr2  &decomprDoc = m_SAPCSConHdl->GetDecompr( connection );
    tpa111_ODDecompr2  &decomprProp = m_SAPCSConHdl->GetDecompr2( connection );
    int             comprErr;    
    char            compressFlag = UNCOMPRESSED_FLAG;
    bool            uncompress;
    tpa110_ComprType comprType;

	err = 0;
    
    // initializing reference pointer of output values
	property = NULL;
	value = NULL;

	name = CS_CAST(ZString&) docId;
	if (forUpdate)
		stmtType = SEL_DOC_FOR_UPD;
	else
		stmtType = SEL_DOC;

	if (!m_SAPCSConHdl->GetOdbcHdl(connection, stmtType, henv, hdbc, hstmt, err))
	{
		err.AddPrefix("FetchDoc ContentStorage, GetOdbcHdl failed");
        XTRACE( err );
		return INTERNAL_ERROR;
	}
	

	SQLBindParameter( hstmt, 1, SQL_PARAM_INPUT,
                      SQL_C_BINARY, SQL_VARCHAR, 0, 0, (char*) name, 0, &lenName);
	
	SQLBindCol( hstmt, DOC_LPROPERTY_COLNO,  SQL_C_SLONG, &localPropLen, 0, &lenLProperty);
	SQLBindCol( hstmt, DOC_LVALUE_COLNO,     SQL_C_SLONG, &localValueLen, 0, &lenLValue);
	SQLBindCol( hstmt, DOC_COMPRESSED_COLNO, SQL_C_BINARY, &compressFlag, 1, &lenCompressFlag);

	retcode = SQLExecute(hstmt); 
	if (!SUCCESS(retcode))
	{
		m_SAPCSConHdl->SQLErr(err, connection, stmtType);
		err.AddPrefix("FetchDoc ContentStorage, SQLExecute ");
		SQLFreeStmt(hstmt, SQL_CLOSE);
        XTRACE( err );
		return INTERNAL_ERROR;
	}

	retcode = SQLFetch(hstmt);
	if (retcode == SQL_NO_DATA_FOUND)
	{
		value  = NULL;
		lValue = 0;
		SQLFreeStmt(hstmt, SQL_CLOSE);
		return KEY_NOT_FOUND;
	}
	else if (retcode == SQL_ERROR)
	{
		m_SAPCSConHdl->SQLErr(err, connection, stmtType);
		err.AddPrefix("FetchDoc ContentStorage, SQLFetch  ");
		SQLFreeStmt(hstmt, SQL_CLOSE);
        XTRACE( err );
		return INTERNAL_ERROR;
	}

    lProperty     = (int) localPropLen;
    lValue        = (int) localValueLen;
	propAllocLen  = (lProperty <= 0)? 1 : lProperty;
	valAllocLen   = (lValue <= 0)? 1 : lValue;

	property  = new char[propAllocLen];
	value     = new char[valAllocLen];
	if (value == NULL || property == NULL)
	{
		err = "FetchDoc ContentStorage, internal error";
		SQLFreeStmt(hstmt, SQL_CLOSE);
        XTRACE( err );
		return INTERNAL_ERROR;
	}

	if (lenCompressFlag == SQL_NULL_DATA)
		compressFlag = UNCOMPRESSED_FLAG;
    comprType = FlagToComprType( compressFlag );
    isCSCompressed = (comprType != NOT_COMPRESSED_PA110);
    uncompress = (isCSCompressed && deCompress);

    // read the property
    if (uncompress) {
        decomprProp.InitDecompr( hstmt, DOC_SHORT_PROPERTY_COLNO );
        retcode = decomprProp.GetData( property, propAllocLen, &lenProperty, &comprErr );
		if ( (lenProperty != SQL_NULL_DATA) && (lProperty != lenProperty) )
		{
			err.AddPrefix("FetchDoc ContentStorage, length of decompressed property (short) is wrong");
			SQLFreeStmt(hstmt, SQL_CLOSE);
            XTRACE( err );
			return INTERNAL_ERROR;
		}
        
    } else {
        if (isCSCompressed) {
            // lenProp is the uncompressed size. Because we dont know the compressed size
            // we use DynSQLGetData.
            retcode = DynSQLGetData( hstmt, DOC_SHORT_PROPERTY_COLNO,
                                     SQL_C_BINARY, (void**) &property, propAllocLen, &lenProperty);
            if (lenProperty != SQL_NULL_DATA) {
                if (comprType == SINGLE_BLOCK_PA110) {
                    lProperty = lenProperty-1; // return the compressed size - TERM_CHAR
                } else {
                    lProperty = lenProperty; // no term byte with block wise compression
                };
            };
        } else {
            retcode = SQLGetData( hstmt, DOC_SHORT_PROPERTY_COLNO, SQL_C_BINARY,
                                  property, propAllocLen, &lenProperty);
        };
    };
    if (lenProperty == SQL_NULL_DATA) {
        // property is in LONG column
        if (uncompress) {
            decomprProp.InitDecompr( hstmt, DOC_LONG_PROPERTY_COLNO );
            retcode = decomprProp.GetData( property, propAllocLen, &lenProperty, &comprErr );
            if ( lProperty != lenProperty )
			{
				err.AddPrefix("FetchDoc ContentStorage, length of decompressed property (long) is wrong");
				SQLFreeStmt(hstmt, SQL_CLOSE);
                XTRACE( err );
				return INTERNAL_ERROR;
			}
        } else {
            if (isCSCompressed) {
                // lenProp is the uncompressed size. Because we dont know the compressed size
                // we use DynSQLGetData.
                retcode = DynSQLGetData( hstmt, DOC_LONG_PROPERTY_COLNO,
                                         SQL_C_BINARY, (void**) &property, propAllocLen, &lenProperty);
                if (comprType == SINGLE_BLOCK_PA110) {
                    lProperty = lenProperty-1; // return the compressed size - TERM_CHAR
                } else {
                    lProperty = lenProperty;
                };
            } else { // uncompressed data
                retcode = SQLGetData( hstmt, DOC_LONG_PROPERTY_COLNO,
                                      SQL_C_BINARY, property, propAllocLen, &lenProperty);
            }; // else
        }; // else
    }; // if    
	if (!SUCCESS(retcode == SQL_SUCCESS))
	{
		m_SAPCSConHdl->SQLErr(err, connection, stmtType);
		err.AddPrefix("FetchDoc ContentStorage, SQLGetData ");
		SQLFreeStmt(hstmt, SQL_CLOSE);
        XTRACE( err );
		return INTERNAL_ERROR;
	}
    if (!uncompress && isCSCompressed) {
        // data stored in the DB has always UNKNOWN_SIZE_PA110 set in the general header
        // to make append possible. When exporting compressed data we write the
        // totalLength in the header because we need this for the import of compressed
        // data (see StoreDoc)
        pa110SetLengthInHeader( (unsigned char*) property, lProperty );
    };
    // restore null bytes at the end.
    if (!isCSCompressed && lenProperty < lProperty) {
        // ODBC has cut off some trailing null bytes
        memset( &property[lenProperty], 0, lProperty-lenProperty );
    }; // if

    // read the document
    if (uncompress) {
        decomprDoc.InitDecompr( hstmt, DOC_SHORT_VALUE_COLNO );
        retcode = decomprDoc.GetData( value, valAllocLen, &lenValue, &comprErr );
        if ( (lenValue != SQL_NULL_DATA) && (lenValue != lValue) )
		{
			err.AddPrefix("FetchDoc ContentStorage, length of decompressed document (short) is wrong");
			SQLFreeStmt(hstmt, SQL_CLOSE);
            XTRACE( err );
			return INTERNAL_ERROR;
		}
    } else {
        if (isCSCompressed) {
            // lenVal is the uncompressed size. Because we dont know the compressed size
            // we use DynSQLGetData.
            retcode = DynSQLGetData( hstmt, DOC_SHORT_VALUE_COLNO,
                                     SQL_C_BINARY, (void**) &value, valAllocLen, &lenValue);
            if (lenValue != SQL_NULL_DATA) {
                if (comprType == SINGLE_BLOCK_PA110) {
                    lValue = lenValue-1; // return the compressed size - TERM_CHAR
                } else {
                    lValue = lenValue;
                };
            };
        } else {
            retcode = SQLGetData( hstmt, DOC_SHORT_VALUE_COLNO,
                                  SQL_C_BINARY, value, valAllocLen, &lenValue);
        }; // else
    }; // else
    if (lenValue == SQL_NULL_DATA) {
        // document is in LONG column
        if (uncompress) {
            decomprDoc.InitDecompr( hstmt, DOC_LONG_VALUE_COLNO );            
            retcode = decomprDoc.GetData( value, valAllocLen, &lenValue, &comprErr );
            if( lValue != lenValue )
			{
				err.AddPrefix("FetchDoc ContentStorage, length of decompressed document (long) is wrong");
				SQLFreeStmt(hstmt, SQL_CLOSE);
                XTRACE( err );
				return INTERNAL_ERROR;
			}
        } else {
            if (isCSCompressed) {
                // lenVal is the uncompressed size. Because we dont know the compressed size
                // we use DynSQLGetData.
                retcode = DynSQLGetData( hstmt, DOC_LONG_VALUE_COLNO,
                                         SQL_C_BINARY, (void**) &value, valAllocLen, &lenValue);
                if (comprType == SINGLE_BLOCK_PA110) {
                    lValue = lenValue-1; // return the compressed size - TERM_CHAR
                } else {
                    lValue = lenValue; // no term byte with block compressed data
                };
            } else {
                retcode = SQLGetData( hstmt, DOC_LONG_VALUE_COLNO,
                                      SQL_C_BINARY, value, valAllocLen, &lenValue);
            }; // else
        };
    }; // if
    
	if (!SUCCESS(retcode))
	{
		err = "FetchDoc ContentStorage, SQLGetData ";
		m_SAPCSConHdl->SQLErr(err, connection, stmtType);
		SQLFreeStmt(hstmt, SQL_CLOSE);
        XTRACE( err );
        return INTERNAL_ERROR;
	}
	if (!uncompress && comprType == BLOCK_WISE_PA110) {
        // data stored in the DB has always UNKNOWN_SIZE_PA110 set in the general header
        // to make append possible. When exporting compressed data we write the total size
        // in the header because we need this for the import of compressed data (see StoreDoc)
        pa110SetLengthInHeader( (unsigned char*) value, lValue );
	};
    // restore null bytes at the end.
    if (!isCSCompressed && lenValue < lValue) {
        // ODBC has cut off some trailing null bytes
        memset( &value[lenValue], 0, lValue-lenValue );
    }; // if

	SQLFreeStmt(hstmt, SQL_CLOSE);
	docStatus = ONLINE;
	return OK;
}; // FetchDoc
// ******************************************************************************

Storage::Error ContentStorage::FetchComp( TransID          tid,
                                          bool             forUpdate,
                                          const ZString  & docId,
                                          const ZString  & compId,
                                          int              fromOffset,
                                          int              toOffset,
                                          DataStatus     & compStatus,
                                          bool             deCompress,
                                          char          *& value,
                                          int            & lValue,
                                          bool           & isCSCompressed,
                                          ZString        & err)
{
	DbConnection	connection = (DbConnection) tid;
	SQLHSTMT		hstmt      = SQL_NULL_HSTMT;
    SQLHDBC         hdbc       = SQL_NULL_HDBC;
	ZString			name;
	RETCODE			retcode    = SQL_SUCCESS;
	StmtType		stmtType;
    long            allocLen     = 0;     // allocated buffer length for value
    SQLLEN          strLenOrIndi = 0;     // length returned by ODBC or ODDecompr
    SQLLEN          uncompressedLen = 0;  // length of uncompressed value
	Error			status = OK;
    int             comprErr = 0;
    bool            uncompress = false;
    tpa111_ODDecompr2  &decompr = m_SAPCSConHdl->GetDecompr( connection );
    tpa110_ComprType    comprType = NOT_COMPRESSED_PA110;

	err   = 0;
	value = NULL;

	if (!(fromOffset == 0 && toOffset == -1))
	{
        if ((fromOffset > toOffset) && (toOffset != -1)) {
            err = "FetchComp ContentStorage, offset has wrong length";
            XTRACE( err );
            return INTERNAL_ERROR;
        }; // if
	}
	if (forUpdate)
		stmtType = SEL_COMP_FOR_UPD;
	else
		stmtType = SEL_COMP;

	name = CS_CAST(ZString&) docId;
	name.Add("\\");
	name.Add(CS_CAST(ZString&) compId);

    status = this->FetchCompOpenResult( tid, name, stmtType,
                                        hstmt, hdbc, uncompressedLen, comprType, err );
    isCSCompressed = (comprType != NOT_COMPRESSED_PA110);
    if (status != OK) {
        if (status == KEY_NOT_FOUND) {
        	value = NULL;
		    lValue = 0;
        }; // if

        return status;
    }; /* if */

    uncompress = (isCSCompressed && deCompress);

	if (!(fromOffset == 0 && toOffset == -1) && isCSCompressed && !uncompress) {
        // positional reading of compressed data without uncompressing
        err = "FetchComp ContentStorage, positional reading of compressed data not allowed.";
		SQLFreeStmt( hstmt, SQL_CLOSE);
        XTRACE( err );

        return INTERNAL_ERROR;
    }; // if        

    allocLen = this->GetCompAllocLen( uncompressedLen, fromOffset+1,
                                      (toOffset >= 0) ? toOffset+1 : toOffset );
    lValue = (int) allocLen; // only correct if data is uncompressed
	value = new char[allocLen];
	if (value == NULL)
	{
		XTRACE("malloc failed in FetchComp");
		err = "FetchComp ContentStorage, internal error";
		SQLFreeStmt(hstmt, SQL_CLOSE);

		return INTERNAL_ERROR;
	}

    // read short column
    if (uncompress) {
        decompr.InitDecompr( hstmt, COMP_SHORT_VALUE_COLNO );
        decompr.SetGetDataPos( fromOffset+1 );
        retcode = decompr.GetData( value, allocLen, &strLenOrIndi, &comprErr );
        if ( SUCCESS(retcode)
             && (strLenOrIndi != SQL_NULL_DATA)
             && (strLenOrIndi != allocLen) )
		{
			err.AddPrefix("FetchComp ContentStorage, length of decompressed component (short) is wrong");
			SQLFreeStmt(hstmt, SQL_CLOSE);
            XTRACE( err );

			return INTERNAL_ERROR;
		}
    } else {
        if (isCSCompressed) {
            // lenVal is the uncompressed size. Because we dont know the compressed size
            // we use DynSQLGetData.
            retcode = DynSQLGetData( hstmt, COMP_SHORT_VALUE_COLNO,
                                     SQL_C_BINARY, (void**) &value, allocLen, &strLenOrIndi );
            if (strLenOrIndi != SQL_NULL_DATA) {
                if (comprType == SINGLE_BLOCK_PA110) {
                    lValue = (int) strLenOrIndi-1;  // return the compressed size - TERM_CHAR
                } else {
                    lValue = (int) strLenOrIndi;
                }; 
            }; // if
        } else {
            // stmt option SQL_ATTR_READ_LONG_POS is only allowed for LONG data
            // so we use PosSQLGetData for positional reading
            retcode = PosSQLGetData( hstmt, COMP_SHORT_VALUE_COLNO,
                                     SQL_C_BINARY, value, allocLen, &strLenOrIndi, fromOffset+1 );
            if (strLenOrIndi != SQL_NULL_DATA) {
                if (retcode == SQL_SUCCESS) {
                    // strLenOrIndi should contain the number of bytes read
                    // it is used to fill up trailing null bytes
                    if (strLenOrIndi != SQL_NO_TOTAL) {
                        strLenOrIndi = min( strLenOrIndi, allocLen );
                    } else {
                        strLenOrIndi = allocLen;
                    }; // else
                } else if (retcode == SQL_NO_DATA_FOUND) {
                    // expect that the reason is that ODBC has cut off null bytes
                    retcode = SQL_SUCCESS;
                    strLenOrIndi = 0;
                }; // else
            }; // if
        }; // else
    }; // else
    if (strLenOrIndi == SQL_NULL_DATA) {
        // component is in LONG column
        if (uncompress) {
            decompr.InitDecompr( hstmt, COMP_LONG_VALUE_COLNO );
            decompr.SetGetDataPos( fromOffset+1 );
            retcode = decompr.GetData( value, allocLen, &strLenOrIndi, &comprErr );
            if( SUCCESS(retcode) && allocLen != strLenOrIndi )
			{	
				err.AddPrefix("FetchComp ContentStorage, length of decompressed component (long) is wrong");
				SQLFreeStmt(hstmt, SQL_CLOSE);
                XTRACE( err );

				return INTERNAL_ERROR;
			}
        } else {
            if (isCSCompressed) {
                // lenVal is the uncompressed size. Because we dont know the compressed size
                // we use DynSQLGetData.
                retcode = DynSQLGetData( hstmt, COMP_LONG_VALUE_COLNO,
                                         SQL_C_BINARY, (void**) &value, allocLen, &strLenOrIndi );
                if( SUCCESS(retcode) && strLenOrIndi == SQL_NULL_DATA )
				{		
					err.AddPrefix("FetchComp ContentStorage, component is empty");
					SQLFreeStmt(hstmt, SQL_CLOSE);
                    XTRACE( err );

					return INTERNAL_ERROR;
				}
                if (comprType == SINGLE_BLOCK_PA110) {
                    lValue = (int) strLenOrIndi-1; // return the compressed size - TERM_CHAR
                } else {
                    lValue = (int) strLenOrIndi;
                };
            } else {
                retcode = SQLSetStmtOption( hstmt, SQL_ATTR_READ_LONG_POS, fromOffset+1 );
                if (!SUCCESS(retcode)) {
                    m_SAPCSConHdl->SQLErr(err, connection, stmtType);
                    err.AddPrefix("FetchComp ContentStorage, SQLSetStmtOption(SQL_ATTR_READ_LONG_POS)");
                    SQLFreeStmt(hstmt, SQL_CLOSE);
                    XTRACE( err );

                    return INTERNAL_ERROR;
                }; // if
                retcode = SQLGetData( hstmt, COMP_LONG_VALUE_COLNO,
                                      SQL_C_BINARY, value, allocLen, &strLenOrIndi);
                if (strLenOrIndi == SQL_NO_TOTAL) {
                    // we have read allocLen bytes
                    strLenOrIndi = allocLen;
                } else if (strLenOrIndi == SQL_NULL_DATA) {
                    // the long AND the short column contain NULL
                    err = "FetchComp ContentStorage, SHORT_VALUE and LONG_VALUE are NULL.";
                    SQLFreeStmt( hstmt, SQL_CLOSE);
                    XTRACE( err );

                    return INTERNAL_ERROR;
                } else {
                    // may be less than allocLen if ODBC stripped off some null bytes
                    strLenOrIndi = min( strLenOrIndi, allocLen );
                }; // else
            }; // else
        }; // else
    }; // if
	if (!SUCCESS(retcode))
	{
        if (comprErr < 0) {
            /* compression error */
            err.Format("Compress error-code %d", comprErr );
        } else {
            /* ODBC error */
            m_SAPCSConHdl->SQLErr(err, connection, stmtType);
        }; /* else */
        err.AddPrefix("FetchComp ContentStorage, SQLGetData ");
        SQLFreeStmt(hstmt, SQL_CLOSE);
        XTRACE( err );
        return INTERNAL_ERROR;
	}; /* if */
	if (!uncompress && comprType == BLOCK_WISE_PA110) {
        // data stored in the DB has always UNKNOWN_SIZE_PA110 set in the general header
        // to make append possible. When exporting compressed data we write the total size
        // in the header because we need this for the import of compressed data (see StoreComp)
        pa110SetLengthInHeader( (unsigned char*) value, uncompressedLen );
	};
    // restore null bytes at the end.
    if (!isCSCompressed && strLenOrIndi < lValue) {
        // ODBC has cut off some trailing null bytes
        memset( &value[strLenOrIndi], 0, lValue - strLenOrIndi );
    }; // if

	SQLFreeStmt(hstmt, SQL_CLOSE);
	compStatus	= ONLINE;
	return OK;
}; // FetchComp

#define MAX_PARAMDATA_LOOP 10 // max calls to SQLParamData

// ******************************************************************************
Storage::Error ContentStorage::StoreDoc(TransID tid,
                                        const ZString& docId, 
                                        const char* property, int lProperty,
                                        const char* value,    int lValue,
                                        bool isCSCompressed,
                                        bool storeCompressed,
                                        const ZString& dateM, const ZString& timeM,
                                        ZString& err)
{	
	DbConnection	connection = (DbConnection) tid;
	SQLHDBC			hdbc  = SQL_NULL_HDBC;
	SQLHSTMT		hstmt = SQL_NULL_HSTMT;
	SQLHENV			henv  = SQL_NULL_HENV;
	ZString			name;
	RETCODE			retcode = SQL_SUCCESS;
	SQLLEN			lenName = 0;
    SQLLEN            lenNull = 0;
    SQLLEN            lenDate = 0;
    SQLLEN            lenTime = 0;
    SQLLEN          lenDataAtExec = SQL_DATA_AT_EXEC;
    int             uncompressedPropLen = 0;
    int             uncompressedDocLen  = 0;
	StmtType		stmtType;
    int             docBytesRead  = 0;
    int             propBytesRead = 0;
    int             storedDocLen  = 0;
    int             storedPropLen = 0;
    int             comprErr      = 0;
    tsp00_Bool      docFinished = true;
    tsp00_Bool      propFinished = true;
    tpa111_ODCompr2  &comprDoc = m_SAPCSConHdl->GetCompr( connection );
    tpa111_ODCompr2  &comprProp = m_SAPCSConHdl->GetCompr2( connection );
    const tsp00_Bool useCompression = !isCSCompressed && storeCompressed;
    // we store documents always in "new" block-wised compressed format
    char            compressFlag = storeCompressed ?
                                     BLOCKCOMPRESSED_FLAG : UNCOMPRESSED_FLAG;
    SQLLEN          lenCompressFlag = 1;

    long            docValLen  = 0; // column length for short_value
    long            docPropLen = 0; // column length for short_property
    long            docNameLen = 0; // column length for name

    long            boundPropLen = 0;
    long            boundDocLen  = 0;

    tpa110_ComprType  docComprType = BLOCK_WISE_PA110;
    tpa110_ComprType  propComprType = BLOCK_WISE_PA110;

    // these header buffers are only needed if data comes SINGLE_BLOCK_COMPRESSED
    unsigned char   docHeader[GENERAL_HEADER_SIZE_PA110+OWN_HEADER_SIZE_PA110];
    unsigned char   propHeader[GENERAL_HEADER_SIZE_PA110+OWN_HEADER_SIZE_PA110];

	if (isCSCompressed && !storeCompressed)
	{
		err.Add("StoreDoc ContentStorage, decompressed storage not implemented");
		return INTERNAL_ERROR;
	}
	
	stmtType = INS_DOC;
	name = CS_CAST(ZString&) docId;
	lenName = name.GetSize();
	lenDate = SQL_NTS;
	lenTime = SQL_NTS;
	lenNull = SQL_NULL_DATA;
    SQLPOINTER nextParm; /* indicates no of next input parameter */
    
	if(!m_SAPCSConHdl->GetOdbcHdl(connection, INS_DOC, henv, hdbc, hstmt, err))
	{
		err.AddPrefix("StoreDoc ContentStorage, GetOdbcHdl failed");
        XTRACE( err );
		return INTERNAL_ERROR;
	}

    // get column length
    retcode = this->GetParamLen( hstmt, DOC_SHORT_VALUE_PARAMNO, docValLen );
    if (SUCCESS(retcode)) {
        retcode = this->GetParamLen( hstmt, DOC_SHORT_PROPERTY_PARAMNO, docPropLen );
        if (SUCCESS(retcode)) {
            retcode = this->GetParamLen( hstmt, DOC_NAME_PARAMNO, docNameLen );
        };
    }; // if
    if (!SUCCESS(retcode)) {
        if (retcode == SQL_ERROR) {
            m_SAPCSConHdl->SQLErr(err, connection, stmtType);
        };
		err.AddPrefix("StoreDoc ContentStorage, GetParamLen failed");
        XTRACE( err );
        return INTERNAL_ERROR;
    }; // if
    if (lenName > docNameLen) {
        err.SetBuf( "StoreDoc ContentStorage, document name too long." );
        return INTERNAL_ERROR;
    }; // if

    if (useCompression) {
        tsp00_Int4 bytesWritten = 0;

        /* the document should be compressed before inserting */
        retcode = comprDoc.InitCompr( hstmt, lValue, &storedDocLen, &comprErr );
		if (retcode == SQL_SUCCESS) {
            /* compress the document to determine the compressed length */
            retcode = comprDoc.CompressBuffer( (SQLPOINTER) value, lValue,
                                               &docBytesRead,
                                               &bytesWritten,
                                               &docFinished,
                                               &comprErr );
            storedDocLen += bytesWritten;
		};
		if (retcode == SQL_SUCCESS) {
            /* compress the property also */
            retcode = comprProp.InitCompr( hstmt, lProperty, &storedPropLen, &comprErr );
		};
		if (retcode == SQL_SUCCESS) {
            retcode = comprProp.CompressBuffer( (SQLPOINTER) property, lProperty,
                                                &propBytesRead,
                                                &bytesWritten,
                                                &propFinished,
                                                &comprErr );
            storedPropLen += bytesWritten;
		};
        if (retcode != SQL_SUCCESS) {
            if (comprErr < 0) {
                /* compression error */
                err.Format("Compress error-code %d", comprErr );
            } else {
                /* ODBC error */
                m_SAPCSConHdl->SQLErr(err, connection, stmtType);
            }; /* else */
            err.AddPrefix("StoreDoc ContentStorage");
            XTRACE( err );
            return INTERNAL_ERROR;
        }; /* if */
    } else { /* don't compress */
		if (!isCSCompressed) {
            /* the document is inserted uncompressed */
			storedDocLen  = lValue;
			storedPropLen = lProperty;
		}
		else
		{
            /* check if if data is single block or block-wise compressed */
            if (!comprDoc.CheckHeader( (SQLPOINTER) value,
                                       &docComprType,
                                       &uncompressedDocLen,
                                       &comprErr )) {
                err.Format( "StoreDoc: pa111CheckHeader error-code %d",
                            comprErr );
                XTRACE( err );
                return INTERNAL_ERROR;
            }; /* if */
            if (!comprProp.CheckHeader( (SQLPOINTER) property,
                                        &propComprType,
                                        &uncompressedPropLen,
                                        &comprErr )) {
                err.Format( "StoreDoc: pa111CheckHeader2 error-code %d",
                            comprErr );
                XTRACE( err );
                return INTERNAL_ERROR;
            }; /* if */
            if (docComprType != propComprType) {
                err.Format( "StoreDoc: docComprType != propComprType" );
                XTRACE( err );
                return INTERNAL_ERROR;
            }; /* if */
            if (docComprType == SINGLE_BLOCK_PA110) {
                /* we have to prepend general and block header */
                storedDocLen  = lValue + sizeof(docHeader);
                storedPropLen = lProperty + sizeof(propHeader);
                pa110SetOneBlockHeader( docHeader, storedDocLen );
                pa110SetOneBlockHeader( propHeader, storedPropLen );
            } else {
                /* doc is stored as it is */
                storedDocLen  = lValue;
                storedPropLen = lProperty;
            };
		}
    }; /* else */

    /* documents smaller than docValLen are inserted in the
     * short column others in the LONG column */
    tsp00_Bool useShortFieldDoc = (docFinished &&
                             (storedDocLen <= docValLen));
    tsp00_Bool useShortFieldProp = (propFinished &&
                             (storedPropLen <= docPropLen));

    if (!isCSCompressed) {
        /* for compressed data we set length above */
        uncompressedDocLen = lValue;
        uncompressedPropLen = lProperty;
    }; /* else */
    /* for ODBC binding we need "long int"s */
    boundDocLen  = (long) uncompressedDocLen;
    boundPropLen = (long) uncompressedPropLen;

	SQLBindParameter( hstmt, DOC_NAME_PARAMNO, SQL_PARAM_INPUT,
                      SQL_C_BINARY, SQL_VARCHAR, 0, 0, (char*) name, 0, &lenName);
	SQLBindParameter( hstmt, DOC_LPROPERTY_PARAMNO, SQL_PARAM_INPUT,
                      SQL_C_SLONG, SQL_DECIMAL, 0, 0, &boundPropLen, 0, NULL);
	SQLBindParameter( hstmt, DOC_LVALUE_PARAMNO,
                      SQL_PARAM_INPUT, SQL_C_SLONG, SQL_DECIMAL,
                      0, 0, &boundDocLen, 0, NULL);

    if (useShortFieldProp) {
        SQLBindParameter( hstmt, DOC_SHORT_PROPERTY_PARAMNO, SQL_PARAM_INPUT,
                          SQL_C_BINARY, SQL_VARBINARY,
                          0, 0, (SQLPOINTER) DOC_SHORT_PROPERTY_PARAMNO,
                          0, &lenDataAtExec);
        SQLBindParameter( hstmt, DOC_LONG_PROPERTY_PARAMNO, SQL_PARAM_INPUT,
                          SQL_C_BINARY, SQL_LONGVARBINARY,
                          0, 0, NULL, 0, &lenNull);
    } else {
        SQLBindParameter( hstmt, DOC_SHORT_PROPERTY_PARAMNO, SQL_PARAM_INPUT,
                          SQL_C_BINARY, SQL_VARBINARY,
                          0, 0, NULL, 0, &lenNull);
        SQLBindParameter( hstmt, DOC_LONG_PROPERTY_PARAMNO, SQL_PARAM_INPUT,
                          SQL_C_BINARY, SQL_LONGVARBINARY,
                          0, 0, (SQLPOINTER) DOC_LONG_PROPERTY_PARAMNO, 0, &lenDataAtExec);
    }; // else

    if (useShortFieldDoc) {
        SQLBindParameter( hstmt, DOC_SHORT_VALUE_PARAMNO, SQL_PARAM_INPUT,
                          SQL_C_BINARY, SQL_VARBINARY,
                          0, 0, (SQLPOINTER) DOC_SHORT_VALUE_PARAMNO, 0, &lenDataAtExec);
        SQLBindParameter( hstmt, DOC_LONG_VALUE_PARAMNO, SQL_PARAM_INPUT,
                          SQL_C_BINARY, SQL_LONGVARBINARY,
                          0, 0, NULL, 0, &lenNull);
    } else {
        SQLBindParameter( hstmt, DOC_SHORT_VALUE_PARAMNO, SQL_PARAM_INPUT,
                          SQL_C_BINARY, SQL_VARBINARY,
                          0, 0, NULL, 0, &lenNull);
        SQLBindParameter( hstmt, DOC_LONG_VALUE_PARAMNO, SQL_PARAM_INPUT,
                          SQL_C_BINARY, SQL_LONGVARBINARY,
                          0, 0, (SQLPOINTER) DOC_LONG_VALUE_PARAMNO, 0, &lenDataAtExec);
    }; // else
        
	SQLBindParameter( hstmt, DOC_DATEM_PARAMNO, SQL_PARAM_INPUT,
                      SQL_C_CHAR, SQL_DATE, 0, 0, (char*) CS_CAST(ZString&) dateM, 0, &lenDate);
	SQLBindParameter( hstmt, DOC_TIMEM_PARAMNO, SQL_PARAM_INPUT,
                      SQL_C_CHAR, SQL_TIME, 0, 0, (char*) CS_CAST(ZString&) timeM, 0, &lenTime);
	SQLBindParameter( hstmt, DOC_COMPRESSED_PARAMNO, SQL_PARAM_INPUT,
                      SQL_C_CHAR, SQL_CHAR, 0, 0, &compressFlag, 0, &lenCompressFlag);
    
	retcode = SQLExecute(hstmt);
    if (retcode != SQL_NEED_DATA) {
        if (retcode == SQL_ERROR) {
            m_SAPCSConHdl->SQLErr(err, connection, stmtType);
        };
		err.AddPrefix("StoreDoc ContentStorage, SQLExecute: retcode != SQL_NEED_DATA");
        XTRACE( err );
        return INTERNAL_ERROR;
    }; // if


    // insert data for all SQL_DATA_AT_EXEC columns
    retcode = SQLParamData( hstmt, &nextParm );
    if (retcode != SQL_NEED_DATA) {
        if (retcode == SQL_ERROR) {
            m_SAPCSConHdl->SQLErr(err, connection, stmtType);
        };
		err.AddPrefix("StoreDoc ContentStorage, SQLParamData: retcode != SQL_NEED_DATA");
        XTRACE( err );
        return INTERNAL_ERROR;
    }; // if

    int loopCount = 0;
    while (retcode == SQL_NEED_DATA && loopCount++ < MAX_PARAMDATA_LOOP) {
        // insert data for column 4, 5, 6 and 7
        switch ((long) nextParm) {
        case DOC_SHORT_PROPERTY_PARAMNO :
            // insert property in column SHORT_PROPERTY
            if (useCompression) {
                retcode = comprProp.FlushBuffer();
            } else {
                if (isCSCompressed && propComprType == SINGLE_BLOCK_PA110) {
                    /* write header for new block wise compression */
                    retcode = SQLPutData( hstmt, (SQLPOINTER) propHeader, sizeof(propHeader) );
                };
                if (retcode == SQL_NEED_DATA || SUCCESS(retcode)) {
                    retcode = SQLPutData( hstmt, (SQLPOINTER) property, lProperty );
                }; // if
            }; // else
            break;
        case DOC_LONG_PROPERTY_PARAMNO :
            // insert property in column LONG_PROPERTY
            if (useCompression) {
                retcode = comprProp.FlushBuffer();
                if (!propFinished) {
                    // send the rest data with compression
                    retcode = comprProp.PutData( (SQLPOINTER) &property[propBytesRead],
                                                 lProperty-propBytesRead,
                                                 &comprErr );
                }; // if
            } else {
                if (isCSCompressed && propComprType == SINGLE_BLOCK_PA110) {
                    /* write header for new block wise compression */
                    retcode = SQLPutData( hstmt, propHeader, sizeof(propHeader) );
                };
                if (retcode == SQL_NEED_DATA || SUCCESS(retcode)) {
                    retcode = SQLPutData( hstmt, (SQLPOINTER) property, lProperty );
                };
            }; // else
            break;
        case DOC_SHORT_VALUE_PARAMNO :
            // insert data in column SHORT_VALUE
            if (useCompression) {
                retcode = comprDoc.FlushBuffer();
            } else {
                if (isCSCompressed && docComprType == SINGLE_BLOCK_PA110) {
                    /* write header for new block wise compression */
                    retcode = SQLPutData( hstmt, (SQLPOINTER) docHeader, sizeof(docHeader) );
                };
                if (retcode == SQL_NEED_DATA || SUCCESS(retcode)) {
                    retcode = SQLPutData( hstmt, (SQLPOINTER) value, lValue );
                };
            }; // else
            break;
        case DOC_LONG_VALUE_PARAMNO :
            // insert data in column LONG_VALUE
            if (useCompression) {
                retcode = comprDoc.FlushBuffer();
                if (!docFinished) {
                    // send the rest data with compression
                    retcode = comprDoc.PutData( (SQLPOINTER) &value[docBytesRead],
                                                (int) lValue-docBytesRead,
                                                &comprErr );
                }; // if
            } else {
                if (isCSCompressed && propComprType == SINGLE_BLOCK_PA110) {
                    /* write header for new block wise compression */
                    retcode = SQLPutData( hstmt, (SQLPOINTER) docHeader, sizeof(docHeader) );
                };
                if (retcode == SQL_NEED_DATA || SUCCESS(retcode)) {
                    retcode = SQLPutData( hstmt, (SQLPOINTER) value, lValue );
                };
            }; // else
            break;
        default:
            // unexpected column no
            err.AddPrefix("StoreDoc ContentStorage, unexpected column no from SQLParamData");
            XTRACE( err );
            return INTERNAL_ERROR;
            break;
        }; // switch
        if (retcode != SQL_ERROR) {
            retcode = SQLParamData( hstmt, &nextParm );
        }; // if
    }; // while
    if (loopCount >= MAX_PARAMDATA_LOOP) {
        // we should walk at least four times through the previous loop
		err.AddPrefix("StoreDoc ContentStorage, Loop count exceeded");
        XTRACE( err );
        return INTERNAL_ERROR;
    }; // if 

	if (!SUCCESS(retcode))
	{
		err.SetBuf("StoreDoc ContentStorage, StmtType %d ", stmtType);
		if (m_SAPCSConHdl->SQLErr(err, connection, stmtType) == SAPCS_ERR_DUPREC)
		{
			SQLFreeStmt(hstmt, SQL_CLOSE);
			return DUPLICATE_KEY;
		}
		else
		{
			SQLFreeStmt(hstmt, SQL_CLOSE);
            XTRACE( err );
			return INTERNAL_ERROR;
		}
	}	

	SQLFreeStmt(hstmt, SQL_CLOSE);
	return OK;
}; // StoreDoc
// ******************************************************************************


Storage::Error ContentStorage::StoreComp( TransID  tid,
                                          const    ZString& docId,
                                          const    ZString& compId,
                                          bool     doCreate,
                                          bool     doAppend,
                                          const    char* value, int lValue,
                                          bool     isCSCompressed,
                                          bool     storeCompressed,
                                          const    ZString& compDateM,
                                          const    ZString& compTimeM,
                                          const    ZString& contentType,
                                          ZString& err)
{
	DbConnection	connection = (DbConnection) tid;
	SQLHDBC			hdbc = SQL_NULL_HDBC;
	SQLHSTMT		hstmt = SQL_NULL_HSTMT;
	SQLHENV			henv = SQL_NULL_HENV;
	ZString			name;	
	RETCODE			retcode;
	SQLLEN			lenName, lenVal, lenNull, lenDate, lenTime, lenContType;
	StmtType		stmtType;
    int             bytesRead;
    int             storedCompLen;
    int             comprErr;
    tsp00_Bool      allBytesCompressed = true;
    int             uncompressedLen; /* of value */
    SQLPOINTER      nextParm;
    tpa111_ODCompr2 &compr = m_SAPCSConHdl->GetCompr( connection );
    const tsp00_Bool  useCompression = !isCSCompressed && storeCompressed;
    char            compressFlag = storeCompressed ? BLOCKCOMPRESSED_FLAG : UNCOMPRESSED_FLAG;
    SQLLEN          lenCompressFlag = 1;

    long            compNameLen = 0; /* length of name column */
    long            compValLen  = 0; /* length of short_value column */
    long            boundLen    = 0;

    tpa110_ComprType  compComprType = BLOCK_WISE_PA110;
    unsigned char     compHeader[GENERAL_HEADER_SIZE_PA110+OWN_HEADER_SIZE_PA110];

	if (isCSCompressed && !storeCompressed)
	{
		err.Add("StoreDoc ContentStorage, decompressed storage not implemented");
		return INTERNAL_ERROR;

	}
	err = 0;
	
    Storage::Error appendErr = OK;
	if (doAppend) {
        appendErr = this->AppendComp( tid, docId, compId,
                                      value, lValue,
                                      isCSCompressed,
                                      compDateM, compTimeM, err );
	}; /* if */
    if (doAppend && appendErr != KEY_NOT_FOUND) {
        return appendErr;
    }; // if
    // if doAppend==true and the component does not exist we create a new one
	name = CS_CAST(ZString&) docId;
	name.Add("\\");
	name.Add(CS_CAST(ZString&) compId);

	lenVal      = SQL_DATA_AT_EXEC;
	lenDate     = SQL_NTS;
	lenTime     = SQL_NTS;
	lenContType = SQL_NTS;
	lenNull     = SQL_NULL_DATA;
	if (doCreate)
		stmtType = INS_COMP;
	else
		stmtType = INS_COMP_WITH_DUP;
	if(!m_SAPCSConHdl->GetOdbcHdl(connection, stmtType, henv, hdbc, hstmt, err))
	{
		err.AddPrefix("StoreComp ContentStorage, GetOdbcHdl failed");
        XTRACE( err );
		return INTERNAL_ERROR;
	}

    retcode = this->GetParamLen( hstmt, COMP_NAME_PARAMNO, compNameLen );
    if (SUCCESS(retcode)) {
        retcode = this->GetParamLen( hstmt, COMP_SHORT_VALUE_PARAMNO, compValLen );
    }; // if
    if (!SUCCESS(retcode)) {
        if (retcode == SQL_ERROR) {
            m_SAPCSConHdl->SQLErr(err, connection, stmtType);
        };
		err.AddPrefix("StoreComp ContentStorage, GetParamLen failed");
        XTRACE( err );
        return INTERNAL_ERROR;
    }; // if

	lenName = name.GetSize();
	if (lenName > compNameLen)
	{
		err = "StoreComp ContentStorage, name of component name is too long";
        XTRACE( err );
		return INTERNAL_ERROR;
	}

    if (useCompression) {
        tsp00_Int4 bytesWritten = 0;

        retcode = compr.InitCompr( hstmt, lValue, &storedCompLen, &comprErr );
		if (retcode == SQL_SUCCESS) {
            retcode = compr.CompressBuffer( (SQLPOINTER) value, lValue,
				                            &bytesRead,
						                    &bytesWritten,
							                &allBytesCompressed,
								            &comprErr );
            storedCompLen += bytesWritten;
		};
        if (retcode != SQL_SUCCESS) {
            if (comprErr < 0) {
                /* compression error */
                err.Format("Compress error-code %d", comprErr );
            } else {
                /* ODBC error */
                m_SAPCSConHdl->SQLErr(err, connection, stmtType);
            }; /* else */
            err.AddPrefix("StoreComp ContentStorage, CompressBuffer failed");
            XTRACE( err );
            return INTERNAL_ERROR;
        }; /* if */
    } else {
		if (!isCSCompressed) {
			storedCompLen = lValue;
		} else {
            /* check if if data is single block or block-wise compressed */
            if (!compr.CheckHeader( (SQLPOINTER) value,
                                    &compComprType,
                                    &uncompressedLen,
                                    &comprErr )) {
                err.Format( "StoreComp: CheckHeader error-code %d",
                            comprErr );
                XTRACE( err );
                return INTERNAL_ERROR;
            }; /* if */
            if (compComprType == SINGLE_BLOCK_PA110) {
                /* we have to prepend general and block header */
                storedCompLen  = lValue + sizeof(compHeader);
                pa110SetOneBlockHeader( compHeader, storedCompLen );
            } else {
                storedCompLen = lValue;
            };
        };
    }; /* else */

    if (!isCSCompressed) {
        uncompressedLen = lValue;
    }; /* else */
    boundLen = uncompressedLen;

	SQLBindParameter( hstmt, COMP_NAME_PARAMNO, SQL_PARAM_INPUT,
                      SQL_C_BINARY, SQL_VARCHAR, 0, 0, (char*) name, 0, &lenName);
	SQLBindParameter( hstmt, COMP_LVALUE_PARAMNO, SQL_PARAM_INPUT,
                      SQL_C_SLONG, SQL_DECIMAL, 0, 0, &boundLen, 0, NULL);

    SQLBindParameter( hstmt, COMP_SHORT_VALUE_PARAMNO,
                      SQL_PARAM_INPUT,
                      SQL_C_BINARY,
                      SQL_VARBINARY,
                      0, 0, (SQLPOINTER) COMP_SHORT_VALUE_PARAMNO, 0, &lenVal);
    SQLBindParameter( hstmt, COMP_LONG_VALUE_PARAMNO,
                      SQL_PARAM_INPUT,
                      SQL_C_BINARY,
                      SQL_LONGVARBINARY,
                      0, 0, (SQLPOINTER) COMP_LONG_VALUE_PARAMNO, 0, &lenVal);

	SQLBindParameter( hstmt, COMP_COMPDATEM_PARAMNO, SQL_PARAM_INPUT,
                      SQL_C_CHAR, SQL_DATE, 0, 0, (char*) CS_CAST(ZString&) compDateM, 0, &lenDate);
	SQLBindParameter( hstmt, COMP_COMPTIMEM_PARAMNO, SQL_PARAM_INPUT,
                      SQL_C_CHAR, SQL_TIME, 0, 0, (char*) CS_CAST(ZString&) compTimeM, 0, &lenTime);
	SQLBindParameter( hstmt, COMP_CONTENTTYPE_PARAMNO, SQL_PARAM_INPUT,
                      SQL_C_BINARY, SQL_VARCHAR, 0, 0, (char*) CS_CAST(ZString&) contentType, 0, &lenContType);
	SQLBindParameter( hstmt, COMP_COMPRESSED_PARAMNO, SQL_PARAM_INPUT,
                      SQL_C_CHAR, SQL_CHAR, 0, 0, &compressFlag, 0, &lenCompressFlag);
    
	retcode = SQLExecute(hstmt);
    
    if (retcode != SQL_NEED_DATA) {
        if (retcode == SQL_ERROR) {
            m_SAPCSConHdl->SQLErr(err, connection, stmtType);
        };
		err.AddPrefix("StoreComp ContentStorage, SQLExecute: retcode != SQL_NEED_DATA");
        XTRACE( err );
        return INTERNAL_ERROR;
    }; /* if */
    
    retcode = SQLParamData( hstmt, &nextParm );
    if (retcode != SQL_NEED_DATA) {
        if (retcode == SQL_ERROR) {
            m_SAPCSConHdl->SQLErr(err, connection, stmtType);
        };
		err.AddPrefix("StoreComp ContentStorage, SQLExecute: retcode != SQL_NEED_DATA");
        XTRACE( err );
        return INTERNAL_ERROR;
    }; /* if */

    bool useShortField = (allBytesCompressed &&
                          (storedCompLen <= compValLen));
    int loopCount = 0;
    while (retcode == SQL_NEED_DATA && loopCount++ < MAX_PARAMDATA_LOOP) {
        switch ((long) nextParm) {
        case COMP_SHORT_VALUE_PARAMNO :
            if (useShortField) {
                /* insert data in column 3 */
                if (useCompression) {
                    /* we already compressed above */
                    retcode = compr.FlushBuffer();
                } else {
                    if (isCSCompressed && compComprType == SINGLE_BLOCK_PA110) {
                        /* write general and block header for BLOCK_WISE compression */
                        retcode = SQLPutData( hstmt, compHeader, sizeof(compHeader) );
                    };
                    if (retcode == SQL_NEED_DATA || SUCCESS(retcode)) {
                        retcode = SQLPutData( hstmt, (SQLPOINTER) value, lValue );
                    };
                }; /* else */
            } else {
                /* insert null value in column 3 */
                retcode = SQLPutData( hstmt, NULL, lenNull );
            }; /* else */
            break;
        case COMP_LONG_VALUE_PARAMNO :
            if (useShortField) {
                /* insert null value in column 4 */
                retcode = SQLPutData( hstmt, NULL, lenNull );
            } else {
                /* insert data in column 4 */
                if (useCompression) {
                    retcode = compr.FlushBuffer();
                    if (!allBytesCompressed) {
                        /* send the rest data */
                        retcode = compr.PutData( (SQLPOINTER) &value[bytesRead],
                                                 (int) lValue-bytesRead,
                                                 &comprErr );
                    }; /* if */
                } else {
                    if (isCSCompressed && compComprType == SINGLE_BLOCK_PA110) {
                        /* write general and block header for BLOCK_WISE compression */
                        retcode = SQLPutData( hstmt, (SQLPOINTER) compHeader, sizeof(compHeader) );
                    };
                    if (retcode == SQL_NEED_DATA || SUCCESS(retcode)) {
                        retcode = SQLPutData( hstmt, (SQLPOINTER) value, lValue );
                    };
                }; /* else */
            }; /* else */            
            break;
        default:
            /* unexpected column no */
            err.AddPrefix("StoreComp ContentStorage, unexpected column no from SQLParamData.");
            XTRACE( err );
            return INTERNAL_ERROR;
            break;
        }; /* switch */
        if (retcode != SQL_ERROR) {
            retcode = SQLParamData( hstmt, &nextParm );
        }; /* if */
    }; /* while */
    if (loopCount >= MAX_PARAMDATA_LOOP) {
        /* we should walk at least twice through the previous loop */
		err.AddPrefix("StoreComp ContentStorage, Loop count exceeded");
        XTRACE( err );
        return INTERNAL_ERROR;
    }; /* if */
    
	if (!SUCCESS(retcode))
	{
		err.SetBuf("StoreComp ContentStorage, StmtType %d ", stmtType);
		if (m_SAPCSConHdl->SQLErr(err, connection, stmtType) == SAPCS_ERR_DUPREC)
		{
			SQLFreeStmt(hstmt, SQL_CLOSE);
			return DUPLICATE_KEY;
		}
		else
		{
			SQLFreeStmt(hstmt, SQL_CLOSE);
            XTRACE( err );
			return INTERNAL_ERROR;
		}
	}

	SQLFreeStmt(hstmt, SQL_CLOSE);
	return OK;
}; // StoreComp

// ******************************************************************************
Storage::Error ContentStorage::DeleteDoc( TransID tid, const ZString& docId, ZString& err)
{	
	DbConnection	connection;
	SQLHDBC			hdbc;
	SQLHSTMT		hstmt;
	SQLHENV			henv;
	ZString			name;	
	RETCODE			retcode;
	SQLLEN			lenName = SQL_NTS;

	name = CS_CAST(ZString&) docId;

	connection = (DbConnection) tid;
	if(!m_SAPCSConHdl->GetOdbcHdl(connection, DEL_DOC, henv, hdbc, hstmt, err))
	{
		err.AddPrefix("DeleteDoc ContentStorage, GetOdbcHdl failed");
        XTRACE( err );
		return INTERNAL_ERROR;
	}

	SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARCHAR, 0, 0, (char*) name, 0, &lenName);

	retcode = SQLExecute(hstmt);
	if (retcode == SQL_ERROR)
	{
		m_SAPCSConHdl->SQLErr(err, connection, DEL_DOC);
		err.AddPrefix("DeleteDoc ContentStorage ");
        XTRACE( err );
		return INTERNAL_ERROR;
	}
	if (retcode == SQL_SUCCESS_WITH_INFO)
	{
		err = "DeleteDoc ContentStorage ";
		if (m_SAPCSConHdl->SQLErr(err, connection, DEL_DOC) == SAPCS_ERR_NOT_FOUND)
            {
                err.AddPrefix("DeleteDoc ContentStorage ");
                return KEY_NOT_FOUND;
            }
	}

	err = 0;
	return OK;
}; // DeleteDoc
// ******************************************************************************
Storage::Error ContentStorage::DeleteComp( TransID          tid,
                                           const ZString  & docId,
                                           const ZString  & compId,
                                           ZString        & err)
{
	DbConnection	connection;
	SQLHDBC			hdbc;
 	SQLHSTMT		hstmt;
	SQLHENV			henv;
	ZString			sqlStmtDel;
	ZString			name;	
	RETCODE			retcode;
	SQLLEN			lenName = SQL_NTS;

	name = CS_CAST(ZString&) docId;
	name.Add("\\");
	name.Add(CS_CAST(ZString&) compId);

	connection = (DbConnection) tid;

	if(!m_SAPCSConHdl->GetOdbcHdl(connection, DEL_COMP, henv, hdbc, hstmt, err))
	{
		err.AddPrefix("DeleteComp ContentStorage, GetOdbcHdl failed");
        XTRACE( err );
		return INTERNAL_ERROR;
	}

	SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARCHAR, 0, 0, (char*) name, 0, &lenName);

	retcode = SQLExecute(hstmt);
	if (retcode == SQL_ERROR)
	{
		m_SAPCSConHdl->SQLErr(err, connection, DEL_COMP);
		err.AddPrefix("DeleteComp ContentStorage ");
        XTRACE( err );
		return INTERNAL_ERROR;
	}
	if (retcode == SQL_SUCCESS_WITH_INFO)
	{
		err = "DeleteComp ContentStorage ";
		if (m_SAPCSConHdl->SQLErr(err, connection, DEL_COMP) == SAPCS_ERR_NOT_FOUND)
            {
                err.AddPrefix("DeleteComp ContentStorage ");
                return KEY_NOT_FOUND;
            }
	}

	err = 0;
	return OK;
}; // DeleteComp
// ******************************************************************************
Storage::Error ContentStorage::DeleteBuf(TransID tid,char*& buf,ZString& err)
{
    delete[] buf;
    buf = NULL;

	return OK;
}; // DeleteBuf

// ******************************************************************************
Storage::Error ContentStorage::GetDocIDInit(TransID tid,bool docIdOnly,ZString& err)
{
	DbConnection	connection;
	SQLHDBC			hdbc;
	SQLHSTMT		hstmt;
	SQLHENV			henv;
	RETCODE			retcode;
	StmtType		stmtType;
	ZString			sqlStmt;

	stmtType = SEL_ID;
	connection = (DbConnection) tid;
	if(!m_SAPCSConHdl->GetOdbcHdl(connection, stmtType, henv, hdbc, hstmt, err))
	{
		err.AddPrefix("GetDocIDInit ContentStorage, GetOdbcHdl failed");
        XTRACE( err );
		return INTERNAL_ERROR;
	}

// with SQL_CONCUR_READ_ONLY the precompiler of DB execute array fetch
	retcode = SQLSetStmtOption(hstmt, SQL_CONCURRENCY, SQL_CONCUR_READ_ONLY);

	if (docIdOnly)
	{
		sqlStmt.SetBuf("SELECT NAME FROM DOCUMENTS");
		sqlStmt.Add(m_ContRepNr);
	}
	else
	{
		sqlStmt.SetBuf("SELECT NAME FROM COMPONENTS");
		sqlStmt.Add(m_ContRepNr);
	}
	retcode = SQLPrepare(hstmt, (UCHAR*)(char*) sqlStmt, SQL_NTS);
	if (!SUCCESS(retcode))
	{
		m_SAPCSConHdl->SQLErr(err, connection, stmtType);
		err.AddPrefix("GetDocInit ContentStorage, SQLPrepare failed ");
        XTRACE( err );
		return INTERNAL_ERROR;
	}
	retcode = SQLExecute(hstmt); 
	if (!SUCCESS(retcode))
	{
		m_SAPCSConHdl->SQLErr(err, connection, stmtType);
		err.AddPrefix("GetDocIDInit ContentStorage, SQLExecute failed ");
		SQLFreeStmt(hstmt, SQL_CLOSE);	
        XTRACE( err );
		return INTERNAL_ERROR;
	}
	m_SAPCSConHdl->SetDocIdOnly(connection, docIdOnly);

	return OK;
}; // GetDocIdInit

// ******************************************************************************
Storage::Error ContentStorage::GetDocIDNext(TransID tid,ZString& docId,ZString& compId,bool& endOfList,ZString& err)
{
	DbConnection	connection;
	SQLHDBC			hdbc;
	SQLHSTMT		hstmt;
	SQLHENV			henv;
	StmtType		stmtType;
	ZString			name;	
	RETCODE			retcode;
	SQLLEN			lenValue;
	bool			docIdOnly;
	char           *docCompId;
	int				pos;

    long            compNameLen = 0;

	stmtType = SEL_ID;
	connection = (DbConnection) tid;
	if(!m_SAPCSConHdl->GetOdbcHdl(connection, stmtType, henv, hdbc, hstmt, err))
	{
		err.AddPrefix("GetDocIDNext ContentStorage, GetOdbcHdl failed ");
        XTRACE( err );
		return INTERNAL_ERROR;
	}
	docIdOnly = m_SAPCSConHdl->GetDocIdOnly(connection);

    retcode = this->GetColumnLen( hstmt, 1, compNameLen );
    if (!SUCCESS(retcode)) {
        if (retcode == SQL_ERROR) {
            m_SAPCSConHdl->SQLErr(err, connection, stmtType);
        };
		err.AddPrefix("GetDocIdNext ContentStorage, GetColumnLen failed");
        XTRACE( err );
        return INTERNAL_ERROR;
    }; // if
    
    docCompId = new char[compNameLen + 1];
	SQLBindCol(hstmt, 1, SQL_C_BINARY, (char*) docCompId, compNameLen + 1, &lenValue);

	retcode = SQLFetch(hstmt);
	switch (retcode)
	{
	case SQL_SUCCESS:
	case SQL_SUCCESS_WITH_INFO:
		endOfList = false;
		break;
	case SQL_NO_DATA_FOUND:
		SQLFreeStmt(hstmt, SQL_CLOSE);
		endOfList = true;
		break;
	case SQL_ERROR:
		m_SAPCSConHdl->SQLErr(err, connection, stmtType);
		err.AddPrefix("GetDocIdNext ContentStorage, SQLFetch ");
		SQLFreeStmt(hstmt, SQL_CLOSE);
		delete docCompId;
        XTRACE( err );
		return INTERNAL_ERROR;
	}
    if (SUCCESS(retcode)) {
        if (!docIdOnly)
            {	
                ZString docCompStr;
                
                docCompStr = docCompId;
                delete docCompId;
                if(docCompStr.Find('\\', pos))
                    {
                        docCompStr.SubString(docId, 0, pos - 1);
                        docCompStr.SubString(compId, pos+1, docCompStr.GetSize());
                    }
                else
                    {
                        err = "GetDocIDNext ContentStorage, Component key has no delimiter: ";
                        err.Add(docCompStr);
                        return INTERNAL_ERROR;
                    }
            }
        else
            {
                docId = docCompId;
                delete docCompId;
            }
    }; // if

	return OK;
}; // GetDocIdNext

// ******************************************************************************
Storage::Error ContentStorage::GetDocIDEnd(TransID tid,ZString& err)
{

	DbConnection	connection;
	SQLHDBC			hdbc;
	SQLHSTMT		hstmt;
	SQLHENV			henv;
	StmtType		stmtType;

	stmtType = SEL_ID;
	connection = (DbConnection) tid;
	if(!m_SAPCSConHdl->GetOdbcHdl(connection, stmtType, henv, hdbc, hstmt, err))
	{
		err.AddPrefix("GetDocIDInit ContentStorage, GetOdbcHdl failed ");
        XTRACE( err );
		return INTERNAL_ERROR;
	}
	SQLFreeStmt(hstmt, SQL_CLOSE);
	return OK;
}; // GetDocIdEnd

// ******************************************************************************
Storage::Error ContentStorage::GetDocStats(TransID tid,double& numDocs,double& numComps,double& sizeCompsMB,ZString& err)
{

	DbConnection	connection;
	SQLHDBC			hdbc;
	SQLHENV			henv;
	SQLHSTMT		hstmt;
	ZString			sqlStmt;
	double			sizeCompByte;
	RETCODE			retcode;

	connection = (DbConnection) tid;
	if(!m_SAPCSConHdl->GetOdbcHdl(connection, henv, hdbc, err))
	{
		err.AddPrefix("GetDocStats ContentStorage, GetOdbcHdl failed ");
        XTRACE( err );
		return INTERNAL_ERROR;
	}
	if (SQLAllocStmt(hdbc, &hstmt) != SQL_SUCCESS)
	{
		err = "GetDocStats ContentStorage, SQLAllocStmt failed";
        XTRACE( err );
		return INTERNAL_ERROR;
	}	
	sqlStmt.SetBuf("SELECT COUNT(*) FROM DOCUMENTS");
	sqlStmt.Add(m_ContRepNr);
	retcode = SQLPrepare(hstmt, (UCHAR*)(char*) sqlStmt, SQL_NTS);
	if (!SUCCESS(retcode))
	{
		err = "GetDocStats ContentStorage, SQLPrepare failed";
        XTRACE( err );
		return INTERNAL_ERROR;
	}
	SQLBindCol(hstmt, 1, SQL_C_DOUBLE, &numDocs, 0, NULL);
	
	retcode = SQLExecute(hstmt);
	if (!SUCCESS(retcode))
	{
		err = "GetDocStats ContentStorage, SQLExecute failed";
        XTRACE( err );
		return INTERNAL_ERROR;
	}
	retcode = SQLFetch(hstmt);
	if (retcode == SQL_ERROR)
	{
		err = "GetDocStats ContentStorage, SQLFetch failed";
	}
	SQLFreeStmt(hstmt, SQL_CLOSE);

	sqlStmt.SetBuf("SELECT COUNT(*), SUM(LVALUE) FROM COMPONENTS");
	sqlStmt.Add(m_ContRepNr);
	retcode = SQLPrepare(hstmt, (UCHAR*)(char*) sqlStmt, SQL_NTS);
	if (!SUCCESS(retcode))
	{
		err = "GetDocStats ContentStorage, SQLPrepare failed";
        XTRACE( err );
		return INTERNAL_ERROR;
	}
	SQLBindCol(hstmt, 1, SQL_C_DOUBLE, &numComps, 0, NULL);
	SQLBindCol(hstmt, 2, SQL_C_DOUBLE, &sizeCompByte, 0, NULL);
	
	retcode = SQLExecute(hstmt);
	if (!SUCCESS(retcode))
	{
		err = "GetDocStats ContentStorage, SQLExecute";
		SQLFreeStmt(hstmt, SQL_CLOSE);
        XTRACE( err );
		return INTERNAL_ERROR;
	}
	retcode = SQLFetch(hstmt);
	if (retcode == SQL_ERROR)
	{
		err = "GetDocStats ContentStorage, SQLFetch failed";
		SQLFreeStmt(hstmt, SQL_CLOSE);
        XTRACE( err );
		return INTERNAL_ERROR;
	}

	sizeCompsMB = (sizeCompByte / 1048576);
	
	return OK;
}; // GetDocStats


// ******************************************************************************
Storage::Error ContentStorage::GetFreeSpaceMB(TransID tid,double& freeSpaceMB,ZString& err)
{

	DbConnection	connection;
	SQLHDBC			hdbc;
	SQLHENV			henv;
	SQLHSTMT		hstmt;	
	ZString			sqlStmt;
	double			freeSpacePages;
	RETCODE			retcode;

	connection = (DbConnection) tid;
	if(!m_SAPCSConHdl->GetOdbcHdl(connection, henv, hdbc, err))
	{
		err = "GetFreeSpaceMB ContentStorage, GetOdbcHdl failed";
        XTRACE( err );
		return INTERNAL_ERROR;
	}
	if (SQLAllocStmt(hdbc, &hstmt) != SQL_SUCCESS)
	{
		err = "GetFreeSpace ContentStorage, SQLAllocStmt failed";
        XTRACE( err );
		return INTERNAL_ERROR;
	}	
	sqlStmt = "SELECT UNUSED FROM SYSDBA.SERVERDBSTATISTICS";
	retcode = SQLPrepare(hstmt, (UCHAR*)(char*) sqlStmt, SQL_NTS);
	if (!SUCCESS(retcode))
	{
		err = "GetFreeSpace ContentStorage, SQLPrepare failed";		
        XTRACE( err );
		return INTERNAL_ERROR;
	}
	SQLBindCol(hstmt, 1, SQL_C_DOUBLE, &freeSpacePages, 0, NULL);
	
	retcode = SQLExecute(hstmt);
	if (!SUCCESS(retcode))
	{
		err = "GetFreeSpace ContentStorage, SQLExecute failed";
		SQLFreeStmt(hstmt, SQL_CLOSE);
        XTRACE( err );
		return INTERNAL_ERROR;
	}
	retcode = SQLFetch(hstmt);
	if (retcode == SQL_ERROR)
	{
		SQLFreeStmt(hstmt, SQL_CLOSE);
		err = "GetFreeSpace ContentStorage, SQLFetch failed";
        XTRACE( err );
		return INTERNAL_ERROR;
	}
	freeSpaceMB = (freeSpacePages / 128);

	SQLFreeStmt(hstmt, SQL_CLOSE);
	return OK;
}; // GetFreeSpaceMB

// ******************************************************************************
Storage::Error ContentStorage::TableCheck(ZString& err)
{
	bool			prepStmt = false;
	bool			newContRepTab = false;
	ZString			tabName;
	ZString			sqlStmt;
	long			contRepNr = 0;
	long			version;
	Error			rc;

	tabName = "CONTREP";
	if (!ExistsTab(tabName, err))
	{
		sqlStmt = CONTREPDEF;
		if (CreateTab(sqlStmt, err))
			newContRepTab = true;
		else {
            XTRACE( err );
			return INTERNAL_ERROR;
        }
	}
	else
		newContRepTab = false;


	rc = OK;
	if (newContRepTab)
	{
		contRepNr = 1;
		NrToStr(contRepNr ,m_ContRepNr); 
		m_TableDef = actTableDef;
		if (CreateNewContRep(m_ContRepName, contRepNr, err) == false)
		{
			err = "TableCheck, ContentStorage, creating new ContRep failed";
            XTRACE( err );
			rc = INTERNAL_ERROR;
		}
	}
	else
	{
		if (GetContRepInfo(m_ContRepName, contRepNr, version, err))
		{
			NrToStr(contRepNr ,m_ContRepNr); 
			tabName = "DOCUMENTS";
			tabName.Add(m_ContRepNr);				
			if (ExistsTab(tabName, err) == false)
			{
				err.Add("document table of ContRep ");
				err.Add(m_ContRepName);
				err.Add(" does not exist. ContRepNr: ");
				err.Add(m_ContRepNr);
                XTRACE( err );
				rc = INTERNAL_ERROR;
			}

			tabName = "COMPONENTS";
			tabName.Add(m_ContRepNr);
			if (ExistsTab(tabName, err) == false)
			{
				err.Add("component table of ContRep ");
				err.Add(m_ContRepName);
				err.Add(" does not exist. ContRepNr: ");
				err.Add(m_ContRepNr);
                XTRACE( err );
                rc = INTERNAL_ERROR;
			}
			switch (version)
			{
			case 1:
				m_TableDef = &tableDef1;
				break;
			case 2:
				m_TableDef = &tableDef2;
				break;
			default:
				m_TableDef = actTableDef;
				break;
			}
		}
		else
		{
			GetHighestContRepNr(contRepNr, err);
			contRepNr++;
			NrToStr(contRepNr ,m_ContRepNr);
			m_TableDef = actTableDef;
			if (CreateNewContRep(m_ContRepName, contRepNr, err) == false)
			{
				err = "TableCheck, ContentStorage, creating new ContRep failed";
                XTRACE( err );
				rc = INTERNAL_ERROR;
			}
		}
	}

	return rc;
}; // TableCheck

// ******************************************************************************
bool ContentStorage::ExistsTab(ZString& tabName, ZString& err) 
{
	DbConnection	connection;
	SQLHENV         henv;
	SQLHDBC         hdbc;
	RETCODE         retcode;
	SQLHSTMT        hstmtSel;
	ZString         sqlStmt;
	long            cnt;
	bool            rc;
	ZString         userId;
	bool            prepared;

	rc = m_Options->GetOption((char*) m_SectionName, "user", userId, "");
	if (userId.GetSize() > 0)
		UpperCase((char*) userId);
	else
		userId = "SAPR3";

	if (m_SAPCSConHdl->GetDbHdl(connection, prepared, err) == false)
	{
		err.SetBuf("ExistsTab ContentStorage, connect error"); 
		return false;
	}
	if (m_SAPCSConHdl->GetOdbcHdl(connection, henv, hdbc, err) == false)
	{
		err.SetBuf("ExistsTab ContentStorage, GetOdbcHdl failed");
		m_SAPCSConHdl->PutDbHdl(connection, err);
		return false;
	}

	if (SQLAllocStmt(hdbc, &hstmtSel) != SQL_SUCCESS)
	{
		err.SetBuf("ExistsTab ContentStorage, SQLAllocStmt failed");
		m_SAPCSConHdl->PutDbHdl(connection, err);
		return false;
	}

	sqlStmt.SetBuf("SELECT COUNT(*) FROM DOMAIN.TABLES WHERE OWNER = '");
	sqlStmt.Add(userId);
	sqlStmt.Add("' AND TABLENAME = '");
	sqlStmt.Add(tabName);
	sqlStmt.Add("'");
	retcode = SQLPrepare(hstmtSel, (UCHAR*)(char*) sqlStmt, SQL_NTS);
	if (!SUCCESS(retcode))
	{	
		SQLFreeStmt(hstmtSel, SQL_CLOSE);
		m_SAPCSConHdl->PutDbHdl(connection, err);
		return false;
	}

	SQLBindCol(hstmtSel, 1, SQL_C_LONG, &cnt, 0, NULL);
	
	retcode = SQLExecute(hstmtSel);
	if (!SUCCESS(retcode))
	{
		SQLFreeStmt(hstmtSel, SQL_CLOSE);
		m_SAPCSConHdl->PutDbHdl(connection, err);
		return false;
	}

	retcode = SQLFetch(hstmtSel);
	if (retcode == SQL_ERROR || cnt == 0)
		rc = false;
	else
		rc = true;
	SQLFreeStmt(hstmtSel, SQL_CLOSE);
	m_SAPCSConHdl->PutDbHdl(connection, err);
	return rc;
}; // ExistsTab

// ******************************************************************************
bool ContentStorage::CreateTab(ZString& sqlStmtCre, ZString& err) 
{
	DbConnection	connection;
	SQLHENV			henv;
	SQLHDBC			hdbc;
	RETCODE			retcode;
	SQLHSTMT		hstmtCre;
	ZString			sqlStmt;
	bool			rc;
	bool			prepared;

	if (m_SAPCSConHdl->GetDbHdl(connection, prepared, err) == false)
	{
		err.AddPrefix("CreateTab ContentStorage, connect error "); 
		return false;
	}
	if (m_SAPCSConHdl->GetOdbcHdl(connection, henv, hdbc, err) == false)
	{
		err.AddPrefix("CreateTab ContentStorage, GetOdbcHdl failed");
		m_SAPCSConHdl->PutDbHdl(connection, err);
		return false;
	}
	if (SQLAllocStmt(hdbc, &hstmtCre) != SQL_SUCCESS)
	{
		err.SetBuf("CreateTab ContentStorage, SQLAllocStmt failed");
		m_SAPCSConHdl->PutDbHdl(connection, err);
		return false;
	}	

	retcode = SQLExecDirect(hstmtCre, (UCHAR*) (char*) sqlStmtCre, SQL_NTS);
	if (SUCCESS(retcode))
		rc = true;
	else
		rc = false;
		
	SQLTransact(henv, hdbc, SQL_COMMIT);
	SQLFreeStmt(hstmtCre, SQL_CLOSE);
	m_SAPCSConHdl->PutDbHdl(connection, err);
	return rc;
}; // CreateTab

// ******************************************************************************
bool ContentStorage::CreateNewContRep(ZString& contRepName, long contRepNr, ZString& err)
{
	DbConnection	connection;
	SQLHENV			henv;
	SQLHDBC			hdbc;
	RETCODE			retcode;
	SQLHSTMT		hstmtIns;
	ZString			sqlStmt;
	ZString			tabName;
	bool			prepared;
	ZString			tabVers;

	if (m_SAPCSConHdl->GetDbHdl(connection, prepared, err) == false)
	{
		err = "CreateNewContRep ContentStorage, connect error"; 
		return false;
	}
	if (m_SAPCSConHdl->GetOdbcHdl(connection, henv, hdbc, err) == false)
	{
		err = "CreateNewContRep ContentStorage, GetOdbcHdl failed";
		m_SAPCSConHdl->PutDbHdl(connection, err);
		return false;
	}
	if (SQLAllocStmt(hdbc, &hstmtIns) != SQL_SUCCESS)
	{
		err = "CreateNewContRep ContentStorage, SQLAllocStmt failed";
		m_SAPCSConHdl->PutDbHdl(connection, err);
		return false;
	}	

	sqlStmt.SetBuf("INSERT INTO CONTREP (CONTREPNAME, CONTREPNR, VERSION) VALUES ('");
	sqlStmt.Add(m_ContRepName);
	sqlStmt.Add("','");
	sqlStmt.Add(m_ContRepNr);
	sqlStmt.Add("',");
	NrToStr(m_TableDef->version, tabVers);
	sqlStmt.Add(tabVers);
	sqlStmt.Add(")");
	retcode = SQLExecDirect(hstmtIns, (UCHAR*) (char*) sqlStmt, SQL_NTS);
	if (SUCCESS(retcode))
	{
		SQLTransact(henv, hdbc, SQL_COMMIT);
		SQLFreeStmt(hstmtIns, SQL_CLOSE);
		m_SAPCSConHdl->PutDbHdl(connection, err);
	}
	else
	{
		SQLTransact(henv, hdbc, SQL_ROLLBACK);
		SQLFreeStmt(hstmtIns, SQL_CLOSE);
		m_SAPCSConHdl->PutDbHdl(connection, err);
		return false;
	}

	tabName.SetBuf("DOCUMENTS");
	tabName.Add(m_ContRepNr);
	if (ExistsTab(tabName, err) == false)
	{
		sqlStmt.SetBuf("CREATE TABLE ");
		sqlStmt.Add(tabName);
		sqlStmt.Add(CS_CAST(ZString&) m_TableDef->docDef);
        if (m_TableDef->version >= 2) {
            // set doc name length as configured in ini file
            // length of short_value depends on_DocNameLen
            ZString formatStr = sqlStmt;
            long    docValLen = DEFAULT_DOC_SHORT_VALUE_LEN
                +DEFAULT_DOC_NAME_LEN-m_DocNameLen;
            if (this->GetDbVersion(hdbc) >= DB_VERSION_72) {
                docValLen += 4096; /* in 7.2 max. record size is 8k instead of 4k */
            }; // if
            sqlStmt.Format( (char*) formatStr, m_DocNameLen, docValLen );
        }; // if
		if (CreateTab(sqlStmt, err) == false)
		{
            SQLTransact(henv, hdbc, SQL_ROLLBACK);
			err = "CreateNewContRep ContentStorage, can not create document table for ContRep ";
			err.Add(contRepName);
			return false;
		}
	}
	tabName.SetBuf("COMPONENTS");
	tabName.Add(m_ContRepNr);
	if (ExistsTab(tabName, err) == false)
	{
		sqlStmt.SetBuf("CREATE TABLE ");
		sqlStmt.Add(tabName);
		sqlStmt.Add(CS_CAST(ZString&) m_TableDef->compDef);
        if (m_TableDef->version >= 2) {
            // set length of short_value depending on db version
            ZString formatStr = sqlStmt;
            long compValLen   = COMP_SHORT_VALUE_LEN;
            if (this->GetDbVersion(hdbc) >= DB_VERSION_72) {
                compValLen += 4056; /* in 7.2 max. record size is 8k instead of 4k */
            }; // if
            sqlStmt.Format( (char*) formatStr, compValLen );
        }; // if
		if (CreateTab(sqlStmt, err) == false)
		{
            SQLTransact(henv, hdbc, SQL_ROLLBACK);
			err = "CreateNewContRep ContentStorage, can not create component table for ContRep ";
			err.Add(contRepName);
			return false;
		}
	}
	return true;
}; // CreateNewContRep

// ******************************************************************************
bool ContentStorage::GetContRepInfo( const ZString& contRepName, long& contRepNr, long& version, ZString& err)
{
	
	RETCODE			retcode;
	DbConnection	connection;
	SQLHENV			henv;
	SQLHDBC			hdbc;
	SQLHSTMT		hstmtSel;
	ZString			sqlStmt;
	bool			rc;
	bool			prepared;

	if (m_SAPCSConHdl->GetDbHdl(connection, prepared, err) == false)
	{
		err.SetBuf("GetContRepInfo ContentStorage, connect error"); 
		return false;
	}
	if (m_SAPCSConHdl->GetOdbcHdl(connection, henv, hdbc, err) == false)
	{
		err.SetBuf("GetContRepInfo ContentStorage, GetOdbcHdl failed");
		m_SAPCSConHdl->PutDbHdl(connection, err);
		return false;
	}
	if (SQLAllocStmt(hdbc, &hstmtSel) != SQL_SUCCESS)
	{
		m_SAPCSConHdl->PutDbHdl(connection, err);
		return false;
	}	
	sqlStmt.SetBuf("SELECT CONTREPNR, VERSION FROM CONTREP WHERE CONTREPNAME = '");
	sqlStmt.Add(CS_CAST(ZString&) contRepName);
	sqlStmt.Add("'");
	retcode = SQLPrepare(hstmtSel, (UCHAR*)(char*) sqlStmt, SQL_NTS);
	if (!SUCCESS(retcode))
	{
		m_SAPCSConHdl->PutDbHdl(connection, err);
		return false;
	}
	SQLBindCol(hstmtSel, 1, SQL_C_LONG, &contRepNr, 0, NULL);
	SQLBindCol(hstmtSel, 2, SQL_C_LONG, &version, 0, NULL);
	
	retcode = SQLExecute(hstmtSel);
	if (!SUCCESS(retcode))
	{
		m_SAPCSConHdl->PutDbHdl(connection, err);
		return false;
	}
	retcode = SQLFetch(hstmtSel);
	switch (retcode)
	{
	case SQL_SUCCESS:
	case SQL_SUCCESS_WITH_INFO:
		rc = true;
		break;
	default:
		rc = false;
	}
	SQLFreeStmt(hstmtSel, SQL_CLOSE);
	m_SAPCSConHdl->PutDbHdl(connection, err);
	return rc;
}; // GetContRepInfo

// ******************************************************************************
bool ContentStorage::GetDbParam( const char *description, long& value, ZString& err)
{
	
	RETCODE			retcode;
	DbConnection	connection;
	SQLHENV			henv;
	SQLHDBC			hdbc;
	SQLHSTMT		hstmtSel;
	ZString			sqlStmt;
	bool			rc;
	bool			prepared;

	if (m_SAPCSConHdl->GetDbHdl(connection, prepared, err) == false)
	{
		err.SetBuf("GetDbParam ContentStorage, connect error"); 
		return false;
	}
	if (m_SAPCSConHdl->GetOdbcHdl(connection, henv, hdbc, err) == false)
	{
		err.SetBuf("GetDbParam ContentStorage, GetOdbcHdl failed");
		m_SAPCSConHdl->PutDbHdl(connection, err);
		return false;
	}
	if (SQLAllocStmt(hdbc, &hstmtSel) != SQL_SUCCESS)
	{
		m_SAPCSConHdl->PutDbHdl(connection, err);
		return false;
	}
	sqlStmt.Format("SELECT VALUE FROM DBPARAMETERS WHERE DESCRIPTION = '%s'", description );

	retcode = SQLPrepare(hstmtSel, (UCHAR*)(char*) sqlStmt, SQL_NTS);
	if (!SUCCESS(retcode))
	{
		m_SAPCSConHdl->PutDbHdl(connection, err);
		return false;
	}
	SQLBindCol(hstmtSel, 1, SQL_C_LONG, &value, 0, NULL);
	
	retcode = SQLExecute(hstmtSel);
	if (!SUCCESS(retcode))
	{
		m_SAPCSConHdl->PutDbHdl(connection, err);
		return false;
	}
	retcode = SQLFetch(hstmtSel);
	switch (retcode)
	{
	case SQL_SUCCESS:
	case SQL_SUCCESS_WITH_INFO:
		rc = true;
		break;
	default:
		rc = false;
	}
	SQLFreeStmt(hstmtSel, SQL_CLOSE);
	m_SAPCSConHdl->PutDbHdl(connection, err);
	return rc;
}; // GetDbParam


// ******************************************************************************
bool ContentStorage::GetHighestContRepNr(long& contRepNr, ZString& err)
{
	RETCODE			retcode;
	DbConnection	connection;
	SQLHENV			henv;
	SQLHDBC			hdbc;
	SQLHSTMT		hstmtSel;
	ZString			sqlStmt;
	SQLLEN			lenContRepNr;
	bool			prepared;

	if (m_SAPCSConHdl->GetDbHdl(connection, prepared, err) == false)
	{
		err.SetBuf("GetHightestContRepNr ContentStorage, connect error"); 
		return false;
	}
	if (m_SAPCSConHdl->GetOdbcHdl(connection, henv, hdbc, err) == false)
	{
		err.SetBuf("GetHighestRepNr ContentStorage, GetOdbcHdl failed");
		m_SAPCSConHdl->PutDbHdl(connection, err);
		return false;
	}
	if (SQLAllocStmt(hdbc, &hstmtSel) != SQL_SUCCESS)
	{
		m_SAPCSConHdl->PutDbHdl(connection, err);
		return false;
	}	

	sqlStmt.SetBuf("SELECT MAX(CONTREPNR) FROM CONTREP");
	retcode = SQLPrepare(hstmtSel, (UCHAR*)(char*) sqlStmt, SQL_NTS);
	if (!SUCCESS(retcode))
	{
		SQLFreeStmt(hstmtSel, SQL_CLOSE);
		m_SAPCSConHdl->PutDbHdl(connection, err);
		return false;
	}
	SQLBindCol(hstmtSel, 1, SQL_C_LONG, &contRepNr, 0, &lenContRepNr);
	retcode = SQLExecute(hstmtSel);
	if (!SUCCESS(retcode))
	{
		SQLFreeStmt(hstmtSel, SQL_CLOSE);
		m_SAPCSConHdl->PutDbHdl(connection, err);
		return false;
	}

	retcode = SQLFetch(hstmtSel);
	if (retcode == SQL_ERROR)
	{
		SQLFreeStmt(hstmtSel, SQL_CLOSE);
		m_SAPCSConHdl->PutDbHdl(connection, err);
		return false;
	}
	else 
	{
		if (lenContRepNr == SQL_NULL_DATA) 
			contRepNr = 0;
	}
	SQLFreeStmt(hstmtSel, SQL_CLOSE);
	m_SAPCSConHdl->PutDbHdl(connection, err);
	return true;
}; // GetHighestContRepNr

// ******************************************************************************
void ContentStorage::NrToStr(long nr, ZString& nrStr)
{
	char			charStr[17];
	ZString			zStr;

#ifdef WIN32
	_itoa(nr, charStr, 10);
#else
    sprintf( charStr, "%d", (int) nr );
#endif
	zStr.SetBuf("00000");
	zStr.SubString(nrStr, 0, 4 - strlen(charStr));
	nrStr.Add(charStr);
}

// ******************************************************************************
bool ContentStorage::PrepareStmt(DbConnection connection, ZString& err)
{
	RETCODE  retcode;
	SQLHENV	 henv;
	SQLHDBC	 hdbc;
	SQLHSTMT hstmt;
	ZString  sqlStmt;
    int      stmtNo;
	bool	 sqlOK,
			 prepare;

	sqlOK = true;
	prepare = true;
	for (stmtNo = 0; stmtNo < NUMBER_STMT_TYPES && sqlOK == true ; stmtNo++)
	{
		if(!m_SAPCSConHdl->GetOdbcHdl(connection, (StmtType) stmtNo, henv, hdbc, hstmt, err))
		{
			err.AddPrefix("ContentStorage::PrepareStmt, GetOdbcHdl failed");
			return false;
		}
		switch(stmtNo)
		{
		case SEL_DOC:
			sqlStmt = "SELECT LPROPERTY, LVALUE, SHORT_PROPERTY, LONG_PROPERTY,";
			sqlStmt.Add(" SHORT_VALUE, LONG_VALUE, COMPRESSED FROM DOCUMENTS");
			sqlStmt.Add(m_ContRepNr);
			sqlStmt.Add(" WHERE NAME = ?");
			break;

		case SEL_DOC_FOR_UPD:
			sqlStmt = "SELECT LPROPERTY, LVALUE, SHORT_PROPERTY, LONG_PROPERTY,";
			sqlStmt.Add(" SHORT_VALUE, LONG_VALUE, COMPRESSED FROM DOCUMENTS");
			sqlStmt.Add(m_ContRepNr);
			sqlStmt.Add(" WHERE NAME = ? WITH LOCK EXCLUSIVE");
			break;

		case SEL_COMP:
			sqlStmt = "SELECT LVALUE, SHORT_VALUE, LONG_VALUE, COMPRESSED FROM COMPONENTS";
			sqlStmt.Add(m_ContRepNr);
			sqlStmt.Add(" WHERE NAME = ?");
			break;

		case SEL_COMP_FOR_UPD:
			sqlStmt = "SELECT LVALUE, SHORT_VALUE, LONG_VALUE, COMPRESSED FROM COMPONENTS";
			sqlStmt.Add(m_ContRepNr);
			sqlStmt.Add("  WHERE NAME = ? WITH LOCK EXCLUSIVE");
			break;

		case INS_DOC:
			sqlStmt = "INSERT INTO DOCUMENTS";
			sqlStmt.Add(m_ContRepNr);	
			sqlStmt.Add(" (NAME, LPROPERTY, LVALUE, SHORT_PROPERTY, LONG_PROPERTY, SHORT_VALUE, LONG_VALUE, DATEM, TIMEM, COMPRESSED)");
			sqlStmt.Add(" VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?) UPDATE DUPLICATES");
			break;	

		case INS_COMP:
			sqlStmt = "INSERT INTO COMPONENTS";
			sqlStmt.Add(m_ContRepNr);	
			sqlStmt.Add(" (NAME, LVALUE, SHORT_VALUE, LONG_VALUE, COMPDATEM, COMPTIMEM, CONTENTTYPE, COMPRESSED)");
			sqlStmt.Add(" VALUES (?, ?, ?, ?, ?, ?, ?, ?)");
			break;

		case INS_COMP_WITH_DUP:
			sqlStmt = "INSERT INTO COMPONENTS";
			sqlStmt.Add(m_ContRepNr);	
			sqlStmt.Add(" (NAME, LVALUE, SHORT_VALUE, LONG_VALUE, COMPDATEM, COMPTIMEM, CONTENTTYPE, COMPRESSED)");
			sqlStmt.Add(" VALUES (?, ?, ?, ?, ?, ?, ?, ?) UPDATE DUPLICATES");
			break;
		
		case DEL_DOC:
			sqlStmt = "DELETE FROM DOCUMENTS";
			sqlStmt.Add(m_ContRepNr);	
			sqlStmt.Add(" WHERE NAME = ?");
			break;

		case DEL_COMP:
			sqlStmt = "DELETE FROM COMPONENTS";
			sqlStmt.Add(m_ContRepNr);	
			sqlStmt.Add(" WHERE NAME = ?");
			break;

		case SEL_ID:
			prepare = false;
			break;
        case UPD_COMP:
			sqlStmt = "UPDATE COMPONENTS";
			sqlStmt.Add(m_ContRepNr);	
            sqlStmt.Add(" (LVALUE, SHORT_VALUE, LONG_VALUE, COMPDATEM, COMPTIMEM, COMPRESSED) VALUES (?,?,?,?,?,?)");
            sqlStmt.Add(" WHERE NAME=?");
            break;

        case APP_COMP:
            if (this->GetDbVersion(hdbc) >= DB_VERSION_72) {
                sqlStmt = "APPEND COMPONENTS";
                sqlStmt.Add(m_ContRepNr);	
                sqlStmt.Add(" (LVALUE, SHORT_VALUE, LONG_VALUE, COMPDATEM, COMPTIMEM, COMPRESSED) VALUES (?,?,?,?,?,?)");
                sqlStmt.Add(" WHERE NAME=?");
            } else {
                prepare = false;
            }; // else
            break;
		}; // switch

		if (prepare)
		{
			retcode = SQLPrepare(hstmt, (UCHAR*) (char*) sqlStmt, SQL_NTS);
			if (!SUCCESS(retcode))
			{
                m_SAPCSConHdl->SQLErr(err, connection, (StmtType) stmtNo);
				sqlOK = false;
			}
		}
		else
		{
			prepare = true;
		}
	}
	if (sqlOK)
		m_SAPCSConHdl->IsPrepared(connection);
	return sqlOK;
}; // PrepareStmt


long ContentStorage::GetCompAllocLen( long docLen, long fromPos, long toPos )
{
    long allocLen = 0;

    if (docLen == SQL_NULL_DATA) {
        allocLen = 1;
    } else {
        if (fromPos > 0) {
            allocLen = -fromPos+1;
        }; // if
        if (toPos != -1) {
            allocLen += min( docLen, toPos );
        } else {
            allocLen += docLen;
        }; // else
    }; // else

    return allocLen;
}; // GetCompAllocLen



Storage::Error ContentStorage::FetchCompOpenResult( TransID           tid,
                                                    ZString          &name,
                                                    StmtType          stmtType,
                                                    SQLHSTMT         &hstmt,
                                                    SQLHDBC          &hdbc,
                                                    SQLLEN           &uncompressedLen,
                                                    tpa110_ComprType &comprType,
                                                    ZString          &err )
{
	DbConnection	connection = (DbConnection) tid;
	SQLHENV			henv;
	RETCODE			retcode;
	SQLLEN			lenName = SQL_NTS;
    SQLLEN          lenLValue;
    SQLLEN          lenCompressFlag;
    char            compressFlag = UNCOMPRESSED_FLAG;

	
	if(!m_SAPCSConHdl->GetOdbcHdl(connection, stmtType, henv, hdbc, hstmt, err))
	{
		err = "FetchCompOpenResult ContentStorage, GetOdbcHdl failed";
        XTRACE( err );
		return INTERNAL_ERROR;
	}

  	SQLBindParameter( hstmt, 1, SQL_PARAM_INPUT,
                      SQL_C_BINARY, SQL_VARCHAR, 0, 0, (char*) name, 0, &lenName);
	
	SQLBindCol( hstmt, COMP_LVALUE_COLNO, SQL_C_SLONG, &uncompressedLen, 0, &lenLValue);
	SQLBindCol( hstmt, COMP_COMPRESSED_COLNO, SQL_C_BINARY, &compressFlag, 1, &lenCompressFlag);

	retcode = SQLExecute(hstmt); 
	if (!SUCCESS(retcode))
	{
		m_SAPCSConHdl->SQLErr(err, connection, stmtType);
		err.AddPrefix("FetchCompOpenResult ContentStorage, SQLExecute ");
		SQLFreeStmt(hstmt, SQL_CLOSE);
        XTRACE( err );
		return INTERNAL_ERROR;
	}

	retcode = SQLFetch(hstmt);
	if (retcode == SQL_NO_DATA_FOUND)
	{
		SQLFreeStmt(hstmt, SQL_CLOSE);
		return KEY_NOT_FOUND;
	}
	else if (retcode == SQL_ERROR)
	{
		m_SAPCSConHdl->SQLErr(err, connection, stmtType);
		err.AddPrefix("FetchCompOpenResult ContentStorage, SQLFetch ");
		SQLFreeStmt(hstmt, SQL_CLOSE);
        XTRACE( err );
		return INTERNAL_ERROR;
	}

	if (lenCompressFlag == SQL_NULL_DATA)
		compressFlag = UNCOMPRESSED_FLAG;
    comprType = FlagToComprType( compressFlag );
    
    return OK;
}; // FetchCompOpenResult


Storage::Error ContentStorage::AppendComp( TransID tid,
                                           const ZString &docId,
                                           const ZString &compId,
                                           const char    *value,
                                           int      lValue,
                                           bool     isCSCompressed,
                                           const ZString &compDateM,
                                           const ZString &compTimeM,
                                           ZString &err )
{
	DbConnection	connection = (DbConnection) tid;
	SQLHDBC			hdbc;
	SQLHSTMT		fetchHstmt;
	SQLHSTMT		updateHstmt;
	SQLHENV			henv;
	ZString			name;
	RETCODE			sqlRetcode;
	SQLLEN			lenName, lenVal, lenNull, lenDate, lenTime;
    long            totalLen = 0;
	StmtType		stmtType;
    int             bytesRead;
    int             totalBytesRead;
    int             storedCompLen;
    int             oldCompressedLen = 0;
    int             comprErr;
    tsp00_Bool      allBytesCompressed = false;
    SQLPOINTER      nextParm;
    tpa111_ODCompr2    &compr = m_SAPCSConHdl->GetCompr( connection );
	tpa111_ODDecompr2  &decompr = m_SAPCSConHdl->GetDecompr( connection );
    char            compressFlag = UNCOMPRESSED_FLAG;
    SQLLEN          lenCompressFlag = 1;
    Error           retcode;
    bool            useAppend = true;

    AutoPtr         oldValue = NULL; // is automatically deleted when leaving this method
    SQLLEN          oldLen   = 0;
    tpa110_ComprType oldComprType  = NOT_COMPRESSED_PA110;    
    long             compValLen    = 0;
    tpa110_ComprType compComprType = NOT_COMPRESSED_PA110;

	if (isCSCompressed) {
        // we cannot append compressed data
		err.Add("AppendComp ContentStorage, append of compressed data not implemented");
        XTRACE( err );

		return INTERNAL_ERROR;
	}; /* if */

    name = CS_CAST(ZString&) docId;
    name.Add("\\");
    name.Add(CS_CAST(ZString&) compId);
	lenName = name.GetSize();

    retcode = this->FetchCompOpenResult( tid, name, SEL_COMP,
                                         fetchHstmt, hdbc, oldLen, oldComprType, err );
    if (retcode != OK) {
        return retcode;
    }; // if
    // for single block compressed data we convert to block wise compression
    switch (oldComprType) {
    case SINGLE_BLOCK_PA110:
    case BLOCK_WISE_PA110:
        compressFlag = BLOCKCOMPRESSED_FLAG;
        break;
    default:
        compressFlag = UNCOMPRESSED_FLAG;
        break;
    }; // switch        
    sqlRetcode = this->GetColumnLen( fetchHstmt, COMP_SHORT_VALUE_COLNO, compValLen );
    if (!SUCCESS(sqlRetcode)) {
        if (sqlRetcode == SQL_ERROR) {
            m_SAPCSConHdl->SQLErr(err, connection, SEL_COMP);
        };
		err.AddPrefix("AppendComp ContentStorage, GetColumnLen failed");
        XTRACE( err );

        return INTERNAL_ERROR;
    }; // if

    totalLen = oldLen + lValue;

    SQLLEN firstStrLen  = 0; // used to read the first byte
    SQLLEN strLenOrIndi = 0;
    char   shortColByte = 0;

    // try to read one byte from the short column to see if it is NULL
    if (oldComprType != NOT_COMPRESSED_PA110) {
        decompr.InitDecompr( fetchHstmt, COMP_SHORT_VALUE_COLNO );

        sqlRetcode = decompr.GetData( &shortColByte, 1, &firstStrLen, &comprErr );
    } else {
        sqlRetcode = SQLGetData( fetchHstmt,
                                 COMP_SHORT_VALUE_COLNO,
                                 SQL_C_BINARY,
                                 &shortColByte,
                                 1,
                                 &firstStrLen );
        // firstStrLen should be 1, 0 or SQL_NULL_DATA
        firstStrLen = (firstStrLen > 0) ? 1 : firstStrLen;
    }; // else
    if (oldComprType == SINGLE_BLOCK_PA110 // can't APPEND single block-compressed data
        || firstStrLen > 0  // can't APPEND varchar column
        || this->GetDbVersion( hdbc ) < DB_VERSION_72) { // APPEND command only in 7.2 available
        // we have to read the whole old value
        useAppend = false;
        oldValue = new char[oldLen+1]; // oldLen may be 0
        if (oldValue == NULL) {
            XTRACE("malloc failed in AppendComp");
            err = "AppendComp ContentStorage, memory allocation failed";
            SQLFreeStmt( fetchHstmt, SQL_CLOSE );

            return INTERNAL_ERROR;
        }; // if
        // read old value 
        if (firstStrLen != SQL_NULL_DATA) {
            // value is in the short column. Read and uncompress
            oldValue[0] = shortColByte;
            if (oldComprType != NOT_COMPRESSED_PA110) {
                // old value is compressed. Decompress content of short column.
                sqlRetcode = decompr.GetData( &oldValue[1], oldLen, &strLenOrIndi, &comprErr );
            } else {
                sqlRetcode = SQLGetData( fetchHstmt, COMP_SHORT_VALUE_COLNO,
                                         SQL_C_BINARY, &oldValue[1], oldLen, &strLenOrIndi );
            }; // else
            strLenOrIndi += firstStrLen; // firstStrLen may be 0 or 1
        } else {
            // value is in the long column single block compressed (DB 7.2 or 7.1) or block wise
            // compressed (DB 7.1). Read without uncompressing
            // We don't know compressed size so use DynSQLGetData
            sqlRetcode = DynSQLGetData( fetchHstmt, COMP_LONG_VALUE_COLNO,
                                        SQL_C_BINARY, (void**) &oldValue, oldLen, &strLenOrIndi );
			oldCompressedLen = (int) strLenOrIndi;
        }; // else
    } else if (firstStrLen == SQL_NULL_DATA) {
        // we can use the append command because the value is block-wise or
        // uncompressed in the long column and DB-Version is >= 7.2
        useAppend = true;
    } else { // firstStrLen == 0
        // data with length 0 in short column
        useAppend = false;
    }; // else

    if (!SUCCESS(sqlRetcode)) {
        if (sqlRetcode == SQL_ERROR) {
            m_SAPCSConHdl->SQLErr( err, connection, SEL_COMP );
        } else {
            err = "AppendComp ContentStorage, Reading of old component failed.";
        }; // else
    }; // if
    sqlRetcode = SQLFreeStmt( fetchHstmt, SQL_CLOSE);
    
	err = 0;

	lenVal  = SQL_DATA_AT_EXEC;
	lenDate = SQL_NTS;
	lenTime = SQL_NTS;
	lenNull = SQL_NULL_DATA;

	if (useAppend) {
	    stmtType = APP_COMP;
    } else {
        stmtType = UPD_COMP;
    }; // else
	if(!m_SAPCSConHdl->GetOdbcHdl( connection, stmtType, henv, hdbc, updateHstmt, err))
	{
		err.AddPrefix("AppendComp ContentStorage, GetOdbcHdl failed");
        XTRACE( err );

		return INTERNAL_ERROR;
	}

    if (firstStrLen >= 0 // comp is in short column
        && oldComprType != NOT_COMPRESSED_PA110) {
        tsp00_Int4 bytesWritten = 0;

        // if old is compressed we store compressed again
        totalBytesRead = 0;
        compr.InitCompr( updateHstmt, totalLen, &storedCompLen, &comprErr );
        if (firstStrLen >= 0 && oldLen >= 0) {
            // oldComp is in short column (we decompressed it above)
            sqlRetcode = compr.CompressBuffer( oldValue,
                                               oldLen,
                                               &totalBytesRead,
                                               &bytesWritten,
                                               &allBytesCompressed,
                                               &comprErr );
            storedCompLen += bytesWritten;
        };
        if (sqlRetcode == SQL_SUCCESS) {
            if (totalBytesRead >= oldLen) {
                // append new value

                sqlRetcode = compr.CompressBuffer( (SQLPOINTER) value, lValue,
                                                   &bytesRead,
                                                   &bytesWritten,
                                                   &allBytesCompressed,
                                                   &comprErr );
                totalBytesRead += bytesRead;
                storedCompLen  += bytesWritten;
            }; // if
        }; // if
        if (sqlRetcode != SQL_SUCCESS) {
            if (comprErr < 0) {
                /* compression error */
                err.Format("Compress error-code %d", comprErr );
            } else {
                /* ODBC error */
                m_SAPCSConHdl->SQLErr(err, connection, stmtType);
            }; /* else */
            err.AddPrefix("AppendComp ContentStorage, CompressBuffer failed");
            XTRACE( err );
            return INTERNAL_ERROR;
        }; /* if */
    } else if (firstStrLen >= 0) {
        // old value is uncompressed in short column
        storedCompLen = totalLen;
    }; /* else */

    bool useShortField = ((firstStrLen >= 0)
                          && allBytesCompressed
                          && (storedCompLen <= compValLen));

	SQLBindParameter( updateHstmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_DECIMAL,
                      0, 0, &totalLen, 0, NULL);
    SQLBindParameter( updateHstmt, 2,
                      SQL_PARAM_INPUT,
                      SQL_C_BINARY,
                      SQL_VARBINARY,
                      0, 0, (SQLPOINTER) 3, 0, &lenVal);
    SQLBindParameter( updateHstmt, 3,
                      SQL_PARAM_INPUT,
                      SQL_C_BINARY,
                      SQL_LONGVARBINARY,
                      0, 0, (SQLPOINTER) 4, 0, &lenVal);

	SQLBindParameter( updateHstmt, 4, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_DATE,
                      0, 0, (char*) CS_CAST(ZString&) compDateM, 0, &lenDate);
	SQLBindParameter( updateHstmt, 5, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_TIME,
                      0, 0, (char*) CS_CAST(ZString&) compTimeM, 0, &lenTime);
	SQLBindParameter( updateHstmt, 6, SQL_PARAM_INPUT,
                      SQL_C_CHAR, SQL_CHAR, 0, 0, &compressFlag, 0, &lenCompressFlag);

	SQLBindParameter( updateHstmt, 7, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARCHAR,
                      0, 0, (char*) name, 0, &lenName);    

	sqlRetcode = SQLExecute( updateHstmt );
    
    if (sqlRetcode != SQL_NEED_DATA) {
        if (sqlRetcode == SQL_ERROR) {
            m_SAPCSConHdl->SQLErr(err, connection, stmtType);
        };
        err.AddPrefix("AppendComp ContentStorage, SQLExecute: retcode != SQL_NEED_DATA");
        XTRACE( err );
        return INTERNAL_ERROR;
    }; /* if */
    
    sqlRetcode = SQLParamData( updateHstmt, &nextParm );
    if (sqlRetcode != SQL_NEED_DATA) {
        if (sqlRetcode == SQL_ERROR) {
            m_SAPCSConHdl->SQLErr( err, connection, stmtType);
        };
        err.AddPrefix("AppendComp ContentStorage, SQLParamData: retcode != SQL_NEED_DATA");
        XTRACE( err );
        return INTERNAL_ERROR;
    }; /* if */

    int loopCount = 0;
    while (sqlRetcode == SQL_NEED_DATA && loopCount++ < MAX_PARAMDATA_LOOP) {
        switch ((long) nextParm) {
        case 3 :
            if (useShortField) {
                /* insert data in column 3 */
                if (oldComprType != NOT_COMPRESSED_PA110) {
                    /* we already compressed above */
                    sqlRetcode = compr.FlushBuffer();
                } else { // store uncompressed
                    // put old value
                    if (oldLen > 0) {
                        sqlRetcode = SQLPutData( updateHstmt, oldValue, oldLen );
                    }; // if
                    // put appended value
                    if (sqlRetcode == SQL_NEED_DATA || SUCCESS(sqlRetcode)) {
                        sqlRetcode = SQLPutData( updateHstmt, (SQLPOINTER) value, lValue );
                    };
                }; /* else */
            } else {
                /* insert null value in column 3 */
                sqlRetcode = SQLPutData( updateHstmt, NULL, lenNull );
            }; /* else */
            break;
        case 4 :
            if (useShortField) {
                /* insert null value in column 4 */
                sqlRetcode = SQLPutData( updateHstmt, NULL, lenNull );
            } else {
                /* insert data in column 4 */
                if (oldComprType != NOT_COMPRESSED_PA110) {
                    // write block compressed data
                    if (firstStrLen >= 0) {
                        // data was in short column.
                        sqlRetcode = compr.FlushBuffer();
                        if (!allBytesCompressed) {
                            /* send the rest data */
                            if (totalBytesRead < oldLen) {
                                sqlRetcode = compr.PutData( &oldValue[totalBytesRead],
                                                            (int) oldLen-totalBytesRead, &comprErr );
                                if (sqlRetcode == SQL_ERROR) {
                                    m_SAPCSConHdl->SQLErr( err, connection, stmtType);
                                    XTRACE( err );
                                    return INTERNAL_ERROR;
                                }; // if
                                totalBytesRead += oldLen-totalBytesRead;
                            }; // if
                            sqlRetcode = compr.PutData( (SQLPOINTER) &value[totalBytesRead-oldLen],
                                                        lValue + oldLen - totalBytesRead, &comprErr );
                        };
                    } else {
                        // data was in long column
                        if (!useAppend) {
                            if (oldComprType == SINGLE_BLOCK_PA110) {
                                unsigned char compHeader[GENERAL_HEADER_SIZE_PA110
                                                        + OWN_HEADER_SIZE_PA110];
                                // prepend general and first block header
                                pa110SetOneBlockHeader( compHeader, oldCompressedLen + sizeof(compHeader) );
                                sqlRetcode = SQLPutData( updateHstmt,
                                                         compHeader,
                                                         sizeof(compHeader) );
                            };
                            // send old compressed value
                            if (sqlRetcode == SQL_NEED_DATA || SUCCESS(sqlRetcode)) {
                                sqlRetcode = SQLPutData( updateHstmt, oldValue, oldCompressedLen );
                            };
                        };
                        if (sqlRetcode == SQL_NEED_DATA || SUCCESS(sqlRetcode)) {
                            sqlRetcode = compr.InitComprAppend( updateHstmt, &comprErr );
                            if (SUCCESS(sqlRetcode)) {
                                if (compComprType == SINGLE_BLOCK_PA110) {
                                // put block of already compressed data
                                    sqlRetcode = compr.PutBlock( (SQLPOINTER) value, lValue, &comprErr );
                                } else { // compComprType == UNCOMPRESSED_PA110
                                // write data with compression
                                    sqlRetcode = compr.PutData( (SQLPOINTER) value, lValue, &comprErr );
                                };
                            }; // if
                        }; // if
                    }; // else
                } else {
                    // write uncompressed data
                    if (!useAppend && oldLen > 0) {
                        sqlRetcode = SQLPutData( updateHstmt, oldValue, oldLen );
                    }; // if
                    if (sqlRetcode == SQL_NEED_DATA || SUCCESS(sqlRetcode)) {
                        sqlRetcode = SQLPutData( updateHstmt, (SQLPOINTER) value, lValue );
                    };
                }; /* else */
            }; /* else */            
            break;
        default:
            /* unexpected column no */
            err.AddPrefix("AppendComp ContentStorage, unexpected column no");
            XTRACE( err );
            return INTERNAL_ERROR;
            break;
        }; /* switch */
        if (sqlRetcode != SQL_ERROR) {
            sqlRetcode = SQLParamData( updateHstmt, &nextParm );
        }; /* if */
    }; /* while */

    if (loopCount >= MAX_PARAMDATA_LOOP) {
        /* we should walk at least twice through the previous loop */
		err.AddPrefix("AppendComp ContentStorage, Loop count exceeded");
        XTRACE( err );

        return INTERNAL_ERROR;
    }; /* if */
    
	if (!SUCCESS(sqlRetcode))
	{
		err.SetBuf("AppendComp ContentStorage, StmtType %d ", stmtType);
		if (m_SAPCSConHdl->SQLErr(err, connection, stmtType) == SAPCS_ERR_DUPREC)
		{
			SQLFreeStmt( updateHstmt, SQL_CLOSE);
			return DUPLICATE_KEY;
		}
		else
		{
			SQLFreeStmt( updateHstmt, SQL_CLOSE);
            XTRACE( err );
			return INTERNAL_ERROR;
		}
	}; // if

	SQLFreeStmt( updateHstmt, SQL_CLOSE );
	return OK;
}; // AppendComp


SQLRETURN ContentStorage::GetColumnLen( SQLHSTMT hstmt, int colNo, long &length )
{
    SQLRETURN      sqlRetcode = SQL_SUCCESS;
    SQLLEN         l_length = (SQLLEN) length;

    sqlRetcode = SQLColAttributes( hstmt, colNo, SQL_COLUMN_PRECISION,
                                   NULL, 0, NULL, &l_length );
    length = (long) l_length;

    return sqlRetcode;
}; // GetColumnLen


SQLRETURN ContentStorage::GetParamLen( SQLHSTMT hstmt, int paramNo, long &length )
{
    SQLRETURN      sqlRetcode = SQL_SUCCESS;
    SWORD          sqlType;
    SWORD          scale;
    SWORD          nullable;
    SQLULEN        precision;

    sqlRetcode = SQLDescribeParam( hstmt, paramNo,
                                   &sqlType, &precision, &scale, &nullable );
    if (SUCCESS(sqlRetcode) && precision <= LONG_MAX) {
        length = (long) precision;
    } else {
        length = 0;
    }; // else

    return sqlRetcode;
}; // GetParamLen


SQLRETURN DynSQLGetData( SQLHSTMT      hstmt,
                         SQLUSMALLINT  colNo,
                         SQLSMALLINT   targetType,
                         SQLPOINTER   *targetValuePtrPtr,
                         SQLLEN        bufferLength,
                         SQLLEN       *strLenOrIndiPtr )
{
    SQLRETURN   sqlRetcode;
    SQLRETURN   errRetcode;
    SQLPOINTER  valuePtr;
    SQLLEN      bufLen;
    SQLLEN      strLenOrIndi;
    bool        moreData;
    UCHAR      *newBuf;
    UCHAR      *bufBeginPtr;
    SQLINTEGER  totalBufLen;
    SQLINTEGER  newBufLen;
    
    UCHAR       sqlState[10];
    SQLINTEGER  nativeErr;
    UCHAR       errorMsg[100];
    SQLSMALLINT msgLen;

    const int   maxLoop = 1000; // prevent infinite loop
    int         loopCount = 0; 

	*strLenOrIndiPtr  = 0;
    valuePtr    = *targetValuePtrPtr;
    bufBeginPtr = (UCHAR*) *targetValuePtrPtr;
    bufLen      = bufferLength;
    totalBufLen = bufferLength;
    moreData = false;
    do {
        loopCount++;
        sqlRetcode = SQLGetData( hstmt, colNo,
                                 targetType, valuePtr,
                                 bufLen, &strLenOrIndi );
        moreData = false;
        if (sqlRetcode == SQL_SUCCESS_WITH_INFO) {
            // get sqlstate value
            errRetcode = SQLError( SQL_NULL_HENV, SQL_NULL_HDBC,
                                   hstmt,
                                   sqlState,
                                   &nativeErr,
                                   errorMsg,
                                   sizeof(errorMsg),
                                   &msgLen );
            if (errRetcode != SQL_SUCCESS) {
                // unexpected return code
                sqlRetcode = SQL_ERROR;
                break;
            }; // if
            if (strcmp( (char*) sqlState, "01004" ) == 0) {
                // Data truncated
                // allocate new bigger buffer
				*strLenOrIndiPtr += bufLen;
                moreData = true;
                if (strLenOrIndi == SQL_NO_TOTAL) {
                    newBufLen = totalBufLen*2;
                } else {
                    newBufLen = totalBufLen-bufLen+strLenOrIndi;
                }; /* else */
                newBuf = new UCHAR[newBufLen];
                if (newBuf != NULL) {
                    memcpy( newBuf, bufBeginPtr, totalBufLen );
                    delete[] bufBeginPtr;
                    bufBeginPtr = newBuf;
                    valuePtr = &newBuf[totalBufLen]; // SQL_C_BINARY
                    bufLen = newBufLen-totalBufLen;
                    totalBufLen = newBufLen;
                } else {
                    // memory allocation failure
                    sqlRetcode = SQL_ERROR;
                    break;
                }; // else
            }; // if
        }; // if
    } while ( (loopCount < maxLoop)
              && (sqlRetcode == SQL_SUCCESS_WITH_INFO)
              && moreData);
    if (loopCount < maxLoop) {
		*strLenOrIndiPtr += strLenOrIndi;
        *targetValuePtrPtr = bufBeginPtr;
    } else {
        sqlRetcode = SQL_ERROR;
    }; // else

    return sqlRetcode;
}; // DynSQLGetData


SQLRETURN PosSQLGetData( SQLHSTMT      hstmt,
                         SQLUSMALLINT  colNo,
                         SQLSMALLINT   targetType,
                         SQLPOINTER    targetValuePtr,
                         SQLINTEGER    bufferLength,
                         SQLLEN       *strLenOrIndiPtr,
                         SQLLEN        readPos )
{
    SQLRETURN   sqlRetcode;
    SQLRETURN   errRetcode;
    char       *garbageBuffer = NULL; // buffer for the data until readPos
    char        garbageChar;

    UCHAR       sqlState[10];
    SQLINTEGER  nativeErr;
    UCHAR       errorMsg[100];
    SQLSMALLINT msgLen;

    if (readPos <= 1) {
        sqlRetcode = SQLGetData( hstmt, colNo, targetType, targetValuePtr,
                                 bufferLength, strLenOrIndiPtr );
    } else {
        // first check for null value
        sqlRetcode = SQLGetData( hstmt, colNo, targetType, &garbageChar,
                                 1, strLenOrIndiPtr );
        if (*strLenOrIndiPtr != SQL_NULL_DATA) {
            // non null value, we already skipped one byte
            garbageBuffer = new char[readPos-2];
            if (garbageBuffer == NULL) {
                // memory allocation failure
                sqlRetcode = SQL_ERROR;
            } else {
                // skip the next readPos-2 bytes
                sqlRetcode = SQLGetData( hstmt,
                                         colNo, targetType,
                                         garbageBuffer,
                                         readPos-2,
                                         strLenOrIndiPtr );
                delete[] garbageBuffer;
                if (sqlRetcode == SQL_SUCCESS_WITH_INFO) {
                    // get sqlstate value
                    errRetcode = SQLError( SQL_NULL_HENV, SQL_NULL_HDBC,
                                           hstmt,
                                           sqlState,
                                           &nativeErr,
                                           errorMsg,
                                           sizeof(errorMsg),
                                           &msgLen );
                    if (errRetcode != SQL_SUCCESS) {
                        // unexpected return code
                        sqlRetcode = SQL_ERROR;
                    } else if (strcmp( (char*) sqlState, "01004" ) == 0) {
                        // Data truncated. Get the data starting at readPos
                        sqlRetcode = SQLGetData( hstmt, colNo,
                                                 targetType, targetValuePtr,
                                                 bufferLength,
                                                 strLenOrIndiPtr );
                    } else {
                        // unexpected error
                        sqlRetcode = SQL_ERROR;
                    }; // else
                } else if (sqlRetcode == SQL_SUCCESS) {
                    // the column contains less than readPos bytes
                    sqlRetcode = SQL_NO_DATA_FOUND;
                }; // else
            }; // else
        } else {
            sqlRetcode = SQL_NO_DATA_FOUND;
        }; // else
    }; // else

    return sqlRetcode;
}; // PosSQLGetData


int ContentStorage::GetDbVersion( SQLHDBC hdbc )
{
    SQLRETURN sqlRetcode = SQL_SUCCESS;
    UCHAR     dbmsver[20];
    SWORD     strLen = 0;
    int       dbVersion = DB_VERSION_UNKNOWN;

    sqlRetcode = SQLGetInfo( hdbc, SQL_DBMS_VER, dbmsver, sizeof(dbmsver), &strLen );
    if (SUCCESS(sqlRetcode)) {
        if ((strlen((char*)dbmsver) >= 4)
            && (dbmsver[0] == '0') && (dbmsver[1] == '7')) {
            if ((dbmsver[3] == '0') && (dbmsver[4] == '1')) {
                dbVersion = DB_VERSION_71;
            } else if ((dbmsver[3] == '0') && (dbmsver[4] == '2')) {
                dbVersion = DB_VERSION_72;
            }; // else
		}; // if
    }; // if
        
    return dbVersion;
}; // GetDbVersion

tpa110_ComprType FlagToComprType( char flag )
{
    tpa110_ComprType comprType = NOT_COMPRESSED_PA110;

    switch (flag) {
    case BLOCKCOMPRESSED_FLAG:
        comprType = BLOCK_WISE_PA110;
        break;
    case CSCOMPRESSED_FLAG:
        comprType = SINGLE_BLOCK_PA110;
        break;
    case UNCOMPRESSED_FLAG:
        comprType = NOT_COMPRESSED_PA110;
        break;
    default:
        // Unknown compress-flag
        comprType = NOT_COMPRESSED_PA110;
        break;
    }; // switch

    return comprType;
}; // FlagToComprType


void UpperCase( char *str )
{
    char *pos = str;
    
    while ( *pos != '\0' ) {
        *pos = toupper( *pos );
		pos++;
    } ; /* while */
} /* UpperCase */
