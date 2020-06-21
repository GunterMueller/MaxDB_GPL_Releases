/***************************************************************************


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
/* lvcbenchObj.h : Declaration of the Clvcbench                                 */
/****************************************************************************/

#ifndef __lvcbenchOBJ_H_
#define __lvcbenchOBJ_H_

#define SQL_INCLUDED
#define OMS_INCLUDED

#define LC_DBPROC_PROTO
#include "livecachedbproc.h"  /* nocheck */


class CObject : public OmsObject<CObject, 40>
{
public :
  OmsOid<CObject> m_next;
  OmsTypeInt4     m_taskId;
  char            m_fill[200];
  CObject() 
	  : m_taskId(-1)
  {}
  CObject(int taskId)
	  : m_taskId(taskId)
  {
  }
};

class I4BigEndian
{
public :
  I4BigEndian()
  {
    this->operator=(0);
  }
  I4BigEndian(OmsTypeUInt4 val)
  {
    for (int ix = 3; ix >= 0; --ix)
    {
      m_long[ix] = val % 256;
      val = val / 256;
    }
  }
  OmsTypeInt4 GetInt4()
  {
    OmsTypeUInt4 val = 0;
    OmsTypeUInt4 exp = 1;
    for (int ix = 3; ix >= 0; --ix)
    {
      val += m_long[ix] * exp;
      exp *= 256;
    }
    return val;
  }
  void operator=(int val)
  {
    for (int ix = 3; ix >= 0; --ix)
    {
      m_long[ix] = val % 256;
      val = val / 256;
    }
  }
private :
  unsigned char m_long[4];
};

class CKey
{
public :
	CKey(OmsTypeInt4 pno, OmsTypeInt4 val)
		: m_pno(pno)
		, m_val(val)
	{
		memset(&m_fill[0], 'A', sizeof(m_fill));
	}
  void  omsKeyToBinary(void* pDest) const 
  {
    memcpy (pDest, this, sizeof(*this));
  }
  void omsBinaryToKey(void* pKey) 
  {
    memcpy(this, pKey, sizeof(*this));
  }
private :
	I4BigEndian m_pno;
	I4BigEndian m_val;
	char        m_fill[14];
};


class CKeyedObject : public OmsKeyedObject<CKeyedObject, CKey, 41>
{
public :
  OmsTypeInt4     m_long;
  short           m_processNo;
  OmsOid<CObject> m_oid;
  OmsTypeInt4     m_listLength;
  OmsTypeInt4     m_deleteInterval;
  char            m_fill[184];
  CKeyedObject() 
	  : m_long(0)
	  , m_processNo(0)
  {}
  CKeyedObject(long val, short taskId)
	  : m_long(val)
	  , m_processNo(taskId)
  {
  }
};

class CVarObjects
{
public :

  CVarObjects(OmsHandle& h,int noOfVarObjects);
  OmsVarOid& operator[](int idx)
  {
    return m_varOids[idx];
  }
  int Size() const
  {
    return m_noOfVarObjects;
  }
  ~CVarObjects();
private :
  OmsHandle& m_handle;
  int        m_noOfVarObjects;
  OmsVarOid* m_varOids; 
};

class CallBack;

const int ClsId = 4715;
typedef OmsArrayObject<int, ClsId> OmsArrayObjectType;
typedef OmsOid<OmsArrayObjectType> OmsArrayObjectOid;


/****************************************************************************/
/* Clvcbench                                                                */
class  Clvcbench :
  public OmsHandle, 
  public SqlHandle, 
  public IliveCacheSpec,
  public Ilvcbench
{
private:
  long      m_cRef;
  IUnknown *m_pIUnknown;
  int             m_taskId;
  OmsTypeInt4     m_runTime;
  OmsOid<CObject> m_listHead;
  CVarObjects*    m_varObjAnchor;
  int             m_ordId;
  int             m_cntObjects;
  OmsMonitorData  m_callStatistics;
  bool            m_profiling;
  SQL*            m_updAccount;
  SQL*            m_selAccount;
  SQL*            m_insertHistory;
  SQL*            m_updTeller;
  SQL*            m_updBranch;
  OmsOid<CObject> *m_pOid;    // PTS 1120717
  const CObject   **m_ppObj;  // PTS 1120717
  CKey               **m_ppKey;     // PTS 1123669
  const CKeyedObject **m_ppKeyObj;  // PTS 1123669
  OmsCallbackInterface  *m_pTransactionInterface; // PTS 1128285
  OmsArrayObjectOid *m_oidOfArrayObjs;
  int                m_noOfArrayObjs;

public:
  friend class CallBack;
	Clvcbench() 
		: m_cRef(0)
		, m_pIUnknown(NULL)
    , m_taskId(0)
		, m_listHead()
    , m_varObjAnchor(0)
    , m_ordId(0)
    , m_cntObjects(0)
    , m_profiling(true)
    , m_updAccount(NULL)
    , m_selAccount(NULL)
    , m_insertHistory(NULL)
    , m_updTeller(NULL)
    , m_updBranch(NULL)
    , m_pOid(NULL)
    , m_ppObj(NULL)
    , m_ppKey(NULL)     // PTS 1123525  
    , m_ppKeyObj(NULL)  // PTS 1123525
    , m_pTransactionInterface(NULL) // PTS 1128285
    , m_oidOfArrayObjs(0)
    , m_noOfArrayObjs(0)
 	{
        IUnknown** pIKernelSink = &m_pIUnknown;
        omsInitSession (pIKernelSink);
        INIT_STRING_HANDLING;       /* macro defined within livecachedbproc.h */
        sqlInitSession (pIKernelSink);
	}

  ~Clvcbench();

  void DropTableOrdkey();

  void ExceptionHandler(DbpError& e);

  void InsertOrdKey(int rowCount);

  void SetRunTime(OmsTypeInt4 runTime)
  {
    m_runTime = runTime;
  }

  void WaitForGarbageCollector();

#define COM_INTERFACE Ilvcbench
#define COM_INTERFACE_IID IID_Ilvcbench
#include "livecachedbproc.h"   /* nocheck */

/*-----------------------------------------------------*/ 
/* following lines only included for ActiveX Wizard    */
/* no code will be generated                           */
#if defined(CODE_FOR_WIZARD_ONLY)
DECLARE_NOT_AGGREGATABLE(Clvcbench)
DECLARE_PROTECT_FINAL_CONSTRUCT()
BEGIN_COM_MAP(Clvcbench)
	COM_INTERFACE_ENTRY(Ilvcbench)
END_COM_MAP()
DECLARE_REGISTRY_RESOURCEID(IDR_lvcbench)
DECLARE_NOT_AGGREGATABLE(Clvcbench)
DECLARE_PROTECT_FINAL_CONSTRUCT()
BEGIN_COM_MAP(Clvcbench)
	COM_INTERFACE_ENTRY(Ilvcbench)
END_COM_MAP()
#endif
/*-----------------------------------------------------*/

public:
  STDMETHOD(CALL_DISPATCHER)(/*[in]*/ OmsTypeInt4 count); 
  STDMETHOD(TP2_TRANSAC_CPP)(/*[in]*/ ACCOUNT_KEY* accountKey, 
    /*[in]*/ int         accountAdd,  
    /*[in]*/ BRANCH_KEY* branchKey, 
    /*[in]*/ TELLER_KEY* tellerKey, 
    /*[in]*/ HISTORY_FILLER* historyFiller, 
    /*[out]*/ int* returnCode);
  STDMETHOD(TP_TRANS) ();
  STDMETHOD(CREATE_TP_DATA) (OmsTypeInt4 maxTaskId, OmsTypeInt4 listLength, OmsTypeInt4 deleteIntervall);
  STDMETHOD(GET_TRANS_COUNT)(OmsTypeInt4 maxTaskId, OmsTypeInt4* transCount);
  STDMETHOD(STOP)();
  STDMETHOD(STD_TRANS)();
  STDMETHOD(GET_CALL_INFO)(OmsTypeInt4  taskId, 
                           OmsTypeInt4* runTime, 
                           OmsTypeInt4* opCount, 
                           OmsTypeInt4* errorCode);
  STDMETHOD(GET_ERROR)(int taskId, OmsTypeInt4* error);
  STDMETHOD(COUNT_TASKS)(OmsTypeInt4* cntTasks);
  //STDMETHOD(CALC_DRYSTONES)(OmsTypeInt4* dryStones);
  STDMETHOD(WAIT_FOR_GARBAGE_COLLECTOR)();
  STDMETHOD(CREATE_CONTAINER)(int containerType, int containerNo, int partitioned);
  STDMETHOD(DROP_CONTAINER)(int containerType, int containerNo);
  STDMETHOD(INIT_SQL)(/*[in]*/ OmsTypeInt4 initRows);
  STDMETHOD(HEAP_TEST)(/*[in]*/ int MemSizeKB, /*[in]*/ int MinChunkSize, /*[in]*/ int MaxCunkSize, /*[in]*/ int loopCnt);
  STDMETHOD(HEAP_SMALL_CHUNKS) (/*[in]*/ int MemSizeKB, /*[in]*/ int loopCnt);
  STDMETHOD(DELETE_VAROBJECTS)();
  STDMETHOD(DEREF_VAROBJECTS)();
  STDMETHOD(CREATE_VAROBJECTS)(/*[in]*/ OmsTypeInt4 noOfObjects, /*[in]*/ OmsTypeInt4 size);
  STDMETHOD(ITER_FORUPD)();
  STDMETHOD(ITER)(OmsTypeInt4 maxNumber, OmsTypeInt4 noOfIter);
  STDMETHOD(OPEN_VERSION)(OmsTypeInt4 vId);
  STDMETHOD(RESET_VERSION)(OmsTypeInt4 vId);
  STDMETHOD(DROP_VERSION)(OmsTypeInt4 vId);
  STDMETHOD(UPDATE_OBJECTS)();
  STDMETHOD(CLOSE_VERSION)(OmsTypeInt4 vId);
  STDMETHOD(CREATE_VERSION)(OmsTypeInt4 vId);
  STDMETHOD(DEREF_KEYED)(/*[in]*/ OmsTypeInt4 cntObjects, /*[in]*/ OmsTypeInt4 startNumber);
  STDMETHOD(CACHED_KEYS_ON)();
  STDMETHOD(CACHED_KEYS_OFF)(); 
  STDMETHOD(DEREFFORUPD)();
  STDMETHOD(GET_RUN_TIME)(/*[out]*/ OmsTypeInt4* runTime);
  STDMETHOD(DEREF)();
  STDMETHOD(MAKE_LIST)();
  STDMETHOD(SET_TASK_ID)(/*[in]*/ int taskId);
  STDMETHOD(CLEAN_UP)();
  STDMETHOD(CREATE_OBJECTS)(/*[in]*/ OmsTypeInt4 cntObjects, /*[out]*/ OmsTypeInt4* milliSecs);
  STDMETHOD(CREATE_KEYED_OBJECTS)(/*[in]*/ OmsTypeInt4 cntObjects, /*[in]*/ OmsTypeInt4 offset);
  STDMETHOD(INSERT_SQL)(/*[in]*/ OmsTypeInt4 noOfRows);
  STDMETHOD(KEY_RANGE_ITER)(OmsTypeInt4 maxNumber, OmsTypeInt4 noOfIter);
  STDMETHOD(GET_CALL_STATISTICS)(OmsMonitorData* info);
  STDMETHOD(LOCK_SHARE)(int);
  STDMETHOD(LOCK_EXCLUSIVE)(int);
  STDMETHOD(UNLOCK)(int);
  STDMETHOD(FILL_ARRAY)(OmsTypeInt4 cntObjects);                           // PTS 1120717
  STDMETHOD(MASS_DEREF)(OmsTypeInt4 cntObjects, OmsTypeInt4 packetSize);   // PTS 1120717 
  STDMETHOD(FILL_ARRAY_KEY)(OmsTypeInt4 cntObjects, OmsTypeInt4 startNumber);  // PTS 1122194
  STDMETHOD(MASS_DEREF_KEY)(OmsTypeInt4 cntObjects, OmsTypeInt4 packetSize);   // PTS 1122194 
  STDMETHOD(INIT_AFTER_EOT)();    // PTS 1128285
  STDMETHOD(CREATE_ARRAY_OBJECTS)(OmsTypeInt4 cntObjects, OmsTypeInt4 arraySize);
  STDMETHOD(DEREF_ARRAY_OBJECTS)();
  STDMETHOD(DELETE_ARRAY_OBJECTS)();
  
  STDMETHOD(Register)();
};

#endif //__lvcbenchOBJ_H_
