/*!
 * \file    OMS_DummyKernelCallback.cpp
 * \author  MarkusSi, PeterG, Roterring
 * \brief   Dummy kernel sink
 */
/*

    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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



#ifndef OMS_DUMMYKERNELCALLBACK_HPP
#define OMS_DUMMYKERNELCALLBACK_HPP

#include "liveCache/LVC_IliveCacheSink.hpp"
#include "ggg01.h"
//#include "ggg00.h"

/*! 
  Class: OMS_DummyKernelCallback

  Description: this class yields a dummy implementation of the kernel sink.
               All interface functions simply return ok.
*/

class OMS_DummyKernelCallback : public IliveCacheSink 
{
    virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
    /* [in] */ REFIID riid,
    /* [iid_is][out] */ void  **ppvObject) { return S_OK; }
    virtual ULONG STDMETHODCALLTYPE AddRef( void) { return S_OK; }
    virtual ULONG STDMETHODCALLTYPE Release( void) { return S_OK; }

  virtual HRESULT STDMETHODCALLTYPE ABAPRead( 
    tgg01_StreamDescriptor &descriptor,
    short                  &error){ return S_OK; }
  
  virtual HRESULT STDMETHODCALLTYPE ABAPWrite( 
    tgg01_StreamDescriptor &descriptor,
    short                  &error){ return S_OK; }
    
  virtual HRESULT STDMETHODCALLTYPE AllocPacket( 
    unsigned char  **p,
    tsp00_Int4      *sz){ return S_OK; }

  virtual HRESULT STDMETHODCALLTYPE CloseVersion( 
    short        *lpb_error){ return S_OK; }
    
  virtual HRESULT STDMETHODCALLTYPE ConsoleMsg( 
    tsp00_Int4      buflen,
    unsigned char  *lpv){ return S_OK; }

  virtual HRESULT STDMETHODCALLTYPE CreateVersion( 
    unsigned char  *lpConsistentView,
    unsigned char  *lpVersionContext,
    short          *lpb_error){ return S_OK; }

   virtual HRESULT STDMETHODCALLTYPE CreateKeyRangeIterator(
    tgg91_TransNo            &consistentView,
    tgg00_FileId             &objFile,
    tgg01_OmsVersionContext  *lpVersionContext,
    void                    **pKeyRangeIterator,
    tsp00_Int4                objBodySize,
    size_t                    keyLen,
    const void               *pStartKey,              
    const void               *pLowerKey,             
    const void               *pUpperKey,              
    tsp00_Int4               &noOfOid,
    SAPDB_UInt8              *pResultOId,  
    tgg91_PageRef            *pObjSeq,
    unsigned char            *pBody,
    tsp00_Int4               &LogReadAccesses,
    tgg00_BasisError         &e,
    OmsIterDirection          direction = OMS_ASCENDING){ return S_OK; }

  virtual HRESULT STDMETHODCALLTYPE StartUnloadedVersionIter(
    tgg91_TransNo           &consistentView,
    tgg01_OmsVersionContext *lpVersionContext,
    tgg00_FileId            &objFile,
    size_t                   keyLen,
    void                    *pStartKey,            
    const void              *pLowerKey,
    const void              *pUpperKey,
    size_t                   ObjBodySize,           
    void                    *pObjBody,             
    SAPDB_UInt8             &ResultOId,  
    short                   &e,
    OmsIterDirection        direction){ return S_OK; }             
  
  virtual HRESULT STDMETHODCALLTYPE DeleteAllObj(
    tgg91_TransNo&  consistentView,
    unsigned char  *lpContainerId,
    unsigned char  *lpVersionFile,
    int            &lpDeleted,
    short          &lpb_error,
    SAPDB_UInt8    &errorOid){ return S_OK; }

  virtual HRESULT STDMETHODCALLTYPE DeleteObj( 
    unsigned char  *lpConsistentView,
    unsigned char  *lpContainerId,
    unsigned char  *lpVersionContext,
    unsigned char  *lpoid,
    unsigned char  *lpobjseq,
    bool            reused,   // PTS 1127661
    bool            contObj,  // PTS 1124278
    short          *lpb_error){ return S_OK; }

  virtual HRESULT STDMETHODCALLTYPE DeleteObjMass(
    tsp00_Int4               cnt,
    tgg91_TransNo           *pConsistentView,
    tgg01_ContainerId      **ppContainerId,
    unsigned char           *pVersionContext,
    SAPDB_UInt8              *pOid,
    tgg91_PageRef           *pObjSeq,
    bool                    *pReused, // PTS 1127661
    bool                    *pContObj,// PTS 1124278
    bool                     clustered,
    tsp00_Int4              &errCnt,
    tgg00_BasisError        *pDBError){ return S_OK; }

  virtual HRESULT STDMETHODCALLTYPE DestroyKeyRangeIterator(
    void* pKeyRangeIterator){ return S_OK; }
  
  virtual HRESULT STDMETHODCALLTYPE EndConsistentView( 
    unsigned char  *lpConsistentView,
    short          *lpb_error){ return S_OK; }

  virtual HRESULT STDMETHODCALLTYPE EnterCriticalSection(
    short regionId){ return S_OK; }
 
  virtual HRESULT STDMETHODCALLTYPE GetContainerId(    
    tgg01_ContainerId              *pcontainerId, 
    const tgg01_OmsVersionContext  *pVersionContext,
    tsp00_Int4                      noOfOid,
    const SAPDB_UInt8              *pOid,
    tgg00_BasisError               *pDBError){ return S_OK; }

  virtual HRESULT STDMETHODCALLTYPE GetObj( 
    tgg91_TransNo                 &consistentView,
    tgg01_ContainerId             &containerId, 
    const tgg01_OmsVersionContext *pVersionContext,
    const SAPDB_UInt8             &oid,
    bool                           doLock,
    bool                           shared,
    tgg91_PageRef                 &objSeq,
    tgg91_TransNo                 &updTransId,
    size_t                         bodySize,
    unsigned char                 *pBody,
    size_t                        &bodyLen,
    tsp00_Int4                    &histLogCount,
    tgg00_BasisError              &e){ return S_OK; } 

  virtual HRESULT STDMETHODCALLTYPE GetObjects(
    tgg91_TransNo                 &consistentView,
    tgg01_ContainerId             *pContainerId, 
    const tgg01_OmsVersionContext *pVersionContext,
    tsp00_Int4                     noOfOid,
    const SAPDB_UInt8             *pOid,
    tgg91_PageRef                 *pObjVers,
    tgg91_TransNo                 *pUpdTransId, 
    size_t                        *pBodySize,
    bool                           doLock,
    bool                           shared,
    unsigned char                **ppObj,
    tsp00_Int4                    &histLogCount,
    tsp00_Int4                    &errorCount,
    tgg00_BasisError              *e){ return S_OK; }

  virtual HRESULT STDMETHODCALLTYPE GetObjWithKey( 
    unsigned char  *lpConsistentView,
    unsigned char  *lptree,
    unsigned char  *lpVersionContext,
    size_t          keylen,
    unsigned char  *lpkey,
    bool            doLock,
    bool            shared,
    size_t          bodysize,
    unsigned char  *lpbody,
    SAPDB_UInt8    *lpoid,
    unsigned char  *lpobjseq,
    tgg91_TransNo  *pUpdTransId,
    int            *hist_log_count,
    short          *lpb_error){ return S_OK; }

  virtual HRESULT STDMETHODCALLTYPE GetObjectsWithKey( 
    tgg91_TransNo            *pConsistentView,
    tgg01_ContainerId        *pContainerId,
    tgg01_OmsVersionContext  *pVersionContext,
    size_t                   *pKeyLen,
    tsp00_Int4                noOfObj,
    unsigned char           **ppKey,
    bool                      doLock,
    bool                      shared,
    size_t                   *pBodySize,
    unsigned char           **ppBody,
    SAPDB_UInt8              *pOid,
    tgg91_PageRef            *pObjSeq,
    tgg91_TransNo            *pUpdTransId,
    tsp00_Int4               &histLogCnt,
    tsp00_Int4               &errorCnt,
    tgg00_BasisError         *pDBError){ return S_OK; }

  virtual HRESULT STDMETHODCALLTYPE HResultError( 
    HRESULT h_error){ return S_OK; }

  virtual HRESULT STDMETHODCALLTYPE IsLocked( 
    unsigned char  *lpContainerId,
    SAPDB_UInt8    *lpoid,
    bool            shared,
    short          *lpb_error){ return S_OK; }
  
  virtual HRESULT STDMETHODCALLTYPE LeaveCriticalSection(
    short regionId){ return S_OK; }

  virtual HRESULT STDMETHODCALLTYPE LockObj( 
    unsigned char  *lpConsistentView,
    unsigned char  *lpContainerId,
    SAPDB_UInt8    *lpoid,
    unsigned char  *lpobjseq,
    short          *lpb_error){ return S_OK; }

  virtual HRESULT STDMETHODCALLTYPE LockObjMass(
    tsp00_Int4              &cnt,
    tgg91_TransNo           *pConsistentView,
    tgg01_ContainerId      **ppContainerId,
    SAPDB_UInt8             *pOid,
    tgg91_PageRef           *pObjSeq,
    tsp00_Int8              &timeout,
    tsp00_Int4              &errCnt,
    tgg00_BasisError        *pDBError){ return S_OK; }
 
  virtual HRESULT STDMETHODCALLTYPE MultiPurpose( 
    unsigned char   MessType,
    unsigned char   MessType2,
    tsp00_Int4     *lpbuflen,
    unsigned char  *lpv,
    short          *lpb_error){ return S_OK; }

  virtual HRESULT STDMETHODCALLTYPE NewConsistentView( 
    bool            isOmsVersion,           /* PTS 1130354 */
    unsigned char  *lpConsistentView,
    short          *lpb_error){ return S_OK; }

  virtual HRESULT STDMETHODCALLTYPE NewObj( 
    unsigned char  *lpConsistentView,
    unsigned char  *lpContainerId,
    unsigned char  *lpVersionContext,
    size_t          KeyPos,
    size_t          KeyColSize,
    unsigned char  *KeyColumn,
    bool            contObj,  // PTS 1124278
    SAPDB_UInt8    *lpoid,
    unsigned char  *lpobjseq,
    tgg91_TransNo  *pUpdTransId,
    short          *lpb_error){ return S_OK; }

  virtual HRESULT STDMETHODCALLTYPE NewObjMass( 
    tsp00_Int4               cnt,
    tgg91_TransNo           *pConsistentView,
    tgg01_ContainerId      **ppContainerId,
    unsigned char           *pVersionContext,
    size_t                  *pKeyPos,
    size_t                  *pKeyColSize,
    unsigned char          **ppKeyColumn, 
    bool                    *pContObj,  // PTS 1124278
    SAPDB_UInt8             *pOid,
    tgg91_PageRef           *pObjSeq,
    tgg91_TransNo           *pUpdTransId,
    tsp00_Int4              &errCnt,
    tgg00_BasisError        *pDBError){ return S_OK; }

  virtual HRESULT STDMETHODCALLTYPE NextOids(
    tgg91_TransNo           &ConsistentView,
    tgg00_FileId            &ContainerId,
    tgg01_OmsVersionContext *pVersionContext,
    bool                     bOnlyHdrVarObjects,
    SAPDB_UInt8              CurrOid,
    tsp00_Int4              &noOfOid,
    SAPDB_UInt8             *pOid,
    tgg91_PageRef           *pObjVers,
    tsp00_Int4              &LogReadAccesses,
    tgg00_BasisError        *pb_error){ return S_OK; }

  virtual HRESULT STDMETHODCALLTYPE NextObjFromKeyIterator (
    const tgg91_TransNo     &ConsistentView,  // PTS 1127520 
    tgg01_ContainerId       &ObjFile,
    tgg01_OmsVersionContext *VersionContext,
    void                    *pKeyIterator,
    tsp00_Int4               objBodySize,
    size_t                   KeyLen,
    void                    *pRestartKey, 
    tsp00_Int4              &NoOfOId,
    SAPDB_UInt8             *pResultOId, 
    tgg91_PageRef           *pObjVers,
    unsigned char           *pBody,
    tsp00_Int4              &objHistReadCount,
    tgg00_BasisError        &e){ return S_OK; }

  virtual HRESULT STDMETHODCALLTYPE VersionNextKey(
    tgg01_OmsVersionContext  *VersionContext,
    const tgg01_ContainerId  &ObjFile,
    void                     *pKey,
    size_t                    KeyLen,
    bool                      Inclusive,
    const void               *pStopKey,
    SAPDB_UInt8              &Oid,
    tgg00_BasisError         &e){ return S_OK; }

  virtual HRESULT STDMETHODCALLTYPE PrevObjFromKeyIterator ( 
    const tgg91_TransNo     &ConsistentView,  // PTS 1127520 
    tgg01_ContainerId       &ObjFile,
    tgg01_OmsVersionContext *VersionContext,
    void                    *pKeyIterator,
    tsp00_Int4               objBodySize,
    size_t                   KeyLen,
    void                    *pRestartKey, 
    tsp00_Int4              &NoOfOId,
    SAPDB_UInt8             *pResultOId, 
    tgg91_PageRef           *pObjVers,
    unsigned char           *pBody,
    tsp00_Int4              &objHistReadCount,
    tgg00_BasisError        &e){ return S_OK; }

  virtual HRESULT STDMETHODCALLTYPE VersionPrevKey(   
    tgg01_OmsVersionContext  *VersionContext,
    const tgg01_ContainerId  &ObjFile,
    void                     *pKey,
    size_t                    KeyLen,
    bool                      Inclusive,
    const void               *pStopKey,
    SAPDB_UInt8              &Oid,
    tgg00_BasisError         &e){ return S_OK; }

  virtual HRESULT STDMETHODCALLTYPE CreateContainer(
    tsp00_Int4           SchemaId,
    GUID                *lpGUID,
    tsp00_KnlIdentifier *lpClassName,
    size_t               size,
    OmsContainerNo       ContainerNo,
    size_t               keyPos,          // PTS 1122540
    size_t               keyLen,          // PTS 1122540
    bool                 keyPartitioned,  // PTS 1122540
    bool                 useCachedKeys,
    bool                 isVarObject,
    bool                 useInternalTrans,
    bool                 noWait,
    unsigned char       *lpContainerId,
    short               *lpb_error){ return S_OK; }

  virtual HRESULT STDMETHODCALLTYPE ResetVersion( 
    unsigned char *lpVersionContext,
    short          &error){ return S_OK; }
 
  virtual HRESULT STDMETHODCALLTYPE SetError( 
    short                 errorno,
    tsp00_Int4            buflen,
    const unsigned char  *lpv,
    tsp00_CodeType        codeType ){ return S_OK; }
  
  virtual HRESULT STDMETHODCALLTYPE SQLRequest( 
    tsp00_Int4      buflen,
    unsigned char  *lpv){ return S_OK; }

  virtual HRESULT STDMETHODCALLTYPE SubtransCommit( 
    short  *lpb_error){ return S_OK; }

  virtual HRESULT STDMETHODCALLTYPE SubtransRollback( 
    short  *lpb_error){ return S_OK; }

  virtual HRESULT STDMETHODCALLTYPE SubtransStart( 
    short  *lpb_error){ return S_OK; }

  virtual HRESULT STDMETHODCALLTYPE UnlockObj( 
    unsigned char  *lpContainerId,
    SAPDB_UInt8    *lpoid,
    short          *lpb_error){ return S_OK; }
  
  virtual HRESULT STDMETHODCALLTYPE UnlockSharedObj( 
    unsigned char  *lpContainerId,
    SAPDB_UInt8    *lpoid,
    short          *lpb_error){ return S_OK; }
  
  virtual HRESULT STDMETHODCALLTYPE UpdateObj( 
    unsigned char  *lpConsistentView,
    unsigned char  *lpContainerId,
    unsigned char  *lpVersionContext,
    unsigned char  *lpoid,
    unsigned char  *lpobjseq,
    size_t          bodysize,
    unsigned char  *lpbody,
    size_t          keyPos,            // PTS 1122540
    size_t      keyLen,            // PTS 1122540
    bool            contObj,           // PTS 1124278
    bool            reused,            // PTS 1127661
    short          *lpb_error){ return S_OK; }

  virtual HRESULT STDMETHODCALLTYPE UpdateObjMass(
    tsp00_Int4               cnt,
    tgg91_TransNo           *pConsistentView,
    tgg01_ContainerId      **ppContainerId,
    unsigned char           *pVersionContext,
    SAPDB_UInt8             *pOid,
    tgg91_PageRef           *pObjSeq,
    size_t                  *pBodySize,
    unsigned char          **ppBody,
    size_t                  *pKeyPos,       // PTS 1122540
    size_t                  *pKeyLen,       // PTS 1122540
    bool                    *pReused,       // PTS 1127661
    bool                     clustered,
    tsp00_Int4              &errCnt,
    tgg00_BasisError        *pDBError){ return S_OK; }

  virtual HRESULT STDMETHODCALLTYPE UpdateVarObjMass(
    tsp00_Int4               cnt,
    tgg91_TransNo           *pConsistentView,
    tgg01_ContainerId      **ppContainerId,
    unsigned char           *pVersionContext,
    SAPDB_UInt8             *pOid,
    tgg91_PageRef           *pObjSeq,
    size_t                  *pObjSize,
    size_t                  *pBodySize,
    unsigned char          **ppBody,
    SAPDB_UInt8             *pNextOid,
    bool                    *pContObj,  // PTS 1124278
    bool                    *pReused,   // PTS 1127661
    tsp00_Int4              &errCnt,
    tgg00_BasisError        *pDBError){ return S_OK; }

  virtual HRESULT STDMETHODCALLTYPE Vtrace( 
    tsp00_Int4      buflen,
    unsigned char  *lpv){ return S_OK; }

  virtual HRESULT STDMETHODCALLTYPE GetDefaultContext(
    void         **p, 
    pasbool      **pCancelByte,
    bool          &optimizedStreamCommunication,
    tsp00_TaskId  &taskId /* PTS 1110315 */){ return S_OK; }
  
  virtual HRESULT STDMETHODCALLTYPE SetDefaultContext(
    void* p){ return S_OK; }

  virtual HRESULT STDMETHODCALLTYPE GetSqlSessionContext(
    void** p,
    tsp00_TaskId  &taskId) { return S_OK; }
        
 virtual HRESULT STDMETHODCALLTYPE SetSqlSessionContext(
   void* p){ return S_OK; }

  virtual HRESULT STDMETHODCALLTYPE OpenFile(
    char        *lpFileName,
    tsp00_Uint4  FileSize,
    boolean      doAppend, 
    tsp00_Int4  *FileHandle,
    short       *lpb_error){ return S_OK; }
    
  virtual HRESULT STDMETHODCALLTYPE WriteFile(
    tsp00_Int4 FileHandle,
    char      *msg,
    short     *lpb_error){ return S_OK; } 

  virtual HRESULT STDMETHODCALLTYPE CloseFile(
    tsp00_Int4  FileHandle,
    short      *lpb_error){ return S_OK; }

  virtual HRESULT STDMETHODCALLTYPE SyncFile(
    char       *lpFileName,
    tsp00_Int4  FileHandle,
    short      *lpb_error){ return S_OK; }

  virtual HRESULT STDMETHODCALLTYPE GetDateTime(
    tsp00_Date* Date,
    tsp00_Time* Time){ return S_OK; }
  
  virtual HRESULT STDMETHODCALLTYPE GetClock(
    tsp00_Int4* Sec,
    tsp00_Int4* MSec){ return S_OK; }

  virtual HRESULT STDMETHODCALLTYPE DropVersion( 
    unsigned char  *lpVersionContext,
    short          *lpb_error){ return S_OK; }
  
  virtual HRESULT STDMETHODCALLTYPE CreateSchema(
    const OmsTypeWyde *lpSchemaName,
    tsp00_Int4        *lpSchemaId,
    short             *lpb_error){ return S_OK; }

  virtual HRESULT STDMETHODCALLTYPE ExistsSchema(
    const OmsTypeWyde *lpSchemaName,
    tsp00_Int4        *lpSchemaId,
    short             *lpb_error){ return S_OK; }

  virtual HRESULT STDMETHODCALLTYPE DropSchema(
    tsp00_Int4  SchemaId,
    short      *lpb_error){ return S_OK; }

  virtual HRESULT STDMETHODCALLTYPE GetSchemaName(
    tsp00_Int4    SchemaId,
    OmsTypeWyde  *lpSchemaName,
    short        *lpb_error){ return S_OK; }

  virtual HRESULT STDMETHODCALLTYPE ExistsFile(
    char    *lpFileName,
    bool    &existsFile,
    bool    &isDirectory,
    short   &error){ return S_OK; }
  
  virtual HRESULT STDMETHODCALLTYPE DropFile(
    char  *lpFileName,
    short &error){ return S_OK; } 

  virtual HRESULT STDMETHODCALLTYPE GetWorkingDirectory(
    tsp00_Pathc &pathName,
    short       &error){ return S_OK; } 

  virtual HRESULT STDMETHODCALLTYPE GetDbrootSapPath(
    tsp00_Pathc &pathName,
    short       &error){ return S_OK; }
    
  virtual HRESULT STDMETHODCALLTYPE LoadLibraryEx( 
    const char    *path, 
    bool           loadGlobal,
    HANDLE        *hd,
    tsp00_ErrText &errtext,
    int            errtext_size){ return S_OK; }

  virtual HRESULT STDMETHODCALLTYPE FreeLibrary( 
    HANDLE         hd,
    bool          &ok,
    tsp00_ErrText &errtext,
    int            errtext_size){ return S_OK; }

  virtual HRESULT STDMETHODCALLTYPE DropContainer(
    OmsSchemaHandle  schema,
    const GUID       &guid,
    OmsContainerNo    cno,
    tgg00_BasisError &error){ return S_OK; }

  virtual HRESULT STDMETHODCALLTYPE GetContainerInfo(
    OMS_ContainerHandle  &containerHandle, //FF 2003-09-23 new FileDir
    GUID            &guid,
    OmsSchemaHandle &schema,
    OmsContainerNo  &cno,
    short           &error){ return S_OK; }

  virtual HRESULT STDMETHODCALLTYPE IsObjHistoryUsed( 
    const SAPDB_UInt8 &oid,
    bool              ignoreVersions,  /*PTS 1130354*/
    bool              &isUsed,
    tgg00_BasisError  &e,
    OmsTypeUInt8      &OldestTransNo /*PTS 1130354*/) { return S_OK; }

  virtual HRESULT STDMETHODCALLTYPE Reschedule(){ return S_OK; } /* PTS 1107849 */

  virtual HRESULT STDMETHODCALLTYPE Sleep(
    const OmsTypeUInt1 sleepTime){ return S_OK; } /* PTS 1107849 */
  
  virtual HRESULT STDMETHODCALLTYPE LockUpdObj( 
    unsigned char   *lpConsistentView,
    unsigned char   *lpContainerId,
    SAPDB_UInt8     *lpoid,
    unsigned char   *lpobjseq,
    short           *lpb_error){ return S_OK; }

  virtual HRESULT STDMETHODCALLTYPE LockUpdObjMass(
    tsp00_Int4               cnt,
    tgg91_TransNo           *pConsistentView,
    tgg01_ContainerId      **ppContainerId,
    SAPDB_UInt8             *pOid,
    tgg91_PageRef           *pObjSeq,
    bool                     clustered,
    tsp00_Int4              &errCnt,
    tgg00_BasisError        *pDBError){ return S_OK; }

  virtual HRESULT STDMETHODCALLTYPE LockSharedObj( 
    unsigned char  *lpConsistentView,
    unsigned char  *lpContainerId,
    SAPDB_UInt8     *lpoid,
    unsigned char  *lpobjseq,
    short          *lpb_error){ return S_OK; }

  virtual HRESULT STDMETHODCALLTYPE LockSharedObjMass(
    tsp00_Int4              &cnt,
    tgg91_TransNo           *pConsistentView,
    tgg01_ContainerId      **ppContainerId,
    SAPDB_UInt8             *pOid,
    tgg91_PageRef           *pObjSeq,
    tsp00_Int8              &timeout,
    tsp00_Int4              &errCnt,
    tgg00_BasisError        *pDBError){ return S_OK; }

  virtual HRESULT STDMETHODCALLTYPE Signal(
    tsp00_TaskId TaskId, 
    tsp00_Bool   ok){ return S_OK; } // PTS 1110315

  virtual HRESULT STDMETHODCALLTYPE Wait(
    tsp00_Bool& ok){ return S_OK; } // PTS 1110315

  virtual HRESULT STDMETHODCALLTYPE ExistsContainer(
    tsp00_Int4            schemaId,
    const GUID           &guid,
    OmsContainerNo        containerNo,
    size_t                objectSize,
    size_t                keyLen,
    tgg01_ContainerId    &containerId,
    bool                 &useCachedKeys,
    bool                 &partitionedKeys,
    short                &error){ return S_OK; } // PTS 1110334

  virtual HRESULT STDMETHODCALLTYPE LockContainer(
      const tgg00_FileId &containerId,
      short              &error){ return S_OK; }

  virtual HRESULT STDMETHODCALLTYPE GetSessionHeapUsage(
    tsp00_8ByteCounter& heapUsage){ return S_OK; } 

  virtual HRESULT STDMETHODCALLTYPE EvalCallStatistics(
     bool            AllocatorOnly,
     OmsMonitorData &MonitorData){ return S_OK; } 

	virtual HRESULT STDMETHODCALLTYPE SimCtlDumpToHtml(
			class OmsHandle     &handle,
			SAPDB_UInt8          obj, 
			class OmsSimDumpCollector	&str)
	{ return S_OK; } 

	virtual HRESULT STDMETHODCALLTYPE SimCtlSetObjectName(
			class OmsHandle  &handle,
			SAPDB_UInt8       obj,
			const char       *name)
	{ return S_OK; }

	virtual HRESULT STDMETHODCALLTYPE SimCtlDestroyHandleCB(
			class OmsHandle				&handle,
			struct IliveCacheSink		**session)
	{
		return S_OK;
	}

    virtual HRESULT STDMETHODCALLTYPE SimCtlGetHTMLRef(
			class OmsHandle  &handle,
			SAPDB_UInt8       obj,
			const char      **data,
			int               flags)
    {
        *data = "";
        return S_OK;
    }


   virtual HRESULT STDMETHODCALLTYPE SimCtlGetObjectName(
			class OmsHandle  &handle,
			SAPDB_UInt8       obj,
			const char      **name)
	{
		*name = NULL;
		return S_OK;
	}


    virtual HRESULT STDMETHODCALLTYPE IsInRegion(
       int    regionId,
       bool  &inRegion,
       short &error)
    {
        inRegion = false;
        error    = 0;
        return S_OK;
    }

    virtual HRESULT STDMETHODCALLTYPE AttachSequence(const tsp00_KnlIdentifier& sequenceName,
                                                     tgg00_Surrogate&           sequenceId,
                                                     tgg00_SeqInfo&             sequenceInfo,
                                                     short&                     error)
    { return S_OK; }
 
    virtual HRESULT STDMETHODCALLTYPE NextVal(const tgg00_Surrogate& sequenceId,
                                              tgg00_SeqInfo&         sequenceInfo,
                                              tsp00_Uint4&           nextVal,
                                              short&                 sqlError)
    {
        nextVal = 0;
        return S_OK;
    }

    virtual HRESULT STDMETHODCALLTYPE IsUnicodeInstance(
         bool& isUnicodeInstance, short& sqlError)
    {
        isUnicodeInstance = false;
        sqlError          = 0;
        return S_OK; 
    }

    virtual HRESULT STDMETHODCALLTYPE IsOneDBSystem(
         bool& isOneDBSystem, short& sqlError)
    {
        isOneDBSystem = false;
        sqlError          = 0;
        return S_OK; 
    }

    virtual HRESULT STDMETHODCALLTYPE VersionDictInsert(
      const OmsVersionId  &versionId,
      const OMS_Context   *pVersionContext,
      tgg00_BasisError    &error)
    { return S_OK; }

    virtual HRESULT STDMETHODCALLTYPE VersionDictFind(
      const OmsVersionId   &versionId,
      OMS_Context         **ppVersionContext)
    { return S_OK; }

    virtual HRESULT STDMETHODCALLTYPE VersionDictDrop(
      const OmsVersionId  &versionId,
      tgg00_BasisError    &error)
    { return S_OK; }

    virtual HRESULT STDMETHODCALLTYPE VersionDictShutdown() 
    { return S_OK; }

    virtual HRESULT STDMETHODCALLTYPE VersionDictUnloadOldestVersion(
      bool &unloaded) 
    { return S_OK; }

    virtual HRESULT STDMETHODCALLTYPE VersionDictMarkNotUnloadable(
      OMS_Context *pVersionContext)
    { return S_OK; }

    virtual HRESULT STDMETHODCALLTYPE VersionDictMarkUnloadable(
      OMS_Context *pVersionContext)
    { return S_OK; }

    virtual HRESULT STDMETHODCALLTYPE VersionDictCreateIter(
      OMS_LockMode   lockMode, 
      void         **ppIter, 
      OMS_Context  **ppContext)
    { return S_OK; }

    virtual HRESULT STDMETHODCALLTYPE VersionDictGetNext(
      void         *pIter, 
      OMS_Context **pContext)
    { return S_OK; }

    virtual HRESULT STDMETHODCALLTYPE VersionDictDestructIter(
      void* pIter)
    { return S_OK; }

    virtual int STDMETHODCALLTYPE GetSinkType()
    {
      return 0;
    }
    virtual HRESULT STDMETHODCALLTYPE SetTransactionComment(
        tsp00_Int2         commentLength,
        const OmsTypeWyde *pComment,
        short             &Errorr) 
    { return S_OK; }

    virtual HRESULT STDMETHODCALLTYPE IsTestKnlBadAllocActive(bool &active)
    { return S_OK; }

    virtual HRESULT STDMETHODCALLTYPE ResetMonitor()
    { return S_OK; }
};

/*! endclass: OMS_DummyKernelCallback */


#endif
