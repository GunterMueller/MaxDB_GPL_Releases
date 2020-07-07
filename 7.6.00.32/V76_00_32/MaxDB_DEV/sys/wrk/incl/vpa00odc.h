#ifdef DEBUG
#else
#line 84 "vpa00odc"
#endif
#define OLDTASK
#ifndef REL313
#define REL313
#endif
#ifdef CRYPTIC_NAMES

#include <apcrypt.h>

#endif

#ifndef SQLODBC
#include <sql.h>                  /* actual ODBC core-level     */
				  /* header file that needs the */
				  /* <windows.h> style macros.  */

#include <sqlext.h>               /* actual ODBC level 1&2      */
				  /* header file that also needs*/
				  /* the <windows.h> things.    */

#include <aptext.h>               /* get the ODBC error codes & */
				  /* text messages.             */
#endif /* SQLODBC */
				/* macro to read out the byte     */
				/* specified via the parameter    */
				/* GETBYTE_IND (may be 0, 1, ..)  */
				/* from the structured variable   */
				/* specified via the adress of    */
				/* the variable defined in the    */
				/* parameter GETBYTE_ADR          */
				/* using the cast-operation the   */
				/* value of the variable is chang.*/
				/* into a char string using the   */
				/* vector arithmetic it is possib.*/
				/* to access the variable byte by */
				/* byte                           */
#define GETBYTE(GETBYTE_ADR, GETBYTE_IND) \
	       (((UCHAR *) (GETBYTE_ADR)) [(GETBYTE_IND)] )
				/* check if value is odd */
#define ODD(VALUE)          (VALUE & 1)
#define HIPART              (0xf0)
#define LOPART              (0x0f)

#ifndef SQLODBC
#ifdef WIN
#include <apdlg.h>                /* for windows dialog boxes */
#endif
#endif /* SQLODBC */
/* this macro is used to get a C++ assert function for making   */
/* sure ("asserting") that pointers are non-null                */
    
/* trace macros              */
#ifndef SAPDB_FAST
extern SWORD api_trace_status;
#define API_TRACE_INIT(status)\
	  {api_trace_status = status;\
	     apdtrac(0, 0, NULL, (SDWORD) 0, (SDWORD) 0);}
#define API_TRACE(type,name,ptr)\
	  {if (api_trace_status == 1)\
	     apdtrac(type, (UCHAR *) name, (VOID *) ptr, (SDWORD) 0, (SDWORD) 0);}
#define API_TRACE_LEN(type,name,ptr,len)\
	  {if (api_trace_status == 1)\
	     apdtrac(type, (UCHAR *) name, (VOID *) ptr, (SDWORD) len, (SDWORD) 0);}
#define API_TRACE_BUF(ptr,pos,len)\
	  {if (api_trace_status == 1)\
	     apdtrac(API_TR_BUFFER, NULL, (VOID *) ptr, (SDWORD) len, (SDWORD) pos);}
#else           
#define API_TRACE_INIT(status)
#define API_TRACE(type,name,ptr)
#define API_TRACE_LEN(type,name,ptr,len)
#define API_TRACE_BUF(ptr,pos,len)
#endif

#define API_SQL_TXN_EMPTY (0)
#define SQL_HIDDEN_HDBC 1
#define SQL_INFO_HIDDEN_TRACE SQL_INFO_LAST + 1 
#define SQL_INFO_HIDDEN_LAST SQL_INFO_HIDDEN_TRACE + 1

VOID SQL_API apdstrc(
#ifdef PROTOTYPE
      SWORD,  
      UCHAR * 
#endif   
);
				  /* trace type codes          */
#define API_TR_PROFENTRY "odbctrace"

#define API_TR_TPR05STRING (97)
#define API_TR_WSTRING     (98)
#define API_TR_STRING  (1)
#define API_TR_UCHAR   (2)
#define API_TR_SCHAR   (3)
#define API_TR_DWORD   (4)
#define API_TR_SDWORD  (5)
#define API_TR_UDWORD  (6)
#define API_TR_WORD    (7)
#define API_TR_SWORD   (8)
#define API_TR_UWORD   (9)
#define API_TR_SDOUBLE (10)
#define API_TR_LDOUBLE (11)
#define API_TR_SFLOAT  (12)
#define API_TR_PTR     (13)
#define API_TR_BOOL    API_TR_WORD

#define API_TR_HANDLE  (30)
#define API_TR_RETCODE (31)
#define API_TR_SQLSTATE (32)
#define API_TR_API_ESQ_BLOCK (33)
#define API_TR_API_RETCODE (34)
#define API_TR_ODBC_STRING (35)
#define API_TR_BUFFER (36)
#define API_TR_GETINFO (37)
#define API_TR_SFINFOTYPE (38)
#define API_TR_HWND (39)
#define API_TR_PARSID (40)

#define API_TR_ENTRY   (50)
#define API_TR_EXIT    (51)
#define API_TR_MSG     (52)
#define API_TR_LASTERROR  (53)

#define API_TR_COLATTR    (54)
#define API_TR_DESCTYPE   (55)
#define API_TR_SQLTYPE    (56)
#define API_TR_CTYPE      (57)
#define API_TR_STMTATTR   (58)
#define API_TR_ENVATTR    (59)
#define API_TR_CONNATTR   (60)
#define API_TR_UPDATEABLE (61)
#define API_TR_DIAGFIELD  (62)
#define API_TR_CURSORTYPE (63)
#define API_TR_FETCHTYPE  (64)
#define API_TR_OPTYPE     (65)

/* structure type codes       */
#define API_INVALID_HANDLE_TYPE (0)
#define API_ENV_TYPE     (1)
#define API_DBC_TYPE     (2)
#define API_STMT_TYPE    (3)
#define API_DESC_TYPE    (4)

#define SQL_TYPE_EMPTY (99)
#define SQL_TYPE_NOTFOUND (100)


#define API_SQL_NO_TOTAL SQL_NO_TOTAL
#define API_LONG_MAX (-1L)
#define API_VALUEMAX (-2L)
#define API_VALUEMAX_PLUSONE (-3L)

				  /* lengths, names, etc.       */
#define API_MESSAGE_MAX_LEN (91)
#define API_CONNECT_STR_MAX_LEN (32000)
#define API_ACTIVESTATEMENTS 32767
#define API_DATA_SOURCE_MAX_LEN (128)
#define PA_MAX_DATABASE_LEN (64)
#define PA_PC_TRACE_FILE  "sqltrace"  
#define PA_FILE_NAME_LEN  64  
#define PA_NO_VALID_LEN  -1  
#define PA_NO_FREE_LONG 0

#ifdef SQLODBC
#define API_SERVER_NAME API_PROD_NAME
#endif
#define API_LONGVAR_BUF_LEN 4000      
#define API_CHAR_LENGTH_LONG (12)
#define API_CHAR_LENGTH_CHAR (255)
#define API_CHAR_LENGTH_DECIMAL (19)
#define API_SQL_STRING_SWAG (512)  /* swag size of generate SQL */
				   /* size                      */
				   /* swag = scientific         */
				   /*          wild             */
				   /*            *ss            */
				   /*              guess        */

				  /* lengths, names, etc.  *** ODBC 2.0 ** */

/* SQLDriverConnect terminal   */
/* symbols                     */
#define API_DRIVERCONN_DSNAME    "DSN"
#define API_DRIVERCONN_USERID    "UID"
#define API_DRIVERCONN_PASSWD    "PWD"
#define API_DRIVERCONN_SERVERDB  "SERVERDB"
#define API_DRIVERCONN_SERVERNODE "SERVERNODE"
#define API_DRIVERCONN_DRIVERNAME "DRIVER"       /* PTS 1108230 */
#define API_DRIVERCONN_EQUAL     "="
#define API_DRIVERCONN_SEPERATOR ";"
#define PA_MAXLOGIN_COUNT        3

/* SQLBrowseConnect symbols  */
#define API_BROWSECONN_QUESTION		"?"
#define API_BROWSECONN_BRACE_OP 	"{"
#define API_BROWSECONN_BRACE_CL 	"}"
#define API_BROWSECONN_COMMA		','

#define API_ESQ_EOF   (100l)      /* end of set/eof             */


				  /* dbc/stmt status codes      */
				  /* see appendix B in ODBC ref */

#define API_DBC_UNALLOCATED (0)   /* connection   : unallocated */
#define API_DBC_ALLOCATED (2)     /* connection S2: allocated   */
#define API_DBC_CONNECTED (3)     /* connection S3: connected   */

/* dbc trans. status codes    */
#define API_DBC_INFLIGHT (1)      /* transaction active         */
#define API_DBC_LANDED   (2)      /* transaction commited or    */
				  /* rolled back                */

#define API_TRUE  (1)             /* deep meaning, the truth    */
#define API_FALSE (0)             /* blaspheme, untruth         */

#define API_RETCODE  RETCODE

#define API_SQLDA_NUM (20)        /* estimated SQLDA elements,  */
                                  /* if too low, number of      */
                                  /* sqldatype elements is      */
                                  /* enlarged dynamically       */

#define API_BIND_NUM  (5)         /* estimated bind elements,   */
				  /* (set a bit low since get-  */
				  /* ting more blocks is cheap).*/

#define API_BIND_INC  (0)         /* increment for overflow of  */
				  /* bind elements              */

#define API_ODBC_CURSOR_LEN (MX_IDENTIFIER)
#define API_SQLDB_PARSID_LEN (16)
#define API_SQLDB_PARSID_NULL "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
				  /* precision for numeric data */
#define API_BIT_PREC      (1)     /* types                      */
#define API_TINYINT_PREC  (3)
#define API_SMALLINT_PREC (5)
#define API_INTEGER_PREC  (10)
#define API_BIGINT_PREC   (20)
#define API_REAL_PREC     (7)
#define API_FLOAT_PREC    (15)
#define API_DOUBLE_PREC   (18)
#define API_DATE_PREC     (10)
#define API_TIME_PREC     (8)
#define API_TIMESTAMP_PREC (19)
#define API_TIMESTAMP_EXT_PREC (26)
#define API_VSTRING_OFFSET 4
#define API_MAX_LONGVARCHAR 65002 - API_VSTRING_OFFSET

/* VDN number restrictions */
#include <float.h>
#define API_MAXDOUBLE_EXP 308
#define API_MAXDOUBLE DBL_MAX
#define API_MINDOUBLE DBL_MIN

#define API_MAXFLOAT_EXP 38
#define API_MAXFLOAT  3.4E+38
#define API_MINFLOAT  3.4E-38

#define PA_DATE_SEP      '-'
#define PA_TIME_SEP      ':'
#define PA_TIMESTAMP_SEP ' '
#define PA_NODE_SEP      ':'
#define PA_DATABASE_SEP  ';'
#define API_ESCAPE_CHAR  '\\'
#define PA_FRAC_SEP '.'

#define SP1PK_TABLENAME SP1PK_FUTURE_USE2
#define SP1PK_SESSION_INFO_RETURNED SP1PK_FUTURE_USE3
#define SP1M_STATE_UTILITY SP1M_SFILL1

enum CSP_COMP {COMP_CALL=0, COMP_CPC, COMP_COBPC, COMP_CONTROL, COMP_DOMAIN, 
COMP_EASY, COMP_LOAD, COMP_ODBC, COMP_OCI, COMP_PLRUN, COMP_QUERY, COMP_SQLPL, 
COMP_TPLRUN, COMP_UTIL, COMP_WB, COMP_XCI};

				  /* length for numeric data    */
#define API_BIT_LEN       (1)     /* types                      */
#define API_TINYINT_LEN   (1)
#define API_SMALLINT_LEN  (2)
#define API_INTEGER_LEN   (4)
#define API_BIGINT_LEN    (20)
#define API_REAL_LEN      (4)
#define API_FLOAT_LEN     (8)
#define API_DOUBLE_LEN    (8)
#define API_DATE_LEN      (6)
#define API_TIME_LEN      (6)
#define API_TIMESTMP_LEN  (16)

				  /* special processing values  */
				  /* the ESQ catalog does not   */
				  /* carry the information in an*/
				  /* ODBC friendly way -- the   */
				  /* special processing in      */
				  /* SQLGetData() handles this  */

#define API_SPEC_NOT        (1)   /* no special processing      */
#define API_SPEC_SQLTABLES  (2)   /* special SQLTables()        */
#define API_SPEC_SQLCOLUMNS (3)   /* special SQLColumns()       */
#define API_SPEC_SQLSTATISTICS (4) /* special SQLStatictics     */
#define API_SPEC_SQLGETTYPEINFO (5) /* special SQLGettypeinfo   */
#define API_SPEC_SQLSPECIALCOLUMNS (6) /* special SQLSpecialColumns */
#define API_SPEC_SQLPROCEDURES (7) /* special SQLProcedures */
#define API_SPEC_SQLPROCEDURESCOLUMNS (8) /* special SQLProceduresColumns */
#define API_SPEC_SQLCOLUMNPRIVILEGES (9) /* special SQLColumnPrivileges */
#define API_SPEC_SQLFOREIGNKEYS (10) /* special SQLForeignKeys */
#define API_SPEC_SQLPRIMARYKEYS (11) /* special SQLPrimaryKeys */
#define API_SPEC_SQLTABLEPRIVILEGES (12) /* special SQLTablePrivileges */
#define API_SPEC_SQLSETPOS (13)      /* special SQLSetPos */
#define API_SPEC_SQLEXTENDEDFETCH (14)      /* special SQLExtendedFetch */

				  /* codes to identify the ddl  */
				  /* statements that need extra */
				  /* sql statements executed.   */
#define API_CREATE_TABLE_STMT (1) /* create table found         */
#define API_DROP_TABLE_STMT   (2) /* drop table found           */
#define API_OTHER_STMT        (3) /* non create/delete found    */

				  /* values for keyword search in */
				  /* the sql_stmt                 */
#define API_MAX_KEYWORD_LEN    (18)
#define API_KEYWORD_SELECT     (1) 
#define API_KEYWORD_INSERT     (2)
#define API_KEYWORD_UPDATE     (3)
#define API_KEYWORD_DELETE     (4)
#define API_KEYWORD_CREATE     (5)
#define API_KEYWORD_DROP       (6)
#define API_KEYWORD_SHOW       (7)
#define API_KEYWORD_DECLARE    (8)
#define API_KEYWORD_PROC       (9)
#define API_KEYWORD_FETCH      (10)
#define API_KEYWORD_OPEN       (11)
#define API_KEYWORD_CLOSE      (12)
#define API_KEYWORD_SET        (13)
#define API_KEYWORD_NEXT       (14)
#define API_KEYWORD_APPEND     (15)

#define API_KEYWORD_NOT_FOUND  (99)

#define PA_OK                        (0)
#define PA_ROW_NOT_FOUND           (100)
#define PA_ERR_SERVERDB_NOT_AVAILABLE (-708)
#define PA_ERR_TOO_MANY_HOSTVAR   (-712)
#define PA_ERR_CONNECTION_DOWN    (-807)
#define PA_ERR_UNKNOWN_TABLENAME (-4004)
#define PA_UPDATE_NOT_ALLOWED (-7008)
#define PA_ERR_NOT_BASE_TABLE    (-8015)
#define PA_ERR_MISSING_PRIVILEGE (-5001)
#define PA_ERR_UNKNOWN_RESULT_TABLE (-4000)
#define PA_ERROR_TEXT_LEN           (100)
				  /* values for order interface */
				  /* used to send commands in   */
				  /* different request types    */
				  /* depents form api_keyword_table */
#define API_CMD_EXECUTE         (1)
#define API_CMD_PARSE           (2)
#define API_CMD_DESCRIBE        (3)
#define API_CMD_ERROR           (99)
#define SYS_TRACE_CALL "ODBC Function :"
/* this macros build a commandstring in the form of "cmd res ..." */

#define SYSDBA "SYSDBA"
#define SYSDOMAIN "DOMAIN"

#define SYS_CMD_COMMIT_WORK "COMMIT WORK"
#define SYS_CMD_ROLLBACK_WORK "ROLLBACK WORK"
#define SYS_CMD_DECLARE_CURSOR "DECLARE \""
#define SYS_CMD_CONNECT "CONNECT ? IDENTIFIED BY ? %s ISOLATION LEVEL %d"
#define SYS_CMD_CURSOR_FOR "\" CURSOR FOR "
#define SYS_CMD_FETCH "FETCH "
#define SYS_CMD_USING_DESCRIPTOR "\" USING DESCRIPTOR"
#define SYS_CMD_CLOSE "CLOSE \""
#define SYS_CMD_SET_FORMAT "SET FORMAT JIS"
#define SYS_CMD_INTO "\" INTO ?"
#define SYS_CMD_PARAM ", ?"
#define SYS_CMD_FOR_REUSE " FOR REUSE"
#define SYS_CMD_FOR_UPDATE " FOR UPDATE OF"

extern UCHAR *PA07_SQLMODE_ORACLE;
extern UCHAR *PA07_SQLMODE_INTERNAL;
extern UCHAR *PA07_SQLMODE_SAPR3;
extern UCHAR *PA07_SQLMODE_ANSI;
extern UCHAR *PA07_SQLMODE_DB2;

#define PA_MAX_RECONNECT 3

#define SYS_TABLE_COLUMNS          "SYS_ODBC_COLUMNS"
#define SYS_TABLE_TABLES           "SYS_ODBC_TABLES"
#define SYS_TABLE_STAT             "SYS_ODBC_STAT"
#define SYS_TABLE_SPECIAL          "SYS_ODBC_SPEC_COL"
#define SYS_TABLE_TYPES            "SYSODBCTYPES"
#define SYS_TABLE_PROC             "SYS_ODBC_PROC"
#define SYS_SYS_TABLES             "SYS_SYS_TABLES"

#define SYS_SHOW_COLUMNS          "SYS_ODBC_S_COLUMNS"
#define SYS_SHOW_TABLES           "SYS_ODBC_S_TABLES"
#define SYS_SHOW_STAT             "SYS_ODBC_S_STAT"
#define SYS_SHOW_STINDEX          "SYS_ODBC_S_STINDEX"
#define SYS_SHOW_INDEX            "SYS_ODBC_S_INDEX"
#define SYS_SHOW_SPECIAL          "SYS_ODBC_S_SPECCOL"
#define SYS_SHOW_TYPES            "SYS_ODBC_S_TYPES"
#define SYS_SHOW_USER             "SYS_ODBC_S_USER"

/* this macros build a tablename in the form of "owner.table" */
#define SYS_T_S(o,t) #o"."#t
#define SYS_TABLE_STRING(o, t) SYS_T_S(##o,##t)    

				  /* SQLTable scan types        */
#define API_EMPTY          (0)
#define API_TABLE_SCAN     (1)
#define API_QUALIFIER_SCAN (2)
#define API_OWNER_SCAN     (3)
#define API_TYPE_SCAN      (4)

				  /* SQLForeingKeys scan types */
#define API_PKTABLE_SCAN               (1)
#define API_FKTABLE_SCAN               (2)
#define API_PKTABLE_FKTABLE_SCAN       (3)

#define API_TABLE_NONE     (0x00000000)
#define API_TABLE_TABLE    (0x00000001)
#define API_TABLE_SYSTEM   (0x00000002)
#define API_TABLE_VIEW     (0x00000004)
#define API_TABLE_SYNONYM  (0x00000008)
#define API_TABLE_ALIAS    (0x00000010)
#define API_TABLE_RESULT   (0x00000020)
#define API_TABLE_UNLOADED (0x00000040)
#define API_TABLE_SNAPSHOT (0x00000080)
#define API_TABLE_SYSTEMTABLE   (0x00000100)
#define API_TABLE_ALL      (0xFFFFFFFF)

#define API_ATTRIB_MFETCH_ALLOWED	(0x00000001L)
#define API_ATTRIB_SELECT_FOR_UPDATE	(0x00000002L)
#define API_ATTRIB_PARSID_SET		(0x00000004L)
#define API_ATTRIB_FETCH_PARSID_SET	(0x00000008L)
#define API_ATTRIB_INIT              API_ATTRIB_MFETCH_ALLOWED

#define PA_PROC(x) if (pa80AnalyserFunctions.lp##x)\
			pa80AnalyserFunctions.lp##x                                                
typedef void *          API_HANDLE;
#define API_NULL_HANDLE     0
typedef void *          HERR;
 
/* a p i _ g l o b _ t a b */

typedef struct {
   HANDLE hTask;
   HWND   hWin;
   SWORD used_count;
   SWORD init_calls;
} api_global_tab;

#define MAX_TASK 8


/* a p i _ k e y w o r d _ t a b */
typedef struct {
  SWORD     entry;
  UCHAR     keyword  [ API_MAX_KEYWORD_LEN ];
  SDWORD    order_type_key;
  SQLWCHAR  keywordW [ API_MAX_KEYWORD_LEN ];    
} api_keyword_tab;

/* a p i _ t e x t _ t a b */

/* this struct is not used with WINDOWS, uses LoadString() */

typedef struct {
  UWORD key;
  UCHAR text [80];
}  api_text_tab;

typedef DWORD PACANCELPROC(DWORD reference);

#ifdef REL313  
#define sqlda sqlda
#else
#define sqlda SQLDA
#endif

/* a p i _ d b c _ s p e c i a l */
typedef struct {
  UWORD special;                  /* flag for special process-  */
  UDWORD blocksize;                /* flag for blocksize of LONGVARCHAR  */
  UWORD kernelversion;            /* version of db kernel    */
  SWORD last_error;
  SDWORD  order_type;
} api_dbc_special;

/* a p i _ r o w _ s t a t u s _ e n t r y */
typedef struct {
  UCHAR status;
} api_row_status_entry;

/* a p i _ r o w _ s t a t u s */
typedef struct {
  UDWORD numentries;
  api_row_status_entry entry[1];
} api_row_status;

typedef struct {
  SWORD usedcount;
  sqltatype sqlta;
} api_trace_block;

typedef struct {
  SWORD utility_session;
  UWORD  component;               /* Senderid-constants of the separate Components */
  UWORD  service;                 /* Service for sqlaconnect */
} api_utility_block;


/* a p i _ e s q _ b l o c k */
typedef struct {
  SDWORD esq_sql_code;            /* SQL code from ESQ          */
  UCHAR  sql_err_text [ PA_ERROR_TEXT_LEN + 1];
  UDWORD number_rows_modified;  /* number of rows touched   */
  UWORD  odbc_function_code;      /* ODBC ordinal               */
  sqlcatype sqlca;
  api_trace_block *tracep;
  void *sqlareas;
} api_esq_block;

#ifdef ASYNCENABLED
/* a p i _ t h r e a d _ b l o c k */
typedef struct {
  teo07_ThreadId  ThreadId;
  teo07_Mutex     cs;
} api_thread_block;
#endif

#ifdef ASYNCENABLED

#define THR_OK (0)
#define THR_NOT_OK (1)
#define THR_NO_DATA_FOUND (100)

#define THR_CREATE_NORMAL    (0x00)
#define THR_CREATE_SUSPENDED (0x01)

#endif


#define PA_ENV_SQLCODE (dbc_block_ptr->esqblk.esq_sql_code)

extern UWORD cross_check;         /* see comments in apcorea.c  */
				  /* data source name from the  */
				  /* SQLOConnect().              */

/* internal function delcarations */

				  /* in apdep.c <============== */
API_HANDLE apdallo (                  /* allocate memory            */
#ifdef PROTOTYPE
       DWORD
#endif
       );
VOID   apdfree (                  /* free allocated memory      */
#ifdef PROTOTYPE
       API_HANDLE
#endif
       );

/* lock and unlock are no longer needed for WIN32 */
#define apdlock(h) (h)
#define apdunlk(h) (h)

API_RETCODE apmfstr(API_HANDLE hand);

API_RETCODE apmrstr( API_HANDLE *phand,
                     UWORD       old_stmt_len,
                     UWORD       new_stmt_len);

VOID   apmunlp (                  /* unlock the parents if there are not null */
#ifdef PROTOTYPE
	    API_HANDLE,
	    API_HANDLE,
	    API_HANDLE
#endif
       );

VOID   apdtrac  (                 /* trace routine              */
#ifdef PROTOTYPE
       UWORD,
       UCHAR *,
       VOID *,
       SDWORD,
       SDWORD
#endif
       );
    
API_RETCODE apdgdbn( 
#ifdef PROTOTYPE
tpr05_String * DSN,
UCHAR * serverdb_ptr, 
SWORD serverdb_len,
UCHAR * servernode_ptr, 
SWORD servernode_len
#endif
);
    
VOID   apdsnap (                  /* snap dump of env/dbc/stmt  */
				  /* structure                  */
#ifdef PROTOTYPE
       HENV
#endif
       );

API_RETCODE apdchkm();            /* check memory table         */

VOID pa08flt(
#ifdef PROTOTYPE
UCHAR *string_ptr, SDOUBLE number, UWORD format
#endif
);

void pa08rmspace(
#ifdef PROTOTYPE
UCHAR *s, int n
#endif
);

VOID   apdhex (                   
#ifdef PROTOTYPE
       UCHAR *,
       SDWORD,
       UCHAR *,
       SDWORD
#endif
       );

VOID   apdtbuf (                   
#ifdef PROTOTYPE
  UCHAR *,
  SDWORD,
  SDWORD
#endif
       );

API_RETCODE apdncnnt( 
#ifdef PROTOTYPE
      HDBC
#endif
);

WORD FAR PCALL SendAHello(
#ifdef PROTOTYPE
	 HWND,
	 UINT,
	 int,
	 DWORD
#endif
);

#ifdef WIN
#include "hsp77.h"
UINT apduser(
#ifdef PROTOTYPE
tpr05_String * szDSNStr,
tpr05_String * useridStr,
tpr05_String * passwdStr,
UCHAR * serverdb_ptr, SWORD serverdb_len,
UCHAR * servernode_ptr, SWORD servernode_len,
HWND       hwnd1,
const tsp77encoding *destEncoding,
char *encrypt
#endif
);
#endif

UCHAR * apdnear (                 /* convert far to near ptr    */
#ifdef PROTOTYPE
                 UCHAR * far_pointer
#endif
                 );

VOID pa07FreeTLS() ;
UCHAR *pa07GetTraceFileName();
void pa07SetTraceFileName(UCHAR *ptr);
int pa07TraceSwitch(SWORD Level);
SWORD pa07GetTraceLevel();
#ifdef WIN32
HWND pa07GetODBCTaskWindow(HTASK hCurr);
#endif

				  /* in apmisc.c <============= */
	  
API_RETCODE apmwtos (              /* ODBC string to C string   */
#ifdef PROTOTYPE
		     UCHAR *, SWORD,
		     UCHAR *, SWORD
#endif
		    );

API_RETCODE apmwchs ( 
#ifdef PROTOTYPE
  UCHAR * wstring,
  SWORD wlen,
  SWORD *len
#endif
		    );


API_RETCODE apmstow (              /* C string to ODBC string   */
#ifdef PROTOTYPE
		      UCHAR *, UCHAR *, SWORD, SWORD *
#endif
		    );

UWORD apmercv (                  /* error code converter      */
#ifdef PROTOTYPE
		 api_esq_block *
#endif
		);
	     
/* PROTOTYPES from VPA06C */

#ifdef ASYNCENABLED
/* Defines and Prototypes for VPA30WC */
#define PA30THREADSAVE
#define PA30PROTECTTHREAD(); pa09ProtectThread();
#define PA30UNPROTECTTHREAD(); pa09UnProtectThread();
#else
#define PA30PROTECTTHREAD();
#define PA30UNPROTECTTHREAD();
#endif

/* Prototypes from VPA04GC */
#ifdef PROTOTYPE
VOID apgpkst (UCHAR *packed_ptr, UWORD scale, UDWORD *length_ptr, UCHAR *decimal_ptr);
API_RETCODE apgstst (UCHAR * in_string_ptr, SDWORD in_length, SWORD in_format,
UCHAR * out_string_ptr, SDWORD out_length, SWORD out_format, UDWORD *used);
API_RETCODE apgstdt( UCHAR * in_string_ptr, SWORD in_format, VOID * out_ptr,
SWORD out_format, UDWORD *used);
API_RETCODE apgetoc(  SWORD esq_type, UCHAR * esq_ptr, UDWORD esq_len, UWORD esq_scale, UDWORD * moved_len_ptr,
SWORD c_type, UWORD numericScale, UCHAR * c_ptr, SDWORD c_len, SDWORD * c_used_ptr);
API_RETCODE apgctoe( SWORD esq_type, UCHAR * esq_ptr, UDWORD esq_len, UWORD esq_scale, UDWORD * moved_len_ptr,
SWORD c_type, UCHAR * c_ptr, UDWORD input_sql_len, UWORD input_sql_scale,
SQLLEN * c_used_ptr, SWORD input_sql_type);
API_RETCODE apgstpk( UCHAR *decimal_ptr, UWORD scale, UDWORD length, UCHAR *packed_ptr);
API_RETCODE apgstys( SWORD esq_type, UDWORD esq_len, UWORD esq_scale, SWORD * sql_type, UDWORD * sql_len, UWORD *scale);
VOID apgstyc( SWORD c_type, SQLULEN c_len, SWORD sql_type, UDWORD sql_len, UWORD sql_scale, SWORD * real_c_type, SQLULEN * real_c_len);

API_RETCODE apgchdt(UCHAR *string_ptr);
API_RETCODE apgchtm(UCHAR *string_ptr);
API_RETCODE apgchts(UCHAR *string_ptr);
BOOL apgislong (SWORD sql_type);
BOOL apgisnum (SWORD sql_type);
BOOL apgischar (SWORD sql_type);
BOOL apgisbin (SWORD sql_type);
BOOL apgistime (SWORD sql_type);
BOOL apgisvstr (SWORD sql_type);
API_RETCODE apgstoc( SWORD sql_type, UCHAR * sql_ptr, UDWORD sql_len, UWORD scale, UDWORD * moved_len_ptr,
SWORD c_type, UWORD numericScale, UCHAR * c_ptr, SDWORD c_len, SDWORD * c_used_ptr);
API_RETCODE apgsti2( UCHAR *in_string, UDWORD in_length, UCHAR * out_ptr, SWORD out_format);
API_RETCODE apgsti4( UCHAR *in_string, UDWORD in_length, UCHAR * out_ptr, SWORD out_format);
API_RETCODE apgsti8( UCHAR *in_string, UDWORD in_length, UCHAR * out_ptr, SWORD out_format);
API_RETCODE apgstfd( UCHAR *in_string, UDWORD in_length, UCHAR * out_ptr, SWORD out_format);
VOID apgsvstrl(UCHAR * ptr, tsp00_Int4 len);
UDWORD apggvstrl(UCHAR * ptr);
UCHAR * apgvstraddr(UCHAR * ptr);
VOID pa04gpatchdate(UCHAR *str_ptr);
VOID pa04gpatchnumber(UCHAR *str_ptr);
BOOL apgisindi(tsp00_Int4 *ind_ptr);
API_RETCODE apgsti1( UCHAR *in_string, UDWORD in_length, UCHAR * out_ptr, SWORD out_format);
#else            
VOID apgpkst ();                 
API_RETCODE apgstst ();
API_RETCODE apgstdt ();
API_RETCODE apgctoe ();
API_RETCODE apgstpk ();
API_RETCODE apgstys ();
VOID apgstyc ();
API_RETCODE apgstyh ();
API_RETCODE apgchdt ();
API_RETCODE apgchtm ();
API_RETCODE apgchts ();
BOOL apgisvstr ();
BOOL apgisnum ();
BOOL apgischar ();
BOOL apgisbin ();
BOOL apgistime ();
BOOL apgislong ();
API_RETCODE apgstoc();
VOID apgsvstrl();
UDWORD apggvstrl();
UCHAR * apgvstraddr();
API_RETCODE apgmove();
API_RETCODE apgpopu();
UDWORD apgbsiz();
API_RETCODE apgset();
VOID apgunset();
API_RETCODE apgput();
API_RETCODE apgparm();
VOID pa04gpatchdate();
VOID pa04gpatchnumber();
API_RETCODE pa04gglad();
API_RETCODE pa04gparm();
BOOL apgisindi();
API_RETCODE apgmovel();
API_RETCODE apegetl ();
API_RETCODE apgsti1();
#endif

/* Prototypes from VPA04FC */
#ifdef PROTOTYPE
API_RETCODE apfbinary (UCHAR * in_string_ptr, UDWORD in_length, UDWORD * moved_len_ptr, SWORD in_format,
UCHAR * out_string_ptr, UDWORD out_length, SWORD out_format, UDWORD *used);
API_RETCODE apfchar (UCHAR * in_string_ptr, UDWORD in_length, UDWORD * moved_len_ptr, SWORD in_format,
UCHAR * out_string_ptr, UDWORD out_length, SWORD out_format, UWORD numericScale, UDWORD *used);
API_RETCODE apfnumeric (UCHAR * in_string_ptr, UDWORD in_length, UWORD in_scale, UDWORD * moved_len_ptr, SWORD in_format,
UCHAR * out_string_ptr, UDWORD out_length, SWORD out_format, UWORD numericScale, UDWORD *used);
API_RETCODE apfdate (UCHAR * in_string_ptr, UDWORD in_length, UDWORD * moved_len_ptr, SWORD in_format,
UCHAR * out_string_ptr, UDWORD out_length, SWORD out_format, UDWORD *used);
#else           
API_RETCODE apfbinary ();
API_RETCODE apfchar ();
API_RETCODE apfnumeric ();
API_RETCODE apfdate ();
VOID aptanly ();
#endif

/* Prototypes from VPA04TC */
#ifdef PROTOTYPE
API_RETCODE apthx2by (UCHAR * in_string_ptr, UDWORD in_length, SWORD in_format,
UCHAR * out_string_ptr, UDWORD out_length, SWORD out_format);
API_RETCODE aptnm2ch (UCHAR * in_string_ptr, UWORD in_format, UCHAR * out_string_ptr,
UDWORD out_length, UWORD out_scale, UWORD out_format, UDWORD *used);
API_RETCODE aptnm2chw (UCHAR * in_string_ptr, UWORD in_format, UCHAR * out_string_ptr,
UDWORD out_length, UWORD out_scale, UWORD out_format, UDWORD *used);
API_RETCODE aptchst(UCHAR *string_ptr);
VOID aptanly (UCHAR *string_ptr, UWORD *right_digits_ptr, UWORD *left_digits_ptr,
UWORD *sign_digits_ptr, UWORD *sign_indicator_ptr, SWORD *exponent_ptr);

/* use these types to check against int4 limits. These are introduced for
 * 64 bit platforms, where long has 8 bytes. PTS 1104432 */
#define API_INT4_TYPE  30000
API_RETCODE aptchsl (UCHAR * string_ptr, UWORD format);

API_RETCODE aptchfd (UCHAR * string_ptr, SWORD max_digits);
API_RETCODE aptchar (UCHAR * in_ptr, UDWORD in_length, SWORD in_format, UCHAR * out_ptr,
UDWORD out_length, UWORD out_scale, SWORD out_format, UDWORD *used);
API_RETCODE aptbinary (UCHAR * in_ptr, UDWORD in_length, SWORD in_format, UCHAR * out_ptr,
UDWORD out_length, UWORD out_scale, SWORD out_format, UDWORD *used);
API_RETCODE aptnumeric( UCHAR * in_ptr,
                        UDWORD in_length, UWORD c_scale, SWORD in_format,
                        UCHAR * out_ptr,
                        UDWORD out_length, UWORD out_scale, SWORD out_format,
                        UDWORD *used);
API_RETCODE aptdate (UCHAR * in_ptr, UDWORD in_length, SWORD in_format, UCHAR * out_ptr,
UDWORD out_length, UWORD out_scale, SWORD out_format, UDWORD *used);
API_RETCODE aptbit (UCHAR * in_ptr, UDWORD in_length, SWORD in_format, UCHAR * out_ptr,
UDWORD out_length, UWORD out_scale, SWORD out_format, UDWORD *used);
#else           
API_RETCODE aptchsl ();
API_RETCODE aptchfd ();
API_RETCODE apthx2by ();
API_RETCODE aptnm2ch ();
API_RETCODE aptnm2chw ();
API_RETCODE aptchst ();
VOID aptanly ();
API_RETCODE aptbinary ();
API_RETCODE aptnumeric ();
API_RETCODE aptchar ();
#endif

				   /* in apumsc.c <=== */
API_RETCODE apucpar(
#ifdef PROTOTYPE
tpr05_String *string, 
tpr05_String *dsnameStr,
tpr05_String *userStr,
tpr05_String *passwStr,
/*
  UCHAR * userid_ptr, SDWORD userid_max_len,
UCHAR * passwd_ptr, SDWORD passwd_max_len,
*/
UCHAR *serverdb_ptr,
SDWORD serverdb_max_len,
UCHAR *servernode_ptr,
SDWORD servernode_max_len,
UCHAR *driverNameStr,           /* PTS 1108230 */
SDWORD driverName_max_len
#endif
		      );

API_RETCODE pa08MakeConnectString (
#ifdef PROTOTYPE
                                    tpr05_String *returnStr,
                                    tpr05_String *dsname,
                                    tpr05_String *userid,
                                    tpr05_String *passwd,
                                    UCHAR *serverdb,
                                    UCHAR *servernode,
                                    UCHAR *drivername,
                                    int   browse_conn
#endif
                                    );

		
VOID aputoup(
#ifdef PROTOTYPE
UCHAR * string_ptr
#endif
	     );

VOID aputoupW(
#ifdef PROTOTYPE
tsp81_UCS2Char * string_ptr
#endif
	     );

VOID apumask(
#ifdef PROTOTYPE
UCHAR * string_ptr
#endif
	     );

UWORD apuscan(
#ifdef PROTOTYPE
UCHAR * sql_string_ptr, UCHAR * table_name_ptr
#endif
	     );

void sqcldsc (
#ifdef PROTOTYPE
sqlcatype *ca,
sqlgaentry *gae,
sqlint2 dbno,
sqldatype *da,
sqlparsid parsid
#endif
	     );
	  
void sqclsyn (
#ifdef PROTOTYPE
sqlcatype *sqlcca, sqlgaentry *ga, sqlint2 dbno,
tsp00_Int4 stmt_len, char *stmt_ptr
#endif
);

void sqcltrc (
#ifdef PROTOTYPE
     sqlcatype *ca,
     UCHAR *stmt_ptr,
     SDWORD stm_len
#endif
	     );

void PCALL p01atrace (
#ifdef PROTOTYPE
     sqlcatype *ca,
     SDWORD stm_len,
     UCHAR * stmt_ptr
#endif
	     );
SWORD pa01reconn (
#ifdef PROTOTYPE
   sqlcatype *sqlcap,
   SWORD dbno
#endif
	     );

void pa01anstm (
#ifdef PROTOTYPE
  tpr05_String *SqlStmt,
	SDWORD *order_type              
#endif
	     );
	  
API_RETCODE pa01chktabtyp( 
#ifdef PROTOTYPE
  tpr05_String *string,
  SDWORD       *tableType
#endif
);
	  
API_RETCODE pa01mktabtyp( 
#ifdef PROTOTYPE
  SDWORD       tableType, 
  tpr05_String *string
#endif
);

tsp00_Int2 pa01UtilityConnect(void);

UCHAR *pa01NextSymbol(
#ifdef PROTOTYPE
UCHAR *strp, unsigned int *len, UWORD *symb, 
UCHAR FAR* out_str, UDWORD out_length_max
#endif
);	       

SQLWCHAR *pa01NextSymbolW (
#ifdef PROTOTYPE
                           SQLWCHAR  *strp,
                           unsigned int *len,
                           UWORD     *symb,
                           SQLCHAR   *out_str,
                           UDWORD     out_length_max,
                           const tsp77encoding *encoding
#endif
                           );

/* returns dbc_block_ptr->unicode_catalog for current thread */
UWORD pa01GetUnicodeCatalog ();

/* returns sp77encodingAscii or sp77encodingUCS2Native corresponding
   to dbc_block_ptr->unicode_catalog for current thread */
const tsp77encoding * pa01GetPacketEncoding ();

#define UNREFERENCED_PARAMETER(P) (P)

/* end of apodbc.h */
    
