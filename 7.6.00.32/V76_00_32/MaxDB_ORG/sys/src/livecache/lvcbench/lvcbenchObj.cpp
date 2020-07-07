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

/****************************************************************************/
/* lvcbenchObj.cpp : Implementation of Clvcbench                            */
/****************************************************************************/

#include "lcbasetypes.h"    /* no check */
#include "lvcbench.h"       /* no check */
#include "Oms/OMS_MassDerefIterators.hpp" // PTS 1120717
#define RESOURCE_FILE
#include "gsp100.h" // nocheck

#if defined(_IN_MSDEV)
#include "lvcbenchObj.h"       /* no check */
#include "lctestversioniter.h" /* no check */
#else
#include "livecache/lvcbench/lvcbenchObj.h"
//#include "livecache/lvcbench/lvcbenchDry.hpp"
#include "livecache/lctest/lctestversioniter.h"
#endif

#ifndef _WIN32
#include <time.h>
#include <sys/times.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <new.h>
#else
#include <new>
#endif

#define MAX_TASK 300

//#define TRACE_METHODS 1

#define TASK_INFO_KEY 999999999

#if defined(TRACE_METHODS)
#define TRACE_METHOD(X) { dbpTrace(#X); dbpOpMsg("%s task %d", #X, m_taskId); }
#else
#define TRACE_METHOD(X)
#endif

#if defined(HPUX)
#define BAD_ALLOC bad_alloc
#else
#define BAD_ALLOC std::bad_alloc
#endif

#define CATCH_ALL catch (DbpError& e)\
              {\
              this->ExceptionHandler (e);\
              }\
              catch (BAD_ALLOC)\
              {\
              DbpError e(-28000);\
              this->ExceptionHandler (e);\
              }\

  
//typedef OmsTypeInt64 TTimeStamp;

struct OneCallInfo 
{
    OmsTypeInt8 m_startTime;
    OmsTypeInt8 m_endTime;
    int         m_opCount;
    int         m_callCnt;
    int         m_errorCode;
};

static volatile bool m_stop           = false;
static volatile int  m_maxTaskId      = 0; 
static volatile int  m_listLength     = 0;
static volatile int  m_deleteInterval = 0;

static OneCallInfo glob_callInfo[MAX_TASK];

struct OneTaskInfo
{
    bool m_inSession;
    int  m_error;
    OneTaskInfo()
        : m_inSession(false)
        , m_error(0)
    {
    }
    int GetError() const
    {
        return m_error;
    }
    bool InSession() const
    {
        return m_inSession;
    }
    void SetInSession(bool inSession)
    {
        m_inSession = inSession;
    }
    void SetError(int error)
    {
        m_error = error;
    }
};

static OneTaskInfo glob_taskInfo[MAX_TASK];

static OmsSchemaHandle globSchemaHandle;
static OmsContainerNo  globContainerNo = 0;


const char* lvcBenchSchema = "LVC_BENCH";

/*
overwriting of operator new / delete
needed on WIN only
*/

#if defined(WIN32)
#include <new>

#pragma warning( disable : 4290 )

void* operator new(size_t sz) throw(std::bad_alloc)
{
    return omsMalloc(sz);
}

void* operator new[] (size_t sz) throw(std::bad_alloc)
{
    return omsMalloc(sz);
}

void operator delete(void* p) throw()
{
    omsFree(p);
}

void operator delete[] (void* p) throw()
{
    omsFree(p);
}
#endif

/*
Random number generator, used instead of rand() to be able to
create same number sequences on all platforms
*/

class Random 
{
public :
    Random(int start = 1)
        : m_random(start)
    {
    }
    
    int Next()
    {
        m_random = (((m_random * (int) 214013 + (int) 2531011) >> 16) & 0x7fff);
        return m_random;
    }
    
    int Next(int max)
    {
        if (0 == max)
        {
            return 0;
        }
        m_random = (((m_random * (int) 214013 + (int) 2531011) >> 16) & 0x7fff);
        return (m_random % max);
    }
private :
    int m_random;
};

static Random Random_glob;

class CallInfo
{
public :
    CallInfo(int taskId)
        : m_taskId   (taskId)
        , m_opCount  (0)
    {
        glob_callInfo[m_taskId].m_startTime = DbpBase::dbpMicroTime();
    }
    CallInfo(int taskId, int opCount)
        : m_taskId   (taskId)
        , m_opCount  (opCount)
    {
        glob_callInfo[m_taskId].m_startTime = DbpBase::dbpMicroTime();
    }
    ~CallInfo()
    {
        glob_callInfo[m_taskId].m_endTime   = DbpBase::dbpMicroTime();
        glob_callInfo[m_taskId].m_opCount   = m_opCount;
        glob_callInfo[m_taskId].m_errorCode = 0;
    }
    void IncOpCount()
    {
        ++m_opCount;
    }
private:
    int  m_taskId;
    int  m_opCount;
};



/****************************************************************************/
/* Register is the very first Method called after the creation of the       */
/* Component-Object.                                                        */
/* (within the "constructor" of the Component-Object no communication with  */
/*  the liveCache has been activated yet!)                                  */

class CallBack : public OmsCallbackInterface
{
public :
    CallBack(Clvcbench& Bench)
        : m_bench(Bench)
    {
    }
    virtual void omsValidate (OmsHandle& h, Event e, bool inVersion)
    {
      glob_callInfo[m_bench.m_taskId].m_startTime = DbpBase::dbpMicroTime();
    }
    virtual void omsInvalidate (OmsHandle& h, Event e, bool inVersion)
    {
        if (OmsCallbackInterface::AfterCommit == e)
        {
            glob_callInfo[m_bench.m_taskId].m_endTime = DbpBase::dbpMicroTime();
        }
    }
    virtual void omsMethodCallEpilog(OmsHandle& h)
    {
        if (m_bench.m_profiling)
        {
            h.omsGetMethodCallMonitorInfo(m_bench.m_callStatistics);
        }
        m_bench.m_profiling = true;
    }
    virtual void omsMethodCallEpilog( OmsHandle& h, SqlHandle& s)     /* PTS 1120794 */
    {
      omsMethodCallEpilog(h);
    }
    virtual void omsDestroySelf()
    {
        // PTS 1120717
        OmsAllocatorInterface& allocator = m_bench.omsGetSessionAllocator();
        if (m_bench.m_pOid != NULL) {
            allocator.omsDeallocate(m_bench.m_pOid);
            m_bench.m_pOid = NULL;
        }
        if (m_bench.m_ppObj != NULL) {
            allocator.omsDeallocate(m_bench.m_ppObj);
            m_bench.m_ppObj = NULL;
        }
        // PTS 1122194
        if (m_bench.m_ppKey != NULL) {
            allocator.omsDeallocate(m_bench.m_ppKey[0]);
            allocator.omsDeallocate(m_bench.m_ppKey);
            m_bench.m_ppKey = NULL;
        }
        if (m_bench.m_ppKeyObj != NULL) {
            allocator.omsDeallocate(m_bench.m_ppKeyObj);
            m_bench.m_ppKeyObj = NULL;
        }
    }
    virtual void omsMemLeftOver(const void *blk, size_t size, BlockType blockType)
    {
    }
    
    virtual void omsMemInvalidFree(const void *blk, size_t size, BlockType blockType, BlockType freeType)
    {
    }
private :
    //TTimeStamp m_startTime;
    Clvcbench& m_bench;
};

CVarObjects::CVarObjects(OmsHandle& h, int noOfVarObjects)
: m_handle(h)
, m_noOfVarObjects(noOfVarObjects)
, m_varOids(0)
{
    m_varOids = (OmsVarOid*) omsMalloc(sizeof(OmsVarOid) * noOfVarObjects);
}

CVarObjects::~CVarObjects()
{
    for (int ix = 0; ix < m_noOfVarObjects; ++ix)
    {
        m_handle.omsLock(m_varOids[ix]);
        m_handle.omsDelete(m_varOids[ix]);
    }
    omsFree(m_varOids);
}


/*-----------------------------------------------------------------------------------------*/

void Clvcbench::ExceptionHandler(DbpError& e)
{
    TRACE_METHOD(ExceptionHandler)
        dbpTrace ("error %d : %s", e.dbpError(), e.dbpErrorText());
    glob_taskInfo[m_taskId].SetError(e.dbpError());
    omsExceptionHandler(e);
}

/*-----------------------------------------------------------------------------------------*/

STDMETHODIMP Clvcbench::Register()
{
  TRACE_METHOD(Register)
  HRESULT hr	= S_OK;
  try {
    co90CheckVersion ((IUnknown**)&this->m_pIUnknown, 
    ILIVECACHESINK_VERSION, 
    LIVECACHETYPES_VERSION, LIVECACHEOMS_VERSION, 
    LIVECACHESQL_VERSION );
    sqlSetHandler(NULL);

    for (int ix = 0; ix < MAX_TASK; ++ix)
    {
      glob_callInfo[ix].m_callCnt = 0;
    }
  }
  CATCH_ALL

  m_pTransactionInterface = new CallBack(*this);   // PTS 1128285

  INIT_AFTER_EOT();   // PTS 1128285

  OMS_REGISTER_CLASS    (CKeyedObject, *this);
  OMS_REGISTER_CLASS    (CObject,      *this);
  OMS_REGISTER_VARCLASS (OMS_VAR_OBJ_GUID, "VarObj", *this);

  OMS_REGISTER_CLASS    (OmsArrayObjectType, *this);

  return hr;
}

/*-----------------------------------------------------------------------------------------*/

Clvcbench::~Clvcbench()
{
  glob_taskInfo[m_taskId].SetInSession(false);
}

/****************************************************************************/
/* Ilvcbench method implementation  (to be inserted here)                   */

//STDMETHODIMP Clvcbench::CALC_DRYSTONES(OmsTypeInt4* dryStones)
//{
//    TRACE_METHOD(CALC_DRYSTONES)
//        *dryStones = CalcDryStones();
//    return S_OK;
//}

/*-----------------------------------------------------------------------------------------*/

STDMETHODIMP Clvcbench::INIT_AFTER_EOT()      // PTS 1128285
{
  TRACE_METHOD(INIT_AFTER_EOT)

  try {
    omsRegTransactionInterface(CObject::omsGetGuid(), m_pTransactionInterface);
    DbpError::dbpSetCallback(NULL);

    omsReadOnly(false);  
  }
  catch (DbpError&)
  {}

  return S_OK;
}
/*-----------------------------------------------------------------------------------------*/

STDMETHODIMP Clvcbench::CALL_DISPATCHER(OmsTypeInt4 count)
{
    TRACE_METHOD(CALL_DISPATCHER)
        try
    {
        CallInfo callInfo(m_taskId, count);
        for (OmsTypeInt4 ix = 0; ix < count; ++ix)
        {
            omsReschedule();
        }
    }
    CATCH_ALL
        
        return S_OK;
}

/*-----------------------------------------------------------------------------------------*/

STDMETHODIMP Clvcbench::CLEAN_UP()
{
    TRACE_METHOD(CLEAN_UP)
 
    try {
        OmsSchemaHandle schemaHandle = omsGetSchemaHandle(lvcBenchSchema);
        omsDropSchema(schemaHandle);
        this->DropTableOrdkey();
        this->WaitForGarbageCollector();  
    }
    catch (DbpError&)
    {}
    
    try {
        globSchemaHandle = omsCreateSchema(lvcBenchSchema);
    }
    CATCH_ALL
        
    return S_OK;
}

/*-----------------------------------------------------------------------------------------*/

STDMETHODIMP Clvcbench::COUNT_TASKS(OmsTypeInt4* cntTasks)
{
    TRACE_METHOD(COUNT_TASKS)
        *cntTasks = 0;
    for (int ix = 0; ix < MAX_TASK; ++ix)
    {
        if (glob_taskInfo[ix].InSession())
        {
            ++(*cntTasks);
        }
    }
    return S_OK;
}

/*-----------------------------------------------------------------------------------------*/

STDMETHODIMP Clvcbench::CREATE_CONTAINER(int containerType, int containerNo, int partitioned)
{
    TRACE_METHOD(CREATE_CONTAINER)
        try
    {
        
        globContainerNo = containerNo;
        if (containerType == 1)  // KeyContainer
        {
          omsCreateContainer(CKeyedObject::omsGetGuid(), globSchemaHandle, containerNo,
            false,             //useCachedKeys
            partitioned != 0); //partitionedKeys

        }
        else if (containerType == 2) // VarContainer
        {
          omsCreateContainer(OMS_VAR_OBJ_GUID, globSchemaHandle, containerNo); 
        }
        else if (containerType == 3) // ArrayObjectContainer
        {
          omsCreateContainer(OmsArrayObjectType::omsGetGuid(), globSchemaHandle, containerNo);
        }
        else // normale Container
        {
          omsCreateContainer(CObject::omsGetGuid(), globSchemaHandle, containerNo);
        }
        
    }
    CATCH_ALL
        return S_OK;
}

/*-----------------------------------------------------------------------------------------*/

STDMETHODIMP Clvcbench::CREATE_OBJECTS(OmsTypeInt4 cntObjects, OmsTypeInt4 *milliSecs)
{
    TRACE_METHOD(CREATE_OBJECTS)
        try
    {
        CallInfo callInfo(m_taskId, cntObjects);
        for (OmsTypeInt4 ix = 0; ix < cntObjects; ++ix)
        {
            CObject* p = new(*this, globSchemaHandle, globContainerNo) CObject(m_taskId);
        }
    }
    CATCH_ALL
        
    return S_OK;
}

/*-----------------------------------------------------------------------------------------*/

STDMETHODIMP Clvcbench::CREATE_KEYED_OBJECTS(OmsTypeInt4 cntObjects, OmsTypeInt4 offset)
{
    try
    {
        CallInfo callInfo(m_taskId, cntObjects);
        OmsTypeInt4 ix;
        for (ix = 0; ix < cntObjects; ++ix)
        {
            CKey key(m_taskId, ix+offset);
            CKeyedObject* p = new(*this, key, globSchemaHandle, globContainerNo) CKeyedObject(ix+offset, m_taskId);
        }
    }
    CATCH_ALL
        return S_OK;
}

/*-----------------------------------------------------------------------------------------*/

STDMETHODIMP Clvcbench::CLOSE_VERSION(OmsTypeInt4 vId)
{
    TRACE_METHOD(CLOSE_VERSION)
        try
    {
        OmsVersionId versionId;
        memset(&versionId[0], 'X', sizeof(versionId));
        sprintf ((char*) &versionId[0], "LVC_BENCH_V%010d", vId); 
        CallInfo callInfo(m_taskId, 1);
        //long startTime = (long) GET_MILLI_SECS;
        omsCloseVersion(versionId);
        //m_runTime = (long) GET_MILLI_SECS - startTime;
    }
    CATCH_ALL
        
        return S_OK;
}

/*-----------------------------------------------------------------------------------------*/

STDMETHODIMP Clvcbench::CREATE_VAROBJECTS(OmsTypeInt4 noOfObjects, OmsTypeInt4 size)
{
    TRACE_METHOD(CREATE_VAROBJECTS)
        try
    {
        int length = sizeof(OmsVarOid) * noOfObjects;
        m_varObjAnchor   = new CVarObjects(*this, noOfObjects); 
        void* pVarObject = omsMalloc(size);
        CallInfo callInfo(m_taskId, noOfObjects);
        for (int ix = 0; ix < noOfObjects; ++ix)
        {
            (*m_varObjAnchor)[ix] = omsNewVarObject(globSchemaHandle, 1);
            omsStoreVarObject((*m_varObjAnchor)[ix], pVarObject, size);
        }
    }
    CATCH_ALL
        
        return S_OK;
}

/*-----------------------------------------------------------------------------------------*/

STDMETHODIMP Clvcbench::CREATE_VERSION(OmsTypeInt4 vId)
{
    TRACE_METHOD(CREATE_VERSION)
        try
    {
        OmsVersionId versionId;
        memset(&versionId[0], 'X', sizeof(versionId));
        sprintf ((char*) &versionId[0], "LVC_BENCH_V%010d", vId); 
        CallInfo callInfo(m_taskId, 1);
        omsCreateVersion(versionId);
    }
    CATCH_ALL
        
        return S_OK;
}

/*-----------------------------------------------------------------------------------------*/

STDMETHODIMP Clvcbench::DROP_CONTAINER(int containerType, int containerNo)
{
    TRACE_METHOD(DROP_CONTAINER)
        try
    {
        if (containerType == 1)
        {
            omsDropContainer (CKeyedObject::omsGetGuid(), globSchemaHandle, containerNo);
        }
        else if (containerType == 2)
        {
            omsDropContainer (OMS_VAR_OBJ_GUID, globSchemaHandle, containerNo);
        }
        else 
        {
            omsDropContainer (CObject::omsGetGuid(), globSchemaHandle, containerNo);
        }

        SQL delObjStorage = sql("DELETE OBJECT STORAGE");
        delObjStorage.sqlExecute();
    }
    CATCH_ALL
        return S_OK;
}

/*-----------------------------------------------------------------------------------------*/

STDMETHODIMP Clvcbench::ITER(OmsTypeInt4 maxNumber, OmsTypeInt4 noOfIter)
{
    TRACE_METHOD(ITER)
        try
    {
        CallInfo callInfo(m_taskId);
        for (int j=0; j<noOfIter; ++j){
        int i = 0;
        for (OmsObjByClsIter<CObject> iter = CObject::omsAllOids(*this, globSchemaHandle, globContainerNo); iter; ++iter) 
        {
            const CObject* pObj = iter.omsDeRef();
            callInfo.IncOpCount();
            if (++i == maxNumber){
              break;
            }
        }
        }
    }
    CATCH_ALL
        
        return S_OK;
}

/*-----------------------------------------------------------------------------------------*/

STDMETHODIMP Clvcbench::GET_ERROR(int taskId, OmsTypeInt4* error)
{
    TRACE_METHOD(GET_ERROR)
        *error = glob_taskInfo[taskId].GetError();
    return S_OK;
}

/*-----------------------------------------------------------------------------------------*/

STDMETHODIMP Clvcbench::ITER_FORUPD()
{
    TRACE_METHOD(ITER_FORUPD)
        try
    {
        CallInfo callInfo(m_taskId);
        for (OmsObjByClsIter<CObject> iter = CObject::omsAllOids(*this, globSchemaHandle, globContainerNo); iter; ++iter) 
        {
            CObject* pObj = iter.omsDeRefForUpd(true);
            callInfo.IncOpCount();
        }
    }
    CATCH_ALL
        
        return S_OK;
}

/*-----------------------------------------------------------------------------------------*/

STDMETHODIMP Clvcbench::SET_TASK_ID(int taskId)
{
    TRACE_METHOD(SET_TASK_ID)
    m_taskId = taskId;
    glob_taskInfo[taskId].SetInSession(true);
    return S_OK;
}

/*-----------------------------------------------------------------------------------------*/

STDMETHODIMP Clvcbench::MAKE_LIST()
{
    TRACE_METHOD(MAKE_LIST)
        try
    {
        CObject* prev = 0;
        for (OmsObjByClsIter<CObject> iter = CObject::omsAllOids(*this, globSchemaHandle, globContainerNo); iter; ++iter) 
        {
            CObject* pObj = iter.omsDeRefForUpd(true);
            pObj->m_next = OmsOid<CObject>();
            pObj->omsStore(*this);
            if (0 == prev)
            {
                m_listHead = pObj->omsGetOid();
            }
            else
            {
                prev->m_next = pObj->omsGetOid();
            }
            prev = pObj;
            
        }
    }
    CATCH_ALL
        
        return S_OK;
}

/*-----------------------------------------------------------------------------------------*/

STDMETHODIMP Clvcbench::OPEN_VERSION(OmsTypeInt4 vId)
{
    TRACE_METHOD(OPEN_VERSION)
        try
    {
        OmsVersionId versionId;
        memset(&versionId[0], 'X', sizeof(versionId));
        sprintf ((char*) &versionId[0], "LVC_BENCH_V%010d", vId); 
        CallInfo callInfo(m_taskId, 1);
        //long startTime = (long) GET_MILLI_SECS;
        omsOpenVersion(versionId);
        //m_runTime = (long) GET_MILLI_SECS - startTime;
    }
    CATCH_ALL
        
        return S_OK;
}

/*-----------------------------------------------------------------------------------------*/

STDMETHODIMP Clvcbench::RESET_VERSION(OmsTypeInt4 vId)
{
    TRACE_METHOD(OPEN_VERSION)
        try
    {
        OmsVersionId versionId;
        memset(&versionId[0], 'X', sizeof(versionId));
        sprintf ((char*) &versionId[0], "LVC_BENCH_V%010d", vId); 
        CallInfo callInfo(m_taskId, 1);
        //long startTime = (long) GET_MILLI_SECS;
        omsResetVersion(versionId);
        //m_runTime = (long) GET_MILLI_SECS - startTime;
    }
    CATCH_ALL
        
        return S_OK;
}

/*-----------------------------------------------------------------------------------------*/

STDMETHODIMP Clvcbench::DROP_VERSION(OmsTypeInt4 vId)
{
    TRACE_METHOD(DROP_VERSION)
        try
    {
        OmsVersionId versionId;
        memset(&versionId[0], 'X', sizeof(versionId));
        sprintf ((char*) &versionId[0], "LVC_BENCH_V%010d", vId); 
        CallInfo callInfo(m_taskId, 1);
        //long startTime = (long) GET_MILLI_SECS;
        omsDropVersion(versionId);
        //m_runTime = (long) GET_MILLI_SECS - startTime;
    }
    CATCH_ALL
        
        return S_OK;
}


/*-----------------------------------------------------------------------------------------*/

STDMETHODIMP Clvcbench::DELETE_VAROBJECTS()
{
    TRACE_METHOD(DELETE_VAROBJECTS)
        try
    {
        CallInfo callInfo(m_taskId, m_varObjAnchor->Size());
        delete m_varObjAnchor;
        m_varObjAnchor = NULL;
    }
    CATCH_ALL
        return S_OK;
}

/*-----------------------------------------------------------------------------------------*/

STDMETHODIMP Clvcbench::DEREF()
{
    TRACE_METHOD(DEREF)
    try
    {
      CallInfo callInfo(m_taskId);
      const CObject* p = m_listHead(*this);
      while (p)
      {
          callInfo.IncOpCount();
          p = p->m_next(*this);
      }
    }
    CATCH_ALL
        return S_OK;
}

/*-----------------------------------------------------------------------------------------*/

STDMETHODIMP Clvcbench::FILL_ARRAY(OmsTypeInt4 cntObjects)   // PTS 1120717
{
  TRACE_METHOD(FILL_ARRAY)  
  try {
    OmsAllocatorInterface& allocator = omsGetSessionAllocator();

    if (m_pOid == NULL){
      m_pOid = (OmsOid<CObject>*) allocator.omsAllocate(cntObjects * sizeof(OmsOid<CObject>));
    }
    if (m_ppObj == NULL) {
      m_ppObj = (const CObject**) allocator.omsAllocate(cntObjects * sizeof(void*));
    }

    CallInfo callInfo(m_taskId);
    const CObject* p = m_listHead(*this); 
    int i = 0;
    while (p){ 
      m_pOid[i++] = p->omsGetOid();
      p = p->m_next(*this);
      callInfo.IncOpCount();
    }
  }
  CATCH_ALL
    return S_OK;
}

/*-----------------------------------------------------------------------------------------*/

STDMETHODIMP Clvcbench::MASS_DEREF(OmsTypeInt4 cntObjects, OmsTypeInt4 packetSize)   // PTS 1120717 
{
  TRACE_METHOD(MASS_DEREF) 

  try {
    CallInfo callInfo(m_taskId, cntObjects);
    if (m_pOid != NULL && m_ppObj != NULL){
      int i;
      for (i=0; i<cntObjects-packetSize; i+=packetSize){
        omsDerefMass(OmsDerefArray(&(m_pOid[i]), &(m_ppObj[i]), packetSize));
      }
      if (i < cntObjects){
        omsDerefMass(OmsDerefArray(&(m_pOid[i]), &(m_ppObj[i]), cntObjects-i));
      }
    }
  }
  CATCH_ALL
    return S_OK;
}

/*-----------------------------------------------------------------------------------------*/
// PTS 1122194
STDMETHODIMP Clvcbench::FILL_ARRAY_KEY(OmsTypeInt4 cntObjects, OmsTypeInt4 startNumber)   
{
  TRACE_METHOD(FILL_ARRAY_KEY)  
  try {
    OmsAllocatorInterface& allocator = omsGetSessionAllocator();
     
    CKey* pKey = (CKey*) allocator.omsAllocate(cntObjects * sizeof(CKey));  // PTS 1123525

    if (m_ppKey == NULL){
      m_ppKey = (CKey**) allocator.omsAllocate(cntObjects * sizeof(void*));
    }
    if (m_ppKeyObj == NULL) {
      m_ppKeyObj = (const CKeyedObject**) allocator.omsAllocate(cntObjects * sizeof(void*));
    }

    CallInfo callInfo(m_taskId);

    for (int i=0; i<cntObjects; ++i){
       CKey key(m_taskId, i+startNumber);
       m_ppKey[i] = &pKey[i];
       memcpy (m_ppKey[i], &key, sizeof(CKey));
    }
  }
  CATCH_ALL
    return S_OK;
}

/*-----------------------------------------------------------------------------------------*/
// PTS 1122194 
STDMETHODIMP Clvcbench::MASS_DEREF_KEY(OmsTypeInt4 cntObjects, OmsTypeInt4 packetSize)   
{
  TRACE_METHOD(MASS_DEREF_KEY) 

  try {
    CallInfo callInfo(m_taskId, cntObjects);
    if (m_ppKey != NULL && m_ppKeyObj != NULL){
      int i;
      for (i=0; i<cntObjects-packetSize; i+=packetSize){
        omsDerefKeyMass(OmsDerefKeyArray((unsigned char**)&(m_ppKey[i]), globSchemaHandle, globContainerNo, 
                                         &(m_ppKeyObj[i]), packetSize));
      }
      if (i < cntObjects){
        omsDerefKeyMass(OmsDerefKeyArray((unsigned char**)&(m_ppKey[i]), globSchemaHandle, globContainerNo, 
                                         &(m_ppKeyObj[i]), cntObjects-i));
      }
    }
  }
  CATCH_ALL
    return S_OK;
}

/*-----------------------------------------------------------------------------------------*/


STDMETHODIMP Clvcbench::DEREFFORUPD()
{
    TRACE_METHOD(DEREFFORUPD)
    try
    {
        CallInfo callInfo(m_taskId);
        CObject* p = m_listHead.omsDeRefForUpd(*this, true);
        while (p)
        {
            callInfo.IncOpCount();
            p = p->m_next.omsDeRefForUpd(*this, true);
        }
    }
    CATCH_ALL
        return S_OK; 
}

/*-----------------------------------------------------------------------------------------*/

STDMETHODIMP Clvcbench::DEREF_KEYED(OmsTypeInt4 cntObjects, OmsTypeInt4 startNumber)
{
    TRACE_METHOD(DEREF_KEYED)
        
        try
    {
        CallInfo callInfo(m_taskId, cntObjects);
        const CKeyedObject* p;
        for (OmsTypeInt4 ix = 0; ix < cntObjects; ++ix)
        {
            CKey key(m_taskId, ix+startNumber);
            p = CKeyedObject::omsKeyAccess(*this, key, globSchemaHandle, globContainerNo);
        }
    }
    CATCH_ALL       
        return S_OK;
}

/*-----------------------------------------------------------------------------------------*/

// PTS 1119234
STDMETHODIMP Clvcbench::CACHED_KEYS_ON(){
  TRACE_METHOD(CACHED_KEYS_ON)
  try{
    CallInfo callInfo(m_taskId, 1);
    omsSetCachedKeys(true, CKeyedObject::omsGetGuid(), globSchemaHandle, globContainerNo);
  }
  CATCH_ALL{      
   return S_OK;
  }
}

/*-----------------------------------------------------------------------------------------*/

// PTS 1119234
STDMETHODIMP Clvcbench::CACHED_KEYS_OFF(){
  TRACE_METHOD(CACHED_KEYS_OFF)
  try{
    CallInfo callInfo(m_taskId, 1);
    omsSetCachedKeys(false, CKeyedObject::omsGetGuid(), globSchemaHandle, globContainerNo);
  }
  CATCH_ALL{      
   return S_OK;
  }
}

/*-----------------------------------------------------------------------------------------*/

STDMETHODIMP Clvcbench::DEREF_VAROBJECTS()
{
    TRACE_METHOD(DEREF_VAROBJECTS)
        try
    {
        char buffer[20000];
        CallInfo callInfo(m_taskId, m_varObjAnchor->Size());
        for (int ix = 0; ix < m_varObjAnchor->Size(); ++ix)
        {
            omsLoadVarObject((*m_varObjAnchor)[ix], sizeof(buffer), &buffer[0]);
            omsReleaseVarObject((*m_varObjAnchor)[ix]);
        }
    }
    CATCH_ALL
        
        return S_OK;
}

/*-----------------------------------------------------------------------------------------*/

STDMETHODIMP Clvcbench::GET_RUN_TIME(OmsTypeInt4 *runTime)
{
    TRACE_METHOD(GET_RUN_TIME)
        m_profiling = false;
    *runTime    = 0;
    
    return S_OK;
}

/*-----------------------------------------------------------------------------------------*/

STDMETHODIMP Clvcbench::KEY_RANGE_ITER(OmsTypeInt4 maxNumber, OmsTypeInt4 noOfIter)
{
    TRACE_METHOD(KEY_RANGE_ITER)
#if MINOR_VERSION_NO_SP100 >= 4
    try
    {
        CallInfo callInfo(m_taskId);
        CKey startKey(0,0);
        CKey stopKey(255,255);
        for (int j=0; j<noOfIter; ++j){
        int i = 0;
        for (OmsKeyRangeIter<CKeyedObject> keyIter =
            CKeyedObject::omsFirstKey(*this, globSchemaHandle, globContainerNo,
            startKey, stopKey); keyIter; ++keyIter)
        {
            const CKeyedObject* pKey = keyIter.omsDeRef();
            callInfo.IncOpCount();
            if (++i == maxNumber){
              break;
            }

        }
        }
    }
    CATCH_ALL
#endif
        return S_OK;
}

/*-----------------------------------------------------------------------------------------*/

STDMETHODIMP Clvcbench::UPDATE_OBJECTS()
{
    TRACE_METHOD(UPDATE_OBJECTS)
        try
    {
        CallInfo callInfo(m_taskId);
        CObject* p = m_listHead.omsDeRefForUpd(*this, true);
        callInfo.IncOpCount();
        while (p->m_next.isTrue())
        {
            callInfo.IncOpCount();
            p = p->m_next.omsDeRefForUpd(*this, true);
            p->omsStore(*this);
        }
    }
    CATCH_ALL
        
        return S_OK;
}

/*-----------------------------------------------------------------------------------------*/

STDMETHODIMP Clvcbench::CREATE_ARRAY_OBJECTS(OmsTypeInt4 cntObjects, OmsTypeInt4 arraySize)
{
  TRACE_METHOD(CREATE_ARRAY_OBJECTS) 

  OmsAllocatorInterface& allocator = omsGetSessionAllocator();
  if (m_oidOfArrayObjs != NULL){
    allocator.omsDeallocate(m_oidOfArrayObjs);
  }
                      
  m_oidOfArrayObjs = (OmsArrayObjectOid*)allocator.omsAllocate(cntObjects * sizeof(OmsArrayObjectOid));
  m_noOfArrayObjs  = cntObjects;

  try {
    CallInfo callInfo(m_taskId, cntObjects);

    for (int i=0; i<m_noOfArrayObjs; ++i){
      OmsArrayObjectType *p = new(*this, globSchemaHandle, globContainerNo, arraySize)   OmsArrayObjectType(*this, arraySize);
      m_oidOfArrayObjs[i] = p->omsGetOid();
    }
  }
  CATCH_ALL
  
  return S_OK;
}

/*-----------------------------------------------------------------------------------------*/

STDMETHODIMP Clvcbench::DEREF_ARRAY_OBJECTS()
{
  TRACE_METHOD(DEREF_ARRAY_OBJECTS) 

  try {
    CallInfo callInfo(m_taskId, m_noOfArrayObjs);
    for (int i=0; i<m_noOfArrayObjs; ++i){
      const OmsArrayObjectType *q1 = m_oidOfArrayObjs[i].omsDeRef(*this);
    }
  }
  CATCH_ALL
    return S_OK;
}
/*-----------------------------------------------------------------------------------------*/

STDMETHODIMP Clvcbench::DELETE_ARRAY_OBJECTS()
{
  TRACE_METHOD(DELETE_ARRAY_OBJECTS) 

  try {
    CallInfo callInfo(m_taskId, m_noOfArrayObjs);

    for (int i=0; i<m_noOfArrayObjs; ++i){
      OmsArrayObjectType *q1 = m_oidOfArrayObjs[i].omsDeRefForUpd(*this);
      q1->omsDelete(*this);
    }
  }
  CATCH_ALL
  
  if (m_oidOfArrayObjs != NULL){
    OmsAllocatorInterface& allocator = omsGetSessionAllocator();
    allocator.omsDeallocate(m_oidOfArrayObjs);
    m_oidOfArrayObjs = NULL;
    m_noOfArrayObjs  = 0;
  }
    
  return S_OK;
}

/*-----------------------------------------------------------------------------------------*/
#define MALLOC_ALIGN_MASK      (7)

inline OmsTypeUInt4 AlignedSize(OmsTypeUInt4 req) 
{
    return ((req) + MALLOC_ALIGN_MASK) & ~(MALLOC_ALIGN_MASK);
}


STDMETHODIMP Clvcbench::HEAP_TEST(int MemSizeKB, int MinChunkSize, int MaxChunkSize, int loopCnt)
{
    TRACE_METHOD(HEAP_TEST)
        CallInfo callInfo(m_taskId);
    
#define MAX_HEADER_ENTRIES 8192
    struct ChunkInfo 
    {
        int   m_size;
        char* m_ptr;
    };
    struct ChunkHeader {
        ChunkHeader() : m_next(NULL), m_cnt(0) {}
        ChunkInfo    m_chunk[MAX_HEADER_ENTRIES];
        ChunkHeader* m_next;
        int          m_cnt;
    };
    bool         error = false;
    DbpError     dbp_Error(0);
    ChunkHeader* pHeader   = new ChunkHeader();
    ChunkHeader* last      = pHeader;
    OmsTypeInt64 all       = sizeof(ChunkHeader);
    int          cntChunkHeader = 1;
    try
    {
        while (1) {
            int required = MinChunkSize + Random_glob.Next(MaxChunkSize - MinChunkSize);
            all         += (OmsTypeInt64) AlignedSize(required);
            last->m_chunk[last->m_cnt].m_ptr  = new char[required];
            callInfo.IncOpCount();
            last->m_chunk[last->m_cnt].m_size = AlignedSize(required);
            ++last->m_cnt;
            if (last->m_cnt == MAX_HEADER_ENTRIES) {
                if (all + sizeof(ChunkHeader) > MemSizeKB * 1024)
                {
                    break;
                }
                last->m_next = new ChunkHeader();
                callInfo.IncOpCount();
                ++cntChunkHeader;
                all         += sizeof(ChunkHeader);
                last         = last->m_next;
            }
            if (all > MemSizeKB * 1024)
            {
                break;
            }
        }
        for (int loop = 0; loop < loopCnt; ++loop)
        {
            int headerIdx = Random_glob.Next() % cntChunkHeader;
            ChunkHeader* pCurr = pHeader;
            while (headerIdx > 0)
            {
                --headerIdx;
                pCurr = pCurr->m_next;
            }
            int idx = Random_glob.Next() % pCurr->m_cnt;
            if (pCurr->m_chunk[idx].m_ptr)
            {
                callInfo.IncOpCount();
                delete[] pCurr->m_chunk[idx].m_ptr;
                all -= pCurr->m_chunk[idx].m_size;
                pCurr->m_chunk[idx].m_size = 0;
                pCurr->m_chunk[idx].m_ptr  = 0;
            }
            int required = Random_glob.Next(1000);
            int alignedSize = AlignedSize(required);
            if (all + alignedSize > MemSizeKB * 1024)
            {
                required = (int) (MemSizeKB * 1024 - all);
                if (required <= 0) continue;
            }
            pCurr->m_chunk[idx].m_ptr  = new char[required];
            callInfo.IncOpCount();
            pCurr->m_chunk[idx].m_size = alignedSize;
            all += alignedSize;
        }
    }
    CATCH_ALL
        ChunkHeader* curr = pHeader;
    do {
        for (int ix = 0; ix < curr->m_cnt; ++ix) {
            if (curr->m_chunk[ix].m_ptr)
            {
                callInfo.IncOpCount();
                delete[] curr->m_chunk[ix].m_ptr;
            }
        }
        pHeader = curr;
        curr    = curr->m_next;
        delete pHeader;
        callInfo.IncOpCount();
    }
    while (pHeader != last);
    return S_OK;
}

/*-----------------------------------------------------------------------------------------*/

STDMETHODIMP Clvcbench::HEAP_SMALL_CHUNKS(int MemSizeKB, int loopCnt)
{
    TRACE_METHOD(HEAP_SMALL_CHUNKS)
        struct SmallChunk
    {
        SmallChunk* m_next;
#if !defined(BIT64)
        OmsTypeInt4 m_filler;
#endif
        SmallChunk()
            : m_next(0)
        {
        }
    };
    int requiredChunks =  MemSizeKB * 1024 / AlignedSize(sizeof(SmallChunk));
    CallInfo callInfo(m_taskId, loopCnt * requiredChunks * 2);
    SmallChunk* head;
    SmallChunk* prev;
    try
    {
        for (int loop = 0; loop < loopCnt; ++loop)
        {
            head = new SmallChunk();
            prev = head;
            for (int ix = 0; ix < requiredChunks; ++ix)
            {
                prev->m_next = new SmallChunk();
                prev         = prev->m_next;
            }
            while (head)
            {
                prev = head;
                head = head->m_next;
                delete prev;
            }
        }
    }
    catch(BAD_ALLOC)
    {
        while (head)
        {
            prev = head;
            head = head->m_next;
            delete prev;
        }
        DbpError e(-28000);
        this->ExceptionHandler(e);
    }
    catch(DbpError& e)
    {
        this->ExceptionHandler(e);
    }
    return S_OK;
}

/*-----------------------------------------------------------------------------------------*/

STDMETHODIMP Clvcbench::INIT_SQL(OmsTypeInt4 initRows)
{
    TRACE_METHOD(INIT_SQL)
        try
    {
        char stmt[1000];
        strcpy(stmt, "CREATE TABLE LVCBENCH_ORDKEY(");
        strcat(stmt, "MANDT           CHAR(3),");
        strcat(stmt, "ORDID           CHAR(22),");
        strcat(stmt, "SIMID           CHAR(22),");
        strcat(stmt, "ORDTYPE         VARCHAR(1) BYTE,");
        strcat(stmt, "ORDNO           VARCHAR(12),");
        strcat(stmt, "APPLI           VARCHAR(4),");
        strcat(stmt, "UPDIN           VARCHAR(1),");
        strcat(stmt, "TRPID           VARCHAR(22),");
        strcat(stmt, "TRPID_TYPE      FIXED(3),");
        strcat(stmt, "PRODUCABLE      VARCHAR(1),");
        strcat(stmt, "OUTPUT_FIXED    VARCHAR(1),");
        strcat(stmt, "PART_DELIVERED  VARCHAR(1),");
        strcat(stmt, "FINAL_DELIVERY  VARCHAR(1),");
        strcat(stmt, "UNPLANED        VARCHAR(1),");
        strcat(stmt, "RELEASED        VARCHAR(1),");
        strcat(stmt, "DATE_FIXED      VARCHAR(1),");
        strcat(stmt, "STARTED         VARCHAR(1),");
        strcat(stmt, "PART_CONFIRMED  VARCHAR(1),");
        strcat(stmt, "FINAL_CONFIRMED VARCHAR(1),");
        strcat(stmt, "TTYPE           VARCHAR(10),");
        strcat(stmt, "TPRIO           VARCHAR(10),");
        strcat(stmt, "TROID           VARCHAR(22),");
        strcat(stmt, "STRAT           VARCHAR(2),");
        strcat(stmt, "LOCID           VARCHAR(22),");
        strcat(stmt, "OLTPTRANSFER    VARCHAR(1),");
        strcat(stmt, "CNVIN           VARCHAR(1),");
        strcat(stmt, "QTY_INVALID     VARCHAR(1),");
        strcat(stmt, "IPPE_PPM_CHAN   VARCHAR(1),");
        strcat(stmt, "CTM_STATUS      VARCHAR(1),");
        strcat(stmt, "ATP_ORDER_STAT  VARCHAR(1),");
        strcat(stmt, "TCCTMS          FIXED(15),");
        strcat(stmt, "PRIMARY KEY(MANDT,ORDID,SIMID))");
        SQL createTab = sql(stmt);
        createTab.sqlExecute();
        CallInfo callInfo(m_taskId, initRows);
        this->InsertOrdKey(initRows);
    }
    CATCH_ALL
        
        return S_OK;
}

/*-----------------------------------------------------------------------------------------*/

STDMETHODIMP Clvcbench::INSERT_SQL(OmsTypeInt4 noOfRows)
{
    TRACE_METHOD(INSERT_SQL)
        try
    {
        CallInfo callInfo(m_taskId, noOfRows);
        this->InsertOrdKey(noOfRows);
    }
    CATCH_ALL
        return S_OK;
}

/*-----------------------------------------------------------------------------------------*/

STDMETHODIMP Clvcbench::GET_CALL_STATISTICS(OmsMonitorData* info)
{
    TRACE_METHOD(GET_CALL_STATISTICS)
#if MINOR_VERSION_NO_SP100 >= 4
#if CORRECTION_LEVEL_SP100 >= 2
        *info = m_callStatistics;
#endif
#endif
    return S_OK;
}

/*-----------------------------------------------------------------------------------------*/

STDMETHODIMP Clvcbench::LOCK_SHARE(int no)
{
    TRACE_METHOD(LOCK_SHARE)
        try
    {
        dbpTrace("LOCK_SHARE");
        OmsLockHandle h;
        memset(&h, ' ', sizeof(h));
        sprintf((char*) &h, "LOCK%d", no);
        OmsLockObject lock(*this);
        lock.omsEnable(h);
        lock.omsLockShare();
    }
    CATCH_ALL
        return S_OK;
}

/*-----------------------------------------------------------------------------------------*/

STDMETHODIMP Clvcbench::LOCK_EXCLUSIVE(int no)
{
    TRACE_METHOD(LOCK_EXCLUSIVE)
        try
    {
        dbpTrace("LOCK_EXCLUSIVE");
        
        OmsLockHandle h;
        memset(&h, ' ', sizeof(h));
        sprintf((char*) &h, "LOCK%d", no);
        OmsLockObject lock(*this);
        lock.omsEnable(h);
        lock.omsLockExclusive();
    }
    CATCH_ALL
        return S_OK;
}

/*-----------------------------------------------------------------------------------------*/

STDMETHODIMP Clvcbench::UNLOCK(int no)
{
    TRACE_METHOD(UNLOCK)
        try
    {
        OmsLockHandle h;
        memset(&h, ' ', sizeof(h));
        sprintf((char*) &h, "LOCK%d", no);
        OmsLockObject lock(*this);
        lock.omsEnable(h);
        lock.omsUnlock();
    }
    CATCH_ALL
        return S_OK;
}

/*-----------------------------------------------------------------------------------------*/

STDMETHODIMP Clvcbench::WAIT_FOR_GARBAGE_COLLECTOR()
{
    TRACE_METHOD(WAIT_FOR_GARBAGE_COLLECTOR)
        this->WaitForGarbageCollector();
    return S_OK;
}

/*-----------------------------------------------------------------------------------------*/

void  Clvcbench::DropTableOrdkey()
{
    TRACE_METHOD(DropTableOrdkey)
        try
    {
        SQL drop = sql("drop table lvcbench_ordkey");
        drop.sqlExecute();
    }
    catch (DbpError&)
    {
    }
}

/*-----------------------------------------------------------------------------------------*/

void Clvcbench::InsertOrdKey(int rowCount)
{
    TRACE_METHOD(InsertOrdKey)
        
        SQL insert = sql("INSERT INTO LVCBENCH_ORDKEY VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)");
    insert.sqlPrepare();
    char mandt[4];
    strcpy(mandt, "000");
    char ordid [23];
    char guid  [23];
    char char1 [2];
    char char4 [5];
    char char10[11];
    char char12[13];
    
    int f3 = 111;
    int f15 = 12345;
    
    strcpy(char1,  "A");
    strcpy(char4,  "BBBB");
    strcpy(char10, "CCCCCCCCCC");
    strcpy(char12, "DDDDDDDDDDDD");
    strcpy(guid,   "GUIDGUIDGUIDGUIDGUIDGU");
    strcpy(ordid, guid);
    
    insert << SqlCol(&mandt[0],  SqlCol::VSTRING, sizeof(mandt));
    insert << SqlCol(&ordid[0],  SqlCol::VCHAR,   sizeof(ordid));
    insert << SqlCol(&guid [0],  SqlCol::VSTRING, sizeof(guid));
    insert << SqlCol(&char1[0],  SqlCol::VSTRING, sizeof(char1));
    insert << SqlCol(&char12[0], SqlCol::VSTRING, sizeof(char12));
    insert << SqlCol(&char4[0],  SqlCol::VSTRING, sizeof(char4));
    insert << SqlCol(&char1[0],  SqlCol::VSTRING, sizeof(char1));
    insert << SqlCol(&guid [0],  SqlCol::VSTRING, sizeof(guid));
    insert << f3;
    for (int ix = 0; ix < 10; ++ix)
    {
        insert << SqlCol(&char1[0],  SqlCol::VSTRING, sizeof(char1));
    }
    insert << SqlCol(&char10[0], SqlCol::VSTRING, sizeof(char10));
    insert << SqlCol(&char10[0], SqlCol::VSTRING, sizeof(char10));
    insert << SqlCol(&guid  [0], SqlCol::VSTRING, sizeof(guid));
    insert << SqlCol(&char1 [0], SqlCol::VSTRING, sizeof(char1));
    insert << SqlCol(&guid  [0], SqlCol::VSTRING, sizeof(guid));
    for (int jx = 0; jx < 6; ++jx)
    {
        insert << SqlCol(&char1[0],  SqlCol::VSTRING, sizeof(char1));
    }
    insert << f15;
    while (rowCount > 0)
    {
        ++m_ordId;
        memcpy(&ordid[17], &m_ordId, 4); 
        insert.sqlExecute();
        --rowCount;
    }
}

/*-----------------------------------------------------------------------------------------*/

void Clvcbench::WaitForGarbageCollector()
{
  TRACE_METHOD(WaitForGarbageCollector)
  SQL sel = sql("select * from sysmon_task where tasktype = 'GarbCol' and not statedesc like 'No-Work%'");
  do {
    sel.sqlExecute();
  } while (0 == sqlCode());
}

/*-----------------------------------------------------------------------------------------*/

STDMETHODIMP Clvcbench::GET_CALL_INFO(OmsTypeInt4  maxTaskId, 
                                      OmsTypeInt4* runTime, 
                                      OmsTypeInt4* opCount, 
                                      OmsTypeInt4* errorCode)
{
    TRACE_METHOD(GET_CALL_INFO)
    m_profiling           = false; // disable profiling for this call, would destroy previous info
    *opCount              = 0;
    OmsTypeInt8 startTime;
    OmsTypeInt8 endTime;
    *errorCode     = 0;
    if (-1 == maxTaskId)
    {
        startTime = glob_callInfo[m_taskId].m_startTime;
        endTime   = glob_callInfo[m_taskId].m_endTime;
        *opCount  = glob_callInfo[m_taskId].m_opCount;
    }
    else
    {
        startTime  = glob_callInfo[1].m_startTime;
        endTime    = glob_callInfo[1].m_endTime;
        *opCount   = glob_callInfo[1].m_opCount;
        *errorCode = glob_callInfo[1].m_errorCode;
        for (int task = 2; task <= maxTaskId; ++task)
        {
            if (glob_callInfo[task].m_startTime < startTime)
            {
                startTime = glob_callInfo[task].m_startTime;
            }
            if (glob_callInfo[task].m_endTime > endTime)
            {
                endTime = glob_callInfo[task].m_endTime;
            }
            *opCount += glob_callInfo[task].m_opCount;
            if (0 == *errorCode)
            {
                *errorCode = glob_callInfo[task].m_errorCode;
            }
        }
    }
    *runTime = (OmsTypeInt4) (endTime - startTime);
    return S_OK;
}

/*-----------------------------------------------------------------------------------------*/

STDMETHODIMP Clvcbench::STD_TRANS()
{
    TRACE_METHOD(STD_TRANS)
        try
    {
        int cntDeleted = 0;
        ++glob_callInfo[m_taskId].m_callCnt;
        for (OmsObjByClsIter<CObject> iter = CObject::omsAllOids(*this, globSchemaHandle, globContainerNo); iter; ++iter) 
        {
            if (m_stop)
            {
                throw DbpError(-4711);
            }
            const CObject* pObj = iter.omsDeRef();
            if (pObj->m_taskId == m_taskId)
            {
                CObject* pDelete = pObj->omsForUpdPtr(*this, true);
                pDelete->omsDelete(*this);
                ++cntDeleted;
                if (1000 == cntDeleted)
                {
                    break;
                }
            }
        }
        while (cntDeleted > 0)
        {
            new(*this, globSchemaHandle, globContainerNo) CObject(m_taskId);
            --cntDeleted;
        }
    }
    CATCH_ALL
        return S_OK;
}

/*-----------------------------------------------------------------------------------------*/

STDMETHODIMP Clvcbench::STOP()
{
    TRACE_METHOD(STOP)
        m_stop = true;
    return S_OK;
}

/*-----------------------------------------------------------------------------------------*/

STDMETHODIMP Clvcbench::GET_TRANS_COUNT (OmsTypeInt4 maxTaskId, OmsTypeInt4* transCount)
{
    *transCount = 0;
    for (int task = 1; task <= maxTaskId; ++task)
    {
        *transCount += glob_callInfo[task].m_callCnt;
    }
    return S_OK;
}

/*-----------------------------------------------------------------------------------------*/

STDMETHODIMP Clvcbench::CREATE_TP_DATA (OmsTypeInt4 maxTaskId, OmsTypeInt4 listLength, OmsTypeInt4 deleteIntervall)
{
    try
    { 
        dbpOpMsg ("CREATE_TP_DATA %d", maxTaskId);
        if (maxTaskId > MAX_TASK)
        {
            throw DbpError (-4711, "too many tasks, max is 300");
        }
        int listCnt; 
        m_maxTaskId = maxTaskId;
        this->CLEAN_UP();
        this->CREATE_CONTAINER(0, 1, 0);
        this->CREATE_CONTAINER(1, 1, 1);
        // store number of tasks
        CKey key(TASK_INFO_KEY, TASK_INFO_KEY);
        CKeyedObject* pKey     = new(*this, key, globSchemaHandle, globContainerNo) CKeyedObject(1, maxTaskId);
		pKey->m_listLength     = listLength;
		pKey->m_deleteInterval = deleteIntervall;
		m_listLength           = listLength;
		m_deleteInterval       = deleteIntervall;
        // 
        // create data
        int task;
        for (task = 1; task <= maxTaskId; ++task)
        {
            listCnt = listLength;
            // create keyed object as list entry for current task
            CKey key(task, 1);
            CKeyedObject* pKey = new(*this, key, globSchemaHandle, globContainerNo) CKeyedObject(1, task);
            // create first list element
            CObject*      pObj = new(*this, globSchemaHandle, globContainerNo) CObject(task);
            pKey->m_oid = pObj->omsGetOid();
            pKey->omsStore(*this);
            {
                CKey key(pObj->omsGetOid().getPno(), pObj->omsGetOid().getPagePos());
                new(*this, key, globSchemaHandle, globContainerNo) 
                    CKeyedObject(pObj->omsGetOid().getPno(), pObj->omsGetOid().getPagePos());
            }
            CObject* pPrev = pObj;
            --listCnt;
            for (int ix = 0; ix < listCnt; ++ix)
            {
               pObj = new(*this, globSchemaHandle, globContainerNo) CObject(ix);
               pPrev->m_next = pObj->omsGetOid();
               pPrev = pObj;
               CKey key(pObj->omsGetOid().getPno(), pObj->omsGetOid().getPagePos());
               new(*this, key, globSchemaHandle, globContainerNo) 
                   CKeyedObject(pObj->omsGetOid().getPno(), pObj->omsGetOid().getPagePos());
            }
        }
        // verify data
        bool error = false;
        for (task = 1; task <= maxTaskId; ++task)
        {
            CKey key(task, 1);
            const CKeyedObject* pKey = CKeyedObject::omsKeyAccess(*this, key, globSchemaHandle, globContainerNo);
            if (!pKey)
            {
                throw DbpError (-31002, "task key not found");
            }
            int elemCnt = 0;
            OmsOid<CObject> listOid = pKey->m_oid;
            while (listOid.isTrue())
            {
                const CObject* pObj = listOid(*this);
                ++elemCnt;
                listOid = pObj->m_next;
            }
            if (elemCnt != listLength)
            {
                dbpOpError ("task %d, wrong list length : %d", task, elemCnt);
                error = true;
                           }
        }
        if (error)
        {
            throw DbpError (-31004, "wrong list length");
        }
    }
    CATCH_ALL
    return S_OK;
}

/*-----------------------------------------------------------------------------------------*/

STDMETHODIMP Clvcbench::TP_TRANS ()
{
    try
    {
        // DebugBreak();
        omsStartSubtrans();
#if defined(TRACE_METHODS)
        dbpOpMsg ("TP_TRANS %d", m_taskId);
#endif
        if (0 == m_taskId)
        {
            OmsCriticalSection(this, 1);
            if (0 == m_maxTaskId)
            {
                globSchemaHandle = omsGetSchemaHandle(lvcBenchSchema);
                // create the containers
                this->CREATE_CONTAINER(0, 1, 0);
                this->CREATE_CONTAINER(1, 1, 1);
                // read max task number from livecache base
                CKey key(TASK_INFO_KEY, TASK_INFO_KEY);
                const CKeyedObject* pKey = CKeyedObject::omsKeyAccess(*this, key, globSchemaHandle, globContainerNo);
                if (!pKey)
                {
                    throw DbpError (-31001, "task info not found");
                }
                else
                {
                    m_maxTaskId      = pKey->m_processNo;
					m_listLength     = pKey->m_listLength;
					m_deleteInterval = pKey->m_deleteInterval;
					dbpOpMsg ("list length     %d", m_listLength);
					dbpOpMsg ("delete interval %d", m_deleteInterval);
                }
            }
            m_taskId = m_maxTaskId--;
            dbpOpMsg ("TP_TRANS assigned %d", m_taskId);
        }
        if (0 == m_taskId)
        {
            throw DbpError (-31001, "wrong task id detected");
        } 
        CKey key(m_taskId, 1);
        ++glob_callInfo[m_taskId].m_callCnt;
        const CKeyedObject* pKey = CKeyedObject::omsKeyAccess(*this, key, globSchemaHandle, globContainerNo);
        if (!pKey)
        {
            throw DbpError (-31002, "task key not found");
        }
        OmsOid<CObject> listOid = pKey->m_oid;
        int delCountDown        = m_deleteInterval;
        int deleted             = 0;
        const CObject* pPrev = NULL;
        CObject*       pUpd;
        int elemCnt = 0;
        while (listOid.isTrue())
        {
            if (m_stop)
            {
                throw DbpError(-4711);
            }
            const CObject* pObj = listOid(*this);
            ++elemCnt;
            listOid = pObj->m_next;
            --delCountDown;
            if (0 == delCountDown)
            {   // delete a list element
                delCountDown = m_deleteInterval;
                // link next to previous
                pUpd = pPrev->omsForUpdPtr(*this, true);
                pUpd->m_next  = pObj->m_next;
                pUpd->omsStore(*this);
                // delete key object
                CKey key(pObj->omsGetOid().getPno(), pObj->omsGetOid().getPagePos());
                CKeyedObject* pKey = CKeyedObject::omsKeyAccessForUpd(*this, key, true, globSchemaHandle, globContainerNo);
                if (pKey)
                {
                    pKey->omsDelete(*this);
                }
                else
                {
                    throw DbpError (-31003, "key not found");
                }
                // delete objct
                CObject* pDel = pObj->omsForUpdPtr(*this, true);
                pDel->omsDelete(*this);
                ++deleted;
            }
            else
            {
                if (pPrev)
                {
                    CONST_CAST(CObject*, pPrev)->omsRelease(*this);
                
                }
                pPrev = pObj;
            }

        }
        if (elemCnt != m_listLength)
        {
            dbpOpError ("wrong list length : %d", elemCnt);
            throw DbpError (-31004, "wrong list length");
        }
        pUpd = pPrev->omsForUpdPtr(*this, true);
        for (int ix = 0; ix < deleted; ++ix)
        {
            CObject* pNewObj = new(*this, globSchemaHandle, globContainerNo) CObject(ix);
            pUpd->m_next = pNewObj->omsGetOid();
            pUpd->omsStore(*this);
            pNewObj->omsStore(*this);
            pUpd = pNewObj;
            CKey key(pNewObj->omsGetOid().getPno(), pNewObj->omsGetOid().getPagePos());
            new(*this, key, globSchemaHandle, globContainerNo) 
                CKeyedObject(pNewObj->omsGetOid().getPno(), pNewObj->omsGetOid().getPagePos());
        }
        omsCommitSubtrans();
    }
    CATCH_ALL
    return S_OK;
}

/*-----------------------------------------------------------------------------------------*/

#define SQLCOL(P) SqlCol (P, SqlCol::VCHAR, sizeof(*P))

STDMETHODIMP Clvcbench::TP2_TRANSAC_CPP(ACCOUNT_KEY    *accountKey,
                                  int            accountAdd, 
                                  BRANCH_KEY     *branchKey, 
                                  TELLER_KEY     *tellerKey, 
                                  HISTORY_FILLER *historyFiller, 
                                  int            *returnCode)
{
  if (0 == m_updAccount)
  {
	  m_updAccount  = new SQL;
	  *m_updAccount = sql("UPDATE bench.ACCOUNT "
		  "SET ACCOUNT_VALUE = ACCOUNT_VALUE + ? "
		  "WHERE ACCOUNT_KEY = ?");
	  m_updAccount->sqlPrepare();
	  if (sqlCode () != 0) {
		  dbpOpError ("could not prepare");
		  throw -1;
	  }

	  m_selAccount    = new SQL;
	  *m_selAccount   = sql("SELECT ACCOUNT_VALUE INTO ? "
		  "FROM bench.ACCOUNT "
		  "WHERE ACCOUNT_KEY = ?");
	  m_selAccount->sqlPrepare();
	  if (sqlCode () != 0) {
		  dbpOpError ("could not prepare");
		  throw -1;
	  }

	  m_insertHistory  = new SQL;
	  *m_insertHistory = sql("INSERT INTO bench.HISTORY VALUES (?,?,?,?,?)");
	  m_insertHistory->sqlPrepare();
	  if (sqlCode () != 0) {
		  dbpOpError ("could not prepare");
		  throw -1;
	  }

	  m_updTeller     = new SQL;
	  *m_updTeller    = sql("UPDATE bench.TELLER "
		  "SET TELLER_VALUE = TELLER_VALUE + ? "
		  "WHERE TELLER_KEY = ?");
	  m_updTeller->sqlPrepare();
      if (sqlCode () != 0) {
		  dbpOpError ("could not prepare");
		  throw -1;
	  }
	  m_updBranch     = new SQL;
	  *m_updBranch    = sql("UPDATE bench.BRANCH "
		  "SET BRANCH_VALUE = BRANCH_VALUE + :ACCOUNT_ADD "
		  "WHERE BRANCH_KEY = :BRANCH_KEY");
	  m_updBranch->sqlPrepare();
      if (sqlCode () != 0) {
		  dbpOpError ("could not prepare");
		  throw -1;
	  }
  }
  unsigned int accountValue;
  int error = 0;
  *m_updAccount << accountAdd << SQLCOL(accountKey);
  m_updAccount->sqlExecute();
  if (sqlCode () != 0) error = sqlCode(); 

  *m_selAccount << accountValue << SQLCOL(accountKey); 
  m_selAccount->sqlExecute();
  if (sqlCode () != 0) error = sqlCode(); 

  *m_insertHistory << SQLCOL(accountKey) << accountAdd << 
	  SQLCOL(branchKey) << SQLCOL(tellerKey) << SQLCOL(historyFiller);
  m_insertHistory->sqlExecute();
  if (sqlCode () != 0) error = sqlCode(); 

  *m_updTeller << accountAdd << SQLCOL(tellerKey);
  m_updTeller->sqlExecute();
  if (sqlCode () != 0) error = sqlCode(); 

  *m_updBranch << accountAdd << SQLCOL(branchKey);
  m_updBranch->sqlExecute(); 
  if (sqlCode () != 0) error = sqlCode(); 
  *returnCode = error;
  return S_OK;
}
