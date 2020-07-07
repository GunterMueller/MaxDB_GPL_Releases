/*!
  @file           ven905.c
  @author         DanielD
  @brief          Client RunTime: local dbm communication class

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

#define MOD__ "ven905.c:"

#include <stddef.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "gen00.h"
#include "gsp09.h"
#include "hsp77.h"
#include "gen003.h"
#include "geo001.h"
#include "geo57.h"
#include "gen42.h"
#include "hen40.h"
#include "RunTime/RTE_IniFileHandling.h"
#include "SAPDBCommon/SAPDB_Names.h"
#include "gen904.h"
#include "heo46.h"

#if 0
#define ERROR_TRACE(args) printf args
#else
#define ERROR_TRACE(args)
#endif

/*----------------------------------------*/

/* can't import declarations from C++ */

typedef struct RTEReqRespDataT {   /* from teo42_RTEReqRespData*/
    tsp00_Uint1    osType;
    tsp00_Uint1    filler;
    tsp00_Uint2    requestVersion;

    tsp00_Uint4    varpartLen;
} RTEReqRespDataT;

#define HEADER_OVERHEAD_C sizeof (rte_header)

#define OSTYPE_WIN32_EO42                  1
#define OSTYPE_UNIX_OS_EO42                2

/* this module only runs on UNIX */
#define OSTYPE_LOCAL_OS_EN905 OSTYPE_UNIX_OS_EO42

#define REQUEST_VERSION_EN905 0

/*  from veo907.cpp */
#define VARPART_VAL_DBROOT_EO44                    0x72   /* = r */
#define VARPART_VAL_SERVERDB_EO44                  0x65   /* = d */
#define VARPART_VAL_SENDERNODEID_EO44              0x6e   /* = n */
#define VARPART_VAL_SENDERPID_EO44                 0x70   /* = p */

/* --- request/response types ( 1 - 32 ) reserved by geo42.h */
#define REQ_RESP_TYPE_RAW_DATA                 1
#define REQ_RESP_TYPE_EVENTLIST                2

/* from geo44.h */
#define REQ_RESP_TYPE_BUFFER_SIZE_REQUEST           33
#define REQ_RESP_TYPE_BUFFER_SIZE_RESPONSE          34
#define REQ_RESP_TYPE_SET_TRANSFER_BUFFER_REQUEST   35
#define REQ_RESP_TYPE_SET_TRANSFER_BUFFER_RESPONSE  36

#define REQ_RESP_TYPE_CANCEL_REQUEST                125
#define REQ_RESP_TYPE_DISCONNECT_REQUEST            127
#define REQ_RESP_TYPE_DISCONNECT_RESPONSE           128

/*  from veo907.cpp */
#define REQ_RESP_TYPE_SET_DBMSERVER_PARAMS_REQUEST           129
#define REQ_RESP_TYPE_SET_DBMSERVER_PARAMS_RESPONSE          130
#define REQ_RESP_TYPE_DBMSERVER_AUTOLOGON_REQUEST            131
#define REQ_RESP_TYPE_DBMSERVER_AUTOLOGON_RESPONSE           132

/* --- VARPART Values ---       */
/*  see veo44.cpp               */
#define VARPART_VAL_MAX_TRANSFER_BUFFER_SIZE_EO44  0x4D   /* = M */
#define VARPART_VAL_TRANSFER_BUFFER_SIZE_EO44      0x54   /* = T */
#define VARPART_VAL_MIN_RESPONSE_PACKET_SIZE_EO44  0x55   /* = U */

static int requestToLocalManager_MF (connection_info     *cip,
                                     tsp00_ErrTextc pErrText);

static int receiveFromLocalManager_MF (connection_info     *cip,
                                       tsp00_ErrTextc pErrText);

#if 0
/* keep around for debugging */
static char hexDigitsC [] = "0123456789abcdef";

static void
dumpHex (const void * bufParam, int len)
{
    const unsigned char * buf = (unsigned char *) bufParam;
    int offset = 0;
    char line [81];
    char * cursor;
    int i;

    while (offset < len) {
        /*
         * init
         */
        int charsInChunk = 16;
        if (len - offset < 16) {
            charsInChunk = len - offset;
        }
        SAPDB_memset (line, ' ', sizeof (line));
        line [80] = '\0';
        cursor = line;
        /*
         * offset
         */
        cursor [0] = '>';
        ++cursor;
        sprintf (cursor, "%04d  ", offset);
        cursor += 6;
        /*
         * hex
         */
        for (i = 0; i < charsInChunk; ++i) {
            int high = buf [offset + i] / 16;
            int low  = buf [offset + i] % 16;
            cursor [0] = hexDigitsC [high];
            cursor [1] = hexDigitsC [low];
            cursor += 3;
            switch (i) {
                case 3:
                case 7:
                case 11:
                    cursor [0] = '-';
                    cursor += 2;
                    break;
                default:
                    break;
            }
        }
        /*
         * chars
         */
        cursor = line + 61;
        cursor [0] = '|';
        ++cursor;
        for (i = 0; i < charsInChunk; ++i) {
            if (buf [offset + i] < 32) {
                cursor [i] = '.';
            }
            else {
                cursor [i] = buf [offset + i];
            }
        }
        cursor += charsInChunk;
        cursor [0] = '|';
        ++cursor;
        /*
         * output
         */
        cursor [0] = '\0';
        fprintf (stdout, "%s\n", line);
        offset += charsInChunk;
    }
}

/*----------------------------------------*/

static void
dumpHexWithComment (
    const char * comment,
    const void * bufParam,
    int len)
{
    fprintf (stdout, "%s\n", comment);
    dumpHex (bufParam, len);
}

#else

#define dumpHex(bufParam, len)
#define dumpHexWithComment(comment, bufParam, len)
#endif

static void terminateChildProcess_MF (connection_info     *cip);

/*----------------------------------------*/

static void initHeader_MF (rte_header * header)
{
    header->rh_act_send_len = 0;
    header->rh_protocol_id = 0;
    header->rh_mess_class = 0;
    header->rh_rte_flags = 0;
    header->rh_residual_packets = 0;
    header->rh_sender_ref = 0;
    header->rh_receiver_ref = 0;
    header->rh_rte_return_code = 0;
    header->rh_new_swap_type = sql42_new_swap_type ();
    header->rh_filler1 = 0;
    header->rh_max_send_len = 0;
}

/*----------------------------------------*/

static void fillHeader_MF (
    rte_header * header,
    int          varpartLen)
{
    /*
     * fill rte_header
     */
    header->rh_act_send_len = HEADER_OVERHEAD_C + varpartLen;
    header->rh_max_send_len = HEADER_OVERHEAD_C + varpartLen;
    if (header->rh_mess_class == RSQL_USER_DATA_REQUEST_EO003) {
        /*
         * translate from remote request type
         * to local request type
         */
        header->rh_mess_class = REQ_RESP_TYPE_RAW_DATA;
    }
}

/*----------------------------------------*/

static void freePackets_MF (connection_info * cip,
                            int               count)
{
    int i;
    for (i = 0; i < count; ++i) {
        if (cip->ci_packet [i] != NULL) {
            FREE_MEM_EO57 (cip->ci_packet [i]);
            cip->ci_packet [i] = NULL;
            cip->ci_packetData [i] = NULL;
            cip->ci_packet_size = -1;
        }
    }
}

/*----------------------------------------*/
static void initPipes_MF (struct pipe_connection_info * pipes)
{
    pipes->data.readHandle = -1;
    pipes->data.writeHandle = -1;
    pipes->async.readHandle = -1;
    pipes->async.writeHandle = -1;
}

/*----------------------------------------*/

static void closePipes_MF (struct pipe_connection_info * pipes)
{
#define CLOSE_PIPE_M(handle) if (handle != -1) {\
        close (handle);         \
        handle = -1; }

    CLOSE_PIPE_M (pipes->data.readHandle);
    CLOSE_PIPE_M (pipes->data.writeHandle);
    CLOSE_PIPE_M (pipes->async.readHandle);
    CLOSE_PIPE_M (pipes->async.writeHandle);

#undef CLOSE_PIPE_M
}

/*----------------------------------------*/

static bool createPipe_MF (pipe_handle_t * readHandle,
                           pipe_handle_t * writeHandle,
                           const char    * comment,
                           tsp00_ErrTextc pErrText)
{
    int         pipeHandles [2];
    int         rc;
    rc = pipe (pipeHandles);
    if (rc != 0) {
        en42FillErrText(pErrText, "Cannot create pipe:(%d:%s)", errno, sqlerrs() );
        return false;
    }
    *readHandle = pipeHandles [0];
    *writeHandle = pipeHandles [1];
    ERROR_TRACE (("%s: read(%d) write (%d)\n", comment, *readHandle, *writeHandle));
    return true;
}

/*----------------------------------------*/

static int createPipes_MF (struct pipe_connection_info * parentPipes,
                           struct pipe_connection_info * childPipes,
                           tsp00_ErrTextc pErrText)
{
    bool ok = true;

    initPipes_MF (parentPipes);
    initPipes_MF (childPipes);

    ok = ok && createPipe_MF (&childPipes->data.readHandle,
                              &parentPipes->data.writeHandle,
                              "child data, parent data", pErrText);
    ok = ok && createPipe_MF (&parentPipes->data.readHandle,
                              &childPipes->data.writeHandle,
                              "parent data, child data", pErrText);
    ok = ok && createPipe_MF (&childPipes->async.readHandle,
                              &parentPipes->async.writeHandle,
                              "child async, parent async", pErrText);
    ok = ok && createPipe_MF (&parentPipes->async.readHandle,
                              &childPipes->async.writeHandle,
                              "parent async, child async", pErrText);
    if (!ok) {
        closePipes_MF (parentPipes);
        closePipes_MF (childPipes);
        return va_notok;
    }
    return 0;
}

/*----------------------------------------*/

static char * copyPIDConnectParameter_MF (char        * writePosition,
                                          int           valueTag,
                                          RTE_OSPid     pid)
{
    int copyLen;
    SAPDB_UInt4     idx;
    SAPDB_Int8      longPID = (SAPDB_Int8)pid;
    SAPDB_Char *    valuePtr = (SAPDB_Char *)(&longPID);

    if (valuePtr == NULL) {
        return writePosition;
    }
    copyLen = sizeof (longPID); 

    writePosition [0] = (char) valueTag;
    writePosition [1] = 0;
    writePosition [2] = copyLen;
    writePosition += 3;

    for (idx = 0; idx < copyLen; ++idx)
    {
        writePosition[idx] = valuePtr[idx];
    }

    return writePosition + copyLen;
}

/*----------------------------------------*/

static char * copyConnectParameter_MF (char        * writePosition,
                                       int           valueTag,
                                       const char  * value)
{
    int copyLen;

    if (value == NULL) {
        return writePosition;
    }
    copyLen = strlen (value) + 1;  /* include 0-byte */
    if (copyLen == 1) {
        return writePosition;
    }
    writePosition [0] = (char) valueTag;
    writePosition [1] = copyLen / 256;
    writePosition [2] = copyLen % 256;
    writePosition += 3;
    SAPDB_memcpy (writePosition, value, copyLen);
    return writePosition + copyLen;
}

/*----------------------------------------*/

static int basicAllocatePacket_MF (connection_info * cip,
                                   int               packetSize,
                                   tsp00_ErrTextc pErrText)
{
    void * packet;
    if ( ALLOC_MEM_EO57(&packet, packetSize) != NO_ERROR_EO001 )
    {
        en42FillErrText(pErrText, "Out of memory (%d bytes):(%d:%s)", packetSize, errno, sqlerrs() );
        return va_notok;
    }
    cip->ci_packet_size = packetSize;
    cip->ci_packet [0] = (rte_header*) packet;
    cip->ci_packetData [0] = (char *) packet + sizeof (rte_header);
    initHeader_MF (cip->ci_packet [0]);
    return va_ok;
}

/*----------------------------------------*/

static void fillRequestHeader_MF (
    RTEReqRespDataT * header,
    int               varpartLen)
{
    header->osType = OSTYPE_LOCAL_OS_EN905;
    header->requestVersion = REQUEST_VERSION_EN905;
    header->varpartLen = varpartLen;
}

/*----------------------------------------*/

static int addRequestParameter (
    unsigned char * varpart,
    int             currentLength,
    int             typeCode,
    const void    * data,
    int             dataLen)
{
    varpart += currentLength;
    varpart [0] = (unsigned char) typeCode;
    varpart [1] = (unsigned char)(dataLen >> 8);
    varpart [2] = (unsigned char) dataLen & 0xFF;
    SAPDB_memcpy (varpart + 3, data, dataLen + 3);
    return dataLen + 3;
}

/*----------------------------------------*/

static int findReplyParameter (
    unsigned char * varpart,
    int             varpartLen,
    int             typeCode,
    void          * data,
    int             dataSize)
{
    unsigned char * varpointer = varpart;
    unsigned char * varpartEnd = varpart + varpartLen;
    int parameterLen;

    while ((varpointer < varpartEnd) && (varpointer [0] != typeCode)) {
        parameterLen = varpointer [1] * 256 + varpointer [2];
        ERROR_TRACE (("%c at %d for %d (0x%x)\n", varpointer [0],
                      varpointer - varpart, parameterLen, parameterLen));
        varpointer += 3 + parameterLen;
    }
    if (varpointer >= varpartEnd) {
        ERROR_TRACE (("nothing found for %c\n", typeCode));
        return 0;
    }
    parameterLen = varpointer [1] * 256 + varpointer [2];
    ERROR_TRACE (("found %d bytes at %d\n", parameterLen, varpointer + 3 - varpart));
    SAPDB_memcpy (data, varpointer + 3, parameterLen);
    return parameterLen;
}

/*----------------------------------------*/

static void findVarpart_MF (
    void  * replyData,
    unsigned char ** varpart,
    int   * varpartLen)
{
    RTEReqRespDataT * header = replyData;
    *varpart = ((unsigned char*) replyData) + sizeof (RTEReqRespDataT);
    *varpartLen = header->varpartLen;
}

/*----------------------------------------*/

static int requestReceive_MF (connection_info * cip,
                              tsp00_ErrTextc pErrText)
{
    int commRC;

    commRC = requestToLocalManager_MF (cip, pErrText);
    if (commRC == va_ok) {
        commRC = receiveFromLocalManager_MF (cip, pErrText);
    }
    return commRC;
}

/*----------------------------------------*/

/*
 * does an info request and allocates one packet
 */
static int allocatePacket_MF (connection_info * cip,
                              tsp00_ErrTextc pErrText)
{
    int               allocRC;
    int               commRC;
    tsp00_Uint4       packetSize = -1;
    tsp00_Uint4       minReplySize = -1;
    RTEReqRespDataT * requestHeader;
    unsigned char   * requestData;
    int               requestLen = 0;

    /*
     * allocate negotiation package
     * minimal packet 8196 (from eo45_MinPacketBufferSize)
     */
    allocRC = basicAllocatePacket_MF (cip, 8196, pErrText);
    if (allocRC != va_ok) {
        return allocRC;
    }
    /*
     * query packet size
     */
    cip->ci_request = cip->ci_packet [0];
    requestHeader = (RTEReqRespDataT *) cip->ci_packetData [0];
    requestData = ((unsigned char *) cip->ci_packetData [0])
        + sizeof (RTEReqRespDataT);
    requestLen = addRequestParameter (requestData, requestLen,
        VARPART_VAL_TRANSFER_BUFFER_SIZE_EO44,
        &packetSize, sizeof (packetSize));
    fillRequestHeader_MF (requestHeader, requestLen);
    cip->ci_request->rh_mess_class = REQ_RESP_TYPE_BUFFER_SIZE_REQUEST;
    cip->ci_request_lgt = requestLen + sizeof (RTEReqRespDataT);
    commRC = requestReceive_MF (cip, pErrText);
    if (commRC == va_ok) {
        if (cip->ci_reply->rh_mess_class == REQ_RESP_TYPE_BUFFER_SIZE_RESPONSE ) {
            /*
             * read packet size and min reply size
             */
            unsigned char * varpart;
            int varpartLen;
            findVarpart_MF (cip->ci_replyData, &varpart, &varpartLen);
            findReplyParameter (varpart, varpartLen,
                VARPART_VAL_TRANSFER_BUFFER_SIZE_EO44,
                &packetSize, sizeof (packetSize));
            findReplyParameter (varpart, varpartLen,
                VARPART_VAL_MIN_RESPONSE_PACKET_SIZE_EO44,
                &minReplySize, sizeof (minReplySize));
            cip->ci_packet_size =  packetSize;
            cip->ci_min_reply_size = minReplySize;
            cip->ci_max_data_size = packetSize - 2 * HEADER_OVERHEAD_C;
            cip->ci_max_request_size = packetSize - minReplySize - 2 * HEADER_OVERHEAD_C;
            ERROR_TRACE (("packet size: %d; min reply size: %d\n",
                          packetSize, minReplySize));
        }
        else {
            commRC = va_notok;
            en42FillErrText(pErrText,"protocol error: unexpected reply type");
            ERROR_TRACE (("reply type %d (%x)\n",
                          cip->ci_reply->rh_mess_class,
                          cip->ci_reply->rh_mess_class));
        }
    }
    /*
     * free negotiation package
     */
    FREE_MEM_EO57(cip->ci_packet [0]);
    cip->ci_packet [0] = NULL;
    cip->ci_packetData [0] = NULL;
    /*
     * check info request succeded
     */
    if (commRC != va_ok) {
        ERROR_TRACE (("negotiation failed: %d %s\n", commRC, pErrText));
        return commRC;
    }
    /*
     * alloc packet
     */
    allocRC = basicAllocatePacket_MF (cip, packetSize, pErrText);
    return allocRC;
}

/*----------------------------------------*/
#if !defined (_WIN32)
static void autoLogon_MF (connection_info * cip)
{
    RTEReqRespDataT * requestHeader;
    unsigned char   * requestData;
    int               requestLen = 0;
    tsp00_Int4        userid = getuid ();
    tsp00_ErrTextc    errtext;
    /*
     * setup request
     */
    cip->ci_request = cip->ci_packet [0];
    cip->ci_requestData = cip->ci_packetData [0];
    /*
     * set parameters
     */
    requestHeader = (RTEReqRespDataT *) cip->ci_packetData [0];
    requestData = ((unsigned char *) cip->ci_packetData [0])
        + sizeof (RTEReqRespDataT);
    fillRequestHeader_MF (requestHeader, requestLen);
    /*
     * send
     */
    cip->ci_request->rh_mess_class = REQ_RESP_TYPE_DBMSERVER_AUTOLOGON_REQUEST;
    cip->ci_request_lgt = sizeof (RTEReqRespDataT) + requestLen;
    requestReceive_MF (cip, errtext);
}
#endif
/*----------------------------------------*/

static int exchangeConnect_MF (connection_info * cip,
                               tsp00_Pathc       dbrootBuf,
                               tsp00_ErrTextc    pErrText)
{
    RTEReqRespDataT * requestHeader;
    char  * startPosition;
    char  * writePosition;
    int     requestLen;
    int     rc;

    /*
     * setup request
     */
    cip->ci_request = cip->ci_packet [0];
    cip->ci_requestData = cip->ci_packetData [0];
    /*
     * set parameters
     */
    requestHeader = (RTEReqRespDataT *) cip->ci_packetData [0];
    writePosition = startPosition = ((char *) cip->ci_packetData [0])
        + sizeof (RTEReqRespDataT);
    writePosition = copyConnectParameter_MF (writePosition,
                    VARPART_VAL_DBROOT_EO44, dbrootBuf);
    writePosition = copyConnectParameter_MF (writePosition,
                    VARPART_VAL_SERVERDB_EO44, cip->ci_peer_dbname);
    writePosition = copyConnectParameter_MF (writePosition,
                    VARPART_VAL_SENDERNODEID_EO44, cip->ci_own_node);
    writePosition = copyPIDConnectParameter_MF (writePosition,
                    VARPART_VAL_SENDERPID_EO44, cip->ci_my_pid);

    requestLen = writePosition - startPosition;
    fillRequestHeader_MF (requestHeader, requestLen);
    /*
     * send
     */
    cip->ci_request->rh_mess_class = REQ_RESP_TYPE_SET_DBMSERVER_PARAMS_REQUEST;
    cip->ci_request_lgt = sizeof (RTEReqRespDataT) + requestLen;
    rc = requestReceive_MF (cip, pErrText);
    return rc;
}

/*----------------------------------------*/

static int startLocalManager_MF (connection_info    * cip,
                                 tsp00_ErrTextc    pErrText)
{
    tsp00_Pathc  executable;
    tsp00_Pathc  dbrootBuf;
    void       * packet;
    struct pipe_connection_info childPipes;
    int          rc;

    ERROR_TRACE (("called startLocalManager_MF\n"));
    /*
     * find program
     */
    if (cip->ci_dbroot == NULL) {       /* PTS 1001140 */
        dbrootBuf [0] = '\0';
    }
    else {
        strcpy (dbrootBuf, cip->ci_dbroot);
    }

    if (!sql904_findControlServer (executable, sizeof (executable),
                               cip->ci_peer_dbname, cip->ci_serverpgm,
                               dbrootBuf, sizeof (dbrootBuf), pErrText)) {
        return va_notok;
    }

    /*
     * create pipes
     */
    rc = createPipes_MF (&cip->ci_pipe_info, &childPipes, pErrText);
    if (rc != va_ok) {
        if (rc != va_ok) {
            ERROR_TRACE (("error creating pipes: %d %s\n", rc, pErrText));
        }
        return rc;
    }

    /*
     * exec program
     */
    cip->ci_peer_pid = fork ();
    if (cip->ci_peer_pid == 0) {
        char argbuf [sizeof (pipe_handle_t) * 2 * 4 + 1];
        const int hexSizeC = sizeof (pipe_handle_t) * 2;
        sp77sprintf (argbuf, sizeof (argbuf), "%0*x%0*x%0*x%0*x",
                 hexSizeC, childPipes.data.readHandle,
                 hexSizeC, childPipes.data.writeHandle,
                 hexSizeC, childPipes.async.readHandle,
                 hexSizeC, childPipes.async.writeHandle);
        closePipes_MF (&cip->ci_pipe_info); /* close parent pipes */
        rc = execl (executable, executable, "-P", argbuf, 0);
        exit (1);
    }
    closePipes_MF (&childPipes);
    if (cip->ci_peer_pid == -1) {
        en42FillErrText(pErrText, "Cannot create new process:(%d:%s)", errno, sqlerrs());
        closePipes_MF (&cip->ci_pipe_info);
        terminateChildProcess_MF (cip);
        return va_notok;
    }
    /*
     * detect packet size
     */
    rc = allocatePacket_MF (cip, pErrText);
    if (rc != va_ok) {
        closePipes_MF (&cip->ci_pipe_info);
        terminateChildProcess_MF (cip);
        return rc;
    }
#if !defined (_WIN32)
    /*
     * autologon must be before parameter request
     * because the protocol doesn't allow
     * for optional requests otherwise
     */
    autoLogon_MF (cip);
#endif

    /*
     * connect exchange
     */
    rc = exchangeConnect_MF (cip, dbrootBuf, pErrText);
    if (rc != va_ok) {
        closePipes_MF (&cip->ci_pipe_info);
        terminateChildProcess_MF (cip);
        freePackets_MF (cip, 1);
        return rc;
    }
    /*
     *
     */
    ERROR_TRACE (("connection established\n"));
    return va_ok;
}

/*----------------------------------------*/

static void terminateChildProcess_MF (connection_info     *cip)
{
    int status;
    int rc;
    
    /* kill server? */
    while ((rc = waitpid (-1, &status, WNOHANG)) > 0) {
    }
    cip->ci_peer_pid = 0;
}

/*----------------------------------------*/
    #undef MF__
    #define MF__ MOD__"clearLocalManagerConnection_MF"
static int clearLocalManagerConnection_MF (connection_info     *cip,
        tsp00_ErrTextc    pErrText)
{
    closePipes_MF (&cip->ci_pipe_info);
    freePackets_MF (cip, 1);
    terminateChildProcess_MF (cip);
    return va_ok;
}

/*----------------------------------------*/

    #undef MF__
    #define MF__ MOD__"releaseLocalManager_MF"
static int releaseLocalManager_MF (connection_info     *cip,
                                   tsp00_ErrTextc    pErrText)
{
    /* send release packet */
    return clearLocalManagerConnection_MF (cip, pErrText);
}

/*----------------------------------------*/

    #undef MF__
    #define MF__ MOD__"requestToLocalManager_MF"
static int writeToPipe_MF (pipe_handle_t pipeHandle,
                        const void *  data,
                        int           dataLen,
                        tsp00_ErrTextc    pErrText)
{
    int writeResult;
    int result = va_ok;

    ERROR_TRACE (("write %d bytes to %d\n", dataLen, pipeHandle));
    dumpHex (data, dataLen);
    do {
        writeResult = write (pipeHandle, data, dataLen);
    } while ((writeResult < 0) && (errno == EINTR));
    if (writeResult < 0) {
        result = va_notok;
        en42FillErrText(pErrText, "Error writing to pipe:(%d:%s)", errno, sqlerrs());
    }
    return result;
}

/*----------------------------------------*/

    #undef MF__
    #define MF__ MOD__"requestToLocalManager_MF"
static int requestToLocalManager_MF (connection_info     *cip,
                                     tsp00_ErrTextc    pErrText)
{
    int               transferLength;
    int               result = va_ok;
    int               writeResult;
    int               alignedLength;
    /*
     * fill req resp info
     */
    fillHeader_MF (cip->ci_request, cip->ci_request_lgt);
    transferLength = HEADER_OVERHEAD_C + cip->ci_request_lgt;
    if (transferLength % 8 == 0) {
        alignedLength = transferLength;
    }
    else {
        alignedLength = transferLength + (8 - (transferLength % 8));
    }
    ERROR_TRACE (("transfer length: %d, aligned length: %d\n",
                  transferLength, alignedLength));
    cip->ci_reply = (rte_header *) ((char *) cip->ci_request + alignedLength);
    cip->ci_replyData = (char *) cip->ci_reply + HEADER_OVERHEAD_C;
    /*
     * write data
     */
    result = writeToPipe_MF (cip->ci_pipe_info.data.writeHandle,
                             cip->ci_request,
                             transferLength,
                             pErrText);
    return result;
}

/*----------------------------------------*/

    #undef MF__
    #define MF__ MOD__"replyAvailableFromLocalManager_MF"
static int replyAvailableFromLocalManager_MF (
    connection_info     *cip,
    tsp00_ErrTextc    pErrText)
{
    /*
     * not really implemented, but answer OK to
     * prevent endless loops
     * implementation: use select
     */
    return va_ok;
}

/*----------------------------------------*/

static int readFromPipe_MF (
    pipe_handle_t   pipeHandle,
    void          * target,
    int             bytesRequested)
{
    int result;
    int readResult;
    char * targetPos = target;
    int bytesRead = 0;
    bool fatalError = false;

    ERROR_TRACE (("reading %d bytes from %d\n", bytesRequested, pipeHandle));
    while ((bytesRead < bytesRequested) && ! fatalError) {
        do
        {
            readResult = read (pipeHandle, targetPos, 
                    bytesRequested - bytesRead);
            ERROR_TRACE (("    read %d bytes\n", readResult));
        }
        while ( (readResult <= 0) && (EINTR == errno) );
        if (readResult <= 0) {
            fatalError = true;
        }
        else {
            bytesRead += readResult;
            targetPos += readResult;
        }
    }

    if (fatalError) {
        result = va_notok;
    }
    else {
        result = va_ok;
    }

    if (result == va_ok) {
        dumpHex (target, bytesRead);
    }

    return result;
}

/*----------------------------------------*/

    #undef MF__
    #define MF__ MOD__"receiveFromLocalManager_MF"
static int receiveFromLocalManager_MF (connection_info     *cip,
                                       tsp00_ErrTextc    pErrText)
{
    int rc;
    int i;
    pipe_handle_t   readPipe = cip->ci_pipe_info.data.readHandle;
    rte_header * header = (rte_header *) cip->ci_reply;
    rc = readFromPipe_MF (readPipe, header, HEADER_OVERHEAD_C);
    if (rc != va_ok) {
        en42FillErrText(pErrText, "Reading header from pipe:(%d:%s)", errno, sqlerrs() );
        return rc;
    }
    rc = readFromPipe_MF (readPipe, cip->ci_replyData,
        header->rh_act_send_len - HEADER_OVERHEAD_C);
    if (rc != va_ok) {
        en42FillErrText(pErrText, "Reading data from pipe:(%d:%s)", errno, sqlerrs() );
        return rc;
    }
    cip->ci_reply_size = header->rh_act_send_len - HEADER_OVERHEAD_C;
    if (cip->ci_reply->rh_mess_class == REQ_RESP_TYPE_EVENTLIST) {
        en42FillErrText(pErrText, "protocol error:got EVENTLIST");
        rc = va_notok;
    }
    return rc;
}

/*----------------------------------------*/

    #undef MF__
    #define MF__ MOD__"cancelLocalManager_MF"
static int cancelLocalManager_MF (connection_info     *cip,
                                  tsp00_ErrTextc    pErrText)
{
    struct  {
        rte_header       rteHeader;
        RTEReqRespDataT  reqResp;
    } fullHeader;
    int result;
    int rc;
    int varpartLen;

    /*
     * build and send request
     */
    initHeader_MF (&fullHeader.rteHeader);
    fillRequestHeader_MF (&fullHeader.reqResp, 0);
    fullHeader.rteHeader.rh_mess_class = REQ_RESP_TYPE_CANCEL_REQUEST;
    fillHeader_MF (&fullHeader.rteHeader, sizeof (RTEReqRespDataT));
    result = writeToPipe_MF (cip->ci_pipe_info.async.writeHandle,
        &fullHeader, sizeof (fullHeader), pErrText);
    if (result != va_ok) {
        return result;
    }
    /*
     * read reply
     */
    rc = readFromPipe_MF (cip->ci_pipe_info.async.readHandle,
        &fullHeader.rteHeader, sizeof (rte_header));
    if (rc != 0) {
        en42FillErrText(pErrText, "Cancel: Read header from pipe:(%d:%s)", errno, sqlerrs() );
        return va_notok;
    }
    varpartLen = fullHeader.rteHeader.rh_act_send_len - HEADER_OVERHEAD_C;
    if (varpartLen > 0) {
        char buf [1024];
        while (varpartLen > 0) {
            rc = readFromPipe_MF (cip->ci_pipe_info.async.readHandle,
                buf, sizeof (buf));
            if (rc != 0) {
                en42FillErrText(pErrText, "Cancel: Read data from pipe:(%d:%s)", errno, sqlerrs() );
                return va_notok;
            }
            varpartLen -= sizeof (buf);
        }
    }
    /*
     * return result
     */
    return va_ok;
}

/*----------------------------------------*/

    #undef MF__
    #define MF__ MOD__"dumpLocalManager_MF"
static int dumpLocalManager_MF (connection_info     *cip,
                                tsp00_ErrTextc    pErrText)
{
    en42FillErrText(pErrText, "Dump not supported" );
    return va_notok;
}

/*----------------------------------------*/

static connection_VMT pipe_class_VMT =
    {
        "LocalManagerConnection",
        startLocalManager_MF,
        clearLocalManagerConnection_MF,
        releaseLocalManager_MF,
        requestToLocalManager_MF,
        replyAvailableFromLocalManager_MF,
        receiveFromLocalManager_MF,
        cancelLocalManager_MF,
        dumpLocalManager_MF,
    };

/*----------------------------------------*/

void    sql03_xconnect (char            * servernode,
                        tsp00_DbNamec     serverdb,
                        char            * dbroot,
                        char            * serverpgm,
                        tsp00_Int4        * reference,
                        tsp00_Int4        * sql_packet_size,
                        void           ** sql_packet_list,
                        connection_VMT  * vmt,
                        tsp00_ErrTextc    perrtext,
                        tsp01_CommErr   * returncode);

/*ARGSUSED*/ /* pid not used */
    #undef MF__
    #define MF__ MOD__"sqlxconnectc"
void    sqlxconnectc (tsp00_TaskId      pid,
                      char            * servernode,
                      tsp00_DbNamec     serverdb,
                      char            * dbroot,
                      tsp00_Int4        * reference,
                      tsp00_Int4        * sql_packet_size,
                      void           ** sql_packet,
                      char            * pErrText,
                      tsp01_CommErr   * returncode)
{
    sql03_xconnect ( servernode,
                     serverdb,
                     dbroot,
                     "dbmsrv",
                     reference,
                     sql_packet_size,
                     sql_packet,
                     &pipe_class_VMT,
                     pErrText,
                     returncode);
}


/*----------------------------------------*/

/*ARGSUSED*/ /* pid not used */
    #undef MF__
    #define MF__ MOD__"sqlxconnectp"
void    sqlxconnectp (tsp00_TaskId      pid,
                      tsp00_NodeId      servernode,
                      tsp00_DbName      serverdb,
                      char            * dbroot,
                      tsp00_Int4      * reference,
                      tsp00_Int4      * sql_packet_size,
                      void           ** sql_packet,
                      tsp00_ErrText     errtext,
                      tsp01_CommErr   * returncode)
{
    tsp00_NodeIdc     szServerNode;
    tsp00_DbNamec     szServerDB;
    tsp00_ErrTextc    pErrText;

    eo46PtoC ( szServerNode, servernode,  sizeof(tsp00_NodeId) );
    eo46PtoC ( szServerDB,   serverdb,    sizeof(tsp00_DbName) );

    sql03_xconnect ( szServerNode,
                     szServerDB,
                     dbroot,
                     "dbmsrv",
                     reference ,
                     sql_packet_size ,
                     sql_packet,
                     &pipe_class_VMT,
                     pErrText,
                     returncode);

    if ( *returncode != commErrOk_esp01 )
        eo46CtoP ( errtext, pErrText,  sizeof(tsp00_ErrText) );

}


/*----------------------------------------*/

/*ARGSUSED*/ /* pid not used */
    #undef MF__
    #define MF__ MOD__"sqlx2connectc"
void    sqlx2connectc (tsp00_TaskId      pid,
                       char            * servernode,
                       char            * serverdb,
                       char            * dbroot,
                       char            * serverpgm,
                       tsp00_Int4        * reference,
                       tsp00_Int4        * sql_packet_size,
                       void           ** sql_packet,
                       tsp00_ErrTextc    pErrText,
                       tsp01_CommErr   * returncode)
{
    sql03_xconnect ( servernode,
                     serverdb ,
                     dbroot,
                     serverpgm,
                     reference ,
                     sql_packet_size ,
                     sql_packet,
                     &pipe_class_VMT,
                     pErrText,
                     returncode);
}

/*----------------------------------------*/

/*ARGSUSED*/ /* pid not used */
    #undef MF__
    #define MF__ MOD__"sqlx2connectp"
void    sqlx2connectp (tsp00_TaskId      pid,
                       tsp00_NodeId      servernode,
                       tsp00_DbName      serverdb,
                       char            * dbroot,
                       char            * serverpgm,
                       tsp00_Int4      * reference,
                       tsp00_Int4      * sql_packet_size,
                       void           ** sql_packet,
                       tsp00_ErrText     errtext,
                       tsp01_CommErr   * returncode)
{
    /* serverpgm: pascal -> C, not imple,mented  */
    tsp00_NodeIdc     szServerNode;
    tsp00_DbNamec     szServerDB;
    tsp00_ErrTextc    pErrText;

    eo46PtoC ( szServerNode, servernode,  sizeof(tsp00_NodeId) );
    eo46PtoC ( szServerDB,   serverdb,    sizeof(tsp00_DbName) );

    sql03_xconnect ( szServerNode,
                     szServerDB ,
                     dbroot,
                     serverpgm,
                     reference ,
                     sql_packet_size ,
                     sql_packet,
                     &pipe_class_VMT,
                     pErrText,
                     returncode);

    if ( *returncode != commErrOk_esp01 )
        eo46CtoP ( errtext, pErrText,  sizeof(tsp00_ErrText) );

}

/*----------------------------------------*/
/* PTS 1111265 */
extern char *sql01_username;

    #undef MF__
    #define MF__ MOD__"sqlxusername"
const char *
sqlxusername ()
{
    return sql01_username;
}
