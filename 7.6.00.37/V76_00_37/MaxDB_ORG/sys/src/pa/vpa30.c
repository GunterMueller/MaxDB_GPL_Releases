/*



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

#include "vpa30.h"

#include "vpa20Desc.h"
#include "vpa40DBC.h"
#include "vpa60Stmt.h"
#include "vpa70SQLDA.h"
#include "vpa06.h"
#include "vpa09.h"

#include "vpr03Packet.h" /* pr03PacketGetKernelParameters */
#include "vpr01Con.h"
#include "vpr10c.h"
#include "gpr03.h"       /* Call_statement_proceduren */
#include "gpr11.h"       /* SQLDB-Auftrags-Schnittstelle */
#include "vpr11c.h"

/* EXTERNAL FUNCTIONS */
extern s26find_part (tsp1_segment * segm_ptr, char part_kind, tsp1_part ** part_ptr);
extern tsp00_Int4 s26partlen (tsp1_part * part_ptr);
VOID apustoup( UCHAR FAR * string_ptr );
tsp00_Int2 pa01UtilityConnect(void);
extern void  p10setkamode (sqlcatype* , sqldatype*);

/* PROTOTYPES */
char* pa42GetURI (UCHAR *servernode, sqldbname serverdb, char SSL);

void p08runtimeerror (sqlcatype*, sqlcxatype*, int);

void pa08UnqoutedStringtoUpper (tpr05_String *);
void pa08rmspace_tpr05String   (tpr05_String *);


BOOLEAN pa30GetKernelParameters( tsp1_segment *segm_ptr,
                                 UWORD         fOption,
                                 VOID         *pKernelParam);

tsp00_Int4 pa30MakeConnStr( UCHAR *str,
                            tpa40DBC *dbc_ptr,
                            sqlcatype *sqlcap);

UCHAR *pa30SQLModeStr( SWORD sqlmode );

VOID pa30mkfetchstr( tpr05_String  *stmt_str,
                     tpr05_String  *cursor_name,
                     UWORD   fetchtype);

VOID pa30mkansifetchstr(UCHAR *stmt_str,
                        UCHAR *cursor_name,
                        UWORD  fetchtype, UDWORD pos);

BOOLEAN FAR pa30GetTableNamePart( tsp1_segment *segm_ptr,
                                  tpr05_String *string);

UWORD pa30GetFunctionCode( sqlgaentry *gaent );

API_RETCODE  pa30SyntaxCheck ( tpa41ESQBlock *api_cb_ptr,
                               tpa40DBC      *api_dbc_ptr,
                               tpa60Stmt     *api_stmt_ptr,
                               tpr05_String  *SqlCmd);

VOID pa30SenderID( UCHAR *szSenderId, UWORD component );

void pa30bpcruntime( tpa40DBC  *dbc_block_ptr,
                     tpa60Stmt *stmt_block_ptr );

void pa30apcruntime( tpa40DBC  *dbc_block_ptr,
                     tpa60Stmt  FAR * stmt_block_ptr );

void pa30OraResultCount( tpa41ESQBlock *api_cb_ptr,
                         tpa60Stmt     *api_stmt_ptr );


SWORD apereconn( tpa41ESQBlock *api_cb_ptr, 
                 tpa40DBC FAR  *api_dbc_ptr);

API_RETCODE pa30commit( tpa41ESQBlock *api_cb_ptr,
                        tpa40DBC      *api_dbc_ptr);

API_RETCODE pa30SetAutocommitFlag(  tpa40DBC *dbc_block_ptr );

API_RETCODE pa30ResizeSQLDA( sqldatype **sqlda, UWORD arg_count );

API_RETCODE pa30ReplaceFirstCharacterWithBlank (tpr05_String *str);

/* CONSTANTS */
UCHAR FAR *PA30_CMD_INTO = { (UCHAR*) "\"" };/*"\" INTO"*/
UCHAR FAR *PA30_CMD_SUBTRANS_BEGIN = { (UCHAR*) "SUBTRANS BEGIN" };
UCHAR FAR *PA30_CMD_SUBTRANS_END = { (UCHAR*) "SUBTRANS END" };
UCHAR FAR *PA30_CMD_SUBTRANS_ROLLBACK = { (UCHAR*) "SUBTRANS ROLLBACK" };

/* aperetg() - Returncodeget from sqlca function */
VOID aperetg  (tpa41ESQBlock FAR * api_cb_ptr)
{
    SWORD errlen;
    SWORD i;
    API_TRACE(API_TR_ENTRY,"aperetg",0);
    API_TRACE(API_TR_SDWORD,"sqlca.sqlcode",&api_cb_ptr->sqlca.sqlcode);
    api_cb_ptr -> esq_sql_code = api_cb_ptr->sqlca.sqlcode;
    if (api_cb_ptr->sqlca.sqlcode != PA_OK) {
        API_TRACE(API_TR_SWORD,"sqlca.sqlerrml",&api_cb_ptr->sqlca.sqlerrml);
        if (api_cb_ptr->sqlca.sqlerrd[5] > 0) {
            API_SPRINTF( (char*) api_cb_ptr->sql_err_text,
                         "POS(%d) ",
                         api_cb_ptr->sqlca.sqlerrd[5] );
            api_cb_ptr->sqlca.sqlerrd[5] = 0;
        }
        else
            api_cb_ptr->sql_err_text[0] = '\0';
        errlen = api_cb_ptr->sqlca.sqlerrml;
        if (errlen > (SWORD)(PA_ERROR_TEXT_LEN
                             -API_STRLEN(api_cb_ptr->sql_err_text)) ) {
            errlen = (SWORD)(PA_ERROR_TEXT_LEN
                             -API_STRLEN(api_cb_ptr->sql_err_text));
        }	
        if (errlen >= 0 ) {
            for (i = (SWORD) (errlen-1); i > 0; i--) {
                if (api_cb_ptr->sqlca.sqlerrmc[i] != ' ') {
                    errlen = (SWORD) (i + 1);
                    break;
                }	
            }
            API_STRNCAT( api_cb_ptr->sql_err_text,
                         api_cb_ptr->sqlca.sqlerrmc,
                         errlen);
        }
        api_cb_ptr->number_rows_modified = 0;
    }
    else {
        api_cb_ptr->sql_err_text[0] = 0;
        api_cb_ptr->number_rows_modified = api_cb_ptr->sqlca.sqlerrd [ 2 ];
    }
    API_TRACE(API_TR_EXIT,"aperetg",0);
} /* aperetg */


/* apeconn() - API ESQ function, execute SQL CONNECT */

API_RETCODE apeconn (  tpa41ESQBlock *api_cb_ptr,
                       UCHAR         *serverdb_ptr,   SWORD serverdb_len,
                       tpr05_String   *useridStr,
                       tpr05_String   *authStr,
/*                       UCHAR         *userid_ptr,     SWORD userid_len, */
/*                       UCHAR         *authstr_ptr,    SWORD authstr_len,*/
                       tpa40DBC      *api_dbc_ptr)
{
    tsp00_Int4 stmt_len;
    API_RETCODE retcode;
    UCHAR local_str[256];
    tsp00_DbName serverdb;
    tsp00_NodeId servernode_dummy;

    retcode = API_OK;

    API_TRACE( API_TR_ENTRY, "apeconn", 0);
    API_TRACE( API_TR_PTR, "api_cb_ptr", &api_cb_ptr);
    API_TRACE_LEN( API_TR_ODBC_STRING, "serverdb_ptr",
                   serverdb_ptr, serverdb_len);
    API_TRACE(API_TR_SWORD, "serverdb_len", &serverdb_len);
    API_TRACE( API_TR_ODBC_STRING, "api_dbc_ptr->servernode",
               api_dbc_ptr->servernode );
    API_TRACE_LEN(API_TR_TPR05STRING, "useridStr", useridStr, useridStr->cbLen);
    API_TRACE(API_TR_SWORD, "useridStr_len", &useridStr->cbLen);
    API_TRACE_LEN(API_TR_ODBC_STRING, "authstr_ptr", "?", 1);
    API_TRACE(API_TR_MSG, "authstr_len=?", 0);
	        
    retcode = apmwtos(serverdb_ptr, (SWORD) serverdb_len,
                      serverdb, (SWORD) sizeof(serverdb) );

    pa08UnqoutedStringtoUpper (useridStr);
    pa08rmspace_tpr05String (useridStr);
    API_STRCPY( servernode_dummy, "" );

    {
#ifndef SAPDB_FAST
        SWORD trace_status=api_trace_status;
        api_trace_status = 2;      
#endif
        pa08UnqoutedStringtoUpper (authStr);
#ifndef SAPDB_FAST
        api_trace_status=trace_status;
#endif
    }

    if (retcode == API_OK) {
        stmt_len = pa30MakeConnStr( local_str,
                                    api_dbc_ptr,
                                    &api_cb_ptr->sqlca );
        API_TRACE(API_TR_SDWORD,"stmt_len",&stmt_len);
        api_cb_ptr->sqlca.sqldbmode = CPR_KIND_EMPTY;
        API_MEMSET(api_cb_ptr->sqlca.sqlresn, bsp_c1,
                   sizeof(api_cb_ptr->sqlca.sqlresn));
        if (api_dbc_ptr->utility.component != COMP_ODBC)
            pa30SenderID( (UCHAR*) api_cb_ptr->sqlca.sqlrap->ratermid,
                          api_dbc_ptr->utility.component);

        {
            tpr05_String ConStr;
            ConStr.allocationType = constant_epr05;
            pr05IfCom_String_InitString(&ConStr, local_str, stmt_len,
                                        sp77encodingAscii, constant_epr05);

            /* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1133247
               no SSL by shared memory communication */
            if (api_dbc_ptr->encrypt == SQL_ATTR_ENCRYPT_SSL  &&  *api_dbc_ptr->servernode != 0) {
              char *p;
              p = pa42GetURI (api_dbc_ptr->servernode, serverdb, api_dbc_ptr->encrypt);
              if (p == NULL)
                return API_NOT_OK;
              else {
                api_dbc_ptr->ConDesc =       /* connect with SSL-connect string */
                  pr10Connect( api_dbc_ptr, &ConStr, (UCHAR*) p, serverdb, 
                               useridStr, authStr);
                free (p);
              }
            } else {
              api_dbc_ptr->ConDesc =        /* usual connect */
                pr10Connect( api_dbc_ptr, &ConStr, api_dbc_ptr->servernode,
                             serverdb, useridStr, authStr);
            }
        }
        if (api_cb_ptr->sqlca.sqlcode == 0) {
#ifdef _UNICODE_ODBC
          /* the ASCII ODBC driver uses always ASCII packets for communication.
             Only the Unicode driver looks at the kernel parameter and uses
             than always (!) Unicode packets, if kernel is Unicode (cmp vpr10c.c). 
             http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1122580 */
            pr03PacketGetKernelParameters( (tsp1_segment*)
                                           api_cb_ptr->sqlca.sqlrap->rasegptr,
                                           UnicodeCatalog_epr03,
                                           &api_dbc_ptr->unicode_catalog);
#endif
            if (!pr03PacketGetKernelParameters( (tsp1_segment*)
                                                api_cb_ptr->sqlca.sqlrap
                                                ->rasegptr,
                                                KernelVersionString_epr03,
                                          api_dbc_ptr->kernelVersionString)) {
                /* we dont get a kernel version string because the kernel
                 * is older than 7.2 */
                api_dbc_ptr->kernelVersionString[0] = '\0';
            }; /* if */
            API_TRACE( API_TR_UWORD, "unicode_catalog",
                       &api_dbc_ptr->unicode_catalog);
        }
        pa08rmspace(serverdb, (int) API_STRLEN (serverdb));
        /*        pa08rmspace(userid, API_USERID_MAX_LEN); */
        aperetg (api_cb_ptr);	
        if (api_cb_ptr->sqlca.sqlcode == PA_OK) {
            if (pr05IfCom_String_strcpy (api_dbc_ptr->userid, useridStr) != ok_epr05)
                retcode = API_NOT_OK;
            if (pr05IfCom_String_strcpy (api_dbc_ptr->passw, authStr) != ok_epr05)
                retcode = API_NOT_OK;

            API_MEMCPY( api_dbc_ptr->serverdb,
                        serverdb,
                        API_STRLEN (serverdb) );
            if (api_dbc_ptr->constmtopt.sqlmode != CPR_KIND_INTERNAL) {
                API_TRACE( API_TR_SWORD,
                           "sqldbmode",
                           &api_cb_ptr->sqlca.sqldbmode );
                api_cb_ptr->sqlca.sqlrap->radbmode = CPR_KIND_INTERNAL;
            }
        }; /* else */
    }
    API_TRACE(API_TR_EXIT,"apeconn",0);
    API_TRACE(API_TR_API_ESQ_BLOCK, "tpa41ESQBlock",api_cb_ptr);
    API_TRACE(API_TR_API_RETCODE,"retcode",&retcode);

    return(retcode);
} /* apeconn */

#define SQLMODE_ORACLE "SQLMODE ORACLE"
#define SQLMODE_SAPR3  "SQLMODE SAPR3"
#define SQLMODE_DB2    "SQLMODE DB2"
#define SQLMODE_ANSI   "SQLMODE ANSI"

UCHAR  *pa30SQLModeStr(SWORD sqlmode)
{
    switch(sqlmode) {
    case (CPR_KIND_ORACLE) : {
        return (UCHAR*) SQLMODE_ORACLE;
        break;
    }	
    case (CPR_KIND_SAPR3) : {
        return (UCHAR*) SQLMODE_SAPR3;
        break;
    }
    case (CPR_KIND_DB2) : {
        return (UCHAR*) SQLMODE_DB2;
     break;
    }
    case (CPR_KIND_ANSI) : {
        return (UCHAR*) SQLMODE_ANSI;
        break;
    }	
    case (CPR_KIND_INTERNAL) : {
        return (UCHAR*) "";
        break;
    default:
        return (UCHAR*) "";
        break;
    }	   
    }; /* switch */
} /* pa30SQLModeStr */


VOID apedisc ( tpa41ESQBlock FAR * api_cb_ptr, tpa40DBC FAR * api_dbc_ptr)
{ 
    API_TRACE(API_TR_ENTRY,"apedisc",0);
    API_TRACE(API_TR_PTR,"api_cb_ptr",&api_cb_ptr);
    api_cb_ptr->sqlca.sqlcode = PA_OK;
    /* ADIS 1000651 
       if (!api_dbc_ptr->autocommit) {
       aperoll (api_cb_ptr, api_dbc_ptr);
       }
       */
    if( api_cb_ptr->sqlca.sqlcode == PA_OK ) {
      /* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1139893
         disconnect only if ConDesc is still available; after pr10Disconnect
         a disconnect has to be regarded as successfull */
      if (api_dbc_ptr->ConDesc != NULL) {
        pr10Disconnect(api_dbc_ptr);
        aperetg (api_cb_ptr);
        api_cb_ptr->esq_sql_code = 0;
      }
    }
    API_TRACE(API_TR_EXIT,"apedisc",0);
    API_TRACE(API_TR_API_ESQ_BLOCK, "tpa41ESQBlock",api_cb_ptr);

    return;
} /* apedisc */


tsp01_CommErr_Enum SqlACancel ( tsp00_Int4      reference ,
                                char           *szSapRouterString,
                                tsp00_ErrTextc  szErrText );

#ifndef WIN32
/* dummy func */
tsp01_CommErr_Enum SqlACancel ( tsp00_Int4      reference ,
                                char           *szSapRouterString,
                                tsp00_ErrTextc  szErrText )
{
    return commErrNotOk_esp01;
}
#endif

VOID apecancel ( tpa41ESQBlock *api_cb_ptr, tpa40DBC *api_dbc_ptr)
{ 
    sqlcatype *sqlcap;
    sqlratype *sqlrap;
    
    API_TRACE(API_TR_ENTRY, "apecancel",0);  
    API_TRACE(API_TR_PTR,"api_cb_ptr",&api_cb_ptr);
    
    api_cb_ptr->sqlca.sqlcode = PA_OK;
    sqlcap = &api_cb_ptr->sqlca;
    sqlrap = sqlcap->sqlrap;
    sqlrap->raactsession = api_dbc_ptr->dbno;
    if( api_cb_ptr->sqlca.sqlcode == PA_OK ) {
        if ( api_dbc_ptr -> cancelRouterStr == NULL ) {
            /* cancel over "normal" connection */
            p03cancel( &api_cb_ptr->sqlca, api_dbc_ptr->ConDesc->ga);
            aperetg (api_cb_ptr);
        } else {
            /* cancel over NI connection. We need a fresh SAP Router String. */
            tsp01_CommErr_Enum   commRet = commErrOk_esp01;
            tsp00_ErrTextc       szErrText;
            
            commRet = SqlACancel( api_dbc_ptr->ConDesc->ga->gareference,
                                  api_dbc_ptr -> cancelRouterStr,
                                  szErrText );
            /* free router string because it may be used only once */
            apdfree( api_dbc_ptr -> cancelRouterStr );
            api_dbc_ptr -> cancelRouterStr = NULL;
            /* error handling */
            if (commRet != commErrOk_esp01) {
                api_dbc_ptr -> esqblk.esq_sql_code = -102;
                API_STRCPY( api_dbc_ptr -> esqblk.sql_err_text,
                            szErrText );
            };
        }; /* else */
    }
    API_TRACE(API_TR_EXIT,"apecancel",0);
    
    return;
} /* apecancel */


/* apecomm() - API ESQ function, execute SQL COMMIT */

VOID apecomm (tpa41ESQBlock FAR * api_cb_ptr, tpa40DBC FAR * api_dbc_ptr)
{
    UCHAR      *stmt_ptr;
    tsp00_Int4  stmt_len;

    API_TRACE(API_TR_ENTRY,"apecomm",0);

    stmt_ptr = (UCHAR*) SYS_CMD_COMMIT_WORK;
    stmt_len = (tsp00_Int4) API_STRLEN(stmt_ptr);

    API_TRACE(API_TR_STRING,"stmt_ptr", stmt_ptr);
    API_TRACE(API_TR_SDWORD,"stmt_len",&stmt_len);

    p10statement (&api_cb_ptr->sqlca, api_dbc_ptr->ConDesc->ga, &api_dbc_ptr->dbno,
                  &stmt_len, (char*) stmt_ptr, sp77encodingAscii);
    apereconn(api_cb_ptr, api_dbc_ptr);
    aperetg (api_cb_ptr);

    API_TRACE(API_TR_EXIT,"apecomm",0);

    return;
} /* apecomm */


/* aperoll() - API ESQ function, execute SQL ROLLBACK */

VOID aperoll ( tpa41ESQBlock FAR * api_cb_ptr, tpa40DBC FAR * api_dbc_ptr)
{
    tsp00_Int4  stmt_len;
    
    API_TRACE(API_TR_ENTRY,"aperoll",0);
    API_TRACE(API_TR_PTR,"api_cb_ptr",&api_cb_ptr);

    if (!pa01UtilityConnect()) {
        stmt_len = sizeof(SYS_CMD_ROLLBACK_WORK)-1;
        p10statement( &api_cb_ptr->sqlca,
		      api_dbc_ptr->ConDesc->ga, 
                      &api_dbc_ptr->dbno,
                      &stmt_len,
                      SYS_CMD_ROLLBACK_WORK, sp77encodingAscii );
        apereconn(api_cb_ptr, api_dbc_ptr);
        aperetg (api_cb_ptr);
    }
    API_TRACE(API_TR_EXIT,"aperoll",0);
    API_TRACE(API_TR_API_ESQ_BLOCK, "tpa41ESQBlock",api_cb_ptr);

    return;
} /* aperoll */


/* apeprep() - API ESQ function, execute SQL PREPARE */

API_RETCODE  apeprep ( tpa41ESQBlock *api_cb_ptr,
                       tpr05_String  *SqlStmt,
                       UCHAR         *parsid_ptr,
                       tpa40DBC      *api_dbc_ptr,
                       tpa60Stmt     *api_stmt_ptr )
{
    sqldatype *sqlda_ptr;
    API_RETCODE retcode=API_OK;
    SWORD try3 = 0;
    tsp00_Int4 localStmtLen = (tsp00_Int4) SqlStmt->cbLen;
    
    API_TRACE(API_TR_ENTRY,"apeprep",0);
    API_TRACE(API_TR_SWORD,"dbno",&api_dbc_ptr->dbno);
    API_TRACE(API_TR_UWORD,"special",&api_stmt_ptr->dbc_special.special);
    API_TRACE_LEN(API_TR_TPR05STRING,"stmt_ptr", SqlStmt, SqlStmt->cbLen);
    if (pa09IsCanceled(0)) {
        p08runtimeerror( &api_cb_ptr->sqlca,
                         api_cb_ptr->sqlca.sqlcxap,
                         cpr_r_sql_cancelled );
        aperetg (api_cb_ptr);
    }
    else {
        if (api_stmt_ptr->dbc_special.special != API_SPEC_NOT) {
            api_cb_ptr->sqlca.sqlrap->rakamode = CPR_KIND_INTERNAL;
            api_cb_ptr->sqlca.sqlrap->rasqlansi = CPR_KIND_EMPTY;
        }
      
        sqlda_ptr = api_stmt_ptr->input_sqlda_ptr;
        api_cb_ptr->sqlca.sqlresn[0] = '\0';
        if (sqlda_ptr != NULL) {
            pa30bpcruntime(api_dbc_ptr, api_stmt_ptr);

            API_TRACE(API_TR_UDWORD,"sqlmax",&sqlda_ptr -> sqlmax);
            API_TRACE(API_TR_UWORD,"sqln",&sqlda_ptr -> sqln);
            API_TRACE(API_TR_UWORD,"sqld",&sqlda_ptr -> sqld);
            do {
                try3++;
                {
                  p10parse ( &api_cb_ptr->sqlca,
                             api_dbc_ptr->ConDesc->ga,
                             &api_dbc_ptr->dbno,
                             &localStmtLen,
                             (char*) SqlStmt->rawString,
                             SqlStmt->encodingType,
                             sqlda_ptr,
                             parsid_ptr );
                }
                if (api_cb_ptr->sqlca.sqlcode == PA_OK) {
                    tsp1_part *part = NULL;
                    UWORD      arg_count = 0;

                    /* get arg_count from part. If sqlda is too small
                     * resize it. If it is not resized before
                     * p11shortfieldparameterput the statement is parsed
                     * again (degrades performance) PTS 1106783 */
                    p03find_part( api_cb_ptr->sqlca.sqlrap,
                                  sp1pk_shortinfo,
                                  &part);
                    if (part != NULL) /* PTS 1110533 */
                      arg_count = (UWORD) part->sp1p_part_header.sp1p_arg_count;
                    if (sqlda_ptr->sqlmax < arg_count) {
                        pa30ResizeSQLDA( &sqlda_ptr, arg_count );
                        api_stmt_ptr->input_sqlda_ptr = sqlda_ptr;
                        api_stmt_ptr->input_sqlda_handle = sqlda_ptr;
                    };
                    p11shortfieldparameterput( &api_cb_ptr->sqlca,
                                               api_dbc_ptr->ConDesc->ga,
                                               sqlda_ptr,
                                               API_FALSE );
                }; /* if */
            }
            while ( apereconn(api_cb_ptr, api_dbc_ptr)
                    && try3 < PA_MAX_RECONNECT );
            API_TRACE(API_TR_UDWORD,"sqlmax",&sqlda_ptr -> sqlmax);
            API_TRACE(API_TR_UWORD,"sqln",&sqlda_ptr -> sqln);
            API_TRACE(API_TR_UWORD,"sqld",&sqlda_ptr -> sqld);

            /* if reconnect failed (e.g. kernel was offline)
               http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1128976
               http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1130529
               http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1131374 */
            if (api_cb_ptr->sqlca.sqlcode == PA_ERR_CONNECTION_DOWN  ||
                api_cb_ptr->sqlca.sqlcode == PA_ERR_SERVERDB_NOT_AVAILABLE) {
              retcode = API_NOT_OK;
              goto exit;
            }

            pa30apcruntime(api_dbc_ptr, api_stmt_ptr);

            aperetg (api_cb_ptr);
     
            /* get cursor_name (from named select) */
            if (API_MEMCMP( api_cb_ptr->sqlca.sqlresn,
                            bsp_c64,
                            sizeof(api_cb_ptr->sqlca.sqlresn))) {
              pr05IfCom_String_ConvertP (api_stmt_ptr->cursor_name,
                                         (char *) api_cb_ptr->sqlca.sqlresn,
                                         sizeof (api_cb_ptr->sqlca.sqlresn),
                                         sp77encodingUTF8);
              API_TRACE_LEN( API_TR_TPR05STRING, "cursor_name",
                             api_stmt_ptr->cursor_name, 
                             api_stmt_ptr->cursor_name->cbLen);
            }
     
            api_stmt_ptr->stmt_function_code = 
                pa30GetFunctionCode( api_dbc_ptr->ConDesc->ga);
     
            pr05IfCom_String_ClearString (api_stmt_ptr->table_name);
            if (parsid_ptr[CPR_PARS_IDIX] == csp1_p_select_for_update_found || 
                parsid_ptr[CPR_PARS_IDIX] == csp1_p_reuse_update_sel_found) {
                if(pa30GetTableNamePart( (tsp1_segment*)
                                         api_cb_ptr->sqlca.sqlrap->rasegptr,
                                         api_stmt_ptr->table_name) )  {
                    api_stmt_ptr->stmt_type |= API_ATTRIB_SELECT_FOR_UPDATE;
                    api_stmt_ptr->stmt_type &= -API_ATTRIB_MFETCH_ALLOWED-1;
                    API_TRACE( API_TR_UWORD,
                               "stmt_type",
                               &api_stmt_ptr->stmt_type );
                }
            }   

            retcode = API_OK;
        }
        else 
            retcode = API_NOT_OK;
  
        if (api_stmt_ptr->dbc_special.special != API_SPEC_NOT) {
            api_cb_ptr->sqlca.sqlrap->rakamode =
                api_stmt_ptr->stmtopt.sqlmode;
            api_cb_ptr->sqlca.sqlrap->rasqlansi =
                api_stmt_ptr->stmtopt.sqlmode;
        }
    }
 exit:
    API_TRACE(API_TR_EXIT,"apeprep",0);
    API_TRACE(API_TR_PARSID, "parsid", parsid_ptr);
    API_TRACE(API_TR_API_RETCODE,"retcode",&retcode);

    return (retcode);
} /* apeprep */


void pa30varcp(sqldatype *da1, sqldatype *da2)
{
    UWORD i;
    for(i=0; i < da2->sqln; i++) {
        API_MEMCPY(&da1->sqlvar[i], &da2->sqlvar[i], sizeof(da2->sqlvar[0]));
    }	
    da1->sqln = da2->sqln;
} /* pa30varcp */


API_RETCODE pa30varcheck(sqldatype *da1, sqldatype *da2)
{
    UWORD i;
    for(i=0; i < da2->sqln; i++)
        if (da2->sqlvar[i].coltype != da1->sqlvar[i].coltype)
            return(API_NOT_OK);
    return(API_OK);
} /* pa30varcheck */


/* apeexec() - API ESQ function, execute SQL EXECUTE */

API_RETCODE apeexec ( tpa41ESQBlock *api_cb_ptr,
                      tpr05_String  *SqlStmt,
                      UCHAR         *parsid_ptr,
                      tpa40DBC      *api_dbc_ptr,
                      tpa60Stmt     *api_stmt_ptr )
{
    sqldatype *sqlda_ptr;
    API_RETCODE retcode=API_OK;
    SWORD try3 = 0;
    tsp00_Int4 localStmtLen = (tsp00_Int4) SqlStmt->cbLen;
  
    API_TRACE(API_TR_ENTRY,"apeexec",0);
    API_TRACE(API_TR_SWORD,"dbno",&api_dbc_ptr->dbno);
    API_TRACE(API_TR_UWORD,"special",&api_stmt_ptr->dbc_special.special);
    API_TRACE(API_TR_PARSID, "parsid", parsid_ptr);
    API_TRACE_LEN(API_TR_TPR05STRING,"stmt_ptr", SqlStmt, SqlStmt->cbLen);

    if (pa09IsCanceled(0)) {
        p08runtimeerror ( &api_cb_ptr->sqlca,
                          api_cb_ptr->sqlca.sqlcxap,
                          cpr_r_sql_cancelled );
        aperetg (api_cb_ptr);
    }
    else {
        if (api_stmt_ptr->dbc_special.special != API_SPEC_NOT) {
            api_cb_ptr->sqlca.sqlrap->rakamode = CPR_KIND_INTERNAL;
            api_cb_ptr->sqlca.sqlrap->rasqlansi = CPR_KIND_EMPTY;
        }          
        sqlda_ptr = api_stmt_ptr->input_sqlda_ptr;
        if (sqlda_ptr != NULL) {
            API_TRACE(API_TR_SWORD,"sqld",&sqlda_ptr->sqld);     
            pa30bpcruntime(api_dbc_ptr, api_stmt_ptr);
            for (;;) {
                try3++;	
                API_TRACE( API_TR_UWORD,
                           "stmt_function_code",
                           &api_stmt_ptr->stmt_function_code);
                if (api_stmt_ptr->stmt_function_code == csp1_select_fc  ||
                    api_stmt_ptr->stmt_function_code == csp1_cursor_pl_sql_execute_fc) {
                  API_MEMSET( api_cb_ptr->sqlca.sqlresn, '\0',
                              sizeof(api_cb_ptr->sqlca.sqlresn));
                  sp77sprintfUnicode (sp77encodingUTF8,
                                      api_cb_ptr->sqlca.sqlresn,
                                      sizeof (api_cb_ptr->sqlca.sqlresn),
                                      "%'=.*S",
                                      api_stmt_ptr->cursor_name->encodingType,
                                      api_stmt_ptr->cursor_name->cbLen,
                                      api_stmt_ptr->cursor_name->rawString);
                  API_TRACE_LEN( API_TR_TPR05STRING, "cursor_name",
                                 api_stmt_ptr->cursor_name, 
                                 api_stmt_ptr->cursor_name->cbLen);
                }
                else
                    API_MEMSET( api_cb_ptr->sqlca.sqlresn,
                                ' ',
                                sizeof(api_cb_ptr->sqlca.sqlresn));
                p10execute( &api_cb_ptr->sqlca,
                            api_dbc_ptr->ConDesc->ga,
                            &api_dbc_ptr->dbno,
                            &localStmtLen,
                            (char*) SqlStmt->rawString,
                            SqlStmt->encodingType,
                            sqlda_ptr,
                            parsid_ptr,
                            api_stmt_ptr->stmtopt.data_caching );

                /* if dbproc with result set: get cursor name */
                if (api_stmt_ptr->stmt_function_code == csp1_cursor_pl_sql_execute_fc) {
                    if (API_MEMCMP( api_cb_ptr->sqlca.sqlresn, bsp_c64,
                                    sizeof(api_cb_ptr->sqlca.sqlresn))) {
                        pr05IfCom_String_ConvertP (api_stmt_ptr->cursor_name,
                                                   (char *) api_cb_ptr->sqlca.sqlresn,
                                                   sizeof (api_cb_ptr->sqlca.sqlresn),
                                                   sp77encodingUTF8);
                    }
                }

                api_stmt_ptr -> no_result_set = FALSE;
                if ( api_cb_ptr->sqlca.sqlcode == 100 ) {
                    /* ROW NOT FOUND */
                    api_stmt_ptr -> row_not_found = TRUE;
                    if ( api_dbc_ptr->set_autocommit_flag) {
                        /* the result set is implicitely closed by the
                         * kernel */
                        api_stmt_ptr -> no_result_set = TRUE;
                    }; /* if */
                } else {
                    api_stmt_ptr -> row_not_found = FALSE;
                }; /* else */
                if ( (api_cb_ptr->sqlca.sqlcode == -8
                      || apereconn(api_cb_ptr, api_dbc_ptr))
                     && try3 < PA_MAX_RECONNECT) {
#ifdef OLD_DA                        
                    API_HANDLE da_handle;
                    retcode = pa70AllocSQLDA( &da_handle, sqlda_ptr -> sqln);
                    if (retcode == API_OK) {
                        sqldatype *dap;
                        dap = (sqldatype FAR *) apdlock(da_handle);
                        pa30varcp(dap, sqlda_ptr);
#endif
                        /* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1125075 */
                        pa30bpcruntime(api_dbc_ptr, api_stmt_ptr);

                        p10parse ( &api_cb_ptr->sqlca,
                                   api_dbc_ptr->ConDesc->ga,
                                   &api_dbc_ptr->dbno,
                                   &localStmtLen,
                                   (char*) SqlStmt->rawString,
                                   SqlStmt->encodingType,
                                   sqlda_ptr,
                                   parsid_ptr );
                        API_TRACE(API_TR_PARSID, "parsid", parsid_ptr);
                        /* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1139091
                           if parsing returns with error: exit */
                        if (api_cb_ptr->sqlca.sqlcode != 0)
                          break;

#ifdef OLD_DA                        
                        pa30varcp(sqlda_ptr, dap);
                        apdunlk(da_handle);
                        apdfree(da_handle);
                    }
#endif                        
                }
                else 
                    break;
            }

            pa30apcruntime(api_dbc_ptr, api_stmt_ptr);

            aperetg (api_cb_ptr);
            pa30OraResultCount(api_cb_ptr, api_stmt_ptr);
            retcode = API_OK;
        }
        else
            retcode = API_NOT_OK;

        if (api_stmt_ptr->dbc_special.special != API_SPEC_NOT) {
            api_cb_ptr->sqlca.sqlrap->rakamode =
                api_stmt_ptr->stmtopt.sqlmode;
            api_cb_ptr->sqlca.sqlrap->rasqlansi =
                api_stmt_ptr->stmtopt.sqlmode;
        }
    
        API_TRACE(API_TR_EXIT,"apeexec",0);
        API_TRACE(API_TR_API_RETCODE,"retcode",&retcode);
    }  
    return (retcode);
} /* apeexec */


API_RETCODE apeput1l ( tpa41ESQBlock *api_cb_ptr,
                       tpa40DBC      *api_dbc_ptr,
                       tpa60Stmt     *api_stmt_ptr )
{
    sqldatype *sqlda_ptr;
    sqlcatype *sqlcap;
    SWORD sqln, varno;
    UCHAR FAR * parsid_ptr;
    API_RETCODE retcode;
  
    API_TRACE(API_TR_ENTRY, "apeput1l",0);
    API_TRACE(API_TR_SWORD, "dbno",&api_dbc_ptr->dbno);
    retcode = API_OK;
    sqlda_ptr = api_stmt_ptr->input_sqlda_ptr;
    parsid_ptr = api_stmt_ptr->stmt_parsid;
    API_TRACE(API_TR_SWORD,"sqln",&sqlda_ptr->sqln);
    sqln=sqlda_ptr->sqln;
    sqlcap = &api_cb_ptr->sqlca;
  
    varno = (SWORD)(api_stmt_ptr->next_param);
    varno++; /* pascal */
    
    API_TRACE(API_TR_SWORD, "varno", &varno);
    API_TRACE(API_TR_MSG, "p10putlongdata", 0);

    pa30bpcruntime(api_dbc_ptr, api_stmt_ptr);

    p10putlongdata( &api_cb_ptr->sqlca,
		    api_dbc_ptr->ConDesc->ga,
                    &api_dbc_ptr->dbno,
                    &varno,
                    sqlda_ptr,
                    parsid_ptr );
    API_TRACE(API_TR_SWORD, "varno", &varno);
    apereconn( api_cb_ptr, api_dbc_ptr );

    pa30apcruntime( api_dbc_ptr, api_stmt_ptr );

    aperetg (api_cb_ptr);
    API_TRACE(API_TR_EXIT, "apeput1l", 0);   
    API_TRACE(API_TR_API_RETCODE, "retcode", &retcode);
  
    return (retcode);
} /* apeput1l */


API_RETCODE apegetl ( tpa41ESQBlock  *api_cb_ptr,
                      tpa40DBC FAR   *api_dbc_ptr,
                      tpa60Stmt FAR  *api_stmt_ptr,
                      tpa20Desc      *impl_desc_ptr, 
                      SWORD          *col )
{
    sqldatype *sqldap;
    sqlcatype *sqlcap;
    SWORD sqln, varno = 0;
    UCHAR FAR * parsid_ptr;
    API_RETCODE retcode;
    tpa21DescRecord *impl_rec_ptr;
    
    API_TRACE(API_TR_ENTRY, "apegetl",0);
    API_TRACE(API_TR_SWORD, "col", col );
    retcode = API_OK;
    sqlcap = &api_cb_ptr->sqlca;
    parsid_ptr = api_stmt_ptr->stmt_parsid;
    sqldap = api_stmt_ptr->output_sqlda_ptr;
    sqln = sqldap->sqln;
    API_TRACE(API_TR_SWORD, "sqln", &sqln);

    pa30bpcruntime(api_dbc_ptr, api_stmt_ptr);
    impl_rec_ptr = pa20GetRecord( impl_desc_ptr, *col );
        API_ASSERT_PTR( impl_rec_ptr );
        if ( apgislong( impl_rec_ptr -> type)) {
            if ( !apgisindi( impl_rec_ptr -> optimizeNullPtr )) {
                UCHAR FAR * hostvaraddr;
                varno = (SWORD)*col;
                pa21GetLongAddr( &hostvaraddr, impl_rec_ptr );
                apgsvstrl( hostvaraddr, 0 );
                API_TRACE(API_TR_SWORD, "varno", &varno);	
                p10getlongdata( sqlcap,
				api_dbc_ptr->ConDesc->ga,
                                &api_dbc_ptr->dbno,
                                &varno,
                                sqldap,
                                parsid_ptr );
                API_TRACE(API_TR_SWORD, "varno", &varno);
                *col = varno;

                if (api_cb_ptr->sqlca.sqlcode != PA_OK) {
                  aperetg (api_cb_ptr);	
                  retcode = API_NOT_OK;
                }

            }; /* if */
        }; /* if */
    if (varno <= 0) {
        impl_rec_ptr -> lastData = API_TRUE;
    }

    pa30apcruntime( api_dbc_ptr, api_stmt_ptr);

    API_TRACE(API_TR_EXIT, "apegetl", 0);   
    API_TRACE(API_TR_SWORD, "col", col);
    API_TRACE(API_TR_API_RETCODE, "retcode", &retcode);
  
    return (retcode);
} /* apegetl */

API_RETCODE apedesc ( tpa41ESQBlock *api_cb_ptr,
                      UCHAR         *parsid_ptr,
                      tpa40DBC      *api_dbc_ptr, 
                      tpa60Stmt     *api_stmt_ptr)
{
    sqldatype *sqlda_ptr;
    API_RETCODE retcode=API_OK;
    SWORD try3 = 0;

    API_TRACE(API_TR_ENTRY,"apedesc",0);
    API_TRACE(API_TR_SWORD,"dbno",&api_dbc_ptr->dbno);
    API_TRACE(API_TR_UWORD,"special",&api_stmt_ptr->dbc_special.special);
    API_TRACE(API_TR_UWORD,"stmt_function_code",&api_stmt_ptr->stmt_function_code);
    API_TRACE(API_TR_PARSID, "parsid", parsid_ptr);
    if (pa09IsCanceled(0)) {
        p08runtimeerror (&api_cb_ptr->sqlca, api_cb_ptr->sqlca.sqlcxap, cpr_r_sql_cancelled);
        aperetg (api_cb_ptr);
    }
    else {
        if (api_stmt_ptr->stmt_function_code <  csp1_select_direct_fc ||
            api_stmt_ptr->stmt_function_code == csp1_cursor_pl_sql_execute_fc) {   /* dbproc: describe result set */
            sqlda_ptr = api_stmt_ptr->output_sqlda_ptr;
            sqlda_ptr->sqln = 0;
            API_TRACE(API_TR_UDWORD,"sqlmax",&sqlda_ptr -> sqlmax);
            API_TRACE(API_TR_UWORD,"sqld",&sqlda_ptr -> sqld);
            if (sqlda_ptr != NULL) {

                pa30bpcruntime(api_dbc_ptr, api_stmt_ptr);

                do {
                    try3++;
                    sqcldsc( &api_cb_ptr->sqlca,
                             api_dbc_ptr->ConDesc->ga,
                             api_dbc_ptr->dbno,
                             sqlda_ptr, parsid_ptr );

                if (api_cb_ptr->sqlca.sqlcode == PA_ERR_TOO_MANY_HOSTVAR) {
                    tsp1_part *part = NULL;
                    UWORD      arg_count = 0;

                api_cb_ptr->sqlca.sqlcode = 0;
                    /* get arg_count from part. If sqlda is too small
                     * resize it. If it is not resized before
                     * p11shortfieldparameterput the statement is parsed
                     * again (degrades performance) PTS 1106783 */
                    p03find_part( api_cb_ptr->sqlca.sqlrap,
                                  sp1pk_shortinfo,
                                  &part);
                    if (part != NULL) /* PTS 1110533 */
                      arg_count = (UWORD) part->sp1p_part_header.sp1p_arg_count;
                    if (sqlda_ptr->sqlmax < arg_count) {
                        pa30ResizeSQLDA( &sqlda_ptr, arg_count );
                        api_stmt_ptr->output_sqlda_ptr = sqlda_ptr;
                        api_stmt_ptr->output_sqlda_handle = sqlda_ptr;
                    };
                    p11shortfieldparameterput( &api_cb_ptr->sqlca,
                                               api_dbc_ptr->ConDesc->ga,
                                               sqlda_ptr,
                                               API_FALSE );


                }; /* if */

                }
                while ( apereconn(api_cb_ptr, api_dbc_ptr) && try3
                        < PA_MAX_RECONNECT);
                API_TRACE(API_TR_UDWORD,"sqlmax",&sqlda_ptr -> sqlmax);
                API_TRACE(API_TR_UWORD,"sqln",&sqlda_ptr -> sqln);
                API_TRACE(API_TR_UWORD,"sqld",&sqlda_ptr -> sqld);

                pa30apcruntime(api_dbc_ptr, api_stmt_ptr);

                aperetg (api_cb_ptr);
            }
            else
                retcode = API_NOT_OK;
        }
    }
    API_TRACE(API_TR_EXIT,"apedesc",0);
    API_TRACE(API_TR_API_RETCODE,"retcode",&retcode);

    return (retcode);
} /* apedesc */


VOID apedrprs ( tpa41ESQBlock *api_cb_ptr,
                UCHAR         *parsid_ptr,
                tpa40DBC      *api_dbc_ptr,
                tpa60Stmt     *api_stmt_ptr)
{
    SWORD try3 = 0;

    API_TRACE(API_TR_ENTRY,"apedrprs",0);
    API_TRACE(API_TR_SWORD,"dbno",&api_dbc_ptr->dbno);
    API_TRACE(API_TR_UWORD,"special",&api_stmt_ptr->dbc_special.special);
    API_TRACE(API_TR_PARSID, "parsid", parsid_ptr);
    if (pa09IsCanceled(0)) {
        p08runtimeerror (&api_cb_ptr->sqlca,
                         api_cb_ptr->sqlca.sqlcxap,
                         cpr_r_sql_cancelled);
        aperetg (api_cb_ptr);
    }
    else {
        if (!pa06IsNullParseId( parsid_ptr )) {
            pa30bpcruntime(api_dbc_ptr, api_stmt_ptr);
            do {
                try3++;
                pr10DropParseID (api_dbc_ptr, parsid_ptr);
            }	
            while ( apereconn( api_cb_ptr, api_dbc_ptr)
                    && try3 < PA_MAX_RECONNECT);
            pa30apcruntime(api_dbc_ptr, api_stmt_ptr);
            aperetg (api_cb_ptr);
            pa06InitNullParseId( parsid_ptr );
        }
    }
    API_TRACE(API_TR_EXIT,"apedrprs",0);
    API_TRACE(API_TR_API_ESQ_BLOCK,"tpa41ESQBlock",api_cb_ptr);  
    return ;
} /* apedrprs */


/* apeimme() - API ESQ function, execute SQL EXECUTE IMMEDIATE */

API_RETCODE  apeimme ( tpa41ESQBlock *api_cb_ptr,
                       tpr05_String  *SqlStmt,
                       tpa40DBC      *api_dbc_ptr,
                       tpa60Stmt     *api_stmt_ptr)
{
    API_RETCODE retcode=API_OK;
    SWORD try3 = 0;
    tsp00_Int4 localStmtLen = (tsp00_Int4) SqlStmt->cbLen;

    API_TRACE(API_TR_ENTRY,"apeimme",0);
    API_TRACE(API_TR_SWORD,"dbno",&api_dbc_ptr->dbno);
    API_TRACE(API_TR_UWORD,"special",&api_stmt_ptr->dbc_special.special);
    API_TRACE_LEN(API_TR_TPR05STRING,"stmt_ptr",SqlStmt, SqlStmt->cbLen);

    if (pa09IsCanceled(0)) {
        p08runtimeerror ( &api_cb_ptr->sqlca,
                          api_cb_ptr->sqlca.sqlcxap,
                          cpr_r_sql_cancelled );
        aperetg (api_cb_ptr);
    }
    else {
        pa30bpcruntime(api_dbc_ptr, api_stmt_ptr);
        /* pa30SetAutocommitFlag( api_dbc_ptr ); */
        do {
            try3++;
            p10statement( &api_cb_ptr->sqlca,
                          api_dbc_ptr->ConDesc->ga, 
                          &api_dbc_ptr->dbno,
                          &localStmtLen,
                          (char*) SqlStmt->rawString, SqlStmt->encodingType);
        } while ( apereconn(api_cb_ptr, api_dbc_ptr)
                  && try3 < PA_MAX_RECONNECT);
        api_stmt_ptr -> no_result_set = FALSE;
        if ( api_cb_ptr->sqlca.sqlcode == 100 ) {
            /* ROW NOT FOUND */
            api_stmt_ptr -> row_not_found = TRUE;
            if ( api_dbc_ptr->set_autocommit_flag) {
                /* the result set is implicitely closed by the
                 * kernel */
                api_stmt_ptr -> no_result_set = TRUE;
            }; /* if */
        } else {
            api_stmt_ptr -> row_not_found = FALSE;
        }; /* else */
        pa30apcruntime(api_dbc_ptr, api_stmt_ptr);
        aperetg (api_cb_ptr);
        pa30OraResultCount(api_cb_ptr, api_stmt_ptr);
        api_stmt_ptr->stmt_function_code = 
            pa30GetFunctionCode( api_dbc_ptr->ConDesc->ga);        
        API_TRACE(API_TR_EXIT,"apeimme",0);
        API_TRACE(API_TR_API_RETCODE,"retcode",&retcode);
    }
    return (retcode);
} /* apeimme */

/*#define OLD_DA*/

API_RETCODE apefetc ( tpa41ESQBlock *api_cb_ptr,
                      tpa40DBC FAR  *api_dbc_ptr,
                      tpa60Stmt     *api_stmt_ptr,
                      UWORD          fetchtype,
                      SDWORD         pos )
{
    tpr05_String *SqlStmt;
    tsp00_Int4   stmt_len;
    API_RETCODE  retcode;
    /*    UCHAR        local_stmt[ 10 + sizeof(SYS_CMD_FETCH)
                             + API_ODBC_CURSOR_LEN
                             + sizeof(PA30_CMD_INTO) + 20];
    */
    unsigned int length;
    SWORD        try3 = 0;
    sqldatype   *sqlda_ptr;
    sqlcxatype  *sqlxap = api_cb_ptr->sqlca.sqlcxap;
    UCHAR       *parsid_ptr;
    UCHAR        local_parsid [ API_SQLDB_PARSID_LEN ]; 
    BOOLEAN      parsenew = FALSE;
    SDWORD       sqlCode;
    
    API_TRACE(API_TR_ENTRY,"apefetc",0);
    API_TRACE(API_TR_PTR,"api_cb_ptr", &api_cb_ptr);
    API_TRACE(API_TR_PTR,"api_dbc_ptr", &api_dbc_ptr);
    API_TRACE(API_TR_PTR,"api_stmt_ptr", &api_stmt_ptr);
    API_TRACE(API_TR_SWORD,"dbno",&api_dbc_ptr->dbno);
    API_TRACE(API_TR_UWORD,"fetchtype", &fetchtype);
    API_TRACE(API_TR_UDWORD,"pos", &pos);

    length = 10 + sizeof(SYS_CMD_FETCH) + API_ODBC_CURSOR_LEN
                + sizeof(PA30_CMD_INTO) + 20;

    SqlStmt = pr05IfCom_String_NewDynString (length * api_stmt_ptr->cursor_name->encodingType->fixedCharacterSize,
                                             NULL);         /* PTS 1110978 */
    
    pa06InitNullParseId( local_parsid );
    retcode = API_OK;
    sqlda_ptr = api_stmt_ptr->output_sqlda_ptr;
    pa30bpcruntime(api_dbc_ptr, api_stmt_ptr);
   
    pa30mkfetchstr (SqlStmt, api_stmt_ptr->cursor_name, fetchtype);
    stmt_len = SqlStmt->cbLen;
    parsid_ptr = api_stmt_ptr->stmt_parsid;
    if (parsid_ptr[CPR_PARS_IDIX] == csp1_p_select_for_update_found ||
        parsid_ptr[CPR_PARS_IDIX] == csp1_p_reuse_update_sel_found  ||
        parsid_ptr[CPR_PARS_IDIX] == csp1_p_dialog_call) {
        /* stmt_ptr[0] = ' ';  MFETCH -> FETCH */
        pa30ReplaceFirstCharacterWithBlank (SqlStmt);
    }
    if (pa60GetFetchParseId( api_stmt_ptr,
                             &parsid_ptr,
                             fetchtype) != API_OK) {
        parsenew = TRUE;
        parsid_ptr = local_parsid;
    } else {
        if ( pa06IsNullParseId( parsid_ptr ) )
            parsenew = TRUE;
    };
    
    /* kein MFETCH wenn mehr als eine Longspalte im Kommando */
    {
        UWORD number_cols;
        UWORD i;
        UWORD lcount=0;
        tpa21DescRecord *ird_rec_ptr;
        
        number_cols = api_stmt_ptr -> number_cols;
        for ( i=1; i <= number_cols; i++ ) {
            ird_rec_ptr = pa20GetRecord( &api_stmt_ptr->ird, i );
            if (apgislong( ird_rec_ptr-> type ))
                lcount++;
            if (lcount > 1) {
                /* stmt_ptr[0] = ' ';  MFETCH -> FETCH */
                pa30ReplaceFirstCharacterWithBlank (SqlStmt);
                break;
            }; /* if */
        }; /* for */
    }
    API_TRACE_LEN( API_TR_TPR05STRING, "stmt_ptr", SqlStmt, SqlStmt->cbLen );

    /* FETCH <curname> */
    API_TRACE(API_TR_UCHAR,"parsenew", &parsenew);
    api_cb_ptr->sqlca.sqlcode = PA_OK;
      
    for (;;) {
        try3++;
        if (parsenew) {
#ifdef OLD_DA
            retcode = pa70AllocSQLDA( &da_handle, sqlda_ptr->sqln );
            if (retcode == API_OK) {
                sqldatype *dap;
                dap = (sqldatype FAR *) apdlock(da_handle);
                pa30varcp(dap, sqlda_ptr);
#endif                
                for (;;) {
                    API_TRACE(API_TR_SDWORD,"sqlloop", &sqlda_ptr->sqlloop);
                    p10parse( &api_cb_ptr->sqlca,
                              api_dbc_ptr->ConDesc->ga,
                              &api_dbc_ptr->dbno,
                              &stmt_len,
                              (char*) SqlStmt->rawString,
                              SqlStmt->encodingType,
                              sqlda_ptr,
                              parsid_ptr );
                    if (api_cb_ptr->sqlca.sqlcode == -705) /* Too many FETCH
                                                            * statements */
                        /*    stmt_ptr[0] = ' ';  FFETCH -> FETCH */
                        pa30ReplaceFirstCharacterWithBlank (SqlStmt);
                    else 
                        break;
                }
#ifdef OLD_DA                
                pa30varcp(sqlda_ptr, dap);
                apdunlk(da_handle);
                apdfree(da_handle);
            };
#endif                
        };
        if (api_cb_ptr->sqlca.sqlcode == PA_OK && retcode == API_OK) {
            switch (fetchtype) {
            case (SQL_FETCH_ABSOLUTE):
                sqlda_ptr -> sqlprno = CPR_FE_POSV;
                sqlxap->xacfetposc = pos;
                break;
            case (SQL_FETCH_FIRST):
                sqlda_ptr -> sqlprno = CPR_FE_FIRST;
                break;
            case (SQL_FETCH_LAST):
                sqlda_ptr -> sqlprno = CPR_FE_LAST;
                break;
            case (SQL_FETCH_NEXT):
                sqlda_ptr -> sqlprno = CPR_FE_NEXT;
                break;
            case (SQL_FETCH_PRIOR):
                sqlda_ptr -> sqlprno = CPR_FE_PREV;
                break;
            case (SQL_FETCH_RELATIVE):
                sqlda_ptr -> sqlprno = CPR_FE_RELV;
                sqlxap->xacfetposc = pos;
                break;
            }; /* switch */
            API_TRACE(API_TR_UDWORD,"sqlmax",&sqlda_ptr -> sqlmax);
            API_TRACE(API_TR_UWORD,"sqln",&sqlda_ptr -> sqln);
            API_TRACE(API_TR_UWORD,"sqld",&sqlda_ptr -> sqld);
            API_TRACE(API_TR_SDWORD,"sqlloop", &sqlda_ptr -> sqlloop);
            p10execute ( &api_cb_ptr->sqlca,
                         api_dbc_ptr->ConDesc->ga,
                         &api_dbc_ptr->dbno,
                         &stmt_len,
                         (char*) SqlStmt->rawString,
                         SqlStmt->encodingType,
                         sqlda_ptr,
                         parsid_ptr,
                         api_stmt_ptr->stmtopt.data_caching );
            if (api_cb_ptr->sqlca.sqlcode == -8) /* Execution failed
                                                  * parse again */
                parsenew = TRUE;
            else
                break;
        }
        /* ADIS 1001416 */
        else
            break;
    } /* for */

    if (!pa06IsNullParseId( local_parsid )) {
        /* save sql-code from fetch */
        sqlCode = api_cb_ptr->sqlca.sqlcode;
        apedrprs( api_cb_ptr, local_parsid, api_dbc_ptr, api_stmt_ptr );
        api_cb_ptr->sqlca.sqlcode = sqlCode;
    }; /* if */
    if ( apereconn(api_cb_ptr, api_dbc_ptr) ) 
        api_cb_ptr->sqlca.sqlcode = +700; /* Command inactivity timeout
                                           * (work rolled back) */
    pa30apcruntime(api_dbc_ptr, api_stmt_ptr);
    aperetg (api_cb_ptr);
    pr05IfCom_String_DeleteString (SqlStmt);     /* PTS 1110978 */

    API_TRACE(API_TR_EXIT,"apefetc",0);
    API_TRACE(API_TR_API_ESQ_BLOCK,"tpa41ESQBlock",api_cb_ptr);
    API_TRACE(API_TR_API_RETCODE,"retcode",&retcode);
    return (retcode);
} /* apefetc */

/*
VOID pa30mkfetchstr( UCHAR  *stmt_str, 
                     UCHAR  *cursor_name, 
                     UWORD   fetchtype,
                     SDWORD  pos )
{
    UCHAR local_str[20];
    API_TRACE(API_TR_ENTRY, "pa30mkfetchstr", 0);
    API_STRCPY (stmt_str, "F"SYS_CMD_FETCH); 
    switch (fetchtype) {
    case (SQL_FETCH_NEXT):{
        API_STRCPY(local_str, "");
        break;
    }
    case (SQL_FETCH_FIRST):{
        API_STRCPY(local_str, "FIRST ");
        break;	 
    }
    case (SQL_FETCH_LAST):{
        API_STRCPY(local_str, "LAST ");
        break;	 
    }
    case (SQL_FETCH_PRIOR):{
        API_STRCPY(local_str, "PREV ");
        break;	 
    }
    case (SQL_FETCH_RELATIVE):{}
        API_STRCPY( local_str, "RELATIVE ? " );
        break;	 
    case (SQL_FETCH_BOOKMARK):{}
    case (SQL_FETCH_ABSOLUTE):{
        API_STRCPY(local_str, "POS(?) " );
        break;	 
    }
    default:{}      
    }
    API_STRCAT (stmt_str, local_str);       
    API_STRCAT (stmt_str, "\"");       
    API_STRCAT (stmt_str, cursor_name);       
    API_STRCAT (stmt_str, PA30_CMD_INTO); 
    API_TRACE(API_TR_EXIT, "pa30mkfetchstr", 0);
}*/ /* pa30mkfetchstr */

VOID pa30mkfetchstr( tpr05_String  *SqlStmt, 
                     tpr05_String  *cursor_name, 
                     UWORD          fetchtype)
{
    UCHAR local_str[20];
    API_TRACE(API_TR_ENTRY, "pa30mkfetchstr", 0);

    switch (fetchtype) {
    case (SQL_FETCH_NEXT):{
        API_STRCPY(local_str, "");
        break;
    }
    case (SQL_FETCH_FIRST):{
        API_STRCPY(local_str, "FIRST ");
        break;	 
    }
    case (SQL_FETCH_LAST):{
        API_STRCPY(local_str, "LAST ");
        break;	 
    }
    case (SQL_FETCH_PRIOR):{
        API_STRCPY(local_str, "PREV ");
        break;	 
    }
    case (SQL_FETCH_RELATIVE):{}
        API_STRCPY( local_str, "RELATIVE ? " );
        break;	 
    case (SQL_FETCH_BOOKMARK):{}
    case (SQL_FETCH_ABSOLUTE):{
        API_STRCPY(local_str, "POS(?) " );
        break;	 
    }
    default:{}      
    }

    SqlStmt->encodingType = cursor_name->encodingType;
    SqlStmt->cbLen = sp77sprintfUnicode (cursor_name->encodingType,
                                         SqlStmt->rawString, SqlStmt->cbMaxLen, 
                                         "F%s%s\"%'=.*S%s",
                                         SYS_CMD_FETCH,
                                         local_str,
                                         cursor_name->encodingType,
                                         cursor_name->cbLen,
                                         cursor_name->rawString,
                                         PA30_CMD_INTO);
    /*    
    API_STRCAT (stmt_str, local_str);       
    API_STRCAT (stmt_str, "\"");       
    API_STRCAT (stmt_str, cursor_name);       
    API_STRCAT (stmt_str, PA30_CMD_INTO);   INTO*/
    API_TRACE(API_TR_EXIT, "pa30mkfetchstr", 0);
} /* pa30mkfetchstr */


/* apeclos() - API ESQ function, execute SQL CLOSE */

VOID apeclos ( tpa41ESQBlock *api_cb_ptr,
               tpr05_String  *curnam_ptr,
               tpa40DBC      *api_dbc_ptr,
               tpa60Stmt     *api_stmt_ptr )
{
    tpr05_String SqlStmt;
    tsp00_Int4   stmt_len;
    API_RETCODE  retcode;
    API_HANDLE   sqlda_handle;
    sqldatype   *sqlda_ptr;
    char         stmt_str[2* (50+MX_IDENTIFIER) ]; 
   
   
    API_TRACE(API_TR_ENTRY,"apeclos",0);
    API_TRACE(API_TR_SWORD,"dbno",&api_dbc_ptr->dbno);
    API_TRACE(API_TR_UWORD,"special",&api_stmt_ptr->dbc_special.special);
    API_TRACE_LEN(API_TR_TPR05STRING,"curnam_ptr",curnam_ptr, curnam_ptr->cbLen);
   
    SqlStmt.allocationType = constant_epr05;
    pr05IfCom_String_InitString (&SqlStmt, stmt_str, sizeof (stmt_str),
                                 curnam_ptr->encodingType, constant_epr05);

    pr05IfCom_String_ClearString (&SqlStmt);

    pr05IfCom_String_strcatP (&SqlStmt,
                              SYS_CMD_CLOSE,
                              sizeof (SYS_CMD_CLOSE)-1,
                              sp77encodingAscii);

    pr05IfCom_String_strcat (&SqlStmt, curnam_ptr);

    pr05IfCom_String_strcatP (&SqlStmt,
                              "\"",
                              sizeof ("\"")-1,
                              sp77encodingAscii);

    /*
    API_STRCPY (stmt_ptr, SYS_CMD_CLOSE);
    API_STRCAT (stmt_ptr, curnam_ptr);  
    API_STRCAT (stmt_ptr, "\"");  
    stmt_len = API_STRLEN(stmt_ptr);
    */
    stmt_len = SqlStmt.cbLen;
    API_TRACE_LEN(API_TR_TPR05STRING,"stmt_ptr", &SqlStmt,SqlStmt.cbLen);
    API_TRACE(API_TR_SWORD,"stmt_len",&stmt_len);
   
    /* CLOSE <curname> */
    retcode = pa70AllocSQLDA( &sqlda_handle, 1 ); /* Dummy sqlda */
    if (retcode == API_OK) {
        sqlda_ptr = (sqldatype FAR *) apdlock(sqlda_handle);
        p10setkamode( &api_cb_ptr->sqlca, sqlda_ptr );
        api_cb_ptr->sqlca.sqlcode = PA_OK;
        pa30bpcruntime(api_dbc_ptr, api_stmt_ptr);
        /* see if parsid can be reused */
        if (pa06IsNullParseId( api_stmt_ptr->close_parsid )) {
            p10parse( &api_cb_ptr->sqlca,
		      api_dbc_ptr->ConDesc->ga,
                      &api_dbc_ptr->dbno,
                      &stmt_len,
                      (char*) SqlStmt.rawString,
                      SqlStmt.encodingType,
                      sqlda_ptr,
                      api_stmt_ptr->close_parsid );
        }; /* if */
        if ( !apereconn(api_cb_ptr, api_dbc_ptr) ) {
            if ( api_cb_ptr->sqlca.sqlcode == PA_OK) {
                /* pa30SetAutocommitFlag( api_dbc_ptr ); */
                p10execute( &api_cb_ptr->sqlca,
			    api_dbc_ptr->ConDesc->ga,
                            &api_dbc_ptr->dbno,
                            &stmt_len,
                            (char*) SqlStmt.rawString,
                            SqlStmt.encodingType,
                            sqlda_ptr,
                            api_stmt_ptr->close_parsid,
                            api_stmt_ptr->stmtopt.data_caching );
            }	
            if ( api_cb_ptr->sqlca.sqlcode == PA_ERR_UNKNOWN_RESULT_TABLE)  
                api_cb_ptr->sqlca.sqlcode = PA_OK;	 
            aperetg (api_cb_ptr);
            if (pa40UseOldAutocommit( api_dbc_ptr )) {
                if (api_dbc_ptr->set_autocommit_flag)
                    retcode = pa30commit(api_cb_ptr, api_dbc_ptr);
            }; /* if */
        }
        else {
            /* the cursor was closed by commandtimeout */
            api_cb_ptr->sqlca.sqlcode = PA_OK;	 
            aperetg (api_cb_ptr);
        }
        pa30apcruntime(api_dbc_ptr, api_stmt_ptr);
        apdunlk(sqlda_handle);	
        apdfree(sqlda_handle);
    }
    API_TRACE(API_TR_EXIT,"apeclos",0);
    API_TRACE(API_TR_API_RETCODE,"retcode",&retcode);
   
    return;
} /* apeclos */


SWORD apereconn( tpa41ESQBlock *api_cb_ptr, 
                 tpa40DBC      *api_dbc_ptr)
{
    tsp00_Int4 stmt_len;   
    SWORD ret;
    UCHAR local_str[256];
    
    ret = FALSE;    
    API_TRACE(API_TR_ENTRY,"apereconn",0);
    /* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1131374 */
    if ( api_cb_ptr->sqlca.sqlcode == 700  ||  api_cb_ptr->sqlca.sqlcode == 70  ||
         api_cb_ptr->sqlca.sqlcode == PA_ERR_CONNECTION_DOWN  ||
         api_cb_ptr->sqlca.sqlcode == PA_ERR_SERVERDB_NOT_AVAILABLE) {
        API_TRACE(API_TR_SWORD, "dbno", &api_dbc_ptr->dbno);
        API_TRACE(API_TR_STRING, "userid", api_dbc_ptr->userid);
        API_TRACE(API_TR_STRING, "passw", api_dbc_ptr->passw);
        API_TRACE(API_TR_STRING, "serverdb", api_dbc_ptr->serverdb);
        API_TRACE(API_TR_STRING, "servernode", api_dbc_ptr->servernode);
        stmt_len = pa30MakeConnStr(local_str, api_dbc_ptr, &api_cb_ptr->sqlca);
        /* sqlmode is set in pa08analyseconnectstm PTS 1104179 */
        api_cb_ptr->sqlca.sqldbmode = cpr_kind_empty;
        api_cb_ptr->sqlca.sqlrap->rasqlansi = cpr_kind_empty;
#ifndef NEWPCRTE
	{
	  tpr05_String ConStr;
	  ConStr.allocationType = constant_epr05;
          pr05IfCom_String_InitString(&ConStr, local_str, stmt_len,
                                sp77encodingAscii,constant_epr05);
	  api_dbc_ptr->ConDesc = 
            pr10Connect( api_dbc_ptr, &ConStr, api_dbc_ptr->servernode, 
                         api_dbc_ptr->serverdb,
                         api_dbc_ptr->userid,
                         api_dbc_ptr->passw);
	}
#else
        p10connect( &api_cb_ptr->sqlca,
		    api_dbc_ptr->ConDesc->ga,
                    &api_dbc_ptr->dbno,
                    api_dbc_ptr->servernode,
                    api_dbc_ptr->serverdb, 
                    &stmt_len,
                    (char*) local_str,
                    api_dbc_ptr->userid,
                    api_dbc_ptr->passw ); 
#endif
        if (api_dbc_ptr->constmtopt.sqlmode != CPR_KIND_INTERNAL) {
            api_cb_ptr->sqlca.sqlrap->radbmode = CPR_KIND_INTERNAL;
        }
        if (api_cb_ptr->sqlca.sqlcode == PA_OK) {
            api_dbc_ptr->status = API_STMT_RECONNECTED;
            ret = TRUE;
            pa40SetReconnectFlag(api_dbc_ptr);
        }
        if (api_dbc_ptr->subtranscount != 0) {
            api_dbc_ptr->subtranscount = 0;
            api_dbc_ptr->set_autocommit_flag = api_dbc_ptr->autocommit;
        }
    }	    
    API_TRACE(API_TR_EXIT,"apereconn",0);
    API_TRACE(API_TR_SWORD,"ret",&ret);
    return(ret);
} /* apereconn */


API_RETCODE pa30commit( tpa41ESQBlock *api_cb_ptr,
                        tpa40DBC      *api_dbc_ptr)
{
    API_RETCODE retcode=API_OK;  
    tsp00_Int4  stmt_len;   
    SDWORD      sqlcode;

    API_TRACE(API_TR_ENTRY,"pa30commit",0);
    if (!pa01UtilityConnect()) {
        stmt_len = sizeof(SYS_CMD_COMMIT_WORK)-1;
        sqlcode = api_cb_ptr->sqlca.sqlcode;
        p10statement( &api_cb_ptr->sqlca,
		      api_dbc_ptr->ConDesc->ga,
		      &api_dbc_ptr->dbno,
                      &stmt_len,
                      SYS_CMD_COMMIT_WORK, sp77encodingAscii );
        if (api_cb_ptr->sqlca.sqlcode != PA_OK) {
            aperetg (api_cb_ptr);	
            retcode = API_NOT_OK;
        }
        else {
            if (api_dbc_ptr->subtranscount != 0) {
                api_dbc_ptr->set_autocommit_flag = api_dbc_ptr->autocommit;
                api_dbc_ptr->subtranscount = 0;
            }
        }    
        api_cb_ptr->sqlca.sqlcode = sqlcode;
    } 
    API_TRACE(API_TR_EXIT,"pa30commit",0);
    API_TRACE(API_TR_API_RETCODE,"retcode",&retcode);
  
    return (retcode);
} /* pa30commit */


API_RETCODE pa30SubtransBegin( tpa41ESQBlock *api_cb_ptr,
                               tpa40DBC      *api_dbc_ptr )
{
    API_RETCODE retcode;  
    tsp00_Int4  stmt_len;   
    SDWORD      sqlcode;
  
    API_TRACE(API_TR_ENTRY, "pa30SubtransBegin",0);
    retcode = API_OK;
    stmt_len = (tsp00_Int4) API_STRLEN(PA30_CMD_SUBTRANS_BEGIN);
    sqlcode = api_cb_ptr->sqlca.sqlcode;
    api_dbc_ptr->set_autocommit_flag = 0;
    p10statement( &api_cb_ptr->sqlca,
		  api_dbc_ptr->ConDesc->ga, 
                  &api_dbc_ptr->dbno,
                  &stmt_len,
                  (const char*) PA30_CMD_SUBTRANS_BEGIN, sp77encodingAscii );
    if ( apereconn(api_cb_ptr, api_dbc_ptr) ) 
        api_cb_ptr->sqlca.sqlcode = +700;
    if (api_cb_ptr->sqlca.sqlcode != PA_OK) {
        aperetg (api_cb_ptr);	
        retcode = API_NOT_OK;
    }
    api_cb_ptr->sqlca.sqlcode = sqlcode;
    API_TRACE(API_TR_EXIT, "pa30SubtransBegin",0);
    API_TRACE(API_TR_API_RETCODE,"retcode",&retcode);
  
    return (retcode);
} /* pa30SubTransBegin */


API_RETCODE pa30SubtransEnd( tpa41ESQBlock *api_cb_ptr,
                             tpa40DBC      *api_dbc_ptr )
{
    API_RETCODE retcode;  
    tsp00_Int4  stmt_len;   
    SDWORD      sqlcode;
  
    API_TRACE(API_TR_ENTRY, "pa30SubtransEnd",0);
    retcode = API_OK;
    stmt_len = (tsp00_Int4) API_STRLEN(PA30_CMD_SUBTRANS_END);
    sqlcode = api_cb_ptr->sqlca.sqlcode;
    p10statement( &api_cb_ptr->sqlca,
		  api_dbc_ptr->ConDesc->ga, 
                  &api_dbc_ptr->dbno,
                  &stmt_len,
                  (const char*) PA30_CMD_SUBTRANS_END, sp77encodingAscii );
    if ( apereconn(api_cb_ptr, api_dbc_ptr) ) 
        api_cb_ptr->sqlca.sqlcode = +700;
    if (api_cb_ptr->sqlca.sqlcode != PA_OK) {
        aperetg (api_cb_ptr);	
        retcode = API_NOT_OK;
    }
    api_cb_ptr->sqlca.sqlcode = sqlcode;
    api_dbc_ptr->set_autocommit_flag = api_dbc_ptr->autocommit;
    API_TRACE(API_TR_EXIT, "pa30SubtransEnd",0);
    API_TRACE(API_TR_API_RETCODE,"retcode",&retcode);
  
    return (retcode);
} /* pa30SubTransEnd */


API_RETCODE pa30SubtransRollback( tpa41ESQBlock *api_cb_ptr,
                                  tpa40DBC      *api_dbc_ptr )
{
    API_RETCODE  retcode;  
    tsp00_Int4   stmt_len;   
    tsp00_Int4   sqlcode;
  
    API_TRACE(API_TR_ENTRY, "pa30SubtransRollback",0);
    retcode = API_OK;
    stmt_len = (tsp00_Int4) API_STRLEN(PA30_CMD_SUBTRANS_ROLLBACK);
    sqlcode = api_cb_ptr->sqlca.sqlcode;
    p10statement( &api_cb_ptr->sqlca,
		  api_dbc_ptr->ConDesc->ga, 
                  &api_dbc_ptr->dbno,
                  &stmt_len,
                  (const char*) PA30_CMD_SUBTRANS_ROLLBACK, sp77encodingAscii );
    if ( apereconn(api_cb_ptr, api_dbc_ptr) ) 
        api_cb_ptr->sqlca.sqlcode = +700;
    if (api_cb_ptr->sqlca.sqlcode != PA_OK) {
        aperetg (api_cb_ptr);	
        retcode = API_NOT_OK;
    }
    api_cb_ptr->sqlca.sqlcode = sqlcode;
    api_dbc_ptr->set_autocommit_flag = api_dbc_ptr->autocommit;
    API_TRACE(API_TR_EXIT, "pa30SubtransRollback",0);
    API_TRACE(API_TR_API_RETCODE,"retcode",&retcode);
  
    return (retcode);
} /* pa30SubtransRollback */


tsp00_Int4 pa30MakeConnStr( UCHAR     *str,
                            tpa40DBC  *dbc_ptr,
                            sqlcatype *sqlcap )
{
    SWORD isolevel = 0;
    switch(dbc_ptr->isolevel) {
    case (SQL_TXN_READ_UNCOMMITTED): {
        isolevel = 0;
        break;
    }
    case (SQL_TXN_READ_COMMITTED): {
        isolevel = 1;
        break;
    }
    case (SQL_TXN_REPEATABLE_READ): {
        isolevel = 2;
        break;
    }
    case (SQL_TXN_SERIALIZABLE): {
        isolevel = 3;
        break;
    }
    }
    API_SPRINTF( (char*) str,
                 SYS_CMD_CONNECT,
                 pa30SQLModeStr(dbc_ptr->constmtopt.sqlmode),
                 isolevel );
    if (dbc_ptr->sessionTimeout != SQL_DEFAULT_SESSION_TIMEOUT) {
        char timeoutStr[30];

        API_SPRINTF( timeoutStr, " TIMEOUT %lu", dbc_ptr->sessionTimeout );
        API_STRCAT( str, timeoutStr );
    };
    if (API_MEMCMP( sqlcap->sqlgap->gaopxuserrec.xu_dblang,
                    bsp_c64,
                    sizeof(sqlcap->sqlgap->gaopxuserrec.xu_dblang))) {
        sqlcap->sqlgap->gauseropset = cpr_is_false;
        API_STRCAT(str, " CHARACTER SET ");
        API_STRNCAT( str,
                     sqlcap->sqlgap->gaopxuserrec.xu_dblang,
                     sizeof(sqlcap->sqlgap->gaopxuserrec.xu_dblang));
    }
    API_TRACE(API_TR_STRING, "str", str);
    return((tsp00_Int4) API_STRLEN(str));
} /* pa30MakeConnStr */


BOOLEAN pa30GetTableNamePart( tsp1_segment *segm_ptr,
                              tpr05_String *string )
{
    SDWORD  len;
    UCHAR  *szTableStr = (UCHAR*) string->rawString;
    tsp1_part_ptr part_ptr;
    int     cbValueMax = string->cbMaxLen;
    
    s26find_part (segm_ptr, (char) sp1pk_tablename, &part_ptr);
    API_TRACE(API_TR_PTR, "sp1pk_tablename", &part_ptr);
    if (part_ptr) {
        /* cbValueMax-1 due to 2-byte Unicode */
        if (cbValueMax-1 < part_ptr->sp1p_part_header.sp1p_buf_len)
            len = cbValueMax-1;
        else
            len = part_ptr->sp1p_part_header.sp1p_buf_len;
            
        API_MEMCPY(szTableStr, part_ptr->sp1p_buf, len);
        szTableStr[len] = szTableStr[len+1] = '\0';
        string->cbLen = len;
        API_TRACE(API_TR_STRING,"szTableStr", szTableStr);
        return(TRUE);
    }	
    else
        return(FALSE);
} /* pa30GetTableNamePart */


API_RETCODE  pa30SyntaxCheck ( tpa41ESQBlock *api_cb_ptr,
                               tpa40DBC      *api_dbc_ptr,
                               tpa60Stmt     *api_stmt_ptr,
                               tpr05_String  *SqlCmd)
{
    API_RETCODE retcode=API_OK;
    SWORD try3 = 0;

    API_TRACE(API_TR_ENTRY, "pa30SyntaxCheck", 0);
    API_TRACE(API_TR_SWORD, "dbno", &api_dbc_ptr->dbno);
    API_TRACE_LEN(API_TR_TPR05STRING,"stmt_ptr", SqlCmd, SqlCmd->cbLen);

    do {
        try3++;

        pr11cSyntax (&api_cb_ptr->sqlca,
                     api_dbc_ptr->ConDesc,
                     SqlCmd);
        /*
        sqclsyn (&api_cb_ptr->sqlca,
                 api_dbc_ptr->ConDesc->ga,
                 stmt_len, (char*) stmt_ptr);
        */
    }
    while ( apereconn(api_cb_ptr, api_dbc_ptr) && try3 < PA_MAX_RECONNECT);
    aperetg (api_cb_ptr);
  
    api_stmt_ptr->stmt_function_code = 
        pa30GetFunctionCode( api_dbc_ptr->ConDesc->ga);
    API_TRACE(API_TR_EXIT,"pa30SyntaxCheck",0);
    API_TRACE(API_TR_API_RETCODE,"retcode",&retcode);

    return (retcode);
} /* pa30SyntaxCheck */


UWORD pa30GetFunctionCode(sqlgaentry *gaent)
{
    tsp1_packet *pck = (tsp1_packet *)((gaent) ? gaent->garecptr : NULL);
    tsp1_segment_header *seghp
        = (pck) ? &pck->sp1_segm.sp1s_segm_header : NULL;
    API_TRACE( API_TR_UWORD,
               "pa30GetFunctionCode",
               (seghp) ? &seghp->sp1r_function_code : NULL);
    return((seghp) ? seghp->sp1r_function_code : csp1_nil_fc);
} /* pa30GetFunctionCode */


API_RETCODE pa30SqlCommand( tpa41ESQBlock *api_cb_ptr,
                            tpa40DBC      *api_dbc_ptr,
                            UCHAR         *szSqlStr)
{
    API_RETCODE retcode;  
    tsp00_Int4  stmt_len;   
    SWORD try3 = 0;
  
    API_TRACE(API_TR_ENTRY, "pa30SQLCommand",0);
    retcode = API_OK;
    stmt_len = (tsp00_Int4) API_STRLEN(szSqlStr);
    do {
        try3++;
        p10statement( &api_cb_ptr->sqlca,
		      api_dbc_ptr->ConDesc->ga, 
                      &api_dbc_ptr->dbno,
                      &stmt_len,
                      (char*) szSqlStr, sp77encodingAscii);
    }
    while ( apereconn(api_cb_ptr, api_dbc_ptr) && try3 < PA_MAX_RECONNECT);
    aperetg (api_cb_ptr);
  
    API_TRACE(API_TR_EXIT, "pa30SQLCommand",0);
    API_TRACE(API_TR_API_RETCODE,"retcode",&retcode);
  
    return (retcode);
} /* pa30SqlCommand */


VOID pa30SenderID(UCHAR FAR *szSenderId, UWORD component)
{
    UCHAR FAR *szLocalStr={ (UCHAR*)
                            "CALCPCCOBCONDOMEASLOAODBOCIPLRQUESQLTPLUTIPLWXCI"};
    if(szSenderId) {
        API_MEMCPY(szSenderId+5, szLocalStr+(component*3), 3);
    }
} /* pa30SenderID */

void pa30bpcruntime(tpa40DBC  *dbc_block_ptr,
                    tpa60Stmt *stmt_block_ptr )
{  
    dbc_block_ptr->esqblk.sqlca.sqlrap->rasqlldp = stmt_block_ptr->rasqlldp;
    API_TRACE( API_TR_PTR,
               "rasqlldp",
               &dbc_block_ptr->esqblk.sqlca.sqlrap->rasqlldp );
    if (stmt_block_ptr->dbc_special.special == API_SPEC_NOT) {
        dbc_block_ptr->esqblk.sqlca.sqlrap->rakamode =
            stmt_block_ptr->stmtopt.sqlmode;
        dbc_block_ptr->esqblk.sqlca.sqlrap->rasqlansi =
            stmt_block_ptr->stmtopt.sqlmode;
        if (dbc_block_ptr->esqblk.sqlca.sqlrap->rasqlansi != CPR_KIND_ANSI) {
            dbc_block_ptr->esqblk.sqlca.sqlrap->rasqlansi = CPR_KIND_EMPTY;
        }
    }
    if (pa01UtilityConnect()) {
        struct SQLERROR *sqlemp=dbc_block_ptr->esqblk.sqlca.sqlemp;
        sqlratype * sqlrap=dbc_block_ptr->esqblk.sqlca.sqlrap;
        sqlgatype * sqlgap=dbc_block_ptr->esqblk.sqlca.sqlgap;
        p03initsqlem(sqlemp);
        p03sqlrelease(sqlrap, sqlgap, dbc_block_ptr->ConDesc->ga , sqlemp);	  
        p03sqlaconnect(sqlrap, sqlgap, dbc_block_ptr->ConDesc->ga, sqlemp);
    }
} /* pa30bpcruntime */


void pa30apcruntime( tpa40DBC  *dbc_block_ptr,
                     tpa60Stmt *stmt_block_ptr )
{
    stmt_block_ptr->rasqlldp = dbc_block_ptr->esqblk.sqlca.sqlrap->rasqlldp;
    API_TRACE(API_TR_PTR, "rasqlldp", &stmt_block_ptr->rasqlldp);
}


void pa30OraResultCount( tpa41ESQBlock *api_cb_ptr,
                         tpa60Stmt     *api_stmt_ptr )
{
    if (api_cb_ptr->sqlca.sqlrap->rakamode == CPR_KIND_ORACLE &&
        api_cb_ptr->sqlca.sqlcode == 0 &&
        api_stmt_ptr->stmt_function_code == csp1_select_fc &&
        api_cb_ptr->number_rows_modified == 0)
        api_cb_ptr->number_rows_modified = -1;
} /* pa30OraResultCount */


API_RETCODE pa30ResizeSQLDA( sqldatype **sqlda, UWORD arg_count )
{
    API_RETCODE api_retcode = API_OK;

    /* free and re-allocate */
    api_retcode = pa70FreeSQLDA( *sqlda );
    api_retcode = pa70AllocSQLDA( (API_HANDLE*) sqlda, arg_count );

    return api_retcode;
} /* pa30ResizeSQLDA */

/* replaces first character of a tpr05_String with blank
   (see FFETCH in apefetc) */
API_RETCODE pa30ReplaceFirstCharacterWithBlank (tpr05_String *str)
{
    static tsp81_UCS2Char blank;
    static int            init = 0;
    UCHAR          c = ' ';
    unsigned int   dummy;
    API_RETCODE    api_retcode = API_OK;

    if (init == 0) {
        init = 1;
        sp81ASCIItoUCS2 (&blank, 1,
                         sp77nativeUnicodeEncoding () == sp77encodingUCS2Swapped,
                         &dummy, &c, 1);
    }

    if (str->encodingType == sp77encodingAscii)
        *((char*) (str->rawString)) = ' ';
    else
        ((tsp81_UCS2Char*)(str->rawString))->s = blank.s;

    return api_retcode;
}  /* pa30ReplaceFirstCharacterWithBlank */
