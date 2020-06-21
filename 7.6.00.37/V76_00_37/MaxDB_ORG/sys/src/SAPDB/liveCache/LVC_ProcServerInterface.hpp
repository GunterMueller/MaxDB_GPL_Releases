/*!
 * \file    LVC_ProcServerInterface.hpp
 * \author  IvanS, MarkusSi
 * \brief   Interface for communicating between ProcServer and kernel.
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

#ifndef __LVC_PROCSERVER_INTERFACE
#define __LVC_PROCSERVER_INTERFACE

#ifndef WIN32
#include <stdlib.h>
#endif

#include "Oms/OMS_Defines.h"
#include "Oms/OMS_DbpError.hpp"
#include "ggg00.h"
#include "ggg01.h"
#include "ggg91.h"
#include "gsp00.h"


class SAPDBMem_IRawAllocator;
class LVC_ProcServerInterface;
class OMS_Context;

/// Packet virtual base.
class LVC_ProcServerPacketBase {
public:
  /// Dummy virtual destructor (never called).
  virtual ~LVC_ProcServerPacketBase() {}
};

/*!
 * \brief ProcServer communication packet header structure.
 *
 * \ingroup oms procserver
 */
class LVC_ProcServerPacketHeader : public LVC_ProcServerPacketBase {
public:
  /*!
   * \brief Execute the action on local sink and send return packet.
   *
   * \param sink liveCache sink (kernel-side),
   * \param interf communication interface for sending reply.
   */
  virtual void execute(IliveCacheSink *sink, LVC_ProcServerInterface *interf);

  /// Register the packet type with executor.
  inline static void registerSelf(int code, void *vtable);

  size_t  m_size;   /// Size of the packet.
  int     m_code;   /// >0 Operation/reply code, =0 extra packet, <0 error code.
  // Packet data follow...
};

/*!
 * \brief ProcServer communication packet.
 *
 * \ingroup oms procserver
 */
template<class T>
class LVC_ProcServerPacket : public LVC_ProcServerPacketHeader {
public:
  /// Register the packet type with executor.
  inline static void registerSelf();
};

/*!
 * \brief ProcServer start packet structure.
 *
 * \ingroup oms procserver
 */
class LVC_ProcServerStart : public LVC_ProcServerPacket<LVC_ProcServerStart> {
public:
  enum {
    CODE  = 1 ///< Packet code.
  };
  tsp00_KnlIdentifier m_method;                 ///< Method name.
  tsp00_KnlIdentifier m_method_ascii;           ///< Method name in ASCII.
  int                 m_libnameofs;             ///< Library name to use (offset within packet).
  tsp00_C32           m_libmd5;                 ///< Library MD5 checksum.
  CLSID               m_clsid;                  ///< GUID of the COM object class.
  IID                 m_iid;                    ///< GUID of the COM object interface.
  int                 m_dispid;                 ///< Dispatcher ID (VTable index).
  int                 m_paramcnt;               ///< Parameter count.
  int                 m_paramofs;               ///< IN parameter start offset.
  // variable-sized debug parameter stack follows
  // variable-sized library name follows
  // variable-sized IN structures/parameters follow (suitably aligned)
};

class LVC_ProcServerReturn : public LVC_ProcServerPacket<LVC_ProcServerReturn> {
public:
  enum {
    CODE  = 2 ///< Packet code.
  };
  short               m_retcode;                ///< Return code (DbpError code or 0 or 9999 when arguments don't fit).
  // variable-sized INOUT and OUT structures/parameters follow (suitably aligned)
};

class LVC_ProcServerTerminateReq : public LVC_ProcServerPacket<LVC_ProcServerTerminateReq> {
public:
  enum {
    CODE  = 3 ///< Packet code.
  };
};

class LVC_ProcServerTerminateRep : public LVC_ProcServerPacket<LVC_ProcServerTerminateRep> {
public:
  enum {
    CODE  = 4 ///< Packet code.
  };
};

/*!
 * \brief Simple reply packet.
 */
class LVC_ProcServerSimpleRep : public LVC_ProcServerPacket<LVC_ProcServerSimpleRep> {
public:
  enum {
    CODE  = 5 ///< Packet code.
  };

  /// Return value.
  int m_return;
};

/*!
 * \brief Reply packet for an operation on a single object.
 */
class LVC_ProcServerSingleObjRep : public LVC_ProcServerPacket<LVC_ProcServerSingleObjRep> {
public:
  enum {
    CODE  = 200 ///< Packet code.
  };

  tgg91_TransNo            m_consistentView;
  tgg00_FileId             m_containerId;
  SAPDB_UInt8               m_oid;
  tgg91_PageRef            m_objSeq;
  tgg91_TransNo            m_updTransId;
  tsp00_Int4               m_bodyLen;
  tsp00_Int4               m_counter;
  tgg00_BasisError         m_DBError;
};

/*!
 * \brief Reply packet for operations on versions.
 */
class LVC_ProcServerVersionRep : public LVC_ProcServerPacket<LVC_ProcServerVersionRep> {
public:
  enum {
    CODE  = 201 ///< Packet code.
  };

  tgg01_OmsVersionContext  m_versionContext;
  tgg00_BasisError         m_DBError;
};

/*!
 * \brief Reply packet for operations on a class/container.
 */
class LVC_ProcServerContainerRep : public LVC_ProcServerPacket<LVC_ProcServerContainerRep> {
public:
  enum {
    CODE  = 202 ///< Packet code.
  };

  tgg01_ContainerId  m_containerId;
  GUID               m_guid;
  OmsSchemaHandle    m_schema;
  OmsContainerNo     m_containerNo;
  bool               m_useCachedKeys;
  bool               m_partitionedKeys;
  int                m_syncCnt;
  tgg00_BasisError   m_DBError;
};


/*!
 * \brief Reply packet for operations on schematas.
 */
class LVC_ProcServerSchemaRep : public LVC_ProcServerPacket<LVC_ProcServerSchemaRep> {
public:
  enum {
    CODE  = 203 ///< Packet code.
  };

  tsp00_Int4         m_schemaId;
  OmsTypeWyde        m_schemaName[OMS_MAX_SCHEMA_NAME_LENGTH+1];
  tgg00_BasisError   m_DBError;
};


/*!
 * \brief Reply packet for handling of consistent views.
 */
class LVC_ProcServerConsistentViewRep : public LVC_ProcServerPacket<LVC_ProcServerConsistentViewRep> {
public:
  enum {
    CODE  = 206 ///< Packet code.
  };

  tgg91_TransNo      m_consistentView;
  tgg00_BasisError   m_DBError;
};

/*!
 * \brief Reply packet for a mass operation on objects.
 */
class LVC_ProcServerMassObjRep : public LVC_ProcServerPacket<LVC_ProcServerMassObjRep> {
public:
  enum {
    CODE  = 207 ///< Packet code.
  };

  tsp00_Int4               m_objCnt;
  tgg91_TransNo            m_consistentView;
  tgg00_FileId             m_pContainerId  [OMS_MASS_OPERATION_CNT];
  SAPDB_UInt8              m_pOid          [OMS_MASS_OPERATION_CNT];
  tgg91_PageRef            m_pObjSeq       [OMS_MASS_OPERATION_CNT];
  tgg91_TransNo            m_pUpdTransId   [OMS_MASS_OPERATION_CNT];
  tsp00_Int4               m_counter;
  tsp00_Int4               m_errCnt;
  tgg00_BasisError         m_pDBError      [OMS_MASS_OPERATION_CNT];
  tsp00_Int8               m_timeout;
};

/*!
 * \brief Reply packet for iterators.
 */
class LVC_ProcServerIteratorRep : public LVC_ProcServerPacket<LVC_ProcServerIteratorRep> {
public:
  enum {
    CODE  = 208 ///< Packet code.
  };

  tsp00_Int4               m_cnt;
  tgg91_TransNo            m_consistentView;
  SAPDB_UInt8              m_pOid[OMS_MASS_OPERATION_CNT];
  tgg91_PageRef            m_pObjSeq[OMS_MASS_OPERATION_CNT];
  tsp00_Int4               m_logHistRead;
  tgg00_BasisError         m_DBError;
  void                    *m_pIterator;

  // For some answers the key is copied just behind the package
};

/*!
 * \brief Reply packet for sequences.
 */
class LVC_ProcServerSequenceRep : public LVC_ProcServerPacket<LVC_ProcServerSequenceRep> {
public:
  enum {
    CODE  = 209 ///< Packet code.
  };

  tgg00_Surrogate     m_sequenceId;
  tgg00_SeqInfo       m_sequenceInfo;
  tsp00_Uint4         m_nextVal;
  tgg00_BasisError    m_error;
};

/*!
 * \brief Reply packet for file-handling.
 */
class LVC_ProcServerFileRep : public LVC_ProcServerPacket<LVC_ProcServerFileRep> {
public:
  enum {
    CODE  = 210 ///< Packet code.
  };

  tsp00_Int4        m_fileHandle;
  bool              m_existsFile;
  bool              m_isDirectory;
  tgg00_BasisError  m_error;
};

/*!
 * \brief Reply packet for ABAP streamhandling.
 */
class LVC_ProcServerABAPStreamRep : public LVC_ProcServerPacket<LVC_ProcServerABAPStreamRep> {
public:
  enum {
    CODE  = 211 ///< Packet code.
  };

  OmsTypeInt4      m_tabHandle;
  OmsTypeInt4      m_count;
  OmsTypeInt4      m_bufSize;
  tgg00_BasisError m_error;
};


/*!
 * \brief Reply packet for handling of version dictionary.
 */
class LVC_ProcServerVersionDictRep : public LVC_ProcServerPacket<LVC_ProcServerVersionDictRep> {
public:
  enum {
    CODE  = 214 ///< Packet code.
  };

  OMS_Context      *m_pVersionContext;
  tgg00_BasisError  m_error;
  void             *m_pIter;
};


/*!
 * \brief Reply packet only with error code.
 */
class LVC_ProcServerErrorStateRep : public LVC_ProcServerPacket<LVC_ProcServerErrorStateRep> {
public:
  enum {
    CODE  = 299 ///< Packet code.
  };

  bool               m_state;
  tgg00_BasisError   m_DBError;
};



/*!
 * \brief Interface for communicating between ProcServer and kernel.
 *
 * Implementation derived from this interface enables the client and server
 * to communicate. Instance of this class is used as parameter to sink
 * and extender classes in OMS and liveCache.
 *
 * Both server (kernel) and client (OMS) will derive a class from this
 * interface, with appropriate additional routines for shared memory allocator
 * management and stored procedure call.
 *
 * \see \ref omsprocserver
 *
 * \ingroup oms procserver
 */
class LVC_ProcServerInterface {
public:
  /// Construct the instance.
  LVC_ProcServerInterface(SAPDBMem_IRawAllocator *alloc) 
    : m_isLocal(true), m_alloc(alloc), m_recvQueuePtr(0),
      m_recvQueueCount(0), m_writePending(0)
  {
  }

  /*!
   * \brief Map an address in ProcServer onto offset or size.
   *
   * This is used by the ProcServer to map an address onto offset or size,
   * depending on the connection type and eventually queue the object
   * to be written into send queue.
   *
   * \param addr ProcServer-local address of the object (in object cache),
   * \param objSize size of the object.
   */
  inline size_t getOffsetForObjectWrite(void *addr, size_t objSize)
  {
    if (m_isLocal) {
      return getOffsetForAddress(addr);
    } else {
      sendSecondary(addr, objSize);
      return objSize;
    }
  }

  /*!
   * \brief Map an address in ProcServer onto offset or size.
   *
   * This is used by the ProcServer to map an address onto offset or size,
   * depending on the connection type and eventually queue the object
   * to be read. After the receive, the objects are read into queued
   * places (in non-local case).
   *
   * \param addr ProcServer-local address of the object (in object cache),
   * \param objSize size of the object.
   */
  inline size_t getOffsetForObjectRead(void *addr, size_t objSize)
  {
    if (m_isLocal) {
      return getOffsetForAddress(addr);
    } else {
      queueObjectForRead(addr, objSize);
      return objSize;
    }
  }

  /*!
   * \brief Map data for reads (in kernel).
   *
   * This method maps supplied offset to address in case of local communication
   * or gets the pointer to additional data packet read from socket in case of
   * remote communication.
   *
   * Thus, the request itself contains only object offsets in case of local
   * communication or object sizes in case of non-local communication. This
   * method will then either map the offset into pointer in case of local
   * communication or return data from helper buffer in case of non-local
   * communication.
   *
   * \param addr pointer to be filled with address where to actually read data,
   * \param offssize offset or size sent by the ProcServer to indicate where
   *  to read the data.
   */
  void getObjectForRead(void* &addr, size_t offssize);

  /*!
   * \brief Map data for writes (in kernel).
   *
   * This method is similar to getObjectForRead in case of local communication.
   * In non-local case, it will allocate a buffer of requested size and return
   * address of this buffer. Buffers will be sent after the request/reply
   * packet to the other side. These buffers do not overwrite other buffers.
   *
   * Since this returns the address of a temporary buffer in non-local case,
   * you need to do extra copying into provided buffer. Consider using
   * copyObjectForWrite() instead.
   *
   * \note In non-local case, the address always points to the secondary send
   *  buffer. Calling getObjectForWrite() will actually flush out the packet
   *  that is currently in buffer.
   */
  void getObjectForWrite(void* &addr, size_t offssize);

  /*!
   * \brief Copy object from the given address (in kernel) at offset (in ProcServer).
   *
   * \param addr kernel-side object address,
   * \param offssize offset or object size sent from ProcServer,
   * \param size object size at kernel side.
   */
  void copyObjectForWrite(const void *addr, size_t offssize, size_t size);

  /*!
   * \brief Get send area.
   *
   * This method queries the address and size of the send area. Send
   * area is used to send data to the other side.
   *
   * \param addr filled with the address of the send buffer,
   * \param maxSize filled with the size of the send buffer.
   */
  virtual void getSendArea(void* &addr, size_t &maxSize) = 0;

  /*!
   * \brief Get secondary send area.
   *
   * This method queries the address and size of the secondary send
   * area. Send area is used to send data to the other side.
   *
   * \param addr filled with the address of the send buffer,
   * \param maxSize filled with the size of the send buffer.
   */
  virtual void getSecondarySendArea(void* &addr, size_t &maxSize) = 0;

  /*!
   * \brief Get receive area.
   *
   * This method queries the address and size of the receive area. Receive
   * area is used to get data from the other side.
   *
   * \param addr filled with the address of the send buffer,
   * \param maxSize filled with the size of the send buffer.
   */
  virtual void getReceiveArea(void* &addr, size_t &maxSize) = 0;

  /*!
   * \brief Send reply with a simple error code.
   *
   * \param errcode error code to send.
   */
  void sendError(int errcode);

  /*!
   * \brief Send data.
   *
   * Send data to the other party. Eventually wait for communication
   * buffer to be written to underlying system buffers and/or for the
   * segment to be read by the other side (if not read yet).
   *
   * \param size size of the packet in send area (needed only for
   *  non-SHM communication).
   */
  void send(size_t size);

  /*!
   * \brief Receive data.
   *
   * If the packet is not ready, wait for packet to come. Data are then
   * stored in receive area. Receiver must be able to get data size from
   * the packet itself.
   */
  void receive();

  /*!
   * \brief Convenience function to prepare a packet.
   */
  template<class T>
  inline void preparePacket(T* &packet, size_t &maxSize)
  {
    void *addr;
    getSendArea(addr, maxSize);
    packet = reinterpret_cast<T*>(addr);
    packet->m_code = T::CODE;
    packet->m_size = sizeof(T);
  }

  /*!
   * \brief Convenience function to prepare a packet.
   */
  template<class T>
  inline void preparePacket(T* &packet)
  {
    void *addr;
    size_t maxSize;
    getSendArea(addr, maxSize);
    packet = reinterpret_cast<T*>(addr);
    packet->m_code = T::CODE;
    packet->m_size = sizeof(T);
  }

  /*!
   * \brief Send LVC_ProcServerSimpleRep packet.
   */
  void simpleReply(int errCode)
  {
    LVC_ProcServerSimpleRep *packet;
    preparePacket(packet);
    packet->m_return = errCode;
    send(sizeof(LVC_ProcServerSimpleRep));
  }

  /*!
   * \brief Send LVC_ProcServerSingleObjRep packet.
   */
  void singleObjReply(const tgg91_TransNo     *pConsistentView,
                      const tgg01_ContainerId *pContainerId,
                      SAPDB_UInt8              oid,
                      const tgg91_PageRef     &objSeq,
                      const tgg91_TransNo     *pUpdTransId,
                      tsp00_Int4               bodyLen,
                      tsp00_Int4               counter,
                      tgg00_BasisError         DBError)
  {
    LVC_ProcServerSingleObjRep *packet;
    preparePacket(packet);

    if (pConsistentView)
      packet->m_consistentView = *pConsistentView;
    if (pContainerId)
      packet->m_containerId    = *pContainerId;
    packet->m_oid              = oid;
    packet->m_objSeq           = objSeq;
    if (pUpdTransId)
      packet->m_updTransId     = *pUpdTransId;
    packet->m_bodyLen          = bodyLen;
    packet->m_counter          = counter;
    packet->m_DBError          = DBError;

    send(sizeof(LVC_ProcServerSingleObjRep));
  }

  /*!
   * \brief Send LVC_ProcServerContainerRep packet.
   */
  void containerReply(const tgg01_ContainerId  *pContainerId,
                      const GUID               *pGuid,
                      OmsSchemaHandle          schema,
                      OmsContainerNo           containerNo,
                      bool                     useCachedKeys,
                      bool                     partitionedKeys,
                      int                      syncCnt,
                      tgg00_BasisError         DBError)
  {
    LVC_ProcServerContainerRep *packet;
    preparePacket(packet);

    if (pContainerId)
      packet->m_containerId  = *pContainerId;
    if (pGuid)
      packet->m_guid          = *pGuid;
    packet->m_schema          = schema;
    packet->m_containerNo     = containerNo;
    packet->m_useCachedKeys   = useCachedKeys;
    packet->m_partitionedKeys = partitionedKeys;
    packet->m_syncCnt         = syncCnt;
    packet->m_DBError         = DBError;

    send(sizeof(LVC_ProcServerContainerRep));
  }

  /*!
   * \brief Send LVC_ProcServerVersionRep packet.
   */
  void versionReply(const tgg01_OmsVersionContext  &versionContext,
                    tgg00_BasisError                DBError)
  {
    LVC_ProcServerVersionRep *packet;
    preparePacket(packet);

    packet->m_versionContext = versionContext;
    packet->m_DBError        = DBError;

    send(sizeof(LVC_ProcServerVersionRep));
  }

  /*!
   * \brief Send LVC_ProcServerSchemaRep packet.
   */
  void schemaReply(tsp00_Int4         schemaId,
                   const OmsTypeWyde *pSchemaName,
                   tgg00_BasisError   DBError)
  {
    LVC_ProcServerSchemaRep *packet;
    preparePacket(packet);

    if (pSchemaName)
      memcpy(&packet->m_schemaName[0], pSchemaName, sizeof(packet->m_schemaName));
    packet->m_schemaId   = schemaId;
    packet->m_DBError    = DBError;

    send(sizeof(LVC_ProcServerSchemaRep));
  }

  
  /*!
   * \brief Send LVC_ProcServerConsistentViewRep packet.
   */
  void consistentViewReply(const tgg91_TransNo &consistentView,
                           tgg00_BasisError     DBError)
  {
    LVC_ProcServerConsistentViewRep *packet;
    preparePacket(packet);

    packet->m_consistentView = consistentView;
    packet->m_DBError        = DBError;

    send(sizeof(LVC_ProcServerConsistentViewRep));
  }


  /*!
   * \brief Send LVC_ProcServerMassObjRep packet.
   */
  void massObjReply(tsp00_Int4                objCnt,
                    const tgg91_TransNo      *pConsistentView,
                    const tgg01_ContainerId  *pContainerId,
                    SAPDB_UInt8              *pOid,
                    const tgg91_PageRef      *pObjSeq,
                    const tgg91_TransNo      *pUpdTransId,
                    tsp00_Int4                counter,
                    tsp00_Int8                timeout,
                    tsp00_Int4                errCnt,
                    tgg00_BasisError         *pDBError)
  {
    LVC_ProcServerMassObjRep *packet;
    preparePacket(packet);

    packet->m_objCnt    = objCnt;
    if (pConsistentView)
      packet->m_consistentView = *pConsistentView;
    if (pContainerId)
      memcpy(&packet->m_pContainerId[0], pContainerId, objCnt * sizeof(packet->m_pContainerId[0]));
    if (pOid)
      memcpy(&packet->m_pOid[0], pOid, objCnt);
    if (pObjSeq)
      memcpy(&packet->m_pObjSeq[0], pObjSeq, objCnt * sizeof(packet->m_pObjSeq[0]));
    if (pUpdTransId)
      memcpy(&packet->m_pUpdTransId[0], pUpdTransId, objCnt * sizeof(packet->m_pUpdTransId[0]));
    packet->m_counter  = counter;
    packet->m_timeout  = timeout;
    packet->m_errCnt   = errCnt;
    if (pDBError)
      memcpy(&packet->m_pDBError[0], pDBError, objCnt * sizeof(packet->m_pDBError[0]));

    send(sizeof(LVC_ProcServerMassObjRep));
  }

  /*!
   * \brief Send LVC_ProcServerIteratorRep packet.
   */
  void iteratorReply(tsp00_Int4                cnt,
                     const tgg91_TransNo      *pConsistentView,
                     SAPDB_UInt8              *pOid,
                     const tgg91_PageRef      *pObjSeq,
                     tsp00_Int4                logHistRead,
                     void                     *pIterator,
                     tsp00_Int4                keyLen,
                     unsigned char            *pKey,
                     tgg00_BasisError          DBError)
  {
    LVC_ProcServerIteratorRep *packet;
    preparePacket(packet);

    packet->m_cnt    = cnt;
    if (pConsistentView)
      packet->m_consistentView = *pConsistentView;
    if (pOid)
      memcpy(&packet->m_pOid[0], pOid, cnt);
    if (pObjSeq)
      memcpy(&packet->m_pObjSeq[0], pObjSeq, cnt * sizeof(packet->m_pObjSeq[0]));
    if (pKey) {
      memcpy((unsigned char*)(packet + 1), pKey, keyLen);
      packet->m_size = sizeof(LVC_ProcServerIteratorRep) + keyLen;
    }
    packet->m_logHistRead = logHistRead;
    packet->m_DBError     = DBError;
    packet->m_pIterator   = pIterator;

    send(packet->m_size);
  }

  /*!
   * \brief Send LVC_ProcServerSequenceRep packet.
   */
  void sequenceReply(tgg00_Surrogate    *pSequenceId,
                     tgg00_SeqInfo      *pSequenceInfo,
                     tsp00_Uint4         nextVal,
                     tgg00_BasisError    error)
  {
    LVC_ProcServerSequenceRep *packet;
    preparePacket(packet);

    if (pSequenceId)
      packet->m_sequenceId = *pSequenceId;
    if (pSequenceInfo)
      packet->m_sequenceInfo = *pSequenceInfo;
    packet->m_nextVal = nextVal;
    packet->m_error   = error;

    send(sizeof(LVC_ProcServerSequenceRep));
  }

  /*!
   * \brief Send LVC_ProcServerFileRep packet.
   */
  void fileReply(tsp00_Int4        fileHandle,
                 bool              existsFile,
                 bool              isDirectory,
                 tgg00_BasisError  error)
  {
    LVC_ProcServerFileRep *packet;
    preparePacket(packet);

    packet->m_fileHandle   = fileHandle;
    packet->m_existsFile   = existsFile;
    packet->m_isDirectory  = isDirectory;
    packet->m_error        = error;

    send(sizeof(LVC_ProcServerFileRep));
  }


  /*!
   * \brief Send LVC_ProcServerABAPStreamRep packet.
   */
  void ABAPStreamReply(OmsTypeInt4      tabHandle,
                       OmsTypeInt4      count,
                       OmsTypeInt4      bufSize,
                       tgg00_BasisError error)
  {
    LVC_ProcServerABAPStreamRep *packet;
    preparePacket(packet);

    packet->m_tabHandle = tabHandle;
    packet->m_count     = count;
    packet->m_bufSize   = bufSize;
    packet->m_error     = error;

    send(sizeof(LVC_ProcServerABAPStreamRep));
  }

    /*!
   * \brief Send LVC_ProcServerVersionDictRep packet.
   */
  void versionDictReply(OMS_Context *pVersionContext, tgg00_BasisError error, void *pIter)
  {
    LVC_ProcServerVersionDictRep *packet;
    preparePacket(packet);

    packet->m_pVersionContext = pVersionContext;
    packet->m_error           = error;
    packet->m_pIter           = pIter;

    send(sizeof(LVC_ProcServerVersionDictRep));
  }

  /*!
   * \brief Send LVC_ProcServerErrorStateRep packet.
   */
  void errorStateReply(bool               state,
                       tgg00_BasisError   DBError)
  {
    LVC_ProcServerErrorStateRep *packet;
    preparePacket(packet);

    packet->m_state          = state;
    packet->m_DBError        = DBError;

    send(sizeof(LVC_ProcServerErrorStateRep));
  }


  /*!
   * \brief Convenience function to receive a packet.
   */
  template<class T>
  void receiveReply(const T* &packet)
  {
    receive();

    void *addr;
    size_t maxSize;
    getReceiveArea(addr, maxSize);

    packet = reinterpret_cast<const T*>(addr);
    if (packet->m_code != T::CODE) {
      // wrong reply; throw error
      throw DbpError(e_dbproc_invalid_reply);
    }
  }

  /*!
   * \brief Receive simple reply.
   *
   * This method receives a simple reply packet and eventually raises
   * an exception if error code is set.
   */
  void receiveSimpleReply()
  {
    const LVC_ProcServerSimpleRep *reply;
    receiveReply(reply);
    if (reply->m_return != 0) {
      // TODO: raise error
      throw DbpError(reply->m_return);
    }
  }

protected:
  /*!
   * \brief Send data.
   *
   * Send data to the other party. Eventually wait for communication
   * buffer to be written to underlying system buffers and/or for the
   * segment to be read by the other side (if not read yet).
   *
   * \param size size of the packet in send area (needed only for
   *  non-SHM communication).
   */
  virtual void sendData(size_t size) = 0;

  /*!
   * \brief Receive data.
   *
   * If the packet is not ready, wait for packet to come. Data are then
   * stored in receive area. Receiver must be able to get data size from
   * the packet itself.
   */
  virtual void receiveData() = 0;

  /*!
   * \brief Map an address in ProcServer onto offset (local mode).
   *
   * This method is used in local communication to map an absolute object
   * address in ProcServer address space (but in shared memory segment)
   * into an offset into shared memory segment.
   *
   * \param addr absolute address to map.
   *
   * \return offset associated with the address.
   */
  virtual size_t getOffsetForAddress(void *addr) = 0;

  /*!
   * \brief Map an offset from ProcServer into address in kernel (local mode).
   *
   * This method is used in local communication to map an object offset
   * from ProcServer in shared memory segment into kernel address space
   * (actually in the same shared memory segment).
   *
   * \param offs offset to map.
   *
   * \return absolute address for the offset.
   */
  virtual void *getAddressForOffset(size_t offs) = 0;

  /*!
   * \brief Send from secondary data buffer (non-local mode).
   *
   * This method is used to send data in non-local case from a secondary
   * data buffer. It is used by both ProcServer and kernel to send object
   * data in advance before sending request/reply buffer. This method
   * must make sure not to overwrite primary buffer while sending data.
   * Such packet will be either received into prepared receive buffer
   * (e.g., reply to deref in ProcServer) or queued on the other side
   * (e.g., object update sent to the kernel).
   *
   * Override this method only for non-local communication. It is not used
   * for local communication at all.
   *
   * \param data data to send,
   * \param dataSize size of data.
   *
   * \note Data are raw data, without a header. You must send a packet header
   *  with code = 0 and size = dataSize + sizeof(LVC_ProcServerPacket).
   */
  virtual void sendSecondary(const void *data, size_t dataSize);

private:
  /*!
   * \brief Queue object for read after reply from server received (in ProcServer).
   *
   * This method is used in non-local case to queue objects to be read from
   * kernel connection after the actual reply packet is received.
   */
  void queueObjectForRead(void *addr, size_t size);

  /*!
   * \brief Reset packet queues.
   *
   * This is called either after the queues have been processed or when
   * a reply packet with an error code is received.
   */
  void resetQueue();

  /// Skip continuation packets, reset receive queue and throw exception.
  void skipErrors();

protected:
  friend class LVCPS_LiveCacheSink;

  /*!
   * \brief Indicate type of connection.
   *
   * When this member is set to \c true (default), the processes communicate
   * using shared memory. When set to \c false, data exchange happens on
   * connection as well.
   */
  bool      m_isLocal;

  /// Get allocator for system data.
  SAPDBMem_IRawAllocator *getAllocator()
  {
    return m_alloc;
  }

private:
  friend class LVC_ProcServerPacketHeader;

  /// Register a packet.
  static void registerPacket(int code, void *vtable);

private:
  /// One queue item.
  class QueueItem {
  public:
    size_t  m_objSize;                  ///< Packet size (bit 30 set, if needs deallocate).
    void    *m_addr;                    ///< Packet address.
  };

  enum { 
    QUEUE_MAXSIZE = 1024,               ///< Maximum size of the packet queue.
    QUEUE_FREEMARK = (1<<30),           ///< Free mark.
    QUEUE_SIZEMASK = QUEUE_FREEMARK - 1 ///< Object size mask.
  };

  SAPDBMem_IRawAllocator  *m_alloc;     ///< Allocator for memory allocation.

  size_t    m_recvQueuePtr;             ///< Receive queue pointer.
  size_t    m_recvQueueCount;           ///< Current receive queue packet count.
  QueueItem m_recvQueue[QUEUE_MAXSIZE]; ///< Receive queue.
  size_t    m_writePending;             ///< A write is pending (with given size >0).
};

inline void LVC_ProcServerPacketHeader::registerSelf(int code, void *vtable)
{
  LVC_ProcServerInterface::registerPacket(code, vtable);
}

template<class T>
inline void LVC_ProcServerPacket<T>::registerSelf()
{
  T instance;
  LVC_ProcServerPacketHeader::registerSelf(T::CODE, *reinterpret_cast<void**>(&instance));
}

#endif  // __LVC_PROCSERVER_INTERFACE
