/****************************************************************************

  module      : OMS_Stream.cpp

  -------------------------------------------------------------------------

  responsible : ThomasA

  special area: OMS 
  description : OMS streams

  last changed: 2000-07-11  13:54
  see also    : example.html ...
  first created:2000-05-26  19:32

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2000-2005 SAP AG




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




*****************************************************************************/

#include "Oms/OMS_Stream.hpp"
#include "Oms/OMS_SessionDef.hpp"
#include "Oms/OMS_StreamRegistry.hpp"
#include "Oms/OMS_Defines.h"
#include "Oms/OMS_Types.hpp"
#include "Oms/OMS_Globals.hpp"
#include "Oms/OMS_Handle.hpp"
#include "Oms/OMS_Session.hpp"
#include "Oms/OMS_Trace.hpp"
#include "Oms/OMS_ObjectContainer.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "hsp20.h"
#include <new.h>

static bool cIsInStream = true;

static SAPDB_Byte cZeroPacked_8_3[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c };
static SAPDB_Byte cZeroPacked_15[]  = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c };

/*!
   @brief defines a buffer for a chain containing rows of an input stream
 */
class OMS_StreamBufferChunk
{
public :
    /*!
      @brief creates a buffer
     */
    static OMS_StreamBufferChunk* Create(OMS_Session *pSession, int rowSize)
    {
        int bufferSize = (rowSize > DEFAULT_BUFFER_SIZE) ? rowSize : DEFAULT_BUFFER_SIZE;
        return new(pSession->allocate(sizeof(OMS_StreamBufferChunk) - DEFAULT_BUFFER_SIZE + bufferSize)) 
            OMS_StreamBufferChunk(bufferSize / rowSize);
    }
    static OMS_StreamBufferChunk* Create(OMS_Session *pSession, const void* pBuf, int BufSize, int RowCount)
    {
        OMS_StreamBufferChunk* pChunk = new(pSession->allocate(sizeof(OMS_StreamBufferChunk) - DEFAULT_BUFFER_SIZE + BufSize)) 
            OMS_StreamBufferChunk(RowCount);
        SAPDB_MemCopyNoCheck(&pChunk->m_buffer[0], pBuf, BufSize);
        return pChunk;
    }
    /*!
       @brief decrements the number of rows in the buffer
     */
    void DecrementCount()
    {
        --m_cnt;
    }
    /*!
      @brief returns a pointer to the first row
     */
    SAPDB_Byte* FirstPos()
    {
        return &m_buffer[0];
    }
    /*!
       @brief returns a pointer to the rows following the last row
     */
    SAPDB_Byte* FirstPos(int rowSize)
    {
        return &m_buffer[m_cnt * rowSize];
    }
    /*!
       @brief returns the number of rows stored inside the buffer
     */
    int GetCount() const
    {
        return m_cnt;
    }
    /*!
       @brief returns a pointer to the next buffer
     */
    OMS_StreamBufferChunk* GetNext() const
    {
        return m_next;
    }
    /*!
       @brief returns the address of the pointer pointing to the next buffer
     */
    OMS_StreamBufferChunk** GetNextAddr()
    {
        return &m_next;
    }
    /*!
       @brief sets the number of rows stored inside the buffer
     */
    void SetCount(int count)
    {
        m_cnt += count;
    }
private :
    enum { DEFAULT_BUFFER_SIZE = 8000 }; //!< default buffer size
    /*!
       @brief constructor, creates an empty buffer
     */
    OMS_StreamBufferChunk(int RowCount)
        : m_next(NULL)
        , m_cnt(RowCount)
    {
    }
    OMS_StreamBufferChunk* m_next;                        //!< pointer to the next buffer
    SAPDB_Int4             m_cnt;                         //!< number of rows currently stored inside the buffer
    SAPDB_Byte             m_buffer[DEFAULT_BUFFER_SIZE]; //!< may be greater in reality
};

/*----------------------------------------------------------------------*/

/*!
   @brief describes stream properties for the callback function (sink) 
 */
class OMS_StreamDescriptor : public tgg01_StreamDescriptor 
{
public :
    /*!
       @brief constructor, describes given stream 
     */
     OMS_StreamDescriptor (OMS_StreamBody* pBody, SAPDB_Int4 StreamId);
     /*!
        @brief appends the descriptor by another stream
      */
     inline bool Append (OMS_StreamBody* pBody, SAPDB_Int4 StreamId);
     /*!
        @brief returns the stream at index Idx
      */
     OMS_StreamBody* GetBody(int Idx) const
     {
         return reinterpret_cast<OMS_StreamBody* >(sdDesc[Idx].osdHandle );
     }
     /*!
        @brief returns the buffer address of the buffer containing rows of the stream at index Idx
      */
     const SAPDB_Byte* GetBufAddr(int Idx) const
     {
         return reinterpret_cast<const SAPDB_Byte*>(sdDesc[Idx].osdBufPtr);
     }
     /*!
        @brief returns the buffer size of the buffer containing rows of the stream at index Idx
      */
     int GetBufSize(int Idx) const
     {
         return sdDesc[Idx].osdBufSize;
     }
     /*!
       @brief returns the number of streams described by the descriptor
      */
     int GetNoOfDescriptions() const
     {
         return sdCount;
     }
     /*!
        @brief returns the number of streams rows inside the buffer 
      */
     int GetRowCount(int Idx) const
     {
         return sdDesc[Idx].osdRowCount; 
     }
     /*!
        @brief returns the stream id 
      */
     int GetStreamId(int Idx) const
     {
         return sdDesc[Idx].osdStreamId; 
     }
     /*!
        @brief returns true, if the end of the input stream has been reached 
      */
     bool IsEndOfStream(int Idx) const
     {
         return (sdDesc[Idx].osdEndOfStream != 0);
     }
};

/*----------------------------------------------------------------------*/

class OMS_StreamBody : public OMS_StreamRegistry::List
{
public:
    OMS_StreamBody(OmsHandle& h, OmsTypeABAPTabHandle& tabHandle, int rowSize, bool isInStream);
    ~OMS_StreamBody();
    void               AllocInStreamBuffer();
    void               AllocOutStreamBuffer();
    SAPDB_Byte*        BuildCurrentRow();
    inline static void CopyAndSwapI2(const SAPDB_Byte* pSource, SAPDB_Byte* pDest);
    inline static void CopyAndSwapI4(const SAPDB_Byte* pSource, SAPDB_Byte* pDest);
    inline static void CopyAndSwapI8(const SAPDB_Byte* pSource, SAPDB_Byte* pDest);
    bool               DefaultCompressionEnabled() const { return m_compressDefaults; }
    void               DisableDefaultCompression(); /* PTS 1135058, PG */
    static int         EvalSetSize(int MemberCount) { return (MemberCount + 7) >> 3; }
    static int         EvalHidearraySize(int MemberCount) { return (MemberCount >> 3) + 1; } /* PTS 1135058, PG */
    void               Flush();
    void               FlushBytes();
    int                GetClientRowSize() const { return m_clientRowSize; }
    int                GetColumnCount() const { return m_streamDescriptor.colCount;}
    SAPDB_Byte*        GetHiddenColumns() const { return m_hiddenColumns; }
    SAPDB_Byte*        GetOutBufferAddr() const { return m_outBuffer; }
    int                GetRowCount() const { return m_rowCount; }
    SAPDB_Byte*        GetNext();
    IliveCacheSink*    GetSink() const { return  m_handle.m_pSession->m_lcSink; }
    void               Hide(unsigned int ColumnNo);
    void               IncopyStream(OMS_StreamDescriptor& descriptor, int descriptorIndex);
    void               PutDefaultValue (const OmsTypeABAPColDesc&, SAPDB_Byte* pDest);
    SAPDB_UInt4        ReadBytes(unsigned char *buf, SAPDB_UInt4 bufLen);
    void               Reset(); /* PTS 1131114, PG */
    inline void        ResetEndOfStream(); /* PTS 1131114, PG */
    inline void        SetEndOfStream();
    inline void        SetRegFlag(bool); /* PTS 1131114, PG */
    inline void        SetInuseFlag(bool); /* PTS 1135058, PG */
    void               Write(SAPDB_Byte* pObj);
    void               WriteBytes(const SAPDB_Byte* pObj, SAPDB_UInt4 bufLen);
    void               WriteCompressed(SAPDB_Byte* pObj);
    const OmsHandle    &getHandle() const
    {
      return m_handle;
    }
private:
    bool IsDefaultBitSet(int Idx)
    {
        if (m_compressDefaults)
        {
            return (m_curr[Idx >> 3] & (1 << (Idx & 7))) != 0;
        }
        return false;
    }

    bool IsDefaultValue (const OmsTypeABAPColDesc&, const SAPDB_Byte* pMember) const;

    bool IsHiddenColumn(int ColumnIdx) const
    {
        return (m_hiddenColumns && ((m_hiddenColumns[ColumnIdx >> 3] & (1 << (ColumnIdx & 7))) != 0));
    }

    int DefaultSetSize() const
    {
        if (m_compressDefaults)
        {
            return EvalSetSize (m_streamDescriptor.colCount);
        }
        return 0;
    }
    void SetBit (int columnNo, SAPDB_Byte* pSet)
    {
        pSet[columnNo >> 3] |= (1 << (columnNo & 7));
    }
    OmsHandle&             m_handle;
    SAPDB_Int4             m_rowSize;
    SAPDB_Int4             m_clientRowSize;
    SAPDB_Int4             m_rowCount;
    SAPDB_Int4             m_maxOutCount;
    int                    m_lowByte;
    int                    m_highByte;
    bool                   m_eot;
    bool                   m_isInStream;
    bool                   m_isAbapStream;
    bool                   m_compressDefaults;
    bool                   m_defaultCompressionSwitchable; /* PTS 1135058, PG */
    bool                   m_hiddenColumnsSupport;
    OmsTypeABAPTabHandle&  m_streamDescriptor;   
    OMS_StreamBufferChunk* m_firstChunk;
    OMS_StreamBufferChunk* m_lastChunk;
    SAPDB_Byte*            m_curr;
    SAPDB_Byte*            m_outBuffer;
    SAPDB_Byte*            m_hiddenColumns;
    const SAPDB_Byte*      m_ReadBytesBuf;
    SAPDB_Int4             m_ReadBytesBufSize;
    SAPDB_UInt4            m_ReadBytesBufOffset;
    SAPDB_UInt4            m_outBufferSize;
    SAPDB_UInt4            m_outBufferLength;
    OMS_StreamRegistry&    m_registry;
    bool                   m_isRegistered; /* PTS 1131114, PG */
    bool                   m_isInUse; /* PTS 1135058, PG */
};

/*----------------------------------------------------------------------*/

OMS_StreamDescriptor::OMS_StreamDescriptor (OMS_StreamBody* pBody, SAPDB_Int4 StreamId)
{
    sdCount              = 0;
    /* sdDefaultCompression = pBody->DefaultCompressionEnabled(); PTS 1135058, PG */
    this->Append(pBody, StreamId);
}

/*----------------------------------------------------------------------*/

inline bool OMS_StreamDescriptor::Append (OMS_StreamBody* pBody, SAPDB_Int4 StreamId)
{
    if (sdCount < MAX_STREAM_DESC_gg01)
    {
        OMS_TRACE (omsTrStream, pBody->GetSink(), "Append " << StreamId << " client size " 
            << pBody->GetClientRowSize() << " rowCount " << pBody->GetRowCount());
        sdDesc[sdCount].osdHandle             = reinterpret_cast<tgg00_VoidPtr>(pBody);
        sdDesc[sdCount].osdStreamId           = StreamId;
        sdDesc[sdCount].osdRowSize            = pBody->GetClientRowSize();
        sdDesc[sdCount].osdRowCount           = pBody->GetRowCount();
        sdDesc[sdCount].osdBufPtr             = pBody->GetOutBufferAddr();
        sdDesc[sdCount].osdBufSize            = 0;
        sdDesc[sdCount].osdResetStream        = false; /* PTS 1131114, PG */
        sdDesc[sdCount].osdHiddenDesc         = pBody->GetHiddenColumns();
        sdDesc[sdCount].osdHiddenDescSize     = (sdDesc[sdCount].osdHiddenDesc) ? OMS_StreamBody::EvalHidearraySize(pBody->GetColumnCount()) : 0; /* PTS 1135058, PG */
        sdDesc[sdCount].osdDefaultCompression = pBody->DefaultCompressionEnabled(); /* PTS 1135058, PG */
        pBody->SetInuseFlag(true);                                                  /* PTS 1135058, PG */
        ++sdCount;
        return true;
    }
    return false;
}

/*----------------------------------------------------------------------*/

OMS_StreamRegistry::OMS_StreamRegistry(bool ClientSupportsOptimizedProtocol)
: m_disabled(!ClientSupportsOptimizedProtocol)
{
    // initialize list of output streams
    new((void*) &m_head[false]) List(true);
    // initialize list of input streams
    new((void*) &m_head[true]) List(true);
}

/*----------------------------------------------------------------------*/

OMS_StreamRegistry::~OMS_StreamRegistry()
{
    bool IsInStream = false;
    // clear lists of out and in streams
    for (int ix = 0; ix < 2; ++ix)
    {
        const OMS_StreamBody* end = this->End(IsInStream); 
        for (OMS_StreamBody* iter = this->Begin(IsInStream); iter != end; iter = iter->GetNextStream())
        {
            iter->SetNextStream(0);
        }
        IsInStream = true;
    }
}

/*----------------------------------------------------------------------*/

OMS_StreamBody* OMS_StreamRegistry::Begin(bool IsInStream) const
{
    return (m_disabled) ? const_cast<OMS_StreamBody*>(End(IsInStream)) : m_head[IsInStream].GetNextStream();
}

/*----------------------------------------------------------------------*/

const OMS_StreamBody* OMS_StreamRegistry::End(bool IsInStream) const
{
    return reinterpret_cast<const OMS_StreamBody*>(&m_head[IsInStream]);
}

/*----------------------------------------------------------------------*/

void OMS_StreamRegistry::Enter (OMS_StreamBody* p, bool IsInStream)
{
    if (!m_disabled)
    {
        p->SetPrevStream(reinterpret_cast<OMS_StreamBody*>(&m_head[IsInStream]));
        p->SetNextStream(m_head[IsInStream].GetNextStream());
        m_head[IsInStream].GetNextStream()->SetPrevStream(p);
        m_head[IsInStream].SetNextStream(p);
        p->SetRegFlag(true); /* PTS 1131114, PG */
    }
};

/*----------------------------------------------------------------------*/

void OMS_StreamRegistry::Remove (OMS_StreamBody* p)
{
    OMS_StreamBody* pNext = p->GetNextStream();
    if (pNext)
    {
        p->GetPrevStream()->SetNextStream(pNext);
        pNext->SetPrevStream(p->GetPrevStream());
        p->SetNextStream(0);
        p->SetPrevStream(0);
        p->SetRegFlag(false); /* PTS 1131114, PG */
    }
};

/*----------------------------------------------------------------------*/

OMS_StreamRegistry::List::List(bool IsHead)
{
    if (IsHead)
    {
        m_prev = reinterpret_cast<OMS_StreamBody*>(this);
        m_next = reinterpret_cast<OMS_StreamBody*>(this);
    }
    else
    {
        m_prev = 0;
        m_next = 0;
    }
}

/*----------------------------------------------------------------------*/

OMS_StreamBody* OMS_StreamRegistry::List::GetNextStream() const
{
    return m_next;
}

/*----------------------------------------------------------------------*/

OMS_StreamBody* OMS_StreamRegistry::List::GetPrevStream() const
{
    return m_prev;
}

/*----------------------------------------------------------------------*/

void OMS_StreamRegistry::List::SetNextStream(OMS_StreamBody* pStream)
{
    m_next = pStream;
}

/*----------------------------------------------------------------------*/

void OMS_StreamRegistry::List::SetPrevStream(OMS_StreamBody* pStream)
{
    m_prev = pStream;
}

/*----------------------------------------------------------------------*/

inline void OMS_StreamBody::CopyAndSwapI2(const SAPDB_Byte* pSource, SAPDB_Byte* pDest)
{
    *pDest     = *(pSource+1);
    *(pDest+1) = *pSource;
}

/*----------------------------------------------------------------------*/

inline void OMS_StreamBody::CopyAndSwapI4(const SAPDB_Byte* pSource, SAPDB_Byte* pDest)
{
    for (int ix = 0; ix < 4; ++ix)
    {
        pDest[ix] = pSource[3-ix];
    }
}

/*----------------------------------------------------------------------*/

inline void OMS_StreamBody::CopyAndSwapI8(const SAPDB_Byte* pSource, SAPDB_Byte* pDest)
{
    for (int ix = 0; ix < 8; ++ix)
    {
        pDest[ix] = pSource[7-ix];
    }
}

/*----------------------------------------------------------------------*/

inline void OMS_StreamBody::SetEndOfStream()
{
    m_registry.Remove(this);
    m_eot = true;
}

/*----- PTS 1131114, PG ------------------------------------------------*/

inline void OMS_StreamBody::ResetEndOfStream()
{
    if (!m_isRegistered)
    {
        m_registry.Enter( this, m_isInStream );
    }
    m_eot = false;
}

/*----- PTS 1131114, PG ------------------------------------------------*/

inline void OMS_StreamBody::SetRegFlag(bool isRegistered)
{
    m_isRegistered = isRegistered;
}

/*----- PTS 1135058, PG ------------------------------------------------*/

inline void OMS_StreamBody::SetInuseFlag(bool isInUse)
{
    m_isInUse = isInUse;
}

/*----------------------------------------------------------------------*/

// meaning of tabHandle.rowCount
//  0  rows, no default value compression
// -1  no rows, no default value compression
// -2  rows, default value compression
// -3  no rows, default value compression
// -4  rows, default value compression switchable, default = on     (PTS 1135058, PG)
// -5  no rows, default value compression switchable, default = on  (PTS 1135058, PG)

OMS_StreamBody::OMS_StreamBody(OmsHandle& h, OmsTypeABAPTabHandle& tabHandle, int rowSize, bool isInStream)
: m_handle(h)
, m_rowSize(rowSize)
, m_clientRowSize(tabHandle.rowSize)
, m_rowCount(0)
, m_maxOutCount(0)
, m_lowByte(0)
, m_highByte(0)
, m_eot(isInStream && (-1 == tabHandle.rowCount || -3 == tabHandle.rowCount || -5 == tabHandle.rowCount)) /* PTS 1135058, PG */
, m_isInStream(isInStream)
, m_isAbapStream(tabHandle.rowCount != 0)
, m_compressDefaults(-2 == tabHandle.rowCount || -3 == tabHandle.rowCount ||
                     -4 == tabHandle.rowCount || -5 == tabHandle.rowCount)             /* PTS 1135058, PG */
, m_defaultCompressionSwitchable(-4 == tabHandle.rowCount || -5 == tabHandle.rowCount) /* PTS 1135058, PG */
, m_hiddenColumnsSupport(m_handle.m_pSession->ClientSupportsOptimizedStreams() && m_compressDefaults) 
, m_streamDescriptor(tabHandle)
, m_firstChunk(NULL)
, m_lastChunk(NULL)
, m_curr(NULL)
, m_hiddenColumns(NULL)
, m_outBuffer(NULL)
, m_ReadBytesBuf(0)
, m_ReadBytesBufSize(0)
, m_ReadBytesBufOffset(0)
, m_registry(h.m_pSession->GetStreamRegistry())
, m_isRegistered(false) /* PTS 1131114, PG */
, m_isInUse(false) /* PTS 1135058, PG: false indicates that no action was yet performed on stream by user */
{
    if (tabHandle.colCount > 0)
    {
        // find out byte order for swapping
        for (int colIdx = 0; colIdx < m_streamDescriptor.colCount; ++colIdx)
        {
            if (m_streamDescriptor.colDesc[colIdx].inout >= OMS_SwapAndModifyInfo::asciiClientWydeCpp)
            {
                if (OMS_SwapAndModifyInfo::unicodeClientAsciiCpp == m_streamDescriptor.colDesc[colIdx].inout)
                {
                    m_lowByte  = 1;
                    break;
                }
                else
                {
                    if (OMS_SwapAndModifyInfo::swappedUnicodeClientAsciiCpp == m_streamDescriptor.colDesc[colIdx].inout)
                    {
                        m_highByte = 1;
                        break;
                    }
                }
            }
        }
    }
    if (!isInStream)
    {
        this->AllocOutStreamBuffer();
    }
    else
    {
        this->AllocInStreamBuffer();
        m_registry.Enter(this, isInStream);
    }
}

/*----------------------------------------------------------------------*/

OMS_StreamBody::~OMS_StreamBody()
{
    m_registry.Remove(this);
    if (m_isInStream)
    {
        while (NULL != m_firstChunk)
        {
            void* toFree = m_firstChunk;
            m_firstChunk = m_firstChunk->GetNext();
            m_handle.m_pSession->deallocate(toFree);
        }
    }
    m_handle.m_pSession->deallocate(m_outBuffer);
    m_handle.m_pSession->deallocate(m_hiddenColumns);
}

/*----------------------------------------------------------------------*/

void OMS_StreamBody::AllocInStreamBuffer()
{
  m_outBuffer = REINTERPRET_CAST(SAPDB_Byte*, m_handle.m_pSession->allocate(m_rowSize));
}

/*----------------------------------------------------------------------*/

void OMS_StreamBody::AllocOutStreamBuffer()
{
    m_outBufferSize            = OMS_Globals::GetKernelInterface()->GetPacketMaxUsableArea();
    m_outBuffer                = REINTERPRET_CAST(SAPDB_Byte*, m_handle.m_pSession->allocate(m_outBufferSize));
    m_maxOutCount              = m_outBufferSize / m_clientRowSize; 
    m_maxOutCount              = (m_maxOutCount < 32767) ? m_maxOutCount : 32767; 
    m_rowCount                 = 0;
    m_curr                     = m_outBuffer;
    m_outBufferLength          = 0;
}


/*----------------------------------------------------------------------*/

SAPDB_Byte* OMS_StreamBody::BuildCurrentRow()
{
    // translate struct from client layout into c++ layout
    register SAPDB_Byte* pSource = m_curr + DefaultSetSize();
    for (int descIdx = 0; descIdx < m_streamDescriptor.colCount; ++descIdx)
    {
        OmsTypeABAPColDesc& abapDesc = m_streamDescriptor.colDesc[descIdx];
        register SAPDB_Byte* pDest   = m_outBuffer + abapDesc.dec;
        if (IsDefaultBitSet(descIdx) || IsHiddenColumn(descIdx) /* PTS 1133741 */)
        {
            PutDefaultValue (abapDesc, pDest);
        }
        else
        {
            if (!m_compressDefaults)
            {
                pSource = m_curr + abapDesc.offset;
            }
            // copy and transform one struct member
            switch (abapDesc.inout) {
                case OMS_SwapAndModifyInfo::swap_2 :
                    {
                        // swap 2 bytes integer(s)
                        // PTS 1132704, PG
                        int intCnt = 0;
                        if (abapDesc.abap_type == ABTYPINT2)
                        {
                            intCnt = 1; // == abapDesc.length/2
                        }
                        else // ABTYPWYDE
                        {
                            intCnt = abapDesc.length;
                        }
                        for (int l = 0; l < intCnt; ++l)
                        {
                            CopyAndSwapI2(pSource, pDest);
                            pSource += 2;
                            pDest   += 2;
                        }
                        break;
                    }
                case OMS_SwapAndModifyInfo::swap_4 :
                    {
                        // swap 4 byte integer
                        CopyAndSwapI4(pSource, pDest);
                        pSource += 4;
                        break;
                    }
                case OMS_SwapAndModifyInfo::swap_8 :
                    {
                        // swap 8 byte datatypes (int8, double)
                        CopyAndSwapI8(pSource, pDest);
                        pSource += 8;
                        break;
                    }
                case OMS_SwapAndModifyInfo::asciiClientWydeCpp :
                    {
                        // expand 1 byte character to wyde
                        OmsTypeWyde* pWyde = REINTERPRET_CAST(OmsTypeWyde*, pDest);
                        for (int l = 0; l < abapDesc.length; ++l)
                        {
                            *pWyde = *pSource;
                            ++pWyde;
                            ++pSource;
                        }
                        break;
                    }
                case OMS_SwapAndModifyInfo::unicodeClientAsciiCpp       :
                case OMS_SwapAndModifyInfo::swappedUnicodeClientAsciiCpp :
                    {
                        // translate UCS2 to ASCII7
                        for (int l = 0; l < abapDesc.length; ++l)
                        {
                            if ((0 != *(pSource + m_highByte)) || (*(pSource + m_lowByte) > 127))
                            {
                                // unicode char does not fit into ascii7
                                DbpBase base(m_handle.m_pSession->m_lcSink);
                                base.dbpOpError("unicode not translatable to ascii7 : %d", *(pSource + m_highByte) * 256 + *(pSource + m_lowByte));
                                base.dbpOpError("offset of member is %d bytes", abapDesc.offset);
                                base.dbpOpError("index  in member is %d", l);
                                tsp00_Int4 bufLength = sizeof(m_streamDescriptor.ABAPTabId);
                                tsp00_Int2 dummyError;
                                m_handle.m_pSession->m_lcSink->MultiPurpose (m_return_error, mm_nil, 
                                    &bufLength, reinterpret_cast<unsigned char*>(&m_streamDescriptor.ABAPTabId), &dummyError);
                                m_handle.m_pSession->ThrowDBError (e_OMS_not_translatable, "OMS_StreamBody::IncopyStream", __MY_FILE__, __LINE__);
                            }
                            *pDest = *(pSource + m_lowByte);
                            ++pDest;
                            pSource += 2;
                        }
                        break;
                    }
                default :
                    {
                        // just copy without any translation 
                        SAPDB_MemCopyNoCheck(pDest, pSource, abapDesc.length);
                        pSource += abapDesc.length;
                    }
            } // switch
        }
    } // for every member
    if (m_compressDefaults)
    {
        m_curr = pSource;
    }
    else
    {
        m_curr += m_clientRowSize;
    }
    m_firstChunk->DecrementCount();
    --m_rowCount;
    return &m_outBuffer[0];
}

/*----------------------------------------------------------------------*/

void OMS_StreamBody::Hide(unsigned int ColumnNo)
{
    m_isInUse = true; /* PTS 1135058, PG */

    if (m_hiddenColumnsSupport)
    {
        if (!m_isInStream && (0 != m_rowCount))
        {
            // already rows in buffer ==> hide not possible
            return;
        }
        if (0 == m_hiddenColumns)
        {
            SAPDB_UInt requiredSize = EvalHidearraySize(m_streamDescriptor.colCount); /* PTS 1135058, PG */
            m_hiddenColumns = reinterpret_cast<SAPDB_Byte*>(m_handle.m_pSession->allocate(requiredSize));
            for (SAPDB_UInt ix = 0; ix < requiredSize; ++ix)
            {
                m_hiddenColumns[ix] = 0x00;
            }
        }
        this->SetBit (ColumnNo, m_hiddenColumns);
    }
}

/*----------------------------------------------------------------------*/


SAPDB_Byte* OMS_StreamBody::GetNext()
{
    m_isInUse = true; /* PTS 1135058, PG */

    if (!m_handle.m_pSession->m_stream_io) 
    {
        return NULL;
    }
    m_handle.m_pSession->IncReadStreamRow();
    if (0 == m_rowCount)
    {
        while (NULL != m_firstChunk)
        {
            void* toFree = m_firstChunk;
            m_firstChunk = m_firstChunk->GetNext();
            m_handle.m_pSession->deallocate(toFree);
        }
        if (m_eot) 
        {
            return NULL;
        }
        SAPDB_Int2  DBError;
        OMS_StreamDescriptor streamDesc(this, m_streamDescriptor.ABAPTabId);
        for (OMS_StreamBody* iter = m_registry.Begin(cIsInStream); iter != m_registry.End(cIsInStream); iter = iter->GetNextStream())
        {
            if (iter != this && !iter->m_eot)
            {
                if (!streamDesc.Append (iter, iter->m_streamDescriptor.ABAPTabId))
                {
                    break;
                }
            }
        }
        m_handle.m_pSession->m_lcSink->ABAPRead (streamDesc, DBError);
        m_handle.m_pSession->IncReadStreamBuffer();
        if ( DBError != 0 )
        {
            m_handle.m_pSession->m_stream_io = false;
            m_handle.m_pSession->ThrowDBError (DBError, "omsNxt", __MY_FILE__, __LINE__);
        }
        for (int ix = 0; ix < streamDesc.GetNoOfDescriptions(); ++ix)
        {
            OMS_StreamBody* pCurrStream = streamDesc.GetBody(ix);
            pCurrStream->IncopyStream(streamDesc, ix);
        }
        if ((0 == streamDesc.GetRowCount(0)) && m_eot)
        {
            return NULL;
        }
    }
    else
    {
        if (0 == m_firstChunk->GetCount())
        {
            // current chunk has been traversed completely, move to next one
            // and release current chunk
            void* toFree = m_firstChunk;
            m_firstChunk = m_firstChunk->GetNext();
            m_handle.m_pSession->deallocate(toFree);
            m_curr = m_firstChunk->FirstPos();
        }
    }
    return BuildCurrentRow();
}

/*----- PTS 1131114, PG ------------------------------------------------*/

void OMS_StreamBody::Reset()
{

    m_isInUse = true; /* PTS 1135058, PG */

    SAPDB_Int2 DBError;

    if (!m_handle.m_pSession->m_stream_io) 
    {
        return;
    }

    if (m_isInStream)
    {
        /* check if resetting input streams is supported by client */
        /* PTS 1135058: modification was accidentally deleted with CL 96214 */
        /* PTS 1134818; we assume now that resetting streams is supported by client */
        //if (!m_compressDefaults)
        //{
        //    DBError = e_reset_istream_not_supported;
        //    m_handle.m_pSession->ThrowDBError( DBError, "omsReset", __MY_FILE__, __LINE__ );
        //}

        /* clear local buffer */
        m_rowCount = 0;
        while (NULL != m_firstChunk)
        {
            void* toFree = m_firstChunk;
            m_firstChunk = m_firstChunk->GetNext();
            m_handle.m_pSession->deallocate(toFree);
        }

        /* if end of stream was set, reset */
        if (m_eot)
        {
            this->ResetEndOfStream();
        }

        /* request again first package from client */
        OMS_StreamDescriptor streamDesc( this, m_streamDescriptor.ABAPTabId );
        streamDesc.sdDesc[0].osdResetStream  = true;
        m_handle.m_pSession->m_lcSink->ABAPRead( streamDesc, DBError );
        m_handle.m_pSession->IncReadStreamBuffer();
        if (DBError != 0)
        {
            m_handle.m_pSession->m_stream_io = false;
            m_handle.m_pSession->ThrowDBError( DBError, "omsIReset", __MY_FILE__, __LINE__ );
        }
        IncopyStream( streamDesc, 0 );
    }

    else
    {
        /* check if resetting output streams is supported by client */
        /* PTS 1135058: modification was accidentally deleted with CL 96214 */
        /* PTS 1134818; we assume now that resetting streams is supported by client */
        //if ( !m_compressDefaults || !m_handle.m_pSession->ClientSupportsOptimizedStreams() )
        //{
        //    DBError = e_reset_ostream_not_supported;
        //    m_handle.m_pSession->ThrowDBError( DBError, "omsReset", __MY_FILE__, __LINE__ );
        //}

        /* reset local buffer */
        m_outBufferLength = 0;
        m_rowCount        = 0;
        m_curr            = m_outBuffer;
        OMS_StreamDescriptor streamDesc( this, m_streamDescriptor.ABAPTabId );
        streamDesc.sdDesc[0].osdResetStream  = true;
        streamDesc.sdDesc[0].osdDefaultCompression = false; /* PTS 1135058, PG */
        if (m_isRegistered) {
          m_registry.Remove(this);
        }

        /* signal client restart of sending data */
        m_handle.m_pSession->m_lcSink->ABAPWrite( streamDesc, DBError );
        m_handle.m_pSession->IncWriteStreamBuffer();
        if (DBError != 0)
        {
            m_handle.m_pSession->m_stream_io = false;
            m_handle.m_pSession->ThrowDBError( DBError, "omsOReset", __MY_FILE__, __LINE__ );
        }
    }

}

/*----------------------------------------------------------------------*/

void OMS_StreamBody::IncopyStream(OMS_StreamDescriptor& descriptor, int descriptorIndex)
{
    if (descriptor.IsEndOfStream(descriptorIndex))
    {
        this->SetEndOfStream();
    }
    if (descriptor.GetRowCount(descriptorIndex) > 0)
    {
        bool                    isFirst;
        OMS_StreamBufferChunk** pPrev;
        if (0 == m_firstChunk)
        {
            isFirst = true;
            pPrev   = &m_firstChunk;
        }
        else
        {
            isFirst = false;
            pPrev   = m_lastChunk->GetNextAddr();
        }
        *pPrev = OMS_StreamBufferChunk::Create(m_handle.m_pSession, descriptor.GetBufAddr(descriptorIndex), 
            descriptor.GetBufSize(descriptorIndex), descriptor.GetRowCount(descriptorIndex));
        m_lastChunk = *pPrev;
        m_rowCount += descriptor.GetRowCount(descriptorIndex);
        if (isFirst)
        {
            m_curr = m_firstChunk->FirstPos();
        }
    }
}

/*----------------------------------------------------------------------*/

bool OMS_StreamBody::IsDefaultValue (const OmsTypeABAPColDesc& ColDesc, const SAPDB_Byte* pMember) const
{
    switch (ColDesc.abap_type)
    {
    case ABTYPC :
        {
            for (int ix = 0; ix < ColDesc.length; ++ix)
            {
                if (' ' != *pMember++)
                {
                    return false;
                }
            }
            return true;
        }
    case ABTYPWYDE :
        {
            const OmsTypeWyde* pWyde = reinterpret_cast<const OmsTypeWyde*>(pMember);
            int length = (OMS_SwapAndModifyInfo::copy == ColDesc.inout) ? ColDesc.length / 2 : ColDesc.length;
            for (int ix = 0; ix < length; ++ix)
            {
                if (' ' != *pWyde++)
                {
                    return false;
                }
            }
            return true;
        }
    case ABTYPINT :
        return (0 == *reinterpret_cast<const SAPDB_Int*>(pMember));
    case ABTYPINT2 :
        return (0 == *reinterpret_cast<const SAPDB_Int2*>(pMember));
    case ABTYPINT1 :
        return (0 == *reinterpret_cast<const SAPDB_Byte*>(pMember));
    case ABTYPP :
        {
            if (8 == ColDesc.length)
            {
                return (0 == memcmp (pMember, &cZeroPacked_8_3[0], ColDesc.length));
            }
            else
            {
                return (0 == memcmp (pMember, &cZeroPacked_15[0], ColDesc.length));
            }
        }
    case ABTYPDATE :
    case ABTYPTIME :
    case ABTYPNUM  :
        {
            for (int ix = 0; ix < ColDesc.length; ++ix)
            {
                if ('0' != *pMember++)
                {
                    return false;
                }
            }
            return true;
        }
    case ABTYPX  :
        {
            for (int ix = 0; ix < ColDesc.length; ++ix)
            {
                if (0 != *pMember++)
                {
                    return false;
                }
            }
            return true;
        }
    }
    return false;
}

/*----------------------------------------------------------------------*/

void OMS_StreamBody::PutDefaultValue (const OmsTypeABAPColDesc& ColDesc, SAPDB_Byte* pDest)
{
    switch (ColDesc.abap_type)
    {
    case ABTYPC :
        {
            for (int ix = 0; ix < ColDesc.length; ++ix)
            {
                *pDest++ = ' ';
            }
            break;
        }
    case ABTYPWYDE :
        {
            int fieldLen = (OMS_SwapAndModifyInfo::copy == ColDesc.inout) ? ColDesc.length / 2 : ColDesc.length; 
            OmsTypeWyde* pWyde = REINTERPRET_CAST(OmsTypeWyde*, pDest);
            while (fieldLen-- > 0)
            {
                *pWyde++ = ' ';
            }
            break;
        }
    case ABTYPINT :
        *reinterpret_cast<SAPDB_Int*>(pDest) = 0;
        break;
    case ABTYPINT2 :
        *reinterpret_cast<SAPDB_Int2*>(pDest) = 0;
        break;
   case ABTYPINT1 :
        *reinterpret_cast<SAPDB_Byte*>(pDest) = 0;
        break;
    case ABTYPP :
        {
            // packed
            if (8 == ColDesc.length)
            {
                SAPDB_MemCopyNoCheck(pDest, &cZeroPacked_8_3[0], ColDesc.length);
            }
            else
            {
                SAPDB_MemCopyNoCheck(pDest, &cZeroPacked_15[0], ColDesc.length);
            }
            break;
        }
    case ABTYPDATE :
    case ABTYPTIME :
    case ABTYPNUM  :
        {
            for (int ix = 0; ix < ColDesc.length; ++ix)
            {
                *pDest++ = '0';
            }
            break;
        }
    case ABTYPX  :
        {
            for (int ix = 0; ix < ColDesc.length; ++ix)
            {
                *pDest++ = 0;
            }
            break;
        }
    default : 
        {
            // unexpected ABAP type
            DbpBase base(m_handle.m_pSession->m_lcSink);
            base.dbpOpError("no default value defined for abap type : %d", ColDesc.abap_type);
            m_handle.m_pSession->ThrowDBError (e_unexpected_abap_type, "OMS_StreamBody::PutDefaultValue", __MY_FILE__, __LINE__);                        
        }
    }
}

/*----------------------------------------------------------------------*/

SAPDB_UInt4 OMS_StreamBody::ReadBytes(unsigned char *buf, SAPDB_UInt4 bufLen)
{
    m_isInUse = true; /* PTS 1135058, PG */

    SAPDB_UInt4 rc = 0;
    SAPDB_UInt4 copyLen = 0;

    if (!m_handle.m_pSession->m_stream_io) 
    {
        return 0;
    }
    m_handle.m_pSession->IncReadStreamRow();

    if (m_eot && m_ReadBytesBufSize == 0) 
    {
        return 0;
    }

    do {
        if (m_ReadBytesBufSize == 0) {
            if (m_firstChunk)
            {
                m_handle.m_pSession->deallocate(m_firstChunk);
                m_firstChunk = 0;
            }
            SAPDB_Int2  DBError;

            m_ReadBytesBuf = NULL;
            m_ReadBytesBufOffset = 0;
            m_ReadBytesBufSize = 0;

            OMS_StreamDescriptor streamDesc(this, m_streamDescriptor.ABAPTabId);
            m_handle.m_pSession->m_lcSink->ABAPRead (streamDesc, DBError);
            m_ReadBytesBuf     = streamDesc.GetBufAddr(0);
            m_rowCount         = streamDesc.GetRowCount(0);
            m_ReadBytesBufSize = streamDesc.GetBufSize(0);
            m_handle.m_pSession->IncReadStreamBuffer();
            if ( DBError != 0 )
            {
                m_handle.m_pSession->m_stream_io = false;
                m_handle.m_pSession->ThrowDBError (DBError, "omsReadBytes", __MY_FILE__, __LINE__);
            }
            if (streamDesc.IsEndOfStream(0))
            {
                this->SetEndOfStream();
                if (0 == m_ReadBytesBufSize)
                {
                    return 0;
                }
            }
        }
        copyLen = ((SAPDB_UInt4)m_ReadBytesBufSize >= (bufLen - rc)) ? (bufLen - rc) : m_ReadBytesBufSize;
        SAPDB_MemCopyNoCheck(buf + rc, m_ReadBytesBuf + m_ReadBytesBufOffset, copyLen);
        m_ReadBytesBufSize -= copyLen;
        m_ReadBytesBufOffset += copyLen;
        rc += copyLen;
    } while (rc < bufLen && !m_eot);
    if ((m_ReadBytesBufSize > 0) && (0 == m_firstChunk))
    {
      m_firstChunk = OMS_StreamBufferChunk::Create(m_handle.m_pSession, &m_ReadBytesBuf[m_ReadBytesBufOffset], 
            m_ReadBytesBufSize, m_ReadBytesBufSize);
        m_ReadBytesBufOffset = 0;
        m_ReadBytesBuf       = m_firstChunk->FirstPos();
    }
    return rc;

}

/*----------------------------------------------------------------------*/

void OMS_StreamBody::Flush()
{
    m_isInUse = true; /* PTS 1135058, PG */

    if ((!m_handle.m_pSession->m_stream_io) || (0 == m_rowCount))
    {
        return;
    }
    SAPDB_Int2 DBError;
    OMS_StreamDescriptor streamDesc(this, m_streamDescriptor.ABAPTabId);
    for (OMS_StreamBody* iter = m_registry.Begin(!cIsInStream); iter != m_registry.End(!cIsInStream); iter = iter->GetNextStream())
    {
        if (iter != this)
        {
            if (!streamDesc.Append (iter, iter->m_streamDescriptor.ABAPTabId))
            {
                break;
            }
        }
    }
    this->GetSink()->ABAPWrite (streamDesc, DBError);
    if (0 == DBError)
    {
        for (int ix = 0; ix < streamDesc.GetNoOfDescriptions(); ++ix)
        {
            OMS_StreamBody* pCurrStream = streamDesc.GetBody(ix);
            int rest    = pCurrStream->m_rowCount - streamDesc.GetRowCount(ix);
            int written = streamDesc.GetBufSize(ix);
            OMS_TRACE (omsTrStream, this->GetSink(), "ABAPWrite result " << streamDesc.GetStreamId(ix) << " rest " << rest << " written " << written); 
            if (rest > 0)
            {
                if (written > 0)
                {
                    pCurrStream->m_outBufferLength -= written; 
                    SAPDB_MemMoveNoCheck(pCurrStream->m_outBuffer, 
                        pCurrStream->m_outBuffer + written, pCurrStream->m_outBufferLength);
                }
            }
            else
            {
                pCurrStream->m_outBufferLength = 0;
                m_registry.Remove(pCurrStream);
            }
            pCurrStream->m_rowCount = rest;
            pCurrStream->m_curr     = &pCurrStream->m_outBuffer[pCurrStream->m_outBufferLength];
        }
        m_handle.m_pSession->IncWriteStreamBuffer();
    }
    else
    {
        m_handle.m_pSession->m_stream_io = false; 
        m_handle.m_pSession->ThrowDBError (DBError, "omsFlush (stream)", __MY_FILE__, __LINE__);
    }
}

/*----------------------------------------------------------------------*/

void OMS_StreamBody::FlushBytes()
{
    m_isInUse = true; /* PTS 1135058, PG */

    if ((!m_handle.m_pSession->m_stream_io) || (m_rowCount == 0))
    {
        return;
    }

    SAPDB_Int2 DBError;

    OMS_StreamDescriptor streamDesc(this, m_streamDescriptor.ABAPTabId);
    m_handle.m_pSession->m_lcSink->ABAPWrite (streamDesc, DBError);
    m_rowCount = 0;
    m_curr     = m_outBuffer;
    m_handle.m_pSession->IncWriteStreamBuffer();
    if ( DBError != 0 ) {
        m_handle.m_pSession->m_stream_io = false; 
        m_handle.m_pSession->ThrowDBError (DBError, "omsFlushBytes", __MY_FILE__, __LINE__);
    }
}

/*----------------------------------------------------------------------*/

void OMS_StreamBody::Write(SAPDB_Byte* pObj)
{
    m_isInUse = true; /* PTS 1135058, PG */

    // write one row into the output buffer. The row is transferred to the client layout
    // if necessary
    if (!m_handle.m_pSession->m_stream_io) 
    {
        return;
    }
    if (this->DefaultCompressionEnabled())
    {
        this->WriteCompressed(pObj);
        return;
    }
    if (m_maxOutCount == m_rowCount)
    {
        this->Flush();
        if (!m_isRegistered)  /* PTS 1133565, PG */
        {
            m_registry.Enter(this, !cIsInStream);
        }
    }
    else
    {
       if (0 == m_rowCount)
       {
           m_registry.Enter(this, !cIsInStream);
       }
    }
    for (int descIdx = 0; descIdx < m_streamDescriptor.colCount; ++descIdx)
    {
        OmsTypeABAPColDesc& abapDesc = m_streamDescriptor.colDesc[descIdx];
        register SAPDB_Byte*       pDest   = m_curr + abapDesc.offset;
        register const SAPDB_Byte* pSource = pObj   + abapDesc.dec;
        // copy and transform one struct member
        switch (abapDesc.inout) {
        case OMS_SwapAndModifyInfo::swap_2 :
            {
                // swap 2 bytes integer(s)
                // PTS 1132704, PG
                int intCnt = 0;
                if (abapDesc.abap_type == ABTYPINT2) 
                {
                    intCnt = 1; // == abapDesc.length/2
                }
                else // ABTYPWYDE
                {
                    intCnt = abapDesc.length;
                }
                for (int l = 0; l < intCnt; ++l)
                {
                    CopyAndSwapI2(pSource, pDest);
                    pSource += 2;
                    pDest   += 2;
                }
                break;
            }
        case OMS_SwapAndModifyInfo::swap_4 :
            {
                // swap 4 byte integer
                CopyAndSwapI4(pSource, pDest);
                break;
            }
        case OMS_SwapAndModifyInfo::swap_8 :
            {
                // swap 8 byte datatypes (int8, double)
                CopyAndSwapI8(pSource, pDest);
                break;
            }
        case OMS_SwapAndModifyInfo::asciiClientWydeCpp :
            {
                // truncate wyde character to ascii character
                const OmsTypeWyde* pWyde = REINTERPRET_CAST(const OmsTypeWyde*, pSource);
                for (int l = 0; l < abapDesc.length; ++l)
                {
                    if (*pWyde > 255)
                    {
                        // unicode char does not fit into ascii
                        DbpBase base(m_handle.m_pSession->m_lcSink);
                        base.dbpOpError("unicode not translatable to ascii : %d", *pWyde);
                        base.dbpOpError("offset of member is %d bytes", abapDesc.offset);
                        base.dbpOpError("index  in member is %d", l);
                        tsp00_Int4 bufLength = sizeof(m_streamDescriptor.ABAPTabId);
                        tsp00_Int2 dummyError;
                        m_handle.m_pSession->m_lcSink->MultiPurpose (m_return_error, mm_nil, 
                            &bufLength, reinterpret_cast<unsigned char*>(&m_streamDescriptor.ABAPTabId), &dummyError);
                        m_handle.m_pSession->ThrowDBError (e_OMS_not_translatable, "OMS_StreamBody::Write", __MY_FILE__, __LINE__);
                    }
                    *pDest = (SAPDB_Byte) *pWyde;
                    ++pWyde;
                    ++pDest;
                }
                break;
            }
        case OMS_SwapAndModifyInfo::unicodeClientAsciiCpp       :
        case OMS_SwapAndModifyInfo::swappedUnicodeClientAsciiCpp :
            {
                // expand ascii to unicode
                for (int l = 0; l < abapDesc.length; ++l)
                {
                    *(pDest + m_highByte) = 0;
                    *(pDest + m_lowByte ) = *pSource;
                    pDest   += 2;
                    ++pSource;
                }
                break;
            }
        default :
            {
                // just copy without any translation 
                SAPDB_MemCopyNoCheck(pDest, pSource, abapDesc.length);
            }
        } // case
    }
    m_outBufferLength += m_clientRowSize;
    ++m_rowCount;
    m_curr += m_clientRowSize;
    m_handle.m_pSession->IncWriteStreamRow();
}

/*----------------------------------------------------------------------*/

void OMS_StreamBody::WriteBytes(const SAPDB_Byte* pObj, SAPDB_UInt4 bufLen)
{
    m_isInUse = true; /* PTS 1135058, PG */

    SAPDB_UInt4 copyLen = 0;
    SAPDB_UInt4 offset = 0;
    SAPDB_UInt4 freeStreamBuf = 0;

    if (!m_handle.m_pSession->m_stream_io) 
    {
        return;
    }

    while (bufLen > 0) {
        freeStreamBuf = m_maxOutCount - m_rowCount;
        copyLen = (freeStreamBuf >= bufLen) ? bufLen : freeStreamBuf;

        SAPDB_MemCopyNoCheck(m_curr, pObj+offset, copyLen);
        m_rowCount += copyLen;
        m_curr += copyLen;

        if (copyLen < bufLen) {
            this->FlushBytes();
        }
        bufLen -= copyLen;
        offset += copyLen;
    }

}

/*----------------------------------------------------------------------*/

void OMS_StreamBody::WriteCompressed (SAPDB_Byte* pObj)
{
    m_isInUse = true; /* PTS 1135058, PG */

    // write one row into the output buffer. The row is transferred to the client layout
    // if necessary
    if (0 == m_rowCount)
    {
        m_registry.Enter(this, !cIsInStream);
    }
    int setSize           = this->DefaultSetSize();
    SAPDB_UInt4 rowLength = 2 + setSize;
FlushAndRepeat :
    if (m_outBufferLength + rowLength > m_outBufferSize)
    {
         this->Flush();
         rowLength = 2 + setSize;
         if (!m_isRegistered)  /* PTS 1133565, PG */
         {
             m_registry.Enter(this, !cIsInStream);
         }
    }
    register SAPDB_Byte* pDest = m_curr + 2; // reseverve space for length information 
    // add empty default value bit map
    for (int ix = 0; ix < setSize; ++ix)
    {
        *pDest++ = 0;
    }
    for (int descIdx = 0; descIdx < m_streamDescriptor.colCount; ++descIdx)
    {
        if (this->IsHiddenColumn(descIdx))
        {
            continue;
        }
         OmsTypeABAPColDesc& abapDesc = m_streamDescriptor.colDesc[descIdx];
        register const SAPDB_Byte* pSource = pObj + abapDesc.dec;
        if (this->IsDefaultValue(abapDesc, pSource))
        {
            this->SetBit (descIdx, m_curr + 2);
            continue;
        }
        // copy and transform one struct member
        switch (abapDesc.inout) {
        case OMS_SwapAndModifyInfo::swap_2 :
            {
                // PTS 1132704, PG
                int intCnt = 0;
                if (abapDesc.abap_type == ABTYPINT2) 
                {
                    intCnt = 1; // == abapDesc.length/2
                }
                else // ABTYPWYDE
                {
                    intCnt = abapDesc.length;
                }
                rowLength += intCnt * 2;
                if (m_outBufferLength + rowLength > m_outBufferSize)
                {
                    goto FlushAndRepeat;
                }
                // swap 2 bytes integer(s)
                for (int l = 0; l < intCnt; ++l)
                {
                    CopyAndSwapI2(pSource, pDest);
                    pSource += 2;
                    pDest   += 2;
                }
                break;
            }
        case OMS_SwapAndModifyInfo::swap_4 :
            {
                rowLength += 4;
                if (m_outBufferLength + rowLength > m_outBufferSize)
                {
                    goto FlushAndRepeat;
                }
                // swap 4 byte integer
                CopyAndSwapI4(pSource, pDest);
                pDest += 4;
                break;
            }
        case OMS_SwapAndModifyInfo::swap_8 :
            {
                rowLength += 8;
                if (m_outBufferLength + rowLength > m_outBufferSize)
                {
                    goto FlushAndRepeat;
                }
                // swap 8 byte datatypes (int8, double)
                CopyAndSwapI8(pSource, pDest);
                pDest += 8;
                break;
            }
        case OMS_SwapAndModifyInfo::asciiClientWydeCpp :
            {
                rowLength += abapDesc.length;
                if (m_outBufferLength + rowLength > m_outBufferSize)
                {
                    goto FlushAndRepeat;
                }
                // truncate wyde character to ascii character
                const OmsTypeWyde* pWyde = REINTERPRET_CAST(const OmsTypeWyde*, pSource);
                for (int l = 0; l < abapDesc.length; ++l)
                {
                    if (*pWyde > 255)
                    {
                        // unicode char does not fit into ascii
                        DbpBase base(m_handle.m_pSession->m_lcSink);
                        base.dbpOpError("unicode not translatable to ascii : %d", *pWyde);
                        base.dbpOpError("offset of member is %d bytes", abapDesc.offset);
                        base.dbpOpError("index  in member is %d", l);
                        tsp00_Int4 bufLength = sizeof(m_streamDescriptor.ABAPTabId);
                        tsp00_Int2 dummyError;
                        m_handle.m_pSession->m_lcSink->MultiPurpose (m_return_error, mm_nil, 
                            &bufLength, reinterpret_cast<unsigned char*>(&m_streamDescriptor.ABAPTabId), &dummyError);
                        m_handle.m_pSession->ThrowDBError (e_OMS_not_translatable, "OMS_StreamBody::WriteCompressed", __MY_FILE__, __LINE__);
                    }
                    *pDest = (SAPDB_Byte) *pWyde;
                    ++pWyde;
                    ++pDest;
                }
                break;
            }
        case OMS_SwapAndModifyInfo::unicodeClientAsciiCpp       :
        case OMS_SwapAndModifyInfo::swappedUnicodeClientAsciiCpp :
            {
                rowLength += abapDesc.length * 2;
                if (m_outBufferLength + rowLength > m_outBufferSize)
                {
                    goto FlushAndRepeat;
                }
                
                // expand ascii to unicode
                for (int l = 0; l < abapDesc.length; ++l)
                {
                    *(pDest + m_highByte) = 0;
                    *(pDest + m_lowByte ) = *pSource;
                    pDest   += 2;
                    ++pSource;
                }
                break;
            }
        default :
            {
                rowLength += abapDesc.length;
                if (m_outBufferLength + rowLength > m_outBufferSize)
                {
                    goto FlushAndRepeat;
                }
                // just copy without any translation 
                SAPDB_MemCopyNoCheck(pDest, pSource, abapDesc.length);
                pDest += abapDesc.length;
            }
        } // case
    }
    if (rowLength > SAPDB_MAX_INT2)
    {
        m_handle.m_pSession->ThrowDBError (e_too_long_record, "OMS_StreamBody::WriteCompressed", __MY_FILE__, __LINE__);
    }
    union IC2
    {
        SAPDB_Int2 m_i2;
        SAPDB_Byte m_c2[2];
    } ic2;
    ic2.m_i2  = rowLength;
    *m_curr++ = ic2.m_c2[0];
    *m_curr   = ic2.m_c2[1];
    m_outBufferLength += rowLength;
    ++m_rowCount;
    m_curr = pDest;
}

/*----- PTS 1135058, PG ------------------------------------------------*/

void OMS_StreamBody::DisableDefaultCompression()
{
    if (m_isInUse)
    {
        char buffer[256];
        memset( buffer, 0, sizeof(buffer) );
        sp77sprintf( buffer, sizeof(buffer),
                     "Requesting disabled default compression for already in-use stream: method %s, stream %x",
                     m_handle.omsGetCurrentMethod(), m_streamDescriptor.ABAPTabId );
        m_handle.dbpOpError(buffer);
        return;
    }

    m_hiddenColumnsSupport = false;

    if (m_defaultCompressionSwitchable)
    {
        if (!m_compressDefaults)
        {
            // compression already disabled
            return;
        }
        m_compressDefaults = false;
        if (0 == m_hiddenColumns)
        {
            SAPDB_UInt requiredSize = EvalHidearraySize(m_streamDescriptor.colCount);
            m_hiddenColumns = reinterpret_cast<SAPDB_Byte*>(m_handle.m_pSession->allocate(requiredSize));
            for ( SAPDB_UInt ix = 0; ix < requiredSize; ++ix )
            {
                m_hiddenColumns[ix] = 0x00;
            }
            SetBit( m_streamDescriptor.colCount, m_hiddenColumns );
        }
        else
        {
            char buffer[256];
            memset( buffer, 0, sizeof(buffer) );
            sp77sprintf( buffer, sizeof(buffer),
                         "OMS_StreamBody::m_hiddenColumns not NULL for stream with hex. ID %x in method %s.",
                         m_streamDescriptor.ABAPTabId, m_handle.omsGetCurrentMethod() );
            m_handle.m_pSession->ThrowDBError( e_internal_stream_error, buffer, __MY_FILE__, __LINE__ );
        }
    }
}

/*----------------------------------------------------------------------*/

OmsStream::OmsStream(OmsHandle* h, OmsTypeABAPTabHandle& sh, long rsz, long sz, bool istream) 
  : m_pSession(h->m_pSession)
{
    if (rsz == 0 || sh.rowSize == 0 || sh.rowSize > OMS_Globals::GetKernelInterface()->
        GetPacketMaxUsableArea()) {
        // invalid row size
        if (rsz == 0 || sh.rowSize == 0) {
            h->dbpOpError("OmsStream::OmsStream: stream row size must be > 0");
        } else {
            h->dbpOpError("OmsStream::OmsStream: stream row size %d must be < %d",
              sh.rowSize, OMS_Globals::GetKernelInterface()->
              GetPacketMaxUsableArea());
        }
        OMS_Globals::Throw(e_invalid_rowsize, "OmsStream::OmsStream", __MY_FILE__, __LINE__);
    }
    m_body = new (m_pSession->allocate(sizeof(OMS_StreamBody))) OMS_StreamBody (*h, sh, rsz, istream);
}

/*----------------------------------------------------------------------*/

OmsStream::OmsStream(OmsHandle* h, OmsTypeStreamHandle& sh, long rsz, long sz, bool istream) 
  : m_pSession(h->m_pSession)
{
    if (rsz == 0 || sh.size == 0 || sh.size > OMS_Globals::GetKernelInterface()->
        GetPacketMaxUsableArea()) {
        // invalid row size
        if (rsz == 0 || sh.size == 0) {
            h->dbpOpError("OmsStream::OmsStream: stream row size must be > 0");
        } else {
            h->dbpOpError("OmsStream::OmsStream: stream row size %d must be < %d",
              sh.size, OMS_Globals::GetKernelInterface()->
              GetPacketMaxUsableArea());
        }
        OMS_Globals::Throw(e_invalid_rowsize, "OmsStream::OmsStream", __MY_FILE__, __LINE__);
    }
    m_body = new (m_pSession->allocate(sizeof(OMS_StreamBody))) OMS_StreamBody (*h, 
        *REINTERPRET_CAST(OmsTypeABAPTabHandle*, &sh), rsz, istream);
}

/*----------------------------------------------------------------------*/

OmsStream::~OmsStream() 
{ 
    if (m_body)
    {
        m_body->~OMS_StreamBody();
        m_pSession->deallocate(m_body);
        m_body = NULL;
    }
}

/*----------------------------------------------------------------------*/

void OmsStream::omsClose() 
{
    m_body->Flush();
}

/*----------------------------------------------------------------------*/

void OmsStream::omsCloseBytes() 
{
    m_body->FlushBytes();
}

/*----------------------------------------------------------------------*/

void OmsStream::omsFlush() {
    m_body->Flush();  
}

/*----------------------------------------------------------------------*/

void OmsStream::omsFlushBytes() {
    m_body->FlushBytes();  
}

/*----------------------------------------------------------------------*/

void OmsStream::omsHide(unsigned int columnNo)
{
    m_body->Hide(columnNo);
}

/*----------------------------------------------------------------------*/

unsigned char* OmsStream::omsNxt() 
{
    _TRACE_METHOD_ENTRY(m_body->getHandle(),"OmsStream::omsNxt");
    OMS_CHECK_EXCEPTION(m_body->getHandle());
    return m_body->GetNext();
}

/*----------------------------------------------------------------------*/

SAPDB_UInt4 OmsStream::omsReadBytes(unsigned char *buf, SAPDB_UInt4 bufLen)
{
    _TRACE_METHOD_ENTRY(m_body->getHandle(),"OmsStream::omsReadBytes");
    OMS_CHECK_EXCEPTION(m_body->getHandle());
    return m_body->ReadBytes(buf, bufLen);
}

/*----- PTS 1131114, PG ------------------------------------------------*/

void OmsStream::omsIReset()
{
    _TRACE_METHOD_ENTRY(m_body->getHandle(),"OmsStream::omsIReset");
    OMS_CHECK_EXCEPTION(m_body->getHandle());
    return m_body->Reset();
}

/*----------------------------------------------------------------------*/

void OmsStream::omsWrite(unsigned char* obj) {
    
    _TRACE_METHOD_ENTRY(m_handle,"OmsStream::omsWrite");
    m_body->Write(obj);
}

/*----------------------------------------------------------------------*/

void OmsStream::omsWriteBytes(const unsigned char* obj, unsigned int bufLen) {
    
    _TRACE_METHOD_ENTRY(m_handle,"OmsStream::omsWriteBytes");
    m_body->WriteBytes(obj, bufLen);
}

/*----- PTS 1131555, PG ------------------------------------------------*/

void OmsStream::omsOReset()
{
    _TRACE_METHOD_ENTRY(m_body->getHandle(),"OmsStream::omsOReset");
    OMS_CHECK_EXCEPTION(m_body->getHandle());
    return m_body->Reset();
}

/*----- PTS 1135058, PG ------------------------------------------------*/

void OmsStream::omsDisableDefaultCompression()
{
    _TRACE_METHOD_ENTRY(m_body->getHandle(),"OmsStream::omsDisableDefaultCompression");
    OMS_CHECK_EXCEPTION(m_body->getHandle());
    m_body->DisableDefaultCompression();
}

/*----- PTS 1135058, PG ------------------------------------------------*/

bool OmsStream::omsDefaultCompressionEnabled() const
{
    _TRACE_METHOD_ENTRY(m_body->getHandle(),"OmsStream::omsDefaultCompressionEnabled");
    OMS_CHECK_EXCEPTION(m_body->getHandle());
    return m_body->DefaultCompressionEnabled();
}

