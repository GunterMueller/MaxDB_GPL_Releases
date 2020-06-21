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
#if !defined(CONTENT_STORAGE_H)
#define CONTENT_STORAGE_H


// ******************************************************************************

#include <WINDOWS.H>
#include <stdlib.h>
#include <sql.h>
#include <sqlext.h>

#include "ContentStorage/Storage.h"
#include "ContentStorage/ZString.h"
#include "ContentStorage/Options.h"
#include "ContentStorage/ContentStorageDefines.h"
#include "ContentStorage/SAPCSConHdl.h"
#include "ContentStorage/TRACE.h"

class TableDef
{
public:
		long			version;
		const ZString	docDef;
		const ZString	compDef;
		TableDef(long version, const char* idocDef, const char* icompDef );
};



class ContentStorage : public Storage  
{
protected:

		Options*		m_Options;
		ZString			m_HomeDir;
		SAPCSConHdl*	m_SAPCSConHdl;
		ZString			m_SectionName;
		ZString			m_ContRepName;
		ZString			m_ContRepNr;
        CSMutex         m_Lock;

		bool			m_TableCheck;
		TableDef*		m_TableDef;
        long            m_DocNameLen;
public:

virtual	Error	Version(ZString& version,ZString& identification,ZString& build,ZString& err);

				ContentStorage();
virtual			~ContentStorage();
virtual Error	Init(Options* options, const ZString& homeDir,ZString& err);

virtual	Error	Create( const ZString& contRep, const ZString& contRepFullPath, const ZString& sectionName,ZString& err);
virtual Error	Open( const ZString& contRep, const ZString& contRepFullPath, const ZString& sectionName, bool readOnly,ZString& err);
virtual	Error	Close(ZString& err);
virtual	Error	DeleteContRep( const ZString& contRep, const ZString& contRepFullPath, const ZString& sectionName,ZString& err);

virtual Error	BeginTransaction(TransID& tid,ZString& err);
virtual Error	EndTransaction(TransID tid,bool commit,ZString& err);

virtual	Error	FetchDoc(TransID tid,bool forUpdate, const ZString& docId,DataStatus& docStatus,
						 bool deCompress,char*& property,int& lProperty,char*& value,int& lValue,bool& isCSCompressed,
						 ZString& err);
virtual	Error	FetchComp(TransID tid,bool forUpdate, const ZString& docId, const ZString& compId,
						  int fromOffset,int toOffset,DataStatus& compStatus,
						  bool deCompress,char*& value,int& lValue,bool& isCSCompressed,
						  ZString& err);

virtual	Error	StoreDoc(TransID tid, const ZString& docId,
						 const char* property,int lProperty, const char* value,int lValue,bool isCSCompressed,bool storeCompressed,
						 const ZString& dateM, const ZString& timeM,
						 ZString& err);
// StoreComp
// ==========
// doAppend == true :
// - if the component does not exist then a new component is created
// - storeCompressed and contentType are ignored if the component already exists
// - if the component exists the COMP_DATEM and COMP_TIMEM fields are updated and
//   value is appended to the SHORT_VALUE/LONG_VALUE field.
// - value is compressed before appending if the existing value is compressed
virtual	Error	StoreComp(TransID tid, const ZString& docId, const ZString& compId,bool doCreate,bool doAppend,
						  const char* value,int lValue,bool isCSCompressed,bool storeCompressed,
						  const ZString& compDateM, const ZString& compTimeM, const ZString& contentType,
						  ZString& err);

virtual	Error	DeleteDoc(TransID tid, const ZString& docId,ZString& err);
virtual	Error	DeleteComp(TransID tid, const ZString& docId, const ZString& compId,ZString& err);

virtual Error	DeleteBuf(TransID tid,char*& buf,ZString& err);

virtual Error	GetDocIDInit(TransID tid,bool docIdOnly,ZString& err);
virtual Error	GetDocIDNext(TransID tid,ZString& docId,ZString& compId,bool& endOfList,ZString& err);
virtual Error	GetDocIDEnd(TransID tid,ZString& err);

virtual Error	GetFreeSpaceMB(TransID tid,double& freeSpaceMB,ZString& err);

virtual Error	GetDocStats(TransID tid,double& numDocs,double& numComps,double& sizeCompsMB,ZString& err);

    virtual Error   SetDocNameLen( long docNameLen );
protected: 
virtual	Error	TableCheck(ZString& err);		
virtual bool	ExistsTab(ZString& tabName, ZString& err);
virtual bool	CreateTab(ZString& sqlStmtCre, ZString& err);  
virtual bool	CreateNewContRep(ZString& contRepName, long contRepNr, ZString& err);
virtual bool	GetContRepInfo( const ZString& contRepName, long& contRepNr, long& version, ZString& err);
virtual bool	GetDbParam( const char *description, long &value, ZString& err);
virtual bool	GetHighestContRepNr(long& contRepNr, ZString& err);
virtual void	NrToStr(long nr, ZString& nrStr);
virtual bool	PrepareStmt(DbConnection connection, ZString& err);
/*
       Function: GetCompAllocLen

       description: Return the maximum needed buffer-size for a component.
*/
 long GetCompAllocLen( long docLen, long fromOffset, long toOffset );

 Error FetchCompOpenResult( TransID           tid,
                            ZString          &name,
                            StmtType          stmtType,
                            SQLHSTMT         &hstmt,
                            SQLHDBC          &hdbc,
                            SQLLEN           &uncompressedLen,  /* PTS 1119292 */
                            tpa110_ComprType &isCSCompressed,
                            ZString          &err );
    
 Error	AppendComp(TransID tid,const ZString& docId, const ZString& compId,
                   const char* value,int lValue,bool isCSCompressed,
                   const ZString& compDateM, const ZString& compTimeM,
                   ZString& err); 
 
 SQLRETURN GetColumnLen( SQLHSTMT hstmt, int colNo, long &length );
 SQLRETURN GetParamLen( SQLHSTMT hstmt, int paramNo, long &length );
    /* return DB_VERSION_71 or DB_VERSION_72 */
 int GetDbVersion( SQLHDBC hdbc );
}; // class ContentStorage

#endif // !defined(CONTENT_STORAGE_H)
