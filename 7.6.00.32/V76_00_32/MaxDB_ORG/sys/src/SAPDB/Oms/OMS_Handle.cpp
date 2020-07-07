/*!
 * \file    OMS_Handle.cpp
 * \author  MarkusSi, PeterG, Roterring
 * \brief   OMS handle implementation
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

#include "Oms/OMS_Handle.hpp"
#include "Oms/OMS_Globals.hpp"
#include "Oms/OMS_Session.hpp"
#include "Oms/OMS_CallbackInterface.hpp"
#include "Oms/OMS_VersionDictionary.hpp"
#include "Oms/OMS_MonitorDirectory.hpp"
#include "Oms/OMS_OidArrayReader.hpp"
#include "Oms/OMS_OidArrayWriter.hpp"
#include "Oms/OMS_ObjectContainer.hpp"
#include "Oms/OMS_VarOid.hpp"
#include "Oms/OMS_COMMonitor.hpp"
#include "Oms/OMS_Defines.h"
#include "Oms/OMS_ArrayObject.hpp"
#include "liveCache/LVC_IliveCacheSink.hpp"
#include "SAPDBCommon/SAPDB_UTF8Basis.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "hsp77.h"
#include "hsp78_0.h"
#include "ggg200.h"
#include "geo573.h"
#include "Oms/OMS_VarObjByClsIter.hpp"
#include "RunTime/System/RTESys_MemoryBarrier.h"


static inline OmsObjectContainerPtr omsGetContainer(const OmsAbstractObject* pObj)
{
  if (NULL == pObj) 
    OMS_Globals::Throw(e_nil_pointer, "omsGetContainer", __MY_FILE__, __LINE__);
  return (OmsObjectContainerPtr) ((unsigned char*) pObj - OmsObjectContainer::headerSize());
}

/*----------------------------------------------------------------------*/

inline void OmsHandle::omsIfNilOidThrowError(OmsHandle* h, const OmsObjectId& oid, const char* method, const char* pFile, unsigned int line) 
{
  if (!oid) {
    h->m_pSession->ThrowDBError (e_nil_pointer, method, pFile, line); 
  }
}


/*----------------------------------------------------------------------*/ 
/* Implementation of class OmsHandle                                    */
/*----------------------------------------------------------------------*/ 

OmsHandle::OmsHandle (OmsTypeInt4 sz)
  : m_session(NULL)   
  , m_pSession(NULL)
  , m_monitor(NULL)
{
}

/*----------------------------------------------------------------------*/

HRESULT OmsHandle::omsAttachDefaultContext()
{
  pasbool*     pToCancel;
  tsp00_TaskId taskId;
  bool         optimizedStreamCommunication; // PTS 1130281
  HRESULT hr = (*m_session)->GetDefaultContext(REINTERPRET_CAST(void**, &m_pSession),  
    &pToCancel, optimizedStreamCommunication, taskId);
  if ((!FAILED(hr)) && (NULL == m_pSession)) {
    m_pSession = new OMS_Session(*m_session, OMS_Globals::KernelInterfaceInstance->GetOmsRegionCnt(), pToCancel, optimizedStreamCommunication, taskId); /* PTS 1107849 */
    if (NULL == m_pSession) {
      return E_OUTOFMEMORY;
    }
    try {
       m_pSession->CreateDefaultContext();
    }
    catch (DbpError &e) {
      DbpErrorCallbackInterface *pCBInterface = DbpError::dbpGetCallback();  // PTS 1127871
      if (pCBInterface){
        pCBInterface->dbpCaughtError(e);
      }

      OMS_TRACE(omsTrSession, (*m_session), "create session failed : out of memory");
      m_pSession->IncRefCnt();
      m_pSession->DeleteSession();
      return E_OUTOFMEMORY;
    }
    hr = (*m_session)->SetDefaultContext((void*) m_pSession);
    OMS_TRACE(omsTrSession, (*m_session), "New Session : " << m_pSession); 
  }
  // PTS 1116693
  // Insert actual OmsHandle into the HandleList of the SessionContext
  // Needed for omsGetOmsHandle
  m_pSession->m_handleList.push_back (this);

  dbpAdviseLcSink(*m_session);
  m_pSession->IncRefCnt();
  OMS_TRACE(omsTrSession, (*m_session), "Session " << m_pSession << " attached to Hndl " << this << " RefCnt " << m_pSession->m_refCnt);
  return hr;
}

/*----------------------------------------------------------------------*/

OmsObjByClsIterBase OmsHandle::omsAllOids( ClassID guid, OmsSchemaHandle Schema, OmsContainerNo ContainerNo, int maxBufferSize) 
{
  _TRACE_METHOD_ENTRY(this,"omsAllOids");
  OMS_CHECK_EXCEPTION(*this);

  
  OmsObjByClsIterBase iterator(m_pSession,
                               m_pSession->m_context->GetContainerDir().GetContainerEntry(guid, Schema, ContainerNo),
                               maxBufferSize);
  return iterator;
}

/*----------------------------------------------------------------------*/

OmsVarObjByClsIter OmsHandle::omsAllVarOids(ClassID guid, OmsSchemaHandle Schema, OmsContainerNo ContainerNo, int maxBufferSize) 
{
  _TRACE_METHOD_ENTRY(this,"omsAllVarOids");
  OMS_CHECK_EXCEPTION(*this);

  //m_pSession->RegVarObjectContainer (Schema, ContainerNo);

  OmsVarObjByClsIter iterator(m_pSession,
                              m_pSession->m_context->GetContainerDir().GetContainerEntry(guid, Schema, ContainerNo),
                              maxBufferSize);
  return iterator;
}

/*----------------------------------------------------------------------*/

void OmsHandle::omsCancel() /* PTS 1113128 */
{
  if (!m_pSession->IsCancelled()){
    m_pSession->SetCancelFlag(true);
    m_pSession->SetCancelledByProgram(true);
  }
}

/*----------------------------------------------------------------------*/

bool OmsHandle::omsCancel(bool toCancel) /* PTS 1118194 */
{
  bool isCancelled = m_pSession->IsCancelled();
  m_pSession->SetCancelFlag(toCancel);

  // Set cancel-flag only if it is not set already
  if (!isCancelled && toCancel){
    m_pSession->SetCancelledByProgram(toCancel);
  }
   
  return isCancelled;
}

/*----------------------------------------------------------------------*/

OmsObjectId OmsHandle::omsCastOid(ClassID guid, const OmsObjectId& oid)
{
  OMS_CHECK_EXCEPTION(*this);

  ALIGN_OID_CONST(oid8, oid);

  const OMS_ObjectId8 resOid = m_pSession->CastOid(guid, oid8);

  return reinterpret_cast<OmsObjectId&>(const_cast<OMS_ObjectId8&>(resOid));
}

/*----------------------------------------------------------------------*/

bool OmsHandle::omsCheckOid ( const OmsObjectId& oid, ClassID guid)
{
  OMS_TRACE(omsTrInterface, m_pSession->m_lcSink, "omsCheckOid : " << guid << "," << oid);
  OMS_CHECK_EXCEPTION(*this);

  ALIGN_OID_CONST(oid8, oid);

  return m_pSession->CurrentContext()->CheckOid(oid8, guid);
}

/*----------------------------------------------------------------------*/

bool OmsHandle::omsCheckOid (const OmsVarOid& oid)
{
  OMS_TRACE(omsTrInterface, m_pSession->m_lcSink, "omsCheckOid : " << oid);
  OMS_CHECK_EXCEPTION(*this);

  ALIGN_OID_CONST(oid8, oid);

  return m_pSession->CurrentContext()->CheckOid(oid8);
}

/*----------------------------------------------------------------------*/

void OmsHandle::omsCloseVersion ( const OmsVersionId& versionId )
{
  OMS_TRACE(omsTrInterface|omsTrVersion, m_pSession->m_lcSink, "omsCloseVersion : " << OMS_CharBuffer(versionId, sizeof(OmsVersionId)));

  OMS_CHECK_EXCEPTION(*this);
  tsp00_Int2 DBError;
  if (!m_pSession->InVersion()) {
    return;
  }
  OMS_Context* pVersion = m_pSession->m_context;
  if (pVersion->IsOpenVersion(versionId)) {  
    if (1 < m_pSession->CurrentSubtransLevel()) {
      OMS_TRACE(omsTrVersion, m_pSession->m_lcSink, " - Close failed because subtrans is open");
      m_pSession->ThrowDBError (e_OMS_not_implemented, "Open Subtrans", versionId, __MY_FILE__, __LINE__);
    }
    OMS_TRACE(omsTrInterface|omsTrVersion, m_pSession->m_lcSink, "---omsCloseVersion : consistent View closed in the kernel");
    OMS_HResult hr = (*m_session)->CloseVersion (&DBError);  

    if ( DBError != 0 ) {
      OMS_TRACE(omsTrVersion, m_pSession->m_lcSink, " - Close failed with error: " << DBError);
      m_pSession->ThrowDBError (DBError, "omsCloseVersion", versionId, __MY_FILE__, __LINE__);
    }

    bool isDropped = false;
    { // Begin of exclusive critical section.    // PTS 1124533, 1133629
      OMS_InternalLockScope scope(m_pSession, OMS_Session::LIBOMS_RWLOCK_AREA, 
        OMS_Globals::m_globalsInstance->m_versionDictionary.GetSingleLockId(versionId), true/*exclusive*/);
      m_pSession->CloseVersion();

      if (pVersion->IsDropped()) {
        isDropped = true;
      }
    } // End of exclusive critical section.

    if (isDropped){
      // version has been marked as to be deleted, do it now
      OMS_TRACE(omsTrVersion, m_pSession->m_lcSink, " - Version is marked as dropped. Try to drop it now.");
      try {
        omsForceDropVersion(versionId);
      }
      catch (DbpError &e) {
        OMS_TRACE(omsTrVersion, m_pSession->m_lcSink, " - Drop failed with error: " << e.dbpDBError());
        DbpErrorCallbackInterface *pCBInterface = DbpError::dbpGetCallback();  // PTS 1127871
        if (pCBInterface){
          pCBInterface->dbpCaughtError(e);
        }
      }
    }
  }
  else {
    OMS_TRACE(omsTrVersion, m_pSession->m_lcSink, " - Version is not open.");
  }
  OMS_TRACE(omsTrVersion, m_pSession->m_lcSink, " - successfully finished.");
}

/*----------------------------------------------------------------------*/
/* PTS 1107849 */

void OmsHandle::omsReschedule() const
{
  OMS_CHECK_EXCEPTION(*this);
  m_pSession->RescheduleAndCheckCancelFlag();
}

/*----- PTS 1132163, PG ------------------------------------------------*/

void OmsHandle::omsSleep(const OmsTypeUInt1 sleepTime) const
{
  OMS_CHECK_EXCEPTION(*this);
  m_pSession->Sleep(sleepTime);
}

/*----------------------------------------------------------------------*/

short OmsHandle::omsCommitHandling ()
{
  OMS_TRACE (omsTrInterface, m_pSession->m_lcSink, "omsCommit");
  
  int phase = 0;
  try {
    // no application wanted exceptions in the course of transaction end
    this->omsControlCancelFlag(-1);
    this->omsControlDbpException(-1);

    phase = 1;
    OmsCallbackInterface* callbackInterface = m_pSession->GetCallbackInterface();
    if (NULL != callbackInterface) {
      callbackInterface->omsValidate (*this, OmsCallbackInterface::BeforeCommit, m_pSession->InVersion());
    }
    phase = 2;
    if (OMS_Globals::m_globalsInstance->InSimulator()) {
      // check for not stored, changed objects in simulator
      m_pSession->m_beforeImages.checkStores();
    }
    phase = 3;
    // Level 1 is used for 'normal' transaction rollback/commit of versions,
    // as this information is not kept in the kernel. As a commit might fail,
    // this lowest level must not be committed before the commit has been
    // processed in the kernel.
    m_pSession->ExecuteSubtransCommit(2);  
    phase = 4;
    m_pSession->m_defaultContext->FlushObjCache(/*emptyObjectCache=*/false);
    phase = 5;
    m_pSession->FreeBeforeImages(2);
    phase = 6;
    m_pSession->TransEnd();
  }
  catch (DbpError& e) {
    DbpErrorCallbackInterface *pCBInterface = DbpError::dbpGetCallback();  // PTS 1127871
    if (pCBInterface){
      pCBInterface->dbpCaughtError(e);
    }

    dbpOpError ("omsCommit (%d): DbpError %d (%s)", phase, e.dbpError(), e.m_errorText);
    omsRollbackHandling(false, false);
    return e_work_rolled_back;
  }
  catch (BAD_ALLOC_GEO573&) { // PTS 1127871
    DbpErrorCallbackInterface *pCBInterface = DbpError::dbpGetCallback();  
    if (pCBInterface){
      pCBInterface->dbpCaughtBadAlloc();
    }

    dbpOpError ("omsCommit (%d): Bad-Alloc", phase);
    omsRollbackHandling(false, false);
    return e_work_rolled_back;
  }
  return 0;
}

/*----------------------------------------------------------------------*/

void OmsHandle::omsControlCancelFlag(int countDown)     // PTS 1112358
{
#if defined(OMSTST)
  m_pSession->SetCancelCountDown(countDown);
#endif
}

/*----------------------------------------------------------------------*/

void OmsHandle::omsControlDbpException(int countDown)   // PTS 1112358
{
#if defined(OMSTST)
  m_pSession->SetExceptionCountDown(countDown);
#endif
}

/*----------------------------------------------------------------------*/

void OmsHandle::omsControlBadAllocExceptions(int countDown)  // PTS 1112358
{
#if defined(OMSTST)
  tsp00_Int2  DummyError;
  tsp00_Int4  BufLen = 0;
  tsp00_Int4    Buf = countDown;
  (*m_session)->MultiPurpose (m_diagnose, mm_down, &BufLen, 
    (unsigned char*) &Buf, &DummyError);
#endif
}

/*----------------------------------------------------------------------*/

OmsObjByKeyIterBase OmsHandle::omsCreateKeyRangeIter(
  ClassID guid,
  OmsSchemaHandle  Schema, 
  OmsContainerNo   ContainerNo,
  const void*      pStartKey,      
  const void*      pLowerKey,      // PTS 1119480
  const void*      pUpperKey,
  OmsIterDirection direction,      // PTS 1119480
  int              maxBufferSize)  // PTS 1119480
{
  OMS_CHECK_EXCEPTION(*this);
  OmsObjByKeyIterBase iterator(*this, guid, Schema, ContainerNo, 
                               pStartKey, pLowerKey, pUpperKey,
                               direction, maxBufferSize);
  return iterator;   
}

/*----------------------------------------------------------------------*/

void OmsHandle::IdentifierToWyde(const char* pSource, OmsTypeWyde* pDest) const
{
  pDest[OMS_MAX_SCHEMA_NAME_LENGTH] = 0;
  for (int ix = 0; ix < OMS_MAX_SCHEMA_NAME_LENGTH; ++ix)
  {
    *pDest = *pSource;
    if (0 == *pSource)
    {
      break;
    }
    ++pSource;
    ++pDest;
  }
}

/*----------------------------------------------------------------------*/
  
void OmsHandle::SessionEnd()
{
  m_pSession->ReleaseCallbackInterface();

  m_pSession->UserAllocatorCtl(OmsCallbackInterface::BlockUnregister, NULL);
  m_pSession->UserAllocatorCtl(OmsCallbackInterface::BlockSession, NULL);
}

/*----------------------------------------------------------------------*/

void OmsHandle::WydeToChar(const OmsTypeWyde* pSource, char* pDest, int destSize, const char* errorMsg) const
{
  int ix = -1;
  do
  {
    ++ix;
    if (pSource[ix] > 255)
    {
      m_pSession->ThrowDBError (e_OMS_not_translatable, errorMsg, __MY_FILE__, __LINE__);
    }
    if (ix == destSize)
    {
      m_pSession->ThrowDBError (e_buffer_too_small, errorMsg, __MY_FILE__, __LINE__);
    }
    pDest[ix] = (char) pSource[ix];
  }
  while (0 != pSource[ix]);
}

/*----- PTS 1135058, PG ------------------------------------------------*/

void OmsHandle::omsSetCurrentMethod(unsigned char * name) {
  m_pSession->setCurrentMethod(name);
}

/*----- PTS 1135058, PG ------------------------------------------------*/

void OmsHandle::omsResetCurrentMethod() {
  m_pSession->resetCurrentMethod();
}

/*----- PTS 1135058, PG ------------------------------------------------*/

const OmsTypeAscii7 * OmsHandle::omsGetCurrentMethod() const {
  return m_pSession->getCurrentMethod();
}

/*----------------------------------------------------------------------*/

void OmsHandle::omsCommitSubtrans( int required_lvl ) {
  OMS_TRACE (omsTrInterface, m_pSession->m_lcSink, "omsCommitSubtrans : reqlvl = " << required_lvl);
  OMS_CHECK_EXCEPTION(*this);
  m_pSession->CommitSubtrans(required_lvl);
}

/*----------------------------------------------------------------------*/

void OmsHandle::omsCleanUpAfterCrash() 
{
  OMS_TRACE (omsTrVersion, m_pSession->m_lcSink, "omsCleanUpAfterCrash");

  if (!m_pSession->m_versionsBoundToTrans.empty()) {
    // Drop all versions which are still bound to the session
    cgg251dclIterator<OMS_Context*,OMS_Session> iter = m_pSession->m_versionsBoundToTrans.begin();
    while (iter) {
      OMS_Context* pContext = *iter();
      ++iter;

      omsForceDropVersion(pContext->GetVersionId());
    }
  }

  omsRollbackHandling(true, true);

  OMS_TRACE (omsTrVersion, m_pSession->m_lcSink, " - omsCleanUpAfterCrash finished.");
}

/*----------------------------------------------------------------------*/

OmsSchemaHandle OmsHandle::omsCreateSchema(const char* SchemaName)
{
  OMS_TRACE (omsTrInterface, m_pSession->m_lcSink, "omsCreateSchema : " << SchemaName);
  OmsTypeWyde SchemaNameWyde[OMS_MAX_SCHEMA_NAME_LENGTH + 1];
  this->IdentifierToWyde(SchemaName, &SchemaNameWyde[0]);
  return omsCreateSchema(&SchemaNameWyde[0]);
}

/*----------------------------------------------------------------------*/

OmsSchemaHandle OmsHandle::omsCreateSchema(const OmsTypeWyde* SchemaName) 
{
  OMS_CHECK_EXCEPTION(*this);
  int length = 0;
  while (SchemaName[length] != 0)
  {
    ++length;
  }
  if (length > OMS_MAX_SCHEMA_NAME_LENGTH)
  {
    CONST_CAST(OmsHandle*, this)->dbpOpError("omsCreateSchema : too long identifier : %d, ", length);
    m_pSession->ThrowDBError (e_OMS_identifier_too_long, "omsCreateSchema", __MY_FILE__, __LINE__);
  }
  tsp00_Int4          SchemaId = 0;
  tgg00_BasisError    DBError;

  OMS_HResult hr = (*m_session)->CreateSchema (SchemaName, &SchemaId, &DBError);
  if ( DBError != 0 ) {
    m_pSession->ThrowDBError (DBError, "omsCreateSchema", __MY_FILE__, __LINE__);
  }
  return SchemaId;
}

/*----------------------------------------------------------------------*/

void OmsHandle::omsCreateVersion(const OmsVersionId& versionId)
{
  OMS_TRACE (omsTrInterface|omsTrVersion, m_pSession->m_lcSink, "omsCreateVersion (I): " << OMS_CharBuffer(versionId, sizeof(OmsVersionId)));
  
  omsCreateVersion(versionId, reinterpret_cast<OmsTypeWyde*>(NULL));
}

/*----------------------------------------------------------------------*/

void OmsHandle::omsCreateVersion(const OmsVersionId& versionId, const char* desc)
{
  OMS_TRACE (omsTrInterface|omsTrVersion, m_pSession->m_lcSink, "omsCreateVersion (II): " << OMS_CharBuffer(versionId, sizeof(OmsVersionId)));
  
  if (desc == NULL){
    omsCreateVersion(versionId, reinterpret_cast<OmsTypeWyde*>(NULL));
  }
  else{
    OmsTypeWyde desc_wyde[OMS_MAX_VERSION_DESC_LENGTH + 1];
    tsp00_Uint4           destBytesWritten;
    tsp00_Uint4           srcBytesParsed;
    tsp78ConversionResult error;
    int len_wyde = (OMS_MAX_VERSION_DESC_LENGTH + 1) * sizeof(OmsTypeWyde);
    
    // truncate input-string if necessary
    int len = (int)strlen(desc);
    if (len > OMS_MAX_VERSION_DESC_LENGTH){
      len = OMS_MAX_VERSION_DESC_LENGTH;
    }

    // Convertion from ASCII to UCS2
    error = sp78convertString(sp77encodingUCS2Native,        // destEndcoding
                              desc_wyde,                     // destBuffer
                              len_wyde,                      // destBufferLengthInBytes
                              &destBytesWritten,             // destBytesWritten
                              true,                          // addZeroTerminator
                              sp77encodingAscii,             // srcEncoding
                              desc,                          // srcBuffer
                              len,                           // srcBufferLengthInBytes
                              &srcBytesParsed);              // srcBytesParsed
        
    if (error != sp78_Ok){
      OMS_TRACE (omsTrVersion, m_pSession->m_lcSink, " - Error when converting description: " << error);
      // Reset array again
      memset(desc_wyde, 0, len_wyde);
    }

    omsCreateVersion(versionId, desc_wyde);
  }
}

/*----------------------------------------------------------------------*/

void OmsHandle::omsCreateVersion( const OmsVersionId& versionId, const OmsTypeWyde* desc )
{
  OMS_TRACE (omsTrInterface|omsTrVersion, m_pSession->m_lcSink, "omsCreateVersion (III): " << OMS_CharBuffer(versionId, sizeof(OmsVersionId)));
  OMS_CHECK_EXCEPTION(*this);

  tsp00_Int2   DBError;
  OMS_Context* initContext = m_pSession->CurrentContext();
  if (m_pSession->IsSubtransOpen()) {
    OMS_TRACE (omsTrVersion, m_pSession->m_lcSink, " - Create failed because subtrans is open.");
    m_pSession->ThrowDBError (e_OMS_too_many_subtrans, "Open Subtrans", versionId, __MY_FILE__, __LINE__);
  }
  if (m_pSession->InVersion()) {
    OMS_TRACE (omsTrVersion, m_pSession->m_lcSink, " - Create failed because a version (" <<
               OMS_CharBuffer(m_pSession->m_context->GetVersionId(),sizeof(OmsVersionId))  << ") is still open");
    m_pSession->ThrowDBError (e_already_in_version, "omsCreateVersion", versionId, __MY_FILE__, __LINE__); 
  }
  m_pSession->CreateVersion(versionId, desc);

  { // Begin of exclusive critical section.    // PTS 1124533, 1133629
    OMS_InternalLockScope scope(m_pSession, OMS_Session::LIBOMS_RWLOCK_AREA, 
      OMS_Globals::m_globalsInstance->m_versionDictionary.GetSingleLockId(versionId), true/*exclusive*/);
    try 
    {
        DBError = OMS_Globals::m_globalsInstance->m_versionDictionary.InsertVersion(versionId, m_pSession->CurrentContext());
    }
    catch (BAD_ALLOC_GEO573&)
    {     
      // Exception is thrown below, therefore no call of callback dbpCaughtError necessary

      DBError = e_no_more_memory;  // Should never happen !
    }
  }  // End of exclusive critical section.

  if (DBError != 0) {
    OMS_TRACE (omsTrVersion, m_pSession->m_lcSink, " - Create failed with error: " << DBError);
    m_pSession->RemoveFromTransVersion(m_pSession->m_context);
    m_pSession->m_context->SetSession(m_pSession);  // PTS 1134529    
    m_pSession->m_context->DeleteSelf();
    m_pSession->SetContext(initContext);
    m_pSession->ThrowDBError (DBError, "omsCreateVersion", versionId, __MY_FILE__, __LINE__);
  }
  OMS_TRACE (omsTrVersion, m_pSession->m_lcSink, " - successfully finished");
}

/*----------------------------------------------------------------------*/

int OmsHandle::omsCurrentSubtransLevel() const
{
  OMS_TRACE (omsTrInterface, m_pSession->m_lcSink, "omsCurrentSubtransLevel : " << m_pSession->CurrentSubtransLevel() - 1);
  OMS_CHECK_EXCEPTION(*this);
  return m_pSession->CurrentSubtransLevel() - 1;
}

/*----------------------------------------------------------------------*/

const OmsVersionId*  OmsHandle::omsCurrentVersionId () const
{
  OMS_TRACE (omsTrInterface|omsTrVersion, m_pSession->m_lcSink, "omsCurrentVersionId");
  OMS_CHECK_EXCEPTION(*this);

  if (m_pSession->InVersion()) {
    OMS_TRACE (omsTrVersion, m_pSession->m_lcSink, " - Current version " << 
               OMS_CharBuffer(m_pSession->CurrentContext()->GetVersionId(), sizeof(OmsVersionId)));
    return REINTERPRET_CAST(const OmsVersionId*, &m_pSession->CurrentContext()->GetVersionId()[0]);
  }
  else {
    OMS_TRACE (omsTrVersion, m_pSession->m_lcSink, " - Not in version.");
    return NULL;
  }
}

/*----------------------------------------------------------------------*/
// PTS 1122839
long OmsHandle::omsGetStoredErrorCode() const
{
  return m_pSession->GetStoredErrorCode();
}

/*----------------------------------------------------------------------*/
// PTS 1117690
bool OmsHandle::omsGetVersionInfo(const OmsVersionId& versionId, 
                                  OmsVersionInfo& info,
                                  char* pDescBuf, 
                                  int bufLen) const
{ 
  OMS_TRACE (omsTrInterface|omsTrVersion, m_pSession->m_lcSink, "omsGetVersionInfo (I): " << OMS_CharBuffer(versionId, sizeof(OmsVersionId)));

  if (pDescBuf == NULL || bufLen <= 0){
    return omsGetVersionInfo(versionId, info, reinterpret_cast<OmsTypeWyde*>(NULL), 0);
  }
  else{
    OmsTypeWyde descBuf_wyde[OMS_MAX_VERSION_DESC_LENGTH + 1];  
    
    // PTS 1118474
    if (bufLen > OMS_MAX_VERSION_DESC_LENGTH){
      bufLen = OMS_MAX_VERSION_DESC_LENGTH;
    }

    // PTS 1118592
    // Initialize output-buffer if requested
    memset(pDescBuf, 0, bufLen);

    bool found = omsGetVersionInfo(versionId, info, descBuf_wyde, bufLen);

    if (found && descBuf_wyde[0] != 0){
      tsp00_Uint4 destBytesWritten;
      tsp00_Uint4 srcBytesParsed;
      tsp78ConversionResult error;
      int bufLen_wyde = bufLen * sizeof(OmsTypeWyde);

      // Convertion from UCS2 to ASCII 
      error = sp78convertString(sp77encodingAscii,      // destEndcoding
                                pDescBuf,               // destBuffer
                                bufLen,                 // destBufferLengthInBytes
                                &destBytesWritten,      // destBytesWritten
                                true,                   // addZeroTerminator
                                sp77encodingUCS2Native, // srcEncoding
                                descBuf_wyde,           // srcBuffer
                                bufLen_wyde,            // srcBufferLengthInBytes
                                &srcBytesParsed);       // srcBytesParsed
        
      if (error != sp78_Ok){
        OMS_TRACE (omsTrVersion, m_pSession->m_lcSink, " - Convertion of description failed with error: " << error);
        // Reset array again
        memset(pDescBuf, 0, bufLen);
      }
    }

    return found;
  }
}

/*----------------------------------------------------------------------*/
// PTS 1117690
void OmsHandle::CopyVersionInfo (OMS_Context    *pContext, 
                                 OmsVersionInfo &info, 
                                 OmsTypeWyde    *pDescBuf, 
                                 int             bufLen) const
{
  *((tsp00_Date*)&info.createDate)  = pContext->GetCreateDate();
  *((tsp00_Time*)&info.createTime)  = pContext->GetCreateTime();
  *((tsp00_Date*)&info.openDate)    = pContext->GetOpenDate();
  *((tsp00_Time*)&info.openTime)    = pContext->GetOpenTime();
  info.consistentView = pContext->m_consistentView.gg90GetInt4();
  info.isMarked       = pContext->IsMarked();
  info.isOpen         = pContext->IsVersionOpen();
  info.isUnloaded     = pContext->IsUnloaded();
  info.heapUsage      = (OmsTypeInt8)pContext->HeapUsed();
  info.hashSize       = pContext->GetHashSize();

  if (pDescBuf != NULL && bufLen > 0 && pContext->GetVersionDesc() != NULL){
    // truncate desciption string if necessary
    int len = OMS_Globals::WideStrlen(pContext->GetVersionDesc());
    if (bufLen < len + 1){
      len = bufLen - 1;
    }

    // copy desciption
    SAPDB_MemCopyNoCheck(pDescBuf, pContext->GetVersionDesc(), len * sizeof(OmsTypeWyde));
    pDescBuf[len] = 0;  // ensure that string is null-terminated
  }
}

/*----------------------------------------------------------------------*/
// PTS 1117690
bool OmsHandle::omsGetVersionInfo (const OmsVersionId& versionId, 
                                   OmsVersionInfo& info, 
                                   OmsTypeWyde* pDescBuf, 
                                   int bufLen) const
{
  OMS_TRACE (omsTrInterface|omsTrVersion, m_pSession->m_lcSink, "omsGetVersionInfo (II): " << OMS_CharBuffer(versionId, sizeof(OmsVersionId)));
  OMS_CHECK_EXCEPTION(*this);

  if (pDescBuf != NULL && bufLen > 0){
     // Initialize output-buffer if requested
    int bufLen_byte = bufLen * sizeof(OmsTypeWyde);
    memset(pDescBuf, 0, bufLen_byte);
  }

  OMS_Context* pContext = NULL;
  if (m_pSession->InVersion()){
    if (!memcmp(&versionId[0], &(*omsCurrentVersionId())[0], sizeof(OmsVersionId))){
      // ***  Check if information can be read from the version which is currently open  ***
      pContext = m_pSession->CurrentContext();
    }
  }

  if (pContext != NULL){
    CopyVersionInfo(pContext, info, pDescBuf, bufLen);
    return true;
  }
  else {
    // Begin of exclusive critical section.    // PTS 1124533, 1133629
    OMS_InternalLockScope scope(m_pSession, OMS_Session::LIBOMS_RWLOCK_AREA, 
      OMS_Globals::m_globalsInstance->m_versionDictionary.GetSingleLockId(versionId), true/*exclusive*/);

    pContext = OMS_Globals::m_globalsInstance->m_versionDictionary.FindVersion(versionId);
    if (pContext == NULL){
      // there is no version which matches specified id
      return false;
    }
    else {
      CopyVersionInfo(pContext, info, pDescBuf, bufLen);
      return true;
    }
  }  // End of exclusive critical section.
}

/*----------------------------------------------------------------------*/

void OmsHandle::omsDelete( const OmsObjectId& oid ) 
{
  const char* msg = "omsDelete";

  OMS_TRACE (omsTrInterface, m_pSession->m_lcSink, msg << " : " << oid);
  OMS_CHECK_EXCEPTION(*this);
  m_pSession->AssertNotReadOnly(msg);
  m_pSession->IncDelete ();

  ALIGN_OID_CONST(oid8, oid);

  OmsObjectContainerPtr found = m_pSession->CurrentContext()->GetObj(oid8, /*doLock=*/false, /*shared=*/false);
  if (!found) 
  {
    m_pSession->ThrowDBError (e_object_not_found, msg, oid8, __MY_FILE__, __LINE__);
  }
  if (!m_pSession->IsLocked(found, /*shared=*/false))
  {
    m_pSession->ThrowDBError (e_object_not_locked, msg, oid8, __MY_FILE__, __LINE__);
  }
  OmsAbstractObject* pObj = &found->m_pobj; /* PTS 1107093 */
  
  m_pSession->InsertBeforeImage (found);
  
  // As the content of the object might get changed in omsCleanup, this method must not
  // be called before copying the before-image in InsertBeforeImage.
  pObj->omsCleanUp(*this);

  found->MarkDeleted();

  // PTS 1127661
  // Insert oid of deleted object into container-specific reuse structure. 
  if (!m_pSession->InVersion()){
    OMS_ContainerEntry *pInfo = found->GetContainerInfoNoCheck();
    if (!pInfo->GetClassEntry().IsKeyedObject()){
      pInfo->InsertReusableOid(oid8); 
    }
  }
};

/*----------------------------------------------------------------------*/

void OmsHandle::omsDelete( OmsAbstractObject* pobj ) 
{
  omsDelete (omsGetContainer(pobj)->m_oid);
}

/*----------------------------------------------------------------------*/

void OmsHandle::omsDelete( const OmsVarOid& oid )
{
  OMS_TRACE (omsTrInterface | omsTrVarObject, m_pSession->m_lcSink, "omsDelete(Var) : " << oid);
  OMS_CHECK_EXCEPTION(*this);

  ALIGN_OID_CONST(oid8, oid);

  if (!m_pSession->IsLocked(oid8, /*shared=*/false))
  {
    m_pSession->ThrowDBError (e_object_not_locked, "omsDelete(varobject)", oid8, __MY_FILE__, __LINE__);
  }
  m_pSession->DeleteVarObject(oid8);
}

/*----------------------------------------------------------------------*/

void OmsHandle::omsDeleteAll (ClassID guid, OmsSchemaHandle Schema, OmsContainerNo ContainerNo) 
{
  OMS_TRACE (omsTrInterface, m_pSession->m_lcSink, "omsDeleteAll: " << guid << ", " << Schema << ", " << ContainerNo);
  OMS_CHECK_EXCEPTION(*this);

  m_pSession->m_context->GetContainerDir().DeleteAllObjects (guid, Schema, ContainerNo);
}

/*----------------------------------------------------------------------*/

const void* OmsHandle::omsDeRef(const OmsVarOid& oid)
{
  omsIfNilOidThrowError (this, oid, "omsDeRef : VarObject", __MY_FILE__, __LINE__);
  OMS_TRACE (omsTrInterface | omsTrVarObject, m_pSession->m_lcSink, "omsDeRef(Var) : " << oid);
  OMS_CHECK_EXCEPTION(*this);

  ALIGN_OID_CONST (oid8, oid);

  return m_pSession->DeRefVarObject(oid8);
}

/*----------------------------------------------------------------------*/

OmsAbstractObject* OmsHandle::omsDeRef (const OmsObjectId& oid, ClassID guid, bool forUpd, bool doLock, bool shared )
{
  OMS_CHECK_EXCEPTION(*this);
  if (!oid) {
    return NULL;
  }
  else {
    ALIGN_OID_CONST (oid8, oid);

    return m_pSession->DeRef (oid8, guid, forUpd, doLock, shared);
  }
};

/*----------------------------------------------------------------------*/

OmsAbstractObject* OmsHandle::omsDeRefKey (unsigned char*  key, 
                                           ClassID guid, 
                                           bool            forUpd, 
                                           bool            doLock,
                                           OmsSchemaHandle Schema,
                                           OmsContainerNo  ContainerNo,
                                           bool            shared) 
{
  OMS_CHECK_EXCEPTION(*this);
  return m_pSession->DeRefViaKey (key, guid, forUpd, doLock, shared, Schema, ContainerNo);  
}

/*----------------------------------------------------------------------*/

void OmsHandle::omsDropSchema(OmsSchemaHandle Schema)
{
  OMS_TRACE (omsTrInterface, m_pSession->m_lcSink, "omsDropSchema : " << Schema);
  OMS_CHECK_EXCEPTION(*this);

  if (m_pSession->InVersion()) {
    m_pSession->ThrowDBError (e_OMS_missing_privilege, "omsDropSchema : not allowed in version", __MY_FILE__, __LINE__);
  }
  if (OMS_DEFAULT_SCHEMA_HANDLE == Schema) {
    m_pSession->ThrowDBError (e_OMS_missing_privilege, "omsDropSchema : OMS Schema not allowed", __MY_FILE__, __LINE__);
  }

  m_pSession->m_context->GetContainerDir().DropSchema(Schema);
}

/*----------------------------------------------------------------------*/

int OmsHandle::omsCreateContainer( ClassID          guid, 
                                    OmsSchemaHandle  schema, 
                                    OmsContainerNo   containerNo,
                                    bool             useCachedKeys,
                                    bool             partitionedKeys,
                                    bool             noWait)
{
  OMS_TRACE (omsTrInterface, m_pSession->m_lcSink, "omsCreateContainer : " 
    << "Guid=" << guid << ", Schema=" << schema << ", CNo=" << containerNo << ", noWait=" << noWait);
  OMS_CHECK_EXCEPTION(*this);

  return m_pSession->m_context->GetContainerDir().CreateContainer(guid, schema, containerNo, useCachedKeys, partitionedKeys, noWait);
}

/*----------------------------------------------------------------------*/

void OmsHandle::omsDropContainer(ClassID guid, OmsSchemaHandle schema, OmsContainerNo containerNo)
{
  OMS_TRACE (omsTrInterface, m_pSession->m_lcSink, "omsDropContainer: " << guid << ", " << schema << ", " << containerNo );
  OMS_CHECK_EXCEPTION(*this);

  if (m_pSession->InVersion()) {
    m_pSession->ThrowDBError (e_OMS_missing_privilege, "omsDropContainer : not allowed in version", __MY_FILE__, __LINE__);
  }

  m_pSession->m_context->GetContainerDir().DropContainer(guid, schema, containerNo);
}

/*----------------------------------------------------------------------*/

void OmsHandle::omsDropVersion( const OmsVersionId& versionId )
{
  OMS_TRACE (omsTrInterface|omsTrVersion, m_pSession->m_lcSink, "omsDropVersion : " << OMS_CharBuffer(versionId, sizeof(OmsVersionId)));
  OMS_CHECK_EXCEPTION(*this);

  OMS_Context*   context = NULL;
  { // Begin of exclusive critical section.    // PTS 1124533, 1133629
    OMS_InternalLockScope scope(m_pSession, OMS_Session::LIBOMS_RWLOCK_AREA, 
      OMS_Globals::m_globalsInstance->m_versionDictionary.GetSingleLockId(versionId), true/*exclusive*/);

    context = OMS_Globals::m_globalsInstance->m_versionDictionary.FindVersion(versionId);
    if (NULL == context) {
      OMS_TRACE (omsTrVersion, m_pSession->m_lcSink, " - Drop failed because version was not found.");
      m_pSession->ThrowDBError (e_unknown_version, "omsDropVersion", versionId, __MY_FILE__, __LINE__);
    }

    OMS_Globals::m_globalsInstance->m_versionDictionary.MarkNotUnloadable(this->m_pSession->m_lcSink, context);
    m_pSession->DropVersionProlog(context);
    OMS_Globals::m_globalsInstance->m_versionDictionary.DropVersion(versionId);
  }  // End of exclusive critical section.

  m_pSession->DropVersionEpilog(context);
  OMS_TRACE (omsTrVersion, m_pSession->m_lcSink, " - successfully finished.");
}

/*----------------------------------------------------------------------*/

const char* OmsHandle::omsSimDumpGetHtmlRef(const OmsObjectId &oid)
{
  const char *data;

  ALIGN_OID_CONST (oid8, oid);

  m_pSession->m_lcSink->SimCtlGetHTMLRef(*this, (SAPDB_UInt8&)oid8, &data, 3);
  return data;
}

/*----------------------------------------------------------------------*/

bool OmsHandle::omsExistsContainer(ClassID guid, OmsSchemaHandle schema, OmsContainerNo cno) // PTS 1110334
{
  OMS_TRACE (omsTrInterface, m_pSession->m_lcSink, "omsExistsContainer : " << guid << "," << schema << "," << cno);
  OMS_CHECK_EXCEPTION(*this);

  return m_pSession->m_context->GetContainerDir().ExistsContainer(guid, schema, cno);
}

/*----------------------------------------------------------------------*/

void OmsHandle::omsForceDropVersion( const OmsVersionId& versionId )
{
  OMS_TRACE (omsTrInterface|omsTrVersion, m_pSession->m_lcSink, "omsForceDropVersion : " << OMS_CharBuffer(versionId, sizeof(OmsVersionId)));
  OMS_CHECK_EXCEPTION(*this);

  OMS_Context*   context = NULL;
  { // Begin of exclusive critical section.    // PTS 1124533, 1133629
    OMS_InternalLockScope scope(m_pSession, OMS_Session::LIBOMS_RWLOCK_AREA, 
      OMS_Globals::m_globalsInstance->m_versionDictionary.GetSingleLockId(versionId), true/*exclusive*/);

    context = OMS_Globals::m_globalsInstance->m_versionDictionary.FindVersion(versionId);
    if (NULL == context) {
      OMS_TRACE (omsTrVersion, m_pSession->m_lcSink, " - Drop failed because version was not found.");
      m_pSession->ThrowDBError (e_unknown_version, "omsDropVersion", versionId, __MY_FILE__, __LINE__);
    }

    OMS_Globals::m_globalsInstance->m_versionDictionary.MarkNotUnloadable(this->m_pSession->m_lcSink, context);
    try {
      m_pSession->DropVersionProlog(context);
      OMS_Globals::m_globalsInstance->m_versionDictionary.DropVersion(versionId);
    }
    catch (DbpError& e) {
      if (e_OMS_missing_privilege == e.dbpError()) {
        tsp00_TaskId tid = context->GetSession()->GetTaskId();
        char tid_c[10];
        sp77sprintf(tid_c, sizeof(tid_c), "T%d", tid); 
        OMS_TRACE (omsTrVersion, m_pSession->m_lcSink, " - Only mark as dropped, as version is still bound to another transaction: " << tid_c);

        // context is still bound to another session, therefore only mark session as dropped
        DbpErrorCallbackInterface *pCBInterface = DbpError::dbpGetCallback();  // PTS 1127871
        if (pCBInterface){
          pCBInterface->dbpCaughtError(e);
        }

        context->MarkDropped();
        OMS_TRACE (omsTrVersion, m_pSession->m_lcSink, " - successfully finished.");
        return;
      }
      else {
        throw;
      }
    }
  } // End of exclusive critical section.

  m_pSession->DropVersionEpilog(context);
  OMS_TRACE (omsTrVersion, m_pSession->m_lcSink, " - successfully finished.");
}
/*----------------------------------------------------------------------*/

void OmsHandle::omsDump ()
{
  m_pSession->CurrentContext()->DumpObjCache(*this);
};

/*----------------------------------------------------------------------*/

void OmsHandle::omsDumpStackTrace()
{
  OMS_Globals::KernelInterfaceInstance->DumpStackTrace();
}

/*----------------------------------------------------------------------*/

unsigned int OmsHandle::omsSaveStackTrace(void **buffer, unsigned int levels)
{
  return OMS_Globals::KernelInterfaceInstance->SaveStackTrace(buffer, levels);
}

/*----------------------------------------------------------------------*/

void OmsHandle::omsEvalStackTrace(void **buffer, unsigned int levels,
  OmsStackTraceCallbackInterface *cb)
{
  OMS_Globals::KernelInterfaceInstance->EvalStackTrace(buffer, levels, cb);
}

/*----------------------------------------------------------------------*/

void OmsHandle::omsSimDump()
{
  tsp00_Int4 BufLen = 1;
  char Buf[1];
  short DummyError = 0;
  OMS_HResult hr = (*m_session)->MultiPurpose (m_describe, 2, &BufLen, 
    (unsigned char*) &Buf, &DummyError);
}

/*----------------------------------------------------------------------*/

void OmsHandle::omsSimDump(bool version)
{
  tsp00_Int4 BufLen = 1;
  char Buf[1];
  short DummyError = 0;
  OMS_HResult hr = (*m_session)->MultiPurpose (m_describe, version ? 1 : 0, &BufLen, 
    (unsigned char*) &Buf, &DummyError);
}

/*----------------------------------------------------------------------*/

void OmsHandle::omsCommit()
{
  tsp00_Int4 BufLen = 1;
  char Buf[4];
  short DummyError = 0;
  OMS_HResult hr = (*m_session)->MultiPurpose (m_commit, mm_nil, &BufLen, 
    (unsigned char*) &Buf, &DummyError);
}

/*----------------------------------------------------------------------*/

void OmsHandle::omsRollback()
{
  tsp00_Int4 BufLen = 1;
  char Buf[4];
  short DummyError = 0;
  OMS_HResult hr = (*m_session)->MultiPurpose (m_rollback, mm_nil, &BufLen, 
    (unsigned char*) &Buf, &DummyError);
}

/*----------------------------------------------------------------------*/

bool OmsHandle::omsExistsVersion( const OmsVersionId& versionId )
{
  OMS_TRACE (omsTrInterface|omsTrVersion, m_pSession->m_lcSink, "omsExistsVersion : " << OMS_CharBuffer(versionId, sizeof(OmsVersionId)));
  OMS_CHECK_EXCEPTION(*this);

  bool isDropped = false;
  OMS_Context* pContext = NULL;
  { // Begin of shared critical section.    // PTS 1124533, 1133629
    OMS_InternalLockScope scope(m_pSession, OMS_Session::LIBOMS_RWLOCK_AREA, 
      OMS_Globals::m_globalsInstance->m_versionDictionary.GetSingleLockId(versionId), false/*share*/);
  
    pContext = OMS_Globals::m_globalsInstance->m_versionDictionary.FindVersion(versionId);
    if ((NULL != pContext) && (pContext->IsDropped())) {  /* PTS 1107209 */ /* PTS 1107277 */
      isDropped = true;
    }
  } // End of shared critical section.

  if (isDropped){ 
    OMS_TRACE (omsTrVersion, m_pSession->m_lcSink, " - Version is marked as dropped. Try to drop it now");
    try {
      omsForceDropVersion(versionId);
      pContext = NULL;
    }
    catch(DbpError &e) {
      OMS_TRACE (omsTrVersion, m_pSession->m_lcSink, " - Drop failed with error: " << e.dbpDBError());
      DbpErrorCallbackInterface *pCBInterface = DbpError::dbpGetCallback();  // PTS 1127871
      if (pCBInterface){
        pCBInterface->dbpCaughtError(e);
      }
    }
  }

  OMS_TRACE (omsTrVersion, m_pSession->m_lcSink, " - Version was " << (pContext ? "" : "not") << "found");

  return (pContext != NULL);
}

/*----------------------------------------------------------------------*/

void OmsHandle::omsExceptionHandler(DbpError& e)
{
  m_pSession->CurrentMethodCallEpilog(-1); // PTS 1126797
  OmsTypeUTF8 utf8Buffer[sizeof(e.m_errorText)*2];
  OmsTypeUTF8* pMsgText;
  if (e.m_isWyde)
  {
    
    SAPDB_UTF8Basis::UCS2ConstPointer srcAt;
    SAPDB_UTF8Basis::UTF8Pointer      destAt;
    pMsgText = &utf8Buffer[0];
    register const OmsTypeWyde* pStart = e.dbpErrorTextWyde();
    register const OmsTypeWyde* pEnd   = pStart;
    while (0 != *pEnd)
    {
      ++pEnd;
    }
    if (!OMS_Globals::KernelInterfaceInstance->IsUnicodeInstance())
    {
      int ix = 0;
      do
      {
        if (*pStart > 255)
        {
          dbpOpError ("omsExceptionHandler : msg not translatable");
          utf8Buffer[ix] = 0;
          break;
        }
        utf8Buffer[ix] = (unsigned char) *pStart;
        if (pStart == pEnd)
        {
          break;
        }
        ++pStart;
        ++ix;
       }
      while (true);
    }
    else
    {
      SAPDB_UTF8Basis::ConversionResult res = SAPDB_UTF8Basis::ConvertFromUCS2 (
        (SAPDB_UTF8Basis::UCS2ConstPointer) pStart,
        (SAPDB_UTF8Basis::UCS2ConstPointer) ++pEnd,
        srcAt,
        &utf8Buffer[0],
        &utf8Buffer[sizeof(e.m_errorText)*2],
        destAt);
    }
  }
  else
  {
    pMsgText = (OmsTypeUTF8*) &e.m_errorText[0];
  }
  switch (e.m_errorKind) {
  case DbpError::DB_ERROR :
    {
      omsRaiseError ((short) e.m_errorNo, pMsgText);
      break;
    }
  case DbpError::HRESULT_ERROR :
    {
      (*m_session)->HResultError (e.m_errorNo);
      break;
    }
  case DbpError::USER_DEFINED :
    {
      omsRaiseError ((short) e.m_errorNo, pMsgText);
      break;
    } 
  default :
    dbpOpError ("Invalid DbpError, kind = %d, errno = %d", e.m_errorKind, e.m_errorNo);
    throw;
  }
} 

/*----------------------------------------------------------------------*/

OmsContainerNo OmsHandle::omsGetContainerNo(const OmsObjectId& oid)
{
  OMS_TRACE (omsTrInterface, m_pSession->m_lcSink, "omsGetContainerNo : " << oid);
  OMS_CHECK_EXCEPTION(*this);
  omsIfNilOidThrowError (this, oid, "omsGetContainerNo", __MY_FILE__, __LINE__);

  ALIGN_OID_CONST (oid8, oid);

  OmsObjectContainerPtr pObj = m_pSession->CurrentContext()->GetObj(oid8, /*doLock=*/false, /*shared=*/false);
  if (NULL != pObj) {
    OMS_ContainerEntry* pContainerInfo = pObj->GetContainerInfo(__MY_FILE__, __LINE__);
    return pContainerInfo->GetContainerNo();
  }
  else {
    m_pSession->ThrowDBError (e_object_not_found, "omsGetContainerNo", oid8, __MY_FILE__, __LINE__);
    return 0;
  }
}

/*----------------------------------------------------------------------*/

void OmsHandle::omsGetMonitorInfo (tgg01_COMMonitorTransferBuf* lpMonInfo)
{
  if (lpMonInfo->ctbDispId_gg00 < 0) {
    // reset statistics
    m_monitor->Reset();
    return;
  }
  while ((lpMonInfo->ctbDispId_gg00 < m_monitor->size()) &&  
    (0 == (*m_monitor)[lpMonInfo->ctbDispId_gg00].m_monInfo.cmiCallCnt_gg00)) {
    ++lpMonInfo->ctbDispId_gg00;
  }
  if (lpMonInfo->ctbDispId_gg00 >= m_monitor->size()) {
    lpMonInfo->ctbDispId_gg00 = -1;
    return;
  }
  SAPDB_MemCopyNoCheck (&lpMonInfo->ctbMonInfo_gg00, 
    &((*m_monitor)[lpMonInfo->ctbDispId_gg00].m_monInfo),
    sizeof (lpMonInfo->ctbMonInfo_gg00));
  if (csp_maxint4 == lpMonInfo->ctbMonInfo_gg00.cmiCounters_gg00[cmiMinWaitNewConsView]) {
    lpMonInfo->ctbMonInfo_gg00.cmiCounters_gg00[cmiMinWaitNewConsView] = 0;
  }
}

/*----------------------------------------------------------------------*/

bool OmsHandle::omsGetMethodCallMonitorInfo (OmsMonitorData& info) const
{
   return m_pSession->GetMethodCallMonitorInfo(info);
}

/*----------------------------------------------------------------------*/

void OmsHandle::omsGetObj(const OmsObjectId& oid, unsigned char* cnsView, int& ObjLen, unsigned char* obj)
// exported for diagnostics only, not included into interface
{
  tgg91_TransNo cView;
  tgg91_TransNo currCView = m_pSession->m_defaultContext->m_consistentView;
  SAPDB_MemCopyNoCheck (&cView, cnsView, sizeof(cView));
  m_pSession->m_defaultContext->SetConsistentView(cView);
  m_pSession->m_defaultContext->SetConsistentView(cView);

  ALIGN_OID_CONST (oid8, oid);

  OmsObjectContainerPtr pObj = m_pSession->m_defaultContext->GetObjFromLiveCacheBase(oid8, OMS_Context::NoLock);
  if (NULL != pObj) {
    OMS_ContainerEntry* pContainerInfo = pObj->GetContainerInfo(__MY_FILE__, __LINE__);
    ObjLen = (int)pContainerInfo->GetClassEntry().GetPersistentSize();
    SAPDB_MemCopyNoCheck (obj, &pObj->m_pobj, ObjLen); 
  }
  else {
    ObjLen = 0;
    obj    = NULL;
  }
  m_pSession->m_defaultContext->SetConsistentView(currCView);
}

/*----------------------------------------------------------------------*/

OmsHandle* OmsHandle::omsGetOmsHandle()
{  // PTS 1116693
   OMS_Session *pSession;
   pasbool         *pToCancel;
   tsp00_TaskId    taskId;
   bool            optimizedStreamCommunication; // PTS 1130281
  
   IliveCacheSink* pSink = OMS_Globals::GetCurrentLcSink();
   pSink->GetDefaultContext(REINTERPRET_CAST(void**, &pSession), &pToCancel, optimizedStreamCommunication, taskId);
   
   if( pSession == NULL || pSession->m_handleList.empty()) // PTS 1130704
    return NULL;
   else
    return pSession->m_handleList.front();
}
 
/*----------------------------------------------------------------------*/

OmsSchemaHandle OmsHandle::omsGetSchemaHandle(const char* SchemaName) const 
{
  OmsTypeWyde SchemaNameWyde[OMS_MAX_SCHEMA_NAME_LENGTH + 1];
  this->IdentifierToWyde(SchemaName, &SchemaNameWyde[0]);
  return omsGetSchemaHandle(&SchemaNameWyde[0]);
}

/*----------------------------------------------------------------------*/

OmsSchemaHandle OmsHandle::omsGetSchemaHandle(const OmsTypeWyde* SchemaName) const 
{
  OMS_TRACE (omsTrInterface, m_pSession->m_lcSink, "omsGetSchemaHandle");
  OMS_CHECK_EXCEPTION(*this);
  tsp00_Int4          SchemaId = 0;
  tgg00_BasisError    DBError;
  OMS_HResult hr = (*m_session)->ExistsSchema (SchemaName, &SchemaId, &DBError);
  if (DBError == e_unknown_name){   // PTS 1122930
    DBError = e_unknown_schema;     // PTS 1122930
  }                                 // PTS 1122930
  if ( 0 != DBError ) {
    m_pSession->ThrowDBError (DBError, "omsGetSchemaHandle", __MY_FILE__, __LINE__);
  }
  return SchemaId;
}

/*----------------------------------------------------------------------*/

OmsSchemaHandle OmsHandle::omsGetSchemaHandle(const OmsObjectId& oid)
{
  OMS_TRACE (omsTrInterface, m_pSession->m_lcSink, "omsGetSchemaHandle : " << oid);
  OMS_CHECK_EXCEPTION(*this);
  
  omsIfNilOidThrowError (this, oid, "omsGetSchemaHandle", __MY_FILE__, __LINE__);

  ALIGN_OID_CONST (oid8, oid);

  OmsObjectContainerPtr pObj = m_pSession->CurrentContext()->GetObj(oid8, /*doLock=*/false, /*shared=*/false);
  if (NULL != pObj) {
    OMS_ContainerEntry* pContainerInfo = pObj->GetContainerInfo(__MY_FILE__, __LINE__);
    return pContainerInfo->GetSchema();
  }
  else {
    m_pSession->ThrowDBError (e_object_not_found, "omsGetSchemaHandle", oid8, __MY_FILE__, __LINE__);
    return 0;
  }
}

/*----------------------------------------------------------------------*/

void OmsHandle::omsGetSchemaName(OmsSchemaHandle Schema, char* SchemaName, int SchemaNameBufSize) const 
{
  const char* msg = "omsGetSchemaName";

  OmsTypeWyde SchemaNameWyde[OMS_MAX_SCHEMA_NAME_LENGTH + 1];
  this->omsGetSchemaName(Schema, &SchemaNameWyde[0], OMS_MAX_SCHEMA_NAME_LENGTH + 1);
  this->WydeToChar(&SchemaNameWyde[0], SchemaName, SchemaNameBufSize, msg);
}

/*----------------------------------------------------------------------*/

void OmsHandle::omsGetSchemaName(OmsSchemaHandle Schema, OmsTypeWyde* SchemaName, int SchemaNameBufSize) const 
{
  OMS_CHECK_EXCEPTION(*this);
  OmsTypeWyde SchemaNameWyde[OMS_MAX_SCHEMA_NAME_LENGTH + 1];
  tgg00_BasisError    DBError;
  OMS_HResult hr = m_pSession->m_lcSink->GetSchemaName(Schema, &SchemaNameWyde[0], &DBError);
  if (0 != DBError)
  {
    DbpBase base(m_pSession->m_lcSink);
    base.dbpOpError("omsGetSchemaName : unknown schema handle %d", Schema);
    m_pSession->ThrowDBError (DBError, "omsGetSchemaName", __MY_FILE__, __LINE__);
  }
  if (SchemaNameBufSize > 0) 
  {
    int ix = 0;
    while (ix < SchemaNameBufSize)
    {
      SchemaName[ix] = SchemaNameWyde[ix];
      if (0 == SchemaNameWyde[ix])
      {
        return;
      }
      ++ix;
    }
    m_pSession->ThrowDBError (e_buffer_too_small, "omsGetSchemaName", __MY_FILE__, __LINE__);
  }
}

/*----------------------------------------------------------------------*/
// PTS 1128641 
int OmsHandle::omsGetTaskId()     
{
  return m_pSession->GetTaskId();
}

/*----------------------------------------------------------------------*/

int OmsHandle::omsGetTimeOut() const
{
  return m_pSession->m_timeout;
}

/*----------------------------------------------------------------------*/

OmsCallbackInterface* OmsHandle::omsGetTransactionInterface (ClassID guid) const
{
  return m_pSession->GetCallbackInterface(guid);
}

/*----------------------------------------------------------------------*/

unsigned long OmsHandle::omsGetVarObjectSize (const OmsVarOid& oid) 
{
  OMS_TRACE (omsTrInterface | omsTrVarObject, m_pSession->m_lcSink, "omsGetVarObjSize : " << oid);
  OMS_CHECK_EXCEPTION(*this);

  ALIGN_OID_CONST (oid8, oid);

  return (unsigned long) m_pSession->GetVarObjSize(oid8);
}

/*----------------------------------------------------------------------*/

void OmsHandle::omsInitSession (IUnknown** s)
{
  m_session = (IliveCacheSink**) s;

  // this will do an attach in simulator, ignored otherwise
  OMS_Globals::KernelInterfaceInstance->SimCtlAttachSink(*this);
}

/*----------------------------------------------------------------------*/

bool OmsHandle::omsIsLocked (const OmsObjectId& oid, bool shared)
{
  OMS_TRACE (omsTrInterface, m_pSession->m_lcSink, "omsIsLocked : " << oid);

  ALIGN_OID_CONST (oid8, oid);

  return m_pSession->IsLocked(oid8, shared);
}

/*----------------------------------------------------------------------*/

bool OmsHandle::omsIsRegistered(ClassID guid) const
{
  OMS_TRACE (omsTrInterface, m_pSession->m_lcSink, "omsIsRegistered : " << guid );
  return (NULL != m_pSession->GetClassDir().Find(guid));
}

/*----------------------------------------------------------------------*/

void OmsHandle::omsLoadVarObject (const OmsVarOid& oid, unsigned long size, void* buf)
{
  OMS_TRACE (omsTrInterface | omsTrVarObject, m_pSession->m_lcSink, "omsLoadVarObject : " << oid << ", size " << size);
  OMS_CHECK_EXCEPTION(*this);

  omsIfNilOidThrowError (this, oid, "omsLoadVarObject", __MY_FILE__, __LINE__);

  ALIGN_OID_CONST (oid8, oid);

  m_pSession->LoadVarObject(oid8, VarObjExclusiveLock, (size_t)size, buf); 
}

/*----------------------------------------------------------------------*/

void OmsHandle::omsLock(const OmsObjectId&oid, bool withoutDeref)
{
  OMS_TRACE (omsTrInterface, m_pSession->m_lcSink, "omsLock : " << oid);
  OMS_CHECK_EXCEPTION(*this);

  // TODO: consider withoutDeref!!

  ALIGN_OID_CONST (oid8, oid);

  m_pSession->CurrentContext()->LockObj(oid8);
}

/*----------------------------------------------------------------------*/

void OmsHandle::omsLockShared(const OmsObjectId &oid, bool withoutDeref)
{
  OMS_TRACE (omsTrInterface, m_pSession->m_lcSink, "omsLockShared : " << oid);
  OMS_CHECK_EXCEPTION(*this);

  // TODO: consider withoutDeref!!

  ALIGN_OID_CONST (oid8, oid);

  m_pSession->CurrentContext()->LockSharedObj(oid8);
}

/*----------------------------------------------------------------------*/

void OmsHandle::omsLock(const OmsVarOid& oid, bool withoutDeref)
{ 
  OMS_TRACE (omsTrInterface, m_pSession->m_lcSink, "omsLock(Var) : " << oid);
  OMS_CHECK_EXCEPTION(*this);

  // TODO: consider withoutDeref!!

  ALIGN_OID_CONST (oid8, oid);

  m_pSession->CurrentContext()->LockObj(oid8);
}

/*----------------------------------------------------------------------*/

void OmsHandle::omsLockShared(const OmsVarOid& oid, bool withoutDeref)
{ 
  OMS_TRACE (omsTrInterface, m_pSession->m_lcSink, "omsLockShared(Var) : " << oid);
  OMS_CHECK_EXCEPTION(*this);

  // TODO: consider withoutDeref!!

  ALIGN_OID_CONST(oid8, oid);

  m_pSession->CurrentContext()->LockSharedObj(oid8);
}

/*----------------------------------------------------------------------*/

bool OmsHandle::omsTryLock(const OmsObjectId&oid, bool withoutDeref)
{
  OMS_TRACE (omsTrInterface, m_pSession->m_lcSink, "omsTryLock : " << oid);
  OMS_CHECK_EXCEPTION(*this);

  // TODO: consider withoutDeref!!

  ALIGN_OID_CONST(oid8, oid);

  return m_pSession->CurrentContext()->TryLockObj(oid8, /*shared=*/false);
}

/*----------------------------------------------------------------------*/

bool OmsHandle::omsTryLock(const OmsVarOid& oid, bool withoutDeref)
{ 
  OMS_TRACE (omsTrInterface, m_pSession->m_lcSink, "omsTryLock(Var) : " << oid);
  OMS_CHECK_EXCEPTION(*this);

  // TODO: consider withoutDeref!!

  ALIGN_OID_CONST(oid8, oid);

  return m_pSession->CurrentContext()->TryLockObj(oid8, /*shared=*/false);
}

/*----------------------------------------------------------------------*/

bool OmsHandle::omsTryLockShared(const OmsObjectId &oid, bool withoutDeref)
{
  OMS_TRACE (omsTrInterface, m_pSession->m_lcSink, "omsTryLockShared : " << oid);
  OMS_CHECK_EXCEPTION(*this);

  // TODO: consider withoutDeref!!

  ALIGN_OID_CONST(oid8, oid);

  return m_pSession->CurrentContext()->TryLockObj(oid8, /*shared=*/true);
}

/*----------------------------------------------------------------------*/

bool OmsHandle::omsTryLockShared(const OmsVarOid &oid, bool withoutDeref)
{ 
  OMS_TRACE (omsTrInterface, m_pSession->m_lcSink, "omsTryLockShared(Var) : " << oid);
  OMS_CHECK_EXCEPTION(*this);

  // TODO: consider withoutDeref!!

  ALIGN_OID_CONST(oid8, oid);

  return m_pSession->CurrentContext()->TryLockObj(oid8, /*shared=*/true);
}

/*----------------------------------------------------------------------*/

void OmsHandle::omsLocks(const OmsObjectId*p, int n, bool withoutDeref)
{ 
  _TRACE_METHOD_ENTRY(this,"omsLocks")
  OMS_CHECK_EXCEPTION(*this);
    
  tsp00_Int4 ix;
  for (ix = 0; ix < n; ++ix) {
    omsLock(*p, withoutDeref);
    ++p;
  }
  return;
}

/*----------------------------------------------------------------------*/

void OmsHandle::omsLocksShared(const OmsObjectId*p, int n, bool withoutDeref)
{ 
  _TRACE_METHOD_ENTRY(this,"omsLocksShared")
  OMS_CHECK_EXCEPTION(*this);
    
  tsp00_Int4 ix;
  for (ix = 0; ix < n; ++ix) {
    omsLockShared(*p, withoutDeref);
    ++p;
  }
  return;
}

/*----------------------------------------------------------------------*/

void OmsHandle::omsLockMass(OmsIOidReadIterator& oids, short timeout, OmsIOidAppender& errOids, bool withoutDeref)
{
  _TRACE_METHOD_ENTRY(this,"omsLockMass")
  OMS_CHECK_EXCEPTION(*this);

  // TODO: consider withoutDeref!!

  m_pSession->LockObjMass(oids, timeout, errOids, /*shared=*/false, /*newConsistentView=*/false);
}

/*----------------------------------------------------------------------*/

void OmsHandle::omsLockSharedMass(OmsIOidReadIterator& oids, short timeout, OmsIOidAppender& errOids, bool withoutDeref)
{
  _TRACE_METHOD_ENTRY(this,"omsLockSharedMass")
  OMS_CHECK_EXCEPTION(*this);

  // TODO: consider withoutDeref!!

  m_pSession->LockObjMass(oids, timeout, errOids, /*shared=*/true, /*newConsistentView=*/false);
}

/*----------------------------------------------------------------------*/

void OmsHandle::omsMarkVersion ( const OmsVersionId& versionId )
{
  OMS_TRACE (omsTrInterface|omsTrVersion, m_pSession->m_lcSink, "omsMarkVersion : " << OMS_CharBuffer(versionId, sizeof(OmsVersionId)));
  OMS_CHECK_EXCEPTION(*this);

  OMS_Context*     context;
  { // Begin of exclusive critical section.    // PTS 1124533, 1133629
    OMS_InternalLockScope scope(m_pSession, OMS_Session::LIBOMS_RWLOCK_AREA, 
      OMS_Globals::m_globalsInstance->m_versionDictionary.GetSingleLockId(versionId), true/*exclusive*/);

    context = OMS_Globals::m_globalsInstance->m_versionDictionary.FindVersion(versionId);
    if (NULL == context) {
      OMS_TRACE (omsTrVersion, m_pSession->m_lcSink, " - Mark failed because version was not found");
      m_pSession->ThrowDBError (e_unknown_version, "omsMarkVersion", versionId, __MY_FILE__, __LINE__);
    }
    context->MarkVersion();
  } // End of exclusive critical section.
}

/*----------------------------------------------------------------------*/

int OmsHandle::omsDerefMass(const OmsIDerefIter &derefIter){
  return m_pSession->MassDeref(*const_cast<OmsIDerefIter*>(&derefIter));
}

/*----------------------------------------------------------------------*/

int OmsHandle::omsDerefMass(const OmsDeref &derefIter){
  return m_pSession->MassDeref(*const_cast<OmsIDerefIter*>(reinterpret_cast<const OmsIDerefIter*>(&derefIter)));
}

/*----------------------------------------------------------------------*/

int OmsHandle::omsDerefMass(const OmsDerefArray &derefIter){
  return m_pSession->MassDeref(*const_cast<OmsIDerefIter*>(reinterpret_cast<const OmsIDerefIter*>(&derefIter)));
}

/*----------------------------------------------------------------------*/

int OmsHandle::omsDerefForUpdMass(const OmsIDerefIterForUpd &derefIter, bool doLock){
  return m_pSession->MassDerefForUpd(*const_cast<OmsIDerefIterForUpd*>(&derefIter), doLock, false);
}

/*----------------------------------------------------------------------*/

int OmsHandle::omsDerefForUpdMass(const OmsDerefForUpd &derefIter, bool doLock){
  return m_pSession->MassDerefForUpd(*const_cast<OmsIDerefIterForUpd*>(reinterpret_cast<const OmsIDerefIterForUpd*>(&derefIter)),
                                     doLock, false);
}

/*----------------------------------------------------------------------*/

int OmsHandle::omsDerefForUpdMass(const OmsDerefArrayForUpd &derefIter, bool doLock){
  return m_pSession->MassDerefForUpd(*const_cast<OmsIDerefIterForUpd*>(reinterpret_cast<const OmsIDerefIterForUpd*>(&derefIter)),
                                     doLock, false);
}

/*----------------------------------------------------------------------*/

int OmsHandle::omsDerefKeyMass(const OmsIDerefKeyIter &derefIter){
  return m_pSession->MassDerefViaKey(*const_cast<OmsIDerefKeyIter*>(&derefIter));
}

/*----------------------------------------------------------------------*/

int OmsHandle::omsDerefKeyMass(const OmsDerefKeyArray &derefIter){
  return m_pSession->MassDerefViaKey(*const_cast<OmsIDerefKeyIter*>(reinterpret_cast<const OmsIDerefKeyIter*>(&derefIter)));
}

/*----------------------------------------------------------------------*/

int OmsHandle::omsDerefKeyMassForUpd(const OmsIDerefKeyIterForUpd &derefIter, bool doLock){
  return m_pSession->MassDerefViaKeyForUpd(*const_cast<OmsIDerefKeyIterForUpd*>(&derefIter), doLock, false);
}

/*----------------------------------------------------------------------*/

int OmsHandle::omsDerefKeyMassForUpd(const OmsDerefKeyArrayForUpd &derefIter, bool doLock){
  return m_pSession->MassDerefViaKeyForUpd(*const_cast<OmsIDerefKeyIterForUpd*>(reinterpret_cast<const OmsIDerefKeyIterForUpd*>(&derefIter)),
                                           doLock, false);
}

/*----------------------------------------------------------------------*/

void OmsHandle::omsMethodCallEpilog(void* pSH) {     /* PTS 1120794 */
  OmsCallbackInterface* cbi = m_pSession->GetCallbackInterface();
  if (NULL != cbi) {
    /* PTS 1120794 */
    if (NULL != pSH) {
      cbi->omsMethodCallEpilog(*this,*REINTERPRET_CAST(SqlHandle*,(pSH)));
    }
    else {
      cbi->omsMethodCallEpilog(*this);
    }
  }
}

/*----------------------------------------------------------------------*/

void OmsHandle::omsMonitorLock()
{
  (*m_session)->EnterCriticalSection(RGN_MONITOR);
}

/*----------------------------------------------------------------------*/

void OmsHandle::omsMonitorUnlock()
{
  (*m_session)->LeaveCriticalSection(RGN_MONITOR);
}

/*----------------------------------------------------------------------*/

void OmsHandle::omsNewConsistentView ()
{
  OMS_TRACE (omsTrInterface, m_pSession->m_lcSink, "omsNewConsistentView : ");
  if (m_pSession->CurrentContext()->IsVersion()) {
    return;
  }
  OMS_CHECK_EXCEPTION(*this);

  m_pSession->CurrentContext()->LockObjWithBeforeImage();    // PTS 1128108
  m_pSession->CurrentContext()->RemoveUnlockedObjFromCache();
  m_pSession->CurrentContext()->ClearReadOnlyObjCache(/*deleteObjects=*/true);
  m_pSession->CurrentContext()->NewConsistentView();
}

/*----------------------------------------------------------------------*/

void OmsHandle::omsNewConsistentView (OmsObjectId* pOids, int cntOids, short timeout)
{

  int dummy = 0;
  omsNewConsistentView (pOids, cntOids, timeout, NULL, dummy);
}

/*----------------------------------------------------------------------*/

//static bool operator<(const OmsObjectId &a, OmsObjectId &b)
//{
//  return (a < b);
//}

void OmsHandle::omsNewConsistentView (OmsObjectId* pOids, int cntOids, short timeout, OmsObjectId* pErrOids, int& cntErrOids)
{
  OMS_TRACE (omsTrInterface, m_pSession->m_lcSink, "omsNewConsistentView : " << "cnt " << cntOids << "timeout " << timeout);
  OMS_CHECK_EXCEPTION(*this);
  
  if (cntOids == 0){
    omsNewConsistentView();
  }
  else {
    gg200QuickSort(&pOids[0], cntOids);

    OMS_OidArrayReader exclOids(pOids, cntOids);
    OMS_OidArrayWriter exclErrOids(pErrOids, cntErrOids);
    OMS_OidArrayReader sharedOids(NULL, 0);  // dummy
    OMS_OidArrayWriter sharedErrOids(NULL, 0); // dummy

    m_pSession->NewConsistentView(exclOids, sharedOids, timeout, exclErrOids, sharedErrOids);
    cntErrOids = exclErrOids.omsGetCntr();
  }
}

/*----------------------------------------------------------------------*/

// PTS 1115027

void OmsHandle::omsNewConsistentView (OmsIOidReadIterator &exclOids, 
                                      short                timeout, 
                                      OmsIOidAppender     &exclErrOids)
{
  OMS_CHECK_EXCEPTION(*this);

  OMS_OidArrayReader sharedOids(NULL, 0);  // dummy
  OMS_OidArrayWriter sharedErrOids(NULL, 0); // dummy
  m_pSession->NewConsistentView(exclOids, sharedOids, timeout, exclErrOids, sharedErrOids);
}

/*----------------------------------------------------------------------*/

void OmsHandle::omsNewConsistentView (OmsIOidReadIterator  &exclOids, 
                                      OmsIOidReadIterator  &sharedOids,
                                      short                 timeout, 
                                      OmsIOidAppender      &exclErrOids,
                                      OmsIOidAppender      &sharedErrOids)
{
  OMS_CHECK_EXCEPTION(*this);

  m_pSession->NewConsistentView(exclOids, sharedOids, timeout, exclErrOids, sharedErrOids);
}

/*----------------------------------------------------------------------*/

OmsAbstractObject* OmsHandle::omsNewKeyedObject(ClassID	 guid, 
                                                const unsigned char* key,
                                                OmsSchemaHandle      Schema,
                                                OmsContainerNo       ContainerNo )
{
  _TRACE_METHOD_ENTRY(this,"omsNewKeyObject");
  OMS_CHECK_EXCEPTION(*this);

  return m_pSession->NewKeyedObject (guid, key, Schema, ContainerNo);
}

/*----------------------------------------------------------------------*/

OmsAbstractObject* OmsHandle::omsNewObject( ClassID guid, OmsSchemaHandle Schema, OmsContainerNo ContainerNo, size_t arrayByteSize )
{
  _TRACE_METHOD_ENTRY(this,"omsNewObject")
  OMS_CHECK_EXCEPTION(*this);

  return m_pSession->NewObject(guid, Schema, ContainerNo, arrayByteSize);    
}

/*----------------------------------------------------------------------*/

OmsAbstractObject* OmsHandle::omsNewRegistryObject (size_t sz, ClassID guid)
{
  OmsObjectContainerPtr p;
  sz += OmsObjectContainer::headerSize();
  if (m_pSession->InVersion()) {
    p = (OmsObjectContainerPtr) m_pSession->m_context->allocate(sz); 
  }
  else {
    p = (OmsObjectContainerPtr) m_pSession->m_context->StackHeapMalloc(sz); // PTS 1134841
  }
  if (p) {
    p->Init();
    return &p->m_pobj;
  }
  return NULL;  
}

/*----------------------------------------------------------------------*/

OmsVarOid OmsHandle::omsNewVarObject(OmsSchemaHandle Schema, OmsContainerNo ContainerNo, ClassID guid)
{
  OMS_TRACE (omsTrInterface | omsTrVarObject, m_pSession->m_lcSink, "omsNewVarObj : " 
    << Schema << ", " << ContainerNo <<", " << guid);
  OMS_CHECK_EXCEPTION(*this);

  OMS_ObjectId8 oid8 = m_pSession->NewVarObject(Schema, ContainerNo, guid);
  return reinterpret_cast<OmsVarOid&>(oid8);
}

/*----------------------------------------------------------------------*/

void  OmsHandle::omsPrintOid (char* buf, size_t bufSize, const OmsObjectId oid)
{
   char auxbuf[256];
   //sprintf ( auxbuf, "OID : %d.%d (vers %d)", oid.getPno(), oid.getPagePos(), oid.getGeneration());                     // PTS 1125307
   sp77sprintf ( auxbuf, sizeof(auxbuf), "OID : %d.%d (vers %d)", oid.getPno(), oid.getPagePos(), oid.getGeneration());   // PTS 1125307
   if (strlen(auxbuf) < bufSize) {
     strcat (buf, auxbuf);
   }
}

/*----------------------------------------------------------------------*/

int OmsHandle::omsOidInfo (const OmsObjectId& oid, 
                           ClassID &Guid, 
                           OmsObjectType &type,
                           OmsSchemaHandle& Schema, char* SchemaName, int SchemaNameBufSize,
                           char* ClassName, int ClassNameBufSize, OmsContainerNo& ContainerNo)
{
  OmsTypeWyde SchemaNameWyde[OMS_MAX_SCHEMA_NAME_LENGTH + 1];

  int rc = omsOidInfo (oid, 
                       Guid, 
                       type,
                       Schema, SchemaNameWyde, OMS_MAX_SCHEMA_NAME_LENGTH + 1,
                       ClassName, ClassNameBufSize, ContainerNo);

  if (SchemaNameBufSize > 0 && SchemaName != NULL){
    WydeToChar(&SchemaNameWyde[0], SchemaName, SchemaNameBufSize, "omsOidInfo");
  }

  return rc;
}

/*----------------------------------------------------------------------*/

int OmsHandle::omsOidInfo (const OmsObjectId& oid, 
                           ClassID &Guid, 
                           OmsObjectType &type,
                           OmsSchemaHandle& Schema, OmsTypeWyde* SchemaName, int SchemaNameBufSize,
                           char* ClassName, int ClassNameBufSize, OmsContainerNo& ContainerNo)
{
  OMS_CHECK_EXCEPTION(*this);
  
  omsIfNilOidThrowError (this, oid, "omsOidInfo", __MY_FILE__, __LINE__);

  OmsObjectContainerPtr pObj = NULL;
  try {
    ALIGN_OID_CONST(oid8, oid);

    pObj = m_pSession->CurrentContext()->GetObj(oid8, /*doLock=*/false, /*shared=*/false);
  }
  catch (OmsObjectNotFound &e){
    // Only errors which corresponds to a "NOT-FOUND" are converted into 'false'.
    // All other exceptions are passed to the calling method.
    DbpErrorCallbackInterface *pCBInterface = DbpError::dbpGetCallback();  
    if (pCBInterface){
      pCBInterface->dbpCaughtError(e);
    }
    return -1;
  }

  if (NULL == pObj) {
    return -1;
  }
  else {
    OMS_ContainerEntry* pContainerInfo = pObj->GetContainerInfo(__MY_FILE__, __LINE__);
    Guid        = pContainerInfo->GetGuid();
    Schema      = pContainerInfo->GetSchema();
    ContainerNo = pContainerInfo->GetContainerNo();
    if (pContainerInfo->GetClassEntry().IsVarObject()){
      type = OMS_VAR_OBJ;
    }
    else if (pContainerInfo->GetClassEntry().IsArrayObject()){
      type = OMS_ARRAY_OBJ;
    }
    else if (pContainerInfo->GetClassEntry().IsKeyedObject()){
      type = OMS_KEYED_OBJ;
    }
    else {
      type = OMS_NORMAL_OBJ;
    }

    if (SchemaNameBufSize > 0 && SchemaName != NULL){
      omsGetSchemaName(Schema, SchemaName, SchemaNameBufSize);
    }

    if (ClassNameBufSize > 0 && ClassName != NULL) {
      int ClassNameLength = (int) strlen(pContainerInfo->GetClassEntry().GetClassName());
      if (ClassNameLength >= ClassNameBufSize) {
        ClassNameLength = ClassNameBufSize - 1;
      }
      SAPDB_MemCopyNoCheck (ClassName, pContainerInfo->GetClassEntry().GetClassName(), ClassNameLength); 
      ClassName[ClassNameLength] = 0;
    }

    return 0;
  }
}

/*----------------------------------------------------------------------*/

void OmsHandle::omsOpenVersion( const OmsVersionId& versionId )
{
  OMS_TRACE (omsTrInterface|omsTrVersion, m_pSession->m_lcSink, "omsOpenVersion : " << OMS_CharBuffer(versionId, sizeof(OmsVersionId)));
  OMS_CHECK_EXCEPTION(*this);

  OMS_Context*        context;
  const char*             msg          = "omsOpenVersion";
  const char*             msgInVersion = "omsOpenVersion, already in version";
 
  if (m_pSession->InVersion()) {
    OMS_TRACE (omsTrVersion, m_pSession->m_lcSink, " - Open failed, because version (" << 
               OMS_CharBuffer(m_pSession->m_context->GetVersionId(), sizeof(OmsVersionId)) << ") is still open.");
    m_pSession->ThrowDBError (e_already_in_version, msg, m_pSession->CurrentContext()->GetVersionId(), __MY_FILE__, __LINE__); 
  }

  // Check if there are objects which have been changed in the current transaction
  m_pSession->OpenVersionProlog(versionId, false);    // PTS 1129082

  { // Begin of exclusive critical section.    // PTS 1124533, 1133629
    OMS_InternalLockScope scope(m_pSession, OMS_Session::LIBOMS_RWLOCK_AREA, 
      OMS_Globals::m_globalsInstance->m_versionDictionary.GetSingleLockId(versionId), true/*exclusive*/);
  
    context = OMS_Globals::m_globalsInstance->m_versionDictionary.FindVersion(versionId);
    if (NULL == context) {
      OMS_TRACE (omsTrVersion, m_pSession->m_lcSink, " - Open failed, because version was not found.");
      m_pSession->ThrowDBError (e_unknown_version, msg, versionId, __MY_FILE__, __LINE__);
    }
    if (context->IsDropped()) {
      goto Drop;  // The deletion itself is done outside the critical section
    }
    if (m_pSession->IsSubtransOpen()) {
      OMS_TRACE (omsTrVersion, m_pSession->m_lcSink, " - Open failed, because subtrans is open.");
      m_pSession->ThrowDBError (e_OMS_too_many_subtrans, "Open Subtrans", versionId, __MY_FILE__, __LINE__); 
    }
    if ((context->IsBoundToTrans()) && (!m_pSession->VersionBoundByMe(context))) 
    {
      tsp00_TaskId tid = context->GetSession()->GetTaskId();
      char tid_c[10];
      sp77sprintf(tid_c, sizeof(tid_c), "T%d", tid);
      char         vId[sizeof(OmsVersionId)+1];
      SAPDB_MemCopyNoCheck (&vId[0], &versionId[0], sizeof(OmsVersionId));
      vId[sizeof(OmsVersionId)] = 0;
      DbpBase b(*m_session);
      if (context->IsVersionOpen()){
        OMS_TRACE (omsTrVersion, m_pSession->m_lcSink, " - Open failed, because version is already opened by another transaction:" << tid_c);
        b.dbpOpError("%d, %s already opened by T%3d", e_version_already_open, &vId[0], tid);
        m_pSession->ThrowDBError (e_version_already_open, msg, versionId, __MY_FILE__, __LINE__);
      }
      else {
        OMS_TRACE (omsTrVersion, m_pSession->m_lcSink, " - Open failed, because version is still bound to another transaction: " << tid_c);
        b.dbpOpError("%d, %s bound by T%3d", e_version_bound_by_trans, &vId[0], tid);
        m_pSession->ThrowDBError (e_version_bound_by_trans, msg, versionId, __MY_FILE__, __LINE__);
      }
    }

    m_pSession->OpenVersion(context, false);
    m_pSession->IncOpenVersion();
    OMS_Globals::m_globalsInstance->m_versionDictionary.MarkNotUnloadable(m_pSession->m_lcSink, context);
  } // End of exclusive critical section.

  // Free consistent view of default context
  m_pSession->OpenVersionEpilog();  // PTS 1129082

  OMS_TRACE (omsTrVersion, m_pSession->m_lcSink, " - successfully finished.");
  return;

Drop:
  {
    OMS_TRACE (omsTrVersion, m_pSession->m_lcSink, " - Version is marked as dropped. Try to drop it now.");

    try {
      omsForceDropVersion(versionId);
    }
    catch(DbpError &e) {
      OMS_TRACE (omsTrVersion, m_pSession->m_lcSink, " - Drop failed with error: " << e.dbpDBError());
    }
    m_pSession->ThrowDBError (e_unknown_version, msg, versionId, __MY_FILE__, __LINE__);
  }
}

/*----------------------------------------------------------------------*/

void OmsHandle::omsRaiseError (short msgno, const OmsTypeUTF8* errmsg) 
{
  _TRACE_METHOD_ENTRY(this,"omsRaiseError");
  int               charCnt = 0;

  if( OMS_Globals::KernelInterfaceInstance->IsUnicodeInstance() )
  {
    tsp00_Uint4           destBytesWritten;
    tsp00_Uint4           srcBytesParsed;
    tsp78ConversionResult error;
    tsp00_Int4            srcLenByte = (int)strlen((char*)&errmsg[0]);

    unsigned char pDest[ OMS_MAX_ERROR_TEXT_LENGTH * 2 ];
    error = sp78convertString( sp77encodingUCS2,      // destEndcoding
      &pDest[0],             // destBuffer
      sizeof( pDest ),       // destBufferLengthInBytes
      &destBytesWritten,     // destBytesWritten
      true,                  // addZeroTerminator
      sp77encodingUTF8,      // srcEncoding
      errmsg,                // srcBuffer
      srcLenByte,            // srcBufferLengthInBytes
      &srcBytesParsed );     // srcBytesParsed

    if( error != sp78_Ok ) {
      destBytesWritten = 0; 
    }
    if ( ! omsIsUnicodeInstance() ) {
      // copy ucs2 string as ascii -> workaround for correct display of message text in abap short dumps
      /* PTS 1123201, PG
      OMS_Globals::KernelInterfaceInstance->IsUnicodeInstance() ist TRUE iff parameter _UNICODE == YES.
      omsIsUnicodeInstance() is TRUE iff 1. parameter _UNICODE == YES and
      2. either USERS.DEFAULTCODE == UNICODE for the standard LC user
      or     USERS.DEFAULTCODE == DEFAULT for the standard LC user
      and parameter DEFAULT_CODE == UNICODE.

      Standard LC user is the user under which the Dbsl connects to the LC.
      Newer versions of the 6.20 Dbsl and higher allow only connections 
      - from an Ascii SAP system to an Ascii LC and
      - from an Unicode SAP system to an Unicode LC.
      Where   Ascii LC means omsIsUnicodeInstance() == FALSE
      and Unicode LC means omsIsUnicodeInstance() == TRUE.
      So, in case OMS_Globals::KernelInterfaceInstance->IsUnicodeInstance() && !omsIsUnicodeInstance()
      the error text needs to be adjusted. Otherwise question marks occur in ABAP short dumps. */

      m_pSession->m_lcSink->SetError( msgno, destBytesWritten, &pDest[0], csp_ascii );
    } else {
      m_pSession->m_lcSink->SetError( msgno, destBytesWritten, &pDest[0], csp_unicode );
    }
  } else {
    charCnt = (int)strlen((char*)&errmsg[0]); 
    m_pSession->m_lcSink->SetError( msgno, charCnt, errmsg, csp_ascii );
  }
};

/*----------------------------------------------------------------------*/

void OmsHandle::omsReadOnly (bool read_only) 
{
  m_pSession->SetReadOnly(read_only);
}

/*----------------------------------------------------------------------*/

bool OmsHandle::omsGetReadOnly()
{
  return m_pSession->GetReadOnly();
}

/*----------------------------------------------------------------------*/

void OmsHandle::omsRegClass(ClassID            guid,
                            const char        *pClassName,
                            size_t             objectSize,
                            const OMS_KeyDesc &keyDesc,
                            ClassID            baseGuid,
                            OmsAbstractObject *pObj)
{
  _TRACE_METHOD_ENTRY(this,"OmsHandle::omsRegClass");
  OMS_CHECK_EXCEPTION(*this);

  m_pSession->RegClass(guid, pClassName, objectSize, keyDesc, baseGuid, pObj,
                       /*isVarObj=*/false, /*isArrayObject=*/false);
}

/*----------------------------------------------------------------------*/

void OmsHandle::omsRegVarClass(ClassID     guid,
                               const char *pClassName)
{
  _TRACE_METHOD_ENTRY(this,"OmsHandle::omsRegVarClass");
  OMS_CHECK_EXCEPTION(*this);

  OMS_VarObjInfo info;
  m_pSession->RegClass(guid, pClassName, sizeof(info), OMS_KeyDesc(),
                       /*baseGuid=*/0, &info, /*isVarObj=*/true,
                       /*isArrayObj=*/false);
}

/*----------------------------------------------------------------------*/

void OmsHandle::omsRegArrayClass(ClassID            guid,
                                 const char        *pClassName,
                                 size_t             objectSize,
                                 OmsAbstractObject *pObj)
{
  _TRACE_METHOD_ENTRY(this,"OmsHandle::omsRegArrayClass");
  OMS_CHECK_EXCEPTION(*this);

  m_pSession->RegClass(guid, pClassName, objectSize, OMS_KeyDesc(),
                       /*baseGuid=*/0, pObj, /*isVarObj=*/false,
                       /*isArrayObj=*/true);
}

/*----------------------------------------------------------------------*/

void OmsHandle::omsRegMonitor (void* pMon)
{ 
  m_monitor = REINTERPRET_CAST(OMS_COMMonitor*, (pMon)); 
} 

/*----------------------------------------------------------------------*/

void OmsHandle::omsRegTransactionInterface (ClassID guid, OmsCallbackInterface* pInterface)
{
  m_pSession->RegisterCallbackInterface(guid, pInterface);
}

/*----------------------------------------------------------------------*/

void OmsHandle::omsRelease( const OmsObjectId& oid )
{
  OMS_TRACE (omsTrInterface, m_pSession->m_lcSink, "omsRelease : " << oid);

  ALIGN_OID_CONST(oid8, oid);

  m_pSession->ReleaseObj(oid8);
}

/*----------------------------------------------------------------------*/

void OmsHandle::omsRelease ( const OmsVarOid& oid )
{
  OMS_TRACE (omsTrInterface, m_pSession->m_lcSink, "omsRelease : " << oid);

  ALIGN_OID_CONST(oid8, oid);

  m_pSession->ReleaseObj (oid8); 
};

/*----------------------------------------------------------------------*/

void OmsHandle::omsReleaseAll()
{
  const bool emptyObjCache = true;
  if (!m_pSession->IsSubtransOpen() && !m_pSession->InVersion()) {
    m_pSession->CurrentContext()->FlushObjCache(emptyObjCache);

    // If no subtrans is open, then before images are only generated 
    // if program runs in the simulator. 
    // TODO: Is the following call needed for the simulator??
    //m_pSession->FreeBeforeImages();                                 
  }
  else {
    // Only objects which not have been changed can be released.
    m_pSession->ReleaseAllUnchanged();
  }
}

/*----------------------------------------------------------------------*/

HRESULT OmsHandle::omsReleaseDefaultContext() 
{
  if (NULL != m_monitor) {
    OMS_SessionCriticalSection region(this->m_pSession, RGN_MONITOR);
    region.Enter();
    OMS_Globals::m_globalsInstance->m_monitorDirectory.Insert(m_monitor); /* PTS 1107731 */
    region.Leave();
  }
  
  OMS_TRACE(omsTrSession, (*m_session), " Release Session " << m_pSession << " of Hndl " << this << " RefCnt " << m_pSession->m_refCnt);
  m_pSession->DeleteSession();
  return S_OK;
}

/*----------------------------------------------------------------------*/

void OmsHandle::omsReleaseUnconditional( const OmsObjectId& oid)
{
  ALIGN_OID_CONST(oid8, oid);

  m_pSession->ReleaseObjUnconditional (oid8); 
}

/*----------------------------------------------------------------------*/

void OmsHandle::omsReleaseVarObject(const OmsVarOid& oid)
{
  OMS_TRACE (omsTrInterface | omsTrVarObject, m_pSession->m_lcSink, "omsReleaseVarObject : " << oid);

  ALIGN_OID_CONST(oid8, oid);

  m_pSession->ReleaseVarObject(oid8);
}

/*----------------------------------------------------------------------*/

void OmsHandle::omsReleaseConsistentView()
{
  OMS_TRACE (omsTrInterface, m_pSession->m_lcSink, "omsReleaseConsistentView");
  tsp00_Int2  DummyError;
  tsp00_Int4  BufLen = 0;
  tsp00_Int4  Buf;
  OMS_HResult hr = (*m_session)->MultiPurpose (m_end, mm_nil, &BufLen, 
    (unsigned char*) &Buf, &DummyError);
}

/*----------------------------------------------------------------------*/

/* PTS 1115134 */
void OmsHandle::omsResetVersion(const OmsVersionId& versionId)
{
  OMS_TRACE (omsTrInterface|omsTrVersion, m_pSession->m_lcSink, "omsResetVersion : " << OMS_CharBuffer(versionId, sizeof(OmsVersionId)));
  OMS_CHECK_EXCEPTION(*this);

  const char*  msg  = "omsResetVersion";

  bool isDropped = false;    
  { // Begin of exclusive critical section.    // PTS 1124533, 1133629
    OMS_InternalLockScope scope(m_pSession, OMS_Session::LIBOMS_RWLOCK_AREA, 
      OMS_Globals::m_globalsInstance->m_versionDictionary.GetSingleLockId(versionId), true/*exclusive*/);

    OMS_Context* context = OMS_Globals::m_globalsInstance->m_versionDictionary.FindVersion(versionId);
    if (NULL == context) {
      OMS_TRACE (omsTrVersion, m_pSession->m_lcSink, " - Reset failed because version was not found.");
      m_pSession->ThrowDBError (e_unknown_version, msg, versionId, __MY_FILE__, __LINE__);
    }
    if (context->IsDropped()) {
      isDropped = true;
    }
    if (m_pSession->IsSubtransOpen()) {
      OMS_TRACE (omsTrVersion, m_pSession->m_lcSink, " - Reset failed because subtrans is open.");
      m_pSession->ThrowDBError (e_OMS_too_many_subtrans, "Open Subtrans", versionId, __MY_FILE__, __LINE__); 
    }
    if (context->IsBoundToTrans()) {
      tsp00_TaskId tid = context->GetSession()->GetTaskId();
      char tid_c[10];
      sp77sprintf(tid_c, sizeof(tid_c), "T%d", tid);
      OMS_TRACE (omsTrVersion, m_pSession->m_lcSink, " - Reset failed because version is still bound to another transaction: " << tid_c);
      m_pSession->ThrowDBError (e_version_already_open, msg, versionId, __MY_FILE__, __LINE__);
    }

    m_pSession->OpenVersion(context, /*create=*/false);
    context->ResetVersion(*m_pSession);
    m_pSession->CloseVersion();
  } // End of exclusive critical section. 

  if (isDropped){
    OMS_TRACE (omsTrVersion, m_pSession->m_lcSink, " - Version is marked as dropped. Try to drop it now.");

    try {
      omsForceDropVersion(versionId);
    }
    catch(DbpError &e) {
      OMS_TRACE (omsTrVersion, m_pSession->m_lcSink, " - Drop failed with error: " << e.dbpDBError());
    }
    m_pSession->ThrowDBError (e_unknown_version, msg, versionId, __MY_FILE__, __LINE__);
  }

  OMS_TRACE (omsTrVersion, m_pSession->m_lcSink, " - successfully finished.");
}

/*----------------------------------------------------------------------*/

void OmsHandle::omsRollbackHandling (bool doRelease, bool ignoreErrors)
{
  OMS_TRACE (omsTrInterface, m_pSession->m_lcSink, "omsRollback");
  
  int phase = 0;
  try
  {
    // no application wanted exceptions in the course of transaction end
    this->omsControlCancelFlag(-1);
    this->omsControlDbpException(-1);

    phase = 1;
    if (!ignoreErrors)
    {
      m_pSession->RollbackSubtrans(2, true);
    }

    phase = 2;
    m_pSession->m_defaultContext->ClearVarObjData();
    phase = 3;
    m_pSession->FreeBeforeImages(2);  
    phase = 4;
    m_pSession->TransEnd();
    phase = 5;
    if (doRelease) 
    {
      SessionEnd();  /* PTS 1107209 */
    }
  }
  catch (DbpError& e)
  {
    if (!ignoreErrors)
    { 
      dbpOpError("omsRollback (%d): DbpError %d (%s)", phase, e.dbpError(), e.m_errorText);
      omsTerminate();
    }
    else {
      DbpErrorCallbackInterface *pCBInterface = DbpError::dbpGetCallback();  // PTS 1127871
      if (pCBInterface){
        pCBInterface->dbpCaughtError(e);
      }
    }
  }
  catch (BAD_ALLOC_GEO573&)
  {
    if (!ignoreErrors)
    { 
      dbpOpError("omsRollback (%d): Bad-Alloc", phase);
      omsTerminate();
    }
    else {
      DbpErrorCallbackInterface *pCBInterface = DbpError::dbpGetCallback();  // PTS 1127871
      if (pCBInterface){
        pCBInterface->dbpCaughtBadAlloc();
      }
    }
  }
}

/*----------------------------------------------------------------------*/

void OmsHandle::omsRollbackSubtrans( int required_lvl )
{
  OMS_TRACE (omsTrInterface, m_pSession->m_lcSink, "omsRollbackSubtrans : " << required_lvl);
  if (!m_pSession->IsSubtransOpen()) {
    return;
  }
  OMS_CHECK_EXCEPTION(*this);
  m_pSession->IncSubtransRollback();
  m_pSession->RollbackSubtrans(required_lvl, false);
}

/*----------------------------------------------------------------------*/

void OmsHandle::omsRenameVersion ( const OmsVersionId& oldId, const OmsVersionId& newId)
{
  OMS_TRACE (omsTrInterface|omsTrVersion, m_pSession->m_lcSink, "omsRenameVersion : " 
            << "Old: " << OMS_CharBuffer(oldId, sizeof(oldId)) 
            << " New: " << OMS_CharBuffer(newId, sizeof(newId)));
  OMS_CHECK_EXCEPTION(*this);

  const char* msg = "omsRenameVersion";

  { // Begin of exclusive critical section.    // PTS 1124533
    int   pLock[2];
    const OmsVersionId *pVersionId[2];
    pVersionId[0] = &newId;
    pVersionId[1] = &oldId;
    int size = 2;
    OMS_Globals::m_globalsInstance->m_versionDictionary.GetMultiLockIds(pVersionId, size, &pLock[0]);
    OMS_InternalMultiScope scope(m_pSession, OMS_Session::LIBOMS_RWLOCK_AREA, pLock, size, true/*exclusive*/); // PTS 1133629

    OMS_Context* context = OMS_Globals::m_globalsInstance->m_versionDictionary.FindVersion(oldId);
    if (NULL == context) {
      OMS_TRACE (omsTrVersion, m_pSession->m_lcSink, " - Rename failed because version was not found");

      m_pSession->ThrowDBError(e_unknown_version, msg, oldId, __MY_FILE__, __LINE__);
    }

    // PTS 1125433 Check that version is not bound by another session
    if ((context->IsBoundToTrans()) && (!m_pSession->VersionBoundByMe(context))){
      OMS_TRACE (omsTrVersion, m_pSession->m_lcSink, " - Rename failed because version is bound to another transaction");
      m_pSession->ThrowDBError (e_version_bound_by_trans, msg, oldId, __MY_FILE__, __LINE__);
    }

    // PTS 1125433 Changed calling sequence of drop and insert
    OMS_Globals::m_globalsInstance->m_versionDictionary.DropVersion(oldId); 
    context->RenameVersion (oldId, newId);

    tgg00_BasisError e = OMS_Globals::m_globalsInstance->m_versionDictionary.InsertVersion(newId, context);
    if (e_ok != e) {
      OMS_TRACE (omsTrVersion, m_pSession->m_lcSink, " - Rename failed because new version cannot be inserted. Revert renaming.");
      // PTS 1125433 Re-Insert version after error occurred
      context->RenameVersion (newId, oldId);
      OMS_Globals::m_globalsInstance->m_versionDictionary.InsertVersion(oldId, context);

      m_pSession->ThrowDBError(e, msg, newId, __MY_FILE__, __LINE__);
    }
  } // End of exclusive critical section.

  OMS_TRACE (omsTrVersion, m_pSession->m_lcSink, " - successfully finished.");
}

/*----------------------------------------------------------------------*/

void OmsHandle::omsSetTimeOut( short timeout )
{
  tsp00_Int2  DummyError;
  tsp00_Int4  BufLen = sizeof(timeout);
  m_pSession->m_timeout = timeout;
  OMS_HResult hr = (*m_session)->MultiPurpose (m_set, mm_lock_timeout, &BufLen, 
    (unsigned char*) &timeout, &DummyError);
}

// PTS 1117571
/*----------------------------------------------------------------------*/
// Switch ON or OFF the feature of CACHED KEYS. 
bool OmsHandle::omsSetCachedKeys(bool on, 
                                 ClassID guid, 
                                 OmsSchemaHandle schema, 
                                 OmsContainerNo containerNo)
{
  OMS_TRACE (omsTrInterface | omsTrContainerDir, m_pSession->m_lcSink, "omsSetCachedKeys: " << (on?"ON":"OFF"));
  OMS_CHECK_EXCEPTION(*this);

  OMS_ContainerEntry* pContainerInfo = m_pSession->m_context->GetContainerDir().GetContainerEntry(guid, schema, containerNo);
  return pContainerInfo->SetCachedKeys(on);
}

/*----------------------------------------------------------------------*/
// PTS 1117690
void OmsHandle::omsSetCurrVersionDesc (const char* desc)
{ 
  if (!m_pSession->InVersion())
    m_pSession->ThrowDBError (e_unknown_version, "omsSetCurrVersionDesc: Currently not in version", __MY_FILE__, __LINE__);

  OMS_TRACE (omsTrInterface|omsTrVersion, m_pSession->m_lcSink, "omsSetCurrVersionDesc (I)");

  if (desc == NULL){
    omsSetCurrVersionDesc(reinterpret_cast<OmsTypeWyde*>(NULL));
  }
  else{
    OmsTypeWyde descBuf_wyde[OMS_MAX_VERSION_DESC_LENGTH + 1];
    int len      = (int)strlen(desc);
    int len_byte = (OMS_MAX_VERSION_DESC_LENGTH + 1) * sizeof(OmsTypeWyde);

    tsp00_Uint4 destBytesWritten;
    tsp00_Uint4 srcBytesParsed;
    tsp78ConversionResult error;

    // Convertion from ASCII to UCS2  
    error = sp78convertString(sp77encodingUCS2Native,  // destEndcoding
                              descBuf_wyde,            // destBuffer
                              len_byte,                // destBufferLengthInBytes
                              &destBytesWritten,       // destBytesWritten
                              true,                    // addZeroTerminator
                              sp77encodingAscii,       // srcEncoding
                              desc,                    // srcBuffer
                              len,                     // srcBufferLengthInBytes
                              &srcBytesParsed);        // srcBytesParsed
        
    if (error == sp78_Ok){
      // Description is only set if there are no errors in convertion
      omsSetCurrVersionDesc(descBuf_wyde);
    }
    else {
      OMS_TRACE (omsTrVersion, m_pSession->m_lcSink, " - Convertion of description failed with error: " << error);
    }
  }
}

/*----------------------------------------------------------------------*/
// PTS 1117690
void OmsHandle::omsSetCurrVersionDesc (const OmsTypeWyde* desc)
{ 
  if (!m_pSession->InVersion())
    m_pSession->ThrowDBError (e_unknown_version, "omsSetCurrVersionDesc: Currently not in version", __MY_FILE__, __LINE__);

  OMS_TRACE (omsTrInterface|omsTrVersion, m_pSession->m_lcSink, "omsSetCurrVersionDesc (II)");

  m_pSession->CurrentContext()->SetVersionDesc(desc);
}

/*----------------------------------------------------------------------*/


int OmsHandle::omsStartSubtrans()
{
  OMS_TRACE (omsTrInterface, m_pSession->m_lcSink, "omsStartSubtrans");
  OMS_CHECK_EXCEPTION(*this);

  return m_pSession->StartSubtrans();
}

/*----------------------------------------------------------------------*/
// PTS 1122839
void OmsHandle::omsStoreErrorCode(long errorCode) 
{
  m_pSession->StoreErrorCode(errorCode);
}

/*----------------------------------------------------------------------*/

void OmsHandle::omsStoreVarObject ( const OmsVarOid& oid, const void* pVarObj, unsigned long objLength)
{
  OMS_TRACE (omsTrInterface | omsTrVarObject, m_pSession->m_lcSink, "omsStoreVarObject : " << oid << ", length is " << objLength);
  OMS_CHECK_EXCEPTION(*this);

  ALIGN_OID_CONST(oid8, oid);

  if (!m_pSession->IsLocked(oid8, /*shared=*/false))
  {
    m_pSession->ThrowDBError (e_object_not_locked, "omsStoreVarObject", oid8, __MY_FILE__, __LINE__);
  }
  m_pSession->StoreVarObject(oid8, pVarObj, objLength);
}

/*----------------------------------------------------------------------*/

void OmsHandle::omsTerminate()
{
  omsTerminate((OmsTypeUTF8*) "");
}

/*----------------------------------------------------------------------*/

void OmsHandle::omsTerminate(const char* msg)
{
  omsTerminate((OmsTypeUTF8*) msg);
}

/*----------------------------------------------------------------------*/

void OmsHandle::omsTerminate(const OmsTypeUTF8* msg) /* PTS 1109963 */
{
  dbpOpMsg("omsTerminate called %s", msg);
  tgg00_BasisError e = e_terminate; /* PTS 1107666 */
  tsp00_Int4  BufLen = sizeof(e);
  tsp00_Int2  SqlError;
  m_pSession->IncOmsTerminate();

  m_pSession->CurrentMethodCallEpilog(-1); // PTS 1126797
  // transform basis error into sql return code
  m_pSession->m_lcSink->MultiPurpose (m_change, mm_nil, &BufLen, 
    (unsigned char*) &e, &SqlError);

  omsRaiseError( SqlError, msg );
}

/*----------------------------------------------------------------------*/
// PTS 1110812
void OmsHandle::omsThrowCancelledError() const
{
  if (m_pSession){
    m_pSession->SetCancelFlag(false);
    OMS_Globals::Throw(e_OMS_cancelled, "cancelled", __MY_FILE__, __LINE__);
  }
}

/*----------------------------------------------------------------------*/

void OmsHandle::omsTrace (const char* tracemsg, int TraceMsgLen) {
  if (NULL != *m_session)
    (*m_session)->Vtrace (TraceMsgLen, (unsigned char*) tracemsg);
}

/*----------------------------------------------------------------------*/

void OmsHandle::omsTracef( const char*fmt, ... )
{
#if defined(WIN32)
  char buffer[256];
#else
  char buffer[1024];
#endif

  va_list args;
  va_start (args, fmt);
#if defined(WIN32)
  _vsnprintf ( buffer, sizeof(buffer), fmt, args );
#else
  vsprintf ( buffer, fmt, args );
  buffer[256] = '\0';
#endif
  omsTrace (buffer, (tsp00_Int4) strlen(buffer));
  va_end (args);
}

/*----------------------------------------------------------------------*/

void OmsHandle::omsTraceOid (const OmsObjectId& oid)
{
  omsTracef ("OID : %d.%d (vers %d)", oid.getPno(), oid.getPagePos(), oid.getGeneration());
}

/*----------------------------------------------------------------------*/

void OmsHandle::omsUnlock (const OmsObjectId& oid)
{
  OMS_TRACE (omsTrInterface, m_pSession->m_lcSink, "omsUnlock : " << oid);

  ALIGN_OID_CONST(oid8, oid);

  m_pSession->UnlockObj(oid8, /*shared=*/false);
}

/*----------------------------------------------------------------------*/

// The Epilog is called after the transaction end (commit or rollback) is
// executed in the kernel.
void OmsHandle::omsTransEndEpilog(bool isCommit, bool release, int &phase) 
{
  m_pSession->AfterKernelTransEnd(true);
  m_pSession->TransEndEpilog(isCommit, phase);

  // PTS 1131350
  OmsCallbackInterface* callbackInterface = m_pSession->GetCallbackInterface();
  if (NULL != callbackInterface) {
    bool read_only = m_pSession->ForceReadOnly();
    if (isCommit){
      callbackInterface->omsInvalidate (*this, 
                                        OmsCallbackInterface::AfterCommit,
                                        m_pSession->InVersion());
    }
    else {
      callbackInterface->omsInvalidate (*this, 
                                        OmsCallbackInterface::AfterRollback,
                                        m_pSession->InVersion());
    }

    m_pSession->UnforceReadOnly(read_only);
  }

  // Check that all memory of transaction allocator has been freed
  m_pSession->UserAllocatorCtl(OmsCallbackInterface::BlockTransaction, NULL);  

  if (release) {
    // SessionEnd
    m_pSession->ReleaseCallbackInterface();

    m_pSession->UserAllocatorCtl(OmsCallbackInterface::BlockUnregister, NULL);
    m_pSession->UserAllocatorCtl(OmsCallbackInterface::BlockSession, NULL);
  }
  m_pSession->AfterKernelTransEnd(false);
}

/*----------------------------------------------------------------------*/

void OmsHandle::omsValidate()
{
}

/*----------------------------------------------------------------------*/

/*!
 * Global allocator is used to allocate memory globally. Applications
 * should use the global allocator only exceptionally, to allocate
 * objects that live longer than a session.
 *
 * \return reference to the global allocator.
 *
 * \see \ref omsuserallocator
 */
OmsAllocatorInterface &OmsHandle::omsGetGlobalAllocator()
{
  OmsAllocatorInterface *alloc;
  alloc = OMS_Globals::m_globalsInstance->m_globalUserAllocatorDirectory.get("");
  return *alloc;
}

/*----------------------------------------------------------------------*/

/*!
 * Global allocator is used to allocate memory globally. Applications
 * should use the global allocator only exceptionally, to allocate
 * objects that live longer than a session.
 *
 * \param name allocator name.
 *
 * \return reference to the specified named global allocator.
 *
 * \see \ref omsuserallocator
 */
OmsAllocatorInterface &OmsHandle::omsGetGlobalAllocator(const char *name)
{
  OmsAllocatorInterface *alloc;
  alloc = OMS_Globals::m_globalsInstance->m_globalUserAllocatorDirectory.get( name);
  return *alloc;
}

/*----------------------------------------------------------------------*/

/*!
 * Session allocator should be used to allocate objects that live until
 * the end of the session. Session allocator will be checked for leftovers
 * on session end. If there was a callback registered, then it will be
 * called for each left-over block.
 *
 * The allocators are actually instantiated at session start time, so this
 * method is exception safe.
 *
 * \return reference to the session-specific allocator.
 *
 * \see \ref omsuserallocator
 */
OmsAllocatorInterface &OmsHandle::omsGetSessionAllocator()
{
  return m_pSession->GetSessionUserAllocator();
}

/*----------------------------------------------------------------------*/

/*!
 * Transaction allocator should be used to allocate objects that live until
 * the end of the transaction. Transaction allocator will be checked for leftovers
 * on commit or rollback. If there was a callback registered, then it will be
 * called for each left-over block.
 *
 * The allocators are actually instantiated at session start time, so this
 * method is exception safe.
 *
 * \return reference to the transaction-specific allocator.
 *
 * \see \ref omsuserallocator
 */
OmsAllocatorInterface &OmsHandle::omsGetTransactionAllocator()
{
  return m_pSession->GetTransactionUserAllocator();
}

/*----------------------------------------------------------------------*/

/*!
 * COM-routine allocator should be used to allocate objects that live only
 * in one COM-routine. COM-routine allocator will be checked for leftovers
 * after each COM-routine. If there was a callback registered, then it will
 * be called for each left-over block.
 *
 * The allocators are actually instantiated at session start time, so this
 * method is exception safe.
 *
 * \return reference to the COM routine-specific allocator.
 *
 * \see \ref omsuserallocator
 */
OmsAllocatorInterface &OmsHandle::omsGetCOMRoutineAllocator()
{
  return m_pSession->GetCOMRoutineUserAllocator();
}

/*----------------------------------------------------------------------*/

/*!
 * This method registers the area in the global area register directory. 
 * The areaId is used internaly to identify a specific application area.
 * The name is used to display a understandable name in the statistic tables 
 * and traces. 
 *
 * \see \ref omslockscopes
 *
 */
void OmsHandle::omsRegisterArea(OmsTypeInt4 areaId, const char *name){
  OMS_Globals::KernelInterfaceInstance->RegisterArea(areaId,name);
}


/*----------------------------------------------------------------------*/

/*!
 * The method sets a memory barrier. It performs a serializing operation on 
 * all load-from-memory and store-to-memory instructions that were issued 
 * prior this method call.
 *
 */
void OmsHandle::omsMemoryBarrier(){
  RTESys_MemoryBarrier(); // PTS 1133627
}

/*----------------------------------------------------------------------*/

void OmsHandle::OmsSetTransactionComment(const char  *pComment)
{
  short DBError = 0;
  size_t len = strlen(pComment);
  OmsTypeWyde* pDest = (OmsTypeWyde *)m_pSession->m_context->allocate((len+1)*2);
  if (NULL == pDest) return;

  size_t ix;
  for (ix = 0; ix < len; ++ix )
    *(pDest+ix) = *(pComment+ix);
  *(pDest+len) = 0;
  m_pSession->m_lcSink->SetTransactionComment(len, pDest, DBError);
  m_pSession->m_context->deallocate(pDest);
  if ( 0 != DBError )
    m_pSession->ThrowDBError (DBError, "OmsSetTransactionComment", __MY_FILE__, __LINE__);
};

/*----------------------------------------------------------------------*/

void OmsHandle::OmsSetTransactionComment(const OmsTypeWyde *pComment)
{
  short DBError = 0;
  short len;
  for (len = 0; len < csp_maxint2; ++len )
  {
    if (0 == *(pComment+len))
      break;
  }
  m_pSession->m_lcSink->SetTransactionComment(len, pComment, DBError);
  if ( 0 != DBError )
    m_pSession->ThrowDBError (DBError, "OmsSetTransactionComment", __MY_FILE__, __LINE__);
};

/*----------------------------------------------------------------------*/

OmsHandle::~OmsHandle ()
{
    _TRACE_METHOD_ENTRY(this,"~OmsHandle");
    // Remove Handle from HandleList         // PTS 1116693
  	if (m_pSession) {
  		for (cgg251dclIterator<OmsHandle*,OMS_Session> iter 
  		    = m_pSession->m_handleList.begin(); iter; ++iter) {
            if (this == *iter()) {
                m_pSession->m_handleList.remove(iter);
                break;
            }
        }
    }

    if (OMS_Globals::m_globalsInstance->InSimulator() && m_pSession && m_pSession->m_lcSink) {
          m_pSession->m_lcSink->SimCtlDestroyHandleCB(*this, &(m_pSession->m_lcSink));
    }
}

/*----------------------------------------------------------------------*/

bool  OmsHandle::omsDerefUntypedOid(unsigned int      page,  
                                    unsigned short    offset,  
                                    unsigned short    generation,
                                    OmsAbstractObject *&pObj, 
                                    int               &byteSize,
                                    OmsVarOid         &varOid)
{
  OMS_TRACE (omsTrInterface, m_pSession->m_lcSink, "omsDerefUntypedOid: Page=" << page << " Offset="<< offset << " Generation=" << generation);

  OMS_ObjectId8 oid = OMS_ObjectId8(page, offset, generation);

  // Is object existing in local cache?
  OmsObjectContainerPtr pFrame = m_pSession->m_defaultContext->FindObjInContext(&oid);
  if (pFrame == NULL){
    // Is object existing in kernel?
    pFrame = m_pSession->m_defaultContext->GetObjFromLiveCacheBase(oid, OMS_Context::Lock, true);
  }
  if (NULL != pFrame) {
    OMS_ContainerEntry* pContainerInfo = pFrame->GetContainerInfo(__MY_FILE__, __LINE__);
    if (!pContainerInfo->GetClassEntry().IsVarObject()){
      // Oid belongs to a normal object
      pObj = m_pSession->ForUpdPtr(pFrame);
      m_pSession->m_defaultContext->LockObj(oid);
      byteSize = (int)pContainerInfo->GetClassEntry().GetObjectSize();
      varOid.setNil();
      return true;
    }
    else {
      // Oid belongs to a variable sized object
      OmsObjectId o(oid);
      varOid   = *(reinterpret_cast<OmsVarOid*>(&o));
      m_pSession->m_defaultContext->LockObj(varOid);
      pObj     = NULL;
      byteSize = 0;
      return true;
    }
  }
  else {
    // There is no object for given oid
    pObj     = NULL;
    byteSize = 0;
    varOid.setNil();
    return false;
  }
}

/*----------------------------------------------------------------------*/

void OmsHandle::omsProvokedError(bool &exception, bool &badAlloc)
{
#if defined(OMSTST)
  exception = m_pSession->IsTestKnlExceptionActive();
  badAlloc  = m_pSession->IsTestKnlBadAllocActive();
#else
  exception = false;
  badAlloc  = false;
#endif
}

/*----------------------------------------------------------------------*/
// PTS 1133968 
bool OmsHandle::omsCurrVersionIsDropped() const
{
  if (!m_pSession->InVersion())
    m_pSession->ThrowDBError (e_unknown_version, "omsCurrVersionIsDropped: Currently not in version", __MY_FILE__, __LINE__);

  OMS_TRACE (omsTrInterface|omsTrVersion, m_pSession->m_lcSink, "omsCurrVersionIsDropped");

  return m_pSession->CurrentContext()->IsDropped();
}

/*----------------------------------------------------------------------*/

void OmsHandle::omsCheckCancelFlag() const // PTS 1110812
{
  if (m_pSession && m_pSession->IsCancelled()){
    omsThrowCancelledError();
  }
}

/*----------------------------------------------------------------------*/

unsigned int OmsHandle::omsGetObjectRevisionNumber(const OmsObjectId &oid)
{
  OMS_TRACE (omsTrInterface, m_pSession->m_lcSink, "omsGetObjectRevisionNumber : " << oid);

  ALIGN_OID_CONST(oid8, oid);

  return m_pSession->GetObjectRevisionNumber(oid8);
}

/*----------------------------------------------------------------------*/

void OmsHandle::omsReadUnchangedObjectsStart(const OmsVersionId& versionId)
{
  const char *msg = "omsReadUnchangedObjectsStart-1- ";

  OMS_TRACE (omsTrInterface|omsTrVersion, m_pSession->m_lcSink, msg << OMS_CharBuffer(versionId, sizeof(OmsVersionId)));
  OMS_CHECK_EXCEPTION(*this);

  // check that no version is open
  if (m_pSession->InVersion()) {
    OMS_TRACE (omsTrVersion, m_pSession->m_lcSink, " - failed, because version (" << 
               OMS_CharBuffer(m_pSession->m_context->GetVersionId(), sizeof(OmsVersionId)) << ") is still open.");
    m_pSession->ThrowDBError (e_already_in_version, msg, m_pSession->CurrentContext()->GetVersionId(), __MY_FILE__, __LINE__); 
  }

  // check that no subtransaction is active
  if (m_pSession->IsSubtransOpen()) {
    OMS_TRACE (omsTrVersion, m_pSession->m_lcSink, " - failed, because subtrans is open.");
    m_pSession->ThrowDBError (e_OMS_too_many_subtrans, msg, versionId, __MY_FILE__, __LINE__); 
  }

  { // Begin of exclusive critical section.    
    OMS_InternalLockScope scope(m_pSession, OMS_Session::LIBOMS_RWLOCK_AREA, 
      OMS_Globals::m_globalsInstance->m_versionDictionary.GetSingleLockId(versionId), /*exclusive=*/true);
  
    // check that version exists and is not marked as dropped
    OMS_Context *pContext = OMS_Globals::m_globalsInstance->m_versionDictionary.FindVersion(versionId);
    if (pContext == NULL) {
      OMS_TRACE (omsTrVersion, m_pSession->m_lcSink, " - failed, because version was not found.");
      m_pSession->ThrowDBError (e_unknown_version, msg, versionId, __MY_FILE__, __LINE__);
    }
    else if (pContext->IsDropped()) {
      OMS_TRACE (omsTrVersion, m_pSession->m_lcSink, " - failed, because version is marked as dropped.");
      m_pSession->ThrowDBError (e_unknown_version, msg, versionId, __MY_FILE__, __LINE__);
    }
    
    // Check that version is not bound to another session
    if (pContext->IsBoundToTrans() && !m_pSession->VersionBoundByMe(pContext)) 
    {
      tsp00_TaskId tid = pContext->GetSession()->GetTaskId();
      if (pContext->IsVersionOpen()){
        OMS_TRACE (omsTrVersion, m_pSession->m_lcSink, " - failed, because version is already opened by another transaction:" << tid);
        m_pSession->ThrowDBError (e_version_already_open, msg, versionId, __MY_FILE__, __LINE__);
      }
      else {
        OMS_TRACE (omsTrVersion, m_pSession->m_lcSink, " - failed, because version is still bound to another transaction: " << tid);
        m_pSession->ThrowDBError (e_version_bound_by_trans, msg, versionId, __MY_FILE__, __LINE__);
      }
    }

    m_pSession->OpenVersion(pContext, false);
    m_pSession->IncOpenVersion();
    OMS_Globals::m_globalsInstance->m_versionDictionary.MarkNotUnloadable(m_pSession->m_lcSink, pContext);
  } // End of exclusive critical section.

  m_pSession->ReadUnchangedObjectsStart();

  OMS_TRACE (omsTrVersion, m_pSession->m_lcSink, " - successfully finished.");
}

/*----------------------------------------------------------------------*/

void OmsHandle::omsReadUnchangedObjectsStart()
{
  const char *msg = "omsReadUnchangedObjectsStart-2- ";

  OMS_TRACE (omsTrInterface, m_pSession->m_lcSink, msg);
  OMS_CHECK_EXCEPTION(*this);

  // check that no version is open
  if (m_pSession->InVersion()) {
    OMS_TRACE (omsTrVersion, m_pSession->m_lcSink, " - failed, because version (" << 
               OMS_CharBuffer(m_pSession->m_context->GetVersionId(), sizeof(OmsVersionId)) << ") is still open.");
    m_pSession->ThrowDBError (e_already_in_version, msg, m_pSession->CurrentContext()->GetVersionId(), __MY_FILE__, __LINE__); 
  }

  // check that no subtransaction is active
  if (m_pSession->IsSubtransOpen()) {
    OMS_TRACE (omsTrVersion, m_pSession->m_lcSink, " - failed, because subtrans is open.");
    m_pSession->ThrowDBError (e_OMS_too_many_subtrans, msg, __MY_FILE__, __LINE__); 
  }

  m_pSession->ReadUnchangedObjectsStart();

  OMS_TRACE (omsTrVersion, m_pSession->m_lcSink, " - successfully finished.");
}

/*----------------------------------------------------------------------*/

void OmsHandle::omsReadUnchangedObjectsEnd()
{
  const char *msg = "omsReadUnchangedObjectsEnd ";

  OMS_TRACE (m_pSession->InVersion()?(omsTrInterface|omsTrVersion):omsTrInterface, m_pSession->m_lcSink, msg);
  OMS_CHECK_EXCEPTION(*this);

  m_pSession->ReadUnchangedObjectsEnd();

  if (m_pSession->InVersion()){
    { // Begin of exclusive critical section.    
      OMS_InternalLockScope scope(m_pSession, OMS_Session::LIBOMS_RWLOCK_AREA, 
        OMS_Globals::m_globalsInstance->m_versionDictionary.GetSingleLockId(m_pSession->m_context->GetVersionId()), /*exclusive=*/true);
    
      // check that version exists and is not marked as dropped
      OMS_Context *pContext = OMS_Globals::m_globalsInstance->m_versionDictionary.FindVersion(m_pSession->m_context->GetVersionId());
      if (pContext == NULL) {
        OMS_TRACE (omsTrVersion, m_pSession->m_lcSink, " - failed, because version was not found.");
        m_pSession->ThrowDBError (e_unknown_version, msg, m_pSession->m_context->GetVersionId(), __MY_FILE__, __LINE__);
      }

      OMS_Globals::m_globalsInstance->m_versionDictionary.MarkNotUnloadable(m_pSession->m_lcSink, pContext);
      m_pSession->CloseVersion();
    } // End of exclusive critical section
  }

  OMS_TRACE (omsTrVersion, m_pSession->m_lcSink, " - successfully finished.");
}

/*----------------------------------------------------------------------*/

// Class is only needed because the ADT OMS_DynamicStack uses the assignment
// operator to copy the data into the stack. As OmsVersionId is only a struct
// a new class with assignment operator is needed.
class OMS_VersionId 
{
private:
  OmsVersionId vId;

public:
  OMS_VersionId(const OmsVersionId &id)    { memcpy(&vId[0], &id[0], sizeof(OmsVersionId)); }
  void operator= (const OmsVersionId &id)  { memcpy(&vId[0], &id[0], sizeof(OmsVersionId)); } 
  const OmsVersionId& GetId() const        { return vId; }
};

void OmsHandle::omsCleanUpAfterInitialization()
{
  ///////////////////////////////////////////////////////////////////////
  // Cleanup global version directory
  {
    OMS_DynamicStack<OMS_VersionId,OMS_Session> stack;
    stack.Advise(m_pSession, 100);

    if (m_pSession->InVersion()){
      omsCloseVersion(*omsCurrentVersionId());
    }

    // Critical section is used to synchronize the access to the global iterator
    // structure. It is NOT used to synchronize access to the version dictionary.
    OMS_SinkCriticalSection cs(m_pSession->m_lcSink, RGN_VERSION_DIR);
    cs.Enter();

    // Reset the global iterator
    OMS_Globals::m_globalsInstance->m_versionIter.Reset(OMS_LOCK_EXCLUSIVE);

    while (true){
      // Position the iterator on the next slot in the hash of the version directory.
      OMS_Context *pContext = OMS_Globals::m_globalsInstance->m_versionIter.GetFirstInSlot();

      if (!pContext)
        break;

      while (pContext){
        OMS_Context *pNext = OMS_Globals::m_globalsInstance->m_versionIter.GetNextInSlot();
        OMS_VersionId vId(pContext->GetVersionId());
        stack.Push(vId);
        pContext = pNext;
      }
      OMS_Globals::m_globalsInstance->m_versionIter.ReleaseSlot();

      // Deletion is done is a separate loop, as during the iteration a lock on the hash chain
      // is hold which would collide with the lock which is acquired in omsDropVersion
      while (!stack.IsEmpty()){
        omsDropVersion(stack.Pop()->GetId());
      }
    }
  }

  ///////////////////////////////////////////////////////////////////////
  // Reset statistics
  m_pSession->m_lcSink->ResetMonitor();
}


/*----------------------------------------------------------------------*/

bool OmsHandle::CheckSession()
{
  return m_pSession ? m_pSession->CheckCorruptFlag() : false;
}

/*----------------------------------------------------------------------*/

void OmsHandle::SetSessionCorrupt()
{
  if (m_pSession){
    m_pSession->SetCorruptFlag();
  }
}

/*------------------------------------------------------------------------*/

void OmsHandle::omsStore(const OmsVarOid& oid)
{
  const char* msg = "OmsHandle::omsStore: "; 
  OMS_TRACE(omsTrInterface, m_pSession->m_lcSink, msg << oid);
  OMS_CHECK_EXCEPTION(*this);

  m_pSession->AssertNotReadOnly(msg);  

  m_pSession->IncStore();

  ALIGN_OID_CONST(oid8, oid);

  OmsObjectContainer *pObj = m_pSession->m_context->FindVarObjInContext(oid8, VarObjNoLock, 0, NULL);
  if (pObj == NULL){
    m_pSession->ThrowDBError(e_object_not_found, msg, oid8, __MY_FILE__, __LINE__);
  }

  OMS_VarObjInfo *pObjInfo = reinterpret_cast<OMS_VarObjInfo*>(&pObj->m_pobj); 

  // Check pattern to recognize writing behind the object.
  size_t sz = ((pObjInfo->m_vobjSize + 3) & ~3) + sizeof(tsp00_Uint4);
  if (reinterpret_cast<tsp00_Uint4*>(pObjInfo->m_pvobj+sz)[-1] != OMS_VAROBJ_CHECK_PATTERN){
    m_pSession->ThrowDBError(e_var_obj_overwritten, msg, oid8, __MY_FILE__, __LINE__);
  }

  // Check that container is not dropped already 
  OMS_ContainerEntry *pContainerInfo = pObj->GetContainerInfoNoCheck();
  if (pContainerInfo->IsDropped()){
    m_pSession->ThrowDBError(e_container_dropped, msg, oid8, __MY_FILE__, __LINE__);
  }

  if (!m_pSession->IsLocked(pObj, /*shared=*/false)){
    m_pSession->ThrowDBError (e_object_not_locked, msg, oid8, __MY_FILE__, __LINE__);
  }

  if (!pObj->existBeforeImage (m_pSession->CurrentSubtransLevel(), m_pSession->InVersion())) {
    m_pSession->ThrowDBError (e_missing_before_image, msg, oid8, __MY_FILE__, __LINE__);
  }

  pObj->MarkStored();
  pObjInfo->unlock();
}

/*------------------------------------------------------------------------*/

void* OmsHandle::omsDeRefForUpd_ConstSize(const OmsVarOid &oid, unsigned long &size)
{
  const char* msg = "OmsHandle::omsDeRefForUpd_ConstSize: "; 
  OMS_TRACE(omsTrInterface, m_pSession->m_lcSink, msg << oid);
  OMS_CHECK_EXCEPTION(*this);

  m_pSession->AssertNotReadOnly(msg);  

  omsIfNilOidThrowError (this, oid, msg, __MY_FILE__, __LINE__);

  ALIGN_OID_CONST(oid8, oid);

  return m_pSession->DeRefVarObjForUpd(oid8, size);
}


/*------------------------------------------------------------------------*/

void OmsHandle::omsRemoveCorruptObjects(
  bool             checkOnly, 
  const ClassID   &guid, 
  OmsSchemaHandle  schema, 
  OmsContainerNo   containerNo)
{
  const char* msg = "OmsHandle::omsRemoveCorruptObjects: "; 
  OMS_TRACE(omsTrInterface, m_pSession->m_lcSink, msg);
  OMS_CHECK_EXCEPTION(*this);

  m_pSession->AssertNotReadOnly(msg);

  m_pSession->m_defaultContext->RemoveCorruptVarObjects(checkOnly, guid, schema, containerNo);
}

/*------------------------------------------------------------------------*/

