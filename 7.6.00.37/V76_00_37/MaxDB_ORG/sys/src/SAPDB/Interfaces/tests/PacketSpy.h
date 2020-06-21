/*!
  @file           PacketSpy.h
  @author         D025086
  @ingroup        Tests
  @brief          Simulation of comunication and order interface errors.
  @see

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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



\endif
*/
#ifndef IFR_PACKETSPY_H
#define IFR_PACKETSPY_H
#include "SAPDBCommon/SAPDB_ReuseLib.h"
#include "SAPDB/Interfaces/Runtime/IFR_String.h"
#include "SAPDB/Interfaces/SQLDBC/SQLDBC_ClientRuntime.h"

/**
 * @brief Simulation of comunication and order interface errors.
 *
 * This class supports the modification of the return result of the
 * next request/receive call.
 * The normal request/receive call is done but the result of sqlarequest or
 * sqlareceive is modified after the call.
 * If a specific sql code is required a temporary replypacket is returned
 * where the sqlcode is set and the first segment contains a error text part.
 */

class PacketSpyRuntime
  :public SQLDBC_ClientRuntime
{
public:
  PacketSpyRuntime();
  /**
     Sets the RTE error code returned by the next request or receive call.
   */
  void setRTEError(tsp01_CommErr_Enum commErr, const char *errtext, bool OnReceive=true);
  /**
     Sets the SQL error code and message returned on the next recieve
   */
  void setSQLError(tsp00_Int2 sqlcode, const IFR_String& errtext);
  /**
     Sets the request data wich will be send after the next <var>count</var> request calls
     instead of the original data.
   */
  void setRequestData(void *requestData, SAPDB_UInt4 requestDataLength, SAPDB_UInt4 count=0);
  /**
     Sets the reply data wich will be recieved after the next <var>count</var> receive calls
     instead of the original recieve data.
   */
  void setReplyData(void *replyData, SAPDB_UInt4 replyDataLength, SAPDB_UInt4 count=0);

  /**
     Wrapper for sqlarequest Internal used by the packet spy.
   */
  virtual SAPDB_Bool request(SAPDB_Int8  sessionID,
                             void       *requestData,
                             SAPDB_UInt4 requestDataLength,
                             SQLDBC_IRuntime::Error& error);
  /**
     Wrapper for sqlareceive. Internal used by the packet spy.
   */
  virtual SAPDB_Bool receive(SAPDB_Int8 sessionID,
                             void **replyData,
                             SAPDB_Int4& replyDataLength,
                             SQLDBC_IRuntime::Error& error);

private:
  /**
     Clears the RTE error code set by setRTEError
  */
  void clearRTEError();

  /**
     Clears the SQL error code and message set by setSQLError
  */
  void clearSQLError();

  SAPDB_Bool m_requesterror; /*!< true if RTE request error was set */
  SAPDB_Bool m_replyerror;   /*!< true if RTE receive error was set */
  SAPDB_Bool m_sqlerror;     /*!< true if the SQL error was set */
  SQLDBC_IRuntime::Error m_error; /*!< holds the runtime error object */
  tsp00_Int2 m_sqlcode;      /*!< holds the SQL error code set by setSQLError */
  IFR_String m_sqlerrtext;   /*!< holds the SQL error message text set by setSQLError */
  void *m_requestData;       /*!< reference to the request data which will be send instead of the original */
  SAPDB_UInt4 m_requestDataLength; /*!< length of the reuest data to send */
  tsp1_packet *m_retpacket;  /*!< buffer for a reply packet */
  void *m_replyData;         /*!< reference to the recieved data which will be returned instead of the 
                               original data */
  SAPDB_Int4 m_replyDataLength; /*!< length of the reply data which will be returned instead of the original */
  SAPDB_Int4 m_count;        /* number of request calls before the packet spy goes active */
};

/**
   Segment struct
   Forward declaration 
*/
typedef tsp1_segment tsp1_segment;

/**
   Class for handling reply segments
*/
class ReplySegment : public PIn_Segment {
public:
  /**
     Ceates a new (empty) segment
     @param segmParm Reference to a segment 
  */
  ReplySegment(tsp1_segment * segmParm);

  /**
     Ads a new part to a segment
     @param partKind Type of the part to create 
  */
  PIn_Part AddPart(tsp1_part_kind_Param partKind);
  /**
     Fnalizes a part
  */
  void ClosePart(PIn_Part & part);

  /**
     Sets the errorcode in the segment header
  */
  void setErrorCode(tsp00_Int2 retcode);

  /**
     Initialized a segment
  */
  void Init(int offset, int index);

  /**
     Extends the reserved size of a segment
  */
  void ExtendLength(int increment);

};

/**
   Class for handling reply packet
*/
class ReplyPacket : public PIn_RequestPacket {
public:
  /**
     creates a new (empty) packet
  */
  ReplyPacket(tsp1_packet * packetPtr, int size, int unicode = 0);
  /**
     Adds a new segment to a packet
   */
  ReplySegment AddSegment();
};

#endif
