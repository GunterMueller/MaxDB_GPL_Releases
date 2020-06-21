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
#if !defined(STORAGE_H)
#define STORAGE_H

// ******************************************************************************
class Options;
// ******************************************************************************
class Storage  
{
public:
	typedef enum
	{
		OK							= 0,
		KEY_NOT_FOUND				= 1,
		CANNOT_OPEN_CONTREP			= 2,
		FILE_READ_ERROR				= 3,
		FILE_WRITE_ERROR			= 4,
		ERROR_CREATING_DIRECTORY	= 5,
		CANNOT_CREATE_DIRECTORY		= 6,
		PATH_NOT_FOUND				= 7,
		DIRECTORY_ALREADY_EXISTS	= 8,
		CANNOT_DELETE_DOC			= 9,
		CANNOT_DELETE_COMP			= 10,
		INTERNAL_ERROR				= 11,
		DUPLICATE_KEY				= 12,
		COMP_ALREADY_EXISTS			= 13,
		FATAL_ERROR					= 14,
		STORAGE_OPEN_ERROR			= 15,
		DOC_ALREADY_EXISTS			= 16,
		NOT_IMPLEMENTED				= 17
	} Error;

	typedef enum
	{
		ONLINE = 0,
		OFFLINE,
		DAMAGED
	} DataStatus;

	typedef long TransID;

public:

virtual	Error	Version(ZString& version,ZString& identification,ZString& build,ZString& err)  = 0;

				Storage()		{};
virtual			~Storage()		{};
virtual	Error	Init( Options* options, const ZString& homeDir,ZString& err) = 0;

virtual	Error	Create( const ZString& contRep, const ZString& contRepFullPath, const ZString& sectionName,ZString& err)	= 0;
virtual Error	Open( const ZString& contRep, const ZString& contRepFullPath, const ZString& sectionName, bool readOnly,ZString& err)	= 0;
virtual	Error	Close(ZString& err)	= 0;
virtual	Error	DeleteContRep( const ZString& contRep, const ZString& contRepFullPath, const ZString& sectionName,ZString& err)	= 0;

virtual Error	BeginTransaction(TransID& tid,ZString& err) = 0;
virtual Error	EndTransaction(TransID tid,bool commit,ZString& err) = 0;

virtual	Error	FetchDoc(TransID tid,bool forUpdate,const ZString& docId,DataStatus& docStatus,
						 bool deCompress,char*& property,int& lProperty,char*& value,int& lValue,bool& isCSCompressed,
						 ZString& err)	= 0;
virtual	Error	FetchComp(TransID tid,bool forUpdate,const ZString& docId,const ZString& compId,
						  int fromOffset,int toOffset,DataStatus& compStatus,
						  bool deCompress,char*& value,int& lValue,bool& isCSCompressed,
						  ZString& err)	= 0;

virtual	Error	StoreDoc(TransID tid,const ZString& docId,
						 const char* property, int lProperty, const char* value,int lValue,bool isCSCompressed,bool doCSCompress,
						 const ZString& dateM, const ZString& timeM,
						 ZString& err)	= 0;
virtual	Error	StoreComp(TransID tid, const ZString& docId, const ZString& compId,bool doCreate,bool doAppend,
						  const char* value,int lValue,bool isCSCompressed,bool doCSCompress,
						  const ZString& compDateM, const ZString& compTimeM, const ZString& contentType,
						  ZString& err)	= 0;

virtual	Error	DeleteDoc(TransID tid, const ZString& docId,ZString& err)	= 0;
virtual	Error	DeleteComp(TransID tid, const ZString& docId, const ZString& compId,ZString& err)	= 0;

virtual Error	DeleteBuf(TransID tid,char*& buf,ZString& err)	= 0;

virtual Error	GetDocIDInit(TransID tid,bool docIdOnly,ZString& err)	= 0;
virtual Error	GetDocIDNext(TransID tid,ZString& docId,ZString& compId,bool& endOfList,ZString& err)	= 0;
virtual Error	GetDocIDEnd(TransID tid,ZString& err)	= 0;

virtual Error	GetFreeSpaceMB(TransID tid,double& freeSpaceMB,ZString& err)	= 0;

virtual Error	GetDocStats(TransID tid,double& numDocs,double& numComps,double& sizeCompsMB,ZString& err)	= 0;
};
// ******************************************************************************
#ifdef  StorageImport
#undef  StorageImport
#define StorageImport __declspec(dllimport)
#else
#define StorageImport
#endif
#ifdef  StorageExport
#undef  StorageExport
#define StorageExport __declspec(dllexport)
#else
#define StorageExport
#endif
// ****************************************************************************** 
extern "C"
{
Storage::Error StorageImport StorageExport  NewStorage( const char* name,Options* options, const ZString& homeDir,Storage*& storage,ZString& err);
Storage::Error StorageImport StorageExport  DeleteStorage(Storage* storage,ZString& err);
}

typedef Storage::Error (*FuncNewStorage)(char* name,Options* options,ZString& homeDir,Storage*& storage,ZString& err);
typedef Storage::Error (*FuncDeleteStorage)(Storage* storage,ZString& err);

// ******************************************************************************



#endif // !defined(STORAGE_H)
