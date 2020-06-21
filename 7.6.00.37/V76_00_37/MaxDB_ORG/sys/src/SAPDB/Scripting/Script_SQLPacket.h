/* @lastChanged: "1998-05-08  8:28"
 *
 * @filename:   hin28.h
 * @purpose:    "Exports for SQL package handling"
 * @release:    7.1.0.0
 * @see:        ""
 *
 * @Copyright (c) 1997-2005 SAP AG"



    ========== licence begin  GPL
    Copyright (c) 1997-2005 SAP AG

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

#ifndef HIN28_H
#define HIN28_H

#include "gsp00.h"
#include "vsp001.h"

#define Script_db_ok                    0
#define Script_db_parse_again         (-8)
#define Script_db_aborted           (-102)
#define Script_db_illegal_rc        (-503)
#define Script_db_dbproc_failed     (-504)
#define Script_db_trigger_failed    (-505)
#define Script_db_dbfunc_failed     (-506)
#define Script_db_invalid_number    (-802)
#define Script_db_invalid_keyword  (-3008)
#define Script_db_missing_keyword  (-5015)
#define Script_db_not_unicode      (-8045)
#define Script_db_not_onebyte      (-8046)
#define Script_db_not_accessible   (-8888)
#define Script_db_dll_trigger_failed (-9406)
#define Script_db_adabasic_failed   (-507)
#define Script_db_row_not_found       100
#define Script_db_cmd_timeout         700
#define Script_db_module_not_found  16001

typedef struct Script_ConnectInfoT {
    char                 node [128];
    tsp00_DbNamec        dbname;
    tsp00_KnlIdentifierc username;
    tsp00_CryptPw        password;
    char                 sqlmode [9];
    int                  cachelimit;
    int                  timeout;
    int                  isolation;
} Script_ConnectInfoT;

typedef struct Script_SqlResultT {
    tsp00_Int4        returnCode;
    tsp00_WarningSet  warnings;
    tsp00_Int4        errorPos;
    tsp00_Int4        rowCount;
    tsp00_SqlState    sqlstate;
    tsp00_ErrText     sqlmsg;
    char            * messagelistData;
    int               messagelistDataLen;
} Script_SqlResultT;

typedef struct Script_SqlLasterrT {
    tsp01_CommErr         sp_rc;
    tsp00_ErrText         errtext;
    tsp00_KnlIdentifier   errname;
    Script_SqlResultT     sqlresult;
    tsp00_Bool            lasterr_on;
} Script_SqlLasterrT;

typedef struct Script_SessionT {
    Script_ConnectInfoT   connectInfo;
    tsp00_Int4            reference;
    tsp00_KnlIdentifier   usergroup;
    tsp00_Bool            is_connected;
    tsp00_Bool            inUse;
    tsp00_Bool            buildingCommand;
    int                   user_info;  /* set of name_info .. node_info */
    tsp00_C8              senderid;
    tsp00_SwapKind        swap_kind;
    tsp00_CodeType        code_type;
    tsp1_packet          *send_packet;
    tsp1_packet          *rec_packet;
    struct tsp1_segment  *segment;
    struct tsp1_part     *part;
    int                   sqlMode;
    void                 *more_data;
    Script_SqlLasterrT    lasterr;
    tsp00_Bool            switch_possible;
    tsp00_Bool            new_session;
    tsp00_Bool            as_utility;
    tsp00_Bool            multiple_drop_parseids;
    tsp00_Bool            space_option;
    tsp00_Bool            variable_input;
    tsp00_Bool            try_challenge_response;
} Script_SessionT;

typedef char    Script_SqlParseid[12];


#if defined (__cplusplus)
extern "C" {
#endif
/* ------------------------------- */
/* specification public functions */
/* ------------------------------- */
void i28errmsg (
     Script_SessionT         * pSession,
     tsp00_C256c             * errbuf);

void i28initsession (
    Script_SessionT         * pSession,
    tsp1_packet             * pPacket);

void i28pascalstring (
     char                    * target,
     int                       targetlen,
     const char              * source,
     int                       sourcelen );

Script_SessionT   * i28lasterr_on (
     Script_SessionT         * pSession );

int i28specialconnect (
      Script_SessionT        * pSession );

int i28eventconnect (
      Script_SessionT        * pSession );

void i28sqllasterr (
      Script_SessionT        * pSession,
      tsp00_ErrTextc         * errtext,
      tsp00_KnlIdentifierc   * errname,
      tsp00_Int2             * errcode,
      tsp00_Int2             * errpos );

void i28specialrelease (
      Script_SessionT        * pSession );

#define i28eventrelease i28specialrelease

int i28utility (
     Script_SessionT         * pSession,
     const char *                cmd );

void i28release (
      Script_SessionT        * pSession,
      tsp00_Bool               bWithCommit );

int i28sqlconnect (
      Script_SessionT        * pSession );

int i28fetchnice (
     Script_SessionT         * pSession,
     char                    * pResult,
     long                      nResultLen);

int i28selectnice (
     Script_SessionT         * pSession,
     const char *                cmd,
     char                    * pResult,
     long                      nResultLen,
     bool                      bOneRowResult,
     bool                      bDesc,
	 bool                      bDataOnly);

int i28selectinfo (
     Script_SessionT         * pSession,
     const char *                cmd,
     char                    * pResult,
     char                      cFieldSep );

int i28fetch (
     Script_SessionT         * pSession,
     char                    * pResult,
     long                      nResultLen,
     char                      cFieldSep,
     char                      cStringDel,
     char                      cRecSep );

int i28select (
     Script_SessionT         * pSession,
     const char *                cmd,
     char                    * pResult,
     long                      nResultLen,
     char                      cFieldSep,
     char                      cStringDel,
     char                      cRecSep );

int i28record (
     Script_SessionT         * pSession,
     const char *                cmd);

int i28nextrecord (
     Script_SessionT         * pSession);

int i28adbs (
     Script_SessionT         * pSession,
     const char *                cmd );

int i28adbs_noinfo (
    Script_SessionT   * session,
    const char * cmd,
    bool withCommit);

int i28adbsf (
     Script_SessionT         * pSession,
     const char *                cmd,
     ...);

int i28replyavailable (
     Script_SessionT         * pSession );

void i28cancel (
     Script_SessionT         * pSession );

int i28fetchcmd (
     Script_SessionT         * pSession,
     const char *              cmd,
     int                     vars);

int
i28switch (
    Script_SessionT          * session,
    const char *               trace,
    const char *               test);

int
i28switchlimit (
    Script_SessionT          * session,
    const char *               trace,
    const char *               test,
    const char *               start,
    const char *               stop,
    int                      limit);

int
i28buflimit (
    Script_SessionT          * session,
    int                      limit);

int
i28minbuf (
    Script_SessionT          * session);

int
i28maxbuf (
    Script_SessionT          * session);

void
i28initparse (
    Script_SessionT   * session);

void
i28initadbs (
    Script_SessionT   * session);

void
i28initprocreply (
    Script_SessionT   * session);

void
i28seterrorinfo  (
    Script_SessionT   * session,
    int                 errorCode,
    const char *        errorMessage,
    int                 messageLen);

void
i28pcmd (
    Script_SessionT   * session,
    const char * cmd,
    int len);

void
i28pcmdf (
    Script_SessionT   * session,
    const char * cmd,
    ...);

int
i28sql (
    Script_SessionT   * session,
    Script_SqlResultT * sqlresult);

void
i28gparseid (
    Script_SessionT   * session,
    Script_SqlParseid pid);

tsp00_Int4
i28resultcount (
    Script_SessionT   * session);

int
i28paramcount (
    Script_SessionT   * session);

tsp1_param_info *
i28gparaminfo (
    Script_SessionT   * session,
    int i);

void
i28initexecute (
    Script_SessionT   * session,
    Script_SqlParseid pid);

void
i28initspecial (
    Script_SessionT   * session,
    tsp1_cmd_mess_type_Param messType);

int
i28droppid (
    Script_SessionT   * session,
    Script_SqlParseid pid);

char /* byte */           *
i28argaddr (
    Script_SessionT   * session,
    tsp00_Int4 bufpos);

tsp00_Bool
i28findpart (
    Script_SessionT   * session,
    tsp1_part_kind_Param requiredKind);

int
i28parg (
    Script_SessionT   * session,
    const char /* byte */ * arg,
    int varlen,  /* in bytes */
    int sqllen,  /* in bytes */
    tsp00_Int4 bufpos,
    char defByte);

void
i28addDataField (
    Script_SessionT   * session,
    const void        * data,
    int                 datalen);

int
i28putUCS2 (
    Script_SessionT   * session,
    const char /* byte */ * arg,
    const tsp77encoding * srcEncoding,
    int varlen,  /* in bytes */
    int sqllen,  /* in bytes */
    tsp00_Int4 bufpos);

void
i28pnull (
    Script_SessionT   * session,
    int sqllen,
    tsp00_Int4 bufpos);

void
i28addNullField (
    Script_SessionT   * session);

tsp1_part      *
i28newpart (
    Script_SessionT   * session,
    tsp1_part_kind_Enum partKind);

tsp1_part      *
i28newDataPart (
    Script_SessionT   * session,
    int valueCount);

tsp00_Bool
i28gresulttablename (
    Script_SessionT   * session,
    int              * len,
    char             * resulttablename);

tsp00_Int4
i28partlen (
    Script_SessionT   * session);

void
i28addpartdata (
    Script_SessionT   * session,
    const void *data,
    int len);

char           *
i28getpart (
    Script_SessionT   * session);

char           *
i28argsbuf (
    Script_SessionT   * session);

tsp00_Int4
i28nameslen (
    Script_SessionT   * session);

const char *
i28colnames (
    Script_SessionT   * session);

void
i28extendpart (
    Script_SessionT   * session,
    int lendiff);

void
i28getFreePartInfo (
    Script_SessionT   * session,
    void ** dataptr,
    int   * bufPos,
    int   * freeLen);

int
i28requestForPython (
    Script_SessionT   * session,
    Script_SqlResultT * sqlresult);
int
i28receiveForPython (
    Script_SessionT   * session,
    Script_SqlResultT * sqlresult);

int i28connect (
    Script_SessionT   * session);

void
i28resetpackets (
    Script_SessionT   * session,
    tsp00_Bool forCommand);

void
i28closesend (
    Script_SessionT   * session);

tsp00_Int4
i28packetlen (
    Script_SessionT   * session);

#if defined (__cplusplus)
}
#endif

#endif
