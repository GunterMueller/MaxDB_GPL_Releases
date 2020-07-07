
#ifndef __OMS_LiveCacheTypes_h__
#define __OMS_LiveCacheTypes_h__

#ifdef __cplusplus
extern "C"{
#endif


typedef long long OmsTypeInt64;

typedef unsigned long long OmsTypeUInt64;

typedef OmsTypeInt64 OmsTypeInt8;

typedef OmsTypeUInt64 OmsTypeUInt8;

typedef unsigned short DbpTypeUnicode;

typedef char OmsTypeChar;

typedef unsigned char OmsTypeByte;

typedef unsigned char OmsTypeAscii7;

typedef unsigned char OmsTypeUTF8;

typedef unsigned short OmsTypeWyde;

typedef signed char OmsTypeInt1;

typedef unsigned char OmsTypeUInt1;

typedef int OmsTypeInt4;

typedef unsigned int OmsTypeUInt4;

typedef unsigned char OmsTypeBool;

union OmsTypeDouble { 
  double d;
  OmsTypeInt8 int8;
  OmsTypeDouble() {d = 0;}
};

typedef struct OmsTypeABAPTimeStamp { 
#ifdef __cplusplus
   inline OmsTypeABAPTimeStamp(); 
#endif
   unsigned char m_timeStamp[8];
} OmsTypeABAPTimeStamp;

#ifdef __cplusplus
inline OmsTypeABAPTimeStamp::OmsTypeABAPTimeStamp() {
  for ( int i = 0; i < 7; i++ ) {
    m_timeStamp[i] = 0x00;
  }
  m_timeStamp[7] = 0x0c;
}
#endif

typedef unsigned int OmsSchemaHandle;

typedef unsigned int OmsContainerNo;

typedef struct TParamDescriptor { 
   OmsTypeInt4 len;
   unsigned char* val;
} TParamDescriptor;

#ifdef OID_8BYTE_ALIGNED

typedef struct OmsTypeOid4BA { 
   OmsTypeUInt4 pno;
   unsigned short pagePos;
   unsigned short generation;
} OmsTypeOid4BA;

union OmsTypeOid { 
  OmsTypeOid4BA OidAsStruct;
  OmsTypeInt8 OidAs8ByteInt;
};

#else

typedef struct OmsTypeOid { 
   OmsTypeUInt4 pno;
   unsigned short pagePos;
   unsigned short generation;
} OmsTypeOid;

#endif

typedef struct OmsTypePacked_8_3 { 
#ifdef __cplusplus
   inline OmsTypePacked_8_3(); 
#endif
   unsigned char m_packed[8];
} OmsTypePacked_8_3;

#ifdef __cplusplus
inline OmsTypePacked_8_3::OmsTypePacked_8_3() {
  for ( int i = 0; i < 7; i++ ) {
    m_packed[i] = 0x00;
  }
  m_packed[7] = 0x0c;
}
#endif

typedef struct OmsTypePacked_15_3 { 
#ifdef __cplusplus
   inline OmsTypePacked_15_3(); 
#endif
   unsigned char m_packed[15];
} OmsTypePacked_15_3;

#ifdef __cplusplus
inline OmsTypePacked_15_3::OmsTypePacked_15_3() {
  for ( int i = 0; i < 14; i++ ) {
    m_packed[i] = 0x00;
  }
  m_packed[14] = 0x0c;
}
#endif

typedef char OmsTypeNumc;

typedef OmsTypeChar OmsVersionId[22];

typedef struct OmsLockHandle { 
   unsigned char m_handle[8];
} OmsLockHandle;

typedef struct OmsMonitorData { 
   OmsTypeChar m_methodName[32];
   OmsTypeUInt4 m_runTime;
   OmsTypeInt8 m_cntNewObject;
   OmsTypeInt8 m_cntNewKeyedObject;
   OmsTypeInt8 m_cntNewVarObject;
   OmsTypeInt8 m_cntDeref;
   OmsTypeInt8 m_cntDerefLCBase;
   OmsTypeInt8 m_cntKeyDeref;
   OmsTypeInt8 m_cntDerefVarObj;
   OmsTypeInt8 m_cntDerefVarObjLCBase;
   OmsTypeInt8 m_cntOmsStore;
   OmsTypeInt8 m_cntStoreVarObj;
   OmsTypeInt8 m_cntOmsStoreLCBase;
   OmsTypeInt8 m_cntStoreVarObjLCBase;
   OmsTypeInt8 m_cntOmsDelete;
   OmsTypeInt8 m_cntOmsDeleteVarObject;
   OmsTypeInt8 m_cntOmsDeleteLC;
   OmsTypeInt8 m_cntOmsDeleteVarObjectLC;
   OmsTypeInt8 m_cntOmsLock;
   OmsTypeInt8 m_logHops;
   OmsTypeInt8 m_logHopsIter;
   OmsTypeInt8 m_cntExceptions;
   OmsTypeInt8 m_cntOutOfDate;
   OmsTypeInt8 m_cntOutOfMemory;
   OmsTypeInt8 m_cntSubtransRollback;
   OmsTypeInt8 m_cntSubtransCommit;
   OmsTypeInt8 m_maxSubtransLevel;
   OmsTypeInt8 m_cntNewConsistentView;
   OmsTypeInt8 m_avgWaitNewConsView;
   OmsTypeInt8 m_cntOperatorNew;
   OmsTypeUInt4 m_maxChunkAllocated;
   OmsTypeUInt4 m_minChunkAllocated;
   OmsTypeUInt4 m_avgChunkAllocated;
   OmsTypeInt8 m_cntOperatorDelete;
   OmsTypeUInt4 m_maxChunkDeleted;
   OmsTypeUInt4 m_minChunkDeleted;
   OmsTypeUInt4 m_avgChunkDeleted;
   OmsTypeInt8 m_maxHeapUsage;
   OmsTypeInt64 m_memoryDelta;
   OmsTypeInt8 m_OmsLVCBaseCalls;
   OmsTypeUInt4 m_cntSelfSuspends;
   OmsTypeUInt4 m_cntDispatcher;
   OmsTypeUInt4 m_cntIO;
   OmsTypeInt4 m_IORelTime;
   OmsTypeInt4 m_IOAbsTime;
   OmsTypeUInt4 m_cntReplyReceive;
   OmsTypeInt4 m_ReplyReceiveRelTime;
   OmsTypeInt4 m_ReplyReceiveAbsTime;
   OmsTypeUInt4 m_cntReceiveReply;
   OmsTypeInt4 m_ReceiveReplyRelTime;
   OmsTypeInt4 m_ReceiveReplyAbsTime;
   OmsTypeUInt4 m_cntWaits;
   OmsTypeInt4 m_WaitsRelTime;
   OmsTypeInt4 m_WaitsAbsTime;
   OmsTypeUInt4 m_cntSleep;
   OmsTypeInt4 m_SleepRelTime;
   OmsTypeInt4 m_SleepAbsTime;
   OmsTypeInt4 m_OmsReadCacheHit;
   OmsTypeInt4 m_OmsReadCacheMiss;
   OmsTypeInt4 m_OmsReadVersion;
   OmsTypeInt4 m_OmsMaxHashChainLen;
   OmsTypeInt4 m_OmsRehash;
   OmsTypeInt8 m_cntStreamRowsRead;
   OmsTypeInt8 m_cntStreamRowsWritten;
   OmsTypeInt4 m_cntWaitForOmsLockObj;
   OmsTypeInt4 m_avgWaitForOmsLockObj;
   OmsTypeInt4 m_taskId;
} OmsMonitorData;

typedef struct SqlMonitorData { 
   OmsTypeChar m_MethodName[32];
   OmsTypeUInt4 m_Runtime;
   OmsTypeUInt4 m_cntPrepare;
   OmsTypeUInt4 m_cntInsert;
   OmsTypeUInt4 m_cntSelect;
   OmsTypeUInt4 m_cntUpdate;
   OmsTypeUInt4 m_cntDelete;
   OmsTypeUInt4 m_cntFetch;
   OmsTypeUInt4 m_cntDropParseid;
   OmsTypeUInt4 m_cntOther;
   OmsTypeUInt4 m_lenInsertData;
   OmsTypeUInt4 m_lenUpdateData;
   OmsTypeUInt4 m_lenDeleteData;
   OmsTypeUInt4 m_lenFetchData;
} SqlMonitorData;

typedef enum { 
  ABTYPC = 0
, ABTYPDATE = 1
, ABTYPP = 2
, ABTYPTIME = 3
, ABTYPX = 4
, ABTYPTABH = 5
, ABTYPNUM = 6
, ABTYPFLOAT = 7
, ABTYPINT = 8
, ABTYPINT2 = 9
, ABTYPINT1 = 10
, ABTYPW = 11
, ABTYP1 = 12
, ABTYP2 = 13
, ABTYPSTRUC1 = 14
, ABTYPSTRUC2 = 15
, ABTYPREF = 16
, ABTYPOBJ1 = 17
, ABTYPOBJ2 = 18
, ABTYPnull = 19
, ABTYPWYDE = 100
} ABTypes;

typedef struct OmsVersionInfo { 
   OmsTypeByte createDate[8];
   OmsTypeByte createTime[8];
   OmsTypeByte openDate[8];
   OmsTypeByte openTime[8];
   OmsTypeInt4 consistentView;
   OmsTypeBool isMarked;
   OmsTypeBool isOpen;
   OmsTypeBool isUnloaded;
   OmsTypeInt8 heapUsage;
   OmsTypeInt4 hashSize;
} OmsVersionInfo;

typedef struct OmsTypeABAPColDesc { 
   unsigned char inout;
   unsigned char abap_type;
   unsigned short dec;
   unsigned short length;
   unsigned short offset;
} OmsTypeABAPColDesc;

typedef struct OmsTypeABAPTabHandle { 
   OmsTypeInt4 reserved;
   unsigned short rowSize;
   unsigned short colCount;
   OmsTypeInt4 rowCount;
   OmsTypeInt4 ABAPTabId;
   OmsTypeABAPColDesc colDesc[256];
} OmsTypeABAPTabHandle;

typedef enum { 
  STYPE_BOOL = 0
, STYPE_I1 = 1
, STYPE_UI1 = 2
, STYPE_I2 = 3
, STYPE_UI2 = 4
, STYPE_I4 = 5
, STYPE_UI4 = 6
, STYPE_I8 = 7
, STYPE_UI8 = 8
, STYPE_R4 = 9
, STYPE_R8 = 10
, STYPE_INT = 12
, STYPE_UINT = 13
, STYPE_CHAR = 14
, STYPE_WCHAR = 15
, STYPE_UDT = 16
, STYPE_WYDE = 17
} OmsStreamTypes;

typedef struct OmsTypeStreamMemberDesc { 
   unsigned char inout;
   unsigned char memberType;
   unsigned short dec;
   unsigned short length;
   unsigned short offset;
} OmsTypeStreamMemberDesc;

typedef struct OmsTypeStreamHandle { 
   OmsTypeInt4 reserved;
   unsigned short size;
   unsigned short memberCount;
   OmsTypeInt4 filler;
   OmsTypeInt4 ABAPTabId;
   OmsTypeStreamMemberDesc colDesc[256];
} OmsTypeStreamHandle;

#ifdef __cplusplus
}
#endif

#if defined (SQLDBC_TYPES_H) || defined (SQLODBC) || defined (LC_STREAMS_ENABLED)
#include <stdlib.h>
/* returncodes for callbackfunktions */
#define SQL_STREAM_OK 0
#define SQL_STREAM_ERROR (-1)
#define SQL_STREAM_EXCEPTION (-2)
#define SQL_STREAM_NO_MORE_DATA (100)
 
typedef struct _ABAPTab {
  void* nilPointer;
  OmsTypeABAPTabHandle hABAPTab;
} ABAPTabStruct;
 
typedef struct _Stream {
  void* nilPointer;
  OmsTypeStreamHandle hStream;
} StreamStruct;
 
typedef struct _SQL_LC_StreamParm {
  union
  {
    ABAPTabStruct ABAPTab;
    StreamStruct Stream;
  }
#ifndef __cplusplus
  C_1
#endif
;
} SQL_LC_StreamParm;
 
/**
 * @brief Call back function for reading stream data from the client and
 *        sending it to the database.
 *
 *        The stream data is written directly to the order interface and send 
 *        to the database system. The length of the stream data is unlimited 
 *        but it will be send in pieces. For sending some pieces of data to the
 *        database the SQLStreamReadProc function should return
 *        #SQL_ABAP_OK for each call. For the last piece the function should 
 *        return #SQL_ABAP_NO_MORE_DATA.
 *
 * @param TabParm      [in]  The table or stream id to read from.
 * @param rgbOutStream [in]  A pointer to the buffer to where the client has  
 *                           to write the stream data.
 * @param cbStreamMax  [in]  The size in bytes of the rgbOutStream 
 *                           buffer.
 * @param pcbStreamLen [out] The number of bytes written to the 
 *                           rgbOutStream buffer.
 * @param pcbNoOfRowsRead [out] If the stream data is portioned into equal 
 *                           pieces (table rows) the client should returns the 
 *                           number of pieces written to the rgbOutStream buffer.
 * @param pMask [in]         Compression mask.
 *
 * @return  #SQL_ABAP_OK if the rgbOutStream buffer was filled and 
 *          the data is ready to send. The client can send more data to the 
 *          database.
 *          #SQL_ABAP_NO_MORE_DATA if the rgbOutStream buffer was 
 *          filled and the stream processing should be finished.
 *          #SQL_ABAP_EXCEPTION if the call back function runs into an 
 *          unhandled exception during processing the data.
 *          Any other returncode will end the processing of the stream and send *          the returncode to the database. 
 */
typedef int (SQLStreamReadProc)(SQL_LC_StreamParm *TabParm, void *rgbOutStream, int cbStreamMax, int *pcbStreamLen, int *pcbNoOfRowsRead, void* pMask);
 
/**
 * @brief Call back function for getting stream data from database.
 *
 *        The stream data is reading directly from the order interface 
 *        and past to the call back function. 
 *        The length of the stream data is unlimited but it will be read in
 *        pieces. The function is called each time the database has send on 
 *        portion of data.
 *        The SQLStreamWriteProc function should return
 *        #SQL_ABAP_OK after processing the data.
 *
 * @param TabParm     [in] The table or stream id to write to.
 * @param rgbInStream [in] A pointer to the buffer from where the client has  
 *                         to read the stream data.
 * @param cbStreamMax [in] The size in bytes of the rgbInStream 
 *                         buffer.
 * @param cbNoRows   [out] The stream data is portioned into equal pieces
 *                          (table rows) the client has to interpret this
 *                          value to calculate the length of the 
 *                          rgbInStream buffer.
 * @param pMask      [out] Compression mask.
 *
 * @note For reading variable length of data use the extended write call back 
 *       function described later in this document.
 * @return  #SQL_ABAP_OK if the rgbInStream buffer was processed
 *          by the client and the next portion of data may be read from the 
 *          database.
 *          #SQL_ABAP_EXCEPTION if the call back function runs into an 
 *          unhandled exception during processing the data.
 *          Any other returncode will end the processing of the stream and send *          the returncode to the database. 
 */
typedef int (SQLStreamWriteProc)(SQL_LC_StreamParm *TabParm, void *rgbInStream, int cbNoOfRows, int cbStreamLen, void* pMask);
 
typedef struct _SQLStreamDesc {
  SQLStreamReadProc  *ReadProc;
  SQLStreamWriteProc *WriteProc;
  SQL_LC_StreamParm  *StreamParam;   
} SQLStreamDesc;
 

#ifdef __cplusplus
template <class T, long MCOUNT> class CStreamDescriptor : public SQLStreamDesc {
public :
    int currMember;
    void describeMember (OmsStreamTypes memberType, short length) {
        if (currMember < MCOUNT) {
            StreamParam->Stream.hStream.colDesc[currMember].memberType = memberType;
            StreamParam->Stream.hStream.colDesc[currMember].length     = length;
            if (0 == currMember) {
              StreamParam->Stream.hStream.colDesc[currMember].offset = 0;
            }
            else {
              StreamParam->Stream.hStream.colDesc[currMember].offset =
                StreamParam->Stream.hStream.colDesc[currMember-1].offset +
                StreamParam->Stream.hStream.colDesc[currMember-1].length;
            }
            currMember++;
        }
    }
public :
    CStreamDescriptor(long streamId) {
        ReadProc    = 0;
        WriteProc   = 0;
        currMember  = 0;
 if (MCOUNT == 0) {
   /* allocate the maximum if the membercount is not set */
   StreamParam = (SQL_LC_StreamParm*) malloc(sizeof(SQL_LC_StreamParm) - sizeof (OmsTypeStreamMemberDesc) * 256 + sizeof (OmsTypeStreamMemberDesc) * 256);
 }
 else {
   StreamParam = (SQL_LC_StreamParm*) malloc(sizeof(SQL_LC_StreamParm) - sizeof (OmsTypeStreamMemberDesc) * 256 + sizeof (OmsTypeStreamMemberDesc) * MCOUNT);
 }
        if (StreamParam) {
            StreamParam->Stream.nilPointer          = (void*) 0;
            StreamParam->Stream.hStream.reserved    = 0;
            StreamParam->Stream.hStream.ABAPTabId   = streamId;
            StreamParam->Stream.hStream.size        = sizeof(T);
            StreamParam->Stream.hStream.memberCount = MCOUNT;
        }
    }
 

    void Describe(short x)                   { describeMember (STYPE_I2,  sizeof(x)); }
    void Describe(unsigned short x)          { describeMember (STYPE_UI2, sizeof(x)); }
    void Describe(OmsTypeInt4 x)             { describeMember (STYPE_I4,  sizeof(x)); }
    void Describe(OmsTypeUInt4 x )           { describeMember (STYPE_UI4, sizeof(x)); }
    void Describe(float x)                   { describeMember (STYPE_R4,  sizeof(x)); }
    void Describe(double x)                  { describeMember (STYPE_R8,  sizeof(x)); }
    void Describe(OmsTypeInt64 x)            { describeMember (STYPE_I8,  sizeof(x)); }
    void Describe(OmsTypeUInt64 x)           { describeMember (STYPE_UI8, sizeof(x)); }
    void Describe(const char* x, short size) { describeMember (STYPE_CHAR, size); }
    void Describe(OmsTypeByte* x, short size) { describeMember (STYPE_CHAR, size); }
 
protected :
    ~CStreamDescriptor() {
        if (StreamParam)
            free (StreamParam);
    }
};
 
template < class T, long MCOUNT> class COStreamDescriptor : public CStreamDescriptor<T,MCOUNT> {
public :
    COStreamDescriptor(long streamId, SQLStreamWriteProc pWriteProc) : 
        CStreamDescriptor<T,MCOUNT>(streamId) 
    {
        CStreamDescriptor<T,MCOUNT>::WriteProc = pWriteProc;
    }
};
 
template <class T, long MCOUNT> class CIStreamDescriptor : public CStreamDescriptor<T,MCOUNT> {
public :
    CIStreamDescriptor(long streamId, SQLStreamReadProc pReadProc) : 
        CStreamDescriptor<T,MCOUNT>(streamId) 
    {
        CStreamDescriptor<T,MCOUNT>::ReadProc = pReadProc;
    }
}; 
#endif
#define SQL_ABAP_OK           SQL_STREAM_OK
#define SQL_ABAP_ERROR        SQL_STREAM_ERROR
#define SQL_ABAP_EXCEPTION    SQL_STREAM_EXCEPTION
#define SQL_ABAP_NO_MORE_DATA SQL_STREAM_NO_MORE_DATA
#define SQLABAPReadProc       SQLStreamReadProc
#define SQLABAPWriteProc      SQLStreamWriteProc
#define SQLABAPDesc           SQLStreamDesc
#define SQL_LC_ABAPTabParm    SQL_LC_StreamParm
#endif

#endif

