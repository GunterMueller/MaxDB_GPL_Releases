/*!*********************************************************************

    module:       Proc_ServerSession.cpp

    --------------------------------------------------------------------

    responsible:  DanielD

    special area:

    description:

    --------------------------------------------------------------------


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




***********************************************************************/

#include "DBProc/Proc_ServerSession.hpp"

#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Contracts.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Stream.hpp"
#include "DBProc/Proc_ParameterCopy.hpp"
#include "DBProc/Proc_ServerControl.hpp"
#include "RunTime/Tasking/RTETask_ITaskClientConnection.hpp"
#include "RunTime/Tasking/RTETask_ITask.hpp"
#include "PacketInterface/PIn_Packet.h"
#include "PacketInterface/PIn_RequestWriter.h"
#include "PacketInterface/PIn_Segment.h"
#include "vsp001.h"
#include "hsp77.h"
#include "SQLManager/DBProc/DBProc_DBProcDescriptor.hpp"
#include "SQLManager/SQLMan_Types.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#if defined PROCSERVER_SIMULATION
#include "heo03.h"              /*&variant +procsim */
#else
#include "SQLManager/KernelSQL/KSQL_Runtime.hpp" /*&variant -procsim */
#endif

Proc_ServerSession::
Proc_ServerSession (
    Proc_ServerControl    * serverControlParam)
{
    this->serverControl = serverControlParam;
    this->packet = NULL;
}

/*----------------------------------------*/

Proc_ServerSession::
~Proc_ServerSession ()
{
    this->release ();
}

/*----------------------------------------*/

void Proc_ServerSession::
release ()
{
    if (this->isConnected ()) {
        this->sendProcCall (0);  // to signal end of comm
        this->rteSession->Release ();
        this->rteSession = NULL;
        this->serverControl->releaseSession (this);
    }
}

/*----------------------------------------*/

bool Proc_ServerSession::
finishConnectHandshake (
    RTETask_ITaskClientConnection * connection)
{
    this->rteSession = connection;
    int bytesReceived;
    bool ok = this->receive (&bytesReceived);
    return ok;
}

/*----------------------------------------*/

bool Proc_ServerSession::
sendProcCall (
    int requestLen)
{
    SAPDBERR_PRECONDITION (this->isConnected ());
    SAPDBErr_MessageList msgList;
    RTETask_ITaskClientConnection::ConnRet rc;
    rc = this->rteSession->Send ((SAPDB_Byte* const) this->packet, requestLen, msgList);
    return rc == RTETask_ITaskClientConnection::NoError;
}

/*----------------------------------------*/

bool Proc_ServerSession::
sendSQLReply (
    int requestLen)
{
    SAPDBERR_PRECONDITION (this->isConnected ());
    SAPDBErr_MessageList msgList;
    RTETask_ITaskClientConnection::ConnRet rc;
    rc = this->rteSession->Send ((SAPDB_Byte* const) this->packet, requestLen, msgList);
    return rc == RTETask_ITaskClientConnection::NoError;
}

/*----------------------------------------*/

bool Proc_ServerSession::
receive (
    int * bytesReceived)
{
    SAPDBErr_MessageList msgList;
    RTETask_ITaskClientConnection::ConnRet rc;
    SAPDB_Byte * requestPacket;
    SAPDB_UInt4  requestLen;
    rc = this->rteSession->Receive (this->rteSession->NoSessionTimeout,
        requestPacket, requestLen, msgList);
    this->packet = requestPacket;
    *bytesReceived = this->reqLen = requestLen;
    return rc == RTETask_ITaskClientConnection::NoError;
}

/*----------------------------------------*/

bool Proc_ServerSession::
isConnected () const
{
    return (this->rteSession != NULL);
}

/*----------------------------------------*/

void Proc_ServerSession::
getPacket (
    void ** packet_out,
    int   * packetSize_out) const
{
    *packet_out = this->packet;
    *packetSize_out = this->rteSession->MaxSendDataLength ();
}

/*----------------------------------------*/

void Proc_ServerSession::
getRequestPacket (
    PIn_RequestPacket * requestPacket) const
{
    void  * pack;
    int     packSize;
    this->getPacket (&pack, &packSize);
    *requestPacket = PIn_RequestPacket ( (tsp1_packet *) pack, packSize);
}

/*----------------------------------------*/

Proc::Error Proc_ServerSession::
callDBProc (
    SQLMan_Context             * context,
    DBProc_DBProcDescriptor    * DBProcDescriptor,
    Proc_ParameterCopy         * parameterMap)
{
    bool waitingForProcResult = true;
    PIn_RequestPacket       requestPacket;
    this->getRequestPacket (&requestPacket);

    if (!this->fillInPacket (&requestPacket, context,
            DBProcDescriptor, parameterMap))
    {
        return Proc::internalError;
    }
    if (!this->sendProcCall (requestPacket.Length ())) {
        return Proc::communicationFailed;
    }
    while (waitingForProcResult) {
        int receiveLength;
        if (!this->receive (&receiveLength)) {
            return Proc::communicationFailed;
        }
        waitingForProcResult = this->handleSQLRequest (context);
    }

    Proc::Error result = this->readOutPacket (context, DBProcDescriptor, parameterMap);
    return result;
}

/*----------------------------------------*/

Proc_IServerControl * Proc_ServerSession::
getController ()
{
    return this->serverControl;
}

/*----------------------------------------*/

void Proc_ServerSession::
dumpOn (
    SAPDBTrace_Stream & printer,
    int indent) const
{
    printer << SAPDB_Indent (indent) << "Proc_ServerSession ("
        << SAPDB_ToStringClass (this) << ")\n";
    printer << SAPDB_Indent (indent + 1) << "serverControl = "
        << SAPDB_ToStringClass (this->serverControl) << "n";
    printer << SAPDB_Indent (indent + 1) << "rteSession = "
        << SAPDB_ToStringClass (this->rteSession) << "n";
}

/*----------------------------------------*/

bool Proc_ServerSession::
fillInPacket (
    PIn_RequestPacket          * requestPacket,
    SQLMan_Context             * context,
    DBProc_DBProcDescriptor    * procDescriptor,
    SAPDBFields_IFieldSequence * parameterMap)
{
    PIn_RequestWriter       request (*requestPacket);
    tsp1_sqlmode_Enum       sqlMode = tsp1_sqlmode::fromConst (sp1sm_internal);

    request.Reset ();
    request.AddSegment (sp1m_procserv_call, sqlMode);
    if (!this->putProcId (&request, procDescriptor)) {
        return false;
    }
    if (!this->putParameterInfo (&request, procDescriptor)) {
        return false;
    }
    if (!this->putParameterValues (&request, procDescriptor, parameterMap)) {
        return false;
    }
    request.Close ();
    return true;
}

/*----------------------------------------*/

static void
addProcIdField (
    PIn_Part   * procidPart,
    const char * value)
{
    int len = strlen (value);
    tsp00_Byte lenfield [2];

    lenfield [0] = len / 256;
    lenfield [1] = len % 256;
    procidPart->AddData (lenfield, sizeof (lenfield));
    procidPart->AddData (value, len);
}

/*----------------------------------------*/

bool Proc_ServerSession::
putProcId (
    PIn_RequestWriter       * request,
    DBProc_DBProcDescriptor * procDescriptor)
{
    PIn_Part * idPart = request->AddPart (sp1pk_procid);

    addProcIdField (idPart, procDescriptor->GetExternalInfo ());
    addProcIdField (idPart, procDescriptor->GetCreateDateTime ());
    return true;
}

/*----------------------------------------*/

static void
fillParamInfo (
    tsp1_param_info         * paramInfo,
    char                      io_type,
    tsp00_DataType_Enum       data_type,
    tsp00_Uint1               frac,
    tsp00_Int2                length,
    tsp00_Int2                in_out_len,
    tsp00_Int4                bufpos)
{
    paramInfo->sp1i_io_type.becomes ((tsp1_param_io_type_Enum)io_type);
    paramInfo->sp1i_data_type.becomes (data_type);
    paramInfo->sp1i_frac       = frac;
    paramInfo->sp1i_length     = length;
    paramInfo->sp1i_in_out_len = in_out_len;
    paramInfo->sp1i_bufpos()   = bufpos;

}

/*----------------------------------------*/

static char
getIOType (
    DBProc_DBProcDescriptor::ParameterIterator * param)
{
    if (param->IsInputParameter ()) {
        if (param->IsOutputParameter ()) {
            return sp1io_inout;
        }
        else {
            return sp1io_input;
        }
    }
    else {
        return sp1io_output;
    }
}

/*----------------------------------------*/

bool Proc_ServerSession::
putParameterInfo (
    PIn_RequestWriter       * request,
    DBProc_DBProcDescriptor * procDescriptor)
{
    PIn_Part * part = request->AddPart (sp1pk_shortinfo);
    DBProc_DBProcDescriptor::ParameterIterator params
        = procDescriptor->FirstParameter ();
    int bufpos = 1;

    while (params) {
        /*
         * build param description
         */
        tsp1_param_info paramInfo;
        fillParamInfo (&paramInfo, getIOType (&params),
            params.ParamDataType (),
            params.Fraction (), params.DataLength (),
            params.InOutLength (), bufpos);
        /*
         * copy to part
         */
        part->AddArgument (&paramInfo, sizeof (paramInfo));
        /*
         * prepare for next iteration
         */
        bufpos += params.InOutLength ();
        ++params;
    }
    return true;
}

/*----------------------------------------*/

static char
getFillChar (
    DBProc_DBProcDescriptor::ParameterIterator * params)
{
    char result;
    switch (params->ParamDataType ()) {
        case dcha:
        case dvarchara:
        case ddate:
        case dtime:
        case dtimestamp:
            result = ' ';
            break;
        default:
            result = '\0';
            break;
    }
    return result;
}

/*----------------------------------------*/

bool Proc_ServerSession::
putParameterValues (
    PIn_RequestWriter          * request,
    DBProc_DBProcDescriptor    * procDescriptor,
    SAPDBFields_IFieldSequence * parameterMap)
{
    PIn_Part * part = request->AddPart (sp1pk_data);
    DBProc_DBProcDescriptor::ParameterIterator params
        = procDescriptor->FirstParameter ();
    SAPDBFields_AttributedField field;
    SAPDBErr_MessageList        errlist;
    int bufpos = 1;

    while (params) {
        if (!params.IsInputParameter ()) {
            ++params;
            continue;
        }
        if(parameterMap->GetField(params.ParameterNo(), field, errlist)
           != SAPDBFields_IFieldSequence::fieldFound)
            RTE_Crash(errlist);
        SAPDB_Byte * raw = field.GetDataPtr () + 1;
        if (raw [-1] == csp_undef_byte) {
            raw = NULL;
        }
        part->AddParameterArg (raw,
            bufpos, field.GetLength () - 1, params.InOutLength (),
            getFillChar (&params));
        /*
         * prepare for next iteration
         */
        bufpos += params.InOutLength ();
        ++params;
    }
    return true;
}

/*----------------------------------------*/

bool Proc_ServerSession::
sendReceive (PIn_RequestPacket       * requestPacket)
{
    if (!this->sendProcCall (requestPacket->Length ())) {
        return false;
    }
    int receiveLength;
    if (!this->receive (&receiveLength)) {
        return false;
    }
    return true;
}

/*----------------------------------------*/

Proc::Error Proc_ServerSession::
readOutPacket (
    SQLMan_Context             * context,
    DBProc_DBProcDescriptor    * procDescriptor,
    Proc_ParameterCopy         * parameterMap)
{
    Proc::Error result;
    void * rawPacket;
    int    packSize;
    this->getPacket (& rawPacket, &packSize);
    PIn_ReplyPacket  replyPacket ((tsp1_packet*) rawPacket);
    PIn_ReplySegment reply = replyPacket.GetFirstSegment ();
    /*
     * check for error message
     */
    if (reply.ErrorCode () == 0) {
        this->readOutputParameters (&reply, context, procDescriptor, parameterMap);
        this->readCursorNames (&reply, parameterMap);
        result = Proc::ok;
    }
    else {
        this->passErrorInfo (&reply, context);
        result = Proc::udeReturnedError;
    }
    return result;
}

/*----------------------------------------*/

bool Proc_ServerSession::
readOutputParameters (
    PIn_ReplySegment           * reply,
    SQLMan_Context             * context,
    DBProc_DBProcDescriptor    * procDescriptor,
    Proc_ParameterCopy         * parameterMap)
{
    const PIn_Part part = reply->FindPart (sp1pk_data);
    if (part.IsValid ()) {
        DBProc_DBProcDescriptor::ParameterIterator params
            = procDescriptor->FirstParameter ();
        SAPDBFields_Field field;
        int bufpos = 1;
        const tsp00_Byte * rawData = part.GetReadData ();

        parameterMap->Reset (part.Length ());
        while (params) {
            int inoutlen = params.InOutLength ();
            if (!params.IsOutputParameter ()) {
                ++params;
                bufpos += inoutlen;
                continue;
            }
            const tsp00_Byte * data = rawData + bufpos;
            parameterMap->StoreParameter (params.ParameterNo (),
                data, inoutlen - 1, data [-1]);
            /*
             * prepare for next iteration
             */
            bufpos += inoutlen;
            ++params;
        }

    }
    return true;
}


/*----------------------------------------*/

bool Proc_ServerSession::
readCursorNames (
    PIn_ReplySegment           * reply,
    Proc_ParameterCopy         * parameterMap)
{
    const PIn_Part part = reply->FindPart (sp1pk_resulttablename);
    if (part.IsValid ()) {
        parameterMap->StoreCursorName (part.GetReadData (),
            part.Length ());
    }
    return true;
}

/*----------------------------------------*/

bool Proc_ServerSession::
passErrorInfo (
    PIn_ReplySegment        * reply,
    SQLMan_Context          * context)
{
    static const int maxMsgLenC = 70;
    static const int prefixLenC =  6; // "[UDE] "
    const PIn_Part part = reply->FindPart (sp1pk_errortext);

    if (part.IsValid ()) {
        /*
         * copy error code and message
         */
        char buf [100];
        int msglen = part.GetRawHeader ()->sp1p_buf_len;
        if (msglen > (maxMsgLenC - prefixLenC)) {
            msglen = maxMsgLenC - prefixLenC;
        }
        sp77sprintf (buf, sizeof (buf), " [UDE] %.*s", msglen, part.GetReadData ());
        context->SetStopMsg (reply->ErrorCode (), buf, strlen (buf));
        /*
         * copy messagelist part if one exists
         */
        const PIn_Part part = reply->FindPart (sp1pk_message_list);
        if (part.IsValid ()) {
            context->CopyResultPart (part);
        }
    }
    else {
        context->SetStopMsg (reply->ErrorCode (),
                " ", 1);
    }
    return true;
}

/*----------------------------------------*/

bool Proc_ServerSession::
handleSQLRequest (
    SQLMan_Context    * context)
{
    /*
     * get raw packet
     */
    void * rawPacket;
    int    packSize;
    this->getPacket (& rawPacket, &packSize);
    PIn_ReplyPacket  replyPacket ((tsp1_packet*) rawPacket);
    PIn_ReplySegment reply = replyPacket.GetFirstSegment ();
    /*
     * check for sql flag
     */
    if (reply.GetRawHeader ()->sp1s_segm_kind () == sp1sk_procreply) {
        /*
         * return false to end loop and return from proc call
         */
        return false;
    }
    /*
     * executesql
     */
    void          * sqlReplyBuf;
    tsp00_Int4      sqlReplyLen;
    KSQL_Runtime& runtime = context->GetSQLDBCRuntime();
    runtime.request(rawPacket);
    runtime.receive(&sqlReplyBuf, sqlReplyLen);
    SAPDBERR_MISSINGCHECK (sqlReplyBuf == rawPacket);
    /*
     * send reply back to proc server
     */
    PIn_ReplyPacket  sqlReplyPacket ((tsp1_packet*) sqlReplyBuf);
    this->sendSQLReply (sqlReplyLen);
    /*
     * return true to continue loop and wait for next packet
     */
    return true;
}

/*----------------------------------------*/

