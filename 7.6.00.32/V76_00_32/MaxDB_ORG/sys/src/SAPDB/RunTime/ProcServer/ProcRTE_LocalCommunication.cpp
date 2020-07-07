/*!*********************************************************************

    @file         ProcRTE_LocalCommunication.cpp

    @author       DanielD

    @brief        simulated local communication from
                    proc server to kernel

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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
***********************************************************************/

#include "RunTime/ProcServer/ProcRTE_LocalCommunication.h"

#if defined (WIN32)
#include <windows.h>
#include "gos003.h"
#endif
#include "heo46.h"
#include "geo007_2.h"

#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"

 /*
 * ========== temporary implementation of proc server communication
 */
const int lenBytesC = 4;
typedef SOCKET procrte_handle;
const procrte_handle invalidHandleC = INVALID_SOCKET;
const int socketErrorC = SOCKET_ERROR;
static bool socketsInitialized = false;
static WSADATA wsaData;
#define INIT_SOCKET_LIB() if (!socketsInitialized) {\
    WSAStartup (MAKEWORD(1, 1), &wsaData);\
    socketsInitialized = true;}
#define LASTERROR GetLastError()

class SimulatedComm : public TOSCM_COMM_CLASS
{
public:
    virtual tsp01_CommErr_Enum connect (
                               CONNECT_PARAM_REC       *pConnParam,
                               PSZ                      pszServerDB,
                               PSZ                      pszDBRoot,
                               PSZ                      pszServerPgm,
                               tsp00_ErrTextc  &           errtext);

    virtual tsp01_CommErr_Enum release ();

    virtual tsp01_CommErr_Enum request (
                               void             *sql_packet,
                               tsp00_Int4          length,
                               tsp00_ErrTextc  &      errtext);

    virtual tsp01_CommErr_Enum receive (
                               void            **                 sql_packet,
                               tsp00_Int4         &               length,
                               tsp00_ErrTextc  &      errtext);

    virtual tsp01_CommErr_Enum replyAvailable (
                               tsp00_ErrTextc  &      errtext);

    virtual tsp01_CommErr_Enum cancel (
                               tsp00_ErrTextc  &     errtext);
private:
     char                *packetMem;
     char                *requestMem;
     procrte_handle       handle;
};


/*----------------------------------------*/

#undef MF__
#define MF__ MOD__"procserver_connect"
tsp01_CommErr_Enum SimulatedComm::
connect (
    CONNECT_PARAM_REC*pConnParam,
    PSZ               pszServerDB,
    PSZ               pszDBRoot,
    PSZ               pszServerPgm,
    tsp00_ErrTextc & errtext)
{
    const char * portString = pszServerDB + 2;
    const char * sizeString = strchr (portString, ':') + 1;
    int          port;
    int          packetSize;
    int          allocatedSize;
    APIRET       rc;
    unsigned int ulCnt;

    /*
     * parse url
     */
    port = atoi (portString);
    packetSize = atoi (sizeString);
    /*
     * alloc packet
     */
    allocatedSize = 0;
    pConnParam->ulMinReplySize = 100;
    pConnParam->ulPacketSize = packetSize + pConnParam->ulMinReplySize;
    pConnParam->ulMaxDataLen = pConnParam->ulPacketSize
        - (2 * RTE_HEADER_SIZE);
    allocatedSize += pConnParam->ulPacketCnt * pConnParam->ulPacketSize;
    rc = ALLOC_MEM ((PPVOID)&this->packetMem, allocatedSize);
    if( rc != NO_ERROR ) {
        sql46c_build_error_string ( errtext, ERRMSG_ALLOC_MEMORY, rc );
        return commErrNotOk_esp01;
    }
    for ( ulCnt = 0; ulCnt < pConnParam->ulPacketCnt; ulCnt++ )
    {
      pConnParam->pCommPacketList[ulCnt] = (PCOMM_PACKET_REC)
              (this->packetMem + (ulCnt * pConnParam->ulPacketSize));
    }
    /*
     * connect to socket
     */
    INIT_SOCKET_LIB();
    {
        unsigned long destAddr = inet_addr ("127.0.0.1");
        struct sockaddr_in destSockAddr;

        memcpy (&destSockAddr.sin_addr, &destAddr, sizeof (destAddr));
        destSockAddr.sin_port = htons ((u_short) port);
        destSockAddr.sin_family = AF_INET;

        this->handle = socket (AF_INET, SOCK_STREAM, 0);
        if (this->handle == invalidHandleC) {
            strcpy (errtext, "socket () failed");
            return commErrNotOk_esp01;
        }
        rc = ::connect (this->handle, (const struct sockaddr *)&destSockAddr,
            sizeof (destSockAddr));
        if (rc == socketErrorC) {
            closesocket (this->handle);
            this->handle = invalidHandleC;
            strcpy (errtext, "connect () failed");
            return commErrNotOk_esp01;
        }
    }
    return commErrOk_esp01;
}

/*----------------------------------------*/

#undef MF__
#define MF__ MOD__"procserver_release"
tsp01_CommErr_Enum SimulatedComm::
release ()
{
    /*
     * close socket
     */
    closesocket (this->handle);
    /*
     * free packet
     */
    FREE_MEM (this->packetMem);
    FREE_MEM (this);
    return commErrOk_esp01;
}
/*----------------------------------------*/
#if 0
#include <stdio.h>

static FILE * tracer = NULL;

static const char hexDigitsC [] = "0123456789abcdef";

static void
traceSocketComm (
    const char * comment,
    const char * sendBuf,
    int sendLen)
{
    const unsigned int bytesPerLineC = 16;
    #define lineSizeC 79
    char line [lineSizeC + 2];
    const unsigned char * bufPtr = (const unsigned char *) sendBuf;
    unsigned int endPos = sendLen;
    int chunkPos;
    unsigned int pos;

    if (tracer == NULL) {
        tracer = fopen ("D:/dbs/wrk/DB76/jdbc-comm.log", "w");
    }
    fprintf (tracer, "%s %d bytes\n", comment, sendLen);
    for (pos = 0; pos < endPos; pos += bytesPerLineC, bufPtr += bytesPerLineC) {
        int linePos;
        unsigned int tmpPos;
        int posDigit;
        int bytesInLine;

        memset (line, ' ', lineSizeC);
        /*
         * write position (4 bytes)
         */
        linePos = 0;
        tmpPos = pos;
        for (posDigit = 3; posDigit >= 0; --posDigit) {
            char hexDigit = hexDigitsC [tmpPos % 16];
            line [linePos + posDigit] = hexDigit;
            tmpPos /= 16;
        }
        /*
         * write 16 * (hex hex blank)
         */
        linePos = 6;
        bytesInLine = (endPos - pos) > bytesPerLineC
                ? bytesPerLineC
                : (endPos - pos);
        for (chunkPos = 0; chunkPos < bytesInLine; ++chunkPos) {
            unsigned char byte = bufPtr [chunkPos];
            line [linePos] = hexDigitsC [byte / 16];
            line [linePos + 1] = hexDigitsC [byte % 16];
            linePos += 3;
        }
        /*
         * write | (16 * plain) |
         */
        linePos = 54;
        line [linePos] = '|';
        ++linePos;
        for (chunkPos = 0; chunkPos < bytesInLine; ++chunkPos) {
            unsigned char byte = bufPtr [chunkPos];
            if (byte < 32) {
                byte = '.';
            }
            line [linePos] = byte;
            ++linePos;
        }
        line [linePos] = '|';
        line [linePos + 1] = '\n';
        line [linePos + 2] = '\0';
        /*
         * output
         */
        fputs (line, tracer);
        fflush (tracer);
    }
}
#endif
/*----------------------------------------*/

#undef MF__
#define MF__ MOD__"procserver_request"
tsp01_CommErr_Enum SimulatedComm::
request (
    void             *sql_packet,
    tsp00_Int4          length,
    tsp00_ErrTextc & errtext)
{
    /*
     * send length
     */
    int rc;
    bool ok = false;
    unsigned char lenbuf [sizeof (tsp00_Int4)];

    this->requestMem = (char *) sql_packet;
    // ProcRTE_Runtime::log ("sending %d bytes", length);
    //fprintf (stdout, "sending %d bytes", length);
    lenbuf [3] = length % 256;
    lenbuf [2] = (length >>  8) % 256;
    lenbuf [1] = (length >> 16) % 256;
    lenbuf [0] = (length >> 24) % 256;
    /*traceSocketComm ("sending len", (unsigned char *) lenbuf, sizeof (lenbuf));*/
    rc = send (this->handle, (const char *) lenbuf, sizeof (lenbuf), 0);
    /*
     * send data
     */
    if (rc == sizeof (lenbuf)) {
        /*traceSocketComm ("sending", (unsigned char *) sql_packet, length);*/
        if (length > 0) {
            rc = send (this->handle, (const char *) sql_packet, length, 0);
            if (rc == length) {
                ok = true;
            }
            else {
                ok = false;
                strcpy (errtext, "send data failed");
            }
        }
        else {
            ok = true;
        }
    }
    else {
        strcpy (errtext, "send length failed");
    }
    if (ok) {
        return commErrOk_esp01;
    }
    else {
        return commErrNotOk_esp01;
    }
}

/*----------------------------------------*/

#undef MF__
#define MF__ MOD__"procserver_receive"
tsp01_CommErr_Enum SimulatedComm::
receive (
    void            **sql_packet,
    tsp00_Int4         &length,
    tsp00_ErrTextc & errtext)
{
    int rc;
    unsigned char lenbuf [sizeof (tsp00_Int4)];
    int bytesRemaining;
    int bytesRead = 0;
    char * receivePtr;

    rc = recv (this->handle, (char *) lenbuf, sizeof (lenbuf), 0);
    if (rc != sizeof (lenbuf)) {
        strcpy (errtext, "recv of length failed");
        return commErrNotOk_esp01;
    }
    /*traceSocketComm ("receiving len", (unsigned char *) lenbuf, sizeof (lenbuf));*/
    bytesRemaining = (lenbuf [0] << 24)
                   + (lenbuf [1] << 16)
                   + (lenbuf [2] <<  8)
                   + (lenbuf [3]);
    if (this->requestMem == NULL) {
        strcpy (errtext, "no previous sqlarequest");
        return commErrNotOk_esp01;
    }
    receivePtr = this->requestMem;
    while ((rc > 0) && (bytesRemaining > 0)) {
        rc = recv (this->handle, receivePtr, bytesRemaining, 0);
        if (rc < 0) {
            int osRC = GetLastError ();
            strcpy (errtext, "recv data failed");
            return commErrNotOk_esp01;
        }
        if (rc > 0) {
            bytesRemaining -= rc;
            bytesRead += rc;
            receivePtr += rc;
        }
    }
    /*traceSocketComm ("receiving", (unsigned char *) this->requestMem, bytesRead);*/
    *sql_packet = this->requestMem - offsetof (COMM_PACKET_REC, pDataPart);
    length = bytesRead;
    this->requestMem = NULL;
    return commErrOk_esp01;
}

/*----------------------------------------*/

#undef MF__
#define MF__ MOD__"procserver_replyavailable"
tsp01_CommErr_Enum SimulatedComm::
replyAvailable (
    tsp00_ErrTextc & errtext)
{
    strcpy (errtext, "not implemented");
    return commErrNotOk_esp01;
}

/*----------------------------------------*/

#undef MF__
#define MF__ MOD__"procserver_cancel"
tsp01_CommErr_Enum SimulatedComm::
cancel (
    tsp00_ErrTextc & errtext)
{
    strcpy (errtext, "not implemented");
    return commErrNotOk_esp01;
}

/*----------------------------------------*/

#undef MF__
#define MF__ MOD__"procserver_constructor"
static TOSCM_COMM_CLASS *
procserver_constructor (
    void          * commInfo,
    tsp00_ErrTextc & errtext,
    tsp01_CommErr_Enum * returncode)
{
    SimulatedComm  *self;
    APIRET              rc;

    // alloc self
    rc = ALLOC_MEM ((PPVOID)&self, sizeof(SimulatedComm) );
    if( rc != NO_ERROR ) {
        sql46c_build_error_string ( errtext, ERRMSG_ALLOC_MEMORY, rc );
        *returncode = commErrNotOk_esp01;
        return NULL;
    }
    memset (self, '\0', sizeof (SimulatedComm));
    new (self) SimulatedComm;
    *returncode = commErrOk_esp01;
    return self;
}

/*----------------------------------------*/

externC void
procRTE_installConstructor ()
{

    TOSCM_COMMSTRUCTOR_FT * sql03c_constructorPointer;
    sql03c_constructorPointer = (TOSCM_COMMSTRUCTOR_FT *) procserver_constructor;

}

