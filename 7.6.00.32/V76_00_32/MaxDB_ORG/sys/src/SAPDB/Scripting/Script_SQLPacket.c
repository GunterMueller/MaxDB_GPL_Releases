/*
   @filename    vin28.c
   @brief       Interface to SQL packet

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


 */

#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <stdio.h>

#include "heo102.h"
#include "heo03.h"
#include "hsp02.h"
#include "hsp26.h"
#include "hsp30.h"
#include "hsp40.h"
#include "hsp41.h"
#include "hsp45.h"
#include "hsp77.h"
#include "hsp78_0.h"
#include "hsp100.h"
#include "RunTime/Security/RTESec_Authentication.h"
#include "Scripting/Script_SQLPacket.h"

/*----------------------------------------*/

typedef char byte_t;

#define missingKeywordIdentifiedC -5015

/*#if COMPILEMODE_MSP00 >= QUICK_MSP00*/
/* quick fix for PTS 1104718*/
/*#define ASSERT_DBG(cond)        ((cond) ? (void) 0 : \*/
/*            sqlk_assert (assertionFailed_esp00, __LINE__, __FILE__, #cond))*/
/*#else*/

#define ASSERT_DBG(cond)        0

/*#endif*/

/*static const char i28_applicationName [4] = "CON";*/
/*static const char i28_applicationVersion [6] = "70200";*/

#define SQLTRACE (session->tracer)

externPascal void s43pstr (
    tsp00_Number    * num,
    tsp00_Int4        pos,
    int               len,
    int               frac,
    char            * source,
    tsp00_Int4        spos,
    int               slen,
    tsp00_NumError  * res);

externPascal void s44egchr (
    void            * buf,
    tsp00_Int4        pos,
    integer           len,
    integer           frac,
    void            * dest,
    tsp00_Int4        dpos,
    integer         * dlen,
    tsp00_DecimalPresentation *decimal,
    tsp00_NumError  * res);

/* ------------------------------- */
/* specification private functions */
/* ------------------------------- */
static void     i28_fetchcmd
    ( Script_SessionT   * session,
      const char *          fetchCmd,
      int                fetchVars );
/* ------------------------------- */
static void     i28_reset
    ( Script_SessionT   * session,
      tsp00_Bool          initClientInfo );
/* ------------------------------- */
static void     i28_machinfo
    ( tsp00_CodeType    * code_type,
      tsp00_SwapKind    * swap_kind,
      char             * termid);
/* ------------------------------- */
void            i28pnull
    ( Script_SessionT   * session,
      int                sqllen,
      tsp00_Int4           bufpos);
/* ------------------------------- */
tsp01_CommErr i28receive
    ( Script_SessionT        * pSession );
/* ------------------------------- */
tsp01_CommErr i28request
    ( Script_SessionT   * session);
/* ------------------------------- */
void            i28callsql
    ( Script_SessionT   * session,
      tsp01_CommErr  * comm_error);
/* ------------------------------- */
void i28pascalstring
      ( char               * target,
        int                  targetlen,
        const char         * source,
        int                  sourcelen );
/* ------------------------------- */
void i28cleanup_session
      ( Script_SessionT     * session);
/* ------------------------------- */


#define UNICODE_CLIENT(session) (M_IS_UNICODE((session)->code_type))
/*
 *
 *#define UNICODE_CLIENT(session) (((session)->code_type == CSP_UNICODE)||((session)->code_type == CSP_UNICODE_SWAP))
 */
/**\
--------------------------------------MF__ MOD__ chapter packet handling
\**/

#define PACKETFIELD(packet,field)      (packet->variant.C_2.field##_F)
#define PACKETINFO(packet,field)       (packet->sp1_header.field)
#define SEGMFIELD(segment,field)       (segment->variant.C_1.field##_F)
#define SEGMINFO(segment,field)        (segment->variant.C_3.field##_F)
#define SENDINFO(segment,field)        (segment->variant.C_3.field##_F)
#define RECINFO(segment,field)         (segment->variant.C_4.field##_F)
#define PARTINFO(part, field)          (part->variant.C_2.field##_F)
#define PARTDATA(part)                 ((char *)&part->variant.C_1.sp1p_buf_F)
#define EMPTYSET                        0
#define M_WARNINGSET(set)               (*(tsp00_Int2*)&set)
#define STRUCT_COPY(target, source)       memcpy (target, source, sizeof (target))

tsp1_segment   *
i28firstsegment (
    tsp1_packet * packet)
{
    /* ROUTINE_DBG_MSP00 ("i28firstsegment"); */
    return &PACKETFIELD (packet, sp1_segm);
}

/*----------------------------------------*/

tsp1_segment   *
i28nextsegment (
    tsp1_segment * segment)
{
    /* ROUTINE_DBG_MSP00 ("i28nextsegment"); */
    s26next_segment (&segment);
    return segment;
}

/*----------------------------------------*/

tsp1_segment   *
i28_lastsegment (
    tsp1_packet * packet)
{
    /* ROUTINE_DBG_MSP00 ("i28_lastsegment"); */
    int             segmentCount;
    int             i;
    tsp1_segment   *segment;

    segmentCount = packet->sp1_header.sp1h_no_of_segm;
    segment = i28firstsegment (packet);
    for (i = 0; i < segmentCount - 1; ++i) {
        segment = i28nextsegment (segment);
    }
    return segment;
}

/*----------------------------------------*/

tsp1_segment   *
i28advsegment (
    Script_SessionT   * session)
{
    /* ROUTINE_DBG_MSP00 ("i28advsegment"); */
    session->segment = i28nextsegment (session->segment);
    return session->segment;
}

/*----------------------------------------*/

tsp1_segment   *
i28newsegment (
    Script_SessionT   * session,
    tsp1_cmd_mess_type_Enum messType)
{
    /* ROUTINE_DBG_MSP00 ("i28newsegment"); */
    s26first_segment_init (session->send_packet, sp1sk_cmd,
        &session->segment);
    SENDINFO (session->segment, sp1c_mess_type) = messType;
    SENDINFO (session->segment, sp1c_sqlmode) = session->sqlMode;
    return session->segment;
}

/*----------------------------------------*/

tsp1_part      *
i28firstpart (
    tsp1_segment * segment)
{
    /* ROUTINE_DBG_MSP00 ("i28firstpart"); */
    return &SEGMFIELD (segment, sp1p_part);
}

/*----------------------------------------*/

tsp1_part      *
i28nextpart (
    tsp1_part * part)
{
    /* ROUTINE_DBG_MSP00 ("i28nextpart"); */
    s26nextpart (&part);
    return part;
}

/*----------------------------------------*/

tsp1_part      *
i28advpart (
    Script_SessionT   * session)
{
    /* ROUTINE_DBG_MSP00 ("i28advpart"); */
    session->part = i28nextpart (session->part);
    return session->part;
}

/*----------------------------------------*/

tsp00_Bool
i28findpart (
    Script_SessionT   * session,
    tsp1_part_kind_Enum requiredKind)
{
    /* ROUTINE_DBG_MSP00 ("i28findpart"); */
    /* Find Part return it in session->part */
    if (session->part != NULL) {
        if (PARTINFO (session->part, sp1p_part_kind) == requiredKind) {
            return true;
        }
    }
    s26find_part (session->segment, requiredKind, &session->part);
    return (session->part != NULL);
} /* i28findpart */

/*----------------------------------------*/

tsp00_Bool
i28_findpart (
    Script_SessionT   * session,
    tsp1_part_kind_Enum requiredKind,
    tsp1_part        ** found_part)
{
    /* ROUTINE_DBG_MSP00 ("i28_findpart"); */
    /* Find Part without changing session->part */
    if (session->part != NULL) {
        if (PARTINFO (session->part, sp1p_part_kind) == requiredKind) {
            *found_part = session->part;
            return true;
        }
    }
    s26find_part (session->segment, requiredKind, found_part);
    return (*found_part != NULL);
} /* i28_findpart */

/*----------------------------------------*/

tsp1_part      *
i28newpart (
    Script_SessionT   * session,
    tsp1_part_kind_Enum partKind)
{
    /* ROUTINE_DBG_MSP00 ("i28newpart"); */
    if (SENDINFO (session->segment, sp1s_no_of_parts) > 0)
        s26finish_part (session->send_packet, session->part);
    s26new_part_init (session->send_packet, session->segment, &session->part);
    ASSERT_DBG (session->part != NULL);
    PARTINFO (session->part, sp1p_part_kind) = partKind;
    return session->part;
}

/*----------------------------------------*/

tsp1_part      *
i28newDataPart (
    Script_SessionT   * session,
    int valueCount)
{
    /* ROUTINE_DBG_MSP00 ("i28newpart"); */
    if (SENDINFO (session->segment, sp1s_no_of_parts) > 0)
        s26finish_part (session->send_packet, session->part);
    s26new_part_init (session->send_packet, session->segment, &session->part);
    ASSERT_DBG (session->part != NULL);
    if (session->variable_input) {
        byte_t * buf = PARTDATA (session->part);
        PARTINFO (session->part, sp1p_part_kind) = sp1pk_vardata;
        buf [0] = (byte_t) (valueCount / 256);
        buf [1] = (byte_t) (valueCount % 256);
        PARTINFO (session->part, sp1p_buf_len) = 2;
    }
    else {
        PARTINFO (session->part, sp1p_part_kind) = sp1pk_data;
    }
    return session->part;
}

/*----------------------------------------*/

static tsp1_part *
i28newVardataPart (
    Script_SessionT   * session,
    int valueCount)
{
    tsp1_part * result = i28newpart (session, sp1pk_vardata);
    byte_t * buf = PARTDATA (session->part);
    PARTINFO (session->part, sp1p_part_kind) = sp1pk_vardata;
    buf [0] = (byte_t) (valueCount / 256);
    buf [1] = (byte_t) (valueCount % 256);
    PARTINFO (session->part, sp1p_buf_len) = 2;
    return result;
}

/*----------------------------------------*/

static void
i28setVardataFieldCount (
    Script_SessionT   * session,
    int valueCount)
{
    byte_t * buf = PARTDATA (session->part);
    buf [0] = (byte_t) (valueCount / 256);
    buf [1] = (byte_t) (valueCount % 256);
}

/*----------------------------------------*/

int
align8 (
    int inval)
{
    int remainder = inval % 8;
    if (remainder == 0) {
        return inval;
    }
    else {
        return inval + 8 - remainder;
    }
}

/*----------------------------------------*/

void
i28closesend (
    Script_SessionT   * session)
{
    /* ROUTINE_DBG_MSP00 ("i28closesend"); */
    int partOffset;

    ASSERT_DBG (session->buildingCommand);
    s26finish_part (session->send_packet, session->part);
    partOffset = (char *) session->part
        - (char *) & PACKETFIELD (session->send_packet, sp1_segm);
    PACKETINFO (session->send_packet, sp1h_varpart_len) =
        partOffset + sizeof (tsp1_part_header) +
        align8 (PARTINFO (session->part, sp1p_buf_len));
}

/*----------------------------------------*/

void
i28resetpackets (
    Script_SessionT   * session,
    tsp00_Bool forCommand)
{
    /* ROUTINE_DBG_MSP00 ("i28resetpackets"); */
    tsp1_packet    *packet;

    session->buildingCommand = forCommand;
    if (forCommand)
        packet = session->send_packet;
    else
        packet = session->rec_packet;
    session->segment = &PACKETFIELD (packet, sp1_segm);
    if (forCommand) {
        session->part = &SEGMFIELD (session->segment, sp1p_part);
    }
    else {
        session->part = NULL;
    }
}

/*----------------------------------------*/

void
i28clearpacketref (
    Script_SessionT   * session)
{
    /* ROUTINE_DBG_MSP00 ("i28clearpacketref"); */
    session->send_packet = NULL;
    session->rec_packet = NULL;
    session->segment = NULL;
    session->part = NULL;
}

/*----------------------------------------*/

void
i28initpacketref (
    Script_SessionT   * session,
    tsp1_packet * packet,
    tsp00_Bool forCommand)
{
    /* ROUTINE_DBG_MSP00 ("i28initpacketref"); */
    session->buildingCommand = forCommand;
    if (packet == NULL)
        i28clearpacketref (session);
    else {
        if (forCommand) {
            session->send_packet = packet;
            session->rec_packet = NULL;
        }
        else
            session->rec_packet = packet;
        session->segment = i28firstsegment (packet);
        session->part = i28firstpart (session->segment);
    }
}

/*----------------------------------------*/

void
i28initsession (
    Script_SessionT   * session,
    tsp1_packet * packet)
{
    /* ROUTINE_DBG_MSP00 ("i28initsession"); */

    memset (&session->connectInfo, '\0', sizeof (session->connectInfo));
    session->connectInfo.cachelimit = -1;
    session->connectInfo.timeout = -1;
    session->connectInfo.isolation = -1;
    session->reference = 0;
    session->is_connected = false;
    session->inUse = false;
    session->buildingCommand = false;
    i28_machinfo (&session->code_type, &session->swap_kind,
        (char *) session->senderid);
    i28initpacketref (session, packet, true);
    session->sqlMode = sp1sm_session_sqlmode;
    session->more_data = NULL;
    session->switch_possible = true;
    session->new_session = true;
    session->space_option = false;
}

/*----------------------------------------*/

void
i28unicodeclient (
    Script_SessionT   * session)
{
    /* ROUTINE_DBG_MSP00 ("i28unicodeclient"); */
    ASSERT_DBG (!session->is_connected);
    session->code_type = csp_unicode;
}

/*----------------------------------------*/

int
i28adabasmode (
    Script_SessionT   * session)
{
    /* ROUTINE_DBG_MSP00 ("i28adabasmode"); */
    int             result;

    result = session->sqlMode;
    session->sqlMode = sp1sm_internal;
    return result;
}

/*----------------------------------------*/

int
i28sessionmode (
    Script_SessionT   * session)
{
    /* ROUTINE_DBG_MSP00 ("i28sessionmode"); */
    int             result;

    result = session->sqlMode;
    session->sqlMode = sp1sm_session_sqlmode;
    return result;
}

/*----------------------------------------*/

int
i28oldmode (
    Script_SessionT   * session,
    int oldMode)
{
    /* ROUTINE_DBG_MSP00 ("i28oldmode"); */
    int             result;

    result = session->sqlMode;
    session->sqlMode = oldMode;
    return result;
}

/*----------------------------------------*/

void
i28initprocreply (
    Script_SessionT   * session)
{
    i28_reset (session, true);
    i28newsegment (session, sp1m_dbs);
    SEGMINFO (session->segment, sp1s_segm_kind) = sp1sk_procreply;
}

/*----------------------------------------*/

void
i28seterrorinfo  (
    Script_SessionT   * session,
    int                 errorCode,
    const char *        errorMessage,
    int                 messageLen)
{
    RECINFO (i28_lastsegment (session->rec_packet), sp1r_returncode) = errorCode;
    i28newpart (session, sp1pk_errortext);
    if (messageLen == -1) {
        messageLen = strlen (errorMessage);
    }
    i28addpartdata (session, errorMessage, messageLen);

}

/**\
--------------------------------------MF__ MOD__ chapter high level sql
\**/

void
i28initadbs (
    Script_SessionT   * session)
{
    /* ROUTINE_DBG_MSP00 ("i28initadbs"); */
    i28_reset (session, true);
    i28newsegment (session, sp1m_dbs);
    i28newpart (session, sp1pk_command);
}

/*----------------------------------------*/

void
i28initparse (
    Script_SessionT   * session)
{
    /* ROUTINE_DBG_MSP00 ("i28initparse"); */
    i28_reset (session, true);
    i28newsegment (session, sp1m_parse);
    i28newpart (session, sp1pk_command);
}

/*----------------------------------------*/

void
i28initexecute (
    Script_SessionT   * session,
    Script_SqlParseid pid)
{
    /* ROUTINE_DBG_MSP00 ("i28initexecute"); */
    i28_reset (session, true);
    i28newsegment (session, sp1m_execute);
    i28newpart (session, sp1pk_parsid);
    memcpy (PARTDATA (session->part) + PARTINFO (session->part, sp1p_buf_len),
        pid, sizeof (Script_SqlParseid));
    PARTINFO (session->part, sp1p_buf_len) += sizeof (Script_SqlParseid);
    /* i28newpart (session, sp1pk_data); */
}

/*----------------------------------------*/

void
i28initspecial (
    Script_SessionT   * session,
    tsp1_cmd_mess_type_Param messType)
{
    /* ROUTINE_DBG_MSP00 ("i28initspecial"); */
    i28_reset (session, true);
    i28newsegment (session, messType);
    i28newpart (session, sp1pk_command);
}

/*----------------------------------------*/

void
i28initoutargs (
    Script_SessionT   * session)
{
    /* ROUTINE_DBG_MSP00 ("i28initoutargs"); */
    i28_reset (session, false);
    s26first_segment_init (session->send_packet, sp1sk_return,
        &session->segment);
    i28newpart (session, sp1pk_data);
}

/*----------------------------------------*/
static int
i28sqlrequest (
    Script_SessionT   * session)
{
    /* ROUTINE_DBG_MSP00 ("i28sqlrequest"); */
  tsp00_Int2         result       = Script_db_ok;

  if (!session->is_connected) {
    result = Script_db_not_accessible;
    return result;
  } /* end if */

  ASSERT_DBG (session->is_connected);
  session->rec_packet = NULL;
  i28closesend (session);

  if (i28request (session) != commErrOk_esp01) {
    result = Script_db_not_accessible;
  } /* end if */

  return result;
} /* end i28sqlrequest */

/*----------------------------------------*/
static int
i28sqlreceive (
    Script_SessionT   * session)
{
    /* ROUTINE_DBG_MSP00 ("i28sqlreceive"); */
  tsp01_CommErr  comm_error;
  tsp00_Int2         result       = Script_db_ok;

  comm_error = i28receive (session);
  i28lasterr_on (session);

  if ((int) comm_error != commErrOk_esp01) {
    result = Script_db_not_accessible;
  } else {
    i28resetpackets (session, false);
    result = RECINFO (i28_lastsegment (session->rec_packet), sp1r_returncode);

/*    result = RECINFO (i28_lastsegment (session->rec_packet), returncode);*/
  } /* end if */

  return result;

} /* end i28sqlreceive */

/*----------------------------------------*/

int
i28sql (
    Script_SessionT   * session,
    Script_SqlResultT * sqlresult)
{
    /* ROUTINE_DBG_MSP00 ("i28sql"); */
    tsp01_CommErr  comm_error;
    tsp00_Int2        result;
    tsp1_packet    *send_packet = session->send_packet;
    tsp1_segment   *segment;
    int             msglen;

    if (sqlresult == NULL)
        sqlresult = &(session->lasterr.sqlresult);
    if (!session->is_connected) {
        result = Script_db_not_accessible;
        if (sqlresult != NULL) {
            sqlresult->returnCode = result;
            M_WARNINGSET (sqlresult->warnings) = EMPTYSET;
            sqlresult->errorPos = 0;
            sqlresult->rowCount = 0;
            STRUCT_COPY (sqlresult->sqlstate, "I8888");
            sqlresult->sqlmsg[0] = '\0';
        }
        return result;
    }
    ASSERT_DBG (session->is_connected);
    session->rec_packet = NULL;
    i28closesend (session);
    i28callsql (session, &comm_error);
    if ((int) comm_error != commErrOk_esp01) {
        result = Script_db_not_accessible;
        /* session->is_connected = false; lower level routines try reconnect */
        if (sqlresult != NULL) {
            sqlresult->returnCode = result;
            M_WARNINGSET (sqlresult->warnings) = EMPTYSET;
            sqlresult->errorPos = 0;
            sqlresult->rowCount = 0;
            STRUCT_COPY (sqlresult->sqlstate, "I8888");
            sqlresult->sqlmsg[0] = '\0';
        }
    }
    else {
        i28resetpackets (session, false);
        segment = i28_lastsegment (session->rec_packet);
        result = RECINFO (segment, sp1r_returncode);
        /* to lasterr, only store errors: */
        if ((sqlresult != NULL) &&
            ((result != Script_db_ok) ||
                (sqlresult != &(session->lasterr.sqlresult)))) {
            sqlresult->returnCode = result;
            M_WARNINGSET(sqlresult->warnings) = M_WARNINGSET
                (RECINFO (segment, sp1r_extern_warning));
            sqlresult->errorPos = RECINFO (segment, sp1r_errorpos);
            if (result == Script_db_row_not_found) {
                sqlresult->rowCount = 0;
            }
            else {
                sqlresult->rowCount = i28resultcount (session);
            }
            STRUCT_COPY (sqlresult->sqlstate, RECINFO (segment, sp1r_sqlstate));
            if (result != Script_db_ok) {
                if (i28findpart (session, sp1pk_errortext)) {
                    memcpy (sqlresult->sqlmsg, PARTDATA (session->part),
                        sizeof (sqlresult->sqlmsg) - 1);
                    msglen = PARTINFO (session->part, sp1p_buf_len);
                    sqlresult->sqlmsg[msglen] = '\0';
                }
            }
        }
    }
    return result;
}

/*----------------------------------------*/

static void
i28_reset (
    Script_SessionT   * session,
    tsp00_Bool initClientInfo)
{
    /* ROUTINE_DBG_MSP00 ("i28_reset"); */
    tsp1_packet_header *header;
    /*
     * init packet
     */
    if (!session->buildingCommand)
        i28resetpackets (session, true);
    header = &session->send_packet->sp1_header;
    if (initClientInfo) {
        header->sp1h_mess_code = session->code_type;
        header->sp1h_mess_swap = session->swap_kind;
        header->sp1h_filler2 = 0;

        memcpy (header->sp1h_appl_version, session->senderid,
            sizeof (session->senderid));
    } /* end if*/
    header->sp1h_filler1 = 0;
    header->sp1h_no_of_segm = 1;
}

/*----------------------------------------*/

void
i28pcmd (
    Script_SessionT   * session,
    const char * cmd,
    int len)
{
    /* ROUTINE_DBG_MSP00 ("i28pcmd"); */
    tsp1_packet    *send_packet = session->send_packet;
    char           *target;

    ASSERT_DBG (send_packet != NULL);
    if (len == UNDEF_SP00)
        len = strlen (cmd);
    target = PARTDATA (session->part) + PARTINFO (session->part, sp1p_buf_len);
    memcpy (target, cmd, len);
    PARTINFO (session->part, sp1p_buf_len) += len;
}

/*----------------------------------------*/

void
i28pcmdf (
    Script_SessionT   * session,
    const char * cmd,
    ...)
{
    /* ROUTINE_DBG_MSP00 ("i28pcmdf"); */
    va_list         args;
    int             bytecnt;
    char           *cmdBuf;
    tsp1_packet    *send_packet = session->send_packet;

    ASSERT_DBG (send_packet != NULL);
    va_start (args, cmd);
    cmdBuf = PARTDATA (session->part) + PARTINFO (session->part, sp1p_buf_len);
    bytecnt = vsprintf (cmdBuf, cmd, args);
    va_end (args);
    if (bytecnt < 0)
        return;
    PARTINFO (session->part, sp1p_buf_len) += bytecnt;
}

/*----------------------------------------*/

void
i28pusercmd (
    Script_SessionT   * session,
    const char * cmd,
    int len)
{
    /* ROUTINE_DBG_MSP00 ("i28pusercmd"); */
    tsp1_packet    *send_packet = session->send_packet;

    ASSERT_DBG (send_packet != NULL);
    if (len == UNDEF_SP00)
        len = strlen (cmd);
    memcpy (PARTDATA (session->part) + PARTINFO (session->part, sp1p_buf_len),
        cmd, len);
    PARTINFO (session->part, sp1p_buf_len) += len;
}

/*----------------------------------------*/

static void
i28_inc_buflen (
    Script_SessionT   * session,
    tsp00_Int4 bufpos,
    int sqllen)
{
    /* ROUTINE_DBG_MSP00 ("i28_inc_buflen"); */
    int newlen = bufpos + sqllen;

    /* old: PARTINFO (session->part, sp1p_buf_len) += sqllen + 1; */
    if (PARTINFO (session->part, sp1p_buf_len) < newlen)
        PARTINFO (session->part, sp1p_buf_len) = newlen;
}

/*----------------------------------------*/

int
i28parg (
    Script_SessionT   * session,
    const byte_t * arg,
    int varlen,  /* in bytes */
    int sqllen,  /* in bytes */
    tsp00_Int4 bufpos,
    char defByte)
{
    /* ROUTINE_DBG_MSP00 ("i28parg"); */
    int             mvlen;
    byte_t      *bufaddr;
    int             maxlen;
    tsp1_packet    *send_packet = session->send_packet;

    if (arg == NULL) {
        i28pnull (session, sqllen, bufpos);
        return Script_db_ok;
    }
    --sqllen;                   /* because of defined byte */
    ASSERT_DBG (send_packet != NULL);
    bufaddr = (byte_t *) (PARTDATA (session->part) + bufpos - 1);
    *bufaddr++ = defByte;
    maxlen = sqllen;
    if (varlen < sqllen)
        mvlen = varlen;
    else
        mvlen = sqllen;
    memcpy (bufaddr, arg, mvlen);

    if (maxlen > mvlen) {
        memset (bufaddr + mvlen, defByte, maxlen - mvlen);
    }
    i28_inc_buflen (session, bufpos, sqllen);
    return Script_db_ok;
}

/*----------------------------------------*/

void
i28addDataField (
    Script_SessionT   * session,
    const void        * data,
    int                 datalen)
{
    /* ROUTINE_DBG_MSP00 ("i28addDataField"); */
    byte_t     *bufaddr;
    tsp1_packet    *send_packet = session->send_packet;
    int             infolen;

    ASSERT_DBG (send_packet != NULL);
    bufaddr = PARTDATA (session->part) + PARTINFO (session->part, sp1p_buf_len);
    if (datalen <= csp1_fi_max_1byte_length) {
        bufaddr [0] = (byte_t) datalen;
        infolen = 1;
    }
    else {
        bufaddr [0] = csp1_fi_2byte_length;
        bufaddr [1] = (byte_t) (datalen / 256);
        bufaddr [2] = (byte_t) (datalen % 256);
        infolen = 3;
    }
    memcpy (bufaddr + infolen, data, datalen);
    PARTINFO (session->part, sp1p_buf_len) += infolen + datalen;
}

/*----------------------------------------*/

void
i28addStringField (
    Script_SessionT   * session,
    const char        * data)
{
    i28addDataField (session, data, strlen (data));
}

/*----------------------------------------*/

int
i28putUCS2 (
    Script_SessionT   * session,
    const byte_t * arg,
    const tsp77encoding * srcEncoding,
    int varlen,  /* in bytes */
    int sqllen,  /* in bytes */
    tsp00_Int4 bufpos)
{
    /* ROUTINE_DBG_MSP00 ("i28parg"); */
    int             mvlen;
    byte_t      *bufaddr;
    int             maxlen;
    tsp1_packet    *send_packet = session->send_packet;
    tsp78ConversionResult convResult;
    tsp00_Uint4 bytesWritten;
    tsp00_Uint4 bytesParsed;
    int         result;

    if (arg == NULL) {
        i28pnull (session, sqllen, bufpos);
        return Script_db_ok;
    }
    --sqllen;                   /* because of defined byte */
    ASSERT_DBG (send_packet != NULL);
    bufaddr = (byte_t *) (PARTDATA (session->part) + bufpos - 1);
    *bufaddr++ = ' ';
    convResult = sp78convertString (
        sp77encodingUCS2Native, bufaddr, sqllen, &bytesWritten, false,
        srcEncoding, arg, varlen, &bytesParsed);
    if ((convResult == sp78_Ok) && (bytesWritten < sqllen)) {
        void * fillTarget = bufaddr + bytesWritten;
        unsigned int remainingLen = sqllen - bytesWritten;
        sp77encodingUCS2Native->fillString (&fillTarget, &remainingLen,
            remainingLen, ' ');
    }
    i28_inc_buflen (session, bufpos, sqllen);
    if (convResult == sp78_Ok) {
        result = Script_db_ok;
    }
    else {
        result = -817;
    }
    return result;
}

/*----------------------------------------*/

void
i28pnull (
    Script_SessionT   * session,
    int sqllen,
    tsp00_Int4 bufpos)
{
    /* ROUTINE_DBG_MSP00 ("i28pnull"); */
    byte_t           *bufaddr;
    tsp1_packet    *send_packet = session->send_packet;

    ASSERT_DBG (send_packet != NULL);
    bufaddr = (byte_t *) (PARTDATA (session->part) + bufpos - 1);
    *bufaddr = (byte_t) csp_undef_byte;
#if !defined (FAST)
    memset (bufaddr + 1, '\0', sqllen - 1);
#endif
    i28_inc_buflen (session, bufpos, sqllen);
}

/*----------------------------------------*/

void
i28addNullField (
    Script_SessionT   * session)
{
    /* ROUTINE_DBG_MSP00 ("i28addNullField"); */
    byte_t           *bufaddr;
    tsp1_packet    *send_packet = session->send_packet;

    ASSERT_DBG (send_packet != NULL);
    bufaddr = PARTDATA (session->part) + PARTINFO (session->part, sp1p_buf_len);
    *bufaddr = csp1_fi_null_value;
    PARTINFO (session->part, sp1p_buf_len) += 1;
}

/*----------------------------------------*/

void
i28ppw (
    Script_SessionT   * session,
    tsp00_CryptPw pw,
    char defbyte)
{
    /* ROUTINE_DBG_MSP00 ("i28ppw"); */
    byte_t           *bufaddr;
    tsp1_packet          *send_packet = session->send_packet;

    ASSERT_DBG (send_packet != NULL);
    bufaddr = PARTDATA (session->part) + PARTINFO (session->part, sp1p_buf_len);
    *bufaddr = defbyte;
    memcpy (bufaddr + 1, pw, sizeof (tsp00_CryptPw));
    PARTINFO (session->part, sp1p_buf_len) += sizeof (tsp00_CryptPw) + 1;
}

/*----------------------------------------*/

void
i28gparseid (
    Script_SessionT   * session,
    Script_SqlParseid pid)
{
    /* ROUTINE_DBG_MSP00 ("i28gparseid"); */
    tsp1_packet    *rec_packet = session->rec_packet;
    tsp00_Bool           found;

    ASSERT_DBG (rec_packet != NULL);
    found = i28findpart (session, sp1pk_parsid);
    if (found) {
        memcpy (pid, PARTDATA (session->part), sizeof (Script_SqlParseid));
    }
    else {
        memset (pid, '\0', sizeof (Script_SqlParseid));
        pid[sizeof (Script_SqlParseid) - 2] = csp1_p_use_adbs;
    }
}

/*----------------------------------------*/

int
i28paramcount (
    Script_SessionT   * session)
{
    /* ROUTINE_DBG_MSP00 ("i28paramcount"); */
    if (i28findpart (session, sp1pk_shortinfo)) {
        return PARTINFO (session->part, sp1p_arg_count);
    }
    else if (i28findpart (session, sp1pk_vardata_shortinfo)) {
        return PARTINFO (session->part, sp1p_arg_count);
    }
    else {
        /*
         * currently, no hostvars -> no shortinfo part
         */
        return 0;
    }
}

/*----------------------------------------*/

tsp1_param_info *
i28gparaminfo (
    Script_SessionT   * session,
    int i)
{
    /* ROUTINE_DBG_MSP00 ("i28gparaminfo"); */
    tsp1_param_info *paramInfo;

    ASSERT_DBG (session->rec_packet != NULL);
    /* ASSERT_DBG (j02between (i, 0, i28paramcount (session) - 1));*/
    if (!i28findpart (session, sp1pk_shortinfo)) {
        i28findpart (session, sp1pk_vardata_shortinfo);
    }
    paramInfo = (tsp1_param_info *) (void*) PARTDATA (session->part);
    return paramInfo + i;
}

/*----------------------------------------*/

tsp00_Int4
i28nameslen (
    Script_SessionT   * session)
{
    /* ROUTINE_DBG_MSP00 ("i28nameslen"); */
    i28findpart (session, sp1pk_columnnames);
    return PARTINFO (session->part, sp1p_buf_len);
}

/*----------------------------------------*/

const char *
i28colnames (
    Script_SessionT   * session)
{
    /* ROUTINE_DBG_MSP00 ("i28colnames"); */
    i28findpart (session, sp1pk_columnnames);
    return PARTDATA (session->part);
}

/*----------------------------------------*/

tsp00_Bool
i28garg (
    Script_SessionT   * session,
    byte_t * arg,
    int varlen,
    int sqllen,
    tsp00_Int4 bufpos)
{
    /* ROUTINE_DBG_MSP00 ("i28garg"); */
    int             mvlen;
    unsigned char  *bufaddr;
    tsp1_packet    *rec_packet = session->rec_packet;

    ASSERT_DBG (rec_packet != NULL);
    if (varlen < sqllen)
        mvlen = varlen;
    else
        mvlen = sqllen;
    bufaddr = (unsigned char *) (PARTDATA (session->part) + bufpos - 1);
    if (*bufaddr == (unsigned char) csp_undef_byte)
        return false;
    memcpy (arg, bufaddr + 1, mvlen);
    return true;
}

/*----------------------------------------*/

byte_t           *
i28argaddr (
    Script_SessionT   * session,
    tsp00_Int4 bufpos)
{
    /* ROUTINE_DBG_MSP00 ("i28argaddr"); */
    unsigned char  *bufaddr;
    byte_t           *byteaddr;
    byte_t           *result;
    tsp1_packet    *rec_packet = session->rec_packet;

    ASSERT_DBG (rec_packet != NULL);
    ASSERT_DBG (PARTINFO (session->part, sp1p_part_kind) == sp1pk_data);
    byteaddr = PARTDATA (session->part) + bufpos - 1;
    bufaddr = (unsigned char *) byteaddr;
    if (*bufaddr == (unsigned char) csp_undef_byte)
        result = NULL;
    else
        result = byteaddr + 1;
    return result;
}

/*----------------------------------------*/

tsp00_Int4
i28resultcount (
    Script_SessionT   * session)
{
    /* ROUTINE_DBG_MSP00 ("i28resultcount"); */
    tsp00_Int4        result = UNDEF_SP00;
    byte_t      *number;
    tsp00_NumError   res;
    tsp1_part      *part_ptr;

    if (i28_findpart (session, sp1pk_resultcount, &part_ptr)) {
        number = PARTDATA (part_ptr) + 1;
        s40glint ((tsp00_Number*)number, 1,
                (PARTINFO (part_ptr, sp1p_buf_len) - 2) * 2, &result, &res);
    }
    return result;
} /* i28resultcount */

/*----------------------------------------*/

static void
i28_machinfo (
    tsp00_CodeType *code_type,
    tsp00_SwapKind *swap_kind,
    char          *termid)
{
    /* ROUTINE_DBG_MSP00 ("i28_machinfo"); */
    tsp00_Int4        i234;
    char             *c234;
    tsp100_VersionID0 VersionID0;

    *code_type = csp_ascii;
    i234 = 1;
    c234 = (char *) &i234;
    if (c234[3] == 1)
        (*swap_kind) = sw_normal;
    else if (c234[0] == 1)
        (*swap_kind) = sw_full_swapped;
    else
        (*swap_kind) = sw_part_swapped;

    sp100_GetVersionID ( VersionIDType0_esp100, s100buildnumberEx, &VersionID0 );

    sprintf(termid, "%01d%02d%02d",
                    (int) VersionID0.MajorVersion_sp100,
                    (int) VersionID0.MinorVersion_sp100,
                    (int) VersionID0.CorrLevel_sp100);
    memcpy (termid + sizeof (tsp00_C5), csp_comp_db_manager, sizeof (tsp00_C3));
}

/*----------------------------------------*/

int
i28adbs (
    Script_SessionT   * session,
    const char * cmd)
{
    /* ROUTINE_DBG_MSP00 ("i28adbs"); */
    i28initadbs (session);
    SENDINFO (session->segment, sp1c_with_info) = true;
    i28pcmd (session, cmd, UNDEF_SP00);
    return i28sql (session, NULL);
}

/*----------------------------------------*/

int
i28adbs_noinfo (
    Script_SessionT   * session,
    const char * cmd,
    bool withCommit)
{
    /* ROUTINE_DBG_MSP00 ("i28adbs_noinfo"); */
    i28initadbs (session);
    i28pcmd (session, cmd, UNDEF_SP00);
    if (withCommit) {
        session->segment->variant.C_3.sp1c_commit_immediately_F = true;
    }
    return i28sql (session, NULL);
}

/*----------------------------------------*/

int
i28adbsf (
    Script_SessionT   * session,
    const char * cmd,
    ...)
{
    /* ROUTINE_DBG_MSP00 ("i28adbsf"); */
    va_list         args;
    int             bytecnt;
    tsp1_packet    *send_packet = session->send_packet;

    ASSERT_DBG (send_packet != NULL);
    va_start (args, cmd);
    i28initadbs (session);
    bytecnt = vsprintf (PARTDATA (session->part), cmd, args);
    va_end (args);
    if (bytecnt < 0)
        return UNDEF_SP00;
    PARTINFO (session->part, sp1p_buf_len) = bytecnt;
    return i28sql (session, NULL);
}

/*----------------------------------------*/

int
i28adbsinfof (
    Script_SessionT   * session,
    const char * cmd,
    ...)
{
    /* ROUTINE_DBG_MSP00 ("i28adbsinfof"); */
    va_list         args;
    int             bytecnt;
    tsp1_packet    *send_packet = session->send_packet;

    ASSERT_DBG (send_packet != NULL);
    va_start (args, cmd);
    i28initadbs (session);
    SENDINFO (session->segment, sp1c_with_info) = true;
    bytecnt = vsprintf (PARTDATA (session->part), cmd, args);
    va_end (args);
    if (bytecnt < 0)
        return UNDEF_SP00;
    PARTINFO (session->part, sp1p_buf_len) = bytecnt;
    return i28sql (session, NULL);
}

/*----------------------------------------*/

int
i28adbs_describe (
    Script_SessionT   * session,
    Script_SqlParseid       pid)
{
    /* ROUTINE_DBG_MSP00 ("i28adbs_describe"); */
    int             bytecnt;
    tsp1_packet    *send_packet = session->send_packet;

    ASSERT_DBG (send_packet != NULL);
    i28initadbs (session);
    bytecnt = sprintf (PARTDATA (session->part), "%s", "DESCRIBE ");

    if (bytecnt < 0)
        return UNDEF_SP00;
    PARTINFO (session->part, sp1p_buf_len) = bytecnt;

    i28newpart (session, sp1pk_data);
    memcpy (PARTDATA (session->part) + PARTINFO (session->part, sp1p_buf_len),
        pid, sizeof (Script_SqlParseid));
    PARTINFO (session->part, sp1p_buf_len) += sizeof (Script_SqlParseid);
    return i28sql (session, NULL);
}   /* i28adbs_describe */

/*----------------------------------------*/

static          int
i28_put_num_string (
    Script_SessionT   * session,
    char *arg,
    tsp1_param_info * paramInfo)
{
    /* ROUTINE_DBG_MSP00 ("i28_put_num_string"); */
    tsp00_NumError   res;
    int             frac;

    frac = paramInfo->sp1i_frac;
    switch (paramInfo->sp1i_data_type) {
        case dfloat:
        case dvfloat:
            frac = -1;
            /* fall through */
        case dinteger:
        case dsmallint:
        case dfixed:
            {
                tsp00_Number num;

                s43pstr (&num, 1, paramInfo->sp1i_length, frac,
                    arg, 1, strlen(arg), &res);
                if (res != num_ok)
                    return Script_db_invalid_number;
                return i28parg (session, (char *) num,
                    paramInfo->sp1i_in_out_len,
                    paramInfo->sp1i_in_out_len,
                    paramInfo->variant.C_true.sp1i_bufpos_F, csp_defined_byte);
            }
     default:
            break;
    }
    return Script_db_ok;
}

/*----------------------------------------*/

static          int
i28_put_bool_string (
    Script_SessionT   * session,
    char             * arg,
    int                datalen,
    tsp1_param_info  * paramInfo)
{
    /* ROUTINE_DBG_MSP00 ("i28_put_bool_string"); */
   char            BoolVal;
   tsp00_C20         Temp;
   tsp00_Int4        NumVal;
   tsp00_NumError   res;
   int             i;

   if (datalen == UNDEF_SP00)
      datalen = strlen(arg);
   strncpy ((char *) Temp, arg, datalen);
   Temp [datalen] = '\0';
   for (i = 0; i < datalen; ++i) {
       Temp [i] = (char) toupper (Temp [i]);
   }

   if (strcmp ((char *) Temp, "true") == 0)
      BoolVal = '\1';
   else if (strcmp ((char *) Temp, "false") == 0)
      BoolVal = '\0';
   else {
       s45stoi4 (&NumVal, arg, 1, strlen (arg), (char *) &res);
       if (res != num_ok)
          return Script_db_invalid_number;
       if (NumVal == 0)
          BoolVal = '\0';
      else
          BoolVal = '\1';
   }
   return i28parg (session, &BoolVal, 1, paramInfo->sp1i_in_out_len,
      paramInfo->variant.C_true.sp1i_bufpos_F, csp_defined_byte);
}

/*----------------------------------------*/

static          tsp00_Int4
i28_get_num_param (
    void *addr,
    tsp1_param_info * paramInfo)
{
    /* ROUTINE_DBG_MSP00 ("i28_get_num_param"); */
    tsp00_NumError   res;
    tsp00_Int4        resval;

    s40glint ((tsp00_Number*)addr, 1,
        paramInfo->sp1i_in_out_len,
        &resval, &res);
    return resval;
}

/*----------------------------------------*/

static void
i28_num_param_to_string (
    char *DestStr,
    void *addr,
    tsp1_param_info * paramInfo)
{
    /* ROUTINE_DBG_MSP00 ("i28_num_param_to_string"); */
    int len;
    tsp00_DecimalPresentation decimal;
    tsp00_NumError res;

    decimal.thousand_token = 'N';
    decimal.zero_point = '.';
    s44egchr (addr, 1,
        paramInfo->sp1i_in_out_len,
        paramInfo->sp1i_frac, DestStr, 1, &len,
        &decimal, &res);
    DestStr [len] = '\0';
}

/*----------------------------------------*/

byte_t           *
i28_longargaddr (
    Script_SessionT   * session,
    tsp1_param_info * paramInfo)
{
    /* ROUTINE_DBG_MSP00 ("i28_longargaddr"); */
    unsigned char  *bufaddr;
    tsp00_LongDescriptor *descriptor;
    byte_t           *byteaddr;
    byte_t           *result;
    tsp1_packet    *rec_packet = session->rec_packet;

    ASSERT_DBG (rec_packet != NULL);
    ASSERT_DBG (paramInfo->sp1i_length == sizeof (tsp00_LongDescriptor));
    ASSERT_DBG (PARTINFO (session->part, sp1p_part_kind) == sp1pk_data);
    byteaddr = PARTDATA (session->part) + paramInfo->variant.C_true.sp1i_bufpos_F - 1;
    bufaddr = (unsigned char *) byteaddr;
    if (*bufaddr == (unsigned char) csp_undef_byte)
        return NULL;
    descriptor = (tsp00_LongDescriptor *)(byteaddr + 1);
    result = PARTDATA (session->part) + descriptor->variant.C_false.ld_valpos_F - 1;
    paramInfo->sp1i_length = descriptor->variant.C_false.ld_vallen_F;
    return result;
}

/*----------------------------------------*/

static          tsp00_Bool
i28_try_again (
    int rc)
{
    /* ROUTINE_DBG_MSP00 ("i28_try_again"); */
    tsp00_Bool           result;

    switch (rc) {
        case -8:               /* parse again */
        case 500:              /* wait for lock release */
        case 700:              /* timeout */
            result = true;
            break;
        default:
            result = false;
            break;
    }
    return result;
}

/*----------------------------------------*/

int
i28longiolen (
    int d_type,
    int len)
{
    /* ROUTINE_DBG_MSP00 ("i28longiolen"); */
    int             result;

    switch (d_type) {
        case dfixed:
        case dfloat:
        case dvfloat:
        case dsmallint:
        case dinteger:
            result = ((len + 1) / 2) + 1;
            break;
        case dboolean:
            result = 1;
            break;
        default:
            result = len;
            break;
    }
    return result + 1;
}

/*----------------------------------------*/

void
i28errmsg (
    Script_SessionT   * session,
    tsp00_C256c       * errbuf)
{
    /* ROUTINE_DBG_MSP00 ("i28errmsg"); */

    (*errbuf)[0] = '\0';

    if (session->rec_packet != NULL) {
        if (RECINFO (session->segment, sp1r_returncode) != Script_db_ok) {
            int     msglen;
            char    *errbuffer;

            if (i28findpart(session, sp1pk_errortext)) {
                errbuffer = PARTDATA (session->part);
                msglen = s30klen (errbuffer, ' ', (int) PARTINFO (session->part, sp1p_buf_len));

                if (msglen > sizeof (*errbuf))
                    msglen = sizeof (*errbuf);

                memcpy (*errbuf, errbuffer, msglen);
                (*errbuf)[msglen] = '\0';
            } /* end if */
        } /* end if */
    }else{
        if (session->lasterr.lasterr_on){
            size_t minlen = sizeof (*errbuf);

            if(minlen>sizeof(tsp00_ErrText)-1)
                minlen = sizeof(tsp00_ErrText)-1;

            memcpy(*errbuf, session->lasterr.errtext, minlen);
            (*errbuf)[minlen] = '\0';
        } /* end if */
    } /* end if */

} /* end i28errmsg */

/*----------------------------------------*/

int
i28droppid (
    Script_SessionT   * session,
    Script_SqlParseid pid)
{
    /* ROUTINE_DBG_MSP00 ("i28droppid"); */
    tsp1_packet    *send_packet = session->send_packet;

    ASSERT_DBG (send_packet != NULL);
    i28initadbs (session);
    i28pcmd (session, "DROP PARSEID", UNDEF_SP00);
    i28newpart (session, sp1pk_parsid);
    memcpy (PARTDATA (session->part) + PARTINFO (session->part, sp1p_buf_len),
        pid, sizeof (Script_SqlParseid));
    PARTINFO (session->part, sp1p_buf_len) += sizeof (Script_SqlParseid);
    return i28sql (session, NULL);
}

/*----------------------------------------*/

#define c_layerstr_len   20
#define c_routinename_len 16

int
i28switch (
    Script_SessionT   * session,
    const char * trace,
    const char * test)
{
    /* ROUTINE_DBG_MSP00 ("i28switch"); */
    tsp00_Int2        result;

    if (!session->switch_possible) {
        return Script_db_ok;
    }
    i28_reset (session, true);
    i28newsegment (session, sp1m_switch);
    i28newpart (session, sp1pk_command);
    i28pascalstring (PARTDATA (session->part), c_layerstr_len, trace, UNDEF_SP00);
    i28pascalstring (PARTDATA (session->part) + c_layerstr_len,
        c_layerstr_len, test, UNDEF_SP00);
    PARTINFO (session->part, sp1p_buf_len) = 2 * c_layerstr_len;
    result = i28sql (session, NULL);
    if (result != Script_db_ok) {
        /* next time be quiet */
        session->switch_possible = false;
    }
    return result;
}

/*----------------------------------------*/

int
i28switchlimit (
    Script_SessionT   * session,
    const char * trace,
    const char * test,
    const char * start,
    const char * stop,
    int limit)
{
    /* ROUTINE_DBG_MSP00 ("i28switchlimit"); */
    tsp00_Int2 result;
    int pos = 0;
    unsigned char buf [2];

    if (!session->switch_possible) {
        return Script_db_ok;
    }
    i28_reset (session, true);
    i28newsegment (session, sp1m_switchlimit);
    i28newpart (session, sp1pk_command);
    i28pascalstring (PARTDATA (session->part), c_layerstr_len, trace, UNDEF_SP00);
    pos += c_layerstr_len;
    i28pascalstring (PARTDATA (session->part) + pos,
        c_layerstr_len, test, UNDEF_SP00);
    pos += c_layerstr_len;
    i28pascalstring (PARTDATA (session->part) + pos,
        c_routinename_len, start, UNDEF_SP00);
    pos += c_routinename_len;
    i28pascalstring (PARTDATA (session->part) + pos,
        c_routinename_len, stop, UNDEF_SP00);
    pos += c_routinename_len;
    PARTINFO (session->part, sp1p_buf_len) = pos;
    buf [0] = limit / 256;
    buf [1] = limit % 256;
    i28pcmd (session, (const char *) buf, sizeof (buf));
    result = i28sql (session, NULL);
    if (result != Script_db_ok) {
        /* next time be quiet */
        session->switch_possible = false;
    }
    return result;
}

/*----------------------------------------*/

int
i28buflimit (
    Script_SessionT   * session,
    int limit)
{
    /* ROUTINE_DBG_MSP00 ("i28buflimit"); */
    tsp00_Int2        result;
    unsigned char buf [2];

    if (!session->switch_possible) {
        return Script_db_ok;
    }
    i28_reset (session, true);
    i28newsegment (session, sp1m_buflength);
    i28newpart (session, sp1pk_command);
    buf [0] = limit / 256;
    buf [1] = limit % 256;
    i28pcmd (session, (const char *) buf, sizeof (buf));
    result = i28sql (session, NULL);
    if (result != Script_db_ok) {
        /* next time be quiet */
        session->switch_possible = false;
    }
    return result;
}

/*----------------------------------------*/

static int
i28_minmaxbuf (
    Script_SessionT   * session,
    tsp1_cmd_mess_type_Enum messKind)
{
    /* ROUTINE_DBG_MSP00 ("i28_minmaxbuf"); */
    tsp00_Int2        result;

    if (!session->switch_possible) {
        return Script_db_ok;
    }
    i28_reset (session, true);
    i28newsegment (session, messKind);
    i28newpart (session, sp1pk_command);
    result = i28sql (session, NULL);
    if (result != Script_db_ok) {
        /* next time be quiet */
        session->switch_possible = false;
    }
    return result;
}

/*----------------------------------------*/

int
i28minbuf (
    Script_SessionT   * session)
{
    /* ROUTINE_DBG_MSP00 ("i28minbuf"); */

    return i28_minmaxbuf (session, sp1m_minbuf);
}

/*----------------------------------------*/

int
i28maxbuf (
    Script_SessionT   * session)
{
    /* ROUTINE_DBG_MSP00 ("i28maxbuf"); */

    return i28_minmaxbuf (session, sp1m_maxbuf);
}

/*----------------------------------------*/

/**\
--- ----------------------------------MF__ MOD__ high level select
    Here some functions providing a interface for execute select
    statements and retrieve the results
\**/

/*----------------------------------------*/

#define cin28_cont_cont "CONTINUE\n"
#define cin28_cont_end  "END     \n"
#define cin28_cont_len  (strlen(cin28_cont_end))
#define cin28_min_len   10
#define cin28_nulltext  "(null)"

/*----------------------------------------*/

/* checks if a data type of the db is a string type
   (for seting of string delimiter sin the result) */
static tsp00_Bool
i28_isstring (tsp00_DataType aDataType)
{
    /* ROUTINE_DBG_MSP00 ("i28_isstring"); */

  tsp00_Bool bRc = false;

  switch(aDataType) {
    /* Had I hit all data types??? */
    case dcha:
    case dche:
    case dchb:
    case dunicode:
    case dvarchara:
    case dvarchare:
    case dvarcharb:
    case ddate:
    case dtime:
    case dtimestamp:
    case dstra:
    case dstre:
    case dstrb:
    case dstrdb:
      bRc = true;
  } /* end switch */

  return bRc;

} /* end i28_isstring */

/*----------------------------------------*/

/* calculates a approximate length of one result record */
static int
i28_rowlen (
    Script_SessionT    * session)
{
    /* ROUTINE_DBG_MSP00 ("i28_rowlen"); */

    int               nParamCount = 0,
                      i           = 0,
                      nRowLen     = 0;
    tsp1_param_info * pParamInfo  = NULL;

    /**/

    /* get some infos */
    pParamInfo = i28gparaminfo(session, 0);
    nParamCount = i28paramcount(session);

    /* calculate approximate rowlen */
    for (i = 0; i < nParamCount; i++) {
      nRowLen = nRowLen + ((pParamInfo[i].sp1i_length < cin28_min_len) ? pParamInfo[i].sp1i_length : cin28_min_len);
    } /* end for */

    /**/

    return nRowLen;

} /* end i28_rowlen */

/*----------------------------------------*/

/**\
--- ----------------------------------MF__ MOD__ chapter special cases
    The following routines serve very specific needs.
    They are included so that clients don't need the
    internal structure of segments and parts
\**/

char           *
i28outargsbuf (
    Script_SessionT   * session)
{
    /* ROUTINE_DBG_MSP00 ("i28outargsbuf"); */
    i28findpart (session, sp1pk_data);
    return PARTDATA (session->part);
}

/*----------------------------------------*/

char           *
i28argsbuf (
    Script_SessionT   * session)
{
    /* ROUTINE_DBG_MSP00 ("i28argsbuf"); */
    return PARTDATA (session->part);
}

/*----------------------------------------*/

void
i28argsbuflen (
    Script_SessionT   * session,
    int buflen)
{
    /* ROUTINE_DBG_MSP00 ("i28argsbuflen"); */
    PARTINFO (session->part, sp1p_buf_len) = buflen;
}

/*----------------------------------------*/

tsp00_Int4
i28packetlen (
    Script_SessionT   * session)
{
    /* ROUTINE_DBG_MSP00 ("i28packetlen"); */
    return s26packet_len (session->send_packet);
}

/*----------------------------------------*/

static void
i28_puttermid (
    Script_SessionT   * session,
    tsp00_TermId termid)
{
    /* ROUTINE_DBG_MSP00 ("i28_puttermid"); */
    char           *target;
    tsp1_packet    *packet = session->send_packet;

    target = PARTDATA (session->part) + PARTINFO (session->part, sp1p_buf_len);
    *target = csp_defined_byte;
    ++target;
    memcpy (target, termid, sizeof (tsp00_TermId));
    PARTINFO (session->part, sp1p_buf_len) += 1 + sizeof (tsp00_TermId);
}

/*----------------------------------------*/

static void
i28_connectOptions (
    Script_SessionT   * session)
{
    /* ROUTINE_DBG_MSP00 ("i28_sqlmode"); */
    if (isalpha (session->connectInfo.sqlmode[0]) != '\0') {
        i28pcmdf (session, " SQLMODE %s",
            (char *) session->connectInfo.sqlmode);
    }
    if (session->connectInfo.cachelimit != -1) {
        i28pcmdf (session, " CACHELIMIT %d ", session->connectInfo.cachelimit);
    }
    if (session->connectInfo.timeout != -1) {
        i28pcmdf (session, " TIMEOUT %d ", session->connectInfo.timeout);
    }
    if (session->connectInfo.isolation != -1) {
        i28pcmdf (session, " ISOLATION LEVEL %d ", session->connectInfo.isolation);
    }
    if (session->space_option) {
        i28pcmd (session, " SPACE OPTION", -1);
    }
}

/*----------------------------------------*/

static void
i28_addFeatureRequests (
    Script_SessionT   * session)
{
    byte_t           *bufaddr;

    i28newpart (session, sp1pk_feature);
    bufaddr = PARTDATA (session->part);
    bufaddr [0] = sp1f_multiple_drop_parseid;
    bufaddr [1] = 1;
    PARTINFO (session->part, sp1p_arg_count) = 1;
    bufaddr += 2;
    PARTINFO (session->part, sp1p_buf_len) += 2;
    if (session->space_option) {
        bufaddr [0] = sp1f_space_option;
        bufaddr [1] = 1;
        PARTINFO (session->part, sp1p_arg_count) += 1;
        bufaddr += 2;
        PARTINFO (session->part, sp1p_buf_len) += 2;
    }
    if (session->variable_input) {
        bufaddr [0] = sp1f_variable_input;
        bufaddr [1] = 1;
        PARTINFO (session->part, sp1p_arg_count) += 1;
        bufaddr += 2;
        PARTINFO (session->part, sp1p_buf_len) += 2;
    }
}

/*----------------------------------------*/

void
i28connectcommand_forResponse (
    Script_SessionT * session)
{
    /* ROUTINE_DBG_MSP00 ("i28connectcommand_forResponse"); */
    session->buildingCommand = false;
    i28initadbs (session);
    SENDINFO (session->segment, sp1c_producer) = sp1pr_internal_cmd;
    i28pcmd (session, "CONNECT ? IDENTIFIED by :PW", UNDEF_SP00);
    i28_connectOptions (session);
    i28_addFeatureRequests (session);
}

/*----------------------------------------*/

void
i28connectcommand (
    Script_SessionT   * session,
    tsp00_TermId termid)
{
    /* ROUTINE_DBG_MSP00 ("i28connectcommand"); */
    session->buildingCommand = false;
    i28initadbs (session);
    SENDINFO (session->segment, sp1c_producer) = sp1pr_internal_cmd;
    i28pcmdf (session, "CONNECT \"%s\" IDENTIFIED by :PW",
        (char *) session->connectInfo.username);
    i28_connectOptions (session);
    if (!session->try_challenge_response) {
        i28newpart (session, sp1pk_data);
        i28ppw (session, session->connectInfo.password, csp_defined_byte);
        i28_puttermid (session, termid);
    }
    i28_addFeatureRequests (session);
}

/*----------------------------------------*/

void
i28tablebuffer (
    Script_SessionT   * session,
    char **tablebuffer,
    tsp00_Int4 * buflen)
{
    /* ROUTINE_DBG_MSP00 ("i28tablebuffer"); */
    tsp1_packet    *rec_packet = session->rec_packet;

    tsp00_Bool           partFound;

    ASSERT_DBG (rec_packet != NULL);
    partFound = i28findpart (session, sp1pk_conv_tables_returned);
    ASSERT_DBG (partFound);
    *tablebuffer = PARTDATA (session->part);
    *buflen = PARTINFO (session->part, sp1p_buf_len);
}

/*----------------------------------------*/

char           *
i28cmdbuf (
    Script_SessionT   * session)
{
    /* ROUTINE_DBG_MSP00 ("i28cmdbuf"); */
    return PARTDATA (session->part);
}

/*----------------------------------------*/

char           *
i28getpart (
    Script_SessionT   * session)
{
    /* ROUTINE_DBG_MSP00 ("i28getpart"); */
    return PARTDATA (session->part);
}

/*----------------------------------------*/

void
i28extendpart (
    Script_SessionT   * session,
    int lendiff)
{
    /* ROUTINE_DBG_MSP00 ("i28extendpart"); */
    PARTINFO (session->part, sp1p_buf_len) += lendiff;
}

/*----------------------------------------*/

tsp00_Int4
i28partlen (
    Script_SessionT   * session)
{
    /* ROUTINE_DBG_MSP00 ("i28partlen"); */
    return PARTINFO (session->part, sp1p_buf_len);
}

/*----------------------------------------*/

tsp00_Int4
i28bytesfree (
    Script_SessionT   * session)
{
    /* ROUTINE_DBG_MSP00 ("i28bytesfree"); */
    return PARTINFO (session->part, sp1p_buf_size)
        - PARTINFO (session->part, sp1p_buf_len) - 4;
}

/*----------------------------------------*/

void
i28addpartdata (
    Script_SessionT   * session,
    const void *data,
    int len)
{
    /* ROUTINE_DBG_MSP00 ("i28addpartdata"); */
    memcpy (PARTDATA (session->part) + PARTINFO (session->part, sp1p_buf_len),
        data, len);
    PARTINFO (session->part, sp1p_buf_len) += len;
    ASSERT_DBG (PARTINFO (session->part, sp1p_buf_len)
        <= PARTINFO (session->part, sp1p_buf_size));
}

/*----------------------------------------*/

tsp1_param_info *
i28allocparaminfos (
    Script_SessionT   * session,
    int paramCount)
{
    /* ROUTINE_DBG_MSP00 ("i28allocparaminfos"); */
    i28_reset (session, true);
    i28newsegment (session, sp1m_parse);
    i28newpart (session, sp1pk_shortinfo);
    PARTINFO (session->part, sp1p_arg_count) = (tsp00_Int2) paramCount;
    PARTINFO (session->part, sp1p_buf_len) = paramCount * sizeof (tsp1_param_info);
    return (tsp1_param_info *) PARTDATA (session->part);
}

/*----------------------------------------*/

void
i28newnamepart (
    Script_SessionT   * session)
{
    /* ROUTINE_DBG_MSP00 ("i28newnamepart"); */
    i28newpart (session, sp1pk_columnnames);
    PARTINFO (session->part, sp1p_arg_count) = 0;
}

/*----------------------------------------*/

void
i28addname (
    Script_SessionT   * session,
    const char * name)
{
    /* ROUTINE_DBG_MSP00 ("i28addname"); */
    int             namelen;
    char           *buf;

    ASSERT_DBG (PARTINFO (session->part, sp1p_part_kind) == sp1pk_columnnames);
    namelen = strlen (name); /* physical, not logical */
    buf = PARTDATA (session->part) + PARTINFO (session->part, sp1p_buf_len);
    buf[0] = (char) namelen;
    memcpy (buf + 1, name, namelen);
    PARTINFO (session->part, sp1p_buf_len) += namelen + 1;
    ++PARTINFO (session->part, sp1p_arg_count);
}

/*----------------------------------------*/

tsp00_Bool
i28hello (
    Script_SessionT   * session)
{
    /* ROUTINE_DBG_MSP00 ("i28hello"); */
    i28_reset (session, true);
    i28newsegment (session, sp1m_hello);
    i28sql (session, NULL);
    return session->is_connected;
}

/*----------------------------------------*/

#define c_intostr " Into ?"
#define c_intolen 7
#define c_varstr ", ?"
#define c_varlen 3

static void
i28_fetchcmd (
    Script_SessionT   * session,
    const char * fetchCmd,
    int fetchVars)
{
    /* ROUTINE_DBG_MSP00 ("i28_fetchcmd"); */
    char           *cmdBuf;
    int             fetchLen;
    int             additional;

    i28initadbs (session);
    fetchLen = strlen (fetchCmd);
    cmdBuf = PARTDATA (session->part) + PARTINFO (session->part, sp1p_buf_len);
    memcpy (cmdBuf, fetchCmd, fetchLen);
    cmdBuf += fetchLen;
    memcpy (cmdBuf, c_intostr, c_intolen);
    --fetchVars;
    cmdBuf += c_intolen;
    additional = fetchLen + c_intolen + (fetchVars * c_varlen);
    while (fetchVars > 0) {
        memcpy (cmdBuf, c_varstr, c_varlen);
        cmdBuf += c_varlen;
        --fetchVars;
    }
    PARTINFO (session->part, sp1p_buf_len) += additional;
}

/*----------------------------------------*/

int
i28fetchcmd (
    Script_SessionT   * session,
    const char * fetchCmd,
    int fetchVars)
{
    /* ROUTINE_DBG_MSP00 ("i28fetchcmd"); */
    i28_fetchcmd (session, fetchCmd, fetchVars);
    return i28sql (session, NULL);
}

/*----------------------------------------*/

int
i28mfetchcmd (
    Script_SessionT   * session,
    const char * fetchCmd,
    int fetchVars,
    int rowsToFetch)
{
    /* ROUTINE_DBG_MSP00 ("i28mfetchcmd"); */
    tsp00_Number      num;
    tsp00_NumError   res;
    int             rc = Script_db_ok;

    i28_fetchcmd (session, fetchCmd, fetchVars);
    i28newpart (session, sp1pk_resultcount);
    s41plint (&num, 1, 18, 0, rowsToFetch, &res);
    rc = i28parg (session, (char *) num, sizeof (num), sizeof (num) + 1,
        1, csp_defined_byte);
    if (rc != Script_db_ok)
        return rc;
    SENDINFO (session->segment, sp1c_mass_cmd) = true;
    return i28sql (session, NULL);
}

/*----------------------------------------*/

int
i28functioncode (
    Script_SessionT   * session)
{
    /* ROUTINE_DBG_MSP00 ("i28functioncode"); */
    return RECINFO (session->segment, sp1r_function_code);
}

/*----------------------------------------*/

tsp00_Bool
i28gresulttablename (
    Script_SessionT   * session,
    int              * len,
    char             * resulttablename)
{
    /* ROUTINE_DBG_MSP00 ("i28gresulttablename"); */
    int                r_len = 0;

    if (i28findpart (session, sp1pk_resulttablename)) {
        r_len = PARTINFO (session->part, sp1p_buf_len);
        if (r_len > 0) {
            memcpy (resulttablename, PARTDATA (session->part), r_len);
        }
    }
    *len = r_len;
    return (r_len > 0);
} /* i28gresulttablename */

/*----------------------------------------*/

void
i28sresulttablename (
    Script_SessionT   * session,
    int                len,
    char             * resulttablename)
{
    /* ROUTINE_DBG_MSP00 ("i28sresulttablename"); */

    i28newpart (session, sp1pk_resulttablename);
    memcpy (PARTDATA (session->part) + PARTINFO (session->part, sp1p_buf_len),
        resulttablename, len );
    PARTINFO (session->part, sp1p_buf_len) += len;
} /* i28sresulttablename */

/*----------------------------------------*/

tsp00_Bool
i28forreuse (
    Script_SessionT   * session)
{
    /* ROUTINE_DBG_MSP00 ("i28forreuse"); */
    Script_SqlParseid       pid;

    if (!i28findpart (session, sp1pk_parsid))
        return false;
    STRUCT_COPY (pid, PARTDATA (session->part));
    return ((pid [10] == csp1_p_reuse_mass_select_found)
         || (pid [10] == csp1_p_reuse_update_sel_found)
         || (pid [10] == csp1_p_reuse_mselect_found)
         || (pid [10] == csp1_p_reuse_upd_mselect_found)
           );
} /* i28forreuse */

/*----------------------------------------*/

tsp00_Bool
i28forupdate (
    Script_SessionT   * session)
{
    /* ROUTINE_DBG_MSP00 ("i28forupdate"); */
    Script_SqlParseid pid;
    if (!i28findpart (session, sp1pk_parsid))
        return false;
    STRUCT_COPY (pid, PARTDATA (session->part) + PARTINFO (session->part, sp1p_buf_len));
    return ((pid [10] == csp1_p_select_for_update_found)
         || (pid [10] == csp1_p_for_upd_mselect_found)
         || (pid [10] == csp1_p_reuse_update_sel_found)
         || (pid [10] == csp1_p_reuse_upd_mselect_found)
           );
} /* i28forupdate */

/*----------------------------------------*/

int
i28rowsfetched (
    Script_SessionT   * session)
{
    /* ROUTINE_DBG_MSP00 ("i28rowsfetched"); */
    i28findpart (session, sp1pk_data);
    return PARTINFO (session->part, sp1p_arg_count);
}

/*----------------------------------------*/

int
i28rowlen (
    Script_SessionT   * session)
{
    /* ROUTINE_DBG_MSP00 ("i28rowlen"); */
    int             result;
    int             recordCount;

    i28findpart (session, sp1pk_data);
    recordCount = PARTINFO (session->part, sp1p_arg_count);
    result = PARTINFO (session->part, sp1p_buf_len) / recordCount;
    return result;
}

/*----------------------------------------*/

tsp00_Bool
i28endoffetch (
    Script_SessionT   * session,
    int rowsExpected)
{
    /* ROUTINE_DBG_MSP00 ("i28endoffetch"); */
    int             endOfFetch;
    int             recordCount;
    int             recordLen;
    int             bytesRemaining;

    switch (RECINFO (session->segment, sp1r_returncode)) {
        case Script_db_row_not_found:
            endOfFetch = true;
            break;
        case Script_db_ok:
            i28findpart (session, sp1pk_data);
            recordCount = PARTINFO (session->part, sp1p_arg_count);
            if (recordCount == rowsExpected)
                endOfFetch = false;
            else {
                recordLen = PARTINFO (session->part, sp1p_buf_len) / recordCount;
                bytesRemaining = PARTINFO (session->part, sp1p_buf_size)
                    - PARTINFO (session->part, sp1p_buf_len);
                if (bytesRemaining >= recordLen)
                    endOfFetch = true;
                else
                    endOfFetch = false;
            }
            break;
        default:
            return true;
            break;
    }
    return endOfFetch;
}

/*----------------------------------------*/
/**\
--- ----------------------------------MF__ MOD__ chapter communication
\**/
void
i28pascalstring (
    char       *target,
    int         targetlen,
    const char *source,
    int         sourcelen)
{
    /* ROUTINE_DBG_MSP00 ("i28pascalstring"); */
    if (source == NULL) {
        /* one blank so memcpy is ok */
        source = " ";
        sourcelen = 1;
    }
    else {
        if (sourcelen == -1)
            sourcelen = strlen (source);
    }
    if (sourcelen > targetlen)
        memcpy (target, source, (int) targetlen);
    else {
        memcpy (target, source, (int) sourcelen);
        memset (target + sourcelen, ' ', (int) (targetlen - sourcelen));
    }
}

/*----------------------------------------*/

Script_SessionT   *
i28lasterr_off (
    Script_SessionT   * session)
{
    /* ROUTINE_DBG_MSP00 ("i28lasterr_off"); */
    session->lasterr.lasterr_on = false;
    return session;
} /* i28lasterr_off */

/*----------------------------------------*/

Script_SessionT   *
i28lasterr_on (
    Script_SessionT   * session)
{
    /* ROUTINE_DBG_MSP00 ("i28lasterr_on"); */
    session->lasterr.lasterr_on = true;
    return session;
} /* i28lasterr_on */

/*----------------------------------------*/

void
i28setlasterr_rte (
    Script_SessionT   * session,
    tsp00_ErrTextc      errtext,
    tsp01_CommErr       sp_rc)
{
    /* ROUTINE_DBG_MSP00 ("i28setlasterr_rte"); */
    ASSERT_DBG (session != NULL);
    if (!session->lasterr.lasterr_on)
        return;
    session->lasterr.sp_rc = sp_rc;
    if (sp_rc == commErrOk_esp01)
        return;
    strncpy ((char *) session->lasterr.errtext, errtext, sizeof (session->lasterr.errtext));
    session->lasterr.errtext [sizeof (session->lasterr.errtext) - 1] = '\0';
}

/*----------------------------------------*/

void
i28setlasterr_rte2 (
    Script_SessionT   * session,
    tsp00_ErrText       errtext,
    tsp01_CommErr       sp_rc)
{
    /* ROUTINE_DBG_MSP00 ("i28setlasterr_rte"); */
    unsigned char * current;

    ASSERT_DBG (session != NULL);
    if (!session->lasterr.lasterr_on)
        return;
    session->lasterr.sp_rc = sp_rc;
    if (sp_rc == commErrOk_esp01)
        return;
    memcpy (session->lasterr.errname, errtext, sizeof (session->lasterr.errname));
    current = session->lasterr.errname
        + sizeof (session->lasterr.errname) - 1;
    while (*current == ' ') {
        --current;
    }
    *current = '\0';
}

/*----------------------------------------*/

void
i28resetlasterr (
    Script_SessionT   * session)
{
    /* ROUTINE_DBG_MSP00 ("i28resetlasterr"); */
    ASSERT_DBG (session != NULL);
    session->lasterr.sqlresult.returnCode = Script_db_ok;
    session->lasterr.sqlresult.errorPos = 0;
    memset (session->lasterr.sqlresult.sqlmsg, ' ',
            sizeof (session->lasterr.sqlresult.sqlmsg));
    memset (session->lasterr.errname, ' ',
            sizeof (session->lasterr.errname));
}

/*----------------------------------------*/

void
i28whenever_errpos (
    Script_SessionT      *session,
    int                 whenever_pos)
{
    /* ROUTINE_DBG_MSP00 ("i28whenever_errpos"); */
    Script_SqlResultT  *sqlresult;
    if (whenever_pos == UNDEF_SP00)
        return;
    sqlresult = &session->lasterr.sqlresult;
    if (sqlresult->errorPos >= whenever_pos)  {
        sqlresult->errorPos = -(sqlresult->errorPos - whenever_pos);
    }
}

/*----------------------------------------*/
int
i28replyavailable (
    Script_SessionT   * session)
{
    /* ROUTINE_DBG_MSP00 ("i28replyavailable"); */
    tsp00_ErrText     errtext;
    tsp01_CommErr  sp_rc;

    sqlareplyavailable (session->reference, errtext, &sp_rc);

    i28setlasterr_rte2 (session, errtext, sp_rc) ;
    return sp_rc;
} /* end i28replyavailable */

/*----------------------------------------*/
void
i28cancel (
    Script_SessionT   * session)
{
    /* ROUTINE_DBG_MSP00 ("i28cancel"); */
    sqlacancel(session->reference);

} /* end i28cancel */

/*----------------------------------------*/

tsp01_CommErr
i28request (
    Script_SessionT   * session)
{
    /* ROUTINE_DBG_MSP00 ("i28request"); */
    tsp00_ErrText     errtext;
    tsp01_CommErr  sp_rc;

    ASSERT_DBG (session->is_connected);
    sqlarequest (session->reference, session->send_packet,
        i28packetlen (session), errtext, &sp_rc);
    if ((sp_rc == commErrTimeout_esp01) || (sp_rc == commErrCrash_esp01)) {
        i28cleanup_session (session);
        /*
        sqlarelease (session->reference);
        session->is_connected = false;
        */
    }
    if (sp_rc != commErrOk_esp01) {
        if (sp_rc == commErrTimeout_esp01)
            i28connect (session);
    }
    i28setlasterr_rte2 (session, errtext, sp_rc) ;
    return sp_rc;
}

/*----------------------------------------*/

tsp01_CommErr
i28receive (
    Script_SessionT   * session)
{
    /* ROUTINE_DBG_MSP00 ("i28receive"); */
    tsp00_Int4        length;
    tsp00_ErrText     errtext;
    tsp01_CommErr  sp_rc;

    ASSERT_DBG (session->is_connected);
    sqlareceive (session->reference, (void**)&session->rec_packet,
        &length, errtext, &sp_rc);
    if ((sp_rc == commErrTimeout_esp01) || (sp_rc == commErrCrash_esp01)) {
        sqlarelease (session->reference);
        session->is_connected = false;
    }
    if (sp_rc != commErrOk_esp01) {
        session->rec_packet = NULL;
    }
    i28setlasterr_rte2 (session, errtext, sp_rc) ;
    return sp_rc;
}

/*----------------------------------------*/

void
i28callsql (
    Script_SessionT   * session,
    tsp01_CommErr * comm_error)
{
    /* ROUTINE_DBG_MSP00 ("i28callsql"); */
    *comm_error = i28request (session);
    if (*comm_error == commErrOk_esp01)
        *comm_error = i28receive (session);
    i28lasterr_on (session);
}

/*----------------------------------------*/

void
i28setlasterr_name (
    Script_SessionT      * session,
    tsp00_KnlIdentifier  * errname)
{
    /* ROUTINE_DBG_MSP00 ("i28setlasterr_name"); */
    ASSERT_DBG (session != NULL);
    STRUCT_COPY (session->lasterr.errname, errname);
}

/*----------------------------------------*/

void
i28sqllasterr (
    Script_SessionT       * session,
    tsp00_ErrTextc        * errtext,
    tsp00_KnlIdentifierc  * errname,
    tsp00_Int2            * errcode,
    tsp00_Int2            * errpos)
{
    /* ROUTINE_DBG_MSP00 ("i28sqllasterr"); */
    #define RTEERR_OFFSET   10000
    * errpos = 0;
    *errcode = 0;
    memset(errtext, 0, sizeof(tsp00_ErrTextc));
    memset(errname, 0, sizeof(tsp00_KnlIdentifierc));
    if (session->lasterr.sp_rc != commErrOk_esp01) {
        memcpy (errtext, session->lasterr.errtext, sizeof (tsp00_ErrText));
        memcpy (errname, session->lasterr.errname, sizeof (tsp00_KnlIdentifier));
        *errcode = (tsp00_Int2) (session->lasterr.sp_rc + RTEERR_OFFSET);
        session->lasterr.sp_rc = commErrOk_esp01;
    }
    else if (session->lasterr.sqlresult.returnCode != Script_db_ok) {
        *errcode = session->lasterr.sqlresult.returnCode;
        *errpos = session->lasterr.sqlresult.errorPos;
        memcpy (errtext, session->lasterr.sqlresult.sqlmsg, sizeof (tsp00_ErrText));
        memcpy (errname, session->lasterr.errname, sizeof (tsp00_KnlIdentifier));
        session->lasterr.sqlresult.returnCode = Script_db_ok;
    }
    else {
        *errcode = 0;
        memset (errtext, ' ', sizeof (tsp00_ErrText));
        memset (errname, ' ', sizeof (tsp00_KnlIdentifier));
    }
}

/*----------------------------------------*/

typedef struct PropertyT {
    char * name;
    int    namelen;
    unsigned char * data;
    int             datalen;
} PropertyT;

/*----------------------------------------*/

void
i28_init_challenge (
    Script_SessionT   * session,
    RTESec_ClientAuthenticationBuffer * clientBuffer,
    tsp00_TermId termid)
{
    /* ROUTINE_DBG_MSP00 ("i28_init_challenge"); */
    int plainParameters = 0;
    session->buildingCommand = false;
    i28initadbs (session);
    SENDINFO (session->segment, sp1c_producer) = sp1pr_internal_cmd;
    i28pcmd (session, "CONNECT ? AUTHENTICATION", UNDEF_SP00);
    i28newVardataPart (session, 0);
    i28addStringField (session, (char *) session->connectInfo.username);
    plainParameters = 1;
    {
        /*
         * add possible algorithms
         */
        const RTESec_Authentication * auth;
        int index = 0;
        int namelen = strlen (session->connectInfo.username);
        while (RTESec_Authentication_Next (&auth, &index)) {
            const char * authName = RTESec_Authentication_Name (auth);
            const void * databuf;
            int  outlen;
            if (RTESec_Authentication_DataForChallengeRequest (
                    auth, clientBuffer,
                    (const char *) session->connectInfo.username, namelen,
                    &databuf, &outlen))
            {
                i28addStringField (session, authName);
                i28addDataField (session, databuf, outlen);
            }
        }
        i28setVardataFieldCount (session, 1 + index * 2);
    }
}

/*----------------------------------------*/

static void
i28_getField (
    unsigned char ** source_in,
    unsigned char ** fieldptr,
    int            * fieldlen)
{
    unsigned char * source = *source_in;
    int indicatorLen = 1;

    switch (source [0]) {
        case csp1_fi_ignored:
        case csp1_fi_special_null:
        case csp1_fi_blob_description:
        case csp1_fi_default_value:
        case csp1_fi_null_value:
            *fieldptr = NULL;
            *fieldlen = 0;
            break;
        case csp1_fi_2byte_length:
            indicatorLen = 3;
            *fieldptr = source + indicatorLen;
            *fieldlen = source [1] * 256 + source [2];
            break;
        default:
            *fieldptr = source + indicatorLen;
            *fieldlen = source [0];
            break;
    }
    *source_in = source + indicatorLen + *fieldlen;
}

/*----------------------------------------*/

static int
i28_challenge_response (
    Script_SessionT   * session,
    tsp00_TermId      termid)
{
    int sqlrc;
    tsp1_part * part;
    unsigned char * vardata;
    int fieldCount;
    PropertyT challenge;
    PropertyT response;
    const RTESec_Authentication * authenticator;
    RTESec_ClientAuthenticationBuffer clientBuffer;
    const char * authName;
    tsp00_Name clearPwd;
    /*
     * build connect packet
     */
    i28_init_challenge (session, &clientBuffer, termid);
    /*
     * get repply
     */
    sqlrc = i28sql (session, NULL);
    if (sqlrc != 0) {
        return sqlrc;
    }
    /*
     * extract challenge
     */
    if (!i28findpart (session, sp1pk_vardata)) {
        return -2;
    }
    vardata = (unsigned char*) PARTDATA(session->part);
    fieldCount = vardata [0] * 256 + vardata [1];
    if (fieldCount < 2) {
        return -2;
    }
    vardata += 2;
    i28_getField (&vardata, (unsigned char **) &challenge.name, &challenge.namelen);
    i28_getField (&vardata, &challenge.data, &challenge.datalen);
    /*
     * create response
     */
    authenticator = RTESec_Authentication_FindByName (challenge.name, challenge.namelen);
    if (authenticator == NULL) {
        return -2;
    }
    s02appldecrypt (clearPwd, session->connectInfo.password);
    if (!RTESec_Authentication_CreateResponse (authenticator,
        &clientBuffer, (void *) &response.data, &response.datalen,
        challenge.data, challenge.datalen,
        (const char *) &clearPwd [0], sizeof (clearPwd)))
    {
        return -2;
    }

    /*
     * exchange real connect
     */
    i28connectcommand_forResponse (session);
    i28newVardataPart (session, 3);
    authName = RTESec_Authentication_Name (authenticator);
    i28addStringField (session, session->connectInfo.username);
    i28addStringField (session, authName);
    i28addDataField (session, response.data, response.datalen);
    i28newpart (session, sp1pk_clientid);
    i28_puttermid (session, termid);
    sqlrc = i28sql (session, NULL);
    if (sqlrc != 0) {
        return sqlrc;
    }
    /*
     * return
     */
    return 0;
}

/*----------------------------------------*/

static void
i28_getConnectReply (
    Script_SessionT   * session)
{
    byte_t           *bufaddr;

    if (i28findpart (session, sp1pk_feature)) {
        int i;
        int featureCount = PARTINFO (session->part, sp1p_arg_count);
        bufaddr = PARTDATA (session->part);
        for (i = 0; i < featureCount; ++i, bufaddr += 2) {
            switch (bufaddr [0]) {
                case sp1f_multiple_drop_parseid:
                    session->multiple_drop_parseids = bufaddr [1];
                    break;
                case sp1f_space_option:
                    session->space_option = bufaddr [1];
                    break;
                case sp1f_variable_input:
                    session->variable_input = bufaddr [1];
                    break;
            }
        }
    }
    else {
        session->multiple_drop_parseids = false;
        session->variable_input = false;
    }
}

/*----------------------------------------*/

static void
i28_keep_packet (
    Script_SessionT   * session,
    void              * packet,
    int                 packetSize)
{
    session->send_packet = (tsp1_packet_ptr ) packet;
    session->send_packet->sp1_header.sp1h_varpart_size =
        packetSize - sizeof (tsp1_packet_header);
    session->is_connected = true;
}

/*----------------------------------------*/

int
i28sqlconnect (
    Script_SessionT   * session)
{
    /* ROUTINE_DBG_MSP00 ("i28sqlconnect"); */

    int                 connect_rc = 0;
    tsp00_Int4          packetSize;
    tsp00_ErrTextc      errtext;
    tsp01_CommErr       sp_rc;
    tsp01_Service       connectKind;
    void               *packetList[1];

    connectKind = srvUser_esp01;
    sp_rc = SqlAConnect (session->connectInfo.node, session->connectInfo.dbname,
        connectKind, 1, &session->reference, &packetSize,
        packetList, errtext);
    if (sp_rc != commErrOk_esp01) {
       i28setlasterr_rte (session, errtext, sp_rc) ;
    }
    else {
        tsp00_TermId      termid;

        ASSERT_DBG (!session->is_connected);
        i28_keep_packet (session, packetList [0], packetSize);
        sqltermid (termid);
        /*
         * try challenge/response
         */
        if (session->try_challenge_response) {
            connect_rc = i28_challenge_response (session, termid);
            if (connect_rc == missingKeywordIdentifiedC) {
                /*
                 * this server doesn't support challenge/response
                 * session has been aborted, reconnect is necessary
                 */
                session->try_challenge_response = false;
                sqlarelease (session->reference);
                sp_rc = SqlAConnect (session->connectInfo.node, session->connectInfo.dbname,
                    connectKind, 1, &session->reference, &packetSize,
                    packetList, errtext);
                if (sp_rc != commErrOk_esp01) {
                   i28setlasterr_rte (session, errtext, sp_rc) ;
                   return commErrNotOk_esp01;
                }
                else {
                    i28_keep_packet (session, packetList [0], packetSize);
                }
            }
        }
        /*
         * try basic authentication
         */
        if (!session->try_challenge_response) {
            i28connectcommand (session, termid);
            connect_rc = i28sql (session, NULL);
        }
        if (connect_rc != Script_db_ok) {
            /*
             * handle connect errors
             */
            sqlarelease (session->reference);
            session->is_connected = false;
        }
        else {
            /*
             * handle session info
             */
            i28_getConnectReply (session);
        }
        return connect_rc;
    }
    return commErrNotOk_esp01;
}

/*----------------------------------------*/

int
i28_connect (
    Script_SessionT   * session,
    tsp01_Service       connectKind)
{
    /* ROUTINE_DBG_MSP00 ("i28_connect"); */

    tsp00_Int4          packetSize;
    tsp00_ErrTextc      errtext;
    tsp01_CommErr       sp_rc;
    void              *packetList [1];

    sp_rc = SqlAConnect (session->connectInfo.node, session->connectInfo.dbname,
        connectKind, 1, &session->reference, &packetSize,
        packetList, errtext);

    if (sp_rc != commErrOk_esp01) {

      i28setlasterr_rte (session, errtext, sp_rc) ;

    } else {

      ASSERT_DBG (!session->is_connected);
      session->send_packet = (tsp1_packet_ptr ) packetList [0];
      session->send_packet->sp1_header.sp1h_varpart_size = packetSize - sizeof (tsp1_packet_header);
      session->is_connected = true;

    }
    return sp_rc;
} /* end i28_connect */

/*----------------------------------------*/

int
i28specialconnect (
    Script_SessionT   * session)
{
    /* ROUTINE_DBG_MSP00 ("i28specialconnect"); */

    tsp01_Service       connectKind;

    connectKind = srvUser_esp01;

    return i28_connect( session, connectKind);
} /* end i28specialconnect */

/*----------------------------------------*/

int
i28eventconnect (
    Script_SessionT   * session)
{
    /* ROUTINE_DBG_MSP00 ("i28eventconnect"); */

    tsp01_Service       connectKind;

    connectKind = srvEvent_esp01;

    return i28_connect( session, connectKind);
} /* end i28eventconnect */

/*----------------------------------------*/

int
i28connect (
    Script_SessionT   * session)
{
    /* ROUTINE_DBG_MSP00 ("i28connect"); */
    return i28sqlconnect ( session );
}

/*----------------------------------------*/

void
i28cleanup_session (
    Script_SessionT   * session)
{
    /* ROUTINE_DBG_MSP00 ("i28cleanup_session"); */
    sqlarelease (session->reference);
    if (session->lasterr.sqlresult.messagelistData == NULL) {
        free (session->lasterr.sqlresult.messagelistData);
        session->lasterr.sqlresult.messagelistData = NULL;
    }
    session->is_connected = false;
    i28clearpacketref (session);
}

/*----------------------------------------*/

void
i28release (
    Script_SessionT   * session,
    tsp00_Bool with_commit)
{
    /* ROUTINE_DBG_MSP00 ("i28release"); */
    int oldMode;

    oldMode = i28adabasmode (session);
    i28oldmode (session, oldMode);
    i28lasterr_off (session);
    if (with_commit)
        i28adbs_noinfo (session, "COMMIT WORK RELEASE", false);
    else
        i28adbs_noinfo (session, "ROLLBACK WORK RELEASE", false);
    i28cleanup_session (session);
}

/*----------------------------------------*/

void
i28specialrelease (
    Script_SessionT   * session)
{
    /* ROUTINE_DBG_MSP00 ("i28specialrelease"); */

    i28cleanup_session(session);
}

/*----------------------------------------*/

void
i28getFreePartInfo (
    Script_SessionT   * session,
    void ** dataPtr,
    int   * bufPos,
    int   * freeLen)
{
    /* ROUTINE_DBG_MSP00 ("i28getFreePartInfo"); */
    *dataPtr = PARTDATA (session->part) + PARTINFO (session->part, sp1p_buf_len);
    *bufPos = PARTINFO (session->part, sp1p_buf_len) + 1 /* is pascal index */;
    *freeLen = i28bytesfree (session);
}

/*----------------------------------------*/

static int
i28_handleCommError (
    Script_SessionT   * session,
    Script_SqlResultT * sqlresult,
    tsp01_CommErr  comm_error)
{
    /* ROUTINE_DBG_MSP00 ("i28_handleCommError"); */
    int result = Script_db_ok;

    if ((int) comm_error != commErrOk_esp01) {
        result = Script_db_not_accessible;
        /* session->is_connected = false; lower level routines try reconnect */
        if (sqlresult != NULL) {
            sqlresult->returnCode = result;
            M_WARNINGSET (sqlresult->warnings) = EMPTYSET;
            sqlresult->errorPos = 0;
            sqlresult->rowCount = 0;
            STRUCT_COPY (sqlresult->sqlstate, "I8888");
            sqlresult->sqlmsg[0] = '\0';
        }
    }
    return result;
}

/*----------------------------------------*/

static int
i28_handleSQLError (
    Script_SessionT   * session,
    Script_SqlResultT * sqlresult)
{
    /* ROUTINE_DBG_MSP00 ("i28_handleSQLError"); */
    int             result;
    tsp1_segment   *segment;
    int             msglen;

    i28resetpackets (session, false);
    segment = i28_lastsegment (session->rec_packet);
    result = RECINFO (segment, sp1r_returncode);
    /* to lasterr, only store errors: */
    if ((sqlresult != NULL) &&
        ((result != Script_db_ok) ||
            (sqlresult != &(session->lasterr.sqlresult)))) {
        sqlresult->returnCode = result;
        M_WARNINGSET(sqlresult->warnings) = M_WARNINGSET
            (RECINFO (segment, sp1r_extern_warning));
        sqlresult->errorPos = RECINFO (segment, sp1r_errorpos);
        if (result == Script_db_row_not_found) {
            sqlresult->rowCount = 0;
        }
        else {
            sqlresult->rowCount = i28resultcount (session);
        }
        STRUCT_COPY (sqlresult->sqlstate, RECINFO (segment, sp1r_sqlstate));
        if (result != Script_db_ok) {
            if (i28findpart (session, sp1pk_errortext)) {
                memcpy (sqlresult->sqlmsg, PARTDATA (session->part),
                    sizeof (sqlresult->sqlmsg) - 1);
                msglen = PARTINFO (session->part, sp1p_buf_len);
                sqlresult->sqlmsg[msglen] = '\0';
            }
            if (i28findpart (session, sp1pk_message_list)) {
                sqlresult->messagelistData = (char *) malloc (PARTINFO (session->part, sp1p_buf_len));
                if (sqlresult->messagelistData != NULL) {
                    sqlresult->messagelistDataLen = PARTINFO (session->part, sp1p_buf_len);
                    memcpy (sqlresult->messagelistData, PARTDATA (session->part),
                        sqlresult->messagelistDataLen);
                }
            }
        }
    }
    return result;
}

/*----------------------------------------*/

int
i28requestForPython (
    Script_SessionT   * session,
    Script_SqlResultT * sqlresult)
{
    /* ROUTINE_DBG_MSP00 ("i28requestForPython"); */
    tsp01_CommErr  comm_error;
    tsp00_Int2        result;

    if (sqlresult == NULL)
        sqlresult = &(session->lasterr.sqlresult);
    if (sqlresult->messagelistData != NULL) {
        free (sqlresult->messagelistData);
        sqlresult->messagelistData = NULL;
    }
    if (!session->is_connected) {
        result = Script_db_not_accessible;
        if (sqlresult != NULL) {
            sqlresult->returnCode = result;
            M_WARNINGSET (sqlresult->warnings) = EMPTYSET;
            sqlresult->errorPos = 0;
            sqlresult->rowCount = 0;
            STRUCT_COPY (sqlresult->sqlstate, "I8888");
            sqlresult->sqlmsg[0] = '\0';
        }
        return result;
    }
    session->rec_packet = NULL;
    i28closesend (session);
    comm_error = i28request (session);
    result = i28_handleCommError (session, sqlresult, comm_error);
    return result;
}

/*----------------------------------------*/

int
i28receiveForPython (
    Script_SessionT   * session,
    Script_SqlResultT * sqlresult)
{
    /* ROUTINE_DBG_MSP00 ("i28receiveForPython"); */
    tsp01_CommErr  comm_error;
    tsp00_Int2        result;

    if (sqlresult == NULL)
        sqlresult = &(session->lasterr.sqlresult);
    comm_error = i28receive (session);
    i28lasterr_on (session);
    result = i28_handleCommError (session, sqlresult, comm_error);
    if (result == Script_db_ok) {
        result = i28_handleSQLError (session, sqlresult);
    }
    return result;
}

/*----------------------------------------*/

