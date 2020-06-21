/*!
  @file           Proc_ServerSession.hpp
  @author         DanielD
  @brief

\if EMIT_LICENCE

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



\endif
*/


#ifndef PROC_SERVERSESSION_HPP
#define PROC_SERVERSESSION_HPP

#include "DBProc/Proc_ISession.hpp"

class Proc_ServerControl;
class RTETask_ITaskClientConnection;
class PIn_RequestWriter;
class PIn_ReplyParser;
class PIn_RequestPacket;
class PIn_ReplySegment;
class SAPDBFields_IFieldSequence;
class SAPDBTrace_Stream;

/*!
  @class          Proc_ServerSession

 */

class Proc_ServerSession : public Proc_ISession
{
public:
    Proc_ServerSession (Proc_ServerControl * serverControlParam);

    ~Proc_ServerSession ();

    virtual void release ();

    bool finishConnectHandshake (RTETask_ITaskClientConnection * connection);

    virtual Proc::Error callDBProc (
                    SQLMan_Context             * context,
                    DBProc_DBProcDescriptor    * procDescriptor,
                    Proc_ParameterCopy         * parameterMap);
    virtual Proc_IServerControl * getController ();
    virtual void dumpOn (SAPDBTrace_Stream & printer, int indent = 0) const;

private:
    bool fillInPacket (
                    PIn_RequestPacket          * requestPacket,
                    SQLMan_Context             * context,
                    DBProc_DBProcDescriptor    * procDescriptor,
                    SAPDBFields_IFieldSequence * parameterMap);

    bool putProcId (
                    PIn_RequestWriter       * request,
                    DBProc_DBProcDescriptor * procDescriptor);

    bool putParameterInfo (
                    PIn_RequestWriter       * request,
                    DBProc_DBProcDescriptor * procDescriptor);

    bool putParameterValues (
                    PIn_RequestWriter          * request,
                    DBProc_DBProcDescriptor    * procDescriptor,
                    SAPDBFields_IFieldSequence * parameterMap);

    bool sendReceive (
                    PIn_RequestPacket       * requestPacket);

    Proc::Error readOutPacket (
                    SQLMan_Context             * context,
                    DBProc_DBProcDescriptor    * procDescriptor,
                    Proc_ParameterCopy         * parameterMap);

    bool readOutputParameters (
                    PIn_ReplySegment           * reply,
                    SQLMan_Context             * context,
                    DBProc_DBProcDescriptor    * procDescriptor,
                    Proc_ParameterCopy         * parameterMap);

    bool readCursorNames (
                    PIn_ReplySegment           * reply,
                    Proc_ParameterCopy         * parameterMap);

    bool passErrorInfo (
                    PIn_ReplySegment        * reply,
                    SQLMan_Context          * context);

    virtual bool sendProcCall (int requestLen);

    virtual bool sendSQLReply (int requestLen);

    virtual bool receive (int * bytesReceived);

    virtual bool isConnected () const;

    virtual void getPacket (
                    void ** packet,
                    int   * packetSize) const;

    virtual void getRequestPacket (PIn_RequestPacket * requestPacket) const;

    virtual bool handleSQLRequest (SQLMan_Context    * context);

private:
    Proc_ServerControl    * serverControl;
    RTETask_ITaskClientConnection * rteSession;
    void * packet;
    int reqLen;
};




#endif
