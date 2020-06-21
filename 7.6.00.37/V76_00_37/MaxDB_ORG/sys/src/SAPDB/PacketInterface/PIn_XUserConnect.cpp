/*!================================================================

 module:        PIn_XUserConnect.cpp

 -------------------------------------------------------------------

 responsible:   TTK

 special area:  SQL

 description:   XUser connect

 -------------------------------------------------------------------

 Copyright (c) 2000-2005 SAP AG




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




 ===================================================================*/

#include "heo02.h"
#include "heo03.h"
#include "gsp01.h"
#include "hsp02.h"

#include "SAPDB/PacketInterface/PIn_FileTraceStream.h"
#include "SAPDB/PacketInterface/PIn_RequestWriter.h"
#include "SAPDB/PacketInterface/PIn_XUserConnect.h"

/*----------------------------------------*/

PIn_ReplyPacket PIn_Sqlaexecute (
    tsp00_Int4 connection,
    PIn_RequestPacket & requestPacket)
{
    tsp00_ErrText errText;
    tsp01_CommErr commErr;
    tsp1_packet * rawRequest;
    void        * rawPointer = NULL;
    tsp1_packet * rawReply;
    tsp00_Int4    replyLen;

    requestPacket.TraceOn (PIn_GlobalTrace, 1000);
    rawRequest = requestPacket.GetRawPacket ();
    sqlarequest (connection, rawRequest, requestPacket.Length (),
        errText, commErr);
    if (commErr == commErrOk_esp01) {
        sqlareceive (connection, &rawPointer, replyLen, errText, commErr);
    }
    if (commErr != commErrOk_esp01) {
        //fprintf (stderr, "Communication failed: [%d] %.*s\n", (int) commErr,
        //    sizeof (errText), errText.asCharp ());
    }
    rawReply = REINTERPRET_CAST (tsp1_packet *, rawPointer);
    PIn_ReplyPacket result (rawReply);
    return result;
}

/*----------------------------------------*/

static int PIn_SQL_Connect (
    tsp4_xuser_record       & xuser,
    tsp00_Int4                connectionHandle,
    PIn_RequestPacket    & requestPacket)
{
    int sqlRC;
    PIn_RequestWriter writer (requestPacket);
    PIn_Part * part;
    /* tsp00_CryptPw stCrypt; */
    tsp00_Byte * writeTarget;
    tsp00_TermId termid;

    part = writer.InitDbsf (sp1sm_oracle, false,
			    "CONNECT \"%.*s\" IDENTIFIED BY ? ",
        sizeof (xuser.xu_user), xuser.xu_user.asCharp ());
    // todo: put other connect options

    /*
     * put password
     */
    part = writer.AddPart (sp1pk_data);
    part->AddArgument ();
    //s02applencrypt(xuser.xu_password, stCrypt);
    writeTarget = part->GetWriteData ();
    writeTarget [0] = csp_defined_byte;
    memcpy (writeTarget + 1, xuser.xu_password, sizeof (xuser.xu_password));
    part->ExtendLength (1 + sizeof (tsp00_CryptPw));
    writeTarget += 1 + sizeof (tsp00_CryptPw);
    /*
     * put termid
     */
    sqltermid (termid);
    writeTarget [0] = csp_defined_byte;
    memcpy (writeTarget + 1, termid, sizeof (termid));
    part->ExtendLength (1 + sizeof (termid));
    /*
     * send to db
     */
    writer.Close ();
    PIn_ReplyPacket reply = PIn_Sqlaexecute (
        connectionHandle, requestPacket);
    if (reply.IsValid ()) {
        PIn_ReplySegment segment = reply.GetFirstSegment ();
        sqlRC = segment.ErrorCode ();
    }
    else {
        sqlRC = -8888;
    }
    /*
     * return result
     */
    return sqlRC;
}

/*----------------------------------------*/

externCpp int PIn_XuserConnect (
    tsp4_xuser_record       & xuser,
    int                       packetCount,
    tsp00_Int4              & connectionHandle,
    PIn_RequestPacket      requestPacket [])
{
    tsp00_ErrText errText;
    tsp01_CommErr commErr;
    tsp00_Int4 packetSize;
    void * packetList [2];
    int sqlRC;

    /*
     * RTE connect
     */
    sqlaconnect (0, xuser.xu_servernode, xuser.xu_serverdb,
        srvUser_esp01, packetCount, connectionHandle,
        packetSize, packetList, errText, commErr);
    if (commErr != commErrOk_esp01) {
        // should really generate proper returncode
        // this has to wait until design for error handling exists
        return -8888;
    }
    /*
     * init request packets
     */
    for (int i = 0; i < packetCount; ++i) {
        requestPacket [i] = PIn_RequestPacket (
            REINTERPRET_CAST (tsp1_packet*, packetList [i]), packetSize);
        requestPacket [i].InitVersion ("CPC");  // should be parameter
    }
    /*
     * SQL connect
     */
    sqlRC = PIn_SQL_Connect (xuser, connectionHandle, requestPacket [0]);
    if (sqlRC != 0) {
        sqlarelease (connectionHandle);
        connectionHandle = -1;
        for (int i = 0; i < packetCount; ++i) {
            requestPacket [i].Invalidate ();
        }
    }
    return sqlRC;
}

/*----------------------------------------*/


