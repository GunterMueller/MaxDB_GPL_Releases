/*!**********************************************************************

  module: vpa41ESQBlock.c

  -----------------------------------------------------------------------

  responsible:  

  special area: ODBC-Driver

  description:  struct tpa41ESQBlock

  see also:     http://www.microsoft.com/data/odbc/

  -----------------------------------------------------------------------

                         Copyright (c) 1998-2005 SAP AG




    ========== licence begin  GPL
    Copyright (c) 1998-2005 SAP AG

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




************************************************************************/

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include "vpa41ESQBlock.h"
#include "vpa30.h"

/* prototype */
void p08runtimeerror (sqlcatype*, sqlcxatype*, int);

/*!**********************************************************************

  Chapter: exported functions

************************************************************************/


				  /* error code conversion      */
				  /* table, this table is used  */
				  /* to convert ESQ errors to   */
				  /* ODBC errors.  ODBC errors  */
				  /* that are NOT ESQ derived,  */
				  /* are directly coded in the  */
				  /* functions.                 */

				  /* Note the -1111 entries in  */
				  /* this table are just place  */
				  /* holders.  Also this table  */
				  /* can be tricky to modify    */
				  /* since some ODBC functions  */
				  /* call other ODBC functions! */

/* a p i _ e r r o r _ t a b */
typedef struct {
  SWORD  stmt_type;
  SDWORD esq_error;
  SWORD  odbc_key;
} api_error_tab;

static api_error_tab convert_table [] = {
  /* odbc_stmt_type,      sqldb_err,          odbc_key  */
  /* ==============,      =======,          ========= */
  { API_ANY,                 API_ANY, API_ODBC_S1000 },
  { API_ANY,  PA_ERR_CONNECTION_DOWN, API_ODBC_08S01 }, /* 148 */
  { API_ANY,                    -102, API_ODBC_S1008 }, 
  { API_ANY,                    -813, API_ODBC_08S01 }, /* 148 */
  { API_ANY,                    -708, API_ODBC_08S01 }, /* 148 */
  { API_ANY,                    -709, API_ODBC_08S01 }, /* 148 */
  { API_ANY,                    -711, API_ODBC_08S01 }, /* 148 */
  { API_ANY,                    +700, API_ODBC_S1T00 }, 
  { API_ANY,			+500, API_ODBC_S1T00 },      
  { API_ANY,                   -3008, API_ODBC_37000 },
  { API_ANY,                   -4023, API_ODBC_37000 },
  { API_ANY,                   -3010, API_ODBC_22005 },
  { API_ANY,                   -8004, API_ODBC_22005 },
  { API_ANY,                   -3019, API_ODBC_22003 },
  { API_ANY,                   -7006, API_ODBC_37000 },
  { API_ANY,                   -3005, API_ODBC_37000 },
  { API_ANY,                   -2000, API_ODBC_37000 },
  { API_ANY,                   -5015, API_ODBC_37000 },
  { API_ANY,                   -3014, API_ODBC_37000 },
  { API_ANY,                   -5010, API_ODBC_37000 }, /* PTS 1105364 */
  { API_ANY,                   -5001, API_ODBC_42000 },
  { API_ANY,                   -4005, API_ODBC_S0022 },
  { API_ANY,                    -743, API_ODBC_01004 },
  { API_ANY,                   +4011, API_ODBC_S0012 },
  { API_ANY,                    -751, API_ODBC_22003 },
  { API_ANY,                    -811, API_ODBC_22003 },
  { API_ANY,                     +70, API_ODBC_S1T00 },
  { API_ANY,                    +700, API_ODBC_S1T00 },
  { API_ANY,		       -6000, API_ODBC_S0001 }, 
  { API_ANY,		       -6008, API_ODBC_S0011 },
  /* return General Error (S1000) for -6006,-955 'Duplicate Name'
   * PTS 1104401 */
  { API_ANY,		       -6006, API_ODBC_S1000 },
  { API_ANY,                   -955,  API_ODBC_S1000 }, /* oracle */
  { API_ANY,		       -4004, API_ODBC_S1000 },
  { API_ANY,		       -942,  API_ODBC_S1000 }, /* oracle */
  { API_ANY,			200,  API_ODBC_23000 },
  { API_ANY,			350,  API_ODBC_23000 },
  { API_ANY,		       -350,  API_ODBC_23000 },
  { API_ANY,		        250,  API_ODBC_23000 },
  { API_ANY,		       -250,  API_ODBC_23000 },
  { API_ANY,		       -706,  API_ODBC_37000 },
  { API_ANY,		       -9803, API_ODBC_S1000 },
  { SQL_API_SQLCOLUMNS,         -50,  API_ODBC_S1T00 },  /* ansi */    
  { SQL_API_SQLCOLUMNS,         -51,  API_ODBC_S1T00 },  /* oracle */
  { SQL_API_SQLCOLUMNS,         -913, API_ODBC_S1T00 },  /* db2 */
  { SQL_API_SQLCOLUMNS,        -3009, API_ODBC_S1C00 },
  { SQL_API_SQLCONNECT,        -9254, API_ODBC_08S01 }, /* 254 */
  { SQL_API_SQLCONNECT,         -807, API_ODBC_08001 },
  { SQL_API_SQLCONNECT,        -9807, API_ODBC_08001 },
  { SQL_API_SQLCONNECT,        -9808, API_ODBC_08001 },
  { SQL_API_SQLCONNECT,         -806, API_ODBC_08001 },
  { SQL_API_SQLCONNECT,         -813, API_ODBC_08001 },
  { SQL_API_SQLCONNECT,         -707, API_ODBC_08001 },
  { SQL_API_SQLCONNECT,         -708, API_ODBC_08001 },
  { SQL_API_SQLCONNECT,         -709, API_ODBC_08001 },
  { SQL_API_SQLCONNECT,         -711, API_ODBC_08001 },
  { SQL_API_SQLCONNECT,         -752, API_ODBC_08001 },
  { SQL_API_SQLCONNECT,         -756, API_ODBC_08001 },
  { SQL_API_SQLCONNECT,        -8002, API_ODBC_08002 },
  { SQL_API_SQLCONNECT,        -4008, API_ODBC_28000 },
  { SQL_API_SQLCONNECT,        -4002, API_ODBC_28000 },
  { SQL_API_SQLCONNECT,      API_ANY, API_ODBC_08001 },
  { SQL_API_SQLDESCRIBECOL,    -1111, API_ODBC_S1000 },
  { SQL_API_SQLDISCONNECT,     -1111, API_ODBC_S1000 },
  { SQL_API_SQLERROR,          -1111, API_ODBC_S1000 }, /* not used */
  { SQL_API_SQLEXECDIRECT,      -50,  API_ODBC_S1T00 }, /* oracle */
  { SQL_API_SQLEXECDIRECT,      -51,  API_ODBC_S1T00 }, /* ansi */
  { SQL_API_SQLCOLUMNS,         -913, API_ODBC_S1T00 },  /* db2 */
  { SQL_API_SQLEXECDIRECT,      -802, API_ODBC_22005 }, 
  { SQL_API_SQLEXECDIRECT,     -1200, API_ODBC_21S01 },
  { SQL_API_SQLEXECDIRECT,     -3016, API_ODBC_22005 }, /* PTS 1105364 */
  { SQL_API_SQLEXECDIRECT,     -3048, API_ODBC_22008 }, 
  { SQL_API_SQLEXECDIRECT,     -3049, API_ODBC_22008 }, 
  { SQL_API_SQLEXECDIRECT,     -3050, API_ODBC_22008 }, 
  { SQL_API_SQLEXECDIRECT,     -4004, API_ODBC_S0002 },
  { SQL_API_SQLEXECDIRECT,     -942,  API_ODBC_S0002 }, /* oracle */
  { SQL_API_SQLEXECDIRECT,     -7055, API_ODBC_S0011 },
  { SQL_API_SQLEXECDIRECT,     -8006, API_ODBC_23000 },
  { SQL_API_SQLEXECDIRECT,     -8020, API_ODBC_01000 },
  { SQL_API_SQLEXECUTE,        -8006, API_ODBC_23000 },
  { SQL_API_SQLEXECUTE,        -6301, API_ODBC_S0002 },
  { SQL_API_SQLEXECUTE,        -3048, API_ODBC_22008 }, 
  { SQL_API_SQLEXECUTE,        -3049, API_ODBC_22008 }, 
  { SQL_API_SQLEXECUTE,        -3050, API_ODBC_22008 }, 
  { SQL_API_SQLEXECUTE,        -3016, API_ODBC_22005 }, /* PTS 1105364 */
  { SQL_API_SQLEXECUTE,         -802, API_ODBC_22005 }, 
  { SQL_API_SQLEXECUTE,        -4004, API_ODBC_S0002 },
  { SQL_API_SQLEXECUTE,        -942,  API_ODBC_S0002 }, /* oracle */
  { SQL_API_SQLEXECUTE,	        350,  API_ODBC_23000 },
  { SQL_API_SQLFETCH,          -1111, API_ODBC_S1000 },
  { SQL_API_SQLFETCH,          -3005, API_ODBC_24000 },
  { SQL_API_SQLFETCH,          +700,  API_ODBC_S1T00 },
  { SQL_API_SQLFETCH,           +70,  API_ODBC_S1T00 },
  { SQL_API_SQLFREECONNECT,    -1111, API_ODBC_S1000 },
  { SQL_API_SQLFREEENV,        -1111, API_ODBC_S1000 },
  { SQL_API_SQLFREESTMT,       -1111, API_ODBC_S1000 },
  { SQL_API_SQLGETCURSORNAME,  -1111, API_ODBC_S1000 },
  { SQL_API_SQLGETDATA,        -802,  API_ODBC_22005 },
  { SQL_API_SQLNUMRESULTCOLS,  -1111, API_ODBC_S1000 },
  { SQL_API_SQLPREPARE,        -7055, API_ODBC_S0011 },
  { SQL_API_SQLPREPARE,        -4004, API_ODBC_S0002 },
  { SQL_API_SQLPREPARE,        -3048, API_ODBC_22008 }, /* PTS 1105364 */
  { SQL_API_SQLPREPARE,        -3016, API_ODBC_22005 }, /* PTS 1105364 */
  { SQL_API_SQLPREPARE,        -942,  API_ODBC_S0002 }, /* oracle */
  { SQL_API_SQLPREPARE,        -1200, API_ODBC_21S02 },
  { SQL_API_SQLPREPARE,        -1111, API_ODBC_S1000 },
  { SQL_API_SQLPREPARE,         -802, API_ODBC_22005 },      
  { SQL_API_SQLPUTDATA,         -802, API_ODBC_22005 },      
  { SQL_API_SQLSTATISTICS,   API_ANY, API_ODBC_S1C00 },
  { SQL_API_SQLROWCOUNT,       -1111, API_ODBC_S1000 },
  { SQL_API_SQLSETCURSORNAME,  -1111, API_ODBC_S1000 },
  { SQL_API_SQLSETPARAM,       -1111, API_ODBC_S1000 },
  { SQL_API_SQLSETPOS,         -802,  API_ODBC_22005 },
  { SQL_API_SQLTRANSACT,       -1111, API_ODBC_S1000 },
  { 0, 0, API_MARK_END}                   /* end-of-table */
}; /* convert_table */

UWORD pa41apmercv ( tpa41ESQBlock *esqblk_ptr )
{
    UWORD i;
    UWORD key;
    SWORD signed_key;

    API_TRACE(API_TR_ENTRY,"pa41apmercv",0);
    API_TRACE(API_TR_SDWORD,"esq_sql_code",
              &(esqblk_ptr -> esq_sql_code));
    API_TRACE(API_TR_UWORD,"odbc_function_code",
              &(esqblk_ptr -> odbc_function_code));

    signed_key = -1;
    /* first, look for a match    */
    /* on both stmt_type and the  */
    /* esq_err                    */
    i = 0;
    while ( convert_table [i] . odbc_key != API_MARK_END) {

        if (convert_table [i] . stmt_type ==
            (SWORD) esqblk_ptr -> odbc_function_code  &&
            convert_table [i] . esq_error ==
            esqblk_ptr -> esq_sql_code) {
            signed_key = convert_table [i] . odbc_key;
            break;
        }
        i++;
    }

    /* second, look for a match   */
    /* on esq_err = API_ANY and   */
    /* the given stmt_type        */
    if (signed_key == -1) {
        i = 0;
        while ( convert_table [i] . odbc_key != API_MARK_END) {
            if ((convert_table [i] . stmt_type ==
                 (SWORD)esqblk_ptr -> odbc_function_code)  &&
                (convert_table [i] . esq_error == API_ANY )) {
                signed_key = convert_table [i] . odbc_key;
                break;
            }
            i++;
        }
    }
    /* third, look for a match    */
    /* on esq_err and stmt_type = */
    /* API_ANY                    */
    if (signed_key == -1) {
        i = 0;
        while ( convert_table [i] . odbc_key != API_MARK_END) {
            if ((convert_table [i] . stmt_type == API_ANY) &&
                (convert_table [i] . esq_error ==
                 esqblk_ptr -> esq_sql_code)) {
                signed_key = convert_table [i] . odbc_key;
                break;
            }
            i++;
        }
    }
    /* forth, look for a match    */
    /* on stmt_type = API_ANY and */
    /* esq_err = API_ANY          */
    if (signed_key == -1) {
        i = 0;
        while ( convert_table [i] . odbc_key != API_MARK_END) {
            if (convert_table [i] . stmt_type == API_ANY &&
                convert_table [i] . esq_error == API_ANY) {
                signed_key = convert_table [i] . odbc_key;
                break;
            }
            i++;
        }
    }

    if (signed_key == 9999) {         /* this is not possible!!!!   */
        API_TRACE(API_TR_MSG," ERROR - possible table overwrite!!!",0);
    }

    key = signed_key;

    API_TRACE(API_TR_EXIT,"pa41apmercv",0);
    API_TRACE(API_TR_UWORD,"sqlkey",&key);

    return (key);
} /* pa41apmercv */



#define PA70SQLODBCERR(x) \
	API_STRNCPY(sqlcap->sqlerrmc, API_SQLTEXT_##x, sizeof(API_SQLTEXT_##x)-1);\
	sqlcap->sqlerrml = sizeof(API_SQLTEXT_##x)-1;\
	sqlcap->sqlcode  = API_SQLODBC_##x;\
	aperetg (api_cb_ptr);

API_RETCODE pa41apmmker( SWORD          code,
                         tpa41ESQBlock *api_cb_ptr)
     
{
    API_RETCODE api_retcode;
    sqlcxatype *sqlcxap;
    sqlcatype *sqlcap;

    API_TRACE(API_TR_ENTRY,"pa41apmmker",0);
    API_TRACE(API_TR_SWORD,"code",&code);
    api_retcode = API_OK;
    sqlcap = &api_cb_ptr->sqlca;
    switch (code) {
    case API_SQLODBC_22000: {
        PA70SQLODBCERR(22000);
        break;
    }
    case API_SQLODBC_22001: {
        PA70SQLODBCERR(22001);
        break;
    }
    case API_SQLODBC_22002: {
        PA70SQLODBCERR(22002);
        break;
    }
    case API_SQLODBC_22003: {
        PA70SQLODBCERR(22003);
        break;
    }
    case API_SQLODBC_22004: {
        PA70SQLODBCERR(22004);
        break;
    }
    case API_SQLODBC_22005: {
        PA70SQLODBCERR(22005);
        break;
    }
    case API_SQLRUNTIME_9803: {
        sqlcxap = api_cb_ptr->sqlca.sqlcxap;
        p08runtimeerror (sqlcap, sqlcxap, cpr_unknown_datatype);
        aperetg (api_cb_ptr);   
        break;
    }
    case API_SQLRUNTIME_3008: {
        API_STRNCPY( sqlcap->sqlerrmc,
                     API_SQLTEXT_3008,
                     sizeof(API_SQLTEXT_3008)-1);
        sqlcap->sqlerrml = sizeof(API_SQLTEXT_3008)-1;
        sqlcap->sqlcode  = API_SQLRUNTIME_3008; 
        aperetg (api_cb_ptr);   
        break;
    }
    default: { 
        PA70SQLODBCERR(899);
        api_retcode = PA_ROW_NOT_FOUND;
        break;
    }
    }
    API_TRACE(API_TR_EXIT,"apmmker", 0);
    API_TRACE(API_TR_API_RETCODE, "api_retcode", &api_retcode);

    return(api_retcode);
} /* pa41apmmker */



/*!**********************************************************************

  EndChapter: exported functions

************************************************************************/
